/*
 * mutation.c
 *
 *  Created on: Nov 20, 2017
 *      Author: davide
 */

#include "../generation.h"

void addRandomChangePoint(Strategy_ptr strategy){
	if(strategy->size < MAX_CHANGE_POINT){
		//int r = randInt(0, strategy->size);
		int randPos = randInt(0, TRACK_LENGTH / SPACE_STEP);
		int r = getChangePointNearAt(strategy, randPos) + 1;

		addElement(strategy->points, r, strategy->size, sizeof(ChangePoint));

		if(r == 0){
			strategy->points[0].positionIndex = randInt(0, strategy->points[1].positionIndex);
		}
		else if(r == strategy->size){
			strategy->points[r].positionIndex = randInt(strategy->points[r - 1].positionIndex , TRACK_LENGTH / SPACE_STEP);
		}
		else {
			strategy->points[r].positionIndex = randInt(strategy->points[r - 1].positionIndex, strategy->points[r + 1].positionIndex);
		}

		strategy->points[r].action = randInt(ACTION_MINUS, ACTION_PLUS);
		strategy->size++;

		/*
		if(r - 1 < strategy->lastChange){
			strategy->lastChange = r - 1;
		}
		*/
	}
}

void removeRandomChangePoint(Strategy_ptr strategy){
	int r = randInt(0, strategy->size - 1);

	removeElement(strategy->points, r, strategy->size, sizeof(ChangePoint));
	/*
	if(r - 1 < strategy->lastChange){
		strategy->lastChange = r - 1;
	}
	*/
	strategy->size--;

	if(strategy->size < MIN_CHANGE_POINT){
		addRandomChangePoint(strategy);
	}

}

void moveRandomChangePoint(Strategy_ptr strategy){
	//int r = randInt(0, strategy->size - 1);
	int randPos = randInt(0, TRACK_LENGTH / SPACE_STEP);
	int r = getChangePointNearAt(strategy, randPos);

	if(r <= 0){
		strategy->points[0].positionIndex = randInt(0, strategy->points[1].positionIndex);
	}
	else if(r == strategy->size - 1){
		strategy->points[r].positionIndex = randInt(strategy->points[r - 1].positionIndex , TRACK_LENGTH / SPACE_STEP);
	}
	else {
		strategy->points[r].positionIndex = randInt(strategy->points[r - 1].positionIndex, strategy->points[r + 1].positionIndex);
	}

	/*
	if(r - 1 < strategy->lastChange){
		strategy->lastChange = r - 1;
	}
	*/
}

void changeRandomChangePointAction(Strategy_ptr strategy){
	int r = randInt(0, strategy->size - 1);

	strategy->points[r].action = randInt(ACTION_MINUS, ACTION_PLUS);

	/*
	if(r - 1 < strategy->lastChange){
		strategy->lastChange = r - 1;
	}
	*/
}

int mutation(Generation_ptr nextGeneration, MutationFunction mutationFunction, float mutationRate){
	int mutationCount = 0;
	float r;

	for(int i = 0; i < nextGeneration->count; i++){
		r = randFloat(0, 1);
		if(r < mutationRate){
			//Apply mutation
			mutationFunction(&nextGeneration->individuals[i]);

			mutationCount++;
		}
	}

	return mutationCount;
}

