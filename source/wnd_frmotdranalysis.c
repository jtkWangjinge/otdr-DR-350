/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdranalysis.c
* 摘    要：  实现主窗体FrmOtdrAnalysisSet的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/8
*
*******************************************************************************/

#include "wnd_frmotdranalysis.h"

#include "guiwindow.h"
#include "guipicture.h"

#include "app_frminit.h"
#include "app_otdr_algorithm_parameters.h"
#include "app_unitconverter.h"
#include "app_systemsettings.h"

#include "wnd_stack.h"
#include "wnd_frmime.h"
#include "wnd_frmotdr.h"
#include "wnd_frmcommonset.h"
#include "wnd_frmthresholdset.h"
#include "wnd_frmreportset.h"

/*************************************
* 定义wnd_frmotdranalysis.c中内部数据结构
*************************************/

#define LAUNCH_RECEIVE_FIBER_OPTION		4

enum Analysis_Set_Option
{
	REFRACTIVE = 0,		//折射率
	BACKSCATTER,		//背向散射
	COEFFICIENT_LENGTH, //余长系数
	ANALYSIS_COUNT
};

//注入光纤和接收光纤结构
struct OtdrFiberSet
{
	int     iAutoAnlys;					//自动分析标志
	int     iEnableLaunchFiber;			//注入光纤使能
	double  fLaunchFiberLen;			//注入光纤长度
	int     iEnableRecvFiber;			//接收光纤使能
	double  fRecvFiberLen;				//接收光纤长度
	double  fLaunchFiberLen_ft;			//注入光纤长度
	double  fLaunchFiberLen_mi;			//注入光纤长度
	double  fRecvFiberLen_ft;			//接收光纤长度
	double  fRecvFiberLen_mi;			//接收光纤长度
	int     iEnableLaunchFiberEvent;	//注入光纤事件使能
	int     iEnableRecvFiberEvent;		//接收光纤事件使能
	int     iLaunchFiberEvent;			//注入光纤的事件编号
	int     iRecvFiberEvent;			//接收光纤的事件编号（从末端算起）
	int     iEventOrLength;				//按事件还是长度选项(0 长度， 非 0 事件)
    int     iEnableLaunchAndReceiveFiber;//注入光纤/接收光纤使能
};

typedef struct Otdr_Analysis_Set
{
	float   fRefrac[OTDR_WAVE_LENGTH_NUM];	//反射率	
	float   fBackSc[OTDR_WAVE_LENGTH_NUM];	//回波损耗
	float   fExceLen[OTDR_WAVE_LENGTH_NUM];	//余长系数
	struct OtdrFiberSet pOtdrFiberSet;		//注入光纤和接收光纤的结构
}OTDR_ANALYSIS;

/*******************************************************************************
**							窗体FrmOtdrAnalysisSet中的控件定义部分			  **
*******************************************************************************/
static GUIWINDOW *pFrmOtdrAnalysisSet = NULL;
//标题栏控件
static GUIPICTURE *pFrmOtdrAnalysisSetBg = NULL;
static GUIPICTURE *pFrmOtdrAnalysisSetBgIcon = NULL;
static GUIPICTURE *pFrmOtdrAnalysisSetBtnBack = NULL;
//当前界面的标题
static GUILABEL *pFrmOtdrAnalysisSetLblCurrTitle = NULL;               //当前界面的标题
static GUICHAR *pFrmOtdrAnalysisSetStrCurrTitle = NULL;
//功能区的标题
static GUIPICTURE *pFrmOtdrAnalysisSetBtnTitle[THRESHOLDSET_TITLE_NUM] = { NULL };
static GUILABEL *pFrmOtdrAnalysisSetLblTitle[THRESHOLDSET_TITLE_NUM] = { NULL };
static GUICHAR *pFrmOtdrAnalysisSetStrTitle[THRESHOLDSET_TITLE_NUM] = { NULL };
//功能区按钮
//选中背景
static GUIPICTURE* pFrmOtdrAnalysisSetBtnLeftSelected[ANALYSIS_COUNT] = {NULL};		//分析参数选中条
static GUIPICTURE *pFrmOtdrAnalysisSetBtnDefaultSelected[ANALYSIS_COUNT] = {NULL};	//默认设置选中条
static GUIPICTURE* pFrmOtdrAnalysisSetBtnEnterIcon[ANALYSIS_COUNT] = { NULL };
static GUIPICTURE *pFrmOtdrAnalysisSetBtnIcon[ANALYSIS_COUNT] = {NULL};				//分析参数图标
static GUIPICTURE *pBtnLaunchReceiveFiberIcon[2] = {NULL};							//注入/接收光纤图标
//默认值
static GUIPICTURE* pFrmOtdrAnalysisSetBtnDefault[ANALYSIS_COUNT] = { NULL };
static GUILABEL* pFrmOtdrAnalysisSetLblDefault[ANALYSIS_COUNT] = { NULL };
static GUICHAR* pFrmOtdrAnalysisSetStrDefault[ANALYSIS_COUNT] = { NULL };
//分析参数文本
static GUILABEL* pFrmOtdrAnalysisSetLblPara = NULL;							//分析参数
static GUILABEL* pFrmOtdrAnalysisSetLblParaInfo[ANALYSIS_COUNT] = { NULL };	//折射率//背向散射系数//余长系数
static GUICHAR *pFrmOtdrAnalysisSetStrPara = NULL;
static GUICHAR *pFrmOtdrAnalysisSetStrParaInfo[ANALYSIS_COUNT] = { NULL };
//注入光纤和接收光纤的按钮
static GUIPICTURE* pFrmOtdrAnalysisSetBtnFiberLength = NULL;				//按光纤长度
static GUIPICTURE* pFrmOtdrAnalysisSetBtnFiberEvent = NULL;					//按事件
static GUIPICTURE* pFrmOtdrAnalysisSetBtnLaunchFiberLength = NULL;			//注入光纤长度/注入光纤事件
static GUIPICTURE* pFrmOtdrAnalysisSetBtnReceiveFiberLength = NULL;			//接收光纤长度/接收光纤事件
static GUIPICTURE* pFrmOtdrAnalysisSetBtnIconLaunchFiberLength = NULL;		//注入光纤长度/注入光纤事件(键盘)
static GUIPICTURE* pFrmOtdrAnalysisSetBtnIconReceiveFiberLength = NULL;		//接收光纤长度/接收光纤事件(键盘)
//注入光纤和接收光纤的背景
static GUIPICTURE* pFrmOtdrAnalysisSetBtnLengthSelected = NULL;				//光纤长度背景
static GUIPICTURE *pFrmOtdrAnalysisSetBtnEventSelected = NULL;				//事件背景
static GUIPICTURE* pFrmOtdrAnalysisSetBtnLaunchFiberSelected = NULL;		//注入光纤背景
static GUIPICTURE* pFrmOtdrAnalysisSetBtnReceiveFiberSelected = NULL;		//接收光纤背景
//注入光纤和接收光纤的文本
static GUIPICTURE *pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber = NULL;      //注入光纤/接收光纤
static GUIPICTURE *pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiberSelected = NULL; //注入光纤/接收光纤背景
static GUILABEL* pFrmOtdrAnalysisSetLblLanuchAndReceiveFiber = NULL;		//注入光纤和接收光纤
static GUILABEL* pFrmOtdrAnalysisSetLblFiberLength = NULL;					//光纤长度
static GUILABEL* pFrmOtdrAnalysisSetLblFiberEvent = NULL;					//事件
static GUILABEL* pFrmOtdrAnalysisSetLblLanuchFiberLength = NULL;			//注入光纤长度/注入光纤事件
static GUILABEL* pFrmOtdrAnalysisSetLblReceiveFiberLength = NULL;			//接收光纤长度/接收光纤事件
static GUILABEL* pFrmOtdrAnalysisSetLblLanuchFiberLengthValue = NULL;		//注入光纤长度数值/注入光纤事件个数
static GUILABEL* pFrmOtdrAnalysisSetLblReceiveFiberLengthValue = NULL;		//接收光纤长度数值/接收光纤事件个数

static GUICHAR *pFrmOtdrAnalysisSetStrLanuchAndReceiveFiber = NULL;
static GUICHAR *pFrmOtdrAnalysisSetStrFiberLength = NULL;
static GUICHAR *pFrmOtdrAnalysisSetStrFiberEvent = NULL;
static GUICHAR *pFrmOtdrAnalysisSetStrLanuchFiberLength = NULL;
static GUICHAR *pFrmOtdrAnalysisSetStrReceiveFiberLength = NULL;
static GUICHAR *pFrmOtdrAnalysisSetStrLanuchFiberLengthValue = NULL;
static GUICHAR *pFrmOtdrAnalysisSetStrReceiveFiberLengthValue = NULL;
//参数显示数值
static GUILABEL* pFrmOtdrAnalysisSetLblParaValue[ANALYSIS_COUNT] = { NULL };//折射率//背向散射系数//余长系数
static GUICHAR *pFrmOtdrAnalysisSetStrParaValue[ANALYSIS_COUNT] = { NULL };

static OTDR_ANALYSIS *pOtdrAnalysisSet = NULL;						//自定义设置结构体
//当前选中项
static int iCurSelectOption = 0;//默认选中第一项
static int iDefaultOption[ANALYSIS_COUNT] = { 0, 0, 0 };			//默认值选中项
//默认值选项
static char *pDefaultBmp[2] = 
{
	BmpFileDirectory "btn_default_unchecked_unpress.bmp", 
	BmpFileDirectory "btn_default_checked_unpress.bmp"
};
static char* pDefaultBmpf[2] = 
{ 
	BmpFileDirectory"btn_default_unchecked_press.bmp", 
	BmpFileDirectory"btn_default_checked_press.bmp" 
};

//分析参数图标
static char *pAnalysisIconBmp[ANALYSIS_COUNT] = 
{
	BmpFileDirectory "btn_refractive_unpress.bmp", 
	BmpFileDirectory "btn_backscatter_unpress.bmp", 
	BmpFileDirectory "btn_helix_unpress.bmp"
};
static char* pAnalysisIconBmpf[ANALYSIS_COUNT] = 
{ 
	BmpFileDirectory"btn_refractive_press.bmp", 
	BmpFileDirectory"btn_backscatter_press.bmp", 
	BmpFileDirectory "btn_helix_press.bmp" 
};
//注入/接收光纤图标
static char *pFiberIconBmp[2] = 
{
	BmpFileDirectory "btn_lanuchFiber_unpress.bmp", 
	BmpFileDirectory "btn_receiveFiber_unpress.bmp"
};
static char* pFiberIconBmpf[2] = 
{ 
	BmpFileDirectory"btn_lanuchFiber_press.bmp", 
	BmpFileDirectory"btn_receiveFiber_press.bmp"
};

