/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmtimeset.c
* 摘    要：  实现主窗体FrmTimeset的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：  2020-8-24
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


#include "wnd_frmtimeset.h"

/***********************************
* 定义wnd_frmtimeset.c引用其他头文件
***********************************/
#include "guipicture.h"
#include "guiphoto.h"

#include "wnd_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmsysmaintain.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmlanguageset.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmabout.h"
#include "wnd_stack.h"
#include "wnd_frmwifi.h"

#include "app_global.h"
#include "app_parameter.h"
#include "app_frmtimeset.h"



/***********************************
* 定义wnd_frmtimeset.c中内部使用的宏
***********************************/
#define POS_LEFT    0
#define POS_CENTER  1
#define POS_RIGHT   2

extern PSYSTEMSET pCurSystemSet;
/*************************************
* 定义wnd_frmtimeset.c中内部使用的变量
*************************************/
static int iEnableApply = 0;		//应用 按键使能标志
static RTCVAL *pNewRtcTime = NULL;	//存储当前设置过Rtc时间 
static RTCVAL *pOldRtcTime = NULL;	//保存设置前的Rtc时间 

/*********************************
* wnd_frmtimeset.c窗体中的窗体资源
*********************************/
static GUIWINDOW *pFrmTimeSet = NULL;

/*****************************************
* wnd_frmtimeset.c窗体标题以及背景控件资源
*****************************************/
static GUIPICTURE *pTimeSetBg = NULL;
static GUILABEL *pTimeSetLblTitle = NULL; 
static GUICHAR *pTimeSetStrTitle = NULL;

static GUILABEL *pDataLblTitle = NULL;
static GUICHAR *pDataStrTitle = NULL;

static GUILABEL *pDataFormatLblTitle = NULL;
static GUICHAR *pDataFormatStrTitle = NULL;

/***********************************
* wnd_frmtimeset.c桌面上的的控件资源
***********************************/
//年.月.日
static GUIPICTURE *pTimeSetBtnDate[3] = {NULL};
static GUIPICTURE *pTimeSetBtnDateAdd[3] = {NULL};
static GUIPICTURE *pTimeSetBtnDateReduce[3] = {NULL};

static GUILABEL *pTimeSetLblDateL[3] = {NULL};
static GUICHAR *pTimeSetStrDateL[3] = {NULL};

static GUIPICTURE* pTimeSetYMDPicture[3] = {NULL};
static GUILABEL *pTimeSetLblYMD[3] = {NULL};
static GUICHAR *pTimeSetStrYMD[3] = {NULL};
//时.分.秒
static GUIPICTURE *pTimeSetBtnTime[3] = {NULL};
static GUIPICTURE *pTimeSetBtnTimeAdd[3] = {NULL};
static GUIPICTURE *pTimeSetBtnTimeReduce[3] = {NULL};

static GUILABEL *pTimeSetLblTimeL[3] = {NULL};
static GUILABEL *pTimeSetLblHMS = NULL;

static GUICHAR *pTimeSetStrTimeL[3] = {NULL};
static GUICHAR *pTimeSetStrHMS = NULL;

//确定按键控件
static GUIPICTURE *pTimeSetBtnApply = NULL;
static GUILABEL *pTimeSetLblApply = NULL;
static GUICHAR *pTimeSetStrApply = NULL;

//菜单栏控件
static WNDMENU1 *pTimeMenu = NULL;

/*********************************
* 声明wnd_frmtimeset.c窗体内部函数
*********************************/
//初始化系统相关参数
static int TimeInitPara(void);
//清理系统相关参数
static int TimeClearPara(void);
//将时间信息转换为宽字符以显示在屏幕上
GUICHAR* GetTimeString(int Value, char* Para);
//年份增加处理函数
static int TimeSetYearAdd(int);
//月份增加处理函数
static int TimeSetMonAdd(int);
//天数增加处理函数
static int TimeSetDayAdd(int);
//年份减少处理函数
static int TimeSetYearReduce(int);
//月份减少处理函数
static int TimeSetMonReduce(int);
//天数减少处理函数
static int TimeSetDayReduce(int);
//小时增加处理函数
static int TimeSetHourAdd(void);
//分钟增加处理函数
static int TimeSetMinAdd(void);
//秒数增加处理函数
static int TimeSetSecAdd(void);
//小时减少处理函数
static int TimeSetHourReduce(void);
//分钟减少处理函数
static int TimeSetMinReduce(void);
//秒数减少处理函数 
static int TimeSetSecReduce(void);
//刷新日期显示
static int DateFlushDisplay(int ymd, int dat, char *para);
//刷新时间显示
static int TimeFlushDisplay(int hms, int dat, char *para);

static void DisplayDate(int y_pos, int m_pos, int d_pos);

static void FlushDate();

//初始化文本函数 
static int FrmTimeSetRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
//销毁文本函数
static int FrmTimeSetRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
//日期调整按钮事件处理
static int TimeSetBtnDateAdd_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen);
static int TimeSetBtnDateAdd_Up(void *pInArg, int iInLen, 
                             	void *pOutArg, int iOutLen);
static int TimeSetBtnDateReduce_Down(void *pInArg, int iInLen, 
                             		 void *pOutArg, int iOutLen);
static int TimeSetBtnDateReduce_Up(void *pInArg, int iInLen, 
                             	   void *pOutArg, int iOutLen);

static int TimeSetYMD_Down(void *pInArg, int iInLen, 
                             	   void *pOutArg, int iOutLen);
