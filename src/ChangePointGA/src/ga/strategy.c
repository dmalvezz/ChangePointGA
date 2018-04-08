/*
 * stategy.c
 *
 *  Created on: Nov 19, 2017
 *      Author: Davide Malvezzi
 */

#include "strategy.h"
#include <omp.h>

void initStrategy(Strategy_ptr strategy, int spaceStep){
	//strategy->lastChange = -1;
	strategy->size = randInt(MIN_CHANGE_POINT, MAX_CHANGE_POINT);

	float positionIndexMin, positionIndexMax;
	float changePointStep = (float)TRACK_END_POINT / strategy->size;

	for(int i = 0; i < strategy->size; i++){
		positionIndexMin = changePointStep * i;
		positionIndexMax = changePointStep * (i + 1);

		strategy->points[i].prevStatus = GENE_UNKNOWN;
		strategy->points[i].currStatus = GENE_UNKNOWN;
		strategy->points[i].positionIndex = randFloat(positionIndexMin, positionIndexMax) / spaceStep;
		strategy->points[i].action = randInt(ACTION_MINUS, ACTION_PLUS);
	}
}

void printStrategy(Strategy_ptr strategy){
	/*
	wprintw(debugWindow, "Size: %d\n", strategy->size);
	wprintw(debugWindow, "CP: ");

	for(int i = 0; i < strategy->size; i++){
		printChangePoint(strategy->points[i]);
		wprintw(debugWindow, " ");
	}

	printf("\n");
	wrefresh(debugWindow);
	*/
}

void strategyToFile(Strategy_ptr strategy, const char* stratName){
	char fileName[32];
	FILE* file;

	Simulation simulation;
	simulateStrategy(strategy, &simulation,
			START_VELOCITY, END_VELOCITY, START_MAP, 1
	);

	struct stat st = {0};

	if (stat(stratName, &st) == -1) {
	    mkdir(stratName, 0700);
	}

	//Save simulation
	strcpy(fileName, stratName);
	strcat(fileName, "/simulation.csv");
	file = fopen(fileName, "wt");
	simulationToCsv(&simulation, file);
	fclose(file);

	//Save strategy
	strcpy(fileName, stratName);
	strcat(fileName, "/strategy.csv");
	file = fopen(fileName, "wt");
	fprintf(file, "%d\n", strategy->size);
	for(int i = 0; i < strategy->size; i++){
		fprintf(file, "%d,%d\n", strategy->points[i].positionIndex, strategy->points[i].action);
	}
	fclose(file);

	//Bin file
	strcpy(fileName, stratName);
	strcat(fileName, "/str.bin");
	file = fopen(fileName, "wt");
	fwrite(strategy, sizeof(Strategy), 1, file);
	fclose(file);

	//Strat file
	strcpy(fileName, stratName);
	strcat(fileName, "/strat.txt");
	file = fopen(fileName, "wt");
	simulationToStrategy(&simulation, file);
	fclose(file);
}

void strategyFromCsv(Strategy_ptr strategy, const char* fileName){
	FILE* file = fopen(fileName, "rt");

	char newLine, comma;
	fscanf(file, "%d%c", &strategy->size, &newLine);
	for(int i = 0; i < strategy->size; i++){
		fscanf(file,"%d%c%d%c", &strategy->points[i].positionIndex, &comma, &strategy->points[i].action, &newLine);
	}
	fclose(file);
}

void getMapProfile(Strategy_ptr strategy, float* profile){
	int map = START_MAP;
	int index = 0;

	for(int i = 0; i < SIM_STEP_COUNT; i++){
		while(index < strategy->size && strategy->points[index].positionIndex == i){
			map = getCurrentMap(strategy->points[index].action, map);
			index++;
		}
		profile[i] = map;
	}
}

void updateChangePointsStatus(Strategy_ptr strategy){
	int map = START_MAP;
	int prevMap = START_MAP;
	int index = 0;

	for(int i = 0; i < SIM_STEP_COUNT; i++){
		while(index < strategy->size && strategy->points[index].positionIndex == i){
			prevMap = map;
			map = getCurrentMap(strategy->points[index].action, map);

			//Update gene status
			strategy->points[index].prevStatus = strategy->points[index].currStatus;
			if(prevMap == map){
				strategy->points[index].currStatus = GENE_NEUTRAL;
			}
			else{
				strategy->points[index].currStatus = GENE_ACTIVE;
			}

			index++;
		}
	}
}


int compareStrategyFitness(const void* elem1, const void* elem2) {
    Strategy_ptr str1 = (Strategy_ptr)elem1;
    Strategy_ptr str2 = (Strategy_ptr)elem2;

    if (str1->fitness > str2->fitness) return 1;
    if (str1->fitness < str2->fitness) return -1;
    return 0;
}

int getChangePointNearAt(Strategy_ptr strategy, int position){
	int sx = 0;
	int dx = strategy->size - 1;
	int m = (sx + dx) / 2;

	if(strategy->size <= 0 || strategy->points[0].positionIndex > position){
		//The value is before the first change point
		return -1;
	}

	while (sx <= dx) {
	  if (strategy->points[m].positionIndex < position){
		 sx = m + 1;
	  }
	  else if (strategy->points[m].positionIndex > position){
		 dx = m - 1;
	  }
	  else{
		  break;
	  }

	  m = (sx + dx) / 2;
	}

	//Search for the last change point with position <= the searched position
	while(m < strategy->size - 1 && strategy->points[m].positionIndex == strategy->points[m + 1].positionIndex){
		m++;
	}

	return m;
}

