#ifndef MACROSEQLIST_H_
#define MACROSEQLIST_H_
#include<assert.h>
#include<malloc.h>
#include<stdint.h>

#define SeqList_Def(ItemType)\
typedef struct _SeqList_##ItemType##_\
{\
	ItemType* data;\
	int capacity;\
	int size;\
}_SeqList_##ItemType;

#define SeqList_Type(ItemType) _SeqList_##ItemType

//#define SeqList_New(ItemType) (SeqList_Type(ItemType)*)calloc(1,sizeof(SeqList_Type(ItemType)));
#define SeqList_New(ItemType,varName) \
	SeqList_Type(ItemType)* varName = (SeqList_Type(ItemType)*)calloc(1,sizeof(SeqList_Type(ItemType)));\
	(varName)->capacity = 8;\
	(varName)->size = 0;\
	(varName)->data = (ItemType*)calloc((varName)->capacity,sizeof(ItemType));\
	assert((varName)->data !=NULL);

#define SeqList_NewReturn(ItemType,varName) \
	 varName = (SeqList_Type(ItemType)*)calloc(1,sizeof(SeqList_Type(ItemType)));\
	(varName)->capacity = 8;\
	(varName)->size = 0;\
	(varName)->data = (ItemType*)calloc((varName)->capacity,sizeof(ItemType));\
	assert((varName)->data !=NULL);

#define SeqList_Delete(list)\
	free(list);

//#define SeqList_Init(list,ItemType)\
//	(list)->capacity = 8;\
//	(list)->size = 0;\
//	(list)->data = (ItemType*)calloc((list)->capacity,sizeof(ItemType));\
//	assert((list)->data !=NULL);
	
#define SeqList_Set(list,item,ItemType)\
	if((list)->size == (list)->capacity)\
	{\
		void* realloc_memory =	(void*)realloc((list)->data, (list)->capacity * 2 * sizeof((list)->data[0]));\
		assert(realloc_memory != NULL);\
		(list)->capacity*=2;\
		(list)->data = (ItemType*)realloc_memory;\
	}\
	(list)->data[(list)->size++] = item;

#define SeqList_Get(list,index)\
	(list)->data[index];

#define SeqList_Remove(list,index,ItemType)\
	assert(index >=0 && index < (list)->size);\
	for (int _i = index; _i < (list)->size -1; _i++)\
	{\
		ItemType tmp = (list)->data[_i];\
		(list)->data[_i] = (list)->data[_i + 1];\
		(list)->data[_i + 1] = tmp;\
	}
	

#define SeqList_Clear(list)\
	(list)->size = 0;

#define SeqList_Foreach(var,list)\
for (int _i = 0,_cnt = 0; _i < (list)->size; _i++,_cnt = 0)\
	for(var = (list)->data[_i];_cnt < 1;_cnt++)

#define Break _i=INT32_MAX;break

#define Continue break

typedef const char* const_str;
typedef const char* str;
#define BaseType_Def\
	SeqList_Def(int);\
	SeqList_Def(int32_t);\
	SeqList_Def(uint32_t);\
	SeqList_Def(int64_t);\
	SeqList_Def(uint64_t);
	SeqList_Def(const_str);\
	SeqList_Def(str);\
	SeqList_Def(float);\
	SeqList_Def(double);

BaseType_Def

#endif // !MACROSEQLIST_H_
