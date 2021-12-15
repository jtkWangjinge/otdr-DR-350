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
#include "wnd_frmlanguageset.h"
#include "wnd_frmlanguageconfig.h"

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
#include "guiphoto.h"
#include "wnd_frmfactorlanguage.h"
#include "app_parameter.h"
#include "app_systemsettings.h"
#include "wnd_frmlanguageconfig.h"

/**************************************************************
* 					wnd_frmlanguageset.c中宏定义					    *
**************************************************************/
#define LANGUAGEWINX 28//初始语言横坐标
#define LANGUAGEWINY 52//初始语言纵坐标

#define MAXLANGNUM	34 //最大语言数量

extern PSYSTEMSET pCurSystemSet;

/**************************************************************
* 					wnd_frmlanguageset.c中变量定义				           *	
**************************************************************/
//static int iLangSetKeyFlag = 0;
//语言序号索引定义
static int GiIndex[MAXLANGNUM] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
};

typedef struct _curr_language
{
    int count;
    int availableLanguage[MAXLANGNUM];
} AVALIALE_LANG;

static AVALIALE_LANG av_lang = {0, {0}};

int mapToAvLang(int currLang)
{
    int i;
    for (i = 0; i < av_lang.count; ++i)
    {
        if (currLang == av_lang.availableLanguage[i])
            return i;
    }
    return -1;
}

//语言标签位置数组
static int GiLangPos[MAXLANGNUM][2] = {
{LANGUAGEWINX, LANGUAGEWINY}, {LANGUAGEWINX, LANGUAGEWINY+24*1}, 
{LANGUAGEWINX, LANGUAGEWINY+24*2}, {LANGUAGEWINX, LANGUAGEWINY+24*3}, 
{LANGUAGEWINX, LANGUAGEWINY+24*4}, {LANGUAGEWINX, LANGUAGEWINY+24*5}, 
{LANGUAGEWINX, LANGUAGEWINY+24*6}, {LANGUAGEWINX, LANGUAGEWINY+24*7}, 
{LANGUAGEWINX, LANGUAGEWINY+24*8},

{LANGUAGEWINX, LANGUAGEWINY+24*9}, {LANGUAGEWINX, LANGUAGEWINY+24*10}, 
{LANGUAGEWINX, LANGUAGEWINY+24*11}, {LANGUAGEWINX, LANGUAGEWINY+24*12}, 
{LANGUAGEWINX, LANGUAGEWINY+24*13}, {LANGUAGEWINX, LANGUAGEWINY+24*14}, 
{LANGUAGEWINX, LANGUAGEWINY+24*15}, {LANGUAGEWINX, LANGUAGEWINY+24*16}, 

{LANGUAGEWINX+301, LANGUAGEWINY}, {LANGUAGEWINX+301, LANGUAGEWINY+24*1}, 
{LANGUAGEWINX+301, LANGUAGEWINY+24*2},{LANGUAGEWINX+301, LANGUAGEWINY+24*3}, 
{LANGUAGEWINX+301, LANGUAGEWINY+24*4}, {LANGUAGEWINX+301, LANGUAGEWINY+24*5},
{LANGUAGEWINX+301, LANGUAGEWINY+24*6}, {LANGUAGEWINX+301, LANGUAGEWINY+24*7}, 
{LANGUAGEWINX+301, LANGUAGEWINY+24*8},  

{LANGUAGEWINX+301, LANGUAGEWINY+24*9}, {LANGUAGEWINX+301, LANGUAGEWINY+24*10}, 
{LANGUAGEWINX+301, LANGUAGEWINY+24*11}, {LANGUAGEWINX+301, LANGUAGEWINY+24*12},
{LANGUAGEWINX+301, LANGUAGEWINY+24*13}, {LANGUAGEWINX+301, LANGUAGEWINY+24*14}, 
{LANGUAGEWINX+301, LANGUAGEWINY+24*15}, {LANGUAGEWINX+301, LANGUAGEWINY+24*16}
};

