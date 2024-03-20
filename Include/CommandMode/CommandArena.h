#ifndef _COMMAND_MODE
    #define _COMMAND_MODE
    #include "../CommandMode.h"
#endif

static void* CommandArenaAlloc(CommandArena* arena) {
  if(arena->arenaOffset >= arena->buffLength-1) {
    return NULL;
  }
  arena->arenaBuffer[arena->arenaOffset++] = '\0';
  void* ptr = &arena->arenaBuffer[arena->arenaOffset];
  return ptr; 
}

static void CommandArenaInit(CommandArena* arena, void *backingBuffer) {
  arena->arenaBuffer = (unsigned char*) backingBuffer;
  arena->arenaOffset = 0;
}