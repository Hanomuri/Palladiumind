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


#define UP_KEY                0x48
#define LEFT_KEY              0x4B
#define DOWN_KEY              0x50
#define RIGHT_KEY             0x4D
#define ESC_KEY               0x1B
#define COLON                 0x3A

#define ESCAPE_ANSII          0x1B

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
  
  pthread_t checkThread, escapeCheckThread;
  FormatData(&mind);
  pthread_create(&checkThread, NULL, DisplayCheck, &mind);

  unsigned char c;
  unsigned char escape;
  unsigned char secuence;

  FILE* a = fopen("a.txt", "a");

  //printf("\33[0;34m"); BLUE TEXT TO SETTINGS :D

  while(true){
    c = getc(stdin);

    if(c == ESCAPE_ANSII) {
      escape    = getc(stdin);
      secuence  = getc(stdin);
    }

    fprintf(a, "%c\n", escape);
    fprintf(a, "%c\n", secuence);
    
    if(c == COLON) {
      printf("%c", COLON);
      CommandMode(&mind);
    }
    else if (c == UP_KEY || c == LEFT_KEY || c == DOWN_KEY || c == RIGHT_KEY) {

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

  fclose(a);

  EXIT_PALLADIUM_SCREEN;

  return 0;
}