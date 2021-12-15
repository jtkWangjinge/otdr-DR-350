#include "wnd_frmsolalinedefine.h"
#include "wnd_frmselector.h"
#include "wnd_frmime.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmsola.h"
#include "wnd_frmsolaidentify.h"
#include "wnd_frmsolalinepassthreshold.h"
#include "wnd_frmsolaitempassthreshold.h"
#include "wnd_frmsolamessager.h"
#include "wnd_frmdialog.h"
#include "app_frminit.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "app_systemsettings.h"
#include "wnd_frmsolaident.h"
#include "app_unitconverter.h"

#define  AXIS  50
#define  Y_AXIS 50
#define  inputCoefficeentMin  1.000000f
#define  inputCoefficeentMax  2.000000f
#define  inputBackscatteringMin  -91.87f
#define  inputBackscatteringMax  -71.87f

extern SOLA_MESSAGER* pSolaMessager ;
static GUICHAR* pSolaSettingStrIdentifyTitle = NULL;
static GUILABEL* pSolaSettingLblIdentifyTitle = NULL;

static GUICHAR* pSolaSettingStrLineDefineTitle = NULL;
static GUICHAR* pSolaSettingStrLinePassThresholdTitle = NULL;
static GUICHAR* pSolaSettingStrItemPassThresholdTitle = NULL;

static GUILABEL* pSolaSettingLblLineDefineTitle = NULL;
static GUILABEL* pSolaSettingLblLinePassThresholdTitle = NULL;
static GUILABEL* pSolaSettingLblItemPassThresholdTitle = NULL;


static int SolaLineDefineTabIdentify_Down(void *pInArg, int iInLen, 
                                          void *pOutArg, int iOutLen);
static int SolaLineDefineTabIdentify_Up(void *pInArg, int iInLen, 
                                        void *pOutArg, int iOutLen);

static int SolaIdentifyTabLinePassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen);
static int SolaIdentifyTabLinePassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);

static int SolaIdentifyTabItemPassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen);
static int SolaIdentifyTabItemPassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);


static GUIWINDOW *pFrmMain = NULL;
//桌面上的控件
static GUIPICTURE * desk =NULL;
static GUIPICTURE * inputOne =NULL;
static GUIPICTURE * inputTwo =NULL;
static GUIPICTURE * reminderDialogPicture =NULL;
static GUIPICTURE * resetPicture =NULL;
static GUIPICTURE * identifyTitlePicture =NULL;
static GUIPICTURE * lineDefineTitlePicture =NULL;
static GUIPICTURE * linePassThresholdTitlePicture =NULL;
static GUIPICTURE * itemPassThresholdTitlePicture =NULL;

static GUIPICTURE* splitting1RatioPictureFirst[8] = {NULL};
static GUIPICTURE* splitting2RatioPictureFirst[8] = {NULL};
static GUIPICTURE* splitting3RatioPictureFirst[8] = {NULL};
static GUILABEL* splittingRatioLabel = NULL;
static GUICHAR *splittingRatioStr = NULL;

static GUICHAR *splittingFirstStr = NULL;
static GUICHAR *splittingSecondStr = NULL;
static GUICHAR *splittingThirdStr = NULL;
static GUILABEL* splittingFirstLabel = NULL;
static GUILABEL* splittingSecondLabel = NULL;
static GUILABEL* splittingThirdLabel = NULL;

static GUILABEL *dbLabel = NULL;
static  GUILABEL *resetLabel = NULL;
static  GUILABEL *coefficentLabel = NULL;
static  GUILABEL *backscatteringLabel = NULL;
static  GUILABEL *inputCoefficentLabel = NULL;
static  GUILABEL *inputBackscatteringLabel = NULL;
static  GUILABEL *reminderDialogLabel = NULL;

static  GUICHAR* pSolaSettingStrWave[8] = {NULL};
static  GUILABEL *pSolaSetting1LblWave[8] = {NULL};
static  GUILABEL *pSolaSetting2LblWave[8] = {NULL};
static  GUILABEL *pSolaSetting3LblWave[8] = {NULL};
static  GUICHAR* dbStr = NULL;
static  GUICHAR* resetStr = NULL;

