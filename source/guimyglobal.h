/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guimyglobal.h
* 摘    要：  声明一些公共函数
*             
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


#ifndef _GUI_MYGLOBAL_H
#define _GUI_MYGLOBAL_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"
#include "guibase.h"
#include <unistd.h>
#include <stdio.h>

#define BYTE_LEN    8

/*
  定义格式化输出函数的宏标志
*/
#ifndef GUI_PRINTF_MACRO
#define GUI_PRINTF_MACRO    1
#endif

//根据格式化输出函数宏标志决定是否输出
#if GUI_PRINTF_MACRO
#define PRINT_INF(inf)  printf(#inf "\n")
#define D_PRINT(exp)    printf(#exp " = %d\n", exp)
#define F_PRINT(exp)    printf(#exp " = %f\n", exp)
#define S_PRINT(exp)    printf(#exp " = %s\n", exp)
#define X_PRINT(exp)    printf(#exp " = %x\n", exp)
#else
#define PRINT_INF(inf)  ;
#define D_PRINT(exp)    ;
#define F_PRINT(exp)    ;
#define S_PRINT(exp)    ;
#define S_PRINT(exp)    ;
#endif


//定义与链表相关的数据结构
typedef struct _queue
{
    void * pContent;            //链表成员的内容
    struct _queue * pPrevious;  //上一个链表成员
    struct _queue * pNext;      //下一个链表成员
} __attribute__((packed))GUIQUEUE;

typedef char * STRING;


/*********************************************************************************************************
* 函数声明
*********************************************************************************************************/

//void * GuiMemAlloc(long lSize);

//long GuiMemFree(void * pMem);

//char *_usrstrcpy(char *des, const char *src);

//unsigned int GetTotalLength(void);

//GUICHAR *GetDropListTextResource(unsigned int uiText[][2], unsigned int uiIndex, char **pString);

//GUICHAR *mySprintf1(char *String1, unsigned int uiText[][2], unsigned int uiIndex, char **pString);

int RegularSeach(char *pString);

/***
  * 功能：
        实现对popen函数的二次封装
  * 参数：
        1.char *pCmd:	需要执行的命令行
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int mysystem(char *pCmd);

#endif  //_WND_FRMSETTING_H
