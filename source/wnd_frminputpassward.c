/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frminputpassward.c
* 摘    要：  实现主窗体FrmInputPassward的窗体处理操作函数
*
* 当前版本：  v1.0
* 作    者：  wjg
* 完成日期：  21020/10/26
*
*******************************************************************************/

#include "wnd_frmmain.h"
#include "wnd_frminputpassward.h"
#include "wnd_frmconfigureinfo.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmcalendar.h"

/*******************************************************************************
**				   	为实现窗体FrmInputPassward而需要引用的其他头文件		**
*******************************************************************************/
#include "app_global.h"

#include "guiphoto.h"

#include "wnd_global.h"
#include "wnd_frmfactory.h"
#include "wnd_frmime.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmdialog.h"

#define FACTORY_PASSWORD    "123456"//"Mzy5x6Myc2#2"
#define TIME_PASSWORD		"111111"//"Mzy5x6Myc2#2"
#define SALE_PASSWORD       "AMPsTPtn-q"

FactoryRank factoryRank = SALE;
/*******************************************************************************
**							窗体FrmInputPassward中内部使用的变量			**
*******************************************************************************/
static int RightpasswardFlag = 0;
static char cInputBuff[100]  = {0};
/*******************************************************************************
**							窗体FrmInputPassward中的控件定义部分			**
*******************************************************************************/
static GUIWINDOW *pInputPassward = NULL;

/*******************************************************************************/
static GUIPICTURE *pInputIconbutton = NULL;					// Upbutton
static GUIPICTURE *pInputPasswardIconInput = NULL;			//输入检测
static GUIPICTURE *pInputPasswardIconBackGround = NULL;		// 背景

static GUILABEL *pInputPasswardLblMark = NULL;				// 提示用户输入
static GUILABEL *pInputPasswardLblInfo = NULL;				// 密码信息
static GUILABEL *pInputPasswardLblReminder = NULL;			// 密码错误
static GUILABEL *pInputPasswardLblOk = NULL;

static GUICHAR *pInputPasswardStrMark = NULL;				// 提示用户输入
static GUICHAR *pInputPasswardStrInfo = NULL;				// 密码信息
static GUICHAR *pInputPasswardStrReminder = NULL;			// 密码错误
static GUICHAR *pInputPasswardStrOK = NULL;
/*******************************************************************************
**	   	窗体FrmInputPassward中的初始化文本资源、 释放文本资源函数定义部分	  **
*******************************************************************************/
static int MainTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int MainTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void ReCreateWindow(GUIWINDOW **pWnd);
static void RePskInputOk(void);

/***************************窗体的按键事件处理函数*****************************/
static int MainWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int MainWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

/**************************四个功能控件的事件处理函数**************************/
    
static int InputPasswardIconInput_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int InputIconButtonPressed_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int InputIconButtonPressed_UP(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

/*******************************************************************************
**			    	窗体FrmInputPassward中的错误事件处理函数定义部分		 **
*******************************************************************************/
static int MainErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/*  侧边菜单控件回调函数 */
static void InputPasswardMenuCallBack(int iOption);

/***
  * 功能：
        窗体FrmMain的初始化函数，创建控件和注册函数
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InputPasswardInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pInputPassward = (GUIWINDOW *) pWndObj;
	
   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
	MainTextRes_Init(NULL, 0, NULL, 0);

	/***************************************************************************
	**							窗体FrmInputPassward中的创建背景			 **
	***************************************************************************/
	pInputPasswardIconBackGround = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
												 BmpFileDirectory "bg_inputPassWard.bmp");

	/***************************************************************************
	**							窗体FrmInputPassward中的创建功能选择控件	**
	***************************************************************************/
    pInputPasswardLblMark = CreateLabel(180, 189, 200, 16, pInputPasswardStrMark);
    pInputPasswardLblInfo = CreateLabel(228, 229+7, 150, 16, pInputPasswardStrInfo);
    pInputPasswardLblReminder = CreateLabel(220, 189+20, 150, 16, pInputPasswardStrReminder);
    pInputPasswardLblOk = CreateLabel(340+10, 273+7, 50, 16, pInputPasswardStrOK);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pInputPasswardLblMark);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pInputPasswardLblInfo);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pInputPasswardLblReminder);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pInputPasswardLblOk);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pInputPasswardLblMark);
    SetLabelFont(getGlobalFnt(EN_FONT_GREEN), pInputPasswardLblInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pInputPasswardLblOk);
    SetLabelFont(getGlobalFnt(EN_FONT_RED), pInputPasswardLblReminder);
	
	/***************************************************************************
	**							窗体FrmInputPassward中的添加控件			**
	***************************************************************************/
    pInputPasswardIconInput = CreatePicture(228, 229, 150, 30, BmpFileDirectory"bg_input_text.bmp");
    pInputIconbutton = CreatePicture(340, 273, 70, 30, BmpFileDirectory"btn_dialog_unpress.bmp");
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pInputPassward,
				  pInputPassward);

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pInputPasswardIconInput,
                  pInputPassward);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pInputIconbutton,
                  pInputPassward);
