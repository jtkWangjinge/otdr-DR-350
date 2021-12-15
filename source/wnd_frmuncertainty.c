/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmuncertainty.c
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
*******************************************************************************/
#include "wnd_frmuncertainty.h"
/*******************************************************************************
*                   定义wnd_frmfac.c引用其他头文件
*******************************************************************************/
#include <stdlib.h>

#include "app_otdrcalibration.h"

#include "guiglobal.h"

#include "wnd_global.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmime.h"

#include "wnd_frmsystemsetting.h"
#include "wnd_frmconfigureinfo.h"
#include "wnd_frmlanguageconfig.h"
#include "wnd_frmlinearity.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmauthorization.h"
//#define UNCERT_DATA_LEN 10

float fData[UNCERT_DATA_LEN] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; 
float fFiberRealLen = 0.0f; 
float fSpecification = 0.0f;

float fDeviation = 0.0f; 
float fFiberTestLen = 0.0f; 
float Uncertainty = 0.0f; 
float fConformance = 0.0f;

extern CURR_WINDOW_TYPE enCurWindow;//当前界面

/*******************************************************************************
*                       窗体frmfac中的窗体控件
*******************************************************************************/
static GUIWINDOW *pFrmUncertainty = NULL;

/*******************************************************************************
*                   窗体frmfac 桌面上的背景控件
*******************************************************************************/
static GUIPICTURE *pFrmUncertaintyLeftBg = NULL;

/*******************************************************************************
*                   窗体frmfac 桌面上的Hz显示控件
*******************************************************************************/

static GUICHAR *pFrmUncertaintyStrTitle = NULL;
static GUILABEL *pFrmUncertaintyLblTitle = NULL;

static GUICHAR *pFrmUncertaintyStrFiberRealLen = NULL;
static GUILABEL *pFrmUncertaintyLblFiberRealLen = NULL;

static GUICHAR *pFrmUncertaintyStrFiberRealLenValue = NULL;
static GUILABEL *pFrmUncertaintyLblFiberRealLenValue = NULL;

static GUICHAR *pFrmUncertaintyStrSpecification = NULL;
static GUILABEL *pFrmUncertaintyLblSpecification = NULL;

static GUICHAR *pFrmUncertaintyStrSpecificationValue = NULL;
static GUILABEL *pFrmUncertaintyLblSpecificationValue = NULL;

static GUICHAR *pFrmUncertaintyStrUncertTitle = NULL;
static GUILABEL *pFrmUncertaintyLblUncertTitle = NULL;

static GUICHAR *pFrmUncertaintyStrUncertData = NULL;
static GUILABEL *pFrmUncertaintyLblUncertData = NULL;

static GUICHAR *pFrmUncertaintyStrDeviation = NULL;
static GUILABEL *pFrmUncertaintyLblDeviation = NULL;

static GUICHAR *pFrmUncertaintyStrDeviationValue = NULL;
static GUILABEL *pFrmUncertaintyLblDeviationValue = NULL;

static GUICHAR *pFrmUncertaintyStrFiberTestLen = NULL;
static GUILABEL *pFrmUncertaintyLblFiberTestLen = NULL;

static GUICHAR *pFrmUncertaintyStrFiberTestLenValue = NULL;
static GUILABEL *pFrmUncertaintyLblFiberTestLenValue = NULL;

static GUICHAR *pFrmUncertaintyStrUncertainty = NULL;
static GUILABEL *pFrmUncertaintyLblUncertainty = NULL;

static GUICHAR *pFrmUncertaintyStrUncertaintyValue = NULL;
static GUILABEL *pFrmUncertaintyLblUncertaintyValue = NULL;

static GUICHAR *pFrmUncertaintyStrConformance = NULL;
static GUILABEL *pFrmUncertaintyLblConformance = NULL;

static GUICHAR *pFrmUncertaintyStrConformanceValue = NULL;
static GUILABEL *pFrmUncertaintyLblConformanceValue = NULL;

static GUICHAR *pFrmUncertaintyStrMeasure = NULL;
static GUILABEL *pFrmUncertaintyLblMeasure = NULL;

