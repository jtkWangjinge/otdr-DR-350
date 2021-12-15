/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmaintancedate.cpp
* 摘    要：  实现主窗体frmmaintancedate的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  v1.0.0
* 原 作 者：  
* 完成日期：  
*******************************************************************************/
#include "wnd_frmmaintancedate.h"

#include "app_parameter.h"
#include "app_systemsettings.h"

#include "wnd_global.h"
#include "wnd_frmselector.h"
#include "wnd_frmauthorization.h"
#include "wnd_frmserialnum.h"
#include "wnd_frmsystemsetting.h"

//定义年月日的最大值
#define MAX_YEAR_NUMBER     67
#define MAX_MONTH_NUMBER    12
#define MAX_DAY_NUMBER      31
/***************************************
*声明wnd_frmselector.c中内部使用的变量
****************************************/
static SELECTOR *pCurrSelector = NULL;  //当前的选择器
extern PFactoryConfig pFactoryConf;     //引用保存到eeprom的变量
/*******************************************************************************
**							声明窗体中相关控件变量							  **
*******************************************************************************/
static GUIWINDOW *pFrmFactoryMaintanceDate                      = NULL;
//定义桌面背景
static GUIPICTURE *pFactoryMaintanceDateBg                      = NULL;
//定义菜单栏
static GUIPICTURE *pFactoryMaintanceDateBtnBack					= NULL;
static GUIPICTURE *pFactoryMaintanceDateBtnMenu[3]				= { NULL };
static GUILABEL *pFactoryMaintanceDateLblMenu[3]				= { NULL };
static GUICHAR *pFactoryMaintanceDateStrMenu[3]					= { NULL };
//日期
static GUILABEL* pFactoryMaintanceDateLblDate[3]				= { NULL };
static GUICHAR *pFactoryMaintanceDateStrDate[3]					= { NULL };
//定义应用按钮的背景
static GUIPICTURE *pFactoryMaintanceDateBtnApply                = NULL;
//声明标题标签变量
static GUICHAR* pFactoryMaintanceDateStrTitle                   = NULL;
static GUILABEL* pFactoryMaintanceDateLblTitle                  = NULL;
//维护日期
static GUICHAR* pFactoryMaintanceDateStrLastDate                = NULL;
static GUILABEL* pFactoryMaintanceDateLblLastDate               = NULL;
static GUICHAR* pFactoryMaintanceDateStrCurrentDate             = NULL;
static GUILABEL* pFactoryMaintanceDateLblCurrentDate            = NULL;
//定义应用按钮文本
static GUICHAR* pFactoryMaintanceDateStrApply                   = NULL;
static GUILABEL* pFactoryMaintanceDateLblApply                  = NULL;
//定义选择器
static SELECTOR *pFactoryMaintanceDateSelector[6]               = {NULL};
//定义选择器中的文本
static GUICHAR *pFactoryMaintanceDateStrYear[MAX_YEAR_NUMBER] = {NULL};
static GUICHAR *pFactoryMaintanceDateStrMonth[MAX_MONTH_NUMBER] = {NULL};
static GUICHAR *pFactoryMaintanceDateStrDay[MAX_DAY_NUMBER] = {NULL};
//选择器的回调函数
static void FactoryMaintanceDateYearBackFunc (int iSelected);
static void FactoryMaintanceDateMonthBackFunc (int iSelected);
static void FactoryMaintanceDateDayBackFunc (int iSelected);
/*******************************************************************************
*                   窗体frmmaintancedate内部函数声明
*******************************************************************************/
//初始化文本资源
static int FactoryMaintanceDateTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FactoryMaintanceDateTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//返回按钮的点击处理函数
static int FactoryMaintanceDateBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryMaintanceDateBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//菜单栏按钮的点击处理函数
static int FactoryMaintanceDateBtnMenu_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryMaintanceDateBtnMenu_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//选择器左选择按钮点击处理函数
static int SelectorBtnLeft_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int SelectorBtnLeft_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//选择器右选择按钮点击处理函数
static int SelectorBtnMidd_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int SelectorBtnMidd_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//选择器元素点击处理函数
static int SelectorBtnRight_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int SelectorBtnRight_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//应用按钮的点击处理函数
static int FactoryMaintanceDateApply_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryMaintanceDateApply_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//点击按键响应函数
static int WndFactoryMaintanceDateKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndFactoryMaintanceDateKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//计算日期
static int addDate(int iType, int iNum, int iSelected);
static int subDate(int iType, int iNum, int iSelected);
//计算天数
static int addDays(int year, int month, int day);
static int subDays(int year, int month, int day);
//处理选择器递增或递减
static void HandleDateAddOrDec(int index, int isAddOrDec);
/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmmaintancedate的初始化函数，建立窗体控件、注册消息处理
int FrmMaintanceDateInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    //得到当前窗体对象 
    pFrmFactoryMaintanceDate = (GUIWINDOW *) pWndObj;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FactoryMaintanceDateTextRes_Init(NULL, 0, NULL, 0);
    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFactoryMaintanceDateBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factory1.bmp");
	//返回键
	pFactoryMaintanceDateBtnBack = CreatePicture(0, 20, 36, 21, BmpFileDirectory"btn_otdrSet_back.bmp");
    pFactoryMaintanceDateBtnApply = CreatePicture(399, 316, 140, 46, BmpFileDirectory"btn_dialog_unpress.bmp");
	pFactoryMaintanceDateLblTitle = CreateLabel(36, 12, 200, 16, pFactoryMaintanceDateStrTitle);
	//
	pFactoryMaintanceDateLblLastDate = CreateLabel(130, 130, 200, 24, pFactoryMaintanceDateStrLastDate);
	pFactoryMaintanceDateLblCurrentDate = CreateLabel(360, 130, 200, 24, pFactoryMaintanceDateStrCurrentDate);
	pFactoryMaintanceDateLblApply = CreateLabel(399+20, 316+15, 100, 24, pFactoryMaintanceDateStrApply);

	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryMaintanceDateLblLastDate);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryMaintanceDateLblCurrentDate);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryMaintanceDateLblApply);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryMaintanceDateLblLastDate);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryMaintanceDateLblCurrentDate);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryMaintanceDateLblApply);
	int i;
	for (i = 0; i < 3; ++i)
	{
		pFactoryMaintanceDateBtnMenu[i] = CreatePicture(10 + 91 * i, 48, 90, 35, pStrButton_unpress[i]);
		pFactoryMaintanceDateLblMenu[i] = CreateLabel(10 + 91 * i, 48 + 10, 90, 16, pFactoryMaintanceDateStrMenu[i]);
		pFactoryMaintanceDateLblDate[i] = CreateLabel(110, 170+50*i, 50, 16, pFactoryMaintanceDateStrDate[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryMaintanceDateLblDate[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryMaintanceDateLblMenu[i]);
	}
    //上次维护日期
    pFactoryMaintanceDateSelector[0] = CreateSelector(180, 170, 67, pFactoryConf->lastDate.year, pFactoryMaintanceDateStrYear, 
								 FactoryMaintanceDateYearBackFunc, 1);
    pFactoryMaintanceDateSelector[1] = CreateSelector(180, 220, 12, pFactoryConf->lastDate.month, pFactoryMaintanceDateStrMonth, 
								 FactoryMaintanceDateMonthBackFunc, 1);
    pFactoryMaintanceDateSelector[2] = CreateSelector(180, 270, 31, pFactoryConf->lastDate.day, pFactoryMaintanceDateStrDay, 
								 FactoryMaintanceDateDayBackFunc, 1);
    //当前维护日期
    pFactoryMaintanceDateSelector[3] = CreateSelector(415, 170, 67, pFactoryConf->currentDate.year, pFactoryMaintanceDateStrYear, 
								 FactoryMaintanceDateYearBackFunc, 1);
    pFactoryMaintanceDateSelector[4] = CreateSelector(415, 220, 12, pFactoryConf->currentDate.month, pFactoryMaintanceDateStrMonth, 
								 FactoryMaintanceDateMonthBackFunc, 1);
    pFactoryMaintanceDateSelector[5] = CreateSelector(415, 270, 31, pFactoryConf->currentDate.day, pFactoryMaintanceDateStrDay, 
								 FactoryMaintanceDateDayBackFunc, 1);
    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //必须在持有控件队列互斥锁的情况下操作
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactoryMaintanceDate, pFrmFactoryMaintanceDate);
    for(i = 0; i < 6; i++)
    {
		if (i < 3)
		{
			AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryMaintanceDateBtnMenu[i], pFrmFactoryMaintanceDate);
		}

        AddSelectorToComp(pFactoryMaintanceDateSelector[i], pFrmFactoryMaintanceDate);
    }

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryMaintanceDateBtnApply, pFrmFactoryMaintanceDate);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryMaintanceDateBtnBack, pFrmFactoryMaintanceDate);
	//注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	
    for(i = 0; i < 6; i++)
    {
		if (i < 3)
		{
			LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryMaintanceDateBtnMenu[i],
				FactoryMaintanceDateBtnMenu_Down, (void*)(i), sizeof(int), pMsg);
			LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryMaintanceDateBtnMenu[i],
				FactoryMaintanceDateBtnMenu_Up, (void*)(i), sizeof(int), pMsg);
		}

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryMaintanceDateSelector[i]->pSelectorBtnLeft, 
                	SelectorBtnLeft_Down, (void*)(i), sizeof(int), pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryMaintanceDateSelector[i]->pSelectorBtnLeft, 
                	SelectorBtnLeft_Up, (void*)(i), sizeof(int), pMsg);
	
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryMaintanceDateSelector[i]->pSelectorBtnMidd, 
                    SelectorBtnMidd_Down, (void*)(i), sizeof(int), pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryMaintanceDateSelector[i]->pSelectorBtnMidd, 
                    SelectorBtnMidd_Up, (void*)(i), sizeof(int), pMsg);
    	
    	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryMaintanceDateSelector[i]->pSelectorBtnRight, 
                    SelectorBtnRight_Down, (void*)(i), sizeof(int), pMsg);
    	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryMaintanceDateSelector[i]->pSelectorBtnRight, 
                    SelectorBtnRight_Up, (void*)(i), sizeof(int), pMsg);
    }
	//注册返回按钮
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryMaintanceDateBtnBack,
		FactoryMaintanceDateBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryMaintanceDateBtnBack,
		FactoryMaintanceDateBtnBack_Up, NULL, 0, pMsg);
	//注册应用按钮
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryMaintanceDateBtnApply, 
                    FactoryMaintanceDateApply_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryMaintanceDateBtnApply, 
                    FactoryMaintanceDateApply_Up, NULL, 0, pMsg);
	//注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmFactoryMaintanceDate, WndFactoryMaintanceDateKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmFactoryMaintanceDate, WndFactoryMaintanceDateKey_Up, NULL, 0, pMsg);

    return iReturn;
}
//窗体frmmaintancedate的退出函数，释放所有资源
int FrmMaintanceDatePaint(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    //得到当前窗体对象
    pFrmFactoryMaintanceDate = (GUIWINDOW *) pWndObj;
    DisplayPicture(pFactoryMaintanceDateBg);
	// DisplayPicture(pFactoryMaintanceDateBtnBack);
    DisplayPicture(pFactoryMaintanceDateBtnApply);
    DisplayLabel(pFactoryMaintanceDateLblTitle);
    DisplayLabel(pFactoryMaintanceDateLblLastDate);
    DisplayLabel(pFactoryMaintanceDateLblCurrentDate);
    DisplayLabel(pFactoryMaintanceDateLblApply);
    //显示选择器
    int i;
    for(i = 0; i < 6; ++i)
    {
		if (i < 3)
		{
			if (i == 2)
			{
				SetPictureBitmap(pStrButton_press[i], pFactoryMaintanceDateBtnMenu[i]);
			}
			DisplayPicture(pFactoryMaintanceDateBtnMenu[i]);
			DisplayLabel(pFactoryMaintanceDateLblMenu[i]);
			DisplayLabel(pFactoryMaintanceDateLblDate[i]);
		}

        DisplaySelector(pFactoryMaintanceDateSelector[i]);
    }

    /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}
