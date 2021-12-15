/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmopm.c
* 摘    要：  实现主窗体FrmOPM的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/30 
*
*******************************************************************************/

#include "wnd_frmopm.h"

#include "guipicture.h"
#include "guiwindow.h"
#include "guibase.h"
#include "guilabel.h"
#include "guimessage.h"

#include "app_unitconverter.h"
#include "app_getopmdata.h"
#include "app_parameter.h"

#include "wnd_global.h"
#include "wnd_frmmain.h"
#include "wnd_frmmenubak.h"

extern PCALIBRATIONPARA pCalibrationPara;
/*********************************背景控件定义*********************************/
static GUIPICTURE* pFrmOPMBg = NULL;                    //背景

/*******************************************************************************
**							窗体FrmOPM中的控件定义部分						**
*******************************************************************************/
static GUIWINDOW *pFrmOPM = NULL;

/*******************************************************************************
**							窗体FrmOPM 标题栏中的控件定义部分				 **
*******************************************************************************/
static GUICHAR* pFrmOPMStrTitle = NULL;                 //OPM
static GUILABEL* pFrmOPMLblTitle = NULL;

/*******************************************************************************
**							窗体FrmOPM 功能区中的控件定义部分				 **
*******************************************************************************/
static GUIPICTURE* pFrmOPMBgIcon = NULL;
static GUIPICTURE* pFrmOPMBgWaveLength = NULL;
static GUIPICTURE* pFrmOPMBgRefValue = NULL;
static GUIPICTURE* pFrmOPMBgRefUnit = NULL;
static GUIPICTURE* pFrmOPMBgDbValue = NULL;
static GUIPICTURE* pFrmOPMBgDbUnit = NULL;
static GUIPICTURE* pFrmOPMBgPowerValue = NULL;
static GUIPICTURE* pFrmOPMBgPowerUnit = NULL;
static GUIPICTURE* pFrmOPMBgWaveLengthUnit = NULL;		 //波长单位图片nm

static GUILABEL* pFrmOPMLblWaveLength = NULL;            //波长文本
static GUILABEL* pFrmOPMLblWaveLengthUnit = NULL;		 //波长单位文本（为了与参考值单位显示统一）
static GUILABEL* pFrmOPMLblRefValue = NULL;              //参考值数值文本
static GUILABEL* pFrmOPMLblRefUnit = NULL;               //参考值数值单位文本
static GUILABEL* pFrmOPMLblDbValue = NULL;               //dB值文本
static GUILABEL* pFrmOPMLblDbUnit = NULL;                //dB单位
static GUILABEL* pFrmOPMLblPowerValue = NULL;            //功率值
static GUILABEL* pFrmOPMLblPowerUnit = NULL;             //功率值

static GUICHAR* pFrmOPMStrWaveLength = NULL;
static GUICHAR* pFrmOPMStrWaveLengthUnit = NULL;
static GUICHAR* pFrmOPMStrRef = NULL;
static GUICHAR* pFrmOPMStrRefValue = NULL;
static GUICHAR* pFrmOPMStrRefUnit = NULL;
static GUICHAR* pFrmOPMStrDbValue = NULL;
static GUICHAR* pFrmOPMStrDbUnit = NULL;
static GUICHAR* pFrmOPMStrPowerValue = NULL;
static GUICHAR* pFrmOPMStrPowerUnit = NULL;

/*******************************************************************************
*               窗体FrmOPM右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFrmOPMMenu = NULL;

/*******************************************************************************
*               窗体FrmOPM功能全局变量声明
*******************************************************************************/
#define WAVE_LENGTH_NUM           6
#define DB_UNIT_NUM               2
#define POWER_UNIT_NUM            4
#define MAX_POWER_VALUE           3981.072f   // 6dBm（μw）
#define MIN_POWER_VALUE           0.0001     // -70dBm（μw）
#define MIN_LOW_POWER_VALUE       MIN_POWER_VALUE * 1000        // -70dBm (nw)
#define MIN_REF_VALUE             1000000     // 1mw
#define MAX_NUMBER_BIT_NUM        7           //最大显示数字图片的位数
#define POINT_BIT_NUMBER          3           //小数点所在的位数
#define OPM_FUNCTION_NUM          4           //OPM右侧菜单栏项数

//静态全局变量
static unsigned int iCurrWaveLengthIndex = 0;           //记录当前显示波长的索引值
static unsigned int iCurrDbUnitIndex = 0;               //记录当前dB单位的索引值
static unsigned int iCurrPowerUnitIndex = 2;               //记录当前功率单位的索引值

static int isWaveSelected = 0;				//波长的未选中/选中（0：未选中，1：选中）
static int isRefSelected = 0;				//参考值未选中/选中（0：未选中，1：选中）
static int isdBValueValid  = 0;				//得到的dbvalue是否有效，无效显示lo

//图片形式显示db
static GUIPICTURE *pFrmOPMBgDbList[MAX_NUMBER_BIT_NUM] = {NULL}; //例如：-23.456
//用于dbvalue无效显示的L
static char* pDbLBmp = BmpFileDirectory"bg_opm_L_DbNumber.bmp";//L
//显示图片资源
static char* pDbNullNumberBmp = BmpFileDirectory"bg_opm_null_DbNumber.bmp";         //空白
static char* pDbNegativeNumberBmp = BmpFileDirectory "bg_opm_negative_DbNumber.bmp";//负数
static char *pDbPointNumberBmp = BmpFileDirectory "bg_opm_point_DbNumber.bmp";      //小数点
//0~9
static char *pDbNumberBmp[10] =
{
    BmpFileDirectory "bg_opm_0_DbNumber.bmp",
    BmpFileDirectory "bg_opm_1_DbNumber.bmp",
    BmpFileDirectory "bg_opm_2_DbNumber.bmp",
    BmpFileDirectory "bg_opm_3_DbNumber.bmp",
    BmpFileDirectory "bg_opm_4_DbNumber.bmp",
    BmpFileDirectory "bg_opm_5_DbNumber.bmp",
    BmpFileDirectory "bg_opm_6_DbNumber.bmp",
    BmpFileDirectory "bg_opm_7_DbNumber.bmp",
    BmpFileDirectory "bg_opm_8_DbNumber.bmp",
    BmpFileDirectory "bg_opm_9_DbNumber.bmp"
};

