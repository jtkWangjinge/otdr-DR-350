/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcablesearch.c
* 摘    要：  主窗体wnd_frmcablesearch的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：
*******************************************************************************/

#include "wnd_frmcablesearch.h"

#include "app_rj45.h"

#include "guiwindow.h"

#include "wnd_global.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmrj45.h"

/*******************************************************************************
*					窗体frmcablesearch中的窗体控件
*******************************************************************************/
static GUIWINDOW* pFrmCableSearch = NULL;

/*******************************************************************************
*                   窗体frmcablesearch桌面上的标题显示控件
*******************************************************************************/
//标题栏信息
static GUIPICTURE *pCableSearchBg = NULL;
static GUIPICTURE *pCableSearchBgIcon = NULL;

static GUICHAR *pCableSearchStrCurTitle = NULL;
static GUILABEL *pCableSearchLblCurTitle = NULL;
//功能区提示信息
static GUIPICTURE *pCableSearchBgStatus = NULL;	//状态
static GUIPICTURE *pCableSearchBgPrompt = NULL;	//提示文本
static GUICHAR *pCableSearchStrPrompt = NULL;
static GUILABEL *pCableSearchLblPrompt = NULL;

/*******************************************************************************
*                   窗体frmcablesearch右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pCableSearchMenu = NULL;
//各项非选中背景资源
static char *pCableSearchUnpressBmp[2] =
{
    BmpFileDirectory "bg_cablesearch_start.bmp",
    BmpFileDirectory "bg_cablesearch_close.bmp"
};

//各项选中背景资源
static char *pCableSearchSelectBmp[2] =
{
    BmpFileDirectory "bg_cablesearch_startSelect.bmp",
    BmpFileDirectory "bg_cablesearch_closeSelect.bmp"
};
//当前的窗口
extern CURR_WINDOW_TYPE enCurWindow;                
//打开寻线功能的标志位(默认是关闭状态)
static int iOpenFlag = 0;
/*******************************************************************************
*                   窗体frmcablesearch内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmCableSearchTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//释放文本资源
static int FrmCableSearchTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体frmcablesearch回调函数声明
*******************************************************************************/
//设置寻线
static void SearchCable(void);
//菜单栏回调函数
static void CableSearchMenuCallBack(int iSelected);

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmcablesearch的初始化函数，建立窗体控件、注册消息处理
int FrmCableSearchInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象 
	pFrmCableSearch = (GUIWINDOW *)pWndObj;
	//初始化文本资源
	FrmCableSearchTextRes_Init(NULL, 0, NULL, 0);
	//初始化参数
	InitialCableSearchParameter();
	/***************************************************************************
	*                      创建桌面上各个区域的背景控件
	***************************************************************************/
	pCableSearchBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_cablesearch.bmp");
	pCableSearchBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_cablesearch_icon.bmp");
	//标题栏文本
	pCableSearchLblCurTitle = CreateLabel(40, 12, 100, 16, pCableSearchStrCurTitle);
	//功能区
	pCableSearchBgStatus = CreatePicture(155, 105, 211, 174, BmpFileDirectory"bg_signal_0.bmp");
	pCableSearchBgPrompt = CreatePicture(215, 346, 101, 16, BmpFileDirectory"bg_prompt.bmp");
	pCableSearchLblPrompt = CreateLabel(215, 346, 100, 16, pCableSearchStrPrompt);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCableSearchLblPrompt);
	//菜单栏
	unsigned int StrConfigMenu[2] = {
		FIP_LBL_OPEN,
		VFL_LBL_CLOSE
	};
	/***************************************************************************
	*                       创建右侧的菜单栏控件
	***************************************************************************/
	pCableSearchMenu = CreateWndMenu1(2, sizeof(StrConfigMenu), StrConfigMenu, 0xff00,
		0, 1, 41, CableSearchMenuCallBack);
	//设置菜单栏背景
	int i;
	for (i = 0; i < 2; i++)
	{
		SetWndMenuItemBg(i, pCableSearchUnpressBmp[i], pCableSearchMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pCableSearchSelectBmp[i], pCableSearchMenu, MENU_SELECT);
	}
	/***************************************************************************
	*           注册窗体(因为所有的按键事件都统一由窗体进行处理)
	***************************************************************************/
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmCableSearch, pFrmCableSearch);
	AddWndMenuToComp1(pCableSearchMenu, pFrmCableSearch);
	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	LoginWndMenuToMsg1(pCableSearchMenu, pFrmCableSearch);

	return iReturn;
}

