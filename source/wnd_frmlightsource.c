/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlightsource.c
* 摘    要：  声明初始化窗体wnd_frmlightsource的窗体处理线程及相关操作函数
*
* 当前版本：
* 作    者：  wjg
* 完成日期：
*******************************************************************************/

#include "wnd_frmlightsource.h"

#include "app_frminit.h"
#include "app_frmsourcelight.h"
#include "app_systemsettings.h"

#include "guiwindow.h"
#include "guipicture.h"
#include "guilabel.h"

#include "wnd_global.h"
#include "wnd_frmmenubak.h"

//稳定光源的结构体
extern SOURCELIGHTCONFIG* pSourceLightConfig;
extern LIGHT_SOURCE_POWER_CONFIG_LIST *pLightSourcePowerConfig;
static int iStatusSwitch = 0;				//稳定光源的开启/关闭（0：close，1：open）
/*******************************************************************************
**							窗体FrmLightSource中的控件定义部分				**
*******************************************************************************/
static GUIWINDOW *pFrmLightSource = NULL;
//背景
static GUIPICTURE* pFrmLightSourceBg = NULL;
//标题栏
static GUIPICTURE* pFrmLightSourceBgIcon = NULL;
static GUIPICTURE* pFrmLightSourceBgStatus = NULL;
static GUIPICTURE* pFrmLightSourceBgWave = NULL;
static GUIPICTURE* pFrmLightSourceBgFreq = NULL;
//文本
static GUILABEL* pFrmLightSourceLblTitle = NULL;
static GUILABEL* pFrmLightSourceLblWave = NULL;
static GUILABEL* pFrmLightSourceLblWaveValue = NULL;
static GUILABEL* pFrmLightSourceLblFrequence = NULL;
static GUILABEL* pFrmLightSourceLblFrequenceValue = NULL;
static GUILABEL* pFrmLightSourceLblWarning = NULL;

static GUICHAR* pFrmLightSourceStrTitle = NULL;
static GUICHAR* pFrmLightSourceStrWave = NULL;
static GUICHAR* pFrmLightSourceStrWaveValue = NULL;
static GUICHAR* pFrmLightSourceStrFrequence = NULL;
static GUICHAR* pFrmLightSourceStrFrequenceValue = NULL;
static GUICHAR* pFrmLightSourceStrWarning = NULL;

/*******************************************************************************
*               窗体FrmLightSource右侧菜单的控件资源
*******************************************************************************/
static WNDMENU1	*pFrmLightSourceMenu = NULL;
#define LIGHT_SOURCE_MODE_NUM			4								//START,CLOSE,WAVE,FREQUENCE
#define FREQUENCE_NUM					3								//CW,1KHZ,2KHZ

/*******************************************************************************
*               窗体FrmOPM功能全局变量声明
*******************************************************************************/
#define LIGHTSOURCE_WAVE_NUM           	2
#define LIGHTSOURCE_HZ_NUM           	3

static unsigned int iCurrLightSourceWaveIndex = 0;    		//记录当前显示波长的索引值，最大值为(LIGHTSOURCE_WAVE_NUM-1)
static unsigned int iCurrHZIndex = 0;               		//记录当前频率的索引值(LIGHTSOURCE_HZ_NUM - 1)

//波长图片
static GUIPICTURE *pFrmLightSourceBgWaveList[LIGHTSOURCE_WAVE_NUM] = {NULL}; //例如：波长

static char *pLightSourceWaveBmp[LIGHTSOURCE_WAVE_NUM] =
{
    BmpFileDirectory "bg_LightSource_1310_wave.bmp",
    BmpFileDirectory "bg_LightSource_1550_wave.bmp"
};

//波长图片
static GUIPICTURE *pFrmLightSourceBgHZList[LIGHTSOURCE_HZ_NUM] = {NULL}; //例如：频率

static char *pLightSourceHZBmp[LIGHTSOURCE_HZ_NUM] =
{
    BmpFileDirectory "bg_LightSource_CW_HZ.bmp",
    BmpFileDirectory "bg_LightSource_1K_HZ.bmp",
	BmpFileDirectory "bg_LightSource_2K_HZ.bmp"
};

