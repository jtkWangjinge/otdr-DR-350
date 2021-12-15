/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiglobal.c
* 摘    要：  实现与GUI相关的公共操作函数，并提供对于GUI环境的初始化设置。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guiglobal.h"
#include "run/iniparser/include/Skin.h"

/**************************************
* 为实现GUIGLOBAL而需要引用的其他头文件
**************************************/
#include "wnd_global.h"
#include "app_frmfilebrowse.h"

HSKIN hSkin = NULL;
#define SKININIPATH MntSDcardDirectory"Skin.ini"
/******************************
* 定义GUI中与环境配置相关的变量
******************************/
static GUIFBMAP *pEnvFbmap = NULL;
static GUIKEYPAD *pEnvKeypad = NULL;
static GUITOUCH *pEnvTouch = NULL;

MULTISELECT *pEnvQueue = NULL;
static GUIEVENT *pEnvEvent = NULL;
static GUIMESSAGE *pEnvMessage = NULL;

static GUIPALETTE *pEnvPalette = NULL;
static GUIPEN *pEnvPen = NULL;
static GUIBRUSH *pEnvBrush = NULL;
//static GUIFONT *pEnvFont = NULL;

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

    if (!iErr)
    {
        //尝试初始化进程退出标志
        if (InitExitFlag())
        {
            iErr = -1;
        }
    }

    if (!iErr)  //设备抽象层初始化
    {
        //创建当前GUI环境的帧缓冲映射对象
        pEnvFbmap = CreateFbmap(DEVFILE_FB, GUI_FBTYP_DUAL,
                                WINDOW_WIDTH, WINDOW_HEIGHT, 
                                DefaultFbThread);
        if (NULL == pEnvFbmap)
        {
            iErr = -2;
        }
        SetCurrFbmap(pEnvFbmap);
        //创建当前GUI环境的键盘对象
        char eventName[64] = {0};
        GetDeviceEventName(eventName, "F216_KEY");
        pEnvKeypad = CreateKeypad(eventName, GUIKEYPAD_TYP_CUST,
                                  DefaultKeypadThread);
        if (NULL == pEnvKeypad)
        {
            iErr = -2;
        }
        SetCurrKeypad(pEnvKeypad);
#ifdef TOUCH_SCREEN
        //创建当前GUI环境的触摸屏对象
        pEnvTouch = CreateTouch(DEVFILE_TOUCH, GUITOUCH_TYP_CUST,
                                WINDOW_WIDTH, WINDOW_HEIGHT,
                                DefaultTouchThread);
        if (NULL == pEnvTouch)
        {
            iErr = -2;
        }
        SetCurrTouch(pEnvTouch);
#endif
    }

    if (!iErr)  //系统调度模块初始化
    {
		pEnvQueue = CreateQueue(MULTISELECT_LIMIT);
		SetCurrQueue(pEnvQueue);
	
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
        if (ThreadCreate(&thdScheduler, NULL,
                         WindowSchedulerThread, pEnvMessage))
        {
            iErr = -3;
        }
        //初始化当前窗体(事件封装线程将访问当前窗体，必须在之前进行此操作)
        if (InitCurrWindow())
        {
            iErr = -3;
        }
        //创建当前GUI环境的事件封装线程
        if (ThreadCreate(&thdEvent, NULL,
                         EventPacketThread, pEnvEvent))
        {
            iErr = -3;
        }
    }

    if (!iErr)
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

		//获取公共资源 包括字体等
		GetGlobalResource();
		SetCurrFont(getGlobalFnt(EN_FONT_WHITE));
				
		/* end */
        hSkin = SkinCreate(SKININIPATH);
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

    if (!iErr)
    {
        //销毁当前GUI环境的字体
        //if (DestroyFont(&pEnvFont))
        //{
        //    iErr = -1;
        //}
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

    if (!iErr)
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
        ClearMessageQueue(pEnvMessage);
        ClearMessageReg(pEnvMessage);
        if (DestroyMessage(&pEnvMessage))
        {
            iErr = -2;
        }
        //销毁当前GUI环境的事件对象
        ClearEventQueue(pEnvEvent);
        if (DestroyEvent(&pEnvEvent))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //销毁当前GUI环境的触摸屏对象
        if (DestroyTouch(&pEnvTouch))
        {
            iErr = -3;
        }
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
	
	/* start add by  2015.1.20 */
	//释放全局资源区域
	ReleasGlobalResource();
	/* end */	

    return iErr;
}


