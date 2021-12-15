 /*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcommonset.c
* 摘    要：  实现主窗体frmcommonset的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者： wjg 
* 完成日期：2020/9/5
*
*******************************************************************************/

#include "wnd_frmcommonset.h"

/*************************************
* 定义wnd_frmcommonset.c引用其他头文件
*************************************/
#include <stdio.h>

#include "app_global.h"
#include "app_frminit.h"
#include "app_getsetparameter.h"
#include "app_frmotdr.h"
#include "app_systemsettings.h"
#include "app_unitconverter.h"

#include "wnd_frmsampleset.h"
#include "wnd_frmanalysset.h"
#include "wnd_frmotdrmeas.h"

#include "guipicture.h"

#include "wnd_global.h"
#include "wnd_frmime.h"
#include "wnd_stack.h"
#include "wnd_frmotdranalysis.h"
#include "wnd_frmthresholdset.h"
#include "wnd_frmreportset.h"
#include "wnd_droppicker.h"
#include "wnd_frmotdr.h"
#include "wnd_popselector.h"


/*************************************
* 定义wnd_frmcommonset.c中内部数据结构
*************************************/
#define COMMONSET_TITLE_NUM         4
#define MAX_APP_OPTION              5
#define MAX_TEST_OPTION_NUM         6           //最大测试项
#define DEFAULT_VALE_NUM            3           //默认值个数
#define TEST_MODE_NUM               2           //测试模式个数
#define RANGE_NUM                   10          //量程个数
#define PULSE_NUM                   9           //脉宽个数
#define TEST_TIME_NUM               7           //测试时长个数
#define DIST_UNIT_NUM               5           //距离单位个数

enum Test_Set_Option
{
    TEST_MODE = 0,
    RANGE,
    PULSE,
    TEST_TIME,
    DIST_UNIT,
    WAVE_LENGTH
};

typedef struct Test_Set_Control
{
    GUICHAR* pStrTitle[MAX_TEST_OPTION_NUM];
    GUICHAR* pStrData[MAX_TEST_OPTION_NUM];

    GUILABEL* pLblTitle[MAX_TEST_OPTION_NUM];
    GUILABEL* pLblData[MAX_TEST_OPTION_NUM];

    GUIPICTURE *pBtnIcon[MAX_TEST_OPTION_NUM];
    GUIPICTURE* pBtnEnterIcon[MAX_TEST_OPTION_NUM];
    GUIPICTURE* pBtnSelect[MAX_TEST_OPTION_NUM];
}TEST_SET_CONTROL;

//图标背景图
char* pStrOtdrSetIcon_unpress[MAX_TEST_OPTION_NUM] =
{
	BmpFileDirectory"btn_testMode_unpress.bmp",
	BmpFileDirectory"btn_range_unpress.bmp",
	BmpFileDirectory"btn_pulse_unpress.bmp",
	BmpFileDirectory"btn_time_unpress.bmp",
	BmpFileDirectory"btn_unit_unpress.bmp",
	BmpFileDirectory"btn_wave_unpress.bmp"
};

char* pStrOtdrSetIcon_press[MAX_TEST_OPTION_NUM] =
{
	BmpFileDirectory"btn_testMode_press.bmp",
	BmpFileDirectory"btn_range_press.bmp",
	BmpFileDirectory"btn_pulse_press.bmp",
	BmpFileDirectory"btn_time_press.bmp",
	BmpFileDirectory"btn_unit_press.bmp",
	BmpFileDirectory"btn_wave_press.bmp"
};

typedef struct OtdrTestSetIndex
{
    int testModeIndex;
    int rangeIndex;
    int pulseIndex;
    int testTimeIndex;
    int distUnitIndex;              //距离单位的index
    int waveLengthIndex;
}OTDR_TESET_SET_INDEX;

static int pTestMode[TEST_MODE_NUM] = {OTDR_SET_LBL_RT, OTDR_SET_LBL_AVERAGE};
static char* pRange[RANGE_NUM] = {NULL};
static char* pPulse[PULSE_NUM] = {NULL};
static char* pTestTime[TEST_TIME_NUM] = {"Auto", "15s", "30s", "60s", "90s", "120s", "180s"};
static char* pDistUnit[DIST_UNIT_NUM] = {"m", "km", "ft", "kft", "mile"};
static char* pWaveLength[OTDR_WAVE_LENGTH_NUM] = {"1310nm", "1550nm", "1310+1550nm"};
//量程(单位是km)
static float iRange[RANGE_NUM] = {0, 0.5, 1, 2, 5, 10, 20, 50, 100, 200};
//量程对应脉宽（单位是ns）
static int iRangePulse[RANGE_NUM][PULSE_NUM] = 
{
    {0, 5, 10, 20, 50, 100, 200, 500, 1000},//auto
    {0, 5, 10, 20, 50, 100, -1, -1, -1},
    {0, 5, 10, 20, 50, 100, 200, -1, -1},
    {0, 5, 10, 20, 50, 100, 200, -1, -1},
    {0, 5, 10, 20, 50, 100, 200, 500, -1},
    {0, 5, 10, 20, 50, 100, 200, 500, 1000},
    {0, 5, 10, 20, 50, 100, 200, 500, 1000},
    {0, 20, 50, 100, 200, 500, 1000, 2000, 10000},
    {0, 50, 100, 200, 500, 1000, 2000, 10000, 20000},
    {0, 50, 100, 200, 500, 1000, 2000, 10000, 20000}
};

