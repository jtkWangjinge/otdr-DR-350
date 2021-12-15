/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmeventtab.c
* 摘    要：  实现主窗体frmeventtab的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/25
*
*******************************************************************************/

/********************************************
* 为实现frmeventtab控件而需要引用的其他头文件
********************************************/
#include "app_curve.h"
#include "app_frminit.h"
#include "app_unitconverter.h"

#include "guiphoto.h"

#include "wnd_frmeventtab.h"
#include "wnd_global.h"
#include "wnd_frmevtbtn.h"
#include "wnd_frmsliderblock.h"
#include "wnd_frmlinkmap.h"

/*********************************
* 定义frmeventab控件所需要全局变量
*********************************/
//定义事件类型图片
char *ppEventTypePicUnSlt[] = 
{
	BmpFileDirectory"eventtype0a.bmp",	//非反射事件
	BmpFileDirectory"eventtype1a.bmp",	//增益事件
	BmpFileDirectory"eventtype2a.bmp",	//反射事件
	BmpFileDirectory"eventtype3a.bmp",	//回波事件
	BmpFileDirectory"eventtype10a.bmp",	//末端事件
	BmpFileDirectory"eventtype20a.bmp",	//连续光纤事件
	BmpFileDirectory"eventtype21a.bmp",	//分析结束事件
	BmpFileDirectory"eventtypesa.bmp",	//起始事件
	BmpFileDirectory"eventtype5a.bmp",	//宏弯曲事件
    BmpFileDirectory"eventtype6a.bmp"	//区段事件
};
char *ppEventTypePicSlt[] = 
{
	BmpFileDirectory"eventtype0b.bmp",
	BmpFileDirectory"eventtype1b.bmp",
	BmpFileDirectory"eventtype2b.bmp",
	BmpFileDirectory"eventtype3b.bmp",
	BmpFileDirectory"eventtype10b.bmp",
	BmpFileDirectory"eventtype20b.bmp",
	BmpFileDirectory"eventtype21b.bmp",
	BmpFileDirectory"eventtypesb.bmp",
	BmpFileDirectory"eventtype5b.bmp",
	BmpFileDirectory"eventtype6b.bmp"
};

//互斥锁
static GUIMUTEX EvtTabMutex = PTHREAD_MUTEX_INITIALIZER;
static int iUnitConverterFlag = 0;              //单位转换的标志位
extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//设置顶层结构
/*************************************
* 定义frmeventab控件事件表所需要的控件
*************************************/
static GUIPICTURE *pEvtPicEvtBg = NULL;
//static GUIPICTURE *pEvtBtnPgDn = NULL;
//static GUIPICTURE *pEvtBtnPgUp = NULL;
//static GUIPICTURE *pEvtBtnNext = NULL;
//static GUIPICTURE *pEvtBtnPrev = NULL;
static GUIPICTURE *pEvtPicTitl = NULL;
static GUIPICTURE *pEvtPicSltBar[TAB_ROWS] = {NULL};

//页码
//static GUILABEL *pEvtLblPage = NULL;
//static GUICHAR *pEvtStrPage = NULL;
//static GUIPICTURE *pEvtBgPage = NULL;

//事件表所需要的标签 表头
static GUILABEL *pEvtLblTypeH = NULL;
static GUILABEL *pEvtLblNumbH = NULL;
static GUILABEL *pEvtLblDistH = NULL;
static GUILABEL *pEvtLblSLosH = NULL;
static GUILABEL *pEvtLblRefrH = NULL;
static GUILABEL *pEvtLblDecaH = NULL;

static GUILABEL *pEvtLblDistUnit = NULL;
static GUILABEL *pEvtLblSLosUnit = NULL;
static GUILABEL *pEvtLblRefrUnit = NULL;
static GUILABEL *pEvtLblDecaUnit = NULL;

//事件表所需要的文本 表头
static GUICHAR *pEvtStrTypeH = NULL;
static GUICHAR *pEvtStrNumbH = NULL;
static GUICHAR *pEvtStrDistH = NULL;
static GUICHAR *pEvtStrSLosH = NULL;
static GUICHAR *pEvtStrRefrH = NULL;
static GUICHAR *pEvtStrDecaH = NULL;
static GUICHAR *pEvtStrTLosH = NULL;

static GUICHAR *pEvtStrDistUnit = NULL;
static GUICHAR *pEvtStrSLosUnit = NULL;
static GUICHAR *pEvtStrRefrUnit = NULL;
static GUICHAR *pEvtStrDecaUnit = NULL;

//提示信息
static GUILABEL *pEvtLblPrompt = NULL;
static GUICHAR *pEvtStrPrompt = NULL;
//事件表所需要的标签 表头
static GUIPICTURE *pEvtPicNumber = NULL;				//事件类型标题背景
static GUIPICTURE *pEvtPicTypeD[TAB_ROWS] = {NULL};
static GUILABEL *pEvtLblNumbD[TAB_ROWS] = {NULL};
static GUILABEL *pEvtLblDistD[TAB_ROWS] = {NULL};
static GUILABEL *pEvtLblSLosD[TAB_ROWS] = {NULL};
static GUILABEL *pEvtLblRefrD[TAB_ROWS] = {NULL};
static GUILABEL *pEvtLblDecaD[TAB_ROWS] = {NULL};
//事件表所需要的文本 表头
static GUICHAR *pEvtStrNumbD[TAB_ROWS] = {NULL};
static GUICHAR *pEvtStrDistD[TAB_ROWS] = {NULL};
static GUICHAR *pEvtStrSLosD[TAB_ROWS] = {NULL};
static GUICHAR *pEvtStrRefrD[TAB_ROWS] = {NULL};
static GUICHAR *pEvtStrDecaD[TAB_ROWS] = {NULL};

//事件表所需要的事件类型图片
static char *pEvtPicTypeFocus[TAB_ROWS] = {NULL};
static char *pEvtPicTypeUnFocus[TAB_ROWS] = {NULL};

//侧边工具控件
static WNDEVTBTN *pEvtBtn = NULL;

//暂存当前的事件表
static EVENTS_TABLE *pEvents = NULL;
static struct Slider_Block* pSliderBlock = NULL;
static OTDR_LINK_MAP* pOtdrLinkMap = NULL;
/*****************************************
* 声明frmeventab控件内部所使用事件处理函数
*****************************************/
//选中条事件处理函数
static int EventSelectBar_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen);
//翻页按钮
//static int EventNext_Down(void *pInArg, int iInLen, 
//                          void *pOutArg, int iOutLen);
static int EventNext_Up(void *pInArg, int iInLen, 
                        void *pOutArg, int iOutLen);
//static int EventPrev_Down(void *pInArg, int iInLen, 
//                          void *pOutArg, int iOutLen);
static int EventPrev_Up(void *pInArg, int iInLen, 
                        void *pOutArg, int iOutLen);

//按钮回调函数组
//转换事件（事件表<->链路地图）
static int SwitchEvts_Down(void *pInArg);
static int AddEvts_Down(void *pInArg);
static int DelEvts_Down(void *pInArg);
//static int MaxEvts_Down(void *pInArg);
//static int SetStart_Down(void *pInArg);
//static int DelStart_Down(void *pInArg);
//static int NoUse_Down(void *pInArg);

static int InitEventTab(GUIWINDOW *pWnd, EVENTTAB *pEvents);
//绘制事件表中的通过/不通过图标
static void PaintEvtTabPassIcon(EVENTTAB *pEvtTab, int iPosition, int iIndex);
static int PaintEventTab(EVENTTAB *pEvents);
static int ExitEventTab(EVENTTAB *pEvents);
static int InitEventTabText(EVENTTAB *pEvents);
static int ExitEventTabText(EVENTTAB *pEvents);

static int AddEvtsTabComb(EVENTTAB *pEvents, GUIWINDOW *pWnd);
static int LoginEvtsTabMsg(EVENTTAB *pEvents);
static int DelEvtsTabComb(EVENTTAB *pEvents, GUIWINDOW *pWnd);
static int LogOutEvtsTabMsg(EVENTTAB *pEvents);

static int GetEvtContent(EVENTTAB *pEvtsTab);
static int CalcEvtPageTatall(EVENTTAB *pEvtsTab);
static void UpdatePageNum(EVENTTAB *pEvtsTab);
static void FreeEvtTabStr(int iLblIdx);
static void TransEvtInfo2Str(EVENTTAB *pEvtsTab, int iLblIdx, int iEvtIdx);
static void UpdateEvtTabLbl(EVENTTAB *pEvtsTab, int iLblIdx, int isGray);
static void UpdateEvtTabPage(EVENTTAB *pEvtTab, int iLblIdx, int iEvtIdx);
static void UpdateEventDisp(EVENTTAB *pEvtsTab);
static void EvtsTabDrawBg(int row, int column, int color, EVENTTAB *pEvtTab);
static void SetEventTabTotalNum(EVENTTAB *pEvtTab);

//重置事件类型数值
static void ResetEventTypeValue(EVENTS_INFO* pEvtsInfo, EVTTABINFO* pEvtTabInfo);
//设置事件类型
static void SetEventType(EVENTTAB* pEvtTab);
//重置事件编号的数值
static void ResetEventNumValue(EVENTTAB* pEvtTab, int index);
//设置事件编号
static void SetEventNum(EVENTTAB* pEvtTab);
//转换时间位置单位
static void SwitchEventPostionUnit(EVENTTAB* pEvtTab, int index, float fDistance);
//设置事件位置
static void SetEventPosition(EVENTTAB* pEvtTab);
//设置事件损耗
static void SetEventLoss(EVENTTAB* pEvtTab);
//设置事件的反射率
static void SetEventReflectivity(EVENTTAB* pEvtTab);
//转换事件衰减单位
static void SwitchEventAttenuationUnit(EVENTTAB* pEvtTab, EVENTS_INFO* pEvtsInfo, int index);
//设置事件衰减
static void SetEventAttenuation(EVENTTAB* pEvtTab);
//设置事件累积损耗
static void SetEventSum(EVENTTAB* pEvtTab);
//设置事件通过
static void SetEventIsPass(EVENTTAB* pEvtTab);
//设置事件的起始index以及末端index
static void SetEventStartAndEndIndex(EVENTTAB* pEvtTab, int* pStartIndex, int* pEndIndex);
//设置起始事件类型
static void SetStartEventType(EVENTTAB* pEvtTab, int iStartIndex,  int iEndIndex);
//设置末端事件的事件类型
static void SetEndEventType(EVENTTAB* pEvtTab, int iStartIndex,  int iEndIndex, int iEventsNumber);
//DEBUG打印
#ifdef EVT_DBUG
void evt_dbg(EVENTTAB *pEvtsTab)
{
	printf("iEvtTotallEvt = %d\niEvtPageTotall = %d\niEvtFocus= %d\n"
		   "iEvtTabPage = %d\niEvtDispIdx = %d\n\n\n",
		   pEvtsTab->iEvtTotallEvt, pEvtsTab->iEvtPageTotall,
		   pEvtsTab->iEvtFocus, pEvtsTab->iEvtTabPage,
		   pEvtsTab->iEvtDispIdx);	
}
#endif

