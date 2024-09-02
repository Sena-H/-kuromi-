#include "Queue.h"

Queue* que_new()
{
    Queue* q = calloc(1, sizeof(Queue));
    if (q)
    {
        q->list = flist_new(10);
    }
    return q;
}

void que_del(Queue* que)
{
    if (!que) return;
    if (que->list)
        flist_del(que->list);
    free(que);
}

void que_push(Queue* que, general v)
{
    flist_set(que->list, v);
}

void que_pop(Queue* que)
{
    flist_removeFirst(que->list);
}

Node* que_front(Queue* que)
{
    if (!que) return NULL;
    return flist_getFirst(que->list);
}

Node* que_back(Queue* que)
{
    if (!que) return NULL;
    return flist_getLast(que->list);
}

bool que_empty(Queue* que)
{
    if (!que) return true;
    return flist_size(que->list) == 0;
}

size_t que_size(Queue* que)
{
    if (!que) return 0;
    return flist_size(que->list);
}
