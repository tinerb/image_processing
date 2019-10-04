#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "filter.h"
#include "image.h"
#include "segmentation.h"


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

	// we super fast vroom vroom
	clock_t begin = clock();

	char *out = find_char_arg(argc, argv, "-o", "qout");
	image target = make_image(5, 5, 1);
	int dataArray[] = {0, 0 ,-1, -1, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, -1, -1, 0, -1};
	for(int i = 0; i < 25; i++){
		target.data[i] = dataArray[i];
	}
	image result = segment_image(target);
	print_image(result);
//	image target = load_image("cat.jpg");
//	image temp = make_box_filter(3);
//	image filter = emboss_filter(temp);
//	image filter = outline_filter(temp);
//	image filter = sobel_filter(temp);
//	image filter = sharpen_filter(temp);
//	image filter = blur_filter(temp);
//	image filter = high_pass_filter(temp);
//	image result = apply_filter(target, filter);
//	save_image(result, out);

	// ending the clock
	clock_t end = clock();
	double time = (double) (end - begin) / CLOCKS_PER_SEC;


	printf("Refresh the folder that you put this program in. The output file is titled 'out.jpg'.\n");
	printf("Program took: %.2f seconds to run.", time);
}
