/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmapd.c
* 摘    要：  实现主窗体frmapd的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-30
*
*******************************************************************************/

#include "guiglobal.h"

#include "wnd_frmapd.h"
#include "wnd_global.h"
#include "wnd_frmmainopt.h"

#include "common_opm.h"
#include "drvoptic.h"
#include "opm.h"

//for apd
#define APDZERO     (0.2f)
#define APDSTEP     (0x10)

//for vdd
#define VDDZERO     (0.05f)
#define VDDSTEP     (0x20)

const static F32 ApdValue[] = {    
	43.0, 
	42.3, 
	39.5, 
	31.2,
};

const static U32 ApdRegValue[] = {    
    0x0900, 
	0x08e0, 
	0x0860, 
	0x0700,
};

const static F32 VddValue[] = {
    2.80, 
	2.62, 
	2.55, 
	2.29,
};

const static U32 VddRegValue[] = {
    0x0680, 
	0x0600, 
	0x0580, 
	0x0500,
};

/* 纭欢灏嗗悗绾ф斁澶ф敼涓轰竴涓€氶亾鍚庯紝鍙娇鐢�5.7鍊嶆斁澶�
 *	ENUM_AMP0 = 0x11, //1.5k & 2.0 RF4
 *	ENUM_AMP1 = 0x21, //1.5k & 5.7 RF4
 *	ENUM_AMP2 = 0x15, //15k & 2.0  RF2
 *	ENUM_AMP3 = 0x25, //15k & 5.7  RF2
 *	ENUM_AMP4 = 0x13, //100k & 2.0 RF3
 *	ENUM_AMP5 = 0x23, //100k & 5.7 RF3
 *	ENUM_AMP6 = 0x17, //300k & 2.0 RF1
 *	ENUM_AMP7 = 0x27, //300k & 5.7 RF1
 *	ENUM_AMP8 = 0x10, //1.8M & 2.0
 *	ENUM_AMP9 = 0x20  //1.8M & 5.7
*/
const static U32 AmpValue[5] = {0x21, 0x25, 0x23, 0x27, 0x20};
extern POPM_TUNING pOpmTunning;
static F32 fadcValue[20];

//按钮区的字体
static GUIFONT *pApdFntWrong = NULL;     //标题按钮的字体
static GUIFONT *pApdFntRight = NULL;     //标题按钮的字体

