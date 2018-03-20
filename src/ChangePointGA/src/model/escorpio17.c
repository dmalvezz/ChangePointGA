/*
 * escorpio17.c
 *
 *  Created on: Nov 24, 2017
 *      Author: davide
 */

#include "simulation.h"

float getPower (Simulation_ptr simulation, float Ftraction, float Speed, float dt) {
	float MotorPd = 0;
	float ret;
	if (Ftraction>=0)
	{
		// Wheel Model
		float WheelSpeed  = Speed*2/WheelDiameter;
		float WheelTorque = Ftraction*WheelDiameter/2;
		//************

		// Transmission Model
		float MotorSpeed = WheelSpeed/TransmissionRatio;
		float MotorTorque  = WheelTorque*TransmissionRatio*(1/TransmissionEff);
		//************

		// Motor Model
		float MotorVoltage = (MotorSpeed+(MotorSTG*MotorTorque))/MotorSC;
		float MotorCurrent;
		float MotDeltaR = MotorR *( ( (1+simulation->Twindings*alpha_cu) / (1+25.0*alpha_cu) ) -1 );
		if(MotorTorque>0)
		{
			MotorCurrent = (MotorMattrito+MotorTorque)/MotorTC;
			MotorVoltage += MotDeltaR*MotorCurrent*MotorCurrent;
		}
		else
		{
			MotorCurrent = 0;
		}
		// ************

		MotorPd = MotorVoltage*MotorCurrent - MotorSpeed*MotorTorque;

		// Driver Model
#ifdef POWERTRAIN_FLAT
		ret  = Ftraction*Speed/ POWERTRAIN_FLAT_EFF;
#endif

#ifndef POWERTRAIN_FLAT
		float eff = MotorVoltage*MotorCurrent / (MotorVoltage*MotorCurrent + DRV_Pq + DRV_req*MotorCurrent*MotorCurrent + DRV_ki*MotorCurrent + DRV_kv*MotorVoltage);
		if (eff < 0.6) eff = 0.6;
		if (eff > 0.99) eff = 0.99;
		ret = (MotorVoltage*MotorCurrent)/eff;
#endif
	}
	else // Ftraction<0
	{
		// Futura implementazione frenata rigenerativa?
		//ret = 0;

		// Wheel Model
		float WheelSpeed  = Speed*2/WheelDiameter;
		float WheelTorque = Ftraction*WheelDiameter/2;
		//************

		// Transmission Model
		float MotorSpeed = WheelSpeed/TransmissionRatio;
		float MotorTorque  = WheelTorque*TransmissionRatio*(1/TransmissionEff);
		//************

		// Motor Model
		float MotorVoltage = (MotorSpeed+(MotorSTG*MotorTorque))/MotorSC;
		float MotorCurrent;
		float MotDeltaR = MotorR *( ( (1+simulation->Twindings*alpha_cu) / (1+25.0*alpha_cu) ) -1 );
		MotorCurrent = (MotorMattrito+MotorTorque)/MotorTC;
		MotorVoltage += MotDeltaR*MotorCurrent*MotorCurrent;

		MotorPd = MotorVoltage*MotorCurrent - MotorSpeed*MotorTorque;

		// Driver Model
#ifdef POWERTRAIN_FLAT
		ret  = Ftraction*Speed/ POWERTRAIN_FLAT_EFF;
#endif

#ifndef POWERTRAIN_FLAT
		float eff = MotorVoltage*MotorCurrent / (MotorVoltage*MotorCurrent + DRV_Pq + DRV_req*MotorCurrent*MotorCurrent + DRV_ki*MotorCurrent + DRV_kv*MotorVoltage);
		if (eff < 0.6) eff = 0.6;
		if (eff > 0.99) eff = 0.99;
		ret = (MotorVoltage*MotorCurrent)/eff;
#endif
	}

	// Aggiornamento Temperatura del motore
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//if (Sequential)
	//{
		float PtoAmbient = (simulation->TmotorCase-Trear) / RthCaseAmbient;
		float PwindtoCase = (simulation->Twindings-simulation->TmotorCase)/RthWindingCase;
		float PtoCase = PwindtoCase-PtoAmbient;
		float PtoWind = MotorPd - PwindtoCase;
		simulation->Twindings += (PtoWind*dt)/CthWinding;
		simulation->TmotorCase += (PtoCase*dt)/CthCase;
	//}

	// Motor swich-on penalty
	//if (Sequential)
	//{
		if (Ftraction >= MotorStartPenaltyThr)
		{
			if (simulation->lastTractiveForce < MotorStartPenaltyThr)
			{
				ret += MotorStartPenalty/dt;
			}
		}
		simulation->lastTractiveForce = Ftraction;
	//}

	return ret;
}


