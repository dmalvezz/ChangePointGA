/*
 * screen.h
 *
 *  Created on: Jan 11, 2018
 *      Author: davide
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>

#include "../utils/mathutils.h"


extern WINDOW *gaOutputWindow, *gaParamWindow, *simParamWindow, *commandWindow;

extern void initWindows();


extern void disposeWindows();


#endif /* SCREEN_H_ */
