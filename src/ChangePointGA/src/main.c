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
#include "test.h"


int main() {
	//Init random
	randInit();

	//Init maps
	initMap(&maps[0], -4800.0, 45.0, 0.0);
	initMap(&maps[1], -4800.0, 40.0, 0.0);
	initMap(&maps[2], -4800.0, 25.0, 0.0);
	initMap(&maps[3], -67500.0, 200.0, 0.0);

	/*
	initMap(&maps[0], -9500.0, 60.0, 0.0);
	initMap(&maps[1], -6100.0, 45.0, 0.0);
	initMap(&maps[2], -16000.0, 65.0, 0.0);
	initMap(&maps[3], -45000.0, 130.0, 0.0);
	*/

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
	unsigned long long generationTimer, timer;
	double generationTime, fitnessTime, sortTime, statTime, elitismTime, crossoverTime, mutationTime;

	//Loop
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
		evalGenerationFitness(currentGeneration, START_VELOCITY, START_MAP, energyTimeFitness);
		fitnessTime = getTimeElapsed(timer);

		//Sort individual by fitness
		timer = getTime();
		sortGenerationByFitness(currentGeneration);
		sortTime = getTimeElapsed(timer);

		//Calculate the statistics
		timer = getTime();
		updateGenerationStatistics(currentGeneration);
		statTime = getTimeElapsed(timer);

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

		//Prints
		if(generationCount % 10 == 0){
			//Get time
			generationTime = getTimeElapsed(generationTimer);

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
		}

		//Check commands
		if(kbhit()){
			char c = getchar();

			//Save
			if(c == 's'){
				strategyToFile(&currentGeneration->individuals[0], "best.csv");
			}

			//End
			if(c == 'e'){
				printf("Stop\n");
				break;
			}

			//Pause
			if(c == 'p'){
				printf("Press a key to resume...");
				while (getchar() == '\n');
			}
		}

		//Next generation
		swap(currentGeneration, nextGeneration);
		generationCount++;
	}

	//Save strategy
	strategyToFile(&currentGeneration->individuals[0], "best.csv");

	//Dispose generation
	disposeGeneration(currentGeneration);
	disposeGeneration(nextGeneration);

	//Dispose track samples
	disposeTrackData();

	return 0;
}

