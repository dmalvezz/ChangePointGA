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

#define ELITISM_PERCENTAGE	0.01f

#define ADD_POINT_MUTATION_RATE 		0.75f
#define REMOVE_POINT_MUTATION_RATE 		0.75f
#define CHANGE_POINT_POS_MUTATION_RATE 	0.75f
#define CHANGE_POINT_ACT_MUTATION_RATE	0.75f

#define INVALID_THRESHOLD 	0.40
#define MAX_LAST_CHANGE		1000

#define SAVE_STATISTICS
#define BEST_FILE			"best.csv"
#define GENERATION_FILE		"generation.bin"
#define SIMULATION_FILE		"simparam.txt"
#define STATISTICS_FILE		"statistics.csv"

typedef struct GA{
	//Counter
	unsigned long int generationCount;

	//Generations
	Generation_ptr currentGeneration;
	Generation_ptr nextGeneration;
}GA;

extern void initGA(GA* ga);

extern void genetic(GA* ga);

extern void disposeGA(GA* ga);

#endif
