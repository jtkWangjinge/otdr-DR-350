/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcurveinfo.c
* 摘    要：  实现主窗体frmotdrmeas的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*
* 取代版本：  
* 原 作 者：
* 完成日期：
**************************************************************/
#include "wnd_frmcurveinfo.h"
#include "app_curve.h"
#include "app_frmotdr.h"
#include "guiphoto.h"
#include "app_parameter.h"
#include "app_unitconverter.h"

#define NOMAL_INFO_LINE	6	//标准显示的行数
#define MIN_INFO_LINE	3	//最小时显示的行数

#define PAGE1_LIST_LEFT_ROW 4
#define PAGE1_LIST_LEFT_COLUMN 3
#define PAGE1_LIST_RIGHT_ROW 3
#define PAGE1_LIST_RIGHT_COLUMN 3

#define PAGE2_LIST_LEFT_ROW 4
#define PAGE2_LIST_LEFT_COLUMN 3
#define PAGE2_LIST_RIGHT_ROW 4
#define PAGE2_LIST_RIGHT_COLUMN 3

#define SMALL_PAGE_ROW 3
#define SMALL_PAGE_COLUMN 4


static char *StrWaveLenght[] = {
"1310 nm",
"1550 nm",
};
/*
*/
static char *StrPulse[] = {
"AUTO",
"5 ns",
"10 ns",
"20 ns",
#ifdef MINI2
"30 ns",
#endif
"50 ns",
"100 ns",
"200 ns",
"500 ns",
"1 us",
"2 us",
"10 us",
"20 us",
};

char *cTmp[] = {
	"LSA",
	"TPA"
};

//量程
static char *RangeKm[] = {
"AUTO",
"1.3 km",
"2.5 km",
"5 km",
"10 km",
"20 km",
"40 km",
"80 km",
"120 km",
"160 km",
"260 km",
};

static char *RangeMi[] = {
"AUTO",
"0.81 mi",
"1.55 mi",
"3.11 mi",
"6.22 mi",
"12.4 mi",
"24.8 mi",
"49.7 mi",
"74.6 mi",
"99.4 mi",
"161.6 mi",
};

static char *RangeKft[] = {
"AUTO",
"4.26 kft",
"8.2  kft",
"16.4 kft",
"32.8 kft",
"65.6 kft",
"131.2 kft",
"262.4 kft",
"393.7 kft",
"524.9 kft",
"852.8 kft",
};

static char *AvgTime[] = {
"AUTO",
"5 sec",
"15 sec",
"30 sec",
"60 sec",
"90 sec",
"120 sec",
"180 sec",
"RT",
};


int smpInterVal[] = {
	1
};

char *cLeftBmp[] = {
    BmpFileDirectory"bg_curveinfo_unpress1.bmp",
    BmpFileDirectory"bg_curveinfo_unpress2.bmp",
    BmpFileDirectory"bg_curveinfo_unpress3.bmp",
    BmpFileDirectory"bg_curveinfo_unpress4.bmp"
};
char *cRightBmp[] = {
    BmpFileDirectory"bg_curveinfo_unpress5.bmp",
    BmpFileDirectory"bg_curveinfo_unpress6.bmp",
    BmpFileDirectory"bg_curveinfo_unpress7.bmp",
    BmpFileDirectory"bg_curveinfo_unpress8.bmp"
};
/*
*/
extern POTDR_TOP_SETTINGS pOtdrTopSettings;


extern PSYSTEMSET    pCurSystemSet;

static int iDisplayCurveInfoFlag = 0;		//是否显示曲线信息
static int iDisplayLineNum = 6;				//显示曲线信息的行数

static int iStartY = 0;						//曲线框开始的y坐标
static int curveInfoPageSelect = 0;
static int currentCurveType = 0;

static PUSER_SETTINGS pUser_Settings = NULL;
static PANALYSIS_SETTING 	pAnalysis_Setting = NULL;
static PSAMPLE_SETTING     pSample_Setting = NULL;
static PCOMMON_SETTING     pCommon_Setting = NULL;

/*******************************************************************************
**							窗体frmcurveinfo中的创建控件 					  **
*******************************************************************************/
static GUIPICTURE *pCurInfoBG[3] = {NULL};
static GUIPICTURE *pCurInfoLeftBG[NOMAL_INFO_LINE] = {NULL};
static GUIPICTURE *pCurInfoRightBG[NOMAL_INFO_LINE] = {NULL};

static GUIPICTURE *pCurveInfoPage1Btn = NULL;
static GUIPICTURE *pCurveInfoPage2Btn = NULL;

static GUILABEL *pCurveInfoLblPage1 = NULL;
static GUILABEL *pCurveInfoLblPage2 = NULL;

static GUICHAR *pCurveInfoStrPage1 = NULL;
static GUICHAR *pCurveInfoStrPage2 = NULL;

static GUILABEL *pCurInfoLblLeftCap[NOMAL_INFO_LINE] = {NULL};
static GUILABEL *pCurInfoLblLeftVal[NOMAL_INFO_LINE] = {NULL};

static GUICHAR *pCurInfoStrLeftCap[NOMAL_INFO_LINE] = {NULL};
static GUICHAR *pCurInfoStrLeftVal[NOMAL_INFO_LINE] = {NULL};

static GUILABEL *pCurInfoLblRightCap[NOMAL_INFO_LINE] = {NULL};
static GUILABEL *pCurInfoLblRightVal[NOMAL_INFO_LINE] = {NULL};

static GUICHAR *pCurInfoStrRightCap[NOMAL_INFO_LINE] = {NULL};
static GUICHAR *pCurInfoStrRightVal[NOMAL_INFO_LINE] = {NULL};

static GUILABEL* pCurveInfoLblPage1LeftListTitle = NULL;
static GUILABEL* pCurveInfoLblPage1LeftList[PAGE1_LIST_LEFT_ROW][PAGE1_LIST_LEFT_COLUMN] = {{NULL}};
static GUILABEL* pCurveInfoLblPage1RightList[PAGE1_LIST_RIGHT_ROW][PAGE1_LIST_RIGHT_COLUMN] = {{NULL}};

static GUILABEL* pCurveInfoLblPage2LeftListTitle = NULL;
static GUILABEL* pCurveInfoLblPage2LeftList[PAGE2_LIST_LEFT_ROW][PAGE2_LIST_LEFT_COLUMN] = {{NULL}};
static GUILABEL* pCurveInfoLblPage2RightList[PAGE2_LIST_RIGHT_ROW][PAGE2_LIST_RIGHT_COLUMN] = {{NULL}};

