
#ifndef _PALLADIUM_MIND
  #define _PALLADIUM_MIND
  #include "../Mind.h"
#endif
#ifndef _STDDEF
  #define _STDDEF
  #include <stddef.h>
#endif


typedef struct BoardListNode {
  short                        numberEntry;
  BookEntry                    entry;           
  struct BoardListNode*        next;
} BoardListNode;

typedef struct BoardList {
  size_t                       length;
  BoardListNode*               head;
  BoardListNode*               tail;
} BoardList;


void InitBoardList(BoardList* boardList);
void InsertBoardList(BoardList* boardList, BookEntry insertEntry, short currentNumber);
void RemoveBoardListHead(BoardList* boardList);
void DeleteBoardList(BoardList* boardList);
unsigned char IsEmpty(BoardList* boardList);