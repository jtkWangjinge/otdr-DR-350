/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolasetting.c
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


#include "wnd_frmsolamessager.h"
#include "wnd_frmsolasetting.h"
#include "wnd_frmsola.h"
#include "wnd_frmsolaitempassthreshold.h"
#include "wnd_frmsolalinepassthreshold.h"
#include "wnd_frmsolalinedefine.h"
#include "wnd_frmsolaidentify.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmselector.h"
#include "wnd_frmsolaconfig.h"
#include "wnd_frmime.h"
#include "wnd_frmsolafilenamed.h"
#include "app_getsetparameter.h"
#include "app_frminit.h"
#include "guiphoto.h"
#include "app_systemsettings.h"

static GUIWINDOW *pFrmSolaSetting = NULL;
static GUIPICTURE * identifyTitlePicture =NULL;
static GUIPICTURE * lineDefineTitlePicture =NULL;
static GUIPICTURE * linePassThresholdTitlePicture =NULL;
static GUIPICTURE * itemPassThresholdTitlePicture =NULL;

static GUICHAR* pSolaSettingStrIdentifyTitle = NULL;
static GUICHAR* pSolaSettingStrLineDefineTitle = NULL;
static GUICHAR* pSolaSettingStrLinePassThresholdTitle = NULL;
static GUICHAR* pSolaSettingStrItemPassThresholdTitle = NULL;

static GUILABEL* pSolaSettingLblLineDefineTitle = NULL;
static GUILABEL* pSolaSettingLblLinePassThresholdTitle = NULL;
static GUILABEL* pSolaSettingLblItemPassThresholdTitle = NULL;
static GUILABEL* pSolaSettingLblIdentifyTitle = NULL;

static GUIPICTURE *pSolaSettingBgDesk = NULL;
static WNDMENU1* pSolaSettingMenu = NULL;

static SOLA_IDENTIFY_TABLE* pSolaIdentifyTable = NULL;

static GUILABEL* pSolaCableLabel = NULL;
static GUICHAR* pSolaCableChar = NULL;
static GUILABEL* pSolaCableTextLabel = NULL;
static GUICHAR* pSolaCableText = NULL;

static GUILABEL* pSolaFiberLabel = NULL;
static GUICHAR* pSolaFiberChar = NULL;
static GUILABEL* pSolaFiberTextLabel = NULL;
static GUICHAR* pSolaFiberText = NULL;

static GUILABEL* pSolaLocationALabel = NULL;
static GUICHAR* pSolaLocationAChar = NULL;
static GUILABEL* pSolaLocationATextLabel = NULL;
static GUICHAR* pSolaLocationAText = NULL;

static GUILABEL* pSolaLocationBLabel = NULL;
static GUICHAR* pSolaLocationBChar = NULL;
static GUILABEL* pSolaLocationBTextLabel = NULL;
static GUICHAR* pSolaLocationBText = NULL;

    
static GUIPICTURE * resetPicture =NULL;
static GUILABEL *resetLabel = NULL;
static GUICHAR* resetStr = NULL;

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

//表格的标签切换设置函数
static void LabelWriteTab(GUILABEL* startLabel,GUICHAR**  startText,
                          GUILABEL* stopLabel,GUICHAR**  stopText,
                          GUILABEL* setpLabel,GUICHAR**  setpText,
                          char* string1,char* string2,char*string3);

//表格的CableID/Auto_increment按钮
static int SolaIdentifyTable1_Down(void *pInArg, int iInLen,
                                   void *pOutArg, int iOutLen);
static int SolaIdentifyTable1_Up(void *pInArg, int iInLen,
                                 void *pOutArg, int iOutLen);
//表格的FiberID/Auto_increment按钮
static int SolaIdentifyTable2_Down(void *pInArg, int iInLen,
                                   void *pOutArg, int iOutLen);
static int SolaIdentifyTable2_Up(void *pInArg, int iInLen,
                                 void *pOutArg, int iOutLen);
//表格的LocationA/Auto_increment按钮
static int SolaIdentifyTable3_Down(void *pInArg, int iInLen,
                                   void *pOutArg, int iOutLen);
static int SolaIdentifyTable3_Up(void *pInArg, int iInLen,
                                 void *pOutArg, int iOutLen);
//表格的LocationB/Auto_increment按钮
static int SolaIdentifyTable4_Down(void *pInArg, int iInLen,
                                   void *pOutArg, int iOutLen);
static int SolaIdentifyTable4_Up(void *pInArg, int iInLen,
                                 void *pOutArg, int iOutLen);
//表格的NULL/Auto_increment按钮
//static int SolaIdentifyTable5_Down(void *pInArg, int iInLen,
//                         void *pOutArg, int iOutLen);
//static int SolaIdentifyTable5_Up(void *pInArg, int iInLen,
//                       void *pOutArg, int iOutLen);

static int SolaIdentifyTableNum_Down(void *pInArg, int iInLen,
                                     void *pOutArg, int iOutLen);
static int SolaIdentifyTableNum_Up(void *pInArg, int iInLen,
                                   void *pOutArg, int iOutLen);

static void RePskInputOk(void);
static void ReCableInputOk(void);
static void ReFiberInputOk(void);
static void ReLocationAInputOk(void);
static void ReLocationBInputOk(void);
static void ReCreateWindow(GUIWINDOW **pWnd);