static GUICHAR* pCurveInfoStrPage1LeftListTitle = NULL;
static GUICHAR* pCurveInfoStrPage1LeftList[PAGE1_LIST_LEFT_ROW][PAGE1_LIST_LEFT_COLUMN] = {{NULL}};
static GUICHAR* pCurveInfoStrPage1RightList[PAGE1_LIST_RIGHT_ROW][PAGE1_LIST_RIGHT_COLUMN] = {{NULL}};

static GUICHAR* pCurveInfoStrPage2LeftListTitle = NULL;
static GUICHAR* pCurveInfoStrPage2LeftList[PAGE2_LIST_LEFT_ROW][PAGE2_LIST_LEFT_COLUMN] = {{NULL}};
static GUICHAR* pCurveInfoStrPage2RightList[PAGE2_LIST_RIGHT_ROW][PAGE2_LIST_RIGHT_COLUMN] = {{NULL}};

static GUILABEL* pCurveInfoLblSmallPageList[SMALL_PAGE_ROW][SMALL_PAGE_COLUMN] = {{NULL}};
static GUICHAR* pCurveInfoStrSmallPageList[SMALL_PAGE_ROW][SMALL_PAGE_COLUMN] = {{NULL}};

static int CurvePage1Btn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CurvePage1Btn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int CurvePage2Btn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CurvePage2Btn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

static int showPage1CurveInfo();
static int showPage2CurveInfo();
static int showSmallViewCurveInfo();

static void refresh1310WaveInfo(PCURVE_INFO curve1310Info, int clearFlag);
static void refresh1550WaveInfo(PCURVE_INFO curve1550Info, int clearFlag);
static void refreshSmallPageWaveInfo(PCURVE_INFO currentCurveInfo);

//设置是否显示曲线信息
int SetCurveInfoDisplay(void *arg, int iFlag, GUIWINDOW *pWnd)
{
	iDisplayCurveInfoFlag = iFlag;

    if((iDisplayCurveInfoFlag)&&(0 == currentCurveType))
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pCurveInfoPage1Btn, pWnd);
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pCurveInfoPage2Btn, pWnd);

        GUIMESSAGE *pMsg = GetCurrMessage();
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCurveInfoPage1Btn, 
                	    CurvePage1Btn_Down, NULL, 0, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pCurveInfoPage1Btn, 
                    	CurvePage1Btn_Up, NULL, 0, pMsg);

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCurveInfoPage2Btn, 
                	    CurvePage2Btn_Down, NULL, 0, pMsg);
	    LoginMessageReg(GUIMESSAGE_TCH_UP, pCurveInfoPage2Btn, 
                    	CurvePage2Btn_Up, NULL, 0, pMsg);
    }
    else
    {
        GUIMESSAGE *pMsg = GetCurrMessage();
        DelWindowComp(pCurveInfoPage1Btn, pWnd);
        DelWindowComp(pCurveInfoPage2Btn, pWnd);

        LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCurveInfoPage1Btn, pMsg);
        LogoutMessageReg(GUIMESSAGE_TCH_UP, pCurveInfoPage1Btn, pMsg);
        LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pCurveInfoPage2Btn, pMsg);
        LogoutMessageReg(GUIMESSAGE_TCH_UP, pCurveInfoPage2Btn, pMsg);
    }
    
	return 0;
}

//获得是否显示曲线信息
int GetCurveInfoDisplay(void)
{
	return iDisplayCurveInfoFlag;
}

//初始化文本信息
void TextRes_Init(void)
{
    pUser_Settings = pOtdrTopSettings->pUser_Setting;
	pAnalysis_Setting = &(pUser_Settings->sAnalysisSetting);
	pSample_Setting = &(pUser_Settings->sSampleSetting);
	pCommon_Setting = &(pUser_Settings->sCommonSetting);
    
    pCurveInfoStrPage1 = TransString("CURVEINFO_PAGE1");
    pCurveInfoStrPage2 = TransString("CURVEINFO_PAGE2");
    
    pCurveInfoStrPage1LeftListTitle = TransString("CURVEINFO_SUMMARYTRACEINFO");
    pCurveInfoStrPage1LeftList[0][0] = TransString("OTDR_LBL_WAVELENGTH");
    pCurveInfoStrPage1LeftList[0][1] = TransString("1310nm");
    pCurveInfoStrPage1LeftList[0][2] = TransString("1550nm");
    pCurveInfoStrPage1LeftList[1][0] = TransString("CURVEINFO_PASS_FAIL_STATUS");
    pCurveInfoStrPage1LeftList[1][1] = TransString(" ");
    pCurveInfoStrPage1LeftList[1][2] = TransString(" ");

    pCurveInfoStrPage1LeftList[2][0] = TransString("CURVEINFO_SPAN_LENGTH");
    pCurveInfoStrPage1LeftList[2][1] = TransString(" ");
    pCurveInfoStrPage1LeftList[2][2] = TransString(" ");
   
    pCurveInfoStrPage1LeftList[3][0] = TransString("CURVEINFO_LBL_DATE");
    pCurveInfoStrPage1LeftList[3][1] = TransString(" ");
    pCurveInfoStrPage1LeftList[3][2] = TransString(" ");
   
    pCurveInfoStrPage1RightList[0][0] = TransString("OTDR_LBL_WAVELENGTH");
    pCurveInfoStrPage1RightList[0][1] = TransString("1310nm");
    pCurveInfoStrPage1RightList[0][2] = TransString("1550nm");
    pCurveInfoStrPage1RightList[1][0] = TransString("CURVEIINFO_SPAN_LOSS");
    
    pCurveInfoStrPage1RightList[1][1] = TransString(" ");
    pCurveInfoStrPage1RightList[1][2] = TransString(" ");    
    pCurveInfoStrPage1RightList[2][0] = TransString("CURVEINFO_SPAN_ORL");

    pCurveInfoStrPage1RightList[2][1] = TransString(" ");
    pCurveInfoStrPage1RightList[2][2] = TransString(" ");
       
    pCurveInfoStrPage2LeftList[0][0] = TransString("OTDR_LBL_WAVELENGTH");
    pCurveInfoStrPage2LeftList[0][1] = TransString("1310nm");
    pCurveInfoStrPage2LeftList[0][2] = TransString("1550nm");
    pCurveInfoStrPage2LeftList[1][0] = TransString("CURVEINFO_RANGE");
    pCurveInfoStrPage2LeftList[1][1] = TransString(" ");
    pCurveInfoStrPage2LeftList[1][2] = TransString(" ");
    pCurveInfoStrPage2LeftList[2][0] = TransString("CURVEINFO_LBL_PULSEWIDTH");
    pCurveInfoStrPage2LeftList[2][1] = TransString(" ");
    pCurveInfoStrPage2LeftList[2][2] = TransString(" ");
    pCurveInfoStrPage2LeftList[3][0] = TransString("OTDR_LBL_TIME");
    pCurveInfoStrPage2LeftList[3][1] = TransString(" ");
    pCurveInfoStrPage2LeftList[3][2] = TransString(" ");
    
    pCurveInfoStrPage2RightList[0][0] = TransString("OTDR_LBL_WAVELENGTH");
    pCurveInfoStrPage2RightList[0][1] = TransString("1310nm");
    pCurveInfoStrPage2RightList[0][2] = TransString("1550nm");
    pCurveInfoStrPage2RightList[1][0] = TransString("CURVEINFO_AVERAGING_TIME");
    pCurveInfoStrPage2RightList[1][1] = TransString(" ");
    pCurveInfoStrPage2RightList[1][2] = TransString("");
    pCurveInfoStrPage2RightList[2][0] = TransString("CURVEINFO_LBL_REFLECTION");
    pCurveInfoStrPage2RightList[2][1] = TransString(" ");
    pCurveInfoStrPage2RightList[2][2] = TransString(" ");
    pCurveInfoStrPage2RightList[3][0] = TransString("CURVEINFO_BACKSCATTER_COEF");
    pCurveInfoStrPage2RightList[3][1] = TransString("----");
    pCurveInfoStrPage2RightList[3][2] = TransString("----");

    pCurveInfoStrPage2LeftListTitle = TransString("CURVEINFO_TEST_SETTING");
    pCurveInfoStrSmallPageList[0][0] = TransString("OTDR_LBL_WAVELENGTH");
    pCurveInfoStrSmallPageList[0][2] = TransString("CURVEINFO_LBL_REFLECTION");
    pCurveInfoStrSmallPageList[0][3] = TransString("0.000000");
    pCurveInfoStrSmallPageList[1][0] = TransString("CURVEINFO_LBL_PULSEWIDTH");
    pCurveInfoStrSmallPageList[1][2] = TransString("CURVEINFO_BACKSCATTER_COEF");
    pCurveInfoStrSmallPageList[1][3] = TransString("0.000000");
    pCurveInfoStrSmallPageList[2][0] = TransString("CURVEINFO_AVERAGING_TIME");
    pCurveInfoStrSmallPageList[2][2] = TransString("CURVEINFO_LBL_LOSSMEASURE");

    char buff[512];
    memset(buff, 0, sizeof(buff));
    snprintf(buff, sizeof(buff), "%s", cTmp[pCommon_Setting->iLossMethodMark]);
    pCurveInfoStrSmallPageList[2][3] = TransString(buff);
    
}