/************************
* 窗体frmmain中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pApdStrTitle = NULL;
static GUILABEL *pApdLblTitle = NULL;

//桌面上的文本
static GUICHAR *pApdStrF1 = NULL;    //窗体图标1的文本
static GUICHAR *pApdStrF2 = NULL;    //窗体图标2的文本
static GUICHAR *pApdStrF3 = NULL;    //窗体图标3的文本
static GUICHAR *pApdStrF4 = NULL;    //窗体图标4的文本
static GUICHAR *pApdStrF5 = NULL;    //窗体图标5的文本
static GUICHAR *pApdStrF6 = NULL;    //窗体图标6的文本

static GUICHAR *pApdStrDataLine[20];
static GUILABEL *pApdLblDataLine[20];

//信息栏上文本
static GUICHAR *pApdStrInfo = NULL;    //提示信息的文本

/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmApd = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pApdBarStatus = NULL;
static GUIPICTURE *pApdBgDesk = NULL;
static GUIPICTURE *pApdBarInfo = NULL;

//桌面上的控件
static GUIPICTURE *pApdBtnF1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pApdBtnF2 = NULL;     //窗体图标2图形框
static GUIPICTURE *pApdBtnF3 = NULL;     //窗体图标3图形框
static GUIPICTURE *pApdBtnF4 = NULL;     //窗体图标4图形框
static GUIPICTURE *pApdBtnF5 = NULL;     //窗体图标5图形框
static GUIPICTURE *pApdBtnF6 = NULL;     //窗体图标6图形框

static GUILABEL *pApdLblF1 = NULL;       //窗体图标1标签
static GUILABEL *pApdLblF2 = NULL;       //窗体图标2标签
static GUILABEL *pApdLblF3 = NULL;       //窗体图标3标签
static GUILABEL *pApdLblF4 = NULL;       //窗体图标4标签
static GUILABEL *pApdLblF5 = NULL;       //窗体图标5标签
static GUILABEL *pApdLblF6 = NULL;       //窗体图标6标签

//数据显示区
static GUIPICTURE *pApdPicDataDispaly = NULL;
static GUILABEL *pApdLblDataDispaly = NULL;
static GUICHAR *pApdStrDataDispaly = NULL;    

//信息栏上的控件
static GUILABEL *pApdLblInfo = NULL;       //提示信息标签

static GUIPICTURE *pApdBtnBack = NULL;

/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int ApdTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int ApdTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int ApdWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int ApdWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int ApdBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ApdBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ApdBtnF3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ApdBtnF4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ApdBtnF5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ApdBtnF6_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ApdBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int ApdBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int ApdBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int ApdBtnF3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int ApdBtnF4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int ApdBtnF5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int ApdBtnF6_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int ApdBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int ApdErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int ConnectMCU();
static int VoltageCheck();
static int Apd_FCT();
static int Vdd_FCT();
//static int Att_FCT();
static int Delete_Att(void);
static int SignalChain();

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
int FrmApdInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

	
    //得到当前窗体对象
    pFrmApd = (GUIWINDOW *) pWndObj;

    //初始化字体资源
    pApdFntWrong = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);
    pApdFntRight = CreateFont(FNTFILE_STD, 16, 16, 0x00FFFFFF, 0xFFFFFFFF);

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    ApdTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pApdBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30,
                                   BmpFileDirectory"bar_status.bmp");
	pApdBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420,
							   BmpFileDirectory "bg_desk.bmp");
	pApdBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30,
								BmpFileDirectory "bar_info.bmp");
	pApdLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pApdStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblTitle);
	
    //建立桌面上的控件
    pApdBtnF1 = CreatePicture(384, 35, 80, 53,
                                 BmpFileDirectory"btn_enable.bmp");
	pApdBtnF2 = CreatePicture(496, 35, 80, 53,
							  BmpFileDirectory "btn_disable.bmp");
	pApdBtnF3 = CreatePicture(496, 95, 80, 53,
							  BmpFileDirectory "btn_disable.bmp");
	pApdBtnF4 = CreatePicture(496, 155, 80, 53,
							  BmpFileDirectory "btn_disable.bmp");
	pApdBtnF5 = CreatePicture(496, 215, 80, 53,
							  BmpFileDirectory "btn_disable.bmp");
	pApdBtnF6 = CreatePicture(496, 275, 80, 53,
							  BmpFileDirectory "btn_enable.bmp");

	pApdLblF1 = CreateLabel(389, 44, 80, 53, pApdStrF1); //connect mcu
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblF1);
	pApdLblF2 = CreateLabel(501, 44, 80, 53, pApdStrF2); //voltage check
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblF2);
	pApdLblF3 = CreateLabel(501, 104, 80, 53, pApdStrF3); //apd_fct
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblF3);
	pApdLblF4 = CreateLabel(501, 164, 80, 53, pApdStrF4); //vdd_fct
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblF4);
	pApdLblF5 = CreateLabel(501, 224, 80, 53, pApdStrF5); //signal_fct
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblF5);
	pApdLblF6 = CreateLabel(501, 284, 80, 53, pApdStrF6); //delete_att
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblF6);
    
	pApdPicDataDispaly = CreatePicture(5, 35, 320, 400, BmpFileDirectory"frm_dataInfo.bmp");
	pApdLblDataDispaly = CreateLabel(5, 220, 320, 16, pApdStrDataDispaly);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pApdLblDataDispaly);
    //数据框中的数据
    for(i = 0; i < 20; i++)
   	{
		pApdLblDataLine[i] = CreateLabel(20, 50 + 19 * i, 296, 16, pApdStrDataLine[i]);
	}

	pApdBtnBack = CreatePicture(520, 454, 60, 25, BmpFileDirectory"btn_back_unpress.bmp");

    //建立信息栏上的控件
    pApdLblInfo = CreateLabel(20, 458, 300, 16, pApdStrInfo);


    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmApd->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmApd, 
                  pFrmApd);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pApdBtnF1, 
                  pFrmApd);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pApdBtnF2, 
                  pFrmApd);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pApdBtnF3, 
                  pFrmApd);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pApdBtnF4, 
                  pFrmApd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pApdBtnF5, 
                  pFrmApd);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pApdBtnF6, 
                  pFrmApd);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pApdBtnBack, 
                  pFrmApd);

    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmApd->Mutex));    //注意，必须释放锁

    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmApd, 
                    ApdWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmApd, 
                    ApdWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pApdBtnF1, 
                    ApdBtnF1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pApdBtnF2, 
                    ApdBtnF2_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pApdBtnF3, 
                    ApdBtnF3_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pApdBtnF4, 
                    ApdBtnF4_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pApdBtnF5, 
                    ApdBtnF5_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pApdBtnF6, 
                    ApdBtnF6_Down, NULL, 0, pMsg);
	
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pApdBtnBack, 
                    ApdBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pApdBtnF1, 
                    ApdBtnF1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pApdBtnF2, 
                    ApdBtnF2_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pApdBtnF3, 
                    ApdBtnF3_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pApdBtnF4, 
                    ApdBtnF4_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pApdBtnF5, 
                    ApdBtnF5_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pApdBtnF6, 
                    ApdBtnF6_Up, NULL, 0, pMsg);
  
    LoginMessageReg(GUIMESSAGE_TCH_UP, pApdBtnBack, 
                    ApdBtnBack_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmApd, 
                    ApdErrProc_Func, NULL, 0, pMsg);

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
int FrmApdExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmApd = (GUIWINDOW *) pWndObj;
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmApd->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmApd);
    MutexUnlock(&(pFrmApd->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pApdBarStatus);
    DestroyPicture(&pApdBgDesk);
    DestroyPicture(&pApdBarInfo);
    DestroyLabel(&pApdLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyPicture(&pApdBtnF1);
    DestroyPicture(&pApdBtnF2);
    DestroyPicture(&pApdBtnF3);
    DestroyPicture(&pApdBtnF4);
 	DestroyPicture(&pApdBtnF5);
 	DestroyPicture(&pApdBtnF6);
	
    DestroyLabel(&pApdLblF1);
    DestroyLabel(&pApdLblF2);
    DestroyLabel(&pApdLblF3);
    DestroyLabel(&pApdLblF4);
	DestroyLabel(&pApdLblF5);
	DestroyLabel(&pApdLblF6);
	
    for(i = 0; i < 20; i++)
   	{
		DestroyLabel(&pApdLblDataLine[i]);
	}
    
    DestroyPicture(&pApdPicDataDispaly);
    DestroyLabel(&pApdLblDataDispaly);
    //销毁信息栏上的控件
    DestroyLabel(&pApdLblInfo);
	
    DestroyPicture(&pApdBtnBack);

    //释放文本资源
    //***************************************************************/
    ApdTextRes_Exit(NULL, 0, NULL, 0);

	SerialClose();
	
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
int FrmApdPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

    //得到当前窗体对象
    pFrmApd = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pApdBarStatus);
    DisplayPicture(pApdBgDesk);
    DisplayPicture(pApdBarInfo);
    DisplayLabel(pApdLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayPicture(pApdBtnF1);
    DisplayPicture(pApdBtnF2);
    DisplayPicture(pApdBtnF3);
    DisplayPicture(pApdBtnF4);
	DisplayPicture(pApdBtnF5);
	DisplayPicture(pApdBtnF6);
	
    DisplayLabel(pApdLblF1);
    DisplayLabel(pApdLblF2);
    DisplayLabel(pApdLblF3);
    DisplayLabel(pApdLblF4);
	DisplayLabel(pApdLblF5);
	DisplayLabel(pApdLblF6);

    for(i = 0; i < 20; i++)
   	{
		DisplayLabel(pApdLblDataLine[i]);
	}

    DisplayPicture(pApdPicDataDispaly);
    DisplayLabel(pApdLblDataDispaly);
    //显示信息栏上的控件
    DisplayLabel(pApdLblInfo);
    DisplayPicture(pApdBtnBack);

	SetPictureEnable(0, pApdBtnF2);
	SetPictureEnable(0, pApdBtnF3);
	SetPictureEnable(0, pApdBtnF4);
	SetPictureEnable(0, pApdBtnF5);

	GUIPEN *pPen = NULL;
    unsigned int uiColor;
    pPen = GetCurrPen();
    uiColor = pPen->uiPenColor;
	pPen->uiPenColor = 0x00FFFFFF;
	DrawLine(469, 62, 496, 62);
	DrawLine(481, 62, 481, 242);
	DrawLine(481, 122, 496, 122);
	DrawLine(481, 182, 496, 182);
	DrawLine(481, 242, 496, 242);
    pPen->uiPenColor = uiColor;
		
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
int FrmApdLoop(void *pWndObj)
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
int FrmApdPause(void *pWndObj)
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
int FrmApdResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int ApdTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //初始化状态栏上的文本
    pApdStrTitle = TransString("**APD FCT**");
    //初始化桌面上的文本
    pApdStrF1 = TransString("Connect MCU");
    pApdStrF2 = TransString("Voltage Check");
    pApdStrF3 = TransString("APD_FCT");
    pApdStrF4 = TransString("VDD_FCT");
	pApdStrF5 = TransString("SIGNAL_FCT");
	pApdStrF6 = TransString("DELETE ATT");
	for(i = 0; i < 10; i++)
   	{
		pApdStrDataLine[i] = TransString("");
	}
    //初始化信息栏上的文本
    pApdStrInfo = TransString("");

    pApdStrDataDispaly = TransString("");

    return iReturn;
}


