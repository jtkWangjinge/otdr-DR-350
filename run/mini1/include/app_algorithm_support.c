/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_algorithm_support.h
* 摘    要：  算法支持的函数及结构体定义
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-21
*
*******************************************************************************/

#include "app_math.h"
#include "app_algorithm_support.h"


// 生成log对数表0~65535
float fLogSig[MAX_DATA_VALUE] = { 0.0f };
void LogTableCreat2(void)
{
	int i = 0;
	fLogSig[0] = 0.0f;
	
	for (i = 1; i < MAX_DATA_VALUE; ++i)
	{
		fLogSig[i] = 5.0f * log10f((float)i);
	}
}

// 该函数用于生成指数数据表格，以方便指数计算
float fPowerSig[MAX_DATA_VALUE] = { 0.0f };
void PowerTableCreat(void)
{
	int i = 0;
	float fTmp = 0.0f;
	
	fPowerSig[0] = 1.0f;
	
	for (i = 1; i < MAX_DATA_VALUE; ++i)
	{
		fTmp = (float)i / (5.0f * SCALE_FACTOR);
		fPowerSig[i] = powf(10.0f, fTmp);
	}
}

// 计算采样点长度
int CalSamplePointNum(float range, float refractiveIndex, float smpRatio)
{
	float fPeriod = 1.0f / ADC_CLOCK_FREQ;
	float vacuumSpeed = 299.792458f; 						// 真空中光速 30W Km/s,单位1000Km/s;    
	float mediaSpeed = vacuumSpeed / refractiveIndex;		// v = c / n 计算出介质中光速          
    float fiberLen = range * 1000.0f * 2.0f / (fPeriod * mediaSpeed);	// Lenth = Cnt * (T * v / 2)
	return (int)(fiberLen * smpRatio + 0.5f);
}

// 计算盲区宽度
int CalBlindWidth(int pulseWidth, float ratio)
{
	 return Max((int)((pulseWidth * ADC_CLOCK_FREQ * ratio) / 1000.0f), 1);
}

// 获取APD增减步长
int GetAPDStep(float temperature)
{
	int step = 0;
	int apdStep[4] = {0, 15, 25, 50};
	float temperatureDial[4] = {0.0f, 50.0f, 55.0f, 60.0f};

	int i = 0;
	for(; i < 3; ++i)
	{
		if((temperature > temperatureDial[i])
			&& (temperature <= temperatureDial[i + 1]))
		{
			step = apdStep[i];
		}
	}

	return step;
}

// 计算光纤实际长度,单位为m
float GetFiberRealLength(int iSampleLength, float fRefractiveIndex, float fRatio)
{						
    float fVacuumSpeed = 3.0f * 100.0f;   					//真空中光速 30W Km/s,单位1000Km/s;    
    float fPeriod = 1.0f / ADC_CLOCK_FREQ;					// T = 1 / F 计算出时钟周期         
    float fMediaSpeed = fVacuumSpeed / fRefractiveIndex;	// v = c / n 计算出介质中光速          
    float fFiberRealLen = (float)iSampleLength * (fPeriod * fMediaSpeed / 2.0f); // 除以2是因为来回
	fFiberRealLen = fFiberRealLen / fRatio;

    return fFiberRealLen;
}

// 计算Rayleigh散射强度
float CalcRayleigh(int enPulse, float fRatio, float fRLCoef)
{
	// 计算瑞利散射强度
	int iBlind = CalBlindWidth(enPulse, fRatio);
	float fRayleigh = 10.0f * log10f((float)(iBlind * ADC_MAX_PHASE / fRatio));
	fRayleigh += fRLCoef;

	// 根据实际脉宽对小脉宽进行修正
	if (iBlind == 1)
    {
        if (enPulse == 0)              
            fRayleigh /= 5.0f;
        else if (enPulse == 1)
            fRayleigh /= 2.5f;
        else if (enPulse == 2)
            fRayleigh /= 1.25f;
    }   

	return fRayleigh;
}

// 该函数主要用来拟合m 到 n直线
SLINE FittingLine(unsigned short *pData, int m, int n)
{
	// 申请资源
	int iLen = abs(m - n);
	int *pPiontX = (int *)malloc(iLen * sizeof(int));
	int *pPiontY = (int *)malloc(iLen * sizeof(int));

	int i = 0;
	for (; i < iLen; ++i)
	{
		pPiontX[i] = m + i;
		pPiontY[i] = pData[m + i];
	}

	// 最小二乘法拟合计算斜率
	SLINE sLine;
	CalLeastSquare(pPiontX, pPiontY, iLen, &sLine.k, &sLine.b);

	// 释放资源
	free(pPiontX);
	free(pPiontY);
	
	return sLine;
}

// 使用LSA计算熔接损耗
float LossLSA(int iIndexA, int iFactor, SLINE *pLineA, SLINE *pLineB) 						
{
	float fLoss = 0.0f;

	// LSA 计算损耗
	fLoss = (pLineA->k - pLineB->k) * (float)iIndexA + (pLineA->b - pLineB->b);
	fLoss /= iFactor;
	
	return fLoss;
}

// 该函数主要用来计算反射率
float CalcReflect(unsigned short *pData, int m, int n, float fRayleigh, int iFactor, SLINE *pLineA)					 
{
	//返回值
	float fRet = 0.0f;

	int iMaxEigen = 0;
	int iMaxEigenCur = -1;

	//查找标记点2-3之间的最大值
	int iLen = abs(n - m);
	int iStart = Min(m, n);

	int i = 0;
	for (; i < iLen; ++i)
	{
		if (iMaxEigen < pData[iStart + i])
		{
			iMaxEigen = pData[iStart + i];
			iMaxEigenCur = iStart + i;
		}
	}

	// 判断是否有效
	if (iMaxEigenCur >= 0)
	{
		fRet = iMaxEigen - (iMaxEigenCur * pLineA->k + pLineA->b);
		fRet /= iFactor;
	}
	
	// 反射率计算
	fRet = fRayleigh + 10.0f * log10f(powf(10.0f, fRet / 5.0f) - 1.0f);
		
	if (isnan(fRet) || isinf(fRet))
	{
		fRet = 0.0f;
	}

	return fRet;
}