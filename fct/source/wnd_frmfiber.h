/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfiber.h
* 摘    要：  声明主窗体frmfiber的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-30
*
*******************************************************************************/

#ifndef __WND_FRMFIBER_H__
#define __WND_FRMFIBER_H__

// #include "guiglobal.h"

int FrmFiberInit(void *pWndObj);
int FrmFiberExit(void *pWndObj);
int FrmFiberPaint(void *pWndObj);
int FrmFiberLoop(void *pWndObj);
int FrmFiberPause(void *pWndObj);
int FrmFiberResume(void *pWndObj);
	
#endif /* __WND_FRMFIBER_H__ */


