/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifinopsk.c
* 摘    要：  显示wifi的连接窗口
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


#include "wnd_frmwifinopsk.h"

/*******************************************************************************
**						为实现窗体pGuiWifi而需要引用的其他头文件 			  **
*******************************************************************************/
#include "wnd_frmwifi.h"
#include "guiglobal.h"
#include "wnd_global.h"
#include "wnd_frmime.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"

static CONNECT_PROGRESS WIFInopskConnect;
/*******************************************************************************
**							窗体pGuiWifi中的控件定义					  	  **
*******************************************************************************/	
static GUIPICTURE *pWifiInput        = NULL; //密码弹窗
static GUILABEL *pWifiLblConnectTips = NULL; //连接中
static GUILABEL *pWifiLblSsid        = NULL; //wifi名称LBl
static GUICHAR *pWifiStrConnectTips  = NULL; //"连接中。。。"
static GUICHAR *pWifiStrSsid         = NULL; //wifi名称
static GUIWINDOW *pWifiNoPsk		 = NULL; //连接窗体

static GUIPICTURE *pWifiNoPskCancel  = NULL; //取消按钮图片
static GUIPICTURE *pWifiNoPskForget  = NULL; //忘记密码按钮图片

static GUILABEL *pWifiNoPskLblCancel  = NULL; //取消按钮图片
static GUILABEL *pWifiNoPskLblForget  = NULL; //忘记密码按钮图片

static GUICHAR *pWifiNoPskStrCancel  = NULL; //取消按钮图片
static GUICHAR *pWifiNoPskStrForget  = NULL; //忘记密码按钮图片

/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/
NETWORK_INFO_SINGLE WifiConnect_nopsk;
char cTmpInputBuff_nopsk[WIFI_PSK_LEN]  = {0};
static int iConnectFlag = 0;
static int iConnectAgain = 0;
extern PWIFISET pCurWiFiSet;


/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/

static int WiFiBtnCancel_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
static int WiFiBtnCancel_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
static int WiFiBtnForget_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
static int WiFiBtnForget_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen);

//刷新显示wifi连接过程提示信息，防止用户误以为死机
static void RefreshWifiDisplay(void);

/*刷新提示信息*/
int ChangePromptInfo_nopsk(GUICHAR *NewInfo)
{
	unsigned int uiOldColor;
	GUIPEN *pWifiPen = NULL;
	
	pWifiPen = GetCurrPen();
	uiOldColor = pWifiPen->uiPenColor;
	SetPenColor(0x70ff, pWifiPen);
	DrawBlock(285, 250, 529, 270);//335,300,150,20
	SetPenColor(uiOldColor, pWifiPen);
	GuiMemFree(pWifiStrConnectTips);
	pWifiStrConnectTips = NewInfo;
	SetLabelText(pWifiStrConnectTips, pWifiLblConnectTips);
	DisplayLabel(pWifiLblConnectTips);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}


/*
连接热点后的回调函数

参数: 是否连接成功(仅连接还未分配IP)
*/
int WiFiConnectCallBack_nopsk(char Connectflag)
{
	if(Connectflag)
	{
		ChangePromptInfo_nopsk(TransString("WIFI_GETIP"));
		WIFInopskConnect = EN_WIFI_GETIP;
	}
	else
	{
		ChangePromptInfo_nopsk(TransString("WIFI_CONNECTFAIL"));
		WIFInopskConnect = EN_WIFI_DISCONNECT;
		iConnectAgain = 1;
		MsecSleep(1000);
	}
	return 0;
}


/*
获取IP后的回调函数

参数: 是否获得IP地址
*/
int WiFiGetIPCallBack_nopsk(char GetIPflag)
{
	if(GetIPflag)
	{
		ChangePromptInfo_nopsk(TransString("WIFI_CONNECTED"));
		WIFInopskConnect = EN_WIFI_CONNECT;
		
		pCurWiFiSet->iWiFiConnectSta = 1;
		memset(pCurWiFiSet->cSSID,0,sizeof(pCurWiFiSet->cSSID));
		memset(pCurWiFiSet->cPassWord,0,sizeof(pCurWiFiSet->cPassWord));
		
		strncpy(pCurWiFiSet->cSSID, WifiConnect_nopsk.ssid, strlen(WifiConnect_nopsk.ssid));//保存到系统设置中
		strncpy(pCurWiFiSet->cPassWord,cTmpInputBuff_nopsk, strlen(cTmpInputBuff_nopsk));//保存到系统设置中
		pCurWiFiSet->cFlags = WifiConnect_nopsk.flags;//加密方式
		iConnectAgain = 0;
		iConnectFlag = 1;
	}
	else
	{
		ChangePromptInfo_nopsk(TransString("WIFI_CONNECTFAIL"));
		WIFInopskConnect = EN_WIFI_DISCONNECT;
		iConnectAgain = 1;
		MsecSleep(1000);
	}
	
	return 0;
}


