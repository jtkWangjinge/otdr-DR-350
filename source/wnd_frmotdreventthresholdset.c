/*******************************************************************************
* Copyright(c)2017，大豪信息技术(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdreventthresholdset.c
* 摘    要：  实现主窗体frmotdreventthresholdset的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  Bihong
* 完成日期：  2017.8.16
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmotdreventthresholdset.h"

/*************************************
* 定义wnd_frmotdreventthresholdset.c引用其他头文件
*************************************/
#include "wnd_frmsampleset.h"
#include "wnd_frmcommonset.h"
#include "wnd_frmanalysset.h"
#include "guipicture.h"
#include "wnd_global.h"
#include "app_global.h"
#include "app_frminit.h"
#include "wnd_frmime.h"
#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"
#include "wnd_stack.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "app_frmotdr.h"
#include "app_systemsettings.h"
#include "app_unitconverter.h"

static float rangeValues[7][2] = {{0.015f,5.000f},{0.015f,5.000f},{-80.0f,0.0f},
                                  {0.000f,5.000f},{0.000f,45.000f},{0.000f,300.000f},
                                  {0.000f,100.000f}};    
int iTouchPoint = 0;
/*************************************
* 定义wnd_frmotdreventthresholdset.c中内部使用变量
*************************************/
//自定义设置结构体指针
static OTHER_SETTING *pOtdrPassThreshold = NULL;

/***********************************
* 定义wnd_frmotdreventthresholdset.c中的窗体资源
***********************************/
static GUIWINDOW *pFrmOtdrEventThresholdSet = NULL;

/***********************************
* 定义wnd_frmotdreventthresholdset.c中的文本资源
***********************************/
static GUICHAR *pOtdrEventThresholdSetStrTitle = NULL;    
static GUICHAR *pOtdrEventThresholdSetStrTitleOTDRSettings = NULL;    
static GUICHAR *pOtdrEventThresholdSetStrParaT = NULL;  
static GUICHAR *pOtdrEventThresholdSetStrWaveT = NULL;
static GUICHAR *pOtdrEventThresholdSetStrWave[3] = {NULL};
static GUICHAR *pOtdrEventThresholdSetStrParaL[7] = {NULL};
static GUICHAR *pOtdrEventThresholdSetStrParaR[7] = {NULL};
static GUICHAR *pOtdrEventThresholdSetStrUnit[7] = {NULL};
static GUICHAR *pOtdrEventThresholdSetStrSelectAll = NULL;
static GUICHAR *pOtdrEventThresholdSetStrSelectNone = NULL;
static GUICHAR *pOtdrEventThresholdSetStrDefValue = NULL;


/***********************************
* 定义wnd_frmotdreventthresholdset.c中的控件资源
***********************************/
static GUIPICTURE *pOtdrEventThresholdSetBg = NULL;
static GUIPICTURE *pOtdrEventThresholdSetBgTableTitle = NULL;
static GUIPICTURE *pOtdrEventThresholdSetWaveCheckBox[3] = {NULL};
static GUIPICTURE *pOtdrEventThresholdSetParaCheckBox[7] = {NULL};
static GUIPICTURE *pOtdrEventThresholdSetParaEnterBg[7] = {NULL};
static GUIPICTURE *pOtdrEventThresholdSetBtnSelectAll = NULL;
static GUIPICTURE *pOtdrEventThresholdSetBtnSelectNone = NULL;
static GUIPICTURE *pOtdrEventThresholdSetBtnDef = NULL;


/***********************************
* 定义wnd_frmotdreventthresholdset.c中的标签资源
***********************************/
static GUILABEL *pOtdrEventThresholdSetLblTitle = NULL;    
static GUILABEL *pOtdrEventThresholdSetLblTitleOTDRSettings = NULL;    
static GUILABEL *pOtdrEventThresholdSetLblParaT = NULL;
static GUILABEL *pOtdrEventThresholdSetLblWaveT = NULL;
static GUILABEL *pOtdrEventThresholdSetLblWave[3] = {NULL};
static GUILABEL *pOtdrEventThresholdSetLblParaL[7] = {NULL};
static GUILABEL *pOtdrEventThresholdSetLblParaR[7] = {NULL};
static GUILABEL *pOtdrEventThresholdSetLblUnit[7] = {NULL};
static GUILABEL *pOtdrEventThresholdSetLblSelectAll = NULL;
static GUILABEL *pOtdrEventThresholdSetLblSelectNone = NULL;
static GUILABEL *pOtdrEventThresholdSetLblDefValue = NULL;

/*******************************
* 定义wnd_frmotdreventthresholdset.c中的控件
*******************************/
static WNDMENU1  *pOtdrEventThresholdSetMenu = NULL;

/*********************************
*声明wnd_frmcommonset.c所引用的外部变量
*********************************/

/**********************************
*声明wnd_frmotdreventthresholdset.c中的内部函数
**********************************/
//相关参数初始化和清理 
static int OtdrEventThresholdInitPara(void);
//退出时清理函数
static int OtdrEventThresholdClearPara(void);
//熔接损耗阈值输入回调 
static void InputSplicLossThresholdBackFunc(void);
//连接损耗阈值输入回调
static void InputConnectLossThresholdBackFunc(void);
//反射率阈值输入回调
static void InputReturnThresholdBackFunc(void);
//衰减率阈值输入回调 
static void InputAttenuationThresholdBackFunc(void);
//跨段损耗输入回调
static void InputRegionLossThresholdBackFunc(void);
//跨段长度输入回调
static void InputRegionLengthThresholdBackFunc(void);
//跨段光回损输入回调
static void InputRegionReturnLossThresholdBackFunc(void);
//刷新界面
static void Refresh(void);

//菜单控件点击回调函数
static void OtdrEventThresholdSetMenuCallBack(int iOption);


//文本初始化和销毁
static int FrmOtdrEventThresholdSetRes_Init(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int FrmOtdrEventThresholdSetRes_Exit(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
//各种点击处理函数
static int OtdrEventThresholdSetSelectAll_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetSelectAll_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetSelectNone_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetSelectNone_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetDefvalue_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetDefvalue_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetInputPara_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetInputPara_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int OtdrEventThresholdSetWaveCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetWaveCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int OtdrEventThresholdSetParaCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int OtdrEventThresholdSetParaCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);					   
/***
  * 功能：
        窗体的初始化函数，建立窗体控件、注册消息处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOtdrEventThresholdSetInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
	//临时变量定义 
	int i = 0;
    GUIMESSAGE *pMsg = NULL;

	unsigned int strMenu[] = {	
		1
	};
		
	//得到当前窗体对象 
	pFrmOtdrEventThresholdSet = (GUIWINDOW *)pWndObj;
	//初始化参数
	OtdrEventThresholdInitPara();
    FrmOtdrEventThresholdSetRes_Init(NULL, 0, NULL, 0);
	
    //桌面上的控件
    pOtdrEventThresholdSetBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT ,BmpFileDirectory"bg_otdr_judgethreshold.bmp");
    pOtdrEventThresholdSetBgTableTitle = CreatePhoto("otdr_top1f");
	//桌面上的标签 
	pOtdrEventThresholdSetLblTitle = CreateLabel(0, 24, 100, 24, pOtdrEventThresholdSetStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrEventThresholdSetLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrEventThresholdSetLblTitle);
	pOtdrEventThresholdSetLblTitleOTDRSettings= CreateLabel(401, 24, 281, 24, pOtdrEventThresholdSetStrTitleOTDRSettings);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrEventThresholdSetLblTitleOTDRSettings);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrEventThresholdSetLblTitleOTDRSettings);

	pOtdrEventThresholdSetLblParaT = CreateLabel(26, 66, 400, 24, pOtdrEventThresholdSetStrParaT);
	pOtdrEventThresholdSetLblWaveT = CreateLabel(48, 130, 120, 24, pOtdrEventThresholdSetStrWaveT);
    pOtdrEventThresholdSetLblSelectAll = CreateLabel(530, 299, 138, 24, pOtdrEventThresholdSetStrSelectAll);
    pOtdrEventThresholdSetLblSelectNone = CreateLabel(530, 359, 138, 24, pOtdrEventThresholdSetStrSelectNone);
	pOtdrEventThresholdSetLblDefValue= CreateLabel(530, 419, 138, 24, pOtdrEventThresholdSetStrDefValue);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrEventThresholdSetLblSelectAll);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrEventThresholdSetLblSelectNone);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrEventThresholdSetLblDefValue);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pOtdrEventThresholdSetLblParaT);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrEventThresholdSetLblWaveT);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblParaT);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblSelectAll);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblSelectNone);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblDefValue);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblWaveT);
    pOtdrEventThresholdSetWaveCheckBox[0] = CreatePicture(168, 126, 25, 25, BmpFileDirectory"bg_otdr_judgethreshold_wave_checkbox_unselect.bmp");
    pOtdrEventThresholdSetWaveCheckBox[1] = CreatePicture(278, 126, 25, 25, BmpFileDirectory"bg_otdr_judgethreshold_wave_checkbox_unselect.bmp");
    pOtdrEventThresholdSetWaveCheckBox[2] = CreatePicture(388, 126, 25, 25, BmpFileDirectory"bg_otdr_judgethreshold_wave_checkbox_unselect.bmp");

    pOtdrEventThresholdSetLblWave[0] = CreateLabel(208, 130, 70, 24, pOtdrEventThresholdSetStrWave[0]);
    pOtdrEventThresholdSetLblWave[1] = CreateLabel(318, 130, 70, 24, pOtdrEventThresholdSetStrWave[1]);
    pOtdrEventThresholdSetLblWave[2] = CreateLabel(428, 130, 70, 24, pOtdrEventThresholdSetStrWave[2]);
    
	for (i = 0; i < 7; ++i)
	{
	    pOtdrEventThresholdSetParaCheckBox[i] = CreatePicture(48, 171 + i*35, 25, 25, 
									 BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp");
	    pOtdrEventThresholdSetParaEnterBg[i] = CreatePicture(208, 169 + i*35, 166, 30, 
									 BmpFileDirectory"bg_otdr_judgethreshold_enable.bmp");

		pOtdrEventThresholdSetLblParaL[i] = CreateLabel(78, 175 + 35 * i, 125, 24,   
							   				pOtdrEventThresholdSetStrParaL[i]);
		pOtdrEventThresholdSetLblParaR[i] = CreateLabel(208, 175 + 35 * i, 165, 24,   
							   				pOtdrEventThresholdSetStrParaR[i]);
		pOtdrEventThresholdSetLblUnit[i] = CreateLabel(382, 175 + 35 * i, 100, 24,   
							   				pOtdrEventThresholdSetStrUnit[i]);
        
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pOtdrEventThresholdSetLblParaL[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pOtdrEventThresholdSetLblUnit[i]);
        SetLabelAlign(GUILABEL_ALIGN_CENTER, pOtdrEventThresholdSetLblParaR[i]);
        
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblParaL[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblParaR[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblUnit[i]);
	}

    pOtdrEventThresholdSetBtnSelectAll = CreatePicture(530, 287, 138, 48, 
									 BmpFileDirectory"bg_otdr_judgethreshold_default_up.bmp");
    pOtdrEventThresholdSetBtnSelectNone = CreatePicture(530, 347, 138, 48, 
									 BmpFileDirectory"bg_otdr_judgethreshold_default_up.bmp");
    pOtdrEventThresholdSetBtnDef = CreatePicture(530, 407, 138, 48, 
									 BmpFileDirectory"bg_otdr_judgethreshold_default_up.bmp");
 	//创建右侧菜单控件
	pOtdrEventThresholdSetMenu = CreateWndMenu1(4,sizeof(strMenu), strMenu, 
								   (UINT16)(MENU_BACK | ~MENU_HOME), 3, 0, 40, 
						  		   OtdrEventThresholdSetMenuCallBack);
	
	//注册窗体(因为所有的按键事件都统一由窗体进行处理) 
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), 
                  pFrmOtdrEventThresholdSet, pFrmOtdrEventThresholdSet);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrEventThresholdSetBtnSelectAll, 
				  pFrmOtdrEventThresholdSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrEventThresholdSetBtnSelectNone, 
				  pFrmOtdrEventThresholdSet);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pOtdrEventThresholdSetBtnDef, 
				  pFrmOtdrEventThresholdSet);
	for (i = 0; i < 3; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pOtdrEventThresholdSetWaveCheckBox[i], pFrmOtdrEventThresholdSet);		
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL), 
					  pOtdrEventThresholdSetLblWave[i], pFrmOtdrEventThresholdSet);

	}

    for (i = 0; i < 7; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pOtdrEventThresholdSetParaCheckBox[i], pFrmOtdrEventThresholdSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pOtdrEventThresholdSetParaEnterBg[i], pFrmOtdrEventThresholdSet);		
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL), 
					  pOtdrEventThresholdSetLblParaL[i], pFrmOtdrEventThresholdSet);
	}

	//注册菜单控件
	AddWndMenuToComp1(pOtdrEventThresholdSetMenu, pFrmOtdrEventThresholdSet);
	

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    pMsg = GetCurrMessage();


	//注册桌面上控件的消息处理 
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetBtnSelectAll, 
                    OtdrEventThresholdSetSelectAll_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetBtnSelectAll, 
                    OtdrEventThresholdSetSelectAll_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetBtnSelectNone, 
                    OtdrEventThresholdSetSelectNone_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetBtnSelectNone, 
                    OtdrEventThresholdSetSelectNone_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetBtnDef, 
                    OtdrEventThresholdSetDefvalue_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetBtnDef, 
                    OtdrEventThresholdSetDefvalue_Up, NULL, 0, pMsg);

	//输入框消息注册
	for (i = 0; i < 3; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetWaveCheckBox[i], 
                    	OtdrEventThresholdSetWaveCheckBox_Down, (void *)(i), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetWaveCheckBox[i], 
                    	OtdrEventThresholdSetWaveCheckBox_Up, (void *)(i), sizeof(int), pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetLblWave[i], 
                    	OtdrEventThresholdSetWaveCheckBox_Down, (void *)(i), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetLblWave[i], 
                    	OtdrEventThresholdSetWaveCheckBox_Up, (void *)(i), sizeof(int), pMsg);

	}

    for (i = 0; i < 7; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetParaCheckBox[i], 
                    	OtdrEventThresholdSetParaCheckBox_Down, (void *)(i), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetParaCheckBox[i], 
                    	OtdrEventThresholdSetParaCheckBox_Up, (void *)(i), sizeof(int), pMsg);

		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetLblParaL[i], 
                    	OtdrEventThresholdSetParaCheckBox_Down, (void *)(i), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetLblParaL[i], 
                    	OtdrEventThresholdSetParaCheckBox_Up, (void *)(i), sizeof(int), pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrEventThresholdSetParaEnterBg[i], 
                    	OtdrEventThresholdSetInputPara_Down, (void *)(i), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrEventThresholdSetParaEnterBg[i], 
                    	OtdrEventThresholdSetInputPara_Up, (void *)(i), sizeof(int), pMsg);
	}
	//注册右侧菜单控件的消息处理函数
	LoginWndMenuToMsg1(pOtdrEventThresholdSetMenu, pFrmOtdrEventThresholdSet);

	return iRet;
}

/***
  * 功能：
        窗体的退出函数，释放所有资源
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOtdrEventThresholdSetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	int i = 0;

	//得到当前窗体对象
    pFrmOtdrEventThresholdSet = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
	//必须在持有消息队列互斥锁的情况下操作
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);
	
    //从当前窗体中注销窗体控件
	//必须在持有控件队列互斥锁的情况下操作
    ClearWindowComp(pFrmOtdrEventThresholdSet);

	//销毁控件
	//桌面上的控件
	DestroyPicture(&pOtdrEventThresholdSetBg);
    DestroyPicture(&pOtdrEventThresholdSetBgTableTitle);
    DestroyPicture(&pOtdrEventThresholdSetBtnSelectAll);
    DestroyPicture(&pOtdrEventThresholdSetBtnSelectNone);
	DestroyPicture(&pOtdrEventThresholdSetBtnDef);
	
	//销毁标签
	//桌面上的标签
	DestroyLabel(&pOtdrEventThresholdSetLblTitle);    
	DestroyLabel(&pOtdrEventThresholdSetLblTitleOTDRSettings);    
	DestroyLabel(&pOtdrEventThresholdSetLblParaT); 
	DestroyLabel(&pOtdrEventThresholdSetLblWaveT);
    DestroyLabel(&pOtdrEventThresholdSetLblSelectAll);
    DestroyLabel(&pOtdrEventThresholdSetLblSelectNone);
	DestroyLabel(&pOtdrEventThresholdSetLblDefValue);
	for (i = 0; i < 3; ++i)
	{
		DestroyPicture(&pOtdrEventThresholdSetWaveCheckBox[i]);
		DestroyLabel(&pOtdrEventThresholdSetLblWave[i]);
	}
    
    for (i = 0; i < 7; ++i)
	{
		DestroyLabel(&pOtdrEventThresholdSetLblParaL[i]);
		DestroyLabel(&pOtdrEventThresholdSetLblParaR[i]);
		DestroyLabel(&pOtdrEventThresholdSetLblUnit[i]);
        
		DestroyPicture(&pOtdrEventThresholdSetParaCheckBox[i]);
		DestroyPicture(&pOtdrEventThresholdSetParaEnterBg[i]);
	}

	//菜单区的控件
	DestroyWndMenu1(&pOtdrEventThresholdSetMenu);

	//释放文本资源
    FrmOtdrEventThresholdSetRes_Exit(NULL, 0, NULL, 0);

	//清理系统相关参数
	OtdrEventThresholdClearPara();

	//退出并保存
    SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sOtherSetting, sizeof(OTHER_SETTING), RESULT_SET);
    SaveSettings(RESULT_SET);

	return iRet;
}

/***
  * 功能：
        窗体的绘制函数，绘制整个窗体
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOtdrEventThresholdSetPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

	//得到当前窗体对象
    pFrmOtdrEventThresholdSet = (GUIWINDOW *) pWndObj;

	//桌面上的控件
	DisplayPicture(pOtdrEventThresholdSetBg);
    DisplayPicture(pOtdrEventThresholdSetBgTableTitle);
    DisplayPicture(pOtdrEventThresholdSetBtnSelectAll);
    DisplayPicture(pOtdrEventThresholdSetBtnSelectNone);
	DisplayPicture(pOtdrEventThresholdSetBtnDef);
	//桌面上的标签
	DisplayLabel(pOtdrEventThresholdSetLblTitle);    
	DisplayLabel(pOtdrEventThresholdSetLblTitleOTDRSettings);    
	DisplayLabel(pOtdrEventThresholdSetLblParaT);   
	DisplayLabel(pOtdrEventThresholdSetLblWaveT); 
    DisplayLabel(pOtdrEventThresholdSetLblSelectAll);
    DisplayLabel(pOtdrEventThresholdSetLblSelectNone);
	DisplayLabel(pOtdrEventThresholdSetLblDefValue);

    Refresh();
	//菜单区的控件
	DisplayWndMenu1(pOtdrEventThresholdSetMenu);

	SetPowerEnable(1, 1);
	
	//刷新帧缓冲
	RefreshScreen(__FILE__, __func__, __LINE__);
	
    return iRet;
}

/***
  * 功能：
        窗体的循环函数，进行窗体循环
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOtdrEventThresholdSetLoop(void *pWndObj)
{
	//错误标志、返回值定义
    int iRet = 0;
	SendWndMsg_LoopDisable(pWndObj);
	return iRet;
}

/***
  * 功能：
        窗体的挂起函数，进行窗体挂起前预处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOtdrEventThresholdSetPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体的恢复函数，进行窗体恢复前预处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOtdrEventThresholdSetResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        初始化文本资源
  * 参数：
  		//...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmOtdrEventThresholdSetRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iErr = 0;
	//桌面上的文本
	pOtdrEventThresholdSetStrTitle 	= TransString("OTDR_THRESHOLD_SET");    
	pOtdrEventThresholdSetStrTitleOTDRSettings = TransString("OTDR_LBL_OTDRSET");    
	pOtdrEventThresholdSetStrParaT	= TransString("OTDR_THRESHOLD_THRESHLODTITLE");
	pOtdrEventThresholdSetStrWaveT	= TransString("OTDR_THRESHOLD_WAVE");
    pOtdrEventThresholdSetStrSelectAll = TransString("THRESOLD_SET_SELECT_ALL");
    pOtdrEventThresholdSetStrSelectNone = TransString("MESSAGEBOX_NO");
	pOtdrEventThresholdSetStrDefValue = TransString("OTDR_SAMPLESET_DEFAULT");

	//损耗参数
	pOtdrEventThresholdSetStrWave[0]= TransString("1310 nm");
	pOtdrEventThresholdSetStrWave[1]= TransString("1550 nm");
	pOtdrEventThresholdSetStrWave[2]= TransString("1625 nm");

    pOtdrEventThresholdSetStrParaL[0] = TransString("OTDR_THRESHOLD_SPLICELOSS");
    pOtdrEventThresholdSetStrParaL[1] = TransString("OTDR_THRESHOLD_CONNECTLOSS");
    pOtdrEventThresholdSetStrParaL[2] = TransString("OTDR_THRESHOLD_RETURN");
    pOtdrEventThresholdSetStrParaL[3] = TransString("OTDR_THRESHOLD_ATTENUATION");
    pOtdrEventThresholdSetStrParaL[4] = TransString("OTDR_THRESHOLD_REGIONLOSS");
    pOtdrEventThresholdSetStrParaL[5] = TransString("OTDR_THRESHOLD_REGIONRETURN");
    pOtdrEventThresholdSetStrParaL[6] = TransString("OTDR_THRESHOLD_REGION_ORL");

    pOtdrEventThresholdSetStrUnit[0] = TransString("dB");
    pOtdrEventThresholdSetStrUnit[1] = TransString("dB");
    pOtdrEventThresholdSetStrUnit[2] = TransString("dB");
    pOtdrEventThresholdSetStrUnit[3] = TransString("dB/km");
    pOtdrEventThresholdSetStrUnit[4] = TransString("dB");
    pOtdrEventThresholdSetStrUnit[5] = GetCurrSystemUnitGuiString(MODULE_UNIT_OTDR);
    pOtdrEventThresholdSetStrUnit[6] = TransString("dB");

    return iErr;
}

/***
  * 功能：
        释放文本资源
  * 参数：
  		//...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmOtdrEventThresholdSetRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iRet = 0;
	int i = 0;
    
	//桌面上的文本
    GuiMemFree(pOtdrEventThresholdSetStrTitle);    
    GuiMemFree(pOtdrEventThresholdSetStrTitleOTDRSettings);    
  	GuiMemFree(pOtdrEventThresholdSetStrParaT);  
  	GuiMemFree(pOtdrEventThresholdSetStrWaveT);
	GuiMemFree(pOtdrEventThresholdSetStrSelectAll);
    GuiMemFree(pOtdrEventThresholdSetStrSelectNone);
    GuiMemFree(pOtdrEventThresholdSetStrDefValue);
    for(i=0;i<7;i++)
    {
        if(i<3)
        {
	        GuiMemFree(pOtdrEventThresholdSetStrWave[i]);
        }
        GuiMemFree(pOtdrEventThresholdSetStrParaL[i]);
        GuiMemFree(pOtdrEventThresholdSetStrParaR[i]);
        GuiMemFree(pOtdrEventThresholdSetStrUnit[i]);
    }

	return iRet;
}

/***
  * 功能：
     	初始化全局相关参数
  * 参数：
  		无
  * 返回：
        成功返0，失败!0
  * 备注：
  		必须在窗体Init函数最开始调用
***/ 
static int OtdrEventThresholdInitPara(void)
{
    pOtdrPassThreshold = &pUser_Settings->sOtherSetting;
    if(pOtdrPassThreshold->iWaveLength == WAVELEN_1625)
    {
        pOtdrPassThreshold->iWaveLength = WAVELEN_1310;
    }
	return 0;
}
/***
  * 功能：
     	清理全局相关参数
  * 参数：
  		无
  * 返回：
        成功返0，失败!0
  * 备注：
  		必须在窗体Exit时调用
***/ 
static int OtdrEventThresholdClearPara(void)
{
	
	return 0;
}

/*重绘窗体函数*/
static void ReCreateEventThresholdWindow(GUIWINDOW **pWnd)
{	
   *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmOtdrEventThresholdSetInit, FrmOtdrEventThresholdSetExit, 
                        FrmOtdrEventThresholdSetPaint, FrmOtdrEventThresholdSetLoop, 
				        FrmOtdrEventThresholdSetPause, FrmOtdrEventThresholdSetResume,
                        NULL);
}

static void Refresh(void)
{
    int i = 0;
    int iCurrWavelength = pOtdrPassThreshold->iWaveLength;
    int isSelected = 0;
    char *checkBox[] = {BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp",
                        BmpFileDirectory"bg_otdr_judgethreshold_checkbox_select.bmp"};
                        
    char *paraBg[] = {BmpFileDirectory"bg_otdr_judgethreshold_disable.bmp",
                        BmpFileDirectory"bg_otdr_judgethreshold_enable.bmp"};

    GUIFONT *color[2] = {getGlobalFnt(EN_FONT_GRAY),
                        getGlobalFnt(EN_FONT_WHITE)};
                        
    for(i = 0; i < 3; i++)
    {
        if(i == iCurrWavelength)
        {
            SetPictureBitmap(checkBox[1], pOtdrEventThresholdSetWaveCheckBox[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrEventThresholdSetLblWave[i]);
        }
        else
        {
            SetPictureBitmap(checkBox[0], pOtdrEventThresholdSetWaveCheckBox[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblWave[i]);
        }
    }
    //隐藏1625波长的选中框以及文字显示
    if(iCurrWavelength == WAVELEN_1625)
    {
        //修改原有选中1625波长的设置，取代为1310波长的设置
        iCurrWavelength = WAVELEN_1310;
    }
    isSelected = pOtdrPassThreshold->PassThr.SpliceLossThr[iCurrWavelength].iSlectFlag;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[0]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[0]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[0]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[0]);
	pOtdrEventThresholdSetStrParaR[0] = Float2GuiString(LOSS_PRECISION, 
	    pOtdrPassThreshold->PassThr.SpliceLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[0], pOtdrEventThresholdSetLblParaR[0]);

    isSelected = pOtdrPassThreshold->PassThr.ConnectLossThr[iCurrWavelength].iSlectFlag;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[1]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[1]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[1]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[1]);
	pOtdrEventThresholdSetStrParaR[1] = Float2GuiString(LOSS_PRECISION, 
	    pOtdrPassThreshold->PassThr.ConnectLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[1], pOtdrEventThresholdSetLblParaR[1]);
    
    isSelected = pOtdrPassThreshold->PassThr.ReturnLossThr[iCurrWavelength].iSlectFlag;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[2]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[2]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[2]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[2]);
	pOtdrEventThresholdSetStrParaR[2] = Float2GuiString(REFLECTANCE_PRECISION, 
	    pOtdrPassThreshold->PassThr.ReturnLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[2], pOtdrEventThresholdSetLblParaR[2]);
    
    isSelected = pOtdrPassThreshold->PassThr.AttenuationThr[iCurrWavelength].iSlectFlag;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[3]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[3]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[3]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[3]);

	pOtdrEventThresholdSetStrParaR[3] = Float2GuiString(ATTENUATION_PRECISION, 
	    pOtdrPassThreshold->PassThr.AttenuationThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[3], pOtdrEventThresholdSetLblParaR[3]);
    
    isSelected = pOtdrPassThreshold->PassThr.RegionLossThr[iCurrWavelength].iSlectFlag;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[4]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[4]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[4]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[4]);
	pOtdrEventThresholdSetStrParaR[4] = Float2GuiString(LOSS_PRECISION, 
	    pOtdrPassThreshold->PassThr.RegionLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[4], pOtdrEventThresholdSetLblParaR[4]);
    
    isSelected = pOtdrPassThreshold->PassThr.RegionLengthThr[iCurrWavelength].iSlectFlag;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[5]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[5]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[5]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[5]);

	pOtdrEventThresholdSetStrParaR[5] = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR,
	    pOtdrPassThreshold->PassThr.fSpanLength_m[iCurrWavelength], 
	    pOtdrPassThreshold->PassThr.fSpanLength_ft[iCurrWavelength], 
	    pOtdrPassThreshold->PassThr.fSpanLength_mi[iCurrWavelength], 0);
	SetLabelText(pOtdrEventThresholdSetStrParaR[5], pOtdrEventThresholdSetLblParaR[5]);
    
    isSelected = pOtdrPassThreshold->PassThr.RegionReturnLossThr[iCurrWavelength].iSlectFlag;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[6]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[6]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[6]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[6]);
	pOtdrEventThresholdSetStrParaR[6] = Float2GuiString(ORL_PRECISION, 
	    pOtdrPassThreshold->PassThr.RegionReturnLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[6], pOtdrEventThresholdSetLblParaR[6]);

    for (i = 0; i < 3; ++i)
	{
	    if(i == WAVELEN_1625)//隐藏1625nm图标显示
        {
            SetPictureEnable(0, pOtdrEventThresholdSetWaveCheckBox[i]);
            SetLabelEnable(0, pOtdrEventThresholdSetLblWave[i]);
        } 
        else
        {
            DisplayPicture(pOtdrEventThresholdSetWaveCheckBox[i]);
		    DisplayLabel(pOtdrEventThresholdSetLblWave[i]);
        }
	}

	for (i = 0; i < 7; ++i)
	{
		DisplayPicture(pOtdrEventThresholdSetParaCheckBox[i]);
		DisplayPicture(pOtdrEventThresholdSetParaEnterBg[i]);
        
		DisplayLabel(pOtdrEventThresholdSetLblParaL[i]);
		DisplayLabel(pOtdrEventThresholdSetLblParaR[i]);
		DisplayLabel(pOtdrEventThresholdSetLblUnit[i]);
	}

}

static void EventThresholdIMEDialog(char *min, char *max)
{
    GUICHAR* curText1 = TransString("INPUTVALUE_OUT_OF_RANGE_1");
    GUICHAR* curText2 = TransString("MAXLINKERLOSS_OUT_OF_RANGE_2");
    GUICHAR* curAnd = TransString("AND");
    GUICHAR* string1Mem = TransString(min);
    GUICHAR* string2Mem = TransString(max);
    GUICHAR* warningData1 = StringCat(&curText1,string1Mem);
    GUICHAR* warningData2 = StringCat(&warningData1,curAnd);
    GUICHAR* warningData3 = StringCat(&warningData2,string2Mem);
    GUICHAR* warningData = StringCat(&warningData3,curText2);
    
    CreateIMEDialog(warningData);
    
    GuiMemFree(curText2);
    GuiMemFree(curAnd);
    GuiMemFree(string1Mem);
    GuiMemFree(string2Mem);
}

//熔接损耗阈值输入回调 
static void InputSplicLossThresholdBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	if (fTmp < rangeValues[0][0] || fTmp > rangeValues[0][1])
	{
	    char *min = Float2String(LOSS_PRECISION, rangeValues[0][0]);
	    char *max = Float2String(LOSS_PRECISION, rangeValues[0][1]);
		EventThresholdIMEDialog(min, max);
		free(min);
		free(max);
		return;
	}

	CODER_LOG(CoderYun, "IME: %f\n", fTmp);
	pOtdrPassThreshold->PassThr.SpliceLossThr[wavelength].fThrValue = fTmp;
}

//连接损耗阈值输入回调
static void InputConnectLossThresholdBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	if (fTmp < rangeValues[1][0] || fTmp > rangeValues[1][1])
	{
	    char *min = Float2String(LOSS_PRECISION, rangeValues[1][0]);
	    char *max = Float2String(LOSS_PRECISION, rangeValues[1][1]);
		EventThresholdIMEDialog(min, max);
		free(min);
		free(max);
		return;
	}

	CODER_LOG(CoderYun, "IME: %f\n", fTmp);
	pOtdrPassThreshold->PassThr.ConnectLossThr[wavelength].fThrValue = fTmp;
}

//反射率阈值输入回调
static void InputReturnThresholdBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < rangeValues[2][0] || fTmp > rangeValues[2][1])
	{
        char *min = Float2String(REFLECTANCE_PRECISION, rangeValues[2][0]);
        char *max = Float2String(REFLECTANCE_PRECISION, rangeValues[2][1]);
        EventThresholdIMEDialog(min, max);
        free(min);
        free(max);
		return;
	}

	CODER_LOG(CoderYun, "IME: %f\n", fTmp);
	pOtdrPassThreshold->PassThr.ReturnLossThr[wavelength].fThrValue = fTmp;
}

//衰减率阈值输入回调 
static void InputAttenuationThresholdBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);
	float min, max;
	
    min = rangeValues[3][0];
    max = rangeValues[3][1];

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < min || fTmp > max)
	{
        char *cmin = Float2String(ATTENUATION_PRECISION, rangeValues[3][0]);
        char *cmax = Float2String(ATTENUATION_PRECISION, rangeValues[3][1]);
        EventThresholdIMEDialog(cmin, cmax);
        free(cmin);
        free(cmax);
		return;
	}

	CODER_LOG(CoderYun, "IME: %f\n", fTmp);

    pOtdrPassThreshold->PassThr.AttenuationThr[wavelength].fThrValue = fTmp;
}

//跨段损耗输入回调
static void InputRegionLossThresholdBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < rangeValues[4][0] || fTmp > rangeValues[4][1])
	{
        char *min = Float2String(LOSS_PRECISION, rangeValues[4][0]);
        char *max = Float2String(LOSS_PRECISION, rangeValues[4][1]);
        EventThresholdIMEDialog(min, max);
        free(min);
        free(max);
		return;
	}

	CODER_LOG(CoderYun, "IME: %f\n", fTmp);
	pOtdrPassThreshold->PassThr.RegionLossThr[wavelength].fThrValue = fTmp;
}

//跨段长度输入回调
static void InputRegionLengthThresholdBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

    /*char *spanLengthUpper[UNIT_COUNT] = {"300000.0", "300.0000", 
        "984251.9685039", "984.2519685039", "186.4113577", "328083.9895013"};*/
    char *spanLengthUpper[UNIT_COUNT] = {(char*)"300000.0", (char*)"300.0000",
        (char*)"984252.0", (char*)"984.2520", (char*)"186.4114", (char*)"328083.9895013"};
	double fTmp = String2Float(cTmpBuff);
	double min, max;

    min = 0.0f;
    max = String2Float(spanLengthUpper[GetCurrSystemUnit(MODULE_UNIT_OTDR)]);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < min || fTmp > max)
	{
        char *cmin = Float2String(GetDistancePrecision(MODULE_UNIT_OTDR), min);
        char *cmax = Float2String(GetDistancePrecision(MODULE_UNIT_OTDR), max);
        EventThresholdIMEDialog(cmin, cmax);
        free(cmin);
        free(cmax);
		return;
	}

	CODER_LOG(CoderYun, "IME: %f\n", fTmp);

    pOtdrPassThreshold->PassThr.fSpanLength_m[wavelength] = 
        UnitConverter_Dist_System2M_String2Float(MODULE_UNIT_OTDR, cTmpBuff);
    max = String2Float(spanLengthUpper[UNIT_M]);
    if(pOtdrPassThreshold->PassThr.fSpanLength_m[wavelength] > max)
    {
       pOtdrPassThreshold->PassThr.fSpanLength_m[wavelength] = max; 
    }
    
    pOtdrPassThreshold->PassThr.fSpanLength_ft[wavelength] = 
        UnitConverter_Dist_System2FT_String2Float(MODULE_UNIT_OTDR, cTmpBuff);
    max = String2Float(spanLengthUpper[UNIT_FT]);
    if(pOtdrPassThreshold->PassThr.fSpanLength_ft[wavelength] > max)
    {
       pOtdrPassThreshold->PassThr.fSpanLength_ft[wavelength] = max; 
    }
    
    pOtdrPassThreshold->PassThr.fSpanLength_mi[wavelength] = 
        UnitConverter_Dist_System2MI_String2Float(MODULE_UNIT_OTDR, cTmpBuff);
    max = String2Float(spanLengthUpper[UNIT_MI]);
    if(pOtdrPassThreshold->PassThr.fSpanLength_mi[wavelength] > max)
    {
       pOtdrPassThreshold->PassThr.fSpanLength_mi[wavelength] = max; 
    }
}

//跨段光回损输入回调
static void InputRegionReturnLossThresholdBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < rangeValues[6][0] || fTmp > rangeValues[6][1])
	{
	    
        char *min = Float2String(ORL_PRECISION, rangeValues[6][0]);
        char *max = Float2String(ORL_PRECISION, rangeValues[6][1]);
        EventThresholdIMEDialog(min, max);
        free(min);
        free(max);
		return;
	}

	CODER_LOG(CoderYun, "IME: %f\n", fTmp);
	pOtdrPassThreshold->PassThr.RegionReturnLossThr[wavelength].fThrValue = fTmp;
}

//iSelectAll:1全选，0全不选
static void SetSelectThresold(int iSelectAll)
{
    int i = 0;
    int iCurrWavelength = pOtdrPassThreshold->iWaveLength;
    int isSelected = iSelectAll;
    char *checkBox[] = {BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp",
                        BmpFileDirectory"bg_otdr_judgethreshold_checkbox_select.bmp"};
                        
    char *paraBg[] = {BmpFileDirectory"bg_otdr_judgethreshold_disable.bmp",
                        BmpFileDirectory"bg_otdr_judgethreshold_enable.bmp"};

    GUIFONT *color[2] = {getGlobalFnt(EN_FONT_GRAY),
                        getGlobalFnt(EN_FONT_WHITE)};
                        
    for(i = 0; i < 3; i++)
    {
        if(i == iCurrWavelength)
        {
            SetPictureBitmap(checkBox[1], pOtdrEventThresholdSetWaveCheckBox[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pOtdrEventThresholdSetLblWave[i]);
        }
        else
        {
            SetPictureBitmap(checkBox[0], pOtdrEventThresholdSetWaveCheckBox[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrEventThresholdSetLblWave[i]);
        }
    }
    //隐藏1625波长的选中框以及文字显示
    if(iCurrWavelength == WAVELEN_1625)
    {
        //修改原有选中1625波长的设置，取代为1310波长的设置
        iCurrWavelength = WAVELEN_1310;
    }
    
    pOtdrPassThreshold->PassThr.SpliceLossThr[iCurrWavelength].iSlectFlag = isSelected;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[0]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[0]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[0]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[0]);
	pOtdrEventThresholdSetStrParaR[0] = Float2GuiString(LOSS_PRECISION, 
	    pOtdrPassThreshold->PassThr.SpliceLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[0], pOtdrEventThresholdSetLblParaR[0]);

    pOtdrPassThreshold->PassThr.ConnectLossThr[iCurrWavelength].iSlectFlag = isSelected;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[1]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[1]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[1]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[1]);
	pOtdrEventThresholdSetStrParaR[1] = Float2GuiString(LOSS_PRECISION, 
	    pOtdrPassThreshold->PassThr.ConnectLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[1], pOtdrEventThresholdSetLblParaR[1]);
    
    pOtdrPassThreshold->PassThr.ReturnLossThr[iCurrWavelength].iSlectFlag = isSelected;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[2]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[2]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[2]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[2]);
	pOtdrEventThresholdSetStrParaR[2] = Float2GuiString(REFLECTANCE_PRECISION, 
	    pOtdrPassThreshold->PassThr.ReturnLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[2], pOtdrEventThresholdSetLblParaR[2]);
    
    pOtdrPassThreshold->PassThr.AttenuationThr[iCurrWavelength].iSlectFlag = isSelected;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[3]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[3]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[3]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[3]);

	pOtdrEventThresholdSetStrParaR[3] = Float2GuiString(ATTENUATION_PRECISION, 
	    pOtdrPassThreshold->PassThr.AttenuationThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[3], pOtdrEventThresholdSetLblParaR[3]);
    
    pOtdrPassThreshold->PassThr.RegionLossThr[iCurrWavelength].iSlectFlag = isSelected;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[4]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[4]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[4]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[4]);
	pOtdrEventThresholdSetStrParaR[4] = Float2GuiString(LOSS_PRECISION, 
	    pOtdrPassThreshold->PassThr.RegionLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[4], pOtdrEventThresholdSetLblParaR[4]);
    
    pOtdrPassThreshold->PassThr.RegionLengthThr[iCurrWavelength].iSlectFlag = isSelected;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[5]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[5]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[5]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[5]);

	pOtdrEventThresholdSetStrParaR[5] = UnitConverter_Dist_Km2System_Float2GuiString(MODULE_UNIT_OTDR,
	    pOtdrPassThreshold->PassThr.RegionLengthThr[iCurrWavelength].fThrValue, 0);
	SetLabelText(pOtdrEventThresholdSetStrParaR[5], pOtdrEventThresholdSetLblParaR[5]);
    
    pOtdrPassThreshold->PassThr.RegionReturnLossThr[iCurrWavelength].iSlectFlag = isSelected;
    SetPictureBitmap(checkBox[isSelected], pOtdrEventThresholdSetParaCheckBox[6]);
    SetPictureBitmap(paraBg[isSelected], pOtdrEventThresholdSetParaEnterBg[6]);
    SetLabelFont(color[isSelected], pOtdrEventThresholdSetLblParaR[6]);
    GuiMemFree(pOtdrEventThresholdSetStrParaR[6]);
	pOtdrEventThresholdSetStrParaR[6] = Float2GuiString(ORL_PRECISION, 
	    pOtdrPassThreshold->PassThr.RegionReturnLossThr[iCurrWavelength].fThrValue);
	SetLabelText(pOtdrEventThresholdSetStrParaR[6], pOtdrEventThresholdSetLblParaR[6]);

    for (i = 0; i < 3; ++i)
	{
	    if(i == WAVELEN_1625)//隐藏1625nm图标显示
        {
            SetPictureEnable(0, pOtdrEventThresholdSetWaveCheckBox[i]);
            SetLabelEnable(0, pOtdrEventThresholdSetLblWave[i]);
        } 
        else
        {
            DisplayPicture(pOtdrEventThresholdSetWaveCheckBox[i]);
		    DisplayLabel(pOtdrEventThresholdSetLblWave[i]);
        }
	}

	for (i = 0; i < 7; ++i)
	{
		DisplayPicture(pOtdrEventThresholdSetParaCheckBox[i]);
		DisplayPicture(pOtdrEventThresholdSetParaEnterBg[i]);
        
		DisplayLabel(pOtdrEventThresholdSetLblParaL[i]);
		DisplayLabel(pOtdrEventThresholdSetLblParaR[i]);
		DisplayLabel(pOtdrEventThresholdSetLblUnit[i]);
	}
}

static int OtdrEventThresholdSetSelectAll_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen)
{
    TouchChange("bg_otdr_judgethreshold_default_down.bmp", pOtdrEventThresholdSetBtnSelectAll, 
				NULL, pOtdrEventThresholdSetLblSelectAll, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int OtdrEventThresholdSetSelectAll_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    TouchChange("bg_otdr_judgethreshold_default_up.bmp", pOtdrEventThresholdSetBtnSelectAll, 
				NULL, pOtdrEventThresholdSetLblSelectAll, 1);
    SetSelectThresold(1);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int OtdrEventThresholdSetSelectNone_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen)
{
    TouchChange("bg_otdr_judgethreshold_default_down.bmp", pOtdrEventThresholdSetBtnSelectNone, 
				NULL, pOtdrEventThresholdSetLblSelectNone, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int OtdrEventThresholdSetSelectNone_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    TouchChange("bg_otdr_judgethreshold_default_up.bmp", pOtdrEventThresholdSetBtnSelectNone, 
				NULL, pOtdrEventThresholdSetLblSelectNone, 1);
    SetSelectThresold(0);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

/*默认值按钮点击处理函数*/
static int OtdrEventThresholdSetDefvalue_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen)
{
	TouchChange("bg_otdr_judgethreshold_default_down.bmp", pOtdrEventThresholdSetBtnDef, 
				NULL, pOtdrEventThresholdSetLblDefValue, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}
static int OtdrEventThresholdSetDefvalue_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
	TouchChange("bg_otdr_judgethreshold_default_up.bmp", pOtdrEventThresholdSetBtnDef, 
				NULL, pOtdrEventThresholdSetLblDefValue, 1);
    int iCurrWavelength = pOtdrPassThreshold->iWaveLength;

    //隐藏1625波长的选中框以及文字显示
    if(iCurrWavelength == WAVELEN_1625)
    {
        //修改原有选中1625波长的设置，取代为1310波长的设置
        iCurrWavelength = WAVELEN_1310;
    }

    //恢复各项参数为默认值
    pOtdrPassThreshold->PassThr.SpliceLossThr[iCurrWavelength].iSlectFlag = 1;//熔接损耗
	pOtdrPassThreshold->PassThr.SpliceLossThr[iCurrWavelength].fThrValue = 0.3f;

	pOtdrPassThreshold->PassThr.ConnectLossThr[iCurrWavelength].iSlectFlag = 1;//接头损耗
	pOtdrPassThreshold->PassThr.ConnectLossThr[iCurrWavelength].fThrValue = 0.75f;

	pOtdrPassThreshold->PassThr.ReturnLossThr[iCurrWavelength].iSlectFlag = 1;//反射率损耗
	pOtdrPassThreshold->PassThr.ReturnLossThr[iCurrWavelength].fThrValue = -40.0f;

	pOtdrPassThreshold->PassThr.AttenuationThr[iCurrWavelength].iSlectFlag = 1;//衰减损耗
	pOtdrPassThreshold->PassThr.AttenuationThr[iCurrWavelength].fThrValue = 0.4f;

	pOtdrPassThreshold->PassThr.RegionLossThr[iCurrWavelength].iSlectFlag = 1;//跨段损耗
	pOtdrPassThreshold->PassThr.RegionLossThr[iCurrWavelength].fThrValue = 20.0f;

	pOtdrPassThreshold->PassThr.RegionLengthThr[iCurrWavelength].iSlectFlag = 1;//跨段长度
	pOtdrPassThreshold->PassThr.RegionLengthThr[iCurrWavelength].fThrValue = 0.0f;

	pOtdrPassThreshold->PassThr.RegionReturnLossThr[iCurrWavelength].iSlectFlag = 1;//跨段光回损
	pOtdrPassThreshold->PassThr.RegionReturnLossThr[iCurrWavelength].fThrValue = 15.0f;
	
	pOtdrPassThreshold->PassThr.fSpanLength_m[iCurrWavelength] = 0.0f;
	pOtdrPassThreshold->PassThr.fSpanLength_ft[iCurrWavelength] = 0.0f;
	pOtdrPassThreshold->PassThr.fSpanLength_mi[iCurrWavelength] = 0.0f;
    Refresh();
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}

/*分析参数点击处理函数*/
static int OtdrEventThresholdSetInputPara_Down(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{

	return 0;
}

static int OtdrEventThresholdSetInputPara_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
	char *buf = NULL;
	int iTmp = (int)pOutArg;
	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	if (iTmp > 7)
	{
		return -1;
	}

	switch (iTmp)
	{
	case 0://输入熔接损耗阈
	    if(pOtdrPassThreshold->PassThr.SpliceLossThr[wavelength].iSlectFlag)
	    {
            buf = Float2String(LOSS_PRECISION, pOtdrPassThreshold->PassThr.SpliceLossThr[wavelength].fThrValue);
            IMEInit(buf, 10, 1, ReCreateEventThresholdWindow, InputSplicLossThresholdBackFunc, NULL);
            free(buf);
            buf = NULL;
		}
		break;
	case 1://输入连接损耗阈值
	    if(pOtdrPassThreshold->PassThr.ConnectLossThr[wavelength].iSlectFlag)
	    {
            buf = Float2String(LOSS_PRECISION, pOtdrPassThreshold->PassThr.ConnectLossThr[wavelength].fThrValue);
            IMEInit(buf, 10, 1, ReCreateEventThresholdWindow, InputConnectLossThresholdBackFunc, NULL);
            free(buf);
            buf = NULL;
		}
		break;
	case 2://反射率阈值输入阈值
	    if(pOtdrPassThreshold->PassThr.ReturnLossThr[wavelength].iSlectFlag)
	    {
            buf = Float2String(REFLECTANCE_PRECISION, pOtdrPassThreshold->PassThr.ReturnLossThr[wavelength].fThrValue);
            IMEInit(buf, 10, 1, ReCreateEventThresholdWindow, InputReturnThresholdBackFunc, NULL);
            free(buf);
            buf = NULL;
		}
		break;
	case 3://衰减率阈值检测阈值
	    if(pOtdrPassThreshold->PassThr.AttenuationThr[wavelength].iSlectFlag)
	    {
            buf = Float2String(ATTENUATION_PRECISION, pOtdrPassThreshold->PassThr.AttenuationThr[wavelength].fThrValue);
            IMEInit(buf, 10, 1, ReCreateEventThresholdWindow, InputAttenuationThresholdBackFunc, NULL);
            free(buf);
            buf = NULL;
		}
		break;
	case 4://输入跨段损耗阈值
	    if(pOtdrPassThreshold->PassThr.RegionLossThr[wavelength].iSlectFlag)
	    {
            buf = Float2String(LOSS_PRECISION, pOtdrPassThreshold->PassThr.RegionLossThr[wavelength].fThrValue);
    		IMEInit(buf, 10, 1, ReCreateEventThresholdWindow, InputRegionLossThresholdBackFunc, NULL);
            free(buf);
            buf = NULL;
		}
		break;
	case 5://输入跨段长度阈值
	    if(pOtdrPassThreshold->PassThr.RegionLengthThr[wavelength].iSlectFlag)
	    {
            buf = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_OTDR,
                    pOtdrPassThreshold->PassThr.fSpanLength_m[wavelength], 
                    pOtdrPassThreshold->PassThr.fSpanLength_ft[wavelength], 
                    pOtdrPassThreshold->PassThr.fSpanLength_mi[wavelength], 0);
    		IMEInit(buf, 10, 1, ReCreateEventThresholdWindow, InputRegionLengthThresholdBackFunc, NULL);
            free(buf);
            buf = NULL;
		}
		break;
	case 6://输入跨段光回损阈值
	    if(pOtdrPassThreshold->PassThr.RegionReturnLossThr[wavelength].iSlectFlag)
	    {
            buf = Float2String(ORL_PRECISION, pOtdrPassThreshold->PassThr.RegionReturnLossThr[wavelength].fThrValue);
            IMEInit(buf, 10, 1, ReCreateEventThresholdWindow, InputRegionReturnLossThresholdBackFunc, NULL);
            free(buf);
            buf = NULL;
		}
		break;
	default:
		break;
	}
	return 0;
}

static int OtdrEventThresholdSetWaveCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    int temp = (int)pOutArg;
    pOtdrPassThreshold->iWaveLength = temp;
    Refresh();
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}

static int OtdrEventThresholdSetWaveCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}

static int OtdrEventThresholdSetParaCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    int temp = (int)pOutArg;
    int *p = NULL;
    switch(temp)
    {
        case 0:
            p = &pOtdrPassThreshold->PassThr.SpliceLossThr[pOtdrPassThreshold->iWaveLength].iSlectFlag;
            break;
        case 1:
            p = &pOtdrPassThreshold->PassThr.ConnectLossThr[pOtdrPassThreshold->iWaveLength].iSlectFlag;
            break;
        case 2:
            p = &pOtdrPassThreshold->PassThr.ReturnLossThr[pOtdrPassThreshold->iWaveLength].iSlectFlag;
            break;
        case 3:
            p = &pOtdrPassThreshold->PassThr.AttenuationThr[pOtdrPassThreshold->iWaveLength].iSlectFlag;
            break;
        case 4:
            p = &pOtdrPassThreshold->PassThr.RegionLossThr[pOtdrPassThreshold->iWaveLength].iSlectFlag;
            break;
        case 5:
            p = &pOtdrPassThreshold->PassThr.RegionLengthThr[pOtdrPassThreshold->iWaveLength].iSlectFlag;
            break;
        case 6:
            p = &pOtdrPassThreshold->PassThr.RegionReturnLossThr[pOtdrPassThreshold->iWaveLength].iSlectFlag;
            break;
        default:
            break;
    }

    *p = *p?0:1;

    Refresh();
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

static int OtdrEventThresholdSetParaCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}

/*  侧边菜单控件回调函数 */
static void OtdrEventThresholdSetMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	Stack *ps = NULL;
	Item func;
	
	switch (iOption)
	{
	case 0://调用分析设置窗体 
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmCommonSetInit, FrmCommonSetExit, 
		                    FrmCommonSetPaint, FrmCommonSetLoop, 
					        FrmCommonSetPause, FrmCommonSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmOtdrEventThresholdSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 1://调用采样设置窗体 
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmSampleSetInit, FrmSampleSetExit, 
		                    FrmSampleSetPaint, FrmSampleSetLoop, 
					        FrmSampleSetPause, FrmSampleSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmOtdrEventThresholdSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调用分析设置窗体 
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmAnalysSetInit, FrmAnalysSetExit, 
		                    FrmAnalysSetPaint, FrmAnalysSetLoop, 
					        FrmAnalysSetPause, FrmAnalysSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmOtdrEventThresholdSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case BACK_DOWN://退出OTDR设置界面 
		ps = GetCurrWndStack();
		WndPop(ps, &func, pFrmOtdrEventThresholdSet);			
		break;
	case HOME_DOWN:
	 	break;
		
	default:
		break;
	}
}