//创建曲线信息控件
//iType: 0标准；1延长
int CreateCurveInfo(GUIWINDOW *pWnd, int iType)
{
	int iLblStartY = 0;

	if(!iType)
	{
		iDisplayLineNum = NOMAL_INFO_LINE;
		iStartY = 330;
		iLblStartY = iStartY + 20;
        currentCurveType = 0;
	}
	else
	{
		iDisplayLineNum = MIN_INFO_LINE;
		iStartY = 384;
		iLblStartY = iStartY;
        currentCurveType = 1;
	}

	if(pWnd == NULL)
	{
		LOG(LOG_ERROR, "pWnd is NULL\n");
		return -1;
	}

	TextRes_Init();
	pCurInfoBG[0] = CreatePicture(0, iStartY, 681, 150, 
								BmpFileDirectory"bg_otdr_curveinfo_page1.bmp");
    pCurInfoBG[1] = CreatePicture(0, iStartY, 681, 150,
                                   BmpFileDirectory"bg_otdr_curveinfo_page2.bmp");
	pCurInfoBG[2] = CreatePicture(0, iStartY, 681, 96, 
								BmpFileDirectory"small_page.bmp");

    int rowCount;
    int columnCount;

    //创建曲线界面PAGE1的左边列表
    pCurveInfoPage1Btn = CreatePicture(0, 331,120, 24,BmpFileDirectory"type_page_btn_release.bmp");
    pCurveInfoLblPage1 = CreateLabel(5, 335, 110, 24, pCurveInfoStrPage1);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveInfoLblPage1);
    pCurveInfoLblPage1LeftListTitle = CreateLabel(14, 359, 317, 24, pCurveInfoStrPage1LeftListTitle);
    for(rowCount = 0; rowCount < PAGE1_LIST_LEFT_ROW; rowCount++)
    {
        for(columnCount = 0; columnCount < PAGE1_LIST_LEFT_COLUMN; columnCount++)
        {
            if(columnCount == 0)
            {
                pCurveInfoLblPage1LeftList[rowCount][columnCount] = CreateLabel(15, 386 + rowCount * 24, 
                                                                145, 24, pCurveInfoStrPage1LeftList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_LEFT, pCurveInfoLblPage1LeftList[rowCount][columnCount]);
            }
            else
            {
                pCurveInfoLblPage1LeftList[rowCount][columnCount] = CreateLabel(10 + 149 + (columnCount - 1) * 86, 386 + rowCount * 24, 
                                                                86, 24, pCurveInfoStrPage1LeftList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveInfoLblPage1LeftList[rowCount][columnCount]);
            }
            
        }
    }

    //创建曲线界面PAGE1的右边列表
    for(rowCount = 0; rowCount < PAGE1_LIST_RIGHT_ROW; rowCount++)
    {
        for(columnCount = 0; columnCount < PAGE1_LIST_RIGHT_COLUMN; columnCount++)
        {
            if(columnCount == 0)
            {
                pCurveInfoLblPage1RightList[rowCount][columnCount] = CreateLabel(353, 386 + rowCount * 24, 
                                                                145, 24, pCurveInfoStrPage1RightList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_LEFT, pCurveInfoLblPage1RightList[rowCount][columnCount]);
            }
            else
            {
                pCurveInfoLblPage1RightList[rowCount][columnCount] = CreateLabel(349 + 149 + (columnCount - 1) * 86, 386 + rowCount * 24, 
                                                                86, 24, pCurveInfoStrPage1RightList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveInfoLblPage1RightList[rowCount][columnCount]);
            }
            
        }
    }
        
    //创建曲线界面PAGE2的左边列表
    pCurveInfoPage2Btn = CreatePicture(120, 331,120, 24,BmpFileDirectory"type_page_btn_release.bmp");
    pCurveInfoLblPage2 = CreateLabel(126, 335, 110, 24, pCurveInfoStrPage2);
    SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveInfoLblPage2);
    pCurveInfoLblPage2LeftListTitle = CreateLabel(14, 359, 317, 24, pCurveInfoStrPage2LeftListTitle);
    for(rowCount = 0; rowCount < PAGE2_LIST_LEFT_ROW; rowCount++)
    {
        for(columnCount = 0; columnCount < PAGE2_LIST_LEFT_COLUMN; columnCount++)
        {
            if(columnCount == 0)
            {
                pCurveInfoLblPage2LeftList[rowCount][columnCount] = CreateLabel(15, 386 + rowCount * 24, 
                                                                145, 24, pCurveInfoStrPage2LeftList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_LEFT, pCurveInfoLblPage2LeftList[rowCount][columnCount]);
            }
            else
            {
                pCurveInfoLblPage2LeftList[rowCount][columnCount] = CreateLabel(10 + 149 + (columnCount - 1) * 86, 386 + rowCount * 24, 
                                                                86, 24, pCurveInfoStrPage1LeftList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveInfoLblPage2LeftList[rowCount][columnCount]);
            }
            
        }
    }
    
    // 创建曲线界面PAGE2的右边列表
    for(rowCount = 0; rowCount < PAGE2_LIST_RIGHT_ROW; rowCount++)
    {
        for(columnCount = 0; columnCount < PAGE2_LIST_RIGHT_COLUMN; columnCount++)
        {
            if(columnCount == 0)
            {
                pCurveInfoLblPage2RightList[rowCount][columnCount] = CreateLabel(353, 386 + rowCount * 24, 
                                                                145, 24, pCurveInfoStrPage2RightList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_LEFT, pCurveInfoLblPage2RightList[rowCount][columnCount]);
            }
            else
            {
                pCurveInfoLblPage2RightList[rowCount][columnCount] = CreateLabel(349 + 149 + (columnCount - 1) * 86, 386 + rowCount * 24, 
                                                                86, 24, pCurveInfoStrPage2RightList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveInfoLblPage2RightList[rowCount][columnCount]);
            }
            
        }
    }

    // 创建小屏情况下的信息显示控件
    for(rowCount = 0; rowCount < SMALL_PAGE_ROW; rowCount++)
    {
        for(columnCount = 0; columnCount < SMALL_PAGE_COLUMN; columnCount++)
        {
            if((columnCount % 2) == 0)
            {
                pCurveInfoLblSmallPageList[rowCount][columnCount] = CreateLabel(14 + columnCount * 165, 408 + rowCount * 24, 
                                                                174, 24, pCurveInfoStrSmallPageList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_LEFT, pCurveInfoLblSmallPageList[rowCount][columnCount]);
            }
            else
            {
                pCurveInfoLblSmallPageList[rowCount][columnCount] = CreateLabel(195 + (columnCount-1) * 165, 408 + rowCount * 24, 
                                                                142, 24, pCurveInfoStrSmallPageList[rowCount][columnCount]);
                SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveInfoLblSmallPageList[rowCount][columnCount]);
            }
        }
    }
    
	return 0;
}

