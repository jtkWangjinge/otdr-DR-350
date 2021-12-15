/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmpulse.c
* 摘    要：  实现主窗体frmpulse的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-30
*******************************************************************************/
#include "wnd_frmpulse.h"
#include "wnd_global.h"
#include "wnd_frmmainopt.h"
#include "wnd_frmtestkeyboard.h"

#include "common_opm.h"
#include "drvoptic.h"
#include "opm.h"


//按钮区的字体
static GUIFONT *pPulseWidFntRight = NULL;     //标题按钮的字体
/************************
* 窗体frmmain中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pPulseWidStrTitle = NULL;
static GUILABEL *pPulseWidLblTitle = NULL;

//桌面上的文本
static GUICHAR *pPulseWidStrF1 = NULL;    //窗体图标1的文本
static GUICHAR *pPulseWidStrF2 = NULL;    //窗体图标2的文本
static GUICHAR *pPulseWidStrF3 = NULL;    //窗体图标3的文本
static GUICHAR *pPulseWidStrF4 = NULL;    //窗体图标4的文本
static GUICHAR *pPulseWidStrF5 = NULL;    //窗体图标4的文本

//信息栏上文本
static GUICHAR *pPulseWidStrInfo = NULL;    //提示信息的文本

/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmPulseWid = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pPulseWidBarStatus = NULL;
static GUIPICTURE *pPulseWidBgDesk = NULL;
static GUIPICTURE *pPulseWidBarInfo = NULL;

//状态栏上的控件

//桌面上的控件
static GUIPICTURE *pPulseWidBtnF1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pPulseWidBtnF2 = NULL;     //窗体图标2图形框
static GUIPICTURE *pPulseWidBtnF3 = NULL;     //窗体图标3图形框
static GUIPICTURE *pPulseWidBtnF4 = NULL;     //窗体图标4图形框
static GUIPICTURE *pPulseWidBtnF5 = NULL;     //窗体图标4图形框


static GUILABEL *pPulseWidLblF1 = NULL;       //窗体图标1标签
static GUILABEL *pPulseWidLblF2 = NULL;       //窗体图标2标签
static GUILABEL *pPulseWidLblF3 = NULL;       //窗体图标3标签
static GUILABEL *pPulseWidLblF4 = NULL;       //窗体图标4标签
static GUILABEL *pPulseWidLblF5 = NULL;       //窗体图标4标签

//数据显示区
static GUIPICTURE *pPulseWidPicDataDispaly = NULL;
static GUILABEL *pPulseWidLblDataDispaly = NULL;
static GUICHAR *pPulseWidStrDataDispaly = NULL;    

//信息栏上的控件
static GUILABEL *pPulseWidLblInfo = NULL;       //提示信息标签

static GUIPICTURE *pPulseWidBtnBack = NULL;

/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int PulseWidTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int PulseWidTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int PulseWidWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int PulseWidWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int PulseWidBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int PulseWidBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int PulseWidBtnF3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int PulseWidBtnF4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int PulseWidBtnF5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int PulseWidBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int PulseWidBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int PulseWidBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int PulseWidBtnF3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int PulseWidBtnF4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int PulseWidBtnF5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int PulseWidBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int PulseWidErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int PulseMode(int iAmp,int iApd,int iPulse,int iWave);
static int ShowWait(void);
//enter键响应处理函数
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
int FrmPulseWidInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	
    //得到当前窗体对象
    pFrmPulseWid = (GUIWINDOW *) pWndObj;


    //初始化字体资源
    pPulseWidFntRight = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    PulseWidTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pPulseWidBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, BmpFileDirectory"bar_status.bmp");
    pPulseWidBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420, BmpFileDirectory "bg_desk.bmp");
    pPulseWidBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30, BmpFileDirectory "bar_info.bmp");
    pPulseWidLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pPulseWidStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pPulseWidLblTitle);
	

    //建立桌面上的控件
    pPulseWidBtnF1 = CreatePicture(500, 35, 80, 53,
                                 BmpFileDirectory"btn_enable.bmp");
    pPulseWidBtnF2 = CreatePicture(500, 95, 80, 53,
                                   BmpFileDirectory "btn_enable.bmp");
    pPulseWidBtnF3 = CreatePicture(500, 155, 80, 53,
                                   BmpFileDirectory "btn_enable.bmp");
    pPulseWidBtnF4 = CreatePicture(500, 215, 80, 53,
                                   BmpFileDirectory "btn_enable.bmp");
    pPulseWidBtnF5 = CreatePicture(500, 275, 80, 53,
                                   BmpFileDirectory "btn_enable.bmp");

    pPulseWidLblF1 = CreateLabel(505, 44, 80, 53, pPulseWidStrF1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pPulseWidLblF1);
    pPulseWidLblF2 = CreateLabel(505, 104, 80, 53, pPulseWidStrF2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pPulseWidLblF2);
    pPulseWidLblF3 = CreateLabel(505, 164, 80, 53, pPulseWidStrF3);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pPulseWidLblF3);
    pPulseWidLblF4 = CreateLabel(505, 224, 80, 53, pPulseWidStrF4);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pPulseWidLblF4);
    pPulseWidLblF5 = CreateLabel(505, 284, 80, 53, pPulseWidStrF5);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pPulseWidLblF5);

	pPulseWidPicDataDispaly = CreatePicture(5, 30, 400, 400,
                                 BmpFileDirectory"frm_dataInfo1.bmp");
	pPulseWidLblDataDispaly = CreateLabel(10, 176, 400, 16, pPulseWidStrDataDispaly);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pPulseWidLblDataDispaly);

	pPulseWidBtnBack = CreatePicture(520, 454, 60, 25,
                                     BmpFileDirectory"btn_back_unpress.bmp");
    //建立按钮区的控件
    //...
    //建立信息栏上的控件
    pPulseWidLblInfo = CreateLabel(20, 458, 300, 16, pPulseWidStrInfo);


    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmPulseWid->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmPulseWid, 
                  pFrmPulseWid);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pPulseWidBtnF1, 
                  pFrmPulseWid);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pPulseWidBtnF2, 
                  pFrmPulseWid);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pPulseWidBtnF3, 
                  pFrmPulseWid);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pPulseWidBtnF4, 
                  pFrmPulseWid);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pPulseWidBtnF5, 
                  pFrmPulseWid);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pPulseWidBtnBack, 
                  pFrmPulseWid);

    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmPulseWid->Mutex));    //注意，必须释放锁

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmPulseWid, 
                    PulseWidWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmPulseWid, 
                    PulseWidWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pPulseWidBtnF1, 
                    PulseWidBtnF1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pPulseWidBtnF2, 
                    PulseWidBtnF2_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pPulseWidBtnF3, 
                    PulseWidBtnF3_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pPulseWidBtnF4, 
                    PulseWidBtnF4_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pPulseWidBtnF5, 
                    PulseWidBtnF5_Down, NULL, 0, pMsg);
			 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pPulseWidBtnBack, 
                    PulseWidBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pPulseWidBtnF1, 
                    PulseWidBtnF1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pPulseWidBtnF2, 
                    PulseWidBtnF2_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pPulseWidBtnF3, 
                    PulseWidBtnF3_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pPulseWidBtnF4, 
                    PulseWidBtnF4_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pPulseWidBtnF5, 
                    PulseWidBtnF5_Up, NULL, 0, pMsg);
  
    LoginMessageReg(GUIMESSAGE_TCH_UP, pPulseWidBtnBack, 
                    PulseWidBtnBack_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmPulseWid, 
                    PulseWidErrProc_Func, NULL, 0, pMsg);

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
int FrmPulseWidExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmPulseWid = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmPulseWid->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmPulseWid);
    MutexUnlock(&(pFrmPulseWid->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pPulseWidBarStatus);
    DestroyPicture(&pPulseWidBgDesk);
    DestroyPicture(&pPulseWidBarInfo);
    DestroyLabel(&pPulseWidLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyPicture(&pPulseWidBtnF1);
    DestroyPicture(&pPulseWidBtnF2);
    DestroyPicture(&pPulseWidBtnF3);
    DestroyPicture(&pPulseWidBtnF4);
	DestroyPicture(&pPulseWidBtnF5);

    DestroyLabel(&pPulseWidLblF1);
    DestroyLabel(&pPulseWidLblF2);
    DestroyLabel(&pPulseWidLblF3);
    DestroyLabel(&pPulseWidLblF4);
	DestroyLabel(&pPulseWidLblF5);

    DestroyPicture(&pPulseWidPicDataDispaly);
    DestroyLabel(&pPulseWidLblDataDispaly);
    //销毁信息栏上的控件
    DestroyLabel(&pPulseWidLblInfo);
	
    DestroyPicture(&pPulseWidBtnBack);

    //释放文本资源
    //***************************************************************/
    PulseWidTextRes_Exit(NULL, 0, NULL, 0);


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
int FrmPulseWidPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmPulseWid = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pPulseWidBarStatus);
    DisplayPicture(pPulseWidBgDesk);
    DisplayPicture(pPulseWidBarInfo);
    DisplayLabel(pPulseWidLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayPicture(pPulseWidBtnF1);
    DisplayPicture(pPulseWidBtnF2);
    DisplayPicture(pPulseWidBtnF3);
    DisplayPicture(pPulseWidBtnF4);
	DisplayPicture(pPulseWidBtnF5);

    DisplayLabel(pPulseWidLblF1);
    DisplayLabel(pPulseWidLblF2);
    DisplayLabel(pPulseWidLblF3);
    DisplayLabel(pPulseWidLblF4);
	DisplayLabel(pPulseWidLblF5);
 
    //...
    DisplayPicture(pPulseWidPicDataDispaly);
    DisplayLabel(pPulseWidLblDataDispaly);
    //显示信息栏上的控件
    DisplayLabel(pPulseWidLblInfo);
    DisplayPicture(pPulseWidBtnBack);

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
int FrmPulseWidLoop(void *pWndObj)
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
int FrmPulseWidPause(void *pWndObj)
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
int FrmPulseWidResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int PulseWidTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化状态栏上的文本
    pPulseWidStrTitle = TransString("**PULSE FCT**");
    //初始化桌面上的文本
    pPulseWidStrF1 = TransString("PulseMode 1");
    pPulseWidStrF2 = TransString("PulseMode 2");
    pPulseWidStrF3 = TransString("PulseMode 3");
    pPulseWidStrF4 = TransString("PulseMode 4");
	pPulseWidStrF5 = TransString("PulseMode 5");

    //初始化信息栏上的文本
    pPulseWidStrInfo = TransString("");

    pPulseWidStrDataDispaly = TransString("");

    return iReturn;
}


