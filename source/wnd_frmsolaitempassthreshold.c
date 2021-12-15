/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolaitempassthreshold..c
* 摘    要：  实现初始化窗体frmsolasetting的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wbq
* 完成日期：  2016-12-30
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/


#include "wnd_frmsolalinedefine.h"
#include "wnd_frmsolamessager.h"
#include "wnd_frmsolasetting.h"
#include "wnd_frmsola.h"
#include "wnd_frmsolaidentify.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmselector.h"
#include "wnd_frmsolaconfig.h"
#include "wnd_frmime.h"
#include "wnd_frmsolaitempassthreshold.h"
#include "wnd_frmsolalinepassthreshold.h"
#include "app_getsetparameter.h"
#include "app_frminit.h"
#include "guiphoto.h"
#include "app_systemsettings.h"
#include "wnd_frmsolaident.h"
#include "app_unitconverter.h"

extern SOLA_MESSAGER* pSolaMessager;

static GUIPICTURE* selectAllPicture = NULL;
static GUIPICTURE* selectNonePicture = NULL;
static GUIPICTURE* resetPicture =NULL;

static GUILABEL *selectAllLabel = NULL;
static GUILABEL *selectNoneLabel = NULL;
static GUILABEL *resetLabel = NULL;

static GUICHAR* selectAllStr = NULL;
static GUICHAR* selectNoneStr = NULL;
static GUICHAR* resetStr = NULL;

static GUIWINDOW *pFrmSolaSetting = NULL;
static GUIPICTURE * identifyTitlePicture =NULL;
static GUIPICTURE * lineDefineTitlePicture =NULL;
static GUIPICTURE * linePassThresholdTitlePicture =NULL;
static GUIPICTURE * itemPassThresholdTitlePicture =NULL;

static GUICHAR* pSolaSettingStrIdentifyTitle = NULL;
static GUILABEL* pSolaSettingLblIdentifyTitle = NULL;

static GUICHAR* pSolaSettingStrLineDefineTitle = NULL;
static GUICHAR* pSolaSettingStrLinePassThresholdTitle = NULL;
static GUICHAR* pSolaSettingStrItemPassThresholdTitle = NULL;

static GUILABEL* pSolaSettingLblLineDefineTitle = NULL;
static GUILABEL* pSolaSettingLblLinePassThresholdTitle = NULL;
static GUILABEL* pSolaSettingLblItemPassThresholdTitle = NULL;


static GUIPICTURE *pSolaSettingBgDesk = NULL;
static WNDMENU1* pSolaSettingMenu = NULL;

//重构界面
#define MAX_LOSS_NUM            11

static GUICHAR* pItemPassThresoldStrTitle = NULL;//通过/不通过阈值
static GUILABEL* pItemPassThresoldLblTitle = NULL;//

static GUICHAR* pItemPassThresoldStrWaveLength = NULL;//波长
static GUILABEL* pItemPassThresoldLblWaveLength = NULL;

static GUIPICTURE* pItemPassThresoldBtnWaveLength[WAVE_NUM] = {NULL};
static GUICHAR* pItemPassThresoldStrWave[WAVE_NUM] = {NULL};
static GUILABEL* pItemPassThresoldLblWave[WAVE_NUM] = {NULL};

static GUIPICTURE* pItemPassThresoldBtnAllWaveLength = NULL;
static GUICHAR* pItemPassThresoldStrAllWaveLength = NULL;
static GUILABEL* pItemPassThresoldLblAllWaveLength = NULL;

static GUIPICTURE* pItemPassThresoldBtnLoss[MAX_LOSS_NUM] = {NULL};
static GUICHAR* pItemPassThresoldStrLoss[MAX_LOSS_NUM] = {NULL};
static GUILABEL* pItemPassThresoldLblLoss[MAX_LOSS_NUM] = {NULL};

static GUIPICTURE* pItemPassThresoldInputBtnLoss[MAX_LOSS_NUM] = {NULL};
static GUICHAR* pItemPassThresoldStrInputLoss[MAX_LOSS_NUM] = {NULL};
static GUILABEL* pItemPassThresoldLblInputLoss[MAX_LOSS_NUM] = {NULL};

static GUICHAR* pItemPassThresoldStrLossUnit[MAX_LOSS_NUM] = {NULL};
static GUILABEL* pItemPassThresoldLblLossUnit[MAX_LOSS_NUM] = {NULL};

struct range
{
    float min;
    float max;
};
static struct range rangeValues[11] = {{-5.000f,5.000f},{-5.000f,5.000f},{-80.0f,0.0f},
                                  {2.500f,26.000f},{6.000f,26.000f},{9.000f,26.000f},
                                  {12.500f,26.000f},{15.500f,26.000f},{19.000f,26.000f},
                                  {22.500f,26.000f},{-80.0f,0.0f}};
static int loss_index = 0;

/*********************************
*声明wnd_frmcommonset.c所引用的外部变量
*********************************/

/********************************
* 窗体frmsolaIdentify中的文本资源处理函数
********************************/
//初始化文本资源
static int SolaSettingTextRes_Init(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen);
//释放文本资源
static int SolaSettingTextRes_Exit(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen);

/********************************
* 窗体frmsola中的错误事件处理函数
********************************/
static int SolaSettingErrProc_Func(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen);

