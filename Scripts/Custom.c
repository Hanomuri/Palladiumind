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
#define WHISHLIST_MARK    4
#define READING_MARK      5

#define BIT_VALUE(X, Y) (((X) >> (Y)) & 0x1)
#define u_short unsigned char

static void PrintCustomSection(const struct CustomPage* customPage) {
  if(customPage->sectionType & BOOK_PAGE) {
    printf("🕮");
  } else {
    printf("☑");
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
  if(type & BOOK_PAGE) {
    if (markBuffer == READING_MARK) {
      printf("➳"); //⛨
    } 
    else if (markBuffer == WHISHLIST_MARK) {
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

  printf(" ");
  if(bookEntry->data & DISCARDED) {
    printf(TEXT_STRIKE);
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

  #define CUSTOM_PAGE_SCAN fscanf(readCustomFile, "%hhd%[^\n]s", &customPage.sectionType, customPage.name)
  for(int currentPage = 1; CUSTOM_PAGE_SCAN != EOF; currentPage++) {
    printf("%d ", currentPage);
    PrintCustomSection(&customPage);
  }
  
  fclose(readCustomFile);
  free(customPage.name);
}

void ReadCustomPage(const char* filepath) {
  FILE* readCustomEntriesFile = fopen(filepath, "r"); 
  u_short type;
  fscanf(readCustomEntriesFile, "%hhd\n", &type);

  if(type & BOOK_PAGE) {
    BookEntry bookEntry = GenBookEntry();
    #define CUSTOM_ENTRY_SCAN fscanf(readCustomEntriesFile, "%hd%hhd%hhd %[^\n]s", &bookEntry.data, &bookEntry.year, &bookEntry.score, bookEntry.name)
    for(int currentEntry = 1; CUSTOM_ENTRY_SCAN != EOF; currentEntry++) {
      fgetc(readCustomEntriesFile);
      fscanf(readCustomEntriesFile, "%[^\n]s", bookEntry.author);
      PrintBookPageEntry(&bookEntry, type);
    }
    free(bookEntry.name);
    free(bookEntry.author);
  } else {
    Entry entry = GenEntry();
    #define ENTRY_SCAN fscanf(readCustomEntriesFile, "%hd%hhd%hhd %[^\n]s", &entry.data, &entry.year, &entry.group, entry.name)
    for(int currentEntry = 1; ENTRY_SCAN != EOF; currentEntry++) {
      printf("%d ", currentEntry);
      PrintEntry(&entry);
    }
    free(entry.name);
  }

  fclose(readCustomEntriesFile);
}

void BookDisplayBoard(const char* filepath) {
  FILE* readBookFile = fopen(filepath, "r"); 
  unsigned char type;
  fscanf(readBookFile, "%hhd\n", &type);

  if(!(type & BOOK_PAGE)) {
    return;
  }
  unsigned char TerminalWidth = GetTerminalWidth();
  if (TerminalWidth < 93) {
    return;
  }
  TerminalWidth /= 3;
  BookList completedBooks;
  BookList readingBooks;
  BookList toReadBooks;
  InitBookList(&completedBooks);
  InitBookList(&readingBooks);
  InitBookList(&toReadBooks);
  unsigned char markBuffer = 0;
  BookEntry bookEntry = GenBookEntry();
  #define BOOK_SCAN fscanf(readBookFile, "%hd%hhd%hhd %[^\n]s", &bookEntry.data, &bookEntry.year, &bookEntry.score, bookEntry.name)
  for(int currentEntry = 1; BOOK_SCAN != EOF; currentEntry++) {
    fgetc(readBookFile);
    fscanf(readBookFile, "%[^\n]s", bookEntry.author);
    for(unsigned char k = 12; k < 15; k++) {
      if(BIT_VALUE(bookEntry.data, k)) markBuffer += pow(2, k-12);
    }
    
    if (markBuffer == READING_MARK) {
      InsertBookList(&readingBooks, bookEntry);
    }
    else if (bookEntry.data & COMPLETED && markBuffer == 0) {
      InsertBookList(&completedBooks, bookEntry);
    } 
    else if (markBuffer == 0){
      InsertBookList(&toReadBooks, bookEntry);
    }
    markBuffer = 0;
  }

  printf("\033[3;0H\33[J");

  printf("Completed");
  for(int k = strlen("completed"); k < TerminalWidth; k++){
    printf(" ");
  }
  printf("Reading");
  for(int k = strlen("Reading"); k < TerminalWidth; k++){
    printf(" ");
  }
  printf("To Read\n");
  while (!IsEmpty(&completedBooks) || !IsEmpty(&readingBooks) || !IsEmpty(&toReadBooks)) {
    if(!IsEmpty(&completedBooks)) {
      printf("%s", completedBooks.head->bookEntry.name);
      for(int k = strlen(completedBooks.head->bookEntry.name); k < TerminalWidth; k++){
        printf(" ");
      }
      RemoveBookListHead(&completedBooks);
    }
    else {
      for(int k = 0; k < TerminalWidth; k++){
        printf(" ");
      }
    }

    if(!IsEmpty(&readingBooks)) {
      printf("%s", readingBooks.head->bookEntry.name);
      for(int k = strlen(readingBooks.head->bookEntry.name); k < TerminalWidth; k++){
        printf(" ");
      }
      RemoveBookListHead(&readingBooks);
    }
    else {
      for(int k = 0; k < TerminalWidth; k++){
        printf(" ");
      }
    }
    
    if(!IsEmpty(&toReadBooks)) {
      printf("%s\n", toReadBooks.head->bookEntry.name);
      RemoveBookListHead(&toReadBooks);
    }
    else {
      for(int k = 0; k < TerminalWidth; k++){
        printf(" ");
      }
    }
  }
  
  free(bookEntry.name);
  free(bookEntry.author);
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