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

//Use escorpio3
#define ESCORPIOEVO3

//Motor comparative energy consumption
//#define POWERTRAIN_FLAT

//Fast simulation
//#define FAST_SIM

//Motors list
//#define RE40_24V
//#define RE40_48V
//#define RE50_24V
#define RE50_36V

#define KEEP_TIME_INVALID	0

#include "escorpio17.h"
#include "londontrack18.h"

#include <math.h>
#include "../ga/changepoint.h"
#include "../window/window.h"
#include "../utils/mathutils.h"

#define SPACE_STEP	1.0
#define LAP_COUNT	1
#define TRACK_START_POINT	0
#define TRACK_END_POINT		(TRACK_LENGTH * LAP_COUNT)
#define SIM_STEP_COUNT		(TRACK_END_POINT / (int)SPACE_STEP)

//#define FIRST_LAP
#ifdef FIRST_LAP
	#define MIN_AVG_SPEED 6.74
#elif LAST_LAP
	#define MIN_AVG_SPEED 6.74
#else
	#define MIN_AVG_SPEED 7.0
#endif

#define MAX_TIME	((float)TRACK_END_POINT / MIN_AVG_SPEED)

#define START_VELOCITY	7.0
#define END_VELOCITY	7.0
#define START_MAP		0
#define MAP_COUNT		4

typedef enum SimulationResult{
	SIM_OK,
	SIM_DELTA_ERR,
	SIM_DT_NEG,
	SIM_VEL_NEG,
	SIM_END_VEL,
	SIM_TIME_MAX,
	SIM_NOT_RUN,

	SIM_RESULT_COUNT
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

typedef struct Simulation{
	float lastTractiveForce;	//Last force applied

	float TmotorCase;	//°C (Motor Case Temperature)
	float Twindings;	//°C (Motor Windings Temperature)

	int selectedMap;	//Current map
	float position;		//Current position
	float velocity;		//Current velocity
	float time;			//Total time
	float energy;		//Total energy consumed

	SimulationResult result;					//Simulation result
	SimulationStep steps[SIM_STEP_COUNT];		//Simulation steps
}Simulation;

typedef Simulation* Simulation_ptr;


//Maps
typedef struct Map{
	float a0, a1, a2;
}Map;

typedef Map* Map_ptr;

//Init a map
extern void initMap(Map_ptr map, float a0, float a1, float a2);

//Get the accelleration
extern float getMapForce(Map_ptr map, float velocity);

extern Map maps[MAP_COUNT];



//Init a simulation
extern void initSimulation(Simulation_ptr simulation, float startVelocity, int startMap);

//Init the mapset
extern void initMapset();

//Simulate
extern SimulationResult simulate(Simulation_ptr simulation, float startPosition, float endPosition, Action action, int keepTimeInvalid);

//Print simulation
extern void printSimulation(Simulation_ptr simulation);

//Get motor power
extern float getPower (Simulation_ptr simulation, float Ftraction, float Speed, float dt);

//Get motor attrito
extern float getMotorMattrito(float Speed);

//Bearings model
extern float getForcebearings();

//Slope resistance
extern float getForceSlope(float TrackSlope);

//Aerodynamic drag
extern float getForceAero(float Speed, float TrackRadiusInv);

//Tricycle model
extern float getForceTyres(float Speed, float TrackRadiusInv);

//Total resistance force
extern float getForceResistent(float Speed, float TrackSlope, float TrackRadiusInv);

//Get traction force
extern float getForceTraction(float Speed, int map);

//Get max traction force
extern float getMaxForceTraction(float Speed);

//Save simulation steps on csv
extern void simulationToCsv(Simulation_ptr simulation, FILE* file);

//Save simulation traction force on csv
extern void simulationToStrategy(Simulation_ptr simulation, FILE* file);

//Export simulation params
extern void saveSimulationParams(const char* fileName);

//Print simulation params
extern void printSimulationParams();


#endif /* SIMULATION_H_ */
