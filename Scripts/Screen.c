#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//FOR TERMINAL SIZE
#include <sys/ioctl.h>
#include <unistd.h>

#include <time.h>

#include "Mind.h"
#include "Screen.h"

void CursorToTheBottom(){
  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

  printf("\033[%d;0H\33[K", window.ws_row);
}

void PrintCurrentTime(unsigned char width) {
  char* timeBuffer = malloc(6*sizeof(char));
  memset(timeBuffer, 0, 6*sizeof(char));
  memcpy(timeBuffer, "00:00", 6*sizeof(char));

  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);

  int hour = now_tm->tm_hour;
  int minutes = now_tm->tm_min;
  
  if(hour/10.0 <= 1) timeBuffer[1] = hour + '0';
  else {
    timeBuffer[0] = ( hour / 10 ) + '0';
    timeBuffer[1] = ( hour % 10 ) + '0';
  }
  
  if(minutes/10.0 <= 1) timeBuffer[4] = minutes + '0';
  else {
    timeBuffer[3] = ( minutes / 10 ) + '0';
    timeBuffer[4] = ( minutes % 10 ) + '0';
  }
  
  printf("\33[0;%dH%s\n", width-4, timeBuffer);
}

void FormatScreen(const unsigned char section){
  printf("\033[H\33[K");

  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
  #define WIDTH terminal.ws_col

  printf(COLOR_BOLD "\033[1FPalladiumind" ATTR_OFF);
  
  char pagesName[4][10];
  memset(pagesName[0], 0, 10*sizeof(char));
  memset(pagesName[1], 0, 10*sizeof(char));
  memset(pagesName[2], 0, 10*sizeof(char));
  memset(pagesName[3], 0, 10*sizeof(char));
  
  strcpy(pagesName[0], "Home");
  strcpy(pagesName[1], "FutureLog");
  strcpy(pagesName[2], "Montly");
  strcpy(pagesName[3], "Custom");
  
  unsigned char currentSection;

  if(WIDTH > 45){
    for(int i = 0; i < 4; i++){
      for(int k = 0; k < (WIDTH-40)/5; k++){
        printf(" ");
      }
      currentSection = pow(2, 4-i);
      if(!(currentSection<<3 & section)) {
        printf(TEXT_DIM);
      }
      printf("%s", pagesName[i]);
      printf(ATTR_OFF);
    } 
  }

  PrintCurrentTime(WIDTH);

  printf(COLOR_BOLD);
  for(int i = 0; i < WIDTH; i++) printf("—");
  printf(ATTR_OFF);

  fflush(stdout);
}