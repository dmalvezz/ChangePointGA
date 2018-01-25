/*
 ============================================================================
 Name        : generation.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : GA generation
 ============================================================================
 */

#ifndef POPULATION_H
#define POPULATON_H

#include <math.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>

#include "statistic.h"
#include "strategy.h"


//MPI
#define SLAVE_CMD_TAG				1
#define SLAVE_SIMULATE_STRAT_CMD	's'
#define SLAVE_QUIT_CMD				'q'

typedef struct Statistics{
	int invalidCount;
	int invalidTypeCount[SIM_RESULT_COUNT];

	unsigned long int lastChange;
	float fitnessSumInverse;

	/*
	float fitnessMax;
	float fitnessMin;
	float fitnessMedian;
	float fitnessAvg;

	float lengthAvg;
	float similarityAvg;
	*/

	Statistic lengthStat;
	Statistic fitnessStat;
	Statistic fitnessSimilarityStat;
	Statistic fenotypeSimilarityStat;
	Statistic genotypeSimilarityStat;

	Strategy best;
}Statistics;

typedef struct Generation{
	int count;							//Current num of individuals
	int size;							//Max num of individuals
	Strategy_ptr individuals;			//Individuals array
	Statistics statistics;				//Population statistics
}Generation;

typedef Generation* Generation_ptr;

typedef float (*FitnessFunction)(Generation_ptr generation, Strategy_ptr strategy);
typedef int (*SelectionFunction)(Generation_ptr generation);
typedef void (*CrossoverFunction)(Strategy_ptr parent1, Strategy_ptr parent2, Strategy_ptr child1, Strategy_ptr child2, int cut);
typedef void (*MutationFunction)(Strategy_ptr strategy);

/*****Init functions*****/
//Initialize a empty population
extern Generation_ptr initEmptyGeneration(int size);

//Initialize a random population
extern Generation_ptr initRandomGeneration(int size);

//Dispose generation
extern void disposeGeneration(Generation_ptr generation);

//Print population individuals
extern void printGeneration(Generation_ptr generation);

//Save the generation to file
extern void generationToFile(Generation_ptr generation, const char* fileName);

//Load the generation from file
extern void generationFromFile(Generation_ptr generation, const char* fileName);

//Save statistics to file
extern void statisticsToFile(Generation_ptr generation, unsigned long int generationCount, FILE* file);


/*****Fitness eval functions*****/
//Pure energy based fitness
extern float energyFitness(Generation_ptr generation, Strategy_ptr strategy);

//Energy and time fitness
extern float energyTimeFitness(Generation_ptr generation, Strategy_ptr strategy);

//Energy and diversity fitness
extern float energyDiversityFitness(Generation_ptr generation, Strategy_ptr strategy);


//Eval fitness for all individuals
extern void evalGenerationFitness(Generation_ptr generation, FitnessFunction fitness, MPI_Comm* comm);

//Sort individual by fitness value
extern void sortGenerationByFitness(Generation_ptr generation);

//Calculate population statistics
extern void updateGenerationStatistics(Generation_ptr generation);



/*****Selection eval functions*****/
//Copy best individual from current generation to the next generation
//The individual must be sorted by fitness asc and statistics update
extern void elitism(Generation_ptr currentGeneration, Generation_ptr nextGeneration, float elitismPercentage);

//Copy individual with fitness nearest to U(fitmin,fitmax) from current generation to the next generation
//The individual must be sorted by fitness asc and statistics update
extern int FUSS(Generation_ptr currentGeneration, Generation_ptr nextGeneration);

//Apply spike filter and FUSS
extern void filteredFUSS(Generation_ptr currentGeneration, Generation_ptr nextGeneration);


//Select an individuals with fitness proportional method
//The statistics must be updated
extern int fitnessProportionalSelection(Generation_ptr generation);

//Select an individuals with tournament selection
extern int tournamentSelection(Generation_ptr generation);

//Select an individuals with rank selection
extern int rankSelection(Generation_ptr generation);

//Select an individuals with linear rank selection with best individual pressure
extern int linearRankWithPressureSelection(Generation_ptr generation);

/*****Crossover functions*****/
//Apply single point crossover and create 2 children
extern void singlePointCrossover(Strategy_ptr parent1, Strategy_ptr parent2, Strategy_ptr child1, Strategy_ptr child2, int cut);

//Apply crossover to current generation and save it in next generation
extern int crossOver(Generation_ptr currentGeneration, Generation_ptr nextGeneration, SelectionFunction selection, CrossoverFunction crossover);



/*****Mutation function*****/
//Add new random change point
extern void addRandomChangePoint(Strategy_ptr strategy);

//Remove a random change point
extern void removeRandomChangePoint(Strategy_ptr strategy);

//Move random change point position
extern void moveRandomChangePoint(Strategy_ptr strategy);

//Change random change point action
extern void changeRandomChangePointAction(Strategy_ptr strategy);

//Remove change points which generate spikes in map profile
extern void filterStrategy(Strategy_ptr strategy);

//Apply mutation to the next generation
extern int mutation(Generation_ptr nextGeneration, MutationFunction mutationFunction, float mutationRate);


#endif