//显示曲线信息
void DisplayCurveInfo(void)
{ 
    if(!currentCurveType)
    {
        if(curveInfoPageSelect)
        {
            showPage2CurveInfo(); 
        }
        else
        {
            showPage1CurveInfo();
        }
    }
    else
    {
        showSmallViewCurveInfo();
    }

}

//释放文本信息
void TextRes_Exit(void)
{
	int i,j;
	for(i = 0; i < NOMAL_INFO_LINE; i++)
	{
		GuiMemFree(pCurInfoStrLeftCap[i]);
		GuiMemFree(pCurInfoStrLeftVal[i]);

		GuiMemFree(pCurInfoStrRightCap[i]);
		GuiMemFree(pCurInfoStrRightVal[i]);
	}

    for(i = 0; i < PAGE1_LIST_LEFT_ROW; i++)
    {
        for(j = 0; j < PAGE1_LIST_LEFT_COLUMN; j++)
        {

            GuiMemFree(pCurveInfoStrPage1LeftList[i][j]);    
        }
    }

    for(i = 0; i < PAGE1_LIST_RIGHT_ROW; i++)
    {
        for(j = 0; j < PAGE1_LIST_RIGHT_COLUMN; j++)
        {

            GuiMemFree(pCurveInfoStrPage1RightList[i][j]);    
        }
    }

    for(i = 0; i < PAGE2_LIST_LEFT_ROW; i++)
    {
        for(j = 0; j < PAGE2_LIST_LEFT_COLUMN; j++)
        {

            GuiMemFree(pCurveInfoStrPage2LeftList[i][j]);    
        }
    }

    for(i = 0; i < PAGE2_LIST_RIGHT_ROW; i++)
    {
        for(j = 0; j < PAGE2_LIST_RIGHT_COLUMN; j++)
        {

            GuiMemFree(pCurveInfoStrPage2RightList[i][j]);    
        }
    }

    for(i = 0; i < SMALL_PAGE_ROW; i++)
    {
        for(j = 0; j < SMALL_PAGE_COLUMN; j++)
        {

            GuiMemFree(pCurveInfoStrSmallPageList[i][j]);    
        }
    }  
}

//释放曲线信息资源
void DestoryCurveInfo()
{
	int i,j;
	TextRes_Exit();

	DestroyPicture(&(pCurInfoBG[0]));
	DestroyPicture(&(pCurInfoBG[1]));
    DestroyPicture(&(pCurInfoBG[2]));
    DestroyPicture(&pCurveInfoPage1Btn);
    DestroyPicture(&pCurveInfoPage2Btn);

	for(i = 0; i < iDisplayLineNum; i++)
	{
	    DestroyPicture(&(pCurInfoLeftBG[i]));
        DestroyPicture(&(pCurInfoRightBG[i]));
		DestroyLabel(&(pCurInfoLblLeftCap[i]));
		DestroyLabel(&(pCurInfoLblLeftVal[i]));
		DestroyLabel(&(pCurInfoLblRightCap[i]));
		DestroyLabel(&(pCurInfoLblRightVal[i]));
	}

    DestroyLabel(&pCurveInfoLblPage1LeftListTitle);
    DestroyLabel(&pCurveInfoLblPage2LeftListTitle);

    for(i = 0; i < PAGE1_LIST_LEFT_ROW; i++)
    {
        for(j = 0; j < PAGE1_LIST_LEFT_COLUMN; j++)
        {

            DestroyLabel(&(pCurveInfoLblPage1LeftList[i][j]));    
        }
    }

    for(i = 0; i < PAGE1_LIST_RIGHT_ROW; i++)
    {
        for(j = 0; j < PAGE1_LIST_RIGHT_COLUMN; j++)
        {

            DestroyLabel(&(pCurveInfoLblPage1RightList[i][j]));    
        }
    }

    for(i = 0; i < PAGE2_LIST_LEFT_ROW; i++)
    {
        for(j = 0; j < PAGE2_LIST_LEFT_COLUMN; j++)
        {

            DestroyLabel(&(pCurveInfoLblPage2LeftList[i][j]));    
        }
    }

    for(i = 0; i < PAGE2_LIST_RIGHT_ROW; i++)
    {
        for(j = 0; j < PAGE2_LIST_RIGHT_COLUMN; j++)
        {

            DestroyLabel(&(pCurveInfoLblPage2RightList[i][j]));    
        }
    }

    for(i = 0; i < SMALL_PAGE_ROW; i++)
    {
        for(j = 0; j < SMALL_PAGE_COLUMN; j++)
        {

            DestroyLabel(&(pCurveInfoLblSmallPageList[i][j]));    
        }
    }
   
}

