/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlinearity.c
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020/10/28
*
*******************************************************************************/

#include "wnd_frmlinearity.h"
#include "wnd_frmuncertainty.h"
#include "wnd_frmfactory.h"
/*******************************************************************************
*                   定义wnd_frmfac.c引用其他头文件
*******************************************************************************/
#include "app_frmotdr.h"
#include "app_otdrcalibration.h"

#include "guiglobal.h"

#include "wnd_global.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmconfigureinfo.h"
#include "wnd_frmlanguageconfig.h"
#include "wnd_frmuncertainty.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmauthorization.h"

extern POTDR_TOP_SETTINGS pOtdrTopSettings;	//当前OTDR的工作设置
extern CURR_WINDOW_TYPE enCurWindow;

/*******************************************************************************
*                       窗体frmfac中的窗体控件
*******************************************************************************/
static GUIWINDOW *pFrmLinearity = NULL;

/*******************************************************************************
*                   窗体frmfac 桌面上的背景控件
*******************************************************************************/
static GUIPICTURE *pFrmLinearityLeftBg = NULL;

/*******************************************************************************
*                   窗体frmfac 桌面上的Hz显示控件
*******************************************************************************/

static GUICHAR *pFrmLinearityStrTitle = NULL;
static GUILABEL *pFrmLinearityLblTitle = NULL;

static GUICHAR *pFrmLinearityStrLinearityL = NULL;
static GUILABEL *pFrmLinearityLblLinearityL = NULL;

static GUICHAR *pFrmLinearityStrLinearityR = NULL;
static GUILABEL *pFrmLinearityLblLinearityR = NULL;

static GUICHAR *pFrmLinearityStrLinearityUnit = NULL;
static GUILABEL *pFrmLinearityLblLinearityUnit = NULL;

static GUICHAR *pFrmLinearityStrInfo = NULL;
static GUILABEL *pFrmLinearityLblInfo = NULL;

static GUICHAR *pFrmLinearityStrMeasure = NULL;
static GUILABEL *pFrmLinearityLblMeasure = NULL;

static GUIPICTURE *pFrmLinearityBtnMeasure = NULL;
static GUIPICTURE *pFrmLinearityBgInfo = NULL;
/*******************************************************************************
*               窗体frmvls右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFrmLinearityMenu = NULL;

/*******************************************************************************
*                   窗体frmvls内部函数声明
*******************************************************************************/
//初始化文本资源
static int FrmLinearityTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FrmLinearityTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int FrmLinearityMeasure_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FrmLinearityMeasure_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
                          
static void FrmLinearityMenuCallBack(int iOption);
static void DisplayInfo(GUICHAR *info);

