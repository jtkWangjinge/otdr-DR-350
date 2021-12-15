/*******************************************************************************
* Copyright(c)2012，大豪信息技术(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmskin.c
* 摘    要：  提供皮肤设置功能
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015-06-17
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_frmskinset.h"


/**********************************************************************************
**						为实现窗体frmskinset而需要引用的其他头文件 				 **
***********************************************************************************/
#include "wnd_global.h"
   
#include "app_middle.h"
#include "app_global.h"
#include "app_parameter.h"

#include "wnd_frmmain.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmwifi.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmmenubak.h"
#include "guiphoto.h"
#include "wnd_frmabout.h"
/**********************************************************************************
**								 变量定义				 						 **
***********************************************************************************/
static int iKeyFlg = 0;
extern PSYSTEMSET pCurSystemSet;
static unsigned char __g_ucSkin = 0;

/**********************************************************************************
**							窗体frmskinset中的控件定义部分						 **
***********************************************************************************/
static GUIWINDOW *pFrmSkinSet = NULL;

static GUICHAR *pSkinSetStrFrmName1  = NULL;	 
static GUICHAR *pSkinSetStrFrmName2  = NULL;	

static GUILABEL *pSkinSetLblFrmName1 = NULL;		
static GUILABEL *pSkinSetLblFrmName2 = NULL;		

static GUIPICTURE *pSkinSetBgDesk    = NULL;	//桌面图片背景
static GUIPICTURE *pSkinSetBtnSkinA  = NULL;  //皮肤A样式
static GUIPICTURE *pSkinSetBgLeft    = NULL;  //皮肤A样式
static GUIPICTURE *pSkinSetBgRight   = NULL;  //皮肤B样式
static GUIPICTURE *pSkinSetBtnChange1 = NULL;
static GUIPICTURE *pSkinSetBtnUnchange = NULL;

/********************************右侧菜单控件定义**********************************/
static WNDMENU1 *pSkinSetMenu = NULL;	
/**********************************************************************************
**	    	窗体frmskinset中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/
static int SkinSetTextRes_Init(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);

static int SkinSetTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);

/**********************************************************************************
**			    	窗体frmskinset中的控件事件处理函数定义部分					 **
***********************************************************************************/

/***************************窗体的按键事件处理函数********************************/
static int SkinSetWndKey_Down(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
static int SkinSetWndKey_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/**************************皮肤设置按钮事件处理函数*******************************/
static int SkinSetBtnSkinA_Down(void *pInArg,  int iInLen, 
                             void *pOutArg, int iOutLen);

static int SkinSetBtnSkinA_Up(void *pInArg,  int iInLen, 
                             void *pOutArg, int iOutLen);

/**********************************************************************************
**			    			窗体内的其他函数声明					 		     **
***********************************************************************************/

static int SkinSetBtnChange1_Down(void *pInArg,	int iInLen, 
									void *pOutArg, int iOutLen);
static void SkinSetWndMenuCallBack(int selected);


/***
  * 功能：
        窗体frmskinset的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSkinSetInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;	

    //得到当前窗体对象
    pFrmSkinSet = (GUIWINDOW *) pWndObj;
	unsigned int strMenu[] = {
		1
	};
    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    SkinSetTextRes_Init(NULL, 0, NULL, 0);

	/* 窗体标题的Label */
	pSkinSetLblFrmName1 = CreateLabel(100, 24, 100, 24, pSkinSetStrFrmName1);
    pSkinSetLblFrmName2 = CreateLabel(0, 24, 100, 24, pSkinSetStrFrmName2);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSkinSetLblFrmName1);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSkinSetLblFrmName1);		
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSkinSetLblFrmName2);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSkinSetLblFrmName2);		
	
    pSkinSetBgDesk = CreatePhoto("bg_skinset");
    pSkinSetBtnChange1 = CreatePhoto("otdr_top1");
    pSkinSetBtnUnchange = CreatePhoto("otdr_top2f");
    pSkinSetBgLeft = CreatePicture(82, 180, 239, 182, NULL);
    pSkinSetBgRight = CreatePicture(366, 180, 239, 182, NULL);
	pSkinSetBtnSkinA = CreatePhoto("btn_skina_unselect");
	//菜单栏控件
	pSkinSetMenu = CreateWndMenu1(5, sizeof(strMenu), strMenu, 
						MENU_BACK | MENU_HOME, 0, 0, 40, SkinSetWndMenuCallBack);
	AddWndMenuToComp1(pSkinSetMenu, pFrmSkinSet);	
	LoginWndMenuToMsg1(pSkinSetMenu, pFrmSkinSet);	

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSkinSet, 
                  pFrmSkinSet);    
    //注册桌面上的控件

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSkinSetBtnChange1, 
                  pFrmSkinSet); 
 
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSkinSetBtnSkinA, 
                  pFrmSkinSet);

    pMsg = GetCurrMessage();
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmSkinSet, 
                    SkinSetWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmSkinSet, 
                    SkinSetWndKey_Up, NULL, 0, pMsg);
	
    //注册桌面上控件的消息处理

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSkinSetBtnChange1, 
                    SkinSetBtnChange1_Down, NULL, 0, pMsg);  

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSkinSetBtnSkinA, 
                    SkinSetBtnSkinA_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSkinSetBtnSkinA, 
                    SkinSetBtnSkinA_Up, NULL, 0, pMsg);	

    return iReturn;
}


