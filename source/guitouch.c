/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guitouch.c
* 摘    要：  GUIFBMAP模块主要完成设备抽象层定义及实现，以便GUI能够适用于不同硬
*             件平台对应用层隔离硬件差异。guitouch.c实现了guitouch.h中声明的触
*             摸屏设备的相关操作，用于实现GUI的触摸屏输入设备抽象。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#include "guitouch.h"


/*************************************
* 为实现GUITOUCH而需要引用的其他头文件
*************************************/
#include "guidevice.h"
#include "guischeduler.h"
#include "guievent.h"


/**************************
* 定义GUI中的当前触摸屏对象
**************************/
static GUITOUCH *pCurrTouch = NULL;


/***
  * 功能：
        依据指定的信息创建触摸屏对象
  * 参数：
        1.char *strDevName:         需要创建触摸屏对象的设备的名称
        2.int iFbType:              触摸屏缓冲设备类型，1标准、2自定义
        3.unsigned int uiHorRes:    触摸屏水平分辨率
        4.unsigned int uiVerRes:    触摸屏垂直分辨率
        5.THREADFUNC fnTouchThread: 回调函数，用于创建帧缓冲的线程
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUITOUCH* CreateTouch(char *strDevName, int iDevType, 
                      unsigned int uiHorRes, unsigned int uiVerRes,
                      THREADFUNC fnTouchThread)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUITOUCH *pTouchObj = NULL;

    if (!iErr)
    {
        //判断strDevName, fnTouchThread是否为有效指针
        if (NULL == strDevName || NULL == fnTouchThread)
        {
            iErr = -1;
        }
        //判断触摸屏分辨率是否有效
        if (uiHorRes > 0xFFFF || uiVerRes > 0xFFFF)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //申请内存资源
        pTouchObj = (GUITOUCH *) malloc(sizeof(GUITOUCH));
        if (NULL == pTouchObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:open()
    if (!iErr)
    {
        //尝试打开设备文件
        pTouchObj->iDevFd = open(strDevName, O_RDWR);//|O_NONBLOCK);
        if (-1 == pTouchObj->iDevFd)
        {
            iErr = -3;
        }
        //设置新建触摸屏对象的iDevType
        pTouchObj->iDevType = iDevType;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    if (!iErr)
    {
        //设置新建触摸屏对象的分辨率
        pTouchObj->uiHorRes = uiHorRes;
        pTouchObj->uiVerRes = uiVerRes;
        pTouchObj->uiCursX = 0;
        pTouchObj->uiCursY = 0;
        //设置新建触摸屏对象的fnTouchThread，并尝试初始化Mutex
        pTouchObj->fnTouchThread = fnTouchThread;
        if (InitMutex(&(pTouchObj->Mutex), NULL))
        {
            iErr = -4;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:close()
    //错误处理
    switch (iErr)
    {
    case -4:
        close(pTouchObj->iDevType);
        //no break
    case -3:
        free(pTouchObj);
        //no break
    case -2:
    case -1:
        pTouchObj = NULL;
        //no break
    default:
        break;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS
    return pTouchObj;
}


/***
  * 功能：
        删除触摸屏对象
  * 参数：
        1.GUITOUCH **ppTouchObj:    指针的指针，指向需要销毁的触摸屏对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyTouch(GUITOUCH **ppTouchObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppTouchObj是否为有效指针
        if (NULL == ppTouchObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppTouchObj所指向的是否为有效指针
        if (NULL == *ppTouchObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:close()
    if (!iErr)
    {
        //释放资源
        if (close((*ppTouchObj)->iDevFd))   //设备文件为共享资源，需验证返回值
        {
            iErr = -3;
        }
        //销毁触摸屏对象，并将指针置空
        free(*ppTouchObj);
        *ppTouchObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iErr;
}


/***
  * 功能：
        获取当前触摸屏对象
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
GUITOUCH* GetCurrTouch(void)
{
    return pCurrTouch;
}


/***
  * 功能：
        设置当前触摸屏对象
  * 参数：
        1.GUITOUCH *pTouchObj:  被设置为当前触摸屏设备对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrTouch(GUITOUCH *pTouchObj)
{
    //判断pTouchObj是否有效
    if (NULL == pTouchObj)
    {
        return -1;
    }

    pCurrTouch = pTouchObj;

    return 0;
}


/***
  * 功能：
        设置触摸屏的光标位置
  * 参数：
        1.unsigned int uiCursX: 触摸屏当前横坐标值，0~0xFFFF
        2.unsigned int uiCursY: 触摸屏当前纵坐标值，0~0xFFFF
        3.GUITOUCH *pTouchObj:  需要设置光标位置的触摸屏对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（触摸屏对象中的互斥锁），不能在持有锁的前提下调用
***/
int SetTouchCursor(unsigned int uiCursX, unsigned int uiCursY,
                   GUITOUCH *pTouchObj)
{    
    //加锁
    MutexLock(&(pTouchObj->Mutex));

    //判断uiCursX、uiCursY和pTouchObj是否有效
    if (uiCursX > 0xFFFF || uiCursY > 0xFFFF || NULL == pTouchObj)
    {
        return -1;
    }

    //设置光标位置
    pTouchObj->uiCursX = uiCursX;
    pTouchObj->uiCursY = uiCursY;

    //解锁
    MutexUnlock(&(pTouchObj->Mutex));

    return 0;
}


