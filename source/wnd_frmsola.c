/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsola.c
* 摘    要：  实现初始化窗体frmsola的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2016-10-28
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#include "wnd_frmsola.h"


/****************************************
* 为实现窗体frminit而需要引用的其他头文件
****************************************/
#include "wnd_frmmain.h"
#include "wnd_frmsolawidget.h"
#include "wnd_frmmenubak.h"
#include "wnd_global.h"
#include "app_global.h"
#include "app_parameter.h"
#include "app_frminit.h"
#include "app_middle.h"
#include "app_frmotdr.h"
#include "app_curve.h"
#include "app_frmotdrmessage.h"
#include "wnd_signal.h"
#include "wnd_frmquicksave.h"
#include "app_queue.h"
#include "wnd_frmoperatesor.h"
#include "wnd_frmsolatable.h"
#include "wnd_frmsolasetting.h"
#include "wnd_frmsolaconfig.h"
#include "wnd_frmprogressbar.h"
#include "app_sola.h"
#include "wnd_frmsolaidentify.h"
#include "guiphoto.h"
#include "app_screenshots.h"
#include "app_solafile.h"
#include "wnd_frmdialog.h"
#include "wnd_saveasdialog.h"
#include "app_pdfreportsola.h"
#include "wnd_frmsolaident.h"
#include "wnd_frmsolasave.h"
#include "app_measuringresult.h"
#include <limits.h>

extern OPM_WAVE enSolaWaveChoose;
extern SOLA_MESSAGER* pSolaMessager;
static int iSetSpliceRatioPrometFlag = 0;
extern char *pSolaOpenCurrPath;
static char* pSolaFileName = NULL;

/************************
* 窗体frmsola中的静态资源
************************/
//侧边栏menu上的文本索引
static unsigned int strMenu[] = 
{
	2
};
static unsigned int strMenu1[] = 
{
	3
};

//事件列表缓冲区
static SolaEvents currSolaEvents = SOLA_EVENTS_INITIALIZER;
static CALLLBACKWINDOW pSolaEscapeTo = NULL;

/************************
* 窗体frmsola中的文本资源
************************/
//状态栏上的文本
static GUICHAR* pSolaStrSettingTitle = NULL;

static GUICHAR* pSolaStrSaveResult = NULL;
static GUICHAR* pSolaStrSavePath = NULL;
static GUICHAR* pSolaStrWave = NULL;

static GUICHAR* pSolaStrDisplayFileName = NULL;
static GUILABEL* pSolaLblDisplayFileName = NULL;
/************************
* 窗体frmsola中的文本资源
************************/
//状态栏上的文本
static GUICHAR* pSolaStrTitle = NULL;

/************************
* 窗体frmsola中的标签资源
************************/
//状态栏上的标签
static GUILABEL* pSolaLblTitle = NULL;
static GUILABEL* pSolaLblSettingTitle = NULL;

static GUILABEL* pSolaLblSaveResult = NULL;
static GUILABEL* pSolaLblSavePath = NULL;
static GUILABEL* pSolaLblWave = NULL;

static GUIPICTURE *pSolaBottumBg = NULL;
static GUIPICTURE *pSolaWaveBg = NULL;
static GUIPICTURE *pSolaDisplayFileNameBg = NULL;

/************************
* 窗体frmsola中的窗体控件
************************/
static GUIWINDOW *pFrmSola = NULL;

//状态栏、桌面、信息栏
static GUIPICTURE *pSolaBgDesk = NULL;
static GUIPICTURE *pSolaBgTableTitle2 = NULL;

static SOLA_WIDGET* pSolaWidget = NULL;
static SOLA_TABLE_EVENT_ITEM* pSolaEventItemTable = NULL;
static SOLA_TABLE_PASS* pSolaPassTable = NULL;
static SOLA_TABLE_LINE* pSolaLineLossTable = NULL;
static WNDMENU1* pSolaMenu = NULL;
static PROGRESS_BAR* pProgressBar = NULL;

/********************************
* 窗体frmsola中的文本资源处理函数
********************************/
//初始化文本资源
static int InitTextRes_Init(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen);
//释放文本资源
static int InitTextRes_Exit(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen);

