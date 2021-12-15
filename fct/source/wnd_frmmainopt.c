/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmainopt.c
* 摘    要：  实现主窗体frmmainopt的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-30
*
*******************************************************************************/
#include "wnd_frmmainopt.h"
#include "wnd_frmmain.h"
#include "wnd_frmapd.h"
#include "wnd_frmother.h"
#include "wnd_frmtemp.h"
#include "wnd_frmeeprom.h"
#include "wnd_frmfiber.h"
#include "wnd_global.h"
#include "wnd_frmpulse.h"
// #include "wnd_frmtestkeyboard.h"

#include "common_opm.h"
#include "opm.h"
#include "drvoptic.h"

/************************
* 窗体frmmain中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pMainOptStrTitle = NULL;
static GUILABEL *pMainOptLblTitle = NULL;

//桌面上的文本
static GUICHAR *pMainOptStrWnd1 = NULL;    //窗体图标1的文本
static GUICHAR *pMainOptStrWnd2 = NULL;    //窗体图标2的文本
static GUICHAR *pMainOptStrWnd3 = NULL;    //窗体图标3的文本
static GUICHAR *pMainOptStrWnd4 = NULL;    //窗体图标4的文本
static GUICHAR *pMainOptStrWnd5 = NULL;    //窗体图标5的文本
static GUICHAR *pMainOptStrWnd6 = NULL;    //窗体图标5的文本

//信息栏上文本
static GUICHAR *pMainOptStrInfo = NULL;    //提示信息的文本
static GUILABEL *pMainOptLblInfo = NULL;   //提示信息标签
/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmMainOpt = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pMainOptBarStatus = NULL;
static GUIPICTURE *pMainOptBgDesk = NULL;
static GUIPICTURE *pMainOptBarInfo = NULL;

//按键控件
// static struct KeyBoard_Control* pKeyBoardControl = NULL;
//桌面上的控件
static GUIPICTURE *pMainOptIcoWnd1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pMainOptIcoWnd2 = NULL;     //窗体图标2图形框
static GUIPICTURE *pMainOptIcoWnd3 = NULL;     //窗体图标3图形框
static GUIPICTURE *pMainOptIcoWnd4 = NULL;     //窗体图标4图形框
static GUIPICTURE *pMainOptIcoWnd5 = NULL;     //窗体图标5图形框
static GUIPICTURE *pMainOptIcoWnd6 = NULL;     //窗体图标5图形框

static GUILABEL *pMainOptLblWnd1 = NULL;       //窗体图标1标签
static GUILABEL *pMainOptLblWnd2 = NULL;       //窗体图标2标签
static GUILABEL *pMainOptLblWnd3 = NULL;       //窗体图标3标签
static GUILABEL *pMainOptLblWnd4 = NULL;       //窗体图标4标签
static GUILABEL *pMainOptLblWnd5 = NULL;       //窗体图标5标签
static GUILABEL *pMainOptLblWnd6 = NULL;       //窗体图标5标签

static GUIPICTURE *pMainOptBtnBack = NULL;

/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int MainOptTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int MainOptTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int MainOptWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int MainOptWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int MainOptIcoWnd1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainOptIcoWnd2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainOptIcoWnd3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainOptIcoWnd4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainOptIcoWnd5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainOptIcoWnd6_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int MainOptIcoWnd1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainOptIcoWnd2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainOptIcoWnd3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainOptIcoWnd4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainOptIcoWnd5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int MainOptIcoWnd6_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);


static int MainOptBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int MainOptBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int MainOptErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//按键响应函数
// static void KeyBoardCallBack(int iSelected);
//enter键响应回调函数
static void KeyEnterCallBack(int iKeyFlag);

    /***
  * 功能：
        窗体frmmain的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
    int FrmMainOptInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	
    //得到当前窗体对象
    pFrmMainOpt = (GUIWINDOW *) pWndObj;

    /****************************************************************/
    MainOptTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
	pMainOptBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, BmpFileDirectory"bar_status.bmp");
    pMainOptBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420, BmpFileDirectory "bg_main.bmp");
    pMainOptBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30, BmpFileDirectory "bar_info.bmp");
    pMainOptLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pMainOptStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainOptLblTitle);
    //建立状态栏上的控件
    //建立桌面上的控件
	pMainOptIcoWnd1 = CreatePicture(112, 107, 80, 80, BmpFileDirectory"ico_5.bmp");
    pMainOptIcoWnd2 = CreatePicture(288, 107, 80, 80, BmpFileDirectory "ico_4.bmp");
    pMainOptIcoWnd3 = CreatePicture(112, 294, 80, 80, BmpFileDirectory "ico_6.bmp");
    pMainOptIcoWnd4 = CreatePicture(288, 294, 80, 80, BmpFileDirectory "ico_7.bmp");
    pMainOptIcoWnd5 = CreatePicture(464, 294, 80, 80, BmpFileDirectory "ico_3.bmp");
    pMainOptIcoWnd6 = CreatePicture(464, 107, 80, 80, BmpFileDirectory "ico_8.bmp");

    pMainOptLblWnd1 = CreateLabel(102, 190, 100, 16, pMainOptStrWnd1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainOptLblWnd1);
    pMainOptLblWnd2 = CreateLabel(278, 190, 100, 16, pMainOptStrWnd2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainOptLblWnd2);
    pMainOptLblWnd3 = CreateLabel(102, 377, 100, 16, pMainOptStrWnd3);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainOptLblWnd3);
    pMainOptLblWnd4 = CreateLabel(278, 377, 100, 16, pMainOptStrWnd4);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainOptLblWnd4);
	pMainOptLblWnd5 = CreateLabel(454, 377, 100, 16, pMainOptStrWnd5);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainOptLblWnd5);
	pMainOptLblWnd6 = CreateLabel(454, 190, 100, 16, pMainOptStrWnd6);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pMainOptLblWnd6);

    pMainOptLblInfo = CreateLabel(20, 458, 300, 16, pMainOptStrInfo);

	pMainOptBtnBack = CreatePicture(520, 454, 60, 25, BmpFileDirectory"btn_back_unpress.bmp");

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmMainOpt->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmMainOpt, 
                  pFrmMainOpt);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainOptIcoWnd1, 
                  pFrmMainOpt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainOptIcoWnd2, 
                  pFrmMainOpt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainOptIcoWnd3, 
                  pFrmMainOpt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainOptIcoWnd4, 
                  pFrmMainOpt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainOptIcoWnd5, 
                  pFrmMainOpt);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainOptIcoWnd6, 
                  pFrmMainOpt);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pMainOptBtnBack, 
                  pFrmMainOpt);
    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmMainOpt->Mutex));    //注意，必须释放锁

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmMainOpt, 
                    MainOptWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmMainOpt, 
                    MainOptWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainOptIcoWnd1, 
                    MainOptIcoWnd1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainOptIcoWnd2, 
                    MainOptIcoWnd2_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainOptIcoWnd3, 
                    MainOptIcoWnd3_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainOptIcoWnd4, 
                    MainOptIcoWnd4_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainOptIcoWnd5, 
                    MainOptIcoWnd5_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainOptIcoWnd6, 
                    MainOptIcoWnd6_Down, NULL, 0, pMsg);
	
    
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainOptIcoWnd1, 
                    MainOptIcoWnd1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainOptIcoWnd2, 
                    MainOptIcoWnd2_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainOptIcoWnd3, 
                    MainOptIcoWnd3_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainOptIcoWnd4, 
                    MainOptIcoWnd4_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pMainOptIcoWnd5, 
                    MainOptIcoWnd5_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainOptIcoWnd6, 
                    MainOptIcoWnd6_Up, NULL, 0, pMsg);
	

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pMainOptBtnBack, 
                    MainOptBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pMainOptBtnBack, 
                    MainOptBtnBack_Up, NULL, 0, pMsg);
    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmMainOpt, 
                    MainOptErrProc_Func, NULL, 0, pMsg);
	//创建按键控件
	// pKeyBoardControl = CreateKeyBoard(320, 0, KeyBoardCallBack);
	// AddKeyBoardControlToWnd(pKeyBoardControl, pFrmMainOpt);

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
int FrmMainOptExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmMainOpt = (GUIWINDOW *) pWndObj;
	//销毁按键控件
	// DelKeyBoardControlFromWnd(pKeyBoardControl, pFrmMainOpt);
	// DestroyKeyBoardControl(&pKeyBoardControl);
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmMainOpt->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmMainOpt);
    MutexUnlock(&(pFrmMainOpt->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pMainOptBarStatus);
    DestroyPicture(&pMainOptBgDesk);
    DestroyPicture(&pMainOptBarInfo);
    DestroyLabel(&pMainOptLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyPicture(&pMainOptIcoWnd1);
    DestroyPicture(&pMainOptIcoWnd2);
    DestroyPicture(&pMainOptIcoWnd3);
    DestroyPicture(&pMainOptIcoWnd4);
    DestroyPicture(&pMainOptIcoWnd5);
	DestroyPicture(&pMainOptIcoWnd6);

    DestroyLabel(&pMainOptLblWnd1);
    DestroyLabel(&pMainOptLblWnd2);
    DestroyLabel(&pMainOptLblWnd3);
    DestroyLabel(&pMainOptLblWnd4);
    DestroyLabel(&pMainOptLblWnd5);
	DestroyLabel(&pMainOptLblWnd6);
	DestroyPicture(&pMainOptBtnBack);

    //销毁信息栏上的控件
    DestroyLabel(&pMainOptLblInfo);

    //释放文本资源
    //***************************************************************/
    MainOptTextRes_Exit(NULL, 0, NULL, 0);

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
int FrmMainOptPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pMainOptBarStatus);
    DisplayPicture(pMainOptBgDesk);
    DisplayPicture(pMainOptBarInfo);
    DisplayLabel(pMainOptLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayPicture(pMainOptIcoWnd1);
    DisplayPicture(pMainOptIcoWnd2);
    DisplayPicture(pMainOptIcoWnd3);
    DisplayPicture(pMainOptIcoWnd4);
    DisplayPicture(pMainOptIcoWnd5);
	DisplayPicture(pMainOptIcoWnd6);

    DisplayLabel(pMainOptLblWnd1);
    DisplayLabel(pMainOptLblWnd2);
    DisplayLabel(pMainOptLblWnd3);
    DisplayLabel(pMainOptLblWnd4);
    DisplayLabel(pMainOptLblWnd5);
	DisplayLabel(pMainOptLblWnd6);
    //显示信息栏上的控件
    DisplayLabel(pMainOptLblInfo);
    DisplayPicture(pMainOptBtnBack);
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
int FrmMainOptLoop(void *pWndObj)
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
int FrmMainOptPause(void *pWndObj)
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
int FrmMainOptResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int MainOptTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化状态栏上的文本
    pMainOptStrTitle = TransString("**Optical FCT**");
    //初始化桌面上的文本
    pMainOptStrWnd1 = TransString("APD_FCT");
    pMainOptStrWnd2 = TransString("Other_FCT");
    pMainOptStrWnd3 = TransString("Temperature_FCT");
    pMainOptStrWnd4 = TransString("EEPROM_FCT");
    pMainOptStrWnd5 = TransString("Pulse_FCT");
	pMainOptStrWnd6 = TransString("Fiber_FCT");

    //初始化信息栏上的文本
    pMainOptStrInfo = TransString("");

	return iReturn;
}   


//释放文本资源
static int MainOptTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
    free(pMainOptStrTitle);

    //释放桌面上的文本
    free(pMainOptStrWnd1);
    free(pMainOptStrWnd2);
    free(pMainOptStrWnd3);
    free(pMainOptStrWnd4);
    free(pMainOptStrWnd5);
	free(pMainOptStrWnd6);

    //释放信息栏上的文本
    free(pMainOptStrInfo);

    return iReturn;
}


