/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcalendar.c
* 摘    要：  实现窗体wnd_frmcalendar(日期或时间)的窗体处理线程及相关操作函数。
*
* 当前版本：  v1.0.0
* 作    者： wjg 
* 完成日期：2020/9/1  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_frmcalendar.h"

#include "guiwindow.h"
#include "guipicture.h"
#include "guilabel.h"
#include "guibase.h"
#include "guimessage.h"
#include "guiglobal.h"

#include "app_frmtimeset.h"


#define CALENDAR_TITLE_NUM       3

typedef struct Calendar_Control
{
    GUIPICTURE* pFrmBtnUp[CALENDAR_TITLE_NUM];              //日期/时间递增按钮
    GUIPICTURE* pFrmBgDispaly[CALENDAR_TITLE_NUM];
    GUIPICTURE* pFrmBtnDown[CALENDAR_TITLE_NUM];            //日期/时间递减按钮
    GUICHAR* pFrmStrDisplay[CALENDAR_TITLE_NUM];
    GUILABEL* pFrmLblDisplay[CALENDAR_TITLE_NUM];           //显示日期/时间
    GUICHAR* pFrmStrTitle[CALENDAR_TITLE_NUM];
    GUILABEL* pFrmLblTitle[CALENDAR_TITLE_NUM];             //标题栏（年月日或时分秒）
    GUICHAR* pFrmStrFormat[CALENDAR_TITLE_NUM-1];           
    GUILABEL* pFrmLblFormat[CALENDAR_TITLE_NUM-1];          //日期：‘-’，时间：‘：’
}CALENDAR_CONTROL;

/**********************************************************************************
**							窗体frmcalendar中的控件定义部分						 **
***********************************************************************************/
static GUIWINDOW *pFrmCalendar = NULL;					        //窗体控件

/**********************************背景控件定义************************************/ 
static GUIPICTURE *pFrmCalendarSetBg = NULL;					//背景图片

/******************************窗体按钮控件定义**********************************/

GUIPICTURE* pFrmCalendarBtnOK = NULL;
GUIPICTURE* pFrmCalendarBtnCancel = NULL;

GUILABEL* pFrmCalendarLblOK = NULL;
GUILABEL* pFrmCalendarLblCancel = NULL;

GUICHAR* pFrmCalendarStrOK = NULL;
GUICHAR* pFrmCalendarStrCancel = NULL;

static CALENDAR_CONTROL* pFrmCalendarCalendarControl = NULL;

static int iCalendarX = 0;                                      //窗体左上角横坐标
static int iCalendarY = 0;                                      //窗体左上角纵坐标
static int iFocus = 0;											//光标的焦点位置,默认是左上角第一个（增加Y或H）
static int iOKCancel = 0;										//选中取消或确定（1：Cancel， 2：OK）
static CALENDAR_TYPE gCalendarType = CALEBDAR_DATE;             //默认类型是日期
static CALLLBACKWINDOW CallBackFunc = NULL;                     //窗体回调函数

static RTCVAL* pOldRtcVal = NULL;                               //用以保存时钟参数
static RTCVAL* pNewRtcVal = NULL;                               //用以临时修改时钟参数
/**********************************************************************************
**	    	窗体frmcalendar中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/
static int CalendarTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int CalendarTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/**********************************************************************************
**			    	窗体frmcalendar中的控件事件处理函数定义部分					 **
***********************************************************************************/
static int CalendarBtnAdd_Down(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);
static int CalendarBtnAdd_Up(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);

static int CalendarBtnReduce_Down(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);
static int CalendarBtnReduce_Up(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);

/***************************窗体的按键事件处理函数********************************/
static int CalendarWndKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CalendarWndKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/************************点击按钮事件处理函数******************************/
static int CalendarBtnOk_Down(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);
static int CalendarBtnOk_Up(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);

static int CalendarBtnCancel_Down(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);
static int CalendarBtnCancel_Up(void *pInArg,   int iInLen, void *pOutArg, int iOutLen);