static int TimeSetYMD_Up(void *pInArg, int iInLen, 
                             	   void *pOutArg, int iOutLen);
//时间调整按钮事件处理
static int TimeSetBtnTimeAdd_Down(void *pInArg, int iInLen, 
                             	  void *pOutArg, int iOutLen);
static int TimeSetBtnTimeAdd_Up(void *pInArg, int iInLen, 
                             	void *pOutArg, int iOutLen);
static int TimeSetBtnTimeReduce_Down(void *pInArg, int iInLen, 
                             		 void *pOutArg, int iOutLen);
static int TimeSetBtnTimeReduce_Up(void *pInArg, int iInLen, 
                             	   void *pOutArg, int iOutLen);
//接受设置按钮事件处理函数
static int TimeSetBtnApply_Down(void *pInArg, int iInLen, 
                             	void *pOutArg, int iOutLen);
static int TimeSetBtnApply_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
//菜单栏回调函数
static void TimeSetWndMenuCallBack(int selected);


/***
  * 功能：
        窗体的初始化函数，建立窗体控件、注册消息处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmTimeSetInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
	//临时变量定义 
	int i = 0;
    GUIMESSAGE *pMsg = NULL;
    #ifdef MINI2
	unsigned int pMenuStr[] = {
		SYSTEMSET_STANDBY,
		SYSTEMSET_TIME,
		SYSTEMSET_LANGUAGE,
		MAIN_LBL_WIFI,
		SYSTEMSET_SYSMAINTAIN,
		SYSTEM_ABOUT
	};
    #else
    unsigned int pMenuStr[] = {
		1
	};
    #endif
    pFrmTimeSet = (GUIWINDOW *)pWndObj;
	//初始化参数 
	TimeInitPara();
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    FrmTimeSetRes_Init(NULL, 0, NULL, 0);
	//桌面背景
    pTimeSetBg = CreatePhoto("sysset_time_bg");
	//桌面上其他的控件
    CreateLoopPhoto(pTimeSetBtnDate,"sysset_btn_time_unpress",0,0,3);
    CreateLoopPhoto(pTimeSetBtnDateAdd,"sysset_btn_up_unpress",0,0,3);
    CreateLoopPhoto(pTimeSetBtnDateReduce,"sysset_btn_dn_unpress",0,0,3);
    
    CreateLoopPhoto(pTimeSetBtnTime,"sysset_btn_time_unpress1",0,0,3);
    CreateLoopPhoto(pTimeSetBtnTimeAdd,"sysset_btn_up_unpress1",0,0,3);
    CreateLoopPhoto(pTimeSetBtnTimeReduce,"sysset_btn_dn_unpress1",0,0,3);
    pTimeSetBtnApply = CreatePhoto("tmie_apply_unpress1");
	//桌面上的标签(时间和日期)
 	pTimeSetLblTitle = CreateLabel(0, 24, 100, 24, pTimeSetStrTitle);
    pDataLblTitle = CreateLabel(25, 70, 200, 24, pDataStrTitle);
    pDataFormatLblTitle = CreateLabel(45, 130, 200, 24, pDataFormatStrTitle); 
	pTimeSetLblHMS = CreateLabel(350, 70, 280, 24, pTimeSetStrHMS);
	for (i = 0; i < 3; ++i)
	{
		pTimeSetLblDateL[i] = CreateLabel(50 + 90*i, 345, 80, 24,   
										  pTimeSetStrDateL[i]);
		pTimeSetLblTimeL[i] = CreateLabel(365 + 100*i, 185, 80, 24,   
										  pTimeSetStrTimeL[i]);
        
        pTimeSetLblYMD[i] = CreateLabel(85, 170+30*i, 250, 24, pTimeSetStrYMD[i]);
	}
	CreateLoopPhoto(pTimeSetYMDPicture,"sysData_set_unpress",0,0,3);
	//按钮的标签
 	pTimeSetLblApply = CreateLabel(530, 420, 130, 24, pTimeSetStrApply);

	//菜单栏控件
	#ifdef MINI2
	pTimeMenu = CreateWndMenu1(6, sizeof(pMenuStr), pMenuStr, 
							   MENU_BACK | MENU_HOME, 1, 0, 40, 
							   TimeSetWndMenuCallBack);
    #else
    pTimeMenu = CreateWndMenu1(5, sizeof(pMenuStr), pMenuStr, 
							   MENU_BACK | MENU_HOME, 1, 0, 40, 
							   TimeSetWndMenuCallBack);
    #endif
	//设置label字体
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pTimeSetLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pDataLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pDataFormatLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pTimeSetLblHMS);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTimeSetLblApply);
	//设置字体的显示方式居中
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pTimeSetLblTitle);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pDataLblTitle);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, pDataFormatLblTitle);
	SetLabelAlign(GUILABEL_ALIGN_LEFT, pTimeSetLblHMS);
	for(i = 0; i < 3; ++i)
	{
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pTimeSetLblDateL[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pTimeSetLblTimeL[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pTimeSetLblYMD[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTimeSetLblYMD[i]);
	}
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pTimeSetLblApply);
    

	//注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
   	//注册窗体控件必须在注册消息处理函数之前进行
   	
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), 
                  pFrmTimeSet, pFrmTimeSet);

	//注册桌面上的控件 
	for (i = 0; i < 3; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pTimeSetBtnDateAdd[i], pFrmTimeSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pTimeSetBtnDateReduce[i], pFrmTimeSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pTimeSetBtnTimeAdd[i], pFrmTimeSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pTimeSetBtnTimeReduce[i], pFrmTimeSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pTimeSetYMDPicture[i], pFrmTimeSet);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
					  pTimeSetLblYMD[i], pFrmTimeSet);
	}
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
				  pTimeSetBtnApply, pFrmTimeSet);

	//注册右侧按钮区的控件
    AddWndMenuToComp1(pTimeMenu, pFrmTimeSet);
	

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    pMsg = GetCurrMessage();
	//注册桌面上控件的消息处理
	for (i = 0; i < 3; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTimeSetBtnDateAdd[i], 
                    TimeSetBtnDateAdd_Down, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pTimeSetBtnDateAdd[i], 
                	TimeSetBtnDateAdd_Up, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTimeSetBtnDateReduce[i], 
                    TimeSetBtnDateReduce_Down, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pTimeSetBtnDateReduce[i], 
                	TimeSetBtnDateReduce_Up, (void*)(i+1), sizeof(int), pMsg);	
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTimeSetBtnTimeAdd[i], 
                    TimeSetBtnTimeAdd_Down, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pTimeSetBtnTimeAdd[i], 
                	TimeSetBtnTimeAdd_Up, (void*)(i+1), sizeof(int), pMsg);		
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTimeSetBtnTimeReduce[i], 
                    TimeSetBtnTimeReduce_Down, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pTimeSetBtnTimeReduce[i], 
                	TimeSetBtnTimeReduce_Up, (void*)(i+1), sizeof(int), pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTimeSetYMDPicture[i], 
                    TimeSetYMD_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pTimeSetYMDPicture[i], 
                	TimeSetYMD_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTimeSetLblYMD[i], 
                    TimeSetYMD_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pTimeSetLblYMD[i], 
                	TimeSetYMD_Up, NULL, i, pMsg);
	}
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTimeSetBtnApply, 
                    TimeSetBtnApply_Down, (void*)(i+1), sizeof(int), pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pTimeSetBtnApply, 
                	TimeSetBtnApply_Up, (void*)(i+1), sizeof(int), pMsg);
	//右侧菜单栏控件
	LoginWndMenuToMsg1(pTimeMenu, pFrmTimeSet);

	return iRet;
}

/***
  * 功能：
        窗体的退出函数，释放所有资源
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmTimeSetExit(void *pWndObj)
{
    //错误标志、返回值定义 
    int iRet = 0;
    //临时变量定义 
    GUIMESSAGE *pMsg = NULL;
	int i = 0;
	
    //清空消息队列中的消息注册项
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);
	
    //从当前窗体中注销窗体控件
    ClearWindowComp(pWndObj);

	//销毁桌面上的控件
	DestroyPicture(&pTimeSetBg);
    //DestroyPicture(&pTimeSetBgTableTitle);
	for (i = 0; i < 3; ++i)
	{
		DestroyPicture(&pTimeSetBtnDate[i]);
		DestroyPicture(&pTimeSetBtnTime[i]);
		DestroyPicture(&pTimeSetBtnDateAdd[i]);
		DestroyPicture(&pTimeSetBtnDateReduce[i]);
		DestroyPicture(&pTimeSetBtnTimeAdd[i]);
		DestroyPicture(&pTimeSetBtnTimeReduce[i]);
        
        DestroyPicture(&pTimeSetYMDPicture[i]);
	}
	DestroyPicture(&pTimeSetBtnApply);

	//菜单区的控件 
	DestroyWndMenu1(&pTimeMenu);

	//桌面上的标签 
	DestroyLabel(&pTimeSetLblTitle);
    DestroyLabel(&pDataLblTitle);
    DestroyLabel(&pDataFormatLblTitle);
	DestroyLabel(&pTimeSetLblHMS);
	for (i = 0; i < 3; ++i)
	{
		DestroyLabel(&pTimeSetLblDateL[i]);
		DestroyLabel(&pTimeSetLblTimeL[i]);
        DestroyLabel(&pTimeSetLblYMD[i]);
	}
	DestroyLabel(&pTimeSetLblApply);

	//释放文本资源 
    FrmTimeSetRes_Exit(NULL, 0, NULL, 0);
	
	//清理系统相关参数 
	TimeClearPara();

	return iRet;
}

/***
  * 功能：
        窗体的绘制函数，绘制整个窗体
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmTimeSetPaint(void *pWndObj)
{
    //错误标志、返回值定义 
    int iRet = 0;
	int i = 0;
	
	//桌面上的控件 
	DisplayPicture(pTimeSetBg);
    //DisplayPicture(pTimeSetBgTableTitle);
	for (i = 0; i < 3; ++i)
	{
		DisplayPicture(pTimeSetBtnDate[i]);
		DisplayPicture(pTimeSetBtnTime[i]);
		DisplayPicture(pTimeSetBtnDateAdd[i]);
		DisplayPicture(pTimeSetBtnDateReduce[i]);
		DisplayPicture(pTimeSetBtnTimeAdd[i]);
		DisplayPicture(pTimeSetBtnTimeReduce[i]);
	}
	DisplayPicture(pTimeSetBtnApply);

	//菜单区的控件 
	DisplayWndMenu1(pTimeMenu);
	
	//桌面上的标签 
	DisplayLabel(pTimeSetLblTitle);
    DisplayLabel(pDataLblTitle);
    DisplayLabel(pDataFormatLblTitle); 
	DisplayLabel(pTimeSetLblHMS);
	for (i = 0; i < 3; ++i)
	{
		DisplayLabel(pTimeSetLblDateL[i]);
		DisplayLabel(pTimeSetLblTimeL[i]);
	}
    for (i = 0; i < 3; i++)
	{
		if(i == pCurSystemSet->uiTimeShowFlag){
			SetPictureBitmap(BmpFileDirectory"sysData_set_press.bmp", pTimeSetYMDPicture[i]);
			DisplayPicture(pTimeSetYMDPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pTimeSetLblYMD[i]);
            DisplayLabel(pTimeSetLblYMD[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"sysData_set_unpress.bmp", pTimeSetYMDPicture[i]);
			DisplayPicture(pTimeSetYMDPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTimeSetLblYMD[i]);
            DisplayLabel(pTimeSetLblYMD[i]);
		}
	}
	DisplayLabel(pTimeSetLblApply);
	SetPowerEnable(1, 1);
    FlushDate();
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}

/***
  * 功能：
        窗体的循环函数，进行窗体循环
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmTimeSetLoop(void *pWndObj)
{
	//错误标志、返回值定义 
    int iRet = 0;

	if (pNewRtcTime == NULL || pOldRtcTime == NULL)
	{
		return -1;
	}

	//判断时间是否已经被修改，如果否设置apply不可用 
	if (!memcmp(pNewRtcTime, pOldRtcTime, sizeof(RTCVAL)) &&
		 iEnableApply == 1)
	{
		iEnableApply = 0;
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTimeSetLblApply);
		DisplayLabel(pTimeSetLblApply);
		SetPictureEnable(iEnableApply, pTimeSetBtnApply);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	//判断时间是否已经被修改，如果是设置apply可用 
	else if (memcmp(pNewRtcTime, pOldRtcTime, sizeof(RTCVAL)) &&
			 iEnableApply == 0)
	{
		iEnableApply = 1;
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTimeSetLblApply);
		DisplayLabel(pTimeSetLblApply);
		SetPictureEnable(iEnableApply, pTimeSetBtnApply);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	
	MsecSleep(10);
	
    return iRet;
}

/***
  * 功能：
        窗体的挂起函数，进行窗体挂起前预处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmTimeSetPause(void *pWndObj)
{
    //错误标志、返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体的恢复函数，进行窗体恢复前预处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmTimeSetResume(void *pWndObj)
{
    //错误标志、返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        初始化文本资源
  * 参数：
  		//...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmTimeSetRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iRet = 0;
 	//桌面上的文本 
	pTimeSetStrTitle = TransString("SYSTEMSET_TIME");
    pDataStrTitle = TransString("TIMESET_DATE");
    pDataFormatStrTitle = TransString("TIMESET_FORMAT");
    pTimeSetStrYMD[0] = TransString("TIMESET_DMY");
    pTimeSetStrYMD[1] = TransString("TIMESET_MDY");
    pTimeSetStrYMD[2] = TransString("TIMESET_YMD");
	pTimeSetStrHMS = TransString("SYSTEMSET_HMS");
	
	pTimeSetStrDateL[T_YEAR] = GetTimeString(pNewRtcTime->tm_year, "%04d");
	pTimeSetStrDateL[T_MONTH] = GetTimeString(pNewRtcTime->tm_mon, "%02d");
	pTimeSetStrDateL[T_DAY] = GetTimeString(pNewRtcTime->tm_mday, "%02d");
	
	pTimeSetStrTimeL[T_HOURS] = GetTimeString(pNewRtcTime->tm_hour, "%02d");
	pTimeSetStrTimeL[T_MINUTE] = GetTimeString(pNewRtcTime->tm_min, "%02d");
	pTimeSetStrTimeL[T_SECOND] = GetTimeString(pNewRtcTime->tm_sec, "%02d");
	
	//按钮区的文本 
	pTimeSetStrApply = TransString("SYSTEMSET_APPLY");

    return iRet;
}

/***
  * 功能：
        释放文本资源
  * 参数：
  		//...
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmTimeSetRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iRet = 0;
	int i = 0;
	
	//桌面上的文本 
    GuiMemFree(pTimeSetStrTitle);
    GuiMemFree(pDataStrTitle);
    GuiMemFree(pDataFormatStrTitle);
	GuiMemFree(pTimeSetStrHMS);
	for (i = 0; i < 3; ++i)
	{
		GuiMemFree(pTimeSetStrDateL[i]);
		GuiMemFree(pTimeSetStrTimeL[i]);
        GuiMemFree(pTimeSetStrYMD[i]);
	}
	GuiMemFree(pTimeSetStrApply);

	return iRet;
}

/***
  * 功能：
     	初始化全局相关参数
  * 参数：
  		无
  * 返回：
        成功返0，失败!0
  * 备注：
  		必须在窗体Init函数最开始调用
***/ 
static int TimeInitPara(void)
{
	//获取当前时间 
	iEnableApply = 1;
	pNewRtcTime = GetCurTime();
	pOldRtcTime = (RTCVAL *)GuiMemAlloc(sizeof(RTCVAL));
	if (pOldRtcTime == NULL)
	{
		CODER_LOG(CoderYun, "malloc");
		return -1;
	}
	memcpy(pOldRtcTime, pNewRtcTime, sizeof(RTCVAL));
	//...
	return 0;
}

