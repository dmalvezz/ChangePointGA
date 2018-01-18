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
#include <stdio.h>

//Use ncurses windows
#define USE_WINDOWS

#include "ga/ga.h"
#include "window/console.h"

//Main init
static void mainInit();

//Main dispose
static void mainDispose();


void plot(Strategy_ptr str, const char*title){
	double points[SIM_STEP_COUNT];
	gnuplot_ctrl* handle = gnuplot_init();
	gnuplot_setstyle(handle, "lines") ;
	gnuplot_set_xlabel(handle, "Track pos");
	gnuplot_set_xlabel(handle, title);
	for (int i = 0 ; i < SIM_STEP_COUNT; i++) {
		points[i] = str->simulation.steps[i].map;
	}
	gnuplot_plot_x(handle, points, SIM_STEP_COUNT, "Map");
}

int main() {
	//Loop flag
	int loop = 1;

	#ifdef SAVE_STATISTICS
		//Create statistics file
		FILE* statisticsFile = fopen(STATISTICS_FILE, "at");
		fprintf(statisticsFile, "generation, energyBest, fitnessBest, fitnessMin, fitnessMax, fitnessMedian, fitnessAvg, lenghtAvg, similarityAvg, %%invalid, lastChange\n");

		FILE* bestFile = fopen("bestSim.csv", "at");
		float currBest = INFINITY;
	#endif

	mainInit();

	//Init GA
	GA ga;
	initGA(&ga, fitnessProportionalSelection, singlePointCrossover, energyFitness);
	addMutation(&ga, addRandomChangePoint, 			ADD_POINT_MUTATION_RATE);
	addMutation(&ga, removeRandomChangePoint, 		REMOVE_POINT_MUTATION_RATE);
	addMutation(&ga, moveRandomChangePoint, 		CHANGE_POS_MUTATION_RATE);
	addMutation(&ga, changeRandomChangePointAction, CHANGE_ACT_MUTATION_RATE);
	addMutation(&ga, filterStrategy, 				FILTER_MUTATION_RATE);

	#ifdef USE_WINDOWS
		printGAParams(&ga);
		printSimulationParams();
	#endif

	//Loop
	while(loop){
		//Apply genetics
		genetic(&ga);

		#ifdef SAVE_STATISTICS
			//Save statistic
			statisticsToFile(ga.currentGeneration, ga.generationCount, statisticsFile);

			if(currBest > ga.currentGeneration->statistics.best.fitness){
				currBest = ga.currentGeneration->statistics.best.fitness;
				for(int i = 0; i < SIM_STEP_COUNT; i++){
					fprintf(bestFile, "%f,", ga.currentGeneration->statistics.best.simulation.steps[i].ftraction);
				}
				fprintf(bestFile, "\n");
			}
		#endif

		#ifdef USE_WINDOWS
			//Update console
			updateConsole(&ga, &loop);
		#endif
	}

	//Save strategy and generation
	strategyToFile(&ga.currentGeneration->statistics.best, BEST_FILE);
	generationToFile(ga.currentGeneration, GENERATION_FILE);

	//Dispose GA
	disposeGA(&ga);

	#ifdef SAVE_STATISTICS
		//Close statistics file
		fclose(statisticsFile);
		fclose(bestFile);
	#endif

	mainDispose();

	return 0;
}


void mainInit(){
	#ifdef USE_WINDOWS
		//Init windows
		initWindows();
		initConsole();
	#endif

	//Init random
	randInit();

	//Init mapset
	initMapset();

	//Init track samples
	generateTrackData(SPACE_STEP);
}

void mainDispose(){
	//Dispose track samples
	disposeTrackData();

	#ifdef USE_WINDOWS
		//Dispose windows
		disposeConsole();
		disposeWindows();
	#endif

}
