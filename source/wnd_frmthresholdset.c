/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmthresholdset.c
* 摘    要：  实现主窗体FrmThresholdSet的窗体处理操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/7 
*
*******************************************************************************/

#include "wnd_frmthresholdset.h"

#include "app_frmotdr.h"
#include "app_systemsettings.h"
#include "app_frminit.h"
#include "app_unitconverter.h"

#include "guiwindow.h"
#include "guibase.h"
#include "guipicture.h"
#include "guimessage.h"
#include "guilabel.h"

#include "wnd_global.h"
#include "wnd_stack.h"
#include "wnd_frmcommonset.h"
#include "wnd_frmotdranalysis.h"
#include "wnd_frmreportset.h"
#include "wnd_frmime.h"
#include "wnd_frmdialog.h"
#include "wnd_frmotdr.h"

/*******************************************************************************
**							窗体FrmThresholdSet中的声明宏定义		                  **
*******************************************************************************/

#define THRESHOLDSET_SWITCH_NUM         2
#define THRESHOLDSET_CURVE_SET_NUM      7
#define THRESHOLDSET_ANALYS_SET_NUM     2
#define MIN_RANGE_INDEX                 0
#define MAX_RANGE_INDEX                 1

//阈值类型
enum threshold_type
{
    CURVE_THRESHOLD = 0,
    ANALYS_THRESHOLD
};
//曲线阈值的类型
enum curve_threshold_type
{
    SPLICE_LOSS_THRESHOLD = 0,          //接头损耗
    CONNECT_LOSS_THRESHOLD,             //连接器损耗
    RETURN_LOSS_THRESHOLD,              //反射率
    ATTENUATION_THRESHOLD,              //衰减
    REGION_LOSS_THRESHOLD,              //跨段损耗
    REGION_LENGTH_THRESHOLD,            //跨段长度
    REGION_RETURN_LOSS_THRESHOLD        //跨段ORL
};
//分析阈值的类型
enum analys_threshold_type
{
    SPLICE_LOSS_ANALYS_THRESHOLD = 0,   //接头损耗
    END_LOSS_ANALYS_THRESHOLD
};
//曲线阈值控件
struct curve_set_control
{
    GUILABEL* pLblTitle[THRESHOLDSET_CURVE_SET_NUM];
    GUICHAR* pStrTitle[THRESHOLDSET_CURVE_SET_NUM];
    GUILABEL* pLblData[THRESHOLDSET_CURVE_SET_NUM];
    GUICHAR* pStrData[THRESHOLDSET_CURVE_SET_NUM];
    GUIPICTURE *pBtnIcon[THRESHOLDSET_CURVE_SET_NUM];
    GUIPICTURE* pBtnEnterIcon[THRESHOLDSET_CURVE_SET_NUM];
    GUIPICTURE* pBtnSelect[THRESHOLDSET_CURVE_SET_NUM];
};
//分析阈值控件
struct analys_set_control
{
    GUILABEL* pLblTitle[THRESHOLDSET_ANALYS_SET_NUM];
    GUICHAR* pStrTitle[THRESHOLDSET_ANALYS_SET_NUM];
    GUILABEL* pLblData[THRESHOLDSET_ANALYS_SET_NUM];
    GUICHAR* pStrData[THRESHOLDSET_ANALYS_SET_NUM];
    GUIPICTURE *pBtnIcon[THRESHOLDSET_ANALYS_SET_NUM];
    GUIPICTURE* pBtnEnterIcon[THRESHOLDSET_ANALYS_SET_NUM];
    GUIPICTURE* pBtnSelect[THRESHOLDSET_ANALYS_SET_NUM];
};

//曲线阈值范围（最大值与最小值）
static float curveThrRangeValues[THRESHOLDSET_CURVE_SET_NUM][2] = 
{{0.015f,5.000f},{0.015f,5.000f},{-80.0f,0.0f},
{0.000f,5.000f},{0.000f,45.000f},{0.000f,300.000f},
{0.000f,100.000f}};    
//重新事件分析的标志位
static int iReAnalysisFlag = 0;		

char* pStrOtdrSetButton_unpress[THRESHOLDSET_TITLE_NUM] =
{
	BmpFileDirectory"btn_test_unpress.bmp",
	BmpFileDirectory"btn_analysis_unpress.bmp",
	BmpFileDirectory"btn_threshold_unpress.bmp",
	BmpFileDirectory"btn_report_unpress.bmp"
};

char* pStrOtdrSetButton_press[THRESHOLDSET_TITLE_NUM] =
{
	BmpFileDirectory"btn_test_press.bmp",
	BmpFileDirectory"btn_analysis_press.bmp",
	BmpFileDirectory"btn_threshold_press.bmp",
	BmpFileDirectory"btn_report_press.bmp"
};

//曲线阈值图标
char* pStrCurveThresholdButton_unpress[THRESHOLDSET_CURVE_SET_NUM] =
{
	BmpFileDirectory"btn_spliceLoss_unpress.bmp",
	BmpFileDirectory"btn_connectorLoss_unpress.bmp",
	BmpFileDirectory"btn_refLoss_unpress.bmp",
	BmpFileDirectory"btn_attLoss_unpress.bmp",
	BmpFileDirectory"btn_regionLoss_unpress.bmp",
	BmpFileDirectory"btn_regionLength_unpress.bmp",
	BmpFileDirectory"btn_regionOrl_unpress.bmp"
};

char* pStrCurveThresholdButton_press[THRESHOLDSET_CURVE_SET_NUM] =
{
	BmpFileDirectory"btn_spliceLoss_press.bmp",
	BmpFileDirectory"btn_connectorLoss_press.bmp",
	BmpFileDirectory"btn_refLoss_press.bmp",
	BmpFileDirectory"btn_attLoss_press.bmp",
	BmpFileDirectory"btn_regionLoss_press.bmp",
	BmpFileDirectory"btn_regionLength_press.bmp",
	BmpFileDirectory"btn_regionOrl_press.bmp"
};

//分析阈值图标
char* pStrAnalysisThresholdButton_unpress[THRESHOLDSET_ANALYS_SET_NUM] =
{
	BmpFileDirectory"btn_analysis_spliceLoss_unpress.bmp",
	BmpFileDirectory"btn_analysis_fiberEnd_unpress.bmp"
};

char* pStrAnalysisThresholdButton_press[THRESHOLDSET_ANALYS_SET_NUM] =
{
	BmpFileDirectory"btn_analysis_spliceLoss_press.bmp",
	BmpFileDirectory"btn_analysis_fiberEnd_press.bmp"
};

/*******************************************************************************
**							窗体FrmThresholdSet中的控件定义部分		                  **
*******************************************************************************/
static GUIWINDOW *pFrmThresholdSet = NULL;

static GUIPICTURE *pFrmThresholdSetBg = NULL;
static GUIPICTURE *pFrmThresholdSetBgIcon = NULL;
static GUIPICTURE *pFrmThresholdSetBtnBack = NULL;
//当前界面的标题
static GUILABEL *pFrmThresholdSetLblCurrTitle = NULL;               //当前界面的标题
static GUICHAR *pFrmThresholdSetStrCurrTitle = NULL;
//功能区的标题
static GUIPICTURE *pFrmThresholdSetBtnTitle[THRESHOLDSET_TITLE_NUM] = {NULL};
static GUILABEL *pFrmThresholdSetLblTitle[THRESHOLDSET_TITLE_NUM] = {NULL};
static GUICHAR *pFrmThresholdSetStrTitle[THRESHOLDSET_TITLE_NUM] = {NULL};
//阈值设置项的标题
static GUILABEL* pFrmThresholdSetLblCurveTitle = NULL;          //曲线通过/不通过阈值设定
static GUICHAR* pFrmThresholdSetStrCurveTitle = NULL;
static GUILABEL* pFrmThresholdSetLblAnalysTitle = NULL;         //分析设置阈值设定
static GUICHAR* pFrmThresholdSetStrAnalysTitle = NULL;
static GUIPICTURE* pFrmThresholdSetBgCurveSelect = NULL;    //选中曲线阈值
static GUIPICTURE *pFrmThresholdSetBgAnalysisSelect = NULL; //选中分析阈值
//阈值设置开关(//曲线通过/不通过设定开关与分析设置开关)
static GUIPICTURE* pFrmThresholdSetBtnSwitch[THRESHOLDSET_SWITCH_NUM] = {NULL};
//控件声明
static struct curve_set_control* pCurveSetControl = NULL;
static struct analys_set_control* pAnalysSetControl = NULL;
static int iCurrentOption = -1;										//当前选中的选项

//自定义设置结构体指针
static OTHER_SETTING *pOtdrPassThreshold = NULL;
static ANALYSIS_SETTING *pOtdrAnalysisSet = NULL;
/*******************************************************************************
*                   窗体FrmThresholdSet内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int ThresholdSetTextRes_Init(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
//释放文本资源
static int ThresholdSetTextRes_Exit(void *pInArg, int iInLen,
                                            void *pOutArg, int iOutLen);
/*******************************************************************************
*                   窗体FrmThresholdSet内部按键响应函数声明
*******************************************************************************/
//返回按钮响应处理函数
static int ThresholdSetBtnBack_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
static int ThresholdSetBtnBack_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
//标题栏按钮响应处理函数
static int ThresholdSetBtnTitle_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
static int ThresholdSetBtnTitle_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);