//释放文本资源
static int PulseWidTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
    free(pPulseWidStrTitle);
    //释放桌面上的文本
    free(pPulseWidStrF1);
    free(pPulseWidStrF2);
    free(pPulseWidStrF3);
    free(pPulseWidStrF4);
	free(pPulseWidStrF5);

    //释放按钮区的文本
    //...
    free(pPulseWidStrDataDispaly);
    //释放信息栏上的文本
    free(pPulseWidStrInfo);

    return iReturn;
}


static int PulseWidWndKey_Down(void *pInArg, int iInLen, 
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
		PulseWidBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int PulseWidWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
    static int iKeyFlag = 0;

    switch (uiValue)
    {
    case KEYCODE_up:
        if (iKeyFlag > 0)
        {
            iKeyFlag--;
        }
        break;
    case KEYCODE_down:
        if (iKeyFlag >= 0 && iKeyFlag < 4)
        {
            iKeyFlag++;
        }
        break;
    case KEYCODE_enter:
        KeyEnterCallBack(iKeyFlag);
        break;
    case KEYCODE_BACK:
        PulseWidBtnBack_Up(NULL, 0, NULL, 0);
        break;
    default:
        break;
    }

    return iReturn;
}

static int PulseWidBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pPulseWidBtnF1, pPulseWidLblF1);

    return iReturn;
}


