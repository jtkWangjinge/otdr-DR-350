/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmAbout.c
* 摘    要：  声明主窗体FrmAbout 的窗体处理线程及相关
                操作函数
*
* 当前版本：  v0.0.1
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/
#include "wnd_frmabout.h"

/**************************************************************
* 	为实现窗体wnd_FrmAbout而需要引用的其他头文件
**************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmstandbyset.h"
#include "app_parameter.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmwifi.h"
#include "app_frmupdate.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "app_systemsettings.h"
#include "wnd_frminstallment.h"
#include "app_installment.h"
#include "app_systemsettings.h"

#ifdef FORC
#define OTDR_PLATFORM    "FORC | MINI2"
#define OTDR_MODULE      "FORC | MINI2"
#else
#define OTDR_PLATFORM    PROJECT_NAME
#define OTDR_MODULE      PROJECT_NAME
#endif

/**************************************************************
* 		定义wnd_FrmAbout中内部使用的数据结构
**************************************************************/

//引用的全局变量
extern PSYSTEMSET pCurSystemSet;
extern SerialNum* pCurSN;
extern PFactoryConfig pFactoryConf;     //引用保存到eeprom的变量
extern CURR_WINDOW_TYPE enCurWindow;
//自定义的全局变量

/***************************************************************
* 			窗体wnd_FrmAbout中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmAbout = NULL;

//菜单栏控件
static WNDMENU1 *pAboutMenu = NULL;

/*********************桌面背景定义************************/
static GUIPICTURE *pFrmAboutBG = NULL;
static GUIPICTURE *pFrmAboutBGTableTitle = NULL;


/**********************桌面控件定义***********************/
//桌面控件
static GUIPICTURE *pAboutBtnSWEdition = NULL ;
static GUIPICTURE *pAboutInfo = NULL;
/************************桌面标签定义*********************/
//桌面标签
static GUILABEL *pAboutLblLabel = NULL;
static GUILABEL *pSerialNumberLblLabel = NULL;
static GUILABEL *pAboutLblCorpName = NULL; 
static GUILABEL *pAboutLblRightsHave = NULL; 
static GUILABEL *pAboutLblInstallmentInfo = NULL;

//桌面标签 
static GUICHAR *pAboutStrLabel = NULL;
static GUICHAR *pSerialNumberStrLabel = NULL; 
static GUICHAR *pAboutStrCorpName = NULL; 
static GUICHAR *pAboutStrRightsHave = NULL; 
static GUICHAR *pAboutStrInstallmentInfo = NULL;
static GUICHAR *pAboutStrInstInfo2 = NULL;
static GUICHAR *pAboutStrInstInfo3 = NULL;
static GUICHAR *pAboutStrInstInfo4 = NULL;

//联系标签
static GUILABEL *pAboutLblNextCalibrationDate = NULL; 
static GUILABEL *pAboutLblLastCalibrationDate = NULL; 
static GUILABEL *pAboutLblLastCalibrationDateNum = NULL; 
static GUILABEL *pAboutLblNextCalibrationDateNum = NULL; 
static GUILABEL *pAboutLblWebsite = NULL;  
//联系标签
static GUICHAR *pAboutStrNextCalibrationDate = NULL; 
static GUICHAR *pAboutStrLastCalibrationDate = NULL; 
static GUICHAR *pAboutStrLastCalibrationDateNum = NULL; 
static GUICHAR *pAboutStrNextCalibrationDateNum = NULL; 
static GUICHAR *pAboutStrWebsite = NULL; 
//信息标签
static GUILABEL *pAboutLblInfo = NULL; 
static GUICHAR *pAboutStrInfo = NULL; 
//模块标签 
static GUICHAR *pAboutStrModule = NULL; 
static GUICHAR *pAboutStrModuleName = NULL; 
static GUICHAR *pAboutStrSerial = NULL; 
static GUICHAR *pAboutStrDeepEdition = NULL;
static GUICHAR *pAboutStrDate = NULL;
static GUICHAR *pAboutStrSWEdition = NULL;
static GUICHAR *pAboutStrModuleNum = NULL; 
static GUICHAR *pAboutStrSerialNum = NULL; 
static GUICHAR *pAboutStrDeepEditionNum = NULL; 
static GUICHAR 	*pAboutStrVersionTime = NULL;