static WNDMENU1* pSolaSettingMenu = NULL;

static GUICHAR *coefficeentStr = NULL;
static GUICHAR *backscatteringStr = NULL;
static GUICHAR *inputCoefficeentStr = NULL;
static GUICHAR *inputBackscatteringStr = NULL;
static GUICHAR *reminderDialogStr = NULL;

/*********************************
*声明wnd_frmcommonset.c所引用的外部变量
*********************************/

static void initSelector()
{
    if(NULL != inputCoefficeentStr)
    {
        GuiMemFree(inputCoefficeentStr);
    }
    
    inputCoefficeentStr = Float2GuiString(IOR_PRECISION, pSolaMessager->lineDefine.groupIndex1550);
    SetLabelText (inputCoefficeentStr, inputCoefficentLabel);

    if(NULL != inputBackscatteringStr)
    {
        GuiMemFree(inputBackscatteringStr);
    }
    
    inputBackscatteringStr = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, pSolaMessager->lineDefine.backScatter1550);
    SetLabelText (inputBackscatteringStr, inputBackscatteringLabel);
}
static  void linkDefineTextRes_Init()
{

    pSolaSettingStrIdentifyTitle = TransString("SOLA_IDENTITY");
    pSolaSettingStrLineDefineTitle = TransString("SOLA_LINE_DEFINE");
    pSolaSettingStrLinePassThresholdTitle = TransString("SOLA_LINEPASS");
    pSolaSettingStrItemPassThresholdTitle = TransString("SOLA_ITEMPASS");
    pSolaSettingStrWave[0] = TransString("SOLA_NONE");
    pSolaSettingStrWave[1] = TransString("1:2");
    pSolaSettingStrWave[2] = TransString("1:4");
    pSolaSettingStrWave[3] = TransString("1:8");
    pSolaSettingStrWave[4] = TransString("1:16");
    pSolaSettingStrWave[5] = TransString("1:32");
    pSolaSettingStrWave[6] = TransString("1:64");
    pSolaSettingStrWave[7] = TransString("1:128");
    coefficeentStr = TransString("SOLA_COEFFICIENT");
    backscatteringStr = TransString("SOLA_BACKSCATTERING");

    splittingRatioStr = TransString("SOLA_SPLITTING_RATIO");  
    splittingFirstStr = TransString("SOLA_SPLITTING_1");
    splittingSecondStr = TransString("SOLA_SPLITTING_2");
    splittingThirdStr = TransString("SOLA_SPLITTING_3");
    dbStr = TransString("dB");
    resetStr = TransString("SOLA_RESET");
    initSelector();
    reminderDialogStr = TransString("SOLA_REMINDER_DIALOG");
}
static  void tlinkDefineTextRes_Exit()
{
    GuiMemFree(pSolaSettingStrIdentifyTitle);
    GuiMemFree(pSolaSettingStrLineDefineTitle);
    GuiMemFree(pSolaSettingStrLinePassThresholdTitle);
    GuiMemFree(pSolaSettingStrItemPassThresholdTitle);
    GuiMemFree(pSolaSettingStrWave[0]);
    GuiMemFree(pSolaSettingStrWave[1]);
    GuiMemFree(pSolaSettingStrWave[2]);
    GuiMemFree(pSolaSettingStrWave[3]);
    GuiMemFree(pSolaSettingStrWave[4]);
    GuiMemFree(pSolaSettingStrWave[5]);
    GuiMemFree(pSolaSettingStrWave[6]);
    GuiMemFree(pSolaSettingStrWave[7]);
    GuiMemFree(splittingRatioStr);
    GuiMemFree(splittingFirstStr);
    GuiMemFree(splittingSecondStr);
    GuiMemFree(splittingThirdStr);
    GuiMemFree(dbStr);
    GuiMemFree(resetStr);

    GuiMemFree(coefficeentStr);
    GuiMemFree(backscatteringStr);
    GuiMemFree(reminderDialogStr);
}

static void ReCreateWindow(GUIWINDOW **pWnd)
{
    * pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                          linkDefineFrmInit , linkDefineFrmMainExit,
                          linkDefineFrmMainPaint, linkDefineFrmMainLoop,
                          linkDefineFrmMainPause, linkDefineFrmMainResume,
                          NULL);          //pWnd由调度线程释放

}

