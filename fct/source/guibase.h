/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibase.h
* 摘    要：  包含GUI中使用的系统头文件，定义GUI的基本数据结构和声明相应的基本
*             操作函数，定义GUI中所使用的回调函数类型为其他模块的实现提供基础。
*
* 当前版本：  v1.0.1 (考虑到跨平台，新增了对于GUI_OS_ENV的预编译检验)
* 作    者：  wjg
* 完成日期：  2012-10-11
*
* 取代版本：  v1.0.0
* 原 作 者：  yexin.zhu
* 完成日期：  2012-7-24
*******************************************************************************/

#ifndef _GUI_BASE_H
#define _GUI_BASE_H


/********************************
* 为实现GUI而需要引用的标准头文件
********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/********************************
* 为实现GUI而需要引用的系统头文件
********************************/
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>


/****************************
* 定义GUI所使用的基本数据类型
****************************/
typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned int COLOR;         //由高到低为ARGB格式，目前尚不支持Alpha分量
typedef unsigned short GUICHAR;     //为支持多国语言，GUI中使用宽字符

//转化C字符串为GUI字符串
GUICHAR* TransString(char *pString);
//字符串粘贴，功能类似于标准C库的strcat()
GUICHAR* StringCat(GUICHAR **ppDest, GUICHAR *pSrc);
//字符串复制，功能类似于标准C库的strcpy()
GUICHAR* StringCpy(GUICHAR **ppDest, GUICHAR *pSrc);
//字符串比较，功能类似于标准C库的strcmp()
int StringCmp(GUICHAR *pS1, GUICHAR *pS2);


/******************************
* 定义GUI所使用的求位掩码宏函数
******************************/
#define bitmask_high(n, bits)   (((1 << (n)) - 1) << ((bits) - (n)))
#define bitmask_low(n, bits)    ((1 << (n)) - 1)


/******************************
* 定义GUI所使用的圆整对齐宏函数
******************************/
#define round_up(x, align)      (((x) + (align) - 1) & ~((align) - 1))
#define round_down(x, align)    ((x) & ~((align) - 1))


/****************************
* 定义GUI中回调函数的调用接口
****************************/
typedef struct _hookpar
{
    int ret;
    void *inpar;
    int inlen;
    void *outpar;
    int outlen;
} HOOKPAR;

typedef void* (*HOOKFUNC)(HOOKPAR *);


/************************
* 定义GUI中最基本的点结构
************************/
typedef struct _point
{
    unsigned int x;     //横坐标
    unsigned int y;     //纵坐标
} GUIPOINT;


/**************************
* 定义GUI中最基本的矩形结构
**************************/
typedef struct _rectangle
{
    GUIPOINT Start;     //矩形起点，即左上角横纵坐标最小的点
    GUIPOINT End;       //矩形终点，即右下角横纵坐标最大的点
} GUIRECTANGLE;


/******************************
* 定义GUI中最基本的可视控件结构
******************************/
typedef struct _visible
{
    GUIRECTANGLE Area;  //可视控件的有效区域
    int iEnable;        //可视控件是否可见，0不可见，1可见
    int iFocus;         //可视控件是否具有焦点，0无焦点，1有焦点
    int iLayer;         //可视控件所处的图层，其值必须>=0
} GUIVISIBLE;


/****************************
* 定义GUI所依赖的操作系统环境
****************************/
#define NONE_OS             0           //无操作系统
#define LINUX_OS            1           //Linux操作系统
#define GUI_OS_ENV          LINUX_OS    //GUI当前所处的操作系统环境


/**********************************
* 定义GUI中使用的线程及相关操作函数
**********************************/
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_t,THREADFUNC
typedef pthread_t GUITHREAD;
typedef void* (*THREADFUNC)(void *arg);
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif

//创建新的线程
int ThreadCreate(GUITHREAD *pThread, THREADFUNC fnEntry, void *pArg);
//退出当前线程
int ThreadExit(void *pReturn);
//等待线程结束
int ThreadJoin(GUITHREAD Thread, void **ppReturn);


