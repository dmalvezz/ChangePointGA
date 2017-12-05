//****************************************************//
// Escorpio 2017 model                                //
//****************************************************//

#include <math.h>
//#define ESCORPIOEVO3
//#define POWERTRAIN_FLAT
//#define POWERTRAIN_2016

#define POWERTRAIN_FLAT_EFF 0.89

#define pi                3.1416
#define gravity           9.81       //  m/s^2
#define WheelDiameter     0.466      //  m

// Flag Modello sequenziale
unsigned char Sequential = 0;
float         Tstep      = 0;
float FrontWheelMaxFy   = 0;
float FrontWheelMaxSlip = 0;
// !!! Se utilizzato, chaimare la funzione Escorpio_init() prima della simulazione!


 
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

#ifdef POWERTRAIN_2017
// RE50 (targa) 
#define MotorSC           25.2         // (RAD/s)/V
#define MotorSTG          71.93        // (RAD/s)/Nm
#define MotorTC           0.0396       //  Nm/A
float  MotorMattrito  =   0.00986;      //  Nm
#define MotorR     0.103   // ohm
#define alpha_cu   0.00426  
#else
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// RE50 (36V targa) 
#define MotorSC           16.5         // (RAD/s)/V
#define MotorSTG          66.77        // (RAD/s)/Nm
#define MotorTC           0.0604       //  Nm/A
float  MotorMattrito = 0.00986;      //  Nm
#define MotorR     0.244   // ohm
#define alpha_cu   0.00426  
#endif



float RthCaseAmbient = 3.81; // K/W
#define RthWindingCase 1.24 // K/W
#define CthWinding     (71.1/RthWindingCase)  // J/K
#define CthCase        (383.0/RthCaseAmbient) // J/K

float LastTractiveForce = 0;
//float MotorStartPenaltyThr = 0.1;    // N 
//float MotorStartPenalty =  70;     // J Energy to switch on motor
float MotorStartPenaltyThr = 0.00001;    // N 
//float MotorStartPenalty =  70;     // J Energy to switch on motor
float MotorStartPenalty = 70;     // J Energy to switch on motor

float Tamb = 25.0; //°C

float Trear = Tamb;                // °C Temp vano posteriore
float TmotorCase = Trear;          // °C (Motor Case Temperature)
float Twindings  = Trear;          // °C (Motor Windings Temperature)
float TmotorCaseStart = Trear;     // °C (Motor Case Temperature)
float TwindingsStart  = Trear;     // °C (Motor Windings Temperature)

// Forze sulle ruote
float FRAD_fi, FRAD_fo, FRAD_r;
float FLAT_fi, FLAT_fo, FLAT_r;

// Trasmissione
#define TransmissionEff   0.99
//float TransmissionRatio = (1.0/24);   //RE50 24V
//float TransmissionRatio = (1.0/16.1);    //RE50 36V
//float TransmissionRatio = (1.0 / 17.05);    //RE50 36V
//float TransmissionRatio = (17.0 / 290.0);    //RE50 36V Z17
//float TransmissionRatio = (16.0 / 290.0);    //RE50 36V Z16
//float TransmissionRatio = (15.0 / 290.0);    //RE50 36V Z15
//float TransmissionRatio = (14.0 / 290.0);    //RE50 36V  Z14  <-----
//float TransmissionRatio = (13.0 / 290.0);    //RE50 36V  Z13
//float TransmissionRatio = (1.0 / 15.26);    //RE50 36V
//#define TransmissionRatio (1.0/20)
//float TransmissionRatio = (1.0 / 28.0);    //RE50 24V  

#ifdef POWERTRAIN_2017
float TransmissionRatio = (1 / 32.0);    //RE50 24V new ratio
//float TransmissionRatio = (1 / 30.0);    //RE50 24V new ratio
#else
float TransmissionRatio = (14.0 / 290.0);    //RE50 36V  Z14  <-----
#endif


