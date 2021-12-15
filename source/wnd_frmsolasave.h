/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmSOLASave.h
* 摘    要：  声明主窗体FrmSOLASave的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期： 
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMSOLASAVE_H
#define _WND_FRMSOLASAVE_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/

#include "guiglobal.h"
#include "app_curve.h"

/*********************
* 声明窗体处理相关函数
*********************/
//窗体FrmSOLASave的初始化函数，建立窗体控件、注册消息处理
int FrmSOLASaveInit(void *pWndObj);
//窗体FrmSOLASave的退出函数，释放所有资源
int FrmSOLASaveExit(void *pWndObj);
//窗体FrmSOLASave的绘制函数，绘制整个窗体
int FrmSOLASavePaint(void *pWndObj);
//窗体FrmSOLASave的循环函数，进行窗体循环
int FrmSOLASaveLoop(void *pWndObj);
//窗体FrmSOLASave的挂起函数，进行窗体挂起前预处理
int FrmSOLASavePause(void *pWndObj);
//窗体FrmSOLASave的恢复函数，进行窗体恢复前预处理
int FrmSOLASaveResume(void *pWndObj);

#endif  //_WND_FRMSOLASAVE_H


