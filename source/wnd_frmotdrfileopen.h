/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmOtdrFileOpen.h
* 摘    要：  声明主窗体wnd_FrmOtdrFileOpen的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者： 	     
* 完成日期：  2015-1-13
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMOTDRFILEOPEN_H
#define _WND_FRMOTDRFILEOPEN_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"
#include "app_frmfilebrowse.h"
#include "wnd_global.h"

/*********************
* 声明窗体处理相关函数
*********************/
//窗体wnd_FrmOtdrFileOpen的初始化函数，建立窗体控件、注册消息处理
int FrmOtdrFileOpenInit(void *pWndObj);
//窗体wnd_FrmOtdrFileOpen的退出函数，释放所有资源
int FrmOtdrFileOpenExit(void *pWndObj);
//窗体wnd_FrmOtdrFileOpen的绘制函数，绘制整个窗体
int FrmOtdrFileOpenPaint(void *pWndObj);
//窗体wnd_FrmOtdrFileOpen的循环函数，进行窗体循环
int FrmOtdrFileOpenLoop(void *pWndObj);
//窗体wnd_FrmOtdrFileOpen的挂起函数，进行窗体挂起前预处理
int FrmOtdrFileOpenPause(void *pWndObj);
//窗体wnd_FrmOtdrFileOpen的恢复函数，进行窗体恢复前预处理
int FrmOtdrFileOpenResume(void *pWndObj);

/***
  * 功能：
		创建用户自定义路径窗体
  * 参数：
  		1、FILEOPERATION enFileOperation:	文件操作类型
  		2、const char *pTopPath			:	允许用户返回的顶层目录
  * 返回：
		无
  * 备注：
***/
int OtdrFileDialog(unsigned int enFileOperation, const char *pTopPath, char *pCurrPath, CALLLBACKWINDOW func, FILETYPE fileType);

//设置初始操作路径
int setOtdrOperateFirstPath(const char *pPath);
#endif  //_WND_FRMOTDRFILEOPEN_H

