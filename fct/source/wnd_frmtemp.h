/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmtemp.h
* 摘    要：  声明一级窗体frmotdr(OTDR)的窗体处理线程及相关操作函数。该窗体用于
*             提供OTDR操作。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMTEMP_H
#define _WND_FRMTEMP_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
// #include "guiglobal.h"

int FrmTempInit(void *pWndObj);
int FrmTempExit(void *pWndObj);
int FrmTempPaint(void *pWndObj);
int FrmTempLoop(void *pWndObj);
int FrmTempPause(void *pWndObj);
int FrmTempResume(void *pWndObj);



int FrmTempSetInfo1(char *pInfo);
int FrmTempSetInfo2(char *pInfo);
void initTraceArea(void);
int GetFlag();
#endif  //_WND_FRMTEMP_H

