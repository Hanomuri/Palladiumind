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
  #include "Mind.h"
#endif
#include "CommandMode.h"
#include "Custom.h"
#ifndef _PALLADIUM_KEYBOARD
  #define _PALLADIUM_MIND
  #include "Keyboard.h"
#endif
#include "NaviMode.h"
#include "Screen.h"

#define COLON                 0x3A

#define PALLADIUM_VERSION     0.15

#define EXIT_PALLADIUM_SCREEN printf("\33[?1049l")
#define INVISIBLE_CURSOR printf("\e[?25l")

void InitPalladium()
{
  #define ENTER_PALLADIUM_SCREEN printf("\33[?1049h\033[H")

  ENTER_PALLADIUM_SCREEN;

  fflush(stdout);

  INVISIBLE_CURSOR;
}

signed main()
{
  InitPalladium();

  struct termios old_settings, new_settings;

  tcgetattr(STDIN_FILENO, &old_settings);
  new_settings = old_settings;

  new_settings.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
  
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

  Mind mind = GenMind();
  
  pthread_t checkThread;
  FormatData(&mind);
  pthread_create(&checkThread, NULL, DisplayCheck, &mind);

  struct Keyboard keyboard;
  keyboard.bytesToRead = 0;

  short c;

  while(true){
    c = KeyboardParser(&keyboard);

    if(c == COLON) {
      CommandMode(&mind, &keyboard);
    }
    else if (c == KEY_UP || c == KEY_DOWN || c == KEY_RIGHT || c == KEY_LEFT) {
      NaviMode(&mind, &keyboard, c);
    }
    else if (c == KEY_ESC && (mind.section & ENTRY || mind.section & BOARD)) {
      mind.section = CUSTOM;
      memset(mind.filepath, 0, 60*sizeof(char));
      memset(mind.pageName, 0, 45*sizeof(char));
      FormatScreen(&mind);
      FormatData(&mind);
    } 
    else if (c == KEY_ESC) {
      break;
    }
  }

  EXIT_PALLADIUM_SCREEN;

  tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

  return 0;
}