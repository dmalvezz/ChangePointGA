/*
 * masterProcess.c
 *
 *  Created on: Jan 20, 2018
 *      Author: davide
 */

#include "masterProcess.h"

#define SINGLE_GA
//#define LEADERS_FOLLOWERS

static void prepareGA(MasterProcess* mProcess);
static void execGA(MasterProcess* mProcess, int generationMax);
static void genetic(MasterProcess* mProcess);


void initMaster(MasterProcess* mProcess, int worldId, int color){
	//Init process
	mProcess->worldId = worldId;
	MPI_Comm_split(MPI_COMM_WORLD, color, worldId, &mProcess->comm);

	//Loop flag
	mProcess->loop = 1;

	//Init console
	//initWindows();
	//initConsole();

#ifdef SINGLE_GA
	prepareGA(mProcess);
#endif
}

void execMaster(MasterProcess* mProcess){
	#ifdef SINGLE_GA
		execGA(mProcess, 1);
	#endif

	#ifdef LEADERS_FOLLOWERS
		Strategy leaders[POPULATION_SIZE];
		char dirName[16];

		for(int i = 0; i < 5; i++){
			sprintf(dirName, "run%d", i);
			mkdir(dirName, 0700);
			chdir(dirName);

			prepareGA(mProcess);

			execGA(mProcess, 10000);
			memcpy(&leaders[i * 10], mProcess->ga.currentGeneration->individuals, sizeof(Strategy) * 10);

			disposeGA(&mProcess->ga);

			chdir("..");
		}

		prepareGA(mProcess);
		memcpy(mProcess->ga.currentGeneration->individuals, leaders, sizeof(Strategy) * 50);

		execGA(mProcess, 100000);

		disposeGA(&mProcess->ga);

	#endif
}

void disposeMaster(MasterProcess* mProcess){
	//Close all slave process
	char cmd = SLAVE_QUIT_CMD;
	MPI_Bcast(&cmd, 1, MPI_CHAR, 0, mProcess->comm);

	//Dispose GA
	#ifdef SINGLE_GA
		disposeGA(&mProcess->ga);
	#endif

	#ifdef SAVE_STATISTICS
		//Close statistics file
		fclose(mProcess->statisticsFile);
	#endif

	#ifdef SAVE_TIMES
		//Close times file
		fclose(mProcess->timesFile);
	#endif

	#ifdef SAVE_PROFILES
		//Create profiles file
		fclose(mProcess->profilesFile);
	#endif

	#ifdef KEEP_BEST
		//Close best file
		fclose(mProcess->bestFile);
	#endif

	//Dispose windows
	//disposeWindows();
	//disposeConsole();
}

