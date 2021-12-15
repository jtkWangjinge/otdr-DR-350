/*******************************************************************************
* Copyright(c)2018，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactorylaserset.c
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

#include "app_frminit.h"
#include "wnd_frmfactorylaserset.h"
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
#include "wnd_frmfactoryspeakerset.h"
#include "app_parameter.h"
#include "wnd_frmime.h"
#include "app_systemsettings.h"
#include "app_frmsourcelight.h"

extern PFactoryConfig pFactoryConf;
static int wavelengthIndex = 0;
static unsigned int pulseFreq = 0;
/*******************************************************************************
**							声明窗体中相关控件变量							  **
*******************************************************************************/
static GUIWINDOW *pFrmFactoryLaserSet = NULL;
static GUIPICTURE *pFactoryLaserSetBg = NULL;
//声明标题标签变量
static GUICHAR* pFactoryLaserSetStrTitle1 = NULL;
static GUILABEL* pFactoryLaserSetLblTitle1 = NULL;

static GUICHAR* pFactoryLaserSetStrTitle2 = NULL;
static GUILABEL* pFactoryLaserSetLblTitle2 = NULL;

static GUICHAR* pFactoryLaserSetStrTitle3 = NULL;
static GUILABEL* pFactoryLaserSetLblTitle3 = NULL;

//稳定光源频率调节
static GUICHAR* pFactoryLaserSetStrLaserTitle = NULL;
static GUICHAR* pFactoryLaserSetStrLaserFreq = NULL;
static GUICHAR* pFactoryLaserSetStrLaserKHz = NULL;
static GUICHAR* pFactoryLaserSetStrLaserTest = NULL;
static GUICHAR* pFactoryLaserSetStrLaserEnter = NULL;

static GUILABEL* pFactoryLaserSetLblLaserTitle = NULL;
static GUILABEL* pFactoryLaserSetLblLaserFreq = NULL;
static GUILABEL* pFactoryLaserSetLblLaserKHz = NULL;
static GUILABEL* pFactoryLaserSetLblLaserTest = NULL;
static GUILABEL* pFactoryLaserSetLblLaserEnter = NULL;

static GUIPICTURE* pFactoryLaserSetBgLaserFreq = NULL;
static GUIPICTURE* pFactoryLaserSetBtnLaserTest = NULL;
static GUIPICTURE* pFactoryLaserSetBtnLaserEnter = NULL;

// extern SOURCELIGHTCONFIG* pSourceLightConfig;
extern LIGHT_SOURCE_POWER_CONFIG_LIST* pLightSourcePowerConfig;
static GUICHAR *pWaveLenghtStrTitle = NULL;
static GUILABEL *pWaveLenghtLblTitle = NULL;

static GUIPICTURE* RadioPicture[2];
static GUILABEL* labelOfSidebar[2];
static GUICHAR*  textOfSidebar[2];
static int lightState = 0;

/*******************************************************************************
*               窗体laserset右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFactoryLaserSetMenu = NULL;
/*******************************************************************************
*                   窗体laserset内部函数声明
*******************************************************************************/
//初始化文本资源
static int FactoryLaserSetTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FactoryLaserSetTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static void FactoryLaserSetMenuCallBack(int iOption);
                            
static int FactoryLaserSetLaserFreq_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryLaserSetLaserFreq_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryTestLaser_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryTestLaser_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryLaserEnter_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryLaserEnter_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
                            
static int FactoryLaserToImportProfile_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryLaserToImportProfile_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int FactoryLaserToSpeakerSet_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactoryLaserToSpeakerSet_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int RadioPicture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int RadioPicture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void ReCreateFactoryLaserSetWindow(GUIWINDOW **pWnd);

static void SetSourceLightWavelength(int wave);

static void SetSourceLightFrequence(int frq);

static void SourcelightPowerSwitch(int iSwitch);

