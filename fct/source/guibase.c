/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibase.c
* 摘    要：  实现GUI基本数据类型的操作函数，为其他模块提供支持
*
* 当前版本：  v1.0.1 (考虑到跨平台，新增了对于GUI_OS_ENV的预编译检验)
* 作    者：  wjg
* 完成日期：  2012-10-11
*
* 取代版本：  v1.0.0
* 原 作 者：  yexin.zhu
* 完成日期：  2012-7-24
*******************************************************************************/

#include "guibase.h"


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
    if (iSrc == 0)
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
        if (*a == 0 || *b == 0) //字符串以'\0'结尾
        {
            break;
        }
    }

    return iCmp;
}


/***
  * 功能：
        创建新的线程
  * 参数：
        1.GUITHREAD *pThread:   指向创建的新线程的线程ID
        2.THREADFUNC fnEntry:   创建新线程时指定的线程入口函数
        3.void *pArg:           创建新线程时传递的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ThreadCreate(GUITHREAD *pThread, THREADFUNC fnEntry, void *pArg)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_create()
    iReturn = pthread_create(pThread, NULL, fnEntry, pArg);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

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
  #endif

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
  #endif

    return iReturn;
}


/***
  * 功能：
        初始化锁
  * 参数：
        1.GUIMUTEX *pMutex:   指针，指向需要初始化的互斥锁
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InitMutex(GUIMUTEX *pMutex)
{
    //错误标志、返回值定义
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:pthread_mutex_init()
    iReturn = pthread_mutex_init(pMutex, NULL);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

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
  #endif

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
  #endif

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
  #endif

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

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:timeval,select()
    struct timeval tv;

    tv.tv_sec = ulMsec / 1000;
    tv.tv_usec = ulMsec % 1000 * 1000;
    iReturn = select(0, NULL, NULL, NULL, &tv);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

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
    if (pList->iCount == 1)
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
    pAttach->pHead->pPrev = pList->pTail;
    pList->pTail->pNext = pAttach->pHead;
    pAttach->pTail->pNext = pList->pHead;
    pList->pHead->pPrev = pAttach->pTail;
    pList->pTail = pAttach->pTail;
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