//刷新曲线信息
int RefreshCurveInfo(void *arg)
{
	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
    pUser_Settings = pOtdrTopSettings->pUser_Setting;
	pAnalysis_Setting = &(pUser_Settings->sAnalysisSetting);
	pSample_Setting = &(pUser_Settings->sSampleSetting);
	pCommon_Setting = &(pUser_Settings->sCommonSetting);
    
    PCURVE_INFO curve1310Info = pDisplay->pCurve[0];
    PCURVE_INFO curve1550Info = pDisplay->pCurve[1];

    if(curve1310Info->pParam.enWave)
    {
        curve1550Info = pDisplay->pCurve[0];
        curve1310Info = pDisplay->pCurve[1];
    }
    
    refresh1310WaveInfo(curve1310Info, !curve1310Info->uiAnalysised);
    refresh1550WaveInfo(curve1550Info, !curve1550Info->uiAnalysised);

    PCURVE_INFO currentCurveInfo;

    if(0 == pDisplay->pCurve[pDisplay->iCurCurve]->pParam.enWave)
    {
        currentCurveInfo = curve1310Info;
    }
    else
    {
        currentCurveInfo = curve1550Info;
    }

    refreshSmallPageWaveInfo(currentCurveInfo);
	DisplayCurveInfo();

	return 0;
}

static int CurvePage1Btn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    curveInfoPageSelect = 0;
    showPage1CurveInfo();
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

static int CurvePage1Btn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    return 0;
}

static int CurvePage2Btn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    curveInfoPageSelect = 1;
    showPage2CurveInfo();
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

static int CurvePage2Btn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    return 0;
}

static int showPage1CurveInfo()
{
	int i,j;
	
    DisplayPicture(pCurInfoBG[1]);
    SetPictureBitmap(BmpFileDirectory"type_page_btn_pressed.bmp", pCurveInfoPage1Btn);
    SetPictureBitmap(BmpFileDirectory"type_page_btn_release.bmp", pCurveInfoPage2Btn);    
    DisplayPicture(pCurveInfoPage1Btn);
    DisplayPicture(pCurveInfoPage2Btn);
    DisplayLabel(pCurveInfoLblPage1);
    DisplayLabel(pCurveInfoLblPage2);
    DisplayLabel(pCurveInfoLblPage1LeftListTitle);

    for(i = 0; i < PAGE1_LIST_LEFT_ROW;i++)
    {
        for(j = 0;j < PAGE1_LIST_LEFT_COLUMN;j++)
        {
            DisplayLabel(pCurveInfoLblPage1LeftList[i][j]);
        }
    }
    
    for(i = 0; i < PAGE1_LIST_RIGHT_ROW;i++)
    {
        for(j = 0;j < PAGE1_LIST_RIGHT_COLUMN;j++)
        {
            DisplayLabel(pCurveInfoLblPage1RightList[i][j]);
        }
    }

    return 0;
}
static int showPage2CurveInfo()
{
	int i,j;
    DisplayPicture(pCurInfoBG[0]);
    SetPictureBitmap(BmpFileDirectory"type_page_btn_release.bmp", pCurveInfoPage1Btn);
    SetPictureBitmap(BmpFileDirectory"type_page_btn_pressed.bmp", pCurveInfoPage2Btn); 
    DisplayPicture(pCurveInfoPage1Btn);
    DisplayPicture(pCurveInfoPage2Btn);
    DisplayLabel(pCurveInfoLblPage1);
    DisplayLabel(pCurveInfoLblPage2);
    DisplayLabel(pCurveInfoLblPage2LeftListTitle);

    for(i = 0; i < PAGE2_LIST_LEFT_ROW;i++)
    {
        for(j = 0;j < PAGE2_LIST_LEFT_COLUMN;j++)
        {
            DisplayLabel(pCurveInfoLblPage2LeftList[i][j]);
        }
    }
    
    for(i = 0; i < PAGE2_LIST_RIGHT_ROW;i++)
    {
        for(j = 0;j < PAGE2_LIST_RIGHT_COLUMN;j++)
        {
            DisplayLabel(pCurveInfoLblPage2RightList[i][j]);
        }
    }

    return 0;
}

static int showSmallViewCurveInfo()
{
    int i,j;
    DisplayPicture(pCurInfoBG[2]);
    for(i = 0; i < SMALL_PAGE_ROW; i++)
    {
        for(j = 0; j < SMALL_PAGE_COLUMN; j++)
        {
            DisplayLabel(pCurveInfoLblSmallPageList[i][j]);
        }
    }

    return 0;
}