static void SetEventTabTotalNum(EVENTTAB *pEvtTab)
{
    int iEventsNumber = pEvents->iEventsNumber;
    int iDisplaySectionEvent = pOtdrTopSettings->pUser_Setting->sCommonSetting.iDisplaySectionEvent;
    
    if (pEvtTab->iDisplaySectionEvt != iDisplaySectionEvent)
    {
        pEvtTab->iDisplaySectionEvt = iDisplaySectionEvent;
    }
    
	if ((pEvtTab->iDisplaySectionEvt == 1) && (iEventsNumber != 0))
    {
        pEvtTab->iEvtTotallEvt = 2*iEventsNumber - 1;
    }
    else
    {
        pEvtTab->iEvtTotallEvt = iEventsNumber;
    }
}

void printEventTable(void)
{
    int i = 0;
    printf("\n=============\n");
    printf("pEvents->iLaunchFiberIndex = %d\n", pEvents->iLaunchFiberIndex);
    printf("pEvents->iRecvFiberIndex = %d\n", pEvents->iRecvFiberIndex);
    printf("pEvents->iIsThereASpan = %d\n", pEvents->iIsThereASpan);
    for(i = 0; i < pEvents->iEventsNumber; i++)
    {
        printf("begin_%d = %d\n", i, pEvents->EventsInfo[i].iBegin);
        printf("iEnd_%d = %d\n", i, pEvents->EventsInfo[i].iEnd);
        printf("iStyle_%d = %d\n", i, pEvents->EventsInfo[i].iStyle);
        //printf("iPulse_%d = %d\n", i, pEvents->EventsInfo[i].iPulse);
        printf("fLoss_%d = %f\n", i, pEvents->EventsInfo[i].fLoss);
        //printf("fDelta_%d = %f\n", i, pEvents->EventsInfo[i].fDelta);
        printf("fReflect_%d = %f\n", i, pEvents->EventsInfo[i].fReflect);
        printf("iContinueLength_%d = %d\n", i, pEvents->EventsInfo[i].iContinueLength);
        printf("fAttenuation_%d = %f\n", i, pEvents->EventsInfo[i].fAttenuation);
        //printf("fContinueAttenuation_%d = %f\n", i, pEvents->EventsInfo[i].fContinueAttenuation);
        printf("fTotalLoss_%d = %f\n", i, pEvents->EventsInfo[i].fTotalLoss);
        printf("fEventsPosition_%d = %f\n", i, pEvents->EventsInfo[i].fEventsPosition);
        //printf("fEventsEndPosition_%d = %f\n", i, pEvents->EventsInfo[i].fEventsEndPosition);
        //printf("fEventReturnLoss_%d = %f\n", i, pEvents->EventsInfo[i].fEventReturnLoss);
        //printf("iSmallReflectFlag_%d = %d\n", i, pEvents->EventsInfo[i].iSmallReflectFlag);
        //printf("iSmallLossFlag_%d = %d\n", i, pEvents->EventsInfo[i].iSmallLossFlag);
        printf("------------------------\n");
    }
}

//创建事件表并初始化
EVENTTAB * CreateEventTab(int iColNum, GUIWINDOW *pWnd)
{
	//临时变量
	EVENTTAB *pEvtTab = NULL;
    iUnitConverterFlag = pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag;
	//获取互斥锁
	if (MutexTrylock(&EvtTabMutex))
	{
		LOG(LOG_ERROR, "Already had a event table\n");
		return NULL;
	}
	
	//为事件表申请资源
	pEvtTab = (EVENTTAB *)calloc(1, sizeof(EVENTTAB));
	if (NULL == pEvtTab)
	{
		return NULL;
	}

	pEvents = (EVENTS_TABLE *)calloc(1, sizeof(EVENTS_TABLE));
	if (NULL == pEvents)
	{
		return NULL;
	}
	
	//重新对各曲线事件列表做阈值判断
	ReevaluateEvent();

	//初始化该结构体元素
	GetCurveEvents(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, pEvents);
    SetEventTabTotalNum(pEvtTab);
	pEvtTab->iColNum = iColNum;
	pEvtTab->iEvtDispIdx = 0;
	pEvtTab->iEvtFocus = 0;
	pEvtTab->iEvtTabPage = 0;
	pEvtTab->iCsrOnEvt = 1;
	pEvtTab->iEnable = 0; //默认不使能
	pEvtTab->iTouchEnable = 1;
	pEvtTab->iNeedGetEvtContent = 1;
	pEvtTab->iEvtPageTotall = CalcEvtPageTatall(pEvtTab);
	//转换事件表
	GetEvtContent(pEvtTab);
	
	//初始化控件
	InitEventTab(pWnd, pEvtTab);

	//调试
	evt_dbg(pEvtTab);

	return pEvtTab;
}

//退出时释放事件表控件所申请的资源
void DestroyEventTab(EVENTTAB **ppEvtTab)
{
	//销毁控件
	ExitEventTab(*ppEvtTab);

	//释放资源
	free(*ppEvtTab);
	*ppEvtTab = NULL;

	free(pEvents);
	pEvents = NULL;

	free(pSliderBlock);
	pSliderBlock = NULL;

	free(pOtdrLinkMap);
	pOtdrLinkMap = NULL;

	//释放互斥锁
	MutexUnlock(&EvtTabMutex);
}

//绘制事件表
int DisplayEvtTab(EVENTTAB *pEvtTab)
{
	if (0 == pEvtTab->iEnable)
	{
		return 0;
	}

	return PaintEventTab(pEvtTab);
}

//初始化事件表属性
int ResetTabAttr(EVENTTAB *pEvtTab, int iColNum)
{	
	//重新获取事件表
	GetCurveEvents(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, pEvents);

	//初始化该结构体元素
	SetEventTabTotalNum(pEvtTab);
	pEvtTab->iColNum = iColNum;
	pEvtTab->iEvtDispIdx = 0;
	pEvtTab->iEvtFocus = 0;
	pEvtTab->iEvtTabPage = 0;
	pEvtTab->iCsrOnEvt = 1;
	pEvtTab->iNeedGetEvtContent = 1;
	pEvtTab->iEvtPageTotall = CalcEvtPageTatall(pEvtTab);

	evt_dbg(pEvtTab);

	return 0;
}

//初始化事件表属性
int ClrEvtTab(EVENTTAB *pEvtTab)
{
	//初始化该结构体元素
	pEvtTab->iEvtTotallEvt = 0;
	pEvtTab->iEvtDispIdx = 0;
	pEvtTab->iEvtFocus = 0;
	pEvtTab->iEvtTabPage = 0;
	pEvtTab->iCsrOnEvt = 1;
	pEvtTab->iNeedGetEvtContent = 0;
	pEvtTab->iEvtPageTotall = 1;

	return 0;
}

//刷新事件表
int FlushEvtTab(EVENTTAB *pEvtTab)
{
	if (0 == pEvtTab->iEnable)
	{
		return 0;
	}

	//转换事件信息到字符串
	GetEvtContent(pEvtTab);

	//更新事件显示信息
	UpdateEventDisp(pEvtTab);
	
	//绘制事件信息
	PaintEventTab(pEvtTab);

	//设置光标位置（为了方便区分链路地图的事件图标）
	SetCursorAtEvent(pEvtTab, 0, 0);

	return 0;
}

//获取事件表
EVENTS_TABLE *GetEvtTabEvents(void)
{
	return pEvents;
}

//设置控件的使能状态
int SetEvtTabEnable(EVENTTAB *pEvtTab, int iEnable, GUIWINDOW *pWnd)
{
	//判断使能状态
	if (pEvtTab->iEnable != iEnable)
	{
		if (iEnable)
		{
			AddEvtsTabComb(pEvtTab, pWnd);
			LoginEvtsTabMsg(pEvtTab);
		}
		else
		{	
			DelEvtsTabComb(pEvtTab, pWnd);
			LogOutEvtsTabMsg(pEvtTab);
		}

		//设置使能标志
		pEvtTab->iEnable = iEnable;
	}

	//设置按钮控件
	SetEvtBtnEnable(pEvtBtn, iEnable, pWnd);
	
	return 0;
}

//
int SetEvtTouchEnable(EVENTTAB *pEvtTab, int iEnable)
{
	//判断使能状态
	if (pEvtTab->iTouchEnable != iEnable)
	{
		//设置使能标志
		pEvtTab->iTouchEnable = iEnable;
	}

	SetBtnTouchEnable(pEvtBtn, iEnable);

	return 0;
}

//初始化事件表
static int InitEventTab(GUIWINDOW *pWnd, EVENTTAB *pEvtTab)
{
    //临时变量定义
    int i, j;

	BTNFUNC ppFunc[] = {
		SwitchEvts_Down, AddEvts_Down, DelEvts_Down
	};

	//初始化文本资源
	InitEventTabText(pEvtTab);
	//事件表的底部按钮以及其他背景
	//pEvtBtnPrev = CreatePicture(5, 221, 38, 17, BmpFileDirectory"event_prev_unpress.bmp");
	//pEvtBtnNext = CreatePicture(114, 221, 38, 17, BmpFileDirectory"event_next_unpress.bmp");
	//pEvtBgPage = CreatePicture(50, 221, 57, 17, BmpFileDirectory"bg_event_page.bmp");
    //创建页码标签
	//pEvtLblPage = CreateLabel(53, 221, 50, 16, pEvtStrPage);
	//SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblPage);
	//SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pEvtLblPage);
	//创建事件表所需要的标签 表内容 选中条等
	for (i = pEvtTab->iColNum - 1, j = 0; i >=0 ; --i, ++j) 
	{
		pEvtPicSltBar[i] = CreatePicture(1, 450 - j*30, 617, 29, BmpFileDirectory"event_bar.bmp");
		pEvtLblNumbD[i] = CreateLabel(1, 450 - j * 30 + 8, 69, 24, pEvtStrNumbD[i]);
		pEvtPicTypeD[i] = CreatePicture(93, 450 - j*30, 25, 29, pEvtPicTypeUnFocus[pEvtTab->sEvtTab[i].iType]);
		pEvtLblDistD[i] = CreateLabel(141, 450 - j*30+8, 119, 24, pEvtStrDistD[i]);
		pEvtLblSLosD[i] = CreateLabel(261, 450 - j*30+8, 119, 24, pEvtStrSLosD[i]);
		pEvtLblRefrD[i] = CreateLabel(380, 450 - j*30+8, 119, 24, pEvtStrRefrD[i]);
		pEvtLblDecaD[i] = CreateLabel(506+7, 450 - j*30+8, 100, 24, pEvtStrDecaD[i]);

		SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblNumbD[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblDistD[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblSLosD[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblRefrD[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblDecaD[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblRefrD[i]);

		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblNumbD[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblDistD[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblSLosD[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblRefrD[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblDecaD[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblRefrD[i]);
	}
	//设置焦点事件为选中状态
	SetPictureBitmap(pEvtPicTypeFocus[pEvtTab->iEvtFocus], 
					 pEvtPicTypeD[pEvtTab->iEvtFocus]);
	//事件类型标题
	pEvtPicNumber = CreatePicture(28, 324, 16, 16, BmpFileDirectory"event_type.bmp");
 //   int hw = 32;
 //   int iHeaderY = 0;// = 409 - ((j-1)*24 + hw);
 //   int iLabelY = 0; // iHeaderY + 8;
 //   //事件列表只有一列的时候出于大屏模式
 //   if (j == 1) 
 //   {
 //       hw = 25;
 //       iHeaderY = 409 - ((j-1)*24 + hw);
 //       iLabelY = iHeaderY + 3;
 //   }
 //   else
 //   {
 //       hw = 32;
 //       iHeaderY = 409 - ((j-1)*24 + hw);
 //       iLabelY = iHeaderY + 8;
 //   }

	pEvtPicEvtBg = CreatePicture(0, 305+55, 640, 160, BmpFileDirectory"event_bg.bmp");
    pEvtPicTitl = CreatePicture(0, 305, 640, 55, BmpFileDirectory"event_tab_h.bmp");

    pEvtLblNumbH = CreateLabel(1, 324, 69, 24, pEvtStrNumbH);
	pEvtLblTypeH = CreateLabel(71, 324, 69, 24, pEvtStrTypeH);

    pEvtLblDistH = CreateLabel(141, 315, 119, 24, pEvtStrDistH);
	pEvtLblDistUnit = CreateLabel(141+30, 315+24, 60, 17, pEvtStrDistUnit);

    pEvtLblSLosH = CreateLabel(261, 315, 119, 24, pEvtStrSLosH);
	pEvtLblSLosUnit = CreateLabel(261+30, 315+24, 55, 17, pEvtStrSLosUnit);
	
	pEvtLblRefrH = CreateLabel(380, 315, 119, 24, pEvtStrRefrH);
	pEvtLblRefrUnit = CreateLabel(380+30, 315+24, 55, 17, pEvtStrRefrUnit);

    pEvtLblDecaH = CreateLabel(506+19, 315, 100, 24, pEvtStrDecaH);
	pEvtLblDecaUnit = CreateLabel(506+42, 315+24, 55, 17, pEvtStrDecaUnit);

	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblTypeH);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblNumbH);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblDistH);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblSLosH);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblRefrH);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblDecaH);

	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblDistUnit);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblSLosUnit);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblRefrUnit);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pEvtLblDecaUnit);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblTypeH);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblNumbH);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblDistH);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblSLosH);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblRefrH);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblDecaH);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblDistUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblSLosUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblRefrUnit);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pEvtLblDecaUnit);

    pEvtBtn = CreateEvtBtn(160, 221, 0, 0, ppFunc, pEvtTab);
	pSliderBlock = CreateSliderBlock(619, 360, pEvtTab->iEvtPageTotall, pEvtTab->iEvtTabPage);
	pOtdrLinkMap = CreateOtdrLinkMap(0, 279, 640, 175, pWnd);
	handleEventList(pOtdrLinkMap, pEvents);
	//提示信息
	pEvtLblPrompt = CreateLabel(15, 279+33+132+6, 400, 16, pEvtStrPrompt);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pEvtLblPrompt);
	return 0;
}

