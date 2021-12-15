/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsystemsetting.h
* 摘    要：  实现主窗体FrmSystemSetting的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/31 
*
*******************************************************************************/

#include "wnd_frmsystemsetting.h"

#include "app_frmtimeset.h"
#include "app_frmbatteryset.h"
#include "app_frmbrightset.h"
#include "app_systemsettings.h"

#include "guimessage.h"

#include "wnd_frmsystemsetcontrol.h"
#include "wnd_frmcalendar.h"
#include "wnd_droppicker.h"
#include "wnd_frmdialog.h"
#include "wnd_frmupdate.h"
#include "wnd_frmversioninfo.h"
#include "wnd_frmlanguageconfig.h"
#include "wnd_frmuserguide.h"
#include "wnd_popselector.h"

typedef struct Default_System_Set
{
    int iLanguageIndex;
    int iAutoOffIndex;
    int iStanbyTimeIndex;
    int iKeyWarningIndex;
}DEFAULT_SYSTEM_SET;

#define LANGUAGE_NUM            2
#define AUTO_OFF_NUM            4
#define STANDBY_TIME_BUM        4

static int* pLanguage = NULL;
static int* pLanguageIndex = NULL;
/*********************************背景控件定义*********************************/
static GUIPICTURE* pFrmSystemSettingBg = NULL;                    //背景

/*******************************************************************************
**							窗体FrmSystemSetting中的控件定义部分		                  **
*******************************************************************************/
static GUIWINDOW *pFrmSystemSetting = NULL;

/*******************************************************************************
**							窗体FrmSystemSetting 标题栏中的控件定义部分				      **
*******************************************************************************/
GUIPICTURE* pFrmSystemSettingMenu = NULL;
GUIPICTURE* pFrmSystemSettingBgIcon = NULL;

static GUILABEL* pFrmSystemSettingLblTitle = NULL;
static GUILABEL* pFrmSystemSettingLblInfo = NULL;

static GUICHAR* pFrmSystemSettingStrTitle = NULL;                 //SystemSetting
static GUICHAR* pFrmSystemSettingStrInfo= NULL;                   //Info

static SYSTEM_SET_CONTROL* pFrmSystemSetControl = NULL;          //系统设置控件

static DEFAULT_SYSTEM_SET* pDefaultSystemSet = NULL;
static unsigned char _g_ucFactoryFlg = 0;					     // 进行了恢复出厂设置的标志

