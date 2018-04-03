/*
 ============================================================================
 Name        : mathutils.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Math utils function
 ============================================================================
 */

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <gsl/gsl_rng.h>

#ifndef M_PI
	#define M_PI	3.14159265359
#endif

#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
	#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

#ifndef swapv
	#define swapv(x, y) do { __typeof__(x) SWAP = x; x = y; y = SWAP; } while (0)
#endif

extern gsl_rng* rng;

extern void randInit();
extern int randInt(int min, int max);
extern float randFloat(float min, float max);

extern int isZero(float val);


extern double fast_sin(double x);

extern double fast_cos(double x);

extern double fast_atan(double x);

#define sin_f	fast_sin
#define cos_f	fast_cos
#define atan_f	fast_atan

#endif
