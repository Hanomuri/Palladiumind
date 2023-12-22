#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include "CommandMode.h"

#define ESC_KEY         0x1B
#define SPACE_KEY       0x20
#define DELETE_KEY      0x7F
#define ENTER_KEY       '\n'

void InitCommandList(CommandList* commandList, const size_t totalSize) {
  commandList->totalSize = totalSize;
  commandList->currSize = 0;
  CommandBlock* command = malloc(sizeof(struct CommandBlock));
  command->argument  = malloc(sizeof(char)*totalSize);
  memset(command->argument, 0, totalSize);
  command->next      = NULL;
  command->prev      = NULL;
  commandList->head  = command;
  commandList->tail  = command;
}

void AppendChar(CommandList* commandList, char character) {
  if(commandList->currSize == commandList->totalSize) {
    return;
  }

  printf("%c", character);

  if(character != SPACE_KEY) {
    #define TAIL_ARGUMENT commandList->tail->argument
    TAIL_ARGUMENT[strlen(TAIL_ARGUMENT)] = character;   
    commandList->currSize++; 
    return;
  }


  #define NEW_COMMAND_SIZE commandList->totalSize-commandList->currSize  
  CommandBlock* commandArg = malloc(sizeof(struct CommandBlock));
  commandArg->argument = malloc(sizeof(char)*(NEW_COMMAND_SIZE));
  memset(commandArg->argument, 0, NEW_COMMAND_SIZE);
  commandArg->next = NULL;
  commandArg->prev = commandList->tail;
  commandList->tail->next = commandArg;
  commandList->tail = commandList->tail->next;
}

void PopChar(CommandList* commandList) {
  if(commandList->currSize == 0) {
    return;
  }
  
  printf("\033[1D \033[1D");

  #define TAIL_ARGUMENT commandList->tail->argument
  if(strlen(TAIL_ARGUMENT) != 0) {
    TAIL_ARGUMENT[strlen(TAIL_ARGUMENT)-1] = 0;
    commandList->currSize--;
    return;
  }

  free(TAIL_ARGUMENT);
  commandList->tail = commandList->tail->prev;
  free(commandList->tail->next);
  commandList->tail->next = NULL;
}

//DEBUGGING STUFFS1
void PrintCommandList(CommandList* commandList) {
  CommandBlock* current = commandList->head;

  FILE* commandFile = fopen("CMDList.txt", "w");

  while(current != NULL) {
    fprintf(commandFile, "%s %d\n", current->argument, strlen(current->argument));
    current = current->next;
  }

  fflush(stdout);
  fclose(commandFile);
}

void CommandMode() {
  CommandList commandList;
  InitCommandList(&commandList, 127);

  char c;

  while(1){
    c = getc(stdin);
    
    if(c == ESC_KEY) {
      break;
    } 
    else if(c == ENTER_KEY) {
      PrintCommandList(&commandList);
      break;
    } 
    else if (c == DELETE_KEY) {
      PopChar(&commandList);
    } 
    else {
    AppendChar(&commandList, c);
    }
  }
}