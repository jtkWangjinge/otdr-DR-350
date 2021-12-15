/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmrj45.c
* 摘    要：  主窗体wnd_frmrj45的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmrj45.h"

#include "app_parameter.h"

#include "guiwindow.h"
#include "guipicture.h"
#include "guilabel.h"

#include "wnd_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmcablerange.h"
#include "wnd_frmcablesequence.h"
#include "wnd_frmcablesearch.h"

#define CABLE_NUM			2

/*******************************************************************************
*					窗体frmrj45中的窗体控件
*******************************************************************************/
static GUIWINDOW* pFrmRJ45 = NULL;

//背景图片
static GUIPICTURE *pFrmRJ45Bg = NULL;							//背景
static GUIPICTURE* pFrmRJ45BgCable[CABLE_NUM] = { NULL };		//网线测距、网线测序、网线寻线
//文本标签
static GUILABEL* pFrmRJ45LblCable[CABLE_NUM] = { NULL };
//文本资源
static GUICHAR* pFrmRJ45StrCable[CABLE_NUM] = { NULL };

static char* pBmpfilePath_normal[CABLE_NUM] =
{
	BmpFileDirectory"bg_rj45_cablesequence_up.bmp",
	// BmpFileDirectory"bg_rj45_cablerange_up.bmp",
	BmpFileDirectory"bg_rj45_cablesearch_up.bmp"
};

static char* pBmpfilePath_press[CABLE_NUM] =
{
	"bg_rj45_cablesequence_down.bmp",
	// "bg_rj45_cablerange_down.bmp",
	"bg_rj45_cablesearch_down.bmp"
};

static char* pBmpfilePath_select[CABLE_NUM] =
{
	BmpFileDirectory"bg_rj45_cablesequence_select.bmp",
	// BmpFileDirectory"bg_rj45_cablerange_select.bmp",
	BmpFileDirectory"bg_rj45_cablesearch_select.bmp"
};

//引用保存到eeprom的变量
extern PFactoryConfig pFactoryConf;
//默认选中项
static int iDefaultOption = 0;
/*******************************************************************************
*                   窗体frmrj45内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmRJ45TextRes_Init(void *pInArg, int iInLen,
				void *pOutArg, int iOutLen);
//释放文本资源
static int FrmRJ45TextRes_Exit(void *pInArg, int iInLen,
				void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体frmrj45内部按键响应函数声明
*******************************************************************************/
//点击按钮响应函数
static int RJ45Btn_Down(void *pInArg, int iInLen, 
				void *pOutArg, int iOutLen);
static int RJ45Btn_Up(void *pInArg, int iInLen, 
				void *pOutArg, int iOutLen);

//点击按键响应函数
static int WndRJ45Key_Down(void *pInArg, int iInLen, 
				void *pOutArg, int iOutLen);
static int WndRJ45Key_Up(void *pInArg, int iInLen, 
				void *pOutArg, int iOutLen);

