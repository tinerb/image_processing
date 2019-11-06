#include <stdio.h>
#include "filter.h"
#include "image.h"

float find_pixel_conv(image im, int x, int y, int c){

	int pixel;

	if(x >= im.w || x < 0 || y >= im.h || y < 0){
		return 0;
	} else {
		pixel = (im.w * y + x);
	}

	return im.data[pixel];
}

image make_box_filter(int w)
{
    // creates an image of size w * w
	// this will be our filter
    return make_image(w,w,1);
}

image high_pass_filter(image target){

	// fill filter with sobel operator values
	target.data[0] = 0; target.data[1] = -1; target.data[2] = 0;
	target.data[3] = -1; target.data[4] = 4; target.data[5] = -1;
	target.data[6] = 0; target.data[7] = -1; target.data[8] = 0;
	return target;
}

image sobel_filter(image target){

	// fill filter with sobel operator values
	target.data[0] = -1; target.data[1] = 0; target.data[2] = 1;
	target.data[3] = -2; target.data[4] = 0; target.data[5] = 2;
	target.data[6] = -1; target.data[7] = 0; target.data[8] = 1;
	return target;
}

image blur_filter(image target){

	// fill filter with blur operator values
	target.data[0] = 1; target.data[1] = 1; target.data[2] = 1;
	target.data[3] = 1; target.data[4] = 1; target.data[5] = 1;
	target.data[6] = 1; target.data[7] = 1; target.data[8] = 1;
	return target;
}


image outline_filter(image target){

	// fill filter with outline operator values
	target.data[0] = -1; target.data[1] = -1; target.data[2] = -1;
	target.data[3] = -1; target.data[4] = 8; target.data[5] = -1;
	target.data[6] = -1; target.data[7] = -1; target.data[8] = -1;
	return target;
}


image sharpen_filter(image target){

	// fill filter with sharpen operator values
	target.data[0] = 0; target.data[1] = -1; target.data[2] = 0;
	target.data[3] = -1; target.data[4] = 5; target.data[5] = -1;
	target.data[6] = 0; target.data[7] = -1; target.data[8] = 0;
	return target;
}

image emboss_filter(image target){

	// fill filter with emboss operator values
	target.data[0] = -2; target.data[1] = -1; target.data[2] = 0;
	target.data[3] = -1; target.data[4] = 1; target.data[5] = 1;
	target.data[6] = 0; target.data[7] = 1; target.data[8] = 2;
	return target;
}


image rotate_filter(image filter){
	int size = filter.w * filter.h;
	int counter = size - 1;
	image rotated = make_image(filter.w, filter.h, 1);
	for(int i = 0; i < size; i++){
		rotated.data[i] = filter.data[counter];
		counter--;
	}
	return rotated;
}


// only for 3 by 3
// prints as the middle pixel in the centre, where x and y are
void print_target_matrix(image target, int x, int y){

	printf("%f ",find_pixel_conv(target, x - 2, y - 2, 0)); // top left corner
	printf("%f ",find_pixel_conv(target, x - 1, y - 2, 0));// top middle
	printf("%f\n",find_pixel_conv(target, x, y - 2, 0)); // top right
	printf("%f ",find_pixel_conv(target, x - 2, y - 1, 0)); // middle left
	printf("%f ",find_pixel_conv(target, x - 1, y - 1, 0)); // middle
	printf("%f\n",find_pixel_conv(target, x, y - 1, 0)); // middle right
	printf("%f ",find_pixel_conv(target, x - 2, y, 0)); // bottom left
	printf("%f ",find_pixel_conv(target, x - 1, y, 0)); // bottom middle
	printf("%f\n",find_pixel_conv(target, x, y, 0));

}

int if_blur(image filter){
	if (filter.data[0] == 1 && filter.data[1] == 1 && filter.data[2] == 1 && filter.data[3] == 1 && filter.data[4] == 1 && filter.data[5] == 1 && filter.data[6] == 1 && filter.data[7] == 1 && filter.data[8] == 1){
		return 1;
	}
	else{
		return 0;
	}
}

image apply_filter(image target, image filter){
	// why are we adding the widths and heights together before sending it through
//	image result = make_image(target.w + filter.w - 1, target.h + filter.h - 1, 1);
	printf("target.w: %d     target.h: %d\n", target.w, target.h);
	fflush(stdout);
	image result = make_image(target.w, target.h, 1);

	printf("we made it here\n");
	fflush(stdout);

	// rotate image before doing convolution
	image rotated = rotate_filter(filter);

	// array to hold the pixel values which will be convoluted
	float pixels_to_change[filter.h * filter.w];

	// finding the centre pixel for our filter
	int current_pixel = 0;
	float sum = 0;
	// runs through all of the pixels in image
	for(int y = 0; y < (target.h); y++){
		for(int x = 0; x < (target.w); x++){
			for(int i = 0; i < filter.h; i++){
				for(int j = 0; j < filter.w; j++){
					pixels_to_change[filter.w * i + j] = find_pixel_conv(target, x + j, y + i, 0);
				}
			}
			if( (y == 1000) && (x == 1000) ){
				print_target_matrix(target, x, y);
			}


			// add the result image to regular

			// do convolution calculations
			for(int q = 0; q < filter.h * filter.w; q++){
				sum += (pixels_to_change[q] * rotated.data[q]);
			}
			// divide to get proper changed value; not blown out
			if (if_blur(filter) == 1){
				sum = sum / 9;
			}

			if(sum < 0.01){
				sum = 0;
			}

			if(sum > 1){
				sum += 200;
			}

			result.data[current_pixel] = sum;
			sum = 0;
			current_pixel++;
		}
	}

	printf("we made it to end of apply filter\n");
	fflush(stdout);
	return result;
}