static int PulseWidBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pPulseWidBtnF2, pPulseWidLblF2);

    return iReturn;
}


static int PulseWidBtnF3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pPulseWidBtnF3, pPulseWidLblF3);

    return iReturn;
}


static int PulseWidBtnF4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pPulseWidBtnF4, pPulseWidLblF4);

    return iReturn;
}

static int PulseWidBtnF5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pPulseWidBtnF5, pPulseWidLblF5);

    return iReturn;
}


static int PulseWidBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
    ShowBtn_FuncUp(pPulseWidBtnF1, pPulseWidLblF1);

    ShowWait();
    PulseMode(0,0,200,0);	

	if(pPulseWidStrDataDispaly != NULL)
	{
		free(pPulseWidStrDataDispaly);
		pPulseWidStrDataDispaly = NULL;
	}
    pPulseWidStrDataDispaly = TransString("Pusle Mode1: 1310nm, 0x16, APD 43v, Pulse 200ns");
    SetLabelText(pPulseWidStrDataDispaly,pPulseWidLblDataDispaly);
    SetLabelFont(pPulseWidFntRight, pPulseWidLblDataDispaly); 
    DisplayPicture(pPulseWidPicDataDispaly);
    DisplayLabel(pPulseWidLblDataDispaly);
	
    return iReturn;
}


