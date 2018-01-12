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

#include "../ga/ga.h"
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
extern int setMutationRate(GA*, char** argv, int argc);

