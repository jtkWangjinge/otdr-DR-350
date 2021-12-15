/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiglobal.c
* 摘    要：  实现与GUI的公共数据相关的操作函数，并提供对于GUI环境的初始化设置。
*
* 当前版本：  v1.0.0
* 作    者：  yexin.zhu
* 完成日期：  2012-8-1
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "guiglobal.h"
#include "wnd_global.h"


/**************************************
* 为实现GUIGLOBAL而需要引用的其他头文件
**************************************/


/******************************
* 定义GUI中与环境配置相关的变量
******************************/
#define DEVFILE_FB      "/dev/fb0"
#define DEVFILE_KEYPAD  "/dev/event0"   
#define DEVFILE_TOUCH   "/dev/event1"

static GUIFBMAP *pEnvFbmap = NULL;
static GUIKEYPAD *pEnvKeypad = NULL;
// static GUITOUCH *pEnvTouch = NULL;

static GUIEVENT *pEnvEvent = NULL;
static GUIMESSAGE *pEnvMessage = NULL;

static GUIPALETTE *pEnvPalette = NULL;
static GUIPEN *pEnvPen = NULL;
static GUIBRUSH *pEnvBrush = NULL;
static GUIFONT *pEnvFont = NULL;

static GUITHREAD thdScheduler = -1;
static GUITHREAD thdEvent = -1;


/***
  * 功能：
        初始化GUI环境，分配GUI资源
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InitializeGUI(void)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //尝试初始化进程退出标志
        if (InitExitFlag())
        {
            iErr = -1;
        }
    }

    if (iErr == 0)  //设备抽象层初始化
    {
        //创建当前GUI环境的帧缓冲映射对象
        pEnvFbmap = CreateFbmap(DEVFILE_FB, GUI_FBTYP_STD, 
                                640, 480, 
                                DefaultFbThread);
        if (NULL == pEnvFbmap)
        {
            iErr = -2;
        }
        SetCurrFbmap(pEnvFbmap);
        //创建当前GUI环境的键盘对象
        pEnvKeypad = CreateKeypad(DEVFILE_KEYPAD, GUIKEYPAD_TYP_USER, 
                                  DefaultKeypadThread);
        if (NULL == pEnvKeypad)
        {
            //iErr = -2;
        }
        SetCurrKeypad(pEnvKeypad);
        //创建当前GUI环境的触摸屏对象
        // pEnvTouch = CreateTouch(DEVFILE_TOUCH, GUITOUCH_TYP_STD, 
        //                         DefaultTouchThread);
        // if (NULL == pEnvTouch)
        // {
        //     iErr = -2;
        // }
        // SetCurrTouch(pEnvTouch);
    }

    if (iErr == 0)  //系统调度模块初始化
    {
        //创建当前GUI环境的事件对象
        pEnvEvent = CreateEvent(GUIEVENT_ITEM_LIMIT, EventPacketThread);
        if (NULL == pEnvEvent)
        {
            iErr = -3;
        }
        SetCurrEvent(pEnvEvent);
        //创建当前GUI环境的消息对象
        pEnvMessage = CreateMessage(GUIMESSAGE_ITEM_LIMIT, 
                                    GUIMESSAGE_REG_LIMIT);
        if (NULL == pEnvMessage)
        {
            iErr = -3;
        }
        SetCurrMessage(pEnvMessage);
        //初始化窗体线程互斥锁(调度线程将访问线程锁，必须在之前进行此操作)
        if (InitThreadLock())
        {
            iErr = -3;
        }
        //创建当前GUI环境的调度线程
        if (ThreadCreate(&thdScheduler, WindowSchedulerThread, pEnvMessage))
        {
            iErr = -3;
        }
        //初始化当前窗体(事件封装线程将访问当前窗体，必须在之前进行此操作)
        if (InitCurrWindow())
        {
            iErr = -3;
        }
        //创建当前GUI环境的事件封装线程
        if (ThreadCreate(&thdEvent, EventPacketThread, pEnvEvent))
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //创建当前GUI环境的调色板
        pEnvPalette = CreatePalette(256);
        if (NULL == pEnvPalette)
        {
            iErr = -4;
        }
        SetCurrPalette(pEnvPalette);
        //创建当前GUI环境的画笔
        pEnvPen = CreatePen(GUIPEN_TYP_SOLID, 1, 0x0);
        if (NULL == pEnvPen)
        {
            iErr = -4;
        }
        SetCurrPen(pEnvPen);
        //创建当前GUI环境的画刷
        pEnvBrush = CreateBrush(GUIBRUSH_TYP_SOLID);
        if (NULL == pEnvBrush)
        {
            iErr = -4;
        }
        SetCurrBrush(pEnvBrush);
        //创建当前GUI环境的字体
        pEnvFont = CreateFont(FNTFILE_STD, 16, 16, 0x00FFFFFF, 0xFFFFFFFF);
        if (NULL == pEnvFont)
        {
            iErr = -4;
        }
        SetCurrFont(pEnvFont);
    }

    return iErr;
}


/***
  * 功能：
        释放GUI环境，回收资源
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ReleaseGUI(void)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //销毁当前GUI环境的字体
        if (DestroyFont(&pEnvFont))
        {
            iErr = -1;
        }
        //销毁当前GUI环境的画笔
        if (DestroyPen(&pEnvPen))
        {
            iErr = -1;
        }
        //销毁当前GUI环境的画刷
        if (DestroyBrush(&pEnvBrush))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //结束当前GUI环境的事件封装线程
        if (ThreadJoin(thdEvent, NULL))
        {
            iErr = -2;
        }
        //结束当前GUI环境的调度线程
        if (ThreadJoin(thdScheduler, NULL))
        {
            iErr = -2;
        }
        //销毁当前GUI环境的消息对象
        MutexLock(&(pEnvMessage->Mutex));
        ClearMessageQueue(pEnvMessage);
        ClearMessageReg(pEnvMessage);
        MutexUnlock(&(pEnvMessage->Mutex));
        if (DestroyMessage(&pEnvMessage))
        {
            iErr = -2;
        }
        //销毁当前GUI环境的事件对象
        MutexLock(&(pEnvEvent->Mutex));
        ClearEventQueue(pEnvEvent);
        MutexUnlock(&(pEnvEvent->Mutex));
        if (DestroyEvent(&pEnvEvent))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //销毁当前GUI环境的触摸屏对象
        // if (DestroyTouch(&pEnvTouch))
        // {
        //     iErr = -3;
        // }
        //销毁当前GUI环境的按键对象
        if (DestroyKeypad(&pEnvKeypad))
        {
            //iErr = -3;
        }
        //销毁当前GUI环境的帧缓冲映射
        if (DestroyFbmap(&pEnvFbmap))
        {
            iErr = -3;
        }
    }

    return iErr;
}