static GUICHAR *pFrmUncertaintyStrUnit = NULL;
static GUILABEL *pFrmUncertaintyLblFiberRealLenUnit = NULL;
static GUILABEL *pFrmUncertaintyLblSpecificationUnit = NULL;
static GUILABEL *pFrmUncertaintyLblUncertDataUnit = NULL;
static GUILABEL *pFrmUncertaintyLblDeviationUnit = NULL;
static GUILABEL *pFrmUncertaintyLblFiberTestLenUnit = NULL;
static GUILABEL *pFrmUncertaintyLblUncertaintyUnit = NULL;
static GUILABEL *pFrmUncertaintyLblConformanceUnit = NULL;

static GUIPICTURE *pFrmUncertaintyBtnMeasure = NULL;
static GUIPICTURE *pFrmUncertaintyBgUncertData = NULL;
static GUIPICTURE *pFrmUncertaintyBgFiberRealLen = NULL;
static GUIPICTURE *pFrmUncertaintyBgSpecification = NULL;

/*******************************************************************************
*               窗体frmvls右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFrmUncertaintyMenu = NULL;

/*******************************************************************************
*                   窗体frmvls内部函数声明
*******************************************************************************/
//初始化文本资源
static int FrmUncertaintyTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FrmUncertaintyTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int FrmUncertaintyFiberRealLen_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
static int FrmUncertaintyFiberRealLen_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
                                        
static int FrmUncertaintySpecification_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
static int FrmUncertaintySpecification_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);

static int FrmUncertaintySpecification_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
static int FrmUncertaintySpecification_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
                                        
static int FrmUncertaintyUncertData_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
static int FrmUncertaintyUncertData_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
                                        
static int FrmUncertaintyBtnMeasure_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);
static int FrmUncertaintyBtnMeasure_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen);