// Rendimento a 300W, 40Vout: 97%
#ifdef POWERTRAIN_2017
//Legacy
#define DriverReq         0.08       //  ohm
#define DriverIq          0.02       //  A
// New accurate model
#define DRV17_Pq 1.183 
#define DRV17_ki -0.04425 
#define DRV17_kv 0.001047 
#define DRV17_req 0.03708
#else
#define DriverReq         0.08       //  ohm
#define DriverIq          0.02       //  A
#endif
//#define DriverReq         (0.18)      //  ohm  <-----
//#define DriverIq           0.035       //  A   <-----
//#define DriverReq         (0.25)      //  ohm
//#define DriverIq           0.035       //  A




#ifdef ESCORPIOEVO3
float BareBoneMass = 27.0; //28.0
#else
float BareBoneMass         = 35.0; 
#endif
#define DriverMass           56.0
#define VehicleMass          (BareBoneMass+DriverMass)  //  Kg
float RotationalEqivalentMass = 4.0; 
#define VehicleInertialMass  (VehicleMass + RotationalEqivalentMass)  //  Kg
#define BearingCr            0.00008
//#define AirDensity           1.225     // 15°C
//#define AirDensity           1.17     // 30°C
#define AirDensity       (101325/287.058/(273+Tamb)) 
#ifdef ESCORPIOEVO3
#define FrontalArea          0.35      // m^2
#define VehicleCx            0.12     // 0.11 //0.16 //0.12 
#else
float FrontalArea = 0.41;      // m^2
#define VehicleCx            0.12 //0.16 //0.12 
#endif
#define TyresCalpha       105// 90  (non più utilizzato)
#define a30 57.806 // Costanti della "Magic Formula"
#define a31 15.101
#define a40 -0.082
#define a41 0.186
#define TyresCr           0.0012 // (Michelin official 4bar: 0.001)   // Cr a 4bar
#define TyresCorneringCorrFactor 1.35

float VehicleWheelBase  = 1.60; // m (passo)
float VehicleTrack      = 0.52; // m (carreggiata)
float VehicleXg         = 0.90; // m (distanza centro di massa - ruota posteriore)
float VehicleZg         = 0.52; // m (distanza centro di massa - piano stradale)
float TyrePressure      = 6.00; // bar (London pressure) 5.00; // bar (!!!! tra 4 e 6.5 bar)

/*
float TyresFrontCamber        = 0.2; // deg (camber ruote anteriori)
float TyresFrontToeIn         = 0.2; // deg (toe in ruote anteriori)
float TyresFrontMisAlignement = 0.4; // deg (non-allineamento tra ruote anteriori e posteriori)
*/
//Nuovo assale Carbonio
float TyresFrontCamber = 0.1; // deg (camber ruote anteriori)
float TyresFrontToeIn = 0.0; // deg (toe in ruote anteriori)
float TyresFrontMisAlignement = 0.0; // deg (non-allineamento tra ruote anteriori e posteriori)

#define MaxMotorPower      500//400 // W
#define MaxWheelForce       65//60 // N
//float BatteryVoltage     = 44;  // V, used for fmax
float BatteryVoltage     = 46;  // V, used for fmax

// Stima momento di inerzia oggetti in moto
// Iruota = 0,075 kg*m^2
// Meq = (I/r^2)*(speed/VehicleSpeed)
// Meq(3ruote) = 0,075*3/0,233^2 = 4,1 kg


void Escorpio_init (float tstp, unsigned char seq)
{
	Tstep = tstp;
	Sequential = seq;
	LastTractiveForce = 0;
	if (Sequential)
	{
		Twindings = TwindingsStart;
		TmotorCase = TmotorCaseStart;
		FrontWheelMaxFy = 0;
		FrontWheelMaxSlip = 0;
	}
}


float TransmissionPwrLoss (float Ftraction, float Speed)
{
	// Wheel Model
	float WheelSpeed  = Speed*2/WheelDiameter;
	float WheelTorque = Ftraction*WheelDiameter/2;
	//************
	if (Ftraction>0)
		return WheelSpeed*WheelTorque*(1-TransmissionEff);
	else 
		return 0;
}

