#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <cmath>
#include <time.h>

#include "StrategyTypes.h"
#include "Simulate-S.h"

// Impostazioni ottimizzatore
#define PULSES 4
#define OPTISPACESTEP 10

#define USE_L17_0
//#define USE_HP1R24V
//#define USE_HP2R24V

#define OPTI_FORWARD
//#define OPTI_REVERSE

// Limiti spaziali(m) delle varie accelerate
// startspace, stop
typedef struct pulselimit_t
{
	float startSpace, stopSpace, minLen, maxLen;
	int minMap, maxMap;
};

//pulselimit_t pulseLimits[PULSES] = { { 300, 600, 0, 100, 1, 3 }, { 850, 1000, 50, 250, 1, 4 }, { 1300, 1600, 200, 450, 2, 4}, { 2050, 2200, 0, 100, 1, 3} };
//pulselimit_t pulseLimits[PULSES] = { { 510, 540, 60, 100, 1, 2 }, { 880, 910, 50, 250, 1, 3 }, { 1300, 1600, 170, 450, 2, 4}, { 2050, 2200, 0, 100, 1, 3} };
//pulselimit_t pulseLimits[PULSES] = { { 410, 410, 60, 60, 1, 1 }, { 880, 880, 110, 110, 2, 2 }, { 1410, 1600, 170, 350, 4, 4 }, { 2080, 2200, 0, 100, 1, 3 } };
//pulselimit_t pulseLimits[PULSES] = { { 0, 0, 60, 150, 1, 4 }, { 880, 880, 110, 110, 2, 2 }, { 1410, 1600, 170, 350, 4, 4 }, { 2080, 2200, 0, 100, 1, 3 } };

/*
// Le più generali
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 240, 1, 4 }, { 880, 880, 110, 150, 2, 3 }, { 1410, 1600, 170, 350, 3, 4 }, { 2080, 2200, 0, 100, 1, 3 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 400, 430, 40, 100, 1, 3 }, { 870, 890, 110, 150, 2, 3 }, { 1350, 1600, 170, 350, 2, 4 }, { 2080, 2200, 0, 100, 1, 3 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 410, 410, 60, 60, 1, 3 }, { 880, 880, 110, 150, 2, 3 }, { 1410, 1600, 170, 350, 2, 3 }, { 2080, 2200, 0, 100, 1, 3 } };
*/

/*
// Un po' più veloci, migliorate
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 100, 1, 4 }, { 260, 430, 0, 130, 1, 3 }, { 880, 880, 110, 150, 2, 3 }, { 1440, 1550, 170, 350, 3, 3 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 400, 430, 40, 100, 1, 3 }, { 870, 890, 110, 150, 2, 3 }, { 1440, 1550, 170, 350, 3, 3 }, { 2080, 2200, 0, 100, 1, 3 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 410, 410, 60, 60, 1, 3 }, { 880, 880, 110, 150, 2, 3 }, { 1440, 1550, 170, 350, 3, 3 }, { 2080, 2200, 0, 100, 1, 3 } };
*/

// Un po' più veloci, migliorate, no last
//pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 100, 3, 4 }, { 880, 880, 110, 180, 2, 3 }, { 1440, 1550, 170, 350, 3, 3 } };
//pulselimit_t pulseLimitsLap[PULSES] = { { 400, 430, 40, 100, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1440, 1550, 170, 350, 3, 3 }, { 2080, 2200, 0, 0, 1, 3 } };
//pulselimit_t pulseLimitsLastLap[PULSES] = { { 410, 410, 60, 60, 1, 3 }, { 880, 880, 110, 180, 2, 3 }, { 1440, 1550, 170, 350, 3, 3 }, { 2080, 2200, 0, 0, 1, 3 } };

//LONDRA
//pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 100, 4, 4 }, { 880, 880, 110, 280, 2, 4 }, { 1350, 1580, 170, 390, 3, 4 } };
//pulselimit_t pulseLimitsLap[PULSES] = { { 400, 430, 40, 100, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1440, 1550, 170, 350, 3, 4 }, { 2080, 2200, 0, 0, 1, 3 } };
//pulselimit_t pulseLimitsLastLap[PULSES] = { { 410, 410, 60, 60, 1, 3 }, { 880, 880, 110, 180, 2, 3 }, { 1440, 1550, 170, 350, 3, 4 }, { 2080, 2200, 0, 0, 1, 3 } };

