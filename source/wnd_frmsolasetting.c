/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolasetting.c
* 摘    要：  实现初始化窗体frmsolasetting的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2016-11-8
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#include "wnd_frmsolasetting.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmsola.h"
#include "wnd_frmselector.h"
#include "wnd_frmsolaconfig.h"
#include "app_frmotdr.h"
#include "app_frminit.h"
#include "guiphoto.h"
#include "wnd_frmsavedefdir.h"
#include "wnd_frmime.h"
#include "app_unitconverter.h"

#define MAX_UNIT_NUM            5

static int iUnitConverterFlag = 0;              //单位转换的标志位

static char *SolaSettingCheckBoxBmp[] = {	
	BmpFileDirectory"bg_comset_unselect.bmp", 
	BmpFileDirectory"bg_comset_select.bmp"
};

static char *pSolaSavePath = NULL;

static GUIWINDOW *pFrmSolaSetting = NULL;

static GUICHAR* pSolaSettingStrSolaTitle = NULL;
static GUILABEL* pSolaSettingLblSolaTitle = NULL;

static GUICHAR* pSolaSettingStrWaveSet = NULL;
static GUILABEL* pSolaSettingLblWaveSet = NULL;

static GUICHAR* pSolaSettingStrSettingTitle = NULL;
static GUILABEL* pSolaSettingLblSettingTitle = NULL;

static GUICHAR* pSolaSettingStrWave[3] = {NULL};
static GUILABEL* pSolaSettingLblWave[3] = {NULL};

static GUICHAR* pSolaSettingStrDefaultPathTitle = NULL;
static GUILABEL* pSolaSettingLblDefaultPathTitle = NULL;

static GUICHAR* pSolaSettingStrDefaultPath = NULL;
static GUILABEL* pSolaSettingLblDefaultPath = NULL;

//单位换算设置
static GUICHAR* pSolaSettingStrUnitTitle = NULL;
static GUILABEL* pSolaSettingLblUnitTitle = NULL;
static GUICHAR* pSolaSettingStrUnit[MAX_UNIT_NUM] = {NULL};
static GUILABEL* pSolaSettingLblUnit[MAX_UNIT_NUM] = {NULL};
static GUIPICTURE* pSolaSettingUnit[MAX_UNIT_NUM] = {NULL};

//注入光纤和接收光纤
static GUICHAR *pSolaSettingLaunchRecvText = NULL;
static GUICHAR *pSolaSettingLaunchFiberLblText = NULL;
static GUICHAR *pSolaSettingRecvFiberLblText = NULL;
static GUICHAR *pSolaSettingLaunchFiberEditText = NULL;
static GUICHAR *pSolaSettingRecvFiberEditText = NULL;
static GUICHAR *pSolaSettingLaunchFiberUnitText = NULL;
static GUICHAR *pSolaSettingRecvFiberUnitText = NULL;
//注入光纤和接收光纤
static GUILABEL *pSolaSettingLaunchRecvLabel = NULL;
static GUILABEL *pSolaSettingLaunchFiberLblLabel = NULL;
static GUILABEL *pSolaSettingRecvFiberLblLabel = NULL;
static GUILABEL *pSolaSettingLaunchFiberEditLabel = NULL;
static GUILABEL *pSolaSettingRecvFiberEditLabel = NULL;
static GUILABEL *pSolaSettingLaunchFiberUnitLabel = NULL;
static GUILABEL *pSolaSettingRecvFiberUnitLabel = NULL;

//注入光纤和接收光纤
static GUIPICTURE *pSolaSettingLaunchFiberCheckBox = NULL;
static GUIPICTURE *pSolaSettingRecvFiberCheckBox = NULL;
static GUIPICTURE *pSolaSettingLaunchFiberEdit = NULL;
static GUIPICTURE *pSolaSettingRecvFiberEdit = NULL;

static GUIPICTURE *pSolaSettingBgDesk = NULL;
static GUIPICTURE *pSolaSettingBgTableTitle1 = NULL;
static GUIPICTURE *pSolaSettingBgTableTitle2 = NULL;
static WNDMENU1* pSolaSettingMenu = NULL;
static GUIPICTURE* pSolaSettingWaveSelect[3] = {NULL};
OPM_WAVE enSolaWaveChoose = ENUM_WAVE_1310NM;
int currentSolaWaveIndex = 0;
/*********************************
*声明wnd_frmcommonset.c所引用的外部变量
*********************************/

