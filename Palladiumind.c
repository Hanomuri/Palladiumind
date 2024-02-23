#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#ifndef _PALLADIUM_MIND
  #define _PALLADIUM_MIND
  #include "Scripts/Mind.h"
#endif
#include "Scripts/Screen.h"
#include "Scripts/CommandMode.h"
#include "Scripts/Custom.h"

#define ESC_KEY               0x1B
#define COLON                 0x3A

#define PALLADIUM_VERSION     0.15

#define EXIT_PALLADIUM_SCREEN printf("\33[?1049l")
#define INVISIBLE_CURSOR printf("\e[?25l")

void InitPalladium(){
  #define ENTER_PALLADIUM_SCREEN printf("\33[?1049h\033[H")

  ENTER_PALLADIUM_SCREEN;

  struct termios old_settings, new_settings;

  tcgetattr(STDIN_FILENO, &old_settings);
  new_settings = old_settings;

  new_settings.c_lflag &= (~ICANON & ~ECHO);
  
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

  INVISIBLE_CURSOR;
}

signed main(){
  InitPalladium();

  Mind mind;
  mind.currentNavi     = 0;
  mind.maximumNavi     = 0;
  mind.section         = 0;
  mind.section         |= HOME;
  mind.heightInCommand = 0;
  mind.currentCollum   = NULL;
  mind.filepath        = malloc(60*sizeof(char));
  mind.pageName        = malloc(45*sizeof(char));
  memset(mind.filepath, 0, 60*sizeof(char));
  
  pthread_t checkThread;
  FormatData(&mind);
  pthread_create(&checkThread, NULL, DisplayCheck, &mind);

  char c;

  //printf("\33[0;34m"); BLUE TEXT TO SETTINGS :D

  while(true){
    c = getc(stdin);

    if(c == COLON) {
      printf("%c", COLON);
      CommandMode(&mind);
    }
    else if (c == ESC_KEY && (mind.section & ENTRY || mind.section & BOARD)) {
      mind.section = CUSTOM;
      memset(mind.filepath, 0, 60*sizeof(char));
      FormatScreen(&mind);
      FormatData(&mind);
    } 
    else if (c == ESC_KEY) {
      break;
    }
  }

  EXIT_PALLADIUM_SCREEN;

  return 0;
}