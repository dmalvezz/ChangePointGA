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

#include "strategy.h"


typedef struct Statistics{
	int invalidCount;
	unsigned long int lastChange;

	float fitnessMax;
	float fitnessMin;
	float fitnessMedian;
	float fitnessAvg;

	float lengthAvg;
	float similarityAvg;

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
Generation_ptr initEmptyGeneration(int size);

//Initialize a random population
Generation_ptr initRandomGeneration(int size);

//Dispose generation
void disposeGeneration(Generation_ptr generation);

//Print population individuals
void printGeneration(Generation_ptr generation);

//Save the generation to file
void generationToFile(Generation_ptr generation, const char* filename);

//Save statistics to file
void statisticsToFile(Generation_ptr generation, unsigned long int generationCount, FILE* file);


/*****Fitness eval functions*****/
//Pure energy based fitness
float energyFitness(Generation_ptr generation, Strategy_ptr strategy);

//Energy and time fitness
float energyTimeFitness(Generation_ptr generation, Strategy_ptr strategy);

//Energy and diversity fitness
float energyDiversityFitness(Generation_ptr generation, Strategy_ptr strategy);


//Eval fitness for all individuals
void evalGenerationFitness(Generation_ptr generation, float startVelocity, int startMap, FitnessFunction fitness);

//Sort individual by fitness value
void sortGenerationByFitness(Generation_ptr generation);

//Calculate population statistics
void updateGenerationStatistics(Generation_ptr generation);



/*****Selection eval functions*****/
//Copy individual from current generation to the next generation
//The individual must be sorted by fitness asc and statistics update
void elitism(Generation_ptr currentGeneration, Generation_ptr nextGeneration, float elitismPercentage);

//Select an individuals with fitness proportional method
//The statistics must be updated
int fitnessProportionalSelection(Generation_ptr generation);

//Select an individuals with tournament selection
int tournamentSelection(Generation_ptr generation);

//Select an individuals with rank selection
int rankSelection(Generation_ptr generation);


/*****Crossover functions*****/
//Apply single point crossover and create 2 children
void singlePointCrossover(Strategy_ptr parent1, Strategy_ptr parent2, Strategy_ptr child1, Strategy_ptr child2, int cut);

//Apply crossover to current generation and save it in next generation
int crossOver(Generation_ptr currentGeneration, Generation_ptr nextGeneration, SelectionFunction selection, CrossoverFunction crossover);



/*****Mutation function*****/
//Add new random change point
void addRandomChangePoint(Strategy_ptr strategy);

//Remove a random change point
void removeRandomChangePoint(Strategy_ptr strategy);

//Move random change point position
void moveRandomChangePoint(Strategy_ptr strategy);

//Change random change point action
void changeRandomChangePointAction(Strategy_ptr strategy);

//Apply mutation to the next generation
int mutation(Generation_ptr nextGeneration, MutationFunction mutationFunction, float mutationRate);


#endif
