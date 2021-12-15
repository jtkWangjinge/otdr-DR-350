#include "wnd_frmsolalinepassthreshold.h"
#include "wnd_frmime.h"
#include "wnd_frmselector.h"
#include "wnd_frmmenubak.h"
#include "wnd_frmsola.h"
#include "wnd_frmsolaidentify.h"
#include "wnd_frmsolalinedefine.h"
#include "wnd_frmsolaitempassthreshold.h"
#include "app_frminit.h"
#include "wnd_frmsolamessager.h"
#include "wnd_frmdialog.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "app_systemsettings.h"
#include "wnd_frmsolaident.h"
#include "app_unitconverter.h"

#define  AXIS  50
#define  LINKLENGTHMIN  0.0f
#define  LINKLENGTHMAX  100.0f

#define  LINKLOSSMIN  0.0f
#define  LINKLOSSMAX  100.0f

#define  LINKORLMIN  0.0f
#define  LINKORLMAX  100.0f

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

static int SolaIdentifyTabLineDefine_Down(void *pInArg, int iInLen, 
                                          void *pOutArg, int iOutLen);
static int SolaIdentifyTabLineDefine_Up(void *pInArg, int iInLen, 
                                        void *pOutArg, int iOutLen);

static int SolaIdentifyTabItemPassThreshold_Down(void *pInArg, int iInLen, 
                                                 void *pOutArg, int iOutLen);
static int SolaIdentifyTabItemPassThreshold_Up(void *pInArg, int iInLen, 
                                               void *pOutArg, int iOutLen);



static GUIWINDOW *pFrmSolaLinePassThreshold = NULL;
//桌面上的控件
static GUIPICTURE * desk =NULL;
static GUIPICTURE * inputLinkLengthMinValue =NULL;
static GUIPICTURE * inputLinkLengthMaxValue =NULL;
static GUIPICTURE * inputLinkLossMinValue =NULL;
static GUIPICTURE * inputLinkLossMaxValue =NULL;
static GUIPICTURE * inputLinkORLMaxValue =NULL;
static GUIPICTURE * resetPicture =NULL;
static GUIPICTURE * identifyTitlePicture =NULL;
static GUIPICTURE * lineDefineTitlePicture =NULL;
static GUIPICTURE * linePassThresholdTitlePicture =NULL;
static GUIPICTURE * itemPassThresholdTitlePicture =NULL;

static GUIPICTURE * linkLengthPicture[4] = {NULL};
static GUIPICTURE * linkLossPicture[4] = {NULL};
static GUIPICTURE * wavelengthPicture1 = NULL;
static GUIPICTURE * wavelengthPicture2 = NULL;
static GUIPICTURE * linkORLPicture1 = NULL;
static GUIPICTURE * linkORLPicture2 = NULL;
/*
static SELECTOR* linkLengthSelector = NULL;
static SELECTOR* waveLengthSelector = NULL;
static SELECTOR* linkLossSelector = NULL;
static SELECTOR* linkORLSelector = NULL;
*/
static  GUILABEL *plinkLengthLbl[4] = {NULL};
static  GUILABEL *plinkLossLbl[4] = {NULL};
static  GUILABEL *wavelengthLbl1 = NULL;
static  GUILABEL *wavelengthLbl2 = NULL;
static  GUILABEL *linkORLLbl1 = NULL;
static  GUILABEL *linkORLLbl2 = NULL;

static GUILABEL *minValueLabel = NULL;
static GUILABEL *maxValueLabel = NULL;
static GUILABEL *minValueLabel1 = NULL;
static GUILABEL *maxValueLabel1 = NULL;
static GUILABEL *maxValueLabel2 = NULL;
static GUILABEL *linkLengthLabel = NULL;
static GUILABEL *wavelengthLabel = NULL;
static GUILABEL *linkLossLabel = NULL;
static GUILABEL *linkORLLabel = NULL;
static GUILABEL *kmLabel = NULL;
static GUILABEL *kmMinValueLabel = NULL;
static GUILABEL *dbLabel = NULL;
static GUILABEL *dbTwoLabel = NULL;
static GUILABEL *dbThreeLabel = NULL;
static GUILABEL *resetLabel = NULL;
static  GUILABEL *inputLabel[5] = {0};



static  GUICHAR* maxValueStr = NULL;
static  GUICHAR* minValueStr = NULL;
static  GUICHAR* linkLengthStr = NULL;
static  GUICHAR* linkLengthSelectorStr[4] = {NULL};
static  GUICHAR* waveLengthStr = NULL;
static  GUICHAR* waveLengthSelectorStr[4] = {NULL};
static  GUICHAR* linkLossStr = NULL;
static  GUICHAR* linkLossSelectorStr[4] = {NULL};
static  GUICHAR* linkORLStr = NULL;
static  GUICHAR* linkORLSelectorStr[2] = {NULL};
static  GUICHAR* kmStr = NULL;
static  GUICHAR* dbStr = NULL;
static  GUICHAR* resetStr = NULL;

static WNDMENU1* pSolaSettingMenu = NULL;

static int iUnitConverterFlag = 0;              //单位转换的标志位
extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//设置顶层结构
/*********************************
*声明wnd_frmcommonset.c所引用的外部变量
*********************************/

#define INPUTCOUNT 5
static GUICHAR *inputStr[INPUTCOUNT] = {NULL};
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
        SendWndMsg_WindowExit(pFrmSolaLinePassThreshold);	//发送消息以便退出当前窗体
        SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
    }
        break;
    default:
        break;
    }
}

//初始化波长显示
static void initlinkLength()
{
    if(NULL != inputStr[0])
    {
        GuiMemFree(inputStr[0]);
    }
    if(NULL != inputStr[1])
    {
        GuiMemFree(inputStr[1]);
    }

    inputStr[0] = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_SOLA, 1000* pSolaMessager->linePassThreshold.lineLength.min,
                                                                    pSolaMessager->linePassThreshold.lineLength_ft.min,
                                                                    pSolaMessager->linePassThreshold.lineLength_mile.min, 0);
    inputStr[1] = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_SOLA, 1000* pSolaMessager->linePassThreshold.lineLength.max,
                                                                    pSolaMessager->linePassThreshold.lineLength_ft.max,
                                                                    pSolaMessager->linePassThreshold.lineLength_mile.max, 0);
    SetLabelText (inputStr[0], inputLabel[0]);
    SetLabelText (inputStr[1], inputLabel[1]);
}