/*******************************************************************************
*                   窗体FrmSystemSetting内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int SystemSettingTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int SystemSettingTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);


//系统信息回调函数
static void SystemSettingWndCallBack(int selected); 

//返回主菜单
static int WndSystemInfoMenuBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndSystemInfoMenuBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//回调函数
static void ReCreateSystemSettingWindow(GUIWINDOW **pWnd);
//选择语言后的回调函数
static void LanguageDropCallBack(int iSelected);
//选择自动关机后的回调函数
static void AutoOffDropCallBack(int iSelected);
//选择待机时间的回调函数
static void ScreenOffDropCallBack(int iSelected);
//选择按键音的回调函数
static void KeyWarningDropCallBack(int iSelected);
//执行恢复出厂设置的回调函数
static void ResetSystemOK(void);

//初始化系统设置的默认参数
static void InitialDefaultSystemPara(void);
//还原系统设置的默认参数
static void ClearDefaultSystemPara(void);
/*******************************************************************************
**							窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
        窗体frmsystemsetting的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmSystemSettingInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    //得到当前窗体对象 
    pFrmSystemSetting= (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    SystemSettingTextRes_Init(NULL, 0, NULL, 0);
    
    pFrmSystemSettingBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_systemsetting.bmp");
	pFrmSystemSettingBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_sys_icon.bmp");

    pFrmSystemSettingLblTitle= CreateLabel(36, 12, 100, 16, pFrmSystemSettingStrTitle);
    pFrmSystemSettingLblInfo= CreateLabel(8, 53, 200, 16, pFrmSystemSettingStrInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmSystemSettingLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmSystemSettingLblInfo);

    pFrmSystemSettingMenu= CreatePicture(0, 20, 36, 21, BmpFileDirectory"bg_sys_home_unpress.bmp");
    pFrmSystemSetControl= CreateSystemSetControlResource(3, 0, SystemSettingWndCallBack);
    
    //初始化系统默认参数
    InitialDefaultSystemPara();
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), 
                  pFrmSystemSetting, pFrmSystemSetting);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pFrmSystemSettingMenu, pFrmSystemSetting);
    //注册系统信息按钮区的控件
    AddSystemSetControlToWindow(pFrmSystemSetControl, pFrmSystemSetting);

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    GUIMESSAGE *pMsg = GetCurrMessage();
	//注册桌面上控件的消息处理
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmSystemSettingMenu, 
                    WndSystemInfoMenuBtn_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmSystemSettingMenu, 
            	    WndSystemInfoMenuBtn_Up, NULL, 0, pMsg);

    LoginSystemSetControlToMsg(pFrmSystemSetControl, pFrmSystemSetting);

    return iRet;
}

/***
  * 功能：
        窗体frmsystemsetting的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmSystemSettingExit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmSystemSetting = (GUIWINDOW *) pWndObj;

	//清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmSystemSetting);

    DestroyPicture(&pFrmSystemSettingBg);
	DestroyPicture(&pFrmSystemSettingBgIcon);
    DestroyPicture(&pFrmSystemSettingMenu);
    DestroyLabel(&pFrmSystemSettingLblTitle);
    DestroyLabel(&pFrmSystemSettingLblInfo);

    DestroySystemSetControlResource(&pFrmSystemSetControl);
    
    SystemSettingTextRes_Exit(NULL, 0, NULL, 0);
    //初始化系统默认参数
    ClearDefaultSystemPara();
#ifdef EEPROM_DATA
    SaveSystemSet(0, 0);
#endif   
    return iRet;
}

/***
  * 功能：
        窗体frmsystemsetting的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingPaint(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    DisplayPicture(pFrmSystemSettingBg);
	// DisplayPicture(pFrmSystemSettingBgIcon);
	// DisplayPicture(pFrmSystemSettingMenu);
    DisplayLabel(pFrmSystemSettingLblTitle);
    DisplayLabel(pFrmSystemSettingLblInfo);

    DisplaySystemSetControlResource(pFrmSystemSetControl);
    
    SetPowerEnable(1, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}

/***
  * 功能：
        窗体frmsystemsetting的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmsystemsetting的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingPause(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmsystemsetting的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingResume(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/*******************************************************************************
*                   窗体FrmSystemSetting内部文本操作函数
*******************************************************************************/
//初始化文本资源
static int SystemSettingTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    pFrmSystemSettingStrTitle= GetCurrLanguageText(MAIN_LBL_SYSTEM_SET);
    pFrmSystemSettingStrInfo= GetCurrLanguageText(SYSTEM_LBL_PARAMETER_SET);
    return 0;
}

//释放文本资源
static int SystemSettingTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    GuiMemFree(pFrmSystemSettingStrTitle);
    GuiMemFree(pFrmSystemSettingStrInfo); 
    return 0;
}

