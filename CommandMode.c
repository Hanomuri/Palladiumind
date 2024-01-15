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
#include "Custom.h"

#define ESC_KEY           0x1B
#define SPACE_KEY         0x20
#define DELETE_KEY        0x7F
#define ENTER_KEY         '\n'

#define HOME              0x80
#define FUTURE_LOG        0x40
#define MONTLY            0x20
#define CUSTOM            0x10
#define ENTRY             0x8

#define COMPLETED         0x8000
#define TASK              0x0
#define EVENT             0x1000
#define RESEARCH          0x2000
#define NOTE              0x3000
#define DATE              0x4000
#define IMPORTANT         0x800
#define DISCARTED         0x400
#define HAVE_DATE         0x200


#define WHISHLIST         0x4000
#define READING           0x5000
#define BOOK_PAGE         0x1

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

static void InitCommandList(CommandList* commandList, const size_t totalSize) {
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

static void AppendChar(CommandList* commandList, char character) {
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

static void PopChar(CommandList* commandList) {
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

#pragma region EntryMethods
static void AddCommand(const CommandList* commandList) {
  CommandBlock* current = commandList->head->next;

  Entry newEntry;
  newEntry.data = 0b0000000000000000;
  newEntry.name = malloc(45*sizeof(char));
  newEntry.year = 0b00000000;
  memset(newEntry.name, 0, 45*sizeof(char));
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
    //ERROR HANDLING
    return;
  }
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
    } else if (strcmp(current->argument, "-m") == 0) {
      if (strcmp(NEXT->argument, "EVENT") == 0) {
        newEntry.data |= EVENT;
      } 
      else if (strcmp(NEXT->argument, "RESEARCH") == 0) {
        newEntry.data |= RESEARCH;
      }
      else if (strcmp(NEXT->argument, "NOTE") == 0) {
        newEntry.data |= NOTE;
      }
      else if (strcmp(NEXT->argument, "DATE") == 0) {
        newEntry.data |= DATE;
      }
    }
    current = current->next;
  }

  FILE* writeEntryFile = fopen("Resources/Entries.txt", "a");
  fprintf(writeEntryFile, "%d %d %s\n", newEntry.data, newEntry.year, newEntry.name);
  fclose(writeEntryFile);

  free(newEntry.name);
}

static void RemoveEntry(FILE* writeFile, Entry entry, int currentLine, AVLNode* deleteLineTree) {
  #define REMOVE_WRITE fprintf(writeFile, "%d %d%s\n", entry.data, entry.year, entry.name);
  if( !InAVLTree(deleteLineTree, currentLine) ) {
    REMOVE_WRITE
  }
}

static void CompleteEntry(FILE* writeFile, Entry entry, int currentLine, AVLNode* completeLineTree) {
  #define COMPLETE_WRITE fprintf(writeFile, "%d %d%s\n", entry.data, entry.year, entry.name);
  if( InAVLTree(completeLineTree, currentLine) ) {
    entry.data |= COMPLETED;
  }
  COMPLETE_WRITE
  currentLine++;
}

