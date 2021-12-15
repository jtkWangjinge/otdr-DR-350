/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsysmaintain.c
* 摘    要：  实现主窗体FrmSysMaintain的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：

*******************************************************************************/

#include "wnd_frmsysmaintain.h"


/**********************************************************************************
**						为实现窗体frmupdate而需要引用的其他头文件 				 **
***********************************************************************************/
#include "wnd_global.h"

#include "app_global.h"
#include "app_frmupdate.h"
#include "app_parameter.h"

#include "wnd_frmmain.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmwifi.h"
#include "wnd_frmdialog.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmabout.h"
#include "wnd_frmskinset.h"

#include "app_systemsettings.h"
#include "app_getsetparameter.h"
#include "app_frmbatteryset.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "guiphoto.h"

/**********************************************************************************
**									变量定义						 			 **
***********************************************************************************/
extern volatile int iSpeakerOnFlag;									// 蜂鸣器打开标志
extern PSYSTEMSET pCurSystemSet;


/**********************************************************************************
**							窗体FrmUpdate中的控件定义部分						 **
***********************************************************************************/
static GUIWINDOW *pFrmSysMaintain = NULL;


/**********************************背景控件定义************************************/
static GUIPICTURE *pSysMaintainBtnLeftBg  = NULL;
//static GUIPICTURE *pSysMaintainBgTabTitle  = NULL;


/******************************窗体标题栏控件定义**********************************/
static GUICHAR *pSysMaintainStrFrmName = NULL;	  			//窗体左上角标题栏文本
static GUILABEL *pSysMaintainLblFrmName = NULL;				//窗体左上角的Label


/****************************软件升级内的控件定义**********************************/
//软件升级内的文本定义
static GUICHAR *pSysMaintainStrUpdateTitle   = NULL;	  
static GUICHAR *pSysMaintainStrWarn 	 = NULL;	  		//显示警告信息的文本
static GUICHAR *pSysMaintainStrUpdate = NULL;	  			//显示升级的文本

//软件升级内的Label定义
static GUILABEL *pSysMaintainLblUpdateTitle = NULL;
static GUILABEL *pSysMaintainLblWarn = NULL;
static GUILABEL *pSysMaintainLblUpdate = NULL;

//软件升级内的图片定义
//static GUIPICTURE *pSysMaintainBtnWarn  = NULL;  
static GUIPICTURE *pSysMaintainBtnUpdate  = NULL; 


/****************************恢复出厂设置内的控件定义******************************/
//恢复出厂设置的文本定义
static GUICHAR *pSysMaintainStrResetTitle   = NULL;	  
static GUICHAR *pSysMaintainStrResetWarn 	 = NULL;	  		//显示警告信息的文本
static GUICHAR *pSysMaintainStrResetOK = NULL;				//'确定'文本

//恢复出厂设置内的Label定义
static GUILABEL *pSysMaintainLblResetTitle = NULL;
static GUILABEL *pSysMaintainLblResetWarn = NULL;
static GUILABEL *pSysMaintainLblResetOK = NULL;

//恢复出厂设置内的图片定义
static GUIPICTURE *pSysMaintainBtnResetOK = NULL;  


/********************************右侧菜单控件定义**********************************/
static WNDMENU1 *pSysMaintainMenu = NULL;


/**********************************************************************************
**	    	窗体FrmSysMaintain中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/
//初始化文本资源
static int SysMaintainTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int SysMaintainTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/**********************************************************************************
**			    	窗体FrmSysMaintain中的控件事件处理函数定义部分					 **
***********************************************************************************/

/***************************窗体的按键事件处理函数********************************/
static int SysMaintainWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int SysMaintainWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);


/************************软件升级内的按键事件处理函数******************************/
static int SysMaintainBtnUpdate_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int SysMaintainBtnUpdate_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);


/***********************恢复出厂设置内的按键事件处理函数***************************/
static int SysMaintainBtnResetOK_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int SysMaintainBtnResetOK_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);


/**********************************************************************************
**			    			窗体内的其他函数声明					 		     **
***********************************************************************************/
static void ReCreateWindow(GUIWINDOW **pWnd);
static void SysMaintainWndMenuCallBack(int selected);


