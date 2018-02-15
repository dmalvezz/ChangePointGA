/*
 * gaMPIDatatype.c
 *
 *  Created on: Jan 20, 2018
 *      Author: davide
 */

#include "gaMPIDatatype.h"
#include "../changepoint.h"
#include "../strategy.h"
#include "../../model/simulation.h"

#include "assert.h"

MPI_Datatype MPI_CHANGEPOINT;
MPI_Datatype MPI_SIMULATION_STEP;
MPI_Datatype MPI_SIMULATION;
MPI_Datatype MPI_STRATEGY;

static void initMPIChangePoint(){
	//Changepoint
	int structlen = 2;
	int blocklengths[structlen];
	MPI_Datatype types[structlen];
	MPI_Aint displacements[structlen];
	ChangePoint cp;

	blocklengths[0] = 1; types[0] = MPI_INT; displacements[0] = (size_t)&(cp.positionIndex) - (size_t)&cp;
	blocklengths[1] = 1; types[1] = MPI_INT; displacements[1] = (size_t)&(cp.action) - (size_t)&cp;

	assert(MPI_Type_create_struct(structlen, blocklengths, displacements,types, &MPI_CHANGEPOINT) == MPI_SUCCESS);
	assert(MPI_Type_commit(&MPI_CHANGEPOINT) == MPI_SUCCESS);
}

static void initMPISimulationStep(){
	//Simulation step
	#ifndef USE_COMPACT_STEP
		int structlen = 13;
		int blocklengths[structlen];
		MPI_Datatype types[structlen];
		MPI_Aint displacements[structlen];
		SimulationStep sim;

		blocklengths[0] = 1; types[0] = MPI_INT; displacements[0] = (size_t)&(sim.map) - (size_t)&sim;
		blocklengths[1] = 1; types[1] = MPI_FLOAT; displacements[1] = (size_t)&(sim.xi) - (size_t)&sim;
		blocklengths[2] = 1; types[2] = MPI_FLOAT; displacements[2] = (size_t)&(sim.radius) - (size_t)&sim;
		blocklengths[3] = 1; types[3] = MPI_FLOAT; displacements[3] = (size_t)&(sim.slope) - (size_t)&sim;
		blocklengths[4] = 1; types[4] = MPI_FLOAT; displacements[4] = (size_t)&(sim.vi) - (size_t)&sim;
		blocklengths[5] = 1; types[5] = MPI_FLOAT; displacements[5] = (size_t)&(sim.ftraction) - (size_t)&sim;
		blocklengths[6] = 1; types[6] = MPI_FLOAT; displacements[6] = (size_t)&(sim.fslope) - (size_t)&sim;
		blocklengths[7] = 1; types[7] = MPI_FLOAT; displacements[7] = (size_t)&(sim.faero) - (size_t)&sim;
		blocklengths[8] = 1; types[8] = MPI_FLOAT; displacements[8] = (size_t)&(sim.ftyres) - (size_t)&sim;
		blocklengths[9] = 1; types[9] = MPI_FLOAT; displacements[9] = (size_t)&(sim.fres) - (size_t)&sim;
		blocklengths[10] = 1; types[10] = MPI_FLOAT; displacements[10] = (size_t)&(sim.a) - (size_t)&sim;
		blocklengths[11] = 1; types[11] = MPI_FLOAT; displacements[11] = (size_t)&(sim.dt) - (size_t)&sim;
		blocklengths[12] = 1; types[12] = MPI_FLOAT; displacements[12] = (size_t)&(sim.dE) - (size_t)&sim;
	#else
		//Simulation
		int structlen = 6;
		int blocklengths[structlen];
		MPI_Datatype types[structlen];
		MPI_Aint displacements[structlen];
		SimulationStep sim;

		blocklengths[0] = 1; types[0] = MPI_INT; displacements[0] = (size_t)&(sim.map) - (size_t)&sim;
		blocklengths[1] = 1; types[1] = MPI_FLOAT; displacements[1] = (size_t)&(sim.vi) - (size_t)&sim;
		blocklengths[2] = 1; types[2] = MPI_FLOAT; displacements[2] = (size_t)&(sim.ftraction) - (size_t)&sim;
		blocklengths[3] = 1; types[3] = MPI_FLOAT; displacements[3] = (size_t)&(sim.a) - (size_t)&sim;
		blocklengths[4] = 1; types[4] = MPI_FLOAT; displacements[4] = (size_t)&(sim.dt) - (size_t)&sim;
		blocklengths[5] = 1; types[5] = MPI_FLOAT; displacements[5] = (size_t)&(sim.dE) - (size_t)&sim;
	#endif

	assert(MPI_Type_create_struct(structlen, blocklengths, displacements,types, &MPI_SIMULATION_STEP) == MPI_SUCCESS);
	assert(MPI_Type_commit(&MPI_SIMULATION_STEP) == MPI_SUCCESS);
}

