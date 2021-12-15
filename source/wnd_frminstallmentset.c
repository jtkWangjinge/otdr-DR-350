/*******************************************************************************
* Copyright(c)2017，宇佳软件有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frminstallmentset.h
* 摘    要：  实现主窗体wnd_frminstallmentset的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  v1.0.0
* 原 作 者：  wbq
* 完成日期：  2017/9
*******************************************************************************/
#include "wnd_frmauthorization.h"
#include "wnd_frminstallmentset.h"

#include "wnd_frmmaintancedate.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmfactory.h"
#include "wnd_frmfactorydr.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmdialog.h"
#include "wnd_frmfactorlanguage.h"
#include "app_frmotdr.h"
#include "app_parameter.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "wnd_frmselector.h"
#include "app_installment.h"
#include "app_systemsettings.h"
#include "wnd_frminputpassward.h"
#include "wnd_frmfactoryfirmware.h"
#include "wnd_frmconfigureinfo.h"

#include <time.h>

//宏定义
#define MONTHNUM 12 //第一个控件元素个数
#define OTHERNUM 13 //其余控件元素个数
#define OPTIONLEN 12 

extern PSerialNum pCurSN;

/*******************************************************************************
**							声明窗体中相关控件变量							  **
*******************************************************************************/
static GUIWINDOW *pFrmFactoryHirePurchase                      = NULL;

//定义桌面背景
static GUIPICTURE *pFactoryHirePurchaseBg                      = NULL;

//选择按钮
static GUIPICTURE *pFactoryHirePurchaseBtnEnable               = NULL;
static GUIPICTURE *pFactoryHirePurchaseBtnOK                   = NULL;

//声明标题标签变量
static GUICHAR* pFactoryHirePurchaseStrTitle                   = NULL;
static GUILABEL* pFactoryHirePurchaseLblTitle                  = NULL;
static GUICHAR* pFactoryAuthorizationStrTitle                  = NULL;
static GUILABEL* pFactoryConfigureInfoLblTitle                 = NULL;
static GUICHAR* pFactoryConfigureInfoStrTitle                  = NULL;
static GUILABEL* pFactoryAuthorizationLblTitle                 = NULL;
static GUICHAR* pFactoryHirePurchaseStrOption[OPTIONLEN];
static GUILABEL *pFactoryHirePurchaseLblOption[OPTIONLEN];

//文本标签
static GUICHAR* pFactoryHirePurchaseStrEnable                  = NULL;
static GUILABEL* pFactoryHirePurchaseLblEnable                 = NULL;
static GUICHAR* pFactoryHirePurchaseStrOK                      = NULL;
static GUILABEL* pFactoryHirePurchaseLblOK                     = NULL;

/*******************************************************************************
*                   窗体frmauthorization右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFactoryHirePurchaseMenu = NULL;

/*******************************************************************************
*                   窗体frmauthorization内部函数声明
*******************************************************************************/
//初始化文本资源
static int FrmHirePurchaseTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//释放文本资源
static int FrmHirePurchaseTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//右侧菜单回调函数
static void FrmHirePurchaseMenuCallBack(int iOption);

//点击处理函数
static int FactoryAuthorizationLblTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryAuthorizationLblTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int FactoryConfigureInfoLblTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryConfigureInfoLblTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int FactoryHirePurchaseEnable_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryHirePurchaseEnable_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int FactoryHirePurchaseOK_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FactoryHirePurchaseOK_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/*******************************************************************************
**							选择器相关参数							  **
*******************************************************************************/
static int SelectEnable = 0;
static unsigned short PeriodMonth[MONTHNUM] =    {0};
static SELECTOR *pFacHireSelector[OPTIONLEN] =   {NULL};
static GUICHAR *pFacStrNum1[MONTHNUM] =          {NULL};
static GUICHAR *pFacStrNum[OTHERNUM] =           {NULL};

//列表元素
static char *pOther[OTHERNUM] ={
"0","1", "2", "3", "4", "5", "6", "7", "8","9","10","11","12"};

//回调函数
void MonthCallBack(int iSelect,int selectID);

//保存分期付款信息
static void SaveInstallment();

