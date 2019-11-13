#include <stdio.h>
#include <stdlib.h>
#include "segmentation.h"
#include "image.h"

// make this global so we can use it with different functions
node *head=NULL, *tail=NULL;

node *new_node(float data, float equivalent) {
	node *np = (node*) malloc(sizeof(node));
	np->data = data;
	np->equivalent = equivalent;
	np->prev = NULL;
	np->next = NULL;
	return np;
}

void insert_end(node **headp, node **tailp, node *new_np) {
	 if (*headp == NULL){
		 *headp = new_np;
		 *tailp = new_np;
	 }
	 else{
		 (*tailp)->next = new_np;
		 new_np ->prev = *tailp;
		 *tailp = new_np;
	 }
}

void display_forward(node *head) {
	node *np;
	for (np = head; np != NULL; np = np->next){
		printf("%.2f, %.2f\n", np->data, np->equivalent);
	}
}

int len(node *head) {
	int len = 0;
	node *np;
	for (np = head; np != NULL; np = np->next){
		len++;
	}

	return len;
}

float get_eq_value_at_index(node *head, float num){
	while ((head != NULL) && (head->data != num)) {
		head = head->next;

	}

	return head->equivalent;
}

// returns the node that we want to change
node* search(node *head, int num) {
	while ((head != NULL) && (head->data != num)) {
		head = head->next;
	}

	return head;
}

// when we find a pixel with set top and left values
// we set that pixel to its lowest value
void update_list(node *tail, int num, int equivalent) {
	while ((tail != NULL) && (tail->data != num)) {
		tail = tail->prev;
	}

	tail->equivalent = equivalent;
	return;
}

int is_unique(node *head, int num){
	int unique = 0;
	while (head != NULL) {
		if(head->equivalent == num){
			// number is not unique
			unique = 1;
		}
		head = head->next;
	}

	return unique;
}


// LOOPING THROUGH ONCE IS NOT ENOUGH; MAKE IT SO THAT WE KNOW FOR SURE EACH VALUE HAS BEEN CHANGED
int get_regions(node *temp){
	int regions = 1;
	node *temp_head=NULL, *temp_tail=NULL;
	node *n = new_node(temp->data, temp->equivalent);
	insert_end(&temp_head, &temp_tail, n); // insert to start the list which avoids errors when we search for unique
	temp = temp->next;
	while (temp != NULL) {
		if(is_unique(temp_head, temp->equivalent) == 0){
			node *n = new_node(temp->data, temp->equivalent);
			insert_end(&temp_head, &temp_tail, n);
			printf("temp->data = %f -- temp->equivalent = %f\n",temp->data, temp->equivalent);
			regions++;
		}
		temp = temp->next;
	}

	return regions;
}

void clean(node **headp) {
	node** np = headp;
	node** temp;

	while ((*np) -> next != NULL){
		temp = np;
		free(temp);
		(*np) = (*np) -> next;
	}
}

/*
 * goes through our target image and labels each pixel with
 * foreground(black pixels) as -1 and background(white pixels)
 * as 0
 *
 * returns (image) result
*/
image label_image(image filtered_image){
	image result = make_image(filtered_image.w, filtered_image.h, 1);
	int black = 0, white = 0;
	float threshold = histogram_threshold(filtered_image);
	for (int i = 0; i < result.w * result.h; i++){
		if (filtered_image.data[i] < threshold){
			result.data[i] = 0; // black pixel
			white++;
		}
		else{
			result.data[i] = 1; // white pixel
			black++;
		}
	}

	return result;
}


// binary image still has unseen
image trivial_clamp(image filtered_image, float threshold){
	image result = make_image(filtered_image.w, filtered_image.h, 1);
	int black = 0, white = 0;
	for (int i = 0; i < result.w * result.h; i++){
		if (filtered_image.data[i] < threshold){
			result.data[i] = 0; // black pixel
			white++;
		}
		else{
			result.data[i] = 1; // white pixel
			black++;
		}
	}

	return result;
}

