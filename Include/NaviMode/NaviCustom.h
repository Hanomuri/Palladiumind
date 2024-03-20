#define WISHLIST          0x4000
#define READING           0x5000

#define BOOK_PAGE         0x1

static void NaviCompleteCustomEntry(FILE* writeFile, BookEntry entry, unsigned char type) {
  if (entry.data & COMPLETED)
  {
    BIT_CLEAR(entry.data, 15);
  }
  else
  {
    entry.data |= COMPLETED; 
  }
  fprintf(writeFile, "%d %d %d%s\n", entry.data, entry.year, entry.score, entry.name);
  if(type == BOOK_PAGE) {
    fprintf(writeFile, "%s\n", entry.author);
  }
}

static void NaviCustomEntryMap(int changeLine, void (*EntryOperation)(FILE*, BookEntry, unsigned char), char** filepath) {
  FILE *readFile, *tempFile;
  BookEntry entry = GenBookEntry();
  
  #define CUSTOM_FILENAME      *filepath
  #define CUSTOM_TEMP_FILENAME "temp_entries.txt"
  readFile = fopen(CUSTOM_FILENAME, "r");
  unsigned char type;
  fscanf(readFile, "%hhd\n", &type);
  tempFile = fopen(CUSTOM_TEMP_FILENAME, "w");
  fprintf(tempFile, "%hhd\n", type);

  #define CUSTOM_ENTRY_SCAN fscanf(readFile, "%hd%hhd%hhd%[^\n]s", &entry.data, &entry.year, &entry.score, entry.name)
  int currentLine = 1;
  while (CUSTOM_ENTRY_SCAN != EOF) {
    if (type == BOOK_PAGE) 
    {
      fgetc(readFile);
      fscanf(readFile, "%[^\n]s", entry.author);
    }
    if (changeLine == currentLine)
    {
      EntryOperation(tempFile, entry, type);
    }
    else
    {
      fprintf(tempFile, "%d %d %d%s\n", entry.data, entry.year, entry.score, entry.name);
      if(type == BOOK_PAGE) {
      fprintf(tempFile, "%s\n", entry.author);
  }
    }
    currentLine++;
  } 
  
  fclose(readFile);
  fclose(tempFile);
  remove(CUSTOM_FILENAME);
  rename(CUSTOM_TEMP_FILENAME, CUSTOM_FILENAME);
  free(entry.name);
  free(entry.author);
}