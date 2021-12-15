/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmserialnum.c
* 摘    要：  实现主窗体wnd_frmserialnum的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmserialnum.h"

#include "app_parameter.h"
#include "app_systemsettings.h"

#include "guiwindow.h"
#include "guipicture.h"
#include "guilabel.h"

#include "wnd_frmmenubak.h"
#include "wnd_frmime.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmauthorization.h"
#include "wnd_frmmaintancedate.h"

//序列号信息最大个数
#define MAX_SN_COUNT				6
#define SnNumCount					100
#define SNBITCount					20
#define MachineTypeCount			1
//标题栏按钮背景图片
char* pStrButton_unpress[3] =
{
	BmpFileDirectory"btn_test_unpress1.bmp",
	BmpFileDirectory"btn_threshold_unpress1.bmp",
	BmpFileDirectory"btn_report_unpress1.bmp"
};

char* pStrButton_press[3] =
{
	BmpFileDirectory"btn_test_press1.bmp",
	BmpFileDirectory"btn_threshold_press1.bmp",
	BmpFileDirectory"btn_report_press1.bmp"
};

static SerialNum stSn =
{
	"I",
	"A",
	"01",
	"21",
	"12",
	"000"
};

static char sn[SNBITCount + 1];

static char* SnNumSet[SnNumCount] =
{
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
};

static char* pSnInfo[MAX_SN_COUNT] =
{
	"MachineName:",
	"MachineType:",
	"Number:",
	"Year:",
	"Month:",
	"Day:"
};

static char* machineTypeStr[MachineTypeCount] =
{
	"O10"
};

extern PSerialNum pCurSN;
static char* pNumber = NULL;
/*******************************************************************************
*					窗体frmSerialnum中的窗体控件
*******************************************************************************/
static GUIWINDOW *pFrmSerialnum = NULL;

/*******************************************************************************
*                   窗体frmSerialnum桌面上的背景控件
*******************************************************************************/
static GUIPICTURE *pSerialnumBg = NULL;
//返回按钮
static GUIPICTURE *pSerialnumBtnBack = NULL;
static GUIPICTURE *pSerialnumBtnTitle[3] = {NULL};		//authorization,serialnum,LeaseDate
static GUILABEL *pSerialnumLblTitle[3] = {NULL};
static GUICHAR *pSerialnumStrTitle[3] = {NULL};
/*******************************************************************************
*                   窗体frmSerialnum桌面上的标题显示控件
*******************************************************************************/
static GUICHAR *pSerialnumStrCurTitle = NULL;
static GUILABEL *pSerialnumLblCurTitle = NULL;
//功能区按钮
static GUILABEL *pSerialnumLblInfo[MAX_SN_COUNT] = { NULL };
static GUICHAR *pSerialnumStrInfo[MAX_SN_COUNT] = { NULL };
//机器名称
static GUILABEL *pSerialnumLblMachineName = NULL;
static GUICHAR *pSerialnumStrMachineName = NULL;
//机器类型
static GUICHAR *pSerialnumStrMachineType[MachineTypeCount] = { NULL };
//年
static GUICHAR *pSerialnumStrYear[SnNumCount] = { NULL };
//月
static GUICHAR *pSerialnumStrMonth[12] = { NULL };
//日
static GUICHAR *pSerialnumStrDay[31] = { NULL };
//编号
static GUIPICTURE *pSerialnumBgNumber = NULL;
static GUICHAR *pSerialnumStrNumber = NULL;
static GUILABEL *pSerialnumLblNumber = NULL;