/********************************
* 窗体frmsola中的控件事件处理函数
********************************/
static int SolaTabSolaSetting_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaTabSolaSetting_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int SolaTypeChangeNext_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaTypeChangePre_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
/********************************
* 窗体frmsola中的错误事件处理函数
********************************/
static int SolaErrProc_Func(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen);

/********************************
* 窗体frmsola中的其它内部处理函数
********************************/
static void SolaMenuCall(int buttonIndex);
static int SolaWidgetItemCheckedCall(int index);
static void DisplaySaveResultLabel(int result, char* path);
static int cheakName();
static void setSolaWidgetsAfterEventsPrepared();
static void setSolaWidgetsWaveLength(OPM_WAVE enSolaWaveChoose);
static void UpdateProgress(int iProgress);
static void UpdateWave(int iWave);
void FrmSolaReCreateWindow(GUIWINDOW **pWnd);
//开始/停止测量sola
static void SolaStartAndStop(void);
//sola另存为功能函数
static void SolaSaveAs(void);
static void SetSolaFileName(const char* fileName, int refreshFlag);//filename全路径带后缀
/***
  * 功能：
        窗体frmsola的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmSola = (GUIWINDOW *) pWndObj;

    if (pSolaFileName == NULL)
    {
        pSolaFileName = (char*)malloc(256);
        memset(pSolaFileName, 0, 256);
    }
    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    InitTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pSolaBgDesk = CreatePhoto("bg_sola");
    pSolaBgTableTitle2 = CreatePhoto("otdr_top2");
    pSolaLblTitle = CreateLabel(0, 24, 100, 40, pSolaStrTitle);
    pSolaLblSettingTitle = CreateLabel(100, 24, 100, 40, pSolaStrSettingTitle);
    pSolaLblDisplayFileName = CreateLabel(401, 24, 281, 24, pSolaStrDisplayFileName);
    pSolaDisplayFileNameBg = CreatePhoto("otdr_top5");
    pSolaBottumBg = CreatePhoto("sola_save_info_bg");
    //Sola控件
    pSolaWidget = CreateSolaWidget(0, 42, 681, 241, 
                                  SolaWidgetItemCheckedCall, pFrmSola);

    //sola事件表控件
    pSolaLineLossTable = CreateSolaLineTable(28, 289, 306, 75, pFrmSola);
    pSolaPassTable = CreateSolaPassTable(449, 289, 207, 75, pFrmSola);
    pSolaEventItemTable = CreateSolaEventItemTable(28, 373, 615, 75, pFrmSola);

    //sola保存结果
    pSolaLblSaveResult = CreateLabel(30, 448, 600, 24, pSolaStrSaveResult);
    pSolaLblSavePath = CreateLabel(30, 464, 600, 24, pSolaStrSavePath);
    
    //进度条
    pProgressBar = ProgressBar_Create(455, 455, 190, 25);
    ProgressBar_SetColor(pProgressBar, 0xffffff);
    ProgressBar_SetBackgroundColor(pProgressBar, 0x1B4352);
    ProgressBar_SetTextEnable(pProgressBar, 0);
    
    //波长
    pSolaWaveBg = CreatePhoto("sola_save_info_bg1");
    pSolaLblWave = CreateLabel(395, 460, 60, 24, pSolaStrWave);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaLblWave);
  	//侧边栏menu

    if(SolaEvents_IsVaild(&currSolaEvents))
        pSolaMenu = CreateWndMenu1(5, sizeof(strMenu), strMenu, MENU_ALL, -1, 0, 40, SolaMenuCall);						   
    else
        pSolaMenu = CreateWndMenu1(3, sizeof(strMenu), strMenu, MENU_ALL, -1, 0, 40, SolaMenuCall);

    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaLblSettingTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaLblSettingTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaLblTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaLblDisplayFileName);

    //设置按钮区的控件
    //...

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键及光标事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSola,
                  pFrmSola);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaBgTableTitle2, 
                  pFrmSola);

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaEventItemTable->downButtonTouch, 
                  pFrmSola);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaEventItemTable->upButtonTouch, 
                  pFrmSola);
    //注册桌面上的控件
    //...
    //注册按钮区的控件
	AddWndMenuToComp1(pSolaMenu, pFrmSola);
    //注册信息栏上的控件
    //...

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaBgTableTitle2, 
                    SolaTabSolaSetting_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaBgTableTitle2, 
                    SolaTabSolaSetting_Up, NULL, 0, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaEventItemTable->downButtonTouch, 
                    SolaTypeChangeNext_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaEventItemTable->upButtonTouch, 
                    SolaTypeChangePre_Down, NULL, 0, pMsg);

    //注册桌面上控件的消息处理
    //...
    //注册按钮区控件的消息处理
	LoginWndMenuToMsg1(pSolaMenu, pFrmSola);
    //注册信息栏上控件的消息处理
    //...
    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmSola,
                    SolaErrProc_Func, NULL, 0, pMsg);

    setSolaWidgetsWaveLength(enSolaWaveChoose);
    setSolaWidgetsAfterEventsPrepared(&currSolaEvents);

    //停止窗体循环
    SendWndMsg_LoopDisable(pFrmSola);
        						   
    return iReturn;
}


/***
  * 功能：
        窗体frmsola的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmSola = (GUIWINDOW *) pWndObj;
    
    ProgressBar_Destroy(pProgressBar);
    //清空消息队列中的消息注册项
    //***************************************************************/
    ClearMessageReg(GetCurrMessage());

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmSola);

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pSolaBgDesk);
    DestroyPicture(&pSolaBgTableTitle2);
    DestroyPicture(&pSolaBottumBg);
    DestroyPicture(&pSolaWaveBg);
    DestroyPicture(&pSolaDisplayFileNameBg);
	DestroyLabel(&pSolaLblTitle);  
   	DestroyLabel(&pSolaLblSettingTitle);  
    DestroyLabel(&pSolaLblDisplayFileName);
    DestroyLabel(&pSolaLblSaveResult);
    DestroyLabel(&pSolaLblSavePath);

    //销毁状态栏上的控件
    //...
    //销毁桌面上的控件
    DestroySolaWidget(&pSolaWidget);
    DestroySolaLineTable(pSolaLineLossTable);
    DestroySolaPassTable(pSolaPassTable);
    DestroySolaLossTable(pSolaEventItemTable);
    
    //销毁按钮区的控件
	DestroyWndMenu1(&pSolaMenu);    
    //销毁信息栏上的控件
    //...

    //释放文本资源
    //***************************************************************/
    InitTextRes_Exit(NULL, 0, NULL, 0);

    //释放字体资源
    //***************************************************************/
    //释放按钮区的字体
    //...
    pSolaEscapeTo = NULL;
    
    return iReturn;
}


