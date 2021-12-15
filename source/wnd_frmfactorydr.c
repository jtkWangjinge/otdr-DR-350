/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactoryDR.c
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  v1.0.0
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#include "wnd_frmfactorydr.h"
#include "wnd_frmfactory.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmmaintancedate.h"
#include "wnd_frmauthorization.h"
/*******************************************************************************
*                   定义wnd_frmfac.c引用其他头文件
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmime.h"
#include "app_parameter.h"
#include "app_getsetparameter.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmfactorlanguage.h"
#include "app_systemsettings.h"
#include "wnd_frminputpassward.h"
#include "wnd_frmfactoryfirmware.h"

extern PFactoryConfig pFactoryConf;
GUIWINDOW* CreateFactoryWindow() ;
GUIWINDOW* CreateSelectLanguageWindow();
#define DYNAMICRANGECOUNT   2
const int iDynamicRangeSet[DYNAMICRANGECOUNT] = {35, 30};

/*******************************************************************************
*                       窗体frmfac中的窗体控件
*******************************************************************************/
static GUIWINDOW *pFrmFactorDR = NULL;

/*******************************************************************************
*                   窗体frmfac 桌面上的背景控件
*******************************************************************************/
static GUIPICTURE *pFacDRLeftBg = NULL;

static GUICHAR *pFacDRStrTitle = NULL;
static GUILABEL *pFacDRLblTitle = NULL;
static SELECTOR *pFacDRSelector = NULL;

static GUILABEL *pFacDRLbl = NULL;
static GUICHAR *pFacDRStr = NULL;

static GUICHAR *pFacDRStrRange[DYNAMICRANGECOUNT] = {NULL};

/*******************************************************************************
*               窗体frmvls右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFactorDRMenu = NULL;

/*******************************************************************************
*                   窗体frmvls内部函数声明
*******************************************************************************/
//初始化文本资源
static int FacDRTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FacDRTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static void FacDRMenuCallBack(int iOption);
static void FacDRBackFunc (int iSelected);

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
int FrmFacDRInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
    int iCurrentRDIndex = 0;
    int i = 0;
	unsigned int StrDRMenu[] = {	
        1
	};
    
	//得到当前窗体对象 
    pFrmFactorDR = (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FacDRTextRes_Init(NULL, 0, NULL, 0);

    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFacDRLeftBg = CreatePicture(0, 0, 681, 480, BmpFileDirectory"sysset_time_bg5.bmp");
    pFacDRLblTitle = CreateLabel(0, 24, 100, 24, pFacDRStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFacDRLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFacDRLblTitle);
    pFacDRLbl = CreateLabel(40, 60, 110, 24, pFacDRStr);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFacDRLbl);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFacDRLbl);

    for(i = 0; i < DYNAMICRANGECOUNT; ++i)
    {
        if(iDynamicRangeSet[i] == pFactoryConf->iDynamicRange)
        {
            iCurrentRDIndex = i;
            break;
        }
    }
    
    pFacDRSelector = CreateSelector(150, 45, DYNAMICRANGECOUNT, 
								 iCurrentRDIndex, pFacDRStrRange, 
								 FacDRBackFunc, 1);
    
    AddSelectorToComp(pFacDRSelector, pFrmFactorDR);


    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFactorDRMenu = CreateWndMenu1(factoryRank, sizeof(StrDRMenu), StrDRMenu,  0xffff,
								3, 0, 40, FacDRMenuCallBack);

    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactorDR, pFrmFactorDR);
    
    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFactorDRMenu, pFrmFactorDR);
	GUIMESSAGE *pMsg = GetCurrMessage();	
	LoginWndMenuToMsg1(pFactorDRMenu, pFrmFactorDR);
    LoginSelectorToMsg(pFacDRSelector, pMsg);
    
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
int FrmFacDRExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmFactorDR = (GUIWINDOW *) pWndObj;


    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFactorDR);


    DestroyPicture(&pFacDRLeftBg);
    DestroyLabel(&pFacDRLblTitle);
    DestroyLabel(&pFacDRLbl);
    DestroySelector(&pFacDRSelector);
	DestroyWndMenu1(&pFactorDRMenu);

    FacDRTextRes_Exit(NULL, 0, NULL, 0);

    SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
    SaveSettings(FACTORY_CONFIG);

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
int FrmFacDRPaint(void *pWndObj)
{
    int iRet = 0;
    
	DisplayPicture(pFacDRLeftBg);
    DisplayLabel(pFacDRLblTitle);
    DisplayLabel(pFacDRLbl);
    DisplaySelector(pFacDRSelector);

	DisplayWndMenu1(pFactorDRMenu);
    
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
int FrmFacDRLoop(void *pWndObj)
{
	//错误标志、返回值定义
    int iRet = 0;
	SendWndMsg_LoopDisable(pWndObj);
    return iRet;
}

int FrmFacDRPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

int FrmFacDRResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

static int FacDRTextRes_Init(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    int i = 0;
    char buff[16];
    pFacDRStrTitle = TransString("FACTORY_DYNAMIC");
    pFacDRStr = TransString("FACTORY_DYNAMIC");
    for(i = 0; i < DYNAMICRANGECOUNT; ++i)
    {
        sprintf(buff, "%ddB", iDynamicRangeSet[i]);
        pFacDRStrRange[i] = TransString(buff);
    }
    
    return 0;
}

static int FacDRTextRes_Exit(void *pInArg, int iInLen, 
                            	 void *pOutArg, int iOutLen)
{
    int i=0;
    GuiMemFree(pFacDRStrTitle);
    for(i = 0; i < DYNAMICRANGECOUNT; ++i)
    {
        GuiMemFree(pFacDRStrRange[i]);
    }
    
	return 0;
}

static void FacDRMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	switch (iOption)
	{
	case 0: 
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAuthorizationInit, FrmAuthorizationExit, 
							FrmAuthorizationPaint, FrmAuthorizationLoop, 
							FrmAuthorizationPause, FrmAuthorizationResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactorDR);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
    case 1:        
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactoryFirmwareInit, FrmFactoryFirmwareExit, 
                        FrmFactoryFirmwarePaint, FrmFactoryFirmwareLoop, 
				        FrmFactoryFirmwarePause, FrmFactoryFirmwareResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactorDR);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 2:
        pWnd = CreateFactoryWindow();
        SendWndMsg_WindowExit(pFrmFactorDR);  
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 3:
        break;
    case 4:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactorySetInit, FrmFactorySetExit, 
                        FrmFactorySetPaint, FrmFactorySetLoop, 
				        FrmFactorySetPause, FrmFactorySetResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactorDR);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 5:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMaintanceDateInit, FrmMaintanceDateExit, 
							FrmMaintanceDatePaint, FrmMaintanceDateLoop, 
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactorDR);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN:
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmStandbysetInit, FrmStandbysetExit, 
						    FrmStandbysetPaint, FrmStandbysetLoop, 
						    FrmStandbysetPause, FrmStandbysetResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFactorDR);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}

static void FacDRBackFunc (int iSelected)
{
    LOG(LOG_INFO, "select = %d \n", iSelected);
    pFactoryConf->iDynamicRange = iDynamicRangeSet[iSelected];
    LOG(LOG_INFO, "iDynamicRange = %d \n", pFactoryConf->iDynamicRange);
}

GUIWINDOW* CreateFactoryDRWindow() 
{
	GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
            					   FrmFacDRInit, FrmFacDRExit, 
            					   FrmFacDRPaint, FrmFacDRLoop, 
            					   FrmFacDRPause, FrmFacDRResume,
            					   NULL);
    return pWnd;
}