///power数值
static GUIPICTURE *pFrmOPMBgPowerList[MAX_NUMBER_BIT_NUM] = {NULL}; //例如：123.456
//显示图片资源
static char* pNullNumberBmp = BmpFileDirectory"bg_opm_null_number.bmp";         //空白
static char* pNegativeNumberBmp = BmpFileDirectory "bg_opm_negative_number.bmp";//负数
static char *pPointNumberBmp = BmpFileDirectory "bg_opm_point_number.bmp";      //小数点
//0~9
static char *pNumberBmp[10] =
{
    BmpFileDirectory "bg_opm_0_number.bmp",
    BmpFileDirectory "bg_opm_1_number.bmp",
    BmpFileDirectory "bg_opm_2_number.bmp",
    BmpFileDirectory "bg_opm_3_number.bmp",
    BmpFileDirectory "bg_opm_4_number.bmp",
    BmpFileDirectory "bg_opm_5_number.bmp",
    BmpFileDirectory "bg_opm_6_number.bmp",
    BmpFileDirectory "bg_opm_7_number.bmp",
    BmpFileDirectory "bg_opm_8_number.bmp",
    BmpFileDirectory "bg_opm_9_number.bmp"
};

//REF图片参考值数字资源
static GUIPICTURE *pFrmOPMBgRefDbList[MAX_NUMBER_BIT_NUM] = {NULL}; //例如：-23.456
//REF未选中
static char* pRefNullNumberBmp = BmpFileDirectory"bg_opm_null_RefNumber.bmp";         //空白
static char* pRefNegativeNumberBmp = BmpFileDirectory "bg_opm_negative_RefNumber.bmp";//负数
static char *pRefPointNumberBmp = BmpFileDirectory "bg_opm_point_RefNumber.bmp";      //小数点
//0~9
static char *pRefNumberBmp[10] =
{
    BmpFileDirectory "bg_opm_0_RefNumber.bmp",
    BmpFileDirectory "bg_opm_1_RefNumber.bmp",
    BmpFileDirectory "bg_opm_2_RefNumber.bmp",
    BmpFileDirectory "bg_opm_3_RefNumber.bmp",
    BmpFileDirectory "bg_opm_4_RefNumber.bmp",
    BmpFileDirectory "bg_opm_5_RefNumber.bmp",
    BmpFileDirectory "bg_opm_6_RefNumber.bmp",
    BmpFileDirectory "bg_opm_7_RefNumber.bmp",
    BmpFileDirectory "bg_opm_8_RefNumber.bmp",
    BmpFileDirectory "bg_opm_9_RefNumber.bmp"
};
//ref选中
static char* pRefSelectNullNumberBmp = BmpFileDirectory"bg_opm_null_RefSelectNumber.bmp";         //空白
static char* pRefSelectNegativeNumberBmp = BmpFileDirectory "bg_opm_negative_RefSelectNumber.bmp";//负数
static char *pRefSelectPointNumberBmp = BmpFileDirectory "bg_opm_point_RefSelectNumber.bmp";      //小数点
//0~9
static char *pRefSelectNumberBmp[10] =
{
    BmpFileDirectory "bg_opm_0_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_1_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_2_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_3_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_4_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_5_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_6_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_7_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_8_RefSelectNumber.bmp",
    BmpFileDirectory "bg_opm_9_RefSelectNumber.bmp"
};

static char* waveLengthArray[WAVE_LENGTH_NUM] = 
{
    "850",
    "1300",
    "1310",
    "1490",
    "1550",
    "1650"
};

//wave波长图片
static GUIPICTURE *pFrmOPMBgWaveList[WAVE_LENGTH_NUM] = {NULL}; //例如：波长
//未选中
static char *pwaveBmp[WAVE_LENGTH_NUM] =
{
    BmpFileDirectory "bg_opm_850_wave.bmp",
    BmpFileDirectory "bg_opm_1300_wave.bmp",
    BmpFileDirectory "bg_opm_1310_wave.bmp",
    BmpFileDirectory "bg_opm_1490_wave.bmp",
    BmpFileDirectory "bg_opm_1550_wave.bmp",
    BmpFileDirectory "bg_opm_1650_wave.bmp"
};
//选中
static char *pwaveSelectBmp[WAVE_LENGTH_NUM] =
{
    BmpFileDirectory "bg_opm_850_waveSelect.bmp",
    BmpFileDirectory "bg_opm_1300_waveSelect.bmp",
    BmpFileDirectory "bg_opm_1310_waveSelect.bmp",
    BmpFileDirectory "bg_opm_1490_waveSelect.bmp",
    BmpFileDirectory "bg_opm_1550_waveSelect.bmp",
    BmpFileDirectory "bg_opm_1650_waveSelect.bmp"
};
//波长单位
//非选中
#define OPM_WAVE_UNIT           BmpFileDirectory"bg_opm_WaveUnit.bmp"
//选中
#define OPM_WAVE_UNIT_SELECT    BmpFileDirectory"bg_opm_WaveUnitSelect.bmp"

//单位 dB 、dBm
static GUIPICTURE *pFrmOPMBgDbUnitList[DB_UNIT_NUM] = {NULL}; //例如：波长单位
static char *pFrmOPMBgDbUnitBmp[DB_UNIT_NUM] =
{
    BmpFileDirectory "bg_opm_db_DbUnit.bmp",
    BmpFileDirectory "bg_opm_dbm_DbUnit.bmp"
};

//ref参考db单位
static GUIPICTURE *pFrmOPMBgRefUnitList[DB_UNIT_NUM] = {NULL}; 
//未选中
static char *pFrmOPMBgRefUnitBmp[DB_UNIT_NUM] =
{
    BmpFileDirectory "bg_opm_db_RefDbUnit.bmp",
    BmpFileDirectory "bg_opm_dbm_RefDbUnit.bmp"
};
//选中
static char *pFrmOPMBgRefSelectUnitBmp[DB_UNIT_NUM] =
{
    BmpFileDirectory "bg_opm_db_RefSelectDbUnit.bmp",
    BmpFileDirectory "bg_opm_dbm_RefSelectDbUnit.bmp"
};

