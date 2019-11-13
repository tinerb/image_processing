#include <stdio.h>
#include <stdlib.h>
#include "feature_vector.h"

float *get_vector(image target){
	int x[4], y[4];
	static float vector[9];
	float black = 0, white = 0;
	for (int i = 0; i < 4; i++){
		x[i] = (target.w/3) * i;
		y[i] = (target.h/3) * i;
	}
	for (int i = 0; i < 9; i++){
		for (int j = y[i/3]; j < y[i/3 + 1]; j++){
			for (int k = x[i%3]; k < x[i%3 + 1]; k++){
				if (target.data[(target.w * j) + k] >= 1){
					white++;
				}
				else{
					black++;
				}
			}
		}
		if (white > 0){
			vector[i] = black/white;
		}
		else{
			vector[i] = black;
		}
		black = 0;
		white = 0;
	}
	return vector;
}