static int MainOptWndKey_Down(void *pInArg, int iInLen, 
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
		MainOptBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
		MainOptBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    default:
        break;
    }

    return iReturn;
}


static int MainOptWndKey_Up(void *pInArg, int iInLen, 
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
    case KEYCODE_up:
        if (iSelected >= 3)
            iSelected -= 3;
        break;
    case KEYCODE_down:
        if (iSelected < 3)
            iSelected += 3;
        break;
    case KEYCODE_left:
        if (iSelected == 0)
            iSelected = 5;
        else
        {
            if (iSelected > 0)
                iSelected--;
        }
        break;
    case KEYCODE_right:
        if (iSelected == 5)
            iSelected = 0;
        else
        {
            if (iSelected < 5)
                iSelected++;
        }
        break;
    case KEYCODE_enter:
        KeyEnterCallBack(iSelected);
        break;
    case KEYCODE_BACK:
    case KEYCODE_HOME:
		MainOptBtnBack_Up(pInArg, iInLen, pOutArg, iOutLen);
        break;
    default:
        break;
    }


    return iReturn;
}

static int MainOptIcoWnd1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_5_press.bmp", pMainOptIcoWnd1);
    DisplayPicture(pMainOptIcoWnd1);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainOptLblWnd1);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);
	
    return iReturn;
}


