/*
 * selection.c
 *
 *  Created on: Nov 20, 2017
 *      Author: davide
 */

#include "../generation.h"

void elitism(Generation_ptr currentGeneration, Generation_ptr nextGeneration, float elitismPercentage){
	int count = max(1, currentGeneration->count * elitismPercentage);

	memcpy(nextGeneration->individuals, currentGeneration->individuals, count * sizeof(Strategy));
}

int FUSS(Generation_ptr currentGeneration, Generation_ptr nextGeneration){
	//Individual to be replaced
	int r = randInt(1, nextGeneration->count - 1);

	int validCount = currentGeneration->count - currentGeneration->statistics.invalidCount;
	//Fitness to keep
	float fitness = randFloat(currentGeneration->individuals[0].fitness, currentGeneration->individuals[validCount - 1].fitness);

	//Binary search
	int sx = 0;
	int dx = validCount - 1;
	int m = (sx + dx) / 2;

	while (sx <= dx) {
		m = (sx + dx) / 2;
		if (currentGeneration->individuals[m].fitness < fitness){
			sx = m + 1;
		}
		else if (currentGeneration->individuals[m].fitness > fitness){
			dx = m - 1;
		}
		else{
			break;
		}
	}

	//Replace
	memcpy(&nextGeneration->individuals[r], &currentGeneration->individuals[m], sizeof(Strategy));

	//Return individual index in next generation
	return r;
}

void filteredFUSS(Generation_ptr currentGeneration, Generation_ptr nextGeneration){
	int r = FUSS(currentGeneration, nextGeneration);

	filterStrategy(&nextGeneration->individuals[r]);
}


int fitnessProportionalSelection(Generation_ptr generation){
	int i = 0;
	int validCount = generation->count - generation->statistics.invalidCount;
	float r = randFloat(0, 1);
	float p = 0;

	while(i < validCount && p <= r){
		p += (1.0 / generation->individuals[i].fitness) / generation->statistics.fitnessSumInverse;
		i++;
	}

	return i - 1;
}

#define MIN_TOURNAMENT_ROUNDS	3
#define MAX_TOURNAMENT_ROUNDS	3

int tournamentSelection(Generation_ptr generation){
	int min = generation->count - 1;//randInt(0, generation->count - 1);
	int round = MAX_TOURNAMENT_ROUNDS;//randInt(MIN_TOURNAMENT_ROUNDS, MAX_TOURNAMENT_ROUNDS);
	int r;

	for(int i = 0; i < round; i++){
		r = randInt(0, generation->count - 1);

		if(generation->individuals[r].fitness < generation->individuals[min].fitness){
			min = r;
		}
	}

	return min;
}


#define RANK_START_PROBABILITY	0.02f

int rankSelection(Generation_ptr generation){
	int i = 0;
	float r = randFloat(0, 1);
	float p = 0;

	while(i < generation->count && p <= r){
		p += RANK_START_PROBABILITY * pow((1.0 - RANK_START_PROBABILITY), i);
		i++;
	}

	return i - 1;
}


#define LINEAR_RANK_SP 1.5f

int linearRankWithPressureSelection(Generation_ptr generation){
	int i = 0;
	float r = randFloat(0, 1);
	float p = 0;

	while(i < generation->count && p <= r){
		p += (2.0f - LINEAR_RANK_SP + 2.0f * (LINEAR_RANK_SP - 1.0f) * (generation->count - (i + 1)) /(generation->count - 1))/generation->count;
		//p += (float)(generation->count - i) / ((generation->count + 1) * generation->count / 2);
		i++;
	}

	return i - 1;
}

