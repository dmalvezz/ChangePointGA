/*
 ============================================================================
 Name        : strategy.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Strategy data structure
 ============================================================================
 */

#ifndef STRATEGY_H
#define STRATEGY_H


#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "changepoint.h"
#include "../model/simulation.h"
#include "../utils/arrayutils.h"
#include "../utils/mathutils.h"

#define MIN_CHANGE_POINT 	3
#define MAX_CHANGE_POINT 	40


typedef struct Strategy{
	int size;									//Num of change points used
	//int lastChange;							//Last change point changed
	float fitness;								//Strategy fitness
	float similarity;							//Strategy similarity factor
	ChangePoint points[MAX_CHANGE_POINT];		//Change points
	Simulation simulation;						//Simulation
}Strategy;

typedef Strategy* Strategy_ptr;


//Init strategy with random size and random change points
extern void initStrategy(Strategy_ptr strategy, int spaceStep);

//Print to stdout
extern void printStrategy(Strategy_ptr strategy);

//Save strategy to csv, bin and strat file
extern void strategyToFile(Strategy_ptr strategy, const char* fileName);

//Load strategy from csv
extern void strategyFromCsv(Strategy_ptr strategy, const char* fileName);

//Compare function used by sort
extern int compareStrategyFitness(const void* elem1, const void* elem2);

//Get the index of changePoint nearest to the given position
extern int getChangePointNearAt(Strategy_ptr strategy, int position);


//New random change point at position
extern void addChangePoint(Strategy_ptr strategy, int position);

//Remove change point at index
extern void removeChangePoint(Strategy_ptr strategy, int index);

//Run simulation
extern void simulateStrategy(Strategy_ptr strategy, float startVelocity, int startMap, int keepTimeInvalid);

//Eval similarity factor
extern float evalStrategySimilarity(Strategy_ptr str1, Strategy_ptr str2);

#endif
