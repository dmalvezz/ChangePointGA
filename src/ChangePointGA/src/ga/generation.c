/*
 * generation.c
 *
 *  Created on: Nov 19, 2017
 *      Author: davide
 */

#include "generation.h"


Generation_ptr initEmptyGeneration(int size){
	Generation_ptr generation = (Generation_ptr)malloc(sizeof(Generation));

	generation->count = 0;
	generation->size = size;
	generation->individuals = (Strategy_ptr)malloc(size * sizeof(Strategy));

	return generation;
}

Generation_ptr initRandomGeneration(int size){
	Generation_ptr generation = (Generation_ptr)malloc(sizeof(Generation));

	generation->count = size;
	generation->size = size;
	generation->individuals = (Strategy_ptr)malloc(size * sizeof(Strategy));

	for(int i = 0; i < size; i++){
		initStrategy(&generation->individuals[i], SPACE_STEP);
	}

	return generation;
}

void disposeGeneration(Generation_ptr generation){
	free(generation->individuals);
	free(generation);
}

void printGeneration(Generation_ptr generation){
	for(int i = 0; i < generation->size; i++){
		printStrategy(&generation->individuals[i]);
		printf("\n");
	}
}



void evalGenerationFitness(Generation_ptr generation, float startVelocity, int startMap){
	#pragma omp parallel
	{
		#pragma omp for
		for(int i = 0; i < generation->size; i++){
			evalStrategyFitness(&generation->individuals[i], startVelocity, startMap);
		}
	}
}

void sortGenerationByFitness(Generation_ptr generation){
	qsort(generation->individuals, generation->size, sizeof(Strategy), compareStrategyFitness);
}

void updateGenerationStatistics(Generation_ptr generation){
	generation->statistics.fitnessMin = generation->individuals[0].fitness;
	generation->statistics.fitnessMax = generation->individuals[generation->size - 1].fitness;
	generation->statistics.fitnessMedian = generation->individuals[generation->size / 2].fitness;
	generation->statistics.fitnessAvg = 0;

	generation->statistics.lengthAvg = 0;

	generation->statistics.invalidCount = 0;

	for(int i = 0; i < generation->size; i++){
		generation->statistics.fitnessAvg += generation->individuals[i].fitness;

		generation->statistics.lengthAvg += generation->individuals[i].size;

		if(generation->individuals[i].simulation.result != SIM_OK){
			generation->statistics.invalidCount++;
		}
	}

	generation->statistics.fitnessAvg /= generation->size;
	generation->statistics.lengthAvg /= generation->size;

}