float GetMotorMattrito (float Speed)
{
	// Da misure di laboratorio
	float ret = 0;
	ret += 0.0046; // Base
	ret += Speed*TransmissionRatio*0.00016;
	return 0.00986; //old
	return ret;
}

float MotorPwrLoss (float Ftraction, float Speed)
{
	if (Ftraction>0)
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
		MotorMattrito = GetMotorMattrito(MotorSpeed);
		float MotorVoltage = (MotorSpeed+(MotorSTG*MotorTorque))/MotorSC;
		float MotorCurrent;
		if(MotorTorque>0)
		{
			MotorCurrent = (MotorMattrito+MotorTorque)/MotorTC;
		}
		else
		{
			MotorCurrent = 0;
		}
	
		return MotorCurrent*MotorVoltage-MotorSpeed*MotorTorque;
	}
	else
		return 0;
}

float DriverPwrLoss (float Ftraction, float Speed)
{
	if (Ftraction>0)
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
		MotorMattrito = GetMotorMattrito(MotorSpeed);
		float MotorVoltage = (MotorSpeed+(MotorSTG*MotorTorque))/MotorSC;
		float MotorCurrent;
		if(MotorTorque>0)
		{
			MotorCurrent = (MotorMattrito+MotorTorque)/MotorTC;
		}
		else
		{
			MotorCurrent = 0;
		}
		// ************
	
		// Driver Model
		float DriverVoltage = MotorVoltage+(MotorCurrent*DriverReq);
		if (DriverVoltage<0) DriverVoltage = 0;
		float DriverCurrent;
		if(MotorCurrent>0)
			DriverCurrent = MotorCurrent + DriverIq;
		else
			DriverCurrent = 0;
		// ************
		return (DriverVoltage*DriverCurrent)-(MotorVoltage*MotorCurrent);
	}
	else // Ftraction<0
	{
		return 0;
	}
}

float Power (float Ftraction, float Speed)
{
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
		MotorMattrito = GetMotorMattrito(MotorSpeed);
		float MotorVoltage = (MotorSpeed+(MotorSTG*MotorTorque))/MotorSC;
		float MotorCurrent;
		float MotDeltaR = MotorR *( ( (1+Twindings*alpha_cu) / (1+25.0*alpha_cu) ) -1 );
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
#ifndef POWERTRAIN_2017
		float DriverVoltage = MotorVoltage+(MotorCurrent*DriverReq);
		if (DriverVoltage<0) DriverVoltage = 0;
		float DriverCurrent;
		if(MotorCurrent>0)
			DriverCurrent = MotorCurrent + DriverIq;
		else
			DriverCurrent = 0;
		// ************
		ret = (DriverVoltage*DriverCurrent);
#else
		float eff = MotorVoltage*MotorCurrent / (MotorVoltage*MotorCurrent + DRV17_Pq + DRV17_req*MotorCurrent*MotorCurrent + DRV17_ki*MotorCurrent + DRV17_kv*MotorVoltage);
		if (eff < 0.6) eff = 0.6;
		if (eff > 0.99) eff = 0.99;
		ret = (MotorVoltage*MotorCurrent)/eff;
#endif
#ifdef POWERTRAIN_FLAT
		ret  = Ftraction*Speed/ POWERTRAIN_FLAT_EFF;
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
		MotorMattrito = GetMotorMattrito(MotorSpeed);
		float MotorVoltage = (MotorSpeed+(MotorSTG*MotorTorque))/MotorSC;
		float MotorCurrent;
		float MotDeltaR = MotorR *( ( (1+Twindings*alpha_cu) / (1+25.0*alpha_cu) ) -1 );
			MotorCurrent = (MotorMattrito+MotorTorque)/MotorTC;
			MotorVoltage += MotDeltaR*MotorCurrent*MotorCurrent;
		
		MotorPd = MotorVoltage*MotorCurrent - MotorSpeed*MotorTorque;

		// Driver Model
		float DriverVoltage = MotorVoltage+(MotorCurrent*DriverReq);
		if (DriverVoltage<0) DriverVoltage = 0;
		float DriverCurrent;
			DriverCurrent = MotorCurrent + DriverIq;

		ret = (DriverVoltage*DriverCurrent);
#ifdef POWERTRAIN_FLAT
		ret = Ftraction*Speed * POWERTRAIN_FLAT_EFF;
#endif
	}

	// Aggiornamento Temperatura del motore 
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (Sequential)
	{
		float PtoAmbient = (TmotorCase-Trear) / RthCaseAmbient;
		float PwindtoCase = (Twindings-TmotorCase)/RthWindingCase;
		float PtoCase = PwindtoCase-PtoAmbient;
		float PtoWind = MotorPd - PwindtoCase;
		Twindings += (PtoWind*Tstep)/CthWinding; 
		TmotorCase += (PtoCase*Tstep)/CthCase; 
	}

	// Motor swich-on penalty
	if (Sequential)
	{
		if (Ftraction >= MotorStartPenaltyThr)
		{
			if (LastTractiveForce < MotorStartPenaltyThr)
			{
				ret += MotorStartPenalty/Tstep;
			}
		}
		LastTractiveForce = Ftraction;
	}
	
	return ret;
}

