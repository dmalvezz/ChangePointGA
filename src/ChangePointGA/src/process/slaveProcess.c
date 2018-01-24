/*
 * slaveProcess.c
 *
 *  Created on: Jan 20, 2018
 *      Author: davide
 */

#include "slaveProcess.h"

void initSlave(SlaveProcess* sProcess, int worldId, int color){
	//Process info
	sProcess->worldId = worldId;
	MPI_Comm_split(MPI_COMM_WORLD, color, worldId, &sProcess->comm);
}

void execSlave(SlaveProcess* sProcess){
	char cmd;
	int size;
	MPI_Comm_size(sProcess->comm, &size);

	int loop = 1;

	int strCount = POPULATION_SIZE / size;
	Strategy strategies[strCount];

	while(loop){
		//Wait for command from mater
		MPI_Bcast(&cmd, 1, MPI_CHAR, 0, sProcess->comm);

		switch (cmd) {
			case SLAVE_QUIT_CMD:
				//Stop slave process
				loop = 0;
				break;

			case SLAVE_SIMULATE_STRAT_CMD:
				//Receive generation portion to simulate
				MPI_Scatter(
					NULL, 0, MPI_STRATEGY,
					strategies, strCount, MPI_STRATEGY,
					0, sProcess->comm
				);

				//Simulate all
				parallelSimulateStrategy(
						strategies, strCount, SIM_THREAD_COUNT,
						START_VELOCITY, END_VELOCITY, START_MAP, KEEP_TIME_INVALID
				);

				//Send back the simulate generation portion
				MPI_Gather(
					strategies, strCount, MPI_STRATEGY,
					NULL, 0, MPI_STRATEGY,
					0, sProcess->comm
				);

				break;
		}

	}
}

void disposeSlave(SlaveProcess* sProcess){

}
