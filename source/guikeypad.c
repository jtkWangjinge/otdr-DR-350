/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guikeypad.c
* 摘    要：  GUIKEYPAD模块主要完成设备抽象层定义及实现，以便GUI能够适用于不同
*             硬件平台对应用层隔离硬件差异。guikeypad.c实现了guikeypad.h中声明
*             的键盘设备的相关操作，用于实现GUI的图形输出设备抽象。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#include "guikeypad.h"


/**************************************
* 为实现GUIKEYPAD而需要引用的其他头文件
**************************************/
#include "guidevice.h"
#include "guischeduler.h"
#include "guievent.h"


/************************
* 定义GUI中的当前键盘对象
************************/
static GUIKEYPAD *pCurrKeypad = NULL;


/***
  * 功能：
        依据指定的信息创建键盘对象
  * 参数：
        1.char *strDevName:             需要创建键盘对象的设备的名称
        2.int iFbType:                  键盘缓冲设备类型，1标准、2自定义
        3.THREADFUNC fnKeypadThread:    回调函数，用于创建帧缓冲的线程
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIKEYPAD* CreateKeypad(char *strDevName, int iDevType, 
                        THREADFUNC fnKeypadThread)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIKEYPAD *pKeypadObj = NULL;

    if (!iErr)
    {
        //判断strDevName, fnKeypadThread是否为有效指针
        if (NULL == strDevName || NULL == fnKeypadThread)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //申请内存资源
        pKeypadObj = (GUIKEYPAD *) malloc(sizeof(GUIKEYPAD));
        if (NULL == pKeypadObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:open()
    if (!iErr)
    {
        //尝试打开设备文件
        pKeypadObj->iDevFd = open(strDevName, O_RDWR|O_NONBLOCK);
        if (-1 == pKeypadObj->iDevFd)
        {
            iErr = -3;
        }
        //设置新建键盘对象的iDevType
        pKeypadObj->iDevType = iDevType;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    if (!iErr)
    {
        //设置新建键盘对象的fnKeypadThread，并尝试初始化Mutex
        pKeypadObj->fnKeypadThread = fnKeypadThread;
        if (InitMutex(&(pKeypadObj->Mutex), NULL))
        {
            iErr = -4;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:close()
    //错误处理
    switch (iErr)
    {
    case -4:
        close(pKeypadObj->iDevFd);
        //no break
    case -3:
        free(pKeypadObj);
        //no break
    case -2:
    case -1:
        pKeypadObj = NULL;
        //no break
    default:
        break;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return pKeypadObj;
}


/***
  * 功能：
        删除键盘对象
  * 参数：
        1.GUIKEYPAD **ppKeypadObj:  指针的指针，指向需要销毁的键盘对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyKeypad(GUIKEYPAD **ppKeypadObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppKeypadObj是否为有效指针
        if (NULL == ppKeypadObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppKeypadObj所指向的是否为有效指针
        if (NULL == *ppKeypadObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:close()
    if (!iErr)
    {
        //释放资源
        if (close((*ppKeypadObj)->iDevFd))  //设备文件为共享资源，需验证返回值
        {
            iErr = -3;
        }
        //销毁键盘对象，并将指针置空
        free(*ppKeypadObj);
        *ppKeypadObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iErr;
}


/***
  * 功能：
        获取当前键盘对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIKEYPAD* GetCurrKeypad(void)
{
    return pCurrKeypad;
}


/***
  * 功能：
        设置当前键盘对象
  * 参数：
        1.GUIKEYPAD *pKeypadObj:    被设置为当前键盘设备对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrKeypad(GUIKEYPAD *pKeypadObj)
{
    //判断pKeypadObj是否有效
    if (NULL == pKeypadObj)
    {
        return -1;
    }

    pCurrKeypad = pKeypadObj;

    return 0;
}


/***
  * 功能：
        默认键盘线程入口函数
  * 参数：
        1.void *pThreadArg:     线程入口函数参数，类型为(GUIKEYPAD *)
  * 返回：
        返回线程结束状态
  * 备注：
***/
void* DefaultKeypadThread(void *pThreadArg)
{
    //错误标志、返回值定义
    int iErr = 0;
    static int iReturn = 0;
    //临时变量定义
    int iExit = 0;
    GUIKEYPAD *pKeypadObj = pThreadArg;

    while (!iExit)
    {
        //根据结束标志判断是否需要结束键盘线程
        iExit = GetExitFlag();
        if (iExit)
        {
            break;
        }

        //键盘事件的封装
        switch (pKeypadObj->iDevType)
        {
        case GUIKEYPAD_TYP_STD:
            iErr = StandardKeypadProcess(pKeypadObj);
            break;
        case GUIKEYPAD_TYP_CUST:
            iErr = CustomKeypadProcess(pKeypadObj);
            break;
        default:
            break;
        }

        //出错为-1，表示无键盘数据可读，睡眠50毫秒
        if (-1 == iErr)
        {
            MsecSleep(50);
        }
    }

    //退出线程
    ThreadExit(&iReturn);
    return &iReturn;    //该语句只是为了消除编译器报警，实际上不起作用
}


