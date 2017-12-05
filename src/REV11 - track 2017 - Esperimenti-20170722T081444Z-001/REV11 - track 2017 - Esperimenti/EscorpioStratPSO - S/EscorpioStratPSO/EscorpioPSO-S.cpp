#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <cmath>
#include <time.h>

#include "Simulate-S.h"

#define NO_CAPOSTIPITI

#define MAX_PSO_NOCHANGE 20
#define MAX_GEN_NOCHANGE 40

#define ForceOffset 10
#define MaxForce   (70+ForceOffset)


//PSO
#define STRATPOINTS 1659
//#define PARTICLES   200
#define PARTICLES   1
#define CAPOSTIPITI 0
//#define c1 0.4
//#define CRND 0.02
#define c1 0.4
#define CRND 0.06

//Genetic
#define ELITISM 3            // Gli n migliori passano alla generazione successiva
#define MUTATION_RATE 0.01
#define CROSSOVER_RATIO 0.5
#define EXTINCTION_NOCHG_RATIO 100
#define EXTINCTION_RATE 0.001
#define EXTINCTION_RATE_MAX 0.01
#define RANDNEW_RATE 0.03
#define INIT_MUTATION_RATE 0.1

#define CONVERGENZA 1000

typedef struct particle_t
{
	float currentEnergy;
	float forceProfile[STRATPOINTS];
	float forceProfileSpeeds[STRATPOINTS];
	float speed[STRATPOINTS];
};

particle_t particles[PARTICLES], newParticels[PARTICLES];
float totalFitness;

void scambia(particle_t v[], long i, long j)
{
	particle_t tmp = v[i];
	v[i] = v[j];
	v[j] = tmp;
}

//Example: quicksort
void Sort(particle_t v[], long inf, long sup)
{
	particle_t pivot = v[(inf + sup) / 2];
	long i = inf, j = sup;
	while (i <= j) {
		while (v[i].currentEnergy<pivot.currentEnergy) i++;
		while (v[j].currentEnergy>pivot.currentEnergy) j--;
		if (i<j)
			scambia(v, i, j);
		if (i <= j) {
			i++;
			j--;
		}
	}
	if (inf<j) Sort(v, inf, j);
	if (i<sup) Sort(v, i, sup);
}

float rnd()
{
	return ((rand() % 1000) / 999.0);
}

particle_t NewPart()
{
	int i, ret;
	particle_t NewP;
	NewP.currentEnergy = 0;
	do
	{
		for (i = 0; i<STRATPOINTS; i++)
		{
			NewP.forceProfile[i] = rand() % MaxForce - ForceOffset;
			NewP.forceProfileSpeeds[i] = (rand() % MaxForce) / (float)MaxForce - 0.5;
		}
		// Verifica che sia valida
		ret = Simulate(NewP.forceProfile, NewP.speed, &NewP.currentEnergy, 0, 0, 1);
	} while (ret != 0);
	return NewP;
}

int PartFromFile(particle_t *dest, char *filename)
{
	int i, ret;

	/*
	particle_t NewP;
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
	*/
	return ret;
}


particle_t RouletteWheelSelection()
{
	double fSlice = rnd() * totalFitness;
	double cfTotal = 0.0;
	int	selectedParticle = 0;

	for (int i = 0; i<PARTICLES; ++i)
	{
		cfTotal += 1 / particles[i].currentEnergy;
		if (cfTotal > fSlice)
		{
			selectedParticle = i;
			break;
		}
	}
	return particles[selectedParticle];
}

particle_t RankSelection()
{
	// each part fitness is now 1/rank
	float totFitness = 0;
	for (int i = 0; i < PARTICLES; i++)
		totFitness += 1 / (i + 1);
	double fSlice = rnd() * totFitness;
	double cfTotal = 0.0;
	int	selectedParticle = 0;

	for (int i = 0; i<PARTICLES; ++i)
	{
		cfTotal += 1 / (1 + i);
		if (cfTotal > fSlice)
		{
			selectedParticle = i;
			break;
		}
	}
	return particles[selectedParticle];
}

#define RANKSEL2_C 0.5
particle_t RankSelection2()
{
	// each part fitness is now 1/rank + RANKSEL2_C
	float totFitness = 0;
	for (int i = 0; i < PARTICLES; i++)
		totFitness += 1 / (i + 1) + RANKSEL2_C;
	double fSlice = rnd() * totFitness;
	double cfTotal = 0.0;
	int	selectedParticle = 0;

	for (int i = 0; i<PARTICLES; ++i)
	{
		cfTotal += 1 / (1 + i) + RANKSEL2_C;
		if (cfTotal > fSlice)
		{
			selectedParticle = i;
			break;
		}
	}
	return particles[selectedParticle];
}

particle_t RndSelection()
{
	return particles[rand() % PARTICLES];
}

void Mutate(particle_t &prt, float mut_rate)
{
	for (int i = 0; i < STRATPOINTS; i++)
	{
		//do we change this?
		if (rnd()<mut_rate)
		{
			prt.forceProfile[i] = rand() % MaxForce - ForceOffset;
		}
	}
}


