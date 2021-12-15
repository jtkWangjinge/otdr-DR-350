/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_math.cpp
* 摘    要：  常用的数学函数的实现
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-21
*
*******************************************************************************/

#include "app_math.h"

/***********************************************
* 得到UINT16型数组的平均值
***********************************************/
unsigned short GetMeanUint16(					 // 得到数据平均值
		unsigned short *pSignal,				 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	if (iSignalLength <= 0)
		return pSignal[0];

	int iSumNumber = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		iSumNumber = iSumNumber + pSignal[i];
	}

	return iSumNumber / iSignalLength;
}

/***********************************************
* 得到UINT16型数组的最大值
***********************************************/
unsigned short GetMaxUint16(					 // 得到数据最大值
		unsigned short *pSignal,				 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度   

{
	unsigned short iMaxNumber = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > iMaxNumber)
		{
			iMaxNumber = pSignal[i];
		}
	}

	return iMaxNumber;
}

/***********************************************
* 得到UINT16型数组的最大值
***********************************************/
unsigned short GetMinUint16(					 // 返回数据最大值
		unsigned short *pSignal,				 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度   
{
	int iMinNumber = 65535;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] < iMinNumber)
		{
			iMinNumber = pSignal[i];
		}
	}

	return iMinNumber;
}

/***********************************************
* 得到unsigned short型数组的标准差
***********************************************/
float GetStdUint16(								 // 得到数据标准差
		unsigned short *pSignal,				 // 输入的数组指针
		int iSignalLength) 						 // 进的数据的数据长度 
{

	unsigned int iSumNumber = 0;
	float fMeanNumber = 0.0f;//平均值
	float fSumNumber = 0.0f;//方差

	if (iSignalLength <= 1)
		return 0;

	int i = 0;
	for (; i < iSignalLength; ++i)
		iSumNumber = iSumNumber + pSignal[i];

	fMeanNumber = (float)iSumNumber / (float)iSignalLength;

	for (i = 0; i < iSignalLength; ++i)
	{
		fSumNumber = fSumNumber + ((float)pSignal[i] - fMeanNumber) * ((float)pSignal[i] - fMeanNumber);
	}

	return sqrtf(fSumNumber / (float)(iSignalLength - 1));
}

/***********************************************
* 得到unsigned short型数组中第一次出现最大值的索引值
***********************************************/
int GetMaxUint16Index(							 // 得到最大值的索引值
		unsigned short *pSignal,				 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	unsigned short iMaxNumber = 0;
	int iMaxIndex = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > iMaxNumber)
		{
			iMaxNumber = pSignal[i];
			iMaxIndex = i;
		}
	}

	return iMaxIndex;
}

/***********************************************
* 得到unsigned short数组中第一次出现最小值的索引值
***********************************************/
int GetMinUint16Index(							 // 得到最小值的索引值
		unsigned short *pSignal,				 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	int iMinIndex = 0;
	unsigned short iMinNumber = 65535;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] < iMinNumber)
		{
			iMinNumber = pSignal[i];
			iMinIndex = i;
		}
	}

	return iMinIndex;
}
 
// unsigned int

/***********************************************
* 得到uint32型数组的最大值
***********************************************/
unsigned int GetMaxUint32(						 // 返得到数据最大值
		unsigned int *pSignal,				     // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度 
{
	unsigned int MaxNumber = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > MaxNumber)
		{
			MaxNumber = pSignal[i];
		}
	}

	return MaxNumber;
}

/***********************************************
* 得到uint32型数组的最小值
***********************************************/
unsigned int GetMinUint32(						 // 得到数据最小值
		unsigned int *pSignal,					 // 输入的数组指针
		int iSignalLength)					     // 输入的数据长度
{
	unsigned int iMinNumber = 100000000;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] < iMinNumber)
		{
			iMinNumber = pSignal[i];
		}
	}

	return iMinNumber;
}