/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 110, 1, 2 }, { 430, 430, 60, 90, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1410, 1600, 170, 350, 3, 4 }};
pulselimit_t pulseLimitsLap[PULSES] = { { 430, 430, 60, 90, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1410, 1600, 170, 350, 3, 4 }, { 2080, 2200, 0, 100, 1, 2 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 430, 430, 60, 90, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1410, 1600, 170, 350, 3, 4 }, { 2080, 2200, 0, 100, 1, 2 } };
*/

//pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 90, 1, 2 }, { 575, 605, 0, 60, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1310, 1600, 200, 490, 3, 4 } };
//pulselimit_t pulseLimitsLap[PULSES] = { { 575, 605, 0, 60, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1310, 1600, 200, 490, 3, 4 }, { 2080, 2200, 0, 0, 1, 2 } };
//pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 605, 0, 60, 1, 2 }, { 870, 890, 110, 180, 2, 3 }, { 1310, 1600, 200, 490, 3, 4 }, { 2080, 2200, 0, 0, 1, 2 } };

// Z17
/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 90, 1, 2 }, { 575, 605, 0, 60, 1, 1 }, { 870, 890, 110, 180, 2, 2 }, { 1310, 1600, 200, 490, 3, 4 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 575, 605, 60, 60, 1, 1 }, { 870, 890, 110, 180, 2, 2 }, { 1310, 1600, 200, 490, 3, 4 }, { 2080, 2200, 0, 0, 1, 2 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 605, 0, 60, 1, 1 }, { 870, 890, 110, 180, 2, 2 }, { 1310, 1600, 200, 490, 3, 4 }, { 2080, 2200, 0, 0, 1, 2 } };
*/

// Z14
//pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 110, 1, 2 }, { 880, 880, 110, 180, 2, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1820, 2150, 0, 100, 2, 3 }};
//pulselimit_t pulseLimitsLap[PULSES] = { { 575, 605, 0, 0, 1, 1 }, { 880, 880, 110, 180, 2, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1820, 2150, 0, 100, 2, 3 } };
//pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 605, 0, 0, 1, 1 }, { 880, 880, 110, 180, 2, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1820, 2150, 0, 100, 2, 3 } };

//pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 110, 1, 2 }, { 880, 880, 110, 180, 2, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1820, 2150, 0, 0, 2, 3 } };
//pulselimit_t pulseLimitsLap[PULSES] = { { 575, 605, 0, 0, 1, 1 }, { 880, 880, 110, 180, 2, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1820, 2150, 0, 0, 2, 3 } };
//pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 605, 0, 0, 1, 1 }, { 880, 880, 110, 180, 2, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1820, 2150, 0, 0, 2, 3 } };

/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 110, 1, 2 }, { 880, 880, 110, 180, 1, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1700, 1850, 0, 90, 1, 3 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 575, 605, 0, 0, 1, 1 }, { 880, 880, 110, 180, 1, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1700, 1850, 0, 90, 1, 3 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 605, 0, 0, 1, 1 }, { 880, 880, 110, 180, 1, 2 }, { 1490, 1600, 200, 490, 3, 4 }, { 1700, 1850, 0, 90, 1, 3 } };
*/

/*
// FAST:
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 110, 2, 2 }, { 880, 880, 20, 150, 2, 2 }, { 1490, 1600, 120, 350, 3, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 575, 575, 0, 0, 1, 1 }, { 880, 880, 20, 150, 2, 2 }, { 1490, 1600, 120, 350, 3, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 575, 0, 0, 1, 1 }, { 880, 880, 20, 150, 2, 2 }, { 1490, 1600, 120, 350, 3, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
*/

// Strategia 2016:
/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 110, 1, 1 }, { 880, 880, 20, 150, 1, 1 }, { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 575, 575, 0, 0, 1, 1 }, { 880, 880, 20, 150, 1, 1 }, { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 575, 0, 0, 1, 1 }, { 880, 880, 20, 150, 1, 1 }, { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
*/


// Strategia 2017:
/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 60, 110, 1, 1 }, { 880, 880, 20, 150, 1, 1 }, { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 1200, 1270, 0, 200, 1, 3 }, { 1100, 1350, 50, 200, 2, 4 }, { 1280, 1330, 12, 60, 0, 3}, { 0, 0, 0, 0, 0, 0 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 575, 575, 0, 0, 1, 1 }, { 880, 880, 20, 150, 1, 1 }, { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
*/

// Pre-londra
/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 50, 90, 1, 4 }, { 855, 890, 0, 75, 1, 3 }, { 1230, 1330, 30, 200, 2, 4 }, { 650, 720, 0, 70, 1, 3 } };// , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 855, 890, 0, 75, 1, 3 }, { 1230, 1330, 30, 200, 2, 4 }, { 650, 720, 0, 70, 1, 3 }, { 0, 0, 0, 0, 0, 0 } }; // , { 1335, 1455, 50, 120, 1, 4 }, { 0, 0, 0, 0, 0, 0 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 855, 890, 0, 75, 1, 3 }, { 1130, 1330, 30, 200, 2, 4 }, { 650, 720, 0, 70, 1, 3 }, { 0, 0, 0, 0, 0, 0 } }; // , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
*/