//释放文本资源
static int ApdTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //释放状态栏上的文本
    free(pApdStrTitle);
    //释放桌面上的文本
    free(pApdStrF1);
    free(pApdStrF2);
    free(pApdStrF3);
    free(pApdStrF4);
	free(pApdStrF5);
	free(pApdStrF6);
    for(i = 0; i < 10; i++)
   	{
		free(pApdStrDataLine[i]);
	}

    free(pApdStrDataDispaly);
    //释放信息栏上的文本
    free(pApdStrInfo);

    return iReturn;
}


static int ApdWndKey_Down(void *pInArg, int iInLen, 
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
		ApdBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int ApdWndKey_Up(void *pInArg, int iInLen, 
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
	case KEYCODE_left:
		if (iKeyFlag == 1)
		{
			iKeyFlag--;
		}
		break;
	case KEYCODE_right:
		if (iKeyFlag == 0)
		{
			iKeyFlag++;
		}
		break;
	case KEYCODE_up:
		if (iKeyFlag > 1 && iKeyFlag <= 5)
		{
			iKeyFlag--;
		}
		break;
	case KEYCODE_down:
		if (iKeyFlag >= 1 && iKeyFlag < 5)
		{
			iKeyFlag++;
		}
		break;
	case KEYCODE_enter:
		KeyEnterCallBack(iKeyFlag);
		break;
	case KEYCODE_BACK:
		ApdBtnBack_Up(NULL, 0, NULL, 0);
		break;
	default:
        break;
    }

	return iReturn;
}

static int ApdBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pApdBtnF1, pApdLblF1);

    return iReturn;
}


static int ApdBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pApdBtnF2, pApdLblF2);

    return iReturn;
}


static int ApdBtnF3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pApdBtnF3, pApdLblF3);

    return iReturn;
}


static int ApdBtnF4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pApdBtnF4, pApdLblF4);

    return iReturn;
}

static int ApdBtnF5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pApdBtnF5, pApdLblF5);

    return iReturn;
}
static int ApdBtnF6_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pApdBtnF6, pApdLblF6);

    return iReturn;
}

