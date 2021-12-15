/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdr.c
* 摘    要：  实现主窗体frmotdr的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/19
*
*******************************************************************************/

#include "wnd_frmotdr.h"

/*******************************
*定义wnd_frmotdr.c引用其他头文件
*******************************/
#include "app_frminit.h"
#include "app_frmotdr.h"
#include "app_filesor.h"
#include "app_screenshots.h"
#include "app_inno.h"
#include "app_frmotdrtime.h"
#include "app_measuringresult.h"
#include "app_queue.h"
#include "app_log.h"
#include "app_pdfreportotdr.h"
#include "app_frmotdrmessage.h"

#include "guiphoto.h"

#include "wnd_frmcoor.h"
#include "wnd_frmeventtab.h"
#include "wnd_frmlinkmap.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmevtbtn.h"
#include "wnd_frmcommonset.h"
#include "wnd_frmotdrsave.h"
#include "wnd_frmchecklight.h"
#include "wnd_frmmark.h"
#include "wnd_frmdialog.h"
#include "wnd_messagebox.h"
#include "wnd_saveasdialog.h"
#include "wnd_frmcurvepara.h"
#include "wnd_frmcurveresult.h"
#include "wnd_frmsavefile.h"
#include "wnd_frmotdrfileopen.h"

/*****************************
*定义wnd_frmotdr.c使用的宏定义
*****************************/
#define		ENABLE		1
#define		DISABLE		0

/******************************
*定义wnd_frmotdr.c使用的GUI控件
*******************************/
static GUIWINDOW *pFrmOtdr = NULL;						//整个窗体

static OTDRCOOR *pOtdrCoor = NULL;						//坐标系控件
//static WNDOTDRBOTTOM *pOtdrSet = NULL;				//普通模式otdr设置控件
static OTDR_CURVE_PARA* pOtdrCurvePara = NULL;			//otdr曲线参数界面控件
static struct Curve_Result* pOtdrCurveResult = NULL;	//db界面控件
static EVENTTAB *pEvtTab = NULL;						//事件控件
static OTDR_LINK_MAP *pOtdrLinkMap = NULL;				//链路地图控件
//static WNDMENU1 *pOtdrMenu = NULL;						//menu控件
static GUIPICTURE *pScreenShot = NULL;					//截图按钮
static CALLLBACKWINDOW pOtdrEscapeTo  = NULL;
extern char* pOtdrFileCurrPath;
static int iReflushMarkedFlag = 0;						//刷新标记线信息
static int iRepaint = 0;								//调用刷新曲线，只有在进入界面和退出界面，该参数置为0，测试过程中置为1
static int iPressKeyArray[2] = {0};                     //检测长按移动标记线存储标志位
/*********************************
*声明wnd_frmotdr.c所引用的外部变量
*********************************/
extern CURR_WINDOW_TYPE enCurWindow;
/*********************************
*定义wnd_frmotdr.c使用内部函数声明
*********************************/
static int OtdrWindow_Down(void *pInArg, int iInLen,
                           void *pOutArg, int iOutLen);
static int OtdrWindow_Move(void *pInArg, int iInLen,
                           void *pOutArg, int iOutLen);
static int OtdrWindow_Up(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen);
static int OtdrWindowMT(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen);
//按键处理响应函数
static int OtdrWndKey_Down(void *pInArg, int iInLen,
						   void *pOutArg, int iOutLen);
static int OtdrWndKey_Up(void *pInArg, int iInLen,
						   void *pOutArg, int iOutLen);
//读取长按移动标志位信息
static void ReadMoveMarkerMsg(void);
static int OtdrWndKey_Press(void *pInArg, int iInLen,
						 void *pOutArg, int iOutLen);
//取消长按按键处理（移动标记线）
static int OtdrWndKey_CancelPress(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen);

static int OtdrOptionOtdr(void *pInArg, int iInLen,
                          void *pOutArg, int iOutLen);
static int OtdrOptionEvt(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen);
static int OtdrOptionLinkMap(void *pInArg, int iInLen,
						 void *pOutArg, int iOutLen);
static int OtdrOptionMeas(void *pInArg, int iInLen,
                          void *pOutArg, int iOutLen);

static int OtdrBackHome(void *pInArg, int iInLen,
						  void *pOutArg, int iOutLen);
static int OtdrSetting(void *pInArg, int iInLen,
	                      void *pOutArg, int iOutLen);
static int OtdrSwitchWave(void *pInArg, int iInLen,
						  void *pOutArg, int iOutLen);
// static int OtdrOpenFile(void *pInArg, int iInLen,
// 						void *pOutArg, int iOutLen);
static int OtdrFile(void *pInArg, int iInLen,
						  void *pOutArg, int iOutLen);

// static int OtdrScreenShot_Down(void *pInArg, int iInLen,
//                           	   void *pOutArg, int iOutLen);
// static int OtdrScreenShot_Up(void *pInArg, int iInLen,
//                              void *pOutArg, int iOutLen);

//设置画曲线标志位
static void SetDrawFlgs(int iCsrFlg, int iEvtFlg,
						int iMakFlg, int iRefFlg,
						int iEventMarkerFlag);
//切换选项卡
static void SwitchOptionCard(void *arg, OPC_PAINT paint,
							 OPC_SETUP setup, int iCurrCard);
//设置OTDR选项卡
static int OtdrOpcOtdr(void);
//事件选项卡
static int OtdrOpcEvt(void);
//链路地图选项卡
static int OtdrOpcLinkMap(void);
//测量选项卡
static int OtdrOpcMeas(void);
//重绘窗体
static void CreateOtdrWindow(GUIWINDOW **pWnd);
//menu回调
static void OtdrMenuCall(int iOp);
//工具栏回调
 //static void OtdrToolBar(int iOp);
//光标按下处理函数
static void OtdrSetCusor(void);
//手型按下处理函数
//static void OtdrSetDrag(void);
//局部放大按下处理函数
//static void OtdrAreaZoom(void);
//桌面上1:1按钮按下处理函数
//static void OtdrZoom11(void);

//桌面上Zoom In按钮弹起处理函数
static void OtdrZoomIn_x(void);
//桌面上Zoom Out按钮弹起处理函数
static void OtdrZoomOut_x(void);
//桌面上Zoom In按钮弹起处理函数
static void OtdrZoomIn_y(void);
//桌面上Zoom Out按钮弹起处理函数
static void OtdrZoomOut_y(void);

//切换伸展模式和普通模式
static void OtdrSwitchModule(void);
//初始化参数
static void OtdrInitPara(void);
//读取到队列中有WAVWARGS时响应函数
static void RecvWaveArgs(void);
//读取到队列中有ENUM_OTDRMSG_STOP时响应函数
static void RecvStop(void);
static void CheckLightCallBack(int iSelect);
//光纤连接检查回调函数
static void CheckConnectCallBack(int iSelect);
static void SaveAllCurves(PDISPLAY_INFO pDisplay);
//刷新波形区域
//static void RefreshCurve(int value);
//设置显示文件名或显示标记线信息
static void isFlushDisplayInfo(int iType, int* iCount);
//测试otdr
// static void OtdrTest(void);
//选项响应函数
static void SelectedOptionCallBack(int iSelectedOption, int iOption);
//处理enter键响应函数
static void HandleEnterKeyCallBack(void);
//移动标记线
static void MoveMarker(int index, int option, int iValue);
//缩放曲线
static void ZoomCurve(int iMarker, int iInOrOut, float fXScale, float fYScale);
//上键响应处理
static void HandleKeyUpDown(void);
//下键响应处理
static void HandleKeyDownDown(void);
//左移响应处理
static void HandleKeyLeftDown(void);
//右移响应处理
static void HandleKeyRightDown(void);

/*******************************
*定义wnd_frmotdr.c使用的内部变量
*******************************/
static int iEvtTabCol = 4;					//当前页面事件表的行数
static int iOtdrTouch = 0;					//屏幕选取标志位
static OTDR_RECT SelRect;					//选择区域的结构
static int iCoorFocus = 0;					//坐标系焦点位置
static int iSelectedOption = 0;				//菜单栏选项焦点位置
static PDISPLAY_INFO pDisplay = NULL;

static INT32 iMTLastX;
static INT32 iMTLastY;

static int iScaleFlag = 0;					//缩放的标志位（shift+up+down+left+right）

static OTDROPC sOtdrOpCard = {				//定义otdr选项卡数据
	.pOpc = NULL,
	.pOpcPaint = NULL,
	.pOpcSetup = NULL,
	.iCurrCard = OTDR_OPC_SETUP,
	.iCurrWnd = COOR_LARGE,
};

static OTDR_WIN_PARA sWinPara = {
	.enCursorType = EN_TCURSOR,
	.enZoomMode = EN_NORMAL,
};

static MSGFUNC ppFunc[OPTION_NUM + FUNCTION_NUM] = {
	OtdrOptionOtdr, OtdrOptionEvt,
	OtdrOptionMeas, OtdrOptionLinkMap,
	OtdrSetting, OtdrFile, OtdrSwitchWave};

//初始化需要的资源
// static unsigned int strMenu[] = {
// 	1
// };

void PrintMemInfo()
{
	FILE *fp = fopen("/proc/meminfo", "r");
	if (fp)
	{
		char buffer[512];
		printf("======================MEM INFO===================\n");
		fgets(buffer, 512, fp);
		printf(buffer);
		fgets(buffer, 512, fp);
		printf(buffer);
		fgets(buffer, 512, fp);
		printf(buffer);
		fgets(buffer, 512, fp);
		printf(buffer);
		fgets(buffer, 512, fp);
		printf(buffer);
		printf("======================MEM INFO===================\n");

		fclose(fp);
	}
}

void PressureTest()
{
	static int iCnt = 0;

	srand(time(NULL));

	pUser_Settings->sFrontSetting[0].enFiberRange = rand() % 11;
	pUser_Settings->sFrontSetting[0].enPulseTime = 0;

	LOG(LOG_INFO, "\t Run times = %d\n", iCnt++);

	PrintMemInfo();

	RunOtdrModule(1);
}