//阈值设置开关按钮响应处理函数
static int ThresholdSetBtnSwitch_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
static int ThresholdSetBtnSwitch_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);

//曲线阈值设置按钮响应处理函数
static int ThresholdSetBtnCurveSet_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
static int ThresholdSetBtnCurveSet_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);

//分析阈值设置按钮响应处理函数
static int ThresholdSetBtnAnalys_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
static int ThresholdSetBtnAnalys_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen);
//点击按键响应函数
static int WndThresholdSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndThresholdSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体FrmThresholdSet内部参数相关功能函数声明
*******************************************************************************/
//初始化参数
static void ThresholdSetInitPara(void);
//清除参数(保存参数)
static void ThresholdSetClearPara(void);
//设置曲线阈值参数的使能（统一设置）
static void SetCurveThresholdEnable(int iWaveLength, int iEnabled);
//设置开关的状态
static void SetSwitchStatus(int iEnabled, int thresholdtype, int iSelectedIndex);
//刷新阈值设置各个选项
static void FlushThresholdSetOption(void);
//刷新参数信息显示
static void SetParaDisplayInfo(void);
//刷新曲线阈值参数显示信息
static void FlushCurveParaDisplayInfo(int iEnable);
//刷新分析阈值参数显示信息
static void FlushAnalysParaDisplayInfo(int iEnable);
//阈值设置范围的提示框
static void ThresholdSetIMEDialog(char *min, char *max);
/*******************************************************************************
*                   窗体FrmThresholdSet内部回调函数函数声明
******************************************************************************/
//重绘窗体函数
static void ReCreateThresholdSetWindow(GUIWINDOW **pWnd);
//输入接头损耗后的回调函数(曲线阈值)
static void InputSpliceLossBackFunc(void);
//输入连接器损耗后的回调函数(曲线阈值)
static void InputConnectLossBackFunc(void);
//输入回波损耗（反射率）后的回调函数(曲线阈值)
static void InputReturnLossBackFunc(void);
//输入衰减后的回调函数(曲线阈值)
static void InputAttenuationBackFunc(void);
//输入跨段损耗后的回调函数(曲线阈值)
static void InputRegionLossBackFunc(void);
//输入跨段长度后的回调函数(曲线阈值)
static void InputRegionLengthBackFunc(void);
//输入跨段ORL（回波损耗）后的回调函数(曲线阈值)
static void InputRegionReturnLossBackFunc(void);
//输入熔接损耗后的回调函数(分析阈值)
static void InputAnalysisSpliceLossBackFunc(void);
//输入光纤末端阈值后的回调函数(分析阈值)
static void InputEndlossBackFunc(void);
//重新事件分析显示对话框
static void DialogReAnalysisBackFunc(void);
//处理enter按键响应
static void HandleEnterKeyCallBack(void);

