

typedef struct CustomPage {
  char*             name;
  unsigned char     sectionType;
} CustomPage;

CustomPage GenCustomPage();
void ReadCustomData();
void ReadCustomPage(const char* filepath);
void DisplayBoard(const char* filepath);