/***
  * 功能：
        窗体的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/
int FrmOtdrInit(void *pWndObj)
{
	//错误标志，返回值定义
    int iErr = 0;
	int iTmp = 0;
	char strBmpFile[512];

	//初始化参数
	enCurWindow = ENUM_OTDR_WIN;	//设置当前窗体
    pFrmOtdr = (GUIWINDOW *) pWndObj;
	OtdrInitPara();
	//创建坐标系控件
	pOtdrCoor = CreateOtdrCoor(ppFunc, OtdrWindow_Down, OtdrWindow_Move,
							   OtdrWindow_Up, OtdrWindowMT, iCoorFocus, pFrmOtdr,
							   sOtdrOpCard.iCurrWnd);
	pOtdrCurvePara = CreateOtdrCurvePara();
	pOtdrCurveResult = CreateCurveResult(0, 305);
	pEvtTab = CreateEventTab(iEvtTabCol, pFrmOtdr);
	//链路地图选项
	pOtdrLinkMap = CreateOtdrLinkMap(0, 305, 652, 175, pFrmOtdr);
	handleEventList(pOtdrLinkMap, GetEvtTabEvents());

	pDisplay = pOtdrTopSettings->pDisplayInfo;

	//创建侧边栏menu
	if ((CurveIsVaild(CURR_CURVE, pDisplay)) &&
		(2 ==  GetCurveNum(pDisplay)))						//有两条曲线
		iTmp = 6;
	else if (CurveIsVaild(CURR_CURVE, pDisplay))			//有一条曲线
		iTmp = 5;
	else													//没有曲线
		iTmp = 3;

	// pOtdrMenu = CreateWndMenu1(iTmp, sizeof(strMenu), strMenu,
	// 					   	   MENU_BACK | MENU_HOME, -1, 40,
	// 					   	   OtdrMenuCall);

    strcpy(strBmpFile, "bg_pre_trace");
    pScreenShot = CreatePhoto(strBmpFile);

	//添加坐标轴控件接受消息
	AddAndLoginCoor(pOtdrCoor, pFrmOtdr);
	//注册窗体(因为所有的按键事件都统一由窗体进行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmOtdr,
				  pFrmOtdr);
	//注册窗体的按键消息处理
	GUIMESSAGE* pMsg = GetCurrMessage();
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmOtdr,
					OtdrWndKey_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmOtdr,
					OtdrWndKey_Up, NULL, 0, pMsg);
	//注册长按按键操作
	LoginMessageReg(GUIMESSAGE_KEY_PRESS, pFrmOtdr,
					OtdrWndKey_Press, NULL, 0, pMsg);
    //注册取消长按按键操作
    LoginMessageReg(GUIMESSAGE_KEY_CANCEL_PRESS, pFrmOtdr,
                    OtdrWndKey_CancelPress, NULL, 0, pMsg);

    return iErr;
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
int FrmOtdrExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	WND_OTDR_DEBUG("Entry\n");
	enCurWindow = ENUM_OTHER_WIN;	//设置当前窗体
	RunOtdrModule(ENUM_RESTART);
	WND_OTDR_DEBUG("Waiting Otdr Stoped\n");

	while(GetWorkStatus() != ENUM_OTDR_STOPPED)	//测量停止后
	{
		MsecSleep(10);
	}

	WND_OTDR_DEBUG("Waiting Otdr Stoped\n");
	SetCurveThreadMode(EN_NOT_DRAW_CURVE_MODE);
	SetDrawScratch(0);
	//清除可能的画线队列命令
	ClearCurveQueue();
	//等待画线线程空闲
	while (CurrDisplayStatus() == EN_STATUS_BUSY)
	{
		MsecSleep(10);
	}

	//保存当前坐标轴的属性
	iCoorFocus = pOtdrCoor->iFocus;

    //得到当前窗体对象
    pFrmOtdr = (GUIWINDOW *) pWndObj;
	//摘除画布
	DelAndLogoutCoor(pOtdrCoor, pFrmOtdr);
	pMsg = GetCurrMessage();
	//按键摘除
	LogoutMessageReg(GUIMESSAGE_KEY_DOWN, pFrmOtdr, pMsg);
	LogoutMessageReg(GUIMESSAGE_KEY_UP, pFrmOtdr, pMsg);
    LogoutMessageReg(GUIMESSAGE_KEY_PRESS, pFrmOtdr, pMsg);
    LogoutMessageReg(GUIMESSAGE_KEY_CANCEL_PRESS, pFrmOtdr, pMsg);
    ClearMessageReg(pMsg);
    ClearWindowComp(pFrmOtdr);

	//销毁坐标系
	DestroyCoor(&pOtdrCoor);

	DestroyOtdrCurvePara(&pOtdrCurvePara);
	DestroyCurveResult(&pOtdrCurveResult);
	DestroyEventTab(&pEvtTab);
	DestroyOtdrLinkMap(&pOtdrLinkMap);
	// 	DestoryCurveInfo();

	//销毁按钮控件
	// DestroyWndMenu1(&pOtdrMenu);

	//截图按钮
	DestroyPicture(&pScreenShot);
    pOtdrEscapeTo = NULL;

	//恢复刷新曲线标志位
	iRepaint = 0;

    return iErr;
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
int FrmOtdrPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iErr = 0;
	SetCoorSelectedOption(pOtdrCoor, iSelectedOption, 0);
	PaintCoor(pOtdrCoor);
	//DisplayOtdrCurvePara(pOtdrCurvePara);
	//DisplayCurveResult(pOtdrCurveResult);
	//刷新波长选项
	SetInitalWaveIndex(pUser_Settings->enWaveCurPos);
	int iEnable[2] = {pUser_Settings->iWave[0], pUser_Settings->iWave[1]};
	SetOptionWaveEnable(pOtdrCoor, iEnable);

	SetPowerEnable(1, 2);
	//刷新波形，只有在进入OTDR界面时刷新，其他时候不刷新
	if (!iRepaint)
	{
		SetCurveThreadMode(EN_NORMAL_MODE);
		SendLineCtlMoveMsg(EN_CTL_NEWDATA, 0, 0);
	}
	

	//设置当前选项卡
	switch (sOtdrOpCard.iCurrCard)
	{
	case OTDR_OPC_SETUP://设置选项卡
		OtdrOpcOtdr();
		break;
	case OTDR_OPC_EVENT://事件选项卡
		OtdrOpcEvt();
		break;
	case OTDR_OPC_MEASU: //测量选项卡
		OtdrOpcMeas();
		break;
	case OTDR_OPC_LINK_MAP: //链路地图选项卡
		OtdrOpcLinkMap();
		break;
	default:
		break;
	}

	//防止有曲线时数据未刷新，只有在进入OTDR界面时刷新，其他时候不刷新
	if (!iRepaint)
		RefreshScreen(__FILE__, __func__, __LINE__);

    return iErr;
}

/***
  * 功能：
        窗体的窗体循环函数，进行窗体循环
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrLoop(void *pWndObj)
{

	/*错误标志、返回值定义*/
    int iErr = 0;
// 	char curveName[8] = {0};
	static char strFile[512];
	int pRecv = 0;
    ReadOTDRMsgQueue(&pRecv);
    //解析长按移动标记线
    ReadMoveMarkerMsg();
    if (0 == pRecv)
    {
        MsecSleep(10);
		return 0;
    }

//     int iValue = OTDRMSG_VALUE(pRecv);
    pRecv = OTDRMSG_TYPE(pRecv);

	switch(pRecv)
	{
	case ENUM_OTDRMSG_NODATA:
		break;
	case ENUM_OTDRMSG_START:
		SetDrawFileName(0);
		iReflushMarkedFlag = 0;
		SetCoorEnabled(pOtdrCoor, iSelectedOption, 0);
		//开始测试时关掉持续刷新显示
		iRepaint = 1;
		//实时模式保证数据刷新
		if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
		{
			sOtdrOpCard.pOpcPaint(sOtdrOpCard.pOpc);
		}
		break;
	case ENUM_OTDRMSG_SHOWING:
		break;
	case ENUM_OTDRMSG_EVENTOK:
	{
		SetEvtTouchEnable(pEvtTab, 1);
		ResetTabAttr(pEvtTab, iEvtTabCol);
		FlushEvtTab(pEvtTab);
		//刷新链路地图
		ResetLinkMap(pOtdrLinkMap);
		RefreshOtdrLinkMap(pOtdrLinkMap);
		//刷新是否刷新文件名（双波长设置）
		static int count = 0;
		isFlushDisplayInfo(0, &count);
		// SetCoorEnabled(pOtdrCoor, iSelectedOption, 1);
	}
		break;
	case ENUM_OTDRMSG_WAVWARGS:
		RecvWaveArgs();
		break;
	case ENUM_OTDRMSG_STOP:
		// SetPictureEnable(1, pScreenShot);
		//是否自动跳转到事件列表界面
		if(pUser_Settings->sCommonSetting.iAutoJumpWindow)
        {
            SetCoorFocus(pOtdrCoor, OTDR_OPC_EVENT);
			iSelectedOption = 1;//事件列表项
		}

		RecvStop();

        MsecSleep(500);

        //提示无法设置跨段
//         EVENTS_TABLE events;
// 	    GetCurveEvents(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, &events);
// 	    if(events.iIsThereASpan == 0 && CurveIsVaild(CURR_CURVE, pOtdrTopSettings->pDisplayInfo))
// 	    {
//             CreateDialog(DIALOG_LBL_NOTE, OTDR_LBL_UNABLE_TO_SET_SPANS, 1, pFrmOtdr, RefreshCurve);
// 	    }
		//PressureTest();
		break;
	case ENUM_OTDRMSG_CONNECTCHECK:
		CreateDialog(OTDR_LBL_CARRAY_OUT, OTDR_LBL_BAD_CONNECTOR_CONTINUE, 2, pFrmOtdr,
					 CheckConnectCallBack);
		break;
	case ENUM_OTDRMSG_LIGHTCHECK:
		CreateDialog(OTDR_LBL_CARRAY_OUT, OTDR_LBL_SIGNAL_CONTINUE, 0, pFrmOtdr,
					 CheckLightCallBack);
		break;
	case ENUM_OTDRMSG_MEASARGS:
		if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
		{
			//实时模式允许刷新界面
			if (iReflushMarkedFlag || OtdrTimeIsRT())
			{
				//防止一直在发送新数据的队列消息，导致坐标轴不断更新，出现某个文本闪动刷新
				iRepaint = 1;
				sOtdrOpCard.pOpcPaint(sOtdrOpCard.pOpc);
			}
		}
		break;
	case ENUM_OTDRMSG_FILENAME:
	{
 		GetFileName(strFile);
 		SetCurveFile(pOtdrTopSettings->pTask_Context->iCurTaskPos,
 					 pOtdrTopSettings->pDisplayInfo, strFile);
	}
		break;
	case ENUM_OTDRMSG_READSOR:
		//显示AB信息
		if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
		{
			sOtdrOpCard.pOpcPaint(sOtdrOpCard.pOpc);
		}
		break;
	case ENUM_OTDRMSG_MEASTIME:
	{
        int elapsedTime = OtdrTimeGetElapsedTime();
		
		if (elapsedTime <= 0)
        {
			elapsedTime = 0;
			static int iWaveCount = 0;
			//设置是否刷新标记线信息
			isFlushDisplayInfo(1, &iWaveCount);
			//刷新波长选项
			SetInitalWaveIndex(pUser_Settings->enWaveCurPos);
			int iEnable[2] = {pUser_Settings->iWave[0], pUser_Settings->iWave[1]};
			SetOptionWaveEnable(pOtdrCoor, iEnable);
		}
    }
		break;
    case ENUM_OTDRMSG_CLEAREVENT:
    {
        ClrEvtTab(pEvtTab);
        SetEvtTouchEnable(pEvtTab, 0);
        FlushEvtTab(pEvtTab);
		//刷新链路地图
		ResetLinkMap(pOtdrLinkMap);
		RefreshOtdrLinkMap(pOtdrLinkMap);
    }
        break;
	default:
		break;
	}

	MsecSleep(10);

    return iErr;
}

