/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcoor.h
* 摘    要：  实现otdr坐标系控件
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*
* 取代版本： 
* 原 作 者：  
* 完成日期：  
*******************************************************************************/


#ifndef _WND_FRMCOOR_H_
#define _WND_FRMCOOR_H_
#include "guiglobal.h"


//定义坐标系控件的宏定义
#define COOR_NOMAL		0		//普通
#define COOR_LARGE		1		//全屏
#define OPTION_NUM		4
#define CURVE_WAVE_NUM	2
#define FUNCTION_NUM	4

//定义otdr坐标系数据结构
typedef struct _otdr_coor 
{
	GUIPICTURE *pCveBg;

	GUIPICTURE *pBgIcon;
	GUILABEL *pLblTitle;
	GUICHAR *pStrTitle;

	GUIPICTURE *pBtnSet;
	GUIPICTURE *pBtnFile;
    GUIPICTURE *pBtnSwitchWave;
    GUIPICTURE *pBtnWave[CURVE_WAVE_NUM];
	GUILABEL *pLblWave[CURVE_WAVE_NUM];
	GUICHAR *pStrWave[CURVE_WAVE_NUM];
	
	GUIPICTURE *pPicFile;
	
	GUIPICTURE *pBtnOp[OPTION_NUM];
	MSGFUNC	pBtnFunc[FUNCTION_NUM + OPTION_NUM];

	GUIPICTURE *pCurve;
	MSGFUNC pCoorFuncDn;
	MSGFUNC pCoorFuncMv;
	MSGFUNC pCoorFuncUp;
	MSGFUNC pCoorFuncMT;

	int 	iFocus;
}OTDRCOOR;

OTDRCOOR *CreateOtdrCoor(MSGFUNC *ppFunc, MSGFUNC pCoorDn, MSGFUNC pCoorMv, 
						 MSGFUNC pCoorUp, MSGFUNC pCoorMT, int iFocus, 
						 GUIWINDOW *pWnd, int iType);
//设置坐标轴选中项
void SetCoorSelectedOption(OTDRCOOR *pCoorObj, int iSelectedOption, int iFlush);
//绘制坐标轴控件
int PaintCoor(OTDRCOOR *pCoorObj);
//销毁坐标轴控件
int DestroyCoor(OTDRCOOR **pCoorObj);

//添加坐标轴控件到接受消息队列 注册消息处理函数
int AddAndLoginCoor(OTDRCOOR *pCoorObj, GUIWINDOW *pWnd);

//从队列摘下坐标轴控件并注销消息处理函数
int DelAndLogoutCoor(OTDRCOOR *pCoorObj, GUIWINDOW *pWnd);

//设置当前选中的选项卡
void SetCoorFocus(OTDRCOOR* pCoorObj, int iFocus);

//设置波长选项卡的使能
void SetOptionWaveEnable(OTDRCOOR* pCoorObj, int* iEnable);
//设置初始选中的波长
void SetInitalWaveIndex(int iWaveIndex);
//重新设置曲线
void ResetCurve(OTDRCOOR* pCoorObj, int type);
//设置控件使能（主菜单+设置+文件）
void SetCoorEnabled(OTDRCOOR *pCoorObj, int iSelectedOption, int iEnabled);

#endif //_WND_FRMCOOR_H_
