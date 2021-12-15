/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfiber.c
* 摘    要：  实现窗体frmfiber的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-30
*******************************************************************************/
#include "wnd_frmfiber.h"
#include "wnd_global.h"
#include "wnd_frmmainopt.h"

#include "common_opm.h"
#include "drvoptic.h"
#include "opm.h"
#include "fct_algorithm.h"


extern POPM_TUNING  pOpmTunning;

//按钮区的字体
static GUIFONT *pFiberFntRight = NULL;     //标题按钮的字体
extern float LogSigTable;

/************************
* 窗体frmmain中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pFiberStrTitle = NULL;
static GUILABEL *pFiberLblTitle = NULL;

//桌面上的文本
static GUICHAR *pFiberStrF1 = NULL;    //窗体图标1的文本
static GUICHAR *pFiberStrF2 = NULL;    //窗体图标2的文本

//信息栏上文本
static GUICHAR *pFiberStrInfo = NULL;    //提示信息的文本

/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmFiber = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pFiberBarStatus = NULL;
static GUIPICTURE *pFiberBgDesk = NULL;
static GUIPICTURE *pFiberBarInfo = NULL;

//桌面上的控件
static GUIPICTURE *pFiberBtnF1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pFiberBtnF2 = NULL;     //窗体图标2图形框

static GUILABEL *pFiberLblF1 = NULL;       //窗体图标1标签
static GUILABEL *pFiberLblF2 = NULL;       //窗体图标2标签

//数据显示区
static GUIPICTURE *pFiberPathPicDispaly = NULL;
static GUILABEL *pFiberPathLblDispaly1 = NULL;
static GUICHAR *pFiberPathStrDispaly1 = NULL;
static GUILABEL *pFiberPathLblDispaly2 = NULL;
static GUICHAR *pFiberPathStrDispaly2 = NULL;

static GUIPICTURE *pFiberLenPicDispaly = NULL;
static GUILABEL *pFiberLenLblDispaly1 = NULL;
static GUICHAR *pFiberLenStrDispaly1 = NULL;  
static GUILABEL *pFiberLenLblDispaly2 = NULL;
static GUICHAR *pFiberLenStrDispaly2 = NULL;  
static GUILABEL *pFiberLenLblDispaly3 = NULL;
static GUICHAR *pFiberLenStrDispaly3 = NULL;  

//信息栏上的控件
static GUILABEL *pFiberLblInfo = NULL;       //提示信息标签

static GUIPICTURE *pFiberBtnBack = NULL;

/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int FiberTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int FiberTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int FiberWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int FiberWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int FiberBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FiberBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int FiberBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FiberBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int FiberBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);


static int FiberBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int FiberErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


//光纤长度检测
static int CalibrationFiberLen(float calibrationFiberLen, float refractive);

//光纤链路检查
static int FiberPathCheck(int wave);

/***
  * 功能：
        窗体frmmain的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmFiberInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	
    //得到当前窗体对象
    pFrmFiber = (GUIWINDOW *) pWndObj;

    //初始化字体资源
    pFiberFntRight = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    FiberTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pFiberBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, BmpFileDirectory"bar_status.bmp");
    pFiberBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420, BmpFileDirectory "bg_desk.bmp");
    pFiberBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30, BmpFileDirectory "bar_info.bmp");
    pFiberLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pFiberStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFiberLblTitle);
	
    //建立桌面上的控件
    pFiberBtnF1 = CreatePicture(456, 115, 80, 53,
                                 BmpFileDirectory"btn_enable.bmp");
    pFiberBtnF2 = CreatePicture(456, 320, 80, 53,
                                BmpFileDirectory "btn_enable.bmp");

    pFiberLblF1 = CreateLabel(456, 124, 80, 53, pFiberStrF1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFiberLblF1);
    pFiberLblF2 = CreateLabel(456, 329, 80, 53, pFiberStrF2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFiberLblF2);

	pFiberPathPicDispaly = CreatePicture(25, 90, 408, 120,
                                 BmpFileDirectory"frm_fiberdata.bmp");
	pFiberPathLblDispaly1 = CreateLabel(30, 100, 400, 30, pFiberPathStrDispaly1);
	pFiberPathLblDispaly2 = CreateLabel(30, 130, 400, 30, pFiberPathStrDispaly2);
	

	pFiberLenPicDispaly = CreatePicture(25, 285, 408, 120,
                                 BmpFileDirectory"frm_fiberdata.bmp");
	pFiberLenLblDispaly1 = CreateLabel(30, 295, 400, 100, pFiberLenStrDispaly1);
	
	pFiberLenLblDispaly2 = CreateLabel(30, 395, 400, 30, pFiberLenStrDispaly2);
	pFiberLenLblDispaly3 = CreateLabel(30, 395, 400, 30, pFiberLenStrDispaly3);

	pFiberBtnBack = CreatePicture(520, 454, 60, 25,
                                     BmpFileDirectory"btn_back_unpress.bmp");
    //建立按钮区的控件
    //...
    //建立信息栏上的控件
    pFiberLblInfo = CreateLabel(20, 458, 300, 16, pFiberStrInfo);

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmFiber->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFiber, 
                  pFrmFiber);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFiberBtnF1, 
                  pFrmFiber);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFiberBtnF2, 
                  pFrmFiber);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFiberBtnBack, 
                  pFrmFiber);

    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmFiber->Mutex));    //注意，必须释放锁

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmFiber, 
                    FiberWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmFiber, 
                    FiberWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFiberBtnF1, 
                    FiberBtnF1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFiberBtnF2, 
                    FiberBtnF2_Down, NULL, 0, pMsg);
			 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFiberBtnBack, 
                    FiberBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFiberBtnF1, 
                    FiberBtnF1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFiberBtnF2, 
                    FiberBtnF2_Up, NULL, 0, pMsg);

  
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFiberBtnBack, 
                    FiberBtnBack_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmFiber, 
                    FiberErrProc_Func, NULL, 0, pMsg);

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
int FrmFiberExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmFiber = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmFiber->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmFiber);
    MutexUnlock(&(pFrmFiber->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pFiberBarStatus);
    DestroyPicture(&pFiberBgDesk);
    DestroyPicture(&pFiberBarInfo);
    DestroyLabel(&pFiberLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyPicture(&pFiberBtnF1);
    DestroyPicture(&pFiberBtnF2);

    DestroyLabel(&pFiberLblF1);
    DestroyLabel(&pFiberLblF2);

    DestroyPicture(&pFiberPathPicDispaly);
    DestroyLabel(&pFiberPathLblDispaly1);
	DestroyLabel(&pFiberPathLblDispaly2);
	
	DestroyPicture(&pFiberLenPicDispaly);
    DestroyLabel(&pFiberLenLblDispaly1);
	DestroyLabel(&pFiberLenLblDispaly2);
	DestroyLabel(&pFiberLenLblDispaly3);
    //销毁信息栏上的控件
    DestroyLabel(&pFiberLblInfo);
	
    DestroyPicture(&pFiberBtnBack);

    //释放文本资源
    //***************************************************************/
    FiberTextRes_Exit(NULL, 0, NULL, 0);

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
int FrmFiberPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmFiber = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pFiberBarStatus);
    DisplayPicture(pFiberBgDesk);
    DisplayPicture(pFiberBarInfo);
    DisplayLabel(pFiberLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayPicture(pFiberBtnF1);
    DisplayPicture(pFiberBtnF2);

    DisplayLabel(pFiberLblF1);
    DisplayLabel(pFiberLblF2);

    //...
    DisplayPicture(pFiberPathPicDispaly);
    DisplayLabel(pFiberPathLblDispaly1);
	DisplayLabel(pFiberPathLblDispaly2);

	DisplayPicture(pFiberLenPicDispaly);
    DisplayLabel(pFiberLenLblDispaly1);
    //显示信息栏上的控件
    DisplayLabel(pFiberLblInfo);
    DisplayPicture(pFiberBtnBack);

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
int FrmFiberLoop(void *pWndObj)
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
int FrmFiberPause(void *pWndObj)
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
int FrmFiberResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int FiberTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化状态栏上的文本
    pFiberStrTitle = TransString("**Fiber FCT**");
    //初始化桌面上的文本
    pFiberStrF1 = TransString("Opt check");
    pFiberStrF2 = TransString("Dev adjust");

    //初始化信息栏上的文本
    pFiberStrInfo = TransString("");

    pFiberPathStrDispaly1 = TransString("Please plug in 1Km - 5Km optical fiber before adjust");
	pFiberPathStrDispaly2 = TransString("");
	
	pFiberLenStrDispaly1 = TransString("Please plug in 5m optical fiber before adjust");
	pFiberLenStrDispaly2 = TransString("");
	pFiberLenStrDispaly3 = TransString("");

    return iReturn;
}


