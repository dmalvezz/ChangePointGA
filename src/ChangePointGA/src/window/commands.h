/*
 ============================================================================
 Name        : commands.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Console commands function
 ============================================================================
 */

#include <ctype.h>
#include <getopt.h>

#include "../ga/ga.h"
#include "../utils/gnuplot/gnuplot_i.h"
#include "../utils/uthash/uthash.h"

#define CMD_SIZE		32
#define CMD_PARAM_COUNT	8
#define CMD_PARAM_SIZE	16

typedef int (*CmdFunction)(GA*, char** argv, int argc);

typedef struct Command{
	char name[CMD_SIZE];
	CmdFunction cmdFunction;
	UT_hash_handle hh;
}Command;

extern char errorBuffer[80];
extern char** argv;

//Init argv buffer
extern void initArgv();

//Dispose argv buffer
extern void disposeArgv();

//Set the mutation rate of a specific mutation function
extern int setMutationRate(GA* ga, char** argv, int argc);

//Set the fitness function
extern int setFitnessFunction(GA* ga, char** argv, int argc);

//Set the selection function
extern int setSelectionFunction(GA* ga, char** argv, int argc);

//Filter a strategy
extern int applyFilterToStrategy(GA* ga, char** argv, int argc);

//Plot simulation
extern int plotSimulation(GA* ga, char** argv, int argc);

//Plot statistics
extern int plotStatistic(GA* ga, char** argv, int argc);

//Load strategy from csv
extern int loadStrategyFromCsv(GA* ga, char** argv, int argc);

//Save strategy to csv
extern int saveStrategyToCsv(GA* ga, char** argv, int argc);

//Save generation to csv
extern int saveGenerationToCsv(GA* ga, char** argv, int argc);

//Print explorer
extern void printExplorerWindow(GA* ga, int index);

