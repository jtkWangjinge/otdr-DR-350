/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmversioninfo.c
* 摘    要：  实现主窗体FrmVersionInfo的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/2 
*
*******************************************************************************/

#include "wnd_frmversioninfo.h"

#include "guipicture.h"
#include "guiwindow.h"
#include "guibase.h"
#include "guilabel.h"
#include "guimessage.h"

#include "app_parameter.h"
#include "app_frmupdate.h"
#include "app_getsetparameter.h"

#include "wnd_global.h"
#include "wnd_frmsystemsetting.h"

//引用外部全局变量
extern PSerialNum      pCurSN;
/*******************************************************************************
**							窗体FrmVersionInfo中的控件定义部分				**
*******************************************************************************/
static GUIWINDOW *pFrmVersionInfo = NULL;

/*******************************************************************************
**							窗体FrmVersionInfo 标题栏中的控件定义部分				 **
*******************************************************************************/
static GUIPICTURE* pFrmVersionInfoBg = NULL;                    //背景图片
static GUIPICTURE* pFrmVersionInfoBtnOK = NULL;                 //确定按钮，退出当前界面

static GUILABEL* pFrmVersionInfoLblTitle = NULL;
static GUILABEL* pFrmVersionInfoLblOK = NULL;                   //OK按钮的文本

static GUICHAR* pFrmVersionInfoStrTitle = NULL;                 //VersionInfo
static GUICHAR* pFrmVersionInfoStrOK = NULL;                    //OK

static GUILABEL* pFrmVersionInfoLblPlatform = NULL;             //平台
static GUILABEL* pFrmVersionInfoLblPlatformContent = NULL;
static GUILABEL* pFrmVersionInfoLblModule = NULL;               //模块
static GUILABEL* pFrmVersionInfoLblModuleContent = NULL;
static GUILABEL* pFrmVersionInfoLblSerialNum = NULL;            //序列号
static GUILABEL* pFrmVersionInfoLblSerialNumContent = NULL;
static GUILABEL* pFrmVersionInfoLblVersion = NULL;              //软件版本
static GUILABEL* pFrmVersionInfoLblVersionContent = NULL;
static GUILABEL* pFrmVersionInfoLblDate = NULL;                 //日期
static GUILABEL* pFrmVersionInfoLblDateContent = NULL;

static GUICHAR* pFrmVersionInfoStrPlatform = NULL;             //平台
static GUICHAR* pFrmVersionInfoStrPlatformContent = NULL;
static GUICHAR* pFrmVersionInfoStrModule = NULL;               //模块
static GUICHAR* pFrmVersionInfoStrModuleContent = NULL;
static GUICHAR* pFrmVersionInfoStrSerialNum = NULL;            //序列号
static GUICHAR* pFrmVersionInfoStrSerialNumContent = NULL;
static GUICHAR* pFrmVersionInfoStrVersion = NULL;              //软件版本
static GUICHAR* pFrmVersionInfoStrVersionContent = NULL;
static GUICHAR* pFrmVersionInfoStrDate = NULL;                 //日期
static GUICHAR* pFrmVersionInfoStrDateContent = NULL;

/*******************************************************************************
*                   窗体FrmVFL内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int VersionInfoTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int VersionInfoTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/***************************窗体的按键事件处理函数*****************************/
static int VersionInfoWndKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int VersionInfoWndKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int VersionInfoBtnOK_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int VersionInfoBtnOK_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);


