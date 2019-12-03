#include <stdio.h>
#include <stdlib.h>
#include "segmentation_list_utilities.h"
#include "segmentation.h"
#include "image.h"
#include "scaling.h"

// make this global so we can use it with different functions
node *head=NULL, *tail=NULL;
segment_data *seg_head = NULL, *seg_tail = NULL;

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
	int seg_temp;
	int seg_num;
	float node_data, node_eq; // used for when we segment_list
	float top_pixel, left_pixel, current_pixel, top_left_pixel;
	image labeled_image;
	image temp_image;
	node *temp = NULL;
	segment_data *temp_seg;

	if(is_binary == 1){
		labeled_image = label_image(filtered_image);
	}
	else{
		// binary images still have strange values which are not 0 or 1
		// clamp using some arbitrary value to enforce binary pixels
		// change threshold depending on image you use
		labeled_image = trivial_clamp(filtered_image, 0.8); // change threshold value depending on image you use
	}

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
				} // BUG SOMEWHERE HERE?
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

	// ---------------------
	// CORRECTING EQUIVALENCE LIST TO GET NUMBER OF REGIONS
	// ---------------------

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

	// ---------------------
	// GET IMAGE SEGMENTS
	// ---------------------

	for(int y = 0; y < (labeled_image.h); y++){
			for(int x = 0; x < (labeled_image.w); x++){
				current_pixel = labeled_image.data[y * labeled_image.w + x];

				// apply equivalence list to labeled_image updating region values
				temp = search(head, current_pixel);
				if(temp != NULL){
					node_eq = temp->equivalent;
					float updated_value = get_eq_value_at_index(head, node_eq);
					labeled_image.data[y * labeled_image.w + x] = updated_value;
					current_pixel = updated_value;
				}

				if(current_pixel > 1){
					seg_temp = is_seg_unique(seg_head, current_pixel);
					// this segment value is already in list; update any values which are valid to update
					if(seg_temp == 1){
						temp_seg = search_seg(seg_head, current_pixel);
						// update any max/min values
						if(x > temp_seg->max_x){
							temp_seg->max_x = x;
						}
						if(y > temp_seg->max_y){
							temp_seg->max_y = y;
						}
						if(x < temp_seg->min_x){
							temp_seg->min_x = x;
						}
						if(y < temp_seg->min_y){
							temp_seg->min_y = y;
						}
					}
					// segment value not in list; add a new seg_node to list
					else{
						// seg_node initialization will have same max and min (x,y) values
						segment_data *n = seg_node(current_pixel, x, y, x, y);
						seg_insert_end(&seg_head, &seg_tail, n);
					}
				}
			}
	}

	// ---------------------
	// MAKE EACH SEGMENT INTO THEIR OWN IMAGE
	// ---------------------


	int regions = get_regions(head);
	printf("There are %d segments.\n", regions);
	printf("Which segment would you like to recognize?\n");
	fflush(stdout);
	scanf("%d", &seg_num);

	temp_seg = get_seg_value_at_index(seg_head, seg_num - 1);

	int black_pixels = 0;
	int white_pixels = 0;

	int temp_w = temp_seg->max_x - temp_seg->min_x;
	int temp_h = temp_seg->max_y - temp_seg->min_y;
	int temp_ny = 0; // for new image
	int temp_nx = 0; // for new image
	temp_image = make_image(temp_w, temp_h, 1);
	for(int y = temp_seg->min_y; y < temp_seg->max_y; y++){
		for(int x = temp_seg->min_x; x < temp_seg->max_x; x++){
			current_pixel = labeled_image.data[y * labeled_image.w + x];
			if(current_pixel == temp_seg->seg_value){
				temp_image.data[temp_ny * temp_w + temp_nx] = 0;
				black_pixels++;
			}
			else{
				temp_image.data[temp_ny * temp_w + temp_nx] = 1;
				white_pixels++;
			}
			temp_nx++;
		}
		temp_ny++;
		temp_nx = 0;
	}

	printf("Region %d has %d white pixels and %d black pixels.\n", seg_num, white_pixels, black_pixels);

	// ---------------------


	// memory clean
	clean(&head);
	seg_clean(&seg_head);
	return temp_image;
}


