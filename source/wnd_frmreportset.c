/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmreportset.c
* 摘    要：  实现主窗体FrmReportSet的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/8
*
*******************************************************************************/

#include "wnd_frmreportset.h"

#include "guiwindow.h"
#include "guidevice.h"
#include "guipicture.h"
#include "guilabel.h"
#include "guibase.h"
#include "guimessage.h"

#include "app_systemsettings.h"
#include "app_frminit.h"

#include "wnd_frmmark.h"
#include "wnd_frmcommonset.h"
#include "wnd_frmotdranalysis.h"
#include "wnd_frmthresholdset.h"
#include "wnd_global.h"
#include "wnd_frmotdr.h"
#include "wnd_frmime.h"
#include "wnd_frmsavedefdir.h"

/*******************************************************************************
**							窗体FrmReportSet中的声明宏定义		                  **
*******************************************************************************/
#define REPORTSET_TITLE_NUM          4
#define REPORTSET_OPTION_NUM	     8

enum option_type
{
	CABLE_ID = 0,
	FIBER_ID,
	POS_A,
	POS_B,
	TASK_ID,
	USER,
	NOTE,
	DEF_PATH
};

struct reportSet_control
{
	GUIPICTURE* pBtnSelected[REPORTSET_OPTION_NUM];
	GUILABEL* pLblTitle[REPORTSET_OPTION_NUM];
	GUICHAR* pStrTitle[REPORTSET_OPTION_NUM];
	GUILABEL* pLblData[REPORTSET_OPTION_NUM];
	GUICHAR *pStrData[REPORTSET_OPTION_NUM];
	GUIPICTURE *pBtnIcon[REPORTSET_OPTION_NUM];
	GUIPICTURE* pBtnEnterIcon[REPORTSET_OPTION_NUM];
};

//图标背景图
char* pStrReportSetIcon_unpress[REPORTSET_OPTION_NUM] =
{
	BmpFileDirectory"btn_cableNum_unpress.bmp",
	BmpFileDirectory"btn_fiberNum_unpress.bmp",
	BmpFileDirectory"btn_posA_unpress.bmp",
	BmpFileDirectory"btn_posB_unpress.bmp",
	BmpFileDirectory"btn_taskNum_unpress.bmp",
	BmpFileDirectory"btn_user_unpress.bmp",
	BmpFileDirectory"btn_note_unpress.bmp",
	BmpFileDirectory"btn_defPath_unpress.bmp"
};

char* pStrReportSetIcon_press[REPORTSET_OPTION_NUM] =
{
	BmpFileDirectory"btn_cableNum_press.bmp",
	BmpFileDirectory"btn_fiberNum_press.bmp",
	BmpFileDirectory"btn_posA_press.bmp",
	BmpFileDirectory"btn_posB_press.bmp",
	BmpFileDirectory"btn_taskNum_press.bmp",
	BmpFileDirectory"btn_user_press.bmp",
	BmpFileDirectory"btn_note_press.bmp",
	BmpFileDirectory"btn_defPath_press.bmp"
};

/***********************************
* 定义wnd_frmreportset.c中的窗体资源
***********************************/
static GUIWINDOW *pFrmReportSet = NULL;

/***********************************
* 定义wnd_frmreportset.c中的标题栏控件资源
***********************************/
static GUIPICTURE *pFrmReportSetBg = NULL;
static GUIPICTURE *pFrmReportSetBgIcon = NULL;
static GUILABEL *pFrmReportSetLblTitle = NULL;
static GUICHAR *pFrmReportSetStrTitle = NULL;

/***********************************
* 定义wnd_frmreportset.c中的功能区控件资源
***********************************/
static GUIPICTURE *pFrmReportSetBtnBack = NULL;
static GUIPICTURE *pFrmReportSetBtnFunction[REPORTSET_TITLE_NUM] = {NULL};//普通设置、阈值设置、报表设置
static GUILABEL *pFrmReportSetLblFunction[REPORTSET_TITLE_NUM] = {NULL};
static GUICHAR *pFrmReportSetStrFunction[REPORTSET_TITLE_NUM] = {NULL};
static GUILABEL *pFrmReportSetLblInfo = NULL;
static GUICHAR *pFrmReportSetStrInfo = NULL;
//
static struct reportSet_control* pReportSetControl = NULL;
extern MarkParam* pMarkParam;
static char *pDefReportSavePath = NULL;						//默认保存路径
static int iSelectedOption = 0;								//默认选中的选项