static GUILABEL *pAboutLblModule = NULL; 
static GUILABEL *pAboutLblModuleName = NULL; 
static GUILABEL *pAboutLblSerial = NULL; 
static GUILABEL *pAboutLblDeepEdition = NULL; 
static GUILABEL *pAboutLblDate = NULL;
static GUILABEL *pAboutLblSWEdition = NULL;
static GUILABEL *pAboutLblModuleNum = NULL; 
static GUILABEL *pAboutLblSerialNum = NULL; 
static GUILABEL *pAboutLblDeepEditionNum = NULL;
static GUILABEL *pAboutLblVersionTime = NULL;
/*******************************************************************************
* 						文本资源初始化及销毁函数声明
*******************************************************************************/
//初始化文本资源
static int AboutTextRes_Init(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
//释放文本资源
static int AboutTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);

static int AboutSwitchSerialNumber_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int AboutSwitchSerialNumber_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/*******************************************************************************
* 								控件函数声明
*******************************************************************************/
static void AboutWndMenuCallBack(int selected);
static void AboutReCreateWindow(GUIWINDOW **pWnd);
//获得分期付款信息
static int GetInstInfo(char *periodBuf, char *daysBuf);

/***
  * 功能：
                窗体FrmAbout 的初始化函数，建立窗体控件、
                注册消息处理
  * 参数：
             1.void *pWndObj:    指向当前窗体对象
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
int FrmAboutInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文
    //本资源
    //初始化文本资源必须在建立文本CreateText()或建立标
    //签CreateLabel()之前
    /**********************************************************/
    #ifdef MINI2
    unsigned int pAboutMenuStr[] = {
		SYSTEMSET_STANDBY,
		SYSTEMSET_TIME,
		SYSTEMSET_LANGUAGE,
		MAIN_LBL_WIFI,
		SYSTEMSET_SYSMAINTAIN,
		SYSTEM_ABOUT
	};
    #else
	unsigned int pAboutMenuStr[] = {
        1
    };
    #endif
    
    enCurWindow = ENUM_ABOUT_WIN;

    //得到当前窗体对象
    pFrmAbout = (GUIWINDOW *) pWndObj;
    AboutTextRes_Init(NULL, 0, NULL, 0);

    //建立桌面上的控件
    pFrmAboutBG = CreatePhoto("bg_about");
    pFrmAboutBGTableTitle = CreatePhoto("otdr_top1f");
    #ifdef VIEW100
    pAboutBtnSWEdition = CreatePhoto("btn_software");
    #else
    pAboutBtnSWEdition = CreatePhoto("about_mini");
    #endif
    pAboutInfo = CreatePhoto("about_info");
    /******************建立按钮区的标签******************/
    pAboutLblLabel = CreateLabel(0, 24, 100,24, pAboutStrLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAboutLblLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pAboutLblLabel);

    pSerialNumberLblLabel = CreateLabel(100, 24, 100,24, pSerialNumberStrLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSerialNumberLblLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSerialNumberLblLabel);
    /******************建立按钮区的标签******************/ 
    pAboutLblCorpName = CreateLabel(310,60,350,24,pAboutStrCorpName);
    pAboutLblRightsHave = CreateLabel(310,90,350,24,pAboutStrRightsHave);
    pAboutLblWebsite = CreateLabel(310,120,350,24,pAboutStrWebsite);
	pAboutLblInstallmentInfo = CreateLabel(310,150,350,24,pAboutStrInstallmentInfo);

    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblRightsHave);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblCorpName);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblWebsite);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblInstallmentInfo);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblCorpName);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblRightsHave);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblWebsite);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblInstallmentInfo);

    //联系标签
    pAboutLblLastCalibrationDate = CreateLabel(34,197,315,24, pAboutStrLastCalibrationDate);
    pAboutLblLastCalibrationDateNum = CreateLabel(349,197,315,24,pAboutStrLastCalibrationDateNum);
    pAboutLblNextCalibrationDate = CreateLabel(34,230,315,24,pAboutStrNextCalibrationDate);
    pAboutLblNextCalibrationDateNum = CreateLabel(349,230,315,24, pAboutStrNextCalibrationDateNum);

    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAboutLblLastCalibrationDate);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAboutLblLastCalibrationDateNum);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAboutLblNextCalibrationDateNum); 
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAboutLblNextCalibrationDate);    

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblLastCalibrationDate);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblLastCalibrationDateNum);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblNextCalibrationDate);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblNextCalibrationDateNum);

    //信息标签
    pAboutLblInfo = CreateLabel(35,280,300,24, pAboutStrInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblInfo);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblInfo);	
    //模块标签(左侧)
    pAboutLblModule = CreateLabel(310,320,110,24,pAboutStrModule);//platform
    pAboutLblModuleName = CreateLabel(310,350,100,24, pAboutStrModuleName);//module
    pAboutLblSerial = CreateLabel(310, 380, 100,24, pAboutStrSerial);//serial
    pAboutLblDeepEdition = CreateLabel(310,410,100,24, pAboutStrDeepEdition);//version
    pAboutLblDate = CreateLabel(310, 440, 100, 24, pAboutStrDate);//date
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblModule);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblModuleName);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblSerial);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblDeepEdition);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblDate);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblModule);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblModuleName);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblSerial);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblDeepEdition);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblDate);
    //模块标签(右侧)
    pAboutLblSWEdition = CreateLabel(420,320,200,24,pAboutStrSWEdition);//platform num
    pAboutLblModuleNum = CreateLabel(420,350,200,24, pAboutStrModuleNum);//module num
    pAboutLblSerialNum = CreateLabel(420,380,200,24, pAboutStrSerialNum);//serial num
    pAboutLblDeepEditionNum = CreateLabel(420,410,200,24, pAboutStrDeepEditionNum);//version num
    pAboutLblVersionTime = CreateLabel(420,440,200,24, pAboutStrVersionTime);//date time
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblSWEdition);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblModuleNum);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblSerialNum);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblDeepEditionNum);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pAboutLblVersionTime);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblSWEdition);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblModuleNum);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblSerialNum);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblDeepEditionNum);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pAboutLblVersionTime);
    //菜单栏控件
    #ifdef MINI2
    pAboutMenu = CreateWndMenu1(6, sizeof(pAboutMenuStr), pAboutMenuStr, 0xffff, 5, 0, 40,
                                AboutWndMenuCallBack);
    #else
    pAboutMenu = CreateWndMenu1(0, sizeof(pAboutMenuStr), pAboutMenuStr, 0xffff, -1, 0, 40,
                                AboutWndMenuCallBack);
    #endif
    //注册窗体控件,只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可
    //以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //*********************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进
    //行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmAbout,pFrmAbout);
    AddWndMenuToComp1(pAboutMenu, pFrmAbout);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSerialNumberLblLabel, pFrmAbout);
    //注册消息处理函数，如果消息接收对象未曾注册
    //到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //*********************************************************/
    pMsg = GetCurrMessage();

    //使用默认曲线按钮
    LoginWndMenuToMsg1(pAboutMenu, pFrmAbout);

	if(InstIsEncrypted())
	{
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSerialNumberLblLabel, 
                        AboutSwitchSerialNumber_Down, NULL, 0, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSerialNumberLblLabel, 
                        AboutSwitchSerialNumber_Up, NULL, 0, pMsg);
    }
    
    return iReturn;
}
/***
  * 功能：
                窗体FrmAbout 的退出函数，释放所有资源
  * 参数：
                1.void *pWndObj:    指向当前窗体对象
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
int FrmAboutExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmAbout = (GUIWINDOW *) pWndObj;

    //清除注册消息
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //*********************************************************/
    ClearWindowComp(pFrmAbout);

    //销毁窗体控件
    //*********************************************************/
    //销毁状态栏、桌面、信息栏控件
    DestroyPicture(&pFrmAboutBG);
    DestroyPicture(&pFrmAboutBGTableTitle);
    DestroyPicture(&pAboutBtnSWEdition);
    DestroyPicture(&pAboutInfo);

    //桌面标签销毁
    DestroyLabel(&pAboutLblLabel);
    DestroyLabel(&pSerialNumberLblLabel);
    DestroyLabel(&pAboutLblCorpName);
    DestroyLabel(&pAboutLblRightsHave);
	DestroyLabel(&pAboutLblInstallmentInfo);
    //联系
    DestroyLabel(&pAboutLblNextCalibrationDate);
    DestroyLabel(&pAboutLblLastCalibrationDate);
    DestroyLabel(&pAboutLblLastCalibrationDateNum);
    DestroyLabel(&pAboutLblNextCalibrationDateNum);
    DestroyLabel(&pAboutLblWebsite);
    //信息标签销毁
    DestroyLabel(&pAboutLblInfo);
    DestroyLabel(&pAboutLblSWEdition);
    DestroyLabel(&pAboutLblVersionTime);

    //模块标签(左侧)销毁
    DestroyLabel(&pAboutLblModule);
    DestroyLabel(&pAboutLblModuleName);
    DestroyLabel(&pAboutLblSerial);
    DestroyLabel(&pAboutLblDeepEdition);
    DestroyLabel(&pAboutLblDate);
    //模块标签(右侧)销毁
    DestroyLabel(&pAboutLblSWEdition);
    DestroyLabel(&pAboutLblModuleNum);
    DestroyLabel(&pAboutLblSerialNum);
    DestroyLabel(&pAboutLblDeepEditionNum);
    DestroyLabel(&pAboutLblVersionTime);
    
    DestroyWndMenu1(&pAboutMenu);
    //文本内容销毁
    AboutTextRes_Exit(NULL, 0, NULL, 0);
    enCurWindow = ENUM_OTHER_WIN;

    return iReturn;
}