image scale_segments(image filtered_image, int is_binary){
	// our current equivalence label number; start at 2 so we dont change pixel values of 1 (white)
	int current_equivalence = 2;
	int list_len;
	int seg_temp;
	int seg_num;
	float node_data, node_eq; // used for when we segment_list
	float top_pixel, left_pixel, current_pixel, top_left_pixel;
	image labeled_image;
	image temp_image;
	node *temp = NULL;
	segment_data *temp_seg;

	if(is_binary == 1){
		labeled_image = label_image(filtered_image);
	}
	else{
		// binary images still have strange values which are not 0 or 1
		// clamp using some arbitrary value to enforce binary pixels
		// change threshold depending on image you use
		labeled_image = trivial_clamp(filtered_image, 0.8); // change threshold value depending on image you use
	}

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
				} // BUG SOMEWHERE HERE?
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

	// ---------------------
	// CORRECTING EQUIVALENCE LIST TO GET NUMBER OF REGIONS
	// ---------------------

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

	// ---------------------
	// GET IMAGE SEGMENTS
	// ---------------------

	for(int y = 0; y < (labeled_image.h); y++){
			for(int x = 0; x < (labeled_image.w); x++){
				current_pixel = labeled_image.data[y * labeled_image.w + x];

				// apply equivalence list to labeled_image updating region values
				temp = search(head, current_pixel);
				if(temp != NULL){
					node_eq = temp->equivalent;
					float updated_value = get_eq_value_at_index(head, node_eq);
					labeled_image.data[y * labeled_image.w + x] = updated_value;
					current_pixel = updated_value;
				}

				if(current_pixel > 1){
					seg_temp = is_seg_unique(seg_head, current_pixel);
					// this segment value is already in list; update any values which are valid to update
					if(seg_temp == 1){
						temp_seg = search_seg(seg_head, current_pixel);
						// update any max/min values
						if(x > temp_seg->max_x){
							temp_seg->max_x = x;
						}
						if(y > temp_seg->max_y){
							temp_seg->max_y = y;
						}
						if(x < temp_seg->min_x){
							temp_seg->min_x = x;
						}
						if(y < temp_seg->min_y){
							temp_seg->min_y = y;
						}
					}
					// segment value not in list; add a new seg_node to list
					else{
						// seg_node initialization will have same max and min (x,y) values
						segment_data *n = seg_node(current_pixel, x, y, x, y);
						seg_insert_end(&seg_head, &seg_tail, n);
					}
				}
			}
	}

	// ---------------------
	// MAKE EACH SEGMENT INTO THEIR OWN IMAGE
	// ---------------------


	image scaled_image = make_image(labeled_image.w, labeled_image.h, 1);
	// make entire new image white
	for(int y = 0; y < (labeled_image.h); y++){
				for(int x = 0; x < (labeled_image.w); x++){
					scaled_image.data[y * labeled_image.w + x] = 1;
				}
	}


	int regions = get_regions(head);
	printf("regions: %d\n", regions);
	for(int i = 0; i < regions; i++){
		temp_seg = get_seg_value_at_index(seg_head, i);
		int temp_w = temp_seg->max_x - temp_seg->min_x;
		int temp_h = temp_seg->max_y - temp_seg->min_y;
		int temp_ny = 0; // for new image
		int temp_nx = 0; // for new image
		temp_image = make_image(temp_w, temp_h, 1);
		for(int y = temp_seg->min_y; y < temp_seg->max_y; y++){
			for(int x = temp_seg->min_x; x < temp_seg->max_x; x++){
				current_pixel = labeled_image.data[y * labeled_image.w + x];
				if(current_pixel == temp_seg->seg_value){
					temp_image.data[temp_ny * temp_w + temp_nx] = 0;
				}
				else{
					temp_image.data[temp_ny * temp_w + temp_nx] = 1;
				}
				temp_nx++;
			}
			temp_ny++;
			temp_nx = 0;
		}

		// we now have one segment in the temp_image
		image scaled_segment = scale(temp_image);

		temp_ny = 0; // for new image
		temp_nx = 0;

		for(int y = temp_seg->min_y; y < temp_seg->max_y - (temp_h / 2) - 1; y++){
			for(int x = temp_seg->min_x; x < temp_seg->max_x - (temp_w / 2) - 1; x++){
				current_pixel = scaled_segment.data[temp_ny * scaled_segment.w + temp_nx];
				if(current_pixel == 0){
					scaled_image.data[y * scaled_image.w + x] = current_pixel;
				}
				else{
					scaled_image.data[y * scaled_image.w + x] = 1;
				}

				temp_nx++;
			}
			temp_ny++;
			temp_nx = 0;
		}

	}



	// ---------------------


	// memory clean
	clean(&head);
	seg_clean(&seg_head);
	return scaled_image;
}

