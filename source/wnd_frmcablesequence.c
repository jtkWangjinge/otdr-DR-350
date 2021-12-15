/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmCableSequence.c
* 摘    要：  实现主窗体wnd_frmCableSequence的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmcablesequence.h"

#include "app_unitconverter.h"
#include "app_rj45.h"

#include "guiwindow.h"
#include "guimessage.h"

#include "wnd_global.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmrj45.h"
#include "wnd_frmcablerange.h"


extern CURR_WINDOW_TYPE enCurWindow;                //当前的窗口

typedef struct function_control
{
	GUILABEL* pLblNumberTitle;			//序号(近端)
	GUICHAR* pStrNumberTitle;
	GUIPICTURE* pBgTypeTitle;			//网线类型
	GUILABEL* pLblType;
	GUICHAR* pStrType;
	GUILABEL* pLblNumber2Title;			//序号(远端)		
	GUICHAR* pStrNumberTitle2;
	GUILABEL* pLblNumber[CABLE_NUMBER];
	GUICHAR* pStrNumber[CABLE_NUMBER];	//近端
	GUIPICTURE* pBgType[CABLE_NUMBER];
	GUILABEL* pLblNumber2[CABLE_NUMBER];
	GUICHAR* pStrNumber2[CABLE_NUMBER];	//远端
	GUIPICTURE* pBgNumber[CABLE_NUMBER];
}functionControl;

static functionControl* pFunctionControl = NULL;	//功能区结构体
static int iCableNumber[CABLE_NUMBER] = { 0.0f };	//网线长度
/*******************************************************************************
*					窗体frmCableSequence中的窗体控件
*******************************************************************************/
static GUIWINDOW *pFrmCableSequence = NULL;

/*******************************************************************************
*                   窗体frmCableSequence桌面上的标题显示控件
*******************************************************************************/
static GUIPICTURE *pCableSequenceBg = NULL;
static GUIPICTURE *pCableSequenceBgIcon = NULL;

static GUICHAR *pCableSequenceStrCurTitle = NULL;
static GUILABEL *pCableSequenceLblCurTitle = NULL;

