/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmenubak.h
* 摘    要：  声明右侧菜单栏处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/25
*
*******************************************************************************/

#ifndef _WND_FRMMENUBAK_H
#define _WND_FRMMENUBAK_H


/**************************** 
* 引用wnd_frmmenu.h所需头文件
****************************/
#include "app_global.h"

#include "wnd_global.h"
#include "wnd_frmselector.h"
#include "wnd_frmfilebrowse1.h"

/******************************
* 定义wnd_frmmenu.h公有的宏定义
******************************/
#define HOME_DOWN 		25		//按键编码，代表退出键按下
#define BACK_DOWN		26		//按键编码，代表help按下

#define MENU_MAX		6		//菜单所能包含的最大项目数


#define MENU_BACK		0xff00u
#define MENU_HOME		0x00ffu
#define MENU_ALL        MENU_BACK | MENU_HOME
#define MENU_NONE       (UINT16)(~(MENU_BACK | MENU_HOME))

#define MENU_ITEM0		0
#define MENU_ITEM1		1
#define MENU_ITEM2		2
#define MENU_ITEM3		3
#define MENU_ITEM4		4
#define MENU_ITEM5		5
#define MENU_ITEM6 		6

#define MENU_UNPRESS    0
#define MENU_SELECT     1
#define MENU_DISABLE    2

//获得菜单控件的焦点	
//#define GetWndMenuFocus(pMenuObj) (pMenuObj->iFocus)
		

/******************************
* 定义wnd_frmmenu.h公有数据类型
******************************/
typedef struct _wnd_menu1 {
	GUIPICTURE *pMenuBg;			//背景
	GUIPICTURE *pScreenShot;        //截图按钮
	GUIPICTURE *pDirection;			//方向按钮
	GUIPICTURE *pBtnItem[MENU_MAX];	//菜单按钮
	char	   *pBtnItemSelectBg[MENU_MAX];	//菜单按钮选中背景
	char	   *pBtnItemUnpressBg[MENU_MAX];	//菜单按钮非选中背景
	char	   *pBtnItemDisableBg[MENU_MAX];	//菜单按钮不使能背景
	GUILABEL   *pLblItem[MENU_MAX];	//按钮上的标签
	GUICHAR	   *pStrItem[MENU_MAX];	//标签文字
	int	   		itemEnable[MENU_MAX];//项使能
	GUIPICTURE *pBtnHome;			//Home按键
	GUIPICTURE *pBtnBack;			//Help按钮
	
	int 		iItemCnt;			//按钮个数
	int 		iFocus;				//焦点的位置 NO_FOCUS:表示所有按钮都没有焦点
	int         iDirectionEnable;   //是否显示方向键

	UINT16		iBackHome;			//按键home和back是否有作用；
									//		高8位为0x00为help不可用，其他为可用
									//		低8位为0x00为home不可用，其他为可用
		
	SECBACKFUNC CallBack;			//点击按键的回调
	
}WNDMENU1;


/**************************
* 声明wnd_frmmenu.h公有变量
**************************/


/*****************************
*    声明wnd_frmmenu.h公有函数
*****************************/
//临时函数（针对于菜单中未实现的多国语翻译的文本）
WNDMENU1 *CreateStringWndMenu(int iNum, unsigned int size, char **uiLblIdx, 
                        UINT16 iBackHome, int iFocus, unsigned int iTopMargin,
                        SECBACKFUNC CallBack);

//创建一个菜单控件
WNDMENU1 *CreateWndMenu1(int iNum, unsigned int size, unsigned int *uiLblIdx, 
						UINT16 iBackHome, int iFocus, int iDirectionEnable, 
						unsigned int iTopMargin, SECBACKFUNC CallBack);
//销毁菜单控件
int DestroyWndMenu1(WNDMENU1 **pMenuObj);

//显示菜单控件
int DisplayWndMenu1(WNDMENU1 *pMenuObj);

//添加菜单控件到接收消息的控件队列
int AddWndMenuToComp1(WNDMENU1 *pMenuObj, GUIWINDOW *pWnd);

//注册菜单控件的消息处理函数
int LoginWndMenuToMsg1(WNDMENU1 *pMenuObj, GUIWINDOW *pWnd);

//设置控件使能状态
int SetWndMenuEnable1(int iEnable, WNDMENU1 *pMenuObj);

//设置控件图层
int SetWndMenuLayer1(int iLayer, WNDMENU1 *pMenuObj);

//刷新menu
void RefreshMenu(WNDMENU1 *pMenuObj, int iFocus, int iNum, 
				UINT16 iBackHome, unsigned int size,unsigned int *uiLblIdx);

//刷新menu
void RefreshMenu1(WNDMENU1 *pMenuObj, int iFocus, int iNum, 
				UINT16 iBackHome, unsigned int size,unsigned int *uiLblIdx, int iEnabled);
//刷新菜单栏背景
int RefreshMenuBackground(WNDMENU1 *pMenuObj);
//获得当前的menu
WNDMENU1 *getCurMenu(void);

//设置当前的menu
void setCurMenu(WNDMENU1 *pOpt);

int getCurItemPush(void);

void setCurItemPush(int iOpt);

//获取是否选中菜单栏
int getSelectedMenu(void);

//设置控件某一项Label颜色
int SetWndMenuItemFont(int item, GUIFONT *pLblFont, WNDMENU1 *pMenuObj);
//设置控件某一项使能状态
int SetWndMenuItemEnble(int item, int enable, WNDMENU1 *pMenuObj);
//设置控件某一项背景
int SetWndMenuItemBg(int item, char *backGroundBmp, WNDMENU1 *pMenuObj,int state);
//设置每一项的文本的偏移
void SetItemWordOffset(int offset);
//得到每一项的文本的偏移
int GetItemWordOffset(void);

#endif //_WND_FRMMENUBAK_H