//各项非选中背景资源
static char *pLightSourceUnpressBmp[LIGHT_SOURCE_MODE_NUM] =
{
    BmpFileDirectory "bg_LightSource_start.bmp",
    BmpFileDirectory "bg_LightSource_close.bmp",
    BmpFileDirectory "bg_LightSource_wave1.bmp",
    BmpFileDirectory "bg_LightSource_hz.bmp"
};

//各项选中背景资源
static char *pLightSourceSelectBmp[LIGHT_SOURCE_MODE_NUM] =
{
    BmpFileDirectory "bg_LightSource_startSelect.bmp",
    BmpFileDirectory "bg_LightSource_closeSelect.bmp",
    BmpFileDirectory "bg_LightSource_waveSelect.bmp",
    BmpFileDirectory "bg_LightSource_hzSelect.bmp"
};

/*******************************************************************************
*                   窗体FrmLightSource内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int LightSourceTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);
//释放文本资源
static int LightSourceTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen);

/*******************************************************************************
*                   窗体FrmLightSource回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void LightSourceMenuCallBack(int iSelected);
/*******************************************************************************
*                   窗体FrmLightSource内部参数相关功能函数声明
*******************************************************************************/
//设置波长
static void SetWavelengthString(int iWave);
//设置频率
static void SetFrequenceString(int iFrequence);
#ifdef OPTIC_DEVICE
//设置稳定光源的波长值
static void SetSourceLightWavelength(int wave);
//设置稳定光源的频率值
static void SetSourceLightFrequence(int frq);
//设置稳定光源的脉宽值
// static void SetSourceLightPulse(unsigned int uiPulse);
//稳定光源打开/关闭
static void SourcelightPowerSwitch(int iSwitch);
#endif