static SELECTOR *pSerialnumSelector[4] = { NULL };
//序列号文本
static GUILABEL *pSerialnumLblSNtitle = NULL;
static GUICHAR *pSerialnumStrSNtitle = NULL;
static GUILABEL *pSerialnumLblSNvalue = NULL;
static GUICHAR *pSerialnumStrSNvalue = NULL;
/*******************************************************************************
*                   窗体frmSerialnum内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmSerialnumTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//释放文本资源
static int FrmSerialnumTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体frmSerialnum内部按键响应函数声明
*******************************************************************************/
//点击返回键响应函数
static int SerialnumBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SerialnumBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击菜单栏响应函数
static int SerialnumBtnTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SerialnumBtnTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击编号响应函数
static int SerialnumBgNumber_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SerialnumBgNumber_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击按键响应函数
static int WndSerialnumKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndSerialnumKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体frmSerialnum回调函数声明
*******************************************************************************/
//设置机器类型的回调函数
static void SerialnumMachineTypeBackFunc(int iSelected);
//设置年份的回调函数
static void SerialnumYearBackFunc(int iSelected);
//设置月份的回调函数
static void SerialnumMonthBackFunc(int iSelected);
//重绘窗体
static void ReCreateWindow(GUIWINDOW **pWnd);
//设置日期的回调函数
static void SerialnumDayBackFunc(int iSelected);
//机器编号输入回调函数
static void MachineNumberInputOk(void);
//检测下标值
static int checkIndex(char* arg, char** dest, int size);
//刷新序列号信息
static void RefreshSN();
/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

