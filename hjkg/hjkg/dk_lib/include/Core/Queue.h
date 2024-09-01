#ifndef QUEUE_H_
#define QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ForwardList.h"

typedef struct _Queue_
{
	ForwardList* list;
}Queue;

Queue* que_new();

void que_del(Queue* que);

void que_push(Queue* que, general v);

void que_pop(Queue* que);

Node* que_front(Queue* que);

Node* que_back(Queue* que);

bool que_empty(Queue* que);

size_t que_size(Queue* que);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif