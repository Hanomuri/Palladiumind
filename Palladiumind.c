#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//THIS IS FOR TERMINAL SIZE DETECTION
#include <sys/ioctl.h>
#include <unistd.h>

#include <time.h>

#include<ctype.h>
#include<termios.h>

#include "Mind.h"
#include "CommandMode.h"

#define ESC_KEY               0x1B
#define COLON                 0x3A

#define HOME                  0x80
//□ ⛾ ⛝ FOR LATER

#define PALLADIUM_VERSION     0.01

#define COLOR_BOLD            "\e[1m"
#define COLOR_OFF             "\e[m"

#define EXIT_PALLADIUM_SCREEN puts("\33[?1049l")
//WITH CURSORTOTHEBOTTOm
void InitPalladium(){
  #define ENTER_PALLADIUM_SCREEN puts("\33[?1049h\033[H")

  ENTER_PALLADIUM_SCREEN;
  //printf(COLOR_BOLD "\033[1FTime Palladium Mind %.2f\n" COLOR_OFF, PALLADIUM_VERSION);
}
//ANOTHER FILE
void GetCurrentTime(char* timeBuffer) {

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
}
//PASS TO ANOTHER FILE
void CursorToTheBottom(){
  printf("\33[H\33[1F");

  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

  printf("\033[%d;0H", window.ws_row);

}

void FormatData(const __int8_t data) {
  printf("\033[3;0H\33[J");
  if(data && HOME) {
    ReadEntriesData();
  }
  CursorToTheBottom();
}
//ANOTHER FILE
void FormatScreen(){
  printf("\033[H");
  
  char time[5] = "00:00";
  GetCurrentTime(time);

  struct winsize terminal;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
  #define WIDTH terminal.ws_col

  printf(COLOR_BOLD "\033[1FPalladiumind" COLOR_OFF);
  
  char pagesName[4][10];
  strcpy(pagesName[0], "Home");
  strcpy(pagesName[1], "FutureLog");
  strcpy(pagesName[2], "Montly");
  strcpy(pagesName[3], "Custome");

  if(WIDTH > 45){
    for(int i = 0; i < 4; i++){
      for(int k = 0; k < (WIDTH-40)/5; k++){
        printf(" ");
      }
      printf("%s", pagesName[i]);
    }
  }
  
  printf("\33[0;%dH%s\n", WIDTH-4, time);
  
  printf(COLOR_BOLD);
  for(int i = 0; i < WIDTH; i++) printf("—");
  printf(COLOR_OFF);

  FormatData(HOME);

  fflush(stdout);
}

signed main(){
  InitPalladium();

  FormatScreen();
  
 struct termios old_settings, new_settings;

  tcgetattr(STDIN_FILENO, &old_settings);
  new_settings = old_settings;

  new_settings.c_lflag &= (~ICANON & ~ECHO);
  
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
  
  char c;

  while(true){
    c = getc(stdin);

    if(c == COLON) {
      printf("%c", COLON);
      CommandMode();
      FormatData(HOME);
    } else if (c == ESC_KEY) {
      break;
    }
  }

  EXIT_PALLADIUM_SCREEN;
  return 0;
}