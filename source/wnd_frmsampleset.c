/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsampleset.c
* 摘    要：  实现主窗体frmsampleset的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014.10.21
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_frmsampleset.h"

/*************************************
* 定义wnd_frmsampleset.c引用其他头文件
*************************************/
#include "wnd_frmanalysset.h"
#include "wnd_frmcommonset.h"
#include "guipicture.h"
#include "wnd_global.h"
#include "app_global.h"
#include "app_frminit.h"
#include "wnd_frmselector.h"
#include "wnd_frmime.h"
#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"
#include "wnd_stack.h"
#include "app_getsetparameter.h"
#include "guiphoto.h"
#include "wnd_frmotdreventthresholdset.h"
#include "app_systemsettings.h"
#include "app_unitconverter.h"

/*****************************************
* 定义wnd_frmsampleset.c内部使用的数据结构
*****************************************/
struct OtdrSampleSet
{
	int				iHighSamp;		//高采样率		
	SAMPLE_INTERVAL	enSamVal;			//采样间隔	
	AVERAGE_TIME	enAvg;				//平均时间	
	float			fRefrac[WAVE_NUM];	//反射率	
	float			fBackSc[WAVE_NUM];	//回波损耗
	float			fExceLen[WAVE_NUM];	//余长系数
};

static int iSelectedWave = 0;	//选中的波长
static struct OtdrSampleSet *pSamSet = NULL;//自定义设置结构体
static int iUnitConverterFlag = 0;  //单位换算标志位

#define MAX_OTDR_UNIT_NUM       5

/***********************************
* wnd_frmsampleset.c窗体中的窗体资源
***********************************/
static GUIWINDOW *pFrmSampleSet = NULL;

/***********************************
* wnd_frmsampleset.c窗体中的文本资源
***********************************/
static GUICHAR *pSampleSetStrTitle = NULL;  
static GUICHAR *pSampleSetStrTitleOTDRSettings = NULL;    
static GUICHAR *pSampleSetStrPara = NULL;
static GUICHAR *pSampleSetStrSmptime = NULL;
static GUICHAR *pSampleSetStrInterval = NULL;
static GUICHAR *pSampleSetStrFiberSet = NULL;
static GUICHAR *pSampleSetStrFiberL[3] = {NULL};
static GUICHAR *pSampleSetStrFiberR[3]= {NULL};
static GUICHAR *pSampleSetStrDefValue = NULL;

/***********************************
* wnd_frmsampleset.c窗体中的控件资源
***********************************/
static GUIPICTURE *pSampleSetBg = NULL;
static GUIPICTURE *pSampleSetBgTableTitle = NULL;
static GUIPICTURE *pSampleSetBtnDefValue[3] = {NULL};
static GUIPICTURE *pSampleSetInputWavePara[3] = {NULL};

//单位换算
static GUICHAR *pSampleSetStrUnitConverter = NULL;  //单位切换
static GUILABEL *pSampleSetLblUnitConverter = NULL;
static GUIPICTURE *pSampleSetBtnUnit[MAX_OTDR_UNIT_NUM] = {NULL};       
static GUICHAR *pSampleSetStrUnit[MAX_OTDR_UNIT_NUM] = {NULL};  
static GUILABEL *pSampleSetLblUnit[MAX_OTDR_UNIT_NUM] = {NULL};

/***********************************
* wnd_frmsampleset.c窗体中的标签资源
***********************************/
static GUILABEL *pSampleSetLblTitle = NULL;    
static GUILABEL *pSampleSetLblTitleOTDRSettings = NULL;
static GUILABEL *pSampleSetLblPara = NULL;
static GUILABEL *pSampleSetLblSmptime = NULL;
static GUILABEL *pSampleSetLblInterval = NULL;
static GUILABEL *pSampleSetLblFiberSet = NULL;
static GUILABEL *pSampleSetLblFiberL[3] = {NULL};
static GUILABEL *pSampleSetLblFiberR[3]= {NULL};
static GUILABEL *pSampleSetLblDefValue[3] = {NULL};

/***********************************
* wnd_frmsampleset.c窗体中的其他资源
***********************************/
static GUIPICTURE *pSampleAvgTime[7] = {NULL};
static GUIPICTURE *pSampleSamplerate[2] = {NULL};
static GUIPICTURE *pSampleFiberset[2] = {NULL};
static char *pAvgtime[7] = {
    "5s",
    "15s",
    "30s",
    "60s",
    "90s",
    "120s",
    "180s"
};
static unsigned int pSamplerate[2] = {
    1
};
static char *pFiberset[2] = {
    "1310 nm",
    "1550 nm"
};
static GUICHAR *pStrAvgtime[7] = {NULL};
static GUICHAR *pStrSamplerate[2] = {NULL};
static GUICHAR *pStrFiberset[2] = {NULL};

static GUILABEL *pLblAvgtime[7] = {NULL};
static GUILABEL *pLblSamplerate[2] = {NULL};
static GUILABEL *pLblFiberset[2] = {NULL};
static WNDMENU1  *pSampleSetMenu = NULL;