/***********************************************
* 得到uint32型数组的平均值
***********************************************/
float GetMeanUint32(							 // 返回数据平均值
		unsigned int *pSignal,					 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{

	if (iSignalLength <= 0)
		return (float)pSignal[0];

	float fSumNumber = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
		fSumNumber = fSumNumber + pSignal[i];

	return fSumNumber / (float)iSignalLength;
}

/***********************************************
* 得到uint32型数组的标准差
***********************************************/
float GetStdUint32(								 // 得到数据标准差
		unsigned int *pSignal,					 // 输入的数组指针
		int iSignalLength) 						 // 输入的数据长度 
{
	if (iSignalLength <= 1)
		return 0;

	float fSumSignal = 0.0f;            // 信号累加和
	float fMeanNumber = 0.0f;			// 均值
	float fSumNumber = 0.0f;			// 减去均值后的平方和
	int i = 0;
	for (; i < iSignalLength; ++i)
		fSumSignal = fSumSignal + (float)pSignal[i];

	fMeanNumber = fSumSignal / (float)iSignalLength;

	for (i = 0; i < iSignalLength; ++i)
	{
		fSumNumber = fSumNumber + ((float)pSignal[i] - fMeanNumber) * ((float)pSignal[i] - fMeanNumber);
	}

	return sqrtf(fSumNumber / (float)(iSignalLength - 1));
}

/***********************************************
* 得到unsigned int型数组中第一次出现fFindNumber的索引值
***********************************************/
int FindFirstIndexUint32(						 // 得到索引值，返回iSignalLength表示查找失败
		unsigned int *pSignal,					 // 输入的数组指针
		int iSignalLength,						 // 输入的数据长度
		unsigned int iFindNumber)				 // 输入待索引的数据
{
	int i = 0;
	for (; i < iSignalLength; ++i)
		if (pSignal[i] == iFindNumber)
			return i;
	return i;
}

// int

/***********************************************
* 得到int型数组的绝对值的平均值
***********************************************/
float GetMeanAbsInt(							 // 得到数据绝对值的平均值
		int *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{  
    if (iSignalLength <= 0)
		return (float)pSignal[0];

	int iSumNumber = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > 0)
			iSumNumber = iSumNumber + pSignal[i];
		else
			iSumNumber = iSumNumber - pSignal[i];
	}

	return (float)iSumNumber / (float)iSignalLength;
}

/**********************************************
* 得到int型数组的最大值
***********************************************/
int GetMaxInt(									 // 得到数据最大值
		int *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度  
{
	int iMaxNumber = -100000000;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > iMaxNumber)
		{
			iMaxNumber = pSignal[i];
		}
	}

	return iMaxNumber;
}

/***********************************************
* 得到int型数组的最小值
***********************************************/
int GetMinInt(									 // 得到数据最小值
		int *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度   
{
	int iMinNumber = 100000000;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] < iMinNumber)
		{
			iMinNumber = pSignal[i];
		}
	}

	return iMinNumber;
}

/***********************************************
* 得到int型数组的平均值
***********************************************/
float GetMeanInt(								 // 返回数据平均值
		int *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
    if (iSignalLength <= 0)
		return (float)pSignal[0];

	int iSumNumber = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
		iSumNumber = iSumNumber + pSignal[i];

	return (float)iSumNumber / (float)iSignalLength;
}

/***********************************************
* 得到int型数组的标准差
***********************************************/
float GetStdInt(								 // 得到数据标准差
		int *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	if (iSignalLength <= 1)
		return 0.0f;

	float fSumSignal = 0;
	float fMeanNumber = 0.0f;
	float fSumNumber = 0.0f;

	int i = 0;
	for (; i < iSignalLength; ++i)
		fSumSignal = fSumSignal + (float)pSignal[i];
	
	fMeanNumber = fSumSignal /(float)iSignalLength;

	for (i = 0; i < iSignalLength; ++i)
	{
		fSumNumber = fSumNumber + ((float)pSignal[i] - fMeanNumber) * ((float)pSignal[i] - fMeanNumber);
	}

	return sqrtf(fSumNumber / ((float)iSignalLength - 1));
}