static int iOldOption = -1;//默认设置左侧的数据
/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int OtdrAnalysisSetTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//释放文本资源
static int OtdrAnalysisSetTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet内部按键响应函数声明
*******************************************************************************/
//返回按钮响应处理函数
static int OtdrAnalysisSetBtnBack_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int OtdrAnalysisSetBtnBack_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//菜单栏按键响应
static int OtdrAnalysisSetBtnTitle_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int OtdrAnalysisSetBtnTitle_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//功能区按键响应
//选中条按键响应处理
static int OtdrAnalysisSetBtnSelected_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int OtdrAnalysisSetBtnSelected_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//键盘响应处理
static int OtdrAnalysisSetBtnIcon_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int OtdrAnalysisSetBtnIcon_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//默认值响应处理
static int OtdrAnalysisSetBtnDefault_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
static int OtdrAnalysisSetBtnDefault_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//光纤长度响应处理
static int OtdrAnalysisSetBtnFiberLength_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//光纤事件响应处理
static int OtdrAnalysisSetBtnFiberEvent_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//注入光纤长度/注入光纤事件响应处理
static int OtdrAnalysisSetBtnLaunchFiberLength_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//接收光纤长度/接收光纤事件响应处理
static int OtdrAnalysisSetBtnReceiveFiberLength_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//注入光纤长度数值/注入光纤事件响应处理
static int OtdrAnalysisSetBtnLanuchFiberLengthValue_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//接收光纤长度数值/接收光纤事件响应处理
static int OtdrAnalysisSetBtnReceiveFiberLengthValue_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//接收光纤长度数值/接收光纤事件响应处理
static int OtdrAnalysisSetBtnLanuchAndReceiveFiber_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//点击按键响应函数
static int WndOtdrAnalysisSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndOtdrAnalysisSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet回调函数声明
*******************************************************************************/
//输入折射率后的回调函数
static void InputRefractiveBackFunc(void);
//输入背向散射系数后的回调函数
static void InputBackscatterBackFunc(void);
//输入余长系数后的回调函数
static void InputCoefficientBackFunc(void);
//注入光纤长度/事件设置
static void InputLaunchLenCallBack(void);
//接收光纤长度/事件设置
static void InputRecvLenCallBack(void);
//重绘窗体
static void ReCreateOtdrAnalysisSetWindow(GUIWINDOW **pWnd);
/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet内部参数相关功能函数声明
*******************************************************************************/
//初始化参数
static void OtdrAnalysisSetInitPara(void);
//清除参数(保存参数)
static void OtdrAnalysisSetClearPara(void);
//获取波长index
static int GetWaveLengthIndex(void);
//显示otdr分析设置后的信息
static void DisplayOtdrAnalysisSetInfo(void);
//设置默认值选项的状态
static void SetDefaultBtnStatus(int option, int iRefresh);
//刷新分析设置选项内容
static void FlushAnalysisOptionInfo(void);
//刷新注入光纤和接收光纤
static void FlushLanuchAndReceiveFiber(void);
//折射率的默认值响应函数
static void RefractiveDefaultValue(int iWaveLengthIndex);
//背向散射默认设置函数
static void BackscatterDefaultValue(int iWaveLengthIndex);
//余长系数默认设置函数
static void CoefficientDefaultValue(int iWaveLengthIndex);
//检查值是否合法并进行单位转换
static void AnalyzeConvertLaunchRecvLen(double in, double* out, double* out_ft, double* out_mi);
//处理注入/接收光纤
static void HandleLanuchReceiveFiberEnterKey(void);
//设置注入/接收光纤文本状态
static void SetLanuchAndReceiveFiberLblStatus(int iStatus);
//设置注入/接收光纤图标状态
static void SetLanuchAndReceiveFiberBmpStatus(int iIndex, int iStatus);
//设置注入/接收光纤数值图标状态
static void SetLanuchAndReceiveFiberValueBmpStatus(int iStatus);


/***
  * 功能：
		窗体frmotdranalysisset的初始化函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		建立窗体控件、注册消息处理
***/
int FrmOtdrAnalysisSetInit(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;
	pFrmOtdrAnalysisSet = (GUIWINDOW*)pWndObj;

	pFrmOtdrAnalysisSetBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
										  BmpFileDirectory"bg_otdr_analysis_set.bmp");
	pFrmOtdrAnalysisSetBgIcon = CreatePicture(0, 0, 23, 20,
											  BmpFileDirectory"bg_analysis_set_icon.bmp");
	pFrmOtdrAnalysisSetBtnBack = CreatePicture(0, 20, 36, 21,
											   BmpFileDirectory"btn_otdrSet_back.bmp");

	//初始化参数
	OtdrAnalysisSetInitPara();
	//初始化文本资源
	OtdrAnalysisSetTextRes_Init(NULL, 0, NULL, 0);
	//标题栏文本
	pFrmOtdrAnalysisSetLblCurrTitle = CreateLabel(8, 53, 100, 16,
												  pFrmOtdrAnalysisSetStrCurrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblCurrTitle);
	//菜单栏文本
	int i;
	for (i = 0; i < THRESHOLDSET_TITLE_NUM; ++i)
	{
		pFrmOtdrAnalysisSetBtnTitle[i] = CreatePicture(232 + 102 * i, 47, 99, 35,
													   pStrOtdrSetButton_unpress[i]);
		pFrmOtdrAnalysisSetLblTitle[i] = CreateLabel(232 + 102 * i + 5, 47 + 9, 90, 16,
													 pFrmOtdrAnalysisSetStrTitle[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblTitle[i]);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pFrmOtdrAnalysisSetLblTitle[i]);
	}
	//功能区图片
	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		pFrmOtdrAnalysisSetBtnLeftSelected[i] = CreatePicture(1, 83 + i * 45, 506, 44,
														  BmpFileDirectory "bg_sys_unselect.bmp");
		pFrmOtdrAnalysisSetBtnDefaultSelected[i] = CreatePicture(509, 83 + i * 45, 129, 44,
															  BmpFileDirectory "bg_sys_unselect.bmp");
		pFrmOtdrAnalysisSetBtnIcon[i] = CreatePicture(45, 95 + i * 45, 20, 20, pAnalysisIconBmp[i]);
		pFrmOtdrAnalysisSetBtnEnterIcon[i] = CreatePicture(470, 95 + i * 45, 22, 16,
													  BmpFileDirectory "bg_sys_keyboard.bmp");
		pFrmOtdrAnalysisSetBtnDefault[i] = CreatePicture(601, 95 + 45 * i, 20, 20,
														 BmpFileDirectory "btn_default_unchecked_unpress.bmp");

		pFrmOtdrAnalysisSetLblParaInfo[i] = CreateLabel(75, 97 + i * 45, 150, 16,
														pFrmOtdrAnalysisSetStrParaInfo[i]);
		pFrmOtdrAnalysisSetLblParaValue[i] = CreateLabel(340, 97 + i * 45, 150, 16,
														 pFrmOtdrAnalysisSetStrParaValue[i]);
		pFrmOtdrAnalysisSetLblDefault[i] = CreateLabel(529, 97 + 45 * i, 70, 16,
													   pFrmOtdrAnalysisSetStrDefault[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblDefault[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblParaInfo[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblParaValue[i]);
		//注入/接收光纤图标
		if (i < 2)
			pBtnLaunchReceiveFiberIcon[i] = CreatePicture(45, 365 + i * 45, 20, 20, pFiberIconBmp[i]);
	}
	//功能区文本
	pFrmOtdrAnalysisSetLblPara = CreateLabel(8, 53, 200, 16, pFrmOtdrAnalysisSetStrPara);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblPara);
	//注入光纤和接收光纤
	//背景
	pFrmOtdrAnalysisSetBtnLengthSelected = CreatePicture(1, 263 + 0 * 45, 637, 44,
														 BmpFileDirectory "bg_sys_unselect.bmp");
	pFrmOtdrAnalysisSetBtnEventSelected = CreatePicture(1, 263 + 1 * 45, 637, 44,
														BmpFileDirectory "bg_sys_unselect.bmp");
	pFrmOtdrAnalysisSetBtnLaunchFiberSelected = CreatePicture(1, 263 + 2 * 45, 637, 44,
															  BmpFileDirectory "bg_sys_unselect.bmp");
	pFrmOtdrAnalysisSetBtnReceiveFiberSelected = CreatePicture(1, 263 + 3 * 45, 637, 44,
															   BmpFileDirectory "bg_sys_unselect.bmp");
	//按钮
	int temp = 0;
    pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiberSelected = CreatePicture(1, 218, 637, 44,
                                                               BmpFileDirectory "bg_sys_unselect.bmp");
    pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber = CreatePicture(45, 218+12, 20, 20, 
                                                    pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber]);
    pFrmOtdrAnalysisSetLblLanuchAndReceiveFiber = CreateLabel(75, 230, 200, 16,
															  pFrmOtdrAnalysisSetStrLanuchAndReceiveFiber);
	pFrmOtdrAnalysisSetBtnFiberLength = CreatePicture(45, 275, 20, 20,
													  pDefaultBmp[(pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)]);
	pFrmOtdrAnalysisSetLblFiberLength = CreateLabel(75, 277, 150, 16,
													pFrmOtdrAnalysisSetStrFiberLength);
	pFrmOtdrAnalysisSetBtnFiberEvent = CreatePicture(45, 320, 20, 20,
													 pDefaultBmp[(pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength != 0)]);
	pFrmOtdrAnalysisSetLblFiberEvent = CreateLabel(75, 322, 150, 16,
												   pFrmOtdrAnalysisSetStrFiberEvent);
	//注入光纤
	temp = pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0 ? pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber
															   : pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent;
	pFrmOtdrAnalysisSetBtnLaunchFiberLength = CreatePicture(11, 3 + 65 + 6 * 16, 10, 10, pDefaultBmp[temp]);
	pFrmOtdrAnalysisSetLblLanuchFiberLength = CreateLabel(75, 367, 150, 16,
														  pFrmOtdrAnalysisSetStrLanuchFiberLength);
	pFrmOtdrAnalysisSetLblLanuchFiberLengthValue = CreateLabel(450, 367, 60, 16,
															   pFrmOtdrAnalysisSetStrLanuchFiberLengthValue);
	pFrmOtdrAnalysisSetBtnIconLaunchFiberLength = CreatePicture(579, 367, 22, 16,
																BmpFileDirectory "bg_sys_keyboard.bmp");
	//接收光纤
	temp = pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0 ? pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber
															   : pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent;
	pFrmOtdrAnalysisSetBtnReceiveFiberLength = CreatePicture(11, 3 + 65 + 7 * 16, 10, 10, pDefaultBmp[temp]);
	pFrmOtdrAnalysisSetLblReceiveFiberLength = CreateLabel(75, 412, 150, 16,
														   pFrmOtdrAnalysisSetStrReceiveFiberLength);
	pFrmOtdrAnalysisSetLblReceiveFiberLengthValue = CreateLabel(450, 412, 60, 16,
																pFrmOtdrAnalysisSetStrReceiveFiberLengthValue);
	pFrmOtdrAnalysisSetBtnIconReceiveFiberLength = CreatePicture(579, 412, 22, 16,
																 BmpFileDirectory "bg_sys_keyboard.bmp");

	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblLanuchAndReceiveFiber);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblFiberLength);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblFiberEvent);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblLanuchFiberLength);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblLanuchFiberLengthValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblReceiveFiberLength);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblReceiveFiberLengthValue);
	//注册窗体(因为所有的按键事件都统一由窗体进行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmOtdrAnalysisSet, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pFrmOtdrAnalysisSetBtnBack, pFrmOtdrAnalysisSet);
	//注册菜单栏
	for (i = 0; i < THRESHOLDSET_TITLE_NUM; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
					  pFrmOtdrAnalysisSetBtnTitle[i], pFrmOtdrAnalysisSet);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
					  pFrmOtdrAnalysisSetLblTitle[i], pFrmOtdrAnalysisSet);
	}
	//注册功能区
	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
					  pFrmOtdrAnalysisSetBtnLeftSelected[i], pFrmOtdrAnalysisSet);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
					  pFrmOtdrAnalysisSetLblParaInfo[i], pFrmOtdrAnalysisSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
					  pFrmOtdrAnalysisSetBtnEnterIcon[i], pFrmOtdrAnalysisSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
					  pFrmOtdrAnalysisSetBtnDefault[i], pFrmOtdrAnalysisSet);
		AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
					  pFrmOtdrAnalysisSetLblDefault[i], pFrmOtdrAnalysisSet);
	}
	//注入光纤和接收光纤功能注册
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pFrmOtdrAnalysisSetBtnFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
				  pFrmOtdrAnalysisSetLblFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pFrmOtdrAnalysisSetBtnFiberEvent, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
				  pFrmOtdrAnalysisSetLblFiberEvent, pFrmOtdrAnalysisSet);

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pFrmOtdrAnalysisSetBtnLaunchFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
				  pFrmOtdrAnalysisSetLblLanuchFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pFrmOtdrAnalysisSetBtnIconLaunchFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
				  pFrmOtdrAnalysisSetLblLanuchFiberLengthValue, pFrmOtdrAnalysisSet);

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pFrmOtdrAnalysisSetBtnReceiveFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
				  pFrmOtdrAnalysisSetLblReceiveFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
				  pFrmOtdrAnalysisSetBtnIconReceiveFiberLength, pFrmOtdrAnalysisSet);
	AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL),
				  pFrmOtdrAnalysisSetLblReceiveFiberLengthValue, pFrmOtdrAnalysisSet);

	//注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
	//即此处的操作应当在注册窗体控件的基础上进行
	//注册消息处理函数必须在注册窗体控件之后进行
	GUIMESSAGE *pMsg = GetCurrMessage();
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmOtdrAnalysisSetBtnBack,
					OtdrAnalysisSetBtnBack_Down, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnBack,
					OtdrAnalysisSetBtnBack_Up, NULL, 0, pMsg);
	for (i = 0; i < THRESHOLDSET_TITLE_NUM; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmOtdrAnalysisSetLblTitle[i],
						OtdrAnalysisSetBtnTitle_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblTitle[i],
						OtdrAnalysisSetBtnTitle_Up, NULL, i, pMsg);
	}
	//功能区
	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmOtdrAnalysisSetBtnLeftSelected[i],
						OtdrAnalysisSetBtnSelected_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnLeftSelected[i],
						OtdrAnalysisSetBtnSelected_Up, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmOtdrAnalysisSetBtnEnterIcon[i],
						OtdrAnalysisSetBtnIcon_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnEnterIcon[i],
						OtdrAnalysisSetBtnIcon_Up, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmOtdrAnalysisSetBtnDefault[i],
						OtdrAnalysisSetBtnDefault_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnDefault[i],
						OtdrAnalysisSetBtnDefault_Up, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pFrmOtdrAnalysisSetLblDefault[i],
						OtdrAnalysisSetBtnDefault_Down, NULL, i, pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblDefault[i],
						OtdrAnalysisSetBtnDefault_Up, NULL, i, pMsg);
	}
	//注入光纤和接收光纤的响应
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnFiberLength,
					OtdrAnalysisSetBtnFiberLength_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblFiberLength,
					OtdrAnalysisSetBtnFiberLength_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnFiberEvent,
					OtdrAnalysisSetBtnFiberEvent_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblFiberEvent,
					OtdrAnalysisSetBtnFiberEvent_Up, NULL, 0, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnLaunchFiberLength,
					OtdrAnalysisSetBtnLaunchFiberLength_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblLanuchFiberLength,
					OtdrAnalysisSetBtnLaunchFiberLength_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnIconLaunchFiberLength,
					OtdrAnalysisSetBtnLanuchFiberLengthValue_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblLanuchFiberLengthValue,
					OtdrAnalysisSetBtnLanuchFiberLengthValue_Up, NULL, 0, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnReceiveFiberLength,
					OtdrAnalysisSetBtnReceiveFiberLength_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblReceiveFiberLength,
					OtdrAnalysisSetBtnReceiveFiberLength_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetBtnIconReceiveFiberLength,
					OtdrAnalysisSetBtnReceiveFiberLengthValue_Up, NULL, 0, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pFrmOtdrAnalysisSetLblReceiveFiberLengthValue,
					OtdrAnalysisSetBtnReceiveFiberLengthValue_Up, NULL, 0, pMsg);

	//物理按键
	LoginMessageReg(GUIMESSAGE_KEY_DOWN, pFrmOtdrAnalysisSet,
					WndOtdrAnalysisSetKey_Down, NULL, 0, GetCurrMessage());
	LoginMessageReg(GUIMESSAGE_KEY_UP, pFrmOtdrAnalysisSet,
					WndOtdrAnalysisSetKey_Up, NULL, 0, GetCurrMessage());

	return iRet;
}

