/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_OtdrCalibration.c
* 摘    要：  生成 校准数据的函数实现
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "app_otdrcalibration.h"
#include "app_math.h"
#include "app_otdr_algorithm_parameters.h"
#include "drv_optic.h"
#include "app_eventsearch.h"
#include "app_algorithm_support.h"

extern float fLogSig[];
//extern UINT32 OffsetTable[OPM_DISTRANGE_KIND][2]; //偏移量

/********************************************************************************************************
* 函数描述                    计算直线的线性度
* @ param[in] LogSignal     输入数据
* @ return                            线性度
* @ note      name 描述
*********************************************************************************************************/
float CalcLinearity(POTDR_TOP_SETTINGS pOtdrTopSettings)
{
	int iRet = 0;
    PDAQ_SETTING pDaq_Setting = NULL;
	DBG_ENTER();
	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;
	POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	
    pDaq_Setting = (PDAQ_SETTING)calloc(1, sizeof(DAQ_SETTING));//为采集设置申请空间
    pDaq_Setting->pOrigData = (UINT16 *)calloc(1, MAX_DATA * sizeof(UINT16));
	pDaq_Setting->iPulseKind = (UINT32)ENUM_PULSE_10US;
   	pDaq_Setting->iAtteDbClass = (UINT32)ENUM_22_5DB;//该处衰减量较大，只需探测较大的端面反射
    pDaq_Setting->iFilter = (UINT32)_IIR_1K;
    pDaq_Setting->iDaqCount = MAX_DATA - BLACK_LEVEL_LEN;
    pDaq_Setting->iDisplayCount = MAX_DATA;
	//pDaq_Setting->iOffset = OffsetTable[5][0];
	pDaq_Setting->iOffset = offsetExp[5][0];
	pDaq_Setting->iSmpReg = INTP01_01;
	pDaq_Setting->fSmpRatio = SMP_COUNT01_01;
	pDaq_Setting->iNoiseCnt = BLACK_LEVEL_LEN;
	pDaq_Setting->iCloseLdPulseFlag = 0;
	
	Opm_SetWave(iOpmFd, pOpmSet, ENUM_WAVE_1310NM);
	
	int iLen = MAX_DATA - pDaq_Setting->iOffset;
	int iBlackLevel = 0;
    float *fSignal = NULL;
	fSignal = (float *)calloc(iLen, sizeof(float));
	float fLinearity = 10.0f;
	iRet = DaqStart(pDaq_Setting);
	if (!iRet)
	{
		//修改噪声计算，changed by  2015.10.8
	    iBlackLevel = (int)floor(GetMeanUint16(pDaq_Setting->pOrigData + iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 0.5f);
		//将原始信号转换为对数信号;
		int iTemp = 0;
		int i = 0;
		for(i=0; i<iLen; i++)
		{
			iTemp = iBlackLevel - pDaq_Setting->pOrigData[i];
			if ((iTemp > 0) && (iTemp < 65536))
			{
				fSignal[i] = fLogSig[iTemp];
			}
			else if((iTemp > -65536) && (iTemp < 0))
			{
				fSignal[i] = - fLogSig[-iTemp];
			}		
			else
			{
				fSignal[i] = 0;
			}
	    }
#if 0
		FILE* fp = NULL;
		char strPath[20] = "/mnt/usb";
		
		char strFile[512];
		sprintf(strFile, "%s/CalcLinearity.txt", strPath);
		fp = fopen(strFile, "w");
		
		if (fp != NULL)
		{
			for(i=0; i<iLen; i++)
				fprintf(fp,"%f\r\n", fSignal[i]);
			fclose(fp);
		}
#endif
		
		float fMaxNoise = GetMaxFloat(fSignal + iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
		int iStart_Count = 0;
		int iStart = 0;
		int iEnd_Count = 0;
		int iEnd = 0;
		if (LINEARITY_MAX_VALUE - fMaxNoise < 9)
		{
			fMaxNoise = LINEARITY_MAX_VALUE - 9.1f;
		}
		//查找拟合的起始点和末端点
		for (i=1000; i<iLen; i++)
		{
			if (fSignal[i] <= LINEARITY_MAX_VALUE)
			{
				iStart_Count++;
			}
			else
			{
				iStart_Count = 0;
			}
			if (iStart_Count == 5)
			{
				iStart = i;
			}
			if (fSignal[i] <= fMaxNoise + 9)
			{
				iEnd_Count++;
			}
			else
			{
				iEnd_Count = 0;
			}
			if (iEnd_Count == 5)
			{
				iEnd = i - 5;
				break;
			}
		}
		
		int iFitLen = Max(iEnd - iStart, 2000);//拟合长度
		iEnd = Max(iEnd, iStart + 2000);
		float fDelta = 10000.0f;//缩小量，防止溢出;
#if 0
		float sx=0, sy=0, sxy=0, sxx=0, syy = 0, fX = 0.0f;
		float fX_avg = 0.0f;
		float fY_avg = 0.0f;

		for (i=iStart; i<iEnd; i++)
		{
			fX = i / fDelta;
			sx = sx + fX;
			sy = sy + fSignal[i];
			sxy = sxy + fX * fSignal[i];
			sxx = sxx + fX * fX;
			syy = syy + fSignal[i] * fSignal[i];
		}
		fX_avg = sx / iFitLen;
		fY_avg = sy / iFitLen;
		
		printf("sy = %f,sx = %f,sxy = %f,sxx = %f,syy = %f,fX_avg = %f,fY_avg = %f\n", sy, sx, sxy, sxx, syy, fX_avg, fY_avg);
		
		fLinearity = 1 + (sxy - iFitLen * fX_avg * fY_avg)
			/ sqrt((sxx - iFitLen * fX_avg * fX_avg) * (syy - iFitLen * fY_avg * fY_avg));
		
		printf("---1-------fLinearity = %f------------\n", fLinearity);
#endif

		float pFitCoef[2] = {0.0f};
		LineFit(fSignal + iStart, iFitLen, iStart, pFitCoef);
		float fMaxDelta = 0.0f;
		float fMinDelta = 0.0f;
		for (i=iStart; i<iEnd; i++)
		{
			float fTmp = fSignal[i] - pFitCoef[0] * i - pFitCoef[1];
			if (fMaxDelta < fTmp)
			{
				fMaxDelta = fTmp;
			}
			if (fMinDelta > fTmp)
			{
				fMinDelta = fTmp;
			} 
		}

		if (fabsf(fMinDelta) > fabsf(fMaxDelta))
		{
			fDelta = fMinDelta;
		}
		else
		{
			fDelta = fMaxDelta;
		}
		
		float fMax = pFitCoef[0] * iFitLen;
		fLinearity = fDelta / fMax; //线性度
		/*
		if (fLinearity <= -1.0f)
		{
			fLinearity = 0.0f;
		}
		else
		{
			fLinearity = 10 * log10f(1.0f + fLinearity);
		}
		*/
		LOG(LOG_INFO, "fMax = %f,fDelta = %f\n", fMax, fDelta);
		LOG(LOG_INFO, "---2-------fLinearity = %f------------\n", fLinearity);
	}

	free(pDaq_Setting->pOrigData);
    pDaq_Setting->pOrigData = NULL;
    free(pDaq_Setting);
    pDaq_Setting = NULL;
	free(fSignal);
	fSignal = NULL;
	
	return fLinearity;

}

/********************************************************************************************************
* 函数描述                                  计算数据的不确定度
* @ param[in] fData                           输入测试数据
* @ param[in] fFiberRealLen             输入光纤校准的长度
* @ param[in] fDeviation                    输出偏差
* @ param[in] fFiberTestLen              输出光纤测试长度的均值
* @ param[in] Uncertainty                  不确定度
* @ param[in] fConformance             输出符合度的极限值
* @ param[in] fSpecification               输入规范值
* @ return                                           不确定度
* @ note      name 描述
*********************************************************************************************************/
float CalcUncertainty(float fData[UNCERT_DATA_LEN], float fFiberRealLen, float *fDeviation,
					float *fFiberTestLen, float *Uncertainty, float *fConformance, float fSpecification)
{
	float fStd = 0.0f;
	
	*fFiberTestLen = GetMeanFloat(fData, UNCERT_DATA_LEN);
	*fDeviation = *fFiberTestLen - fFiberRealLen;
	fStd = GetStdFloat(fData, UNCERT_DATA_LEN);
	*Uncertainty = fStd / sqrt(UNCERT_DATA_LEN - 1);    //U=S/N^0.5
	*fConformance = fSpecification - 0.83f * (*Uncertainty);

	LOG(LOG_INFO, "fFiberTestLen = %f, fDeviation = %f, Uncertainty = %f, fStd = %f, fConformance = %f, fSpecification = %f\n",
		*fFiberTestLen, *fDeviation, *Uncertainty, fStd, *fConformance, fSpecification);

	/*
	int i = 0;
	float fSum = 0.0f;
	for (i=0; i<UNCERT_DATA_LEN; i++)
	{
		fSum += (fData[i] - fFiberTestLen) * (fData[i] - fFiberTestLen);
		
		sx = sx + fX;
		sy = sy + fData[i];
		sxy = sxy + fX * fData[i];
		sxx = sxx + fX * fX;
		syy = syy + fData[i] * fData[i];
		
	}
	
	*fFiberTestLen = fSum / UNCERT_DATA_LEN;
	*fDeviation = *fFiberTestLen - fFiberRealLen;
	*/
	return *Uncertainty;
}

