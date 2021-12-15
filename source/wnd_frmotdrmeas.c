/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrmeas.c
* 摘    要：  实现主窗体frmotdrmeas的窗体处理线程及相关
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
#include "app_curve.h"
#include "wnd_frmcommonset.h"
#include "wnd_frmabout.h"
#include "wnd_frmotdrsave.h"
#include "wnd_frmmark.h"
#include "app_frmotdrmessage.h"
#include "wnd_frmdialog.h"
#include "guiphoto.h"
#ifdef MINI2
#include "app_algorithm_support.h"
#endif
#include "app_unitconverter.h"

/**************************************************************
* 		窗体frmotdrmeas中引用的变量				*
**************************************************************/
static PDISPLAY_INFO pDisplay  = NULL;

static char *pMarkNum[] = {"a", "A", "B", "b", "ab", "ALL"};

extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//设置顶层结构
/**********************************背景定义************************************/
static GUIPICTURE *pOtdrMeasBg = NULL;


/*****************************下部右侧损耗控件定义*****************************/
static GUIPICTURE *pOtdrMeasBtnInfoField = NULL;	//背景
static GUIPICTURE *pOtdrMeasBtnMarkpointRefresh= NULL;
static GUIPICTURE *pOtdrMeasBtnLoss = NULL; 		//信息选项按钮
static GUIPICTURE *pOtdrMeasBtnAttenuation = NULL; 
static GUIPICTURE *pOtdrMeasBtnREF = NULL; 
static GUIPICTURE *pOtdrMeasBtnORL = NULL; 

static GUILABEL *pOtdrMeasLblResult = NULL;
static GUILABEL *pOtdrMeasLblLossTitle = NULL;		//损耗标签
static GUILABEL *pOtdrMeasLblLossVal = NULL; 
static GUILABEL *pOtdrMeasLblLossInfo = NULL; 
static GUILABEL *pOtdrMeasLblAtteTitle = NULL; 		//增益标签
static GUILABEL *pOtdrMeasLblAtteVal = NULL; 
static GUILABEL *pOtdrMeasLblAtteInfo = NULL; 
static GUILABEL *pOtdrMeasLblRefTitle = NULL; 		//反射率标签
static GUILABEL *pOtdrMeasLblRefVal = NULL; 
static GUILABEL *pOtdrMeasLblRefInfo = NULL; 
static GUILABEL *pOtdrMeasLblORLVal1 = NULL; 		//光回损标签
static GUILABEL *pOtdrMeasLblORLVal2 = NULL; 
static GUILABEL *pOtdrMeasLblORLTitle = NULL; 
static GUILABEL *pOtdrMeasLblORLInfo1 = NULL; 
static GUILABEL *pOtdrMeasLblORLInfo2 = NULL; 
static GUILABEL *pOtdrMeasLblLoss = NULL; 			//信息数据标签
static GUILABEL *pOtdrMeasLblAttenuation = NULL; 
static GUILABEL *pOtdrMeasLblREF = NULL; 
static GUILABEL *pOtdrMeasLblORL = NULL; 

static GUICHAR *pOtdrMeasStrResult = NULL;
static GUICHAR *pOtdrMeasStrLoss = NULL; 			//损耗信息文本
static GUICHAR *pOtdrMeasStrAttenuation = NULL; 	//增益信息文本
static GUICHAR *pOtdrMeasStrREF = NULL; 			//反射率信息文本
static GUICHAR *pOtdrMeasStrORL = NULL; 			//光回损信息文本
static GUICHAR *pOtdrMeasStrLossTitle = NULL; 		//损耗选项卡文本
static GUICHAR *pOtdrMeasStrLossVal = NULL; 
static GUICHAR *pOtdrMeasStrLossInfo = NULL; 
static GUICHAR *pOtdrMeasStrAtteTitle = NULL; 		//增益选项卡文本
static GUICHAR *pOtdrMeasStrAtteVal = NULL; 
static GUICHAR *pOtdrMeasStrAtteInfo = NULL; 
static GUICHAR *pOtdrMeasStrRefTitle = NULL; 		//反射率选项卡文本
static GUICHAR *pOtdrMeasStrRefVal = NULL; 
static GUICHAR *pOtdrMeasStrRefInfo = NULL; 
static GUICHAR *pOtdrMeasStrORLVal1 = NULL; 		//光回损选项卡文本
static GUICHAR *pOtdrMeasStrORLVal2 = NULL; 
static GUICHAR *pOtdrMeasStrORLTitle = NULL; 
static GUICHAR *pOtdrMeasStrORLInfo1 = NULL; 
static GUICHAR *pOtdrMeasStrORLInfo2 = NULL; 

/*******************************下部左侧控件定义*******************************/
static GUIPICTURE *pOtdrMeasBtnSlideLeft = NULL; 
static GUIPICTURE *pOtdrMeasBtnSlideRight = NULL; 
static GUIPICTURE *pOtdrMeas1Dist = NULL;
static GUIPICTURE *pOtdrMeas2Dist = NULL;
static GUIPICTURE *pOtdrMeas21Dist = NULL;
static GUIPICTURE *pOtdrMeas1dB = NULL;
static GUIPICTURE *pOtdrMeas2dB = NULL;
static GUIPICTURE *pOtdrMeas21dB = NULL;

static GUILABEL *pOtdrMeasLblMarkNum[3] = {};
static GUILABEL *pOtdrMeasLblMark1Dist = NULL; 
static GUILABEL *pOtdrMeasLblMark1dB = NULL; 
static GUILABEL *pOtdrMeasLblMark2Dist = NULL; 
static GUILABEL *pOtdrMeasLblMark2dB = NULL; 
static GUILABEL *pOtdrMeasLblMark21Dist = NULL; 
static GUILABEL *pOtdrMeasLblMark21dB = NULL; 
static GUILABEL *pOtdrMeasLblCurMarkNum = NULL; 
static GUILABEL *pOtdrMeasLblMarkLineInfo = NULL;
static GUILABEL *pOtdrMeasLblMarkLine = NULL;

