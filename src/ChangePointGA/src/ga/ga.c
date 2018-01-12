/*
 * ga.c
 *
 *  Created on: Dec 25, 2017
 *      Author: davide
 */

#include "ga.h"
#include "../utils/uthash/uthash.h"

typedef struct functionReg{
	void* key;
	char name[16];
	UT_hash_handle hh;
}functionReg;

static functionReg* functions = NULL;

static void registerFunctions(void* functionPointer, const char* name){
	functionReg* reg = (functionReg*)malloc(sizeof(functionReg));
	reg->key = functionPointer;
	strcpy(reg->name, name);
	HASH_ADD_PTR(functions, key, reg);
}

static void findFunction(void* functionPointer, functionReg** reg){
	void* ptr = (void*)functionPointer;
	HASH_FIND_PTR(functions, &ptr, (*reg));
}

void initGA(GA* ga, SelectionFunction selectionFunction, CrossoverFunction crossoverFunction, FitnessFunction fitnessFunction){
	//Counter
	ga->generationCount = 0;

	//Mutations
	ga->mutationCount = 0;

	//Other function
	ga->selectionFunction = selectionFunction;
	ga->crossoverFunction = crossoverFunction;
	ga->fitnessFunction = fitnessFunction;

	//Init random first generation
	ga->currentGeneration = initRandomGeneration(POPULATION_SIZE);
	//Init empty next generation
	ga->nextGeneration = initEmptyGeneration(POPULATION_SIZE);

	//Selection functions
	registerFunctions(fitnessProportionalSelection, "fitProp");
	registerFunctions(rankSelection, "rank");
	registerFunctions(tournamentSelection, "tournament");


	//Fitness functions
	registerFunctions(energyFitness, "energy");
	registerFunctions(energyTimeFitness, "energy+time");
	registerFunctions(energyDiversityFitness, "energy+div");

	//Crossover functions
	registerFunctions(singlePointCrossover, "singlePoint");

	//Mutation functions
	registerFunctions(addRandomChangePoint, "addRandom");
	registerFunctions(removeRandomChangePoint, "removeRandom");
	registerFunctions(moveRandomChangePoint, "moveRandom");
	registerFunctions(changeRandomChangePointAction, "changeRandom");
	registerFunctions(filterStrategy, "filter");

}

void addMutation(GA* ga, MutationFunction mutation, float rate){
	if(ga->mutationCount < MAX_MUTATION_COUNT){
		ga->mutationRates[ga->mutationCount] = rate;
		ga->mutationsFunction[ga->mutationCount] = mutation;
		ga->mutationCount++;
	}
}

