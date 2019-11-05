#include <stdlib.h>
#include <stdio.h>
#include "image.h"

// DONT USE INTS USE UNSIGNED CHAR'S

void print_hist(int hist[]) {

	for(int i = 0; i < 256; i++){
		printf("hist[%d] = %u\n", i, hist[i]);
	}
}

unsigned char get_threshold(int hist[], int T){

	// means for our two groups
	int mean_1 = 0, mean_2 = 0;
	// used for our mean division; counts how many numbers in each group
	int mean_1_amount = 0, mean_2_amount = 0, prev_mean_1_amount = 0, prev_mean_2_amount = 0;
	float average_1, average_2;
	// flag for when to stop threshold calculations
	int flag = 0;

	while(flag == 0){
		for(int i = 0; i < 256; i++){
			if(hist[i] > T){
				mean_1 += hist[i];
				mean_1_amount++;
			} else {
				mean_2 += hist[i];
				mean_2_amount++;
			}
		}

		// checks if there was a change in amount from last iter; if not then we are done
		if(mean_1_amount == prev_mean_1_amount || mean_2_amount == prev_mean_2_amount){
			flag = 1;
		}

		// math to find new threshold
		prev_mean_1_amount = mean_1_amount;
		prev_mean_2_amount = mean_2_amount;
		average_1 = mean_1 / mean_1_amount;
		average_2 = mean_2 / mean_2_amount;
		float temp = (average_1 + average_2) / 2;
		T = (int) temp;

		// reset for next iteration
		mean_1 = 0;
		mean_1_amount = 0;
		mean_2 = 0;
		mean_2_amount = 0;
	}

	return (unsigned char) prev_mean_2_amount; // unsigned char for further calculations
}

// gets the threshold value from our image
float histogram_threshold(image im){
	int hist[255];
	unsigned char threshold = 0;
	int T = 128; // our initial threshold value
	float return_threshold, t;
	// create our empty histogram
	for(int i = 0; i < 256; i++){
		hist[i] = 0;
	}

	for (int i = 0; i < im.w * im.h; i++){
		// change it to range 0..255
		unsigned char pixel_hist_value = im.data[i] * 255;
		int n = (int) pixel_hist_value; // this casting makes it work
		hist[n] += 1;
	}
	threshold = get_threshold(hist, T);
	return_threshold = (float)threshold/255.;
	t = return_threshold;
//	printf("t: %f\n", t);
	return t;
}
