/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfip.h
* 摘    要：  声明主窗体FrmFip的窗体处理线程及相关操
				作函数
*
* 当前版本：  v1.0.2	
* 作    者：  	
* 完成日期：  2015-5-27
*
* 取代版本：	v1.0.0
* 原 作 者：	
* 完成日期：	2014-8-18
*******************************************************************************/

#ifndef _WND_FRMFIP_H
#define _WND_FRMFIP_H


/*******************************************************************************
***                     为使用GUI而需要引用的头文件                          ***
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"


//定义端面检查自定义的回调函数
typedef void (*FIPCALLLBACK)(GUIWINDOW **);

/*******************************************************************************
***                        声明窗体处理相关函数                              ***
*******************************************************************************/
//窗体FrmFip的初始化函数，建立窗体控件、注册消息
int FrmFipInit(void *pWndObj);
//窗体FrmFip的退出函数，释放所有资源
int FrmFipExit(void *pWndObj);
//窗体FrmFip的绘制函数，绘制整个窗体
int FrmFipPaint(void *pWndObj);
//窗体FrmFip的循环函数，进行窗体循环
int FrmFipLoop(void *pWndObj);
//窗体FrmFip的挂起函数，进行窗体挂起前预处理
int FrmFipPause(void *pWndObj);
//窗体FrmFip的恢复函数，进行窗体恢复前预处理
int FrmFipResume(void *pWndObj);


/*******************************************************************************
***                       frmfip对外提供的接口函数                           ***
*******************************************************************************/
/***
  * 功能：
		调用fip窗体
  * 参数：
		GUIWINDOW *LastWindow   : 上个窗体
		OTDRFIBERCALLLBACK func : 回调函数
  * 返回：
		空
  * 备注：	
***/
void CallFipWnd(GUIWINDOW *LastWindow, FIPCALLLBACK func);

void PthreadJoin();

//使用Fiber Microscope打开GDM文件
int FiberViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where);

#endif  //_WND_FRMFIP_H