//窗体frmmaintancedate的绘制函数，绘制整个窗体
int FrmMaintanceDateLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    return iReturn;
}
//窗体frmmaintancedate的循环函数，进行窗体循环
int FrmMaintanceDatePause(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    return iReturn;
}
//窗体frmmaintancedate的挂起函数，进行窗体挂起前预处理
int FrmMaintanceDateResume(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    return iReturn;
}
//窗体frmmaintancedate的恢复函数，进行窗体恢复前预处理
int FrmMaintanceDateExit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmFactoryMaintanceDate = (GUIWINDOW *) pWndObj;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFactoryMaintanceDate);

    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFactoryMaintanceDateBg);
	DestroyPicture(&pFactoryMaintanceDateBtnBack);
    DestroyPicture(&pFactoryMaintanceDateBtnApply);
    DestroyLabel(&pFactoryMaintanceDateLblTitle);
    DestroyLabel(&pFactoryMaintanceDateLblLastDate);
    DestroyLabel(&pFactoryMaintanceDateLblCurrentDate);
    DestroyLabel(&pFactoryMaintanceDateLblApply);
    //
    int i;
    for(i = 0; i < 6; i++)
    {
		if (i < 3)
		{
			DestroyPicture(&pFactoryMaintanceDateBtnMenu[i]);
			DestroyLabel(&pFactoryMaintanceDateLblMenu[i]);
			DestroyLabel(&pFactoryMaintanceDateLblDate[i]);
		}

        DestroySelector(&(pFactoryMaintanceDateSelector[i]));
    }
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
    SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
    SaveSettings(FACTORY_CONFIG);
    //释放文本资源
    FactoryMaintanceDateTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}
