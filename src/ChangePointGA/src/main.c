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

#include "ga/ga.h"
#include "utils/sysutils.h"

//Debug define
//#define CHECK_GENERATIONS
//#define CHECK_BEST
#define SAVE_STATISTICS
#include "test.h"


void execCommands(Generation_ptr currentGeneration, int* loop);

int main() {
	//Init random
	randInit();

	//Init mapset
	initMapset();

	//Init track samples
	generateTrackData(SPACE_STEP);

#ifdef CHECK_BEST
	testStrategy();
	return 0;
#endif

	//Init random first generation
	Generation_ptr currentGeneration = initRandomGeneration(POPULATION_SIZE);
	//Init empty next generation
	Generation_ptr nextGeneration = initEmptyGeneration(POPULATION_SIZE);

	//Counter
	unsigned long int generationCount = 0;

	//Timers
	int loop = 1;
	unsigned long long generationTimer, timer;
	double generationTime, fitnessTime, sortTime, statTime, elitismTime, crossoverTime, mutationTime;

#ifdef SAVE_STATISTICS
	FILE* statisticsFile = fopen("statistics.csv", "wt");
	fprintf(statisticsFile, "generation, energyBest,fitnessBest, fitnessMin, fitnessMedian, lenghtAvg, similarityAvg, %%invalid\n");
#endif

	//Loop
	while(loop){
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
		evalGenerationFitness(currentGeneration, START_VELOCITY, START_MAP, energyTimeFitness);
		fitnessTime = getTimeElapsed(timer);

		//Sort individual by fitness
		timer = getTime();
		sortGenerationByFitness(currentGeneration);
		sortTime = getTimeElapsed(timer);

		//Calculate the statistics
		timer = getTime();
		updateGenerationStatistics(currentGeneration);
#ifdef SAVE_STATISTICS
		statisticsToFile(currentGeneration, generationCount, statisticsFile);
#endif
		statTime = getTimeElapsed(timer);

		//Perform elitism selection
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

		//Prints
		if(generationCount % 10 == 0){
			//Get time
			generationTime = getTimeElapsed(generationTimer);

			//Print best
			printf("Best energy %.2f   time %.2f/%.2f\n",
					currentGeneration->individuals[0].simulation.energy,
					currentGeneration->individuals[0].simulation.time,
					(float)TRACK_LENGTH / MIN_AVG_SPEED
					);

			//Print stats
			printf("Gen %lu   fmin %.2f   fmed %.2f   lavg %d   inv %.2f   sim %.2f\n",
					generationCount,
					currentGeneration->statistics.fitnessMin,
					currentGeneration->statistics.fitnessMedian,
					(int)currentGeneration->statistics.lengthAvg,
					(float)currentGeneration->statistics.invalidCount / currentGeneration->count,
					currentGeneration->statistics.similarityAvg
					);

			//Print time
			printf("ft %0.3lf   st %0.3lf   ut %0.3lf   et %0.3lf   ct %0.3lf   mt %0.3lf\n",
					fitnessTime / generationTime,
					sortTime / generationTime,
					statTime / generationTime,
					elitismTime / generationTime,
					crossoverTime / generationTime,
					mutationTime / generationTime
					);
			printf("Total time %lf\n", generationTime);
			printf("==============================\n");

		#ifdef SAVE_STATISTICS
			fflush(statisticsFile);
		#endif
		}

		//Check commands
		execCommands(currentGeneration, &loop);

		//Next generation
		swap(currentGeneration, nextGeneration);
		generationCount++;
	}

	//Save strategy and generation
	strategyToFile(&currentGeneration->statistics.best, "best.csv");
	generationToFile(currentGeneration, "generation.bin");
	saveSimulationParams("simparam.txt");
#ifdef SAVE_STATISTICS
	fclose(statisticsFile);
#endif

	//Dispose generation
	disposeGeneration(currentGeneration);
	disposeGeneration(nextGeneration);

	//Dispose track samples
	disposeTrackData();

	return 0;
}


void execCommands(Generation_ptr currentGeneration, int* loop){
	if(kbhit()){
		char key = getchar();

		switch (key){

			case 's':
				//Save
				strategyToFile(&currentGeneration->statistics.best, "best.csv");
				generationToFile(currentGeneration, "generation.bin");
				saveSimulationParams("simparam.txt");
				break;

			case 'e':
				//End
				(*loop) = 0;
				printf("Stop\n");
				break;

			case 'p':
				//Pause
				printf("Press a key to resume...");
				while (getchar() == '\n');
				break;

			case 'l':
				//Load


				break;
		}
	}
}