static int MainOptIcoWnd2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_4_press.bmp", pMainOptIcoWnd2);
    DisplayPicture(pMainOptIcoWnd2);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainOptLblWnd2);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


static int MainOptIcoWnd3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_6_press.bmp", pMainOptIcoWnd3);
    DisplayPicture(pMainOptIcoWnd3);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainOptLblWnd3);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


static int MainOptIcoWnd4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_7_press.bmp", pMainOptIcoWnd4);
    DisplayPicture(pMainOptIcoWnd4);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainOptLblWnd4);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}

static int MainOptIcoWnd5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_3_press.bmp", pMainOptIcoWnd5);
    DisplayPicture(pMainOptIcoWnd5);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainOptLblWnd5);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}


static int MainOptIcoWnd6_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"ico_8_press.bmp", pMainOptIcoWnd6);
    DisplayPicture(pMainOptIcoWnd6);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x00F8A800, 0xFFFFFFFF, pFont);
    DisplayLabel(pMainOptLblWnd6);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}

static int MainOptIcoWnd1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;
	
    SetPictureBitmap(BmpFileDirectory"ico_5.bmp", pMainOptIcoWnd1);
    DisplayPicture(pMainOptIcoWnd1);
    DisplayLabel(pMainOptLblWnd1);

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmApdInit, FrmApdExit, 
                        FrmApdPaint,FrmApdLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainOpt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

	
    return iReturn;
}


