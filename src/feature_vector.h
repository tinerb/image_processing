/*
 * feature_vector.h
 *
 *  Created on: 2019 M11 12
 *      Author: Brian
 */

#ifndef SRC_FEATURE_VECTOR_H_
#define SRC_FEATURE_VECTOR_H_

#include "image.h"

float *get_vector(image target);
int recognize(FILE *test, float *vectors);


#endif /* SRC_FEATURE_VECTOR_H_ */
