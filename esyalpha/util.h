#ifndef __UTIL_H
#define __UTIL_H

#include <math.h>

#define MAX(x,y)	(((x)>(y))?(x):(y))
#define MIN(x,y)	(((x)<(y))?(x):(y))
#define DELTA		1.0e-10

int eq(float x, float y);
int le(float x, float y);
int ge(float x, float y);
void fatal (char *s);

#endif
