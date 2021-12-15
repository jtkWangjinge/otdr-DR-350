/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsola.h
* 摘    要：  声明初始化窗体wnd_frmsola的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2016-10-28
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#ifndef _WND_FRMSOLA_H
#define _WND_FRMSOLA_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "wnd_frmsolawidget.h"
#include "wnd_frmsolatable.h"

/*********************
* 声明窗体处理相关函数
*********************/
//窗体frmsola的初始化函数，建立窗体控件、注册消息处理
int FrmSolaInit(void *pWndObj);
//窗体frmsola的退出函数，释放所有资源
int FrmSolaExit(void *pWndObj);
//窗体frmsola的绘制函数，绘制整个窗体
int FrmSolaPaint(void *pWndObj);
//窗体frmsola的循环函数，进行窗体循环
int FrmSolaLoop(void *pWndObj);
//窗体frmsola的挂起函数，进行窗体挂起前预处理
int FrmSolaPause(void *pWndObj);
//窗体frmsola的恢复函数，进行窗体恢复前预处理
int FrmSolaResume(void *pWndObj);

GUIWINDOW* CreateSolaWindow();
int SolaViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where);
void clearSolaEvents();
int WriteSola(char* fileName, int checkFile);

SOLA_WIDGET* getSolaWidget();
SOLA_TABLE_EVENT_ITEM* getSolaTableEventItem();
SOLA_TABLE_PASS* getSolaTablePass();
SOLA_TABLE_LINE* getSolaTableLine();
SolaEvents *getCurrSolaEvents();
//获得当前曲线测量时间 成功返回0. 失败返回非0
int getSolaMeasureDate(char *date, char *time, int dateFormat);
//设置提示框是否显示
void setReminderDialogFlag(int iFlag);
#endif  //_WND_FRMSOLA_H

