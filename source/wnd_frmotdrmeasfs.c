/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrmeas.c
* 摘    要：  实现全屏时frmotdrmeas的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.1
* 作    者：  	
* 完成日期：  2014-12-19
*
* 取代版本：  v1.0.0
* 原 作 者：	
* 完成日期：  2014-12-19
**************************************************************/
#include "wnd_frmotdrmeas.h"


/**************************************************************
* 	为实现窗体frmotdrmeas而需要引用的其他头文件	      	    *
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmmain.h"
#include "app_frminit.h"
#include "app_frminit.h"
#include "app_curve.h"
#include "wnd_frmmark.h"
#include "wnd_frmdialog.h"
#include "guiphoto.h"
#ifdef MINI2
#include "app_algorithm_support.h"
#endif
#include "app_unitconverter.h"
/**************************************************************
* 		窗体frmotdrmeas中引用的变量				*
**************************************************************/
extern POTDR_TOP_SETTINGS pOtdrTopSettings;
static PDISPLAY_INFO pDisplay  = NULL;

extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//设置顶层结构
static GUICHAR *pOtdrMeasStrLoss_Full = NULL; 			//损耗信息文本
static GUICHAR *pOtdrMeasStrAttenuation_Full = NULL; 	//增益信息文本
static GUICHAR *pOtdrMeasStrREF_Full = NULL; 			//反射率信息文本
static GUICHAR *pOtdrMeasStrORL_Full = NULL; 			//光回损信息文本
static GUICHAR *pOtdrMeasStrLossTitle_Full = NULL; 		//损耗选项卡文本
static GUICHAR *pOtdrMeasStrLossVal_Full = NULL; 
static GUICHAR *pOtdrMeasStrAtteTitle_Full = NULL; 		//增益选项卡文本
static GUICHAR *pOtdrMeasStrAtteVal_Full = NULL; 
static GUICHAR *pOtdrMeasStrRefTitle_Full = NULL; 		//反射率选项卡文本
static GUICHAR *pOtdrMeasStrRefVal_Full = NULL; 
static GUICHAR *pOtdrMeasStrORLVal1_Full = NULL; 		//光回损选项卡文本
static GUICHAR *pOtdrMeasStrORLVal2_Full = NULL; 
static GUICHAR *pOtdrMeasStrORLInfo1_Full = NULL; 
static GUICHAR *pOtdrMeasStrORLInfo2_Full = NULL; 
static GUICHAR *pOtdrMeasStrMarkNum_Full[3] = {};
static GUICHAR *pOtdrMeasStrMark1Dist_Full = NULL; 
static GUICHAR *pOtdrMeasStrMark1dB_Full = NULL; 
static GUICHAR *pOtdrMeasStrMark2Dist_Full = NULL; 
static GUICHAR *pOtdrMeasStrMark2dB_Full = NULL; 
static GUICHAR *pOtdrMeasStrMark21Dist_Full = NULL; 
static GUICHAR *pOtdrMeasStrMark21dB_Full = NULL; 


/*********************************全屏界面信息*********************************/
static GUIPICTURE *pOtdrMeasBg_Full = NULL;

static GUIPICTURE *pOtdrMeasBtnLoss_Full = NULL; 		//信息选项按钮
static GUIPICTURE *pOtdrMeasBtnAttenuation_Full = NULL; 
static GUIPICTURE *pOtdrMeasBtnREF_Full = NULL; 
static GUIPICTURE *pOtdrMeasBtnORL_Full = NULL; 

static GUILABEL *pOtdrMeasLblLoss_Full = NULL; 			//信息数据标签
static GUILABEL *pOtdrMeasLblAttenuation_Full = NULL; 
static GUILABEL *pOtdrMeasLblREF_Full = NULL; 
static GUILABEL *pOtdrMeasLblORL_Full = NULL; 
static GUILABEL *pOtdrMeasLblLossVal_Full = NULL; 
static GUILABEL *pOtdrMeasLblAtteVal_Full = NULL; 
static GUILABEL *pOtdrMeasLblRefVal_Full = NULL; 
static GUILABEL *pOtdrMeasLblORLVal1_Full = NULL; 		//光回损标签
static GUILABEL *pOtdrMeasLblORLVal2_Full = NULL; 

