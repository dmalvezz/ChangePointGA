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
	float changePointStep = (float)TRACK_LENGTH / strategy->size;

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

void strategyToCsv(Strategy_ptr strategy, const char* fileName){
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

	if(strategy->points[0].positionIndex > position){
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
		  return m;
	  }

	  m = (sx + dx) / 2;
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

void evalStrategyFitness(Strategy_ptr strategy, float startVelocity, int startMap){
	int i = 0;
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
				TRACK_LENGTH,
				strategy->points[0].action
				);
	}

	strategy->fitness = strategy->simulation.energy + pow(strategy->simulation.time - (TRACK_LENGTH / MIN_AVG_SPEED), 4);
}



