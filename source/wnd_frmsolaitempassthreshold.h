/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolaitempassthreshold.h
* 摘    要：  声明初始化窗体wnd_frmsolaitempassthreshold的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wbq
* 完成日期：  2016-1-10
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#ifndef _WND_FRMSOLAITEMPASSTHRESHOLD_H
#define _WND_FRMSOLAITEMPASSTHRESHOLD_H
#include "guiglobal.h"
#include "wnd_global.h"

//窗体frmsolaItemPassThreshold的初始化函数，建立窗体控件、注册消息处理
int FrmSolaItemPassThresholdInit(void *pWndObj);
//窗体frmsolaItemPassThreshold的退出函数，释放所有资源
int FrmSolaItemPassThresholdExit(void *pWndObj);
//窗体frmsolaItemPassThreshold的绘制函数，绘制整个窗体
int FrmSolaItemPassThresholdPaint(void *pWndObj);
//窗体frmsolaItemPassThreshold的循环函数，进行窗体循环
int FrmSolaItemPassThresholdLoop(void *pWndObj);
//窗体frmsolaItemPassThreshold的挂起函数，进行窗体挂起前预处理
int FrmSolaItemPassThresholdPause(void *pWndObj);
//窗体frmsolaItemPassThreshold的恢复函数，进行窗体恢复前预处理
int FrmSolaItemPassThresholdResume(void *pWndObj);

GUIWINDOW* CreateSolaItemPassThresholdWindow();
void setSolaItemPassThresholdWavelength(int wave);
#endif  //_WND_FRMSOLAITEMPASSTHRESHOLD_H