//     AddWndMenuToComp1(pInputPasswardMenu, pInputPassward);
	
	/***************************************************************************
	**						窗体FrmInputPassward中的添加控件响应函数		  **
	***************************************************************************/
	pMsg = GetCurrMessage();
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pInputPasswardIconInput, 
                    InputPasswardIconInput_Down, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pInputIconbutton, 
                    InputIconButtonPressed_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pInputIconbutton, 
                    InputIconButtonPressed_UP, NULL, 0, pMsg);
//     LoginWndMenuToMsg1(pInputPasswardMenu, pInputPassward);
	//注册错误消息处理函数
	LoginMessageReg(GUIMESSAGE_ERR_PROC, pInputPassward, 
                    MainErrProc_Func, NULL, 0, pMsg);

	//注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pInputPassward, 
	                MainWndKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pInputPassward, 
	                MainWndKey_Up, NULL, 0, pMsg);

	return iReturn;
}


/***
  * 功能：
        窗体FrmMain的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InputPasswardExit(void *pWndObj)
{

	//错误标志、返回值定义
	int iReturn = 0;

	GUIMESSAGE *pMsg = NULL;
	//得到当前窗体对象
	pInputPassward = (GUIWINDOW *) pWndObj;
	/***************************************************************************
	**							  清空消息队列中的消息注册项     			  **
	***************************************************************************/
	pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);

	/***************************************************************************
	**						      清空指定窗体的窗体控件队列      			  **
	***************************************************************************/
	ClearWindowComp(pInputPassward);

	/***************************************************************************
	**						          销毁窗体背景控件         	    		  **
	***************************************************************************/	

	/***************************************************************************
	**						         销毁窗体功能模块控件      	    		  **
	***************************************************************************/
    DestroyPicture(&pInputPasswardIconBackGround);
    DestroyPicture(&pInputPasswardIconInput);
    DestroyPicture(&pInputIconbutton);
    DestroyLabel(&pInputPasswardLblMark);
    DestroyLabel(&pInputPasswardLblInfo);
    DestroyLabel(&pInputPasswardLblOk);

	/***************************************************************************
	**						             释放文本资源       	     		  **
	***************************************************************************/
	MainTextRes_Exit(NULL, 0, NULL, 0);

	//清空时间信息
    memset(cInputBuff, 0, sizeof(cInputBuff));
    RightpasswardFlag = 0;
    
	return iReturn;
}


/***
  * 功能：
        窗体FrmMain的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InputPasswardPaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//得到当前窗体对象
	pInputPassward= (GUIWINDOW *) pWndObj;

	/***************************************************************************
	**						          显示窗体背景图片          			  **
	***************************************************************************/	

	/***************************************************************************
	**					           显示窗体功能模块控件        		    	  **
	***************************************************************************/
    DisplayPicture(pInputPasswardIconBackGround);
//     DisplayWndMenu1(pInputPasswardMenu);
    DisplayPicture(pInputIconbutton);
    DisplayPicture(pInputPasswardIconInput);
    if (NULL != pInputPasswardLblInfo)
    {
        DisplayLabel(pInputPasswardLblReminder);
    }
    DisplayLabel(pInputPasswardLblMark);
    DisplayLabel(pInputPasswardLblInfo);
    DisplayLabel(pInputPasswardLblOk);
    
	//刷新屏幕缓存
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}


/***
  * 功能：
        窗体FrmMain的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InputPaawardLoop(void *pWndObj)
{
    
	//错误标志、返回值定义
	int iReturn = 0;
    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);

	return iReturn;
}


/***
  * 功能：
        窗体FrmMain的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int InputPasswardPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	return iReturn;
}


/***
  * 功能：
        窗体FrmMain的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注:输入完密码时，初始化label为密码(密码的保存和获取???)
***/
int InputPasswardResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	return iReturn;
}

//初始化文本资源
static int MainTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	/***************************************************************************
	**					           初始化功能模块的文本        		    	  **
	***************************************************************************/
	pInputPasswardStrOK = GetCurrLanguageText(SYSTEM_LBL_CONFIRM);
    pInputPasswardStrMark = TransString("Please enter password：");
    pInputPasswardStrInfo = NULL;
    pInputPasswardStrReminder = NULL;
    
	if (RightpasswardFlag)
    {
        pInputPasswardStrInfo = TransString(cInputBuff);
        pInputPasswardStrReminder = NULL;
    }   
	else if(strlen(cInputBuff))
    {
        pInputPasswardStrReminder = TransString("Password Error!");
    }
    return iReturn;
}


//释放文本资源
static int MainTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	
	/***************************************************************************
	**					            释放功能模块的文本          	    	  **
	***************************************************************************/
	GuiMemFree(pInputPasswardStrMark);
    GuiMemFree(pInputPasswardStrInfo);
    GuiMemFree(pInputPasswardStrReminder);
    GuiMemFree(pInputPasswardStrOK);
    return iReturn;
}

