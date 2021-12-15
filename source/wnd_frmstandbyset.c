/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmstandbyset.c
* 摘    要：  实现主窗体FrmStandbyset的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


#include "wnd_frmstandbyset.h"

/*******************************************************************************
**                为实现窗体FrmStandbyset而需要引用的其他头文件	              **
*******************************************************************************/

#include "app_global.h"
#include "app_getsetparameter.h"
#include "app_systemsettings.h"
#include "app_frmotdr.h"
#include "app_frmbrightset.h"
#include "app_frmbatteryset.h"

#include "guiphoto.h"

#include "wnd_frmmain.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmtimeset.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmabout.h"
#include "wnd_frmwifi.h"
#include "wnd_frmskinset.h"
#include "wnd_frmdialog.h"
#include "wnd_frminputpassward.h"
#include "wnd_global.h"
#include "wnd_frmfactory.h"
#include "wnd_frmbmpfilenamed.h"


/*******************************************************************************
**                 定义wnd_FrmStandbyset.c中内部使用的宏                      **
*******************************************************************************/
#define CURSOR_X (47)  //游标移动的距离
#define POINT_X (108)  //待机时间和自动关机横坐标之间的距离
#define POINT_Y (35)   //每一个自动关机时间纵坐标之间的距离

//引用外部全局变量
extern PSYSTEMSET pCurSystemSet;
// unsigned char GucKeyDownFlg;
// unsigned char GucScreenOffFlg;
// unsigned char GucPowerOffFlg;
// unsigned char GucExecFlg;

static unsigned int  uiFactoryCode = 0;         //进入工厂菜单


extern POTDR_TOP_SETTINGS pOtdrTopSettings;
/*******************************************************************************
**               定义wnd_FrmStandbyset.c中内部使用的变量                      **
*******************************************************************************/
//电池和适配器八个标签的文字
static char *standby[9] = {
"10s",
"30s",
"1m",
"5m",
"off",
"1m",
"5m",
"10m",
"off",
};

//选择，亮度游标，电池选择，适配器选择，声音选择
static int iCursorOption[9] = {0,1,2,3,4,5,6,7,8};
static int iBatteryOption[9] = {0,1,2,3,4,5,6,7,8};
static int iAdapterOption[9] = {0,1,2,3,4,5,6,7,8};
static int iSoundOption[3] = {0,1,2};

//声音打开标志
static int iSound1Flag = 0;     //声音1打开标志
static int iSound2Flag = 0;		//声音2打开标志

//标志位用于设置亮度游标的位置
static int iAdapterFlag = 0;	//适配器的亮度游标标志
static int iBatteryFlag = 0;	//电池的亮度游标标志

// static unsigned int  GuiScreenTimeCnt = 0;		//屏幕自动关闭计时
// static unsigned int  GuiPowerTimeCnt  = 0;		//自动关机计时

/*******************************************************************************
**               窗体FrmStandbyset中的窗体控件定义部分	                      **
*******************************************************************************/
static GUIWINDOW *pFrmStandbyset = NULL;

/********************************背景控件定义**********************************/
static GUIPICTURE *pStandbysetBgLeft = NULL;
static GUIPICTURE *pStandbysetBgTabTitle = NULL;

/********************************右侧菜单控件定义******************************/
//菜单栏控件
static WNDMENU1 *pStandbysetMenu = NULL;


/********************************标题控件定义******************************/

static GUIPICTURE *pStandbysetBtnChange1 = NULL;

/********************************模块功能控件定义******************************/
static GUIPICTURE *pStandbysetCursor[9] = {};		//游标九个亮度
static GUIPICTURE *pStandbysetCursorMask = NULL;		//游标九个亮度
static GUIPICTURE *pStandbysetCursorBar[9] = {};		//游标九个亮度

static GUIPICTURE *pStandbysetBattery[9] = {};		//电池八个点
static GUIPICTURE *pStandbysetAdapter[9] = {};		//适配器八个点
static GUIPICTURE *pStandbysetSound[3] = {};		//声音3个点


//左上角文本
static GUICHAR *pStandbysetStrTitle = NULL;       	//待机和亮度
static GUICHAR *pStandbysetStrBmpTitle = NULL;     //皮肤设置
static GUICHAR *pStandbysetStrBackgroundBrightness = NULL;       //背景亮度
//左上角标签
static GUILABEL *pStandbysetLblTitle = NULL;       //待机和亮度
static GUILABEL *pStandbysetLblBmpTitle = NULL;   //皮肤设置
static GUIPICTURE *pStandbysetBtnChange2 = NULL;
static GUILABEL *pStandbysetLblBackgroundBrightness = NULL;       //背景亮度
//电池的文字
static GUICHAR *pStandbysetStrBattery = NULL;    //电池
static GUICHAR *pStandbysetStrBStandbyTime = NULL;    //电池的待机时间
static GUICHAR *pStandbysetStrBAutoOff = NULL;    //电池的自动关机
static GUICHAR *pStandbysetStrBStandby[9] = {};    //电池的八个点

//电池的标签
static GUILABEL *pStandbysetLblBattery = NULL;    //电池
static GUILABEL *pStandbysetLblBStandbyTime = NULL;    //电池的待机时间
static GUILABEL *pStandbysetLblBAutoOff = NULL;    //电池的自动关机
static GUILABEL *pStandbysetLblBStandby[9] = {};    //电池的八个点

