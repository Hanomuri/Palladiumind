#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include "CommandMode/CommandMind.h"
#include "Screen.h"


static void CallCommand(const CommandList* commandList, unsigned char* type) {
  CommandBlock* commandHead = commandList->head;
  
  if(*type & HOME) {
    if (strcmp(commandHead->argument, "CUSTOM") == 0) {
      (*type) = (*type)>>3;
      FormatScreen((*type));
    }
    else if (strcmp(commandHead->argument, "ADD") == 0) {
      AddEntryCommand(commandList);
    } 
    else if (strcmp(commandHead->argument, "REMOVE") == 0) {
      EntryMap(commandList, RemoveEntry);
    } 
    else if (strcmp(commandHead->argument, "COMPLETE") == 0) {
      EntryMap(commandList, CompleteEntry);
    }
    else if (strcmp(commandHead->argument, "DISCARD") == 0) {
      EntryMap(commandList, DiscardEntry);
    }
  }
  else if (*type & ENTRY) {
    if (strcmp(commandHead->argument, "ADD") == 0) {
      AddCustomCommand(commandList);
    } 
    else if (strcmp(commandHead->argument, "REMOVE") == 0) {
      EntryMap(commandList, RemoveEntry);
    } 
    else if (strcmp(commandHead->argument, "COMPLETE") == 0) {
      EntryMap(commandList, CompleteEntry);
    }
  } 
  else if (*type & FUTURE_LOG) {

  }
  else if (*type & MONTLY) {

  }
  else if (*type & CUSTOM) {
    if (strcmp(commandHead->argument, "HOME") == 0 || strcmp(commandHead->argument, "GOHO-M") == 0) {
      (*type) = (*type)<<3;
      FormatScreen((*type));
    }
    else if (strcmp(commandHead->argument, "ADD") == 0) {
      AddCustomPage(commandList);
    }
    else if (strcmp(commandHead->argument, "ENTER") == 0 && (commandHead->next != NULL)) {
      EnterCustomPage(commandList, atoi(commandHead->next->argument), type);
    }
  }
}

void CommandMode(unsigned char* section, char* filepath) {
  CommandList commandList;
  InitCommandList(&commandList, 127);
  commandList.filepath = &filepath;

  char c;

  while(1){
    c = getc(stdin);
    
    if(c == ESC_KEY) {
      break;
    } 
    else if(c == ENTER_KEY) {
      CallCommand(&commandList, section);
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
  CursorToTheBottom();
  DeleteCommandList(&commandList);
}