static int WiFiBtnCancel_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_press.bmp",pWifiNoPskCancel);
	DisplayPicture(pWifiNoPskCancel);
	DisplayLabel(pWifiNoPskLblCancel);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;


}

static int WiFiBtnCancel_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    GUIWINDOW *pWnd = NULL;
	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_unpress.bmp",pWifiNoPskCancel);
	DisplayPicture(pWifiNoPskCancel);
	DisplayLabel(pWifiNoPskLblCancel);
	ChangePromptInfo_nopsk(TransString("WIFI_CANCELING"));
	RefreshScreen(__FILE__, __func__, __LINE__);
	iConnectAgain = 0;
	if(getConnectready())
	{
		wifi_Connect_Cancel();
	}
	//setWifiEnable(1);
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
					   FrmWiFiInit, FrmWiFiExit,
					   FrmWiFiPaint, FrmWiFiLoop,
					   FrmWiFiPause, FrmWiFiResume,
					   NULL);	//pWnd由调度线程释放
	SendWndMsg_WindowExit(pWifiNoPsk);//pGuiWifi
	SendSysMsg_ThreadCreate(pWnd);
	return iReturn;

}




static int WiFiBtnForget_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_press.bmp",pWifiNoPskForget);
	DisplayPicture(pWifiNoPskForget);
	DisplayLabel(pWifiNoPskLblForget);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}



static int WiFiBtnForget_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    GUIWINDOW *pWnd = NULL;

	
	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_unpress.bmp",pWifiNoPskForget);
	DisplayPicture(pWifiNoPskForget);
	DisplayLabel(pWifiNoPskLblForget);
	ChangePromptInfo_nopsk(TransString("WIFI_CANCELING"));
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	iConnectAgain = 0;
	if(getConnectready())
	{
		wifi_Connect_Cancel();
	}

	forget_network_config();

	//setWifiEnable(1);
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
					   FrmWiFiInit, FrmWiFiExit,
					   FrmWiFiPaint, FrmWiFiLoop,
					   FrmWiFiPause, FrmWiFiResume,
					   NULL);	//pWnd由调度线程释放
	SendWndMsg_WindowExit(pWifiNoPsk);//pGuiWifi
	SendSysMsg_ThreadCreate(pWnd);
	return iReturn;
}