//适配器的文本
static GUICHAR *pStandbysetStrAdapter = NULL;    //适配器
static GUICHAR *pStandbysetStrAStandbyTime = NULL;    //适配器的待机时间
static GUICHAR *pStandbysetStrAAutoOff = NULL;    //适配器的自动关机
static GUICHAR *pStandbysetStrAStandby[9] = {};    //适配器的八个点

//适配器的标签
static GUILABEL *pStandbysetLblAdapter = NULL;    //适配器
static GUILABEL *pStandbysetLblAStandbyTime = NULL;    //适配器的待机时间
static GUILABEL *pStandbysetLblAAutoOff = NULL;    //适配器的自动关机
static GUILABEL *pStandbysetLblAStandby[9] = {};    //适配器的八个点

//提示音的文本
static GUICHAR *pStandbysetStrSound = NULL;    //提示音
static GUICHAR *pStandbysetStrSSound[3] = {};    //提示音的3个点

//提示音的标签
static GUILABEL *pStandbysetLblSound = NULL;    //提示音
static GUILABEL *pStandbysetLblSSound[3] = {};    //提示音的3个点


/*******************************************************************************
**     窗体FrmStandbyset中的初始化文本资源、 释放文本资源函数定义部分         **
*******************************************************************************/
//初始化文本资源
static int StandbysetTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
//释放文本资源
static int StandbysetTextRes_Exit(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

/*******************************************************************************
**                   窗体FrmStandbyset中的控件事件处理函数                    **
*******************************************************************************/
/***************************右边按钮区控件的事件处理函数***********************/
//菜单栏回调函数
static void StandbysetWndMenuCallBack(int selected);

/*******************亮度游标、电池、适配器、声音的处理函数*********************/				
static int StandbysetCursor_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int StandbysetCursor_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int StandbysetBattery_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int StandbysetBattery_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);

static int StandbysetAdapter_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int StandbysetAdapter_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
						   
static int StandbysetSound_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen);
static int StandbysetSound_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen);
/*******************************************************************************
**                窗体FrmStandbyset中的错误事件处理函数                       **
*******************************************************************************/
static int StandbysetErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static int StandbysetBtnChange1_Down(void *pInArg,  int iInLen, 
                                    void *pOutArg, int iOutLen);
//适配器和电池的切换
static int SwitchPowerDisplay();

