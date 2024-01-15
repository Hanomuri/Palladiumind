#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 
#include <string.h>
#include "Mind.h"
#include "Custom.h"


#define COLOR_BOLD            "\e[1m"
#define COLOR_OFF             "\e[m"

#define BOOK_PAGE 0x1

#define COMPLETED         0x8000
#define TO_READ           0x0
#define EVENT             1
#define RESEARCH          2
#define NOTE              3
#define WHISHLIST         4
#define READING           5
#define IMPORTANT         0x800
#define DISCARTED         0x400
#define HAVE_DATE         0x200

#define BIT_VALUE(X, Y) (((X) >> (Y)) & 0x1)
#define u_short unsigned char

static void PrintCustomSection(const struct CustomPage* customPage) {
  if(customPage->sectionType & BOOK_PAGE) {
    printf("🕮");
  }
  printf("%s\n", customPage->name);
}

static void PrintBookPageEntry(const struct BookEntry* bookEntry, const u_short type) {
  u_short markBuffer = 0;
  for(u_short k = 12; k < 15; k++) {
    if(BIT_VALUE(bookEntry->data, k)) markBuffer += pow(2, k-12);
  }
  if(type & BOOK_PAGE) {
    if (markBuffer == READING) {
      printf("❣︎");
    } 
    else if (markBuffer == WHISHLIST) {
      printf("❣︎");
    } 
    else if (markBuffer == NOTE) {
      printf("─");
    } 
    else if (markBuffer == RESEARCH) {
      printf("⌕");
    } 
    else if (markBuffer == EVENT) {
      printf("◯");
    } 
    else if(bookEntry->data & COMPLETED) {
      printf("⛝");
    } 
    else {
      printf("□");
    }
  }

  printf("%s - %s", bookEntry->name, bookEntry->author);
  
  if(bookEntry->data & HAVE_DATE) {
    struct tm date = {0}; // To calculate Day

    u_short dateBuffer = 0; 
    //First data Buffer will be for months
    for(u_short k = 5; k < 9; k++) {
      if(BIT_VALUE(bookEntry->data, k)) dateBuffer += pow(2, k-5);
    }
    printf(" %d/", dateBuffer);

    date.tm_mon = dateBuffer-1;   //month zeor-base

    dateBuffer = 0;
    //Now data Buffer will be for days
    for(u_short k = 0; k < 5; k++) {
      if(BIT_VALUE(bookEntry->data, k)) dateBuffer += pow(2, k);
    }
    printf("%d ", dateBuffer);
    
    date.tm_mday = dateBuffer;
    date.tm_year = 100 + bookEntry->year;

    if(mktime(&date) == - 1) { 
      printf("Operation failed.\n"); 
      return; 
    } 
    const char *day[7] = { 
                   "SUN"  , "MON", "TU", "WED", 
                   "TH", "FRI", "SAT" 
                       }; 

    printf("%s ", day[date.tm_wday]);
    
    time_t now = time(NULL);
    struct tm *now_tm = localtime(&now);

    if(now_tm->tm_year != 100 + bookEntry->year)
      printf("%d", 2000 + bookEntry->year);
  }
  if (bookEntry->data & IMPORTANT) {
    printf("!");
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
    #define CUSTOM_ENTRY_SCAN fscanf(readCustomEntriesFile, "%hd%hhd%[^\n]s", &bookEntry.data, &bookEntry.year, bookEntry.name)
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