/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiwindow.c
* 摘    要：  实现GUI的窗体类型及操作，为GUI窗体调度机制提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：
*******************************************************************************/

#include "guiwindow.h"


/**************************************
* 为实现GUIWINDOW而需要引用的其他头文件
**************************************/
//#include ""


/******************************
* GUI中的当前窗体对象及其互斥锁
******************************/
//GUI中的当前窗体对象
static GUIWINDOW *pCurrWindow = NULL;
//用于控制访问当前窗体对象的互斥锁
static GUIMUTEX mutexWndLock;


/***
  * 功能：
        根据指定的信息直接建立窗体对象
  * 参数：
        1.unsigned int uiPlaceX:    窗体水平放置位置，以左上角为基点
        2.unsigned int uiPlaceY:    窗体垂直放置位置，以左上角为基点
        3.unsigned int uiEndX:      窗体水平宽度
        4.unsigned int uiEndY:      窗体垂直高度
        5.WNDFUNC fnWndInit:        指向窗体初始化函数，用于完成窗体的初始化
        6.WNDFUNC fnWndExit:        指向窗体退出函数，用于完成窗体的退出
        7.WNDFUNC fnWndPaint:       指向窗体绘制函数，用于完成窗体的绘制
        8.WNDFUNC fnWndLoop:        指向窗体循环函数，为空则不进行循环处理
        9.WNDFUNC fnWndPause:       指向窗体挂起函数，为空则不进行挂起前预处理
        10.WNDFUNC fnWndResume:     指向窗体恢复函数，为空则不进行恢复前预处理
        11.THREADFUNC fnWndThread:  创建窗体处理线程，为空使用系统的窗体处理线程
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIWINDOW* CreateWindow(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                        unsigned int uiWndWidth, unsigned int uiWndHeight, 
                        WNDFUNC fnWndInit, WNDFUNC fnWndExit, 
                        WNDFUNC fnWndPaint, WNDFUNC fnWndLoop, 
                        WNDFUNC fnWndPause, WNDFUNC fnWndResume,
                        THREADFUNC fnWndThread)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIWINDOW *pWndObj = NULL;

    if (!iErr)
    {
        //判断uiWndWidth和uiWndHeight是否有效
        if (uiWndWidth < 1 || uiWndHeight < 1)
        {
            iErr = -1;
        }
        //判断fnWndInit、fnWndExit和fnWndPaint是否为有效指针
        if (NULL == fnWndInit || NULL == fnWndExit || NULL == fnWndPaint)
        {
            iErr = -1;
        }
    }
    
    if (!iErr)
    {
        //申请内存资源
        pWndObj = (GUIWINDOW *) malloc(sizeof(GUIWINDOW));
        if (NULL == pWndObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //创建窗体的控件队列
        pWndObj->pWndComps = CreateList(GUIWINDOW_COMP_LIMIT);
        if (NULL == pWndObj->pWndComps)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //初始化互斥锁
        if (InitMutex(&(pWndObj->Mutex), NULL))
        {
            iErr = -4;
        }
    }

    if (!iErr)
    {
        //窗体对象可视信息赋值
        pWndObj->Visible.Area.Start.x = uiPlaceX;
        pWndObj->Visible.Area.Start.y = uiPlaceY;
        pWndObj->Visible.Area.End.x = uiPlaceX + uiWndWidth - 1;
        pWndObj->Visible.Area.End.y = uiPlaceY + uiWndHeight - 1;
        pWndObj->Visible.iEnable = 1;   //默认可见
        pWndObj->Visible.iLayer = 0;    //默认图层为0(最底层)
        pWndObj->Visible.iFocus = 0;    //默认无焦点
        pWndObj->Visible.iCursor = 0;   //默认无光标
        memset(&(pWndObj->Visible.Hide), 0, sizeof(GUIRECTANGLE));
        pWndObj->Visible.pResume = NULL;
        pWndObj->Visible.fnDestroy = (DESTFUNC) DestroyWindow;
        pWndObj->Visible.fnDisplay = (DISPFUNC) fnWndPaint;
        //设置窗体对象的窗体处理函数
        pWndObj->fnWndInit = fnWndInit;
        pWndObj->fnWndExit = fnWndExit;
        pWndObj->fnWndPaint = fnWndPaint;
        pWndObj->fnWndLoop = fnWndLoop;
        pWndObj->fnWndPause = fnWndPause;
        pWndObj->fnWndResume = fnWndResume;
        //设置窗体对象对应的线程函数及线程ID
        pWndObj->fnWndThread = fnWndThread;
        pWndObj->thdWndTid = -1;        //默认无效
        //窗体对象的画刷、画笔及字体设置
        pWndObj->pWndPen = NULL;        //默认使用当前画笔
        pWndObj->pWndBrush = NULL;      //默认使用当前画刷
        pWndObj->pWndFont = NULL;       //默认使用当前字体
    }

	//错误处理
    switch (iErr)
    {
    case -4:
        DestroyList(pWndObj->pWndComps);
        //no break
    case -3:
        free(pWndObj);
        //no break
    case -2:
    case -1:
        pWndObj = NULL;
        //no break
    default:
        break;
    }

    return pWndObj;
}


/***
  * 功能：
        删除窗体对象
  * 参数：
        1.GUIWINDOW **ppWndObj: 指针的指针，指向需要销毁的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyWindow(GUIWINDOW **ppWndObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppWndObj是否为有效指针
        if (NULL == ppWndObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppWndObj所指向的是否为有效指针
        if (NULL == *ppWndObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //如果控件队列已包含有GUI窗体控件对象，尝试销毁控件队列
        if (NULL != (*ppWndObj)->pWndComps)
        {
            if (DestroyList((*ppWndObj)->pWndComps))
            {
                iErr = -4;
            }
        }
    }

    if (!iErr)
    {
        //销毁窗体对象，并将指针置空
        free(*ppWndObj);
        *ppWndObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        初始化当前窗体
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        默认将当前窗体初始化为NULL
***/
int InitCurrWindow(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    if (InitMutex(&mutexWndLock, NULL))
    {
        iReturn = -1;
    }
    else 
    {
        MutexLock(&mutexWndLock);   
        pCurrWindow = NULL;
        MutexUnlock(&mutexWndLock);
    }

    return iReturn;
}