//自动量程下量程与脉宽的对应表(索引值)
static int iRangePulseIndex[RANGE_NUM] = {0, 20, 50, 100, 100, 200, 500, 1000, 10000, 20000};
//记录上次选中的脉宽值
static int iPulseValue = 0;
//记录平均模式下选择的时长下标值
static int iTimeIndex = 1;//默认选择15s
//记录是否选中自动模式
static int iSelectedAuto = 0;//默认不选中
/***********************************
* 定义wnd_frmcommonset.c中的窗体资源
***********************************/
static GUIWINDOW *pFrmCommonSet = NULL;

/*********************************** 
* 定义wnd_frmcommonset.c中的文本资源
***********************************/
static GUICHAR *pCmnSetSetStrTitle = NULL;									//测试设置
static GUICHAR *pCmnSetStrTestContent = NULL;								//进行OTDR测试设定
static GUICHAR *pCmnSetStrOptionTitle[COMMONSET_TITLE_NUM] = { NULL };		//测试、阈值、报表

/***********************************
* 定义wnd_frmcommonset.c中的控件资源
***********************************/
static GUIPICTURE *pCmnSetBg = NULL;
static GUIPICTURE *pCmnSetBgIcon = NULL;
static GUIPICTURE *pCmnSetBtnBack = NULL;									//返回按钮
static GUIPICTURE *pCmnSetBtnTitle[COMMONSET_TITLE_NUM] = {NULL};			//测试设置，阈值设置，报表设置
/***********************************
* 定义wnd_frmcommonset.c中的标签资源
***********************************/
static GUILABEL *pCmnSetLblTitle = NULL;
static GUILABEL *pCmnSetLblTestContent = NULL;
static GUILABEL *pCmnSetLblOptionTitle[COMMONSET_TITLE_NUM] = {NULL};

/***********************************
* 定义wnd_frmcommonset.c中的菜单控件
***********************************/
static TEST_SET_CONTROL* pTestSetControl = NULL;
static int iSelectedOption = 0;									//默认选中第一个选项
static OTDR_TESET_SET_INDEX *pOtdrTestSetIndex = NULL;
/***********************************
* 声明wnd_frmcommonset.c中的内部函数
***********************************/
//初始化参数
static int CommonInitPara(void);
//清理参数
static int CommonClearPara(void);
//初始化文本
static int FrmCommonSetRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//清理文本
static int FrmCommonSetRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//点击阈值设置文本响应函数
static int CmnLblOptionTitle_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CmnLblOptionTitle_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//点击返回按钮响应函数
static int CmnBtnBack_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CmnBtnBack_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//点击设置按钮响应函数
static int CmnBtnIcon_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CmnBtnIcon_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//点击按键响应函数
static int WndCommonSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndCommonSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//获取脉宽索引值（用于算法）
static int GetPulseIndex(int pulseTime);
//获取脉宽显示的个数
static int GetPulseCount(int rangeIndex);
//设置脉宽索引值
static void SetPulseIndex(int pulseTime, int enPulseTime);
//显示otdr测试设置后的信息
static void DisplayOtdrTestSetInfo(void);
//刷新测试选项内容
static void FlushTestOptionInfo(void);
//设置量程显示信息(根据距离单位设置)
static void SetRangeInfo(int iDistUnitIndex);
//设置脉宽显示信息(根据量程设置)
static void SetPulseInfo(int iRangeIndex);
//选择测试模式后的回调函数
static void TestModeDropCallBack(int iSelected);
//选择量程后的回调函数
static void RangeDropCallBack(int iSelected);
//选择脉宽后的回调函数
static void PulseDropCallBack(int iSelected);
//选择测试时长后的回调函数
static void TestTimeDropCallBack(int iSelected);
//选择距离单位后的回调函数
static void DistUnitDropCallBack(int iSelected);
//选择波长后的回调函数
static void WaveLengthDropCallBack(int iSelected);
//从系统中获取测试参数
static void GetTestParaFromSystem(void);
//保存测试参数到系统中
static void SaveTestParaToSystem(void);