//销毁事件表控件的GUI资源
static int ExitEventTab(EVENTTAB *pEvtTab)
{
 	int iTmp;
	
	//销毁事件表格控件
	DestroyPicture(&pEvtPicEvtBg);
	//DestroyPicture(&pEvtBtnNext);
	//DestroyPicture(&pEvtBtnPrev);
 //   DestroyPicture(&pEvtBgPage);
	DestroyPicture(&pEvtPicTitl);
	//DestroyLabel(&pEvtLblPage);

	//销毁表所需要的标签 表头
	DestroyLabel(&pEvtLblTypeH);
	DestroyLabel(&pEvtLblNumbH);
	DestroyLabel(&pEvtLblDistH);
	DestroyLabel(&pEvtLblSLosH);
	DestroyLabel(&pEvtLblRefrH);
	DestroyLabel(&pEvtLblDecaH);

	//销毁事件表格所用资源
	for (iTmp = 0; iTmp < pEvtTab->iColNum; ++iTmp) 
	{
		DestroyPicture(&pEvtPicSltBar[iTmp]);
		DestroyPicture(&pEvtPicTypeD[iTmp]);
		DestroyLabel(&pEvtLblNumbD[iTmp]);
		DestroyLabel(&pEvtLblDistD[iTmp]);
		DestroyLabel(&pEvtLblSLosD[iTmp]);
		DestroyLabel(&pEvtLblRefrD[iTmp]);
		DestroyLabel(&pEvtLblDecaD[iTmp]);
	}

	DestroyPicture(&pEvtPicNumber);
	DestroyEvtBtn(&pEvtBtn);
	DestroySliderBlock(&pSliderBlock);
	DestroyOtdrLinkMap(&pOtdrLinkMap);
	//提示信息
	DestroyLabel(&pEvtLblPrompt);

	ExitEventTabText(pEvtTab);

	return 0;
}

//绘制事件表中的通过/不通过图标
static void PaintEvtTabPassIcon(EVENTTAB *pEvtTab, int iPosition, int iIndex)
{
    EVTTABINFO *eventInfo = &pEvtTab->sEvtTab[iPosition+iIndex];
    
    if (pEvtTab->iDisplaySectionEvt)
    {
        if (pEvtTab->sEvtTab[iPosition+iIndex].iType == 9)
        {
            eventInfo->iIsAttenPass ? 0 : EvtsTabDrawBg(iIndex+1, 6, 0x00ff0000, pEvtTab);
        }
        else
        {
            eventInfo->iIsLossPass ? 0 : EvtsTabDrawBg(iIndex+1, 4, 0x00ff0000, pEvtTab);
            eventInfo->iIsReflectPass ? 0 : EvtsTabDrawBg(iIndex+1, 5, 0x00ff0000, pEvtTab);
        }
    }
    else
    {
        eventInfo->iIsLossPass ? 0 : EvtsTabDrawBg(iIndex+1, 4, 0x00ff0000, pEvtTab);
        eventInfo->iIsReflectPass ? 0 : EvtsTabDrawBg(iIndex+1, 5, 0x00ff0000, pEvtTab);
        eventInfo->iIsAttenPass ? 0 : EvtsTabDrawBg(iIndex+1, 6, 0x00ff0000, pEvtTab);
    }
}

//绘制事件表
static int PaintEventTab(EVENTTAB *pEvtTab)
{
	int i, iTmp;
	int iPosition = 0;
	//计算当前页显示几个事件 
	iTmp = (pEvtTab->iEvtTabPage == pEvtTab->iEvtPageTotall - 1) ? 
			(pEvtTab->iEvtTotallEvt % pEvtTab->iColNum) : pEvtTab->iColNum;
    iPosition = pEvtTab->iEvtTabPage * pEvtTab->iColNum;
    
	if ((0 != pEvtTab->iEvtTotallEvt)
		&& (0 == iTmp))
	{
		iTmp = pEvtTab->iColNum;
	}	

	if (pOtdrLinkMap->iEnabled)//显示链路地图
	{
		handleEventList(pOtdrLinkMap, pEvents);
		DisplayOtdrLinkMap(pOtdrLinkMap, pEvtTab->iEvtDispIdx);
	}
	else//显示事件表
	{
		//显示事件表格控件
		DisplayPicture(pEvtPicEvtBg);
		DisplayPicture(pEvtPicTitl);
		DisplayPicture(pEvtPicNumber);
		//DisplayPicture(pEvtBtnNext);
		//DisplayPicture(pEvtBtnPrev);
		//DisplayPicture(pEvtBgPage);
		//DisplayLabel(pEvtLblPage);

		//显示表所需要的标签 表头
		DisplayLabel(pEvtLblTypeH);
		// DisplayLabel(pEvtLblNumbH);
		DisplayLabel(pEvtLblDistH);
		DisplayLabel(pEvtLblSLosH);
		DisplayLabel(pEvtLblRefrH);
		DisplayLabel(pEvtLblDecaH);

		DisplayLabel(pEvtLblDistUnit);
		DisplayLabel(pEvtLblSLosUnit);
		DisplayLabel(pEvtLblRefrUnit);
		DisplayLabel(pEvtLblDecaUnit);

		//显示滑块
		DisplaySliderBlock(pSliderBlock);
		//显示选中条
		if (iTmp)
		{
			DisplayPicture(pEvtPicSltBar[pEvtTab->iEvtFocus]);
		}

		//销毁事件表格所用资源
		for (i = 0; i < iTmp; ++i)
		{
			PaintEvtTabPassIcon(pEvtTab, iPosition, i);

			DisplayPicture(pEvtPicTypeD[i]);
			DisplayLabel(pEvtLblNumbD[i]);
			DisplayLabel(pEvtLblDistD[i]);
			DisplayLabel(pEvtLblSLosD[i]);
			DisplayLabel(pEvtLblRefrD[i]);
			DisplayLabel(pEvtLblDecaD[i]);
		}

		//刷新滑块
		FlushSliderBlock(pSliderBlock, pEvtTab->iEvtTabPage, pEvtTab->iEvtPageTotall);
		//显示提示信息
		// DisplayLabel(pEvtLblPrompt);
	}
	//显示按钮控件
	//PanitEvtBtn(pEvtBtn);

	//灰度
	//if (!pEvtTab->iTouchEnable)
	//{
	//	GUIVISIBLE *pBgV = &pEvtPicEvtBg->Visible;
	//	DispTransparent(100, 0x0, pBgV->Area.Start.x, pBgV->Area.Start.y - 14,
	//					680, pBgV->Area.End.y - pBgV->Area.Start.y + 25);
	//}
	
	return 0;
}

//初始化文本资源
static int InitEventTabText(EVENTTAB *pEvtTab)
{
	//临时变量定义
	int i, iEvtIndex;
	char strTmp[16] = {0};
	
	//计算事件起始索引
	iEvtIndex = pEvtTab->iEvtTabPage * pEvtTab->iColNum;

	pEvtStrTypeH = GetCurrLanguageText(OTDR_LBL_STYLE);
	pEvtStrNumbH = GetCurrLanguageText(OTDR_LBL_NUMBER);

	//距离（带单位）
	// unsigned int distIndex[UNIT_COUNT] = {
	// 	OTDR_LBL_DISTANCE_M, OTDR_LBL_DISTANCE_KM,
	// 	OTDR_LBL_DISTANCE_FT, OTDR_LBL_DISTANCE_KFT,
	// 	OTDR_LBL_DISTANCE_YD, OTDR_LBL_DISTANCE_MILE
	// };
	// int distUnit = GetCurrSystemUnit(MODULE_UNIT_OTDR);
	// if (distUnit >= UNIT_M && distUnit <= UNIT_MI)
	// {
	// 	pEvtStrDistH = GetCurrLanguageText(distIndex[distUnit]);
	// }

	pEvtStrDistH = GetCurrLanguageText(OTDR_LBL_DISTANCE);
	//拼接距离单位信息
	char distUnit[10] = {0};
	char *pDistUnit = GetCurrSystemUnitString(MODULE_UNIT_OTDR);
	sprintf(distUnit, "(%s)", pDistUnit);
	pEvtStrDistUnit = TransString(distUnit);
	GuiMemFree(pDistUnit);

	pEvtStrSLosH = GetCurrLanguageText(OTDR_LBL_LOSS);
	pEvtStrSLosUnit = TransString("(dB)");

	pEvtStrRefrH = GetCurrLanguageText(OTDR_LBL_REF);
	pEvtStrRefrUnit = TransString("(dB)");

	pEvtStrDecaH = GetCurrLanguageText(OTDR_LBL_ATT);

	pEvtStrDecaUnit = TransString("(dB/km)");

	pEvtStrTLosH = TransString("EVENT_LBL_SUM");

	//事件表个字符显示
	for (i = 0; i < pEvtTab->iColNum; ++i, ++iEvtIndex)
	{
		pEvtStrNumbD[i] = TransString(pEvtTab->sEvtTab[iEvtIndex].pNum);
        pEvtStrDistD[i] = TransString(pEvtTab->sEvtTab[iEvtIndex].pDist);		
		pEvtStrSLosD[i] = TransString(pEvtTab->sEvtTab[iEvtIndex].pLoss);				 
		pEvtStrRefrD[i] = TransString(pEvtTab->sEvtTab[iEvtIndex].pRef);		
		pEvtStrDecaD[i] = TransString(pEvtTab->sEvtTab[iEvtIndex].pDec);			
	}

	//页码
	sprintf(strTmp, "%d/%d", pEvtTab->iEvtTabPage + 1, pEvtTab->iEvtPageTotall);
	//pEvtStrPage =  TransString(strTmp);
	//提示信息
	pEvtStrPrompt = GetCurrLanguageText(OTDR_LBL_SWITCH_LINK_MAP);

    return 0;
}