/********************************
* 窗体frmsola中的错误事件处理函数
********************************/
static int SolaSettingErrProc_Func(void *pInArg, int iInLen,
                                   void *pOutArg, int iOutLen);

static int SolaIdentifyTabItemPassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen);
static int SolaIdentifyTabItemPassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);

static int SolaIdentifyTabLineDefine_Down(void *pInArg, int iInLen, 
                                          void *pOutArg, int iOutLen);
static int SolaIdentifyTabLineDefine_Up(void *pInArg, int iInLen, 
                                        void *pOutArg, int iOutLen);

static int SolaIdentifyTabLinePassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen);
static int SolaIdentifyTabLinePassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);

//static int SolaCableTextLabel_Down(void *pInArg, int iInLen, 
//                                               void *pOutArg, int iOutLen);
static int SolaCableTextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);
static int SolaFiberTextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);
static int SolaLocationATextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);
static int SolaLocationBTextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);

static void SolaSettingMenuCall(int buttonIndex);
GUIWINDOW* CreateSolaIdentifyWindow();
//跳转到自动命名界面响应函数
static int SolaFileNameJumpToSolaFilenamed_Fun();

static int Number = 0;
static int IOutLen = 0;
extern SOLA_MESSAGER* pSolaMessager;

static int resetButtonUp(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{ //错误标志、返回值定义
    int iReturn = 0;
    GUIWINDOW* pWnd = NULL;

    ResetSolaIdentifySet((void*)&pSolaMessager->identify);
    
    pWnd = CreateSolaIdentifyWindow();
    SendWndMsg_WindowExit(pFrmSolaSetting);
    SendSysMsg_ThreadCreate(pWnd);

    return  iReturn;
}
static int resetButtonDown(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    TouchChange("btn_reset_press1.bmp", resetPicture,
                NULL, resetLabel, 0);
    RefreshScreen(__FILE__, __func__, __LINE__);
    //错误标志、返回值定义
    return 0;
}

int FrmSolaIdentifyInit(void *pWndObj)
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
    pSolaSettingBgDesk = CreatePhoto("bg_sola1");

    identifyTitlePicture =CreatePhoto("otdr_top1f");
    lineDefineTitlePicture =CreatePhoto("otdr_top2");
    linePassThresholdTitlePicture =CreatePhoto("otdr_top3");
    itemPassThresholdTitlePicture =CreatePhoto("otdr_top4");
    
    pSolaSettingLblIdentifyTitle = CreateLabel(0, 22, 100, 24, pSolaSettingStrIdentifyTitle);
    pSolaSettingLblLineDefineTitle = CreateLabel(100, 22, 100, 24, pSolaSettingStrLineDefineTitle);
    pSolaSettingLblLinePassThresholdTitle = CreateLabel(200, 22, 100, 24, pSolaSettingStrLinePassThresholdTitle);
    pSolaSettingLblItemPassThresholdTitle = CreateLabel(300, 22, 100, 24, pSolaSettingStrItemPassThresholdTitle);

    resetPicture = CreatePhoto("btn_reset_unpress1");
    resetLabel = CreateLabel(536, 412, 100, 24, resetStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), resetLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, resetLabel);
    //表格
    pSolaIdentifyTable = CreateSolaIdentifyTable(38,76,681,20,pSolaMessager,pFrmSolaSetting);

    //自定义 CableID FiberID LocationA LocationB 文言 
    pSolaCableLabel = CreateLabel(32, 315, 151, 24, pSolaCableChar);
    pSolaCableTextLabel = CreateLabel(32, 351, 151, 24, pSolaCableText);

    pSolaFiberLabel = CreateLabel(187, 315, 151, 24, pSolaFiberChar);
    pSolaFiberTextLabel = CreateLabel(187, 351, 151, 24, pSolaFiberText);

    pSolaLocationALabel = CreateLabel(343, 315, 151, 24, pSolaLocationAChar);
    pSolaLocationATextLabel = CreateLabel(343, 351, 151, 24, pSolaLocationAText);

    pSolaLocationBLabel = CreateLabel(497, 315, 151, 24, pSolaLocationBChar);
    pSolaLocationBTextLabel = CreateLabel(497, 351, 151, 24, pSolaLocationBText);

    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaCableLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaCableLabel);
    
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaCableTextLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaCableTextLabel);
        
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaFiberLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaFiberLabel);
    
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaFiberTextLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaFiberTextLabel);
    
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaLocationALabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaLocationALabel);
    
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaLocationATextLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaLocationATextLabel);
    
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaLocationBLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaLocationBLabel);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaLocationBTextLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaLocationBTextLabel);

    /* 建立右侧菜单栏控件 */
    unsigned int strSolaSettingMenu[] = {
		1
	};
    pSolaSettingMenu = CreateWndMenu1(2, sizeof(strSolaSettingMenu), strSolaSettingMenu,
									(UINT16)(MENU_BACK | ~MENU_HOME), 0, 0, 40, SolaSettingMenuCall);

    //设置窗体控件的画刷、画笔及字体
    /****************************************************************/
    //设置桌面上的控件

    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaSettingLblIdentifyTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblIdentifyTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLineDefineTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLineDefineTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLinePassThresholdTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLinePassThresholdTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblItemPassThresholdTitle);
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

    // AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),pSolaSettingPictureDefaultValuesButton,
    //              pFrmSolaSetting);
    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentifyTable->auto_incrementCableIDPicture,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentifyTable->auto_incrementFiberIDPicture,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentifyTable->auto_incrementLocationAPicture,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentifyTable->auto_incrementLocationBPicture,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pSolaIdentifyTable->auto_incrementNULLPicture,
                  pFrmSolaSetting);

    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->startCableIDLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->startFiberIDLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->startLocationALabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->startLocationBLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->startNULLLabel,
                  pFrmSolaSetting);

    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->stopCableIDLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->stopFiberIDLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->stopLocationALabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->stopLocationBLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->stopNULLLabel,
                  pFrmSolaSetting);

    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->setpCableIDLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->setpFiberIDLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->setpLocationALabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->setpLocationBLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaIdentifyTable->setpNULLLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaCableTextLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaFiberTextLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaLocationATextLabel,
                  pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaLocationBTextLabel,
                  pFrmSolaSetting);

        
    //注册桌面上的控件
    //...
    //注册按钮区的控件
    AddWndMenuToComp1(pSolaSettingMenu, pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), resetPicture,pFrmSolaSetting);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), itemPassThresholdTitlePicture,pFrmSolaSetting);
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

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, resetPicture,resetButtonDown, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, resetPicture,resetButtonUp, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Up, NULL, 0, pMsg);



    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->auto_incrementCableIDPicture,
                    SolaIdentifyTable1_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->auto_incrementCableIDPicture,
                    SolaIdentifyTable1_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->auto_incrementFiberIDPicture,
                    SolaIdentifyTable2_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->auto_incrementFiberIDPicture,
                    SolaIdentifyTable2_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->auto_incrementLocationAPicture,
                    SolaIdentifyTable3_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->auto_incrementLocationAPicture,
                    SolaIdentifyTable3_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->auto_incrementLocationBPicture,
                    SolaIdentifyTable4_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->auto_incrementLocationBPicture,
                    SolaIdentifyTable4_Up, NULL, 0, pMsg);
    /*LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->auto_incrementNULLPicture,
                    SolaIdentifyTable5_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->auto_incrementNULLPicture,
                    SolaIdentifyTable5_Up, NULL, 0, pMsg);
    */
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->startCableIDLabel,
                    SolaIdentifyTableNum_Down, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->startCableIDLabel,
                    SolaIdentifyTableNum_Up, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->startFiberIDLabel,
                    SolaIdentifyTableNum_Down, NULL, 4, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->startFiberIDLabel,
                    SolaIdentifyTableNum_Up, NULL, 4, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->startLocationALabel,
                    SolaIdentifyTableNum_Down, NULL, 7, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->startLocationALabel,
                    SolaIdentifyTableNum_Up, NULL, 7, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->startLocationBLabel,
                    SolaIdentifyTableNum_Down, NULL, 10, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->startLocationBLabel,
                    SolaIdentifyTableNum_Up, NULL, 10, pMsg);
    /*  LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->startNULLLabel,
                    SolaIdentifyTableNum_Down, NULL, 5, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->startNULLLabel,
                    SolaIdentifyTableNum_Up, NULL, 5, pMsg);
*/
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->stopCableIDLabel,
                    SolaIdentifyTableNum_Down, NULL, 2, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->stopCableIDLabel,
                    SolaIdentifyTableNum_Up, NULL, 2, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->stopFiberIDLabel,
                    SolaIdentifyTableNum_Down, NULL, 5, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->stopFiberIDLabel,
                    SolaIdentifyTableNum_Up, NULL, 5, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->stopLocationALabel,
                    SolaIdentifyTableNum_Down, NULL, 8, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->stopLocationALabel,
                    SolaIdentifyTableNum_Up, NULL, 8, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->stopLocationBLabel,
                    SolaIdentifyTableNum_Down, NULL, 11, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->stopLocationBLabel,
                    SolaIdentifyTableNum_Up, NULL, 11, pMsg);
    /*  LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->stopNULLLabel,
                    SolaIdentifyTableNum_Down, NULL, 10, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->stopNULLLabel,
                    SolaIdentifyTableNum_Up, NULL, 10, pMsg);
*/
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->setpCableIDLabel,
                    SolaIdentifyTableNum_Down, NULL, 3, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->setpCableIDLabel,
                    SolaIdentifyTableNum_Up, NULL, 3, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->setpFiberIDLabel,
                    SolaIdentifyTableNum_Down, NULL, 6, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->setpFiberIDLabel,
                    SolaIdentifyTableNum_Up, NULL, 6, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->setpLocationALabel,
                    SolaIdentifyTableNum_Down, NULL, 9, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->setpLocationALabel,
                    SolaIdentifyTableNum_Up, NULL, 9, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->setpLocationBLabel,
                    SolaIdentifyTableNum_Down, NULL, 12, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->setpLocationBLabel,
                    SolaIdentifyTableNum_Up, NULL, 12, pMsg);
    /*  LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaIdentifyTable->setpNULLLabel,
                    SolaIdentifyTableNum_Down, NULL, 15, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaIdentifyTable->setpNULLLabel,
                    SolaIdentifyTableNum_Up, NULL, 15, pMsg);

*/
    //LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaCableTextLabel,
    //                SolaCableTextLabel_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaCableTextLabel,
                    SolaCableTextLabel_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaFiberTextLabel,
                    SolaFiberTextLabel_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaLocationATextLabel,
                    SolaLocationATextLabel_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaLocationBTextLabel,
                    SolaLocationBTextLabel_Up, NULL, 0, pMsg);

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
int FrmSolaIdentifyExit(void *pWndObj)
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
    DestroyPicture(&resetPicture);
    DestroyLabel(&resetLabel);
    DestroySolaIdentifyTable(pSolaIdentifyTable);
    // DestroyLabel(&pSolaSettingLblDefaultValuesButton);
    DestroyLabel(&pSolaSettingLblIdentifyTitle);
    DestroyLabel(&pSolaSettingLblLineDefineTitle);
    DestroyLabel(&pSolaSettingLblLinePassThresholdTitle);
    DestroyLabel(&pSolaSettingLblItemPassThresholdTitle);
    
    DestroyLabel(&pSolaCableLabel);
    DestroyLabel(&pSolaCableTextLabel);
    
    DestroyLabel(&pSolaFiberLabel);
    DestroyLabel(&pSolaFiberTextLabel);
    
    DestroyLabel(&pSolaLocationALabel);
    DestroyLabel(&pSolaLocationATextLabel);
    
    DestroyLabel(&pSolaLocationBLabel);
    DestroyLabel(&pSolaLocationBTextLabel);

    
    //销毁按钮区的控件
    DestroyWndMenu1(&pSolaSettingMenu);
    //销毁信息栏上的控件
    //...

    //释放文本资源
    //***************************************************************/
    SolaSettingTextRes_Exit(NULL, 0, NULL, 0);
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
int FrmSolaIdentifyPaint(void *pWndObj)
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
    
    DisplayPicture(resetPicture);
    DisplayLabel(resetLabel);
    //显示按钮区的控件
    DisplayWndMenu1(pSolaSettingMenu);
    
    SetPowerEnable(1, 1);

    DisplaySolaIdentifyTable(pSolaIdentifyTable);
    
    DisplayLabel(pSolaCableLabel);
    DisplayLabel(pSolaCableTextLabel);
    
    DisplayLabel(pSolaFiberLabel);
    DisplayLabel(pSolaFiberTextLabel);
    
    DisplayLabel(pSolaLocationALabel);
    DisplayLabel(pSolaLocationATextLabel);
    
    DisplayLabel(pSolaLocationBLabel);
    DisplayLabel(pSolaLocationBTextLabel);
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
int FrmSolaIdentifyLoop(void *pWndObj)
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
int FrmSolaIdentifyPause(void *pWndObj)
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
int FrmSolaIdentifyResume(void *pWndObj)
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
    pSolaSettingStrIdentifyTitle = TransString("OTDR_ABOUT_INFOMATION");
    pSolaSettingStrLineDefineTitle = TransString("SOLA_LINE_DEFINE");
    pSolaSettingStrLinePassThresholdTitle = TransString("SOLA_LINEPASS");
    pSolaSettingStrItemPassThresholdTitle = TransString("SOLA_ITEMPASS");

    pSolaCableChar = TransString("SOLA_MANAGE_CABLEID");   
    pSolaFiberChar = TransString("SOLA_MANAGE_FIBERID");
    pSolaLocationAChar = TransString("SOLA_MANAGE_LOCATIONA");
    pSolaLocationBChar = TransString("SOLA_MANAGE_LOCATIONB");

    solaIdentifySetting solaIdentify = pSolaMessager->identify.solaIdentify;
    LOG(LOG_INFO, "pSolaCableText = %s\n", solaIdentify.cableID);

    if (strlen(solaIdentify.cableID) == 0)
    {
        pSolaCableText = TransString("--");
    }
    else
    {
        pSolaCableText = TransString(solaIdentify.cableID);
    }
    if (strlen(solaIdentify.fiberID) == 0)
    {
        pSolaFiberText = TransString("--");
    }
    else
    {
        pSolaFiberText = TransString(solaIdentify.fiberID);
    }
    if (strlen(solaIdentify.LocationA) == 0)
    {
        pSolaLocationAText = TransString("--");
    }
    else
    {
        pSolaLocationAText = TransString(solaIdentify.LocationA);
    }
    if (strlen(solaIdentify.LocationB) == 0)
    {
        pSolaLocationBText = TransString("--");
    }
    else
    {
        pSolaLocationBText = TransString(solaIdentify.LocationB);
    }

   // pSolaCableText = TransString("---");
   // pSolaFiberText = TransString("---");
   // pSolaLocationAText = TransString("---");
   // pSolaLocationBText = TransString("---");
  
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
    GuiMemFree(pSolaCableChar);
    GuiMemFree(pSolaFiberChar);
    GuiMemFree(pSolaLocationAChar);
    GuiMemFree(pSolaLocationBChar);

    GuiMemFree(pSolaCableText);
    GuiMemFree(pSolaFiberText);
    GuiMemFree(pSolaLocationAText);
    GuiMemFree(pSolaLocationBText);
    
    GuiMemFree(resetStr);
    return iReturn;
}

