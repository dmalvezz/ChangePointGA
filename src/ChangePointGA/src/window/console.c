/*
 * console.c
 *
 *  Created on: Jan 12, 2018
 *      Author: davide
 */

#include "console.h"

void initConsole(){
	printConsoleMenu();
}

void printConsoleMenu(){
	wclear(commandWindow);
	wprintw(commandWindow, "Command(c)   Load(l)   Save(s)   Pause(p)   Quit(q)");
	wrefresh(commandWindow);
}

void updateConsole(GA* ga, int* loop){
	int key = getch();

	switch (key){

		case 's':
			//Save
			wclear(commandWindow);
			wprintw(commandWindow, "Saving...");
			wrefresh(commandWindow);

			strategyToFile(&ga->currentGeneration->statistics.best, BEST_FILE);
			generationToFile(ga->currentGeneration, GENERATION_FILE);
			saveSimulationParams(SIMULATION_FILE);

			printConsoleMenu();
			break;

		case 'q':
			//Quit
			(*loop) = 0;
			break;

		case 'p':
			//Pause
			wclear(commandWindow);
			wprintw(commandWindow, "Press a key to resume...");
			wrefresh(commandWindow);

			while (getch() == ERR);

			printConsoleMenu();
			break;

		case 'l':
			//Load
			wclear(commandWindow);
			wprintw(commandWindow, "Loading generation from file...");
			wrefresh(commandWindow);

			generationFromFile(ga->currentGeneration, GENERATION_FILE);

			printConsoleMenu();
			break;

	}
}


void disposeConsole(){

}
