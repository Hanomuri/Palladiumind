

typedef struct CustomPage {
  char*             name;
  __uint8_t         sectionType;
} CustomPage;

void ReadCustomData();
void ReadCustomPage(const char* filepath);