//测试结果提示信息
static GUIPICTURE *pCableSequenceBgTestReult = NULL;
static GUICHAR *pCableSequenceStrTestResultTitle = NULL;
static GUILABEL *pCableSequenceLblTestResultTitle = NULL;
static GUICHAR *pCableSequenceStrTestResultInfo = NULL;
static GUILABEL *pCableSequenceLblTestResultInfo = NULL;
/*******************************************************************************
*                   窗体frmCableSequence右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pCableSequenceMenu = NULL;
//各项非选中背景资源
static char *pCableSequenceUnpressBmp[2] =
{
    BmpFileDirectory "bg_cablesequence_start.bmp",
    BmpFileDirectory "bg_cablesequence_std.bmp"
};

//各项选中背景资源
static char *pCableSequenceSelectBmp[2] =
{
    BmpFileDirectory "bg_cablesequence_startSelect.bmp",
    BmpFileDirectory "bg_cablesequence_stdSelect.bmp"
};

/*******************************************************************************
*                   窗体frmCableSequence内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmCableSequenceTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//释放文本资源
static int FrmCableSequenceTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体frmCableSequence内部功能函数声明
*******************************************************************************/
//切换网线类型
static void SwitchCableType();
//刷新远端线序数值
static void RefreshNumberValue();
//检测远端网线序号
static void CheckCableSequence();
/*******************************************************************************
*                   窗体frmCableSequence回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void CableSequenceMenuCallBack(int iSelected);

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmCableSequence的初始化函数，建立窗体控件、注册消息处理
int FrmCableSequenceInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象 
	pFrmCableSequence = (GUIWINDOW *)pWndObj;

	//申请内存
	if (pFunctionControl == NULL)
	{
		pFunctionControl = (functionControl*)malloc(sizeof(functionControl));
	}

	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
	FrmCableSequenceTextRes_Init(NULL, 0, NULL, 0);
	//初始化参数
	InitialCableSequenceParameter();
	/***************************************************************************
	*                      创建桌面上各个区域的背景控件
	***************************************************************************/
	pCableSequenceBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_cablesequence.bmp");
	pCableSequenceBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_cablesequence_icon.bmp");
	//标题栏文本
	pCableSequenceLblCurTitle = CreateLabel(40, 12, 100, 16, pCableSequenceStrCurTitle);
	//菜单栏
	int i = 0;
	//菜单栏
	unsigned int StrConfigMenu[2] = {
		RJ45_LBL_CHECK,
		RJ45_LBL_STANDARD
		
	};
	/***************************************************************************
	*                       创建右侧的菜单栏控件
	***************************************************************************/
	pCableSequenceMenu = CreateWndMenu1(2, sizeof(StrConfigMenu), StrConfigMenu, 0xff00,
		0, 1, 41, CableSequenceMenuCallBack);
	//设置菜单栏背景
	for (i = 0; i < 2; i++)
	{
		SetWndMenuItemBg(i, pCableSequenceUnpressBmp[i], pCableSequenceMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pCableSequenceSelectBmp[i], pCableSequenceMenu, MENU_SELECT);
	}
	
	//功能区
	pFunctionControl->pLblNumberTitle = CreateLabel(54, 51, 50, 16, pFunctionControl->pStrNumberTitle);
	pFunctionControl->pLblType = CreateLabel(224, 51, 100, 16, pFunctionControl->pStrType);
	pFunctionControl->pBgTypeTitle = CreatePicture(224, 51, 101, 16, BmpFileDirectory"bg_cable_type.bmp");
	pFunctionControl->pLblNumber2Title = CreateLabel(436, 51, 50, 16, pFunctionControl->pStrNumberTitle2);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblNumberTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblType);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblNumber2Title);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblNumberTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblNumber2Title);
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		pFunctionControl->pLblNumber[i] = CreateLabel(54, 105 + 39 * i, 50, 16, pFunctionControl->pStrNumber[i]);
		pFunctionControl->pBgType[i] = CreatePicture(168, 105 + 39 * i + 3, 200, 10, pBmpCableTypeA[i]);//默认A类线序
		pFunctionControl->pLblNumber2[i] = CreateLabel(436, 105 + 39 * i, 50, 16, pFunctionControl->pStrNumber2[i]);
		pFunctionControl->pBgNumber[i] = CreatePicture(436, 105 + 39 * i, 50, 16, BmpFileDirectory"bg_cable_type.bmp");
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblNumber[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblNumber2[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblNumber[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblNumber2[i]);
	}
	//测试结果信息
	pCableSequenceBgTestReult = CreatePicture(80, 105 + 39 * i, 100, 16, BmpFileDirectory"bg_cable_type.bmp");
	pCableSequenceLblTestResultTitle = CreateLabel(20, 105 + 39 * i, 50, 16, pCableSequenceStrTestResultTitle);
	pCableSequenceLblTestResultInfo = CreateLabel(80, 105 + 39 * i, 100, 16, pCableSequenceStrTestResultInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCableSequenceLblTestResultTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCableSequenceLblTestResultInfo);
	/***************************************************************************
	*           注册窗体(因为所有的按键事件都统一由窗体进行处理)
	***************************************************************************/
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmCableSequence, pFrmCableSequence);
	AddWndMenuToComp1(pCableSequenceMenu, pFrmCableSequence);

	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	LoginWndMenuToMsg1(pCableSequenceMenu, pFrmCableSequence);

	return iReturn;
}
//窗体frmCableSequence的退出函数，释放所有资源
int FrmCableSequenceExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象
	pFrmCableSequence = (GUIWINDOW *)pWndObj;
	/***************************************************************************
	*                       清空消息队列中的消息注册项
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	/***************************************************************************
	*                       从当前窗体中注销窗体控件
	***************************************************************************/
	ClearWindowComp(pFrmCableSequence);

	//销毁控件
	DestroyPicture(&pCableSequenceBg);
	DestroyPicture(&pCableSequenceBgIcon);
	DestroyLabel(&pCableSequenceLblCurTitle);

	int i;

	//销毁功能区控件
	DestroyLabel(&pFunctionControl->pLblNumberTitle);
	DestroyLabel(&pFunctionControl->pLblType);
	DestroyPicture(&pFunctionControl->pBgTypeTitle);
	DestroyLabel(&pFunctionControl->pLblNumber2Title);
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		DestroyLabel(&pFunctionControl->pLblNumber[i]);
		DestroyPicture(&pFunctionControl->pBgType[i]);
		DestroyPicture(&pFunctionControl->pBgNumber[i]);
		DestroyLabel(&pFunctionControl->pLblNumber2[i]);
	}
	//测试结果信息
	DestroyPicture(&pCableSequenceBgTestReult);
	DestroyLabel(&pCableSequenceLblTestResultTitle);
	DestroyLabel(&pCableSequenceLblTestResultInfo);
	//销毁菜单栏
	DestroyWndMenu1(&pCableSequenceMenu);
	//销毁文本资源
	FrmCableSequenceTextRes_Exit(NULL, 0, NULL, 0);
	GuiMemFree(pFunctionControl);
	//清除参数
	ClearCableSequenceParameter();

	return iReturn;
}
//窗体frmCableSequence的绘制函数，绘制整个窗体
int FrmCableSequencePaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	DisplayPicture(pCableSequenceBg);
	// DisplayPicture(pCableSequenceBgIcon);
	DisplayLabel(pCableSequenceLblCurTitle);

	int i;

	enCurWindow = ENUM_CABLE_SEQUENCE_WIN;	//设置当前窗体

	//显示功能区
	DisplayLabel(pFunctionControl->pLblNumberTitle);
	DisplayPicture(pFunctionControl->pBgTypeTitle);
	DisplayLabel(pFunctionControl->pLblType);
	DisplayLabel(pFunctionControl->pLblNumber2Title);

	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		DisplayLabel(pFunctionControl->pLblNumber[i]);
		DisplayPicture(pFunctionControl->pBgType[i]);
		DisplayPicture(pFunctionControl->pBgNumber[i]);
		DisplayLabel(pFunctionControl->pLblNumber2[i]);
	}
	//显示测试信息
	DisplayLabel(pCableSequenceLblTestResultTitle);
	DisplayPicture(pCableSequenceBgTestReult);
	DisplayLabel(pCableSequenceLblTestResultInfo);
	//显示菜单栏
	DisplayWndMenu1(pCableSequenceMenu);
	//刷新菜单栏
	for (i = 0; i < 2; ++i)
	{
		SetWndMenuItemEnble(i, 1, pCableSequenceMenu);
	}

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}
//窗体frmCableSequence的循环函数，进行窗体循环
int FrmCableSequenceLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//禁止并停止窗体循环
	SendWndMsg_LoopDisable(pWndObj);
	return iReturn;
}
//窗体frmCableSequence的挂起函数，进行窗体挂起前预处理
int FrmCableSequencePause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}
//窗体frmCableSequence的恢复函数，进行窗体恢复前预处理
int FrmCableSequenceResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/*******************************************************************************
*                   窗体frmCableSequence内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmCableSequenceTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pCableSequenceStrCurTitle = GetCurrLanguageText(RJ45_LBL_SEQUENCE);

	//功能区
	pFunctionControl->pStrNumberTitle = GetCurrLanguageText(CABLE_SEQUENCE_LBL_NUMBER1);
	pFunctionControl->pStrType = TransString("EIA/TIA568A");
	pFunctionControl->pStrNumberTitle2 = GetCurrLanguageText(CABLE_SEQUENCE_LBL_NUMBER2);
	char* pStrType[CABLE_NUMBER] =
	{ "1", "2", "3", "4", "5", "6", "7", "8" };
	int i;
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		pFunctionControl->pStrNumber[i] = TransString(pStrType[i]);
		pFunctionControl->pStrNumber2[i] = TransString("--");
	}
	//测试结果信息
	pCableSequenceStrTestResultTitle = GetCurrLanguageText(RJ45_LBL_RESULT);
	pCableSequenceStrTestResultInfo = TransString("");

	return iReturn;
}

//释放文本资源
static int FrmCableSequenceTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pCableSequenceStrCurTitle);

	GuiMemFree(pFunctionControl->pStrNumberTitle);
	GuiMemFree(pFunctionControl->pStrType);
	GuiMemFree(pFunctionControl->pStrNumberTitle2);
	int i;
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		GuiMemFree(pFunctionControl->pStrNumber[i]);
		GuiMemFree(pFunctionControl->pStrNumber2[i]);
	}

	//测试结果
	GuiMemFree(pCableSequenceStrTestResultTitle);
	GuiMemFree(pCableSequenceStrTestResultInfo);

	return iReturn;
}

/*******************************************************************************
*                   窗体frmCableSequence内部功能函数声明
*******************************************************************************/
//切换网线类型
static void SwitchCableType()
{
	static int iType = 0;
	iType = iType ? 0 : 1;
	int i = 0;

	GuiMemFree(pFunctionControl->pStrType);

	if (iType)
	{
		pFunctionControl->pStrType = TransString("EIA/TIA568B");

		for (i = 0; i < CABLE_NUMBER; ++i)
		{
			SetPictureBitmap(pBmpCableTypeB[i], pFunctionControl->pBgType[i]);
			DisplayPicture(pFunctionControl->pBgType[i]);
		}
	}
	else
	{
		pFunctionControl->pStrType = TransString("EIA/TIA568A");

		for (i = 0; i < CABLE_NUMBER; ++i)
		{
			SetPictureBitmap(pBmpCableTypeA[i], pFunctionControl->pBgType[i]);
			DisplayPicture(pFunctionControl->pBgType[i]);
		}
	}

	SetLabelText(pFunctionControl->pStrType, pFunctionControl->pLblType);
	DisplayPicture(pFunctionControl->pBgTypeTitle);
	DisplayLabel(pFunctionControl->pLblType);
	RefreshScreen(__FILE__, __func__, __LINE__);
}

