/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlinearity.h
* 摘    要：  
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020/10/28
*
*******************************************************************************/

#ifndef _WND_FRMUNCERTAINTY_H
#define _WND_FRMUNCERTAINTY_H

// #include "guiglobal.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体FrmUncertaintyInit的初始化函数，建立窗体控件、注册消息处理
int FrmUncertaintyInit(void *pWndObj);
//窗体FrmUncertaintyPaint的退出函数，释放所有资源
int FrmUncertaintyPaint(void *pWndObj);
//窗体FrmUncertaintyLoop的绘制函数，绘制整个窗体
int FrmUncertaintyLoop(void *pWndObj);
//窗体FrmUncertaintyPause的循环函数，进行窗体循环
int FrmUncertaintyPause(void *pWndObj);
//窗体FrmUncertaintyResume的挂起函数，进行窗体挂起前预处理
int FrmUncertaintyResume(void *pWndObj);
//窗体FrmUncertaintyExit的恢复函数，进行窗体恢复前预处理
int FrmUncertaintyExit(void *pWndObj);

//按键响应处理函数
void UncertaintyKeyCallBack(int iOption);
#endif	//_WND_FRMUNCERTAINTY_H