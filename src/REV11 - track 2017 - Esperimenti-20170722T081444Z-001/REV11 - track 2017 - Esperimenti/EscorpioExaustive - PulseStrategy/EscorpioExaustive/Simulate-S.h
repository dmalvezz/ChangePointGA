#ifndef SIMULATES_H
#define SIMULATES_H

#define FAST_SIM
#define POWERTRAIN_2017
#define ESCORPIOEVO3
//#define POWERTRAIN_FLAT

#include "LondonTrack17.c"
#include "Escorpio17.c"
#include "StrategyTypes.h"

/*
firstlap: 328s -> 6.84
lap: 320s -> 7.02
lastlap: 332s -> 6.47
#define LAP_AVG 7.02
#define LASTLAP_AVG 6.47
#define FIRSTLAP_AVG 6.84
*/

/*
// 14 sec safetime
//firstlap: 330s -> 6.8
//lap: 317s -> 7.08
//lastlap: 334s -> 6.44
#define LAP_AVG 7.08
#define LASTLAP_AVG 6.44
#define FIRSTLAP_AVG 6.8
*/

/*
// 14 sec safetime
//firstlap: 330s -> 6.8
//lap: 316s -> 7.10
//lastlap: 340s -> 6.325
#define LAP_AVG 7.10
#define LASTLAP_AVG 6.325
#define FIRSTLAP_AVG 6.8
*/


/*
// 14 sec safetime
//firstlap: 340s -> 6.6
//lap: 315s -> 7.124
//lastlap: 336s -> 6.4
#define LAP_AVG 7.124
#define LASTLAP_AVG 6.4
#define FIRSTLAP_AVG 6.6
*/

// London 2017
// 8 sec safetime
//firstlap: 246s -> 6.74
//lap: 230s -> 7.2
//lastlap: 246s -> 6.74
#define LAP_AVG 7.2
#define LASTLAP_AVG 6.74
#define FIRSTLAP_AVG 6.74

float exchangeSpeed = 6;

#define SPACESTEP 10


#ifdef FIRST_LAP
float StartSpeed = 0;// 5.0;
float EndSpeed = exchangeSpeed;
#define MinAvgSpeed 6.84
float EndSpace = TrackLenght;
#elseif defined(LAST_LAP)
float StartSpeed = exchangeSpeed;// 5.0;
float EndSpeed = 0;
#define MinAvgSpeed 6.47
float EndSpace = TrackFinish;
#else
float StartSpeed = exchangeSpeed;// 5.0;
float EndSpeed = StartSpeed;
float MinAvgSpeed = LAP_AVG;
float EndSpace = TrackLenght;
#endif

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

