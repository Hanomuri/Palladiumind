
#ifndef _PALLADIUM_MIND
    #define _PALLADIUM_MIND
    #include "../Mind.h"
#endif


typedef struct BookListNode {
    BookEntry                      bookEntry;
    struct BookListNode*           next;
} BookListNode;

typedef struct BookList {
  size_t                         length;
  BookListNode*                  head;
  BookListNode*                  tail;
} BookList;


void InitBookList(BookList* bookList);
void InsertBookList(BookList* bookList, BookEntry insertEntry);
void RemoveBookListHead(BookList* bookList);
void DeleteBookList(BookList* bookList);
unsigned char IsEmpty(BookList* bookList);