static void initWaveLength(int index)
{
    int i=0;

    for(i = 0;i<3;i++)
    {
        if(NULL != inputStr[i+2])
        {
            GuiMemFree(inputStr[i+2]);
        }
    }

    inputStr[2] = Float2GuiString(LOSS_PRECISION, pSolaMessager->linePassThreshold.lineWave[index].lineLoss.min);
    inputStr[3] = Float2GuiString(LOSS_PRECISION, pSolaMessager->linePassThreshold.lineWave[index].lineLoss.max);
    inputStr[4] = Float2GuiString(ORL_PRECISION, pSolaMessager->linePassThreshold.lineWave[index].lineReturnLoss.max);
    
    for(i = 0; i < 3; ++i)
    {
        SetLabelText (inputStr[i+2], inputLabel[i+2]);
    }

}

static  void linepassTextRes_Init()
{
    pSolaSettingStrIdentifyTitle = TransString("SOLA_IDENTITY");
    pSolaSettingStrLineDefineTitle = TransString("SOLA_LINE_DEFINE");
    pSolaSettingStrLinePassThresholdTitle = TransString("SOLA_LINEPASS");
    pSolaSettingStrItemPassThresholdTitle = TransString("SOLA_ITEMPASS");

    minValueStr = TransString("SOLA_MINVALUE");
    maxValueStr = TransString("SOLA_MAXVALUE");
    //链路长度
    linkLengthStr = TransString("SOLA_LINKLENGTH");
    linkLengthSelectorStr[0] = TransString("SOLA_NONE");
    linkLengthSelectorStr[1] = TransString("SOLA_MINVALUE");
    linkLengthSelectorStr[2] = TransString("SOLA_MAXVALUE");
    linkLengthSelectorStr[3] = TransString("SOLA_MIN_MAXVALUE");

    //波长
    waveLengthStr = TransString("SOLA_WAVALENGTH");
    waveLengthSelectorStr[0] = TransString("1310 nm");
    waveLengthSelectorStr[1] = TransString("1550 nm");
    waveLengthSelectorStr[2] = TransString("1625 nm");
    waveLengthSelectorStr[3] = TransString("Random");

    //链路损耗
    linkLossStr = TransString("SOLA_LINKLOSS");
    linkLossSelectorStr[0] = TransString("SOLA_NONE");
    linkLossSelectorStr[1] = TransString("SOLA_MINVALUE");
    linkLossSelectorStr[2] = TransString("SOLA_MAXVALUE");
    linkLossSelectorStr[3] = TransString("SOLA_MIN_MAXVALUE");

    //链路光回损
    linkORLStr =TransString("SOLA_LINKORL");
    linkORLSelectorStr[0] = TransString("SOLA_NONE");
    linkORLSelectorStr[1] = TransString("SOLA_MAXVALUE");
    
    kmStr = GetCurrSystemUnitGuiString(MODULE_UNIT_SOLA);
    dbStr = TransString("dB");
    resetStr = TransString("SOLA_RESET");

    //初始化所有输入选项
    initlinkLength();
    initWaveLength(pSolaMessager->linePassThreshold.currentWave);
}



static  void linepassTextRes_Exit()
{
    int i = 0;
    GuiMemFree(pSolaSettingStrIdentifyTitle);
    GuiMemFree(pSolaSettingStrLineDefineTitle);
    GuiMemFree(pSolaSettingStrLinePassThresholdTitle);
    GuiMemFree(pSolaSettingStrItemPassThresholdTitle);
    //链路长度
    GuiMemFree(linkLengthStr);
    GuiMemFree(linkLengthSelectorStr[0]);
    GuiMemFree(linkLengthSelectorStr[1]);
    GuiMemFree(linkLengthSelectorStr[2]);
    GuiMemFree(linkLengthSelectorStr[3]);
    //波长
    GuiMemFree(waveLengthStr);
    GuiMemFree(waveLengthSelectorStr[0]);
    GuiMemFree(waveLengthSelectorStr[1]);
    GuiMemFree(waveLengthSelectorStr[2]);
    GuiMemFree(waveLengthSelectorStr[3]);
    //链路损耗
    GuiMemFree(linkLossStr);
    GuiMemFree(linkLossSelectorStr[0]);
    GuiMemFree(linkLossSelectorStr[1]);
    GuiMemFree(linkLossSelectorStr[2]);
    GuiMemFree(linkLossSelectorStr[3]);
    //链路光回损
    GuiMemFree(linkORLStr);
    GuiMemFree(linkORLSelectorStr[0]);
    GuiMemFree(linkORLSelectorStr[1]);

    GuiMemFree(kmStr);
    GuiMemFree(dbStr);

    for(i = 0; i < INPUTCOUNT; ++i)
    {
        GuiMemFree(inputStr[i]);
    }

    GuiMemFree(resetStr);
}

void JudgeEnable()
{

    SOLA_SET_TYPE type;

    type = pSolaMessager->linePassThreshold.lineLength.type;


    if( (type == KNONE) || (type == NOLYMAX) )  //disable min
    {
        SetPictureEnable (0, inputLinkLengthMinValue);
    }
    else
        SetPictureEnable (1, inputLinkLengthMinValue);

    if( (type == KNONE) || (type == NOLYMIN) )   //disable max
    {
        SetPictureEnable (0, inputLinkLengthMaxValue);
    }
    else
        SetPictureEnable (1, inputLinkLengthMaxValue);




    SOLA_SET_TYPE waveLengthType;
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    waveLengthType = pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.type;
    if( (waveLengthType == KNONE) || (waveLengthType == NOLYMAX) )  //disable min
    {
        SetPictureEnable (0, inputLinkLossMinValue);
    }
    else
        SetPictureEnable (1, inputLinkLossMinValue);

    if( (waveLengthType == KNONE) || (waveLengthType == NOLYMIN) )   //disable max
    {
        SetPictureEnable (0, inputLinkLossMaxValue);
    }
    else
        SetPictureEnable (1, inputLinkLossMaxValue);



    SOLA_SET_TYPE linkORFType;
    linkORFType = pSolaMessager->linePassThreshold.lineWave[currentWave].lineReturnLoss.type;
    if( linkORFType == KNONE)   //disable max
    {
        SetPictureEnable (0, inputLinkORLMaxValue);
    }
    else
        SetPictureEnable (1, inputLinkORLMaxValue);

}

