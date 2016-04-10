#ifndef __IMAGESRC
#define __IMAGESRC

#include <stdlib.h>

extern char _binary_sea_jpg_start, _binary_see_jpg_end;

#define NBIMAGES 1
#define SEAIMAGE 0

void* images_src[NBIMAGES];
size_t images_size[SEAIMAGE];

void initImage();

#endif