//窗体frmSerialnum的初始化函数，建立窗体控件、注册消息处理
int FrmSerialnumInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象 
	pFrmSerialnum = (GUIWINDOW *)pWndObj;
	//获取系统的SN
	memcpy(&stSn, pCurSN, sizeof(SerialNum));
	if (NULL != pNumber) {
		strcpy(stSn.Number, pNumber);
		free(pNumber);
		pNumber = NULL;
	}
	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
	FrmSerialnumTextRes_Init(NULL, 0, NULL, 0);

	/***************************************************************************
	*                      创建桌面上各个区域的背景控件
	***************************************************************************/
	pSerialnumBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factory1.bmp");

	//标题栏文本
	pSerialnumLblCurTitle = CreateLabel(36, 12, 200, 16, pSerialnumStrCurTitle);
	//返回键
	pSerialnumBtnBack = CreatePicture(0, 20, 36, 21, BmpFileDirectory"btn_otdrSet_back.bmp");
	//菜单栏
	int i = 0;
	for (; i < 3; ++i)
	{
		pSerialnumBtnTitle[i] = CreatePicture(10 + 91 * i, 48, 90, 35, pStrButton_unpress[i]);
		pSerialnumLblTitle[i] = CreateLabel(10 + 91 * i, 48 + 10, 90, 16, pSerialnumStrTitle[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSerialnumLblTitle[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pSerialnumLblTitle[i]);
	}

	//功能区
	for (i = 0; i < MAX_SN_COUNT; i++)
	{
		//machineName,machineType,number//年月日
		pSerialnumLblInfo[i] = CreateLabel(130, 100 + 35 * i, 100, 16, pSerialnumStrInfo[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pSerialnumLblInfo[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSerialnumLblInfo[i]);
	}
	//机器名称
	pSerialnumLblMachineName = CreateLabel(300, 100 + 35 * 0, 100, 16, pSerialnumStrMachineName);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSerialnumLblMachineName);
	//机器类型
	int index = checkIndex(stSn.MachineType, SnNumSet, MachineTypeCount + 1);
	index = (index < 0 || index > MachineTypeCount) ? 1 : index;
	pSerialnumSelector[0] = CreateSelector(300, 100 + 35 * 1, MachineTypeCount,
		index-1, pSerialnumStrMachineType, SerialnumMachineTypeBackFunc, 1);
	//机器编号
	pSerialnumBgNumber = CreatePicture(300, 100 + 35 * 2, 100, 20, BmpFileDirectory"bg_serial_number.bmp");
	pSerialnumLblNumber = CreateLabel(300, 100 + 35 * 2+2, 100, 16, pSerialnumStrNumber);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSerialnumLblNumber);
	//年
	index = checkIndex(stSn.Year, SnNumSet, SnNumCount);
	index = (index < 0 || index > SnNumCount) ? 1 : index;
	pSerialnumSelector[1] = CreateSelector(300, 100 + 35 * 3, SnNumCount,
		index - 1, pSerialnumStrYear, SerialnumYearBackFunc, 1);
	//月
	index = checkIndex(stSn.Month, SnNumSet, 12);
	index = (index < 0 || index > 12) ? 1 : index;
	pSerialnumSelector[2] = CreateSelector(300, 100 + 35 * 4, 12,
		index, pSerialnumStrMonth, SerialnumMonthBackFunc, 1);
	//日
	index = checkIndex(stSn.Date, SnNumSet, SnNumCount);
	index = (index <= 0 || index > 31) ? 0 : index-1;
	pSerialnumSelector[3] = CreateSelector(300, 100 + 35 * 5, 31,
		index, pSerialnumStrDay, SerialnumDayBackFunc, 1);
	//序列号
	pSerialnumLblSNtitle = CreateLabel(130, 100 + 35 * i, 100, 16, pSerialnumStrSNtitle);
	pSerialnumLblSNvalue = CreateLabel(300, 100 + 35 * i, 100, 16, pSerialnumStrSNvalue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSerialnumLblSNtitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSerialnumLblSNvalue);
	/***************************************************************************
	*           注册窗体(因为所有的按键事件都统一由窗体进行处理)
	***************************************************************************/
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSerialnum, pFrmSerialnum);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSerialnumBtnBack, pFrmSerialnum);

	for (i = 0; i < 3; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSerialnumBtnTitle[i], pFrmSerialnum);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSerialnumLblTitle[i], pFrmSerialnum);
	}
	//注册控件
	for (i = 0; i < 4; ++i)
	{
		AddSelectorToComp(pSerialnumSelector[i], pFrmSerialnum);
	}

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSerialnumBgNumber, pFrmSerialnum);
	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSerialnumBtnBack, SerialnumBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSerialnumBtnBack, SerialnumBtnBack_Up, NULL, 0, pMsg);

	for (i = 0; i < 3; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSerialnumBtnTitle[i], SerialnumBtnTitle_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSerialnumBtnTitle[i], SerialnumBtnTitle_Up, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSerialnumLblTitle[i], SerialnumBtnTitle_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSerialnumLblTitle[i], SerialnumBtnTitle_Up, NULL, i, pMsg);
	}

	//注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmSerialnum, WndSerialnumKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmSerialnum, WndSerialnumKey_Up, NULL, 0, pMsg);

	for (i = 0; i < 4; i++)
	{
		LoginSelectorToMsg(pSerialnumSelector[i], pMsg);
	}

	//编号
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSerialnumBgNumber, SerialnumBgNumber_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSerialnumBgNumber, SerialnumBgNumber_Up, NULL, 0, pMsg);

	return iReturn;
}

//窗体frmSerialnum的退出函数，释放所有资源
int FrmSerialnumExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//得到当前窗体对象
	pFrmSerialnum = (GUIWINDOW *)pWndObj;
	/***************************************************************************
	*                       清空消息队列中的消息注册项
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	/***************************************************************************
	*                       从当前窗体中注销窗体控件
	***************************************************************************/
	ClearWindowComp(pFrmSerialnum);
	//销毁控件
	DestroyPicture(&pSerialnumBg);
	DestroyLabel(&pSerialnumLblCurTitle);
	DestroyPicture(&pSerialnumBtnBack);
	int i;
	for (i = 0; i < 3; ++i)
	{
		DestroyPicture(&pSerialnumBtnTitle[i]);
		DestroyLabel(&pSerialnumLblTitle[i]);
	}
	//销毁功能区文本
	for (i = 0; i < MAX_SN_COUNT; ++i)
	{
		DestroyLabel(&pSerialnumLblInfo[i]);
	}

	for (i = 0; i < 4; i++)
	{
		DestroySelector(&(pSerialnumSelector[i]));
	}

	DestroyPicture(&pSerialnumBgNumber);
	DestroyLabel(&pSerialnumLblNumber);
	DestroyLabel(&pSerialnumLblMachineName);
	DestroyLabel(&pSerialnumLblSNtitle);
	DestroyLabel(&pSerialnumLblSNvalue);
	//销毁文本
	FrmSerialnumTextRes_Exit(NULL, 0, NULL, 0);
	//save the sn
	memcpy(pCurSN, &stSn, sizeof(SerialNum));
	SetSettingsData((void*)pCurSN, sizeof(SerialNum), SERIAL_NUM);
	SaveSettings(SERIAL_NUM);
	return iReturn;
}

