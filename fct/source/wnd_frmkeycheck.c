/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmkeycheck.c
* 摘    要：  实现二级窗体keycheck(按键检查)的窗体处理及相关操作函数。该窗
*             体用于提供检查按键操作是否实现。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
*******************************************************************************/

/*****************************************
* 为实现窗体frmkeycheck而需要引用的其他头文件
*****************************************/

#include "wnd_global.h"
#include "wnd_frmmainboard.h"
#include "wnd_frmkeycheck.h"

/*************************
* 窗体frmkeycheck中的字体资源
************************/


/************************
* 窗体frmkeycheck中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pKeyCheckStrTitle = NULL;
static GUILABEL *pKeyCheckLblTitle = NULL;
/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmKeyCheck = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pKeyCheckBarStatus = NULL;
static GUIPICTURE *pKeyCheckBgDesk = NULL;
static GUIPICTURE *pKeyCheckBarInfo = NULL;

//桌面上的控件 12个按钮 12文本 12个标签
static GUIPICTURE *pKeyCheckBtnShift = NULL;			//shift
static GUIPICTURE *pKeyCheckBtnOtdrLeft = NULL;			//otdr left
static GUIPICTURE *pKeyCheckBtnOtdrRight = NULL;		//otdr right
static GUIPICTURE *pKeyCheckBtnStart = NULL;			//start
static GUIPICTURE *pKeyCheckBtnUp = NULL;				//up
static GUIPICTURE *pKeyCheckBtnDown = NULL;				//down
static GUIPICTURE *pKeyCheckBtnLeft = NULL;				//left
static GUIPICTURE *pKeyCheckBtnRight = NULL;			//right
static GUIPICTURE *pKeyCheckBtnEnter = NULL;			//enter
static GUIPICTURE *pKeyCheckBtnFile = NULL;				//file
static GUIPICTURE *pKeyCheckBtnVfl = NULL;				//vfl
static GUIPICTURE *pKeyCheckBtnEsc = NULL;				//esc
//文本资源
static GUICHAR *pKeyCheckStrShift = NULL;				//shift
static GUICHAR *pKeyCheckStrOtdrLeft = NULL;			//OtdrLeft
static GUICHAR *pKeyCheckStrOtdrRight = NULL;			//OtdrRight
static GUICHAR *pKeyCheckStrStart = NULL;				//start
static GUICHAR *pKeyCheckStrUp = NULL;					//up
static GUICHAR *pKeyCheckStrDown = NULL;				//down
static GUICHAR *pKeyCheckStrLeft = NULL;				//left
static GUICHAR *pKeyCheckStrRight = NULL;				//right
static GUICHAR *pKeyCheckStrEnter = NULL;				//enter
static GUICHAR *pKeyCheckStrFile = NULL;				//file
static GUICHAR *pKeyCheckStrVfl = NULL;					//vfl
static GUICHAR *pKeyCheckStrEsc = NULL;					//Esc
//文本
static GUILABEL *pKeyCheckLblShift = NULL;				//shift
static GUILABEL *pKeyCheckLblOtdrLeft = NULL;			//OtdrLeft
static GUILABEL *pKeyCheckLblOtdrRight = NULL;			//OtdrRight
static GUILABEL *pKeyCheckLblStart = NULL;				//start
static GUILABEL *pKeyCheckLblUp = NULL;
static GUILABEL *pKeyCheckLblDown = NULL;
static GUILABEL *pKeyCheckLblLeft = NULL;
static GUILABEL *pKeyCheckLblRight = NULL;
static GUILABEL *pKeyCheckLblEnter = NULL;
static GUILABEL *pKeyCheckLblFile = NULL;
static GUILABEL *pKeyCheckLblVfl = NULL;
static GUILABEL *pKeyCheckLblEsc = NULL;
//信息栏上控件
static GUIPICTURE *pKeyCheckBtnBack = NULL;

/*********************************************************************************************************
* 窗体frmkeycheck中的文本资源处理函数
*********************************************************************************************************/
//初始化文本资源
static int KeyCheckTextRes_Init(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
//释放文本资源
static int KeyCheckTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
                               
/***********************************
* 窗体frmkeycheck中的控件事件处理函数
***********************************/
//窗体的按键事件处理函数
static int KeyCheckWndKey_Down(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
static int KeyCheckWndKey_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
/****************************
* 信息栏控件的事件处理函数
****************************/
static int KeyCheckBtnBack_Down(void *pInArg,  int iInLen, 
                               void *pOutArg, int iOutLen);

static int KeyCheckBtnBack_Up(void *pInArg,  int iInLen, 
                               void *pOutArg, int iOutLen);                              
/********************************
* 窗体中的错误事件处理函数
********************************/
static int KeyCheckErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


int FrmKeyCheckInit(void *pWndObj)
{

    int iReturn = 0;
	GUIMESSAGE *pMsg = NULL;
    pFrmKeyCheck = (GUIWINDOW *)pWndObj;
	  
	  //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    KeyCheckTextRes_Init(NULL, 0, NULL, 0);
    
      //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
	pKeyCheckBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, BmpFileDirectory"bar_status.bmp");
    pKeyCheckBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420, BmpFileDirectory "bg_desk.bmp");
    pKeyCheckBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30, BmpFileDirectory "bar_info.bmp");
    pKeyCheckLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pKeyCheckStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblTitle);

    //建立桌面上的控件
    pKeyCheckBtnShift = CreatePicture(40, 30, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnOtdrLeft = CreatePicture(200, 30, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnOtdrRight = CreatePicture(360, 30, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnStart = CreatePicture(520, 30, 80, 36, BmpFileDirectory "enter_unpress.bmp");

    pKeyCheckBtnUp = CreatePicture(280, 126, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnDown = CreatePicture(280, 318, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnLeft = CreatePicture(120, 222, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnRight = CreatePicture(440, 222, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnEnter = CreatePicture(280, 222, 80, 36, BmpFileDirectory "enter_unpress.bmp");

    pKeyCheckBtnFile = CreatePicture(67, 414, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnVfl = CreatePicture(280, 414, 80, 36, BmpFileDirectory "enter_unpress.bmp");
    pKeyCheckBtnEsc = CreatePicture(493, 414, 80, 36, BmpFileDirectory "enter_unpress.bmp");

    pKeyCheckBtnBack = CreatePicture(520, 454, 60, 25, BmpFileDirectory"btn_back_unpress.bmp");

    pKeyCheckLblShift = CreateLabel(40, 30+10, 80, 16, pKeyCheckStrShift);
    pKeyCheckLblOtdrLeft = CreateLabel(200, 30+10, 80, 36, pKeyCheckStrOtdrLeft);
    pKeyCheckLblOtdrRight = CreateLabel(360, 30+10, 80, 36, pKeyCheckStrOtdrRight);
    pKeyCheckLblStart = CreateLabel(520, 30+10, 80, 36, pKeyCheckStrStart);

    pKeyCheckLblUp = CreateLabel(280, 126+10, 80, 36, pKeyCheckStrUp);
    pKeyCheckLblDown = CreateLabel(280, 318+10, 80, 36, pKeyCheckStrDown);
    pKeyCheckLblLeft = CreateLabel(120, 222+10, 80, 36, pKeyCheckStrLeft);
    pKeyCheckLblRight = CreateLabel(440, 222+10, 80, 36, pKeyCheckStrRight);
    pKeyCheckLblEnter = CreateLabel(280, 222+10, 80, 36, pKeyCheckStrEnter);

    pKeyCheckLblFile = CreateLabel(67, 414+10, 80, 36, pKeyCheckStrFile);
    pKeyCheckLblVfl = CreateLabel(280, 414+10, 80, 36, pKeyCheckStrVfl);
    pKeyCheckLblEsc = CreateLabel(493, 414+10, 80, 36, pKeyCheckStrEsc);

    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblShift);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblOtdrLeft);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblOtdrRight);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblStart);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblUp);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblDown);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblLeft);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblRight);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblEnter);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblFile);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblVfl);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pKeyCheckLblEsc);
    //获得控件队列的互斥锁
    MutexLock(&(pFrmKeyCheck->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmKeyCheck, 
                  pFrmKeyCheck);
    //注册桌面上的控件
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnShift,
				pFrmKeyCheck);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnOtdrLeft,
				pFrmKeyCheck);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnOtdrRight,
				pFrmKeyCheck);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnStart,
				pFrmKeyCheck);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnUp, 
                  pFrmKeyCheck);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnDown, 
                  pFrmKeyCheck);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnLeft, 
                  pFrmKeyCheck);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnRight, 
                  pFrmKeyCheck);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnEnter, 
                  pFrmKeyCheck);

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnFile,
				pFrmKeyCheck);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnVfl,
				pFrmKeyCheck);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnEsc,
				pFrmKeyCheck);

     //注册信息栏上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pKeyCheckBtnBack, 
                  pFrmKeyCheck);
     //释放控件队列的互斥锁
    MutexUnlock(&(pFrmKeyCheck->Mutex));    //注意，必须释放锁
    
    pMsg = GetCurrMessage();
      //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmKeyCheck, 
                    KeyCheckWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmKeyCheck, 
                    KeyCheckWndKey_Up, NULL, 0, pMsg);	

    //注册信息栏上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pKeyCheckBtnBack, 
                    KeyCheckBtnBack_Down, NULL, 0, pMsg); //注册信息栏上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_UP, pKeyCheckBtnBack, 
                    KeyCheckBtnBack_Up, NULL, 0, pMsg);
     //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmKeyCheck, 
                    KeyCheckErrProc_Func, NULL, 0, pMsg);

     //释放消息队列的互斥锁
    MutexUnlock(&(pMsg->Mutex));    //注意，必须释放锁


    return iReturn;
}

