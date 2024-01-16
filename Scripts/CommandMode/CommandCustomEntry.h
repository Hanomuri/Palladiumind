#ifndef _COMMAND_MODE
    #define _COMMAND_MODE
    #include "../CommandMode.h"
#endif
#ifndef _COMMAND_MODE
    #define _COMMAND_MODE
    #include "../CommandMode.h"
#endif
#ifndef _PALLADIUM_MIND
    #define _PALLADIUM_MIND
    #include "../Mind.h"
#endif
#ifndef _STDIO
    #define _STDIO
    #include <stdio.h>
#endif
#ifndef _STRING_H
    #define _STRING_H
    #include <string.h>
#endif
#ifndef _STDLIB
    #define _STDLIB
    #include <stdlib.h>
#endif
#ifndef _AVL
    #define _AVL
    #include "../AVLTree.h"
#endif
#ifndef _CUSTOM
    #define _CUSTOM
    #include "../Custom.h"
#endif

#define WHISHLIST         0x4000
#define READING           0x5000

#define BOOK_PAGE         0x1

static FILE* OpenCustomFilepath(char* filename, unsigned char Option) {
  char* filepath = (char*)malloc(60*sizeof(char));
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
  newCustomPage.name = (char*)malloc(60*sizeof(char));
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


static void EnterCustomPage (const CommandList* commandList, size_t enterPage, unsigned char* type) {
  CustomPage readPage;
  readPage.name = (char*)malloc(60*sizeof(char));
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

  unsigned char typePage;
  FILE* readCustomEntriesFile = fopen(*(commandList->filepath), "r");

  fscanf(readCustomEntriesFile, "%hhu", &typePage);
  fclose(readCustomEntriesFile);
  
  BookEntry newEntry;
  //IF YOU NEED VOID POINTER, ALE
  newEntry.data   = 0b0000000000000000;
  newEntry.name   = (char*)malloc(45*sizeof(char));
  newEntry.year   = 0b00000000;
  newEntry.score  = 0b00000000;
  memset(newEntry.name, 0, 45*sizeof(char));
  
  newEntry.author = (char*)malloc(45*sizeof(char));
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
    } 
    else if (strcmp(current->argument, "-i") == 0) {
      newEntry.data |= IMPORTANT;
    } 
    else if (strcmp(current->argument, "-d") == 0) {
      newEntry.data |= HAVE_DATE;
      newEntry.data |= (atoi(NEXT->argument) << 5);
      newEntry.data |= (atoi(NEXT->next->argument));
      newEntry.year |= (atoi(NEXT->next->next->argument) -2000);
    } 
    else if (strcmp(current->argument, "-DISCARTED") == 0) {
      newEntry.data |= DISCARTED;
    } 
    else if (strcmp(current->argument, "-m") == 0 && typePage & BOOK_PAGE) {
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
    else if (strcmp(current->argument, "-s") == 0 && typePage & BOOK_PAGE) {
      current = current->next;
      char* number = (char*)malloc(2*sizeof(char));
      char* mantissa = (char*)malloc(2*sizeof(char));
      memset(number, 0, 2*sizeof(char));
      memset(mantissa, 0, 2*sizeof(char));
      for(unsigned char currentChar = 0; currentChar < 2; currentChar++) {
        if (current->argument[currentChar] == '.') {
          break;
        }
        number[strlen(number)] = current->argument[currentChar];
      }
      newEntry.score |= atoi(number)<<1;
      for(int currentChar = strlen(number)+1; currentChar < strlen(current->argument); currentChar++) {
        mantissa[strlen(mantissa)] = current->argument[currentChar];
      }
      for (unsigned char k = atoi(mantissa); k > 49;) {
        if (k>49) {
          k -= 50;
          newEntry.score |= 0x1;
        }
      }
      free(number);
      free(mantissa);
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
  if(typePage & BOOK_PAGE) {
    fprintf(writeCustomFile, "%d %d %d %s\n", newEntry.data, newEntry.year, newEntry.score, newEntry.name);
    fprintf(writeCustomFile, "%s\n", newEntry.author);
  } 
  else {
    fprintf(writeCustomFile, "%d %d %s\n", newEntry.data, newEntry.year, newEntry.name);
  }
  fclose(writeCustomFile);
}