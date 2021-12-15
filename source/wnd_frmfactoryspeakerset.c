/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactoryspeakerset.c
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  v1.0.0
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#include "wnd_frmfactoryspeakerset.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmfactory.h"
#include "wnd_frmfactorydr.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmdialog.h"
#include "wnd_frmfactorlanguage.h"
#include "wnd_frmmaintancedate.h"
#include "wnd_frmauthorization.h"
#include "app_frmotdr.h"
#include "wnd_frminputpassward.h"
#include "wnd_frmfactoryfirmware.h"
#include "wnd_frmfactorylaserset.h"
#include "app_parameter.h"
#include "wnd_frmime.h"
#include "app_systemsettings.h"

extern PFactoryConfig pFactoryConf;
extern POTDR_TOP_SETTINGS pOtdrTopSettings;
unsigned int speakerFreq = 0;
int imeTag = 0;
/*******************************************************************************
**							声明窗体中相关控件变量							  **
*******************************************************************************/
static GUIWINDOW *pFrmFactorySpeakerSet = NULL;
static GUIPICTURE *pFactorySpeakerSetBg = NULL;
//声明标题标签变量
static GUICHAR* pFactorySpeakerSetStrTitle1 = NULL;
static GUILABEL* pFactorySpeakerSetLblTitle1 = NULL;

static GUICHAR* pFactorySpeakerSetStrTitle2 = NULL;
static GUILABEL* pFactorySpeakerSetLblTitle2 = NULL;

static GUICHAR* pFactorySpeakerSetStrTitle3 = NULL;
static GUILABEL* pFactorySpeakerSetLblTitle3 = NULL;

//蜂鸣器音量调节
static GUICHAR* pFactorySpeakerSetStrSpeakerTitle = NULL;
static GUICHAR* pFactorySpeakerSetStrSpeakerFreq = NULL;
static GUICHAR* pFactorySpeakerSetStrSpeakerKHz = NULL;
static GUICHAR* pFactorySpeakerSetStrSpeakerTest = NULL;
static GUICHAR* pFactorySpeakerSetStrSpeakerEnter = NULL;

static GUILABEL* pFactorySpeakerSetLblSpeakerTitle = NULL;
static GUILABEL* pFactorySpeakerSetLblSpeakerFreq = NULL;
static GUILABEL* pFactorySpeakerSetLblSpeakerKHz = NULL;
static GUILABEL* pFactorySpeakerSetLblSpeakerTest = NULL;
static GUILABEL* pFactorySpeakerSetLblSpeakerEnter = NULL;

static GUIPICTURE* pFactorySpeakerSetBgSpeakerFreq = NULL;
static GUIPICTURE* pFactorySpeakerSetBtnSpeakerTest = NULL;
static GUIPICTURE* pFactorySpeakerSetBtnSpeakerEnter = NULL;

/*******************************************************************************
*               窗体speakerset右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFactorySpeakerSetMenu = NULL;
/*******************************************************************************
*                   窗体speakerset内部函数声明
*******************************************************************************/
//初始化文本资源
static int FactorySpeakerSetTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FactorySpeakerSetTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static void FactorySpeakerSetMenuCallBack(int iOption);
                            
static int FactorySpeakerSetSpeakerFreq_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySpeakerSetSpeakerFreq_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryTestSpeaker_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryTestSpeaker_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySpeakerEnter_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySpeakerEnter_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
                            
static int FactorySpeakerToImportProfile_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySpeakerToImportProfile_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
                            
static int FactorySpeakerToLaserSet_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySpeakerToLaserSet_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void FactorySpeakerSetMenuCallBack(int iOption)
{
    GUIWINDOW *pWnd = NULL;
    
	switch (iOption)
	{
	case 0: 
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAuthorizationInit, FrmAuthorizationExit, 
							FrmAuthorizationPaint, FrmAuthorizationLoop, 
							FrmAuthorizationPause, FrmAuthorizationResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactorySpeakerSet);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
    case 1:        
        break;
    case 2:        
        pWnd = CreateFactoryWindow();
        SendWndMsg_WindowExit(pFrmFactorySpeakerSet);  
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 3:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacDRInit,FrmFacDRExit,
					  FrmFacDRPaint,FrmFacDRLoop,
					  FrmFacDRPause,FrmFacDRResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmFactorySpeakerSet);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 4:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactorySetInit, FrmFactorySetExit, 
                        FrmFactorySetPaint, FrmFactorySetLoop, 
				        FrmFactorySetPause, FrmFactorySetResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactorySpeakerSet);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 5:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMaintanceDateInit, FrmMaintanceDateExit, 
							FrmMaintanceDatePaint, FrmMaintanceDateLoop, 
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactorySpeakerSet);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN:
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmStandbysetInit, FrmStandbysetExit, 
						    FrmStandbysetPaint, FrmStandbysetLoop, 
						    FrmStandbysetPause, FrmStandbysetResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFactorySpeakerSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmvls的初始化函数，建立窗体控件、注册消息处理
int FrmFactorySpeakerSetInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;
    unsigned int StrVlsMenu[] = {	
        1
	};
    //得到当前窗体对象 
    pFrmFactorySpeakerSet = (GUIWINDOW *) pWndObj;
    if(!imeTag)
    {
        speakerFreq = pFactoryConf->speakerFreq;
    }
    imeTag = 0;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FactorySpeakerSetTextRes_Init(NULL, 0, NULL, 0);
    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFactorySpeakerSetBg = CreatePicture(0, 0, 681, 480, BmpFileDirectory"sysset_time_bg5.bmp");
    
    pFactorySpeakerSetLblTitle1 = CreateLabel(0, 24, 100, 24, pFactorySpeakerSetStrTitle1);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactorySpeakerSetLblTitle1);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactorySpeakerSetLblTitle1);
	
    pFactorySpeakerSetLblTitle2 = CreateLabel(100, 24, 100, 24, pFactorySpeakerSetStrTitle2);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactorySpeakerSetLblTitle2);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactorySpeakerSetLblTitle2);
	
    pFactorySpeakerSetLblTitle3 = CreateLabel(200, 24, 100, 24, pFactorySpeakerSetStrTitle3);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactorySpeakerSetLblTitle3);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactorySpeakerSetLblTitle3);

	//蜂鸣器音量调节
    pFactorySpeakerSetLblSpeakerTitle = CreateLabel(200, 100, 300, 24, pFactorySpeakerSetStrSpeakerTitle);
    pFactorySpeakerSetLblSpeakerFreq = CreateLabel(200, 154, 140, 24, pFactorySpeakerSetStrSpeakerFreq);
    pFactorySpeakerSetLblSpeakerKHz = CreateLabel(350, 154, 100, 24, pFactorySpeakerSetStrSpeakerKHz);
    pFactorySpeakerSetLblSpeakerTest = CreateLabel(200, 216, 138, 24, pFactorySpeakerSetStrSpeakerTest);
    pFactorySpeakerSetLblSpeakerEnter = CreateLabel(350, 216, 138, 24, pFactorySpeakerSetStrSpeakerEnter);

    pFactorySpeakerSetBgSpeakerFreq = CreatePicture(200, 150, 140, 25, BmpFileDirectory"btn_mark_input.bmp");
    pFactorySpeakerSetBtnSpeakerTest = CreatePicture(200, 200, 138, 48, BmpFileDirectory"btn_reset_unpress1.bmp");
    pFactorySpeakerSetBtnSpeakerEnter = CreatePicture(350, 200, 138, 48, BmpFileDirectory"btn_reset_unpress1.bmp");

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactorySpeakerSetLblSpeakerTitle);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFactorySpeakerSetLblSpeakerTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactorySpeakerSetLblSpeakerFreq);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactorySpeakerSetLblSpeakerFreq);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactorySpeakerSetLblSpeakerKHz);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFactorySpeakerSetLblSpeakerKHz);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactorySpeakerSetLblSpeakerTest);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactorySpeakerSetLblSpeakerTest);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactorySpeakerSetLblSpeakerEnter);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactorySpeakerSetLblSpeakerEnter);
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFactorySpeakerSetMenu = CreateWndMenu1(factoryRank, sizeof(StrVlsMenu), StrVlsMenu,  0xffff,
								1, 0, 40, FactorySpeakerSetMenuCallBack);

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //必须在持有控件队列互斥锁的情况下操作
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactorySpeakerSet, pFrmFactorySpeakerSet);
    AddWndMenuToComp1(pFactorySpeakerSetMenu, pFrmFactorySpeakerSet);
    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactorySpeakerSetBgSpeakerFreq, pFrmFactorySpeakerSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactorySpeakerSetBtnSpeakerTest, pFrmFactorySpeakerSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactorySpeakerSetBtnSpeakerEnter, pFrmFactorySpeakerSet);
    
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactorySpeakerSetLblTitle1, pFrmFactorySpeakerSet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactorySpeakerSetLblTitle3, pFrmFactorySpeakerSet);

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	LoginWndMenuToMsg1(pFactorySpeakerSetMenu, pFrmFactorySpeakerSet); 
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactorySpeakerSetBgSpeakerFreq, 
                    FactorySpeakerSetSpeakerFreq_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactorySpeakerSetBgSpeakerFreq,
                    FactorySpeakerSetSpeakerFreq_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactorySpeakerSetBtnSpeakerTest, 
                    FactoryTestSpeaker_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactorySpeakerSetBtnSpeakerTest,
                    FactoryTestSpeaker_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactorySpeakerSetBtnSpeakerEnter, 
                    FactorySpeakerEnter_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactorySpeakerSetBtnSpeakerEnter,
                    FactorySpeakerEnter_Up, NULL, 0, pMsg);
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactorySpeakerSetLblTitle1, 
                    FactorySpeakerToImportProfile_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactorySpeakerSetLblTitle1,
                    FactorySpeakerToImportProfile_Up, NULL, 0, pMsg);
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactorySpeakerSetLblTitle3, 
                    FactorySpeakerToLaserSet_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactorySpeakerSetLblTitle3,
                    FactorySpeakerToLaserSet_Up, NULL, 0, pMsg);
    return iRet;
    
}
//窗体frmvls的退出函数，释放所有资源
int FrmFactorySpeakerSetPaint(void *pWndObj)
{
   ///错误标志、返回值定义
    int iRet = 0;
   //得到当前窗体对象
    pFrmFactorySpeakerSet = (GUIWINDOW *) pWndObj;

    DisplayPicture(pFactorySpeakerSetBg);
    DisplayLabel(pFactorySpeakerSetLblTitle1);
    DisplayLabel(pFactorySpeakerSetLblTitle2);
    DisplayLabel(pFactorySpeakerSetLblTitle3);

    //调节蜂鸣器音量
    DisplayPicture(pFactorySpeakerSetBgSpeakerFreq);
    DisplayPicture(pFactorySpeakerSetBtnSpeakerTest);
    DisplayPicture(pFactorySpeakerSetBtnSpeakerEnter);
    
    DisplayLabel(pFactorySpeakerSetLblSpeakerTitle);
    DisplayLabel(pFactorySpeakerSetLblSpeakerFreq);
    DisplayLabel(pFactorySpeakerSetLblSpeakerKHz);
    DisplayLabel(pFactorySpeakerSetLblSpeakerTest);
    DisplayLabel(pFactorySpeakerSetLblSpeakerEnter);
   /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pFactorySpeakerSetMenu);
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}
//窗体frmvls的绘制函数，绘制整个窗体
int FrmFactorySpeakerSetLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
	//SendMsg_DisableLoop(pWndObj);
    return iRet;
}
//窗体frmvls的循环函数，进行窗体循环
int FrmFactorySpeakerSetPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}
//窗体frmvls的挂起函数，进行窗体挂起前预处理
int FrmFactorySpeakerSetResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}
//窗体frmvls的恢复函数，进行窗体恢复前预处理
int FrmFactorySpeakerSetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmFactorySpeakerSet = (GUIWINDOW *) pWndObj;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFactorySpeakerSet);
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFactorySpeakerSetBg);

    DestroyLabel(&pFactorySpeakerSetLblTitle1);
    DestroyLabel(&pFactorySpeakerSetLblTitle2);
    DestroyLabel(&pFactorySpeakerSetLblTitle3);

    DestroyPicture(&pFactorySpeakerSetBgSpeakerFreq);
    DestroyPicture(&pFactorySpeakerSetBtnSpeakerTest);
    DestroyPicture(&pFactorySpeakerSetBtnSpeakerEnter);
    
    DestroyLabel(&pFactorySpeakerSetLblSpeakerTitle);
    DestroyLabel(&pFactorySpeakerSetLblSpeakerFreq);
    DestroyLabel(&pFactorySpeakerSetLblSpeakerKHz);
    DestroyLabel(&pFactorySpeakerSetLblSpeakerTest);
    DestroyLabel(&pFactorySpeakerSetLblSpeakerEnter);

    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFactorySpeakerSetMenu);

    if(pFactoryConf->speakerFreq != speakerFreq && !imeTag)
    {
        POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
        //设置蜂鸣器频率
    	Opm_SetSpeakerFreq(GetGlb_DevFd()->iOpticDev, pOpmSet, pFactoryConf->speakerFreq);
    	//设置蜂鸣器占空比
    	Opm_SetSpeakerPWM(GetGlb_DevFd()->iOpticDev, pOpmSet, 50);
    }

    //释放文本资源
    FactorySpeakerSetTextRes_Exit(NULL, 0, NULL, 0);
    return iRet;
}