static void FrmUncertaintyMenuCallBack(int iOption);
static void ReCreateFactoryUncertaintyWindow(GUIWINDOW** pWnd) ;

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
int FrmUncertaintyInit(void *pWndObj)
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
    pFrmUncertainty = (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FrmUncertaintyTextRes_Init(NULL, 0, NULL, 0);

    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFrmUncertaintyLeftBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factory.bmp");
    pFrmUncertaintyLblTitle = CreateLabel(36, 12, 100, 16, pFrmUncertaintyStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmUncertaintyLblTitle);

    pFrmUncertaintyBtnMeasure = CreatePicture(258, 210, 55, 18, BmpFileDirectory"btn_dialog_unpress.bmp");
    pFrmUncertaintyBgUncertData = CreatePicture(20, 100, 200, 50, BmpFileDirectory"bg_Info.bmp");
    pFrmUncertaintyBgFiberRealLen = CreatePicture(108, 25, 120, 25, BmpFileDirectory"otdr_input_loss_unpress.bmp");
    pFrmUncertaintyBgSpecification = CreatePicture(108, 55, 120, 25, BmpFileDirectory"otdr_input_loss_unpress.bmp");

    pFrmUncertaintyLblFiberRealLen = CreateLabel(10, 28, 100, 16, pFrmUncertaintyStrFiberRealLen);
    pFrmUncertaintyLblFiberRealLenValue = CreateLabel(130, 28, 100, 16, pFrmUncertaintyStrFiberRealLenValue);
    pFrmUncertaintyLblSpecification = CreateLabel(10, 120, 90, 16, pFrmUncertaintyStrSpecification);
    pFrmUncertaintyLblSpecificationValue = CreateLabel(130, 120, 90, 16, pFrmUncertaintyStrSpecificationValue);

    pFrmUncertaintyLblUncertTitle = CreateLabel(10, 80, 200, 16, pFrmUncertaintyStrUncertTitle);
    pFrmUncertaintyLblUncertData = CreateLabel(30, 105, 180, 50, pFrmUncertaintyStrUncertData);

    pFrmUncertaintyLblDeviation = CreateLabel(10, 160, 100, 16, pFrmUncertaintyStrDeviation);
    pFrmUncertaintyLblDeviationValue = CreateLabel(150, 160, 100, 16, pFrmUncertaintyStrDeviationValue);

    pFrmUncertaintyLblFiberTestLen = CreateLabel(10, 180, 150, 16, pFrmUncertaintyStrFiberTestLen);
    pFrmUncertaintyLblFiberTestLenValue = CreateLabel(150, 180, 100, 16, pFrmUncertaintyStrFiberTestLenValue);

    pFrmUncertaintyLblUncertainty = CreateLabel(10, 200, 200, 16, pFrmUncertaintyStrUncertainty);
    pFrmUncertaintyLblUncertaintyValue = CreateLabel(150, 200, 120, 16, pFrmUncertaintyStrUncertaintyValue);

    pFrmUncertaintyLblConformance = CreateLabel(10, 220, 200, 16, pFrmUncertaintyStrConformance);
    pFrmUncertaintyLblConformanceValue = CreateLabel(150, 220, 120, 16, pFrmUncertaintyStrConformanceValue);

    pFrmUncertaintyLblMeasure = CreateLabel(258, 211, 55, 16, pFrmUncertaintyStrMeasure);
    
    pFrmUncertaintyLblFiberRealLenUnit = CreateLabel(230, 28, 30, 16, pFrmUncertaintyStrUnit);
    pFrmUncertaintyLblSpecificationUnit = CreateLabel(230, 58, 30, 16, pFrmUncertaintyStrUnit);
    pFrmUncertaintyLblUncertDataUnit = CreateLabel(230, 115, 30, 16, pFrmUncertaintyStrUnit);
    pFrmUncertaintyLblDeviationUnit = CreateLabel(230, 160, 30, 16, pFrmUncertaintyStrUnit);
    pFrmUncertaintyLblFiberTestLenUnit = CreateLabel(230, 180, 30, 16, pFrmUncertaintyStrUnit);
    pFrmUncertaintyLblUncertaintyUnit = CreateLabel(230, 200, 30, 16, pFrmUncertaintyStrUnit);
    pFrmUncertaintyLblConformanceUnit = CreateLabel(230, 220, 30, 16, pFrmUncertaintyStrUnit);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblFiberRealLen);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmUncertaintyLblFiberRealLenValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblSpecification);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmUncertaintyLblSpecificationValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblUncertTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmUncertaintyLblUncertData);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblDeviation);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblDeviationValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblFiberTestLen);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblFiberTestLenValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblUncertainty);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblUncertaintyValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblConformance);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblConformanceValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblMeasure);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblFiberRealLenUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblSpecificationUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblUncertDataUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblDeviationUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblFiberTestLenUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblUncertaintyUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUncertaintyLblConformanceUnit);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblFiberRealLen);  
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmUncertaintyLblFiberRealLenValue);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblSpecification);  
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmUncertaintyLblSpecificationValue); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblUncertTitle);  
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmUncertaintyLblUncertData);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblDeviation);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblDeviationValue);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblFiberTestLen);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblFiberTestLenValue);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblUncertainty);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblUncertaintyValue);   
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblConformance);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblConformanceValue);  
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmUncertaintyLblMeasure);  
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblFiberRealLenUnit); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblSpecificationUnit); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblUncertDataUnit); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblDeviationUnit); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblFiberTestLenUnit); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblUncertaintyUnit); 
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFrmUncertaintyLblConformanceUnit); 
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFrmUncertaintyMenu = CreateStringWndMenu(FACTORY_MENU_COUNT, sizeof(StrMenu), StrMenu,  0xff00,
								3, 54, FrmUncertaintyMenuCallBack);

    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmUncertainty, pFrmUncertainty);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmUncertaintyBtnMeasure, pFrmUncertainty);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmUncertaintyBgUncertData, pFrmUncertainty);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmUncertaintyBgFiberRealLen, pFrmUncertainty);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmUncertaintyBgSpecification, pFrmUncertainty);
    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFrmUncertaintyMenu, pFrmUncertainty);
	
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	LoginWndMenuToMsg1(pFrmUncertaintyMenu, pFrmUncertainty);
    
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmUncertaintyBgFiberRealLen, 
                    FrmUncertaintyFiberRealLen_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmUncertaintyBgFiberRealLen,
		            FrmUncertaintyFiberRealLen_Up, NULL, 0, pMsg);
		            
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmUncertaintyBgSpecification, 
                    FrmUncertaintySpecification_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmUncertaintyBgSpecification,
		            FrmUncertaintySpecification_Up, NULL, 0, pMsg);
		            
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmUncertaintyBtnMeasure, 
                    FrmUncertaintyBtnMeasure_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmUncertaintyBtnMeasure,
		            FrmUncertaintyBtnMeasure_Up, NULL, 0, pMsg);
		            
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmUncertaintyBgUncertData, 
                    FrmUncertaintyUncertData_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmUncertaintyBgUncertData,
		            FrmUncertaintyUncertData_Up, NULL, 0, pMsg);
    
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
int FrmUncertaintyExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmUncertainty = (GUIWINDOW *) pWndObj;


    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmUncertainty);


    DestroyPicture(&pFrmUncertaintyLeftBg);
    DestroyLabel(&pFrmUncertaintyLblTitle);
	DestroyWndMenu1(&pFrmUncertaintyMenu);

    DestroyPicture(&pFrmUncertaintyBtnMeasure);
    DestroyPicture(&pFrmUncertaintyBgUncertData);
    DestroyPicture(&pFrmUncertaintyBgFiberRealLen);
    DestroyPicture(&pFrmUncertaintyBgSpecification);
    
    DestroyLabel(&pFrmUncertaintyLblFiberRealLen);
    DestroyLabel(&pFrmUncertaintyLblFiberRealLenValue);
    DestroyLabel(&pFrmUncertaintyLblSpecification);
    DestroyLabel(&pFrmUncertaintyLblSpecificationValue);
    DestroyLabel(&pFrmUncertaintyLblUncertTitle);
    DestroyLabel(&pFrmUncertaintyLblUncertData);
    DestroyLabel(&pFrmUncertaintyLblDeviation);
    DestroyLabel(&pFrmUncertaintyLblDeviationValue);
    DestroyLabel(&pFrmUncertaintyLblFiberTestLen);
    DestroyLabel(&pFrmUncertaintyLblFiberTestLenValue);
    DestroyLabel(&pFrmUncertaintyLblUncertainty);
    DestroyLabel(&pFrmUncertaintyLblUncertaintyValue);
    DestroyLabel(&pFrmUncertaintyLblConformance);
    DestroyLabel(&pFrmUncertaintyLblConformanceValue);
    DestroyLabel(&pFrmUncertaintyLblMeasure);
    DestroyLabel(&pFrmUncertaintyLblFiberRealLenUnit);
    DestroyLabel(&pFrmUncertaintyLblSpecificationUnit);
    DestroyLabel(&pFrmUncertaintyLblUncertDataUnit);
    DestroyLabel(&pFrmUncertaintyLblDeviationUnit);
    DestroyLabel(&pFrmUncertaintyLblFiberTestLenUnit);
    DestroyLabel(&pFrmUncertaintyLblUncertaintyUnit);
    DestroyLabel(&pFrmUncertaintyLblConformanceUnit);

    FrmUncertaintyTextRes_Exit(NULL, 0, NULL, 0);

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
int FrmUncertaintyPaint(void *pWndObj)
{
    int iRet = 0;
    
	DisplayPicture(pFrmUncertaintyLeftBg);
    DisplayLabel(pFrmUncertaintyLblTitle);

    DisplayPicture(pFrmUncertaintyBtnMeasure);
    DisplayPicture(pFrmUncertaintyBgUncertData);
    DisplayPicture(pFrmUncertaintyBgFiberRealLen);
    DisplayPicture(pFrmUncertaintyBgSpecification);
    
    DisplayLabel(pFrmUncertaintyLblFiberRealLen);
    DisplayLabel(pFrmUncertaintyLblFiberRealLenValue);
    DisplayLabel(pFrmUncertaintyLblSpecification);
    DisplayLabel(pFrmUncertaintyLblSpecificationValue);
    DisplayLabel(pFrmUncertaintyLblUncertTitle);
    DisplayLabel(pFrmUncertaintyLblUncertData);
    DisplayLabel(pFrmUncertaintyLblDeviation);
    DisplayLabel(pFrmUncertaintyLblDeviationValue);
    DisplayLabel(pFrmUncertaintyLblFiberTestLen);
    DisplayLabel(pFrmUncertaintyLblFiberTestLenValue);
    DisplayLabel(pFrmUncertaintyLblUncertainty);
    DisplayLabel(pFrmUncertaintyLblUncertaintyValue);
    DisplayLabel(pFrmUncertaintyLblConformance);
    DisplayLabel(pFrmUncertaintyLblConformanceValue);
    DisplayLabel(pFrmUncertaintyLblMeasure);
    DisplayLabel(pFrmUncertaintyLblFiberRealLenUnit);
    DisplayLabel(pFrmUncertaintyLblSpecificationUnit);
    DisplayLabel(pFrmUncertaintyLblUncertDataUnit);
    DisplayLabel(pFrmUncertaintyLblDeviationUnit);
    DisplayLabel(pFrmUncertaintyLblFiberTestLenUnit);
    DisplayLabel(pFrmUncertaintyLblUncertaintyUnit);
    DisplayLabel(pFrmUncertaintyLblConformanceUnit);

	DisplayWndMenu1(pFrmUncertaintyMenu);
    
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
int FrmUncertaintyLoop(void *pWndObj)
{
	//错误标志、返回值定义
    int iRet = 0;
	SendWndMsg_LoopDisable(pWndObj);
    return iRet;
}

int FrmUncertaintyPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

int FrmUncertaintyResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

static int FrmUncertaintyTextRes_Init(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    char buff[128] = {0};
    pFrmUncertaintyStrTitle = TransString("Uncertainty");
    
    pFrmUncertaintyStrFiberRealLen = TransString("Fiber real length:");
    pFrmUncertaintyStrSpecification = TransString("Specification:");
    pFrmUncertaintyStrUncertTitle = TransString("Uncert Data(Please enter the length of the 10 measurements, separated by commas):");
    pFrmUncertaintyStrDeviation = TransString("Deviation:");
    pFrmUncertaintyStrFiberTestLen = TransString("Fiber test length:");
    pFrmUncertaintyStrUncertainty = TransString("Uncertainty:");
    pFrmUncertaintyStrConformance = TransString("Conformance:");
    pFrmUncertaintyStrMeasure = TransString("Measure"); 
    pFrmUncertaintyStrUnit = TransString("m"); 
    sprintf(buff,"%.2f",fFiberRealLen);
    pFrmUncertaintyStrFiberRealLenValue = TransString(buff);
    sprintf(buff,"%.2f",fSpecification);
    pFrmUncertaintyStrSpecificationValue = TransString(buff);
    sprintf(buff,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
                ,fData[0],fData[1],fData[2],fData[3],fData[4],fData[5]
                ,fData[6],fData[7],fData[8],fData[9]);
    pFrmUncertaintyStrUncertData = TransString(buff); 
    sprintf(buff,"%.2f",fDeviation);
    pFrmUncertaintyStrDeviationValue = TransString(buff);
    sprintf(buff,"%.2f",fFiberTestLen);
    pFrmUncertaintyStrFiberTestLenValue = TransString(buff);
    sprintf(buff,"%.2f",Uncertainty);
    pFrmUncertaintyStrUncertaintyValue = TransString(buff);
    sprintf(buff,"%.2f",fConformance);
    pFrmUncertaintyStrConformanceValue = TransString(buff);

    return 0;
}

static int FrmUncertaintyTextRes_Exit(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    GuiMemFree(pFrmUncertaintyStrTitle);
    
    GuiMemFree(pFrmUncertaintyStrFiberRealLen);
    GuiMemFree(pFrmUncertaintyStrFiberRealLenValue);
    GuiMemFree(pFrmUncertaintyStrSpecification);
    GuiMemFree(pFrmUncertaintyStrSpecificationValue);
    GuiMemFree(pFrmUncertaintyStrUncertTitle);
    GuiMemFree(pFrmUncertaintyStrUncertData);
    GuiMemFree(pFrmUncertaintyStrDeviation);
    GuiMemFree(pFrmUncertaintyStrDeviationValue);
    GuiMemFree(pFrmUncertaintyStrFiberTestLen);
    GuiMemFree(pFrmUncertaintyStrFiberTestLenValue);
    GuiMemFree(pFrmUncertaintyStrUncertainty);
    GuiMemFree(pFrmUncertaintyStrUncertaintyValue);
    GuiMemFree(pFrmUncertaintyStrConformance);
    GuiMemFree(pFrmUncertaintyStrConformanceValue);
    GuiMemFree(pFrmUncertaintyStrMeasure);
    GuiMemFree(pFrmUncertaintyStrUnit);

	return 0;
}

static void inputFiberRealLenCallBack(void)
{
    char buff[64] = {0};
    GetIMEInputBuff(buff);
    
    fFiberRealLen = atof(buff);

    LOG(LOG_INFO, "fFiberRealLen %f buff = %s\n", fFiberRealLen, buff);
}

static void inputSpecificationCallBack(void)
{
    char buff[64] = {0};
    GetIMEInputBuff(buff);

    LOG(LOG_INFO, "Specification: %s\n", buff);
    fSpecification = atof(buff);
}

static void inputUncertDataCallBack(void)
{
    char *p = NULL;
    char buff[128];
    int k = 0;
    
    GetIMEInputBuff(buff);
    LOG(LOG_INFO, "Uncert Data: %s\n", buff);

    p = strtok(buff,",");
    while(p!=NULL)
    {
        fData[k++] =  atof(p);
        p = strtok(NULL,",");
        if(k == UNCERT_DATA_LEN)
        {
            break;
        }
    }
}

static int FrmUncertaintyFiberRealLen_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}
static int FrmUncertaintyFiberRealLen_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    char buff[32] = {0};
    sprintf(buff,"%.2f",fFiberRealLen);
    IMEInit(buff, 20, 0, ReCreateFactoryUncertaintyWindow, inputFiberRealLenCallBack, NULL);

    return iReturn;
}