/***
  * 功能：
        窗体frmthresholdset的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmThresholdSetInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    pFrmThresholdSet= (GUIWINDOW*)pWndObj;

    pFrmThresholdSetBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                       BmpFileDirectory"bg_otdr_threshold_set.bmp");
    pFrmThresholdSetBgIcon = CreatePicture(0, 0, 23, 20,
                                           BmpFileDirectory"bg_thresholdset_icon.bmp"); //待定
    pFrmThresholdSetBtnBack = CreatePicture(0, 20, 36, 21,
                                            BmpFileDirectory"btn_otdrSet_back.bmp");

    ThresholdSetInitPara();
    //初始化文本资源
    ThresholdSetTextRes_Init(NULL, 0, NULL, 0);
    
    pFrmThresholdSetLblCurrTitle= CreateLabel(8, 53, 100, 16, pFrmThresholdSetStrCurrTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmThresholdSetLblCurrTitle);
    int i;
    for(i = 0; i < THRESHOLDSET_TITLE_NUM; ++i)
    {
        pFrmThresholdSetBtnTitle[i] = CreatePicture(232 + 102 * i, 47, 99, 35,
                                                    pStrOtdrSetButton_unpress[i]);
        pFrmThresholdSetLblTitle[i] = CreateLabel(232 + 102 * i + 5, 47 + 9, 90, 16,
                                                  pFrmThresholdSetStrTitle[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmThresholdSetLblTitle[i]);
        SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmThresholdSetLblTitle[i]);
    }

    pFrmThresholdSetBgCurveSelect = CreatePicture(1, 83, 637, 35, BmpFileDirectory "bg_sys_unselect.bmp");
    pFrmThresholdSetLblCurveTitle = CreateLabel(40, 92, 200, 16, pFrmThresholdSetStrCurveTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmThresholdSetLblCurveTitle);
    pFrmThresholdSetBgAnalysisSelect = CreatePicture(1, 371, 637, 35, BmpFileDirectory "bg_sys_unselect.bmp");
    pFrmThresholdSetLblAnalysTitle = CreateLabel(40, 381, 200, 16, pFrmThresholdSetStrAnalysTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmThresholdSetLblAnalysTitle);
    //阈值开关
    for(i = 0; i < THRESHOLDSET_SWITCH_NUM; ++i)
    {
        pFrmThresholdSetBtnSwitch[i] = CreatePicture(570, 93 + 288 * i, 32, 16,
                                                     BmpFileDirectory "btn_on_unpress.bmp"); //默认打开
    }
    //曲线阈值设置
    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
    {
        //曲线阈值参数
        pCurveSetControl->pBtnSelect[i] = CreatePicture(1, 119 + i * 36, 637, 35,
                                                        BmpFileDirectory "bg_sys_unselect.bmp");
        
        pCurveSetControl->pLblTitle[i] = CreateLabel(75, 129 + i * 36, 150, 16,
                                                        pCurveSetControl->pStrTitle[i]);
        pCurveSetControl->pLblData[i] = CreateLabel(450, 129 + i * 36, 100, 16,
                                                    pCurveSetControl->pStrData[i]);
        pCurveSetControl->pBtnIcon[i] = CreatePicture(45, 127 + i * 36, 20, 20,
                                                        pStrCurveThresholdButton_unpress[i]);
        pCurveSetControl->pBtnEnterIcon[i] = CreatePicture(580, 129 + i * 36, 22, 16,
                                                            BmpFileDirectory "bg_sys_keyboard.bmp");

        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveSetControl->pLblTitle[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveSetControl->pLblData[i]);
        //分析阈值参数
        if (i < THRESHOLDSET_ANALYS_SET_NUM)
        {
            pAnalysSetControl->pBtnSelect[i] = CreatePicture(1, 407 + 36 * i, 637, 35,
                                                                BmpFileDirectory "bg_sys_unselect.bmp");
            pAnalysSetControl->pLblTitle[i] = CreateLabel(75, 417 + 36 * i, 150, 16,
                                                            pAnalysSetControl->pStrTitle[i]);
            pAnalysSetControl->pLblData[i] = CreateLabel(450, 417 + 36 * i, 100, 16,
                                                            pAnalysSetControl->pStrData[i]);
            pAnalysSetControl->pBtnIcon[i] = CreatePicture(45, 417 + 36 * i, 20, 20,
                                                            pStrAnalysisThresholdButton_unpress[i]);
            pAnalysSetControl->pBtnEnterIcon[i] = CreatePicture(580, 417 + 36 * i, 22, 16,
                                                                BmpFileDirectory "bg_sys_keyboard.bmp");
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pAnalysSetControl->pLblTitle[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pAnalysSetControl->pLblData[i]);
        }
    }

    //注册窗体(因为所有的按键事件都统一由窗体进行处理)
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW),
                  pFrmThresholdSet, pFrmThresholdSet);
    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                  pFrmThresholdSetBtnBack, pFrmThresholdSet);

    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
    {
        if(i < THRESHOLDSET_TITLE_NUM)
        {
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                          pFrmThresholdSetBtnTitle[i], pFrmThresholdSet);
            AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
                          pFrmThresholdSetLblTitle[i], pFrmThresholdSet);
        }
        
        if(i < THRESHOLDSET_SWITCH_NUM)
        {
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                          pFrmThresholdSetBtnSwitch[i], pFrmThresholdSet);
        }

        if(i < THRESHOLDSET_ANALYS_SET_NUM)
        {
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                          pAnalysSetControl->pBtnEnterIcon[i], pFrmThresholdSet);
        }
        //曲线阈值设置控件注册
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
                      pCurveSetControl->pBtnEnterIcon[i], pFrmThresholdSet);
    }

    
    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    GUIMESSAGE *pMsg = GetCurrMessage();

    LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmThresholdSetBtnBack,
                    ThresholdSetBtnBack_Down, NULL, 0, pMsg);
    LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmThresholdSetBtnBack,
                    ThresholdSetBtnBack_Up, NULL, 0, pMsg);
    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
    {
        if(i < THRESHOLDSET_TITLE_NUM)
        {
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmThresholdSetLblTitle[i],
                            ThresholdSetBtnTitle_Down, NULL, i, pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmThresholdSetLblTitle[i],
                            ThresholdSetBtnTitle_Up, NULL, i, pMsg);
        }
        
        if(i < THRESHOLDSET_SWITCH_NUM)
        {
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmThresholdSetBtnSwitch[i],
                            ThresholdSetBtnSwitch_Down, NULL, i, pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmThresholdSetBtnSwitch[i],
                            ThresholdSetBtnSwitch_Up, NULL, i, pMsg);
        }

        if(i < THRESHOLDSET_ANALYS_SET_NUM)
        {
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pAnalysSetControl->pBtnEnterIcon[i],
                            ThresholdSetBtnAnalys_Down, NULL, i, pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pAnalysSetControl->pBtnEnterIcon[i],
                            ThresholdSetBtnAnalys_Up, NULL, i, pMsg);
        }

        //曲线阈值设置消息注册
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pCurveSetControl->pBtnEnterIcon[i],
                        ThresholdSetBtnCurveSet_Down, NULL, i, pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pCurveSetControl->pBtnEnterIcon[i],
                        ThresholdSetBtnCurveSet_Up, NULL, i, pMsg);
    }

    LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmThresholdSet,
                    WndThresholdSetKey_Down, NULL, 0, GetCurrMessage());
    LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmThresholdSet,
                    WndThresholdSetKey_Up, NULL, 0, GetCurrMessage());

    return iRet;
}

/***
  * 功能：
        窗体frmthresholdset的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmThresholdSetExit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;
    //得到当前窗体对象
    pFrmThresholdSet= (GUIWINDOW*)pWndObj;
	//清空消息队列中的消息注册项
    GUIMESSAGE* pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);
    //从当前窗体中注销窗体控件
    ClearWindowComp(pFrmThresholdSet);

    DestroyPicture(&pFrmThresholdSetBg);
	DestroyPicture(&pFrmThresholdSetBgIcon);
    DestroyPicture(&pFrmThresholdSetBtnBack);

    DestroyLabel(&pFrmThresholdSetLblCurrTitle);
    DestroyLabel(&pFrmThresholdSetLblCurveTitle);
    DestroyLabel(&pFrmThresholdSetLblAnalysTitle);

    DestroyPicture(&pFrmThresholdSetBgCurveSelect);
    DestroyPicture(&pFrmThresholdSetBgAnalysisSelect);

    int i;
    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
    {
        if(i < THRESHOLDSET_TITLE_NUM)
        {
            DestroyPicture(&pFrmThresholdSetBtnTitle[i]);
            DestroyLabel(&pFrmThresholdSetLblTitle[i]);
        }
        
        if(i < THRESHOLDSET_SWITCH_NUM)
        {
            DestroyPicture(&pFrmThresholdSetBtnSwitch[i]);
        }

        if(i < THRESHOLDSET_ANALYS_SET_NUM)
        {
            DestroyPicture(&(pAnalysSetControl->pBtnSelect[i]));
            DestroyLabel(&(pAnalysSetControl->pLblTitle[i]));
            DestroyLabel(&(pAnalysSetControl->pLblData[i]));
            DestroyPicture(&(pAnalysSetControl->pBtnIcon[i]));
            DestroyPicture(&(pAnalysSetControl->pBtnEnterIcon[i]));
        }

        //曲线阈值设置控件销毁
        DestroyPicture(&(pCurveSetControl->pBtnSelect[i]));
        DestroyLabel(&(pCurveSetControl->pLblTitle[i]));
        DestroyLabel(&(pCurveSetControl->pLblData[i]));
        DestroyPicture(&(pCurveSetControl->pBtnIcon[i]));
        DestroyPicture(&(pCurveSetControl->pBtnEnterIcon[i]));
    }

    ThresholdSetTextRes_Exit(NULL, 0, NULL, 0);

    ThresholdSetClearPara();
#ifdef EEPROM_DATA    
	//退出并保存
    SetSettingsData((void*)&pOtdrTopSettings->pUser_Setting->sOtherSetting, sizeof(OTHER_SETTING), RESULT_SET);
    SaveSettings(RESULT_SET);
#endif    
    return iRet;
}

/***
  * 功能：
        窗体frmthresholdset的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetPaint(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

	if (iReAnalysisFlag)
	{
		DialogInit(120, 90, TransString("Warning"),
			GetCurrLanguageText(OTDR_SET_LBL_ENSURE_CHANGE),
			0, 1, ReCreateThresholdSetWindow, DialogReAnalysisBackFunc, NULL);

		iReAnalysisFlag = 0;
		return 0;
	}

    DisplayPicture(pFrmThresholdSetBg);
	// DisplayPicture(pFrmThresholdSetBgIcon);
    // DisplayPicture(pFrmThresholdSetBtnBack);
    //设置开关背景
    if (iCurrentOption == -1)
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp", pFrmThresholdSetBgCurveSelect);
    if (iCurrentOption == THRESHOLDSET_CURVE_SET_NUM)
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp", pFrmThresholdSetBgAnalysisSelect);

    DisplayPicture(pFrmThresholdSetBgCurveSelect);
    DisplayPicture(pFrmThresholdSetBgAnalysisSelect);

    DisplayLabel(pFrmThresholdSetLblCurrTitle);
    DisplayLabel(pFrmThresholdSetLblCurveTitle);
    DisplayLabel(pFrmThresholdSetLblAnalysTitle);

    //设置参数显示信息
    SetParaDisplayInfo();
	//刷新各个选项
	//FlushThresholdSetOption();
#if 1
	int iWaveLength = pOtdrPassThreshold->iWaveLength;
	int iCurveEnable = pOtdrPassThreshold->PassThr.iThrEnable[iWaveLength];
	int iAnalysEnable = pOtdrAnalysisSet->iEnableAnalysisThreshold;//默认值

    int i;
    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)//THRESHOLDSET_CURVE_SET_NUM
    {
		if (i < THRESHOLDSET_TITLE_NUM)
		{
			if (i == THRESHOLD_SET)
			{
				SetPictureBitmap(pStrOtdrSetButton_press[THRESHOLD_SET], pFrmThresholdSetBtnTitle[i]);
			}
			DisplayPicture(pFrmThresholdSetBtnTitle[i]); 
			DisplayLabel(pFrmThresholdSetLblTitle[i]);
		}

        if(i < THRESHOLDSET_SWITCH_NUM)
        {
			SetSwitchStatus(iCurveEnable, CURVE_THRESHOLD, -1);
			SetSwitchStatus(iAnalysEnable, ANALYS_THRESHOLD, THRESHOLDSET_CURVE_SET_NUM);
            DisplayPicture(pFrmThresholdSetBtnSwitch[i]);
        }

        if(i < THRESHOLDSET_ANALYS_SET_NUM)
        {
            if(iCurrentOption == (i+THRESHOLDSET_CURVE_SET_NUM+1))
            {
                SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", 
                                        pAnalysSetControl->pBtnSelect[i]);
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp",
                                    pAnalysSetControl->pBtnEnterIcon[i]);
                SetPictureBitmap(pStrAnalysisThresholdButton_press[i],
                                    pAnalysSetControl->pBtnIcon[i]);
            }
            else
            {
                SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", 
                                        pAnalysSetControl->pBtnSelect[i]);
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp",
                                    pAnalysSetControl->pBtnEnterIcon[i]);
                SetPictureBitmap(pStrAnalysisThresholdButton_unpress[i],
                                    pAnalysSetControl->pBtnIcon[i]);
            }
            
            DisplayPicture(pAnalysSetControl->pBtnSelect[i]);
            DisplayLabel(pAnalysSetControl->pLblTitle[i]);
            SetPictureEnable(1, pAnalysSetControl->pBtnEnterIcon[i]);
            DisplayPicture(pAnalysSetControl->pBtnEnterIcon[i]);
            DisplayPicture(pAnalysSetControl->pBtnIcon[i]);
            if (pOtdrAnalysisSet->iEnableAnalysisThreshold == 0)
            {
                //不使能键盘
                if (iCurrentOption == (i+THRESHOLDSET_CURVE_SET_NUM+1))
                    SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                     pAnalysSetControl->pBtnEnterIcon[i]);
                else
                    SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp",
                                     pAnalysSetControl->pBtnEnterIcon[i]);

                DisplayPicture(pAnalysSetControl->pBtnEnterIcon[i]);
                SetPictureEnable(0, pAnalysSetControl->pBtnEnterIcon[i]);
                SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pAnalysSetControl->pLblData[i]);
            }
            DisplayLabel(pAnalysSetControl->pLblData[i]);
        }

        //显示曲线阈值设置控件
        if(iCurrentOption == i)
        {
            SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", 
                                    pCurveSetControl->pBtnSelect[i]);
            SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp",
                                pCurveSetControl->pBtnEnterIcon[i]);
            SetPictureBitmap(pStrCurveThresholdButton_press[i],
                                pCurveSetControl->pBtnIcon[i]);
        }
        else
        {
            SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", 
                                    pCurveSetControl->pBtnSelect[i]);
            SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp",
                                pCurveSetControl->pBtnEnterIcon[i]);
            SetPictureBitmap(pStrCurveThresholdButton_unpress[i],
                                pCurveSetControl->pBtnIcon[i]);
        }
        
        DisplayPicture(pCurveSetControl->pBtnSelect[i]);
        DisplayLabel(pCurveSetControl->pLblTitle[i]);
        SetPictureEnable(1, pCurveSetControl->pBtnEnterIcon[i]);
        DisplayPicture(pCurveSetControl->pBtnEnterIcon[i]);
        DisplayPicture(pCurveSetControl->pBtnIcon[i]);

        if (pOtdrPassThreshold->PassThr.iThrEnable[pOtdrPassThreshold->iWaveLength] == 0)
        {
            //不使能键盘
            if (iCurrentOption == i)
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            else
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            DisplayPicture(pCurveSetControl->pBtnEnterIcon[i]);
            SetPictureEnable(0, pCurveSetControl->pBtnEnterIcon[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pCurveSetControl->pLblData[i]);
        }
        DisplayLabel(pCurveSetControl->pLblData[i]);
	}
#endif

    SetPowerEnable(1, 1);
    RefreshScreen(__FILE__, __func__, __LINE__);

    return iRet;
}

/***
  * 功能：
        窗体frmthresholdset的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmthresholdset的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetPause(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/***
  * 功能：
        窗体frmthresholdset的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetResume(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}

/*******************************************************************************
*                   窗体FrmThresholdSet内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int ThresholdSetTextRes_Init(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;

	pFrmThresholdSetStrCurrTitle = GetCurrLanguageText(OTDR_SET_LBL_THRESHOLD);
	pFrmThresholdSetStrTitle[TEST_SET] = GetCurrLanguageText(OTDR_SET_LBL_TEST);
	pFrmThresholdSetStrTitle[OTDR_ANALYSIS_SET] = GetCurrLanguageText(OTDR_SET_LBL_ANALYSIS);
	pFrmThresholdSetStrTitle[THRESHOLD_SET] = GetCurrLanguageText(OTDR_SET_LBL_THRESHOLD);
    pFrmThresholdSetStrTitle[REPORT_SET] = GetCurrLanguageText(OTDR_SET_LBL_REPORT);

    pFrmThresholdSetStrCurveTitle= GetCurrLanguageText(OTDR_SET_LBL_CURVE_THRESHOLD);
    pFrmThresholdSetStrAnalysTitle= GetCurrLanguageText(OTDR_SET_LBL_ANALYSIS_SETTING);

    //曲线阈值设置
    int i;
    unsigned int strCurveSetTitle[THRESHOLDSET_CURVE_SET_NUM] = 
    {
		OTDR_SET_LBL_CONNECTION_LOSS,
		OTDR_SET_LBL_CONNECTOR_LOSS,
		OTDR_SET_LBL_REFLECTIVITY,
		OTDR_SET_LBL_ATTENUATION,
		OTDR_SET_LBL_SECTION_LOSS,
		OTDR_SET_LBL_SECTION_DISTANCE,
		OTDR_SET_LBL_SECTION_ORL
    };

    char* strCurveSetData[THRESHOLDSET_CURVE_SET_NUM] = 
    {
        "0.3", "0.75", "-40", "0.4", "20.00", "0.0", "15.00"
    };

    unsigned int strAnalySetTitle[THRESHOLDSET_ANALYS_SET_NUM] = 
    {
		OTDR_SET_LBL_SPLICING_LOSS,
		OTDR_SET_LBL_END_LOSS
    };

    char* strAnalySetData[THRESHOLDSET_ANALYS_SET_NUM] = 
    {
        "0.02",
        "5.000"
    };
        
    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
    {
        if(i < THRESHOLDSET_ANALYS_SET_NUM)
        {
            pAnalysSetControl->pStrTitle[i] = GetCurrLanguageText(strAnalySetTitle[i]);
            pAnalysSetControl->pStrData[i] = TransString(strAnalySetData[i]);
        }
        
        pCurveSetControl->pStrTitle[i] = GetCurrLanguageText(strCurveSetTitle[i]);
        pCurveSetControl->pStrData[i] = TransString(strCurveSetData[i]);
    }
    
    return iRet;
}
//释放文本资源
static int ThresholdSetTextRes_Exit(void *pInArg, int iInLen,
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;

    GuiMemFree(pFrmThresholdSetStrCurrTitle);

    int i;

    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
    {
        if(i < THRESHOLDSET_TITLE_NUM)
        {
            if(i < THRESHOLDSET_ANALYS_SET_NUM)
            {   
                GuiMemFree(pAnalysSetControl->pStrTitle[i]);
                GuiMemFree(pAnalysSetControl->pStrData[i]);
            }
            
            GuiMemFree(pFrmThresholdSetStrTitle[i]);
        }
        
        GuiMemFree(pCurveSetControl->pStrTitle[i]);
        GuiMemFree(pCurveSetControl->pStrData[i]);
    }
    
    GuiMemFree(pFrmThresholdSetStrCurveTitle);
    GuiMemFree(pFrmThresholdSetStrAnalysTitle);
    
    return iRet;
}

/*******************************************************************************
*                   窗体FrmThresholdSet内部按键响应函数声明
*******************************************************************************/
//返回按钮响应处理函数
static int ThresholdSetBtnBack_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
    
    return iRet;
}
                                            
