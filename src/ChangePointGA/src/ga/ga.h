/*
 ============================================================================
 Name        : population.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : GA params
 ============================================================================
 */

#ifndef GA_H_
#define GA_H_

#include "generation.h"

#define POPULATION_SIZE		200

#define ELITISM_PERCENTAGE	0.01f

#define ADD_POINT_MUTATION_RATE 		0.75f
#define REMOVE_POINT_MUTATION_RATE 		0.75f
#define CHANGE_POINT_POS_MUTATION_RATE 	0.75f
#define CHANGE_POINT_ACT_MUTATION_RATE	0.75f

#define INVALID_THRESHOLD 	0.40
#define MAX_LAST_CHANGE		1000


#endif