/************************************
* 定义GUI中使用的互斥锁及相关操作函数
************************************/
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_t
typedef pthread_mutex_t GUIMUTEX;
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif

//初始化锁
int InitMutex(GUIMUTEX *pMutex);
//加锁操作
int MutexLock(GUIMUTEX *pMutex);
//测试加锁
int MutexTrylock(GUIMUTEX *pMutex);
//解锁操作
int MutexUnlock(GUIMUTEX *pMutex);


/************************
* 定义GUI中使用的休眠函数
************************/
//休眠若干毫秒
int MsecSleep(unsigned long ulMsec);


/**************************************
* 定义GUI中使用的列表结构及相关操作函数
**************************************/
typedef struct _list_item
{
    struct _list *pOwner;           //指向列表头
    struct _list_item *pNext;       //下一个列表项
    struct _list_item *pPrev;       //前一个列表项
} GUILIST_ITEM;

typedef struct _list
{
    int iCount;                     //列表项计数，0 <= iCount <= iLimit
    int iLimit;                     //列表项限制，iLimit >= iCount >= 0
    struct _list_item *pHead;       //指向列表头
    struct _list_item *pTail;       //指向列表尾
} GUILIST;

//创建列表
GUILIST* CreateList(int iLimit);
//删除列表
int DestroyList(GUILIST *pList);

//初始化列表
inline int InitList(GUILIST *pList);

//添加一列表项到列表尾端
int ListAdd(GUILIST_ITEM *pItem, GUILIST *pList);
//从列表中删除一列表项
int ListDel(GUILIST_ITEM *pItem, GUILIST *pList);
//从一个列表中移除一列表项到另一列表尾端
int ListMove(GUILIST_ITEM *pItem, GUILIST *pList);
//合并列表到另一列表尾端
int ListSplice(GUILIST *pAttach, GUILIST *pList);

//判断列表是否为空
inline int ListEmpty(GUILIST *pList);
//判断列表是否为满
inline int ListFull(GUILIST *pList);


//后向遍历列表的宏函数
#define list_next_each(item, list, pos)                                     \
    for (item = (list)->pHead, pos = (list)->iCount; pos--; item = item->pNext)
//前向遍历列表的宏函数
#define list_prev_each(item, list, pos)                                     \
    for (item = (list)->pTail, pos = (list)->iCount; pos--; item = item->pPrev)


/**********************************
* 定义GUI中最基本的包容器对象GUIOBJ
**********************************/
typedef struct _object
{
    GUILIST_ITEM List;  //相邻对象，指向同一集合中相邻的另一个GUIOBJ
    int iType;          //对象类型，GUIOBJ中所包含对象的类型(主要用于窗体控件)
    int iLength;        //对象长度，GUIOBJ中所包含对象的长度
    void *pObj;         //包含对象，所包含的特定GUI对象
} GUIOBJ;

//定义在GUIOBJ中使用的对象类型(主要为窗体控件)
#define OBJTYP_GUIBUTTON        1
#define OBJTYP_GUICAPTION       2
#define OBJTYP_GUILABEL         3
#define OBJTYP_GUIMENU          4
#define OBJTYP_GUIPICTURE       5
#define OBJTYP_GUISCROLL        6
#define OBJTYP_GUISTATUS        7
#define OBJTYP_GUIWINDOW        8

//用于获取包容器对象的宏函数
#define container_of(ptr, type, member)                                     \
({                                                                          \
    const typeof(((type *)0)->member) *__mptr = (ptr);                      \
    (type *)((char *)__mptr - ((unsigned int)&((type *)0)->member));        \
})


/************************************
* 定义GUI中用于遍历控件队列的控件对象
************************************/
typedef struct _component
{
    GUIVISIBLE Visible;
} GUICOMPONENT;


#endif  //_GUI_BASE_H