//窗体frmSerialnum的绘制函数，绘制整个窗体
int FrmSerialnumPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	DisplayPicture(pSerialnumBg);
	DisplayLabel(pSerialnumLblCurTitle);

	// DisplayPicture(pSerialnumBtnBack);
	int i;
	for (i = 0; i < 3; ++i)
	{
		if (i == 1)
		{
			SetPictureBitmap(pStrButton_press[i], pSerialnumBtnTitle[i]);
		}
		DisplayPicture(pSerialnumBtnTitle[i]);
		DisplayLabel(pSerialnumLblTitle[i]);
	}

	//显示功能区文本
	for (i = 0; i < MAX_SN_COUNT; ++i)
	{
		DisplayLabel(pSerialnumLblInfo[i]);
	}

	for (i = 0; i < 4; ++i)
	{
		DisplaySelector(pSerialnumSelector[i]);
	}

	DisplayPicture(pSerialnumBgNumber);
	DisplayLabel(pSerialnumLblNumber);

	DisplayLabel(pSerialnumLblMachineName);
	DisplayLabel(pSerialnumLblSNtitle);
	DisplayLabel(pSerialnumLblSNvalue);

	SetPowerEnable(1, 1);
	RefreshSN();	//RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

//窗体frmSerialnum的循环函数，进行窗体循环
int FrmSerialnumLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//禁止并停止窗体循环
	SendWndMsg_LoopDisable(pWndObj);
	return iReturn;
}

//窗体frmSerialnum的挂起函数，进行窗体挂起前预处理
int FrmSerialnumPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

//窗体frmSerialnum的恢复函数，进行窗体恢复前预处理
int FrmSerialnumResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/*******************************************************************************
*                   窗体frmSerialnum内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmSerialnumTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pSerialnumStrCurTitle = TransString("Serial Num");
	pSerialnumStrTitle[0] = TransString("Authorization");
	pSerialnumStrTitle[1] = TransString("SerialNum");
	pSerialnumStrTitle[2] = TransString("LeaseDate");

	int i;
	for (i = 0; i < MAX_SN_COUNT; ++i)
	{
		pSerialnumStrInfo[i] = TransString(pSnInfo[i]);
	}

	for (i = 0; i < MachineTypeCount; i++)
	{
		pSerialnumStrMachineType[i] = TransString(machineTypeStr[i]);
	}

	for (i = 0; i < SnNumCount; ++i)
	{
		pSerialnumStrYear[i] = TransString(SnNumSet[i+1]);
	}

	for (i = 0; i < 12; ++i)
	{
		pSerialnumStrMonth[i] = TransString(SnNumSet[i+1]);
	}

	for (i = 0; i < 31; ++i)
	{
		pSerialnumStrDay[i] = TransString(SnNumSet[i + 1]);
	}

	pSerialnumStrNumber = TransString(stSn.Number);
	pSerialnumStrMachineName = TransString("OTDR");
	pSerialnumStrSNtitle = TransString("SerialNum:");

	return iReturn;
}

//释放文本资源
static int FrmSerialnumTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pSerialnumLblCurTitle);
	GuiMemFree(pSerialnumStrTitle[0]);
	GuiMemFree(pSerialnumStrTitle[1]);
	GuiMemFree(pSerialnumStrTitle[2]);

	int i;
	for (i = 0; i < MAX_SN_COUNT; ++i)
	{
		GuiMemFree(pSerialnumStrInfo[i]);
	}

	for (i = 0; i < MachineTypeCount; i++)
	{
		GuiMemFree(pSerialnumStrMachineType[i]);
	}

	for (i = 0; i < SnNumCount; ++i)
	{
		GuiMemFree(pSerialnumStrYear[i]);
	}

	for (i = 0; i < 12; ++i)
	{
		GuiMemFree(pSerialnumStrMonth[i]);
	}

	for (i = 0; i < 31; ++i)
	{
		GuiMemFree(pSerialnumStrDay[i]);
	}

	GuiMemFree(pSerialnumStrNumber);
	GuiMemFree(pSerialnumStrMachineName);
	GuiMemFree(pSerialnumStrSNtitle);
	GuiMemFree(pSerialnumStrSNvalue);

	return iReturn;
}

/*******************************************************************************
*                   窗体frmSerialnum内部按键响应函数声明
*******************************************************************************/
//点击返回键响应函数
static int SerialnumBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