/***
  * 功能：
		窗体frmotdranalysisset的退出函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		释放所有资源
***/
int FrmOtdrAnalysisSetExit(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;
	//得到当前窗体对象
	pFrmOtdrAnalysisSet = (GUIWINDOW*)pWndObj;
	//清空消息队列中的消息注册项
	GUIMESSAGE* pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	//从当前窗体中注销窗体控件
	ClearWindowComp(pFrmOtdrAnalysisSet);
	//销毁控件
	DestroyPicture(&pFrmOtdrAnalysisSetBg);
	DestroyPicture(&pFrmOtdrAnalysisSetBgIcon);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnBack);
	//销毁标题栏文本
	DestroyLabel(&pFrmOtdrAnalysisSetLblCurrTitle);
	//销毁菜单栏
	int i = 0;
	for (; i < THRESHOLDSET_TITLE_NUM; ++i)
	{
		DestroyPicture(&pFrmOtdrAnalysisSetBtnTitle[i]);
		DestroyLabel(&pFrmOtdrAnalysisSetLblTitle[i]);
	}
	//销毁功能区
	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		DestroyPicture(&pFrmOtdrAnalysisSetBtnLeftSelected[i]);
		DestroyPicture(&pFrmOtdrAnalysisSetBtnDefaultSelected[i]);
		DestroyPicture(&pFrmOtdrAnalysisSetBtnIcon[i]);
		DestroyPicture(&pFrmOtdrAnalysisSetBtnEnterIcon[i]);
		DestroyPicture(&pFrmOtdrAnalysisSetBtnDefault[i]);
		DestroyLabel(&pFrmOtdrAnalysisSetLblDefault[i]);
		DestroyLabel(&pFrmOtdrAnalysisSetLblParaInfo[i]);
		DestroyLabel(&pFrmOtdrAnalysisSetLblParaValue[i]);
		//注入/接收光纤图标
		if (i < 2)
			DestroyPicture(&pBtnLaunchReceiveFiberIcon[i]);
	}

	DestroyLabel(&pFrmOtdrAnalysisSetLblPara);
	//注入光纤和接收光纤
	//背景
    DestroyPicture(&pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiberSelected);
    DestroyPicture(&pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber);
    DestroyPicture(&pFrmOtdrAnalysisSetBtnLengthSelected);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnEventSelected);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnLaunchFiberSelected);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnReceiveFiberSelected);

	DestroyLabel(&pFrmOtdrAnalysisSetLblLanuchAndReceiveFiber);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnFiberLength);
	DestroyLabel(&pFrmOtdrAnalysisSetLblFiberLength);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnFiberEvent);
	DestroyLabel(&pFrmOtdrAnalysisSetLblFiberEvent);

	DestroyPicture(&pFrmOtdrAnalysisSetBtnLaunchFiberLength);
	DestroyLabel(&pFrmOtdrAnalysisSetLblLanuchFiberLength);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnIconLaunchFiberLength);
	DestroyLabel(&pFrmOtdrAnalysisSetLblLanuchFiberLengthValue);

	DestroyPicture(&pFrmOtdrAnalysisSetBtnReceiveFiberLength);
	DestroyLabel(&pFrmOtdrAnalysisSetLblReceiveFiberLength);
	DestroyPicture(&pFrmOtdrAnalysisSetBtnIconReceiveFiberLength);
	DestroyLabel(&pFrmOtdrAnalysisSetLblReceiveFiberLengthValue);

	OtdrAnalysisSetTextRes_Exit(NULL, 0, NULL, 0);
	//清除参数
	OtdrAnalysisSetClearPara();
#ifdef EEPROM_DATA
	//退出并保存
	SetSettingsData((void*)&pUser_Settings->sFrontSetting, sizeof(FRONT_SETTING), FRONTSET_1310);
	SaveSettings(FRONTSET_1310);
	SetSettingsData((void*)&pUser_Settings->sFrontSetting, sizeof(FRONT_SETTING), FRONTSET_1550);
	SaveSettings(FRONTSET_1550);
	SetSettingsData((void*)&pUser_Settings->sFrontSetting, sizeof(FRONT_SETTING), FRONTSET_1625);
	SaveSettings(FRONTSET_1625);
    SetSettingsData((void *)&pUser_Settings->sAnalysisSetting, sizeof(ANALYSIS_SETTING), ANALYSIS_SET);
    SaveSettings(ANALYSIS_SET);
#endif
	return iRet;
}

