#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/list.h"

#include "includes/dbObj.h"

typedef struct Node {
	void * data;
	struct Node * next;
} Node;


typedef struct List {
	Node * first;
	Node * last;
	int bytes;
	int size;
} List;

typedef struct ListIterator {
	Node * node;
	int bytes;
} ListIterator;

ListPtr listInit(int bytes) {
	ListPtr list = malloc(sizeof(List));
	list->first = NULL;
	list->last = NULL;
	list->size = 0;
	list->bytes = bytes;

	return list;
}

void addToList(ListPtr list, void * data) {

	Node * node = malloc(sizeof(Node));
	node->data = malloc(list->bytes);
	node->next = NULL;
	memcpy(node->data, data, list->bytes);

	if(list->first == NULL) {
		list->first = node;
		list->last = list->first;
	} else {
		list->last->next = node;
		list->last = list->last->next;
	}

	list->size += 1;
}

void freeList(ListPtr list) {
	Node * node = list->first;
	Node * aux;

	while(node != NULL) {
		free(node->data);
		aux = node;
		node = node->next;
		free(aux);
	}

	free(list);
}

ListIteratorPtr listIterator(ListPtr list) {
	ListIteratorPtr iterator = malloc(sizeof(ListIterator));
	iterator->node = list->first;
	iterator->bytes = list->bytes;
	return iterator;
}

int iteratorHasNext(ListIteratorPtr iterator) {
	if(iterator->node == NULL) {
		return 0;
	}
	return 1;
}

void iteratorGetNext(ListIteratorPtr iterator, void * data) {
	if(!iteratorHasNext(iterator)) {
		data = NULL;
		return;
	}
	memcpy(data, iterator->node->data, iterator->bytes);
	iterator->node = iterator->node->next;
}

void freeIterator(ListIteratorPtr iterator) {
	free(iterator);
}