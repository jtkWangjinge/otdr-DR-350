/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlogo.h
* 摘    要：  声明LOGO窗体frmlogo的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMLOGO_H
#define _WND_FRMLOGO_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"
#include "wnd_frmotdrmeas.h"


/*********************
* 声明窗体处理相关函数
*********************/
//窗体frmlogo的初始化函数，建立窗体控件、注册消息处理
int FrmLogoInit(void *pWndObj);
//窗体frmlogo的退出函数，释放所有资源
int FrmLogoExit(void *pWndObj);
//窗体frmlogo的绘制函数，绘制整个窗体
int FrmLogoPaint(void *pWndObj);
//窗体frmlogo的循环函数，进行窗体循环
int FrmLogoLoop(void *pWndObj);
//窗体frmlogo的挂起函数，进行窗体挂起前预处理
int FrmLogoPause(void *pWndObj);
//窗体frmlogo的恢复函数，进行窗体恢复前预处理
int FrmLogoResume(void *pWndObj);

void *InitThreadFunc(void *pThreadArg);

#endif  //_WND_FRMLOGO_H