//提示窗口回调函数
static void HirePurchaseReCreateWindow(GUIWINDOW **pWnd);

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmauthorization的初始化函数，建立窗体控件、注册消息处理
int FrmHirePurchaseInit(void *pWndObj)
{
    int i;
    int index = 0;
    //错误标志，返回值定义 
    int iReturn = 0;
    
    unsigned int StrVlsMenu[] = {	
        1
    };

	//初始化
	memset(PeriodMonth, 0 , sizeof(unsigned short) * MONTHNUM);
    PeriodMonth[0] = 1;
    SelectEnable = 0;

    //得到当前窗体对象 
    pFrmFactoryHirePurchase = (GUIWINDOW *) pWndObj;
    
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FrmHirePurchaseTextRes_Init(NULL, 0, NULL, 0);
    
    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/

    pFactoryHirePurchaseBg = CreatePhoto("fac_purchase");

    pFactoryAuthorizationLblTitle = CreateLabel(0, 24, 100, 24, pFactoryAuthorizationStrTitle);
    pFactoryHirePurchaseLblTitle  = CreateLabel(100, 24, 100, 24, pFactoryHirePurchaseStrTitle);
    pFactoryConfigureInfoLblTitle = CreateLabel(200, 24, 100, 24, pFactoryConfigureInfoStrTitle);
    
    pFactoryHirePurchaseLblEnable = CreateLabel(150, 425, 100, 24, pFactoryHirePurchaseStrEnable);
    pFactoryHirePurchaseBtnEnable = CreatePhoto("fac_purchase_enable_unpress");
    
    pFactoryHirePurchaseLblOK= CreateLabel(412, 432, 138, 48, pFactoryHirePurchaseStrOK);
    pFactoryHirePurchaseBtnOK = CreatePhoto("fac_purchase_OK_unpress");
    
    for(i = 0;i < OPTIONLEN; i++)
    {
        if(i<6)
        {
            if(i==0)
            {
               pFacHireSelector[i] = CreateSelector1(120, 60+60*i, 12, index, pFacStrNum1, MonthCallBack, 1);
               pFactoryHirePurchaseLblOption[i] = CreateLabel(50, 75+60*i, 100, 24, pFactoryHirePurchaseStrOption[i]);
            }
            else
            {
               pFacHireSelector[i] = CreateSelector1(120, 60+60*i, 13, index, pFacStrNum, MonthCallBack, 1);
               pFactoryHirePurchaseLblOption[i] = CreateLabel(50, 75+60*i, 100, 24, pFactoryHirePurchaseStrOption[i]);  
            }
        }
        else
        {
           pFacHireSelector[i] = CreateSelector1(412, 60+60*(i-6), 13, index, pFacStrNum, MonthCallBack, 1);
           pFactoryHirePurchaseLblOption[i] = CreateLabel(342, 75+60*(i-6), 100, 24, pFactoryHirePurchaseStrOption[i]); 
        }
        
        SetSelectorEnable(SelectEnable,pFacHireSelector[i]);
        AddSelectorToComp(pFacHireSelector[i], pFrmFactoryHirePurchase);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryHirePurchaseLblOption[i]);
    }
    
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryHirePurchaseLblOK);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryAuthorizationLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryHirePurchaseLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryHirePurchaseLblEnable);

    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryHirePurchaseLblOK);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryAuthorizationLblTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryHirePurchaseLblTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryConfigureInfoLblTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryHirePurchaseLblEnable);
    
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFactoryHirePurchaseMenu = CreateWndMenu1(factoryRank, sizeof(StrVlsMenu), StrVlsMenu,  0xffff,
								0, 0, 40, FrmHirePurchaseMenuCallBack);
    
    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //必须在持有控件队列互斥锁的情况下操作
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactoryHirePurchase, pFrmFactoryHirePurchase);
    AddWndMenuToComp1(pFactoryHirePurchaseMenu, pFrmFactoryHirePurchase);
    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryHirePurchaseBtnEnable, pFrmFactoryHirePurchase);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryHirePurchaseBtnOK, pFrmFactoryHirePurchase);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryAuthorizationLblTitle, pFrmFactoryHirePurchase);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryHirePurchaseLblTitle, pFrmFactoryHirePurchase);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryConfigureInfoLblTitle, pFrmFactoryHirePurchase);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryHirePurchaseLblEnable, pFrmFactoryHirePurchase);
    
    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	LoginWndMenuToMsg1(pFactoryHirePurchaseMenu, pFrmFactoryHirePurchase);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryAuthorizationLblTitle, 
                    FactoryAuthorizationLblTitle_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryAuthorizationLblTitle, 
                    FactoryAuthorizationLblTitle_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryConfigureInfoLblTitle, 
                    FactoryConfigureInfoLblTitle_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryConfigureInfoLblTitle, 
                    FactoryConfigureInfoLblTitle_Up, NULL, 0, pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryHirePurchaseBtnEnable, 
                    FactoryHirePurchaseEnable_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryHirePurchaseBtnEnable, 
                    FactoryHirePurchaseEnable_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryHirePurchaseLblEnable, 
                    FactoryHirePurchaseEnable_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryHirePurchaseLblEnable, 
                    FactoryHirePurchaseEnable_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryHirePurchaseBtnOK, 
                    FactoryHirePurchaseOK_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryHirePurchaseBtnOK, 
                    FactoryHirePurchaseOK_Up, NULL, 0, pMsg);

    for(i = 0; i < OPTIONLEN; i++)
    {
        LoginSelectorToMsg1(pFacHireSelector[i],i,pMsg);
    }
    return iReturn;
}