/********************************
* 窗体frmsolaSetting中的文本资源处理函数
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


static int SolaSettingTabSola_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaSettingTabSola_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);
static int SolaSettingWaveSelect_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaSettingWaveSelect_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int SolaSettingDefaultPath_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen);
static int SolaSettingDefaultPath_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen);

static int SolaSettingLaunchCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int SolaSettingLaunchCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int SolaSettingRecvCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int SolaSettingRecvCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int SolaSettingLaunchEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int SolaSettingLaunchEdit_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int SolaSettingRecvEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int SolaSettingRecvEdit_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);

static int SolaSettingUnitSelect_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen);
static int SolaSettingUnitSelect_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);
                                 
static void SolaSettingMenuCall(int buttonIndex);
static void SelectWaveLenBackCall (int iSelected);
//注入光纤和接收光纤输入回调
static void SolaSettingInputLaunchLenCallBack(void);
static void SolaSettingInputRecvLenCallBack(void);

//右侧菜单文本
static unsigned int strSolaSettingMenu[] = {	
	
};

int FrmSolaSettingInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
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
    pSolaSettingBgDesk = CreatePhoto("bg_solasetting");
    pSolaSettingBgTableTitle1 = CreatePhoto("otdr_top1");
    pSolaSettingBgTableTitle2 = CreatePhoto("otdr_top2f");
    pSolaSettingLblSolaTitle = CreateLabel(0, 24, 100, 42, pSolaSettingStrSolaTitle);
    pSolaSettingLblSettingTitle = CreateLabel(100, 24, 100, 42, pSolaSettingStrSettingTitle);
    
  	//侧边栏menu
	pSolaSettingMenu = CreateWndMenu1(0, sizeof(strSolaSettingMenu), strSolaSettingMenu, 
							   (UINT16)(MENU_BACK | ~MENU_HOME), 
							   -1, 0, 40, SolaSettingMenuCall);

    pSolaSettingLblWaveSet = CreateLabel(39, 55, 500, 24, pSolaSettingStrWaveSet);

    if(pUser_Settings->sSolaSetting.Wave[0])
    {
        if(pUser_Settings->sSolaSetting.Wave[1])
        {
            currentSolaWaveIndex = 2;
        }
        else
        {
            currentSolaWaveIndex = 0;
        }
    }
    else
    {
        if(pUser_Settings->sSolaSetting.Wave[1])
        {
            currentSolaWaveIndex = 1;
        }
    }


    CreateLoopPhoto(pSolaSettingWaveSelect, "bg_sloasetting_unselect", 0, 0, 3);
    int i;
    for(i = 0; i < 3; ++i)
    {
        pSolaSettingLblWave[i] = CreateLabel(117 + i * 135, 98, 90, 24, pSolaSettingStrWave[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblWave[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaSettingLblWave[i]);
        if(i == currentSolaWaveIndex)
        {
            SetPictureBitmap(BmpFileDirectory"bg_sloasetting_select.bmp", pSolaSettingWaveSelect[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pSolaSettingLblWave[i]);
        }
    }
    pSolaSettingLblDefaultPathTitle = CreateLabel(39, 406, 500, 24, pSolaSettingStrDefaultPathTitle);
    pSolaSettingLblDefaultPath = CreateLabel(70, 435, 500, 24, pSolaSettingStrDefaultPath);

    //单位设置
    pSolaSettingLblUnitTitle = CreateLabel(39, 150, 300, 24, pSolaSettingStrUnitTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblUnitTitle);
    for(i = 0; i < MAX_UNIT_NUM; ++i)
    {
        if(i < 3)
        {
            pSolaSettingUnit[i] = CreatePicture(79+180*i, 188, 25, 25, BmpFileDirectory"bg_sloasetting_unselect.bmp");
            pSolaSettingLblUnit[i] = CreateLabel(117+180*i, 193, 90, 24, pSolaSettingStrUnit[i]);
        }
        else
        {
            pSolaSettingUnit[i] = CreatePicture(79+180*(i-3), 228, 25, 25, BmpFileDirectory"bg_sloasetting_unselect.bmp");
            pSolaSettingLblUnit[i] = CreateLabel(117+180*(i-3), 233, 90, 24, pSolaSettingStrUnit[i]);
        }

        if(i == GetCurrSystemUnit(MODULE_UNIT_SOLA))
        {
            SetPictureBitmap(BmpFileDirectory"bg_sloasetting_select.bmp", pSolaSettingUnit[i]);
        }
        
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblUnit[i]);
    }
    
    //注入光纤和接收光纤
    pSolaSettingLaunchRecvLabel = CreateLabel(39, 280, 500, 24, pSolaSettingLaunchRecvText);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLaunchRecvLabel);

    pSolaSettingLaunchFiberLblLabel = CreateLabel(130, 324, 240, 24, pSolaSettingLaunchFiberLblText);
  	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLaunchFiberLblLabel);
    pSolaSettingRecvFiberLblLabel = CreateLabel(130, 373, 240, 24, pSolaSettingRecvFiberLblText);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingRecvFiberLblLabel);

    pSolaSettingLaunchFiberEditLabel = CreateLabel(370, 324, 142, 24, pSolaSettingLaunchFiberEditText);
	SetLabelAlign(GUILABEL_ALIGN_RIGHT, pSolaSettingLaunchFiberEditLabel);
    if (!pUser_Settings->sSolaSetting.iEnableLaunchFiber)
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pSolaSettingLaunchFiberEditLabel);

    pSolaSettingRecvFiberEditLabel = CreateLabel(370, 373, 142, 24, pSolaSettingRecvFiberEditText);
	SetLabelAlign(GUILABEL_ALIGN_RIGHT, pSolaSettingRecvFiberEditLabel);
    if (!pUser_Settings->sSolaSetting.iEnableRecvFiber)
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pSolaSettingRecvFiberEditLabel);

    pSolaSettingLaunchFiberUnitLabel = CreateLabel(520, 324, 100, 24, pSolaSettingLaunchFiberUnitText);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLaunchFiberUnitLabel);
    pSolaSettingRecvFiberUnitLabel = CreateLabel(520, 373, 100, 24, pSolaSettingRecvFiberUnitText);
  	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingRecvFiberUnitLabel);

    pSolaSettingLaunchFiberCheckBox = CreatePicture(100, 319, 25, 25, 
        SolaSettingCheckBoxBmp[pUser_Settings->sSolaSetting.iEnableLaunchFiber]);
    pSolaSettingRecvFiberCheckBox = CreatePicture(100, 368, 25, 25, 
        SolaSettingCheckBoxBmp[pUser_Settings->sSolaSetting.iEnableRecvFiber]);
    pSolaSettingLaunchFiberEdit = CreatePicture(370, 319, 145, 25, BmpFileDirectory"input_loss_unpress.bmp");
    pSolaSettingRecvFiberEdit = CreatePicture(370, 368, 145, 25, BmpFileDirectory"input_loss_unpress.bmp");
    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblWaveSet);

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblSolaTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblSolaTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaSettingLblSettingTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblSettingTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblDefaultPathTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblDefaultPath);

    //设置按钮区的控件
    //...

    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行
    //***************************************************************/
    //注册窗体(因为所有的按键及光标事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSolaSetting,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaSettingBgTableTitle1, 
                  pFrmSolaSetting);
    for(i = 0; i < 3; ++i)
    {
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSettingLblWave[i], pFrmSolaSetting);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaSettingWaveSelect[i], pFrmSolaSetting);
    }

    for(i = 0; i < MAX_UNIT_NUM; ++i)
    {
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSettingLblUnit[i], pFrmSolaSetting);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaSettingUnit[i], pFrmSolaSetting);
    }
    
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSettingLblDefaultPath, pFrmSolaSetting);

    //注入光纤
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSettingLaunchFiberLblLabel, pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaSettingLaunchFiberCheckBox, pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaSettingLaunchFiberEdit, pFrmSolaSetting);

    //接收光纤
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSettingRecvFiberLblLabel, pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaSettingRecvFiberCheckBox, pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaSettingRecvFiberEdit, pFrmSolaSetting);
    //注册桌面上的控件
    //...
    //注册按钮区的控件
	AddWndMenuToComp1(pSolaSettingMenu, pFrmSolaSetting);
    //注册信息栏上的控件
    //...

    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    //注册消息处理函数必须在持有锁的前提下进行
    //***************************************************************/
    pMsg = GetCurrMessage();
    //注册窗体的按键消息处理
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingBgTableTitle1, 
                    SolaSettingTabSola_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingBgTableTitle1, 
                    SolaSettingTabSola_Up, NULL, 0, pMsg);
                    
    for(i = 0; i < 3; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingLblWave[i], 
                    SolaSettingWaveSelect_Down, (void*)(i), sizeof(int), pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingLblWave[i], 
                    SolaSettingWaveSelect_Up, (void*)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingWaveSelect[i], 
                    SolaSettingWaveSelect_Down, (void*)(i), sizeof(int), pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingWaveSelect[i], 
                    SolaSettingWaveSelect_Up, (void*)(i), sizeof(int), pMsg);
        
    }

    for(i = 0; i < MAX_UNIT_NUM; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingLblUnit[i], 
                    SolaSettingUnitSelect_Down, (void*)(i), sizeof(int), pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingLblUnit[i], 
                    SolaSettingUnitSelect_Up, (void*)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingUnit[i], 
                    SolaSettingUnitSelect_Down, (void*)(i), sizeof(int), pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingUnit[i], 
                    SolaSettingUnitSelect_Up, (void*)(i), sizeof(int), pMsg);
    }
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingLblDefaultPath, 
                SolaSettingDefaultPath_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingLblDefaultPath, 
                SolaSettingDefaultPath_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingLaunchFiberLblLabel, 
                SolaSettingLaunchCheckBox_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingLaunchFiberLblLabel, 
                SolaSettingLaunchCheckBox_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingLaunchFiberCheckBox, 
                SolaSettingLaunchCheckBox_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingLaunchFiberCheckBox, 
                SolaSettingLaunchCheckBox_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingRecvFiberLblLabel, 
                SolaSettingRecvCheckBox_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingRecvFiberLblLabel, 
                SolaSettingRecvCheckBox_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingRecvFiberCheckBox, 
                SolaSettingRecvCheckBox_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingRecvFiberCheckBox, 
                SolaSettingRecvCheckBox_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingLaunchFiberEdit, 
                SolaSettingLaunchEdit_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingLaunchFiberEdit, 
                SolaSettingLaunchEdit_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSettingRecvFiberEdit, 
                SolaSettingRecvEdit_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSettingRecvFiberEdit, 
                SolaSettingRecvEdit_Up, NULL, 0, pMsg);

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
int FrmSolaSettingExit(void *pWndObj)
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
    DestroyPicture(&pSolaSettingBgTableTitle1);
    DestroyPicture(&pSolaSettingBgTableTitle2);
    int i;
    for(i = 0; i < 3; ++i)
    {
        DestroyPicture(&pSolaSettingWaveSelect[i]);
        DestroyLabel(&pSolaSettingLblWave[i]);
    }
    for(i = 0; i < MAX_UNIT_NUM; ++i)
    {
        DestroyPicture(&pSolaSettingUnit[i]);
        DestroyLabel(&pSolaSettingLblUnit[i]);
    }
    //DestroyPicture(&pSolaSettingDialogBg);
    DestroyLabel(&pSolaSettingLblWaveSet);
	DestroyLabel(&pSolaSettingLblSolaTitle);
	DestroyLabel(&pSolaSettingLblSettingTitle);    
    DestroyLabel(&pSolaSettingLblUnitTitle);
	//DestroySelector(&pSolaSettingSelector);
    DestroyLabel(&pSolaSettingLblDefaultPathTitle);
    DestroyLabel(&pSolaSettingLblDefaultPath);
    
    //注入光纤和接收光纤
    DestroyLabel(&pSolaSettingLaunchRecvLabel);
    DestroyLabel(&pSolaSettingLaunchFiberLblLabel);
    DestroyLabel(&pSolaSettingRecvFiberLblLabel);
    DestroyLabel(&pSolaSettingLaunchFiberEditLabel);
    DestroyLabel(&pSolaSettingRecvFiberEditLabel);
    DestroyLabel(&pSolaSettingLaunchFiberUnitLabel);
    DestroyLabel(&pSolaSettingRecvFiberUnitLabel);
    DestroyPicture(&pSolaSettingLaunchFiberCheckBox);
    DestroyPicture(&pSolaSettingRecvFiberCheckBox);
    DestroyPicture(&pSolaSettingLaunchFiberEdit);
    DestroyPicture(&pSolaSettingRecvFiberEdit);
    
    //销毁按钮区的控件
	DestroyWndMenu1(&pSolaSettingMenu);    
    //销毁信息栏上的控件
    //...

    //释放文本资源
    //***************************************************************/
    SolaSettingTextRes_Exit(NULL, 0, NULL, 0);

    //释放字体资源
    //***************************************************************/
    //释放按钮区的字体
    //...
    
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
int FrmSolaSettingPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //得到当前窗体对象
    pFrmSolaSetting = (GUIWINDOW *) pWndObj;

    //显示状态栏、桌面、信息栏
    DisplayPicture(pSolaSettingBgDesk);
    DisplayPicture(pSolaSettingBgTableTitle1);
    DisplayPicture(pSolaSettingBgTableTitle2);
    int i;
    for(i = 0; i < 3; ++i)
    {
        DisplayPicture(pSolaSettingWaveSelect[i]);
        DisplayLabel(pSolaSettingLblWave[i]);
    }
    for(i = 0; i < MAX_UNIT_NUM; ++i)
    {
        DisplayPicture(pSolaSettingUnit[i]);
        DisplayLabel(pSolaSettingLblUnit[i]);
    }
    DisplayLabel(pSolaSettingLblWaveSet);
    DisplayLabel(pSolaSettingLblSettingTitle);    
    DisplayLabel(pSolaSettingLblSolaTitle);    
    DisplayLabel(pSolaSettingLblUnitTitle);
	//DisplaySelector(pSolaSettingSelector);
    DisplayLabel(pSolaSettingLblDefaultPathTitle);    
    DisplayLabel(pSolaSettingLblDefaultPath);

    //注入光纤和接收光纤
    DisplayLabel(pSolaSettingLaunchRecvLabel);
    DisplayLabel(pSolaSettingLaunchFiberLblLabel);
    DisplayLabel(pSolaSettingRecvFiberLblLabel);
    DisplayLabel(pSolaSettingLaunchFiberUnitLabel);
    DisplayLabel(pSolaSettingRecvFiberUnitLabel);    
    DisplayPicture(pSolaSettingLaunchFiberCheckBox);
    DisplayPicture(pSolaSettingRecvFiberCheckBox);
    DisplayPicture(pSolaSettingLaunchFiberEdit);
    DisplayPicture(pSolaSettingRecvFiberEdit);
    DisplayLabel(pSolaSettingLaunchFiberEditLabel);
    DisplayLabel(pSolaSettingRecvFiberEditLabel);
    
    //显示按钮区的控件
	DisplayWndMenu1(pSolaSettingMenu);
    
	SetPowerEnable(1, 1);

    SyncCurrFbmap();

    return iReturn;
}


/***
  * 功能：
        窗体frmsolaSetting的循环函数，进行窗体循环
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaSettingLoop(void *pWndObj)
{
	/*错误标志、返回值定义*/
	int iErr = 0;

    SendWndMsg_LoopDisable(pWndObj);

	return iErr;
}


/***
  * 功能：
        窗体frmsolaSetting的挂起函数，进行窗体挂起前预处理
  * 参数：
        1.void *pWndObj:    指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmSolaSettingPause(void *pWndObj)
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
int FrmSolaSettingResume(void *pWndObj)
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
    pSolaSavePath = pOtdrTopSettings->sDefSavePath.cSolaSavePath;
    iUnitConverterFlag = pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag;

    //初始化状态栏上的文本
    pSolaSettingStrSolaTitle = TransString("SOLA");
	pSolaSettingStrSettingTitle = TransString("SOLA_SETTING");    
    pSolaSettingStrWaveSet = TransString("SOLA_SET_WAVELENGTH");  
    pSolaSettingStrWave[0] = TransString("1310 nm"); 
    pSolaSettingStrWave[1] = TransString("1550 nm");  
    pSolaSettingStrWave[2] = TransString("1310+1550 nm");  

    //单位转换设置
    pSolaSettingStrUnitTitle = TransString("SAMPLESET_LABEL_UNITCONVERTER");
    	
    int i;
    for(i = 0; i < MAX_UNIT_NUM; ++i)
    {
        pSolaSettingStrUnit[i] = TransString("km");
    }
    pSolaSettingStrDefaultPathTitle = TransString("SOLA_SET_DEFAULT_PATH");
    pSolaSettingStrDefaultPath = TransString(pSolaSavePath + 4);

    pSolaSettingLaunchRecvText  = TransString("ANALYZE_INJECT_AND_RECV");
    pSolaSettingLaunchFiberLblText = TransString("ANALYZE_INJECT_LEN");
    pSolaSettingRecvFiberLblText = TransString("ANALYZE_RECV_LEN");
    
    pSolaSettingLaunchFiberEditText = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_SOLA, 
                                        pUser_Settings->sSolaSetting.fLaunchFiberLen, 
                                        pUser_Settings->sSolaSetting.fLaunchFiberLen_ft,
                                        pUser_Settings->sSolaSetting.fLaunchFiberLen_mile, 0);
    pSolaSettingRecvFiberEditText = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_SOLA, 
                                        pUser_Settings->sSolaSetting.fRecvFiberLen, 
                                        pUser_Settings->sSolaSetting.fRecvFiberLen_ft,
                                        pUser_Settings->sSolaSetting.fRecvFiberLen_mile, 0);
    pSolaSettingLaunchFiberUnitText = GetCurrSystemUnitGuiString(MODULE_UNIT_SOLA);
    pSolaSettingRecvFiberUnitText = GetCurrSystemUnitGuiString(MODULE_UNIT_SOLA);
    return iReturn;
}


//释放文本资源
static int SolaSettingTextRes_Exit(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //释放状态栏上的文本
    GuiMemFree(pSolaSettingStrSolaTitle);
	GuiMemFree(pSolaSettingStrSettingTitle);
    GuiMemFree(pSolaSettingStrWaveSet);
    GuiMemFree(pSolaSettingStrWave[0]); 
    GuiMemFree(pSolaSettingStrWave[1]);
    GuiMemFree(pSolaSettingStrWave[2]);
    GuiMemFree(pSolaSettingStrUnitTitle);
    GuiMemFree(pSolaSettingStrDefaultPathTitle);
    GuiMemFree(pSolaSettingStrDefaultPath);
    
    GuiMemFree(pSolaSettingLaunchRecvText);
    GuiMemFree(pSolaSettingLaunchFiberLblText);
    GuiMemFree(pSolaSettingRecvFiberLblText);
    GuiMemFree(pSolaSettingLaunchFiberEditText);
    GuiMemFree(pSolaSettingRecvFiberEditText);
    GuiMemFree(pSolaSettingLaunchFiberUnitText);
    GuiMemFree(pSolaSettingRecvFiberUnitText);

    int i;
    for(i = 0; i < MAX_UNIT_NUM; ++i)
    {
        GuiMemFree(pSolaSettingStrUnit[i]); 
    }
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

static void SolaSettingConvertLaunchRecvLen(double in, double* out_m, double* out_ft, double* out_mile)
{
    char* threshold[UNIT_COUNT] = {"360000.0000", "360.0", "1181102.4",
                                   "1181.1024", "223.6936", "393700.8"};
    double min = 0.0f;
    double max = String2Float(threshold[GetCurrSystemUnit(MODULE_UNIT_SOLA)]);
    if(in < min || in > max)
    {
        switch(GetCurrSystemUnit(MODULE_UNIT_SOLA))
        {
            case UNIT_M:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_M_LIMIT"));
                return;
            case UNIT_KM:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_KM_LIMIT"));
                return;
            case UNIT_FT:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_FT_LIMIT"));  
                return;
            case UNIT_KFT:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_KFT_LIMIT"));
                return;
            case UNIT_YD:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_YD_LIMIT"));
                return;
            case UNIT_MI:
                CreateIMEDialog(TransString("ANALYZE_LAUNCH_MI_LIMIT"));
                return;
            default:
                break;
        }
    }
    
    *out_m = UnitConverter_Dist_System2M_Float2Float(MODULE_UNIT_SOLA, in);
    *out_ft = UnitConverter_Dist_System2FT_Float2Float(MODULE_UNIT_SOLA, in);
    *out_mile = UnitConverter_Dist_System2MI_Float2Float(MODULE_UNIT_SOLA, in);  
}

//注入光纤和接收光纤长度设置
static void SolaSettingInputLaunchLenCallBack(void)
{
	char inputBuffer[100];
	double out_m = pUser_Settings->sSolaSetting.fLaunchFiberLen;
    double out_ft = pUser_Settings->sSolaSetting.fLaunchFiberLen_ft;
    double out_mile = pUser_Settings->sSolaSetting.fLaunchFiberLen_mile;

	GetIMEInputBuff(inputBuffer);
    double launchLen = String2Float(inputBuffer);
    SolaSettingConvertLaunchRecvLen(launchLen, &out_m, &out_ft, &out_mile);
    pUser_Settings->sSolaSetting.fLaunchFiberLen = out_m;
    pUser_Settings->sSolaSetting.fLaunchFiberLen_ft = out_ft;
    pUser_Settings->sSolaSetting.fLaunchFiberLen_mile = out_mile;
}

static void SolaSettingInputRecvLenCallBack(void)
{
	char inputBuffer[100];
	double out_m = pUser_Settings->sSolaSetting.fRecvFiberLen;
    double out_ft = pUser_Settings->sSolaSetting.fRecvFiberLen_ft;
    double out_mile = pUser_Settings->sSolaSetting.fRecvFiberLen_mile;
    
	GetIMEInputBuff(inputBuffer);
    double recvLen = String2Float(inputBuffer);
    SolaSettingConvertLaunchRecvLen(recvLen, &out_m, &out_ft, &out_mile);
    pUser_Settings->sSolaSetting.fRecvFiberLen = out_m;
    pUser_Settings->sSolaSetting.fRecvFiberLen_ft = out_ft;
    pUser_Settings->sSolaSetting.fRecvFiberLen_mile = out_mile;
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

static int SolaSettingTabSola_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaSettingTabSola_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = CreateSolaWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}
 
static void SelectWaveLenBackCall (int iSelected)
{
    int index = 0;
    
    for(index = 0; index < WAVE_NUM; ++index)
    {
        pUser_Settings->sSolaSetting.Wave[index] = 0;
    }
    
    if(iSelected == 0)
        pUser_Settings->sSolaSetting.Wave[0] = 1;
    else if(iSelected == 1)
        pUser_Settings->sSolaSetting.Wave[1] = 1;
    else
    {
        pUser_Settings->sSolaSetting.Wave[0] = 1;
        pUser_Settings->sSolaSetting.Wave[1] = 1;
    }

    //clearSolaEvents(); 设置修改清空事件?
}


//创建solaSetting window
GUIWINDOW* CreateSolaSettingWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
          	                       FrmSolaSettingInit, FrmSolaSettingExit, 
          	                       FrmSolaSettingPaint, FrmSolaSettingLoop, 
          				           FrmSolaSettingPause, FrmSolaSettingResume,
          	                       NULL);
    return pWnd;
}

/* 重绘窗体函数 */
static void ReCreateSolaSettingWindow(GUIWINDOW **pWnd)
{	
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
   		                 FrmSolaSettingInit, FrmSolaSettingExit, 
   		                 FrmSolaSettingPaint, FrmSolaSettingLoop, 
   				         FrmSolaSettingPause, FrmSolaSettingResume,
   		                 NULL);
}