/**********************************************************************************
**			    	窗体frmcalendar中的功能函数定义部分							 **
***********************************************************************************/
//初始化日期/时间参数
static int InitialCalendarPara(void);
//清除日期/时间参数
static int ClearCalendarPara(void);
//设置显示区域的内容（日期/时间）
static void SetDisplayContent(void);
//刷新日期/时间显示
static void FlushCalendar(void);
/**********************************************************************************
**			    	窗体frmcalendar中的按键功能函数定义部分						 **
***********************************************************************************/
//获取按键焦点位置
static void SetKeyFocus(int iOldFocus, int iOption);
//刷新按键显示
static void FlushKeyDisplay(void);
//处理Enter响应函数
static void HandleEnterKeyCallBack(void);

/***
  * 功能：
        窗体frmcalendar的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
static int FrmCalendarInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/    
    if(pFrmCalendarCalendarControl== NULL)
    {
        pFrmCalendarCalendarControl= (CALENDAR_CONTROL *)calloc(1, sizeof(CALENDAR_CONTROL));
    }

    InitialCalendarPara();
    
    CalendarTextRes_Init(NULL, 0, NULL, 0);
    //建立窗体控件
    /****************************************************************/
    //建立桌面上的控件
    pFrmCalendarSetBg = CreatePicture(iCalendarX, iCalendarY, 524, 280, BmpFileDirectory "bg_time_date.bmp");
    
    if(pFrmCalendarCalendarControl!= NULL)
    {
        for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
        {
            pFrmCalendarCalendarControl->pFrmBtnUp[i] = CreatePicture(iCalendarX+ 82 + i*160, iCalendarY+ 37,
                                                                    40, 30, BmpFileDirectory"sysset_btn_up_unpress1.bmp");            
            pFrmCalendarCalendarControl->pFrmBgDispaly[i] = CreatePicture(iCalendarX+ 49 + i*160, iCalendarY+ 87,
                                                                    106, 38, BmpFileDirectory"sysset_btn_time_unpress1.bmp");
            pFrmCalendarCalendarControl->pFrmBtnDown[i] = CreatePicture(iCalendarX+ 82 + i*160, iCalendarY+ 137,
                                                                    40, 30, BmpFileDirectory"sysset_btn_down_unpress1.bmp"); 
            pFrmCalendarCalendarControl->pFrmLblTitle[i] = CreateLabel(iCalendarX+ 82 + i*160, iCalendarY+ 10,
                                                                    40, 16, pFrmCalendarCalendarControl->pFrmStrTitle[i]);
            pFrmCalendarCalendarControl->pFrmLblDisplay[i] = CreateLabel(iCalendarX+ 49 + i*160+3, iCalendarY+ 87+11,
                                                                    100, 16, pFrmCalendarCalendarControl->pFrmStrDisplay[i]);

            if(i < CALENDAR_TITLE_NUM - 1)
            {
                pFrmCalendarCalendarControl->pFrmLblFormat[i] = CreateLabel(iCalendarX+ 79 + i*160, iCalendarY+ 50,
                                                                        20, 16, pFrmCalendarCalendarControl->pFrmStrFormat[i]);
				SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmCalendarCalendarControl->pFrmLblFormat[i]);
            }
            
            SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmCalendarCalendarControl->pFrmLblTitle[i]);
            SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmCalendarCalendarControl->pFrmLblDisplay[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmCalendarCalendarControl->pFrmLblTitle[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmCalendarCalendarControl->pFrmLblDisplay[i]);
        }
    }

    pFrmCalendarBtnOK= CreatePicture(iCalendarX+ 287, iCalendarY+ 207, 140, 46, BmpFileDirectory"btn_dialog_unpress.bmp");
    pFrmCalendarBtnCancel= CreatePicture(iCalendarX+ 97, iCalendarY+ 207, 140, 46, BmpFileDirectory"btn_dialog_unpress.bmp");
    
    pFrmCalendarLblOK= CreateLabel(iCalendarX + 287+20, iCalendarY + 207+15, 100, 16, pFrmCalendarStrOK);
    pFrmCalendarLblCancel= CreateLabel(iCalendarX + 97+20, iCalendarY + 207+15, 100, 16, pFrmCalendarStrCancel);

    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmCalendarLblOK);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmCalendarLblCancel);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmCalendarLblOK);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmCalendarLblCancel);

    //得到当前窗体对象
    pFrmCalendar = (GUIWINDOW *) pWndObj;
    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmCalendar,pFrmCalendar);

    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                                pFrmCalendarCalendarControl->pFrmBtnUp[i], pFrmCalendar);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                                pFrmCalendarCalendarControl->pFrmBtnDown[i], pFrmCalendar);
    }
    
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                                pFrmCalendarBtnOK, pFrmCalendar);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                                pFrmCalendarBtnCancel, pFrmCalendar);

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    GUIMESSAGE *pMsg = NULL;
    pMsg = GetCurrMessage();

    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmCalendarCalendarControl->pFrmBtnUp[i], 
                                        CalendarBtnAdd_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmCalendarCalendarControl->pFrmBtnUp[i], 
                                        CalendarBtnAdd_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmCalendarCalendarControl->pFrmBtnDown[i], 
                                        CalendarBtnReduce_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmCalendarCalendarControl->pFrmBtnDown[i], 
                                        CalendarBtnReduce_Down, NULL, i, pMsg);
    }

    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmCalendar, 
                                        CalendarWndKey_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmCalendar, 
                                        CalendarWndKey_Up, NULL, 0, pMsg);
	
    //注册桌面上控件的消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmCalendarBtnOK, 
                                        CalendarBtnOk_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmCalendarBtnOK, 
                                        CalendarBtnOk_Up, NULL, 0, pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmCalendarBtnCancel, 
                                        CalendarBtnCancel_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmCalendarBtnCancel, 
                                        CalendarBtnCancel_Up, NULL, 0, pMsg);

    
    return iReturn;
}

