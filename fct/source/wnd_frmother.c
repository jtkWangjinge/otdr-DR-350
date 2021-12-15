/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmother.c
* 摘    要：  实现窗体frmother的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
*******************************************************************************/

#include "wnd_frmother.h"
#include "wnd_global.h"
#include "wnd_frmmainopt.h"

#include "common_opm.h"
#include "drvoptic.h"
#include "opm.h"


//电源
typedef struct _power
{
	unsigned char ucStateDc:1;	//占1个byte，电源适配器插入状态，1表示适配器插入，0表示未插入。（只读）;
    unsigned char ucStateBat:1;	//占1个byte，电池连接状态，1表示已连接，0表示未连接。只有当适配插入时才有效。（只读）;
    unsigned char ucStateFull:1;//占1个byte，充电状态，1表示充满电，0表示正在充电。只有当适配插入时才有效。（只读）;
	unsigned char ucStateType:2;//占2个byte，01代表933传ad值给9g45, 00代表933传%给9g45
	unsigned char :2;			//保留了2个byte;
    unsigned char ucStatePd:1;	//占1个byte，关机，1关闭主板电源，0无动作。（可写）;
    
    unsigned char ucPowerLevel;	//电源电量;
}PWRINFO;


const static U16 AttValue[OPM_AMP_KIND]={0x000,0x004,0x005,0x006,0x007};
extern POPM_TUNING pOpmTunning;

static int On = 0;
//按钮区的字体
static GUIFONT *pAttFntWrong = NULL;     //标题按钮的字体
static GUIFONT *pAttFntRight = NULL;     //标题按钮的字体

/************************
* 窗体frmmain中的文本资源
************************/
//状态栏上的文本
static GUICHAR *pAttStrTitle = NULL;
static GUILABEL *pAttLblTitle = NULL;

//桌面上的文本
static GUICHAR *pAttStrF1 = NULL;    //窗体图标1的文本
static GUICHAR *pAttStrF2 = NULL;    //窗体图标2的文本
static GUICHAR *pAttStrF3 = NULL;    //窗体图标3的文本
static GUICHAR *pAttStrF4 = NULL;    //窗体图标4的文本
static GUICHAR *pAttStrF5 = NULL;    //窗体图标5的文本
static GUICHAR *pAttStrF6 = NULL;    //窗体图标6的文本
static GUICHAR *pAttStrF7 = NULL;    //窗体图标7的文本

static GUICHAR *pAttStrDataLine[15];
static GUILABEL *pAttLblDataLine[15];

//信息栏上文本
static GUICHAR *pAttStrInfo = NULL;    //提示信息的文本


/************************
* 窗体frmmain中的窗体控件
************************/
static GUIWINDOW *pFrmAtt = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pAttBarStatus = NULL;
static GUIPICTURE *pAttBgDesk = NULL;
static GUIPICTURE *pAttBarInfo = NULL;

//状态栏上的控件

//桌面上的控件
static GUIPICTURE *pAttBtnF1 = NULL;     //窗体图标1图形框
static GUIPICTURE *pAttBtnF2 = NULL;     //窗体图标2图形框
static GUIPICTURE *pAttBtnF3 = NULL;     //窗体图标3图形框
static GUIPICTURE *pAttBtnF4 = NULL;     //窗体图标4图形框
static GUIPICTURE *pAttBtnF5 = NULL;     //窗体图标5图形框
static GUIPICTURE *pAttBtnF6 = NULL;     //窗体图标6图形框
static GUIPICTURE *pAttBtnF7 = NULL;     //窗体图标7图形框

static GUILABEL *pAttLblF1 = NULL;       //窗体图标1标签
static GUILABEL *pAttLblF2 = NULL;       //窗体图标2标签
static GUILABEL *pAttLblF3 = NULL;       //窗体图标3标签
static GUILABEL *pAttLblF4 = NULL;       //窗体图标4标签
static GUILABEL *pAttLblF5 = NULL;       //窗体图标5标签
static GUILABEL *pAttLblF6 = NULL;       //窗体图标6标签
static GUILABEL *pAttLblF7 = NULL;       //窗体图标7标签

