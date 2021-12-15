/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guischeduler.h
* 摘    要：  定义GUI的窗体调度机制及相关操作，用于实现多窗体、多线程运行。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guischeduler.h"


/*****************************************
* 为实现GUISCHEDULER而需要引用的其他头文件
*****************************************/
#include "guibase.h"
#include "guievent.h"
#include "guimessage.h"
#include "guiwindow.h"
#include "guiglobal.h"


/**********************************
* 定义GUI中的进程退出标志及其互斥锁
**********************************/
//GUI中的进程退出标志
static int iExitFlag = 0;
//用于控制访问进程退出标志的互斥锁
static GUIMUTEX mutexExitFlag;


/**************************
* 定义GUI中的窗体线程互斥锁
**************************/
//用于控制窗体线程数量的互斥锁(GUI目前只支持一个窗体线程运行)
static GUIMUTEX mutexThreadLock;


/***
  * 功能：
        初始化GUI进程退出标志
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        默认将进程退出标志初始化为0
***/
int InitExitFlag(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    if (InitMutex(&mutexExitFlag, NULL))
    {
        iReturn = -1;
    }
    else 
    {
        MutexLock(&mutexExitFlag);   
        iExitFlag = 0;
        MutexUnlock(&mutexExitFlag);
    }

    return iReturn;
}


/***
  * 功能：
        得到GUI进程退出标志
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int GetExitFlag(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    MutexLock(&mutexExitFlag);   
    iReturn = iExitFlag;
    MutexUnlock(&mutexExitFlag);

    return iReturn;
}


/***
  * 功能：
        设置GUI进程退出标志
  * 参数：
        1.int iFlag:    需要设为的GUI进程退出标志
  * 返回：
        无
  * 备注：
***/
void SetExitFlag(int iFlag)
{
    MutexLock(&mutexExitFlag);   
    iExitFlag = iFlag;
    MutexUnlock(&mutexExitFlag);
}


