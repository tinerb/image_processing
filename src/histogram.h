/*
 * histogram.h
 *
 *  Created on: Oct. 14, 2019
 *      Author: Adam Gumieniak
 */

#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include "image.h"
#include "filter.h"

void print_hist(int hist[]);
float histogram_threshold(image im);
unsigned char get_threshold(int hist[], int T);


#endif /* HISTOGRAM_H_ */
