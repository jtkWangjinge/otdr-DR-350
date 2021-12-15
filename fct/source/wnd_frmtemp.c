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
* 完成日期：  2020-10-30
*
*******************************************************************************/
#include "wnd_frmtemp.h"
#include "wnd_global.h"
#include "wnd_frmmainopt.h"
// #include "wnd_frmtestkeyboard.h"

#include "common_opm.h"


APD_BREAK_PARA *pApd_Break_Para = NULL;

extern int iDrawX[];
extern int iDrawY[];

static int flag = 0;
static GUITHREAD threadId = -1;
/************************
* 窗体frmmain中的字体资源
************************/
//桌面上的字体
static GUIFONT *pTempFntPoint = NULL;     //标题按钮的字体

//状态栏上的文本
static GUICHAR *pTempStrTitle = NULL;
static GUILABEL *pTempLblTitle = NULL;

//桌面上的文本
static GUICHAR *pTempStrInfo1 = NULL;
static GUICHAR *pTempStrF1 = NULL;    //窗体图标1的文本
static GUICHAR *pTempStrF2 = NULL;    //窗体图标1的文本
static GUICHAR *pTempStrPointY[3];	//曲线上的点坐标
static GUILABEL *pTempLblPointY[3];//曲线上的标签

static GUICHAR *pTempStrPointX[5];	//曲线上的点坐标
static GUILABEL *pTempLblPointX[5];//曲线上的标签

//信息栏上文本
static GUICHAR *pTempStrInfo = NULL;    //提示信息的文本

/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmTemp = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pTempBarStatus = NULL;
static GUIPICTURE *pTempBgDesk = NULL;
static GUIPICTURE *pTempBarInfo = NULL;

//状态栏上的控件

//桌面上的控件
static GUIPICTURE *pTempBtnF1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pTempBtnF2 = NULL;     //窗体图标1图形框
static GUILABEL *pTempLblF1 = NULL;       //窗体图标1标签
static GUILABEL *pTempLblF2 = NULL;       //窗体图标1标签
static GUILABEL *pTempLblInfo1 = NULL;

//曲线显示区
static GUIPICTURE *pTempPicMainTrce = NULL;
//数据显示区
static GUIPICTURE *pTempPicDataDispaly1 = NULL;
static GUILABEL *pTempLblDataDispaly1 = NULL;
static GUICHAR *pTempStrDataDispaly1 = NULL;  

static GUIPICTURE *pTempPicDataDispaly2 = NULL;
static GUILABEL *pTempLblDataDispaly2 = NULL;
static GUICHAR *pTempStrDataDispaly2 = NULL;    

//信息栏上的控件
static GUILABEL *pTempLblInfo = NULL;       //提示信息标签

static GUIPICTURE *pTempBtnBack = NULL;
//按键控件
// static struct KeyBoard_Control* pKeyBoardControl = NULL;
/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int TempTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int TempTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int TempWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int TempWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int TempBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int TempBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int TempBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int TempBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int TempBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int TempBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int TempErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void *TempFCT(void *pThreadArg);

//按键响应回调函数
// static void KeyBoardCallBack(int iSelected);

