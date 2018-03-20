/*
 * crossover.c
 *
 *  Created on: Nov 20, 2017
 *      Author: davide
 */

#include "../generation.h"


void singlePointCrossover(Strategy_ptr parent1, Strategy_ptr parent2, Strategy_ptr child1, Strategy_ptr child2){
	int cut = randFloat(0, TRACK_END_POINT) / SPACE_STEP;
	int cutIndexP1 = getChangePointNearAt(parent1, cut);
	int cutIndexP2 = getChangePointNearAt(parent2, cut);

	child1->size = parent2->size - cutIndexP2 + cutIndexP1;
	child2->size = parent1->size - cutIndexP1 + cutIndexP2;

	ChangePoint tmpBuf[MAX_CHANGE_POINT * 2];

	//Create child1
	mergeElement(tmpBuf, parent1->points, parent2->points, 0, cutIndexP1 + 1, cutIndexP2 + 1, parent2->size - 1 - cutIndexP2, sizeof(ChangePoint));
	while(child1->size > MAX_CHANGE_POINT){
		removeElement(tmpBuf, randInt(0, child1->size - 1), child1->size, sizeof(ChangePoint));
		child1->size--;
	}

	memcpy(child1->points, tmpBuf, child1->size * sizeof(ChangePoint));

	while(child1->size < MIN_CHANGE_POINT){
		addRandomChangePoint(child1);
	}



	//Create child2
	mergeElement(tmpBuf, parent2->points, parent1->points, 0, cutIndexP2 + 1, cutIndexP1 + 1, parent1->size - 1 - cutIndexP1, sizeof(ChangePoint));
	while(child2->size > MAX_CHANGE_POINT){
		removeElement(tmpBuf, randInt(0, child2->size - 1), child2->size, sizeof(ChangePoint));
		child2->size--;
	}

	memcpy(child2->points, tmpBuf, child2->size * sizeof(ChangePoint));

	while(child2->size < MIN_CHANGE_POINT){
		addRandomChangePoint(child2);
	}

}

void multiPointCrossover(Strategy_ptr parent1, Strategy_ptr parent2, Strategy_ptr child1, Strategy_ptr child2){
	int cut;
	int index1 = 0;
	int index2 = 0;
	ChangePoint tmpChild1[MAX_CHANGE_POINT * 2];
	ChangePoint tmpChild2[MAX_CHANGE_POINT * 2];

	child1->size = 0;
	child2->size = 0;
	for(int i = 0; i < LAP_COUNT; i++){
		cut = (randFloat(0, TRACK_LENGTH) + TRACK_LENGTH * i) / SPACE_STEP;

		if (i % 2 == 0) {
			while(index1 < parent1->size && parent1->points[i].positionIndex <= cut){
				tmpChild1[child1->size] = parent1->points[index1];
				child1->size++;
				index1++;
			}

			while(index2 < parent2->size && parent2->points[i].positionIndex <= cut){
				tmpChild2[child2->size] = parent2->points[index2];
				child2->size++;
				index2++;
			}
		}
		else{
			while(index1 < parent1->size && parent1->points[i].positionIndex <= cut){
				tmpChild2[child2->size] = parent1->points[index1];
				child2->size++;
				index1++;
			}

			while(index2 < parent2->size && parent2->points[i].positionIndex <= cut){
				tmpChild1[child1->size] = parent2->points[index2];
				child1->size++;
				index2++;
			}
		}
	}

	while(child1->size > MAX_CHANGE_POINT){
		removeElement(tmpChild1, randInt(0, child1->size - 1), child1->size, sizeof(ChangePoint));
		child1->size--;
	}

	while(child2->size > MAX_CHANGE_POINT){
		removeElement(tmpChild2, randInt(0, child2->size - 1), child2->size, sizeof(ChangePoint));
		child2->size--;
	}

	memcpy(child1->points, tmpChild1, child1->size * sizeof(ChangePoint));
	memcpy(child2->points, tmpChild2, child2->size * sizeof(ChangePoint));


	while(child1->size < MIN_CHANGE_POINT){
		addRandomChangePoint(child1);
	}

	while(child2->size < MIN_CHANGE_POINT){
		addRandomChangePoint(child2);
	}
}

int crossover(Generation_ptr currentGeneration, Generation_ptr nextGeneration,
		SelectionFunction selectionFunction, CrossoverFunction crossoverFunction){
	int parentIndex1, parentIndex2;
	int childCount = 0;
	//wclear(debugWindow);


	while(nextGeneration->count < currentGeneration->size){
		parentIndex1 = selectionFunction(currentGeneration);
		parentIndex2 = selectionFunction(currentGeneration);

		crossoverFunction(
				&currentGeneration->individuals[parentIndex1],
				&currentGeneration->individuals[parentIndex2],
				&nextGeneration->individuals[nextGeneration->count],
				&nextGeneration->individuals[nextGeneration->count + 1]
		);

/*
		wprintw(debugWindow, "%d %d   %d %d\n", parentIndex1, parentIndex2,
				nextGeneration->individuals[nextGeneration->count].size,
				nextGeneration->individuals[nextGeneration->count+1].size);

*/
		nextGeneration->count += 2;
		childCount += 2;
	}
	//wrefresh(debugWindow);
	//while(getch() == ERR);
	return childCount;
}