static GUIPICTURE *pFrmOPMBgPowerUnitList[POWER_UNIT_NUM] = {NULL}; //例如：功率单位
static char *pFrmOPMBgPowerUnitBmp[POWER_UNIT_NUM] =
{
    BmpFileDirectory "bg_opm_mW_PowerUnit.bmp",
    BmpFileDirectory "bg_opm_uW_PowerUnit.bmp",
    BmpFileDirectory "bg_opm_nW_PowerUnit.bmp",
    BmpFileDirectory "bg_opm_pW_PowerUnit.bmp"
};

//各项非选中背景资源
static char *pOPMUnpressBmp[OPM_FUNCTION_NUM] =
{
    BmpFileDirectory "bg_opm_wave.bmp",
    BmpFileDirectory "bg_opm_dbm.bmp",
    BmpFileDirectory "bg_opm_ref.bmp",
    BmpFileDirectory "bg_opm_zero.bmp"
};

//各项选中背景资源
static char *pOPMSelectBmp[OPM_FUNCTION_NUM] =
{
    BmpFileDirectory "bg_opm_waveSelect.bmp",
    BmpFileDirectory "bg_opm_dbmSelect.bmp",
    BmpFileDirectory "bg_opm_refSelect.bmp",
    BmpFileDirectory "bg_opm_zeroSelect.bmp"
};
/*
static char* dBUnitArray[DB_UNIT_NUM] = 
{
    "dBm",
    "dB"
};
*/
static double powerValue [WAVE_LENGTH_NUM]= {0};// 记录功率值（μw）
static double refValue [WAVE_LENGTH_NUM] = {0}; // 记录参考值（μw）
static double refDbValue [WAVE_LENGTH_NUM] = {0}; // 记录参考值（μw）

static GUITHREAD threadRead = -1;
static int iThreadExitFlag = 1; //线程退出标识  0表示退出线程
static GUIMUTEX GStatusMutex;   //线程锁
//读取AD数值的线程
static void *ReadAdValue(void *pThreadArg);
//回收读取AD值线程
static void CancelReadADPthread();
/*******************************************************************************
*                   窗体FrmOPM内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int OPMTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//释放文本资源
static int OPMTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体FrmOPM内部按钮响应函数声明
*******************************************************************************/
static void OPMMenuCallBack(int option);

/*******************************************************************************
**			    	窗体FrmOPM中的错误事件处理函数定义部分					  **
*******************************************************************************/
static int OPMErrProc_Func(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//初始化数值（功率值与参考值）
static void InitOpmPara(void);
//清除参数
static void ClearOpmPara(void);
//获取光功率数值（nw）
static OPMPOWERSTATE getOpmPower(int iCurWaveIndex, double *powerValue);
//设置db图片形式的数据
static void SetListBmp(double dBvalue, GUIPICTURE **ppList,OPMNUMBERBMP *pNumberBmpResource);
//设置功率值的单位(传入的参数默认时nw)
static void SetPowerUnit(double power);
//获取功率值（需要切换单位，返回小于1000的浮点数）
static float GetPowerValue(double power);
//刷新波长
static void RefreshWaveDisplay(void);
//刷新参考值内容
static void RefreshRefDisplay(double powerValueTemp, double refValueTemp, int isValid);
//设置无效的显示dbvalue
static void SetNoValidBmp(GUIPICTURE **ppList);

/***
  * 功能：
        窗体frmopm的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmOpmInit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;
    int i = 0;
    //得到当前窗体对象 
    pFrmOPM = (GUIWINDOW *) pWndObj;

    //初始化文本资源必须在建立文本CreateText()或建立标签CreateLabel()之前
    OPMTextRes_Init(NULL, 0, NULL, 0);
    //初始化参数值
    InitOpmPara();
    //线程开启
    iThreadExitFlag = 1;
    //初始化线程锁
    InitMutex(&GStatusMutex, NULL);
    //创建图片
    pFrmOPMBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_opm.bmp");
	pFrmOPMBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_opm_icon.bmp");

    pFrmOPMBgWaveLength = CreatePicture(36, 374, 200, 60, BmpFileDirectory"bg_opm_wavelength.bmp");
    pFrmOPMBgWaveLengthUnit = CreatePicture(156, 375, 79, 58, OPM_WAVE_UNIT);//波长单位

    pFrmOPMBgRefValue = CreatePicture(281, 374, 200, 60,  BmpFileDirectory"bg_opm_refvalue.bmp");
    pFrmOPMBgRefUnit = CreatePicture(446, 370, 58, 44,  BmpFileDirectory"bg_opm_refunit.bmp");
    //图片显示单位
    for (i = 0; i < DB_UNIT_NUM; i++)
    {
        pFrmOPMBgRefUnitList[i] = CreatePicture(402, 375, 78, 58, pFrmOPMBgRefUnitBmp[i]);
    }

    pFrmOPMBgDbValue = CreatePicture(110, 125, 200, 46, BmpFileDirectory"bg_opm_value.bmp"); 
    pFrmOPMBgDbUnit = CreatePicture(375, 140, 40, 20, BmpFileDirectory"bg_opm_dbunit.bmp");
    //图片显示单位
    for (i = 0; i < DB_UNIT_NUM; i++)
    {
        pFrmOPMBgDbUnitList[i] = CreatePicture(388, 118, 47, 18, pFrmOPMBgDbUnitBmp[i]);
    }

    pFrmOPMBgPowerValue = CreatePicture(110, 225, 200, 46, BmpFileDirectory"bg_opm_value.bmp");
    pFrmOPMBgPowerUnit = CreatePicture(375, 225+15, 40, 20, BmpFileDirectory"bg_opm_dbunit.bmp");
    //图片显示功率单位
    for (i = 0; i < POWER_UNIT_NUM; i++)
    {
        pFrmOPMBgPowerUnitList[i] = CreatePicture(388, 236, 47, 18, pFrmOPMBgPowerUnitBmp[i]);
    }
	//创建文本
    pFrmOPMLblTitle = CreateLabel(40, 12, 100, 16, pFrmOPMStrTitle);

    pFrmOPMLblWaveLength= CreateLabel(68, 384, 40, 16, pFrmOPMStrWaveLength);
	pFrmOPMLblWaveLengthUnit = CreateLabel(167, 385, 40, 16, pFrmOPMStrWaveLengthUnit);

    pFrmOPMLblRefValue= CreateLabel(360, 384, 60, 16, pFrmOPMStrRefValue);
    pFrmOPMLblRefUnit= CreateLabel(455, 384, 40, 16, pFrmOPMStrRefUnit);

    pFrmOPMLblDbValue= CreateLabel(110, 125, 60, 16, pFrmOPMStrDbValue);
    pFrmOPMLblDbUnit= CreateLabel(375, 125+15, 40, 16, pFrmOPMStrDbUnit);

    pFrmOPMLblPowerValue= CreateLabel(110, 225, 60, 16, pFrmOPMStrPowerValue);
    pFrmOPMLblPowerUnit= CreateLabel(375, 225+15, 24, 16, pFrmOPMStrPowerUnit);
    
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmOPMLblTitle);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOPMLblWaveLength);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmOPMLblWaveLengthUnit);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOPMLblRefValue);
    SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmOPMLblRefUnit);

    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOPMLblDbValue);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOPMLblDbUnit);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOPMLblPowerValue);
    SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmOPMLblPowerUnit);
    
    //图片显示db、power值
    for (i = 0; i < MAX_NUMBER_BIT_NUM; i++)
    {
        pFrmOPMBgDbList[i] = CreatePicture(70 + 45 * i, 76, 45, 60, pDbNullNumberBmp);
        pFrmOPMBgPowerList[i] = CreatePicture(135 + 33 * i, 209, 33, 53, pNullNumberBmp);
        //参考值数字
        //recordRefValue = 0.00f;
        pFrmOPMBgRefDbList[i] = CreatePicture(282 + 17 * i, 375, 17, 58, pRefNullNumberBmp);
    }
    //图片显示波长
    for (i = 0; i < WAVE_LENGTH_NUM; i++)
    {
        pFrmOPMBgWaveList[i] = CreatePicture(37, 375, 118, 58, pwaveBmp[i]);
    }
    /***************************************************************************
    *                       创建右侧的菜单栏控件
    ***************************************************************************/
    unsigned int strOpmMenu[OPM_FUNCTION_NUM] = {OPM_LBL_WAVELENGTH, OPM_LBL_DBM, OPM_LBL_REF ,OPM_LBL_ZERO_ADJUST};
    pFrmOPMMenu = CreateWndMenu1(OPM_FUNCTION_NUM, sizeof(strOpmMenu), strOpmMenu,  0xff00,
								0, 1, 41, OPMMenuCallBack);
    for (i = 0; i < OPM_FUNCTION_NUM; ++i)
    {
        SetWndMenuItemBg(i, pOPMUnpressBmp[i], pFrmOPMMenu, MENU_UNPRESS);
        SetWndMenuItemBg(i, pOPMSelectBmp[i], pFrmOPMMenu, MENU_SELECT);
    }
    /***************************************************************************
    *           注册窗体(因为所有的按键事件都统一由窗体进行处理)
    ***************************************************************************/
    AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmOPM, pFrmOPM);
    
    /***************************************************************************
    *                       注册右侧菜单栏各个菜单控件
    ***************************************************************************/

	AddWndMenuToComp1(pFrmOPMMenu, pFrmOPM);

    /***************************************************************************
    *                      注册菜单区控件的消息处理
    ***************************************************************************/
	GUIMESSAGE *pMsg = GetCurrMessage();

	LoginWndMenuToMsg1(pFrmOPMMenu, pFrmOPM);
    
    //注册错误消息处理函数
	LoginMessageReg(GUIMESSAGE_ERR_PROC, pFrmOPM, 
                    OPMErrProc_Func, NULL, 0, pMsg);    
    
    return iRet;
}

