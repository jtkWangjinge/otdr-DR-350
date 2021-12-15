/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibase.h
* 摘    要：  包含GUI中使用的系统头文件，定义GUI的基本数据结构和声明相应的基本
*             操作函数，定义GUI中所使用的回调函数类型为其他模块的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_BASE_H
#define _GUI_BASE_H


/********************************
* 为实现GUI而需要引用的标准头文件
********************************/
#include <stddef.h>
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


/************************************
* 为定义GUIBASE而需要引用的其他头文件
************************************/
#include "guiconfig.h"

#include  "app_memory_detecter.h"


/****************************
* 定义GUI所使用的基本数据类型
****************************/    
typedef signed char		INT8;
typedef signed short 	INT16;
typedef signed int		INT32;
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
//diff start
typedef unsigned long long UINT64;
//diff end
typedef unsigned int	COLOR;			//由高到低为ARGB格式，目前尚不支持Alpha分量
typedef unsigned short	GUICHAR;		//为支持多国语言，GUI中使用宽字符

//定义GUI中使用的透明色
#define COLOR_TRANS     0xFFFFFFFF  //全透明色，ARGB格式

//转化C字符串为GUI字符串
GUICHAR* TransString(char *pString);
//字符串粘贴，功能类似于标准C库的strcat()
GUICHAR* StringCat(GUICHAR **ppDest, GUICHAR *pSrc);
//字符串复制，功能类似于标准C库的strcpy()
GUICHAR* StringCpy(GUICHAR **ppDest, GUICHAR *pSrc);
//字符串比较，功能类似于标准C库的strcmp()
int StringCmp(GUICHAR *pS1, GUICHAR *pS2);

//diff start 
/***
  * 功能：
        将UTF-8编码字符串转换成unicode编码字符串
  * 参数：
        1.const unsigned char *pUtf8Code:    指向UTF-8编码字符串
  * 返回：
        成功返回unicode编码字符串，以\0结束
        失败返回NULL
  * 备注：add by  2014.12.31
***/
unsigned short* GetUtf8ToUcs2(const char *pUtf8Code);

/***
  * 功能：
        将UCS-2编码字符串转换成UTF-8编码字符串
  * 参数：
        1.unsigned char *pUcs2Code:    指向UCS-2编码字符串
        2.unsigned char *pUtf8Code:	   指向输出的用于存储UTF8编码值的缓冲区的指针 
  * 返回：
        成功返回0
        失败返回非零值
  * 备注：add by  2014.12.31
***/
int GetUcs2ToUtf8(unsigned short *pUcs2Code, char *pUtf8Code);


/***
  * 功能：
        转化unicode字符串为GUI字符串
  * 参数：
        1.unsigned short  *pString:    要转换的C字符串
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：add by  2014.12.31
***/
GUICHAR* GetUnicode(unsigned short *pString);

/***
  * 功能：
        字符串粘贴，功能类似于标准C库的strcat()
  * 参数：
        1.GUICHAR *pDest: 指向目标字符串，不能为空
        2.GUICHAR *pSrc:    源字符串，不能为空
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：add by  2014.12.31
***/
GUICHAR* StringCat1(GUICHAR *pDest, GUICHAR *pSrc);

/***
  * 功能：
        字符串复制，功能类似于标准C库的strcpy()
  * 参数：
        1.GUICHAR *ppDest:  指向目标字符串，不能为空
        2.GUICHAR *pSrc:    源字符串，不能为空
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：add by  2014.12.31
***/
GUICHAR* StringCpy1(GUICHAR *pDest, GUICHAR *pSrc);

/***
  * 功能：
        计算宽字符串的字符数，不包含\0
  * 参数：
        1.GUICHAR *pStr:  指向宽符串
  * 返回：
        返回宽字符串的长度
  * 备注：add by  2014.12.31
***/
unsigned int StringStrlen(GUICHAR *pStr);
//diff end

/***
  * 功能：
        去掉宽字符串前后的空格
  * 参数：
        1.GUICHAR *pStr:  指向宽符串
  * 返回：
        0：表示ok
        -1：表示输入全为空格
  * 备注：
***/
int StringUcs2TrimSpace(GUICHAR *pSrc);

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


/********************************
* 定义GUI中回调函数的调用接口类型
********************************/
//定义GUI中回调函数使用的参数结构
typedef struct _hookpar
{
    int ret;
    void *inpar;
    int inlen;
    void *outpar;
    int outlen;
} HOOKPAR;

//定义GUI中回调函数的调用接口类型
typedef void* (*HOOKFUNC)(HOOKPAR *);


/************************
* 定义GUI中最基本的点结构
************************/
typedef struct _point
{
    unsigned int x;     //横坐标
    unsigned int y;     //纵坐标
} GUIPOINT;


/****************************************
* 定义GUI中最基本的矩形结构及相关操作函数
****************************************/
//定义GUI中的矩形结构
typedef struct _rectangle
{
    GUIPOINT Start;     //矩形起点，即左上角横纵坐标最小的点
    GUIPOINT End;       //矩形终点，即右下角横纵坐标最大的点
} GUIRECTANGLE;

//定义GUI中的矩形关系(枚举类型)
typedef enum _rectangle_relation
{
    enuSeparate    = 0, //矩形之间彼此是分离的
    enuOverlay     = 1, //矩形之间存在完全覆盖
    enuPortion     = 2, //矩形之间存在完全包含
    enuOverlap     = 3  //矩形之间有部分重叠
} GUIRECTANGLE_RELA;

