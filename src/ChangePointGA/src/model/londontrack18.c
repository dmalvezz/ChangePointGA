/*
 * londontrack18.c
 *
 *  Created on: Dec 19, 2017
 *      Author: davide
 */

#include <stdlib.h>
#include "londontrack18.h"

//Pendenze da topografica shell
float SLOPE_DATA[TRACK_DATA_LENGTH] = { 0,-0.02,-0.042846547,-0.05422538,-0.063072925,-0.069352773,-0.074595101,-0.08032197,-0.087285745,-0.095767395,-0.108193476,-0.127293394,-0.151810535,-0.17681762,-0.198222063,-0.214587545,-0.228674205,-0.243645171,-0.260407849,-0.275607233,-0.285714394,-0.289893986,-0.292150518,-0.296758514,-0.305916008,-0.319757886,-0.33359571,-0.340862299,-0.341824967,-0.341611098,-0.345339542,-0.352201943,-0.359842055,-0.366413357,-0.370980756,-0.373344994,-0.373751343,-0.372644991,-0.369262351,-0.357892328,-0.333997352,-0.295189507,-0.235309286,-0.1513763,-0.045284575,0.080505588,0.221486529,0.371135305,0.522752759,0.761381987,0.94859503,1.083388422,1.180439664,1.234916558,1.189153571,1.072196314,0.910976993,0.731761747,0.499148458,0.205386889,0.00619856,-0.119716676,-0.194117344,-0.232701934,-0.247097346,-0.246433425,-0.237828236,-0.226083446,-0.214015009,-0.201583941,-0.186932407,-0.169011935,-0.147968219,-0.12473846,-0.10073198,-0.07742116,-0.055893747,-0.037088761,-0.022792939,-0.014205601,-0.009220824,0.003538071,0.033546642,0.079010963,0.129795093,0.176796383,0.214649101,0.239078068,0.246620805,0.238130459,0.215973777,0.187889109,0.159160237,0.131789028,0.11,0.077,0.033,0,0};

//Raggi stimati da topografica
float RADIUS_DATA[TRACK_DATA_LENGTH] = { 0.0088, 0.02056, 0.038392, 0.054566708, 0.065889003, 0.070122302, 0.073085612, 0.069159928, 0.05561195, 0.039288365, 0.027861855, 0.019863299, 0.014264309, 0.010345016, 0.007601511, 0.005681058, 0.004696741, 0.006887718, 0.005541403, 0.004238982, 0.003327287, 0.002689101, 0.002242371, 0.00192966, 0.002250762, 0.003375533, 0.002877159, 0.002374011, 0.002021808, 0.001775266, 0.001602686, 0.00148188, 0.001397316, 0.001338121, 0.001296685, 0.001627679, 0.002039376, 0.002327563, 0.004629294, 0.007740506, 0.017418354, 0.026592848, 0.042614993, 0.053830495, 0.061681347, 0.055176943, 0.03952386, 0.028026702, 0.019978691, 0.014345084, 0.010401559, 0.007641091, 0.005708764, 0.004356135, 0.004849294, 0.009394506, 0.016861868, 0.029803308, 0.048554623, 0.051988236, 0.039991765, 0.028894236, 0.021125965, 0.015688176, 0.011881723, 0.009217206, 0.007352044, 0.006046431, 0.005132502, 0.004492751, 0.006744926, 0.011921448, 0.015545014, 0.021167224, 0.038817057, 0.04517194, 0.038820358, 0.03077425, 0.021991975, 0.015844383, 0.011541068, 0.008528748, 0.006420123, 0.004944086, 0.00391086, 0.006337602, 0.013436322, 0.018405425, 0.016483798, 0.011898658, 0.008689061, 0.006442343, 0.00486964, 0.003768748, 0.002998124, 0.002458686, 0.002081081, 0, 0 };

float* TRACK_SLOPE = 0;

float* TRACK_RADIUS = 0;

static float getTrackSlopeLinInt(float position) {
	float relativePosition = position - (floor(position / TRACK_LENGTH) * TRACK_LENGTH);

	int i = relativePosition / TRACK_DATA_STEP;

	float coeff = (relativePosition - TRACK_DATA_STEP * i) / TRACK_DATA_STEP;

	float slope = SLOPE_DATA[i+1] * coeff + SLOPE_DATA[i] * (1 - coeff);

	//TODO: i + 1 % TRACK_DATA_LENGTH
	return slope * M_PI / 180.0;
}

static float getTrackRadiusLinInt(float position) {
	float relativePosition = position - ((int)(position / TRACK_LENGTH) * TRACK_LENGTH);

	int i = relativePosition / TRACK_DATA_STEP;

	float coeff = (relativePosition - TRACK_DATA_STEP * i) / TRACK_DATA_STEP;

	float radius = fabs(RADIUS_DATA[i+1]) * coeff + fabs(RADIUS_DATA[i]) * (1 - coeff);

	//TODO: i + 1 % TRACK_DATA_LENGTH
	return radius;
}



void generateTrackData(float spaceStep){
	int size = TRACK_LENGTH / spaceStep;
	TRACK_SLOPE = (float*)malloc(size * sizeof(float));
	TRACK_RADIUS = (float*)malloc(size * sizeof(float));

	for(int i = 0; i < size; i++){
		TRACK_SLOPE[i] = getTrackSlopeLinInt(i * spaceStep);
		TRACK_RADIUS[i] = getTrackRadiusLinInt(i * spaceStep);
	}

}

void disposeTrackData(){
	free(TRACK_SLOPE);
	free(TRACK_RADIUS);
}


float getTrackSlope(float position, float spaceStep){
	int index = floor(position / spaceStep);
	return TRACK_SLOPE[index];
}

float getTrackRadius(float position, float spaceStep){
	int index = floor(position / spaceStep);
	return TRACK_RADIUS[index];
}