void genetic(GA* ga){
	//Counters
	int childCount = 0, mutationCount = 0;

	//Timers
	unsigned long long generationTimer, timer;
	double generationTime, fitnessTime, sortTime, statTime, elitismTime, crossoverTime, mutationTime;

	//Start timer
	generationTimer = getTime();

	//Eval fitness
	timer = getTime();
	evalGenerationFitness(ga->currentGeneration, START_VELOCITY, START_MAP, ga->fitnessFunction);
	fitnessTime = getTimeElapsed(timer);

	//Sort individual by fitness
	timer = getTime();
	sortGenerationByFitness(ga->currentGeneration);
	sortTime = getTimeElapsed(timer);

	//Calculate the statistics
	timer = getTime();
	updateGenerationStatistics(ga->currentGeneration);
	statTime = getTimeElapsed(timer);

	if (ga->currentGeneration->statistics.invalidCount / ga->currentGeneration->size > INVALID_THRESHOLD
			|| ga->currentGeneration->statistics.lastChange > MAX_LAST_CHANGE) {

		//Add new valid random strategy

	}

	//Perform crossover
	timer = getTime();
	childCount = crossOver(ga->currentGeneration, ga->nextGeneration, ga->selectionFunction, ga->crossoverFunction);
	crossoverTime = getTimeElapsed(timer);

	//Perform mutations
	timer = getTime();
	for(int i = 0; i < ga->mutationCount; i++){
		mutationCount += mutation(ga->nextGeneration, ga->mutationsFunction[i], ga->mutationRates[i]);
	}
	mutationTime = getTimeElapsed(timer);

	//Perform elitism selection
	timer = getTime();
	elitism(ga->currentGeneration, ga->nextGeneration, ELITISM_PERCENTAGE);
	FUSS(ga->currentGeneration, ga->nextGeneration);
	/*
	if(ga->generationCount % 100 == 0){
		filterStrategy(&ga->nextGeneration->individuals[0]);
	}
	*/
	elitismTime = getTimeElapsed(timer);

	//Prints
	if(ga->generationCount % 10 == 0){
		//Get time
		generationTime = getTimeElapsed(generationTimer);

		wclear(gaOutputWindow);
		//Print generation info
		wprintw(gaOutputWindow, "Gen %lu   individuals %d/%d   child %d   mutations %d\n",
				ga->generationCount,
				ga->currentGeneration->count,
				ga->currentGeneration->size,
				childCount,
				mutationCount
				);

		//Print best
		wprintw(gaOutputWindow, "Best energy %.2f   time %.2f/%.2f\n",
				ga->currentGeneration->statistics.best.simulation.energy,
				ga->currentGeneration->statistics.best.simulation.time,
				(float)TRACK_END_POINT / MIN_AVG_SPEED
				);

		//Print stats
		wprintw(gaOutputWindow, "Stat fmin %.2f   fmed %.2f   lavg %d   inv %.2f   sim %.2f   lch %lu\n",
				ga->currentGeneration->statistics.fitnessMin,
				ga->currentGeneration->statistics.fitnessMedian,
				(int)ga->currentGeneration->statistics.lengthAvg,
				(float)ga->currentGeneration->statistics.invalidCount / ga->currentGeneration->count,
				ga->currentGeneration->statistics.similarityAvg,
				ga->currentGeneration->statistics.lastChange
				);

		//Print time
		wprintw(gaOutputWindow, "Time ft %0.3lf   st %0.3lf   ut %0.3lf   et %0.3lf   ct %0.3lf   mt %0.3lf\n",
				fitnessTime,
				sortTime,
				statTime,
				elitismTime,
				crossoverTime,
				mutationTime
				);
		wprintw(gaOutputWindow, "Total time %lf\n", generationTime);
		wprintw(gaOutputWindow, "==========================================================================================\n");
		wrefresh(gaOutputWindow);

	}

	//Next generation
	swap(ga->currentGeneration->individuals, ga->nextGeneration->individuals);
	ga->nextGeneration->count = 0;
	ga->generationCount++;

}

void disposeGA(GA* ga){
	//Dispose generation
	disposeGeneration(ga->currentGeneration);
	disposeGeneration(ga->nextGeneration);
}

void printGAParams(GA* ga){
	functionReg reg;
	functionReg* regPtr = &reg;

	wclear(gaParamWindow);
	box(gaParamWindow,0,0);

	int row = 0;
	mvwprintw(gaParamWindow, row++, 1, "GA params");

	findFunction(ga->fitnessFunction, &regPtr);
	mvwprintw(gaParamWindow, row++, 1, "Fitness: %s", regPtr->name);

	findFunction(ga->selectionFunction, &regPtr);
	mvwprintw(gaParamWindow, row++, 1, "Selection: %s", regPtr->name);

	findFunction(ga->crossoverFunction, &regPtr);
	mvwprintw(gaParamWindow, row++, 1, "Crossover: %s", regPtr->name);

	mvwprintw(gaParamWindow, row++, 1, "Mutations:");
	for(int i = 0; i < ga->mutationCount; i++){
		findFunction(ga->mutationsFunction[i], &regPtr);
		mvwprintw(gaParamWindow, row++, 1, " - %-20s   %-10.2f", regPtr->name, ga->mutationRates[i]);
	}

	wrefresh(gaParamWindow);
}