int FrmKeyCheckExit(void *pWndObj)
{
	 int iReturn = 0;
	 GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmKeyCheck = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁
    
    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmKeyCheck->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmKeyCheck);
    MutexUnlock(&(pFrmKeyCheck->Mutex));    //注意，必须释放锁
    
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pKeyCheckBarStatus);
    DestroyPicture(&pKeyCheckBgDesk);
    DestroyPicture(&pKeyCheckBarInfo);
    DestroyLabel(&pKeyCheckLblTitle);
    
     //销毁桌面上的控件
    DestroyPicture(&pKeyCheckBtnShift);
    DestroyPicture(&pKeyCheckBtnOtdrLeft);
    DestroyPicture(&pKeyCheckBtnOtdrRight);
	DestroyPicture(&pKeyCheckBtnStart);
    DestroyPicture(&pKeyCheckBtnUp);
    DestroyPicture(&pKeyCheckBtnDown);
    DestroyPicture(&pKeyCheckBtnLeft);
    DestroyPicture(&pKeyCheckBtnRight);
    DestroyPicture(&pKeyCheckBtnEnter);
	DestroyPicture(&pKeyCheckBtnFile);
	DestroyPicture(&pKeyCheckBtnVfl);
	DestroyPicture(&pKeyCheckBtnEsc);

    DestroyLabel(&pKeyCheckLblShift);
    DestroyLabel(&pKeyCheckLblOtdrLeft);
    DestroyLabel(&pKeyCheckLblOtdrRight);
	DestroyLabel(&pKeyCheckLblStart);
    DestroyLabel(&pKeyCheckLblUp);
    DestroyLabel(&pKeyCheckLblDown);
    DestroyLabel(&pKeyCheckLblLeft);
    DestroyLabel(&pKeyCheckLblRight);
    DestroyLabel(&pKeyCheckLblEnter);
	DestroyLabel(&pKeyCheckLblFile);
	DestroyLabel(&pKeyCheckLblVfl);
	DestroyLabel(&pKeyCheckLblEsc);

    DestroyPicture(&pKeyCheckBtnBack);
    //释放文本资源
    //***************************************************************/
    KeyCheckTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}

