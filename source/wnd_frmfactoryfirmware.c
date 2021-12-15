/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactoryfirmware.c
* 摘    要：  实现烧录工厂配置文件功能
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2018-01-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmfactoryfirmware.h"
#include "app_systemsettings.h"

/**********************************************************************************
**						为实现窗体frmfactoryfirmware而需要引用的其他头文件 	    **
***********************************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "app_parameter.h"
#include "wnd_frmdialog.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmmaintancedate.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmauthorization.h"
#include "wnd_frmfactory.h"
#include "wnd_frmfactorydr.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frminputpassward.h"
#include "guiphoto.h"
#include "wnd_frmoperatesor.h"
#include "wnd_frmfactoryspeakerset.h"
#include "wnd_frmfactorylaserset.h"
/**********************************************************************************
**									变量定义						 			 **
***********************************************************************************/
extern PFactoryConfig  pFactoryConf;
/**********************************************************************************
**							窗体Frmfactoryfirmware中的控件定义部分				**
***********************************************************************************/
static GUIWINDOW *pFrmFactoryFirmware = NULL;


/**********************************背景控件定义************************************/
static GUIPICTURE *pFactoryFirmwareBtnLeftBg  = NULL;
static GUIPICTURE *pFactoryFirmwareBgTabTitle  = NULL;


/******************************窗体标题栏控件定义**********************************/
static GUICHAR *pFactoryFirmwareStrFrmName = NULL;	  			//窗体左上角标题栏文本
static GUILABEL *pFactoryFirmwareLblFrmName = NULL;				//窗体左上角的Label
static GUICHAR *pFactoryFirmwareStrSpeaker = NULL;	  			//窗体左上角标题栏文本
static GUILABEL *pFactoryFirmwareLblSpeaker = NULL;				//窗体左上角的Label
static GUICHAR *pFactoryFirmwareStrLaser = NULL;	  			//窗体左上角标题栏文本
static GUILABEL *pFactoryFirmwareLblLaser = NULL;				//窗体左上角的Label


/****************************软件升级内的控件定义**********************************/
//软件升级内的文本定义
static GUICHAR *pFactoryFirmwareStrUpdateTitle   = NULL;	  
static GUICHAR *pFactoryFirmwareStrWarn 	 = NULL;	  		//显示警告信息的文本
static GUICHAR *pFactoryFirmwareStrUpdate = NULL;	  			//显示升级的文本

//软件升级内的Label定义
static GUILABEL *pFactoryFirmwareLblUpdateTitle = NULL;
static GUILABEL *pFactoryFirmwareLblWarn = NULL;
static GUILABEL *pFactoryFirmwareLblUpdate = NULL;

//软件升级内的图片定义
static GUIPICTURE *pFactoryFirmwareBtnUpdate  = NULL;  

/********************************右侧菜单控件定义**********************************/
static WNDMENU1 *pFactoryFirmwareMenu = NULL;


/**********************************************************************************
**	    	窗体FrmFactoryFirmware中的初始化文本资源、 释放文本资源函数定义部分	**
***********************************************************************************/
//初始化文本资源
static int FactoryFirmwareTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int FactoryFirmwareTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/**********************************************************************************
**			    	窗体FrmFactoryFirmware中的控件事件处理函数定义部分			**
***********************************************************************************/

/***************************窗体的按键事件处理函数********************************/
static int FactoryFirmwareWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int FactoryFirmwareWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);


/************************软件升级内的按键事件处理函数******************************/
static int FactoryFirmwareBtnBurn_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int FactoryFirmwareBtnBurn_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int FactoryFirmwareBtnGotoSpeakerSet_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int FactoryFirmwareBtnGotoSpeakerSet_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int FactoryFirmwareBtnGotoLaserSet_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int FactoryFirmwareBtnGotoLaserSet_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

/**********************************************************************************
**			    			窗体内的其他函数声明					 		     **
***********************************************************************************/
static void ReCreateWindow(GUIWINDOW **pWnd);
static void FactoryFirmwareWndMenuCallBack(int selected);


