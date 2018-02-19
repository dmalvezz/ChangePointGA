/*
 ============================================================================
 Name        : slaveProcess.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Slave process function for parallel implementation
 ============================================================================
 */

#ifndef PROCESS_SLAVEPROCESS_H_
#define PROCESS_SLAVEPROCESS_H_

#include <mpi.h>
#include <omp.h>

#include <math.h>
#include <time.h>
#include <stdio.h>

#include "../ga/ga.h"
#include "../window/console.h"

typedef struct SlaveProcess{
	//Proc info
	int worldId;				//Process word rank
	MPI_Comm comm;				//Process group communicator

	Strategy_ptr strategies;	//Strategies temp buffer
	SimulationOutput simOut[POPULATION_SIZE];	//Simulation out buffer
}SlaveProcess;

//Initialize the slave process
extern void initSlave(SlaveProcess* sProcess, int worldId, int color);

//Exec the slave process
extern void execSlave(SlaveProcess* sProcess);

//Dispose the slave process
extern void disposeSlave(SlaveProcess* sProcess);


#endif /* PROCESS_SLAVEPROCESS_H_ */
