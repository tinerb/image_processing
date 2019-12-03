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
#include "histogram.h"
#include "scaling.h"
#include "segmentation_list_utilities.h"

image label_image(image filtered_image);
image apply_equivalence_list(image labeled_image, int equivalence_list);
image raster_scan(image filtered_image, int is_binary);
image scale_segments(image filtered_image, int is_binary);
image write_entire_image(image filtered_image, int is_binary);
image apply_equivalence_list(image labeled_image, int equivalence_list);
image segment_image(image filtered_image, int is_binary);



#endif /* IMAGE_PROCESSING_SRC_SEGMENTATION_H_ */
