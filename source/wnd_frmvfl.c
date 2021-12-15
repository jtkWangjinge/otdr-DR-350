/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmvfl.c
* 摘    要：  实现主窗体FrmVFL的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/25 
*
*******************************************************************************/

#include "wnd_frmvfl.h"

#include "guipicture.h"
#include "guiwindow.h"
#include "guibase.h"
#include "guilabel.h"
#include "guimessage.h"

#include "app_frmvls.h"

#include "wnd_global.h"
#include "wnd_frmdialog.h"
#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"

/*********************************背景控件定义*********************************/
static GUIPICTURE* pFrmVFLBg = NULL;                    //背景

/*******************************************************************************
**							窗体FrmVFL中的控件定义部分						 **
*******************************************************************************/
static GUIWINDOW *pFrmVFL = NULL;

/*******************************************************************************
**							窗体FrmVFL 标题栏中的控件定义部分				**
*******************************************************************************/
static GUICHAR* pFrmVFLStrTitle = NULL;                 //VFL
static GUILABEL* pFrmVFLLblTitle = NULL;
static GUICHAR* pFrmVFLStrWarning = NULL;                
static GUILABEL* pFrmVFLLblWarning = NULL;				//警告标语
/*******************************************************************************
**							窗体FrmVFL 功能区中的控件定义部分				**
*******************************************************************************/
static GUIPICTURE* pFrmVFLBgIcon = NULL;				//功能图标
static GUIPICTURE* pFrmVFLBgStatus = NULL;              //VFL状态图片

/*******************************************************************************
*               窗体FrmVFL右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFrmVFLMenu = NULL;
#define VFL_MODE_NUM        3

//各项非选中背景资源
static char *pVFLUnpressBmp[VFL_MODE_NUM] =
{
    BmpFileDirectory "bg_vfl_cw.bmp",
    BmpFileDirectory "bg_vfl_flash.bmp",
    BmpFileDirectory "bg_vfl_close.bmp"
};

//各项选中背景资源
static char *pVFLSelectBmp[VFL_MODE_NUM] =
{
    BmpFileDirectory "bg_vfl_cwSelect.bmp",
    BmpFileDirectory "bg_vfl_flashSelect.bmp",
    BmpFileDirectory "bg_vfl_closeSelect.bmp"
};

/*******************************************************************************
**							VFL 状态声明					                  **
*******************************************************************************/
static VLS_MODE gVflMode;
/*******************************************************************************
*                   窗体FrmVFL内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int VFLTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int VFLTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体FrmVFL内部按钮响应函数声明
*******************************************************************************/
static void VFLMenuCallBack(int option);


/***
  * 功能：
        窗体frmvfl的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmVflInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    //得到当前窗体对象 
    pFrmVFL = (GUIWINDOW *) pWndObj;
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    VFLTextRes_Init(NULL, 0, NULL, 0);
    //初始化VFL状态标志位
    gVflMode = CLOSE;

    pFrmVFLBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_vfl.bmp");
	pFrmVFLBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_vfl_icon.bmp");
    pFrmVFLBgStatus = CreatePicture(27, 86, 478, 226, BmpFileDirectory"bg_vfl_status_disable.bmp");

    pFrmVFLLblTitle = CreateLabel(40, 12, 100, 16, pFrmVFLStrTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmVFLLblTitle);
    pFrmVFLLblWarning = CreateLabel(252, 415, 200, 16, pFrmVFLStrWarning);
    SetLabelFont(getGlobalFnt(EN_FONT_RED), pFrmVFLLblWarning);
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
    unsigned int strVflMenu[VFL_MODE_NUM] = { VFL_LBL_CW, VFL_LBL_2HZ, VFL_LBL_CLOSE };
    pFrmVFLMenu = CreateWndMenu1(VFL_MODE_NUM, sizeof(strVflMenu), strVflMenu,  0xff00,
								0, 1, 41, VFLMenuCallBack);
    //设置菜单栏
    int i;
    for (i = 0; i < VFL_MODE_NUM; i++)
    {
        SetWndMenuItemBg(i, pVFLUnpressBmp[i], pFrmVFLMenu, MENU_UNPRESS);
        SetWndMenuItemBg(i, pVFLSelectBmp[i], pFrmVFLMenu, MENU_SELECT);
    }
    
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmVFL, pFrmVFL);
    
    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFrmVFLMenu, pFrmVFL);

    /***************************************************************************
    *                      注册菜单区控件的消息处理
    ***************************************************************************/
	LoginWndMenuToMsg1(pFrmVFLMenu, pFrmVFL);
    
    return iRet;
}