/***
  * 功能：
        窗体frmcalendar的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
static int FrmCalendarExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    
    //清空消息队列中的消息注册项
    //***************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);
    //得到当前窗体对象
    pFrmCalendar = (GUIWINDOW *) pWndObj;
    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmCalendar);
    DestroyPicture(&pFrmCalendarSetBg);
    DestroyPicture(&pFrmCalendarBtnOK);
    DestroyPicture(&pFrmCalendarBtnCancel);
    
    DestroyLabel(&pFrmCalendarLblOK);
    DestroyLabel(&pFrmCalendarLblCancel);
    int i;
    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
        DestroyPicture(&(pFrmCalendarCalendarControl->pFrmBtnUp[i]));
        DestroyPicture(&(pFrmCalendarCalendarControl->pFrmBtnDown[i]));
        DestroyPicture(&(pFrmCalendarCalendarControl->pFrmBgDispaly[i]));
        DestroyLabel(&(pFrmCalendarCalendarControl->pFrmLblTitle[i]));
        DestroyLabel(&(pFrmCalendarCalendarControl->pFrmLblDisplay[i]));
        
        if(i < CALENDAR_TITLE_NUM - 1)
        {
            DestroyLabel(&(pFrmCalendarCalendarControl->pFrmLblFormat[i]));
        }
    }
    
    CalendarTextRes_Exit(NULL, 0, NULL, 0);
    ClearCalendarPara();

    return iReturn;
}

/***
  * 功能：
        窗体frmcalendar的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmCalendarPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

	/* 绘制无效区域窗体 */
	DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    DisplayPicture(pFrmCalendarSetBg);
    DisplayPicture(pFrmCalendarBtnOK);
    DisplayPicture(pFrmCalendarBtnCancel);
    DisplayLabel(pFrmCalendarLblOK);
    DisplayLabel(pFrmCalendarLblCancel);

    int i;
    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
		if (i == 0)
		{
			SetPictureBitmap(BmpFileDirectory"sysset_btn_up_select.bmp", pFrmCalendarCalendarControl->pFrmBtnUp[0]);
		}
        DisplayPicture(pFrmCalendarCalendarControl->pFrmBtnUp[i]);
        DisplayPicture(pFrmCalendarCalendarControl->pFrmBtnDown[i]);
        DisplayPicture(pFrmCalendarCalendarControl->pFrmBgDispaly[i]);
        DisplayLabel(pFrmCalendarCalendarControl->pFrmLblTitle[i]);
        DisplayLabel(pFrmCalendarCalendarControl->pFrmLblDisplay[i]);
        
        if(i < CALENDAR_TITLE_NUM - 1)
        {
            // DisplayLabel(pFrmCalendarCalendarControl->pFrmLblFormat[i]);
        }
    }


    SetPowerEnable(0, 0);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

