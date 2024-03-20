

typedef struct CustomPage {
  char*             name;
  unsigned char     sectionType;
} CustomPage;

CustomPage GenCustomPage();
void ReadCustomData(Mind* mind);
void ReadCustomPage(Mind* mind);
void DisplayBoard(const char* filepath);