static GUIPICTURE *pOtdrMeasBtnInfoField_Full = NULL;	//背景

static GUILABEL *pOtdrMeasLblMarkNum_Full[3] = {};
static GUILABEL *pOtdrMeasLblMark1Dist_Full = NULL; 
static GUILABEL *pOtdrMeasLblMark1dB_Full = NULL; 
static GUILABEL *pOtdrMeasLblMark2Dist_Full = NULL; 
static GUILABEL *pOtdrMeasLblMark2dB_Full = NULL; 
static GUILABEL *pOtdrMeasLblMark21Dist_Full = NULL; 
static GUILABEL *pOtdrMeasLblMark21dB_Full = NULL; 

static MARKER mMarkerF;

/**************************************************************
* 				声明frmotdrmeas.c中内部使用的函数
**************************************************************/
static int FullMeasBtnLoss_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int FullMeasBtnAttenuation_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int FullMeasBtnREF_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int FullMeasBtnORL_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);


/******************初始化文本资源函数声明*************/
static int FullMeasTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
/******************释放文本资源函数声明****************/
static int FullMeasTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void AddFullMeasToComp(GUIWINDOW *pWndObj);
static int DelFullMeasComp(GUIWINDOW *pWndObj);
static void LoginFullMeasMsg(WNDOTDRMEAS *pMeasWnd);
static void LogoutFullMeasMsg(void);
static void FullMeasWndPaint(WNDOTDRMEAS *pWnd);

/*******************************************************************************
* 					定义frmotdrmeas.c中内部使用的函数						   *
*******************************************************************************/

/******************************全屏函数定义************************************/



//改变损耗等的显示
static void ChangeMeasInfo(WNDOTDRMEAS *pMeasObj, int iCurr)
{
	GUIPICTURE *pMeas[] = {
		pOtdrMeasBtnLoss_Full,
		pOtdrMeasBtnAttenuation_Full,
		pOtdrMeasBtnREF_Full,
		pOtdrMeasBtnORL_Full
	};

	GUILABEL *pMeasLbl[] = {
		pOtdrMeasLblLoss_Full,
		pOtdrMeasLblAttenuation_Full,
		pOtdrMeasLblREF_Full,
		pOtdrMeasLblORL_Full
	};
    char unpressBuff[512] = {};
    sprintf(unpressBuff, "%s%d%s", BmpFileDirectory"btn_fullscreen_meas_exl_unpress", pMeasObj->eMeasCard, ".bmp");
	SetPictureBitmap(unpressBuff, pMeas[pMeasObj->eMeasCard]);
    char pressBuff[512] = {};
    sprintf(pressBuff, "%s%d%s", BmpFileDirectory"btn_fullscreen_meas_exl_press", iCurr, ".bmp");
	SetPictureBitmap(pressBuff, pMeas[iCurr]);
	
	DisplayPicture(pMeas[pMeasObj->eMeasCard]);
	DisplayPicture(pMeas[iCurr]);
	DisplayLabel(pMeasLbl[pMeasObj->eMeasCard]);
	DisplayLabel(pMeasLbl[iCurr]);

	pMeasObj->eMeasCard = iCurr;
    SendMakerCtlMsg(EN_MAKER_MEAS, iCurr);
}


/***
  * 功能：
       		 全屏下pOtdrMeasBtnLoss 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
       		 成功返回零，失败返回非零值
  * 备注：
***/
static int FullMeasBtnLoss_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
    WNDOTDRMEAS *this = (WNDOTDRMEAS *)pOutArg;
    
    if(this->eMeasCard == EN_MEAS_LOSS)
    {
        pUser_Settings->sCommonSetting.iLossMethodMark = pUser_Settings->sCommonSetting.iLossMethodMark ? 0 : 1;
        DISPLAY_PARA CtlPara;

    	GetDisplayCtlPara(pOtdrTopSettings->pDisplayInfo, &CtlPara);
    	
    	if (pUser_Settings->sCommonSetting.iLossMethodMark)
    		CtlPara.enMethd = EN_METHD_TPA;
    	else
    		CtlPara.enMethd = EN_METHD_LSA;
    		    		
    	SetDisplayCtlPara(pOtdrTopSettings->pDisplayInfo, &CtlPara);
    }
    
	DisplayPicture(pOtdrMeasBtnInfoField_Full);
	DisplayLabel(pOtdrMeasLblLossVal_Full); 
	SetMarerCurrentNum(EN_MEAS_LOSS);    
	ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_LOSS);
	
	return iReturn;
}


