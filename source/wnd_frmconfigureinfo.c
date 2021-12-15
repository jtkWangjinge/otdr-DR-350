/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmconfigureinfo.c
* 摘    要：  实现主窗体wnd_frmconfigureinfo的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  Wangjinge
* 完成日期：  2020/10/26
*******************************************************************************/

#include "wnd_frmconfigureinfo.h"

/*************************************
* 定义wnd_frmconfigure.c引用其他头文件
*************************************/
#include "app_parameter.h"
#include "app_systemsettings.h"
#include "app_matfile.h"
#include "app_measuringresult.h"

#include "wnd_frmmenubak.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmlanguageconfig.h"
#include "wnd_frmlinearity.h"
#include "wnd_frmuncertainty.h"
#include "wnd_frmfactoryset.h"
#include "wnd_frmauthorization.h"


//设置标志位
static int iSaveMatlabFileFlag = 0;
static int iSaveResultFileFlag = 0;
extern CURR_WINDOW_TYPE enCurWindow;                //当前的窗口

/*******************************************************************************
**							声明窗体中相关控件变量							  **
*******************************************************************************/
static GUIWINDOW *pFrmConfigureInfo                         = NULL;
//定义桌面背景
static GUIPICTURE *pConfigureInfoBg                         = NULL;
//声明标题标签变量
static GUICHAR* pConfigureInfoStrTitle                      = NULL;
static GUILABEL* pConfigureInfoLblTitle                     = NULL;

static GUIPICTURE *pConfigureInfoBtnSaveMatlabFile          = NULL;//保存matlab文件按钮
static GUIPICTURE *pConfigureInfoBtnSaveResultFile          = NULL;//保存结果信息按钮

static GUICHAR* pConfigureInfoStrBatteryVersion             = NULL;//电源管理MCU版本 label
static GUILABEL* pConfigureInfoLblBatteryVersion            = NULL;//
static GUICHAR* pConfigureInfoStrBatteryVersionNum          = NULL;//电源管理MCU版本号 label
static GUILABEL* pConfigureInfoLblBatteryVersionNum         = NULL;//

static GUICHAR* pConfigureInfoStrOPMVersion                 = NULL;//OPM MCU版本 label
static GUILABEL* pConfigureInfoLblOPMVersion                = NULL;//
static GUICHAR* pConfigureInfoStrOPMVersionNum              = NULL;//OPM MCU版本号 label
static GUILABEL* pConfigureInfoLblOPMVersionNum             = NULL;//

static GUICHAR* pConfigureInfoStrFPGAVersion                = NULL;//FPGA版本 label
static GUILABEL* pConfigureInfoLblFPGAVersion               = NULL;//
static GUICHAR* pConfigureInfoStrFPGAVersionNum             = NULL;//FPGA版本号 label
static GUILABEL* pConfigureInfoLblFPGAVersionNum            = NULL;//

static GUICHAR* pConfigureInfoStrSaveMatlabFile             = NULL;//保存matlab文件label
static GUILABEL* pConfigureInfoLblSaveMatlabFile            = NULL;//
static GUICHAR* pConfigureInfoStrSaveResultFile             = NULL;//保存matlab文件label
static GUILABEL* pConfigureInfoLblSaveResultFile            = NULL;//

/*******************************************************************************
*                   窗体frmauthorization右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pConfigureInfoMenu = NULL;
/*******************************************************************************
*                   窗体frmConfigureInfo内部函数声明
*******************************************************************************/
//初始化文本资源
static int FrmConfigureInfoTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int FrmConfigureInfoTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//保存matlab文件
static int FrmConfigureInfoSaveMatlabFile_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FrmConfigureInfoSaveMatlabFile_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//保存结果文件
static int FrmConfigureInfoSaveResultFile_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int FrmConfigureInfoSaveResultFile_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//右侧菜单回调函数
static void FrmConfigureInfoMenuCallBack(int iOption);

//功能函数声明
static void GetBatteryVersion(char* versionNum);
static void GetOPMVersion(char* versionNum);
static void GetFPGAVersion(char* versionNum);
static void SaveMatlabFile(int saveFlag);
static void SaveResultFile(int saveFlag);

int FrmConfigureInfoInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    char* StrConfigMenu[FACTORY_MENU_COUNT] = {
        "DebugInfo",
		"LanguageSet",
		"Non-linearity",
		"Uncertainty",
		"FactorySet",
		"Authorization"/*,
		"SerialNum"*/
    };

	//得到当前窗体对象 
    pFrmConfigureInfo = (GUIWINDOW *) pWndObj;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FrmConfigureInfoTextRes_Init(NULL, 0, NULL, 0);
    /***************************************************************************
    *                      创建桌面上各个区域的背景控件otdr_drop_wave_unpress.bmp
    ***************************************************************************/
    pConfigureInfoBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_factory.bmp");

    //标题栏文本
    pConfigureInfoLblTitle = CreateLabel(36, 12, 200, 16, pConfigureInfoStrTitle);

    //电源MCU版本
    pConfigureInfoLblBatteryVersion = CreateLabel(40, 100, 180, 16, pConfigureInfoStrBatteryVersion);
    pConfigureInfoLblBatteryVersionNum = CreateLabel(230, 100, 50, 16, pConfigureInfoStrBatteryVersionNum);

    pConfigureInfoLblOPMVersion = CreateLabel(40, 130, 150, 16, pConfigureInfoStrOPMVersion);
    pConfigureInfoLblOPMVersionNum = CreateLabel(230, 130, 50, 16, pConfigureInfoStrOPMVersionNum);

    pConfigureInfoLblFPGAVersion = CreateLabel(40, 160, 100, 16, pConfigureInfoStrFPGAVersion);
    pConfigureInfoLblFPGAVersionNum = CreateLabel(230, 160, 50, 16, pConfigureInfoStrFPGAVersionNum);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblBatteryVersion);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblBatteryVersionNum);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblOPMVersion);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblOPMVersionNum);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblFPGAVersion);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblFPGAVersionNum);
    
    //保存Matlab文件
    if (iSaveMatlabFileFlag)
    {
        pConfigureInfoBtnSaveMatlabFile = CreatePicture(40, 190, 25, 25,
                                                        BmpFileDirectory "fiber_mask_select.bmp");
    }
	else
	{
        pConfigureInfoBtnSaveMatlabFile = CreatePicture(40, 190, 25, 25,
                                                        BmpFileDirectory "fiber_mask_unpress.bmp");
    }
	
    //保存Matlab文件
    if (iSaveMatlabFileFlag)
    {
        pConfigureInfoBtnSaveResultFile = CreatePicture(40, 230, 25, 25,
                                                        BmpFileDirectory "fiber_mask_select.bmp");
    }
    else
    {
        pConfigureInfoBtnSaveResultFile = CreatePicture(40, 230, 25, 25,
                                                        BmpFileDirectory "fiber_mask_unpress.bmp");
    }

    pConfigureInfoLblSaveMatlabFile = CreateLabel(75, 194, 150, 24, pConfigureInfoStrSaveMatlabFile);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblSaveMatlabFile);

    pConfigureInfoLblSaveResultFile = CreateLabel(75, 234, 150, 24, pConfigureInfoStrSaveResultFile);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pConfigureInfoLblSaveResultFile);
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
	pConfigureInfoMenu = CreateStringWndMenu(FACTORY_MENU_COUNT, sizeof(StrConfigMenu), StrConfigMenu, 0xff00,
											0, 54, FrmConfigureInfoMenuCallBack);

	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //必须在持有控件队列互斥锁的情况下操作
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmConfigureInfo, pFrmConfigureInfo);
    AddWndMenuToComp1(pConfigureInfoMenu, pFrmConfigureInfo);

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pConfigureInfoBtnSaveMatlabFile, pFrmConfigureInfo);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pConfigureInfoLblSaveMatlabFile, pFrmConfigureInfo);
    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pConfigureInfoBtnSaveResultFile, pFrmConfigureInfo);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pConfigureInfoLblSaveResultFile, pFrmConfigureInfo);
    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //必须在持有消息队列的互斥锁情况下操作
    /***************************************************************************
    *                      注册ODTR右侧菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pConfigureInfoBtnSaveMatlabFile, 
                    FrmConfigureInfoSaveMatlabFile_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pConfigureInfoBtnSaveMatlabFile, 
                    FrmConfigureInfoSaveMatlabFile_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pConfigureInfoLblSaveMatlabFile, 
                    FrmConfigureInfoSaveMatlabFile_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pConfigureInfoLblSaveMatlabFile, 
                    FrmConfigureInfoSaveMatlabFile_Up, NULL, 0, pMsg);
                    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pConfigureInfoBtnSaveResultFile, 
                    FrmConfigureInfoSaveResultFile_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pConfigureInfoBtnSaveResultFile, 
                    FrmConfigureInfoSaveResultFile_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pConfigureInfoLblSaveResultFile, 
                    FrmConfigureInfoSaveResultFile_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pConfigureInfoLblSaveResultFile, 
                    FrmConfigureInfoSaveResultFile_Up, NULL, 0, pMsg);
                    
	LoginWndMenuToMsg1(pConfigureInfoMenu, pFrmConfigureInfo);

    return iReturn;
}
int FrmConfigureInfoPaint(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;

	DisplayPicture(pConfigureInfoBg);

    DisplayLabel(pConfigureInfoLblTitle);

    DisplayLabel(pConfigureInfoLblBatteryVersion);
    DisplayLabel(pConfigureInfoLblBatteryVersionNum);
    DisplayLabel(pConfigureInfoLblOPMVersion);
    DisplayLabel(pConfigureInfoLblOPMVersionNum);
    DisplayLabel(pConfigureInfoLblFPGAVersion);
    DisplayLabel(pConfigureInfoLblFPGAVersionNum);

    DisplayPicture(pConfigureInfoBtnSaveMatlabFile);
    DisplayLabel(pConfigureInfoLblSaveMatlabFile);
    DisplayPicture(pConfigureInfoBtnSaveResultFile);
    DisplayLabel(pConfigureInfoLblSaveResultFile);
    
	//设置当前界面
	enCurWindow = ENUM_DEBUG_INFO_WIN;
    /***************************************************************************
    *                        显示右侧菜单栏控件
    ***************************************************************************/
	DisplayWndMenu1(pConfigureInfoMenu);
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}
int FrmConfigureInfoLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}
int FrmConfigureInfoPause(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}
int FrmConfigureInfoResume(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;
    
    return iReturn;
}
int FrmConfigureInfoExit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iReturn = 0;

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //得到当前窗体对象
    pFrmConfigureInfo = (GUIWINDOW *) pWndObj;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
	
    ClearMessageReg(pMsg);
	
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmConfigureInfo);

    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pConfigureInfoBg);

    DestroyLabel(&pConfigureInfoLblTitle);

    DestroyLabel(&pConfigureInfoLblBatteryVersion);
    DestroyLabel(&pConfigureInfoLblBatteryVersionNum);
    DestroyLabel(&pConfigureInfoLblOPMVersion);
    DestroyLabel(&pConfigureInfoLblOPMVersionNum);
    DestroyLabel(&pConfigureInfoLblFPGAVersion);
    DestroyLabel(&pConfigureInfoLblFPGAVersionNum);

    DestroyPicture(&pConfigureInfoBtnSaveMatlabFile);
    DestroyLabel(&pConfigureInfoLblSaveMatlabFile);
    
    DestroyPicture(&pConfigureInfoBtnSaveResultFile);
    DestroyLabel(&pConfigureInfoLblSaveResultFile);
    
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pConfigureInfoMenu);   
    //释放文本资源
    FrmConfigureInfoTextRes_Exit(NULL, 0, NULL, 0);
    
    return iReturn;
}

