/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcurveresult.h
* 摘    要：  实现主窗体frmcurveresult的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/26
*
*******************************************************************************/

#ifndef _WND_FRM_CURVE_RESULT_H_
#define _WND_FRM_CURVE_RESULT_H_

#include "guipicture.h"
#include "guilabel.h"
#include "guibase.h"
#include "guiwindow.h"

#define CURVE_RESULT_TITLE_NUM		4

struct Curve_Result
{
	GUIPICTURE* pBg;
	GUILABEL* pLblFiberLength;
	GUILABEL* pLblFiberLengthValue;		//带单位
	GUILABEL* pLblTableTitle[CURVE_RESULT_TITLE_NUM];
	GUILABEL* pLblWaveInfo[2][CURVE_RESULT_TITLE_NUM];
	GUICHAR* pStrFiberLength;
	GUICHAR* pStrFiberLengthValue;
	GUICHAR* pStrTableTitle[CURVE_RESULT_TITLE_NUM];
	GUICHAR* pStrWaveInfo[2][CURVE_RESULT_TITLE_NUM];
	int iEnable;
};

/***
  * 功能：
		创建一个曲线结果控件
  * 参数：
		1.int iStartX:			控件的起始横坐标
		2.int iStartY:			控件的起始纵坐标
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
struct Curve_Result* CreateCurveResult(int iStartX, int iStartY);
/***
  * 功能：
		销毁一个曲线结果控件
  * 参数：
		struct Curve_Result** pCurveResult：指向曲线结果控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroyCurveResult(struct Curve_Result** pCurveResult);
/***
  * 功能：
		显示一个曲线结果控件
  * 参数：
		struct Curve_Result* pCurveResult：指向曲线结果控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplayCurveResult(struct Curve_Result* pCurveResult);
/***
  * 功能：
		注册一个曲线结果控件
  * 参数：
		1.struct Curve_Result* pCurveREsult：指向曲线结果控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int AddCurveResultToWnd(struct Curve_Result* pCurveResult, GUIWINDOW* pWnd);
/***
  * 功能：
		移除一个曲线结果控件
  * 参数：
		1.struct Curve_Result* pCurveResult：指向曲线结果控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DelCurveResultFromWnd(struct Curve_Result* pCurveResult, GUIWINDOW* pWnd);

/***
  * 功能：
		刷新曲线结果控件
  * 参数：
		1.struct Curve_Result* pCurveResult：指向曲线结果控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int FlushCurveResult(struct Curve_Result* pCurveResult);

//设置控件的使能状态
int SetCurveResultEnable(struct Curve_Result* pCurveResult, int iEnable, GUIWINDOW *pWnd);

#endif
