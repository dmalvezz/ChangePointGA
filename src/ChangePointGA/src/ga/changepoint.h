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

typedef enum GeneStatus{
	GENE_NEUTRAL = 0,
	GENE_ACTIVE = 1,
	GENE_UNKNOWN = -1
}GeneStatus;


typedef struct ChangePoint{
	GeneStatus prevStatus, currStatus;

	unsigned int positionIndex;			//Position index
	Action action;						//Action
}ChangePoint;

typedef ChangePoint* ChangePoint_ptr;


//Print to stdout
extern void printChangePoint(ChangePoint point);

//Get map
extern int getCurrentMap(Action action, int map);


#endif
