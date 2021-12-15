#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 基本类型 */
#define F32                                 float                       
#define F64                                 double                      
#define U8                                  unsigned char               
#define U16                                 unsigned short              
#define U32                                 unsigned int                
#define U64                                 unsigned long long          
#define I32                                 signed int

#ifndef BOOL
#define BOOL                                U32
#endif
#ifndef FALSE
#define FALSE                               0
#endif
#ifndef TRUE
#define TRUE                                (!FALSE)
#endif
#ifndef NULL
#ifdef __cplusplus
#define NULL                                0
#else
#define NULL								((void*)0)
#endif
#endif


/* 调试相关宏定义,以及log级别 */	
#define F_ENABLE_DEBUG						0
#define F_ENABLE_ASSERT						0
#define F_MEMORY_DEBUG						0

#define F_LOG_LEVEL							4
#define F_LOG_VERBO							0
#define F_LOG_TRACE							1
#define F_LOG_DEBUG							2
#define F_LOG_INFO							3
#define F_LOG_ERROR							4

/* 内存管理调试 */
#if (F_MEMORY_DEBUG)				
static inline void* MemAlloc(U32 size) 
{
	void *res = malloc(size);
	printf("malloc(%p, %d)\n", res, size);
	return res;
}

static inline void MemFree(void* ptr) 
{
	printf("free(%p)\n", ptr);
	free(ptr);
}

static inline void* MemRealloc(void* ptr, U32 size) 
{
	void *res = realloc(ptr, size);
	printf("(%p)realloc(%p, %d)\n", ptr, res, size);
	return res;
}
#else
#define MemAlloc(_size) 		malloc(_size)
#define MemRealloc(_ptr, _size) realloc(_ptr, _size)
#define MemFree(_ptr)			free(_ptr)

#endif

/* memory-related */
#define F_NEW(_type) 						(_type*)MemAlloc(sizeof(_type))
#define F_NEWARR(_type, _size)				(_type*)MemAlloc(_size * sizeof(_type))
#define F_RENEW(_type, _ptr, _size)			(_type*)MemRealloc(_ptr, _size)
#define F_DELETE(_ptr) 						MemFree(_ptr)


/* log 输出 */
#if (F_ENABLE_DEBUG)
static const char f_strLog_message[][10] = {{"VERBOSE"}, {"TRACE"}, {"DEBUG"}, {"INFOR"}, {"ERROR"}};
#define F_LOG(level, format, ...) \
	do {\
    	if (level >= F_LOG_LEVEL) {\
			fprintf(stderr, "[%s In function (%s)] " format, \
				f_strLog_message[level], __func__, ##__VA_ARGS__); \
    	}\
    } while (0)
#else
#define F_LOG(level, format, ...)
#endif

/* 断言 */
#if (F_ENABLE_ASSERT)
#define F_ASSERT(_expression) \
	do {\
		if (!(_expression)) {\
			F_LOG(F_LOG_ERROR, "F_ASSERT:(%s)\nExiting...\n", #_expression);\
			exit(1);\
		}\
	} while (0)
#else
#define F_ASSERT(_expression)
#endif

/* 契约式设计(DbC)条件 */
//检验前置条件
#define F_REQUIRE(_expression)		F_ASSERT(_expression)
//检验后置条件
#define F_ENSURE(_expression)		F_ASSERT(_expression)
//不变的条件
#define F_INVARIANT(_expression)	F_ASSERT(_expression)

#ifdef __cplusplus
}
#endif

#endif //__INCLUDES_H__
