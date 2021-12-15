/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmauthorization.c
* 摘    要：  声明初始化窗体wnd_frmauthorization的窗体处理线程及相关操作函数
*
* 当前版本：
* 作    者：  wjg
* 完成日期：
*******************************************************************************/

#include "wnd_frmauthorization.h"

#include "app_getsetparameter.h"
#include "app_systemsettings.h"

#include "guiwindow.h"
#include "guipicture.h"
#include "guilabel.h"

#include "wnd_global.h"
#include "wnd_frmmenubak.h"

#include "wnd_frmconfigureinfo.h"
#include "wnd_frmlanguageconfig.h"
#include "wnd_frmlinearity.h"
#include "wnd_frmuncertainty.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmserialnum.h"
#include "wnd_frmmaintancedate.h"

//引用保存到eeprom的变量
extern PFactoryConfig pFactoryConf;
//当前的窗口
extern CURR_WINDOW_TYPE enCurWindow;

enum function
{
	FIP	= 0,		//端面检查
	SEQUENCE = 1,	//网线测序
	RANGE = 2,		//网线测距
	SEARCH = 3,		//网线寻线
	OPL = 4			//稳定光源
};

#define MAX_FUNCTION_NUM		5		//需要授权的功能数量（FIP,RJ45(SEQUENCE,RANGE,SEARCH),OPL）
static int authorizationFlag[MAX_FUNCTION_NUM] = { 0 };
//设置标志位
static int iFlagArray[MAX_FUNCTION_NUM] = { 0x01, 0x02, 0x04, 0x08, 0x10 };