/***
  * 功能：
        窗体FrmStandbyset的初始化函数，建立窗体控件、注册消息处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmStandbysetInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	iSound1Flag = 0;
	iSound2Flag = 0;

	uiFactoryCode = 0;
    #ifdef MINI2
	unsigned int StrStandbyMenu[] = {
	SYSTEMSET_STANDBY,
	SYSTEMSET_TIME,
	SYSTEMSET_LANGUAGE,
	MAIN_LBL_WIFI,
	SYSTEMSET_SYSMAINTAIN,
	SYSTEM_ABOUT
	};
    #else
    unsigned int StrStandbyMenu[] = {
		1
	};
    #endif
	GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmStandbyset = (GUIWINDOW *) pWndObj;

   	//初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    StandbysetTextRes_Init(NULL, 0, NULL, 0);

	/***********************建立整体桌面背景和右边选项的控件***********************/
    pStandbysetBgLeft = CreatePhoto("standby_left_bg");
    pStandbysetBgTabTitle = CreatePhoto("otdr_top1f");
	//菜单栏控件
	#ifdef MINI2
	pStandbysetMenu = CreateWndMenu1(6, sizeof(StrStandbyMenu), StrStandbyMenu,
									0xffff, 0, 0, 40, StandbysetWndMenuCallBack);
    #else
    pStandbysetMenu = CreateWndMenu1(5, sizeof(StrStandbyMenu), StrStandbyMenu,
									0xffff, 0, 0, 40, StandbysetWndMenuCallBack);
    #endif
	pStandbysetBtnChange1 = CreatePicture(100, 0, 100, 50, NULL);
	pStandbysetBtnChange2 = CreatePicture(200, 0, 100, 50, NULL);
    pStandbysetCursorMask = CreatePhoto("cursor_mask_standby");
	//游标
    CreateLoopPhoto(pStandbysetCursor,"cursor_standby", 0, 0, 9);
	//图片为空的游标点击事件
	for(i = 0; i < 9; i++)
	{
		pStandbysetCursorBar[i] = CreatePicture(223+i*CURSOR_X, 78, 47, 44, NULL);
	}
    
    CreateLoopPhoto(pStandbysetBattery, "standby_point_unselect1", 5, 0, 9);
    
    CreateLoopPhoto(pStandbysetAdapter,"standby_point_unselect2", 5, 0, 9);

    CreateLoopPhoto(pStandbysetSound, "standby_point_unselect3", 0, 0, 3);
	
	//左上角标签
	pStandbysetLblBmpTitle = CreateLabel(100, 24, 100, 24, pStandbysetStrBmpTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pStandbysetLblBmpTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblBmpTitle);
    pStandbysetLblTitle = CreateLabel(0, 24, 100, 24, pStandbysetStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pStandbysetLblTitle);
	pStandbysetLblBackgroundBrightness = CreateLabel(25, 103, 190, 24, 
						     pStandbysetStrBackgroundBrightness);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pStandbysetLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblBackgroundBrightness);
	//电池的所有标签
	pStandbysetLblBattery = CreateLabel(25, 180, 200, 24, 
						     pStandbysetStrBattery);
	pStandbysetLblBStandbyTime = CreateLabel(34, 220, 100, 24, 
						     pStandbysetStrBStandbyTime);
	pStandbysetLblBAutoOff = CreateLabel(150, 220, 100, 24,
						     pStandbysetStrBAutoOff);

	for(i = 0; i < 9; i++)
	{
		if(i < 5)
			pStandbysetLblBStandby[i] = CreateLabel(80, 255+i*POINT_Y, 40, 25, 
				    pStandbysetStrBStandby[i]);
		else
			pStandbysetLblBStandby[i] = CreateLabel(80+POINT_X, 255+(i-5)*POINT_Y, 40, 25,
				    pStandbysetStrBStandby[i]);
	}
	//适配器的所有标签
	pStandbysetLblAdapter = CreateLabel(260, 180, 200, 24, pStandbysetStrAdapter);
	pStandbysetLblAStandbyTime = CreateLabel(267, 220, 100, 24, pStandbysetStrAStandbyTime);
	pStandbysetLblAAutoOff = CreateLabel(384, 220, 100, 24, pStandbysetStrAAutoOff);
	
	for(i = 0; i < 9; i++)
	{
		if(i < 5)
			pStandbysetLblAStandby[i] = CreateLabel(314, 255+i*POINT_Y, 40, 25,
			    pStandbysetStrAStandby[i]);
		else
			pStandbysetLblAStandby[i] = CreateLabel(314+POINT_X, 255+(i-5)*POINT_Y, 40, 25, 
			    pStandbysetStrAStandby[i]);
	}
	//声音的所有标签
	pStandbysetLblSound = CreateLabel(494, 180, 200, 24, pStandbysetStrSound);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblSound);
	for(i = 0; i < 3; i++)
	{
		pStandbysetLblSSound[i] = CreateLabel(546, 260+i*53, 120, 25, 
			    pStandbysetStrSSound[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblSSound[i]);
	}
	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmStandbyset, 
                  pFrmStandbyset);

	//注册桌面右侧系统功能选项的控件
	//注册右侧按钮区的控件
    AddWndMenuToComp1(pStandbysetMenu, pFrmStandbyset);
               			  
	//注册空图片的游标控件
	for(i = 0; i < 9; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pStandbysetCursorBar[i], pFrmStandbyset);
	}
	//电池			  
	for(i = 0; i < 9; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pStandbysetBattery[i], pFrmStandbyset);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pStandbysetLblBStandby[i], pFrmStandbyset);
	}
	//适配器
	for(i = 0; i < 9; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pStandbysetAdapter[i], pFrmStandbyset);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pStandbysetLblAStandby[i], pFrmStandbyset);
	}
	//声音
	for(i = 0; i < 3; i++)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pStandbysetSound[i], pFrmStandbyset);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pStandbysetLblSSound[i], pFrmStandbyset);
	}

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pStandbysetBtnChange1, 
                  pFrmStandbyset); 
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pStandbysetBtnChange2, 
                  pFrmStandbyset); 
	//注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //***************************************************************/
    pMsg = GetCurrMessage();

	//注册桌面右侧五个系统功能选项的消息处理
	//右侧菜单栏控件
	LoginWndMenuToMsg1(pStandbysetMenu, pFrmStandbyset);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetBtnChange1, 
                    StandbysetBtnChange1_Down, NULL, 0, pMsg);

	//游标事件
	for(i = 0; i < 9; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetCursorBar[i], 
                    StandbysetCursor_Down, &iCursorOption[i], sizeof(iCursorOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pStandbysetCursorBar[i], 
                    StandbysetCursor_Up, &iCursorOption[i], sizeof(iCursorOption[i]), pMsg);
	}
	//电池事件
	for(i = 0; i < 9; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetBattery[i], 
                    StandbysetBattery_Down, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pStandbysetBattery[i], 
                    StandbysetBattery_Up, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetLblBStandby[i], 
                    StandbysetBattery_Down, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pStandbysetLblBStandby[i], 
                    StandbysetBattery_Up, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
	}
	//适配器事件
	for(i = 0; i < 9; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetAdapter[i], 
                    StandbysetAdapter_Down, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pStandbysetAdapter[i], 
                    StandbysetAdapter_Up, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetLblAStandby[i], 
                    StandbysetAdapter_Down, &iAdapterOption[i], sizeof(iAdapterOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pStandbysetLblAStandby[i], 
                    StandbysetAdapter_Up, &iAdapterOption[i], sizeof(iAdapterOption[i]), pMsg);
	}
	//声音事件
	for(i = 0; i < 3; i++)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetSound[i], 
                    StandbysetSound_Down, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pStandbysetSound[i], 
                    StandbysetSound_Up, &iBatteryOption[i], sizeof(iBatteryOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pStandbysetLblSSound[i], 
                    StandbysetSound_Down, &iSoundOption[i], sizeof(iSoundOption[i]), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pStandbysetLblSSound[i], 
                    StandbysetSound_Up, &iSoundOption[i], sizeof(iSoundOption[i]), pMsg);
	}
	//注册错误消息处理函数
	LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmStandbyset, 
                    StandbysetErrProc_Func, NULL, 0, pMsg);
    
    return iReturn;
}