/***********************************************************
* *             声明wnd_frmreportset.c中的文本资源        **
************************************************************/
//初始化文本
static int FrmReportSetRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//清理文本
static int FrmReportSetRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/***************************************************************
* *             声明wnd_frmreportset.c中的按键响应函数        **
***************************************************************/
//点击返回按钮函数
static int FrmReportSetBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FrmReportSetBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击标题栏功能项响应函数
static int FrmReportSetBtnFunction_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FrmReportSetBtnFunction_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击功能项响应函数
static int FrmReportSetBtnSelected_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FrmReportSetBtnSelected_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击功能图标响应函数
static int FrmReportSetBtnIcon_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FrmReportSetBtnIcon_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击按键响应函数
static int WndReportSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndReportSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
/***************************************************************
* *             声明wnd_frmreportset.c中的内部处理函数        **
***************************************************************/
//初始化参数
static void InitReportSetPara(void);
//清除参数
static void ClearReportSetPara(void);
//刷新各个选项
static void FlushReportSetOption(void);
/***************************************************************
* *             wnd_frmreportset.c中的回调函数        **
***************************************************************/
static void RecreateReportSetWindow(GUIWINDOW** ppWnd);
//输入光缆ID回调函数
static void InputCableIDBackFunc(void);
//输入光线编号回调函数
static void InputFiberIDBackFunc(void);
//输入位置a回调函数
static void InputPosABackFunc(void);
//输入位置b回调函数
static void InputPosBBackFunc(void);
//输入任务ID回调函数
static void InputTaskIDBackFunc(void);
//输入用户回调函数
static void InputUserBackFunc(void);
//输入注释回调函数
static void InputNoteBackFunc(void);