//刷新按钮
static void RefreshBtn(int iOption, int iPressOrSelect);

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmrj45的初始化函数，建立窗体控件、注册消息处理
int FrmRJ45Init(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象 
	pFrmRJ45 = (GUIWINDOW *)pWndObj;
	//初始化文本资源
	FrmRJ45TextRes_Init(NULL, 0, NULL, 0);
	//背景
	pFrmRJ45Bg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							   BmpFileDirectory "bg_rj45.bmp");
	//模块
	int i;
	for (i = 0; i < CABLE_NUM; ++i)
	{
		pFrmRJ45BgCable[i] = CreatePicture(78+235*i, 162, 220, 160, pBmpfilePath_normal[i]);
		pFrmRJ45LblCable[i] = CreateLabel(138 + 235 * i, 284, 100, 16, pFrmRJ45StrCable[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmRJ45LblCable[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmRJ45LblCable[i]);
	}

	/***************************************************************************
	*           注册窗体(因为所有的按键事件都统一由窗体进行处理)
	***************************************************************************/
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmRJ45, pFrmRJ45);
	for (i = 0; i < CABLE_NUM; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmRJ45BgCable[i], pFrmRJ45);
	}
	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	//注册按钮
	for (i = 0; i < CABLE_NUM; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmRJ45BgCable[i], RJ45Btn_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmRJ45BgCable[i], RJ45Btn_Up, NULL, i, pMsg);
	}

	//注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmRJ45, WndRJ45Key_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmRJ45, WndRJ45Key_Up, NULL, 0, pMsg);

	return iReturn;
}
//窗体frmrj45的退出函数，释放所有资源
int FrmRJ45Exit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象
	pFrmRJ45 = (GUIWINDOW *)pWndObj;
	
	/***************************************************************************
	*                       清空消息队列中的消息注册项
	***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	/***************************************************************************
	*                       从当前窗体中注销窗体控件
	***************************************************************************/
	ClearWindowComp(pFrmRJ45);

	//销毁图片以及文本标签
	DestroyPicture(&pFrmRJ45Bg);
	int i;
	for (i = 0; i < CABLE_NUM; ++i)
	{
		DestroyPicture(&pFrmRJ45BgCable[i]);
		DestroyLabel(&pFrmRJ45LblCable[i]);
	}

	//销毁文本资源
	FrmRJ45TextRes_Exit(NULL, 0, NULL, 0);

	return iReturn;
}
//窗体frmrj45的绘制函数，绘制整个窗体
int FrmRJ45Paint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	/* 绘制无效区域窗体 */
	// DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	DisplayPicture(pFrmRJ45Bg);
	int i;
	for (i = 0; i < CABLE_NUM; ++i)
	{
		if (i == iDefaultOption)
		{
			SetPictureBitmap(pBmpfilePath_select[i], pFrmRJ45BgCable[i]);
		}
		DisplayPicture(pFrmRJ45BgCable[i]);
		DisplayLabel(pFrmRJ45LblCable[i]);
	}

	SetPowerEnable(1, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}
//窗体frmrj45的循环函数，进行窗体循环
int FrmRJ45Loop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//禁止并停止窗体循环
	SendWndMsg_LoopDisable(pWndObj);

	return iReturn;
}
//窗体frmrj45的挂起函数，进行窗体挂起前预处理
int FrmRJ45Pause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}
//窗体frmrj45的恢复函数，进行窗体恢复前预处理
int FrmRJ45Resume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/*******************************************************************************
*                   窗体frmrj45内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int FrmRJ45TextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pFrmRJ45StrCable[0] = GetCurrLanguageText(RJ45_LBL_SEQUENCE);
	// pFrmRJ45StrCable[1] = GetCurrLanguageText(RJ45_LBL_RANGE);
	pFrmRJ45StrCable[1] = GetCurrLanguageText(RJ45_LBL_SEARCH);

	return iReturn;
}

//释放文本资源
static int FrmRJ45TextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	int i;
	for (i = 0; i < CABLE_NUM; ++i)
	{
		GuiMemFree(pFrmRJ45StrCable[i]);
	}

	return iReturn;
}

/*******************************************************************************
*                   窗体frmrj45内部按键响应函数声明
*******************************************************************************/
//点击按钮响应函数
static int RJ45Btn_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	RefreshBtn(iOutLen, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

static int RJ45Btn_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	TouchChange(pBmpfilePath_normal[iOutLen], pFrmRJ45BgCable[iOutLen], NULL, pFrmRJ45LblCable[iOutLen], 0);
	RefreshScreen(__FILE__, __func__, __LINE__);

	GUIWINDOW *pWnd = NULL;
#ifdef EEPROM_DATA
	int iFlagArray[3] = { 0x02, 0x04, 0x08 };
#endif
	switch (iOutLen)
	{
	case 0://网线测序
#ifdef EEPROM_DATA
		if (pFactoryConf->enabled & iFlagArray[0])
		{
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
				FrmCableSequenceInit, FrmCableSequenceExit,
				FrmCableSequencePaint, FrmCableSequenceLoop,
				FrmCableSequencePause, FrmCableSequenceResume,
				NULL);			//pWnd由调度线程释放
		}
		else
		{
			iReturn = -1;
		}
#else
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmCableSequenceInit, FrmCableSequenceExit,
			FrmCableSequencePaint, FrmCableSequenceLoop,
			FrmCableSequencePause, FrmCableSequenceResume,
			NULL);			//pWnd由调度线程释放