/***
  * 功能：
                窗体FrmAbout 的绘制函数，绘制整个窗体
  * 参数：
             1.void *pWndObj:    指向当前窗体对象
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
int FrmAboutPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmAbout = (GUIWINDOW *) pWndObj;
    //显示状态栏、桌面、信息栏控件
    DisplayPicture(pFrmAboutBG);
    DisplayPicture(pFrmAboutBGTableTitle);
    DisplayPicture(pAboutBtnSWEdition);
    //桌面标签销毁
    DisplayLabel(pAboutLblLabel);
    if(InstIsEncrypted())
	{
        DisplayLabel(pSerialNumberLblLabel);
    }
    DisplayLabel(pAboutLblCorpName);
    DisplayLabel(pAboutLblRightsHave);
	DisplayLabel(pAboutLblInstallmentInfo);
    //联系
    DisplayLabel(pAboutLblNextCalibrationDate);
    DisplayLabel(pAboutLblNextCalibrationDateNum);
    DisplayLabel(pAboutLblWebsite);
    DisplayLabel(pAboutLblLastCalibrationDate);
    DisplayLabel(pAboutLblLastCalibrationDateNum);

    //信息标签显示
    DisplayLabel(pAboutLblInfo);
    DisplayLabel(pAboutLblSWEdition);
    DisplayLabel(pAboutLblVersionTime);
    //模块标签(左侧)显示
    DisplayLabel(pAboutLblModule);
    DisplayLabel(pAboutLblModuleName);
    DisplayLabel(pAboutLblSerial);
    DisplayLabel(pAboutLblDeepEdition);
    DisplayLabel(pAboutLblDate);
    //模块标签(右侧)显示
    DisplayLabel(pAboutLblSWEdition);
    DisplayLabel(pAboutLblModuleNum);
    DisplayLabel(pAboutLblSerialNum);
    DisplayLabel(pAboutLblDeepEditionNum);
    DisplayLabel(pAboutLblVersionTime);
    
    DisplayWndMenu1(pAboutMenu);
    SetPowerEnable(1, 1);

    #ifndef VIEW100
    if(pCurSystemSet->uiCountryStyle == LANG_KOREAN)
    {
        setAboutStyle(1);
    }
    else
    {
        setAboutStyle(0);
    }
    #endif
    RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
            窗体FrmAbout 的循环函数，进行窗体循环
  * 参数：
         1.void *pWndObj:    指向当前窗体对象
  * 返回：
            成功返回零，失败返回非零值
  * 备注：
***/
int FrmAboutLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;


    return iReturn;
}


