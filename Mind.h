
// 0000 0000 0000 0000
// 16 Completed
// 15-13 Mark
// 12 Important
// 11 Discarted
// 10 Have date
// 9-6 Month
// 5-1 Day
// char bits is 2000+year 
typedef struct Entry {
  short         data;
  __uint8_t     year;
  char*         name;
}Entry;

typedef struct BookEntry {
  short         data;
  __uint8_t     year;
  char*         name;
  char*         author;
}BookEntry;

void ReadEntriesData();