static GUICHAR *pOtdrMeasStrMarkNum[3] = {};
static GUICHAR *pOtdrMeasStrMark1Dist = NULL; 
static GUICHAR *pOtdrMeasStrMark1dB = NULL; 
static GUICHAR *pOtdrMeasStrMark2Dist = NULL; 
static GUICHAR *pOtdrMeasStrMark2dB = NULL; 
static GUICHAR *pOtdrMeasStrMark21Dist = NULL; 
static GUICHAR *pOtdrMeasStrMark21dB = NULL; 
static GUICHAR *pOtdrMeasStrCurMarkNum = NULL; 
static GUICHAR *pOtdrMeasStrMarkLineInfo = NULL;	
static GUICHAR *pOtdrMeasStrMarkLine = NULL;

static MARKER mMarker;

/**************************************************************
* 				声明frmotdrmeas.c中内部使用的函数
**************************************************************/
/*****************控制窗口按钮点击事件声明***********/
static int OtdrMeasBtnSlideLeft_Down(void *pInArg, int iInLen, 
                                     void *pOutArg, int iOutLen);
static int OtdrMeasBtnSlideLeft_Move(void *pInArg, int iInLen, 
                                     void *pOutArg, int iOutLen);
static int OtdrMeasBtnSlideLeft_Up(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen);
static int OtdrMeasBtnSlideRight_Down(void *pInArg, int iInLen, 
                                      void *pOutArg, int iOutLen);
static int OtdrMeasBtnSlideRight_Move(void *pInArg, int iInLen, 
                                      void *pOutArg, int iOutLen);
static int OtdrMeasBtnSlideRight_Up(void *pInArg, int iInLen, 
                                    void *pOutArg, int iOutLen);

static int MarkpointSwitch_Down(void *pInArg, int iInLen,
                                void *pOutArg, int iOutLen);
static int OtdrMeasBtnLoss_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int OtdrMeasBtnAttenuation_Down(void *pInArg, int iInLen, 
                                       void *pOutArg, int iOutLen);
static int OtdrMeasBtnREF_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
static int OtdrMeasBtnORL_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);