static int ThresholdSetBtnBack_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
    
	Stack *ps = NULL;
	Item func;
    ps = GetCurrWndStack();
    WndPop(ps, &func, pFrmThresholdSet);
    
    return iRet;
}

//标题栏响应处理函数
static int ThresholdSetBtnTitle_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
    
    return iRet;
}
                                            
static int ThresholdSetBtnTitle_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;    
	int option = iOutLen;
	GUIWINDOW *pWnd = NULL;

    switch(option)
    {
        case TEST_SET:
            {
				pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
					FrmCommonSetInit, FrmCommonSetExit,
					FrmCommonSetPaint, FrmCommonSetLoop,
					FrmCommonSetPause, FrmCommonSetResume,
					NULL);
				SendWndMsg_WindowExit(pFrmThresholdSet);
				SendSysMsg_ThreadCreate(pWnd);
            }
            break;
		case OTDR_ANALYSIS_SET:
			{
				pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
					FrmOtdrAnalysisSetInit, FrmOtdrAnalysisSetExit,
					FrmOtdrAnalysisSetPaint, FrmOtdrAnalysisSetLoop,
					FrmOtdrAnalysisSetPause, FrmOtdrAnalysisSetResume,
					NULL);
				SendWndMsg_WindowExit(pFrmThresholdSet);
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
				SendWndMsg_WindowExit(pFrmThresholdSet);
				SendSysMsg_ThreadCreate(pWnd);
			}
            break;
        default:
            break;
    }
    
    return iRet;
}

//阈值设置开关按钮响应处理函数
static int ThresholdSetBtnSwitch_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;

    return iRet;
}
                                            
static int ThresholdSetBtnSwitch_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
    int option = iOutLen;
    int iWaveLength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(iWaveLength == WAVELEN_1625)
    {
        iWaveLength = WAVELEN_1310;
    }
    
    int* iCurveEnable = &pOtdrPassThreshold->PassThr.iThrEnable[iWaveLength];
    int* iAnalysEnable = &pOtdrAnalysisSet->iEnableAnalysisThreshold;//默认值
    
    switch(option)
    {
        case CURVE_THRESHOLD:
            *iCurveEnable = *iCurveEnable ? 0:1;
            SetCurveThresholdEnable(iWaveLength, *iCurveEnable);
			SetSwitchStatus(*iCurveEnable, CURVE_THRESHOLD, -1);
            iCurveEnable ? SetPictureBitmap(BmpFileDirectory "btn_sys_select.bmp", pFrmThresholdSetBgCurveSelect)
                         : SetPictureBitmap(BmpFileDirectory "btn_sys_unselect.bmp", pFrmThresholdSetBgCurveSelect);
            DisplayPicture(pFrmThresholdSetBgCurveSelect);
            DisplayPicture(pFrmThresholdSetBtnSwitch[CURVE_THRESHOLD]);
            DisplayLabel(pFrmThresholdSetLblCurveTitle);
            FlushCurveParaDisplayInfo(*iCurveEnable);
            break;
        case ANALYS_THRESHOLD:
            *iAnalysEnable = *iAnalysEnable ? 0:1;
			SetSwitchStatus(*iAnalysEnable, ANALYS_THRESHOLD, THRESHOLDSET_CURVE_SET_NUM);
            iAnalysEnable ? SetPictureBitmap(BmpFileDirectory "btn_sys_select.bmp", pFrmThresholdSetBgAnalysisSelect)
                          : SetPictureBitmap(BmpFileDirectory "btn_sys_unselect.bmp", pFrmThresholdSetBgAnalysisSelect);
            DisplayPicture(pFrmThresholdSetBgAnalysisSelect);
            DisplayLabel(pFrmThresholdSetLblAnalysTitle);
            DisplayPicture(pFrmThresholdSetBtnSwitch[ANALYS_THRESHOLD]);
			FlushAnalysParaDisplayInfo(*iAnalysEnable);
            break;
        default:
            break;
    }
    
    return iRet;
}

