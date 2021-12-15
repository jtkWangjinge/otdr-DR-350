/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcablesequence.h
* 摘    要：  声明主窗体wnd_frmcablesequence的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRM_CABLE_SEQUENCE_H_
#define _WND_FRM_CABLE_SEQUENCE_H_

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmCableSequence的初始化函数，建立窗体控件、注册消息处理
int FrmCableSequenceInit(void *pWndObj);
//窗体frmCableSequence的退出函数，释放所有资源
int FrmCableSequenceExit(void *pWndObj);
//窗体frmCableSequence的绘制函数，绘制整个窗体
int FrmCableSequencePaint(void *pWndObj);
//窗体frmCableSequence的循环函数，进行窗体循环
int FrmCableSequenceLoop(void *pWndObj);
//窗体frmCableSequence的挂起函数，进行窗体挂起前预处理
int FrmCableSequencePause(void *pWndObj);
//窗体frmCableSequence的恢复函数，进行窗体恢复前预处理
int FrmCableSequenceResume(void *pWndObj);

//创建网线测序界面
void CreateCableSequenceWindow();

#endif

