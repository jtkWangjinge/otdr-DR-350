/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifi.c
* 摘    要：  实现wifi的连接
*             
*
* 当前版本：  v1.0.0
* 作    者：  xiazhizhu
* 完成日期：  2016-4-7
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "wnd_frmwifi.h"

/*******************************************************************************
**						为实现窗体frmwifi而需要引用的其他头文件 			  **
*******************************************************************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmabout.h"
#include "wnd_stack.h"
#include "app_getsetparameter.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmmain.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmime.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmwifipsk.h"
#include "wnd_frmwifinopsk.h"
#include "wnd_frmwifiwidget.h"
#include "app_frmotdrmessage.h"
#include "app_frmsendfile.h"
#include "appwifi.h"
#include "app_systemsettings.h"

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "guiphoto.h"
/*******************************************************************************
**									宏定义 				 					  **
*******************************************************************************/	
#define DIS_WIFI_ITEM 		20							//屏幕显示的WiFi信息条目数


/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/	
NETWORK_INFO_SINGLE *pNetInfo_List = NULL;				//wifi列表
static int AP_count=0;									//wifi热点数
int WifiSwitch = 0;										//wifi开启关闭开关0->关闭、1->打开
GUIMUTEX NetInfo_List_Mutex;							//wifi列表互斥锁


static int iHaveData = 0;				 				//wifi数据准备完成标志(已完成排序)
extern PWIFISET pCurWiFiSet;			 				//系统设置wifi信息(存入EEPROM)

extern NETWORK_INFO_SINGLE WifiConnect_nopsk;
extern char cTmpInputBuff_nopsk[WIFI_PSK_LEN];

/*******************************************************************************
**							窗体frmwifi中的控件定义部分					  	  **
*******************************************************************************/
static GUIWINDOW *pFrmWiFi                   = NULL;

/**********************************背景控件定义********************************/ 
static GUIPICTURE *pWiFiBtnLeftBg            = NULL;

/******************************窗体标题栏控件定义******************************/
static GUICHAR *pWiFiStrFrmName              = NULL;	//窗体左上角标题栏文本
static GUILABEL *pWiFiLblFrmName             = NULL;	//左上角窗体名字的Label

/***************************打开关闭wifi内的控件定义***************************/
static GUICHAR *pWiFiStrLEnable              = NULL;	//显示 开启WLAN
static GUILABEL *pWiFiLblLEnable             = NULL;	//打开 WLAN
static GUIPICTURE *pWiFiBtnREnableOn	     = NULL;	//on_button 
static GUIPICTURE *pWiFiBtnREnableOff        = NULL; 	//off_button

/****************************已连接wifi内的控件定义****************************/
static GUIPEN *pWifiPen						 = NULL;	//绘制块(刷新无密码连接提示)

static GUICHAR *pWiFiStrConnectedCap         = NULL;	//"连接的WLAN"
static GUICHAR *pWifiStrTopInfo              = NULL;	//顶部提示信息str
static GUICHAR *pWifiStrIP     	    		 = NULL;	//"IP"
static GUICHAR *pWifiStrOn                   = NULL;
static GUICHAR *pWifiStrOff                  = NULL;

static GUILABEL *pWiFiLblConnectedCap        = NULL;    //"连接的WLAN"LBl
static GUILABEL *pWifiLblTopInfo             = NULL;	//顶部提示信息LBl
static GUILABEL *pWifiLblIP     			 = NULL;	//"IP"
static GUILABEL *pWifiLblOn                  = NULL;
static GUILABEL *pWifiLblOff                 = NULL;

static GUIPICTURE *pWiFiBtnConectedBg		 = NULL;	// 已连接WIFI显示区域的背景图片
/* 未选中状态时wifi强度图标 */
static char *pLevelBmp[] = 
{
	BmpFileDirectory"ico_wifi_empty_unpress1.bmp", 
	BmpFileDirectory"ico_wifi_quar_unpress1.bmp", 
	BmpFileDirectory"ico_wifi_half_unpress1.bmp", 
	BmpFileDirectory"ico_wifi_three_quars_unpress1.bmp", 
	BmpFileDirectory"ico_wifi_full_unpress1.bmp"
};

/***************************扫描到的wifi内的控件定义***************************/
static WIFIWIDGET *pWifiWidget[DIS_WIFI_ITEM];			//显示扫描到的WIFI信息(含ssid/level/encrypt/singnal)