//配置信息的按键响应
void DebugInfoKeyCallBack(int iSelected)
{
	static int iKeyFlag = 0;

	switch (iSelected)
	{
	case 0://up
		iKeyFlag = iKeyFlag ? 0 : 1;
		break;
	case 1://down
		iKeyFlag = iKeyFlag ? 0 : 1;
		break;
	case 2://enter
		if (!iKeyFlag)
		{
			FrmConfigureInfoSaveMatlabFile_Down(NULL, 0, NULL, 0);
			FrmConfigureInfoSaveMatlabFile_Up(NULL, 0, NULL, 0);
		}
		else
		{
			FrmConfigureInfoSaveResultFile_Down(NULL, 0, NULL, 0);
			FrmConfigureInfoSaveResultFile_Up(NULL, 0, NULL, 0);
		}
		break;
	default:
		break;
	}
}

/*******************************************************************************
*                   窗体frmConfigureInfo内部函数声明
*******************************************************************************/
//初始化文本资源
static int FrmConfigureInfoTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

    pConfigureInfoStrTitle = TransString("Debug Information");

    pConfigureInfoStrBatteryVersion = TransString("Battery version:");
    pConfigureInfoStrOPMVersion = TransString("OPM MCU version:");
    pConfigureInfoStrFPGAVersion = TransString("FPGA version:");

    char buf[20] = {0};
    GetBatteryVersion(buf);
    pConfigureInfoStrBatteryVersionNum = TransString(buf);
    memset(buf, 0, 20);
    GetOPMVersion(buf);
    pConfigureInfoStrOPMVersionNum = TransString(buf);
    memset(buf, 0, 20);
    GetFPGAVersion(buf);
    pConfigureInfoStrFPGAVersionNum= TransString(buf);
    
    pConfigureInfoStrSaveMatlabFile = TransString("Save matlab file");
    pConfigureInfoStrSaveResultFile = TransString("Save result info");
    
    return iReturn;
}
//释放文本资源
static int FrmConfigureInfoTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

    GuiMemFree(pConfigureInfoStrTitle);

    GuiMemFree(pConfigureInfoStrBatteryVersion);
    GuiMemFree(pConfigureInfoStrOPMVersion);
    GuiMemFree(pConfigureInfoStrFPGAVersion);
    GuiMemFree(pConfigureInfoStrBatteryVersionNum);
    GuiMemFree(pConfigureInfoStrOPMVersionNum);
    GuiMemFree(pConfigureInfoStrFPGAVersionNum);
    
    GuiMemFree(pConfigureInfoStrSaveMatlabFile);
    GuiMemFree(pConfigureInfoStrSaveResultFile);
    
    return iReturn;
}

