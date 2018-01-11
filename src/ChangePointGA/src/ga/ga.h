/*
 ============================================================================
 Name        : population.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : GA params
 ============================================================================
 */

#ifndef GA_H_
#define GA_H_

#include "generation.h"
#include "../utils/sysutils.h"

#define POPULATION_SIZE		200
#define MAX_MUTATION_COUNT	10

#define ELITISM_PERCENTAGE	0.01f

#define ADD_POINT_MUTATION_RATE 		0.15f
#define REMOVE_POINT_MUTATION_RATE 		0.15f
#define CHANGE_POS_MUTATION_RATE 		0.15f
#define CHANGE_ACT_MUTATION_RATE		0.15f
#define FILTER_MUTATION_RATE			0.05f

#define INVALID_THRESHOLD 	0.40
#define MAX_LAST_CHANGE		1000

//Save define
#define SAVE_STATISTICS
#define BEST_FILE			"best.csv"
#define GENERATION_FILE		"generation.bin"
#define SIMULATION_FILE		"simparam.txt"
#define STATISTICS_FILE		"statistics.csv"

typedef struct GA{
	//Counter
	unsigned long int generationCount;

	//Mutations
	int mutationCount;
	float mutationRates[MAX_MUTATION_COUNT];
	MutationFunction mutationsFunction[MAX_MUTATION_COUNT];

	//Other function
	SelectionFunction selectionFunction;
	CrossoverFunction crossoverFunction;
	FitnessFunction fitnessFunction;


	//Generations
	Generation_ptr currentGeneration;
	Generation_ptr nextGeneration;
}GA;

//Init GA struct
extern void initGA(GA* ga, SelectionFunction selectionFunction, CrossoverFunction crossoverFunction, FitnessFunction fitnessFunction);

//Add mutation to the list
extern void addMutation(GA* ga, MutationFunction mutation, float rate);

//Run a generation
extern void genetic(GA* ga);

//Dispose GA struct
extern void disposeGA(GA* ga);

#endif
