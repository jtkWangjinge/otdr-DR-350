/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmuserguide.c
* 摘    要：  实现主窗体FrmUserGuide的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2021/6/23 
*
*******************************************************************************/

#include "wnd_frmuserguide.h"

#include "guiwindow.h"
#include "guipicture.h"
#include "guilabel.h"
#include "guimessage.h"

#include "wnd_global.h"
#include "wnd_frmsystemsetting.h"
#include "wnd_frmimageviewer.h"


/*******************************************************************************
**							窗体FrmUserGuide中的控件定义部分		            **
*******************************************************************************/
static GUIWINDOW *pFrmUserGuide = NULL;


static GUIPICTURE *pFrmUserGuideBg = NULL; //背景
static GUIPICTURE *pFrmUserGuideBgIcon = NULL;

static GUILABEL *pFrmUserGuideLblTitle = NULL;
static GUILABEL *pFrmUserGuideLblInfo = NULL;

static GUICHAR *pFrmUserGuideStrTitle = NULL;     //UserGuide
static GUICHAR *pFrmUserGuideStrInfo = NULL;      //Info

//目录选项
typedef struct user_guide_control
{
    GUILABEL* pLblTitle;
    GUICHAR* pStrTitle;
    GUIPICTURE* pBgIcon;
    GUIPICTURE* pBgEnterIcon;
    GUIPICTURE* pBgSelected;
}USER_GUIDE_CONTROL;

enum options
{
    OTDR_OPTION=0,
    VFL_OPTION,
    OPM_OPTION,
    OPL_OPTION,
    FIP_OPTION,
    RJ45_OPTION,
    FILE_MANAGER_OPTION,
    SYSTEM_SET_OPTION,
    OPTION_COUNT
};

//图标背景资源
static char *pBtnIconBmp[OPTION_COUNT] = {
    BmpFileDirectory "bg_userGuide_otdr_unpress.bmp",
    BmpFileDirectory "bg_userGuide_vfl_unpress.bmp",
    BmpFileDirectory "bg_userGuide_opm_unpress.bmp",
    BmpFileDirectory "bg_userGuide_opl_unpress.bmp",
    BmpFileDirectory "bg_userGuide_fip_unpress.bmp",
    BmpFileDirectory "bg_userGuide_rj45_unpress.bmp",
    BmpFileDirectory "bg_userGuide_file_unpress.bmp",
    BmpFileDirectory "bg_userGuide_sys_unpress.bmp",
};

static char *pBtnIconBmpf[OPTION_COUNT] = {
    BmpFileDirectory "bg_userGuide_otdr_press.bmp",
    BmpFileDirectory "bg_userGuide_vfl_press.bmp",
    BmpFileDirectory "bg_userGuide_opm_press.bmp",
    BmpFileDirectory "bg_userGuide_opl_press.bmp",
    BmpFileDirectory "bg_userGuide_fip_press.bmp",
    BmpFileDirectory "bg_userGuide_rj45_press.bmp",
    BmpFileDirectory "bg_userGuide_file_press.bmp",
    BmpFileDirectory "bg_userGuide_sys_press.bmp",
};