/*********************************
*声明wnd_frmcommonset.c所引用的外部变量
*********************************/
/***********************************
* wnd_frmsampleset.c窗体中的内部函数
***********************************/
//初始化系统参数 
static int SampleInitPara(void);
//清空相关标志以及参数 
static int SampleClearPara(void);
//刷新不同波形相关信息显示的函数 
static void SampleFlushDisplay(int iIndex, int iWaveIndex);
//平均测量时间选择器回调函数 
static void AvgTimeBackFunc(int iSelected);
//波长选择器回调函数 
static void WaveLenBackFunc(int iSelected);
//采样分辨率选择器回调函数 
static void IntervalBackFunc(int iSelected);
//输入反射率键盘控件的回调函数 
static void InputRefrBackFunc(void);
//输入回波损耗键盘控件的回调函数 
static void InputBackBackFunc(void);
//输入余长系数耗键盘控件的回调函数 
static void InputExceBackFunc(void);
//反射率默认设置函数 
static void WaveRefrDefaultValue(void);
//回波损耗默认设置函数 
static void WaveBackDefaultValue(void);
//余长系数默认设置函数 
static void WaveExceDefaultValue(void);
//菜单控件回调函数
static void SampleMenuCallBack(int iOption);

//文本的初始化以及销毁函数
static int FrmSampleSetRes_Init(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int FrmSampleSetRes_Exit(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
//单位换算处理函数
static int SampleSetUnit_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);
static int SampleSetUnit_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);

//桌面上的按钮处理 
static int SampleSetInputWave_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);
static int SampleSetInputWave_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int SampleSetBtnDef_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int SampleSetBtnDef_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);

static int SampleSetAvgtime_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen);
static int SampleSetAvgtime_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int SampleSetSamplerate_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int SampleSetSamplerate_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
static int SampleSetFiberset_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen);
static int SampleSetFiberset_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen);
/***
  * 功能：
        窗体的初始化函数，建立窗体控件、注册消息处理
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSampleSetInit(void *pWndObj)
{
	//错误标志，返回值定义 
    int iRet = 0;
	//临时变量定义 
	int i = 0;
    GUIMESSAGE *pMsg = NULL;

	unsigned int strMenu[] = {	
		1
	};

	//得到当前窗体对象 
    pFrmSampleSet = (GUIWINDOW *)pWndObj;
	//初始化参数 
	SampleInitPara();
    FrmSampleSetRes_Init(NULL, 0, NULL, 0);

	//建立状态栏、桌面、信息栏 
    pSampleSetBg = CreatePhoto("bg_sampleset");
    pSampleSetBgTableTitle = CreatePhoto("otdr_top1f");
    CreateLoopPhoto(pSampleSetInputWavePara, "btn_sampleset_index", 0, 0, 3);
    CreateLoopPhoto(pSampleSetBtnDefValue, "btn_sampleset_default_unpress", 0, 0, 3);
    CreateLoopPhoto(pSampleAvgTime, "bg_avgtime_unselect", 0, 0, 7);
    CreateLoopPhoto(pSampleSamplerate, "bg_samplerate_unselect", 0, 0, 2);
    CreateLoopPhoto(pSampleFiberset, "bg_fiberset_unselect", 0, 0, 2);
    //单位换算相关的图片以及文本
    pSampleSetLblUnitConverter = CreateLabel(183, 250, 200, 24, pSampleSetStrUnitConverter);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblUnitConverter);
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        pSampleSetBtnUnit[i] = CreatePicture(183 + 90*(i%2), 281 + (i/2)*35, 25, 25, BmpFileDirectory"bg_samplerate_unselect.bmp");
        pSampleSetLblUnit[i] = CreateLabel(215 + 90*(i%2), 285 + (i/2)*35, 65, 24, pSampleSetStrUnit[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblUnit[i]);
    }
    
    for(i = 0; i < 7; ++i)
    {
        if(i < 2)
        {
            pLblAvgtime[i] = CreateLabel(110, 145 + i * 35, 70, 24, pStrAvgtime[i]);
            pLblSamplerate[i] = CreateLabel(215, 145 + i * 35, 70, 24, pStrSamplerate[i]);
            pLblFiberset[i] = CreateLabel(445 + i * 102, 120, 70, 24, pStrFiberset[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLblAvgtime[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLblSamplerate[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLblFiberset[i]);
        }
        else
        {
            pLblAvgtime[i] = CreateLabel(110, 142 + i * 36, 70, 24, pStrAvgtime[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLblAvgtime[i]);
        }  
    }
	for (i = 0; i < 3; ++i)
	{
		pSampleSetLblFiberL[i] = CreateLabel(412, 170 + i * 76, 220, 24,  
											 pSampleSetStrFiberL[i]);
		pSampleSetLblFiberR[i] = CreateLabel(417, 203 + i * 76, 100, 24, 
											 pSampleSetStrFiberR[i]);
		pSampleSetLblDefValue[i]= CreateLabel(564, 206 + i * 76, 59, 24, 
											  pSampleSetStrDefValue);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pSampleSetLblDefValue[i]);
		SetLabelAlign(GUILABEL_ALIGN_LEFT, pSampleSetLblFiberR[i]);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblFiberL[i]);
        SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblDefValue[i]);
	}
	
	//桌面上的标签 
	pSampleSetLblTitle = CreateLabel(0, 24, 100, 24, pSampleSetStrTitle);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSampleSetLblTitle);
	pSampleSetLblTitleOTDRSettings = CreateLabel(401, 24, 281, 24, pSampleSetStrTitleOTDRSettings);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pSampleSetLblTitleOTDRSettings);
	pSampleSetLblPara = CreateLabel(35, 70, 300, 24, pSampleSetStrPara);
	pSampleSetLblSmptime = CreateLabel(71, 110, 200, 24, pSampleSetStrSmptime);
	pSampleSetLblInterval = CreateLabel(183, 110, 200, 24, pSampleSetStrInterval);
	pSampleSetLblFiberSet = CreateLabel(355, 70, 300, 24, pSampleSetStrFiberSet);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSampleSetLblTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pSampleSetLblTitleOTDRSettings);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblPara);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblSmptime);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblInterval);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblFiberSet);
    
	//菜单控件
	pSampleSetMenu = CreateWndMenu1(4, sizeof(strMenu), strMenu, 
									(UINT16)(MENU_BACK | ~MENU_HOME), 1, 0, 40, 
						  		    SampleMenuCallBack);
		
    //注册窗体控件，只对那些需要接收事件的控件进行
    //即如果该控件没有或者不需要响应输入事件，可以无需注册
    //注册窗体控件必须在注册消息处理函数之前进行

	//注册窗体(因为所有的按键事件都统一由窗体进行处理) 
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), 
                  pFrmSampleSet, pFrmSampleSet);

    //注册单位换算相关的控件
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pSampleSetBtnUnit[i], pFrmSampleSet);
        AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
					  pSampleSetLblUnit[i], pFrmSampleSet);
    }
    
	for (i = 0; i < 3; ++i)
	{
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pSampleSetInputWavePara[i], pFrmSampleSet);
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
					  pSampleSetBtnDefValue[i], pFrmSampleSet);
	}
    for (i = 0; i < 7; ++i)
    {
        if( i < 2)
        {
            AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
                    pLblAvgtime[i], pFrmSampleSet);
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                    pSampleAvgTime[i], pFrmSampleSet);
            AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
                    pLblSamplerate[i], pFrmSampleSet);
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                    pSampleSamplerate[i], pFrmSampleSet);
            AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
                    pLblFiberset[i], pFrmSampleSet);
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                    pSampleFiberset[i], pFrmSampleSet);
        }
        else
        {
            AddWindowComp(OBJTYP_GUILABEL, sizeof(GUILABEL), 
                    pLblAvgtime[i], pFrmSampleSet);
            AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                    pSampleAvgTime[i], pFrmSampleSet);
        }
    }
    
	//菜单控件
	AddWndMenuToComp1(pSampleSetMenu, pFrmSampleSet);


    //注册消息处理函数，如果消息接收对象未曾注册到窗体，将无法正常接收消息
    //即此处的操作应当在注册窗体控件的基础上进行
    //注册消息处理函数必须在注册窗体控件之后进行
    pMsg = GetCurrMessage();
    
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleSetBtnUnit[i], 
                        SampleSetUnit_Down, (void*)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleSetBtnUnit[i], 
                        SampleSetUnit_Up, (void*)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleSetLblUnit[i], 
                        SampleSetUnit_Down, (void*)(i), sizeof(int), pMsg);
        LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleSetLblUnit[i], 
                        SampleSetUnit_Up, (void*)(i), sizeof(int), pMsg);
    }
    
    for(i = 0; i < 7; ++i)
    {
        if(i < 2)
        {
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLblAvgtime[i], 
                        SampleSetAvgtime_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pLblAvgtime[i], 
                        SampleSetAvgtime_Up, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLblSamplerate[i], 
                        SampleSetSamplerate_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pLblSamplerate[i], 
                        SampleSetSamplerate_Up, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLblFiberset[i], 
                        SampleSetFiberset_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pLblFiberset[i], 
                        SampleSetFiberset_Up, (void*)(i), sizeof(int), pMsg);

            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleAvgTime[i], 
                        SampleSetAvgtime_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleAvgTime[i], 
                        SampleSetAvgtime_Up, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleSamplerate[i], 
                        SampleSetSamplerate_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleSamplerate[i], 
                        SampleSetSamplerate_Up, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleFiberset[i], 
                        SampleSetFiberset_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleFiberset[i], 
                        SampleSetFiberset_Up, (void*)(i), sizeof(int), pMsg);
        }
        else
        {
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pLblAvgtime[i], 
                        SampleSetAvgtime_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pLblAvgtime[i], 
                        SampleSetAvgtime_Up, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleAvgTime[i], 
                        SampleSetAvgtime_Down, (void*)(i), sizeof(int), pMsg);
            LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleAvgTime[i], 
                        SampleSetAvgtime_Up, (void*)(i), sizeof(int), pMsg);
        }
        
    }
	//输入框消息处理注册
	for (i = 0; i < 3; ++i)
	{
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleSetInputWavePara[i], 
                    	SampleSetInputWave_Down, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleSetInputWavePara[i], 
                		SampleSetInputWave_Up, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSampleSetBtnDefValue[i], 
                    	SampleSetBtnDef_Down, (void*)(i+1), sizeof(int), pMsg);
		LoginMessageReg(GUIMESSAGE_TCH_UP, pSampleSetBtnDefValue[i], 
                		SampleSetBtnDef_Up, (void*)(i+1), sizeof(int), pMsg);
	}
	//注册菜单控件消息处理
	LoginWndMenuToMsg1(pSampleSetMenu, pFrmSampleSet);

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
int FrmSampleSetExit(void *pWndObj)
{
    //错误标志、返回值定义 
    int iRet = 0;
    //临时变量定义 
    GUIMESSAGE *pMsg = NULL;
	int i = 0;

	//得到当前窗体对象 
    pFrmSampleSet = (GUIWINDOW *) pWndObj;

    //清空消息队列中的消息注册项
	//必须在持有消息队列互斥锁的情况下操作
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);
	
	//从当前窗体中注销窗体控件
	//必须在持有控件队列互斥锁的情况下操作
    ClearWindowComp(pFrmSampleSet);

	//销毁控件 
	//桌面上的控件 
	DestroyPicture(&pSampleSetBg);
    DestroyPicture(&pSampleSetBgTableTitle);
    //摧毁单位换算相关的控件
    DestroyLabel(&pSampleSetLblUnitConverter);
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        DestroyPicture(&pSampleSetBtnUnit[i]);
        DestroyLabel(&pSampleSetLblUnit[i]);
    }
    
	for (i = 0; i < 3; ++i)
	{
		DestroyPicture(&pSampleSetInputWavePara[i]);
		DestroyPicture(&pSampleSetBtnDefValue[i]);
	}
	for (i = 0; i < 7; ++i)
    {
        if(i < 2)
        {
            DestroyPicture(&pSampleAvgTime[i]);
            DestroyPicture(&pSampleSamplerate[i]);
            DestroyPicture(&pSampleFiberset[i]);
        }
        else
        {
            DestroyPicture(&pSampleAvgTime[i]);
        }
    }   
	//销毁标签 
	//桌面上的标签 
	DestroyLabel(&pSampleSetLblTitle);    
	DestroyLabel(&pSampleSetLblTitleOTDRSettings);    
	DestroyLabel(&pSampleSetLblPara);
	DestroyLabel(&pSampleSetLblSmptime);
	DestroyLabel(&pSampleSetLblInterval);
	DestroyLabel(&pSampleSetLblFiberSet);
	for(i = 0; i < 3; ++i){
		DestroyLabel(&pSampleSetLblFiberL[i]);
		DestroyLabel(&pSampleSetLblFiberR[i]);
		DestroyLabel(&pSampleSetLblDefValue[i]);
	}
    for(i = 0; i < 7; ++i)
    {
        if(i < 2)
        {
            DestroyLabel(&pLblAvgtime[i]);
            DestroyLabel(&pLblSamplerate[i]);
            DestroyLabel(&pLblFiberset[i]);
        }
        else
        {
            DestroyLabel(&pLblAvgtime[i]);
        }   
    }
    
	//销毁菜单控件
	DestroyWndMenu1(&pSampleSetMenu);

	//释放文本资源 
    FrmSampleSetRes_Exit(NULL, 0, NULL, 0);

	//清理系统相关参数 
	SampleClearPara();
	
	//退出并保存
	SetSettingsData((void*)&pUser_Settings->sSampleSetting, sizeof(SAMPLE_SETTING), SAMPLE_SET);
    SaveSettings(SAMPLE_SET);
    SetSettingsData((void*)&pUser_Settings->sCommonSetting, sizeof(COMMON_SETTING), COMMON_SET);
    SaveSettings(COMMON_SET);
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
int FrmSampleSetPaint(void *pWndObj)
{
    //错误标志、返回值定义 
    int iRet = 0;
	int i = 0;
	
    //得到当前窗体对象 
    pFrmSampleSet = (GUIWINDOW *) pWndObj;

	//桌面上的控件 
	DisplayPicture(pSampleSetBg);
    DisplayPicture(pSampleSetBgTableTitle);
    
	for (i = 0; i < 3; ++i)
	{
		DisplayPicture(pSampleSetInputWavePara[i]);
		DisplayPicture(pSampleSetBtnDefValue[i]);
	}
    for (i = 0; i < 7; ++i)
    {
        if(i < 2)
        {
            DisplayPicture(pSampleAvgTime[i]);
            DisplayPicture(pSampleSamplerate[i]);
            DisplayPicture(pSampleFiberset[i]);
        }
        else
        {
            DisplayPicture(pSampleAvgTime[i]);
        }
    }   
	//桌面上的标签 
	DisplayLabel(pSampleSetLblTitle);    
	DisplayLabel(pSampleSetLblTitleOTDRSettings);    
	DisplayLabel(pSampleSetLblPara);
	DisplayLabel(pSampleSetLblSmptime);
	DisplayLabel(pSampleSetLblInterval);
	DisplayLabel(pSampleSetLblFiberSet);
	for(i = 0; i < 3; ++i)
	{
		DisplayLabel(pSampleSetLblFiberL[i]);
		DisplayLabel(pSampleSetLblFiberR[i]);
		DisplayLabel(pSampleSetLblDefValue[i]);
	}
    for(i = 0; i < 7; ++i)
    {
        if(i < 2)
        {
            DisplayLabel(pLblAvgtime[i]);
            DisplayLabel(pLblSamplerate[i]);
            DisplayLabel(pLblFiberset[i]);
        }
        else
        {
            DisplayLabel(pLblAvgtime[i]);
        }   
    }
    
	//菜单控件
	DisplayWndMenu1(pSampleSetMenu);
	SetPowerEnable(1, 1);
    //显示单位换算相关的控件
    DisplayLabel(pSampleSetLblUnitConverter); 
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        if(i == iUnitConverterFlag)
        {
            SetPictureBitmap(BmpFileDirectory"bg_samplerate_select.bmp", pSampleSetBtnUnit[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pSampleSetLblUnit[i]);
        }
        else
        {
            SetPictureBitmap(BmpFileDirectory"bg_samplerate_unselect.bmp", pSampleSetBtnUnit[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblUnit[i]);
        }
        DisplayPicture(pSampleSetBtnUnit[i]);
        DisplayLabel(pSampleSetLblUnit[i]);
    }
    
    for(i = 0; i < 7; ++i)
    {
        if((i + 1) == pSamSet->enAvg)
        {
            TouchChange("bg_avgtime_select.bmp", pSampleAvgTime[i],
                pStrAvgtime[i], pLblAvgtime[i], 2);
        }
    }
    for(i = 0; i < 2; ++i)
    {
        if( i == pSamSet->enSamVal)
        {
            TouchChange("bg_samplerate_select.bmp", pSampleSamplerate[i],
                pStrSamplerate[i], pLblSamplerate[i], 2);
        }
    }
    TouchChange("bg_fiberset_select.bmp", pSampleFiberset[iSelectedWave],
                pStrFiberset[iSelectedWave], pLblFiberset[iSelectedWave], 2);
	//刷新帧缓冲 
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
int FrmSampleSetLoop(void *pWndObj)
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
int FrmSampleSetPause(void *pWndObj)
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
int FrmSampleSetResume(void *pWndObj)
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
static int FrmSampleSetRes_Init(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iRet = 0;
	
	//桌面上的文本 
	pSampleSetStrTitle 	= TransString("OTDR_SAMPLESET_SAMPLESET");  
	pSampleSetStrTitleOTDRSettings = TransString("OTDR_LBL_OTDRSET");    
	pSampleSetStrPara = TransString("OTDR_SAMPLESET_SAMPLESET");
	pSampleSetStrSmptime = TransString("OTDR_SAMPLESET_AVGTIME");
	pSampleSetStrInterval = TransString("OTDR_SAMPLESET_SAMPLEINTERVAL");
	pSampleSetStrFiberSet = TransString("OTDR_SAMPLESET_NEWFIBERSET");
    //单位换算相关的文本
    pSampleSetStrUnitConverter = TransString("SAMPLESET_LABEL_UNITCONVERTER");
    int i;
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        pSampleSetStrUnit[i] = TransString("km");
    }
	//反射率、回波损耗等
	pSampleSetStrFiberL[0] = TransString("OTDR_SAMPLESET_REFRACTIVE");
	pSampleSetStrFiberL[1] = TransString("OTDR_SAMPLESET_BACKSCATTERING");
	pSampleSetStrFiberL[2] = TransString("OTDR_SAMPLESET_EXCESSLENGTH");
	
	pSampleSetStrFiberR[0] = Float2GuiString(IOR_PRECISION, 
	    pSamSet->fRefrac[iSelectedWave]);
	pSampleSetStrFiberR[1] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, 
	    pSamSet->fBackSc[iSelectedWave]);
	pSampleSetStrFiberR[2] = Float2GuiString(HELIX_FACTOR_PRECISION, 
	    pSamSet->fExceLen[iSelectedWave]);

	//默认值按钮标签
	pSampleSetStrDefValue = TransString("OTDR_SAMPLESET_DEFAULT");
    
    for(i = 0; i < 7; ++i)
    {
        if( i < 2)
        {
            pStrAvgtime[i] = TransString(pAvgtime[i]);
            pStrSamplerate[i] = GetCurrLanguageText(pSamplerate[i]);
            pStrFiberset[i] = TransString(pFiberset[i]);
        }
        else
        {
            pStrAvgtime[i] = TransString(pAvgtime[i]);
        }
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
static int FrmSampleSetRes_Exit(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int iRet = 0;
	int i = 0;
	
	//桌面上的文本 
    GuiMemFree(pSampleSetStrTitle);  
    GuiMemFree(pSampleSetStrTitleOTDRSettings);    
	GuiMemFree(pSampleSetStrPara);
	GuiMemFree(pSampleSetStrSmptime);
	GuiMemFree(pSampleSetStrInterval);
	GuiMemFree(pSampleSetStrFiberSet);
    //单位换算的文本
    GuiMemFree(pSampleSetStrUnitConverter);
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        GuiMemFree(pSampleSetStrUnit[i]);
    }
	for (i = 0; i < 3; ++i)
	{
		GuiMemFree(pSampleSetStrFiberL[i]);
		GuiMemFree(pSampleSetStrFiberR[i]);
	}
	GuiMemFree(pSampleSetStrDefValue);
    for(i = 0; i < 7; ++i)
    {
        if( i < 2)
        {
            GuiMemFree(pStrAvgtime[i]);
            GuiMemFree(pStrSamplerate[i]);
            GuiMemFree(pStrFiberset[i]);
        }
        else
        {
           GuiMemFree(pStrAvgtime[i]); 
        }
    }
    
	return iRet;
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
static int SampleInitPara(void)
{
	int i = 0;
	
	pSamSet = malloc(sizeof(struct OtdrSampleSet));
	if (!pSamSet)
	{
		CODER_LOG(CoderYun, "Malloc failed\n");
		return -1;
	}

	//读取全局变量初始化该结构体 ，必须获得锁
	CODER_LOG(CoderYun, "Geting System set...\n");
	pSamSet->enAvg = pUser_Settings->sSampleSetting.enAutoAvrTime;
	pSamSet->enSamVal = pUser_Settings->sSampleSetting.enSmpResolution;
	for (i = 0; i < 3; ++i)
	{
		pSamSet->fRefrac[i] = pUser_Settings->sSampleSetting.sWaveLenArgs[i].fRefractiveIndex;
		pSamSet->fBackSc[i] = pUser_Settings->sSampleSetting.sWaveLenArgs[i].fBackScattering;
		pSamSet->fExceLen[i] = pUser_Settings->sSampleSetting.sWaveLenArgs[i].fExcessLength;
	}
    iUnitConverterFlag = pUser_Settings->sCommonSetting.iUnitConverterFlag;
	
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
static int SampleClearPara(void)
{
	int i = 0;
	
 	//将数据回写 
 	CODER_LOG(CoderYun, "Saving System set...\n");
	pUser_Settings->sSampleSetting.enAutoAvrTime = pSamSet->enAvg;
	pUser_Settings->sSampleSetting.enSmpResolution = pSamSet->enSamVal;
	for (i = 0; i < 3; ++i)
	{
		pUser_Settings->sSampleSetting.sWaveLenArgs[i].fRefractiveIndex = pSamSet->fRefrac[i];
		pUser_Settings->sSampleSetting.sWaveLenArgs[i].fBackScattering = pSamSet->fBackSc[i];
		pUser_Settings->sSampleSetting.sWaveLenArgs[i].fExcessLength = pSamSet->fExceLen[i];
	}
    pUser_Settings->sCommonSetting.iUnitConverterFlag = iUnitConverterFlag;

	//释放临时空间
	free(pSamSet);
	pSamSet = NULL;
	
	return 0;
}

/*重绘窗体函数*/
static void ReCreateSampleSetWindow(GUIWINDOW **pWnd)
{	
    *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
 						 FrmSampleSetInit, FrmSampleSetExit, 
 						 FrmSampleSetPaint, FrmSampleSetLoop, 
 						 FrmSampleSetPause, FrmSampleSetResume,
 						 NULL);
}