static int PulseWidBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncUp(pPulseWidBtnF2, pPulseWidLblF2);

    ShowWait();
    PulseMode(1,0,100,0);	

	if(pPulseWidStrDataDispaly != NULL)
	{
		free(pPulseWidStrDataDispaly);
		pPulseWidStrDataDispaly = NULL;
	}
    pPulseWidStrDataDispaly = TransString("Pusle Mode2: 1310nm, 0x17, APD 43v, Pulse 100ns");
    SetLabelText(pPulseWidStrDataDispaly,pPulseWidLblDataDispaly);
    SetLabelFont(pPulseWidFntRight, pPulseWidLblDataDispaly);  
	DisplayPicture(pPulseWidPicDataDispaly);
    DisplayLabel(pPulseWidLblDataDispaly);

    return iReturn;
}


static int PulseWidBtnF3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    ShowBtn_FuncUp(pPulseWidBtnF3, pPulseWidLblF3);

    ShowWait();
	PulseMode(2,1,100,1);		

	
	if(pPulseWidStrDataDispaly != NULL)
	{
		free(pPulseWidStrDataDispaly);
		pPulseWidStrDataDispaly = NULL;
	}
    pPulseWidStrDataDispaly = TransString("Pusle Mode3: 1550nm, 0x15, APD 43v, Pulse 100ns");
    SetLabelText(pPulseWidStrDataDispaly,pPulseWidLblDataDispaly);
    SetLabelFont(pPulseWidFntRight, pPulseWidLblDataDispaly);
	DisplayPicture(pPulseWidPicDataDispaly);
    DisplayLabel(pPulseWidLblDataDispaly);
	
    return iReturn;
}


static int PulseWidBtnF4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    ShowBtn_FuncUp(pPulseWidBtnF4, pPulseWidLblF4);

	ShowWait();
	PulseMode(3,1,100,1);		

  
	if(pPulseWidStrDataDispaly != NULL)
	{
		free(pPulseWidStrDataDispaly);
		pPulseWidStrDataDispaly = NULL;
	}
    pPulseWidStrDataDispaly = TransString("Pusle Mode4: 1550nm, 0x14, APD 43v, Pulse 100ns");
    SetLabelText(pPulseWidStrDataDispaly,pPulseWidLblDataDispaly);
    SetLabelFont(pPulseWidFntRight, pPulseWidLblDataDispaly);
	DisplayPicture(pPulseWidPicDataDispaly);
    DisplayLabel(pPulseWidLblDataDispaly);
	
    return iReturn;
}

static int PulseWidBtnF5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    ShowBtn_FuncUp(pPulseWidBtnF5, pPulseWidLblF5);

	ShowWait();
	PulseMode(4,1,100,1);

  
	if(pPulseWidStrDataDispaly != NULL)
	{
		free(pPulseWidStrDataDispaly);
		pPulseWidStrDataDispaly = NULL;
	}
    pPulseWidStrDataDispaly = TransString("Pusle Mode5: 1550nm, 0x08, APD 43v, Pulse 100ns");
    SetLabelText(pPulseWidStrDataDispaly,pPulseWidLblDataDispaly);
    SetLabelFont(pPulseWidFntRight, pPulseWidLblDataDispaly);
	DisplayPicture(pPulseWidPicDataDispaly);
    DisplayLabel(pPulseWidLblDataDispaly);
	
    return iReturn;
}

static int PulseWidBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pPulseWidBtnBack);
    DisplayPicture(pPulseWidBtnBack);


    return iReturn;
}

static int PulseWidBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	OPMSetApd(0);
	OPMSetVdd(0);

	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pPulseWidBtnBack);
    DisplayPicture(pPulseWidBtnBack);
	
	
    GUIWINDOW *pWnd = NULL; 
	
    pWnd = CreateWindow(0, 0, 800, 480, 
                        FrmMainOptInit, FrmMainOptExit, 
                        FrmMainOptPaint, FrmMainOptLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmPulseWid);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