//释放文本资源
static int FiberTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
    free(pFiberStrTitle);
    //释放桌面上的文本
    free(pFiberStrF1);
    free(pFiberStrF2);

    //释放按钮区的文本
    //...
    free(pFiberPathStrDispaly1);
	free(pFiberPathStrDispaly2);
	
	free(pFiberLenStrDispaly1);
	free(pFiberLenStrDispaly2);
	free(pFiberLenStrDispaly3);
    //释放信息栏上的文本
    free(pFiberStrInfo);

    return iReturn;
}


static int FiberWndKey_Down(void *pInArg, int iInLen, 
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
		FiberBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int FiberWndKey_Up(void *pInArg, int iInLen, 
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
        iKeyFlag = iKeyFlag ? 0 : 1;
        break;
    case KEYCODE_down:
        iKeyFlag = iKeyFlag ? 0 : 1;
        break;
    case KEYCODE_enter:
        if (!iKeyFlag)
        {
            FiberBtnF1_Down(NULL, 0, NULL, 0);
            FiberBtnF1_Up(NULL, 0, NULL, 0);
        }
        else
        {
            FiberBtnF2_Down(NULL, 0, NULL, 0);
            FiberBtnF2_Up(NULL, 0, NULL, 0);
        }
        break;
    case KEYCODE_BACK:
        iKeyFlag = 0;
        FiberBtnBack_Up(NULL, 0, NULL, 0);
        break;
    default:
        break;
    }

    return iReturn;
}