//释放文本资源
static int ExitEventTabText(EVENTTAB *pEvtTab)
{
	int iTmp = 0;
	
	GuiMemFree(pEvtStrTypeH);
	GuiMemFree(pEvtStrNumbH);
	GuiMemFree(pEvtStrDistH);
	GuiMemFree(pEvtStrSLosH);
	GuiMemFree(pEvtStrRefrH);
	GuiMemFree(pEvtStrDecaH);
	GuiMemFree(pEvtStrTLosH);

	GuiMemFree(pEvtStrDistUnit);
	GuiMemFree(pEvtStrSLosUnit);
	GuiMemFree(pEvtStrRefrUnit);
	GuiMemFree(pEvtStrDecaUnit);
	
	//页码
	//GuiMemFree(pEvtStrPage);
	//提示信息
	GuiMemFree(pEvtStrPrompt);
	//释放表格文本
	for (iTmp = 0; iTmp < pEvtTab->iColNum; ++iTmp) 
	{	
		FreeEvtTabStr(iTmp);
	}
	
    return 0;
}


//内部获得锁 不能在持有互斥锁的情况下调用
static int AddEvtsTabComb(EVENTTAB *pEvtTab, GUIWINDOW *pWnd)
{
	int iTmp;
	//AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
	//			  pEvtBtnNext, pWnd);
	//AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
	//			  pEvtBtnPrev, pWnd);
	for (iTmp = 0; iTmp < pEvtTab->iColNum; ++iTmp)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pEvtPicSltBar[iTmp], pWnd);
	}

	return 0;
}

//内部获得锁 不能在持有互斥锁的情况下调用
static int LoginEvtsTabMsg(EVENTTAB *pEvtTab)
{
	int iTmp;
   	GUIMESSAGE *pMsg = NULL;
	
	pMsg = GetCurrMessage();

	//翻页按钮
	//LoginMessageReg(GUIMESSAGE_TCH_DOWN, pEvtBtnNext, 
 //                   EventNext_Down, (void *)pEvtTab, 0, pMsg);
	//LoginMessageReg(GUIMESSAGE_TCH_DOWN, pEvtBtnPrev, 
 //                   EventPrev_Down, (void *)pEvtTab, 0, pMsg);
	//LoginMessageReg(GUIMESSAGE_TCH_UP, pEvtBtnNext, 
 //                   EventNext_Up, (void *)pEvtTab, 0, pMsg);
	//LoginMessageReg(GUIMESSAGE_TCH_UP, pEvtBtnPrev, 
 //                   EventPrev_Up, (void *)pEvtTab, 0, pMsg);

	//选中条
	for (iTmp = 0; iTmp < pEvtTab->iColNum; ++iTmp) 
	{
		 LoginMessageReg(GUIMESSAGE_TCH_DOWN, pEvtPicSltBar[iTmp], 
                    	 EventSelectBar_Down, (void *)pEvtTab, iTmp, pMsg);
	}

	return 0;
}

//内部获得锁 不能在持有互斥锁的情况下调用
static int DelEvtsTabComb(EVENTTAB *pEvtTab, GUIWINDOW *pWnd)
{
	int iTmp;

	//DelWindowComp(pEvtBtnNext, pWnd);
	//DelWindowComp(pEvtBtnPrev, pWnd);
	for (iTmp = 0; iTmp < pEvtTab->iColNum; ++iTmp)
	{
		DelWindowComp(pEvtPicSltBar[iTmp], pWnd);
	}

	return 0;
}

//内部获得锁 不能在持有互斥锁的情况下调用
static int LogOutEvtsTabMsg(EVENTTAB *pEvtTab)
{
	int iTmp;
   	GUIMESSAGE *pMsg = NULL;

	pMsg = GetCurrMessage();

	//LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pEvtBtnNext, pMsg);
	//LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pEvtBtnPrev, pMsg);
	//LogoutMessageReg(GUIMESSAGE_TCH_UP, pEvtBtnNext, pMsg);
	//LogoutMessageReg(GUIMESSAGE_TCH_UP, pEvtBtnPrev, pMsg);
	for (iTmp = 0; iTmp < pEvtTab->iColNum; ++iTmp) 
	{
		 LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pEvtPicSltBar[iTmp], pMsg);
	}

	return 0;
}

/******************************************************************************/				  
//重置事件类型数值
static void ResetEventTypeValue(EVENTS_INFO* pEvtsInfo, EVTTABINFO* pEvtTabInfo)
{
    if (pEvtsInfo->iStyle >= 10) {
		pEvtTabInfo->iType= 4;
	}
	if (20 == pEvtsInfo->iStyle) {
		pEvtTabInfo->iType = 5;
	} 
    else if (21 == pEvtsInfo->iStyle) {
		pEvtTabInfo->iType = 6;
	}
    else if (5 == pEvtsInfo->iStyle) {
        //宏弯曲事件
        pEvtTabInfo->iType = 8;
    }
}

//设置事件类型
static void SetEventType(EVENTTAB* pEvtTab)
{
    int i;
    for (i = 0; i < pEvents->iEventsNumber; ++i)
	{
        EVENTS_INFO* pEvtsInfo = &(pEvents->EventsInfo[i]);
        int iStyle = pEvtsInfo->iStyle;
        if (pEvtTab->iDisplaySectionEvt)
        {
            if (i != pEvents->iEventsNumber - 1)
            {
                pEvtTab->sEvtTab[2*i + 1].iType = 9;
            }
            pEvtTab->sEvtTab[2*i].iType = iStyle;
            ResetEventTypeValue(pEvtsInfo, &(pEvtTab->sEvtTab[2*i]));
        }
        else
        {
            pEvtTab->sEvtTab[i].iType = iStyle;
            ResetEventTypeValue(pEvtsInfo, &(pEvtTab->sEvtTab[i]));
        }
    }
    
}
//重置事件编号的数值
static void ResetEventNumValue(EVENTTAB* pEvtTab, int index)
{
    int iEventsNumber = 0;
    int iEndIndex = 0;
    int iStartIndex = 0;
    int i = 0;
    memset(pEvtTab->sEvtTab[index].pNum, 0, sizeof(pEvtTab->sEvtTab[index].pNum));
    if (pEvtTab->iDisplaySectionEvt)
    {
        if (pEvents->iEventsNumber != 0)
        {
            iEventsNumber = 2*pEvents->iEventsNumber - 1;
            iStartIndex = (pEvents->iLaunchFiberIndex > 0) ? 2*pEvents->iLaunchFiberIndex : 2*pEvents->iEventsStart;
            iEndIndex = (pEvents->iRecvFiberIndex > 0) ? 2*pEvents->iRecvFiberIndex: iEventsNumber - 1;
            i = (index - iStartIndex)/2;
        }
    }
    else
    {
        iEventsNumber = pEvents->iEventsNumber;
        iStartIndex = (pEvents->iLaunchFiberIndex > 0) ? pEvents->iLaunchFiberIndex : pEvents->iEventsStart;
        iEndIndex = (pEvents->iRecvFiberIndex > 0) ? pEvents->iRecvFiberIndex : iEventsNumber - 1;
        i = index - iStartIndex;
    }

    //跨段不存在，同时注入光纤和接收光纤都为0，则接收光纤长度超出光纤长度
    if(pEvents->iIsThereASpan == 0)
    {
        iEndIndex = iStartIndex;
    }

    if (index > iStartIndex && index < iEndIndex)
    {
        sprintf(pEvtTab->sEvtTab[index].pNum, "%d", i);
    }
    else
    {
        strcpy(pEvtTab->sEvtTab[index].pNum, "*");
        //在注入光纤前和接收光纤后的事件显示灰色
        if (index != iStartIndex || (index == iStartIndex && iStartIndex == iEndIndex))
        {
            if(index != iEndIndex)
            {
                pEvtTab->sEvtTab[index].iIsGray = 1;
            }
            
            if(pEvtTab->iDisplaySectionEvt)
            {
                pEvtTab->sEvtTab[index + 1].iIsGray = 1;
            }
        }
    }
}
//设置事件编号
static void SetEventNum(EVENTTAB* pEvtTab)
{
    int i;
    for (i = 0; i < pEvents->iEventsNumber; ++i)
    {
        if (pEvtTab->iDisplaySectionEvt)
        {
            if (i != pEvents->iEventsNumber - 1)
            {
                memset(pEvtTab->sEvtTab[2*i + 1].pNum, 0, sizeof(pEvtTab->sEvtTab[2*i + 1].pNum));
                strcpy(pEvtTab->sEvtTab[2*i + 1].pNum, " ");
                pEvtTab->sEvtTab[2*i + 1].iIsGray = 0;
            }
            pEvtTab->sEvtTab[2*i].iIsGray = 0;
            ResetEventNumValue(pEvtTab, 2*i);
        }
        else
        {
            pEvtTab->sEvtTab[i].iIsGray = 0;
            ResetEventNumValue(pEvtTab, i);
        }
    }
}
//转换时间位置单位
static void SwitchEventPostionUnit(EVENTTAB* pEvtTab, int index, float fDistance)
{
    char *buff = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, 
        fDistance, 0);
    memset(pEvtTab->sEvtTab[index].pDist, 0, sizeof(pEvtTab->sEvtTab[index].pDist));
    strcpy(pEvtTab->sEvtTab[index].pDist, buff);
    free(buff);
    buff = NULL;
}
//设置事件位置
static void SetEventPosition(EVENTTAB* pEvtTab)
{
    int iEventsStart = pEvents->iEventsStart;
    int iLaunchFiberIndex = pEvents->iLaunchFiberIndex;
    int iStartIndex = (iLaunchFiberIndex > 0) ? iLaunchFiberIndex : iEventsStart;
    float fStartDistance = pEvents->EventsInfo[iStartIndex].fEventsPosition;
    int i;
    for (i = 0; i < pEvents->iEventsNumber; ++i)
	{
        EVENTS_INFO* pEvtsInfo = &(pEvents->EventsInfo[i]);
        //事件的距离和累积损耗以起始点为准(如果存在起始点)
        float fDistance = pEvtsInfo->fEventsPosition;
        if (iStartIndex != 0)
	    {
		    fDistance -= fStartDistance;
        }
        
        if (pEvtTab->iDisplaySectionEvt)
        {
            if (i != pEvents->iEventsNumber-1)
            {
                float fDist = pEvents->EventsInfo[i+1].fEventsPosition - pEvents->EventsInfo[i].fEventsPosition;
                pEvtTab->sEvtTab[2*i + 1].fDist = fDist;
                SwitchEventPostionUnit(pEvtTab, 2*i + 1, fDist);
            }
            pEvtTab->sEvtTab[2*i].fDist = fDistance;
            SwitchEventPostionUnit(pEvtTab, 2*i, fDistance);
        }
        else
        {
            pEvtTab->sEvtTab[i].fDist = fDistance;
            SwitchEventPostionUnit(pEvtTab, i, fDistance);
        }
    }
}
//设置事件损耗
static void SetEventLoss(EVENTTAB* pEvtTab)
{
    int i;
    char *buff = NULL;
    for (i = 0; i < pEvents->iEventsNumber; ++i)
	{
        EVENTS_INFO* pEvtsInfo = &(pEvents->EventsInfo[i]);

        if (pEvtTab->iDisplaySectionEvt)
        {
            if (i != (pEvents->iEventsNumber - 1))
            {
                //为保证从SOR读取的曲线精度丢失，衰减值显示时重新计算。
                double fAttenuation = Float2Float(PRECISION_3, pEvtsInfo->fAttenuation);
                double SectionLen = pEvents->EventsInfo[i+1].fEventsPosition
                     - pEvtsInfo->fEventsPosition;
                double fSectionEvtLoss = fAttenuation * (SectionLen / 1000);
                buff = Float2String(LOSS_PRECISION, fSectionEvtLoss);
                memset(pEvtTab->sEvtTab[2*i+1].pLoss, 0, sizeof(pEvtTab->sEvtTab[2*i+1].pLoss));
                strcpy(pEvtTab->sEvtTab[2*i+1].pLoss, buff);
                free(buff);
            }
            //第一个和最后一个事件不显示损耗
            if(i == 0 || i == (pEvents->iEventsNumber - 1))
            {
                memset(pEvtTab->sEvtTab[2*i].pLoss, 0, sizeof(pEvtTab->sEvtTab[2*i].pLoss));
                continue;
            }
            buff = Float2String(LOSS_PRECISION, pEvtsInfo->fLoss);
            memset(pEvtTab->sEvtTab[2*i].pLoss, 0, sizeof(pEvtTab->sEvtTab[2*i].pLoss));
            strcpy(pEvtTab->sEvtTab[2*i].pLoss, buff);
            free(buff);
        }
        else
        {
            //第一个和最后一个事件不显示损耗
            if(i == 0 || i == (pEvents->iEventsNumber - 1))
            {
                memset(pEvtTab->sEvtTab[i].pLoss, 0, sizeof(pEvtTab->sEvtTab[i].pLoss));
                continue;
            }
            buff = Float2String(LOSS_PRECISION, pEvtsInfo->fLoss);
            memset(pEvtTab->sEvtTab[i].pLoss, 0, sizeof(pEvtTab->sEvtTab[i].pLoss));
            strcpy(pEvtTab->sEvtTab[i].pLoss, buff);
            free(buff);
        }
    }
    buff = NULL;
}
//设置事件的反射率
static void SetEventReflectivity(EVENTTAB* pEvtTab)
{
    int i;
    char *buff = NULL;
    for (i = 0; i < pEvents->iEventsNumber; ++i)
	{
        EVENTS_INFO* pEvtsInfo = &(pEvents->EventsInfo[i]);
        if (pEvtTab->iDisplaySectionEvt)
        {
            if (i != pEvents->iEventsNumber - 1)
            {
                memset(pEvtTab->sEvtTab[2*i+1].pRef, 0, sizeof(pEvtTab->sEvtTab[2*i+1].pRef));
                strcpy(pEvtTab->sEvtTab[2*i+1].pRef, " ");
            }
            //只有反射事件、回波事件和合并事件显示反射率
            if (pEvtsInfo->iStyle == 2 || pEvtsInfo->iStyle == 3 || pEvtsInfo->iStyle == 4
                || pEvtsInfo->iStyle == 12 || pEvtsInfo->iStyle == 13 || pEvtsInfo->iStyle == 14)
            {
                buff = Float2String(REFLECTANCE_PRECISION, pEvtsInfo->fReflect);
                memset(pEvtTab->sEvtTab[2*i].pRef, 0, sizeof(pEvtTab->sEvtTab[2*i].pRef));
                strcpy(pEvtTab->sEvtTab[2*i].pRef, buff);
                free(buff);
            }
            else
            {
                memset(pEvtTab->sEvtTab[2*i].pRef, 0, sizeof(pEvtTab->sEvtTab[2*i].pRef));
            }
        }
        else
        {
            //只有反射事件、回波事件和合并事件显示反射率
            if (pEvtsInfo->iStyle == 2 || pEvtsInfo->iStyle == 3 || pEvtsInfo->iStyle == 4
                || pEvtsInfo->iStyle == 12 || pEvtsInfo->iStyle == 13 || pEvtsInfo->iStyle == 14)
            {
                buff = Float2String(REFLECTANCE_PRECISION, pEvtsInfo->fReflect);
                memset(pEvtTab->sEvtTab[i].pRef, 0, sizeof(pEvtTab->sEvtTab[i].pRef));
                strcpy(pEvtTab->sEvtTab[i].pRef, buff);
                free(buff);
            }
            else
            {
                memset(pEvtTab->sEvtTab[i].pRef, 0, sizeof(pEvtTab->sEvtTab[i].pRef));
            }
        }
        buff = NULL;
    }
}

