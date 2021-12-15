/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrbottom.h
* 摘    要：  实现右侧菜单栏控件
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  2015-05-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMOTDRBOTTOM_H
#define _WND_FRMOTDRBOTTOM_H


/****************************
* 引用wnd_frmotdrbottom.h所需头文件
****************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmselector.h"
#include <math.h>


/*******************************************************************************
***                定义wnd_frmotdrbottom.h公有的宏定义                       ***
*******************************************************************************/
#define KNOB_LBL_MAX		20			//旋钮所能包含的最大项目数

typedef void (*OTDRBACKFUNC)(int Args);

/*******************************************************************************
***                定义wnd_frmotdrbottom.h公有数据类型                       ***
*******************************************************************************/
//三个小圆的结构体
typedef struct _wnd_knob {
	GUIPICTURE *pKnobBg;					//整体背景
	GUIPICTURE *pBtnItemPic[KNOB_LBL_MAX];//旋钮图片的对应控件
	GUILABEL *pCenter;					//小圆中心
	GUILABEL *pBtnItem[KNOB_LBL_MAX]; 	//旋钮周围的选项控件
	
	int 		iEnable[KNOB_LBL_MAX];  	//每个标签的使能状态
	int 		iItemCnt;					//标签的个数
	int 		iFocus;						//焦点的位置	
}WNDKNOB;

//波长的结构体
typedef struct _wnd_wavelength{
	GUIPICTURE  *pWaveLengthBg;				//波长整体背景
	GUILABEL    *pWaveLengthLbl;			//波长标签
	GUICHAR     *pWaveLengthStr;			//波长字符

	GUIPICTURE  *p1310nmBg;					//当前选择的1310
	GUIPICTURE  *p1550nmBg;					//当前选择的1550
	
	GUIPICTURE  *p1310nmPoint;				//1310nm点的选择
	GUIPICTURE  *p1550nmPoint;				//1550nm点的选择
	
	int 		iWave[2];					//
	int         iCurWave;					//当前选中波长0--1310，1--1550

	int 		iEnable;					//左边波长控件的使能
}WNDWAVELENGTH;

//otdrbottom整体底部栏
typedef struct _wnd_otdr_bottom{
	WNDWAVELENGTH WaveLength;
	WNDKNOB Knob[3];
	int	iKnobEnable;						//整个圆控件使能
	int iEnable;							//整体使能
	OTDRBACKFUNC BackCall;
}WNDOTDRBOTTOM;


/*******************************************************************************
***                    声明wnd_frmotdrbottom.h公有函数                       ***
*******************************************************************************/
//创建一个旋钮控件
WNDOTDRBOTTOM *CreateOtdrBottom(OTDRBACKFUNC BackCall);

//销毁旋钮控件
int DestroyOtdrBottom(WNDOTDRBOTTOM **pOtdrBottomObj);

//显示旋钮控件
int DisplayOtdrBottom(WNDOTDRBOTTOM *pOtdrBottomObj);

//添加旋钮控件到接收消息的控件队列
int AddOtdrBottomToComp(WNDOTDRBOTTOM *pOtdrBottomObj, GUIWINDOW *pWnd);

//删除控件从接受消息的控件队列中
int DelOtdrBottomFromComp(WNDOTDRBOTTOM *pOtdrBottomObj, GUIWINDOW *pWnd);


//设置波长使能
int SetWaveEnable(WNDOTDRBOTTOM *pOtdrBottomObj,int iEnable);

//设置波长选中
int SetWaveSelect(WNDOTDRBOTTOM *pOtdrBottomObj,int iSelect);

//设置整个otdrbottom控件的使能
int SetOtdrBottomEnable(WNDOTDRBOTTOM *pOtdrBottomObj, int iEnbale, GUIWINDOW *pWnd);

int SetKnobEnable(WNDOTDRBOTTOM *pOtdrBottomObj,int iEnable);
int DisplayAllKnob(WNDOTDRBOTTOM *pOtdrBottomObj);

//刷新时间控件
int RefreshKnobTime(WNDOTDRBOTTOM *pOtdrBottomObj, int elapsedTime);
#endif //_WND_FRMOTDRBOTTOM_H
