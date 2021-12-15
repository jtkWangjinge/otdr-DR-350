/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmainopt.h
* 摘    要：  声明主窗体frmmainopt的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
*******************************************************************************/

#ifndef _WND_FRMMAINOPT_H
#define _WND_FRMMAINOPT_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
// #include "guiglobal.h"


/*********************
* 声明窗体处理相关函数
*********************/
//窗体frmmain的初始化函数，建立窗体控件、注册消息处理
int FrmMainOptInit(void *pWndObj);
//窗体frmmain的退出函数，释放所有资源
int FrmMainOptExit(void *pWndObj);
//窗体frmmain的绘制函数，绘制整个窗体
int FrmMainOptPaint(void *pWndObj);
//窗体frmmain的循环函数，进行窗体循环
int FrmMainOptLoop(void *pWndObj);
//窗体frmmain的挂起函数，进行窗体挂起前预处理
int FrmMainOptPause(void *pWndObj);
//窗体frmmain的恢复函数，进行窗体恢复前预处理
int FrmMainOptResume(void *pWndObj);

#endif  //_WND_FRMMAINOPT_H

