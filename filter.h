#include "image.h"

#ifndef CP467_A1_COMPUTER_VISION_SRC_FILTER_H_
#define CP467_A1_COMPUTER_VISION_SRC_FILTER_H_

float find_pixel_conv(image im, int x, int y, int c);
image make_box_filter(int w);
image high_pass_filter(image target);
image sobel_filter(image target);
image blur_filter(image target);
image outline_filter(image target);
image sharpen_filter(image target);
image emboss_filter(image target);
image rotate_filter(image filter);
image apply_filter(image target, image filter);


#endif /* CP467_A1_COMPUTER_VISION_SRC_FILTER_H_ */