/***
  * 功能：
		窗体frmotdranalysisset的绘制函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetPaint(void *pWndObj)
{
	//显示标题栏控件
	DisplayPicture(pFrmOtdrAnalysisSetBg);
	// DisplayPicture(pFrmOtdrAnalysisSetBgIcon);
	// DisplayPicture(pFrmOtdrAnalysisSetBtnBack);

	//显示标题栏信息
	// DisplayLabel(pFrmOtdrAnalysisSetLblCurrTitle);
	//显示菜单栏信息
	int i = 0;
	for (; i < THRESHOLDSET_TITLE_NUM; ++i)
	{
		if (i == OTDR_ANALYSIS_SET)
		{
			SetPictureBitmap(pStrOtdrSetButton_press[OTDR_ANALYSIS_SET], pFrmOtdrAnalysisSetBtnTitle[i]);
		}
		DisplayPicture(pFrmOtdrAnalysisSetBtnTitle[i]);
		DisplayLabel(pFrmOtdrAnalysisSetLblTitle[i]);
	}
	//显示功能区
	DisplayOtdrAnalysisSetInfo();

	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		if (i == iCurSelectOption)
		{
			if (iOldOption == -1)
				SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
								 pFrmOtdrAnalysisSetBtnLeftSelected[i]);
			else//选中默认选项
				SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
								 pFrmOtdrAnalysisSetBtnDefaultSelected[i]);
			SetPictureBitmap(BmpFileDirectory"bg_sys_keyboard_press.bmp",
				pFrmOtdrAnalysisSetBtnEnterIcon[i]);
			SetPictureBitmap(pAnalysisIconBmpf[i],
							 pFrmOtdrAnalysisSetBtnIcon[i]);
		}
		DisplayPicture(pFrmOtdrAnalysisSetBtnLeftSelected[i]);
		DisplayPicture(pFrmOtdrAnalysisSetBtnDefaultSelected[i]);
		DisplayPicture(pFrmOtdrAnalysisSetBtnEnterIcon[i]);
		DisplayPicture(pFrmOtdrAnalysisSetBtnIcon[i]);
		//设置默认选项的状态
		SetDefaultBtnStatus(i, 1);
		DisplayPicture(pFrmOtdrAnalysisSetBtnDefault[i]);
		DisplayLabel(pFrmOtdrAnalysisSetLblDefault[i]);
		DisplayLabel(pFrmOtdrAnalysisSetLblParaInfo[i]);
		DisplayLabel(pFrmOtdrAnalysisSetLblParaValue[i]);
	}
	//设置注入光纤和接收光纤
    int iEnable = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber;
    if (iCurSelectOption >= ANALYSIS_COUNT)
	{
        SetLanuchAndReceiveFiberBmpStatus(iCurSelectOption - ANALYSIS_COUNT, iEnable);
    }

	DisplayLabel(pFrmOtdrAnalysisSetLblPara);
    //注入光纤和接收光纤
    SetLanuchAndReceiveFiberLblStatus(iEnable);
    SetLanuchAndReceiveFiberValueBmpStatus(iEnable);

    DisplayPicture(pFrmOtdrAnalysisSetBtnLengthSelected);
	DisplayPicture(pFrmOtdrAnalysisSetBtnEventSelected);
	DisplayPicture(pFrmOtdrAnalysisSetBtnLaunchFiberSelected);
	DisplayPicture(pFrmOtdrAnalysisSetBtnReceiveFiberSelected);

    DisplayPicture(pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiberSelected);
    DisplayPicture(pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber);
    DisplayLabel(pFrmOtdrAnalysisSetLblLanuchAndReceiveFiber);
	DisplayPicture(pFrmOtdrAnalysisSetBtnFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblFiberLength);
	DisplayPicture(pFrmOtdrAnalysisSetBtnFiberEvent);
	DisplayLabel(pFrmOtdrAnalysisSetLblFiberEvent);

	// DisplayPicture(pFrmOtdrAnalysisSetBtnLaunchFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblLanuchFiberLength);
	DisplayPicture(pFrmOtdrAnalysisSetBtnIconLaunchFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblLanuchFiberLengthValue);

	// DisplayPicture(pFrmOtdrAnalysisSetBtnReceiveFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblReceiveFiberLength);
	DisplayPicture(pFrmOtdrAnalysisSetBtnIconReceiveFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblReceiveFiberLengthValue);

	//注入/接收光纤图标
	DisplayPicture(pBtnLaunchReceiveFiberIcon[0]);
	DisplayPicture(pBtnLaunchReceiveFiberIcon[1]);

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

/***
  * 功能：
		窗体frmotdranalysisset的循环函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetLoop(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

/***
  * 功能：
		窗体frmotdranalysisset的挂起函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetPause(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}
/***
  * 功能：
		窗体frmotdranalysisset的恢复函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetResume(void *pWndObj)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int OtdrAnalysisSetTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//标题栏
	pFrmOtdrAnalysisSetStrCurrTitle = GetCurrLanguageText(OTDR_SET_LBL_ANALYSIS);
	//菜单栏
	pFrmOtdrAnalysisSetStrTitle[TEST_SET] = GetCurrLanguageText(OTDR_SET_LBL_TEST);
	pFrmOtdrAnalysisSetStrTitle[OTDR_ANALYSIS_SET] = GetCurrLanguageText(OTDR_SET_LBL_ANALYSIS);
	pFrmOtdrAnalysisSetStrTitle[THRESHOLD_SET] = GetCurrLanguageText(OTDR_SET_LBL_THRESHOLD);
	pFrmOtdrAnalysisSetStrTitle[REPORT_SET] = GetCurrLanguageText(OTDR_SET_LBL_REPORT);
	//功能区
	pFrmOtdrAnalysisSetStrPara = GetCurrLanguageText(OTDR_SET_LBL_ANALYSIS_PARA);
	pFrmOtdrAnalysisSetStrParaInfo[REFRACTIVE] = GetCurrLanguageText(OTDR_SET_LBL_REFRACTIVE_INDEX);
	pFrmOtdrAnalysisSetStrParaInfo[BACKSCATTER] = GetCurrLanguageText(OTDR_SET_LBL_BACK_SCATTERING);
	pFrmOtdrAnalysisSetStrParaInfo[COEFFICIENT_LENGTH] = GetCurrLanguageText(OTDR_SET_LBL_HELIX_FACTOR);
	pFrmOtdrAnalysisSetStrParaValue[REFRACTIVE] = TransString("1.4");
	pFrmOtdrAnalysisSetStrParaValue[BACKSCATTER] = TransString("-68.33");
	pFrmOtdrAnalysisSetStrParaValue[COEFFICIENT_LENGTH] = TransString("0.00");
	pFrmOtdrAnalysisSetStrDefault[REFRACTIVE] = GetCurrLanguageText(OTDR_SET_LBL_DEFAULT);
	pFrmOtdrAnalysisSetStrDefault[BACKSCATTER] = GetCurrLanguageText(OTDR_SET_LBL_DEFAULT);
	pFrmOtdrAnalysisSetStrDefault[COEFFICIENT_LENGTH] = GetCurrLanguageText(OTDR_SET_LBL_DEFAULT);
	//注入光纤和接收光纤
	pFrmOtdrAnalysisSetStrLanuchAndReceiveFiber = GetCurrLanguageText(OTDR_SET_LBL_LANUCH_RECEIVE_FIBER);
	pFrmOtdrAnalysisSetStrFiberLength = GetCurrLanguageText(OTDR_SET_LBL_FIBER_LENGTH);
	pFrmOtdrAnalysisSetStrFiberEvent = GetCurrLanguageText(OTDR_SET_LBL_FIBER_EVENT);

	if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
	{
		pFrmOtdrAnalysisSetStrLanuchFiberLength = GetCurrLanguageText(OTDR_SET_LBL_LANUCH_FIBER_LENGTH);
		pFrmOtdrAnalysisSetStrReceiveFiberLength = GetCurrLanguageText(OTDR_SET_LBL_RECEIVE_FIBER_LENGTH);
		pFrmOtdrAnalysisSetStrLanuchFiberLengthValue = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR,
			pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen, pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_ft,
			pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_mi, 1);
		pFrmOtdrAnalysisSetStrReceiveFiberLengthValue = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR,
			pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen, pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_ft, 
			pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_mi, 1);
	}
	else
	{
		pFrmOtdrAnalysisSetStrLanuchFiberLength = GetCurrLanguageText(OTDR_SET_LBL_LANUCH_FIBER_EVENT);
		pFrmOtdrAnalysisSetStrReceiveFiberLength = GetCurrLanguageText(OTDR_SET_LBL_RECEIVE_FIBER_EVENT);
		pFrmOtdrAnalysisSetStrLanuchFiberLengthValue = Int2GuiString(pOtdrAnalysisSet->pOtdrFiberSet.iLaunchFiberEvent);
		pFrmOtdrAnalysisSetStrReceiveFiberLengthValue = Int2GuiString(pOtdrAnalysisSet->pOtdrFiberSet.iRecvFiberEvent);
	}

	return 0;
}

//释放文本资源
static int OtdrAnalysisSetTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//标题栏
	GuiMemFree(pFrmOtdrAnalysisSetStrCurrTitle);
	//菜单栏
	int i = 0;
	for (; i < THRESHOLDSET_TITLE_NUM; ++i)
	{
		GuiMemFree(pFrmOtdrAnalysisSetStrTitle[i]);
	}
	//功能区
	GuiMemFree(pFrmOtdrAnalysisSetStrPara);
	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		GuiMemFree(pFrmOtdrAnalysisSetStrParaInfo[i]);
		GuiMemFree(pFrmOtdrAnalysisSetStrParaValue[i]);
	}
	//默认值
	GuiMemFree(pFrmOtdrAnalysisSetStrDefault[REFRACTIVE]);
	GuiMemFree(pFrmOtdrAnalysisSetStrDefault[BACKSCATTER]);
	GuiMemFree(pFrmOtdrAnalysisSetStrDefault[COEFFICIENT_LENGTH]);
	//注入光纤和接收光纤
	GuiMemFree(pFrmOtdrAnalysisSetStrLanuchAndReceiveFiber);
	GuiMemFree(pFrmOtdrAnalysisSetStrFiberLength);
	GuiMemFree(pFrmOtdrAnalysisSetStrFiberEvent);
	GuiMemFree(pFrmOtdrAnalysisSetStrLanuchFiberLength);
	GuiMemFree(pFrmOtdrAnalysisSetStrLanuchFiberLengthValue);
	GuiMemFree(pFrmOtdrAnalysisSetStrReceiveFiberLength);
	GuiMemFree(pFrmOtdrAnalysisSetStrReceiveFiberLengthValue);

	return 0;
}

/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet内部按键响应函数声明
*******************************************************************************/
//返回按钮响应处理函数
static int OtdrAnalysisSetBtnBack_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

static int OtdrAnalysisSetBtnBack_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	Stack *ps = NULL;
	Item func;
	ps = GetCurrWndStack();
	WndPop(ps, &func, pFrmOtdrAnalysisSet);

	return iRet;
}

//菜单栏按键响应
static int OtdrAnalysisSetBtnTitle_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

static int OtdrAnalysisSetBtnTitle_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	int option = iOutLen;
	GUIWINDOW *pWnd = NULL;

	switch (option)
	{
	case TEST_SET:
	{
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			FrmCommonSetInit, FrmCommonSetExit,
			FrmCommonSetPaint, FrmCommonSetLoop,
			FrmCommonSetPause, FrmCommonSetResume,
			NULL);
		SendWndMsg_WindowExit(pFrmOtdrAnalysisSet);
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
		SendWndMsg_WindowExit(pFrmOtdrAnalysisSet);
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
		SendWndMsg_WindowExit(pFrmOtdrAnalysisSet);
		SendSysMsg_ThreadCreate(pWnd);
	}
	break;
	default:
		break;
	}
	return iRet;
}

