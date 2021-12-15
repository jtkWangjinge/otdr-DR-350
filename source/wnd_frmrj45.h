﻿/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmrj45.h
* 摘    要：  声明主窗体wnd_frmrj45的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRM_RJ45_H_
#define _WND_FRM_RJ45_H_

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmrj45的初始化函数，建立窗体控件、注册消息处理
int FrmRJ45Init(void *pWndObj);
//窗体frmrj45的退出函数，释放所有资源
int FrmRJ45Exit(void *pWndObj);
//窗体frmrj45的绘制函数，绘制整个窗体
int FrmRJ45Paint(void *pWndObj);
//窗体frmrj45的循环函数，进行窗体循环
int FrmRJ45Loop(void *pWndObj);
//窗体frmrj45的挂起函数，进行窗体挂起前预处理
int FrmRJ45Pause(void *pWndObj);
//窗体frmrj45的恢复函数，进行窗体恢复前预处理
int FrmRJ45Resume(void *pWndObj);

#endif // !_WND_FRM_RJ45_H_
