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

//Debug define
//#define CHECK_GENERATIONS
//#define CHECK_BEST
#include "test.h"


void execCommands(GA* ga, int* loop);

int main() {
	//Loop flag
	int loop = 1;

#ifdef SAVE_STATISTICS
	//Create statistics file
	FILE* statisticsFile = fopen(STATISTICS_FILE, "wt");
	fprintf(statisticsFile, "generation, energyBest, fitnessBest, fitnessMin, fitnessMax, fitnessMedian, fitnessAvg, lenghtAvg, similarityAvg, %%invalid, lastChange\n");
#endif

	//Init random
	randInit();

	//Init mapset
	initMapset();

	//Init track samples
	generateTrackData(SPACE_STEP);

	GA ga;
	initGA(&ga, rankSelection, singlePointCrossover, energyFitness);
	addMutation(&ga, addRandomChangePoint, 			ADD_POINT_MUTATION_RATE);
	addMutation(&ga, removeRandomChangePoint, 		REMOVE_POINT_MUTATION_RATE);
	addMutation(&ga, moveRandomChangePoint, 			CHANGE_POS_MUTATION_RATE);
	addMutation(&ga, changeRandomChangePointAction, 	CHANGE_ACT_MUTATION_RATE);
	addMutation(&ga, filterStrategy, 				FILTER_MUTATION_RATE);


	/*
	generationFromFile(ga.currentGeneration, GENERATION_FILE);
	filterStrategy(&ga.currentGeneration->individuals[0]);
	simulateStrategy(&ga.currentGeneration->individuals[0], START_VELOCITY, START_MAP);
	strategyToFile(&ga.currentGeneration->individuals[0], BEST_FILE);
	return 0;
	*/

	//Loop
	while(loop){
		//Apply genetics
		genetic(&ga);

		//Check commands
		execCommands(&ga, &loop);

		//Save statistic
		#ifdef SAVE_STATISTICS
			statisticsToFile(ga.currentGeneration, ga.generationCount, statisticsFile);
		#endif
	}

	//Save strategy and generation
	strategyToFile(&ga.currentGeneration->statistics.best, BEST_FILE);
	generationToFile(ga.currentGeneration, GENERATION_FILE);
	saveSimulationParams(SIMULATION_FILE);

	//Dispose GA
	disposeGA(&ga);

	//Dispose track samples
	disposeTrackData();

#ifdef SAVE_STATISTICS
	//Close statistics file
	fclose(statisticsFile);
#endif

	return 0;
}


void execCommands(GA* ga, int* loop){
	if(kbhit()){
		char key = getchar();

		switch (key){

			case 's':
				//Save
				printf("Saving...\n");
				strategyToFile(&ga->currentGeneration->statistics.best, BEST_FILE);
				generationToFile(ga->currentGeneration, GENERATION_FILE);
				saveSimulationParams(SIMULATION_FILE);
				break;

			case 'e':
				//End
				(*loop) = 0;
				printf("Stop\n");
				break;

			case 'p':
				//Pause
				printf("Press a key to resume...\n");
				while (getchar() == '\n');
				break;

			case 'l':
				//Load
				printf("Loading generation from file...\n");
				generationFromFile(ga->currentGeneration, GENERATION_FILE);
				break;
		}
	}
}