static int FiberBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pFiberBtnF1, pFiberLblF1);

	if(pFiberPathStrDispaly1 != NULL)
	{
		free(pFiberPathStrDispaly1);
		pFiberPathStrDispaly1 = NULL;
	}
    pFiberPathStrDispaly1 = TransString("Optical check start, please wait...");
    SetLabelText(pFiberPathStrDispaly1,pFiberPathLblDispaly1);
    DisplayPicture(pFiberPathPicDispaly);
    DisplayLabel(pFiberPathLblDispaly1);
	

    return iReturn;
}

static int FiberBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pFiberBtnF2, pFiberLblF2);

	if(pFiberLenStrDispaly1 != NULL)
	{
		free(pFiberLenStrDispaly1);
		pFiberLenStrDispaly1 = NULL;
	}
    pFiberLenStrDispaly1 = TransString("Deviation adjust start, please wait...");
	SetLabelText(pFiberLenStrDispaly1,pFiberLenLblDispaly1); 
    DisplayPicture(pFiberLenPicDispaly);
    DisplayLabel(pFiberLenLblDispaly1);
	
    return iReturn;
}

static int FiberBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int iRet = 0;
	
    ShowBtn_FuncUp(pFiberBtnF1, pFiberLblF1);

	SetPictureEnable(0, pFiberBtnF1);
	SetPictureEnable(0, pFiberBtnF2);


	//波长1310
	iRet = FiberPathCheck(0);
	if(pFiberPathStrDispaly1 != NULL)
	{
		free(pFiberPathStrDispaly1);
		pFiberPathStrDispaly1 = NULL;
	}
	if(iRet)
	{
		pFiberPathStrDispaly1 = TransString("1310 Optical check good");
	}
	else
	{
		pFiberPathStrDispaly1 = TransString("1310 Optical check bad");
	}

	if(pFiberPathStrDispaly2 != NULL)
	{
		free(pFiberPathStrDispaly2);
		pFiberPathStrDispaly2 = NULL;
	}
	pFiberPathStrDispaly2 = TransString("checking 1550...");
    SetLabelText(pFiberPathStrDispaly1,pFiberPathLblDispaly1);
	SetLabelText(pFiberPathStrDispaly2,pFiberPathLblDispaly2);
	DisplayPicture(pFiberPathPicDispaly);
    DisplayLabel(pFiberPathLblDispaly1);
	DisplayLabel(pFiberPathLblDispaly2);


	//波长1550
	iRet = FiberPathCheck(1);
	if(pFiberPathStrDispaly2 != NULL)
	{
		free(pFiberPathStrDispaly2);
		pFiberPathStrDispaly2 = NULL;
	}
	if(iRet)
	{
		pFiberPathStrDispaly2 = TransString("1550 Optical check good");
	}
	else
	{
		pFiberPathStrDispaly2 = TransString("1550 Optical check bad");
	}
    SetLabelText(pFiberPathStrDispaly2,pFiberPathLblDispaly2);
	DisplayPicture(pFiberPathPicDispaly);
    DisplayLabel(pFiberPathLblDispaly1);
	DisplayLabel(pFiberPathLblDispaly2);

	SetPictureEnable(1, pFiberBtnF1);
	SetPictureEnable(1, pFiberBtnF2);
	
    return iReturn;
}


