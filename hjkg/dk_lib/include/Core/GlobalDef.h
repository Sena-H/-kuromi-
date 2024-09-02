#ifndef GLOBALDEF_H_
#define GLOBALDEF_H_

#include<stdint.h>
#include<stdbool.h>
#include<malloc.h>



typedef void* general;	//通用类型定义

typedef bool (*CompareCallBack)(general, general);	//比较回调

#define VALUE(val) (general)(val)

#define _Break _i=INT32_MAX;break	//跳出foreach

#define _Continue break				//继续foreach

#endif // !GLOBALDEF_H_