/***********************************************
* 得到int型数组中第一次出现最大值的索引值
***********************************************/
int GetMaxIntIndex(								 // 得到最大值的索引值
	int *pSignal,								 // 输入的数组指针
	int iSignalLength)						 // 输入的数据长度
{
	int iMaxNumber = -100000000;
	int iMaxIndex = 0;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > iMaxNumber)
		{
			iMaxNumber = pSignal[i];
			iMaxIndex = i;
		}
	}

	return iMaxIndex;
}

/***********************************************
* 得到int型数组中第一次出现最小值的索引值
***********************************************/
int GetMinIntIndex(								 // 得到最小值的索引值
	int *pSignal,								 // 输入的数组指针
	int iSignalLength)							 // 输入的数据长度
{
	int iMinIndex = 0;
	int iMinNumber = 100000000;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] < iMinNumber)
		{
			iMinNumber = pSignal[i];
			iMinIndex = i;
		}
	}

	return iMinIndex;
}

// float 

/***********************************************
* 得到float型数组的最大值
***********************************************/
float GetMaxFloat(								 // 得到数据最大值
		float *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	float fMaxNumber = -100000000.0f;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > fMaxNumber)
		{
			fMaxNumber = pSignal[i];
		}
	}

	return fMaxNumber;
}

/***********************************************
* 得到float型数组的最小值
***********************************************/
float GetMinFloat(								 // 得到数据最小值
		float *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	float fMinNumber = 3e+38f;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] < fMinNumber)
		{
			fMinNumber = pSignal[i];
		}
	}

	return fMinNumber;
}

/***********************************************
* 得到float型数组的绝对值的平均值
***********************************************/
float GetMeanAbsFloat(							 // 计算数据绝对值的平均值
		float *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度   
{
    if (iSignalLength <= 0)
    {
        return fabsf(pSignal[0]);
    }

	int i = 0;
	float fSumNumber = 0.0f;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > 0.0f)
			fSumNumber = fSumNumber + pSignal[i];
		else
			fSumNumber = fSumNumber - pSignal[i];
	}

	return fSumNumber / (float)iSignalLength;
}

/***********************************************
* 得到float型数组的平均值
***********************************************/
float GetMeanFloat(								 // 得到数据平均值
		float *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度   
{
    if (iSignalLength <= 0)
    {
        return pSignal[0];
    }

	int i = 0;
	float fSumNumber = 0.0f;

	for (; i < iSignalLength; ++i)
		fSumNumber = fSumNumber + pSignal[i];

	return fSumNumber / (float)iSignalLength;
}

/***********************************************
* 得到float型数组的标准差
***********************************************/
float GetStdFloat(								 // 得到数据标准差
		float *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度 
{
    if (iSignalLength <= 1)
        return pSignal[0];

	float fSumSignal = 0.0f;
	float fSumSignalSquare = 0.0f;
	int i = 0;

    for (; i < iSignalLength; ++i)
    {
        fSumSignal = fSumSignal + (float)pSignal[i];
    }
    float avg = fSumSignal / (float)iSignalLength;

    for (i = 0; i < iSignalLength; ++i)
    {
        fSumSignalSquare += (pSignal[i] - avg) * (pSignal[i] - avg);
    }
    return sqrtf(fSumSignalSquare / (float)iSignalLength);
}

/***********************************************
* 得到float型数组的绝对值的最大值
***********************************************/
float GetMaxAbsFloat(							 // 得到数据绝对值最大值
		float *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	float fMaxNumber = 0.0f;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] > fMaxNumber && pSignal[i] > 0.0f)
		{
			fMaxNumber = pSignal[i];
		}
		else if(pSignal[i] < -fMaxNumber && pSignal[i] < 0.0f)
		{
			fMaxNumber = -pSignal[i];
		}
	}
	return fMaxNumber;
}

/***********************************************
* 得到数组中第一次出现fFindNumber的索引值
***********************************************/
int FindFirstIndex(								 // 得到索引值，返回iSignalLength表示查找失败
		float *pSignal,							 // 输入的数组指针
		int iSignalLength,						 // 输入的数据长度
		float fFindNumber)						 // 输入待索引的数据
{
	int i = 0;
	for (; i < iSignalLength; ++i)
		if (fabsf(pSignal[i] - fFindNumber) < fEps)
			return i;
	return i;
}

