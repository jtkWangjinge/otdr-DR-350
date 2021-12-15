/*******************************************************************************
* Copyright(c)2017，宇佳软件有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmhirepurchase.h
* 摘    要：  实现主窗体wnd_frmhirepurchase的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  v1.0.0
* 原 作 者：  wbq
* 完成日期：  2017/9
*******************************************************************************/
#ifndef _WND_FRMHIREPURCHASE_H
#define _WND_FRMHIREPURCHASE_H

#include "guiglobal.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmhirepurchase的初始化函数，建立窗体控件、注册消息处理
int FrmHirePurchaseInit(void *pWndObj);
//窗体frmhirepurchase的退出函数，释放所有资源
int FrmHirePurchasePaint(void *pWndObj);
//窗体frmhirepurchase的绘制函数，绘制整个窗体
int FrmHirePurchaseLoop(void *pWndObj);
//窗体frmhirepurchase的循环函数，进行窗体循环
int FrmHirePurchasePause(void *pWndObj);
//窗体frmhirepurchase的挂起函数，进行窗体挂起前预处理
int FrmHirePurchaseResume(void *pWndObj);
//窗体frmhirepurchase的恢复函数，进行窗体恢复前预处理
int FrmHirePurchaseExit(void *pWndObj);

GUIWINDOW* CreateFactoryHirePurchaseWindow();
#endif	