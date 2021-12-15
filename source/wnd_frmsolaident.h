/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmSolaIdent.h
* 摘    要：  声明主窗体FrmSolaIdent 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014-12-30
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/

#ifndef _WND_FrmSolaIdent_H
#define _WND_FrmSolaIdent_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"

/*********************
* 声明窗体处理相关函数
*********************/


//窗体FrmSolaIdent的初始化函数，建立窗体控件、注册
//消息处理
int FrmSolaIdentInit(void *pWndObj);
//窗体FrmMark的退出函数，释放所有资源
int FrmSolaIdentExit(void *pWndObj);
//窗体FrmMark的绘制函数，绘制整个窗体
int FrmSolaIdentPaint(void *pWndObj);
//窗体FrmMark的循环函数，进行窗体循环
int FrmSolaIdentLoop(void *pWndObj);
//窗体FrmMark的挂起函数，进行窗体挂起前预处理
int FrmSolaIdentPause(void *pWndObj);
//窗体FrmMark的恢复函数，进行窗体恢复前预处理
int FrmSolaIdentResume(void *pWndObj);

GUIWINDOW* CreateSolaIdentWindow();

#endif  //_WND_FrmSolaIdent_H
