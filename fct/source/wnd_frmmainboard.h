/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmain.h
* 摘    要：  声明主窗体frmmain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef __WND_FRMMAINBOARD_H__
#define __WND_FRMMAINBOARD_H__

#include "guiglobal.h"

int FrmMainBoardInit(void *pWndObj);
int FrmMainBoardExit(void *pWndObj);
int FrmMainBoardPaint(void *pWndObj);
int FrmMainBoardLoop(void *pWndObj);
int FrmMainBoardPause(void *pWndObj);
int FrmMainBoardResume(void *pWndObj);

#endif /* __WND_FRMMAINBOARD_H__ */