/***
  * 功能：
        窗体frmmain的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmTempInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	int x = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	flag = 0;
    //得到当前窗体对象
    pFrmTemp = (GUIWINDOW *) pWndObj;
	
	if(pApd_Break_Para == NULL)
	{
		pApd_Break_Para = (APD_BREAK_PARA*)calloc(1,sizeof(APD_BREAK_PARA));
	}

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    TempTextRes_Init(NULL, 0, NULL, 0);
	
    pTempFntPoint = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pTempBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, BmpFileDirectory"bar_status.bmp");
    pTempBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420, BmpFileDirectory "bg_desk.bmp");
    pTempBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30, BmpFileDirectory "bar_info.bmp");
    pTempLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pTempStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pTempLblTitle);

    //建立桌面上的控件
    pTempBtnF1 = CreatePicture(556, 95, 80, 53, BmpFileDirectory"btn_enable.bmp");
 	pTempBtnF2 = CreatePicture(556, 155, 80, 53, BmpFileDirectory"btn_disable.bmp");

    pTempLblF1 = CreateLabel(556, 113, 80, 16, pTempStrF1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pTempLblF1);
    pTempLblF2 = CreateLabel(556, 173, 80, 16, pTempStrF2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pTempLblF2);
	
    pTempLblInfo1 = CreateLabel(40, 35, 50, 20, pTempStrInfo1);
	
	pTempPicMainTrce = CreatePicture(40, 55, 508, 230, BmpFileDirectory"bg_main_trace.bmp");
	pTempPicDataDispaly1 = CreatePicture(40, 315, 272, 120,
                                 			BmpFileDirectory"frm_data.bmp");
	pTempLblDataDispaly1 = CreateLabel(45, 320, 262, 110, pTempStrDataDispaly1);
	
	pTempPicDataDispaly2 = CreatePicture(312, 315, 236, 120,
                                 			BmpFileDirectory"frm_cond.bmp");
	pTempLblDataDispaly2 = CreateLabel(317, 320, 226, 110, pTempStrDataDispaly2);
		
	pTempBtnBack = CreatePicture(520, 454, 60, 25,
                                     BmpFileDirectory"btn_back_unpress.bmp");
	
	pTempLblPointY[2] = CreateLabel(0, 45, 40, 20, pTempStrPointY[2]);
	pTempLblPointY[1] = CreateLabel(0, 160, 40, 20, pTempStrPointY[1]);
	pTempLblPointY[0] = CreateLabel(0, 270, 40, 20, pTempStrPointY[0]);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pTempLblPointY[0]);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pTempLblPointY[1]);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pTempLblPointY[2]);
	SetLabelFont(pTempFntPoint, pTempLblPointY[0]);
	SetLabelFont(pTempFntPoint, pTempLblPointY[1]);
	SetLabelFont(pTempFntPoint, pTempLblPointY[2]);
	
	for(i = 0; i < 4; i++)
	{
		x = 42 + i * 122;
		pTempLblPointX[i] = CreateLabel(x, 285, 70, 20, pTempStrPointX[i]);
		SetLabelFont(pTempFntPoint, pTempLblPointX[i]);
		DisplayLabel(pTempLblPointX[i]);
	}
	x = 42 + 4 * 122;
	pTempLblPointX[4] = CreateLabel(x, 285, 70, 20, pTempStrPointX[4]);
	SetLabelFont(pTempFntPoint, pTempLblPointX[4]);
	DisplayLabel(pTempLblPointX[i]);

    //建立信息栏上的控件
    pTempLblInfo = CreateLabel(20, 458, 300, 16, pTempStrInfo);

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmTemp->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmTemp, 
                  pFrmTemp);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pTempBtnF1, 
                  pFrmTemp);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pTempBtnF2, 
                  pFrmTemp);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pTempBtnBack, 
                  pFrmTemp);

    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmTemp->Mutex));    //注意，必须释放锁

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmTemp, 
                    TempWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmTemp, 
                    TempWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTempBtnF1, 
                    TempBtnF1_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTempBtnF2, 
                    TempBtnF2_Down, NULL, 0, pMsg);
	
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTempBtnBack, 
                    TempBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pTempBtnF1, 
                    TempBtnF1_Up, NULL, 0, pMsg);
 	 LoginMessageReg(GUIMESSAGE_TCH_UP, pTempBtnF2, 
                    TempBtnF2_Up, NULL, 0, pMsg);
	 
    LoginMessageReg(GUIMESSAGE_TCH_UP, pTempBtnBack, 
                    TempBtnBack_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmTemp, 
                    TempErrProc_Func, NULL, 0, pMsg);
	//创建按键控件
	// pKeyBoardControl = CreateKeyBoard(320, 0, KeyBoardCallBack);
	// AddKeyBoardControlToWnd(pKeyBoardControl, pFrmTemp);

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
int FrmTempExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmTemp = (GUIWINDOW *) pWndObj;
	//销毁按键控件
	// DelKeyBoardControlFromWnd(pKeyBoardControl, pFrmTemp);
	// DestroyKeyBoardControl(&pKeyBoardControl);
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmTemp->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmTemp);
    MutexUnlock(&(pFrmTemp->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pTempBarStatus);
    DestroyPicture(&pTempBgDesk);
    DestroyPicture(&pTempBarInfo);
    DestroyLabel(&pTempLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyLabel(&pTempLblInfo1);
    DestroyPicture(&pTempBtnF1);
	DestroyPicture(&pTempBtnF2);
    DestroyLabel(&pTempLblF1);
	DestroyLabel(&pTempLblF2);
    //销毁按钮区的控件
    DestroyPicture(&pTempPicMainTrce);
    DestroyPicture(&pTempPicDataDispaly1);
    DestroyLabel(&pTempLblDataDispaly1);
    DestroyPicture(&pTempPicDataDispaly2);
    DestroyLabel(&pTempLblDataDispaly2);
	for(i = 0; i < 5; i++)
	{
    	DestroyLabel(&pTempLblPointX[i]);
	}
	for(i = 0; i < 3; i++)
	{
    	DestroyLabel(&pTempLblPointY[i]);
	}
    //销毁信息栏上的控件
    DestroyLabel(&pTempLblInfo);
	
    DestroyPicture(&pTempBtnBack);

    //释放文本资源
    //***************************************************************/
    TempTextRes_Exit(NULL, 0, NULL, 0);

	if(pApd_Break_Para)
	{
		free(pApd_Break_Para);
		pApd_Break_Para = NULL;
	}	
	
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
int FrmTempPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

    //得到当前窗体对象
    pFrmTemp = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pTempBarStatus);
    DisplayPicture(pTempBgDesk);
    DisplayPicture(pTempBarInfo);
    DisplayLabel(pTempLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayLabel(pTempLblInfo1);
    DisplayPicture(pTempBtnF1);
	DisplayPicture(pTempBtnF2);
    DisplayLabel(pTempLblF1);
	DisplayLabel(pTempLblF2);

    //显示按钮区的控件
    DisplayPicture(pTempPicMainTrce);
    DisplayPicture(pTempPicDataDispaly1);
    DisplayLabel(pTempLblDataDispaly1);
    DisplayPicture(pTempPicDataDispaly2);
    DisplayLabel(pTempLblDataDispaly2);
	for(i = 0; i < 5; i++)
	{
    	DisplayLabel(pTempLblPointX[i]);
	}
	for(i = 0; i < 3; i++)
	{
    	DisplayLabel(pTempLblPointY[i]);
	}
    //显示信息栏上的控件
    DisplayLabel(pTempLblInfo);
    DisplayPicture(pTempBtnBack);
	DrawCurve();
	//显示按键控件
	// DisplayKeyBoardControl(pKeyBoardControl);

	SetPictureEnable(0, pTempBtnF2);
	
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
int FrmTempLoop(void *pWndObj)
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
int FrmTempPause(void *pWndObj)
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
int FrmTempResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int TempTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	char buff[20];

    //初始化状态栏上的文本
    pTempStrTitle = TransString("**TEMPERATURE FCT**");
    //初始化桌面上的文本
    pTempStrInfo1 = TransString("Trace :");
    pTempStrF1 = TransString("Start");
	pTempStrF2 = TransString("Stop");

	pTempStrDataDispaly1 = TransString("data area");
	pTempStrDataDispaly2 = TransString("info area");
	pTempStrPointY[2] = TransString("3000");
	pTempStrPointY[1] = TransString("2500");
	pTempStrPointY[0] = TransString("2000");
	for(i = 0; i < 4; i++)
	{
		sprintf((char *)buff, "%d",20 * i);
		pTempStrPointX[i] = TransString((char *)buff);
	}
	sprintf((char *)buff, "%d C",80);
	pTempStrPointX[4] = TransString((char *)buff);
	
    //初始化信息栏上的文本
    pTempStrInfo = TransString("");

    return iReturn;
}