//跳转到自动命名界面响应函数
static int SolaFileNameJumpToSolaFilenamed_Fun()
{
    //错误标志、返回值定义
	int iReturn = 0;
	GUIWINDOW *pWnd = NULL;
	
     pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmSolaFileNamedInit, FrmSolaFileNamedExit, 
						FrmSolaFileNamedPaint, FrmSolaFileNamedLoop, 
						FrmSolaFileNamedPause, FrmSolaFileNamedResume,
    					NULL); 		 
     SendWndMsg_WindowExit(pFrmSolaSetting);  
     SendSysMsg_ThreadCreate(pWnd); 

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
        SolaFileNameJumpToSolaFilenamed_Fun();
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
//错误事件处理函数
static int SolaSettingErrProc_Func(void *pInArg, int iInLen,
                                   void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
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

//切换到ItemPass窗体
static int SolaIdentifyTabItemPassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaIdentifyTabItemPassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    GUIWINDOW *pWnd = CreateSolaItemPassThresholdWindow();
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
/*
static int SolaCableTextLabel_Down(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    return 0;
}
*/
static int SolaCableTextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    char buf[16] = {0};
    
    memcpy(buf, pSolaMessager->identify.solaIdentify.cableID, 16);
    LOG(LOG_INFO, "SolaCableTextLabel -----> %s\n", buf);
    IMEInit(buf, 15, 0, ReCreateWindow, ReCableInputOk, NULL);
    return 0;
}
static int SolaFiberTextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    char buf[16] = {0};
    memcpy(buf, pSolaMessager->identify.solaIdentify.fiberID, 16);
    LOG(LOG_INFO, "SolaCableTextLabel -----> %s\n", buf);
    IMEInit(buf, 15, 0, ReCreateWindow, ReFiberInputOk, NULL);
    return 0;
}
static int SolaLocationATextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    char buf[16] = {0};
    memcpy(buf, pSolaMessager->identify.solaIdentify.LocationA, 16);
    LOG(LOG_INFO, "SolaCableTextLabel -----> %s\n", buf);
    IMEInit(buf, 15, 0, ReCreateWindow, ReLocationAInputOk, NULL);
    return 0;
}
static int SolaLocationBTextLabel_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen)
{
    char buf[16] = {0};
    memcpy(buf, pSolaMessager->identify.solaIdentify.LocationB, 16);
    LOG(LOG_INFO, "SolaCableTextLabel -----> %s\n", buf);
    IMEInit(buf, 15, 0, ReCreateWindow, ReLocationBInputOk, NULL);
    return 0;
}