//数据显示区
static GUIPICTURE *pAttPicDataDispaly = NULL;
static GUILABEL *pAttLblDataDispaly = NULL;
static GUICHAR *pAttStrDataDispaly = NULL;    

//信息栏上的控件
static GUILABEL *pAttLblInfo = NULL;       //提示信息标签
static GUIPICTURE *pAttBtnBack = NULL;

/********************************
* 窗体frmmain中的文本资源处理函数
********************************/
//初始化文本资源
static int AttTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int AttTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);


/********************************
* 窗体frmmain中的控件事件处理函数
********************************/
//窗体的按键事件处理函数
static int AttWndKey_Down(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
static int AttWndKey_Up(void *pInArg, int iInLen, 
                         void *pOutArg, int iOutLen);

//桌面上控件的事件处理函数
static int AttBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int AttBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int AttBtnF3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int AttBtnF4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int AttBtnF5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int AttBtnF6_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int AttBtnF7_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int AttBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int AttBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int AttBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int AttBtnF3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int AttBtnF4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int AttBtnF5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int AttBtnF6_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int AttBtnF7_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int AttBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

/********************************
* 窗体frmmain中的错误事件处理函数
********************************/
static int AttErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int GetTemp();
static int EepromCheck();
static int SpeakerCheck();
static int VLSCheck(int on);
static int PowerInfo();
static PWRINFO* Pwr_GetInfo(int iPwrFd);
static int DDRCheck();
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
int FrmAttInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	int iLineGap = 16;
	int iLineY = 0;
	int iLineWidth = 150;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

	On = 0;
	VLSCheck(0);
	
    //得到当前窗体对象
    pFrmAtt = (GUIWINDOW *) pWndObj;

    //初始化字体资源
    pAttFntWrong = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);
    pAttFntRight = CreateFont(FNTFILE_STD, 16, 16, 0x00FFFFFF, 0xFFFFFFFF);

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    AttTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
	pAttBarStatus = CreatePicture(0, 0, WINDOW_WIDTH, 30, BmpFileDirectory"bar_status.bmp");
    pAttBgDesk = CreatePicture(0, 30, WINDOW_WIDTH, 420, BmpFileDirectory "bg_desk.bmp");
    pAttBarInfo = CreatePicture(0, 450, WINDOW_WIDTH, 30, BmpFileDirectory "bar_info.bmp");
    pAttLblTitle = CreateLabel(0, 5, WINDOW_WIDTH, 30, pAttStrTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblTitle);

    //建立桌面上的控件
    pAttBtnF1 = CreatePicture(424, 35, 80, 53,
                                 BmpFileDirectory"btn_enable.bmp");
    pAttBtnF2 = CreatePicture(424, 35, 80, 53,
                              BmpFileDirectory "btn_enable.bmp");
    pAttBtnF3 = CreatePicture(424, 95, 80, 53,
                              BmpFileDirectory "btn_enable.bmp");
    pAttBtnF4 = CreatePicture(424, 155, 80, 53,
                              BmpFileDirectory "btn_enable.bmp");
    pAttBtnF5 = CreatePicture(424, 215, 80, 53,
                              BmpFileDirectory "btn_enable.bmp");
    pAttBtnF6 = CreatePicture(424, 275, 80, 53,
                              BmpFileDirectory "btn_enable.bmp");
    pAttBtnF7 = CreatePicture(424, 335, 80, 53,
                              BmpFileDirectory "btn_enable.bmp");

    pAttLblF1 = CreateLabel(429, 44, 80, 53, pAttStrF1); //ATT
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblF1);
    pAttLblF2 = CreateLabel(429, 44, 80, 53, pAttStrF2); //EEPROM
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblF2);
    pAttLblF3 = CreateLabel(429, 104, 80, 53, pAttStrF3); //temperature
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblF3);
    pAttLblF4 = CreateLabel(429, 164, 80, 53, pAttStrF4); //vfl
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblF4);
    pAttLblF5 = CreateLabel(429, 224, 80, 53, pAttStrF5); //speaker
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblF5);
    pAttLblF6 = CreateLabel(429, 284, 80, 53, pAttStrF6); //charger
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblF6);
    pAttLblF7 = CreateLabel(429, 344, 80, 53, pAttStrF7); //DDR check
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblF7);

	SetPictureEnable(0, pAttBtnF1);
	SetLabelEnable(0, pAttLblF1);

	pAttPicDataDispaly = CreatePicture(5, 30, 320, 400,
                                 BmpFileDirectory"frm_dataInfo.bmp");
	pAttLblDataDispaly = CreateLabel(5, 176, 320, 16, pAttStrDataDispaly);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pAttLblDataDispaly);
    //数据框中的数据
    for(i = 0; i < 15; i++)
   	{
   		iLineY = 45 + iLineGap * i;
		pAttLblDataLine[i] = CreateLabel(20, iLineY, iLineWidth, iLineGap, pAttStrDataLine[i]);
	}

	pAttBtnBack = CreatePicture(520, 454, 60, 25,
                                     BmpFileDirectory"btn_back_unpress.bmp");

    //建立信息栏上的控件
    pAttLblInfo = CreateLabel(20, 458, 300, 16, pAttStrInfo);


    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //获得控件队列的互斥锁
    MutexLock(&(pFrmAtt->Mutex));      //注意，必须获得锁
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmAtt, 
                  pFrmAtt);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnF1, 
                  pFrmAtt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnF2, 
                  pFrmAtt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnF3, 
                  pFrmAtt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnF4, 
                  pFrmAtt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnF5, 
                  pFrmAtt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnF6, 
                  pFrmAtt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnF7, 
                  pFrmAtt);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pAttBtnBack, 
                  pFrmAtt);

    //释放控件队列的互斥锁
    MutexUnlock(&(pFrmAtt->Mutex));    //注意，必须释放锁

    //***************************************************************/
    pMsg = GetCurrMessage();
    //获得消息队列的互斥锁
    MutexLock(&(pMsg->Mutex));      //注意，必须获得锁
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmAtt, 
                    AttWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmAtt, 
                    AttWndKey_Up, NULL, 0, pMsg);
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnF1, 
                    AttBtnF1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnF2, 
                    AttBtnF2_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnF3, 
                    AttBtnF3_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnF4, 
                    AttBtnF4_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnF5, 
                    AttBtnF5_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnF6, 
                    AttBtnF6_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnF7, 
                    AttBtnF7_Down, NULL, 0, pMsg);
			 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAttBtnBack, 
                    AttBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnF1, 
                    AttBtnF1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnF2, 
                    AttBtnF2_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnF3, 
                    AttBtnF3_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnF4, 
                    AttBtnF4_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnF5, 
                    AttBtnF5_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnF6, 
                    AttBtnF6_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnF7, 
                    AttBtnF7_Up, NULL, 0, pMsg);

  
    LoginMessageReg(GUIMESSAGE_TCH_UP, pAttBtnBack, 
                    AttBtnBack_Up, NULL, 0, pMsg);

    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmAtt, 
                    AttErrProc_Func, NULL, 0, pMsg);

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
int FrmAttExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmAtt = (GUIWINDOW *) pWndObj;
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    MutexLock(&(pMsg->Mutex));          //注意，必须获得锁
    ClearMessageReg(pMsg);
    MutexUnlock(&(pMsg->Mutex));        //注意，必须释放锁

    //从当前窗体中注销窗体控件
    //***************************************************************/
    MutexLock(&(pFrmAtt->Mutex));      //注意，必须获得锁
    ClearWindowComp(pFrmAtt);
    MutexUnlock(&(pFrmAtt->Mutex));    //注意，必须释放锁

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pAttBarStatus);
    DestroyPicture(&pAttBgDesk);
    DestroyPicture(&pAttBarInfo);
    DestroyLabel(&pAttLblTitle);
    //销毁状态栏上的控件
    //销毁桌面上的控件
    DestroyPicture(&pAttBtnF1);
    DestroyPicture(&pAttBtnF2);
    DestroyPicture(&pAttBtnF3);
    DestroyPicture(&pAttBtnF4);
    DestroyPicture(&pAttBtnF5);
    DestroyPicture(&pAttBtnF6);
    DestroyPicture(&pAttBtnF7);

    DestroyLabel(&pAttLblF1);
    DestroyLabel(&pAttLblF2);
    DestroyLabel(&pAttLblF3);
    DestroyLabel(&pAttLblF4);
    DestroyLabel(&pAttLblF5);
    DestroyLabel(&pAttLblF6);
    DestroyLabel(&pAttLblF7);

    for(i = 0; i < 15; i++)
   	{
		DestroyLabel(&pAttLblDataLine[i]);
	}

    DestroyPicture(&pAttPicDataDispaly);
    DestroyLabel(&pAttLblDataDispaly);
    //销毁信息栏上的控件
    DestroyLabel(&pAttLblInfo);
	
    DestroyPicture(&pAttBtnBack);

    //释放文本资源
    //***************************************************************/
    AttTextRes_Exit(NULL, 0, NULL, 0);

	On = 0;
	VLSCheck(0);
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
int FrmAttPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

    //得到当前窗体对象
    pFrmAtt = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pAttBarStatus);
    DisplayPicture(pAttBgDesk);
    DisplayPicture(pAttBarInfo);
    DisplayLabel(pAttLblTitle);
    //显示状态栏上的控件
    
    //显示桌面上的控件
    DisplayPicture(pAttBtnF1);
    DisplayPicture(pAttBtnF2);
    DisplayPicture(pAttBtnF3);
    DisplayPicture(pAttBtnF4);
    DisplayPicture(pAttBtnF5);
    DisplayPicture(pAttBtnF6);
    DisplayPicture(pAttBtnF7);

    DisplayLabel(pAttLblF1);
    DisplayLabel(pAttLblF2);
    DisplayLabel(pAttLblF3);
    DisplayLabel(pAttLblF4);
    DisplayLabel(pAttLblF5);
    DisplayLabel(pAttLblF6);
    DisplayLabel(pAttLblF7);

    for(i = 0; i < 15; i++)
   	{
		DisplayLabel(pAttLblDataLine[i]);
	}
    //显示按钮区的控件
    //...
    DisplayPicture(pAttPicDataDispaly);
    DisplayLabel(pAttLblDataDispaly);
    //显示信息栏上的控件
    DisplayLabel(pAttLblInfo);
    DisplayPicture(pAttBtnBack);

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
int FrmAttLoop(void *pWndObj)
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
int FrmAttPause(void *pWndObj)
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
int FrmAttResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int AttTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //初始化状态栏上的文本
    pAttStrTitle = TransString("**ATT FCT**");
    //初始化桌面上的文本
    pAttStrF1 = TransString("ATT");
    pAttStrF2 = TransString("EEPROM");
    pAttStrF3 = TransString("Temperature");
    pAttStrF4 = TransString("VFL");
    pAttStrF5 = TransString("Speaker");
    pAttStrF6 = TransString("Charger");
    pAttStrF7 = TransString("DDR Check");

	for(i = 0; i < 15; i++)
   	{
		pAttStrDataLine[i] = TransString("");
	}
    //初始化信息栏上的文本
    pAttStrInfo = TransString("");

    pAttStrDataDispaly = TransString("");

    return iReturn;
}