//窗体frmHirePurchase的退出函数，释放所有资源
int FrmHirePurchasePaint(void *pWndObj)
{
    int i;
    //错误标志，返回值定义 
    int iReturn = 0;
    
    //得到当前窗体对象
    pFrmFactoryHirePurchase = (GUIWINDOW *) pWndObj;
    DisplayPicture(pFactoryHirePurchaseBg);

    DisplayPicture(pFactoryHirePurchaseBtnEnable);
    DisplayPicture(pFactoryHirePurchaseBtnOK);
    for(i = 0; i < OPTIONLEN; i++)
    {
        DisplaySelector(pFacHireSelector[i]);
        DisplayLabel(pFactoryHirePurchaseLblOption[i]);
    }

    //显示文本
    DisplayLabel(pFactoryAuthorizationLblTitle);
    DisplayLabel(pFactoryHirePurchaseLblTitle);
    DisplayLabel(pFactoryConfigureInfoLblTitle);
    DisplayLabel(pFactoryHirePurchaseLblEnable);
    DisplayLabel(pFactoryHirePurchaseLblOK);
    
    /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pFactoryHirePurchaseMenu);
	SetPowerEnable(1, 1);
    
	RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

//窗体frmHirePurchase的绘制函数，绘制整个窗体
int FrmHirePurchaseLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}

//窗体frmHirePurchase的循环函数，进行窗体循环
int FrmHirePurchasePause(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}

//窗体frmHirePurchase的挂起函数，进行窗体挂起前预处理
int FrmHirePurchaseResume(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}

//窗体frmHirePurchase的恢复函数，进行窗体恢复前预处理
int FrmHirePurchaseExit(void *pWndObj)
{
    int i;
    //错误标志，返回值定义 
    int iReturn = 0;
    
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmFactoryHirePurchase = (GUIWINDOW *) pWndObj;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFactoryHirePurchase);
    
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFactoryHirePurchaseBg);
    DestroyPicture(&pFactoryHirePurchaseBtnEnable);
    DestroyPicture(&pFactoryHirePurchaseBtnOK);
    for(i = 0; i < OPTIONLEN; i++)
    {
        DestroySelector(&(pFacHireSelector[i]));
    }
    
    //销毁文本
    DestroyLabel(&pFactoryAuthorizationLblTitle);
    DestroyLabel(&pFactoryHirePurchaseLblTitle);
    DestroyLabel(&pFactoryConfigureInfoLblTitle);
    DestroyLabel(&pFactoryHirePurchaseLblEnable);
    DestroyLabel(&pFactoryHirePurchaseLblOK);
    
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFactoryHirePurchaseMenu);   
    
    //释放文本资源
    FrmHirePurchaseTextRes_Exit(NULL, 0, NULL, 0);
    
    return iReturn;
}

