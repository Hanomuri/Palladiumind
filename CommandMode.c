#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include "CommandMode.h"
#include "Mind.h"
#include "AVLTree.h"

#define ESC_KEY         0x1B
#define SPACE_KEY       0x20
#define DELETE_KEY      0x7F
#define ENTER_KEY       '\n'

#define COMPLETED       0x8000
#define IMPORTANT       0x800
#define MIGRATION       0x400
#define HAVE_DATE       0x200

void* CommandArenaAlloc(CommandArena* arena) {
  if(arena->arenaOffset >= arena->buffLength-1) {
    return NULL;
  }
  arena->arenaBuffer[arena->arenaOffset++] = '\0';
  void* ptr = &arena->arenaBuffer[arena->arenaOffset];
  return ptr; 
}

void CommandArenaInit(CommandArena* arena, void *backingBuffer) {
  arena->arenaBuffer = (unsigned char*) backingBuffer;
  arena->arenaOffset = 0;
}

void InitCommandList(CommandList* commandList, const size_t totalSize) {
  commandList->totalSize  = totalSize;
  commandList->currSize   = 0;
  CommandBlock* command   = malloc(sizeof(struct CommandBlock));
  commandList->masterMind = malloc(sizeof(char)*totalSize*2);
  memset(commandList->masterMind, 0, totalSize*2);
  CommandArenaInit(&commandList->arena, commandList->masterMind);
  command->argument       = CommandArenaAlloc(&commandList->arena); 
  command->next           = NULL;
  command->prev           = NULL;
  commandList->head       = command;
  commandList->tail       = command;
}

void DeleteCommandList(CommandList* commandList) {
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

void AppendChar(CommandList* commandList, char character) {
  if(commandList->currSize == commandList->totalSize) {
    return;
  }
  commandList->currSize++;

  printf("%c", character);

  if(character != SPACE_KEY) {
    #define TAIL_ARGUMENT commandList->tail->argument
    TAIL_ARGUMENT[strlen(TAIL_ARGUMENT)] = character;
    commandList->arena.arenaBuffer++;
    return;
  }

  #define NEW_COMMAND_SIZE commandList->totalSize-commandList->currSize  
  CommandBlock* commandArg  = malloc(sizeof(struct CommandBlock));
  commandArg->argument      = CommandArenaAlloc(&commandList->arena); 
  commandArg->next          = NULL;
  commandArg->prev          = commandList->tail;
  commandList->tail->next   = commandArg;
  commandList->tail         = commandList->tail->next;
}

void PopChar(CommandList* commandList) {
  if(commandList->currSize == 0) {
    return;
  }
  commandList->arena.arenaBuffer--;
  commandList->currSize--;
  printf("\033[1D \033[1D");

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

//DEBUGGING STUFFS1
void PrintCommandList(const CommandList* commandList) {
  CommandBlock* current = commandList->head;

  FILE* commandFile = fopen("CMDList.txt", "w");

  while(current != NULL) {
    fprintf(commandFile, "%s %d\n", current->argument, strlen(current->argument));
    current = current->next;
  }

  fflush(stdout);
  fclose(commandFile);
}

void AddCommand(const CommandList* commandList) {
  CommandBlock* current = commandList->head->next;

  Entry newEntry;
  newEntry.data = 0b0000000000000000;
  newEntry.name = malloc(sizeof(char)*45);
  newEntry.year = 0b00000000;
  memset(newEntry.name, 0, 45);
  //FIRST THE NAME
  while(current != NULL) {
    //OPTIONS ENTER
    if(current->argument[0] == '-') {
      break;
    }
    strcat(newEntry.name, current->argument);
    
    current = current->next;
    if(current != NULL && current->argument[0] != '-') {
      strcat(newEntry.name, " ");
    }
  }
  if(strlen(newEntry.name) == 0) {
    free(newEntry.name);
    return;
  }
  int c;
  while(current != NULL) {
    #define NEXT current->next
    //MIGATION IS ANOTHER OPTION
    if(strcmp(current->argument, "-c") == 0) {
      newEntry.data |= COMPLETED;
    } else if (strcmp(current->argument, "-i") == 0) {
      newEntry.data |= IMPORTANT;
    } else if (strcmp(current->argument, "-d") == 0) {
      newEntry.data |= HAVE_DATE;
      newEntry.data |= (atoi(NEXT->argument) << 5);
      newEntry.data |= (atoi(NEXT->next->argument));
      newEntry.year |= (atoi(NEXT->next->next->argument) -2000);
    }
    current = current->next;
  }

  FILE* writeEntryFile = fopen("entries.txt", "a");
  fprintf(writeEntryFile, "%d %d %s\n", newEntry.data, newEntry.year, newEntry.name);
  fclose(writeEntryFile);

  free(newEntry.name);
}

void RemoveEntry(FILE* writeFile, Entry entry, int currentLine, AVLNode* deleteLineTree) {
  #define REMOVE_WRITE fprintf(writeFile, "%d %d%s\n", entry.data, entry.year, entry.name);
  if( !InAVLTree(deleteLineTree, currentLine) ) {
    REMOVE_WRITE
  }
}

void CompleteEntry(FILE* writeFile, Entry entry, int currentLine, AVLNode* completeLineTree) {
  #define COMPLETE_WRITE fprintf(writeFile, "%d %d%s\n", entry.data, entry.year, entry.name);
  if( InAVLTree(completeLineTree, currentLine) ) {
    entry.data |= COMPLETED;
  }
  COMPLETE_WRITE
  currentLine++;
}

void EntryMap(const CommandList* commandList, void (*EntryOperation)(FILE*, Entry, int, AVLNode*)) {
  CommandBlock* current = commandList->head->next;
  FILE *readFile, *tempFile;
  Entry entry;
  entry.name = malloc(sizeof(char)*45);
  AVLNode* changeTreeLines = NULL;
  while(current != NULL) {
    InsertToAVL(&changeTreeLines, atoi(current->argument));
    current = current->next;
  }

  #define FILENAME      "entries.txt"
  #define TEMP_FILENAME "temp_entries.txt"
  readFile = fopen(FILENAME, "r");
  tempFile = fopen(TEMP_FILENAME, "w");

  #define ENTRY_SCAN fscanf(readFile, "%d%d%[^\n]s", &entry.data, &entry.year, entry.name)
  int currentLine = 1;
  while (ENTRY_SCAN != EOF) {
    EntryOperation(tempFile, entry, currentLine, changeTreeLines);
    currentLine++;
  } 
  
  fclose(readFile);
  fclose(tempFile);
  remove(FILENAME);
  rename(TEMP_FILENAME, FILENAME);
  DeleteAVLTree(changeTreeLines);
  free(entry.name);
}

void CallCommand(const CommandList* commandList) {
  CommandBlock* commandHead = commandList->head;
  
  if (strcmp(commandHead->argument, "ADD") == 0) {
    AddCommand(commandList);
  } else if(strcmp(commandHead->argument, "REMOVE") == 0) {
    EntryMap(commandList, RemoveEntry);
  } else if(strcmp(commandHead->argument, "COMPLETE") == 0) {
    EntryMap(commandList, CompleteEntry);
  }

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
      CallCommand(&commandList);
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
  printf("\033[%dD\33[K", commandList.currSize+1);
  DeleteCommandList(&commandList);
}