/***
  * 功能：
		窗体frmreportset的初始化函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		建立窗体控件、注册消息处理
***/
int FrmReportSetInit(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	InitReportSetPara();
	//初始化文本资源
	FrmReportSetRes_Init(NULL, 0, NULL, 0);

	pFrmReportSetBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
									BmpFileDirectory"bg_otdr_report_set.bmp");
	pFrmReportSetBgIcon = CreatePicture(0, 0, 23, 20,
										BmpFileDirectory"bg_reportSet_icon.bmp"); //待定
	pFrmReportSetLblTitle = CreateLabel(23, 2, 100, 16,
										pFrmReportSetStrTitle);
	pFrmReportSetBtnBack = CreatePicture(0, 20, 36, 21,
										 BmpFileDirectory"btn_otdrSet_back.bmp");

	pFrmReportSetLblInfo = CreateLabel(8, 53, 200, 16, pFrmReportSetStrInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmReportSetLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmReportSetLblInfo);

	int i;
	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		if (i < REPORTSET_TITLE_NUM)
		{
			pFrmReportSetBtnFunction[i] = CreatePicture(232 + 102 * i, 47, 99, 35,
														pStrOtdrSetButton_unpress[i]);
			pFrmReportSetLblFunction[i] = CreateLabel(232 + 102 * i + 5, 47 + 9, 90, 16,
													  pFrmReportSetStrFunction[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmReportSetLblFunction[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmReportSetLblFunction[i]);
		}

		pReportSetControl->pBtnIcon[i] = CreatePicture(45, 95 + i * 45, 21, 20,
													   pStrReportSetIcon_unpress[i]);
		pReportSetControl->pBtnSelected[i] = CreatePicture(1, 83 + i * 45, 637, 44,
														   BmpFileDirectory "bg_sys_unselect.bmp");
		pReportSetControl->pLblTitle[i] = CreateLabel(75, 97 + i * 45, 150, 16,
													  pReportSetControl->pStrTitle[i]);
		pReportSetControl->pLblData[i] = CreateLabel(450, 97 + i * 45, 130, 16,
													 pReportSetControl->pStrData[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pReportSetControl->pLblTitle[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pReportSetControl->pLblData[i]);

		if (i == (REPORTSET_OPTION_NUM - 1))
			pReportSetControl->pBtnEnterIcon[i] = CreatePicture(580, 97 + i * 45, 22, 16,
																BmpFileDirectory "bg_sys_arrow.bmp");
		else
			pReportSetControl->pBtnEnterIcon[i] = CreatePicture(580, 97 + i * 45, 22, 16,
																BmpFileDirectory "bg_sys_keyboard.bmp");
	}

	//得到当前窗体对象
	pFrmReportSet = (GUIWINDOW *)pWndObj;
	// 注册窗体控件，只对那些需要接收事件的控件进行
	// 即如果该控件没有或者不需要响应输入事件，可以无需注册
	// 注册窗体控件必须在注册消息处理函数之前进行

	//注册窗体(因为所有的按键事件都统一由窗体进行处理) 
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmReportSet, pFrmReportSet);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmReportSetBtnBack, pFrmReportSet);
	
	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		if (i < REPORTSET_TITLE_NUM)
		{
			AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
						  pFrmReportSetBtnFunction[i], pFrmReportSet);
			AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
						  pFrmReportSetLblFunction[i], pFrmReportSet);
		}

		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
		pReportSetControl->pBtnSelected[i], pFrmReportSet);

		if (i < (REPORTSET_OPTION_NUM-1))
		{
			AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
						  pReportSetControl->pBtnEnterIcon[i], pFrmReportSet);
		}

		if (i == (REPORTSET_OPTION_NUM-1))
		{
			AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL),
						  pReportSetControl->pLblData[i], pFrmReportSet);
		}
	}

	GUIMESSAGE* pMsg = GetCurrMessage();

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmReportSetBtnBack,
					FrmReportSetBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmReportSetBtnBack,
					FrmReportSetBtnBack_Up, NULL, 0, pMsg);

	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		if (i < REPORTSET_TITLE_NUM)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmReportSetBtnFunction[i],
							FrmReportSetBtnFunction_Down, NULL, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmReportSetBtnFunction[i],
							FrmReportSetBtnFunction_Up, NULL, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmReportSetLblFunction[i],
							FrmReportSetBtnFunction_Down, NULL, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmReportSetLblFunction[i],
							FrmReportSetBtnFunction_Up, NULL, i, pMsg);
		}

		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pReportSetControl->pBtnSelected[i],
						FrmReportSetBtnSelected_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pReportSetControl->pBtnSelected[i],
						FrmReportSetBtnSelected_Up, NULL, i, pMsg);

		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pReportSetControl->pBtnEnterIcon[i],
						FrmReportSetBtnIcon_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pReportSetControl->pBtnEnterIcon[i],
						FrmReportSetBtnIcon_Up, NULL, i, pMsg);

		if (i == (REPORTSET_OPTION_NUM-1))
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pReportSetControl->pLblData[i],
							FrmReportSetBtnIcon_Down, NULL, i, pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pReportSetControl->pLblData[i],
							FrmReportSetBtnIcon_Up, NULL, i, pMsg);
		}
	}

	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmReportSet,
					WndReportSetKey_Down, pReportSetControl, 0, GetCurrMessage());
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmReportSet,
					WndReportSetKey_Up, pReportSetControl, 0, GetCurrMessage());

	return iRet;
}

