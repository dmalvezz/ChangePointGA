/*
 * simulation.c
 *
 *  Created on: Nov 24, 2017
 *      Author: davide
 */


#include "simulation.h"

void initSimulation(Simulation_ptr simulation, float startVelocity, int startMap){
	simulation->lastTractiveForce = 0;

	simulation->Twindings = 38;
	simulation->TmotorCase = 35;

	simulation->selectedMap = startMap;
	simulation->position = 0;
	simulation->velocity = startVelocity;
	simulation->time = 0;
	simulation->energy = 0;
}

SimulationResult simulate(Simulation_ptr simulation, float startPosition, float endPosition, Action action){

	if(action == ACTION_PLUS){
		simulation->selectedMap++;
	}
	else if(action == ACTION_MINUS){
		simulation->selectedMap--;
	}

	if(simulation->selectedMap > MAP_COUNT){
		simulation->selectedMap = MAP_COUNT;
	}
	else if(simulation->selectedMap < 0){
		simulation->selectedMap = 0;
	}

	int startIndex = startPosition / SPACE_STEP;
	int simStep = (endPosition - startPosition) / SPACE_STEP;
	float radius, slope, fTraction, fSlope, fAero, fTyres, fResistent, fTot, a, delta, dt, dE;
	SimulationResult simRes = SIM_OK;

	for(int i = 0; i < simStep; i++){
		slope = getTrackSlope(simulation->position);

		radius = getTrackRadius(simulation->position);

		fTraction = getForceTraction(simulation->velocity, simulation->selectedMap);

		fSlope = getForceSlope(slope);

		fAero = getForceAero(simulation->velocity, radius);

		fTyres = getForceTyres(simulation->velocity, radius);

		fResistent = getForcebearings() + fSlope + fAero + fTyres;

		fTot = fTraction - fResistent;

		a = fTot / VehicleInertialMass;

		delta = simulation->velocity * simulation->velocity + 2 * a * SPACE_STEP;

		if(delta < 0){
			simRes = SIM_DELTA_ERR;
		}

		dt = (sqrt(delta) - simulation->velocity) / a;

		dE = getPower(simulation, fTraction, simulation->velocity, dt) * dt;

		#ifdef KEEP_SIM_STEP
			simulation->steps[startIndex + i].map = simulation->selectedMap;
			simulation->steps[startIndex + i].xi = startPosition + i * SPACE_STEP;
			simulation->steps[startIndex + i].slope = slope;
			simulation->steps[startIndex + i].radius = radius;
			simulation->steps[startIndex + i].vi = simulation->velocity;
			simulation->steps[startIndex + i].ftraction = fTraction;
			simulation->steps[startIndex + i].fslope = fSlope;
			simulation->steps[startIndex + i].faero = fAero;
			simulation->steps[startIndex + i].ftyres = fTyres;
			simulation->steps[startIndex + i].fres = fResistent;
			simulation->steps[startIndex + i].a = a;
			simulation->steps[startIndex + i].dt = dt;
			simulation->steps[startIndex + i].dE = dE;
		#endif

		simulation->position += SPACE_STEP;
		simulation->velocity = simulation->velocity + a * dt;
		simulation->time += dt;
		simulation->energy += dE;

		if(simulation->velocity < 0){
			simRes = SIM_VEL_NEG;
		}


		if(simulation->time > TRACK_LENGTH / MIN_AVG_SPEED){
			simRes = SIM_TIME_MAX;
		}



		if(simRes != SIM_OK){
			simulation->time = INFINITY;
			simulation->energy = INFINITY;
			return simRes;
		}
	}


	return SIM_OK;
}

void printSimulation(Simulation_ptr simulation){
	printf("Sim res %d   time %f   energy %f\n", simulation->result, simulation->time, simulation->energy);

#ifdef KEEP_SIM_STEP
	printf("map   x   v   ft  fslope   faero   ftyres   fres   a   dt   dE\n");
	for(int i = 0; i < SIM_STEP_COUNT; i++){
			printf("%d  %f  %f  %f   %f   %f   %f   %f   %f   %f   %f\n",
				simulation->steps[i].map,
				simulation->steps[i].xi,
				simulation->steps[i].vi,
				simulation->steps[i].ftraction,
				simulation->steps[i].fslope,
				simulation->steps[i].faero,
				simulation->steps[i].ftyres,
				simulation->steps[i].fres,
				simulation->steps[i].a,
				simulation->steps[i].dt,
				simulation->steps[i].dE
			);
	}
#endif
}


void simulationToCsv(Simulation_ptr simulation, FILE* file){
	fprintf(file, "sim result, %d\n", simulation->result);
	fprintf(file, "total time, %f\n", simulation->time);
	fprintf(file, "total energy, %f\n", simulation->energy);
	fprintf(file, "map,x,slope,radius,v,ft,fslope,faero,ftyres,fres,ftot,a,dt,dE\n");
	for(int i = 0; i < SIM_STEP_COUNT; i++){
			fprintf(file, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
				simulation->steps[i].map,
				simulation->steps[i].xi,
				simulation->steps[i].slope,
				simulation->steps[i].radius,
				simulation->steps[i].vi,
				simulation->steps[i].ftraction,
				simulation->steps[i].fslope,
				simulation->steps[i].faero,
				simulation->steps[i].ftyres,
				simulation->steps[i].fres,
				simulation->steps[i].ftraction - simulation->steps[i].fres,
				simulation->steps[i].a,
				simulation->steps[i].dt,
				simulation->steps[i].dE
			);
	}
}
