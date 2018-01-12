/*
 * console.h
 *
 *  Created on: Jan 12, 2018
 *      Author: davide
 */

#ifndef WINDOW_CONSOLE_H_
#define WINDOW_CONSOLE_H_

#include "window.h"
#include "../ga/ga.h"

extern void initConsole();

extern void printConsoleMenu();

extern void updateConsole(GA* ga, int* loop);

extern void disposeConsole();


#endif /* WINDOW_CONSOLE_H_ */