/***********************************************
* 返回float型数组中第一次出现最大值的索引值
***********************************************/
int GetMaxIndex(								 // 返回最大值的索引值
		float *pSignal,							 // 输入的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	float fMaxNumber = 0.0f;
	int iMaxIndex = 0;
	int i = 0;

	for (; i<iSignalLength; ++i)
	{
		if (pSignal[i] > fMaxNumber)
		{
			fMaxNumber = pSignal[i];
			iMaxIndex = i;
		}
	}

	return iMaxIndex;
}

/***********************************************
* 返回float型数组中第一次出现最小值的索引值
***********************************************/
int GetMinIndex(								 // 返回最小值的索引值
		float *pSignal,							 // 进的数据的数组指针
		int iSignalLength)						 // 输入的数据长度
{
	int iMinIndex = 0;
	float fMinNumber = 3e+38f;
	int i = 0;

	for (; i < iSignalLength; ++i)
	{
		if (pSignal[i] < fMinNumber)
		{
			fMinNumber = pSignal[i];
			iMinIndex = i;
		}
	}

	return iMinIndex;
}

/***********************************************
* 最小二乘法计算;
***********************************************/
void CalLeastSquare(
		int * pXBuf,        					 // 输入的x变量
		int * pYbuf,       						 // 输入的y变量
		int count,          					 // 数据的长度
		float *k, 								 // 斜率
		float *b)    							 // 截距                        
{
	//防止数据太短，出现无穷大
	if (count < 2)
	{
		*k = 0.0f;
		*b = (float)pYbuf[0];
		return;
	}

	float fSumX = 0.0f;
	float fSumY = 0.0f;
	float fSumMultXY = 0.0f;
	float fSumMultXX = 0.0f;

	int i = 0;
	for(i = 0; i < count; ++i)
	{
		fSumX += pXBuf[i];
		fSumY += pYbuf[i];
		fSumMultXY += pXBuf[i] * pYbuf[i];
		fSumMultXX += pXBuf[i] * pXBuf[i];
	}
	
	*k = (count * fSumMultXY - fSumX*fSumY) / (count * fSumMultXX - fSumX * fSumX);
	*b = fSumY / count - fSumX / count * (*k);
}

/*******************************************
* 实现连续小波haar变换;
*******************************************/    
void Cwt_Haar(
		const float *pSignal,					 // 进行小波变换的数据的数组指针;
		float *pTransSignal,					 // 小波变化后的数据的数组指针;
		const int iSignalLength,				 // 输入数据长度;
		const int iScale)						 // 小波变换尺度;
{
	/********************************************************************
	*运用连续小波变换的定义式进行参数计算;
	*参考地址http://wenku.baidu.com/view/237fde8571fe910ef12df81a.html;
	*haar小波滤波系数为+1和-1;
	*********************************************************************/

	// 小波变换的尺度衰减值大小，最终结果需除以dSqrtScale为真是cwt值;
	float dSqrtScale = sqrtf((float)iScale); 

	int i, j, k;

	for (i = 0; i < iSignalLength; ++i)
	{
		pTransSignal[i] = 0.0f;
		for (j = -iScale / 2; j < iScale / 2; ++j)
		{
			k = i + j;
			if (k < iSignalLength && k >= 0)
			{
				if (j < 0)
					pTransSignal[i] = pTransSignal[i] + pSignal[k];
				else
					pTransSignal[i] = pTransSignal[i] - pSignal[k];
			}

		}
		pTransSignal[i] = pTransSignal[i] / dSqrtScale;
	}
}

/*******************************************
* 实现IIR滤波器函数;
*******************************************/
void Iir_Filter(
		const float *pSignal,					 // 进行滤波的数据的数组指针;
		float *pFilterSignal,					 // 滤波后的数据的数组指针;
		const float *pCoefB,					 // iir滤波器系数b;
		const float *pCoefA,					 // iir滤波器系数a;
		const int iSignalLength,				 // 进行滤波的数据长度;
		const int iFilterLength)				 // 滤波器系数长度;
{	
	int i, j, k;

	for (i = 0; i < iSignalLength; ++i)
	{
		pFilterSignal[i] = pSignal[i] * pCoefB[0];
		for (j = 1; j < iFilterLength; ++j)
		{
			k = i - j;
			if (k >= 0)
			{
				pFilterSignal[i] = pFilterSignal[i] + pSignal[k] * pCoefB[j];
				pFilterSignal[i] = pFilterSignal[i] - pFilterSignal[k] * pCoefA[j];
			}
		}
	}
}

