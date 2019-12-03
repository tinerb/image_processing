#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "segmentation.h"

// returns 0 if condition is false, 1 if true
int check_condition_a(float *arr_ptr){
	int sum = 0, result;
	float *check_condition = arr_ptr;
	for(int i = 0; i < 8; i++){
		// black pixels are 0; white are 1
		// we must change for condition checking to work properly
		if(arr_ptr[i] == 0){
			arr_ptr[i] = 1;
		}
		else{
			arr_ptr[i] = 0;
		}
	}

	for(int i = 0; i < 8; i++){
		// summing all values
		sum += check_condition[i];
	}

	if(sum >= 2 && sum <= 6){
		result = 1;
	}
	else{
		result = 0;
	}


	return result;
}

// returns 0 if condition is false, 1 if true
int check_condition_b(float *arr_ptr){
	int transition = 0, result;
	float *check_condition = arr_ptr;
	for(int i = 0; i < 8; i++){
		// black pixels are 0; white are 1
		// we must change for condition checking to work properly
		if(arr_ptr[i] == 0){
			arr_ptr[i] = 1;
		}
		else{
			arr_ptr[i] = 0;
		}
	}

	// pixels 2 and 9 are beside each other; loop won't be able to check that so compare directly
	if(check_condition[0] != check_condition[7]){
		transition++;
	}

	for(int i = 0; i < 7; i++){
		if(check_condition[i] != check_condition[i + 1]){
			transition++;
		}
	}

	if(transition == 2){
		result = 1;
	}
	else{
		result = 0;
	}

	return result;
}

// returns 0 if condition is false, 1 if true
int check_neighbor_product(float *arr_ptr){
	int result = 0, is_valid = 0;

	for(int i = 0; i < 3; i++){
		// black pixels are 0; white are 1
		// we must change for condition checking to work properly
		if(arr_ptr[i] == 0){
			arr_ptr[i] = 1;
		}
		else{
			arr_ptr[i] = 0;
		}
	}

	result = arr_ptr[0] * arr_ptr[1] * arr_ptr[2];
	// if result is 0 then we have true condition; else false set to 0
	is_valid = (result == 0) ? 1 : 0;

	return is_valid;
}