/***
  * 功能：
       		 全屏下pOtdrMeasBtnAttenuation按钮按下处理函数
  * 参数：
       		 无
  * 返回：
       		 成功返回零，失败返回非零值
  * 备注：
***/
static int FullMeasBtnAttenuation_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	DisplayPicture(pOtdrMeasBtnInfoField_Full);
	DisplayLabel(pOtdrMeasLblAtteVal_Full);
	SetMarerCurrentNum(EN_MEAS_ATTE);
	ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_ATTE);
    
	return iReturn;
}


/***
  * 功能：
       		 全屏下pOtdrMeasBtnREF 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
       		 成功返回零，失败返回非零值
  * 备注：
***/
static int FullMeasBtnREF_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	DisplayPicture(pOtdrMeasBtnInfoField_Full);
	DisplayLabel(pOtdrMeasLblRefVal_Full);
	SetMarerCurrentNum(EN_MEAS_REFC);
	ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_REFC);
    
	return iReturn;
}


/***
  * 功能：
       		 全屏下pOtdrMeasBtnORL 按钮按下处理函数
  * 参数：
       		 无
  * 返回：
       		 成功返回零，失败返回非零值
  * 备注：
***/
static int FullMeasBtnORL_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	DisplayPicture(pOtdrMeasBtnInfoField_Full);
	DisplayLabel(pOtdrMeasLblORLVal1_Full);
	DisplayLabel(pOtdrMeasLblORLVal2_Full);
	SetMarerCurrentNum(EN_MEAS_ORL);
	ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_ORL);

	return iReturn;
}

