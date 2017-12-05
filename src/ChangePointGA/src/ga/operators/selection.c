/*
 * selection.c
 *
 *  Created on: Nov 20, 2017
 *      Author: davide
 */

#include "../generation.h"

#define MIN_TOURNAMENT_ROUNDS	5
#define MAX_TOURNAMENT_ROUNDS	10


void elitism(Generation_ptr currentGeneration, Generation_ptr nextGeneration, float elitismPercentage){
	int count = currentGeneration->size * elitismPercentage;

	nextGeneration->count = count;
	memcpy(nextGeneration->individuals, currentGeneration->individuals, count * sizeof(Strategy));
}


int fitnessProportionalSelection(Generation_ptr generation){
	int i = 0;
	float r = randFloat(0, 1);
	float p = 0;
	while(i < generation->size && p <= r){
		p += 1.0f / (generation->individuals[i].fitness / (generation->statistics.fitnessAvg * generation->size));
		i++;
	}

	return i - 1;
}

int tournamentSelection(Generation_ptr generation){
	int min = generation->size - 1;
	int round = randInt(MIN_TOURNAMENT_ROUNDS, MAX_TOURNAMENT_ROUNDS);
	int r;
	for(int i = 0; i < round; i++){
		r = randInt(0, generation->size - 1);

		if(generation->individuals[r].fitness < generation->individuals[min].fitness){
			min = r;
		}
	}

	return min;
}