/*****************************右侧菜单栏控件定义*******************************/
static WNDMENU1 *pWiFiMenu                  = NULL;		// menu控件


/*******************************************************************************
**	    	窗体frmwifi中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/
static int WiFiTextRes_Init(void *pInArg, int iInLen, 
							void *pOutArg, int iOutLen);
static int WiFiTextRes_Exit(void *pInArg, int iInLen, 
							void *pOutArg, int iOutLen);

/*******************************************************************************
**			    	窗体frmwifi中的控件事件处理函数定义部分				  	  **
*******************************************************************************/

/************************打开/关闭wifi按钮事件处理函数*************************/
static int WiFiBtnEnableOn_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int WiFiBtnEnableOff_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);

static int WiFiBtnEnableOn_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);
static int WiFiBtnEnableOff_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen);


/**************************连接wifi按钮事件处理函数****************************/

static int WiFiBtnWidget_Down(void *pInArg,   int iInLen, 
                            void *pOutArg, int iOutLen);
static int WiFiBtnWidget_Up(void *pInArg,	int iInLen, 
						  void *pOutArg, int iOutLen);


/**************************菜单栏控件的事件处理函数****************************/
static int WiFiBtnBack_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/
static void WiFiWndMenuCallBack(int selected);
void RefreshTopInfo(GUICHAR *Info, GUICHAR *IP);
static void TopInfoTips();
extern int DestroyWifiWidget(WIFIWIDGET **pWifiObj);
int WifiStartInit(void);										//开机wifi信息恢复函数



/***
  * 功能：
        设置合理的wifi信息显示列表
  * 参数：
        1.int index:    接收扫描到的wifi热点信息数量
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int SetWifiList(int index)
{
	int i;
	int iReturn = 0;
	if(index > DIS_WIFI_ITEM)
	{
		iReturn = 1;
		return iReturn;
	}
	else
	{
		for(i = 0;i < DIS_WIFI_ITEM; i++)
			{
				if(i < 10)
				{
					pWifiWidget[i] = CreateWifiWidget(32, 190+(i*25));
				}
				else
				{
					pWifiWidget[i] = CreateWifiWidget(340, 190+((i-10)*25));
				}
				if(i % 2 == 1)
                {
                    SetPictureBitmap(BmpFileDirectory"btn_wifi_ssid_unpress1.bmp", 
                        pWifiWidget[i]->pBg);
                }            
				SetWifiwidgeEnable(0,pWifiWidget[i]);
			}
	}
    
	return iReturn;
}

int Auto_GetIP_callback(char getip_flag)
{
	if(iHaveData) iHaveData=0;
	
	sort_wifilist();
	
	MutexLock(&NetInfo_List_Mutex);
	if(pNetInfo_List)
	{
		GuiMemFree(pNetInfo_List);
	}
	pNetInfo_List = getNetworkInfo();
	
	MutexUnlock(&NetInfo_List_Mutex);
	
	if(pNetInfo_List != NULL)
	{
		iHaveData = 1;
	}

	return 0;
}
/**
*扫描热点后的回调函数
*
*参数:扫描到的热点数量
*
**/
int wifi_ScanCallback(char AP_Count)
{
	AP_count = AP_Count;
	
	if(iHaveData) iHaveData=0;

	if(AP_count != 0)
	{	
		MutexLock(&NetInfo_List_Mutex);
		if(pNetInfo_List)
		{
			GuiMemFree(pNetInfo_List);
		}
		pNetInfo_List = getNetworkInfo();
		
		MutexUnlock(&NetInfo_List_Mutex);

	}
	
	if(getWifiEnable())
	{
		iHaveData = 1;
	}

	if(0 == getDhcpready() && 0 == getConnectready() && AP_count != 0)
	{
		wifi_Auto_Connect(NULL,Auto_GetIP_callback);
	}
	
	return 0;
}

