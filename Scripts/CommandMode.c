#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include "CommandMode/CommandMind.h"
#include "Screen.h"

#define SHOW_CURSOR printf("\e[?25h")
#define INVISIBLE_CURSOR printf("\e[?25l")

static void CallCommand(const CommandList* commandList, Mind* mind) {
  CommandBlock* commandHead = commandList->head;
  
  if(mind->section & HOME) {
    if (strcmp(commandHead->argument, "CUSTOM") == 0) {
      (mind->section) = CUSTOM;
      FormatScreen((mind));
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
    FormatData(mind);
  }
  else if (mind->section & ENTRY) {
    if (strcmp(commandHead->argument, "ADD") == 0) {
      AddCustomCommand(commandList, &mind->filepath);
    } 
    else if (strcmp(commandHead->argument, "REMOVE") == 0) {
      CustomEntryMap(commandList, RemoveCustomEntry, &mind->filepath);
    } 
    else if (strcmp(commandHead->argument, "COMPLETE") == 0) {
      CustomEntryMap(commandList, CompleteCustomEntry, &mind->filepath);
    }
    else if (strcmp(commandHead->argument, "READING") == 0) {
      CustomEntryMap(commandList, ReadingCustomEntry, &mind->filepath);
    }
    else if (strcmp(commandHead->argument, "TO_READ") == 0 || strcmp(commandHead->argument, "TOREAD") == 0) {
      CustomEntryMap(commandList, ClearCustomEntry, &mind->filepath);
    }
    else if (strcmp(commandHead->argument, "DISPLAY") == 0) {
      if(commandHead->next == NULL){
        return;
      } else if (strcmp(commandHead->next->argument, "BOARD") == 0) {
        mind->section = BOARD;
      }
    }
    FormatData(mind);
  } 
  else if (mind->section & MARKED) {

  }
  else if (mind->section & CUSTOM) {
    if (strcmp(commandHead->argument, "HOME") == 0 || strcmp(commandHead->argument, "GOHO-M") == 0) {
      (mind->section) = HOME;
      FormatScreen((mind));
    }
    else if (strcmp(commandHead->argument, "ADD") == 0) {
      AddCustomPage(commandList);
    }
    else if (strcmp(commandHead->argument, "ENTER") == 0 && (commandHead->next != NULL)) {
      EnterCustomPage(commandList, atoi(commandHead->next->argument), mind);
    }
    FormatData(mind);
  }
  else if (mind->section & CONFIG) {

  }
}

void CommandMode(Mind* mind) {
  CommandList commandList;
  InitCommandList(&commandList, 127);

  mind->heightInCommand = 1;
  mind->currentCollum = &commandList.currSize;

  char c;
  while(1){
    c = getc(stdin);
    
    if(c == ESC_KEY) {
      break;
    } 
    else if(c == ENTER_KEY) {
      CallCommand(&commandList, mind);
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

  mind->currentCollum = NULL;
  CursorToTheBottom();
  DeleteCommandList(&commandList);
}