/*
 * test.h
 *
 *  Created on: Nov 24, 2017
 *      Author: davide
 */

#ifndef TEST_H_
#define TEST_H_

#include "ga/ga.h"

static int checkGeneration(Generation_ptr generation){
	Strategy str;

	for(int strIndex = 0; strIndex < generation->count; strIndex++){
		str = generation->individuals[strIndex];

		for(int i = 0; i < str.size; i++){

			if(i > 0 && str.points[i].positionIndex < str.points[i - 1].positionIndex){
				printStrategy(&str);
				printf("checkGeneration err1\n");
				return 0;
			}

			if(str.points[i].positionIndex < 0){
				printStrategy(&str);
				printf("checkGeneration err2\n");
				return 0;
			}

			if(str.points[i].positionIndex > TRACK_LENGTH / SPACE_STEP){
				printStrategy(&str);
				printf("checkGeneration err3\n");
				return 0;
			}

			if(str.points[i].action < ACTION_MINUS || str.points[i].action > ACTION_PLUS){
				printStrategy(&str);
				printf("checkGeneration err4\n");
				return 0;
			}
		}

	}

	return 1;
}


#endif /* TEST_H_ */
