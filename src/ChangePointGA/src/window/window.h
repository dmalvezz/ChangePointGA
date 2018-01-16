/*
 ============================================================================
 Name        : window.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : Ncurses windows manager
 ============================================================================
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>

#include "../utils/mathutils.h"

//#define DEBUG_WINDOW

extern WINDOW *gaOutputWindow, *gaParamWindow, *simParamWindow, *commandWindow, *explorerWindow;
extern WINDOW* debugWindow;

//Init ncurses session
extern void initWindows();

//Dispose ncurses session
extern void disposeWindows();


#endif /* SCREEN_H_ */