/***
  * 功能：
        窗体的窗体暂停函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrPause(void *pWndObj)
{
    /*错误标志、返回值定义*/
    int iErr = 0;

    return iErr;
}

/***
  * 功能：
        窗体的窗体挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrResume(void *pWndObj)
{
    /*错误标志、返回值定义*/
    int iErr = 0;

    return iErr;
}

//初始化参数
static void OtdrInitPara(void)
{
	//清除otdr测试过程中的队列消息
	ClearOTDRMsgQueue();

	sOtdrOpCard.pOpc = NULL,
	sOtdrOpCard.pOpcPaint = NULL,
	sOtdrOpCard.pOpcSetup = NULL,

	//设置光标类型
	OtdrSetCusor();

	//绘制光标距离
	if (pUser_Settings->sCommonSetting.iCusorLblFlag)
		SetDrawCursorDist(1);
	else
		SetDrawCursorDist(0);

	//绘制缩略图
// 	SetDrawPreview(1);
}

//重绘窗体函数
static void CreateOtdrWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                         FrmOtdrInit, FrmOtdrExit,
                         FrmOtdrPaint, FrmOtdrLoop,
				         FrmOtdrPause, FrmOtdrResume,
                         NULL);
}

//画图窗口按下
static int OtdrWindow_Down(void *pInArg, int iInLen,
                           void *pOutArg, int iOutLen)
{
	pDisplay = pOtdrTopSettings->pDisplayInfo;

	if (!CurveIsVaild(CURR_CURVE, pDisplay))
		return -1;

	int iTmp = -1;
	int iTouchX = ((unsigned int)pInArg >> 16) & 0x0000ffff;
	int iTouchY = ((unsigned int)pInArg) & 0x0000ffff;


	iMTLastX = 0;
	iMTLastY = 0;

	SelRect.lSX = iTouchX;
	SelRect.lEX = iTouchX;
	SelRect.lSY = iTouchY;
	SelRect.lEY = iTouchY;

	//拖动放大模式
	if (EN_SCATCH == sWinPara.enZoomMode)
	{
		SendLineCtlMoveMsg(EN_CTL_DOWN, iTouchX, iTouchY);
		iOtdrTouch = 1; 						//标明down事件已发生
	}
	//光标移动
	else if (EN_TCURSOR == sWinPara.enCursorType)
	{
		switch(sOtdrOpCard.iCurrCard)
		{
		case OTDR_OPC_EVENT:
			if (TouchIsOnEvent(iTouchX, pEvtTab))
			{
				FlushEvtTab(pEvtTab);
				SetCursorAtEvent(pEvtTab, 1, 0);
			}
			else
			{
				//SendCursorCtlMsg(EN_MOVE_TOUCH, iTouchX);
                SendEventMakerCtlMsg(EN_MOVE_TOUCH, iTouchX, -1, -1, 1);
			}
			break;
		case OTDR_OPC_SETUP:
			iTmp = TouchOnMaker(CURR_CURVE, pDisplay, iTouchX);
			if (-1 != iTmp)		//匹配到有效标记线
			{
				iOtdrTouch = 1; //标明已经选中了一个标记线
				SendMakerCtlMsg(EN_MAKER_PATCH, iTmp);
			}
            else
            {
                SendMakerCtlMsg(EN_MAKER_DOWN, iTouchX);
				LOG(LOG_INFO, "--iTouchX = %d----\n", iTouchX);
			}
			break;
		default:
			SendCursorCtlMsg(EN_MOVE_TOUCH, iTouchX);
			break;
		}
	}

    return 0;
}

//画图窗口按下
static int OtdrWindow_Move(void *pInArg, int iInLen,
                           void *pOutArg, int iOutLen)
{
	pDisplay = pOtdrTopSettings->pDisplayInfo;

	if (!CurveIsVaild(CURR_CURVE, pDisplay))
		return -1;

	int iTouchX = ((unsigned int)pInArg >> 16) & 0x0000ffff;
	int iTouchY = ((unsigned int)pInArg) & 0x0000ffff;
	iMTLastX = 0;
	iMTLastY = 0;

	LOG(LOG_INFO,"--iTouchX = %d-----\n", iTouchX);

	if (EN_SCATCH == sWinPara.enZoomMode)	//拖动放大模式
	{
		if (((SelRect.lEX != iTouchX) || (SelRect.lEY != iTouchY)) &&
			(1 == iOtdrTouch))//位置改变
		{
			SelRect.lEX = iTouchX;
			SelRect.lEY = iTouchY;
			SendLineCtlMoveMsg(EN_CTL_MOVE, iTouchX, iTouchY);
		}
	}
	else if (EN_TCURSOR == sWinPara.enCursorType)//光标移动
	{
		switch(sOtdrOpCard.iCurrCard)
		{
		case OTDR_OPC_EVENT:
			if (SelRect.lEX != iTouchX)
			{
				if (TouchIsOnEvent(iTouchX, pEvtTab))
				{
					FlushEvtTab(pEvtTab);
					SetCursorAtEvent(pEvtTab, 1, 0);
				}
				else
				{
                    SendEventMakerCtlMsg(EN_MOVE_TOUCH, iTouchX, -1, -1, 1);
				}
				SelRect.lEX = iTouchX;
			}
			break;
		case OTDR_OPC_SETUP:
			if ((1 == iOtdrTouch) &&			//有标记线被选中
				(SelRect.lEX != iTouchX))		//且位置发生变化
			{
				SendMakerCtlMsg(EN_MAKER_MOVE, iTouchX);
				SelRect.lEX = iTouchX;
			}
			break;
		default:
			if (SelRect.lEX != iTouchX)
			{
				SendCursorCtlMsg(EN_MOVE_TOUCH, iTouchX);
				SelRect.lEX = iTouchX;
			}
			break;
		}
	}
	else										//波形平移模式
	{
		if((SelRect.lEX != iTouchX) ||
		   (SelRect.lEY != iTouchY))
		{
			SendLineCtlMoveMsg(EN_CTL_DRAG, iTouchX - SelRect.lEX,
							   iTouchY - SelRect.lEY);
			SelRect.lEX = iTouchX;
			SelRect.lEY = iTouchY;
		}
	}
    return 0;
}

//画波形窗口抬起
static int OtdrWindow_Up(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen)
{
	iOtdrTouch = 0;							//恢复点击标志位
	iMTLastX = 0;
	iMTLastY = 0;

	//区域放大选择区域完成，进行区域放大
	if (EN_SCATCH == sWinPara.enZoomMode)
	{
		SendLineCtlMoveMsg(EN_CTL_SCRATCH, SelRect.lEX, SelRect.lEY);
	}

    return 0;
}

//MT(多点触摸)响应函数
static int OtdrWindowMT(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen)
{
	int iRet = 0;
	PDISPLAY_INFO pDisplayInfo = NULL;
	pDisplayInfo = pOtdrTopSettings->pDisplayInfo;
	int iStartX = pOtdrCoor->pCurve->Visible.Area.Start.x;
	int iStartY = pOtdrCoor->pCurve->Visible.Area.Start.y;
	int iEndX = pOtdrCoor->pCurve->Visible.Area.End.x;
	int iEndY = pOtdrCoor->pCurve->Visible.Area.End.y;

	UINT32 uiTmpCen = GetMtCenterPosition();

	UINT32 uiTmpCenX = ((unsigned int)uiTmpCen >> 16) & 0x0000ffff;
	UINT32 uiTmpCenY = ((unsigned int)uiTmpCen) & 0x0000ffff;

	if( (uiTmpCenX < iStartX) || (uiTmpCenX > iEndX) ||
		(uiTmpCenY < iStartY) || (uiTmpCenY > iEndY))
	{
		LOG(LOG_ERROR, "out of boundary\n");
		return -1;
	}

	INT32 iTouchX = ((unsigned int)pInArg >> 16) & 0x0000ffff;
	INT32 iTouchY = ((unsigned int)pInArg) & 0x0000ffff;

	if( (iMTLastX != 0) || (iMTLastY != 0))
	{
		if( (iMTLastX != iTouchX) || (iMTLastY != iTouchY))
		{
			int deltaX = (iTouchX - iMTLastX) / 10;
			int deltaY = (iTouchY - iMTLastY) / 10;
			SendLineCtlScaleMsg(EN_CTL_MT, deltaX , deltaY);
		}
	}

	iMTLastX = iTouchX;
	iMTLastY = iTouchY;

	return iRet;
}

