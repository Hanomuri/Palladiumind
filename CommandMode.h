
#include <stddef.h>

typedef struct CommandBlock {
  char*                    argument;
  struct CommandBlock*     next;
  struct CommandBlock*     prev;
} CommandBlock;

typedef struct CommandList {
  size_t            totalSize;
  size_t            currSize;
  CommandBlock*     head;
  CommandBlock*     tail;
} CommandList;

void InitCommandList(CommandList* commandList, const size_t totalSize);
void AppendChar(CommandList* commandList, char character);
void PopChar(CommandList* commandList);
void CommandMode();