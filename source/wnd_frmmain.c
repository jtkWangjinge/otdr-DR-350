/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmMain.c
* 摘    要：  实现主窗体FrmMain的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-8-13
*
*******************************************************************************/

#include "wnd_frmmain.h"

#include <sys/types.h>
#include <unistd.h>

#include "app_sorviewer.h"
#include "app_curve.h"
#include "app_parameter.h"
#include "app_getsetparameter.h"

#include "wnd_frmdialog.h"
#include "wnd_frmabout.h"
#include "wnd_frmotdr.h"
#include "wnd_frmvfl.h"
#include "wnd_frmlightsource.h"
#include "wnd_frmopm.h"
#include "wnd_frmfip.h"
#include "wnd_frmfilebrowse.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmcommonset.h"
#include "wnd_frmrj45.h"


/*********************************背景控件定义*********************************/
static GUIPICTURE *pMainBg = NULL;
/*******************************************************************************
**							窗体FrmMain中的控件定义部分						 **
*******************************************************************************/
static GUIWINDOW *pFrmMain = NULL;

/*******************************模块功能控件定义*******************************/
//图标
static GUIPICTURE *pMainIconOtdr = NULL;     		//OTDR
static GUIPICTURE *pMainIconVFL = NULL;             //光源
static GUIPICTURE *pMainIconOPM = NULL;     	    //OPM
static GUIPICTURE *pMainIconOPL = NULL;     	    //OPL(稳定光源)

static GUIPICTURE *pMainIconFip = NULL;             //FIP
static GUIPICTURE *pMainIconRJ45 = NULL;            //RJ45
static GUIPICTURE *pMainIconFile = NULL;     		//文件管理
static GUIPICTURE *pMainIconSetting = NULL;         //系统设置
//文本
static GUILABEL *pMainLblOtdr = NULL;     			//OTDR
static GUILABEL *pMainLblVFL = NULL;     	        //光源
static GUILABEL *pMainLblOPM = NULL;     	        //OPM
static GUILABEL *pMainLblOPL = NULL;     	        //稳定光源

static GUILABEL *pMainLblFip = NULL;     		    //FIP
static GUILABEL *pMainLblRJ45 = NULL;     		    //RJ45
static GUILABEL *pMainLblFile = NULL;     			//文件管理
static GUILABEL *pMainLblSetting = NULL;     		//设置
//文本数据
static GUICHAR *pMainStrOtdr = NULL;     			//OTDR
static GUICHAR *pMainStrVFL = NULL;     	        //光源
static GUICHAR *pMainStrOPM = NULL;     	        //OPM
static GUICHAR *pMainStrOPL = NULL;     	        //OPL

static GUICHAR *pMainStrFip = NULL;     		    //FIP
static GUICHAR *pMainStrRJ45 = NULL;     		    //RJ45
static GUICHAR *pMainStrFile = NULL;     			//文件管理
static GUICHAR *pMainStrSetting = NULL;     		//设置

static GUILABEL *pMainLblTitle = NULL;              //标题信息
static GUICHAR *pMainStrTitle = NULL;               //标题信息

/*******************************宏定义*******************************/
#define MAX_ICON_NUM            8                   // 图标的最大个数

/*******************************全局变量定义*******************************/
static int WndRecordFlag = 1;                       //窗口记录标志位
static int iCurrIco = 0;                            //记录当前选中的按钮
extern CURR_WINDOW_TYPE enCurWindow;                //当前的窗口
//引用保存到eeprom的变量
extern PFactoryConfig pFactoryConf;

static char *ppMainIco_Nomal[MAX_ICON_NUM] = { 
	BmpFileDirectory"bg_otdr_up.bmp", 
	BmpFileDirectory"bg_light_up.bmp",
	BmpFileDirectory"bg_opm_up.bmp",
	BmpFileDirectory"bg_opl_up.bmp",
	BmpFileDirectory"bg_fip_up.bmp",
	BmpFileDirectory"bg_RJ45_up.bmp",
	BmpFileDirectory"bg_file_up.bmp",
	BmpFileDirectory"bg_setting_up.bmp"
};

static char *ppMainIco_Focus[MAX_ICON_NUM] = {
	BmpFileDirectory"bg_otdr_select.bmp", 
	BmpFileDirectory"bg_light_select.bmp",
	BmpFileDirectory"bg_opm_select.bmp",
	BmpFileDirectory"bg_opl_select.bmp",
	BmpFileDirectory"bg_fip_select.bmp",
	BmpFileDirectory"bg_RJ45_select.bmp",
	BmpFileDirectory"bg_file_select.bmp",
	BmpFileDirectory"bg_setting_select.bmp"
};

static GUIPICTURE *ppMainIco[MAX_ICON_NUM] = {NULL};
static GUILABEL *ppMainLbl[MAX_ICON_NUM] = {NULL};
/*******************************************************************************
**	   	窗体FrmMain中的初始化文本资源、 释放文本资源函数定义部分			 **
*******************************************************************************/
static int MainTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
/**************************功能控件的事件处理函数**************************/
static int MainIconOtdr_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIconOtdr_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int MainIconVFL_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIconVFL_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int MainIconOPM_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIconOPM_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
//稳定光源
static int MainIconOPL_Down(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);
static int MainIconOPL_Up(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);

static int MainIconFip_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIconFip_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
//RJ45
static int MainIconRJ45_Down(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);
static int MainIconRJ45_Up(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen);

static int MainIconFile_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIconFile_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int MainIconSetting_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIconSetting_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

/*******************************************************************************
**			    	窗体FrmMain中的错误事件处理函数定义部分					  **
*******************************************************************************/
static int MainErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
/***************************窗体的按键事件处理函数*****************************/
static int MainWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int MainWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);
/***************************图标移动事件处理函数*****************************/
static void MainIcoJmp(int iPrev, int iCurr);  

static void MainIcoJmpLeft(void);

static void MainIcoJmpRight(void);

static void MainIcoJmpUp(void);

static void MainIcoJmpDown(void);

static int MainIcoOK_Down(void);

static int MainIcoOK_Up(void);

//otdr窗口切换故障检测窗口清空曲线
static int ClearCurveData(int WndNum);