/***
  * 功能：
        窗体frmskinset的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSkinSetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmSkinSet = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmSkinSet);

    DestroyPicture(&pSkinSetBgDesk);
	DestroyPicture(&pSkinSetBtnChange1);
	DestroyPicture(&pSkinSetBtnUnchange);
	
	DestroyPicture(&pSkinSetBtnSkinA);
	DestroyPicture(&pSkinSetBgLeft);
	DestroyPicture(&pSkinSetBgRight);

	DestroyLabel(&pSkinSetLblFrmName1);
	DestroyLabel(&pSkinSetLblFrmName2);
	//销毁右侧区域的Label
	DestroyWndMenu1(&pSkinSetMenu);	

    //释放文本资源
    //***************************************************************/
    SkinSetTextRes_Exit(NULL, 0, NULL, 0);
	
    return iReturn;
}


/***
  * 功能：
        窗体frmskinset的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSkinSetPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmSkinSet = (GUIWINDOW *) pWndObj;

    DisplayPicture(pSkinSetBgDesk);	
	DisplayPicture(pSkinSetBtnChange1);
	DisplayPicture(pSkinSetBtnUnchange);
	
	DisplayLabel(pSkinSetLblFrmName1);
	DisplayLabel(pSkinSetLblFrmName2);
    
	DisplayPicture(pSkinSetBtnSkinA);

	//显示菜单控件
	DisplayWndMenu1(pSkinSetMenu);

	SetPowerEnable(1, 1);	
	
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
        窗体frmskinset的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSkinSetLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
		

    return iReturn;
}


/***
  * 功能：
        窗体frmskinset的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSkinSetPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmskinset的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSkinSetResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    窗体frmskinset中的初始化文本资源、 释放文本资源函数定义部分		  **
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
static int SkinSetTextRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	pSkinSetStrFrmName1 = TransString("SYSTEMSET_SKINSET");	
	pSkinSetStrFrmName2 = TransString("SYSTEMSET_STANDBY");
    
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
static int SkinSetTextRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	GuiMemFree(pSkinSetStrFrmName1);
	GuiMemFree(pSkinSetStrFrmName2);
    
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
static int SkinSetWndKey_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

	if(iKeyFlg == 0)
	{
		iKeyFlg = 1;
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
static int SkinSetWndKey_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

	if(iKeyFlg == 1)
	{
		iKeyFlg = 0;
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
	}
    return iReturn;
}
/***
  * 功能：
        窗体切换按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/

static int SkinSetBtnChange1_Down(void *pInArg,  int iInLen, 
                                    void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	GUIWINDOW *pWnd = NULL;

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmStandbysetInit, FrmStandbysetExit, 
						FrmStandbysetPaint, FrmStandbysetLoop, 
						FrmStandbysetPause, FrmStandbysetResume,
						NULL);			
	SendWndMsg_WindowExit(pFrmSkinSet);	 
	SendSysMsg_ThreadCreate(pWnd);		

    return iReturn;	
}

/***
  * 功能：
        皮肤A设置按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int SkinSetBtnSkinA_Down(void *pInArg,  int iInLen, 
                                    void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    TouchChange("bg_left_select.bmp", pSkinSetBgLeft, 
				NULL, NULL, 1);
	TouchChange("btn_skina_unselect.bmp", pSkinSetBtnSkinA, 
				NULL, NULL, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}

/***
  * 功能：
        皮肤A设置按钮up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/                               
static int SkinSetBtnSkinA_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	GUIWINDOW *pWnd = NULL;
	TouchChange("bg_left_unselect.bmp", pSkinSetBgLeft, 
				NULL, NULL, 1);
	TouchChange("btn_skina_unselect.bmp", pSkinSetBtnSkinA, 
				NULL, NULL, 1);
    
	RefreshScreen(__FILE__, __func__, __LINE__);	

	if(pCurSystemSet->ucSkinStyle == SKIN_B)
	{
		pCurSystemSet->ucSkinStyle = SKIN_A;
		SetSkin(pCurSystemSet->ucSkinStyle);
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
					FrmSkinSetInit, FrmSkinSetExit, 
					FrmSkinSetPaint, FrmSkinSetLoop, 
					FrmSkinSetPause, FrmSkinSetResume,
					NULL);			
		SendWndMsg_WindowExit(pFrmSkinSet);	 
		SendSysMsg_ThreadCreate(pWnd);		
	}

    return iReturn;
}   

/***
  * 功能：
        设置皮肤
  * 参数：
        1、unsigned char ucSkin:	皮肤索引
  * 返回：
        无
  * 备注：
***/
void SetSkin(unsigned char ucSkin)
{
	if(ucSkin >= SKIN_NUM)
	{	
		return;
	}	
	__g_ucSkin = ucSkin;
}