float Fbearings ()
{
	// Bearings Model
	return BearingCr*VehicleMass*gravity;
	//****************
}

float Fslope(float TrackSlope)
{
	// Slope Resistance
	return VehicleMass*gravity*sin(TrackSlope);
	//****************
}

float Faero (float Speed, float TrackRadiusInv)
{
	// Aerodynamic Drag
	float Faero = 0.5*AirDensity*Speed*Speed*FrontalArea*VehicleCx;
	float Yaw = (25/0.10)*TrackRadiusInv;  // Deg
	float YawFactor = 3E-17*Yaw*Yaw*Yaw*Yaw*Yaw + 2E-05*Yaw*Yaw*Yaw*Yaw + 5E-14*Yaw*Yaw*Yaw + 0.0037*Yaw*Yaw + 2E-11*Yaw + 1;
	Faero = Faero*YawFactor;
	//*****************
	return Faero;
}

float ModelloTriciclo (float Speed, float TrackRadiusInv)
{
	float TractiveForce;

#ifndef FAST_SIM
	// Nessuna differenza sul verso della sterzata
	float TrackRadiusInvSign = (TrackRadiusInv>0) ? 1.0 : -1.0;
	TrackRadiusInv = abs(TrackRadiusInv);
	// Tricycle model
	// ripartizione dei pesi
	float Fzr = VehicleMass*gravity*(1-VehicleXg/VehicleWheelBase);
	FRAD_r = Fzr;
	float Fzi = VehicleMass*(gravity*VehicleXg/VehicleWheelBase/2 - VehicleZg*Speed*Speed/VehicleTrack*TrackRadiusInv);
	FRAD_fi = Fzi;
	if (Fzi<0) Fzi=0;
	float Fzo = VehicleMass*(gravity*VehicleXg/VehicleWheelBase/2 + VehicleZg*Speed*Speed/VehicleTrack*TrackRadiusInv);
	FRAD_fo = Fzo;
	if (Fzo<0) Fzo=0;
	// caratteristiche delle gomme !!!!!!!!!!!!
	// The magic formula
	float Car = (a30+a31*TyrePressure)*sin(2*atan(Fzr/1000/(a40+a41*TyrePressure)))*180/3.14;
	float Cai = (a30+a31*TyrePressure)*sin(2*atan(Fzi/1000/(a40+a41*TyrePressure)))*180/3.14;
	float Cao = (a30+a31*TyrePressure)*sin(2*atan(Fzo/1000/(a40+a41*TyrePressure)))*180/3.14;
	float Crr = TyresCr * (-0.0156*pow(TyrePressure, 3) + 0.289*pow(TyrePressure, 2) - 1.803*TyrePressure + 4.587);
	float Cri = Crr;
	float Cro = Crr;
	// Sterzo anteriore di Ackerman
	float angleo, anglei;
	if(TrackRadiusInv != 0)
		{angleo = atan(VehicleWheelBase/(1/TrackRadiusInv + VehicleTrack/2));
	anglei = atan(VehicleWheelBase/(1/TrackRadiusInv - VehicleTrack/2));}
	else angleo=anglei = 0;
	//angleo = anglei; // Sterzo parallelo
	// Geometria ipotetica: Camber: lineare 0-10 gradi tra 0-0.1 trackradiusinv
	float Cambo = TrackRadiusInv*10.0/0.1*3.14/180;
	float Cambi = Cambo = 0;

	// Toe in angle
	angleo += TyresFrontToeIn/180.0*3.14;
	anglei -= TyresFrontToeIn/180.0*3.14;
	// Angolo fuori asse
	if(TrackRadiusInvSign < 0)
	{
		angleo -= TyresFrontMisAlignement/180.0*3.14;
		anglei -= TyresFrontMisAlignement/180.0*3.14;
	}
	else
	{
		angleo += TyresFrontMisAlignement/180.0*3.14;
		anglei += TyresFrontMisAlignement/180.0*3.14;

	}
	// Camber angle
	Cambo += TyresFrontCamber/180.0*3.14;
	Cambi -= TyresFrontCamber/180.0*3.14;
	
	// forza longitudinale
	float Fxr = Crr*Fzr;
	float Fxi = Cri*Fzi*(1+0.0005*pow((Cambi*3.14/180), 3)-0.01*pow((Cambi*3.14/180), 2)+0.076*(Cambi*3.14/180));
	float Fxo = Cro*Fzo*(1+0.0005*pow((Cambo*3.14/180), 3)-0.01*pow((Cambo*3.14/180), 2)+0.076*(Cambo*3.14/180));
	// forza centrifuga
	float Fc = Speed*Speed*TrackRadiusInv*VehicleMass;
	
	
	// Ricerca iterativa
	// Determino iterativamente il miglior angolo beta che individua la posizione del centro di rotazione
	float betamax = asin(VehicleXg*TrackRadiusInv);
	float beta = betamax;
	//float res[10000]; int rif=0;
#define betastep (0.001*3.14/180)
	
	float Slipo, Slipi, Slipr, e;
	float Fyr, Fyi, Fyo;
	while (beta>=-betamax)
	{
		//Slip angles
		if (TrackRadiusInv != 0){
			e = VehicleXg-sin(beta)/TrackRadiusInv;
			Slipr = atan(e*TrackRadiusInv/cos(beta));
			Slipi = anglei-atan((VehicleWheelBase-e)/(1/TrackRadiusInv*cos(beta)-VehicleTrack/2));
			Slipo = angleo-atan((VehicleWheelBase-e)/(1/TrackRadiusInv*cos(beta)+VehicleTrack/2));
		} else {
			e = 0;
			Slipr = Slipi = Slipo = 0;
		}
		//Forze trasversali
		FLAT_r = Fyr = Car*Slipr;
		FLAT_fi = Fyi = Cai*Slipi + Cambi*Cai/10; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		FLAT_fo = Fyo = Cao*Slipo + Cambo*Cao/10;

		// Equazione dell'equilibrio lungo l'asse trasversale
		float equilibrio;
		equilibrio  = Fc*cos(beta) + Fxo*sin(angleo) + Fxi*sin(anglei);
		equilibrio -= Fyr + Fyo*cos(angleo) + Fyi*cos(anglei);
		//res[rif++] = equilibrio;
		beta-=betastep;
		if (equilibrio<=0) break;
	}

	float eq = Fc*cos(beta) + Fxo*sin(angleo) + Fxi*sin(anglei) - (Fyr + Fyo*cos(angleo) + Fyi*cos(anglei));

	//printf("beta1 %f  eq %f \n", beta, eq);

	// Debug
	//printf("Speed: %f  e %f\n", Speed, e);

	// Ho finalmente trovato il beta giusto
	
	TractiveForce = Fxr + Fxo*cos(angleo) + Fxi*cos(anglei)+ Fyo*sin(angleo) + Fyi*sin(anglei) - Fc*sin(beta);
	//float TractiveForceb; // Tractive perpendicolare a Fc
	//TractiveForceb = Fxr*cos(beta) + Fxo*cos(angleo-beta) + Fxi*cos(anglei-beta)+ Fyo*sin(anglei-beta) + Fyi*sin(anglei-beta) - Fyr*sin(beta);
	
	if (FrontWheelMaxFy<Fyo*cos(angleo)) FrontWheelMaxFy = Fyo*cos(angleo);
	if (FrontWheelMaxFy<Fyi*cos(anglei)) FrontWheelMaxFy = Fyi*cos(anglei);
	
	if (FrontWheelMaxSlip<Slipi) FrontWheelMaxSlip = Slipi;
	if (FrontWheelMaxSlip<Slipo) FrontWheelMaxSlip = Slipo;

	
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
	return eq;
}

