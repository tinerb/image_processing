#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "filter.h"
#include "image.h"
#include "scaling.h"
#include "segmentation.h"
#include "feature_vector.h"


void print_image(image target){
	for(int i = 0; i < target.w * target.h; i++){
		printf("%f ", target.data[i]);
		if((i + 1) % target.w == 0){
			printf("\n");
		}
	}
}

// returns the index of the x, y, c pixel given
int find_pixel(image im, int x, int y, int c){
	// clamping
	if(x >= im.w){
		x = im.w - 1;
	}
	if(x < 0){
		x = 0;
	}
	if(y >= im.h){
		y = im.h - 1;
	}
	if(y < 0){
		y = 0;
	}

	int pixel = (x * y + x + y);
	int channel = (im.w * im.h * c);
	pixel += channel;
	return pixel;
}

float get_pixel(image im, int x, int y, int c)
{
	int pixel = find_pixel(im, x, y, c);
    return im.data[pixel];
}

void set_pixel(image im, int x, int y, int c, float v)
{
	if(x < 0 || x >= im.w || y < 0 || y >= im.h){
		return;
	}
	else{
		int pixel = find_pixel(im, x, y, c);
		im.data[pixel] = v;
	}
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);

    for(int i = 0; i < im.w * im.h * im.c; i++){
    	copy.data[i] = im.data[i];
    }

    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    printf("width: %d   height: %d\n", im.w, im.h);
    int counter = 0;
    // loop through image array, retrieving each value
    for(int i = 0; i < gray.w; i++){
    	for(int j = 0; j < gray.h; j++){
    		float red, green, blue;
    		red = get_pixel(im, i, j, 0);
    		green = get_pixel(im, i, j, 1);
    		blue = get_pixel(im, i, j, 2);
    		float luma = ((0.299 * red) + (0.587 * green) + (0.114 * blue));

    		int pixel = get_pixel(gray, i, j, 0);
    		gray.data[pixel] = luma;
    		counter++;
    		//printf("r: %f g: %f b: %f  LUMA: %f\n", red, green, blue, luma);
    	}
    }
    printf("counter: %d\n", counter);
    return gray;
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

int main(int argc, char **argv){
	char image_name[100];
	int assignment;
	char *out = find_char_arg(argc, argv, "-o", "out");
	char *out2 = find_char_arg(argc, argv, "-o", "out2");
	int is_binary;

	//starting clock
	clock_t begin = clock();
	//prompting for input
	printf("Enter the image name you want to use:\n");
	fflush(stdout);
	scanf("%s", &image_name);
	image target = load_image(image_name);
	printf("Enter the assignment # you want to demo:\n");
	fflush(stdout);
	scanf("%d", &assignment);
	//different assignments
	if (assignment == 1){
		int filter_number;
		image temp = make_box_filter(3), filter;
		//prompting for input
		printf("Which filter do you want to use?\n");
		printf("outline = 1|sobel = 2|blur = 3|high pass = 4\n");
		fflush(stdout);
		scanf("%d", &filter_number);
		//different filters
		if (filter_number == 1){
			filter = outline_filter(temp);
		}
		else if(filter_number == 2){
			filter = sobel_filter(temp);
		}
		else if(filter_number == 3){
			filter = blur_filter(temp);
		}
		else if(filter_number == 4){
			filter = high_pass_filter(temp);
		}
		image result = apply_filter(target, filter);
		save_image(result, out);
	}
	else if(assignment == 2){
		printf("Is the image binary? (0 if yes, 1 if greyscale)\n");
		fflush(stdout);
		scanf("%d", &is_binary);
		image result = segment_image(target, is_binary);
		save_image(result, out);

	}
	else if(assignment == 3){
		//filters for 1 by 3
		image filter1 = make_image(3, 1, 1);
		image filter2 = make_image(1, 3, 1);
		//filter for 3 by 3
		image filter3 = make_image(3, 3, 1);
		//populating filter arrays
		for (int i = 0; i < 3; i++){
			filter1.data[i] = (float)(1)/(float)(3);
			filter2.data[i] = (float)(1)/(float)(3);
		}
		for (int i = 0; i < 9; i++){
			filter3.data[i] = (float)(1)/(float)(9);
		}
		//doing convolutions
		image result1 = apply_filter(target, filter1);
		result1 = apply_filter(result1, filter2);
		image result2 = apply_filter(target, filter3);
		save_image(result1, out);
		save_image(result2, out2);
	}
	else if(assignment == 4){
		image result = scale_segments(target, 0);
		save_image(result, out);
	}
	else if(assignment == 5){
		int temp, result;
		float *vector = malloc(sizeof(float) * 16);
		FILE *fptr;
		vector = get_vector(target);
		printf("0 to write to dataset. 1 to recognize character.\n");
		fflush(stdout);
		scanf("%d", &temp);
		if (temp == 0){
			image result = write_entire_image(target, 0);
			save_image(result, out);
		}
		else if(temp == 1){
			fptr = fopen("vector.txt", "r");
			result = recognize(fptr, vector);
			printf("The character is: %d\n", result);
		}
		fclose(fptr);
	}
	else if(assignment == 6){
		image result = label_image(target);
		save_image(result, out);
	}

	// ending the clock
	clock_t end = clock();
	double time = (double) (end - begin) / CLOCKS_PER_SEC;


	printf("Refresh the folder that you put this program in. The output file is titled 'out.jpg'.\n");
	printf("Program took: %.2f seconds to run.", time);
}