//转换事件衰减单位
static void SwitchEventAttenuationUnit(EVENTTAB* pEvtTab, EVENTS_INFO* pEvtsInfo, int index)
{
    char *buff = Float2String(ATTENUATION_PRECISION, pEvtsInfo->fAttenuation);
    memset(pEvtTab->sEvtTab[index].pDec, 0, sizeof(pEvtTab->sEvtTab[index].pDec));
    strcpy(pEvtTab->sEvtTab[index].pDec, buff);
    free(buff);
    buff = NULL;
}

//设置事件衰减
static void SetEventAttenuation(EVENTTAB* pEvtTab)
{
    int i;

    for (i = 0; i < pEvents->iEventsNumber; ++i)
    {
        EVENTS_INFO* pEvtsInfo = &(pEvents->EventsInfo[i]);
        //显示衰减
        if (i != pEvents->iEventsNumber - 1)
        {
            SwitchEventAttenuationUnit(pEvtTab, pEvtsInfo, i);
        }
    }
}

//设置事件累积损耗
static void SetEventSum(EVENTTAB* pEvtTab)
{
    int iEventsStart = pEvents->iEventsStart;
    int iLaunchFiberIndex = pEvents->iLaunchFiberIndex;
    int iRecvFiberIndex = pEvents->iRecvFiberIndex;
    if(pEvents->iIsThereASpan == 0)
    {
        iLaunchFiberIndex = iLaunchFiberIndex == 0 ? iRecvFiberIndex : iLaunchFiberIndex;
        iRecvFiberIndex = iRecvFiberIndex == 0 ? iLaunchFiberIndex : iRecvFiberIndex;
    }
    
    int iStartIndex = (iLaunchFiberIndex > 0) ? iLaunchFiberIndex : iEventsStart;
    float fStartTotalLoss = pEvents->EventsInfo[iStartIndex].fTotalLoss;
    int i;
    char *buff = NULL;
    
    for (i = 0; i < pEvents->iEventsNumber; ++i)
    {
        EVENTS_INFO* pEvtsInfo = &(pEvents->EventsInfo[i]);
        double fTLoss = pEvtsInfo->fTotalLoss;
        double fSectionEvtTLoss = pEvents->EventsInfo[i + 1].fTotalLoss;
        if (iStartIndex != 0)
		{
			fTLoss = pEvtsInfo->fTotalLoss - fStartTotalLoss;
			fSectionEvtTLoss = pEvents->EventsInfo[i + 1].fTotalLoss - fStartTotalLoss;
		}
		
        if (pEvtTab->iDisplaySectionEvt)
        {
            //起始点前的事件不显示累积损耗
            if(i < iStartIndex || (iRecvFiberIndex > 0 && i >= iRecvFiberIndex)
                || (pEvents->iIsThereASpan == 0 && iLaunchFiberIndex == 0 && iRecvFiberIndex == 0))
            {
                if (i != (pEvents->iEventsNumber - 1))
                {
                    memset(pEvtTab->sEvtTab[2*i+1].pSum, 0, sizeof(pEvtTab->sEvtTab[2*i+1].pSum));
                }
                
                if((iRecvFiberIndex > 0 
                    || (pEvents->iIsThereASpan == 0 && iLaunchFiberIndex == 0 && iRecvFiberIndex == 0)) 
                    && i == iRecvFiberIndex
                    )
                {
                    buff = Float2String(LOSS_PRECISION, fTLoss + Float2Float(PRECISION_3, pEvtsInfo->fLoss));
                    memset(pEvtTab->sEvtTab[2*i].pSum, 0, sizeof(pEvtTab->sEvtTab[2*i].pSum));
                    strcpy(pEvtTab->sEvtTab[2*i].pSum, buff);
                    free(buff);
                }
                else
                {
                    memset(pEvtTab->sEvtTab[2*i].pSum, 0, sizeof(pEvtTab->sEvtTab[2*i].pSum));
                }
            }
            else
            {
                if (i != (pEvents->iEventsNumber - 1))
                {
                    buff = Float2String(LOSS_PRECISION, fSectionEvtTLoss);
                    memset(pEvtTab->sEvtTab[2*i+1].pSum, 0, sizeof(pEvtTab->sEvtTab[2*i+1].pSum));
                    strcpy(pEvtTab->sEvtTab[2*i+1].pSum, buff);
                    free(buff);
                    //第一个事件累计损耗显示0.000
                    if(i == 0)
                    {
                        buff = Float2String(LOSS_PRECISION, 0.0f);
                    }
                    else
                    {
                        buff = Float2String(LOSS_PRECISION, fTLoss + Float2Float(PRECISION_3, pEvtsInfo->fLoss));
                    }
                    memset(pEvtTab->sEvtTab[2*i].pSum, 0, sizeof(pEvtTab->sEvtTab[2*i].pSum));
                    strcpy(pEvtTab->sEvtTab[2*i].pSum, buff);
                    free(buff);
                }
				else
				{
                    buff = Float2String(LOSS_PRECISION, fTLoss);
                    memset(pEvtTab->sEvtTab[2*i].pSum, 0, sizeof(pEvtTab->sEvtTab[2*i].pSum));
					strcpy(pEvtTab->sEvtTab[2*i].pSum, buff);
                    free(buff);
				}
            }
        }
        else
        {
            if(i < iStartIndex || (iRecvFiberIndex > 0 && i > iRecvFiberIndex)
                || (pEvents->iIsThereASpan == 0 && iLaunchFiberIndex == 0 && iRecvFiberIndex == 0 && i != 0))
            {
                memset(pEvtTab->sEvtTab[i].pSum, 0, sizeof(pEvtTab->sEvtTab[i].pSum));
            }
            else if (i < pEvents->iEventsNumber - 1)
            {
                //第一个事件累计损耗显示0.000
                if(i == 0)
                {
                    buff = Float2String(LOSS_PRECISION, 0.0f);
                }
                else
                {
                    buff = Float2String(LOSS_PRECISION, fTLoss + Float2Float(PRECISION_3, pEvtsInfo->fLoss));
                }
                memset(pEvtTab->sEvtTab[i].pSum, 0, sizeof(pEvtTab->sEvtTab[i].pSum));
                strcpy(pEvtTab->sEvtTab[i].pSum, buff);
                free(buff);
            }
			else
			{
                buff = Float2String(LOSS_PRECISION, fTLoss);
                memset(pEvtTab->sEvtTab[i].pSum, 0, sizeof(pEvtTab->sEvtTab[i].pSum));
                strcpy(pEvtTab->sEvtTab[i].pSum, buff);
                free(buff);
			}
        }
    }
}
//设置事件通过
static void SetEventIsPass(EVENTTAB* pEvtTab)
{
    int i;
    for (i = 0; i < pEvents->iEventsNumber; ++i)
    {
        EVENTS_ISPASS* pEvtsIsPass = &(pEvents->EventsIsPass[i]);
        if (pEvtTab->iDisplaySectionEvt)
        {
            if (i != pEvents->iEventsNumber - 1)
            {
                pEvtTab->sEvtTab[2*i+1].iIsAttenPass = pEvtsIsPass->iIsAttenuationPass;
            }
            pEvtTab->sEvtTab[2*i].iIsLossPass = pEvtsIsPass->iIsEventLossPass;
            pEvtTab->sEvtTab[2*i].iIsReflectPass = pEvtsIsPass->iIsReflectPass;
        }
        else
        {
            pEvtTab->sEvtTab[i].iIsLossPass = pEvtsIsPass->iIsEventLossPass;
    		pEvtTab->sEvtTab[i].iIsReflectPass = pEvtsIsPass->iIsReflectPass;
    		pEvtTab->sEvtTab[i].iIsAttenPass = 1;       //衰减只显示在区段事件上
        }
    }
}

