/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlogo.c
* 摘    要：  实现主窗体FrmLogo的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmlogo.h"

/****************************************
* 为实现窗体frmlogo而需要引用的其他头文件
****************************************/
#include "app_global.h"
#include "app_parameter.h"
#include "app_frminit.h"
#include "app_frmsendfile.h"
#include "app_getsetparameter.h"
#include "app_frmupdate.h"
#include "appwifi.h"
#include "app_systemsettings.h"
#include "app_frmbatteryset.h"
#include "app_frmbrightset.h"

#include "guiphoto.h"

#include "wnd_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmfilebrowse.h"
#include "wnd_frminstallment.h"
#include "wnd_frmmark.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmdialog.h"

#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <errno.h>
/*********************************
* OTDR应用交互界面设置的数据结构
**********************************/
extern int iPowerDownFlag;
extern PSYSTEMSET pCurSystemSet;
extern SOLA_FILE_NAME_SETTING *pBmpFileNamedSetting;
extern MarkParam* pMarkParam;
extern SOLA_MESSAGER* pSolaMessager;

int iInitOver = 0;		//标记初始化或资源释放是否完成
extern CURR_WINDOW_TYPE enCurWindow;

//启动动画
#define BOOT_LOGO_CNT	7
char *pStrBootResourceAnim[BOOT_LOGO_CNT] = {
	BmpFileDirectory"bg_logo_0.bmp", BmpFileDirectory"bg_logo_1.bmp",
	BmpFileDirectory"bg_logo_2.bmp", BmpFileDirectory"bg_logo_3.bmp",
	BmpFileDirectory"bg_logo_4.bmp", BmpFileDirectory"bg_logo_5.bmp",
	BmpFileDirectory"bg_logo_6.bmp"
};
/************************
* 窗体frmlogo中的窗体控件
************************/
static GUIWINDOW *pFrmLogo = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pLogoBgDesk = NULL;

//状态栏上的控件
//...

//桌面上的控件
//...

//按钮区的控件
//...

//信息栏上的控件
//...


/********************************
* 窗体frmlogo中的文本资源处理函数
********************************/
//初始化文本资源
static int LogoTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int LogoTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmlogo中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int LogoWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int LogoWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
//...

//按钮区控件的事件处理函数
//...

//信息栏控件的事件处理函数
//...


/********************************
* wifi 处理
********************************/
//开机wifi信息检测
extern int WifiStartInit(void);
//vsystem声明

/***
  * 功能：
        负责进入应用程序时初始化和退出应用时资源回收的线程
  * 参数：
        1.void *pThreadArg:    线程参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
void *InitThreadFunc(void *pThreadArg)
{
	//错误标志、返回值定义
	static int iReturn = 0;
	
	//OTDR应用初始化，包含所需内存空间分配和处理线程的创建
	if (iPowerDownFlag == 0)
	{
#ifdef EEPROM_DATA
        /* 读取eeprom中的参数 */
        LoadAllSettings();
#endif
        SystemSetInit();
		iReturn = OtdrPlatformInit();
        __SetFileOperationType(pOtdrTopSettings->pUser_Setting->sFileNameSetting.enFileOperation);
        __SetCurFileType(pOtdrTopSettings->pUser_Setting->sFileNameSetting.enFilter);
#ifdef POWER_DEVICE
        if(0 == UpdatePwrInfoFromMcu())
        {
            SetBrightness(pCurSystemSet->uiDCLCDBright, pCurSystemSet->uiACLCDBright); 
        }
