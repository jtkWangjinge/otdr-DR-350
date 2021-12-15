/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifipsk.c
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


#include "wnd_frmwifipsk.h"

/*******************************************************************************
**						为实现窗体pGuiWifi而需要引用的其他头文件 			  **
*******************************************************************************/
#include "wnd_frmwifi.h"
#include "guiglobal.h"
#include "wnd_global.h"
#include "wnd_frmime.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
/*******************************************************************************
**								变量定义				 					  **
*******************************************************************************/	
static GUIWINDOW *pGuiWifi 	   = NULL;
static NETWORK_INFO_SINGLE SaveWifi;
static PSK_INFO pskinfo;
static char cTmpInputBuff[WIFI_PSK_LEN]  = {0};
extern PWIFISET pCurWiFiSet;
static int iConnectFlag = 0;

static int iMutexInitFlag = 0;
GUIMUTEX Connect_State_Mutex;

static CONNECT_PROGRESS WIFIpskConnect;
/*******************************************************************************
**							窗体pGuiWifi中的控件定义					  	  **
*******************************************************************************/

static GUIPICTURE *pWifiInput        = NULL; //密码弹窗
static GUIPICTURE *pWifiApplyInfo    = NULL; //确认
static GUIPICTURE *pWifiCancelInfo   = NULL; //取消
static GUIPICTURE *pWiFiPskInput     = NULL; //密码输入框
static GUIPICTURE *pWifiPskDisplay   = NULL; //密码显示

static GUILABEL *pWifiLblPskInput    = NULL;
static GUILABEL *pWifiLblApplyInfo   = NULL; //"确认"
static GUILABEL *pWifiLblCancelInfo  = NULL; //"取消"
static GUILABEL *pWifiLblConnectTips = NULL; //连接中
static GUILABEL *pWifiLblSsid        = NULL; //wifi名称LBl

static GUICHAR *pWifiStrPskInput     = NULL; //密码信息
static GUICHAR *pWifiStrApplyInfo    = NULL; //确认
static GUICHAR *pWifiStrCancleInfo   = NULL; //取消
static GUICHAR *pWifiStrConnectTips  = NULL; //"连接中。。。"
static GUICHAR *pWifiStrSsid         = NULL; //wifi名称

static GUIPEN *pWifiPen              = NULL; //绘制块(密码出错提示)

/*******************************************************************************
**			    			窗体内的其他函数声明					 		  **
*******************************************************************************/

static int WiFiTextResInfo_Init(void *pInArg, int iInLen,void *pOutArg, int iOutLen);
static int WiFiTextResInfo_Exit(void *pInArg, int iInLen,void *pOutArg, int iOutLen);
static int WiFiBtnApply_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
static int WiFiBtnApply_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
static int WiFiBtnCancel_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
static int WiFiBtnCancel_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
static int pWiFiPskKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen);
int        DisplayPskWindow(void *pWndObj, NETWORK_INFO_SINGLE WifiInfo);
//刷新显示wifi连接过程提示信息，防止用户误以为死机
static void RefreshWifiDisplay(void);

static void SetConnectState(CONNECT_PROGRESS currConnectState)
{
    MutexLock(&Connect_State_Mutex);
    WIFIpskConnect = currConnectState;
    MutexUnlock(&Connect_State_Mutex);
}

static CONNECT_PROGRESS getConnectState()
{
    CONNECT_PROGRESS ret = EN_WIFI_DISCONNECT;
    MutexLock(&Connect_State_Mutex);
    ret = WIFIpskConnect;
    MutexUnlock(&Connect_State_Mutex);
    return ret;
}