void prepareGA(MasterProcess* mProcess){
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

		printStatisticCsvHeader("neutral", mProcess->statisticsFile);
		printStatisticCsvHeader("activeToNeutral", mProcess->statisticsFile);
		printStatisticCsvHeader("neutralToActive", mProcess->statisticsFile);


		printStatisticCsvHeader("bestChildren", mProcess->statisticsFile);
		fprintf(mProcess->statisticsFile, "bestChildrenCount, bestChildrenInvalid,");

		fprintf(mProcess->statisticsFile, "\n");
	#endif

	#ifdef SAVE_TIMES
		//Create time file
		mProcess->timesFile = fopen(TIME_FILE, "wt");
		fprintf(mProcess->timesFile, "generation, mutation, crossover, elitism, simulation, sort, stats, comm,\n");
	#endif

	#ifdef SAVE_PROFILES
		//Create profiles file
		mProcess->profilesFile = fopen(PROFILES_FILE, "wb");
	#endif

	#ifdef KEEP_BEST
		//Create best file
		mProcess->bestFile = fopen("bestSim.csv", "at");
	#endif

	mProcess->currBest = INFINITY;

	//Init ga
	initGA(&mProcess->ga,
			linearRankWithPressureSelection,
			singlePointCrossover,
			energyTimeFitness
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

	#ifdef LOAD_SEEDS
		int i = mProcess->ga.currentGeneration->count - 1;
		struct dirent *dp;
		DIR *dfd;
		if ((dfd = opendir(SEED_DIR)) != NULL){
			char filename[100];
			while ((dp = readdir(dfd)) != NULL && i > 0){
				  sprintf(filename, "%s/%s", SEED_DIR, dp->d_name);

				  if(strlen(filename) > 4 && !strcmp(filename + strlen(filename) - 4, ".csv")){
					  strategyFromCsv(&mProcess->ga.currentGeneration->individuals[i], filename);
					  i--;
					  printf("loaded %s\n", filename);
				  }

			}
		}
	#endif
	//strategyFromCsv(&mProcess->ga.currentGeneration->individuals[0], "str.csv");

	//Save settings
	saveSimulationParams(SIMULATION_FILE);
	saveGAParams(&mProcess->ga, GA_FILE);
}

void execGA(MasterProcess* mProcess, int generationMax){
	//Refresh console
	//printGAParams(&mProcess->ga);
	//printSimulationParams();

	//Run first gen
	evalGenerationFitness(mProcess->ga.currentGeneration, mProcess->ga.fitnessFunction, /*&mProcess->comm*/ NULL);
	for(int i = 0; i < POPULATION_SIZE; i++){
		printf("%d %f %f\n", mProcess->ga.currentGeneration->simOutputs[i].result,
				mProcess->ga.currentGeneration->simOutputs[i].energy,
				mProcess->ga.currentGeneration->simOutputs[i].time);
	}

	sortGenerationByFitness(mProcess->ga.currentGeneration);
	updateGenerationStatistics(mProcess->ga.currentGeneration);

	//Loop
#ifdef SINGLE_GA
	while(mProcess->loop){
#endif

#ifdef LEADERS_FOLLOWERS
	while(mProcess->ga.generationCount < generationMax){
#endif
		//MAX_TIME = 2200 * exp(-0.00000093 * (mProcess->ga.generationCount + 1));
		//MAX_TIME = RACE_TIME + 60.0 * (1.0 - (float)mProcess->ga.generationCount / 50000.0);
		//if(MAX_TIME < RACE_TIME)MAX_TIME = RACE_TIME;
		//Run genetics
		genetic(mProcess);

		#ifdef SAVE_STATISTICS
			//Save statistic
			statisticsToFile(mProcess->ga.currentGeneration, mProcess->ga.generationCount, mProcess->statisticsFile);
		#endif

		//Auto save
		generationToFile(mProcess->ga.currentGeneration, GENERATION_FILE);
		if(mProcess->currBest > mProcess->ga.currentGeneration->statistics.best.fitness){
			strategyToFile(&mProcess->ga.currentGeneration->statistics.best, BEST_FILE);

			#ifdef KEEP_BEST
				//Save the new best
				float profile[SIM_STEP_COUNT];
				getMapProfile(&mProcess->ga.currentGeneration->statistics.best, profile);
				for(int i = 0; i < SIM_STEP_COUNT; i++){
					fprintf(mProcess->bestFile, "%d,", (int)profile[i]);
				}
				fprintf(mProcess->bestFile, "\n");
				fflush(mProcess->bestFile);
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

void genetic(MasterProcess* mProcess){
	GA* ga = &mProcess->ga;

	//Counters
	int childCount = 0, mutationCount = 0;

	//Timers
	double generationTimer, timer;
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
	commTime = evalGenerationFitness(ga->nextGeneration, ga->fitnessFunction, /*&mProcess->comm*/ NULL);
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

		if(ga->currentGeneration->statistics.lastChange == 1000){
			for(int i = 0; i < ga->currentGeneration->count / 2 - 1; i++){
				initStrategy(&ga->currentGeneration->individuals[ga->currentGeneration->count / 2 + i], SPACE_STEP);
			}
		}
	}

	#ifdef SAVE_PROFILES
		//Create profiles file
		float profile[SIM_STEP_COUNT];
		for(int i = 0; i < mProcess->ga.currentGeneration->count; i++){
			getMapProfile(&mProcess->ga.currentGeneration->individuals[i], profile);
			for(int i = 0; i < SIM_STEP_COUNT; i++){
				fprintf(mProcess->profilesFile, "%d,", (int)profile[i]);
			}
			fprintf(mProcess->profilesFile, "\n");
			//fwrite(profile, sizeof(float), SIM_STEP_COUNT, mProcess->profilesFile);
		}
		fflush(mProcess->profilesFile);
	#endif

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