#endif
		iInitOver = 1;
		
		while(1)
		{
			MsecSleep(500);
			sync();
		}	
	}
	else
	{
		/* 没有进行恢复出厂设置 */
		if(!GetFactoryFlag())
		{
            CODER_LOG(CoderGu, "Start save eeprom \n");
#ifdef EEPROM_DATA
            //设置FrontSet并保存
            SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sFrontSetting[WAVELEN_1310], sizeof(FRONT_SETTING), FRONTSET_1310);
            SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sFrontSetting[WAVELEN_1550], sizeof(FRONT_SETTING), FRONTSET_1550);
            SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sFrontSetting[WAVELEN_1625], sizeof(FRONT_SETTING), FRONTSET_1625);
            SaveSettings(FRONTSET_1310);
            SaveSettings(FRONTSET_1550);
            SaveSettings(FRONTSET_1625);
			//设置CommonSet并保存
	        SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sCommonSetting, sizeof(COMMON_SETTING), COMMON_SET);
            SaveSettings(COMMON_SET);

			//设置SampleSet并保存
			SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sSampleSetting, sizeof(SAMPLE_SETTING), SAMPLE_SET);
            SaveSettings(SAMPLE_SET);

			//设置AnalysisSet并保存
        	SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sAnalysisSetting, sizeof(ANALYSIS_SETTING), ANALYSIS_SET);
            SaveSettings(ANALYSIS_SET);
			
			//设置OtherSet并保存
		    SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sOtherSetting, sizeof(OTHER_SETTING), RESULT_SET);
            SaveSettings(RESULT_SET);

			//设置FileNameSet并保存
		    SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sFileNameSetting, sizeof(FILE_NAME_SETTING), FILENAME_SET);
            SaveSettings(FILENAME_SET);
            
			//设置DefSavePath并保存
		    SetSettingsData((void*)&pOtdrTopSettings->sDefSavePath, sizeof(DEFAULT_SAVEPATH), OTDR_SOLA_DEFSAVEPATH);
            SaveSettings(OTDR_SOLA_DEFSAVEPATH);
            
			// 保存系统设置参数
            SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
            SaveSettings(SYSTEM_SET);

            // 保存标识参数
            SetSettingsData((void*)pMarkParam, sizeof(MarkParam), OTDR_MARK_SET);
            SaveSettings(OTDR_MARK_SET);

            // 保存SOLA标识参数
            SetSettingsData((void*)&pSolaMessager->newIdentify, sizeof(MarkParam), SOLA_IDENT);
            SaveSettings(SOLA_IDENT);            

            // 保存SOLA设置界面参数
            SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sSolaSetting, sizeof(SolaSettings), SOLA_SETTING_PARA);
            SaveSettings(SOLA_SETTING_PARA);

            // 保存SOLA自动命名参数
            SetSettingsData((void*)&pSolaMessager->autoFilename, sizeof(SOLA_FILE_NAME_SETTING), SOLA_FILENAME_SET);
            SaveSettings(SOLA_FILENAME_SET);
            
			// 保存BMP自动命名设置参数
            SetSettingsData((void*)pBmpFileNamedSetting, sizeof(SOLA_FILE_NAME_SETTING), BMP_FILENAME_SET);
            SaveSettings(BMP_FILENAME_SET);
			LOG(LOG_OPEN, "------- EEPROM save OK\n");
#endif		
		}

		iInitOver = 1;

        mysystem("poweroff");
	}

    //退出线程
    ThreadExit(&iReturn);
    
    return &iReturn;
}

/***
  * 功能：
        窗体frmlogo的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLogoInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	iInitOver = 0;
	
	enCurWindow = ENUM_LOGO_WIN;	//设置当前窗体

    //得到当前窗体对象
    pFrmLogo = (GUIWINDOW *) pWndObj;

    //初始化字体资源
    //...

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    LogoTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    if (iPowerDownFlag == 0)
    {
		pLogoBgDesk = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_logo_0.bmp");
    }
    else
    {
		pLogoBgDesk = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_poweroff.bmp");
    }
    
    //建立状态栏上的控件
    //...
    //建立桌面上的控件
    //...
    //建立按钮区的控件
    //...
    //建立信息栏上的控件
    //...

    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件
    //...
    //设置按钮区的控件
    //...

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //获得控件队列的互斥锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmLogo, 
                  pFrmLogo);
    //注册桌面上的控件
    //...
    //注册按钮区的控件
    //...
    //注册信息栏上的控件
    //...
    //释放控件队列的互斥锁

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmLogo, 
                    LogoWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmLogo, 
                    LogoWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    //...
    //注册按钮区控件的消息处理
    //...
    //注册信息栏上控件的消息处理
    //...
    //释放消息队列的互斥锁
    return iReturn;
}


/***
  * 功能：
        窗体frmlogo的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLogoExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmLogo = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmLogo);

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pLogoBgDesk);
    //销毁状态栏上的控件
    //...
    //销毁桌面上的控件
    //...
    //销毁按钮区的控件
    //...
    //销毁信息栏上的控件
    //...

    //释放文本资源
    //***************************************************************/
    LogoTextRes_Exit(NULL, 0, NULL, 0);
	

    //释放字体资源
    //***************************************************************/
    //释放按钮区的字体
    //...
    return iReturn;
}


