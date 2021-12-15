/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmtimeset.h
* 摘    要：  实现主窗体frmtimeset的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014.10.11
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMTIMESET_H
#define _WND_FRMTIMESET_H

int FrmTimeSetInit(void *pWndObj);
int FrmTimeSetPaint(void *pWndObj);
int FrmTimeSetLoop(void *pWndObj);
int FrmTimeSetPause(void *pWndObj);
int FrmTimeSetResume(void *pWndObj);
int FrmTimeSetExit(void *pWndObj);

#endif	//_WND_FRMTIMESET_H

