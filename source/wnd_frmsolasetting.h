/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolasetting.h
* 摘    要：  声明初始化窗体wnd_frmsolasetting的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2016-11-7
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#ifndef _WND_FRMSOLASETTING_H
#define _WND_FRMSOLASETTING_H
#include "guiglobal.h"
#include "wnd_global.h"

//窗体frmsolaSetting的初始化函数，建立窗体控件、注册消息处理
int FrmSolaSettingInit(void *pWndObj);
//窗体frmsolaSetting的退出函数，释放所有资源
int FrmSolaSettingExit(void *pWndObj);
//窗体frmsolaSetting的绘制函数，绘制整个窗体
int FrmSolaSettingPaint(void *pWndObj);
//窗体frmsolaSetting的循环函数，进行窗体循环
int FrmSolaSettingLoop(void *pWndObj);
//窗体frmsolaSetting的挂起函数，进行窗体挂起前预处理
int FrmSolaSettingPause(void *pWndObj);
//窗体frmsolaSetting的恢复函数，进行窗体恢复前预处理
int FrmSolaSettingResume(void *pWndObj);

GUIWINDOW* CreateSolaSettingWindow();
void setSolaSettingWavelength(int wave);
#endif  //_WND_FRMSOLASETTING_H

