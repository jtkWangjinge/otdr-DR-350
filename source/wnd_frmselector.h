/*******************************************************************************
* Copyright(c)2014，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmselector.h 
* 摘    要：  实现选择器控件相关头文件
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  2014-10-23
*
* 取代版本：
* 原 作 者：
* 完成日期：
******************************************************************************/

#ifndef _WND_FRMSELECTOR_H
#define _WND_FRMSELECTOR_H

#include "wnd_global.h"
#include "app_global.h"

//定义回调函数类型
typedef void (*SECBACKFUNC)(int selected);
typedef void (*SECBACKFUNC1)(int selected,int selectID);
#define MAX_ITEM_NUM 	100	//最大元素数量

//选择器所使用的图片
#define SEC_UP_UNPRESS 	BmpFileDirectory"sysset_btn_up_unpress.bmp"
#define SEC_UP_PRESS	BmpFileDirectory"sysset_btn_up_press.bmp"
#define SEC_DN_UNPRESS	BmpFileDirectory"sysset_btn_down_unpress.bmp"
#define SEC_DN_PRESS	BmpFileDirectory"sysset_btn_down_press.bmp"
#define SEC_MD_UNPRESS	BmpFileDirectory"sysset_btn_time_unpress.bmp"
#define SEC_MD_PRESS	BmpFileDirectory"sysset_btn_time_press.bmp"


//定义公有选择器数据结构
typedef struct _selector 
{
	UINT16		uiPosiX;			//放置的位置x坐标
	UINT16		uiPosiY;			//放置的位置y坐标
	UINT16		uiItemCnt;			//元素的个数
	UINT16		uiCurrSelected;		//当前选中的元素
		
	GUICHAR		*pItemString[MAX_ITEM_NUM];		//每个元素上要显示的字符
	
	GUIPICTURE	*pSelectorBtnLeft;	//左按键控件
	GUIPICTURE	*pSelectorBtnRight;	//右按键控件
	GUIPICTURE	*pSelectorBtnMidd;	//中间控件
	GUILABEL	*pSelectorLblItem;	//标签

	SECBACKFUNC BackCallFunc;		//回调函数
	SECBACKFUNC1 BackCallFunc1;		//回调函数1

	UINT8		uiItemTouchFlg;		//元素点击标志
	UINT8		uiEnable;			//使能标志
}SELECTOR;

//创建一个选择器 
SELECTOR *CreateSelector(UINT16 uiPosiX, UINT16 uiPosiY, UINT16 uiItemCnt,
						 UINT16 uiCurrSelected, GUICHAR **pItemStr, 
						 SECBACKFUNC BackCall, UINT8 uiItemTouchFlg);

SELECTOR *CreateSelector1(UINT16 uiPosiX, UINT16 uiPosiY, UINT16 uiItemCnt,
						 UINT16 uiCurrSelected, GUICHAR **pItemStr, 
						 SECBACKFUNC1 BackCall,UINT8 uiItemTouchFlg);

//销毁一个选择器 
int DestroySelector(SELECTOR **pSampleTimeSelector);
//绘制一个选择器 
int DisplaySelector(SELECTOR *pSampleTimeSelector);
//加入选择器到控件队列使其能够接收消息 
int AddSelectorToComp(SELECTOR *pSampleTimeSelector, GUIWINDOW *pWnd);
//注册选择器事件到消息队列 
int LoginSelectorToMsg(SELECTOR *pSelector, GUIMESSAGE *pMsg);
int LoginSelectorToMsg1(SELECTOR *pSelector, int selectID, GUIMESSAGE *pMsg);
//设置控件是否使能，也就是是否接受点击事件 
int SetSelectorEnable(UINT8 uiEnable, SELECTOR *pSelector);
//设置控件选择成第几个
int SetSelectorSelected(UINT32 uiSelect, SELECTOR *pSelector);
//处理选择器递增或递减
void HandleSelectorAddOrDec(SELECTOR *pSelector, int isAddOrDec);

#endif //_WND_FRMSELECTOR_H