/*刷新提示信息*/
int ChangePromptInfo(GUICHAR *NewInfo)
{
	unsigned int uiOldColor;
	
	pWifiPen = GetCurrPen();
	uiOldColor = pWifiPen->uiPenColor;
	SetPenColor(0x70ff, pWifiPen);
	DrawBlock(288, 245, 529, 270);//335,300,150,20
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
int WiFiConnectCallBack(char Connectflag)
{
	if(0 == Connectflag)
	{
		SetConnectState(EN_WIFI_DISCONNECT);
		ChangePromptInfo(TransString("WIFI_PASSWORDERROR"));

        SetPictureEnable(1, pWiFiPskInput);
		SetPictureEnable(1, pWifiPskDisplay);
		SetPictureEnable(1, pWifiApplyInfo);
		memset(cTmpInputBuff,0,sizeof(cTmpInputBuff));

		GuiMemFree(pWifiStrPskInput);
		pWifiStrPskInput = TransString("");
		SetLabelText(pWifiStrPskInput, pWifiLblPskInput);
		DisplayPicture(pWiFiPskInput);
		DisplayLabel(pWifiLblPskInput);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	else
	{
		SetConnectState(EN_WIFI_GETIP);
		ChangePromptInfo(TransString("WIFI_GETIP"));
	}
	return 0;
}


/*
获取IP后的回调函数

参数: 是否获得IP地址
*/
int WiFiGetIPCallBack(char GetIPflag)
{
	if(GetIPflag)
	{
		SetConnectState(EN_WIFI_CONNECT);
		ChangePromptInfo(TransString("WIFI_CONNECTED"));
		
		pCurWiFiSet->iWiFiConnectSta = 1;
		memset(pCurWiFiSet->cSSID,0,sizeof(pCurWiFiSet->cSSID));
		memset(pCurWiFiSet->cPassWord,0,sizeof(pCurWiFiSet->cPassWord));
		
		strncpy(pCurWiFiSet->cSSID, SaveWifi.ssid, strlen(SaveWifi.ssid));//保存到系统设置中
		strncpy(pCurWiFiSet->cPassWord,cTmpInputBuff, strlen(cTmpInputBuff));//保存到系统设置中
		pCurWiFiSet->cFlags = SaveWifi.flags;//加密方式	
		iConnectFlag = 1;
	}
	else
	{
		SetConnectState(EN_WIFI_DISCONNECT);
		ChangePromptInfo(TransString("WIFI_GETIPFAIL"));

        SetPictureEnable(1, pWiFiPskInput);
		SetPictureEnable(1, pWifiPskDisplay);
		SetPictureEnable(1, pWifiApplyInfo);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	

	return 0;
}

/***
  * 功能：
        密码提交确认 down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int WiFiBtnApply_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_press.bmp",pWifiApplyInfo);
	DisplayPicture(pWifiApplyInfo);
	DisplayLabel(pWifiLblApplyInfo);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

/***
  * 功能：
        密码提交确认 up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int WiFiBtnApply_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;

	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_unpress.bmp",pWifiApplyInfo);
	DisplayPicture(pWifiApplyInfo);
	DisplayLabel(pWifiLblApplyInfo);
	DisplayLabel(pWifiLblConnectTips);//connect...
	RefreshScreen(__FILE__, __func__, __LINE__);

	SetPictureEnable(0, pWifiApplyInfo);
	
	if( 0 == strlen(cTmpInputBuff))
	{
		ChangePromptInfo(TransString("WIFI_PASSWOEDNULL"));
	}
	else
	{
		//连接期间，取消按钮使能关闭(密码是否明文显示)
		SetPictureEnable(0, pWiFiPskInput);
		SetPictureEnable(0, pWifiPskDisplay);
		
		DisplayLabel(pWifiLblConnectTips);//connect...
		RefreshScreen(__FILE__, __func__, __LINE__);
		
		SetConnectState(EN_WIFI_AUTHENTICATOR);
		if(-1 == wifi_Connect(SaveWifi, cTmpInputBuff))
		{
			ChangePromptInfo(TransString("WIFI_CONNECTFAIL"));
		}
		
	}
	
	return iReturn;
}

/***
  * 功能：
        密码提交取消 down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int WiFiBtnCancel_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_press.bmp", pWifiCancelInfo);
	DisplayPicture(pWifiCancelInfo);
	DisplayLabel(pWifiLblCancelInfo);
	RefreshScreen(__FILE__, __func__, __LINE__);
	return iReturn;
}

/***
  * 功能：
        密码提交取消 up事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int WiFiBtnCancel_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
	SetPictureBitmap(BmpFileDirectory"wifi_mima_btn_unpress.bmp", pWifiCancelInfo);
	DisplayPicture(pWifiCancelInfo);
	DisplayLabel(pWifiLblCancelInfo);
	if(getConnectready())
	{
		ChangePromptInfo(TransString("WIFI_CANCELING"));
	}
	RefreshScreen(__FILE__, __func__, __LINE__);

	if(getConnectready())
	{
		wifi_Connect_Cancel();
	}
	
	SetPictureEnable(0, pWifiCancelInfo);

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						   FrmWiFiInit, FrmWiFiExit,
						   FrmWiFiPaint, FrmWiFiLoop,
						   FrmWiFiPause, FrmWiFiResume,
						   NULL);	//pWnd由调度线程释放
	SendWndMsg_WindowExit(pGuiWifi);
	SendSysMsg_ThreadCreate(pWnd);

	return iReturn;
}



/***
  * 功能：
        将用户输入的密码字符转成'*'
  * 参数：
        1.char *pDes	:   转换成*的字符串
  * 返回：
  * 备注：
***/
static void intostar(char *pDes)
{
	int i = 0;
	int iLen = 0;
	char buff[66] = {0};
	iLen = strlen(cTmpInputBuff);
	
	strncpy(buff,cTmpInputBuff,iLen);
	for(i=0;i<iLen;i++)
	{
		buff[i] = '*';
	}
	buff[i] = '\0';
	strcpy(pDes,buff);
}

/***
  * 功能：
        输入密码的显示方式(是否明文显示)
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0

  * 备注：
***/
static int PskDisplayOrNot(void *pInArg,int iInLen,void *pOutArg, int iOutLen)	
{
	int iReturn = 0;

	char buff[66] = {0};
	intostar(buff);//明文字符转成'*'

	if( 1 == pskinfo.iCount)
	{
		SetPictureBitmap(BmpFileDirectory"wifi_mima_display.bmp", pWifiPskDisplay);
		SetPictureBitmap(BmpFileDirectory"wifi_mima_input.bmp", pWiFiPskInput);
		DisplayPicture(pWiFiPskInput);
		DisplayPicture(pWifiPskDisplay);
		SetLabelText(TransString(cTmpInputBuff), pWifiLblPskInput);
		DisplayLabel(pWifiLblPskInput);
		RefreshScreen(__FILE__, __func__, __LINE__);

		pskinfo.iCount = -1;

	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"wifi_mima_undisplay.bmp", pWifiPskDisplay);
		SetPictureBitmap(BmpFileDirectory"wifi_mima_input.bmp", pWiFiPskInput);
		DisplayPicture(pWiFiPskInput);
		DisplayPicture(pWifiPskDisplay);
		SetLabelText(TransString(buff), pWifiLblPskInput);
		DisplayLabel(pWifiLblPskInput);
		RefreshScreen(__FILE__, __func__, __LINE__);
	
		pskinfo.iCount = 1;
	}
    
	return iReturn;
}

/***
  * 功能：
        密码界面窗体回调
  * 参数：
        1.GUIWINDOW **pWnd	:   回调的窗体

  * 返回：
  		成功返回0，失败返回非0

  * 备注：
***/
static void ReCreateWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  GuiWifiInit,GuiWifiExit,
					  GuiWifiPaint,GuiWifiLoop,
					  GuiWifiPause,GuiWifiResume,NULL);
}

