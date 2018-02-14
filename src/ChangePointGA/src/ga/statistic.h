/*
 ============================================================================
 Name        : statistic.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : General statistic
 ============================================================================
 */

#ifndef STATISTIC_H_
#define STATISTIC_H_

#include <math.h>
#include <stdio.h>

typedef struct Statistic{
	int dataCount;		//Max numeber of data samples
	float min;			//Min sample
	float max;			//Max sample
	float median;		//Samples median
	float avg;			//Samples avg
	float std;			//Samples std deviation
}Statistic;

typedef Statistic* Statistic_ptr;

//Reset all the statistic data
extern void resetStatistic(Statistic_ptr stat, int dataCount);

//Add new value to the statistic
extern void updateStatistic(Statistic_ptr stat, float value, int index);

//Work out statistics values
extern void finalizeStatistic(Statistic_ptr stat);

//Print statistic header on csv file
extern void printStatisticCsvHeader(const char* statName, FILE* file);

//Print statistic values on csv file
extern void printStatisticCsvData(Statistic_ptr stat, FILE* file);

#endif
