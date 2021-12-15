/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifiwidget.h
* 摘    要：  wifi信息控件整合
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
#ifndef _WND_FRMWIFIWIDGET_H
#define _WND_FRMWIFIWIDGET_H

#include "guibase.h"
#include "guiglobal.h"

//定义调试信息(Level显示)
//wifi控件信息结构
typedef struct WifiWidgetTag 
{
	GUIVISIBLE  Visible;	//窗体控件可视化信息
	GUIPICTURE  *pBg;		//wifi_item背景图
	GUILABEL    *pName;		//wifi  ssid名称
	GUIPICTURE  *pLock;		//wifi加密锁图标
	GUIPICTURE  *pLevel;	//wifi信号强度图标
	int 		iLock;		//wifi加密值(数据，默认为0)
	int      	iLevel;		//wifi信号值(数据，默认为0)
	char		**pLockBmp;	//wifi加密图(数据，默认为未选中-无加密)
	char		**pLvlBmp;	//wifi强度图(数据，默认为未选中-强度值0)
} WIFIWIDGET;

//wifi控件的初始化函数
WIFIWIDGET* CreateWifiWidget(int iStartX, int iStartY);

//WIFI控件的显示函数
int DisplayWifiWidget(WIFIWIDGET *pWifiObj);

//根据data设置具体的参数信息
int SetWifiWidgetData(WIFIWIDGET *pWifiObj, char *pStrName
					,int iLock, int iLevel);

//设置wifi控件的字体
int SetWifiWidgetFont(WIFIWIDGET *pWifiObj, GUIFONT *pFont);

//整体item控件按下
void WifiWidgetDown(WIFIWIDGET *pWifiObj);

//整体item控件松开
void WifiWidgetUp(WIFIWIDGET *pWifiObj, int index);

//设置wifi控件是否使能
int SetWifiwidgeEnable(int iEnable, WIFIWIDGET *pPicObj);

#endif //_WND_FRMWIFIWIDGET_H


