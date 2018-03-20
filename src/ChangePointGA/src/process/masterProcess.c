/*
 * masterProcess.c
 *
 *  Created on: Jan 20, 2018
 *      Author: davide
 */

#include "masterProcess.h"

static void genetic(MasterProcess* mProcess);

void initMaster(MasterProcess* mProcess, int worldId, int color){
	//Init process
	mProcess->worldId = worldId;
	//MPI_Comm_split(MPI_COMM_WORLD, color, worldId, &mProcess->comm);

	//Loop flag
	mProcess->loop = 1;

	#ifdef SAVE_STATISTICS
		//Create statistics file
		mProcess->statisticsFile = fopen(STATISTICS_FILE, "wt");
		fprintf(mProcess->statisticsFile, "generation, lastChange, bestEnergy, bestTime, bestFitness, %%invalid, fitnessInverseSum, validCount, deltaInv, dtInv, negVelInv, endVelInv, timeInv, notRun,");
		printStatisticCsvHeader("fitness", mProcess->statisticsFile);
		printStatisticCsvHeader("length", mProcess->statisticsFile);
		printStatisticCsvHeader("genotypeSim", mProcess->statisticsFile);
		printStatisticCsvHeader("genotypeAbsSim", mProcess->statisticsFile);
		printStatisticCsvHeader("fenotypeSim", mProcess->statisticsFile);
		printStatisticCsvHeader("fitnessSim", mProcess->statisticsFile);
		printStatisticCsvHeader("fitnessAbsSim", mProcess->statisticsFile);
		fprintf(mProcess->statisticsFile, "\n");
	#endif

	#ifdef SAVE_TIMES
		//Create time file
		mProcess->timesFile = fopen(TIME_FILE, "wt");
		fprintf(mProcess->timesFile, "generation, mutation, crossover, elitism, simulation, sort, stats, comm,\n");
	#endif

	#ifdef KEEP_BEST
		//Create best file
		mProcess->bestFile = fopen("bestSim.csv", "at");
	#endif

	mProcess->currBest = INFINITY;

	//Init console
	//initWindows();
	//initConsole();

	//Init ga
	initGA(&mProcess->ga,
			fitnessProportionalSelection,
			multiPointCrossover,
			energyFitness
		);
	addGAMutation(&mProcess->ga, addRandomChangePoint, 				ADD_POINT_MUTATION_RATE);
	addGAMutation(&mProcess->ga, removeRandomChangePoint, 			REMOVE_POINT_MUTATION_RATE);
	addGAMutation(&mProcess->ga, moveRandomChangePoint, 			CHANGE_POS_MUTATION_RATE);
	addGAMutation(&mProcess->ga, changeRandomChangePointAction, 	CHANGE_ACT_MUTATION_RATE);
	addGAMutation(&mProcess->ga, filterStrategy, 					FILTER_MUTATION_RATE);

#ifdef AUTO_LOAD_GENERATION
	if(access(GENERATION_FILE, F_OK ) != -1 ) {
		generationFromFile(mProcess->ga.currentGeneration, GENERATION_FILE);
	}
#endif

	//Sve settings
	saveSimulationParams(SIMULATION_FILE);
	saveGAParams(&mProcess->ga, GA_FILE);
}

void execMaster(MasterProcess* mProcess){
	//Refresh console
	//printGAParams(&mProcess->ga);
	//printSimulationParams();

	//Run first gen
	evalGenerationFitness(mProcess->ga.currentGeneration, mProcess->ga.fitnessFunction, NULL);
	sortGenerationByFitness(mProcess->ga.currentGeneration);
	updateGenerationStatistics(mProcess->ga.currentGeneration);

	//Loop
	while(mProcess->loop){
		//Run genetics
		genetic(mProcess);

		#ifdef SAVE_STATISTICS
			//Save statistic
			statisticsToFile(mProcess->ga.currentGeneration, mProcess->ga.generationCount, mProcess->statisticsFile);
		#endif


		//Auto save
		if(mProcess->currBest > mProcess->ga.currentGeneration->statistics.best.fitness){
			strategyToFile(&mProcess->ga.currentGeneration->statistics.best, BEST_FILE);
			generationToFile(mProcess->ga.currentGeneration, GENERATION_FILE);

			#ifdef KEEP_BEST
				//Save the new best
				for(int i = 0; i < SIM_STEP_COUNT; i++){
					fprintf(mProcess->bestFile, "%d,", mProcess->ga.currentGeneration->statistics.best.simulation.steps[i].map);
				}
				fprintf(mProcess->bestFile, "\n");
			#endif

			mProcess->currBest = mProcess->ga.currentGeneration->statistics.best.fitness;
		}
		//Update console
		//updateConsole(&mProcess->ga, &mProcess->loop);
	}

	//Save strategy and generation
	strategyToFile(&mProcess->ga.currentGeneration->statistics.best, BEST_FILE);
	generationToFile(mProcess->ga.currentGeneration, GENERATION_FILE);
}

