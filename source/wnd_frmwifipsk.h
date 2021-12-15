/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifipsk.h
* 摘    要：  显示wifi的连接窗口
*             
*
* 当前版本：  v1.0.0
* 作    者：  xiazhizhu
* 完成日期：  2016-4-7
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMWIFIPSK_H
#define _WND_FRMWIFIPSK_H

#include "appwifi.h"

//窗体pGuiWifi初始化函数，建立窗体控件、注册消息处理
int GuiWifiInit(void *pWndObj);
//窗体pGuiWifi退出函数，释放所有资源
int GuiWifiExit(void *pWndObj);
//窗体pGuiWifi绘制函数，绘制整个窗体
int GuiWifiPaint(void *pWndObj);
//窗体pGuiWifi循环函数，进行窗体循环
int GuiWifiLoop(void *pWndObj);
//窗体pGuiWifi的挂起函数，进行窗体挂起前预处理
int GuiWifiPause(void *pWndObj);
//窗体pGuiWifi的恢复函数，进行窗体恢复前预处理
int GuiWifiResume(void *pWndObj);


typedef struct
{
	int iCount;
	char buff[512];
}PSK_INFO;


int DisplayPskWindow(void *pWndObj, NETWORK_INFO_SINGLE WifiInfo);

int WiFiConnectCallBack(char Connectflag);

int WiFiGetIPCallBack(char GetIPflag);


#endif //_WND_FRMWIFIPSK_H


