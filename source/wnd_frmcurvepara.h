/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcurvepara.h
* 摘    要：  实现主窗体FrmCurvePara的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/19
*
*******************************************************************************/

#ifndef _WND_FRM_CURVE_PARA_H_
#define _WND_FRM_CURVE_PARA_H_

#include "guipicture.h"
#include "guilabel.h"
#include "guibase.h"
#include "guiwindow.h"

#define CURVE_TITLE_NUM		4

typedef struct Otdr_Curve_Para
{
	GUIPICTURE* pBg;
	GUILABEL* pLblTitle[CURVE_TITLE_NUM];
	GUICHAR* pStrTitle[CURVE_TITLE_NUM];
	GUIPICTURE* pBtn[2][3];
	GUILABEL* pLblDataTitle[2][2];
	GUICHAR* pStrDataTitle[2][2];
	GUILABEL* pLblDataContent[2][3];
	GUICHAR* pStrDataContent[2][3];
	int iEnabled;
	int iFocus;
}OTDR_CURVE_PARA;

/***
  * 功能：
		创建一个otdr界面底部参数设置界面
  * 参数：
		无
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
OTDR_CURVE_PARA *CreateOtdrCurvePara(void);

/***
  * 功能：
		销毁OtdrCurvePara控件
  * 参数：
		1.OTDR_CURVE_PARA **pOtdrCurveParaObj : 指向OtdrCurvePara控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroyOtdrCurvePara(OTDR_CURVE_PARA **pOtdrCurveParaObj);

/***
  * 功能：
		显示OtdrCurvePara控件
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplayOtdrCurvePara(OTDR_CURVE_PARA *pOtdrCurveParaObj);

/***
  * 功能：
		设置OtdrCurvePara控件信息显示
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
		2.int iEnabled:0,不使能，1，使能（显示参数信息）
		3., GUIWINDOW *pWnd:注册到窗体上
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int SetOtdrCurveParaEnable(OTDR_CURVE_PARA *pOtdrCurveParaObj, int iEnabled, GUIWINDOW *pWnd);

/***
  * 功能：
		刷新OtdrCurvePara控件
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int RefreshOtdrCurvePara(OTDR_CURVE_PARA *pOtdrCurveParaObj);

/***
  * 功能：
		获取是否在当前窗体的标志位
  * 参数：
		无
  * 返回：
		0:不在当前窗体，1：在当前窗体，针对于按键的控制
  * 备注：
***/
int GetCurrentWindow(void);

/***
  * 功能：
		按键控件响应回调函数
  * 参数：
		int iSelected: 按键值（keyboard_up等）
  * 返回：
		无
  * 备注：
***/
void CurveParaKeyBoardCallBack(int iSelected);

#endif
