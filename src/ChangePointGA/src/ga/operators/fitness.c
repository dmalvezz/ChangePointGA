/*
 * fitness.c
 *
 *  Created on: Dec 23, 2017
 *      Author: davide
 */

#include "../generation.h"


//Pure energy based fitness
float energyFitness(Strategy_ptr strategy){
	return strategy->simulation.energy;
}

//Energy and time fitness
float energyTimeFitness(Strategy_ptr strategy){
	return strategy->fitness = strategy->simulation.energy + pow(strategy->simulation.time - (TRACK_LENGTH / MIN_AVG_SPEED), 4);
}