/***
  * 功能：
        锁住当前窗体对象
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        窗体线程运行或恢复后，应第一时间调用该函数，保证始终只有一个活动窗体
***/
int LockCurrWindow(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    iReturn = MutexLock(&mutexWndLock);

    return iReturn;
}


/***
  * 功能：
        解锁当前窗体对象
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        窗体线程退出或挂起前，必须调用该函数，以使其他窗体线程得以运行
***/
int UnlockCurrWindow(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    iReturn = MutexUnlock(&mutexWndLock);

    return iReturn;
}


/***
  * 功能：
        保持当前的窗体对象不变，需要与UnlockCurrWindow()成对使用
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIWINDOW* HoldCurrWindow(void)
{
    //错误标志、返回值定义
    GUIWINDOW *pReturn = NULL;

    MutexLock(&mutexWndLock);
    pReturn = pCurrWindow;

    return pReturn;
}


/***
  * 功能：
        得到当前的窗体对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIWINDOW* GetCurrWindow(void)
{
    //错误标志、返回值定义
    GUIWINDOW *pReturn = NULL;

    MutexLock(&mutexWndLock);
    pReturn = pCurrWindow;
    MutexUnlock(&mutexWndLock);

    return pReturn;
}


/***
  * 功能：
        设置当前的窗体对象
  * 参数：
        1.GUIWINDOW *pWndObj:   被设置为当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrWindow(GUIWINDOW *pWndObj)
{
    //判断pWndObj是否有效
    //if (NULL == pWndObj)
    //{
       // return -1;
    //}

    MutexLock(&mutexWndLock);   
    pCurrWindow = pWndObj;
    MutexUnlock(&mutexWndLock);

    return 0;
}


/***
  * 功能：
        设置窗体的有效区域
  * 参数：
        1.unsigned int uiStartX:    窗体有效区域左上角横坐标，以左上角为基点
        2.unsigned int uiStartY:    窗体有效区域左上角纵坐标，以左上角为基点
        3.unsigned int uiEndX:      窗体有效区域右下角横坐标，以左上角为基点
        4.unsigned int uiEndY:      窗体有效区域右下角纵坐标，以左上角为基点
        5.GUIWINDOW *pWndObj:       需要设置有效区域的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetWindowArea(unsigned int uiStartX, unsigned int uiStartY, 
                  unsigned int uiEndX, unsigned int uiEndY,
                  GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断uiEndX和uiEndY是否有效
        if (uiEndX < uiStartX || uiEndY < uiStartY)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //窗体对象有效区域赋值
        pWndObj->Visible.Area.Start.x = uiStartX;
        pWndObj->Visible.Area.Start.y = uiStartY;
        pWndObj->Visible.Area.End.x = uiEndX;
        pWndObj->Visible.Area.End.y = uiEndY;
        //如果控件队列已包含有GUI窗体控件对象，修改各控件的显示大小
    }

    return iErr;
}


/***
  * 功能：
        设置窗体所使用的画刷
  * 参数：
        1.GUIBRUSH *pWndBrush:  用于窗体对象的画刷
        2.GUIWINDOW *pWndObj:   需要设置所使用画刷的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetWindowBrush(GUIBRUSH *pWndBrush, GUIWINDOW *pWndObj)
{
    //判断pWndBrush和pWndObj是否有效
    if (NULL == pWndBrush || NULL == pWndObj)
    {
        return -1;
    }

    pWndObj->pWndBrush = pWndBrush;

    return 0;
}


/***
  * 功能：
        设置窗体所使用的画笔
  * 参数：
        1.GUIPEN *pWndPen:      用于窗体对象的画笔
        2.GUIWINDOW *pWndObj:   需要设置所使用画笔的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetWindowPen(GUIPEN *pWndPen, GUIWINDOW *pWndObj)
{
    //判断pWndPen和pWndObj是否有效
    if (NULL == pWndPen || NULL == pWndObj)
    {
        return -1;
    }

    pWndObj->pWndPen = pWndPen;

    return 0;
}


/***
  * 功能：
        设置窗体所对应的字体
  * 参数：
        1.GUIFONT *pWndFont:    用于窗体对象的字体
        2.GUIWINDOW *pWndObj:   需要设置所使用字体的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetWindowFont(GUIFONT *pWndFont, GUIWINDOW *pWndObj)
{
    //判断pWndFont和pWndObj是否有效
    if (NULL == pWndFont || NULL == pWndObj)
    {
        return -1;
    }

    pWndObj->pWndFont = pWndFont;

    return 0;
}


/***
  * 功能：
        添加GUI窗体控件到指定窗体的窗体控件队列
  * 参数：
        1.int iCompTyp:         GUI窗体控件对象的类型
        2.int iCompLen:         GUI窗体控件对象所占用的内存空间
        3.void *pWndComp:       指针，指向要添加的GUI窗体控件
        4.GUIWINDOW *pWndObj:   需要添加GUI窗体控件的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int AddWindowComp(int iCompTyp, int iCompLen, void *pWndComp, 
                  GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;

    if (!iErr)
    {
        //判断pWndComp和pWndObj是否有效
        if (NULL == pWndComp || NULL == pWndObj)
        {
            return -1;
        }
    }
    //加锁
    MutexLock(&(pWndObj->Mutex));

    if (!iErr)
    {
        //判断pWndComps是否为有效指针和控件队列是否已满
        if ((NULL == pWndObj->pWndComps) || 
            ListFull(pWndObj->pWndComps))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //申请内存
        pPkt = (GUIOBJ *) malloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //添加GUI窗体控件到控件队列
        pPkt->iType = iCompTyp;
        pPkt->iLength = iCompLen;
        pPkt->pObj = pWndComp;
        if (ListAdd(&(pPkt->List), pWndObj->pWndComps))
        {
            iErr = -4;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -4:
        free(pPkt);
        //no break
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pWndObj->Mutex));

    return iErr;
}


/***
  * 功能：
        从指定窗体的窗体控件队列删除GUI窗体控件
  * 参数：
        1.void *pWndComp:       指针，指向要删除的GUI窗体控件
        2.GUIWINDOW *pWndObj:   需要删除GUI窗体控件的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int DelWindowComp(void *pWndComp, GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;


    if (!iErr)
    {
        //判断pWndComp和pWndObj是否有效
        if (NULL == pWndComp || NULL == pWndObj)
        {
            iErr = -1;
        }
    }
		
	//加锁
    MutexLock(&(pWndObj->Mutex));
	
    if (!iErr)
    {
        //判断pWndComps是否为有效指针和控件队列是否为空
        if ((NULL == pWndObj->pWndComps) || 
            ListEmpty(pWndObj->pWndComps))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历控件队列找到匹配的GUI窗体控件
        list_next_each(pItem, pWndObj->pWndComps, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            if (pPkt->pObj == pWndComp)
            {
                //找到类型匹配的窗体控件后从控件队列中删除
                if (ListDel(pItem, pWndObj->pWndComps))
                {
                    iErr = -3;
                }
                break;
            }
        }

        //如果没有找到类型匹配的消息项
        if (-1 == iPos)
        {
            iErr = -4;
        }
    }

    if (!iErr)
    {
        //释放资源
        free(pPkt);
    }

    //解锁
    MutexUnlock(&(pWndObj->Mutex));

    return iErr;
}


/***
  * 功能：
        清空指定窗体的窗体控件队列
  * 参数：
        1.GUIWINDOW *pWndObj:   需要被清空窗体控件队列的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int ClearWindowComp(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;

    //加锁
    MutexLock(&(pWndObj->Mutex));

    if (!iErr)
    {
        //判断pWndObj是否为有效指针
        if (NULL == pWndObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断窗体控件队列pWndComps是否为有效指针
        if (NULL == pWndObj->pWndComps)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历删除窗体控件队列项
        while (pWndObj->pWndComps->iCount)
        {
            pPkt= container_of(pWndObj->pWndComps->pHead, GUIOBJ, List);
            if (ListDel(pWndObj->pWndComps->pHead, pWndObj->pWndComps))
            {
                iErr = -3;
                break;
            }

            //释放资源
            free(pPkt);
        }
    }

    //解锁
    MutexUnlock(&(pWndObj->Mutex));

    return iErr;
}

