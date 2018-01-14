/*
 ============================================================================
 Name        : console.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Console
 ============================================================================
 */

#ifndef WINDOW_CONSOLE_H_
#define WINDOW_CONSOLE_H_

#include "window.h"
#include "commands.h"
#include "../ga/ga.h"
#include "../utils/uthash/uthash.h"

//Init the console
extern void initConsole();

//Print the menu
extern void printConsoleMenu();

//Update the console
extern void updateConsole(GA* ga, int* loop);

//Register a new command function
extern void registerCommand(const char* cmdName, CmdFunction cmdFunction);

//Parse a command
extern void parseCommand(GA* ga, char* buffer, int size);

//Dispose the console
extern void disposeConsole();


#endif /* WINDOW_CONSOLE_H_ */