/***
  * 功能：
        窗体frmversioninfo的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmVersionInfoInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    //得到当前窗体对象 
    pFrmVersionInfo = (GUIWINDOW *) pWndObj;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    VersionInfoTextRes_Init(NULL, 0, NULL, 0);

    pFrmVersionInfoBg= CreatePicture(58, 100, 524, 280, BmpFileDirectory"bg_time_date.bmp");
	pFrmVersionInfoLblTitle = CreateLabel(47 + 20, 77 + 5, 100, 16, pFrmVersionInfoStrTitle);
    pFrmVersionInfoBtnOK= CreatePicture(250, 307, 140, 46, BmpFileDirectory"btn_dialog_press.bmp");
    pFrmVersionInfoLblOK= CreateLabel(250+20, 307+15, 100, 16, pFrmVersionInfoStrOK);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmVersionInfoLblOK);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblOK);

    pFrmVersionInfoLblPlatform = CreateLabel(58+14, 100+13, 100, 16, pFrmVersionInfoStrPlatform);
    pFrmVersionInfoLblPlatformContent = CreateLabel(58 + 14 + 201, 100 + 13, 100, 16, pFrmVersionInfoStrPlatformContent);

    pFrmVersionInfoLblModule = CreateLabel(58 + 14, 100 + 13 + 32, 100, 16, pFrmVersionInfoStrModule);
    pFrmVersionInfoLblModuleContent = CreateLabel(58 + 14 + 201, 100 + 13 + 32, 80, 16, pFrmVersionInfoStrModuleContent);

    pFrmVersionInfoLblSerialNum = CreateLabel(58 + 14, 100 + 13 + 64, 70, 16, pFrmVersionInfoStrSerialNum);
    pFrmVersionInfoLblSerialNumContent = CreateLabel(58 + 14 + 201, 100 + 13 + 64, 80, 16, pFrmVersionInfoStrSerialNumContent);

    pFrmVersionInfoLblVersion = CreateLabel(58 + 14, 100 + 13 + 96, 70, 16, pFrmVersionInfoStrVersion);
    pFrmVersionInfoLblVersionContent = CreateLabel(58 + 14 + 201, 100 + 13 + 96, 80, 16, pFrmVersionInfoStrVersionContent);

    pFrmVersionInfoLblDate = CreateLabel(58 + 14, 100 + 13 + 128, 70, 16, pFrmVersionInfoStrDate);
    pFrmVersionInfoLblDateContent = CreateLabel(58 + 14 + 201, 100 + 13 + 128, 150, 16, pFrmVersionInfoStrDateContent);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblPlatform);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblPlatformContent);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblModule);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblModuleContent);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblSerialNum);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblSerialNumContent);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblVersion);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblVersionContent);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblDate);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmVersionInfoLblDateContent);

     /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmVersionInfo, pFrmVersionInfo);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmVersionInfoBtnOK, pFrmVersionInfo);
    /***************************************************************************
    *                      注册菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();
    //注册窗体的按键消息处理
	//注册软件升级内的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmVersionInfoBtnOK, 
                    VersionInfoBtnOK_Down, NULL, 0, pMsg);	
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmVersionInfoBtnOK, 
                    VersionInfoBtnOK_Up, NULL, 0, pMsg);
    //注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmVersionInfo, 
	                VersionInfoWndKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmVersionInfo, 
	                VersionInfoWndKey_Up, NULL, 0, pMsg);
    
    return iRet;
}

/***
  * 功能：
        窗体frmversioninfo的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmVersionInfoExit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //得到当前窗体对象
    pFrmVersionInfo = (GUIWINDOW *) pWndObj;
    
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmVersionInfo);
    
    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFrmVersionInfoBg);
    DestroyPicture(&pFrmVersionInfoBtnOK);
    DestroyLabel(&pFrmVersionInfoLblTitle);
    DestroyLabel(&pFrmVersionInfoLblOK);

    DestroyLabel(&pFrmVersionInfoLblPlatform);
    DestroyLabel(&pFrmVersionInfoLblPlatformContent); 
    DestroyLabel(&pFrmVersionInfoLblModule);
    DestroyLabel(&pFrmVersionInfoLblModuleContent); 
    DestroyLabel(&pFrmVersionInfoLblSerialNum);
    DestroyLabel(&pFrmVersionInfoLblSerialNumContent);
    DestroyLabel(&pFrmVersionInfoLblVersion);
    DestroyLabel(&pFrmVersionInfoLblVersionContent); 
    DestroyLabel(&pFrmVersionInfoLblDate);
    DestroyLabel(&pFrmVersionInfoLblDateContent);
    
    //释放文本资源
    VersionInfoTextRes_Exit(NULL, 0, NULL, 0);

    return iRet;
}

/***
  * 功能：
        窗体frmversioninfo的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoPaint(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    /* 绘制无效区域窗体 */
	DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    DisplayPicture(pFrmVersionInfoBg);
    DisplayPicture(pFrmVersionInfoBtnOK);
    //DisplayLabel(pFrmVersionInfoLblTitle);
    DisplayLabel(pFrmVersionInfoLblOK);
    
    //DisplayLabel(pFrmVersionInfoLblPlatform);
    //DisplayLabel(pFrmVersionInfoLblPlatformContent); 
    DisplayLabel(pFrmVersionInfoLblModule);
    DisplayLabel(pFrmVersionInfoLblModuleContent); 
    DisplayLabel(pFrmVersionInfoLblSerialNum);
    DisplayLabel(pFrmVersionInfoLblSerialNumContent);
    DisplayLabel(pFrmVersionInfoLblVersion);
    DisplayLabel(pFrmVersionInfoLblVersionContent); 
    DisplayLabel(pFrmVersionInfoLblDate);
    DisplayLabel(pFrmVersionInfoLblDateContent);

	SetPowerEnable(0, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iRet;
}

