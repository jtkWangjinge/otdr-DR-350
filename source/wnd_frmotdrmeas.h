/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrmeas.h
* 摘    要：  声明主窗体frmotdrmeas的窗体处理线程及相
				关操作函数
*
* 当前版本：  v1.0.1
* 作    者：  	
* 完成日期：  2014-12-29
*
* 取代版本：	v1.0.0
* 原 作 者：	
* 完成日期：  2014-8-19
**************************************************************/

#ifndef _WND_FRMOTDRMEAS_H
#define _WND_FRMOTDRMEAS_H


/**************************************************************
* 			为使用GUI而需要引用的头文件
**************************************************************/
#include "guiglobal.h"
#include "app_report.h"
#include "app_curve.h"


typedef struct _wnd_otdr_meas
{
	int 			iEnable;		//使能
	MAKER_MEAS 		eMeasCard;		//损耗卡
	MAKER_LINK 		enPitchOn; 		//光标链接方式
} WNDOTDRMEAS;


extern WNDOTDRMEAS *pOtdrMeasWnd;

/**************************************************************
* 				声明窗体处理相关函数
**************************************************************/
//窗体frmmain的初始化函数，建立窗体控件、注册消
//息处理
WNDOTDRMEAS * CreateOtdrMeasWnd(void);
int DestroyOtdrMeasWnd(WNDOTDRMEAS *pWnd);
//int DisplayOtdrMeasWnd(WNDOTDRMEAS *pMeasWnd);
int LoopOtdrMeasWnd(WNDOTDRMEAS *pWnd);
int SetOtdrMeasEnable(WNDOTDRMEAS *pMeasWnd,int iEnable,GUIWINDOW *pWndObj);
int RefreshOtdrMeasWnd(WNDOTDRMEAS *pWnd);

int TouchMarkLineLabelChange(int);

/**************************************************************
* 				声明全屏窗体处理相关函数
**************************************************************/
WNDOTDRMEAS * CreateFullMeasWnd(void);
int DestroyFullMeasWnd(WNDOTDRMEAS *pWnd);
int LoopFullMeasWnd(WNDOTDRMEAS *pWnd);
int SetFullMeasEnable(WNDOTDRMEAS *pMeasWnd,int iEnable,GUIWINDOW *pWndObj);
int RefreshFullMeasWnd(WNDOTDRMEAS *pWnd);
//设置当前标记线数字
int SetMarerCurrentNum(int iCurr);

#endif  //_WND_FRMOTDRMEAS_H