/*******************************************************************************
**							声明窗体中相关控件变量							  **
*******************************************************************************/
static GUIWINDOW *pFrmFactoryAuthorization = NULL;
//authorization,serialnum
static GUIPICTURE *pFactoryAuthorizationBtnTitle[3] = { NULL };		
static GUILABEL *pFactoryAuthorizationLblTitle[3] = { NULL };
static GUICHAR *pFactoryAuthorizationStrTitle[3] = { NULL };
//定义桌面背景
static GUIPICTURE *pFactoryAuthorizationBg = NULL;
//声明标题标签变量
static GUICHAR* pFactoryAuthorizationStrCurTitle = NULL;
static GUILABEL* pFactoryAuthorizationLblCurTitle = NULL;
//功能区
static GUIPICTURE *pFactoryAuthorizationBtn[MAX_FUNCTION_NUM] = { NULL };
static GUICHAR* pFactoryAuthorizationStr[MAX_FUNCTION_NUM] = { NULL };
static GUILABEL* pFactoryAuthorizationLbl[MAX_FUNCTION_NUM] = { NULL };
/*******************************************************************************
*                   窗体frmauthorization右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFactoryAuthorizationMenu = NULL;
/*******************************************************************************
*                   窗体FrmAuthorization内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmAuthorizationTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//释放文本资源
static int FrmAuthorizationTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体FrmAuthorization内部按键响应函数声明
*******************************************************************************/
//点击标题栏按键响应函数
static int FactoryAuthorizationBtnTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryAuthorizationBtnTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击功能按键响应函数
static int FactoryAuthorizationBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryAuthorizationBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体FrmAuthorization回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void FactoryAuthorizationMenuCallBack(int iSelected);

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmauthorization的初始化函数，建立窗体控件、注册消息处理
int FrmAuthorizationInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象 
	pFrmFactoryAuthorization = (GUIWINDOW *)pWndObj;
	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
	FrmAuthorizationTextRes_Init(NULL, 0, NULL, 0);
	/***************************************************************************
	*                      创建桌面上各个区域的背景控件
	***************************************************************************/
	pFactoryAuthorizationBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factory1.bmp");

	//标题栏文本
	pFactoryAuthorizationLblCurTitle = CreateLabel(36, 12, 200, 16, pFactoryAuthorizationStrCurTitle);

	int i;
	for (i = 0; i < MAX_FUNCTION_NUM; ++i)
	{
		if (i < 3)//标题栏
		{
			pFactoryAuthorizationBtnTitle[i] = CreatePicture(10 + 91 * i, 48, 90, 35, pStrButton_unpress[i]);
			pFactoryAuthorizationLblTitle[i] = CreateLabel(10 + 91 * i, 48+10, 90, 16, pFactoryAuthorizationStrTitle[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryAuthorizationLblTitle[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryAuthorizationLblTitle[i]);
		}
		//功能区
		pFactoryAuthorizationBtn[i] = CreatePicture(200, 100+40*i, 25, 25, BmpFileDirectory"fiber_mask_unpress.bmp");
		pFactoryAuthorizationLbl[i] = CreateLabel(240, 100 + 40 * i+5, 150, 16, pFactoryAuthorizationStr[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryAuthorizationLbl[i]);
	}
	
	//菜单栏
	char* StrConfigMenu[FACTORY_MENU_COUNT] = {
		"DebugInfo",
		"LanguageSet",
		"Non-linearity",
		"Uncertainty",
		"FactorySet",
		"Authorization"/*,
		"SerialNum"*/
	};
	/***************************************************************************
	*                       创建右侧的菜单栏控件
	***************************************************************************/
	//无中间文件浏览，按键光标默认在菜单栏
	isInSmallFileBrowse = 0;
	pFactoryAuthorizationMenu = CreateStringWndMenu(FACTORY_MENU_COUNT, sizeof(StrConfigMenu), StrConfigMenu, 0xff00,
									5, 54, FactoryAuthorizationMenuCallBack);

	/***************************************************************************
	*           注册窗体(因为所有的按键事件都统一由窗体进行处理)
	***************************************************************************/
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactoryAuthorization, pFrmFactoryAuthorization);
	AddWndMenuToComp1(pFactoryAuthorizationMenu, pFrmFactoryAuthorization);

	for (i = 0; i < MAX_FUNCTION_NUM; ++i)
	{
		if (i < 3)//标题栏
		{
			AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryAuthorizationBtnTitle[i], pFrmFactoryAuthorization);
			AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryAuthorizationLblTitle[i], pFrmFactoryAuthorization);
		}
		//功能区
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryAuthorizationBtn[i], pFrmFactoryAuthorization);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryAuthorizationLbl[i], pFrmFactoryAuthorization);
	}

	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	for (i = 0; i < MAX_FUNCTION_NUM; ++i)
	{
		if (i < 3)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryAuthorizationBtnTitle[i], FactoryAuthorizationBtnTitle_Down, NULL, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryAuthorizationBtnTitle[i], FactoryAuthorizationBtnTitle_Up, NULL, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryAuthorizationLblTitle[i], FactoryAuthorizationBtnTitle_Down, NULL, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryAuthorizationLblTitle[i], FactoryAuthorizationBtnTitle_Up, NULL, i, pMsg);
		}
		//功能区
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryAuthorizationBtn[i], FactoryAuthorizationBtn_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryAuthorizationBtn[i], FactoryAuthorizationBtn_Up, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryAuthorizationLbl[i], FactoryAuthorizationBtn_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryAuthorizationLbl[i], FactoryAuthorizationBtn_Up, NULL, i, pMsg);
	}

	//注册窗体的按键消息处理

	LoginWndMenuToMsg1(pFactoryAuthorizationMenu, pFrmFactoryAuthorization);

	return iReturn;
}

//窗体frmauthorization的退出函数，释放所有资源
int FrmAuthorizationExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象
	pFrmFactoryAuthorization = (GUIWINDOW *)pWndObj;
	/***************************************************************************
	*                       清空消息队列中的消息注册项
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	/***************************************************************************
	*                       从当前窗体中注销窗体控件
	***************************************************************************/
	ClearWindowComp(pFrmFactoryAuthorization);
	/***************************************************************************
	*                      销毁桌面上各个区域的背景控件
	***************************************************************************/
	DestroyPicture(&pFactoryAuthorizationBg);
	//销毁文本
	DestroyLabel(&pFactoryAuthorizationLblCurTitle);
	//销毁标题栏
	int i;
	for (i = 0; i < MAX_FUNCTION_NUM; ++i)
	{
		if (i < 3)
		{
			DestroyPicture(&pFactoryAuthorizationBtnTitle[i]);
			DestroyLabel(&pFactoryAuthorizationLblTitle[i]);
		}

		DestroyPicture(&pFactoryAuthorizationBtn[i]);
		DestroyLabel(&pFactoryAuthorizationLbl[i]);
	}
	//销毁菜单栏
	DestroyWndMenu1(&pFactoryAuthorizationMenu);
	//释放文本资源
	FrmAuthorizationTextRes_Exit(NULL, 0, NULL, 0);
	//保存参数到eeprom中
	SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
	SaveSettings(FACTORY_CONFIG);

	return iReturn;
}