/**********************************************************************************
**				    	       声明窗体处理相关函数			            		 **
***********************************************************************************/
//窗体FrmMain的初始化函数，建立窗体控件、注册消息处理
int FrmMainInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
	MainTextRes_Init(NULL, 0, NULL, 0);
    /***************************************************************************
	**							窗体FrmMain中的创建背景         			  **
	***************************************************************************/    
	pMainBg = CreatePicture(0, 0, WINDOW_WIDTH, 40, BmpFileDirectory"bg_main_info.bmp");
    //模块背景
    pMainIconOtdr = CreatePicture(0, 40, 160, 220, BmpFileDirectory"bg_otdr_up.bmp");
    pMainIconVFL = CreatePicture(160, 40, 160, 220, BmpFileDirectory"bg_light_up.bmp");
    pMainIconOPM = CreatePicture(2*160, 40, 160, 220, BmpFileDirectory"bg_opm_up.bmp");
	pMainIconOPL = CreatePicture(3*160, 40, 160, 220, BmpFileDirectory"bg_opl_up.bmp");
    
    pMainIconFip = CreatePicture(0, 40+220, 160, 220, BmpFileDirectory"bg_fip_up.bmp");
	pMainIconRJ45 = CreatePicture(160, 40 + 220, 160, 220, BmpFileDirectory"bg_RJ45_up.bmp");
    pMainIconFile = CreatePicture(2*160, 40+220, 160, 220, BmpFileDirectory"bg_file_up.bmp");
    pMainIconSetting = CreatePicture(3*160, 40+220, 160, 220, BmpFileDirectory"bg_setting_up.bmp");
    //模块文本
    pMainLblOtdr = CreateLabel( 0, 200, 160, 24, pMainStrOtdr);
	pMainLblVFL = CreateLabel( 160, 200, 160, 24, pMainStrVFL);
    pMainLblOPM = CreateLabel( 160*2, 200, 160, 24, pMainStrOPM);
	pMainLblOPL = CreateLabel(160*3, 200, 160, 24, pMainStrOPL);
    
    pMainLblFip = CreateLabel( 0, 200+220, 160, 24, pMainStrFip);
	pMainLblRJ45 = CreateLabel(160, 200 + 220, 160, 24, pMainStrRJ45);
    pMainLblFile = CreateLabel(160*2, 200+220, 160, 24, pMainStrFile);
    pMainLblSetting = CreateLabel(160*3, 200+220, 160, 24, pMainStrSetting);
    
    pMainLblTitle= CreateLabel(0, 22, 100, 24, pMainStrTitle);
    //设置文本对齐方式
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblOtdr);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblVFL);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblOPM);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblOPL);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblFip);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblRJ45);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblFile);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblSetting);
    //设置文本字体(默认白色字体)
    //设置模块图标
    GUIPICTURE *ppMainIco1[] = {
        pMainIconOtdr,
        pMainIconVFL,
        pMainIconOPM,
		pMainIconOPL,
        pMainIconFip,
		pMainIconRJ45,
        pMainIconFile,
        pMainIconSetting
    };

    GUILABEL *ppMainLbl1[] = {
        pMainLblOtdr,
        pMainLblVFL,
        pMainLblOPM,
		pMainLblOPL,
        pMainLblFip,
		pMainLblRJ45,
        pMainLblFile,
        pMainLblSetting,
    };
    
    int i;
    for(i = 0; i < MAX_ICON_NUM; ++i)
    {
        ppMainIco[i] = ppMainIco1[i];
        ppMainLbl[i] = ppMainLbl1[i];
    }
	SetPictureBitmap(ppMainIco_Focus[iCurrIco], ppMainIco[iCurrIco]);
    
    /***************************************************************************
	**							窗体FrmMain中的添加控件         			  **
	***************************************************************************/
	//得到当前窗体对象
	pFrmMain = (GUIWINDOW *) pWndObj;
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmMain, 
	              pFrmMain);
    
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconOtdr, 
                  pFrmMain);  
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconVFL, 
                  pFrmMain);    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconOPM, 
                  pFrmMain);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconOPL,
				  pFrmMain);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconFip, 
                  pFrmMain);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconRJ45,
				  pFrmMain);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconFile, 
                  pFrmMain);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIconSetting, 
                  pFrmMain);
	/***************************************************************************
	**						窗体FrmMain中的添加控件响应函数         		  **
	***************************************************************************/
	GUIMESSAGE *pMsg = NULL;
    pMsg = GetCurrMessage();
    
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconOtdr, 
                    MainIconOtdr_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconOtdr, 
                    MainIconOtdr_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconVFL, 
                    MainIconVFL_Down, NULL, 1, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconVFL, 
                    MainIconVFL_Up, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconOPM, 
                    MainIconOPM_Down, NULL, 2, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconOPM, 
                    MainIconOPM_Up, NULL, 2, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconOPL,
					MainIconOPL_Down, NULL, 3, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconOPL,
					MainIconOPL_Up, NULL, 3, pMsg);


    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconFip, 
                    MainIconFip_Down, NULL, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconFip, 
                    MainIconFip_Up, NULL, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconRJ45,
					MainIconRJ45_Down, NULL, 5, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconRJ45,
					MainIconRJ45_Up, NULL, 5, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconFile, 
                    MainIconFile_Down, NULL, 6, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconFile, 
                    MainIconFile_Up, NULL, 6, pMsg);  
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIconSetting, 
                    MainIconSetting_Down, NULL, 7, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIconSetting, 
                    MainIconSetting_Up, NULL, 7, pMsg);

    //进入主界面则设置SorViewer为 OtdrSorViewer
	SetCurrSorViewer(OtdrSorViewer);
    
    //注册错误消息处理函数
	LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmMain, 
                    MainErrProc_Func, NULL, 0, pMsg);
    
    //注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmMain, 
	                MainWndKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmMain, 
	                MainWndKey_Up, NULL, 0, pMsg);

	return iReturn;
}