//按键处理响应函数
static int OtdrWndKey_Down(void *pInArg, int iInLen,
						   void *pOutArg, int iOutLen)
{
	int iRet = 0;

	return iRet;
}
static int OtdrWndKey_Up(void *pInArg, int iInLen,
						   void *pOutArg, int iOutLen)
{
	int iRet = 0;
	unsigned int uiValue = (unsigned int)pInArg;
	
	switch (uiValue)
	{
	case KEYCODE_SHIFT:
		iScaleFlag = iScaleFlag ? 0 : 1;
		//曲线参数界面、事件列表界面以及链路地图界面有效
		if (iScaleFlag && (sOtdrOpCard.iCurrCard <= OTDR_OPC_LINK_MAP))
		{
			//设置绘制放大镜图标
			SetDrawMagnifier(1);
		}
		else
		{
			//设置绘制放大镜图标
			SetDrawMagnifier(0);
		}
		SendLineCtlScaleMsg(EN_CTL_MAGNIFIER, 1.0f, 1.0f);//用于更新显示放大镜图标
		break;
	case KEYCODE_OTDR_LEFT:
        if (iSelectedOption != 0)
		{
			iSelectedOption--;
			SelectedOptionCallBack(iSelectedOption, 0);
		}
		else
		{
			// OtdrSwitchWave(NULL, 0, NULL, 0);
            iSelectedOption = OPTION_NUM + 2;
			SetCoorSelectedOption(pOtdrCoor, iSelectedOption, 1);
			RefreshScreen(__FILE__, __func__, __LINE__);
		}
		
		break;
	case KEYCODE_OTDR_RIGHT:
        if (iSelectedOption != (OPTION_NUM + 2))
		{
			iSelectedOption++;
			SelectedOptionCallBack(iSelectedOption, 1);
		}
		else
		{
            SetCoorSelectedOption(pOtdrCoor, iSelectedOption+1, 1);
            SetCoorFocus(pOtdrCoor, OTDR_OPC_SETUP);
			iSelectedOption = 0; //事件列表项
		}
		break;
	case KEYCODE_START:
		LOG(LOG_INFO, "-----start------\n");
		OtdrTest(1);
		break;
	case KEYCODE_UP:
		LOG(LOG_INFO, "-----up------\n");
		HandleKeyUpDown();
		break;
	case KEYCODE_DOWN:
		LOG(LOG_INFO, "-----down------\n");
		HandleKeyDownDown();
		break;
	case KEYCODE_LEFT:
		LOG(LOG_INFO, "-----left------\n");
		HandleKeyLeftDown();
		break;
	case KEYCODE_RIGHT:
		LOG(LOG_INFO, "-----right------\n");
		HandleKeyRightDown();
		break;
	case KEYCODE_ENTER:
		LOG(LOG_INFO, "-----enter------\n");
		HandleEnterKeyCallBack();
		//恢复原状
		// iScaleFlag = 0;
		break;
	case KEYCODE_ESC:
		LOG(LOG_INFO, "-----esc------\n");
		iScaleFlag = 0;
		SetDrawMagnifier(0);
		OtdrBackHome(NULL, 0, NULL, 0);
		break;
	case KEYCODE_FILE:
		LOG(LOG_INFO, "-----file------\n");
		iScaleFlag = 0;
		SetDrawMagnifier(0);
		//设置光标位置（从保存文件返回）
		iSelectedOption = 5;
		OtdrFile(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}

	return iRet;
}

//读取长按移动标志位信息
static void ReadMoveMarkerMsg(void)
{
    static int count = 0;
    count++;
    //计算进入移动标记线处理的次数，减缓发送画线消息处理
    if (count % 25 != 0)
        return;
    count = 0;  //计数清零
    //向左移动标记线
    if (iPressKeyArray[0])
    {
        if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
            //向左移动选中的标记线
            MoveMarker(pOtdrCurvePara->iFocus, 0, 3 * 2); // move left
    }

    //向右移动标记线
    if (iPressKeyArray[1])
    {
        if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
            //向左移动选中的标记线
            MoveMarker(pOtdrCurvePara->iFocus, 1, 3 * 2); // move left
    }
}

//长按按键处理（移动标记线）
static int OtdrWndKey_Press(void *pInArg, int iInLen,
						 void *pOutArg, int iOutLen)
{
	int iRet = 0;
	unsigned int uiValue = (unsigned int)pInArg;

	switch (uiValue)
	{
	case KEYCODE_LEFT:
        iPressKeyArray[0] = 1;
		break;
	case KEYCODE_RIGHT:
        iPressKeyArray[1] = 1;
		break;
	default:
		break;
	}

	return iRet;
}

//取消长按按键处理（移动标记线）
static int OtdrWndKey_CancelPress(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    int iRet = 0;
    unsigned int uiValue = (unsigned int)pInArg;

    switch (uiValue)
    {
    case KEYCODE_LEFT:
        iPressKeyArray[0] = 0;
        break;
    case KEYCODE_RIGHT:
        iPressKeyArray[1] = 0;
        break;
    default:
        break;
    }

    return iRet;
}

//设置画曲线标志位
static void SetDrawFlgs(int iCsrFlg, int iEvtFlg, int iMakFlg, int iRefFlg, int iEventMarkerFlag)
{
	SetDrawCursor(iCsrFlg);
	SetDrawEvent(iEvtFlg);
	SetDrawMarker(iMakFlg);
	SetDrawReference(iRefFlg);
    SetDrawEventMarker(iEventMarkerFlag);
}

//切换选项卡
static void SwitchOptionCard(void *arg, OPC_PAINT paint,
							 OPC_SETUP setup, int iCurrCard)
{
	if (sOtdrOpCard.pOpcSetup)
	{
		sOtdrOpCard.pOpcSetup(sOtdrOpCard.pOpc, 0, pFrmOtdr);
	}

	//设置新的选项卡
	sOtdrOpCard.pOpc = arg;
	sOtdrOpCard.pOpcPaint = paint;
	sOtdrOpCard.pOpcSetup = setup;
	sOtdrOpCard.iCurrCard = iCurrCard;

	//绘制新选项卡
	sOtdrOpCard.pOpcSetup(sOtdrOpCard.pOpc, 1, pFrmOtdr);
	sOtdrOpCard.pOpcPaint(sOtdrOpCard.pOpc);
}

//设置OTDR选项卡
static int OtdrOpcOtdr(void)
{
	int iErr = 0;

	sOtdrOpCard.iCurrCard = OTDR_OPC_SETUP;
	pOtdrCoor->iFocus = OTDR_OPC_SETUP;
	//设置显示光标距离
	SetDrawCursorDist(0);

	if (sOtdrOpCard.iCurrWnd == COOR_NOMAL)
	{
		OtdrSwitchModule();
	}

	SwitchOptionCard(pOtdrCurvePara, (OPC_PAINT)RefreshOtdrCurvePara,
		(OPC_SETUP)SetOtdrCurveParaEnable, OTDR_OPC_SETUP);
	SetDrawFlgs(DISABLE, DISABLE, ENABLE, DISABLE, DISABLE);

	return iErr;
}

//事件选项卡
static int OtdrOpcEvt(void)
{
	int iErr = 0;

	sOtdrOpCard.iCurrCard = OTDR_OPC_EVENT;
	pOtdrCoor->iFocus = OTDR_OPC_EVENT;
	//设置显示光标距离
	SetDrawCursorDist(0);

	if (sOtdrOpCard.iCurrWnd == COOR_LARGE)
	{
		OtdrSwitchModule();
	}

	SwitchOptionCard(pEvtTab, (OPC_PAINT)FlushEvtTab,
		(OPC_SETUP)SetEvtTabEnable, OTDR_OPC_EVENT);

	SetDrawFlgs(DISABLE, ENABLE, DISABLE, DISABLE, DISABLE);

	return iErr;
}

//链路地图选项卡
static int OtdrOpcLinkMap(void)
{
	int iErr = 0;

	sOtdrOpCard.iCurrCard = OTDR_OPC_LINK_MAP;
	pOtdrCoor->iFocus = OTDR_OPC_LINK_MAP;
	//设置显示光标距离
	SetDrawCursorDist(0);

	if (sOtdrOpCard.iCurrWnd == COOR_LARGE)
	{
		OtdrSwitchModule();
	}
	//刷新链路地图信息
	handleEventList(pOtdrLinkMap, GetEvtTabEvents());
	SwitchOptionCard(pOtdrLinkMap, (OPC_PAINT)RefreshOtdrLinkMap,
					 (OPC_SETUP)SetOtdrLinkMapEnable, OTDR_OPC_LINK_MAP);

	SetDrawFlgs(DISABLE, ENABLE, DISABLE, DISABLE, DISABLE);

	return iErr;
}

//测量选项卡
static int OtdrOpcMeas(void)
{
	int iErr = 0;

	sOtdrOpCard.iCurrCard = OTDR_OPC_MEASU;
	pOtdrCoor->iFocus = OTDR_OPC_MEASU;
	//设置不显示光标距离
	SetDrawCursorDist(0);

	if (sOtdrOpCard.iCurrWnd == COOR_LARGE)
	{
		OtdrSwitchModule();
	}

	SwitchOptionCard(pOtdrCurveResult, (OPC_PAINT)FlushCurveResult,
		(OPC_SETUP)SetCurveResultEnable, OTDR_OPC_MEASU);
	SetDrawFlgs(DISABLE, DISABLE, DISABLE, DISABLE, DISABLE);

	return iErr;
}

//OTDR选项卡回调
static int OtdrOptionOtdr(void *pInArg, int iInLen,
                          void *pOutArg, int iOutLen)
{
	int iErr = 0;

	OtdrOpcOtdr();

	if(GetWorkStatus() != ENUM_OTDR_STOPPED)
	{
		RecvWaveArgs();
		if(sOtdrOpCard.iCurrWnd == COOR_NOMAL)
        {
            //RefreshKnobTime(pOtdrSet, -1);
        }
        else
        {
// 			RefreshDropTime(-1);
			RefreshOtdrCurvePara(pOtdrCurvePara);
        }
	}
	else
	{
        if(sOtdrOpCard.iCurrWnd == COOR_NOMAL)
        {
            //RefreshKnobTime(pOtdrSet, 0);
        }
        else
        {
// 			RefreshDropTime(0);
			RefreshOtdrCurvePara(pOtdrCurvePara);
        }
	}

	SendLineCtlMoveMsg(EN_CTL_SWITCH, 0, 0);

	return iErr;
}

//事件选项卡回调
static int OtdrOptionEvt(void *pInArg, int iInLen,
                         void *pOutArg, int iOutLen)
{
	int iErr = 0;

	OtdrOpcEvt();

	SendLineCtlMoveMsg(EN_CTL_SWITCH, 0, 0);

	return iErr;
}

//链路地图选项卡回调
static int OtdrOptionLinkMap(void *pInArg, int iInLen,
						 void *pOutArg, int iOutLen)
{
	int iErr = 0;

	OtdrOpcLinkMap();

	SendLineCtlMoveMsg(EN_CTL_SWITCH, 0, 0);

	return iErr;
}

//测量选项卡回调
static int OtdrOptionMeas(void *pInArg, int iInLen,
                          void *pOutArg, int iOutLen)
{
	int iErr = 0;

	OtdrOpcMeas();

	SendLineCtlMoveMsg(EN_CTL_SWITCH, 0, 0);

	return iErr;
}

//返回主菜单回调
int OtdrBackHome(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;

	if (pOtdrEscapeTo)
	{
		pOtdrEscapeTo(&pWnd);
		pOtdrEscapeTo = NULL;
	}
	else {
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmMainInit, FrmMainExit,
			FrmMainPaint, FrmMainLoop,
			FrmMainPause, FrmMainResume,
			NULL);
	}

	SendWndMsg_WindowExit(pFrmOtdr);
	SendSysMsg_ThreadCreate(pWnd);

	return 0;
}

