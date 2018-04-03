/*
 * mathutils.c
 *
 *  Created on: Apr 3, 2018
 *      Author: davide
 */


#include "mathutils.h"

gsl_rng* rng = NULL;

void randInit(){
	srand(time(NULL));

	rng = gsl_rng_alloc (gsl_rng_taus);
	gsl_rng_set(rng, time(NULL));
}

int randInt(int min, int max){
	return (gsl_rng_get(rng) % (max - min + 1)) + min;
	//return (rand() % (max - min + 1)) + min;
}

float randFloat(float min, float max){
	return ((double)gsl_rng_get(rng) / (gsl_rng_max(rng) + 1.0)) * (max - min) + min;
	//return ((float)rand() / INT_MAX) * (max - min) + min;
}

int isZero(float val){
	if(val >= -0.00001 && val <= 0.00001){
		return 1;
	}

	return 0;
}

typedef union { double x; uint64_t i; } fd;
double fast_sin(double x) {
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

double fast_cos(double x) {
	const double pi_2 =  1.57079632679489661923132169163975144;
    return fast_sin(x + pi_2);
}

double fast_atan(double x){
	double x2 = x*x;
	double x3 = x2 * x;
	return x - x3/3 + x3*x2 / 5 + x3*x3*x / 7 - x3*x3*x3/9;
}