/*******************************************************************************
*                   窗体frmauthorization内部函数声明
*******************************************************************************/
//初始化文本资源
static int FrmHirePurchaseTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int i;
   //错误标志，返回值定义 
    int iReturn = 0;

    pFactoryHirePurchaseStrTitle  = TransString("INSTALLMENT_TITLE");
    pFactoryAuthorizationStrTitle = TransString("FACTORY_AUTHORIZATION_MANAGER");
    pFactoryConfigureInfoStrTitle = TransString("FACTORY_CONFIGURE_INFO");
    
    pFactoryHirePurchaseStrEnable = TransString("INSTALLMENT_TITLE");
    pFactoryHirePurchaseStrOK     = TransString("UPDATE_OK");

    for(i = 0;i < OPTIONLEN; i++)
    {
        pFacStrNum1[i] = TransString(pOther[i+1]);
    }
    
    for(i = 0;i < OPTIONLEN+1; i++)
    {
        pFacStrNum[i] = TransString(pOther[i]);
    }
    
    pFactoryHirePurchaseStrOption[0]  = TransString("INSTALLMENT_ONE");
    pFactoryHirePurchaseStrOption[1]  = TransString("INSTALLMENT_TWO");
    pFactoryHirePurchaseStrOption[2]  = TransString("INSTALLMENT_THREE");
    pFactoryHirePurchaseStrOption[3]  = TransString("INSTALLMENT_FOUR");
    pFactoryHirePurchaseStrOption[4]  = TransString("INSTALLMENT_FIVE");
    pFactoryHirePurchaseStrOption[5]  = TransString("INSTALLMENT_SIX");
    pFactoryHirePurchaseStrOption[6]  = TransString("INSTALLMENT_SEVEN");
    pFactoryHirePurchaseStrOption[7]  = TransString("INSTALLMENT_EIGHT");
    pFactoryHirePurchaseStrOption[8]  = TransString("INSTALLMENT_NINE");
    pFactoryHirePurchaseStrOption[9]  = TransString("INSTALLMENT_TEN");
    pFactoryHirePurchaseStrOption[10] = TransString("INSTALLMENT_ELEVEN");
    pFactoryHirePurchaseStrOption[11] = TransString("INSTALLMENT_TWELVE");
    
    return iReturn; 
}

//释放文本资源
static int FrmHirePurchaseTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int i;
    //错误标志，返回值定义 
    int iReturn = 0;
    
    GuiMemFree(pFactoryHirePurchaseStrTitle);
    GuiMemFree(pFactoryAuthorizationStrTitle);
    GuiMemFree(pFactoryConfigureInfoStrTitle);
    
    GuiMemFree(pFactoryHirePurchaseStrEnable);
    GuiMemFree(pFactoryHirePurchaseStrOK);

    for(i = 0;i < OPTIONLEN; i++)
    {
       GuiMemFree(pFacStrNum1[i]); 
    }

    for(i = 0;i < OPTIONLEN+1; i++)
    {
       GuiMemFree(pFacStrNum[i]); 
    }
    
    for(i = 0;i < OPTIONLEN; i++)
    {
       GuiMemFree(pFactoryHirePurchaseStrOption[i]); 
    }
    
    return iReturn;
}
//右侧菜单回调函数
static void FrmHirePurchaseMenuCallBack(int iOption)
{
    GUIWINDOW *pWnd = NULL;
    
	switch (iOption)
	{
	case 0: 
        break;
    case 1:        
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactoryFirmwareInit, FrmFactoryFirmwareExit, 
                        FrmFactoryFirmwarePaint, FrmFactoryFirmwareLoop, 
				        FrmFactoryFirmwarePause, FrmFactoryFirmwareResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactoryHirePurchase);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 2:
        pWnd = CreateFactoryWindow();
        SendWndMsg_WindowExit(pFrmFactoryHirePurchase);  
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 3:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacDRInit,FrmFacDRExit,
					  FrmFacDRPaint,FrmFacDRLoop,
					  FrmFacDRPause,FrmFacDRResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmFactoryHirePurchase);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 4:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactorySetInit, FrmFactorySetExit, 
                        FrmFactorySetPaint, FrmFactorySetLoop, 
				        FrmFactorySetPause, FrmFactorySetResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactoryHirePurchase);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 5:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMaintanceDateInit, FrmMaintanceDateExit, 
							FrmMaintanceDatePaint, FrmMaintanceDateLoop, 
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactoryHirePurchase);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN:
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmStandbysetInit, FrmStandbysetExit, 
						    FrmStandbysetPaint, FrmStandbysetLoop, 
						    FrmStandbysetPause, FrmStandbysetResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFactoryHirePurchase);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}
//切换到授权管理界面
static int FactoryAuthorizationLblTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}
static int FactoryAuthorizationLblTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    //GUIWINDOW *pWnd = CreateFactoryAuthorizationWindow();
    //SendWndMsg_WindowExit(pFrmFactoryHirePurchase); //发送消息以便退出当前窗体
    //SendSysMsg_ThreadCreate(pWnd);      //发送消息以便调用新的窗体
    return iReturn;
}

static int FactoryConfigureInfoLblTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}

static int FactoryConfigureInfoLblTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmConfigureInfoInit, FrmConfigureInfoExit,
                                   FrmConfigureInfoPaint, FrmConfigureInfoLoop,
                                   FrmConfigureInfoPause, FrmConfigureInfoResume,
                                   NULL);          //pWnd由调度线程释放
    SendWndMsg_WindowExit(pFrmFactoryHirePurchase); //发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);      //发送消息以便调用新的窗体
    
    return iReturn;
}