/***
  * 功能：
		窗体frmreportset的退出函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		释放所有资源
***/
int FrmReportSetExit(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	//得到当前窗体对象
	pFrmReportSet = (GUIWINDOW *)pWndObj;
	/***************************************************************************
	**							  清空消息队列中的消息注册项     			  **
	***************************************************************************/
	GUIMESSAGE* pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);

	ClearWindowComp(pFrmReportSet);

	DestroyPicture(&pFrmReportSetBg);
	DestroyPicture(&pFrmReportSetBgIcon);
	DestroyLabel(&pFrmReportSetLblTitle);
	DestroyLabel(&pFrmReportSetLblInfo);

	DestroyPicture(&pFrmReportSetBtnBack);
	int i;
	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		if (i < REPORTSET_TITLE_NUM)
		{
			DestroyPicture(&pFrmReportSetBtnFunction[i]);
			DestroyLabel(&pFrmReportSetLblFunction[i]);
		}

		DestroyPicture(&(pReportSetControl->pBtnSelected[i]));
		DestroyLabel(&(pReportSetControl->pLblTitle[i]));
		DestroyLabel(&(pReportSetControl->pLblData[i]));
		DestroyPicture(&(pReportSetControl->pBtnIcon[i]));
		DestroyPicture(&(pReportSetControl->pBtnEnterIcon[i]));
	}

	FrmReportSetRes_Exit(NULL, 0, NULL, 0);
	ClearReportSetPara();

	return iRet;
}

/***
  * 功能：
		窗体frmreportset的绘制函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetPaint(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	DisplayPicture(pFrmReportSetBg);
	// DisplayPicture(pFrmReportSetBgIcon);
	// DisplayLabel(pFrmReportSetLblTitle);
	DisplayLabel(pFrmReportSetLblInfo);

	// DisplayPicture(pFrmReportSetBtnBack);
	int i;
	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		if (i < REPORTSET_TITLE_NUM)
		{
			if (i == REPORT_SET)
			{
				SetPictureBitmap(pStrOtdrSetButton_press[REPORT_SET], pFrmReportSetBtnFunction[i]);
			}
			DisplayPicture(pFrmReportSetBtnFunction[i]);
			DisplayLabel(pFrmReportSetLblFunction[i]);
		}

		if (i == iSelectedOption)
		{
			SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pReportSetControl->pBtnSelected[i]);
			SetPictureBitmap(pStrReportSetIcon_press[i], pReportSetControl->pBtnIcon[i]);
			if (i == (REPORTSET_OPTION_NUM - 1))
				SetPictureBitmap(BmpFileDirectory "bg_sys_arrow_press.bmp", pReportSetControl->pBtnEnterIcon[i]);
			else
				SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp", pReportSetControl->pBtnEnterIcon[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pReportSetControl->pBtnSelected[i]);
			SetPictureBitmap(pStrReportSetIcon_unpress[i], pReportSetControl->pBtnIcon[i]);
			if (i == (REPORTSET_OPTION_NUM - 1))
				SetPictureBitmap(BmpFileDirectory "bg_sys_arrow.bmp", pReportSetControl->pBtnEnterIcon[i]);
			else
				SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp", pReportSetControl->pBtnEnterIcon[i]);
		}
		DisplayPicture(pReportSetControl->pBtnSelected[i]);
		DisplayLabel(pReportSetControl->pLblTitle[i]);
		DisplayLabel(pReportSetControl->pLblData[i]);
		DisplayPicture(pReportSetControl->pBtnIcon[i]);
		DisplayPicture(pReportSetControl->pBtnEnterIcon[i]);
	}

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iRet;
}

/***
  * 功能：
		窗体frmreportset的循环函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetLoop(void *pWndObj)
{
	int iRet = 0;

	return iRet;
}

/***
  * 功能：
		窗体frmreportset的挂起函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetPause(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

/***
  * 功能：
		窗体frmreportset的恢复函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetResume(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

/***********************************************************
* *             wnd_frmreportset.c中的文本资源        **
************************************************************/
//初始化文本
static int FrmReportSetRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	pFrmReportSetStrTitle = GetCurrLanguageText(OTDR_SET_LBL_REPORT);
	pFrmReportSetStrInfo = GetCurrLanguageText(OTDR_SET_LBL_REPORT_SETTING);

	pFrmReportSetStrFunction[TEST_SET] = GetCurrLanguageText(OTDR_SET_LBL_TEST);
	pFrmReportSetStrFunction[OTDR_ANALYSIS_SET] = GetCurrLanguageText(OTDR_SET_LBL_ANALYSIS);
	pFrmReportSetStrFunction[THRESHOLD_SET] = GetCurrLanguageText(OTDR_SET_LBL_THRESHOLD);
	pFrmReportSetStrFunction[REPORT_SET] = GetCurrLanguageText(OTDR_SET_LBL_REPORT);

	unsigned int pStrTitle[REPORTSET_OPTION_NUM] = 
	{
		OTDR_SET_LBL_NAME_CABLE,
		OTDR_SET_LBL_NAME_FIBER,
		OTDR_SET_LBL_LOCATION_A,
		OTDR_SET_LBL_LOCATION_B,
		OTDR_SET_LBL_NAME_TEST,
		OTDR_SET_LBL_USER,
		OTDR_SET_LBL_NOTE,
		OTDR_SET_LBL_DEFAULT_PATH
	};

	char* pStrData[REPORTSET_OPTION_NUM] =
	{
		pMarkParam->CableMark,
		pMarkParam->FiberMark,
		pMarkParam->Location_A,
		pMarkParam->Location_B,
		pMarkParam->TaskID_Unicode,
		pMarkParam->Customer,
		pMarkParam->Note,
		pDefReportSavePath + strlen(MntUpDirectory)
	};

	int i;

	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		pReportSetControl->pStrTitle[i] = GetCurrLanguageText(pStrTitle[i]);
		pReportSetControl->pStrData[i] = TransString(pStrData[i]);
	}

	return 0;
}