/***
  * 功能：
        窗体FrmUpdate的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSysMaintainInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    #ifdef MINI2
    unsigned int strMenu[] = {
		SYSTEMSET_STANDBY,
		SYSTEMSET_TIME,
		SYSTEMSET_LANGUAGE,
		MAIN_LBL_WIFI,
		SYSTEMSET_SYSMAINTAIN,
		SYSTEM_ABOUT
	};
    #else
	unsigned int strMenu[] = {
		1
	};
    #endif
	GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmSysMaintain = (GUIWINDOW *) pWndObj;

   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    SysMaintainTextRes_Init(NULL, 0, NULL, 0);

	/* 窗体背景图片 */
    pSysMaintainBtnLeftBg = CreatePhoto("bg_sysmaintain");
    //pSysMaintainBgTabTitle = CreatePhoto("otdr_top1f");
	/* 窗体标题的Label */
	pSysMaintainLblFrmName = CreateLabel(0, 24, 100, 24, 
								  		    pSysMaintainStrFrmName);	
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSysMaintainLblFrmName);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSysMaintainLblFrmName);	

	//软件升级内的图片定义	
    pSysMaintainBtnUpdate = CreatePhoto("btn_update_unpress0");
	//软件升级内的Label定义
	pSysMaintainLblUpdateTitle = CreateLabel(25, 130, 300, 24, 
								  		    pSysMaintainStrUpdateTitle);
    pSysMaintainLblWarn = CreateLabel(40, 198, 280, 60,
								  		    pSysMaintainStrWarn);	
    pSysMaintainLblUpdate = CreateLabel(124, 277, 110, 24,
								  		    pSysMaintainStrUpdate);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSysMaintainLblUpdateTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSysMaintainLblWarn);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSysMaintainLblUpdate);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSysMaintainLblWarn);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSysMaintainLblUpdate);		

    pSysMaintainBtnResetOK = CreatePhoto("btn_update_unpress");
	//出厂设置内的Label定义
	pSysMaintainLblResetWarn = CreateLabel(367, 198, 280, 60, 
								  		    pSysMaintainStrResetWarn);	
	pSysMaintainLblResetTitle = CreateLabel(353, 130, 300, 24, 
								  		    pSysMaintainStrResetTitle);		
    pSysMaintainLblResetOK = CreateLabel(455, 277, 110, 24,
								  		    pSysMaintainStrResetOK);	
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSysMaintainLblResetTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSysMaintainLblResetWarn);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSysMaintainLblResetOK);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSysMaintainLblResetWarn);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSysMaintainLblResetOK);

	//菜单栏控件
	#ifdef MINI2
    pSysMaintainMenu = CreateWndMenu1(6, sizeof(strMenu), strMenu, 
						MENU_BACK | MENU_HOME, 4, 0, 40, SysMaintainWndMenuCallBack);
    #else
	pSysMaintainMenu = CreateWndMenu1(5, sizeof(strMenu), strMenu, 
						MENU_BACK | MENU_HOME, 4, 0, 40, SysMaintainWndMenuCallBack);
    #endif
	//注册桌面右侧按钮
	AddWndMenuToComp1(pSysMaintainMenu, pFrmSysMaintain);	

	//注册桌面右侧按钮的消息处理
	LoginWndMenuToMsg1(pSysMaintainMenu, pFrmSysMaintain);	

	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSysMaintain, 
                  pFrmSysMaintain);
	
    //注册软件升级内的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSysMaintainBtnUpdate, 
                  pFrmSysMaintain);

	//注册出厂设置内的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSysMaintainBtnResetOK, 
              		pFrmSysMaintain);

	 //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmSysMaintain, 
                    SysMaintainWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmSysMaintain, 
                    SysMaintainWndKey_Up, NULL, 0, pMsg);
    //注册软件升级内的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSysMaintainBtnUpdate, 
                    SysMaintainBtnUpdate_Down, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSysMaintainBtnUpdate, 
                    SysMaintainBtnUpdate_Up, NULL, 0, pMsg);

	//注册出厂设置内的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSysMaintainBtnResetOK, 
                    SysMaintainBtnResetOK_Down, NULL, 0, pMsg);
	
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSysMaintainBtnResetOK, 
                    SysMaintainBtnResetOK_Up, NULL, 0, pMsg);
    
    return iReturn;
}