/*static void SelectWaveLenBackCall (int iSelected)
{
    pSolaMessager->linePassThreshold.currentWave = iSelected;
    initWaveLength(iSelected);

    SetPictureEnable (1, inputLinkLossMinValue);
    SetPictureEnable (1, inputLinkLossMaxValue);
    SetPictureEnable (1, inputLinkORLMaxValue);
    DisplayPicture(inputLinkLossMinValue);
    DisplayPicture(inputLinkLossMaxValue);
    DisplayPicture(inputLinkORLMaxValue);

    DisplayLabel(inputLabel[2]);
    DisplayLabel(inputLabel[3]);
    DisplayLabel(inputLabel[4]);
    SyncCurrFbmap();
    JudgeEnable ();

    return ;
}
*/
static int wavelengthPicture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iSelected = iOutLen;
    pSolaMessager->linePassThreshold.currentWave = iSelected;
    initWaveLength(iSelected);
    JudgeEnable ();
	if(iSelected == 0){
		SetPictureBitmap(BmpFileDirectory"wavelengthPush.bmp", wavelengthPicture1);
		DisplayPicture(wavelengthPicture1);
		SetPictureBitmap(BmpFileDirectory"wavelength1.bmp", wavelengthPicture2);
		DisplayPicture(wavelengthPicture2);
        
        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), wavelengthLbl1);
        DisplayLabel(wavelengthLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), wavelengthLbl2);
        DisplayLabel(wavelengthLbl2);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"wavelengthPush.bmp", wavelengthPicture2);
		DisplayPicture(wavelengthPicture2);
		SetPictureBitmap(BmpFileDirectory"wavelength1.bmp", wavelengthPicture1);
		DisplayPicture(wavelengthPicture1);
        
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), wavelengthLbl1);
        DisplayLabel(wavelengthLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), wavelengthLbl2);
        DisplayLabel(wavelengthLbl2);
	}
    GUIWINDOW *pWnd = CreateSolaLinePassThresholdWindow();
    SendWndMsg_WindowExit(pFrmSolaLinePassThreshold);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
//    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int wavelengthPicture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
/*
static void linkLengthBackCall (int iSelected)
{
    pSolaMessager->linePassThreshold.lineLength.type = iSelected;

    JudgeEnable ();
}*/
static int linkLengthPicture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int i;
    int iSelected = iOutLen;
    pSolaMessager->linePassThreshold.lineLength.type = iSelected;

    JudgeEnable ();
    LOG(LOG_INFO, "iSelected = %d\n", iSelected);

    for (i = 0; i < 4; i++)
	{
		if(i == iSelected){
			SetPictureBitmap(BmpFileDirectory"linkLengthPush.bmp", linkLengthPicture[i]);
			DisplayPicture(linkLengthPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), plinkLengthLbl[i]);
            DisplayLabel(plinkLengthLbl[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"linkLength.bmp", linkLengthPicture[i]);
			DisplayPicture(linkLengthPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), plinkLengthLbl[i]);
            DisplayLabel(plinkLengthLbl[i]);
		}
	}

    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int linkLengthPicture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
/*
static void linkLossBackCall (int iSelected)

{
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.type  =iSelected;
    JudgeEnable ();
}*/
static int linkLossPicture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int i;
    int iSelected = iOutLen;
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.type  =iSelected;
    JudgeEnable ();
   	for (i = 0; i < 4; i++)
	{
		if(i == iSelected){
			SetPictureBitmap(BmpFileDirectory"linkLossPush.bmp", linkLossPicture[i]);
			DisplayPicture(linkLossPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), plinkLossLbl[i]);
            DisplayLabel(plinkLossLbl[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"linkLoss.bmp", linkLossPicture[i]);
			DisplayPicture(linkLossPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), plinkLossLbl[i]);
            DisplayLabel(plinkLossLbl[i]);
		}
	}
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int linkLossPicture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}
/*
static void linkORLBackCall (int iSelected)

{
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    pSolaMessager->linePassThreshold.lineWave[currentWave].lineReturnLoss.type  =iSelected;
    JudgeEnable ();

}
*/
static int linkORLPicture_Down(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    int iSelected = iOutLen;
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    pSolaMessager->linePassThreshold.lineWave[currentWave].lineReturnLoss.type  =iSelected;
    JudgeEnable ();
	if(iSelected == 0){
		SetPictureBitmap(BmpFileDirectory"linkORLPush.bmp", linkORLPicture1);
		DisplayPicture(linkORLPicture1);
        SetPictureBitmap(BmpFileDirectory"linkORL1.bmp", linkORLPicture2);
		DisplayPicture(linkORLPicture2);

        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), linkORLLbl1);
        DisplayLabel(linkORLLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), linkORLLbl2);
        DisplayLabel(linkORLLbl2);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"linkORLPush.bmp", linkORLPicture2);
		DisplayPicture(linkORLPicture2);
        SetPictureBitmap(BmpFileDirectory"linkORL1.bmp", linkORLPicture1);
		DisplayPicture(linkORLPicture1);

        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), linkORLLbl1);
        DisplayLabel(linkORLLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), linkORLLbl2);
        DisplayLabel(linkORLLbl2);
	}
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int linkORLPicture_Up(void *pInArg, int iInLen, 
                            void *pOutArg, int iOutLen)
{
    return 0;
}


static void ReCreateWindow(GUIWINDOW **pWnd)
{
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                         linkbyFrmMainInit, linkbyFrmMainExit,
                         linkbyFrmMainPaint, linkbyFrmMainLoop,
                         linkbyFrmMainPause, linkbyFrmMainResume,
                         NULL);          //pWnd由调度线程释放

}

static void AnalyzeConvertLinkLengthMin(double in, double* out_m, double* out_ft, double* out_mile)
{
    double threshold[UNIT_COUNT][2] = {{0.0f, 100000.0f}, {0.0f, 100.0000f}, 
                                       {0.0f, 328084.0f}, {0.0f, 328.0840f}, 
                                       {0.0f, 62.1371f}, {0.0f, 109361.3f}
                                      };
    double min = threshold[GetCurrSystemUnit(MODULE_UNIT_SOLA)][0];
    double max = threshold[GetCurrSystemUnit(MODULE_UNIT_SOLA)][1];

    double lengthMax = UnitConverter_Dist_Km2System_Float2Float(MODULE_UNIT_SOLA, 
                                pSolaMessager->linePassThreshold.lineLength.max);
    if(in < min || in > max)
    {
        switch(GetCurrSystemUnit(MODULE_UNIT_SOLA))
        {
            case UNIT_M:
                CreateIMEDialog(TransString("BETWEEN_0_TO_100000"));
                return;
            case UNIT_KM:
                CreateIMEDialog(TransString("BETWEEN_0_TO_100"));
                return;
            case UNIT_FT:
                CreateIMEDialog(TransString("BETWEEN_0_TO_328084"));  
                return;
            case UNIT_KFT:
                CreateIMEDialog(TransString("BETWEEN_0_TO_328_08"));
                return;
            case UNIT_YD:
                CreateIMEDialog(TransString("BETWEEN_0_TO_109361_3"));
                return;
            case UNIT_MI:
                CreateIMEDialog(TransString("BETWEEN_0_TO_62_15"));
                return;
            default:
                break;
        }
    }
    else if(in - lengthMax > 0.0f)
    {
        CreateIMEDialog(TransString("MIN_GREATER_THAN_MAX"));
        return;
    }
    
    *out_m = (float)UnitConverter_Dist_System2Km_Float2Float(MODULE_UNIT_SOLA, in);
    *out_ft = (float)UnitConverter_Dist_System2FT_Float2Float(MODULE_UNIT_SOLA, in);
    *out_mile = (float)UnitConverter_Dist_System2MI_Float2Float(MODULE_UNIT_SOLA, in);
}

