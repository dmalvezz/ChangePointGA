//****************************************************//
// London Track 2017 model                            //
//****************************************************//

#ifndef TRACK_H
#define TRACK_H

#include "../utils/mathutils.h"

#define RACE_LAPS	15
#define RACE_TIME   2100	// s
#define TRACK_LENGTH 970	// m

#define TRACK_DATA_STEP	10
#define TRACK_DATA_LENGTH 99

//Pendenze da topografica shell
extern float SLOPE_DATA[TRACK_DATA_LENGTH];

//Raggi stimati da topografica
extern float RADIUS_DATA[TRACK_DATA_LENGTH];

//
extern float* TRACK_SLOPE;

extern float* TRACK_RADIUS;


extern void generateTrackData(float spaceStep);

extern void disposeTrackData();

extern float getTrackSlope(float position, float spaceStep);

extern float getTrackRadius(float position, float spaceStep);

#endif