//曲线阈值设置按钮响应处理函数
static int ThresholdSetBtnCurveSet_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
        
    return iRet;
}
                                            
static int ThresholdSetBtnCurveSet_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
	char *tempBuf = NULL;       //接收临时存储界面上的数值
	int iWaveLength = pOtdrPassThreshold->iWaveLength;
    int option = iOutLen;

	if (!pOtdrPassThreshold->PassThr.iThrEnable[iWaveLength])
	{
		return -1;
	}

    switch (option)
    {
        case SPLICE_LOSS_THRESHOLD:
            tempBuf = Float2String(LOSS_PRECISION, pOtdrPassThreshold->PassThr.SpliceLossThr[iWaveLength].fThrValue);
            IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputSpliceLossBackFunc, NULL);
            break;
        case CONNECT_LOSS_THRESHOLD:
            tempBuf = Float2String(LOSS_PRECISION, pOtdrPassThreshold->PassThr.ConnectLossThr[iWaveLength].fThrValue);
            IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputConnectLossBackFunc, NULL);
            break;
        case RETURN_LOSS_THRESHOLD:
            tempBuf = Float2String(REFLECTANCE_PRECISION, pOtdrPassThreshold->PassThr.ReturnLossThr[iWaveLength].fThrValue);
            IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputReturnLossBackFunc, NULL);
            break;
        case ATTENUATION_THRESHOLD:
            tempBuf = Float2String(ATTENUATION_PRECISION, pOtdrPassThreshold->PassThr.AttenuationThr[iWaveLength].fThrValue);
            IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputAttenuationBackFunc, NULL);
            break;
        case REGION_LOSS_THRESHOLD:
            tempBuf = Float2String(LOSS_PRECISION, pOtdrPassThreshold->PassThr.RegionLossThr[iWaveLength].fThrValue);
    		IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputRegionLossBackFunc, NULL);
            break;
        case REGION_LENGTH_THRESHOLD:
            tempBuf = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_OTDR,
                    pOtdrPassThreshold->PassThr.fSpanLength_m[iWaveLength], 
                    pOtdrPassThreshold->PassThr.fSpanLength_ft[iWaveLength], 
                    pOtdrPassThreshold->PassThr.fSpanLength_mi[iWaveLength], 0);
    		IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputRegionLengthBackFunc, NULL);
            break;
        case REGION_RETURN_LOSS_THRESHOLD:
            tempBuf = Float2String(ORL_PRECISION, pOtdrPassThreshold->PassThr.RegionReturnLossThr[iWaveLength].fThrValue);
            IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputRegionReturnLossBackFunc, NULL);
            break;
        default:
            break;
    }

    GuiMemFree(tempBuf);
    
    return iRet;
}

//分析阈值设置按钮响应处理函数
static int ThresholdSetBtnAnalys_Down(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
        
    return iRet;
}
                                            
static int ThresholdSetBtnAnalys_Up(void *pInArg, int iInLen, 
                                            void *pOutArg, int iOutLen)
{
    //错误标志，返回值定义 
    int iRet = 0;
    int option = iOutLen;  
	char *tempBuf = NULL;//临时存储损耗值

	if (!pOtdrAnalysisSet->iEnableAnalysisThreshold)
	{
		return -1;
	}

	switch (option)
	{
	case SPLICE_LOSS_ANALYS_THRESHOLD://输入熔接损耗
		tempBuf = Float2String(LOSS_PRECISION, pOtdrAnalysisSet->fSpliceLossThr);
		IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputAnalysisSpliceLossBackFunc, NULL);
		break;
	case END_LOSS_ANALYS_THRESHOLD://输入光纤端部检测阈值
		tempBuf = Float2String(LOSS_PRECISION, pOtdrAnalysisSet->fEndLossThr);
		IMEInit(tempBuf, 10, 1, ReCreateThresholdSetWindow, InputEndlossBackFunc, NULL);
		break;
	default:
		break;
	}

	GuiMemFree(tempBuf);
    
    return iRet;
}

int WndThresholdSetKey_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	return 0;
}

int WndThresholdSetKey_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	unsigned int uiValue;
	uiValue = (unsigned int)pInArg;

    switch (uiValue)
    {
    case KEYCODE_OTDR_LEFT:
        ThresholdSetBtnTitle_Up(NULL, 0, NULL, OTDR_ANALYSIS_SET);
        break;
    case KEYCODE_OTDR_RIGHT:
        ThresholdSetBtnTitle_Up(NULL, 0, NULL, REPORT_SET);
        break;
    case KEYCODE_UP:
        if (iCurrentOption != -1)
        {
            iCurrentOption--;
        }
        FlushThresholdSetOption();
        RefreshScreen(__FILE__, __func__, __LINE__);
        break;
    case KEYCODE_DOWN:
        if (iCurrentOption != (THRESHOLDSET_CURVE_SET_NUM + THRESHOLDSET_ANALYS_SET_NUM))
        {
            iCurrentOption++;
        }
        FlushThresholdSetOption();
        RefreshScreen(__FILE__, __func__, __LINE__);
        break;
    case KEYCODE_ENTER:
        HandleEnterKeyCallBack();
        break;
    case KEYCODE_START:
        //先退出当前界面
        ThresholdSetBtnBack_Up(NULL, 0, NULL, 0);
        //调用测试接口
        OtdrTest(0);
        break;
    case KEYCODE_ESC:
        ThresholdSetBtnBack_Up(NULL, 0, NULL, 0);
        break;
    }

    return 0;
}


/*******************************************************************************
*                   窗体FrmThresholdSet内部功能函数声明
*******************************************************************************/
/***
  * 功能：
        初始化参数
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void ThresholdSetInitPara(void)
{
    //申请内存
    if(pCurveSetControl== NULL)
    {
        pCurveSetControl= (struct curve_set_control*)calloc(1, sizeof(struct curve_set_control));
        memset(pCurveSetControl, 0, sizeof(struct curve_set_control));
    }

    if(pAnalysSetControl== NULL)
    {
        pAnalysSetControl= (struct analys_set_control*)calloc(1, sizeof(struct analys_set_control));
        memset(pAnalysSetControl, 0, sizeof(struct analys_set_control));
    }

    //获取曲线通过/不通过结构体
    pOtdrPassThreshold = &pUser_Settings->sOtherSetting;
    if(pOtdrPassThreshold->iWaveLength == WAVELEN_1625)
    {
        pOtdrPassThreshold->iWaveLength = WAVELEN_1310;
    }
    //获取分析设置结构体
    pOtdrAnalysisSet = &pUser_Settings->sAnalysisSetting;
}

/***
  * 功能：
        清除参数（保存系统参数）
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void ThresholdSetClearPara(void)
{
    GuiMemFree(pCurveSetControl);
    GuiMemFree(pAnalysSetControl);
}

/***
  * 功能：
        设置曲线阈值参数的使能（统一设置）
  * 参数：
        1.int iWaveLength:需要设置的波长index
        2.int iEnabled：0，1
  * 返回：
        无
  * 备注：
***/
static void SetCurveThresholdEnable(int iWaveLength, int iEnabled)
{
    pOtdrPassThreshold->PassThr.SpliceLossThr[iWaveLength].iSlectFlag = iEnabled;//接头损耗
    pOtdrPassThreshold->PassThr.ConnectLossThr[iWaveLength].iSlectFlag = iEnabled;//连接器损耗
    pOtdrPassThreshold->PassThr.ReturnLossThr[iWaveLength].iSlectFlag = iEnabled;//反射率
    pOtdrPassThreshold->PassThr.AttenuationThr[iWaveLength].iSlectFlag = iEnabled;//衰减
    pOtdrPassThreshold->PassThr.RegionLossThr[iWaveLength].iSlectFlag = iEnabled;//跨段损耗
    pOtdrPassThreshold->PassThr.RegionLengthThr[iWaveLength].iSlectFlag = iEnabled;//跨段长度
    pOtdrPassThreshold->PassThr.RegionReturnLossThr[iWaveLength].iSlectFlag = iEnabled;//跨段ORL
}

/***
  * 功能：
		设置开关的状态
  * 参数：
		1.int iEnabled:开关的使能（0，1）
		2.int thresholdtype：阈值类型（curve，analysis）
		3.int iSelectedOption：选中时的index
  * 返回：
		无
  * 备注：
***/
static void SetSwitchStatus(int iEnabled, int thresholdtype, int iSelectedIndex)
{
	if (iEnabled)
	{
		if (iCurrentOption == iSelectedIndex)
		{
            SetPictureBitmap(BmpFileDirectory"btn_on_select.bmp", pFrmThresholdSetBtnSwitch[thresholdtype]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"btn_on_unpress.bmp", pFrmThresholdSetBtnSwitch[thresholdtype]);
		}
	}
	else
	{
		if (iCurrentOption == iSelectedIndex)
		{
			SetPictureBitmap(BmpFileDirectory"btn_off_select.bmp", pFrmThresholdSetBtnSwitch[thresholdtype]);
		}
		else
		{
			SetPictureBitmap(BmpFileDirectory"btn_off_unpress.bmp", pFrmThresholdSetBtnSwitch[thresholdtype]);
		}
	}
}