int FrmKeyCheckPaint(void *pWndObj)
{
    int iReturn = 0;
    //得到当前窗体对象
    pFrmKeyCheck= (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pKeyCheckBarStatus);
    DisplayPicture(pKeyCheckBgDesk);
    DisplayPicture(pKeyCheckBarInfo);
	DisplayLabel(pKeyCheckLblTitle);
		
    //显示桌面上的控件
    DisplayPicture(pKeyCheckBtnShift);
 	DisplayPicture(pKeyCheckBtnOtdrLeft);
    DisplayPicture(pKeyCheckBtnOtdrRight);
	DisplayPicture(pKeyCheckBtnStart);
    DisplayPicture(pKeyCheckBtnUp);
    DisplayPicture(pKeyCheckBtnDown);
    DisplayPicture(pKeyCheckBtnLeft);
    DisplayPicture(pKeyCheckBtnRight);
    DisplayPicture(pKeyCheckBtnEnter);
	DisplayPicture(pKeyCheckBtnFile);
	DisplayPicture(pKeyCheckBtnVfl);
	DisplayPicture(pKeyCheckBtnEsc);

    DisplayLabel(pKeyCheckLblShift);
    DisplayLabel(pKeyCheckLblOtdrLeft);
    DisplayLabel(pKeyCheckLblOtdrRight);
	DisplayLabel(pKeyCheckLblStart);
    DisplayLabel(pKeyCheckLblUp);
    DisplayLabel(pKeyCheckLblDown);
    DisplayLabel(pKeyCheckLblLeft);
    DisplayLabel(pKeyCheckLblRight);
    DisplayLabel(pKeyCheckLblEnter);
	DisplayLabel(pKeyCheckLblFile);
	DisplayLabel(pKeyCheckLblVfl);
	DisplayLabel(pKeyCheckLblEsc);

    DisplayPicture(pKeyCheckBtnBack);

    return iReturn;
}

int FrmKeyCheckLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    return iReturn;
}

int FrmKeyCheckPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}

int FrmKeyCheckResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}

//初始化文本资源
static int KeyCheckTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化状态栏上的文本
    pKeyCheckStrTitle = TransString("Key Check");
    //初始化桌面上的文本
    pKeyCheckStrShift = TransString("Shift");
    pKeyCheckStrOtdrLeft = TransString("OtdrLeft");
    pKeyCheckStrOtdrRight = TransString("OtdrRight");
	pKeyCheckStrStart = TransString("Start/Stop");
    pKeyCheckStrUp = TransString("Up");
    pKeyCheckStrDown = TransString("Down");
    pKeyCheckStrLeft = TransString("Left");
    pKeyCheckStrRight = TransString("Right");
    pKeyCheckStrEnter = TransString("Enter");
	pKeyCheckStrFile = TransString("File");
	pKeyCheckStrVfl = TransString("Vfl");
	pKeyCheckStrEsc = TransString("Esc");

    return iReturn;
}

//释放文本资源
static int KeyCheckTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
    int iReturn = 0;
    //释放状态栏上的文本
    free(pKeyCheckStrTitle);
    //释放桌面上的文本
    free(pKeyCheckStrShift);
    free(pKeyCheckStrOtdrLeft);
    free(pKeyCheckStrOtdrRight);
	free(pKeyCheckStrStart);
    free(pKeyCheckStrUp);
    free(pKeyCheckStrDown);
    free(pKeyCheckStrLeft);
    free(pKeyCheckStrRight);
    free(pKeyCheckStrEnter);
	free(pKeyCheckStrFile);
	free(pKeyCheckStrVfl);
	free(pKeyCheckStrEsc);

    return iReturn;
}

static int ShowKeyCheckBtn_FuncDown_Key(GUIPICTURE *pBtnFx, GUILABEL *pLblFx)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIFONT *pFont;
    unsigned int uiColor;

    SetPictureBitmap(BmpFileDirectory"enter_press.bmp", pBtnFx);
    DisplayPicture(pBtnFx);

    pFont = GetCurrFont();
    uiColor = pFont->uiFgColor;
    SetFontColor(0x003C3028, 0xFFFFFFFF, pFont);
    DisplayLabel(pLblFx);
    SetFontColor(uiColor, 0xFFFFFFFF, pFont);

    return iReturn;
}

