/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlanguageset.c
* 摘    要：  实现主窗体frmlanguageset  的窗体处理线程及相
关操作函数
*
* 当前版本：  v2.0.0
* 作		 者：  
* 完成日期：  2015-5-29
*
* 取代版本：  v1.0.1
* 原   作   者：  
* 完成日期：	2014-12-20
**************************************************************/

#include "wnd_frmfactorlanguage.h"

/**************************************************************
* 	为实现窗体frmlanguageset而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmstandbyset.h"
#include "app_parameter.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmabout.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmwifi.h"
#include "wnd_frmfactory.h"
#include "wnd_frmfactorydr.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmmaintancedate.h"
#include "wnd_frmauthorization.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "app_systemsettings.h"
#include "wnd_frminputpassward.h"
#include "wnd_frmlanguageconfig.h"

GUIWINDOW* CreateFactoryWindow();
GUIWINDOW* CreateFactoryDRWindow();
/**************************************************************
* 					wnd_frmlanguageset.c中宏定义					    *
**************************************************************/
#define LANGUAGEWINX 28//初始语言横坐标
#define LANGUAGEWINY 52//初始语言纵坐标

#define MAXLANGNUM	34 //最大语言数量

/**************************************************************
* 					wnd_frmlanguageset.c中变量定义				           *	
**************************************************************/
//语言序号索引定义
static int GiIndex[MAXLANGNUM] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
};

extern PFactoryConfig  pFactoryConf;

static int GiLangSelect = 0;
unsigned long long int languageMessager = 0;

/**************************************************************
* 					窗体frmlanguageset中的窗体控件						*
**************************************************************/

static GUIWINDOW *pFrmLangSet = NULL;

/**************************************************************
* 						窗体frmlanguageset中的标签						  *
**************************************************************/

static GUICHAR *pFacTabLangguagesetStrTitle = NULL;
static GUILABEL *pFacTabLangguagesetLblTitle = NULL;
static WNDMENU1 *pLangMenu = NULL;

//状态栏、桌面、信息栏控件
static GUIPICTURE *pLangSetBGL = NULL;
//各国语言选择按钮
static GUIPICTURE *pLanguageSetBtn[MAXLANGNUM];    


/**************************************************************
* 						窗体frmlanguageset中的函数声明
**************************************************************/

//初始化文本资源
static int LangSetTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

//释放文本资源
static int LangSetTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//更新系统语言
static void Display(void);

//语言选择按钮函数声明
static int LanguageSetBtn_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int LanguageSetBtn_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static void FacMenuCallBack(int selected);

extern int isBitSetted(unsigned long long lw, int bit);

extern void setBit(unsigned long long* lw, int bit);

extern void clearBit(unsigned long long* lw, int bit);

/***
  * 功能：
		    语言设置按下处理函数
  * 参数：
		        无
  * 返回：
		        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageSetBtn_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
    int iTemp = iOutLen;
	LOG(LOG_INFO, "iTemp = %d\n", iTemp);
	GiLangSelect = iTemp;

    if (isBitSetted(languageMessager, GiLangSelect))
        clearBit(&languageMessager, GiLangSelect);
    else
        setBit(&languageMessager, GiLangSelect);
    
    Display();

	return iReturn;
}

/***
  * 功能：
		     	  语言设置抬起处理函数
  * 参数：
		        无
  * 返回：
		        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageSetBtn_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	return iReturn;
}

//菜单栏控件回调
static void FacMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	switch (iOption)
	{
	case 0: 
        break;
    case 1:        
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAuthorizationInit, FrmAuthorizationExit, 
							FrmAuthorizationPaint, FrmAuthorizationLoop, 
							FrmAuthorizationPause, FrmAuthorizationResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmLangSet);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);

        break;
    case 2:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacInit,FrmFacExit,
					  FrmFacPaint,FrmFacLoop,
					  FrmFacPause,FrmFacResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmLangSet);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 3:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacDRInit,FrmFacDRExit,
					  FrmFacDRPaint,FrmFacDRLoop,
					  FrmFacDRPause,FrmFacDRResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmLangSet);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 4:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactorySetInit, FrmFactorySetExit, 
                        FrmFactorySetPaint, FrmFactorySetLoop, 
				        FrmFactorySetPause, FrmFactorySetResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmLangSet);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 5:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMaintanceDateInit, FrmMaintanceDateExit, 
							FrmMaintanceDatePaint, FrmMaintanceDateLoop, 
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmLangSet);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN:
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmStandbysetInit, FrmStandbysetExit, 
						    FrmStandbysetPaint, FrmStandbysetLoop, 
						    FrmStandbysetPause, FrmStandbysetResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmLangSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}

/***
  * 功能：
        窗体frmFaultDtc的初始化函数，建立窗体控件、
        注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSelectLanguageInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	GUIMESSAGE *pMsg = NULL;

    //读取系统设置的值
    languageMessager = pFactoryConf->ulAvailableLanguage;

    unsigned int StrVlsMenu[] = {	
        1
	};
	//初始化文本资源
	//如果GUI存在多国语言，在此处获得对应语言的
	//文本资源
	//初始化文本资源必须在建立文本CreateText()或建立
	//标签CreateLabel()之前
	/**********************************************************/
	//得到当前窗体对象
	pFrmLangSet = (GUIWINDOW *) pWndObj;
	LangSetTextRes_Init(NULL, 0, NULL, 0);

	//建立桌面上的控件
    pLangSetBGL = CreatePicture(0, 0, 681, 480, BmpFileDirectory"sysset_time_bg5.bmp");
	//菜单栏控件
	pLangMenu = CreateWndMenu1(factoryRank, sizeof(StrVlsMenu), StrVlsMenu,  0xffff,
								1, 0, 40, FacMenuCallBack);

    pFacTabLangguagesetLblTitle = CreateLabel(0, 24, 100, 24, pFacTabLangguagesetStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFacTabLangguagesetLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFacTabLangguagesetLblTitle);
    
    CreateLoopPhoto1(pLanguageSetBtn,"IcnLanguage",17,0 ,34);
	//建立按钮区的标签
	//注册窗体控件，只对那些需要接收事件的控件进行
	//即如果该控件没有或者不需要响应输入事件，可以
	//无需注册
	//注册窗体控件必须在注册消息处理函数之前进行
	//*********************************************************/
	//注册窗体(因为所有的按键事件都统一由窗体进行
	//处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmLangSet, 
	              pFrmLangSet);
	AddWndMenuToComp1(pLangMenu, pFrmLangSet);
	
	for(i=0; i<MAXLANGNUM; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pLanguageSetBtn[i], 
		          		pFrmLangSet);
	}
	//注册消息处理函数，如果消息接收对象未曾注册
	//到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	//注册消息处理函数必须在持有锁的前提下进行
	//*********************************************************/
	pMsg = GetCurrMessage();
	//语言选择按钮
	for(i = 0; i < MAXLANGNUM; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLanguageSetBtn[i], 
				LanguageSetBtn_Down, &GiIndex[i], i, pMsg);	
		LoginMessageReg(GUIMESSAGE_TCH_UP, pLanguageSetBtn[i], 
				LanguageSetBtn_Up, &GiIndex[i], i, pMsg);
	}

	LoginWndMenuToMsg1(pLangMenu, pFrmLangSet);
    
	return iReturn;
}
/***
  * 功能：
        窗体frmFaultDtc的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSelectLanguageExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int i = 0;
	//临时变量定义
	GUIMESSAGE *pMsg = NULL;
	
	//得到当前窗体对象
	pFrmLangSet = (GUIWINDOW *) pWndObj;
	
	//清除注册消息
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//*********************************************************/
	ClearWindowComp(pFrmLangSet);

	//销毁窗体控件
	//*********************************************************/
	//销毁状态栏、桌面、信息栏控件
	DestroyPicture(&pLangSetBGL);
	DestroyWndMenu1(&pLangMenu);
	DestroyLabel(&pFacTabLangguagesetLblTitle);
	//语言图片销毁
	for(i = 0; i < MAXLANGNUM; i++)
	{
		DestroyPicture(&pLanguageSetBtn[i]);
	}

	//文本内容销毁
	LangSetTextRes_Exit(NULL, 0, NULL, 0);

    //保存系统设置
    pFactoryConf->ulAvailableLanguage = languageMessager;  
    
    SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
    SaveSettings(FACTORY_CONFIG);
    
    return iReturn;
}