static int MainErrProc_Func(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//根据实际情况，进行错误处理

	return iReturn;
}

static int InputPasswardIconInput_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;	
	RefreshScreen(__FILE__, __func__, __LINE__);
	//调度frmiolm,开启vk。
	IMEInit("", 60, 0, ReCreateWindow, RePskInputOk, NULL);
	return iReturn;
}

static int InputIconButtonPressed_Down(void * pInArg, int iInLen, 
                               void * pOutArg, int iOutLen)
{
    int iReturn = 0;
    TouchChange("btn_dialog_press.bmp", pInputIconbutton, NULL, NULL, 0);   
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pInputPasswardLblOk);
    DisplayLabel(pInputPasswardLblOk);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

static int InputIconButtonPressed_UP(void * pInArg, int iInLen, 
                               void * pOutArg, int iOutLen)
{
    int iReturn = 0;
    TouchChange("btn_dialog_unpress.bmp", pInputIconbutton, NULL, NULL, 0);
    
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pInputPasswardLblOk);
    DisplayLabel(pInputPasswardLblOk);
	RefreshScreen(__FILE__, __func__, __LINE__);
        
	//调度frmiolm
    if(RightpasswardFlag == 1)
    {
        GUIWINDOW* pWnd = NULL;        
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmConfigureInfoInit, FrmConfigureInfoExit,
							FrmConfigureInfoPaint, FrmConfigureInfoLoop,
							FrmConfigureInfoPause, FrmConfigureInfoResume,
                        NULL);
        SendWndMsg_WindowExit(pInputPassward);	
        SendSysMsg_ThreadCreate(pWnd);
    }
	else if (RightpasswardFlag == 2)
	{
		CalendarInit(120, 90, CALEBDAR_DATE, ReCreateWindow);
	}

    return iReturn;
    
}

//按键按下处理函数
static int MainWndKey_Down(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	LOG(LOG_INFO, "MainWndKey_Down pInArg = %d\n", (unsigned int)pInArg);
	unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	{
        switch (uiValue)
	    {
		case KEYCODE_ESC:
			break;
		case KEYCODE_HOME:
			break;
		default:
			break;
    	}
	}
    
	return iReturn;
}

//按键弹起处理函数
static int MainWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	LOG(LOG_INFO, "MainWndKey_Up pInArg = %d\n", (unsigned int)pInArg);
	unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
	{
		static int iKeyFlag = 0;

		switch (uiValue)
		{
		case KEYCODE_UP:
			iKeyFlag = iKeyFlag ? 0 : 1;
			break;
		case KEYCODE_DOWN:
			iKeyFlag = iKeyFlag ? 0 : 1;
			break;
		case KEYCODE_ENTER:
			if (!iKeyFlag)
			{
				InputPasswardIconInput_Down(NULL, 0, NULL, 0);
			}
			else
			{
				InputIconButtonPressed_Down(NULL, 0, NULL, 0);
				InputIconButtonPressed_UP(NULL, 0, NULL, 0);
				iKeyFlag = 0;
			}
			break;
		case KEYCODE_ESC:
			InputPasswardMenuCallBack(BACK_DOWN);
			break;
		default:
			break;
    	}
    }

	return iReturn;
}

static void ReCreateWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  InputPasswardInit,InputPasswardExit,
					  InputPasswardPaint,InputPaawardLoop,
					  InputPasswardPause,InputPasswardResume,
					  NULL);
}

static void RePskInputOk(void)
{
	char buff[512] = {0};
	GetIMEInputBuff(buff);

	strncpy(cInputBuff, buff,strlen(buff));
    RightpasswardFlag = 1;
    
	if (!strcmp(cInputBuff, FACTORY_PASSWORD))
    {
        factoryRank = FACTORY;
    }   
    else if(!strcmp(cInputBuff, SALE_PASSWORD))
    {
        factoryRank = SALE;
    }
    else if (!strcmp(cInputBuff, TIME_PASSWORD))
    {
		RightpasswardFlag = 2;
    }
	else
    {
        RightpasswardFlag = 0;
    }
}



static void InputPasswardMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	
	switch (iOption)
	{
	case 0:    
	case BACK_DOWN:
        	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmSystemSettingInit, FrmSystemSettingExit,
						FrmSystemSettingPaint, FrmSystemSettingLoop,
						FrmSystemSettingPause, FrmSystemSettingResume,
	                    NULL);          
	        SendWndMsg_WindowExit(pInputPassward);  
	        SendSysMsg_ThreadCreate(pWnd); 
	 	    break;        
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmMainInit, FrmMainExit, 
	                        FrmMainPaint, FrmMainLoop, 
					        FrmMainPause, FrmMainResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pInputPassward);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;	 
	 	break;
	default:
		break;
	}
}

