/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmOtdrSave.h
* 摘    要：  声明主窗体FrmOtdrSave的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者： 	     
* 完成日期：  2015-1-9
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMOTDRSAVE_H
#define _WND_FRMOTDRSAVE_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/

#include "guiglobal.h"
#include "app_curve.h"

/****************************
* 为使用GUI而需要引用的头文件
****************************/
	
typedef struct _save_curve
{
	int iSaveCurveNum;					//显示曲线的个数
	int iActiveCurve;					//当前活动曲线的序号
	int iActCurItemSelect;				//当前活动曲线Item选中条
	int iSaveCurveFlag[CURVE_MAX]; //每条曲线是否保存的标志
}SAVE_CURVE;

/*********************
* 声明窗体处理相关函数
*********************/
//窗体FrmOtdrSave的初始化函数，建立窗体控件、注册消息处理
int FrmOtdrSaveInit(void *pWndObj);
//窗体FrmOtdrSave的退出函数，释放所有资源
int FrmOtdrSaveExit(void *pWndObj);
//窗体FrmOtdrSave的绘制函数，绘制整个窗体
int FrmOtdrSavePaint(void *pWndObj);
//窗体FrmOtdrSave的循环函数，进行窗体循环
int FrmOtdrSaveLoop(void *pWndObj);
//窗体FrmOtdrSave的挂起函数，进行窗体挂起前预处理
int FrmOtdrSavePause(void *pWndObj);
//窗体FrmOtdrSave的恢复函数，进行窗体恢复前预处理
int FrmOtdrSaveResume(void *pWndObj);

extern  int OtdrSaveSorNameIncrease(void);
#endif  //_WND_FRMOTDRSAVE_H