/***
  * 功能：
        窗体frmvfl的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmOpmExit(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    //临时变量定义
    GUIMESSAGE *pMsg = NULL;
    /***************************************************************************
    *                       清空消息队列中的消息注册项
    ***************************************************************************/
    pMsg = GetCurrMessage();
    ClearMessageReg(pMsg);

    //得到当前窗体对象
    pFrmOPM = (GUIWINDOW *) pWndObj;
    
    /***************************************************************************
    *                       从当前窗体中注销窗体控件
    ***************************************************************************/
    ClearWindowComp(pFrmOPM);

    /***************************************************************************
    *                      销毁桌面上各个区域的背景控件
    ***************************************************************************/
    DestroyPicture(&pFrmOPMBg);
	DestroyPicture(&pFrmOPMBgIcon);

    DestroyPicture(&pFrmOPMBgWaveLength);
    DestroyPicture(&pFrmOPMBgWaveLengthUnit);
    
    DestroyPicture(&pFrmOPMBgRefValue);
    DestroyPicture(&pFrmOPMBgRefUnit);
    
    DestroyPicture(&pFrmOPMBgDbValue);
    DestroyPicture(&pFrmOPMBgDbUnit);
    
    DestroyPicture(&pFrmOPMBgPowerValue);
    DestroyPicture(&pFrmOPMBgPowerUnit);

    DestroyLabel(&pFrmOPMLblTitle); 
    DestroyLabel(&pFrmOPMLblWaveLength);
	DestroyLabel(&pFrmOPMLblWaveLengthUnit);
    
    DestroyLabel(&pFrmOPMLblRefValue);
    DestroyLabel(&pFrmOPMLblRefUnit);
    
    DestroyLabel(&pFrmOPMLblDbValue);
    DestroyLabel(&pFrmOPMLblDbUnit);
    
    DestroyLabel(&pFrmOPMLblPowerValue);
    DestroyLabel(&pFrmOPMLblPowerUnit);

    //销毁显示数字的图片
    int i;
    for (i = 0; i < MAX_NUMBER_BIT_NUM; i++)
    {
        DestroyPicture(&pFrmOPMBgPowerList[i]);
        DestroyPicture(&pFrmOPMBgDbList[i]);
        DestroyPicture(&pFrmOPMBgRefDbList[i]);
    }
    //销毁显示波长的图片
    for (i = 0; i < WAVE_LENGTH_NUM; i++)
    {
        DestroyPicture(&pFrmOPMBgWaveList[i]);
    }
    for (i = 0; i < DB_UNIT_NUM; i++)
    {
        DestroyPicture(&pFrmOPMBgRefUnitList[i]);
        DestroyPicture(&pFrmOPMBgDbUnitList[i]);
    }
    for (i = 0; i < POWER_UNIT_NUM; i++)
    {
        DestroyPicture(&pFrmOPMBgPowerUnitList[i]);
    }
    /***************************************************************************
    *                              菜单区的控件
    ***************************************************************************/
	DestroyWndMenu1(&pFrmOPMMenu);
    
    //释放文本资源
    OPMTextRes_Exit(NULL, 0, NULL, 0);

    iCurrDbUnitIndex= 0;
    iCurrWaveLengthIndex= 0;
    //清除数据
    ClearOpmPara();
    //线程销毁
    iThreadExitFlag = 0;
    CancelReadADPthread();
    MutexDestroy(&GStatusMutex);

    return iRet;
}

