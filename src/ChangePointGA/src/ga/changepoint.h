/*
 ============================================================================
 Name        : changepoint.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : ChangePoint data structure
 ============================================================================
 */

#ifndef CHANGE_POINT_H
#define CHANGE_POINT_H

#include <stdlib.h>
#include <stdio.h>
#include "../window/window.h"


//Change point possible action
typedef enum Action{
	ACTION_NONE = 0,
	ACTION_MINUS = 1,
	ACTION_PLUS = 2,
}Action;


typedef struct ChangePoint{
	unsigned int positionIndex;			//Position index
	Action action;						//Action
}ChangePoint;

typedef ChangePoint* ChangePoint_ptr;


//Print to stdout
static void printChangePoint(ChangePoint point){
	printf("{%d, %d}", point.positionIndex, point.action);
}



#endif