//声明目录选项
static USER_GUIDE_CONTROL *pUserGuideControl[OPTION_COUNT] = {NULL};
static int iSelectedOption = 0;//默认选中第一项
static unsigned int option[OPTION_COUNT] =
{
    MAIN_LBL_OTDR, MAIN_LBL_VFL, MAIN_LBL_OPM, MAIN_LBL_LIGHT_SOURCE,
    MAIN_LBL_FIP, MAIN_LBL_RJ45, MAIN_LBL_FILE, MAIN_LBL_SYSTEM_SET
};
static void InitialOptions(void);//初始化目录结构体
static void ClearOptions(void);//销毁目录结构体
static void RefreshOption(void);//刷新目录选项
/*******************************************************************************
*                   窗体FrmUserGuide内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int UserGuideTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int UserGuideTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//按键响应处理
static int WndUserGuideKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndUserGuideKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
/***
  * 功能：
		用于对话框重新还原窗体时的回调函数
  * 参数：
		1.GUIWINDOW **pWnd	:	指向窗体的指针
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static void ReCreateWindow(GUIWINDOW **pWnd);


/***
  * 功能：
        窗体frmuserguide的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/
int FrmUserGuideInit(void *pWndObj)
{
    //错误标志，返回值定义
    int iRet = 0;
    //得到当前窗体对象
    pFrmUserGuide = (GUIWINDOW *)pWndObj;
    //初始化目录结构体
    InitialOptions();
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    UserGuideTextRes_Init(NULL, 0, NULL, 0);

    pFrmUserGuideBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_userguide.bmp");
    pFrmUserGuideBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_sys_icon.bmp");

    pFrmUserGuideLblTitle = CreateLabel(36, 12, 100, 16, pFrmUserGuideStrTitle);
    pFrmUserGuideLblInfo = CreateLabel(8, 53, 200, 16, pFrmUserGuideStrInfo);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmUserGuideLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmUserGuideLblInfo);

    //目录选项
    int i;
    for(i = 0; i < OPTION_COUNT; ++i)
    {
        pUserGuideControl[i]->pLblTitle = CreateLabel(75, 89 + i * 45, 150, 16,
                                                      pUserGuideControl[i]->pStrTitle);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pUserGuideControl[i]->pLblTitle);
        pUserGuideControl[i]->pBgIcon = CreatePicture(45, 87 + i * 45, 20, 20, pBtnIconBmp[i]);
        pUserGuideControl[i]->pBgEnterIcon = CreatePicture(604, 89 + i * 45, 16, 16,
                                                      BmpFileDirectory "bg_sys_arrow.bmp");
        pUserGuideControl[i]->pBgSelected = CreatePicture(1, 75 + i * 45, 637, 44,
                                                          BmpFileDirectory "bg_sys_unselect.bmp");
    }
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW),
                  pFrmUserGuide, pFrmUserGuide);
    //注册目录选项
    for(i = 0; i < OPTION_COUNT; ++i)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                      pUserGuideControl[i]->pBgSelected, pFrmUserGuide);
    }
    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    GUIMESSAGE *pMsg = GetCurrMessage();
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmUserGuide,
                    WndUserGuideKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmUserGuide,
                    WndUserGuideKey_Up, NULL, 0, pMsg);

    return iRet;
}

/***
  * 功能：
        窗体frmuserguide的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/
int FrmUserGuideExit(void *pWndObj)
{
    //错误标志，返回值定义
    int iRet = 0;
    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmUserGuide = (GUIWINDOW *)pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmUserGuide);

    DestroyPicture(&pFrmUserGuideBg);
    DestroyPicture(&pFrmUserGuideBgIcon);

    DestroyLabel(&pFrmUserGuideLblTitle);
    DestroyLabel(&pFrmUserGuideLblInfo);

    UserGuideTextRes_Exit(NULL, 0, NULL, 0);

    //销毁目录选项
    int i;
    for (i = 0; i < OPTION_COUNT; ++i)
    {
        DestroyPicture(&(pUserGuideControl[i]->pBgIcon));
        DestroyPicture(&(pUserGuideControl[i]->pBgEnterIcon));
        DestroyPicture(&(pUserGuideControl[i]->pBgSelected));
        DestroyLabel(&(pUserGuideControl[i]->pLblTitle));
    }
    //销毁目录结构体
    ClearOptions();

    return iRet;
}

/***
  * 功能：
        窗体frmuserguide的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuidePaint(void *pWndObj)
{
    //错误标志，返回值定义
    int iRet = 0;

    DisplayPicture(pFrmUserGuideBg);
    // DisplayPicture(pFrmUserGuideBgIcon);

    DisplayLabel(pFrmUserGuideLblTitle);
    DisplayLabel(pFrmUserGuideLblInfo);

    //显示目录选项
    RefreshOption();
    SetPowerEnable(1, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}

/***
  * 功能：
        窗体frmuserguide的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuideLoop(void *pWndObj)
{
    //错误标志，返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmuserguide的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuidePause(void *pWndObj)
{
    //错误标志，返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmuserguide的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuideResume(void *pWndObj)
{
    //错误标志，返回值定义
    int iRet = 0;

    return iRet;
}

/*******************************************************************************
*                   窗体FrmUserGuide内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int UserGuideTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义
    int iRet = 0;

    pFrmUserGuideStrTitle = GetCurrLanguageText(SYSTEM_LBL_USER_GUIDE);
    pFrmUserGuideStrInfo = GetCurrLanguageText(USER_GUIDE_LBL_INFO);

    int i;
    
    for(i = 0; i < OPTION_COUNT; ++i)
    {
        pUserGuideControl[i]->pStrTitle = GetCurrLanguageText(option[i]);
    }

    return iRet;
}

//释放文本资源
static int UserGuideTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义
    int iRet = 0;

    GuiMemFree(pFrmUserGuideStrTitle);
    GuiMemFree(pFrmUserGuideStrInfo);

    int i;
    for(i = 0; i < OPTION_COUNT; ++i)
    {
        GuiMemFree(pUserGuideControl[i]->pStrTitle);
    }

    return iRet;
}

//按键响应处理
static int WndUserGuideKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义
    int iRet = 0;

    return iRet;
}

static int WndUserGuideKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义
    int iRet = 0;

    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;

    switch (uiValue)
	{
        case KEYCODE_UP:
        {
            if(iSelectedOption != 0)
                iSelectedOption--;
            //刷新界面
            RefreshOption();
        }
            break;
        case KEYCODE_DOWN:
        {
            if (iSelectedOption != (OPTION_COUNT-1))
                iSelectedOption++;
            //刷新界面
            RefreshOption();
        }
            break;
        case KEYCODE_ENTER:
        //打开相应目录的文件夹图片
        {
            //获取文件名称
            char fileName[128] = {0};
            memset(fileName, 0, 128);
            char *pOption[OPTION_COUNT] ={
            "OTDR", "VFL", "OPM", "OPL",
            "FIP", "RJ45", "FILE MANAGER", "SYSTEM"
            };
            //目前支持中英文，可添加其他语言文件夹
            char *pLanguage[2] ={
                "CN","EN"
            };
            int index = GetCurrLanguageSet();
            if (index == LANG_CHINASIM)
	        {
		        index = 0;
	        }
            else //其他语言统一先使用英语
            {
                index = 1;
            }
            sprintf(fileName, BmpFileDirectory "%s/%s/%s_0.bmp",
                    pOption[iSelectedOption], pLanguage[index], pOption[iSelectedOption]);
            ImageViewer(fileName, pFrmUserGuide, ReCreateWindow);
            SetImageViewerSort(GetCurSortMethod());
        }
            break;
        case KEYCODE_ESC:
        {
            GUIWINDOW *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                           FrmSystemSettingInit, FrmSystemSettingExit,
                                           FrmSystemSettingPaint, FrmSystemSettingLoop,
                                           FrmSystemSettingPause, FrmSystemSettingResume,
                                           NULL);
            SendWndMsg_WindowExit(GetCurrWindow());
            SendSysMsg_ThreadCreate(pWnd);
        }
            break;
        case KEYCODE_VFL:
            JumpVflWindow();
            break;
        default:
            break;
    }

    RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}

//初始化目录结构体
static void InitialOptions(void)
{
    int i;
    for(i = 0; i < OPTION_COUNT; ++i)
    {
        if(!pUserGuideControl[i])
            //分配资源
            pUserGuideControl[i] = (USER_GUIDE_CONTROL *)calloc(1, sizeof(USER_GUIDE_CONTROL));
    }
}

//销毁目录结构体
static void ClearOptions(void)
{
    int i;
    for (i = 0; i < OPTION_COUNT; ++i)
    {
        if (pUserGuideControl[i])
            //销毁资源
            GuiMemFree(pUserGuideControl[i]);
    }
}

//刷新目录选项
static void RefreshOption(void)
{
    int i;
    for (i = 0; i < OPTION_COUNT; ++i)
    {
        SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp",
                         pUserGuideControl[i]->pBgSelected);
        SetPictureBitmap(pBtnIconBmp[i], pUserGuideControl[i]->pBgIcon);
        SetPictureBitmap(BmpFileDirectory "bg_sys_arrow.bmp",
                         pUserGuideControl[i]->pBgEnterIcon);
        //设置选中项
        if (iSelectedOption == i)
        {
            SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
                             pUserGuideControl[i]->pBgSelected);
            SetPictureBitmap(pBtnIconBmpf[i], pUserGuideControl[i]->pBgIcon);
            SetPictureBitmap(BmpFileDirectory "bg_sys_arrow_press.bmp",
                             pUserGuideControl[i]->pBgEnterIcon);
        }

        DisplayPicture(pUserGuideControl[i]->pBgSelected);
        DisplayPicture(pUserGuideControl[i]->pBgIcon);
        DisplayPicture(pUserGuideControl[i]->pBgEnterIcon);
        DisplayLabel(pUserGuideControl[i]->pLblTitle);
    }
}

/***
  * 功能：
		用于对话框重新还原窗体时的回调函数
  * 参数：
		1.GUIWINDOW **pWnd	:	指向窗体的指针
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
static void ReCreateWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                         FrmUserGuideInit, FrmUserGuideExit,
                         FrmUserGuidePaint, FrmUserGuideLoop,
                         FrmUserGuidePause, FrmUserGuideResume,
                         NULL);
}