static void  inputLinkLengthMinCallBack(void)
{
    char buff[64];
    GetIMEInputBuff(buff);
    double fTmp = String2Float(buff);   
    double out_m = pSolaMessager->linePassThreshold.lineLength.min;
    double out_ft = pSolaMessager->linePassThreshold.lineLength_ft.min;
    double out_mile = pSolaMessager->linePassThreshold.lineLength_mile.min;
    AnalyzeConvertLinkLengthMin(fTmp, &out_m, &out_ft, &out_mile);
    pSolaMessager->linePassThreshold.lineLength.min = out_m;
    pSolaMessager->linePassThreshold.lineLength_ft.min = out_ft;
    pSolaMessager->linePassThreshold.lineLength_mile.min = out_mile;
    //  SetLabelText(TransString(buff[0]),inputLabel[0]);

}

static void AnalyzeConvertLinkLengthMax(double in, double* out_m, double* out_ft, double* out_mile)
{
    double threshold[UNIT_COUNT][2] = {{0.0f, 100000.0f}, {0.0f, 100.0000f}, 
                                       {0.0f, 328084.0f}, {0.0f, 328.0840f}, 
                                       {0.0f, 62.1371f}, {0.0f, 109361.3f}
                                      };
    double min = threshold[GetCurrSystemUnit(MODULE_UNIT_SOLA)][0];
    double max = threshold[GetCurrSystemUnit(MODULE_UNIT_SOLA)][1];

    double lengthMin = UnitConverter_Dist_Km2System_Float2Float(MODULE_UNIT_SOLA, 
                                pSolaMessager->linePassThreshold.lineLength.min);
    if(in < min || in > max)
    {
        switch(GetCurrSystemUnit(MODULE_UNIT_SOLA))
        {
            case UNIT_M:
                CreateIMEDialog(TransString("BETWEEN_0_TO_100000"));
                return;
            case UNIT_KM:
                CreateIMEDialog(TransString("BETWEEN_0_TO_100"));
                return;
            case UNIT_FT:
                CreateIMEDialog(TransString("BETWEEN_0_TO_328084"));  
                return;
            case UNIT_KFT:
                CreateIMEDialog(TransString("BETWEEN_0_TO_328_08"));
                return;
            case UNIT_YD:
                CreateIMEDialog(TransString("BETWEEN_0_TO_109361_3"));
                return;
            case UNIT_MI:
                CreateIMEDialog(TransString("BETWEEN_0_TO_62_15"));
                return;
            default:
                break;
        }
    }
    else if(lengthMin - in > 0.0f)
    {
        CreateIMEDialog(TransString("MIN_GREATER_THAN_MAX"));
        return;
    }
    
    *out_m = (float)UnitConverter_Dist_System2Km_Float2Float(MODULE_UNIT_SOLA, in);
    *out_ft = (float)UnitConverter_Dist_System2FT_Float2Float(MODULE_UNIT_SOLA, in);
    *out_mile = (float)UnitConverter_Dist_System2MI_Float2Float(MODULE_UNIT_SOLA, in);
}

static void  inputLinkLengthMaxCallBack(void)
{
    char buff[512];
    GetIMEInputBuff(buff);
    double fTmp = String2Float(buff);   
    double out_m = pSolaMessager->linePassThreshold.lineLength.max;
    double out_ft = pSolaMessager->linePassThreshold.lineLength_ft.max;
    double out_mile = pSolaMessager->linePassThreshold.lineLength_mile.max;
    AnalyzeConvertLinkLengthMax(fTmp, &out_m, &out_ft, &out_mile);
    pSolaMessager->linePassThreshold.lineLength.max= out_m;
    pSolaMessager->linePassThreshold.lineLength_ft.max= out_ft;
    pSolaMessager->linePassThreshold.lineLength_mile.max= out_mile;
}

static void inputWaveLengthMinCallBack(void)
{
    int currentWave =  pSolaMessager->linePassThreshold.currentWave;
    char buff[64];
    GetIMEInputBuff(buff);

    double fTmp = String2Float(buff);

    if (fTmp < LINKLOSSMIN || fTmp > LINKLOSSMAX )
    {
        CreateIMEDialog(TransString("LOSS_BETWEEN_0_TO_100"));
        return;
    }
    //最小值大于最大值
    else if(pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.max < fTmp)
    {
        CreateIMEDialog(TransString("MIN_GREATER_THAN_MAX"));
        return;
    }
    
    pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.min = fTmp;
}

static void inputWaveLengthMaxCallBack(void)
{

    int currentWave =  pSolaMessager->linePassThreshold.currentWave;
    char buff[64];
    GetIMEInputBuff(buff);

    double fTmp = String2Float(buff);

    if (fTmp < LINKLOSSMIN || fTmp > LINKLOSSMAX)
    {
        CreateIMEDialog(TransString("LOSS_BETWEEN_0_TO_100"));
        return;
    }
    //最小值大于最大值
    else if(pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.min > fTmp)
    {
        CreateIMEDialog(TransString("MIN_GREATER_THAN_MAX"));
        return;
    }

    pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.max = fTmp;
    LOG(LOG_INFO, "currentWave = %d,pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.max = %f\n", currentWave, pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.max);
}

static void inputLinkPRLMaxCallBack(void)
{
    int currentWave =  pSolaMessager->linePassThreshold.currentWave;
    char buff[64];
    GetIMEInputBuff(buff);

    double fTmp = String2Float(buff);

    if (fTmp < LINKORLMIN || fTmp > LINKORLMAX)
    {
        CreateIMEDialog(TransString("ORL_BETWEEN_0_TO_100"));
        return;
    }
    
    pSolaMessager->linePassThreshold.lineWave[currentWave].lineReturnLoss.max = fTmp;

}

//链路长度输入最小值

