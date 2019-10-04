/*
 * segmentation.h
 *
 *  Created on: Oct. 1, 2019
 *      Author: Adam Gumieniak
 */

#ifndef IMAGE_PROCESSING_SRC_SEGMENTATION_H_
#define IMAGE_PROCESSING_SRC_SEGMENTATION_H_

#include "image.h"
#include "filter.h"


struct node {
	int value;
	int equivalent;
	struct node *next;
};



image label_image(image filtered_image);
image apply_equivalence_list(image labeled_image, int equivalence_list);
image raster_scan(image filtered_image);
image apply_equivalence_list(image labeled_image, int equivalence_list);
image segment_image(image filtered_image);



#endif /* IMAGE_PROCESSING_SRC_SEGMENTATION_H_ */
