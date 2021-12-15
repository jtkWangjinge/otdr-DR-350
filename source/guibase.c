/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibase.c
* 摘    要：  实现GUI基本数据类型的操作函数，为其他模块提供支持
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guibase.h"
#include "app_text.h"

/***
  * 功能：
        转化C字符串为GUI字符串
  * 参数：
        1.char *pString:    要转换的C字符串
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
***/
GUICHAR* TransString(char *pString)
{
    //错误标志、返回值定义
    GUICHAR *pReturn = NULL;
    //临时变量定义
    char *pTmp = NULL;
    int iSize;

    //参数检查
    if (NULL == pString)
    {
        return NULL;
    }

	/* start add by  2015.2.10 */
	pReturn = GetUcs2FromUtf8(pString);
	return pReturn;
	/* end */

    //统计字符串长度，包含'\0'
    for (pTmp = pString, iSize = 1; *pTmp; pTmp++)
    {
        iSize++;
    }

    //尝试为字符串内容分配内存
    pReturn = (GUICHAR *) malloc(iSize * sizeof(GUICHAR));
    if (NULL == pReturn)
    {
        return NULL;
    }

    //逐个字符进行转换
    while (--iSize >= 0)
    {
        pReturn[iSize] = pString[iSize];
    }

    return pReturn;
}


/***
  * 功能：
        字符串粘贴，功能类似于标准C库的strcat()
  * 参数：
        1.GUICHAR **ppDest: 指向目标字符串，不能为空
        2.GUICHAR *pSrc:    源字符串，不能为空
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
***/
GUICHAR* StringCat(GUICHAR **ppDest, GUICHAR *pSrc)
{
    //临时变量定义
    GUICHAR *pTmp = NULL;
    int iDest, iSrc;

    //参数检查
    if (NULL == ppDest || NULL == pSrc)
    {
        return NULL;
    }

    //判断ppDest所指向的是否为有效指针
    if (NULL == *ppDest)
    {
        return NULL;
    }

    //统计目标字符串与源字符串长度，不包含'\0'
    for (pTmp = *ppDest, iDest = 0; *pTmp; pTmp++)
    {
        iDest++;
    }
    for (pTmp = pSrc, iSrc = 0; *pTmp; pTmp++)
    {
        iSrc++;
    }

    //考虑当源为空字符串的情况
    if (0 == iSrc)
    {
        return *ppDest;
    }

    //分配目的字符串存储空间
    pTmp = (GUICHAR *) malloc((iDest + iSrc + 1) * sizeof(GUICHAR));
    if (NULL == pTmp)
    {
        return NULL;
    }

    //完成字符串粘贴
    if (iDest)
    {
        memcpy(pTmp, *ppDest, iDest * sizeof(GUICHAR));
    }
    memcpy(pTmp + iDest, pSrc, (iSrc + 1) * sizeof(GUICHAR));

    //释放旧的目标字符串内容，并重新定向目标字符串
    free(*ppDest);
    *ppDest = pTmp;

    return *ppDest;
}


/***
  * 功能：
        字符串复制，功能类似于标准C库的strcpy()
  * 参数：
        1.GUICHAR **ppDest: 指向目标字符串，不能为空
        2.GUICHAR *pSrc:    源字符串，不能为空
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
***/
GUICHAR* StringCpy(GUICHAR **ppDest, GUICHAR *pSrc)
{
    //临时变量定义
    GUICHAR *pTmp = NULL;
    int iSize;

    //参数检查
    if (NULL == ppDest || NULL == pSrc)
    {
        return NULL;
    }

    //判断ppDest所指向的是否为有效指针
    if (NULL == *ppDest)
    {
        return NULL;
    }

    //统计源字符串长度，包含'\0'
    for (pTmp = pSrc, iSize = 1; *pTmp; pTmp++)
    {
        iSize++;
    }

    //分配目的字符串存储空间
    pTmp = (GUICHAR *) malloc(iSize * sizeof(GUICHAR));
    if (NULL == pTmp)
    {
        return NULL;
    }

    //完成字符串复制
    memcpy(pTmp, pSrc, iSize * sizeof(GUICHAR));

    //释放旧的目标字符串内容，并重新定向目标字符串
    free(*ppDest);
    *ppDest = pTmp;

    return *ppDest;
}


