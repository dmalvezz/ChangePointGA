/*
 ============================================================================
 Name        : slaveProcess.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : GA struct MPI data structs
 ============================================================================
 */

#ifndef GA_MPI_DATATYPE_H_
#define GA_MPI_DATATYPE_H_

#include <mpi.h>

//GA datatype
extern MPI_Datatype MPI_CHANGEPOINT;
extern MPI_Datatype MPI_SIMULATION_STEP;
extern MPI_Datatype MPI_SIMULATION;
extern MPI_Datatype MPI_STRATEGY;

//Init MPI datatype for custom struct
extern void initGAMPIDatatypes();

//Dispose MPI datatype for custom struct
extern void disposteGAMPIDatatypes();


#endif
