/*
 * commands.c
 *
 *  Created on: Jan 12, 2018
 *      Author: davide
 */
#include "commands.h"

char errorBuffer[80];
char** argv = NULL;

void initArgv(){
	argv = (char**)malloc(sizeof(char*) * CMD_PARAM_COUNT);
	for(int i = 0; i < CMD_PARAM_COUNT; i++){
		argv[i] = (char*)malloc(sizeof(char) * CMD_PARAM_SIZE);
	}
}

void disposeArgv(){
	for(int i = 0; i < CMD_PARAM_COUNT; i++){
		free(argv[i]);
	}

	free(argv);
}

int setMutationRate(GA* ga, char** argv, int argc){
	//Default params
	int mutationIndex = -1;
	float newRate = -1;

	//Parsing
	int c;
	char* end;

	while ((c = getopt(argc, argv, "m:r:")) != -1){
		switch (c) {
			case 'm':
				mutationIndex = strtol(optarg, &end, 10);

				if(mutationIndex < 0 || mutationIndex > ga->mutationCount - 1){
					sprintf(errorBuffer, "Invalid mutation index %d", mutationIndex);
					return 1;
				}

				break;

			case 'r':
				newRate = strtof(optarg, &end);

				if(newRate < 0 || newRate > 1){
					sprintf(errorBuffer, "Invalid mutation rate %.2f", newRate);
					return 1;
				}

				break;

			default:
				if (optopt == 'm' || optopt == 'r'){
					sprintf(errorBuffer, "Option -%c requires an argument", optopt);
				}
				else if (isprint(optopt)){
					sprintf(errorBuffer, "Unknown option `-%c'", optopt);
				}
				else{
					sprintf(errorBuffer, "Unknown option character `\\x%x'", optopt);
				}

				return 1;
		}
	}

	if(argc < 2 || mutationIndex == -1 || newRate < 0){
		sprintf(errorBuffer, "Missing parameters");
		return 1;
	}
	else{
		ga->mutationRates[mutationIndex] = newRate;
		printGAParams(ga);
	}


	return 0;
}

int setFitnessFunction(GA* ga, char** argv, int argc){

	return 0;
}

int setSelectionFunction(GA* ga, char** argv, int argc){

	return 0;
}

int applyFilterToStrategy(GA* ga, char** argv, int argc){
	//Default params
	int simIndex = -1;

	//Parsing
	int c;
	char* end;

	while ((c = getopt(argc, argv, "i:")) != -1){
		switch (c) {
			case 'i':
				simIndex = strtol(optarg, &end, 10);

				if(simIndex < 0 || simIndex > ga->currentGeneration->count - 1){
					sprintf(errorBuffer, "Invalid index %d", simIndex);
					return 1;
				}
				break;

			default:
				if (optopt == 'i'){
					sprintf(errorBuffer, "Option -%c requires an argument", optopt);
				}
				else if (isprint(optopt)){
					sprintf(errorBuffer, "Unknown option `-%c'", optopt);
				}
				else{
					sprintf(errorBuffer, "Unknown option character `\\x%x'", optopt);
				}

				return 1;
		}
	}

	if(argc < 1 || simIndex == -1){
		sprintf(errorBuffer, "Missing parameters");
		return 1;
	}
	else{
		filterStrategy(&ga->currentGeneration->individuals[simIndex]);

		simulateStrategy(&ga->currentGeneration->individuals[simIndex], START_VELOCITY, START_MAP, 1);
		ga->currentGeneration->individuals[simIndex].similarity = evalStrategySimilarity(&ga->currentGeneration->individuals[simIndex], &ga->currentGeneration->statistics.best);
		ga->currentGeneration->individuals[simIndex].fitness = ga->fitnessFunction(ga->currentGeneration, &ga->currentGeneration->individuals[simIndex]);

		printExplorerWindow(ga, simIndex);
	}
	return 0;
}


int plotSimulation(GA* ga, char** argv, int argc){
	//Default params
	int simIndex = -1;

	//Parsing
	int c;
	char* end;

	while ((c = getopt(argc, argv, "i:")) != -1){
		switch (c) {
			case 'i':
				simIndex = strtol(optarg, &end, 10);

				if(simIndex < 0 || simIndex > ga->currentGeneration->count - 1){
					sprintf(errorBuffer, "Invalid index %d", simIndex);
					return 1;
				}

				break;

			default:
				if (optopt == 'i'){
					sprintf(errorBuffer, "Option -%c requires an argument", optopt);
				}
				else if (isprint(optopt)){
					sprintf(errorBuffer, "Unknown option `-%c'", optopt);
				}
				else{
					sprintf(errorBuffer, "Unknown option character `\\x%x'", optopt);
				}

				return 1;
		}
	}

	if(argc < 1 || simIndex == -1){
		sprintf(errorBuffer, "Missing parameters");
		return 1;
	}
	else{
		double points[SIM_STEP_COUNT];
		gnuplot_ctrl* handle = gnuplot_init();
		gnuplot_setstyle(handle, "lines") ;
		gnuplot_set_xlabel(handle, "Track pos");
		gnuplot_set_xlabel(handle, "Map");
		for (int i = 0 ; i < SIM_STEP_COUNT; i++) {
			points[i] = ga->currentGeneration->individuals[simIndex].simulation.steps[i].map;
		}
		gnuplot_plot_x(handle, points, SIM_STEP_COUNT, "Map");

	}
	return 0;
}


void printExplorerWindow(GA* ga, int index){
	int row = 0;
	wclear(explorerWindow);
	box(explorerWindow, 0, 0);

	mvwprintw(explorerWindow, row++, 1, "Strategy %d", index);
	mvwprintw(explorerWindow, row++, 1, "Energy %.2f", ga->currentGeneration->individuals[index].simulation.energy);
	mvwprintw(explorerWindow, row++, 1, "Time %.2f", ga->currentGeneration->individuals[index].simulation.time);
	mvwprintw(explorerWindow, row++, 1, "Fitness %.2f", ga->currentGeneration->individuals[index].fitness);
	mvwprintw(explorerWindow, row++, 1, "Length %d", ga->currentGeneration->individuals[index].size);
	mvwprintw(explorerWindow, row++, 1, "Similarity %0.2f",ga->currentGeneration->individuals[index].similarity);
	mvwprintw(explorerWindow, row++, 1, "Valid %d", ga->currentGeneration->individuals[index].simulation.result);
	mvwprintw(explorerWindow, row++, 1, "End vel %.2f", ga->currentGeneration->individuals[index].simulation.velocity);

	wrefresh(explorerWindow);
}