/***
  * 功能：
        窗体frmotdrmeas的初始化函数，建立窗体控件、
        注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
WNDOTDRMEAS * CreateFullMeasWnd(void)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	WNDOTDRMEAS *pMeas = NULL;
		
	//分配资源
	pMeas = (WNDOTDRMEAS *)calloc(1, sizeof(WNDOTDRMEAS));
	if (NULL == pMeas)
	{
		LOG(LOG_ERROR, "Menu Malloc ERR\n");
		iReturn = -2;
	}

	//获取显示对象以及标记信息
	pDisplay = pOtdrTopSettings->pDisplayInfo;
	GetCurveMarker(CURR_CURVE, pDisplay, &mMarkerF);
	
	//初始化结构体
	pMeas->eMeasCard = mMarkerF.enMeas;//test
	pMeas->enPitchOn = mMarkerF.enPitchOn;
	pMeas->iEnable = 0;
	
	//初始化文本资源
	FullMeasTextRes_Init(pMeas, 0, NULL, 0);
	
	/********************************创建背景控件******************************/
    pOtdrMeasBg_Full= CreatePhoto("bg_fullscreen_meas");
	/***************************************************************************
	**				       窗体frmotdrmeas中下部左侧控件                      **
	***************************************************************************/

	//左侧表格信息刷新背景图片
	pOtdrMeasLblMarkNum_Full[0] = CreateLabel(20,403,50,24,
									    pOtdrMeasStrMarkNum_Full[0]);
	pOtdrMeasLblMarkNum_Full[1] = CreateLabel(20,429,50,24,   
									    pOtdrMeasStrMarkNum_Full[1]);
	pOtdrMeasLblMarkNum_Full[2] = CreateLabel(20,459,50,24, 
										pOtdrMeasStrMarkNum_Full[2]);
	
	pOtdrMeasLblMark1Dist_Full = CreateLabel(120,403,95,24,  
									  pOtdrMeasStrMark1Dist_Full);
	pOtdrMeasLblMark1dB_Full = CreateLabel(220,403,95,24,   
									  pOtdrMeasStrMark1dB_Full);
	pOtdrMeasLblMark2Dist_Full = CreateLabel(120,429,95,24,   
									  pOtdrMeasStrMark2Dist_Full);
	pOtdrMeasLblMark2dB_Full = CreateLabel(220,429,95,24,   
									  pOtdrMeasStrMark2dB_Full);
	pOtdrMeasLblMark21Dist_Full = CreateLabel(120,458,95,24,   
									  pOtdrMeasStrMark21Dist_Full);
	pOtdrMeasLblMark21dB_Full = CreateLabel(220,458,95,24,
										pOtdrMeasStrMark21dB_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrMeasLblMarkNum_Full[0]);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrMeasLblMarkNum_Full[1]);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrMeasLblMarkNum_Full[2]);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark1Dist_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark2Dist_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark1dB_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark2dB_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark21dB_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark21Dist_Full);

	/***************************************************************************
	**				       窗体frmotdrmeas中下部右侧控件                      **
	***************************************************************************/
	
    pOtdrMeasBtnInfoField_Full = CreatePhoto("bg_full_meas_info_field");

	pOtdrMeasLblLoss_Full = CreateLabel(373, 441, 65, 24, pOtdrMeasStrLoss_Full);	
	pOtdrMeasLblAttenuation_Full = CreateLabel(438, 441, 65, 24, pOtdrMeasStrAttenuation_Full);	
	pOtdrMeasLblREF_Full = CreateLabel(503, 441, 97, 24, pOtdrMeasStrREF_Full);
	pOtdrMeasLblORL_Full = CreateLabel(600, 441, 68, 24, pOtdrMeasStrORL_Full);
	pOtdrMeasLblLossVal_Full = CreateLabel(341,405,334,24, pOtdrMeasStrLossVal_Full);
	pOtdrMeasLblAtteVal_Full = CreateLabel(341,405,334,24, pOtdrMeasStrAtteVal_Full);
	pOtdrMeasLblRefVal_Full = CreateLabel(341,405,334,24, pOtdrMeasStrRefVal_Full);
	pOtdrMeasLblORLVal1_Full = CreateLabel(378,405,130,24, pOtdrMeasStrORLVal1_Full);
	pOtdrMeasLblORLVal2_Full = CreateLabel(520,405,130,24, pOtdrMeasStrORLVal2_Full);

	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblLoss_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblAttenuation_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORL_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblREF_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblLossVal_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblAtteVal_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblRefVal_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORLVal1_Full);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORLVal2_Full);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblLoss_Full);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblAttenuation_Full);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblORL_Full);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblREF_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblLossVal_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblAtteVal_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblRefVal_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblORLVal1_Full);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblORLVal2_Full);

    pOtdrMeasBtnLoss_Full = CreatePhoto("btn_fullscreen_meas_exl_unpress0");
    pOtdrMeasBtnAttenuation_Full = CreatePhoto("btn_fullscreen_meas_exl_unpress1");
    pOtdrMeasBtnREF_Full = CreatePhoto("btn_fullscreen_meas_exl_unpress2");
    pOtdrMeasBtnORL_Full = CreatePhoto("btn_fullscreen_meas_exl_unpress3");

	if(pMeas->eMeasCard == EN_MEAS_LOSS)
	{
        SetPictureBitmap(BmpFileDirectory"btn_fullscreen_meas_exl_press0.bmp",
						pOtdrMeasBtnLoss_Full);
	}
	else if(pMeas->eMeasCard == EN_MEAS_ATTE)
	{
        SetPictureBitmap(BmpFileDirectory"btn_fullscreen_meas_exl_press1.bmp",
						pOtdrMeasBtnAttenuation_Full);
	}
	else if(pMeas->eMeasCard == EN_MEAS_REFC)
	{
        SetPictureBitmap(BmpFileDirectory"btn_fullscreen_meas_exl_press2.bmp",
						pOtdrMeasBtnREF_Full);
	}
	else if(pMeas->eMeasCard == EN_MEAS_ORL)
	{
        SetPictureBitmap(BmpFileDirectory"btn_fullscreen_meas_exl_press3.bmp",
						pOtdrMeasBtnORL_Full);
	}

	return pMeas;
}

