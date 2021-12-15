/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guischeduler.h
* 摘    要：  定义GUI的窗体调度机制及相关操作，用于实现多窗体、多线程运行。
*
* 当前版本：  v1.0.0
* 作    者：  yexin.zhu
* 完成日期：  2012-8-31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "guischeduler.h"


/*****************************************
* 为实现GUISCHEDULER而需要引用的其他头文件
*****************************************/
#include "guibase.h"
#include "guimessage.h"
#include "guiwindow.h"


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
//用于控制窗体线程数量的互斥锁(GUI目前只支持一个窗体线程)
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

    if (InitMutex(&mutexExitFlag))
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
    while (1)
    {
        //根据结束标志判断是否需要结束调度线程
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        //从消息队列读特定类型消息项(系统消息)
        MutexLock(&(pMsg->Mutex));
        pRecv = RecvSpecialMessage(GUIMESSAGE_TYP_SYS, pMsg);
        MutexUnlock(&(pMsg->Mutex));
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
            break;
        case GUIMESSAGE_THD_CREAT:
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
                }                                       //创建窗体线程
                ThreadCreate(&thdWnd, fnThread, pWnd);  //完成窗体线程的创建
                pWnd->thdWndTid = thdWnd;               //保存线程ID
            }
            break;
        case GUIMESSAGE_THD_DEST:
            //销毁窗体线程，调ThreadJoin()
            pWnd = (GUIWINDOW *) (pRecv->uiMsgValue);
            if (NULL != pWnd)                           //若消息值有效
            {                                           //即得到要结束的窗体结构
                thdWnd = pWnd->thdWndTid;               //提取窗体对应的线程ID
                ThreadJoin(thdWnd, NULL);               //等待线程结束并回收资源
                if (pWnd != GetCurrWindow())            //若当前窗体已重新设置
                {                                       //即新的窗体已完成绘制
                    DestroyWindow(&pWnd);               //销毁旧的窗体
                }
            }
            break;
        default:
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
    GUIWINDOW *pWnd = (GUIWINDOW *) pThreadArg;
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;
    GUIMESSAGE_ITEM *pRecv = NULL;

    //锁住当前窗体线程
    LockWindowThread();

    //得到当前消息对象
    pMsg = GetCurrMessage();

    //发窗体消息GUIMESSAGE_WND_INIT给自身
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_WND_INIT;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, pMsg); //pItem由本窗体的消息处理释放
    MutexUnlock(&(pMsg->Mutex));

    //窗体处理
    while (iRun)
    {
        //从消息队列读窗体消息项
        MutexLock(&(pMsg->Mutex));
        pRecv = RecvSpecialMessage(GUIMESSAGE_TYP_WND, pMsg);
        MutexUnlock(&(pMsg->Mutex));
        if (NULL == pRecv)
        {
            MsecSleep(20);          //若没有窗体消息，睡眠20毫秒
            continue;
        }

        //根据不同窗体消息编码进行相应处理
        switch (pRecv->iMsgCode)
        {
        case GUIMESSAGE_WND_INIT:
            if (NULL != pWnd->fnWndInit)
            {
                //窗体初始化，调fnWndInit()
                pWnd->fnWndInit(pWnd);
                //初始化完成后，发送GUIMESSAGE_WND_PAINT，进行窗体绘制
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_WND;
                pItem->iMsgCode = GUIMESSAGE_WND_PAINT;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem由本窗体的消息处理释放
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_WND_EXIT:
            if (NULL != pWnd->fnWndExit)
            {
                //窗体退出，调fnWndExit()
                pWnd->fnWndExit(pWnd);
                iRun = 0;                       //运行标志置0，窗体处理结束
                //退出完成后，发送GUIMESSAGE_THRD_DEST，销毁窗体线程
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_SYS;
                pItem->iMsgCode = GUIMESSAGE_THD_DEST;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem由窗体调度线程释放
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_WND_PAINT:
            if (NULL != pWnd->fnWndPaint)
            {
                //窗体绘制，调fnWndPaint()
                pWnd->fnWndPaint(pWnd);
                //窗体已绘制，设置焦点及当前窗体
                pWnd->Visible.iFocus = 1;       //关系到对按键事件的封装
                SetCurrWindow(pWnd);            //关系到事件封装线程能否工作
                //绘制完成后，发送GUIMESSAGE_WND_LOOP，进行窗体循环
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_WND;
                pItem->iMsgCode = GUIMESSAGE_WND_LOOP;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem由窗体调度线程释放
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_WND_LOOP:
            if (NULL != pWnd->fnWndLoop)
            {
                //根据循环标志判断是否允许窗体循环
                if (iLoop != 1)
                {
                    break;                      //若不允许，忽略窗体循环
                }
                //窗体循环，调fnWndLoop()
                pWnd->fnWndLoop(pWnd);
                //本次处理完成后，发送GUIMESSAGE_WND_LOOP，进行下一次窗体循环
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_WND;
                pItem->iMsgCode = GUIMESSAGE_WND_LOOP;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem由窗体调度线程释放
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_LOP_ENABLE:
            //循环标志置1，允许窗体循环
            iLoop = 1;
            //发送GUIMESSAGE_WND_LOOP，重新启动窗体循环
            pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
            pItem->iMsgType = GUIMESSAGE_TYP_WND;
            pItem->iMsgCode = GUIMESSAGE_WND_LOOP;
            pItem->uiMsgValue = (unsigned int) pWnd;
            MutexLock(&(pMsg->Mutex));
            WriteMessageQueue(pItem, pMsg);     //pItem由窗体调度线程释放
            MutexUnlock(&(pMsg->Mutex));
            break;
        case GUIMESSAGE_LOP_DISABLE:
            //循环标志置0，禁止窗体循环
            iLoop = 0;
            break;
        default:
            //进行其他窗体消息处理
            ExecMessageProc(pRecv, pMsg);
            break;
        }

        //释放读出的消息项
        free(pRecv);
    }

    //解锁当前窗体线程
    UnlockWindowThread();

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

    if (InitMutex(&mutexThreadLock))
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

    iReturn = MutexLock(&mutexThreadLock);

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

