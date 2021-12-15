/**************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmlanguageset.h
* 摘    要：  声明窗体wnd_frmlanguageset的窗体处理线程及相
				关操作函数
*
* 当前版本：  v1.0.1
* 作    者： 		 
* 完成日期：  2014-12-29
*
* 取代版本：	v1.0.0
* 原 作 者：	
* 完成日期：2014-10-11
**************************************************************/

#ifndef _WND_FRMLANGUAGESET_H
#define _WND_FRMLANGUAGESET_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"
#include "wnd_global.h"

/*********************
* 声明窗体处理相关函数
*********************/
//窗体frmlanguageset的初始化函数，建立窗体控件、注册
//消息处理
int FrmLanguageSetInit(void *pWndObj);
//窗体frmlanguageset的退出函数，释放所有资源
int FrmLanguageSetExit(void *pWndObj);
//窗体frmlanguageset的绘制函数，绘制整个窗体
int FrmLanguageSetPaint(void *pWndObj);
//窗体frmlanguageset的循环函数，进行窗体循环
int FrmLanguageSetLoop(void *pWndObj);
//窗体frmlanguageset的挂起函数，进行窗体挂起前预处理
int FrmLanguageSetPause(void *pWndObj);
//窗体frmlanguageset的恢复函数，进行窗体恢复前预处理
int FrmLanguageSetResume(void *pWndObj);

#endif  //_WND_FRMLANGUAGESET_H