/***
  * 功能：
        窗体frmotdrmeas的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DestroyFullMeasWnd(WNDOTDRMEAS *pMeas)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	/***********************销毁背景窗体控件***********************************/
	DestroyPicture(&pOtdrMeasBg_Full);

	/************************销毁下部右侧选项卡控件****************************/
	DestroyPicture(&pOtdrMeasBtnLoss_Full);
	DestroyPicture(&pOtdrMeasBtnAttenuation_Full);
	DestroyPicture(&pOtdrMeasBtnREF_Full);
	DestroyPicture(&pOtdrMeasBtnORL_Full);
	DestroyLabel(&pOtdrMeasLblLoss_Full);
	DestroyLabel(&pOtdrMeasLblAttenuation_Full);
	DestroyLabel(&pOtdrMeasLblREF_Full);
	DestroyLabel(&pOtdrMeasLblORL_Full);
	
	/**************************销毁下部右侧控件********************************/
	DestroyPicture(&pOtdrMeasBtnInfoField_Full);
	
	DestroyLabel(&pOtdrMeasLblLossVal_Full); 
	DestroyLabel(&pOtdrMeasLblAtteVal_Full);
	DestroyLabel(&pOtdrMeasLblRefVal_Full);
	
	DestroyLabel(&pOtdrMeasLblORLVal1_Full);
	DestroyLabel(&pOtdrMeasLblORLVal2_Full);

	DestroyLabel(&pOtdrMeasLblMark1Dist_Full);
	DestroyLabel(&pOtdrMeasLblMark1dB_Full);
	DestroyLabel(&pOtdrMeasLblMark2Dist_Full);
	DestroyLabel(&pOtdrMeasLblMark2dB_Full);
	DestroyLabel(&pOtdrMeasLblMark21Dist_Full);
	DestroyLabel(&pOtdrMeasLblMark21dB_Full);
    
	for(i = 0; i < 3; i++)
	{
		DestroyLabel(&pOtdrMeasLblMarkNum_Full[i]);
	}
	//释放结构体
	GuiMemFree(pMeas);
	
	FullMeasTextRes_Exit(NULL, 0, NULL, 0);

	
    return iReturn;
}

static void FullMeasWndPaint(WNDOTDRMEAS *pWnd)
{
	/***********************显示背景窗体控件***********************************/
	DisplayPicture(pOtdrMeasBg_Full);

	/**************************显示下部左侧控件********************************/
	DisplayPicture(pOtdrMeasBtnLoss_Full);
	DisplayPicture(pOtdrMeasBtnAttenuation_Full);
	DisplayPicture(pOtdrMeasBtnREF_Full);
    DisplayPicture(pOtdrMeasBtnORL_Full);

	DisplayLabel(pOtdrMeasLblMarkNum_Full[0]);
	DisplayLabel(pOtdrMeasLblMarkNum_Full[1]);
	DisplayLabel(pOtdrMeasLblMarkNum_Full[2]);

	/**************************显示下部右侧控件********************************/
	DisplayLabel(pOtdrMeasLblLoss_Full);
	DisplayLabel(pOtdrMeasLblAttenuation_Full);
	DisplayLabel(pOtdrMeasLblREF_Full);
	DisplayLabel(pOtdrMeasLblORL_Full);

	DisplayLabel(pOtdrMeasLblMark1Dist_Full);
	DisplayLabel(pOtdrMeasLblMark1dB_Full);
	DisplayLabel(pOtdrMeasLblMark2Dist_Full);
	DisplayLabel(pOtdrMeasLblMark2dB_Full);
	DisplayLabel(pOtdrMeasLblMark21Dist_Full);
	DisplayLabel(pOtdrMeasLblMark21dB_Full);

	DisplayPicture(pOtdrMeasBtnInfoField_Full);
	if(pWnd->eMeasCard == EN_MEAS_LOSS)
	{
		DisplayLabel(pOtdrMeasLblLossVal_Full); 		
	}
	else if(pWnd->eMeasCard == EN_MEAS_ATTE)
	{
		DisplayLabel(pOtdrMeasLblAtteVal_Full);
	}
	else if(pWnd->eMeasCard == EN_MEAS_REFC)
	{
		DisplayLabel(pOtdrMeasLblRefVal_Full);
	}
	else if(pWnd->eMeasCard == EN_MEAS_ORL)
	{
		DisplayLabel(pOtdrMeasLblORLVal1_Full);
		DisplayLabel(pOtdrMeasLblORLVal2_Full);
	}

    SendMakerCtlMsg(EN_MAKER_MEAS, pWnd->eMeasCard);
	SetMarerCurrentNum(pWnd->eMeasCard);

}


