/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmapd.h
* 摘    要：  实现主窗体frmapd的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-30
*
*******************************************************************************/

#ifndef __WND_FRMAPD_H__
#define __WND_FRMAPD_H__

#include "common_opm.h"

int FrmApdInit(void *pWndObj);
int FrmApdExit(void *pWndObj);
int FrmApdPaint(void *pWndObj);
int FrmApdLoop(void *pWndObj);

#endif /* __WND_FRMAPD_H__ */