/***
  * 功能：
        窗体frmcalendar的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmCalendarLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    
    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj); 
    
    return iReturn;
}

/***
  * 功能：
        窗体frmcalendar的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmCalendarPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    
    return iReturn;
}

/***
  * 功能：
        窗体frmcalendar的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
static int FrmCalendarResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    
    return iReturn;
}

/**********************************************************************************
**	    	窗体frmcalendar中的初始化文本资源、 释放文本资源函数定义部分			 **
***********************************************************************************/
static int CalendarTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;    
    pFrmCalendarStrOK = GetCurrLanguageText(SYSTEM_LBL_CONFIRM);
    pFrmCalendarStrCancel = GetCurrLanguageText(SYSTEM_LBL_CANCEL);

    char* tempDateTitle[CALENDAR_TITLE_NUM] = {"Y","M", "D"};
    char* tempTimeTitle[CALENDAR_TITLE_NUM] = {"H","M", "S"};
    char* tempFormat[CALENDAR_TITLE_NUM-1] = {"-", ":"};
    int i;
    
    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
        pFrmCalendarCalendarControl->pFrmStrTitle[i] = (gCalendarType== CALEBDAR_DATE) 
                                                            ? TransString(tempDateTitle[i])
                                                            : TransString(tempTimeTitle[i]);
        if(i < CALENDAR_TITLE_NUM-1)
        {
            pFrmCalendarCalendarControl->pFrmStrFormat[i] = TransString(tempFormat[gCalendarType]);
        }
    }

    SetDisplayContent();
    
    return iReturn;
}

static int CalendarTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    GuiMemFree(pFrmCalendarStrOK);
    GuiMemFree(pFrmCalendarStrCancel);

    int i;
    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
        GuiMemFree(pFrmCalendarCalendarControl->pFrmStrDisplay[i]);
        GuiMemFree(pFrmCalendarCalendarControl->pFrmStrTitle[i]);
        if(i < CALENDAR_TITLE_NUM-1)
        {
            GuiMemFree(pFrmCalendarCalendarControl->pFrmStrFormat[i]);
        }
    }
    
    return iReturn;
}

/**********************************************************************************
**			    	窗体frmcalendar中的控件事件处理函数定义部分					 **
***********************************************************************************/
static int CalendarBtnAdd_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    if(gCalendarType== CALEBDAR_DATE)
    {
        ModifyDate(pNewRtcVal, 0, iOutLen);
    }
    else
    {
        ModifyTime(pNewRtcVal, 0, iOutLen);
    }
    
    return iReturn;
}

static int CalendarBtnAdd_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    FlushCalendar();
    //刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

static int CalendarBtnReduce_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    if(gCalendarType== CALEBDAR_DATE)
    {
        ModifyDate(pNewRtcVal, 1, iOutLen);
    }
    else
    {
        ModifyTime(pNewRtcVal, 1, iOutLen);
    }
    
    return iReturn;
}

static int CalendarBtnReduce_Up(void *pInArg,   int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    FlushCalendar();
    //刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);
    
    return iReturn;
}

/***************************窗体的按键事件处理函数********************************/
static int CalendarWndKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    
    return iReturn;
}

static int CalendarWndKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
    switch (uiValue)
    {
    case KEYCODE_UP:
        SetKeyFocus(iFocus, 0);
        FlushKeyDisplay();
        break;
    case KEYCODE_DOWN:
        SetKeyFocus(iFocus, 1);
        FlushKeyDisplay();
        break;
    case KEYCODE_LEFT:
        SetKeyFocus(iFocus, 2);
        FlushKeyDisplay();
        break;
    case KEYCODE_RIGHT:
        SetKeyFocus(iFocus, 3);
        FlushKeyDisplay();
        break;
    case KEYCODE_ENTER:
        HandleEnterKeyCallBack();
        break;
    case KEYCODE_ESC:
        CalendarBtnCancel_Up(NULL, 0, NULL, 0);
        break;
    default:
        break;
    }

    RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}

/************************点击按钮事件处理函数******************************/
static int CalendarBtnOk_Down(void *pInArg,   int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //设置修改过的时间到系统中
	SetCurTime(pNewRtcVal);
	//同步存储修改过已生效的新时间
	memcpy(pOldRtcVal, pNewRtcVal, sizeof(RTCVAL));

	TouchChange("btn_dialog_press.bmp", pFrmCalendarBtnOK, NULL, pFrmCalendarLblOK, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);

    
    return iReturn;
}

