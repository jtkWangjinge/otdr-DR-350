/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmquicksave.h
* 摘    要：  声明主窗体frmquicksave的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者： 	     
* 完成日期：  2015-3-2
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMQUICKSAVE_H
#define _WND_FRMQUICKSAVE_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/

#include "guiglobal.h"
#include "wnd_global.h"

/****************************
* 为使用GUI而需要引用的头文件
****************************/

/*********************
* 声明窗体处理相关函数
*********************/
//窗体frmquicksave的初始化函数，建立窗体控件、注册消息处理
int FrmQuickSaveInit(void *pWndObj);
//窗体frmquicksave的退出函数，释放所有资源
int FrmQuickSaveExit(void *pWndObj);
//窗体frmquicksave的绘制函数，绘制整个窗体
int FrmQuickSavePaint(void *pWndObj);
//窗体frmquicksave的循环函数，进行窗体循环
int FrmQuickSaveLoop(void *pWndObj);
//窗体frmquicksave的挂起函数，进行窗体挂起前预处理
int FrmQuickSavePause(void *pWndObj);
//窗体frmquicksave的恢复函数，进行窗体恢复前预处理
int FrmQuickSaveResume(void *pWndObj);

//设置传入快速储存界面时的文件名字
void SetQuickSaveName(char * arg);


/***
  * 功能：
		创建快速保存窗体
  * 参数：
  		1、const char *pTopPath:	允许用户返回的顶层目录
  		2、CALLLBACKWINDOW func:	重绘前一个窗体的回调函数
  * 返回：
		无
  * 备注：
***/

int CreateQuickSaveWin(const char *pTopPath, char* pLastPath,
                    CALLLBACKWINDOW func, GUICHAR *pTitleStr, FILETYPE filetype);


#endif  //_WND_FRMQUICKSAVE_H
