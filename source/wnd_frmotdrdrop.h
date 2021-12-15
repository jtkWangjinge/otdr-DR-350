/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrdrop.h
* 摘    要：  otdr伸展模式下的参数设置控件
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015-5-25
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_OTDRDROP_H
#define _WND_OTDRDROP_H

#include "guiglobal.h"

typedef void (*OTDRDROPBACKFUNC)(int Args);
/*******************************************************************************
***                  定义wnd_frmotdrdrop.h公有数据类型                       ***
*******************************************************************************/
typedef struct _wnd_otdr_drop{

	GUIPICTURE *pOtdrDropBg;
	GUIPICTURE *pOtdrDropWavePoint;
	GUIPICTURE *pOtdrDropWave;
	GUIPICTURE *pOtdrDropDist;
	GUIPICTURE *pOtdrDropPulse;
	GUIPICTURE *pOtdrDropTime;
	
	GUIPICTURE *pOtdrDropWavePointTouch;
	GUIPICTURE *pOtdrDropWaveTouch;
	GUIPICTURE *pOtdrDropDistTouch;
	GUIPICTURE *pOtdrDropPulseTouch;
	GUIPICTURE *pOtdrDropTimeTouch;

	GUIPICTURE *pOtdrDropAllDrop;
	
	GUICHAR *pOtdrDropStrWave;
	GUICHAR *pOtdrDropStrPulse;
	GUICHAR *pOtdrDropStrDist;
	GUICHAR *pOtdrDropStrTime;
	GUICHAR *pOtdrDropStrWaveVal;
	GUICHAR *pOtdrDropStrDistVal;
	GUICHAR *pOtdrDropStrPulseVal;
	GUICHAR *pOtdrDropStrTimeVal;
	
	GUILABEL *pOtdrDropLblWave;	
	GUILABEL *pOtdrDropLblPulse;
	GUILABEL *pOtdrDropLblDist;
	GUILABEL *pOtdrDropLblTime;
	GUILABEL *pOtdrDropLblWaveVal;
	GUILABEL *pOtdrDropLblDistVal;
	GUILABEL *pOtdrDropLblPulseVal;
	GUILABEL *pOtdrDropLblTimeVal;

	int iCurWave;
	int iWave[2];
	int iDist;
	int iPulse;
	int iTime;

	int iWaveEnable;
	int iDropEnable;
	int iEnable;

	OTDRDROPBACKFUNC BackCall;
	
}OTDRDROP;

/*******************************************************************************
***                    声明wnd_frmotdrdrop.h公有函数                       ***
*******************************************************************************/
//生成OtdrDrop控件
OTDRDROP * CreateOtdrDrop(OTDRDROPBACKFUNC BackCall);
//刷新控件到缓冲层
int DisplayOtdrDrop(void *pArg);
//销毁OtdrDrop控件
int DestroyOtdrDrop(OTDRDROP **pOtdrDrop);
//设置OtdrDrop的使能
int SetOtdrDropEnable(void *pArg, int iEnable, GUIWINDOW *pWnd);
/***
  * 功能：
     	使能otdrdrop控件，以便注册和注销事件
  * 参数：
  		1.void *pArg: 控件的地址
  		2.int iEnable: 1--注册，0--注销
  		3.GUIWINDOW *pWnd:当前窗体
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int SetOtdrDropEnable(void *pArg, int iEnable, GUIWINDOW *pWnd);

/***
  * 功能：
     	设置otdrdrop控件中波长的使能
  * 参数：
  		1.OTDRDROP *pOtdrDrop: 控件指针
  		2.int iEnable: 1--使能，0--取消
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int DropSetWaveEnable(OTDRDROP *pOtdrDrop, int iEnable);


/***
  * 功能：
     	设置当前波长的选择
  * 参数：
  		1.OTDRDROP *pOtdrDrop: 控件指针
  		2.int iCurWave: 1--1550nm，0--1310nm
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int DropSetWaveSelect(OTDRDROP *pOtdrDrop,int iCurWave);


int DisplayAllDrop(OTDRDROP *pOtdrDrop);
int SetAllDropEnable(OTDRDROP *pOtdrDrop ,int iEnable);

//根据OTDR已消耗的时间，刷新时间下拉框可用项
int RefreshDropTime(int elapsedTime);
#endif