//进入otdr设置界面的回调函数
int OtdrSetting(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
	Stack *ps = NULL;

	ps = GetCurrWndStack();
	WndPush(ps, CreateOtdrWindow);
	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmCommonSetInit, FrmCommonSetExit,
		FrmCommonSetPaint, FrmCommonSetLoop,
		FrmCommonSetPause, FrmCommonSetResume,
		NULL);
	SendWndMsg_WindowExit(pFrmOtdr);
	SendSysMsg_ThreadCreate(pWnd);

	return 0;
}


//切换波长的回调函数
int OtdrSwitchWave(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	int iEnable[2] = { pUser_Settings->iWave[0], pUser_Settings->iWave[1] };
	int enWaveCurPos = pUser_Settings->enWaveCurPos;
	if (pUser_Settings->iWave[0] && pUser_Settings->iWave[1])
	{
        SetInitalWaveIndex(1 - enWaveCurPos);
        SetOptionWaveEnable(pOtdrCoor, iEnable);
		//显示下一条曲线
		DisplayNextCur();
	}
	else
	{
		SetInitalWaveIndex(enWaveCurPos);
		SetOptionWaveEnable(pOtdrCoor, iEnable);
	}

	// RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

//打开文件按钮回调函数
static int OtdrOpenFile(void *pInArg, int iInLen,
						void *pOutArg, int iOutLen)
{
	char* pOtdrFileCurrentPath = NULL;

	if (access("/mnt/dataDisk", F_OK) && access("/tmp/usb", F_OK))
	{
		MsgNoSdcard(pFrmOtdr);
	}
	else
	{
		if (pOtdrFileCurrentPath == NULL)
		{
			pOtdrFileCurrentPath = GuiMemAlloc(512);
		}

		strcpy(pOtdrFileCurrentPath, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath); //每次打开都是OTDR设置的路径
		OtdrFileDialog(FILE_OPEN_MODE, MntUpDirectory, pOtdrFileCurrentPath, CreateOtdrWindow, SOR);
	}

	return 0;
}

//保存文件按钮的回调函数
int OtdrFile(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	OtdrOpenFile(pInArg, iInLen, pOutArg, iOutLen);

	return 0;
}

////截图按下处理函数
//static int OtdrScreenShot_Down(void *pInArg, int iInLen,
//                          	   void *pOutArg, int iOutLen)
//{
//	return 0;
//}
//
//static int OtdrScreenShot_Up(void *pInArg, int iInLen,
//                             void *pOutArg, int iOutLen)
//{
///*
//	if (pUser_Settings->sCommonSetting.iScreenShotFlag)
//	{
//        while(CurrDisplayStatus() == EN_STATUS_BUSY)
//        {
//            MsecSleep(50);
//        }
//		OtdrScreenShot(pFrmOtdr);
//	}*/
//	OtdrMenuCall(0);
//	return 0;
//}


//光标按下处理函数
static void OtdrSetCusor(void)
{
	sWinPara.enCursorType = EN_TCURSOR;
	sWinPara.enZoomMode = EN_NORMAL;
}

//手型按下处理函数
//static void OtdrSetDrag(void)
//{
//	sWinPara.enCursorType = EN_THAND;
//	sWinPara.enZoomMode = EN_NORMAL;
//}

//局部放大按下处理函数
//static void OtdrAreaZoom(void)
//{
//	sWinPara.enCursorType = EN_TCURSOR;
//	sWinPara.enZoomMode = EN_SCATCH;
//}
//
////桌面上1:1按钮按下处理函数
//static void OtdrZoom11(void)
//{
//	SendLineCtlMoveMsg(EN_CTL_RECOVER, 0, 0);
//}

//桌面上Zoom In按钮弹起处理函数
static void OtdrZoomIn_x(void)
{
	SendLineCtlScaleMsg(EN_CTL_ZOOMIN, 2.0f, 1.0f);
}

//桌面上Zoom Out按钮弹起处理函数
static void OtdrZoomOut_x(void)
{
	SendLineCtlScaleMsg(EN_CTL_ZOOMOUT, 2.0f, 1.0f);
}

//桌面上Zoom In按钮弹起处理函数
static void OtdrZoomIn_y(void)
{
	SendLineCtlScaleMsg(EN_CTL_ZOOMIN, 1.0f, 2.0f);
}

//桌面上Zoom Out按钮弹起处理函数
static void OtdrZoomOut_y(void)
{
	SendLineCtlScaleMsg(EN_CTL_ZOOMOUT, 1.0f, 2.0f);
}

//切换坐标系模式
static void OtdrSwitchModule(void)
{
	//当前为普通模式需要切换到伸展模式,当前为伸展模式需要切换到普通模式
	sOtdrOpCard.iCurrWnd = (sOtdrOpCard.iCurrWnd) ? COOR_NOMAL : COOR_LARGE;
	iEvtTabCol = 4;
	DelAndLogoutCoor(pOtdrCoor, pFrmOtdr);
	AddAndLoginCoor(pOtdrCoor, pFrmOtdr);
	//Curve重新设置，需要重新注册画布
	ResetCurve(pOtdrCoor, sOtdrOpCard.iCurrWnd);
	//刷新波形
	SendLineCtlMoveMsg(EN_CTL_NEWDATA, 0, 0);
	//SetPowerEnable(1, 2);
	//不是瞎延时，阻塞当前线程
	MsecSleep(50);
}

//工具栏回调
//static void OtdrToolBar(int iOp)
//{
//	switch (iOp)
//	{
//	case 0://光标
//		if(EN_THAND == sWinPara.enCursorType)
//		{
//			OtdrSetCusor();
//		}
//		else if ((EN_TCURSOR == sWinPara.enCursorType) ||
//				  (EN_SCATCH == sWinPara.enZoomMode))
//		{
//			OtdrSetDrag();
//		}
//		break;
//	case 1://拖放
//		if (EN_NORMAL == sWinPara.enZoomMode)
//		{
//			OtdrAreaZoom();
//		}
//		else if(EN_SCATCH == sWinPara.enZoomMode)
//		{
//			OtdrSetCusor();
//		}
//		RefreshScreen(__FILE__, __func__, __LINE__);
//		break;
//	case 2://1比1放大
//		OtdrZoom11();
//		break;
//	case 3://放大
//	    if(sWinPara.enZoomMode == EN_NORMAL)
//	    {
//		    OtdrZoomIn_x();
//		}
//		else
//		{
//            OtdrZoomIn_y();
//		}
//		break;
//	case 4://缩小
//	    if(sWinPara.enZoomMode == EN_NORMAL)
//	    {
//		    OtdrZoomOut_x();
//		}
//		else
//		{
//            OtdrZoomOut_y();
//		}
//		break;
//
//	case 5://模式切换
//		OtdrSwitchModule();
//		break;
//	}
//}

static void otdrSave(char *absolutePath, Operate operateFunc)
{
    if(!absolutePath)
    {
        return;
    }
    if (CanWriteFile(absolutePath))
    {
        MsecSleep(500);
		//创建报告
		GuiOperateSave(pFrameBufferBak, pFrmOtdr, operateFunc, absolutePath);
    }
}

//生成报告回调
static void ReportCall(int iOp)
{
	//恢复显示
	SendCurveAdditionCmd(EN_ADD_REFSH, 0);

	switch (iOp)
	{
	case 0:
		break;
	case 1:
    	{
            char *reportPath = otdrPdfReportGetPathOrName(0);
            otdrSave(reportPath, CreateOtdrReport);
            free(reportPath);
    	}
		break;
	default:
		break;
	}

}

static void PrepareReport(void)
{

	if (access(MntSDcardDataDirectory, F_OK))
	{
		MsgNoSdcard(pFrmOtdr);
	}
	else
	{
		CreateDialog(OTDR_LBL_CARRAY_OUT, DIALOG_LBL_CREAT_REPORT, 2, pFrmOtdr, ReportCall);
	}
}

static int SaveAsSOR(char *absolutePath)
{
    return WriteSor(CURR_CURVE, absolutePath);
}

static int SaveAsINNO(char *absolutePath)
{
    char *sorTempPath = "/app/sor";
    char **sorList = NULL;
    PDISPLAY_INFO pDisplayInfo = pOtdrTopSettings->pDisplayInfo;
    int i, iErr = 0;
    unsigned int curveNum = pDisplayInfo->iCurveNum;

    isExist(sorTempPath);
    sorList = (char **)calloc(sizeof(char *), curveNum);
    for(i = 0; i < curveNum; i++)
    {
        sorList[i] = (char *)calloc(F_NAME_MAX, 1);
        sprintf(sorList[i], "%s/%s", sorTempPath, pDisplayInfo->pCurve[i]->strFName);
        iErr = WriteSor(i, sorList[i]);
        if(iErr) break;
    }

    if(!iErr)
    {
        iErr = InnoCreate(absolutePath, sorList, curveNum);
    }

    for(i = 0; i < curveNum; i++)
    {
        if(sorList[i])
            free(sorList[i]);
    }
    free(sorList);

    mysystem("rm /app/sor -rf");

    return iErr;
}

static void SaveAs(void)
{
    char curveName[F_NAME_MAX] = {0};
    unsigned int defaultOption = pOtdrTopSettings->pUser_Setting->sFileNameSetting.fileFormat;
    int isSaveToUsb;
    int count = 3;
    char *fileFormat[3] = {".PDF", ".SOR", ".CUR"};
    char newName[F_NAME_MAX];
    memset(newName, 0, F_NAME_MAX);
    memset(curveName, 0, F_NAME_MAX);
	GetCurveFile(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, curveName);
    if(SAVE_AS_DIALOG_KEY_ENTER == ShowSaveAsDialog(curveName, newName, fileFormat, count, defaultOption, &isSaveToUsb))
    {
        char absolutePath[F_NAME_MAX];
	    SendCurveAdditionCmd(EN_ADD_REFSH, 0);	//恢复显示
	    MsecSleep(500);

        int i;
        for(i = 0; i < count; ++i)
        {
            if(!strcasecmp(newName, fileFormat[i]))
            {
                DialogInit(120, 90, TransString("Warning"),
    						TransString("DIALOG_FILE_NOT_EMPTY"),
    						0, 0, CreateOtdrWindow, NULL, NULL);
    		    return;
            }
        }

        if(strstr(newName, fileFormat[0]))
        {
            if(!isSaveToUsb)
            {
                isExist(pOtdrTopSettings->sDefSavePath.cOtdrReportSavePath);
            }

            isSaveToUsb ? sprintf(absolutePath, MntUsbDirectory"/%s", newName)
                        : sprintf(absolutePath, "%s/%s", pOtdrTopSettings->sDefSavePath.cOtdrReportSavePath,newName);

            otdrSave(absolutePath, CreateOtdrReport);
        }
        else
        {
            if(!isSaveToUsb)
            {
                isExist(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
            }

            isSaveToUsb ? sprintf(absolutePath, MntUsbDirectory"/%s", newName)
                        : sprintf(absolutePath, "%s/%s", pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath,newName);

            if(strstr(newName, fileFormat[1]))
            {
                otdrSave(absolutePath, SaveAsSOR);
            }
            else
            {
                otdrSave(absolutePath, SaveAsINNO);
            }
        }

		LOG(LOG_INFO, "save as path: %s\n", absolutePath);
	}

}

//菜单控件回调函数
static void OtdrMenuCall(int iOp)
{
	GUIWINDOW *pWnd = NULL;
	Stack *ps = NULL;

	switch (iOp)
	{
	case 0://开始测量
        if(GetWorkStatus() == ENUM_OTDR_STOPPED)
        {
            SetDrawIsPass(0);
			pDisplay = pOtdrTopSettings->pDisplayInfo;
            if (CurveIsAnalysised(CURR_CURVE, pDisplay)
                && CurveIsVaild(CURR_CURVE, pDisplay))
            {
                int iPromptSaveFlag = pOtdrTopSettings->pUser_Setting->sCommonSetting.iPromptSaveFlag;
                if ((!IsCurveSaved(pDisplay)) && iPromptSaveFlag)
                {
                    unsigned int button = ShowStandardMessageBox(
                        NULL,
                        TransString("OTDR_SAVE_RESULT")
                        );
                    //sleep 200ms 防止界面出现覆盖
                    MsecSleep(200);
                    if (button == MessageBox_Yes)
                    {
                        SaveAllCurves(pDisplay);
                    }
                }
            }
        }

		if(sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
		{
			SetPictureEnable(1, pScreenShot);
			RecvWaveArgs();
			if(sOtdrOpCard.iCurrWnd == COOR_NOMAL)
			{
				//SetKnobEnable(pOtdrSet, 0);
				//DispTransparent(150, 0x0, 159, 330, 521, 150); //灰度
			}
			else
			{
// 				SetAllDropEnable(pOtdrDrop, 0);
				//DispTransparent(150, 0x0, 167, 402, 528, 78); //灰度
				RefreshOtdrCurvePara(pOtdrCurvePara);
			}
		}

//         SetWndMenuItemEnble(0,0,pOtdrMenu);
//         RefreshScreen(__FILE__, __func__, __LINE__);
		RunOtdrModule(1);
		break;
	case 1://存储
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
	                        FrmOtdrSaveInit, FrmOtdrSaveExit,
	                        FrmOtdrSavePaint, FrmOtdrSaveLoop,
					        FrmOtdrSavePause, FrmOtdrSaveResume,
	                        NULL);
	    SendWndMsg_WindowExit(pFrmOtdr);
   	 	SendSysMsg_ThreadCreate(pWnd);
		break;
	case 2://otdr设置

		ps = GetCurrWndStack();
		WndPush(ps, CreateOtdrWindow);
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
	                        FrmCommonSetInit, FrmCommonSetExit,
	                        FrmCommonSetPaint, FrmCommonSetLoop,
					        FrmCommonSetPause, FrmCommonSetResume,
	                        NULL);
	    SendWndMsg_WindowExit(pFrmOtdr);
   	 	SendSysMsg_ThreadCreate(pWnd);
		break;
	case 3://生成报告
		PrepareReport();
		break;
    case 4:
        SaveAs();
        break;
	case 5://下一条曲线
		DisplayNextCur();
		break;
	case HOME_DOWN://主页
    	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                        FrmMainInit, FrmMainExit,
                        FrmMainPaint, FrmMainLoop,
				        FrmMainPause, FrmMainResume,
                        NULL);

	    SendWndMsg_WindowExit(pFrmOtdr);
   	 	SendSysMsg_ThreadCreate(pWnd);
	    break;
	case BACK_DOWN://退出
        if (pOtdrEscapeTo)
        {
            pOtdrEscapeTo(&pWnd);
            pOtdrEscapeTo = NULL;
        }
        else {
    		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                FrmMainInit, FrmMainExit,
                                FrmMainPaint, FrmMainLoop,
        				        FrmMainPause, FrmMainResume,
                                NULL);
        }
	    SendWndMsg_WindowExit(pFrmOtdr);
   	 	SendSysMsg_ThreadCreate(pWnd);
	 	break;
	default:
		break;
	}
}