/***
  * 功能：
        窗体调度线程入口函数
  * 参数：
        1.void *pThreadArg:     线程入口函数参数，类型为(GUIMESSAGE *)
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
void* WindowSchedulerThread(void *pThreadArg)
{
    //错误标志、返回值定义
    static int iReturn = 0;
    //临时变量定义
    int iExit = 0;
    GUIMESSAGE *pMsg = (GUIMESSAGE *) pThreadArg;
    GUIMESSAGE_ITEM *pRecv = NULL;
    GUIWINDOW *pWnd = NULL;
    GUITHREAD thdWnd = -1;
    THREADFUNC fnThread = NULL;

    //等待100ms，以便GUI主进程准备好资源
    MsecSleep(100);

    //主工作循环
    while (!iExit)
    {
        //根据结束标志判断是否需要结束调度线程
        //注意此处不可立即退出主工作循环，否则会导致销毁窗体无法完成，形成僵尸
        iExit = GetExitFlag();

        //从消息队列读特定类型消息项(系统消息)
        pRecv = RecvSpecialMessage(GUIMESSAGE_TYP_SYS, pMsg);
        if (NULL == pRecv)
        {
            MsecSleep(50);          //若没有系统消息，睡眠50毫秒
            continue;
        }

        //根据不同系统消息编码进行相应处理
        switch (pRecv->iMsgCode)
        {
        case GUIMESSAGE_SYS_EXIT:
            //系统退出，设GUI环境退出标志
            SetExitFlag(1);
            //释放读出的消息项
            free(pRecv);
            break;
        case GUIMESSAGE_THD_CREATE:
            //建立窗体线程，调ThreadCreate()
            pWnd = (GUIWINDOW *) (pRecv->uiMsgValue);
            if (NULL != pWnd)                           //若消息值有效
            {                                           //即得到要结束的窗体结构
                if (NULL != pWnd->fnWndThread)          //判断是否指定线程函数
                {                                       //若已指定窗体线程函数
                    fnThread = pWnd->fnWndThread;       //使用指定的线程函数来
                }                                       //创建窗体线程
                else                                    //否则
                {                                       //若未指定窗体线程函数
                    fnThread = DefaultWindowThread;     //使用默认的线程函数来
                }                                       //创建窗体线程前
                if (LockWindowThread())                 //尝试对窗体线程上锁
                {                                       //若上锁失败
                    WriteMessageQueue(pRecv, pMsg, 0);  //将已读出的消息项回写
                    MsecSleep(50);                      //上锁失败后睡眠50毫秒
                    break;                              //中断建立窗体操作
                }                                       //若锁住窗体线程锁
                ThreadCreate(&thdWnd, NULL,             //按默认线程属性
                             fnThread, pWnd);           //完成窗体线程的创建
                pWnd->thdWndTid = thdWnd;               //保存线程ID
            }
            //释放读出的消息项
            free(pRecv);
            break;
        case GUIMESSAGE_THD_DESTROY:
            //销毁窗体线程，调ThreadJoin()
            pWnd = (GUIWINDOW *) (pRecv->uiMsgValue);
            if (NULL != pWnd)                           //若消息值有效
            {                                           //即得到要结束的窗体结构
                thdWnd = pWnd->thdWndTid;               //提取窗体对应的线程ID
                ThreadJoin(thdWnd, NULL);               //等待线程结束并回收资源
                DestroyWindow(&pWnd);                   //销毁旧的窗体
                SetCurrWindow(NULL);                    //当前窗体置空
                UnlockWindowThread();                   //解锁窗体线程
            }
            //释放读出的消息项
            free(pRecv);
            break;
        default:
            //释放读出的消息项
            free(pRecv);
            break;
        }

    }

    //退出线程
    ThreadExit(&iReturn);
    return &iReturn;    //该语句只是为了消除编译器报警，实际上不起作用
}


/***
  * 功能：
        默认窗体处理线程入口函数
  * 参数：
        1.void *pThreadArg:     线程入口函数参数，类型为(GUIWINDOW *)
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
        默认的窗体处理线程函数。当在创建窗体时，若未指定fnWndThread，则使用该函
        数创建窗体处理线程。如果需要使用自定义的函数来做为窗体处理线程，请参考
        本函数的框架结构。
***/
void* DefaultWindowThread(void *pThreadArg)
{
    //错误标志、返回值定义
    static int iReturn = 0;
    //临时变量定义
    int iRun = 1;
    int iLoop = 1;
    int iExec = 0;
    GUIWINDOW *pWnd = (GUIWINDOW *) pThreadArg;
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pRecv = NULL;

    //得到当前消息对象
    pMsg = GetCurrMessage();

    //开始窗体处理前先清除之前窗体可能残留的事件及消息
    ClearEventQueue(GetCurrEvent());
    ClearSpecialMessage(GUIMESSAGE_TYP_WND, pMsg);
    //发窗体消息GUIMESSAGE_WND_INIT
    SendWndMsg_WindowInit(pWnd);

    //窗体处理
    while (iRun)
    {
        //从消息队列读窗体消息项
        pRecv = RecvSpecialMessage(GUIMESSAGE_TYP_WND, pMsg);
        if (NULL == pRecv)
        {
            MsecSleep(50);          //若没有窗体消息，睡眠50毫秒
            continue;
        }
        //根据不同窗体消息编码进行相应处理
        switch (pRecv->iMsgCode)
        {
        case GUIMESSAGE_WND_INIT:
            if (NULL != pWnd->fnWndInit)
            {
                pWnd->fnWndInit(pWnd);          //窗体初始化
                SendWndMsg_WindowPaint(pWnd);   //准备执行窗体绘制
            }
            break;
        case GUIMESSAGE_WND_EXIT:
            if (NULL != pWnd->fnWndExit)
            { 
                pWnd->fnWndExit(pWnd);              //窗体退出
                iRun = 0;                           //运行标志置0
                SendSysMsg_ThreadDestroy(pWnd);     //准备销毁线程
            }
            break;
        case GUIMESSAGE_WND_PAINT:
            if (NULL != pWnd->fnWndPaint)
            {
                pWnd->fnWndPaint(pWnd);             //窗体绘制
                pWnd->Visible.iFocus = 1;           //设置焦点，影响按键事件封装
                SetCurrWindow(pWnd);                //设置当前窗体，影响事件封装
                iExec = 1;                          //执行标志置1
                SendWndMsg_WindowLoop(pWnd);        //准备执行窗体循环
            }
            break;
        case GUIMESSAGE_WND_LOOP:
            if (NULL != pWnd->fnWndLoop)
            {
                if (iLoop)                          //判断是否允许窗体循环
                {                                   //若允许
                    pWnd->fnWndLoop(pWnd);          //执行一次窗体循环
                    SendWndMsg_WindowLoop(pWnd);    //准备执行下一次窗体循环 
                }
            }
            break;
        case GUIMESSAGE_WND_PAUSE:
            if (NULL != pWnd->fnWndPause)
            {
                ClearEventQueue(GetCurrEvent());                //清空事件队列
                ClearSpecialMessage(GUIMESSAGE_TYP_WND, pMsg);  //清空窗体消息
                iExec = 0;                                      //执行标志置0
                //...
                UnlockWindowThread();                           //解锁窗体线程
            }
            break;
        case GUIMESSAGE_WND_RESUME:
            if (NULL != pWnd->fnWndResume)
            {
                LockWindowThread();
                //...
                iExec = 1;
            }
            break;
        case GUIMESSAGE_LOP_ENABLE:
            iLoop = 1;                      //循环标志置1，允许窗体循环
            SendWndMsg_WindowLoop(pWnd);    //准备执行窗体循环
            break;
        case GUIMESSAGE_LOP_DISABLE:
            iLoop = 0;                      //循环标志置0，禁止窗体循环
            break;
        default:
            if (iExec)                          //判断是否允许执行消息分发处理
            {                                   //若允许
                ExecMessageProc(pRecv, pMsg);   //进行其他窗体消息处理
            }
            break;
        }

        //释放读出的消息项
        free(pRecv);
    }

    //退出线程
    ThreadExit(&iReturn);
    return &iReturn;    //该语句只是为了消除编译器报警，实际上不起作用
}


/***
  * 功能：
        初始化窗体线程互斥锁
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InitThreadLock(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    if (InitMutex(&mutexThreadLock, NULL))
    {
        iReturn = -1;
    }

    return iReturn;
}


/***
  * 功能：
        锁住当前窗体线程
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        窗体线程运行后，应当第一时间调用该函数，以保证GUI中始终只有一个窗体线程
***/
int LockWindowThread(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    iReturn = MutexTrylock(&mutexThreadLock);

    return iReturn;
}


/***
  * 功能：
        解锁当前窗体线程
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        窗体线程退出前，必须调用该函数，以使其他窗体线程得以运行
***/
int UnlockWindowThread(void)
{
    //错误标志、返回值定义
    int iReturn = 0;

    iReturn = MutexUnlock(&mutexThreadLock);

    return iReturn;
}