static int ShowKeyCheckBtn_FuncUp_Key(GUIPICTURE *pBtnFx, GUILABEL *pLblFx)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

    SetPictureBitmap(BmpFileDirectory"enter_unpress.bmp", pBtnFx);
    DisplayPicture(pBtnFx);

    DisplayLabel(pLblFx);

    return iReturn;
}

//按键松开处理函数
static int KeyCheckWndKey_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    switch (uiValue)
    {
    case KEYCODE_SHIFT:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnShift, pKeyCheckLblShift);
        break;
    case KEYCODE_OTDR_LEFT:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnOtdrLeft, pKeyCheckLblOtdrLeft);
        break;
    case KEYCODE_OTDR_RIGHT:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnOtdrRight, pKeyCheckLblOtdrRight);
        break;
    case KEYCODE_Start:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnStart, pKeyCheckLblStart);
        break;
    case KEYCODE_up:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnUp, pKeyCheckLblUp);
        break;
    case KEYCODE_down:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnDown, pKeyCheckLblDown);
        break;
    case KEYCODE_left:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnLeft, pKeyCheckLblLeft);
        break;
    case KEYCODE_right:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnRight, pKeyCheckLblRight);
        break;
    case KEYCODE_enter:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnEnter, pKeyCheckLblEnter);
        break;
    case KEYCODE_VFL:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnVfl, pKeyCheckLblVfl);
        break;
    case KEYCODE_File:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnFile, pKeyCheckLblFile);
        break;
    case KEYCODE_BACK:
        ShowKeyCheckBtn_FuncUp_Key(pKeyCheckBtnEsc, pKeyCheckLblEsc);
        MsecSleep(10);
        KeyCheckBtnBack_Up(NULL, 0, NULL, 0);
        break;
    default:
        break;
    }

    return iReturn;
}


//键按下处理函数
static int KeyCheckWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;

    switch (uiValue)
    {
    case KEYCODE_SHIFT:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnShift, pKeyCheckLblShift);
        break;
    case KEYCODE_OTDR_LEFT:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnOtdrLeft, pKeyCheckLblOtdrLeft);
        break;
    case KEYCODE_OTDR_RIGHT:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnOtdrRight, pKeyCheckLblOtdrRight);
        break;
    case KEYCODE_Start:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnStart, pKeyCheckLblStart);
        break;
    case KEYCODE_up:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnUp, pKeyCheckLblUp);
        break;
    case KEYCODE_down:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnDown, pKeyCheckLblDown);
        break;
    case KEYCODE_left:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnLeft, pKeyCheckLblLeft);
        break;
    case KEYCODE_right:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnRight, pKeyCheckLblRight);
        break;
    case KEYCODE_enter:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnEnter, pKeyCheckLblEnter);
        break;
    case KEYCODE_VFL:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnVfl, pKeyCheckLblVfl);
        break;
    case KEYCODE_File:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnFile, pKeyCheckLblFile);
        break;
    case KEYCODE_BACK:
        ShowKeyCheckBtn_FuncDown_Key(pKeyCheckBtnEsc, pKeyCheckLblEsc);
        MsecSleep(10);
        KeyCheckBtnBack_Down(NULL, 0, NULL, 0);
        break;
    default:
        break;
    }

    return iReturn;
}

static int KeyCheckBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pKeyCheckBtnBack);
    DisplayPicture(pKeyCheckBtnBack);

    return iReturn;
}

static int KeyCheckBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pKeyCheckBtnBack);
    DisplayPicture(pKeyCheckBtnBack);
	
    GUIWINDOW *pWnd = NULL; 
	
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmMainBoardInit, FrmMainBoardExit, 
                        FrmMainBoardPaint, FrmMainBoardLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmKeyCheck);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}



static int KeyCheckErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}