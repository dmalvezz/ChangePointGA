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
#include <time.h>
#include <omp.h>


#include "ga/ga.h"

//Debug define
//#define CHECK_GENERATIONS
//#define CHECK_BEST
#include "test.h"
#include "window/console.h"


int main() {
	//Loop flag
	int loop = 1;

#ifdef SAVE_STATISTICS
	//Create statistics file
	FILE* statisticsFile = fopen(STATISTICS_FILE, "wt");
	fprintf(statisticsFile, "generation, energyBest, fitnessBest, fitnessMin, fitnessMax, fitnessMedian, fitnessAvg, lenghtAvg, similarityAvg, %%invalid, lastChange\n");
#endif

	//Init windows
	initWindows();
	initConsole();

	//Init random
	randInit();

	//Init mapset
	initMapset();

	//Init track samples
	generateTrackData(SPACE_STEP);

	//Init GA
	GA ga;
	initGA(&ga, fitnessProportionalSelection, singlePointCrossover, energyFitness);
	addMutation(&ga, addRandomChangePoint, 			ADD_POINT_MUTATION_RATE);
	addMutation(&ga, removeRandomChangePoint, 		REMOVE_POINT_MUTATION_RATE);
	addMutation(&ga, moveRandomChangePoint, 		CHANGE_POS_MUTATION_RATE);
	addMutation(&ga, changeRandomChangePointAction, CHANGE_ACT_MUTATION_RATE);
	addMutation(&ga, filterStrategy, 				FILTER_MUTATION_RATE);
	printGAParams(&ga);

	/*
	generationFromFile(ga.currentGeneration, GENERATION_FILE);
	filterStrategy(&ga.currentGeneration->individuals[0]);
	simulateStrategy(&ga.currentGeneration->individuals[0], START_VELOCITY, START_MAP);
	strategyToFile(&ga.currentGeneration->individuals[0], BEST_FILE);
	return 0;
	*/


	printSimulationParams();
	//Loop
	while(loop){
		//Apply genetics
		genetic(&ga);

		//Save statistic
		#ifdef SAVE_STATISTICS
			statisticsToFile(ga.currentGeneration, ga.generationCount, statisticsFile);
		#endif

		updateConsole(&ga, &loop);
	}

	//Save strategy and generation
	strategyToFile(&ga.currentGeneration->statistics.best, BEST_FILE);
	//strategyToFile(&ga.currentGeneration->individuals[0], BEST_FILE);
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

	//Dispose windows
	disposeConsole();
	disposeWindows();

	return 0;
}