//释放文本资源
static int TempTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

    //释放状态栏上的文本
    free(pTempStrTitle);

    //释放桌面上的文本
    free(pTempStrInfo1);
    free(pTempStrF1);
	free(pTempStrF2);
	for(i = 0; i < 5; i++)
	{
    	free(pTempStrPointX[i]);
	}
	for(i = 0; i < 3; i++)
	{
    	free(pTempStrPointY[i]);
	}

    //释放按钮区的文本
    //...
    free(pTempStrDataDispaly1);
    free(pTempStrDataDispaly2);
    //释放信息栏上的文本
    free(pTempStrInfo);

    return iReturn;
}


static int TempWndKey_Down(void *pInArg, int iInLen, 
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
		TempBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int TempWndKey_Up(void *pInArg, int iInLen, 
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
			TempBtnF1_Down(NULL, 0, NULL, 0);
			TempBtnF1_Up(NULL, 0, NULL, 0);
		}
		else
		{
			TempBtnF2_Down(NULL, 0, NULL, 0);
			TempBtnF2_Up(NULL, 0, NULL, 0);
		}
        break;
    case KEYCODE_BACK:
    case KEYCODE_HOME:
        TempBtnBack_Up(pInArg, iInLen, pOutArg, iOutLen);
        break;
    default:
        break;
    }

    return iReturn;
}

