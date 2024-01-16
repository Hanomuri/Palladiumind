

typedef struct CustomPage {
  char*             name;
  unsigned char     sectionType;
} CustomPage;

void ReadCustomData();
void ReadCustomPage(const char* filepath);