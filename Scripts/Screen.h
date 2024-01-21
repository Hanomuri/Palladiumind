
#ifndef _PALLADIUM_MIND
  #define _PALLADIUM_MIND
  #include "Mind.h"
#endif

void CursorToTheBottom();
unsigned char GetTerminalWidth();
void PrintCurrentTime(unsigned char width);
void FormatData(const Mind* mind);
void FormatScreen(const Mind* mind);
void* DisplayCheck(void* args);