static int FiberBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	//检测光纤链路长度
	CalibrationFiberLen(DEFAULTCALIBRATIONFIBERLEN,DEFAULTREFRACTIVE);
	
    //临时变量定义
    ShowBtn_FuncUp(pFiberBtnF2, pFiberLblF2);

    SetLabelText(pFiberLenStrDispaly1,pFiberLenLblDispaly1); 
	//SetLabelText(pFiberLenStrDispaly2,pFiberLenLblDispaly2); 
	//SetLabelText(pFiberLenStrDispaly3,pFiberLenLblDispaly3);
	//SetLabelFont(pFiberFntRight, pFiberLenLblDispaly1); 
	//SetLabelFont(pFiberFntRight, pFiberLenLblDispaly2); 
	//SetLabelFont(pFiberFntRight, pFiberLenLblDispaly3); 
    DisplayPicture(pFiberLenPicDispaly);
    DisplayLabel(pFiberLenLblDispaly1);
	//DisplayLabel(pFiberLenLblDispaly2);
	//DisplayLabel(pFiberLenLblDispaly3);

    return iReturn;
}

static int FiberBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pFiberBtnBack);
    DisplayPicture(pFiberBtnBack);

    return iReturn;
}

static int FiberBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	OPMSetApd(0);
	OPMSetVdd(0);

	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pFiberBtnBack);
    DisplayPicture(pFiberBtnBack);
	
    GUIWINDOW *pWnd = NULL; 
    pWnd = CreateWindow(0, 0, 320, 240, 
                        FrmMainOptInit, FrmMainOptExit, 
                        FrmMainOptPaint, FrmMainOptLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmFiber);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

static int FiberErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}

/*
 *校准光纤长度
 *@para[in] calibrationFiberLen 用于校准的标准光纤长度
 *@return   校准长度(以点位单位)
 *@note     
*/