/***
  * 功能：
		刷新阈值设置各个选项
  * 参数：无
  * 返回：
		无
  * 备注：
***/
static void FlushThresholdSetOption(void)
{
	int i;
	int iWaveLength = pOtdrPassThreshold->iWaveLength;
	int iCurveEnable = pOtdrPassThreshold->PassThr.iThrEnable[iWaveLength];
	int iAnalysEnable = pOtdrAnalysisSet->iEnableAnalysisThreshold;//默认值

    //设置开关背景
    SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp", pFrmThresholdSetBgCurveSelect);
    SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp", pFrmThresholdSetBgAnalysisSelect);

    if (iCurrentOption == -1)
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp", pFrmThresholdSetBgCurveSelect);
    if (iCurrentOption == THRESHOLDSET_CURVE_SET_NUM)
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp", pFrmThresholdSetBgAnalysisSelect);

    for (i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
	{
		if (i < THRESHOLDSET_ANALYS_SET_NUM)
		{
			SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pAnalysSetControl->pBtnSelect[i]);
			SetPictureBitmap(BmpFileDirectory"bg_sys_keyboard.bmp", pAnalysSetControl->pBtnEnterIcon[i]);
            SetPictureBitmap(pStrAnalysisThresholdButton_unpress[i], pAnalysSetControl->pBtnIcon[i]);
        }

		SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pCurveSetControl->pBtnSelect[i]);
		SetPictureBitmap(BmpFileDirectory"bg_sys_keyboard.bmp", pCurveSetControl->pBtnEnterIcon[i]);
        SetPictureBitmap(pStrCurveThresholdButton_unpress[i], pCurveSetControl->pBtnIcon[i]);
    }

	if (iCurrentOption < THRESHOLDSET_CURVE_SET_NUM)
	{
		if (iCurrentOption == -1)
		{
			SetSwitchStatus(iCurveEnable, CURVE_THRESHOLD, -1);
		}
		else
		{
			SetSwitchStatus(iCurveEnable, CURVE_THRESHOLD, -1);
			SetSwitchStatus(iAnalysEnable, ANALYS_THRESHOLD, THRESHOLDSET_CURVE_SET_NUM);
			SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pCurveSetControl->pBtnSelect[iCurrentOption]);
			SetPictureBitmap(BmpFileDirectory"bg_sys_keyboard_press.bmp", pCurveSetControl->pBtnEnterIcon[iCurrentOption]);
            SetPictureBitmap(pStrCurveThresholdButton_press[iCurrentOption], pCurveSetControl->pBtnIcon[iCurrentOption]);
        }
	}
	else
	{
		if (iCurrentOption == THRESHOLDSET_CURVE_SET_NUM)
		{
			SetSwitchStatus(iAnalysEnable, ANALYS_THRESHOLD, THRESHOLDSET_CURVE_SET_NUM);
		}
		else
		{
			SetSwitchStatus(iAnalysEnable, ANALYS_THRESHOLD, THRESHOLDSET_CURVE_SET_NUM);
			SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp",
				pAnalysSetControl->pBtnSelect[iCurrentOption-(THRESHOLDSET_CURVE_SET_NUM+1)]);
			SetPictureBitmap(BmpFileDirectory"bg_sys_keyboard_press.bmp",
				pAnalysSetControl->pBtnEnterIcon[iCurrentOption - (THRESHOLDSET_CURVE_SET_NUM+1)]);
            SetPictureBitmap(pStrAnalysisThresholdButton_press[iCurrentOption - (THRESHOLDSET_CURVE_SET_NUM + 1)],
                             pAnalysSetControl->pBtnIcon[iCurrentOption - (THRESHOLDSET_CURVE_SET_NUM + 1)]);
        }
	}

	for (i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)//THRESHOLDSET_CURVE_SET_NUM
	{
		if (i < THRESHOLDSET_ANALYS_SET_NUM)
		{
			DisplayPicture(pAnalysSetControl->pBtnSelect[i]);
			DisplayLabel(pAnalysSetControl->pLblTitle[i]);
			SetPictureEnable(1, pAnalysSetControl->pBtnEnterIcon[i]);
			DisplayPicture(pAnalysSetControl->pBtnEnterIcon[i]);
            DisplayPicture(pAnalysSetControl->pBtnIcon[i]);

            if (pOtdrAnalysisSet->iEnableAnalysisThreshold == 0)
            {
                //不使能键盘
                if (iCurrentOption == (i + THRESHOLDSET_CURVE_SET_NUM + 1))
                    SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                     pAnalysSetControl->pBtnEnterIcon[i]);
                else
                    SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp",
                                     pAnalysSetControl->pBtnEnterIcon[i]);
                DisplayPicture(pAnalysSetControl->pBtnEnterIcon[i]);
				SetPictureEnable(0, pAnalysSetControl->pBtnEnterIcon[i]);
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pAnalysSetControl->pLblData[i]);
            }

            DisplayLabel(pAnalysSetControl->pLblData[i]);
		}

		//显示曲线阈值设置控件
		DisplayPicture(pCurveSetControl->pBtnSelect[i]);
		DisplayLabel(pCurveSetControl->pLblTitle[i]);
		SetPictureEnable(1, pCurveSetControl->pBtnEnterIcon[i]);
		DisplayPicture(pCurveSetControl->pBtnEnterIcon[i]);
        DisplayPicture(pCurveSetControl->pBtnIcon[i]);

        if (pOtdrPassThreshold->PassThr.iThrEnable[pOtdrPassThreshold->iWaveLength] == 0)
        {
            //不使能键盘
            if (iCurrentOption == i)
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            else
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            DisplayPicture(pCurveSetControl->pBtnEnterIcon[i]);
			SetPictureEnable(0, pCurveSetControl->pBtnEnterIcon[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pCurveSetControl->pLblData[i]);
        }

        DisplayLabel(pCurveSetControl->pLblData[i]);
	}

    DisplayPicture(pFrmThresholdSetBgCurveSelect);
    DisplayPicture(pFrmThresholdSetBgAnalysisSelect);
    DisplayLabel(pFrmThresholdSetLblCurveTitle);
    DisplayLabel(pFrmThresholdSetLblAnalysTitle);
    DisplayPicture(pFrmThresholdSetBtnSwitch[CURVE_THRESHOLD]);
	DisplayPicture(pFrmThresholdSetBtnSwitch[ANALYS_THRESHOLD]);
}