//读取到队列中有WAVWARGS时响应函数
static void RecvWaveArgs(void)
{
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
	{
		switch (sOtdrOpCard.iCurrWnd)
		{
			case COOR_NOMAL://当前为普通模式
// 				SetWaveEnable(pOtdrSet, 0);
// 				SetWaveSelect(pOtdrSet, pUser_Settings->enWaveCurPos);
// 				DisplayOtdrBottom(pOtdrSet);

				break;
			case COOR_LARGE://当前为伸展模式
// 				DestroyDropList();
// 				DropSetWaveEnable(pOtdrDrop, 0);
// 				DropSetWaveSelect(pOtdrDrop, pUser_Settings->enWaveCurPos);
// 				DisplayOtdrDrop(pOtdrDrop);
// 				if (pOtdrDrop->iEnable)
// 				{
// // 					DispTransparent(150, 0x0, 0, 402, 167, 78); //灰度
// 					SendCurveAdditionCmd(EN_ADD_REFSH, 0);
// 				}
				if (pOtdrCurvePara->iEnabled)
				{
					SendCurveAdditionCmd(EN_ADD_REFSH, 0);
				}

				break;
			default:
				break;
		}
	}
}

//读取到队列中有ENUM_OTDRMSG_STOP时响应函数
static void RecvStop(void)
{
	int iTmp;
	int iNum;
	switch (sOtdrOpCard.iCurrWnd)
	{
	case COOR_NOMAL://当前为普通模式
// 		SetWaveEnable(pOtdrSet, 1);
// 		SetKnobEnable(pOtdrSet, 1);
// 		SetWaveSelect(pOtdrSet, pUser_Settings->enWaveCurPos);
		break;
	case COOR_LARGE://当前为伸展模式
// 		DestroyDropList();
// 		DropSetWaveEnable(pOtdrDrop, 1);
// 		SetAllDropEnable(pOtdrDrop, 1);
// 		DropSetWaveSelect(pOtdrDrop, pUser_Settings->enWaveCurPos);
		break;
	default:
		break;
	}

	pDisplay = pOtdrTopSettings->pDisplayInfo;
	iNum = GetCurveNum(pDisplay);
	if (CurveIsVaild(CURR_CURVE, pDisplay) && (2 == iNum))	//有两条曲线
		iTmp = 6;
	else if (CurveIsVaild(CURR_CURVE, pDisplay))			//有一条曲线
		iTmp = 5;
	else													//没有曲线
		iTmp = 3;

	sOtdrOpCard.pOpcPaint(sOtdrOpCard.pOpc);
	//恢复图标的使能
	SetCoorEnabled(pOtdrCoor, iSelectedOption, 1);
	SyncCurrFbmap();
    //SendCurveAdditionCmd(EN_ADD_REFSH, 0);

	// 保存测量结果信息，导出excel --by SHI
	SaveOTDRMeasuringResult();

    //刷新曲线，主要目的是为了当计算sor文件名时间过久，UI刷新文件名
    //SendLineCtlMoveMsg(EN_CTL_NEWDATA, 0, 0);
	//判断是否自动存曲线
	if (pUser_Settings->sCommonSetting.iAutoSaveFlag)
	{
	    if(pUser_Settings->sFileNameSetting.fileFormat == 1)
	    {
            char *reportPath = otdrPdfReportGetPathOrName(0);
            if(reportPath)
            {
                CreateOtdrReport(reportPath);
                free(reportPath);
            }
	    }
	    else if(pUser_Settings->sFileNameSetting.fileFormat == 2)
	    {
	        SaveAllCurves(pDisplay);
	    }
	    else
	    {
	        char absolutePath[512] = {0};
	        char strFile[F_NAME_MAX];
			GetCurveFile(CURR_CURVE, pDisplay, strFile);
			char *temp = strstr(strFile, ".SOR");
			if(temp)
			{
    			*(temp - 5) = 0;                        //inno文件名没用波长
    	        isExist(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
    	        sprintf(absolutePath, "%s/%s.CUR", pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath,strFile);
                if(!SaveAsINNO(absolutePath))
                {
                    SetSavedFlag(pDisplay, 1);
                }
            }
	    }
	}
}

//显示下一条曲线
void DisplayNextCur(void)
{
	int iPos = NextCurve(pOtdrTopSettings->pDisplayInfo);
//     char curveName[8] = {0};

	if (iPos < 0)
		return;

	pUser_Settings->enWaveCurPos = iPos;

	//重新计算事件表
	ResetTabAttr(pEvtTab, iEvtTabCol);

	sOtdrOpCard.pOpcPaint(sOtdrOpCard.pOpc);

	SendLineCtlMoveMsg(EN_CTL_SWITCH, 0, 0);
}

//改变参数后重新开始测量
void ChangeArgsRestart(int arg)
{
	if(GetWorkStatus() != ENUM_OTDR_STOPPED)	//正在测量
	{
		LOG(LOG_ERROR,"arg = %d\n",arg);
		if(arg == 1)
		{
// 			SetKnobEnable(pOtdrSet, 0);
// 			RefreshKnobTime(pOtdrSet, 0);
			DispTransparent(150, 0x0, 0, 184, 310, 56); //灰度
			RunOtdrModule(ENUM_RESTART);

			while(GetWorkStatus() != ENUM_OTDR_STOPPED)	//测量停止后
			{
				MsecSleep(20);
			}

			RunOtdrModule(1);
		}
		else if(arg == 2)
		{
//             RefreshKnobTime(pOtdrSet, -1);
            RunOtdrModule(ENUM_RESET_TIME);
        }
        else if(arg == 3)
        {
//             RefreshKnobTime(pOtdrSet, -1);
        }
	}
	else
	{
		return;
	}
}

void CheckOtdrStop(int arg)
{
	if(GetWorkStatus() != ENUM_OTDR_STOPPED)	//正在测量
	{
		if(arg == 1)
		{
// 			SetAllDropEnable(pOtdrDrop, 0);
// 			RefreshDropTime(0);
// 			DispTransparent(150, 0x0, 167, 402, 528, 78); //灰度
// 			RefreshMenu(pOtdrMenu, -1, 0, (UINT16)(~(MENU_BACK | MENU_HOME)),
// 						0, NULL);
//             RefreshScreen(__FILE__, __func__, __LINE__);

			RunOtdrModule(ENUM_RESTART);

			while(GetWorkStatus() != ENUM_OTDR_STOPPED)	//测量停止后
			{
				MsecSleep(20);
			}

			RunOtdrModule(1);
		}
		else if(arg == 2)
		{
            RunOtdrModule(ENUM_RESET_TIME);
		}
	}
	else
	{
// 		DisplayOtdrDrop(pOtdrDrop);
		//DisplayWndMenu1(pOtdrMenu);
	}
}


//有光检查返回函数
void CheckLightCallBack(int iSelect)
{
	if(iSelect == 1)
	{
		RunOtdrModule(ENUM_IGNORE_EX_CHECK);
	}
	else
	{
		WriteOTDRMsgQueue(ENUM_OTDRMSG_STOP);
		ReportOtdrProgress(NULL);
	}
	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
}

//连接检查回调函数
void CheckConnectCallBack(int iSelect)
{
	if(iSelect == 1)
	{
		RunOtdrModule(ENUM_IGNORE_CON_CHECK);
	}
	else
	{
		WriteOTDRMsgQueue(ENUM_OTDRMSG_STOP);
		ReportOtdrProgress(NULL);
	}

	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
}

static void SaveAllCurves(PDISPLAY_INFO pDisplay)
{
    int iNum = GetCurveNum(pDisplay);
	int i;
	for (i = 0; i < iNum; i++)
	{
		//有数据并且已经分析
		if (CurveIsVaild(i, pDisplay) &&
			CurveIsAnalysised(i, pDisplay))
		{
			char filePath[F_NAME_MAX];
			char strFile[F_NAME_MAX];

			GetCurveFile(i, pDisplay, strFile);
            sprintf(filePath, "%s/%s", pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, strFile);

			if (access(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, F_OK) != 0)
			{
				if (mkdir(pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath, 0666))
					return;
			}

            if (CanWriteFile(filePath))
            {
				if (WriteSor(i, filePath) < 0)
				{
					break;
				}
                else
                {
                    SetSavedFlag(pDisplay, 1);
                }
            }
		}
	}
}

static void SetCurrentPath(void)
{
    if(pOtdrFileCurrPath)
    {
        char buf[512] = {0};
        getcwd(buf, 512);
        if(strncmp(buf, MntUsbDirectory, strlen(MntUsbDirectory)) != 0)
        {
            strcpy(pOtdrFileCurrPath, buf);
        }
        else
        {
            chdir(pOtdrFileCurrPath);
        }
    }
}

//打开SOR文件
int OtdrSorViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where)
{

	int iErr;
	LOG(LOG_INFO, "file name:%s\n", fileName);
#ifdef FORC
    if(strstr(fileName, ".cur") || strstr(fileName, ".CUR"))
    #else
    if(strstr(fileName, ".inno") || strstr(fileName, ".INNO")
        || strstr(fileName, ".cur") || strstr(fileName, ".CUR"))
    #endif
	{
	    iErr = ReadInno((char*)fileName);
	}
	else
	{
        iErr = ReadSor(0, (char*)fileName, 1);
	}
    

	if (!iErr)
	{
		GUIWINDOW* pDest = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
										FrmOtdrInit, FrmOtdrExit,
										FrmOtdrPaint, FrmOtdrLoop,
										FrmOtdrPause, FrmOtdrResume,
										NULL);
		SendWndMsg_WindowExit(from);
		SendSysMsg_ThreadCreate(pDest);
        if (where)
        {
            pOtdrEscapeTo = where;
        }
    }

    SetCurrentPath();
	return iErr;
}