//功能区按键响应
static int OtdrAnalysisSetBtnSelected_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

static int OtdrAnalysisSetBtnSelected_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

static int OtdrAnalysisSetBtnIcon_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

static int OtdrAnalysisSetBtnIcon_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;
	int option = iOutLen;

	switch (option)
	{
	case REFRACTIVE:
	{
		char buf[10] = { 0 };
		int iWaveLengthIndex = GetWaveLengthIndex();
		if (iWaveLengthIndex < OTDR_WAVE_LENGTH_NUM - 1)
		{
			sprintf(buf, "%.6f", pOtdrAnalysisSet->fRefrac[iWaveLengthIndex]);
		}
		else
		{
			sprintf(buf, "%.6f", pOtdrAnalysisSet->fRefrac[ENUM_WAVE_1310NM]);
		}

		IMEInit(buf, 10, 1, ReCreateOtdrAnalysisSetWindow, InputRefractiveBackFunc, NULL);
	}
	break;
	case BACKSCATTER:
	{
		char buf[10] = { 0 };
		int iWaveLengthIndex = GetWaveLengthIndex();
		if (iWaveLengthIndex < OTDR_WAVE_LENGTH_NUM - 1)
		{
			sprintf(buf, "%.2f", pOtdrAnalysisSet->fBackSc[iWaveLengthIndex]);
		}
		else
		{
			sprintf(buf, "%.2f", pOtdrAnalysisSet->fBackSc[ENUM_WAVE_1310NM]);
		}

		IMEInit(buf, 10, 1, ReCreateOtdrAnalysisSetWindow, InputBackscatterBackFunc, NULL);
	}
	break;
	case COEFFICIENT_LENGTH:
	{
		char buf[10] = { 0 };
		int iWaveLengthIndex = GetWaveLengthIndex();
		if (iWaveLengthIndex < OTDR_WAVE_LENGTH_NUM - 1)
		{
			sprintf(buf, "%.3f", pOtdrAnalysisSet->fExceLen[iWaveLengthIndex]);
		}
		else
		{
			sprintf(buf, "%.3f", pOtdrAnalysisSet->fExceLen[ENUM_WAVE_1310NM]);
		}

		IMEInit(buf, 10, 1, ReCreateOtdrAnalysisSetWindow, InputCoefficientBackFunc, NULL);
	}
	break;
	default:
		break;
	}
	return iRet;
}

//默认值响应处理
static int OtdrAnalysisSetBtnDefault_Down(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	return iRet;
}

static int OtdrAnalysisSetBtnDefault_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;
	int option = iOutLen;
	int waveLengthIndex = GetWaveLengthIndex();

	switch (option)
	{
	case REFRACTIVE:
		RefractiveDefaultValue(waveLengthIndex);
		break;
	case BACKSCATTER:
		BackscatterDefaultValue(waveLengthIndex);
		break;
	case COEFFICIENT_LENGTH:
		CoefficientDefaultValue(waveLengthIndex);
		break;
	default:
		break;
	}

	SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp",
					 pFrmOtdrAnalysisSetBtnLeftSelected[option]);
	DisplayPicture(pFrmOtdrAnalysisSetBtnLeftSelected[option]);
	SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
					 pFrmOtdrAnalysisSetBtnDefaultSelected[option]);
	DisplayPicture(pFrmOtdrAnalysisSetBtnDefaultSelected[option]);
	DisplayLabel(pFrmOtdrAnalysisSetLblParaInfo[option]);
	DisplayLabel(pFrmOtdrAnalysisSetLblParaValue[option]);
	DisplayPicture(pFrmOtdrAnalysisSetBtnEnterIcon[option]);
	DisplayPicture(pFrmOtdrAnalysisSetBtnIcon[option]);
	//设置默认值按钮的状态
	SetDefaultBtnStatus(option, 0);
	DisplayPicture(pFrmOtdrAnalysisSetBtnDefault[option]);
	DisplayLabel(pFrmOtdrAnalysisSetLblDefault[option]);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iRet;
}

//光纤长度响应处理
static int OtdrAnalysisSetBtnFiberLength_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;
    //判断是否使能
    if (pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber)
    {
        pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength = 0;
        SetPictureBitmap(pDefaultBmpf[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0], pFrmOtdrAnalysisSetBtnFiberLength);
        DisplayPicture(pFrmOtdrAnalysisSetBtnFiberLength);
        SetPictureBitmap(pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength != 0], pFrmOtdrAnalysisSetBtnFiberEvent);
        DisplayPicture(pFrmOtdrAnalysisSetBtnFiberEvent);
        //设置光纤长度使能
        if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
        {
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 0;
        }
        else
        {
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 1;
        }
    }
    else
    {
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 0;
    }

	FlushLanuchAndReceiveFiber();
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iRet;
}

//光纤事件响应处理
static int OtdrAnalysisSetBtnFiberEvent_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;
    //判断是否使能
    if (pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber)
    {
        pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength = 1;
        SetPictureBitmap(pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0], pFrmOtdrAnalysisSetBtnFiberLength);
        DisplayPicture(pFrmOtdrAnalysisSetBtnFiberLength);
        SetPictureBitmap(pDefaultBmpf[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength != 0], pFrmOtdrAnalysisSetBtnFiberEvent);
        DisplayPicture(pFrmOtdrAnalysisSetBtnFiberEvent);
        //设置光纤长度使能
        if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
        {
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 0;
        }
        else
        {
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 1;
        }
    }
    else
    {
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 0;
    }

	FlushLanuchAndReceiveFiber();
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iRet;
}

//注入光纤长度/注入光纤事件响应处理
static int OtdrAnalysisSetBtnLaunchFiberLength_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
	{
		pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber ? 0 : 1;
	}
	else
	{
		pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent ? 0 : 1;
	}

	FlushLanuchAndReceiveFiber();
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iRet;
}

//接收光纤长度/接收光纤事件响应处理
static int OtdrAnalysisSetBtnReceiveFiberLength_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;

	if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
	{
		pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber ? 0 : 1;
	}
	else
	{
		pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent ? 0 : 1;
	}

	FlushLanuchAndReceiveFiber();
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iRet;
}

//注入光纤长度数值/注入光纤事件响应处理
static int OtdrAnalysisSetBtnLanuchFiberLengthValue_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;
	char *buf = NULL;

	if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
	{
		//按光纤长度
		if (pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber)
		{
			buf = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_OTDR,
				pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen, pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_ft,
				pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_mi, 0);
			IMEInit(buf, 10, 1, ReCreateOtdrAnalysisSetWindow, InputLaunchLenCallBack, NULL);
			free(buf);
			buf = NULL;
		}
	}
	else
	{
		//按事件
		if (pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent)
		{
			buf = Int2String(pOtdrAnalysisSet->pOtdrFiberSet.iLaunchFiberEvent);
			IMEInit(buf, 10, 1, ReCreateOtdrAnalysisSetWindow, InputLaunchLenCallBack, NULL);
			free(buf);
			buf = NULL;
		}
	}

	return iRet;
}

//接收光纤长度数值/接收光纤事件响应处理
static int OtdrAnalysisSetBtnReceiveFiberLengthValue_Up(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志，返回值定义 
	int iRet = 0;
	char *buf = NULL;

	if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
	{
		//按光纤长度
		if (pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber)
		{
			buf = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_OTDR,
				pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen, pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_ft,
				pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_mi, 0);
			IMEInit(buf, 10, 1, ReCreateOtdrAnalysisSetWindow, InputRecvLenCallBack, NULL);
			free(buf);
			buf = NULL;
		}
	}
	else
	{
		//按事件
		if (pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent)
		{
			buf = Int2String(pOtdrAnalysisSet->pOtdrFiberSet.iRecvFiberEvent);
			IMEInit(buf, 10, 1, ReCreateOtdrAnalysisSetWindow, InputRecvLenCallBack, NULL);
			free(buf);
			buf = NULL;
		}
	}

	return iRet;
}

static int OtdrAnalysisSetBtnLanuchAndReceiveFiber_Up(void *pInArg, int iInLen,
                                                      void *pOutArg, int iOutLen)
{
    pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber 
            = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber ? 0 : 1;
    SetPictureBitmap(pDefaultBmpf[pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber], pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber);
    DisplayPicture(pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber);
    //
    if (pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber)
    {
        //设置光纤长度使能
        if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
        {
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 0;
        }
        else
        {
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 0;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 1;
            pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 1;
        }
    }
    else
    {
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = 0;
        pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = 0;
    }
        
    FlushLanuchAndReceiveFiber();
    RefreshScreen(__FILE__, __func__, __LINE__);

    return 0;
}

//点击按键响应函数
static int WndOtdrAnalysisSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

static int WndOtdrAnalysisSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	unsigned int uiValue;
	uiValue = (unsigned int)pInArg;

	switch (uiValue)
	{
	case KEYCODE_OTDR_LEFT:
		OtdrAnalysisSetBtnTitle_Up(NULL, 0, NULL, TEST_SET);
		break;
	case KEYCODE_OTDR_RIGHT:
		OtdrAnalysisSetBtnTitle_Up(NULL, 0, NULL, THRESHOLD_SET);
		break;
	case KEYCODE_UP:
		if (iCurSelectOption != 0)
		{
			iCurSelectOption--;
		}
		FlushAnalysisOptionInfo();
		break;
	case KEYCODE_DOWN:
		if (iCurSelectOption != (LAUNCH_RECEIVE_FIBER_OPTION + ANALYSIS_COUNT))
		{
			iCurSelectOption++;
		}
		FlushAnalysisOptionInfo();
		break;
	case KEYCODE_LEFT:
		iOldOption = -1;
		FlushAnalysisOptionInfo();
		break;
	case KEYCODE_RIGHT:
		iOldOption = iCurSelectOption;
		FlushAnalysisOptionInfo();
		break;
	case KEYCODE_ENTER:
		if (iOldOption >= 0)
		{
			if (iOldOption < ANALYSIS_COUNT && iCurSelectOption < ANALYSIS_COUNT)
			{
				OtdrAnalysisSetBtnDefault_Up(NULL, 0, NULL, iCurSelectOption);
			}
			else
			{
				//注入光纤/接收光纤数值设置
				HandleLanuchReceiveFiberEnterKey();
			}
		}
		else
		{
			if (iCurSelectOption < ANALYSIS_COUNT)
			{
				if (!iDefaultOption[iCurSelectOption]) //只有取消默认值情况下，才可更改左侧数据
				{
					OtdrAnalysisSetBtnIcon_Up(NULL, 0, NULL, iCurSelectOption);
				}
			}
			else
			{
				//注入光纤/接收光纤按钮设置
				HandleLanuchReceiveFiberEnterKey();
			}
		}
		break;
	case KEYCODE_START:
		//先退出当前界面
		OtdrAnalysisSetBtnBack_Up(NULL, 0, NULL, 0);
		//调用测试接口
		OtdrTest(0);
		break;
	case KEYCODE_ESC:
		OtdrAnalysisSetBtnBack_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}

	return 0;
}

