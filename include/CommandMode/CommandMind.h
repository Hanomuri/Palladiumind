
#ifndef _COMMAND_MIND
  #define _COMMAND_MIND
  #ifndef _STRING_H
    #define _STRING_H
    #include <string.h>
  #endif
  #ifndef _STDIO
    #define _STDIO
    #include <stdio.h>
  #endif
  #ifndef _STDLIB
    #define _STDLIB
    #include <stdlib.h>
  #endif
  #ifndef _PALLADIUM_MIND
    #define _PALLADIUM_MIND
    #include "../Mind.h"
  #endif
  #ifndef _COMMAND_MODE
    #define _COMMAND_MODE
    #include "../CommandMode.h"
  #endif
  #ifndef _AVL
    #define _AVL
    #include "../DataStructures/AVLTree.h"
  #endif
  #ifndef _COMMAND_ARENA
    #define _COMMAND_ARENA
    #include "CommandArena.h"
  #endif
  #ifndef _COMMAND_ENTRY
    #define _COMMAND_ENTRY
    #include "CommandEntry.h"
  #endif
  #ifndef _COMMAND_CUSTOM_ENTRY
    #define _COMMAND_CUSTOM_ENTRY
    #include "CommandCustomEntry.h"
  #endif
  #ifndef _PALLADIUM_KEYS
    #define _PALLADIUM_KEYS
    #define KEY_SPACE         0x20
    #define KEY_DELETE        0x7F
    #define KEY_ENTER         '\n'
  #endif
#endif

static void InitCommandList(CommandList* commandList, const size_t totalSize) {
  commandList->totalSize  = totalSize;
  commandList->currSize   = 0;
  CommandBlock* command   = (CommandBlock*)malloc(sizeof(struct CommandBlock));
  commandList->masterMind = (unsigned char*)malloc(sizeof(char)*totalSize*2);
  memset(commandList->masterMind, 0, totalSize*2);
  CommandArenaInit(&commandList->arena, commandList->masterMind);
  command->argument       = (char*)CommandArenaAlloc(&commandList->arena); 
  command->next           = NULL;
  command->prev           = NULL;
  commandList->head       = command;
  commandList->tail       = command;
}

static void DeleteCommandList(CommandList* commandList) {
  #define TAIL_ARGUMENT commandList->tail->argument
  free(commandList->masterMind);
  while(commandList->head != NULL) {
    if(commandList->tail == commandList->head) {
      commandList->head = NULL;
      free(commandList->tail);
    } else {
      commandList->tail = commandList->tail->prev;
      free(commandList->tail->next);
      commandList->tail->next = NULL;
    }
  }
}

static void AppendChar(CommandList* commandList, short character) {
  if(commandList->currSize == commandList->totalSize) {
    return;
  }
  commandList->currSize++;

  printf("%c", (char)character);
  fflush(stdout);

  if(character != KEY_SPACE) {
    #define TAIL_ARGUMENT commandList->tail->argument
    TAIL_ARGUMENT[strlen(TAIL_ARGUMENT)] = (char)character;
    commandList->arena.arenaBuffer++;
    return;
  }

  #define NEW_COMMAND_SIZE commandList->totalSize-commandList->currSize  
  CommandBlock* commandArg  = (CommandBlock*)malloc(sizeof(struct CommandBlock));
  commandArg->argument      = (char*)CommandArenaAlloc(&commandList->arena); 
  commandArg->next          = NULL;
  commandArg->prev          = commandList->tail;
  commandList->tail->next   = commandArg;
  commandList->tail         = commandList->tail->next;
}

static void PopChar(CommandList* commandList) {
  if(commandList->currSize == 0) {
    return;
  }
  commandList->arena.arenaBuffer--;
  commandList->currSize--;
  printf("\033[1D \033[1D");
  fflush(stdout);

  #define TAIL_ARGUMENT commandList->tail->argument
  if(strlen(TAIL_ARGUMENT) != 0) {
    TAIL_ARGUMENT[strlen(TAIL_ARGUMENT)-1] = 0;
    return;
  }

  commandList->arena.arenaBuffer[commandList->arena.arenaOffset] = 0;
  commandList->tail = commandList->tail->prev;
  free(commandList->tail->next);
  commandList->tail->next = NULL;
}

static void PrintCommandList(const CommandList* commandList) {
  CommandBlock* current = commandList->head;

  FILE* commandFile = fopen("Resources/CMDList.txt", "w");

  while(current != NULL) {
    fprintf(commandFile, "%s %ld\n", current->argument, strlen(current->argument));
    current = current->next;
  }

  fflush(stdout);
  fclose(commandFile);
}