static int SolaLineDefineTabIdentify_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaLineDefineTabIdentify_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int SolaIdentifyTabLineDefine_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaIdentifyTabLineDefine_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int SolaIdentifyTabLinePassThreshold_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaIdentifyTabLinePassThreshold_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int ItemPassThresoldBtnWaveLength_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ItemPassThresoldBtnWaveLength_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ItemPassThresoldBtnAllWaveLength_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ItemPassThresoldBtnAllWaveLength_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ItemPassThresoldBtnLoss_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ItemPassThresoldBtnLoss_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ItemPassThresoldBtnInputLoss_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int ItemPassThresoldBtnInputLoss_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);

static void SolaSettingMenuCall(int buttonIndex);
GUIWINDOW* CreateSolaLineDefineWindow();
static void Refresh(void);
static WAVELEN getCurrentWaveLength();
static void ReCreateWindow(GUIWINDOW **pWnd);
//右侧菜单文本
static unsigned int strSolaSettingMenu[] = {	
	
};

static void setThresoldSelectState(WAVELEN iCurrWaveLength, int iSelectedFlag)
{
    SOLA_ITEM_PASSTHRESHOLD* pSolaItemPassSet = &pSolaMessager->itemPassThreshold;
    pSolaItemPassSet->maxSpliceLoss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxLinkerLoss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxLinkerReflectance.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitter2Loss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitter4Loss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitter8Loss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitter16Loss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitter32Loss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitter64Loss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitter128Loss.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
    pSolaItemPassSet->maxSplitterReflectance.iSelectedFlag[iCurrWaveLength] = iSelectedFlag;
}

static void setAllWaveLengthValue(void)
{
    SOLA_ITEM_PASSTHRESHOLD* pSolaItemPassSet = &pSolaMessager->itemPassThreshold;
    int i;
    for(i = 0; i < WAVE_NUM; ++i)
    {
        pSolaItemPassSet->maxSpliceLoss.fValue[i] = pSolaItemPassSet->maxSpliceLoss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxLinkerLoss.fValue[i] = pSolaItemPassSet->maxLinkerLoss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxLinkerReflectance.fValue[i]= pSolaItemPassSet->maxLinkerReflectance.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitter2Loss.fValue[i] = pSolaItemPassSet->maxSplitter2Loss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitter4Loss.fValue[i] = pSolaItemPassSet->maxSplitter4Loss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitter8Loss.fValue[i] = pSolaItemPassSet->maxSplitter8Loss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitter16Loss.fValue[i] = pSolaItemPassSet->maxSplitter16Loss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitter32Loss.fValue[i] = pSolaItemPassSet->maxSplitter32Loss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitter64Loss.fValue[i] = pSolaItemPassSet->maxSplitter64Loss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitter128Loss.fValue[i] = pSolaItemPassSet->maxSplitter128Loss.fValue[WAVE_NUM];
        pSolaItemPassSet->maxSplitterReflectance.fValue[i] = pSolaItemPassSet->maxSplitterReflectance.fValue[WAVE_NUM];
        
        pSolaItemPassSet->maxSpliceLoss.iSelectedFlag[i] = pSolaItemPassSet->maxSpliceLoss.iSelectedFlag[WAVE_NUM];
        pSolaItemPassSet->maxLinkerLoss.iSelectedFlag[i] = pSolaItemPassSet->maxLinkerLoss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxLinkerReflectance.iSelectedFlag[i] = pSolaItemPassSet->maxLinkerReflectance.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitter2Loss.iSelectedFlag[i] = pSolaItemPassSet->maxSplitter2Loss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitter4Loss.iSelectedFlag[i] = pSolaItemPassSet->maxSplitter4Loss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitter8Loss.iSelectedFlag[i] = pSolaItemPassSet->maxSplitter8Loss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitter16Loss.iSelectedFlag[i] = pSolaItemPassSet->maxSplitter16Loss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitter32Loss.iSelectedFlag[i] = pSolaItemPassSet->maxSplitter32Loss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitter64Loss.iSelectedFlag[i] = pSolaItemPassSet->maxSplitter64Loss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitter128Loss.iSelectedFlag[i] = pSolaItemPassSet->maxSplitter128Loss.iSelectedFlag[WAVE_NUM];;
        pSolaItemPassSet->maxSplitterReflectance.iSelectedFlag[i] = pSolaItemPassSet->maxSplitterReflectance.iSelectedFlag[WAVE_NUM];;
    }
}

static int selectAllButtonUp(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    WAVELEN waveLength = getCurrentWaveLength();
    if(waveLength == WAVE_NUM)
    {
        setThresoldSelectState(WAVELEN_1310, 1);
        setThresoldSelectState(WAVELEN_1550, 1);
    }
    setThresoldSelectState(waveLength, 1);
    GUIWINDOW *pWnd = CreateSolaItemPassThresholdWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
    return 0;
}

