/*
 * stategy.c
 *
 *  Created on: Nov 19, 2017
 *      Author: Davide Malvezzi
 */

#include "strategy.h"

void initStrategy(Strategy_ptr strategy, int spaceStep){
	//strategy->lastChange = -1;
	strategy->size = randInt(MIN_CHANGE_POINT, MAX_CHANGE_POINT);

	float positionIndexMin, positionIndexMax;
	float changePointStep = (float)TRACK_END_POINT / strategy->size;

	for(int i = 0; i < strategy->size; i++){
		positionIndexMin = changePointStep * i;
		positionIndexMax = changePointStep * (i + 1);

		strategy->points[i].positionIndex = randFloat(positionIndexMin, positionIndexMax) / spaceStep;
		strategy->points[i].action = randInt(ACTION_MINUS, ACTION_PLUS);
	}
}

void printStrategy(Strategy_ptr strategy){
	printf("Size: %d\n", strategy->size);
	printf("CP: ");

	for(int i = 0; i < strategy->size; i++){
		printChangePoint(strategy->points[i]);
		printf(" ");
	}

	printf("\n");
}

void strategyToFile(Strategy_ptr strategy, const char* fileName){
	FILE* file = fopen(fileName, "wt");

	simulationToCsv(&strategy->simulation, file);

	fprintf(file, "\n\n");
	fprintf(file, "Size: %d\n", strategy->size);
	fprintf(file, "CP: \n");

	for(int i = 0; i < strategy->size; i++){
		fprintf(file, "%d, %d", strategy->points[i].positionIndex, strategy->points[i].action);
		fprintf(file, "\n");
	}

	fclose(file);

	//Bin file
	FILE* bestStrategyFile = fopen("str.bin", "wb");
	fwrite(strategy, sizeof(Strategy), 1, bestStrategyFile);
	fclose(bestStrategyFile);

	//Strat file
	file = fopen("strat.txt", "wt");
	simulationToStrategy(&strategy->simulation, file);
	fclose(file);

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

void simulateStrategy(Strategy_ptr strategy, float startVelocity, int startMap){
	int i = 0;

	//Init simulation
	initSimulation(&strategy->simulation, startVelocity, startMap);

	//Simulate from start to first change point
	strategy->simulation.result = simulate(&strategy->simulation,
			0,
			strategy->points[0].positionIndex * SPACE_STEP,
			ACTION_NONE
			);

	//Simulate all change point
	while(i < strategy->size - 1 && strategy->simulation.result == SIM_OK){
		strategy->simulation.result = simulate(&strategy->simulation,
				strategy->points[i].positionIndex * SPACE_STEP,
				strategy->points[i + 1].positionIndex * SPACE_STEP,
				strategy->points[i].action
				);
		i++;
	}

	if(strategy->simulation.result == SIM_OK){
		//Simulate from last change point to track end
		strategy->simulation.result = simulate(&strategy->simulation,
				strategy->points[strategy->size - 1].positionIndex * SPACE_STEP,
				TRACK_END_POINT,
				strategy->points[strategy->size - 1].action
				);

		//Get double energy penalty if started lap with gas on and ended lap with gas on
		if (strategy->simulation.selectedMap != 0 && strategy->simulation.steps[0].map != 0) {
			strategy->simulation.energy -= MotorStartPenalty;
		}

		//Check if last velocity is >= than the start velocity
		//Just for general lap
		if(strategy->simulation.result == SIM_OK
				&& strategy->simulation.velocity < END_VELOCITY){
			strategy->simulation.time = INFINITY;
			strategy->simulation.energy = INFINITY;
			strategy->simulation.result = SIM_END_VEL;
		}
	}

}

float evalStrategySimilarity(Strategy_ptr str1, Strategy_ptr str2){
	float factor;
	float scalar = 0, l1 = 0, l2 = 0;

	if(str1->simulation.result == SIM_OK && str2->simulation.result == SIM_OK){

		for(int i = 0; i < SIM_STEP_COUNT; i++){
			scalar += str1->simulation.steps[i].map * str2->simulation.steps[i].map;
			l1 += str1->simulation.steps[i].map * str1->simulation.steps[i].map;
			l2 += str2->simulation.steps[i].map * str2->simulation.steps[i].map;
		}

		//Cosine measure
		factor = scalar / (sqrt(l1) * sqrt(l2));

		//Jacquard measure
		//factor = scalar / (l1 + l2 - scalar);

		return factor;
	}

	return INFINITY;
}



