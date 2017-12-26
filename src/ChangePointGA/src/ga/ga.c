/*
 * ga.c
 *
 *  Created on: Dec 25, 2017
 *      Author: davide
 */

#include "ga.h"

void initGA(GA* ga){
	//Init random
	randInit();

	//Init mapset
	initMapset();

	//Init track samples
	generateTrackData(SPACE_STEP);

	//Init random first generation
	ga->currentGeneration = initRandomGeneration(POPULATION_SIZE);
	//Init empty next generation
	ga->nextGeneration = initEmptyGeneration(POPULATION_SIZE);

	//Counter
	ga->generationCount = 0;


}

void genetic(GA* ga){
	int childCount = 0, mutationCount = 0;

	//Timers
	unsigned long long generationTimer, timer;
	double generationTime, fitnessTime, sortTime, statTime, elitismTime, crossoverTime, mutationTime;

	//Start timer
	generationTimer = getTime();

	//Eval fitness
	timer = getTime();
	evalGenerationFitness(ga->currentGeneration, START_VELOCITY, START_MAP, energyFitness);
	fitnessTime = getTimeElapsed(timer);

	//Sort individual by fitness
	timer = getTime();
	sortGenerationByFitness(ga->currentGeneration);
	sortTime = getTimeElapsed(timer);

	//Calculate the statistics
	timer = getTime();
	updateGenerationStatistics(ga->currentGeneration);
	statTime = getTimeElapsed(timer);

	if (ga->currentGeneration->statistics.invalidCount / ga->currentGeneration->size > INVALID_THRESHOLD
			|| ga->currentGeneration->statistics.lastChange > MAX_LAST_CHANGE) {

		//Add new valid random strategy

	}

	//Perform crossover
	timer = getTime();
	childCount = crossOver(ga->currentGeneration, ga->nextGeneration, fitnessProportionalSelection, singlePointCrossover);
	//childCount = crossOver(ga->currentGeneration, ga->nextGeneration, tournamentSelection, singlePointCrossover);
	//childCount = crossOver(ga->currentGeneration, ga->nextGeneration, rankSelection, singlePointCrossover);
	crossoverTime = getTimeElapsed(timer);

	//Perform mutations
	timer = getTime();
	mutationCount = 0;
	mutationCount += mutation(ga->nextGeneration, addRandomChangePoint, 		 ADD_POINT_MUTATION_RATE);
	mutationCount += mutation(ga->nextGeneration, removeRandomChangePoint, 		 REMOVE_POINT_MUTATION_RATE);
	mutationCount += mutation(ga->nextGeneration, moveRandomChangePoint, 		 CHANGE_POINT_POS_MUTATION_RATE);
	mutationCount += mutation(ga->nextGeneration, changeRandomChangePointAction, CHANGE_POINT_ACT_MUTATION_RATE);
	mutationTime = getTimeElapsed(timer);

	//Perform elitism selection
	timer = getTime();
	elitism(ga->currentGeneration, ga->nextGeneration, ELITISM_PERCENTAGE);
	FUSS(ga->currentGeneration, ga->nextGeneration);
	elitismTime = getTimeElapsed(timer);

	//Prints
	if(ga->generationCount % 10 == 0){
		//Get time
		generationTime = getTimeElapsed(generationTimer);

		//Print generation info
		printf("Gen %lu   individuals %d/%d   child %d   mutations %d\n",
				ga->generationCount,
				ga->currentGeneration->count,
				ga->currentGeneration->size,
				childCount,
				mutationCount
				);

		//Print best
		printf("Best energy %.2f   time %.2f/%.2f\n",
				ga->currentGeneration->statistics.best.simulation.energy,
				ga->currentGeneration->statistics.best.simulation.time,
				(float)TRACK_LENGTH / MIN_AVG_SPEED
				);

		//Print stats
		printf("Stat fmin %.2f   fmed %.2f   lavg %d   inv %.2f   sim %.2f   lch %lu\n",
				ga->currentGeneration->statistics.fitnessMin,
				ga->currentGeneration->statistics.fitnessMedian,
				(int)ga->currentGeneration->statistics.lengthAvg,
				(float)ga->currentGeneration->statistics.invalidCount / ga->currentGeneration->count,
				ga->currentGeneration->statistics.similarityAvg,
				ga->currentGeneration->statistics.lastChange
				);

		//Print time
		printf("Time ft %0.3lf   st %0.3lf   ut %0.3lf   et %0.3lf   ct %0.3lf   mt %0.3lf\n",
				fitnessTime / generationTime,
				sortTime / generationTime,
				statTime / generationTime,
				elitismTime / generationTime,
				crossoverTime / generationTime,
				mutationTime / generationTime
				);
		printf("Total time %lf\n", generationTime);
		printf("==============================\n");


	}

	//Next generation
	swap(ga->currentGeneration->individuals, ga->nextGeneration->individuals);
	ga->nextGeneration->count = 0;
	ga->generationCount++;
}

void disposeGA(GA* ga){
	//Dispose generation
	disposeGeneration(ga->currentGeneration);
	disposeGeneration(ga->nextGeneration);

	//Dispose track samples
	disposeTrackData();
}