//窗体frmauthorization的绘制函数，绘制整个窗体
int FrmAuthorizationPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	DisplayPicture(pFactoryAuthorizationBg);
	DisplayLabel(pFactoryAuthorizationLblCurTitle);

	int i;
	for (i = 0; i < MAX_FUNCTION_NUM; ++i)
	{
		//标题栏
		if (i < 3)
		{
			if (i == 0)
			{
				SetPictureBitmap(pStrButton_press[i], pFactoryAuthorizationBtnTitle[i]);
			}

			DisplayPicture(pFactoryAuthorizationBtnTitle[i]);
			DisplayLabel(pFactoryAuthorizationLblTitle[i]);
		}
		//功能区
		if (pFactoryConf->enabled & iFlagArray[i])
		{
			SetPictureBitmap(BmpFileDirectory"fiber_mask_select.bmp", pFactoryAuthorizationBtn[i]);
			authorizationFlag[i] = 1;
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"fiber_mask_unpress.bmp", pFactoryAuthorizationBtn[i]);
			authorizationFlag[i] = 0;
		}

		DisplayPicture(pFactoryAuthorizationBtn[i]);
		DisplayLabel(pFactoryAuthorizationLbl[i]);
	}

	//显示菜单栏
	DisplayWndMenu1(pFactoryAuthorizationMenu);

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

//窗体frmauthorization的循环函数，进行窗体循环
int FrmAuthorizationLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

//窗体frmauthorization的挂起函数，进行窗体挂起前预处理
int FrmAuthorizationPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

//窗体frmauthorization的恢复函数，进行窗体恢复前预处理
int FrmAuthorizationResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/*******************************************************************************
*                   窗体FrmAuthorization内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmAuthorizationTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pFactoryAuthorizationStrCurTitle = TransString("Authorization");

	pFactoryAuthorizationStrTitle[0] = TransString("Authorization");
	pFactoryAuthorizationStrTitle[1] = TransString("SerialNum");
	pFactoryAuthorizationStrTitle[2] = TransString("LeaseDate");

	pFactoryAuthorizationStr[FIP] = TransString("FIP");
	pFactoryAuthorizationStr[SEQUENCE] = TransString("Cable Sequence");
	pFactoryAuthorizationStr[RANGE] = TransString("Cable Range");
	pFactoryAuthorizationStr[SEARCH] = TransString("Cable Search");
	pFactoryAuthorizationStr[OPL] = TransString("OPL");

	return iReturn;
}

//释放文本资源
static int FrmAuthorizationTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pFactoryAuthorizationStrCurTitle);

	GuiMemFree(pFactoryAuthorizationStrTitle[0]);
	GuiMemFree(pFactoryAuthorizationStrTitle[1]);
	GuiMemFree(pFactoryAuthorizationStrTitle[2]);

	GuiMemFree(pFactoryAuthorizationStr[FIP]);
	GuiMemFree(pFactoryAuthorizationStr[SEQUENCE]);
	GuiMemFree(pFactoryAuthorizationStr[RANGE]);
	GuiMemFree(pFactoryAuthorizationStr[SEARCH]);
	GuiMemFree(pFactoryAuthorizationStr[OPL]);

	return iReturn;
}

/*******************************************************************************
*                   窗体FrmAuthorization内部按键响应函数声明
*******************************************************************************/
//点击标题栏按键响应函数
static int FactoryAuthorizationBtnTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

static int FactoryAuthorizationBtnTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;

	switch (iOutLen)
	{
	case 1:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmSerialnumInit, FrmSerialnumExit,
			FrmSerialnumPaint, FrmSerialnumLoop,
			FrmSerialnumPause, FrmSerialnumResume,
			NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case 2:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmMaintanceDateInit, FrmMaintanceDateExit,
			FrmMaintanceDatePaint, FrmMaintanceDateLoop,
			FrmMaintanceDatePause, FrmMaintanceDateResume,
			NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
	default:
		break;
	}

	return iReturn;
}