/*刷新不同波形相关信息显示的函数*/
static void SampleFlushDisplay(int iIndex, int iWaveIndex)
{
	free(pSampleSetStrFiberR[iIndex]);
	switch (iIndex)
	{
	case 0://反射率
		pSampleSetStrFiberR[iIndex] = Float2GuiString(IOR_PRECISION, 
		    pSamSet->fRefrac[iWaveIndex]);
		break;
	case 1://回波损耗
		pSampleSetStrFiberR[iIndex] = Float2GuiString(BACKSCATTER_COEFFICIENT_PRECISION, 
		    pSamSet->fBackSc[iWaveIndex]);
		break;
	case 2://余长系数
		pSampleSetStrFiberR[iIndex] = Float2GuiString(HELIX_FACTOR_PRECISION, 
		    pSamSet->fExceLen[iWaveIndex]);
		break;

	default:
		break;
	}
	
	SetLabelText(pSampleSetStrFiberR[iIndex], pSampleSetLblFiberR[iIndex]);
	DisplayPicture(pSampleSetInputWavePara[iIndex]);
	DisplayLabel(pSampleSetLblFiberR[iIndex]); 
}

//平均测量时间选择器回调函数 
static void AvgTimeBackFunc (int iSelected)
{
	pSamSet->enAvg = iSelected + 1;
}

