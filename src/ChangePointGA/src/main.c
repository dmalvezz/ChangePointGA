/*
 ============================================================================
 Name        : main.c
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   : 
 Description : ChangePoint GA
 ============================================================================
 */

#include "process/masterProcess.h"
#include "process/slaveProcess.h"

//Main init
static void mainInit();

//Main dispose
static void mainDispose();

#define MASTER_PROCS_COUNT	1

int main(int argc, char *argv[]) {
	//Init MPI
	int rank, procsCount;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &procsCount);

	//Init main
	mainInit();

	//Check if it is a master process
	if(rank < MASTER_PROCS_COUNT){
		MasterProcess master;
		initMaster(&master, rank, rank);

		//Exec master
		execMaster(&master);

		//Dispose master
		disposeMaster(&master);
	}
	//It is a slave process
	else{
		//Init slave
		int color = (rank - MASTER_PROCS_COUNT) % MASTER_PROCS_COUNT;
		SlaveProcess slave;
		initSlave(&slave, rank, color);

		//Exec slave
		execSlave(&slave);

		//Dispose slave
		disposeSlave(&slave);
	}

	//Dispose main
	mainDispose();

	//Shut down MPI
	MPI_Finalize();

	return 0;
}


void mainInit(){
	//Init windows
	initWindows();

	//Init random
	randInit();

	//Init mapset
	initMapset();

	//Init track samples
	generateTrackData(SPACE_STEP);

	//Init GA MPI data types
	initGAMPIDatatypes();
}

void mainDispose(){
	//Init GA MPI data types
	disposteGAMPIDatatypes();

	//Dispose track samples
	disposeTrackData();

	//Dispose windows
	disposeWindows();
}