static void refresh1310WaveInfo(PCURVE_INFO curve1310Info, int clearFlag)
{
    char buff[512];
    if(clearFlag)
    {
        GuiMemFree(pCurveInfoStrPage1LeftList[1][1]);
        pCurveInfoStrPage1LeftList[1][1] = TransString("");
        SetLabelText(pCurveInfoStrPage1LeftList[1][1], pCurveInfoLblPage1LeftList[1][1]);

        GuiMemFree(pCurveInfoStrPage1LeftList[2][1]);
        pCurveInfoStrPage1LeftList[2][1] = TransString("");
        SetLabelText(pCurveInfoStrPage1LeftList[2][1], pCurveInfoLblPage1LeftList[2][1]);

        GuiMemFree(pCurveInfoStrPage1LeftList[3][1]);
        pCurveInfoStrPage1LeftList[3][1] = TransString("");
        SetLabelText(pCurveInfoStrPage1LeftList[3][1], pCurveInfoLblPage1LeftList[3][1]);

        GuiMemFree(pCurveInfoStrPage1RightList[1][1]);
        pCurveInfoStrPage1RightList[1][1] = TransString("");
        SetLabelText(pCurveInfoStrPage1RightList[1][1], pCurveInfoLblPage1RightList[1][1]);
        
        GuiMemFree(pCurveInfoStrPage1RightList[2][1]);
        pCurveInfoStrPage1RightList[2][1] = TransString("");
        SetLabelText(pCurveInfoStrPage1RightList[2][1], pCurveInfoLblPage1RightList[2][1]);

        GuiMemFree(pCurveInfoStrPage2LeftList[1][1]);
        pCurveInfoStrPage2LeftList[1][1] = TransString("");
        SetLabelText(pCurveInfoStrPage2LeftList[1][1], pCurveInfoLblPage2LeftList[1][1]);

        GuiMemFree(pCurveInfoStrPage2LeftList[2][1]);
        pCurveInfoStrPage2LeftList[2][1] = TransString("");
        SetLabelText(pCurveInfoStrPage2LeftList[2][1], pCurveInfoLblPage2LeftList[2][1]);

        GuiMemFree(pCurveInfoStrPage2LeftList[3][1]);
        pCurveInfoStrPage2LeftList[3][1] = TransString("");
        SetLabelText(pCurveInfoStrPage2LeftList[3][1], pCurveInfoLblPage2LeftList[3][1]);

        GuiMemFree(pCurveInfoStrPage2RightList[1][1]);
        pCurveInfoStrPage2RightList[1][1] = TransString("");
        SetLabelText(pCurveInfoStrPage2RightList[1][1], pCurveInfoLblPage2RightList[1][1]);

        GuiMemFree(pCurveInfoStrPage2RightList[2][1]);
        pCurveInfoStrPage2RightList[2][1] = TransString("");
        SetLabelText(pCurveInfoStrPage2RightList[2][1], pCurveInfoLblPage2RightList[2][1]);

        GuiMemFree(pCurveInfoStrPage2RightList[3][1]);
        pCurveInfoStrPage2RightList[3][1] = TransString("");
        SetLabelText(pCurveInfoStrPage2RightList[3][1], pCurveInfoLblPage2RightList[3][1]);
        
    }
    else
    {
        GuiMemFree(pCurveInfoStrPage1LeftList[1][1]);
        if(curve1310Info->Events.iIsPass)
        {
            pCurveInfoStrPage1LeftList[1][1] = TransString("OTDR_PASS");
        }
        else
        {
            pCurveInfoStrPage1LeftList[1][1] = TransString("OTDR_FAIL");
        }
        SetLabelText(pCurveInfoStrPage1LeftList[1][1], pCurveInfoLblPage1LeftList[1][1]);

        GuiMemFree(pCurveInfoStrPage1LeftList[2][1]);
        float currentSpanLength_1310 = GetSpanLength(&curve1310Info->Events);
        pCurveInfoStrPage1LeftList[2][1] = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
            currentSpanLength_1310, 1);
        SetLabelText(pCurveInfoStrPage1LeftList[2][1], pCurveInfoLblPage1LeftList[2][1]);

        GuiMemFree(pCurveInfoStrPage1RightList[1][1]);
        float totalLoss = GetSpanLoss(&curve1310Info->Events);
        pCurveInfoStrPage1RightList[1][1] = Float2GuiStringUnit(LOSS_PRECISION, totalLoss, "dB");
        SetLabelText(pCurveInfoStrPage1RightList[1][1], pCurveInfoLblPage1RightList[1][1]);
        
        GuiMemFree(pCurveInfoStrPage1RightList[2][1]);
        GuiMemFree(pCurveInfoStrPage1RightList[2][2]);
        memset(buff, 0, sizeof(buff));
        MARKER markerInfo_1310 = curve1310Info->mMarker;
        if(markerInfo_1310.fTotalReutrnLoss < RETURNLOSS_MAXVALUE)
        {
            pCurveInfoStrPage1RightList[2][1] = Float2GuiStringUnit(ORL_PRECISION, 
                markerInfo_1310.fTotalReutrnLoss, "dB");
        }
        else
        {
            pCurveInfoStrPage1RightList[2][1] = TransString("----");
        }
        SetLabelText(pCurveInfoStrPage1RightList[2][1], pCurveInfoLblPage1RightList[2][1]);

        GuiMemFree(pCurveInfoStrPage1LeftList[3][1]);
        memset(buff, 0, sizeof(buff));
        getCurveMeasureDate(buff, NULL, pCurSystemSet->uiTimeShowFlag);
        pCurveInfoStrPage1LeftList[3][1] = TransString(buff);
        SetLabelText(pCurveInfoStrPage1LeftList[3][1], pCurveInfoLblPage1LeftList[3][1]);

        GuiMemFree(pCurveInfoStrPage2LeftList[1][1]);
        switch(GetCurrSystemUnit(MODULE_UNIT_OTDR))
        {
            case UNIT_M:
            case UNIT_KM:
            pCurveInfoStrPage2LeftList[1][1] = TransString(RangeKm[curve1310Info->pParam.enRange]);
            break;
            case UNIT_FT:
            case UNIT_KFT:
            pCurveInfoStrPage2LeftList[1][1] = TransString(RangeKft[curve1310Info->pParam.enRange]);
            break;
            case UNIT_YD:
            case UNIT_MI:
            pCurveInfoStrPage2LeftList[1][1] = TransString(RangeMi[curve1310Info->pParam.enRange]);
            break;
            default:
            break;
        }
        SetLabelText(pCurveInfoStrPage2LeftList[1][1], pCurveInfoLblPage2LeftList[1][1]);

        
        GuiMemFree(pCurveInfoStrPage2LeftList[2][1]);
        
        pCurveInfoStrPage2LeftList[2][1] = TransString(StrPulse[curve1310Info->pParam.enPluse]);
        SetLabelText(pCurveInfoStrPage2LeftList[2][1], pCurveInfoLblPage2LeftList[2][1]);

        GuiMemFree(pCurveInfoStrPage2LeftList[3][1]);
        memset(buff, 0, sizeof(buff));
        getCurveMeasureDate(NULL, buff, pCurSystemSet->uiTimeShowFlag);
        pCurveInfoStrPage2LeftList[3][1] = TransString(buff);
        SetLabelText(pCurveInfoStrPage2LeftList[3][1], pCurveInfoLblPage2LeftList[3][1]);

        GuiMemFree(pCurveInfoStrPage2RightList[1][1]);
        memset(buff, 0, sizeof(buff));
        if(curve1310Info->pParam.enTime == ENUM_AVG_TIME_AUTO || curve1310Info->pParam.enTime == ENUM_AVG_TIME_REALTIME)
        {
            sprintf(buff, "%s", AvgTime[curve1310Info->pParam.enTime]);
        }
        else
        {
            sprintf(buff, "%d sec", curve1310Info->pParam.uiAvgTime);
        }
        pCurveInfoStrPage2RightList[1][1] = TransString(buff);
        SetLabelText(pCurveInfoStrPage2RightList[1][1], pCurveInfoLblPage2RightList[1][1]);

        memset(buff, 0, sizeof(buff));
        GuiMemFree(pCurveInfoStrPage2RightList[2][1]);
        pCurveInfoStrPage2RightList[2][1] = Float2GuiString(IOR_PRECISION, 
            pUser_Settings->sSampleSetting.sWaveLenArgs[curve1310Info->pParam.enWave].fRefractiveIndex);
        SetLabelText(pCurveInfoStrPage2RightList[2][1], pCurveInfoLblPage2RightList[2][1]);

        memset(buff, 0, sizeof(buff));
        GuiMemFree(pCurveInfoStrPage2RightList[3][1]);
        pCurveInfoStrPage2RightList[3][1] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, 
            curve1310Info->pParam.fBackScatter);
        SetLabelText(pCurveInfoStrPage2RightList[3][1], pCurveInfoLblPage2RightList[3][1]);
    }
}

