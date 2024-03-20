#include <stdio.h>
#include "CommandMode.h"
#include "CoreMode.h"
#include "NaviMode.h"
#include "NaviMode/NaviCustom.h"
#include "Screen.h"
#ifndef _PALLADIUM_KEYS
  #define _PALLADIUM_KEYS
  #define KEY_SPACE         0x20
  #define KEY_DELETE        0x7F
  #define KEY_ENTER         '\n'
#endif


#define COLON                 0x3A

void NaviMode(Mind* mind, Keyboard* keyboard, short c) 
{
  if (c == KEY_DOWN || c == KEY_UP)
  {
    c = 0;
  }

  mind->currentNavi = 1;
  FormatData(mind);
  
  do
  {
    if( c == KEY_ESC ) 
    {
      break;
    }
    else if ( c == COLON)
    {
      CommandMode(mind, keyboard);
    }
    else if (c == KEY_UP && (mind->section == CUSTOM || mind->section == ENTRY))
    {
      if (mind->currentNavi > 1)
      {
        mind->currentNavi--;
      }
    }
    else if (c == KEY_DOWN && (mind->section == CUSTOM || mind->section == ENTRY))
    {
      if (mind->currentNavi < mind->maximumNavi)
      {
        mind->currentNavi++;
      }
    }
    else if (c == KEY_RIGHT && (mind->section <= HOME && mind->section >= CONFIG) )
    {
     if ( mind->section == CONFIG ) 
     {
      mind->section = HOME;
     }
     else
     {
      mind->section = mind->section>>1;
     }
    }
    else if (c == KEY_LEFT && (mind->section <= HOME && mind->section >= CONFIG) )
    {
     if ( mind->section == HOME ) 
     {
      mind->section = CONFIG;
     }
     else
     {
      mind->section = mind->section<<1;
     }
    }
    else if ( c == KEY_ENTER ) 
    {
      if ( mind->section == CUSTOM )
      {
        EnterCustomPage(mind, mind->currentNavi);
      }
      else if (mind->section == ENTRY)
      {
        NaviCustomEntryMap(mind->currentNavi, NaviCompleteCustomEntry, &mind->filepath);
      }
    }
    FormatScreen(mind);
    FormatData(mind); 
  } while ( c = KeyboardParser(keyboard) );

  mind->currentNavi = 0;
  FormatData(mind);
  CursorToTheBottom();

}