static int splitting1Picture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iSelected = iOutLen;
    int i = 0;

    pSolaMessager->lineDefine.splittingRatioLevel1 = iSelected;
    if(iSelected == 0){
        pSolaMessager->lineDefine.splittingRatioLevel2 = 0;
        pSolaMessager->lineDefine.splittingRatioLevel3 = 0;
    }
    
   	for (i = 0; i < 8; i++)
	{
		if(i == iSelected){
			SetPictureBitmap(BmpFileDirectory"splitting1Push.bmp", splitting1RatioPictureFirst[i]);
			DisplayPicture(splitting1RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pSolaSetting1LblWave[i]);
            DisplayLabel(pSolaSetting1LblWave[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"splitting1.bmp", splitting1RatioPictureFirst[i]);
			DisplayPicture(splitting1RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting1LblWave[i]);
            DisplayLabel(pSolaSetting1LblWave[i]);
		}
	}

    LOG(LOG_INFO, "splittingPicture_Down--iSelected = %d\n", iSelected);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int splitting1Picture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int splitting2Picture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iSelected = iOutLen;
    int i = 0;

    pSolaMessager->lineDefine.splittingRatioLevel2 = iSelected;
    if(iSelected == 0){
        pSolaMessager->lineDefine.splittingRatioLevel3 = 0;
    }
    
   	for (i = 0; i < 8; i++)
	{
		if(i == iSelected){
			SetPictureBitmap(BmpFileDirectory"splitting2Push.bmp", splitting2RatioPictureFirst[i]);
			DisplayPicture(splitting2RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pSolaSetting2LblWave[i]);
            DisplayLabel(pSolaSetting2LblWave[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"splitting2.bmp", splitting2RatioPictureFirst[i]);
			DisplayPicture(splitting2RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting2LblWave[i]);
            DisplayLabel(pSolaSetting2LblWave[i]);
		}
	}

    LOG(LOG_INFO, "splitting2Picture_Down--iSelected = %d\n", iSelected);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

static int splitting2Picture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}

static int splitting3Picture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iSelected = iOutLen;
    int i = 0;

    pSolaMessager->lineDefine.splittingRatioLevel3 = iSelected;

   	for (i = 0; i < 8; i++)
	{
		if(i == iSelected){
			SetPictureBitmap(BmpFileDirectory"splitting3Push.bmp", splitting3RatioPictureFirst[i]);
			DisplayPicture(splitting3RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pSolaSetting3LblWave[i]);
            DisplayLabel(pSolaSetting3LblWave[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"splitting3.bmp", splitting3RatioPictureFirst[i]);
			DisplayPicture(splitting3RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting3LblWave[i]);
            DisplayLabel(pSolaSetting3LblWave[i]);
		}
	}

    LOG(LOG_INFO, "splitting3Picture_Down--iSelected = %d\n", iSelected);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int splitting3Picture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
static void inputCoefficient(void)
{
    char buff[64];
    GetIMEInputBuff(buff);
    double Tmp = String2Float(buff);

    if (Tmp < inputCoefficeentMin || Tmp > inputCoefficeentMax)
    {
        CreateIMEDialog(TransString("BETWEEN_1_TO_2"));

        return;
    }
    pSolaMessager->lineDefine.groupIndex1550 = Tmp;


}

static void inputBackscattering(void)
{
    char buff[64];
    GetIMEInputBuff(buff);

    double Tmp = String2Float(buff);

    if (Tmp < inputBackscatteringMin  || Tmp > inputBackscatteringMax)
    {
        CreateIMEDialog(TransString("BETWEEN_NEGAVIVE91_TO_NEGATIVE71"));
        return;
    }
    pSolaMessager->lineDefine.backScatter1550 = Tmp;

}

static int coefficentKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    char buff[32];
    char* pGroupIndex1550 = Float2String(IOR_PRECISION, pSolaMessager->lineDefine.groupIndex1550);
    sprintf(buff, "%s", pGroupIndex1550);
    GuiMemFree(pGroupIndex1550);
    IMEInit(buff, 20, 1, ReCreateWindow, inputCoefficient, NULL);
    return 0;
}

//链路损耗输入最小值
static int backscatteringKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    char buff[32];
    sprintf (buff,"%5.2f",pSolaMessager->lineDefine.backScatter1550);
    IMEInit(buff, 20, 1, ReCreateWindow, inputBackscattering, NULL);
    return 0;
}