//释放文本资源
static int AttTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //释放状态栏上的文本
    free(pAttStrTitle);
    //释放桌面上的文本
    free(pAttStrF1);
    free(pAttStrF2);
    free(pAttStrF3);
    free(pAttStrF4);
    free(pAttStrF5);
    free(pAttStrF6);
    free(pAttStrF7);

    for(i = 0; i < 15; i++)
   	{
		free(pAttStrDataLine[i]);
	}

    free(pAttStrDataDispaly);
    //释放信息栏上的文本
    free(pAttStrInfo);

    return iReturn;
}


static int AttWndKey_Down(void *pInArg, int iInLen, 
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
		AttBtnBack_Down(pInArg, iInLen, pOutArg, iOutLen);
        break;
    case KEYCODE_HOME:
        break;
    default:
        break;
    }

    return iReturn;
}


static int AttWndKey_Up(void *pInArg, int iInLen, 
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
        if (iKeyFlag < 5)
        {
            iKeyFlag++;
        }
        break;
    case KEYCODE_enter:
        KeyEnterCallBack(iKeyFlag);
        break;
    case KEYCODE_BACK:
        iKeyFlag = 0;
        AttBtnBack_Up(NULL, 0, NULL, 0);
        break;
    default:
        break;
    }

    return iReturn;
}

