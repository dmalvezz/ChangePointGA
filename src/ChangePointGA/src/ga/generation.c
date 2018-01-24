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
	/*
	fprintf(file, "%lu, %f, %f, %f, %f, %f, %f, %f, %f, %f, %lu\n",
			generationCount,
			generation->statistics.best.simulation.energy,
			generation->statistics.best.fitness,
			generation->statistics.fitnessMin,
			generation->statistics.fitnessMax,
			generation->statistics.fitnessMedian,
			generation->statistics.fitnessAvg,
			generation->statistics.lengthAvg,
			generation->statistics.similarityAvg,
			(float)generation->statistics.invalidCount / generation->count,
			generation->statistics.lastChange
	);
*/
}



void evalGenerationFitness(Generation_ptr generation, FitnessFunction fitnessFunction, MPI_Comm* comm){
	//Simulate

	//Use local thread
	if(comm == NULL){
		parallelSimulateStrategy(
				generation->individuals, generation->count, SIM_THREAD_COUNT,
				START_VELOCITY, END_VELOCITY, START_MAP, KEEP_TIME_INVALID
		);
	}
	//Use slaves
	else{
		char cmd;
		int size;
		MPI_Comm_size(*comm, &size);

		int strCount = generation->count / size;
		Strategy strategies[strCount];

		//Broadcast the command
		cmd = SLAVE_SIMULATE_STRAT_CMD;
		MPI_Bcast(&cmd, 1, MPI_CHAR, 0, *comm);

		//Scatter the strategies
		MPI_Scatter(
			generation->individuals, strCount, MPI_STRATEGY,
			strategies, strCount, MPI_STRATEGY,
			0, *comm
		);

		//Simulate all
		parallelSimulateStrategy(
				strategies, strCount, SIM_THREAD_COUNT,
				START_VELOCITY, END_VELOCITY, START_MAP, KEEP_TIME_INVALID
		);

		//Send back the simulate generation portion
		MPI_Gather(
			strategies, strCount, MPI_STRATEGY,
			generation->individuals, strCount, MPI_STRATEGY,
			0, *comm
		);
	}

	//Eval fitness
	#pragma omp parallel num_threads(SIM_THREAD_COUNT)
	{
		#pragma omp for
		for(int i = 0; i < generation->count; i++){
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
	generation->statistics.fitnessSumInverse = 0;
	generation->statistics.invalidCount = 0;

	//Save best
	if(generation->individuals[0].simulation.result == SIM_OK &&
			generation->individuals[0].fitness < generation->statistics.best.fitness){
		memcpy(&generation->statistics.best, &generation->individuals[0], sizeof(Strategy));

		generation->statistics.lastChange = 0;
	}
	else{
		generation->statistics.lastChange++;
	}

	//Reset statitics working with all the population
	resetStatistic(&generation->statistics.lengthStat, generation->count);
	resetStatistic(&generation->statistics.genotypeSimilarityStat, generation->count);

	for(int i = 0; i < generation->size; i++){
		updateStatistic(&generation->statistics.lengthStat, generation->individuals[i].size, i);
		updateStatistic(&generation->statistics.genotypeSimilarityStat, fabs(generation->statistics.best.size - generation->individuals[i].size), i);

		if(generation->individuals[i].simulation.result != SIM_OK){
			generation->statistics.invalidCount++;
		}
		else{
			generation->statistics.fitnessSumInverse += 1.0 / generation->individuals[i].fitness;
		}
	}

	//Reset statitics working with only the valid population
	resetStatistic(&generation->statistics.fitnessStat, generation->count - generation->statistics.invalidCount);
	resetStatistic(&generation->statistics.fitnessSimilarityStat, generation->count - generation->statistics.invalidCount);
	resetStatistic(&generation->statistics.fenotypeSimilarityStat, generation->count - generation->statistics.invalidCount);

	for(int i = 0; i < generation->size - generation->statistics.invalidCount; i++){
		//Similarity factor with previous best
		generation->individuals[i].similarity = evalStrategySimilarity(&generation->individuals[i], &generation->statistics.best);

		updateStatistic(&generation->statistics.fitnessStat, generation->individuals[i].fitness, i);
		updateStatistic(&generation->statistics.fitnessSimilarityStat, fabs(generation->statistics.best.fitness - generation->individuals[i].fitness), i);
		updateStatistic(&generation->statistics.fenotypeSimilarityStat, generation->individuals[i].similarity, i);
	}


	//Finalize the statistics
	finalizeStatistic(&generation->statistics.lengthStat);
	finalizeStatistic(&generation->statistics.genotypeSimilarityStat);
	finalizeStatistic(&generation->statistics.fitnessStat);
	finalizeStatistic(&generation->statistics.fitnessSimilarityStat);
	finalizeStatistic(&generation->statistics.fenotypeSimilarityStat);
}








