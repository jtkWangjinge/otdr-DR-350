/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcurvepara.h
* 摘    要：  实现主窗体FrmCurvePara的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/19
*
*******************************************************************************/

#include "wnd_frmcurvepara.h"

#include "app_frminit.h"
#include "app_frmotdr.h"
#include "app_unitconverter.h"

#include "wnd_global.h"

static PUSER_SETTINGS pUser_Setting = NULL;
static int iCurrentWaveIndex = 0;
static MARKER mMarker;									//存储标记线配置信息
static PDISPLAY_INFO pDisplay = NULL;					//存储曲线配置信息
OTDR_CURVE_PARA* gOtdrCurveParaObj = NULL;				//窗体控件
static int isInCurrWindow = 0;							//是否在当前窗体,默认不在当前窗体

extern POTDR_TOP_SETTINGS pOtdrTopSettings;				//设置顶层结构

//设置测试配置信息
static void SetTestParaInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj);
//设置测试模式信息
static void SetTestModeInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj);
//设置脉宽配置信息
static void SetTestPulseTimeInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj);
//设置测试时长配置信息
static void SetTestAverageTimeInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj);
//设置测试量程的配置信息
static void SetTestRangeInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj);
//显示测量配置信息
static void DisplayTestParaInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj);

//设置AB标记线的测试信息
static int SetMarkInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj);
//设置标记线标题文本
static void SetMarkTitleInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj);
//显示标记线文本(包括标题以及信息)
static void DisplayMarkInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj);