/***
  * 功能：
        窗体frmvls的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmPaint(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;

    /***************************************************************************
    *                      显示桌面上各个区域的背景控件
    ***************************************************************************/
	DisplayPicture(pFrmOPMBg);
	// DisplayPicture(pFrmOPMBgIcon);
    DisplayPicture(pFrmOPMBgWaveLength);
    DisplayPicture(pFrmOPMBgWaveLengthUnit);
    
    
    DisplayPicture(pFrmOPMBgRefValue);
    //DisplayPicture(pFrmOPMBgRefUnit);
    DisplayPicture(pFrmOPMBgRefUnitList[iCurrDbUnitIndex]);
    
    
    //DisplayPicture(pFrmOPMBgDbValue);
    DisplayPicture(pFrmOPMBgDbUnitList[iCurrDbUnitIndex]);
    //DisplayPicture(pFrmOPMBgDbUnit);
    //DisplayPicture(pFrmOPMBgPowerUnit);
    DisplayPicture(pFrmOPMBgPowerUnitList[iCurrPowerUnitIndex]);  
    
    // DisplayPicture(pFrmOPMBgPowerValue);
    
	DisplayLabel(pFrmOPMLblTitle);
    DisplayLabel(pFrmOPMLblWaveLength);
	//DisplayLabel(pFrmOPMLblWaveLengthUnit);
    
    //DisplayLabel(pFrmOPMLblRefValue);
    //DisplayLabel(pFrmOPMLblRefUnit);
    
    // DisplayLabel(pFrmOPMLblDbValue);
    //DisplayLabel(pFrmOPMLblDbUnit);
    
    // DisplayLabel(pFrmOPMLblPowerValue);
    //DisplayLabel(pFrmOPMLblPowerUnit);

    int i;
    for (i = 0; i < MAX_NUMBER_BIT_NUM; i++)
    {
        DisplayPicture(pFrmOPMBgPowerList[i]);
        DisplayPicture(pFrmOPMBgDbList[i]);
        DisplayPicture(pFrmOPMBgRefDbList[i]);
    }
    //显示当前波长的图片
    DisplayPicture(pFrmOPMBgWaveList[iCurrWaveLengthIndex]);

	DisplayWndMenu1(pFrmOPMMenu);
    //重新刷新菜单栏
    for (i = 0; i < OPM_FUNCTION_NUM; ++i)
    {
        SetWndMenuItemEnble(i, 1, pFrmOPMMenu);
    }
    
	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);
    //开启读取AD值线程
    if (threadRead == -1)
    {
        iRet = ThreadCreate(&threadRead, NULL, &ReadAdValue, NULL);
        if (iRet == 0)
        {
            LOG(LOG_INFO, "-----create readAD thread success!!---\n");
        }
        else
        {
            LOG(LOG_ERROR, "-----create readAD thread failure\n");
        }
    }
    return iRet;
}

