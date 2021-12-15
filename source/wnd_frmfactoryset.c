/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactoryset.c
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020/10/28
*   
*******************************************************************************/
#include "wnd_frmfactoryset.h"

#include "app_frmotdr.h"

#include "wnd_frmmenubak.h"
#include "wnd_frmdialog.h"

#include "wnd_frmsystemsetting.h"
#include "wnd_frmconfigureinfo.h"
#include "wnd_frmlanguageconfig.h"
#include "wnd_frmlinearity.h"
#include "wnd_frmuncertainty.h"
#include "wnd_frmauthorization.h"

extern CURR_WINDOW_TYPE enCurWindow;
/*******************************************************************************
**							声明窗体中相关控件变量							  **
*******************************************************************************/
static GUIWINDOW *pFrmFactorySet = NULL;
static GUIPICTURE *pFactorySetBg = NULL;
//声明背景图片
static GUIPICTURE* pFactorySetBgDeleteAtt0 = NULL;
static GUIPICTURE* pFactorySetBgEraseNandflash = NULL;
static GUIPICTURE* pFactorySetBgStartFCT = NULL;
static GUIPICTURE* pFactorySetBgPowerTest = NULL;
static GUIPICTURE* pFactorySetBgStartOPM = NULL;
//声明标题标签变量
static GUICHAR* pFacStrTitle = NULL;
static GUILABEL* pFacLblTitle = NULL;
//声明功能按键标签变量
static GUICHAR* pFactorySetStrDeleteAtt0 = NULL;
static GUICHAR* pFactorySetStrEraseNandflash = NULL;
static GUICHAR* pFactorySetStrStartFCT = NULL;
static GUICHAR* pFactorySetStrPowerTest = NULL;
static GUICHAR* pFactorySetStrOpmTest = NULL;

static GUILABEL* pFacLblDeleteAtt0 = NULL;
static GUILABEL* pFactoryLblEraseNandflash = NULL;
static GUILABEL* pFactoryLblStartFCT = NULL;
static GUILABEL* pFactorySetLblPowerTest = NULL;
static GUILABEL* pFactoryLblStartOPM = NULL;

/*******************************************************************************
*               窗体frmvls右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFactorySetMenu = NULL;
/*******************************************************************************
*                   窗体frmvls内部函数声明
*******************************************************************************/
//初始化文本资源
static int FactorySetTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FactorySetTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static void FactorySetMenuCallBack(int iOption);
static void ReCreateWindow(GUIWINDOW **pWnd);

static int FactorySetDeleteAtt0_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySetDeleteAtt0_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySetEraseNandFlash_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySetEraseNandFlash_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySetStartFCT_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySetStartFCT_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
// static int FactorySetPowerTest_Down(void *pInArg, int iInLen, 
//                             void *pOutArg, int iOutLen);
// static int FactorySetPowerTest_Up(void *pInArg, int iInLen, 
//                             void *pOutArg, int iOutLen);
static int FactorySetStartOPM_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int FactorySetStartOPM_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void DeleteATT0(void);
//擦除nandflash
static void EraseNandOk(void);
static void StartFct(void);

static void FactorySetMenuCallBack(int iOption)
{
    GUIWINDOW *pWnd = NULL;
    
	switch (iOption)
	{
	case DEBUG_INFO:
		enCurWindow = ENUM_DEBUG_INFO_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmConfigureInfoInit, FrmConfigureInfoExit,
							FrmConfigureInfoPaint, FrmConfigureInfoLoop,
							FrmConfigureInfoPause, FrmConfigureInfoResume,
							NULL);
	    SendWndMsg_WindowExit(pFrmFactorySet);		//发送消息以便退出当前窗体
	    SendSysMsg_ThreadCreate(pWnd);
        break;
    case LANGUAGE_SET:     
		enCurWindow = ENUM_LANGUAGE_CONFIG_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmLanguageConfigInit, FrmLanguageConfigExit,
							FrmLanguageConfigPaint, FrmLanguageConfigLoop,
							FrmLanguageConfigPause, FrmLanguageConfigResume,
							NULL);
        SendWndMsg_WindowExit(pFrmFactorySet);	
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case NON_LINEARITY:
		enCurWindow = ENUM_NON_LINEARITY_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmLinearityInit, FrmLinearityExit,
			FrmLinearityPaint, FrmLinearityLoop,
			FrmLinearityPause, FrmLinearityResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactorySet);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
        break;
    case UNCERTAINTY:
		enCurWindow = ENUM_UNCERTAINTY_WIN;
        pWnd = CreateWindow(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,
					  FrmUncertaintyInit,FrmUncertaintyExit,
					  FrmUncertaintyPaint,FrmUncertaintyLoop,
					  FrmUncertaintyPause,FrmUncertaintyResume,
					  NULL);
        SendWndMsg_WindowExit(pFrmFactorySet);		//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);
        break;
    case FACTORY_SET:
        break;
	case AUTHORIZATION:
		enCurWindow = ENUM_AUTHORIZATION_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmAuthorizationInit, FrmAuthorizationExit,
			FrmAuthorizationPaint, FrmAuthorizationLoop,
			FrmAuthorizationPause, FrmAuthorizationResume,
			NULL);
		SendWndMsg_WindowExit(pFrmFactorySet);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	case BACK_DOWN:
	case HOME_DOWN:
		enCurWindow = ENUM_OTHER_WIN;
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmSystemSettingInit, FrmSystemSettingExit,
						    FrmSystemSettingPaint, FrmSystemSettingLoop,
						    FrmSystemSettingPause, FrmSystemSettingResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmFactorySet);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}
}