static int selectAllButtonDown(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    TouchChange("btn_reset_press4.bmp", selectAllPicture,
                NULL, selectAllLabel, 0);
    
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

static int selectNoneButtonDown(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    TouchChange("btn_reset_press4.bmp", selectNonePicture,
                NULL, selectNoneLabel, 0);
    
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

static int selectNoneButtonUp(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    WAVELEN waveLength = getCurrentWaveLength();
    if(waveLength == WAVE_NUM)
    {
        setThresoldSelectState(WAVELEN_1310, 0);
        setThresoldSelectState(WAVELEN_1550, 0);
    }
    setThresoldSelectState(waveLength, 0);
    GUIWINDOW *pWnd = CreateSolaItemPassThresholdWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
    return 0;
}

static void ResetItemPassThresoldSet(void)
{
    SOLA_ITEM_PASSTHRESHOLD* pSolaItemPassSet = &pSolaMessager->itemPassThreshold;
    WAVELEN iCurrWaveLength = getCurrentWaveLength();
    pSolaItemPassSet->maxSpliceLoss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSpliceLoss.fValue[iCurrWaveLength] = 1.000;
    pSolaItemPassSet->maxLinkerLoss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxLinkerLoss.fValue[iCurrWaveLength] = 1.000;
    pSolaItemPassSet->maxLinkerReflectance.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxLinkerReflectance.fValue[iCurrWaveLength]= -40.0;
    pSolaItemPassSet->maxSplitter2Loss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitter2Loss.fValue[iCurrWaveLength] = 4.500;
    pSolaItemPassSet->maxSplitter4Loss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitter4Loss.fValue[iCurrWaveLength] = 8.500;
    pSolaItemPassSet->maxSplitter8Loss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitter8Loss.fValue[iCurrWaveLength] = 12.000;
    pSolaItemPassSet->maxSplitter16Loss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitter16Loss.fValue[iCurrWaveLength] = 15.000;
    pSolaItemPassSet->maxSplitter32Loss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitter32Loss.fValue[iCurrWaveLength] = 18.500;
    pSolaItemPassSet->maxSplitter64Loss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitter64Loss.fValue[iCurrWaveLength] = 22.000;
    pSolaItemPassSet->maxSplitter128Loss.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitter128Loss.fValue[iCurrWaveLength] = 25.500;
    pSolaItemPassSet->maxSplitterReflectance.iSelectedFlag[iCurrWaveLength] = 1;
    pSolaItemPassSet->maxSplitterReflectance.fValue[iCurrWaveLength] = -40.0;
    if(iCurrWaveLength == WAVE_NUM)
    {
        setAllWaveLengthValue();
    }
}

static int resetButtonUp(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    ResetItemPassThresoldSet();
    GUIWINDOW *pWnd = CreateSolaItemPassThresholdWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return iReturn;
}
static int resetButtonDown(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    TouchChange("btn_reset_press4.bmp", resetPicture,
                NULL, resetLabel, 0);
    RefreshScreen(__FILE__, __func__, __LINE__);
    //错误标志、返回值定义
    return 0;
}

int FrmSolaItemPassThresholdInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	
    //得到当前窗体对象
    pFrmSolaSetting = (GUIWINDOW *) pWndObj;

    //初始化文本资源
    //如果GUI存在多国语言，在此处获得对应语言的文本资源
    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    /****************************************************************/
    SolaSettingTextRes_Init(NULL, 0, NULL, 0);

    //建立窗体控件
    /****************************************************************/
    //建立状态栏、桌面、信息栏
    pSolaSettingBgDesk = CreatePhoto("bg_sola4");

    identifyTitlePicture =CreatePhoto("otdr_top1");
    lineDefineTitlePicture =CreatePhoto("otdr_top2");
    linePassThresholdTitlePicture =CreatePhoto("otdr_top3");
    itemPassThresholdTitlePicture =CreatePhoto("otdr_top4f");

    pSolaSettingLblIdentifyTitle = CreateLabel(0, 22, 100, 24, pSolaSettingStrIdentifyTitle);
    pSolaSettingLblLineDefineTitle = CreateLabel(100, 22, 100, 24, pSolaSettingStrLineDefineTitle);
    pSolaSettingLblLinePassThresholdTitle = CreateLabel(200, 22, 100, 24, pSolaSettingStrLinePassThresholdTitle);
    pSolaSettingLblItemPassThresholdTitle = CreateLabel(300, 22, 100, 24, pSolaSettingStrItemPassThresholdTitle);

    //侧边栏menu
	pSolaSettingMenu = CreateWndMenu1(0, sizeof(strSolaSettingMenu), strSolaSettingMenu, 
							   (UINT16)(MENU_BACK | ~MENU_HOME), 
							   -1, 0, 40, SolaSettingMenuCall);    
    //重构界面
    pItemPassThresoldLblTitle = CreateLabel(26, 56, 400, 24, pItemPassThresoldStrTitle);
    pItemPassThresoldLblWaveLength = CreateLabel(48, 82, 120, 24, pItemPassThresoldStrWaveLength);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblWaveLength);
    for(i = 0; i < WAVE_NUM; ++i)
    {
        pItemPassThresoldBtnWaveLength[i] = CreatePicture(208 + i*90, 82, 25, 25,
                                            BmpFileDirectory"bg_otdr_judgethreshold_wave_checkbox_unselect.bmp");
        pItemPassThresoldLblWave[i] = CreateLabel(240 + i*90, 86, 70, 24, pItemPassThresoldStrWave[i]);
        
    }

    pItemPassThresoldBtnAllWaveLength = CreatePicture(208 + 2*90, 82, 25, 25,
                                            BmpFileDirectory"bg_otdr_judgethreshold_wave_checkbox_unselect.bmp");
    pItemPassThresoldLblAllWaveLength = CreateLabel(240 + 2*90, 86, 70, 24, pItemPassThresoldStrAllWaveLength);
    
    for(i = 0; i < MAX_LOSS_NUM; ++i)
    {
        pItemPassThresoldBtnLoss[i] = CreatePicture(48, 112 + i*32, 25, 25,
                                            BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp");
        pItemPassThresoldLblLoss[i] = CreateLabel(78, 116 + 32 * i, 225, 24, pItemPassThresoldStrLoss[i]);
        pItemPassThresoldInputBtnLoss[i] = CreatePicture(298, 112 + 32 * i, 165, 24,
                                            BmpFileDirectory"bg_otdr_judgethreshold_enable.bmp");
        pItemPassThresoldLblInputLoss[i] = CreateLabel(298, 116 + 32 * i, 165, 24, pItemPassThresoldStrInputLoss[i]);
        pItemPassThresoldLblLossUnit[i] = CreateLabel(472, 116 + 32 * i, 100, 24, pItemPassThresoldStrLossUnit[i]);

        SetLabelAlign(GUILABEL_ALIGN_LEFT, pItemPassThresoldLblLoss[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pItemPassThresoldLblLossUnit[i]);
        SetLabelAlign(GUILABEL_ALIGN_CENTER, pItemPassThresoldLblInputLoss[i]);
        
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblLoss[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblLossUnit[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblInputLoss[i]);
    }
    selectAllPicture = CreatePicture(539, 296, 138, 48, BmpFileDirectory"btn_reset_unpress4.bmp");
    selectNonePicture = CreatePicture(539, 356, 138, 48, BmpFileDirectory"btn_reset_unpress4.bmp");
    resetPicture = CreatePhoto("btn_reset_unpress4");
    selectAllLabel = CreateLabel(537, 310, 136, 48, selectAllStr);
    selectNoneLabel = CreateLabel(537, 370, 136, 48, selectNoneStr);
    resetLabel = CreateLabel(537, 430, 136, 48, resetStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), selectAllLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), selectNoneLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), resetLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, selectAllLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, selectNoneLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, resetLabel);

    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblIdentifyTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblIdentifyTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLineDefineTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLineDefineTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLinePassThresholdTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLinePassThresholdTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaSettingLblItemPassThresholdTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblItemPassThresholdTitle);

    //设置按钮区的控件
    //...

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键及光标事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSolaSetting,
                  pFrmSolaSetting);
    //注册按键
    for(i = 0; i < WAVE_NUM; ++i)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL), pItemPassThresoldBtnWaveLength[i], 
                  pFrmSolaSetting);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pItemPassThresoldLblWave[i], 
                  pFrmSolaSetting);
    }

    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL), pItemPassThresoldBtnAllWaveLength, 
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pItemPassThresoldLblAllWaveLength, 
                  pFrmSolaSetting);
        
    for(i = 0; i < MAX_LOSS_NUM; ++i)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL), pItemPassThresoldBtnLoss[i], 
                  pFrmSolaSetting);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pItemPassThresoldLblLoss[i], 
                  pFrmSolaSetting);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUILABEL), pItemPassThresoldInputBtnLoss[i], 
                  pFrmSolaSetting);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pItemPassThresoldLblInputLoss[i], 
                  pFrmSolaSetting);
    }
    
    //注册桌面上的控件
    //...
    //注册按钮区的控件
	AddWndMenuToComp1(pSolaSettingMenu, pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), selectAllPicture,pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), selectNonePicture,pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), resetPicture,pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), identifyTitlePicture,pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), lineDefineTitlePicture,pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), linePassThresholdTitlePicture,pFrmSolaSetting);
    //注册信息栏上的控件
    //...

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //注册窗体的按键消息处理
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, identifyTitlePicture,
                    SolaLineDefineTabIdentify_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, identifyTitlePicture,
                    SolaLineDefineTabIdentify_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Up, NULL, 0, pMsg);

    for(i = 0; i < WAVE_NUM; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldBtnWaveLength[i], 
                    ItemPassThresoldBtnWaveLength_Down, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldBtnWaveLength[i], 
                    ItemPassThresoldBtnWaveLength_Up, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldLblWave[i], 
                    ItemPassThresoldBtnWaveLength_Down, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldLblWave[i], 
                    ItemPassThresoldBtnWaveLength_Up, (void *)(i), sizeof(int), pMsg);
    }

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldBtnAllWaveLength, 
                    ItemPassThresoldBtnAllWaveLength_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldBtnAllWaveLength, 
                    ItemPassThresoldBtnAllWaveLength_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldLblAllWaveLength, 
                    ItemPassThresoldBtnAllWaveLength_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldLblAllWaveLength, 
                    ItemPassThresoldBtnAllWaveLength_Up, NULL, 0, pMsg);

    for(i = 0; i < MAX_LOSS_NUM; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldBtnLoss[i], 
                    ItemPassThresoldBtnLoss_Down, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldBtnLoss[i], 
                    ItemPassThresoldBtnLoss_Up, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldLblLoss[i], 
                    ItemPassThresoldBtnLoss_Down, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldLblLoss[i], 
                    ItemPassThresoldBtnLoss_Up, (void *)(i), sizeof(int), pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldInputBtnLoss[i], 
                    ItemPassThresoldBtnInputLoss_Down, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldInputBtnLoss[i], 
                    ItemPassThresoldBtnInputLoss_Up, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pItemPassThresoldLblInputLoss[i], 
                    ItemPassThresoldBtnInputLoss_Down, (void *)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pItemPassThresoldLblInputLoss[i], 
                    ItemPassThresoldBtnInputLoss_Up, (void *)(i), sizeof(int), pMsg);
    }
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, selectAllPicture,selectAllButtonDown, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, selectAllPicture,selectAllButtonUp, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, selectNonePicture,selectNoneButtonDown, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, selectNonePicture,selectNoneButtonUp, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, resetPicture,resetButtonDown, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, resetPicture,resetButtonUp, NULL, 0, pMsg);
        //注册桌面上控件的消息处理
    //...
    //注册按钮区控件的消息处理
	LoginWndMenuToMsg1(pSolaSettingMenu, pFrmSolaSetting);
    //注册信息栏上控件的消息处理
    //...
    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmSolaSetting,
                    SolaSettingErrProc_Func, NULL, 0, pMsg);

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
int FrmSolaItemPassThresholdExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmSolaSetting = (GUIWINDOW *) pWndObj;


    //清空消息队列中的消息注册项
    //***************************************************************/
    ClearMessageReg(GetCurrMessage());

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmSolaSetting);

    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏
    DestroyPicture(&pSolaSettingBgDesk);
    DestroyPicture(&identifyTitlePicture);
    DestroyPicture(&lineDefineTitlePicture);
    DestroyPicture(&linePassThresholdTitlePicture);
    DestroyPicture(&itemPassThresholdTitlePicture);
    DestroyLabel(&pSolaSettingLblIdentifyTitle);
    DestroyLabel(&pSolaSettingLblLineDefineTitle);
    DestroyLabel(&pSolaSettingLblLinePassThresholdTitle);
    DestroyLabel(&pSolaSettingLblItemPassThresholdTitle);
    DestroyLabel(&selectAllLabel);
    DestroyLabel(&selectNoneLabel);
    DestroyLabel(&resetLabel);
    DestroyPicture(&selectAllPicture);
    DestroyPicture(&selectNonePicture);
    DestroyPicture(&resetPicture);
    //销毁按钮区的控件
	DestroyWndMenu1(&pSolaSettingMenu);    
    //销毁信息栏上的控件
    //...
    DestroyLabel(&pItemPassThresoldLblTitle);
    DestroyLabel(&pItemPassThresoldLblWaveLength);
    int i;
    for(i = 0; i < WAVE_NUM; ++i)
    {
        DestroyPicture(&pItemPassThresoldBtnWaveLength[i]);
        DestroyLabel(&pItemPassThresoldLblWave[i]);
    }

    DestroyPicture(&pItemPassThresoldBtnAllWaveLength);
    DestroyLabel(&pItemPassThresoldLblAllWaveLength);
    
    for(i = 0; i < MAX_LOSS_NUM; ++i)
    {
        DestroyPicture(&pItemPassThresoldBtnLoss[i]);
        DestroyLabel(&pItemPassThresoldLblLoss[i]);
        DestroyPicture(&pItemPassThresoldInputBtnLoss[i]);
        DestroyLabel(&pItemPassThresoldLblInputLoss[i]);
        DestroyLabel(&pItemPassThresoldLblLossUnit[i]);
    }
    //释放文本资源
    //***************************************************************/
    SolaSettingTextRes_Exit(NULL, 0, NULL, 0);

    //保存参数
    SetSettingsData((void*)&pSolaMessager->itemPassThreshold, sizeof(SOLA_ITEM_PASSTHRESHOLD), SOLA_ITEMPASS_SET);
    SaveSettings(SOLA_ITEMPASS_SET);
    
    return iReturn;
}