static int CalendarBtnOk_Up(void *pInArg,   int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    GUIWINDOW *pWnd = NULL;
    
    if(CallBackFunc)
    {
        (*CallBackFunc)(&pWnd);
        SendWndMsg_WindowExit(pFrmCalendar);      //发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
    }
    
    return iReturn;
}

static int CalendarBtnCancel_Down(void *pInArg,   int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

	TouchChange("btn_dialog_press.bmp", pFrmCalendarBtnCancel, NULL, pFrmCalendarLblCancel, 1);
	//刷新屏幕缓存
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return iReturn;
}

static int CalendarBtnCancel_Up(void *pInArg,   int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //临时变量定义
    GUIWINDOW *pWnd = NULL;
    
    if(CallBackFunc)
    {
        (*CallBackFunc)(&pWnd);
        SendWndMsg_WindowExit(pFrmCalendar);      //发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
    }
    
    return iReturn;
}

/**********************************************************************************
**			    	窗体frmcalendar中的功能函数定义部分					 **
***********************************************************************************/
//初始化日期/时间参数
static int InitialCalendarPara(void)
{
    //获取当前时间 
	pNewRtcVal = GetCurTime();
	pOldRtcVal= (RTCVAL *)GuiMemAlloc(sizeof(RTCVAL));
    
	if (pOldRtcVal == NULL)
	{
		return -1;
	}
    
	memcpy(pOldRtcVal, pNewRtcVal, sizeof(RTCVAL));

	return 0;
}

//清除日期/时间参数
static int ClearCalendarPara(void)
{
    GuiMemFree(pNewRtcVal);
	GuiMemFree(pOldRtcVal);
	//清除全局变量
	iFocus = 0;
	iOKCancel = 0;

	return 0;
}

//设置显示区域的内容（日期/时间）
static void SetDisplayContent(void)
{
    int date[CALENDAR_TITLE_NUM] = {pNewRtcVal->tm_year, pNewRtcVal->tm_mon, pNewRtcVal->tm_mday};
    int time[CALENDAR_TITLE_NUM] = {pNewRtcVal->tm_hour, pNewRtcVal->tm_min, pNewRtcVal->tm_sec};
    char tempDisplay[10] = {0};  
    int i;
    
    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
        if(gCalendarType== CALEBDAR_DATE)
        {
            if(i == T_YEAR)
            {
                sprintf(tempDisplay, "%04d", date[i]);
            }
            else
            {
                sprintf(tempDisplay, "%02d", date[i]);
            }
        }
        else
        {
            sprintf(tempDisplay, "%02d", time[i]);
        }
        
        pFrmCalendarCalendarControl->pFrmStrDisplay[i] = TransString(tempDisplay);
        memset(tempDisplay, 0, 10);
    }
}

//刷新日期/时间显示
static void FlushCalendar(void)
{
    SetDisplayContent();
    int i;
    for(i = 0; i < CALENDAR_TITLE_NUM; ++i)
    {
        SetLabelText(pFrmCalendarCalendarControl->pFrmStrDisplay[i], 
                        pFrmCalendarCalendarControl->pFrmLblDisplay[i]);
        DisplayPicture(pFrmCalendarCalendarControl->pFrmBgDispaly[i]);
        DisplayLabel(pFrmCalendarCalendarControl->pFrmLblDisplay[i]);
    }
}

//获取按键焦点位置
static void SetKeyFocus(int iOldFocus, int iOption)
{
	switch (iOption)
	{
	case 0://up
		if (iOldFocus >= 3)
		{
			if (iOKCancel != 0)
			{
				iOKCancel = 0;
			}
			else
			{
				iOldFocus -= 3;
			}
		}
		break;
	case 1://down
		if (iOldFocus < 3)
		{
			iOldFocus += 3;
		}
		else
		{
			iOKCancel = 1;
		}
		break;
	case 2://left
		if (iOKCancel != 0)
		{
			(iOKCancel == 1) ? (iOKCancel = 2) : (iOKCancel = 1);
		}
		else
		{
			if (iOldFocus == 0)
			{
				iOldFocus = 5;
			}
			else
			{
				iOldFocus--;
			}
		}
		break;
	case 3://right
		if (iOKCancel != 0)
		{
			(iOKCancel == 1) ? (iOKCancel = 2) : (iOKCancel = 1);
		}
		else
		{
			if (iOldFocus == 5)
			{
				iOldFocus = 0;
			}
			else
			{
				iOldFocus++;
			}
		}
		break;
	default:
		break;
	}

	iFocus = iOldFocus;
}