/***
  * 功能：
        字符串比较，功能类似于标准C库的strcmp()
  * 参数：
        1.GUICHAR *pS1: 要比较的字符串1，不能为空
        2.GUICHAR *pS2: 要比较的字符串2，不能为空
  * 返回：
        pS1<pS2，返回负数；pS1>pS2，返回正数；相等，返回零
  * 备注：
***/
int StringCmp(GUICHAR *pS1, GUICHAR *pS2)
{
    //错误标志、返回值定义
    int iCmp = 0;
    //临时变量定义
    GUICHAR *a = NULL, *b = NULL;

    //判断pS1和pS2是否为有效指针
    if (NULL == pS1 || NULL == pS2)
    {
        return 0;
    }

    //比较字符串
    for (a = pS1, b = pS2; !(iCmp = *a - *b); a++, b++)
    {
        if (0 == *a || 0 == *b) //字符串以'\0'结尾
        {
            break;
        }
    }

    return iCmp;
}


/***
  * 功能：
        比较两个矩形，进行矩形关系运算
  * 参数：
        1.GUIRECTANGLE *pS1:    要比较的矩形1，不能为空
        2.GUIRECTANGLE *pS2:    要比较的矩形2，不能为空
  * 返回：
        返回矩形关系枚举值
  * 备注：
***/
GUIRECTANGLE_RELA RectangleCompare(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2)
{
    if (RectangleSeparate(pR1, pR2))        //判断是否彼此分离
    {
        return enuSeparate;
    }
    else
    {
        if (RectangleOverlay(pR1, pR2))     //判断是否完全覆盖
        {
            return enuOverlay;
        }
        if (RectanglePortion(pR1, pR2))     //判断是否完全包含
        {
            return enuPortion;
        }
    }

    return enuOverlap;
}


/***
  * 功能：
        比较矩形pR1与矩形pR2是否分离，分离返回1，否则返回0
  * 参数：
        1.GUIRECTANGLE *pS1:    要比较的矩形1，不能为空
        2.GUIRECTANGLE *pS2:    要比较的矩形2，不能为空
  * 返回：
        分离返回1，否则返回0
  * 备注：
***/
int RectangleSeparate(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2)
{
    //临时变量定义
    GUIPOINT p;

    //检查pR1的四个端点是否位于pR2内
    p.x = pR1->Start.x;
    p.y = pR1->Start.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1左上角是否位于pR2内
    {
        return 0;
    }
    p.x = pR1->End.x;
    p.y = pR1->Start.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1右上角是否位于pR2内
    {
        return 0;
    }
    p.x = pR1->Start.x;
    p.y = pR1->End.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1左下角是否位于pR2内
    {
        return 0;
    }
    p.x = pR1->End.x;
    p.y = pR1->End.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1右下角是否位于pR2内
    {
        return 0;
    }

    //检查pR2的四个端点是否位于pR1内
    p.x = pR2->Start.x;
    p.y = pR2->Start.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2左上角是否位于pR1内
    {
        return 0;
    }
    p.x = pR2->End.x;
    p.y = pR2->Start.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2右上角是否位于pR1内
    {
        return 0;
    }
    p.x = pR2->Start.x;
    p.y = pR2->End.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2左下角是否位于pR1内
    {
        return 0;
    }
    p.x = pR2->End.x;
    p.y = pR2->End.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2右下角是否位于pR1内
    {
        return 0;
    }

    return 1;
}


