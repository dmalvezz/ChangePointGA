/*
 * fitness.c
 *
 *  Created on: Dec 23, 2017
 *      Author: davide
 */

#include "../generation.h"


float energyFitness(Generation_ptr generation, int index){
	return generation->simOutputs[index].energy;
}

float energySquaredFitness(Generation_ptr generation, int index){
	return generation->simOutputs[index].energy * generation->simOutputs[index].energy;
}

float energyTimeFitness(Generation_ptr generation, int index){
	//return strategy->simulation.energy + pow(strategy->simulation.time - (MAX_TIME), 10);
	float fitness =  generation->simOutputs[index].energy;
	if(generation->simOutputs[index].time > MAX_TIME){
		fitness = fitness + 100 + exp(2 * fabs(MAX_TIME - generation->simOutputs[index].time));
	}

	return fitness;
}

float energyDiversityFitness(Generation_ptr generation, int index){
	//float x = generation->simOutputs[index].energy / generation->statistics.best.simulation.energy;
	//float y = generation->individuals[index].similarity;

	//return pow(x, 2) + pow(y, 4);
	return 0;
}