/***
  * 功能：
        窗体FrmSysMaintain的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSysMaintainExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmSysMaintain = (GUIWINDOW *) pWndObj;

	//清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmSysMaintain);

    //销毁窗体控件
    //***************************************************************/
    //销毁桌面背景图片
    DestroyPicture(&pSysMaintainBtnLeftBg);
    //DestroyPicture(&pSysMaintainBgTabTitle);
	//销毁软件升级内的图片
    //DestroyPicture(&pSysMaintainBtnWarn);
	DestroyPicture(&pSysMaintainBtnUpdate);

	//销毁出厂设置内的图片
	DestroyPicture(&pSysMaintainBtnResetOK);

	//销毁窗体左上角窗体Label
	DestroyLabel(&pSysMaintainLblFrmName);
	
	//销毁软件升级内的Label
	DestroyLabel(&pSysMaintainLblUpdateTitle);
	DestroyLabel(&pSysMaintainLblWarn);
	DestroyLabel(&pSysMaintainLblUpdate);

	//销毁出厂设置内的Label
	DestroyLabel(&pSysMaintainLblResetTitle);
	DestroyLabel(&pSysMaintainLblResetWarn);
	DestroyLabel(&pSysMaintainLblResetOK);	

	//销毁右侧区域的Label
	DestroyWndMenu1(&pSysMaintainMenu);
	
	//释放文本资源
    //***************************************************************/
    SysMaintainTextRes_Exit(NULL, 0, NULL, 0);
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmSysMaintain的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSysMaintainPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmSysMaintain = (GUIWINDOW *) pWndObj;

    //显示桌面背景图片
    DisplayPicture(pSysMaintainBtnLeftBg);
    //DisplayPicture(pSysMaintainBgTabTitle);
	//显示软件升级内的图片
    //DisplayPicture(pSysMaintainBtnWarn);
	DisplayPicture(pSysMaintainBtnUpdate);

	//显示出厂设置内的图片
	DisplayPicture(pSysMaintainBtnResetOK);
	
	//显示窗体左上角窗体Label
	DisplayLabel(pSysMaintainLblFrmName);
	
	//显示软件升级内的Label
	DisplayLabel(pSysMaintainLblUpdateTitle);
	DisplayLabel(pSysMaintainLblWarn);
	DisplayLabel(pSysMaintainLblUpdate);

	//显示出厂设置内的Label
	DisplayLabel(pSysMaintainLblResetTitle);
	DisplayLabel(pSysMaintainLblResetWarn);
	DisplayLabel(pSysMaintainLblResetOK);	

	//显示菜单控件
	DisplayWndMenu1(pSysMaintainMenu);

	SetPowerEnable(1, 1);	

	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmSysMaintain的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSysMaintainLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);   	
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmSysMaintain的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSysMaintainPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体FrmSysMaintain的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSysMaintainResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    窗体FrmSysMaintain中的初始化文本资源、 释放文本资源函数定义部分		  **
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
static int SysMaintainTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//窗体左上角的文本
	pSysMaintainStrFrmName = TransString("SYSTEMSET_SYSMAINTAIN");

	//系统升级内的文本
	pSysMaintainStrUpdateTitle = TransString("SYSTEMSET_SOFTWAREUPDATE");
	pSysMaintainStrWarn = TransString("UPDATE_ENSUREPOWER");
	pSysMaintainStrUpdate = TransString("SYSTEMSET_UPDATE");

	//恢复出厂设置内的文本
	pSysMaintainStrResetTitle = TransString("SYSTEMSET_FACTORYRESE");
	pSysMaintainStrResetWarn = TransString("SET_RESET_INFO");
	pSysMaintainStrResetOK = TransString("SYSTEMSET_OK");	
	
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
static int SysMaintainTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//释放左上角的文本
	GuiMemFree(pSysMaintainStrFrmName);

	//释放系统升级内的文本
	GuiMemFree(pSysMaintainStrUpdateTitle);
	GuiMemFree(pSysMaintainStrWarn);
	GuiMemFree(pSysMaintainStrUpdate);

	//释放恢复出厂设置内的文本
	GuiMemFree(pSysMaintainStrResetTitle);
	GuiMemFree(pSysMaintainStrResetWarn);
	GuiMemFree(pSysMaintainStrResetOK);		

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
static int SysMaintainWndKey_Down(void *pInArg, int iInLen, 
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
static int SysMaintainWndKey_Up(void *pInArg, int iInLen, 
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
static int SysMaintainBtnUpdate_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
	TouchChange("btn_update_press0.bmp", pSysMaintainBtnUpdate, 
				NULL, pSysMaintainLblUpdate, 1);
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
static int SysMaintainBtnUpdate_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iErr = 0;

    //临时变量定义
	GUIWINDOW *pWnd = NULL;    

	if(iErr == 0)
	{
		TouchChange("btn_update_unpress0.bmp", pSysMaintainBtnUpdate, 
					NULL, pSysMaintainLblUpdate, 0);
		//刷新屏幕缓存
		RefreshScreen(__FILE__, __func__, __LINE__);		
	}

	if(iErr == 0)
	{
		if((CheckPowerOn(BATTERY) == 1) && (CheckBatteryValueOverHalf() == 1))
		{
			/* 调用系统升级窗体 */
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmUpdateInit, FrmUpdateExit, 
								FrmUpdatePaint, FrmUpdateLoop, 
								FrmUpdatePause, FrmUpdateResume,
								NULL);			
			SendWndMsg_WindowExit(pFrmSysMaintain);  
			SendSysMsg_ThreadCreate(pWnd); 				
		}
		/* 电池电量不足或者没插入电池 */
		else
		{
		    DialogInit(120, 90, TransString("Warning"), 
        		   TransString("SYSTEMSET_VALUE_OVER_HALF"),
        		   0, 0, ReCreateWindow, NULL, NULL);
		}
	}
	
    return iErr;
}


