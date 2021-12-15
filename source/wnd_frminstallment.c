/**************************************************************
* Copyright(c)2017,宇佳软甲技术有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frminstallment.h
* 摘    要：  本机序列号输入界面
*
* 当前版本：  v0.0.1
* 作    者：  wbq
* 完成日期：  2017-9
*
* 取代版本：
* 原 作 者：
* 完成日期：
**************************************************************/
#include "wnd_frminstallment.h"

/**************************************************************
* 	为实现窗体wnd_FrmAbout而需要引用的其他头文件
**************************************************************/
#include "wnd_frmabout.h"
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmstandbyset.h"
#include "app_parameter.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmwifi.h"
#include "app_frmupdate.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "app_systemsettings.h"
#include "app_installment.h"
#include "wnd_frmdialog.h"

#define BUFFLEN     25
#define INPUTLEN    8
#define BTNLEN      12
#define BLUE        0x002048


/**************************************************************
* 		定义wnd_FrmSerialNumber中内部使用的数据结构
**************************************************************/

extern PSerialNum pCurSN;

static char DataBuff[BUFFLEN];
static int Index = 0;
static int ReturnBtnEnable = 1;
CALLLBACKWINDOW Okfunc;

//自定义的全局变量

/***************************************************************
* 			窗体wnd_FrmSerialNumber中的窗体控件
****************************************************************/

static GUIWINDOW *pFrmSerialNumber =                NULL;

/*********************桌面背景定义************************/
static GUIPICTURE *pFrmSerialNumberBG =             NULL;
static GUIPICTURE *pFrmSerialNumberReturn =         NULL;
static GUIPICTURE *pFrmSerialNumberBtn[BTNLEN];

/**********************桌面控件定义***********************/
//桌面控件
//菜单栏控件
static WNDMENU1 *pStandbysetMenu = NULL;
/************************桌面标签定义*********************/
//桌面标签
static GUILABEL *pSerialNumberLblLabel =            NULL;
static GUILABEL *pSerialNumberDataLblLabel =        NULL;
static GUILABEL *pSerialNumberLblInstallmentInfo =  NULL;
static GUILABEL *pSerialNumberNoteLblLabel =        NULL;
static GUILABEL *pSerialNumberInputLblLabel[INPUTLEN];

//桌面标签 
static GUICHAR  *pSerialNumberStrLabel =            NULL; 
static GUICHAR  *pSerialNumberDataStrLabel =        NULL; 
static GUICHAR  *pSerialNumberStrInstallmentInfo =  NULL;
static GUICHAR  *pSerialNumberNoteStrLabel =        NULL; 
static GUICHAR  *pSerialNumberStrInstInfo2 =        NULL;
static GUICHAR  *pSerialNumberStrInstInfo3 =        NULL;
static GUICHAR  *pSerialNumberStrInstInfo4 =        NULL;
static GUICHAR  *pSerialNumberInputStrLabel[INPUTLEN];

/*******************************************************************************
* 						文本资源初始化及销毁函数声明
*******************************************************************************/
//初始化文本资源
static int SerialNumberTextRes_Init(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);

//释放文本资源
static int SerialNumberTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);

/*******************************************************************************
* 								控件函数声明
*******************************************************************************/
static int SerialNumberBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SerialNumberBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//返回按键点击函数
static int SerialNumberReturnBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SerialNumberReturnBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static void FlushLabel(); 
static void SerialNumberValueCal(int iOutLen);

//提示窗口回调函数
static void SerialNumberReCreateWindow(GUIWINDOW **pWnd);

//解密判断函数
static void DecryptionJudgment();

//获得分期付款信息，当前第几期，剩余多少天
static int GetInstInfo(char *periodBuf, char *daysBuf);