void disposeMaster(MasterProcess* mProcess){
	//Close all slave process
	char cmd = SLAVE_QUIT_CMD;
	//MPI_Bcast(&cmd, 1, MPI_CHAR, 0, mProcess->comm);

	//Dispose GA
	disposeGA(&mProcess->ga);

	#ifdef SAVE_STATISTICS
		//Close statistics file
		fclose(mProcess->statisticsFile);
	#endif

	#ifdef SAVE_TIMES
		//Close times file
		fclose(mProcess->timesFile);
	#endif

	#ifdef KEEP_BEST
		//Close best file
		fclose(mProcess->bestFile);
	#endif

	//Dispose windows
	//disposeWindows();
	//disposeConsole();
}


void genetic(MasterProcess* mProcess){
	GA* ga = &mProcess->ga;

	//Counters
	int childCount = 0, mutationCount = 0;

	//Timers
	clock_t generationTimer, timer;
	double generationTime, fitnessTime, commTime, sortTime, statTime, elitismTime, crossoverTime, mutationTime;

	//Start timer
	generationTimer = getTime();

	//Perform crossover
	timer = getTime();
	childCount = crossover(ga->currentGeneration, ga->nextGeneration, ga->selectionFunction, ga->crossoverFunction);
	crossoverTime = getTimeElapsed(timer);

	//Perform mutations
	timer = getTime();
	for(int i = 0; i < ga->mutationCount; i++){
		mutationCount += mutation(ga->nextGeneration, ga->mutationsFunction[i], ga->mutationRates[i]);
	}
	mutationTime = getTimeElapsed(timer);

	//Perform elitism selection
	timer = getTime();
	elitism(ga->currentGeneration, ga->nextGeneration, ELITISM_PERCENTAGE);
	//FUSS(ga->currentGeneration, ga->nextGeneration);
	elitismTime = getTimeElapsed(timer);

	//Eval fitness
	timer = getTime();
	commTime = evalGenerationFitness(ga->nextGeneration, ga->fitnessFunction, NULL);
	fitnessTime = getTimeElapsed(timer);

	//Sort individual by fitness
	timer = getTime();
	sortGenerationByFitness(ga->nextGeneration);
	sortTime = getTimeElapsed(timer);

	//Calculate the statistics
	timer = getTime();
	swapv(ga->currentGeneration->individuals, ga->nextGeneration->individuals);
	swapv(ga->currentGeneration->simOutputs, ga->nextGeneration->simOutputs);

	updateGenerationStatistics(ga->currentGeneration);
	statTime = getTimeElapsed(timer);

	/* Implement migration
	if (ga->currentGeneration->statistics.invalidCount / ga->currentGeneration->size > INVALID_THRESHOLD
			|| ga->currentGeneration->statistics.lastChange > MAX_LAST_CHANGE) {

		//Add new valid random strategy

	}
	*/

	//Prints
	if(ga->generationCount % 10 == 0){
		//Get time
		generationTime = getTimeElapsed(generationTimer);

		//int row = 0;
		//wclear(gaOutputWindow);
		//box(gaOutputWindow,0,0);
		//Print generation info
		printf("Gen %lu   individuals %d/%d   child %d   mutations %d \n",
				ga->generationCount,
				ga->currentGeneration->count,
				ga->currentGeneration->size,
				childCount,
				mutationCount
			);

		//Print best
		printf("Best fitness %.2f   energy %.2f   time %.2f/%.2f \n",
				ga->currentGeneration->statistics.best.fitness,
				ga->currentGeneration->statistics.bestOutput.energy,
				ga->currentGeneration->statistics.bestOutput.time,
				MAX_TIME
			);

		//Print stats

		printf("Stat fmin %.2f   fmed %.2f   lavg %d   inv %.2f   sim %.2f   lch %lu \n",
				ga->currentGeneration->statistics.fitnessStat.min,
				ga->currentGeneration->statistics.fitnessStat.median,
				(int)ga->currentGeneration->statistics.lengthStat.avg,
				(float)ga->currentGeneration->statistics.invalidCount / ga->currentGeneration->count,
				ga->currentGeneration->statistics.fenotypeSimilarityStat.avg,
				ga->currentGeneration->statistics.lastChange
			);

		//Print time
		printf("Time ft %0.3lf   st %0.3lf   ut %0.3lf   et %0.3lf   ct %0.3lf   mt %0.3lf \n",
				fitnessTime,
				sortTime,
				statTime,
				elitismTime,
				crossoverTime,
				mutationTime
			);
		printf("Total %0.5lf    ser %0.5lf   par %0.5lf   comm %0.5lf \n",
				generationTime,
				sortTime + statTime + elitismTime + crossoverTime + mutationTime,
				fitnessTime - commTime,
				commTime
				);
		printf("=====================================================\n");
		//wrefresh(gaOutputWindow);
	}

	#ifdef SAVE_TIMES
		//Save times
		fprintf(mProcess->timesFile, "%lu, %f, %f, %f, %f, %f, %f, %f\n",
				ga->generationCount,
				mutationTime,
				crossoverTime,
				elitismTime,
				fitnessTime - commTime,
				sortTime,
				statTime,
				commTime
		);
	#endif

	//Next generation
	ga->nextGeneration->count = 0;
	ga->generationCount++;
}
