#ifndef SIMULATES_H
#define SIMULATES_H

#define FAST_SIM
#define POWERTRAIN_2017
#define ESCORPIOEVO3
#include "LondonTrack17.c"
#include "Escorpio17.c"
#include "StrategyTypes.h"


#define SPACESTEP 15

float StartSpeed = 4.1;
#define MinAvgSpeed 7.2

//int Simulate(float *TractionForceProfileS, float *energy, unsigned char print, unsigned char save, unsigned char sequential);

float motorForce(MotorMap *map, float Speed)
{
	float MotorSpeed = Speed * 2 / WheelDiameter / TransmissionRatio;
	// Driver 2014 Model
	float OptimalTorque = (MotorSpeed - map->a0) / map->a1;
	float OptimalForce = OptimalTorque * 2 / WheelDiameter / TransmissionRatio;
	if (OptimalForce < 0) OptimalForce = 0;
	return OptimalForce;
}

int Simulate(lapClass &lap, MotorMap *mapset, float *energy, unsigned char print, unsigned char save, unsigned char sequential)
{
	TwindingsStart = 38;
	TmotorCaseStart = 35;

	Escorpio_init(SPACESTEP / MinAvgSpeed, sequential);

	// Log parametri runtime
	FILE *f;
	if (save)
	{
		f = fopen("RunTime.txt", "w");
		fprintf(f, "Time(s), Twind(°C), TMotCase(°C), Flat_i(N), Flat_o(N), Flat_r(N), Frad_i(N), Frad_o(N), Frad_r(N)\n");
	}


	float CurrentTime, CurrentSpeed, CurrentPosition, Energy, FtractionStrat;

	float Risultato[4][50000];
	int Risultato_index;
	Energy = 0;
	CurrentPosition = 0;
	CurrentSpeed = StartSpeed;
	CurrentTime = 0;
	FtractionStrat = 0;
	Risultato_index = 0;

	float MaxTime = TrackLenght / MinAvgSpeed;
	int stratindex = 0;

	while (1)
	{
		// *********************** Strategy:  *************************
		for (int p = 0; p < lap.nPulses; p++)
		{
			if ((CurrentPosition >= lap.pulses[p].startSpace) && (CurrentPosition <= (lap.pulses[p].startSpace+lap.pulses[p].pulseLenght)))
			{
				
				if ((lap.pulses[p].engineMap > 0) && (lap.pulses[p].engineMap <= MOTORMAPSPERSET))
				{
					FtractionStrat = min(motorForce(&mapset[lap.pulses[p].engineMap - 1], CurrentSpeed), FtractionMax(CurrentSpeed));
				}
				else
					FtractionStrat = 0;
				break;
				
				//FtractionStrat = min(lap.pulses[p].pulseForce, FtractionMax(CurrentSpeed));
				break;
			}
			else
				FtractionStrat = 0;
		}
		stratindex++;
		// *********************** End Strategy: *************************

		if (CurrentSpeed<0)
		{
			if (save) fclose(f);
			return 1; // ERROR
		}

		if (CurrentTime>MaxTime)
		{
			if (save) fclose(f);
			return 3; // ERROR
		}

		// Dinamica del Veicolo
		float Ftot = FtractionStrat - Fresistent(CurrentSpeed, Tslope(CurrentPosition), Tradius(CurrentPosition));
		float acc = Ftot / VehicleInertialMass;
		float delta = CurrentSpeed*CurrentSpeed + 2 * acc*SPACESTEP;
		if (delta<0)
		{
			if (save) fclose(f);
			return 1; // ERROR
		}
		float dT = (sqrt(delta) - CurrentSpeed) / acc;
		float NewSpeed = CurrentSpeed + acc*dT;
		float EnergyStep = dT*Power(FtractionStrat, CurrentSpeed);

		//Log Runtime
		if (save) fprintf(f, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", CurrentTime, Twindings, TmotorCase, FLAT_fi, FLAT_fo, FLAT_r, FRAD_fi, FRAD_fo, FRAD_r, CurrentPosition);

		Energy += EnergyStep;
		CurrentPosition += SPACESTEP;
		CurrentTime += dT;
		CurrentSpeed = NewSpeed;

		Risultato[0][Risultato_index] = EnergyStep;
		Risultato[1][Risultato_index] = FtractionStrat;
		Risultato[2][Risultato_index] = CurrentSpeed;
		Risultato[3][Risultato_index] = CurrentTime * 1000;
		Risultato_index++;
		// ********************

		if (CurrentPosition >= TrackLenght) //((RaceLaps-1)*TrackLenght)+TrackFinish)
			break;
	}

	if (CurrentSpeed<StartSpeed)
	{
		if (save) fclose(f);
		return 2; // ERROR
	}

	float KineticEnergy = 0.5*VehicleInertialMass*CurrentSpeed*CurrentSpeed;
	if (print)
	{
		printf("Simulation Finished!\n");
		printf("   Time : %f\n", CurrentTime);
		printf("   Space: %f\n", CurrentPosition);
		printf("   Speed: %f\n", CurrentSpeed);
		printf("   AvgS : %f\n", CurrentPosition / CurrentTime);
		if (CurrentTime <= RaceTime)
			printf("Run VALID\n");
		else
			printf("Run NOT VALID\n");
		printf("   Energy: %f\n", Energy);
		printf("   Corrected Energy: %f\n", Energy - KineticEnergy / 0.90); // Tengo presente il rendimento medio del powertrain
	}
	if (save) fclose(f);

	//*****************************************************************************************************
	// Salvataggio
	//*****************************************************************************************************
	if (save)
	{
		long ks;
		double s = 0;
		FILE *Resfile, *ResfileS, *Timefile, *GASfile, *Spacefile, *StratFile;
		printf("Salvataggio risultato in Res.txt...\n");
		Resfile = fopen("Res.txt", "w");
		StratFile = fopen("Strat.dat", "w");
		fprintf(Resfile, "Time,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++)
			fprintf(Resfile, "%f,", Risultato[3][ks] / 1000);
		fprintf(Resfile, "\nEnergy,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++) fprintf(Resfile, "%f,", Risultato[0][ks]);
		fprintf(Resfile, "\nGAS,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++) fprintf(Resfile, "%f,", Risultato[1][ks]);
		fprintf(Resfile, "\nSpeed,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++) fprintf(Resfile, "%f,", Risultato[2][ks]);

		/*
		for (int i = 0; i <= (TrackLenght / SPACESTEP); i++)
		{
			fprintf(StratFile, "%f,", TractionForceProfileS[i]);
		}*/

		fwrite(&lap, sizeof(lap), 1, StratFile);

		printf("Salvato.\n");
		fclose(Resfile);
		fclose(StratFile);
	}
	//****************************************************************************************************************

	*energy = Energy; //-KineticEnergy/0.90;
	return 0;
}


#endif