/***
  * 功能：
                窗体FrmSerialNumber 的初始化函数，建立窗体控件、
                注册消息处理
  * 参数：
             1.void *pWndObj:    指向当前窗体对象
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
int FrmSerialNumberInit(void *pWndObj)
{
    int i;
    int iReturn = 0;

    //初始化
    memset(DataBuff,'\0',sizeof(DataBuff));
    Index = 0;
    SerialNumberTextRes_Init(NULL, 0, NULL, 0);
    
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmSerialNumber = (GUIWINDOW *) pWndObj;

    //建立桌面上的控件
    pFrmSerialNumberBG = CreatePhoto("bg_serialNumber");

    pFrmSerialNumberReturn = CreatePhoto("btn_serialNumber_return");
    CreateLoopPhoto1(pFrmSerialNumberBtn,"btn_serialNumber_unpress", 0, 0, 12);
    
    /******************建立按钮区的标签******************/
    pSerialNumberLblLabel = CreateLabel(142, 165, 251,24, pSerialNumberStrLabel);
    SetLabelAlign(GUILABEL_ALIGN_RIGHT, pSerialNumberLblLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSerialNumberLblLabel);

    pSerialNumberDataLblLabel = CreateLabel(395, 165, 251,24, pSerialNumberDataStrLabel);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pSerialNumberDataLblLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSerialNumberDataLblLabel);  

    pSerialNumberLblInstallmentInfo = CreateLabel(165, 195, 250,24, pSerialNumberStrInstallmentInfo);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pSerialNumberLblInstallmentInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSerialNumberLblInstallmentInfo); 

    pSerialNumberNoteLblLabel = CreateLabel(165, 225, 300,24, pSerialNumberNoteStrLabel);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pSerialNumberNoteLblLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSerialNumberNoteLblLabel);  

    for(i = 0;i < INPUTLEN; i++)
    {
        pSerialNumberInputLblLabel[i] = CreateLabel(165+59*i,270,47,24,pSerialNumberInputStrLabel[i]);
        SetLabelAlign(GUILABEL_ALIGN_CENTER, pSerialNumberInputLblLabel[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_WHITE),pSerialNumberInputLblLabel[i]);  
    }

    //注册窗体(因为所有的按键事件都统一由窗体进
    //行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSerialNumber,pFrmSerialNumber);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmSerialNumberReturn, pFrmSerialNumber);

	//注册桌面右侧五个系统功能选项的消息处理
	//右侧菜单栏控件
	LoginWndMenuToMsg1(pStandbysetMenu, pFrmSerialNumber);
    
    for(i=0;i<BTNLEN;i++)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmSerialNumberBtn[i], pFrmSerialNumber);
    }

    //注册消息处理函数，如果消息接收对象未曾注册
    //到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //*********************************************************/
    pMsg = GetCurrMessage();
    
    //注册窗体的按键消息处理
    if(ReturnBtnEnable){
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmSerialNumberReturn,
                            SerialNumberReturnBtn_Down, NULL, 0, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmSerialNumberReturn,
                            SerialNumberReturnBtn_Up, NULL, 0, pMsg);
    }
    for(i = 0;i < BTNLEN; i++)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmSerialNumberBtn[i],
                        SerialNumberBtn_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmSerialNumberBtn[i],
                        SerialNumberBtn_Up, NULL, i, pMsg);
    }
    return iReturn;
}
/***
  * 功能：
                窗体FrmSerialNumber 的退出函数，释放所有资源
  * 参数：
                1.void *pWndObj:    指向当前窗体对象
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
int FrmSerialNumberExit(void *pWndObj)
{
    int i;
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmSerialNumber = (GUIWINDOW *) pWndObj;

    //清除注册消息
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //*********************************************************/
    ClearWindowComp(pFrmSerialNumber);

    //销毁窗体控件
    //*********************************************************/
    //销毁状态栏、桌面、信息栏控件
    DestroyPicture(&pFrmSerialNumberBG);
    DestroyPicture(&pFrmSerialNumberReturn);
    for(i=0;i<BTNLEN;i++)
    {
        DestroyPicture(&pFrmSerialNumberBtn[i]);
    }

    //桌面标签销毁
    DestroyLabel(&pSerialNumberLblLabel);
    DestroyLabel(&pSerialNumberDataLblLabel);
    DestroyLabel(&pSerialNumberLblInstallmentInfo);
    DestroyLabel(&pSerialNumberNoteLblLabel);
    for(i=0;i<INPUTLEN;i++)
    {
       DestroyLabel(&pSerialNumberInputLblLabel[i]); 
    }
    
    //文本内容销毁
    SerialNumberTextRes_Exit(NULL, 0, NULL, 0);

    return iReturn;
}