/***
  * 功能：
        标准类型键盘(PC键盘)的处理函数，用于完成键盘线程的处理
  * 参数：
        1.GUIKEYPAD *pKeypadObj:    处理函数要处理的键盘对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        返回-1表示无键盘数据可读
***/
int StandardKeypadProcess(GUIKEYPAD *pKeypadObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iTmp;
    GUIEVENT_ITEM *pEventItem = NULL;
    GUIEVENT *pEventObj = NULL;
  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:input_event
    struct input_event event;
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS
  
    //加锁
    MutexLock(&(pKeypadObj->Mutex));

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:read()
    if (!iErr)
    {
        //读取键盘返回的数据
        ThreadTestcancel();
        iTmp = read(pKeypadObj->iDevFd, &event, sizeof(event));
        ThreadTestcancel();
        if (iTmp != sizeof(event))
        {
            iErr = -1;
        }
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    if (!iErr)
    {
        //尝试为按键事件项分配内存
        pEventItem = (GUIEVENT_ITEM *) malloc(sizeof(GUIEVENT_ITEM));
        if (NULL == pEventItem)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:event
    if (!iErr)
    {
        //生成GUIEVENT_ITEM
        if (EV_KEY == event.type)
        {
            switch (event.value)    //0:KeyUp，1:KeyDown
            {
            case 0:
                pEventItem->uiEventType = GUIEVENT_TYP_IN;
                pEventItem->uiEventCode = GUIEVENT_KEY_UP;
                pEventItem->uiEventValue = (unsigned int) event.code;
                break;
            case 1:
                pEventItem->uiEventType = GUIEVENT_TYP_IN;
                pEventItem->uiEventCode = GUIEVENT_KEY_DOWN;
                pEventItem->uiEventValue = (unsigned int) event.code;
                break;
            default:
                iErr = -3;
                break;
            }
        }
        else
        {
            iErr = -3;
        }
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    if (!iErr)
    {
        //事件项写入事件队列
        pEventObj = GetCurrEvent();
        if (WriteEventQueue(pEventItem, pEventObj))
        {
            iErr = -4;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -4:
    case -3:
        free(pEventItem);
        //no break
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pKeypadObj->Mutex));

    return iErr;
}


/***
  * 功能：
        自定义类型键盘的处理函数，用于完成键盘线程的处理
  * 参数：
        1.GUIKEYPAD *pKeypadObj:    处理函数要处理的键盘对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        返回-1表示无键盘数据可读
***/
int CustomKeypadProcess(GUIKEYPAD *pKeypadObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    //调用设备抽象层函数完成自定义类型键盘的处理
    iErr = CustomKeypadFunc(pKeypadObj);

    return iErr;
}

