/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmevtbtn.h
* 摘    要：  实现事件表附加功能按钮控件
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  2015-01-05
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMEVTBTN_H
#define _WND_FRMEVTBTN_H


/****************************** 
* 引用wnd_frmevtbtn.h所需头文件
******************************/
#include "wnd_global.h"
#include "app_global.h"



/******************************** 
* 定义wnd_frmevtbtn.h公有的宏定义
********************************/
#define 	EVTBTN_MAX		3	
#define 	EVTBTN_PAGE		2

#define		BTN_ADDEVT		0
#define 	BTN_DELEVT		1
#define		BTN_MAXEVT		2
#define 	BTN_SETREF		3
#define 	BTN_DELREF		4

typedef int (*BTNFUNC)(void *);

/******************************
* 定义wnd_frmmenu.h公有数据类型
******************************/
typedef struct _wnd_evtbtn {
	GUIPICTURE *pBg;
	GUIPICTURE *pBtnUp;
	GUIPICTURE *pBtnDn;
    #if 0
	GUIPICTURE *pBtnItem[EVTBTN_MAX];				//菜单按钮
	GUILABEL   *pLblItem[EVTBTN_MAX];				//按钮上的标签

	GUICHAR	   *pStrItem[EVTBTN_PAGE * EVTBTN_MAX];	//标签文字

	int 		iPageNum;							//页数
	int 		iEachPage;							//每页的个数
	int			iCurrPage;							//当前的页数
	int 		iFocus;								//当前点击的位置

	int 		iEnable;							//使能标志位
	int 		iTouchEnable;						//点击使能
	
	int 		iItemEn[EVTBTN_PAGE * EVTBTN_MAX];	//元素的使能标志
	int 		iItemFcs[EVTBTN_PAGE * EVTBTN_MAX];	//元素常亮标志
	void 		*pCallerArg;						//响应函数参数
	BTNFUNC		pItemFunc[EVTBTN_PAGE * EVTBTN_MAX];//消息响应函数
    #endif
    GUIPICTURE *pBtnItem[3];				//菜单按钮
	GUILABEL   *pLblItem[3];				//按钮上的标签

	GUICHAR	   *pStrItem[3];	//标签文字

	int 		iPageNum;							//页数
	int 		iEachPage;							//每页的个数
	int			iCurrPage;							//当前的页数
	int 		iFocus;								//当前点击的位置

	int 		iEnable;							//使能标志位
	int 		iTouchEnable;						//点击使能
	
	int 		iItemEn[3];	//元素的使能标志
	int 		iItemFcs[3];	//元素常亮标志
	void 		*pCallerArg;						//响应函数参数
	BTNFUNC		pItemFunc[3];//消息响应函数
	
}WNDEVTBTN;

//创建事件按钮控件
WNDEVTBTN *CreateEvtBtn(int x, int y, int iPage, int iCnt, BTNFUNC *ppFunc,
						void *pFuncArg);
//销毁控件
int DestroyEvtBtn(WNDEVTBTN **pBtnObj);
//绘制事件按钮控件
int PanitEvtBtn(WNDEVTBTN*pBtnObj);
//添加控件队列
int AddEvtBtnToCom(WNDEVTBTN*pBtnObj, GUIWINDOW *pWnd);
//从控件队列摘下
int DelEvtBtnToCom(WNDEVTBTN*pBtnObj, GUIWINDOW *pWnd);
//添加消息队列
int LoginEvtBtnToMsg(WNDEVTBTN*pBtnObj, GUIMESSAGE *pMsg);
//添加消息队列
int LogoutEvtBtnToMsg(WNDEVTBTN*pBtnObj, GUIMESSAGE *pMsg);
//设置控件的使能状态
int SetEvtBtnEnable(WNDEVTBTN *pBtnObj, int iEnable, GUIWINDOW *pWnd);
//获得元素使能
int *GetBtnItemEnable(WNDEVTBTN *pBtnObj);
//设置元素焦点
int SetEvtBtnFocus(WNDEVTBTN *pBtnObj, int iIndex, int iFocus);

//设置元素使能与否
int SetBtnItemEnable(WNDEVTBTN *pBtnObj, int *pEnable);

//
int SetBtnTouchEnable(WNDEVTBTN *pBtnObj, int iEnable);

#endif //_WND_FRMEVTBTN_H