int Simulate(lapClass &lap, MotorMap *mapset, float *energy, float tempPosteriore, unsigned char print, unsigned char save, unsigned char sequential, char *directory)
{
	Trear = tempPosteriore;
	TwindingsStart = tempPosteriore;
	TmotorCaseStart = tempPosteriore;

	Escorpio_init(SPACESTEP / MinAvgSpeed, sequential);

	// Log parametri runtime
	FILE *f;
	char fname[200];
	if (save)
	{
		strcpy(fname, directory);
		strcat(fname, "RunTime.txt");
		f = fopen(fname, "w");
		fprintf(f, "Time(s), Twind(°C), TMotCase(°C), Flat_i(N), Flat_o(N), Flat_r(N), Frad_i(N), Frad_o(N), Frad_r(N)\n");
	}


	float CurrentTime, CurrentSpeed, CurrentPosition, Energy, FtractionStrat;

	float Risultato[5][50000];
	int Risultato_index;
	Energy = 0;
	CurrentPosition = 0;
	CurrentSpeed = StartSpeed;
	CurrentTime = 0;
	FtractionStrat = 0;
	Risultato_index = 0;

	float MaxTime = EndSpace / MinAvgSpeed;
	int stratindex = 0;

	while (1)
	{
		// *********************** Strategy:  *************************
		int map = 0;
		for (int p = 0; p < lap.nPulses; p++)
		{
			if ((CurrentPosition >= lap.pulses[p].startSpace) && (CurrentPosition <= (lap.pulses[p].startSpace+lap.pulses[p].pulseLenght)))
			{
				if ((lap.pulses[p].engineMap > 0) && (lap.pulses[p].engineMap <= MOTORMAPSPERSET))
				{
					if (mapset[lap.pulses[p].engineMap - 1].maxSpeed) // Constant speed
					{
						FtractionStrat = min(Fresistent(CurrentSpeed, Tslope(CurrentPosition), Tradius(CurrentPosition)), FtractionMax(CurrentSpeed));
					}
					else
					{
						FtractionStrat = min(motorForce(&mapset[lap.pulses[p].engineMap - 1], CurrentSpeed), FtractionMax(CurrentSpeed));
					}
					map = lap.pulses[p].engineMap;
				}
				else
					FtractionStrat = 0;
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
		if (acc == 0)
			dT = SPACESTEP / CurrentSpeed;
		float NewSpeed = CurrentSpeed + acc*dT;
		float EnergyStep = dT*Power(FtractionStrat, CurrentSpeed);

		//Log Runtime
		if (save) fprintf(f, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", CurrentTime, Twindings, TmotorCase, FLAT_fi, FLAT_fo, FLAT_r, FRAD_fi, FRAD_fo, FRAD_r, CurrentPosition);

		Energy += EnergyStep;
		CurrentPosition += SPACESTEP;
		CurrentTime += dT;
		CurrentSpeed = NewSpeed;

		Risultato[0][Risultato_index] = EnergyStep;
		Risultato[1][Risultato_index] = map * 10; // FtractionStrat;
		Risultato[2][Risultato_index] = FtractionStrat;
		Risultato[3][Risultato_index] = CurrentSpeed;
		Risultato[4][Risultato_index] = CurrentTime * 1000;
		Risultato_index++;
		// ********************

		if (CurrentPosition >= EndSpace) //((RaceLaps-1)*TrackLenght)+TrackFinish)
			break;
	}

	if (CurrentSpeed<EndSpeed)
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
		FILE *Resfile, *ResfileS, *Timefile, *GASfile, *Spacefile, *StratFile, *MapSetFile;
		if (print) ("Salvataggio risultato in Res.txt...\n");
		strcpy(fname, directory);
		strcat(fname, "Res.txt");
		Resfile = fopen(fname, "w");
		strcpy(fname, directory);
		strcat(fname, "Strat.dat");
		StratFile = fopen(fname, "w");
		strcpy(fname, directory);
		strcat(fname, "MapSet.dat");
		MapSetFile = fopen(fname, "w");
		fprintf(Resfile, "Time,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++)
			fprintf(Resfile, "%f,", Risultato[4][ks] / 1000);
		fprintf(Resfile, "\nEnergy,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++) fprintf(Resfile, "%f,", Risultato[0][ks]);
		fprintf(Resfile, "\nMAP,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++) fprintf(Resfile, "%f,", Risultato[1][ks]);
		fprintf(Resfile, "\nGAS,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++) fprintf(Resfile, "%f,", Risultato[2][ks]);
		fprintf(Resfile, "\nSpeed,");
		for (ks = 0; ks <= (TrackLenght / SPACESTEP); ks++) fprintf(Resfile, "%f,", Risultato[3][ks]);

		/*
		for (int i = 0; i <= (TrackLenght / SPACESTEP); i++)
		{
			fprintf(StratFile, "%f,", TractionForceProfileS[i]);
		}*/

		fwrite(&lap, sizeof(lap), 1, StratFile);
		fwrite(mapset, sizeof(MotorMap), 4, MapSetFile);

		if (print) printf("Salvato.\n");
		fclose(Resfile);
		fclose(StratFile);
		fclose(MapSetFile);
	}
	//****************************************************************************************************************

	*energy = Energy; //-KineticEnergy/0.90;
	return 0;
}


#endif