/***
  * 功能：
        窗体FrmFactoryFirmware的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFactoryFirmwareInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    unsigned int strMenu[] = {	
        1
	};

	GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmFactoryFirmware = (GUIWINDOW *) pWndObj;

   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FactoryFirmwareTextRes_Init(NULL, 0, NULL, 0);

	/* 窗体背景图片 */
    pFactoryFirmwareBtnLeftBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factoryfirmware.bmp");
    pFactoryFirmwareBgTabTitle = CreatePhoto("otdr_top1f");
	/* 窗体标题的Label */
	pFactoryFirmwareLblFrmName = CreateLabel(0, 24, 100, 24, 
								  		    pFactoryFirmwareStrFrmName);	
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryFirmwareLblFrmName);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryFirmwareLblFrmName);
	
	pFactoryFirmwareLblSpeaker = CreateLabel(100, 24, 100, 24, 
								  		    pFactoryFirmwareStrSpeaker);	
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryFirmwareLblSpeaker);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryFirmwareLblSpeaker);	
	
	pFactoryFirmwareLblLaser = CreateLabel(200, 24, 100, 24, 
								  		    pFactoryFirmwareStrLaser);	
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryFirmwareLblLaser);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryFirmwareLblLaser);

	//软件升级内的图片定义	
    pFactoryFirmwareBtnUpdate = CreatePicture(273, 265, 138, 48, BmpFileDirectory"btn_update_unpress0.bmp");
	//软件升级内的Label定义
	pFactoryFirmwareLblUpdateTitle = CreateLabel(25, 130, 200, 24, 
								  		    pFactoryFirmwareStrUpdateTitle);
    pFactoryFirmwareLblWarn = CreateLabel(55, 198, 570, 40,
								  		    pFactoryFirmwareStrWarn);	
    pFactoryFirmwareLblUpdate = CreateLabel(273, 277, 138, 24,
								  		    pFactoryFirmwareStrUpdate);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryFirmwareLblUpdateTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryFirmwareLblWarn);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryFirmwareLblUpdate);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryFirmwareLblWarn);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryFirmwareLblUpdate);		


	//菜单栏控件
    pFactoryFirmwareMenu = CreateWndMenu1(factoryRank, sizeof(strMenu), strMenu, 
						MENU_BACK | MENU_HOME, 1, 0, 40, FactoryFirmwareWndMenuCallBack); 
	//注册桌面右侧按钮
	AddWndMenuToComp1(pFactoryFirmwareMenu, pFrmFactoryFirmware);	

	//注册桌面右侧按钮的消息处理
	LoginWndMenuToMsg1(pFactoryFirmwareMenu, pFrmFactoryFirmware);	

	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactoryFirmware, 
                  pFrmFactoryFirmware);
	
    //注册软件升级内的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryFirmwareBtnUpdate, 
                  pFrmFactoryFirmware);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryFirmwareLblSpeaker, 
                  pFrmFactoryFirmware);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryFirmwareLblLaser, 
                  pFrmFactoryFirmware);

	 //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmFactoryFirmware, 
                    FactoryFirmwareWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmFactoryFirmware, 
                    FactoryFirmwareWndKey_Up, NULL, 0, pMsg);
    //注册软件升级内的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryFirmwareBtnUpdate, 
                    FactoryFirmwareBtnBurn_Down, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryFirmwareBtnUpdate, 
                    FactoryFirmwareBtnBurn_Up, NULL, 0, pMsg);
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryFirmwareLblSpeaker, 
                    FactoryFirmwareBtnGotoSpeakerSet_Down, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryFirmwareLblSpeaker, 
                    FactoryFirmwareBtnGotoSpeakerSet_Up, NULL, 0, pMsg);
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryFirmwareLblLaser, 
                    FactoryFirmwareBtnGotoLaserSet_Down, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryFirmwareLblLaser, 
                    FactoryFirmwareBtnGotoLaserSet_Up, NULL, 0, pMsg);
    
    return iReturn;
}


