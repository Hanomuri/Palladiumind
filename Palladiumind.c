#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ctype.h>
#include <termios.h>
#include <unistd.h>


#include "Scripts/Mind.h"
#include "Scripts/Screen.h"
#include "Scripts/CommandMode.h"
#include "Scripts/Custom.h"

#define ESC_KEY               0x1B
#define COLON                 0x3A

#define PALLADIUM_VERSION     0.15

#define EXIT_PALLADIUM_SCREEN printf("\33[?1049l")
#define SHOW_CURSOR printf("\e[?25h")
#define INVISIBLE_CURSOR printf("\e[?25l")
//WITH CURSORTOTHEBOTTOm
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

  unsigned char section = 0;
  char* filepath;
  filepath = malloc(60*sizeof(char));
  memset(filepath, 0, 60*sizeof(char));

  section |= HOME;
  
  pthread_t checkThread;
  FormatData(section, filepath);
  pthread_create(&checkThread, NULL, DisplayCheck, &section);

  char c;

  while(true){
    c = getc(stdin);

    if(c == COLON) {
      printf("%c", COLON);
      CommandMode(&section, filepath);
    }
    else if (c == ESC_KEY && section & ENTRY) {
      section = section<<1;
      memset(filepath, 0, 60*sizeof(char));
      FormatScreen(section);
      FormatData(section, filepath);
    } 
    else if (c == ESC_KEY) {
      break;
    }
  }

  EXIT_PALLADIUM_SCREEN;
  return 0;
}