//窗体FrmMain的退出函数，释放所有资源
int FrmMainExit(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;

    GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pFrmMain = (GUIWINDOW *) pWndObj;
	
	/***************************************************************************
	**							  清空消息队列中的消息注册项     			  **
	***************************************************************************/
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);

	/***************************************************************************
	**						      清空指定窗体的窗体控件队列      			  **
	***************************************************************************/
	ClearWindowComp(pFrmMain);

	/***************************************************************************
	**						          销毁窗体背景控件         	    		  **
	***************************************************************************/
	DestroyPicture(&pMainBg);
    /***************************************************************************
	**						         销毁窗体功能模块控件      	    		  **
	***************************************************************************/
	DestroyPicture(&pMainIconOtdr);
	DestroyPicture(&pMainIconVFL);
    DestroyPicture(&pMainIconOPM);
	DestroyPicture(&pMainIconOPL);
	DestroyPicture(&pMainIconFip);
	DestroyPicture(&pMainIconRJ45);
   	DestroyPicture(&pMainIconFile);
	DestroyPicture(&pMainIconSetting);
    
	DestroyLabel(&pMainLblOtdr);
	DestroyLabel(&pMainLblVFL);
    DestroyLabel(&pMainLblOPM);
	DestroyLabel(&pMainLblOPL);
	DestroyLabel(&pMainLblFip);
	DestroyLabel(&pMainLblRJ45);
	DestroyLabel(&pMainLblFile);
    DestroyLabel(&pMainLblSetting);

    DestroyLabel(&pMainLblTitle);

	/***************************************************************************
	**						             释放文本资源       	     		  **
	***************************************************************************/
	MainTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}

//窗体FrmMain的绘制函数，绘制整个窗体
int FrmMainPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

    /***************************************************************************
	**						          显示窗体背景图片          			  **
	***************************************************************************/	
	DisplayPicture(pMainBg);
    /***************************************************************************
	**					           显示窗体功能模块控件        		    	  **
	***************************************************************************/
	DisplayPicture(pMainIconOtdr);
    DisplayPicture(pMainIconVFL);
    DisplayPicture(pMainIconOPM);
	DisplayPicture(pMainIconOPL);
    DisplayPicture(pMainIconFip);
	DisplayPicture(pMainIconRJ45);
	DisplayPicture(pMainIconFile);
	DisplayPicture(pMainIconSetting);
    
	DisplayLabel(pMainLblOtdr);
	DisplayLabel(pMainLblVFL);
    DisplayLabel(pMainLblOPM);
	DisplayLabel(pMainLblOPL);
	DisplayLabel(pMainLblFip);
	DisplayLabel(pMainLblRJ45);
	DisplayLabel(pMainLblFile);
    DisplayLabel(pMainLblSetting);

	//DisplayLabel(pMainLblTitle);
    
    enCurWindow = ENUM_MAIN_WIN;	//设置当前窗体
	
	//刷新屏幕缓存
	SetPowerEnable(1, 1);
	//显示日期信息
	ShowSta_DateInfo(0);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

//窗体FrmMain的循环函数，进行窗体循环
int FrmMainLoop(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;
    //ShowSta_FiberMicroscopeInfo(0);
    //ShowSta_UDiskInfo(0);
	//刷新日期
	//ShowSta_DateInfo(1);
    
    //RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

//窗体FrmMain的挂起函数，进行窗体挂起前预处理
int FrmMainPause(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;
    
    return iReturn;
}

//窗体FrmMain的恢复函数，进行窗体恢复前预处理
int FrmMainResume(void *pWndObj)
{
    //错误标志、返回值定义
	int iReturn = 0;
    
    return iReturn;
}

/*******************************************************************************
**	   	窗体FrmMain中的初始化文本资源、 释放文本资源函数定义部分			 **
*******************************************************************************/
static int MainTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

	pMainStrOtdr = GetCurrLanguageText(MAIN_LBL_OTDR);
    pMainStrVFL = GetCurrLanguageText(MAIN_LBL_VFL);
    pMainStrOPM = GetCurrLanguageText(MAIN_LBL_OPM);
	pMainStrOPL = GetCurrLanguageText(MAIN_LBL_LIGHT_SOURCE);
    pMainStrFip = GetCurrLanguageText(MAIN_LBL_FIP);
	pMainStrRJ45 = GetCurrLanguageText(MAIN_LBL_RJ45);
    pMainStrFile = GetCurrLanguageText(MAIN_LBL_FILE);
    pMainStrSetting = GetCurrLanguageText(MAIN_LBL_SYSTEM_SET);
    pMainStrTitle = TransString("MAIN_LBL_MAINMENU");

	return iReturn;
}