static int linkLengthKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    char buff[32];
    double dist_m = pSolaMessager->linePassThreshold.lineLength.min;
    double dist_ft = pSolaMessager->linePassThreshold.lineLength_ft.min;
    double dist_mile = pSolaMessager->linePassThreshold.lineLength_mile.min;
    char* temp = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, 1000 * dist_m, dist_ft, dist_mile, 0);
    sprintf(buff, "%s", temp);
    GuiMemFree(temp);
    IMEInit(buff, 20, 1, ReCreateWindow, inputLinkLengthMinCallBack, NULL);
    return 0;
}
//链路长度输入最大值
static int linkLengthMaxValueKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    char buff[32];
    double dist_m = pSolaMessager->linePassThreshold.lineLength.max;
    double dist_ft = pSolaMessager->linePassThreshold.lineLength_ft.max;
    double dist_mile = pSolaMessager->linePassThreshold.lineLength_mile.max;
    char* temp = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, 1000 * dist_m, dist_ft, dist_mile, 0);
    sprintf(buff, "%s", temp);
    GuiMemFree(temp);
    IMEInit(buff, 20, 1, ReCreateWindow, inputLinkLengthMaxCallBack, NULL);
    return 0;
}


//链路损耗输入最小值
static int LossMinValueKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    char buff[32];
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    char* pMin = Float2String(LOSS_PRECISION, pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.min);
    sprintf(buff, "%s", pMin);
    GuiMemFree(pMin);
    IMEInit(buff, 20, 1, ReCreateWindow, inputWaveLengthMinCallBack, NULL);
    return 0;
}

//链路损耗输入最大值
static int LossMaxValueKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    char buff[32];
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    char* pMax = Float2String(LOSS_PRECISION, pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.max);
    sprintf(buff, "%s", pMax);
    GuiMemFree(pMax);
    IMEInit(buff, 20, 1, ReCreateWindow, inputWaveLengthMaxCallBack, NULL);
    return 0;
}

//链路光回损输入最大值
static int ORLMaxValueKeyboard(void *pInArg,int iInLen,void *pOutArg, int iOutLen)
{
    char buff[32];
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    char* pMax = Float2String(ORL_PRECISION, pSolaMessager->linePassThreshold.lineWave[currentWave].lineReturnLoss.max);
    sprintf(buff, "%s", pMax);
    GuiMemFree(pMax);
    IMEInit(buff, 20, 1, ReCreateWindow, inputLinkPRLMaxCallBack, NULL);
    return 0;
}

//错误事件处理函数
static int MainErrProc_Func(void *pInArg, int iInLen,
                            void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iReturn = 0;
    return iReturn;
}

//右侧菜单文本
static unsigned int strSolaSettingMenu[] = {

};

static int resetButtonUp(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    int iReturn = 0;
    GUIWINDOW *pWnd = NULL;
    ResetSolaLinePassSet((void*)&pSolaMessager->linePassThreshold);
    
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                        linkbyFrmMainInit , linkbyFrmMainExit,
                        linkbyFrmMainPaint, linkbyFrmMainLoop,
                        linkbyFrmMainPause, linkbyFrmMainResume,
                        NULL);          //pWnd由调度线程释放
    SendWndMsg_WindowExit(GetCurrWindow());	    //发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);           //发送消息以便调用新的窗体
    return  iReturn;
}
static int resetButtonDown(void *pInArg, int iInLen,
                       void *pOutArg, int iOutLen)
{
    TouchChange("btn_reset_press3.bmp", resetPicture,
                NULL, resetLabel, 0);
    RefreshScreen(__FILE__, __func__, __LINE__);
    //错误标志、返回值定义
    return 0;
}

int linkbyFrmMainInit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
    int currentWave = pSolaMessager->linePassThreshold.currentWave;   
    GUIMESSAGE *pMsg = NULL;//临时变量定义
    iUnitConverterFlag = pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag;
    linepassTextRes_Init();
    //得到当前窗体对象
    pFrmSolaLinePassThreshold = (GUIWINDOW *) pWndObj;
    desk = CreatePhoto("bg_sola3");
    //侧边栏menu
    pSolaSettingMenu = CreateWndMenu1(0, sizeof(strSolaSettingMenu), strSolaSettingMenu,
                                      (UINT16)(MENU_BACK | ~MENU_HOME),
                                      -1, 0, 40, SolaSettingMenuCall);

    minValueLabel  = CreateLabel(230, 175, 100, 30, minValueStr);
    maxValueLabel = CreateLabel(230, 110, 100, 30, maxValueStr);
    minValueLabel1  = CreateLabel(230, 385, 100, 30, minValueStr);
    maxValueLabel1 = CreateLabel(230, 315, 100, 30, maxValueStr);
    maxValueLabel2 = CreateLabel(480, 185, 100, 30, maxValueStr);
    
    for(i = 0; i < 4; i++)
    {
        plinkLengthLbl[i] = CreateLabel(115,103+35*i,120,24,linkLengthSelectorStr[i]);
        plinkLossLbl[i] = CreateLabel(115,311+35*i,120,24,linkLossSelectorStr[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), plinkLengthLbl[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, plinkLengthLbl[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), plinkLossLbl[i]);
        SetLabelAlign(GUILABEL_ALIGN_LEFT, plinkLossLbl[i]);
    }

    wavelengthLbl1 = CreateLabel(517,317,70,24,waveLengthSelectorStr[0]);
    wavelengthLbl2 = CreateLabel(517,352,70,24,waveLengthSelectorStr[1]);
    linkORLLbl1 = CreateLabel(517,103,70,24,linkORLSelectorStr[0]);
    linkORLLbl2 = CreateLabel(517,138,70,24,linkORLSelectorStr[1]);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), wavelengthLbl1);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, wavelengthLbl1);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), wavelengthLbl2);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, wavelengthLbl2);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), linkORLLbl1);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, linkORLLbl1);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), linkORLLbl2);
    SetLabelAlign(GUILABEL_ALIGN_LEFT, linkORLLbl2);
    
    linkLengthLabel = CreateLabel(35, 55, 350, 24, linkLengthStr);
    CreateLoopPhoto(linkLengthPicture,"linkLength",0,0,4);
    inputLinkLengthMinValue = CreatePhoto("otdr_input_loss_unpress");
    inputLabel[0] = CreateLabel(240, 205, 100, 30, inputStr[0]);
    kmMinValueLabel = CreateLabel(AXIS+310, 203, 80, 16, kmStr);
    inputLinkLengthMaxValue = CreatePhoto("otdr_input_loss_unpress1");
    inputLabel[1] = CreateLabel(240, 138, 100, 30, inputStr[1]);
    kmLabel = CreateLabel(AXIS+310, 135, 80, 24, kmStr);
    
    wavelengthLabel = CreateLabel(430, 265, 250, 24, waveLengthStr);
    wavelengthPicture1 = CreatePhoto("wavelength1");
    wavelengthPicture2 = CreatePhoto("wavelength2");
    linkLossLabel = CreateLabel(35, 265, 350, 24, linkLossStr);
    CreateLoopPhoto(linkLossPicture,"linkLoss",0,0,4);
    inputLinkLossMinValue = CreatePhoto("otdr_input_loss_unpress2");
    inputLabel[2] = CreateLabel(240, 410, 100, 30, inputStr[2]);
    inputLinkLossMaxValue = CreatePhoto("otdr_input_loss_unpress3");
    inputLabel[3] = CreateLabel(240, 345, 100, 30, inputStr[3]);
    dbLabel = CreateLabel(AXIS+310, 343, 80, 24, dbStr);
    dbTwoLabel = CreateLabel(AXIS+310, 410, 80, 24, dbStr);
    


    linkORLLabel = CreateLabel(430, 55, 250, 24, linkORLStr);
    SOLA_SET_TYPE type = pSolaMessager->linePassThreshold.lineWave[currentWave].lineReturnLoss.type;
    if(type == NOLYMAX) type = 1;

