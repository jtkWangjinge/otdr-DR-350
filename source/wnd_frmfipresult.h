/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfipresult.h
* 摘    要：  声明主窗体FrmFip的窗体处理线程及相关操
				作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2017-1-19
*
* 取代版本：	
* 原 作 者：	
* 完成日期：	
*******************************************************************************/

#ifndef _WND_FRMFIPRESULT_H
#define _WND_FRMFIPRESULT_H


/*******************************************************************************
***                     为使用GUI而需要引用的头文件                          ***
*******************************************************************************/
#include "guiglobal.h"


/*******************************************************************************
***                        声明窗体处理相关函数                              ***
*******************************************************************************/
//窗体FrmFip的初始化函数，建立窗体控件、注册消息
int FrmFipResultInit(void *pWndObj);
//窗体FrmFip的退出函数，释放所有资源
int FrmFipResultExit(void *pWndObj);
//窗体FrmFip的绘制函数，绘制整个窗体
int FrmFipResultPaint(void *pWndObj);
//窗体FrmFip的循环函数，进行窗体循环
int FrmFipResultLoop(void *pWndObj);
//窗体FrmFip的挂起函数，进行窗体挂起前预处理
int FrmFipResultPause(void *pWndObj);
//窗体FrmFip的恢复函数，进行窗体恢复前预处理
int FrmFipResultResume(void *pWndObj);


#endif  //_WND_FRMFIPRESULT_H