/***
  * 功能：
        窗体frmsola的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmSola = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pSolaBgDesk);
    DisplayPicture(pSolaBgTableTitle2);
    DisplayLabel(pSolaLblTitle);    
    DisplayLabel(pSolaLblSettingTitle);
    DisplayPicture(pSolaDisplayFileNameBg);
    DisplayLabel(pSolaLblDisplayFileName);
    DisplayPicture(pSolaBottumBg);
    
    DisplayLabel(pSolaLblSaveResult);
    DisplayLabel(pSolaLblSavePath);

    //显示桌面上的控件
    DisplaySolaWidget(pSolaWidget);
    DisplaySolaLineTable(pSolaLineLossTable);
    DisplaySolaPassTable(pSolaPassTable);
    DisplaySolaLossTable(pSolaEventItemTable);
    
    //显示按钮区的控件
	DisplayWndMenu1(pSolaMenu);
	SetPowerEnable(1, 1);

    SyncCurrFbmap();
    if(iSetSpliceRatioPrometFlag)
    {
        iSetSpliceRatioPrometFlag = !iSetSpliceRatioPrometFlag;
    	DialogInit(120, 90, TransString("Warning"), 
        		   TransString("SOLA_START_PROMPT_INFO"),
        		   0, 0, FrmSolaReCreateWindow, NULL, NULL);
    }

    return iReturn;
}

void setTestWidgetEnable(int enable)
{
    SetPictureEnable(enable, pSolaBgTableTitle2);
    if(enable)
    {
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaLblSettingTitle);
    }
    else
    {
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pSolaLblSettingTitle);
    }
    DisplayLabel(pSolaLblSettingTitle);
    SyncCurrFbmap();
}


/***
  * 功能：
        窗体frmsola的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaLoop(void *pWndObj)
{
	/*错误标志、返回值定义*/
	int iErr = 0;
    SolaMessage message;

    //读取Sola的消息
    if (0 != CurrSola_GetMessage(&message))
    {
        MsecSleep(50);
        return iErr;
    }
    //根据消息类型进行处理
    switch (message.Type)
    {
    //SOLA已经启动测量
    case SOLA_STARTED: {
        //清除可能存在的上一次的测量结果
        clearSolaEvents();
        setSolaWidgetsAfterEventsPrepared(&currSolaEvents);
        DisplaySolaWidget(pSolaWidget);
        //刷新界面底部的显示
        DisplayPicture(pSolaBottumBg);
        setTestWidgetEnable(0);
        SetWndMenuItemEnble(0,1,pSolaMenu);
        RefreshScreen(__FILE__, __func__, __LINE__);
        break;
    }

    //波长发生变化
    case SOLA_WAVE_CHANGED: {
        int iWave = message.Value;
        UpdateWave(iWave);
        break;
    }
    
    //SOLA测量进度发生变化
    case SOLA_PROGRESS_CHANGED: {
        int iProgress = message.Value;
        UpdateProgress(iProgress);
        break;
    }

    //SOLA数据准备好
    case SOLA_DATA_READY: {
        //读取事件，并刷新刷新界面的显示
        CurrSola_GetSolaEvents(&currSolaEvents);
        setSolaWidgetsAfterEventsPrepared(&currSolaEvents);
        DisplaySolaWidget(pSolaWidget);
        cheakName();
        SetSolaFileName(currSolaEvents.absolutePath, 0);
        RefreshScreen(__FILE__, __func__, __LINE__);
        break;
    }
    
    //SOLA测量已经停止
    case SOLA_STOPED: {
        //停止轮询任务
        SendWndMsg_LoopDisable(pFrmSola);
        //刷新界面底部的显示
        DisplayPicture(pSolaBottumBg);
        //刷新侧边按钮栏
        int count = SolaEvents_IsVaild(&currSolaEvents) ? 5 : 3;
        RefreshMenu(pSolaMenu, -1, count, MENU_ALL, sizeof(strMenu), strMenu);
        SetWndMenuItemEnble(0, 1, pSolaMenu);
        RefreshScreen(__FILE__, __func__, __LINE__);
        setTestWidgetEnable(1);

		// 保存SOLA测量结果，导出excel	--by SHI
		SaveSOLAMeasuringResult();
		
        break;
    }

    //产生错误
    case SOLA_ERROR: {
        fprintf(stderr, "Sola error:[%d]%s\n", message.Value, CurrSola_GetErrorString(message.Value));
        break;   
    }

    default:
        break;
    }

	return iErr;
}