/***
  * 功能：
        窗体frmopm的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmLoop(void *pWndObj)
{
    //错误标志，返回值定义 
    int iRet = 0;
    MutexLock(&GStatusMutex);
    //显示db、power
    int i;
    for (i = 0; i < MAX_NUMBER_BIT_NUM; i++)
    {
        DisplayPicture(pFrmOPMBgPowerList[i]);
        DisplayPicture(pFrmOPMBgDbList[i]);
    }
    //显示单位
    DisplayPicture(pFrmOPMBgPowerUnitList[iCurrPowerUnitIndex]); 
    RefreshScreen(__FILE__, __func__, __LINE__);
    MutexUnlock(&GStatusMutex);

    return iRet;
}

/***
  * 功能：
        窗体frmopm的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmPause(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;
    
    return iRet;
}

/***
  * 功能：
        窗体frmopm的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmResume(void *pWndObj)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

//初始化文本资源
static int OPMTextRes_Init(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iRet = 0;

	pFrmOPMStrTitle = GetCurrLanguageText(MAIN_LBL_OPM);

    pFrmOPMStrWaveLength= TransString(waveLengthArray[iCurrWaveLengthIndex] );
	pFrmOPMStrWaveLengthUnit = TransString("nm");
    pFrmOPMStrRef= TransString("REF");
    pFrmOPMStrRefValue= TransString("0.00");
    pFrmOPMStrRefUnit= TransString("dBm");
    pFrmOPMStrDbValue= TransString("0.00");
    pFrmOPMStrDbUnit= TransString("dBm");
    pFrmOPMStrPowerValue= TransString("0.00");
    pFrmOPMStrPowerUnit= TransString("nW");

    return iRet;
}

//释放文本资源
static int OPMTextRes_Exit(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iRet = 0;
    
    GuiMemFree(pFrmOPMStrTitle);

    GuiMemFree(pFrmOPMStrWaveLength);
	GuiMemFree(pFrmOPMStrWaveLengthUnit);

    GuiMemFree(pFrmOPMStrRef);
    GuiMemFree(pFrmOPMStrRefValue);
    GuiMemFree(pFrmOPMStrRefUnit);
    
    GuiMemFree(pFrmOPMStrDbValue);
    GuiMemFree(pFrmOPMStrDbUnit);
    
    GuiMemFree(pFrmOPMStrPowerValue);
    GuiMemFree(pFrmOPMStrPowerUnit);
    
    return iRet;
}
/*
//右侧菜单栏的回调函数
static void OPMMenuCallBack(int option)
{
    switch(option)
    {
        case 0://波长
            (iCurrWaveLengthIndex<      WAVE_LENGTH_NUM-1) ? (iCurrWaveLengthIndex++) : (iCurrWaveLengthIndex= 0);
            pFrmOPMStrWaveLength= TransString(waveLengthArray[iCurrWaveLengthIndex]);
            TouchChange("bg_opm_wavelength.bmp", pFrmOPMBgWaveLength, pFrmOPMStrWaveLength, pFrmOPMLblWaveLength, 1);
			DisplayLabel(pFrmOPMLblWaveLengthUnit);
			break;
        case 1://dBm
            (iCurrDbUnitIndex== 0) ? (iCurrDbUnitIndex= 1) : (iCurrDbUnitIndex= 0);
            pFrmOPMStrRefUnit= TransString(dBUnitArray[iCurrDbUnitIndex]);
            pFrmOPMStrDbUnit= TransString(dBUnitArray[iCurrDbUnitIndex]);
			TouchChange("bg_opm_refvalue.bmp", pFrmOPMBgRefValue, pFrmOPMStrRefValue, pFrmOPMLblRefValue, 1);
			TouchChange("bg_opm_refunit.bmp", pFrmOPMBgRefUnit, pFrmOPMStrRefUnit, pFrmOPMLblRefUnit, 0);
			TouchChange("bg_opm_dbunit.bmp", pFrmOPMBgDbUnit, pFrmOPMStrDbUnit, pFrmOPMLblDbUnit, 1);
            break;
        case 2://REF
            refValue[iCurrWaveLengthIndex]=  powerValue[iCurrWaveLengthIndex];
            pFrmOPMStrRefValue= UnitConver_Power_Float2GuiString(iCurrDbUnitIndex, UNIT_NW, 
                powerValue[iCurrWaveLengthIndex], refValue[iCurrWaveLengthIndex]);
            TouchChange("bg_opm_refvalue.bmp", pFrmOPMBgRefValue, pFrmOPMStrRefValue, pFrmOPMLblRefValue, 1);
			TouchChange("bg_opm_refunit.bmp", pFrmOPMBgRefUnit, pFrmOPMStrRefUnit, pFrmOPMLblRefUnit, 0);
            break;
        case 3://调零
            refValue[iCurrWaveLengthIndex]=  MIN_REF_VALUE;
            pFrmOPMStrRefValue= UnitConver_Power_Float2GuiString(iCurrDbUnitIndex, UNIT_NW, 
                MIN_REF_VALUE, refValue[iCurrWaveLengthIndex]);
            TouchChange("bg_opm_refvalue.bmp", pFrmOPMBgRefValue, pFrmOPMStrRefValue, pFrmOPMLblRefValue, 1);
			TouchChange("bg_opm_refunit.bmp", pFrmOPMBgRefUnit, pFrmOPMStrRefUnit, pFrmOPMLblRefUnit, 0);
            break;
        case HOME_DOWN:
            ReturnMenuOK();
            break;
        default:
            break;
    }

    RefreshScreen(__FILE__, __func__, __LINE__);
        
}*/
//右侧菜单栏的回调函数
static void OPMMenuCallBack(int option)
{
    switch(option)
    {
        case 0://波长
            isWaveSelected = 1;
            isRefSelected = 0;
            (iCurrWaveLengthIndex<      WAVE_LENGTH_NUM-1) ? (iCurrWaveLengthIndex++) : (iCurrWaveLengthIndex= 0);
            RefreshRefDisplay(0, 0, 0);//保持不变
            RefreshWaveDisplay();
			break;
        case 1://dBm
            //未不到有效的db数据
            if (isdBValueValid == 0)
            {
                break;
            }
            isWaveSelected = 0;
            isRefSelected = 0;
            (iCurrDbUnitIndex== 0) ? (iCurrDbUnitIndex= 1) : (iCurrDbUnitIndex= 0);
            RefreshRefDisplay(0, 0, 0);
            DisplayPicture(pFrmOPMBgDbUnitList[iCurrDbUnitIndex]);
            RefreshWaveDisplay();
            break;
        case 2://REF
            //未不到有效的db数据
            if (isdBValueValid == 0)
            {
                break;
            }
            isWaveSelected = 0;
            isRefSelected = 1;
            refValue[iCurrWaveLengthIndex]=  powerValue[iCurrWaveLengthIndex];
            RefreshRefDisplay(powerValue[iCurrWaveLengthIndex], refValue[iCurrWaveLengthIndex], 1);
            RefreshWaveDisplay();
            break;
        case 3://调零
            //未不到有效的db数据
            if (isdBValueValid == 0)
            {
                break;
            }
            isWaveSelected = 0;
            isRefSelected = 0;
            refValue[iCurrWaveLengthIndex]=  MIN_REF_VALUE;
            RefreshRefDisplay(MIN_REF_VALUE, refValue[iCurrWaveLengthIndex], 1);
            RefreshWaveDisplay();
            break;
        case HOME_DOWN:
            ReturnMenuOK();
            break;
        default:
            break;
    }

    RefreshScreen(__FILE__, __func__, __LINE__);
        
}

static int OPMErrProc_Func(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
    int iRet = 0;

    return iRet;
}

//初始化数值（功率值与参考值）
static void InitOpmPara(void)
{
    //打开opm
    OpenOpmDevice();
    //打开opm电源
    SetPowerSwitch(1);

    int i = 0;
    for(i = 0; i < WAVE_LENGTH_NUM; ++i)
    {
        powerValue[i] = MIN_POWER_VALUE;
        refValue[i] = MIN_REF_VALUE;
    }
}

//清除参数
static void ClearOpmPara(void)
{
    //关闭电源
    SetPowerSwitch(0);
    //关闭描述符
    CloseOpmDevice();
}

