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
	if(val >= -0.000001 && val <= 0.000001){
		return 1;
	}

	return 0;
}

static float atanApprox(float x){
	return x - x*x*x / 3 + x*x*x*x*x / 5 - x*x*x*x*x*x*x / 7;
}

typedef union { double x; uint64_t i; } fd;
static double fast_sin(double x) {
    // Polynomial constants generated with sollya.
    // fpminimax(sin(x), [|1,3,5,7|], [|D...|], [-pi/2;pi/2]);
    // Both relative and absolute error is 9.39e-7.
    const double magicround =  6755399441055744.0;
    const double negpi      = -3.14159265358979323846264338327950288;
    const double invpi      =  0.31830988618379067153776752674502872;
    const double a          = -0.00018488140186756154724131984146140;
    const double b          =  0.00831189979755905285208061883395203;
    const double c          = -0.16665554092439083255783316417364403;
    const double d          =  0.99999906089941981157664940838003531;

    // Range-reduce to [-pi/2, pi/2] and store if period is odd.
    fd u = {invpi*x + magicround };
    uint64_t odd_period = u.i << 63;
    u.x = x + negpi * (int32_t)(u.i & 0xffffffff);

    // 7th degree odd polynomial followed by IEEE754 sign flip on odd periods.
    double x2 = u.x*u.x;
    double p = d + x2*(c + x2*(b + x2*a));
    u.i ^= odd_period;
    return u.x * p;
}

static double fast_cos(double x) {
    const double pi_2 =  1.57079632679489661923132169163975144;
    return fast_sin(x + pi_2);
}

static double fast_atan(double x){
	double x2 = x*x;
	double x3 = x2 * x;
	return x - x3/3 + x3*x2 / 5 + x3*x3*x / 7 - x3*x3*x3/9;
}

#define sin_f	fast_sin
#define cos_f	fast_cos
#define atan_f	fast_atan

#endif