/***
  * 功能：
        窗体frmsolaSetting的绘制函数，绘制整个窗体
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaItemPassThresholdPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //得到当前窗体对象
    pFrmSolaSetting = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pSolaSettingBgDesk);
    DisplayPicture(identifyTitlePicture);
    DisplayPicture(lineDefineTitlePicture);
    DisplayPicture(linePassThresholdTitlePicture);
    DisplayPicture(itemPassThresholdTitlePicture);

    DisplayLabel(pSolaSettingLblIdentifyTitle);
    DisplayLabel(pSolaSettingLblLineDefineTitle);
    DisplayLabel(pSolaSettingLblLinePassThresholdTitle);
    DisplayLabel(pSolaSettingLblItemPassThresholdTitle);
    DisplayLabel(pItemPassThresoldLblTitle);
    DisplayLabel(pItemPassThresoldLblWaveLength);
        
    DisplayPicture(selectAllPicture);
    DisplayPicture(selectNonePicture);
    DisplayPicture(resetPicture);
    DisplayLabel(selectAllLabel);
    DisplayLabel(selectNoneLabel);
    DisplayLabel(resetLabel);
    Refresh();
    //显示按钮区的控件
	DisplayWndMenu1(pSolaSettingMenu);
    
	SetPowerEnable(1, 1);

    SyncCurrFbmap();

    return iReturn;
}

/***
  * 功能：
        窗体frmsolaLineDefine的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaItemPassThresholdLoop(void *pWndObj)
{
	/*错误标志、返回值定义*/
	int iErr = 0;

    SendWndMsg_LoopDisable(pWndObj);

	return iErr;
}