/***
  * 功能：
        		窗体frmotdrmeas的循环函数，进行窗体循环
  * 参数：
        		1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/

int LoopFullMeasWnd(WNDOTDRMEAS *pWnd)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	//临时变量定义
	float fDistAB, fdBAB;
    float fTmpVal = 0.0f;
	//获得标记信息
    //若没有曲线，则返回，让界面上的内容保持默认的空值
    if (GetCurveMarker(CURR_CURVE, pDisplay, &mMarkerF) == 1)
        return -1;
	fDistAB = mMarkerF.dDot[2].fDist - mMarkerF.dDot[1].fDist;
	fdBAB = mMarkerF.dDot[2].iValue - mMarkerF.dDot[1].iValue;

    GuiMemFree(pOtdrMeasStrMark1Dist_Full);
    pOtdrMeasStrMark1Dist_Full = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
        mMarkerF.dDot[1].fDist, 1);
    SetLabelText(pOtdrMeasStrMark1Dist_Full, pOtdrMeasLblMark1Dist_Full);
    DisplayLabel(pOtdrMeasLblMark1Dist_Full);

    GuiMemFree(pOtdrMeasStrMark2Dist_Full);
    pOtdrMeasStrMark2Dist_Full = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
        mMarkerF.dDot[2].fDist, 1);
    SetLabelText(pOtdrMeasStrMark2Dist_Full, pOtdrMeasLblMark2Dist_Full);
    DisplayLabel(pOtdrMeasLblMark2Dist_Full);

    GuiMemFree(pOtdrMeasStrMark21Dist_Full);
    pOtdrMeasStrMark21Dist_Full = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
        fDistAB, 1);
    SetLabelText(pOtdrMeasStrMark21Dist_Full, pOtdrMeasLblMark21Dist_Full);
    DisplayLabel(pOtdrMeasLblMark21Dist_Full);

	GuiMemFree(pOtdrMeasStrMark1dB_Full);
	pOtdrMeasStrMark1dB_Full = Float2GuiStringUnit(LOSS_PRECISION, 
        (float)mMarkerF.dDot[1].iValue / SCALE_FACTOR, "dB");
	SetLabelText(pOtdrMeasStrMark1dB_Full, pOtdrMeasLblMark1dB_Full);
	DisplayLabel(pOtdrMeasLblMark1dB_Full);	

	GuiMemFree(pOtdrMeasStrMark2dB_Full);
	pOtdrMeasStrMark2dB_Full = Float2GuiStringUnit(LOSS_PRECISION, 
        (float)mMarkerF.dDot[2].iValue / SCALE_FACTOR, "dB");
	SetLabelText(pOtdrMeasStrMark2dB_Full, pOtdrMeasLblMark2dB_Full);
	DisplayLabel(pOtdrMeasLblMark2dB_Full);	

	GuiMemFree(pOtdrMeasStrMark21dB_Full);
	pOtdrMeasStrMark21dB_Full = Float2GuiStringUnit(LOSS_PRECISION, 
        fdBAB / SCALE_FACTOR, "dB");
	SetLabelText(pOtdrMeasStrMark21dB_Full, pOtdrMeasLblMark21dB_Full);
	DisplayLabel(pOtdrMeasLblMark21dB_Full);	

	GuiMemFree(pOtdrMeasStrLossVal_Full);
    if(EN_METHD_LSA == mMarkerF.enMethd)
    {
        fTmpVal = mMarkerF.fLoss_aABb;
    }
    else
    {
        fTmpVal = mMarkerF.fLoss_AB;
    }
	pOtdrMeasStrLossVal_Full = Float2GuiString(LOSS_PRECISION, fTmpVal);
	SetLabelText(pOtdrMeasStrLossVal_Full, pOtdrMeasLblLossVal_Full);

	GuiMemFree(pOtdrMeasStrAtteVal_Full);
    if(EN_METHD_LSA == mMarkerF.enMethdAtten)
    {
        fTmpVal = mMarkerF.fAtten_aABb;
    }
    else
    {
        fTmpVal = mMarkerF.fAttenAB;
    }
	pOtdrMeasStrAtteVal_Full = Float2GuiString(ATTENUATION_PRECISION, fTmpVal);
	SetLabelText(pOtdrMeasStrAtteVal_Full, pOtdrMeasLblAtteVal_Full);

	GuiMemFree(pOtdrMeasStrRefVal_Full);
	pOtdrMeasStrRefVal_Full = Float2GuiString(REFLECTANCE_PRECISION, 
        mMarkerF.fReflect_AB);
	SetLabelText(pOtdrMeasStrRefVal_Full, pOtdrMeasLblRefVal_Full);

	GuiMemFree(pOtdrMeasStrORLVal1_Full);
	pOtdrMeasStrORLVal1_Full = Float2GuiString(ORL_PRECISION, 
        mMarkerF.fReturnLoss);
	SetLabelText(pOtdrMeasStrORLVal1_Full, pOtdrMeasLblORLVal1_Full);

    GuiMemFree(pOtdrMeasStrORLVal2_Full);
    if(mMarkerF.fTotalReutrnLoss < RETURNLOSS_MAXVALUE)
    {
        pOtdrMeasStrORLVal2_Full= Float2GuiString(ORL_PRECISION, 
            mMarkerF.fTotalReutrnLoss);
    }
    else
    {
        pOtdrMeasStrORLVal2_Full = TransString("------");
    }
    SetLabelText(pOtdrMeasStrORLVal2_Full, pOtdrMeasLblORLVal2_Full);

	return iReturn;
}

static void AddFullMeasToComp(GUIWINDOW *pWndObj)
{
	/**************************注册下部右侧选项卡控件**************************/
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnLoss_Full, 
	              pWndObj);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnAttenuation_Full, 
	              pWndObj);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnREF_Full, 
	              pWndObj);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnORL_Full, 
	              pWndObj);
}