/***
  * 功能：
        窗体的初始化函数，建立窗体控件、注册消息处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmCommonSetInit(void *pWndObj)
{
	//错误标志，返回值定义
    int iRet = 0;
	//临时变量定义
	int i = 0;
	//初始化参数
	CommonInitPara();
    FrmCommonSetRes_Init(NULL, 0, NULL, 0);
	//建立窗体控件
    pCmnSetBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                              BmpFileDirectory"bg_otdr_test_set.bmp");
    pCmnSetBgIcon = CreatePicture(0, 0, 23, 20,
                                  BmpFileDirectory"bg_test_set_icon.bmp"); //待定
    pCmnSetBtnBack = CreatePicture(0, 20, 36, 21,
                                   BmpFileDirectory"btn_otdrSet_back.bmp");
    //桌面上的标签 
	pCmnSetLblTitle = CreateLabel(23, 2, 100, 16, pCmnSetSetStrTitle);
	pCmnSetLblTestContent = CreateLabel(8, 56, 200, 16, pCmnSetStrTestContent);
    
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pCmnSetLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCmnSetLblTestContent);

    for(i = 0; i < MAX_TEST_OPTION_NUM; ++i)
    {
        if (i < COMMONSET_TITLE_NUM)
        {
            pCmnSetBtnTitle[i] = CreatePicture(232 + 102 * i, 47, 99, 35,
                                               pStrOtdrSetButton_unpress[i]);
            pCmnSetLblOptionTitle[i] = CreateLabel(232 + 102 * i+5, 47+9, 90, 16,
                                                   pCmnSetStrOptionTitle[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCmnSetLblOptionTitle[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pCmnSetLblOptionTitle[i]);
        }

        pTestSetControl->pBtnIcon[i] = CreatePicture(45, 95 + i * 45, 20, 20,
                                                     pStrOtdrSetIcon_unpress[i]);
        pTestSetControl->pLblTitle[i] = CreateLabel(75, 97 + i * 45, 120, 16,
                                                        pTestSetControl->pStrTitle[i]);
        pTestSetControl->pLblData[i] = CreateLabel(450, 97 + i * 45, 130, 16,
                                                        pTestSetControl->pStrData[i]);
        
        pTestSetControl->pBtnEnterIcon[i] = CreatePicture(586, 97 + i * 45, 16, 16, 
                                                        BmpFileDirectory"bg_sys_arrow.bmp");
		pTestSetControl->pBtnSelect[i] = CreatePicture(1, 83 + i * 45, 637, 44, 
                                                        BmpFileDirectory"bg_sys_unselect.bmp");

        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTestSetControl->pLblTitle[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pTestSetControl->pLblData[i]);
    }
    		
     //得到当前窗体对象
     pFrmCommonSet = (GUIWINDOW *)pWndObj;
	// 注册窗体控件，只对那些需要接收事件的控件进行
    // 即如果该控件没有或者不需要响应输入事件，可以无需注册
    // 注册窗体控件必须在注册消息处理函数之前进行

	//注册窗体(因为所有的按键事件都统一由窗体进行处理) 
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmCommonSet, pFrmCommonSet);
    
    for(i = 0; i < MAX_TEST_OPTION_NUM; ++i)
    {
		if (i < COMMONSET_TITLE_NUM)
		{
            AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
                          pCmnSetLblOptionTitle[i], pFrmCommonSet);
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                          pCmnSetBtnTitle[i], pFrmCommonSet);
        }

        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                      pTestSetControl->pBtnEnterIcon[i], pFrmCommonSet);
    }

    // 注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    // 即此处的操作应当在注册窗体控件的基础上进行
    // 注册消息处理函数必须在注册窗体控件之后进行
    // 必须在持有消息队列的互斥锁情况下操作
    
    GUIMESSAGE *pMsg = NULL;
    pMsg = GetCurrMessage();
    
    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCmnSetBtnBack, CmnBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pCmnSetBtnBack, CmnBtnBack_Up, NULL, 0, pMsg);

    for(i = 0; i < MAX_TEST_OPTION_NUM; ++i)
    {
		if (i < COMMONSET_TITLE_NUM)
		{
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCmnSetLblOptionTitle[i],
                            CmnLblOptionTitle_Down, NULL, i, pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pCmnSetLblOptionTitle[i],
                            CmnLblOptionTitle_Up, NULL, i, pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCmnSetBtnTitle[i],
                            CmnLblOptionTitle_Down, NULL, i, pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pCmnSetBtnTitle[i],
                            CmnLblOptionTitle_Up, NULL, i, pMsg);
        }

        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pTestSetControl->pBtnEnterIcon[i], 
                        	CmnBtnIcon_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pTestSetControl->pBtnEnterIcon[i], 
	                	    CmnBtnIcon_Up, NULL, i, pMsg);
    }

    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmCommonSet,
                    WndCommonSetKey_Down, pTestSetControl, 0, GetCurrMessage());
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmCommonSet,
                    WndCommonSetKey_Up, pTestSetControl, 0, GetCurrMessage());

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
int FrmCommonSetExit(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
	int i = 0;

	//得到当前窗体对象
	pFrmCommonSet = (GUIWINDOW *)pWndObj;

	// 清空消息队列中的消息注册项
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //从当前窗体中注销窗体控件
    ClearWindowComp(pFrmCommonSet);

	//销毁控件 
	//桌面上的控件 
	DestroyPicture(&pCmnSetBg);
	DestroyPicture(&pCmnSetBgIcon);
    DestroyPicture(& pCmnSetBtnBack);   
	//桌面上的标签 
	DestroyLabel(&pCmnSetLblTitle);    
    DestroyLabel(&pCmnSetLblTestContent);

    for(i = 0; i < MAX_TEST_OPTION_NUM; ++i)
    {
		if (i < COMMONSET_TITLE_NUM)
		{
			DestroyPicture(&pCmnSetBtnTitle[i]);
			DestroyLabel(&pCmnSetLblOptionTitle[i]);
		}

        DestroyPicture(&(pTestSetControl->pBtnSelect[i]));
        DestroyLabel(&(pTestSetControl->pLblTitle[i]));
        DestroyLabel(&(pTestSetControl->pLblData[i]));
        DestroyPicture(&(pTestSetControl->pBtnIcon[i]));
        DestroyPicture(&(pTestSetControl->pBtnEnterIcon[i]));
    }

	//释放文本资源 
    FrmCommonSetRes_Exit(NULL, 0, NULL, 0);
	//清理系统相关参数 
	CommonClearPara();
#ifdef EEPROM_DATA
	// 保存系统设置参数
	SetSettingsData((void*)&pUser_Settings->sCommonSetting, sizeof(COMMON_SETTING), COMMON_SET);
    SaveSettings(COMMON_SET);
	SetSettingsData((void*)&pUser_Settings->sFrontSetting, sizeof(FRONT_SETTING), FRONTSET_1310);
	SaveSettings(FRONTSET_1310);
	SetSettingsData((void*)&pUser_Settings->sFrontSetting, sizeof(FRONT_SETTING), FRONTSET_1550);
	SaveSettings(FRONTSET_1550);
	SetSettingsData((void*)&pUser_Settings->sFrontSetting, sizeof(FRONT_SETTING), FRONTSET_1625);
	SaveSettings(FRONTSET_1625);
#endif
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
int FrmCommonSetPaint(void *pWndObj)
{
    //错误标志、返回值定义 
    int iRet = 0;
    
	DisplayPicture(pCmnSetBg);
	// DisplayPicture(pCmnSetBgIcon);
    // DisplayPicture(pCmnSetBtnBack);

    // DisplayLabel(pCmnSetLblTitle);
    DisplayLabel(pCmnSetLblTestContent);
    
    DisplayOtdrTestSetInfo();
    
	int i = 0;
    for(i = 0; i < MAX_TEST_OPTION_NUM; ++i)
    {
		//设置标题栏
		if (i < COMMONSET_TITLE_NUM)
		{
			if (i == TEST_SET)
			{
				SetPictureBitmap(pStrOtdrSetButton_press[TEST_SET], pCmnSetBtnTitle[i]);
			}

			DisplayPicture(pCmnSetBtnTitle[i]);
			DisplayLabel(pCmnSetLblOptionTitle[i]);
		}
		//设置选中项
		if (iSelectedOption == i)
		{
            SetPictureBitmap(pStrOtdrSetIcon_press[i], pTestSetControl->pBtnIcon[i]);
            SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pTestSetControl->pBtnSelect[i]);
			SetPictureBitmap(BmpFileDirectory"bg_sys_arrow_press.bmp", pTestSetControl->pBtnEnterIcon[i]);
		}
		else
		{
            SetPictureBitmap(pStrOtdrSetIcon_unpress[i], pTestSetControl->pBtnIcon[i]);
            SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pTestSetControl->pBtnSelect[i]);
			SetPictureBitmap(BmpFileDirectory"bg_sys_arrow.bmp", pTestSetControl->pBtnEnterIcon[i]);
		}
		//显示控件信息
        DisplayPicture(pTestSetControl->pBtnSelect[i]);
        DisplayLabel(pTestSetControl->pLblTitle[i]);
        DisplayLabel(pTestSetControl->pLblData[i]);
        DisplayPicture(pTestSetControl->pBtnIcon[i]);
        DisplayPicture(pTestSetControl->pBtnEnterIcon[i]);
    }
    
	SetPowerEnable(1, 1);
	
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
int FrmCommonSetLoop(void *pWndObj)
{
	//错误标志、返回值定义 
    int iRet = 0;
	SendWndMsg_LoopDisable(pWndObj);
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
int FrmCommonSetPause(void *pWndObj)
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
int FrmCommonSetResume(void *pWndObj)
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
static int FrmCommonSetRes_Init(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义 
    int iRet = 0;

	//桌面上的文本 
	pCmnSetSetStrTitle = GetCurrLanguageText(OTDR_SET_LBL_TEST);
	pCmnSetStrTestContent = GetCurrLanguageText(OTDR_SET_LBL_MEASURE);

	pCmnSetStrOptionTitle[TEST_SET] = GetCurrLanguageText(OTDR_SET_LBL_TEST);
	pCmnSetStrOptionTitle[OTDR_ANALYSIS_SET] = GetCurrLanguageText(OTDR_SET_LBL_ANALYSIS);
	pCmnSetStrOptionTitle[THRESHOLD_SET] = GetCurrLanguageText(OTDR_SET_LBL_THRESHOLD);
	pCmnSetStrOptionTitle[REPORT_SET] = GetCurrLanguageText(OTDR_SET_LBL_REPORT);

    unsigned testTitle[MAX_TEST_OPTION_NUM] = 
    {
        OTDR_SET_LBL_TEST_MODE, OTDR_SET_LBL_RANGE, OTDR_SET_LBL_PULSE,
		OTDR_SET_LBL_TEST_TIME, OTDR_SET_LBL_DISTANCE_UNIT, OTDR_SET_LBL_WAVE_LENGTH
    };
    char* testData[MAX_TEST_OPTION_NUM] = 
    {
        "AUTO","5KM","20ns","30s",
        "m","1310nm"
    };    
    int i;
    for(i = 0; i < MAX_TEST_OPTION_NUM; ++i)
    {
        pTestSetControl->pStrTitle[i] = GetCurrLanguageText(testTitle[i]);
        pTestSetControl->pStrData[i] = TransString(testData[i]);
    }
    
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
static int FrmCommonSetRes_Exit(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    int iRet = 0;
	int i = 0;
	
	//桌面上的文本
    GuiMemFree(pCmnSetSetStrTitle);    

    for(i = 0; i < MAX_TEST_OPTION_NUM; ++i)
    {
		if (i < COMMONSET_TITLE_NUM)
		{
			GuiMemFree(pCmnSetStrOptionTitle[i]);
		}
        
        GuiMemFree(pTestSetControl->pStrTitle[i]);
        GuiMemFree(pTestSetControl->pStrData[i]);
    }

	return iRet;
}

//标题栏按下响应处理
int CmnLblOptionTitle_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	return 0;
}

int CmnLblOptionTitle_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = NULL;
	switch (iOutLen)
	{
	case OTDR_ANALYSIS_SET:
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmOtdrAnalysisSetInit, FrmOtdrAnalysisSetExit,
			FrmOtdrAnalysisSetPaint, FrmOtdrAnalysisSetLoop,
			FrmOtdrAnalysisSetPause, FrmOtdrAnalysisSetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmCommonSet);
		SendSysMsg_ThreadCreate(pWnd);
	}
		break;
	case THRESHOLD_SET:
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmThresholdSetInit, FrmThresholdSetExit,
			FrmThresholdSetPaint, FrmThresholdSetLoop,
			FrmThresholdSetPause, FrmThresholdSetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmCommonSet);
		SendSysMsg_ThreadCreate(pWnd);
	}
		break;
	case REPORT_SET:
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmReportSetInit, FrmReportSetExit,
			FrmReportSetPaint, FrmReportSetLoop,
			FrmReportSetPause, FrmReportSetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmCommonSet);
		SendSysMsg_ThreadCreate(pWnd);
	}
		break;
	default:
		break;
	}

	return 0;
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
static int CommonInitPara(void)
{
	int i = 0;
		
	//读取全局变量并将其值存入暂存区域以便于设置操作
    //申请内存
    if(pTestSetControl== NULL)
    {
        pTestSetControl= (TEST_SET_CONTROL*)calloc(1, sizeof(TEST_SET_CONTROL));
        memset(pTestSetControl, 0, sizeof(TEST_SET_CONTROL));
    }

    if(pOtdrTestSetIndex== NULL)
    {
        pOtdrTestSetIndex= (OTDR_TESET_SET_INDEX*)calloc(1, sizeof(OTDR_TESET_SET_INDEX));
        memset(pOtdrTestSetIndex, 0, sizeof(OTDR_TESET_SET_INDEX));
    }

    //range
    for(i = 0; i < RANGE_NUM; ++i)
    {
        pRange[i] = (char*)calloc(1, sizeof(char*)*RANGE_NUM);
        memset(pRange[i], 0, sizeof(char*)*RANGE_NUM);
    }
    //pulse
    for(i = 0; i < PULSE_NUM; ++i)
    {
        pPulse[i] = (char*)calloc(1, sizeof(char*)*PULSE_NUM);
        memset(pPulse[i], 0, sizeof(char*)*PULSE_NUM);
    }

    //单位
    pOtdrTestSetIndex->distUnitIndex = pUser_Settings->sCommonSetting.iUnitConverterFlag;
	//从系统中获取测试参数
    GetTestParaFromSystem();
    
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
static int CommonClearPara(void)
{
	int i = 0;
	//将设置后的新参数写入全局的数据结构中
	//必须持有互斥锁
	CODER_LOG(CoderYun, "Saving Commonset to system...\n");
    //释放所分配的空间
	GuiMemFree(pTestSetControl);
    //range
    for(i = 0; i < RANGE_NUM; ++i)
    {
        GuiMemFree(pRange[i]);
    }
    //pulse
    for(i = 0; i < PULSE_NUM; ++i)
    {
        GuiMemFree(pPulse[i]);
    }
	//dist_unit
    pUser_Settings->sCommonSetting.iUnitConverterFlag = pOtdrTestSetIndex->distUnitIndex;
    pUser_Settings->sOtherSetting.iWaveLength = pOtdrTestSetIndex->waveLengthIndex;
    SaveTestParaToSystem();

	return 0;
}

//点击返回按钮响应函数
static int CmnBtnBack_Down(void *pInArg, int iInLen, 
                  void *pOutArg, int iOutLen)
{
    return 0;
}
                  
static int CmnBtnBack_Up(void *pInArg, int iInLen, 
                void *pOutArg, int iOutLen)
{
	Stack *ps = NULL;
	Item func;
    ps = GetCurrWndStack();
    WndPop(ps, &func, pFrmCommonSet);

    return 0;
}

//点击设置按钮响应函数
static int CmnBtnIcon_Down(void *pInArg, int iInLen, 
                      void *pOutArg, int iOutLen)
{
    return 0;
}
static int CmnBtnIcon_Up(void *pInArg, int iInLen, 
                    void *pOutArg, int iOutLen)
{
    int option = iOutLen;
    switch(option)
    {
        case TEST_MODE:
            {
				//char* pTestMode[] = {"Auto", "Expert"};
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmCommonSet);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                CreatePopSelector(120, TEST_MODE_NUM, NULL, pTestMode, pOtdrTestSetIndex->testModeIndex,
                                  TestModeDropCallBack, pFrmCommonSet);
            }
            break;
        case RANGE:
            {
                SetRangeInfo(pOtdrTestSetIndex->distUnitIndex);
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmCommonSet);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                CreatePopSelector(120, RANGE_NUM, pRange, NULL,
                                  pOtdrTestSetIndex->rangeIndex, RangeDropCallBack, pFrmCommonSet);
            }
            break;
        case PULSE:
            {
				int count = GetPulseCount(pOtdrTestSetIndex->rangeIndex);
                SetPulseInfo(pOtdrTestSetIndex->rangeIndex);
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmCommonSet);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                CreatePopSelector(120, count, pPulse, NULL,
                                  pOtdrTestSetIndex->pulseIndex, PulseDropCallBack, pFrmCommonSet);
            }
            break;
        case TEST_TIME:
            {
                //只有在平均模式下才可设置时长
                if (pOtdrTestSetIndex->testModeIndex == ENUM_AVG_MODE)
                {
                    //解决下拉列表出现时插拔usb产生界面重叠问题
                    SendWndMsg_LoopDisable(pFrmCommonSet);
                    DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                    CreatePopSelector(120, TEST_TIME_NUM, pTestTime, NULL,
                                      pOtdrTestSetIndex->testTimeIndex, TestTimeDropCallBack, pFrmCommonSet);
                }
            }
            break;
        case DIST_UNIT:
            {
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmCommonSet);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                CreatePopSelector(120, DIST_UNIT_NUM, pDistUnit, NULL,
                                  pOtdrTestSetIndex->distUnitIndex, DistUnitDropCallBack, pFrmCommonSet);
            }
            break;
        case WAVE_LENGTH:
            {
                //解决下拉列表出现时插拔usb产生界面重叠问题  
                SendWndMsg_LoopDisable(pFrmCommonSet);
                DispTransparent(80, 0x0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                CreatePopSelector(120, OTDR_WAVE_LENGTH_NUM, pWaveLength, NULL,
                                  pOtdrTestSetIndex->waveLengthIndex, WaveLengthDropCallBack, pFrmCommonSet);
            }
            break;
        default:
            break;
    }
    
    return 0;
}

//按键响应函数
int WndCommonSetKey_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	return 0;
}

int WndCommonSetKey_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	unsigned int uiValue;
	uiValue = (unsigned int)pInArg;

    switch (uiValue)
    {
    case KEYCODE_OTDR_RIGHT:
        CmnLblOptionTitle_Up(NULL, 0, NULL, OTDR_ANALYSIS_SET);
        break;
    case KEYCODE_UP:
        if (iSelectedOption != 0)
        {
            iSelectedOption--;
        }
        FlushTestOptionInfo();
        break;
    case KEYCODE_DOWN:
        if (iSelectedOption != (MAX_TEST_OPTION_NUM - 1))
        {
            iSelectedOption++;
        }
        FlushTestOptionInfo();
        break;
    case KEYCODE_LEFT:
        break;
    case KEYCODE_RIGHT:
        break;
    case KEYCODE_START:
        //先退出界面
        CmnBtnBack_Up(NULL, 0, NULL, 0);
        //执行测试操作
        OtdrTest(0);
        break;
    case KEYCODE_ENTER:
        CmnBtnIcon_Up(NULL, 0, NULL, iSelectedOption);
        break;
    case KEYCODE_ESC:
        CmnBtnBack_Up(NULL, 0, NULL, 0);
        break;
    default:
        break;
    }

    return 0;
}

/***
  * 功能：
		获取脉宽index
  * 参数：
		int pulseTime:实际脉宽数值
  * 返回：
		无
  * 备注：用于算法
***/
static int GetPulseIndex(int pulseTime)
{
	int pulseIndex = 0;
	int array[12] = {0, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
	int i;

	for (i = 0; i < 12; ++i)
	{
		if (array[i] == pulseTime)
		{
			pulseIndex = i;
			break;
		}
	}

	return pulseIndex;
}

/***
  * 功能：
		获取脉宽显示的个数
  * 参数：
		int rangeIndex:量程index
  * 返回：
		无
  * 备注：用于显示下拉框脉宽的个数
***/
static int GetPulseCount(int rangeIndex)
{
	int i;
	int count = 0;

	for (i = 0; i < PULSE_NUM; ++i)
	{
		if (iRangePulse[rangeIndex][i] != -1)
		{
			count++;
		}
	}

	return count;
}

/***
  * 功能：
		设置显示的脉宽index
  * 参数：
		1.int rangeIndex:量程index
		2.int enPulseIndex:算法传入的index
  * 返回：
		无
  * 备注：用于算法
***/
void SetPulseIndex(int rangeIndex, int enPulseIndex)
{
	int pulseIndex = 0;
	int array[12] = { 0, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000 };
	int i;

	for (i = 0; i < PULSE_NUM; ++i)
	{
		if (iRangePulse[rangeIndex][i] != -1)
		{
			if (iRangePulse[rangeIndex][i] == array[enPulseIndex])
			{
				pulseIndex = i;
				break;
			}
		}
	}

	pOtdrTestSetIndex->pulseIndex = pulseIndex;
	//存储脉宽值
    if (!iPulseValue)
        iPulseValue = array[enPulseIndex];
}

/***
  * 功能：
        显示otdr测试设置后的信息
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void DisplayOtdrTestSetInfo(void)
{
    //测试模式
    pTestSetControl->pStrData[TEST_MODE] = GetCurrLanguageText(pTestMode[pOtdrTestSetIndex->testModeIndex]);
    SetLabelText(pTestSetControl->pStrData[TEST_MODE], pTestSetControl->pLblData[TEST_MODE]);
    //量程
    SetRangeInfo(pOtdrTestSetIndex->distUnitIndex);
    pTestSetControl->pStrData[RANGE] = TransString(pRange[pOtdrTestSetIndex->rangeIndex]);
    SetLabelText(pTestSetControl->pStrData[RANGE], pTestSetControl->pLblData[RANGE]);
    //脉宽
    SetPulseInfo(pOtdrTestSetIndex->rangeIndex);
    pTestSetControl->pStrData[PULSE] = TransString(pPulse[pOtdrTestSetIndex->pulseIndex]);
    SetLabelText(pTestSetControl->pStrData[PULSE], pTestSetControl->pLblData[PULSE]);
    //测试时间
    //实时模式下测试时长设置为RT
    if (pOtdrTestSetIndex->testModeIndex == ENUM_RT_MODE) //RT
    {
        pOtdrTestSetIndex->testTimeIndex = ENUM_AVG_TIME_REALTIME;
        pTestSetControl->pStrData[TEST_TIME] = TransString("RT");
    }
    else
    {
        //时长选项可选
        pOtdrTestSetIndex->testTimeIndex = iTimeIndex;
        pTestSetControl->pStrData[TEST_TIME] = TransString(pTestTime[pOtdrTestSetIndex->testTimeIndex]);
    }
    
    SetLabelText(pTestSetControl->pStrData[TEST_TIME], pTestSetControl->pLblData[TEST_TIME]);
    //距离单位
    pTestSetControl->pStrData[DIST_UNIT] = TransString(pDistUnit[pOtdrTestSetIndex->distUnitIndex]);
    SetLabelText(pTestSetControl->pStrData[DIST_UNIT], pTestSetControl->pLblData[DIST_UNIT]);
    //波长
    pTestSetControl->pStrData[WAVE_LENGTH] = TransString(pWaveLength[pOtdrTestSetIndex->waveLengthIndex]);
    SetLabelText(pTestSetControl->pStrData[WAVE_LENGTH], pTestSetControl->pLblData[WAVE_LENGTH]);
}

//刷新测试选项内容
static void FlushTestOptionInfo(void)
{
	int i;
	for (i = 0; i < MAX_TEST_OPTION_NUM; ++i)
	{
        SetPictureBitmap(pStrOtdrSetIcon_unpress[i], pTestSetControl->pBtnIcon[i]);
        SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pTestSetControl->pBtnSelect[i]);
		SetPictureBitmap(BmpFileDirectory"bg_sys_arrow.bmp", pTestSetControl->pBtnEnterIcon[i]);
	}

    SetPictureBitmap(pStrOtdrSetIcon_press[iSelectedOption], pTestSetControl->pBtnIcon[iSelectedOption]);
    SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pTestSetControl->pBtnSelect[iSelectedOption]);
	SetPictureBitmap(BmpFileDirectory"bg_sys_arrow_press.bmp", pTestSetControl->pBtnEnterIcon[iSelectedOption]);

	for (i = 0; i < MAX_TEST_OPTION_NUM; ++i)
	{
		DisplayPicture(pTestSetControl->pBtnSelect[i]);
		DisplayLabel(pTestSetControl->pLblTitle[i]);
		DisplayLabel(pTestSetControl->pLblData[i]);
        DisplayPicture(pTestSetControl->pBtnIcon[i]);
        DisplayPicture(pTestSetControl->pBtnEnterIcon[i]);
    }

	RefreshScreen(__FILE__, __func__, __LINE__);
}

/***
  * 功能：
        设置量程显示信息(根据距离单位设置)
  * 参数：
        int iDistUnitIndex:距离单位的下标
  * 返回：
        无
  * 备注：
***/
static void SetRangeInfo(int iDistUnitIndex)
{
    char* tempRange = NULL;
    strcpy(pRange[0], "Auto");
    //自动脉宽下，量程变成自动
    if (iSelectedAuto)
    {
        pOtdrTestSetIndex->rangeIndex = ENUM_FIBER_RANGE_AUTO;
        pOtdrTestSetIndex->pulseIndex = ENUM_PULSE_AUTO;
    }
    int i;
    for(i = 1; i < RANGE_NUM; ++i)
    {
        memset(pRange[i], 0, sizeof(char*)*RANGE_NUM);
        double data = UnitConverter_Dist_Float2Float(UNIT_KM, iDistUnitIndex, iRange[i]);
        tempRange = Float2String(PRECISION_1,  data);
        sprintf(pRange[i], "%s%s", tempRange, pDistUnit[iDistUnitIndex]);
        memset(tempRange, 0, sizeof(tempRange));
    }
    
    GuiMemFree(tempRange);
}


/***
  * 功能：
        设置脉宽显示信息(根据量程设置)
  * 参数：
        int iRangeIndex：量程的下标
  * 返回：
        无
  * 备注：
***/
static void SetPulseInfo(int iRangeIndex)
{
    char tempPulse[PULSE_NUM] = {0};
    int i = 0;

    for(i = 0; i < PULSE_NUM; ++i)
    {
        memset(pPulse[i], 0, sizeof(char*)*PULSE_NUM);
        memset(tempPulse, 0, PULSE_NUM);
        if(iRangePulse[iRangeIndex][i] != -1)
        {
            if(iRangePulse[iRangeIndex][i] == 0)
            {
                strcpy(pPulse[i], "Auto");
            }
            else
            {
                sprintf(tempPulse, "%dns", iRangePulse[iRangeIndex][i]);
                strcpy(pPulse[i], tempPulse);
            }
            //计算最新的pulseIndex
            if (iRangePulse[iRangeIndex][i] == iPulseValue)
                pOtdrTestSetIndex->pulseIndex = i;
        }
    }

    //重新计算index（使用自动量程下默认index）
    if (iRangePulse[iRangeIndex][pOtdrTestSetIndex->pulseIndex] != iPulseValue)
    {
        for(i = 0; i < PULSE_NUM; ++i)
        {
            if (iRangePulseIndex[iRangeIndex] == iRangePulse[iRangeIndex][i])
            {
                pOtdrTestSetIndex->pulseIndex = i;
            }
        }
    }
    //自动脉宽下，量程变成自动
    if (iSelectedAuto)
    {
        pOtdrTestSetIndex->rangeIndex = ENUM_FIBER_RANGE_AUTO;
        pOtdrTestSetIndex->pulseIndex = ENUM_PULSE_AUTO;
    }
    //更新数值
    iPulseValue = iRangePulse[pOtdrTestSetIndex->rangeIndex][pOtdrTestSetIndex->pulseIndex];
}

/***
  * 功能：
        选择测试模式后的回调函数
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/
static void TestModeDropCallBack(int iSelected)
{
    pOtdrTestSetIndex->testModeIndex = iSelected;    
    FrmCommonSetPaint(pFrmCommonSet);
    SendWndMsg_LoopEnable(pFrmCommonSet);
}

/***
  * 功能：
        选择量程后的回调函数
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/
static void RangeDropCallBack(int iSelected)
{
    pOtdrTestSetIndex->rangeIndex = iSelected;    
    //选中自动量程下
    (!iSelected) ? (iSelectedAuto = 1) : (iSelectedAuto = 0);
    FrmCommonSetPaint(pFrmCommonSet);
    SendWndMsg_LoopEnable(pFrmCommonSet);
}

/***
  * 功能：
        选择脉宽后的回调函数
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/

static void PulseDropCallBack(int iSelected)
{
    pOtdrTestSetIndex->pulseIndex = iSelected;
    //选中自动脉宽下
    (!iSelected) ? (iSelectedAuto = 1) : (iSelectedAuto = 0);
    //保存脉宽数值
    iPulseValue = iRangePulse[pOtdrTestSetIndex->rangeIndex][pOtdrTestSetIndex->pulseIndex];
    FrmCommonSetPaint(pFrmCommonSet);
    SendWndMsg_LoopEnable(pFrmCommonSet);
}

/***
  * 功能：
        选择测试时长后的回调函数
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/
static void TestTimeDropCallBack(int iSelected)
{
    pOtdrTestSetIndex->testTimeIndex = iSelected;
    iTimeIndex = iSelected;
    FrmCommonSetPaint(pFrmCommonSet);
    SendWndMsg_LoopEnable(pFrmCommonSet);
}

/***
  * 功能：
        选择距离单位后的回调函数
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/
static void DistUnitDropCallBack(int iSelected)
{
    pOtdrTestSetIndex->distUnitIndex = iSelected;
    
    FrmCommonSetPaint(pFrmCommonSet);
    SendWndMsg_LoopEnable(pFrmCommonSet);
}

/***
  * 功能：
        选择波长后的回调函数
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/
static void WaveLengthDropCallBack(int iSelected)
{
    pOtdrTestSetIndex->waveLengthIndex = iSelected;    
    FrmCommonSetPaint(pFrmCommonSet);
    SendWndMsg_LoopEnable(pFrmCommonSet);
}

/***
  * 功能：
        从系统中获取测试参数
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/
void GetTestParaFromSystem(void)
{
    PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;
    int i;
    int index = 0;
    int count= 0;
    for(i = 0; i < OTDR_WAVE_LENGTH_NUM; ++i)
    {
        if(pUser_Setting->iWave[i] == 1)
        {
            count++;
            index = i;
        }
    }

    if(count > 1)
    {
        pOtdrTestSetIndex->waveLengthIndex = 2;
    }
    else
    {
        pOtdrTestSetIndex->waveLengthIndex = index;
    }

    pOtdrTestSetIndex->testModeIndex = 
                    pUser_Setting->sFrontSetting[pUser_Setting->enWaveCurPos].enTestMode;
    pOtdrTestSetIndex->rangeIndex = 
                    pUser_Setting->sFrontSetting[pUser_Setting->enWaveCurPos].enFiberRange;
	SetPulseIndex(pOtdrTestSetIndex->rangeIndex, 
                    pUser_Setting->sFrontSetting[pUser_Setting->enWaveCurPos].enPulseTime);
    pOtdrTestSetIndex->testTimeIndex = 
                    pUser_Setting->sFrontSetting[pUser_Setting->enWaveCurPos].enAverageTime;
    //平均模式下保存时长index
    if (pOtdrTestSetIndex->testModeIndex == ENUM_AVG_MODE)
        iTimeIndex = pOtdrTestSetIndex->testTimeIndex;
    //自动量程或自动脉宽下，自动标志位置1
    if ((pOtdrTestSetIndex->rangeIndex == ENUM_FIBER_RANGE_AUTO)
    || (pOtdrTestSetIndex->pulseIndex == ENUM_PULSE_AUTO))
        iSelectedAuto = 1;
}


/***
  * 功能：
        保存测试参数到系统中
  * 参数：
        int iSelected：下拉框的选择项
  * 返回：
        无
  * 备注：
***/
static void SaveTestParaToSystem(void)
{
    PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;
    int iWaveLengthIndex = pOtdrTestSetIndex->waveLengthIndex;
    if(iWaveLengthIndex == 0)
    {
        pUser_Setting->iWave[0] = 1;
        pUser_Setting->iWave[1] = 0;
        pUser_Setting->enWaveCurPos = ENUM_WAVE_1310NM;
    }
    else if(iWaveLengthIndex == 1)
    {
        pUser_Setting->iWave[0] = 0;
        pUser_Setting->iWave[1] = 1;
        pUser_Setting->enWaveCurPos = ENUM_WAVE_1550NM;
    }
    else
    {
        pUser_Setting->iWave[0] = 1;
        pUser_Setting->iWave[1] = 1;
        pUser_Setting->enWaveCurPos = ENUM_WAVE_1310NM;
    }
    
    int i;
    for(i = 0; i < OTDR_WAVE_LENGTH_NUM; ++i){
        pUser_Setting->sFrontSetting[i].enTestMode = pOtdrTestSetIndex->testModeIndex;
		pUser_Setting->sFrontSetting[i].enFiberRange = pOtdrTestSetIndex->rangeIndex;
        pUser_Setting->sFrontSetting[i].enPulseTime =  GetPulseIndex(iRangePulse[pOtdrTestSetIndex->rangeIndex][pOtdrTestSetIndex->pulseIndex]);
        pUser_Setting->sFrontSetting[i].enAverageTime = pOtdrTestSetIndex->testTimeIndex;
    }
}