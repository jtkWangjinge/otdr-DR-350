/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiglobal.h
* 摘    要：  定义GUI的公共数据、公共操作函数，并提供对于GUI环境的初始化设置。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_GLOBAL_H
#define _GUI_GLOBAL_H


/**************************************
* 为定义GUIGLOBAL而需要引用的其他头文件
**************************************/
#include "guibase.h"
#include "guidevice.h"
#include "guifbmap.h"
#include "guikeypad.h"
#include "guitouch.h"
#include "guievent.h"
#include "guimessage.h"
#include "guischeduler.h"
#include "guipalette.h"
#include "guibrush.h"
#include "guipen.h"
#include "guifont.h"
#include "guidraw.h"
#include "guiimage.h"
#include "guibitmap.h"
#include "guitext.h"
#include "guilabel.h"
#include "guipicture.h"
#include "guiwindow.h"


/**********************************
* 定义与GUI环境初始化相关的操作函数
**********************************/
//初始化GUI环境，分配GUI资源
int InitializeGUI(void);
//释放GUI环境，回收资源
int ReleaseGUI(void);


/***************************************
* 定义用于发送特定类型消息的公共操作函数
***************************************/
//发送系统消息以便建立新的窗体线程
int SendSysMsg_ThreadCreate(GUIWINDOW *pWndObj);
//发送系统消息以便销毁已有窗体线程
int SendSysMsg_ThreadDestroy(GUIWINDOW *pWndObj);

//发送窗体消息以便执行窗体初始化
int SendWndMsg_WindowInit(GUIWINDOW *pWndObj);
//发送窗体消息以便执行窗体退出
int SendWndMsg_WindowExit(GUIWINDOW *pWndObj);
//发送窗体消息以便执行窗体绘制
int SendWndMsg_WindowPaint(GUIWINDOW *pWndObj);
//发送窗体消息以便执行窗体循环
int SendWndMsg_WindowLoop(GUIWINDOW *pWndObj);
//发送窗体消息以便执行窗体挂起
int SendWndMsg_WindowPause(GUIWINDOW *pWndObj);
//发送窗体消息以便执行窗体恢复
int SendWndMsg_WindowResume(GUIWINDOW *pWndObj);
//发送窗体消息以便允许并启动窗体循环
int SendWndMsg_LoopEnable(GUIWINDOW *pWndObj);
//发送窗体消息以便禁止并停止窗体循环
int SendWndMsg_LoopDisable(GUIWINDOW *pWndObj);
//发送窗体消息以便进行错误处理
int SendWndMsg_ErrorProc(void *pErrData, int iDataLen, void *pRecvObj);


#endif  //_GUI_GLOBAL_H