/***
  * 功能：
  		密码输入回调，获取输入的密码字符串
  * 参数：
  * 返回：
  * 备注：
***/
static void RePskInputOk(void)
{
	char buff[512] = {0};
	GetIMEInputBuffWithoutSpaceTrim(buff);//cTmpInputBuff
	if(strlen(buff) > 66)
	{
		strncpy(cTmpInputBuff, buff,65);
	}
	else
	{
		strncpy(cTmpInputBuff, buff,sizeof(buff));
	}
}

/***
  * 功能：
  		调用输入法，显示键盘输入界面
        
  * 参数：
  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int pWiFiPskKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)	
{
	IMEInit("", 60, 0, ReCreateWindow, RePskInputOk, NULL);
	
	return 0;	
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
static int WiFiTextResInfo_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//WiFi密码输入框信息
	pWifiStrApplyInfo = TransString("WIFI_APPLY");
	pWifiStrCancleInfo = TransString("WIFI_CANCEL");
	pWifiStrPskInput = TransString(cTmpInputBuff);
	pWifiStrConnectTips = TransString("WIFI_VERACCOUNT");
	
	pWifiStrSsid = TransString(SaveWifi.ssid);
    return iReturn;
}

/***
  * 功能：
        wifi密码界面文本资源销毁
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度

  * 返回：
  		成功返回0，失败返回非0
  * 备注：
***/
static int WiFiTextResInfo_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    free(pWifiStrPskInput);
    free(pWifiStrCancleInfo);
	free(pWifiStrApplyInfo);
	free(pWifiStrConnectTips);
	free(pWifiStrSsid);
    return iReturn;
}