/***
  * 功能：
        wifi密码界面文本资源初始化
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int ResInfo_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;


	pWifiStrConnectTips = TransString("WIFI_VERACCOUNT");
	pWifiStrSsid = TransString(WifiConnect_nopsk.ssid);

	pWifiNoPskStrCancel = TransString("WIFI_CANCEL");
	
	if(WifiConnect_nopsk.flags)
	{
		pWifiNoPskStrForget = TransString("WIFI_FORGET");
	}
	
    return iReturn;
}

/***
  * 功能：
        wifi连接界面文本资源销毁
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int ResInfo_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    int iReturn = 0;

	GuiMemFree(pWifiStrConnectTips);
	GuiMemFree(pWifiStrSsid);
	GuiMemFree(pWifiNoPskStrCancel);	
	if(WifiConnect_nopsk.flags)
	{
		GuiMemFree(pWifiNoPskStrForget);
	}
	
    return iReturn;
}



/***
  * 功能：
        wifi连接界面(无密码)初始化
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
int WIfiNoPskInit(void *pWndObj)
{
	int iReturn = 0;	
	GUIMESSAGE *pMsg = NULL;

	//获得当前窗体对象
	pWifiNoPsk = (GUIWINDOW *)pWndObj;
	iConnectFlag = 0;

	ResInfo_Init(NULL, 0, NULL, 0);
	//pWifiInput = CreatePicture(230,138,340,200,BmpFileDirectory"wifi_mimaBG.bmp");
    pWifiInput = CreatePhoto("wifi_mimaBG");
	//SSID显示
	pWifiLblSsid = CreateLabel(285,170,230,48,pWifiStrSsid);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblSsid);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pWifiLblSsid);

	//连接信息提示"连接中"
	pWifiLblConnectTips = CreateLabel(285,250,230,24,pWifiStrConnectTips);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblConnectTips);
	SetLabelFont(getGlobalFnt(EN_FONT_GREEN),pWifiLblConnectTips);

	if(WifiConnect_nopsk.flags)
	{
		//取消
		pWifiNoPskCancel = CreatePicture(270,279,123,48,BmpFileDirectory"wifi_mima_btn_unpress.bmp");
		pWifiNoPskLblCancel= CreateLabel(270,291,123,24,pWifiNoPskStrCancel);
		SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiNoPskLblCancel);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pWifiNoPskLblCancel);
		//忘记密码
		pWifiNoPskForget = CreatePicture(416,279,123,48,BmpFileDirectory"wifi_mima_btn_unpress.bmp");
		pWifiNoPskLblForget = CreateLabel(416,291,123,24,pWifiNoPskStrForget);
		SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiNoPskLblForget);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pWifiNoPskLblForget);	
		
		//注册窗体控件
		AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), pWifiNoPskCancel,pWifiNoPsk);
		AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), pWifiNoPskForget,pWifiNoPsk);

		//注册窗体控件的消息处理
		pMsg = GetCurrMessage();
		LoginMessageReg(GUIMESSAGE_TCH_UP,pWifiNoPskForget,WiFiBtnForget_Up, NULL, 0, pMsg); 
		LoginMessageReg(GUIMESSAGE_TCH_DOWN,pWifiNoPskForget,WiFiBtnForget_Down, NULL, 0, pMsg); 
		LoginMessageReg(GUIMESSAGE_TCH_UP,pWifiNoPskCancel,WiFiBtnCancel_Up, NULL, 0, pMsg); 
		LoginMessageReg(GUIMESSAGE_TCH_DOWN,pWifiNoPskCancel,WiFiBtnCancel_Down, NULL, 0, pMsg); 
	}
	else
	{
		//取消
		pWifiNoPskCancel = CreatePicture(339,279,123,48,BmpFileDirectory"wifi_mima_btn_unpress.bmp");
		pWifiNoPskLblCancel= CreateLabel(339,291,123,24,pWifiNoPskStrCancel);
		SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiNoPskLblCancel);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pWifiNoPskLblCancel);
		
		//注册窗体控件
		AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), pWifiNoPskCancel,pWifiNoPsk);
		
		//注册窗体控件的消息处理
		pMsg = GetCurrMessage();
		LoginMessageReg(GUIMESSAGE_TCH_UP,pWifiNoPskCancel,WiFiBtnCancel_Up, NULL, 0, pMsg); 
		LoginMessageReg(GUIMESSAGE_TCH_DOWN,pWifiNoPskCancel,WiFiBtnCancel_Down, NULL, 0, pMsg); 
	}

	

	return iReturn;
}

/***
  * 功能：
        窗体pWifiNoPsk的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		密码弹窗界面窗体
***/
int WIfiNoPskExit(void *pWndObj)
{
    int iReturn = 0;
	GUIMESSAGE *pMsg = NULL;
	//获取当前窗体
	pWifiNoPsk = (GUIWINDOW *)pWndObj;
    pMsg = GetCurrMessage();
	//清空消息队列中的消息注册项
    ClearMessageReg(pMsg);
	//销毁窗体注册控件
    ClearWindowComp(pWifiNoPsk);
	DestroyPicture(&pWifiInput);
	DestroyLabel(&pWifiLblConnectTips);
	DestroyLabel(&pWifiLblSsid);
	
	DestroyPicture(&pWifiNoPskCancel);
	DestroyLabel(&pWifiNoPskLblCancel);
	
	if(WifiConnect_nopsk.flags)
	{	
		DestroyPicture(&pWifiNoPskForget);
		DestroyLabel(&pWifiNoPskLblForget);
	}

	//释放窗体的文本资源
    ResInfo_Exit(NULL, 0, NULL, 0);

	return iReturn;
	
}

