#ifndef SEQLIST_H_
#define SEQLIST_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include"GlobalDef.h"

typedef struct _SeqList_
{
	general* data;		//数组指针
	int size;			//元素个数
	int capacity;		//容量
}SeqList;

//创建顺序表
SeqList* seqList_new(size_t size);
//释放顺序表
void seqList_del(SeqList* list);
//添加数据
void seqList_set(SeqList* list, general val);
//获取第一个数据
general seqList_getFirst(SeqList* list);
//获取最后一个数据
general seqList_getLast(SeqList* list);
//获取下标为index的数据
general seqList_get(SeqList* list, size_t index);
//删除元素
void seqList_pop_back(SeqList* list);

//获取顺序表大小
size_t seqList_size(SeqList* list);


#define SeqList_foreach(type,name,list)\
for (int _i = 0,_cnt = 0; _i < (list)->size; _i++,_cnt = 0)\
	for(type name = (type)(list)->data[_i];_cnt < 1;_cnt++)


#ifdef __cplusplus
}
#endif // __cplusplus




#endif