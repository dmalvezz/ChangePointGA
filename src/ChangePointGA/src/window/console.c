/*
 * console.c
 *
 *  Created on: Jan 12, 2018
 *      Author: davide
 */

/*
#include "console.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../ga/ga.h"
#include "../model/simulation.h"
#include "../utils/mathutils.h"

static int stratIndex = 0;
static Command* commandList = NULL;

void initConsole(){
	//Create argv buffer
	initArgv();
	//Print console menu
	printConsoleMenu();

	//Register all commands
	registerCommand("filter", applyFilterToStrategy);

	registerCommand("loadstrat", loadStrategyFromCsv);
	registerCommand("savestrat", saveStrategyToCsv);

	registerCommand("savegen", saveGenerationToCsv);

	registerCommand("plotsim", plotSimulation);
	registerCommand("setmutrate", setMutationRate);
}

void printConsoleMenu(){
	wclear(commandWindow);
	wprintw(commandWindow, "Command(c)   Load(l)   Save(s)   Pause(p)   Quit(q)");
	wrefresh(commandWindow);
}

void updateConsole(GA* ga, int* loop){
	int key = getch();

	int cmdMode = 1;
	int cmdBufferSize = 0;
	char cmdBuffer[COLS];
	char prevCmd[COLS];

	//Empty buffer
	cmdBuffer[0] = '\0';
	prevCmd[0] = '\0';

	switch (key){
		case 'c':
			//Command mode

			//Print explorer
			printExplorerWindow(ga, stratIndex);

			//Prepare console
			wclear(commandWindow);
			curs_set(1);
			mvwprintw(commandWindow, 0, 0, "> ");
			wrefresh(commandWindow);

			//Read cmd
			while(cmdMode){
				int c = getch();

				if(c != ERR){
					switch(c){
						//Cancel last char
						case KEY_BACKSPACE:
							cmdBufferSize = max(0, cmdBufferSize - 1);
							cmdBuffer[cmdBufferSize] = '\0';
							break;

						//Close console
						case KEY_END:
							cmdMode = 0;
							break;

						//Parse cmd
						case '\n':
							//Save current cmd
							strncpy(prevCmd, cmdBuffer, cmdBufferSize);
							prevCmd[cmdBufferSize] = '\0';
							//Parse
							parseCommand(ga, cmdBuffer, cmdBufferSize);
							//Reset buffer
							cmdBufferSize = 0;
							cmdBuffer[cmdBufferSize] = '\0';
							break;

						case KEY_UP:
							strcpy(cmdBuffer, prevCmd);
							cmdBufferSize = strlen(prevCmd);
							break;

						case KEY_NPAGE:
							stratIndex = max(0, stratIndex - 1);
							printExplorerWindow(ga, stratIndex);
							break;

						case KEY_PPAGE:
							stratIndex = min(stratIndex + 1, ga->currentGeneration->count - 1);
							printExplorerWindow(ga, stratIndex);
							break;

						//Read cmd char
						default:

							if(cmdBufferSize < COLS - 1){
								cmdBuffer[cmdBufferSize++] = c;
								cmdBuffer[cmdBufferSize] = '\0';
							}
					}

					wclear(commandWindow);
					mvwprintw(commandWindow, 0, 0, "> %s", cmdBuffer);
					wrefresh(commandWindow);
				}

			}

			//Reset console
			curs_set(0);
			printConsoleMenu();

			break;

		case 's':
			//Save
			wclear(commandWindow);
			wprintw(commandWindow, "Saving...");
			wrefresh(commandWindow);

			strategyToFile(&ga->currentGeneration->statistics.best, BEST_FILE);
			generationToFile(ga->currentGeneration, GENERATION_FILE);
			saveSimulationParams(SIMULATION_FILE);
			saveGAParams(ga, GA_FILE);

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

void registerCommand(const char* cmdName, CmdFunction cmdFunction){
	Command* cmd = (Command*)malloc(sizeof(Command));
	strcpy(cmd->name, cmdName);
	cmd->cmdFunction = cmdFunction;
	HASH_ADD_PTR(commandList, name, cmd);
}

void parseCommand(GA* ga, char* buffer, int size){
	char cmdName[CMD_SIZE];
	int argc = 1;

	argv[0][0] = 'c';
	argv[0][1] = 'm';
	argv[0][2] = 'd';
	argv[0][3] = '\0';

	//Read cmd name
	int i = 0, j;
	while(buffer[i] != ' ' && i < size && i < CMD_SIZE){
		cmdName[i] = buffer[i];
		i++;
	}
	cmdName[min(i, CMD_SIZE - 1)] = '\0';

	//Read params
	for(; i < size; i++){
		if(buffer[i] == '-' && argc < CMD_PARAM_COUNT){
			j = 0;
			while(buffer[i] != ' ' && i < size && j < CMD_PARAM_SIZE){
				argv[argc][j] = buffer[i];
				i++;
				j++;
			}
			argv[argc][min(j, CMD_PARAM_SIZE - 1)] = '\0';
			argc++;
		}
	}

	//Exec command
	Command* cmdPtr;
	HASH_FIND_PTR(commandList, &cmdName, cmdPtr);

	if(cmdPtr == NULL){
		mvwprintw(commandWindow, 0, 0, "Command not found! Press a key to continue...");
		wrefresh(commandWindow);
		while(getch() == ERR);
	}
	else{
		//Reset getopt
		optind = 1;
		int res = cmdPtr->cmdFunction(ga, argv, argc);
		if(res != 0){
			wclear(commandWindow);
			wprintw(commandWindow, "%s", errorBuffer);
			wrefresh(commandWindow);
			while(getch() == ERR);
		}
	}

}

void disposeConsole(){
	//Dispose hash
	HASH_CLEAR(hh, commandList);

	//Dispose argv
	disposeArgv();
}
*/
