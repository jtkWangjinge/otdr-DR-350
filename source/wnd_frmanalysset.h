/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmanalysset.h
* 摘    要：  实现主窗体frmanalysset的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMANALYSSET_H
#define _WND_FRMANALYSSET_H

int FrmAnalysSetInit(void *pWndObj);
int FrmAnalysSetPaint(void *pWndObj);
int FrmAnalysSetLoop(void *pWndObj);
int FrmAnalysSetPause(void *pWndObj);
int FrmAnalysSetResume(void *pWndObj);
int FrmAnalysSetExit(void *pWndObj);

#endif	//_WND_FRMANALYSSET_H