void addChangePoint(Strategy_ptr strategy, int index){
	addElement(strategy->points, index, strategy->size, sizeof(ChangePoint));
	strategy->size++;
}

void removeChangePoint(Strategy_ptr strategy, int index){
	removeElement(strategy->points, index, strategy->size, sizeof(ChangePoint));
	strategy->size--;
}

void simulateStrategy(Strategy_ptr strategy, Simulation_ptr simulation, float startVelocity, float endVelocity, int startMap, int keepTimeInvalid){
	int i = 0;

	//Init simulation
	initSimulation(simulation, startVelocity, startMap);

	//Simulate from start to first change point
	simulation->result = simulate(simulation,
			0,
			strategy->points[0].positionIndex * SPACE_STEP,
			ACTION_NONE,
			keepTimeInvalid
		);

	//Simulate all change point
	while(i < strategy->size - 1 && simulation->result == SIM_OK){
		simulation->result = simulate(simulation,
				strategy->points[i].positionIndex * SPACE_STEP,
				strategy->points[i + 1].positionIndex * SPACE_STEP,
				strategy->points[i].action,
				keepTimeInvalid
			);
		i++;
	}

	if(simulation->result == SIM_OK){
		//Simulate from last change point to track end
		simulation->result = simulate(simulation,
				strategy->points[strategy->size - 1].positionIndex * SPACE_STEP,
				TRACK_END_POINT,
				strategy->points[strategy->size - 1].action,
				keepTimeInvalid
			);

		#ifdef N_LAP
			//Get double energy penalty if started lap with gas on and ended lap with gas on
			if (simulation->selectedMap != 0 && simulation->steps[0].map != 0) {
				simulation->energy -= MotorStartPenalty;
			}

			//Check if last velocity is >= than the start velocity
			//Just for general lap
			if(simulation->result == SIM_OK
					&& simulation->velocity < endVelocity){
				simulation->time = INFINITY;
				simulation->energy = INFINITY;
				simulation->result = SIM_END_VEL;
			}
		#endif

		#ifdef PATTERN_LAPS
			if(simulation->result == SIM_OK &&
				fabs(simulation->steps[TRACK_LENGTH * PATTERN_START_LAP].vi - simulation->steps[TRACK_LENGTH * PATTERN_END_LAP].vi) < 0.01){

				float patternLapsEnergy = 0;
				float patternLapsTime = 0;
				for(int i = TRACK_LENGTH * PATTERN_START_LAP; i < TRACK_LENGTH * PATTERN_END_LAP; i++){
					patternLapsEnergy += simulation->steps[i].dE;
					patternLapsTime += simulation->steps[i].dt;
				}

				simulation->time += patternLapsTime * PATTERN_REPEAT;

				if(simulation->time <= RACE_TIME){
					if(simulation->steps[TRACK_LENGTH * PATTERN_START_LAP - 1].map == 0 &&
							simulation->steps[TRACK_LENGTH * PATTERN_START_LAP].map != 0 &&
							simulation->steps[TRACK_LENGTH * PATTERN_END_LAP - 1].map != 0){
						//simulation->energy -= MotorStartPenalty;
						patternLapsEnergy -= MotorStartPenalty;
					}
					simulation->energy += patternLapsEnergy * PATTERN_REPEAT;

				}
				else{
					simulation->time = INFINITY;
					simulation->energy = INFINITY;
					simulation->result = SIM_TIME_MAX;
				}
			}
			else{
				simulation->time = INFINITY;
				simulation->energy = INFINITY;
				simulation->result = SIM_END_VEL;
			}
		#endif

	}

}

void parallelSimulateStrategy(Strategy_ptr strategies, SimulationOutput_ptr simOut, int count, int threadCount, float startVelocity, float endVelocity, int startMap, int keepTimeInvalid){
	#pragma omp parallel num_threads(threadCount)
	{
		Simulation sim;

		#pragma omp for private(sim)
		for(int i = 0; i < count; i++){
			//Simulate the strategy
			simulateStrategy(&strategies[i], &sim, startVelocity, endVelocity, startMap, keepTimeInvalid);

			//Save the output
			simOut[i].energy = sim.energy;
			simOut[i].time = sim.time;
			simOut[i].velocity = sim.velocity;
			simOut[i].result = sim.result;
		}
	}
}

float evalStrategySimilarity(Strategy_ptr str1, Strategy_ptr str2){
	float factor;
	float scalar = 0, l1 = 0, l2 = 0;
	int map1 = START_MAP;
	int map2 = START_MAP;
	int index1 = 0, index2 = 0;

	if(str1->size == 0 || str2->size == 0)return INFINITY;

	for(int i = 0; i < SIM_STEP_COUNT; i++){
		while(index1 < str1->size && str1->points[index1].positionIndex == i){
			map1 = getCurrentMap(str1->points[index1].action, map1);
			index1++;
		}

		while(index2 < str2->size && str2->points[index2].positionIndex == i){
			map2 = getCurrentMap(str2->points[index2].action, map2);
			index2++;
		}

		scalar += map1 * map2;
		l1 += map1 * map1;
		l2 += map2 * map2;
	}

	//Cosine measure
	factor = scalar / (sqrt(l1) * sqrt(l2));
	factor = acosf(factor) * 180.0 / M_PI;

	//Jacquard measure
	//factor = scalar / (l1 + l2 - scalar);

	return factor;

}