static int FrmUncertaintySpecification_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}

static int FrmUncertaintySpecification_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    char buff[32] = {0};
    sprintf(buff,"%.2f",fSpecification);
    IMEInit(buff, 20, 0, ReCreateFactoryUncertaintyWindow, inputSpecificationCallBack, NULL);

    return iReturn;
}
                               
static int FrmUncertaintyUncertData_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}
static int FrmUncertaintyUncertData_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    char buff[128] = {0};
    sprintf(buff,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
                    fData[0],fData[1],fData[2],fData[3],fData[4],
                    fData[5],fData[6],fData[7],fData[8],fData[9]);
    IMEInit(buff, 100, 0, ReCreateFactoryUncertaintyWindow, inputUncertDataCallBack, NULL);

    return iReturn;
}
                                        
static int FrmUncertaintyBtnMeasure_Down(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    TouchChange("btn_dialog_press.bmp", pFrmUncertaintyBtnMeasure, NULL, NULL, 1);
    
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}
static int FrmUncertaintyBtnMeasure_Up(void *pInArg,   int iInLen, 
                                        void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    char buff[16] = {0};
    TouchChange("btn_dialog_unpress.bmp", pFrmUncertaintyBtnMeasure, pFrmUncertaintyStrMeasure, pFrmUncertaintyLblMeasure, 3);
    SetPictureEnable(0, pFrmUncertaintyBtnMeasure);   
	RefreshScreen(__FILE__, __func__, __LINE__);

	CalcUncertainty(fData,fFiberRealLen,&fDeviation,&fFiberTestLen,&Uncertainty,&fConformance,fSpecification);

	GuiMemFree(pFrmUncertaintyStrDeviationValue);
	GuiMemFree(pFrmUncertaintyStrFiberTestLenValue);
	GuiMemFree(pFrmUncertaintyStrUncertaintyValue);
	GuiMemFree(pFrmUncertaintyStrConformanceValue);
    sprintf(buff,"%.2f",fDeviation);
	pFrmUncertaintyStrDeviationValue = TransString(buff);
    sprintf(buff,"%.2f",fFiberTestLen);
	pFrmUncertaintyStrFiberTestLenValue = TransString(buff);
    sprintf(buff,"%.2f",Uncertainty);
	pFrmUncertaintyStrUncertaintyValue = TransString(buff);
    sprintf(buff,"%.2f",fConformance);
	pFrmUncertaintyStrConformanceValue = TransString(buff);


	SetLabelText(pFrmUncertaintyStrDeviationValue, pFrmUncertaintyLblDeviationValue);
	SetLabelText(pFrmUncertaintyStrFiberTestLenValue, pFrmUncertaintyLblFiberTestLenValue);
	SetLabelText(pFrmUncertaintyStrUncertaintyValue, pFrmUncertaintyLblUncertaintyValue);
	SetLabelText(pFrmUncertaintyStrConformanceValue, pFrmUncertaintyLblConformanceValue);
	
    SetPictureEnable(1, pFrmUncertaintyBtnMeasure);   
	TouchChange("btn_dialog_unpress.bmp", pFrmUncertaintyBtnMeasure, pFrmUncertaintyStrMeasure, pFrmUncertaintyLblMeasure, 1);
    FrmUncertaintyPaint(NULL);
    
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}