//波长选择器回调函数 
static void WaveLenBackFunc (int iSelected)
{
	if (iSelectedWave != iSelected)
	{	
		int i;
		iSelectedWave = iSelected;
		for (i = 0; i < 3; ++i)
		{
			SampleFlushDisplay(i, iSelectedWave);
		}
	}
}

//采样分辨率选择器回调函数 
static void IntervalBackFunc (int iSelected)
{
	pSamSet->enSamVal = iSelected;
}

//输入反射率键盘控件的回调函数 
static void InputRefrBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	double fTmp = String2Float(cTmpBuff);
	//1.00~2.00
	if (fTmp < String2Float("1.00") || fTmp > String2Float("2.00"))
	{
		CreateIMEDialog(TransString("IME_REFRAC_LIMIT"));
		return;
	}

	//此处需要检查参数，暂时省略
	pUser_Settings->sSampleSetting.sWaveLenArgs[iSelectedWave].fRefractiveIndex = fTmp;
}

//输入回波损耗键盘控件的回调函数 
static void InputBackBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	double fTmp = String2Float(cTmpBuff);

	//此处需要检查参数，暂时省略
	//-89.44~ -65.45
	if(iSelectedWave == 0)
	{
    	if (fTmp < String2Float("-89.44") || fTmp > String2Float("-65.45"))
    	{
    		CreateIMEDialog(TransString("IME_BACKLOSS_LIMIT"));
    		return;
    	}
    }
    else if(iSelectedWave == 1)
    {
    	if (fTmp < String2Float("-91.86") || fTmp > String2Float("-71.87"))
    	{
    		CreateIMEDialog(TransString("IME_BACKLOSS_LIMIT_1550"));
    		return;
    	}
    }

	pUser_Settings->sSampleSetting.sWaveLenArgs[iSelectedWave].fBackScattering = fTmp;
}

