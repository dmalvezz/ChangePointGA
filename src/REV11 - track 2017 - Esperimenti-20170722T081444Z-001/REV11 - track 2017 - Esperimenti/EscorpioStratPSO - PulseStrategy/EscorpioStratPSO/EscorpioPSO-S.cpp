#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <cmath>
#include <time.h>

#include "StrategyTypes.h"
#include "Simulate-S.h"

// Impostazioni ottimizzatore
//#define PULSES 4
// Limiti spaziali(m) delle varie accelerate
//float pulseLimits[PULSES][2] = { { 300, 400 }, { 850, 250 }, { 1300, 450 }, { 2050, 150 } };

//float pulseLimits[PULSES][2] = { { 100, 500 }, { 850, 450 }, { 1300, 550 }, { 1300, 500 } };
//float pulseLimits[PULSES][2] = { { 100, 500 }, { 850, 450 }, { 1300, 550 }, { 1900, 300 } };

//float pulseLimits[PULSES][2] = { { 00, 1000 }, { 0, 1000 }, { 0, 1000 }, { 0, 1000 } };

#define PULSES 2
float pulseLimits[PULSES][2] = { { 850, 30 }, { 1280, 130 } };

//#define PARTICLES   200
#define PARTICLES   50
#define CAPOSTIPITI 0
//#define CAPOSTIPITI 5

#define MINGASSPACE 30

#define MaxForce   75

//#define STRATPOINTS 165


#define c1 0.5
#define CRND 0.02


strategyClass myStrategy;

typedef struct particle_t
{
	float currentEnergy;
	float mapSpeed[MOTORMAPSPERSET];
	lapClass lap, speeds;
};

particle_t particles[PARTICLES];

float rnd()
{
	return ((rand()%1000)/999.0);
}

particle_t NewPart ()
{
	int i, ret;
	particle_t NewP;
	NewP.currentEnergy = 0;
	NewP.lap.nPulses = PULSES;
	do
	{
		for (i = 0; i<PULSES; i++)
		{
			NewP.lap.pulses[i].startSpace = pulseLimits[i][0] + rnd()*(pulseLimits[i][1] );
			NewP.speeds.pulses[i].startSpace = rnd();
			NewP.lap.pulses[i].pulseLenght = rnd()*(pulseLimits[i][1]);
			NewP.speeds.pulses[i].pulseLenght = rnd();
			NewP.lap.pulses[i].engineMap = round(rnd()*(MOTORMAPSPERSET+0.4));
			NewP.lap.pulses[i].pulseForce = rnd()*MaxForce;
			NewP.mapSpeed[i] = rnd();
		}
		float GasSpace = 0;
		for (i = 0; i<PULSES; i++)
			GasSpace += NewP.lap.pulses[i].pulseLenght;
		ret = -1;
		if (GasSpace < MINGASSPACE) continue;
		// Verifica che sia valida
		ret = Simulate(NewP.lap, myStrategy.mapSet, &NewP.currentEnergy, 0, 0, 1);
		//ret = Simulate(NewP.lap, myStrategy.mapSet, &NewP.currentEnergy, 1, 1, 1);
	} while (ret != 0);
	return NewP;

}