//    linkORLSelector  = CreateSelector(AXIS+160, 255, 2,type,linkORLSelectorStr,linkORLBackCall, 1);
   // inputLinkORLMaxValue =CreatePicture(AXIS+470, 261, 110, 30,BmpFileDirectory"otdr_input_loss_unpress4.bmp");
    linkORLPicture1 = CreatePhoto("linkORL1");
    linkORLPicture2 = CreatePhoto("linkORL2");
    inputLinkORLMaxValue = CreatePhoto("otdr_input_loss_unpress4");
    inputLabel[4] = CreateLabel(490, 213, 100, 30, inputStr[4]);
    dbThreeLabel = CreateLabel(AXIS+560, 211, 80, 24, dbStr);

    //resetPicture =CreatePicture(AXIS+521, 400, 110, 40,BmpFileDirectory"btn_otdr_save_press.bmp");
    resetPicture = CreatePhoto("btn_reset_unpress3");
    resetLabel = CreateLabel(504, 429, 136, 24, resetStr);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), resetLabel);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, resetLabel);

//    identifyTitlePicture = CreatePicture(0, 0, 100, 40,
//                                        BmpFileDirectory"otdr_top1.bmp");
//    lineDefineTitlePicture = CreatePicture(100, 0, 100, 40,
//                                        BmpFileDirectory"otdr_top2.bmp");
//    linePassThresholdTitlePicture = CreatePicture(200, 0, 100, 40,
//                                            BmpFileDirectory"otdr_top3f.bmp");
//    itemPassThresholdTitlePicture = CreatePicture(300, 0, 100, 40,
//                                            BmpFileDirectory"otdr_top4.bmp");
    identifyTitlePicture = CreatePhoto("otdr_top1");
    lineDefineTitlePicture = CreatePhoto("otdr_top2");
    linePassThresholdTitlePicture = CreatePhoto("otdr_top3f");
    itemPassThresholdTitlePicture = CreatePhoto("otdr_top4");

    pSolaSettingLblIdentifyTitle = CreateLabel(0, 22, 100, 24, pSolaSettingStrIdentifyTitle);
    pSolaSettingLblLineDefineTitle = CreateLabel(100, 22, 100, 24, pSolaSettingStrLineDefineTitle);
    pSolaSettingLblLinePassThresholdTitle = CreateLabel(200, 22, 100, 24, pSolaSettingStrLinePassThresholdTitle);
    pSolaSettingLblItemPassThresholdTitle = CreateLabel(300, 22, 100, 24, pSolaSettingStrItemPassThresholdTitle);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblIdentifyTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblIdentifyTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblLineDefineTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLineDefineTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSolaSettingLblLinePassThresholdTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblLinePassThresholdTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSolaSettingLblItemPassThresholdTitle);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pSolaSettingLblItemPassThresholdTitle);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), minValueLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), maxValueLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), minValueLabel1);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), maxValueLabel1);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), maxValueLabel2);
    
    SetLabelFont(getGlobalFnt(EN_FONT_BLUE), linkLengthLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLUE), wavelengthLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLUE), linkLossLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), dbTwoLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLUE), linkORLLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), kmLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), kmMinValueLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), dbLabel);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), dbThreeLabel);
    //注册桌面上的控件
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmSolaLinePassThreshold,pFrmSolaLinePassThreshold);
    AddWndMenuToComp1(pSolaSettingMenu, pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), resetPicture,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), identifyTitlePicture,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), lineDefineTitlePicture,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), itemPassThresholdTitlePicture,pFrmSolaLinePassThreshold);

    AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), inputLinkLengthMinValue, pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), inputLinkLossMinValue, pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), inputLinkLossMaxValue, pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), inputLinkORLMaxValue, pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIBUTTON, sizeof(GUIPICTURE), inputLinkLengthMaxValue, pFrmSolaLinePassThreshold);

/*
    AddSelectorToComp(linkLengthSelector, pFrmSolaLinePassThreshold);
    AddSelectorToComp(waveLengthSelector, pFrmSolaLinePassThreshold);
    AddSelectorToComp(linkLossSelector, pFrmSolaLinePassThreshold);
    AddSelectorToComp(linkORLSelector, pFrmSolaLinePassThreshold);
*/
    for(i=0;i<4;i++){
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), linkLengthPicture[i],pFrmSolaLinePassThreshold);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), linkLossPicture[i],pFrmSolaLinePassThreshold);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), plinkLengthLbl[i],pFrmSolaLinePassThreshold);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), plinkLossLbl[i],pFrmSolaLinePassThreshold);
    }
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), wavelengthLbl1,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), wavelengthLbl2,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), linkORLLbl1,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), linkORLLbl2,pFrmSolaLinePassThreshold);
    
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), wavelengthPicture1,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), wavelengthPicture2,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), linkORLPicture1,pFrmSolaLinePassThreshold);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), linkORLPicture2,pFrmSolaLinePassThreshold);
    pMsg = GetCurrMessage();

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, identifyTitlePicture,
                    SolaLineDefineTabIdentify_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, identifyTitlePicture,
                    SolaLineDefineTabIdentify_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, lineDefineTitlePicture,
                    SolaIdentifyTabLineDefine_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, itemPassThresholdTitlePicture,
                    SolaIdentifyTabItemPassThreshold_Up, NULL, 0, pMsg);


    LoginMessageReg(GUIMESSAGE_TCH_DOWN, inputLinkLengthMinValue, linkLengthKeyboard, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, inputLinkLossMinValue, LossMinValueKeyboard, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, inputLinkLossMaxValue, LossMaxValueKeyboard, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, inputLinkORLMaxValue, ORLMaxValueKeyboard, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, inputLinkLengthMaxValue, linkLengthMaxValueKeyboard, NULL, 0, pMsg);
