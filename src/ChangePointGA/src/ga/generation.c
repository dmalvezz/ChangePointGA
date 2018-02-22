/*
 * generation.c
 *
 *  Created on: Nov 19, 2017
 *      Author: davide
 */

#include "generation.h"
#include "../utils/sysutils.h"


Generation_ptr initEmptyGeneration(int size){
	Generation_ptr generation = (Generation_ptr)malloc(sizeof(Generation));

	generation->count = 0;
	generation->size = size;
	generation->individuals = (Strategy_ptr)malloc(size * sizeof(Strategy));
	generation->simOutputs = (SimulationOutput_ptr)malloc(size * sizeof(SimulationOutput));

	generation->statistics.lastChange = 0;

	generation->statistics.best.fitness = INFINITY;
	generation->statistics.bestOutput.energy = INFINITY;
	generation->statistics.bestOutput.time = INFINITY;

	return generation;
}

Generation_ptr initRandomGeneration(int size){
	Generation_ptr generation = (Generation_ptr)malloc(sizeof(Generation));

	generation->count = size;
	generation->size = size;
	generation->individuals = (Strategy_ptr)malloc(size * sizeof(Strategy));
	generation->simOutputs = (SimulationOutput_ptr)malloc(size * sizeof(SimulationOutput));

	for(int i = 0; i < size; i++){
		initStrategy(&generation->individuals[i], SPACE_STEP);
	}

	generation->statistics.lastChange = 0;

	generation->statistics.best.fitness = INFINITY;
	generation->statistics.bestOutput.energy = INFINITY;
	generation->statistics.bestOutput.time = INFINITY;

	return generation;
}