static int SerialnumBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmSystemSettingInit, FrmSystemSettingExit,
		FrmSystemSettingPaint, FrmSystemSettingLoop,
		FrmSystemSettingPause, FrmSystemSettingResume,
		NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmSerialnum);		//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);

	return iReturn;
}

//点击菜单栏响应函数
static int SerialnumBtnTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

static int SerialnumBtnTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;

	switch (iOutLen)
	{
	case 0:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmAuthorizationInit, FrmAuthorizationExit,
			FrmAuthorizationPaint, FrmAuthorizationLoop,
			FrmAuthorizationPause, FrmAuthorizationResume,
			NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmSerialnum);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case 1:
		break;
	case 2:
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmMaintanceDateInit, FrmMaintanceDateExit,
			FrmMaintanceDatePaint, FrmMaintanceDateLoop,
			FrmMaintanceDatePause, FrmMaintanceDateResume,
			NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmSerialnum);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
		break;
	default:
		break;
	}

	return iReturn;
}

//点击编号响应函数
static int SerialnumBgNumber_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	return iReturn;
}

//点击编号响应函数
static int SerialnumBgNumber_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	IMEInit(stSn.Number, 3, 1, ReCreateWindow, MachineNumberInputOk, NULL);
	return iReturn;
}

//点击按键响应函数
static int WndSerialnumKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

static int WndSerialnumKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	unsigned int uiValue;
	uiValue = (unsigned int)pInArg;
	static int iUpDownSelected = 0;
	static int iLeftRightSelected = 0;
	
	switch (uiValue)
	{
	case KEYCODE_OTDR_LEFT:
	{
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmAuthorizationInit, FrmAuthorizationExit,
							FrmAuthorizationPaint, FrmAuthorizationLoop,
							FrmAuthorizationPause, FrmAuthorizationResume,
							NULL);			  //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmSerialnum); //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
	}
	break;
	case KEYCODE_OTDR_RIGHT:
	{
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmMaintanceDateInit, FrmMaintanceDateExit,
							FrmMaintanceDatePaint, FrmMaintanceDateLoop,
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			  //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmSerialnum); //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
	}
	break;
	case KEYCODE_UP:
		if (iUpDownSelected > 0)
		{
			iUpDownSelected--;
		}
		break;
	case KEYCODE_DOWN:
		if (iUpDownSelected < 4)
		{
			iUpDownSelected++;
		}
		break;
	case KEYCODE_LEFT:
		iLeftRightSelected = 0;
		break;
	case KEYCODE_RIGHT:
		iLeftRightSelected = 1;
		break;
	case KEYCODE_ENTER:
		if (iUpDownSelected == 1)
		{
			SerialnumBgNumber_Up(NULL, 0, NULL, 0);
		}
		else
		{
			if (iUpDownSelected < 1)
			{
				HandleSelectorAddOrDec(pSerialnumSelector[iUpDownSelected], iLeftRightSelected);
			}
			else
			{
				HandleSelectorAddOrDec(pSerialnumSelector[iUpDownSelected - 1], iLeftRightSelected);
			}
		}
		break;
	case KEYCODE_ESC:
		SerialnumBtnBack_Up(NULL, 0, NULL, 0);
	default:
		break;
	}

	return iReturn;
}