static void LabelWriteTab(GUILABEL* startLabel,GUICHAR**  startText,
                          GUILABEL* stopLabel,GUICHAR**  stopText,
                          GUILABEL* setpLabel,GUICHAR**  setpText,
                          char* string1,char* string2,char*string3)
{
    GUIPEN *pPen;
    unsigned int uiColorBack;
    GuiMemFree(*startText);
    GuiMemFree(*stopText);
    GuiMemFree(*setpText);
    *startText= TransString(string1);
    *stopText= TransString(string2);
    *setpText= TransString(string3);

    pPen = GetCurrPen();
    uiColorBack = pPen->uiPenColor;
    pPen->uiPenColor = 0xEDEDED;
    SetLabelText(*startText,startLabel);
    DrawBlock(startLabel->Visible.Area.Start.x+5,
              startLabel->Visible.Area.Start.y,
              startLabel->Visible.Area.End.x-2,
              startLabel->Visible.Area.End.y-3);
    SetLabelText(*stopText,stopLabel);
    DrawBlock(stopLabel->Visible.Area.Start.x+5,
              stopLabel->Visible.Area.Start.y,
              stopLabel->Visible.Area.End.x-2,
              stopLabel->Visible.Area.End.y-3);
    SetLabelText(*setpText,setpLabel);
    DrawBlock(setpLabel->Visible.Area.Start.x+5,
              setpLabel->Visible.Area.Start.y,
              setpLabel->Visible.Area.End.x+1,
              setpLabel->Visible.Area.End.y-3);
    pPen->uiPenColor = uiColorBack;
    DisplayLabel(startLabel);
    DisplayLabel(stopLabel);
    DisplayLabel(setpLabel);
}