static int AttBtnF1_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pAttBtnF1, pAttLblF1);

    return iReturn;
}


static int AttBtnF2_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pAttBtnF2, pAttLblF2);

    return iReturn;
}


static int AttBtnF3_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pAttBtnF3, pAttLblF3);

    return iReturn;
}


static int AttBtnF4_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pAttBtnF4, pAttLblF4);

    return iReturn;
}


static int AttBtnF5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pAttBtnF5, pAttLblF5);

    return iReturn;
}



static int AttBtnF6_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pAttBtnF6, pAttLblF6);

    return iReturn;
}


static int AttBtnF7_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncDown(pAttBtnF7, pAttLblF7);

    return iReturn;
}


static int AttBtnF1_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	
	ShowBtn_FuncUp(pAttBtnF1, pAttLblF1);
	
    return iReturn;
}


static int AttBtnF2_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    ShowBtn_FuncUp(pAttBtnF2, pAttLblF2);

	On = 0;
	VLSCheck(0);
	
	EepromCheck();
	
    return iReturn;
}


static int AttBtnF3_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncUp(pAttBtnF3, pAttLblF3);

	On = 0;
	VLSCheck(0);
	
	GetTemp();
	
    return iReturn;
}


static int AttBtnF4_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	On = !On;
	VLSCheck(On);

    //临时变量定义
    ShowBtn_FuncUp(pAttBtnF4, pAttLblF4);

    return iReturn;
}