/***
  * 功能：
     	清理全局相关参数
  * 参数：
  		无
  * 返回：
        成功返0，失败!0
  * 备注：
  		必须在窗体Exit时调用
***/ 
static int TimeClearPara(void)
{
	GuiMemFree(pNewRtcTime);
	GuiMemFree(pOldRtcTime);
	iEnableApply = 0;
	
	return 0;
}

/***
  * 功能：
     	获取时间文本字符串
  * 参数：
  		int Value :数值
  		int Len :长度
  * 返回：
        成功返宽字符串，失败返回NULL
  * 备注：
***/ 
GUICHAR* GetTimeString(int Value, char *Para)
{
	GUICHAR *pDateString = NULL;
	char pBuffer[6] = {0};

	sprintf(pBuffer, Para, Value);
	pDateString = TransString(pBuffer);
	
	return pDateString;
}

/***
  * 功能：
     	刷新日期显示
  * 参数：
  		int ymd :年月日索引(T_YEAR, T_MONTH, T_DAY)
  		int dat :具体的数据
  		int len :字符串长度
  * 返回：
        成功0，失败非0
  * 备注：
***/
static int DateFlushDisplay(int ymd, int dat, char *para)
{
	GuiMemFree(pTimeSetStrDateL[ymd]);
	pTimeSetStrDateL[ymd] = GetTimeString(dat, para);
	SetLabelText(pTimeSetStrDateL[ymd], pTimeSetLblDateL[ymd]);
	DisplayPicture(pTimeSetBtnDate[ymd]);
	DisplayLabel(pTimeSetLblDateL[ymd]);

	return 0;
}