//设置事件的起始index以及末端index
static void SetEventStartAndEndIndex(EVENTTAB* pEvtTab, int* pStartIndex, int* pEndIndex)
{
    int iEventsNumber = 0;
    
    if (pEvtTab->iDisplaySectionEvt)
    {
        if (pEvents->iEventsNumber != 0)
        {
            iEventsNumber = 2* (pEvents->iEventsNumber) - 1;
            *pStartIndex = (pEvents->iLaunchFiberIndex > 0) ? 2*pEvents->iLaunchFiberIndex : 2*pEvents->iEventsStart;
            *pEndIndex = (pEvents->iRecvFiberIndex > 0) ? (2*pEvents->iRecvFiberIndex): (iEventsNumber - 1);
        }
    }
    else
    {
        iEventsNumber = pEvents->iEventsNumber;
        *pStartIndex = (pEvents->iLaunchFiberIndex > 0) ? pEvents->iLaunchFiberIndex : pEvents->iEventsStart;
        *pEndIndex = (pEvents->iRecvFiberIndex > 0) ? pEvents->iRecvFiberIndex : (iEventsNumber - 1);
    }

    if(pEvents->iIsThereASpan == 0)
    {
        *pEndIndex = *pStartIndex;
    }
}

//设置起始事件类型
static void SetStartEventType(EVENTTAB* pEvtTab, int iStartIndex,  int iEndIndex)
{
    pEvtTab->sEvtTab[0].iType = 7;
    if (pEvents->iEventsStart > 0)
    {
        pEvtTab->sEvtTab[iStartIndex].pNum[0] = 'R';
        pEvtTab->sEvtTab[iStartIndex].pNum[1] = '\0';
        pEvtTab->sEvtTab[0].pLoss[0] = 0;
    }
    else
    {
        if(iStartIndex == iEndIndex)
        {
            pEvtTab->sEvtTab[iStartIndex].pNum[0] = 'S';
            pEvtTab->sEvtTab[iStartIndex].pNum[1] = '/';
            pEvtTab->sEvtTab[iStartIndex].pNum[2] = 'E';
            pEvtTab->sEvtTab[iStartIndex].pNum[3] = '\0';
        }
        else
        {
            pEvtTab->sEvtTab[iStartIndex].pNum[0] = 'S';
            pEvtTab->sEvtTab[iStartIndex].pNum[1] = '\0';
        }

    }
}

//设置末端事件的事件类型
static void SetEndEventType(EVENTTAB* pEvtTab, int iStartIndex, int iEndIndex, int iEventsNumber)
{
    if (iEndIndex > 0)
    {
        if(iStartIndex == iEndIndex)
        {
            
        }
        else
        {
            pEvtTab->sEvtTab[iEndIndex].pNum[0] = 'E';
            pEvtTab->sEvtTab[iEndIndex].pNum[1] = '\0';
        }

        pEvtTab->sEvtTab[iEventsNumber - 1].pDec[0] = 0;
        pEvtTab->sEvtTab[iEventsNumber - 1].pLoss[0] = 0;

        //如果末端事件是连续光纤事件、者光功率不足事件、末端损耗事件 只显示累积损耗
        int type = pEvents->EventsInfo[iEventsNumber - 1].iStyle;
        if (type == 10 || type == 11 || type == 20 || type == 21)
        {
            pEvtTab->sEvtTab[iEventsNumber - 1].pRef[0] = 0;
        }
    }
}

//填充事件表格
static void FillEventTable(EVENTTAB* pEvtTab)
{
    if (pEvents->iEventsNumber <= 0)
        return;
    
	//获得波形参数
	CURVE_PRARM Param;
	GetCurvePara(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, &Param);
	//获得最新的事件列表
	GetCurveEvents(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, pEvents);
    
	//转换事件类型
	SetEventType(pEvtTab);
    //设置事件编号
    SetEventNum(pEvtTab);
    //设置事件位置
    SetEventPosition(pEvtTab);
    //设置事件损耗
    SetEventLoss(pEvtTab);
    //设置事件反射率
    SetEventReflectivity(pEvtTab);
    //设置事件衰减率
    SetEventAttenuation(pEvtTab);
    //设置事件累积损耗
    SetEventSum(pEvtTab);
    //设置事件是否通过
    SetEventIsPass(pEvtTab);

    //参考起始点的距离
    int iStartIndex = 0;
    int iEndIndex = 0;
    SetEventStartAndEndIndex(pEvtTab, &iStartIndex, &iEndIndex);
    
    //设置起始事件类型
    SetStartEventType(pEvtTab, iStartIndex, iEndIndex);

    //如果设置了接收光纤，则设置末端事件类型
    int iEventsNumber = pEvtTab->iEvtTotallEvt;
    SetEndEventType(pEvtTab, iStartIndex, iEndIndex, iEventsNumber);
}

/******************************************************************************/				  
//获得事件表要显示的内容字符串
static int GetEvtContent(EVENTTAB *pEvtTab)
{

	//判断是否需要转换
	if (!pEvtTab->iNeedGetEvtContent)
	{
		return 0;
	}

    FillEventTable(pEvtTab);

	//标明事件转换完毕
	pEvtTab->iNeedGetEvtContent = 0;
	
	return 0;
}


//计算当前事件总数一共占事件表的多少页
static int CalcEvtPageTatall(EVENTTAB *pEvtsTab)
{	
	int iTmp = pEvtsTab->iEvtTotallEvt + pEvtsTab->iColNum - 1;

	if (pEvtsTab->iEvtTotallEvt > pEvtsTab->iColNum)
	{
		return	iTmp / (pEvtsTab->iColNum);
	}
	else
	{
		return 1;
	}
}

//向下翻页事件表
static int EvtTabPgDn(EVENTTAB *pEvtTab, int iFocs)
{
	//判断使能是否有效
	if ((0 == pEvtTab->iEnable) ||	
		(0 == pEvtTab->iTouchEnable))
	{
		return 0;
	}
	
	//判断当前的页号是不是小于总数-1，等于总数-1的时候表示为最后一页
	if (pEvtTab->iEvtTabPage < pEvtTab->iEvtPageTotall - 1)
	{
		pEvtTab->iEvtTabPage++;
		pEvtTab->iEvtFocus = iFocs;
		pEvtTab->iEvtDispIdx = pEvtTab->iEvtTabPage * pEvtTab->iColNum + iFocs;
	}
	
	return 0;
}

//向上翻页事件表
static int EvtTabPgUp(EVENTTAB *pEvtTab, int iFocs)
{
	//判断使能是否有效
	if ((0 == pEvtTab->iEnable) ||	
		(0 == pEvtTab->iTouchEnable))
	{
		return 0;
	}

	//翻页
	if (pEvtTab->iEvtTabPage > 0)
	{
		pEvtTab->iEvtTabPage--;
		pEvtTab->iEvtFocus = iFocs;
		pEvtTab->iEvtDispIdx = pEvtTab->iEvtTabPage * pEvtTab->iColNum + iFocs;
	}
	
	return 0;
}

/******************************************************************************/				  
//设置光标在事件上
int SetCursorAtEvent(EVENTTAB *pEvtTab, int isTouchMove, int iMaxFlg)
{
	int iPos = 0;

    if (pEvtTab->iDisplaySectionEvt == 0)
    {
        iPos = pEvtTab->iEvtDispIdx;
        SendEventMakerCtlMsg(EN_MOVE_SETVAL, pEvents->EventsInfo[iPos].iBegin, -1, iMaxFlg, isTouchMove);
    }
    else
    {
        if (pEvtTab->iEvtDispIdx % 2 == 1)
        {
            iPos = pEvtTab->iEvtDispIdx / 2;
            SendEventMakerCtlMsg(EN_MOVE_SETVAL, pEvents->EventsInfo[iPos].iBegin,
                pEvents->EventsInfo[iPos+1].iBegin, iMaxFlg, isTouchMove);
        }
        else
        {
            iPos = pEvtTab->iEvtDispIdx / 2;
            SendEventMakerCtlMsg(EN_MOVE_SETVAL, pEvents->EventsInfo[iPos].iBegin, -1, iMaxFlg, isTouchMove);
        }
    }
    
    return 0;
}


//判断点击是否在事件上
int TouchIsOnEvent(int iTouchX, EVENTTAB *pEvtTab)
{
	int iIndex = -1;

	//判断有木有点击在当前事件上
	iIndex = TouchOnEvent(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, iTouchX);
    
	if (-1 == iIndex)
	{
		return 0;
	}
    
	//计算新的页码，事件焦点位置等 
	pEvtTab->iEvtDispIdx = (pEvtTab->iDisplaySectionEvt == 0) ? iIndex : iIndex*2;// - pEvents->iEventsStart;
	pEvtTab->iEvtTabPage = pEvtTab->iEvtDispIdx / pEvtTab->iColNum;
	pEvtTab->iEvtFocus = pEvtTab->iEvtDispIdx - 
						pEvtTab->iEvtTabPage * pEvtTab->iColNum;

	return 1;
}