/***
  * 功能：
        窗体FrmStandbyset的退出函数，释放所有资源
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmStandbysetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i= 0;
    GUIMESSAGE *pMsg = NULL;
    //得到当前窗体对象
    pFrmStandbyset = (GUIWINDOW *) pWndObj;

	//清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmStandbyset);

/******************销毁桌面框架背景和右边栏的系统功能控件图片******************/
    DestroyPicture(&pStandbysetBgLeft);
    DestroyPicture(&pStandbysetBgTabTitle);
	DestroyPicture(&pStandbysetBtnChange1);
	DestroyPicture(&pStandbysetBtnChange2);
	DestroyPicture(&pStandbysetCursorMask);
	for(i = 0; i < 9; i++)
	{
		DestroyPicture(&pStandbysetCursor[i]);
	}
	for(i = 0; i < 9; i++)
	{
		DestroyPicture(&pStandbysetCursorBar[i]);
	}
	
	for(i = 0; i < 9; i++)
	{
		DestroyPicture(&pStandbysetBattery[i]);
	}
	
	for(i = 0; i < 9; i++)
	{
		DestroyPicture(&pStandbysetAdapter[i]);
	}
	
	for(i = 0; i < 3; i++)
	{
		DestroyPicture(&pStandbysetSound[i]);
	}
	
	//销毁桌面右边栏的系统功能控件的标签
	//菜单区的控件 
	DestroyWndMenu1(&pStandbysetMenu);
	
	DestroyLabel(&pStandbysetLblTitle);
	DestroyLabel(&pStandbysetLblBmpTitle);
	DestroyLabel(&pStandbysetLblBackgroundBrightness);
	//电池
	DestroyLabel(&pStandbysetLblBattery);
	DestroyLabel(&pStandbysetLblBStandbyTime);
	DestroyLabel(&pStandbysetLblBAutoOff);
	for(i = 0; i < 9; i++)
	{
		DestroyLabel(&pStandbysetLblBStandby[i]);
	}
	//适配器
	DestroyLabel(&pStandbysetLblAdapter);
	DestroyLabel(&pStandbysetLblAStandbyTime);
	DestroyLabel(&pStandbysetLblAAutoOff);
	for(i = 0; i < 9; i++)
	{
		DestroyLabel(&pStandbysetLblAStandby[i]);
	}
	//声音
	DestroyLabel(&pStandbysetLblSound);
	for(i = 0; i < 3; i++)
	{
		DestroyLabel(&pStandbysetLblSSound[i]);
	}
/*****************************释放文本资源*************************************/
    StandbysetTextRes_Exit(NULL, 0, NULL, 0);

	// 保存系统设置参数
	SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);

    return iReturn;
}