/***
  * 功能：
     	刷新时间显示
  * 参数：
  		int hms :时分秒索引(T_HOURS, T_MINUTE, T_SECOND)
  		int dat :具体的数据
  		int len :字符串长度
  * 返回：
        成功0，失败非0
  * 备注：
***/
static int TimeFlushDisplay(int hms, int dat, char *para)
{
	GuiMemFree(pTimeSetStrTimeL[hms]);
	pTimeSetStrTimeL[hms] = GetTimeString(dat, para);
	SetLabelText(pTimeSetStrTimeL[hms], pTimeSetLblTimeL[hms]);
	DisplayPicture(pTimeSetBtnTime[hms]);
	DisplayLabel(pTimeSetLblTimeL[hms]);

	return 0;
}

static void DisplayDate(int y_pos, int m_pos, int d_pos)
{
	DateFlushDisplay(y_pos, pNewRtcTime->tm_year, "%04d");
	DateFlushDisplay(m_pos, pNewRtcTime->tm_mon , "%02d");
	DateFlushDisplay(d_pos, pNewRtcTime->tm_mday, "%02d");
}

static void FlushDate()
{
    switch (pCurSystemSet->uiTimeShowFlag)
    {
    case 0:
        DisplayDate(POS_RIGHT, POS_CENTER, POS_LEFT);
        break;
    case 1:
        DisplayDate(POS_RIGHT, POS_LEFT, POS_CENTER);
        break;
    case 2:
        DisplayDate(POS_LEFT, POS_CENTER, POS_RIGHT);
        break;
    }
}