/***
  * 功能：
        窗体frmsola的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体frmsola的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//创建sola window
GUIWINDOW* CreateSolaWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
          	                       FrmSolaInit, FrmSolaExit, 
          	                       FrmSolaPaint, FrmSolaLoop, 
          				           FrmSolaPause, FrmSolaResume,
          	                       NULL);
    return pWnd;
}

static void SetCurrentPath(void)
{
    if(pSolaOpenCurrPath)
    {
        char buf[512] = {};
        getcwd(buf, 512);
        if(strncmp(buf, MntUsbDirectory, strlen(MntUsbDirectory)) != 0)
        {
            strcpy(pSolaOpenCurrPath, buf);
        }
        else
        {
            chdir(pSolaOpenCurrPath);
        }
    }
}

//使用Sola打开SOLA文件
int SolaViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where)
{
    LOG(LOG_INFO, "fileName is %s\n", fileName);
    int iErr = SolaLoad(fileName, &currSolaEvents);
	if (!iErr)
	{
	    SetSolaFileName(fileName, 1);
		GUIWINDOW* pDest = CreateSolaWindow();		 
		SendWndMsg_WindowExit(from);  
		SendSysMsg_ThreadCreate(pDest);
        if(where)
        {
            pSolaEscapeTo = where;
        }

    }

    SetCurrentPath();
	return iErr;
}


//初始化文本资源
static int InitTextRes_Init(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化状态栏上的文本
	pSolaStrTitle = TransString("SOLA");  
    pSolaStrSettingTitle = TransString("SOLA_SETTING");
    pSolaStrSaveResult = TransString("");  
    pSolaStrSavePath = TransString("");  
    pSolaStrWave = TransString("");
    pSolaStrDisplayFileName = (strlen(pSolaFileName) == 0) ? TransString("")
                                                           : TransString(pSolaFileName);

    return iReturn;
}


//释放文本资源
static int InitTextRes_Exit(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
	GuiMemFree(pSolaStrTitle);
    GuiMemFree(pSolaStrSettingTitle);
    GuiMemFree(pSolaStrSaveResult);  
    GuiMemFree(pSolaStrSavePath);  
    GuiMemFree(pSolaStrDisplayFileName);
    
    return iReturn;
}

//错误事件处理函数
static int SolaErrProc_Func(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}

//弹出重命名键盘
static int PopRenameKeyboard(char* fileName, char* pathAndName)
{
    int iRet = 0;
    int flag = 0;
    char newFileName[128] = {};
    char* fileType[2] = {".PDF", ".SOLA"};
    int defaultOption = pSolaMessager->autoFilename.fileFormat;
    
    if(SAVE_AS_DIALOG_KEY_ENTER == ShowSaveAsDialog(fileName, newFileName, fileType, 2, defaultOption, &flag))
    {
        if(!strcasecmp(newFileName, fileType[0]) || !strcasecmp(newFileName, fileType[1]))
        {
            DialogInit(120, 90, TransString("Warning"),
						TransString("DIALOG_FILE_NOT_EMPTY"),
						0, 0, FrmSolaReCreateWindow, NULL, NULL);
            iRet = -1;
        } 
        else
        {
            if(!flag)
            {
                isExist(pOtdrTopSettings->sDefSavePath.cSolaSavePath);
            }

            if(flag)
            {
                sprintf(pathAndName, "%s/%s", MntUsbDirectory, newFileName);
            }
            else
            {
                sprintf(pathAndName, "%s/%s", pOtdrTopSettings->sDefSavePath.cSolaSavePath, newFileName);
            }

            LOG(LOG_INFO, "pathAndName is %s\n", pathAndName);

            if(strstr(newFileName, fileType[1]))
            {
                iRet = WriteSola(pathAndName, 1);
            }
            else
            {
                iRet = CreateSolaReport(pathAndName);
            }
        }
    }
    else
    {
        iRet = -3;
    }
    return iRet;
}

//开始/停止测量sola
static void SolaStartAndStop(void)
{
    RefreshMenu1(pSolaMenu, -1, 1, MENU_NONE, sizeof(strMenu1), strMenu1, 0);
    SetWndMenuItemEnble(0,0,pSolaMenu);
    RefreshScreen(__FILE__, __func__, __LINE__);
    if (CurrSola_IsRunning())
    {
        CurrSola_Stop();
    }
    else
    {
        //启动Sola
        ClearSolaEventLossOffset();
        CurrSola_Start();
        //启动窗体循环，执行轮询任务(轮询任务会在Sola停止后自动停止)
        SendWndMsg_LoopEnable(pFrmSola);
        setTestWidgetEnable(0);
    }
}

//sola另存为功能函数
static void SolaSaveAs(void)
{
    int iRet = 0;       
    DisplaySaveResultLabel(0, NULL);
    if(cheakName())
    {
        return;
    }
    char *fileName = strrchr(currSolaEvents.absolutePath,'/')+1;
    char pathAndName[512] = {0};
    iRet = PopRenameKeyboard(fileName, pathAndName);
    DisplaySaveResultLabel(iRet, pathAndName);
    SyncCurrFbmap();
}

//设置sola显示文件名
static void SetSolaFileName(const char* fileName, int refreshFlag)
{
    if (fileName == NULL)
        return;
    char* temp = strrchr(fileName, '/');
    if (temp)
    {
        char buf[256] = {0};
        memcpy(buf, temp+1, strlen(temp)-(strlen(".SOLA")+1));
        if (pSolaFileName == NULL)
        {
            pSolaFileName = (char*)malloc(256);
            memset(pSolaFileName, 0, 256);
        }
        if(!strcmp(pSolaFileName, buf))
        {
            return;
        } 
        strcpy(pSolaFileName, buf);
        if (pSolaStrDisplayFileName)
        {
            GuiMemFree(pSolaStrDisplayFileName);
        }
        pSolaStrDisplayFileName = TransString(buf);
        SetLabelText(pSolaStrDisplayFileName, pSolaLblDisplayFileName);
        DisplayPicture(pSolaDisplayFileNameBg);
        DisplayLabel(pSolaLblDisplayFileName);
        if(refreshFlag)
        {
            RefreshScreen(__FILE__, __func__, __LINE__);
        }  
    }
}

static void SolaRunReport()
{
    int iRet = 0;      
    DisplaySaveResultLabel(0, NULL);
    if(cheakName())
    {
        return;
    }
    char pathAndName[512] = {0};
    strcpy(pathAndName, currSolaEvents.absolutePath);
    
    //更换后缀名
    char *suffix = strstr(pathAndName, ".SOLA");
    char *suffix1 = strstr(pathAndName, ".sola");
    
    if(suffix || suffix1)
    {
        if(suffix)
            *suffix = 0;
        if(suffix1)
            *suffix1 = 0;
        strcat(pathAndName, ".PDF");
    }
    
    iRet = CreateSolaReport(pathAndName);
    DisplaySaveResultLabel(iRet, pathAndName);
    SyncCurrFbmap();
}


static void SolaMenuCall(int buttonIndex)
{
    GUIWINDOW* pWnd = NULL;
	
	switch (buttonIndex)
	{
	case 0: //开始测量
	    SolaStartAndStop();
        break;
	case 1: //保存/打开
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmSOLASaveInit, FrmSOLASaveExit, 
						FrmSOLASavePaint, FrmSOLASaveLoop, 
						FrmSOLASavePause, FrmSOLASaveResume,
						NULL);
        SendWndMsg_WindowExit(pFrmSola);  
    	SendSysMsg_ThreadCreate(pWnd);

	    break;
	case 2: //管理
	    pWnd = CreateSolaIdentWindow();
        SendWndMsg_WindowExit(pFrmSola);  
    	SendSysMsg_ThreadCreate(pWnd);

        break;
	case 3: //生产报告
	    SolaRunReport();
        break;
    case 4://Save As...
        SolaSaveAs();
        break;
    case 5:
        break;
	case HOME_DOWN:
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                            FrmMainInit, FrmMainExit, 
                            FrmMainPaint, FrmMainLoop, 
				            FrmMainPause, FrmMainResume,
                            NULL);
        SendWndMsg_WindowExit(pFrmSola);  
    	SendSysMsg_ThreadCreate(pWnd);
        break;
	case BACK_DOWN: 
        if (pSolaEscapeTo)
        {
            pSolaEscapeTo(&pWnd);
        }
        else
        {
    		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                            FrmMainInit, FrmMainExit, 
	                            FrmMainPaint, FrmMainLoop, 
					            FrmMainPause, FrmMainResume,
	                            NULL);
        }    	    
        SendWndMsg_WindowExit(pFrmSola);  
    	SendSysMsg_ThreadCreate(pWnd); 
        break;
	default:
		break;
	}
}


//当SolaWidget的每个事件图标被选中的时候此函数被调用
//此函数是一个回调函数
static int SolaWidgetItemCheckedCall(int index)
{
    //获取对应事件的指针
    pSolaEventItemTable->setEventItem(pSolaEventItemTable, &currSolaEvents, index);
    
    //注意:此处绘制完表格之后无需刷新帧缓冲
    
    return 0;
}

static int SolaTabSolaSetting_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaTabSolaSetting_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = CreateSolaSettingWindow();
    SendWndMsg_WindowExit(pFrmSola);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}

static int SolaTypeChangeNext_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    if (CurrSola_IsRunning())
        return -1;
    if(pSolaEventItemTable->eventSelectedIndex == 0)
        return -1;
    
    pSolaEventItemTable->setEventItemType(pSolaEventItemTable, &currSolaEvents,
                                        pSolaEventItemTable->eventSelectedIndex, 1);//0
    pSolaEventItemTable->setEventItem(pSolaEventItemTable, &currSolaEvents, 
                                        pSolaEventItemTable->eventSelectedIndex);
    SolaWidgetOnEventTypeChanged(pSolaWidget);
    pSolaPassTable->setIsPassed(pSolaPassTable, &currSolaEvents);

    SyncCurrFbmap();
    
    return 0;
}

static int SolaTypeChangePre_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    if (CurrSola_IsRunning())
        return -1;
    if(pSolaEventItemTable->eventSelectedIndex == 0)
        return -1;

    pSolaEventItemTable->setEventItemType(pSolaEventItemTable, &currSolaEvents,
                                        pSolaEventItemTable->eventSelectedIndex, -1);//1
    pSolaEventItemTable->setEventItem(pSolaEventItemTable, &currSolaEvents, 
                                        pSolaEventItemTable->eventSelectedIndex);
    SolaWidgetOnEventTypeChanged(pSolaWidget);
    pSolaPassTable->setIsPassed(pSolaPassTable, &currSolaEvents);

    SyncCurrFbmap();
    
    return 0;
}

static int cheakName()
{
    char *pDirPath = pOtdrTopSettings->sDefSavePath.cSolaSavePath;
    
    if(!currSolaEvents.absolutePath)
    {
        char nameTemp[512];
    	int iRet = SolaNamed(pDirPath, nameTemp, sizeof(nameTemp), &pSolaMessager->autoFilename);
        
    	if(!iRet)
    	{
            return -1;
    	}
        
        char pathAndName[512] = {0};
        sprintf(pathAndName, "%s/%s", pDirPath, nameTemp);
    	setSolaName(&currSolaEvents, pathAndName);
	}
	return 0;
}

//参数可以为空，为空时保存到默认路径，否则保存到指定路径
int WriteSola(char *absolutePath, int checkFile)
{
    int iRet = 0;
    if(cheakName())
    {
        return -2;
    }
	
	if(absolutePath)
	{    
	    if (!checkFile || CanWriteFile(absolutePath))
        {
            iRet = SolaSave(&currSolaEvents, absolutePath);
        }
        else
        {
            iRet = -1;
        }
    }
    else
    {
	    if (!checkFile || CanWriteFile(currSolaEvents.absolutePath))
        {
            isExist(pOtdrTopSettings->sDefSavePath.cSolaSavePath);
            iRet = SolaSave(&currSolaEvents, currSolaEvents.absolutePath);
        }
        else
        {
            iRet = -1;
        }
    }
    return iRet;
}

static void DisplaySaveResultLabel(int result, char* path)
{    
    if(pSolaStrSaveResult)
        GuiMemFree(pSolaStrSaveResult);
        
    if(pSolaStrSavePath)
        GuiMemFree(pSolaStrSavePath);

    if(NULL == path)
    {
        pSolaStrSaveResult = TransString("");
        pSolaStrSavePath = TransString("");
    }
    else
    {
        if((result == -3) || (result == -1))
        {
            pSolaStrSaveResult = TransString("");
            pSolaStrSavePath = TransString("");
        }
        else if(result == 0)
        {
            char *fileName = path+5;
            pSolaStrSaveResult = TransString("SOLA_SAVE_SUCCESSFUL");
            pSolaStrSavePath = TransString(fileName);
        }
        else
        {
            pSolaStrSaveResult = TransString("SOLA_SAVE_FAILED");
            pSolaStrSavePath = TransString("");
        }
    }
    
    DisplayPicture(pSolaBottumBg);

    SetLabelText(pSolaStrSaveResult, pSolaLblSaveResult);
    SetLabelText(pSolaStrSavePath, pSolaLblSavePath);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaLblSaveResult);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaLblSavePath);
    DisplayLabel(pSolaLblSaveResult);
    DisplayLabel(pSolaLblSavePath);
}

static void setSolaWidgetsWaveLength(OPM_WAVE enSolaWaveChoose)
{
    int wavelen;
    if(enSolaWaveChoose == ENUM_WAVE_1310NM)
        wavelen = 1310;
    else
        wavelen = 1550;
    
    pSolaLineLossTable->setWaveLen(pSolaLineLossTable, wavelen);
    pSolaEventItemTable->setWaveLen(pSolaEventItemTable, wavelen);
}


static void setSolaWidgetsAfterEventsPrepared(SolaEvents* events)
{
    if(NULL == events)
        return;
    
    SetSolaWidgetEvent(pSolaWidget, events);
    pSolaEventItemTable->setEventItem(pSolaEventItemTable, events, 
        pSolaEventItemTable->eventSelectedIndex);
    pSolaLineLossTable->setTotalLoss(pSolaLineLossTable, events);
    pSolaPassTable->setIsPassed(pSolaPassTable, events);
}

//更新进度条
static void UpdateProgress(int iProgress)
{
    ProgressBar_SetValue(pProgressBar, iProgress);
    ProgressBar_Display(pProgressBar);
    SyncCurrFbmap();
}

//更新波长显示
static void UpdateWave(int iWave)
{
    static const char* wave[] = {"1310nm", "1550nm", "1625nm"};
    GuiMemFree(pSolaStrWave);
    pSolaStrWave = TransString((char*)wave[iWave]);
    SetLabelText(pSolaStrWave, pSolaLblWave);
    DisplayPicture(pSolaWaveBg);
    DisplayLabel(pSolaLblWave);
    SyncCurrFbmap();
}

void clearSolaEvents()
{
    SolaEvents_Clear(&currSolaEvents);
    if(pSolaFileName)//点击otdr按钮时，文件名未实例化，容易死机
        memset(pSolaFileName, 0, 256);
}

//用于对话框重新还原窗体时的回调函数
void FrmSolaReCreateWindow(GUIWINDOW **pWnd)
{	
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmSolaInit, FrmSolaExit, 
						FrmSolaPaint, FrmSolaLoop, 
						FrmSolaPause, FrmSolaResume,
						NULL);
}

SOLA_WIDGET* getSolaWidget()
{
    return pSolaWidget;
}

SOLA_TABLE_EVENT_ITEM* getSolaTableEventItem()
{
    return pSolaEventItemTable;
}

SOLA_TABLE_PASS* getSolaTablePass()
{
    return pSolaPassTable;
}

SOLA_TABLE_LINE* getSolaTableLine()
{
    return pSolaLineLossTable;
}

SolaEvents *getCurrSolaEvents()
{
    if(SolaEvents_IsVaild(&currSolaEvents))
    {
        return &currSolaEvents;
    }

    return NULL;
}

//获得当前曲线测量时间 成功返回0. 失败返回非0
int getSolaMeasureDate(char *date, char *time, int dateFormat)
{
    if(!date && !time)
    {
        return -1;
    }

    if(currSolaEvents.WaveIsSetted[WAVELEN_1310] == 0
        && currSolaEvents.WaveIsSetted[WAVELEN_1550] == 0
        && currSolaEvents.WaveIsSetted[WAVELEN_1625] == 0)
    {
        return -2;
    }
    
	struct tm *newtime;
	
	newtime = localtime(&(currSolaEvents.ulDTS));
	
	if(date)
	{
    	if(dateFormat == 0)
        {
    	    strftime(date, 128, "%d/%m/%Y", newtime);
        }
        else if(dateFormat == 1)
        {
            strftime(date, 128, "%m/%d/%Y", newtime);
        }
        else
        {
            strftime(date, 128, "%Y/%m/%d", newtime);
        }
	}

	if(time)
	{
	    strftime(time, 128, "%T", newtime);
	}

	return 0;
}

//设置提示框是否显示
void setReminderDialogFlag(int iFlag)
{
    iSetSpliceRatioPrometFlag = iFlag;
}
