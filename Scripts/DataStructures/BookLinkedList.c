#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BookLinkedList.h"

void InitBoardList(BoardList* boardList) {
  boardList->length  = 0;
  boardList->head    = NULL;
  boardList->tail    = NULL;
}

void InsertBoardList(BoardList* boardList, BookEntry insertEntry, short currentNumber) {
  BoardListNode* newNode = (BoardListNode*)malloc(sizeof(struct BoardListNode));
  newNode->entry.name = (char*)malloc(30*sizeof(char));
  strcpy(newNode->entry.name, insertEntry.name);
  newNode->next = NULL;
  newNode->entry.data = insertEntry.data;
  newNode->numberEntry = currentNumber;
  boardList->length++;
  if(boardList->head == NULL) {
    boardList->head = newNode;
    boardList->tail = newNode;
    return;
  }
  boardList->tail->next   = newNode;
  boardList->tail         = boardList->tail->next;
}

void RemoveBoardListHead(BoardList* boardList) {
  if (boardList->tail == boardList->head) {
    free(boardList->tail->entry.name);
    free(boardList->tail);
    boardList->head = NULL;
  } else {
    BoardListNode* auxNode = boardList->head;
    boardList->head = boardList->head->next;
    free(auxNode->entry.name);
    free(auxNode);
  }
  boardList->length--;
}

void DeleteBoardList(BoardList* boardList) {
  while(boardList->head != NULL) {
    if (boardList->tail == boardList->head) {
      boardList->head = NULL;
      free(boardList->tail);
    } else {
      BoardListNode* auxNode = boardList->head;
      boardList->head = boardList->head->next;
      free(auxNode->entry.name);
      free(auxNode);
    }
  }
}

unsigned char IsEmpty(BoardList* BoardList) {
  if (BoardList->length == 0) {
    return 1;
  }
  return 0;
}