/**
 * @file mandel_lib.c
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "jpegrw.h"
#include "mandel_lib.h"
#include <pthread.h>
#include "thread_arg.h"

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

void* thread_fn(void *args) {
	thread_arg* thread_args = (thread_arg *) args;

	for (int i = thread_args->start; i < thread_args->end; i++) {
		int row = i / thread_args->width;
		int col = i % thread_args->width;

		// Determine the point in x,y space for that pixel.
		double x = thread_args->xmin + col*(thread_args->xmax-thread_args->xmin)/thread_args->width;
		double y = thread_args->ymin + row*(thread_args->ymax-thread_args->ymin)/thread_args->height;

		// Compute the iterations at that point.
		int iters = iterations_at_point(x,y,thread_args->max);

		setPixelCOLOR(thread_args->img,col,row,iteration_to_color(iters,thread_args->max));
	}

	free(thread_args);
	return NULL;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/
void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max, int num_threads)
{
	pthread_t* threads[num_threads];
	int width = img->width;
	int height = img->height;

	pthread_attr_t attr;

	pthread_attr_init(&attr);

	// For every pixel in the image...
	int pixels = height * width;

	int last_end = 0;

	for (int t = 0; t < num_threads; t++) {
		thread_arg *args = (thread_arg *) malloc(sizeof(thread_arg));
			
		*args = (thread_arg) {
			img,
			xmin,
			xmax,
			ymin,
			ymax,
			max,
			height,
			width,
			start: last_end,
			end: last_end + pixels / num_threads,
		};

		if (t < pixels % num_threads) {
			args->end++;
		}

		last_end = args->end;
		
		pthread_t* thread = malloc(sizeof(pthread_t));

		if (pthread_create(thread, &attr, &thread_fn, args)) {
			puts("Error occured creating thread");
		}

		threads[t] = thread;
	}

	for (int t = 0; t < num_threads; t++) {
		if (pthread_join(*threads[t], NULL)) {
			puts("Error occured joining thread");
		}
		free(threads[t]);
	}

	pthread_attr_destroy(&attr);
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}