static int DelFullMeasComp(GUIWINDOW *pWndObj)
{
	DelWindowComp(pOtdrMeasBtnLoss_Full, pWndObj);
	DelWindowComp(pOtdrMeasBtnAttenuation_Full, pWndObj);
	DelWindowComp(pOtdrMeasBtnREF_Full, pWndObj);
	DelWindowComp(pOtdrMeasBtnORL_Full, pWndObj);
	return 0;
}

static void LoginFullMeasMsg(WNDOTDRMEAS *pMeasWnd)
{

	GUIMESSAGE * pMsg = NULL;
	pMsg = GetCurrMessage();
	/************************注册下部右侧选项卡处理函数************************/
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnLoss_Full, 
	               FullMeasBtnLoss_Down, (void *)pMeasWnd, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnAttenuation_Full, 
	               FullMeasBtnAttenuation_Down, (void *)pMeasWnd, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnREF_Full, 
	               FullMeasBtnREF_Down, (void *)pMeasWnd, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnORL_Full, 
	               FullMeasBtnORL_Down, (void *)pMeasWnd, 0, pMsg);
}

static void LogoutFullMeasMsg(void)
{

   	GUIMESSAGE *pMsg = NULL;
	pMsg = GetCurrMessage();
	/************************注册下部右侧选项卡处理函数************************/
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnLoss_Full,pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnAttenuation_Full, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnREF_Full,pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnORL_Full, pMsg);
}

int SetFullMeasEnable(WNDOTDRMEAS *pMeasWnd,int iEnable,GUIWINDOW *pWndObj)
{
	if (pMeasWnd->iEnable != iEnable)
	{
		if (iEnable)
		{
			AddFullMeasToComp( pWndObj);
			LoginFullMeasMsg(pMeasWnd);
		}
		else
		{	
			DelFullMeasComp(pWndObj);
			LogoutFullMeasMsg();
		}

		//设置使能标志
		pMeasWnd->iEnable = iEnable;
	}
	return 0;
}

int RefreshFullMeasWnd(WNDOTDRMEAS *pWnd)
{
	int iRet = 0;

	if(pWnd->iEnable == 0)
	{
		return -1;
	}

	LoopFullMeasWnd(pWnd);
	FullMeasWndPaint(pWnd);

	return iRet;
}

/***********************全屏时的模块文本初始化函数定义*************************/