static void ReCreateWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,
					  FrmFactorySetInit,FrmFactorySetExit,
					  FrmFactorySetPaint,FrmFactorySetLoop,
					  FrmFactorySetPause,FrmFactorySetResume,
					  NULL);
}
/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmvls的初始化函数，建立窗体控件、注册消息处理
int FrmFactorySetInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;
    char* StrFactorySetMenu[FACTORY_MENU_COUNT] = {
		"DebugInfo",
		"LanguageSet",
		"Non-linearity",
		"Uncertainty",
		"FactorySet",
		"Authorization"
	};
    //得到当前窗体对象 
    pFrmFactorySet = (GUIWINDOW *) pWndObj;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FactorySetTextRes_Init(NULL, 0, NULL, 0);
    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pFactorySetBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factory.bmp");
    pFactorySetBgDeleteAtt0 = CreatePicture(30, 100, 162, 58, BmpFileDirectory"tmie_apply_unpress5.bmp");
    pFactorySetBgEraseNandflash = CreatePicture(30, 180, 162, 58, BmpFileDirectory"tmie_apply_unpress5.bmp");
    pFactorySetBgStartFCT = CreatePicture(30, 260, 162, 58, BmpFileDirectory"tmie_apply_unpress5.bmp");
    pFactorySetBgPowerTest = CreatePicture(30, 340, 162, 58, BmpFileDirectory"tmie_apply_unpress5.bmp");
    pFactorySetBgStartOPM = CreatePicture(30, 340, 162, 58, BmpFileDirectory"tmie_apply_unpress5.bmp");

    pFacLblTitle = CreateLabel(36, 12, 200, 16, pFacStrTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFacLblTitle);
    
    pFacLblDeleteAtt0 = CreateLabel(61, 100+21, 100, 24, pFactorySetStrDeleteAtt0);
    pFactoryLblEraseNandflash = CreateLabel(51, 180+21, 120, 24, pFactorySetStrEraseNandflash);
    pFactoryLblStartFCT = CreateLabel(61, 260+21, 100, 24, pFactorySetStrStartFCT);
    pFactorySetLblPowerTest = CreateLabel(61, 340+21, 100, 24, pFactorySetStrPowerTest);
    pFactoryLblStartOPM = CreateLabel(61, 340+21, 100, 24, pFactorySetStrOpmTest);
    
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFacLblDeleteAtt0);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFacLblDeleteAtt0);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryLblEraseNandflash);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLblEraseNandflash);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryLblStartFCT);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLblStartFCT);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactorySetLblPowerTest);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactorySetLblPowerTest);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFactoryLblStartOPM);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pFactoryLblStartOPM);
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pFactorySetMenu = CreateStringWndMenu(FACTORY_MENU_COUNT, sizeof(StrFactorySetMenu), 
							StrFactorySetMenu, 0xff00, 4, 54, FactorySetMenuCallBack);

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //必须在持有控件队列互斥锁的情况下操作
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmFactorySet, pFrmFactorySet);
    AddWndMenuToComp1(pFactorySetMenu, pFrmFactorySet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFacLblDeleteAtt0, pFrmFactorySet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryLblEraseNandflash, pFrmFactorySet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryLblStartFCT, pFrmFactorySet);
//     AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactorySetLblPowerTest, pFrmFactorySet);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pFactoryLblStartOPM, pFrmFactorySet);

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
	
	LoginWndMenuToMsg1(pFactorySetMenu, pFrmFactorySet);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFacLblDeleteAtt0, 
                    FactorySetDeleteAtt0_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFacLblDeleteAtt0,
                    FactorySetDeleteAtt0_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLblEraseNandflash, 
                    FactorySetEraseNandFlash_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLblEraseNandflash,
                    FactorySetEraseNandFlash_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLblStartFCT, 
                    FactorySetStartFCT_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLblStartFCT,
                    FactorySetStartFCT_Up, NULL, 0, pMsg);
