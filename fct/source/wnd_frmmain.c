/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmain.c
* 摘    要：  实现主窗体frmmain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
*******************************************************************************/

#include "wnd_frmmain.h"
#include "wnd_frmmainboard.h"
#include "wnd_global.h"
#include "wnd_frmmainopt.h"
// #include "wnd_frmtestkeyboard.h"

/************************
* 窗体frmmain中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pMainStrTitle = NULL;
static GUILABEL *pMainLblTitle = NULL;

//桌面上的文本
static GUICHAR *pMainStrWnd1 = NULL;    //窗体图标1的文本
static GUICHAR *pMainStrWnd2 = NULL;    //窗体图标2的文本
static GUICHAR *pMainStrWnd3 = NULL;    //窗体图标3的文本

//按钮区的文本
//...

//信息栏上文本
static GUICHAR *pMainStrInfo = NULL;    //提示信息的文本

/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmMain = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pMainBarStatus = NULL;
static GUIPICTURE *pMainBgDesk = NULL;
static GUIPICTURE *pMainBarInfo = NULL;

//状态栏上的控件

//桌面上的控件
static GUIPICTURE *pMainIcoWnd1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pMainIcoWnd2 = NULL;     //窗体图标2图形框
static GUIPICTURE *pMainIcoWnd3 = NULL;     //窗体图标3图形框

static GUILABEL *pMainLblWnd1 = NULL;       //窗体图标1标签
static GUILABEL *pMainLblWnd2 = NULL;       //窗体图标2标签
static GUILABEL *pMainLblWnd3 = NULL;       //窗体图标3标签

//信息栏上的控件
static GUILABEL *pMainLblInfo = NULL;       //提示信息标签

//按键控件
// static struct KeyBoard_Control* pKeyBoardControl = NULL;
/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int MainTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int MainTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int MainWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int MainWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int MainIcoWnd1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIcoWnd2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainIcoWnd3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int MainIcoWnd1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainIcoWnd2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainIcoWnd3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int MainErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//按键响应处理
// static void KeyCallBack(int iSelected);

/***
  * 功能：
        窗体frmmain的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmMainInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmMain = (GUIWINDOW *) pWndObj;


    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    MainTextRes_Init(NULL, 0, NULL, 0);
    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pMainBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30,
                                   BmpFileDirectory "bar_status.bmp");
    pMainBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420,
                                BmpFileDirectory "bg_main.bmp");
    pMainBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30,
                                 BmpFileDirectory "bar_info.bmp");
    pMainLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pMainStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblTitle);
    //建立状态栏上的控件
    //建立桌面上的控件
    pMainIcoWnd1 = CreatePicture(170, 170, 80, 80, 
                                 BmpFileDirectory"ico_2.bmp");
    pMainIcoWnd2 = CreatePicture(406, 170, 80, 80, 
                                 BmpFileDirectory"ico_1.bmp");
    pMainIcoWnd3 = CreatePicture(580, 170, 80, 80, 
                                 BmpFileDirectory"ico_8.bmp");

    pMainLblWnd1 = CreateLabel(160, 255, 100, 16, pMainStrWnd1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblWnd1);
    pMainLblWnd2 = CreateLabel(396, 255, 100, 16, pMainStrWnd2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblWnd2);
    pMainLblWnd3 = CreateLabel(550, 255, 140, 16, pMainStrWnd3);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainLblWnd3);
    
    pMainLblInfo = CreateLabel(20, 458, 300, 16, pMainStrInfo);

    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmMain->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmMain, 
                  pFrmMain);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIcoWnd1, 
                 pFrmMain);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIcoWnd2, 
                  pFrmMain);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainIcoWnd3, 
                  pFrmMain);
    
  
    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmMain->Mutex));    //注意，必须释放锁

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmMain, 
                    MainWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmMain, 
                    MainWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIcoWnd1, 
                    MainIcoWnd1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIcoWnd2, 
                    MainIcoWnd2_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainIcoWnd3, 
                    MainIcoWnd3_Down, NULL, 0, pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIcoWnd1, 
                    MainIcoWnd1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIcoWnd2, 
                    MainIcoWnd2_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainIcoWnd3, 
                    MainIcoWnd3_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmMain, 
                    MainErrProc_Func, NULL, 0, pMsg);
    //释放消息队列的互斥锁
    MutexUnlock(&(pMsg->Mutex));    //注意，必须释放锁

	SetPictureEnable(0, pMainIcoWnd3);
	SetLabelEnable(0, pMainLblWnd3);

	//创建按键控件
	// pKeyBoardControl = CreateKeyBoard(320, 0, KeyCallBack);
	// AddKeyBoardControlToWnd(pKeyBoardControl, pFrmMain);

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
int FrmMainExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmMain = (GUIWINDOW *) pWndObj;
	//销毁按键控件
	// DelKeyBoardControlFromWnd(pKeyBoardControl, pFrmMain);
	// DestroyKeyBoardControl(&pKeyBoardControl);
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmMain->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmMain);
    MutexUnlock(&(pFrmMain->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pMainBarStatus);
    DestroyPicture(&pMainBgDesk);
    DestroyPicture(&pMainBarInfo);
    DestroyLabel(&pMainLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyPicture(&pMainIcoWnd1);
    DestroyPicture(&pMainIcoWnd2);
    DestroyPicture(&pMainIcoWnd3);
    
    DestroyLabel(&pMainLblWnd1);
    DestroyLabel(&pMainLblWnd2);
    DestroyLabel(&pMainLblWnd3);
    
    //销毁信息栏上的控件
    DestroyLabel(&pMainLblInfo);

    //释放文本资源
    //***************************************************************/
    MainTextRes_Exit(NULL, 0, NULL, 0);


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
int FrmMainPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pMainBarStatus);
    DisplayPicture(pMainBgDesk);
    DisplayPicture(pMainBarInfo);
    DisplayLabel(pMainLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayPicture(pMainIcoWnd1);
    DisplayPicture(pMainIcoWnd2);
    DisplayPicture(pMainIcoWnd3);
    
    DisplayLabel(pMainLblWnd1);
    DisplayLabel(pMainLblWnd2);
    DisplayLabel(pMainLblWnd3);
   
    //显示信息栏上的控件
    DisplayLabel(pMainLblInfo);

	//显示按键控件
	// DisplayKeyBoardControl(pKeyBoardControl);

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
int FrmMainLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //禁止并停止窗体循环
    SendMsg_DisableLoop(pWndObj);
	
    //更新状态栏显示

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
int FrmMainPause(void *pWndObj)
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
int FrmMainResume(void *pWndObj)
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

    //初始化状态栏上的文本
    pMainStrTitle = TransString("**F216 FCT TEST**");
    //初始化桌面上的文本
    pMainStrWnd1 = TransString("MainBoard_FCT");
    pMainStrWnd2 = TransString("Optical_FCT");
    pMainStrWnd3 = TransString("Software Update");

    pMainStrInfo = TransString("Hello, this is F216_FCT software!");

	return iReturn;
}   