/***
  * 功能：
        比较矩形pR1与矩形pR2是否完全覆盖，pR1完全覆盖pR2返回1，否则返回0
  * 参数：
        1.GUIRECTANGLE *pS1:    要比较的矩形1，不能为空
        2.GUIRECTANGLE *pS2:    要比较的矩形2，不能为空
  * 返回：
        完全覆盖返回1，否则返回0
  * 备注：
***/
int RectangleOverlay(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2)
{
    if ((pR1->Start.x <= pR2->Start.x) &&
        (pR1->Start.y <= pR2->Start.y) &&
        (pR1->End.x >= pR2->End.x) &&
        (pR1->End.y >= pR2->End.y))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/***
  * 功能：
        比较矩形pR1与矩形pR2是否完全包含，pR1完全被pR2包含返回1，否则返回0
  * 参数：
        1.GUIRECTANGLE *pS1:    要比较的矩形1，不能为空
        2.GUIRECTANGLE *pS2:    要比较的矩形2，不能为空
  * 返回：
        完全包含返回1，否则返回0
  * 备注：
***/
int RectanglePortion(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2)
{
    if ((pR1->Start.x >= pR2->Start.x) &&
        (pR1->Start.y >= pR2->Start.y) &&
        (pR1->End.x <= pR2->End.x) &&
        (pR1->End.y <= pR2->End.y))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/***
  * 功能：
        比较矩形pR1与矩形pR2是否部分重叠，部分重叠返回1，否则返回0
  * 参数：
        1.GUIRECTANGLE *pS1:    要比较的矩形1，不能为空
        2.GUIRECTANGLE *pS2:    要比较的矩形2，不能为空
  * 返回：
        部分重叠返回1，否则返回0
  * 备注：
***/
int RectangleOverlap(GUIRECTANGLE *pR1, GUIRECTANGLE *pR2)
{
    //临时变量定义
    GUIPOINT p;

    //检查pR1的四个端点是否位于pR2内
    p.x = pR1->Start.x;
    p.y = pR1->Start.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1左上角是否位于pR2内
    {
        return 1;
    }
    p.x = pR1->End.x;
    p.y = pR1->Start.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1右上角是否位于pR2内
    {
        return 1;
    }
    p.x = pR1->Start.x;
    p.y = pR1->End.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1左下角是否位于pR2内
    {
        return 1;
    }
    p.x = pR1->End.x;
    p.y = pR1->End.y;
    if ((p.x >= pR2->Start.x) &&
        (p.x <= pR2->End.x) &&
        (p.y >= pR2->Start.y) &&
        (p.y <= pR2->End.y))    //检查pR1右下角是否位于pR2内
    {
        return 1;
    }

    //检查pR2的四个端点是否位于pR1内
    p.x = pR2->Start.x;
    p.y = pR2->Start.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2左上角是否位于pR1内
    {
        return 1;
    }
    p.x = pR2->End.x;
    p.y = pR2->Start.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2右上角是否位于pR1内
    {
        return 1;
    }
    p.x = pR2->Start.x;
    p.y = pR2->End.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2左下角是否位于pR1内
    {
        return 1;
    }
    p.x = pR2->End.x;
    p.y = pR2->End.y;
    if ((p.x >= pR1->Start.x) &&
        (p.x <= pR1->End.x) &&
        (p.y >= pR1->Start.y) &&
        (p.y <= pR1->End.y))    //检查pR2右下角是否位于pR1内
    {
        return 1;
    }

    return 0;
}


/***
  * 功能：
        创建新的线程
  * 参数：
        1.GUITHREAD *pThread:       指向创建的新线程的线程ID
        2.GUITHREAD_ATTR *pAttr:    指向新线程的属性
        3.THREADFUNC fnEntry:       创建新线程时指定的线程入口函数
        4.void *pArg:               创建新线程时传递的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ThreadCreate(GUITHREAD *pThread, GUITHREAD_ATTR *pAttr,
                 THREADFUNC fnEntry, void *pArg)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_create()
    iReturn = pthread_create(pThread, pAttr, fnEntry, pArg);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}


/***
  * 功能：
        退出当前线程
  * 参数：
        1.void *pReturn:    线程结束状态
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ThreadExit(void *pReturn)
{
  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_exit()
    pthread_exit(pReturn);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return 0;
}


/***
  * 功能：
        等待线程结束
  * 参数：
        1.GUITHREAD Thread:     指向需要等待的线程的线程ID
        2.void **ppReturn:      指向线程结束状态，用来传给pthread_exit()
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ThreadJoin(GUITHREAD Thread, void **ppReturn)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_join()
    iReturn = pthread_join(Thread, ppReturn);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}

/***
  * 功能：
        取消线程
  * 参数：
        1.GUITHREAD Thread:     指向需要取消的线程的线程ID
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ThreadCancel(GUITHREAD Thread)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_cancel()
    iReturn = pthread_cancel(Thread);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}

/***
  * 功能：
        设置线程取消点
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ThreadTestcancel(void)
{
  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_testcancel()
    pthread_testcancel();
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return 0;
}

/***
  * 功能：
        初始化锁
  * 参数：
        1.GUIMUTEX *pMutex:     指针，指向需要初始化的互斥锁
        2.GUIMUTEX_ATTR *pAttr: 指针，指向互斥锁的属性
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InitMutex(GUIMUTEX *pMutex, GUIMUTEX_ATTR *pAttr)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_init()
    iReturn = pthread_mutex_init(pMutex, pAttr);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}


/***
  * 功能：
        加锁操作
  * 参数：
        1.GUIMUTEX *pMutex:   指针，指向需要被锁上的互斥锁
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int MutexLock(GUIMUTEX *pMutex)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_lock()
    iReturn = pthread_mutex_lock(pMutex);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}


/***
  * 功能：
        测试加锁
  * 参数：
        1.GUIMUTEX *pMutex:   指针，指向需要测试加锁的互斥锁
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int MutexTrylock(GUIMUTEX *pMutex)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_trylock()
    iReturn = pthread_mutex_trylock(pMutex);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}


/***
  * 功能：
        解锁操作
  * 参数：
        1.GUIMUTEX *pMutex:   指针，指向需要测试解锁的互斥锁
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int MutexUnlock(GUIMUTEX *pMutex)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_unlock()
    iReturn = pthread_mutex_unlock(pMutex);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}

/***
  * 功能：
        释放锁操作
  * 参数：
        1.GUIMUTEX *pMutex:   指针，指向需要测试解锁的互斥锁
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int MutexDestroy(GUIMUTEX *pMutex)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_destroy()
    iReturn = pthread_mutex_destroy(pMutex);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}
/***
  * 功能：
        休眠若干毫秒
  * 参数：
        1.unsigned long ulMsec:   需要睡眠的毫秒数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int MsecSleep(unsigned long ulMsec)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:timeval,select()
    struct timeval tv;

    tv.tv_sec = ulMsec / 1000;
    tv.tv_usec = ulMsec % 1000 * 1000;
    iReturn = select(0, NULL, NULL, NULL, &tv);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iReturn;
}


/***
  * 功能：
        新建一个列表，该列表最多容纳iLimit个项目
  * 参数：
        1.int iLimit:  要建立的列表对象所能容纳的项目数，iLimit不能小于1
  * 返回：
        成功返回指向新列表的有效指针，否则返回NULL
  * 备注：
        使用CreateList函数新建的列表对象总是为空。
***/
GUILIST* CreateList(int iLimit)
{
    //错误标志、返回值定义
    GUILIST *pList = NULL;

    //参数检查
    if (iLimit < 0)
    {
        return NULL;    
    }

    //分配资源
    pList = (GUILIST *) malloc(sizeof(GUILIST));
    if (NULL == pList)
    {
        return NULL;    
    }

    //初始化列表
    pList->iCount = 0;
    pList->iLimit = iLimit;
    pList->pHead = NULL;
    pList->pTail = NULL;

    return pList;
}


/***
  * 功能：
        删除列表pList，并释放pList占用的所有资源，成功返回零
  * 参数：
        1.GUILIST *pList:   指针，指向由CreateList建立的列表对象
  * 返回：
        成功返回零，否则返回错误代码
  * 备注：
        DestroyList保证传入参数pList为非空指针，如果列表pList为空，
        DestroyList会返回错误代码-2。
***/
int DestroyList(GUILIST *pList)
{
    //判断pList是否为有效指针
    if (NULL == pList)
    {
        return -1;      
    }

    //判断pList是否为空，若非空则错误，列表项的删除由程序员自己控制
    if (!ListEmpty(pList))
    {
        return -2;
    }

    //销毁列表对象
    free(pList);

    return 0;
}


/***
  * 功能：
        初始化列表pList， 成功返回零，否则返回非零值
  * 参数：
        1.GUILIST *pList:   指针，指向由CreateList建立的列表对象
  * 返回：
        成功返回零，否则返回错误代码
  * 备注：
        初始化pList的iCount = 0、pHead = pTail = NULL
***/
inline int InitList(GUILIST *pList)
{
    //判断pList是否为有效指针
    if (NULL == pList)
    {
        return -1;
    }

    //初始化列表
    pList->iCount = 0;
    pList->pHead = NULL;
    pList->pTail = NULL;

    return 0;
}


/***
  * 功能：
        插入列表项pItem到列表pList尾端
  * 参数：
        1.GUILIST_ITEM *pItem:  指针，指向需要插入的列表项
        2.GUILIST *pList:       指针，指向由CreateList建立的列表对象
  * 返回：
        成功返回零，否则返回错误代码
  * 备注：
        函数ListAdd要求pList所指向的列表未满，并总是将新项目加入到
        列表pList的尾端，如果列表已满，函数出错。
***/
int ListAdd(GUILIST_ITEM *pItem, GUILIST *pList)
{
    //判断pItem和pList是否为有效指针
    if (NULL == pList || NULL == pItem)
    {
        return -1;
    }

    // 判断pList是否为满
    if (ListFull(pList))
    {
        return -2;
    }

    //添加pItem到pList尾端
    if (ListEmpty(pList))
    {
        //列表pList为空时
        pItem->pPrev = pItem;
        pItem->pNext = pItem;
        pList->pHead = pItem;
        pList->pTail = pItem;
    }
    else
    {
        //列表pList不为空时
        pItem->pPrev = pList->pTail;
        pItem->pNext = pList->pHead;
        pList->pTail->pNext = pItem;
        pList->pHead->pPrev = pItem;
        pList->pTail = pItem;
    }
    pItem->pOwner = pList;
    pList->iCount++;

    return 0;
}

/***
  * 功能：
        插入列表项pItem到列表pList顶部
  * 参数：
        1.GUILIST_ITEM *pItem:  指针，指向需要插入的列表项
        2.GUILIST *pList:       指针，指向由CreateList建立的列表对象
  * 返回：
        成功返回零，否则返回错误代码
  * 备注：
        函数ListAddToHead要求pList所指向的列表未满，并总是将新项目加入到
        列表pList的头部，如果列表已满，函数出错。
***/
int ListAddToHead(GUILIST_ITEM *pItem, GUILIST *pList)
{
    //判断pItem和pList是否为有效指针
    if (NULL == pList || NULL == pItem)
    {
        return -1;
    }

    // 判断pList是否为满
    if (ListFull(pList))
    {
        return -2;
    }

    //添加pItem到pList尾端
    if (ListEmpty(pList))
    {
        //列表pList为空时
        pItem->pPrev = pItem;
        pItem->pNext = pItem;
        pList->pHead = pItem;
        pList->pTail = pItem;
    }
    else
    {
        //列表pList不为空时
        pItem->pPrev = pList->pTail;
        pItem->pNext = pList->pHead;
        pList->pTail->pNext = pItem;
        pList->pHead->pPrev = pItem;
        pList->pHead = pItem;
    }
    pItem->pOwner = pList;
    pList->iCount++;

    return 0;
}

/***
  * 功能：
        从列表pList中删除列表项pItem
  * 参数：
        1.GUIITEM *pItem:   指针，指向需要删除的列表项
        2.GUILIST *pList:   指针，指向由CreateList建立的列表对象
  * 返回：
        成功返回零，否则返回错误代码
  * 备注：
        函数ListAdd要求pList所指向的列表未空，如果列表已空，则函数出错。
***/
int ListDel(GUILIST_ITEM *pItem, GUILIST *pList)
{
    //判断pItem和pList是否为有效指针
    if (NULL == pList || NULL == pItem)
    {
        return -1;
    }

    //判断pItem是否在pList中和pList是否为空
    if ((pItem->pOwner != pList) || 
        ListEmpty(pList))
    {
        return -2;
    }

    //删除pList中pItem
    if (1 == pList->iCount)
    {
        //列表中只有一个列表项
        pList->pHead = NULL;
        pList->pTail = NULL;
    }
    else
    {
        //列表中有多个列表项
        pItem->pNext->pPrev = pItem->pPrev;
        pItem->pPrev->pNext = pItem->pNext;
        if (pItem == pList->pHead)
        {
            pList->pHead = pItem->pNext;
        }
        else if (pItem == pList->pTail)
        {
            pList->pTail = pItem->pPrev;
        }
    }

    //删除后，置pItem各成员为NULL
    pItem->pOwner = NULL;
    pItem->pNext = NULL;
    pItem->pPrev = NULL;
    pList->iCount--;

    return 0;
}


/***
  * 功能：
        把一个列表中列表项pItem移到另一个列表尾端
  * 参数：
        1.GUIITEM *pItem:   指针，指向需要移动的列表项
        2.GUILIST *pList:   指针，指向由CreateList建立的列表对象
  * 返回：
        成功返回零，否则返回错误代码
  * 备注：
***/
int ListMove(GUILIST_ITEM *pItem, GUILIST *pList)
{
    //判断pItem和pList是否为有效指针
    if (NULL == pList || NULL == pItem)
    {
        return -1;
    }

    //判断pList是否为满
    if (ListFull(pList))
    {
        return -2;
    }

    //从pItem->pOwner删除列表项pItem
    if (ListDel(pItem, pItem->pOwner))
    {
        return -3;
    }

    //添加pItem到pList尾端
    ListAdd(pItem, pList);

    return 0;
}


/***
  * 功能：
        合并列表pAttach到另一列表pList的尾端并初始化原列表  
  * 参数：
        1.GUILIST *pAttach: 指针，指向需要合并的列表对象
        2.GUILIST *pList:   指针，指向由CreateList建立的列表对象
  * 返回：
        已空返回0，失败返回非零
  * 备注：
        需保证pAttach不为空，若为空则返回错误代码-2，
        且pList合并pAttach后需要未满,即pAttach->iCount加上pList->iCount要不大
        于pList->iLimit。
***/
int ListSplice(GUILIST *pAttach, GUILIST *pList)
{
    //临时变量定义
    int iCnt;
    GUILIST_ITEM *pTmp = NULL;

    //判断pAttach和pList是否为有效指针
    if (NULL == pList || NULL == pAttach)
    {
        return -1;
    }

    //判断pAttach是否为空和pList是否未满
    if (ListEmpty(pAttach) || 
        ListFull(pList))
    {
        return -2;
    }

    //判断合并后pList长度大于iLimit
    if (pList->iCount + pAttach->iCount > pList->iLimit)
    {
        return -3;
    }

    //合并pAttach到pList尾端并增加iCount
    if (pList->iCount)
    {
        pAttach->pHead->pPrev = pList->pTail;
        pList->pTail->pNext = pAttach->pHead;
        pAttach->pTail->pNext = pList->pHead;
        pList->pHead->pPrev = pAttach->pTail;
        pList->pTail = pAttach->pTail;
    }
    else
    {
        pList->pHead = pAttach->pHead;
        pList->pTail = pAttach->pTail;
    }
    pList->iCount +=  pAttach->iCount;

    iCnt = pAttach->iCount;
    pTmp = pAttach->pHead;

    //遍历pAttach改变其列表项的pOwner
    while (iCnt--)
    {
        pTmp->pOwner = pList;
        pTmp = pTmp->pNext;
    }

    //初始化pAttach
    InitList(pAttach);

    return 0;
}


/***
  * 功能：
        检测pList是否为空，已空返回1，非空返回0
  * 参数：
        1.CGList *pList:    指针，指向由GreateList建立的列表对象
  * 返回：
        已空返回1，非空返回0
  * 备注：
***/
inline int ListEmpty(GUILIST *pList)
{
    return !(pList->iCount);
}


/***
  * 功能：
        检测pList是否为满，已满返回1，未满返回0
  * 参数：
        1.CGList *pList:    指针，指向由GreateList建立的列表对象
  * 返回：
        已满返回1，未满返回0
  * 备注：
***/
inline int ListFull(GUILIST *pList)
{
    return pList->iCount >= pList->iLimit;
}


/***
  * 功能：
        返回pList中含有的项目数
  * 参数：
        1.CGList *pList:    指针，指向由GreateList建立的列表对象
  * 返回：
        返回pList中含有的项目数
  * 备注：
***/
inline int ListCount(GUILIST *pList)
{
    return (pList->iCount);
}


//diff start
/***
  * 功能：
        将UTF-8编码字符串转换成unicode编码字符串
  * 参数：
        1.const unsigned char *pUtf8Code:    指向UTF-8编码字符串
  * 返回：
        成功返回unicode编码字符串指针，以\0结束
        失败返回NULL
  * 备注：add by  2014.12.31
***/
unsigned short* GetUtf8ToUcs2(const char *pUtf8Code)
{
	int i = 0;
	int j = 0;
	int iLength;
	int iUnicodeLen = 0;
	unsigned short *pUnicode = NULL;

	if(NULL == pUtf8Code)
	{
		return NULL;
	}

	iLength = strlen(pUtf8Code);

	/* 得到pUtf8Code指向的字符串实际代表的字符个数，不等同于字节数 */
	for(i=0; i<iLength;)
	{
		if(pUtf8Code[i]>0 && pUtf8Code[i]<=0x7F)	//UTF8(UCS2:0~0x7F):0xxxxxxx
		{
			iUnicodeLen++;
			i++;
		}	
		else if((pUtf8Code[i] & 0xE0)== 0xc0)		//UTF8(UCS2:0x80~0x7FF):110xxxxx 10xxxxxx
		{
			iUnicodeLen++;
			i+=2;
		}
		else if((pUtf8Code[i] & 0XF0)== 0xe0)		//UTF8(UCS2:0x800~0xFFFF):1110xxxx 10xxxxxx 10xxxxxx
		{
			iUnicodeLen++;
			i+=3;
		}	
	}

	/* 申请unicode编码占用的内存，以\0结束 */
	pUnicode = malloc((iUnicodeLen+1)*sizeof(unsigned short));

	/* 将UTF-8编码字符串转换成unicode编码字符串 */
	j = 0;
	for(i=0; i<iLength;)
	{
		if(pUtf8Code[i]>0 && pUtf8Code[i]<=0x7F)	//UTF8(UCS2:0~0x7F):0xxxxxxx
		{
			pUnicode[j] = (unsigned short) (pUtf8Code[i] & 0x7F);
			i++;
			j++;
		}	
		else if((pUtf8Code[i] & 0xE0)== 0xc0)		//UTF8(UCS2:0x80~0x7FF):110xxxxx 10xxxxxx
		{
            pUnicode[j] = (unsigned short) (((pUtf8Code[i] & 0x1F) << 6) |
                  			(pUtf8Code[i+1] & 0x3F));
			i+=2;
			j++;
		}
		else if((pUtf8Code[i] & 0XF0)== 0xe0)		//UTF8(UCS2:0x800~0xFFFF):1110xxxx 10xxxxxx 10xxxxxx
		{
            pUnicode[j] = (unsigned short) (((pUtf8Code[i] & 0x0F) << 12) |
                     		((pUtf8Code[i+1] & 0x3F) << 6) |
                      		(pUtf8Code[i+2] & 0x3F));	
			i+=3;			
			j++;
		}
	}	
	pUnicode[iUnicodeLen] = 0x00;

	return pUnicode;
}

/***
  * 功能：
        将UCS-2编码字符串转换成UTF-8编码字符串
  * 参数：
        1.unsigned char *pUcs2Code:    指向UCS-2编码字符串
        2.unsigned char *pUtf8Code:	   指向输出的用于存储UTF8编码值的缓冲区的指针 
        							   请确保pUtf8Code指向的缓冲区有足够的空间
  * 返回：
        成功返回0
        失败返回非零值
  * 备注：add by  2014.12.31
***/
int GetUcs2ToUtf8(unsigned short *pUcs2Code, char *pUtf8Code)
{
	int iReturn = 0;
	int iUcs2Len = 0;
	int i = 0;

	if((NULL==pUcs2Code) || (NULL==pUtf8Code))
	{
		return -1;
	}

	iUcs2Len = StringStrlen(pUcs2Code);
	for(i=0; i<iUcs2Len; i++)
	{
		if ( pUcs2Code[i] <= 0x0000007F )  
	    {  
	        // * U-00000000 - U-0000007F:  0xxxxxxx  
	        *pUtf8Code     = (pUcs2Code[i] & 0x7F);   
			pUtf8Code++;
	    }  
	    else if ( pUcs2Code[i] >= 0x00000080 && pUcs2Code[i] <= 0x000007FF )  
	    {  
	        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
	        *(pUtf8Code+1) = (pUcs2Code[i] & 0x3F) | 0x80;  
	        *pUtf8Code     = ((pUcs2Code[i] >> 6) & 0x1F) | 0xC0;  
			pUtf8Code+=2;
	    }  
	    else if ( pUcs2Code[i] >= 0x00000800 && pUcs2Code[i] <= 0x0000FFFF )  
	    {  
	        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
	        *(pUtf8Code+2) = (pUcs2Code[i] & 0x3F) | 0x80;  
	        *(pUtf8Code+1) = ((pUcs2Code[i] >>  6) & 0x3F) | 0x80;  
	        *pUtf8Code     = ((pUcs2Code[i] >> 12) & 0x0F) | 0xE0;  
			pUtf8Code+=3;
	    }  
	}
	*pUtf8Code = '\0';
		
	return iReturn;
}


/***
  * 功能：
        转化unicode字符串为GUI字符串
  * 参数：
        1.unsigned short  *pString:    要转换的C字符串
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：add by  2014.12.31
***/
GUICHAR* GetUnicode(unsigned short *pString)
{
    //错误标志、返回值定义
    GUICHAR *pReturn = NULL;
    //临时变量定义
    GUICHAR *pTmp = NULL;
    int iSize;

    //参数检查
    if (NULL == pString)
    {
        return NULL;
    }

    //统计字符串长度，包含'\0'
    for (pTmp = pString, iSize = 1; *pTmp; pTmp++)
    {
        iSize++;
    }

    //尝试为字符串内容分配内存
    pReturn = (GUICHAR *) malloc(iSize * sizeof(GUICHAR));
    if (NULL == pReturn)
    {
        return NULL;
    }

    //逐个字符进行转换
    while (--iSize >= 0)
    {
        pReturn[iSize] = pString[iSize];
    }

    return pReturn;
}


/***
  * 功能：
        字符串粘贴，功能类似于标准C库的strcat()
  * 参数：
        1.GUICHAR *pDest: 		指向目标字符串，不能为空
        2.GUICHAR *pSrc	:    	源字符串，不能为空
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：add by  2014.12.31
***/
GUICHAR* StringCat1(GUICHAR *pDest, GUICHAR *pSrc)
{
    //临时变量定义
    GUICHAR *pTmp = NULL;
    int iDest, iSrc;

    //参数检查
    if (NULL == pDest || NULL == pSrc)
    {
        return NULL;
    }

    //统计目标字符串与源字符串长度，不包含'\0'
    for (pTmp = pDest, iDest = 0; *pTmp; pTmp++)
    {
        iDest++;
    }
    for (pTmp = pSrc, iSrc = 0; *pTmp; pTmp++)
    {
        iSrc++;
    }

    //考虑当源为空字符串的情况
    if (iSrc == 0)
    {
        return NULL;
    }

    memcpy(pDest + iDest, pSrc, (iSrc + 1) * sizeof(GUICHAR));

    return pDest;
}


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
GUICHAR* StringCpy1(GUICHAR *pDest, GUICHAR *pSrc)
{
    //临时变量定义
    GUICHAR *pTmp = NULL;
    int iSize;

    //参数检查
    if (NULL == pDest || NULL == pSrc)
    {
        return NULL;
    }

    //统计源字符串长度，包含'\0'
    for (pTmp = pSrc, iSize = 1; *pTmp; pTmp++)
    {
        iSize++;
    }

    //完成字符串复制
    memcpy(pDest, pSrc, iSize * sizeof(GUICHAR));

    return pDest;
}


/***
  * 功能：
        计算宽字符串的字符数，不包含\0
  * 参数：
        1.GUICHAR *pStr:  指向宽符串
  * 返回：
        返回宽字符串的长度
  * 备注：add by  2014.12.31
***/
unsigned int StringStrlen(GUICHAR *pStr)
{
	unsigned int uiLength = 0;
	//临时变量定义
    GUICHAR *pTmp = NULL;

	if(NULL == pStr)
	{
		return 0;
	}

	//统计宽字符串的长度，不包含\0
    for (pTmp = pStr, uiLength = 0; *pTmp; pTmp++)
    {
        uiLength++;
    }

	return uiLength;
}

int StringUcs2TrimSpace(GUICHAR *pSrc)
{
    GUICHAR *tmpPtStart = pSrc;
    int iLen = 0;
    int i = 0;

    if(NULL == pSrc)
    {
        return -1;
    }

    while(' ' == *tmpPtStart)
    {
        tmpPtStart++;
    }

    iLen = StringStrlen(tmpPtStart);
    while(iLen > 0)
    {
        if(' ' == tmpPtStart[iLen-1])
        {
            tmpPtStart[iLen-1] = '\0';
            iLen--;
        }
        else
        {
            break;
        }
    }
    if(0 == iLen)
    {
        *pSrc = '\0';
        return -1;
    }
    if(pSrc != tmpPtStart)
    {
        for(i = 0; i<iLen;i++)
        {
            *(pSrc+i) = *(tmpPtStart+i);
        }
        *(pSrc+i) = '\0';
    }

    return 0;    
}


//diff end
