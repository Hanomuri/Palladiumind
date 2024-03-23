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
#ifndef _SCREEN
    #define _SCREEN
    #include "../Screen.h"
#endif

#define WISHLIST          0x4000
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

  CustomPage newCustomPage = GenCustomPage();

  while(current != NULL) {
    if(current->argument[0] == '-') {
      break;
    }
    strcat(newCustomPage.name, current->argument);
    
    current = current->next;
    if(current != NULL && current->argument[0] != '-') {
      strcat(newCustomPage.name, " ");
    }
  }

  if(strlen(newCustomPage.name) == 0 || strlen(newCustomPage.name) == commandList->currSize-4) {
    free(newCustomPage.name);
    //ERROR HANDLING
    return;
  }
  
  if(strcmp(current->argument, "-BOOKPAGE") == 0) {
    newCustomPage.sectionType |= BOOK_PAGE;
  }
  else if(strcmp(current->argument, "-TODOPAGE") == 0 || strcmp(current->argument, "-TODOLIST") == 0) {
    newCustomPage.sectionType = 0;
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

static void AddCustomCommand(const CommandList* commandList, char** filepath) {
  CommandBlock* current = commandList->head->next;

  unsigned char typePage;
  FILE* readCustomEntriesFile = fopen(*filepath, "r");

  fscanf(readCustomEntriesFile, "%hhu", &typePage);
  fclose(readCustomEntriesFile);
  
  BookEntry newEntry = GenBookEntry();
  #define ENTRY_GROUP newEntry.score
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
    free(newEntry.author);
    //ERROR HANDLING
    return;
  }
  while(current != NULL) {
    #define NEXT current->next
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
    else if (strcmp(current->argument, "-DISCARDED") == 0) {
      newEntry.data |= DISCARDED;
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
        newEntry.data |= WISHLIST;
      }
      else if (strcmp(NEXT->argument, "READING") == 0) {
        newEntry.data |= READING;
      }
    }
    else if (strcmp(current->argument, "-m") == 0) {
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

  FILE* writeCustomFile = fopen(*filepath, "a");
  if(typePage & BOOK_PAGE) {
    fprintf(writeCustomFile, "%d %d %d %s\n", newEntry.data, newEntry.year, newEntry.score, newEntry.name);
    fprintf(writeCustomFile, "%s\n", newEntry.author);
  } 
  else {
    fprintf(writeCustomFile, "%d %d %d %s\n", newEntry.data, newEntry.year, ENTRY_GROUP, newEntry.name);
  }
  fclose(writeCustomFile);
}

static void RemoveCustomEntry(FILE* writeFile, BookEntry entry, int currentLine, AVLNode* deleteLineTree, unsigned char type) {
  if( !InAVLTree(deleteLineTree, currentLine) ) {
    fprintf(writeFile, "%d %d %d%s\n", entry.data, entry.year, entry.score, entry.name);
    if(type == BOOK_PAGE) {
      fprintf(writeFile, "%s\n", entry.author);
    }
  }
}

static void CompleteCustomEntry(FILE* writeFile, BookEntry entry, int currentLine, AVLNode* deleteLineTree, unsigned char type) {
  if( InAVLTree(deleteLineTree, currentLine) ) {
    entry.data |= COMPLETED; 
  }
  fprintf(writeFile, "%d %d %d%s\n", entry.data, entry.year, entry.score, entry.name);
  if(type == BOOK_PAGE) {
    fprintf(writeFile, "%s\n", entry.author);
  }
}

static void ClearCustomEntry(FILE* writeFile, BookEntry entry, int currentLine, AVLNode* deleteLineTree, unsigned char type) {
  if( InAVLTree(deleteLineTree, currentLine) ) {
    for (int k = 12; k < 15; k++) {
      entry.data = BIT_CLEAR(entry.data, k);
    }
  }
  fprintf(writeFile, "%d %d %d%s\n", entry.data, entry.year, entry.score, entry.name);
  if(type == BOOK_PAGE) {
    fprintf(writeFile, "%s\n", entry.author);
  }
}

static void ReadingCustomEntry(FILE* writeFile, BookEntry entry, int currentLine, AVLNode* deleteLineTree, unsigned char type) {
  if(type != BOOK_PAGE) {
    fprintf(writeFile, "%d %d %d%s\n", entry.data, entry.year, entry.score, entry.name);
    return;
  }
  if( InAVLTree(deleteLineTree, currentLine) ) {
    entry.data |= READING; 
  }
  fprintf(writeFile, "%d %d %d%s\n", entry.data, entry.year, entry.score, entry.name);
  fprintf(writeFile, "%s\n", entry.author);
}

static void CustomEntryMap(const CommandList* commandList, void (*EntryOperation)(FILE*, BookEntry, int, AVLNode*, unsigned char), char** filepath) {
  CommandBlock* current = commandList->head->next;
  FILE *readFile, *tempFile;
  BookEntry entry = GenBookEntry();
  AVLNode* changeTreeLines = NULL;
  while(current != NULL) {
    InsertToAVL(&changeTreeLines, atoi(current->argument));
    current = current->next;
  }
  
  #define CUSTOM_FILENAME      *filepath
  #define CUSTOM_TEMP_FILENAME "temp_entries.txt"
  readFile = fopen(CUSTOM_FILENAME, "r");
  unsigned char type;
  fscanf(readFile, "%hhd\n", &type);
  tempFile = fopen(CUSTOM_TEMP_FILENAME, "w");
  fprintf(tempFile, "%hhd\n", type);

  #define CUSTOM_ENTRY_SCAN fscanf(readFile, "%hd%hhu%hhu%[^\n]s", &entry.data, &entry.year, &entry.score, entry.name)
  int currentLine = 1;
  while (CUSTOM_ENTRY_SCAN != EOF) {
    if (type == BOOK_PAGE) {
      fgetc(readFile);
      fscanf(readFile, "%[^\n]s", entry.author);
    }
    EntryOperation(tempFile, entry, currentLine, changeTreeLines, type);
    currentLine++;
  } 
  
  fclose(readFile);
  fclose(tempFile);
  remove(CUSTOM_FILENAME);
  rename(CUSTOM_TEMP_FILENAME, CUSTOM_FILENAME);
  DeleteAVLTree(changeTreeLines);
  free(entry.name);
  free(entry.author);
}