static int AttBtnF5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    ShowBtn_FuncUp(pAttBtnF5, pAttLblF5);

	On = 0;
	VLSCheck(0);
	
	SpeakerCheck();
	
    return iReturn;
}


static int AttBtnF6_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncUp(pAttBtnF6, pAttLblF6);

	On = 0;
	VLSCheck(0);
	
	PowerInfo();
	
    return iReturn;
}


static int AttBtnF7_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    ShowBtn_FuncUp(pAttBtnF7, pAttLblF7);

   	On = 0;
	VLSCheck(0);
	
	DDRCheck();
	
    return iReturn;
}

static int GetTemp()
{
	int iReturn = 0;
	
	float fCurTemp = 0.0f;
	char buff[48];
 
	GetOpmTemperature(&fCurTemp);
	printf("fCurTemp = %f\n", fCurTemp);
	sprintf((char *)buff, "Temperature = %f", fCurTemp);
	if(pAttStrDataDispaly != NULL)

	{
		free(pAttStrDataDispaly);
		pAttStrDataDispaly = NULL;
	}
    pAttStrDataDispaly = TransString(buff);
    SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
    SetLabelFont(pAttFntRight, pAttLblDataDispaly);  
    DisplayPicture(pAttPicDataDispaly);
    DisplayLabel(pAttLblDataDispaly);
	
	return iReturn;
}