/***
  * 功能：
        窗体pGuiWifi的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注:
  		密码弹窗界面窗体
***/
int GuiWifiInit(void *pWndObj)
{
	//定义错误标志
	int iReturn = 0;
	

	GUIMESSAGE *pMsg = NULL;

	//获得当前窗体对象
	pGuiWifi = (GUIWINDOW *)pWndObj;
	
    WiFiTextResInfo_Init(NULL, 0, NULL, 0);

	iConnectFlag = 0;

    pWifiInput = CreatePhoto("wifi_mimaBG");
	pWiFiPskInput = CreatePhoto("wifi_mima_input");
    pWifiPskDisplay = CreatePhoto("wifi_mima_display");
    pWifiLblPskInput = CreateLabel(322,204,171,24,pWifiStrPskInput);//密码输入
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblPskInput);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pWifiLblPskInput);

	//SSID显示
	pWifiLblSsid = CreateLabel(322,170,171,24,pWifiStrSsid);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblSsid);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pWifiLblSsid);
	
	//密码提交"取消"
	pWifiCancelInfo = CreatePicture(270,279,123,48,BmpFileDirectory"wifi_mima_btn_unpress.bmp");
	pWifiLblCancelInfo= CreateLabel(270,291,123,24,pWifiStrCancleInfo);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblCancelInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pWifiLblCancelInfo);
	//密码提交"确认"
	pWifiApplyInfo = CreatePicture(416,279,123,48,BmpFileDirectory"wifi_mima_btn_unpress.bmp");
	pWifiLblApplyInfo = CreateLabel(416,291,123,24,pWifiStrApplyInfo);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblApplyInfo);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK),pWifiLblApplyInfo);
	//连接信息提示"连接中"
	pWifiLblConnectTips = CreateLabel(288,250,241,24,pWifiStrConnectTips);
	SetLabelAlign(GUILABEL_ALIGN_CENTER,pWifiLblConnectTips);
	SetLabelFont(getGlobalFnt(EN_FONT_GREEN),pWifiLblConnectTips);
	
	//注册窗体控件
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pGuiWifi,pGuiWifi);
	AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), pWiFiPskInput, pGuiWifi);
	AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), pWifiPskDisplay, pGuiWifi);
	AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), pWifiApplyInfo,pGuiWifi);
	AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), pWifiCancelInfo,pGuiWifi);

	//注册窗体控件的消息处理
	pMsg = GetCurrMessage();
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pWiFiPskInput, pWiFiPskKeyboard, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pWifiPskDisplay, PskDisplayOrNot, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP,pWifiApplyInfo,WiFiBtnApply_Up, &SaveWifi, sizeof(NETWORK_INFO_SINGLE), pMsg); 
	LoginMessageReg(GUIMESSAGE_TCH_DOWN,pWifiApplyInfo,WiFiBtnApply_Down, NULL, 0, pMsg); 
	LoginMessageReg(GUIMESSAGE_TCH_UP,pWifiCancelInfo,WiFiBtnCancel_Up, NULL, 0, pMsg); 
	LoginMessageReg(GUIMESSAGE_TCH_DOWN,pWifiCancelInfo,WiFiBtnCancel_Down, NULL, 0, pMsg); 

	return iReturn;
}

/***
  * 功能：
        窗体pGuiWifi的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		密码弹窗界面窗体
***/
int GuiWifiExit(void *pWndObj)
{
    int iReturn = 0;
	GUIMESSAGE *pMsg = NULL;
	//获取当前窗体
	pGuiWifi = (GUIWINDOW *)pWndObj;
    pMsg = GetCurrMessage();
	//清空消息队列中的消息注册项
    ClearMessageReg(pMsg);
	//销毁窗体注册控件
    ClearWindowComp(pGuiWifi);
	//销毁窗体上的Label
	DestroyLabel(&pWifiLblApplyInfo);
	DestroyLabel(&pWifiLblCancelInfo);
	DestroyLabel(&pWifiLblPskInput);
	DestroyLabel(&pWifiLblSsid);
	DestroyLabel(&pWifiLblConnectTips);
	//销毁窗体上的picture
	DestroyPicture(&pWifiInput);
	DestroyPicture(&pWifiApplyInfo);
	DestroyPicture(&pWifiCancelInfo);
	DestroyPicture(&pWiFiPskInput);
	DestroyPicture(&pWifiPskDisplay);
	
	//释放窗体的文本资源
    WiFiTextResInfo_Exit(NULL, 0, NULL, 0);

	return iReturn;
}