static void FactoryLaserSetMenuCallBack(int iOption)
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
	    SendWndMsg_WindowExit(pFrmFactoryLaserSet);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
    case 1:        
        break;
    case 2:        
        pWnd = CreateFactoryWindow();
        SendWndMsg_WindowExit(pFrmFactoryLaserSet);  
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 3:
        pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
					  FrmFacDRInit,FrmFacDRExit,
					  FrmFacDRPaint,FrmFacDRLoop,
					  FrmFacDRPause,FrmFacDRResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmFactoryLaserSet);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 4:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmFactorySetInit, FrmFactorySetExit, 
                        FrmFactorySetPaint, FrmFactorySetLoop, 
				        FrmFactorySetPause, FrmFactorySetResume,
                        NULL);
        SendWndMsg_WindowExit(pFrmFactoryLaserSet);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case 5:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmMaintanceDateInit, FrmMaintanceDateExit, 
							FrmMaintanceDatePaint, FrmMaintanceDateLoop, 
							FrmMaintanceDatePause, FrmMaintanceDateResume,
							NULL);			//pWnd由调度线程释放
	    SendWndMsg_WindowExit(pFrmFactoryLaserSet);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN:
	case HOME_DOWN:
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmStandbysetInit, FrmStandbysetExit, 
						    FrmStandbysetPaint, FrmStandbysetLoop, 
						    FrmStandbysetPause, FrmStandbysetResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFactoryLaserSet);  
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
int FrmFactoryLaserSetInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;
    unsigned int StrVlsMenu[] = {	
        1
	};
    //得到当前窗体对象 
    pFrmFactoryLaserSet = (GUIWINDOW *) pWndObj;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FactoryLaserSetTextRes_Init(NULL, 0, NULL, 0);
    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFactoryLaserSetBg = CreatePicture(0, 0, 681, 480, BmpFileDirectory"sysset_time_bg5.bmp");
    
    pFactoryLaserSetLblTitle1 = CreateLabel(0, 24, 100, 24, pFactoryLaserSetStrTitle1);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryLaserSetLblTitle1);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLaserSetLblTitle1);
	
    pFactoryLaserSetLblTitle2 = CreateLabel(100, 24, 100, 24, pFactoryLaserSetStrTitle2);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryLaserSetLblTitle2);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLaserSetLblTitle2);
    
    pFactoryLaserSetLblTitle3 = CreateLabel(200, 24, 100, 24, pFactoryLaserSetStrTitle3);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryLaserSetLblTitle3);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLaserSetLblTitle3);
    
    //左侧边栏
    pWaveLenghtLblTitle = CreateLabel(20, 100, 220, 24, pWaveLenghtStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pWaveLenghtLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pWaveLenghtLblTitle);

    int i;
    for(i=0;i<2;i++)
    {
        if(i == wavelengthIndex)
            RadioPicture[i] =CreatePicture(22, 126+35*i, 28, 28,BmpFileDirectory"btn_sourcelight_enable.bmp");
        else
            RadioPicture[i] =CreatePicture(22, 126+35*i, 28, 28,BmpFileDirectory"btn_sourcelight_unable.bmp");
        
        labelOfSidebar[i] = CreateLabel(60, 129+35*i, 120, 24, textOfSidebar[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), labelOfSidebar[i]);
	    SetLabelAlign(GUILABEL_ALIGN_LEFT, labelOfSidebar[i]);
    }

	//稳定光源频率调节
    pFactoryLaserSetLblLaserTitle = CreateLabel(200, 100, 300, 24, pFactoryLaserSetStrLaserTitle);
    pFactoryLaserSetLblLaserFreq = CreateLabel(200, 154, 140, 24, pFactoryLaserSetStrLaserFreq);
    pFactoryLaserSetLblLaserKHz = CreateLabel(350, 154, 100, 24, pFactoryLaserSetStrLaserKHz);
    pFactoryLaserSetLblLaserTest = CreateLabel(200, 216, 138, 24, pFactoryLaserSetStrLaserTest);
    pFactoryLaserSetLblLaserEnter = CreateLabel(350, 216, 138, 24, pFactoryLaserSetStrLaserEnter);

    pFactoryLaserSetBgLaserFreq = CreatePicture(200, 150, 140, 25, BmpFileDirectory"btn_mark_input.bmp");
    pFactoryLaserSetBtnLaserTest = CreatePicture(200, 200, 138, 48, BmpFileDirectory"btn_reset_unpress1.bmp");
    pFactoryLaserSetBtnLaserEnter = CreatePicture(350, 200, 138, 48, BmpFileDirectory"btn_reset_unpress1.bmp");

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryLaserSetLblLaserTitle);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFactoryLaserSetLblLaserTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryLaserSetLblLaserFreq);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLaserSetLblLaserFreq);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryLaserSetLblLaserKHz);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pFactoryLaserSetLblLaserKHz);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryLaserSetLblLaserTest);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLaserSetLblLaserTest);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFactoryLaserSetLblLaserEnter);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLaserSetLblLaserEnter);
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFactoryLaserSetMenu = CreateWndMenu1(factoryRank, sizeof(StrVlsMenu), StrVlsMenu,  0xffff,
								1, 0, 40, FactoryLaserSetMenuCallBack);

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //必须在持有控件队列互斥锁的情况下操作
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactoryLaserSet, pFrmFactoryLaserSet);
    AddWndMenuToComp1(pFactoryLaserSetMenu, pFrmFactoryLaserSet);
    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryLaserSetBgLaserFreq, pFrmFactoryLaserSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryLaserSetBtnLaserTest, pFrmFactoryLaserSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFactoryLaserSetBtnLaserEnter, pFrmFactoryLaserSet);
    
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryLaserSetLblTitle1, pFrmFactoryLaserSet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryLaserSetLblTitle2, pFrmFactoryLaserSet);

    for(i=0;i<2;i++)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), RadioPicture[i], 
                  pFrmFactoryLaserSet);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), labelOfSidebar[i], 
                  pFrmFactoryLaserSet);
    }
    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	LoginWndMenuToMsg1(pFactoryLaserSetMenu, pFrmFactoryLaserSet); 
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLaserSetBgLaserFreq, 
                    FactoryLaserSetLaserFreq_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLaserSetBgLaserFreq,
                    FactoryLaserSetLaserFreq_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLaserSetBtnLaserTest, 
                    FactoryTestLaser_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLaserSetBtnLaserTest,
                    FactoryTestLaser_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLaserSetBtnLaserEnter, 
                    FactoryLaserEnter_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLaserSetBtnLaserEnter,
                    FactoryLaserEnter_Up, NULL, 0, pMsg);
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLaserSetLblTitle1, 
                    FactoryLaserToImportProfile_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLaserSetLblTitle1,
                    FactoryLaserToImportProfile_Up, NULL, 0, pMsg);
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLaserSetLblTitle2, 
                    FactoryLaserToSpeakerSet_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLaserSetLblTitle2,
                    FactoryLaserToSpeakerSet_Up, NULL, 0, pMsg);
    
    for(i=0;i<2;i++)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, RadioPicture[i], 
                    RadioPicture_Down, NULL, i, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, RadioPicture[i], 
                    RadioPicture_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, labelOfSidebar[i], 
                    RadioPicture_Down, NULL, i, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, labelOfSidebar[i], 
                    RadioPicture_Up, NULL, i, pMsg);
    }
    return iRet;
    
}
//窗体frmvls的退出函数，释放所有资源
int FrmFactoryLaserSetPaint(void *pWndObj)
{
   ///错误标志、返回值定义
    int iRet = 0;
   //得到当前窗体对象
    pFrmFactoryLaserSet = (GUIWINDOW *) pWndObj;

    DisplayPicture(pFactoryLaserSetBg);
    DisplayLabel(pFactoryLaserSetLblTitle1);
    DisplayLabel(pFactoryLaserSetLblTitle2);
    DisplayLabel(pFactoryLaserSetLblTitle3);

    //调节蜂鸣器音量
    DisplayPicture(pFactoryLaserSetBgLaserFreq);
    DisplayPicture(pFactoryLaserSetBtnLaserTest);
    DisplayPicture(pFactoryLaserSetBtnLaserEnter);
    
    DisplayLabel(pFactoryLaserSetLblLaserTitle);
    DisplayLabel(pFactoryLaserSetLblLaserFreq);
    DisplayLabel(pFactoryLaserSetLblLaserKHz);
    DisplayLabel(pFactoryLaserSetLblLaserTest);
    DisplayLabel(pFactoryLaserSetLblLaserEnter);
    
    DisplayLabel(pWaveLenghtLblTitle);
    int i;
    for(i=0;i<2;i++){
       DisplayPicture(RadioPicture[i]);
       DisplayLabel(labelOfSidebar[i]); 
    }
   /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pFactoryLaserSetMenu);
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
    BoardWorkInit(pOtdrTopSettings->pOpmSet, 0);

    return iRet;
}
//窗体frmvls的绘制函数，绘制整个窗体
int FrmFactoryLaserSetLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
	//SendMsg_DisableLoop(pWndObj);
    return iRet;
}
//窗体frmvls的循环函数，进行窗体循环
int FrmFactoryLaserSetPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}
//窗体frmvls的挂起函数，进行窗体挂起前预处理
int FrmFactoryLaserSetResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}
//窗体frmvls的恢复函数，进行窗体恢复前预处理
int FrmFactoryLaserSetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmFactoryLaserSet = (GUIWINDOW *) pWndObj;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFactoryLaserSet);
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFactoryLaserSetBg);

    DestroyLabel(&pFactoryLaserSetLblTitle1);
    DestroyLabel(&pFactoryLaserSetLblTitle2);

    DestroyPicture(&pFactoryLaserSetBgLaserFreq);
    DestroyPicture(&pFactoryLaserSetBtnLaserTest);
    DestroyPicture(&pFactoryLaserSetBtnLaserEnter);
    
    DestroyLabel(&pFactoryLaserSetLblLaserTitle);
    DestroyLabel(&pFactoryLaserSetLblLaserFreq);
    DestroyLabel(&pFactoryLaserSetLblLaserKHz);
    DestroyLabel(&pFactoryLaserSetLblLaserTest);
    DestroyLabel(&pFactoryLaserSetLblLaserEnter);

    DestroyLabel(&pWaveLenghtLblTitle);
    int i;
    for(i=0;i<2;i++){
       DestroyPicture(&RadioPicture[i]);
       DestroyLabel(&labelOfSidebar[i]); 
    }
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFactoryLaserSetMenu);

    //释放文本资源
    FactoryLaserSetTextRes_Exit(NULL, 0, NULL, 0);
    
    SourcelightPowerSwitch(0);
    return iRet;
}