//窗体FrmLightSource的初始化函数，建立窗体控件、注册消息处理
int FrmLightSourceInit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//得到当前窗体对象
	pFrmLightSource = (GUIWINDOW *)pWndObj;
	//初始化文本
	LightSourceTextRes_Init(NULL, 0, NULL, 0);
	//标题栏
	pFrmLightSourceBg = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BmpFileDirectory"bg_lightSource.bmp");
	pFrmLightSourceBgIcon = CreatePicture(0, 0, 23, 20, BmpFileDirectory"bg_lightsource_icon.bmp");
	pFrmLightSourceBgStatus = CreatePicture(27, 86, 336, 159, BmpFileDirectory"bg_lightsource_status_close.bmp");

	pFrmLightSourceLblTitle = CreateLabel(40, 12, 100, 16, pFrmLightSourceStrTitle);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pFrmLightSourceLblTitle);
	pFrmLightSourceLblWarning = CreateLabel(252, 415, 200, 16, pFrmLightSourceStrWarning);
	SetLabelFont(getGlobalFnt(EN_FONT_RED), pFrmLightSourceLblWarning);

	//波长和频率
	pFrmLightSourceBgWave = CreatePicture(390, 142, 100, 24, BmpFileDirectory"bg_lightsource_wave.bmp");
	pFrmLightSourceLblWave = CreateLabel(390, 142+4, 100, 16, pFrmLightSourceStrWave);
	pFrmLightSourceLblWaveValue = CreateLabel(390, 142+4, 100, 16, pFrmLightSourceStrWaveValue);
	pFrmLightSourceBgFreq = CreatePicture(232, 296, 100, 24, BmpFileDirectory"bg_lightsource_freq.bmp");
	pFrmLightSourceLblFrequence = CreateLabel(45, 322, 100, 16, pFrmLightSourceStrFrequence);
	pFrmLightSourceLblFrequenceValue = CreateLabel(232, 296+4, 100, 16, pFrmLightSourceStrFrequenceValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmLightSourceLblWaveValue);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmLightSourceLblFrequence);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pFrmLightSourceLblFrequenceValue);
	//图片显示波长
	int i = 0;
	for(i = 0; i < LIGHTSOURCE_WAVE_NUM; i++)
	{
		pFrmLightSourceBgWaveList[i] = CreatePicture(394, 154, 82, 17, pLightSourceWaveBmp[i]);
	}
	for(i = 0; i < LIGHTSOURCE_HZ_NUM; i++)
	{
		pFrmLightSourceBgHZList[i] = CreatePicture(245, 322, 72, 15, pLightSourceHZBmp[i]);
	}
	//图片显示频率

	/***************************************************************************
	*                       创建右侧的菜单栏控件
	***************************************************************************/
	unsigned int strLightSourceMenu[LIGHT_SOURCE_MODE_NUM] = 
	{
		LIGHT_SOURCE_LBL_START,
		LIGHT_SOURCE_LBL_CLOSE, 
		LIGHT_SOURCE_LBL_WAVE,
		LIGHT_SOURCE_LBL_HZ 
	};
	pFrmLightSourceMenu = CreateWndMenu1(LIGHT_SOURCE_MODE_NUM, sizeof(strLightSourceMenu),
										 strLightSourceMenu, 0xff00, 0, 1, 41, LightSourceMenuCallBack);
	//设置菜单栏背景
	for (i = 0; i < LIGHT_SOURCE_MODE_NUM; ++i)
	{
		SetWndMenuItemBg(i, pLightSourceUnpressBmp[i], pFrmLightSourceMenu, MENU_UNPRESS);
		SetWndMenuItemBg(i, pLightSourceSelectBmp[i], pFrmLightSourceMenu, MENU_SELECT);
	}
	//注册窗体(因为所有的按键事件都统一由窗体进行处理)
	AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pFrmLightSource, pFrmLightSource);
	/***************************************************************************
	*                       注册右侧菜单栏各个菜单控件
	***************************************************************************/
	AddWndMenuToComp1(pFrmLightSourceMenu, pFrmLightSource);
	/***************************************************************************
	*                      注册菜单区控件的消息处理
	***************************************************************************/
	LoginWndMenuToMsg1(pFrmLightSourceMenu, pFrmLightSource);

	return iReturn;
}
//窗体FrmLightSource的退出函数，释放所有资源
int FrmLightSourceExit(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
	int i = 0;

	//得到当前窗体对象
	pFrmLightSource = (GUIWINDOW*)pWndObj;
	//清空消息队列中的消息注册项
	GUIMESSAGE* pMsg = GetCurrMessage();
	ClearMessageReg(pMsg);
	//从当前窗体中注销窗体控件
	ClearWindowComp(pFrmLightSource);
	//销毁控件
	DestroyPicture(&pFrmLightSourceBg);
	DestroyPicture(&pFrmLightSourceBgIcon);
	DestroyPicture(&pFrmLightSourceBgStatus);
	DestroyPicture(&pFrmLightSourceBgWave);
	DestroyPicture(&pFrmLightSourceBgFreq);
	//销毁标题栏文本
	DestroyLabel(&pFrmLightSourceLblTitle);
	DestroyLabel(&pFrmLightSourceLblWave);
	DestroyLabel(&pFrmLightSourceLblWaveValue);
	DestroyLabel(&pFrmLightSourceLblFrequence);
	DestroyLabel(&pFrmLightSourceLblFrequenceValue);
	DestroyLabel(&pFrmLightSourceLblWarning);

	//销毁图片资源
	for(i = 0; i < LIGHTSOURCE_WAVE_NUM; i++)
	{
		DestroyPicture(&pFrmLightSourceBgWaveList[i]);
	}
	for(i = 0; i < LIGHTSOURCE_HZ_NUM; i++)
	{
		DestroyPicture(&pFrmLightSourceBgHZList[i]);
	}
	/***************************************************************************
	*                              菜单区的控件
	***************************************************************************/
	DestroyWndMenu1(&pFrmLightSourceMenu);
	//销毁文本
	LightSourceTextRes_Exit(NULL, 0, NULL, 0);
	//保存参数
#ifdef EEPROM_DATA
	SetSettingsData((void*)pSourceLightConfig, sizeof(SOURCELIGHTCONFIG), LIGHTSOURCE);
	SaveSettings(LIGHTSOURCE);
#endif
#ifdef OPTIC_DEVICE
	//关闭稳定光源
	SourcelightPowerSwitch(0);
#endif
	return iReturn;
}
//窗体FrmLightSource的绘制函数，绘制整个窗体
int FrmLightSourcePaint(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	//显示控件
	DisplayPicture(pFrmLightSourceBg);
	// DisplayPicture(pFrmLightSourceBgIcon);
	DisplayPicture(pFrmLightSourceBgStatus);
	//DisplayPicture(pFrmLightSourceBgWave);
	//DisplayPicture(pFrmLightSourceBgFreq);
	//设置波长和频率值
	SetWavelengthString(pSourceLightConfig->wavelength);
	SetFrequenceString(pSourceLightConfig->frequence);

	//显示波长和频率
	DisplayPicture(pFrmLightSourceBgWaveList[iCurrLightSourceWaveIndex]);
	DisplayPicture(pFrmLightSourceBgHZList[iCurrHZIndex]);

	//显示文本
	DisplayLabel(pFrmLightSourceLblTitle);
	//DisplayLabel(pFrmLightSourceLblWave);
	//DisplayLabel(pFrmLightSourceLblWaveValue);
	DisplayLabel(pFrmLightSourceLblFrequence);
	//DisplayLabel(pFrmLightSourceLblFrequenceValue);
	DisplayLabel(pFrmLightSourceLblWarning);
	//显示菜单栏
	DisplayWndMenu1(pFrmLightSourceMenu);
	//刷新菜单栏
	int i;
	for (i = 0; i < LIGHT_SOURCE_MODE_NUM; ++i)
	{
		SetWndMenuItemEnble(i, 1, pFrmLightSourceMenu);
	}

	SetPowerEnable(1, 1);
	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}
