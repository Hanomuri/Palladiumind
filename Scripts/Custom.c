#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 
#include <string.h>
#include "DataStructures/BookLinkedList.h"
#include "Custom.h"
#include "Screen.h"

#define BOOK_PAGE 0x1

#define TO_READ           0x0
#define EVENT_MARK        1
#define RESEARCH_MARK     2
#define NOTE_MARK         3
#define WISHLIST_MARK    4
#define READING_MARK      5

#define BIT_VALUE(X, Y) (((X) >> (Y)) & 0x1)
#define u_short unsigned char

static void PrintCustomSection(const struct CustomPage* customPage) {
  if(customPage->sectionType & BOOK_PAGE) {
    printf("🕮 ");
  } else {
    printf("☑ ");
  }
  printf("%s\n", customPage->name);
}

static void PrintScore(unsigned char number) {
  float actualNumber = number>>1;
  if (BIT_VALUE(number, 0)) {
    actualNumber += 0.5f;
  }
  if (!BIT_VALUE(number, 0) && number != 20) {
    printf("    %.0f / 10♡", actualNumber);
  } 
  else if (!BIT_VALUE(number, 0)) {
    printf("   %.0f / 10♡", actualNumber);
  } 
  else {
    printf("  %.1f / 10♡", actualNumber);
  }
}

static void PrintBookPageEntry(const struct BookEntry* bookEntry, const u_short type) {
  unsigned char markBuffer = 0;
  for(unsigned char k = 12; k < 15; k++) {
    if(BIT_VALUE(bookEntry->data, k)) markBuffer += pow(2, k-12);
  }
  if (bookEntry->data & DISCARDED) {
    //TWENTY ONEEE
  }
  else if(type & BOOK_PAGE) {
    if (markBuffer == READING_MARK) {
      printf("➳"); //⛨
    } 
    else if (markBuffer == WISHLIST_MARK) {
      printf("★");
    } 
    else if (markBuffer == NOTE_MARK) {
      printf("─");
    } 
    else if (markBuffer == RESEARCH_MARK) {
      printf("⌕");
    } 
    else if (markBuffer == EVENT_MARK) {
      printf("◯");
    } 
    else if(bookEntry->data & COMPLETED) {
      printf("⛝");
    } 
    else {
      printf("□");
    }
  }

  if(bookEntry->data & DISCARDED) {
    printf(TEXT_STRIKE);
  } 
  else {
    printf(" ");
  }
  printf("%s - %s", bookEntry->name, bookEntry->author);
  printf(ATTR_OFF);
  
  if(bookEntry->data & HAVE_DATE) {
    PrintDateEntry(bookEntry->data, bookEntry->year);
  }
  if (bookEntry->data & IMPORTANT) {
    printf("!");
  }

  if(bookEntry->score > 0) {
    PrintScore(bookEntry->score);
  }

  printf("\n");
}

void ReadCustomData() {
  FILE* readCustomFile = fopen("Resources/CustomSection.txt", "r");

  if(readCustomFile == NULL) {
    printf("-EMPTY-\n");
    return;
  }

  CustomPage customPage = GenCustomPage();

  int currentPage = 0;

  #define CUSTOM_PAGE_SCAN fscanf(readCustomFile, "%hhd%[^\n]s", &customPage.sectionType, customPage.name)
  for(; CUSTOM_PAGE_SCAN != EOF;) {
    printf("%d ", ++currentPage);
    PrintCustomSection(&customPage);
  }
  
  if (currentPage == 0) {
    printf("-EMPTY-\n");
  }

  fclose(readCustomFile);
  free(customPage.name);
}

void ReadCustomPage(Mind* mind) {
  FILE* readCustomEntriesFile = fopen(mind->filepath, "r"); 
  unsigned char type;
  fscanf(readCustomEntriesFile, "%hhd\n", &type);
  
  int currentEntry = 0;

  if(type & BOOK_PAGE) {
    BookEntry bookEntry = GenBookEntry();
    #define CUSTOM_ENTRY_SCAN fscanf(readCustomEntriesFile, "%hd%hhd%hhd %[^\n]s", &bookEntry.data, &bookEntry.year, &bookEntry.score, bookEntry.name)
    for(; CUSTOM_ENTRY_SCAN != EOF;) {
      fgetc(readCustomEntriesFile);
      fscanf(readCustomEntriesFile, "%[^\n]s", bookEntry.author);
      printf("%d ", ++currentEntry);
      PrintBookPageEntry(&bookEntry, type);
    }
    free(bookEntry.name);
    free(bookEntry.author);
  } else {
    Entry entry = GenEntry();
    #define ENTRY_SCAN fscanf(readCustomEntriesFile, "%hd%hhd%hhd %[^\n]s", &entry.data, &entry.year, &entry.group, entry.name)
    for(; ENTRY_SCAN != EOF;) {
      printf("%d ", ++currentEntry);
      PrintEntry(&entry);
    }
    free(entry.name);
  }

  if(currentEntry == 0) {
    printf("-EMPTY-\n");
  }

  mind->maximumNavi = currentEntry;

  fclose(readCustomEntriesFile);
}