static int SolaIdentifyTable1_Down(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    return 0;
}

static int SolaIdentifyTable1_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{

    pSolaMessager->identify.cableID.auto_increment =  1 - pSolaMessager->identify.cableID.auto_increment;

    if(pSolaMessager->identify.cableID.auto_increment){
        LabelWriteTab(pSolaIdentifyTable->startCableIDLabel,&pSolaIdentifyTable->startCableIDText,
                      pSolaIdentifyTable->stopCableIDLabel,&pSolaIdentifyTable->stopCableIDText,
                      pSolaIdentifyTable->setpCableIDLabel,&pSolaIdentifyTable->setpCableIDText,
                      "1","64","1");
        SetPictureBitmap(BmpFileDirectory"btn_otdr_exit_press.bmp", pSolaIdentifyTable->auto_incrementCableIDPicture);
    }
    else{
        LabelWriteTab(pSolaIdentifyTable->startCableIDLabel,&pSolaIdentifyTable->startCableIDText,
                      pSolaIdentifyTable->stopCableIDLabel,&pSolaIdentifyTable->stopCableIDText,
                      pSolaIdentifyTable->setpCableIDLabel,&pSolaIdentifyTable->setpCableIDText,
                      "--","--","--");
        SetPictureBitmap(BmpFileDirectory"btn_key_press.bmp", pSolaIdentifyTable->auto_incrementCableIDPicture);
    }        

    DisplayPicture(pSolaIdentifyTable->auto_incrementCableIDPicture);
    SyncCurrFbmap();

    return 0;
}