//释放文本资源
static int MainTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
    free(pMainStrTitle);

    //释放桌面上的文本
    free(pMainStrWnd1);
    free(pMainStrWnd2);
    free(pMainStrWnd3);
        
    free(pMainStrInfo);

    return iReturn;
}


static int MainWndKey_Down(void *pInArg, int iInLen, 
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
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int MainWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
    static int iSelected = 0;

    switch (uiValue)
    {
    case KEYCODE_left:
        iSelected = iSelected ? 0 : 1;
        break;
    case KEYCODE_right:
        iSelected = iSelected ? 0 : 1;
        break;
    case KEYCODE_enter:
        if (iSelected)
        {
            MainIcoWnd2_Down(NULL, 0, NULL, 0);
            MainIcoWnd2_Up(NULL, 0, NULL, 0);
        }
        else
        {
            MainIcoWnd1_Down(NULL, 0, NULL, 0);
            MainIcoWnd1_Up(NULL, 0, NULL, 0);
        }
        break;
    default:
        break;
    }

    return iReturn;
}



static int MainIcoWnd1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_2_press.bmp", pMainIcoWnd1);
    DisplayPicture(pMainIcoWnd1);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainLblWnd1);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


static int MainIcoWnd2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_1_press.bmp", pMainIcoWnd2);
    DisplayPicture(pMainIcoWnd2);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainLblWnd2);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


static int MainIcoWnd3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_8_press.bmp", pMainIcoWnd3);
    DisplayPicture(pMainIcoWnd3);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainLblWnd3);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


static int MainIcoWnd1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;

    SetPictureBitmap(BmpFileDirectory"ico_2.bmp", pMainIcoWnd1);
    DisplayPicture(pMainIcoWnd1);
    DisplayLabel(pMainLblWnd1);
	
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmMainBoardInit, FrmMainBoardExit, 
                        FrmMainBoardPaint,FrmMainBoardLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMain);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体
	
    return iReturn;
}

static int MainIcoWnd2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    SetPictureBitmap(BmpFileDirectory"ico_1.bmp", pMainIcoWnd2);
    DisplayPicture(pMainIcoWnd2);
    DisplayLabel(pMainLblWnd2);


    //临时变量定义
    GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmMainOptInit, FrmMainOptExit, 
                        FrmMainOptPaint,FrmMainOptLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMain);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体
	
    return iReturn;
}


static int MainIcoWnd3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    SetPictureBitmap(BmpFileDirectory"ico_8.bmp", pMainIcoWnd3);
    DisplayPicture(pMainIcoWnd3);
    DisplayLabel(pMainLblWnd3);

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

//按键响应处理
// static void KeyCallBack(int iSelected)
// {
// 	static int iKeyFlag = 0;

// 	switch (iSelected)
// 	{
// 	case KEYBOARD_LEFT:
// 		iKeyFlag = iKeyFlag ? 0 : 1;
// 		break;
// 	case KEYBOARD_RIGHT:
// 		iKeyFlag = iKeyFlag ? 0 : 1;
// 		break;
// 	case KEYBOARD_ENTER:
// 		if (iKeyFlag)
// 		{
// 			MainIcoWnd2_Down(NULL, 0, NULL, 0);
// 			MainIcoWnd2_Up(NULL, 0, NULL, 0);
// 		}
// 		else
// 		{
// 			MainIcoWnd1_Down(NULL, 0, NULL, 0);
// 			MainIcoWnd1_Up(NULL, 0, NULL, 0);
// 		}
// 		break;
// 	default:
// 		break;
// 	}
// }