//获取光功率数值（nw）
static OPMPOWERSTATE getOpmPower(int iCurWaveIndex, double *powerValue)
{    
    //循环通过0-7通道
    int i = 0;
    static int defaultChannel = 0;
#ifndef OPM_IOCTL
    #define STABLE_DATA_COUNTS 6 //获取稳定数据次数
    int j = 0;
    short dataTemp[STABLE_DATA_COUNTS] = {0}; //用于缓存稳定数据
    memset(dataTemp, 0, STABLE_DATA_COUNTS * sizeof(short));
#endif
    short data[CHANNEL_COUNT] = {0};
    memset(data, 0, CHANNEL_COUNT * sizeof(short));
    unsigned int index = 0;
    OPMPOWERSTATE iState = NORMAL;
    //添加默认通道
    data[defaultChannel] = GetOpmData(CHANNEL_7 - defaultChannel);

    float minValue, maxValue;
    minValue = MIN_AD_VALUE;
	maxValue = MAX_AD_VALUE;
	if (defaultChannel == CHANNEL_6)
	{
		minValue = 2500;
	}
	if (defaultChannel == CHANNEL_7)
	{
		maxValue = 8000;
	}

     //选取第一个符合条件的通道
    if (abs(data[defaultChannel]) >= minValue && abs(data[defaultChannel]) <= maxValue)
    {
        index = defaultChannel;
    }
    else
    {
        for (i = 0; i < CHANNEL_COUNT; i++)
        {
#ifndef OPM_IOCTL
            //因为在切换通道时，前五个数据不稳定
            for (j = 0; j < STABLE_DATA_COUNTS; j++)
            {
                dataTemp[j] = GetOpmData(CHANNEL_7 - (CHANNEL_0 + i));
            }
            data[i] = dataTemp[STABLE_DATA_COUNTS - 1];
#else
            data[i] = GetOpmData(CHANNEL_7 - (CHANNEL_0 + i));
#endif
            if (i == CHANNEL_6)
            {
                minValue = 2500;
            }else
            {
                minValue = MIN_AD_VALUE;
            }
            if (i == CHANNEL_7)
            {
                maxValue = 8000;
            }
            else
            {
                maxValue = MAX_AD_VALUE;
            }
            //选取第一个符合条件的通道
            if (abs(data[i]) >= minValue && abs(data[i]) <= maxValue)
            {
                index = i;
                defaultChannel = i;
                break;
            }
            if (i == 0 && data[i] > MIN_AD_VALUE)
            {
                // iState = TOOHIGHT;
            }
        }
    }
    float k = 0.0f;
    float b = 0.0f;
    //当没有一个通道符合时，给最低值，显示lo
    if (i == CHANNEL_COUNT)
    {
        iState = TOOLOW;
    }
    else
    {
        k = pCalibrationPara->channelPara[iCurWaveIndex].k[index];
        b = pCalibrationPara->channelPara[iCurWaveIndex].b[index];
        *powerValue = k * data[index] + b;
    }
    //修改bug：弱光下，获取的power不能时负数，应为在取log的db值会挂掉
    if (*powerValue < MIN_LOW_POWER_VALUE)
    {
        iState = TOOLOW;
    }
    return iState;
}

//设置图片形式的数据
static void SetListBmp(double dBvalue, GUIPICTURE **ppList ,OPMNUMBERBMP *pNumberBmpResource)
{
    //先刷一遍空白
    int bitIndex;
    for (bitIndex = 0; bitIndex < MAX_NUMBER_BIT_NUM; bitIndex++)
    {
        SetPictureBitmap(pNumberBmpResource->pNullNumber, ppList[bitIndex]);
    }
    //检查参数
    if (!ppList || !(*ppList))
    {
        return;
    }
    //判断数据是否合法
    //TODO
    //先判断正负值,确定第一位的图片
    if (dBvalue < 0)
        SetPictureBitmap(pNumberBmpResource->pNegativeNumber, ppList[0]);
    else
        SetPictureBitmap(pNumberBmpResource->pNullNumber, ppList[0]);
    //第四位（小数点）
    SetPictureBitmap(pNumberBmpResource->pPointNumber, ppList[POINT_BIT_NUMBER]);
    //获取其余各个位上的数值
    int count = 0;
    int* number = NULL;
    number = GetFloatNumbersByCharacters(dBvalue, &count);

    if (number)
    {
        int i;
        int j;
        for (i = count-1; i >= 0; i--)
        {
            j = ((count - 1 - i) < (POINT_BIT_NUMBER-1)) ? (MAX_NUMBER_BIT_NUM - 2 - (count - 1 - i)) 
                                                    : (MAX_NUMBER_BIT_NUM - 3 - (count - 1 - i));
            SetPictureBitmap(pNumberBmpResource->pNumber[number[i]], ppList[j]);
        }
        //考虑到整数位只有一位的情况
        if ((count == POINT_BIT_NUMBER) && (dBvalue < 0))//小数位2位加一位整数位,保留两位有效小鼠
        {
            SetPictureBitmap(pNumberBmpResource->pNullNumber, ppList[0]);
            SetPictureBitmap(pNumberBmpResource->pNegativeNumber, ppList[1]);
        }

        free(number);
        number = NULL;
    }
}

//设置功率值的单位(传入的参数默认时nw)
static void SetPowerUnit(double power)
{
    if (power / pow(10, POINT_BIT_NUMBER) > 1) //μw、mw
    {
        if (power / pow(10, 2 * POINT_BIT_NUMBER) > 1) //mw
            //pFrmOPMStrPowerUnit = TransString("mW");
            iCurrPowerUnitIndex = 0;
        else//μw
            //pFrmOPMStrPowerUnit = TransString("μW");
            iCurrPowerUnitIndex = 1;
    }
    else//nw
        //pFrmOPMStrPowerUnit = TransString("nW");
        iCurrPowerUnitIndex = 2;

    //SetLabelText(pFrmOPMStrPowerUnit, pFrmOPMLblPowerUnit);
}

//设置功率值（需要切换单位，返回小于1000的浮点数）
static float GetPowerValue(double power)
{
    float data = 0.0f;

    if (power / pow(10, POINT_BIT_NUMBER) > 1) //μw、mw
    {
        if (power / pow(10, 2 * POINT_BIT_NUMBER) > 1) //mw
            data = Float2Float(PRECISION_3, power / pow(10, 2 * POINT_BIT_NUMBER));
        else //μw
            data = Float2Float(PRECISION_3, (power / pow(10, POINT_BIT_NUMBER)));
    }
    else //nw
        data = Float2Float(PRECISION_3, power);

    return data;
}

//刷新波长
static void RefreshWaveDisplay(void)
{
    int i = 0;
    //加载选中的图片资源
    if (isWaveSelected)
    {
        for (i = 0; i < WAVE_LENGTH_NUM; i++)
        {
            SetPictureBitmap(pwaveSelectBmp[i],pFrmOPMBgWaveList[i]);
        }
        //单位
        SetPictureBitmap(OPM_WAVE_UNIT_SELECT,pFrmOPMBgWaveLengthUnit);
    }
    else//加载非选中的图片资源
    {
        for (i = 0; i < WAVE_LENGTH_NUM; i++)
        {
             SetPictureBitmap(pwaveBmp[i],pFrmOPMBgWaveList[i]);
        }
        //单位
        SetPictureBitmap(OPM_WAVE_UNIT,pFrmOPMBgWaveLengthUnit);
    }
    DisplayPicture(pFrmOPMBgWaveList[iCurrWaveLengthIndex]);
    DisplayPicture(pFrmOPMBgWaveLengthUnit);
    
}