/***
  * 功能：
        窗体FrmFactoryFirmware的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFactoryFirmwareExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmFactoryFirmware = (GUIWINDOW *) pWndObj;

	//清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmFactoryFirmware);

    //销毁窗体控件
    //***************************************************************/
    //销毁桌面背景图片
    DestroyPicture(&pFactoryFirmwareBtnLeftBg);
    DestroyPicture(&pFactoryFirmwareBgTabTitle);
	//销毁软件升级内的图片
	DestroyPicture(&pFactoryFirmwareBtnUpdate);

	//销毁窗体左上角窗体Label
	DestroyLabel(&pFactoryFirmwareLblFrmName);
	DestroyLabel(&pFactoryFirmwareLblSpeaker);
	DestroyLabel(&pFactoryFirmwareLblLaser);
	
	//销毁软件升级内的Label
	DestroyLabel(&pFactoryFirmwareLblUpdateTitle);
	DestroyLabel(&pFactoryFirmwareLblWarn);
	DestroyLabel(&pFactoryFirmwareLblUpdate);

	//销毁右侧区域的Label
	DestroyWndMenu1(&pFactoryFirmwareMenu);
	
	//释放文本资源
    //***************************************************************/
    FactoryFirmwareTextRes_Exit(NULL, 0, NULL, 0);
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmFactoryFirmware的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFactoryFirmwarePaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmFactoryFirmware = (GUIWINDOW *) pWndObj;

    //显示桌面背景图片
    DisplayPicture(pFactoryFirmwareBtnLeftBg);
    DisplayPicture(pFactoryFirmwareBgTabTitle);
	//显示软件升级内的图片
	DisplayPicture(pFactoryFirmwareBtnUpdate);
	
	//显示窗体左上角窗体Label
	DisplayLabel(pFactoryFirmwareLblFrmName);
	DisplayLabel(pFactoryFirmwareLblSpeaker);
	DisplayLabel(pFactoryFirmwareLblLaser);
	
	//显示软件升级内的Label
	DisplayLabel(pFactoryFirmwareLblUpdateTitle);
	DisplayLabel(pFactoryFirmwareLblWarn);
	DisplayLabel(pFactoryFirmwareLblUpdate);

	//显示菜单控件
	DisplayWndMenu1(pFactoryFirmwareMenu);

	SetPowerEnable(1, 1);	

	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmFactoryFirmware的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFactoryFirmwareLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);   	
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmFactoryFirmware的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFactoryFirmwarePause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体FrmFactoryFirmware的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFactoryFirmwareResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    窗体FrmFactoryFirmware中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/

/***
  * 功能：
        初始化文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FactoryFirmwareTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//窗体左上角的文本
	pFactoryFirmwareStrFrmName = TransString("FACTORY_IMPORT_PROFILE");
	pFactoryFirmwareStrSpeaker = TransString("FACTORY_SPEAKER_VOLUMESETTING");
	pFactoryFirmwareStrLaser = TransString("FACTORY_LASER_FREQUENCYSETTING");

	//系统升级内的文本
	pFactoryFirmwareStrUpdateTitle = TransString("FACTORY_IMPORT_PROFILE_TITLE");
	pFactoryFirmwareStrWarn = TransString("FACTORY_IMPORT_PROFILE_PROMPT");
	pFactoryFirmwareStrUpdate = TransString("FACTORY_IMPORT");
	
    return iReturn;
}


/***
  * 功能：
        释放文本资源
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FactoryFirmwareTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//释放左上角的文本
	GuiMemFree(pFactoryFirmwareStrFrmName);
	GuiMemFree(pFactoryFirmwareStrSpeaker);
	GuiMemFree(pFactoryFirmwareStrLaser);

	//释放系统升级内的文本
	GuiMemFree(pFactoryFirmwareStrUpdateTitle);
	GuiMemFree(pFactoryFirmwareStrWarn);
	GuiMemFree(pFactoryFirmwareStrUpdate);

    return iReturn;
}


/**********************************************************************************
**			    			窗体的按键事件处理函数				 				 **
***********************************************************************************/
/***
  * 功能：
        窗体的按键按下事件处理函数
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FactoryFirmwareWndKey_Down(void *pInArg, int iInLen, 
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

/***
  * 功能：
        窗体的按键弹起事件处理函数
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FactoryFirmwareWndKey_Up(void *pInArg, int iInLen, 
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

/**********************************************************************************
**			    			软件升级内的按键事件处理函数					     **
***********************************************************************************/

/***
  * 功能：
        升级按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FactoryFirmwareBtnBurn_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
	TouchChange("btn_update_press0.bmp", pFactoryFirmwareBtnUpdate, 
				NULL, pFactoryFirmwareLblUpdate, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
    return iReturn;

}


/***
  * 功能：
        升级按钮up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int FactoryFirmwareBtnBurn_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iErr = 0;
    static char buff[512];

	TouchChange("btn_update_unpress0.bmp", pFactoryFirmwareBtnUpdate, 
				NULL, pFactoryFirmwareLblUpdate, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);

    strcpy(buff, MntUsbDirectory);
	if(access(buff, F_OK) == 0)
	{
	    setLanguageConfigSerialNoFlag(0);
	    FileDialog(FILE_OPEN_MODE, MntUsbDirectory, buff, ReCreateWindow, BIN);
	}
	else
	{
        DialogInit(120, 90, TransString("DIALOG_WARN"),
		    TransString("DIALOG_NO_USB"),
			0, 0, ReCreateWindow, NULL, NULL);
	}

    return iErr;
}


/**********************************************************************************
**			    			恢复出厂设置内的按键事件处理函数					 **
***********************************************************************************/


 /***
  * 功能：
        用于对话框重新还原窗体时的回调函数
  * 参数：
        1.GUIWINDOW **pWnd:		指向窗体对象的指针的指针
  * 返回：
        无
  * 备注：
***/
static void ReCreateWindow(GUIWINDOW **pWnd)
{	
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                    FrmFactoryFirmwareInit, FrmFactoryFirmwareExit, 
                    FrmFactoryFirmwarePaint, FrmFactoryFirmwareLoop, 
			        FrmFactoryFirmwarePause, FrmFactoryFirmwareResume,
                    NULL);
}