//按键点击函数
static int FactoryHirePurchaseEnable_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}
static int FactoryHirePurchaseEnable_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    //int i;
    int iReturn = 0;
    
    SelectEnable = 1-SelectEnable;
    if(SelectEnable)
    {
        SetSelectorEnable(SelectEnable,pFacHireSelector[0]);
        SetSelectorEnable(SelectEnable,pFacHireSelector[1]);
        TouchChange("fac_purchase_enable_press.bmp", pFactoryHirePurchaseBtnEnable,
                NULL, NULL, 0);
    }
    else
    {
        GUIWINDOW *pWnd = CreateFactoryHirePurchaseWindow();
        SendWndMsg_WindowExit(pFrmFactoryHirePurchase);
        SendSysMsg_ThreadCreate(pWnd);
    }

    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

//确定按钮
static int FactoryHirePurchaseOK_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

	if (SelectEnable && PeriodMonth[1])
	{
	    TouchChange("fac_purchase_OK_press.bmp", pFactoryHirePurchaseBtnOK,
	                NULL, pFactoryHirePurchaseLblOK, 1);
	    
	    RefreshScreen(__FILE__, __func__, __LINE__);
	}
    return iReturn;
}

static int FactoryHirePurchaseOK_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

    if (SelectEnable && PeriodMonth[1])
    {
    	TouchChange("fac_purchase_OK_unpress.bmp", pFactoryHirePurchaseBtnOK,
                NULL, pFactoryHirePurchaseLblOK, 1);
    	RefreshScreen(__FILE__, __func__, __LINE__);
	
    	//保存分期付款信息
    	SaveInstallment();
        
    	DialogInit(120, 90, TransString("Warning"),
    	TransString("INTSTALLMENT_INSURETIME"),
    	0, 0, HirePurchaseReCreateWindow, NULL, NULL);
    }
    
    return iReturn;
}

//控件回调函数
void MonthCallBack(int iSelect,int selecterID)
{
    LOG(LOG_INFO, "iSelect = %d,selectID = %d\n", iSelect, selecterID);

    PeriodMonth[selecterID] = selecterID == 0 ? iSelect + 1 : iSelect;
    
	if (SelectEnable != 0 && selecterID > 0 && selecterID < 11)
	{
		int flag = iSelect > 0 ? 1 : 0;
        
		if (flag)
		{
			SetSelectorEnable(flag,pFacHireSelector[selecterID + 1]);
		}
		else
		{
			int index = 0;
			for (index = selecterID + 1; index < MONTHNUM; ++index)
			{
				SetSelectorSelected(0,pFacHireSelector[index]);
				SetSelectorEnable(0,pFacHireSelector[index]);
				PeriodMonth[index] = 0;
			}
		}
	}
}

//窗体构造函数
GUIWINDOW* CreateFactoryHirePurchaseWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmHirePurchaseInit, FrmHirePurchaseExit,
                                   FrmHirePurchasePaint, FrmHirePurchaseLoop,
                                   FrmHirePurchasePause, FrmHirePurchaseResume,
                                   NULL);          //pWnd由调度线程释放
                                   
    return pWnd;
}

//提示窗口回调函数
static void HirePurchaseReCreateWindow(GUIWINDOW **pWnd)
{
     *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmHirePurchaseInit, FrmHirePurchaseExit,
                                   FrmHirePurchasePaint, FrmHirePurchaseLoop,
                                   FrmHirePurchasePause, FrmHirePurchaseResume,
                               NULL);          //pWnd由调度线程释放
}

static void SaveInstallment()
{
	//赋值installment
	INSTALLMENT inst;
	InstMemset(&inst);
	//表明还没输入第一期的解密秘钥
	InstSetCurrentPeriod(&inst, 0);
	InstSetEncryptedFlag(&inst, Encrypted);

	char SN[20] = {0};
	GenerateSerialNumber(SN, pCurSN);
	InstSetSerialNum(&inst, SN);
	
	int iCount = 0;
	int index = 0;
	for (index = 0; index < MONTHNUM; ++index)
	{
		if (PeriodMonth[index] != 0)
		{
			++iCount;
			InstSetPeriodMonth(&inst, index+1, PeriodMonth[index]);
		}
	}
	//就一个期那还叫啥分期
    if (1 == iCount)
		return;
	
	InstSetTotalPeriod(&inst, iCount);

	time_t timep;
    time(&timep);
	InstSetUtcTime(&inst,0,timep);

	//存eeprom
	SetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET);
	SaveSettings(INSTALLMENT_SET);
	InstPrint(&inst);
}