/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 50, 90, 1, 2 }, { 855, 890, 0, 75, 1, 3 }, { 1230, 1330, 30, 200, 2, 4 }, { 720, 770, 0, 50, 1, 3 } };// , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 855, 890, 0, 75, 1, 3 }, { 1230, 1330, 30, 200, 2, 4 }, { 720, 770, 0, 50, 1, 3 },  { 0, 0, 0, 0, 0, 0 } }; // , { 1335, 1455, 50, 120, 1, 4 }, { 0, 0, 0, 0, 0, 0 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 855, 890, 0, 75, 1, 3 }, { 1130, 1330, 30, 200, 2, 4 }, { 720, 770, 0, 50, 1, 3 }, { 0, 0, 0, 0, 0, 0 } }; // , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
*/

/*
pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 70, 100, 1, 2 }, { 650, 890, 0, 80, 1, 3 }, { 1230, 1330, 30, 200, 2, 4 }, { 0, 0, 0, 0, 0, 0 } };// , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 650, 890, 0, 80, 1, 3 }, { 1230, 1330, 30, 200, 2, 4 }, { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 } }; // , { 1335, 1455, 50, 120, 1, 4 }, { 0, 0, 0, 0, 0, 0 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 650, 890, 0, 80, 1, 3 }, { 1230, 1330, 30, 200, 2, 4 }, { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 } }; // , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
*/

pulselimit_t pulseLimitsFirstLap[PULSES] = { { 0, 0, 70, 100, 1, 2 }, { 0, 0, 0, 0, 0, 0 }, { 1170, 1330, 30, 200, 1, 4 }, { 0, 0, 0, 0, 0, 0 } };// , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };
pulselimit_t pulseLimitsLap[PULSES] = { { 0, 0, 0, 0, 0, 0 }, { 1170, 1330, 30, 200, 1, 4 }, { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 } }; // , { 1335, 1455, 50, 120, 1, 4 }, { 0, 0, 0, 0, 0, 0 } };
pulselimit_t pulseLimitsLastLap[PULSES] = { { 0, 0, 0, 0, 0, 0 }, { 1170, 1330, 30, 200, 1, 4 }, { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 } }; // , { 1490, 1600, 120, 350, 4, 4 }, { 1700, 1700, 0, 0, 2, 2 } };

//float pulseLimits[PULSES][2] = { { 300, 400 }, { 850, 250 }, { 1300, 450 }, { 2050, 150 } };

#define MINGASSPACE 30

strategyClass myStrategy;

float Optimize(char *savedir, char lapType, float exSpeed, pulselimit_t *pulseLimits);
#define FIRSTLAP 0
#define LAP 1
#define LASTLAP 2

float TempPosteriore = 25;

