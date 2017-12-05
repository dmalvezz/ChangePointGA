/*
 * map.c
 *
 *  Created on: Nov 25, 2017
 *      Author: davide
 */


#include "simulation.h"

Map maps[MAP_COUNT];

//Init a map
void initMap(Map_ptr map, float a0, float a1, float a2){
	map->a0 = a0;
	map->a1 = a1;
	map->a2 = a2;
}

//Get the force
float getMapForce(Map_ptr map, float velocity){
	float torque;

	// Wheel Model
	//float WheelSpeed  = velocity * 2 / WheelDiameter;
	// Transmission Model
	//float MotorSpeed = WheelSpeed / TransmissionRatio;

	float rpm = velocity / TransmissionRatio * 60 / (M_PI * WheelDiameter);

	//rpm = a0 + a1 * torque[mNm] + a2 * torque[mNm] ^ 2
	if(isZero(map->a2)){
		torque = (rpm - map->a0) / map->a1;
	}
	else{
		torque = (-map->a1 + sqrt(map->a1 * map->a1 - 4 * map->a2 * (map->a0 - rpm))) / (2.0 * map->a2);
	}

	torque /= 1000.0;

	return torque / (WheelDiameter / 2) / TransmissionRatio;
}
