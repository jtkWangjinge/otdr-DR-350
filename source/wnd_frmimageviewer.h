/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmimageviewer.h
* 摘    要：  提供图片浏览的功能
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*
*******************************************************************************/

#ifndef _WND_FRMIMAGEVIEWER_H_
#define _WND_FRMIMAGEVIEWER_H_


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"

#include "app_frmfilebrowse.h"

typedef void (*ImageViewerBackFunc)(GUIWINDOW **);

int ImageViewer(const char* imgFile, GUIWINDOW* currWnd, ImageViewerBackFunc createCaller);

void SetImageViewerSort(FILESORT enSort);


/*********************
* 声明窗体处理相关函数
*********************/
//窗体ImageViewer的初始化函数，建立窗体控件、注册消息处理
int FrmImageViewerInit(void *pWndObj);
//窗体ImageViewer的退出函数，释放所有资源
int FrmImageViewerExit(void *pWndObj);
//窗体ImageViewer的绘制函数，绘制整个窗体
int FrmImageViewerPaint(void *pWndObj);
//窗体ImageViewer的循环函数，进行窗体循环
int FrmImageViewerLoop(void *pWndObj);
//窗体ImageViewer的挂起函数，进行窗体挂起前预处理
int FrmImageViewerPause(void *pWndObj);
//窗体ImageViewer的恢复函数，进行窗体恢复前预处理
int FrmImageViewerResume(void *pWndObj);

#endif