/***
  * 功能：
        窗体frmFaultDtc的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSelectLanguagePaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//int i = 0;
	//得到当前窗体对象
	pFrmLangSet = (GUIWINDOW *) pWndObj;

	//显示状态栏、桌面、信息栏
	DisplayPicture(pLangSetBGL);
	DisplayWndMenu1(pLangMenu);
	GiLangSelect = GetCurrLanguageSet();
	//语言图片显示
	Display();
    DisplayLabel(pFacTabLangguagesetLblTitle);
	//关于按钮显示
	//显示状态栏、桌面、信息栏标签
	//按键区标签显示
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	 
	return iReturn;
}

/***
  * 功能：
        窗体frmFaultDtc的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSelectLanguageLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/***
  * 功能：
        窗体frmFaultDtc的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSelectLanguagePause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        		窗体frmFaultDtc的恢复函数，进行窗体恢复前预
        		处理
  * 参数：
        		1.void *pWndObj:    指向当前窗体对象
  * 返回：
        		成功返回零，失败返回非零值
  * 备注：
***/
int FrmSelectLanguageResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
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
static int LangSetTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

    pFacTabLangguagesetStrTitle = TransString("SYSTEMSET_LANGUAGE");
    
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
static int LangSetTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pFacTabLangguagesetStrTitle);
    
	return iReturn;
}



/***
  * 功能：
		        更新当前选中的语言
  * 参数：
		        无
  * 返回：
		        无
  * 备注：
***/
static void Display(void)
{
	char cPicturePath[100];
	int i = 0;	
	for(i=1; i<MAXLANGNUM+1; i++)
	{
		if(isBitSetted(languageMessager, (i-1)))
		{
			bzero(cPicturePath, sizeof(cPicturePath));
			sprintf(cPicturePath, "%s%d%s", BmpFileDirectory"IcnLanguage", i, "Push.bmp");
			SetPictureBitmap(cPicturePath, pLanguageSetBtn[i-1]);
			DisplayPicture(pLanguageSetBtn[i-1]);
		}
		else
		{
			bzero(cPicturePath, sizeof(cPicturePath));
			sprintf(cPicturePath, "%s%d%s", BmpFileDirectory"IcnLanguage", i, ".bmp");
			SetPictureBitmap(cPicturePath, pLanguageSetBtn[i-1]);
			DisplayPicture(pLanguageSetBtn[i-1]);
		}
	}
	RefreshScreen(__FILE__, __func__, __LINE__);
}

GUIWINDOW* CreateSelectLanguageWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmSelectLanguageInit, FrmSelectLanguageExit, 
                        FrmSelectLanguagePaint, FrmSelectLanguageLoop, 
				        FrmSelectLanguagePause, FrmSelectLanguageResume,
                        NULL);
  return pWnd;
}

