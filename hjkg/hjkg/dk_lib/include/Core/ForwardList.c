#include "ForwardList.h"
#include<stdio.h>
#include<malloc.h>

static Node* flist_newNode(general v)
{
	Node* newNode = calloc(1, sizeof(Node));
	if (newNode)
	{
		newNode->data = v;
		newNode->next = NULL;
	}
	return newNode;
}

//--
ForwardList* flist_new()
{
	ForwardList* list = calloc(1, sizeof(ForwardList));
	if (list)
	{
		list->front = list->tail = NULL;
		list->size = 0;
	}
	return list;
}

void flist_del(ForwardList* list)
{
	if (!list) return;
	Node* curNode = list->front;
	Node* delNode = NULL;
	while (curNode)
	{
		delNode = curNode;
		curNode = curNode->next;
		free(delNode);
	}
	free(list);
}

void flist_set(ForwardList* list, general val)
{
	if (!list)return;

	Node* newNode = flist_newNode(val);
	if (list->size == 0)
	{
		list->front = newNode;
		list->tail = newNode;
	}
	else
	{
		list->tail->next = newNode;
		list->tail = newNode;
	}
	list->size++;
}

Node* flist_getFirst(ForwardList* list)
{
	return list->front;
}

Node* flist_getLast(ForwardList* list)
{
	return list->tail;
}

Node* flist_get(ForwardList* list, CompareCallBack cmp,general v)
{
	Node* curNode = list->front;
	while (curNode)
	{
		if (cmp(curNode->data, v))
		{
			return curNode;
		}
		curNode = curNode->next;
	}
	return NULL;
}

void flist_removeFirst(ForwardList* list)
{
	if (!list) return;
	if (list->size == 0)return;

	Node* delNode = list->front;
	list->front = delNode->next;

	if (list->front == NULL)
		list->tail = NULL;

	free(delNode);
	list->size--;
}

void flist_removeLast(ForwardList* list)
{
	if (!list) return;
	if (list->size == 0)return;

	if (list->size == 1)
	{
		free(list->front);
		list->front = list->tail = NULL;
	}
	else
	{
		Node* curNode = list->front;
		while (curNode->next != list->tail)
		{
			curNode = curNode->next;
		}

		Node* delNode = list->tail;
		list->tail = curNode;
		list->tail->next = NULL;

		free(delNode);
	}
	list->size--;
}

size_t flist_size(ForwardList* list)
{
	if (!list) return 0;
	return list->size;
}