//系统信息回调函数
static void SystemSettingWndCallBack(int selected)
{
    switch (selected)
    {
        case LANGUAGE:
            {
				int count = GetLanguageCount();
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmSystemSetting);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                int iSelected = GetSelectedIndex(pDefaultSystemSet->iLanguageIndex);
                CreatePopSelector(120, count, NULL, pLanguage, iSelected,
                                    LanguageDropCallBack, pFrmSystemSetting);
            }
            break;
        case AUTO_OFF:
            {
                char* pAutoOff[AUTO_OFF_NUM] = {"1m", "5m", "10m", "Off"};
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmSystemSetting);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                CreatePopSelector(120, AUTO_OFF_NUM, pAutoOff, NULL, pDefaultSystemSet->iAutoOffIndex,
                                  AutoOffDropCallBack, pFrmSystemSetting);
            }
            break;
        case STANDBY_TIME:
            {
                char* pScreenOff[STANDBY_TIME_BUM] = {"30s", "1m", "5m", "Off"};
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmSystemSetting);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                CreatePopSelector(120, STANDBY_TIME_BUM, pScreenOff, NULL, pDefaultSystemSet->iStanbyTimeIndex,
                                  ScreenOffDropCallBack, pFrmSystemSetting);
            }
            break;
        case KEY_WARNING:
            KeyWarningDropCallBack(!pDefaultSystemSet->iKeyWarningIndex);
            break;
        case RESET:
            DialogInit(120, 90, GetCurrLanguageText(SYSTEM_LBL_RESTORE_FACTORY_DEFAULT),
                       GetCurrLanguageText(SYSTEM_LBL_CONFIRM_TO_RESTORE),
                       0, 1, ReCreateSystemSettingWindow, ResetSystemOK, NULL);
            break;
        case UPGRADE:
            {
                //if((CheckPowerOn(BATTERY) == 1) && (CheckBatteryValueOverHalf() == 1))
                if (1)
         		{
        		    //临时变量定义
	                GUIWINDOW *pWnd = NULL;
        			/* 调用系统升级窗体 */
        			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
        								FrmUpdateInit, FrmUpdateExit, 
        								FrmUpdatePaint, FrmUpdateLoop, 
        								FrmUpdatePause, FrmUpdateResume,
        								NULL);			
        			SendWndMsg_WindowExit(pFrmSystemSetting);  
        			SendSysMsg_ThreadCreate(pWnd); 				
         		}
         		/* 电池电量不足或者没插入电池 */
         		else
         		{
         		    DialogInit(120, 90, TransString("Warning"), 
                 		   GetCurrLanguageText(SYSTEM_LBL_INPUT_BATTERY),
                 		   0, 0, ReCreateSystemSettingWindow, NULL, NULL);
         		}
            }
            break;
        case VERSION_INFO:
            {
                //临时变量定义
                GUIWINDOW *pWnd = NULL;
    			/* 调用系统升级窗体 */
    			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    								FrmVersionInfoInit, FrmVersionInfoExit, 
    								FrmVersionInfoPaint, FrmVersionInfoLoop, 
    								FrmVersionInfoPause, FrmVersionInfoResume,
    								NULL);
    			SendWndMsg_WindowExit(pFrmSystemSetting);  
    			SendSysMsg_ThreadCreate(pWnd);
            }
            break;
		case USER_GUIDE:
			// DialogInit(120, 90, TransString("Warning"),
			// 	TransString("Please design user guide window!"),
			// 	1, 0, ReCreateSystemSettingWindow, NULL, NULL);
            {
                //临时变量定义
                GUIWINDOW *pWnd = NULL;
                /* 调用系统升级窗体 */
                pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                    FrmUserGuideInit, FrmUserGuideExit,
                                    FrmUserGuidePaint, FrmUserGuideLoop,
                                    FrmUserGuidePause, FrmUserGuideResume,
                                    NULL);
                SendWndMsg_WindowExit(pFrmSystemSetting);
                SendSysMsg_ThreadCreate(pWnd);
            }
			break;
        default:
            break;
    }

}


//返回主菜单
static int WndSystemInfoMenuBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    return 0;
}

static int WndSystemInfoMenuBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    ReturnMenuOK();
    
    return 0;
}

//回调函数
static void ReCreateSystemSettingWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                			FrmSystemSettingInit, FrmSystemSettingExit, 
                			FrmSystemSettingPaint, FrmSystemSettingLoop, 
                			FrmSystemSettingPause, FrmSystemSettingResume,
                			NULL);
}