static int DDRCheck()
{
	UINT32 uiFpgaDdr2Reset = RESET_DDR2;
    write_opt_reg(opticFD, FPGA_DDR_RESET, &uiFpgaDdr2Reset);   

   	int i=0;
	int iRet=1;
	while(i++ < 100)
	{
		 UINT32 uiMode = 0;
    	 read_opt_reg(opticFD, FPGA_DAQMODE_SET, &uiMode);
 		 if(!(uiMode & C1_RST0) && (uiMode & C1_CALIB_DONE))  //检查DDR2是否校准成功
 		 {
 			iRet=0;
 			break;
 		 }
 		 MsecSleep(10);
	}

	if(pAttStrDataDispaly != NULL)

	{
		free(pAttStrDataDispaly);
		pAttStrDataDispaly = NULL;
	}
	
	if(0 == iRet)
	{
        pAttStrDataDispaly = TransString("FPGA DDR OK!!");
		SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
		SetLabelFont(pAttFntRight, pAttLblDataDispaly);   		
	}
	else
	{
        pAttStrDataDispaly = TransString("FPGA DDR ERROR!!");
		SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
		SetLabelFont(pAttFntRight, pAttLblDataDispaly);   
	}
	DisplayPicture(pAttPicDataDispaly);
	DisplayLabel(pAttLblDataDispaly);
	
	return 0;
}


/***
  * 功能：
        得到电源信息
  * 参数：
        1.int iPwrFd:   电源设备文件描述符
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
***/
static PWRINFO* Pwr_GetInfo(int iPwrFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    PWRINFO *pPwrInfo = NULL;
    //临时变量定义

    //参数检查
	if (iErr == 0)
	{
		if (iPwrFd < 0)
		{
			iErr = -1;
		}
	}
	
	//申请资源
    if (iErr == 0)
    {
		pPwrInfo = (PWRINFO *)malloc(sizeof(PWRINFO));
		if (NULL == pPwrInfo)
		{
			iErr = -2;
		}
    }

	//从MCU读取电源信息
    if (iErr == 0)
    {
    	iErr = read(iPwrFd, pPwrInfo, sizeof(PWRINFO));
		if (iErr == sizeof(PWRINFO))
		{
			iErr = 0;
		}
		else
		{
			iErr = -3;
		}
    }

    switch (iErr)
    {
	case -1:
		break;
	case -2:
		break;
	case -3:
		free(pPwrInfo);
		pPwrInfo = NULL;
		break;
	default:
		break;
    }
    
    return pPwrInfo;
}