//初始化文本资源
static int FactoryLaserSetTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    /***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	pFactoryLaserSetStrTitle1 = TransString("FACTORY_IMPORT_PROFILE");
	pFactoryLaserSetStrTitle2 = TransString("FACTORY_SPEAKER_VOLUMESETTING");
	pFactoryLaserSetStrTitle3 = TransString("FACTORY_LASER_FREQUENCYSETTING");

    char buff[16] = {0};
    if (0 == pulseFreq)
    {
        pulseFreq = pLightSourcePowerConfig->powerConfig[wavelengthIndex].pulse;
    }
    sprintf(buff, "%d", pulseFreq);
    pFactoryLaserSetStrLaserTitle = TransString("FACTORY_LASER_TITLE");
    pFactoryLaserSetStrLaserFreq = TransString(buff);
    pFactoryLaserSetStrLaserKHz = TransString("Hz");
    pFactoryLaserSetStrLaserTest = TransString("FACTORY_SPEAKER_TEST");
    pFactoryLaserSetStrLaserEnter = TransString("OTDR_SAVE_LBL_SAVE");

    // 波长选择文本
    static char* name[2] = {"1310nm(SM)","1550nm(SM)"};
    int i;
    pWaveLenghtStrTitle = TransString("SOURCE_LIGHT_WAVE");
    for(i=0;i<2;i++){
        textOfSidebar[i] = TransString(name[i]);
    }
    
    return 0;
}
//释放文本资源
static int FactoryLaserSetTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    GuiMemFree(pFactoryLaserSetStrTitle1);
    GuiMemFree(pFactoryLaserSetStrTitle2);
    GuiMemFree(pFactoryLaserSetStrTitle3);
    GuiMemFree(pFactoryLaserSetStrLaserTitle);
    GuiMemFree(pFactoryLaserSetStrLaserFreq);
    GuiMemFree(pFactoryLaserSetStrLaserKHz);
    GuiMemFree(pFactoryLaserSetStrLaserTest);
    GuiMemFree(pFactoryLaserSetStrLaserEnter);
    GuiMemFree(pWaveLenghtStrTitle);
    int i;
    for(i=0;i<2;i++){
        GuiMemFree(textOfSidebar[i]);    
    }
    return 0;
}

/*
 * 用于对话框重新还原窗体时的回调函数
 */
static void ReCreateFactoryLaserSetWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		FrmFactoryLaserSetInit, FrmFactoryLaserSetExit, 
		FrmFactoryLaserSetPaint, FrmFactoryLaserSetLoop, 
		FrmFactoryLaserSetPause, FrmFactoryLaserSetResume,
		NULL);
}

/*
 * 执行输入的回调函数
 */
static void InputOK(void)
{
    char buff[16] = {0};
    unsigned int temp = 0.0f;
    
	memset(buff,0,16);
	GetIMEInputBuff(buff);
	temp = atoi(buff);

	if(temp < 1 || temp > 499)
	{
        CreateIMEDialog(TransString("Please input numbers between 1  ~  499 "));
	}
	else
	{
	    pulseFreq = temp;
	    pLightSourcePowerConfig->powerConfig[wavelengthIndex].pulse = temp;
	}
}


static int FactoryLaserSetLaserFreq_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactoryLaserSetLaserFreq_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    char buff[16] = {0};
    sprintf(buff, "%d", pLightSourcePowerConfig->powerConfig[wavelengthIndex].pulse);
    IMEInit(buff, 10, 1, ReCreateFactoryLaserSetWindow, InputOK, NULL);
    return 0;
}

static int FactoryTestLaser_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
static int FactoryTestLaser_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    if (lightState)
    {
        lightState = 0;
        SourcelightPowerSwitch(0); 
        // 设置测试
        pFactoryLaserSetStrLaserTest = TransString("FACTORY_SPEAKER_TEST");
    }
    else
    {
        lightState = 1;
        SourcelightPowerSwitch(1); 
        // 设置停止
        pFactoryLaserSetStrLaserTest = TransString("OTDR_LBL_TEST_STOP");
    }
    SetLabelText(pFactoryLaserSetStrLaserTest, pFactoryLaserSetLblLaserTest);
    DisplayPicture(pFactoryLaserSetBtnLaserTest);
    DisplayLabel(pFactoryLaserSetLblLaserTest);
    SyncCurrFbmap();
    return 0;
}

