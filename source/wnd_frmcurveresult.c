/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcurveresult.c
* 摘    要：  实现主窗体frmcurveresult的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/26
*
*******************************************************************************/

#include "wnd_frmcurveresult.h"

#include "app_global.h"
#include "app_frmotdr.h"
#include "app_frminit.h"
#include "app_algorithm_support.h"
#include "app_unitconverter.h"

#include "wnd_global.h"


extern POTDR_TOP_SETTINGS pOtdrTopSettings;		//设置顶层结构
static PUSER_SETTINGS pUser_Setting = NULL;
static PDISPLAY_INFO pDisplay = NULL;
static int iCurrentWaveIndex = 0;
//暂存当前的事件表
static EVENTS_TABLE *pEvents = NULL;

//复制曲线结果
static int CopyCurveResultInfo(struct Curve_Result* pCurveResult, int ioldWaveIndex, int iNewWaveIndex);
//设置曲线结果
static int SetCurveResultInfo(struct Curve_Result* pCurveResult, int iwaveIndex);
//获取总的事件数
static int GetTotalEventNum(int iWaveIndex);

/***
  * 功能：
		创建一个曲线结果控件
  * 参数：
		1.int iStartX:			控件的起始横坐标
		2.int iStartY:			控件的起始纵坐标
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
struct Curve_Result* CreateCurveResult(int iStartX, int iStartY)
{
	//错误标志定义
	int iErr = 0;
	pUser_Setting = pOtdrTopSettings->pUser_Setting;
	pDisplay = pOtdrTopSettings->pDisplayInfo;
	iCurrentWaveIndex = pUser_Settings->enWaveCurPos;
	pEvents = (EVENTS_TABLE *)calloc(1, sizeof(EVENTS_TABLE));

	if (NULL == pEvents)
	{
		LOG(LOG_ERROR, "--NULL == pEvents------\n");
	}

	struct Curve_Result* pCurveResult = NULL;

	if (!iErr)
	{
		//分配资源
		pCurveResult = (struct Curve_Result*)calloc(1, sizeof(struct Curve_Result));
		if (NULL == pCurveResult)
		{
			LOG(LOG_ERROR, "pCurveResult Malloc ERR\n");
			iErr = -2;
		}
	}

	if (!iErr)
	{
		pCurveResult->pBg = CreatePicture(iStartX, iStartY, 640, 201, BmpFileDirectory"bg_otdr_db.bmp");
		pCurveResult->pStrFiberLength = GetCurrLanguageText(OTDR_LBL_FIBER_LENGTH);
		pCurveResult->pStrFiberLengthValue = TransString(" ");
		pCurveResult->pLblFiberLength = CreateLabel(iStartX+256, iStartY+11, 100, 24, pCurveResult->pStrFiberLength);
		pCurveResult->pLblFiberLengthValue = CreateLabel(iStartX + 256 + 100, iStartY + 11, 100, 24, pCurveResult->pStrFiberLengthValue);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveResult->pLblFiberLength);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveResult->pLblFiberLengthValue);
		unsigned int strTableTitle[CURVE_RESULT_TITLE_NUM] = 
		{ OTDR_LBL_WAVE, OTDR_LBL_TOTAL_ATT, OTDR_LBL_ATT_FACTOR, OTDR_LBL_TOTAL_EVENT_NUM };
		int i = 0;
		int j = 0;
		for (i = 0; i < CURVE_RESULT_TITLE_NUM; ++i)
		{
			pCurveResult->pStrTableTitle[i] = GetCurrLanguageText(strTableTitle[i]);
			pCurveResult->pLblTableTitle[i] = CreateLabel(iStartX + 160 * i+20, iStartY + 49, 120, 24, pCurveResult->pStrTableTitle[i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveResult->pLblTableTitle[i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveResult->pLblTableTitle[i]);
			pCurveResult->pLblWaveInfo[j][i] = CreateLabel(iStartX + 160 * i+30, iStartY + 49 + 37, 100, 24, pCurveResult->pStrWaveInfo[j][i]);
			pCurveResult->pLblWaveInfo[j+1][i] = CreateLabel(iStartX + 160 * i+30, iStartY + 49 + 37 + 31, 100, 24, pCurveResult->pStrWaveInfo[j+1][i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveResult->pLblWaveInfo[j][i]);
			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pCurveResult->pLblWaveInfo[j+1][i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveResult->pLblWaveInfo[j][i]);
			SetLabelAlign(GUILABEL_ALIGN_CENTER, pCurveResult->pLblWaveInfo[j + 1][i]);
		}
	}

	pCurveResult->iEnable = 0; //默认不使能

	return pCurveResult;
}

/***
  * 功能：
		销毁一个曲线结果控件
  * 参数：
		struct Curve_Result** pCurveResult：指向曲线结果控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroyCurveResult(struct Curve_Result** pCurveResult)
{
	//错误标志定义
	int iErr = 0;

	if (*pCurveResult == NULL)
	{
		LOG(LOG_ERROR, "pCurveResult is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		DestroyPicture(&((*pCurveResult)->pBg));
		GuiMemFree((*pCurveResult)->pStrFiberLength);
		GuiMemFree((*pCurveResult)->pStrFiberLengthValue);
		DestroyLabel(&((*pCurveResult)->pLblFiberLength));
		DestroyLabel(&((*pCurveResult)->pLblFiberLengthValue));
		int i = 0;
		int j = 0;
		for (i = 0; i < CURVE_RESULT_TITLE_NUM; ++i)
		{
			DestroyLabel(&((*pCurveResult)->pLblTableTitle[i]));
			DestroyLabel(&((*pCurveResult)->pLblWaveInfo[j][i]));
			DestroyLabel(&((*pCurveResult)->pLblWaveInfo[j+1][i]));
			GuiMemFree((*pCurveResult)->pStrTableTitle[i]);
			GuiMemFree((*pCurveResult)->pStrWaveInfo[j][i]);
			GuiMemFree((*pCurveResult)->pStrWaveInfo[j][i]);
		}
	}

	free(*pCurveResult);
	(*pCurveResult) = NULL;

	GuiMemFree(pEvents);

	return iErr;
}

/***
  * 功能：
		显示一个曲线结果控件
  * 参数：
		struct Curve_Result* pCurveResult：指向曲线结果控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplayCurveResult(struct Curve_Result* pCurveResult)
{
	//错误标志定义
	int iErr = 0;

	if (pCurveResult == NULL)
	{
		LOG(LOG_ERROR, "pCurveResult is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		DisplayPicture(pCurveResult->pBg);
// 		SetSliderBackground(pSliderBlock);
		DisplayLabel(pCurveResult->pLblFiberLength);
		DisplayLabel(pCurveResult->pLblFiberLengthValue);
		int i = 0;
		int j = 0;
		for (i = 0; i < CURVE_RESULT_TITLE_NUM; ++i)
		{
			DisplayLabel(pCurveResult->pLblTableTitle[i]);
			DisplayLabel(pCurveResult->pLblWaveInfo[j][i]);
			DisplayLabel(pCurveResult->pLblWaveInfo[j + 1][i]);
		}
	}

	return iErr;
}

/***
  * 功能：
		注册一个曲线结果控件
  * 参数：
		1.struct Curve_Result* pCurveREsult：指向曲线结果控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int AddCurveResultToWnd(struct Curve_Result* pCurveResult, GUIWINDOW* pWnd)
{
	//错误标志定义
	int iErr = 0;
// 	GUIMESSAGE *pMsg = GetCurrMessage();;

	if ((NULL == pCurveResult) || (NULL == pWnd))
	{
		LOG(LOG_ERROR, "pCurveResult is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		//addwnd
		//logreg
	}

	return iErr;

}

/***
  * 功能：
		移除一个曲线结果控件
  * 参数：
		1.struct Curve_Result* pCurveResult：指向曲线结果控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DelCurveResultFromWnd(struct Curve_Result* pCurveResult, GUIWINDOW* pWnd)
{
	//错误标志定义
	int iErr = 0;
// 	GUIMESSAGE *pMsg = GetCurrMessage();;

	if ((NULL == pCurveResult) || (NULL == pWnd))
	{
		LOG(LOG_ERROR, "pCurveResult is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
	}

	return iErr;

}

/***
  * 功能：
		刷新曲线结果控件
  * 参数：
		1.struct Curve_Result* pCurveResult：指向曲线结果控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int FlushCurveResult(struct Curve_Result* pCurveResult)
{
	//错误标志定义
	int iErr = 0;

	if (pCurveResult == NULL)
	{
		LOG(LOG_ERROR, "pCurveResult is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		if (pCurveResult->iEnable)
		{
			int iWave0 = pUser_Setting->iWave[0];
			int iWave1 = pUser_Setting->iWave[1];

			if (((iWave0 == 1) && (iWave1 == 0))
				|| ((iWave0 == 0) && (iWave1 == 1)))
			{
				SetCurveResultInfo(pCurveResult, pDisplay->iCurCurve);
			}
			else
			{
				SetCurveResultInfo(pCurveResult, 0);
				SetCurveResultInfo(pCurveResult, 1);
			}

			if (iWave0 == 0 && iWave1 == 1)
			{
				//将1550的曲线结果拷贝到1310曲线的位置
				CopyCurveResultInfo(pCurveResult, 1, 0);
			}

			DisplayCurveResult(pCurveResult);
		}
	}

	return iErr;
}
//复制曲线结果
int CopyCurveResultInfo(struct Curve_Result * pCurveResult, int ioldWaveIndex, int iNewWaveIndex)
{
	int i;
	for (i = 0; i < CURVE_RESULT_TITLE_NUM; ++i)
	{
		if (!(pCurveResult->pStrWaveInfo[ioldWaveIndex][i]))
			SetLabelText(pCurveResult->pStrWaveInfo[ioldWaveIndex][i],
						 pCurveResult->pLblWaveInfo[iNewWaveIndex][i]);
		GuiMemFree(pCurveResult->pStrWaveInfo[ioldWaveIndex][i]);
		DestroyLabel(&pCurveResult->pLblWaveInfo[ioldWaveIndex][i]);
	}

	return 0;
}

//设置曲线结果
int SetCurveResultInfo(struct Curve_Result* pCurveResult, int iwaveIndex)
{
	//错误标志定义
	int iErr = 0;

	if (pCurveResult == NULL)
	{
		LOG(LOG_ERROR, "pCurveResult is NULL\n");
		iErr = -2;
	}

	if (!iErr)
	{
		//1.wave
		if (pUser_Setting->iWave[0] == 0 && pUser_Setting->iWave[1] == 1)
		{
			pCurveResult->pStrWaveInfo[iwaveIndex][0] = TransString("1550");
		}
		else
		{
			pCurveResult->pStrWaveInfo[iwaveIndex][0] = (iwaveIndex == 0) ? 
														TransString("1310") : TransString("1550");
		}
		//获取事件个数
		int iEventNum = GetTotalEventNum(iwaveIndex);
		//线路总损耗(最后一个事件点的totalLoss)
		float fTotalLoss = 0.000f;
		//光纤长度
		float fTotalFiberLength = 0.00f;
		//平均损耗
		float fAvgLoss = 0.000f;
		//当超过一个事件时，计算各个数值，其他情况都为0
		if (iEventNum > 1)
		{
			EVENTS_INFO *pEventInfo = &(pEvents->EventsInfo[iEventNum - 1]);
			fTotalLoss = pEventInfo->fTotalLoss;
			fTotalFiberLength = pEventInfo->fEventsPosition;
			fAvgLoss = fTotalLoss / UnitConverter_Dist_Float2Float(UNIT_M, UNIT_KM, fTotalFiberLength);
		}
		//2.total atten
		pCurveResult->pStrWaveInfo[iwaveIndex][1] = Float2GuiString(LOSS_PRECISION, fTotalLoss);
		//3.atten
		pCurveResult->pStrWaveInfo[iwaveIndex][2] = Float2GuiString(ATTENUATION_PRECISION, fAvgLoss);
		//4.total event num
		pCurveResult->pStrWaveInfo[iwaveIndex][3] = Int2GuiString(iEventNum);
		//光纤长度
		pCurveResult->pStrFiberLengthValue = UnitConverter_Dist_M2System_Float2GuiString(
												MODULE_UNIT_OTDR, fTotalFiberLength, 1);
		SetLabelText(pCurveResult->pStrFiberLengthValue, pCurveResult->pLblFiberLengthValue);
		int i;
		for (i = 0; i < CURVE_RESULT_TITLE_NUM; ++i)
		{
			SetLabelText(pCurveResult->pStrWaveInfo[iwaveIndex][i], 
							pCurveResult->pLblWaveInfo[iwaveIndex][i]);
		}
	}

	return iErr;

}

//获取总的事件数
static int GetTotalEventNum(int iWaveIndex)
{
	if (pEvents == NULL)
	{
		return -1;
	}
	
	int iEventNum = 0;
	//获取事件列表
	GetCurveEvents(iWaveIndex, pDisplay, pEvents);
	iEventNum = pEvents->iEventsNumber;

	return iEventNum;
}

//设置控件的使能状态
int SetCurveResultEnable(struct Curve_Result* pCurveResult, int iEnable, GUIWINDOW *pWnd)
{
	//判断使能状态
	if (pCurveResult->iEnable != iEnable)
	{
		if (iEnable)
		{
			AddCurveResultToWnd(pCurveResult, pWnd);
		}
		else
		{
			DelCurveResultFromWnd(pCurveResult, pWnd);
		}

		//设置使能标志
		pCurveResult->iEnable = iEnable;
	}

	return 0;
}