static int MainOptIcoWnd2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;

    SetPictureBitmap(BmpFileDirectory"ico_4.bmp", pMainOptIcoWnd2);
    DisplayPicture(pMainOptIcoWnd2);
    DisplayLabel(pMainOptLblWnd2);
	
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmAttInit, FrmAttExit, 
                        FrmAttPaint,FrmAttLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainOpt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}


static int MainOptIcoWnd3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;
	
  	SetPictureBitmap(BmpFileDirectory"ico_6.bmp", pMainOptIcoWnd3);
    DisplayPicture(pMainOptIcoWnd3);
    DisplayLabel(pMainOptLblWnd3);

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmTempInit, FrmTempExit, 
                        FrmTempPaint,FrmTempLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainOpt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

static int MainOptIcoWnd4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;

    SetPictureBitmap(BmpFileDirectory"ico_7.bmp", pMainOptIcoWnd4);
    DisplayPicture(pMainOptIcoWnd4);
    DisplayLabel(pMainOptLblWnd4);
	
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmEEPROMInit, FrmEEPROMExit, 
                        FrmEEPROMPaint,FrmEEPROMLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainOpt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体
 
    return iReturn;
}


static int MainOptIcoWnd5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    SetPictureBitmap(BmpFileDirectory"ico_3.bmp", pMainOptIcoWnd5);
    DisplayPicture(pMainOptIcoWnd5);
    DisplayLabel(pMainOptLblWnd5);

	
	GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmPulseWidInit, FrmPulseWidExit, 
                        FrmPulseWidPaint,FrmPulseWidLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainOpt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体
	
    return iReturn;
}