static int MainTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
	
	/***************************************************************************
	**					            释放功能模块的文本          	    	  **
	***************************************************************************/
	GuiMemFree(pMainStrOtdr);
	GuiMemFree(pMainStrVFL);
    GuiMemFree(pMainStrOPM);
	GuiMemFree(pMainStrOPL);
	GuiMemFree(pMainStrFip);
	GuiMemFree(pMainStrRJ45);
	GuiMemFree(pMainStrFile);
	GuiMemFree(pMainStrSetting);
    GuiMemFree(pMainStrTitle);
    
    return iReturn;
}

/**************************功能控件的事件处理函数**************************/
static int MainIconOtdr_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
    DisplayLabel(ppMainLbl[iCurrIco]);
    iCurrIco = iOutLen;
    TouchChange("bg_otdr_down.bmp", pMainIconOtdr, NULL, pMainLblOtdr, 2);
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

static int MainIconOtdr_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    enCurWindow = ENUM_OTDR_WIN;	//设置当前窗体
	TouchChange("bg_otdr_up.bmp", pMainIconOtdr, NULL, pMainLblOtdr, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	//调度frmiolm
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmOtdrInit, FrmOtdrExit, 
                        FrmOtdrPaint, FrmOtdrLoop, 
			            FrmOtdrPause, FrmOtdrResume,
                     NULL);         //pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
	ClearCurveData(1);
    return iReturn;
}

//光源的响应函数
static int MainIconVFL_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
    DisplayLabel(ppMainLbl[iCurrIco]);
    iCurrIco = iOutLen;
    TouchChange("bg_light_down.bmp", pMainIconVFL, NULL, pMainLblVFL, 2);
	RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

static int MainIconVFL_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
	
	TouchChange("bg_light_up.bmp", pMainIconVFL, NULL, pMainLblVFL, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);

    GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						FrmVflInit, FrmVflExit, 
						FrmVflPaint, FrmVflLoop, 
						FrmVflPause, FrmVflResume,
						NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

	return iReturn;
}

//OPM的响应函数
static int MainIconOPM_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
    DisplayLabel(ppMainLbl[iCurrIco]);
    iCurrIco = iOutLen;
    TouchChange("bg_opm_down.bmp", pMainIconOPM, NULL, pMainLblOPM, 2);
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

static int MainIconOPM_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    TouchChange("bg_opm_up.bmp", pMainIconOPM, NULL, pMainLblOPM, 0);
    RefreshScreen(__FILE__, __func__, __LINE__);

    GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						FrmOpmInit, FrmOpmExit, 
						FrmOpmPaint, FrmOpmLoop, 
						FrmOpmPause, FrmOpmResume,
						NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
	
    return iReturn;
}

//稳定光源
static int MainIconOPL_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
	DisplayLabel(ppMainLbl[iCurrIco]);
	iCurrIco = iOutLen;
	TouchChange("bg_opl_down.bmp", pMainIconOPL, NULL, pMainLblOPL, 2);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

static int MainIconOPL_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	TouchChange("bg_opl_up.bmp", pMainIconOPL, NULL, pMainLblOPL, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
#ifdef EEPROM_DATA
	//opl授权使能
	if (pFactoryConf->enabled & 0x10)
	{
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmLightSourceInit, FrmLightSourceExit,
			FrmLightSourcePaint, FrmLightSourceLoop,
			FrmLightSourcePause, FrmLightSourceResume,
			NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
	}
	else
	{
		DialogInit(120, 90, TransString(""),
				   GetCurrLanguageText(DIALOG_LBL_PURCHASE),
				   0, 0, FrmMainReCreateWindow, NULL, NULL);
	}
#else
	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						FrmLightSourceInit, FrmLightSourceExit,
						FrmLightSourcePaint, FrmLightSourceLoop,
						FrmLightSourcePause, FrmLightSourceResume,
						NULL);		 //pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmMain); //发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);	 //发送消息以便调用新的窗体
#endif
	return iReturn;
}

//fip的响应函数
static int MainIconFip_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
    DisplayLabel(ppMainLbl[iCurrIco]);
    iCurrIco = iOutLen;
	TouchChange("bg_fip_down.bmp", pMainIconFip, NULL, pMainLblFip, 2);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}