int PartFromFile(particle_t *dest, char *filename)
{
	int i, ret;
	particle_t NewP;

	for (i = 0; i<PULSES; i++)
	{
		NewP.speeds.pulses[i].startSpace = rnd();
		NewP.speeds.pulses[i].pulseLenght = rnd();
		NewP.mapSpeed[i] = rnd();
	}
	FILE *StratFile;
	if ((StratFile = fopen(filename, "r")) == NULL) return -1;
	fread(&NewP.lap, sizeof(lapClass), 1, StratFile);
	fclose(StratFile);

	ret = Simulate(NewP.lap, myStrategy.mapSet, &NewP.currentEnergy, 0, 0, 1);

	*dest = NewP;
	return ret;

	/*
	for (i = 0; i<STRATPOINTS; i++)
	{
		NewP.forceProfile[i] = 0;
		NewP.forceProfileSpeeds[i] = (rand() % MaxForce) / (float)MaxForce - 0.5;
	}
	NewP.currentEnergy = 0;

	//Carico da un file Strat con esattamente lo stesso SPACESTEP
	FILE *StratFile;
	float temp;
	if ((StratFile = fopen(filename, "r")) == NULL) return -1;

	i = 0;
	while (fscanf(StratFile, "%f,", &temp) == 1 && i < STRATPOINTS)
	{
		NewP.forceProfile[i++] = temp;
	}
	fclose(StratFile);
	for (; i < STRATPOINTS; i++) NewP.forceProfile[i] = 0;

	ret = Simulate((float*)NewP.forceProfile, (float*)&NewP.currentEnergy, 0, 0, 1);
	//if (ret == 0)
		*dest = NewP;

		
	return ret;
	*/
}



void Mutate(particle_t &prt, float mut_rate)
{
	for (int i = 0; i<PULSES; i++)
	{
		//do we change this?
		if (rnd()<mut_rate)
			prt.lap.pulses[i].startSpace = pulseLimits[i][0] + rnd()*(pulseLimits[i][1]);
		//do we change this?
		if (rnd()<mut_rate)
			prt.lap.pulses[i].pulseLenght = rnd()*(pulseLimits[i][1]);
		//do we change this?
		if (rnd()<mut_rate)
			prt.lap.pulses[i].engineMap = round(rnd()*MOTORMAPSPERSET);
	}
	
	/*
	for (int i = 0; i < STRATPOINTS; i++)
	{
		//do we change this?
		if (rnd()<mut_rate)
		{
			prt.forceProfile[i] = rand() % MaxForce;
		}
	}
	*/
}


particle_t BestParticle;
particle_t NewPartFromBest()
{
	int i, ret;
	particle_t NewP = BestParticle;
	NewP.currentEnergy = 0;
	NewP.lap.nPulses = PULSES;
	for (i = 0; i<PULSES; i++)
	{
		NewP.speeds.pulses[i].startSpace = rnd();
		NewP.speeds.pulses[i].pulseLenght = rnd();
		NewP.mapSpeed[i] = rnd();
	}
	do
	{
			Mutate(NewP, 0.1);
		float GasSpace = 0;
		for (i = 0; i<PULSES; i++)
			GasSpace += NewP.lap.pulses[i].pulseLenght;
		ret = -1;
		if (GasSpace < MINGASSPACE) continue;
		// Verifica che sia valida
		ret = Simulate(NewP.lap, myStrategy.mapSet, &NewP.currentEnergy, 0, 0, 1);
		//ret = Simulate(NewP.lap, myStrategy.mapSet, &NewP.currentEnergy, 1, 1, 1);
	} while (ret != 0);
	return NewP;
}


