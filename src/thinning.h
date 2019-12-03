/*
 * thinning.h
 *
 *  Created on: Dec. 3, 2019
 *      Author: Adam Gumieniak
 */

#ifndef THINNING_H_
#define THINNING_H_

int check_condition_a(float *arr_ptr);
int check_condition_b(float *arr_ptr);
int check_neighbor_product(float *arr_ptr);
image thin_image(image grayscale_image);



#endif /* THINNING_H_ */