void setSolaSettingWavelength(int wave)
{
    enSolaWaveChoose = wave;
}

static int SolaSettingWaveSelect_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int i = (int)pOutArg;;
    int j;
    for(j = 0; j < 3; ++j)
    {
        if(j == i)
        {
            TouchChange("bg_sloasetting_select.bmp", pSolaSettingWaveSelect[j], 
                        pSolaSettingStrWave[j], pSolaSettingLblWave[j], 2);
        }
        else
        {
            TouchChange("bg_sloasetting_unselect.bmp", pSolaSettingWaveSelect[j], 
                        pSolaSettingStrWave[j], pSolaSettingLblWave[j], 1);
        }
        
    }
    RefreshScreen(__FILE__, __func__, __LINE__);
    SelectWaveLenBackCall(i);
    return 0;
}
static int SolaSettingWaveSelect_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaSettingDefaultPath_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaSettingDefaultPath_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    Stack *ps = NULL;
	
	ps = GetCurrWndStack();
	WndPush(ps, ReCreateSolaSettingWindow);
	
	CreateUserDefinedPath(pSolaSavePath, MntDataDirectory);

    return 0;
}

static int SolaSettingLaunchCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pSolaSettingLaunchFiberLblLabel);
    DisplayLabel(pSolaSettingLaunchFiberLblLabel);
    SyncCurrFbmap();
    return 0;
}
static int SolaSettingLaunchCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLaunchFiberLblLabel);
    DisplayLabel(pSolaSettingLaunchFiberLblLabel);
    
    pUser_Settings->sSolaSetting.iEnableLaunchFiber = pUser_Settings->sSolaSetting.iEnableLaunchFiber ? 0 : 1;
    SetPictureBitmap(SolaSettingCheckBoxBmp[pUser_Settings->sSolaSetting.iEnableLaunchFiber], 
        pSolaSettingLaunchFiberCheckBox);
    DisplayPicture(pSolaSettingLaunchFiberCheckBox);
    
    GUIFONT* theFont = pUser_Settings->sSolaSetting.iEnableLaunchFiber ? getGlobalFnt(EN_FONT_WHITE): getGlobalFnt(EN_FONT_GRAY);
    SetLabelFont(theFont, pSolaSettingLaunchFiberEditLabel);
    DisplayLabel(pSolaSettingLaunchFiberEditLabel);
    
    SyncCurrFbmap();
    return 0;
}