/*******************************************
* 实现平稳db1小波专用滤波器;滤波系数分别为1/sqrt(2) 1/sqrt(2)和-1/sqrt(2) 1/sqrt(2)
* matlab内置swt滤波器采用循环卷积，对信号进行了双边延拓，因而采用专用滤波器进行swt;
* 输入信号的长度需为2^iScale-1的整数倍;
* 原理包括周期卷积、以及滤波系数的插零（1 1 插值为1 0 1 0 再后一层插值为1 0 0 0 1 0 0 0）;
*******************************************/
void Filt_Db1(                     
		const float *pSignal,					 // 进行滤波变换的数据指针;
		float *pLowSignal,						 // 滤波信号低频分量;
		float *pHighSignal,						 // 滤波信号高频分量;
		const int iSignalLength,				 // 输入信号的数据长度;
		const int iScale)						 // 表示进行db1小波滤波的尺度;
{
	float SQRT_2 = sqrtf(2);
	int i = 0;
	int j = 0;

	int iTmpScale = 1; //存储2^iScale-1次方;
	//2^iScale-1次方;
	for (i = 1; i < iScale; ++i)
	{
		iTmpScale = iTmpScale * 2;
	}

	int iTmpIndex = 0;
	int iTmpSignalLength = iSignalLength / iTmpScale; //信号长度;

	// 参照matlab swt函数内容编写
	for (i = 0; i < iTmpScale; ++i)
	{
		for (j = 0; j < iTmpSignalLength; ++j)
		{
			iTmpIndex = iTmpScale * j + i;
	
			//周期卷积;
			if (j < iTmpSignalLength - 1)
			{
				pLowSignal[iTmpIndex] = (pSignal[iTmpIndex] + pSignal[iTmpIndex + iTmpScale]) / SQRT_2;
				pHighSignal[iTmpIndex] = (pSignal[iTmpIndex] - pSignal[iTmpIndex + iTmpScale]) / SQRT_2;
			}
			else
			{
				pLowSignal[iTmpIndex] = (pSignal[iTmpIndex] + pSignal[i]) / SQRT_2;
				pHighSignal[iTmpIndex] = (pSignal[iTmpIndex] - pSignal[i]) / SQRT_2;
			}
		}
	}
}

/***************************************
* 平稳小波db1变换的实现;
***************************************/
void Swt_Db1(
		const float *pSignal,					 // 进行小波变换的数据的数组指针;
        float *pTransSignal[],  				 // 进行平稳小波变换后的低频数据指针;
		const int iSignalLength,				 // 输入小波变换的数据长度;
		const int iScale)      					 // 平稳小波变换尺度;
{
	float *pTmpSignal = (float *)malloc(sizeof(float) * iSignalLength);
	memcpy(pTmpSignal, pSignal, sizeof(float) * iSignalLength);

	int i = 0;
	for (; i < iScale; ++i)
	{
		//swt定义，分别用高通滤波系数以及低通滤波系数进行滤波;
		//迭代运算，第二层小波系数由第一层小波系数低频部分得出;
		Filt_Db1(pTmpSignal, pTransSignal[i + 1], pTransSignal[i], iSignalLength, i + 1);
		memcpy(pTmpSignal, pTransSignal[i + 1], sizeof(float) * iSignalLength);
	}
	free(pTmpSignal);
}

