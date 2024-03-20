#include <stdio.h>
#include "CoreMode.h"
#include "Custom.h"

void EnterCustomPage (Mind* mind, size_t enterPage) {
  CustomPage readPage = GenCustomPage();

  FILE* readCustomFile;
  readCustomFile = fopen("Resources/CustomSection.txt", "r");

  #define CUSTOM_SCAN fscanf(readCustomFile, "%hhd %[^\n]s", &readPage.sectionType, readPage.name)
  int currentPage = 1;
  while (CUSTOM_SCAN != EOF) {
    if(currentPage == enterPage) {
      strcpy(mind->filepath, "Custom/");
      strcat(mind->filepath, readPage.name);
      strcat(mind->filepath, ".txt");
      strcpy(mind->pageName, readPage.name);
      mind->section = ENTRY;
      break;
    }
    currentPage++;
  }
  free(readPage.name);
  fclose(readCustomFile);

  FormatScreen(mind);
}