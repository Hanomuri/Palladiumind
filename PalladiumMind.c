#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//THIS IS FOR TERMINAL SIZE DETECTION
#include <sys/ioctl.h>
#include <unistd.h>

#define PALLADIUM_VERSION 0.01

#define COLOR_BOLD  "\e[1m"
#define COLOR_OFF   "\e[m"

#define EXIT_PALLADIUM_SCREEN puts("\33[?1049l")


void InitPalladium(){
  #define ENTER_PALLADIUM_SCREEN puts("\33[?1049h\033[H")

  ENTER_PALLADIUM_SCREEN;
  printf(COLOR_BOLD "\033[1FTime Palladium Mind %.2f\n" COLOR_OFF, PALLADIUM_VERSION);
}

bool PalladiumShouldClose(void){

  return true;
}

void CursorToTheBottom(){
  printf("\33[H\33[1F");

  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

  printf("\033[%d;0H:", window.ws_row);

  fflush(stdout);
}

signed main(){
  InitPalladium();


  CursorToTheBottom();
  
  char* input = malloc(128*sizeof(char));
  
  while(true){
    scanf("%128s%*c", input);
    
    
    fflush(stdin);

    if(strcmp(input, "exit") == 0){
      break;
    }
  }

  EXIT_PALLADIUM_SCREEN;
  return 0;
}