/*******************************************
* 平稳db1小波逆变换滤波器;
* matlab内置swt滤波器采用循环卷积，对信号进行了双边延拓，因而采用专用滤波器进行swt;
* 输入信号的长度需为2^iScale-1的整数倍; 
*********************************************/
void ReFilt_Db1(                     
		const float *pLowSignal,				 // 滤波信号低频分量;
		const float *pHighSignal,				 // 滤波信号高频分量 ;
		float *pTransLowSignal,					 // 进行滤波变换的数据指针;
		const int iSignalLength, 				 // 输入信号的数据长度;
		const int iScale)						 // 表示进行db1小波滤波的尺度;
{
	float SQRT_2 = sqrtf(2);
	int i = 0;
	int j = 0;
	
	int iTmpScale = 1; //存储2^iScale-1次方;
	//2^iScale-1次方;
	for (i = 1; i < iScale; ++i)
	{
		iTmpScale = iTmpScale * 2;
	}

	int iTmpSignalLength = iSignalLength / iTmpScale; //信号长度;
	int iTmpIndex = 0;

	float *pTmpLowSignal = (float *)malloc(sizeof(float) * iSignalLength);
	float *pTmpHighSignal = (float *)malloc(sizeof(float) * iSignalLength);
	memset(pTmpLowSignal, 0, sizeof(float) * iSignalLength);
	memset(pTmpHighSignal, 0, sizeof(float) * iSignalLength);

	// 低频信号复原; 参照matlab iswt函数内容编写;滤波系数分别为1/sqrt(2) 1/sqrt(2), 进行补零运算，swt信号冗余;
	for (i = 0; i < iTmpScale; ++i)
	{
		for (j = 0; j < iTmpSignalLength; ++j)
		{
			iTmpIndex = j * iTmpScale + i;
			if (j % 2 == 0)
			{
			    pTmpLowSignal[iTmpIndex] = (pLowSignal[iTmpIndex]) / SQRT_2;
			}
			else
			{
				pTmpLowSignal[iTmpIndex] = (pLowSignal[iTmpIndex - iTmpScale]) / SQRT_2;
			}
		}
	}

	// 高频信号复原; 高频滤波系数为1/sqrt(2） -1/sqrt(2); 进行补零运算，swt信号冗余;
	for (i = 0; i < iTmpScale; ++i)
	{
		for (j = 0; j < iTmpSignalLength; ++j)
		{
			iTmpIndex = j * iTmpScale + i;
			if (j % 2 == 0)
			{
				pTmpHighSignal[iTmpIndex] = (pHighSignal[iTmpIndex]) / SQRT_2;
			}
			else
			{
				pTmpHighSignal[iTmpIndex] = -(pHighSignal[iTmpIndex - iTmpScale]) / SQRT_2;
			}
		}
	}

	// 高频恢复信号与低频恢复信号相加;
	for (i = 0; i < iSignalLength; ++i)
	{
		pTransLowSignal[i] = pTmpHighSignal[i] + pTmpLowSignal[i];
	}

	free(pTmpLowSignal);
	free(pTmpHighSignal);
}

/***************************************
* 平稳小波db1变换逆变换的实现;
***************************************/
void iSwt_Db1(
        float *pTransSignal[],					 // 进行平稳小波变换后的低频数据指针;
		float *pSignal,							 // 进行小波变换的数据的数组指针;
		const int iSignalLength,				 // 输入小波变换的数据长度;
		const int iScale)						 // 平稳小波变换尺度;
{	
	float *pTmpLowSignal = (float *)malloc(sizeof(float) * iSignalLength);
	memcpy(pTmpLowSignal, pTransSignal[iScale], sizeof(float) * iSignalLength);
	
	int i = 0;
	for (i = iScale-1; i >= 0; --i)
	{
		//iswt定义，分别用高通滤波系数以及低通滤波系数进行进行还原成低层次小波;
		ReFilt_Db1(pTmpLowSignal, pTransSignal[i], pSignal, iSignalLength, i + 1);
		memcpy(pTmpLowSignal, pSignal, sizeof(float) * iSignalLength);
	}
	free(pTmpLowSignal);
}

//打印LOG信息到SDCARD
void APP_LOG(char *log)
{
	/*
    const char *sigcombine_log = "/mnt/usb/Algorithm.txt";
    FILE *stream;

    if (!log)
        return;

    stream = fopen(sigcombine_log, "a+");

    if (stream == NULL)
        return;

    fprintf(stream, "%s\n", log);

    fclose(stream);
	*/

}


