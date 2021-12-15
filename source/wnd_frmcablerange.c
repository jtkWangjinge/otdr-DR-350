/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcablerange.c
* 摘    要：  实现主窗体wnd_frmcablerange的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmcablerange.h"

#include "app_unitconverter.h"

#include "guiwindow.h"
#include "guimessage.h"

#include "wnd_global.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmrj45.h"
#include "wnd_frmcablesequence.h"

//标题栏按钮背景图片
char* pStrMenu_unpress[2] =
{
	BmpFileDirectory"btn_test_unpress.bmp",
	BmpFileDirectory"btn_report_unpress.bmp"
};

char* pStrMenu_press[2] =
{
	BmpFileDirectory"btn_test_press.bmp",
	BmpFileDirectory"btn_report_press.bmp"
};

extern CURR_WINDOW_TYPE enCurWindow;                //当前的窗口

typedef struct function_control
{
	GUILABEL* pLblNumberTitle;
	GUICHAR* pStrNumberTitle;
	GUIPICTURE* pBgTypeTitle;
	GUILABEL* pLblType;
	GUICHAR* pStrType;
	GUIPICTURE* pBgLengthTitle;
	GUILABEL* pLblLengthTitle;
	GUICHAR* pStrLengthTitle;
	GUILABEL* pLblNumber[CABLE_NUMBER];
	GUICHAR* pStrNumber[CABLE_NUMBER];
	GUIPICTURE* pBgType[CABLE_NUMBER];
	GUILABEL* pLblLength[CABLE_NUMBER];
	GUICHAR* pStrLength[CABLE_NUMBER];
	GUIPICTURE* pBgLength[CABLE_NUMBER];
}functionControl;

char* pBmpCableTypeA[CABLE_NUMBER] =
{
	BmpFileDirectory"bg_cable_white_green.bmp",
	BmpFileDirectory"bg_cable_green.bmp",
	BmpFileDirectory"bg_cable_white_organge.bmp",
	BmpFileDirectory"bg_cable_blue.bmp",
	BmpFileDirectory"bg_cable_white_blue.bmp",
	BmpFileDirectory"bg_cable_organge.bmp",
	BmpFileDirectory"bg_cable_white_brown.bmp",
	BmpFileDirectory"bg_cable_brown.bmp"
};

char* pBmpCableTypeB[CABLE_NUMBER] =
{
	BmpFileDirectory"bg_cable_white_organge.bmp",
	BmpFileDirectory"bg_cable_organge.bmp",
	BmpFileDirectory"bg_cable_white_green.bmp",
	BmpFileDirectory"bg_cable_blue.bmp",
	BmpFileDirectory"bg_cable_white_blue.bmp",
	BmpFileDirectory"bg_cable_green.bmp",
	BmpFileDirectory"bg_cable_white_brown.bmp",
	BmpFileDirectory"bg_cable_brown.bmp"
};

unsigned int pUnit[UNIT_COUNT] = 
{ 
	CABLE_RANGE_LBL_LENGTH_M, 
	CABLE_RANGE_LBL_LENGTH_KM, 
	CABLE_RANGE_LBL_LENGTH_FT, 
	CABLE_RANGE_LBL_LENGTH_KFT, 
	CABLE_RANGE_LBL_LENGTH_MI,
	CABLE_RANGE_LBL_LENGTH_YD
};

static functionControl* pFunctionControl = NULL;	//功能区结构体
static int iLengthUnit = 0;							//默认是米
static float fCableLength[CABLE_NUMBER] = { 0.0f };	//网线长度
/*******************************************************************************
*					窗体frmCableRange中的窗体控件
*******************************************************************************/
static GUIWINDOW *pFrmCableRange = NULL;

/*******************************************************************************
*                   窗体frmCableRange桌面上的标题显示控件
*******************************************************************************/
static GUIPICTURE *pCableRangeBg = NULL;
static GUIPICTURE *pCableRangeBgIcon = NULL;

static GUICHAR *pCableRangeStrCurTitle = NULL;
static GUILABEL *pCableRangeLblCurTitle = NULL;

