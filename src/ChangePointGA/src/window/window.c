/*
 * screen.c
 *
 *  Created on: Jan 11, 2018
 *      Author: davide
 */

#include "window.h"

WINDOW *gaOutputWindow = NULL, *gaParamWindow = NULL, *simParamWindow = NULL, *commandWindow = NULL, *explorerWindow = NULL;

void initWindows(){
	//ncurses init
	initscr();
	cbreak();
	raw();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);

	gaOutputWindow = newwin(7, COLS, 0, 0);
	gaParamWindow = newwin(11, COLS / 2, 7, 0);
	simParamWindow = newwin(5, COLS / 2, LINES-6, 0);
	explorerWindow = newwin(16, COLS / 2, 7, COLS / 2);
	commandWindow = newwin(1, COLS, LINES - 1, 0);
	refresh();

	box(gaOutputWindow, 0, 0);
	box(gaParamWindow, 0, 0);
	box(simParamWindow, 0, 0);
	box(explorerWindow, 0, 0);

	wrefresh(gaOutputWindow);
	wrefresh(gaParamWindow);
	wrefresh(simParamWindow);
	wrefresh(explorerWindow);
}

void disposeWindows(){
	delwin(gaOutputWindow);
	delwin(gaParamWindow);
	delwin(simParamWindow);
	delwin(commandWindow);
	endwin();
}