//刷新参考值
//刷新波长
static void RefreshRefDisplay(double powerValueTemp, double refValueTemp, int isValid)
{
    OPMNUMBERBMP OpmRefNumberBmp;
    int i = 0;
    //波长的图片资源
    //加载选中的图片资源
    if (isRefSelected)
    {
        OpmRefNumberBmp.pNullNumber     = pRefSelectNullNumberBmp;
        OpmRefNumberBmp.pNegativeNumber = pRefSelectNegativeNumberBmp;
        OpmRefNumberBmp.pPointNumber    = pRefSelectPointNumberBmp;
        OpmRefNumberBmp.pNumber         = pRefSelectNumberBmp;
        //单位
        for (i = 0; i < DB_UNIT_NUM; i++)
        {
            SetPictureBitmap(pFrmOPMBgRefSelectUnitBmp[i],pFrmOPMBgRefUnitList[i]);
        }
    }
    else//加载非选中的图片资源
    {
        OpmRefNumberBmp.pNullNumber     = pRefNullNumberBmp;
        OpmRefNumberBmp.pNegativeNumber = pRefNegativeNumberBmp;
        OpmRefNumberBmp.pPointNumber    = pRefPointNumberBmp;
        OpmRefNumberBmp.pNumber         = pRefNumberBmp;
        //单位
        for (i = 0; i < DB_UNIT_NUM; i++)
        {
             SetPictureBitmap(pFrmOPMBgRefUnitBmp[i],pFrmOPMBgRefUnitList[i]);
        }
    }
    if (isValid)
    {
        refDbValue[iCurrWaveLengthIndex] = UnitConverter_Power_P2Db_Float2Float(iCurrDbUnitIndex, UNIT_NW, 
                powerValueTemp, refValueTemp);
    }
    //显示数字图片参考的db值
    SetListBmp(refDbValue[iCurrWaveLengthIndex], pFrmOPMBgRefDbList, &OpmRefNumberBmp);
    //显示参考dbvalue
    for (i = 0; i < MAX_NUMBER_BIT_NUM; i++)
    {
        DisplayPicture(pFrmOPMBgRefDbList[i]);
    }
    //显示参考单位
    DisplayPicture(pFrmOPMBgRefUnitList[iCurrDbUnitIndex]);
}

//设置无效的显示dbvalue
static void SetNoValidBmp(GUIPICTURE **ppList)
{
    //先刷一遍空白
    int bitIndex;
    for (bitIndex = 0; bitIndex < MAX_NUMBER_BIT_NUM; bitIndex++)
    {
        SetPictureBitmap(pDbNullNumberBmp, ppList[bitIndex]);
    }
    SetPictureBitmap(pDbLBmp, ppList[3]);
    SetPictureBitmap(pDbNumberBmp[0], ppList[4]);
}

/***
  * 功能：
        读取AD数值的线程
  * 参数：
        void *pThreadArg: 线程传入的参数
  * 返回：
        空
  * 备注：   
***/
static void *ReadAdValue(void *pThreadArg)
{
    //打印运行时间
    // #define PRINT_TIME
    while (iThreadExitFlag)
    {
        MutexLock(&GStatusMutex);
        OPMPOWERSTATE powerSate = NORMAL;
        double power = 0;
#ifdef PRINT_TIME
        struct timeval tv_start, tv_end;
        gettimeofday(&tv_start, NULL);
#endif
        powerSate = getOpmPower(iCurrWaveLengthIndex, &power); // nw
#ifdef PRINT_TIME
        gettimeofday(&tv_end, NULL);
        printf("time----%ld\n",((tv_end.tv_sec - tv_start.tv_sec)*1000000 + tv_end.tv_usec - tv_start.tv_usec));
#endif
        OPMNUMBERBMP OpmNumberBmp;
        OpmNumberBmp.pNullNumber = pNullNumberBmp;
        OpmNumberBmp.pNegativeNumber = pNegativeNumberBmp;
        OpmNumberBmp.pPointNumber = pPointNumberBmp;
        OpmNumberBmp.pNumber = pNumberBmp;
        if (powerSate == TOOHIGHT || powerSate == TOOLOW) //目前过高或过低都显示lo
        {
            isdBValueValid = 0; // db数据无效
            SetNoValidBmp(pFrmOPMBgDbList);
            SetListBmp(0, pFrmOPMBgPowerList, &OpmNumberBmp);
        }
        else
        {
            LOG(LOG_INFO, "powerVal = %f.\n", power);
            isdBValueValid = 1; // db数据有效
            powerValue[iCurrWaveLengthIndex] = power;
            float dBValue = UnitConverter_Power_P2Db_Float2Float(iCurrDbUnitIndex, UNIT_NW,
                                                                 powerValue[iCurrWaveLengthIndex], refValue[iCurrWaveLengthIndex]);
            OPMNUMBERBMP OpmDbNumberBmp;
            OpmDbNumberBmp.pNullNumber = pDbNullNumberBmp;
            OpmDbNumberBmp.pNegativeNumber = pDbNegativeNumberBmp;
            OpmDbNumberBmp.pPointNumber = pDbPointNumberBmp;
            OpmDbNumberBmp.pNumber = pDbNumberBmp;
            //显示数字图片类型的db、power
            SetListBmp(dBValue, pFrmOPMBgDbList, &OpmDbNumberBmp);
            SetListBmp(GetPowerValue(power), pFrmOPMBgPowerList, &OpmNumberBmp);
            SetPowerUnit(power);
        }
        MutexUnlock(&GStatusMutex);
    }

    return NULL;
}

/***
  * 功能：
        回收读取AD值线程
  * 参数：
        空
  * 返回：
        空
  * 备注：
***/
static void CancelReadADPthread()
{
    if (threadRead != -1)
    {
        ThreadCancel(threadRead);
        ThreadJoin(threadRead, NULL);
        threadRead = -1;
    }
}