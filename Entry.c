#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 

#include "Mind.h"

#define COMPLETED         0x8000
#define TASK              0x0
#define EVENT             1
#define RESEARCH          2
#define NOTE              3
#define DATE              4
#define IMPORTANT         0x800
#define DISCARTED         0x400
#define HAVE_DATE         0x200

#define TRUE 1

#define BIT_VALUE(X, Y) (((X) >> (Y)) & 0x1)
#define u_short __uint8_t
//□ ☕ ⛝  ◯ FOR LATER

static void PrintEntry(const struct Entry* entry) {
  u_short markBuffer = 0;
  for(u_short k = 12; k < 15; k++) {
    if(BIT_VALUE(entry->data, k)) markBuffer += pow(2, k-12);
  }
  if (markBuffer == DATE) {
    printf("☕");
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
  else if(entry->data & COMPLETED) {
    printf("⛝");
  } 
  else {
    printf("□");
  }

  printf("%s", entry->name);
  
  if(entry->data & HAVE_DATE) {
    struct tm date = {0}; // To calculate Day

    u_short dateBuffer = 0; 
    //First data Buffer will be for months
    for(u_short k = 5; k < 9; k++) {
      if(BIT_VALUE(entry->data, k)) dateBuffer += pow(2, k-5);
    }
    printf(" %d/", dateBuffer);

    date.tm_mon = dateBuffer-1;   //month zeor-base

    dateBuffer = 0;
    //Now data Buffer will be for days
    for(u_short k = 0; k < 5; k++) {
      if(BIT_VALUE(entry->data, k)) dateBuffer += pow(2, k);
    }
    printf("%d ", dateBuffer);
    
    date.tm_mday = dateBuffer;
    date.tm_year = 100 + entry->year;

    if(mktime(&date) == - 1) { 
      printf("Operation failed.\n"); 
      return; 
    } 
    const char *day[7] = { 
                   "SUN"  , "MON", "TU", "WED", 
                   "TH", "FRI", "SAT" 
                       }; 

    printf("%s", day[date.tm_wday]);
    
    time_t now = time(NULL);
    struct tm *now_tm = localtime(&now);

    if(now_tm->tm_year != 100 + entry->year)
      printf(" %d", 2000 + entry->year);
  }
  if (entry->data & IMPORTANT) {
    printf("!");
  }

  printf("\n");
}

void ReadEntriesData() {
  FILE* readEntriesFile = fopen("Resources/Entries.txt", "r"); 

  if(readEntriesFile == NULL) {
    printf("-EMPTY-\n");
    return;
  }

  Entry entry;
  entry.name = malloc(sizeof(char)*45);
  #define ENTRY_SCAN fscanf(readEntriesFile, "%hd%hhd%[^\n]s", &entry.data, &entry.year, entry.name)
  for(int currentEntry = 1; ENTRY_SCAN != EOF; currentEntry++) {
    printf("%d ", currentEntry);
    PrintEntry(&entry);
  }
  
  fclose(readEntriesFile);
  free(entry.name);
}