//获得OTDR当前窗体的模式    普通模式或伸展模式
int GetOtdrCurrWnd(void)
{
    return sOtdrOpCard.iCurrWnd;
}

//切换到测量界面 传入1切换到测量界面， 0切换回来
int OtdrSwitchToMeas(int iSwitchDir)
{
    static int lastCard = 0;
    if(iSwitchDir)
    {
        lastCard = sOtdrOpCard.iCurrCard;
        if(lastCard != OTDR_OPC_MEASU)
        {
            OtdrOptionMeas(NULL, 0, NULL, 0);
        }
    }
    else
    {
        switch(lastCard)
        {
            case OTDR_OPC_SETUP:
                OtdrOptionOtdr(NULL, 0, NULL, 0);
                break;
            case OTDR_OPC_EVENT:
                OtdrOptionEvt(NULL, 0, NULL, 0);
                break;
            case OTDR_OPC_MEASU:
                break;
            default:
            break;
        }
    }
    return 0;
}

void otdrModifyCoorScale(int Mode)
{
    switch(Mode)
    {
        case KEYCODE_UP:
            OtdrZoomIn_y();
            break;
        case KEYCODE_DOWN:
            OtdrZoomOut_y();
            break;
        case KEYCODE_LEFT:
            OtdrZoomOut_x();
            break;
        case KEYCODE_RIGHT:
            OtdrZoomIn_x();
            break;
        default:
            break;
    }
	// if (2 < pOtdrMenu->iItemCnt)		//不在测量中
 	// {
    //  	switch (Mode)
 	//     {
 	// 	case KEYCODE_ENTER:
    //          ScreenShot();
 	// 		break;
 	// 	default:
 	// 		break;
    //  	}
    //  }
}

void otdrStartOrStopMeasure(void)
{
    OtdrMenuCall(0);
}

//刷新波形区域
//static void RefreshCurve(int value)
//{
//    SendCurveAdditionCmd(EN_ADD_REFSH, 0);
//}