//点击fip按键响应函数
static int FactoryAuthorizationBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

static int FactoryAuthorizationBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	if (authorizationFlag[iOutLen])
	{
		TouchChange("fiber_mask_unpress.bmp", pFactoryAuthorizationBtn[iOutLen],
			NULL, NULL, 0);
		authorizationFlag[iOutLen] = 0;
		pFactoryConf->enabled = pFactoryConf->enabled & (~iFlagArray[iOutLen]);
	}
	else
	{
		TouchChange("fiber_mask_select.bmp", pFactoryAuthorizationBtn[iOutLen],
			NULL, NULL, 0);
		authorizationFlag[iOutLen] = 1;
		pFactoryConf->enabled = pFactoryConf->enabled | iFlagArray[iOutLen];
	}

	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

/*******************************************************************************
*                   窗体FrmAuthorization回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void FactoryAuthorizationMenuCallBack(int iSelected)
{
	GUIWINDOW *pWnd = NULL;

	switch (iSelected)
	{
	case DEBUG_INFO:
		//设置当前界面
		enCurWindow = ENUM_DEBUG_INFO_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmConfigureInfoInit, FrmConfigureInfoExit,
			FrmConfigureInfoPaint, FrmConfigureInfoLoop,
			FrmConfigureInfoPause, FrmConfigureInfoResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case LANGUAGE_SET:
		//设置当前界面
		enCurWindow = ENUM_LANGUAGE_CONFIG_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmLanguageConfigInit, FrmLanguageConfigExit,
			FrmLanguageConfigPaint, FrmLanguageConfigLoop,
			FrmLanguageConfigPause, FrmLanguageConfigResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case NON_LINEARITY:
		enCurWindow = ENUM_NON_LINEARITY_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmLinearityInit, FrmLinearityExit,
			FrmLinearityPaint, FrmLinearityLoop,
			FrmLinearityPause, FrmLinearityResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case UNCERTAINTY:
		enCurWindow = ENUM_UNCERTAINTY_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmUncertaintyInit, FrmUncertaintyExit,
			FrmUncertaintyPaint, FrmUncertaintyLoop,
			FrmUncertaintyPause, FrmUncertaintyResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case FACTORY_SET:
		enCurWindow = ENUM_FACTORY_SET_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmFactorySetInit, FrmFactorySetExit,
			FrmFactorySetPaint, FrmFactorySetLoop,
			FrmFactorySetPause, FrmFactorySetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case AUTHORIZATION:
		enCurWindow = ENUM_AUTHORIZATION_WIN;
		break;
	/*case SERIALNUM:
		enCurWindow = ENUM_SERIALNUM_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmFactorySetInit, FrmFactorySetExit,
			FrmFactorySetPaint, FrmFactorySetLoop,
			FrmFactorySetPause, FrmFactorySetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);
		SendSysMsg_ThreadCreate(pWnd);
		break;*/
	case BACK_DOWN:
	case HOME_DOWN:
		enCurWindow = ENUM_OTHER_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmSystemSettingInit, FrmSystemSettingExit,
			FrmSystemSettingPaint, FrmSystemSettingLoop,
			FrmSystemSettingPause, FrmSystemSettingResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactoryAuthorization);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	default:
		break;
	}
}

//按键响应处理
void AuthorizationKeyBackCall(int iOption)
{
	static int iKeyFlag = 0;

	switch (iOption)
	{
	case 0://up
		if (iKeyFlag)
		{
			iKeyFlag--;
		}
		break;
	case 1://down
		if (iKeyFlag >= 0 && iKeyFlag < 4)
		{
			iKeyFlag++;
		}
		break;
	case 2://enter
		FactoryAuthorizationBtn_Up(NULL, 0, NULL, iKeyFlag);
		break;
	default:
		break;
	}
}

//创建授权管理界面
void CreateAuthorizationWindow()
{
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmAuthorizationInit, FrmAuthorizationExit,
		FrmAuthorizationPaint, FrmAuthorizationLoop,
		FrmAuthorizationPause, FrmAuthorizationResume,
		NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());		//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);
}