static int GiLangSelect = -1;
extern PFactoryConfig  pFactoryConf;
/**************************************************************
* 					窗体frmlanguageset中的窗体控件						*
**************************************************************/

static GUIWINDOW *pFrmLangSet = NULL;

/**************************************************************
* 					窗体frmlanguageset中的文本资源						*
**************************************************************/
static GUICHAR *pLangSetStrLanguageSet = NULL; 
static GUILABEL *pLangSetLblLanguageSet = NULL;

static GUICHAR *pLangSetStrLanguageConfig = NULL;	  		//窗体左上角标题栏文本
static GUILABEL *pLangSetLblLanguageConfig = NULL;			//窗体左上角的Label
/**************************************************************
* 						窗体frmlanguageset中的标签						  *
**************************************************************/


static WNDMENU1 *pLangMenu = NULL;

//状态栏、桌面、信息栏控件
static GUIPICTURE *pLangSetBGL = NULL;
static GUIPICTURE *pLangSetBGTableTitle = NULL;
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

                               
static int LanguageSetToLanguageConfig_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int LanguageSetToLanguageConfig_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);

static void LangSetWndMenuCallBack(int selected);

extern int isBitSetted(unsigned long long lw, int bit);

extern void setBit(unsigned long long* lw, int bit);

extern void clearBit(unsigned long long* lw, int bit);

void InitAvLanguage()
{
    int i;
    av_lang.count = 0;

    for (i = 0; i < MAXLANGNUM; ++i)
	{	        
		if (isBitSetted(pFactoryConf->ulAvailableLanguage, i))
        {
            av_lang.availableLanguage[av_lang.count++] = i;    
        }
	}
}

/***
  * 功能：
        窗体出栈时调用的函数
  * 参数：
        1.GUIWINDOW **pWnd:    指向当前窗体对象
  * 返回：
        无
  * 备注：
***/
static void ReCreateLangSetWindow(GUIWINDOW **pWnd)
{	
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmLanguageSetInit, FrmLanguageSetExit, 
                        FrmLanguageSetPaint, FrmLanguageSetLoop, 
				        FrmLanguageSetPause, FrmLanguageSetResume,
                        NULL);
}

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
	int iTemp = *((int *)pOutArg);

	GiLangSelect = iTemp;
	
	//设置当前GUI环境的语言
	GUIWINDOW *pWnd = NULL;
	SetCurrLanguage(av_lang.availableLanguage[iTemp]);
	// 保存系统设置参数
    SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);

	ReCreateLangSetWindow(&pWnd);
	SendWndMsg_WindowExit(pFrmLangSet);  
	SendSysMsg_ThreadCreate(pWnd); 

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

/***
  * 功能：
		    跳转到导入配置文件界面
  * 参数：
		        无
  * 返回：
		        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageSetToLanguageConfig_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/***
  * 功能：
		     	  跳转到导入配置文件界面
  * 参数：
		        无
  * 返回：
		        成功返回零，失败返回非零值
  * 备注：
***/
static int LanguageSetToLanguageConfig_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmLanguageConfigInit, FrmLanguageConfigExit, 
                        FrmLanguageConfigPaint, FrmLanguageConfigLoop, 
    			        FrmLanguageConfigPause, FrmLanguageConfigResume,
	                    NULL);      
	                    
	SendWndMsg_WindowExit(pFrmLangSet);  
	SendSysMsg_ThreadCreate(pWnd); 
	return iReturn;
}

