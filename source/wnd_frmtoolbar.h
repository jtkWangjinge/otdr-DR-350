/*******************************************************************************
* Copyright(c)2015，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmtoolbar.h
* 摘    要：  提供otdr窗体左侧工具栏
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015.04.17
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMTOOLBAR_H
#define _WND_FRMTOOLBAR_H

/*******************************************
* 为实现frmtoolbar控件而需要引用的其他头文件
*******************************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "wnd_frmdialog.h"


//定义工具按钮对应编码
#define 	TB_HAND		    0
#define 	TB_EZOOM		1
#define 	TB_ZOOM_11		2
#define 	TB_ZOOM_IN		3
#define 	TB_ZOOM_OUT		4
#define		TB_FULL			5


//定义工具栏回调函数
typedef void (*TBACTION)(int);


//工具数量
#define TOOLS_NUM			6


//定义工具栏数据结构
typedef struct wnd_tool_bar
{
	GUIPICTURE 	*pToolBtn[TOOLS_NUM];
	TBACTION	pDnAction;
	TBACTION	pUpAction;
	int 		pEnable[TOOLS_NUM];
}WNDTOOLBAR;

/*************************
* 声明frmtoolbar的操作函数
*************************/
//创建工具栏
WNDTOOLBAR *CreateToolBar(TBACTION pDnAction, TBACTION pUpAction,
						  GUIWINDOW *pWnd);
//销毁工具栏
int DestroyToolBar(WNDTOOLBAR **pTBObj);
//绘制工具栏
int DisplayToolBar(WNDTOOLBAR *pTBObj);

/***
  * 功能：
     	设置工具栏的图标状态
  * 参数：
  		int 
  		int iNum:       对应的第几个图标  0~5
  		int iStatus:    0----unpress状态     1----press状态
  * 返回：
        成功返0，失败返回-1
  * 备注：
***/ 
int SetPicStatus(int iNum,int iStatus,WNDTOOLBAR *pTBObj);

#endif //_WND_FRMTOOLBAR_H