#endif
		break;
	// case 1://网线测距
// #ifdef EEPROM_DATA
		// if (pFactoryConf->enabled & iFlagArray[1])
		// {
			// pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
				// FrmCableRangeInit, FrmCableRangeExit,
				// FrmCableRangePaint, FrmCableRangeLoop,
				// FrmCableRangePause, FrmCableRangeResume,
				// NULL);			//pWnd由调度线程释放
		// }
		// else
		// {
			// iReturn = -1;
		// }
// #else
		// pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			// FrmCableRangeInit, FrmCableRangeExit,
			// FrmCableRangePaint, FrmCableRangeLoop,
			// FrmCableRangePause, FrmCableRangeResume,
			// NULL); //pWnd由调度线程释放
// #endif
		// break;
	case 1://网线寻线
#ifdef EEPROM_DATA
		if (pFactoryConf->enabled & iFlagArray[2])
		{
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
				FrmCableSearchInit, FrmCableSearchExit,
				FrmCableSearchPaint, FrmCableSearchLoop,
				FrmCableSearchPause, FrmCableSearchResume,
				NULL);			//pWnd由调度线程释放
		}
		else
		{
			iReturn = -1;
		}
#else
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmCableSearchInit, FrmCableSearchExit,
			FrmCableSearchPaint, FrmCableSearchLoop,
			FrmCableSearchPause, FrmCableSearchResume,
			NULL); //pWnd由调度线程释放
#endif
		break;
	default:
		break;
	}
	
	if (iReturn == -1)
	{
		DialogInit(120, 90, TransString(""),
			GetCurrLanguageText(DIALOG_LBL_PURCHASE),
			0, 0, FrmMainReCreateWindow, NULL, NULL);
	}
	else
	{
		if (pWnd)
		{
			SendWndMsg_WindowExit(pFrmRJ45);	//发送消息以便退出当前窗体
			SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
		}
	}

	return iReturn;
}

//点击按键响应函数
static int WndRJ45Key_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

static int WndRJ45Key_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	unsigned int uiValue;
	uiValue = (unsigned int)pInArg;

	switch (uiValue)
	{
	case KEYCODE_LEFT:
		if (iDefaultOption > 0)
		{
			iDefaultOption--;
		}
		RefreshBtn(iDefaultOption, 1);
		RefreshScreen(__FILE__, __func__, __LINE__);
		break;
	case KEYCODE_RIGHT:
		if (iDefaultOption < (CABLE_NUM-1))
		{
			iDefaultOption++;
		}
		RefreshBtn(iDefaultOption, 1);
		RefreshScreen(__FILE__, __func__, __LINE__);
		break;
	case KEYCODE_ESC:
		ReturnMenuOK();
		break;
	case KEYCODE_VFL:
		JumpVflWindow();
		break;
	case KEYCODE_ENTER:
		//RJ45Btn_Down(NULL, 0, NULL, iDefaultOption);
		RJ45Btn_Up(NULL, 0, NULL, iDefaultOption);
		break;
	default:
		break;
	}

	return iReturn;
}

//刷新按钮
static void RefreshBtn(int iOption, int iPressOrSelect)
{
	int i;
	for (i = 0; i < CABLE_NUM; ++i)
	{
		SetPictureBitmap(pBmpfilePath_normal[i], pFrmRJ45BgCable[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmRJ45LblCable[i]);
	}

	if (iPressOrSelect == 0)//默认是按下操作
	{
		SetPictureBitmap(pBmpfilePath_press[iOption], pFrmRJ45BgCable[iOption]);
		SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pFrmRJ45LblCable[iOption]);
	}
	else
	{
		SetPictureBitmap(pBmpfilePath_select[iOption], pFrmRJ45BgCable[iOption]);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmRJ45LblCable[iOption]);
	}

	for (i = 0; i < CABLE_NUM; ++i)
	{
		DisplayPicture(pFrmRJ45BgCable[i]);
		DisplayLabel(pFrmRJ45LblCable[i]);
	}
}