/***
  * 功能：
        窗体pWifiNoPsk的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		密码弹窗界面窗体
***/
int WIfiNoPskPaint(void *pWndObj)
{
	int iReturn = 0;
	pWifiNoPsk = (GUIWINDOW *)pWndObj;
	DispTransparent(0, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//显示桌面控件Picture
	DisplayPicture(pWifiInput);
	DisplayLabel(pWifiLblSsid);
	DisplayLabel(pWifiLblConnectTips);

	DisplayPicture(pWifiNoPskCancel);
	DisplayLabel(pWifiNoPskLblCancel);
	
	if(WifiConnect_nopsk.flags)
	{
		DisplayPicture(pWifiNoPskForget);
		DisplayLabel(pWifiNoPskLblForget);
	}
    SetPowerEnable(2, 1);
	
	RefreshScreen(__FILE__, __func__, __LINE__);

	WIFInopskConnect = EN_WIFI_AUTHENTICATOR;
	
	if(-1 == wifi_Connect(WifiConnect_nopsk, cTmpInputBuff_nopsk))
	{
		ChangePromptInfo_nopsk(TransString("WIFI_CONNECTFAIL"));
	}
	
	
	return iReturn;
}

/***
  * 功能：
        窗体pWifiNoPsk的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int WIfiNoPskLoop(void *pWndObj)
{
	int iReturn = 0;
    GUIWINDOW *pWnd = NULL;

	
	if(iConnectFlag)
	{
		//setWifiEnable(1);
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						   FrmWiFiInit, FrmWiFiExit,
						   FrmWiFiPaint, FrmWiFiLoop,
						   FrmWiFiPause, FrmWiFiResume,
						   NULL);	//pWnd由调度线程释放
		SendWndMsg_WindowExit(pWifiNoPsk);//pGuiWifi
		SendSysMsg_ThreadCreate(pWnd);
	}
	else
	{
		MsecSleep(10);
		if((0 == getConnectready()) && (1 == iConnectAgain))
		{
			wifi_Connect(WifiConnect_nopsk, cTmpInputBuff_nopsk);
			ChangePromptInfo_nopsk(TransString("WIFI_VERACCOUNT"));
			WIFInopskConnect = EN_WIFI_AUTHENTICATOR;
		}
	}
	
	RefreshWifiDisplay();
	
	return iReturn;
}

/***
  * 功能：
        窗体pWifiNoPsk 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int WIfiNoPskPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/***
  * 功能：
        	窗体pWifiNoPsk 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int WIfiNoPskResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}
//刷新显示wifi连接过程提示信息，防止用户误以为死机
static void RefreshWifiDisplay(void)
{
	static int refreshDisplay1 = 0;
	static int refreshDisplay2 = 0;
	static int time = 0;

	switch(WIFInopskConnect)
	{
		case EN_WIFI_AUTHENTICATOR:
			if(++time == 50)
			{
				switch(refreshDisplay1)
				{
					case 0:
						ChangePromptInfo_nopsk(TransString("WIFI_VERACCOUNT"));
						break;
					case 1:
						ChangePromptInfo_nopsk(TransString("WIFI_VERACCOUNT1"));
						break;
					case 2:
						ChangePromptInfo_nopsk(TransString("WIFI_VERACCOUNT2"));
						break;
					case 3:
						ChangePromptInfo_nopsk(TransString("WIFI_VERACCOUNT3"));
						break;
				}
				time = 0 ;			
				if(++refreshDisplay1 == 4)
				{
					refreshDisplay1 = 0;
				}
			}

			refreshDisplay2 = 0;
			break;
		case EN_WIFI_GETIP:
			if(++time == 50)
			{
				switch(refreshDisplay2)
				{
					case 0:
						ChangePromptInfo_nopsk(TransString("WIFI_GETIP"));
						break;
					case 1:
						ChangePromptInfo_nopsk(TransString("WIFI_GETIP1"));
						break;
					case 2:
						ChangePromptInfo_nopsk(TransString("WIFI_GETIP2"));
						break;
					case 3:
						ChangePromptInfo_nopsk(TransString("WIFI_GETIP3"));
						break;
				}
				time = 0 ;
				if(++refreshDisplay2 == 4)
				{
					refreshDisplay2 = 0;
				}
			}
			refreshDisplay1 = 0;
			break;
		default: 			
			refreshDisplay1 = 0;
			refreshDisplay2 = 0;
			time = 0 ;
			break;
			
	}
}

/***
  * 功能：
        无密码连接的弹窗界面
  * 参数：
        1.void *pWndObj:  		指向当前窗体对象
        2.WIFI_INFO WifiInfo:	用户点击连接的wifi信息结构
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		连接弹窗界面窗体
***/
int DisplayConnect(void *pWndObj, NETWORK_INFO_SINGLE WifiInfo)
{
	int iReturn = 0;

	GUIWINDOW *pWnd = NULL;
	//得到当前窗体对象
    pWnd = (GUIWINDOW *)pWndObj;
	
	memcpy(&WifiConnect_nopsk,&WifiInfo, sizeof(WifiInfo));//WifiConnect 保存用户随机选择的wifi信息
	pWifiNoPsk = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
									  WIfiNoPskInit,WIfiNoPskExit,
									  WIfiNoPskPaint,WIfiNoPskLoop,
									  WIfiNoPskPause,WIfiNoPskResume,NULL);
		
	SendWndMsg_WindowExit(pWnd);
	SendSysMsg_ThreadCreate(pWifiNoPsk);
	
	return iReturn;
}