/***
  * 功能：
        窗体FrmStandbyset的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmStandbysetPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
    //得到当前窗体对象
    pFrmStandbyset = (GUIWINDOW *) pWndObj;
	LOG(LOG_DEBUG,"enter systempain\n");

    //显示状态栏、桌面、信息栏
	//显示桌面背景框架和右边栏的控件
    DisplayPicture(pStandbysetBgLeft);
    DisplayPicture(pStandbysetBgTabTitle);
	//菜单区的控件 
	DisplayWndMenu1(pStandbysetMenu);

	//检查适配器是否插入
	if(CheckPowerOn(ADAPTER))
	{	
		DisplayPicture(pStandbysetCursor[((pCurSystemSet->uiACLCDBright)/10 - 1)]);
	}
	else
	{
		DisplayPicture(pStandbysetCursor[((pCurSystemSet->uiDCLCDBright)/10 - 1)]);
	}

	for(i = 0; i < 3; i++)
	{
		DisplayPicture(pStandbysetSound[i]);
	}
	//检查是否所选声音
	if(pCurSystemSet->ucKeyWarning == 1)
	{
		iSound1Flag = 1;
		SetPictureBitmap(BmpFileDirectory"standby_point_select3.bmp", pStandbysetSound[0]);
		DisplayPicture(pStandbysetSound[0]);
	}
	if(pCurSystemSet->uiWarning == 1)
	{
		iSound2Flag = 1;
		SetPictureBitmap(BmpFileDirectory"standby_point_select3.bmp", pStandbysetSound[1]);
		DisplayPicture(pStandbysetSound[1]);
	}
	if((pCurSystemSet->uiWarning == 0) && (pCurSystemSet->ucKeyWarning == 0))
	{
		iSound1Flag = 0;
		iSound2Flag = 0;
		SetPictureBitmap(BmpFileDirectory"standby_point_select3.bmp", pStandbysetSound[2]);
		DisplayPicture(pStandbysetSound[2]);
	}

	DisplayLabel(pStandbysetLblTitle);
	DisplayLabel(pStandbysetLblBmpTitle);
	DisplayLabel(pStandbysetLblBackgroundBrightness);
	DisplayLabel(pStandbysetLblSound);
    
	for(i = 0; i < 3; i++)
	{
		DisplayLabel(pStandbysetLblSSound[i]);
	}
	
	SwitchPowerDisplay();
	SetPowerEnable(1, 1);
	
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}


/***
  * 功能：
        窗体FrmStandbyset的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmStandbysetLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int iAdapterSta = CheckPowerOn(ADAPTER);
	if((iAdapterSta == 1) && (iAdapterFlag == 0))
	{
		iAdapterFlag = 1;
		iBatteryFlag = 0;
		DisplayPicture(pStandbysetCursorMask);
		TouchChange("cursor_standby.bmp", 
			pStandbysetCursor[((pCurSystemSet->uiACLCDBright)/10 - 1)], NULL, NULL, 0);	
		SwitchPowerDisplay();
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	
	if((iAdapterSta == 0) && (iBatteryFlag == 0))
	{
		iAdapterFlag = 0;
		iBatteryFlag = 1;
		DisplayPicture(pStandbysetCursorMask);
		TouchChange("cursor_standby.bmp",
			pStandbysetCursor[((pCurSystemSet->uiDCLCDBright)/10 - 1)], NULL, NULL, 0);
		SwitchPowerDisplay();
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
    MsecSleep(10);
    return iReturn;
}


/***
  * 功能：
        窗体FrmStandbyset的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmStandbysetPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


/***
  * 功能：
        窗体FrmStandbyset的恢复函数，进行窗体恢复前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmStandbysetResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int StandbysetTextRes_Init(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;

	pStandbysetStrTitle = TransString("SYSTEMSET_STANDBY");
	pStandbysetStrBmpTitle = TransString("SCREENSHOT_SET");
	pStandbysetStrBackgroundBrightness = TransString("SYSTEMSET_BACKGROUND");
	pStandbysetStrBattery = TransString("SYSTEMSET_BATTERY");
	pStandbysetStrBStandbyTime = TransString("SYSTEMSET_STANDBYTIME");
	pStandbysetStrBAutoOff = TransString("SYSTEMSET_AUTOOFF");
    
	for(i = 0; i < 9; i++)
	{
		pStandbysetStrBStandby[i] = TransString((char *)standby[i]);
	}
	
	pStandbysetStrAdapter = TransString("SYSTEMSET_ADAPTER");
	pStandbysetStrAStandbyTime = TransString("SYSTEMSET_STANDBYTIME");
	pStandbysetStrAAutoOff = TransString("SYSTEMSET_AUTOOFF");
    
	for(i = 0; i < 9; i++)
	{
		pStandbysetStrAStandby[i] = TransString((char *)standby[i]);
	}
	
	pStandbysetStrSound = TransString("SYSTEMSET_SOUND");
	for(i = 0; i < 3; i++)
	{
		pStandbysetStrSSound[i] = TransString("sound");
	}
	
    return iReturn;
}


//释放文本资源
static int StandbysetTextRes_Exit(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = 0;
	
	GuiMemFree(pStandbysetStrTitle);
    GuiMemFree(pStandbysetStrBmpTitle);
	GuiMemFree(pStandbysetStrBackgroundBrightness);
	//释放电池、游标、声音的文本
	GuiMemFree(pStandbysetStrBattery);
	GuiMemFree(pStandbysetStrBStandbyTime);
	GuiMemFree(pStandbysetStrBAutoOff);
	for(i = 0; i < 9; i++)
	{
		GuiMemFree(pStandbysetStrBStandby[i]);
	}

	GuiMemFree(pStandbysetStrAdapter);
	GuiMemFree(pStandbysetStrAStandbyTime);
	GuiMemFree(pStandbysetStrAAutoOff);
	for(i = 0; i < 9; i++)
	{
		GuiMemFree(pStandbysetStrAStandby[i]);
	}
	
	GuiMemFree(pStandbysetStrSound);
	for(i = 0; i < 3; i++)
	{
		GuiMemFree(pStandbysetStrSSound[i]);
	}
	
    return iReturn;
}

/***
  * 功能：
        窗体错误处理函数
  * 参数：
        ...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int StandbysetErrProc_Func(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理
    return iReturn;
}


/***
  * 功能：
        窗体切换按钮down事件
  * 参数：
        1.void *pInArg	:   指向输入参数的指针
        2.int iInLen	:	输入参数的长度
        3.void *pOutArg	:	指向传出参数的指针
        4.int iOutLen	:	输出参数的长度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
static int StandbysetBtnChange1_Down(void *pInArg,  int iInLen, 
                                    void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	GUIWINDOW *pWnd = NULL;

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmBmpFileNamedInit, FrmBmpFileNamedExit, 
						FrmBmpFileNamedPaint, FrmBmpFileNamedLoop, 
						FrmBmpFileNamedPause, FrmBmpFileNamedResume,
						NULL);			
	SendWndMsg_WindowExit(pFrmStandbyset);	 
	SendSysMsg_ThreadCreate(pWnd);		
    return iReturn;	
}

//电池的事件处理函数
static int StandbysetBattery_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	//临时变量定义
	int j = 0;
	int i = *((int *)pOutArg);
	
	if(i < 5)
	{
		for(j = 0;j < 5;j++)
		{
			SetPictureBitmap(BmpFileDirectory"standby_point_unselect1.bmp", pStandbysetBattery[j]);
			DisplayPicture(pStandbysetBattery[j]);
		}
	}
	else
	{
		for(j = 5;j < 9;j++)
		{
			SetPictureBitmap(BmpFileDirectory"standby_point_unselect1.bmp", pStandbysetBattery[j]);
			DisplayPicture(pStandbysetBattery[j]);
		}
	}
	TouchChange("standby_point_select1.bmp", pStandbysetBattery[i],
		NULL, pStandbysetLblBStandby[i], 2);
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//电池的事件处理函数
static int StandbysetBattery_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;	
	
	int i = *((int *)pOutArg);
	switch (i)
	{
		case 0:
			pCurSystemSet->uiDCScreenOffValue = 10;
			break;
		case 1:
			pCurSystemSet->uiDCScreenOffValue = 30;
			break;
		case 2:
			pCurSystemSet->uiDCScreenOffValue = 60;
			break;
		case 3:
			pCurSystemSet->uiDCScreenOffValue = 300;
			break;
		case 4:
			pCurSystemSet->uiDCScreenOffValue = 0;
			break;
		case 5:
			pCurSystemSet->uiDCPowerOffValue = 60;
			break;
		case 6:
			pCurSystemSet->uiDCPowerOffValue = 300;
			break;
		case 7:
			pCurSystemSet->uiDCPowerOffValue = 600;
			break;
		case 8:
			pCurSystemSet->uiDCPowerOffValue = 0;
			break;
		default:
			break;
	}
	TouchChange(NULL, NULL, NULL, pStandbysetLblBStandby[i], 1);
	
	// 保存系统设置参数
    SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);
			
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//适配器的事件处理函数
static int StandbysetAdapter_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	//临时变量定义
	int j = 0;
	int i = *((int *)pOutArg);
	if(i < 5)
	{
		for(j = 0;j < 5;j++)
		{
			SetPictureBitmap(BmpFileDirectory"standby_point_unselect2.bmp", pStandbysetAdapter[j]);
			DisplayPicture(pStandbysetAdapter[j]);
		}
	}
	else
	{
		for(j = 5;j < 9;j++)
		{
			SetPictureBitmap(BmpFileDirectory"standby_point_unselect2.bmp", pStandbysetAdapter[j]);
			DisplayPicture(pStandbysetAdapter[j]);
		}
	}
	TouchChange("standby_point_select2.bmp", pStandbysetAdapter[i], NULL, 
		pStandbysetLblAStandby[i], 2);
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//适配器的事件处理函数
static int StandbysetAdapter_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = *((int *)pOutArg);
	switch (i)
	{
		case 0:
			pCurSystemSet->uiACScreenOffValue = 10;
			break;
		case 1:
			pCurSystemSet->uiACScreenOffValue = 30;
			break;
		case 2:
			pCurSystemSet->uiACScreenOffValue = 60;
			break;
		case 3:
			pCurSystemSet->uiACScreenOffValue = 300;
			break;
		case 4:
			pCurSystemSet->uiACScreenOffValue = 0;
			break;
		case 5:
			pCurSystemSet->uiACPowerOffValue = 60;
			break;
		case 6:
			pCurSystemSet->uiACPowerOffValue = 300;
			break;
		case 7:
			pCurSystemSet->uiACPowerOffValue = 600;
			break;
		case 8:
			pCurSystemSet->uiACPowerOffValue = 0;
			break;
		default:
			break;
	}
	TouchChange(NULL, NULL, NULL, pStandbysetLblAStandby[i], 1);
	// 保存系统设置参数
    SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);
			
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//声音的事件处理函数						   
static int StandbysetSound_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = *((int *)pOutArg);
	if(i == 0)
	{
		if(iSound1Flag == 0)
		{
			iSound1Flag = 1;
			pCurSystemSet->ucKeyWarning = 1;
			SetPictureBitmap(BmpFileDirectory"standby_point_select.bmp", pStandbysetSound[0]);
			DisplayPicture(pStandbysetSound[0]);
			TouchChange("standby_point_unselect3.bmp", pStandbysetSound[2], 
				NULL, pStandbysetLblSSound[0], 2);
		}
		else
		{
			iSound1Flag = 0;
			pCurSystemSet->ucKeyWarning = 0;
			TouchChange("standby_point_unselect3.bmp", pStandbysetSound[0],
				NULL, pStandbysetLblSSound[0], 2);	
			if(iSound2Flag == 0)
				TouchChange("standby_point_select3.bmp", pStandbysetSound[2], 
				NULL, NULL, 0);	
		}
	}
	else if(i == 1)
	{
		if(iSound2Flag == 0)
		{
			iSound2Flag = 1;
			pCurSystemSet->uiWarning = 1;
			SetPictureBitmap(BmpFileDirectory"standby_point_select3.bmp", pStandbysetSound[1]);
			DisplayPicture(pStandbysetSound[1]);
			TouchChange("standby_point_unselect3.bmp", pStandbysetSound[2], 
				NULL, pStandbysetLblSSound[1], 2);
		}
		else
		{
			iSound2Flag = 0;
			pCurSystemSet->uiWarning = 0;
			TouchChange("standby_point_unselect3.bmp", pStandbysetSound[1],
				NULL, pStandbysetLblSSound[1], 2);	
			if(iSound1Flag == 0)
				TouchChange("standby_point_select3.bmp", pStandbysetSound[2],
				NULL, NULL, 0);	
		}
	}
	else
	{
		iSound1Flag = 0;
		iSound2Flag = 0;
		pCurSystemSet->uiWarning = 0;
		pCurSystemSet->ucKeyWarning = 0;
		for(i = 0;i < 2;i++)
		{
			SetPictureBitmap(BmpFileDirectory"standby_point_unselect3.bmp", pStandbysetSound[i]);
			DisplayPicture(pStandbysetSound[i]);
		}
		TouchChange("standby_point_select3.bmp", pStandbysetSound[2],
			NULL, pStandbysetLblSSound[2], 2);
	}

	// 保存系统设置参数
    SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);
	
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//声音的事件处理函数
static int StandbysetSound_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	int i = *((int *)pOutArg);
	if(i == 0)
	{
		TouchChange(NULL, NULL, NULL, pStandbysetLblSSound[0], 1);
	}
	else if(i == 1)
	{
		TouchChange(NULL, NULL, NULL, pStandbysetLblSSound[1], 1);
	}
	else
	{
		TouchChange(NULL, NULL, NULL, pStandbysetLblSSound[2], 1);
	}
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//亮度游标的事件处理函数
static int StandbysetCursor_Down(void *pInArg, int iInLen, 
                             void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int i = *((int *)pOutArg);
	
	DisplayPicture(pStandbysetCursorMask);
	TouchChange("cursor_standby.bmp", pStandbysetCursor[i], NULL, NULL, 0);

	SetBrightness((i+1)*10,(i+1)*10);
	if(CheckPowerOn(ADAPTER))
		pCurSystemSet->uiACLCDBright = (i+1)*10;
	else
		pCurSystemSet->uiDCLCDBright = (i+1)*10;

	// 保存系统设置参数
    SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);
		
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

//亮度游标的事件处理函数
static int StandbysetCursor_Up(void *pInArg, int iInLen, 
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    return iReturn;
}

//菜单栏控件回调
static void StandbysetWndMenuCallBack(int selected)
{
	GUIWINDOW *pWnd = NULL;
    
	switch (selected)
	{
	case 0:
        {
            uiFactoryCode <<= 1;
            uiFactoryCode |= 1;
            if(uiFactoryCode == 0x1f7)
            {
		        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    InputPasswardInit, InputPasswardExit, 
		                    InputPasswardPaint, InputPaawardLoop, 
					        InputPasswardPause, InputPasswardResume,
		                    NULL);          
		        SendWndMsg_WindowExit(pFrmStandbyset);  
		        SendSysMsg_ThreadCreate(pWnd); 
            }
         }
		break;
	case 1://调取时间窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmTimeSetInit, FrmTimeSetExit, 
		                    FrmTimeSetPaint, FrmTimeSetLoop, 
					        FrmTimeSetPause, FrmTimeSetResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmStandbyset);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调取语言设置窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmLanguageSetInit, FrmLanguageSetExit, 
							FrmLanguageSetPaint, FrmLanguageSetLoop, 
							FrmLanguageSetPause, FrmLanguageSetResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmStandbyset);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 3:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
								FrmWiFiInit, FrmWiFiExit, 
								FrmWiFiPaint, FrmWiFiLoop, 
								FrmWiFiPause, FrmWiFiResume,
								NULL);			
		SendWndMsg_WindowExit(pFrmStandbyset);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 4:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmSysMaintainInit, FrmSysMaintainExit, 
		                    FrmSysMaintainPaint, FrmSysMaintainLoop, 
					        FrmSysMaintainPause, FrmSysMaintainResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmStandbyset);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 5:
        #ifdef MINI2
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAboutInit, FrmAboutExit, 
							FrmAboutPaint, FrmAboutLoop, 
							FrmAboutPause, FrmAboutResume,
							NULL);			       //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmStandbyset);	  //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			  //发送消息以便调用新的窗体
        #endif
		break;		
	case BACK_DOWN://退出时间设置
        if(uiFactoryCode == 0x1f)
        {
            uiFactoryCode <<= 1;
        }
        else
        {
    	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
    	                        FrmMainInit, FrmMainExit, 
    	                        FrmMainPaint, FrmMainLoop, 
    					        FrmMainPause, FrmMainResume,
    	                        NULL);          
    	    SendWndMsg_WindowExit(pFrmStandbyset);  
    	    SendSysMsg_ThreadCreate(pWnd); 
        }
        break;	
	case HOME_DOWN://退出时间设置
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmMainInit, FrmMainExit, 
	                        FrmMainPaint, FrmMainLoop, 
					        FrmMainPause, FrmMainResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmStandbyset);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;	
	default:
		break;
	}
}

//切换电池和适配器
static int SwitchPowerDisplay()
{
	if(CheckPowerOn(ADAPTER))
	{
		int i = 0;

		SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblBattery);
		SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblBStandbyTime);
		SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblBAutoOff);
		DisplayLabel(pStandbysetLblBattery);
		DisplayLabel(pStandbysetLblBStandbyTime);
		DisplayLabel(pStandbysetLblBAutoOff);
		for(i = 0; i < 9; i++)
		{
			SetPictureEnable(1, pStandbysetBattery[i]);
			SetLabelEnable(1, pStandbysetLblBStandby[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblBStandby[i]);
			TouchChange("standby_point_disable1.bmp", pStandbysetBattery[i], NULL, pStandbysetLblBStandby[i], 3);
			SetPictureEnable(0, pStandbysetBattery[i]);
			SetLabelEnable(0, pStandbysetLblBStandby[i]);
			SetPictureEnable(1, pStandbysetAdapter[i]);
			SetLabelEnable(1, pStandbysetLblAStandby[i]);
		}


		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblAdapter);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblAStandbyTime);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblAAutoOff);
		DisplayLabel(pStandbysetLblAdapter);
		DisplayLabel(pStandbysetLblAStandbyTime);
		DisplayLabel(pStandbysetLblAAutoOff);
		for(i = 0;i < 9; i++)
		{
			SetPictureBitmap(BmpFileDirectory"standby_point_unselect2.bmp", pStandbysetAdapter[i]);
			DisplayPicture(pStandbysetAdapter[i]);
		}
		//屏幕适配器待机时间选择
		switch (pCurSystemSet->uiACScreenOffValue)
		{
			case 10:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[0]);
				DisplayPicture(pStandbysetAdapter[0]);
				break;
			case 30:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[1]);
				DisplayPicture(pStandbysetAdapter[1]);
				break;
			case 60:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[2]);
				DisplayPicture(pStandbysetAdapter[2]);
				break;
			case 300:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[3]);
				DisplayPicture(pStandbysetAdapter[3]);
				break;
			case 0:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[4]);
				DisplayPicture(pStandbysetAdapter[4]);
				break;
			default:
			LOG(LOG_DEBUG, "pCurSystemSet->uiACScreenOffValue = %d\n", pCurSystemSet->uiACScreenOffValue);
				break;
		}

		//屏幕适配器自动关机时间选择
		switch (pCurSystemSet->uiACPowerOffValue)
		{
			case 60:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[5]);
				DisplayPicture(pStandbysetAdapter[5]);
				break;
			case 300:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[6]);
				DisplayPicture(pStandbysetAdapter[6]);
				break;
			case 600:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[7]);
				DisplayPicture(pStandbysetAdapter[7]);
				break;
			case 0:
				SetPictureBitmap(BmpFileDirectory"standby_point_select2.bmp", pStandbysetAdapter[8]);
				DisplayPicture(pStandbysetAdapter[8]);
				break;
			default:
				break;
		}
		for(i = 0; i < 9; i++)
		{
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblAStandby[i]);
			DisplayLabel(pStandbysetLblAStandby[i]);
		}
		
	}
	else
	{
		int i = 0;
		
		SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblAdapter);
		SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblAStandbyTime);
		SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblAAutoOff);
		DisplayLabel(pStandbysetLblAdapter);
		DisplayLabel(pStandbysetLblAStandbyTime);
		DisplayLabel(pStandbysetLblAAutoOff);
		for(i = 0; i < 9; i++)
		{
			SetPictureEnable(1, pStandbysetAdapter[i]);
			SetLabelEnable(1, pStandbysetLblAStandby[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pStandbysetLblAStandby[i]);
			TouchChange("standby_point_disable2.bmp", pStandbysetAdapter[i], NULL, pStandbysetLblAStandby[i], 3);
			SetPictureEnable(0, pStandbysetAdapter[i]);
			SetLabelEnable(0, pStandbysetLblAStandby[i]);
			SetPictureEnable(1, pStandbysetBattery[i]);
			SetLabelEnable(1, pStandbysetLblBStandby[i]);
		}


		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblBattery);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblBStandbyTime);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblBAutoOff);
		DisplayLabel(pStandbysetLblBattery);
		DisplayLabel(pStandbysetLblBStandbyTime);
		DisplayLabel(pStandbysetLblBAutoOff);
		for(i = 0;i < 9; i++)
		{
			SetPictureBitmap(BmpFileDirectory"standby_point_unselect1.bmp", pStandbysetBattery[i]);
			DisplayPicture(pStandbysetBattery[i]);
		}
		//电池待机时间选择
		switch (pCurSystemSet->uiDCScreenOffValue)
		{	
			case 10:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[0]);
				DisplayPicture(pStandbysetBattery[0]);
				break;
			case 30:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[1]);
				DisplayPicture(pStandbysetBattery[1]);
				break;
			case 60:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[2]);
				DisplayPicture(pStandbysetBattery[2]);
				break;
			case 300:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[3]);
				DisplayPicture(pStandbysetBattery[3]);
				break;
			case 0:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[4]);
				DisplayPicture(pStandbysetBattery[4]);
			default:
				LOG(LOG_DEBUG, "pCurSystemSet->uiDCScreenOffValue = %d\n", pCurSystemSet->uiDCScreenOffValue);	
				break;
		}

		//电池自动关机时间选择
		switch (pCurSystemSet->uiDCPowerOffValue)
		{
			case 60:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[5]);
				DisplayPicture(pStandbysetBattery[5]);
				break;
			case 300:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[6]);
				DisplayPicture(pStandbysetBattery[6]);
				break;
			case 600:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[7]);
				DisplayPicture(pStandbysetBattery[7]);
				break;
			case 0:
				SetPictureBitmap(BmpFileDirectory"standby_point_select1.bmp", pStandbysetBattery[8]);
				DisplayPicture(pStandbysetBattery[8]);
				break;
			default:
				break;
		}	
		for(i = 0; i < 9; i++)
		{
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pStandbysetLblBStandby[i]);
			DisplayLabel(pStandbysetLblBStandby[i]);
		}
	}
	return 0;
}