/***
  * 功能：
        默认触摸屏线程入口函数
  * 参数：
        1.void *pThreadArg:     线程入口函数参数，类型为(GUITOUCH *)
  * 返回：
        返回线程结束状态
  * 备注：
***/
void* DefaultTouchThread(void *pThreadArg)
{
    //错误标志、返回值定义
    int iErr = 0;
    static int iReturn = 0;
    //临时变量定义
    int iExit = 0;
    GUITOUCH *pTouchObj = pThreadArg;

    while (!iExit)
    {
        //根据结束标志判断是否需要结束触摸屏线程
        iExit = GetExitFlag();
        if (iExit)
        {
            break;
        }

        //触摸屏事件的封装
        switch (pTouchObj->iDevType)
        {
        case GUITOUCH_TYP_STD:
            iErr = StandardTouchProcess(pTouchObj);
            break;
        case GUITOUCH_TYP_CUST:
            iErr = CustomTouchProcess(pTouchObj);
            break;
        default:
            break;
        }

        //出错为-1，表示无触摸屏数据可读，睡眠50毫秒
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
        标准类型触摸屏(PC鼠标)的处理函数，用于完成触摸屏线程的处理
  * 参数：
        1.GUITOUCH *pTouchObj:  处理函数要处理的触摸屏对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        返回-1表示无触摸屏数据可读
***/
int StandardTouchProcess(GUITOUCH *pTouchObj)
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
    MutexLock(&(pTouchObj->Mutex));

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:read()
    if (!iErr)
    {
        //读取触摸屏返回的数据
        ThreadTestcancel();
        iTmp = read(pTouchObj->iDevFd, &event, sizeof(event));
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
        //尝试为触摸屏事件项分配内存
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
        switch (event.type)
        {
        case EV_KEY:
            if (BTN_MOUSE == event.code)
            {
                if (event.value)    //0:KeyUp，1:KeyDown
                {
                    pEventItem->uiEventType = GUIEVENT_TYP_IN;
                    pEventItem->uiEventCode = GUIEVENT_ABS_DOWN;
                    pEventItem->uiEventValue = (pTouchObj->uiCursX << 16) |
                                               (pTouchObj->uiCursY);
                }
                else
                {
                    pEventItem->uiEventType = GUIEVENT_TYP_IN;
                    pEventItem->uiEventCode = GUIEVENT_ABS_UP;
                    pEventItem->uiEventValue = (pTouchObj->uiCursX << 16) |
                                               (pTouchObj->uiCursY);
                }
            }
            else
            {
                iErr = -3;
            }
            break;
        case EV_REL:
            switch (event.code)
            {
            case REL_X:
                iTmp = (int) (pTouchObj->uiCursX);
                iTmp += event.value;
                if (iTmp < 0)
                {
                    iTmp = 0;
                }
                if (iTmp > ((int) pTouchObj->uiHorRes))
                {
                    iTmp = (int) pTouchObj->uiHorRes;
                }
                pTouchObj->uiCursX = (unsigned int) iTmp;
                pEventItem->uiEventType = GUIEVENT_TYP_IN;
                pEventItem->uiEventCode = GUIEVENT_MOV_CURSOR;
                pEventItem->uiEventValue = (pTouchObj->uiCursX << 16) |
                                           (pTouchObj->uiCursY);
                break;
            case REL_Y:
                iTmp = (int) (pTouchObj->uiCursY);
                iTmp += event.value;
                if (iTmp < 0)
                {
                    iTmp = 0;
                }
                if (iTmp > ((int) pTouchObj->uiVerRes))
                {
                    iTmp = (int) pTouchObj->uiVerRes;
                }
                pTouchObj->uiCursY = (unsigned int) iTmp;
                pEventItem->uiEventType = GUIEVENT_TYP_IN;
                pEventItem->uiEventCode = GUIEVENT_MOV_CURSOR;
                pEventItem->uiEventValue = (pTouchObj->uiCursX << 16) |
                                           (pTouchObj->uiCursY);
                break;
            default:
                iErr = -3;
                break;
            }
            break;
        default:
            iErr = -3;
            break;
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
    MutexUnlock(&(pTouchObj->Mutex));

    return iErr;
}


/***
  * 功能：
        自定义类型触摸屏的处理函数，用于完成触摸屏线程的处理
  * 参数：
        1.GUITOUCH *pTouchObj:  处理函数要处理的触摸屏对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        返回-1表示无触摸屏数据可读
***/
int CustomTouchProcess(GUITOUCH *pTouchObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    //调用设备抽象层函数完成自定义类型触摸屏的处理
    iErr = CustomTouchFunc(pTouchObj);

    return iErr;
}

