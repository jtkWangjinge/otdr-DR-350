/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_global.c
* 摘    要：  实现各窗体的公共数据、公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2012-12-31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_global.h"


/***************************************
* 为实现wnd_global而需要引用的其他头文件
***************************************/
//#include "app_global.h"
#include <sys/types.h>
#include <sys/wait.h>

/***
  * 功能：
        发送消息以便退出当前窗体
  * 参数：
        1.GUIWINDOW *pWndObj:   需要退出的当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendMsg_ExitWindow(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送窗体消息GUIMESSAGE_WND_EXIT给自身
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_WND_EXIT;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, pMsg); //pItem由本窗体的消息处理释放
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * 功能：
        发送消息以便调用新的窗体
  * 参数：
        1.GUIWINDOW *pWndObj:   需要调用的新的窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendMsg_CallWindow(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_THD_CREAT给调度线程以便调度新窗体
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_SYS;
    pItem->iMsgCode = GUIMESSAGE_THD_CREAT;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, GetCurrMessage()); //pItem由调度线程释放
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * 功能：
        发送消息以便允许并启动窗体循环
  * 参数：
        1.GUIWINDOW *pWndObj:   需要允许并启动窗体循环的当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendMsg_EnableLoop(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_THD_CREAT给调度线程以便调度新窗体
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_LOP_ENABLE;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, GetCurrMessage()); //pItem由调度线程释放
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * 功能：
        发送消息以便禁止并停止窗体循环
  * 参数：
        1.GUIWINDOW *pWndObj:   需要禁止并停止窗体循环的当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SendMsg_DisableLoop(GUIWINDOW *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    pMsg = GetCurrMessage();

    //发送系统消息GUIMESSAGE_THD_CREAT给调度线程以便调度新窗体
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_LOP_DISABLE;
    pItem->uiMsgValue = (unsigned int) pWndObj;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, GetCurrMessage()); //pItem由调度线程释放
    MutexUnlock(&(pMsg->Mutex));

    return iReturn;
}


/***
  * 功能：
        显示功能按钮按下
  * 参数：
        1.GUIPICTURE *pBtnFx:   需要显示的功能按钮图形框
        2.GUILABEL *pLblFx:     需要显示的功能按钮标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowBtn_FuncDown(GUIPICTURE *pBtnFx, GUILABEL *pLblFx)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"btn_active.bmp", pBtnFx);
    DisplayPicture(pBtnFx);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x003C3028, 0xFFFFFFFF, pFont);
    DisplayLabel(pLblFx);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


/***
  * 功能：
        显示功能按钮弹起
  * 参数：
        1.GUIPICTURE *pBtnFx:   需要显示的功能按钮图形框
        2.GUILABEL *pLblFx:     需要显示的功能按钮标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowBtn_FuncUp(GUIPICTURE *pBtnFx, GUILABEL *pLblFx)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

    SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pBtnFx);
    DisplayPicture(pBtnFx);

    DisplayLabel(pLblFx);

    return iReturn;
}


/***
  * 功能：
        显示当前日期
  * 参数：
        1.GUILABEL *pLblDate:   需要显示的当前日期标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowSta_CurrDate(GUILABEL *pLblDate)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

    return iReturn;
}


/***
  * 功能：
        显示当前时间
  * 参数：
        1.GUILABEL *pLblTime:   需要显示的当前时间标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowSta_CurrTime(GUILABEL *pLblTime)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

    return iReturn;
}


/***
  * 功能：
        显示LCD背光亮度
  * 参数：
        1.GUILABEL *pLblLight:  需要显示的当前亮度标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowSta_LcdLight(GUILABEL *pLblLight)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

    return iReturn;
}


/***
  * 功能：
        显示电源信息
  * 参数：
        1.GUIPICTURE *pIcoPower:    需要显示的电源状态图形框
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowSta_PowerInfo(GUIPICTURE *pIcoPower)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

    return iReturn;
}

/***
  * 功能：
        更新LABEL的文本资源
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int UpdateLabelRes(GUICHAR *pTargetStr, GUILABEL *pTargetLbl, GUIPICTURE *pBackGround)
{
    //错误标志、返回值定义
	int iReturn = 0;
	GUIPEN *pPen = NULL;
	GUIRECTANGLE *pRectangle = NULL;
	unsigned int uiColorBack = 0;
	
	if (iReturn == 0)
	{
		//参数检查
		if ((NULL == pTargetLbl) || (NULL == pTargetStr))
		{
			iReturn = -1;
		}
	}
	
	if (iReturn == 0)
	{
		//背景图片不为NULL，则需要重绘背景
		if (pBackGround != NULL)
		{
			DisplayPicture(pBackGround);
		}
		//背景为NULL，则重绘黑色矩形为背景
		else
		{
			pPen = GetCurrPen();
			uiColorBack = pPen->uiPenColor;
			pPen->uiPenColor = 0;
			pRectangle = &(pTargetLbl->Visible.Area);
			DrawBlock(pRectangle->Start.x, pRectangle->Start.y, pRectangle->End.x, pRectangle->End.y);
			pPen->uiPenColor = uiColorBack;
		}
		
		//检查标签是否可见
		if (pTargetLbl->Visible.iEnable == 0)
		{
			iReturn = -2;
		}
	}
	
	if (iReturn == 0)
	{
		SetLabelText(pTargetStr, pTargetLbl);
		DisplayLabel(pTargetLbl);
	}

	return iReturn;
}


/***
  * 功能：
		自己实现system系统调用
  * 参数：
		1.const char * cmdstring:	 需要执行的命令
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int my_system(const char * cmdstring) 
{ 
	pid_t pid; 
	int status; 
	
	if(cmdstring == NULL) 
	{ 
		return (1); //如果cmdstring为空，返回非零值，一般为1 
	} 

	if((pid = vfork())<0) 
	{ 
		status = -1; //fork失败，返回-1 
	} 
	else if(pid == 0) 
	{ 
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0); 
		_exit(127); // exec执行失败返回127，注意exec只在失败时才返回现在的进程，成功的话现在的进程就不存在啦~~ 
	} 
	else //父进程 
	{ 
		while(waitpid(pid, &status, 0) < 0) 
		{ 
			if(errno != EINTR) 
			{ 
				status = -1; //如果waitpid被信号中断，则返回-1 
				break; 
			} 
		} 

	} 

	return status; //如果waitpid成功，则返回子进程的返回状态 
} 