static int PowerInfo()
{
	int iPowerDevFd = 0;
	PWRINFO *pPwrInfo = NULL;
    //临时变量定义
    unsigned char iPowerLevel;
	char buff[64];

	iPowerDevFd = open("/dev/p89lpc933_0", O_RDWR);
	
	if (iPowerDevFd == -1)
	{
		perror("open /dev/p89lpc933_0:");
		if(pAttStrDataDispaly != NULL)

		{
			free(pAttStrDataDispaly);
			pAttStrDataDispaly = NULL;
		}
		pAttStrDataDispaly = TransString("No PowerDev in F216");
		SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
		SetLabelFont(pAttFntWrong, pAttLblDataDispaly);   
		DisplayPicture(pAttPicDataDispaly);
		DisplayLabel(pAttLblDataDispaly);
	}
	else
	{
		pPwrInfo = Pwr_GetInfo(iPowerDevFd);
		if (NULL == pPwrInfo)
		{
			if(pAttStrDataDispaly != NULL)

			{
				free(pAttStrDataDispaly);
				pAttStrDataDispaly = NULL;
			}
			pAttStrDataDispaly = TransString("No PowerDev in F216");
			SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
			SetLabelFont(pAttFntWrong, pAttLblDataDispaly);   
			DisplayPicture(pAttPicDataDispaly);
			DisplayLabel(pAttLblDataDispaly);
		}
		else
		{
			if (pPwrInfo->ucStateBat == 1)
			{
				iPowerLevel = pPwrInfo->ucPowerLevel;
				if(0 == pPwrInfo->ucStateType)
				{
					if(iPowerLevel >= 90)
					{
						sprintf(buff, "battery level = %d [8.2V-8.6V]", iPowerLevel);
					}
					else if(iPowerLevel >= 80)
					{
						sprintf(buff, "battery level = %d [8.0V-8.2V]", iPowerLevel);
					}
					else if(iPowerLevel >= 70)
					{
						sprintf(buff, "battery level = %d [7.8V-8.0V]", iPowerLevel);
					}
					else if(iPowerLevel >= 60)
					{
						sprintf(buff, "battery level = %d [7.6V-7.8V]", iPowerLevel);
					}
					else if(iPowerLevel >= 50)
					{
						sprintf(buff, "battery level = %d [7.4V-7.6V]", iPowerLevel);
					}
					else if(iPowerLevel >= 40)
					{
						sprintf(buff, "battery level = %d [7.3V-7.4V]", iPowerLevel);
					}
					else if(iPowerLevel >= 30)
					{
						sprintf(buff, "battery level = %d [7.2V-7.3V]", iPowerLevel);
					}
					else if(iPowerLevel >= 20)
					{
						sprintf(buff, "battery level = %d [7.0V-7.2V]", iPowerLevel);
					}
					else if(iPowerLevel >= 10)
					{
						sprintf(buff, "battery level = %d [6.9V-7.0V]", iPowerLevel);
					}
					else
					{
						sprintf(buff, "battery level = %d [6.4V-6.9V]", iPowerLevel);
					}
				}
				else
				{ 
					iPowerLevel += (unsigned char)pPwrInfo->ucStateDc * 30;
				
					if(iPowerLevel <= 42)
					{
						sprintf(buff, "battery level = %d [8.2V-8.6V]", 100);
					}
					else if(iPowerLevel <= 54)
					{
						sprintf(buff, "battery level = %d [8.0V-8.2V]", 80);
					}
					else if(iPowerLevel <= 63)
					{
						sprintf(buff, "battery level = %d [7.8V-8.0V]", 70);
					}
					else if(iPowerLevel <= 72)
					{
						sprintf(buff, "battery level = %d [7.6V-7.8V]", 60);
					}
					else if(iPowerLevel <= 79)
					{
						sprintf(buff, "battery level = %d [7.4V-7.6V]", 50);
					}
					else if(iPowerLevel <= 84)
					{
						sprintf(buff, "battery level = %d [7.3V-7.4V]", 40);
					}
					else if(iPowerLevel <= 90)
					{
						sprintf(buff, "battery level = %d [7.2V-7.3V]", 30);
					}
					else if(iPowerLevel <= 100)
					{
						sprintf(buff, "battery level = %d [7.0V-7.2V]", 20);
					}
					else if(iPowerLevel <= 120)
					{
						sprintf(buff, "battery level = %d [6.9V-7.0V]", 10);
					}
					else
					{
						sprintf(buff, "battery level = %d [6.4V-6.9V]", 0);
					}
				}
				if(pAttStrDataDispaly != NULL)

				{
					free(pAttStrDataDispaly);
					pAttStrDataDispaly = NULL;
				}
				pAttStrDataDispaly = TransString(buff);
				SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
				SetLabelFont(pAttFntRight, pAttLblDataDispaly);   
				DisplayPicture(pAttPicDataDispaly);
				DisplayLabel(pAttLblDataDispaly);
			}
			else
			{	
				if(pAttStrDataDispaly != NULL)

				{
					free(pAttStrDataDispaly);
					pAttStrDataDispaly = NULL;
				}
				pAttStrDataDispaly = TransString("No PowerDev in F216");
				SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
				SetLabelFont(pAttFntWrong, pAttLblDataDispaly);   
				DisplayPicture(pAttPicDataDispaly);
				DisplayLabel(pAttLblDataDispaly);
			}
		}
	}
	close(iPowerDevFd);

	return 0;
}

static int VLSCheck(int on)
{

	U32 Reg = 0;

	DisplayPicture(pAttPicDataDispaly);

	if(on == 1)
	{
		read_opt_reg(opticFD, FPGA_CTRLOUT_REG, &Reg);
		Reg |= VFL_CW;
		write_opt_reg(opticFD,FPGA_CTRLOUT_REG,&Reg);
	}
	else
	{
		read_opt_reg(opticFD, FPGA_CTRLOUT_REG, &Reg);
		Reg &= ~VFL_CW;
		write_opt_reg(opticFD,FPGA_CTRLOUT_REG,&Reg);
	}
	
	return 0;
}


