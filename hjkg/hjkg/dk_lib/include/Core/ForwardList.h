#ifndef _LIST_H_
#define _LIST_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include"GlobalDef.h"

//节点
typedef struct _Node_
{
	general data;
	struct _Node_* next;
}Node;

//管理链表
typedef struct _ForwardList_
{
	int size;
	Node* front;
	Node* tail;
}ForwardList;

ForwardList* flist_new();

void flist_del(ForwardList* list);

void flist_set(ForwardList* list, general val);

Node* flist_getFirst(ForwardList* list);

Node* flist_getLast(ForwardList* list);

Node* flist_get(ForwardList* list, CompareCallBack cmp, general v);

void flist_removeFirst(ForwardList* list);

void flist_removeLast(ForwardList* list);

size_t flist_size(ForwardList* list);

#define FList_foreach(type,name,flist)\
	for (Node* _curNode = (list)->front, *_i = 0; _curNode; _curNode = _curNode->next, _i = 0)\
		for (type name = (type)_curNode->data; (int)_i < 1; _i++)


#ifdef __cplusplus
}
#endif // __cplusplus

#endif