static int SolaSettingRecvCheckBox_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pSolaSettingRecvFiberLblLabel);
    DisplayLabel(pSolaSettingRecvFiberLblLabel);
    SyncCurrFbmap();
    return 0;
}
static int SolaSettingRecvCheckBox_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingRecvFiberLblLabel);
    DisplayLabel(pSolaSettingRecvFiberLblLabel);
    pUser_Settings->sSolaSetting.iEnableRecvFiber = pUser_Settings->sSolaSetting.iEnableRecvFiber ? 0 : 1;
    SetPictureBitmap(SolaSettingCheckBoxBmp[pUser_Settings->sSolaSetting.iEnableRecvFiber], 
        pSolaSettingRecvFiberCheckBox);
    DisplayPicture(pSolaSettingRecvFiberCheckBox);

    GUIFONT* theFont = pUser_Settings->sSolaSetting.iEnableRecvFiber ? getGlobalFnt(EN_FONT_WHITE) : getGlobalFnt(EN_FONT_GRAY);
    SetLabelFont(theFont, pSolaSettingRecvFiberEditLabel);
    DisplayLabel(pSolaSettingRecvFiberEditLabel);

    SyncCurrFbmap();
    return 0;
}

static int SolaSettingLaunchEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaSettingLaunchEdit_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    if (!pUser_Settings->sSolaSetting.iEnableLaunchFiber)
        return 0;
    
    char buf[24]; 
    double dist_m = pUser_Settings->sSolaSetting.fLaunchFiberLen;
    double dist_ft = pUser_Settings->sSolaSetting.fLaunchFiberLen_ft;
    double dist_mile = pUser_Settings->sSolaSetting.fLaunchFiberLen_mile;
    char* temp = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, dist_m, dist_ft, dist_mile, 0);
    sprintf(buf, "%s", temp);
    GuiMemFree(temp);
	IMEInit(buf, 10, 1, ReCreateSolaSettingWindow, SolaSettingInputLaunchLenCallBack, NULL);

    return 0;
}