static int TempBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pTempBtnF1, pTempLblF1);
	
	char buf[64];
	sprintf(buf, "Tunning Start!");
	FrmTempSetInfo1(buf);
	
	SetPictureEnable(1, pTempBtnF2);
	SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pTempBtnF2);
	DisplayPicture(pTempBtnF2);
	DisplayLabel(pTempLblF2);

	flag = 1;

    return iReturn;
}

static int TempBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pTempBtnF2, pTempLblF2);

	flag = 0;
	
    return iReturn;
}

static void *TempFCT(void *pThreadArg)
{
	pthread_detach(pthread_self());
	
	TempTuning(pApd_Break_Para, 60);
	
	return NULL;
}

//开始
static int TempBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	BoardWorkInit();
		
	iReturn = ThreadCreate(&threadId, TempFCT, NULL);
	if(iReturn == 0)
		printf("-----create TempFCT success!!---\n");
	else
		printf("-----create TempFCT failure!!---\n");
	
    ShowBtn_FuncUp(pTempBtnF1, pTempLblF1);
	
    return iReturn;
}

//开始
static int TempBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	MsecSleep(500);

    ShowBtn_FuncUp(pTempBtnF2, pTempLblF2);

	SetPictureBitmap(BmpFileDirectory"btn_disable.bmp", pTempBtnF2);
	DisplayPicture(pTempBtnF2);
	DisplayLabel(pTempLblF2);
	SetPictureEnable(0, pTempBtnF2);

	char buf[64];
	sprintf(buf, "Tuning Stop!");
	FrmTempSetInfo1(buf);
	
    return iReturn;
}


static int TempBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
	flag = 0;

	MsecSleep(500);

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pTempBtnBack);
    DisplayPicture(pTempBtnBack);
	
    return iReturn;
}

static int TempBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pTempBtnBack);
    DisplayPicture(pTempBtnBack);
	
    GUIWINDOW *pWnd = NULL; 
	
    pWnd = CreateWindow(0, 0, 320, 240, 
                        FrmMainOptInit, FrmMainOptExit, 
                        FrmMainOptPaint, FrmMainOptLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmTemp);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

static int TempErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


int FrmTempSetInfo1(char *pInfo)
{
    int iReturn  = 0;
   
    if (pInfo != NULL)
    {
    	if(pTempStrDataDispaly1 != NULL)
    	{
    		free(pTempStrDataDispaly1);
			pTempStrDataDispaly1 = NULL;
    	}
        pTempStrDataDispaly1 = TransString(pInfo);
        SetLabelText(pTempStrDataDispaly1, pTempLblDataDispaly1);		
		DisplayPicture(pTempPicDataDispaly1);
        DisplayLabel(pTempLblDataDispaly1);
    }
    
    return iReturn;
}

int FrmTempSetInfo2(char *pInfo)
{
    int iReturn  = 0;
    
    if (pInfo != NULL)
    {
    	if(pTempStrDataDispaly2 != NULL)
    	{
    		free(pTempStrDataDispaly2);
			pTempStrDataDispaly2 = NULL;
    	}
        pTempStrDataDispaly2 = TransString(pInfo);
        SetLabelText(pTempStrDataDispaly2, pTempLblDataDispaly2);		
		DisplayPicture(pTempPicDataDispaly2);
        DisplayLabel(pTempLblDataDispaly2);
    }
    
    return iReturn;
}

void initTraceArea(void)
{
	DisplayPicture(pTempPicMainTrce);
}

int GetFlag()
{
	return flag;
}

//按键响应回调函数
// static void KeyBoardCallBack(int iSelected)
// {
// 	static int iKeyFlag = 0;

// 	switch (iSelected)
// 	{
// 	case KEYBOARD_UP:
// 		iKeyFlag = iKeyFlag ? 0 : 1;
// 		break;
// 	case KEYBOARD_DOWN:
// 		iKeyFlag = iKeyFlag ? 0 : 1;
// 		break;
// 	case KEYBOARD_ENTER:
// 		if (!iKeyFlag)
// 		{
// 			TempBtnF1_Down(NULL, 0, NULL, 0);
// 			TempBtnF1_Up(NULL, 0, NULL, 0);
// 		}
// 		else
// 		{
// 			TempBtnF2_Down(NULL, 0, NULL, 0);
// 			TempBtnF2_Up(NULL, 0, NULL, 0);
// 		}		break;
// 	case KEYBOARD_ESC:
// 		break;
// 	default:
// 		break;
// 	}
// }