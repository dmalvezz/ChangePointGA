//****************************************************//
// Escorpio 2017 model                                //
//****************************************************//

#include <math.h>



#include "powertrain/powertrain.h"


#define POWERTRAIN_FLAT_EFF 0.89

#define gravity           9.81       //  m/s^2

//Penalty
#define MotorStartPenaltyThr	0.00001  // N
#define MotorStartPenalty		70.0     // J Energy to switch on motor

//Masses
#ifdef ESCORPIOEVO3
	#define BareBoneMass 	27.0 //28.0
#else
	#define BareBoneMass	35.0;
#endif
#define DriverMass          	 	56.0
#define VehicleMass          		(BareBoneMass+DriverMass)  //  Kg
#define RotationalEqivalentMass 	4.0
#define VehicleInertialMass  		(VehicleMass + RotationalEqivalentMass)  //  Kg

//Bearing
#define BearingCr            0.00008

//Aerodinamycs
#define Tamb			25.0	 	//°C
#define Trear			25.0        // °C Temp vano posteriore
#define AirDensity      (101325/287.058/(273+Tamb))
//#define AirDensity           1.225     // 15°C
//#define AirDensity           1.17     // 30°C

#ifdef ESCORPIOEVO3
	#define FrontalArea         0.35      // m^2
	#define VehicleCx           0.12     // 0.11 //0.16 //0.12
#else
	#define FrontalArea 		0.41;      // m^2
	#define VehicleCx           0.12 //0.16 //0.12
#endif

//Wheels
#define WheelDiameter     0.466      //  m
#define TyresCalpha       105		// 90  (non più utilizzato)

// Costanti della "Magic Formula"
#define a30 	57.806
#define a31 	15.101
#define a40 	-0.082
#define a41 	0.186
#define TyresCr           			0.0012 // (Michelin official 4bar: 0.001)   // Cr a 4bar
#define TyresCorneringCorrFactor 	1.35

//Nuovo assale Carbonio
#define TyresFrontCamber		 	0.1 // deg (camber ruote anteriori)
#define TyresFrontToeIn		 		0.0 // deg (toe in ruote anteriori)
#define TyresFrontMisAlignement		0.0 // deg (non-allineamento tra ruote anteriori e posteriori)

#define VehicleWheelBase  	1.60 // m (passo)
#define VehicleTrack      	0.52 // m (carreggiata)
#define VehicleXg         	0.90 // m (distanza centro di massa - ruota posteriore)
#define VehicleZg         	0.52 // m (distanza centro di massa - piano stradale)
#define TyrePressure      	6.00 // bar (London pressure) 5.00; // bar (!!!! tra 4 e 6.5 bar)

/*
float TyresFrontCamber        = 0.2; // deg (camber ruote anteriori)
float TyresFrontToeIn         = 0.2; // deg (toe in ruote anteriori)
float TyresFrontMisAlignement = 0.4; // deg (non-allineamento tra ruote anteriori e posteriori)
*/

#define MaxMotorPower      500//400 // W
#define MaxWheelForce       65//60 // N
#define BatteryVoltage     46  // V, used for fmax

// Stima momento di inerzia oggetti in moto
// Iruota = 0,075 kg*m^2
// Meq = (I/r^2)*(speed/VehicleSpeed)
// Meq(3ruote) = 0,075*3/0,233^2 = 4,1 kg

