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
	//return strategy->simulation.energy + pow(strategy->simulation.time - (MAX_TIME), 10);
	float fitness =  strategy->simulation.energy;
	if(strategy->simulation.time > MAX_TIME){
		fitness = fitness + 100 + exp(2 * fabs(MAX_TIME - strategy->simulation.time));
	}

	return fitness;
}

float energyDiversityFitness(Generation_ptr generation, Strategy_ptr strategy){
	float x = strategy->simulation.energy / generation->statistics.best.simulation.energy;
	float y = strategy->similarity;

	return pow(x, 2) + pow(y, 4);
}