//输入余长系数耗键盘控件的回调函数 
static void InputExceBackFunc(void)
{
	char cTmpBuff[100];

	GetIMEInputBuff(cTmpBuff);

	double fTmp = String2Float(cTmpBuff);

	//此处需要检查参数，暂时省略
	//0 ~10
	if (fTmp < String2Float("0.0") || fTmp > String2Float("10.0"))
	{
		CreateIMEDialog(TransString("IME_EXECE_LIMIT"));
		return;
	}

	pUser_Settings->sSampleSetting.sWaveLenArgs[iSelectedWave].fExcessLength = fTmp;
}

//反射率默认设置函数 
static void WaveRefrDefaultValue(void)
{
	switch (iSelectedWave)
	{
	case 0://选中1310波长
		pSamSet->fRefrac[0] = 1.467700f;
		break;
	case 1://选中1550波长
		pSamSet->fRefrac[1] = 1.468325f;
		break;
	case 2://选中1625波长
		pSamSet->fRefrac[2] = 1.460000f;
		break;

	default:
		break;
	}
	SampleFlushDisplay(0, iSelectedWave);
}

//回波损耗默认设置函数
static void WaveBackDefaultValue(void)
{
	switch (iSelectedWave)
	{
	case 0://选中1310波长
		pSamSet->fBackSc[0] = -79.44f;
		break;
	case 1://选中1550波长
		pSamSet->fBackSc[1] = -81.87f;
		break;
	case 2://选中1625波长
		pSamSet->fBackSc[2] = -82.58f;
		break;

	default:
		break;
	}
	SampleFlushDisplay(1, iSelectedWave);
}

