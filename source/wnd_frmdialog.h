/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmdialog.h
* 摘    要：  声明窗体wnd_frmdialog(对话框)的窗体处理线程及相关操作函数。该窗体
*             提供了对话框功能。
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMDIALOG_H
#define _WND_FRMDIALOG_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "wnd_global.h"

/*******************************************************************************
**								变量声明				 					  **
*******************************************************************************/
extern GUICHAR *pDialogStrInfo;


/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmdialog的初始化函数，建立窗体控件、注册消息处理
int FrmDialogInit(void *pWndObj);
//窗体frmdialog的退出函数，释放所有资源
int FrmDialogExit(void *pWndObj);
//窗体frmdialog的绘制函数，绘制整个窗体
int FrmDialogPaint(void *pWndObj);
//窗体frmdialog的循环函数，进行窗体循环
int FrmDialogLoop(void *pWndObj);
//窗体frmdialog的挂起函数，进行窗体挂起前预处理
int FrmDialogPause(void *pWndObj);
//窗体frmdialog的恢复函数，进行窗体恢复前预处理
int FrmDialogResume(void *pWndObj);


/***
  * 功能：
		按下OK按钮后需要显示的信息
  * 参数：
		1.info : 	需要显示的信息
  * 返回：
		无
  * 备注：
***/
void ShowDialogExtraInfo(GUICHAR *info);


/***
  * 功能：
		建立对话框
  * 参数：
		1.x         : 	对话框左上角的横坐标
		2.y         : 	对话框左上角的纵坐标
		3.pTitleStr : 	对话框的标题文本
		4.pTextStr  : 	对话框上要显示的信息
		5.ucIcoType : 	对话框类型 0:警告对话框
		6.ucButtonType:	0、仅显示确定按钮；1、显示确定和取消按钮；2、不显示确定和取消按钮
		7.func	   : 	还原窗体的回调函数
		8.okfunc	   : 	按下OK按钮后执行的回调函数S
		9.cancelfunc: 	按下cancel按钮后执行的回调函数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
  		如果没有需要在按下ok或cancel按钮后需要处理的回调函数，可输入NULL
***/
void DialogInit(int x, int y, GUICHAR *pTitleStr, GUICHAR *pTextStr, 
				unsigned char ucIcoType, unsigned char ucButtonType,
				CALLLBACKWINDOW func, CALLLBACKHANDLE okfunc, CALLLBACKHANDLE cancelfunc);

/***
  * 功能：
        设置是否跳转界面的标志位
  * 参数：
        1.jumpFlag  :   标志位，0：不跳转界面，1：跳转界面
  * 返回：
        无
  * 备注：
        如果需要点击确定或取消按钮时，界面需要跳转到其他界面，则jumpFlag设置为1，否则设置为0
***/

void SetJumpWindowFlag(unsigned char jumpFlag);

#endif
