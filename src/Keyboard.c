#include <stdio.h>
#include <unistd.h>
#include "Keyboard.h"

#define UP_CODE                0x41
#define LEFT_CODE              0x42
#define DOWN_CODE              0x43
#define RIGHT_CODE             0x44

#define ESCAPE_ANSII          0x1B
#define SPECIAL_FUNCTION      0x5b

#define MAX_KEYBUFFER_SIZE    3    // Max size in bytes to read

short KeyboardParser(Keyboard* keyboard)
{ 
  // POSIX system call
  keyboard->bytesToRead = read(STDIN_FILENO, keyboard->keysBuffer, MAX_KEYBUFFER_SIZE);
  for ( int i = 0; i < keyboard->bytesToRead; i++ )
  {
    if ( keyboard->keysBuffer[i] == 0x1b )
    {
      if ( keyboard->bytesToRead == 1 ) 
      {
        return KEY_ESC;  
      }
      else 
      {
        if ( keyboard->keysBuffer[i + 1] == SPECIAL_FUNCTION )    // [
        { 
          if ( keyboard->keysBuffer[i + 2] == 65 )
          {
            return KEY_UP;
          }
          else if ( keyboard->keysBuffer[i + 2] == 66 )
          {
            return KEY_DOWN;
          }
          else if ( keyboard->keysBuffer[i + 2] == 67 )
          {
            return KEY_RIGHT;
          }
          else if ( keyboard->keysBuffer[i + 2] == 68 )
          {
            return KEY_LEFT;
          }
          else
          {
            break;
          }
        }
      }
    }
  }
  return keyboard->keysBuffer[0];
}