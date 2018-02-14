/*
 ============================================================================
 Name        : sysutils.h
 Author      : Davide Malvezzi
 Version     : 1.0
 Copyright   :
 Description : System utils function
 ============================================================================
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>


static int getKey(void){
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF){
    ungetc(ch, stdin);
  }

  return ch;
}


static double getTime(){
   return MPI_Wtime();
}

static double getTimeElapsed(double start){
	double end = MPI_Wtime();
	return end - start;
}