/***
  * 功能：
                窗体FrmSerialNumber 的绘制函数，绘制整个窗体
  * 参数：
             1.void *pWndObj:    指向当前窗体对象
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
int FrmSerialNumberPaint(void *pWndObj)
{
    int i;
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmSerialNumber = (GUIWINDOW *) pWndObj;
    //显示状态栏、桌面、信息栏控件
    DisplayPicture(pFrmSerialNumberBG);
    if(ReturnBtnEnable)
    {
        DisplayPicture(pFrmSerialNumberReturn);
    }
    DisplayLabel(pSerialNumberLblLabel);
    DisplayLabel(pSerialNumberDataLblLabel);
    DisplayLabel(pSerialNumberLblInstallmentInfo);
    DisplayLabel(pSerialNumberNoteLblLabel);
    for(i = 0;i < INPUTLEN; i++)
    {
       DisplayLabel(pSerialNumberInputLblLabel[i]); 
    }
    for(i = 0;i < BTNLEN; i++)
    {
        DisplayPicture(pFrmSerialNumberBtn[i]);
    }
    SetPowerEnable(2,1);
    RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}


/***
  * 功能：
            窗体FrmSerialNumber 的循环函数，进行窗体循环
  * 参数：
         1.void *pWndObj:    指向当前窗体对象
  * 返回：
            成功返回零，失败返回非零值
  * 备注：
***/
int FrmSerialNumberLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
   
    return iReturn;
}


/***
  * 功能：
        窗体FrmSerialNumber 的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSerialNumberPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
            窗体FrmSerialNumber 的恢复函数，进行窗体恢复前预处理
  * 参数：
            1.void *pWndObj:    指向当前窗体对象
  * 返回：
            成功返回零，失败返回非零值
  * 备注：
***/
int FrmSerialNumberResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
             初始化文本资源
  * 参数：
                无
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
static int SerialNumberTextRes_Init(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    int i;
    //错误标志、返回值定义
    int iReturn = 0;
    
    pSerialNumberStrLabel = TransString("INSTALLMENT_SERIAL_NO");
    
    char SN[20] = {0};
	GenerateSerialNumber(SN, pCurSN);
    pSerialNumberDataStrLabel = TransString(SN);

	//获得申请分期付款信息
	char periodBuf[16] = {0};
	char daysBuf[16] = {0};
	int isEncrypted = GetInstInfo(periodBuf, daysBuf);
	
	if(isEncrypted)
	{
	    pSerialNumberStrInstallmentInfo = TransString("INSTALLMENT_SERIAL_COUNT");
    	pSerialNumberStrInstInfo2 = TransString(periodBuf);
    	pSerialNumberStrInstInfo3 = TransString("INSTALLMENT_SERIAL_DAYS");
    	pSerialNumberStrInstInfo4 = TransString(daysBuf);
		StringCat(&pSerialNumberStrInstallmentInfo, pSerialNumberStrInstInfo2);
		StringCat(&pSerialNumberStrInstallmentInfo, pSerialNumberStrInstInfo3);
		StringCat(&pSerialNumberStrInstallmentInfo, pSerialNumberStrInstInfo4);
	}
	else
	{
		pSerialNumberStrInstallmentInfo = TransString("");
	}
    
    pSerialNumberNoteStrLabel = TransString("INSTALLMENT_SERIAL_OPTION");
    for(i=0;i<INPUTLEN;i++)
    {
        pSerialNumberInputStrLabel[i] = TransString("");
    }

    return iReturn;
}

