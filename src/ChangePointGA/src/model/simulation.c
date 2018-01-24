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

	simulation->result = SIM_NOT_RUN;
}

void initMapset(){
#ifdef RE50_24V
	initMap(&maps[0], -100000, 1000.0, 0.0);
	initMap(&maps[1], -9800, 100.0, 0.0);
	initMap(&maps[2], -5600.0, 40.0, 0.0);
	initMap(&maps[3], -4800.0, 45.0, 0.0);

	/*
	initMap(&maps[0], -4800.0, 100.0, 0.0);
	initMap(&maps[1], -9000.0, 100.0, 0.0);
	initMap(&maps[2], -14000.0, 100.0, 0.0);
	initMap(&maps[3], -4800.0, 45.0, 0.0);
	*/

	/*
	initMap(&maps[0], -4800.0, 45.0, 0.0);
	initMap(&maps[1], -4800.0, 40.0, 0.0);
	initMap(&maps[2], -4800.0, 25.0, 0.0);
	initMap(&maps[3], -67500.0, 200.0, 0.0);
	*/

	/*
	initMap(&maps[0], -9500.0, 60.0, 0.0);
	initMap(&maps[1], -6100.0, 45.0, 0.0);
	initMap(&maps[2], -16000.0, 65.0, 0.0);
	initMap(&maps[3], -45000.0, 130.0, 0.0);
	*/
#endif

#ifdef RE50_36V
	initMap(&maps[0], -4800.0, 100.0, 0.0);
	initMap(&maps[1], -9000.0, 100.0, 0.0);
	initMap(&maps[2], -4800.0, 45.0, 0.0);
	initMap(&maps[3], -14000.0, 100.0, 0.0);
#endif

#ifdef RE40_48V
	initMap(&maps[0], -60000, 1000.0, 0.0);
	initMap(&maps[1], -6000, 130.0, 0.0);
	initMap(&maps[2], -9000, 130.0, 0.0);
	initMap(&maps[3], -9800, 100.0, 0.0);
#endif

#ifdef RE40_24V
	initMap(&maps[0], -48000, 1000.0, 0.0);
	initMap(&maps[1], -70000, 1000.0, 0.0);
	initMap(&maps[2], -14000, 200.0, 0.0);
	initMap(&maps[3], -11000, 130.0, 0.0);
#endif

}

SimulationResult simulate(Simulation_ptr simulation, float startPosition, float endPosition, Action action, int keepTimeInvalid){

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
		slope = getTrackSlope(simulation->position, SPACE_STEP);

		radius = getTrackRadius(simulation->position, SPACE_STEP);

		fTraction = getForceTraction(simulation->velocity, simulation->selectedMap);

		fSlope = getForceSlope(slope);

		fAero = getForceAero(simulation->velocity, radius);

		fTyres = getForceTyres(simulation->velocity, radius);

		fResistent = getForcebearings() + fSlope + fAero + fTyres;

		fTot = fTraction - fResistent;

		a = fTot / VehicleInertialMass;

		//If a = 0 use costant velocity
		if(!isZero(a)){
			delta = simulation->velocity * simulation->velocity + 2 * a * SPACE_STEP;

			if(delta < 0){
				simRes = SIM_DELTA_ERR;
			}

			dt = (sqrt(delta) - simulation->velocity) / a;
		}
		else{
			delta = -1;

			dt = SPACE_STEP / simulation->velocity;
		}

		if(dt < 0){
			simRes = SIM_DT_NEG;
		}

		dE = getPower(simulation, fTraction, simulation->velocity, dt) * dt;

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

		simulation->position += SPACE_STEP;
		simulation->velocity = simulation->velocity + a * dt;
		simulation->time += dt;
		simulation->energy += dE;

		if(simulation->velocity < 0){
			simRes = SIM_VEL_NEG;
		}

		if(simulation->time > MAX_TIME && keepTimeInvalid == 0){
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
}


void simulationToCsv(Simulation_ptr simulation, FILE* file){
	fprintf(file, "sim result, %d\n", simulation->result);
	fprintf(file, "total time, %f\n", simulation->time);
	fprintf(file, "total energy, %f\n", simulation->energy);
	fprintf(file, "map,x,slope,radius,vi,ft,fslope,faero,ftyres,fres,ftot,a,dt,dE\n");
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

void simulationToStrategy(Simulation_ptr simulation, FILE* file){
	for(int i = 0; i < SIM_STEP_COUNT; i++){
		fprintf(file, "%f,", simulation->steps[i].ftraction);
	}
}

void saveSimulationParams(const char* fileName){
	FILE* file = fopen(fileName, "wt");

	fprintf(file, "Motor: %s\n", MOTOR_NAME);
	fprintf(file, "Trasmission: %f\n", 1.0 / TransmissionRatio);
	fprintf(file, "Start vel: %f\n", START_VELOCITY);
	fprintf(file, "End vel: %f\n", END_VELOCITY);
	fprintf(file, "Laps: %d\n", LAP_COUNT);
	fprintf(file, "Start map: %d\n", START_MAP);
	for(int i = 0; i < MAP_COUNT; i++){
		fprintf(file, "Map%d   a0: %f   a1: %f   a2: %f\n", i, maps[i].a0, maps[i].a1, maps[i].a2);
	}

	fclose(file);
}

void printSimulationParams(){
	char simFlags[8];
	int flagsCount = 0;
	int row = 0;
	wclear(simParamWindow);
	box(simParamWindow,0,0);

#ifdef FAST_SIM
	simFlags[flagsCount++] = 'F';
#endif

	if(KEEP_TIME_INVALID == 1){
		simFlags[flagsCount++] = 'K';
	}

	simFlags[flagsCount]= '\0';
	mvwprintw(simParamWindow, row++, 1, "Simulation params(%s)", simFlags);
	mvwprintw(simParamWindow, row++, 1, "Motor: %s   Transmission: %.2f", MOTOR_NAME, 1.0 / TransmissionRatio);
	mvwprintw(simParamWindow, row++, 1, "Laps: %d           Start map: %d", LAP_COUNT, START_MAP);
	mvwprintw(simParamWindow, row++, 1, "Start vel: %.2f   End vel: %.2f", START_VELOCITY, END_VELOCITY);

	/*
	for(int i = 0; i < MAP_COUNT; i++){
		mvwprintw(simParamWindow, row++, 1, "Map%d", i);
		mvwprintw(simParamWindow, row++, 1, "   a0:%.2f  a1:%.2f  a2:%.2f", maps[i].a0, maps[i].a1, maps[i].a2);
	}
	*/
	wrefresh(simParamWindow);
}