static int FrmConfigureInfoSaveMatlabFile_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

	iSaveMatlabFileFlag = !iSaveMatlabFileFlag;
	iSaveMatlabFileFlag ? SetPictureBitmap(BmpFileDirectory"fiber_mask_select.bmp", pConfigureInfoBtnSaveMatlabFile)
              : SetPictureBitmap(BmpFileDirectory"fiber_mask_unpress.bmp", pConfigureInfoBtnSaveMatlabFile);
	
    DisplayPicture(pConfigureInfoBtnSaveMatlabFile);
    DisplayLabel(pConfigureInfoLblSaveMatlabFile);
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

static int FrmConfigureInfoSaveMatlabFile_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

    SaveMatlabFile(iSaveMatlabFileFlag);
    
    return iReturn;
}

static int FrmConfigureInfoSaveResultFile_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

	iSaveResultFileFlag = !iSaveResultFileFlag;
	iSaveResultFileFlag ? SetPictureBitmap(BmpFileDirectory"fiber_mask_select.bmp", pConfigureInfoBtnSaveResultFile)
              : SetPictureBitmap(BmpFileDirectory"fiber_mask_unpress.bmp", pConfigureInfoBtnSaveResultFile);
	
    DisplayPicture(pConfigureInfoBtnSaveResultFile);
    DisplayLabel(pConfigureInfoLblSaveResultFile);
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

static int FrmConfigureInfoSaveResultFile_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iReturn = 0;

    SaveResultFile(iSaveResultFileFlag);
    
    return iReturn;
}