/***
  * 功能：
        窗体pGuiWifi的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		密码弹窗界面窗体
***/
int GuiWifiPaint(void *pWndObj)
{
	int iReturn = 0;
	pGuiWifi = (GUIWINDOW *)pWndObj;
	DispTransparent(0, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//显示桌面控件Picture
	DisplayPicture(pWifiInput);
	DisplayPicture(pWifiApplyInfo);
	DisplayPicture(pWifiCancelInfo);
	DisplayPicture(pWiFiPskInput);
	DisplayPicture(pWifiPskDisplay);
	//显示桌面控件Label	
	DisplayLabel(pWifiLblApplyInfo);
	DisplayLabel(pWifiLblCancelInfo);
	DisplayLabel(pWifiLblPskInput);
	DisplayLabel(pWifiLblSsid);
	
	SetConnectState(EN_WIFI_DISCONNECT);
    SetPowerEnable(2, 1);

	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}

/***
  * 功能：
        窗体pGuiWifi的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		密码弹窗界面窗体
***/
int GuiWifiLoop(void *pWndObj)
{
	
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
	if(iConnectFlag)
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						   FrmWiFiInit, FrmWiFiExit,
						   FrmWiFiPaint, FrmWiFiLoop,
						   FrmWiFiPause, FrmWiFiResume,
						   NULL);	//pWnd由调度线程释放
		SendWndMsg_WindowExit(pGuiWifi);//pGuiWifi
		SendSysMsg_ThreadCreate(pWnd);
	}
	else
	{
		MsecSleep(10);
	}
	
	RefreshWifiDisplay();
		
	return iReturn;
}


/***
  * 功能：
        窗体GuiWifi 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int GuiWifiPause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}


/***
  * 功能：
        	窗体GuiWifi 的恢复函数，进行窗体恢复前预处理
  * 参数：
        	1.void *pWndObj:    指向当前窗体对象
  * 返回：
        	成功返回零，失败返回非零值
  * 备注：
***/
int GuiWifiResume(void *pWndObj)
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

	switch(getConnectState())
	{
		case EN_WIFI_AUTHENTICATOR:
			if(++time == 50)
			{
				switch(refreshDisplay1)
				{
					case 0:
						ChangePromptInfo(TransString("WIFI_VERACCOUNT"));
						break;
					case 1:
						ChangePromptInfo(TransString("WIFI_VERACCOUNT1"));
						break;
					case 2:
						ChangePromptInfo(TransString("WIFI_VERACCOUNT2"));
						break;
					case 3:
						ChangePromptInfo(TransString("WIFI_VERACCOUNT3"));
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
						ChangePromptInfo(TransString("WIFI_GETIP"));
						break;
					case 1:
						ChangePromptInfo(TransString("WIFI_GETIP1"));
						break;
					case 2:
						ChangePromptInfo(TransString("WIFI_GETIP2"));
						break;
					case 3:
						ChangePromptInfo(TransString("WIFI_GETIP3"));
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
        显示密码输入弹窗
  * 参数：
        1.void *pWndObj:  		指向当前窗体对象
        2.NETWORK_INFO_SINGLE WifiInfo:	用户点击连接的wifi信息结构
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		密码弹窗界面窗体
***/
int DisplayPskWindow(void *pWndObj, NETWORK_INFO_SINGLE WifiInfo)
{
	int iReturn = 0;

	GUIWINDOW *pWnd = NULL;
	//得到当前窗体对象
    pWnd = (GUIWINDOW *)pWndObj;
	memcpy(&SaveWifi,&WifiInfo, sizeof(WifiInfo));//SaveWifi 保存用户随机选择的wifi信息

    if(iMutexInitFlag == 0)
    {
        iMutexInitFlag = 1;
        InitMutex(&Connect_State_Mutex, NULL);
    }
	
	memset(cTmpInputBuff,'\0',66);
    
	pGuiWifi = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
									  GuiWifiInit,GuiWifiExit,
									  GuiWifiPaint,GuiWifiLoop,
									  GuiWifiPause,GuiWifiResume,NULL);
	SendWndMsg_WindowExit(pWnd);
	SendSysMsg_ThreadCreate(pGuiWifi);
    
	return iReturn;
}
