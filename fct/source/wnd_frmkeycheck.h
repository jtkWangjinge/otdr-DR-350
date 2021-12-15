/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmkeycheck.h
* 摘    要：  实现二级窗体keycheck(按键检查)的窗体处理及相关操作函数。该窗
*             体用于提供检查按键操作是否实现。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/



#ifndef __WND_FRMKEYCHECK_H__
#define __WND_FRMKEYCHECK_H__

#include "guiglobal.h"

int FrmKeyCheckInit(void *pWndObj);
int FrmKeyCheckExit(void *pWndObj);
int FrmKeyCheckPaint(void *pWndObj);
int FrmKeyCheckLoop(void *pWndObj);
int FrmKeyCheckPause(void *pWndObj);
int FrmKeyCheckResume(void *pWndObj);


#endif