/***
  * 功能：
        菜单栏控件回调
  * 参数：
        1、int selected:	按钮索引
  * 返回：
        无
  * 备注：
***/
static void FactoryFirmwareWndMenuCallBack(int selected)
{
	GUIWINDOW *pWnd = NULL;
	switch (selected)
	{
	case 0: 
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAuthorizationInit, FrmAuthorizationExit, 
							FrmAuthorizationPaint, FrmAuthorizationLoop, 
							FrmAuthorizationPause, FrmAuthorizationResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactoryFirmware);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
    case 1:        
        break;
    case 2:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacInit,FrmFacExit,
					  FrmFacPaint,FrmFacLoop,
					  FrmFacPause,FrmFacResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmFactoryFirmware);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 3:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacDRInit,FrmFacDRExit,
					  FrmFacDRPaint,FrmFacDRLoop,
					  FrmFacDRPause,FrmFacDRResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmFactoryFirmware);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 4:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactorySetInit, FrmFactorySetExit, 
                        FrmFactorySetPaint, FrmFactorySetLoop, 
				        FrmFactorySetPause, FrmFactorySetResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactoryFirmware);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 5:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMaintanceDateInit, FrmMaintanceDateExit, 
							FrmMaintanceDatePaint, FrmMaintanceDateLoop, 
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactoryFirmware);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN:
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmStandbysetInit, FrmStandbysetExit, 
						    FrmStandbysetPaint, FrmStandbysetLoop, 
						    FrmStandbysetPause, FrmStandbysetResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFactoryFirmware);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}

int FactoryFirmwareViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where)
{
	int iRet = 0;
	FACTORY_FIRMWARE factoryFirmware;

    iRet = LoadOtdrConfig(&factoryFirmware, fileName);
    
	if (!iRet)
	{
        LOG(LOG_INFO, "old Available Language : %llx \n", pFactoryConf->ulAvailableLanguage);
        pFactoryConf->ulAvailableLanguage = factoryFirmware.ulAvailableLanguage;
        LOG(LOG_INFO, "new Available Language : %llx \n", pFactoryConf->ulAvailableLanguage);

        //保存参数到eeprom中
        SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
        SaveSettings(FACTORY_CONFIG);

		DialogInit(120, 90, TransString("DIALOG_WARN"),
			TransString("FACTORY_IMPORT_PROFILE_SUCCESS"),
			0, 0, ReCreateWindow, NULL, NULL); 
    }
	
	return iRet;
}



static int FactoryFirmwareBtnGotoSpeakerSet_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactoryFirmwareBtnGotoSpeakerSet_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
					    FrmFactorySpeakerSetInit, FrmFactorySpeakerSetExit, 
					    FrmFactorySpeakerSetPaint, FrmFactorySpeakerSetLoop, 
					    FrmFactorySpeakerSetPause, FrmFactorySpeakerSetResume,
                        NULL);          
    SendWndMsg_WindowExit(pFrmFactoryFirmware);  
    SendSysMsg_ThreadCreate(pWnd); 
    return 0;
}


static int FactoryFirmwareBtnGotoLaserSet_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactoryFirmwareBtnGotoLaserSet_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
					    FrmFactoryLaserSetInit, FrmFactoryLaserSetExit, 
					    FrmFactoryLaserSetPaint, FrmFactoryLaserSetLoop, 
					    FrmFactoryLaserSetPause, FrmFactoryLaserSetResume,
                        NULL);          
    SendWndMsg_WindowExit(pFrmFactoryFirmware);  
    SendSysMsg_ThreadCreate(pWnd); 
    return 0;
}