static int ApdBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	SetPictureEnable(0, pApdBtnBack);

	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("please wait ......!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	
	ConnectMCU();

	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);

	
	ShowBtn_FuncUp(pApdBtnF1, pApdLblF1);

	SetPictureEnable(1, pApdBtnBack);
	DisplayPicture(pApdBtnBack);
	
    return iReturn;
}


static int ApdBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	VoltageCheck();
		
    ShowBtn_FuncUp(pApdBtnF2, pApdLblF2);
	
    return iReturn;
}


static int ApdBtnF3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	Apd_FCT();
	
    ShowBtn_FuncUp(pApdBtnF3, pApdLblF3);
		
    return iReturn;
}


static int ApdBtnF4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	Vdd_FCT();
	
    ShowBtn_FuncUp(pApdBtnF4, pApdLblF4);

    return iReturn;
}
static int ApdBtnF5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//Att_FCT();
	SignalChain();
	
    ShowBtn_FuncUp(pApdBtnF5, pApdLblF5);

    return iReturn;
}
static int ApdBtnF6_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
	//Att_FCT();
	
	Delete_Att();
    ShowBtn_FuncUp(pApdBtnF6, pApdLblF6);

    return iReturn;
}

static int ApdBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pApdBtnBack);
    DisplayPicture(pApdBtnBack);

    return iReturn;
}

static int ApdBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;;
    //临时变量定义
	
    GUIWINDOW *pWnd = NULL; 

	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pApdBtnBack);
    DisplayPicture(pApdBtnBack);
	
    pWnd = CreateWindow(0, 0, 800, 480, 
                        FrmMainOptInit, FrmMainOptExit, 
                        FrmMainOptPaint, FrmMainOptLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmApd);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}


static int ApdErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