//清理文本
static int FrmReportSetRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	GuiMemFree(pFrmReportSetStrTitle);
	GuiMemFree(pFrmReportSetStrInfo);

	int i;
	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		if (i < REPORTSET_TITLE_NUM)
		{
			GuiMemFree(pFrmReportSetStrFunction[i]);
		}

		GuiMemFree(pReportSetControl->pStrTitle[i]);
		GuiMemFree(pReportSetControl->pStrData[i]);
	}

	return 0;
}

/***************************************************************
* *             声明wnd_frmreportset.c中的按键响应函数        **
***************************************************************/
//点击返回按钮函数
static int FrmReportSetBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}
static int FrmReportSetBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmOtdrInit, FrmOtdrExit,
		FrmOtdrPaint, FrmOtdrLoop,
		FrmOtdrPause, FrmOtdrResume,
		NULL);
	SendWndMsg_WindowExit(pFrmReportSet);
	SendSysMsg_ThreadCreate(pWnd);

	return 0;
}

//点击标题栏功能项响应函数
static int FrmReportSetBtnFunction_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

static int FrmReportSetBtnFunction_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
	switch (iOutLen)
	{
	case OTDR_ANALYSIS_SET:
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmOtdrAnalysisSetInit, FrmOtdrAnalysisSetExit,
			FrmOtdrAnalysisSetPaint, FrmOtdrAnalysisSetLoop,
			FrmOtdrAnalysisSetPause, FrmOtdrAnalysisSetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmReportSet);
		SendSysMsg_ThreadCreate(pWnd);
	}
		break;
	case TEST_SET:
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmCommonSetInit, FrmCommonSetExit,
			FrmCommonSetPaint, FrmCommonSetLoop,
			FrmCommonSetPause, FrmCommonSetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmReportSet);
		SendSysMsg_ThreadCreate(pWnd);
	}
		break;
	case THRESHOLD_SET:
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmThresholdSetInit, FrmThresholdSetExit,
			FrmThresholdSetPaint, FrmThresholdSetLoop,
			FrmThresholdSetPause, FrmThresholdSetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmReportSet);
		SendSysMsg_ThreadCreate(pWnd);
	}
		break;
	default:
		break;
	}
	return 0;
}

//点击功能项响应函数
static int FrmReportSetBtnSelected_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

static int FrmReportSetBtnSelected_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

//点击功能图标响应函数
static int FrmReportSetBtnIcon_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