/*
    LoginSelectorToMsg(linkLengthSelector, pMsg);
    LoginSelectorToMsg(waveLengthSelector, pMsg);
    LoginSelectorToMsg(linkLossSelector, pMsg);
    LoginSelectorToMsg(linkORLSelector, pMsg);
*/
    for(i=0;i<4;i++){
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkLengthPicture[i], linkLengthPicture_Down, NULL, i, pMsg);    
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkLengthPicture[i], linkLengthPicture_Up, NULL, i, pMsg); 
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkLossPicture[i], linkLossPicture_Down, NULL, i, pMsg); 
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkLossPicture[i], linkLossPicture_Up, NULL, i, pMsg); 

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, plinkLengthLbl[i], linkLengthPicture_Down, NULL, i, pMsg);    
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, plinkLengthLbl[i], linkLengthPicture_Up, NULL, i, pMsg); 
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, plinkLossLbl[i], linkLossPicture_Down, NULL, i, pMsg); 
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, plinkLossLbl[i], linkLossPicture_Up, NULL, i, pMsg);
    }
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthLbl1, wavelengthPicture_Down, NULL, 0, pMsg); 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthLbl1, wavelengthPicture_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthLbl2, wavelengthPicture_Down, NULL, 1, pMsg); 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthLbl2, wavelengthPicture_Up, NULL, 1, pMsg); 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLLbl1, linkORLPicture_Down, NULL, 0, pMsg); 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLLbl1, linkORLPicture_Up, NULL, 0, pMsg); 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLLbl2, linkORLPicture_Down, NULL, 1, pMsg); 
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLLbl2, linkORLPicture_Up, NULL, 1, pMsg); 
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthPicture1, wavelengthPicture_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthPicture1, wavelengthPicture_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthPicture2, wavelengthPicture_Down, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, wavelengthPicture2, wavelengthPicture_Up, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLPicture1, linkORLPicture_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLPicture1, linkORLPicture_Up, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLPicture2, linkORLPicture_Down, NULL, 1, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, linkORLPicture2, linkORLPicture_Up, NULL, 1, pMsg);
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, resetPicture,resetButtonDown, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, resetPicture,resetButtonUp, NULL, 0, pMsg);


    //注册按钮区控件的消息处理
    LoginWndMenuToMsg1(pSolaSettingMenu, pFrmSolaLinePassThreshold);
    //注册窗体的错误处理函数
    LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmSolaLinePassThreshold,
                    MainErrProc_Func, NULL, 0, pMsg);

    return iReturn;
}
//判断是否可以输入值

int linkbyFrmMainExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
	
    //得到当前窗体对象
    pFrmSolaLinePassThreshold = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
    //***************************************************************/
    ClearMessageReg(GetCurrMessage());

    //从当前窗体中注销窗体控件
    //***************************************************************/
    ClearWindowComp(pFrmSolaLinePassThreshold);
    linepassTextRes_Exit();
    //销毁窗体控件
    //***************************************************************/
    //销毁状态栏、桌面、信息栏

    DestroyPicture(&desk);
    DestroyPicture(&inputLinkLengthMinValue);
    DestroyPicture(&inputLinkLengthMaxValue);
    DestroyPicture(&inputLinkLossMinValue);
    DestroyPicture(&inputLinkLossMaxValue);
    DestroyPicture(&inputLinkORLMaxValue);
    DestroyPicture(&resetPicture);
    DestroyPicture(&identifyTitlePicture);
    DestroyPicture(&lineDefineTitlePicture);
    DestroyPicture(&linePassThresholdTitlePicture);
    DestroyPicture(&itemPassThresholdTitlePicture);
    for(i=0;i<4;i++){
       DestroyPicture(&linkLengthPicture[i]);
       DestroyPicture(&linkLossPicture[i]);
       DestroyLabel(&plinkLengthLbl[i]);
       DestroyLabel(&plinkLossLbl[i]);
    }
    DestroyLabel(&wavelengthLbl1);
    DestroyLabel(&wavelengthLbl2);
    DestroyLabel(&linkORLLbl1);
    DestroyLabel(&linkORLLbl2);
    
    DestroyPicture(&wavelengthPicture1);
    DestroyPicture(&wavelengthPicture2);
    DestroyPicture(&linkORLPicture1);
    DestroyPicture(&linkORLPicture2);

    DestroyPicture(&itemPassThresholdTitlePicture);
    DestroyLabel(&minValueLabel);
    DestroyLabel(&maxValueLabel);
    DestroyLabel(&minValueLabel1);
    DestroyLabel(&maxValueLabel1);
    DestroyLabel(&maxValueLabel2);
    DestroyLabel(&linkLengthLabel);
    DestroyLabel(&wavelengthLabel);
    DestroyLabel(&linkLossLabel);
    DestroyLabel(&linkORLLabel);
    DestroyLabel(&kmLabel);
    DestroyLabel(&kmMinValueLabel);
    DestroyLabel(&dbLabel);
    DestroyLabel(&dbTwoLabel);
    DestroyLabel(&dbThreeLabel);
    DestroyLabel(&inputLabel[0]);
    DestroyLabel(&inputLabel[1]);
    DestroyLabel(&inputLabel[2]);
    DestroyLabel(&inputLabel[3]);
    DestroyLabel(&inputLabel[4]);
    DestroyLabel(&resetLabel);
    DestroyLabel(&pSolaSettingLblIdentifyTitle);
    DestroyLabel(&pSolaSettingLblLineDefineTitle);
    DestroyLabel(&pSolaSettingLblLinePassThresholdTitle);
    DestroyLabel(&pSolaSettingLblItemPassThresholdTitle);

/*
    DestroySelector(&linkLengthSelector);
    DestroySelector(&waveLengthSelector);
    DestroySelector(&linkLossSelector);
    DestroySelector(&linkORLSelector);
*/
    //销毁按钮区的控件
    DestroyWndMenu1(&pSolaSettingMenu);

    //保存参数
    SetSettingsData((void*)&pSolaMessager->linePassThreshold, sizeof(SOLA_LINE_PASSTHRESHOLD), SOLA_LINEPASS_SET);
    SaveSettings(SOLA_LINEPASS_SET);

    return iReturn;
}