static int SpeakerCheck()
{

	DisplayPicture(pAttPicDataDispaly);


    int iErr = 0;
    //临时变量定义
    unsigned int iSpeaker = 0x2000;//104ms
    
	iErr = write_opt_reg(opticFD, FPGA_SPEAK_TIME, &iSpeaker);

	iSpeaker = SPEAK_CTRL;
    iErr = write_opt_reg(opticFD, FPGA_TRIG_CTRL, &iSpeaker);

	return 0;
}


static int EepromCheck(void)
{
	int iReturn = 0;
	int len = sizeof(OPM_TUNING);

	UINT8 buf[12] = {'a','b','c','d','e','f','g','h','i','j','k','l',};
	UINT8 buf1[12] = {'0','0','0','0','0','0','0','0','0','0','0','0',};

 	lseek(opticFD, len, SEEK_SET);

	write(opticFD, buf, 12);

	lseek(opticFD, len, SEEK_SET);

	read(opticFD, buf1, 12);


	if(0 == memcmp(buf, buf1, 12))
	{
		iReturn = 1;
	}
	else
	{
		iReturn = 0;
	}
	
	if(pAttStrDataDispaly != NULL)

	{
		free(pAttStrDataDispaly);
		pAttStrDataDispaly = NULL;
	}
	
	if(iReturn == 1)
	{
		pAttStrDataDispaly = TransString("Eeprom Check OK!");
		SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
		SetLabelFont(pAttFntRight, pAttLblDataDispaly);   
	}
	else
	{
		pAttStrDataDispaly = TransString("Eeprom Check Error!");
		SetLabelText(pAttStrDataDispaly,pAttLblDataDispaly);
		SetLabelFont(pAttFntWrong, pAttLblDataDispaly);   
	}
	DisplayPicture(pAttPicDataDispaly);
	DisplayLabel(pAttLblDataDispaly);
	
	return iReturn;
}

static int AttBtnBack_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	SetPictureBitmap(BmpFileDirectory"btn_back_press.bmp", pAttBtnBack);
    DisplayPicture(pAttBtnBack);
	

    return iReturn;
}

static int AttBtnBack_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;;
    //临时变量定义


	SetPictureBitmap(BmpFileDirectory"btn_back_unpress.bmp", pAttBtnBack);
    DisplayPicture(pAttBtnBack);
	
    GUIWINDOW *pWnd = NULL; 
	
    pWnd = CreateWindow(0, 0, 320, 240, 
                        FrmMainOptInit, FrmMainOptExit, 
                        FrmMainOptPaint, FrmMainOptLoop, 
                        NULL);          //pWnd由调度线程释放
    SendMsg_ExitWindow(pFrmAtt);       //发送消息以便退出当前窗体
    SendMsg_CallWindow(pWnd);           //发送消息以便调用新的窗体

    return iReturn;
}

static int AttErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}

//enter键响应处理函数
static void KeyEnterCallBack(int iKeyFlag)
{
	switch (iKeyFlag)
	{
	case 0://EEPROM
		AttBtnF2_Down(NULL, 0, NULL, 0);
		AttBtnF2_Up(NULL, 0, NULL, 0);
		break;
	case 1://Temperature
		AttBtnF3_Down(NULL, 0, NULL, 0);
		AttBtnF3_Up(NULL, 0, NULL, 0);
		break;
	case 2://VFL
		AttBtnF4_Down(NULL, 0, NULL, 0);
		AttBtnF4_Up(NULL, 0, NULL, 0);
		break;
	case 3://Speaker
		AttBtnF5_Down(NULL, 0, NULL, 0);
		AttBtnF5_Up(NULL, 0, NULL, 0);
		break;
	case 4://Charger
		AttBtnF6_Down(NULL, 0, NULL, 0);
		AttBtnF6_Up(NULL, 0, NULL, 0);
		break;
	case 5://DDR Check
		AttBtnF7_Down(NULL, 0, NULL, 0);
		AttBtnF7_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}