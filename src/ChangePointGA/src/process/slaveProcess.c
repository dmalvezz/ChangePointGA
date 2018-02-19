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

	sProcess->strategies = (Strategy_ptr)malloc(sizeof(Strategy) * POPULATION_SIZE);
}

void execSlave(SlaveProcess* sProcess){
	char cmd;
	int loop = 1;

	//Get strategies portion count
	int size;
	MPI_Comm_size(sProcess->comm, &size);
	int strCount = POPULATION_SIZE / size;

	while(loop){
		//Wait for command from master
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
					sProcess->strategies, strCount, MPI_STRATEGY,
					0, sProcess->comm
				);

				//Simulate all
				parallelSimulateStrategy(
						sProcess->strategies, sProcess->simOut, strCount,
						SIM_THREAD_COUNT,
						START_VELOCITY, END_VELOCITY, START_MAP, KEEP_TIME_INVALID
				);


				//Send back the simulation output
				MPI_Gather(
					sProcess->simOut, strCount, MPI_SIMULATION_OUTPUT,
					NULL, 0, MPI_SIMULATION_OUTPUT,
					0, sProcess->comm
				);

				break;
		}

	}
}

void disposeSlave(SlaveProcess* sProcess){
	free(sProcess->strategies);
}