//刷新远端线序数值
static void RefreshNumberValue()
{
	int i;
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		char buf[10] = { 0 };
		sprintf(buf, "%d", iCableNumber[i]);
		pFunctionControl->pStrNumber2[i] = TransString(buf);
		SetLabelText(pFunctionControl->pStrNumber2[i], pFunctionControl->pLblNumber2[i]);
		DisplayPicture(pFunctionControl->pBgNumber[i]);
		DisplayLabel(pFunctionControl->pLblNumber2[i]);
	}

	RefreshScreen(__FILE__, __func__, __LINE__);
}

//检测远端网线序号
static void CheckCableSequence()
{
	//检测网线测序模块
	int result = TestCableSequence();
	int i = 0;

	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		if (result & (0x1 << i))
		{
			iCableNumber[i] = i+1;
		}
		else
		{
			iCableNumber[i] = -1;
		}
	}
	//显示测试信息
	DisplayLabel(pCableSequenceLblTestResultTitle);
	DisplayPicture(pCableSequenceBgTestReult);
	pCableSequenceStrTestResultInfo = GetCurrLanguageText(RJ45_LBL_TEST_COMPLETED);
	SetLabelText(pCableSequenceStrTestResultInfo, pCableSequenceLblTestResultInfo);
	DisplayLabel(pCableSequenceLblTestResultInfo);
	//刷新长度
	RefreshNumberValue();
}

/*******************************************************************************
*                   窗体frmCableSequence回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void CableSequenceMenuCallBack(int iSelected)
{
	//窗体
	GUIWINDOW *pWnd = NULL;

	switch (iSelected)
	{
	case 0://check//也就是开始
		CheckCableSequence();
		break;
	case 1://standard
		SwitchCableType();
		break;
	case BACK_DOWN:
	case HOME_DOWN:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmRJ45Init, FrmRJ45Exit,
							FrmRJ45Paint, FrmRJ45Loop,
							FrmRJ45Pause, FrmRJ45Resume,
							NULL);				//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmCableSequence); //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			//发送消息以便调用新的窗体
		break;
	default:
		break;
	}
}

//创建网线测距界面
void CreateCableSequenceWindow()
{
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmCableSequenceInit, FrmCableSequenceExit,
		FrmCableSequencePaint, FrmCableSequenceLoop,
		FrmCableSequencePause, FrmCableSequenceResume,
		NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());		//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);
}