static void refresh1550WaveInfo(PCURVE_INFO curve1550Info, int clearFlag)
{    
    char buff[512];
    if(clearFlag)
    {
        GuiMemFree(pCurveInfoStrPage1LeftList[1][2]);   
        pCurveInfoStrPage1LeftList[1][2] = TransString("");
        SetLabelText(pCurveInfoStrPage1LeftList[1][2], pCurveInfoLblPage1LeftList[1][2]);

        GuiMemFree(pCurveInfoStrPage1LeftList[2][2]);   
        pCurveInfoStrPage1LeftList[2][2] = TransString("");
        SetLabelText(pCurveInfoStrPage1LeftList[2][2], pCurveInfoLblPage1LeftList[2][2]);

        GuiMemFree(pCurveInfoStrPage1LeftList[3][2]);   
        pCurveInfoStrPage1LeftList[3][2] = TransString("");
        SetLabelText(pCurveInfoStrPage1LeftList[3][2], pCurveInfoLblPage1LeftList[3][2]);

        GuiMemFree(pCurveInfoStrPage1RightList[1][2]);
        pCurveInfoStrPage1RightList[1][2] = TransString("");
        SetLabelText(pCurveInfoStrPage1RightList[1][2], pCurveInfoLblPage1RightList[1][2]);

        GuiMemFree(pCurveInfoStrPage1RightList[2][2]);
        pCurveInfoStrPage1RightList[2][2] = TransString("");
        SetLabelText(pCurveInfoStrPage1RightList[2][2], pCurveInfoLblPage1RightList[2][2]);

        GuiMemFree(pCurveInfoStrPage2LeftList[1][2]);
        pCurveInfoStrPage2LeftList[1][2] = TransString("");
        SetLabelText(pCurveInfoStrPage2LeftList[1][2], pCurveInfoLblPage2LeftList[1][2]);

        GuiMemFree(pCurveInfoStrPage2LeftList[2][2]);
        pCurveInfoStrPage2LeftList[2][2] = TransString("");
        SetLabelText(pCurveInfoStrPage2LeftList[2][2], pCurveInfoLblPage2LeftList[2][2]);

        GuiMemFree(pCurveInfoStrPage2LeftList[3][2]);
        pCurveInfoStrPage2LeftList[3][2] = TransString("");
        SetLabelText(pCurveInfoStrPage2LeftList[3][2], pCurveInfoLblPage2LeftList[3][2]);
        
        GuiMemFree(pCurveInfoStrPage2RightList[1][2]);
        pCurveInfoStrPage2RightList[1][2] = TransString("");
        SetLabelText(pCurveInfoStrPage2RightList[1][2], pCurveInfoLblPage2RightList[1][2]);

        GuiMemFree(pCurveInfoStrPage2RightList[2][2]);
        pCurveInfoStrPage2RightList[2][2] = TransString("");
        SetLabelText(pCurveInfoStrPage2RightList[2][2], pCurveInfoLblPage2RightList[2][2]);

        GuiMemFree(pCurveInfoStrPage2RightList[3][2]);
        pCurveInfoStrPage2RightList[3][2] = TransString("");
        SetLabelText(pCurveInfoStrPage2RightList[3][2], pCurveInfoLblPage2RightList[3][2]);   
    }
    else
    {
        GuiMemFree(pCurveInfoStrPage1LeftList[1][2]);   
        if(curve1550Info->Events.iIsPass)
        {
            pCurveInfoStrPage1LeftList[1][2] = TransString("OTDR_PASS");
        }
        else
        {
            pCurveInfoStrPage1LeftList[1][2] = TransString("OTDR_FAIL");
        }
        SetLabelText(pCurveInfoStrPage1LeftList[1][2], pCurveInfoLblPage1LeftList[1][2]);

        GuiMemFree(pCurveInfoStrPage1LeftList[2][2]);
        float currentSpanLength_1550 = GetSpanLength(&curve1550Info->Events);
        pCurveInfoStrPage1LeftList[2][2] = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, 
            currentSpanLength_1550, 1);
        SetLabelText(pCurveInfoStrPage1LeftList[2][2], pCurveInfoLblPage1LeftList[2][2]);

        GuiMemFree(pCurveInfoStrPage1RightList[1][2]);
        memset(buff, 0, sizeof(buff));
        float totalLoss = GetSpanLoss(&curve1550Info->Events);        
        pCurveInfoStrPage1RightList[1][2] = Float2GuiStringUnit(LOSS_PRECISION, totalLoss, "dB");
        SetLabelText(pCurveInfoStrPage1RightList[1][2], pCurveInfoLblPage1RightList[1][2]);
        
        GuiMemFree(pCurveInfoStrPage1RightList[2][2]);
        memset(buff, 0, sizeof(buff));
        MARKER markerInfo_1550 = curve1550Info->mMarker;
        if(markerInfo_1550.fTotalReutrnLoss < RETURNLOSS_MAXVALUE)
        {
            pCurveInfoStrPage1RightList[2][2] = Float2GuiStringUnit(ORL_PRECISION, 
                markerInfo_1550.fTotalReutrnLoss, "dB");
        }
        else
        {
            pCurveInfoStrPage1RightList[2][2] = TransString("----");
        }
        SetLabelText(pCurveInfoStrPage1RightList[2][2], pCurveInfoLblPage1RightList[2][2]);

        GuiMemFree(pCurveInfoStrPage1LeftList[3][2]);
        memset(buff, 0, sizeof(buff));
        getCurveMeasureDate(buff, NULL, pCurSystemSet->uiTimeShowFlag);
        pCurveInfoStrPage1LeftList[3][2] = TransString(buff);
        SetLabelText(pCurveInfoStrPage1LeftList[3][2], pCurveInfoLblPage1LeftList[3][2]);

        GuiMemFree(pCurveInfoStrPage2LeftList[1][2]);
        switch(GetCurrSystemUnit(MODULE_UNIT_OTDR))
        {
            case UNIT_M:
            case UNIT_KM:
            pCurveInfoStrPage2LeftList[1][2] = TransString(RangeKm[curve1550Info->pParam.enRange]);
            break;
            case UNIT_FT:
            case UNIT_KFT:
            pCurveInfoStrPage2LeftList[1][2] = TransString(RangeKft[curve1550Info->pParam.enRange]);
            break;
            case UNIT_YD:
            case UNIT_MI:
            pCurveInfoStrPage2LeftList[1][2] = TransString(RangeMi[curve1550Info->pParam.enRange]);
            break;
            default:
            break;
        }
        SetLabelText(pCurveInfoStrPage2LeftList[1][2], pCurveInfoLblPage2LeftList[1][2]);

        GuiMemFree(pCurveInfoStrPage2LeftList[2][2]);
        pCurveInfoStrPage2LeftList[2][2] = TransString(StrPulse[curve1550Info->pParam.enPluse]);
        SetLabelText(pCurveInfoStrPage2LeftList[2][2], pCurveInfoLblPage2LeftList[2][2]);

        GuiMemFree(pCurveInfoStrPage2LeftList[3][2]);
        memset(buff, 0, sizeof(buff));;
        getCurveMeasureDate(NULL, buff, pCurSystemSet->uiTimeShowFlag);
        pCurveInfoStrPage2LeftList[3][2] = TransString(buff);
        SetLabelText(pCurveInfoStrPage2LeftList[3][2], pCurveInfoLblPage2LeftList[3][2]);

        GuiMemFree(pCurveInfoStrPage2RightList[1][2]);
        memset(buff, 0, sizeof(buff));
        if(curve1550Info->pParam.enTime == ENUM_AVG_TIME_AUTO || curve1550Info->pParam.enTime == ENUM_AVG_TIME_REALTIME)
        {
            sprintf(buff, "%s", AvgTime[curve1550Info->pParam.enTime]);
        }
        else
        {
            sprintf(buff, "%d sec", curve1550Info->pParam.uiAvgTime);
        }
        pCurveInfoStrPage2RightList[1][2] = TransString(buff);
        SetLabelText(pCurveInfoStrPage2RightList[1][2], pCurveInfoLblPage2RightList[1][2]);

        memset(buff, 0, sizeof(buff));
        GuiMemFree(pCurveInfoStrPage2RightList[2][2]);
        pCurveInfoStrPage2RightList[2][2] = Float2GuiString(IOR_PRECISION, 
            pUser_Settings->sSampleSetting.sWaveLenArgs[curve1550Info->pParam.enWave].fRefractiveIndex);
        SetLabelText(pCurveInfoStrPage2RightList[2][2], pCurveInfoLblPage2RightList[2][2]);

        memset(buff, 0, sizeof(buff));
        GuiMemFree(pCurveInfoStrPage2RightList[3][2]);
        pCurveInfoStrPage2RightList[3][2] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, 
            curve1550Info->pParam.fBackScatter);
        SetLabelText(pCurveInfoStrPage2RightList[3][2], pCurveInfoLblPage2RightList[3][2]);
    }
}