/***
  * 功能：
        窗体frmsolaLineDefine的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaItemPassThresholdPause(void *pWndObj)
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
int FrmSolaItemPassThresholdResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}


//初始化文本资源
static int SolaSettingTextRes_Init(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //初始化状态栏上的文本 
    pSolaSettingStrIdentifyTitle = TransString("SOLA_IDENTITY");
    pSolaSettingStrLineDefineTitle = TransString("SOLA_LINE_DEFINE");
    pSolaSettingStrLinePassThresholdTitle = TransString("SOLA_LINEPASS");
    pSolaSettingStrItemPassThresholdTitle = TransString("SOLA_ITEMPASS");
    
    pItemPassThresoldStrTitle = TransString("OTDR_THRESHOLD_THRESHLODTITLE");
    pItemPassThresoldStrWaveLength = TransString("OTDR_THRESHOLD_WAVE");

    int i;
    char* pStrWave[] = {"1310 nm", "1550 nm", "1625 nm"};
    for(i = 0; i < WAVE_NUM; ++i)
    {
        pItemPassThresoldStrWave[i] = TransString(pStrWave[i]);
    }

    pItemPassThresoldStrAllWaveLength = TransString("All");
    for(i = 0;i < MAX_LOSS_NUM; ++i)
    {
        pItemPassThresoldStrLoss[i] = TransString("loss");
        pItemPassThresoldStrLossUnit[i] = TransString("dB");
    }
    
    selectAllStr = TransString("THRESOLD_SET_SELECT_ALL");
    selectNoneStr = TransString("MESSAGEBOX_NO");
    resetStr = TransString("SOLA_RESET");
    return iReturn;
}


//释放文本资源
static int SolaSettingTextRes_Exit(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
    GuiMemFree(pSolaSettingStrIdentifyTitle);
    GuiMemFree(pSolaSettingStrLineDefineTitle);
    GuiMemFree(pSolaSettingStrLinePassThresholdTitle);
    GuiMemFree(pSolaSettingStrItemPassThresholdTitle);
    GuiMemFree(pItemPassThresoldStrTitle);
    GuiMemFree(pItemPassThresoldStrWaveLength);
    GuiMemFree(pItemPassThresoldStrAllWaveLength);
    int i;
    for(i = 0; i < WAVE_NUM; ++i)
    {
        GuiMemFree(pItemPassThresoldStrWave[i]);
    }
    for(i = 0; i < MAX_LOSS_NUM; ++i)
    {
        GuiMemFree(pItemPassThresoldStrLoss[i]);
        GuiMemFree(pItemPassThresoldStrInputLoss[i]);
        GuiMemFree(pItemPassThresoldStrLossUnit[i]);
    }
    GuiMemFree(selectAllStr);
    GuiMemFree(selectNoneStr);
    GuiMemFree(resetStr);
    return iReturn;
}

static void SolaSettingMenuCall(int buttonIndex)
{
    GUIWINDOW* pWnd = NULL;
	
	switch (buttonIndex)
	{
	case 0:
        break;
	case 1:
        break;
	case 2:
		break;
	case HOME_DOWN:
        break;
	case BACK_DOWN: 
        {
        	pWnd = CreateSolaWindow();
        	SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
        	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
       }
        break;
	default:
		break;
	}
}

static void SetLossState(int waveLength, int iSelected, int index)
{
    char *checkBox[] = {BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp",
                        BmpFileDirectory"bg_otdr_judgethreshold_checkbox_select.bmp"};
    char *paraBg[] = {BmpFileDirectory"bg_otdr_judgethreshold_disable.bmp",
                        BmpFileDirectory"bg_otdr_judgethreshold_enable.bmp"};
    GUIFONT *color[2] = {getGlobalFnt(EN_FONT_GRAY),
                        getGlobalFnt(EN_FONT_WHITE)};
    SetPictureBitmap(checkBox[iSelected], pItemPassThresoldBtnLoss[index]);
    SetPictureBitmap(paraBg[iSelected], pItemPassThresoldInputBtnLoss[index]);
    SetLabelFont(color[iSelected], pItemPassThresoldLblInputLoss[index]);
    GuiMemFree(pItemPassThresoldStrInputLoss[index]);

    switch(index)
    {
        case 0:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[waveLength]);
            break;
        case 1:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[waveLength]);
            break;
        case 2:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(REFLECTANCE_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[waveLength]);
            break;
        case 3:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[waveLength]);
            break;
        case 4:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[waveLength]);
            break;
        case 5:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[waveLength]);
            break;
        case 6:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[waveLength]);
            break;
        case 7:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[waveLength]);
            break;
        case 8:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[waveLength]);
            break;
        case 9:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(LOSS_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[waveLength]);
            break;
        case 10:
            pItemPassThresoldStrInputLoss[index] = Float2GuiString(REFLECTANCE_PRECISION, 
	            pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[waveLength]);
            break;
        default:
            break;
    }
	
	SetLabelText(pItemPassThresoldStrInputLoss[index], pItemPassThresoldLblInputLoss[index]);
}

static void Refresh(void)
{
    WAVELEN iCurrentWave = getCurrentWaveLength();
    
    int i;

    if(iCurrentWave == WAVE_NUM)
    {
        for(i = 0; i < WAVE_NUM; ++i)
        {
            SetPictureBitmap(BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp",
                                pItemPassThresoldBtnWaveLength[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblWave[i]);
        }
        
        SetPictureBitmap(BmpFileDirectory"bg_otdr_judgethreshold_checkbox_select.bmp",
                                    pItemPassThresoldBtnAllWaveLength);
        SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pItemPassThresoldLblAllWaveLength);
    }
    else
    {
        SetPictureBitmap(BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp",
                                    pItemPassThresoldBtnAllWaveLength);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblAllWaveLength);
        for(i = 0; i < WAVE_NUM; i++)
        {
            if(i == iCurrentWave)
            {
                SetPictureBitmap(BmpFileDirectory"bg_otdr_judgethreshold_checkbox_select.bmp",
                                    pItemPassThresoldBtnWaveLength[i]);
                SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pItemPassThresoldLblWave[i]);
            }
            else
            {
                SetPictureBitmap(BmpFileDirectory"bg_otdr_judgethreshold_checkbox_unselect.bmp",
                                    pItemPassThresoldBtnWaveLength[i]);
                SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pItemPassThresoldLblWave[i]);
            }
        }
    }
    
    int isSelected = 0;
    isSelected = pSolaMessager->itemPassThreshold.maxSpliceLoss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 0);
    isSelected = pSolaMessager->itemPassThreshold.maxLinkerLoss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 1);
    isSelected = pSolaMessager->itemPassThreshold.maxLinkerReflectance.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 2);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitter2Loss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 3);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitter4Loss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 4);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitter8Loss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 5);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitter16Loss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 6);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitter32Loss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 7);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitter64Loss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 8);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitter128Loss.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 9);
    isSelected = pSolaMessager->itemPassThreshold.maxSplitterReflectance.iSelectedFlag[iCurrentWave];
    SetLossState(iCurrentWave, isSelected, 10);

    for (i = 0; i < WAVE_NUM; ++i)
	{
	    if(i == WAVELEN_1625)//隐藏1625nm图标显示
        {
            SetPictureEnable(0, pItemPassThresoldBtnWaveLength[i]);
            SetLabelEnable(0, pItemPassThresoldLblWave[i]);
        } 
        else
        {
            DisplayPicture(pItemPassThresoldBtnWaveLength[i]);
		    DisplayLabel(pItemPassThresoldLblWave[i]);
        }
	}

    DisplayPicture(pItemPassThresoldBtnAllWaveLength);
    DisplayLabel(pItemPassThresoldLblAllWaveLength);
            
	for (i = 0; i < MAX_LOSS_NUM; ++i)
	{
		DisplayPicture(pItemPassThresoldBtnLoss[i]);
		DisplayPicture(pItemPassThresoldInputBtnLoss[i]);
        
		DisplayLabel(pItemPassThresoldLblLoss[i]);
		DisplayLabel(pItemPassThresoldLblInputLoss[i]);
		DisplayLabel(pItemPassThresoldLblLossUnit[i]);
	}
    
}

//错误事件处理函数
static int SolaSettingErrProc_Func(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}

//切换到Identify窗体
static int SolaLineDefineTabIdentify_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaLineDefineTabIdentify_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = CreateSolaIdentWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}

//切换到LineDefine窗体
static int SolaIdentifyTabLineDefine_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaIdentifyTabLineDefine_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = CreateSolaLineDefineWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}


//切换到LinePass窗体
static int SolaIdentifyTabLinePassThreshold_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaIdentifyTabLinePassThreshold_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = CreateSolaLinePassThresholdWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}

static int ItemPassThresoldBtnWaveLength_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    int temp = (int)pOutArg;
    pSolaMessager->itemPassThreshold.iCurrentWave = temp;
    Refresh();
	RefreshScreen(__FILE__, __func__, __LINE__);

    return iReturn;
}
static int ItemPassThresoldBtnWaveLength_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}

static int ItemPassThresoldBtnAllWaveLength_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    pSolaMessager->itemPassThreshold.iCurrentWave = WAVE_NUM;
    setAllWaveLengthValue();
    Refresh();
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}

static int ItemPassThresoldBtnAllWaveLength_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}

static int ItemPassThresoldBtnLoss_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    int temp = (int)pOutArg;
    LOG(LOG_INFO, "----ItemPassThresoldBtnLoss_Down-----temp %d\n", temp);
    WAVELEN iCurrWaveLength = getCurrentWaveLength();
    char *p = NULL;
    switch(temp)
    {
        case 0:
            p = &pSolaMessager->itemPassThreshold.maxSpliceLoss.iSelectedFlag[iCurrWaveLength];
            break;
        case 1:
            p = &pSolaMessager->itemPassThreshold.maxLinkerLoss.iSelectedFlag[iCurrWaveLength];
            break;
        case 2:
            p = &pSolaMessager->itemPassThreshold.maxLinkerReflectance.iSelectedFlag[iCurrWaveLength];
            break;
        case 3:
            p = &pSolaMessager->itemPassThreshold.maxSplitter2Loss.iSelectedFlag[iCurrWaveLength];
            break;
        case 4:
            p = &pSolaMessager->itemPassThreshold.maxSplitter4Loss.iSelectedFlag[iCurrWaveLength];
            break;
        case 5:
            p = &pSolaMessager->itemPassThreshold.maxSplitter8Loss.iSelectedFlag[iCurrWaveLength];
            break;
        case 6:
            p = &pSolaMessager->itemPassThreshold.maxSplitter16Loss.iSelectedFlag[iCurrWaveLength];
            break;
        case 7:
            p = &pSolaMessager->itemPassThreshold.maxSplitter32Loss.iSelectedFlag[iCurrWaveLength];
            break;
        case 8:
            p = &pSolaMessager->itemPassThreshold.maxSplitter64Loss.iSelectedFlag[iCurrWaveLength];
            break;
        case 9:
            p = &pSolaMessager->itemPassThreshold.maxSplitter128Loss.iSelectedFlag[iCurrWaveLength];
            break;
        case 10:
            p = &pSolaMessager->itemPassThreshold.maxSplitterReflectance.iSelectedFlag[iCurrWaveLength];
            break;
        default:
            break;
    }

    *p = *p?0:1;
    if(iCurrWaveLength == WAVE_NUM)
    {
        setAllWaveLengthValue();
    }
    
    Refresh();
	RefreshScreen(__FILE__, __func__, __LINE__);
    return iReturn;
}
static int ItemPassThresoldBtnLoss_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}

static void ItemPassThresoldIMEDialog(float min, float max)
{
    GUICHAR* string1Mem = NULL;
    GUICHAR* string2Mem = NULL;
    if(min == -80.0f && max == 0.0f)
    {
        string1Mem = Float2GuiString(REFLECTANCE_PRECISION, min);
        string2Mem = Float2GuiString(REFLECTANCE_PRECISION, max);
    }
    else
    {
        string1Mem = Float2GuiString(LOSS_PRECISION, min);
        string2Mem = Float2GuiString(LOSS_PRECISION, max);
    }
    GUICHAR* curText1 = TransString("INPUTVALUE_OUT_OF_RANGE_1");
    GUICHAR* curText2 = TransString("MAXLINKERLOSS_OUT_OF_RANGE_2");
    GUICHAR* curAnd = TransString("AND");
    GUICHAR* warningData1 = StringCat(&curText1,string1Mem);
    GUICHAR* warningData2 = StringCat(&warningData1,curAnd);
    GUICHAR* warningData3 = StringCat(&warningData2,string2Mem);
    GUICHAR* warningData = StringCat(&warningData3,curText2);
    CreateIMEDialog(warningData);
    
    GuiMemFree(string1Mem);
    GuiMemFree(string2Mem);
    GuiMemFree(curAnd);
    GuiMemFree(curText2);
}

static void InputMaxThresholdBackFunc(void)
{
    char cTmpBuff[100];
	GetIMEInputBuff(cTmpBuff);
	double fTmp = String2Float(cTmpBuff);
	WAVELEN wavelength = getCurrentWaveLength();
	if (fTmp < rangeValues[loss_index].min || fTmp > rangeValues[loss_index].max)
	{
		ItemPassThresoldIMEDialog(rangeValues[loss_index].min, rangeValues[loss_index].max);
		return;
	}

    switch(loss_index)
    {
        case 0:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[wavelength] = fTmp;
            break;
        case 1:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[wavelength] = fTmp;
            break;
        case 2:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[wavelength] = fTmp;
            break;
        case 3:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[wavelength] = fTmp;
            break;
        case 4:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[wavelength] = fTmp;
            break;
        case 5:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[wavelength] = fTmp;
            break;
        case 6:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[wavelength] = fTmp;
            break;
        case 7:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[wavelength] = fTmp;
            break;
        case 8:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[wavelength] = fTmp;
            break;
        case 9:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[wavelength] = fTmp;
            break;
        case 10:
            if(wavelength == WAVE_NUM)
            {
                pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[WAVELEN_1310] = fTmp;
                pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[WAVELEN_1550] = fTmp;
            }
            pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[wavelength] = fTmp;
            break;
        default:
            break;
    }
	
}

static void setThresoldValue(PRECISION precision, int iSelectedFlag, float data)
{
    char *buf = NULL;
    if(iSelectedFlag)
    {
        buf = Float2String(precision, data);
        IMEInit(buf, 10, 1, ReCreateWindow, InputMaxThresholdBackFunc, NULL);
        free(buf);
        buf = NULL;
	}
}

static int ItemPassThresoldBtnInputLoss_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    int iTmp = (int)pOutArg;
    loss_index = iTmp;
    LOG(LOG_INFO, "----ItemPassThresoldBtnInputLoss_Down-----temp %d\n", iTmp);
    WAVELEN iCurrWaveLength = getCurrentWaveLength();
    switch (iTmp)
	{
    	case 0:
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSpliceLoss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[iCurrWaveLength]);
    		break;
        case 1:
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxLinkerLoss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[iCurrWaveLength]);
            break;
        case 2://
            setThresoldValue(REFLECTANCE_PRECISION, pSolaMessager->itemPassThreshold.maxLinkerReflectance.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[iCurrWaveLength]);
            break;
        case 3://
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSplitter2Loss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitter2Loss.fValue[iCurrWaveLength]);
            break;
        case 4://
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSplitter4Loss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitter4Loss.fValue[iCurrWaveLength]);
            break;
        case 5://
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSplitter8Loss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitter8Loss.fValue[iCurrWaveLength]);
            break;
        case 6://
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSplitter16Loss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitter16Loss.fValue[iCurrWaveLength]);
            break;
        case 7://
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSplitter32Loss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitter32Loss.fValue[iCurrWaveLength]);
            break;
        case 8://
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSplitter64Loss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitter64Loss.fValue[iCurrWaveLength]);
            break;
        case 9://
            setThresoldValue(LOSS_PRECISION, pSolaMessager->itemPassThreshold.maxSplitter128Loss.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitter128Loss.fValue[iCurrWaveLength]);
            break;
        case 10://
            setThresoldValue(REFLECTANCE_PRECISION, pSolaMessager->itemPassThreshold.maxSplitterReflectance.iSelectedFlag[iCurrWaveLength],
                pSolaMessager->itemPassThreshold.maxSplitterReflectance.fValue[iCurrWaveLength]);
            break;
        default:
            break;

    }
    return iReturn;
}
static int ItemPassThresoldBtnInputLoss_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iReturn = 0;

    return iReturn;
}

static void ReCreateWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
          	             FrmSolaItemPassThresholdInit, FrmSolaItemPassThresholdExit, 
          	             FrmSolaItemPassThresholdPaint, FrmSolaItemPassThresholdLoop, 
          				 FrmSolaItemPassThresholdPause, FrmSolaItemPassThresholdResume,
					     NULL);
}

//创建solaLineDefine window
GUIWINDOW* CreateSolaItemPassThresholdWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
          	                       FrmSolaItemPassThresholdInit, FrmSolaItemPassThresholdExit, 
          	                       FrmSolaItemPassThresholdPaint, FrmSolaItemPassThresholdLoop, 
          				           FrmSolaItemPassThresholdPause, FrmSolaItemPassThresholdResume,
          	                       NULL);
    return pWnd;
}

static WAVELEN getCurrentWaveLength()
{
    WAVELEN wavelength = pSolaMessager->itemPassThreshold.iCurrentWave;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
    return wavelength;
}