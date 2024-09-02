#include "Stack.h"

Stack* stack_new()
{
    Stack* sk = calloc(1, sizeof(Stack));
    if (sk)
    {
        sk->seqList = seqList_new(10);
    }
    return sk;
}

void stack_del(Stack* sk)
{
    if (!sk) return;
    if (sk->seqList)
        seqList_del(sk->seqList);
    free(sk);
}

void stack_push(Stack* sk, general v)
{
    seqList_set(sk->seqList, v);
}

void stack_pop(Stack* sk)
{
    if (!sk) return;

    if(sk->seqList)
        sk->seqList->size--;
}

general stack_top(Stack* sk)
{
    return seqList_getLast(sk->seqList);
}

bool stack_empty(Stack* sk)
{
    return seqList_size(sk->seqList) == 0;
}

size_t stack_size(Stack* sk)
{
    return seqList_size(sk->seqList);
}