/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet回调函数声明
*******************************************************************************/
/***
  * 功能：
		输入折射率后的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void InputRefractiveBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);
	double fTmp = String2Float(cTmpBuff);
	//1.00~2.00
	if (fTmp < String2Float("1.00") || fTmp > String2Float("2.00"))
	{
		CreateIMEDialog(GetCurrLanguageText(IME_LBL_REFRACTIVE_LIMIT));
		return;
	}
	//此处需要检查参数，暂时省略
	int iWaveLengthIndex = GetWaveLengthIndex();

	if (iWaveLengthIndex < OTDR_WAVE_LENGTH_NUM - 1)
	{
		pUser_Settings->sSampleSetting.sWaveLenArgs[iWaveLengthIndex].fRefractiveIndex = fTmp;
	}
	else
	{
		pUser_Settings->sSampleSetting.sWaveLenArgs[ENUM_WAVE_1310NM].fRefractiveIndex = fTmp;
		pUser_Settings->sSampleSetting.sWaveLenArgs[ENUM_WAVE_1550NM].fRefractiveIndex = fTmp;
	}
}

/***
  * 功能：
		输入背向散射系数后的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void InputBackscatterBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	double fTmp = String2Float(cTmpBuff);

	//此处需要检查参数，暂时省略
	int iwaveLengthIndex = GetWaveLengthIndex();
	//-89.44~ -65.45
	if (iwaveLengthIndex == ENUM_WAVE_1310NM)
	{
		if (fTmp < String2Float("-89.44") || fTmp > String2Float("-65.45"))
		{
			CreateIMEDialog(GetCurrLanguageText(IME_LBL_BACKLOSS_LIMIT_1310));
			return;
		}

		pUser_Settings->sSampleSetting.sWaveLenArgs[iwaveLengthIndex].fBackScattering = fTmp;
	}
	else if (iwaveLengthIndex == ENUM_WAVE_1550NM)
	{
		if (fTmp < String2Float("-91.86") || fTmp > String2Float("-71.87"))
		{
			CreateIMEDialog(GetCurrLanguageText(IME_LBL_BACKLOSS_LIMIT_1550));
			return;
		}

		pUser_Settings->sSampleSetting.sWaveLenArgs[iwaveLengthIndex].fBackScattering = fTmp;

	}
	else
	{
		pUser_Settings->sSampleSetting.sWaveLenArgs[ENUM_WAVE_1310NM].fBackScattering = fTmp;
		pUser_Settings->sSampleSetting.sWaveLenArgs[ENUM_WAVE_1310NM].fBackScattering = fTmp;
	}
}

/***
  * 功能：
		输入余长系数后的回调函数
  * 参数：
		无
  * 返回：
		无
  * 备注：
***/
static void InputCoefficientBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	double fTmp = String2Float(cTmpBuff);

	//此处需要检查参数，暂时省略
	//0 ~10
	if (fTmp < String2Float("0.0") || fTmp > String2Float("10.0"))
	{
		CreateIMEDialog(GetCurrLanguageText(IME_LBL_EXEC_LIMIT));
		return;
	}

	int iWaveLengthIndex = GetWaveLengthIndex();

	if (iWaveLengthIndex < OTDR_WAVE_LENGTH_NUM - 1)
	{
		pUser_Settings->sSampleSetting.sWaveLenArgs[iWaveLengthIndex].fExcessLength = fTmp;
	}
	else
	{
		pUser_Settings->sSampleSetting.sWaveLenArgs[ENUM_WAVE_1310NM].fExcessLength = fTmp;
		pUser_Settings->sSampleSetting.sWaveLenArgs[ENUM_WAVE_1550NM].fExcessLength = fTmp;
	}
}

//注入光纤长度/事件设置
static void InputLaunchLenCallBack(void)
{
	char inputBuffer[100];
	GetIMEInputBuff(inputBuffer);

	if (pUser_Settings->sAnalysisSetting.iEventOrLength == 0)
	{
		double out = pUser_Settings->sAnalysisSetting.fLaunchFiberLen;
		double out_ft = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft;
		double out_mi = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi;

		double launchLen = String2Float(inputBuffer);
		AnalyzeConvertLaunchRecvLen(launchLen, &out, &out_ft, &out_mi);
		pUser_Settings->sAnalysisSetting.fLaunchFiberLen = out;
		pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft = out_ft;
		pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi = out_mi;
	}
	else
	{
		int launchEvent = String2Int(inputBuffer);
		if (launchEvent < 1 || launchEvent > 10)
		{
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_EVENT_OUT_OF_RANGE));
			return;
		}
		else
		{
			pUser_Settings->sAnalysisSetting.iLaunchFiberEvent = launchEvent;
		}
	}
}

//接收光纤长度/事件设置
static void InputRecvLenCallBack(void)
{
	char inputBuffer[100];
	GetIMEInputBuff(inputBuffer);
	if (pUser_Settings->sAnalysisSetting.iEventOrLength == 0)
	{
		double out = pUser_Settings->sAnalysisSetting.fRecvFiberLen;
		double out_ft = pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft;
		double out_mi = pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi;

		double recvLen = String2Float(inputBuffer);
		AnalyzeConvertLaunchRecvLen(recvLen, &out, &out_ft, &out_mi);
		pUser_Settings->sAnalysisSetting.fRecvFiberLen = out;
		pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft = out_ft;
		pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi = out_mi;
	}
	else
	{
		int RecvEvent = String2Int(inputBuffer);
		if (RecvEvent < 1 || RecvEvent > 10)
		{
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_EVENT_OUT_OF_RANGE));
			return;
		}
		else
		{
			pUser_Settings->sAnalysisSetting.iRecvFiberEvent = RecvEvent;
		}
	}
}

//重绘窗体
static void ReCreateOtdrAnalysisSetWindow(GUIWINDOW **pWnd)
{
	*pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		FrmOtdrAnalysisSetInit, FrmOtdrAnalysisSetExit,
		FrmOtdrAnalysisSetPaint, FrmOtdrAnalysisSetLoop,
		FrmOtdrAnalysisSetPause, FrmOtdrAnalysisSetResume,
		NULL);
}

/*******************************************************************************
*                   窗体FrmOtdrAnalysisSet内部参数相关功能函数声明
*******************************************************************************/
//初始化参数
static void OtdrAnalysisSetInitPara(void)
{
	if (pOtdrAnalysisSet == NULL)
	{
		pOtdrAnalysisSet = (OTDR_ANALYSIS*)calloc(1, sizeof(OTDR_ANALYSIS));
		memset(pOtdrAnalysisSet, 0, sizeof(OTDR_ANALYSIS));
	}

	int i;
	for (i = 0; i < OTDR_WAVE_LENGTH_NUM; ++i)
	{
		pOtdrAnalysisSet->fRefrac[i] = pUser_Settings->sSampleSetting.sWaveLenArgs[i].fRefractiveIndex;
		pOtdrAnalysisSet->fBackSc[i] = pUser_Settings->sSampleSetting.sWaveLenArgs[i].fBackScattering;
		pOtdrAnalysisSet->fExceLen[i] = pUser_Settings->sSampleSetting.sWaveLenArgs[i].fExcessLength;
	}

	//注入光纤和接收光纤
	pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber = pUser_Settings->sAnalysisSetting.iEnableLaunchFiber ? 1 : 0;
	pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen = pUser_Settings->sAnalysisSetting.fLaunchFiberLen;
	pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber = pUser_Settings->sAnalysisSetting.iEnableRecvFiber ? 1 : 0;
	pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen = pUser_Settings->sAnalysisSetting.fRecvFiberLen;
	pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_ft = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft;
	pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_mi = pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi;
	pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_ft = pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft;
	pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_mi = pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi;
	pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent = pUser_Settings->sAnalysisSetting.iEnableLaunchFiberEvent;
	pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent = pUser_Settings->sAnalysisSetting.iEnableRecvFiberEvent;
	pOtdrAnalysisSet->pOtdrFiberSet.iLaunchFiberEvent = pUser_Settings->sAnalysisSetting.iLaunchFiberEvent;
	pOtdrAnalysisSet->pOtdrFiberSet.iRecvFiberEvent = pUser_Settings->sAnalysisSetting.iRecvFiberEvent;
	pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength = pUser_Settings->sAnalysisSetting.iEventOrLength;
    pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber = pUser_Settings->sAnalysisSetting.iEnableLaunchAndReceiveFiber;
}

//清除参数(保存参数)
static void OtdrAnalysisSetClearPara(void)
{
	int i;
	for (i = 0; i < OTDR_WAVE_LENGTH_NUM; ++i)
	{
		pUser_Settings->sSampleSetting.sWaveLenArgs[i].fRefractiveIndex = pOtdrAnalysisSet->fRefrac[i];
		pUser_Settings->sSampleSetting.sWaveLenArgs[i].fBackScattering = pOtdrAnalysisSet->fBackSc[i];
		pUser_Settings->sSampleSetting.sWaveLenArgs[i].fExcessLength = pOtdrAnalysisSet->fExceLen[i];
	}

	//注入光纤和接收光纤
	pUser_Settings->sAnalysisSetting.iEnableLaunchFiber = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber;
	pUser_Settings->sAnalysisSetting.fLaunchFiberLen = pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen;
	pUser_Settings->sAnalysisSetting.iEnableRecvFiber = pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber;
	pUser_Settings->sAnalysisSetting.fRecvFiberLen = pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen;

	pUser_Settings->sAnalysisSetting.fLaunchFiberLen_ft = pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_ft;
	pUser_Settings->sAnalysisSetting.fLaunchFiberLen_mi = pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_mi;
	pUser_Settings->sAnalysisSetting.fRecvFiberLen_ft = pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_ft;
	pUser_Settings->sAnalysisSetting.fRecvFiberLen_mi = pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_mi;

	pUser_Settings->sAnalysisSetting.iEnableLaunchFiberEvent = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent;
	pUser_Settings->sAnalysisSetting.iEnableRecvFiberEvent = pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent;
	pUser_Settings->sAnalysisSetting.iLaunchFiberEvent = pOtdrAnalysisSet->pOtdrFiberSet.iLaunchFiberEvent;
	pUser_Settings->sAnalysisSetting.iRecvFiberEvent = pOtdrAnalysisSet->pOtdrFiberSet.iRecvFiberEvent;
	pUser_Settings->sAnalysisSetting.iEventOrLength = pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength;
    pUser_Settings->sAnalysisSetting.iEnableLaunchAndReceiveFiber = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber;
}

//获取波长index
static int GetWaveLengthIndex()
{
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;
	int i;
	int index = 0;
	int count = 0;
	for (i = 0; i < OTDR_WAVE_LENGTH_NUM; ++i)
	{
		if (pUser_Setting->iWave[i] == 1)
		{
			count++;
			index = i;
		}
	}

	if (count > 1)
	{
		return 2;
	}
	else
	{
		return index;
	}
}