/***
  * 功能：
        窗体FrmAbout 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmAboutPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
            窗体FrmAbout 的恢复函数，进行窗体恢复前预处理
  * 参数：
            1.void *pWndObj:    指向当前窗体对象
  * 返回：
            成功返回零，失败返回非零值
  * 备注：
***/
int FrmAboutResume(void *pWndObj)
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
static int AboutTextRes_Init(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //显示版本时间
    char s_month[5];
	int month, day, year;
	char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
	month = (strstr(month_names, s_month) - month_names) / 3;    
    char arrFwDate[128] = {0};
    sprintf(arrFwDate, "%d-%d-%d %s", year, month + 1, day, __TIME__ );
    pAboutStrVersionTime = TransString(arrFwDate);
    VERSION pPar;
    memset(&pPar, 0, sizeof(VERSION));
    ReadUpdateVersion("/app/version.bin", &pPar);
    char vBuf[12] = {0};
    #ifdef VersionB
    sprintf(vBuf,"B%s",pPar.Userfs);
    #else
    sprintf(vBuf,"V%s",pPar.Userfs);
    #endif
	pAboutStrLabel = TransString("System Info");
	pSerialNumberStrLabel = TransString("Ultimate Key");
	pAboutStrCorpName = TransString("Copyright (C) 2015-2020");
	pAboutStrRightsHave = TransString("All Rights Reserved");
    #ifdef FORC
    pAboutStrWebsite = TransString("http://www.fiberopticresale.com");
    #else
    pAboutStrWebsite = TransString("http://www.innoinstrument.com/");
    #endif
	//获得申请分期付款信息
	char periodBuf[16] = {0};
	char daysBuf[16] = {0};
	int isEncrypted = GetInstInfo(periodBuf, daysBuf);
	if(isEncrypted)
	{
		pAboutStrInstallmentInfo = TransString("N-Count:");
    	pAboutStrInstInfo2 = TransString(periodBuf);
		pAboutStrInstInfo3 = TransString("Days Remaining:");
    	pAboutStrInstInfo4 = TransString(daysBuf);
		StringCat(&pAboutStrInstallmentInfo, pAboutStrInstInfo2);
		StringCat(&pAboutStrInstallmentInfo, pAboutStrInstInfo3);
		StringCat(&pAboutStrInstallmentInfo, pAboutStrInstInfo4);
	}
	else
	{
		pAboutStrInstallmentInfo = TransString("");
	}

    //校准
	pAboutStrNextCalibrationDate = TransString("Next Calibration Date：");
    pAboutStrLastCalibrationDate = TransString("Last Calibration Date：");
    char lastTemp[128] = {0};
    char currentTemp[128] = {0};
    sprintf(lastTemp, "%.4d-%.2d-%.2d", pFactoryConf->lastDate.year+1970, pFactoryConf->lastDate.month+1, pFactoryConf->lastDate.day+1);
    sprintf(currentTemp, "%.4d-%.2d-%.2d", pFactoryConf->currentDate.year+1970, pFactoryConf->currentDate.month+1, pFactoryConf->currentDate.day+1);
    pAboutStrLastCalibrationDateNum = TransString(lastTemp);
    pAboutStrNextCalibrationDateNum =  TransString(currentTemp);
    //信息
	pAboutStrInfo = TransString("Information");
    //模块
	pAboutStrModule = TransString("Platform：");
    pAboutStrModuleName = TransString("Module：");
    pAboutStrSerial = TransString("Serial No:");
	pAboutStrDeepEdition = TransString("Version：");
	pAboutStrDate = TransString("S/W Date：");
    pAboutStrSWEdition = TransString("OTDR_PLATFORM"); 
    pAboutStrModuleNum = TransString("OTDR_MODULE");
    sprintf(vBuf,"V%s",pPar.Userfs);
    char SN[20];
    GenerateSerialNumber(SN, pCurSN);
    pAboutStrSerialNum = TransString(SN);
    pAboutStrDeepEditionNum = TransString(vBuf);
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
static int AboutTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //释放状态栏上的文本
    //...
    //释放按钮区的文本
    //...
    GuiMemFree(pAboutStrCorpName);
    GuiMemFree(pAboutStrRightsHave);
    GuiMemFree(pAboutStrLabel);
    GuiMemFree(pSerialNumberStrLabel);
	GuiMemFree(pAboutStrInstallmentInfo);
	GuiMemFree(pAboutStrInstInfo2);
	GuiMemFree(pAboutStrInstInfo3);
	GuiMemFree(pAboutStrInstInfo4);
    //联系标签
    GuiMemFree(pAboutLblNextCalibrationDateNum);
    GuiMemFree(pAboutStrLastCalibrationDate);
    GuiMemFree(pAboutStrLastCalibrationDateNum);
    GuiMemFree(pAboutStrNextCalibrationDate);
    GuiMemFree(pAboutStrWebsite);
    //信息标签
    GuiMemFree(pAboutStrInfo);
    GuiMemFree(pAboutStrSWEdition);
    //GuiMemFree(pAboutStrVersionNum);
    GuiMemFree(pAboutStrVersionTime);

    //模块标签
    GuiMemFree(pAboutStrModule);
    GuiMemFree(pAboutStrModuleName);
    GuiMemFree(pAboutStrSerial);
    GuiMemFree(pAboutStrDeepEdition);
    GuiMemFree(pAboutStrModuleNum);
    GuiMemFree(pAboutStrSerialNum);
    GuiMemFree(pAboutStrDeepEditionNum);
    //释放桌面上的文本
    //...
    //...

    return iReturn;
}

