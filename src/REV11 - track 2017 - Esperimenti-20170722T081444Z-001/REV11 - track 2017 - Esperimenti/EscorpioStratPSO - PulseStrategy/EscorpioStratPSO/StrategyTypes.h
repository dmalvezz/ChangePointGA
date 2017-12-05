#ifndef STRATTYPES
#define STRATTYPES

#define MAXPULSESPERLAP 6
#define MOTORMAPSPERSET 4

typedef struct MotorMap{
	char name[8];
	float a0;
	float a1;
	float a2;
	float syncTrh;
	float flatLev;
	float maxSpeed;
	bool flatOut;
	bool useSyncRect;
	bool useSyncSafe;
	bool useEnergyRecovery;
}MotorMap;

// Classe che rappresenta una singola accelerazione
class pulseClass
{
public:
	float startSpace;
	float pulseLenght;
	int engineMap;
	float pulseForce;   // <- aggiunta in un secondo momento
};

// Classe che rappresenta un singolo giro
class lapClass
{
public:
	int nPulses;
	pulseClass pulses[MAXPULSESPERLAP];
};

// Classe che rappresenta una intera strategia
class strategyClass
{
public:
	lapClass firstLap;
	lapClass lastLap;
	lapClass defaultLap;

	MotorMap mapSet[MOTORMAPSPERSET];
};







#endif