//窗体FrmLightSource的循环函数，进行窗体循环
int FrmLightSourceLoop(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;
#if 1
	//禁止并停止窗体循环
	SendWndMsg_LoopDisable(pWndObj);
#else//测试使用
	//预防测试过程中总电源突然被关闭的现象，需要重新上电
	if (iStatusSwitch && !Opm_OpenPwr(pOtdrTopSettings->pOpmSet))
	{
		LOG(LOG_ERROR, "PWR is down!!!\n");
		OpenOpmPwr(pOtdrTopSettings->pOpmSet);
	}
	MsecSleep(300);
#endif

	return iReturn;
}
//窗体FrmLightSource的挂起函数，进行窗体挂起前预处理
int FrmLightSourcePause(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}
//窗体FrmLightSource的恢复函数，进行窗体恢复前预处理
int FrmLightSourceResume(void *pWndObj)
{
	//错误标志、返回值定义
	int iReturn = 0;

	return iReturn;
}

/*******************************************************************************
*                   窗体FrmLightSource内部文本操作函数声明
*******************************************************************************/
//初始化文本资源
static int LightSourceTextRes_Init(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	pFrmLightSourceStrTitle = GetCurrLanguageText(MAIN_LBL_LIGHT_SOURCE);
	pFrmLightSourceStrWave = TransString("Wave:");
	pFrmLightSourceStrWaveValue = TransString("1310 nm");
	pFrmLightSourceStrFrequence = GetCurrLanguageText(LIGHT_SOURCE_LBL_FREQUENCE);
	pFrmLightSourceStrFrequenceValue = TransString("CW");
	pFrmLightSourceStrWarning = GetCurrLanguageText(VFL_LBL_WARNING);

	return iReturn;
}
//释放文本资源
static int LightSourceTextRes_Exit(void *pInArg, int iInLen,
	void *pOutArg, int iOutLen)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GuiMemFree(pFrmLightSourceStrTitle);
	GuiMemFree(pFrmLightSourceStrWave);
	GuiMemFree(pFrmLightSourceStrWaveValue);
	GuiMemFree(pFrmLightSourceStrFrequence);
	GuiMemFree(pFrmLightSourceStrFrequenceValue);
	GuiMemFree(pFrmLightSourceStrWarning);

	return iReturn;
}

