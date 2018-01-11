/*
 * fitness.c
 *
 *  Created on: Dec 23, 2017
 *      Author: davide
 */

#include "../generation.h"


float energyFitness(Generation_ptr generation, Strategy_ptr strategy){
	return strategy->simulation.energy;
}

float energyTimeFitness(Generation_ptr generation, Strategy_ptr strategy){
	return strategy->simulation.energy + pow(strategy->simulation.time - (TRACK_LENGTH / MIN_AVG_SPEED), 10);
}

float energyDiversityFitness(Generation_ptr generation, Strategy_ptr strategy){
	float x = strategy->simulation.energy / generation->statistics.best.simulation.energy;
	float y = strategy->similarity;

	return pow(x, 2) + pow(y, 4);
}
