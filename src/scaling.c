#include "scaling.h"
#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "filter.h"

image scale(image target){
    int x, y, x2, y2;
    float p, q;
    image result = make_image(target.w/2, target.h/2, 1);
    for (y = 0; y < result.h; y++) {
        y2 = 2 * y;
        for (x = 0; x < result.w; x++) {
            x2 = 2 * x;
            p = average(target.data[y2 * target.w + x2], target.data[y2 * target.w + x2 + 1]);
            q = average(target.data[(y2+1) * target.w + x2], target.data[(y2+1) * target.w + x2 + 1]);
            result.data[y * result.w + x] = average(p, q);
        }
    }
    return result;
}

float average(float a, float b){
	return (a + b)/2;
}