/*******************************************************************************
*                   窗体FrmLightSource回调函数声明
*******************************************************************************/
//菜单栏回调函数
static void LightSourceMenuCallBack(int iSelected)
{
	switch (iSelected)
	{
	case 0://开始
		iStatusSwitch = 1;
		SetPictureBitmap(BmpFileDirectory"bg_lightsource_status_open.bmp", pFrmLightSourceBgStatus);
		break;
	case 1://关闭
		if (iStatusSwitch)
		{
			iStatusSwitch = 0;
			//pSourceLightConfig->frequence = -1;
		}
		SetPictureBitmap(BmpFileDirectory"bg_lightsource_status_close.bmp", pFrmLightSourceBgStatus);
		break;
	case 2://波长
		if (iStatusSwitch)
		{
			pSourceLightConfig->wavelength = pSourceLightConfig->wavelength ? 0 : 1;
		}
		break;
	case 3://频率

		if (iStatusSwitch)
		{
			(pSourceLightConfig->frequence < FREQUENCE_NUM - 1) ? (pSourceLightConfig->frequence++) 
															: (pSourceLightConfig->frequence = 0);
		}
		break;
	case BACK_DOWN:
	case HOME_DOWN:
		ReturnMenuOK();
		return;
	default:
		break;
	}
	//稳定光源打开才会设置
	if (iStatusSwitch)
	{
		SetWavelengthString(pSourceLightConfig->wavelength);
		SetFrequenceString(pSourceLightConfig->frequence);
		//刷新波长和频率值
		DisplayPicture(pFrmLightSourceBgWaveList[iCurrLightSourceWaveIndex]);
		DisplayPicture(pFrmLightSourceBgHZList[iCurrHZIndex]);
		DisplayLabel(pFrmLightSourceLblFrequence);
		//DisplayPicture(pFrmLightSourceBgWave);
		//DisplayLabel(pFrmLightSourceLblWave);
		//DisplayLabel(pFrmLightSourceLblWaveValue);
		//DisplayPicture(pFrmLightSourceBgFreq);
		//DisplayLabel(pFrmLightSourceLblFrequenceValue);
	}
#ifdef OPTIC_DEVICE
	//打开/关闭稳定光源
	SourcelightPowerSwitch(iStatusSwitch);
#endif

	DisplayPicture(pFrmLightSourceBgStatus);

	RefreshScreen(__FILE__, __func__, __LINE__);

}

/*******************************************************************************
*                   窗体FrmLightSource内部参数相关功能函数声明
*******************************************************************************/
//设置波长
static void SetWavelengthString(int iWave)
{
	switch (iWave)
	{
	case ENUM_WAVE_1310NM:
		iCurrLightSourceWaveIndex = 0;
		//pFrmLightSourceStrWaveValue = TransString("1310 nm");
		break;
	case ENUM_WAVE_1550NM:
		iCurrLightSourceWaveIndex = 1;
		//pFrmLightSourceStrWaveValue = TransString("1550 nm");
		break;
	default:
		break;
	}

	//SetLabelText(pFrmLightSourceStrWaveValue, pFrmLightSourceLblWaveValue);
}

//设置频率
static void SetFrequenceString(int iFrequence)
{
	switch (iFrequence)
	{
	case SOURCELIGHTMODECW:
		iCurrHZIndex = 0;
		//pFrmLightSourceStrFrequenceValue = TransString("CW");
		break;
	case SOURCELIGHTMODE1KHZ:
		iCurrHZIndex = 1;
		//pFrmLightSourceStrFrequenceValue = TransString("1KHz");
		break;
	case SOURCELIGHTMODE2KHZ:
		iCurrHZIndex = 2;
		//pFrmLightSourceStrFrequenceValue = TransString("2KHz");
		break;
	default:
		break;
	}

	//SetLabelText(pFrmLightSourceStrFrequenceValue, pFrmLightSourceLblFrequenceValue);
}
#ifdef OPTIC_DEVICE
//设置稳定光源的波长值
static void SetSourceLightWavelength(int wave)
{
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;

	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;

	if (SOURCELIGHTWAVELENTH1310 == wave)
	{
		Opm_SetWave(iOpmFd, pOtdrTopSet->pOpmSet, ENUM_WAVE_1310NM);
	}
	else
	{
		Opm_SetWave(iOpmFd, pOtdrTopSet->pOpmSet, ENUM_WAVE_1550NM);
	}
}