image thin_image(image grayscale_image){

	// set flag as 1 to allow while loop to start first iteration
	int changed_flag = 1;
	image input_image = trivial_clamp(grayscale_image, 0.8);
	image white_image = make_image(input_image.w, input_image.h, 1);
	image temp_image;
	float current_pixel, pixel_2, pixel_3, pixel_4, pixel_5, pixel_6, pixel_7, pixel_8, pixel_9;

	// make static white image to allow resetting of temp_image
	for(int y = 0; y < (input_image.h); y++){
			for(int x = 0; x < (input_image.w); x++){
				white_image.data[y * input_image.w + x]	= 1;
		}
	}

	/*
	 * NEIGHBORHOOD OF PIXELS; CURRENT PIXEL IS PIXEL 1
	 * 9 | 2 | 3
	 * 8 | 1 | 4
	 * 7 | 6 | 5
	 */

	// thinning algorithm
	while(changed_flag != 0){
		// change it first at each iteration
		changed_flag = 0;
		temp_image = white_image;

		// first sub-iteration
		for(int y = 0; y < (input_image.h); y++){
			for(int x = 0; x < (input_image.w); x++){
				current_pixel = input_image.data[y * input_image.w + x];
				// got a black pixel; check if we can delete it
				if(current_pixel == 0){
//					printf("current_pixel == 0 : iteration 1\n");
					pixel_2 = find_pixel_conv(input_image, x, y - 1, 0); // top middle
					pixel_3 = find_pixel_conv(input_image, x + 1, y - 1, 0); // top right
					pixel_4 = find_pixel_conv(input_image, x + 1, y, 0); // middle right
					pixel_5 = find_pixel_conv(input_image, x + 1, y + 1, 0); // bottom right
					pixel_6 = find_pixel_conv(input_image, x, y + 1, 0); // bottom middle
					pixel_7 = find_pixel_conv(input_image, x - 1, y - 1, 0); // bottom left
					pixel_8 = find_pixel_conv(input_image, x - 1, y, 0); // middle left
					pixel_9 = find_pixel_conv(input_image, x - 1, y - 1, 0); // top left corner

					// ugly declaration
					float check_a[] = {pixel_2, pixel_3, pixel_4, pixel_5, pixel_6, pixel_7, pixel_8, pixel_9};
					float check_b[] = {pixel_2, pixel_3, pixel_4, pixel_5, pixel_6, pixel_7, pixel_8, pixel_9};
					float check_c[] = {pixel_2, pixel_4, pixel_6};
					float check_d[] = {pixel_4, pixel_6, pixel_8};

					int condition_a = check_condition_a(&check_a);
					int condition_b = check_condition_b(&check_b);
					int condition_c = check_neighbor_product(&check_c);
					int condition_d = check_neighbor_product(&check_d);

					// this if is wrong
					if(condition_a == 1 && condition_b == 1 && condition_c == 1 && condition_d == 1){
						temp_image.data[y * input_image.w + x] = 1;
						changed_flag++;
					}
					// conditions were not met; do not delete the pixel
					else{
						temp_image.data[y * input_image.w + x] = 0;
					}

				}
			}
		}

		// second sub-iteration
		for(int y = 0; y < (input_image.h); y++){
			for(int x = 0; x < (input_image.w); x++){
				current_pixel = input_image.data[y * input_image.w + x];
				// got a black pixel; check if we can delete it
				if(current_pixel == 0){
//					printf("current_pixel == 0 : iteration 2\n");
					pixel_2 = find_pixel_conv(input_image, x, y - 1, 0); // top middle
					pixel_3 = find_pixel_conv(input_image, x + 1, y - 1, 0); // top right
					pixel_4 = find_pixel_conv(input_image, x + 1, y, 0); // middle right
					pixel_5 = find_pixel_conv(input_image, x + 1, y + 1, 0); // bottom right
					pixel_6 = find_pixel_conv(input_image, x, y + 1, 0); // bottom middle
					pixel_7 = find_pixel_conv(input_image, x - 1, y - 1, 0); // bottom left
					pixel_8 = find_pixel_conv(input_image, x - 1, y, 0); // middle left
					pixel_9 = find_pixel_conv(input_image, x - 1, y - 1, 0); // top left corner

					// ugly declaration
					int check_a[] = {pixel_2, pixel_3, pixel_4, pixel_5, pixel_6, pixel_7, pixel_8, pixel_9};
					int check_b[] = {pixel_2, pixel_3, pixel_4, pixel_5, pixel_6, pixel_7, pixel_8, pixel_9};
					int check_c[] = {pixel_2, pixel_4, pixel_6};
					int check_d[] = {pixel_2, pixel_6, pixel_8};

					int condition_a = check_condition_a(&check_a);
					int condition_b = check_condition_b(&check_b);
					int condition_c = check_neighbor_product(&check_c);
					int condition_d = check_neighbor_product(&check_d);

					printf("condition_a: %d    condition_b: %d     condition_c: %d    condition_d: %d\n", condition_a, condition_b, condition_c, condition_d);

					// if all conditions are satisfied we delete
					if(condition_a == 1 && condition_b == 1 && condition_c == 1 && condition_d == 1){
						temp_image.data[y * input_image.w + x] = 1;
						changed_flag++;
						printf("changed_flag: %d\n", changed_flag);
					}
					// conditions were not met; do not delete the pixel
					else{
						temp_image.data[y * input_image.w + x] = 0;
					}

				}

			}
		}
//		printf("changed_flag: %d\n", changed_flag);
		input_image = temp_image;
	}

	return input_image;
}
