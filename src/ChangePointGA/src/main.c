/*
 ============================================================================
 Name        : main.c
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   : 
 Description : ChangePoint GA
 ============================================================================
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "utils/sysutils.h"

//Debug define
//#define CHECK_GENERATIONS
#include "test.h"

#include "ga/ga.h"

int main() {
	randInit();

	initMap(&maps[0], -4800.0, 45.0, 0.0);
	initMap(&maps[1], -4800.0, 40.0, 0.0);
	initMap(&maps[2], -4800.0, 25.0, 0.0);
	initMap(&maps[3], -67500.0, 200.0, 0.0);


	//Init random first generation
	Generation_ptr currentGeneration = initRandomGeneration(POPULATION_SIZE);
	//Init empty next generation
	Generation_ptr nextGeneration =  initEmptyGeneration(POPULATION_SIZE);

	unsigned long long generationTimer, timer;
	unsigned long int generationCount = 0;
	double generationTime, fitnessTime, sortTime, statTime, elitismTime, crossoverTime, mutationTime;

	//for(int gen = 0; gen < 3; gen++){
	while(1){
		//Start timer
		generationTimer = getTime();

		//Test
		#ifdef CHECK_GENERATIONS
			if(!checkGeneration(currentGeneration)){
				exit(1);
			}
		#endif

		//Eval fitness
		timer = getTime();
		evalGenerationFitness(currentGeneration, START_VELOCITY, START_MAP);
		fitnessTime = getTimeElapsed(timer);

		//Sort individual by fitness
		timer = getTime();
		sortGenerationByFitness(currentGeneration);
		sortTime = getTimeElapsed(timer);

		//Calculate the statistics
		timer = getTime();
		updateGenerationStatistics(currentGeneration);
		statTime = getTimeElapsed(timer);

		//Print stats
		printf("Gen %lu   fmin %.2f   fmed %.2f   lavg %d   inv %.2f\n",
				generationCount,
				currentGeneration->statistics.fitnessMin,
				currentGeneration->statistics.fitnessMedian,
				(int)currentGeneration->statistics.lengthAvg,
				(float)currentGeneration->statistics.invalidCount / currentGeneration->count
				);

		//Perform elitsm selection
		timer = getTime();
		elitism(currentGeneration, nextGeneration, ELITISM_PERCENTAGE);
		elitismTime = getTimeElapsed(timer);

		//Perform crossover
		timer = getTime();
		crossOver(currentGeneration, nextGeneration, fitnessProportionalSelection, singlePointCrossover);
		crossoverTime = getTimeElapsed(timer);

		//Perform mutations
		timer = getTime();
		mutation(nextGeneration, addRandomChangePoint, 			ADD_POINT_MUTATION_RATE);
		mutation(nextGeneration, removeRandomChangePoint, 		REMOVE_POINT_MUTATION_RATE);
		mutation(nextGeneration, moveRandomChangePoint, 		CHANGE_POINT_POS_MUTATION_RATE);
		mutation(nextGeneration, changeRandomChangePointAction, CHANGE_POINT_ACT_MUTATION_RATE);
		mutationTime = getTimeElapsed(timer);

		//Get time
		generationTime = getTimeElapsed(generationTimer);
		//Print time %
		printf("ft %lf   st %lf   ut %lf   et %lf   ct %lf   mt %lf\n",
				fitnessTime / generationTime,
				sortTime / generationTime,
				statTime / generationTime,
				elitismTime / generationTime,
				crossoverTime / generationTime,
				mutationTime / generationTime
				);
		printf("Total time %lf\n", generationTime);
		printf("==============================\n");

		//Next generation
		swap(currentGeneration, nextGeneration);
		generationCount++;

		//Check stop
		if(kbhit()){
			char c = getchar();
			if(c == 's'){
				strategyToCsv(&currentGeneration->individuals[0], "best.csv");
			}

			if(c == 'e'){
				printf("Stop\n");
				break;
			}
		}
	}

	FILE* bestStrategyFile = fopen("str.bin", "wb");
	fwrite(&currentGeneration->individuals[0].simulation, sizeof(Simulation), 1 , bestStrategyFile);
	fclose(bestStrategyFile);

	simulationToCsv(&currentGeneration->individuals[0].simulation, "best.csv");

	disposeGeneration(currentGeneration);
	disposeGeneration(nextGeneration);

	return 0;
}

