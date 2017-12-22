/*
 * powertrain.h
 *
 *  Created on: Dec 22, 2017
 *      Author: davide
 */

#ifndef MODEL_POWERTRAIN_POWERTRAIN_H_
#define MODEL_POWERTRAIN_POWERTRAIN_H_

#ifdef RE40_24V
	// RE40 misurato
	/*
	#define TransmissionRatio (1.0/30.0)
	#define MotorSC           33.7       // (RAD/s)/V
	#define MotorSTG          400.0      // (RAD/s)/Nm
	#define MotorTC           0.032      //  Nm/A
	#define MotorMattrito     0.006      //  Nm
	*/


	// RE40 (targa)
	/*
	#define TransmissionRatio (1.0/30.0)
	#define MotorSC           33.2       // (RAD/s)/V
	#define MotorSTG          348.5      // (RAD/s)/Nm
	#define MotorTC           0.0302     //  Nm/A
	#define MotorMattrito     0.0045     //  Nm
	*/
#endif

#ifdef RE40_48V

#endif

#ifdef RE50_24V
	//Modello meccanico
	#define MotorSC			25.2        // (RAD/s)/V
	#define MotorSTG		71.93       // (RAD/s)/Nm
	#define MotorTC			0.0396      // Nm/A
	//#define MotorMattrito	0.00986    // Nm
	static float MotorMattrito = 0.00986;
	#define MotorR     		0.103   	// ohm
	#define alpha_cu   		0.00426

	//Modello elettrico
	//Legacy
	#define DriverReq		0.08       //	ohm
	#define DriverIq        0.02       // A
	//New accurate model
	#define DRV17_Pq 		1.183
	#define DRV17_ki 		-0.04425
	#define DRV17_kv 		0.001047
	#define DRV17_req 		0.03708

	//Modello termico
	#define RthCaseAmbient 	3.81 // K/W
	#define RthWindingCase 	1.24 // K/W
	#define CthWinding     	(71.1/RthWindingCase)  // J/K
	#define CthCase        	(383.0/RthCaseAmbient) // J/K

	//Trasmissione
	#define TransmissionEff   	0.99
	#define TransmissionRatio 	(1 / 32.0)    //RE50 24V new ratio
	//float TransmissionRatio = (1.0/24);   //RE50 24V
	//float TransmissionRatio = (1.0 / 28.0);    //RE50 24V

#endif

#ifdef RE50_36V
	// RE50 (36V targa)

	//Modello meccanico
	#define MotorSC           16.5         // (RAD/s)/V
	#define MotorSTG          66.77        // (RAD/s)/Nm
	#define MotorTC           0.0604       //  Nm/A
	#define MotorMattrito	  0.00986      //  Nm
	#define MotorR     0.244   // ohm
	#define alpha_cu   0.00426

	//Modello elettrico
	#define DriverReq         0.08       //  ohm
	#define DriverIq          0.02       //  A

	#define TransmissionRatio	(14.0 / 290.0)    //RE50 36V  Z14  <-----
	//float TransmissionRatio = (1.0/16.1);    //RE50 36V
	//float TransmissionRatio = (1.0 / 17.05);    //RE50 36V
	//float TransmissionRatio = (17.0 / 290.0);    //RE50 36V Z17
	//float TransmissionRatio = (16.0 / 290.0);    //RE50 36V Z16
	//float TransmissionRatio = (15.0 / 290.0);    //RE50 36V Z15
	//float TransmissionRatio = (14.0 / 290.0);    //RE50 36V  Z14  <-----
	//float TransmissionRatio = (13.0 / 290.0);    //RE50 36V  Z13
	//float TransmissionRatio = (1.0 / 15.26);    //RE50 36V

#endif





#endif /* MODEL_POWERTRAIN_POWERTRAIN_H_ */
