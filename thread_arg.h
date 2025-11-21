#ifndef THREAD_ARG_H
#define THREAD_ARG_H

#include "jpegrw.h"

#define MAX_THREADS 20

typedef struct thread_arg {
    imgRawImage* img;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    int max;
    unsigned int height;
    unsigned int width;
    int start;
    int end;
} thread_arg;
#endif