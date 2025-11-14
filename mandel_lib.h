/**
 * @file mandel_lib.h
 * @brief functions moved from mandle.c
 * @name Charles Feyen
 * 
 * Lab 11 - Multiprocessing
 * CPE 2600 - 121
 * 
 * Compile with: gcc -o mandelmovie mandelmovie.c mandel_lib.c jpegrw.c -ljpeg -lpthread -lrt
 * 
 * @date 11/12/2025
 */

#ifndef MANDEL_H
#define MANDEL_H

#include "jpegrw.h"

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max );

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max );

/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max );


#endif