//刷新按键显示
static void FlushKeyDisplay(void)
{
	int i;
	for (i = 0; i < CALENDAR_TITLE_NUM; ++i)
	{
		TouchChange("sysset_btn_up_unpress1.bmp", pFrmCalendarCalendarControl->pFrmBtnUp[i],
			NULL, NULL, 0);
		TouchChange("sysset_btn_down_unpress1.bmp", pFrmCalendarCalendarControl->pFrmBtnDown[i],
			NULL, NULL, 0);
	}

	TouchChange("btn_dialog_unpress.bmp", pFrmCalendarBtnCancel,
					pFrmCalendarStrCancel, pFrmCalendarLblCancel, 1);
	TouchChange("btn_dialog_unpress.bmp", pFrmCalendarBtnOK,
					pFrmCalendarStrOK, pFrmCalendarLblOK, 1);

	if ((iFocus < CALENDAR_TITLE_NUM) && (iFocus >= 0))
	{
		TouchChange("sysset_btn_up_select.bmp", pFrmCalendarCalendarControl->pFrmBtnUp[iFocus],
			NULL, NULL, 0);
	}
	else
	{
		if (iOKCancel == 1)
		{
			TouchChange("btn_dialog_press.bmp", pFrmCalendarBtnCancel,
				pFrmCalendarStrCancel, pFrmCalendarLblCancel, 1);
			TouchChange("btn_dialog_unpress.bmp", pFrmCalendarBtnOK,
				pFrmCalendarStrOK, pFrmCalendarLblOK, 1);
		}
		else if (iOKCancel == 2)
		{
			TouchChange("btn_dialog_unpress.bmp", pFrmCalendarBtnCancel,
				pFrmCalendarStrCancel, pFrmCalendarLblCancel, 1);
			TouchChange("btn_dialog_press.bmp", pFrmCalendarBtnOK,
				pFrmCalendarStrOK, pFrmCalendarLblOK, 1);
		}
		else
		{
			if (iFocus >= CALENDAR_TITLE_NUM && iFocus <= (CALENDAR_TITLE_NUM+2))
			{
				TouchChange("sysset_btn_down_select.bmp", pFrmCalendarCalendarControl->pFrmBtnDown[iFocus - 3],
					NULL, NULL, 0);
			}
		}
	}
}

//处理Enter响应函数
static void HandleEnterKeyCallBack(void)
{
	if (!iOKCancel)
	{
		if (iFocus < CALENDAR_TITLE_NUM)
		{
			CalendarBtnAdd_Down(NULL, 0, NULL, iFocus);
			CalendarBtnAdd_Up(NULL, 0, NULL, 0);
		}
		else
		{
			CalendarBtnReduce_Down(NULL, 0, NULL, iFocus-3);
			CalendarBtnReduce_Up(NULL, 0, NULL, 0);
		}
	}
	else//1:Cancel,2:OK
	{
		if (iOKCancel == 1)
		{
			CalendarBtnCancel_Down(NULL, 0, NULL, 0);
			CalendarBtnCancel_Up(NULL, 0, NULL, 0);
		}
		else
		{
			CalendarBtnOk_Down(NULL, 0, NULL, 0);
			CalendarBtnOk_Up(NULL, 0, NULL, 0);
		}
	}
}

/***
  * 功能：
        外部调用函数显示日历（日期/时间）窗体
  * 参数：
		1.int x : 	                  对话框左上角的横坐标
		2.int y : 	                  对话框左上角的纵坐标
  		3.CALENDAR_TYPE calendarType：0：date，1：time
  		4.CALLLBACKWINDOW func：       窗体回调函数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int CalendarInit(int x, int y, CALENDAR_TYPE calendarType, CALLLBACKWINDOW func)
{
    //错误标志、返回值定义
    int iReturn = 0;

    iCalendarX = x;
    iCalendarY = y;
    gCalendarType= calendarType;	
	CallBackFunc = func;
    
    GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(x, y, 226, 120,
                        FrmCalendarInit , FrmCalendarExit, 
                        FrmCalendarPaint, FrmCalendarLoop, 
					    FrmCalendarPause, FrmCalendarResume,
                        NULL);          				//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());				//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);						//发送消息以便调用新的窗体
  
    return iReturn;
}