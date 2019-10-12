/*
 * segmentation.h
 *
 *  Created on: Oct. 1, 2019
 *      Author: Adam Gumieniak
 */

#ifndef IMAGE_PROCESSING_SRC_SEGMENTATION_H_
#define IMAGE_PROCESSING_SRC_SEGMENTATION_H_

typedef struct node {
	float data;
	float equivalent;
	struct node *next;
	struct node *prev;
} node;

#include "image.h"
#include "filter.h"

node *new_node(float data, float equivalent);
void insert_end(node **headp, node **tailp, node *new_np);
void display_forward(node *head);
void display_backward(node *tail);
void clean(node **headp);
node* search(node *tail, int num);
image label_image(image filtered_image);
image apply_equivalence_list(image labeled_image, int equivalence_list);
image raster_scan(image filtered_image);
image apply_equivalence_list(image labeled_image, int equivalence_list);
image segment_image(image filtered_image);



#endif /* IMAGE_PROCESSING_SRC_SEGMENTATION_H_ */