//增加一个事件
int AddEventsInTable(int iNewLocation, EVENTTAB *pEvtTab)
{
	int iTmp;
	
	ALGORITHM_INFO *pAlgInfo = NULL;
	PTASK_SETTINGS pTskSet = NULL;
	
	//获取otdr事件信息
	iTmp = GetCurrCurvePos(pOtdrTopSettings->pDisplayInfo);
	pTskSet = &(pOtdrTopSettings->pTask_Context->Task_SetQueue[iTmp]);

	pAlgInfo = pTskSet->pAlgorithm_info;
	if ((NULL == pAlgInfo) ||
	    (!CurveIsVaild(CURR_CURVE, pOtdrTopSettings->pDisplayInfo)))
	{
		CODER_LOG(CoderYun, "Algorithm is NULL\n");
		return -1;
	}

    //调用app中的增加事件
    int index = AddEventsTable(pAlgInfo->pSignalLog, pAlgInfo->iSignalLength, 
				      pEvents, pAlgInfo->iBlind, iNewLocation, pAlgInfo->SetParameters.fSampleRating);
	if(index < 0)
	{
		CODER_LOG(CoderYun, "Add Event Failed\n");
		return -3;
	}
    

    //计算新添加的事件索引
    iTmp = 0;
	while ((pEvents->EventsInfo[iTmp].iBegin) != iNewLocation)
	{
		iTmp++;
	}

	//增加成功后重新计算参数 
    SetEventTabTotalNum(pEvtTab);
	pEvtTab->iEvtPageTotall = CalcEvtPageTatall(pEvtTab);
	pEvtTab->iEvtDispIdx = (pEvtTab->iDisplaySectionEvt == 0) ? iTmp : 2*iTmp;// - pEvents->iEventsStart;
	pEvtTab->iEvtTabPage = pEvtTab->iEvtDispIdx / pEvtTab->iColNum;
	pEvtTab->iEvtFocus = pEvtTab->iEvtDispIdx - 
								pEvtTab->iEvtTabPage * pEvtTab->iColNum;
	pEvtTab->iNeedGetEvtContent = 1;
	SetCurveEvents(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, pEvents);

	return 0;
}

//删除一个事件
int DelEventsInTable(EVENTTAB *pEvtTab)
{
	int iTmp, iEvtIndex;
	ALGORITHM_INFO *pAlgInfo = NULL;
	PTASK_SETTINGS pTskSet = NULL;
	
	//获取otdr事件信息
	iTmp = GetCurrCurvePos(pOtdrTopSettings->pDisplayInfo);
	pTskSet = &(pOtdrTopSettings->pTask_Context->Task_SetQueue[iTmp]);

	pAlgInfo = pTskSet->pAlgorithm_info;
	if ((NULL == pAlgInfo) ||
	    (!CurveIsVaild(CURR_CURVE, pOtdrTopSettings->pDisplayInfo)))
	{
		CODER_LOG(CoderYun, "Algorithm is NULL\n");
		return -1;
	}

	iEvtIndex = pEvtTab->iEvtDispIdx;
    //区段事件
    if (pEvtTab->iDisplaySectionEvt == 1)
    {
        if (iEvtIndex % 2 == 1)
            return -2;
        if(pEvents->iEventsNumber == 2)
            return -2;
        else
            iEvtIndex /= 2;
    }
    //从事件表中删除事件(算法中的函数) 
	if (DelEventsTable(pAlgInfo->pSignalLog, pAlgInfo->iSignalLength, 
					   pEvents, iEvtIndex, pTskSet->pAlgorithm_info->SetParameters.fSampleRating))
	{
		CODER_LOG(CoderYun, "Del Event Failed\n");
		return -3;
	}

    //判断删除的是否为最后一个事件如果是则焦点等需要改变 
	if((iEvtIndex == pEvents->iEventsNumber)
		&&(iEvtIndex > 0))
	{
		iEvtIndex--;
	}
	evt_dbg(pEvtTab);

	//删除成功后重新计算参数 
    SetEventTabTotalNum(pEvtTab);
	pEvtTab->iEvtPageTotall = CalcEvtPageTatall(pEvtTab);
	pEvtTab->iEvtDispIdx = (pEvtTab->iDisplaySectionEvt == 0) ? iEvtIndex: 2*iEvtIndex;// - pEvents->iEventsStart;
	pEvtTab->iEvtTabPage = pEvtTab->iEvtDispIdx / pEvtTab->iColNum;
	pEvtTab->iEvtFocus = pEvtTab->iEvtDispIdx - 
								pEvtTab->iEvtTabPage * pEvtTab->iColNum;

	evt_dbg(pEvtTab);

	pEvtTab->iNeedGetEvtContent = 1;

	SetCurveEvents(CURR_CURVE, pOtdrTopSettings->pDisplayInfo, pEvents);
	
	return 0;	
}

//设置参考起始点
int SetEventsRef(int iNewLocation, EVENTTAB *pEvtTab)
{
	//临时变量定义
	int iTmp;
	
	ALGORITHM_INFO *pAlgInfo = NULL;
	PTASK_SETTINGS pTskSet = NULL;
	
	//获取otdr事件信息
	iTmp = GetCurrCurvePos(pOtdrTopSettings->pDisplayInfo);
	pTskSet = &(pOtdrTopSettings->pTask_Context->Task_SetQueue[iTmp]);

	pAlgInfo = pTskSet->pAlgorithm_info;
	if (NULL == pAlgInfo)
	{
		CODER_LOG(CoderYun, "Algorithm is NULL\n");
		return -1;
	}
    //设置参考起始点
	if (SetStartRef(pAlgInfo->pSignalLog, pAlgInfo->iSignalLength, 
					pTskSet->iBlindLength, iNewLocation, pEvents, 
					pOtdrTopSettings->pDisplayInfo))
	{
		CODER_LOG(CoderYun, "Set Start Ref Error\n");
		return -2;
	}
	
	//重新计算参数 
    SetEventTabTotalNum(pEvtTab);
	pEvtTab->iEvtPageTotall = CalcEvtPageTatall(pEvtTab);
	pEvtTab->iEvtDispIdx = 0;
	pEvtTab->iEvtTabPage = pEvtTab->iEvtDispIdx / pEvtTab->iColNum;
	pEvtTab->iEvtFocus = pEvtTab->iEvtDispIdx - 
								pEvtTab->iEvtTabPage * pEvtTab->iColNum;
	pEvtTab->iNeedGetEvtContent = 1;
	return 0;
}

//清除参考起始点
int ClearEventsRef(EVENTTAB *pEvtTab)
{
	//临时变量定义
	int iTmp;
	
	ALGORITHM_INFO *pAlgInfo = NULL;
	PTASK_SETTINGS pTskSet = NULL;
	
	//获取otdr事件信息
	iTmp = GetCurrCurvePos(pOtdrTopSettings->pDisplayInfo);
	pTskSet = &(pOtdrTopSettings->pTask_Context->Task_SetQueue[iTmp]);

	pAlgInfo = pTskSet->pAlgorithm_info;
	if (NULL == pAlgInfo)
	{
		CODER_LOG(CoderYun, "Algorithm is NULL\n");
		return -1;
	}
	
    //清除参考起始点
	if (ClearStartRef(pAlgInfo->pSignalLog, pAlgInfo->iSignalLength, pEvents, 
					  pOtdrTopSettings->pDisplayInfo))
	{
		CODER_LOG(CoderYun, "Clear Start Ref Error\n");
		return -2;
	}

	//重新计算参数 
    SetEventTabTotalNum(pEvtTab);
	pEvtTab->iEvtPageTotall = CalcEvtPageTatall(pEvtTab);
	pEvtTab->iEvtDispIdx = 0;
	pEvtTab->iEvtTabPage = pEvtTab->iEvtDispIdx / pEvtTab->iColNum;
	pEvtTab->iEvtFocus = pEvtTab->iEvtDispIdx - 
								pEvtTab->iEvtTabPage * pEvtTab->iColNum;
	pEvtTab->iNeedGetEvtContent = 1;
	return 0;	
}


/******************************************************************************/				  
//选中条事件处理函数
static int EventSelectBar_Down(void *pInArg, int iInLen, 
                               void *pOutArg, int iOutLen)
{
	EVENTTAB *pEvtTab = (EVENTTAB *)pOutArg;
	int iEvtNum = pEvtTab->iEvtTabPage * pEvtTab->iColNum + iOutLen;

	if (pEvtTab->iEnable &&
		pEvtTab->iTouchEnable)
	{
		//判断当前选中的位置是否超限
		if ((iEvtNum < pEvtTab->iEvtTotallEvt))
		{
			//更新相关标志
			pEvtTab->iEvtFocus = iOutLen;
			pEvtTab->iEvtDispIdx = iEvtNum;

			FlushEvtTab(pEvtTab);
			// SetCursorAtEvent(pEvtTab, 0, 0);
		}
	}
    return 0;
}

//下一个事件
//static int EventNext_Down(void *pInArg, int iInLen, 
//                          void *pOutArg, int iOutLen)
//{
//	EVENTTAB *pEvtTab = (EVENTTAB *)pOutArg;
//	//判断使能是否有效
//	if ((0 == pEvtTab->iEnable) ||	
//		(0 == pEvtTab->iTouchEnable))
//	{
//		return 0;
//	}
//	//SetPictureBitmap(BmpFileDirectory"event_next_press.bmp", pEvtBtnNext);
//// 	DisplayPicture(pEvtBtnNext);
//	RefreshScreen(__FILE__, __func__, __LINE__);
//	
//	return 0;
//}

//下一个事件
static int EventNext_Up(void *pInArg, int iInLen, 
                        void *pOutArg, int iOutLen)
{
	EVENTTAB *pEvtTab = (EVENTTAB *)pOutArg;

	//判断使能是否有效
	if ((0 == pEvtTab->iEnable) ||	
		(0 == pEvtTab->iTouchEnable))
	{
		return 0;
	}
	//SetPictureBitmap(BmpFileDirectory"event_next_unpress.bmp", pEvtBtnNext);
// 	DisplayPicture(pEvtBtnNext);
	EvtTabPgDn(pEvtTab, 0);
	FlushEvtTab(pEvtTab);
	// SetCursorAtEvent(pEvtTab, 0, 0);
	
	return 0;
}

//上一个事件
//static int EventPrev_Down(void *pInArg, int iInLen, 
//                          void *pOutArg, int iOutLen)
//{
//	EVENTTAB *pEvtTab = (EVENTTAB *)pOutArg;
//
//	//判断使能是否有效
//	if ((0 == pEvtTab->iEnable) ||	
//		(0 == pEvtTab->iTouchEnable))
//	{
//		return 0;
//	}
//	
//	//SetPictureBitmap(BmpFileDirectory"event_prev_press.bmp", pEvtBtnPrev);
// //	DisplayPicture(pEvtBtnPrev);
//	RefreshScreen(__FILE__, __func__, __LINE__);
//
//	return 0;
//}

//上一个事件
static int EventPrev_Up(void *pInArg, int iInLen, 
                        void *pOutArg, int iOutLen)

{
	EVENTTAB *pEvtTab = (EVENTTAB *)pOutArg;

	//判断使能是否有效
	if ((0 == pEvtTab->iEnable) ||	
		(0 == pEvtTab->iTouchEnable))
	{
		return 0;
	}
	
	//SetPictureBitmap(BmpFileDirectory"event_prev_unpress.bmp", pEvtBtnPrev);
 //	DisplayPicture(pEvtBtnPrev);
	EvtTabPgUp(pEvtTab, pEvtTab->iColNum-1);
	FlushEvtTab(pEvtTab);
	// SetCursorAtEvent(pEvtTab, 0, 0);
	
	return 0;
}

//转换事件（事件表<->链路地图）
static int SwitchEvts_Down(void *pInArg)
{
	static int iEnable = 0;
	EVENTTAB *pEvtTab = (EVENTTAB *)pInArg;

	if (pOtdrTopSettings->pDisplayInfo->iCurveNum)
	{
		iEnable = iEnable ? 0 : 1;
		SetOtdrLinkMapEnable(pOtdrLinkMap, iEnable, NULL);
		FlushEvtTab(pEvtTab);
	}

	return 0;
}