image write_entire_image(image filtered_image, int is_binary){
	// our current equivalence label number; start at 2 so we dont change pixel values of 1 (white)
	int current_equivalence = 2;
	int list_len;
	int seg_temp;
	int seg_num;
	float node_data, node_eq; // used for when we segment_list
	float top_pixel, left_pixel, current_pixel, top_left_pixel;
	image labeled_image;
	image temp_image;
	node *temp = NULL;
	segment_data *temp_seg;

	if(is_binary == 1){
		labeled_image = label_image(filtered_image);
	}
	else{
		// binary images still have strange values which are not 0 or 1
		// clamp using some arbitrary value to enforce binary pixels
		// change threshold depending on image you use
		labeled_image = trivial_clamp(filtered_image, 0.8); // change threshold value depending on image you use
	}

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
				} // BUG SOMEWHERE HERE?
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

	// ---------------------
	// CORRECTING EQUIVALENCE LIST TO GET NUMBER OF REGIONS
	// ---------------------

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

	// ---------------------
	// GET IMAGE SEGMENTS
	// ---------------------

	for(int y = 0; y < (labeled_image.h); y++){
			for(int x = 0; x < (labeled_image.w); x++){
				current_pixel = labeled_image.data[y * labeled_image.w + x];

				// apply equivalence list to labeled_image updating region values
				temp = search(head, current_pixel);
				if(temp != NULL){
					node_eq = temp->equivalent;
					float updated_value = get_eq_value_at_index(head, node_eq);
					labeled_image.data[y * labeled_image.w + x] = updated_value;
					current_pixel = updated_value;
				}

				if(current_pixel > 1){
					seg_temp = is_seg_unique(seg_head, current_pixel);
					// this segment value is already in list; update any values which are valid to update
					if(seg_temp == 1){
						temp_seg = search_seg(seg_head, current_pixel);
						// update any max/min values
						if(x > temp_seg->max_x){
							temp_seg->max_x = x;
						}
						if(y > temp_seg->max_y){
							temp_seg->max_y = y;
						}
						if(x < temp_seg->min_x){
							temp_seg->min_x = x;
						}
						if(y < temp_seg->min_y){
							temp_seg->min_y = y;
						}
					}
					// segment value not in list; add a new seg_node to list
					else{
						// seg_node initialization will have same max and min (x,y) values
						segment_data *n = seg_node(current_pixel, x, y, x, y);
						seg_insert_end(&seg_head, &seg_tail, n);
					}
				}
			}
	}

	// ---------------------
	// MAKE EACH SEGMENT INTO THEIR OWN IMAGE
	// ---------------------


	int regions = get_regions(head);
	printf("There are %d segments.\n", regions);
//	printf("Which segment would you like to recognize?\n");
//	fflush(stdout);
//	scanf("%d", &seg_num);

	for(int i = 0; i < regions; i++){
		temp_seg = get_seg_value_at_index(seg_head, i);
		int temp_w = temp_seg->max_x - temp_seg->min_x;
		int temp_h = temp_seg->max_y - temp_seg->min_y;
		int temp_ny = 0; // for new image
		int temp_nx = 0; // for new image
		temp_image = make_image(temp_w, temp_h, 1);
		for(int y = temp_seg->min_y; y < temp_seg->max_y; y++){
			for(int x = temp_seg->min_x; x < temp_seg->max_x; x++){
				current_pixel = labeled_image.data[y * labeled_image.w + x];
				if(current_pixel == temp_seg->seg_value){
					temp_image.data[temp_ny * temp_w + temp_nx] = 0;
				}
				else{
					temp_image.data[temp_ny * temp_w + temp_nx] = 1;
				}
				temp_nx++;
			}
			temp_ny++;
			temp_nx = 0;
		}

		// ---------------------
		// WRITE ENTIRE SEGMENT INTO A FEATURE VECTOR AND PUT INTO A .TXT FILE
		// ---------------------

		float *vector = malloc(sizeof(float) * 16);
		FILE *fptr;
		vector = get_vector(temp_image);
		printf("Printing into text file at index: %d", i);
		fptr = fopen("vector.txt", "a");
		for (int i = 0; i < 16; i++){
			fprintf(fptr, "%f ", vector[i]);
		}
		fprintf(fptr, "\n");


		fclose(fptr);
   }


	// ---------------------


	// memory clean
	clean(&head);
	seg_clean(&seg_head);
	return temp_image;
}



/*
 * we use this function in the main .c file
 *
 * does raster scan and assigns segment labels to foreground
 *
 * returns segmented_image (labeled_image); we might also want to return an array of images with each char in an image
 */
image segment_image(image filtered_image, int is_binary){

	image labeled_image;
	labeled_image = raster_scan(filtered_image, is_binary);


	return labeled_image;
}