//显示otdr分析设置后的信息
static void DisplayOtdrAnalysisSetInfo(void)
{
	char tempRefValue[10] = { 0 };        //反射率
	char tempBackValue[10] = { 0 };       //背向散射系数
	char tempExecValue[10] = { 0 };       //余长系数

	int iWaveLengthIndex = GetWaveLengthIndex();

	if (iWaveLengthIndex < OTDR_WAVE_LENGTH_NUM - 1)//目前只支持1310+1550nm
	{
		sprintf(tempRefValue, "%.6f", pOtdrAnalysisSet->fRefrac[iWaveLengthIndex]);
		sprintf(tempBackValue, "%.2f", pOtdrAnalysisSet->fBackSc[iWaveLengthIndex]);
		sprintf(tempExecValue, "%.3f", pOtdrAnalysisSet->fExceLen[iWaveLengthIndex]);
	}
	else//双波长时显示1310nm
	{
		sprintf(tempRefValue, "%.6f", pOtdrAnalysisSet->fRefrac[iWaveLengthIndex]);
		sprintf(tempBackValue, "%.2f", pOtdrAnalysisSet->fBackSc[iWaveLengthIndex]);
		sprintf(tempExecValue, "%.3f", pOtdrAnalysisSet->fExceLen[iWaveLengthIndex]);
	}

	pFrmOtdrAnalysisSetStrParaValue[REFRACTIVE] = TransString(tempRefValue);//反射率
	pFrmOtdrAnalysisSetStrParaValue[BACKSCATTER] = TransString(tempBackValue);
	pFrmOtdrAnalysisSetStrParaValue[COEFFICIENT_LENGTH] = TransString(tempExecValue);

	SetLabelText(pFrmOtdrAnalysisSetStrParaValue[REFRACTIVE], pFrmOtdrAnalysisSetLblParaValue[REFRACTIVE]);
	SetLabelText(pFrmOtdrAnalysisSetStrParaValue[BACKSCATTER], pFrmOtdrAnalysisSetLblParaValue[BACKSCATTER]);
	SetLabelText(pFrmOtdrAnalysisSetStrParaValue[COEFFICIENT_LENGTH], pFrmOtdrAnalysisSetLblParaValue[COEFFICIENT_LENGTH]);
}

//设置默认值选项的状态
static void SetDefaultBtnStatus(int option, int iRefresh)
{
	if (option >= 0)
	{
		if (!iRefresh)
		{
			//不需要重新绘制窗体
			iDefaultOption[option] = (iDefaultOption[option] == 0) ? 1 : 0;
		}

		if (iCurSelectOption == option && (iOldOption != -1))
			SetPictureBitmap(pDefaultBmpf[iDefaultOption[option]], pFrmOtdrAnalysisSetBtnDefault[option]);
		else
			SetPictureBitmap(pDefaultBmp[iDefaultOption[option]], pFrmOtdrAnalysisSetBtnDefault[option]);
	}
}

//刷新分析设置选项内容
static void FlushAnalysisOptionInfo(void)
{
	//统一设置所有选项为非选中状态
	int i;
	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pFrmOtdrAnalysisSetBtnLeftSelected[i]);
		SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp", pFrmOtdrAnalysisSetBtnDefaultSelected[i]);
		SetPictureBitmap(BmpFileDirectory"bg_sys_keyboard.bmp", pFrmOtdrAnalysisSetBtnEnterIcon[i]);
		SetPictureBitmap(pAnalysisIconBmp[i], pFrmOtdrAnalysisSetBtnIcon[i]);
		SetDefaultBtnStatus(i, 1);
	}
	//设置注入光纤和接收光纤非选中状态
    SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp", pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiberSelected);
    SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pFrmOtdrAnalysisSetBtnLengthSelected);
	SetPictureBitmap(BmpFileDirectory "bg_sys_unselect.bmp", pFrmOtdrAnalysisSetBtnEventSelected);
	SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pFrmOtdrAnalysisSetBtnLaunchFiberSelected);
	SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pFrmOtdrAnalysisSetBtnReceiveFiberSelected);
    //设置使能图标
    int iEnable = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber;
    SetLanuchAndReceiveFiberValueBmpStatus(iEnable);
	
	//设置按长度/按事件
    SetPictureBitmap(pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber],
                     pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber);
    SetPictureBitmap(pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0],
					 pFrmOtdrAnalysisSetBtnFiberLength);
	SetPictureBitmap(pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength != 0],
					 pFrmOtdrAnalysisSetBtnFiberEvent);
	//图标
	SetPictureBitmap(pFiberIconBmp[0], pBtnLaunchReceiveFiberIcon[0]);
	SetPictureBitmap(pFiberIconBmp[1], pBtnLaunchReceiveFiberIcon[1]);
	//设置分析参数选中状态
	if (iCurSelectOption < ANALYSIS_COUNT)
	{
		if (iOldOption == -1)
		{
			SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp", pFrmOtdrAnalysisSetBtnLeftSelected[iCurSelectOption]);
			SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp", pFrmOtdrAnalysisSetBtnEnterIcon[iCurSelectOption]);
			SetPictureBitmap(pAnalysisIconBmpf[iCurSelectOption], pFrmOtdrAnalysisSetBtnIcon[iCurSelectOption]);
		}
		else
			SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp", pFrmOtdrAnalysisSetBtnDefaultSelected[iCurSelectOption]);
	}
	else
	{
        SetLanuchAndReceiveFiberBmpStatus(iCurSelectOption - ANALYSIS_COUNT, iEnable);
    }
	//刷新分析参数
	for (i = 0; i < ANALYSIS_COUNT; ++i)
	{
		DisplayPicture(pFrmOtdrAnalysisSetBtnLeftSelected[i]);
		DisplayPicture(pFrmOtdrAnalysisSetBtnDefaultSelected[i]);
		DisplayLabel(pFrmOtdrAnalysisSetLblParaInfo[i]);
		DisplayLabel(pFrmOtdrAnalysisSetLblParaValue[i]);
		DisplayPicture(pFrmOtdrAnalysisSetBtnEnterIcon[i]);
		DisplayPicture(pFrmOtdrAnalysisSetBtnIcon[i]);
		DisplayPicture(pFrmOtdrAnalysisSetBtnDefault[i]);
		DisplayLabel(pFrmOtdrAnalysisSetLblDefault[i]);
	}
	//刷新注入光纤和接收光纤
    DisplayPicture(pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiberSelected);
    DisplayPicture(pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber);
    DisplayLabel(pFrmOtdrAnalysisSetLblLanuchAndReceiveFiber);
    DisplayPicture(pFrmOtdrAnalysisSetBtnLengthSelected);
	DisplayPicture(pFrmOtdrAnalysisSetBtnEventSelected);
	DisplayPicture(pFrmOtdrAnalysisSetBtnFiberLength);
	DisplayPicture(pFrmOtdrAnalysisSetBtnFiberEvent);
	DisplayLabel(pFrmOtdrAnalysisSetLblFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblFiberEvent);
	FlushLanuchAndReceiveFiber();

	RefreshScreen(__FILE__, __func__, __LINE__);
}

//刷新注入光纤和接收光纤
static void FlushLanuchAndReceiveFiber(void)
{
	//刷新背景
	DisplayPicture(pFrmOtdrAnalysisSetBtnLaunchFiberSelected);
	DisplayPicture(pFrmOtdrAnalysisSetBtnReceiveFiberSelected);

	int temp = pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0 ? pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiber
		: pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchFiberEvent;
	SetPictureBitmap(pDefaultBmp[temp], pFrmOtdrAnalysisSetBtnLaunchFiberLength);
	// DisplayPicture(pFrmOtdrAnalysisSetBtnLaunchFiberLength);

	temp = pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0 ? pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiber
		: pOtdrAnalysisSet->pOtdrFiberSet.iEnableRecvFiberEvent;
	SetPictureBitmap(pDefaultBmp[temp], pFrmOtdrAnalysisSetBtnReceiveFiberLength);
	// DisplayPicture(pFrmOtdrAnalysisSetBtnReceiveFiberLength);

	GuiMemFree(pFrmOtdrAnalysisSetStrLanuchFiberLength);
	GuiMemFree(pFrmOtdrAnalysisSetStrReceiveFiberLength);
	GuiMemFree(pFrmOtdrAnalysisSetStrLanuchFiberLengthValue);
	GuiMemFree(pFrmOtdrAnalysisSetStrReceiveFiberLengthValue);

	if (pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0)
	{
		pFrmOtdrAnalysisSetStrLanuchFiberLength = GetCurrLanguageText(OTDR_SET_LBL_LANUCH_FIBER_LENGTH);
		pFrmOtdrAnalysisSetStrReceiveFiberLength = GetCurrLanguageText(OTDR_SET_LBL_RECEIVE_FIBER_LENGTH);

		pFrmOtdrAnalysisSetStrLanuchFiberLengthValue = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR,
			pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen, pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_ft,
			pOtdrAnalysisSet->pOtdrFiberSet.fLaunchFiberLen_mi, 1);
		pFrmOtdrAnalysisSetStrReceiveFiberLengthValue = UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT_OTDR,
			pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen, pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_ft,
			pOtdrAnalysisSet->pOtdrFiberSet.fRecvFiberLen_mi, 1);
	}
	else
	{
		pFrmOtdrAnalysisSetStrLanuchFiberLength = GetCurrLanguageText(OTDR_SET_LBL_LANUCH_FIBER_EVENT);
		pFrmOtdrAnalysisSetStrReceiveFiberLength = GetCurrLanguageText(OTDR_SET_LBL_RECEIVE_FIBER_EVENT);
		pFrmOtdrAnalysisSetStrLanuchFiberLengthValue = Int2GuiString(pOtdrAnalysisSet->pOtdrFiberSet.iLaunchFiberEvent);
		pFrmOtdrAnalysisSetStrReceiveFiberLengthValue = Int2GuiString(pOtdrAnalysisSet->pOtdrFiberSet.iRecvFiberEvent);
	}

	SetLabelText(pFrmOtdrAnalysisSetStrLanuchFiberLength, pFrmOtdrAnalysisSetLblLanuchFiberLength);
	SetLabelText(pFrmOtdrAnalysisSetStrReceiveFiberLength, pFrmOtdrAnalysisSetLblReceiveFiberLength);
	SetLabelText(pFrmOtdrAnalysisSetStrLanuchFiberLengthValue, pFrmOtdrAnalysisSetLblLanuchFiberLengthValue);
	SetLabelText(pFrmOtdrAnalysisSetStrReceiveFiberLengthValue, pFrmOtdrAnalysisSetLblReceiveFiberLengthValue);
    //判断是否使能
    int iEnable = pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber;
    SetLanuchAndReceiveFiberLblStatus(iEnable);
    SetLanuchAndReceiveFiberValueBmpStatus(iEnable);

    DisplayLabel(pFrmOtdrAnalysisSetLblFiberLength);
    DisplayLabel(pFrmOtdrAnalysisSetLblFiberEvent);
    DisplayLabel(pFrmOtdrAnalysisSetLblLanuchFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblReceiveFiberLength);
	DisplayPicture(pFrmOtdrAnalysisSetBtnIconLaunchFiberLength);
	DisplayPicture(pFrmOtdrAnalysisSetBtnIconReceiveFiberLength);
	DisplayLabel(pFrmOtdrAnalysisSetLblLanuchFiberLengthValue);
	DisplayLabel(pFrmOtdrAnalysisSetLblReceiveFiberLengthValue);
	//注入/接收光纤图标
	DisplayPicture(pBtnLaunchReceiveFiberIcon[0]);
	DisplayPicture(pBtnLaunchReceiveFiberIcon[1]);
}