static int FactoryLaserEnter_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
static int FactoryLaserEnter_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    SetSettingsData((void*)pLightSourcePowerConfig, sizeof(LIGHT_SOURCE_POWER_CONFIG_LIST), LIGHTSOURCE_POWER_CONFIG_LIST);
    SaveSettings(LIGHTSOURCE_POWER_CONFIG_LIST);

    DialogInit(120, 90, TransString("Message"),
					   GetCurrLanguageText(DIALOG_SAVE_SUCCESS),
					   0, 0, ReCreateFactoryLaserSetWindow, NULL, NULL);

    return 0;
}

static int FactoryLaserToImportProfile_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactoryLaserToImportProfile_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = NULL;
    
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                    FrmFactoryFirmwareInit, FrmFactoryFirmwareExit, 
                    FrmFactoryFirmwarePaint, FrmFactoryFirmwareLoop, 
			        FrmFactoryFirmwarePause, FrmFactoryFirmwareResume,
                    NULL);
    SendWndMsg_WindowExit(pFrmFactoryLaserSet);	
    SendSysMsg_ThreadCreate(pWnd);
    
    return 0;
}

static int FactoryLaserToSpeakerSet_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int FactoryLaserToSpeakerSet_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = NULL;
    
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                    FrmFactorySpeakerSetInit, FrmFactorySpeakerSetExit, 
                    FrmFactorySpeakerSetPaint, FrmFactorySpeakerSetLoop, 
			        FrmFactorySpeakerSetPause, FrmFactorySpeakerSetResume,
                    NULL);
    SendWndMsg_WindowExit(pFrmFactoryLaserSet);	
    SendSysMsg_ThreadCreate(pWnd);
    
    return 0;
}