static int PulseWidErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}


static int ShowWait(void)
{
	int iReturn = 0;

	if(pPulseWidStrDataDispaly != NULL)
	{
		free(pPulseWidStrDataDispaly);
		pPulseWidStrDataDispaly = NULL;
	}
	pPulseWidStrDataDispaly = TransString("Please Wait 2 Seconds......");
	SetLabelText(pPulseWidStrDataDispaly,pPulseWidLblDataDispaly);
	SetLabelFont(pPulseWidFntRight, pPulseWidLblDataDispaly);  
	DisplayPicture(pPulseWidPicDataDispaly);
	DisplayLabel(pPulseWidLblDataDispaly);
	
	return iReturn;
}


static int PulseMode(int iAmp,int iApd,int iPulse,int iWave)
{
	int iReturn = 0;
	//U32 AmpValue[5] = {0x16,0x17,0x15,0x14,0x08};
	U32 AmpValue[5] = { 0x21, 0x25, 0x23, 0x27, 0x20 };
	//U32 ApdRegValue[4] = {0x0da0,0x0d85, 0x0ce7,0x0b11};
	U32 ApdRegValue[4] = { 0x0900, 0x08e0, 0x0860, 0x0700 };

	fpga_init(FPGA_DATA_NAME);
	MsecSleep(500);
	InitFpgaRegVal();
	BoardWorkInit();
	MsecSleep(200);

	//设置脉宽，
	Opm_SetPulse(iPulse);
	//设置波长，
	Opm_SetWave(iWave);
	
    OPMSetApd(ApdRegValue[iApd]);
	OPMSetVdd(0x0680);
	Opm_SetAmp(AmpValue[iAmp]);
	OPMSetAtt(0x0a80);
	
	//设置移相位1:1
	Opm_SetPhase();
	//设置DAQMOD_SET为1:1采集
	Opm_SetIntp(DAQ_INTP01_01);

	//Opm_SetMode(DAQMODE_ACC);
	//设置滤波
    Opm_SetFilter(_IIR_256);

	//停止数据采集，设置DAQ_CTRL
	Opm_SetDaqCmd(ENUM_DAQ_CMD_STOP);
	
	//设置数据采集的ADC基本参数 分别是 采集个数和采集间隔
    SedAdcParameter(0x4000);

	Opm_SetDaqCmd(ENUM_DAQ_CMD_START);
	
	return iReturn;
}

//enter键响应处理函数
static void KeyEnterCallBack(int iKeyFlag)
{
	pPulseWidStrF1 = TransString("PulseMode 1");
	pPulseWidStrF2 = TransString("PulseMode 2");
	pPulseWidStrF3 = TransString("PulseMode 3");
	pPulseWidStrF4 = TransString("PulseMode 4");
	pPulseWidStrF5 = TransString("PulseMode 5");

	switch (iKeyFlag)
	{
	case 0://PulseMode 1
		printf("-----%s-----%d----PulseMode 1------\n", __func__, __LINE__);
		PulseWidBtnF1_Down(NULL, 0, NULL, 0);
		PulseWidBtnF1_Up(NULL, 0, NULL, 0);
		break;
	case 1://PulseMode 2
		printf("-----%s-----%d----PulseMode 2------\n", __func__, __LINE__);
		PulseWidBtnF2_Down(NULL, 0, NULL, 0);
		PulseWidBtnF2_Up(NULL, 0, NULL, 0);
		break;
	case 2://PulseMode 3
		printf("-----%s-----%d----PulseMode 3------\n", __func__, __LINE__);
		PulseWidBtnF3_Down(NULL, 0, NULL, 0);
		PulseWidBtnF3_Up(NULL, 0, NULL, 0);
		break;
	case 3://PulseMode 4
		printf("-----%s-----%d----PulseMode 4------\n", __func__, __LINE__);
		PulseWidBtnF4_Down(NULL, 0, NULL, 0);
		PulseWidBtnF4_Up(NULL, 0, NULL, 0);
		break;
	case 4://PulseMode 5
		printf("-----%s-----%d----PulseMode 5------\n", __func__, __LINE__);
		PulseWidBtnF5_Down(NULL, 0, NULL, 0);
		PulseWidBtnF5_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}