static int MainOptIcoWnd6_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    SetPictureBitmap(BmpFileDirectory"ico_8.bmp", pMainOptIcoWnd6);
    DisplayPicture(pMainOptIcoWnd6);
    DisplayLabel(pMainOptLblWnd6);

	GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFiberInit, FrmFiberExit, 
                        FrmFiberPaint,FrmFiberLoop,
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainOpt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体
	
    return iReturn;
}

static int MainOptErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


static int MainOptBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pMainOptBtnBack);
    DisplayPicture(pMainOptBtnBack);

	
    return iReturn;
}

static int MainOptBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    GUIWINDOW *pWnd = NULL; 

	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pMainOptBtnBack);
    DisplayPicture(pMainOptBtnBack);
	
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmMainInit, FrmMainExit, 
                        FrmMainPaint, FrmMainLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmMainOpt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

//enter键响应回调函数
static void KeyEnterCallBack(int iKeyFlag)
{
	switch (iKeyFlag)
	{
	case 0://APD_FCT
 		MainOptIcoWnd1_Down(NULL, 0, NULL, 0);
 		MainOptIcoWnd1_Up(NULL, 0, NULL, 0);
		break;
	case 1://other_fct
 		MainOptIcoWnd2_Down(NULL, 0, NULL, 0);
 		MainOptIcoWnd2_Up(NULL, 0, NULL, 0);
		break;
	case 2://fiber_fct
 		MainOptIcoWnd6_Down(NULL, 0, NULL, 0);
 		MainOptIcoWnd6_Up(NULL, 0, NULL, 0);
		break;
	case 3://temperature_fct
 		MainOptIcoWnd3_Down(NULL, 0, NULL, 0);
 		MainOptIcoWnd3_Up(NULL, 0, NULL, 0);
		break;
	case 4://eeprom_fct
 		MainOptIcoWnd4_Down(NULL, 0, NULL, 0);
 		MainOptIcoWnd4_Up(NULL, 0, NULL, 0);
		break;
	case 5://pulse_fct
 		MainOptIcoWnd5_Down(NULL, 0, NULL, 0);
 		MainOptIcoWnd5_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}

//按键响应函数
// static void KeyBoardCallBack(int iSelected)
// {
// 	static int iKeyFlag = 0;

// 	switch (iSelected)
// 	{
// 	case KEYBOARD_UP:
// 		if (iKeyFlag > 2 && iKeyFlag <= 5)
// 		{
// 			iKeyFlag -= 3;
// 		}
// 		break;
// 	case KEYBOARD_DOWN:
// 		if (iKeyFlag >= 0 && iKeyFlag <= 2)
// 		{
// 			iKeyFlag += 3;
// 		}
// 		break;
// 	case KEYBOARD_LEFT:
// 		if (iKeyFlag == 0)
// 		{
// 			iKeyFlag = 5;
// 		}
// 		else
// 		{
// 			if (iKeyFlag > 0 && iKeyFlag <= 5)
// 			{
// 				iKeyFlag--;
// 			}
// 		}
// 		break;
// 	case KEYBOARD_RIGHT:
// 		if (iKeyFlag == 5)
// 		{
// 			iKeyFlag = 0;
// 		}
// 		else
// 		{
// 			if (iKeyFlag >= 0 && iKeyFlag < 5)
// 			{
// 				iKeyFlag++;
// 			}
// 		}
// 		break;
// 	case KEYBOARD_ENTER:
// 		KeyEnterCallBack(&iKeyFlag);
// 		break;
// 	case KEYBOARD_ESC:
// 		iKeyFlag = 0;
// 		MainOptBtnBack_Down(NULL, 0, NULL, 0);
// 		MainOptBtnBack_Up(NULL, 0, NULL, 0);
// 		break;
// 	default:
// 		break;
// 	}
// }