static int FrmReportSetBtnIcon_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	char buff[TEMP_ARRAY_SIZE] = { 0 };

	switch (iOutLen)
	{
	case CABLE_ID:
		memset(buff, 0, TEMP_ARRAY_SIZE);
		sprintf(buff, "%s", pMarkParam->CableMark);
		IMEInit(buff, 17, 0, RecreateReportSetWindow, InputCableIDBackFunc, NULL);
		break;
	case FIBER_ID:
		memset(buff, 0, TEMP_ARRAY_SIZE);
		sprintf(buff, "%s", pMarkParam->FiberMark);
		IMEInit(buff, 17, 0, RecreateReportSetWindow, InputFiberIDBackFunc, NULL);
		break;
	case POS_A:
		memset(buff, 0, TEMP_ARRAY_SIZE);
		sprintf(buff, "%s", pMarkParam->Location_A);
		IMEInit(buff, 17, 0, RecreateReportSetWindow, InputPosABackFunc, NULL);
		break;
	case POS_B:
		memset(buff, 0, TEMP_ARRAY_SIZE);
		sprintf(buff, "%s", pMarkParam->Location_B);
		IMEInit(buff, 17, 0, RecreateReportSetWindow, InputPosBBackFunc, NULL);
		break;
	case TASK_ID:
		memset(buff, 0, TEMP_ARRAY_SIZE);
		sprintf(buff, "%s", pMarkParam->TaskID_Unicode);
		IMEInit(buff, 17, 0, RecreateReportSetWindow, InputTaskIDBackFunc, NULL);
		break;
	case USER:
		memset(buff, 0, TEMP_ARRAY_SIZE);
		sprintf(buff, "%s", pMarkParam->Customer);
		IMEInit(buff, 17, 0, RecreateReportSetWindow, InputUserBackFunc, NULL);
		break;
	case NOTE:
		memset(buff, 0, TEMP_ARRAY_SIZE);
		sprintf(buff, "%s", pMarkParam->Note);
		IMEInit(buff, 100, 0, RecreateReportSetWindow, InputNoteBackFunc, NULL);
		break;
	case DEF_PATH:
	{
		Stack *ps = NULL;
		ps = GetCurrWndStack();
		WndPush(ps, RecreateReportSetWindow);
		CreateUserDefinedPath(pDefReportSavePath, MntDataDirectory);
	}
		break;
	default:
		break;
	}

	return 0;
}
//点击按键响应函数
static int WndReportSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{

	return 0;
}

static int WndReportSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	unsigned int uiValue;
	uiValue = (unsigned int)pInArg;

	switch (uiValue)
	{
	case KEYCODE_OTDR_LEFT:
		FrmReportSetBtnFunction_Up(NULL, 0, NULL, THRESHOLD_SET);
		break;
	case KEYCODE_UP:
		if (iSelectedOption != 0)
		{
			iSelectedOption--;
		}
		FlushReportSetOption();
		break;
	case KEYCODE_DOWN:
		if (iSelectedOption != (REPORTSET_OPTION_NUM - 1))
		{
			iSelectedOption++;
		}
		FlushReportSetOption();
		break;
	case KEYCODE_ENTER:
		FrmReportSetBtnIcon_Up(NULL, 0, NULL, iSelectedOption);
		break;
	case KEYCODE_START:
		//先退出当前界面
		FrmReportSetBtnBack_Up(NULL, 0, NULL, 0);
		//调用测试otdr接口
		OtdrTest(0);
		break;
	case KEYCODE_ESC:
		FrmReportSetBtnBack_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}

	return 0;
}

/***************************************************************
* *             wnd_frmreportset.c中的内部处理函数        **
***************************************************************/
//初始化参数
static void InitReportSetPara(void)
{
	pReportSetControl = (struct reportSet_control*)calloc(1, sizeof(struct reportSet_control));
	if (pReportSetControl)
	{
		memset(pReportSetControl, 0, sizeof(struct reportSet_control));
	}

	pDefReportSavePath = pOtdrTopSettings->sDefSavePath.cOtdrReportSavePath;
}

