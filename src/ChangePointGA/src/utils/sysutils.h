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

#define SECONDS 1000000000


static int kbhit(void){
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
    return 1;
  }

  return 0;
}

unsigned long long getTime(){
    struct timespec t;
    int r;

    r = clock_gettime(CLOCK_MONOTONIC, &t);
    if (r < 0) {
        fprintf(stderr,"Error to get time! (%i)\n", r);
        return -1;
    }

    return (unsigned long long) t.tv_sec * SECONDS + t.tv_nsec;
}

double getTimeElapsed(unsigned long long start){
	unsigned long long end = getTime();
	return (double)(end - start) / SECONDS;
}
