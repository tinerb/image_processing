#include <stdio.h>
#include "segmentation.h"
#include "image.h"

// make this global so we can use it with different functions
equivalence_list *list = NULL;

/*
 * goes through our target image and labels each pixel with
 * foreground(black pixels) as -1 and background(white pixels)
 * as 0
 *
 * returns result
*/
image label_image(image filtered_image){
	image result = make_image(filtered_image.w, filtered_image.h, 1);
	for (int i = 0; i < result.w * result.h; i++){
		if (filtered_image.data[i] < 0.05){
			result.data[i] = -1;
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
	equivalence_list *current;

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
					if(current_equivalence == 1){
						list->equivalent = current_equivalence;
						list->value = current_equivalence;
						current = list;
					}
					else{
						equivalence_list *new_node;
						new_node->equivalent = current_equivalence;
						new_node->value = current_equivalence;
						current->next = new_node;
						current = current->next;
					}
					current_equivalence++;
				}

			}
			// sets the labeled image to the region label value
			labeled_image.data[y * labeled_image.w + x] = current_pixel;
		}
	}

	print_list(&current);

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

void print_list(equivalence_list *list){
	while(list->next != NULL){
		printf("value: %d		equivalent: %d", list->value);
	}
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
