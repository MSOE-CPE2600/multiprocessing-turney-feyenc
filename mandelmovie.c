/**
 * @file mandelmovie.c
 * @brief creates 50 jpeg frames a video zooming in on a mandelbrot fractal
 * @name Charles Feyen
 * 
 * Lab 11 - Multiprocessing
 * CPE 2600 - 121
 * 
 * Compile with: gcc -o mandelmovie mandelmovie.c mandel_lib.c jpegrw.c -ljpeg -lpthread -lrt
 * 
 * @date 11/14/2025
 */

#include <wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "jpegrw.h"
#include "mandel_lib.h"
#include "thread_arg.h"

/**
 * @brief creates a frame
 * @param xcenter the center x coordinate
 * @param ycenter the center y coordinate
 * @param image_width the width of the image
 * @param image_height
 * @param xscale
 * @param max
 * @param outfile
 */
void make_frame(const double xcenter, const double ycenter, const unsigned int image_width, const unsigned int image_height, const double xscale, const int max, const char *outfile, int num_threads);
void show_help();

int main(int argc, char *argv[]) {
    double xcenter = 0.0;
    double ycenter = 0.0;
    double xscale = 4.0;
    int image_width = 1000;
    int image_height = 1000;
    int max = 1000;
    int num_threads = 1;
    char c;
    unsigned int max_procs = 100;

    // gets arguments
    while((c = getopt(argc,argv,"x:y:s:W:H:m:o:p:t:h")) != -1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'p':
				max_procs = atoi(optarg);
				break;
			case 't':
				int thread_count = atoi(optarg);
                if (thread_count >= 1 && thread_count <= MAX_THREADS) {
                    num_threads = thread_count;
                }
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

    const unsigned int frames = 50;

    // creates a semaphore
    void *semaphore = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (semaphore == MAP_FAILED) {
        puts("Could allocate shared memory");
        exit(1);
    }

    if (sem_init(semaphore, 1, max_procs) == -1) {
        puts("Could initialize semaphore");
        exit(1);
    }

    printf("semaphore: %p\n", semaphore);

    // creates the frames
    for (int i = 0; i < frames; i++) {
        sem_wait(semaphore);

        __pid_t pid = fork();

        if (pid == 0) {
            char name[25];

            sprintf(name, "mandelmovie%d.jpg", i);
            make_frame(xcenter, ycenter, image_width, image_height, xscale - i / 25.0, max, name, num_threads);

            sem_post(semaphore);
            exit(0);
        }
    }

    int procs_left = 0;

    // waits for frames to finish
    while (procs_left != max_procs) {
        sem_getvalue(semaphore, &procs_left);
    }

    // frees the semaphore
    if (sem_destroy(semaphore) == -1) {
        puts("Could not close semaphore");
        exit(1);
    }
}

void make_frame(const double xcenter, const double ycenter, const unsigned int image_width, const unsigned int image_height, const double xscale, const int max, const char *outfile, int num_threads) {
    // Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)
    double yscale = xscale / image_width * image_height;

    // Display the configuration of the image.
    printf("mandel: x=%lf y=%lf xscale=%lf yscale=%1f max=%d outfile=%s\n", xcenter, ycenter, xscale, yscale, max, outfile);

    // Create a raw image of the appropriate size.
    imgRawImage* img = initRawImage(image_width, image_height);

    // Fill it with a black
    setImageCOLOR(img, 0);

    // Compute the Mandelbrot image
    compute_image(img, xcenter-xscale/2, xcenter+xscale/2, ycenter-yscale/2, ycenter+yscale/2, max, num_threads);

    // Save the image in the stated file.
    storeJpegImageFile(img, outfile);

    // free the mallocs
    freeRawImage(img);
}

// Show help message
void show_help()
{
	puts("Use: mandelmovie [options]");
	puts("Where options are:");
	puts("-m <max>    The maximum number of iterations per point per frame. (default=1000)");
	puts("-x <coord>  X coordinate of each frame center point. (default=0)");
	puts("-y <coord>  Y coordinate of each frame center point. (default=0)");
	puts("-s <scale>  Scale of each frame in Mandlebrot coordinates (X-axis). (default=4)");
	puts("-W <pixels> Width of each frame in pixels. (default=1000)");
	puts("-H <pixels> Height of each frame in pixels. (default=1000)");
	puts("-p <procs>  Maximum number of frames that can be generated at a time");
	puts("-h          Show this help text.");
}