//菜单栏控件回调
static void LangSetWndMenuCallBack(int selected)
{
	GUIWINDOW *pWnd = NULL;

	switch (selected)
	{
	case 0://调取待机和亮度窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmStandbysetInit, FrmStandbysetExit, 
		                    FrmStandbysetPaint, FrmStandbysetLoop, 
					        FrmStandbysetPause, FrmStandbysetResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmLangSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 1://调取时间设置
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmTimeSetInit, FrmTimeSetExit, 
		                    FrmTimeSetPaint, FrmTimeSetLoop, 
					        FrmTimeSetPause, FrmTimeSetResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmLangSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调取语言设置窗体
		//... 
		break;
	case 3: //调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmWiFiInit, FrmWiFiExit, 
								FrmWiFiPaint, FrmWiFiLoop, 
								FrmWiFiPause, FrmWiFiResume,
								NULL);			
		SendWndMsg_WindowExit(pFrmLangSet);	
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 4: //调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmSysMaintainInit, FrmSysMaintainExit, 
							FrmSysMaintainPaint, FrmSysMaintainLoop, 
							FrmSysMaintainPause, FrmSysMaintainResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmLangSet);	
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 5:
        #ifdef MINI2
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAboutInit, FrmAboutExit, 
							FrmAboutPaint, FrmAboutLoop, 
							FrmAboutPause, FrmAboutResume,
							NULL);			       //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmLangSet);	  //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			  //发送消息以便调用新的窗体
		#endif
		break;	
	case BACK_DOWN:
	case HOME_DOWN://退出时间设置
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmMainInit, FrmMainExit, 
	                        FrmMainPaint, FrmMainLoop, 
					        FrmMainPause, FrmMainResume,
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
int FrmLanguageSetInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	int i = 0;
	char cPicturePath[100];
	GUIMESSAGE *pMsg = NULL;
    #ifdef MINI2
    unsigned int StrLanguageMenu[] = {
	SYSTEMSET_STANDBY,
	SYSTEMSET_TIME,
	SYSTEMSET_LANGUAGE,
	MAIN_LBL_WIFI,
	SYSTEMSET_SYSMAINTAIN,
	SYSTEM_ABOUT
	};
    #else
	unsigned int StrLanguageMenu[] = {
		1
	};
    #endif
	//初始化文本资源
	//如果GUI存在多国语言，在此处获得对应语言的
	//文本资源
	//初始化文本资源必须在建立文本CreateText()或建立
	//标签CreateLabel()之前
	/**********************************************************/
	//得到当前窗体对象
	pFrmLangSet = (GUIWINDOW *) pWndObj;
	LangSetTextRes_Init(NULL, 0, NULL, 0);
    InitAvLanguage();

	//建立桌面上的控件
    pLangSetBGL = CreatePhoto("bg_language");
    pLangSetBGTableTitle = CreatePhoto("otdr_top1f");
	//菜单栏控件
	#ifdef MINI2
    pLangMenu = CreateWndMenu1(6, sizeof(StrLanguageMenu),StrLanguageMenu, 0xffff,
								 2, 0, 40, LangSetWndMenuCallBack);
    #else
	pLangMenu = CreateWndMenu1(5, sizeof(StrLanguageMenu),StrLanguageMenu, 0xffff,
								 2, 0, 40, LangSetWndMenuCallBack);
    #endif

   	for (i = 0; i < av_lang.count; ++i)
	{	
		    bzero(cPicturePath, sizeof(cPicturePath));
		    sprintf(cPicturePath, "%s%d%s", BmpFileDirectory"IcnLanguage", (av_lang.availableLanguage[i]+1), ".bmp");
		    pLanguageSetBtn[i] = CreatePicture(GiLangPos[i][0], GiLangPos[i][1],  312, 24, 
	    							           cPicturePath);
	}
	//建立按钮区的标签
	pLangSetLblLanguageSet = CreateLabel(0, 24, 100, 24,    
									pLangSetStrLanguageSet);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pLangSetLblLanguageSet);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pLangSetLblLanguageSet);

	pLangSetLblLanguageConfig = CreateLabel(100, 24, 100, 24,    
									pLangSetStrLanguageConfig);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pLangSetLblLanguageConfig);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pLangSetLblLanguageConfig);
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

    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pLangSetLblLanguageConfig, 
	          		pFrmLangSet);
	          		
   	for (i = 0; i < av_lang.count; ++i)
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
   	for (i = 0; i < av_lang.count; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLanguageSetBtn[i], 
    		    LanguageSetBtn_Down, &GiIndex[i], sizeof(int), pMsg);	
        LoginMessageReg(GUIMESSAGE_TCH_UP, pLanguageSetBtn[i], 
    		    LanguageSetBtn_Up, &GiIndex[i], sizeof(int), pMsg);
    }


    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLangSetLblLanguageConfig, 
    		    LanguageSetToLanguageConfig_Down, NULL, 0, pMsg);	
    LoginMessageReg(GUIMESSAGE_TCH_UP, pLangSetLblLanguageConfig, 
    		    LanguageSetToLanguageConfig_Up, NULL, 0, pMsg);

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
int FrmLanguageSetExit(void *pWndObj)
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
    DestroyPicture(&pLangSetBGTableTitle);
	DestroyWndMenu1(&pLangMenu);
	
	//语言图片销毁
   	for (i = 0; i < av_lang.count; ++i)
	{
		DestroyPicture(&pLanguageSetBtn[i]);
	}
	//销毁状态栏、桌面、信息栏标签
	DestroyLabel(&pLangSetLblLanguageSet);
	DestroyLabel(&pLangSetLblLanguageConfig);

	//文本内容销毁
	LangSetTextRes_Exit(NULL, 0, NULL, 0);
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
int FrmLanguageSetPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//int i = 0;
	//得到当前窗体对象
	pFrmLangSet = (GUIWINDOW *) pWndObj;

	//显示状态栏、桌面、信息栏
	DisplayPicture(pLangSetBGL);
    DisplayPicture(pLangSetBGTableTitle);
	DisplayWndMenu1(pLangMenu);
	//CreateTitle(pLangSetLblLanguageSet);
	GiLangSelect = mapToAvLang(GetCurrLanguageSet());
	//语言图片显示
	Display();
	
	DisplayLabel(pLangSetLblLanguageSet);
	DisplayLabel(pLangSetLblLanguageConfig);
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
int FrmLanguageSetLoop(void *pWndObj)
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
int FrmLanguageSetPause(void *pWndObj)
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
int FrmLanguageSetResume(void *pWndObj)
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

	//初始化状态栏上的文本
	//...
	//初始化桌面上的文本
	//...
	//初始化按钮区的文本
	//...
	pLangSetStrLanguageSet = TransString("SYSTEMSET_LANGUAGE"); 
	pLangSetStrLanguageConfig = TransString("FACTORY_IMPORT_PROFILE"); 
	//初始化信息栏上的文本
	//...
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

	//释放状态栏上的文本
	//...
	//释放桌面上的文本
	//...
	GuiMemFree(pLangSetStrLanguageSet); 
	GuiMemFree(pLangSetStrLanguageConfig); 
	//释放按钮区的文本
	//...
	//释放信息栏上的文本
	//...
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
   	for (i = 0; i < av_lang.count; ++i)
	{
		if(i == GiLangSelect){
			bzero(cPicturePath, sizeof(cPicturePath));
			sprintf(cPicturePath, "%s%d%s", BmpFileDirectory"IcnLanguage", (av_lang.availableLanguage[i]+1), "Push.bmp");
			SetPictureBitmap(cPicturePath, pLanguageSetBtn[i]);
			DisplayPicture(pLanguageSetBtn[i]);
		}
		else
		{
			bzero(cPicturePath, sizeof(cPicturePath));
			sprintf(cPicturePath, "%s%d%s", BmpFileDirectory"IcnLanguage", (av_lang.availableLanguage[i]+1), ".bmp");
			SetPictureBitmap(cPicturePath, pLanguageSetBtn[i]);
			DisplayPicture(pLanguageSetBtn[i]);
		}
	}
	RefreshScreen(__FILE__, __func__, __LINE__);
}