static int CalibrationFiberLen(float calibrationFiberLen, float refractive)
{
	printf("calibrationFiberLen=%f\n",calibrationFiberLen);
	int iReturn = 0, i = 0, j = 0;

	// 设置光速、采样比率(移相)、采样频率
	int lightSpeed = 299792458;
	float sampleRating = 64.0f;
	int g_sampleRating = 40000000;
	float perPointDistance = lightSpeed / ( 2 * g_sampleRating *sampleRating * refractive);

	int pointNum = (int)(calibrationFiberLen / perPointDistance);
	
	//采样长度
	//4/3是为了避免出现该距离的回波影响判断结果
	//512是默认的采集512个噪声点；
	int len = (int)(pointNum * 4 / 3) + 512;  
    len = max(len,2500);
    
	//对数表
	LogTableCreat();
	
	//数据采集
	U16* data = (U16*)malloc(sizeof(U16) * len);
	memset(data,0,len * sizeof(U16));
	GetCheckFiberLen(data,len);

	data[0] = data[1];
	
#if 0
	FILE* fp;
	int num = 0;
	char name[512];
	sprintf(name,"/mnt/usb/LogSignal[%d].txt",num++);
	fp = fopen(name,  "w+");
	if (fp != NULL)
	{
		for(i=0;i < len;i++)
			fprintf(fp,"%u\r\n",data[i]);
		fclose(fp);

		system("sync");
	}
#endif
	printf("sample over!!!\n");

	//黑电平(对数基准值)
	U16 BlackLevel = (U16)floor(GetMeanUint16(data + len - 512, 512) + 0.5f);
    printf("BlackLevel=%u\n",BlackLevel);

	//对数转换
	F32* logSig = (F32*)malloc(sizeof(F32) * len);
	IntTransferLog(data, len, logSig, (int)BlackLevel);

	// 参数设置
    int minPeakDistance = 62;  //!< 最小峰值距离，这里为半个衰减盲区
    float thresh = 15.0f;      //!< 极值阈值，该极值点对应的最小峰值

	//寻找极值
	INDEX_LIST* p = FindPeak(logSig,len,minPeakDistance,thresh);

    printf("pointNum=%d\n",p->num);
 	int offsetPoint = 0; 
    if (p->num == 1) 
    { 
        if (p->indexValue[0].index < 900) 
        { 
            printf("calibration error\n"); 
        } 
        else 
        { 
             offsetPoint = p->indexValue[0].index - F216BLINDWUDTH - (int)(calibrationFiberLen / perPointDistance); 
        } 
        printf("offsetPoint=%d\n",offsetPoint);
    }
    else
    {
    	// 校准精度
        float precision = (calibrationFiberLen <= 100.0f) ? 0.5 : ((calibrationFiberLen <= 2.5) ? 1.0f : 1.5f);

        for (i = 0; i < p->num; i++)
        {
            for (j = i + 1; j < p->num; j++)
            {
                if (((abs((p->indexValue[j].index - p->indexValue[i].index)
                    * perPointDistance - calibrationFiberLen)) < precision)
                    && (p->indexValue[i].index != 0))
                {
                    offsetPoint = p->indexValue[i].index - F216BLINDWUDTH;
                    break;
                }
            }
        }
    }

	char buff[128] = {0};
	char buffTemp1[128] = {0};
	char buffTemp2[128] = {0};
	char buffTemp3[128] = {0};
	
	
	printf("offsetPoint = %d\n",offsetPoint); 
	sprintf((char *)buffTemp2,"offsetPoint = %d       ",offsetPoint);
	
	if(offsetPoint < 900) 
	{
		printf("Check Error!!!\n");
		sprintf((char *)buffTemp3,"Check Error!!!");

		offsetPoint = 0;
		sprintf((char *)buffTemp1,"offset = %d Adjust Error",offsetPoint);
	}	
	else
	{
		printf("Check Success!!!\n");
		sprintf((char *)buffTemp3,"Check OK!!!");

		sprintf((char *)buffTemp1,"offset = %d Adjust Success",offsetPoint);
	}
	strncat(buff, buffTemp3, strlen(buffTemp3));

	if(pFiberLenStrDispaly1 != NULL)
	{
		free(pFiberLenStrDispaly1);
		pFiberLenStrDispaly1 = NULL;
	}
    pFiberLenStrDispaly1 = TransString(buffTemp1);

	if(pFiberLenStrDispaly2 != NULL)
	{
		free(pFiberLenStrDispaly2);
		pFiberLenStrDispaly2 = NULL;
	}
    pFiberLenStrDispaly2 = TransString(buffTemp2);

	if(pFiberLenStrDispaly3 != NULL)
	{
		free(pFiberLenStrDispaly3);
		pFiberLenStrDispaly3 = NULL;
	}
    pFiberLenStrDispaly3 = TransString(buffTemp3);

	if(offsetPoint < 0)
	{
		offsetPoint = 0;
	}
	pOpmTunning->OffsetPoint = (unsigned int)offsetPoint;
	SaveOpmTunning();
	
	free(data);
	free(logSig);
	free(p);
	
	return iReturn;
}

//0:代表1310， 1:代表1550
static int FiberPathCheck(int wave)
{
	int iRet = 0;
	//找到击穿电压
	iRet = GetFiberpath(wave);
	
	return iRet;
}