/***
  * 功能：
        窗体FrmWiFi的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmWiFiInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
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
    pFrmWiFi = (GUIWINDOW *) pWndObj;
   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    WiFiTextRes_Init(NULL, 0, NULL, 0);
	
	/* 桌面背景图片 */
    pWiFiBtnLeftBg = CreatePhoto("bg_wifi");
	/* 窗体标题Label */
    pWiFiLblFrmName = CreateLabel(0, 24, 100, 24,
								      pWiFiStrFrmName);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pWiFiLblFrmName);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pWiFiLblFrmName);	

	/* 打开关闭WiFi区域控件定义 */
	pWiFiLblLEnable   = CreateLabel(85, 90, 300, 24, pWiFiStrLEnable);	
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pWiFiLblLEnable);
	
    pWiFiBtnREnableOn = CreatePhoto("btn_wifi_on_unpress");
    pWiFiBtnREnableOff = CreatePhoto("btn_wifi_off_press");
 
    pWifiLblOn = CreateLabel(423, 90, 100, 24, pWifiStrOn);
    pWifiLblOff = CreateLabel(545, 90, 100, 24, pWifiStrOff);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pWifiLblOn);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pWifiLblOff);
    SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblOn);
    SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblOff);
	/*"ip"*/
	pWifiLblIP = CreateLabel(434, 160, 200, 24, pWifiStrIP);
	SetLabelAlign(GUILABEL_ALIGN_LEFT,pWifiLblIP);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pWifiLblIP);
	
	/* WiFi顶部信息提示框*/
	pWifiLblTopInfo = CreateLabel(163,160,496,30,pWifiStrTopInfo);
	SetLabelAlign(GUILABEL_ALIGN_LEFT,pWifiLblTopInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pWifiLblTopInfo);

	/* 已连接WiFi区域控件定义 */
	pWiFiLblConnectedCap = CreateLabel(42, 160, 100, 24, pWiFiStrConnectedCap);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pWiFiLblConnectedCap);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWiFiLblConnectedCap);
    pWiFiBtnConectedBg	= CreatePicture(32, 198, 310, 25, BmpFileDirectory"btn_wifi_ssid_unpress.bmp"); 

	/* 创建wifi列表 */
	SetWifiList(DIS_WIFI_ITEM);

	/*设置wifiwidget控件的数据信息并将控件添加到GUI窗体控件队列中*/
	i = 0;
	for(i=0;i<DIS_WIFI_ITEM;i++)
	{
		SetWifiWidgetFont(pWifiWidget[i], getGlobalFnt(EN_FONT_BLACK));
	}

	//侧边栏menu
	#ifdef MINI2
    pWiFiMenu = CreateWndMenu1(6, sizeof(strMenu), strMenu, 
								MENU_BACK | MENU_HOME, 3, 0, 40, WiFiWndMenuCallBack);
    #else
	pWiFiMenu = CreateWndMenu1(5, sizeof(strMenu), strMenu, 
								MENU_BACK | MENU_HOME, 3, 0, 40, WiFiWndMenuCallBack);	
    #endif
	//添加消息到控件队列
	AddWndMenuToComp1(pWiFiMenu, pFrmWiFi);
	//注册菜单控件的消息处理函数
	LoginWndMenuToMsg1(pWiFiMenu, pFrmWiFi);	
	
	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmWiFi, 
                  pFrmWiFi);
	
	//注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pWiFiBtnREnableOn, 
                  pFrmWiFi);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pWiFiBtnREnableOff, 
                  pFrmWiFi);	
	
	for(i=0;i<DIS_WIFI_ITEM;i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(WIFIWIDGET), pWifiWidget[i], pFrmWiFi);

	}

	 //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pWiFiBtnREnableOn, 
                    WiFiBtnEnableOn_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pWiFiBtnREnableOff, 
                    WiFiBtnEnableOff_Down, NULL, 0, pMsg);	

    LoginMessageReg(GUIMESSAGE_TCH_UP, pWiFiBtnREnableOn, 
                    WiFiBtnEnableOn_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pWiFiBtnREnableOff, 
                    WiFiBtnEnableOff_Up, NULL, 0, pMsg);	

	//wifi列表触控注册
	for(i=0; i<DIS_WIFI_ITEM; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pWifiWidget[i], 
						WiFiBtnWidget_Down, NULL, i, pMsg);	
		
		LoginMessageReg(GUIMESSAGE_TCH_UP, pWifiWidget[i], 
						WiFiBtnWidget_Up, NULL, i, pMsg);
	}

    return iReturn;
}