static int SolaIdentifyTable2_Down(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    
    return 0;
}
static int SolaIdentifyTable2_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    pSolaMessager->identify.fiberID.auto_increment = 1 - pSolaMessager->identify.fiberID.auto_increment;
    if(pSolaMessager->identify.fiberID.auto_increment){
        LabelWriteTab(pSolaIdentifyTable->startFiberIDLabel,&pSolaIdentifyTable->startFiberIDText,
                      pSolaIdentifyTable->stopFiberIDLabel,&pSolaIdentifyTable->stopFiberIDText,
                      pSolaIdentifyTable->setpFiberIDLabel,&pSolaIdentifyTable->setpFiberIDText,
                      "1","999","1");
        SetPictureBitmap(BmpFileDirectory"btn_otdr_exit_press.bmp", pSolaIdentifyTable->auto_incrementFiberIDPicture);
    }
    else{
        LabelWriteTab(pSolaIdentifyTable->startFiberIDLabel,&pSolaIdentifyTable->startFiberIDText,
                      pSolaIdentifyTable->stopFiberIDLabel,&pSolaIdentifyTable->stopFiberIDText,
                      pSolaIdentifyTable->setpFiberIDLabel,&pSolaIdentifyTable->setpFiberIDText,
                      "--","--","--");
        SetPictureBitmap(BmpFileDirectory"btn_key_press.bmp", pSolaIdentifyTable->auto_incrementFiberIDPicture);
    }
    DisplayPicture(pSolaIdentifyTable->auto_incrementFiberIDPicture);
    SyncCurrFbmap();
    return 0;
}

static int SolaIdentifyTable3_Down(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    
    return 0;
}
static int SolaIdentifyTable3_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    pSolaMessager->identify.locationA.auto_increment = 1 - pSolaMessager->identify.locationA.auto_increment;
    if(pSolaMessager->identify.locationA.auto_increment){
        LabelWriteTab(pSolaIdentifyTable->startLocationALabel,&pSolaIdentifyTable->startLocationAText,
                      pSolaIdentifyTable->stopLocationALabel,&pSolaIdentifyTable->stopLocationAText,
                      pSolaIdentifyTable->setpLocationALabel,&pSolaIdentifyTable->setpLocationAText,
                      "1","64","1");
        SetPictureBitmap(BmpFileDirectory"btn_otdr_exit_press.bmp", pSolaIdentifyTable->auto_incrementLocationAPicture);
    }
    else{
        LabelWriteTab(pSolaIdentifyTable->startLocationALabel,&pSolaIdentifyTable->startLocationAText,
                      pSolaIdentifyTable->stopLocationALabel,&pSolaIdentifyTable->stopLocationAText,
                      pSolaIdentifyTable->setpLocationALabel,&pSolaIdentifyTable->setpLocationAText,
                      "--","--","--");
        SetPictureBitmap(BmpFileDirectory"btn_key_press.bmp", pSolaIdentifyTable->auto_incrementLocationAPicture);
    }
    DisplayPicture(pSolaIdentifyTable->auto_incrementLocationAPicture);
    SyncCurrFbmap();
    return 0;
}

static int SolaIdentifyTable4_Down(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    
    return 0;
}
static int SolaIdentifyTable4_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    pSolaMessager->identify.locationB.auto_increment = 1 - pSolaMessager->identify.locationB.auto_increment;
    if(pSolaMessager->identify.locationB.auto_increment){
        LabelWriteTab(pSolaIdentifyTable->startLocationBLabel,&pSolaIdentifyTable->startLocationBText,
                      pSolaIdentifyTable->stopLocationBLabel,&pSolaIdentifyTable->stopLocationBText,
                      pSolaIdentifyTable->setpLocationBLabel,&pSolaIdentifyTable->setpLocationBText,
                      "1","64","1");
        SetPictureBitmap(BmpFileDirectory"btn_otdr_exit_press.bmp", pSolaIdentifyTable->auto_incrementLocationBPicture);
    }
    else{
        LabelWriteTab(pSolaIdentifyTable->startLocationBLabel,&pSolaIdentifyTable->startLocationBText,
                      pSolaIdentifyTable->stopLocationBLabel,&pSolaIdentifyTable->stopLocationBText,
                      pSolaIdentifyTable->setpLocationBLabel,&pSolaIdentifyTable->setpLocationBText,
                      "--","--","--");
        SetPictureBitmap(BmpFileDirectory"btn_key_press.bmp", pSolaIdentifyTable->auto_incrementLocationBPicture);
    }
    DisplayPicture(pSolaIdentifyTable->auto_incrementLocationBPicture);
    SyncCurrFbmap();
    return 0;
}