//     LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactorySetLblPowerTest, 
//                     FactorySetPowerTest_Down, NULL, 0, pMsg);
// 	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactorySetLblPowerTest,
//                     FactorySetPowerTest_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFactoryLblStartOPM, 
                    FactorySetStartOPM_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFactoryLblStartOPM,
                    FactorySetStartOPM_Up, NULL, 0, pMsg);
    return iRet;
    
}
//窗体frmvls的退出函数，释放所有资源
int FrmFactorySetPaint(void *pWndObj)
{
   ///错误标志、返回值定义
    int iRet = 0;
   //得到当前窗体对象
    pFrmFactorySet = (GUIWINDOW *) pWndObj;

    DisplayPicture(pFactorySetBg);
    DisplayPicture(pFactorySetBgDeleteAtt0);
    DisplayPicture(pFactorySetBgEraseNandflash);
    DisplayPicture(pFactorySetBgStartFCT);
//     DisplayPicture(pFactorySetBgPowerTest);
    DisplayPicture(pFactorySetBgStartOPM);

    DisplayLabel(pFacLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFacLblDeleteAtt0);
    SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFactorySetLblPowerTest);
    DisplayLabel(pFacLblDeleteAtt0);
    DisplayLabel(pFactoryLblEraseNandflash);
    DisplayLabel(pFactoryLblStartFCT);
//     DisplayLabel(pFactorySetLblPowerTest);
    DisplayLabel(pFactoryLblStartOPM);
    SetLabelEnable(0, pFacLblDeleteAtt0);
    SetLabelEnable(0, pFactorySetLblPowerTest);
   /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pFactorySetMenu);
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}
//窗体frmvls的绘制函数，绘制整个窗体
int FrmFactorySetLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    
    return iRet;
}
//窗体frmvls的循环函数，进行窗体循环
int FrmFactorySetPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}
//窗体frmvls的挂起函数，进行窗体挂起前预处理
int FrmFactorySetResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}
//窗体frmvls的恢复函数，进行窗体恢复前预处理
int FrmFactorySetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmFactorySet = (GUIWINDOW *) pWndObj;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmFactorySet);
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFactorySetBg);
    DestroyPicture(&pFactorySetBgDeleteAtt0);
    DestroyPicture(&pFactorySetBgEraseNandflash);
    DestroyPicture(&pFactorySetBgStartFCT);
    DestroyPicture(&pFactorySetBgPowerTest);
    DestroyPicture(&pFactorySetBgStartOPM);

    DestroyLabel(&pFacLblTitle);
    DestroyLabel(&pFacLblDeleteAtt0);
    DestroyLabel(&pFactoryLblEraseNandflash);
    DestroyLabel(&pFactoryLblStartFCT);
    DestroyLabel(&pFactorySetLblPowerTest);
    DestroyLabel(&pFactoryLblStartOPM);
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFactorySetMenu);

    //释放文本资源
    FactorySetTextRes_Exit(NULL, 0, NULL, 0);
    return iRet;
}

//初始化文本资源
static int FactorySetTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    /***************************************************************************
    *                         初始化桌面上的文本
    ***************************************************************************/
	pFacStrTitle = TransString("Factory Set");
    pFactorySetStrDeleteAtt0 = TransString("Delete Att0");
    pFactorySetStrEraseNandflash = TransString("Erase NandFlash");
    pFactorySetStrStartFCT = TransString("Start FCT");
    pFactorySetStrPowerTest = TransString("Power Test");
    pFactorySetStrOpmTest = TransString("Start OPM");
    return 0;
}
//释放文本资源
static int FactorySetTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    GuiMemFree(pFacStrTitle);
    GuiMemFree(pFactorySetStrDeleteAtt0);
    GuiMemFree(pFactorySetStrEraseNandflash);
    GuiMemFree(pFactorySetStrStartFCT);
    GuiMemFree(pFactorySetStrPowerTest);
    GuiMemFree(pFactorySetStrOpmTest);
    return 0;
}
//删除att0
static void DeleteATT0(void)
{
    
}
//擦除nandflash
static void EraseNandOk(void)
{
	ShowDialogExtraInfo(TransString("Erasing nandflash..."));
    mysystem("flash_eraseall /dev/mtd0");
	mysystem("flash_eraseall /dev/mtd1");
	mysystem("flash_eraseall /dev/mtd2");
	mysystem("flash_eraseall /dev/mtd3");
	ShowDialogExtraInfo(TransString("Done, Please reboot!!!"));
	MsecSleep(2000);
}
//启动FCT
static void StartFct(void)
{
	char buf[64];
	mysystem("killall -62 listener.sh");

    LOG(LOG_INFO, "---ppid = %d-- \n", getppid());
    sprintf(buf,"kill -9 %d",getppid());
	mysystem(buf);

    LOG(LOG_INFO, "---pid = %d-- \n", getpid());
    sprintf(buf,"kill -9 %d",getpid());
	mysystem(buf);
}

