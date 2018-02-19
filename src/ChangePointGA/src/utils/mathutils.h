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

#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
	#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

#ifndef swapv
	#define swapv(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)
#endif

static void randInit(){
	srand(time(NULL));
}

static int randInt(int min, int max){
	return (rand() % (max - min + 1)) + min;
}

static float randFloat(float min, float max){
	return ((float)rand() / INT_MAX) * (max - min) + min;
}

static int isZero(float val){
	if(val > -FLT_EPSILON && val < FLT_EPSILON){
		return 1;
	}

	return 0;
}

#endif