int main()
{
	/*
	// Map1
	myStrategy.mapSet[0].a0 = (-3000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (30.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2
	myStrategy.mapSet[1].a0 = (-3000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (30.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	myStrategy.mapSet[2].a0 = (-3000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (21.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (-3000.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (25.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	*/

	/*
	// Map1
	myStrategy.mapSet[0].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[0].a1 = (40.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map2
	myStrategy.mapSet[1].a0 = (-4800.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[1].a1 = (40.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	// Map3
	myStrategy.mapSet[2].a0 = (-3600.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[2].a1 = (30.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
	//Map 4
	myStrategy.mapSet[3].a0 = (-1560.0 / 60 * 2 * pi);      // RAD/s
	myStrategy.mapSet[3].a1 = (23.0 / 60 * 2 * pi * 1000);  // (RAD/s)/Nm
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

	myStrategy.defaultLap.nPulses = 4;

	srand(time(0));
	char h;
	int ret;
	float joule;
	int i, part = 0;
	printf("StartSpeed m/s?  ");
	scanf("%f", &StartSpeed);
	//char ch;
	//while ((ch = getchar()) != EOF && ch != '\n');

	// Inizializzazione particelle
	particle_t capostipite;
	int rt = PartFromFile(&capostipite, "Capostipiti\\Strat.dat");
	if (rt == 0)
	{
		printf("Correctly loaded. \n");
		for (part = 0; part < CAPOSTIPITI; part++)
		{
				particles[part] = capostipite;
		}
	}
	else
	{
		printf("Not valid. Try to make it valid?(y/n)\n");
		char c = getchar();
		if (c == 'y')
		{
			for (part = 0; part < CAPOSTIPITI; part++)
			{
				particle_t newPart = capostipite;
				int simret;
				do
				{
					newPart = capostipite;
					Mutate(newPart, 0.01);
					simret = Simulate(newPart.lap, myStrategy.mapSet, &newPart.currentEnergy, 0, 0, 1);
				} while (simret != 0);
				particles[part] = newPart;
			}
		}
		else
		{
			for (part = 0; part < CAPOSTIPITI; part++)
			{
				particles[part] = NewPart();
			}
		}
	}
	
	/*
	for (part = 0; part < CAPOSTIPITI; part++)
	{		
		if (PartFromFile(&particles[part], "Capostipiti\\Strat.dat"))
		{
			particles[part] = NewPart();
		}
		else
		{
			printf("Loading ok.\n");
		}
	}
	*/

	for (; part<PARTICLES; part++)
	{
		particles[part] = NewPart();
		printf("Generated n %d\n", part);
	}
	int it = 0;

	
	BestParticle.currentEnergy = 100000000;
	
	while (1)
	{
		// Cerco la strategia migliore
		float minEnergy;
		int minEnergyPos;
		for (part = 0; part<PARTICLES; part++)
		{
			if (part ==0 || particles[part].currentEnergy<minEnergy)
			{
				minEnergy = particles[part].currentEnergy;
				minEnergyPos = part;
			}
		}
		if (minEnergy<=BestParticle.currentEnergy)
			BestParticle = particles[minEnergyPos];

		//if (it>=300) break;
		int save = 0;
		if(kbhit())
		{
			if (getch() == 's')
			{
				printf("Simulate!\n");
				Simulate(BestParticle.lap, myStrategy.mapSet, &BestParticle.currentEnergy, 1, 1, 1);
			}
		}
		int news = 0, filt = 0, ant = 0, cln = 0;
		//Faccio muovere le particelle nella direzione della migliore
		for (part = 0; part<PARTICLES; part++)
		{
			particle_t lastP = particles[part];
			int cnt = 0;
			do
			{
				if (cnt >= 10)
				{
					//lastP = NewPart();
					lastP = NewPartFromBest();
					particles[part] = lastP;
					news++;
					break;
				}
				particles[part] = lastP;
				// V(new) = w*V(old) + c1*RND*(Xbest-Xi)
				// w = (1+RND)/2
				// c1 = 1.5
				for (i = 0; i<PULSES; i++)
				{
					float w = (1 + 2 * rnd()) / 2;
					particles[part].speeds.pulses[i].startSpace = w*particles[part].speeds.pulses[i].startSpace + c1*rnd()*(BestParticle.lap.pulses[i].startSpace - particles[part].lap.pulses[i].startSpace) + CRND*(2 * rnd() - 1);
					particles[part].lap.pulses[i].startSpace += particles[part].speeds.pulses[i].startSpace;
					
					particles[part].speeds.pulses[i].pulseLenght = w*particles[part].speeds.pulses[i].pulseLenght + c1*rnd()*(BestParticle.lap.pulses[i].pulseLenght - particles[part].lap.pulses[i].pulseLenght) + CRND*(2 * rnd() - 1);
					particles[part].lap.pulses[i].pulseLenght += particles[part].speeds.pulses[i].pulseLenght;

					particles[part].mapSpeed[i] = w*particles[part].mapSpeed[i] + c1*rnd()*(BestParticle.lap.pulses[i].engineMap - particles[part].lap.pulses[i].engineMap) + CRND*(2 * rnd() - 1);
					particles[part].lap.pulses[i].engineMap += particles[part].mapSpeed[i];
				}


				/*
				for (i = 0; i < STRATPOINTS; i++)
				{
					//float w = (1+rnd())/2;
					float w = (1 + 2 * rnd()) / 2;
					particles[part].forceProfileSpeeds[i] = w*particles[part].forceProfileSpeeds[i] + c1*rnd()*(BestParticle.forceProfile[i] - particles[part].forceProfile[i]) +CRND*(2 * rnd() - 1);
					particles[part].forceProfile[i] += particles[part].forceProfileSpeeds[i];
					if (particles[part].forceProfile[i]<0) particles[part].forceProfile[i] = 0;
					if (particles[part].forceProfile[i]>MaxForce) particles[part].forceProfile[i] = MaxForce;
				}*/

				if (particles[part].currentEnergy>BestParticle.currentEnergy) //Inserzione casuale solo se non è la migliore!
				{
					
					//Inserzione casuale nuovo elemento
					if (rnd() < 0.002)
					{
						particles[part] = NewPart();
						news++;
						break;
					}
					
					/*
					//Inserzione casuale elemento derivato dal migliore
					if (rnd() < 0.03)
					{
						particles[part] = BestParticle;

						//Filtraggio
						float c = rnd()*0.5;
						for (int j = 1; j < STRATPOINTS; j++)
						{
							particles[part].forceProfile[j] = c*particles[part].forceProfile[j - 1] + (1 - c)*particles[part].forceProfile[j];
						}
						ret = Simulate(particles[part].forceProfile, &particles[part].currentEnergy, 0, 0, 1);
						if (ret == 0)
						{
							filt++;
							break;
						}
					}
					*/

					//Inserzione casuale elemento derivato dal migliore
					if (rnd() < 0.02)
					{
						particles[part] = BestParticle;

						//Anticipo
						int tau = rnd()*50;
						for (int j = 0; j < PULSES; j++)
						{
							if (rnd()<0.2)
							particles[part].lap.pulses[i].startSpace -= tau;
							//particles[part].lap.pulses[i].stopSpace -= tau;
						}
						ret = Simulate(particles[part].lap, myStrategy.mapSet, &particles[part].currentEnergy, 0, 0, 1);
						if (ret == 0)
						{
							ant++;
							break;
						}
					}
					

					
					//Inserzione casuale cloni migliore
					if (rnd() < 0.002)
					{
						particles[part] = BestParticle;
						//Randomize vel
						for (i = 0; i<PULSES; i++)
						{
							particles[part].speeds.pulses[i].startSpace = rnd();
							particles[part].speeds.pulses[i].pulseLenght = rnd();
							particles[part].mapSpeed[i] = rnd();
						}
						cln++;
						break;
					}
						
				}
				
				//la strategia è valida? la provo!
				ret = Simulate(particles[part].lap, myStrategy.mapSet, &particles[part].currentEnergy, 0, 0, 1);
				cnt++;
			}while (ret != 0);
		}
		it++;
		printf ("Iterazioni: %d news:%d cln:%d filt:%d ant:%d StartSpd: %.2f Consumo: %.2f\n", it, news, cln, filt, ant, StartSpeed, BestParticle.currentEnergy);
	}
	
	printf("Simulate!\n");
	Simulate(BestParticle.lap, myStrategy.mapSet, &BestParticle.currentEnergy, 1, 1, 1);

	printf ("Iterazioni: %d\n", it);
	system("PAUSE");
	return 0;
}