/**********************************************************************************
**			    			恢复出厂设置内的按键事件处理函数					 **
***********************************************************************************/

/***
  * 功能：
        恢复出厂设置按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int SysMaintainBtnResetOK_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义

	TouchChange("btn_update_press.bmp", pSysMaintainBtnResetOK, 
				NULL, pSysMaintainLblResetOK, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

	return iReturn;
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
static void ResetOK(void)
{
    unsigned short languageRecover = pCurSystemSet->lCurrentLanguage;               //工厂提出恢复出厂设置不改变语言
	//_g_ucFactoryFlg = 1;
	
	ShowDialogExtraInfo(TransString("DIALOG_FACTORYING"));
	
    ResetAllSettings();
    
    ResetSystemSet((void *)pCurSystemSet);
    pCurSystemSet->lCurrentLanguage = languageRecover;
    SetSettingsData((void *)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);
    
    ShowDialogExtraInfo(TransString("DIALOG_FACTORY_SUCESSS"));
    
	while(1);
}

/***
  * 功能：
        恢复出厂设置按钮up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int SysMaintainBtnResetOK_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	TouchChange("btn_update_unpress.bmp", pSysMaintainBtnResetOK, 
				NULL, pSysMaintainLblResetOK, 0);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);	

	DialogInit(120, 90, TransString("Warning"),
				TransString("DIALOG_FACTORY_ASK"),
				0, 1, ReCreateWindow, ResetOK, NULL);	
		
	return iReturn;
}


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
                    FrmSysMaintainInit, FrmSysMaintainExit, 
                    FrmSysMaintainPaint, FrmSysMaintainLoop, 
			        FrmSysMaintainPause, FrmSysMaintainResume,
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
static void SysMaintainWndMenuCallBack(int selected)
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
			SendWndMsg_WindowExit(pFrmSysMaintain);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;
		case 1://调取时间设置
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
			                    FrmTimeSetInit, FrmTimeSetExit, 
			                    FrmTimeSetPaint, FrmTimeSetLoop, 
						        FrmTimeSetPause, FrmTimeSetResume,
			                    NULL);          
			SendWndMsg_WindowExit(pFrmSysMaintain);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;	
		case 2:	//调取语言设置窗体
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmLanguageSetInit, FrmLanguageSetExit, 
								FrmLanguageSetPaint, FrmLanguageSetLoop, 
								FrmLanguageSetPause, FrmLanguageSetResume,
								NULL);			
			SendWndMsg_WindowExit(pFrmSysMaintain);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;
		case 3:	//调取wifi窗体
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmWiFiInit, FrmWiFiExit, 
								FrmWiFiPaint, FrmWiFiLoop, 
								FrmWiFiPause, FrmWiFiResume,
								NULL);			
			SendWndMsg_WindowExit(pFrmSysMaintain);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;			
		case 4:	//调取软件升级窗体
			break;
		case 5:
            #ifdef MINI2
            pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    							FrmAboutInit, FrmAboutExit, 
    							FrmAboutPaint, FrmAboutLoop, 
    							FrmAboutPause, FrmAboutResume,
    							NULL);			       //pWnd由调度线程释放
    		SendWndMsg_WindowExit(pFrmSysMaintain);	  //发送消息以便退出当前窗体
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
		    SendWndMsg_WindowExit(pFrmSysMaintain);  
		    SendSysMsg_ThreadCreate(pWnd); 
			break;			
		default:
			break;
	}
}