/***
  * 功能：
        发送系统消息以便建立新的窗体线程
  * 参数：
        1.GUIWINDOW *pWndObj:   需要调用的新的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendSysMsg_ThreadCreate(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_THD_CREAT给窗体调度线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_SYS;
        pItem->iMsgCode = GUIMESSAGE_THD_CREATE;
        pItem->uiMsgValue = (unsigned int) pWndObj;
		iReturn = WriteMessageQueue(pItem, pMsg,0); //pItem由调度线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送系统消息以便销毁已有窗体线程
  * 参数：
        1.GUIWINDOW *pWndObj:   需要调用的新的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendSysMsg_ThreadDestroy(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_THD_DESTROY给窗体调度线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_SYS;
        pItem->iMsgCode = GUIMESSAGE_THD_DESTROY;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        iReturn = WriteMessageQueue(pItem, pMsg, 1); //pItem由调度线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便执行窗体初始化
  * 参数：
        1.GUIWINDOW *pWndObj:   需要初始化的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_WindowInit(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();
    //发送窗体消息GUIMESSAGE_WND_INIT给窗体处理线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_WND_INIT;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        WriteMessageQueue(pItem, pMsg, 0); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便执行窗体退出
  * 参数：
        1.GUIWINDOW *pWndObj:   需要退出的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_WindowExit(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送窗体消息GUIMESSAGE_WND_EXIT给窗体处理线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_WND_EXIT;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        iReturn = WriteMessageQueue(pItem, pMsg, 1); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便执行窗体绘制
  * 参数：
        1.GUIWINDOW *pWndObj:   需要绘制的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_WindowPaint(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送窗体消息GUIMESSAGE_WND_PAINT给窗体处理线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_WND_PAINT;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        WriteMessageQueue(pItem, pMsg ,0); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便执行窗体循环
  * 参数：
        1.GUIWINDOW *pWndObj:   需要绘制的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_WindowLoop(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送窗体消息GUIMESSAGE_WND_LOOP给窗体处理线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_WND_LOOP;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        WriteMessageQueue(pItem, pMsg, 0); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便执行窗体挂起
  * 参数：
        1.GUIWINDOW *pWndObj:   需要绘制的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_WindowPause(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送窗体消息GUIMESSAGE_WND_PAUSE给窗体处理线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_WND_PAUSE;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        WriteMessageQueue(pItem, pMsg, 0); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便执行窗体恢复
  * 参数：
        1.GUIWINDOW *pWndObj:   需要绘制的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_WindowResume(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送窗体消息GUIMESSAGE_WND_RESUME给窗体处理线程
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_WND_RESUME;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        WriteMessageQueue(pItem, pMsg, 0); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便允许并启动窗体循环
  * 参数：
        1.GUIWINDOW *pWndObj:   需要允许并启动窗体循环的当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_LoopEnable(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_LOP_ENABLE给窗体处理线程以便允许并启动窗体循环
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_LOP_ENABLE;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        WriteMessageQueue(pItem, pMsg ,1); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便禁止并停止窗体循环
  * 参数：
        1.GUIWINDOW *pWndObj:   需要禁止并停止窗体循环的当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_LoopDisable(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_LOP_DISABLE给窗体处理线程以便禁止窗体循环
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_LOP_DISABLE;
        pItem->uiMsgValue = (unsigned int) pWndObj;
        WriteMessageQueue(pItem, pMsg, 1); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}


/***
  * 功能：
        发送窗体消息以便进行错误处理
  * 参数：
        1.void *pErrData:   传递给错误处理函数的传入参数
        2.int iDataLen:     指定错误处理函数传入参数的长度
        3.void *pRecvObj:   需要接收错误消息并执行处理的对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendWndMsg_ErrorProc(void *pErrData, int iDataLen, void *pRecvObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_ERR_PROC给窗体处理线程以便执行错误处理
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    if (NULL == pItem)
    {
        iReturn = -1;
    }
    else
    {
        pItem->iMsgType = GUIMESSAGE_TYP_WND;
        pItem->iMsgCode = GUIMESSAGE_ERR_PROC;
        pItem->uiMsgValue = (unsigned int) pErrData;
        pItem->iValueLength = iDataLen;
        pItem->pRecvObj = pRecvObj;
        WriteMessageQueue(pItem, pMsg, 0); //pItem由本窗体的处理线程释放
    }

    return iReturn;
}

