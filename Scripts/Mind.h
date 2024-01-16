

/* ENTRY DATA
0000000000000000
16 Completed
15-13 Mark
12 Important
11 Discarted
10 Have date
9-6 Month
5-1 Day
char bits is 2000+year 
*/
#ifndef _SECTIONS
  #define _SECTIONS
  #define HOME                  0x80
  #define FUTURE_LOG            0x40
  #define MONTLY                0x20
  #define CUSTOM                0x10
  #define ENTRY                 0x8
#endif

#ifndef _TEXT_MOD
  #define _TEXT_MOD
  #define COLOR_BOLD            "\e[1m"
  #define TEXT_DIM              "\e[2m"
  #define COLOR_REV             "\e[7m"
  #define TEXT_STRIKE           "\e[9m"
  #define ATTR_OFF              "\e[m"
#endif

#ifndef _ENTRY_ATR
  #define COMPLETED         0x8000
  #define TASK              0x0
  #define EVENT             0x1000
  #define RESEARCH          0x2000
  #define NOTE              0x3000
  #define DATE              0x4000
  #define IMPORTANT         0x800
  #define DISCARTED         0x400
  #define HAVE_DATE         0x200
#endif

#define BIT_VALUE(X, Y) (((X) >> (Y)) & 0x1)

typedef struct Entry {
  short               data;
  char*               name;
  unsigned char       year;
  unsigned char       group;
}Entry;

typedef struct BookEntry {
  short               data;
  char*               name;
  char*               author;
  unsigned char       score;
  unsigned char       year;
}BookEntry;

void PrintDateEntry(short entryData, unsigned char yearData);
void ReadEntriesData();