void Crossover(particle_t &mum, particle_t &dad, particle_t &baby)
{
	if (rnd() >CROSSOVER_RATIO)
	{
		baby = mum;
		return;
	}

	//determine a crossover point
	int cp = rand() % STRATPOINTS;

	//swap the bits
	for (int i = 0; i <= cp; ++i)
	{
		baby.forceProfile[i] = mum.forceProfile[i];
	}

	for (int i = cp + 1; i<STRATPOINTS; ++i)
	{
		baby.forceProfile[i] = dad.forceProfile[i];
	}
}

void Mean(particle_t &mum, particle_t &dad, particle_t &baby)
{
	for (int i = 0; i<STRATPOINTS; ++i)
	{
		baby.forceProfile[i] = 0.5*(dad.forceProfile[i] + dad.forceProfile[i]);
	}
}


int main()
{
	srand(time(0));
	char h;
	int ret;
	float joule;
	int i, part;

#ifdef SIM
	printf("StartSpeed m/s?  ");
	scanf("%f", &StartSpeed);
	char ch;
	while ((ch = getchar()) != EOF && ch != '\n');

	bool usePSO = false;

	// Inizializzazione particelle
	particle_t capostipite;
	int rt;// PartFromFile(&capostipite, "Capostipiti\\Strat.txt");
#ifdef NO_CAPOSTIPITI
	rt = 1;
#endif
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
#ifndef NO_CAPOSTIPITI
		printf("Not valid. Try to make it valid?(y/n)\n");
		char c = getchar();
#else
		char c = 'n';
#endif
		if (c == 'y')
		{
			/*
			for (part = 0; part < CAPOSTIPITI; part++)
			{
				particle_t newPart = capostipite;
				int simret;
				do
				{
					newPart = capostipite;
					Mutate(newPart, 0.01);
					simret =  Simulate((float*)newPart.forceProfile, (float*)&newPart.currentEnergy, 0, 0, 1);
				} while (simret != 0);
				particles[part] = newPart;
			}*/
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

	if (PartFromFile(&particles[part], "Capostipiti\\Strat.txt"))
	{
	particles[part] = NewPart();
	}
	}
	*/
	for (; part<PARTICLES; part++)
	{
		particles[part] = NewPart();
	}
	Sort(particles, 0, PARTICLES - 1);

#endif

	int it = 0;

	int PSOnoChange = 0;
	int GENnoChange = 0;
	int noChange = 0;

	particle_t BestParticle;
	BestParticle.currentEnergy = 100000000;

	printf("inizio\n");
	double mean = 0;
	int k;


	double m1 = 0, m2 = 0, e1 = 0, e2 = 0, diff = 0;
	srand(time(NULL));   

	for(k = 0; k < TrackLenght; k++)
	{
		int res;
		clock_t t_start, t_end;
		double t_passed;

		float v = (double)rand() / RAND_MAX * 12;
		/*
		for (int i = 0; i < PARTICLES; i++) {
			particles[i].speed[0] = 0;
			res = Simulate(particles[i].forceProfile, particles[i].speed, &particles[i].currentEnergy, 0, 0, 0);


		}
		*/

		t_start = clock();
		float f1 = ModelloTriciclo(12, Tslope(k));
		t_end = clock();
		t_passed = ((double)(t_end - t_start)) / CLOCKS_PER_SEC;

		m1 += t_passed;
		e1 += abs(f1);

		t_start = clock();
		float f2 = ModelloTriciclo2(12, Tslope(k));
		t_end = clock();
		t_passed = ((double)(t_end - t_start)) / CLOCKS_PER_SEC;

		m2 += t_passed;
		e2 += abs(f2);

		diff += abs(f1-f2);

		//printf("\n\n");


		//printf("%f   %f\n", f1, f2);


		//printf("Elapsed time: %f seconds.\n", t_passed);


		/*
		particle_t ptemp;
		for (int i = 0; i < STRATPOINTS; i++) {
			ptemp.forceProfile[i] = particles[0].forceProfile[i];
		}
		ptemp.speed[0] = 0;

		t_start = clock();
		for (int i = 0; i < STRATPOINTS; i++) {
			res = Simulate2(ptemp.speed[i], &ptemp.speed[i+1], ptemp.forceProfile[i], &ptemp.currentEnergy, 0, Tslope(i), Tradius(i));
		}
		t_end = clock();
		t_passed = ((double)(t_end - t_start)) / CLOCKS_PER_SEC;

		printf("Elapsed time: %f seconds.\n", t_passed);

		for (int i = 0; i < STRATPOINTS; i++) {
			printf("%f %f\n", particles[0].speed[i], ptemp.speed[i]);
		}

		system("pause");
		*/
		mean += t_passed;
	}

	printf("tot time1 %f   avg time1: %f     avg eq1: %f\n", m1, m1 / TrackLenght, e1 / TrackLenght);
	printf("tot time2 %f   avg time2: %f     avg eq2: %f\n", m2, m2 / TrackLenght, e2 / TrackLenght);
	printf("diff %f\n", diff / TrackLenght);

	//printf("mean sim time: %f", mean/k);

//	Simulate(BestParticle.forceProfile, &joule, 1, 1, 1);

	//printf("Iterazioni: %d\n", it);

	system("PAUSE");
	return 0;
}