float getForcebearings(){
	return BearingCr * VehicleMass * gravity;
}

float getForceSlope(float TrackSlope){
	return VehicleMass * gravity * sin_f(TrackSlope);
}

float getForceAero(float Speed, float TrackRadiusInv){
	float Faero = 0.5 * AirDensity * Speed * Speed * FrontalArea * VehicleCx;

	float Yaw = (25 / 0.10) * TrackRadiusInv;  // Deg

	float YawFactor = 3E-17*Yaw*Yaw*Yaw*Yaw*Yaw + 2E-05*Yaw*Yaw*Yaw*Yaw + 5E-14*Yaw*Yaw*Yaw + 0.0037*Yaw*Yaw + 2E-11*Yaw + 1;

	return Faero * YawFactor;
}

float getForceTyres(float Speed, float TrackRadiusInv){
	float TractiveForce;

#ifndef FAST_SIM
	// Nessuna differenza sul verso della sterzata
	float TrackRadiusInvSign = (TrackRadiusInv>0) ? 1.0 : -1.0;
	TrackRadiusInv = fabs(TrackRadiusInv);
	// Tricycle model
	// ripartizione dei pesi
	float Fzr = VehicleMass*gravity*(1 - VehicleXg / VehicleWheelBase);
	//FRAD_r = Fzr;
	float Fzi = VehicleMass*(gravity*VehicleXg / VehicleWheelBase / 2 - VehicleZg*Speed*Speed / VehicleTrack*TrackRadiusInv);
	//FRAD_fi = Fzi;
	if (Fzi<0) Fzi = 0;
	float Fzo = VehicleMass*(gravity*VehicleXg / VehicleWheelBase / 2 + VehicleZg*Speed*Speed / VehicleTrack*TrackRadiusInv);
	//FRAD_fo = Fzo;
	if (Fzo<0) Fzo = 0;
	// caratteristiche delle gomme !!!!!!!!!!!!
	// The magic formula
	float Car = (a30 + a31*TyrePressure)*sin_f(2 * atan_f(Fzr / 1000 / (a40 + a41*TyrePressure))) * 180 / 3.14;
	float Cai = (a30 + a31*TyrePressure)*sin_f(2 * atan_f(Fzi / 1000 / (a40 + a41*TyrePressure))) * 180 / 3.14;
	float Cao = (a30 + a31*TyrePressure)*sin_f(2 * atan_f(Fzo / 1000 / (a40 + a41*TyrePressure))) * 180 / 3.14;
	float Crr = TyresCr * (-0.0156*pow(TyrePressure, 3) + 0.289*pow(TyrePressure, 2) - 1.803*TyrePressure + 4.587);
	float Cri = Crr;
	float Cro = Crr;
	// Sterzo anteriore di Ackerman
	float angleo, anglei;
	if (TrackRadiusInv != 0)
	{
		angleo = atan_f(VehicleWheelBase / (1 / TrackRadiusInv + VehicleTrack / 2));
		anglei = atan_f(VehicleWheelBase / (1 / TrackRadiusInv - VehicleTrack / 2));
	}
	else angleo = anglei = 0;
	//angleo = anglei; // Sterzo parallelo
	// Geometria ipotetica: Camber: lineare 0-10 gradi tra 0-0.1 trackradiusinv
	float Cambo = TrackRadiusInv*10.0 / 0.1*3.14 / 180;
	float Cambi = Cambo = 0;

	// Toe in angle
	angleo += TyresFrontToeIn / 180.0*3.14;
	anglei -= TyresFrontToeIn / 180.0*3.14;
	// Angolo fuori asse
	if (TrackRadiusInvSign < 0)
	{
		angleo -= TyresFrontMisAlignement / 180.0*3.14;
		anglei -= TyresFrontMisAlignement / 180.0*3.14;
	}
	else
	{
		angleo += TyresFrontMisAlignement / 180.0*3.14;
		anglei += TyresFrontMisAlignement / 180.0*3.14;

	}
	// Camber angle
	Cambo += TyresFrontCamber / 180.0*3.14;
	Cambi -= TyresFrontCamber / 180.0*3.14;

	// forza longitudinale
	float Fxr = Crr*Fzr;
	float Fxi = Cri*Fzi*(1 + 0.0005*pow((Cambi*3.14 / 180), 3) - 0.01*pow((Cambi*3.14 / 180), 2) + 0.076*(Cambi*3.14 / 180));
	float Fxo = Cro*Fzo*(1 + 0.0005*pow((Cambo*3.14 / 180), 3) - 0.01*pow((Cambo*3.14 / 180), 2) + 0.076*(Cambo*3.14 / 180));
	// forza centrifuga
	float Fc = Speed*Speed*TrackRadiusInv*VehicleMass;


	// Ricerca iterativa
	// Determino iterativamente il miglior angolo beta che individua la posizione del centro di rotazione
	//float betamax = asin_f(VehicleXg*TrackRadiusInv);

	float Slipo, Slipi, Slipr, e;
	float Fyr, Fyi, Fyo;

	float y, dy, dfyr, dfyo, dfyi;
	float beta = 0;


	//printf("betamax %f\n", betamax);
	float sin_angleo = sin_f(angleo);
	float cos_angleo = cos_f(angleo);
	float sin_anglei = sin_f(anglei);
	float cos_anglei = cos_f(anglei);
	float sin_beta,cos_beta;


	for (int it = 0; it < 5; it++) {
		sin_beta = sin_f(beta);
		cos_beta = cos_f(beta);
		//Slip angles
		if (TrackRadiusInv > 0.0000001 || TrackRadiusInv < -0.0000001) {
			e = VehicleXg - sin_beta / TrackRadiusInv;
			Slipr = atan_f(e*TrackRadiusInv / cos_beta);
			Slipi = anglei - atan_f((VehicleWheelBase - e) / (1 / TrackRadiusInv*cos_beta - VehicleTrack / 2));
			Slipo = angleo - atan_f((VehicleWheelBase - e) / (1 / TrackRadiusInv*cos_beta + VehicleTrack / 2));
		}
		else {
			e = 0;
			Slipr = Slipi = Slipo = 0;
		}
		//Forze trasversali
		/*FLAT_r = */Fyr = Car*Slipr;
		/*FLAT_fi = */Fyi = Cai*Slipi + Cambi*Cai / 10; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		/*FLAT_fo = */Fyo = Cao*Slipo + Cambo*Cao / 10;

		// Equazione dell'equilibrio lungo l'asse trasversale
		y = Fc*cos_beta + Fxo*sin_angleo + Fxi*sin_anglei - Fyr - Fyo*cos_angleo - Fyi*cos_anglei;
		dy = 1;

		if (TrackRadiusInv > 0.0000001 || TrackRadiusInv < -0.0000001) {
			dy = -Fc * sin_beta;

			/*
			dfyr = Car * 1 / (1 + pow(e * TrackRadiusInv / cos_f(beta), 2)) * (-1 + e * TrackRadiusInv * sin_f(beta) / pow(cos_f(beta), 2));

			dfyo = Cao * cos_f(angleo) * (-1 / (1 + pow((VehicleWheelBase - e) / (cos_f(beta) / TrackRadiusInv + VehicleTrack / 2), 2)))
				* ((pow(cos_f(beta) / TrackRadiusInv, 2) + (VehicleTrack * cos_f(beta) + 2 * (VehicleWheelBase - e) * sin_f(beta) / (2 * TrackRadiusInv))) / (pow(cos_f(beta) / TrackRadiusInv + VehicleTrack / 2, 2)));

			dfyi = Cai * cos_f(anglei) * (-1 / (1 + pow((VehicleWheelBase - e) / (cos_f(beta) / TrackRadiusInv - VehicleTrack / 2), 2)))
				* ((pow(cos_f(beta) / TrackRadiusInv, 2) - (VehicleTrack * cos_f(beta) + 2 * (VehicleWheelBase - e) * sin_f(beta) / (2 * TrackRadiusInv))) / (pow(cos_f(beta) / TrackRadiusInv - VehicleTrack / 2, 2)));
			*/

			dfyr = Car * (-1 + e * TrackRadiusInv * sin_beta / pow(cos_beta, 2)) / (1 + pow(e * TrackRadiusInv / cos_beta, 2)) ;

			dfyo = -Cao * cos_angleo *
				(pow(cos_beta / TrackRadiusInv, 2) + (VehicleTrack * cos_beta + 2 * (VehicleWheelBase - e) * sin_beta / (2 * TrackRadiusInv))) /
				(pow(VehicleWheelBase - e, 2) + (pow(cos_beta / TrackRadiusInv + VehicleTrack / 2, 2)));

			dfyi = -Cai * cos_f(anglei) *
				(pow(cos_beta / TrackRadiusInv, 2) - (VehicleTrack * cos_beta + 2 * (VehicleWheelBase - e) * sin_beta / (2 * TrackRadiusInv))) /
				(pow(VehicleWheelBase - e, 2) + (pow(cos_beta / TrackRadiusInv - VehicleTrack / 2, 2)));

			dy -= (dfyr + dfyo + dfyi);
		}

		//printf("%.10f  %.10f   %.10f \n", beta, y, dy);
		beta = beta - y / dy;

	}

	//float eq = Fc*cos_f(beta) + Fxo*sin_f(angleo) + Fxi*sin_f(anglei) - (Fyr + Fyo*cos_f(angleo) + Fyi*cos_f(anglei));


	//printf("beta2 %f  eq %f \n", beta, eq);

	// Debug
	//printf("Speed: %f  e %f\n", Speed, e);

	// Ho finalmente trovato il beta giusto

	TractiveForce = Fxr + Fxo*cos_angleo + Fxi*cos_anglei + Fyo*sin_angleo + Fyi*sin_anglei - Fc*sin_f(beta);
	// Introduce now cornering factor correction
	float straightTractiveForce = Fzi*Cri + Fzo*Cro + Fzr*Crr;

	TractiveForce = TractiveForce + (TractiveForce - straightTractiveForce)*(TyresCorneringCorrFactor - 1) * 2;
	//float TractiveForceb; // Tractive perpendicolare a Fc
	//TractiveForceb = Fxr*cos_f(beta) + Fxo*cos_f(angleo-beta) + Fxi*cos_f(anglei-beta)+ Fyo*sin_f(anglei-beta) + Fyi*sin_f(anglei-beta) - Fyr*sin_f(beta);

	/*
	//if (FrontWheelMaxFy<Fyo*cos_f(angleo)) FrontWheelMaxFy = Fyo*cos_f(angleo);
	//if (FrontWheelMaxFy<Fyi*cos_f(anglei)) FrontWheelMaxFy = Fyi*cos_f(anglei);

	//if (FrontWheelMaxSlip<Slipi) FrontWheelMaxSlip = Slipi;
	//if (FrontWheelMaxSlip<Slipo) FrontWheelMaxSlip = Slipo;
	 */
#else
	// Approssimazione veloce del modello triciclo
	// drag = a + b * (radinv(m))^3/2 * (speed(km/h)+5)^4
	float temp = Speed*3.6 + 5;

	//TractiveForce = 2.368 + 0.0005064 * pow((TrackRadiusInv), 1.5F) *temp*temp*temp*temp;// pow(Speed*3.6+5, 4);
	//TractiveForce = 1973*TyresCr + 0.0005064 * pow((TrackRadiusInv), 1.5F) *temp*temp*temp*temp;// pow(Speed*3.6+5, 4);
	//Dopo correzione 2016:
	float Cr = TyresCr * (-0.0156*pow(TyrePressure, 3) + 0.289*pow(TyrePressure, 2) - 1.803*TyrePressure + 4.587);
	TractiveForce = 0.5342 / 0.0008*Cr + 0.0004782 / 0.0008*Cr * pow((TrackRadiusInv), 1.5F)*TyresCorneringCorrFactor*temp*temp*temp*temp;// pow(Speed*3.6+5, 4);
	// Scale to mass:
	TractiveForce = TractiveForce / 98.0*VehicleMass;
#endif
	// Aero Drag on tyres    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//TractiveForceb += (0.0153*Speed*Speed - 0.0124*Speed)*0.5;
	TractiveForce += (0.00765*Speed*Speed - 0.0062*Speed);

	return TractiveForce;
}


