/*
 ============================================================================
 Name        : powertrain.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Power train configuration
 ============================================================================
 */

#ifndef MODEL_POWERTRAIN_POWERTRAIN_H_
#define MODEL_POWERTRAIN_POWERTRAIN_H_

#ifdef RE40_24V
	#define MOTOR_NAME	"RE40_24V"

	//Modello elettrico driver
	#define DRV_Pq 		1.183
	#define DRV_ki 		-0.04425
	#define DRV_kv 		0.001047
	#define DRV_req 	0.03708

	//Modello elettromeccanico
	#define MotorSC           33.2       // (RAD/s)/V
	#define MotorSTG          348.5      // (RAD/s)/Nm
	#define MotorTC           0.0302     //  Nm/A
	#define MotorMattrito     0.0045     //  Nm
	#define MotorR     0.299   // ohm
	#define alpha_cu   0.00426

	//Modello termico
	static float RthCaseAmbient = 4.7; // K/W
	#define RthWindingCase 1.9 // K/W
	#define CthWinding     ((71.1/RthWindingCase)  /2.0) // J/K est
	#define CthCase        ((383.0/RthCaseAmbient) /2.0) // J/K est

	//Massa motore
	#define MotorMass      0.48 // kg

	//Trasmissione
	#define TransmissionEff   	0.99
	#define TransmissionRatio 	(1 / 25.0)    //RE40 48V new ratio
#endif

#ifdef RE40_48V
	#define MOTOR_NAME	"RE40_48V"

	//Modello elettrico driver
	#define DRV_Pq 		1.183
	#define DRV_ki 		-0.04425
	#define DRV_kv 		0.001047
	#define DRV_req 	0.03708

	//Modello elettromeccanico
	#define MotorSC           16.54      // (RAD/s)/V
	#define MotorSTG          310.86     // (RAD/s)/Nm
	#define MotorTC           0.0603     //  Nm/A
	#define MotorMattrito     0.0045     //  Nm
	#define MotorR     1.13   // ohm
	#define alpha_cu   0.00426

	//Modello termico
	static float RthCaseAmbient = 4.7; // K/W
	#define RthWindingCase 1.9 // K/W
	#define CthWinding     ((71.1/RthWindingCase)  /2.0) // J/K est
	#define CthCase        ((383.0/RthCaseAmbient) /2.0) // J/K est

	//Massa motore
	#define MotorMass      0.48 // kg

	//Trasmissione
	#define TransmissionEff   	0.99
	#define TransmissionRatio 	(1 / 25.0)    //RE40 48V new ratio
#endif

#ifdef RE50_24V
	#define MOTOR_NAME	"RE50_24V"

	//Modello elettrico driver
	#define DRV_Pq 		1.183
	#define DRV_ki 		-0.04425
	#define DRV_kv 		0.001047
	#define DRV_req 	0.03708

	//Modello elettromeccanico
	#define MotorSC			25.2        // (RAD/s)/V
	#define MotorSTG		71.93       // (RAD/s)/Nm
	#define MotorTC			0.0396      // Nm/A
	#define MotorMattrito	0.00986    // Nm
	#define MotorR     		0.103   	// ohm
	#define alpha_cu   		0.00426

	//Massa motore
	#define MotorMass      1.1 // kg

	//Modello termico
	#define RthCaseAmbient 	3.81 // K/W
	#define RthWindingCase 	1.24 // K/W
	#define CthWinding     	(71.1/RthWindingCase)  // J/K
	#define CthCase        	(383.0/RthCaseAmbient) // J/K

	//Trasmissione
	#define TransmissionEff   	0.99
	#define TransmissionRatio 	(1 / 32.0)    //RE50 24V new ratio
#endif

#ifdef RE50_36V
	#define MOTOR_NAME	"RE50_36V"

	//Modello elettrico driver
	#define DRV_Pq 		1.183
	#define DRV_ki 		-0.04425
	#define DRV_kv 		0.001047
	#define DRV_req 	0.03708

	//Modello elettromeccanico
	#define MotorSC           16.5         // (RAD/s)/V
	#define MotorSTG          66.77        // (RAD/s)/Nm
	#define MotorTC           0.0604       //  Nm/A
	#define MotorMattrito	  0.00986      //  Nm
	#define MotorR     0.244   // ohm
	#define alpha_cu   0.00426

	//Modello termico
	#define RthCaseAmbient 	3.81 // K/W
	#define RthWindingCase 	1.24 // K/W
	#define CthWinding     	(71.1/RthWindingCase)  // J/K
	#define CthCase        	(383.0/RthCaseAmbient) // J/K

	//Massa motore
	#define MotorMass      1.1 // kg

	//Trasmissione
	#define TransmissionEff   	0.99
	//#define TransmissionRatio 	(1 / 26.0)    //RE50 36V new ratio
	#define TransmissionRatio 	(1 / 25.0)    //RE50 36V new ratio

#endif





#endif /* MODEL_POWERTRAIN_POWERTRAIN_H_ */
