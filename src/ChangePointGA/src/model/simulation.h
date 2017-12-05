/*
 ============================================================================
 Name        : simulation.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Simulation params and functions
 ============================================================================
 */

#ifndef SIMULATION_H_
#define SIMULATION_H_

#define POWERTRAIN_2017
#define ESCORPIOEVO3
#define POWERTRAIN_FLAT

#include "escorpio17.h"
#include "londontrack17.h"

#include <math.h>
#include "../ga/changepoint.h"
#include "../utils/mathutils.h"

#define SPACE_STEP	15.0
#define SIM_STEP_COUNT	(TRACK_LENGTH / (int)SPACE_STEP)

#define FIRST_LAP
#ifdef FIRST_LAP
	#define MIN_AVG_SPEED 6.74
#elif LAST_LAP
	#define MIN_AVG_SPEED 6.74
#else
	#define MIN_AVG_SPEED 7.2
#endif

#define START_VELOCITY	0
#define START_MAP		1
#define MAP_COUNT		4

typedef enum SimulationResult{
	SIM_OK,
	SIM_DELTA_ERR,
	SIM_VEL_NEG,
	SIM_TIME_MAX
}SimulationResult;

typedef struct SimulationStep{
	int map;
	float xi;
	float radius;
	float slope;
	float vi;
	float ftraction;
	float fslope;
	float faero;
	float ftyres;
	float fres;
	float a;
	float dt;
	float dE;
}SimulationStep;

typedef SimulationStep* SimulationStep_ptr;

#define KEEP_SIM_STEP
typedef struct Simulation{
	float Tstep;
	float lastTractiveForce;

	float TmotorCase;	//°C (Motor Case Temperature)
	float Twindings;	//°C (Motor Windings Temperature)

	int selectedMap;	//Current map
	float position;		//Current position
	float velocity;		//Current velocity
	float time;			//Total time
	float energy;		//Total energy consumed

	SimulationResult result;

	//Simulation steps
#ifdef KEEP_SIM_STEP
	SimulationStep steps[SIM_STEP_COUNT];
#endif
}Simulation;

typedef Simulation* Simulation_ptr;


//Maps
typedef struct Map{
	float a0, a1, a2;
}Map;

typedef Map* Map_ptr;

//Init a map
void initMap(Map_ptr map, float a0, float a1, float a2);

//Get the accelleration
float getMapForce(Map_ptr map, float velocity);

extern Map maps[MAP_COUNT];



//Init a simulation
void initSimulation(Simulation_ptr simulation, float startVelocity, int startMap);

//Simulate
SimulationResult simulate(Simulation_ptr simulation, float startPosition, float endPosition, Action action);

//Print simulation
void printSimulation(Simulation_ptr simulation);

//Get motor power
float getPower (Simulation_ptr simulation, float Ftraction, float Speed);

//Get motor attrito
float getMotorMattrito(float Speed);

//Bearings model
float getForcebearings();

//Slope resistance
float getForceSlope(float TrackSlope);

//Aerodynamic drag
float getForceAero(float Speed, float TrackRadiusInv);

//Tricycle model
float getForceTyres(float Speed, float TrackRadiusInv);

//Total resistance force
float getForceResistent(float Speed, float TrackSlope, float TrackRadiusInv);

//Get traction force
float getForceTraction(float Speed, int map);

//Get max traction force
float getMaxForceTraction(float Speed);

//Save simulation steps on csv
void simulationToCsv(Simulation_ptr simulation, FILE* file);


#endif /* SIMULATION_H_ */