//菜单栏控件回调
static void AboutWndMenuCallBack(int selected)
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
		SendWndMsg_WindowExit(pFrmAbout);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
    case 1://调取时间窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmTimeSetInit, FrmTimeSetExit, 
		                    FrmTimeSetPaint, FrmTimeSetLoop, 
					        FrmTimeSetPause, FrmTimeSetResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmAbout);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调取语言设置窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmLanguageSetInit, FrmLanguageSetExit, 
							FrmLanguageSetPaint, FrmLanguageSetLoop, 
							FrmLanguageSetPause, FrmLanguageSetResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmAbout);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 3:	//调取wifi窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmWiFiInit, FrmWiFiExit, 
							FrmWiFiPaint, FrmWiFiLoop, 
							FrmWiFiPause, FrmWiFiResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmAbout);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 4:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmSysMaintainInit, FrmSysMaintainExit, 
		                    FrmSysMaintainPaint, FrmSysMaintainLoop, 
					        FrmSysMaintainPause, FrmSysMaintainResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmAbout);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 5://调取关于
	    #ifdef MINI2
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAboutInit, FrmAboutExit, 
							FrmAboutPaint, FrmAboutLoop, 
							FrmAboutPause, FrmAboutResume,
							NULL);			        //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmAbout);	        //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			    //发送消息以便调用新的窗体
		#endif
		break;	
    case BACK_DOWN:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                            FrmMainInit, FrmMainExit,
                            FrmMainPaint, FrmMainLoop,
                            FrmMainPause, FrmMainResume,
                            NULL);
        SendWndMsg_WindowExit(pFrmAbout);
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case HOME_DOWN:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                            FrmMainInit, FrmMainExit,
                            FrmMainPaint, FrmMainLoop,
                            FrmMainPause, FrmMainResume,
                            NULL);
        SendWndMsg_WindowExit(pFrmAbout);
        SendSysMsg_ThreadCreate(pWnd);
        break;
    default:
        break;
    }
}

