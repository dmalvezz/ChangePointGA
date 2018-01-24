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
	int dataCount;
	float min;
	float max;
	float median;
	float avg;
	float std;
}Statistic;

typedef Statistic* Statistic_ptr;

extern void resetStatistic(Statistic_ptr stat, int dataCount);

extern void updateStatistic(Statistic_ptr stat, float value, int index);

extern void finalizeStatistic(Statistic_ptr stat);

extern void printStatisticCsvHeader(const char* statName, FILE* file);

extern void printStatisticCsvData(Statistic_ptr stat, FILE* file);

#endif