//启动OPM
static void StartOpm(void)
{
	char buf[64];
	mysystem("killall -61 listener.sh");

    LOG(LOG_INFO, "---ppid = %d-- \n", getppid());
    sprintf(buf,"kill -9 %d",getppid());
	mysystem(buf);

    LOG(LOG_INFO, "---pid = %d-- \n", getpid());
    sprintf(buf,"kill -9 %d",getpid());
	mysystem(buf);
}

static int FactorySetDeleteAtt0_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    DialogInit(120, 90, TransString("Message"),
						   TransString("Sure to delete ATT0?"),
						   1, 1, ReCreateWindow, DeleteATT0, NULL);
    return 0;
}
static int FactorySetDeleteAtt0_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    TouchChange("tmie_apply_press5.bmp", pFactorySetBgDeleteAtt0, 
        pFactorySetStrDeleteAtt0, pFacLblDeleteAtt0, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int FactorySetEraseNandFlash_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    DialogInit(120, 90, TransString("Message"),
						   TransString("Sure to erase nandflash?"),
						   1, 1, ReCreateWindow, EraseNandOk, NULL);
    return 0;
}
static int FactorySetEraseNandFlash_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    TouchChange("tmie_apply_press5.bmp", pFactorySetBgEraseNandflash, 
        pFactorySetStrEraseNandflash, pFactoryLblEraseNandflash, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int FactorySetStartFCT_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    TouchChange("tmie_apply_press5.bmp", pFactorySetBgStartFCT, 
        pFactorySetStrStartFCT, pFactoryLblStartFCT, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int FactorySetStartFCT_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    DialogInit(120, 90, TransString("Message"),
						   TransString("Sure to start FCT?"),
						   1, 1, ReCreateWindow, StartFct, NULL);
    return 0;
}
// static int FactorySetPowerTest_Down(void *pInArg, int iInLen, 
//                             void *pOutArg, int iOutLen)
// {
//     printf("FactorySetPowerTest_Down\n");
//     TouchChange("tmie_apply_press5.bmp", pFactorySetBgPowerTest, 
//         pFactorySetStrPowerTest, pFactorySetLblPowerTest, 1);
//     RefreshScreen(__FILE__, __func__, __LINE__);
//     return 0;
// }
// static int FactorySetPowerTest_Up(void *pInArg, int iInLen, 
//                             void *pOutArg, int iOutLen)
// {
//     printf("FactorySetPowerTest_Up\n");
//     
//     return 0;
// }

static int FactorySetStartOPM_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    TouchChange("tmie_apply_press5.bmp", pFactorySetBgStartOPM, 
        pFactorySetStrOpmTest, pFactoryLblStartOPM, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int FactorySetStartOPM_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    DialogInit(120, 90, TransString("Message"),
						   TransString("Sure to start OPM?"),
						   1, 1, ReCreateWindow, StartOpm, NULL);
    return 0;
}

//enter键响应处理函数
static void KeyEnterCallBack(int* iSelected)
{
	switch (*iSelected)
	{
	case 0://delete att0
		*iSelected = 0;
		FactorySetDeleteAtt0_Down(NULL, 0, NULL, 0);
		FactorySetDeleteAtt0_Up(NULL, 0, NULL, 0);
		break;
	case 1://erase nandflash
		*iSelected = 0;
		FactorySetEraseNandFlash_Down(NULL, 0, NULL, 0);
		FactorySetEraseNandFlash_Up(NULL, 0, NULL, 0);
		break;
	case 2://start fct
		*iSelected = 0;
		FactorySetStartFCT_Down(NULL, 0, NULL, 0);
		FactorySetStartFCT_Up(NULL, 0, NULL, 0);
		break;
    case 3://start OPM
		*iSelected = 0;
		FactorySetStartOPM_Down(NULL, 0, NULL, 0);
		FactorySetStartOPM_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}

//按键响应处理
void FactorySetKeyBackCall(int iOption)
{
	static int iKeyFlag = 0;

	switch (iOption)
	{
	case 0://up
		if (iKeyFlag)
		{
			iKeyFlag--;
		}
		break;
	case 1://down
		if (iKeyFlag >= 0 && iKeyFlag < 3)
		{
			iKeyFlag++;
		}
		break;
	case 2://enter
		KeyEnterCallBack(&iKeyFlag);
		break;
	default:
		break;
	}
}