static int MainIconFip_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
	
	TouchChange("bg_fip_up.bmp", pMainIconFip, NULL, pMainLblFip, 0);
#ifdef EEPROM_DATA
	//fip授权使能
	if (pFactoryConf->enabled & 0x01)
	{
		CallFipWnd(pFrmMain, FrmMainReCreateWindow);
	}
	else
	{
		DialogInit(120, 90, TransString(""),
				   GetCurrLanguageText(DIALOG_LBL_PURCHASE),
				   0, 0, FrmMainReCreateWindow, NULL, NULL);
	}
#else
	CallFipWnd(pFrmMain, FrmMainReCreateWindow);
#endif
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}

//RJ45
static int MainIconRJ45_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
	DisplayLabel(ppMainLbl[iCurrIco]);
	iCurrIco = iOutLen;
	TouchChange("bg_RJ45_down.bmp", pMainIconRJ45, NULL, pMainLblRJ45, 2);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

static int MainIconRJ45_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	TouchChange("bg_RJ45_up.bmp", pMainIconRJ45, NULL, pMainLblRJ45, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);

	GUIWINDOW *pWnd = NULL;
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmRJ45Init, FrmRJ45Exit,
		FrmRJ45Paint, FrmRJ45Loop,
		FrmRJ45Pause, FrmRJ45Resume,
		NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

	return iReturn;
}

//文件的响应函数
static int MainIconFile_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
    DisplayLabel(ppMainLbl[iCurrIco]);
    iCurrIco = iOutLen;
	TouchChange("bg_file_down.bmp", pMainIconFile, NULL, pMainLblFile, 2);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}

static int MainIconFile_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
	
	TouchChange("bg_file_up.bmp", pMainIconFile, NULL, pMainLblFile, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	enCurWindow = ENUM_FILE_WIN;
    iReturn = FileManager(pFrmMain);
    
	return iReturn;
}

//系统设置的响应函数
static int MainIconSetting_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    SetPictureBitmap(ppMainIco_Nomal[iCurrIco], ppMainIco[iCurrIco]);
	DisplayPicture(ppMainIco[iCurrIco]);
    DisplayLabel(ppMainLbl[iCurrIco]);
    iCurrIco = iOutLen;
	TouchChange("bg_setting_down.bmp", pMainIconSetting, NULL, pMainLblSetting, 2);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}

static int MainIconSetting_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
	
    TouchChange("bg_setting_up.bmp", pMainIconSetting, NULL, pMainLblSetting, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
    GUIWINDOW *pWnd = NULL;

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						FrmSystemSettingInit, FrmSystemSettingExit, 
						FrmSystemSettingPaint, FrmSystemSettingLoop, 
						FrmSystemSettingPause, FrmSystemSettingResume,
						NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

	return iReturn;
}
                          

/*******************************************************************************
**			    	窗体FrmMain中的错误事件处理函数定义部分					  **
*******************************************************************************/
static int MainErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;

    return iReturn;
}

/***************************窗体的按键事件处理函数*****************************/
static int MainWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
	
	//CODER_LOG(CoderGu, "MainWndKey_Down pInArg = %d\n", (unsigned int)pInArg);
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;

	switch(uiValue)
    {
        case KEYCODE_FILE:
            MainIconFile_Down(pInArg, iInLen, pOutArg, 6);
            break;
        case KEYCODE_ENTER:
            MainIcoOK_Down();
            break;
        case KEYCODE_HOME:
            break;
        default:
            break;
    }

	return iReturn;
}