//余长系数默认设置函数
static void WaveExceDefaultValue(void)
{
	//所有波形的默认余长系数相同
	pSamSet->fExceLen[iSelectedWave] = 0.0f;
	SampleFlushDisplay(2, iSelectedWave);
}
//单位换算处理函数
static int SampleSetUnit_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    int iTmp = (int)pOutArg;
    int i;
    for(i = 0; i < MAX_OTDR_UNIT_NUM; ++i)
    {
        if(i == iTmp)
        {
            TouchChange("bg_samplerate_select.bmp", pSampleSetBtnUnit[i], 
                    pSampleSetStrUnit[i], pSampleSetLblUnit[i], 2);
        }
        else
        {
            TouchChange("bg_samplerate_unselect.bmp", pSampleSetBtnUnit[i], 
                    pSampleSetStrUnit[i], pSampleSetLblUnit[i], 1);
        }
    }

    iUnitConverterFlag = iTmp;
    //刷新帧缓冲 
	RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}

static int SampleSetUnit_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义 
    int iRet = 0;

    return iRet;
}

//输入框按下处理函数
static int SampleSetInputWave_Down(void *pInArg, int iInLen, 
                                   void *pOutArg, int iOutLen)
{
	int iTmp = (int)pOutArg;
	if (--iTmp > 2)
	{
		return -1;
	}
	
	TouchChange("btn_sampleset_index.bmp", pSampleSetInputWavePara[iTmp], 
				NULL, pSampleSetLblFiberR[iTmp], 0);
	RefreshScreen(__FILE__, __func__, __LINE__);
	return 0;
}
static int SampleSetInputWave_Up(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
	char *buf = NULL;

	int iTmp = (int)pOutArg;
	if (--iTmp > 2)
	{
		return -1;
	}
	
	SetPictureBitmap(BmpFileDirectory"btn_sampleset_index.bmp", 
					 pSampleSetInputWavePara[iTmp]);
	DisplayPicture(pSampleSetInputWavePara[iTmp]);
	DisplayLabel(pSampleSetLblFiberR[iTmp]);
	RefreshScreen(__FILE__, __func__, __LINE__);
	switch (iTmp)
	{
	case 0://调用键盘输入反射率 
	    buf = Float2String(IOR_PRECISION, pSamSet->fRefrac[iSelectedWave]);
		IMEInit(buf, 10, 1, ReCreateSampleSetWindow, InputRefrBackFunc, NULL);
		break;
	case 1://调用键盘输入回波损耗 
	    buf = Float2String(BACKSCATTER_COEFFICIENT_PRECISION, pSamSet->fBackSc[iSelectedWave]);
		IMEInit(buf, 10, 1, ReCreateSampleSetWindow, InputBackBackFunc, NULL);
		break;
	case 2://调用键盘输入余长系数 
	    buf = Float2String(HELIX_FACTOR_PRECISION, pSamSet->fExceLen[iSelectedWave]);
		IMEInit(buf, 10, 1, ReCreateSampleSetWindow, InputExceBackFunc, NULL);
		break;

	default:
		break;
	}

	if(buf)
	{
        free(buf);
        buf = NULL;
	}
	
	return 0;
}

