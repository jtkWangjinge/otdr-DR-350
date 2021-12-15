/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcablesearch.h
* 摘    要：  声明主窗体wnd_frmcablesearch的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRM_CABLE_SEARCH_H_
#define _WND_FRM_CABLE_SEARCH_H_


/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmcablesearch的初始化函数，建立窗体控件、注册消息处理
int FrmCableSearchInit(void *pWndObj);
//窗体frmcablesearch的退出函数，释放所有资源
int FrmCableSearchExit(void *pWndObj);
//窗体frmcablesearch的绘制函数，绘制整个窗体
int FrmCableSearchPaint(void *pWndObj);
//窗体frmcablesearch的循环函数，进行窗体循环
int FrmCableSearchLoop(void *pWndObj);
//窗体frmcablesearch的挂起函数，进行窗体挂起前预处理
int FrmCableSearchPause(void *pWndObj);
//窗体frmcablesearch的恢复函数，进行窗体恢复前预处理
int FrmCableSearchResume(void *pWndObj);


#endif// !_WND_FRM_CABLE_SEARCH_H_