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
//WITH CURSORTOTHEBOTTOm
void InitPalladium(){
  #define ENTER_PALLADIUM_SCREEN printf("\33[?1049h\033[H")

  ENTER_PALLADIUM_SCREEN;

  struct termios old_settings, new_settings;

  tcgetattr(STDIN_FILENO, &old_settings);
  new_settings = old_settings;

  new_settings.c_lflag &= (~ICANON & ~ECHO);
  
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

static void FormatData(const unsigned char section, char* filepath) {
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

signed main(){
  InitPalladium();

  unsigned char section = 0;
  char* filepath;
  filepath = malloc(60*sizeof(char));
  memset(filepath, 0, 60*sizeof(char));

  section |= HOME;

  FormatScreen(section);
  FormatData(section, filepath);
  
  char c;

  while(true){
    c = getc(stdin);

    if(c == COLON) {
      printf("%c", COLON);
      CommandMode(&section, filepath);
      FormatData(section, filepath);
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