/***
  * 功能：
        		初始化文本资源
  * 参数：
        		无
  * 返回：
       		 成功返回零，失败返回非零值
  * 备注：
***/
static int FullMeasTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	/**************************初始化下部左侧文本信息**************************/
	pOtdrMeasStrMarkNum_Full[0] = TransString("A  :"); 
	pOtdrMeasStrMarkNum_Full[1] = TransString("B  :"); 
	pOtdrMeasStrMarkNum_Full[2] = TransString("B-A:");
	
	pOtdrMeasStrMark1Dist_Full = TransString(""); 
	pOtdrMeasStrMark2Dist_Full = TransString(""); 
	pOtdrMeasStrMark21Dist_Full = TransString(""); 
	pOtdrMeasStrMark1dB_Full = TransString(""); 
	pOtdrMeasStrMark2dB_Full = TransString(""); 
	pOtdrMeasStrMark21dB_Full = TransString(""); 
	
	/**************************初始化下部右侧文本信息**************************/
	pOtdrMeasStrLoss_Full = TransString("MEASURE_LBL_LOSS"); 
	pOtdrMeasStrAttenuation_Full = TransString("MEASURE_LBL_ATTENUATION"); 
	pOtdrMeasStrREF_Full = TransString("MEASURE_LBL_REFLECT");
	pOtdrMeasStrORL_Full = TransString("MAESURE_LBL_ORL"); 
	pOtdrMeasStrLossTitle_Full = TransString("MEASURE_LBL_LOSS_DB");
	pOtdrMeasStrLossVal_Full = TransString(""); 
    pOtdrMeasStrAtteTitle_Full = TransString("MEASURE_LBL_ATTENUATION_DB");

	pOtdrMeasStrAtteVal_Full = TransString("");
	pOtdrMeasStrRefTitle_Full = TransString("MEASURE_LBL_REFLECT_DB");
	pOtdrMeasStrRefVal_Full = TransString(""); 
	pOtdrMeasStrORLVal1_Full = TransString("");
	pOtdrMeasStrORLVal2_Full = TransString("------");
	pOtdrMeasStrORLInfo1_Full = TransString("MEASURE_LBL_A_BLREFLECT"); 
	pOtdrMeasStrORLInfo2_Full = TransString("MAESURE_LBL_SPANORL");
	return iReturn;
}

/***
  * 功能：
        		释放文本资源
  * 参数：
        		无
  * 返回：
       		 成功返回零，失败返回非零值
  * 备注：
***/
static int FullMeasTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int i =0;

	/***************************释放下部左侧文本信息***************************/
	GuiMemFree(pOtdrMeasStrMark1Dist_Full); 
	GuiMemFree(pOtdrMeasStrMark1dB_Full); 
	GuiMemFree(pOtdrMeasStrMark2Dist_Full); 
	GuiMemFree(pOtdrMeasStrMark2dB_Full); 
	GuiMemFree(pOtdrMeasStrMark21Dist_Full); 
	GuiMemFree(pOtdrMeasStrMark21dB_Full); 
	
	for(i=0; i<3; i++)
	{
		GuiMemFree(pOtdrMeasStrMarkNum_Full[i]); 
	}
	
	/***************************释放下部右侧文本信息***************************/
	GuiMemFree(pOtdrMeasStrLoss_Full); 
	GuiMemFree(pOtdrMeasStrAttenuation_Full); 
	GuiMemFree(pOtdrMeasStrREF_Full); 
	GuiMemFree(pOtdrMeasStrORL_Full); 
	
	GuiMemFree(pOtdrMeasStrLossTitle_Full); 
	GuiMemFree(pOtdrMeasStrLossVal_Full);
	
	GuiMemFree(pOtdrMeasStrAtteTitle_Full); 
	GuiMemFree(pOtdrMeasStrAtteVal_Full);
	
	GuiMemFree(pOtdrMeasStrRefTitle_Full); 
	GuiMemFree(pOtdrMeasStrRefVal_Full);
	
	GuiMemFree(pOtdrMeasStrORLVal1_Full);
	GuiMemFree(pOtdrMeasStrORLVal2_Full);
	GuiMemFree(pOtdrMeasStrORLInfo1_Full);
	GuiMemFree(pOtdrMeasStrORLInfo2_Full);
	return iReturn;
}