//设置稳定光源的频率值
static void SetSourceLightFrequence(int frq)
{
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;

	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;

	unsigned int mode;
	UINT32 uiTemp = Opm_GetDaqMode(iOpmFd, pOtdrTopSet->pOpmSet);
	unsigned int uiMode = uiTemp;

	if(frq < 0)//关闭稳定光源
		mode = 0;
	else //cw,1khz,2khz
		mode = 2 * frq + 1;
	uiMode = (uiMode & ~SOURCELIGHT_MASK) | (mode << SOURCELIGHT_SHIFT);
	LOG(LOG_INFO, "sourcelight power uiMode 0x%x\n", uiMode);

	Opm_WriteReg(iOpmFd, FPGA_DAQMODE_SET, &uiMode);

	uiTemp = Opm_GetDaqMode(iOpmFd, pOtdrTopSet->pOpmSet);
	LOG(LOG_INFO, "FPGA_DAQMODE_SET = 0x%x\n", uiTemp);
}

//设置稳定光源的脉宽值
// static void SetSourceLightPulse(unsigned int uiPulse)
// {
// 	DEVFD *pDevFd = GetGlb_DevFd();
// 	int iOpmFd = pDevFd->iOpticDev;
// 	Opm_WriteReg(iOpmFd, FPGA_LD_PULSE_WIDTH, &uiPulse);
// }

//稳定光源打开/关闭
static void SourcelightPowerSwitch(int iSwitch)
{
	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;
	//关闭稳定光源
	if (iSwitch == 0)
	{
		//关闭
		SetSourceLightFrequence(-1);
		//关闭电源
		Opm_LdPulseTurnOff(iOpmFd, pOtdrTopSet->pOpmSet);
		Opm_DisableAdc(iOpmFd, pOtdrTopSet->pOpmSet);
		Opm_DisablePwrCtl(iOpmFd, pOtdrTopSet->pOpmSet, _C_5V_OPM_CTL);
		Opm_DisablePwrCtl(iOpmFd, pOtdrTopSet->pOpmSet, C_5V_OPM_CTL);
		//Opm_DisablePwr(iOpmFd, pOtdrTopSet->pOpmSet);
		//检测光模块电源是否关闭，若未关闭，需关闭
		if (Opm_OpenPwr(pOtdrTopSet->pOpmSet))
			CloseOpmPwr(pOtdrTopSet->pOpmSet);
		return;
	}

	Opm_LdPulseTurnOn(iOpmFd, pOtdrTopSet->pOpmSet);
	Opm_EnableAdc(iOpmFd, pOtdrTopSet->pOpmSet);

	Opm_EnablePwrCtl(iOpmFd, pOtdrTopSet->pOpmSet, _C_5V_OPM_CTL);
	Opm_EnablePwrCtl(iOpmFd, pOtdrTopSet->pOpmSet, C_5V_OPM_CTL);

	Opm_SetVddVoltage(iOpmFd, pOtdrTopSet->pOpmSet, 0x0600);

	//设置波长和频率
	SetSourceLightWavelength(pSourceLightConfig->wavelength);
	SetSourceLightFrequence(pSourceLightConfig->frequence);
	//检测光模块是否打开，若未打开，需打开
	if (!Opm_OpenPwr(pOtdrTopSet->pOpmSet))
	{
		OpenOpmPwr(pOtdrTopSet->pOpmSet);
	}

#if 0
	unsigned int uiFpgaCtrlOut = 0;
	Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &(uiFpgaCtrlOut));
	LOG(LOG_INFO, "FPGA_CTRLOUT_REG = 0x%x---\n", uiFpgaCtrlOut);
	Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaCtrlOut));
	LOG(LOG_INFO, "FPGA_TRIG_CTRL = 0x%x---\n", uiFpgaCtrlOut);
	Opm_ReadReg(iOpmFd, OPM_CTRLOUT_REG, &(uiFpgaCtrlOut));
	LOG(LOG_INFO, "OPM_CTRLOUT_REG = 0x%x---\n", uiFpgaCtrlOut);
#endif
}

#endif