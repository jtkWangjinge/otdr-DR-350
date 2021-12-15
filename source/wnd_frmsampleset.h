/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsampleset.h
* 摘    要：  实现主窗体frmsampleset的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014.10.21
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMSAMPLESET_H
#define _WND_FRMSAMPLESET_H

int FrmSampleSetInit(void *pWndObj);
int FrmSampleSetPaint(void *pWndObj);
int FrmSampleSetLoop(void *pWndObj);
int FrmSampleSetPause(void *pWndObj);
int FrmSampleSetResume(void *pWndObj);
int FrmSampleSetExit(void *pWndObj);

#endif	//_WND_FRMSAMPLESET_H


