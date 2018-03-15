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

static functionReg* functionsList = NULL;

static void registerFunctions(void* functionPointer, const char* name){
	functionReg* reg = (functionReg*)malloc(sizeof(functionReg));
	reg->key = functionPointer;
	strcpy(reg->name, name);
	HASH_ADD_PTR(functionsList, key, reg);
}

static void findFunction(void* functionPointer, functionReg** reg){
	void* ptr = (void*)functionPointer;
	HASH_FIND_PTR(functionsList, &ptr, (*reg));
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
	registerFunctions(linearRankWithPressureSelection, "linearRank");
	registerFunctions(tournamentSelection, "tournament");

	//Fitness functions
	registerFunctions(energyFitness, "energy");
	registerFunctions(energySquaredFitness, "energy^2");
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

void addGAMutation(GA* ga, MutationFunction mutation, float rate){
	if(ga->mutationCount < MAX_MUTATION_COUNT){
		ga->mutationRates[ga->mutationCount] = rate;
		ga->mutationsFunction[ga->mutationCount] = mutation;
		ga->mutationCount++;
	}
}

void disposeGA(GA* ga){
	//Dispose generation
	disposeGeneration(ga->currentGeneration);
	disposeGeneration(ga->nextGeneration);

	//Dispose hash
	HASH_CLEAR(hh, functionsList);
}

void printGAParams(GA* ga){
	/*
	functionReg* regPtr;

	wclear(gaParamWindow);
	box(gaParamWindow, 0, 0);

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
		mvwprintw(gaParamWindow, row++, 1, " %d) %-20s   %-10.2f", i, regPtr->name, ga->mutationRates[i]);
	}

	wrefresh(gaParamWindow);
	*/
}

void saveGAParams(GA* ga, const char* fileName){
	functionReg* regPtr;
	FILE* file = fopen(fileName, "wt");

	findFunction(ga->fitnessFunction, &regPtr);
	fprintf(file, "Fitness: %s\n", regPtr->name);

	findFunction(ga->selectionFunction, &regPtr);
	fprintf(file, "Selection: %s\n", regPtr->name);

	findFunction(ga->crossoverFunction, &regPtr);
	fprintf(file, "Crossover: %s\n", regPtr->name);

	fprintf(file, "Mutations:\n");
	for(int i = 0; i < ga->mutationCount; i++){
		findFunction(ga->mutationsFunction[i], &regPtr);
		fprintf(file, " %d) %-20s   %-10.2f\n", i, regPtr->name, ga->mutationRates[i]);
	}

	fprintf(file, "Population size: %d\n", ga->currentGeneration->count);
	fprintf(file, "Strategy size: %d - %d\n", MIN_CHANGE_POINT, MAX_CHANGE_POINT);

	fclose(file);
}