static int reminderDialog_Down(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    int iSelected = pSolaMessager->lineDefine.isReminderDialog;
    iSelected = iSelected ? 0 : 1;
    iSelected ? SetPictureBitmap(BmpFileDirectory"fiber_mask_select.bmp", reminderDialogPicture)
              : SetPictureBitmap(BmpFileDirectory"fiber_mask_unpress.bmp", reminderDialogPicture);
    pSolaMessager->lineDefine.isReminderDialog = iSelected;
    DisplayPicture(reminderDialogPicture);
    DisplayLabel(reminderDialogLabel);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

static int reminderDialog_Up(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    return 0;
}

//错误事件处理函数
static int MainErrProc_Func(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;

    //根据实际情况，进行错误处理

    return iReturn;
}
//右侧菜单文本
static unsigned int strSolaSettingMenu[] = {

};
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
        SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
    }
        break;
    default:
        break;
    }
}
//窗体的按键按下事件处理函数
static int resetButtonUp(void *pInArg, int iInLen,
                           void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    GUIWINDOW *pWnd = NULL;
    
    ResetSolaLineDefineSet((void*)&pSolaMessager->lineDefine);
    ReCreateWindow(&pWnd);
    SendWndMsg_WindowExit(GetCurrWindow());	    //发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
    return iReturn;
}
static int resetButtonDown(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    TouchChange("btn_reset_press2.bmp", resetPicture,
                NULL, resetLabel, 0);
    RefreshScreen(__FILE__, __func__, __LINE__);
    //错误标志、返回值定义
    return 0;
}
int linkDefineFrmInit(void *pWndObj)
{

    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
    
    linkDefineTextRes_Init();

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;

    //得到当前窗体对象
    pFrmMain = (GUIWINDOW *) pWndObj;


    //建立窗体控件
    /****************************************************************/
    desk = CreatePhoto("bg_sola2");

    pSolaSettingMenu = CreateWndMenu1(0, sizeof(strSolaSettingMenu), strSolaSettingMenu,
                                      (UINT16)(MENU_BACK | ~MENU_HOME),
                                      -1, 0, 40, SolaSettingMenuCall);
    splittingRatioLabel= CreateLabel(40,70,500,24,splittingRatioStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLUE), splittingRatioLabel);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, splittingRatioLabel);

    splittingFirstLabel= CreateLabel(73,110,100,24,splittingFirstStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), splittingFirstLabel);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, splittingFirstLabel);
    splittingSecondLabel= CreateLabel(200,110,100,24,splittingSecondStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), splittingSecondLabel);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, splittingSecondLabel);
    splittingThirdLabel= CreateLabel(320,110,100,24,splittingThirdStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), splittingThirdLabel);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, splittingThirdLabel);

    for(i = 0; i < 8; i++)
    {
        pSolaSetting1LblWave[i] = CreateLabel(115,145+35*i,70,24,pSolaSettingStrWave[i]);
        pSolaSetting2LblWave[i] = CreateLabel(240,145+35*i,70,24,pSolaSettingStrWave[i]);
        pSolaSetting3LblWave[i] = CreateLabel(365,145+35*i,70,24,pSolaSettingStrWave[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting1LblWave[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaSetting1LblWave[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting2LblWave[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaSetting2LblWave[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting3LblWave[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, pSolaSetting3LblWave[i]);
    }
    
    coefficentLabel = CreateLabel(480, 115, 180, 24, coefficeentStr);
    backscatteringLabel = CreateLabel(480, 190, 200, 24, backscatteringStr);
    dbLabel = CreateLabel(AXIS+586, Y_AXIS+169, 80, 24, dbStr);
    
    CreateLoopPhoto(splitting1RatioPictureFirst,"splitting1",0,0,8);
    CreateLoopPhoto(splitting2RatioPictureFirst,"splitting2",0,0,8);
    CreateLoopPhoto(splitting3RatioPictureFirst,"splitting3",0,0,8);
    inputOne = CreatePhoto("otdr_input1");
    inputCoefficentLabel = CreateLabel(AXIS+434, Y_AXIS+94, 100, 30, inputCoefficeentStr);
    inputTwo = CreatePhoto("otdr_input2");
    inputBackscatteringLabel = CreateLabel(AXIS+434, Y_AXIS+170, 100, 30, inputBackscatteringStr);

    //弹出提示框的开关
    reminderDialogPicture = CreatePicture(482, 220+42, 25, 25, BmpFileDirectory"fiber_mask_unpress.bmp");
    reminderDialogLabel = CreateLabel(520, 224+42, 150, 25, reminderDialogStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), reminderDialogLabel);

    resetPicture = CreatePhoto("btn_reset_unpress2");
    resetLabel = CreateLabel(479, 378, 136, 24, resetStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), resetLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, resetLabel);

    identifyTitlePicture = CreatePhoto("otdr_top1");
    lineDefineTitlePicture = CreatePhoto("otdr_top2f");
    linePassThresholdTitlePicture = CreatePhoto("otdr_top3");
    itemPassThresholdTitlePicture = CreatePhoto("otdr_top4");

    pSolaSettingLblIdentifyTitle = CreateLabel(0, 22, 100, 24, pSolaSettingStrIdentifyTitle);
    pSolaSettingLblLineDefineTitle = CreateLabel(100, 22, 100, 24, pSolaSettingStrLineDefineTitle);
    pSolaSettingLblLinePassThresholdTitle = CreateLabel(200, 22, 100, 24, pSolaSettingStrLinePassThresholdTitle);
    pSolaSettingLblItemPassThresholdTitle = CreateLabel(300, 22, 100, 24, pSolaSettingStrItemPassThresholdTitle);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblIdentifyTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblIdentifyTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaSettingLblLineDefineTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLineDefineTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLinePassThresholdTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLinePassThresholdTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblItemPassThresholdTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblItemPassThresholdTitle);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), coefficentLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), backscatteringLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), dbLabel);

    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmMain,pFrmMain);
    AddWndMenuToComp1(pSolaSettingMenu, pFrmMain);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), resetPicture,pFrmMain);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), identifyTitlePicture,pFrmMain);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), linePassThresholdTitlePicture,pFrmMain);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), itemPassThresholdTitlePicture,pFrmMain);

    for(i = 0; i < 8; i++)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),splitting1RatioPictureFirst[i],pFrmMain);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),splitting2RatioPictureFirst[i],pFrmMain);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),splitting3RatioPictureFirst[i],pFrmMain);

        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSetting1LblWave[i],pFrmMain);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSetting2LblWave[i],pFrmMain);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), pSolaSetting3LblWave[i],pFrmMain);
    }
    
    AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), inputOne, pFrmMain);
    AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), inputTwo, pFrmMain);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), reminderDialogPicture, pFrmMain);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), reminderDialogLabel, pFrmMain);

    pMsg = GetCurrMessage();
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, identifyTitlePicture,
                    SolaLineDefineTabIdentify_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, identifyTitlePicture,
                    SolaLineDefineTabIdentify_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, linePassThresholdTitlePicture,
                    SolaIdentifyTabLinePassThreshold_Up, NULL, 0, pMsg);

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Up, NULL, 0, pMsg);

    for(i = 0; i < 8; i++)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, splitting1RatioPictureFirst[i],
                        splitting1Picture_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, splitting1RatioPictureFirst[i],
                        splitting1Picture_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, splitting2RatioPictureFirst[i],
                        splitting2Picture_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, splitting2RatioPictureFirst[i],
                        splitting2Picture_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, splitting3RatioPictureFirst[i],
                        splitting3Picture_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, splitting3RatioPictureFirst[i],
                        splitting3Picture_Up, NULL, i, pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSetting1LblWave[i],
                        splitting1Picture_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSetting1LblWave[i],
                        splitting1Picture_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSetting2LblWave[i],
                        splitting2Picture_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSetting2LblWave[i],
                        splitting2Picture_Up, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSolaSetting3LblWave[i],
                        splitting3Picture_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSolaSetting3LblWave[i],
                        splitting3Picture_Up, NULL, i, pMsg);
    }
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, inputOne, coefficentKeyboard, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, inputTwo, backscatteringKeyboard, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, resetPicture,resetButtonDown, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, resetPicture,resetButtonUp, NULL, 0, pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, reminderDialogPicture, reminderDialog_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, reminderDialogLabel, reminderDialog_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, reminderDialogPicture, reminderDialog_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, reminderDialogLabel, reminderDialog_Up, NULL, 0, pMsg);

    //注册桌面上控件的消息处理
    //...
    //注册按钮区控件的消息处理
    LoginWndMenuToMsg1(pSolaSettingMenu, pFrmMain);
    //...
    //注册信息栏上控件的消息处理
    //...
    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmMain,
                    MainErrProc_Func, NULL, 0, pMsg);

    return iReturn;
}

int linkDefineFrmMainExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
    //临时变量定义

    //得到当前窗体对象
    pFrmMain = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    ClearMessageReg(GetCurrMessage());

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmMain);
    tlinkDefineTextRes_Exit();
    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏

    DestroyPicture(&desk);
    DestroyPicture(&inputOne);
    DestroyPicture(&inputTwo);
    DestroyPicture(&reminderDialogPicture);
    DestroyPicture(&resetPicture);
    DestroyPicture(&identifyTitlePicture);
    DestroyPicture(&lineDefineTitlePicture);
    DestroyPicture(&linePassThresholdTitlePicture);
    DestroyPicture(&itemPassThresholdTitlePicture);

    DestroyLabel(&pSolaSettingLblIdentifyTitle);
    DestroyLabel(&pSolaSettingLblLineDefineTitle);
    DestroyLabel(&pSolaSettingLblLinePassThresholdTitle);
    DestroyLabel(&pSolaSettingLblItemPassThresholdTitle);
    DestroyLabel(&splittingRatioLabel);
    DestroyLabel(&coefficentLabel);
    DestroyLabel(&backscatteringLabel);
    DestroyLabel(&splittingFirstLabel);
    DestroyLabel(&splittingSecondLabel);
    DestroyLabel(&splittingThirdLabel);
    DestroyLabel(&dbLabel);
    DestroyLabel(&inputCoefficentLabel);
    DestroyLabel(&inputBackscatteringLabel);
    DestroyLabel(&reminderDialogLabel);
    DestroyLabel(&resetLabel);
    DestroyLabel(&pSolaSettingLblIdentifyTitle);
    DestroyLabel(&pSolaSettingLblLineDefineTitle);
    DestroyLabel(&pSolaSettingLblLinePassThresholdTitle);
    DestroyLabel(&pSolaSettingLblItemPassThresholdTitle);
    
    for(i = 0; i < 8; i++)
    {
        DestroyPicture(&splitting1RatioPictureFirst[i]);
        DestroyPicture(&splitting2RatioPictureFirst[i]);
        DestroyPicture(&splitting3RatioPictureFirst[i]);
        
        DestroyLabel(&pSolaSetting1LblWave[i]);
        DestroyLabel(&pSolaSetting2LblWave[i]);
        DestroyLabel(&pSolaSetting3LblWave[i]);
    }
    //销毁按钮区的控件
    DestroyWndMenu1(&pSolaSettingMenu);
    
    //保存参数
    SetSettingsData((void*)&pSolaMessager->lineDefine, sizeof(SOLA_LINEDEFINE), SOLA_LINEDEFINE_SET);
    SaveSettings(SOLA_LINEDEFINE_SET);

    return iReturn;
}

int linkDefineFrmMainPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
    //得到当前窗体对象
    pFrmMain = (GUIWINDOW *) pWndObj;

    DisplayPicture(desk);
    //显示按钮区的控件
    DisplayWndMenu1(pSolaSettingMenu);

    DisplayPicture(identifyTitlePicture);
    DisplayPicture(lineDefineTitlePicture);
    DisplayPicture(linePassThresholdTitlePicture);
    DisplayPicture(itemPassThresholdTitlePicture);

    DisplayLabel(pSolaSettingLblIdentifyTitle);
    DisplayLabel(pSolaSettingLblLineDefineTitle);
    DisplayLabel(pSolaSettingLblLinePassThresholdTitle);
    DisplayLabel(pSolaSettingLblItemPassThresholdTitle);
    DisplayPicture(inputOne);
    DisplayPicture(inputTwo);
    DisplayLabel(splittingRatioLabel);
    DisplayLabel(splittingFirstLabel);
    DisplayLabel(splittingSecondLabel);
    DisplayLabel(splittingThirdLabel);
    DisplayLabel(coefficentLabel);
    DisplayLabel(backscatteringLabel);
    DisplayLabel(inputCoefficentLabel);
    DisplayLabel(inputBackscatteringLabel);
    DisplayLabel(dbLabel);
    
   	for (i = 0; i < 8; i++)
	{
		if(i == pSolaMessager->lineDefine.splittingRatioLevel1)
        {
			SetPictureBitmap(BmpFileDirectory"splitting1Push.bmp", splitting1RatioPictureFirst[i]);
			DisplayPicture(splitting1RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pSolaSetting1LblWave[i]);
            DisplayLabel(pSolaSetting1LblWave[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"splitting1.bmp", splitting1RatioPictureFirst[i]);
			DisplayPicture(splitting1RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting1LblWave[i]);
            DisplayLabel(pSolaSetting1LblWave[i]);
		}
        
        if(i == pSolaMessager->lineDefine.splittingRatioLevel2){
			SetPictureBitmap(BmpFileDirectory"splitting1Push.bmp", splitting2RatioPictureFirst[i]);
			DisplayPicture(splitting2RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pSolaSetting2LblWave[i]);
            DisplayLabel(pSolaSetting2LblWave[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"splitting1.bmp", splitting2RatioPictureFirst[i]);
			DisplayPicture(splitting2RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting2LblWave[i]);
            DisplayLabel(pSolaSetting2LblWave[i]);
		}
        
        if(i == pSolaMessager->lineDefine.splittingRatioLevel3){
			SetPictureBitmap(BmpFileDirectory"splitting1Push.bmp", splitting3RatioPictureFirst[i]);
			DisplayPicture(splitting3RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), pSolaSetting3LblWave[i]);
            DisplayLabel(pSolaSetting3LblWave[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"splitting1.bmp", splitting3RatioPictureFirst[i]);
			DisplayPicture(splitting3RatioPictureFirst[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSetting3LblWave[i]);
            DisplayLabel(pSolaSetting3LblWave[i]);
		}
	}

    pSolaMessager->lineDefine.isReminderDialog 
    ? SetPictureBitmap(BmpFileDirectory"fiber_mask_select.bmp", reminderDialogPicture)
    : SetPictureBitmap(BmpFileDirectory"fiber_mask_unpress.bmp", reminderDialogPicture);
    DisplayPicture(reminderDialogPicture);
    DisplayLabel(reminderDialogLabel);
    
    DisplayPicture(resetPicture);
    DisplayLabel(resetLabel);

	SetPowerEnable(1, 1);
    SyncCurrFbmap();

    return iReturn;
}

int linkDefineFrmMainLoop(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
    GUIWINDOW *pWnd = NULL;

    //禁止并停止窗体循环
    SendWndMsg_LoopDisable(pWndObj);

    //设置窗体光标
    pWnd = pWndObj;
    pWnd->Visible.iCursor = 1;

    return iReturn;
}

int linkDefineFrmMainPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

    return iReturn;
}

int linkDefineFrmMainResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;

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
    SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
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
    SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
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
    SendWndMsg_WindowExit(pFrmMain);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}

GUIWINDOW* CreateSolaLineDefineWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   linkDefineFrmInit, linkDefineFrmMainExit,
                                   linkDefineFrmMainPaint, linkDefineFrmMainLoop,
                                   linkDefineFrmMainPause, linkDefineFrmMainResume,
                                   NULL);          //pWnd由调度线程释放
    return pWnd;
}