/******************初始化文本资源函数声明*************/
static int OtdrMeasTextRes_Init(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
/******************释放文本资源函数声明****************/
static int OtdrMeasTextRes_Exit(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);

static void AddOtdrMeasToComp(GUIWINDOW *pWndObj);
static int DelOtdrMeasComp(GUIWINDOW *pWndObj);
static void LoginOtdrMeasMsg(WNDOTDRMEAS *pMeasWnd);
static void LogoutOtdrMeasMsg(void);
static void OtdrMeasWndPaint(WNDOTDRMEAS *pWnd);
//刷新当前标记线的数值
static void RefreshCurrentMarkerNum(int iCurr);

/***
  * 功能：
            SlideLeft 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnSlideLeft_Down(void *pInArg, int iInLen, 
                                     void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    TouchChange("btn_otdr_meas_left_press.bmp", pOtdrMeasBtnSlideLeft,
                NULL, NULL, 0);
                
	if (KnobFrequency(200))
        return 0;
        
    GetCurveMarker(CURR_CURVE, pDisplay, &mMarker);

    if(mMarker.enPitchOn == EN_PATCH_ab)
    {
        SendMakerCtlMsg(EN_MAKER_AUTO, 1);
    }
    else
    {
        SendMakerCtlMsg(EN_MAKER_ANTI, 3);
    }

    return iReturn;
}

/***
  * 功能：
            SlideLeft 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnSlideLeft_Move(void *pInArg, int iInLen, 
                                     void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    
	if (KnobFrequency(200))
    		return 0;
    		
    GetCurveMarker(CURR_CURVE, pDisplay, &mMarker);
    if(mMarker.enPitchOn != EN_PATCH_ab)
    {
        SendMakerCtlMsg(EN_MAKER_ANTI, 3);
    }

    return iReturn;
}

/***
  * 功能：
            pOtdrMeasBtnZoomOut 按钮抬起处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnSlideLeft_Up(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    TouchChange("btn_otdr_meas_left_unpress1.bmp", pOtdrMeasBtnSlideLeft,
                NULL, NULL, 0);

    //为了消除闪屏
    if (CurrDisplayStatus() == EN_STATUS_FREE)
    {
        RefreshScreen(__FILE__, __func__, __LINE__);
    }

    return iReturn;
}

/***
  * 功能：
            SlideRight 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnSlideRight_Down(void *pInArg, int iInLen, 
                                      void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    TouchChange("btn_otdr_meas_right_press.bmp", pOtdrMeasBtnSlideRight,
                NULL, NULL, 0);
                
	if (KnobFrequency(200))
        return 0;
        
    GetCurveMarker(CURR_CURVE, pDisplay, &mMarker);
    if(mMarker.enPitchOn == EN_PATCH_ab)
    {
        SendMakerCtlMsg(EN_MAKER_AUTO, 0);
    }
    else
    {
        SendMakerCtlMsg(EN_MAKER_CLOCK, 3);
    }

    return iReturn;
}
/***
  * 功能：
            SlideRight 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnSlideRight_Move(void *pInArg, int iInLen, 
                                      void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    
	if (KnobFrequency(200))
        return 0;
        
    GetCurveMarker(CURR_CURVE, pDisplay, &mMarker);
    if(mMarker.enPitchOn != EN_PATCH_ab)
    {
        SendMakerCtlMsg(EN_MAKER_CLOCK, 3);
    }
    
    return iReturn;
}


/***
  * 功能：
            SlideRight 按钮抬起处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnSlideRight_Up(void *pInArg, int iInLen, 
                                    void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    TouchChange("btn_otdr_meas_right_unpress1.bmp", pOtdrMeasBtnSlideRight,
                NULL, NULL, 0);

    //为了消除闪屏
    if (CurrDisplayStatus() == EN_STATUS_FREE)
    {
        RefreshScreen(__FILE__, __func__, __LINE__);
    }

    return iReturn;
}

//改变损耗等的显示
static void ChangeMeasInfo(WNDOTDRMEAS *pMeasObj, int iCurr)
{
    GUIPICTURE *pMeas[] = {
        pOtdrMeasBtnLoss,
        pOtdrMeasBtnAttenuation,
        pOtdrMeasBtnREF,
        pOtdrMeasBtnORL
    };
    GUILABEL *pMeasLbl[] = {
        pOtdrMeasLblLoss,
        pOtdrMeasLblAttenuation,
        pOtdrMeasLblREF,
        pOtdrMeasLblORL
    };
    
    if(pMeasObj->eMeasCard == 2){
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_unpress2.bmp",
                    pMeas[pMeasObj->eMeasCard]);
    }
    else{
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_unpress.bmp",
                    pMeas[pMeasObj->eMeasCard]);
    }   
    
    if(iCurr == 2){
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_press1.bmp",
                        pMeas[iCurr]);
    }
    else{
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_press.bmp",
                        pMeas[iCurr]);
    }
    
    SetPictureEnable(1, pMeas[pMeasObj->eMeasCard]);
    DisplayPicture(pMeas[pMeasObj->eMeasCard]);
    DisplayPicture(pMeas[iCurr]);
    if(iCurr != EN_MEAS_LOSS)
    {
        SetPictureEnable(0, pMeas[iCurr]);
    }
    
    DisplayLabel(pMeasLbl[pMeasObj->eMeasCard]);
    DisplayLabel(pMeasLbl[iCurr]);

    pMeasObj->eMeasCard = iCurr;

    SendMakerCtlMsg(EN_MAKER_MEAS, iCurr);
}

//设置当前标记线数字
int SetMarerCurrentNum(int iCurr)
{
    MARKER mMarker;
    //获取当前marker
	if (pDisplay == NULL)//直接进入全屏模式就会出现死机，重启。
	{
		pDisplay = pOtdrTopSettings->pDisplayInfo;
	}
    GetCurveMarker(CURR_CURVE, pDisplay, &mMarker);
    int iTmp = mMarker.enPitchOn; 

    switch (iCurr)
    {
        case EN_MEAS_LOSS:
            if(pUser_Settings->sCommonSetting.iLossMethodMark)
            {
                if (iTmp == 0)
                {
                    iTmp = 1;
                }
                if (iTmp == 3)
                {
                    iTmp = 5;
                }
                if (iTmp == 4)
                {
                    iTmp = 5;
                }
            }
            break;
        case EN_MEAS_REFC:
            if (iTmp == 3)
            {
                iTmp = 5;
            }
            if (iTmp == 4)
            {
                iTmp = 5;
            }
            break;
        case EN_MEAS_ATTE:
        case EN_MEAS_ORL:
            if (iTmp == 0)
            {
                iTmp = 1;
            }
            if (iTmp == 3)
            {
                iTmp = 5;
            }
            if (iTmp == 4)
            {
                iTmp = 5;
            }
            break;
        default: 
            break;
    }

    SendMakerCtlMsg(EN_MAKER_PATCH, iTmp);

    return iTmp;
}

//刷新当前标记线的数值
static void RefreshCurrentMarkerNum(int iCurr)
{
    int iTmp = SetMarerCurrentNum(iCurr);
    GuiMemFree(pOtdrMeasStrCurMarkNum);
    pOtdrMeasStrCurMarkNum = TransString(pMarkNum[iTmp]);
    SetLabelText(pOtdrMeasStrCurMarkNum, pOtdrMeasLblCurMarkNum);
    DisplayPicture(pOtdrMeasBtnMarkpointRefresh);
    DisplayLabel(pOtdrMeasLblCurMarkNum);
}

/***
  * 功能：
            pOtdrMeasBtnLoss 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnLoss_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iErr = 0;
    
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
    
    GuiMemFree(pOtdrMeasStrLossInfo);
    pOtdrMeasStrLossInfo = pUser_Settings->sCommonSetting.iLossMethodMark ? 
                        TransString("MEASURE_LBL_TWOPOINTSLOSS") : 
                        TransString("MEASURE_LBL_FOREPOINTSLOSS");
    SetLabelText(pOtdrMeasStrLossInfo, pOtdrMeasLblLossInfo);
    DisplayPicture(pOtdrMeasBtnInfoField);
    DisplayLabel(pOtdrMeasLblLossTitle);
    DisplayLabel(pOtdrMeasLblLossVal);
    DisplayLabel(pOtdrMeasLblLossInfo);
    RefreshCurrentMarkerNum(EN_MEAS_LOSS);
    ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_LOSS);

    return iErr;
}


/***
  * 功能：
            pOtdrMeasBtnAttenuation按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnAttenuation_Down(void *pInArg, int iInLen, 
                                       void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    DisplayPicture(pOtdrMeasBtnInfoField);
    DisplayLabel(pOtdrMeasLblAtteTitle);
    DisplayLabel(pOtdrMeasLblAtteVal);
    DisplayLabel(pOtdrMeasLblAtteInfo);
    RefreshCurrentMarkerNum(EN_MEAS_ATTE);
    ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_ATTE);

    return iReturn;
}


/***
  * 功能：
            pOtdrMeasBtnREF 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnREF_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    DisplayPicture(pOtdrMeasBtnInfoField);
    DisplayLabel(pOtdrMeasLblRefTitle);
    DisplayLabel(pOtdrMeasLblRefVal);
    DisplayLabel(pOtdrMeasLblRefInfo);
    RefreshCurrentMarkerNum(EN_MEAS_REFC);
    ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_REFC);

    return iReturn;
}

/***
  * 功能：
            pOtdrMeasBtnORL 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasBtnORL_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    DisplayPicture(pOtdrMeasBtnInfoField);
    DisplayLabel(pOtdrMeasLblORLVal1);
    DisplayLabel(pOtdrMeasLblORLVal2);
    DisplayLabel(pOtdrMeasLblORLTitle);
    DisplayLabel(pOtdrMeasLblORLInfo1);
    DisplayLabel(pOtdrMeasLblORLInfo2);
    RefreshCurrentMarkerNum(EN_MEAS_ORL);
    ChangeMeasInfo((WNDOTDRMEAS *)pOutArg, EN_MEAS_ORL);

    return iReturn;
}



/***
  * 功能：
             MarkpointSwitch 按钮按下处理函数
  * 参数：
             无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int MarkpointSwitch_Down(void *pInArg, int iInLen,
                                void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iTmp = EN_PATCH_ALL;

    //获取当前marker
    if(0 == GetCurveMarker(CURR_CURVE, pDisplay, &mMarker))
    {
        iTmp = mMarker.enPitchOn;
        iTmp = (iTmp + 1) % 6;
    }

    // 根据当前选项卡，只切换可见的标记线
    switch (mMarker.enMeas)
    {
    // aABb 均可见
    case EN_MEAS_LOSS:            
        if(pUser_Settings->sCommonSetting.iLossMethodMark)
        {
            // 跳过a
            if (iTmp == 0)
                iTmp = 1;
            // 跳过b
            if (iTmp == 3)
                iTmp = 5;
            //跳过ab
            if (iTmp == 4)
                iTmp = 5;
        }
        break;
    // 仅aAB 可见
    case EN_MEAS_REFC:
        // 跳过b
        if (iTmp == 3)
            iTmp = 5;
        //跳过ab
        if (iTmp == 4)
            iTmp = 5;
        break;
    // 仅AB 可见
    case EN_MEAS_ATTE:
    case EN_MEAS_ORL:
        // 跳过a
        if (iTmp == 0)
            iTmp = 1;
        // 跳过b
        if (iTmp == 3)
            iTmp = 5;
        //跳过ab
        if (iTmp == 4)
            iTmp = 5;
        break;
    default: 
        break;
    }

    GuiMemFree(pOtdrMeasStrCurMarkNum);
    pOtdrMeasStrCurMarkNum = TransString(pMarkNum[iTmp]);
    SetLabelText(pOtdrMeasStrCurMarkNum, pOtdrMeasLblCurMarkNum);
    DisplayPicture(pOtdrMeasBtnMarkpointRefresh);
    DisplayLabel(pOtdrMeasLblCurMarkNum);

    SendMakerCtlMsg(EN_MAKER_PATCH, iTmp);

    return iErr;
}

//点击屏幕标记线时更改 左下当前选中标记线标签
int TouchMarkLineLabelChange(int iIndex)
{	
    GuiMemFree(pOtdrMeasStrCurMarkNum);
    pOtdrMeasStrCurMarkNum = TransString(pMarkNum[iIndex]);
    SetLabelText(pOtdrMeasStrCurMarkNum, pOtdrMeasLblCurMarkNum);
    DisplayPicture(pOtdrMeasBtnMarkpointRefresh);
    DisplayLabel(pOtdrMeasLblCurMarkNum);

    return 0;
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
WNDOTDRMEAS * CreateOtdrMeasWnd(void)
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

    //初始化资源
    pDisplay = pOtdrTopSettings->pDisplayInfo;
    GetCurveMarker(CURR_CURVE, pDisplay, &mMarker);

    //初始化结构体
    pMeas->eMeasCard = mMarker.enMeas;
    pMeas->enPitchOn = mMarker.enPitchOn;
    pMeas->iEnable = 0;

    //初始化文本资源
    OtdrMeasTextRes_Init(pMeas, 0, NULL, 0);

    /********************************创建背景控件******************************/
    pOtdrMeasBg= CreatePhoto("bg_otdr_meas");
    /***************************************************************************
    **				       窗体frmotdrmeas中下部左侧控件                      **
    ***************************************************************************/
    pOtdrMeasLblMarkLineInfo = CreateLabel(20,333,300,24,
                                           pOtdrMeasStrMarkLineInfo);
    pOtdrMeasLblMarkLine = CreateLabel(20,434,95,24,
                                       pOtdrMeasStrMarkLine);

    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pOtdrMeasLblMarkLineInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pOtdrMeasLblMarkLine);

    pOtdrMeasBtnSlideLeft= CreatePhoto("btn_otdr_meas_left_unpress1");
    pOtdrMeasBtnSlideLeft->Visible.iCursor = 1;
    pOtdrMeasBtnSlideRight= CreatePhoto("btn_otdr_meas_right_unpress1");
    pOtdrMeasBtnSlideRight->Visible.iCursor = 1;

    pOtdrMeasLblMarkNum[0] = CreateLabel(40,358,50,24,
                                         pOtdrMeasStrMarkNum[0]);
    pOtdrMeasLblMarkNum[1] = CreateLabel(40,378,50,24,
                                         pOtdrMeasStrMarkNum[1]);
    pOtdrMeasLblMarkNum[2] = CreateLabel(40,398,50,24,
                                         pOtdrMeasStrMarkNum[2]);

    pOtdrMeasLblMark1Dist = CreateLabel(120,358,95,24,
                                        pOtdrMeasStrMark1Dist);
    pOtdrMeasLblMark1dB = CreateLabel(220,358,95,24,
                                      pOtdrMeasStrMark1dB);
    pOtdrMeasLblMark2Dist = CreateLabel(120,378,95,24,
                                        pOtdrMeasStrMark2Dist);
    pOtdrMeasLblMark2dB = CreateLabel(220,378,95,24,
                                      pOtdrMeasStrMark2dB);
    pOtdrMeasLblMark21Dist = CreateLabel(120,398,95,24,
                                         pOtdrMeasStrMark21Dist);
    pOtdrMeasLblMark21dB = CreateLabel(220,398,95,24,
                                       pOtdrMeasStrMark21dB);

    pOtdrMeasLblCurMarkNum = CreateLabel(185,434,80,24,
                                         pOtdrMeasStrCurMarkNum);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblCurMarkNum);
    /***************************************************************************
    **				       窗体frmotdrmeas中下部右侧控件                      **
    ***************************************************************************/
    pOtdrMeasLblResult = CreateLabel(362,334,95,24, pOtdrMeasStrResult);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrMeasLblResult);

    pOtdrMeasBtnInfoField = CreatePhoto("bg_otdr_meas_info_field");
    pOtdrMeasBtnMarkpointRefresh = CreatePicture(185, 422, 80, 45,
                                                 BmpFileDirectory"btn_markpoint_refresh.bmp");

    pOtdrMeasLblLoss = CreateLabel(373, 437, 65, 24, pOtdrMeasStrLoss);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblLoss);
    pOtdrMeasLblAttenuation = CreateLabel(438, 437, 65, 24, pOtdrMeasStrAttenuation);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblAttenuation);
    pOtdrMeasLblREF = CreateLabel(503, 437, 97, 24, pOtdrMeasStrREF);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblREF);
    pOtdrMeasLblORL = CreateLabel(600, 437, 65, 24, pOtdrMeasStrORL);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORL);

    pOtdrMeasLblLossTitle = CreateLabel(375,358,282,24, pOtdrMeasStrLossTitle);
    pOtdrMeasLblLossVal = CreateLabel(375,378,282,24, pOtdrMeasStrLossVal);
    pOtdrMeasLblLossInfo = CreateLabel(375,398,282,24, pOtdrMeasStrLossInfo);

    pOtdrMeasLblAtteTitle = CreateLabel(375,358,282,24, pOtdrMeasStrAtteTitle);
    pOtdrMeasLblAtteVal = CreateLabel(375,378,282,24, pOtdrMeasStrAtteVal);
    pOtdrMeasLblAtteInfo = CreateLabel(375,398,282,24, pOtdrMeasStrAtteInfo);

    pOtdrMeasLblRefTitle = CreateLabel(375,358,282,24, pOtdrMeasStrRefTitle);
    pOtdrMeasLblRefVal = CreateLabel(375,378,282,24, pOtdrMeasStrRefVal);
    pOtdrMeasLblRefInfo = CreateLabel(375,398,282,24, pOtdrMeasStrRefInfo);

    pOtdrMeasLblORLTitle = CreateLabel(375,358,282,24, pOtdrMeasStrORLTitle);
    pOtdrMeasLblORLVal1 = CreateLabel(378,378,130,24, pOtdrMeasStrORLVal1);
    pOtdrMeasLblORLVal2 = CreateLabel(520,378,130,24, pOtdrMeasStrORLVal2);
    pOtdrMeasLblORLInfo1 = CreateLabel(378,398,130,24, pOtdrMeasStrORLInfo1);
    pOtdrMeasLblORLInfo2 = CreateLabel(520,398,130,24, pOtdrMeasStrORLInfo2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblLossTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblLossVal);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblLossInfo);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblAtteTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblAtteVal);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblAtteInfo);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblRefTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblRefVal);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblRefInfo);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORLVal1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORLVal2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORLTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORLInfo1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrMeasLblORLInfo2);

    pOtdrMeasBtnLoss= CreatePhoto("btn_otdr_meas_exl_unpress");
    pOtdrMeasBtnAttenuation= CreatePhoto("btn_otdr_meas_exl_unpress1");
    pOtdrMeasBtnREF= CreatePhoto("btn_otdr_meas_exl_unpress2");
    pOtdrMeasBtnORL= CreatePhoto("btn_otdr_meas_exl_unpress3");

    if(pMeas->eMeasCard == EN_MEAS_LOSS)
    {
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_press.bmp", pOtdrMeasBtnLoss);
    }
    else if(pMeas->eMeasCard == EN_MEAS_ATTE)
    {
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_press.bmp",
                         pOtdrMeasBtnAttenuation);
    }
    else if(pMeas->eMeasCard == EN_MEAS_REFC)
    {
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_press1.bmp", pOtdrMeasBtnREF);
    }
    else if(pMeas->eMeasCard == EN_MEAS_ORL)
    {
        SetPictureBitmap(BmpFileDirectory"btn_otdr_meas_exl_press.bmp", pOtdrMeasBtnORL);
    }
    
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrMeasLblMarkNum[0]);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrMeasLblMarkNum[1]);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrMeasLblMarkNum[2]);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblLoss);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblAttenuation);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblREF);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrMeasLblORL);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark1Dist);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark1dB);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark2Dist);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark2dB);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark21Dist);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblMark21dB);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblCurMarkNum);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblLossTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblLossVal);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblLossInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblAtteTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblAtteVal);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblAtteInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblRefTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblRefVal);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblRefInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblORLVal1);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblORLVal2);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblORLTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblORLInfo1);
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrMeasLblORLInfo2);

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
int DestroyOtdrMeasWnd(WNDOTDRMEAS *pWnd)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    int i = 0;

    /***********************销毁背景窗体控件***********************************/
    DestroyPicture(&pOtdrMeasBg);
    /************************销毁下部右侧选项卡控件****************************/
    DestroyPicture(&pOtdrMeasBtnLoss);
    DestroyPicture(&pOtdrMeasBtnAttenuation);
    DestroyPicture(&pOtdrMeasBtnREF);
    DestroyPicture(&pOtdrMeasBtnORL);
    DestroyLabel(&pOtdrMeasLblLoss);
    DestroyLabel(&pOtdrMeasLblAttenuation);
    DestroyLabel(&pOtdrMeasLblREF);
    DestroyLabel(&pOtdrMeasLblORL);

    /**************************销毁下部右侧控件********************************/
    DestroyPicture(&pOtdrMeasBtnInfoField);
    DestroyPicture(&pOtdrMeasBtnMarkpointRefresh);

    DestroyLabel(&pOtdrMeasLblLossTitle);
    DestroyLabel(&pOtdrMeasLblLossVal);
    DestroyLabel(&pOtdrMeasLblLossInfo);
    DestroyLabel(&pOtdrMeasLblAtteTitle);
    DestroyLabel(&pOtdrMeasLblAtteVal);
    DestroyLabel(&pOtdrMeasLblAtteInfo);
    DestroyLabel(&pOtdrMeasLblRefTitle);
    DestroyLabel(&pOtdrMeasLblRefVal);
    DestroyLabel(&pOtdrMeasLblRefInfo);
    DestroyLabel(&pOtdrMeasLblORLVal1);
    DestroyLabel(&pOtdrMeasLblORLVal2);
    DestroyLabel(&pOtdrMeasLblORLTitle);
    DestroyLabel(&pOtdrMeasLblORLInfo1);
    DestroyLabel(&pOtdrMeasLblORLInfo2);
    /**************************销毁下部左侧控件********************************/
    DestroyPicture(&pOtdrMeasBtnSlideLeft);
    DestroyPicture(&pOtdrMeasBtnSlideRight);
    DestroyPicture(&pOtdrMeas1Dist);
    DestroyPicture(&pOtdrMeas2Dist);
    DestroyPicture(&pOtdrMeas21Dist);
    DestroyPicture(&pOtdrMeas1dB);
    DestroyPicture(&pOtdrMeas2dB);
    DestroyPicture(&pOtdrMeas21dB);

    DestroyLabel(&pOtdrMeasLblResult);
    DestroyLabel(&pOtdrMeasLblMarkLineInfo);
    DestroyLabel(&pOtdrMeasLblMarkLine);
    DestroyLabel(&pOtdrMeasLblCurMarkNum);
    DestroyLabel(&pOtdrMeasLblMark1Dist);
    DestroyLabel(&pOtdrMeasLblMark1dB);
    DestroyLabel(&pOtdrMeasLblMark2Dist);
    DestroyLabel(&pOtdrMeasLblMark2dB);
    DestroyLabel(&pOtdrMeasLblMark21Dist);
    DestroyLabel(&pOtdrMeasLblMark21dB);

    for(i =0; i<3; i++)
    {
        DestroyLabel(&pOtdrMeasLblMarkNum[i]);
    }

    //释放结构体
    GuiMemFree(pWnd);
    OtdrMeasTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}


static void OtdrMeasWndPaint(WNDOTDRMEAS *pMeas)
{
    /***********************显示背景窗体控件***********************************/
    DisplayPicture(pOtdrMeasBg);
    SetPictureEnable(1, pOtdrMeasBtnLoss);
    SetPictureEnable(1, pOtdrMeasBtnAttenuation);
    SetPictureEnable(1, pOtdrMeasBtnREF);
    SetPictureEnable(1, pOtdrMeasBtnORL);

    /**************************显示下部左侧控件********************************/
    DisplayPicture(pOtdrMeasBtnSlideLeft);
    DisplayPicture(pOtdrMeasBtnSlideRight);
    DisplayPicture(pOtdrMeasBtnLoss);
    DisplayPicture(pOtdrMeasBtnAttenuation);
    DisplayPicture(pOtdrMeasBtnREF);
    DisplayPicture(pOtdrMeasBtnORL);

    //标记线信息 标题
    DisplayLabel(pOtdrMeasLblMarkLineInfo);
    DisplayLabel(pOtdrMeasLblMarkLine);

    DisplayLabel(pOtdrMeasLblMarkNum[0]);
    DisplayLabel(pOtdrMeasLblMarkNum[1]);
    DisplayLabel(pOtdrMeasLblMarkNum[2]);

    /**************************显示下部右侧控件********************************/
    DisplayLabel(pOtdrMeasLblResult);
    DisplayLabel(pOtdrMeasLblLoss);
    DisplayLabel(pOtdrMeasLblAttenuation);
    DisplayLabel(pOtdrMeasLblREF);
    DisplayLabel(pOtdrMeasLblORL);
    DisplayLabel(pOtdrMeasLblMark1Dist);
    DisplayLabel(pOtdrMeasLblMark1dB);
    DisplayLabel(pOtdrMeasLblMark2Dist);
    DisplayLabel(pOtdrMeasLblMark2dB);
    DisplayLabel(pOtdrMeasLblMark21Dist);
    DisplayLabel(pOtdrMeasLblMark21dB);
    DisplayPicture(pOtdrMeasBtnMarkpointRefresh);
    DisplayLabel(pOtdrMeasLblCurMarkNum);
    DisplayPicture(pOtdrMeasBtnInfoField);

    if(pMeas->eMeasCard == EN_MEAS_LOSS)
    {
        DisplayLabel(pOtdrMeasLblLossTitle);
        DisplayLabel(pOtdrMeasLblLossVal);
        DisplayLabel(pOtdrMeasLblLossInfo);
    }
    else if(pMeas->eMeasCard == EN_MEAS_ATTE)
    {
        DisplayLabel(pOtdrMeasLblAtteTitle);
        DisplayLabel(pOtdrMeasLblAtteVal);
        DisplayLabel(pOtdrMeasLblAtteInfo);
        SetPictureEnable(0, pOtdrMeasBtnAttenuation);
    }
    else if(pMeas->eMeasCard == EN_MEAS_REFC)
    {
        DisplayLabel(pOtdrMeasLblRefTitle);
        DisplayLabel(pOtdrMeasLblRefVal);
        DisplayLabel(pOtdrMeasLblRefInfo);
        SetPictureEnable(0, pOtdrMeasBtnREF);
    }
    else if(pMeas->eMeasCard == EN_MEAS_ORL)
    {
        DisplayLabel(pOtdrMeasLblORLVal1);
        DisplayLabel(pOtdrMeasLblORLVal2);
        DisplayLabel(pOtdrMeasLblORLTitle);
        DisplayLabel(pOtdrMeasLblORLInfo1);
        DisplayLabel(pOtdrMeasLblORLInfo2);
        SetPictureEnable(0, pOtdrMeasBtnORL);
    }

    SendMakerCtlMsg(EN_MAKER_MEAS, pMeas->eMeasCard);
    RefreshCurrentMarkerNum(pMeas->eMeasCard);
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
int LoopOtdrMeasWnd(WNDOTDRMEAS *pWnd)
{
    //错误标志、返回值定义
    int iErr = 0;

    //临时变量定义
    float fDistAB, fdBAB;
    float fTmpVal = 0.0f;
    
    
    //获得标记信息
    //若没有曲线，则返回，让界面上的内容保持默认的空值
    if (GetCurveMarker(CURR_CURVE, pDisplay, &mMarker) == 1)
        return -1;
    fDistAB = mMarker.dDot[2].fDist - mMarker.dDot[1].fDist;
    fdBAB = mMarker.dDot[2].iValue - mMarker.dDot[1].iValue;
    
    GuiMemFree(pOtdrMeasStrMark1Dist);
    pOtdrMeasStrMark1Dist = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
        mMarker.dDot[1].fDist, 1);
    SetLabelText(pOtdrMeasStrMark1Dist, pOtdrMeasLblMark1Dist);

    GuiMemFree(pOtdrMeasStrMark2Dist);
    pOtdrMeasStrMark2Dist = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
        mMarker.dDot[2].fDist, 1);
    SetLabelText(pOtdrMeasStrMark2Dist, pOtdrMeasLblMark2Dist);

    GuiMemFree(pOtdrMeasStrMark21Dist);
    pOtdrMeasStrMark21Dist = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
        fDistAB, 1);
    SetLabelText(pOtdrMeasStrMark21Dist, pOtdrMeasLblMark21Dist);

    GuiMemFree(pOtdrMeasStrMark1dB);
    pOtdrMeasStrMark1dB = Float2GuiStringUnit(LOSS_PRECISION, 
        (float)mMarker.dDot[1].iValue / SCALE_FACTOR, "dB");
    SetLabelText(pOtdrMeasStrMark1dB, pOtdrMeasLblMark1dB);

    GuiMemFree(pOtdrMeasStrMark2dB);
    pOtdrMeasStrMark2dB = Float2GuiStringUnit(LOSS_PRECISION, 
        (float)mMarker.dDot[2].iValue / SCALE_FACTOR, "dB");
    SetLabelText(pOtdrMeasStrMark2dB, pOtdrMeasLblMark2dB);

    GuiMemFree(pOtdrMeasStrMark21dB);
    pOtdrMeasStrMark21dB = Float2GuiStringUnit(LOSS_PRECISION, 
        fdBAB / SCALE_FACTOR, "dB");
    SetLabelText(pOtdrMeasStrMark21dB, pOtdrMeasLblMark21dB);

    GuiMemFree(pOtdrMeasStrLossVal);
    if(EN_METHD_LSA == mMarker.enMethd)
    {
        fTmpVal = mMarker.fLoss_aABb;
    }
    else
    {
        fTmpVal = mMarker.fLoss_AB;
    }
    pOtdrMeasStrLossVal = Float2GuiString(LOSS_PRECISION, fTmpVal);
    SetLabelText(pOtdrMeasStrLossVal, pOtdrMeasLblLossVal);
    
    GuiMemFree(pOtdrMeasStrAtteVal);
    if(EN_METHD_LSA == mMarker.enMethdAtten)
    {
        fTmpVal = mMarker.fAtten_aABb;
    }
    else
    {
        fTmpVal = mMarker.fAttenAB;
    }
    pOtdrMeasStrAtteVal = Float2GuiString(ATTENUATION_PRECISION, fTmpVal);
    SetLabelText(pOtdrMeasStrAtteVal, pOtdrMeasLblAtteVal);

    GuiMemFree(pOtdrMeasStrRefVal);
    pOtdrMeasStrRefVal= Float2GuiString(REFLECTANCE_PRECISION, 
        mMarker.fReflect_AB);
    SetLabelText(pOtdrMeasStrRefVal, pOtdrMeasLblRefVal);

    GuiMemFree(pOtdrMeasStrORLVal1);
    pOtdrMeasStrORLVal1= Float2GuiString(ORL_PRECISION, 
        mMarker.fReturnLoss);
    SetLabelText(pOtdrMeasStrORLVal1, pOtdrMeasLblORLVal1);

    GuiMemFree(pOtdrMeasStrORLVal2);
    if(mMarker.fTotalReutrnLoss < RETURNLOSS_MAXVALUE)
    {
        pOtdrMeasStrORLVal2= Float2GuiString(ORL_PRECISION, 
            mMarker.fTotalReutrnLoss);
    }
    else
    {
        pOtdrMeasStrORLVal2 = TransString("------");
    }
    SetLabelText(pOtdrMeasStrORLVal2, pOtdrMeasLblORLVal2);

    return iErr;
}


static void AddOtdrMeasToComp(GUIWINDOW *pWndObj)
{
    /**************************注册下部右侧选项卡控件**************************/
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnLoss,
                  pWndObj);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnAttenuation,
                  pWndObj);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnREF,
                  pWndObj);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnORL,
                  pWndObj);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnMarkpointRefresh,pWndObj);				//刷新背景

    /**************************注册下部左侧选项卡控件**************************/
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnSlideLeft,
                  pWndObj);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrMeasBtnSlideRight,
                  pWndObj);

}

static int DelOtdrMeasComp(GUIWINDOW *pWndObj)
{
    DelWindowComp(pOtdrMeasBtnLoss, pWndObj);
    DelWindowComp(pOtdrMeasBtnAttenuation, pWndObj);
    DelWindowComp(pOtdrMeasBtnREF, pWndObj);
    DelWindowComp(pOtdrMeasBtnORL, pWndObj);

    DelWindowComp(pOtdrMeasBtnMarkpointRefresh, pWndObj);
    DelWindowComp(pOtdrMeasBtnSlideLeft, pWndObj);
    DelWindowComp(pOtdrMeasBtnSlideRight, pWndObj);

    return 0;
}

static void LoginOtdrMeasMsg(WNDOTDRMEAS *pMeasWnd)
{

    GUIMESSAGE * pMsg = NULL;
    pMsg = GetCurrMessage();

    /************************注册下部右侧选项卡处理函数************************/
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnLoss,
                    OtdrMeasBtnLoss_Down, (void *)pMeasWnd, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnAttenuation,
                    OtdrMeasBtnAttenuation_Down, (void *)pMeasWnd, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnREF,
                    OtdrMeasBtnREF_Down, (void *)pMeasWnd, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnORL,
                    OtdrMeasBtnORL_Down, (void *)pMeasWnd, 0, pMsg);

    /************************注册下部左侧选项卡处理函数************************/
    LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrMeasBtnSlideLeft,
                    OtdrMeasBtnSlideLeft_Up, (void *)pMeasWnd, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnSlideLeft,
                    OtdrMeasBtnSlideLeft_Down, (void *)pMeasWnd, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_MOV_CURSOR, pOtdrMeasBtnSlideLeft,
                    OtdrMeasBtnSlideLeft_Move, (void *)pMeasWnd, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrMeasBtnSlideRight,
                    OtdrMeasBtnSlideRight_Up, (void *)pMeasWnd, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnSlideRight,
                    OtdrMeasBtnSlideRight_Down, (void *)pMeasWnd, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_MOV_CURSOR, pOtdrMeasBtnSlideRight,
                    OtdrMeasBtnSlideRight_Move, (void *)pMeasWnd, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnMarkpointRefresh, MarkpointSwitch_Down, (void *)pMeasWnd, 0, pMsg);
}

static void LogoutOtdrMeasMsg(void)
{

    GUIMESSAGE *pMsg = NULL;
    pMsg = GetCurrMessage();

    /************************注册下部右侧选项卡处理函数************************/
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnLoss,pMsg);
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnAttenuation, pMsg);
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnREF,pMsg);
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnORL, pMsg);

    /************************注册下部左侧选项卡处理函数************************/
    LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrMeasBtnSlideLeft,pMsg);
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnSlideLeft,pMsg);
    LogoutMessageReg(GUIMESSAGE_MOV_CURSOR, pOtdrMeasBtnSlideLeft,pMsg);

    LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrMeasBtnSlideRight,pMsg);
    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnSlideRight,pMsg);
    LogoutMessageReg(GUIMESSAGE_MOV_CURSOR, pOtdrMeasBtnSlideRight,pMsg);

    LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrMeasBtnMarkpointRefresh,pMsg);
}

int SetOtdrMeasEnable(WNDOTDRMEAS *pMeasWnd,int iEnable,GUIWINDOW *pWndObj)
{
    if (pMeasWnd->iEnable != iEnable)
    {
        if (iEnable)
        {
            AddOtdrMeasToComp( pWndObj);
            LoginOtdrMeasMsg(pMeasWnd);
        }
        else
        {
            DelOtdrMeasComp(pWndObj);
            LogoutOtdrMeasMsg();
        }

        //设置使能标志
        pMeasWnd->iEnable = iEnable;
    }
    return 0;
}

int RefreshOtdrMeasWnd(WNDOTDRMEAS *pWnd)
{
    int iRet = 0;

    if(pWnd->iEnable == 0)
    {
        return -1;
    }

    LoopOtdrMeasWnd(pWnd);

    OtdrMeasWndPaint(pWnd);

    return iRet;
}

/***
  * 功能：
                初始化文本资源
  * 参数：
                无
  * 返回：
             成功返回零，失败返回非零值
  * 备注：
***/
static int OtdrMeasTextRes_Init(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    WNDOTDRMEAS *pMeas = (WNDOTDRMEAS *)pInArg;

    /**************************初始化下部左侧文本信息**************************/
    pOtdrMeasStrMarkLineInfo = TransString("MEASURE_LBL_MARKLINEINF");
    pOtdrMeasStrMarkLine = TransString("MEASURE_LBL_MARKLINE");
    pOtdrMeasStrMarkNum[0] = TransString("A  :");
    pOtdrMeasStrMarkNum[1] = TransString("B  :");
    pOtdrMeasStrMarkNum[2] = TransString("B-A:");

    pOtdrMeasStrMark1Dist = TransString("");
    pOtdrMeasStrMark2Dist = TransString("");
    pOtdrMeasStrMark21Dist = TransString("");
    pOtdrMeasStrMark1dB = TransString("");
    pOtdrMeasStrMark2dB = TransString("");
    pOtdrMeasStrMark21dB = TransString("");

    pOtdrMeasStrCurMarkNum = TransString(pMarkNum[pMeas->enPitchOn]);

    /**************************初始化下部右侧文本信息**************************/
    pOtdrMeasStrResult = TransString("MEASURE_LBL_RESULT");
    pOtdrMeasStrLoss = TransString("MEASURE_LBL_LOSS");
    pOtdrMeasStrAttenuation = TransString("MEASURE_LBL_ATTENUATION");
    pOtdrMeasStrREF = TransString("MEASURE_LBL_REFLECT");
    pOtdrMeasStrORL = TransString("MAESURE_LBL_ORL");

    pOtdrMeasStrLossTitle = TransString("MEASURE_LBL_LOSS_DB");
    pOtdrMeasStrLossVal = TransString("");

    pOtdrMeasStrLossInfo = pUser_Settings->sCommonSetting.iLossMethodMark ? 
                            TransString("MEASURE_LBL_TWOPOINTSLOSS") : 
                            TransString("MEASURE_LBL_FOREPOINTSLOSS");
                            
                            
    pOtdrMeasStrAtteTitle = TransString("MEASURE_LBL_ATTENUATION_DB");

    pOtdrMeasStrAtteVal = TransString("");
    
    pOtdrMeasStrAtteInfo = TransString("MEASURE_LBL_TWOPOINTSATTEN");
    pOtdrMeasStrRefTitle = TransString("MEASURE_LBL_REFLECT_DB");
    pOtdrMeasStrRefVal = TransString("");
    pOtdrMeasStrRefInfo = TransString("MEASURE_LBL_REFLECT_INFO");
    pOtdrMeasStrORLVal1 = TransString("");
    pOtdrMeasStrORLVal2 = TransString("------");
    pOtdrMeasStrORLTitle = TransString("MEASURE_LBL_LIGHTREFLECT");
    pOtdrMeasStrORLInfo1 = TransString("MEASURE_LBL_A_BLREFLECT");
    pOtdrMeasStrORLInfo2 = TransString("MAESURE_LBL_SPANORL");

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
static int OtdrMeasTextRes_Exit(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i =0;

    /***************************释放下部左侧文本信息***************************/
    GuiMemFree(pOtdrMeasStrMarkLineInfo);
    GuiMemFree(pOtdrMeasStrMarkLine);
    GuiMemFree(pOtdrMeasStrMark1Dist);
    GuiMemFree(pOtdrMeasStrMark1dB);
    GuiMemFree(pOtdrMeasStrMark2Dist);
    GuiMemFree(pOtdrMeasStrMark2dB);
    GuiMemFree(pOtdrMeasStrMark21Dist);
    GuiMemFree(pOtdrMeasStrMark21dB);
    GuiMemFree(pOtdrMeasStrCurMarkNum);
    for(i=0; i<3; i++)
    {
        GuiMemFree(pOtdrMeasStrMarkNum[i]);
    }

    /***************************释放下部右侧文本信息***************************/
    GuiMemFree(pOtdrMeasStrResult);
    GuiMemFree(pOtdrMeasStrLoss);
    GuiMemFree(pOtdrMeasStrAttenuation);
    GuiMemFree(pOtdrMeasStrREF);
    GuiMemFree(pOtdrMeasStrORL);
    GuiMemFree(pOtdrMeasStrLossTitle);
    GuiMemFree(pOtdrMeasStrLossVal);
    GuiMemFree(pOtdrMeasStrLossInfo);
    GuiMemFree(pOtdrMeasStrAtteTitle);
    GuiMemFree(pOtdrMeasStrAtteVal);
    GuiMemFree(pOtdrMeasStrAtteInfo);
    GuiMemFree(pOtdrMeasStrRefTitle);
    GuiMemFree(pOtdrMeasStrRefVal);
    GuiMemFree(pOtdrMeasStrRefInfo);
    GuiMemFree(pOtdrMeasStrORLVal1);
    GuiMemFree(pOtdrMeasStrORLVal2);
    GuiMemFree(pOtdrMeasStrORLTitle);
    GuiMemFree(pOtdrMeasStrORLInfo1);
    GuiMemFree(pOtdrMeasStrORLInfo2);

    return iReturn;
}