/***
  * 功能：
     	年份增加处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetYearAdd(int temp)
{
	pNewRtcTime->tm_year++;
	if (pNewRtcTime->tm_year > 2037)
	{
		pNewRtcTime->tm_year = 1970;
	}
	
	return 0;
}

/***
  * 功能：
     	年份减少处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetYearReduce(int temp)
{
	pNewRtcTime->tm_year--;
	if (pNewRtcTime->tm_year < 1970)
	{
		pNewRtcTime->tm_year = 2037;
	}
	
	return 0;
}

/***
  * 功能：
     	月份增加处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetMonAdd(int temp)
{
	pNewRtcTime->tm_mon++;
	if (pNewRtcTime->tm_mon > 12)
	{
		pNewRtcTime->tm_mon = 1;
	}
	//判断当前设置的月份天数是否符合要求，比如当前是5.31号，
	//那么月份加为6时day不能为显示31，2月也要特殊处理 
	switch(pNewRtcTime->tm_mon)
	{
	case 4:
	case 6:
	case 9:
	case 11:
		if (pNewRtcTime->tm_mday > 30)
	    {
	        pNewRtcTime->tm_mday = 1;
	    }
	    break;
	case 2:    
		if ( (pNewRtcTime->tm_year%4 == 0 && pNewRtcTime->tm_year%100 != 0) || 
			 (pNewRtcTime->tm_year%400 == 0) ) 
	    {
	        if (pNewRtcTime->tm_mday > 29)
	        {
	            pNewRtcTime->tm_mday = 1;
	        }
	    }
		else
		{
	        if (pNewRtcTime->tm_mday > 28)
	        {
	            pNewRtcTime->tm_mday = 1;
	        }			
		}
	    break;
		
	default:
		break;
	}
	
	return 0;
}

/***
  * 功能：
     	月份减少处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetMonReduce(int temp)
{
	pNewRtcTime->tm_mon--;
	if (pNewRtcTime->tm_mon < 1)
	{
		pNewRtcTime->tm_mon = 12;
	}
	//判断当前设置的月份天数是否符合要求，比如当前是5.31号，
	//那么月份加为6时day不能为显示31，2月也要特殊处理 
	switch(pNewRtcTime->tm_mon)
	{
	case 4:
	case 6:
	case 9:
	case 11:
		if (pNewRtcTime->tm_mday > 30)
	    {
	        pNewRtcTime->tm_mday = 1;
	    }
	    break;
	case 2:    
		if ( (pNewRtcTime->tm_year%4 == 0 && pNewRtcTime->tm_year%100 != 0) || 
			 (pNewRtcTime->tm_year%400 == 0) ) 
	    {
	        if (pNewRtcTime->tm_mday > 29)
	        {
	            pNewRtcTime->tm_mday = 1;
	        }
	    }
		else
		{
	        if (pNewRtcTime->tm_mday > 28)
	        {
	            pNewRtcTime->tm_mday = 1;
	        }			
		}
	    break;
		
	default:
		break;
	}
		
	return 0;
}

/***
  * 功能：
     	天数增加处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetDayAdd(int temp)
{
	pNewRtcTime->tm_mday++;
	switch (pNewRtcTime->tm_mon)
	{
	case 0://此处是为了修复一个超低概率bug其实是没有零月的
	case 1:     
	case 3:     
	case 5:   
	case 7:   
	case 8:
	case 10:
	case 12:
	    if (pNewRtcTime->tm_mday > 31)
	    {
	        pNewRtcTime->tm_mday = 1;
	    }
	    break;
	case 4:     
	case 6:     
	case 9:
	case 11:   
	    if (pNewRtcTime->tm_mday > 30)
	    {
	        pNewRtcTime->tm_mday = 1;
	    }
	    break;
	case 2:    
		if ( (pNewRtcTime->tm_year%4 == 0 && pNewRtcTime->tm_year%100 != 0) || 
			 (pNewRtcTime->tm_year%400 == 0) ) 
	    {
	        if (pNewRtcTime->tm_mday > 29)
	        {
	            pNewRtcTime->tm_mday = 1;
	        }
	    }
		else
		{
	        if (pNewRtcTime->tm_mday > 28)
	        {
	            pNewRtcTime->tm_mday = 1;
	        }			
		}
	    break;
	default:
	    break;
	}

	return 0;
}

/***
  * 功能：
     	天数减少处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetDayReduce(int temp)
{
	pNewRtcTime->tm_mday--;
	switch (pNewRtcTime->tm_mon)
	{
	case 0://此处是为了修复一个超低概率bug其实是没有零月的
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
	    if (pNewRtcTime->tm_mday < 1)
	    {
	        pNewRtcTime->tm_mday = 31;
	    }
	    break;
	case 4:
	case 6:
	case 9:
	case 11:   
	    if (pNewRtcTime->tm_mday < 1)
	    {
	        pNewRtcTime->tm_mday = 30;
	    }
	    break;
	case 2:    
		if ( (pNewRtcTime->tm_year%4 == 0 && pNewRtcTime->tm_year%100 != 0) || 
			 (pNewRtcTime->tm_year%400 == 0) ) 
	    {
	        if (pNewRtcTime->tm_mday < 1)
	        {
	            pNewRtcTime->tm_mday = 29;
	        }
	    }
		else
		{
	        if (pNewRtcTime->tm_mday < 1)
	        {
	            pNewRtcTime->tm_mday = 28;
	        }			
		}
	    break;
	default:
	    break;
	}
	
	return 0;
}

/***
  * 功能：
     	小时增加处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetHourAdd(void)
{
	pNewRtcTime->tm_hour++;
	if (pNewRtcTime->tm_hour > 23)
	{
		pNewRtcTime->tm_hour = 0;
	}
	
	TimeFlushDisplay(T_HOURS, pNewRtcTime->tm_hour, "%02d");
	
	return 0;
}

/***
  * 功能：
     	小时减少处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetHourReduce(void)
{	
	pNewRtcTime->tm_hour--;
	if (pNewRtcTime->tm_hour < 0)
	{
		pNewRtcTime->tm_hour = 23;
	}

	TimeFlushDisplay(T_HOURS, pNewRtcTime->tm_hour, "%02d");
	
	return 0;
}

/***
  * 功能：
     	分钟增加处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetMinAdd(void)
{
	pNewRtcTime->tm_min++;
	if (pNewRtcTime->tm_min > 59)
	{
		pNewRtcTime->tm_min = 0;
	}
		
	TimeFlushDisplay(T_MINUTE, pNewRtcTime->tm_min, "%02d");
	
	return 0;
}

/***
  * 功能：
     	分钟减少处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetMinReduce(void)
{
	pNewRtcTime->tm_min--;
	if (pNewRtcTime->tm_min <0)
	{
		pNewRtcTime->tm_min = 59;
	}
	
	TimeFlushDisplay(T_MINUTE, pNewRtcTime->tm_min, "%02d");
	
	return 0;
}

/***
  * 功能：
     	秒数增加处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetSecAdd(void)
{
	pNewRtcTime->tm_sec++;
	if (pNewRtcTime->tm_sec > 59)
	{
		pNewRtcTime->tm_sec = 0;
	}
	
	TimeFlushDisplay(T_SECOND, pNewRtcTime->tm_sec, "%02d");
	
	return 0;
}

/***
  * 功能：
     	秒数减少处理函数
  * 参数：
  		无
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int TimeSetSecReduce(void)
{
	pNewRtcTime->tm_sec--;
	if (pNewRtcTime->tm_sec < 0)
	{
		pNewRtcTime->tm_sec = 59;
	}
	
	TimeFlushDisplay(T_SECOND, pNewRtcTime->tm_sec, "%02d");
	
	return 0;
}

//日期调整按钮增加按下处理(包含年月日) 
static int TimeSetBtnDateAdd_Down(void *pInArg, int iInLen, 
                             	  void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;

	if (--iTmp > 2)
	{
		return -1;
	}

    if(!InstIsEncrypted()){
    	TouchChange("sysset_btn_up_press.bmp", pTimeSetBtnDateAdd[iTmp], 
    				NULL, NULL, 0);
        if(pCurSystemSet->uiTimeShowFlag == 0){
        	switch (iTmp)
        	{
        	case T_YEAR://年增加
        	    TimeSetDayAdd(0);
        		break;
        	case T_MONTH://月增加
        		TimeSetMonAdd(1);
        		break;
        	case T_DAY://天增加
        	    TimeSetYearAdd(2);
        		break;
        	default:
        		break;
        	}
        }
        if(pCurSystemSet->uiTimeShowFlag == 1){
        	switch (iTmp)
        	{
        	case T_YEAR://年增加
                TimeSetMonAdd(0);
        		break;
        	case T_MONTH://月增加
        		TimeSetDayAdd(1);
        		break;
        	case T_DAY://天增加
        	    TimeSetYearAdd(2);
        		break;
        	default:
        		break;
        	}
        }
        if(pCurSystemSet->uiTimeShowFlag == 2){
        	switch (iTmp)
        	{
        	case T_YEAR://年增加
        		TimeSetYearAdd(0);
        		break;
        	case T_MONTH://月增加
        		TimeSetMonAdd(1);
        		break;
        	case T_DAY://天增加
        		TimeSetDayAdd(2);
        		break;
        	default:
        		break;
        	}
        }
        FlushDate();
    	RefreshScreen(__FILE__, __func__, __LINE__);
    }
	return 0;
}

//日期调整按钮增加弹起处理(包含年月日) 
static int TimeSetBtnDateAdd_Up(void *pInArg, int iInLen, 
                              	void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;
	
	if (--iTmp > 2)
	{
		return -1;
	}

	TouchChange("sysset_btn_up_unpress.bmp", pTimeSetBtnDateAdd[iTmp], 
				NULL, NULL, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return 0;
}

//日期调整按钮增加弹起处理(包含年月日) 
static int TimeSetBtnDateReduce_Down(void *pInArg, int iInLen, 
                             		 void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;
	
	if (--iTmp > 2)
	{
		return -1;
	}
    if(!InstIsEncrypted()){
    	TouchChange("sysset_btn_dn_press.bmp", pTimeSetBtnDateReduce[iTmp], 
    				NULL, NULL, 0);
        if(pCurSystemSet->uiTimeShowFlag == 0){
        	switch (iTmp)
        	{
        	case T_YEAR://年减少
                TimeSetDayReduce(0);
        		break;
        	case T_MONTH://月减少
        		TimeSetMonReduce(1);
        		break;
        	case T_DAY://天减少
                TimeSetYearReduce(2);
        		break;
        	default:
        		break;
        	}
        }
        else if(pCurSystemSet->uiTimeShowFlag == 1){
        	switch (iTmp)
        	{
        	case T_YEAR://年减少
                TimeSetMonReduce(0);
        		break;
        	case T_MONTH://月减少
                TimeSetDayReduce(1);
        		break;
        	case T_DAY://天减少
                TimeSetYearReduce(2);
        		break;
        	default:
        		break;
        	}
        }
        else if(pCurSystemSet->uiTimeShowFlag == 2){
        	switch (iTmp)
        	{
        	case T_YEAR://年减少
        		TimeSetYearReduce(0);
        		break;
        	case T_MONTH://月减少
        		TimeSetMonReduce(1);
        		break;
        	case T_DAY://天减少
        		TimeSetDayReduce(2);
        		break;
        	default:
        		break;
        	}
        }
        FlushDate();
    	RefreshScreen(__FILE__, __func__, __LINE__);
    }
	return 0;
}

//日期调整按钮减少弹起处理(包含年月日) 
static int TimeSetBtnDateReduce_Up(void *pInArg, int iInLen, 
                             	   void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;
	
	if (--iTmp > 2)
	{
		return -1;
	}
	
	TouchChange("sysset_btn_dn_unpress.bmp", pTimeSetBtnDateReduce[iTmp], 
				NULL, NULL, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}

//时间调整按钮增加按下处理(包含时分秒) 
static int TimeSetBtnTimeAdd_Down(void *pInArg, int iInLen, 
                                  void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;
	
	if (--iTmp > 2)
	{
		return -1;
	}
	if(!InstIsEncrypted()){
    	TouchChange("sysset_btn_up_press.bmp", pTimeSetBtnTimeAdd[iTmp], 
    				NULL, NULL, 0);
    	switch (iTmp)
    	{
    	case T_HOURS://小时增加
    		TimeSetHourAdd();
    		break;
    	case T_MINUTE://分钟增加
    		TimeSetMinAdd();
    		break;
    	case T_SECOND://秒增加
    		TimeSetSecAdd();
    		break;

    	default:
    		break;
    	}	
    	 
    	RefreshScreen(__FILE__, __func__, __LINE__);
    }
	return 0;
}

//时间调整按钮增加弹起处理(包含时分秒) 
static int TimeSetBtnTimeAdd_Up(void *pInArg, int iInLen, 
                             	void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;
	
	if (--iTmp > 2)
	{
		return -1;
	}
	
	TouchChange("sysset_btn_up_unpress1.bmp", pTimeSetBtnTimeAdd[iTmp], 
				NULL, NULL, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return 0;
}

//时间调整按钮减少按下处理(包含时分秒) 
static int TimeSetBtnTimeReduce_Down(void *pInArg, int iInLen, 
                             		 void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;

	//检查参数
	if (--iTmp > 2)
	{
		return -1;
	}
    if(!InstIsEncrypted()){
    	TouchChange("sysset_btn_dn_press.bmp", pTimeSetBtnTimeReduce[iTmp], 
    				NULL, NULL, 0);
    	switch (iTmp)
    	{
    	case T_HOURS://小时减少
    		TimeSetHourReduce();
    		break;
    	case T_MINUTE://分钟减少
    		TimeSetMinReduce();
    		break;
    	case T_SECOND://秒减少
    		TimeSetSecReduce();
    		break;

    	default:
    		break;
    	}	 	
    	RefreshScreen(__FILE__, __func__, __LINE__);
    }
	return 0;
}

//时间调整按钮减少弹起处理(包含时分秒) 
static int TimeSetBtnTimeReduce_Up(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
	int iTmp = (UINT32)pOutArg;
	
	if (--iTmp > 2)
	{
		return -1;
	}

	TouchChange("sysset_btn_dn_unpress1.bmp", pTimeSetBtnTimeReduce[iTmp], 
				NULL, NULL, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return 0;
}

static int TimeSetYMD_Down(void *pInArg, int iInLen, 
                             	   void *pOutArg, int iOutLen)
{
    int iSelected = iOutLen;
    int i = 0;
    pCurSystemSet->uiTimeShowFlag = iSelected;
   	for (i = 0; i < 3; i++)
	{
		if(i == iSelected){
			SetPictureBitmap(BmpFileDirectory"sysData_set_press.bmp", pTimeSetYMDPicture[i]);
			DisplayPicture(pTimeSetYMDPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pTimeSetLblYMD[i]);
            DisplayLabel(pTimeSetLblYMD[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"sysData_set_unpress.bmp", pTimeSetYMDPicture[i]);
			DisplayPicture(pTimeSetYMDPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTimeSetLblYMD[i]);
            DisplayLabel(pTimeSetLblYMD[i]);
		}
	}
    
    FlushDate();
    RefreshScreen(__FILE__, __func__, __LINE__);
    
    return 0;
}
static int TimeSetYMD_Up(void *pInArg, int iInLen, 
                             	   void *pOutArg, int iOutLen)
{
    return 0;
}

//接受按钮按下处理函数 
static int TimeSetBtnApply_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
	if(!InstIsEncrypted()){
    	TouchChange("tmie_apply_unpress.bmp", pTimeSetBtnApply, 
    				pTimeSetStrApply, pTimeSetLblApply, 0);
    	RefreshScreen(__FILE__, __func__, __LINE__);
    }
	return 0;
}

//接受按钮弹起处理函数 
static int TimeSetBtnApply_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
	if(!InstIsEncrypted()){
    	TouchChange("tmie_apply_unpress1.bmp", pTimeSetBtnApply, 
    				pTimeSetStrApply, pTimeSetLblApply, 1);
    	//设置修改过的时间到系统中
    	SetCurTime(pNewRtcTime);
    	//同步存储修改过已生效的新时间
    	memcpy(pOldRtcTime, pNewRtcTime, sizeof(RTCVAL));
    	RefreshScreen(__FILE__, __func__, __LINE__);
    }
	return 0;
}
//菜单栏控件回调
static void TimeSetWndMenuCallBack(int selected)
{
	GUIWINDOW *pWnd = NULL;

	switch (selected)
	{
	case 0://调取待机和亮度窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmStandbysetInit, FrmStandbysetExit, 
		                    FrmStandbysetPaint, FrmStandbysetLoop, 
					        FrmStandbysetPause, FrmStandbysetResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmTimeSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调取语言设置窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmLanguageSetInit, FrmLanguageSetExit, 
							FrmLanguageSetPaint, FrmLanguageSetLoop, 
							FrmLanguageSetPause, FrmLanguageSetResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmTimeSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 3:	//调取wifi窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmWiFiInit, FrmWiFiExit, 
							FrmWiFiPaint, FrmWiFiLoop, 
							FrmWiFiPause, FrmWiFiResume,
							NULL);			
		SendWndMsg_WindowExit(pFrmTimeSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 4:	//调取软件升级窗体
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmSysMaintainInit, FrmSysMaintainExit, 
		                    FrmSysMaintainPaint, FrmSysMaintainLoop, 
					        FrmSysMaintainPause, FrmSysMaintainResume,
		                    NULL);          
		SendWndMsg_WindowExit(pFrmTimeSet);  
		SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 5://调取关于
	    #ifdef MINI2
        pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
							FrmAboutInit, FrmAboutExit, 
							FrmAboutPaint, FrmAboutLoop, 
							FrmAboutPause, FrmAboutResume,
							NULL);			        //pWnd由调度线程释放
		SendWndMsg_WindowExit(pFrmTimeSet);	        //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			    //发送消息以便调用新的窗体
		#endif
		break;		
	case BACK_DOWN:
	case HOME_DOWN://退出时间设置
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmMainInit, FrmMainExit, 
	                        FrmMainPaint, FrmMainLoop, 
					        FrmMainPause, FrmMainResume,
	                        NULL);          
	    SendWndMsg_WindowExit(pFrmTimeSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
			
	default:
		break;
	}
}