/*******************************************************************************
*                   窗体frmmaintancedate内部函数声明
*******************************************************************************/
//初始化文本资源
static int FactoryMaintanceDateTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    /***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	pFactoryMaintanceDateStrTitle = TransString("LeaseDate");
    pFactoryMaintanceDateStrLastDate = TransString("START DATE");
    pFactoryMaintanceDateStrCurrentDate = TransString("END DATE");
    pFactoryMaintanceDateStrApply = TransString("APPLY");
    int i;

    for(i = 0; i < MAX_YEAR_NUMBER; ++i)
    {
        char buf[30] = {0};
        sprintf(buf, "%.4d", i+1970);
        pFactoryMaintanceDateStrYear[i] = TransString(buf);
    }

    for(i = 0; i < MAX_MONTH_NUMBER; ++i)
    {
        char buf[10] = {0};
        sprintf(buf, "%.2d", i+1);
        pFactoryMaintanceDateStrMonth[i] = TransString(buf);
    }

    for(i = 0; i < MAX_DAY_NUMBER; ++i)
    {
        char buf[10] = {0};
        sprintf(buf, "%.2d", i+1);
        pFactoryMaintanceDateStrDay[i] = TransString(buf);
	}
	//菜单栏
	pFactoryMaintanceDateStrMenu[0] = TransString("Authorization");
	pFactoryMaintanceDateStrMenu[1] = TransString("SerialNum");
	pFactoryMaintanceDateStrMenu[2] = TransString("LeaseDate");

	pFactoryMaintanceDateStrDate[0] = TransString("Year:");
	pFactoryMaintanceDateStrDate[1] = TransString("Month:");
	pFactoryMaintanceDateStrDate[2] = TransString("Day:");

    return iReturn;
}
//释放文本资源
static int FactoryMaintanceDateTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    GuiMemFree(pFactoryMaintanceDateStrTitle);
    GuiMemFree(pFactoryMaintanceDateStrLastDate);
    GuiMemFree(pFactoryMaintanceDateStrCurrentDate);
    GuiMemFree(pFactoryMaintanceDateStrApply);
    int i;

    for(i = 0; i < MAX_YEAR_NUMBER; ++i)
    {
        GuiMemFree(pFactoryMaintanceDateStrYear[i]);
    }

    for(i = 0; i < MAX_MONTH_NUMBER; ++i)
    {
        GuiMemFree(pFactoryMaintanceDateStrMonth[i]);
    }

    for(i = 0; i < MAX_DAY_NUMBER; ++i)
    {
        GuiMemFree(pFactoryMaintanceDateStrDay[i]);
    }

	for (i = 0; i < 3; ++i)
	{
		GuiMemFree(pFactoryMaintanceDateStrMenu[i]);
		GuiMemFree(pFactoryMaintanceDateStrDate[i]);
	}

    return iReturn;
}

//返回按钮的点击处理函数
static int FactoryMaintanceDateBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

static int FactoryMaintanceDateBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmSystemSettingInit, FrmSystemSettingExit,
		FrmSystemSettingPaint, FrmSystemSettingLoop,
		FrmSystemSettingPause, FrmSystemSettingResume,
		NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmFactoryMaintanceDate);		//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);

	return 0;
}

//菜单栏按钮的点击处理函数
static int FactoryMaintanceDateBtnMenu_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

static int FactoryMaintanceDateBtnMenu_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	int option = (int)iOutLen;
	GUIWINDOW *pWnd = NULL;

	switch (option)
	{
	case 0://authorization
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmAuthorizationInit, FrmAuthorizationExit,
			FrmAuthorizationPaint, FrmAuthorizationLoop,
			FrmAuthorizationPause, FrmAuthorizationResume,
			NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFactoryMaintanceDate);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case 1://serialnum
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmSerialnumInit, FrmSerialnumExit,
			FrmSerialnumPaint, FrmSerialnumLoop,
			FrmSerialnumPause, FrmSerialnumResume,
			NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmFactoryMaintanceDate);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
	default:
		break;
	}

	return 0;
}

//应用按钮按下处理
static int FactoryMaintanceDateApply_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    TouchChange("btn_dialog_press.bmp", pFactoryMaintanceDateBtnApply, 
				pFactoryMaintanceDateStrApply, pFactoryMaintanceDateLblApply, 1);
    pFactoryConf->lastDate.year = pFactoryMaintanceDateSelector[0]->uiCurrSelected;
    pFactoryConf->lastDate.month = pFactoryMaintanceDateSelector[1]->uiCurrSelected;
    pFactoryConf->lastDate.day = pFactoryMaintanceDateSelector[2]->uiCurrSelected;
    pFactoryConf->currentDate.year = pFactoryMaintanceDateSelector[3]->uiCurrSelected;
    pFactoryConf->currentDate.month = pFactoryMaintanceDateSelector[4]->uiCurrSelected;
    pFactoryConf->currentDate.day = pFactoryMaintanceDateSelector[5]->uiCurrSelected;
    RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}
static int FactoryMaintanceDateApply_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    TouchChange("btn_dialog_unpress.bmp", pFactoryMaintanceDateBtnApply, 
				pFactoryMaintanceDateStrApply, pFactoryMaintanceDateLblApply, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//点击按键响应函数
static int WndFactoryMaintanceDateKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

static int WndFactoryMaintanceDateKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
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
		FactoryMaintanceDateBtnMenu_Up(NULL, 0, NULL, 1); //serialnum
		break;
	case KEYCODE_UP:
		if (iUpDownSelected > 0)
		{
			iUpDownSelected--;
		}
		break;
	case KEYCODE_DOWN:
		if (iUpDownSelected < 7)
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
	case KEYCODE_ESC:
		FactoryMaintanceDateBtnBack_Up(NULL, 0, NULL, 0);
		break;
	case KEYCODE_ENTER:
		if (iUpDownSelected == 6) //apply
		{
			FactoryMaintanceDateApply_Down(NULL, 0, NULL, 0);
			FactoryMaintanceDateApply_Up(NULL, 0, NULL, 0);
		}
		else
		{
			HandleDateAddOrDec(iUpDownSelected, iLeftRightSelected);
		}
		break;
	default:
		break;
	}

	return iReturn;
}


//选择器左选择按钮点击处理函数
static int SelectorBtnLeft_Down(void *pInArg,   int iInLen, 
                                void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    int i = (int)pOutArg;
    SELECTOR *pSelector = pFactoryMaintanceDateSelector[i];
	
	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{
		SetPictureBitmap(SEC_DN_PRESS, pSelector->pSelectorBtnLeft);
		DisplayPicture(pSelector->pSelectorBtnLeft);
        pSelector->uiCurrSelected = subDate(pSelector->uiItemCnt, pSelector->uiCurrSelected, i);
        SetLabelText(pSelector->pItemString[pSelector->uiCurrSelected], 
        pSelector->pSelectorLblItem);
        DisplayPicture(pSelector->pSelectorBtnMidd);
        DisplayLabel(pSelector->pSelectorLblItem);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}

    return iReturn;
}
static int SelectorBtnLeft_Up(void *pInArg,   int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    int i = (int)pOutArg;
    SELECTOR *pSelector = pFactoryMaintanceDateSelector[i];
	
	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{	
		//更新显示
		SetPictureBitmap(SEC_DN_UNPRESS, pSelector->pSelectorBtnLeft);
		DisplayPicture(pSelector->pSelectorBtnLeft);
		//执行回调函数
		pSelector->BackCallFunc(pSelector->uiCurrSelected);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
    return iReturn;
}
//选择器右选择按钮点击处理函数
static int SelectorBtnMidd_Down(void *pInArg,   int iInLen, 
                                void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    int i = (int)pOutArg;
    SELECTOR *pSelector = pFactoryMaintanceDateSelector[i];

	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{
		if (pSelector->uiItemTouchFlg)
		{
			pCurrSelector = pSelector;
			TouchChange(SEC_MD_PRESS, pSelector->pSelectorBtnMidd,
						NULL, pSelector->pSelectorLblItem, 1);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}	
    return iReturn;
}
static int SelectorBtnMidd_Up(void *pInArg,   int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    int i = (int)pOutArg;
    SELECTOR *pSelector = pFactoryMaintanceDateSelector[i];

	if (NULL == pSelector)
	{
		return -1;
	}

	if (pSelector->uiEnable)
	{
		if (pSelector->uiItemTouchFlg)
		{
			//更新显示
			SetPictureBitmap(SEC_MD_UNPRESS, pSelector->pSelectorBtnMidd);
			DisplayPicture(pSelector->pSelectorBtnMidd);
			DisplayLabel(pSelector->pSelectorLblItem);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
	}
    return iReturn;
}
//选择器元素点击处理函数
static int SelectorBtnRight_Down(void *pInArg,   int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    int i = (int)pOutArg;
    SELECTOR *pSelector = pFactoryMaintanceDateSelector[i];
	
	if (NULL == pSelector)
	{
		return -1;
	}
	
	if (pSelector->uiEnable)
	{
		SetPictureBitmap(SEC_UP_PRESS, pSelector->pSelectorBtnRight);
		DisplayPicture(pSelector->pSelectorBtnRight);
        pSelector->uiCurrSelected = addDate(pSelector->uiItemCnt, pSelector->uiCurrSelected, i);
        SetLabelText(pSelector->pItemString[pSelector->uiCurrSelected], 
				 pSelector->pSelectorLblItem);
    	DisplayPicture(pSelector->pSelectorBtnMidd);
    	DisplayLabel(pSelector->pSelectorLblItem);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
    return iReturn;
}
static int SelectorBtnRight_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    int i = (int)pOutArg;
    SELECTOR *pSelector = pFactoryMaintanceDateSelector[i];

	if (NULL == pSelector)
	{
		return -1;
	}

	if (pSelector->uiEnable)
	{
		//更新显示
		SetPictureBitmap(SEC_UP_UNPRESS, pSelector->pSelectorBtnRight);
		DisplayPicture(pSelector->pSelectorBtnRight);		 
		//执行回调函数 
		pSelector->BackCallFunc(pSelector->uiCurrSelected);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}

    return iReturn;
}
//日期增加
static int addDate(int iType, int iNum, int iSelected)
{
    if(iType == MAX_YEAR_NUMBER)
    {
        if(iNum == MAX_YEAR_NUMBER-1)
        {
            iNum = 0;
        }
        else
        {
            iNum++;
        }
    }
    else if(iType == MAX_MONTH_NUMBER)
    {
        if(iNum == MAX_MONTH_NUMBER-1)
        {
            iNum = 0;
        }
        else
        {
            iNum++;
        }
    }
    else
    {
        iNum = addDays(pFactoryMaintanceDateSelector[iSelected-2]->uiCurrSelected,
            pFactoryMaintanceDateSelector[iSelected-1]->uiCurrSelected, iNum);
    }

    return iNum;
}
//日期减少
static int subDate(int iType, int iNum, int iSelected)
{
    //错误标志，返回值定义 
    if(iType == MAX_YEAR_NUMBER)
    {
        if(iNum == 0)
        {
            iNum = MAX_YEAR_NUMBER-1;
        }
        else
        {
            iNum--;
        }
    }
    else if(iType == MAX_MONTH_NUMBER)
    {
        if(iNum == 0)
        {
            iNum = MAX_MONTH_NUMBER-1;
        }
        else
        {
            iNum--;
        }
    }
    else
    {
        iNum = subDays(pFactoryMaintanceDateSelector[iSelected-2]->uiCurrSelected,
            pFactoryMaintanceDateSelector[iSelected-1]->uiCurrSelected, iNum);
    }
    return iNum;
}
//天数增加
static int addDays(int year, int month, int day)
{
    day++;
    month++;
    switch(month)
    {
	case 0:
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12://31天
	    if (day > 30)
	    {
	        day = 0;
	    }
	    break;
	case 4:
	case 6:
	case 9:
	case 11://30天   
	    if (day > 29)
	    {
	        day = 0;
	    }
	    break;
	case 2:    
		if ( ((year+1970)%4 == 0 && (year+1970)%100 != 0) || 
			 ((year+1970)%400 == 0) ) 
	    {
	        if (day > 28)//29天
	        {
	            day = 0;
	        }
	    }
		else//28天
		{
	        if (day > 27)
	        {
	            day = 0;
	        }			
		}
	    break;
	default:
	    break;
	}
    return day;
}
//天数减少
static int subDays(int year, int month, int day)
{
    day--;
    month++;
    switch(month)
    {
	case 0:
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12://31天
	    if (day < 0)
	    {
	        day = 30;
	    }
	    break;
	case 4:
	case 6:
	case 9:
	case 11://30天   
	    if (day < 0)
	    {
	        day = 29;
	    }
	    break;
	case 2:    
		if ( ((year+1970)%4 == 0 && (year+1970)%100 != 0) || 
			 ((year+1970)%400 == 0) ) 
	    {
	        if (day < 0)//29天
	        {
	            day = 28;
	        }
	    }
		else//28天
		{
	        if (day < 0)
	        {
	            day = 27;
	        }			
		}
	    break;
	default:
	    break;
	}
    return day;
}
//
static void FactoryMaintanceDateYearBackFunc (int iSelected)
{

}
static void FactoryMaintanceDateMonthBackFunc (int iSelected)
{

}
static void FactoryMaintanceDateDayBackFunc (int iSelected)
{

}

//处理选择器递增或递减
static void HandleDateAddOrDec(int index, int isAddOrDec)
{
	if (isAddOrDec)//add
	{
		SelectorBtnRight_Down(NULL, 0, (void*)index, 0);
		SelectorBtnRight_Up(NULL, 0, (void*)index, 0);
	}
	else//dec
	{
		SelectorBtnLeft_Down(NULL, 0, (void*)index, 0);
		SelectorBtnLeft_Up(NULL, 0, (void*)index, 0);
	}
}

