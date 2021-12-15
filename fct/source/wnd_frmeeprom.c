/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmeeprom.c
* 摘    要：  实现主窗体frmeeprom的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-30
*
*******************************************************************************/

#include "wnd_frmeeprom.h"
#include "wnd_frmmainopt.h"
#include "wnd_global.h"

#include "common_opm.h"
#include "opm.h"


extern POPM_TUNING  pOpmTunning;

//桌面上的字体
static GUIFONT *pEEPROMFntPoint = NULL;     //标题按钮的字体

/************************
* 窗体frmmain中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pEEPROMStrTitle = NULL;
static GUILABEL *pEEPROMLblTitle = NULL;

//信息栏上文本
static GUICHAR *pEEPROMStrInfo = NULL;    //提示信息的文本

/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmEEPROM = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pEEPROMBarStatus = NULL;
static GUIPICTURE *pEEPROMBgDesk = NULL;
static GUIPICTURE *pEEPROMBarInfo = NULL;

//状态栏上的控件

//桌面上的控件
static GUILABEL *pEEPROMLbl[17];
static GUICHAR *pEEPROMStr[17];

//信息栏上的控件
static GUILABEL *pEEPROMLblInfo = NULL;       //提示信息标签

static GUIPICTURE *pEEPROMBtnBack = NULL;

/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int EEPROMTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int EEPROMTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int EEPROMWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int EEPROMWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int EEPROMBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int EEPROMBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int EEPROMErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/***
  * 功能：
        窗体frmmain的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmEEPROMInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	
    //得到当前窗体对象
    pFrmEEPROM = (GUIWINDOW *) pWndObj;
	
    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    EEPROMTextRes_Init(NULL, 0, NULL, 0);
	
    pEEPROMFntPoint = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pEEPROMBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, BmpFileDirectory"bar_status.bmp");
    pEEPROMBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420, BmpFileDirectory "bg_desk.bmp");
    pEEPROMBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30, BmpFileDirectory "bar_info.bmp");
    pEEPROMLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pEEPROMStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pEEPROMLblTitle);
    //建立状态栏上的控件

    //建立桌面上的控件
  	for(i = 0; i < 9; i++)
  	{
		pEEPROMLbl[i] = CreateLabel(20, (60 + (20 * i)), 300, 20, pEEPROMStr[i]);
		SetLabelFont(pEEPROMFntPoint, pEEPROMLbl[i]);
	}
  	for(i = 9; i < 17; i++)
  	{
		pEEPROMLbl[i] = CreateLabel(340, (60 + (20 * (i - 9))), 300, 20, pEEPROMStr[i]);
		SetLabelFont(pEEPROMFntPoint, pEEPROMLbl[i]);
	}

    //建立信息栏上的控件
    pEEPROMLblInfo = CreateLabel(20, 458, 300, 16, pEEPROMStrInfo);
	pEEPROMBtnBack = CreatePicture(520, 454, 60, 25, BmpFileDirectory"btn_back_unpress.bmp");

    //获得控件队列的互斥锁
    MutexLock(&(pFrmEEPROM->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmEEPROM, 
                  pFrmEEPROM);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pEEPROMBtnBack, 
                  pFrmEEPROM);

    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmEEPROM->Mutex));    //注意，必须释放锁

    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmEEPROM, 
                    EEPROMWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmEEPROM, 
                    EEPROMWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pEEPROMBtnBack, 
                    EEPROMBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pEEPROMBtnBack, 
                    EEPROMBtnBack_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmEEPROM, 
                    EEPROMErrProc_Func, NULL, 0, pMsg);

    //释放消息队列的互斥锁
    MutexUnlock(&(pMsg->Mutex));    //注意，必须释放锁

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmEEPROMExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmEEPROM = (GUIWINDOW *) pWndObj;
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmEEPROM->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmEEPROM);
    MutexUnlock(&(pFrmEEPROM->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pEEPROMBarStatus);
    DestroyPicture(&pEEPROMBgDesk);
    DestroyPicture(&pEEPROMBarInfo);
    DestroyLabel(&pEEPROMLblTitle);
    //销毁状态栏上的控件
    for(i = 0; i < 17; i++)
   	{
		DestroyLabel(&pEEPROMLbl[i]);
	}
    //销毁桌面上的控件
    //销毁按钮区的控件
    //...
    //销毁信息栏上的控件
    DestroyLabel(&pEEPROMLblInfo);
    DestroyPicture(&pEEPROMBtnBack);

    //释放文本资源
    //***************************************************************/
    EEPROMTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmEEPROMPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

    //得到当前窗体对象
    pFrmEEPROM = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pEEPROMBarStatus);
    DisplayPicture(pEEPROMBgDesk);
    DisplayPicture(pEEPROMBarInfo);
    DisplayLabel(pEEPROMLblTitle);
    //显示状态栏上的控件
    for(i = 0; i < 17; i++)
   	{
		DisplayLabel(pEEPROMLbl[i]);
	}
    
    //显示桌面上的控件
    //显示信息栏上的控件
    DisplayLabel(pEEPROMLblInfo);
    DisplayPicture(pEEPROMBtnBack);

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmEEPROMLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendMsg_DisableLoop(pWndObj);
	
    return iReturn;
}


