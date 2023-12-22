#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 

#include "Mind.h"

#define COMPLETED 0x8000
#define IMPORTANT 0x800
#define MIGRATION 0x400
#define HAVE_DATE 0x200

#define TRUE 1

#define BIT_VALUE(X, Y) (((X) >> (Y)) & 0x1)
//□ ⛾ ⛝ FOR LATER

void GetStringEntry(struct Entry* entry) {
  if(entry->data & COMPLETED) printf("⛝ ");
  else printf("□ ");

  printf("%s ", entry->name);
  
  if(entry->data & HAVE_DATE) {
    struct tm date = {0}; // To calculate Day

    __uint8_t dataBuffer = 0; 
    //First data Buffer will be for months
    for(__uint8_t k = 5; k < 9; k++) {
      if(BIT_VALUE(entry->data, k)) dataBuffer += pow(2, k-5);
    }
    printf("%d/", dataBuffer);

    date.tm_mon = dataBuffer-1;   //month zeor-base

    dataBuffer = 0;
    //Now data Buffer will be for days
    for(__uint8_t k = 0; k < 5; k++) {
      if(BIT_VALUE(entry->data, k)) dataBuffer += pow(2, k);
    }
    printf("%d ", dataBuffer);
    
    date.tm_mday = dataBuffer;
    date.tm_year = 100 + entry->year;

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

    if(now_tm->tm_year != 100 + entry->year)
      printf("%d", 2000 + entry->year);
  }
  printf("\n");
}

void ReadEntriesData() {
  FILE* readEntriesFile = fopen("entries.txt", "r"); 

  if(readEntriesFile == NULL) {
    return;
  }

  Entry entry;
  entry.name = malloc(sizeof(char)*45);
  #define ENTRY_SCAN fscanf(readEntriesFile, "%d%d%[^\n]s", &entry.data, &entry.year, entry.name)
  while (ENTRY_SCAN != EOF) {
    GetStringEntry(&entry);
  } 
  
  fclose(readEntriesFile);
}