/*
 * selection.c
 *
 *  Created on: Nov 20, 2017
 *      Author: davide
 */

#include "../generation.h"

#define MIN_TOURNAMENT_ROUNDS	1
#define MAX_TOURNAMENT_ROUNDS	3

#define RANK_START_PROBABILITY	0.33f


void elitism(Generation_ptr currentGeneration, Generation_ptr nextGeneration, float elitismPercentage){
	int count = currentGeneration->size * elitismPercentage;

	//nextGeneration->count = count;
	//Keep best in the generation
	/*
	if(currentGeneration->individuals[0].simulation.energy < currentGeneration->statistics.best.simulation.energy){
		memcpy(&nextGeneration->individuals[0], &currentGeneration->statistics.best, sizeof(Strategy));
		memcpy(&nextGeneration->individuals[1], currentGeneration->individuals, (count - 1) * sizeof(Strategy));
	}
	else{
	*/
		memcpy(nextGeneration->individuals, currentGeneration->individuals, count * sizeof(Strategy));
	//}
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

int rankSelection(Generation_ptr generation){
	int i = 0;
	float r = randFloat(0, 1);
	float p = 0;
	while(i < generation->size && p <= r){
		p += RANK_START_PROBABILITY * pow((1.0 - RANK_START_PROBABILITY), i);
		i++;
	}

	return i - 1;
}