/***
  * 功能：
             释放文本资源
  * 参数：
                无
  * 返回：
                成功返回零，失败返回非零值
  * 备注：
***/
static int SerialNumberTextRes_Exit(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    int i;
    //错误标志、返回值定义
    int iReturn = 0;
    //释放状态栏上的文本
    //...
    //释放按钮区的文本
    //...
    GuiMemFree(pSerialNumberStrLabel);
    GuiMemFree(pSerialNumberDataStrLabel);
    GuiMemFree(pSerialNumberNoteStrLabel);
    GuiMemFree(pSerialNumberStrInstallmentInfo);
    GuiMemFree(pSerialNumberStrInstInfo2);
    GuiMemFree(pSerialNumberStrInstInfo3);
    GuiMemFree(pSerialNumberStrInstInfo4);
    
    for(i=0;i<INPUTLEN;i++)
    {
        GuiMemFree(pSerialNumberInputStrLabel[i]);
    }
    
    return iReturn;
}

//按键点击函数
static int SerialNumberBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    char buff[128];
    
    SerialNumberValueCal(iOutLen);
    sprintf(buff, "%s%d%s","btn_serialNumber_press", iOutLen, ".bmp");
    
    TouchChange(buff, pFrmSerialNumberBtn[iOutLen],
            NULL, NULL, 0);
    
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}
static int SerialNumberBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    char buff[128];
    
    sprintf(buff, "%s%d%s","btn_serialNumber_unpress", iOutLen, ".bmp");
    TouchChange(buff, pFrmSerialNumberBtn[iOutLen],
            NULL, NULL, 0);
    
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

//返回按键点击函数
static int SerialNumberReturnBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    TouchChange("btn_serialNumber_return.bmp", pFrmSerialNumberReturn,
            NULL, NULL, 0);
    
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}
static int SerialNumberReturnBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    TouchChange("btn_serialNumber_return.bmp", pFrmSerialNumberReturn,
            NULL, NULL, 0);
    
    GUIWINDOW *pWnd = CreateAboutWindow();
    SendWndMsg_WindowExit(pFrmSerialNumber); //发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
    
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

static void EraserLabel(const char* text, int index)
{
    GUICHAR* gtext = TransString((char*)text);
    SetLabelText(gtext, pSerialNumberInputLblLabel[index]);

    GUIPEN *pPen = GetCurrPen();
    unsigned int uiColorBack = pPen->uiPenColor;
    pPen->uiPenColor = BLUE;
    DrawBlock(pSerialNumberInputLblLabel[index]->Visible.Area.Start.x+2,
              pSerialNumberInputLblLabel[index]->Visible.Area.Start.y+2,
              pSerialNumberInputLblLabel[index]->Visible.Area.End.x-2,
              pSerialNumberInputLblLabel[index]->Visible.Area.End.y-2);
    pPen->uiPenColor = uiColorBack;
    
    DisplayLabel(pSerialNumberInputLblLabel[index]);
    
    GuiMemFree(gtext);
}

static void FlushLabel()
{
    int i;
    
    for (i = 0; i < INPUTLEN; ++i)
    {
        char buff[4];
        buff[0] = DataBuff[i*3 + 0];
        buff[1] = DataBuff[i*3 + 1];
        buff[2] = DataBuff[i*3 + 2];
        buff[3] = '\0';
        
        EraserLabel(buff, i);
    }
}

//获得分期付款信息，当前第几期，剩余多少天
//如果有分期付款信息，返回1，没有则返回0
static int GetInstInfo(char *periodBuf, char *daysBuf)
{
	int iRet = 0;
	int total = 0;
	int period = 0;
	int remainDays = 0;
	
	INSTALLMENT inst;
	GetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET);
    
	if (Encrypted == InstEncryptedFlag(&inst))
	{
		total = InstTotalPeriods(&inst);
		period = InstCurrentPeriod(&inst);
		remainDays = InstRemainDays(&inst, 1, 0);
		if (0 == period)
		{
			sprintf(periodBuf, " X-X , ");
			sprintf(daysBuf, " XX");
		}
		else
		{
			sprintf(periodBuf, " %d-%d , ", total, period);
			sprintf(daysBuf, " %d", remainDays);
		}
		iRet = 1;
	}
    
	return iRet;
}