/*static int SolaIdentifyTable5_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{

    return 0;
}
static int SolaIdentifyTable5_Up(void *pInArg, int iInLen, 
                          void *pOutArg, int iOutLen)
{
    if(count5){
        count5=0;
        SetPictureBitmap(BmpFileDirectory"btn_otdr_exit_press.bmp", pSolaIdentifyTable->auto_incrementNULLPicture);
        DisplayPicture(pSolaIdentifyTable->auto_incrementNULLPicture);
        SyncCurrFbmap();
        }
    else{
        count5=1;
        SetPictureBitmap(BmpFileDirectory"btn_key_press.bmp", pSolaIdentifyTable->auto_incrementNULLPicture);
        DisplayPicture(pSolaIdentifyTable->auto_incrementNULLPicture);
        SyncCurrFbmap();
        }
    return 0;
}
*/
static int SolaIdentifyTableNum_Down(void *pInArg, int iInLen, 
                                     void *pOutArg, int iOutLen)
{
    return 0;
}
static int SolaIdentifyTableNum_Up(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
    char buf[24] = {0};
    IOutLen = iOutLen;
    switch(IOutLen)
    {
      case 1: sprintf(buf, "%d", pSolaMessager->identify.cableID.start);break;      
      case 2: sprintf(buf, "%d", pSolaMessager->identify.cableID.stop);break;
      case 3: sprintf(buf, "%d", pSolaMessager->identify.cableID.setp);break;
      case 4: sprintf(buf, "%d", pSolaMessager->identify.fiberID.start);break;
      case 5: sprintf(buf, "%d", pSolaMessager->identify.fiberID.stop);break;
      case 6: sprintf(buf, "%d", pSolaMessager->identify.fiberID.setp);break;
      case 7: sprintf(buf, "%d", pSolaMessager->identify.locationA.start);break;
      case 8: sprintf(buf, "%d", pSolaMessager->identify.locationA.stop);break;
      case 9: sprintf(buf, "%d", pSolaMessager->identify.locationA.setp);break;
      case 10: sprintf(buf, "%d", pSolaMessager->identify.locationB.start);break;
      case 11: sprintf(buf, "%d", pSolaMessager->identify.locationB.stop);break;
      case 12: sprintf(buf, "%d", pSolaMessager->identify.locationB.setp);break;
      case 13:break;
      case 14:break;
      case 15:break;
      default:break;
    }
    switch(IOutLen)
    {
      case 1:       
      case 2: 
      case 3: 
             if(pSolaMessager->identify.cableID.auto_increment){
                        IMEInit(buf, 3, 1, ReCreateWindow, RePskInputOk, NULL);
                    }
             break;
      case 4: 
      case 5: 
      case 6:
             if(pSolaMessager->identify.fiberID.auto_increment){
                        IMEInit(buf, 3, 1, ReCreateWindow, RePskInputOk, NULL);
                    }
             break;
      case 7: 
      case 8: 
      case 9:
            if(pSolaMessager->identify.locationA.auto_increment){
                        IMEInit(buf, 3, 1, ReCreateWindow, RePskInputOk, NULL);
                    }
             break;
      case 10: 
      case 11: 
      case 12:
            if(pSolaMessager->identify.locationB.auto_increment){
                        IMEInit(buf, 3, 1, ReCreateWindow, RePskInputOk, NULL);
                    }
            break;
      case 13:break;
      case 14:break;
      case 15:break;
      default:break;
    }
    return 0;
}

static void ReCreateWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,
                         FrmSolaIdentifyInit, FrmSolaIdentifyExit,
                         FrmSolaIdentifyPaint, FrmSolaIdentifyLoop,
                         FrmSolaIdentifyPause, FrmSolaIdentifyResume,
                         NULL);
}

static void RePskInputOk(void)
{
    char buff[512] = {0};
    GetIMEInputBuff(buff);

    int num = atoi(buff);
    Number = num;
    
    switch(IOutLen)
    {
         case 1: {
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number >= pSolaMessager->identify.cableID.stop)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.cableID.setp > (abs(pSolaMessager->identify.cableID.stop - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.cableID.start = Number;
                    }
                    break;
                 }
         case 2: {
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number <= pSolaMessager->identify.cableID.start)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.cableID.setp > (abs(pSolaMessager->identify.cableID.start - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.cableID.stop = Number;
                    }
                    break;
                 }
         case 3: {
                    if(Number > (abs(pSolaMessager->identify.cableID.start - pSolaMessager->identify.cableID.stop)))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_STEP_ZERO"));
                    }
                    else
                    {
                        pSolaMessager->identify.cableID.setp = Number;
                    }
                    break;
                 }
         case 4: {
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number >= pSolaMessager->identify.fiberID.stop)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.fiberID.setp > (abs(pSolaMessager->identify.fiberID.stop - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.fiberID.start = Number;
                    }
                    break;
                 }
         case 5:{
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number <= pSolaMessager->identify.fiberID.start)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.fiberID.setp > (abs(pSolaMessager->identify.fiberID.start - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.fiberID.stop = Number;
                    }
                    break;
                 }
         case 6: {
                    if(Number > (abs(pSolaMessager->identify.fiberID.start - pSolaMessager->identify.fiberID.stop)))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_STEP_ZERO"));
                    }
                    else
                    {
                        pSolaMessager->identify.fiberID.setp = Number;
                    }
                    break;
                 }
         case 7: {
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number >= pSolaMessager->identify.locationA.stop)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.locationA.setp > (abs(pSolaMessager->identify.locationA.stop - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.locationA.start = Number;
                    }
                    break;
                 }
         case 8: {
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number <= pSolaMessager->identify.locationA.start)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.locationA.setp > (abs(pSolaMessager->identify.locationA.start - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.locationA.stop = Number;
                    }
                    break;
                 }
         case 9: {
                    if(Number > (abs(pSolaMessager->identify.locationA.start - pSolaMessager->identify.locationA.stop)))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                        break;
                    }
                    else if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_STEP_ZERO"));
                    }
                    else
                    {
                        pSolaMessager->identify.locationA.setp = Number;
                    }
                    break;
                 }
         case 10:{ 
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number >= pSolaMessager->identify.locationB.stop)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.locationB.setp > (abs(pSolaMessager->identify.locationB.stop - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.locationB.start = Number;
                    }
                    break;
                }
         case 11: {
                    if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_GREATER_THAN_ZERO"));
                        break;
                    }
                    else if(Number <= pSolaMessager->identify.locationB.start)
                    {
                        CreateIMEDialog(TransString("STARTVALUE_UNEQUAL_ENDVALUE"));
                    }
                    else if(pSolaMessager->identify.locationB.setp > (abs(pSolaMessager->identify.locationB.start - Number )))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else
                    {
                        pSolaMessager->identify.locationB.stop = Number;
                    }
                    break;
                 }
         case 12: {
                    if(Number > (abs(pSolaMessager->identify.locationB.start - pSolaMessager->identify.locationB.stop)))
                    {
                        CreateIMEDialog(TransString("STEPSIZE_BETWEEN"));
                    }
                    else if(Number <= 0)
                    {
                        CreateIMEDialog(TransString("SOLA_IDENTIFY_STEP_ZERO"));
                    }
                    else
                    {
                        pSolaMessager->identify.locationB.setp = Number;
                    }
                    break;
                 }
         case 13:break;
         case 14:break;
         case 15:break;
         default:break;
    }
}

