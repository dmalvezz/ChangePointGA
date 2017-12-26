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

	generation->statistics.lastChange = 0;

	generation->statistics.best.fitness = INFINITY;
	generation->statistics.best.simulation.energy = INFINITY;
	generation->statistics.best.simulation.time = INFINITY;

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

	generation->statistics.lastChange = 0;

	generation->statistics.best.fitness = INFINITY;
	generation->statistics.best.simulation.energy = INFINITY;
	generation->statistics.best.simulation.time = INFINITY;

	return generation;
}

void disposeGeneration(Generation_ptr generation){
	free(generation->individuals);
	free(generation);
}

void printGeneration(Generation_ptr generation){
	printf("Generation\n");
	for(int i = 0; i < generation->size; i++){
		printf("fitness %f\n", generation->individuals[i].fitness);
		printStrategy(&generation->individuals[i]);
		printf("\n");
	}
}

void generationToFile(Generation_ptr generation, const char* fileName){
	FILE* file = fopen(fileName, "wb");

	fwrite(generation->individuals, sizeof(Strategy), generation->count, file);

	fclose(file);
}

void generationFromFile(Generation_ptr generation, const char* fileName){
	FILE* file = fopen(fileName, "rb");

	generation->count = 0;
	while(fread(&generation->individuals[generation->count], sizeof(Strategy), 1, file) > 0){
		generation->count++;
	}

	fclose(file);
}


void statisticsToFile(Generation_ptr generation, unsigned long int generationCount, FILE* file){
	fprintf(file, "%lu, %f, %f, %f, %f, %f, %f, %f, %lu\n",
			generationCount,
			generation->statistics.best.simulation.energy,
			generation->statistics.best.fitness,
			generation->statistics.fitnessMin,
			generation->statistics.fitnessMedian,
			generation->statistics.lengthAvg,
			generation->statistics.similarityAvg,
			(float)generation->statistics.invalidCount / generation->count,
			generation->statistics.lastChange
	);

}



void evalGenerationFitness(Generation_ptr generation, float startVelocity, int startMap, FitnessFunction fitnessFunction){
	#pragma omp parallel
	{
		#pragma omp for
		for(int i = 0; i < generation->size; i++){
			//Simulate the strategy
			simulateStrategy(&generation->individuals[i], startVelocity, startMap);

			//Similarity factor with previous best
			generation->individuals[i].similarity = evalStrategySimilarity(&generation->individuals[i], &generation->statistics.best);

			//Update fitness
			generation->individuals[i].fitness = fitnessFunction(generation, &generation->individuals[i]);
		}
	}
}

void sortGenerationByFitness(Generation_ptr generation){
	qsort(generation->individuals, generation->count, sizeof(Strategy), compareStrategyFitness);
}

void updateGenerationStatistics(Generation_ptr generation){
	//Init
	generation->statistics.fitnessMin = generation->individuals[0].fitness;
	generation->statistics.fitnessMax = generation->individuals[generation->size - 1].fitness;
	generation->statistics.fitnessMedian = generation->individuals[generation->size / 2].fitness;
	generation->statistics.fitnessAvg = 0;
	generation->statistics.fitnessSumInverse = 0;


	generation->statistics.lengthAvg = 0;
	generation->statistics.similarityAvg = 0;

	generation->statistics.invalidCount = 0;

	//Sum up
	for(int i = 0; i < generation->size; i++){
		generation->statistics.lengthAvg += generation->individuals[i].size;

		if(generation->individuals[i].simulation.result != SIM_OK){
			generation->statistics.invalidCount++;
		}
		else{
			generation->statistics.fitnessAvg += generation->individuals[i].fitness;
			generation->statistics.fitnessSumInverse += 1.0 / generation->individuals[i].fitness;
			generation->statistics.similarityAvg += generation->individuals[i].similarity;
		}
	}

	//Avgs
	generation->statistics.fitnessAvg /= (generation->size - generation->statistics.invalidCount);
	generation->statistics.lengthAvg /= generation->size;
	generation->statistics.similarityAvg /= (generation->size - generation->statistics.invalidCount);

	//Save best
	if(generation->individuals[0].simulation.result == SIM_OK &&
			generation->individuals[0].simulation.energy < generation->statistics.best.simulation.energy){
		memcpy(&generation->statistics.best, &generation->individuals[0], sizeof(Strategy));

		generation->statistics.lastChange = 0;
	}
	else{
		generation->statistics.lastChange++;
	}
}