static void EntryMap(const CommandList* commandList, void (*EntryOperation)(FILE*, Entry, int, AVLNode*)) {
  CommandBlock* current = commandList->head->next;
  FILE *readFile, *tempFile;
  Entry entry;
  entry.name = malloc(sizeof(char)*45);
  AVLNode* changeTreeLines = NULL;
  while(current != NULL) {
    InsertToAVL(&changeTreeLines, atoi(current->argument));
    current = current->next;
  }

  #define FILENAME      "Resources/Entries.txt"
  #define TEMP_FILENAME "Resources/temp_entries.txt"
  readFile = fopen(FILENAME, "r");
  tempFile = fopen(TEMP_FILENAME, "w");

  #define ENTRY_SCAN fscanf(readFile, "%hd%hhd%[^\n]s", &entry.data, &entry.year, entry.name)
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

#pragma endregion
#pragma region CustomMethods
static FILE* OpenCustomFilepath(char* filename, __uint8_t Option) {
  char* filepath = malloc(60*sizeof(char));
  memset(filepath, 0, 60*sizeof(char));
  strcat(filepath, "Custom/");
  strcat(filepath, filename);
  strcat(filepath, ".txt");
  FILE* CustomFile;
  if (Option & 0x1) {
    CustomFile = fopen(filepath, "a");
  } else {
    CustomFile = fopen(filepath, "r");
  }
  free(filepath);
  return CustomFile;
}

static void AddCustomPage(const CommandList* commandList) {
  CommandBlock* current = commandList->head->next;

  CustomPage newCustomPage;
  newCustomPage.sectionType = 0b00000000;
  newCustomPage.name = malloc(60*sizeof(char));
  memset(newCustomPage.name, 0, 60*sizeof(char));
  //FIRST THE NAME
  while(current != NULL) {
    //OPTIONS ENTER
    if(current->argument[0] == '-') {
      break;
    }
    strcat(newCustomPage.name, current->argument);
    
    current = current->next;
    if(current != NULL && current->argument[0] != '-') {
      strcat(newCustomPage.name, " ");
    }
  }

  if(strlen(newCustomPage.name) == 0) {
    free(newCustomPage.name);
    //ERROR HANDLING
    return;
  }

  if(strcmp(current->argument, "-BOOKPAGE") == 0) {
    newCustomPage.sectionType |= BOOK_PAGE;
  }
  else {
    free(newCustomPage.name);
    //ERROR HANDLING
    return;
  }

  FILE* writeCustomSectionFile = fopen("Resources/CustomSection.txt", "a");
  fprintf(writeCustomSectionFile, "%d %s\n", newCustomPage.sectionType, newCustomPage.name);
  fclose(writeCustomSectionFile);
  
  FILE* writeCustomFile = OpenCustomFilepath(newCustomPage.name, 1);
  fprintf(writeCustomFile, "%d\n", newCustomPage.sectionType);
  fclose(writeCustomFile);

  free(newCustomPage.name);
}


static void EnterCustomPage (const CommandList* commandList, size_t enterPage, __uint8_t* type) {
  CustomPage readPage;
  readPage.name = malloc(60*sizeof(char));
  memset(readPage.name, 0, 60*sizeof(char));

  FILE* readCustomFile;
  readCustomFile = fopen("Resources/CustomSection.txt", "r");

  #define CUSTOM_SCAN fscanf(readCustomFile, "%hhd %[^\n]s", &readPage.sectionType, readPage.name)
  int currentPage = 1;
  while (CUSTOM_SCAN != EOF) {
    if(currentPage == enterPage) {
      strcpy(*(commandList->filepath), "Custom/");
      strcat(*(commandList->filepath), readPage.name);
      strcat(*(commandList->filepath), ".txt");
      *type = ENTRY;
      break;
    }
    currentPage++;
  } 

  free(readPage.name);
  fclose(readCustomFile);
}
static void AddCustomCommand(const CommandList* commandList) {
  CommandBlock* current = commandList->head->next;

  __uint8_t typePage;
  FILE* readCustomEntriesFile = fopen(*(commandList->filepath), "r");

  fscanf(readCustomEntriesFile, "%hhu", &typePage);
  fclose(readCustomEntriesFile);
  
  BookEntry newEntry;
  //IF YOU NEED VOID POINTER, ALE
  newEntry.data = 0b0000000000000000;
  newEntry.name = malloc(45*sizeof(char));
  newEntry.year = 0b00000000;
  memset(newEntry.name, 0, 45*sizeof(char));
  
  newEntry.author = malloc(45*sizeof(char));
  memset(newEntry.author, 0, 45*sizeof(char)); 
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
    if(typePage & BOOK_PAGE) {
      free(newEntry.author);
    }
    //ERROR HANDLING
    return;
  }
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
    } else if (strcmp(current->argument, "-m") == 0 && typePage & BOOK_PAGE) {
      if (strcmp(NEXT->argument, "EVENT") == 0) {
        newEntry.data |= EVENT;
      } 
      else if (strcmp(NEXT->argument, "RESEARCH") == 0) {
        newEntry.data |= RESEARCH;
      }
      else if (strcmp(NEXT->argument, "NOTE") == 0) {
        newEntry.data |= NOTE;
      }
      else if (strcmp(NEXT->argument, "WISHLIST") == 0) {
        newEntry.data |= WHISHLIST;
      }
      else if (strcmp(NEXT->argument, "READING") == 0) {
        newEntry.data |= READING;
      }
    }
    else if (strcmp(current->argument, "-a") == 0 && typePage & BOOK_PAGE) {
      current = current->next;
      while(current != NULL) {
        //OPTIONS ENTER
        if(current->argument[0] == '-') {
          break;
        }
        strcat(newEntry.author, current->argument);
        
        current = current->next;
        if(current != NULL && current->argument[0] != '-') {
          strcat(newEntry.author, " ");
        }
      }
    }
    if(current != NULL) {
      current = current->next;
    }
  }
  if(typePage & BOOK_PAGE) {
    if(strlen(newEntry.author) == 0) {
      free(newEntry.name);
      free(newEntry.author);
      //ERROR HANDLING
      return;
    }
  }

  FILE* writeCustomFile = fopen(*(commandList->filepath), "a");
  fprintf(writeCustomFile, "%d %d %s\n", newEntry.data, newEntry.year, newEntry.name);
  if(typePage & BOOK_PAGE) {
    fprintf(writeCustomFile, "%s\n", newEntry.author);
  }
  fclose(writeCustomFile);
}
#pragma endregion
static void CallCommand(const CommandList* commandList, __uint8_t* type) {
  CommandBlock* commandHead = commandList->head;
  
  if(*type & HOME) {
    if (strcmp(commandHead->argument, "CUSTOM") == 0) {
      (*type) = (*type)>>3;
    }
    else if (strcmp(commandHead->argument, "ADD") == 0) {
      AddCommand(commandList);
    } 
    else if (strcmp(commandHead->argument, "REMOVE") == 0) {
      EntryMap(commandList, RemoveEntry);
    } 
    else if (strcmp(commandHead->argument, "COMPLETE") == 0) {
      EntryMap(commandList, CompleteEntry);
    }
  }
  else if (*type & ENTRY) {
    if (strcmp(commandHead->argument, "ADD") == 0) {
      printf("SEXO");
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
    }
    else if (strcmp(commandHead->argument, "ADD") == 0) {
      AddCustomPage(commandList);
    }
    else if (strcmp(commandHead->argument, "ENTER") == 0) {
      EnterCustomPage(commandList, atoi(commandHead->next->argument), type);
    }
  }
}

void CommandMode(__uint8_t* section, char* filepath) {
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
  printf("\033[%ldD\33[K", commandList.currSize+1);
  DeleteCommandList(&commandList);
}