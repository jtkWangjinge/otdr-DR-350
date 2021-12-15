
#ifndef __STRINGLIST_H__
#define __STRINGLIST_H__

#include "FIncludes.h"

#ifdef __cplusplus
extern "C" {
#endif

//前向声明内部结构
struct f_string_list_private;

//字符串列表定义
typedef struct f_string_list 
{
	struct f_string_list_private* d;
} FStringList;



/*
 *PUBLIC:
 */
//相当于构造函数
void  FStringList_ctor		(FStringList* me);
//相当于析构函数
void  FStringList_dtor		(FStringList* me);
//为一个StringList 保留一定的空间，用于优化频繁的内存操作
BOOL  FStringList_reserved	(FStringList* me, U32 count,  U32 bytes);
//返回StringList 中字符串的个数
U32   FStringList_count		(FStringList *me);
//获取指定索引上的字符串(char*)
char* FStringList_atIndex	(FStringList *me, U32 index);
//输出StringList到标准流
void  FStringList_output	(FStringList* me, FILE *stream);
//在StringList的尾部追加一个字符串
BOOL  FStringList_pushBack	(FStringList* me, const char *string);
//弹出尾部的字符串
BOOL  FStringList_popBack	(FStringList* me);


/*
 *PUBLIC STATIC:
 */
//相当于类的静态函数
//用来分隔字符串，并生成一个StringList,该StringList需要手动调用
//StringList_dtor函数来释放内存
//tips:该函数会修改源字符串str的内容
FStringList FStringList_stringTok(char* str, const char *delim);


#ifdef __cplusplus
}
#endif

#endif //__STRINGLIST_H__
