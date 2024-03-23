#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h> 

#include "Mind.h"

#define EVENT_MARK        1
#define RESEARCH_MARK     2
#define NOTE_MARK         3
#define DATE_MARK         4

#define TRUE 1

//□ ☕ ⛝  ◯ FOR LATER

void PrintDateEntry(short entryData, unsigned char yearData) {
  struct tm date = {0}; // To calculate Day

  unsigned char dateBuffer = 0; 
  //First data Buffer will be for months
  for(unsigned char k = 5; k < 9; k++) {
    if(BIT_VALUE(entryData, k)) dateBuffer += pow(2, k-5);
  }
  printf(" %d/", dateBuffer);

  date.tm_mon = dateBuffer-1;   //month zeor-base

  dateBuffer = 0;
  //Now data Buffer will be for days
  for(unsigned char k = 0; k < 5; k++) {
    if(BIT_VALUE(entryData, k)) dateBuffer += pow(2, k);
  }
  printf("%d ", dateBuffer);
  
  date.tm_mday = dateBuffer;
  date.tm_year = 100 + yearData;

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

  if(now_tm->tm_year != 100 + yearData)
    printf(" %d", 2000 + entryData);
}

void PrintEntry(const struct Entry* entry, bool rev) {
  unsigned char markBuffer = 0;
  for(unsigned char k = 12; k < 15; k++) {
    if(BIT_VALUE(entry->data, k)) markBuffer += pow(2, k-12);
  }
  if (entry->data & DISCARDED) {
    //TWENTY ONEEE
  }
  else if (markBuffer == DATE_MARK) {
    printf("☕");
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
  else if(entry->data & COMPLETED) {
    printf("⛝");
  } 
  else {
    printf("□");
  }
  
  if(entry->data & DISCARDED) {
    printf(TEXT_STRIKE);
  }
  else if (markBuffer != DATE_MARK){
    printf(" ");
  }
  if( rev )
  {
    printf(COLOR_REV);
  }
  printf("%s", entry->name);
  printf(ATTR_OFF);
  
  if(entry->data & HAVE_DATE) {
    PrintDateEntry(entry->data, entry->year);
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
  #define ENTRY_SCAN fscanf(readEntriesFile, "%hd%hhu%hhu %[^\n]s", &entry.data, &entry.year, &entry.group, entry.name)
  for(int currentEntry = 1; ENTRY_SCAN != EOF; currentEntry++) {
    printf("%d ", currentEntry);
    PrintEntry(&entry, 1);
  }
  
  fclose(readEntriesFile);
  free(entry.name);
}

Entry GenEntry() {
  Entry newEntry;
  newEntry.data   = 0;
  newEntry.name   = (char*)malloc(45*sizeof(char));
  newEntry.year   = 0;
  newEntry.group  = 0;
  memset(newEntry.name, 0, 45*sizeof(char));
  return newEntry;
}

Mind GenMind() {
  Mind mind;
  mind.section         = 0;
  mind.section         |= HOME;
  mind.heightInCommand = 0;
  mind.currentNavi     = 0;
  mind.currentCollum   = NULL;
  mind.filepath        = malloc(60*sizeof(char));
  memset(mind.filepath, 0, 60*sizeof(char));
  mind.pageName        = malloc(45*sizeof(char));
  memset(mind.pageName, 0, 45*sizeof(char));

  return mind;
}