int linkbyFrmMainPaint(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int i = 0;
    //得到当前窗体对象
    pFrmSolaLinePassThreshold = (GUIWINDOW *) pWndObj;

    DisplayPicture(desk);
    DisplayPicture(identifyTitlePicture);
    DisplayPicture(lineDefineTitlePicture);
    DisplayPicture(linePassThresholdTitlePicture);
    DisplayPicture(itemPassThresholdTitlePicture);
    //显示按钮区的控件
    DisplayWndMenu1(pSolaSettingMenu);
    DisplayPicture(identifyTitlePicture);
    DisplayPicture(lineDefineTitlePicture);
    DisplayPicture(linePassThresholdTitlePicture);
    DisplayPicture(itemPassThresholdTitlePicture);
    DisplayLabel(minValueLabel);
    DisplayLabel(maxValueLabel);
    DisplayLabel(minValueLabel1);
    DisplayLabel(maxValueLabel1);
    DisplayLabel(maxValueLabel2);
    DisplayLabel(kmLabel);
    DisplayLabel(kmMinValueLabel);
	if(pSolaMessager->linePassThreshold.currentWave == 0){
		SetPictureBitmap(BmpFileDirectory"wavelengthPush.bmp", wavelengthPicture1);
		DisplayPicture(wavelengthPicture1);
		SetPictureBitmap(BmpFileDirectory"wavelength1.bmp", wavelengthPicture2);
		DisplayPicture(wavelengthPicture2);
        
        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), wavelengthLbl1);
        DisplayLabel(wavelengthLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), wavelengthLbl2);
        DisplayLabel(wavelengthLbl2);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"wavelengthPush.bmp", wavelengthPicture2);
		DisplayPicture(wavelengthPicture2);
		SetPictureBitmap(BmpFileDirectory"wavelength1.bmp", wavelengthPicture1);
		DisplayPicture(wavelengthPicture1);
        
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), wavelengthLbl1);
        DisplayLabel(wavelengthLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), wavelengthLbl2);
        DisplayLabel(wavelengthLbl2);
	}
    int currentWave = pSolaMessager->linePassThreshold.currentWave;
    if(pSolaMessager->linePassThreshold.lineWave[currentWave].lineReturnLoss.type == 0){
		SetPictureBitmap(BmpFileDirectory"linkORLPush.bmp", linkORLPicture1);
		DisplayPicture(linkORLPicture1);
        SetPictureBitmap(BmpFileDirectory"linkORL1.bmp", linkORLPicture2);
		DisplayPicture(linkORLPicture2);

        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), linkORLLbl1);
        DisplayLabel(linkORLLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), linkORLLbl2);
        DisplayLabel(linkORLLbl2);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"linkORLPush.bmp", linkORLPicture2);
		DisplayPicture(linkORLPicture2);
        SetPictureBitmap(BmpFileDirectory"linkORL1.bmp", linkORLPicture1);
		DisplayPicture(linkORLPicture1);

        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), linkORLLbl1);
        DisplayLabel(linkORLLbl1);
        SetLabelFont(getGlobalFnt(EN_FONT_BLUE), linkORLLbl2);
        DisplayLabel(linkORLLbl2);
	}
   	for (i = 0; i < 4; i++)
	{
		if(i == pSolaMessager->linePassThreshold.lineLength.type){
			SetPictureBitmap(BmpFileDirectory"linkLengthPush.bmp", linkLengthPicture[i]);
			DisplayPicture(linkLengthPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), plinkLengthLbl[i]);
            DisplayLabel(plinkLengthLbl[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"linkLength.bmp", linkLengthPicture[i]);
			DisplayPicture(linkLengthPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), plinkLengthLbl[i]);
            DisplayLabel(plinkLengthLbl[i]);
		}
		if(i == pSolaMessager->linePassThreshold.lineWave[currentWave].lineLoss.type){
			SetPictureBitmap(BmpFileDirectory"linkLossPush.bmp", linkLossPicture[i]);
			DisplayPicture(linkLossPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLUE), plinkLossLbl[i]);
            DisplayLabel(plinkLossLbl[i]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"linkLoss.bmp", linkLossPicture[i]);
			DisplayPicture(linkLossPicture[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), plinkLossLbl[i]);
            DisplayLabel(plinkLossLbl[i]);
		}
	}
    DisplayLabel(wavelengthLbl1);
    DisplayLabel(wavelengthLbl2);
    DisplayLabel(linkORLLbl1);
    DisplayLabel(linkORLLbl2);
    
    DisplayPicture(wavelengthPicture1);
    DisplayPicture(wavelengthPicture2);
    DisplayPicture(linkORLPicture1);
    DisplayPicture(linkORLPicture2);
    
    DisplayLabel(linkLengthLabel);
//    DisplaySelector(linkLengthSelector);
    DisplayPicture(inputLinkLengthMinValue);
    DisplayLabel(inputLabel[0]);
    DisplayPicture(inputLinkLengthMaxValue);
    DisplayLabel(inputLabel[1]);


    DisplayLabel(pSolaSettingLblIdentifyTitle);
    DisplayLabel(pSolaSettingLblLineDefineTitle);
    DisplayLabel(pSolaSettingLblLinePassThresholdTitle);
    DisplayLabel(pSolaSettingLblItemPassThresholdTitle);


    DisplayLabel(wavelengthLabel);
//    DisplaySelector(waveLengthSelector);

    DisplayLabel(linkLossLabel);
//   DisplaySelector(linkLossSelector);
    DisplayPicture(inputLinkLossMinValue);
    DisplayLabel(inputLabel[2]);
    DisplayPicture(inputLinkLossMaxValue);
    DisplayLabel(inputLabel[3]);
    DisplayLabel(dbTwoLabel);

    DisplayLabel(linkORLLabel);
//   DisplaySelector(linkORLSelector);
    DisplayPicture(inputLinkORLMaxValue);
    DisplayLabel(inputLabel[4]);
    DisplayLabel(dbLabel);
    DisplayLabel(dbThreeLabel);
    DisplayPicture(resetPicture);
    DisplayLabel(resetLabel);

	SetPowerEnable(1, 1);

    SyncCurrFbmap();

    JudgeEnable();
    return iReturn;
}

int linkbyFrmMainLoop(void *pWndObj)
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

int linkbyFrmMainPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iReturn = 0;
    return iReturn;
}

int linkbyFrmMainResume(void *pWndObj)
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
    SendWndMsg_WindowExit(pFrmSolaLinePassThreshold);	//发送消息以便退出当前窗体
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
    SendWndMsg_WindowExit(pFrmSolaLinePassThreshold);	//发送消息以便退出当前窗体
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
    SendWndMsg_WindowExit(pFrmSolaLinePassThreshold);	//发送消息以便退出当前窗体
    SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体

    return 0;
}



GUIWINDOW* CreateSolaLinePassThresholdWindow()
{
    GUIWINDOW* pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   linkbyFrmMainInit, linkbyFrmMainExit,
                                   linkbyFrmMainPaint, linkbyFrmMainLoop,
                                   linkbyFrmMainPause, linkbyFrmMainResume,
                                   NULL);          //pWnd由调度线程释放
    return pWnd;
}




