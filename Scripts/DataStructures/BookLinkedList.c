#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BookLinkedList.h"

void InitBookList(BookList* bookList) {
  bookList->length  = 0;
  bookList->head    = NULL;
  bookList->tail    = NULL;
}

void InsertBookList(BookList* bookList, BookEntry insertEntry) {
  BookListNode* newNode = (BookListNode*)malloc(sizeof(struct BookListNode));
  newNode->bookEntry.name = (char*)malloc(30*sizeof(char));
  strcpy(newNode->bookEntry.name, insertEntry.name);
  newNode->next = NULL;
  newNode->bookEntry.data = insertEntry.data;
  bookList->length++;
  if(bookList->head == NULL) {
    bookList->head = newNode;
    bookList->tail = newNode;
    return;
  }
  bookList->tail->next   = newNode;
  bookList->tail         = bookList->tail->next;
}

void RemoveBookListHead(BookList* bookList) {
  if (bookList->tail == bookList->head) {
    bookList->head = NULL;
    free(bookList->tail->bookEntry.name);
    free(bookList->tail);
  } else {
    BookListNode* auxNode = bookList->head;
    bookList->head = bookList->head->next;
    free(auxNode->bookEntry.name);
    free(auxNode);
  }
  bookList->length--;
}

void DeleteBookList(BookList* bookList) {
  while(bookList->head != NULL) {
    if (bookList->tail == bookList->head) {
      bookList->head = NULL;
      free(bookList->tail);
    } else {
      BookListNode* auxNode = bookList->head;
      bookList->head = bookList->head->next;
      free(auxNode->bookEntry.name);
      free(auxNode);
    }
  }
}

unsigned char IsEmpty(BookList* bookList) {
  if (bookList->length == 0) {
    return 1;
  }
  return 0;
}