/***
  * 功能：
		创建一个otdr界面底部参数设置界面
  * 参数：
		无
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
OTDR_CURVE_PARA *CreateOtdrCurvePara(void)
{
	//错误标志定义
	int iErr = 0;
	//初始化资源
	pDisplay = pOtdrTopSettings->pDisplayInfo;
	pUser_Setting = pOtdrTopSettings->pUser_Setting;
	iCurrentWaveIndex = pUser_Settings->enWaveCurPos;
	//临时变量定义
	OTDR_CURVE_PARA *pOtdrCurveParaObj = NULL;

	if (!iErr)
	{	//分配资源
		pOtdrCurveParaObj = (OTDR_CURVE_PARA *)calloc(1, sizeof(OTDR_CURVE_PARA));
		if (NULL == pOtdrCurveParaObj)
		{
			LOG(LOG_ERROR, "NULL == pOtdrCurveParaObj");
			iErr = -1;
		}
	}

	if (!iErr)
	{
		int i = 0;
		int j = 0;
		pOtdrCurveParaObj->iFocus = 0;
		pOtdrCurveParaObj->pBg = CreatePicture(0, 378, 640, 103, BmpFileDirectory"bg_otdrCurve.bmp");

		for (i = 0; i < CURVE_TITLE_NUM; ++i)
		{
			pOtdrCurveParaObj->pStrTitle[i] = TransString(" ");
			pOtdrCurveParaObj->pLblTitle[i] = CreateLabel(28+175*i, 384, 162, 16, pOtdrCurveParaObj->pStrTitle[i]);
		}

		SetMarkTitleInfo(pOtdrCurveParaObj);
		
		for (i = 0; i < 2; ++i)
		{
			for (j = 0; j < 3; ++j)
			{
				if (j < 1)
				{
					pOtdrCurveParaObj->pBtn[i][j] = CreatePicture(60, 408 + i * 36, 150, 35,
																  BmpFileDirectory "bg_otdrCurve_dist_unpress.bmp");

					pOtdrCurveParaObj->pLblDataTitle[i][j] = CreateLabel(25, 418 + i * 36, 35, 16, 
													pOtdrCurveParaObj->pStrDataTitle[i][j]);
					pOtdrCurveParaObj->pLblDataContent[i][j] = CreateLabel(72, 418 + i * 36, 100, 16, 
													pOtdrCurveParaObj->pStrDataContent[i][j]);
					SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrCurveParaObj->pLblDataTitle[i][j]);
				}
				else
				{
					pOtdrCurveParaObj->pBtn[i][j] = CreatePicture(307 + 149 * (j - 1), 408 + i * 36, 180, 35,
																  BmpFileDirectory "bg_otdrCurve_loss_unpress.bmp");
					if (j < 2)
					{
						pOtdrCurveParaObj->pLblDataTitle[i][j] = CreateLabel(247, 418 + i * 36, 35, 16,
																			 pOtdrCurveParaObj->pStrDataTitle[i][j]);
						SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrCurveParaObj->pLblDataTitle[i][j]);
					}
						
					pOtdrCurveParaObj->pLblDataContent[i][j] = CreateLabel(320 + 149 * (j - 1), 418 + i * 36, 100, 24,
																		   pOtdrCurveParaObj->pStrDataContent[i][j]);
				}

				pOtdrCurveParaObj->pStrDataContent[i][j] = TransString(" ");
				SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrCurveParaObj->pLblDataContent[i][j]);
			}
		}
	}

	gOtdrCurveParaObj = pOtdrCurveParaObj;

	return pOtdrCurveParaObj;
}

/***
  * 功能：
		销毁OtdrCurvePara控件
  * 参数：
		1.OTDR_CURVE_PARA **pOtdrCurveParaObj : 指向OtdrCurvePara控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroyOtdrCurvePara(OTDR_CURVE_PARA **pOtdrCurveParaObj)
{
	int i = 0;
	int j = 0;
	//参数检测
	if (NULL == *pOtdrCurveParaObj)
	{
		return -1;
	}

	DestroyPicture(&((*pOtdrCurveParaObj)->pBg));

	for (i = 0; i < CURVE_TITLE_NUM; ++i)
	{
		DestroyLabel(&((*pOtdrCurveParaObj)->pLblTitle[i]));
		GuiMemFree((*pOtdrCurveParaObj)->pStrTitle[i]);
	}

	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			if (j < 2)
			{
				DestroyLabel(&((*pOtdrCurveParaObj)->pLblDataTitle[i][j]));
				GuiMemFree((*pOtdrCurveParaObj)->pStrDataTitle[i][j]);
			}

			DestroyPicture(&((*pOtdrCurveParaObj)->pBtn[i][j]));
			DestroyLabel(&((*pOtdrCurveParaObj)->pLblDataContent[i][j]));
			GuiMemFree((*pOtdrCurveParaObj)->pStrDataContent[i][j]);
		}
	}

	//释放空间
	free(*pOtdrCurveParaObj);
	*pOtdrCurveParaObj = NULL;

	return 0;
}

/***
  * 功能：
		显示OtdrCurvePara控件
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplayOtdrCurvePara(OTDR_CURVE_PARA *pOtdrCurveParaObj)
{
	int i = 0;
	int j = 0;

	if (NULL == pOtdrCurveParaObj)
	{
		return -1;
	}

	DisplayPicture(pOtdrCurveParaObj->pBg);

	for (i = 0; i < CURVE_TITLE_NUM; ++i)
	{
		DisplayLabel(pOtdrCurveParaObj->pLblTitle[i]);
	}

	SetPictureBitmap(BmpFileDirectory "bg_otdrCurve_dist_press.bmp",
					 pOtdrCurveParaObj->pBtn[0][pOtdrCurveParaObj->iFocus]);

	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			DisplayPicture(pOtdrCurveParaObj->pBtn[i][j]);
			if (j < 2)
			{
				DisplayLabel(pOtdrCurveParaObj->pLblDataTitle[i][j]);
			}
			DisplayLabel(pOtdrCurveParaObj->pLblDataContent[i][j]);

		}
	}

	return 0;
}

/***
  * 功能：
		设置OtdrCurvePara控件信息显示
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
		2.int iEnabled:0,不使能，1，使能（显示参数信息）
		3., GUIWINDOW *pWnd:注册到窗体上
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int SetOtdrCurveParaEnable(OTDR_CURVE_PARA *pOtdrCurveParaObj, int iEnabled, GUIWINDOW *pWnd)
{
	//检查参数
	if (NULL == pOtdrCurveParaObj || iEnabled > 1 || iEnabled < 0)
	{
		return -1;
	}

	//设置使能状态
	if (pOtdrCurveParaObj->iEnabled != iEnabled)
	{
		pOtdrCurveParaObj->iEnabled = iEnabled;
	}

	return 0;
}

/***
  * 功能：
		刷新OtdrCurvePara控件
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int RefreshOtdrCurvePara(OTDR_CURVE_PARA *pOtdrCurveParaObj)
{
	//测量配置信息
	SetTestParaInfo(pOtdrCurveParaObj);
 	//测量标记线信息
 	SetMarkInfo(pOtdrCurveParaObj);	
	//显示测量配置信息
	DisplayTestParaInfo(pOtdrCurveParaObj);
	//显示标记线信息
	DisplayMarkInfo(pOtdrCurveParaObj);

	return 0;
}

/***
  * 功能：
		设置测试配置信息
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static void SetTestParaInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj)
{	
	//1.测量模式
	SetTestModeInfo(pOtdrCurveParaObj);
	//2.脉宽 pUser_Setting->sFrontSetting[i].enPulseTime
	SetTestPulseTimeInfo(pOtdrCurveParaObj);
	//3.测试时长
	SetTestAverageTimeInfo(pOtdrCurveParaObj);
	//4.测试量程
	SetTestRangeInfo(pOtdrCurveParaObj);
}

/***
  * 功能：
		设置测试模式信息
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static void SetTestModeInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj)
{
	if (pUser_Setting->sFrontSetting[iCurrentWaveIndex].enTestMode == 0)
	{
		pOtdrCurveParaObj->pStrTitle[0] = GetCurrLanguageText(OTDR_SET_LBL_RT);
	}
	else
	{
		pOtdrCurveParaObj->pStrTitle[0] = GetCurrLanguageText(OTDR_SET_LBL_AVERAGE);
	}
}

/***
  * 功能：
		设置脉宽配置信息
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static void SetTestPulseTimeInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj)
{
	//获取测试参数
	CURVE_PRARM Param;
	GetCurvePara(CURR_CURVE, pDisplay, &Param);
	int array[12] = {0, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
	int enPulseTime = pUser_Setting->sFrontSetting[iCurrentWaveIndex].enPulseTime;
	char *temp = NULL;
	char buf[TEMP_ARRAY_SIZE] = { 0 };
	//脉宽参数不为0，使用上次测量的参数值
	if (Param.enPluse != 0)
		temp = Int2String(array[Param.enPluse]);
	else
	{
		if (enPulseTime == 0)//测试设置中设置为0
			sprintf(buf, "Auto");
		else//使用测试设置的参数值
			temp = Int2String(array[enPulseTime]);
	}
	//temp不为空
	if (temp)
		sprintf(buf, "%s ns", temp);

	pOtdrCurveParaObj->pStrTitle[1] = TransString(buf);
	GuiMemFree(temp);
}

/***
  * 功能：
		设置测试时长配置信息
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static void SetTestAverageTimeInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj)
{
	int averageTime = GetAverageTime(pUser_Setting->sFrontSetting[iCurrentWaveIndex].enAverageTime);

	if (averageTime == 1)
	{
		pOtdrCurveParaObj->pStrTitle[2] = TransString("RT");
	}
	else
	{
		char *temp = NULL;
		char buf[TEMP_ARRAY_SIZE] = { 0 };
		free(temp);
		temp = Int2String(averageTime);
		sprintf(buf, "%s s", temp);
		pOtdrCurveParaObj->pStrTitle[2] = TransString(buf);
		GuiMemFree(temp);
	}
}

/***
  * 功能：
		设置测试量程的配置信息
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static void SetTestRangeInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj)
{
	CURVE_PRARM Param;
	GetCurvePara(CURR_CURVE, pDisplay, &Param);
	int enFiberRange = pUser_Setting->sFrontSetting[iCurrentWaveIndex].enFiberRange;
	float fRange = 0.0f;
	//量程参数不为0，使用上次测量的参数值
	if (Param.enRange != 0)
		fRange = Param.fDistance;
	else
	{
		if (enFiberRange == 0) //测试设置中设置为0
			fRange = 0.0f;
		else //使用测试设置的参数值
			fRange = Range2Dist(enFiberRange);
	}
	
	//设置文本
	if (FLOATEQZERO(fRange))
		pOtdrCurveParaObj->pStrTitle[3] = TransString("Auto");
	else
		pOtdrCurveParaObj->pStrTitle[3] = UnitConverter_Dist_M2System_Float2GuiString(
			MODULE_UNIT_OTDR, fRange, 1);
}

/***
  * 功能：
		显示测量配置信息
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
void DisplayTestParaInfo(OTDR_CURVE_PARA * pOtdrCurveParaObj)
{
	DisplayPicture(pOtdrCurveParaObj->pBg);

	if (pOtdrCurveParaObj->iEnabled)
	{
		int i;
		for (i = 0; i < CURVE_TITLE_NUM; ++i)
		{
			SetLabelText(pOtdrCurveParaObj->pStrTitle[i], pOtdrCurveParaObj->pLblTitle[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pOtdrCurveParaObj->pLblTitle[i]);
			DisplayLabel(pOtdrCurveParaObj->pLblTitle[i]);
		}
	}
}

/***
  * 功能：
		设置AB标记线的测试信息
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static int SetMarkInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj)
{
	//设置标记线标题文本
// 	SetMarkTitleInfo(pOtdrCurveParaObj);
	//设置标记线数据文本
	if (pOtdrCurveParaObj->iEnabled)
	{
		//获得标记信息
		//若没有曲线，则返回，让界面上的内容保持默认的空值
		if (GetCurveMarker(CURR_CURVE, pDisplay, &mMarker) == 1)
			return -1;
		//A点距离
		pOtdrCurveParaObj->pStrDataContent[0][0] = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, mMarker.dDot[1].fDist, 1);
		//B点距离
		pOtdrCurveParaObj->pStrDataContent[1][0] = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, mMarker.dDot[2].fDist, 1);
		//A-B之间的距离
		float fDistAB = mMarker.dDot[2].fDist - mMarker.dDot[1].fDist;
		pOtdrCurveParaObj->pStrDataContent[0][1] = UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT_OTDR, fDistAB, 1);
		//A-B之间的损耗
		float fdBAB = mMarker.dDot[2].iValue - mMarker.dDot[1].iValue;
		pOtdrCurveParaObj->pStrDataContent[1][1] = Float2GuiStringUnit(LOSS_PRECISION, fdBAB / SCALE_FACTOR, "dB");
		//A-B间平均距离
		PCURVE_INFO pCurve = pDisplay->pCurve[pDisplay->iCurCurve];
		char *buff = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, (pCurve->cCoor.fMaxX)/ DIV_NUM_X, 1);
		char strTemp[TEMP_ARRAY_SIZE] = {0};
		sprintf(strTemp, "%s/div", buff);
		pOtdrCurveParaObj->pStrDataContent[0][2] = TransString(strTemp);
		GuiMemFree(buff);
		//A-B间衰减
		float fTmpVal = mMarker.fAttenAB;
		pOtdrCurveParaObj->pStrDataContent[1][2] = Float2GuiStringUnit(ATTENUATION_PRECISION, fTmpVal, "dB/km");
	}
	else
	{
		int i;
		int j;
		for (i = 0; i < 2; ++i)
		{
			for (j = 0; j < 3; ++j)
			{
				pOtdrCurveParaObj->pStrDataContent[i][j] = TransString("---");
			}
		}
	}

	return 0;
}

/***
  * 功能：
		设置标记线标题文本
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static void SetMarkTitleInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj)
{
	char* pStrTitle[CURVE_TITLE_NUM] = {"A", "A->B", "B", "Loss"};
	pOtdrCurveParaObj->pStrDataTitle[0][0] = TransString(pStrTitle[0]);
	pOtdrCurveParaObj->pStrDataTitle[0][1] = TransString(pStrTitle[1]);
	pOtdrCurveParaObj->pStrDataTitle[1][0] = TransString(pStrTitle[2]);
	pOtdrCurveParaObj->pStrDataTitle[1][1] = GetCurrLanguageText(OTDR_LBL_LOSS);
}

/***
  * 功能：
		显示标记线文本(包括标题以及信息)
  * 参数：
		1.OTDR_CURVE_PARA *pOtdrCurveParaObj : 指向pOtdrCurveParaObj控件
  * 返回：
		无
  * 备注：
***/
static void DisplayMarkInfo(OTDR_CURVE_PARA* pOtdrCurveParaObj)
{
	int i;
	int j;
	//刷新选中标记线信息背景
	SetPictureBitmap(BmpFileDirectory "bg_otdrCurve_dist_press.bmp",
					 gOtdrCurveParaObj->pBtn[gOtdrCurveParaObj->iFocus][0]);

	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			DisplayPicture(pOtdrCurveParaObj->pBtn[i][j]);
			//显示标记线标题
			if (j < 2)
			{
				DisplayLabel(pOtdrCurveParaObj->pLblDataTitle[i][j]);
			}
			//显示标记线数据内容
			SetLabelText(pOtdrCurveParaObj->pStrDataContent[i][j], pOtdrCurveParaObj->pLblDataContent[i][j]);
			DisplayLabel(pOtdrCurveParaObj->pLblDataContent[i][j]);
		}
	}
}

//获取是否在当前窗体的标志位
int GetCurrentWindow(void)
{
	return isInCurrWindow;
}

//按键控件响应回调函数
void CurveParaKeyBoardCallBack(int iSelected)
{
	int iFocus = gOtdrCurveParaObj->iFocus;

	switch (iSelected)
	{
	case KEYCODE_UP:
		gOtdrCurveParaObj->iFocus = (iFocus == 0) ? 1 : 0;
		isInCurrWindow = 0;
		break;
	case KEYCODE_DOWN:
		gOtdrCurveParaObj->iFocus = (iFocus == 1) ? 0 : 1;
		isInCurrWindow = 0;
		break;
	default:
		break;
	}

	SetPictureBitmap(BmpFileDirectory "bg_otdrCurve_dist_press.bmp",
					 gOtdrCurveParaObj->pBtn[gOtdrCurveParaObj->iFocus][0]);
	SetPictureBitmap(BmpFileDirectory "bg_otdrCurve_dist_unpress.bmp",
					 gOtdrCurveParaObj->pBtn[1 - gOtdrCurveParaObj->iFocus][0]);

	DisplayMarkInfo(gOtdrCurveParaObj);
	RefreshScreen(__FILE__, __func__, __LINE__);
}