float ModelloTriciclo2(float Speed, float TrackRadiusInv)
{
	float TractiveForce;

	// Nessuna differenza sul verso della sterzata
	float TrackRadiusInvSign = (TrackRadiusInv>0) ? 1.0 : -1.0;
	TrackRadiusInv = abs(TrackRadiusInv);
	// Tricycle model
	// ripartizione dei pesi
	float Fzr = VehicleMass*gravity*(1 - VehicleXg / VehicleWheelBase);
	FRAD_r = Fzr;
	float Fzi = VehicleMass*(gravity*VehicleXg / VehicleWheelBase / 2 - VehicleZg*Speed*Speed / VehicleTrack*TrackRadiusInv);
	FRAD_fi = Fzi;
	if (Fzi<0) Fzi = 0;
	float Fzo = VehicleMass*(gravity*VehicleXg / VehicleWheelBase / 2 + VehicleZg*Speed*Speed / VehicleTrack*TrackRadiusInv);
	FRAD_fo = Fzo;
	if (Fzo<0) Fzo = 0;
	// caratteristiche delle gomme !!!!!!!!!!!!
	// The magic formula
	float Car = (a30 + a31*TyrePressure)*sin(2 * atan(Fzr / 1000 / (a40 + a41*TyrePressure))) * 180 / 3.14;
	float Cai = (a30 + a31*TyrePressure)*sin(2 * atan(Fzi / 1000 / (a40 + a41*TyrePressure))) * 180 / 3.14;
	float Cao = (a30 + a31*TyrePressure)*sin(2 * atan(Fzo / 1000 / (a40 + a41*TyrePressure))) * 180 / 3.14;
	float Crr = TyresCr * (-0.0156*pow(TyrePressure, 3) + 0.289*pow(TyrePressure, 2) - 1.803*TyrePressure + 4.587);
	float Cri = Crr;
	float Cro = Crr;
	// Sterzo anteriore di Ackerman
	float angleo, anglei;
	if (TrackRadiusInv != 0)
	{
		angleo = atan(VehicleWheelBase / (1 / TrackRadiusInv + VehicleTrack / 2));
		anglei = atan(VehicleWheelBase / (1 / TrackRadiusInv - VehicleTrack / 2));
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
	float betamax = asin(VehicleXg*TrackRadiusInv);

	float Slipo, Slipi, Slipr, e;
	float Fyr, Fyi, Fyo;

	float y, dy, dfyr, dfyo, dfyi;
	float beta = betamax;

	for (int it = 0; it < 5; it++) {
		//Slip angles
		if (TrackRadiusInv > 0.0000001 || TrackRadiusInv < -0.0000001) {
			e = VehicleXg - sin(beta) / TrackRadiusInv;
			Slipr = atan(e*TrackRadiusInv / cos(beta));
			Slipi = anglei - atan((VehicleWheelBase - e) / (1 / TrackRadiusInv*cos(beta) - VehicleTrack / 2));
			Slipo = angleo - atan((VehicleWheelBase - e) / (1 / TrackRadiusInv*cos(beta) + VehicleTrack / 2));
		}
		else {
			e = 0;
			Slipr = Slipi = Slipo = 0;
		}
		//Forze trasversali
		FLAT_r = Fyr = Car*Slipr;
		FLAT_fi = Fyi = Cai*Slipi + Cambi*Cai / 10; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		FLAT_fo = Fyo = Cao*Slipo + Cambo*Cao / 10;

		// Equazione dell'equilibrio lungo l'asse trasversale
		y = Fc*cos(beta) + Fxo*sin(angleo) + Fxi*sin(anglei) - Fyr - Fyo*cos(angleo) - Fyi*cos(anglei);
		dy = 1;

		if (TrackRadiusInv > 0.0000001 || TrackRadiusInv < -0.0000001) {
			dy = -Fc * sin(beta);

			//printf("vado qui");
			/*
			dfyr = Car * 1 / (1 + pow(e * TrackRadiusInv / cos(beta), 2)) * (-1 + e * TrackRadiusInv * sin(beta) / pow(cos(beta), 2));
			
			dfyo = Cao * cos(angleo) * (-1 / (1 + pow((VehicleWheelBase - e) / (cos(beta) / TrackRadiusInv + VehicleTrack / 2), 2)))
				* ((pow(cos(beta) / TrackRadiusInv, 2) + (VehicleTrack * cos(beta) + 2 * (VehicleWheelBase - e) * sin(beta) / (2 * TrackRadiusInv))) / (pow(cos(beta) / TrackRadiusInv + VehicleTrack / 2, 2)));

			dfyi = Cai * cos(anglei) * (-1 / (1 + pow((VehicleWheelBase - e) / (cos(beta) / TrackRadiusInv - VehicleTrack / 2), 2)))
				* ((pow(cos(beta) / TrackRadiusInv, 2) - (VehicleTrack * cos(beta) + 2 * (VehicleWheelBase - e) * sin(beta) / (2 * TrackRadiusInv))) / (pow(cos(beta) / TrackRadiusInv - VehicleTrack / 2, 2)));
			*/

			dfyr = Car * (-1 + e * TrackRadiusInv * sin(beta) / pow(cos(beta), 2)) / (1 + pow(e * TrackRadiusInv / cos(beta), 2)) ;

			dfyo = -Cao * cos(angleo) * 
				(pow(cos(beta) / TrackRadiusInv, 2) + (VehicleTrack * cos(beta) + 2 * (VehicleWheelBase - e) * sin(beta) / (2 * TrackRadiusInv))) / 
				(pow(VehicleWheelBase - e, 2) + (pow(cos(beta) / TrackRadiusInv + VehicleTrack / 2, 2)));

			dfyi = -Cai * cos(anglei) *
				(pow(cos(beta) / TrackRadiusInv, 2) - (VehicleTrack * cos(beta) + 2 * (VehicleWheelBase - e) * sin(beta) / (2 * TrackRadiusInv))) /
				(pow(VehicleWheelBase - e, 2) + (pow(cos(beta) / TrackRadiusInv - VehicleTrack / 2, 2)));

			dy -= (dfyr + dfyo + dfyi);
		}

		beta = beta - y / dy;
	}

	float eq = Fc*cos(beta) + Fxo*sin(angleo) + Fxi*sin(anglei) - (Fyr + Fyo*cos(angleo) + Fyi*cos(anglei));


	//printf("beta2 %f  eq %f \n", beta, eq);

	// Debug
	//printf("Speed: %f  e %f\n", Speed, e);

	// Ho finalmente trovato il beta giusto

	TractiveForce = Fxr + Fxo*cos(angleo) + Fxi*cos(anglei) + Fyo*sin(angleo) + Fyi*sin(anglei) - Fc*sin(beta);
	//float TractiveForceb; // Tractive perpendicolare a Fc
	//TractiveForceb = Fxr*cos(beta) + Fxo*cos(angleo-beta) + Fxi*cos(anglei-beta)+ Fyo*sin(anglei-beta) + Fyi*sin(anglei-beta) - Fyr*sin(beta);

	if (FrontWheelMaxFy<Fyo*cos(angleo)) FrontWheelMaxFy = Fyo*cos(angleo);
	if (FrontWheelMaxFy<Fyi*cos(anglei)) FrontWheelMaxFy = Fyi*cos(anglei);

	if (FrontWheelMaxSlip<Slipi) FrontWheelMaxSlip = Slipi;
	if (FrontWheelMaxSlip<Slipo) FrontWheelMaxSlip = Slipo;


	// Aero Drag on tyres    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//TractiveForceb += (0.0153*Speed*Speed - 0.0124*Speed)*0.5;
	TractiveForce += (0.00765*Speed*Speed - 0.0062*Speed);
	return eq;
}


float Ftyres_straight (float Speed)
{
	// Rolling Resistance
	float Ftyres = (VehicleMass*gravity*TyresCr);
	// Aero Drag on tyres    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Ftyres += (0.0153*Speed*Speed - 0.0124*Speed)*0.5;
	//*******************
	return Ftyres;
}
float Ftyres_cornering (float Speed, float TrackRadiusInv)
{
	// Rolling Resistance
	// Utilizzo il modello ad una ruota, che fornisce una Fr il 20% più alta. Poi correggo.
	float SlipAngle = (TrackRadiusInv*VehicleMass*Speed*Speed)/(TyresCalpha-(pi/180*VehicleMass*gravity*TyresCr)); // Deg!!!!
	float Ftyres = ((100.0/120.0)*(SlipAngle*SlipAngle*(pi/180.0)*TyresCalpha));
	return Ftyres;
}

float Ftyres (float Speed, float TrackRadiusInv)
{
	/*
	// Rolling Resistance
	// Utilizzo il modello ad una ruota, che fornisce una Fr il 20% più alta. Poi correggo.
	float SlipAngle = (TrackRadiusInv*VehicleMass*Speed*Speed)/(TyresCalpha-(pi/180*VehicleMass*gravity*TyresCr)); // Deg!!!!
	float Ftyres = (VehicleMass*gravity*TyresCr)+((100.0/120.0)*(SlipAngle*SlipAngle*(pi/180.0)*TyresCalpha));
	// Aero Drag on tyres    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Ftyres += (0.0153*Speed*Speed - 0.0124*Speed)*0.5;
	//*******************
	return Ftyres;
	*/
	//return Ftyres_cornering(Speed, TrackRadiusInv)+Ftyres_straight(Speed);

	// Bicycle model
	/*
	// ripartizione dei pesi
	float Fzf = VehicleMass*gravity*VehicleXg/VehicleWheelBase;
	float Fzr = VehicleMass*gravity*(VehicleWheelBase-VehicleXg)/VehicleWheelBase;
	// caratteristiche delle gomme !!!!!!!!!!!!
	float Caf = 100*180/3.14;
	float Car = 100*180/3.14;
	float Crf = TyresCr;
	float Crr = TyresCr;
	// forza longitudinale
	float Fxf = Crf*Fzf;
	float FXr = Crr*Fzr;
	// forza centrifuga
	float Fc = Speed*Speed*TrackRadiusInv*VehicleMass;
	// angolo beta
	float beta = 0.5*asin(2*VehicleWheelBase*VehicleXg*Car*TrackRadiusInv/Fc/(VehicleWheelBase-VehicleXg));
	*/
	
	//return Ftyres_cornering(Speed, TrackRadiusInv)+Ftyres_straight(Speed);
	return ModelloTriciclo(Speed, TrackRadiusInv);
}


float Fresistent (float Speed, float TrackSlope, float TrackRadiusInv)
{
	return (Fbearings()+Fslope(TrackSlope)+Faero(Speed, TrackRadiusInv)+Ftyres(Speed, TrackRadiusInv));
}
	
float Ftraction (float Acceleration, float Speed, float TrackSlope, float TrackRadiusInv)
{
	return (VehicleInertialMass*Acceleration)+Fresistent(Speed, TrackSlope, TrackRadiusInv);
}

float FtractionMax (float Speed)
{
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