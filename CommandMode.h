
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
  char**            filepath;
  unsigned char*    masterMind;         //CONTAIN ALL CHAR
  size_t            totalSize;
  size_t            currSize;
  CommandArena      arena;
  CommandBlock*     head;
  CommandBlock*     tail;
} CommandList;

void CommandMode(__uint8_t* section, char* filepath);