static void ReCableInputOk(void)
{
    char buff[16] = {0};
    char *illegal = "/\\:*?<>|\"";
    int i = 0;
    GetIMEInputBuff(buff);
    LOG(LOG_INFO, "buff is %s\n", buff);

    while(i < strlen(illegal))
    {
        if(strrchr(buff, illegal[i]))
        {
            CreateIMEDialog(TransString("SOLA_IDENTIFY_ILLEGAL_CHAR"));
            return;
        }
        i++;
    }
    memset(pSolaMessager->identify.solaIdentify.cableID, 0, 16);
    memcpy(pSolaMessager->identify.solaIdentify.cableID, buff, 16);
    pSolaCableText = TransString(buff);
    SetLabelText(pSolaCableText, pSolaCableTextLabel);
    DisplayLabel(pSolaCableTextLabel);
    SyncCurrFbmap();
}
static void ReFiberInputOk(void)
{
    char buff[16] = {0};    
    char *illegal = "/\\:*?<>|\"";
    int i = 0;
    GetIMEInputBuff(buff);
    LOG(LOG_INFO, "buff is %s\n", buff);

    while(i < strlen(illegal))
    {
        if(strrchr(buff, illegal[i]))
        {
            CreateIMEDialog(TransString("SOLA_IDENTIFY_ILLEGAL_CHAR"));
            return;
        }
        i++;
    }
    memset(pSolaMessager->identify.solaIdentify.fiberID, 0, 16);
    memcpy(pSolaMessager->identify.solaIdentify.fiberID, buff, 16);
    pSolaFiberText = TransString(buff);
    SetLabelText(pSolaFiberText, pSolaFiberTextLabel);
}
static void ReLocationAInputOk(void)
{
    char buff[16] = {0};
    char *illegal = "/\\:*?<>|\"";
    int i = 0;
    GetIMEInputBuff(buff);
    LOG(LOG_INFO, "buff is %s\n", buff);

    while(i < strlen(illegal))
    {
        if(strrchr(buff, illegal[i]))
        {
            CreateIMEDialog(TransString("SOLA_IDENTIFY_ILLEGAL_CHAR"));
            return;
        }
        i++;
    }
    memset(pSolaMessager->identify.solaIdentify.LocationA, 0, 16);
    memcpy(pSolaMessager->identify.solaIdentify.LocationA, buff, 16);
    pSolaLocationAText = TransString(buff);
    SetLabelText(pSolaLocationAText, pSolaLocationATextLabel);
}
static void ReLocationBInputOk(void)
{
    char buff[16] = {0};
    char *illegal = "/\\:*?<>|\"";
    int i = 0;
    GetIMEInputBuff(buff);
    LOG(LOG_INFO, "buff is %s\n", buff);

    while(i < strlen(illegal))
    {
        if(strrchr(buff, illegal[i]))
        {
            CreateIMEDialog(TransString("SOLA_IDENTIFY_ILLEGAL_CHAR"));
            return;
        }
        i++;
    }
    memset(pSolaMessager->identify.solaIdentify.LocationB, 0, 16);
    memcpy(pSolaMessager->identify.solaIdentify.LocationB, buff, 16);
    pSolaLocationBText = TransString(buff);
    SetLabelText(pSolaLocationBText, pSolaLocationBTextLabel);
}


//创建solaIdentify window
GUIWINDOW* CreateSolaIdentifyWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   FrmSolaIdentifyInit, FrmSolaIdentifyExit,
                                   FrmSolaIdentifyPaint, FrmSolaIdentifyLoop,
                                   FrmSolaIdentifyPause, FrmSolaIdentifyResume,
                                   NULL);
    return pWnd;
}