static int MainWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
	//CODER_LOG(CoderGu, "MainWndKey_Up pInArg = %d\n", (unsigned int)pInArg);
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;

	switch(uiValue)
    {
		case KEYCODE_OTDR_LEFT:
			LOG(LOG_INFO, "---otdr left------\n");
			break;
		case KEYCODE_OTDR_RIGHT:
			LOG(LOG_INFO, "---otdr right------\n");
			break;
		case KEYCODE_UP:
			LOG(LOG_INFO, "---up------\n");
			MainIcoJmpUp();
			break;
		case KEYCODE_DOWN:
			LOG(LOG_INFO, "---down------\n");
			MainIcoJmpDown();
			break;
		case KEYCODE_LEFT:
			LOG(LOG_INFO, "---left------\n");
			MainIcoJmpLeft();
			break;
		case KEYCODE_RIGHT:
			LOG(LOG_INFO, "---right------\n");
			MainIcoJmpRight();
			break;
		case KEYCODE_ENTER:
			LOG(LOG_INFO, "---enter------\n");
			MainIcoOK_Up();
			break;
        case KEYCODE_FILE:
			LOG(LOG_INFO, "---file------\n");
			MainIconFile_Up(NULL, 0, NULL, 6);
            break;
		case KEYCODE_VFL:
			LOG(LOG_INFO, "---VFL------\n");
			MainIconVFL_Up(NULL, 0, NULL, 0);
			break;
		default:
            break;
    }
	return iReturn;
}

static void MainIcoJmp(int iPrev, int iCurr)
{
    if (iPrev != iCurr)
	{
		SetPictureBitmap(ppMainIco_Nomal[iPrev], ppMainIco[iPrev]);
		SetPictureBitmap(ppMainIco_Focus[iCurr], ppMainIco[iCurr]);
		DisplayPicture(ppMainIco[iPrev]);
		DisplayPicture(ppMainIco[iCurr]);
		DisplayLabel(ppMainLbl[iPrev]);
		DisplayLabel(ppMainLbl[iCurr]);
	}
}

static void MainIcoJmpLeft(void)
{
    int iPrev = iCurrIco;

	--iCurrIco;
	if (0 > iCurrIco)
	{
		iCurrIco = 7;
	}
	MainIcoJmp(iPrev, iCurrIco);
	RefreshScreen(__FILE__, __func__, __LINE__);
}

static void MainIcoJmpRight(void)
{
    int iPrev = iCurrIco;

	++iCurrIco;
	if (7 < iCurrIco)
	{
		iCurrIco = 0;
	}
	MainIcoJmp(iPrev, iCurrIco);
	RefreshScreen(__FILE__, __func__, __LINE__);
}

static void MainIcoJmpUp(void)
{
    int iPrev = iCurrIco;
	if (3 < iCurrIco)
	{
		iCurrIco -= 4;
	}
	MainIcoJmp(iPrev, iCurrIco);
	RefreshScreen(__FILE__, __func__, __LINE__);
}

static void MainIcoJmpDown(void)
{
    int iPrev = iCurrIco;
	if (4 > iCurrIco)
	{
		iCurrIco += 4;
	}
	MainIcoJmp(iPrev, iCurrIco);
	RefreshScreen(__FILE__, __func__, __LINE__);
}

static int MainIcoOK_Down(void)
{
    MSGFUNC ppIcoFun[] = {
        MainIconOtdr_Down,
        MainIconVFL_Down,
        MainIconOPM_Down,
		MainIconOPL_Down,
        MainIconFip_Down,
		MainIconRJ45_Down,
        MainIconFile_Down,
        MainIconSetting_Down
	};
        
	ppIcoFun[iCurrIco](NULL, 0, NULL, iCurrIco);

	return 0;
}

static int MainIcoOK_Up(void)
{
    MSGFUNC ppIcoFun[] = {
        MainIconOtdr_Up,
        MainIconVFL_Up,
        MainIconOPM_Up,
		MainIconOPL_Up,
        MainIconFip_Up,
		MainIconRJ45_Up,
        MainIconFile_Up,
        MainIconSetting_Up
	};
	ppIcoFun[iCurrIco](NULL, 0, NULL, iCurrIco);

	return 0;
}

//获得当前的窗体类型
int GetCurrWindowType(void)
{
    return enCurWindow;
}

void FrmMainReCreateWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmMainInit, FrmMainExit, 
						FrmMainPaint, FrmMainLoop, 
						FrmMainPause, FrmMainResume,
						NULL);
}

//otdr窗口切换故障检测窗口清空曲线
static int ClearCurveData(int WndNum)
{
	
	if(WndNum != WndRecordFlag)
	{
		ClearAllCurve();
	}
    
	WndRecordFlag = WndNum;
    
	return 0;
}