float getForceResistent(float Speed, float TrackSlope, float TrackRadiusInv){
	return getForcebearings() +
			getForceSlope(TrackSlope) +
			getForceAero(Speed, TrackRadiusInv) +
			getForceTyres(Speed, TrackRadiusInv);
}

float getMaxForceTraction(float Speed){
	// Wheel Model
	float WheelSpeed  = Speed*2/WheelDiameter;
	// Transmission Model
	float MotorSpeed = WheelSpeed/TransmissionRatio;
	// Motor Model
	float MotorTorque = ((BatteryVoltage*MotorSC)-MotorSpeed)/MotorSTG;
	// Transmission Model
	float WheelTorque = MotorTorque/TransmissionRatio*TransmissionEff;
	//************
	// Wheel Model
	float Ftraction = WheelTorque/(WheelDiameter/2);
	// Theorical max Ftraction
	// Pratical: limited to 400W, <60N
	if ((Speed*Ftraction)>MaxMotorPower)
		Ftraction = MaxMotorPower/Speed;
	if (Ftraction>MaxWheelForce)
		Ftraction = MaxWheelForce;
	if (Ftraction<0)
		Ftraction = 0;
	return Ftraction;
}

float getForceTraction(float Speed, int map){
	if(map == 0){
		return 0;
	}

	return min(getMapForce(&maps[map - 1], Speed), getMaxForceTraction(Speed));
}

