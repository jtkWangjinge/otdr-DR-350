/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmeventtab.h
* 摘    要：  声明主窗体frmeventtab的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/25
*
*******************************************************************************/

#ifndef _WND_FRMEVENTTAB_H
#define _WND_FRMEVENTTAB_H

#include "app_eventsearch.h"
#include "guiwindow.h"
#include "wnd_frmevtbtn.h"
#include "app_curve.h"


/******************************
* 定义frmeventtab控件所需宏定义
******************************/
#define TAB_ROWS 			5 	//事件表行数最大值
//#define MAX_EVT_NUM			500	//事件表最大支持的事件数量

/******************************
* 声明frmeventtab控件的数据结构
******************************/
typedef struct evt_tab_info		//GUI显示所需要的数据结构
{
	int iType;
	char pNum[4];
	char pDist[12];
	char pLoss[12];
	char pRef[12];
	char pDec[12];
	char pSum[12];
    float fDist;            //距离
	int iIsInVaild;			//该事件是否在径距内
	int iIsLossPass;        //该事件损耗是否通过
	int iIsReflectPass;     //该事件反射率是否通过
	int iIsAttenPass;       //该事件衰减是否通过
	int iIsGray;            //该条目是否灰色显示
} EVTTABINFO;

typedef struct gui_evt_tab	//GUI事件表结构
{
	int 	iColNum;			//事件表的行数
	int 	iEnable;			//控件的使能状态
	int		iTouchEnable;		//事件表点击使能
	int 	iEvtTotallEvt;		//事件总数
	int 	iEvtDispIdx;		//当前选中的事件的在事件总数中的索引
	int 	iEvtFocus;			//当前选中的事件在gui事件表中的位置
	int 	iEvtTabPage;		//当前焦点所在的页
	int		iEvtPageTotall;		//事件表总页数
	int 	iCsrOnEvt;			//光标移动方式
	int 	iSetOrDelRef;		//是否设置参考起始点
	int 	iNeedGetEvtContent;	//记录前一次的页数
	int	    iDisplaySectionEvt; //区段事件
	EVTTABINFO sEvtTab[MAX_EVENTS_NUMBER];//事件表的内容
}EVENTTAB;


/********************************
* 声明frmeventtab控件基本操作函数
********************************/
//创建事件表控件
EVENTTAB * CreateEventTab(int iColNum, GUIWINDOW *pWnd);
//销毁事件表控件
void DestroyEventTab(EVENTTAB **ppEvtTab);
//绘制事件表
int DisplayEvtTab(EVENTTAB *pEvents);
//重设事件表
int ResetTabAttr(EVENTTAB *pEvtTab, int iColNum);
//清空事件表
int ClrEvtTab(EVENTTAB *pEvents);
//刷新事件表
int FlushEvtTab(EVENTTAB *pEvents);
//获取事件表
EVENTS_TABLE *GetEvtTabEvents(void);
//设置控件的使能状态
int SetEvtTabEnable(EVENTTAB *pEvtTab, int iEnable, GUIWINDOW *pWnd);
//
int SetEvtTouchEnable(EVENTTAB *pEvtTab, int iEnable);
//增加一个事件
int AddEventsInTable(int iNewLocation, EVENTTAB *pEvtTab);
//删除一个事件
int DelEventsInTable(EVENTTAB *pEvents);
//设置参考起始点
int SetEventsRef(int iNewLocation, EVENTTAB *pEvtTab);
//清除参考起始点
int ClearEventsRef(EVENTTAB *pEvents);
//设置光标在事件上
int SetCursorAtEvent(EVENTTAB *pEvents, int isTouchMove, int iMaxFlg);
//判断点击是否在事件上
int TouchIsOnEvent(int iTouchX, EVENTTAB *pEvtTab);
//设置事件点光标位置
void SetEventFocus(EVENTTAB *pEvtTab, int option);
//切换链路地图与事件表
void SwitchLinkMapOrEventTab(EVENTTAB *pEvtTab);
//DEBUG打印
//#define EVT_DBUG
#ifdef EVT_DBUG
void evt_dbg(EVENTTAB *pEvtsTab);
#else
#define evt_dbg(pEvtsTab)
#endif

#endif //_WND_FRMEVENTTAB_H