//flag = 1，logo，号码无
//flag = 0，二维码，号码
void setAboutStyle(int flag)
{
    LOG(LOG_INFO, "-- flag = %d --\n", flag);

    if(flag)
    {
        setCountryStyle(LANG_KOREAN);
        TouchChange("about_mini.bmp", pAboutBtnSWEdition, NULL, NULL, 1);
    }
    else
    {
        setCountryStyle(LANG_CHINASIM);
        TouchChange("about_mini.bmp", pAboutBtnSWEdition, NULL, NULL, 1);
    }
    RefreshScreen(__FILE__, __func__, __LINE__);
}


void setCountryStyle(unsigned int uiCountryStyle)
{
    pCurSystemSet->uiCountryStyle = (char)uiCountryStyle;
#ifdef EEPROM_DATA
    SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);
#endif
}

//切换到序列号输入界面
static int AboutSwitchSerialNumber_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    return iReturn;
}
static int AboutSwitchSerialNumber_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    serialNumberDialogInit(1,AboutReCreateWindow);
    
    return iReturn;
}

//提示窗口回调函数
static void AboutReCreateWindow(GUIWINDOW **pWnd)
{
     *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmAboutInit,  FrmAboutExit,
                                   FrmAboutPaint, FrmAboutLoop,
                                   FrmAboutPause, FrmAboutResume,
                                   NULL);          //pWnd由调度线程释放
}

//窗体构造函数
GUIWINDOW* CreateAboutWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmAboutInit,  FrmAboutExit,
                                   FrmAboutPaint, FrmAboutLoop,
                                   FrmAboutPause, FrmAboutResume,
                                   NULL);          //pWnd由调度线程释放
    return pWnd;
}

//获得分期付款信息，当前第几期，剩余多少天
//如果有分期付款信息，返回1，没有则返回0
static int GetInstInfo(char *periodBuf, char *daysBuf)
{
	int iRet = 0;
	int total = 0;
	int period = 0;
	int remainDays = 0;
	
	INSTALLMENT inst;
	GetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET);
	if (Encrypted == InstEncryptedFlag(&inst))
	{
		total = InstTotalPeriods(&inst);
		period = InstCurrentPeriod(&inst);
		remainDays = InstRemainDays(&inst, 1, 0);
		if (0 == period)
		{
			sprintf(periodBuf, " X-X , ");
			sprintf(daysBuf, " XX");
		}
		else
		{
			sprintf(periodBuf, " %d-%d , ", total, period);
			sprintf(daysBuf, " %d", remainDays);
		}
		iRet = 1;
	}
	return iRet;
}