//设置机器类型的回调函数
static void SerialnumMachineTypeBackFunc(int iSelected)
{
	char buf[4] = { 0 };
	sprintf(buf, "%c", 'A'+iSelected);
	strcpy(stSn.MachineType, buf);
	//printSN();
	RefreshSN();
}

//设置年份的回调函数
static void SerialnumYearBackFunc(int iSelected)
{
	strcpy(stSn.Year, SnNumSet[iSelected + 1]);
	RefreshSN();
}

//设置月份的回调函数
static void SerialnumMonthBackFunc(int iSelected)
{
	strcpy(stSn.Month, SnNumSet[iSelected + 1]);
	RefreshSN();
}

//设置日期的回调函数
static void SerialnumDayBackFunc(int iSelected)
{
	sprintf(stSn.Date, "%02d", iSelected + 1);
	RefreshSN();
}

//重绘窗体
static void ReCreateWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmSerialnumInit, FrmSerialnumExit,
		FrmSerialnumPaint, FrmSerialnumLoop,
		FrmSerialnumPause, FrmSerialnumResume,
		NULL);
}

//机器编号输入回调函数
static void MachineNumberInputOk(void)
{
	char buff[512] = { 0 };
	GetIMEInputBuff(buff);

	if ((buff[0] == '0') && (buff[1] == '0') && (buff[2] == '0'))
	{
		buff[0] = '0';
		buff[1] = '0';
		buff[2] = '1';
	}

	int num = atoi(buff);
	pNumber = (char*)malloc(24);

	if (strlen(buff) == 0)
	{
		num = 1;
	}

	sprintf(pNumber, "%03d", num);
}

//检测下标值
static int checkIndex(char* arg, char** dest, int size)
{
	int i = 0;
	for (i = 0; i < size; i++)
	{
		if (strcmp(arg, *(dest + i)) == 0)
			return i;
	}

	return -1;
}

//刷新序列号信息
static void RefreshSN()
{
	unsigned int uiOldColor;
	GUIPEN *pPen = NULL;
	pPen = GetCurrPen();

	uiOldColor = pPen->uiPenColor;
	SetPenColor(0xffffffff, pPen);
	DrawBlock(200, 310, 300, 330);
	SetPenColor(uiOldColor, pPen);

	sprintf(sn, "%s%s%s%s%s%s", stSn.MachineName,
		stSn.Year,
		stSn.Month,
		stSn.Date,
		stSn.Number,
		stSn.MachineType);

	LOG(LOG_INFO, "sn : %s\n", sn);

	if (NULL != pSerialnumStrSNvalue)
	{
		GuiMemFree(pSerialnumStrSNvalue);
		pSerialnumStrSNvalue = NULL;
	}

	pSerialnumStrSNvalue = TransString(sn);
	SetLabelText(pSerialnumStrSNvalue, pSerialnumLblSNvalue);
	DisplayLabel(pSerialnumLblSNvalue);
	DisplayLabel(pSerialnumLblSNtitle);

	RefreshScreen(__FILE__, __func__, __LINE__);
}

//创建序列号界面
void CreateSerialNumWindow()
{
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmSerialnumInit, FrmSerialnumExit,
		FrmSerialnumPaint, FrmSerialnumLoop,
		FrmSerialnumPause, FrmSerialnumResume,
		NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());		//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);
}