//矩形关系运算
GUIRECTANGLE_RELA RectangleRelation(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2);

//判断矩形间是否分离
int RectangleSeparate(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2);
//判断矩形间是否完全覆盖
int RectangleOverlay(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2);
//判断矩形间是否完全包含
int RectanglePortion(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2);
//判断矩形间是否部分层叠
int RectangleOverlap(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2);


/******************************
* 定义GUI中最基本的可视控件结构
******************************/
//定义GUI可视控件结构中对象删除函数的调用接口类型
typedef int (*DESTFUNC) (void **);
//定义GUI可视控件结构中显示输出函数的调用接口类型
typedef int (*DISPFUNC)(void *);

//定义GUI中最基本的可视控件结构
typedef struct _visible
{
    GUIRECTANGLE Area;  //可视控件的有效区域
    int iEnable;        //可视控件是否可见，0不可见，1可见
    int iLayer;         //可视控件所处的图层，其值必须>=0，0为最底层

    int iFocus;         //可视控件是否具有焦点，0无焦点，1有焦点
    int iCursor;        //可视控件是否具有光标，0无光标，1有光标

    GUIRECTANGLE Hide;  //可视控件的被覆盖区域
    void *pResume;      //保存被覆盖区域的图像，用于恢复显示

    DESTFUNC fnDestroy; //可视控件的对象删除函数
    DISPFUNC fnDisplay; //可视控件的显示输出函数
} GUIVISIBLE;


/**********************************
* 定义GUI中使用的线程及相关操作函数
**********************************/
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_t,pthread_attr_t,THREADFUNC
//定义GUI中的线程类型
typedef pthread_t GUITHREAD;
//定义GUI中的线程属性类型 
typedef pthread_attr_t GUITHREAD_ATTR;
//定义GUI中线程函数的调用接口类型
typedef void* (*THREADFUNC)(void *arg);
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif    //GUI_OS_ENV == LINUX_OS

//创建新的线程
int ThreadCreate(GUITHREAD *pThread, GUITHREAD_ATTR *pAttr,
                 THREADFUNC fnEntry, void *pArg);
//退出当前线程
int ThreadExit(void *pReturn);
//等待线程结束
int ThreadJoin(GUITHREAD Thread, void **ppReturn);
//取消线程
int ThreadCancel(GUITHREAD Thread);
//设置线程取消点
int ThreadTestcancel(void);


/************************************
* 定义GUI中使用的互斥锁及相关操作函数
************************************/
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_t,pthread_mutexattr_t
//定义GUI中的线程锁类型
typedef pthread_mutex_t GUIMUTEX;
//定义GUI中的线程锁属性类型
typedef pthread_mutexattr_t GUIMUTEX_ATTR;
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif    //GUI_OS_ENV == LINUX_OS

//初始化锁
int InitMutex(GUIMUTEX *pMutex, GUIMUTEX_ATTR *pAttr);
//加锁操作
int MutexLock(GUIMUTEX *pMutex);
//测试加锁
int MutexTrylock(GUIMUTEX *pMutex);
//解锁操作
int MutexUnlock(GUIMUTEX *pMutex);
//释放锁操作
int MutexDestroy(GUIMUTEX *pMutex);

/************************
* 定义GUI中使用的休眠函数
************************/
//休眠若干毫秒
int MsecSleep(unsigned long ulMsec);


/**************************************
* 定义GUI中使用的列表结构及相关操作函数
**************************************/
//定义GUI列表结构中使用的列表项
typedef struct _list_item
{
    struct _list *pOwner;       //指向列表头
    struct _list_item *pNext;   //下一个列表项
    struct _list_item *pPrev;   //前一个列表项
} GUILIST_ITEM;

//定义GUI中的列表结构
typedef struct _list
{
    int iCount;                 //列表项计数，0 <= iCount <= iLimit
    int iLimit;                 //列表项限制，iLimit >= iCount >= 0
    GUILIST_ITEM *pHead;        //指向列表头
    GUILIST_ITEM *pTail;        //指向列表尾
} GUILIST;

//创建列表
GUILIST* CreateList(int iLimit);
//删除列表
int DestroyList(GUILIST *pList);

//初始化列表
inline int InitList(GUILIST *pList);

//添加一列表项到列表尾端
int ListAdd(GUILIST_ITEM *pItem, GUILIST *pList);
//添加一列表项到列表头部
int ListAddToHead(GUILIST_ITEM *pItem, GUILIST *pList);
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

/***
  * 功能：
        返回pList中含有的项目数
  * 参数：
        1.CGList *pList:    指针，指向由GreateList建立的列表对象
  * 返回：
        返回pList中含有的项目数
  * 备注：
***/
inline int ListCount(GUILIST *pList);

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
    int iLength;        //对象长度，GUIOBJ中所包含对象的长度(主要用于窗体控件)
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
#define container_of(ptr, type, member)                         \
({                                                              \
    const typeof(((type *)0)->member) *__mptr = (ptr);          \
    (type *)((char *)__mptr - ((size_t)&((type *)0)->member));  \
})


/************************************
* 定义GUI中用于遍历控件队列的控件对象
************************************/
typedef struct _component
{
    GUIVISIBLE Visible;
} GUICOMPONENT;


#endif  //_GUI_BASE_H

