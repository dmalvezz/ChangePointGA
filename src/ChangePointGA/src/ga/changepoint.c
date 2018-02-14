/*
 * changepoint.c
 *
 *  Created on: Feb 1, 2018
 *      Author: davide
 */


#include "changepoint.h"


void printChangePoint(ChangePoint point){
	wprintw(debugWindow, "{%d, %d}", point.positionIndex, point.action);
}
