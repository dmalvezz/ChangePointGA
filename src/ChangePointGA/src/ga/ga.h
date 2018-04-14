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

//Population
#define POPULATION_SIZE		100

//Percentage of individuals to keep
#define ELITISM_PERCENTAGE	0.01f

//Mutations rates
#define MAX_MUTATION_COUNT				10
#define ADD_POINT_MUTATION_RATE 		0.15f
#define REMOVE_POINT_MUTATION_RATE 		0.15f
#define CHANGE_POS_MUTATION_RATE 		0.15f
#define CHANGE_ACT_MUTATION_RATE		0.15f
#define FILTER_MUTATION_RATE			0.0f

#define INVALID_THRESHOLD 	0.40
#define MAX_LAST_CHANGE		1000

//Save define
#define SAVE_STATISTICS
#define SAVE_TIMES
//#define SAVE_PROFILES
#define SEED_DIR			"seed"
#define BEST_FILE			"best"
#define GENERATION_FILE		"generation.bin"
#define PROFILES_FILE		"profiles.bin"
#define SIMULATION_FILE		"simparam.txt"
#define GA_FILE				"gaparam.txt"
#define STATISTICS_FILE		"statistics.csv"
#define TIME_FILE			"time.csv"

//GA
typedef struct GA{
	unsigned long int generationCount;				//Generation counter

	//Mutations
	int mutationCount;											//Registered mutations count
	float mutationRates[MAX_MUTATION_COUNT];					//Registered mutations  rates
	MutationFunction mutationsFunction[MAX_MUTATION_COUNT];		//Registered mutations functions

	//Other function
	SelectionFunction selectionFunction;	//Selection function
	CrossoverFunction crossoverFunction;	//Crossover function
	FitnessFunction fitnessFunction;		//Fitness function

	//Generations
	Generation_ptr currentGeneration;		//Current generation population
	Generation_ptr nextGeneration;			//Used as temp buffer to create the new generation
}GA;

//Init GA struct
extern void initGA(GA* ga, SelectionFunction selectionFunction, CrossoverFunction crossoverFunction, FitnessFunction fitnessFunction);

//Add mutation to the list
extern void addGAMutation(GA* ga, MutationFunction mutation, float rate);

//Dispose GA struct
extern void disposeGA(GA* ga);

//Print GA params
extern void printGAParams(GA* ga);

//Save GA params on file
extern void saveGAParams(GA* ga, const char* fileName);

#endif