//波形参数默认值点击处理函数
static int SampleSetBtnDef_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
	int iTmp = (int)pOutArg;
	if (--iTmp > 2)
	{
		return -1;
	}

	TouchChange("btn_sampleset_default_press.bmp", pSampleSetBtnDefValue[iTmp], 
				NULL, pSampleSetLblDefValue[iTmp], 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
	
	return 0;
}
static int SampleSetBtnDef_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
	int iTmp = (int)pOutArg;
	if (--iTmp > 2)
	{
		return -1;
	}
	
	SetPictureBitmap(BmpFileDirectory"btn_sampleset_default_unpress.bmp", 
					 pSampleSetBtnDefValue[iTmp]);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSampleSetLblDefValue[iTmp]);
	DisplayPicture(pSampleSetBtnDefValue[iTmp]);
	DisplayLabel(pSampleSetLblDefValue[iTmp]);
	switch (iTmp)
	{
	case 0://设置反射率为默认值
		WaveRefrDefaultValue();
		break;
	case 1://设置回波损耗为默认值
		WaveBackDefaultValue();
		break;
	case 2://设置余长系数为默认值
		WaveExceDefaultValue();
		break;

	default:
		break;
	}
	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}
static int SampleSetAvgtime_Down(void *pInArg, int iInLen, 
                                 void *pOutArg, int iOutLen)
{
    return 0;
}
static int SampleSetAvgtime_Up(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    int i = (int)pOutArg;
    int j;
    AvgTimeBackFunc(i);
    for(j = 0; j < 7; ++j)
    {
        if( j != i)
        {
            TouchChange("bg_avgtime_unselect.bmp", pSampleAvgTime[j],
                pStrAvgtime[j], pLblAvgtime[j], 1);
        }
        else
        {
            TouchChange("bg_avgtime_select.bmp", pSampleAvgTime[i],
                pStrAvgtime[i], pLblAvgtime[i], 2);
        }
    }
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int SampleSetSamplerate_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    return 0;
}
static int SampleSetSamplerate_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int i = (int)pOutArg;
    int j;
    IntervalBackFunc(i);
    for(j = 0; j < 2; ++j)
    {
        if(j == i)
        {
            TouchChange("bg_samplerate_select.bmp", pSampleSamplerate[j],
                pStrSamplerate[j], pLblSamplerate[j], 2);
        }
        else
        {
            TouchChange("bg_samplerate_unselect.bmp", pSampleSamplerate[j],
                pStrSamplerate[j], pLblSamplerate[j], 1);
        }
    }
    
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0;
}
static int SampleSetFiberset_Down(void *pInArg, int iInLen, 
                                void *pOutArg, int iOutLen)
{
    return 0;
}
static int SampleSetFiberset_Up(void *pInArg, int iInLen, 
                              void *pOutArg, int iOutLen)
{
    int i = (int)pOutArg;
    int j;
    for(j = 0; j < 2; ++j)
    {
        if( j != i)
        {
            TouchChange("bg_avgtime_unselect.bmp", pSampleFiberset[j],
                pStrFiberset[j], pLblFiberset[j], 1);
        }
        else
        {
            TouchChange("bg_avgtime_select.bmp", pSampleFiberset[i],
                pStrFiberset[i], pLblFiberset[i], 2);
        }
    }
    WaveLenBackFunc(i);
    RefreshScreen(__FILE__, __func__, __LINE__);
    return 0; 
}
// 侧边菜单控件回调函数 
static void SampleMenuCallBack(int iOption)
{
	GUIWINDOW *pWnd = NULL;
	Stack *ps = NULL;
	Item func;
	
	switch (iOption)
	{
	case 0://调取一般设置窗体 
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmCommonSetInit, FrmCommonSetExit, 
		                    FrmCommonSetPaint, FrmCommonSetLoop, 
					        FrmCommonSetPause, FrmCommonSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmSampleSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 2:	//调用分析设置窗体 
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmAnalysSetInit, FrmAnalysSetExit, 
		                    FrmAnalysSetPaint, FrmAnalysSetLoop, 
					        FrmAnalysSetPause, FrmAnalysSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmSampleSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case 3://事件表阈值窗口
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		                    FrmOtdrEventThresholdSetInit, FrmOtdrEventThresholdSetExit, 
		                    FrmOtdrEventThresholdSetPaint, FrmOtdrEventThresholdSetLoop, 
					        FrmOtdrEventThresholdSetPause, FrmOtdrEventThresholdSetResume,
		                    NULL);          
	    SendWndMsg_WindowExit(pFrmSampleSet);  
	    SendSysMsg_ThreadCreate(pWnd); 
		break;
	case BACK_DOWN://退出OTDR设置界面 
		ps = GetCurrWndStack();
		WndPop(ps, &func, pFrmSampleSet);			
		break;		

	case HOME_DOWN://退出
	 	break;
		
	default:
		break;
	}
}

