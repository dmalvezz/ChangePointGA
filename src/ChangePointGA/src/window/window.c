/*
 * screen.c
 *
 *  Created on: Jan 11, 2018
 *      Author: davide
 */

#include "window.h"

WINDOW *gaOutputWindow = NULL, *gaParamWindow = NULL, *simParamWindow = NULL, *commandWindow = NULL;

void initWindows(){
	//ncurses init
	initscr();
	cbreak();
	raw();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);

	gaOutputWindow = newwin(8, COLS, 0, 0);
	gaParamWindow = newwin(LINES - 9, COLS / 2, 8, 0);
	simParamWindow = newwin(LINES - 9, COLS / 2, 8, COLS / 2);
	commandWindow = newwin(1, COLS, LINES - 1, 0);
	refresh();

	wrefresh(gaParamWindow);
	wrefresh(simParamWindow);
}

void disposeWindows(){
	delwin(gaOutputWindow);
	delwin(gaParamWindow);
	delwin(simParamWindow);
	delwin(commandWindow);
	endwin();
}