//测试结果提示信息
static GUIPICTURE *pCableRangeBgTestReult = NULL;
static GUICHAR *pCableRangeStrTestResultTitle = NULL;
static GUILABEL *pCableRangeLblTestResultTitle = NULL;
static GUICHAR *pCableRangeStrTestResultInfo = NULL;
static GUILABEL *pCableRangeLblTestResultInfo = NULL;
/*******************************************************************************
*                   窗体frmCableRange右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pCableRangeMenu = NULL;
//各项非选中背景资源
static char *pCableRangeUnpressBmp[3] =
{
    BmpFileDirectory "bg_cablerange_start.bmp",
    BmpFileDirectory "bg_cablerange_unit.bmp",
    BmpFileDirectory "bg_cablerange_std.bmp"
};

//各项选中背景资源
static char *pCableRangeSelectBmp[3] =
{
    BmpFileDirectory "bg_cablerange_startSelect.bmp",
    BmpFileDirectory "bg_cablerange_unitSelect.bmp",
    BmpFileDirectory "bg_cablerange_stdSelect.bmp"
};
/*******************************************************************************
*                   窗体frmCableRange内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmCableRangeTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//释放文本资源
static int FrmCableRangeTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体frmCableRange内部功能函数声明
*******************************************************************************/
//切换网线类型
static void SwitchCableType();
//刷新长度数值
static void RefreshLengthValue();
//切换单位
static void SwitchLengthUnit();
//检测网线长度
static void CheckCableLength();
/*******************************************************************************
*                   窗体frmCableRange回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void CableRangeMenuCallBack(int iSelected);

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmCableRange的初始化函数，建立窗体控件、注册消息处理
int FrmCableRangeInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象 
	pFrmCableRange = (GUIWINDOW *)pWndObj;

	//申请内存
	if (pFunctionControl == NULL)
	{
		pFunctionControl = (functionControl*)malloc(sizeof(functionControl));
	}

	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
	FrmCableRangeTextRes_Init(NULL, 0, NULL, 0);
	memset(fCableLength, 0, sizeof(float)*CABLE_NUMBER);
	/***************************************************************************
	*                      创建桌面上各个区域的背景控件
	***************************************************************************/
	pCableRangeBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_cablerange.bmp");
	pCableRangeBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_cablerange_icon.bmp");
	//标题栏文本
	pCableRangeLblCurTitle = CreateLabel(40, 12, 100, 16, pCableRangeStrCurTitle);
	//菜单栏
	int i = 0;

	//菜单栏
	unsigned int StrConfigMenu[3] = {
		RJ45_LBL_CHECK,
		CABLE_RANGE_LBL_UNIT,
		RJ45_LBL_STANDARD
		
	};
	/***************************************************************************
	*                       创建右侧的菜单栏控件
	***************************************************************************/
	pCableRangeMenu = CreateWndMenu1(3, sizeof(StrConfigMenu), StrConfigMenu, 0xff00,
		0, 1, 41, CableRangeMenuCallBack);
	//设置菜单栏背景
	for (i = 0; i < 3; i++)
	{
		SetWndMenuItemBg(i, pCableRangeUnpressBmp[i], pCableRangeMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pCableRangeSelectBmp[i], pCableRangeMenu, MENU_SELECT);
	}
	//功能区
	pFunctionControl->pLblNumberTitle = CreateLabel(54, 51, 50, 16, pFunctionControl->pStrNumberTitle);
	pFunctionControl->pLblType = CreateLabel(224, 51, 100, 16, pFunctionControl->pStrType);
	pFunctionControl->pBgTypeTitle = CreatePicture(224, 51, 101, 16, BmpFileDirectory "bg_cable_type.bmp");
	pFunctionControl->pBgLengthTitle = CreatePicture(411, 51, 101, 16, BmpFileDirectory "bg_cable_type.bmp");
	pFunctionControl->pLblLengthTitle = CreateLabel(411, 51, 100, 16, pFunctionControl->pStrLengthTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblNumberTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblType);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblLengthTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblNumberTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblLengthTitle);

	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		pFunctionControl->pLblNumber[i] = CreateLabel(54, 105 + 39 * i, 50, 16, pFunctionControl->pStrNumber[i]);
		pFunctionControl->pBgType[i] = CreatePicture(168, 105 + 39 * i + 3, 200, 10, pBmpCableTypeA[i]); //默认A类线序
		pFunctionControl->pLblLength[i] = CreateLabel(436, 105 + 39 * i, 50, 16, pFunctionControl->pStrLength[i]);
		pFunctionControl->pBgLength[i] = CreatePicture(436, 105 + 39 * i, 50, 16, BmpFileDirectory "bg_cable_type.bmp");
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblNumber[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFunctionControl->pLblLength[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblNumber[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pFunctionControl->pLblLength[i]);
	}
	//测试结果信息
	pCableRangeBgTestReult = CreatePicture(80, 105 + 39 * i, 100, 16, BmpFileDirectory "bg_cable_type.bmp");
	pCableRangeLblTestResultTitle = CreateLabel(20, 105 + 39 * i, 50, 16, pCableRangeStrTestResultTitle);
	pCableRangeLblTestResultInfo = CreateLabel(80, 105 + 39 * i, 100, 16, pCableRangeStrTestResultInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCableRangeLblTestResultTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCableRangeLblTestResultInfo);
	/***************************************************************************
	*           注册窗体(因为所有的按键事件都统一由窗体进行处理)
	***************************************************************************/
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmCableRange, pFrmCableRange);
	AddWndMenuToComp1(pCableRangeMenu, pFrmCableRange);

	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	LoginWndMenuToMsg1(pCableRangeMenu, pFrmCableRange);

	return iReturn;
}
//窗体frmCableRange的退出函数，释放所有资源
int FrmCableRangeExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象
	pFrmCableRange = (GUIWINDOW *)pWndObj;
	/***************************************************************************
	*                       清空消息队列中的消息注册项
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	/***************************************************************************
	*                       从当前窗体中注销窗体控件
	***************************************************************************/
	ClearWindowComp(pFrmCableRange);

	//销毁控件
	DestroyPicture(&pCableRangeBg);
	DestroyPicture(&pCableRangeBgIcon);
	DestroyLabel(&pCableRangeLblCurTitle);

	int i;

	//销毁功能区控件
	DestroyLabel(&pFunctionControl->pLblNumberTitle);
	DestroyLabel(&pFunctionControl->pLblType);
	DestroyPicture(&pFunctionControl->pBgTypeTitle);
	DestroyPicture(&pFunctionControl->pBgLengthTitle);
	DestroyLabel(&pFunctionControl->pLblLengthTitle);
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		DestroyLabel(&pFunctionControl->pLblNumber[i]);
		DestroyPicture(&pFunctionControl->pBgType[i]);
		DestroyPicture(&pFunctionControl->pBgLength[i]);
		DestroyLabel(&pFunctionControl->pLblLength[i]);
	}

	//测试结果信息
	DestroyPicture(&pCableRangeBgTestReult);
	DestroyLabel(&pCableRangeLblTestResultTitle);
	DestroyLabel(&pCableRangeLblTestResultInfo);

	//销毁菜单栏
	DestroyWndMenu1(&pCableRangeMenu);
	//销毁文本资源
	FrmCableRangeTextRes_Exit(NULL, 0, NULL, 0);
	GuiMemFree(pFunctionControl);

	return iReturn;
}
//窗体frmCableRange的绘制函数，绘制整个窗体
int FrmCableRangePaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	DisplayPicture(pCableRangeBg);
	// DisplayPicture(pCableRangeBgIcon);
	DisplayLabel(pCableRangeLblCurTitle);

	int i;

	enCurWindow = ENUM_CABLE_RANGE_WIN;	//设置当前窗体

	//显示功能区
	DisplayLabel(pFunctionControl->pLblNumberTitle);
	DisplayPicture(pFunctionControl->pBgTypeTitle);
	DisplayLabel(pFunctionControl->pLblType);
	DisplayPicture(pFunctionControl->pBgLengthTitle);
	DisplayLabel(pFunctionControl->pLblLengthTitle);
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		DisplayLabel(pFunctionControl->pLblNumber[i]);
		DisplayPicture(pFunctionControl->pBgType[i]);
		DisplayPicture(pFunctionControl->pBgLength[i]);
		DisplayLabel(pFunctionControl->pLblLength[i]);
	}

	//显示测试信息
	DisplayLabel(pCableRangeLblTestResultTitle);
	DisplayPicture(pCableRangeBgTestReult);
	DisplayLabel(pCableRangeLblTestResultInfo);

	//显示菜单栏
	DisplayWndMenu1(pCableRangeMenu);
	//刷新菜单栏
	for (i = 0; i < 3; ++i)
	{
		SetWndMenuItemEnble(i, 1, pCableRangeMenu);
	}

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}
//窗体frmCableRange的循环函数，进行窗体循环
int FrmCableRangeLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//禁止并停止窗体循环
	SendWndMsg_LoopDisable(pWndObj);
	return iReturn;
}
//窗体frmCableRange的挂起函数，进行窗体挂起前预处理
int FrmCableRangePause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}
//窗体frmCableRange的恢复函数，进行窗体恢复前预处理
int FrmCableRangeResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/*******************************************************************************
*                   窗体frmCableRange内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmCableRangeTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	pCableRangeStrCurTitle = GetCurrLanguageText(RJ45_LBL_RANGE);
	//功能区
	pFunctionControl->pStrNumberTitle = GetCurrLanguageText(CABLE_RANGE_LBL_NUMBER);
	pFunctionControl->pStrType = TransString("EIA/TIA568A");
	pFunctionControl->pStrLengthTitle = GetCurrLanguageText(CABLE_RANGE_LBL_LENGTH_M);
	char* pStrType[CABLE_NUMBER] = {"1", "2", "3", "4", "5", "6", "7", "8"};
	int i;
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		pFunctionControl->pStrNumber[i] = TransString(pStrType[i]);
		pFunctionControl->pStrLength[i] = TransString("--");
	}
	//测试结果信息
	pCableRangeStrTestResultTitle = GetCurrLanguageText(RJ45_LBL_RESULT);
	pCableRangeStrTestResultInfo = TransString("");

	return iReturn;
}

//释放文本资源
static int FrmCableRangeTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pCableRangeStrCurTitle);

	GuiMemFree(pFunctionControl->pStrNumberTitle);
	GuiMemFree(pFunctionControl->pStrType);
	GuiMemFree(pFunctionControl->pStrLengthTitle);
	int i;
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		GuiMemFree(pFunctionControl->pStrNumber[i]);
		GuiMemFree(pFunctionControl->pStrLength[i]);
	}

	//测试结果
	GuiMemFree(pCableRangeStrTestResultTitle);
	GuiMemFree(pCableRangeStrTestResultInfo);

	return iReturn;
}

/*******************************************************************************
*                   窗体frmCableRange内部功能函数声明
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

//刷新长度数值
static void RefreshLengthValue()
{
	char buf[20] = { 0 };
	int i;
	for (i = 0; i < CABLE_NUMBER; ++i)
	{
		double length = UnitConverter_Dist_Float2Float(UNIT_M, iLengthUnit, fCableLength[i]);
		sprintf(buf, "%.1f", length);
		pFunctionControl->pStrLength[i] = TransString(buf);
		SetLabelText(pFunctionControl->pStrLength[i], pFunctionControl->pLblLength[i]);
		DisplayPicture(pFunctionControl->pBgLength[i]);
		DisplayLabel(pFunctionControl->pLblLength[i]);
	}

	pFunctionControl->pStrLengthTitle = GetCurrLanguageText(pUnit[iLengthUnit]);
	SetLabelText(pFunctionControl->pStrLengthTitle, pFunctionControl->pLblLengthTitle);
	DisplayPicture(pFunctionControl->pBgLengthTitle);
	DisplayLabel(pFunctionControl->pLblLengthTitle);

	RefreshScreen(__FILE__, __func__, __LINE__);
}

//切换单位
static void SwitchLengthUnit()
{
	(iLengthUnit < UNIT_COUNT - 1) ? (iLengthUnit++) : (iLengthUnit = 0);
	RefreshLengthValue();
}

//检测网线长度
static void CheckCableLength()
{
	//调用算法接口
	//显示测试信息
	DisplayLabel(pCableRangeLblTestResultTitle);
	DisplayPicture(pCableRangeBgTestReult);
	pCableRangeStrTestResultInfo = GetCurrLanguageText(RJ45_LBL_TEST_COMPLETED);
	SetLabelText(pCableRangeStrTestResultInfo, pCableRangeLblTestResultInfo);
	DisplayLabel(pCableRangeLblTestResultInfo);
	//刷新长度
	RefreshLengthValue();
}

/*******************************************************************************
*                   窗体frmCableRange回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void CableRangeMenuCallBack(int iSelected)
{ 
	//窗体
	GUIWINDOW *pWnd = NULL;

	switch (iSelected)
	{
	case 0://check
		CheckCableLength();
		break;
	case 1://unit
		SwitchLengthUnit();
		break;
	case 2://standard
		SwitchCableType();
		break;
	case BACK_DOWN:
	case HOME_DOWN:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmRJ45Init, FrmRJ45Exit,
							FrmRJ45Paint, FrmRJ45Loop,
							FrmRJ45Pause, FrmRJ45Resume,
							NULL);				  //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmCableRange); //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			  //发送消息以便调用新的窗体
		break;
	default:
		break;
	}
}

//创建网线测距界面
void CreateCableRangeWindow()
{
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmCableRangeInit, FrmCableRangeExit,
		FrmCableRangePaint, FrmCableRangeLoop,
		FrmCableRangePause, FrmCableRangeResume,
		NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());		//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);
}