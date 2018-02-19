/*
 * changepoint.c
 *
 *  Created on: Feb 1, 2018
 *      Author: davide
 */


#include "changepoint.h"
#include "../model/simulation.h"


void printChangePoint(ChangePoint point){
	wprintw(debugWindow, "{%d, %d}", point.positionIndex, point.action);
}

int getCurrentMap(Action action, int map){
	if(action == ACTION_PLUS){
		map++;
	}
	else if(action == ACTION_MINUS){
		map--;
	}

	if(map > MAP_COUNT){
		map = MAP_COUNT;
	}
	else if(map < 0){
		map = 0;
	}

	return map;
}
