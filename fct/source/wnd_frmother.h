/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmain.h
* 摘    要：  声明窗体frmother的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
*******************************************************************************/

#ifndef __WND_FRMATT_H__
#define __WND_FRMATT_H__

// #include "guiglobal.h"

int FrmAttInit(void *pWndObj);
int FrmAttExit(void *pWndObj);
int FrmAttPaint(void *pWndObj);
int FrmAttLoop(void *pWndObj);
int FrmAttPause(void *pWndObj);
int FrmAttResume(void *pWndObj);
	
#endif /* __WND_FRMAtt_H__ */