//右侧菜单回调函数
static void FrmConfigureInfoMenuCallBack(int iOption)
{
    GUIWINDOW *pWnd = NULL;
	
	switch (iOption)
	{
	case DEBUG_INFO: 
        break;
    case LANGUAGE_SET:
		//设置当前界面
		enCurWindow = ENUM_LANGUAGE_CONFIG_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmLanguageConfigInit, FrmLanguageConfigExit,
			FrmLanguageConfigPaint, FrmLanguageConfigLoop,
			FrmLanguageConfigPause, FrmLanguageConfigResume,
			NULL);
		SendWndMsg_WindowExit(pFrmConfigureInfo);
		SendSysMsg_ThreadCreate(pWnd);
        break;
    case NON_LINEARITY:
		enCurWindow = ENUM_NON_LINEARITY_WIN;
		pWnd = CreateWindow(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,
 			FrmLinearityInit,FrmLinearityExit,
 			FrmLinearityPaint,FrmLinearityLoop,
 			FrmLinearityPause,FrmLinearityResume,
 			NULL);
		SendWndMsg_WindowExit(pFrmConfigureInfo);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
        break;
    case UNCERTAINTY:
		enCurWindow = ENUM_UNCERTAINTY_WIN;
		pWnd = CreateWindow(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,
 			FrmUncertaintyInit,FrmUncertaintyExit,
 			FrmUncertaintyPaint,FrmUncertaintyLoop,
 			FrmUncertaintyPause,FrmUncertaintyResume,
 			NULL);
		SendWndMsg_WindowExit(pFrmConfigureInfo);		//发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);
        break;
    case FACTORY_SET:
		enCurWindow = ENUM_FACTORY_SET_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmFactorySetInit, FrmFactorySetExit, 
						FrmFactorySetPaint, FrmFactorySetLoop, 
 				        FrmFactorySetPause, FrmFactorySetResume,
                         NULL);
         SendWndMsg_WindowExit(pFrmConfigureInfo);	
         SendSysMsg_ThreadCreate(pWnd);
        break;
	case AUTHORIZATION:
		enCurWindow = ENUM_AUTHORIZATION_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmAuthorizationInit, FrmAuthorizationExit,
			FrmAuthorizationPaint, FrmAuthorizationLoop,
			FrmAuthorizationPause, FrmAuthorizationResume,
			NULL);
		SendWndMsg_WindowExit(pFrmConfigureInfo);
		SendSysMsg_ThreadCreate(pWnd);
		break;
	/*case SERIALNUM:
		enCurWindow = ENUM_SERIALNUM_WIN;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmFactorySetInit, FrmFactorySetExit,
			FrmFactorySetPaint, FrmFactorySetLoop,
			FrmFactorySetPause, FrmFactorySetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmConfigureInfo);
		SendSysMsg_ThreadCreate(pWnd);
		break;*/
	case BACK_DOWN:
	case HOME_DOWN:
		enCurWindow = ENUM_OTHER_WIN;
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						    FrmSystemSettingInit, FrmSystemSettingExit,
						    FrmSystemSettingPaint, FrmSystemSettingLoop,
						    FrmSystemSettingPause, FrmSystemSettingResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmConfigureInfo);  
	    SendSysMsg_ThreadCreate(pWnd); 
	 	break;
	default:
		break;
	}   
}
//获取电池的版本号
static void GetBatteryVersion(char* versionNum)
{
    if (versionNum)
        strcpy(versionNum, "Vx.xx");
}
//获取opm的版本号
static void GetOPMVersion(char* versionNum)
{
    if (versionNum)
        strcpy(versionNum, "Vx.xx");
}
//获取fpga的版本号
static void GetFPGAVersion(char* versionNum)
{
    DEVFD *pDevFd = GetGlb_DevFd();

    unsigned int iVersion = 0;
    
    //读取版本寄存器
	int iErr = Opm_ReadReg(pDevFd->iOpticDev, FPGA_VERSION_ADDR, &iVersion);
	if (iErr == 0)
	{
		sprintf(versionNum, "0x%x", iVersion);
        LOG(LOG_INFO, "fpga version is %x\n", iVersion);
    }
	else
	{
        LOG(LOG_ERROR, "read fpga version error:%x\n", iErr);
    }
    
}

//保存matlab文件
static void SaveMatlabFile(int saveFlag)
{
	PMATFILE mat = GetMatFile();
	mat->setSave(mat, saveFlag);
}
//保存测量结果文件
static void SaveResultFile(int saveFlag)
{
	SetMeasuringResultEnable(saveFlag);
}

