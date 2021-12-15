/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolaidentify.h
* 摘    要：  声明初始化窗体wnd_frmsolaidentify的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wbq
* 完成日期：  2016-12-30
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#ifndef _WND_FRMSOLAIDENTIFY_H
#define _WND_FRMSOLAIDENTIFY_H
#include "guiglobal.h"
#include "wnd_global.h"

//窗体frmsolaIdentify的初始化函数，建立窗体控件、注册消息处理
int FrmSolaIdentifyInit(void *pWndObj);
//窗体frmsolaSetting的退出函数，释放所有资源
int FrmSolaIdentifyExit(void *pWndObj);
//窗体frmsolaIdentify的绘制函数，绘制整个窗体
int FrmSolaIdentifyPaint(void *pWndObj);
//窗体frmsolaIdentify的循环函数，进行窗体循环
int FrmSolaIdentifyLoop(void *pWndObj);
//窗体frmsolaIdentify的挂起函数，进行窗体挂起前预处理
int FrmSolaIdentifyPause(void *pWndObj);
//窗体frmsolaIdentify的恢复函数，进行窗体恢复前预处理
int FrmSolaIdentifyResume(void *pWndObj);

GUIWINDOW* CreateSolaIdentifyWindow();
void setSolaIdentifyWavelength(int wave);
#endif  //_WND_FRMSOLAIDENTIFY_H