int main()
{
	//float exSpeed;
	//printf("Exchange speed? (m/s)\n");
	//scanf("%f", &exSpeed);
	//printf("Exchange speed set to %2.2f m/s\n", exSpeed);
	
	/*
	float pignone;
	printf("Pignone Z? \n");
	scanf("%f", &pignone);
	TransmissionRatio = (pignone / 307.0);
	printf("TransmissionRatio set to %2.2f \n", 1.0/TransmissionRatio);
	*/
	
	/*
	float energy;
	lapClass mylap;
	mylap.nPulses = 0;
	mylap.pulses[0].engineMap = 4
	int ret = Simulate(mylap, myStrategy.mapSet, &energy, 0, 0, 1, "");
	*/

	/*
	// Attrito pneumatici
	FILE *f = fopen("TyresDrag.txt", "w");
	fprintf(f, "Spd/Rad, ");
	float Spd, Rad;
	for (Rad = 5; Rad <= 100; Rad++)
		fprintf(f, "%f, ", Rad);
	fprintf(f, "\n");
	for (Spd = 10; Spd <= 40; Spd += 5)
	{
		fprintf(f, "%f, ", Spd);
		for (Rad = 8; Rad <= 100; Rad++)
			fprintf(f, "%f, ", ModelloTriciclo(Spd / 3.6, 1 / Rad));
		fprintf(f, "\n");
	}
	fclose(f);
	*/
	
	char savedirLapSTD[100] = "/RisultatiLap/";
	char savedirLastLapSTD[100] = "/RisultatiLastLap/";
	char savedirFirstLapSTD[100] = "/RisultatiFirstLap/";
	/*
	// Swipe di un parametro
	char ParamName[40] = "Tratio Znto290 (RE5036V) 56V";
	char FileName[50] = "";
	strcat(FileName, ParamName); strcat(FileName, ".txt");
	printf("Swipe...\n");
	FILE *fp = fopen(FileName, "w");
	float Par;
	for (int giro = 0; giro <= 1; giro++)
	{
		if (giro == 0) fprintf(fp, "%s, ", ParamName);
		else          fprintf(fp, "CorrEnergy, ");
		for (Par = 10; Par <= 18; Par += 0.25)
		{
			if (giro == 0)
				fprintf(fp, "%f,", Par);
			else
			{
				TransmissionRatio = Par / 290.0;

				float energy = 0;
				float exSpeed = (float)41 / 10.0;
				{
					char dir[20];
					sprintf(dir, "./%1.1f", exSpeed);

					char savedirLap[100];
					strcpy(savedirLap, dir);
					strcat(savedirLap, savedirLapSTD);
					char savedirLastLap[100];
					strcpy(savedirLastLap, dir);
					strcat(savedirLastLap, savedirLastLapSTD);
					char savedirFirstLap[100];
					strcpy(savedirFirstLap, dir);
					strcat(savedirFirstLap, savedirFirstLapSTD);

					printf("Opti Started - exSpeed %2.1f \n", exSpeed);

					energy += (RaceLaps - 2)*Optimize(savedirLap, LAP, exSpeed, pulseLimitsLap);
					energy += Optimize(savedirFirstLap, FIRSTLAP, exSpeed, pulseLimitsFirstLap);
					energy += Optimize(savedirLastLap, LASTLAP, exSpeed, pulseLimitsLastLap);
				}
				printf("Par: %f   -- Energy: %.1f \n", Par, energy);
				fprintf(fp, "%f,", energy);
			}
		}
		fprintf(fp, "\n");
	}
	printf("Swipe Done.\n");
	fclose(fp);
	*/
	

#ifdef OPTI_FORWARD
	for (int i = 65; i <= 71; i+=1)
#endif
#ifdef OPTI_REVERSE
	for (int i = 64; i >= 58; i-=1)
#endif
	{
		float energy = 0;
		float exSpeed = (float)i / 10.0;

		char dir[20];
		sprintf(dir, "./%1.1f", exSpeed);

		char savedirLap[100];
		strcpy(savedirLap, dir);
		strcat(savedirLap, savedirLapSTD);
		char savedirLastLap[100];
		strcpy(savedirLastLap, dir);
		strcat(savedirLastLap, savedirLastLapSTD);
		char savedirFirstLap[100];
		strcpy(savedirFirstLap, dir);
		strcat(savedirFirstLap, savedirFirstLapSTD);

		printf("Opti Started - exSpeed %2.1f \n", exSpeed);

		//energy += 10*Optimize(savedirLap, LAP, exSpeed, pulseLimitsLap);
		energy += (RaceLaps - 2)*Optimize(savedirLap, LAP, exSpeed, pulseLimitsLap);
		energy += Optimize(savedirFirstLap, FIRSTLAP, exSpeed, pulseLimitsFirstLap);
		energy += Optimize(savedirLastLap, LASTLAP, exSpeed, pulseLimitsLastLap);
		printf("TotalEnergy: %8.1f J - %f m/s\n", energy, exSpeed);
		FILE *reportFile = fopen("Report.txt", "a");
		if (reportFile)
		{
			fprintf(reportFile, "TotalEnergy: %8.1f J - %f m/s\n", energy, exSpeed);
			fclose(reportFile);
		}
	}
	
	
	
	system("PAUSE");
	return 0;
}

