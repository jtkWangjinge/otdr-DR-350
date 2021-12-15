/**************************************************************
* Copyright(c)2017,宇佳软甲技术有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmSerialNumber.h
* 摘    要：  本机序列号输入界面
*
* 当前版本：  v0.0.1
* 作    者：  wbq
* 完成日期：  2017-9
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/
#ifndef _WND_FrmSerialNumber_H
#define _WND_FrmSerialNumber_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/

#include "guiglobal.h"
#include "wnd_global.h"
/*********************
* 声明窗体处理相关函数
*********************/


//窗体FrmSerialNumber的初始化函数，建立窗体控件、注册
//消息处理
int FrmSerialNumberInit(void *pWndObj);
//窗体FrmSerialNumber的退出函数，释放所有资源
int FrmSerialNumberExit(void *pWndObj);
//窗体FrmSerialNumber的绘制函数，绘制整个窗体
int FrmSerialNumberPaint(void *pWndObj);
//窗体FrmSerialNumber的循环函数，进行窗体循环
int FrmSerialNumberLoop(void *pWndObj);
//窗体FrmSerialNumber的挂起函数，进行窗体挂起前预处理
int FrmSerialNumberPause(void *pWndObj);
//窗体FrmSerialNumber的恢复函数，进行窗体恢复前预处理
int FrmSerialNumberResume(void *pWndObj);


/***
  * 功能：
		建立对话框
  * 参数：
		1.返回按钮的使能标记 : 	对话框左上角的横坐标
		2.okfunc	         : 	按下OK按钮后执行的回调函数S
  * 返回：void
  * 备注：
  		如果没有需要在按下ok按钮后需要处理的回调函数，可输入NULL
***/
void serialNumberDialogInit(int returnBtnEnable,CALLLBACKWINDOW func);

//构造窗体
GUIWINDOW* CreateSerialNumberWindow();
#endif  //_WND_FrmAbout_H