void disposeGeneration(Generation_ptr generation){
	free(generation->individuals);
	free(generation->simOutputs);
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

void generationToCsv(Generation_ptr generation, const char* fileName){
	FILE* file = fopen(fileName, "wt");

	fprintf(file, "index, fitness, energy, time, length,\n");
	for(int i = 0; i < generation->count; i++){
		fprintf(file, "%d, %f, %f, %f, %d,\n",
				i,
				generation->individuals[i].fitness,
				generation->simOutputs[i].energy,
				generation->simOutputs[i].time,
				generation->individuals[i].size
		);
	}

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
	fprintf(file, "%lu,  %lu,  %f,  %f,  %f,  %f,   %f,",
			generationCount,
			generation->statistics.lastChange,
			generation->statistics.bestOutput.energy,
			generation->statistics.bestOutput.time,
			generation->statistics.best.fitness,
			(float)generation->statistics.invalidCount / generation->count,
			generation->statistics.fitnessSumInverse
		);

	for(int i = 0; i < SIM_RESULT_COUNT; i++){
		fprintf(file, "%d, ", generation->statistics.invalidTypeCount[i]);
	}

	printStatisticCsvData(&generation->statistics.fitnessStat, file);
	printStatisticCsvData(&generation->statistics.lengthStat, file);
	printStatisticCsvData(&generation->statistics.genotypeSimilarityStat, file);
	printStatisticCsvData(&generation->statistics.genotypeAbsSimilarityStat, file);
	printStatisticCsvData(&generation->statistics.fenotypeSimilarityStat, file);
	printStatisticCsvData(&generation->statistics.fitnessSimilarityStat, file);
	printStatisticCsvData(&generation->statistics.fitnessAbsSimilarityStat, file);

	fprintf(file, "\n");
}



double evalGenerationFitness(Generation_ptr generation, FitnessFunction fitnessFunction, MPI_Comm* comm){
	//Simulate
	double commTime = 0, timer;

	//Use local thread
	if(comm == NULL){
		parallelSimulateStrategy(
				generation->individuals, generation->simOutputs, generation->count,
				SIM_THREAD_COUNT,
				START_VELOCITY, END_VELOCITY, START_MAP, KEEP_TIME_INVALID
		);
	}
	//Use slaves
	else{
		char cmd;
		int size;
		MPI_Comm_size(*comm, &size);

		int strCount = generation->count / size;
		//Strategy_ptr strategies = (Strategy_ptr)malloc(sizeof(Strategy) * generation->count);
		Strategy strategies[generation->count];
		SimulationOutput simOut[generation->count];

		//Broadcast the command
		timer = getTime();
		cmd = SLAVE_SIMULATE_STRAT_CMD;
		MPI_Bcast(&cmd, 1, MPI_CHAR, 0, *comm);

		//Scatter the strategies
		MPI_Scatter(
			generation->individuals, strCount, MPI_STRATEGY,
			strategies, strCount, MPI_STRATEGY,
			0, *comm
		);
		commTime += (getTime() - timer);

		//Simulate all
		parallelSimulateStrategy(
				strategies, simOut, strCount,
				SIM_THREAD_COUNT,
				START_VELOCITY, END_VELOCITY, START_MAP, KEEP_TIME_INVALID
		);

		//Send back the simulate generation portion
		timer = getTime();
		MPI_Gather(
			simOut, strCount, MPI_SIMULATION_OUTPUT,
			generation->simOutputs, strCount, MPI_SIMULATION_OUTPUT,
			0, *comm
		);
		commTime += (getTime() - timer);

		//free(strategies);
	}

	//Eval fitness
	#pragma omp parallel num_threads(SIM_THREAD_COUNT)
	{
		#pragma omp for
		for(int i = 0; i < generation->count; i++){
			//Update fitness
			generation->individuals[i].fitness = fitnessFunction(generation, i);
		}
	}

	return commTime;
}

static void qsortkeyvalue(Strategy_ptr key, SimulationOutput_ptr value, int left, int right) {
	int i, last;

	if (left >= right)
		return;
	swapv(key[left], key[(left + right)/2]);
	swapv(value[left], value[(left + right)/2]);

	last = left;

	for (i = left+1; i <= right; i++){
		if (key[i].fitness < key[left].fitness){
			last++;
			swapv(key[last], key[i]);
			swapv(value[last], value[i]);
		}
	}

	swapv(key[left], key[last]);
	swapv(value[left], value[last]);


	qsortkeyvalue(key, value, left, last-1);
	qsortkeyvalue(key, value, last+1, right);
}

void sortGenerationByFitness(Generation_ptr generation){
	//qsort(generation->individuals, generation->count, sizeof(Strategy), compareStrategyFitness);

	qsortkeyvalue(generation->individuals, generation->simOutputs, 0, generation->count - 1);
}

#include <assert.h>
void updateGenerationStatistics(Generation_ptr generation){
	//Init
	generation->statistics.fitnessSum = 0;
	generation->statistics.fitnessSumInverse = 0;
	generation->statistics.invalidCount = 0;
	for(int i = 0; i < SIM_RESULT_COUNT; i++){
		generation->statistics.invalidTypeCount[i] = 0;
	}

	//Reset statistics working with all the population
	resetStatistic(&generation->statistics.lengthStat, generation->count);
	resetStatistic(&generation->statistics.genotypeSimilarityStat, generation->count);
	resetStatistic(&generation->statistics.genotypeAbsSimilarityStat, generation->count);

	for(int i = 0; i < generation->count; i++){
		generation->statistics.invalidTypeCount[generation->simOutputs[i].result]++;

		updateStatistic(&generation->statistics.lengthStat, generation->individuals[i].size, i);
		updateStatistic(&generation->statistics.genotypeSimilarityStat, generation->individuals[i].size - generation->statistics.best.size, i);
		updateStatistic(&generation->statistics.genotypeAbsSimilarityStat, fabs(generation->individuals[i].size - generation->statistics.best.size), i);

		if(generation->simOutputs[i].result != SIM_OK){
			generation->statistics.invalidCount++;
		}
		else{
			generation->statistics.fitnessSumInverse += 1.0 / generation->individuals[i].fitness;
			generation->statistics.fitnessSum = generation->individuals[i].fitness;
		}
	}

	//Reset statistics working with only the valid population
	resetStatistic(&generation->statistics.fitnessStat, generation->count - generation->statistics.invalidCount);
	resetStatistic(&generation->statistics.fitnessSimilarityStat, generation->count - generation->statistics.invalidCount);
	resetStatistic(&generation->statistics.fitnessAbsSimilarityStat, generation->count - generation->statistics.invalidCount);
	resetStatistic(&generation->statistics.fenotypeSimilarityStat, generation->count - generation->statistics.invalidCount);

	for(int i = 0; i < generation->count - generation->statistics.invalidCount; i++){
		updateStatistic(&generation->statistics.fitnessStat, generation->individuals[i].fitness, i);
		updateStatistic(&generation->statistics.fitnessSimilarityStat, generation->individuals[i].fitness - generation->statistics.best.fitness , i);
		updateStatistic(&generation->statistics.fitnessAbsSimilarityStat, fabs(generation->individuals[i].fitness - generation->statistics.best.fitness) , i);
		updateStatistic(&generation->statistics.fenotypeSimilarityStat, evalStrategySimilarity(&generation->individuals[i], &generation->statistics.best), i);

	}

	//Finalize the statistics
	finalizeStatistic(&generation->statistics.lengthStat);
	finalizeStatistic(&generation->statistics.genotypeSimilarityStat);
	finalizeStatistic(&generation->statistics.genotypeAbsSimilarityStat);
	finalizeStatistic(&generation->statistics.fitnessStat);
	finalizeStatistic(&generation->statistics.fitnessSimilarityStat);
	finalizeStatistic(&generation->statistics.fitnessAbsSimilarityStat);
	finalizeStatistic(&generation->statistics.fenotypeSimilarityStat);

	if(generation->statistics.fenotypeSimilarityStat.avg == NAN){
		assert(0);


	}

	//Save best
	if(generation->simOutputs[0].result == SIM_OK &&
			generation->individuals[0].fitness < generation->statistics.best.fitness){

		memcpy(&generation->statistics.best, &generation->individuals[0], sizeof(Strategy));
		memcpy(&generation->statistics.bestOutput, &generation->simOutputs[0], sizeof(SimulationOutput));

		generation->statistics.lastChange = 0;
	}
	else{
		generation->statistics.lastChange++;
	}
}