/***
  * 功能：
        窗体FrmWiFi的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmWiFiExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmWiFi = (GUIWINDOW *) pWndObj;

	//off 时重置了readyWifiPakge.iConnected 的值，在正常退出前需要重新获取一下当前的连接状态

	//清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    SetSettingsData((void*)pCurWiFiSet, sizeof(WIFISET), WIFI_SET);
    SaveSettings(WIFI_SET);


    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmWiFi);

    //销毁窗体控件
    //***************************************************************/
    //销毁桌面背景图片
    DestroyPicture(&pWiFiBtnLeftBg);
    //销毁桌面上的控件
    DestroyPicture(&pWiFiBtnREnableOn);
	DestroyPicture(&pWiFiBtnREnableOff);
	DestroyPicture(&pWiFiBtnConectedBg);
 
	//销毁窗体左上角窗体Label
	DestroyLabel(&pWiFiLblFrmName);
 	
	//销毁桌面上的Label
	DestroyLabel(&pWiFiLblLEnable);
	DestroyLabel(&pWiFiLblConnectedCap);
 	
	//销毁顶部WiFi提示:
	DestroyLabel(&pWifiLblTopInfo);
	//销毁无密码连接提示
	DestroyLabel(&pWifiLblIP);
    DestroyLabel(&pWifiLblOn);
    DestroyLabel(&pWifiLblOff);
	 
	/* 销毁右侧菜单栏 */
	DestroyWndMenu1(&pWiFiMenu);

	for(i=0;i<DIS_WIFI_ITEM;i++)
	{
		DestroyWifiWidget(&pWifiWidget[i]);
	}
	
	//释放文本资源
    //***************************************************************/
    WiFiTextRes_Exit(NULL, 0, NULL, 0);
 
    return iReturn;
}