static void SerialNumberValueCal(int iOutLen)
{
    if(iOutLen == 10)
    {
       if(Index > 0)
       {
            DataBuff[--Index] = '\0';
       }  
       FlushLabel();   
    }
    else if(iOutLen == 11)
    {
        if(Index == 24)
        {
            DecryptionJudgment();
        }
        else
        {
            DialogInit(120, 90, TransString("Warning"),
                        TransString("INTSTALLMENT_ERROR"),
                        0, 0, SerialNumberReCreateWindow, NULL, NULL);
        }
    }
    else
    {
        if(Index < 24)
        {
            DataBuff[Index++] = iOutLen + '0'; 
        }
        FlushLabel();
    }
}

//提示窗口回调函数
static void SerialNumberReCreateWindow(GUIWINDOW **pWnd)
{
     *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                               FrmSerialNumberInit,  FrmSerialNumberExit,
                               FrmSerialNumberPaint, FrmSerialNumberLoop,
                               FrmSerialNumberPause, FrmSerialNumberResume,
                               NULL);          //pWnd由调度线程释放
}

//解密判断函数
static void DecryptionJudgment()
{
    int iRet = -1; 
    INSTALLMENT inst; 
    
    InstMemset(&inst); 
    
    //读eeprom 
    GetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET); 
    iRet = InstVerifyLicense(&inst, DataBuff); 
	InstPrint(&inst);
    if(!InstIsOutOfDate(&inst, 1, 0) && iRet != 1)
    {
        DialogInit(120, 90, TransString("Warning"),
		TransString("INTSTALLMENT_WARNNING"),
		0, 0, SerialNumberReCreateWindow, NULL, NULL);

        return;
    }
    
    if (0 == iRet) 
    { 
        //todo 授权成功 
        int period = InstCurrentPeriod(&inst) + 1; 
        InstSetCurrentPeriod(&inst, period); 
        InstSetUtcTime(&inst, 1, 0); 
        
        //显示一下授权成功，跳出界面
        DialogInit(120, 90, TransString("DIALOG_NOTE"),
        TransString("INTSTALLMENT_AUTH_SUCC"),
        0, 0, Okfunc, NULL, NULL);
    } 
    else if(1 == iRet) 
    { 
        //todo 解密成功 
        InstSetEncryptedFlag(&inst, NoEncryption); 
        InstSetTotalPeriod(&inst, 0); 
        InstSetUtcTime(&inst, 0, 0);
        
        //显示一下解密成功，跳出界面 
        DialogInit(120, 90, TransString("DIALOG_NOTE"),
        TransString("INTSTALLMENT_PAID_ALL"),
        0, 0, Okfunc, NULL, NULL);
    } 
    else 
    { 
    	//todo 弹窗显示失败
		DialogInit(120, 90, TransString("Warning"),
		TransString("INTSTALLMENT_ERROR"),
		0, 0, SerialNumberReCreateWindow, NULL, NULL);
    }
    
    SetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET); 
    SaveSettings(INSTALLMENT_SET);
	InstPrint(&inst);
}

/***
  * 功能：
		建立对话框
  * 参数：
		1.返回按钮的使能标记 : 	对话框左上角的横坐标
		2.okfunc	         : 	按下OK按钮后执行的回调函数
  * 返回：void
  * 备注：
  		如果没有需要在按下ok按钮后需要处理的回调函数，可输入NULL
***/
void serialNumberDialogInit(int returnBtnEnable,CALLLBACKWINDOW func)
{
    ReturnBtnEnable = returnBtnEnable;
    GUIWINDOW *pWnd = NULL;
	Okfunc = func;
    
    pWnd = CreateSerialNumberWindow();         	//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());		//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);				//发送消息以便调用新的窗体
}


//窗体构造函数
GUIWINDOW* CreateSerialNumberWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmSerialNumberInit,  FrmSerialNumberExit,
                                   FrmSerialNumberPaint, FrmSerialNumberLoop,
                                   FrmSerialNumberPause, FrmSerialNumberResume,
                                   NULL);          //pWnd由调度线程释放
    return pWnd;
}

