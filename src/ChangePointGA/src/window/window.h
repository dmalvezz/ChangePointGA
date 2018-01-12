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

extern WINDOW *gaOutputWindow, *gaParamWindow, *simParamWindow, *commandWindow;

//Init ncurses session
extern void initWindows();

//Dispose ncurses session
extern void disposeWindows();


#endif /* SCREEN_H_ */
