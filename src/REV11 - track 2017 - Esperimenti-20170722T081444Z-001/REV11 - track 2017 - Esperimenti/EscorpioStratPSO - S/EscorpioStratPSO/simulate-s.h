#ifndef SIMULATES_H
#define SIMULATES_H

//#define FAST_SIM
#define POWERTRAIN_2017
//#define POWERTRAIN_2016
#define ESCORPIOEVO3
#define POWERTRAIN_FLAT
#include "LondonTrack17.c"
#include "Escorpio17.c"

//#define FIRST_LAP
//#define LAST_LAP

#define SPACESTEP 1

float StartSpeed = 6.4;

#ifdef FIRST_LAP
#define MinAvgSpeed 6.74
#elif defined LAST_LAP
#define MinAvgSpeed 6.74
#else
#define MinAvgSpeed 7.2
#endif

int Simulate(float *TractionForceProfileS, float *speed, float *energy, unsigned char print, unsigned char save, unsigned char sequential);


int Simulate(float *TractionForceProfileS, float *speed, float *energy, unsigned char print, unsigned char save, unsigned char sequential)
{
	TwindingsStart = 38;
	TmotorCaseStart = 35;

	Escorpio_init(SPACESTEP / MinAvgSpeed, sequential);

	float CurrentTime, CurrentSpeed, CurrentPosition, Energy, FtractionStrat;

	int Risultato_index;
	Energy = 0;
	CurrentPosition = 0;
#ifdef FIRST_LAP
	CurrentSpeed = 0;
#else
	CurrentSpeed = StartSpeed;
#endif
	CurrentTime = 0;
	FtractionStrat = 0;
	Risultato_index = 0;

	float MaxTime = TrackLenght / MinAvgSpeed;
	int stratindex = 0;

	double t1 = 0, t2 = 0;
	double fmean = 0;

	while (1)
	{

		clock_t t_start, t_end;
		double t_passed;

		

		// *********************** Strategy:  *************************
		FtractionStrat = min(TractionForceProfileS[stratindex], FtractionMax(CurrentSpeed));
		// *********************** End Strategy: *************************

		if (CurrentSpeed<0)
		{
			return 1; // ERROR
		}

		if (CurrentTime>MaxTime)
		{
			return 3; // ERROR
		}

		// Dinamica del Veicolo
		t_start = clock();
		float Ftot = FtractionStrat - Fresistent(CurrentSpeed, Tslope(CurrentPosition), Tradius(CurrentPosition));
		t_end = clock();
		t1 += ((double)(t_end - t_start)) / CLOCKS_PER_SEC;


		float acc = Ftot / VehicleInertialMass;
		float delta = CurrentSpeed*CurrentSpeed + 2 * acc*SPACESTEP;
		if (delta<0)
		{
			return 1; // ERROR
		}
		float dT = (sqrt(delta) - CurrentSpeed) / acc;
		float NewSpeed = CurrentSpeed + acc*dT;

		t_start = clock();
		float EnergyStep = dT*Power(FtractionStrat, CurrentSpeed);
		t_end = clock();
		t2 += ((double)(t_end - t_start)) / CLOCKS_PER_SEC;
		speed[stratindex] = NewSpeed;

		float f1 = ModelloTriciclo(CurrentSpeed, Tradius(CurrentPosition));
		float f2 = ModelloTriciclo(CurrentSpeed, Tradius(CurrentPosition));
		fmean = abs(f1 - f2);

		//Log Runtime

		Energy += EnergyStep;
		CurrentPosition += SPACESTEP;
		CurrentTime += dT;
		CurrentSpeed = NewSpeed;

		
		// ********************

		if (CurrentPosition >= TrackLenght) { //((RaceLaps-1)*TrackLenght)+TrackFinish)
			printf("break %d\n", stratindex);
			break;
		}


		stratindex++;
		//printf("Sim time: %f seconds.\n", t_passed);
	}

#ifdef LAST_LAP
	if (CurrentSpeed<0)
#else
	if (CurrentSpeed<StartSpeed)
#endif
	{
		return 2; // ERROR
	}

	float KineticEnergy = 0.5*VehicleInertialMass*CurrentSpeed*CurrentSpeed;
	
	//printf("t1 %f   t2 %f\n", t1, t2);
	printf("fmean %f \n", fmean/stratindex);

	*energy = Energy; //-KineticEnergy/0.90;
	return 0;
}




int Simulate2(float CurrentSpeed, float* nspeed, float TractionForceProfileS, float *energy, unsigned char sequential, float slope, float radius);


int Simulate2(float CurrentSpeed, float* nspeed, float TractionForceProfileS, float *energy, unsigned char sequential, float slope, float radius)
{
	TwindingsStart = 38;
	TmotorCaseStart = 35;

	Escorpio_init(SPACESTEP / MinAvgSpeed, sequential);

	float CurrentTime, CurrentPosition, Energy, FtractionStrat;

	Energy = 0;
	CurrentPosition = 0;


	CurrentTime = 0;
	FtractionStrat = 0;

	int stratindex = 0;

	//while (1)
	//{
	// *********************** Strategy:  *************************
	FtractionStrat = min(TractionForceProfileS, FtractionMax(CurrentSpeed));
	stratindex++;
	// *********************** End Strategy: *************************


	// Dinamica del Veicolo
	float Ftot = FtractionStrat - Fresistent(CurrentSpeed, slope, radius);
	float acc = Ftot / VehicleInertialMass;
	float delta = CurrentSpeed*CurrentSpeed + 2 * acc*SPACESTEP;
	/*
	if (delta<0)
	{
	return 1; // ERROR
	}
	*/
	float dT = (sqrt(delta) - CurrentSpeed) / acc;
	float NewSpeed = CurrentSpeed + acc*dT;
	float EnergyStep = dT*Power(FtractionStrat, CurrentSpeed);

	Energy += EnergyStep;
	CurrentPosition += SPACESTEP;
	CurrentTime += dT;
	CurrentSpeed = NewSpeed;
	(*nspeed) = NewSpeed;

	//}

	float KineticEnergy = 0.5*VehicleInertialMass*CurrentSpeed*CurrentSpeed;

	/*
	printf("Simulation Finished!\n");
	printf("   Time : %f\n", CurrentTime);
	printf("   Space: %f\n", CurrentPosition);
	printf("   Speed: %f\n", CurrentSpeed);
	printf("   AvgS : %f\n", CurrentPosition / CurrentTime);
	printf("   Energy: %f\n", Energy);
	printf("   Corrected Energy: %f\n", Energy - KineticEnergy / 0.90); // Tengo presente il rendimento medio del powertrain
	*/

	*energy = Energy; //-KineticEnergy/0.90;
	return 0;
}

#endif