//菜单栏响应处理函数
static void FrmUncertaintyMenuCallBack(int iOption)
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
	    SendWndMsg_WindowExit(pFrmUncertainty);  
	    SendSysMsg_ThreadCreate(pWnd); 

        break;
	case LANGUAGE_SET:
		enCurWindow = ENUM_LANGUAGE_CONFIG_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmLanguageConfigInit, FrmLanguageConfigExit,
			FrmLanguageConfigPaint, FrmLanguageConfigLoop,
			FrmLanguageConfigPause, FrmLanguageConfigResume,
			NULL);
		SendWndMsg_WindowExit(pFrmUncertainty);
		SendSysMsg_ThreadCreate(pWnd);
        break;
	case NON_LINEARITY:
		enCurWindow = ENUM_NON_LINEARITY_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmLinearityInit, FrmLinearityExit,
			FrmLinearityPaint, FrmLinearityLoop,
			FrmLinearityPause, FrmLinearityResume,
			NULL);
		SendWndMsg_WindowExit(pFrmUncertainty);
		SendSysMsg_ThreadCreate(pWnd);
        break;
    case UNCERTAINTY:
        break;
	case FACTORY_SET:
		enCurWindow = ENUM_FACTORY_SET_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmFactorySetInit, FrmFactorySetExit,
			FrmFactorySetPaint, FrmFactorySetLoop,
			FrmFactorySetPause, FrmFactorySetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmUncertainty);
		SendSysMsg_ThreadCreate(pWnd);
        break;
	case AUTHORIZATION:
		enCurWindow = ENUM_AUTHORIZATION_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmAuthorizationInit, FrmAuthorizationExit,
			FrmAuthorizationPaint, FrmAuthorizationLoop,
			FrmAuthorizationPause, FrmAuthorizationResume,
			NULL);
		SendWndMsg_WindowExit(pFrmUncertainty);
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
	    SendWndMsg_WindowExit(pFrmUncertainty);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}