static void initMPISimulation(){
	//Simulation
	int structlen = 10;
	int blocklengths[structlen];
	MPI_Datatype types[structlen];
	MPI_Aint displacements[structlen];
	Simulation sim;

	blocklengths[0] = 1; types[0] = MPI_FLOAT; displacements[0] = (size_t)&(sim.lastTractiveForce) - (size_t)&sim;
	blocklengths[1] = 1; types[1] = MPI_FLOAT; displacements[1] = (size_t)&(sim.TmotorCase) - (size_t)&sim;
	blocklengths[2] = 1; types[2] = MPI_FLOAT; displacements[2] = (size_t)&(sim.Twindings) - (size_t)&sim;
	blocklengths[3] = 1; types[3] = MPI_INT;   displacements[3] = (size_t)&(sim.selectedMap) - (size_t)&sim;
	blocklengths[4] = 1; types[4] = MPI_FLOAT; displacements[4] = (size_t)&(sim.position) - (size_t)&sim;
	blocklengths[5] = 1; types[5] = MPI_FLOAT; displacements[5] = (size_t)&(sim.velocity) - (size_t)&sim;
	blocklengths[6] = 1; types[6] = MPI_FLOAT; displacements[6] = (size_t)&(sim.time) - (size_t)&sim;
	blocklengths[7] = 1; types[7] = MPI_FLOAT; displacements[7] = (size_t)&(sim.energy) - (size_t)&sim;
	blocklengths[8] = 1; types[8] = MPI_INT;   displacements[8] = (size_t)&(sim.result) - (size_t)&sim;
	blocklengths[9] = SIM_STEP_COUNT; types[9] = MPI_SIMULATION_STEP; displacements[9] = (size_t)&(sim.steps) - (size_t)&sim;

	assert(MPI_Type_create_struct(structlen, blocklengths, displacements,types, &MPI_SIMULATION) == MPI_SUCCESS);
	assert(MPI_Type_commit(&MPI_SIMULATION) == MPI_SUCCESS);
}

static void initMPIStrategy(){
	//Strategy
	int structlen = 5;
	int blocklengths[structlen];
	MPI_Datatype types[structlen];
	MPI_Aint displacements[structlen];
	Strategy str;

	blocklengths[0] = 1; types[0] = MPI_INT; displacements[0] = (size_t)&(str.size) - (size_t)&str;
	blocklengths[1] = 1; types[1] = MPI_FLOAT; displacements[1] = (size_t)&(str.fitness) - (size_t)&str;
	blocklengths[2] = 1; types[2] = MPI_FLOAT; displacements[2] = (size_t)&(str.similarity) - (size_t)&str;
	blocklengths[3] = MAX_CHANGE_POINT; types[3] = MPI_CHANGEPOINT; displacements[3] = (size_t)&(str.points) - (size_t)&str;
	blocklengths[4] = 1; types[4] = MPI_SIMULATION; displacements[4] = (size_t)&(str.simulation) - (size_t)&str;

	assert(MPI_Type_create_struct(structlen, blocklengths, displacements,types, &MPI_STRATEGY) == MPI_SUCCESS);
	assert(MPI_Type_commit(&MPI_STRATEGY) == MPI_SUCCESS);
}


void initGAMPIDatatypes(){
	initMPIChangePoint();
	initMPISimulationStep();
	initMPISimulation();
	initMPIStrategy();
}

void disposteGAMPIDatatypes(){
	MPI_Type_free(&MPI_CHANGEPOINT);
	MPI_Type_free(&MPI_SIMULATION_STEP);
	MPI_Type_free(&MPI_SIMULATION);
	MPI_Type_free(&MPI_STRATEGY);
}