//添加事件
static int AddEvts_Down(void *pInArg)
{
    if(pOtdrTopSettings->pDisplayInfo->iCurveNum)
    {
    	EVENTTAB *pEvtTab = (EVENTTAB *)pInArg;
    	int iEventMarker = GetEventMarker(CURR_CURVE, pOtdrTopSettings->pDisplayInfo);
    	AddEventsInTable(iEventMarker, pEvtTab);
    	//重新对各曲线事件列表做阈值判断
    	ReevaluateEvent();
    	FlushEvtTab(pEvtTab);
    	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
	}
	return 0;
}
//删除事件
static int DelEvts_Down(void *pInArg)
{	
    if(pOtdrTopSettings->pDisplayInfo->iCurveNum)
    {
    	EVENTTAB *pEvtTab = (EVENTTAB *)pInArg;

    	DelEventsInTable(pEvtTab);
    	//重新对各曲线事件列表做阈值判断
    	ReevaluateEvent();
    	FlushEvtTab(pEvtTab);
    	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
    }
	return 0;
}

//最大化
//static int MaxEvts_Down(void *pInArg)
//{	
//	PanitEvtBtn(pEvtBtn);
//    EVENTTAB *pEvtTab = (EVENTTAB *)pInArg;
//    SetCursorAtEvent(pEvtTab, 0, 1);
//	
//	return 0;
//}

//设置参考起始点
//static int SetStart_Down(void *pInArg)
//{
//    if(pOtdrTopSettings->pDisplayInfo->iCurveNum)
//    {
//    	EVENTTAB *pEvtTab = (EVENTTAB *)pInArg;
//        int iEventMarker = GetEventMarker(CURR_CURVE, pOtdrTopSettings->pDisplayInfo);
//    	SetEventsRef(iEventMarker, pEvtTab);
//    	//重新对各曲线事件列表做阈值判断
//    	ReevaluateEvent();
//    	FlushEvtTab(pEvtTab);
//    	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
//        //刷新波形
//        SendLineCtlMoveMsg(EN_CTL_NEWDATA, 0, 0);
//    }
//	return 0;
//}
////删除参考起始点
//static int DelStart_Down(void *pInArg)
//{
//    if(pOtdrTopSettings->pDisplayInfo->iCurveNum)
//    {
//    	EVENTTAB *pEvtTab = (EVENTTAB *)pInArg;
//
//    	ClearEventsRef(pEvtTab);
//    	//重新对各曲线事件列表做阈值判断
//    	ReevaluateEvent();
//    	FlushEvtTab(pEvtTab);
//    	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
//        //刷新波形
//        SendLineCtlMoveMsg(EN_CTL_NEWDATA, 0, 0);
//    }
//	return 0;
//}
////暂未使用
//static int NoUse_Down(void *pInArg)
//{
//	printf("Nouse\n\n");
//	return 0;
//}


/******************************************************************************/				  
//更新页码
static void UpdatePageNum(EVENTTAB *pEvtsTab)
{
	char strTmp[16] = {0};

	//页码
	sprintf(strTmp, "%d/%d", pEvtsTab->iEvtTabPage + 1, 
			pEvtsTab->iEvtPageTotall);
	//free(pEvtStrPage);	
	//pEvtStrPage =  TransString(strTmp);
	//SetLabelText(pEvtStrPage, pEvtLblPage);

// 	DisplayLabel(pEvtLblPage);
}

//释放事件表的文本
static void FreeEvtTabStr(int iLblIdx)
{
	GuiMemFree(pEvtStrNumbD[iLblIdx]);
	GuiMemFree(pEvtStrDistD[iLblIdx]);
	GuiMemFree(pEvtStrSLosD[iLblIdx]);
	GuiMemFree(pEvtStrRefrD[iLblIdx]);
	GuiMemFree(pEvtStrDecaD[iLblIdx]);
}

//转换事件信息到宽字符以便于显示
static void TransEvtInfo2Str(EVENTTAB *pEvtsTab, int iLblIdx, int iEvtIdx)
{
	//获得事件信息字符串
	EVTTABINFO *pEvtInfo = pEvtsTab->sEvtTab;

	//转换字符串
	pEvtPicTypeUnFocus[iLblIdx] = ppEventTypePicUnSlt[pEvtInfo[iEvtIdx].iType];
	pEvtPicTypeFocus[iLblIdx] = ppEventTypePicSlt[pEvtInfo[iEvtIdx].iType];
	pEvtStrNumbD[iLblIdx] = TransString(pEvtInfo[iEvtIdx].pNum);
	pEvtStrDistD[iLblIdx] = TransString(pEvtInfo[iEvtIdx].pDist);
	pEvtStrSLosD[iLblIdx] = TransString(pEvtInfo[iEvtIdx].pLoss);
	pEvtStrRefrD[iLblIdx] = TransString(pEvtInfo[iEvtIdx].pRef);
	pEvtStrDecaD[iLblIdx] = TransString(pEvtInfo[iEvtIdx].pDec);
}
  
//更新事件表标签
static void UpdateEvtTabLbl(EVENTTAB *pEvtsTab, int iLblIdx, int isGray)
{
	char *strEvtType = NULL;
	//重设事件类型图片
	strEvtType = (iLblIdx == pEvtsTab->iEvtFocus) ?
					   pEvtPicTypeFocus[iLblIdx] : pEvtPicTypeUnFocus[iLblIdx];
    
	SetPictureBitmap(strEvtType, pEvtPicTypeD[iLblIdx]);
	
	//重设lbl的文本 
	SetLabelText(pEvtStrNumbD[iLblIdx], pEvtLblNumbD[iLblIdx]);
	SetLabelText(pEvtStrDistD[iLblIdx], pEvtLblDistD[iLblIdx]);
	SetLabelText(pEvtStrSLosD[iLblIdx], pEvtLblSLosD[iLblIdx]);
	SetLabelText(pEvtStrRefrD[iLblIdx], pEvtLblRefrD[iLblIdx]);
	SetLabelText(pEvtStrDecaD[iLblIdx], pEvtLblDecaD[iLblIdx]);
	
    GUIFONT *font = getGlobalFnt(EN_FONT_BLACK);

    if (isGray)
    {
        font = getGlobalFnt(EN_FONT_GRAY);
    }
    
    SetLabelFont(font, pEvtLblNumbD[iLblIdx]);
    SetLabelFont(font, pEvtLblDistD[iLblIdx]);
    SetLabelFont(font, pEvtLblSLosD[iLblIdx]);
    SetLabelFont(font, pEvtLblRefrD[iLblIdx]);
    SetLabelFont(font, pEvtLblDecaD[iLblIdx]);
}

//刷新事件表整页
static void UpdateEvtTabPage(EVENTTAB *pEvtTab, int iLblIdx, int iEvtIdx)
{
	FreeEvtTabStr(iLblIdx);
	TransEvtInfo2Str(pEvtTab, iLblIdx, iEvtIdx);
	UpdateEvtTabLbl(pEvtTab, iLblIdx, pEvtTab->sEvtTab[iEvtIdx].iIsGray);
}

//刷新事件显示表格
static void UpdateEventDisp(EVENTTAB *pEvtsTab)
{		
	int iTmp, iEvtDispNum, iEvtTabIdx;	
	
	iEvtTabIdx = pEvtsTab->iEvtTabPage * pEvtsTab->iColNum;

	//如果事件总数为零则不需要计算
	if (0 != pEvtsTab->iEvtTotallEvt)
	{	
		//计算当前页显示几个事件 
		iEvtDispNum = (pEvtsTab->iEvtTabPage == pEvtsTab->iEvtPageTotall - 1) ? 
				(pEvtsTab->iEvtTotallEvt % pEvtsTab->iColNum) : pEvtsTab->iColNum;
		if (0 == iEvtDispNum)
		{
			iEvtDispNum = pEvtsTab->iColNum;
		}	

		//更新Label
		for (iTmp = 0; iTmp < iEvtDispNum; ++iTmp, ++iEvtTabIdx)
		{
			UpdateEvtTabPage(pEvtsTab, iTmp, iEvtTabIdx);
		}
	}
	//更新页码
	UpdatePageNum(pEvtsTab);

	evt_dbg(pEvtsTab);
}

static void EvtsTabDrawBg(int row, int column, int color, EVENTTAB *pEvtTab)
{
    //临时变量定义
    GUIPEN *pPen;
	unsigned int uiColorBack;
	if(!pEvtTab)
	{
        return;
	}
    unsigned int StartX = 0;
    unsigned int StartY = 0;
    unsigned int EndX = 0;
    unsigned int EndY = 0;

	StartX = 21+(column-1 - 1) * 120;
    StartY = 450 - (pEvtTab->iColNum - row) * 30;
    EndX = StartX + 119;
    EndY = StartY + 28;

	pPen = GetCurrPen();
	uiColorBack = pPen->uiPenColor;
	pPen->uiPenColor = color;
	DrawBlock(StartX, StartY, EndX, EndY);
	pPen->uiPenColor = uiColorBack;		
}

//设置事件点光标位置
void SetEventFocus(EVENTTAB *pEvtTab, int option)
{
	int iFocus = pEvtTab->iEvtFocus;
	switch (option)
	{
	case 0://up
		if (!pEvtTab->iEvtTabPage)
		{
			if (iFocus)
			{
				pEvtTab->iEvtFocus = iFocus-1;
				pEvtTab->iEvtDispIdx = pEvtTab->iEvtTabPage * pEvtTab->iColNum + pEvtTab->iEvtFocus;
				FlushEvtTab(pEvtTab);
				// SetCursorAtEvent(pEvtTab, 0, 0);
			}
		}
		else
		{
			if (iFocus)
			{
				pEvtTab->iEvtFocus = iFocus - 1;
				pEvtTab->iEvtDispIdx = pEvtTab->iEvtTabPage * pEvtTab->iColNum + pEvtTab->iEvtFocus;
				FlushEvtTab(pEvtTab);
				// SetCursorAtEvent(pEvtTab, 0, 0);
			}
			else
			{
				EventPrev_Up(NULL, 0, pEvtTab, 0);
			}
		}
		break;
	case 1://down
		if (pEvtTab->iEvtFocus == pEvtTab->iColNum-1)
		{
			if (pEvtTab->iEvtTabPage != pEvtTab->iEvtPageTotall-1)
			{
				EventNext_Up(NULL, 0, pEvtTab, 0);
			}
		}
		else
		{
			int iEvtDispNum = (pEvtTab->iEvtTabPage == pEvtTab->iEvtPageTotall - 1) ?
				(pEvtTab->iEvtTotallEvt % pEvtTab->iColNum) : pEvtTab->iColNum;

			if (pEvtTab->iEvtFocus != iEvtDispNum-1)
			{
				pEvtTab->iEvtFocus = iFocus + 1;
				pEvtTab->iEvtDispIdx = pEvtTab->iEvtTabPage * pEvtTab->iColNum + pEvtTab->iEvtFocus;
				FlushEvtTab(pEvtTab);
				// SetCursorAtEvent(pEvtTab, 0, 0);
			}
		}
		break;
	default:
		break;
	}
}

//切换链路地图与事件表
void SwitchLinkMapOrEventTab(EVENTTAB *pEvtTab)
{
	SwitchEvts_Down(pEvtTab);
	RefreshScreen(__FILE__, __func__, __LINE__);
}
