/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmAbout.h
* 摘    要：  声明主窗体FrmAbout 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v0.0.1
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/
#ifndef _WND_FrmAbout_H
#define _WND_FrmAbout_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/

#include "guiglobal.h"
#include "wnd_global.h"
/*********************
* 声明窗体处理相关函数
*********************/


//窗体FrmAbout的初始化函数，建立窗体控件、注册
//消息处理
int FrmAboutInit(void *pWndObj);
//窗体FrmAbout的退出函数，释放所有资源
int FrmAboutExit(void *pWndObj);
//窗体FrmAbout的绘制函数，绘制整个窗体
int FrmAboutPaint(void *pWndObj);
//窗体FrmAbout的循环函数，进行窗体循环
int FrmAboutLoop(void *pWndObj);
//窗体FrmAbout的挂起函数，进行窗体挂起前预处理
int FrmAboutPause(void *pWndObj);
//窗体FrmAbout的恢复函数，进行窗体恢复前预处理
int FrmAboutResume(void *pWndObj);
void setAboutStyle(int flag);
void setCountryStyle(unsigned int uiCountryStyle);

//窗体构造函数
GUIWINDOW* CreateAboutWindow();
#endif  //_WND_FrmAbout_H

