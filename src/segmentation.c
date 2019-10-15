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

void display_backward(node *tail) {
	node *np;
	for (np = tail; np != NULL; np = np->prev){
		printf("%.2f, %.2f\n", np->data, np->equivalent);
	}
}


// returns the node that we want to change
node* search(node *tail, int num) {
	while ((tail != NULL) && (tail->data != num)) {
		tail = tail->prev;
	}
	return tail;
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
 * returns result
*/
image label_image(image filtered_image){
	image result = make_image(filtered_image.w, filtered_image.h, 1);

	float threshold = histogram_threshold(filtered_image);
	for (int i = 0; i < result.w * result.h; i++){
		if (filtered_image.data[i] < threshold){
			result.data[i] = 1;
		}
		else{
			result.data[i] = 0;
		}
	}
	return result;
}

/*
 * does raster scan and assigns segment labels to foreground
 *
 * returns labeled_image
 */
image raster_scan(image filtered_image){
	// our current equivalence label number
	int current_equivalence = 1;
	int top_pixel, left_pixel, current_pixel, top_left_pixel;
	image labeled_image = label_image(filtered_image);

	for(int y = 0; y < (labeled_image.h); y++){
		for(int x = 0; x < (labeled_image.w); x++){
			// sets current pixel
			current_pixel = labeled_image.data[y * labeled_image.w + x]; // this might be very wrong; too tired to do math rn
			// found foreground
			if(current_pixel == -1){
				// looks to the needed adjacent pixel to see if they're already labeled
				top_pixel = find_pixel_conv(labeled_image, x, y - 1, 0); // top middle
				left_pixel = find_pixel_conv(labeled_image, x - 1, y, 0); // middle left
				top_left_pixel = find_pixel_conv(labeled_image, x - 1, y - 1, 0);

				// SOMEWHERE IN THIS LOGIC WE HAVE TO ADD EQUIVALENCE LIST STUFF

				// if both VIP pixels are labeled, choose smallest value
				// keeps out equivalence list consistent
				// this might be wrong; might only want top pixel
				if(top_pixel > 0 && left_pixel > 0){
					if(top_pixel < left_pixel){
						current_pixel = top_pixel;
					} else{
						current_pixel = left_pixel;
					}
				}

				else if(top_pixel > 0){
					current_pixel = top_pixel;
				}
				else if(left_pixel > 0){
					current_pixel = left_pixel;
				}
				else if(top_left_pixel > 0){
					current_pixel = top_left_pixel;
				}
				// found new region; top and left pixels are not foreground
				else{
					current_pixel = current_equivalence;
					if(current_equivalence == 1){ // this IF might be redundant
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

	display_forward(head);

	return labeled_image;
}

/*
 * go through equivalence list against the labeled image and
 * update any connected regions
 *
 * returns segmented_image
 */
image apply_equivalence_list(image labeled_image, int equivalence_list){
	// just temporary to get rid of stupid warning
	image segmented_image = make_image(labeled_image.w, labeled_image.h, 1);

	return segmented_image;
}

/*
 * we use this function in the main .c file
 *
 * does raster scan and assigns segment labels to foreground
 *
 * returns segmented_image (labeled_image)
 */
image segment_image(image filtered_image){

	image labeled_image = raster_scan(filtered_image);
	// CHANGE labeled_image WITH EQUIVALENCE LIST STUFF
	//image segmented_image = apply_equivalence_list(labeled_image, equivalence_list);

	return labeled_image;
}