float Optimize(char *savedir, char lapType, float exSpeed, pulselimit_t *pulseLimits)
{
	if (lapType == LASTLAP)
	{
		StartSpeed = exSpeed;
		EndSpeed = 0;
		MinAvgSpeed = LASTLAP_AVG;
		EndSpace = TrackFinish;
	}
	else if (lapType == FIRSTLAP)
	{
		StartSpeed = 0;
		EndSpeed = exSpeed;
		MinAvgSpeed = FIRSTLAP_AVG;
		EndSpace = TrackLenght;
	}
	else
	{
		StartSpeed = exSpeed;
		EndSpeed = exSpeed;
		MinAvgSpeed = LAP_AVG;
		EndSpace = TrackLenght;
	}
	
	/*
	// London 1
	// Map1
	myStrategy.mapSet[0].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (45.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2
	myStrategy.mapSet[1].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (40.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	myStrategy.mapSet[2].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (25.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (-67500.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (200.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	*/

	/*
	// London 2
	// Map1
	myStrategy.mapSet[0].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (40.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2
	myStrategy.mapSet[1].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (25.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	myStrategy.mapSet[2].a0 = (85000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (-220.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (34000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (-49.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	*/

	/*
	// London 3
	// Map1
	myStrategy.mapSet[0].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (40.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2
	myStrategy.mapSet[1].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (25.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	myStrategy.mapSet[2].a0 = (65000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (-125.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (34000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (-49.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	*/

	// London 4 (RE5036V)
	/*
	// Map1
	myStrategy.mapSet[0].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (30.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2
	myStrategy.mapSet[1].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (25.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	//myStrategy.mapSet[2].a0 = (-95000.0 / 60 * 2 * pi);      // RAD/s
	//myStrategy.mapSet[2].a1 = (200.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	myStrategy.mapSet[2].a0 = (80000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (-150.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (34000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (-49.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	*/
	

#ifdef USE_HP1R24V
	// HP1 RE5024V
	// Map1
	myStrategy.mapSet[0].a0 = (-4500.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (45.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	myStrategy.mapSet[0].maxSpeed = 0;
	// Map2
	myStrategy.mapSet[1].a0 = (-15000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (60.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	myStrategy.mapSet[1].maxSpeed = 0;
	// Map3
	myStrategy.mapSet[2].a0 = (-95000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (200.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//myStrategy.mapSet[2].a0 = (70000.0 / 60 * 2 * pi);      // RAD/s
	//myStrategy.mapSet[2].a1 = (-150.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	myStrategy.mapSet[2].maxSpeed = 0;
	//Map 4
	myStrategy.mapSet[3].a0 = (80000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (-150.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	myStrategy.mapSet[3].maxSpeed = 1; //Constant speed
#endif

#ifdef USE_HP2R24V
	// HP2 RE5024V
	// Map1
	myStrategy.mapSet[0].a0 = (-4500.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (45.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2
	myStrategy.mapSet[1].a0 = (70000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (-150.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	//myStrategy.mapSet[2].a0 = (-95000.0 / 60 * 2 * pi);      // RAD/s
	//myStrategy.mapSet[2].a1 = (200.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	myStrategy.mapSet[2].a0 = (80000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (-150.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (15000.0 / 60 * 2 * pi);      // RAD/s       //Assurda, sempre 400W
	myStrategy.mapSet[3].a1 = (-5.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
#endif

#ifdef USE_L17_0
	// L17.0
	// Map1
	myStrategy.mapSet[0].a0 = (-9500.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (60.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2 (ideal)
	myStrategy.mapSet[1].a0 = (-6100.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (45.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	myStrategy.mapSet[2].a0 = (-16000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (65.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (-45000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (130.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
#endif

	myStrategy.defaultLap.nPulses = 4;
	


	int i, pulse = 0;
	float bestEnergy = 100000000000;
	long long cycles = 0, sims = 0, validsims = 0;

	// First pulse
	for (int P1start = pulseLimits[0].startSpace; P1start <= pulseLimits[0].stopSpace; P1start += OPTISPACESTEP)
	{
		for (int P1dur = pulseLimits[0].minLen; P1dur <= pulseLimits[0].maxLen; P1dur += OPTISPACESTEP)
		{
			for (int P1map = pulseLimits[0].minMap; P1map <= (P1dur ? pulseLimits[0].maxMap : pulseLimits[0].minMap); P1map++)
			{
				lapClass mylap;
				mylap.nPulses = 4;
				mylap.pulses[0].startSpace = P1start;
				mylap.pulses[0].pulseLenght = P1dur;
				mylap.pulses[0].engineMap = P1map;

				for (int P2start = pulseLimits[1].startSpace; P2start <= pulseLimits[1].stopSpace; P2start += OPTISPACESTEP)
				{
					for (int P2dur = pulseLimits[1].minLen; P2dur <= pulseLimits[1].maxLen; P2dur += OPTISPACESTEP)
					{
						for (int P2map = pulseLimits[1].minMap; P2map <= (P2dur ? pulseLimits[1].maxMap : pulseLimits[1].minMap); P2map++)
						{
							mylap.pulses[1].startSpace = P2start;
							mylap.pulses[1].pulseLenght = P2dur;
							mylap.pulses[1].engineMap = P2map;

							

							
							for (int P3start = pulseLimits[2].startSpace; P3start <= pulseLimits[2].stopSpace; P3start += OPTISPACESTEP)
							{
								for (int P3dur = pulseLimits[2].minLen; P3dur <= pulseLimits[2].maxLen; P3dur += OPTISPACESTEP)
								{
									for (int P3map = pulseLimits[2].minMap; P3map <= (P3dur ? pulseLimits[2].maxMap : pulseLimits[2].minMap); P3map++)
									{
										mylap.pulses[2].startSpace = P3start;
										mylap.pulses[2].pulseLenght = P3dur;
										mylap.pulses[2].engineMap = P3map;

										/*
										float GasSpace = 0;
										for (i = 0; i < PULSES; i++)
											GasSpace += mylap.pulses[i].pulseLenght;
										if (GasSpace < MINGASSPACE) continue;

										float energy;
										int ret = Simulate(mylap, myStrategy.mapSet, &energy, TempPosteriore, 0, 0, 1, savedir);
										sims++;
										if (ret == 0)
										{
											validsims++;
											if (energy < bestEnergy)
											{
												bestEnergy = energy;
												myStrategy.defaultLap = mylap;
												printf("Current energy: %f\n", bestEnergy);
											}
										}


										if (kbhit())
										{
											if (getch() == 's')
											{
												printf("Simulate!\n");
												float en;
												Simulate(myStrategy.defaultLap, myStrategy.mapSet, &en, TempPosteriore, 1, 1, 1, savedir);
											}
										}
										*/
										
										//Testa la salita
										mylap.pulses[3].startSpace = pulseLimits[3].startSpace;
										mylap.pulses[3].pulseLenght = pulseLimits[3].maxLen;
										mylap.pulses[3].engineMap = pulseLimits[3].maxMap;
										float energy;
										int ret = Simulate(mylap, myStrategy.mapSet, &energy, TempPosteriore, 0, 0, 1, savedir);
										if (ret != 0) continue;
										else
										{
											int c;
											c = 3;
										}

										for (int P4start = pulseLimits[3].startSpace; P4start <= pulseLimits[3].stopSpace; P4start += OPTISPACESTEP)
										{
											for (int P4dur = pulseLimits[3].minLen; P4dur <= pulseLimits[3].maxLen; P4dur += OPTISPACESTEP)
											{
												for (int P4map = pulseLimits[3].minMap; P4map <= (P4dur ? pulseLimits[3].maxMap : pulseLimits[3].minMap); P4map++)
												{
													mylap.pulses[3].startSpace = P4start;
													mylap.pulses[3].pulseLenght = P4dur;
													mylap.pulses[3].engineMap = P4map;
													cycles++;

													float GasSpace = 0;
													for (i = 0; i < PULSES; i++)
														GasSpace += mylap.pulses[i].pulseLenght;
													if (GasSpace < MINGASSPACE) continue;

													float energy;
													int ret = Simulate(mylap, myStrategy.mapSet, &energy, TempPosteriore, 0, 0, 1, savedir);
													sims++;
													if (ret == 0)
													{
														validsims++;
														if (energy < bestEnergy)
														{
															bestEnergy = energy;
															myStrategy.defaultLap = mylap;
															printf("Current energy: %f\n", bestEnergy);
														}
													}


													if (kbhit())
													{
														if (getch() == 's')
														{
															printf("Simulate!\n");
															float en;
															Simulate(myStrategy.defaultLap, myStrategy.mapSet, &en, TempPosteriore, 1, 1, 1, savedir);
														}
													}

												}
											}
										}

										

									}
								}
							}
							//printf("P3 finished cycles: %d sims:", cycles);
							//printf("%d validsims : ", sims);
							//printf("%d \n", validsims);
							
						}
					}
				}
				

			}
		}
	}
	float en;
	Simulate(myStrategy.defaultLap, myStrategy.mapSet, &en, TempPosteriore, 0, 1, 1, savedir);
	printf("Final Sim -- %8.1f J\n", en);
	return en;
}