/***
  * 功能：
		选择语言后的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void LanguageDropCallBack(int iSelected)
{
	pDefaultSystemSet->iLanguageIndex = pLanguageIndex[iSelected];
#ifdef EEPROM_DATA
    SetCurrLanguage(pLanguageIndex[iSelected]);
#endif
#ifdef EEPROM_DATA
    SaveSystemSet(0, 0);
#endif
	GUIWINDOW* pWnd = NULL;
	ReCreateSystemSettingWindow(&pWnd);
	SendWndMsg_WindowExit(pFrmSystemSetting);
	SendSysMsg_ThreadCreate(pWnd);
}

/***
  * 功能：
		选择自动关机后的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void AutoOffDropCallBack(int iSelected)
{
    int index[AUTO_OFF_NUM] = {1, 5, 10, 0};//单位是分钟
    pDefaultSystemSet->iAutoOffIndex = iSelected;
#ifdef EEPROM_DATA
    SetPowerOffTime(index[iSelected]);
    SaveSystemSet(0, 0);
#endif
    FrmSystemSettingPaint(pFrmSystemSetting);
    SendWndMsg_LoopEnable(pFrmSystemSetting);
}

/***
  * 功能：
		选择待机时间的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void ScreenOffDropCallBack(int iSelected)
{
    int index[STANDBY_TIME_BUM] = {30, 60, 300, 0};//单位是s
    pDefaultSystemSet->iStanbyTimeIndex = iSelected;
#ifdef EEPROM_DATA
    SetScreenOffTime(index[iSelected]);
    SaveSystemSet(0, 0);
#endif
    FrmSystemSettingPaint(pFrmSystemSetting);
    SendWndMsg_LoopEnable(pFrmSystemSetting);
}

/***
  * 功能：
		选择按键音的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void KeyWarningDropCallBack(int iSelected)
{
    pDefaultSystemSet->iKeyWarningIndex = iSelected;
#ifdef EEPROM_DATA
    SetWarningSpeakerEnable(iSelected);
    SaveSystemSet(0, 0);
#endif
    FrmSystemSettingPaint(pFrmSystemSetting);
    SendWndMsg_LoopEnable(pFrmSystemSetting);
}

/***
  * 功能：
        执行恢复出厂设置的回调函数
  * 参数：
		无
  * 返回：
        无
  * 备注：
***/
static void ResetSystemOK(void)
{
	_g_ucFactoryFlg = 1;
	
	ShowDialogExtraInfo(GetCurrLanguageText(SYSTEM_LBL_RESTORING));
#ifdef EEPROM_DATA
    ResetAllSettings();
    unsigned short languageRecover = GetCurrLanguage(); //工厂提出恢复出厂设置不改变语言
    SaveSystemSet(1, languageRecover);
#endif    
    ShowDialogExtraInfo(GetCurrLanguageText(SYSTEM_LBL_RESTORED_SUCCESS));
    
	while(1);
}

/***
  * 功能：
        获取恢复出厂设置的标志位
  * 参数：
		无
  * 返回：
        无
  * 备注：
***/
unsigned char GetFactoryFlag(void)
{
    return _g_ucFactoryFlg;
}

//初始化系统设置的默认参数
static void InitialDefaultSystemPara(void)
{
    pDefaultSystemSet= (DEFAULT_SYSTEM_SET*)calloc(1, sizeof(DEFAULT_SYSTEM_SET));

    if(pDefaultSystemSet== NULL)
    {
        LOG(LOG_ERROR, "----FrmSystemSettingInit---pDefaultSystemSet malloc error ----\n");
        return;
    }
	pLanguage = GetSelectedLanguageList();
	pLanguageIndex = GetSelectedLanguageListIndex();
    pDefaultSystemSet->iLanguageIndex = GetCurrLanguage();
    pDefaultSystemSet->iAutoOffIndex = GetPowerOffTimeIndex(GetPowerOffTime());
    pDefaultSystemSet->iStanbyTimeIndex = GetScreenOffTimeIndex(GetScreenOffTime());
    pDefaultSystemSet->iKeyWarningIndex = GetWarningSpeakerEnable();
}

//还原系统设置的默认参数
static void ClearDefaultSystemPara(void)
{
    if(pDefaultSystemSet)
    {
        GuiMemFree(pDefaultSystemSet);
    }

	if (pLanguage)
	{
		GuiMemFree(pLanguage);
	}
}