/***
  * 功能：
        窗体frmmain的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmEEPROMPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmmain的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmEEPROMResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int EEPROMTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	char buff[100] = {0};
	int len;
	OPM_TUNING  pOpmTunning;
	
	memset(&pOpmTunning,0,sizeof(OPM_TUNING));
	
	len = sizeof(OPM_TUNING);
 	
 	lseek(opticFD, 0, SEEK_SET);
	read(opticFD, (U08*)&pOpmTunning, len);


    //初始化状态栏上的文本
    pEEPROMStrTitle = TransString("**EEPROM_FCT**");
    //初始化桌面上的文本

	int i = 0 ;

	for(i = 0; i < 4; i++)
	{
		sprintf((char *)buff,"APD[%d]=0x%04x",i,(U16)pOpmTunning.iApd[i]);
		pEEPROMStr[i] = TransString((char *)buff);
	}

	pEEPROMStr[4] = TransString("");
	
	for(i = 0; i < 4; i++)
	{
		sprintf((char *)buff,"VDD[%d]=0x%04x",i,(U16)pOpmTunning.iVdd[i]);
		pEEPROMStr[5+i] = TransString((char *)buff);
	}

	for(i = 0; i < 4; i++)
	{
		sprintf((char *)buff,"fTempCoefficient[%d]=%f",i,pOpmTunning.fTempCoefficient[i]);
		pEEPROMStr[9+i] = TransString((char *)buff);
	}

	pEEPROMStr[13] = TransString("");
	
	sprintf((char *)buff,"fTemperature=%f",pOpmTunning.fTemperature);
	pEEPROMStr[14] = TransString((char *)buff);

	pEEPROMStr[15] = TransString("");

	sprintf((char *)buff,"OffsetPoint=%d",pOpmTunning.OffsetPoint);
	pEEPROMStr[16] = TransString((char *)buff);
	
    pEEPROMStrInfo = TransString("");

    return iReturn;
}


//释放文本资源
static int EEPROMTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

    //释放状态栏上的文本
    free(pEEPROMStrTitle);

    //释放桌面上的文本
    for(i = 0; i < 17; i++)
   	{
		free(pEEPROMStr[i]);
	}
    //释放信息栏上的文本
    free(pEEPROMStrInfo);

    return iReturn;
}


static int EEPROMWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    switch (uiValue)
    {
    case KEYCODE_OTDR:
        break;
    case KEYCODE_BACK:
		EEPROMBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int EEPROMWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    switch (uiValue)
    {
    case KEYCODE_OTDR:
        break;
    case KEYCODE_BACK:
		EEPROMBtnBack_Up(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int EEPROMBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pEEPROMBtnBack);
    DisplayPicture(pEEPROMBtnBack);
	
    return iReturn;
}

static int EEPROMBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pEEPROMBtnBack);
    DisplayPicture(pEEPROMBtnBack);
	
    GUIWINDOW *pWnd = NULL; 
	
    pWnd = CreateWindow(0, 0, 320, 240, 
                        FrmMainOptInit, FrmMainOptExit, 
                        FrmMainOptPaint, FrmMainOptLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmEEPROM);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

static int EEPROMErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    return iReturn;
}