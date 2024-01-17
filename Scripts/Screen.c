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
#include "Custom.h"

#define WIDTH   terminal.ws_col
#define HEIGHT  terminal.ws_row

void CursorToTheBottom() {
  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

  printf("\033[%d;0H\33[K", HEIGHT);
}

unsigned char GetTerminalWidth() {
  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
  return WIDTH;
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

void FormatData(const unsigned char section, const char* filepath) {
  printf("\033[3;0H\33[J");
  if (section & HOME) {
    ReadEntriesData();
  } 
  else if (section & CUSTOM) {
    ReadCustomData();
  }
  else if (section & ENTRY) {
    ReadCustomPage(filepath);
  }
  CursorToTheBottom();
}

void FormatScreen(const unsigned char section){
  printf("\033[H\33[K");

  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

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

void* DisplayCheck(void* args) {
  unsigned char *section = args;
  struct winsize terminal;
  short width   = 0;
  short height  = 0;

  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);
  int minutes = now_tm->tm_min;

  while (1){
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
    time_t now = time(NULL);
    now_tm = localtime(&now);
    if(width != WIDTH || height != HEIGHT) {
      width   = WIDTH;
      height  = HEIGHT;
      FormatScreen(*section);
      CursorToTheBottom();
    }
    if (minutes != now_tm->tm_min) {
      minutes = now_tm->tm_min;
      PrintCurrentTime(width);
      CursorToTheBottom();
    }
  }
  return NULL;
}