/***
  * 功能：
        设置参数显示信息
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void SetParaDisplayInfo(void)
{
    int iCurWaveLength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(iCurWaveLength == WAVELEN_1625)
    {
        iCurWaveLength = WAVELEN_1310;
    }
    //曲线阈值
    //1.接头损耗
    pCurveSetControl->pStrData[SPLICE_LOSS_THRESHOLD] = Float2GuiStringUnit(LOSS_PRECISION, 
	                    pOtdrPassThreshold->PassThr.SpliceLossThr[iCurWaveLength].fThrValue, " db");
    SetLabelText(pCurveSetControl->pStrData[SPLICE_LOSS_THRESHOLD],
                        pCurveSetControl->pLblData[SPLICE_LOSS_THRESHOLD]);
    //2.连接器损耗
    pCurveSetControl->pStrData[CONNECT_LOSS_THRESHOLD] = Float2GuiStringUnit(LOSS_PRECISION, 
                        pOtdrPassThreshold->PassThr.ConnectLossThr[iCurWaveLength].fThrValue, " db");
    SetLabelText(pCurveSetControl->pStrData[CONNECT_LOSS_THRESHOLD],
                        pCurveSetControl->pLblData[CONNECT_LOSS_THRESHOLD]);
    //3.反射率
    pCurveSetControl->pStrData[RETURN_LOSS_THRESHOLD] = Float2GuiStringUnit(REFLECTANCE_PRECISION, 
                        pOtdrPassThreshold->PassThr.ReturnLossThr[iCurWaveLength].fThrValue, " db");
    SetLabelText(pCurveSetControl->pStrData[RETURN_LOSS_THRESHOLD],
                        pCurveSetControl->pLblData[RETURN_LOSS_THRESHOLD]);
    //4.衰减
    pCurveSetControl->pStrData[ATTENUATION_THRESHOLD] = Float2GuiStringUnit(ATTENUATION_PRECISION, 
                        pOtdrPassThreshold->PassThr.AttenuationThr[iCurWaveLength].fThrValue, " db/km");
    SetLabelText(pCurveSetControl->pStrData[ATTENUATION_THRESHOLD],
                        pCurveSetControl->pLblData[ATTENUATION_THRESHOLD]);
    //5.跨段损耗
    pCurveSetControl->pStrData[REGION_LOSS_THRESHOLD] = Float2GuiStringUnit(LOSS_PRECISION, 
                        pOtdrPassThreshold->PassThr.RegionLossThr[iCurWaveLength].fThrValue, " db");
    SetLabelText(pCurveSetControl->pStrData[REGION_LOSS_THRESHOLD],
                        pCurveSetControl->pLblData[REGION_LOSS_THRESHOLD]);
    //6.跨段长度
    pCurveSetControl->pStrData[REGION_LENGTH_THRESHOLD] = 
        UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR,
	    pOtdrPassThreshold->PassThr.fSpanLength_m[iCurWaveLength], 
	    pOtdrPassThreshold->PassThr.fSpanLength_ft[iCurWaveLength], 
	    pOtdrPassThreshold->PassThr.fSpanLength_mi[iCurWaveLength], 1);
    SetLabelText(pCurveSetControl->pStrData[REGION_LENGTH_THRESHOLD],
                        pCurveSetControl->pLblData[REGION_LENGTH_THRESHOLD]);
    //7.跨段ORL
    pCurveSetControl->pStrData[REGION_RETURN_LOSS_THRESHOLD] = Float2GuiStringUnit(ORL_PRECISION, 
                        pOtdrPassThreshold->PassThr.RegionReturnLossThr[iCurWaveLength].fThrValue, " db");
    SetLabelText(pCurveSetControl->pStrData[REGION_RETURN_LOSS_THRESHOLD],
                        pCurveSetControl->pLblData[REGION_RETURN_LOSS_THRESHOLD]);
    //分析阈值设置
    //1.熔接损耗
	pAnalysSetControl->pStrData[SPLICE_LOSS_ANALYS_THRESHOLD] = Float2GuiString(LOSS_PRECISION, pOtdrAnalysisSet->fSpliceLossThr);
	SetLabelText(pAnalysSetControl->pStrData[SPLICE_LOSS_ANALYS_THRESHOLD],
		pAnalysSetControl->pLblData[SPLICE_LOSS_ANALYS_THRESHOLD]);
	//2.光纤末端阈值
	pAnalysSetControl->pStrData[END_LOSS_ANALYS_THRESHOLD] = Float2GuiString(LOSS_PRECISION, pOtdrAnalysisSet->fEndLossThr);
	SetLabelText(pAnalysSetControl->pStrData[END_LOSS_ANALYS_THRESHOLD],
		pAnalysSetControl->pLblData[END_LOSS_ANALYS_THRESHOLD]);
}

/***
  * 功能：
        刷新曲线阈值参数显示信息
  * 参数：
        int iEnable:曲线阈值的使能状态（0，1）
  * 返回：
        无
  * 备注：
***/
static void FlushCurveParaDisplayInfo(int iEnable)
{
    int i;
    for(i = 0; i < THRESHOLDSET_CURVE_SET_NUM; ++i)
    {
        DisplayPicture(pCurveSetControl->pBtnSelect[i]);
        DisplayPicture(pCurveSetControl->pBtnIcon[i]);
        DisplayLabel(pCurveSetControl->pLblTitle[i]);
        if(iEnable)
        {
            //使能键盘显示
            if (iCurrentOption == i)
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            else
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveSetControl->pLblData[i]);
            DisplayLabel(pCurveSetControl->pLblData[i]);
            //此处需设置图片的状态
            SetPictureEnable(1, pCurveSetControl->pBtnEnterIcon[i]);
            DisplayPicture(pCurveSetControl->pBtnEnterIcon[i]);
        }
        else
        {
            //考虑键盘不使能情况
            if (iCurrentOption == i)
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            else
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp",
                                 pCurveSetControl->pBtnEnterIcon[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pCurveSetControl->pLblData[i]);
            DisplayLabel(pCurveSetControl->pLblData[i]);
            //此处需设置图片的状态
            DisplayPicture(pCurveSetControl->pBtnEnterIcon[i]);
            SetPictureEnable(0, pCurveSetControl->pBtnEnterIcon[i]);
        }
    }

    RefreshScreen(__FILE__, __func__, __LINE__);
}

/***
  * 功能：
        刷新分析阈值参数显示信息
  * 参数：
        int iEnable:曲线阈值的使能状态（0，1）
  * 返回：
        无
  * 备注：
***/
static void FlushAnalysParaDisplayInfo(int iEnable)
{
    int i;
    for(i = 0; i < THRESHOLDSET_ANALYS_SET_NUM; ++i)
    {
        DisplayPicture(pAnalysSetControl->pBtnSelect[i]);
        DisplayPicture(pAnalysSetControl->pBtnIcon[i]);
        DisplayLabel(pAnalysSetControl->pLblTitle[i]);
        if(iEnable)
        {
            //使能键盘显示
            if (iCurrentOption == (i + THRESHOLDSET_CURVE_SET_NUM + 1))
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp",
                                 pAnalysSetControl->pBtnEnterIcon[i]);
            else
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp",
                                 pAnalysSetControl->pBtnEnterIcon[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pAnalysSetControl->pLblData[i]);
            DisplayLabel(pAnalysSetControl->pLblData[i]);
            //此处需设置图片的状态
            SetPictureEnable(1, pAnalysSetControl->pBtnEnterIcon[i]);
            DisplayPicture(pAnalysSetControl->pBtnEnterIcon[i]);
        }
        else
        {
            //考虑键盘不使能情况
            if (iCurrentOption == (i + THRESHOLDSET_CURVE_SET_NUM + 1))
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                 pAnalysSetControl->pBtnEnterIcon[i]);
            else
                SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp",
                                 pAnalysSetControl->pBtnEnterIcon[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pAnalysSetControl->pLblData[i]);
            DisplayLabel(pAnalysSetControl->pLblData[i]);
            //此处需设置图片的状态
            DisplayPicture(pAnalysSetControl->pBtnEnterIcon[i]);
            SetPictureEnable(0, pAnalysSetControl->pBtnEnterIcon[i]);
        }
    }

    RefreshScreen(__FILE__, __func__, __LINE__);
}

/***
  * 功能：
        阈值设置范围的提示框
  * 参数：
        1.char *min:最小值
        2.char *max:最大值
  * 返回：
        无
  * 备注：
***/
static void ThresholdSetIMEDialog(char *min, char *max)
{
    GUICHAR* curText1 = GetCurrLanguageText(OTDR_SET_LBL_ERROR_ENTER);
    GUICHAR* curText2 = TransString(".");
    GUICHAR* curAnd = TransString(" and ");
    GUICHAR* string1Mem = TransString(min);
    GUICHAR* string2Mem = TransString(max);
    GUICHAR* warningData1 = StringCat(&curText1,string1Mem);
    GUICHAR* warningData2 = StringCat(&warningData1,curAnd);
    GUICHAR* warningData3 = StringCat(&warningData2,string2Mem);
    GUICHAR* warningData = StringCat(&warningData3,curText2);
    
    CreateIMEDialog(warningData);
    
    GuiMemFree(curText2);
    GuiMemFree(curAnd);
    GuiMemFree(string1Mem);
    GuiMemFree(string2Mem);
}