/***
  * 功能：
        获取当前设置的皮肤
  * 参数：
        无
  * 返回：
        获取当前设置的皮肤
  * 备注：
***/
unsigned char GetSkin(void)
{
	return __g_ucSkin;
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
static void SkinSetWndMenuCallBack(int selected)
{
	GUIWINDOW *pWnd = NULL;
	switch (selected)
	{
	case 0:
		break;
	case 1://调取时间窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmTimeSetInit, FrmTimeSetExit, 
		                    FrmTimeSetPaint, FrmTimeSetLoop, 
					        FrmTimeSetPause, FrmTimeSetResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmSkinSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调取语言设置窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmLanguageSetInit, FrmLanguageSetExit, 
							FrmLanguageSetPaint, FrmLanguageSetLoop, 
							FrmLanguageSetPause, FrmLanguageSetResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmSkinSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 3:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmWiFiInit, FrmWiFiExit, 
								FrmWiFiPaint, FrmWiFiLoop, 
								FrmWiFiPause, FrmWiFiResume,
								NULL);			
		SendWndMsg_WindowExit(pFrmSkinSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 4:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmSysMaintainInit, FrmSysMaintainExit, 
		                    FrmSysMaintainPaint, FrmSysMaintainLoop, 
					        FrmSysMaintainPause, FrmSysMaintainResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmSkinSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;

	case 5:
		break;		
	case BACK_DOWN://退出时间设置
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmMainInit, FrmMainExit, 
	                        FrmMainPaint, FrmMainLoop, 
					        FrmMainPause, FrmMainResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmSkinSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;	
	case HOME_DOWN://退出时间设置
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmMainInit, FrmMainExit, 
	                        FrmMainPaint, FrmMainLoop, 
					        FrmMainPause, FrmMainResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmSkinSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;	
	default:
		break;
	}
}