/***
  * 功能：
        窗体frmfacDR的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmLinearityInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
	char* StrMenu[FACTORY_MENU_COUNT] = {
		"DebugInfo",
		"LanguageSet",
		"Non-linearity",
		"Uncertainty",
		"FactorySet",
		"Authorization"
	};
    
	//得到当前窗体对象 
    pFrmLinearity = (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FrmLinearityTextRes_Init(NULL, 0, NULL, 0);

    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFrmLinearityLeftBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factory.bmp");
    pFrmLinearityLblTitle = CreateLabel(36, 12, 100, 16, pFrmLinearityStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmLinearityLblTitle);
	
    pFrmLinearityBgInfo = CreatePicture(20, 50, 200, 119, BmpFileDirectory"bg_Info.bmp");
    pFrmLinearityBtnMeasure = CreatePicture(250, 200, 55, 18, BmpFileDirectory"btn_dialog_unpress.bmp");

    pFrmLinearityLblLinearityL = CreateLabel(40, 200, 100, 24, pFrmLinearityStrLinearityL);
    pFrmLinearityLblLinearityR = CreateLabel(120, 200, 100, 24, pFrmLinearityStrLinearityR);
    pFrmLinearityLblLinearityUnit = CreateLabel(200, 200, 50, 24, pFrmLinearityStrLinearityUnit);
    pFrmLinearityLblInfo = CreateLabel(25, 90, 200, 60, pFrmLinearityStrInfo);
    pFrmLinearityLblMeasure = CreateLabel(250, 201, 55, 16, pFrmLinearityStrMeasure);
    
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmLinearityLblLinearityL);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmLinearityLblLinearityR);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmLinearityLblLinearityUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmLinearityLblInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmLinearityLblMeasure);
	
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmLinearityLblLinearityL); 
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmLinearityLblLinearityR); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmLinearityLblLinearityUnit); 
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmLinearityLblInfo); 
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmLinearityLblMeasure); 

    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFrmLinearityMenu = CreateStringWndMenu(FACTORY_MENU_COUNT, sizeof(StrMenu), StrMenu,  0xff00,
								2, 54, FrmLinearityMenuCallBack);

    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmLinearity, pFrmLinearity);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPALETTE), pFrmLinearityBtnMeasure, pFrmLinearity);
    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFrmLinearityMenu, pFrmLinearity);
	
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	LoginWndMenuToMsg1(pFrmLinearityMenu, pFrmLinearity);
    

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmLinearityBtnMeasure, 
                    FrmLinearityMeasure_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmLinearityBtnMeasure,
		            FrmLinearityMeasure_Up, NULL, 0, pMsg);
    
	return iRet;
}


/***
  * 功能：
        窗体frmvls的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmLinearityExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmLinearity = (GUIWINDOW *) pWndObj;


    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmLinearity);


    DestroyPicture(&pFrmLinearityLeftBg);
    DestroyLabel(&pFrmLinearityLblTitle);
	DestroyWndMenu1(&pFrmLinearityMenu);

    DestroyPicture(&pFrmLinearityBtnMeasure);
    DestroyPicture(&pFrmLinearityBgInfo);
    
    DestroyLabel(&pFrmLinearityLblLinearityL);
    DestroyLabel(&pFrmLinearityLblLinearityR);
    DestroyLabel(&pFrmLinearityLblLinearityUnit);
    DestroyLabel(&pFrmLinearityLblInfo);
    DestroyLabel(&pFrmLinearityLblMeasure);
    FrmLinearityTextRes_Exit(NULL, 0, NULL, 0);

    return iRet;
}

/***
  * 功能：
        窗体frmvls的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmLinearityPaint(void *pWndObj)
{
    int iRet = 0;
    
	DisplayPicture(pFrmLinearityLeftBg);
    DisplayLabel(pFrmLinearityLblTitle);

    DisplayPicture(pFrmLinearityBtnMeasure);
    DisplayPicture(pFrmLinearityBgInfo);
    
    DisplayLabel(pFrmLinearityLblLinearityL);
    DisplayLabel(pFrmLinearityLblLinearityR);
    DisplayLabel(pFrmLinearityLblLinearityUnit);
    DisplayLabel(pFrmLinearityLblInfo);
    DisplayLabel(pFrmLinearityLblMeasure);
    
	DisplayWndMenu1(pFrmLinearityMenu);
    
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}

/***
  * 功能：
        窗体frmvls的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmLinearityLoop(void *pWndObj)
{
	//错误标志、返回值定义
    int iRet = 0;
	SendWndMsg_LoopDisable(pWndObj);
    return iRet;
}

int FrmLinearityPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

int FrmLinearityResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

static int FrmLinearityTextRes_Init(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    pFrmLinearityStrTitle = TransString("Non-linearity");

    pFrmLinearityStrLinearityL = TransString("Non-linearity:");
    pFrmLinearityStrLinearityR = TransString("--");
    pFrmLinearityStrLinearityUnit = TransString("dB");
    pFrmLinearityStrInfo = TransString("Please connect the 50KM fiber before measuring");
    pFrmLinearityStrMeasure = TransString("Measure");
    return 0;
}

static int FrmLinearityTextRes_Exit(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    GuiMemFree(pFrmLinearityStrTitle);

    GuiMemFree(pFrmLinearityStrLinearityL);
    GuiMemFree(pFrmLinearityStrLinearityR);
    GuiMemFree(pFrmLinearityStrLinearityUnit);
    GuiMemFree(pFrmLinearityStrInfo);
    GuiMemFree(pFrmLinearityStrMeasure);
	return 0;
}

static int FrmLinearityMeasure_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 1;
    
    TouchChange("btn_dialog_press.bmp", pFrmLinearityBtnMeasure, NULL, NULL, 1);
    
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

static int FrmLinearityMeasure_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    int iReturn = 1;
    float fLinearity = 0.0f;
    char buff[16] = {0};
    
    TouchChange("btn_dialog_unpress.bmp", pFrmLinearityBtnMeasure, pFrmLinearityStrMeasure, pFrmLinearityLblMeasure, 3);
    SetPictureEnable(0, pFrmLinearityBtnMeasure);   
    DisplayInfo(TransString("Measuring, please wait a moment..."));
	RefreshScreen(__FILE__, __func__, __LINE__);
	
    BoardWorkInit(pOtdrTopSettings->pOpmSet, 0);

	fLinearity = CalcLinearity(pOtdrTopSettings);
    LOG(LOG_INFO, "fLinearity = %f\n", fLinearity);
    sprintf(buff,"%.3f",fLinearity);

	GuiMemFree(pFrmLinearityStrLinearityR);
    pFrmLinearityStrLinearityR = TransString(buff);
    SetLabelText(pFrmLinearityStrLinearityR, pFrmLinearityLblLinearityR);
    DisplayLabel(pFrmLinearityLblLinearityR);

    SetPictureEnable(1, pFrmLinearityBtnMeasure);   
	TouchChange("btn_dialog_unpress.bmp", pFrmLinearityBtnMeasure, pFrmLinearityStrMeasure, pFrmLinearityLblMeasure, 1);
    DisplayInfo(TransString("Measurement complete"));
    FrmLinearityPaint(NULL);
    return iReturn;
}

//右侧菜单响应处理
static void FrmLinearityMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	
	switch (iOption)
	{
	case DEBUG_INFO:
		enCurWindow = ENUM_DEBUG_INFO_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmConfigureInfoInit, FrmConfigureInfoExit,
			FrmConfigureInfoPaint, FrmConfigureInfoLoop,
			FrmConfigureInfoPause, FrmConfigureInfoResume,
			NULL);
		SendWndMsg_WindowExit(pFrmLinearity);
		SendSysMsg_ThreadCreate(pWnd);
        break;
    case LANGUAGE_SET:
		enCurWindow = ENUM_LANGUAGE_CONFIG_WIN;
    	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
        		FrmLanguageConfigInit, FrmLanguageConfigExit,
        		FrmLanguageConfigPaint, FrmLanguageConfigLoop,
        		FrmLanguageConfigPause, FrmLanguageConfigResume,
			NULL);          
	    SendWndMsg_WindowExit(pFrmLinearity);  
	    SendSysMsg_ThreadCreate(pWnd); 
        break;
    case NON_LINEARITY:
        break;
    case UNCERTAINTY:
		enCurWindow = ENUM_UNCERTAINTY_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmUncertaintyInit, FrmUncertaintyExit,
			FrmUncertaintyPaint, FrmUncertaintyLoop,
			FrmUncertaintyPause, FrmUncertaintyResume,
			NULL);
		SendWndMsg_WindowExit(pFrmLinearity);
		SendSysMsg_ThreadCreate(pWnd);
        break;
    case FACTORY_SET:
		enCurWindow = ENUM_FACTORY_SET_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmFactorySetInit, FrmFactorySetExit,
			FrmFactorySetPaint, FrmFactorySetLoop,
			FrmFactorySetPause, FrmFactorySetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmLinearity);
		SendSysMsg_ThreadCreate(pWnd);
        break;
	case AUTHORIZATION:
		enCurWindow = ENUM_AUTHORIZATION_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmAuthorizationInit, FrmAuthorizationExit,
			FrmAuthorizationPaint, FrmAuthorizationLoop,
			FrmAuthorizationPause, FrmAuthorizationResume,
			NULL);
		SendWndMsg_WindowExit(pFrmLinearity);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case BACK_DOWN:
	case HOME_DOWN:
		enCurWindow = ENUM_OTHER_WIN;
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmSystemSettingInit, FrmSystemSettingExit,
						    FrmSystemSettingPaint, FrmSystemSettingLoop,
						    FrmSystemSettingPause, FrmSystemSettingResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmLinearity);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}

//显示测试信息
static void DisplayInfo(GUICHAR *info)
{
    if(info)
    {
        GuiMemFree(pFrmLinearityStrInfo);
        pFrmLinearityStrInfo = info;
        SetLabelText(pFrmLinearityStrInfo, pFrmLinearityLblInfo);
        DisplayPicture(pFrmLinearityBgInfo);
        DisplayLabel(pFrmLinearityLblInfo);
	    RefreshScreen(__FILE__, __func__, __LINE__);
    }
}

//按键响应处理
void LinearityKeyBackCall(int iOption)
{
	switch (iOption)
	{
	case 2://enter
		FrmLinearityMeasure_Down(NULL, 0, NULL, 0);
		FrmLinearityMeasure_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}

