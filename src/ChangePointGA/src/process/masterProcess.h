/*
 ============================================================================
 Name        : masterProcess.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Master process function for parallel implementation
 ============================================================================
 */


#ifndef PROCESS_MASTERPROCESS_H_
#define PROCESS_MASTERPROCESS_H_

#include "slaveProcess.h"

//Keep trace of all best strategies
#define KEEP_BEST

//Autosave on each new best strategies
#define AUTO_SAVE

//If generation.bin file exists load that
#define AUTO_LOAD_GENERATION

//Load seed strategy
#define LOAD_SEEDS

//Master process global variables
typedef struct MasterProcess{
	//Proc info
	int worldId;		//Process id in the world
	MPI_Comm comm;		//Communicator with slaves

	//Ga
	int loop;
	GA ga;

	//Files
	FILE* statisticsFile;
	FILE* timesFile;
	FILE* profilesFile;

	float currBest;
	FILE* bestFile;
}MasterProcess;

//Initialize the master process
extern void initMaster(MasterProcess* mProcess, int worldId, int color);

//Exec the master process
extern void execMaster(MasterProcess* mProcess);

//Dispose the master process
extern void disposeMaster(MasterProcess* mProcess);




#endif /* PROCESS_MASTERPROCESS_H_ */
