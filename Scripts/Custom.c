#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 
#include <string.h>
#include "Mind.h"
#include "Custom.h"

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
  u_short markBuffer = 0;
  for(u_short k = 12; k < 15; k++) {
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
  if(bookEntry->data & DISCARTED) {
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

  CustomPage customPage;
  customPage.name = malloc(sizeof(char)*45);

  #define ENTRY_SCAN fscanf(readCustomFile, "%hhd%[^\n]s", &customPage.sectionType, customPage.name)
  for(int currentPage = 1; ENTRY_SCAN != EOF; currentPage++) {
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

  if(readCustomEntriesFile == NULL) {
    printf("-EMPTY-\n");
    return;
  }

  if(type & BOOK_PAGE) {
    BookEntry bookEntry;
    bookEntry.name   = malloc(sizeof(char)*45);
    bookEntry.author = malloc(sizeof(char)*45);
    memset(bookEntry.name, 0, 45*sizeof(char));
    memset(bookEntry.author, 0, 45*sizeof(char));
    #define CUSTOM_ENTRY_SCAN fscanf(readCustomEntriesFile, "%hd%hhd%hhd %[^\n]s", &bookEntry.data, &bookEntry.year, &bookEntry.score, bookEntry.name)
    for(int currentEntry = 1; CUSTOM_ENTRY_SCAN != EOF; currentEntry++) {
      fgetc(readCustomEntriesFile);
      fscanf(readCustomEntriesFile, "%[^\n]s", bookEntry.author);
      PrintBookPageEntry(&bookEntry, type);
    }
    free(bookEntry.name);
    free(bookEntry.author);
  }

  fclose(readCustomEntriesFile);
}