//左侧复选框对应函数
static int RadioPicture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int RadioPicture_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    int i;
    LOG(LOG_INFO, "iOutLen = %d\n", iOutLen);
    if(iOutLen < 2)
    {
        for(i=0;i<2;i++)
        {
            if(i == iOutLen)
            {
                SetPictureBitmap(BmpFileDirectory"btn_sourcelight_enable.bmp", RadioPicture[i]);
            }
            else
            {
                SetPictureBitmap(BmpFileDirectory"btn_sourcelight_unable.bmp", RadioPicture[i]);
            }
        } 
        wavelengthIndex = iOutLen;
    }
    
    for(i = 0; i < 2; ++i)
    {
        DisplayPicture(RadioPicture[i]);
    }

    if(lightState)
    {
        SourcelightPowerSwitch(1);
    }

    pulseFreq = pLightSourcePowerConfig->powerConfig[wavelengthIndex].pulse;
    char buff[16] = {0};
    sprintf(buff, "%d", pLightSourcePowerConfig->powerConfig[wavelengthIndex].pulse);
    pFactoryLaserSetStrLaserFreq = TransString(buff);
    SetLabelText(pFactoryLaserSetStrLaserFreq, pFactoryLaserSetLblLaserFreq);
    DisplayPicture(pFactoryLaserSetBgLaserFreq);
    DisplayLabel(pFactoryLaserSetLblLaserFreq);
    
    SyncCurrFbmap();
    return 0;
}


static void SetSourceLightWavelength(int wave)
{
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;

	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;

    if(SOURCELIGHTWAVELENTH1310 == wave)
    {
        Opm_SetWave(iOpmFd, pOtdrTopSet->pOpmSet, ENUM_WAVE_1310NM);
    }
    else
    {
        Opm_SetWave(iOpmFd, pOtdrTopSet->pOpmSet, ENUM_WAVE_1550NM);
    }
}

static void SetSourceLightFrequence(int frq)
{
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;

	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;

    unsigned int mode;
	UINT32 uiTemp = Opm_GetDaqMode(iOpmFd, pOtdrTopSet->pOpmSet);
    unsigned int uiMode = uiTemp;

    mode = frq - SOURCELIGHTMODECW;
    uiMode = (uiMode & ~SOURCELIGHT_MASK) | (mode<< SOURCELIGHT_SHIFT);
    LOG(LOG_INFO, "sourcelight power uiMode 0x%x\n", uiMode);

    Opm_WriteReg(iOpmFd, FPGA_DAQMODE_SET, &uiMode);

	uiTemp = Opm_GetDaqMode(iOpmFd, pOtdrTopSet->pOpmSet);
    LOG(LOG_INFO, "FPGA_DAQMODE_SET = 0x%x\n", uiTemp);
}

//0:close   1: open
static void SourcelightPowerSwitch(int iSwitch)
{
	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;
    unsigned int uiPulse = 0xffff;
    unsigned int uiVdd = 0x0c00;

    if(iSwitch == 0)
    {
        uiPulse = 0;
        Opm_LdPulseTurnOff(iOpmFd, pOtdrTopSet->pOpmSet);
    }
    else
    {
        Opm_LdPulseTurnOn(iOpmFd, pOtdrTopSet->pOpmSet);
        uiPulse = 0xffff;
    }
    Opm_WriteReg(iOpmFd, FPGA_LD_PULSE_WIDTH, &uiPulse);
    
    unsigned int pwmValue = pLightSourcePowerConfig->powerConfig[wavelengthIndex].pulse;
    Opm_WriteReg(iOpmFd, FPGA_OPTICAL_PWM_SET, &pwmValue);

    if(iSwitch == 0)
        return;
    
    Opm_SetVddVoltage(iOpmFd, pOtdrTopSet->pOpmSet, uiVdd);
    
    SetSourceLightWavelength(wavelengthIndex);
    SetSourceLightFrequence(SOURCELIGHTMODECW);
}

