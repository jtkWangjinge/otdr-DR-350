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

#ifndef _WND_FRMLINEARITY_H
#define _WND_FRMLINEARITY_H

// #include "guiglobal.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体FrmLinearityInit的初始化函数，建立窗体控件、注册消息处理
int FrmLinearityInit(void *pWndObj);
//窗体FrmLinearityPaint的退出函数，释放所有资源
int FrmLinearityPaint(void *pWndObj);
//窗体FrmLinearityLoop的绘制函数，绘制整个窗体
int FrmLinearityLoop(void *pWndObj);
//窗体FrmLinearityPause的循环函数，进行窗体循环
int FrmLinearityPause(void *pWndObj);
//窗体FrmLinearityResume的挂起函数，进行窗体挂起前预处理
int FrmLinearityResume(void *pWndObj);
//窗体FrmLinearityExit的恢复函数，进行窗体恢复前预处理
int FrmLinearityExit(void *pWndObj);

//按键响应处理
void LinearityKeyBackCall(int iOption);

#endif	//_WND_FRMLINEARITY_H