/***
  * 功能：
        窗体frmlogo的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLogoPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmLogo = (GUIWINDOW *) pWndObj;
    DisplayPicture(pLogoBgDesk);	
	RefreshScreen(__FILE__, __func__, __LINE__);
	
    return iReturn;
}


/***
  * 功能：
        窗体frmlogo的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLogoLoop(void *pWndObj)
{
     //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    int iBootLogoNum = 0;
    int iLoopCnt = 0;
	GUITHREAD thdInitThread;
    
    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);
	
	LOG(LOG_INFO, "-4----------iPowerDownFlag = %d\n", iPowerDownFlag);
	ThreadCreate(&thdInitThread, NULL, InitThreadFunc, NULL);
	if (iPowerDownFlag == 0)
	{
		iBootLogoNum = 0;
	}
	else
	{
		DisplayPicture(pLogoBgDesk);
		RefreshScreen(__FILE__, __func__, __LINE__);	 
	}

	while(0 == iPowerDownFlag)
	{
		if ( iInitOver == 1 )
		{
            SetPictureBitmap(pStrBootResourceAnim[6], pLogoBgDesk);
			DisplayPicture(pLogoBgDesk);
			 
			break;
		}

		SetPictureBitmap(pStrBootResourceAnim[iBootLogoNum], pLogoBgDesk);
		DisplayPicture(pLogoBgDesk);
		iBootLogoNum = (iBootLogoNum + 1) % BOOT_LOGO_CNT;
		if (iBootLogoNum == 0)
		{
			iLoopCnt++;
        }
        RefreshScreen(__FILE__, __func__, __LINE__);	 
			 
		MsecSleep(50);
    }

    if (iPowerDownFlag == 0)
    {
        GUIWINDOW *pWnd;

		#ifdef FIRST_WRITE
		//调度frmupdate
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmUpdateInit, FrmUpdateExit, 
							FrmUpdatePaint, FrmUpdateLoop, 
							FrmUpdatePause, FrmUpdateResume,
							NULL);			//pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmLogo);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
		#else
		
		//wifi_Init();
		
		//开机wifi状态检测与恢复
		//WifiStartInit();
		
		//文件传输
		//Sendfile_Init();
        #ifdef EEPROM_DATA
		//检查分期付款是否过期
		if (InstOutOfDate(0))
		{
			//调用锁住界面
			serialNumberDialogInit(0,FrmMainReCreateWindow);
		}
		else
		{
			//判断是否到期
			if (IsLeaseExpiration())
			{
				DialogInit(120, 90, TransString("Message"),
					GetCurrLanguageText(DIALOG_LBL_LEASE_DATE),
					0, 2, NULL, NULL, NULL);
			}
			else
			{
				//调度frmmain
				pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
					FrmMainInit, FrmMainExit,
					FrmMainPaint, FrmMainLoop,
					FrmMainPause, FrmMainResume,
					NULL);			//pWnd由调度线程释放
				SendWndMsg_WindowExit(pFrmLogo);		//发送消息以便退出当前窗体
				SendSysMsg_ThreadCreate(pWnd);
            }
       	}
        #else
        //调度frmmain
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
            FrmMainInit, FrmMainExit,
            FrmMainPaint, FrmMainLoop,
            FrmMainPause, FrmMainResume,
            NULL);			//pWnd由调度线程释放
        SendWndMsg_WindowExit(pFrmLogo);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        #endif//END EEPROM_DATA

        #endif//END FIRST_WRITE
		//启动公共线程
		CreateThread();
    }
    else
	{
    	while (!iInitOver) 
    	{
		    MsecSleep(20);
		}
        
		GUIMESSAGE_ITEM* pItem;
		//发送窗体消息GUIMESSAGE_SYS_EXIT给自身
		pItem = (GUIMESSAGE_ITEM *) GuiMemAlloc(sizeof(GUIMESSAGE_ITEM));
		if (NULL == pItem)
		{
			return -1; 
		}
		else
		{
			GUIMESSAGE* pMsg;
			pMsg = GetCurrMessage();
			pItem->iMsgType = GUIMESSAGE_TYP_SYS;
			pItem->iMsgCode = GUIMESSAGE_SYS_EXIT;
			
			WriteMessageQueue(pItem, pMsg, 1); //pItem由本窗体的消息处理释放
			
			LOG(LOG_INFO, "Exit Wndproc!\n");
		}

		
	}

    return iReturn;
}


/***
  * 功能：
        窗体frmlogo的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLogoPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmlogo的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmLogoResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int LogoTextRes_Init(void *pInArg, int iInLen, 
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
    //初始化信息栏上的文本
    //...

    return iReturn;
}


//释放文本资源
static int LogoTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
    //...
    //释放桌面上的文本
    //...
    //释放按钮区的文本
    //...
    //释放信息栏上的文本
    //...

    return iReturn;
}


static int LogoWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    switch (uiValue)
    {
		case KEYCODE_ESC:
			break;
		case KEYCODE_HOME:
			break;
		default:
			break;
    }

    return iReturn;
}


static int LogoWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    switch (uiValue)
    {
		case KEYCODE_ESC:
			break;
		case KEYCODE_HOME:
#ifdef SAVE_SCREEN
			ScreenShot();
#endif
			break;
		default:
			break;
    }

    return iReturn;
}

