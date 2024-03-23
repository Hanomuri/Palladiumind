#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include <time.h>

#include "Screen.h"
#include "Custom.h"

#define WIDTH   terminal.ws_col
#define HEIGHT  terminal.ws_row

void CursorToTheBottom() {
  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

  printf("\033[%d;0H\33[K", HEIGHT);
}

static void CursorCommandToTheBottom(size_t* width) {
  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

  if (width == NULL) {
    printf("\033[%d;0H", HEIGHT);
  }
  else {
    printf("\033[%d;%ldH", HEIGHT, *width+2);
  }
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

void FormatData(Mind* mind) {
  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

  for(int currentRow = 3; currentRow <= HEIGHT-mind->heightInCommand; currentRow++) {
    printf("\033[%d;0H\33[K", currentRow);
  }
  printf("\033[3;0H");
  if (mind->section & HOME) {
  } 
  else if (mind->section & CUSTOM) {
    ReadCustomData(mind);
  }
  else if (mind->section & ENTRY) {
    ReadCustomPage(mind);
  }
  else if (mind->section & BOARD) {
    DisplayBoard(mind->filepath);
  }
  CursorCommandToTheBottom(mind->currentCollum);
  fflush(stdout);
}

void FormatHomeTopBar(const Mind* mind, const int width) {
  char pagesName[4][10];
  memset(pagesName[0], 0, 10*sizeof(char));
  memset(pagesName[1], 0, 10*sizeof(char));
  memset(pagesName[2], 0, 10*sizeof(char));
  memset(pagesName[3], 0, 10*sizeof(char));
  
  strcpy(pagesName[0], "Home");
  strcpy(pagesName[1], "Marked");
  strcpy(pagesName[2], "Custom");
  strcpy(pagesName[3], "Config");
  
  unsigned char currentSection;

  if(width > 45){
    for(int i = 0; i < 4; i++){
      for(int k = 0; k < (width-40)/5; k++){
        printf(" ");
      }
      currentSection = pow(2, 4-i);
      if(!(currentSection<<3 & mind->section)) {
        printf(TEXT_DIM);
      }
      printf("%s", pagesName[i]);
      printf(ATTR_OFF);
    } 
  }
  fflush(stdout);
}

void FormatScreen(const Mind* mind){
  printf("\033[H\33[K");

  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

  printf(COLOR_BOLD "\033[1FPalladiumind" ATTR_OFF);
  
  if((mind->section & ENTRY || mind->section & BOARD)) {
    printf(" \033[%ldC", (WIDTH-strlen(mind->pageName))/2-strlen("Palladiumind"));
    printf(COLOR_BOLD "%s" ATTR_OFF, mind->pageName);
  } else {
    FormatHomeTopBar(mind, WIDTH);
  }

  PrintCurrentTime(WIDTH);

  printf(COLOR_BOLD);
  for(int i = 0; i < WIDTH; i++) printf("—");
  printf(ATTR_OFF);
}

void* DisplayCheck(void* args) {
  Mind* mind = (Mind*)args;
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
      FormatScreen(mind);
      FormatData(mind);
      CursorCommandToTheBottom(mind->currentCollum);
    }
    if (minutes != now_tm->tm_min) {
      minutes = now_tm->tm_min;
      PrintCurrentTime(width);
      CursorCommandToTheBottom(mind->currentCollum);
    }
  }
  return NULL;
}