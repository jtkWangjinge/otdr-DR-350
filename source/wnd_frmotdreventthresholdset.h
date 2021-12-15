/*******************************************************************************
* Copyright(c)2017，大豪信息技术(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdreventthresholdset.h
* 摘    要：  实现主窗体frmotdreventthresholdset的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  Bihong
* 完成日期：  2017.8.16
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMOTDREVENTTHRESHOTHSET_H
#define _WND_FRMOTDREVENTTHRESHOTHSET_H

int FrmOtdrEventThresholdSetInit(void *pWndObj);
int FrmOtdrEventThresholdSetPaint(void *pWndObj);
int FrmOtdrEventThresholdSetLoop(void *pWndObj);
int FrmOtdrEventThresholdSetPause(void *pWndObj);
int FrmOtdrEventThresholdSetResume(void *pWndObj);
int FrmOtdrEventThresholdSetExit(void *pWndObj);

#endif	//_WND_FRMOTDREVENTTHRESHOTHSET_H


