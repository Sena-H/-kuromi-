#ifndef STACK_H_
#define STACK_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "SeqList.h"

typedef struct _Stack_
{
	SeqList* seqList;
}Stack;

Stack* stack_new();

void stack_del(Stack* sk);

void stack_push(Stack* sk, general v);

void stack_pop(Stack* sk);

general stack_top(Stack* sk);

bool stack_empty(Stack* sk);

size_t stack_size(Stack* sk);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif