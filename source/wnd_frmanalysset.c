/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmanalysset.c
* 摘    要：  实现主窗体frmanalysset的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmanalysset.h"

/*************************************
* 定义wnd_frmanalysset.c引用其他头文件
*************************************/
#include "wnd_frmsampleset.h"
#include "wnd_frmcommonset.h"
#include "guipicture.h"
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmselector.h"
#include "app_frminit.h"
#include "wnd_frmime.h"
#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"
#include "wnd_stack.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "wnd_frmotdreventthresholdset.h"
#include "app_systemsettings.h"
#include "app_unitconverter.h"
#include "wnd_frmdialog.h"

/*************************************
* 定义wnd_frmanalysset.c中内部数据结构
*************************************/
struct OtdrAnalysSet
{
    int     iAutoAnlys;         //自动分析标志
    float   fLossThr[3];        //损耗分析阈值
    int     iEnableLaunchFiber; //注入光纤使能
    double  fLaunchFiberLen;    //注入光纤长度
    int     iEnableRecvFiber;   //接收光纤使能
    double  fRecvFiberLen;      //接收光纤长度
    double  fLaunchFiberLen_ft; //注入光纤长度
    double  fLaunchFiberLen_mi; //注入光纤长度
    double  fRecvFiberLen_ft;   //接收光纤长度
    double  fRecvFiberLen_mi;   //接收光纤长度
    int     iEnableLaunchFiberEvent; //注入光纤事件使能
    int     iEnableRecvFiberEvent;   //接收光纤事件使能
    int     iLaunchFiberEvent;       //注入光纤的事件编号
    int     iRecvFiberEvent;         //接收光纤的事件编号（从末端算起）
    int     iEventOrLength;          //按事件还是长度选项(0 长度， 非 0 事件)
    CALLLBACKHANDLE dialogCallBack;  //弹出框回调函数，不为NULL时弹出。
};

static char *AnalyzeCheckBoxBmp[] = {
    BmpFileDirectory"bg_comset_unselect.bmp",
    BmpFileDirectory"bg_comset_select.bmp"
};

/*************************************
* 定义wnd_frmanalysset.c中内部使用变量
*************************************/
//自定义设置结构体指针
struct OtdrAnalysSet *pAnalySet = NULL;

static PRECISION precision[3] = {LOSS_PRECISION, REFLECTANCE_PRECISION, LOSS_PRECISION};

/***********************************
* 定义wnd_frmanalysset.c中的窗体资源
***********************************/
static GUIWINDOW *pFrmAnalysSet = NULL;

/***********************************
* 定义wnd_frmanalysset.c中的文本资源
***********************************/
static GUICHAR *pAnalysSetStrTitle = NULL;
static GUICHAR *pAnalysSetStrTitleOTDRSettings = NULL;
static GUICHAR *pAnalysSetStrParaT = NULL;
static GUICHAR *pAnalysSetStrParaL[3] = {NULL};
static GUICHAR *pAnalysSetStrParaR[3] = {NULL};
static GUICHAR *pAnalysSetStrDefValue = NULL;

//注入光纤和接收光纤
static GUICHAR *pLaunchRecvText = NULL;
static GUICHAR *pLengthItemText = NULL;
static GUICHAR *pEventItemText = NULL;
static GUICHAR *pLaunchFiberLblText = NULL;
static GUICHAR *pRecvFiberLblText = NULL;
static GUICHAR *pLaunchFiberEditText = NULL;
static GUICHAR *pRecvFiberEditText = NULL;
static GUICHAR *pLaunchFiberUnitText = NULL;
static GUICHAR *pRecvFiberUnitText = NULL;
/***********************************
* 定义wnd_frmanalysset.c中的控件资源
***********************************/
static GUIPICTURE *pAnalysSetBg = NULL;
static GUIPICTURE *pAnalysSetBgTableTitle = NULL;
static GUIPICTURE *pAnalysSetInputPara[3] = {NULL};
static GUIPICTURE *pAnalysSetBtnDef = NULL;

//注入光纤和接收光纤
static GUIPICTURE *pLengthItemCheckBox = NULL;
static GUIPICTURE *pEventItemCheckBox = NULL;
static GUIPICTURE *pLaunchFiberCheckBox = NULL;
static GUIPICTURE *pRecvFiberCheckBox = NULL;
static GUIPICTURE *pLaunchFiberEdit = NULL;
static GUIPICTURE *pRecvFiberEdit = NULL;

/***********************************
* 定义wnd_frmanalysset.c中的标签资源
***********************************/
static GUILABEL *pAnalysSetLblTitle = NULL;    
static GUILABEL *pAnalysSetLblTitleOTDRSettings = NULL;    
static GUILABEL *pAnalysSetLblParaT = NULL;
static GUILABEL *pAnalysSetLblParaL[3] = {NULL};
static GUILABEL *pAnalysSetLblParaR[3] = {NULL};
static GUILABEL *pAnalysSetLblDefValue = NULL;

//注入光纤和接收光纤
static GUILABEL *pLaunchRecvLabel = NULL;
static GUILABEL *pLengthItemLblLabel = NULL;
static GUILABEL *pEventItemLblLabel = NULL;
static GUILABEL *pLaunchFiberLblLabel = NULL;
static GUILABEL *pRecvFiberLblLabel = NULL;
static GUILABEL *pLaunchFiberEditLabel = NULL;
static GUILABEL *pRecvFiberEditLabel = NULL;
static GUILABEL *pLaunchFiberUnitLabel = NULL;
static GUILABEL *pRecvFiberUnitLabel = NULL;

/*******************************
* 定义wnd_frmanalysset.c中的控件
*******************************/
static WNDMENU1  *pAnalysisMenu = NULL;

/**********************************
*声明wnd_frmanalysset.c中的内部函数
**********************************/
//相关参数初始化和清理 
static int AnalysInitPara(void);
//退出时清理函数
static int AnalysClearPara(void);
//熔接损耗输入回调 
static void InputSplicLossBackFunc(void);
//回波损耗输入回调
static void InputRetLossBackFunc(void);
//末端损耗输入回调
static void InputEndlossBackFunc(void);

//注入光纤和接收光纤输入回调
static void InputLaunchLenCallBack(void);
static void InputRecvLenCallBack(void);

//菜单控件点击回调函数
static void AnalysisMenuCallBack(int iOption);

/*重绘窗体函数*/
static void ReCreateAnalysSetWindow(GUIWINDOW **pWnd);

//文本初始化和销毁
static int FrmAnalysSetRes_Init(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int FrmAnalysSetRes_Exit(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
//各种点击处理函数 
static int AnalysSetDefvalue_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen);
static int AnalysSetDefvalue_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int AnalysSetInputPara_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int AnalysSetInputPara_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int AnalysLengthItemCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int AnalysLengthItemCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int AnalysEventItemCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int AnalysEventItemCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int AnalysLaunchCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int AnalysLaunchCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int AnalysRecvCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int AnalysRecvCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int AnalysLaunchEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int AnalysLaunchEdit_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int AnalysRecvEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int AnalysRecvEdit_Up(void *pInArg, int iInLen, 
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
int FrmAnalysSetInit(void *pWndObj)
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
	pFrmAnalysSet = (GUIWINDOW *)pWndObj;
	//初始化参数
	AnalysInitPara();
    FrmAnalysSetRes_Init(NULL, 0, NULL, 0);
	
    //桌面上的控件
    pAnalysSetBg = CreatePhoto("bg_analysis");
    pAnalysSetBgTableTitle = CreatePhoto("otdr_top1f");
	//桌面上的标签 
	pAnalysSetLblTitle = CreateLabel(0, 24, 100, 24, pAnalysSetStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pAnalysSetLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pAnalysSetLblTitle);
	pAnalysSetLblTitleOTDRSettings= CreateLabel(401, 24, 281, 24, pAnalysSetStrTitleOTDRSettings);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pAnalysSetLblTitleOTDRSettings);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pAnalysSetLblTitleOTDRSettings);

	pAnalysSetLblParaT = CreateLabel(90, 71, 500, 24, pAnalysSetStrParaT);
	pAnalysSetLblDefValue= CreateLabel(497, 210, 59, 24, pAnalysSetStrDefValue);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pAnalysSetLblDefValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pAnalysSetLblParaT);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pAnalysSetLblDefValue);
	CreateLoopPhoto(pAnalysSetInputPara, "input_loss_unpress", 0, 0, 3);
	for (i = 0; i < 3; ++i)
	{
		pAnalysSetLblParaL[i] = CreateLabel(130, 118 + 45 * i, 200, 24,   
							   				pAnalysSetStrParaL[i]);
		pAnalysSetLblParaR[i] = CreateLabel(337, 118 + 45 * i, 100, 24,   
							   				pAnalysSetStrParaR[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pAnalysSetLblParaR[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pAnalysSetLblParaL[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pAnalysSetLblParaL[i]);
	}

    //注入光纤和接收光纤
    pLaunchRecvLabel = CreateLabel(90, 268, 500, 24, pLaunchRecvText);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLaunchRecvLabel);

    pLengthItemLblLabel = CreateLabel(160, 317, 185, 24, pLengthItemText);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLengthItemLblLabel);
    pEventItemLblLabel = CreateLabel(394, 317, 185, 24, pEventItemText);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEventItemLblLabel);

    pLaunchFiberLblLabel = CreateLabel(160, 366, 240, 24, pLaunchFiberLblText);
  	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLaunchFiberLblLabel);
    pRecvFiberLblLabel = CreateLabel(160, 411, 240, 24, pRecvFiberLblText);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pRecvFiberLblLabel);
	
    int temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableLaunchFiber : pAnalySet->iEnableLaunchFiberEvent;
    pLaunchFiberEditLabel = CreateLabel(400, 366, 142, 24, pLaunchFiberEditText);
	SetLabelAlign(GUILABEL_ALIGN_RIGHT, pLaunchFiberEditLabel);
    if (!temp)
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pLaunchFiberEditLabel);
        
    temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableRecvFiber : pAnalySet->iEnableRecvFiberEvent;
    pRecvFiberEditLabel = CreateLabel(400, 411, 142, 24, pRecvFiberEditText);
	SetLabelAlign(GUILABEL_ALIGN_RIGHT, pRecvFiberEditLabel);
    if (!temp)
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pRecvFiberEditLabel);

    pLaunchFiberUnitLabel = CreateLabel(550, 366, 32, 24, pLaunchFiberUnitText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLaunchFiberUnitLabel);
    pRecvFiberUnitLabel = CreateLabel(550, 411, 32, 24, pRecvFiberUnitText);
  	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pRecvFiberUnitLabel);

    pLengthItemCheckBox = CreatePicture(130, 312, 25, 25,
        AnalyzeCheckBoxBmp[(pAnalySet->iEventOrLength == 0)]);
    pEventItemCheckBox = CreatePicture(364, 312, 25, 25,
        AnalyzeCheckBoxBmp[(pAnalySet->iEventOrLength != 0)]);

    temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableLaunchFiber : pAnalySet->iEnableLaunchFiberEvent;
    pLaunchFiberCheckBox = CreatePicture(130, 361, 25, 25,
        AnalyzeCheckBoxBmp[temp]);
    temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableRecvFiber : pAnalySet->iEnableRecvFiberEvent;
    pRecvFiberCheckBox = CreatePicture(130, 406, 25, 25,
        AnalyzeCheckBoxBmp[temp]);
    pLaunchFiberEdit = CreatePicture(400, 361, 145, 25, BmpFileDirectory"input_loss_unpress.bmp");
    pRecvFiberEdit = CreatePicture(400, 406, 145, 25, BmpFileDirectory"input_loss_unpress.bmp");

    pAnalysSetBtnDef = CreatePhoto("btn_default_unpress");
 	//创建右侧菜单控件
	pAnalysisMenu = CreateWndMenu1(4,sizeof(strMenu), strMenu, 
								   (UINT16)(MENU_BACK | ~MENU_HOME), 2, 0, 40, 
						  		   AnalysisMenuCallBack);
	
	//注册窗体(因为所有的按键事件都统一由窗体进行处理) 
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), 
                  pFrmAnalysSet, pFrmAnalysSet);


	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAnalysSetBtnDef, 
				  pFrmAnalysSet);
	for (i = 0; i < 3; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pAnalysSetInputPara[i], pFrmAnalysSet);
	}

    //注入光纤和接收光纤
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pLengthItemCheckBox, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pEventItemCheckBox, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pLaunchFiberCheckBox, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pRecvFiberCheckBox, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pLaunchFiberEdit, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pRecvFiberEdit, 
				  pFrmAnalysSet);
				  
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pLengthItemLblLabel, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pEventItemLblLabel, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pLaunchFiberLblLabel, 
				  pFrmAnalysSet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pRecvFiberLblLabel, 
				  pFrmAnalysSet);
    
    //注册菜单控件
	AddWndMenuToComp1(pAnalysisMenu, pFrmAnalysSet);
	

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    pMsg = GetCurrMessage();


	//注册桌面上控件的消息处理 
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAnalysSetBtnDef, 
                    AnalysSetDefvalue_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pAnalysSetBtnDef, 
                    AnalysSetDefvalue_Up, NULL, 0, pMsg);

	//输入框消息注册
	for (i = 0; i < 3; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAnalysSetInputPara[i], 
                    	AnalysSetInputPara_Down, (void *)(i + 1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pAnalysSetInputPara[i], 
                    	AnalysSetInputPara_Up, (void *)(i + 1), sizeof(int), pMsg);
	}
    
    //注入光纤和接收光纤	
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLengthItemCheckBox, 
                	AnalysLengthItemCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pLengthItemCheckBox, 
                	AnalysLengthItemCheckBox_Up, NULL, sizeof(int), pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLengthItemLblLabel, 
                	AnalysLengthItemCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pLengthItemLblLabel, 
                	AnalysLengthItemCheckBox_Up, NULL, sizeof(int), pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pEventItemCheckBox, 
                	AnalysEventItemCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pEventItemCheckBox, 
                	AnalysEventItemCheckBox_Up, NULL, sizeof(int), pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pEventItemLblLabel, 
                	AnalysEventItemCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pEventItemLblLabel, 
                	AnalysEventItemCheckBox_Up, NULL, sizeof(int), pMsg);
                	
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLaunchFiberCheckBox, 
                	AnalysLaunchCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pLaunchFiberCheckBox, 
                	AnalysLaunchCheckBox_Up, NULL, sizeof(int), pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLaunchFiberLblLabel, 
                	AnalysLaunchCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pLaunchFiberLblLabel, 
                	AnalysLaunchCheckBox_Up, NULL, sizeof(int), pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pRecvFiberCheckBox, 
                	AnalysRecvCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pRecvFiberCheckBox, 
                	AnalysRecvCheckBox_Up, NULL, sizeof(int), pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pRecvFiberLblLabel, 
                	AnalysRecvCheckBox_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pRecvFiberLblLabel, 
                	AnalysRecvCheckBox_Up, NULL, sizeof(int), pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLaunchFiberEdit, 
                	AnalysLaunchEdit_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pLaunchFiberEdit, 
                	AnalysLaunchEdit_Up, NULL, sizeof(int), pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pRecvFiberEdit, 
                	AnalysRecvEdit_Down, NULL, sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pRecvFiberEdit, 
                	AnalysRecvEdit_Up, NULL, sizeof(int), pMsg);

    //注册右侧菜单控件的消息处理函数
	LoginWndMenuToMsg1(pAnalysisMenu, pFrmAnalysSet);

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
int FrmAnalysSetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	int i = 0;

	//得到当前窗体对象
    pFrmAnalysSet = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
	//必须在持有消息队列互斥锁的情况下操作
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);
	
    //从当前窗体中注销窗体控件
	//必须在持有控件队列互斥锁的情况下操作
    ClearWindowComp(pFrmAnalysSet);

	//销毁控件
	//桌面上的控件
	DestroyPicture(&pAnalysSetBg);
    DestroyPicture(&pAnalysSetBgTableTitle);
	DestroyPicture(&pAnalysSetBtnDef);
	for (i = 0; i < 3; ++i)
	{
		DestroyPicture(&pAnalysSetInputPara[i]);
	}
	
	//销毁标签
	//桌面上的标签
	DestroyLabel(&pAnalysSetLblTitle);    
	DestroyLabel(&pAnalysSetLblTitleOTDRSettings);    
	DestroyLabel(&pAnalysSetLblParaT);
	DestroyLabel(&pAnalysSetLblDefValue);
	for (i = 0; i < 3; ++i)
	{
		DestroyLabel(&pAnalysSetLblParaL[i]);
		DestroyLabel(&pAnalysSetLblParaR[i]);
	}

    //注入光纤和接收光纤
    DestroyLabel(&pLaunchRecvLabel);
    DestroyLabel(&pLengthItemLblLabel);
    DestroyLabel(&pEventItemLblLabel);
    DestroyLabel(&pLaunchFiberLblLabel);
    DestroyLabel(&pRecvFiberLblLabel);
    DestroyLabel(&pLaunchFiberEditLabel);
    DestroyLabel(&pRecvFiberEditLabel);
    DestroyLabel(&pLaunchFiberUnitLabel);
    DestroyLabel(&pRecvFiberUnitLabel);
    DestroyPicture(&pLengthItemCheckBox);
    DestroyPicture(&pEventItemCheckBox);
    DestroyPicture(&pLaunchFiberCheckBox);
    DestroyPicture(&pRecvFiberCheckBox);
    DestroyPicture(&pLaunchFiberEdit);
    DestroyPicture(&pRecvFiberEdit);
    
	//菜单区的控件
	DestroyWndMenu1(&pAnalysisMenu);

	//释放文本资源
    FrmAnalysSetRes_Exit(NULL, 0, NULL, 0);

	//清理系统相关参数
	AnalysClearPara();

	//退出并保存
	SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sAnalysisSetting, sizeof(ANALYSIS_SETTING), ANALYSIS_SET);
    SaveSettings(ANALYSIS_SET);
    
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
int FrmAnalysSetPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
	int i = 0;

	if(pAnalySet->dialogCallBack != NULL)
	{
        DialogInit(120, 90, TransString("Warning"),
                            TransString("Are thresholds used in the measurement now?"),
                            0, 1, ReCreateAnalysSetWindow, pAnalySet->dialogCallBack, NULL);
                            
       pAnalySet->dialogCallBack = NULL;

       return 0;
	}

	//得到当前窗体对象
    pFrmAnalysSet = (GUIWINDOW *) pWndObj;

	//桌面上的控件
	DisplayPicture(pAnalysSetBg);
    DisplayPicture(pAnalysSetBgTableTitle);
	DisplayPicture(pAnalysSetBtnDef);
	for (i = 0; i < 3; ++i)
	{
		DisplayPicture(pAnalysSetInputPara[i]);
	}

	//桌面上的标签
	DisplayLabel(pAnalysSetLblTitle);    
	DisplayLabel(pAnalysSetLblTitleOTDRSettings);    
	DisplayLabel(pAnalysSetLblParaT); 
	DisplayLabel(pAnalysSetLblDefValue);
	for (i = 0; i < 3; ++i)
	{
		DisplayLabel(pAnalysSetLblParaL[i]);
		DisplayLabel(pAnalysSetLblParaR[i]);
	}

    //注入光纤和接收光纤
    DisplayLabel(pLaunchRecvLabel);
    DisplayLabel(pLengthItemLblLabel);
    DisplayLabel(pEventItemLblLabel);
    DisplayLabel(pLaunchFiberLblLabel);
    DisplayLabel(pRecvFiberLblLabel);
    
    if(pAnalySet->iEventOrLength == 0)
    {
        DisplayLabel(pLaunchFiberUnitLabel);
        DisplayLabel(pRecvFiberUnitLabel);
    }
    DisplayPicture(pLengthItemCheckBox);
    DisplayPicture(pEventItemCheckBox);
    DisplayPicture(pLaunchFiberCheckBox);
    DisplayPicture(pRecvFiberCheckBox);
    DisplayPicture(pLaunchFiberEdit);
    DisplayPicture(pRecvFiberEdit);
    DisplayLabel(pLaunchFiberEditLabel);
    DisplayLabel(pRecvFiberEditLabel);

	//菜单区的控件
	DisplayWndMenu1(pAnalysisMenu);

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
int FrmAnalysSetLoop(void *pWndObj)
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
int FrmAnalysSetPause(void *pWndObj)
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
int FrmAnalysSetResume(void *pWndObj)
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
static int FrmAnalysSetRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iErr = 0;

	//桌面上的文本
	pAnalysSetStrTitle 	= TransString("OTDR_ANALYSISSET_ANALYSISSET");
	pAnalysSetStrTitleOTDRSettings= TransString("OTDR_LBL_OTDRSET");
	pAnalysSetStrParaT	= TransString("OTDR_ANALYSISSET_ANALYSISPARA");
	pAnalysSetStrDefValue = TransString("OTDR_SAMPLESET_DEFAULT");

	//损耗参数
	pAnalysSetStrParaL[0]= TransString("OTDR_ANALYSISSET_SPLICELOSS");
	pAnalysSetStrParaL[1]= TransString("OTDR_ANALYSISSET_RETURNLOSS");
	pAnalysSetStrParaL[2]= TransString("OTDR_ANALYSISSET_ENDLOSS");

	pAnalysSetStrParaR[0] = Float2GuiString(LOSS_PRECISION, pAnalySet->fLossThr[0]);
	pAnalysSetStrParaR[1] = Float2GuiString(REFLECTANCE_PRECISION, pAnalySet->fLossThr[1]);
	pAnalysSetStrParaR[2] = Float2GuiString(LOSS_PRECISION, pAnalySet->fLossThr[2]);
	
    pLaunchRecvText  = TransString("ANALYZE_INJECT_AND_RECV");
    pLengthItemText = TransString("ANALYZE_BY_FIBER_LENGTHS");
    pEventItemText = TransString("ANALYZE_BY_EVENT");

    if(pAnalySet->iEventOrLength == 0)
    {
        pLaunchFiberLblText = TransString("ANALYZE_INJECT_LEN");
        pRecvFiberLblText = TransString("ANALYZE_RECV_LEN");
        pLaunchFiberEditText = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR, 
            pAnalySet->fLaunchFiberLen, pAnalySet->fLaunchFiberLen_ft, pAnalySet->fLaunchFiberLen_mi, 0);
        pRecvFiberEditText = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR, 
            pAnalySet->fRecvFiberLen, pAnalySet->fRecvFiberLen_ft, pAnalySet->fRecvFiberLen_mi, 0);
    }
    else
    {
        pLaunchFiberLblText = TransString("ANALYZE_LAUNCH_FIBER_EVENT");
        pRecvFiberLblText = TransString("ANALYZE_RECV_FIBER_EVENT");
        pLaunchFiberEditText = Int2GuiString(pAnalySet->iLaunchFiberEvent);
        pRecvFiberEditText = Int2GuiString(pAnalySet->iRecvFiberEvent);
    }
    
    pLaunchFiberUnitText = GetCurrSystemUnitGuiString(MODULE_UNIT_OTDR);
    pRecvFiberUnitText = GetCurrSystemUnitGuiString(MODULE_UNIT_OTDR);
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
static int FrmAnalysSetRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iRet = 0;
	
	//桌面上的文本
    GuiMemFree(pAnalysSetStrTitle);   
    GuiMemFree(pAnalysSetStrTitleOTDRSettings);    
  	GuiMemFree(pAnalysSetStrParaT);
	GuiMemFree(pAnalysSetStrDefValue);
	GuiMemFree(pAnalysSetStrParaL[0]);
	GuiMemFree(pAnalysSetStrParaR[0]);
	GuiMemFree(pAnalysSetStrParaL[1]);
	GuiMemFree(pAnalysSetStrParaR[1]);
	GuiMemFree(pAnalysSetStrParaL[2]);
	GuiMemFree(pAnalysSetStrParaR[2]);

    GuiMemFree(pLaunchRecvText);
    GuiMemFree(pLengthItemText);
    GuiMemFree(pEventItemText);
    GuiMemFree(pLaunchFiberLblText);
    GuiMemFree(pRecvFiberLblText);
    GuiMemFree(pLaunchFiberEditText);
    GuiMemFree(pRecvFiberEditText);
    GuiMemFree(pLaunchFiberUnitText);
    GuiMemFree(pRecvFiberUnitText);

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
static int AnalysInitPara(void)
{
	//为设置参数分配暂存空间
	if(pAnalySet == NULL)
	{
	    pAnalySet = (struct OtdrAnalysSet*)calloc(1, sizeof(struct OtdrAnalysSet));
	}
	if (NULL == pAnalySet)
	{
		CODER_LOG(CoderYun, "Malloc Failed\n");
		return -1;
	}

    if(pAnalySet->dialogCallBack != NULL)
    {
        return 0;
    }

	CODER_LOG(CoderYun, "Geting System Set...\n");
	//获取系统OTDR部分设置参数存入暂存空间，以便于设置操作
	pAnalySet->iAutoAnlys = pUser_Settings->sAnalysisSetting.iAutoAnalysisFlag;
	pAnalySet->fLossThr[0] = pUser_Settings->sAnalysisSetting.fSpliceLossThr;
	pAnalySet->fLossThr[1] = pUser_Settings->sAnalysisSetting.fReturnLossThr;
	pAnalySet->fLossThr[2] = pUser_Settings->sAnalysisSetting.fEndLossThr;

    pAnalySet->iEnableLaunchFiber = pUser_Settings->sAnalysisSetting.iEnableLaunchFiber ? 1 : 0;
    pAnalySet->fLaunchFiberLen = pUser_Settings->sAnalysisSetting.fLaunchFiberLen;
    pAnalySet->iEnableRecvFiber = pUser_Settings->sAnalysisSetting.iEnableRecvFiber ? 1 : 0;
    pAnalySet->fRecvFiberLen = pUser_Settings->sAnalysisSetting.fRecvFiberLen;
    pAnalySet->fLaunchFiberLen_ft = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft;
    pAnalySet->fLaunchFiberLen_mi = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi;
    pAnalySet->fRecvFiberLen_ft = pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft;
    pAnalySet->fRecvFiberLen_mi = pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi;
    pAnalySet->iEnableLaunchFiberEvent = pUser_Settings->sAnalysisSetting.iEnableLaunchFiberEvent;
    pAnalySet->iEnableRecvFiberEvent = pUser_Settings->sAnalysisSetting.iEnableRecvFiberEvent;
    pAnalySet->iLaunchFiberEvent = pUser_Settings->sAnalysisSetting.iLaunchFiberEvent;
    pAnalySet->iRecvFiberEvent = pUser_Settings->sAnalysisSetting.iRecvFiberEvent;
    pAnalySet->iEventOrLength = pUser_Settings->sAnalysisSetting.iEventOrLength;
    pAnalySet->dialogCallBack = NULL;

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
static int AnalysClearPara(void)
{
	//将设置好的参数回写至系统变量
	//需要获取互斥锁
	CODER_LOG(CoderYun, "Saving System Set...\n");
	
    
	pUser_Settings->sAnalysisSetting.iAutoAnalysisFlag =pAnalySet->iAutoAnlys;

    pUser_Settings->sAnalysisSetting.iEnableLaunchFiber = pAnalySet->iEnableLaunchFiber;
    pUser_Settings->sAnalysisSetting.fLaunchFiberLen = pAnalySet->fLaunchFiberLen;
    pUser_Settings->sAnalysisSetting.iEnableRecvFiber = pAnalySet->iEnableRecvFiber;
    pUser_Settings->sAnalysisSetting.fRecvFiberLen = pAnalySet->fRecvFiberLen;
    
    pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft = pAnalySet->fLaunchFiberLen_ft;
    pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi = pAnalySet->fLaunchFiberLen_mi;
    pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft = pAnalySet->fRecvFiberLen_ft;
    pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi = pAnalySet->fRecvFiberLen_mi;

    pUser_Settings->sAnalysisSetting.iEnableLaunchFiberEvent = pAnalySet->iEnableLaunchFiberEvent;
    pUser_Settings->sAnalysisSetting.iEnableRecvFiberEvent = pAnalySet->iEnableRecvFiberEvent;
    pUser_Settings->sAnalysisSetting.iLaunchFiberEvent = pAnalySet->iLaunchFiberEvent;
    pUser_Settings->sAnalysisSetting.iRecvFiberEvent = pAnalySet->iRecvFiberEvent;
    pUser_Settings->sAnalysisSetting.iEventOrLength = pAnalySet->iEventOrLength;
    	
	return 0;
}

/*重绘窗体函数*/
static void ReCreateAnalysSetWindow(GUIWINDOW **pWnd)
{	
   *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmAnalysSetInit, FrmAnalysSetExit, 
                        FrmAnalysSetPaint, FrmAnalysSetLoop, 
				        FrmAnalysSetPause, FrmAnalysSetResume,
                        NULL);
}

static void DialogApplyToCurrBackFunc(void)
{
	ShowDialogExtraInfo(TransString("ANALYZE_REEVENT_ANALYZE"));
	ReeventAnalyze();
}

//熔接损耗输入回调
static void InputSplicLossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < 0.010f || fTmp > 5.0f)
	{
		CreateIMEDialog(TransString("IME_SPLICLOSS_LIMIT"));
		return;
	}
	
	if(pOtdrTopSettings->pDisplayInfo->iCurveNum != 0)
	{
        if(fabsf(fTmp - pUser_Settings->sAnalysisSetting.fSpliceLossThr) < 0.001f)
        {
            return;
        }
        
        CODER_LOG(CoderYun, "IME: %f\n", fTmp);

        pAnalySet->dialogCallBack = DialogApplyToCurrBackFunc;
    }
    pUser_Settings->sAnalysisSetting.fSpliceLossThr = fTmp;
}

//回波损耗输入回调
static void InputRetLossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);
	//-78.0 ~ -14.0
	//此处需要检查参数，暂时省略
	if (fTmp < -78.0f || fTmp > -14.0f)
	{
		CreateIMEDialog(TransString("IME_RETLOSS_LIMIT"));
		return;
	}
	
	if(pOtdrTopSettings->pDisplayInfo->iCurveNum != 0)
	{
        if(fabsf(fTmp - pUser_Settings->sAnalysisSetting.fReturnLossThr) < 0.001f)
        {
            return;
        }
        
    	CODER_LOG(CoderYun, "IME: %f\n", fTmp);

        pAnalySet->dialogCallBack = DialogApplyToCurrBackFunc;
    }
    pUser_Settings->sAnalysisSetting.fReturnLossThr = fTmp;
}

//光纤端部损耗输入回调
static void InputEndlossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	//此处需要检查参数，暂时省略
	//1 ~ 25
	if (fTmp < 1.0f || fTmp > 25.0f)
	{
		CreateIMEDialog(TransString("IME_ENDLOSS_LIMIT"));
		return;
	}
	
	if(pOtdrTopSettings->pDisplayInfo->iCurveNum != 0)
	{
        if(fabsf(fTmp - pUser_Settings->sAnalysisSetting.fEndLossThr) < 0.001f)
        {
            return;
        }
    	CODER_LOG(CoderYun, "IME: %f\n", fTmp);
    	
        pAnalySet->dialogCallBack = DialogApplyToCurrBackFunc;
    }
    pUser_Settings->sAnalysisSetting.fEndLossThr = fTmp;
}

//检查值是否合法并进行单位转换
static void AnalyzeConvertLaunchRecvLen(double in, double* out, double* out_ft, double* out_mi)
{
    char* threshold[UNIT_COUNT] = {"360000.0000", "360.0", "1181102.4",
                                        "1181.1024", "223.6936", "393700.8"};

    double min = 0.0f;
    double max = String2Float(threshold[GetCurrSystemUnit(MODULE_UNIT_OTDR)]);

    if(in < min || in > max)
    {
        switch(GetCurrSystemUnit(MODULE_UNIT_OTDR))
        {
            case UNIT_M:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_M_LIMIT"));
                return;
            case UNIT_KM:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_KM_LIMIT"));
                return;
            case UNIT_FT:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_FT_LIMIT"));
                return;
            case UNIT_KFT:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_KFT_LIMIT"));
                return;
            case UNIT_YD:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_YD_LIMIT"));
                return;
            case UNIT_MI:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_MI_LIMIT"));
                return;
            default:
            break;
        }
    }
    
    *out = UnitConverter_Dist_System2M_Float2Float(MODULE_UNIT_OTDR, in);
    *out_ft = UnitConverter_Dist_System2FT_Float2Float(MODULE_UNIT_OTDR, in);
    *out_mi = UnitConverter_Dist_System2MI_Float2Float(MODULE_UNIT_OTDR, in);
}

//注入光纤和接收光纤长度设置
static void InputLaunchLenCallBack(void)
{
    char inputBuffer[100];
    GetIMEInputBuff(inputBuffer);
    
    if(pUser_Settings->sAnalysisSetting.iEventOrLength == 0)
    {
        double out = pUser_Settings->sAnalysisSetting.fLaunchFiberLen;
        double out_ft = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft;
        double out_mi = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi;

        double launchLen = String2Float(inputBuffer);
        AnalyzeConvertLaunchRecvLen(launchLen, &out, &out_ft, &out_mi);
        pUser_Settings->sAnalysisSetting.fLaunchFiberLen = out;
        pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft = out_ft;
        pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi = out_mi;
    }
    else
    {
        int launchEvent = String2Int(inputBuffer);
        if (launchEvent < 1 || launchEvent > 10)
        {
            CreateIMEDialog(TransString("ANALYZE_OUT_OF_RANGE"));
            return;
        }
        else
        {
            pUser_Settings->sAnalysisSetting.iLaunchFiberEvent = launchEvent;
        }
    }
}

static void InputRecvLenCallBack(void)
{
	char inputBuffer[100];
    GetIMEInputBuff(inputBuffer);
    if(pUser_Settings->sAnalysisSetting.iEventOrLength == 0)
    {
    	double out = pUser_Settings->sAnalysisSetting.fRecvFiberLen;
        double out_ft = pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft;
        double out_mi = pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi;
    	
        double recvLen = String2Float(inputBuffer);
        AnalyzeConvertLaunchRecvLen(recvLen, &out, &out_ft, &out_mi);
        pUser_Settings->sAnalysisSetting.fRecvFiberLen = out;
        pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft = out_ft;
        pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi = out_mi;
    }
    else
    {
        int RecvEvent = String2Int(inputBuffer);
        if (RecvEvent < 1 || RecvEvent > 10)
        {
            CreateIMEDialog(TransString("ANALYZE_OUT_OF_RANGE"));
            return;
        }
        else
        {
            pUser_Settings->sAnalysisSetting.iRecvFiberEvent = RecvEvent;
        }
    }
}

/*刷新分析参数输入框的内容*/
static int AnalysFulshParaDisplay(UINT8 iIndex)
{	
	if (iIndex > 2) 
	{
		return -1;
	}

	//重新转换字符串
	GuiMemFree(pAnalysSetStrParaR[iIndex]);
	pAnalysSetStrParaR[iIndex] = Float2GuiString(precision[iIndex], pAnalySet->fLossThr[iIndex]);

	//设置标签文本并显示
	SetLabelText(pAnalysSetStrParaR[iIndex], pAnalysSetLblParaR[iIndex]);
	DisplayPicture(pAnalysSetInputPara[iIndex]);
	DisplayLabel(pAnalysSetLblParaR[iIndex]);
	
	return 0;
}


/*默认值按钮点击处理函数*/
static int AnalysSetDefvalue_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen)
{
	TouchChange("btn_default_press.bmp", pAnalysSetBtnDef, 
				NULL, pAnalysSetLblDefValue, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}

static int AnalysSetDefvalue_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{	
	TouchChange("btn_default_unpress.bmp", pAnalysSetBtnDef, 
				NULL, pAnalysSetLblDefValue, 1);

	RefreshScreen(__FILE__, __func__, __LINE__);

	if(pOtdrTopSettings->pDisplayInfo->iCurveNum != 0)
	{
        if(fabsf(0.020f - pUser_Settings->sAnalysisSetting.fSpliceLossThr) < 0.001f
            && fabsf(-72.00f - pUser_Settings->sAnalysisSetting.fReturnLossThr) < 0.001f
            && fabsf(5.00f - pUser_Settings->sAnalysisSetting.fEndLossThr) < 0.001f)
        {
            return 0;
        }
        
        DialogInit(120, 90, TransString("Warning"),
                            TransString("ANALYZE_ENSURE_CHANGE"),
                            0, 1, ReCreateAnalysSetWindow, DialogApplyToCurrBackFunc, NULL);
   }
   else
   {
        pAnalySet->fLossThr[0] = 0.020f;
        pAnalySet->fLossThr[1] = -72.00f;
        pAnalySet->fLossThr[2] = 5.00f;
        int i = 0;
        //刷新显示
        for(i = 0; i < 3; ++i)
        {
            AnalysFulshParaDisplay(i);
        }
        RefreshScreen(__FILE__, __func__, __LINE__);
   }
   pUser_Settings->sAnalysisSetting.fSpliceLossThr = 0.020f;
   pUser_Settings->sAnalysisSetting.fReturnLossThr = -72.0f;
   pUser_Settings->sAnalysisSetting.fEndLossThr = 5.000f;
	return 0;
}

/*分析参数点击处理函数*/
static int AnalysSetInputPara_Down(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
	int iTmp = (int)pOutArg;
	if (--iTmp > 2)
	{
		return -1;
	}
	
	TouchChange("input_loss_unpress.bmp", pAnalysSetInputPara[iTmp], 
				NULL, pAnalysSetLblParaR[iTmp], 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}
static int AnalysSetInputPara_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
	char *buf = NULL;
	int iTmp = (int)pOutArg;
	if (--iTmp > 2)
	{
		return -1;
	}

	TouchChange("input_loss_unpress.bmp", pAnalysSetInputPara[iTmp],
				 NULL, pAnalysSetLblParaR[iTmp], 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	buf = Float2String(precision[iTmp], pAnalySet->fLossThr[iTmp]);
	switch (iTmp)
	{
	case 0://输入熔接损耗
		IMEInit(buf, 10, 1, ReCreateAnalysSetWindow, InputSplicLossBackFunc, NULL);
		break;
	case 1://输入反射阈值
		IMEInit(buf, 10, 1, ReCreateAnalysSetWindow, InputRetLossBackFunc, NULL);
		break;
	case 2://输入光纤端部检测阈值
		IMEInit(buf, 10, 1, ReCreateAnalysSetWindow, InputEndlossBackFunc, NULL);
		break;
	default:
		break;
	}

	free(buf);
	buf = NULL;
	
	return 0;
}

static void RefreshLaunchAndRecv(void)
{
    GUIPEN *pPen;
    unsigned int uiColorBack;
    pPen = GetCurrPen();
    uiColorBack = pPen->uiPenColor;
    pPen->uiPenColor = 0xE5E5E5; 
    
    int temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableLaunchFiber
                                              : pAnalySet->iEnableLaunchFiberEvent;
    SetPictureBitmap(AnalyzeCheckBoxBmp[temp], pLaunchFiberCheckBox);
    DisplayPicture(pLaunchFiberCheckBox);
    
    temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableRecvFiber
                                          : pAnalySet->iEnableRecvFiberEvent;
    SetPictureBitmap(AnalyzeCheckBoxBmp[temp], pRecvFiberCheckBox);
    DisplayPicture(pRecvFiberCheckBox);

    if(pAnalySet->iEventOrLength == 0)
    {
        GuiMemFree(pLaunchFiberLblText);
        GuiMemFree(pRecvFiberLblText);
        GuiMemFree(pLaunchFiberEditText);
        GuiMemFree(pRecvFiberEditText);
        
        pLaunchFiberLblText = TransString("ANALYZE_INJECT_LEN");
        pRecvFiberLblText = TransString("ANALYZE_RECV_LEN");
        SetLabelText(pLaunchFiberLblText, pLaunchFiberLblLabel);
        SetLabelText(pRecvFiberLblText, pRecvFiberLblLabel);

        pLaunchFiberEditText = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR, 
            pAnalySet->fLaunchFiberLen, pAnalySet->fLaunchFiberLen_ft, pAnalySet->fLaunchFiberLen_mi, 0);
        pRecvFiberEditText = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR, 
            pAnalySet->fRecvFiberLen, pAnalySet->fRecvFiberLen_ft, pAnalySet->fRecvFiberLen_mi, 0);
        SetLabelText(pLaunchFiberEditText, pLaunchFiberEditLabel);
        SetLabelText(pRecvFiberEditText, pRecvFiberEditLabel);
        
        GUIFONT* theFont = pAnalySet->iEnableLaunchFiber ? getGlobalFnt(EN_FONT_WHITE) : getGlobalFnt(EN_FONT_GRAY);
        SetLabelFont(theFont, pLaunchFiberEditLabel);
        theFont = pAnalySet->iEnableRecvFiber ? getGlobalFnt(EN_FONT_WHITE) : getGlobalFnt(EN_FONT_GRAY);
        SetLabelFont(theFont, pRecvFiberEditLabel);

        DrawBlock(160, 366, 160+240, 366+24);
        DrawBlock(160, 411, 160+240, 411+24);

        DisplayLabel(pLaunchFiberLblLabel);
        DisplayLabel(pRecvFiberLblLabel);
        DisplayPicture(pLaunchFiberCheckBox);
        DisplayPicture(pRecvFiberCheckBox);
        DisplayPicture(pLaunchFiberEdit);
        DisplayPicture(pRecvFiberEdit);
        DisplayLabel(pLaunchFiberEditLabel);
        DisplayLabel(pRecvFiberEditLabel);
        DisplayLabel(pLaunchFiberUnitLabel);
        DisplayLabel(pRecvFiberUnitLabel);
    }
    else
    {
        GuiMemFree(pLaunchFiberLblText);
        GuiMemFree(pRecvFiberLblText);
        GuiMemFree(pLaunchFiberEditText);
        GuiMemFree(pRecvFiberEditText);
        
        pLaunchFiberLblText = TransString("ANALYZE_LAUNCH_FIBER_EVENT");
        pRecvFiberLblText = TransString("ANALYZE_RECV_FIBER_EVENT");
        SetLabelText(pLaunchFiberLblText, pLaunchFiberLblLabel);
        SetLabelText(pRecvFiberLblText, pRecvFiberLblLabel);

        pLaunchFiberEditText = Int2GuiString(pAnalySet->iLaunchFiberEvent);
        pRecvFiberEditText = Int2GuiString(pAnalySet->iRecvFiberEvent);
        SetLabelText(pLaunchFiberEditText, pLaunchFiberEditLabel);
        SetLabelText(pRecvFiberEditText, pRecvFiberEditLabel);
        
        GUIFONT* theFont = pAnalySet->iEnableLaunchFiberEvent ? getGlobalFnt(EN_FONT_WHITE): getGlobalFnt(EN_FONT_GRAY);
        SetLabelFont(theFont, pLaunchFiberEditLabel);
        theFont = pAnalySet->iEnableRecvFiberEvent ? getGlobalFnt(EN_FONT_WHITE): getGlobalFnt(EN_FONT_GRAY);
        SetLabelFont(theFont, pRecvFiberEditLabel);
        
        DrawBlock(160, 366, 160+240, 366+24);
        DrawBlock(160, 411, 160+240, 411+24);
        DrawBlock(550, 366, 550+20, 366+24);
        DrawBlock(550, 411, 550+20, 411+24);

        DisplayLabel(pLaunchFiberLblLabel);
        DisplayLabel(pRecvFiberLblLabel);
        DisplayPicture(pLaunchFiberCheckBox);
        DisplayPicture(pRecvFiberCheckBox);
        DisplayPicture(pLaunchFiberEdit);
        DisplayPicture(pRecvFiberEdit);
        DisplayLabel(pLaunchFiberEditLabel);
        DisplayLabel(pRecvFiberEditLabel);
    }

    pPen->uiPenColor = uiColorBack;
}

static int AnalysLengthItemCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pLengthItemLblLabel);
    DisplayLabel(pLengthItemLblLabel);
    SyncCurrFbmap();
    return 0;
}
static int AnalysLengthItemCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLengthItemLblLabel);
    DisplayLabel(pLengthItemLblLabel);
    
    pAnalySet->iEventOrLength = 0;
    SetPictureBitmap(AnalyzeCheckBoxBmp[pAnalySet->iEventOrLength == 0], pLengthItemCheckBox);
    DisplayPicture(pLengthItemCheckBox);
    SetPictureBitmap(AnalyzeCheckBoxBmp[pAnalySet->iEventOrLength != 0], pEventItemCheckBox);
    DisplayPicture(pEventItemCheckBox);
    
    RefreshLaunchAndRecv();
    SyncCurrFbmap();
    return 0;
}

static int AnalysEventItemCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pEventItemLblLabel);
    DisplayLabel(pEventItemLblLabel);
    SyncCurrFbmap();
    return 0;
}
static int AnalysEventItemCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEventItemLblLabel);
    DisplayLabel(pEventItemLblLabel);
    
    pAnalySet->iEventOrLength = 1;
    SetPictureBitmap(AnalyzeCheckBoxBmp[pAnalySet->iEventOrLength == 0], pLengthItemCheckBox);
    DisplayPicture(pLengthItemCheckBox);
    SetPictureBitmap(AnalyzeCheckBoxBmp[pAnalySet->iEventOrLength != 0], pEventItemCheckBox);
    DisplayPicture(pEventItemCheckBox);
    
    RefreshLaunchAndRecv();
    SyncCurrFbmap();
    return 0;
}

static int AnalysLaunchCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pLaunchFiberLblLabel);
    DisplayLabel(pLaunchFiberLblLabel);
    SyncCurrFbmap();
    return 0;
}
static int AnalysLaunchCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLaunchFiberLblLabel);
    DisplayLabel(pLaunchFiberLblLabel);

    int temp;
    if(pAnalySet->iEventOrLength == 0)
    {
        pAnalySet->iEnableLaunchFiber = pAnalySet->iEnableLaunchFiber ? 0 : 1;
        temp = pAnalySet->iEnableLaunchFiber;
    }
    else
    {
        pAnalySet->iEnableLaunchFiberEvent = pAnalySet->iEnableLaunchFiberEvent ? 0 : 1;
        temp = pAnalySet->iEnableLaunchFiberEvent;
    }
    
    SetPictureBitmap(AnalyzeCheckBoxBmp[temp], pLaunchFiberCheckBox);
    DisplayPicture(pLaunchFiberCheckBox);
    
    GUIFONT* theFont = temp ? getGlobalFnt(EN_FONT_WHITE): getGlobalFnt(EN_FONT_GRAY);
    SetLabelFont(theFont, pLaunchFiberEditLabel);
    DisplayLabel(pLaunchFiberEditLabel);
    
    SyncCurrFbmap();
    return 0;
}

static int AnalysRecvCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pRecvFiberLblLabel);
    DisplayLabel(pRecvFiberLblLabel);
    SyncCurrFbmap();
    return 0;
}
static int AnalysRecvCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pRecvFiberLblLabel);
    DisplayLabel(pRecvFiberLblLabel);
    int temp;
    if(pAnalySet->iEventOrLength == 0)
    {
        pAnalySet->iEnableRecvFiber = pAnalySet->iEnableRecvFiber ? 0 : 1;
        temp = pAnalySet->iEnableRecvFiber;
    }
    else
    {
        pAnalySet->iEnableRecvFiberEvent = pAnalySet->iEnableRecvFiberEvent ? 0 : 1;
        temp = pAnalySet->iEnableRecvFiberEvent;
    }
    
    SetPictureBitmap(AnalyzeCheckBoxBmp[temp], pRecvFiberCheckBox);
    DisplayPicture(pRecvFiberCheckBox);
    
    GUIFONT* theFont = temp ? getGlobalFnt(EN_FONT_WHITE): getGlobalFnt(EN_FONT_GRAY);
    SetLabelFont(theFont, pRecvFiberEditLabel);
    DisplayLabel(pRecvFiberEditLabel);

    SyncCurrFbmap();
    return 0;
}

static int AnalysLaunchEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    return 0;
}

static int AnalysLaunchEdit_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    int temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableLaunchFiber : pAnalySet->iEnableLaunchFiberEvent;

    if (!temp)
        return 0;
    
    char *buf = NULL; 
    if(pAnalySet->iEventOrLength == 0)
    {
        buf = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_OTDR, 
            pAnalySet->fLaunchFiberLen, pAnalySet->fLaunchFiberLen_ft, pAnalySet->fLaunchFiberLen_mi, 0);
    }
    else
    {
        buf = Int2String(pAnalySet->iLaunchFiberEvent);
    }
	IMEInit(buf, 10, 1, ReCreateAnalysSetWindow, InputLaunchLenCallBack, NULL);
    free(buf);
    buf = NULL;
    
    return 0;
}

static int AnalysRecvEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    return 0;
}

static int AnalysRecvEdit_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    int temp = pAnalySet->iEventOrLength == 0 ? pAnalySet->iEnableRecvFiber : pAnalySet->iEnableRecvFiberEvent;

    if (!temp)
        return 0;
        
    char *buf = NULL; 
    if(pAnalySet->iEventOrLength == 0)
    {
        buf = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_OTDR, 
            pAnalySet->fRecvFiberLen, pAnalySet->fRecvFiberLen_ft, pAnalySet->fRecvFiberLen_mi, 0);
    }
    else
    {
        buf = Int2String(pAnalySet->iRecvFiberEvent);
    }
    
    IMEInit(buf, 10, 1, ReCreateAnalysSetWindow, InputRecvLenCallBack, NULL);
    free(buf);
    buf = NULL;
    return 0;
}


/*  侧边菜单控件回调函数 */
static void AnalysisMenuCallBack(int iOption)
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
	    SendWndMsg_WindowExit(pFrmAnalysSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 1://调用采样设置窗体 
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmSampleSetInit, FrmSampleSetExit, 
		                    FrmSampleSetPaint, FrmSampleSetLoop, 
					        FrmSampleSetPause, FrmSampleSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmAnalysSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 3://事件表阈值窗口
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmOtdrEventThresholdSetInit, FrmOtdrEventThresholdSetExit, 
		                    FrmOtdrEventThresholdSetPaint, FrmOtdrEventThresholdSetLoop, 
					        FrmOtdrEventThresholdSetPause, FrmOtdrEventThresholdSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmAnalysSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case BACK_DOWN://退出OTDR设置界面 
		ps = GetCurrWndStack();
		WndPop(ps, &func, pFrmAnalysSet);			
		break;
	case HOME_DOWN:
	 	break;
		
	default:
		break;
	}
}
