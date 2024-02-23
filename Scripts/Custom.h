

typedef struct CustomPage {
  char*             name;
  unsigned char     sectionType;
} CustomPage;

CustomPage GenCustomPage();
void ReadCustomData();
void ReadCustomPage(Mind* mind);
void DisplayBoard(const char* filepath);