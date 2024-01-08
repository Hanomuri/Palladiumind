
#include <stddef.h>


typedef struct CommandArena {
  unsigned char*    arenaBuffer;
  size_t            buffLength;
  size_t            arenaOffset;    
}CommandArena;

typedef struct CommandBlock {
  char*                    argument;
  struct CommandBlock*     next;
  struct CommandBlock*     prev;
} CommandBlock;

typedef struct CommandList {
  unsigned char*    masterMind;         //CONTAIN ALL CHAR
  size_t            totalSize;
  size_t            currSize;
  CommandArena      arena;
  CommandBlock*     head;
  CommandBlock*     tail;
} CommandList;

void CommandMode();