/*
 * does raster scan and assigns segment labels to foreground
 * int is_binary - 0 if it is binary, 1 if it is grayscale
 *
 * returns labeled_image
 */
image raster_scan(image filtered_image, int is_binary){
	// our current equivalence label number; start at 2 so we dont change pixel values of 1 (white)
	int current_equivalence = 2;
	int list_len;
	float node_data, node_eq; // used for when we correct
	float top_pixel, left_pixel, current_pixel, top_left_pixel;
	image labeled_image;
	node *temp = NULL;

	if(is_binary == 1){
		labeled_image = label_image(filtered_image);
	}
	else{
		// binary images still have strange values which are not 0 or 1
		// clamp using some arbitrary value to enforce binary pixels
		// change threshold depending on image you use
		labeled_image = trivial_clamp(filtered_image, 0.9); // change threshold value depending on image you use
	}

//	return labeled_image;

	for(int y = 0; y < (labeled_image.h); y++){
		for(int x = 0; x < (labeled_image.w); x++){
			// sets current pixel
			current_pixel = labeled_image.data[y * labeled_image.w + x];
			// found foreground
			if(current_pixel == 0){
				// looks to the needed adjacent pixel to see if they're already labeled
				top_pixel = find_pixel_conv(labeled_image, x, y - 1, 0); // top middle
				left_pixel = find_pixel_conv(labeled_image, x - 1, y, 0); // middle left
				top_left_pixel = find_pixel_conv(labeled_image, x - 1, y - 1, 0); // top left

				if(top_pixel > 1 && left_pixel > 1 && top_pixel != left_pixel){
					if(top_pixel < left_pixel){
						current_pixel = top_pixel;
						// change left pixel to top pixel equivalence
						update_list(tail, left_pixel, top_pixel);
					} else{
						current_pixel = left_pixel;
						// change top pixel to left pixel equivalence
						update_list(tail, top_pixel, left_pixel);
					}
				}

				else if(top_pixel > 1){
					current_pixel = top_pixel;
				}
				else if(left_pixel > 1){
					current_pixel = left_pixel;
				}
				else if(top_left_pixel > 1){
					current_pixel = top_left_pixel;
				}
				// found new region; top and left pixels are not foreground
				else{
					current_pixel = current_equivalence;
					if(current_equivalence == 2){ // this IF might be redundant
						node *n = new_node(current_equivalence, current_equivalence);
						insert_end(&head, &tail, n);
					}
					else{
						node *n = new_node(current_equivalence, current_equivalence);
						insert_end(&head, &tail, n);
					}
					current_equivalence++;
				}

			}
			// sets the labeled image to the region label value
			labeled_image.data[y * labeled_image.w + x] = current_pixel;
		}
	}

	list_len = len(head);
	int s = 2;  // since eq list started at 2, we have to look for 2 + i

	// correct the equivalence list
	for(int i = 0; i < list_len; i++){
		temp = search(head, s);
		s++;
		node_data = temp->data;
		node_eq = temp->equivalent;
		if(node_data != node_eq){
			float updated_value = get_eq_value_at_index(head, node_eq);
			update_list(tail, node_data, updated_value);
		}
	}

	int regions = get_regions(head);
	printf("REGIONS = %d\n", regions);
//	display_forward(head);
	clean(&head);
	// MAKE ALL REGIONS BLACK AGAIN; OUTPUT RN IS WEIRD
	return labeled_image;
}

/*
 * we use this function in the main .c file
 *
 * does raster scan and assigns segment labels to foreground
 *
 * returns segmented_image (labeled_image); we might also want to return an array of images with each char in an image
 */
image segment_image(image filtered_image, int is_binary){

	image labeled_image = raster_scan(filtered_image, is_binary);
	// CHANGE labeled_image WITH EQUIVALENCE LIST STUFF
	//image segmented_image = apply_equivalence_list(labeled_image, equivalence_list);

	return labeled_image;
}
