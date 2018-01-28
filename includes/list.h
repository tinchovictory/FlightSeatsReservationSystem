#ifndef __LIST_H__
#define __LIST_H__


typedef struct List * ListPtr;

typedef struct ListIterator * ListIteratorPtr;

ListPtr listInit(int bytes);

void addToList(ListPtr list, void * data);

void freeList(ListPtr list);

ListIteratorPtr listIterator(ListPtr list);

int iteratorHasNext(ListIteratorPtr iterator);

void iteratorGetNext(ListIteratorPtr iterator, void * data);

void freeIterator(ListIteratorPtr iterator);

#endif