//初始化文本资源
static int FactorySpeakerSetTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    /***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	pFactorySpeakerSetStrTitle1 = TransString("FACTORY_IMPORT_PROFILE");
	pFactorySpeakerSetStrTitle2 = TransString("FACTORY_SPEAKER_VOLUMESETTING");
	pFactorySpeakerSetStrTitle3 = TransString("FACTORY_LASER_FREQUENCYSETTING");

    char buff[16] = {0};
    sprintf(buff, "%d", speakerFreq);
    pFactorySpeakerSetStrSpeakerTitle = TransString("FACTORY_SPEAKER_TITLE");
    pFactorySpeakerSetStrSpeakerFreq = TransString(buff);
    pFactorySpeakerSetStrSpeakerKHz = TransString("Hz");
    pFactorySpeakerSetStrSpeakerTest = TransString("FACTORY_SPEAKER_TEST");
    pFactorySpeakerSetStrSpeakerEnter = TransString("OTDR_SAVE_LBL_SAVE");
    return 0;
}
//释放文本资源
static int FactorySpeakerSetTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    GuiMemFree(pFactorySpeakerSetStrTitle1);
    GuiMemFree(pFactorySpeakerSetStrTitle2);
    GuiMemFree(pFactorySpeakerSetStrTitle3);
    GuiMemFree(pFactorySpeakerSetStrSpeakerTitle);
    GuiMemFree(pFactorySpeakerSetStrSpeakerFreq);
    GuiMemFree(pFactorySpeakerSetStrSpeakerKHz);
    GuiMemFree(pFactorySpeakerSetStrSpeakerTest);
    GuiMemFree(pFactorySpeakerSetStrSpeakerEnter);
    return 0;
}

/*
 * 用于对话框重新还原窗体时的回调函数
 */
static void ReCreateFactorySpeakerSetWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmFactorySpeakerSetInit, FrmFactorySpeakerSetExit, 
		FrmFactorySpeakerSetPaint, FrmFactorySpeakerSetLoop, 
		FrmFactorySpeakerSetPause, FrmFactorySpeakerSetResume,
		NULL);
}

/*
 * 执行输入的回调函数
 */
static void InputOK(void)
{
    char buff[16] = {0};
    unsigned int temp = 0.0f;
    POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
     
	memset(buff,0,16);
	GetIMEInputBuff(buff);
	temp = atoi(buff);

	if(temp < 1000 || temp > 10000)
	{
        CreateIMEDialog(TransString("Please input numbers between 1000  ~  10000 "));
	}
	else
	{
        speakerFreq = temp;
        //设置蜂鸣器频率
		Opm_SetSpeakerFreq(GetGlb_DevFd()->iOpticDev, pOpmSet, speakerFreq);
		//设置蜂鸣器占空比
		Opm_SetSpeakerPWM(GetGlb_DevFd()->iOpticDev, pOpmSet, 50);
	}
}


static int FactorySpeakerSetSpeakerFreq_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactorySpeakerSetSpeakerFreq_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    char buff[16] = {0};
    sprintf(buff, "%d", speakerFreq);
    imeTag = 1;
    IMEInit(buff, 10, 1, ReCreateFactorySpeakerSetWindow, InputOK, NULL);
    return 0;
}

static int FactoryTestSpeaker_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
static int FactoryTestSpeaker_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactorySpeakerEnter_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
static int FactorySpeakerEnter_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{

    pFactoryConf->speakerFreq = speakerFreq;

    SetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);
    SaveSettings(FACTORY_CONFIG);

    DialogInit(180, 97, TransString("Message"),
					   GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
					   0, 0, ReCreateFactorySpeakerSetWindow, NULL, NULL);

    return 0;
}

static int FactorySpeakerToImportProfile_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactorySpeakerToImportProfile_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = NULL;
    
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                    FrmFactoryFirmwareInit, FrmFactoryFirmwareExit, 
                    FrmFactoryFirmwarePaint, FrmFactoryFirmwareLoop, 
			        FrmFactoryFirmwarePause, FrmFactoryFirmwareResume,
                    NULL);
    SendWndMsg_WindowExit(pFrmFactorySpeakerSet);	
    SendSysMsg_ThreadCreate(pWnd);
    
    return 0;
}


static int FactorySpeakerToLaserSet_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactorySpeakerToLaserSet_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = NULL;
    
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                    FrmFactoryLaserSetInit, FrmFactoryLaserSetExit, 
                    FrmFactoryLaserSetPaint, FrmFactoryLaserSetLoop, 
			        FrmFactoryLaserSetPause, FrmFactoryLaserSetResume,
                    NULL);
    SendWndMsg_WindowExit(pFrmFactorySpeakerSet);	
    SendSysMsg_ThreadCreate(pWnd);
    
    return 0;
}