//在scath模式下，坐标校正，当触点脱离曲线区域时也能够进行拖放
void OtdrScathCoorAdjust(unsigned int* CoorX, unsigned int* CoorY)
{
    GUIRECTANGLE * tmpPicInfo = NULL;
    if((NULL == pOtdrCoor) || (0 == iOtdrTouch) || (EN_SCATCH != sWinPara.enZoomMode))
    {
        return;
    }

    tmpPicInfo = &pOtdrCoor->pCurve->Visible.Area;
    if(*CoorX < tmpPicInfo->Start.x)
    {
        *CoorX = tmpPicInfo->Start.x; 
    }
    else if(*CoorX > tmpPicInfo->End.x)
    {
        *CoorX = tmpPicInfo->End.x;
    }
    else
    {
        //no handle
    }

    if(*CoorY < tmpPicInfo->Start.y)
    {
       *CoorY = tmpPicInfo->Start.y;
    }
    else if(*CoorY > tmpPicInfo->End.y)
    {
        *CoorY = tmpPicInfo->End.y;
    }
    else
    {
        //no handle
    }
    return;
}

//设置显示标记线信息
static void isFlushDisplayInfo(int iType, int* iCount)
{
	int iEnable[2] = { pUser_Settings->iWave[0], pUser_Settings->iWave[1] };

	(*iCount)++;
	if (iEnable[0] && iEnable[1])
	{
		if (((*iCount) == 2) && iType)
		{
			iReflushMarkedFlag = 1;
			(*iCount) = 0;
		}
		else
		{
			if (iType)
				iReflushMarkedFlag = 0;
		}
	}
	else
	{
		if (iType)
			iReflushMarkedFlag = 1;
		(*iCount) = 0;
	}
}

//测试otdr
void OtdrTest(int iRefreshPara)
{
	//取消界面缩放（shift使能）
	// iScaleFlag = 0;

	if (GetWorkStatus() == ENUM_OTDR_STOPPED)
	{
		SetDrawIsPass(0);
		pDisplay = pOtdrTopSettings->pDisplayInfo;
		if (CurveIsAnalysised(CURR_CURVE, pDisplay)
			&& CurveIsVaild(CURR_CURVE, pDisplay))
		{
			int iPromptSaveFlag = pOtdrTopSettings->pUser_Setting->sCommonSetting.iPromptSaveFlag;
			if ((!IsCurveSaved(pDisplay)) && iPromptSaveFlag)
			{
				unsigned int button = ShowStandardMessageBox(
					NULL,
					TransString("OTDR_SAVE_RESULT")
				);
				//sleep 200ms 防止界面出现覆盖
				MsecSleep(200);
				if (button == MessageBox_Yes)
				{
					SaveAllCurves(pDisplay);
				}
			}
		}
	}

	if ((sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP) && iRefreshPara)
	{
		SetPictureEnable(1, pScreenShot);
		RecvWaveArgs();
		if (sOtdrOpCard.iCurrWnd == COOR_LARGE)
		{
			RefreshOtdrCurvePara(pOtdrCurvePara);
		}
	}

	RunOtdrModule(1);
}

//选项响应函数
void SelectedOptionCallBack(int iSelectedOption, int iOption)
{
	switch (iSelectedOption)
	{
	case OTDR_OPC_SETUP:
		OtdrOpcOtdr();
		break;
	case OTDR_OPC_EVENT:
		OtdrOpcEvt();
		break;
	case OTDR_OPC_MEASU:
		OtdrOpcMeas();
		break;
	case OTDR_OPC_LINK_MAP:
		OtdrOpcLinkMap();
		break;
	default:
		break;
	}

	SetCoorSelectedOption(pOtdrCoor, iSelectedOption, 1);
	//设置并刷新波长
	// if (iSelectedOption >= (OPTION_NUM + 2))
	// {
		// OtdrSwitchWave(NULL, 0, NULL, iOption);
	// }
	SendLineCtlMoveMsg(EN_CTL_SWITCH, 0, 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
}

//处理enter键响应函数
static void HandleEnterKeyCallBack(void)
{
	switch (iSelectedOption)
	{
	case 4://set
		OtdrSetting(NULL, 0, NULL, 0);
		break;
	case 5://file
		OtdrFile(NULL, 0, NULL, 0);
		break;
    case 6: // wave
        //显示下一条曲线
        // DisplayNextCur();
        OtdrSwitchWave(NULL, 0, NULL, 0);
        break;
	default:
		if (iScaleFlag) //若处于缩放状态则恢复1：1
		{
			SendLineCtlMoveMsg(EN_CTL_RECOVER, 0, 0);
			// SetDrawMagnifier(0);
		}
		break;
	}
}

//移动标记线
static void MoveMarker(int index, int option, int iValue)
{
	int iMarker = index + 1;
	iOtdrTouch = iMarker;
	SendMakerCtlMsg(EN_MAKER_PATCH, iMarker);

	if (!option)//左移
	{
		SendMakerCtlMsg(EN_MAKER_ANTI, iValue);
	}
	else//右移
	{
		SendMakerCtlMsg(EN_MAKER_CLOCK, iValue);
	}
}

//缩放曲线
static void ZoomCurve(int iMarker, int iInOrOut, float fXScale, float fYScale)
{
	//标记
	iOtdrTouch = iMarker+1;
	SendMakerCtlMsg(EN_MAKER_PATCH, iMarker+1);
	if (!iInOrOut)//缩小
	{
		SendLineCtlScaleMsg(EN_CTL_ZOOMOUT, fXScale, fYScale);
	}
	else//放大
	{
		SendLineCtlScaleMsg(EN_CTL_ZOOMIN, fXScale, fYScale);
	}
}

//上键响应处理
static void HandleKeyUpDown(void)
{
	//曲线界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
	{
		if (iScaleFlag)
		{
			//沿着y轴放大曲线
			ZoomCurve(pOtdrCurvePara->iFocus, 1, 1.0f, 2.0f);
		}
		else
		{
			//向上移动选中的标记线选择框
			CurveParaKeyBoardCallBack(KEYCODE_UP);
			SendMakerCtlMsg(EN_MAKER_PATCH, pOtdrCurvePara->iFocus + 1);
		}
	}
	//事件界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_EVENT)
	{
		if (iScaleFlag)
		{
			//沿着y轴放大曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMIN, 1.0f, 2.0f);
		}
		else
		{
			//设置光标的位置
			SetEventFocus(pEvtTab, 0);
		}
	}
	//链路地图界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_LINK_MAP)
	{
		if (iScaleFlag)
		{
			//沿着y轴放大曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMIN, 1.0f, 2.0f);
		}
		else
		{
			//设置光标的位置
			SethOtdrLinkMapFocus(pOtdrLinkMap, 0);
		}
	}
}

//下键响应处理
static void HandleKeyDownDown(void)
{
	//曲线界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
	{
		if (iScaleFlag)
		{
			//沿着y轴缩小曲线
			ZoomCurve(pOtdrCurvePara->iFocus, 0, 1.0f, 2.0f);
		}
		else
		{
			//向下移动选中的标记线选择框
			CurveParaKeyBoardCallBack(KEYCODE_DOWN);
			SendMakerCtlMsg(EN_MAKER_PATCH, pOtdrCurvePara->iFocus + 1);
		}
	}
	//事件界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_EVENT)
	{
		if (iScaleFlag)
		{
			//沿着y轴缩小曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMOUT, 1.0f, 2.0f);
		}
		else
		{
			//设置事件表光标的位置
			SetEventFocus(pEvtTab, 1);
		}
	}
	//链路地图界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_LINK_MAP)
	{
		if (iScaleFlag)
		{
			//沿着y轴缩小曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMOUT, 1.0f, 2.0f);
		}
		else
		{
			//设置光标的位置
			SethOtdrLinkMapFocus(pOtdrLinkMap, 1);
		}
	}
}

//左移响应处理
static void HandleKeyLeftDown(void)
{
	int curveNum = GetCurveNum(pDisplay);
	//曲线参数界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
	{
		if (curveNum)
		{
			iReflushMarkedFlag = 1;
		}

		if (iScaleFlag)
		{
			//沿着x轴缩小曲线
			ZoomCurve(pOtdrCurvePara->iFocus, 0, 2.0f, 1.0f);
		}
		else
		{
			//向左移动选中的标记线
			MoveMarker(pOtdrCurvePara->iFocus, 0, 3); //move left
		}
	}
	//事件列表界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_EVENT)
	{
		if (iScaleFlag)
		{
			//沿着x轴缩小曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMOUT, 2.0f, 1.0f);
		}
		else
		{
			//设置光标的位置
			SetEventFocus(pEvtTab, 0);
		}
	}
	//链路地图界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_LINK_MAP)
	{
		if (iScaleFlag)
		{
			//沿着x轴缩小曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMOUT, 2.0f, 1.0f);
		}
		else
		{
			//设置光标的位置
			SethOtdrLinkMapFocus(pOtdrLinkMap, 0);
		}
	}
}

//右移响应处理
static void HandleKeyRightDown(void)
{
	int curveNum = GetCurveNum(pDisplay);
	//曲线参数界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_SETUP)
	{
		if (curveNum)
		{
			iReflushMarkedFlag = 1;
		}

		if (iScaleFlag)
		{
			//沿着x轴放大曲线
			ZoomCurve(pOtdrCurvePara->iFocus, 1, 2.0f, 1.0f);
		}
		else
		{
			//向右移动选中的标记线
			MoveMarker(pOtdrCurvePara->iFocus, 1, 3); //move right
		}
	}
	//事件列表界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_EVENT)
	{
		if (iScaleFlag)
		{
			//沿着x轴放大曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMIN, 2.0f, 1.0f);
		}
		else
		{
			//设置光标的位置
			SetEventFocus(pEvtTab, 1);
		}
	}
	//事件列表界面
	if (sOtdrOpCard.iCurrCard == OTDR_OPC_LINK_MAP)
	{
		if (iScaleFlag)
		{
			//沿着x轴放大曲线
			SendLineCtlScaleMsg(EN_CTL_ZOOMIN, 2.0f, 1.0f);
		}
		else
		{
			//设置光标的位置
			SethOtdrLinkMapFocus(pOtdrLinkMap, 1);
		}
	}
}