//窗体frmcablesearch的退出函数，释放所有资源
int FrmCableSearchExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象
	pFrmCableSearch = (GUIWINDOW *)pWndObj;

	/***************************************************************************
	*                       清空消息队列中的消息注册项
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	/***************************************************************************
	*                       从当前窗体中注销窗体控件
	***************************************************************************/
	ClearWindowComp(pFrmCableSearch);

	//销毁控件
	DestroyPicture(&pCableSearchBg);
	DestroyPicture(&pCableSearchBgIcon);
	DestroyLabel(&pCableSearchLblCurTitle);

	DestroyPicture(&pCableSearchBgStatus);
	DestroyPicture(&pCableSearchBgPrompt);
	DestroyLabel(&pCableSearchLblPrompt);

	//销毁菜单栏
	DestroyWndMenu1(&pCableSearchMenu);

	//销毁文本资源
	FrmCableSearchTextRes_Exit(NULL, 0, NULL, 0);

	//清除参数数据
	iOpenFlag = 0;	//清除打开标志位
	ClearCableSearchParameter();

	return iReturn;
}

//窗体frmcablesearch的绘制函数，绘制整个窗体
int FrmCableSearchPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//显示控件
	DisplayPicture(pCableSearchBg);
	// DisplayPicture(pCableSearchBgIcon);
	DisplayLabel(pCableSearchLblCurTitle);

	DisplayPicture(pCableSearchBgStatus);
	DisplayPicture(pCableSearchBgPrompt);
	DisplayLabel(pCableSearchLblPrompt);

	////设置当前窗体
	enCurWindow = ENUM_CABLE_SEARCH_WIN;	
	//显示菜单栏
	DisplayWndMenu1(pCableSearchMenu);
	//刷新菜单栏
	int i;
	for (i = 0; i < 2; ++i)
	{
		SetWndMenuItemEnble(i, 1, pCableSearchMenu);
	}

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

//窗体frmcablesearch的循环函数，进行窗体循环
int FrmCableSearchLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//记录进入该函数的次数
	static int count = 0;

	if (iOpenFlag)//打开寻线的标志位
	{
		if (count < 5)
		{
			TouchChange("bg_signal_1.bmp", pCableSearchBgStatus, NULL, NULL, 0);
			RefreshScreen(__FILE__, __func__, __LINE__);
			count++;
		}
		else if (count < 10)
		{
			TouchChange("bg_signal_2.bmp", pCableSearchBgStatus, NULL, NULL, 0);
			RefreshScreen(__FILE__, __func__, __LINE__);
			count++;
		}
		else
		{
			count = 0;
		}

		//开始测试
		TestCableSearch();
	}
	else
	{
		count = 0;
	}

	//DisplayPicture(pCableSearchBgStatus);
	//RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

//窗体frmcablesearch的挂起函数，进行窗体挂起前预处理
int FrmCableSearchPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

//窗体frmcablesearch的恢复函数，进行窗体恢复前预处理
int FrmCableSearchResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/*******************************************************************************
*                   窗体frmcablesearch内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmCableSearchTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pCableSearchStrCurTitle = GetCurrLanguageText(RJ45_LBL_SEARCH);
	pCableSearchStrPrompt = GetCurrLanguageText(CABLE_SEARCH_LBL_COMMON);

	return iReturn;
}

//释放文本资源
static int FrmCableSearchTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pCableSearchStrCurTitle);
	GuiMemFree(pCableSearchStrPrompt);

	return iReturn;
}

/*******************************************************************************
*                   窗体frmcablesearch回调函数声明
*******************************************************************************/
//设置寻线
static void SearchCable(void)
{
	//iOpenFlag = iOpenFlag ? 0 : 1;

	if (iOpenFlag)
	{
		pCableSearchStrPrompt = GetCurrLanguageText(CABLE_SEARCH_LBL_ING);
		//todo
		TouchChange("bg_prompt.bmp", pCableSearchBgPrompt, pCableSearchStrPrompt,
			pCableSearchLblPrompt, 1);
	}
	else
	{
		pCableSearchStrPrompt = GetCurrLanguageText(CABLE_SEARCH_LBL_COMMON);
		//todo
		TouchChange("bg_signal_0.bmp", pCableSearchBgStatus, NULL, NULL, 0);
		TouchChange("bg_prompt.bmp", pCableSearchBgPrompt, pCableSearchStrPrompt,
			pCableSearchLblPrompt, 1);
	}
	RefreshScreen(__FILE__, __func__, __LINE__);

}

//菜单栏回调函数
static void CableSearchMenuCallBack(int iSelected)
{
	//窗体
	GUIWINDOW *pWnd = NULL;

	switch (iSelected)
	{
	case 0://开始/打开
		iOpenFlag = 1;
		SearchCable();
		break;
	case 1://关闭
		iOpenFlag = 0;
		SearchCable();
		break;
	case BACK_DOWN:
	case HOME_DOWN:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmRJ45Init, FrmRJ45Exit,
							FrmRJ45Paint, FrmRJ45Loop,
							FrmRJ45Pause, FrmRJ45Resume,
							NULL); //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmCableSearch); //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);	 //发送消息以便调用新的窗体
		break;
	default:
		break;
	}

}