static int VoltageCheck()
{
	int iReturn = 0;
    int ret = 0;	
	char buff[64];
	char *flag = NULL;
	int i = 0;
	int num = 0;
	
	for(i = 0; i < 10; i++)
	{
		fadcValue[i] = 99.99999f;
	}

	for(i = 0; i < 10; i++)
	{
		if(pApdStrDataLine[i] != NULL)
		{
			free(pApdStrDataLine[i]);
			pApdStrDataLine[i] = NULL;
		}
	}

	if(flag == NULL)
	{
		flag = (char *)malloc(sizeof(char) * 10);
	}

	DisplayPicture(pApdPicDataDispaly);

	//-1--VSYS
	ret = OPMGetZhijuAdcValue(ADC_VSYS_TP97, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP7", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);
		
	MsecSleep(100);
	num++;
	//-2--VDD_MCU
	ret = OPMGetZhijuAdcValue(ADC_VSYS_1V8_TP103, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP1", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-3--
	ret = OPMGetZhijuAdcValue(ADC_FPGA_CORE_1V2_TP100, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP16", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-4--
	ret = OPMGetZhijuAdcValue(ADC_ADC_3V_TP99, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP18", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-5--
	ret = OPMGetZhijuAdcValue(ADC_OPM_PWR_5V_TP113, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP12", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);
	
	MsecSleep(100);
	num++;
	//-6--
	ret = OPMGetZhijuAdcValue(ADC_ARM_1V0_TP105, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP21", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-7--
	ret = OPMGetZhijuAdcValue(ADC_5V_A0, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP20", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-8--
	ret = OPMGetZhijuAdcValue(ADC_OPM_7V_TP111, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP11", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-9--
	ret = OPMGetZhijuAdcValue(ADC_VSYSIO_SD_3V3_TP24, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP26", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-10--
	ret = OPMGetZhijuAdcValue(ADC_5V_A1, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP17", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);
#if 1
	MsecSleep(100);
	num++;
	//-11--
	ret = OPMGetZhijuAdcValue(ADC_3_3V_A_TP14, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP23", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-12--
	ret = OPMGetZhijuAdcValue(ADC_7V_A_TP12, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP27", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-13--
	ret = OPMGetZhijuAdcValue(ADC_5V_A2, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP13", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-14--
	#if 1
	ret = OPMGetZhijuAdcValue(ADC_F5V_A2, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP15", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	#endif
	//-15--
	ret = OPMGetZhijuAdcValue(ADC_OPM_7V_TP114, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP14", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

	MsecSleep(100);
	num++;
	//-16--
	ret = OPMGetZhijuAdcValue(ADC_F5V_A1, 0x07, &fadcValue[num], NULL, GET_ADC_VALUE_FCT, COMMAND_LEN);
	if((fadcValue[num] < 1.10f) && (fadcValue[num] > 0.85f))
	{
		strncpy(flag, "OK", 10);
		SetLabelFont(pApdFntRight, pApdLblDataLine[num]);
	}
	else
	{
		strncpy(flag, "F.", 10);
		SetLabelFont(pApdFntWrong, pApdLblDataLine[num]);
	}
	sprintf((char *)buff, " %s %s: %7.5fV ", flag, "TP19", fadcValue[num]);
	pApdStrDataLine[num] = TransString(buff);
	SetLabelText(pApdStrDataLine[num], pApdLblDataLine[num]);
	DisplayLabel(pApdLblDataLine[num]);

#endif
	
	for(i = 0; i < 20; i++)
	{
		SetLabelFont(pApdFntRight, pApdLblDataLine[i]);
	}

	return iReturn;
}

static int Apd_FCT()
{
	int iReturn = 0;
	S32 i = 0;
    S32 ret = 0;
    S32 tryTimes = 0;
	char buf[128] = {0};
	F32 adcValue34 = 0.0;
    U32 regValueTmp = 0;
	int iFail = 0;

	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("APD FCT Satrt!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	OPMSetApd(0x0880);
	MsecSleep(1000);
	ret = OPMGetAdcValue(3, 0x07, &adcValue34);
	
    for (i = 0; i < OPM_APD_KIND; ++i)
    {
    	
        regValueTmp = ApdRegValue[i];
		OPMSetApd(regValueTmp);
        tryTimes = 100;
        do
        {
        	MsecSleep(500);
            ret = OPMGetAdcValue(3, 0x07, &adcValue34);
			if(ret)
                continue;
			sprintf(buf, "%d AD Value :%7.5fV, times:%d, reg:0x%x", i, adcValue34, tryTimes, regValueTmp);
			if(pApdStrDataDispaly != NULL)
			{
				free(pApdStrDataDispaly);
				pApdStrDataDispaly = NULL;
			}
		    pApdStrDataDispaly = TransString(buf);
			SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
			SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
			DisplayPicture(pApdPicDataDispaly);
			DisplayLabel(pApdLblDataDispaly);
            
            ret = IsEqual(adcValue34, ApdValue[i], APDZERO);
            if (ret < 0)
            {
            	regValueTmp += APDSTEP;
                if (regValueTmp < 0x0c00)
                {                    	                    
                    OPMSetApd(regValueTmp);					
                }
				else
					regValueTmp -= APDSTEP;
            }
            else if (ret > 0)
            {
            	regValueTmp -= APDSTEP;
                if (regValueTmp > 0x0800)
                { 
                    OPMSetApd(regValueTmp);
                }
				else
					regValueTmp += APDSTEP;
            }
            else
            {
                break;
            }
        }while(tryTimes--);
        
        if(tryTimes > 0)
        {
			pOpmTunning->iApd[i]= regValueTmp;
        }
        else
        {
        	iFail++;
            perror("Time Over");
        }
    }

	if(iFail > 0)
	{
		if(pApdStrDataDispaly != NULL)
		{
			free(pApdStrDataDispaly);
			pApdStrDataDispaly = NULL;
		}
	    pApdStrDataDispaly = TransString("Sorry, APD FCT Fail!!!");
		SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
		SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
		DisplayPicture(pApdPicDataDispaly);
		DisplayLabel(pApdLblDataDispaly);
		
		return iReturn;
	}
	
	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("APD FCT Over!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	MsecSleep(1000);

	SaveOpmTunning();
	
	DisplayPicture(pApdPicDataDispaly);
	for(i=0; i<OPM_APD_KIND; i++)
	{
		if(pApdStrDataLine[i] != NULL)
		{
			free(pApdStrDataLine[i]);
			pApdStrDataLine[i] = NULL;
		}
		sprintf(buf, "APD[%d] = 0x%x --- %f",i,pOpmTunning->iApd[i],ApdValue[i]);
        pApdStrDataLine[i] = TransString(buf);
		SetLabelText(pApdStrDataLine[i],pApdLblDataLine[i]);
		SetLabelFont(pApdFntRight, pApdLblDataLine[i]);   		
		DisplayLabel(pApdLblDataLine[i]);
	}
	
	return iReturn;
}

static int Vdd_FCT()
{
	int iReturn = 0;
	S32 i = 0;
    S32 ret = 0;
    S32 tryTimes = 0;
    U16 regValueTmp = 0;
   	F32 adcValue46 = 0.0;
	char buf[128] = {0};
	int iFail = 0;

	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("VDD FCT Satrt!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	MsecSleep(1000);

	
    for (i = 0; i < OPM_VDD_KIND; ++i)
    {
        tryTimes = 100;
        regValueTmp = VddRegValue[i];
		OPMSetVdd(regValueTmp);
        do
        {
        	MsecSleep(500);
			ret = OPMGetAdcValue(2, 0x07, &adcValue46);
			if(ret)
                continue;
			sprintf(buf, "%d AD Value:%7.5fV, times:%d, reg:0x%x", i, adcValue46, tryTimes, regValueTmp);
			if(pApdStrDataDispaly != NULL)
			{
				free(pApdStrDataDispaly);
				pApdStrDataDispaly = NULL;
			}
		    pApdStrDataDispaly = TransString(buf);
			SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
			SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
			DisplayPicture(pApdPicDataDispaly);
			DisplayLabel(pApdLblDataDispaly);
			
            ret = IsEqual(adcValue46, VddValue[i], VDDZERO);
            if (ret < 0)
            {
            	regValueTmp += VDDSTEP;
                if (regValueTmp < (0x0b50-VDDSTEP))
                {                    
                    OPMSetVdd(regValueTmp);
                }
				else
					regValueTmp -= VDDSTEP;
            }
            else if (ret > 0)
            {
            	regValueTmp -= VDDSTEP;
                if (regValueTmp > 0x02a0+VDDSTEP)
                {
                    OPMSetVdd(regValueTmp);
                }
				else
					regValueTmp += VDDSTEP;
            }
            else
            {
                break;
            }
        } while(tryTimes--);
		
        if (tryTimes > 0)
        {
			pOpmTunning->iVdd[i] = regValueTmp;
        }
        else
        {
        	iFail++;
            perror("Time Over");
        }
    }

	if(iFail > 0)
	{
		if(pApdStrDataDispaly != NULL)
		{
			free(pApdStrDataDispaly);
			pApdStrDataDispaly = NULL;
		}
	    pApdStrDataDispaly = TransString("Sorry, VDD FCT Fail!!!");
		SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
		SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
		DisplayPicture(pApdPicDataDispaly);
		DisplayLabel(pApdLblDataDispaly);
		
		return iReturn;
	}
	
	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("VDD FCT Over!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	MsecSleep(1000);

	SaveOpmTunning();
	
	DisplayPicture(pApdPicDataDispaly);
	for(i=0; i<OPM_VDD_KIND; i++)
	{
		if(pApdStrDataLine[i] != NULL)
		{
			free(pApdStrDataLine[i]);
			pApdStrDataLine[i] = NULL;
		}
		sprintf(buf, "VDD[%d] = 0x%x --- %f",i,pOpmTunning->iVdd[i],VddValue[i]);
        pApdStrDataLine[i] = TransString(buf);
		SetLabelText(pApdStrDataLine[i],pApdLblDataLine[i]);
		SetLabelFont(pApdFntRight, pApdLblDataLine[i]);   		
		DisplayLabel(pApdLblDataLine[i]);
	}
	
	return iReturn;
}


static int Delete_Att(void)
{
	int iReturn = 0;

	int i = 0;

	pApdStrDataDispaly = TransString("DELETE ATT Satrt!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	MsecSleep(1000);
	
	for(;i<OPM_AMP_KIND;i++)
	{
		pOpmTunning->iAtt[i] = 1;
	}
	SaveOpmTunning();
	
	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
	
    pApdStrDataDispaly = TransString("DELETE ATT Success!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	return iReturn;
}

#if 0
static int Att_FCT()
{
	int iReturn = 0;
	S32 i = 0;
    S32 tryTimes = 0;
    U32 regValueTmp = 0;
	F32 iSignalMean = 0;
	char buf[256] = {0};
	
	fpga_init(FPGA_DATA_NAME);
	MsecSleep(200);

	InitFpgaRegVal();
	BoardWorkInit();
	//激光器脉冲关闭
	Opm_LdPulse(0);
    OPMSetApd(0);
	OPMSetVdd(0);
	
	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("ATT FCT Satrt!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	MsecSleep(1000);
	
	for (i = 0; i < OPM_AMP_KIND; i++)
	{
		tryTimes = 200;
		regValueTmp = 0x0730;
		OPMSetAtt(regValueTmp);		
		MsecSleep(100);

		do
	    {
			MsecSleep(100);
			iSignalMean = GetAttData(AmpValue[i]);

			if(pApdStrDataDispaly != NULL)
			{
				free(pApdStrDataDispaly);
				pApdStrDataDispaly = NULL;
			}
			sprintf(buf, "%d  iSignalMean:%f, times:%d, reg:0x%x",i, iSignalMean, tryTimes, regValueTmp);
	        pApdStrDataDispaly = TransString(buf);
			SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
			SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
			DisplayPicture(pApdPicDataDispaly);
			DisplayLabel(pApdLblDataDispaly);
			
	        if(iSignalMean < 58000)
	        {
	            regValueTmp += 0x20;
	            if ((regValueTmp < (0x0600)) && (regValueTmp > 0x0900))
	            {                
	                OPMSetAtt(regValueTmp);
	            }
				else
				{
					regValueTmp = 0x0830;
					OPMSetAtt(regValueTmp);
				}
				
	        }
	        else if (iSignalMean > 63000)
	        {
	        	regValueTmp -= 0x20;
	            if ((regValueTmp < (0x0600)) && (regValueTmp > 0x0900))
	            {                
	                OPMSetAtt(regValueTmp);
	            }
				else
				{
					regValueTmp = 0x0830;
					OPMSetAtt(regValueTmp);
				}
	        }
	        else
	        {
	            break;
	        }
	    } while(tryTimes--);

	    if(tryTimes > 0)
	    {
			pOpmTunning->iAtt[i] = regValueTmp;
	    }
	    else
	    {
	        printf("---time over---\n");
	    }
		
	}	
	
	SaveOpmTunning();

	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("ATT FCT Over!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	MsecSleep(1000);

	DisplayPicture(pApdPicDataDispaly);
	for(i=0; i<OPM_AMP_KIND; i++)
	{

		if(pApdStrDataLine[i] != NULL)
		{
			free(pApdStrDataLine[i]);
			pApdStrDataLine[i] = NULL;
		}
		sprintf(buf, "ATT[%d] = 0x%x",i,pOpmTunning->iAtt[i]);
        pApdStrDataLine[i] = TransString(buf);
		SetLabelText(pApdStrDataLine[i],pApdLblDataLine[i]);
		SetLabelFont(pApdFntRight, pApdLblDataLine[i]);   		
		DisplayLabel(pApdLblDataLine[i]);

	}
	return iReturn;
}

#endif

static int SignalChain()
{	
	F32 iSignalMean = 0;
	int i = 0;
	int iFail = 0;
		
	char buf[256] = {0};
	
	char *Buf[5] = {
		"1.5k----0x16",
		"15k-----0x17",
		"102k----0x15",
		"300k----0x14",
		"1.96M---0x08"};


	//int ValueBuf[5][2] = {{61266,49054},{59707,45994},
	//	{60304,49938},{58955,31865},{59765,38500}};
	int ValueBuf[5][2] = {{65160,61311},{64171,56594},
		{64197,58219},{63210,47737},{63037,55324}};

	int absValue[5] = {2000,2500,2000,2500,3500};
	
	int Value[5][2] = {{0}};
	
	//fpga_init(FPGA_DATA_NAME);
	MsecSleep(200);

	InitFpgaRegVal();
	BoardWorkInit();
    OPMSetApd(0);
	OPMSetVdd(0);
	MsecSleep(200);
	//激光器脉冲关闭
	Opm_LdPulse(0);
	OPMSetAtt(0x0a80);
	
	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("Signal Chain Test Satrt!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	MsecSleep(1000);

	OPMSetDacPulse(VOLTAGE_0, 0X07);
	for (i = 0; i < 5; i++)
	{
		OPMSetDacPulse(i + CHANNEL_1, 0X07);
		
		SignalChainSet(AmpValue[i]);
		int j = 0;
		for(j = 0;j < 10; j++)
		{
			if(j%2 == 0)
			{
				//设置单片机信号
				OPMSetDacPulse(VOLTAGE_1, 0X07);
			}
			else
			{
				//设置单片机信号
				OPMSetDacPulse(VOLTAGE_0, 0X07);
			}
			
			iSignalMean = GetData();

			sprintf(buf, "0x%04x: -%d- AD:%7.5f", AmpValue[i],j, iSignalMean);
			if(pApdStrDataDispaly != NULL)
			{
				free(pApdStrDataDispaly);
				pApdStrDataDispaly = NULL;
			}
		    pApdStrDataDispaly = TransString(buf);
			SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
			SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
			DisplayPicture(pApdPicDataDispaly);
			DisplayLabel(pApdLblDataDispaly);

			
			if(j%2 == 0)
			{
				Value[i][1] += (int)iSignalMean;
			}
			else
			{
				Value[i][0] += (int)iSignalMean;
			}
			
			MsecSleep(200);//200
		}

		Value[i][0] /= 5;
		Value[i][1] /= 5;

		if((0 == IsEqual(Value[i][0], ValueBuf[i][0], absValue[i]))\
			&& (0 == IsEqual(Value[i][1], ValueBuf[i][1], absValue[i])))
		{
			sprintf(buf, "0x%04x: -%d- AD:%d--%d", AmpValue[i], i, Value[i][0], Value[i][1]);
		}
		else
		{
			sprintf(buf, "0x%04x: -%d- AD:%d--%d", AmpValue[i], i, Value[i][0], Value[i][1]);
			iFail += (1 << i);
		}
		
		if(pApdStrDataDispaly != NULL)
		{
			free(pApdStrDataDispaly);
			pApdStrDataDispaly = NULL;
		}
		pApdStrDataDispaly = TransString(buf);
		SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
		SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
		DisplayPicture(pApdPicDataDispaly);
		DisplayLabel(pApdLblDataDispaly);

		MsecSleep(500);//500
	}
	
	OPMSetDacPulse(VOLTAGE_0, 0X07);
	

	if(pApdStrDataDispaly != NULL)
	{
		free(pApdStrDataDispaly);
		pApdStrDataDispaly = NULL;
	}
    pApdStrDataDispaly = TransString("Signal Chain Test Over!");
	SetLabelText(pApdStrDataDispaly,pApdLblDataDispaly);
	SetLabelFont(pApdFntRight, pApdLblDataDispaly);   		
	DisplayPicture(pApdPicDataDispaly);
	DisplayLabel(pApdLblDataDispaly);
	
	MsecSleep(500);

	DisplayPicture(pApdPicDataDispaly);
	for(i=0; i<5; i++)
	{
		if(pApdStrDataLine[i] != NULL)
		{
			free(pApdStrDataLine[i]);
			pApdStrDataLine[i] = NULL;
		}
		if(iFail & (1 << i))
		{
			sprintf(buf, "-%d- %s Fail!!!",i,Buf[i]);
		}
		else
		{
			sprintf(buf, "-%d- %s Success!",i,Buf[i]);
		}
        pApdStrDataLine[i] = TransString(buf);
		SetLabelText(pApdStrDataLine[i],pApdLblDataLine[i]);
		SetLabelFont(pApdFntRight, pApdLblDataLine[i]);   		
		DisplayLabel(pApdLblDataLine[i]);
	}	

	return 0;
}


static int ConnectMCU()
{
	static S32 ret = 0;
    S32 tryTimes = 3;
    U08 SendBuffer[BUFFER_SIZE];
	U08 RecvBuffer[BUFFER_SIZE];
	
    ret = SerialInit();

    if (!ret)
    { 
        SendBuffer[0] = 2;
        SendBuffer[1] = CONNECT;
        SendBuffer[2] = '#';
        while (tryTimes--)
        {
            SerialSendData(SendBuffer, SendBuffer[0]+1);
            ret = SerialRecvData(RecvBuffer, sizeof(RecvBuffer), COMMAND_LEN);
			
            if (ret)
            {
            	printf("--apd connect ---ret = %d---\n",ret);
                if(pApdStrInfo != NULL)
				{
					free(pApdStrInfo);
					pApdStrInfo = NULL;
				}
				pApdStrInfo = TransString("Connect MCU Time Out!");
				SetLabelText(pApdStrInfo,pApdLblInfo);
	   			SetLabelFont(pApdFntWrong, pApdLblInfo);
				DisplayPicture(pApdBarInfo);
				DisplayPicture(pApdBtnBack);
				DisplayLabel(pApdLblInfo);
                MsecSleep(50);
                continue;
            }
            else
            {
                if (RecvBuffer[1] == CONNECT)
                {
                    break;
                }
                else
                {
                    MsecSleep(50);
                    continue;
                }
            }
         }
         if (tryTimes >= 0)
         {
         	if(pApdStrInfo != NULL)
			{
				free(pApdStrInfo);
				pApdStrInfo = NULL;
			}
			pApdStrInfo = TransString("Connected Success......Oh yeah!");
			SetLabelText(pApdStrInfo,pApdLblInfo);
   			SetLabelFont(pApdFntRight, pApdLblInfo);
			DisplayPicture(pApdBarInfo);
			DisplayPicture(pApdBtnBack);
			DisplayLabel(pApdLblInfo);

			SetPictureEnable(1, pApdBtnF2);
			SetPictureEnable(1, pApdBtnF3);
			SetPictureEnable(1, pApdBtnF4);
			SetPictureEnable(1, pApdBtnF5);
			SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pApdBtnF2);
			SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pApdBtnF3);
			SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pApdBtnF4);
			SetPictureBitmap(BmpFileDirectory"btn_enable.bmp", pApdBtnF5);
			DisplayPicture(pApdBtnF2);
			DisplayLabel(pApdLblF2);
			DisplayPicture(pApdBtnF3);
			DisplayLabel(pApdLblF3);
			DisplayPicture(pApdBtnF4);
			DisplayLabel(pApdLblF4);
			DisplayPicture(pApdBtnF5);
			DisplayLabel(pApdLblF5);
         }
         else
         {   
        	 if (serialFD != -1)
             {
                 SerialClose();
             }
             if(pApdStrInfo != NULL)
			 {
				 free(pApdStrInfo);
				 pApdStrInfo = NULL;
			 }
			 pApdStrInfo = TransString("Connected Failure......oh sorry!");
			 SetLabelText(pApdStrInfo,pApdLblInfo);
   			 SetLabelFont(pApdFntWrong, pApdLblInfo);
			 DisplayPicture(pApdBarInfo);
			 DisplayPicture(pApdBtnBack);
			 DisplayLabel(pApdLblInfo);
         }
    }
    else if(ret == REOPENSERIAL)
    {
        SerialClose();
        if(pApdStrInfo != NULL)
		{
			free(pApdStrInfo);
			pApdStrInfo = NULL;
		}
		pApdStrInfo = TransString("close the serial device!");
		SetLabelText(pApdStrInfo,pApdLblInfo);
		SetLabelFont(pApdFntWrong, pApdLblInfo);
		DisplayPicture(pApdBarInfo);
		DisplayPicture(pApdBtnBack);
		DisplayLabel(pApdLblInfo);

		SetPictureBitmap(BmpFileDirectory"btn_disable.bmp", pApdBtnF2);
		SetPictureBitmap(BmpFileDirectory"btn_disable.bmp", pApdBtnF3);
		SetPictureBitmap(BmpFileDirectory"btn_disable.bmp", pApdBtnF4);
		SetPictureBitmap(BmpFileDirectory"btn_disable.bmp", pApdBtnF5);
		DisplayPicture(pApdBtnF3);
		DisplayLabel(pApdLblF3);
		DisplayPicture(pApdBtnF4);
		DisplayLabel(pApdLblF4);
		DisplayPicture(pApdBtnF2);
		DisplayLabel(pApdLblF2);
		DisplayPicture(pApdBtnF5);
		DisplayLabel(pApdLblF5);
		SetPictureEnable(0, pApdBtnF2);
		SetPictureEnable(0, pApdBtnF3);
		SetPictureEnable(0, pApdBtnF4);
		SetPictureEnable(0, pApdBtnF5);
    }
	else
	{
		printf("--apd connect ---ret = %d---\n",ret);
        if(pApdStrInfo != NULL)
		{
			free(pApdStrInfo);
			pApdStrInfo = NULL;
		}
		pApdStrInfo = TransString("unknown err!");
		SetLabelText(pApdStrInfo,pApdLblInfo);
		SetLabelFont(pApdFntWrong, pApdLblInfo);
		DisplayPicture(pApdBarInfo);
		DisplayPicture(pApdBtnBack);
		DisplayLabel(pApdLblInfo);
	}

	return ret;
}

//enter键响应处理函数
static void KeyEnterCallBack(int iKeyFlag)
{
	switch (iKeyFlag)
	{
	case 0://connect mcu
 		ApdBtnF1_Down(NULL, 0, NULL, 0);
 		ApdBtnF1_Up(NULL, 0, NULL, 0);
		break;
	case 1://voltage check
		ApdBtnF2_Down(NULL, 0, NULL, 0);
		ApdBtnF2_Up(NULL, 0, NULL, 0);
		break;
	case 2://apd check
		ApdBtnF3_Down(NULL, 0, NULL, 0);
		ApdBtnF3_Up(NULL, 0, NULL, 0);
		break;
	case 3://vdd check
		ApdBtnF4_Down(NULL, 0, NULL, 0);
		ApdBtnF4_Up(NULL, 0, NULL, 0);
		break;
	case 4://signal check
		ApdBtnF5_Down(NULL, 0, NULL, 0);
		ApdBtnF5_Up(NULL, 0, NULL, 0);
		break;
	case 5://delete fct
		ApdBtnF6_Down(NULL, 0, NULL, 0);
		ApdBtnF6_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}