void DisplayBoard(const char* filepath) {
  FILE* readBookFile = fopen(filepath, "r"); 
  unsigned char type;
  fscanf(readBookFile, "%hhd\n", &type);

  unsigned char TerminalWidth = GetTerminalWidth();
  if (TerminalWidth < 99) {
    return;
  }
  TerminalWidth /= 3;
  BoardList completedList;
  BoardList doingList;
  BoardList toDoList;
  InitBoardList(&completedList);
  InitBoardList(&doingList);
  InitBoardList(&toDoList);
  unsigned char markBuffer = 0;
  BookEntry entry = GenBookEntry();
  #define BOARD_SCAN fscanf(readBookFile, "%hd%hhd%hhd %[^\n]s", &entry.data, &entry.year, &entry.score, entry.name)
  for(int currentEntry = 1; BOARD_SCAN != EOF; currentEntry++) {
    if (type == BOOK_PAGE) {
      fgetc(readBookFile);
      fscanf(readBookFile, "%[^\n]s", entry.author);
    }
    for(unsigned char k = 12; k < 15; k++) {
      if(BIT_VALUE(entry.data, k)) markBuffer += pow(2, k-12);
    }
    
    //TO DO WITH OTHERS
    if (markBuffer == READING_MARK) {
      InsertBoardList(&doingList, entry, currentEntry);
    }
    else if (entry.data & COMPLETED && markBuffer == 0) {
      InsertBoardList(&completedList, entry, currentEntry);
    } 
    else if (markBuffer == 0){
      InsertBoardList(&toDoList, entry, currentEntry);
    }
    markBuffer = 0;
  }

  printf("\033[3;0H\33[J");
  
  if (type == BOOK_PAGE) {
    printf("Readed");
    for(int k = strlen("Readed"); k < TerminalWidth; k++){
      printf(" ");
    }
    printf("Reading");
    for(int k = strlen("Reading"); k < TerminalWidth; k++){
      printf(" ");
    }
    printf("To Read\n");
  }
  else {
    printf("Completed");
    for(int k = strlen("Completed"); k < TerminalWidth; k++){
      printf(" ");
    }
    printf("Doing");
    for(int k = strlen("Doing"); k < TerminalWidth; k++){
      printf(" ");
    }
    printf("To Do\n");
  }

  BoardList* boardlist[3] = {&completedList, &doingList, & toDoList};
  #define NUMBER_ENTRY boardlist[currentList]->head->numberEntry
  while (!IsEmpty(&completedList) || !IsEmpty(&doingList) || !IsEmpty(&toDoList)) {
    for (int currentList = 0; currentList < 3; currentList++) {
      if(!IsEmpty(boardlist[currentList])) {
        printf("%hd %s", NUMBER_ENTRY, boardlist[currentList]->head->entry.name);
        for(int k = strlen(boardlist[currentList]->head->entry.name)+((NUMBER_ENTRY/10)+2); k < TerminalWidth; k++){
          printf(" ");
        }
        RemoveBoardListHead(boardlist[currentList]);
      }
      else {
        for(int k = 0; k < TerminalWidth; k++){
          printf(" ");
        }
      }
    }
    printf("\n");
  }
  
  free(entry.name);
  free(entry.author);
  fclose(readBookFile);
}

BookEntry GenBookEntry() {
  BookEntry newEntry;
  newEntry.data   = 0b0000000000000000;
  newEntry.year   = 0b00000000;
  newEntry.score  = 0b00000000;
  newEntry.name   = (char*)malloc(30*sizeof(char));
  memset(newEntry.name, 0, 30*sizeof(char));
  newEntry.author = (char*)malloc(30*sizeof(char));
  memset(newEntry.author, 0, 30*sizeof(char));
  return newEntry;
}

CustomPage GenCustomPage() {
  CustomPage newCustomPage;
  newCustomPage.sectionType = 0b00000000;
  newCustomPage.name = (char*)malloc(60*sizeof(char));
  memset(newCustomPage.name, 0, 60*sizeof(char));
  return newCustomPage;
}