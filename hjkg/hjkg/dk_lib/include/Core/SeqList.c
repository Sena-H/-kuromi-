#include "SeqList.h"
#include<stdio.h>
#include<memory.h>

static bool seqList_grow(SeqList* list)
{
	if (!list) return false;

	general* g = calloc(list->capacity * 2, sizeof(general));
	if (g)
	{
		memcpy(g, list->data, sizeof(general) * list->capacity);

		free(list->data);

		list->data = g;

		list->capacity *= 2;
		return true;
	}
	return false;
}

SeqList* seqList_new(size_t size)
{
	SeqList* list = calloc(1, sizeof(SeqList));
	if (list)
	{
		list->size = 0;
		list->capacity = size;
		list->data = calloc(list->capacity, sizeof(general));
		if (!list->data)
		{
			printf("%s failed!\n",__FUNCTION__);
			free(list);
			list = NULL;
		}
	}
	return list;
}

void seqList_del(SeqList* list)
{
	if (!list) return;
	free(list->data);
	free(list);
}

void seqList_set(SeqList* list, general val)
{
	if (!list) return;

	if (list->size == list->capacity)
	{
		if (!seqList_grow(list))
			return;
	}

	list->data[list->size++] = val;
}

general seqList_getFirst(SeqList* list)
{
	if (!list || list->size == 0)
		return NULL;
	return list->data[0];
}

general seqList_getLast(SeqList* list)
{
	if (!list || list->size == 0)
		return NULL;
	return list->data[list->size-1];
}

general seqList_get(SeqList* list, size_t index)
{
	if (!list || list->size == 0)
		return NULL;

	if (index < 0 || index >= list->size)
		return NULL;

	return list->data[index];
}

void seqList_pop_back(SeqList* list)
{
	if(!list)
		--list->size;
}

size_t seqList_size(SeqList* list)
{
	if (!list)
		return 0;
	return list->size;
}