static int SolaSettingRecvEdit_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaSettingRecvEdit_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    if (!pUser_Settings->sSolaSetting.iEnableRecvFiber)
        return 0;

    char buf[24]; 
    double dist_m = pUser_Settings->sSolaSetting.fRecvFiberLen;
    double dist_ft = pUser_Settings->sSolaSetting.fRecvFiberLen_ft;
    double dist_mile = pUser_Settings->sSolaSetting.fRecvFiberLen_mile;
    char* temp = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, dist_m, dist_ft, dist_mile, 0);
    sprintf(buf, "%s", temp);
    GuiMemFree(temp);
	IMEInit(buf, 10, 1, ReCreateSolaSettingWindow, SolaSettingInputRecvLenCallBack, NULL);

    return 0;
}

static int SolaSettingUnitSelect_Down(void *pInArg, int iInLen, 
                               	   void *pOutArg, int iOutLen)
{
    int i = (int)pOutArg;
    int j;
    for(j = 0; j < MAX_UNIT_NUM; ++j)
    {
        if(j == i)
        {
            TouchChange("bg_sloasetting_select.bmp", pSolaSettingUnit[j], 
                        pSolaSettingStrUnit[j], pSolaSettingLblUnit[j], 2);
        }
        else
        {
            TouchChange("bg_sloasetting_unselect.bmp", pSolaSettingUnit[j], 
                        pSolaSettingStrUnit[j], pSolaSettingLblUnit[j], 1);
        }
        
    }
    //RefreshScreen(__FILE__, __func__, __LINE__);
    SetCurrSystemUnit(MODULE_UNIT_SOLA, (UNIT)i);
    GUIWINDOW* pWnd = CreateSolaSettingWindow();
	SendWndMsg_WindowExit(pFrmSolaSetting);	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}

static int SolaSettingUnitSelect_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    return 0;
}