//重新绘制窗体
static void ReCreateFactoryUncertaintyWindow(GUIWINDOW** pWnd) 
{
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    					   FrmUncertaintyInit, FrmUncertaintyExit, 
    					   FrmUncertaintyPaint, FrmUncertaintyLoop, 
    					   FrmUncertaintyPause, FrmUncertaintyResume,
    					   NULL);
}

//enter键处理响应函数
static void KeyEnterCallBack(int* iSelected)
{
	switch (*iSelected)
	{
	case 0://real fiber length
		*iSelected = 0;
		FrmUncertaintyFiberRealLen_Down(NULL, 0, NULL, 0);
		FrmUncertaintyFiberRealLen_Up(NULL, 0, NULL, 0);
		break;
	case 1://specified
		*iSelected = 0;
		FrmUncertaintySpecification_Down(NULL, 0, NULL, 0);
		FrmUncertaintySpecification_Up(NULL, 0, NULL, 0);
		break;
	case 2://uncertainty parameter
		*iSelected = 0;
 		FrmUncertaintyUncertData_Down(NULL, 0, NULL, 0);
 		FrmUncertaintyUncertData_Up(NULL, 0, NULL, 0);
		break;
	case 3://measure
		*iSelected = 0;
  		FrmUncertaintyBtnMeasure_Down(NULL, 0, NULL, 0);
  		FrmUncertaintyBtnMeasure_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}

//按键响应处理函数
void UncertaintyKeyCallBack(int iOption)
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
		KeyEnterCallBack(&iKeyFlag);
		break;
	default:
		break;
	}
}


