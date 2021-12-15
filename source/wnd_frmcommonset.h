/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcommonset.h
* 摘    要：  实现主窗体frmcommonset的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014.10.21
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMCOMMONSET_H
#define _WND_FRMCOMMONSET_H

#include "guiglobal.h"

#define OTDR_WAVE_LENGTH_NUM        3           //波长个数

int FrmCommonSetInit(void *pWndObj);
int FrmCommonSetPaint(void *pWndObj);
int FrmCommonSetLoop(void *pWndObj);
int FrmCommonSetPause(void *pWndObj);
int FrmCommonSetResume(void *pWndObj);
int FrmCommonSetExit(void *pWndObj);

#endif	//_WND_FRMCOMMONSET_H