/***
  * 功能：
		折射率的默认值响应函数
  * 参数：
		int iWaveLengthIndex:当前选中的波长index
  * 返回：
		无
  * 备注：
***/
static void RefractiveDefaultValue(int iWaveLengthIndex)
{
	switch (iWaveLengthIndex)
	{
	case ENUM_WAVE_1310NM://选中1310波长
		pOtdrAnalysisSet->fRefrac[ENUM_WAVE_1310NM] = 1.467700f;
		pFrmOtdrAnalysisSetStrParaValue[REFRACTIVE] = Float2GuiString(IOR_PRECISION, 1.467700f);
		break;
	case ENUM_WAVE_1550NM://选中1550波长
		pOtdrAnalysisSet->fRefrac[ENUM_WAVE_1550NM] = 1.468325f;
		pFrmOtdrAnalysisSetStrParaValue[REFRACTIVE] = Float2GuiString(IOR_PRECISION, 1.468325f);
		break;
	case ENUM_WAVE_1625NM://选中1625波长(此处选择的是1310+1550nm)
		pOtdrAnalysisSet->fRefrac[ENUM_WAVE_1310NM] = 1.467700f;
		pOtdrAnalysisSet->fRefrac[ENUM_WAVE_1550NM] = 1.468325f;
		pOtdrAnalysisSet->fRefrac[ENUM_WAVE_1625NM] = 1.460000f;
		pFrmOtdrAnalysisSetStrParaValue[REFRACTIVE] = Float2GuiString(IOR_PRECISION, 1.467700f); //TransString("1.467700f");
		break;
	default:
		break;
	}

	SetLabelText(pFrmOtdrAnalysisSetStrParaValue[REFRACTIVE],
		pFrmOtdrAnalysisSetLblParaValue[REFRACTIVE]);
}

/***
  * 功能：
		背向散射默认设置函数
  * 参数：
		int iWaveLengthIndex:当前选中的波长index
  * 返回：
		无
  * 备注：
***/
static void BackscatterDefaultValue(int iWaveLengthIndex)
{
	switch (iWaveLengthIndex)
	{
	case ENUM_WAVE_1310NM://选中1310波长
		pOtdrAnalysisSet->fBackSc[ENUM_WAVE_1310NM] = -79.44f;
		pFrmOtdrAnalysisSetStrParaValue[BACKSCATTER] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, -79.44f);
		break;
	case ENUM_WAVE_1550NM://选中1550波长
		pOtdrAnalysisSet->fBackSc[ENUM_WAVE_1550NM] = -81.87f;
		pFrmOtdrAnalysisSetStrParaValue[BACKSCATTER] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, -81.87f);
		break;
	case ENUM_WAVE_1625NM://选中1625波长(此处选择的是1310+1550nm)
		pOtdrAnalysisSet->fBackSc[ENUM_WAVE_1310NM] = -79.44f;
		pOtdrAnalysisSet->fBackSc[ENUM_WAVE_1550NM] = -81.87f;
		pOtdrAnalysisSet->fBackSc[ENUM_WAVE_1625NM] = -82.58f;
		pFrmOtdrAnalysisSetStrParaValue[BACKSCATTER] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, -79.44f);
		break;
	default:
		break;
	}

	SetLabelText(pFrmOtdrAnalysisSetStrParaValue[BACKSCATTER],
		pFrmOtdrAnalysisSetLblParaValue[BACKSCATTER]);
}

/***
  * 功能：
		余长系数默认设置函数
  * 参数：
		int iWaveLengthIndex:当前选中的波长index
  * 返回：
		无
  * 备注：
***/
static void CoefficientDefaultValue(int iWaveLengthIndex)
{
	pOtdrAnalysisSet->fExceLen[iWaveLengthIndex] = 0.0f;
	pFrmOtdrAnalysisSetStrParaValue[COEFFICIENT_LENGTH] = Float2GuiString(HELIX_FACTOR_PRECISION, 0.0f);
	SetLabelText(pFrmOtdrAnalysisSetStrParaValue[COEFFICIENT_LENGTH],
		pFrmOtdrAnalysisSetLblParaValue[COEFFICIENT_LENGTH]);
}

//检查值是否合法并进行单位转换
static void AnalyzeConvertLaunchRecvLen(double in, double* out, double* out_ft, double* out_mi)
{
	char* threshold[UNIT_COUNT] = { "360000.0000", "360.0", "1181102.4",
										"1181.1024", "223.6936", "393700.8" };

	double min = 0.0f;
	double max = String2Float(threshold[GetCurrSystemUnit(MODULE_UNIT_OTDR)]);

	if (in < min || in > max)
	{
		switch (GetCurrSystemUnit(MODULE_UNIT_OTDR))
		{
		case UNIT_M:
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_LENGTH_M_RANGE));
			return;
		case UNIT_KM:
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_LENGTH_KM_RANGE));
			return;
		case UNIT_FT:
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_LENGTH_FT_RANGE));
			return;
		case UNIT_KFT:
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_LENGTH_KFT_RANGE));
			return;
		case UNIT_YD:
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_LENGTH_YD_RANGE));
			return;
		case UNIT_MI:
			CreateIMEDialog(GetCurrLanguageText(OTDR_SET_LBL_FIBER_LENGTH_MI_RANGE));
			return;
		default:
			break;
		}
	}

	*out = UnitConverter_Dist_System2M_Float2Float(MODULE_UNIT_OTDR, in);
	*out_ft = UnitConverter_Dist_System2FT_Float2Float(MODULE_UNIT_OTDR, in);
	*out_mi = UnitConverter_Dist_System2MI_Float2Float(MODULE_UNIT_OTDR, in);
}

//处理注入/接收光纤(iLeftRight:0,left,1:right)
static void HandleLanuchReceiveFiberEnterKey(void)
{
	switch (iCurSelectOption - ANALYSIS_COUNT)
	{
    case 0: //
        OtdrAnalysisSetBtnLanuchAndReceiveFiber_Up(NULL, 0, NULL, 0);
        break;
    case 1://按长度
		OtdrAnalysisSetBtnFiberLength_Up(NULL, 0, NULL, 0);
		break;
	case 2: //按事件
		OtdrAnalysisSetBtnFiberEvent_Up(NULL, 0, NULL, 0);
		break;
	case 3: //注入光纤
		OtdrAnalysisSetBtnLanuchFiberLengthValue_Up(NULL, 0, NULL, 0);
		break;
	case 4: //接收光纤
		OtdrAnalysisSetBtnReceiveFiberLengthValue_Up(NULL, 0, NULL, 0);
		break;
	default:
		break;
	}
}

//设置注入/接收光纤图标状态
static void SetLanuchAndReceiveFiberLblStatus(int iStatus)
{
    //判断是否使能
    if (iStatus)
    {
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblFiberLength);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblFiberEvent);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblLanuchFiberLength);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblReceiveFiberLength);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblLanuchFiberLengthValue);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOtdrAnalysisSetLblReceiveFiberLengthValue);
    }
    else
    {
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFrmOtdrAnalysisSetLblFiberLength);
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFrmOtdrAnalysisSetLblFiberEvent);
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFrmOtdrAnalysisSetLblLanuchFiberLength);
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFrmOtdrAnalysisSetLblReceiveFiberLength);
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFrmOtdrAnalysisSetLblLanuchFiberLengthValue);
        SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pFrmOtdrAnalysisSetLblReceiveFiberLengthValue);
    }
}

//设置注入/接收光纤图标状态
static void SetLanuchAndReceiveFiberBmpStatus(int iIndex, int iStatus)
{
    switch (iIndex)
    {
    case 0: //选中注入光纤/接收光纤
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
                         pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiberSelected);
        SetPictureBitmap(pDefaultBmpf[pOtdrAnalysisSet->pOtdrFiberSet.iEnableLaunchAndReceiveFiber],
                         pFrmOtdrAnalysisSetBtnLanuchAndReceiveFiber);
        break;
    case 1: //按光纤长度
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
                         pFrmOtdrAnalysisSetBtnLengthSelected);
        SetPictureBitmap(pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength == 0],
                         pFrmOtdrAnalysisSetBtnFiberLength);

        break;
    case 2: //按事件
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
                         pFrmOtdrAnalysisSetBtnEventSelected);
        SetPictureBitmap(pDefaultBmp[pOtdrAnalysisSet->pOtdrFiberSet.iEventOrLength != 0],
                         pFrmOtdrAnalysisSetBtnFiberEvent);
        break;
    case 3: //注入光纤
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
                         pFrmOtdrAnalysisSetBtnLaunchFiberSelected);
        iStatus ? SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp",
                                   pFrmOtdrAnalysisSetBtnIconLaunchFiberLength)
                : SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                   pFrmOtdrAnalysisSetBtnIconLaunchFiberLength);
        SetPictureBitmap(pFiberIconBmpf[0],
                         pBtnLaunchReceiveFiberIcon[0]);
        break;
    case 4: //接收光纤
        SetPictureBitmap(BmpFileDirectory "bg_sys_select.bmp",
                         pFrmOtdrAnalysisSetBtnReceiveFiberSelected);
        iStatus ? SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_press.bmp",
                                   pFrmOtdrAnalysisSetBtnIconReceiveFiberLength)
                : SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_press.bmp",
                                   pFrmOtdrAnalysisSetBtnIconReceiveFiberLength);
        SetPictureBitmap(pFiberIconBmpf[1],
                         pBtnLaunchReceiveFiberIcon[1]);
        break;
    default:
        break;
    }
}

//设置注入/接收光纤数值图标状态
static void SetLanuchAndReceiveFiberValueBmpStatus(int iStatus)
{
    if (iStatus)
    {
        SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp", pFrmOtdrAnalysisSetBtnIconLaunchFiberLength);
        SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard.bmp", pFrmOtdrAnalysisSetBtnIconReceiveFiberLength);
    }
    else
    {
        SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp", pFrmOtdrAnalysisSetBtnIconLaunchFiberLength);
        SetPictureBitmap(BmpFileDirectory "bg_sys_keyboard_disable_unpress.bmp", pFrmOtdrAnalysisSetBtnIconReceiveFiberLength);
    }
}