/***
  * 功能：
        重绘窗体函数
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void ReCreateThresholdSetWindow(GUIWINDOW **pWnd)
{	
   *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                        FrmThresholdSetInit, FrmThresholdSetExit, 
                        FrmThresholdSetPaint, FrmThresholdSetLoop, 
				        FrmThresholdSetPause, FrmThresholdSetResume,
                        NULL);
}


/***
  * 功能：
        输入接头损耗后的回调函数(曲线阈值)
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void InputSpliceLossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	if (fTmp < curveThrRangeValues[SPLICE_LOSS_THRESHOLD][MIN_RANGE_INDEX] 
        || fTmp > curveThrRangeValues[SPLICE_LOSS_THRESHOLD][MAX_RANGE_INDEX])
	{
	    char *min = Float2String(LOSS_PRECISION, 
            curveThrRangeValues[SPLICE_LOSS_THRESHOLD][MIN_RANGE_INDEX]);
	    char *max = Float2String(LOSS_PRECISION,
            curveThrRangeValues[SPLICE_LOSS_THRESHOLD][MAX_RANGE_INDEX]);
		ThresholdSetIMEDialog(min, max);
        GuiMemFree(min);
		GuiMemFree(max);
		return;
	}

	pOtdrPassThreshold->PassThr.SpliceLossThr[wavelength].fThrValue = fTmp;
}

/***
  * 功能：
        输入连接器损耗后的回调函数(曲线阈值)
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void InputConnectLossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
    
	if (fTmp < curveThrRangeValues[CONNECT_LOSS_THRESHOLD][MIN_RANGE_INDEX] 
                || fTmp > curveThrRangeValues[CONNECT_LOSS_THRESHOLD][MAX_RANGE_INDEX])
	{
	    char *min = Float2String(LOSS_PRECISION, 
                    curveThrRangeValues[CONNECT_LOSS_THRESHOLD][MIN_RANGE_INDEX]);
	    char *max = Float2String(LOSS_PRECISION, 
                    curveThrRangeValues[CONNECT_LOSS_THRESHOLD][MAX_RANGE_INDEX]);
		ThresholdSetIMEDialog(min, max);
		GuiMemFree(min);
		GuiMemFree(max);
		return;
	}

	pOtdrPassThreshold->PassThr.ConnectLossThr[wavelength].fThrValue = fTmp;
}

/***
  * 功能：
        输入回波损耗（反射率）后的回调函数(曲线阈值)
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void InputReturnLossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < curveThrRangeValues[RETURN_LOSS_THRESHOLD][MIN_RANGE_INDEX] 
        || fTmp > curveThrRangeValues[RETURN_LOSS_THRESHOLD][MAX_RANGE_INDEX])
	{
        char *min = Float2String(REFLECTANCE_PRECISION, 
                    curveThrRangeValues[RETURN_LOSS_THRESHOLD][MIN_RANGE_INDEX]);
        char *max = Float2String(REFLECTANCE_PRECISION, 
                    curveThrRangeValues[RETURN_LOSS_THRESHOLD][MAX_RANGE_INDEX]);
        ThresholdSetIMEDialog(min, max);
        GuiMemFree(min);
        GuiMemFree(max);
		return;
	}

	pOtdrPassThreshold->PassThr.ReturnLossThr[wavelength].fThrValue = fTmp;
}

/***
  * 功能：
        输入衰减后的回调函数(曲线阈值)
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void InputAttenuationBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < curveThrRangeValues[ATTENUATION_THRESHOLD][MIN_RANGE_INDEX] 
        || fTmp > curveThrRangeValues[ATTENUATION_THRESHOLD][MAX_RANGE_INDEX])
	{
        char *cmin = Float2String(ATTENUATION_PRECISION, 
                        curveThrRangeValues[ATTENUATION_THRESHOLD][MIN_RANGE_INDEX]);
        char *cmax = Float2String(ATTENUATION_PRECISION, 
                        curveThrRangeValues[ATTENUATION_THRESHOLD][MAX_RANGE_INDEX]);
        ThresholdSetIMEDialog(cmin, cmax);
        GuiMemFree(cmin);
        GuiMemFree(cmax);
		return;
	}

    pOtdrPassThreshold->PassThr.AttenuationThr[wavelength].fThrValue = fTmp;
}

/***
  * 功能：
        输入跨段损耗后的回调函数(曲线阈值)
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void InputRegionLossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < curveThrRangeValues[REGION_LOSS_THRESHOLD][MIN_RANGE_INDEX] 
        || fTmp > curveThrRangeValues[REGION_LOSS_THRESHOLD][MAX_RANGE_INDEX])
	{
        char *min = Float2String(LOSS_PRECISION, 
                        curveThrRangeValues[REGION_LOSS_THRESHOLD][MIN_RANGE_INDEX]);
        char *max = Float2String(LOSS_PRECISION, 
                        curveThrRangeValues[REGION_LOSS_THRESHOLD][MAX_RANGE_INDEX]);
        ThresholdSetIMEDialog(min, max);
        GuiMemFree(min);
        GuiMemFree(max);
		return;
	}

	pOtdrPassThreshold->PassThr.RegionLossThr[wavelength].fThrValue = fTmp;
}

/***
  * 功能：
        输入跨段长度后的回调函数(曲线阈值)
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void InputRegionLengthBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

    char *spanLengthUpper[UNIT_COUNT] = {(char*)"300000.0", (char*)"300.0000",
        (char*)"984252.0", (char*)"984.2520", (char*)"186.4114", (char*)"328083.9895013"};
	double fTmp = String2Float(cTmpBuff);
	double min, max;

    min = 0.0f;
    max = String2Float(spanLengthUpper[GetCurrSystemUnit(MODULE_UNIT_OTDR)]);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < min || fTmp > max)
	{
        char *cmin = Float2String(GetDistancePrecision(MODULE_UNIT_OTDR), min);
        char *cmax = Float2String(GetDistancePrecision(MODULE_UNIT_OTDR), max);
        ThresholdSetIMEDialog(cmin, cmax);
        GuiMemFree(cmin);
        GuiMemFree(cmax);
		return;
	}

    pOtdrPassThreshold->PassThr.fSpanLength_m[wavelength] = 
        UnitConverter_Dist_System2M_String2Float(MODULE_UNIT_OTDR, cTmpBuff);
    max = String2Float(spanLengthUpper[UNIT_M]);
    if(pOtdrPassThreshold->PassThr.fSpanLength_m[wavelength] > max)
    {
       pOtdrPassThreshold->PassThr.fSpanLength_m[wavelength] = max; 
    }
    
    pOtdrPassThreshold->PassThr.fSpanLength_ft[wavelength] = 
        UnitConverter_Dist_System2FT_String2Float(MODULE_UNIT_OTDR, cTmpBuff);
    max = String2Float(spanLengthUpper[UNIT_FT]);
    if(pOtdrPassThreshold->PassThr.fSpanLength_ft[wavelength] > max)
    {
       pOtdrPassThreshold->PassThr.fSpanLength_ft[wavelength] = max; 
    }
    
    pOtdrPassThreshold->PassThr.fSpanLength_mi[wavelength] = 
        UnitConverter_Dist_System2MI_String2Float(MODULE_UNIT_OTDR, cTmpBuff);
    max = String2Float(spanLengthUpper[UNIT_MI]);
    if(pOtdrPassThreshold->PassThr.fSpanLength_mi[wavelength] > max)
    {
       pOtdrPassThreshold->PassThr.fSpanLength_mi[wavelength] = max; 
    }
}

/***
  * 功能：
        输入跨段ORL（回波损耗）后的回调函数(曲线阈值)
  * 参数：
        无
  * 返回：
        无
  * 备注：
***/
static void InputRegionReturnLossBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	float fTmp = atof(cTmpBuff);

	WAVELEN wavelength = pOtdrPassThreshold->iWaveLength;
    //使用3110nm取代1625nm
    if(wavelength == WAVELEN_1625)
    {
        wavelength = WAVELEN_1310;
    }
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < curveThrRangeValues[REGION_RETURN_LOSS_THRESHOLD][MIN_RANGE_INDEX] 
        || fTmp > curveThrRangeValues[REGION_RETURN_LOSS_THRESHOLD][MAX_RANGE_INDEX])
	{
	    
        char *min = Float2String(ORL_PRECISION,
                    curveThrRangeValues[REGION_RETURN_LOSS_THRESHOLD][MIN_RANGE_INDEX]);
        char *max = Float2String(ORL_PRECISION,
                    curveThrRangeValues[REGION_RETURN_LOSS_THRESHOLD][MAX_RANGE_INDEX]);
        ThresholdSetIMEDialog(min, max);
        GuiMemFree(min);
        GuiMemFree(max);
		return;
	}

	pOtdrPassThreshold->PassThr.RegionReturnLossThr[wavelength].fThrValue = fTmp;
}

/***
  * 功能：
		输入熔接损耗后的回调函数(分析阈值)
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void InputAnalysisSpliceLossBackFunc(void)
{
	char cTmpBuff[100];
	GetIMEInputBuff(cTmpBuff);
	float fTmp = atof(cTmpBuff);
	//此处需要检查参数，暂时省略
	//0.010 ~ 5.00
	if (fTmp < 0.010f || fTmp > 5.0f)
	{
		CreateIMEDialog(GetCurrLanguageText(IME_LBL_SPLICLOSS_LIMIT));
		return;
	}

	if (pOtdrTopSettings->pDisplayInfo->iCurveNum != 0)
	{
		if (fabsf(fTmp - pOtdrAnalysisSet->fSpliceLossThr) < 0.001f)
		{
			return;
		}

		iReAnalysisFlag = 1;
	}

	pOtdrAnalysisSet->fSpliceLossThr = fTmp;
}

/***
  * 功能：
		输入光纤末端阈值后的回调函数(分析阈值)
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void InputEndlossBackFunc(void)
{
	char cTmpBuff[100];
	GetIMEInputBuff(cTmpBuff);
	float fTmp = atof(cTmpBuff);

	//此处需要检查参数，暂时省略
	//1 ~ 25
	if (fTmp < 1.0f || fTmp > 25.0f)
	{
		CreateIMEDialog(GetCurrLanguageText(IME_LBL_ENDLOSS_LIMIT));
		return;
	}

	if (pOtdrTopSettings->pDisplayInfo->iCurveNum != 0)
	{
		if (fabsf(fTmp - pUser_Settings->sAnalysisSetting.fEndLossThr) < 0.001f)
		{
			return;
		}

		iReAnalysisFlag = 1;
	}

	pOtdrAnalysisSet->fEndLossThr = fTmp;
}

/***
  * 功能：
		重新事件分析显示对话框
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void DialogReAnalysisBackFunc(void)
{
	ShowDialogExtraInfo(GetCurrLanguageText(OTDR_SET_REEVENT_ANALYZE));
	ReeventAnalyze();
}

//处理enter按键响应
static void HandleEnterKeyCallBack(void)
{
	if (iCurrentOption < THRESHOLDSET_CURVE_SET_NUM)
	{
		if (iCurrentOption == -1)
		{
			ThresholdSetBtnSwitch_Up(NULL, 0, NULL, CURVE_THRESHOLD);
		}
		else
		{
			ThresholdSetBtnCurveSet_Up(NULL, 0, NULL, iCurrentOption);
		}
	}
	else
	{
		if (iCurrentOption == THRESHOLDSET_CURVE_SET_NUM)
		{
			ThresholdSetBtnSwitch_Up(NULL, 0, NULL, ANALYS_THRESHOLD);
		}
		else
		{
			ThresholdSetBtnAnalys_Up(NULL, 0, NULL, iCurrentOption - (THRESHOLDSET_CURVE_SET_NUM + 1));
		}
	}
}