/***
  * 功能：
        窗体frmversioninfo的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmversioninfo的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoPause(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmversioninfo的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoResume(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

//初始化文本资源
static int VersionInfoTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;

    pFrmVersionInfoStrTitle= TransString("OTDR_ABOUT_INFOMATION");
    pFrmVersionInfoStrOK= GetCurrLanguageText(SYSTEM_LBL_CONFIRM);
    pFrmVersionInfoStrPlatform = GetCurrLanguageText(SYSTEM_LBL_MODULE);
    pFrmVersionInfoStrPlatformContent = TransString("O10");
    pFrmVersionInfoStrModule = GetCurrLanguageText(SYSTEM_LBL_MODULE);
    pFrmVersionInfoStrModuleContent = TransString("O10");
    pFrmVersionInfoStrSerialNum = GetCurrLanguageText(SYSTEM_LBL_SERIAL_NUM);

    VERSION pPar;
    memset(&pPar, 0, sizeof(VERSION));
    ReadUpdateVersion(VERSION_PATH, &pPar);
    char vBuf[12] = {0};
    sprintf(vBuf,"V%s",pPar.Userfs);
    char SN[20];
    GenerateSerialNumber(SN, pCurSN);
    pFrmVersionInfoStrSerialNumContent = TransString(SN);
    pFrmVersionInfoStrVersion = GetCurrLanguageText(SYSTEM_LBL_VERSION);
    pFrmVersionInfoStrVersionContent = TransString(vBuf);
    pFrmVersionInfoStrDate = GetCurrLanguageText(SYSTEM_LBL_SOFTWARE_DATE);
    //显示版本时间
    char s_month[5];
	int month, day, year;
	char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
	month = (strstr(month_names, s_month) - month_names) / 3;    
    char arrFwDate[TEMP_ARRAY_SIZE] = {0};
    sprintf(arrFwDate, "%d-%d-%d %s", year, month + 1, day, __TIME__ );
    pFrmVersionInfoStrDateContent = TransString(arrFwDate);

    return iRet;
}
//释放文本资源
static int VersionInfoTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
    
    GuiMemFree(pFrmVersionInfoStrTitle);
    GuiMemFree(pFrmVersionInfoStrOK);

    GuiMemFree(pFrmVersionInfoStrPlatform);
    GuiMemFree(pFrmVersionInfoStrPlatformContent);
    GuiMemFree(pFrmVersionInfoStrModule);
    GuiMemFree(pFrmVersionInfoStrModuleContent);
    GuiMemFree(pFrmVersionInfoStrSerialNum);
    GuiMemFree(pFrmVersionInfoStrSerialNumContent);
    GuiMemFree(pFrmVersionInfoStrVersion);
    GuiMemFree(pFrmVersionInfoStrVersionContent);
    GuiMemFree(pFrmVersionInfoStrDate);
    GuiMemFree(pFrmVersionInfoStrDateContent);

    return iRet;
}

//按键按下响应函数
static int VersionInfoWndKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iRet = 0;
	unsigned int uiValue;
	uiValue = (unsigned int)pInArg;

	switch (uiValue)
	{
	case KEYCODE_ESC:
        LOG(LOG_INFO, "---esc---down---\n");
        VersionInfoBtnOK_Down(NULL, 0, NULL, 0);
		break;
	case KEYCODE_ENTER:
        LOG(LOG_INFO, "---enter---down---\n");
        VersionInfoBtnOK_Down(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
    return iRet;
}

//按键弹起响应函数
static int VersionInfoWndKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iRet = 0;
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;

    switch (uiValue)
    {
    case KEYCODE_ENTER:
        VersionInfoBtnOK_Up(NULL, 0, NULL, 0);
        break;
    case KEYCODE_ESC:
        VersionInfoBtnOK_Up(NULL, 0, NULL, 0);
        break;
    case KEYCODE_VFL:
        JumpVflWindow();
        break;
    default:
        break;
    }

    return iRet;
}

static int VersionInfoBtnOK_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iRet = 0;

	TouchChange("btn_dialog_press.bmp", pFrmVersionInfoBtnOK, NULL, pFrmVersionInfoLblOK, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iRet;
}

static int VersionInfoBtnOK_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iRet = 0;
	TouchChange("btn_dialog_unpress.bmp", pFrmVersionInfoBtnOK, NULL, pFrmVersionInfoLblOK, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
    //临时变量定义
    GUIWINDOW *pWnd = NULL;
	/* 调用系统升级窗体 */
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmSystemSettingInit, FrmSystemSettingExit, 
						FrmSystemSettingPaint, FrmSystemSettingLoop, 
						FrmSystemSettingPause, FrmSystemSettingResume,
						NULL);			
	SendWndMsg_WindowExit(pFrmVersionInfo);  
	SendSysMsg_ThreadCreate(pWnd);
    return iRet;
}