/***
  * 功能：
        窗体frmvfl的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmVflExit(void *pWndObj)
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
    pFrmVFL = (GUIWINDOW *) pWndObj;
    
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmVFL);

    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFrmVFLBg);
	DestroyPicture(&pFrmVFLBgIcon);
    DestroyPicture(&pFrmVFLBgStatus);

    DestroyLabel(&pFrmVFLLblTitle);
	DestroyLabel(&pFrmVFLLblWarning);
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFrmVFLMenu);
    
    //释放文本资源
    VFLTextRes_Exit(NULL, 0, NULL, 0);
#ifdef OPTIC_DEVICE
    //关闭CW与2Hz
    CreateCW(0);
    Create2Hz(0);
#endif
    gVflMode = CLOSE;

    return iRet;
}

/***
  * 功能：
        窗体frmvls的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVflPaint(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    /***************************************************************************
    *                      显示桌面上各个区域的背景控件
    ***************************************************************************/
	DisplayPicture(pFrmVFLBg);
	// DisplayPicture(pFrmVFLBgIcon);
    DisplayPicture(pFrmVFLBgStatus);
    
	DisplayLabel(pFrmVFLLblTitle);
	DisplayLabel(pFrmVFLLblWarning);
        
	DisplayWndMenu1(pFrmVFLMenu);
    //设置菜单栏
    int i;
    for (i = 0; i < VFL_MODE_NUM; i++)
    {
        SetWndMenuItemEnble(i, 1, pFrmVFLMenu);
    }

    SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
      
    return iRet;
}

/***
  * 功能：
        窗体frmvfl的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVflLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;
    //记录进入该函数的次数
    static int count = 0;

    if(gVflMode == HZ_2)
    {
        if(count < 5)
        {
            SetPictureBitmap(BmpFileDirectory"bg_vfl_status_unpress.bmp", pFrmVFLBgStatus);
            count++;
        }
        else if(count < 10)
        {
            SetPictureBitmap(BmpFileDirectory"bg_vfl_status_press.bmp", pFrmVFLBgStatus);
            count++;
        }
        else
        {
            count = 0;
        }
    }
    else
    {
        count = 0;
    }

    //只有在打开状态下（cw或2hz才刷新界面）
    if (gVflMode)
    {
        DisplayPicture(pFrmVFLBgStatus);
        RefreshScreen(__FILE__, __func__, __LINE__);
    }
    
    return iRet;
}

/***
  * 功能：
        窗体frmvfl的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVflPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmvfl的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVflResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

//初始化文本资源
static int VFLTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iRet = 0;

	pFrmVFLStrTitle = GetCurrLanguageText(MAIN_LBL_VFL);
	pFrmVFLStrWarning = GetCurrLanguageText(VFL_LBL_WARNING);

    return iRet;
}

//释放文本资源
static int VFLTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iRet = 0;
    
    GuiMemFree(pFrmVFLStrTitle);
	GuiMemFree(pFrmVFLStrWarning);

    return iRet;
}

static void VFLMenuCallBack(int option)
{
    switch(option)
    {
        case CW:
#ifdef OPTIC_DEVICE
            Create2Hz(0);//关闭2Hz
            CreateCW(1);//打开CW
#endif
            gVflMode = CW;//设置模式为CW
            TouchChange("bg_vfl_status_press.bmp", pFrmVFLBgStatus, NULL, NULL, 0);
            break;
        case HZ_2:
#ifdef OPTIC_DEVICE
            CreateCW(0);//关闭CW
            Create2Hz(1);//打开2Hz
#endif
            gVflMode = HZ_2;//设置模式为2HZ 
            TouchChange("bg_vfl_status_unpress.bmp", pFrmVFLBgStatus, NULL, NULL, 0);
            break;
        case CLOSE:
        if((gVflMode == CW) || (gVflMode == HZ_2) )
        {
#ifdef OPTIC_DEVICE
            //关闭CW或2HZ
            CreateCW(0);
            Create2Hz(0);
#endif
            gVflMode= CLOSE;
            TouchChange("bg_vfl_status_disable.bmp", pFrmVFLBgStatus, NULL, NULL, 0);
        }
        break;
        case BACK_DOWN:
		case HOME_DOWN:
#ifdef OPTIC_DEVICE
            Create2Hz(0);//关闭2Hz
			CreateCW(0);//关闭CW
#endif
			gVflMode = CLOSE; //设置模式为关闭
			ReturnMenuOK();
		default:
            break;
    }

    RefreshScreen(__FILE__, __func__, __LINE__);
        
}