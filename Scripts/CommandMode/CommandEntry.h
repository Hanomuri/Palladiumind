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

static void AddEntryCommand(const CommandList* commandList) {
  CommandBlock* current = commandList->head->next;

  Entry newEntry = GenEntry();
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
    current = current->next;
  }

  FILE* writeEntryFile = fopen("Resources/Entries.txt", "a");
  fprintf(writeEntryFile, "%d %d %d %s\n", newEntry.data, newEntry.year, newEntry.group ,newEntry.name);
  fclose(writeEntryFile);

  free(newEntry.name);
}

static void RemoveEntry(FILE* writeFile, Entry entry, int currentLine, AVLNode* deleteLineTree) {
  #define REMOVE_WRITE fprintf(writeFile, "%d %d%s\n", entry.data, entry.year, entry.name);
  if( !InAVLTree(deleteLineTree, currentLine) ) {
    REMOVE_WRITE
  }
}

static void CompleteEntry(FILE* writeFile, Entry entry, int currentLine, AVLNode* completeLineTree) {
  #define COMPLETE_WRITE fprintf(writeFile, "%d %d%s\n", entry.data, entry.year, entry.name);
  if( InAVLTree(completeLineTree, currentLine) ) {
    entry.data |= COMPLETED;
  }
  COMPLETE_WRITE
  currentLine++;
}

static void DiscardEntry(FILE* writeFile, Entry entry, int currentLine, AVLNode* completeLineTree) {
  #define COMPLETE_WRITE fprintf(writeFile, "%d %d%s\n", entry.data, entry.year, entry.name);
  if( InAVLTree(completeLineTree, currentLine) ) {
    entry.data |= DISCARTED;
  }
  COMPLETE_WRITE
  currentLine++;
}

static void EntryMap(const CommandList* commandList, void (*EntryOperation)(FILE*, Entry, int, AVLNode*)) {
  CommandBlock* current = commandList->head->next;
  FILE *readFile, *tempFile;
  Entry entry = GenEntry();
  AVLNode* changeTreeLines = NULL;
  while(current != NULL) {
    InsertToAVL(&changeTreeLines, atoi(current->argument));
    current = current->next;
  }

  #define FILENAME      "Resources/Entries.txt"
  #define TEMP_FILENAME "Resources/temp_entries.txt"
  readFile = fopen(FILENAME, "r");
  tempFile = fopen(TEMP_FILENAME, "w");

  #define ENTRY_SCAN fscanf(readFile, "%hd%hhd%[^\n]s", &entry.data, &entry.year, entry.name)
  int currentLine = 1;
  while (ENTRY_SCAN != EOF) {
    EntryOperation(tempFile, entry, currentLine, changeTreeLines);
    currentLine++;
  } 
  
  fclose(readFile);
  fclose(tempFile);
  remove(FILENAME);
  rename(TEMP_FILENAME, FILENAME);
  DeleteAVLTree(changeTreeLines);
  free(entry.name);
}