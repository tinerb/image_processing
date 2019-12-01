#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "feature_vector.h"

float *get_vector(image target){
	int x[5], y[5];
	static float vector[16];
	float black = 0, white = 0;
	for (int i = 0; i < 5; i++){
		x[i] = (target.w/4) * i;
		y[i] = (target.h/4) * i;
	}
	for (int i = 0; i < 16; i++){
		for (int j = y[i/4]; j < y[i/4 + 1]; j++){
			for (int k = x[i%4]; k < x[i%4 + 1]; k++){
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

int recognize(FILE *file, float *vector){
	int result = 0;
	float sum = 0, compare = 10000, temp_vector[16], vector_sum[10];
	for (int i = 0; i < 10; i++){
		vector_sum[i] = 0;
	}
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 16; j++){
			fscanf(file, "%f", &temp_vector[j]);
			sum += abs(temp_vector[j] - vector[j]);
		}
		vector_sum[i%10] += sum;
		sum = 0;
	}
	for (int i = 0; i < 10; i++){
		vector_sum[i] = vector_sum[i]/2;
		if (vector_sum[i] < compare){
			compare = vector_sum[i];
			result = i;
		}
	}

	return result;
}