/***
  * 功能：
        窗体FrmWiFi的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmWiFiPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
    //得到当前窗体对象
    pFrmWiFi = (GUIWINDOW *) pWndObj;

	//显示桌面背景图片
	DisplayPicture(pWiFiBtnLeftBg);
	
	//显示窗体左上角窗体Label
	DisplayLabel(pWiFiLblFrmName);
	DisplayLabel(pWiFiLblLEnable);
	//DisplayLabel(pWiFiLblOnOffInfo);

	DisplayPicture(pWiFiBtnREnableOn);
	DisplayPicture(pWiFiBtnREnableOff); 
	DisplayLabel(pWifiLblOn);
    DisplayLabel(pWifiLblOff);
	DisplayLabel(pWiFiLblConnectedCap);
 	for(i = 0; i < DIS_WIFI_ITEM; i++)
	{		
		DisplayPicture(pWifiWidget[i]->pBg);
		SetWifiwidgeEnable(0,pWifiWidget[i]);
	}

	/* 更新右侧菜单栏 */
	DisplayWndMenu1(pWiFiMenu);
	
	if(1 == WifiSwitch)
	{
		
		SetPictureBitmap(BmpFileDirectory"btn_wifi_on_press.bmp",
						pWiFiBtnREnableOn);
		SetPictureBitmap(BmpFileDirectory"btn_wifi_off_unpress.bmp",
						pWiFiBtnREnableOff);
		DisplayPicture(pWiFiBtnREnableOn);
		DisplayPicture(pWiFiBtnREnableOff);
        DisplayLabel(pWifiLblOn);
        DisplayLabel(pWifiLblOff);
		SetPictureEnable(0, pWiFiBtnREnableOn);
		if(getListready())
		{
			sort_wifilist();
			
			MutexLock(&NetInfo_List_Mutex);
			if(pNetInfo_List)
			{
				GuiMemFree(pNetInfo_List);
			}
			pNetInfo_List = getNetworkInfo();
			
			MutexUnlock(&NetInfo_List_Mutex);


			if(pNetInfo_List != NULL)
			{
				iHaveData = 1;
			}
		}
		setWifiEnable(1);
	}
	else
	{
		SetPictureEnable(0, pWiFiBtnREnableOff);
	}
	
	//显示顶部信息
	DisplayLabel(pWifiLblTopInfo);	
	DisplayLabel(pWifiLblIP);

	SetPowerEnable(1, 1);

	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
        窗体FrmWiFi的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmWiFiLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	static int scanTime = 0;
	static int connectTime = 0;
 		
	if( 1 == WifiSwitch && getWifiEnable())								//on、off开关是否开启
	{
		if( iHaveData==1 ) 
		{
 			//重新设置数据并显示
			iHaveData = 0;
						
			scanTime = 0;
			connectTime = 0;
			MutexLock(&NetInfo_List_Mutex);

			for(i = 0; i < DIS_WIFI_ITEM; i++)
			{	
				if(i < AP_count)
				{
				    
					SetWifiWidgetData(pWifiWidget[i], pNetInfo_List[i].ssid,
                        pNetInfo_List[i].flags, calcsignallevel(pNetInfo_List[i].signallevel, 5));
                    if(i % 2 == 1)
                    {
                        SetPictureBitmap(pLevelBmp[calcsignallevel(pNetInfo_List[i].signallevel, 5)], 
                            pWifiWidget[i]->pLevel);
                    }
					DisplayWifiWidget(pWifiWidget[i]);
				}
				else
				{		
					DisplayPicture(pWifiWidget[i]->pBg);
					SetWifiwidgeEnable(0,pWifiWidget[i]);
				}
 			}
			MutexUnlock(&NetInfo_List_Mutex);

			if(getDhcpready())
			{
				SetWifiWidgetFont(pWifiWidget[0], getGlobalFnt(EN_FONT_YELLOW));//设置首位ssid的字体为红色
				DisplayWifiWidget(pWifiWidget[0]);
				SetWifiwidgeEnable(0, pWifiWidget[0]);
			}
			else
			{
				SetWifiWidgetFont(pWifiWidget[0], getGlobalFnt(EN_FONT_BLACK));//设置首位ssid的字体为黑色
				DisplayWifiWidget(pWifiWidget[0]);
			}
			
			TopInfoTips();										//顶部信息
			//wifi是否为连接状态
			if(getWifiEnable())
			{
				RefreshScreen(__FILE__, __func__, __LINE__);
			}
		}	
		else
		{
			//if(!AP_count)
			if(0 == getDhcpready())
			{
				if(0 == getListready())
				{
					scanTime++;
					if(scanTime == 2)
					{
						RefreshTopInfo(TransString("WIFI_SCAN"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(scanTime == 30)
					{
						RefreshTopInfo(TransString("WIFI_SCAN1"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(scanTime == 60)
					{
						RefreshTopInfo(TransString("WIFI_SCAN2"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(scanTime == 90)
					{
						RefreshTopInfo(TransString("WIFI_SCAN3"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(scanTime == 120)
					{
						scanTime = 1;
					}
				}
				else if(1 == getConnectready())
				{
					connectTime++;
					if(connectTime == 2)
					{
						RefreshTopInfo(TransString("WIFI_CONNECT"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(connectTime == 30)
					{
						RefreshTopInfo(TransString("WIFI_CONNECT1"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(connectTime == 60)
					{
						RefreshTopInfo(TransString("WIFI_CONNECT2"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(connectTime == 90)
					{
						RefreshTopInfo(TransString("WIFI_CONNECT3"),TransString(" "));
						RefreshScreen(__FILE__, __func__, __LINE__);
					}
					else if(connectTime == 120)
					{
						connectTime = 1;
					}
					
				}
				else
				{
					if(scanTime || connectTime)
					{
						scanTime = 0;
						connectTime = 0;
						TopInfoTips();										//顶部信息
						if(getWifiEnable())
						{
							RefreshScreen(__FILE__, __func__, __LINE__);
						}
					}
				}
			}
		}
	}
	
	MsecSleep(10);
	
    return iReturn;
}


/***
  * 功能：
        窗体FrmWiFi的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmWiFiPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体FrmWiFi的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmWiFiResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/*******************************************************************************
**	    	窗体frmwifi中的初始化文本资源、 释放文本资源函数定义部分		  **
*******************************************************************************/
/***
  * 功能：
		初始化文本资源
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//初始化wifi
	if(1 == pCurWiFiSet->iWiFiOpenSta)//系统设置的wifi信息
	{
		WifiSwitch = 1;
	}

	//窗体左上角的文本
	pWiFiStrFrmName = TransString("WLAN");

	//桌面上的文本
	pWiFiStrLEnable        = TransString("WIFI_STARTWLAN");
	pWiFiStrConnectedCap   = TransString("WIFI_CONNECTED_CAP");
    pWifiStrOn             = TransString("ON");
    pWifiStrOff            = TransString("OFF");
	//顶部提示信息tips
	if(1 == WifiSwitch)
	{	
		
		if( 1 == getDhcpready())
		{			
			char hostIP[128] = {0};
		  	char IP[32];
			NETWORK_INFO_SINGLE ConnectNetInfo = getConnectNetInfo();
			pWifiStrTopInfo = TransString(ConnectNetInfo.ssid);
			
			strcpy(hostIP,getIP());
			sprintf(IP,"IP:%s",hostIP);
			pWifiStrIP	 = TransString(IP);
			
		}
		else
		{
			pWifiStrTopInfo = TransString("WIFI_DISCONNECT");
			pWifiStrIP	 = TransString(" ");
		}
	}
	else
	{
		pWifiStrTopInfo = TransString("WIFI_CLOSE");
		pWifiStrIP	 = TransString(" ");
	}

	return iReturn;
}


/***
  * 功能：
		释放文本资源
  * 参数：
		1.void *pInArg	:	指向输入参数的指针
		2.int iInLen	:	输入参数的长度
		3.void *pOutArg :	指向传出参数的指针
		4.int iOutLen	:	输出参数的长度
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	//int i = 0;
	
	//释放左上角的文本
	GuiMemFree(pWiFiStrFrmName);

    //释放桌面上的文本
    GuiMemFree(pWiFiStrLEnable);
	GuiMemFree(pWiFiStrConnectedCap);
 	GuiMemFree(pWifiStrTopInfo);			//顶部信息
	GuiMemFree(pWifiStrIP);		//连接中信息
	GuiMemFree(pWifiStrOn);
    GuiMemFree(pWifiStrOff);
    return iReturn;
}

/*******************************************************************************
**			    		打开/关闭wifi按钮事件处理函数				 		  **
*******************************************************************************/
/***
  * 功能：
        打开WiFi down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiBtnEnableOn_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	return iReturn;
}

/***
  * 功能：
        关闭WiFi down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiBtnEnableOff_Down(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	return iReturn;
}                               

/***
  * 功能：
        打开WiFi up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiBtnEnableOn_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	if(1 != WifiSwitch)
	{
		WifiSwitch = 1;

		setWifiEnable(1);
	}

	TopInfoTips();
	
	pCurWiFiSet->iWiFiOpenSta = 1;//系统设置区

	
	SetPictureBitmap(BmpFileDirectory"btn_wifi_on_press.bmp",
					pWiFiBtnREnableOn);
	SetPictureBitmap(BmpFileDirectory"btn_wifi_off_unpress.bmp",
					pWiFiBtnREnableOff);
	SetPictureEnable(1, pWiFiBtnREnableOff);
	DisplayPicture(pWiFiBtnREnableOn);
	DisplayPicture(pWiFiBtnREnableOff);
    DisplayLabel(pWifiLblOn);
    DisplayLabel(pWifiLblOff);
	SetPictureEnable(0, pWiFiBtnREnableOn);
	
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}  


/***
  * 功能：
        关闭WiFi up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiBtnEnableOff_Up(void *pInArg,   int iInLen, 
                               void *pOutArg, int iOutLen)
{
     //错误标志、返回值定义
    int iReturn = 0;
	int i;
	
	if( 0 != WifiSwitch )
	{
		WifiSwitch = 0;
	}
	
	setWifiEnable(0);

	TopInfoTips(); 
	//断开WIFI
	wifi_Disconnect();
	//系统设置区
	pCurWiFiSet->iWiFiOpenSta    = 0;
	pCurWiFiSet->iWiFiConnectSta = 0;

	/*退出连接网络线程*/
	if(1 == getConnectready())
	{
		wifi_Connect_Cancel();
	}

	for(i = 0; i < DIS_WIFI_ITEM; i++)
	{		
		DisplayPicture(pWifiWidget[i]->pBg);
		SetWifiwidgeEnable(0,pWifiWidget[i]);
	}

	SetPictureBitmap(BmpFileDirectory"btn_wifi_on_unpress.bmp",pWiFiBtnREnableOn);
	SetPictureBitmap(BmpFileDirectory"btn_wifi_off_press.bmp",pWiFiBtnREnableOff);
	SetPictureEnable(1, pWiFiBtnREnableOn);
	DisplayPicture(pWiFiBtnREnableOn);
	DisplayPicture(pWiFiBtnREnableOff);
	SetPictureEnable(0, pWiFiBtnREnableOff);
	DisplayLabel(pWifiLblOn);
    DisplayLabel(pWifiLblOff);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}              

/*******************************************************************************
**			    			连接wifi事件处理函数				 		      **
*******************************************************************************/


/***
  * 功能：
        wifi item 按下
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiBtnWidget_Down(void *pInArg,   int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	if( 1 == WifiSwitch)				//wifi状态为开启时响应
	{
		WifiWidgetDown(pWifiWidget[iOutLen]);	//设置按下时的picture
		DisplayWifiWidget(pWifiWidget[iOutLen]);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	
	return iReturn;
}                               

/***
  * 功能：
        wifi item 松开
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiBtnWidget_Up(void *pInArg,   int iInLen, 
                          void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
    int iReturn = 0;
	NETWORK_INFO_SINGLE SelectAP;
	
	if( 1 == WifiSwitch)			//wifi状态为开启时响应
	{
		WifiWidgetUp(pWifiWidget[iOutLen], iOutLen);
		DisplayWifiWidget(pWifiWidget[iOutLen]);
		RefreshScreen(__FILE__, __func__, __LINE__);
		
		SetWifiwidgeEnable(0,pWifiWidget[iOutLen]);

		/*停止扫描热点*/
		setWifiEnable(0);
		
		/*取消网络连接，防止出现卡顿*/
		if(1 == getConnectready())
		{
			wifi_Connect_Cancel();
		}

		MutexLock(&NetInfo_List_Mutex);
		SelectAP = pNetInfo_List[iOutLen];
		MutexUnlock(&NetInfo_List_Mutex);

		if( 1 == SelectAP.flags)		//wifi为加密
		{
			
			if(0 == check_if_network_saved(SelectAP,cTmpInputBuff_nopsk,sizeof(cTmpInputBuff_nopsk)))
			{
				setWifiConnectFunc(WiFiConnectCallBack);
				setWifiGetIPFunc(WiFiGetIPCallBack);
				
				DisplayPskWindow(pFrmWiFi, SelectAP);//显示密码输入框			
			}
			else
			{
				setWifiConnectFunc(WiFiConnectCallBack_nopsk);
				setWifiGetIPFunc(WiFiGetIPCallBack_nopsk);
				
				wifi_Disconnect();
				DisplayConnect(pFrmWiFi, SelectAP);//连接连接过的WIFI
			}
		}
		else								//wifi未加密
		{
			setWifiConnectFunc(WiFiConnectCallBack_nopsk);
			setWifiGetIPFunc(WiFiGetIPCallBack_nopsk);
			
			wifi_Disconnect();
			DisplayConnect(pFrmWiFi, SelectAP);//连接没有密码的WIFI
 		}
	}
	
	return iReturn;
}  


void RefreshTopInfo(GUICHAR *Info, GUICHAR *IP)
{
	unsigned int uiOldColor;
	
	pWifiPen = GetCurrPen();
	uiOldColor = pWifiPen->uiPenColor;
    SetPenColor(0x555555, pWifiPen);
	DrawBlock(153,152,649,181);
	SetPenColor(uiOldColor, pWifiPen);

	
	GuiMemFree(pWifiStrTopInfo);
	pWifiStrTopInfo = Info;
	GuiMemFree(pWifiStrIP);
	pWifiStrIP	 = IP;

	
	SetLabelText(pWifiStrTopInfo, pWifiLblTopInfo);
	SetLabelText(pWifiStrIP, pWifiLblIP);
	
	DisplayLabel(pWifiLblTopInfo);								//显示顶部提示信息
	DisplayLabel(pWifiLblIP);								//显示顶部提示信息
}
/***
  * 功能：
        顶部信息提示栏
  * 参数：
  * 返回：
  * 备注：
***/
static void TopInfoTips()
{ 	
	if(1 == WifiSwitch)
	{
		if( 1 == getDhcpready())
		{
			NETWORK_INFO_SINGLE ConnectNetInfo = getConnectNetInfo();
			char hostIP[128] = {0};
		  	char IP[32];	
			
			strcpy(hostIP,getIP());
			sprintf(IP,"IP:%s",hostIP);
			
			RefreshTopInfo(TransString(ConnectNetInfo.ssid),TransString(IP));
		}
		else
		{
			RefreshTopInfo(TransString("WIFI_DISCONNECT"),TransString(" "));
		}
	}
	else
	{
		RefreshTopInfo(TransString("WIFI_CLOSE"),TransString(" "));
	}

 }

/***
  * 功能：
		设备开机时wifi状态检测及恢复
  * 参数：
  * 返回：
  * 备注：
		1.wifi关闭:不做处理
		2.wifi打开，连接状态为关闭:仅打开wifi，不做连接处理
		3.wifi打开，连接状态为连接:
		(1)ssid在scan_r列表中，正常连接；
		(2)ssid不在scan_r列表中
***/
int WifiStartInit(void)
{
	int iReturn = 0;

	LOG(LOG_INFO, "\n设备开机时wifi状态检测及恢复\n");
	LOG(LOG_INFO, "pCurWiFiSet->iWiFiOpenSta = %d\t,pCurWiFiSet->iWiFiConnectSta = %d\t,pCurWiFiSet->cSSID = %s\t,pCurWiFiSet->cPassWord = %s\t,pCurWiFiSet->cFlags = %d\n",
		pCurWiFiSet->iWiFiOpenSta,
		pCurWiFiSet->iWiFiConnectSta,
		pCurWiFiSet->cSSID,
		pCurWiFiSet->cPassWord,
		pCurWiFiSet->cFlags);

	setWifiScanFunc(wifi_ScanCallback);
	
	WifiSwitch = pCurWiFiSet->iWiFiOpenSta;					//应用界面wifi总开关(是否创建wifi扫描应用线程)
	setWifiEnable(WifiSwitch);
	
	InitMutex(&NetInfo_List_Mutex, NULL);

	return iReturn;
}
/*******************************************************************************
**							帮助和返回事件定义						          **
*******************************************************************************/
/***
  * 功能：
        返回按钮up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int WiFiBtnBack_Up(void *pInArg, int iInLen, 
						   void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	GUIWINDOW *pWnd = NULL;	
	
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                    FrmMainInit, FrmMainExit, 
	                    FrmMainPaint, FrmMainLoop, 
				        FrmMainPause, FrmMainResume,
	                    NULL);			    //pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);			//发送消息以便调用新的窗体

	return iReturn;	
}

/***
  * 功能：
        菜单栏控件回调
  * 参数：
        无
  * 返回：
		无
  * 备注：
***/
static void WiFiWndMenuCallBack(int selected)
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
			SendWndMsg_WindowExit(pFrmWiFi);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;
		case 1://调取时间设置
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
			                    FrmTimeSetInit, FrmTimeSetExit, 
			                    FrmTimeSetPaint, FrmTimeSetLoop, 
						        FrmTimeSetPause, FrmTimeSetResume,
			                    NULL);          
			SendWndMsg_WindowExit(pFrmWiFi);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;	
		case 2:	//调取语言设置窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmLanguageSetInit, FrmLanguageSetExit, 
								FrmLanguageSetPaint, FrmLanguageSetLoop, 
								FrmLanguageSetPause, FrmLanguageSetResume,
								NULL);			
			SendWndMsg_WindowExit(pFrmWiFi);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;
		case 3:	//调取wifi窗体
			//..
			break;			
		case 4:	//调取软件升级窗体
			pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmSysMaintainInit, FrmSysMaintainExit, 
								FrmSysMaintainPaint, FrmSysMaintainLoop, 
								FrmSysMaintainPause, FrmSysMaintainResume,
								NULL);			
			SendWndMsg_WindowExit(pFrmWiFi);  
			SendSysMsg_ThreadCreate(pWnd); 
			break;
		case 5:
            #ifdef MINI2
            pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    							FrmAboutInit, FrmAboutExit, 
    							FrmAboutPaint, FrmAboutLoop, 
    							FrmAboutPause, FrmAboutResume,
    							NULL);			       //pWnd由调度线程释放
    		SendWndMsg_WindowExit(pFrmWiFi);	      //发送消息以便退出当前窗体
    		SendSysMsg_ThreadCreate(pWnd);			  //发送消息以便调用新的窗体
    		#endif
			break;			
		case BACK_DOWN:
			WiFiBtnBack_Up(NULL, 0, NULL, 0);
			break;				
		case HOME_DOWN:
			WiFiBtnBack_Up(NULL, 0, NULL, 0);
			break;
		default:
			break;
	}
}