//清除参数（保存系统参数）
static void ClearReportSetPara(void)
{
	if (pReportSetControl)
	{
		GuiMemFree(pReportSetControl);
	}

	memset(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, 0, sizeof(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath));
	strcpy(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, pDefReportSavePath);
#ifdef EEPROM_DATA
	SetSettingsData((void*)pMarkParam, sizeof(MarkParam), OTDR_MARK_SET);
	SaveSettings(OTDR_MARK_SET);
#endif
}

//刷新各个选项
static void FlushReportSetOption(void)
{
	int i;
	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp", pReportSetControl->pBtnSelected[i]);
		SetPictureBitmap(pStrReportSetIcon_unpress[i], pReportSetControl->pBtnIcon[i]);
		if (i == (REPORTSET_OPTION_NUM - 1))
			SetPictureBitmap(BmpFileDirectory "bg_sys_arrow.bmp", pReportSetControl->pBtnEnterIcon[i]);
		else
			SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp", pReportSetControl->pBtnEnterIcon[i]);
	}

	SetPictureBitmap(pStrReportSetIcon_press[iSelectedOption], pReportSetControl->pBtnIcon[iSelectedOption]);
	SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pReportSetControl->pBtnSelected[iSelectedOption]);
	if (iSelectedOption == (REPORTSET_OPTION_NUM - 1))
		SetPictureBitmap(BmpFileDirectory "bg_sys_arrow_press.bmp", pReportSetControl->pBtnEnterIcon[iSelectedOption]);
	else
		SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp", pReportSetControl->pBtnEnterIcon[iSelectedOption]);

	for (i = 0; i < REPORTSET_OPTION_NUM; ++i)
	{
		DisplayPicture(pReportSetControl->pBtnSelected[i]);
		DisplayLabel(pReportSetControl->pLblTitle[i]);
		DisplayLabel(pReportSetControl->pLblData[i]);
		DisplayPicture(pReportSetControl->pBtnIcon[i]);
		DisplayPicture(pReportSetControl->pBtnEnterIcon[i]);
	}

	RefreshScreen(__FILE__, __func__, __LINE__);
}

/***************************************************************
* *             wnd_frmreportset.c中的回调函数        **
***************************************************************/
static void RecreateReportSetWindow(GUIWINDOW** ppWnd)
{
	*ppWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmReportSetInit, FrmReportSetExit,
		FrmReportSetPaint, FrmReportSetLoop,
		FrmReportSetPause, FrmReportSetResume,
		NULL);
}

//输入光缆ID的回调函数
void InputCableIDBackFunc(void)
{
	memset(pMarkParam->CableMark, 0, sizeof(pMarkParam->CableMark));
	GetIMEInputBuff(pMarkParam->CableMark);
}

//输入光纤编号的回调函数
void InputFiberIDBackFunc(void)
{
	memset(pMarkParam->FiberMark, 0, sizeof(pMarkParam->FiberMark));
	GetIMEInputBuff(pMarkParam->FiberMark);
}

//输入位置A的回调函数
void InputPosABackFunc(void)
{
	memset(pMarkParam->Location_A, 0, sizeof(pMarkParam->Location_A));
	GetIMEInputBuff(pMarkParam->Location_A);
}

//输入位置B的回调函数
void InputPosBBackFunc(void)
{
	memset(pMarkParam->Location_B, 0, sizeof(pMarkParam->Location_B));
	GetIMEInputBuff(pMarkParam->Location_B);
}

//输入任务ID的回调函数
void InputTaskIDBackFunc(void)
{
	memset(pMarkParam->TaskID_Unicode, 0, sizeof(pMarkParam->TaskID_Unicode));
	GetIMEInputBuff(pMarkParam->TaskID_Unicode);
}
//输入用户的回调函数
void InputUserBackFunc(void)
{
	memset(pMarkParam->Customer, 0, sizeof(pMarkParam->Customer));
	GetIMEInputBuff(pMarkParam->Customer);
}

//输入注释的回调函数
void InputNoteBackFunc(void)
{
	memset(pMarkParam->Note, 0, sizeof(pMarkParam->Note));
	GetIMEInputBuff(pMarkParam->Note);
}