static void refreshSmallPageWaveInfo(PCURVE_INFO currentCurveInfo)
{
    char buff[512];
    if(currentCurveInfo->uiIsVaild)
    {
        GuiMemFree(pCurveInfoStrSmallPageList[0][1]);
        pCurveInfoStrSmallPageList[0][1] = TransString(StrWaveLenght[currentCurveInfo->pParam.enWave]);
        SetLabelText(pCurveInfoStrSmallPageList[0][1], pCurveInfoLblSmallPageList[0][1]);

        memset(buff, 0, sizeof(buff));
        GuiMemFree(pCurveInfoStrSmallPageList[0][3]);
        pCurveInfoStrSmallPageList[0][3] = Float2GuiString(IOR_PRECISION, 
            pUser_Settings->sSampleSetting.sWaveLenArgs[currentCurveInfo->pParam.enWave].fRefractiveIndex);
        SetLabelText(pCurveInfoStrSmallPageList[0][3], pCurveInfoLblSmallPageList[0][3]);
        
        GuiMemFree(pCurveInfoStrSmallPageList[1][1]);
        pCurveInfoStrSmallPageList[1][1] = TransString(StrPulse[currentCurveInfo->pParam.enPluse]);
        SetLabelText(pCurveInfoStrSmallPageList[1][1], pCurveInfoLblSmallPageList[1][1]);

        memset(buff, 0, sizeof(buff));
        GuiMemFree(pCurveInfoStrSmallPageList[1][3]);
        pCurveInfoStrSmallPageList[1][3] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, 
            currentCurveInfo->pParam.fBackScatter);
        SetLabelText(pCurveInfoStrSmallPageList[0][3], pCurveInfoLblSmallPageList[0][3]);
        SetLabelText(pCurveInfoStrSmallPageList[1][3], pCurveInfoLblSmallPageList[1][3]);
        
        GuiMemFree(pCurveInfoStrSmallPageList[2][1]);
        memset(buff, 0, sizeof(buff));
        if(currentCurveInfo->pParam.enTime == ENUM_AVG_TIME_AUTO || currentCurveInfo->pParam.enTime == ENUM_AVG_TIME_REALTIME)
        {
            sprintf(buff, "%s", AvgTime[currentCurveInfo->pParam.enTime]);
        }
        else
        {
            sprintf(buff, "%d sec", currentCurveInfo->pParam.uiAvgTime);
        }
        pCurveInfoStrSmallPageList[2][1] = TransString(buff);
        SetLabelText(pCurveInfoStrSmallPageList[2][1], pCurveInfoLblSmallPageList[2][1]);
        
        GuiMemFree(pCurveInfoStrSmallPageList[2][3]);
        memset(buff, 0, sizeof(buff));
        snprintf(buff, sizeof(buff), "%s", cTmp[pCommon_Setting->iLossMethodMark]);
        pCurveInfoStrSmallPageList[2][3] = TransString(buff);
        SetLabelText(pCurveInfoStrSmallPageList[2][3], pCurveInfoLblSmallPageList[2][3]);
    }
}
