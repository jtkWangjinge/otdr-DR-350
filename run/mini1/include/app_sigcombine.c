/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_sigcombine.c
* 摘    要：  otdr通道组合函数定义
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-31
*
*******************************************************************************/

#include "app_sigcombine.h"
// #include "app_math.h"
#define PI 				3.1415926f	//圆周率

// 通道组合主接口
void SignalCombine(COMBINE_INFO *pCombine_Info)
{
    //printf("iDataNum = %d\n", pCombine_Info->iDataNum);
    
	int iCombineFlag = pCombine_Info->iCombineFlag;    //!< 组合标识位，若该位为1，则对结构体内的信号进行组合;
	APP_LOG("SignalCombine start");
	
	//修改反射峰前的过冲
	changeEShot(pCombine_Info);

	// 信号传输函数
	// SigTransform(pCombine_Info);
	LogSigTransform(pCombine_Info);
	APP_LOG("SigTransform");
	
	// 输入信号分析
	SignatureAnalysis(pCombine_Info);
	APP_LOG("SignatureAnalysis");
	
	// 查找饱和区间
	FindSatuRegion(pCombine_Info);	
 	APP_LOG("FindSatuRegion");
	
	// 查找保留区间及末端位置
	FindSaveRegion(pCombine_Info);
	APP_LOG("FindSaveRegion");
	
	// 查找组合点位置并判断位置属性
	FindCombinePosition(pCombine_Info);
	APP_LOG("FindCombinePosition");
	
	//if (iCombineFlag == 1)
	{	
		// 对信号进行组合
		SigCombine(pCombine_Info);
		APP_LOG("SigCombine over !!!");
		// 通道重新优化选择
		// ChannelReChoise(pCombine_Info);
		APP_LOG("FindCombinePosition");
	}
	if (iCombineFlag == 1)
	{	
		pCombine_Info->iTaskCycle++;
	}
}

// 对数信号转换（先累加再取对数）
void SigTransform(COMBINE_INFO *pCombine_Info)
{
	//APP_LOG("------SigTransform start-----------\n");
	int i = 0;
	int iLen = pCombine_Info->iSigLen;
	int SumNum = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum;
	float *pData = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pData;
	unsigned short *pOriginalData = pCombine_Info->pOriginalData;
    unsigned int *pOriData = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pOriData;
	
	// 末端噪声计算
	int iBlackLevel = (int)floor(GetMeanUint16(pOriginalData
		+ iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 0.5f);
	float fTemp = 1.0f / (pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum + 1);
	
	// 先累加
	if ((pCombine_Info->iRealTimeFlag == 0))
	{
		if (SumNum > 0)
		{
			
			for (i = 0; i < iLen; ++i)
			{	
				pOriData[i] = (unsigned int)((pOriData[i] * SumNum + pOriginalData[i]) * fTemp);	
			}
		}
		else
		{
			for (i = 0; i < iLen; i++)
			{
				pOriData[i] = (unsigned int)pOriginalData[i];
			}
		}
		
		pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum++;
	} 
	else
	{
		// 实时模式
		for (i = 0; i < iLen; ++i)
		{
			pOriData[i] = pOriginalData[i];
		}		
		pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum = 1;
	}

    SumNum = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum;
    iBlackLevel = (int)floor(GetMeanUint32(pOriData + iLen - 200, 200) + 0.5f);
    iBlackLevel = Max((int)floor(GetMeanUint32(pOriData
		+ iLen - BLACK_LEVEL_LEN, 200) + 0.5f), iBlackLevel);
    iBlackLevel = Max((int)floor(GetMeanUint32(pOriData
		+ iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 0.5f), iBlackLevel);

	// 再取对数
	for(i = 0; i < iLen; ++i)
	{
		int iTemp = iBlackLevel - pOriData[i];
		if ((iTemp >= 1) && (iTemp < 65535))
		{
			pData[i] = fLogSig[iTemp];
		}
		else if((iTemp > -65535) && (iTemp <= -1))
		{
			pData[i] = - fLogSig[-iTemp];
		}		
		else
		{
			pData[i] = fEps;
		}
	}
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iInputBlacklevel = iBlackLevel;
}

// 对数信号转换（先取对数再累加）
void LogSigTransform(COMBINE_INFO *pCombine_Info)
{
	//APP_LOG("------SigTransform start-----------\n");
	int i = 0;
	int iLen = pCombine_Info->iSigLen;
	int SumNum = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum;
	float *pData = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pData;
	unsigned short *pOriginalData = pCombine_Info->pOriginalData;
	float *pLogData = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pLogData;
	// 末端噪声计算
	int iBlackLevel = (int)floor(GetMeanUint16(pOriginalData + iLen - 200, 200) + 0.5f);
    iBlackLevel = Max((int)floor(GetMeanUint16(pOriginalData
		+ iLen - BLACK_LEVEL_LEN, 200) + 0.5f), iBlackLevel);
    iBlackLevel = Max((int)floor(GetMeanUint16(pOriginalData
		+ iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 0.5f), iBlackLevel);
	
	// 对数数据
	for(i = 0; i < iLen; ++i)
	{
		int iTemp = iBlackLevel - pOriginalData[i];
		if ((iTemp >= 1) && (iTemp < 65535))
		{
			pData[i] = fLogSig[iTemp];
		}
		else if((iTemp > -65535) && (iTemp <= -1))
		{
			pData[i] = - fLogSig[-iTemp];
		}		
		else
		{
			pData[i] = fEps;
		}
	}
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iInputBlacklevel = iBlackLevel;

	float fTemp = 1.0f / (pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum + 1);
	
	if ((pCombine_Info->iRealTimeFlag == 0))
	{
		if (SumNum > 0)
		{
			for (i = 0; i < iLen; ++i)
			{	
				pData[i] = (pLogData[i] * SumNum + pData[i]) * fTemp;	
				pLogData[i] = pData[i];
			}
		}
		else
		{
			for (i = 0; i < iLen; ++i)
			{
				//pData[i] = pData[i];
				pLogData[i] = pData[i];
			}
		}
		
		pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum++;
	} 
	else
	{
		// 实时模式
		for (i = 0; i < iLen; ++i)
		{
			//pData[i] = pData[i];
			pLogData[i] = pData[i];
		}		
		pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum = 1;
	}

    SumNum = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSumNum;
    
}

// 计算噪声值、各个阈值，以及信号是否出现错误
void SignatureAnalysis(COMBINE_INFO *pCombine_Info)
{
    float *InputSignal = NULL;
    float fSatuThr = 0.0f;
    float fInputNoise = 0.0f;
    float fMinBlackLevel = 0.0f;
    float fSaveThr = 0.0f;
    float fSatuValue = 0.0f;
	unsigned int iSNR_Threshold = 8;

    float fMean = 0, fMax = 0;
    int iSignalLength = pCombine_Info->iSigLen;

    // 计算有效信号检测阈值
    InputSignal = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pData;
    fInputNoise = Max(2.5f * GetStdFloat(InputSignal
		+ iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN), 3.0f) / 2.0f;
	float fMaxNoise = GetMaxAbsFloat(InputSignal
		+ iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
    fSaveThr = iSNR_Threshold + Max(fInputNoise, fMaxNoise);

    // 计算信号饱和值
    fSatuValue = 5.0f * log10f((float)pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iInputBlacklevel);
    fSatuThr = fSatuValue - 1.5f;
    if (fSaveThr > fSatuThr)
    {
        fSaveThr -= 3;
    }
    if (pCombine_Info->iPulseWidth >= ENUM_PULSE_10US)
    {
        // fSaveThr = Min(fSaveThr, 8.0f);
		fSaveThr = Min(fSaveThr, fMaxNoise + 7.0f);
    }

    // 计算强度平均值
    fMean = GetMeanFloat(InputSignal, iSignalLength);

    // 计算信号最大值
    fMax = GetMaxFloat(InputSignal, iSignalLength);

    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iErrorFlag
		= ENUM_COMBINE_ERRORFLAG_NORMAL;

    // 情况1：输入信号出现击穿现象或者噪声过大，无法组合，若单单纯噪声过大问题，则错误标识位为2;	
    if ((fSatuThr - fInputNoise < 8.0f)
		&& (pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iInputBlacklevel < 45000))
    {
        pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iErrorFlag
			= ENUM_COMBINE_ERRORFLAG_BLACKBREAK;   //!< 黑电平击穿标识位;
    }
    else if (fSatuThr - fInputNoise < 10.0f)
    {
        pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iErrorFlag
			= ENUM_COMBINE_ERRORFLAG_NOISEBREAK;   //!< 噪声击穿标识位;
    }
	/*
	if ((pCombine_Info->iPulseWidth >= ENUM_PULSE_10US) && (pCombine_Info->iDataNum == 1))
	{
		pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iErrorFlag
			= ENUM_COMBINE_ERRORFLAG_NOISEBREAK;
	}
	*/
    // 情况2：ATT0整定错误导致的非线性现象;
    fMinBlackLevel = GetMaxFloat(InputSignal
		+ iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
    if (fMinBlackLevel <= fEps && fSatuValue >= 5.0f * log10f(65500))
    {
        pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iErrorFlag
			= ENUM_COMBINE_ERRORFLAG_ATT0ERROR;    //!< ATT0错误，易出现非线性;
    }

    // 对于小脉冲信号，为了获得最佳信号强度，需要调整APD电压保证处于最佳状态;
    if ((pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iInputBlacklevel > 65500)
        && (pCombine_Info->iDataNum > 1))
    {
        fInputNoise = Max(5.0f, pCombine_Info->combine_para[pCombine_Info->iDataNum - 2].fInputNoise);
    }
    //pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fInputNoise = fInputNoise;
	pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fInputNoise = fMaxNoise;
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fSatuThr = fSatuThr;
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fSatuValue = fSatuValue;
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fSaveThr = fSaveThr;
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fComGain
		= pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iAttenuation * 2.5f;//增益值与衰减值的关系;
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fMeanIntensity = fMean;
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fMaxIntensity = fMax;
}

// 查找饱和区间函数
void FindSatuRegion(COMBINE_INFO *pCombine_Info)
{
    int i = 0, Num = 0, status = 0, SatuCount = 0;
    int iSignalLength = pCombine_Info->iSigLen;
    int iSaturationLength = 0, iBlind = pCombine_Info->iBlind;
    int(*iComPosition)[2] = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComRegion;
    float *InputSignal = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pData;
    float fSatuThr = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fSatuThr;
    int iTransition = 300;			// 判断毛刺所用阈值宽度;
   
	if (iSignalLength <= 10)
	{
		iComPosition[0][0] = 0;
		iComPosition[0][1] = 10;
		return;
	}
	for (i = 10; i < iSignalLength; ++i)
    {
        switch (status)
        {
        case 0:
            if (InputSignal[i] > fSatuThr)
            {
                status = 1;
                Num = Num + 1;
                iSaturationLength += 1;
                SatuCount = 0;
                iComPosition[Num - 1][0] = i;
            }
            else
            {
                status = 0;
            }
            break;
        case 1:
            if (InputSignal[i] <= fSatuThr)
            {
                SatuCount++;
                iSaturationLength++;
                if (SatuCount > 10)       //为避免毛刺影响而添加该判断;
                {
					//若后方区域信号不够强，或最大值点小于饱和值，则认为找到饱和区末端;
                    if ((GetMaxFloat(InputSignal + i, Min(iTransition, iSignalLength - i)) <= fSatuThr) 
                        || (GetMinFloat(InputSignal + i, Min(iTransition, iSignalLength - i))
							< pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fSaveThr))
                    {
                        status = 0;
                        iComPosition[Num - 1][1] = Min(Max(iBlind, iSaturationLength - 10), iSignalLength);
                        iSaturationLength = 0;
                        SatuCount = 0;
                    }
                    else
                    {
                        SatuCount = -GetMaxIndex(InputSignal + i, Min(iTransition, iSignalLength - i));//避免重复运算，对SatuCount赋负值;
                    }
                }
            }
            else
            {
                iSaturationLength++;
                SatuCount = 0;
            }
            break;
        default:
            status = 0;
            break;
        }
        if (Num >= MAX_COMBINE_NUMBER)
            break;
    }
    if (Num == 0)
    {
		iComPosition[0][0] = 0;
		iComPosition[0][1] = 10;
        //对于无饱和区间的信号，应计算信号强度;
        return;
    }
    //对第一个饱和区进行判断并重新赋值;
    if (iComPosition[0][0] == 10)
    {
        iComPosition[0][0] = 0;
        iComPosition[0][1] = Max(10, iComPosition[0][1] + 10);
    }
    else
    {
        Num++;
        for (i = Num; i > 0; --i)//如果初始短没有饱和，则插入0
        {
            iComPosition[i][0] = iComPosition[i - 1][0];
            iComPosition[i][1] = iComPosition[i - 1][1];
        }
        iComPosition[0][0] = 0;
        iComPosition[0][1] = 0;
    }
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iCombineNum = Num;
}

// 查找保留区间和末端位置函数;
void FindSaveRegion(COMBINE_INFO *pCombine_Info)
{
    int i, iState = 0, iEndCount = 0, iSaveCount = 0, iTail = -1, iSave = -1;
    int iSignalLength = pCombine_Info->iSigLen;
    float *pInputSignal = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pData;
    float fSaveThr = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fSaveThr;
    float fEndThr = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fInputNoise;

    // 寻找末端点、保留点
    for (i = 10; i < iSignalLength; ++i)
    {
        switch (iState)
        {
            case 0:
            {
                if (pInputSignal[i] < fEndThr)
                {
                    iEndCount = 1;
                    iState = 1;
                }
                else if ((pInputSignal[i] < fSaveThr) && (iSave == -1))
                {
                    iSaveCount = 1;
                    iState = 2;
                }
                else
                {
                    iEndCount = 0;
                    iSaveCount = 0;
                    iState = 0;
                }
                break;
            }
            case 1:
            {
                if (pInputSignal[i] < fEndThr)
                {
                    iEndCount++;
                    if (iEndCount >= 10)        // 连续10个点小于EndThr视为光纤末端
                    {
                        iTail = i - 10;
                        iState = 0;
                        break;
                    }
                    iState = 1;
                }
                else
                {
                    iEndCount = 0;
                    iState = 0;
                }
                break;
            }
            case 2:
            {
                if (pInputSignal[i] < fSaveThr)
                {
                    iSaveCount++;
                    if (iSaveCount >= 10)     // 连续10个点小于fSaveThr视为保留区间
                    {
                        iSave = i - 10;
                        iState = 0;
                        break;
                    }
                    iState = 2;
                }
                else
                {
                    iSaveCount = 0;
                    iState = 0;
                }
                break;
            }
            default:
            {
                iEndCount = 0;
                iSaveCount = 0;
                iState = 0;
                break;
            }
        }
        if (iTail != -1) break;			// iTail！=-1表示已经查找到光纤末端，跳出循环
    }
    if (iTail == -1)					// 若未找到末端，则认为末端为光纤长度;
    {
        iTail = iSignalLength;
    }
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iEndPosition = iTail;
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSavePosition = iSave;
}

// 查找组合点位置函数,并判断组合类型;
void FindCombinePosition(COMBINE_INFO *pCombine_Info)
{
    int i = 0;
    int j = 0;
    int minIndex = 0, maxIndex = 0;
    int(*iComPosition)[2] = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComRegion;
    int Num = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iCombineNum;
    float delta = 0.0f;
    int iBlind = pCombine_Info->iBlind;
    int iSaveIndex = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iSavePosition;
    int iSatuIndex = 0;
    float *pData = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].pData;
    float fSatuValue = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].fSatuValue;

    if (pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iCombineNum > 0)
        iSatuIndex = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComRegion[0][1];

	int iComStyle = ENUM_COMBINE_STYLE_NORMAL;
	if (pCombine_Info->iTaskCycle >= 1)
	{
		iComStyle = pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle;
	}

    // 保留点位置靠近初始段定义为事件组合类型
	int tmpLen = Max(300, 3 * iBlind);
	tmpLen = Min(tmpLen, (int)(2500.0f / 2.56 * pCombine_Info->fSampleRating));
	if (iSaveIndex <= tmpLen)
    {
        delta = pData[iSaveIndex] - GetMinFloat(pData
			+ Min(iSaveIndex, pCombine_Info->iSigLen - iBlind), iBlind);

        // 散射饱和(属于正常组合类型)
        if ((iSatuIndex > Max(40, iBlind)) && (delta < 1.0f))
        {
            pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle
				= ENUM_COMBINE_STYLE_NORMAL;
        }
        else
        {
            // 事件饱和
            pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle
				= ENUM_COMBINE_STYLE_EVENT;
        }
    }
    else
    {
        // 从保留点位置向前推进一个盲区宽度寻找信号的最大值点
		int tmpS = Max(iSaveIndex - Max(iBlind, 50), 0);
		maxIndex = tmpS + GetMaxIndex(pData + tmpS, Max(iBlind, 50));
        delta = pData[maxIndex] - pData[iSaveIndex];
        if (delta < 1.0f)
        {
            pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle
				= ENUM_COMBINE_STYLE_NORMAL;
        }
        else
        {
            // 判断最大值位置是否在饱和区间内，若是，则将最大值点定位在该饱和区间开始位置
            // 防止组合点位置定位在事件的边缘上
            for (i = 0; i < Num; ++i)
            {
                if ((iComPosition[i][0] < maxIndex)
					&& ((iComPosition[i][1] + iComPosition[i][0]) >= maxIndex))
                {
                    maxIndex = iComPosition[i][0];
                    break;
                }
            }

            if ((maxIndex >= pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComRegion[0][1])
				&& (maxIndex > iBlind))
            {
                // 曲线接近饱和，事件类型需要再次判断
				tmpS = Max(maxIndex - 2 * iBlind, 0);
                minIndex = tmpS + GetMinIndex(pData + tmpS, 2 * iBlind);
                if (fSatuValue - pData[minIndex] < 5.0f)
                {
                    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle
						= ENUM_COMBINE_STYLE_NEEDJUDGE;
					iSaveIndex = minIndex - 5;
                }
                else
                {
                    // 继续向前推进N个盲区，防止组合点定位在事件的边缘上
                    //while ((j < 10) && (minIndex > 3 * iBlind)) //
					while ((j < 10) && (minIndex > tmpLen))
                    {
                        j++;
                        delta = pData[Max(minIndex - Min(iBlind, 50), 0)] - pData[Max(minIndex, 0)];
                        if ((delta < 0.5f) && (delta > 0))
                        {
                            iSaveIndex = minIndex - Min(iBlind, 50);
                            break;
                        }
                        else
                            minIndex = minIndex - Min(iBlind, 50);
                    }
                    if (fSatuValue - pData[minIndex] < 5.0f)
                    {
                        iSaveIndex = Max(minIndex - 2 * iBlind, 0);
                        pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle 
							= ENUM_COMBINE_STYLE_NORMAL;
                    }
                    else
                    {
                        pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle
							= ENUM_COMBINE_STYLE_NORMAL;
                    }
                }
            }
            else
            {
                pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle
					= ENUM_COMBINE_STYLE_END;
            }
        }
    }
    pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComPosition = iSaveIndex;
	if (pCombine_Info->iDataNum > 1)
	{
		pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComPosition =
			Max(pCombine_Info->combine_para[pCombine_Info->iDataNum - 2].iComPosition + 1 ,
			pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComPosition);
	}

	// 前面几组数据的组合类型是事件类型，突然在另外一个循环中是正常组合，将正常组合设置为事件类型组合
	if ((pCombine_Info->iTaskCycle >= 1) && (iComStyle == ENUM_COMBINE_STYLE_EVENT)
		&& (pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iComStyle <= ENUM_COMBINE_STYLE_NORMAL))
	{
		pCombine_Info->combine_para[pCombine_Info->iDataNum - 1].iErrorFlag = iComStyle;
	}
}

// 通道重新选择函数，对组合过程中的通道进行最优选择;
void ChannelReChoise(COMBINE_INFO *pCombine_Info)
{
	int i = 0;
	if (pCombine_Info->iTaskCycle == 0)
	{
        int Num = pCombine_Info->iDataNum;
		int ThresholdLen = 10;
		int Count = 0;
		if (Num == pCombine_Info->iTotalNum)
		{
			pCombine_Info->ReChanIndex[0] = 0;
			for (i = 1; i < Num; i++)
			{
				if (pCombine_Info->combine_para[i].iComPosition 
					< pCombine_Info->combine_para[i - 1].iComPosition + ThresholdLen) 
				{
					if ((i == 1) && (pCombine_Info->combine_para[i].iComPosition < 100)
						&& (pCombine_Info->combine_para[i].iComPosition
						== pCombine_Info->combine_para[i - 1].iComPosition + 1))
					{
						pCombine_Info->ReChanIndex[0] = 1;
					}
					else if (i == 1)
					{
						pCombine_Info->ReChanIndex[0] = 0;
					}
					else
					{
						pCombine_Info->ReChanIndex[Count] = i;
					}
				}
				else
				{
					Count++;
					pCombine_Info->ReChanIndex[Count] = i;
				}
			}
			Count++;
			pCombine_Info->iTotalNum = Count;
			for (i = 0; i < Count; ++i)
			{
				pCombine_Info->combine_para[i] = pCombine_Info->combine_para[pCombine_Info->ReChanIndex[i]];
				pCombine_Info->combine_pos[i] = pCombine_Info->combine_pos[pCombine_Info->ReChanIndex[i]];
			}
		}
	}
}

// 信号组合函数，对结构体内所有数据进行组合;
void SigCombine(COMBINE_INFO *pCombine_Info)
{
	int Num = 0, Cur_Num = 0, Next_Num = 0, Last_Num = 0, iComPosition = 0, SaveNum = 0, SavePos = 0;
	int i = 0, j = 0, k = 0, start = 0, len = 0;
	int ErrorFlag = 0,Style = 0;
	int iLen = pCombine_Info->iSigLen;
	int iBlind = pCombine_Info->iBlind;
	int iPointNum = (int)(100.0f * pCombine_Info->fSampleRating); // 40M采样率下100个点对应的当前采样率下的数据个数
	float *pCombineData = pCombine_Info->pCombineData;
	float *pInputSignal = NULL, *pCurSignal = NULL;
	float *pMidSignal[MAX_DATA_NUMBER] = { NULL };
	float Gain = 0.0f, InputMean = 0.0f, CurMean = 0.0f;
    float fTemp = 0.0f;

    // 信号数量判断;
    if (pCombine_Info->iTaskCycle == 0)
    {
        Num = pCombine_Info->iDataNum;
    }
    else
    {
        Num = pCombine_Info->iTotalNum;
    }

	// 若只存在一组信号，无需后续组合;
    if (Num == 1)
    {
        for (i = 0; i < iLen; ++i)
        {
            pCombineData[i] = pCombine_Info->combine_para[SavePos].pData[i];
        }
        return;
    }

    // 5NS组合,特殊处理
    if (pCombine_Info->iPulseWidth == 1)
    {
        SigCombine2(pCombine_Info);

        return;
    }

    APP_LOG("----1-----");
    // 如果当前信号与下一组信号都是事件类型组合，则将当前信号不进行组合
    for (i = 0; i < Num - 1; ++i)
    {
        if (((pCombine_Info->combine_para[i].iComStyle == ENUM_COMBINE_STYLE_EVENT)
			&& (pCombine_Info->combine_para[i + 1].iComStyle == ENUM_COMBINE_STYLE_EVENT))
            || ((pCombine_Info->combine_para[i].iComStyle == ENUM_COMBINE_STYLE_EVENT)
            && (pCombine_Info->combine_para[i].iComPosition < Max(iBlind, 50))))
        {
            pCombine_Info->combine_para[i].iErrorFlag = ENUM_COMBINE_ERRORFLAG_SIGNALWEEK;
        }
		/*
        printf("iErrorFlag = %d\n", pCombine_Info->combine_para[i].iErrorFlag);
        printf("iComStyle = %d\n", pCombine_Info->combine_para[i].iComStyle);
        printf("iComPosition = %d\n", pCombine_Info->combine_para[i].iComPosition);
        printf("iSavePosition = %d\n", pCombine_Info->combine_para[i].iSavePosition);
        printf("iEndPosition = %d\n", pCombine_Info->combine_para[i].iEndPosition);
        printf("iComRegion = %d\n\n", pCombine_Info->combine_para[i].iComRegion[0][1]);
		*/
    }
	/*
    printf("iErrorFlag = %d\n", pCombine_Info->combine_para[i].iErrorFlag);
    printf("iComStyle = %d\n", pCombine_Info->combine_para[i].iComStyle);
    printf("iComPosition = %d\n", pCombine_Info->combine_para[i].iComPosition);
    printf("iSavePosition = %d\n", pCombine_Info->combine_para[i].iSavePosition);
    printf("iEndPosition = %d\n", pCombine_Info->combine_para[i].iEndPosition);
    printf("iComRegion = %d\n\n", pCombine_Info->combine_para[i].iComRegion[0][1]);
    */
     APP_LOG("----2-----");
    
    // 信号强度检测，是否出现强度变弱的信号,该信号在组合中应当舍弃;
    do
    {
        ErrorFlag = 0;
        for (i = 0; i < Num - 2; ++i)
        {
            Cur_Num = i;
            Next_Num = 0;
            if (pCombine_Info->combine_para[Cur_Num].iErrorFlag > ENUM_COMBINE_ERRORFLAG_NORMAL)
            {
                continue;
            }

            if ((pCombine_Info->combine_para[Cur_Num].iSavePosition < Min(5 * iBlind, iPointNum))
                || (pCombine_Info->combine_para[Cur_Num].iComPosition < Min(5 * iBlind, iPointNum)))
            {
                pCombine_Info->combine_para[Cur_Num].iErrorFlag = ENUM_COMBINE_ERRORFLAG_SIGNALWEEK;
                continue;
            }

            for (j = i + 1; j < Num - 1; ++j)
            {
                if (pCombine_Info->combine_para[j].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
                {
                    Next_Num = j;
                    break;
                }
            }
            if (Next_Num > Cur_Num)
            {
                if (((pCombine_Info->combine_para[Next_Num].iComPosition
						< pCombine_Info->combine_para[Cur_Num].iComPosition)
                    || (abs(pCombine_Info->combine_para[Next_Num].iComPosition
						- pCombine_Info->combine_para[Cur_Num].iComPosition) < 2 * iBlind))
                    && ((pCombine_Info->iPulseWidth < ENUM_PULSE_2US)
                    || (pCombine_Info->combine_para[Next_Num].iSavePosition
						< pCombine_Info->combine_para[Cur_Num].iSavePosition)))
                {
                    ErrorFlag = 1;
                    pCombine_Info->combine_para[Next_Num].iErrorFlag = ENUM_COMBINE_ERRORFLAG_SIGNALWEEK;
                }
				// 当前要组合的信号，非常弱，不存在饱留区间
                if ((pCombine_Info->combine_para[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_EVENT) 
                    && (pCombine_Info->combine_para[Next_Num].iComStyle == ENUM_COMBINE_STYLE_EVENT))
                {
					// 噪声过大判断;
                    if ((pCombine_Info->iTaskCycle == 0)
                        || (pCombine_Info->combine_pos[Next_Num].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NOISEESTERROR))
                    {
                        ErrorFlag = 0;
                        pCombine_Info->combine_para[Next_Num].iErrorFlag = ENUM_COMBINE_ERRORFLAG_NORMAL;
                    }
                }

                // 对于弱信号，通过信号平均值来判断大小;
                if (((pCombine_Info->combine_para[Next_Num].iCombineNum == 0)
                    && (pCombine_Info->combine_para[Next_Num].fMeanIntensity
						<= pCombine_Info->combine_para[Cur_Num].fMeanIntensity)
                    && (pCombine_Info->combine_para[Next_Num].fMaxIntensity
						<= pCombine_Info->combine_para[Cur_Num].fMaxIntensity)))
                {
                    ErrorFlag = 0;
                    pCombine_Info->combine_para[Next_Num].iErrorFlag = ENUM_COMBINE_ERRORFLAG_NORMAL;
                }

                if ((pCombine_Info->combine_para[Cur_Num].iSavePosition 
					>= pCombine_Info->combine_para[Next_Num].iSavePosition)
                    || (pCombine_Info->combine_para[Cur_Num].iComRegion[0][1] 
					> pCombine_Info->combine_para[Next_Num].iComRegion[0][1]))
                {
					// 判断下一组信号强度是否比当前信号要更强;
                    if (((pCombine_Info->combine_para[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_NORMAL)
						|| (pCombine_Info->combine_para[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_NEEDJUDGE))
                        && (pCombine_Info->combine_para[Next_Num].iComPosition
							- pCombine_Info->combine_para[Next_Num].iComRegion[0][1] > 400)) 
                    {
                        InputMean = GetMeanFloat(pCombine_Info->combine_para[Next_Num].pData
							+ pCombine_Info->combine_para[Next_Num].iComRegion[0][1], 100);
                        CurMean = GetMeanFloat(pCombine_Info->combine_para[Cur_Num].pData
							+ pCombine_Info->combine_para[Next_Num].iComRegion[0][1], 100);
                        if (CurMean > InputMean)
                        {
                            ErrorFlag = 1;
                            pCombine_Info->combine_para[Next_Num].iErrorFlag = ENUM_COMBINE_ERRORFLAG_SIGNALWEEK;
                        }
                    }
                    else if ((pCombine_Info->combine_para[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_NORMAL)
						&& (pCombine_Info->iPulseWidth < ENUM_PULSE_20NS)
                        && (pCombine_Info->combine_para[Cur_Num].iComPosition 
							- pCombine_Info->combine_para[Next_Num].iComPosition > Max(40, pCombine_Info->iBlind)))
                    {
                        ErrorFlag = 1;
                        pCombine_Info->combine_para[Next_Num].iErrorFlag = ENUM_COMBINE_ERRORFLAG_SHORTWAVE;
                    }
                    else if ((pCombine_Info->combine_para[Cur_Num].iSavePosition
							- pCombine_Info->combine_para[Next_Num].iSavePosition > 400)
                        && (pCombine_Info->combine_para[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_END))
                    {
                        ErrorFlag = 1;
                        pCombine_Info->combine_para[Next_Num].iErrorFlag = ENUM_COMBINE_ERRORFLAG_ENDERROR;
                    }
                }
            }
        }
    } while (ErrorFlag == 1);

     APP_LOG("----3-----");
     
    // 小脉宽无需进行组合;
    if ((pCombine_Info->iRealTimeFlag == 1)
		&& (pCombine_Info->iPulseWidth < ENUM_PULSE_20NS)
        && ((pCombine_Info->combine_para[Num - 1].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
        || (pCombine_Info->combine_para[Num - 1].iErrorFlag > ENUM_COMBINE_ERRORFLAG_ATT0ERROR)))
    {
        for (i = 0; i < iLen; ++i)
        {
            pCombineData[i] = pCombine_Info->combine_para[Num - 1].pData[i];
        }
        return;
    }
    
     APP_LOG("----4-----");
     
#if 0
    // 对信号标识进行修正
    for (i = 0; i < Num - 1; ++i)
    {
        if ((pCombine_Info->combine_para[i].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
            && (pCombine_Info->combine_para[i + 1].iErrorFlag>ENUM_COMBINE_ERRORFLAG_NORMAL))
        {
            if (pCombine_Info->combine_para[i + 1].iComPosition<pCombine_Info->combine_para[i].iComPosition + 2 * iBlind)
            {
                if (pCombine_Info->combine_para[i + 1].iSavePosition>pCombine_Info->combine_para[i].iSavePosition)
                {
                    if (pCombine_Info->combine_para[i + 1].iComPosition>pCombine_Info->combine_para[i + 1].iComRegion[0][1] + 400)
                    {
                        pCombine_Info->combine_para[i + 1].iErrorFlag = ENUM_COMBINE_ERRORFLAG_NORMAL;
                        pCombine_Info->combine_para[i].iErrorFlag = ENUM_COMBINE_ERRORFLAG_ENDERROR;
                    }
                }
            }
        }
    }
#endif

    APP_LOG("----5-----");

    // 若正常信号小于等于1组，则无需后续计算;
    for (i = 0; i < Num; ++i)
    {
        if (pCombine_Info->combine_para[i].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
        {
            SaveNum++;
            SavePos = i;
        }
    }
    if (SaveNum < 2)
    {
        for (i = 0; i < iLen; ++i)
        {
            pCombineData[i] = pCombine_Info->combine_para[SavePos].pData[i];
        }
        return;
    }

    APP_LOG("----6-----");
    
    // 判断组合类型是否冲突,事件组合前方不应该出现其他类型组合;
    for (i = 0; i < Num - 1; ++i)
    {
        if ((pCombine_Info->combine_para[i].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
			&& (pCombine_Info->combine_para[i].iComStyle > ENUM_COMBINE_STYLE_EVENT))
        {
            for (j = i + 1; j < Num; ++j)
            {
                if ((pCombine_Info->combine_para[j].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
					&& (pCombine_Info->combine_para[j].iComStyle == ENUM_COMBINE_STYLE_EVENT))
                {
                    pCombine_Info->combine_para[i].iComStyle = ENUM_COMBINE_STYLE_EVENT;
                    break;
                }
            }
        }
    }

    APP_LOG("----7-----");
    
    // 平均模式下,第一次组合时，将组合位置等信息保留下来，作为参考信息。后续组合时，跟据此参考信息进行组合;
    if (pCombine_Info->iRealTimeFlag == 0)
    {
		// 或者动态调整APD电压情况下，组合位置重新保留;
        if (pCombine_Info->iTaskCycle < 3) 
        {
            for (i = 0; i < Num; ++i)
            {
                pCombine_Info->combine_pos[i].iComPosition = pCombine_Info->combine_para[i].iComPosition;
                pCombine_Info->combine_pos[i].iComStyle = pCombine_Info->combine_para[i].iComStyle;
                pCombine_Info->combine_pos[i].iErrorFlag = pCombine_Info->combine_para[i].iErrorFlag;
                pCombine_Info->combine_para[i].fComGain = pCombine_Info->combine_para[i].iAttenuation * 2.5f;
            }
        }
        else
        {
			// 需要根据参考信息判断是否对组合位置进行更改;
            for (i = 0; i < Num; ++i)
            {
                if ((pCombine_Info->combine_pos[i].iErrorFlag > ENUM_COMBINE_ERRORFLAG_NORMAL)
					|| (pCombine_Info->combine_para[i].iErrorFlag > ENUM_COMBINE_ERRORFLAG_NORMAL))
                {
					// 若信号有错误，不管是之前的错误，还是当前的错误，组合位置重新赋值;
                    for (j = i; j < Num; ++j)
                    {
                        pCombine_Info->combine_pos[j].iComPosition = pCombine_Info->combine_para[j].iComPosition;
                        pCombine_Info->combine_pos[j].iComStyle = pCombine_Info->combine_para[j].iComStyle;
                        pCombine_Info->combine_pos[j].iErrorFlag = pCombine_Info->combine_para[j].iErrorFlag;
                        pCombine_Info->combine_para[i].fComGain = pCombine_Info->combine_para[i].iAttenuation * 2.5f;
                    }
                    break;
                }
                else
                {
                    if ((pCombine_Info->combine_para[i].iComPosition
							< pCombine_Info->combine_pos[i].iComPosition - 100)// 组合数据变弱，组合点往前走，则使用新组合位置;
                        || ((i < Num - 1) && (pCombine_Info->combine_pos[i].iComPosition 
							< pCombine_Info->combine_para[i + 1].iComRegion[0][1] + 10)))// 组合点位置为过饱和位置;
                    {
                        pCombine_Info->combine_pos[i].iComPosition = pCombine_Info->combine_para[i].iComPosition;
                        pCombine_Info->combine_pos[i].iComStyle = pCombine_Info->combine_para[i].iComStyle;
                        pCombine_Info->combine_pos[i].iErrorFlag = pCombine_Info->combine_para[i].iErrorFlag;
                        pCombine_Info->combine_para[i].fComGain -= pCombine_Info->combine_para[Num - 1].fComGain;
                    }
                }

            }
        }
    }
    else
    {
		// 实时模式下，组合位置实时更新。对增益值也需要重新赋值;
        for (i = 0; i < Num; ++i)
        {
            pCombine_Info->combine_pos[i].iComPosition = pCombine_Info->combine_para[i].iComPosition;
            pCombine_Info->combine_pos[i].iComStyle = pCombine_Info->combine_para[i].iComStyle;
            pCombine_Info->combine_pos[i].iErrorFlag = pCombine_Info->combine_para[i].iErrorFlag;
            pCombine_Info->combine_para[i].fComGain -= pCombine_Info->combine_para[Num - 1].fComGain;
        }
    }

    APP_LOG("----8-----");
    
    // 计算各组信号之间的增益值，方便后续组合;
    for (i = 0; i < Num - 1; ++i)
    {
        Cur_Num = i;
        Next_Num = 0;

		// 若当前信号错误，无法用于判断后续信号;
        if (pCombine_Info->combine_para[Cur_Num].iErrorFlag > ENUM_COMBINE_ERRORFLAG_NORMAL)
        {
            continue;
        }
		// 寻找该信号后续第一个正常信号;
        for (j = i + 1; j < Num; ++j)
        {
            if (pCombine_Info->combine_para[j].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
            {
                Next_Num = j;
                break;
            }
        }

        if (Next_Num > Cur_Num)
        {
            iComPosition = pCombine_Info->combine_pos[Cur_Num].iComPosition;
            pCurSignal = pCombine_Info->combine_para[Cur_Num].pData;
            pInputSignal = pCombine_Info->combine_para[Next_Num].pData;

			int tmpL = Min(10, iLen - iComPosition + 5);
            if (iComPosition > 50)
            {
                InputMean = GetMeanFloat(pInputSignal + iComPosition - 5, tmpL);
                CurMean = GetMeanFloat(pCurSignal + iComPosition - 5, tmpL);
                Gain = InputMean - CurMean;

                if ((fabsf(InputMean - pInputSignal[iComPosition]) > 0.2f))
                {
                    if (pCombine_Info->iPulseWidth < ENUM_PULSE_10US)
                    {
						tmpL = Min(20, iLen - iComPosition + 10);
                        Gain = GetMeanFloat(pInputSignal + iComPosition - 10, tmpL)
                            - GetMeanFloat(pCurSignal + iComPosition - 10, tmpL);
                    }
                    else
                    {
                        Gain = pInputSignal[iComPosition] - pCurSignal[iComPosition];
                    }
                }
            }
            else
            {
                Gain = GetMeanFloat(pInputSignal + Max(iComPosition - 5, 0), tmpL)
                    - GetMeanFloat(pCurSignal + Max(iComPosition - 5, 0), tmpL);
            }

            APP_LOG("----9-----");
            
			// 当前信号无保留区间;
            if (pCombine_Info->combine_pos[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_EVENT)
            {
                continue;
            }
            else if (pCombine_Info->combine_pos[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_NORMAL)
            {
                // 需判断保留位置是否在后续信号的饱和位置和保留位置的中间，若不在中间，还需后续处理;
                if (pCombine_Info->combine_pos[Cur_Num].iComPosition
					<= pCombine_Info->combine_para[Next_Num].iComRegion[0][1] + 100)
                {
                    pCurSignal = pCombine_Info->combine_para[Cur_Num].pData;
                    pInputSignal = pCombine_Info->combine_para[Next_Num].pData;
                    iComPosition = pCombine_Info->combine_para[Next_Num].iComRegion[0][1];

                    if (iComPosition > 1000)
                    {
                        fTemp = GetMinFloat(pCurSignal + iComPosition - 100, 100)
							- pCombine_Info->combine_para[Cur_Num].fInputNoise;
                    }
                    if ((fTemp > 5) && (pCombine_Info->combine_para[Next_Num].iEndPosition
						- pCombine_Info->combine_para[Next_Num].iComRegion[0][1] > 2000)
                        && (iComPosition - pCombine_Info->combine_pos[Cur_Num].iComPosition < 5000))
                    {
                        Style = 1;
                        pCombine_Info->combine_pos[Cur_Num].iComPosition
							= pCombine_Info->combine_para[Next_Num].iComRegion[0][1];
                    }
                    else if ((pCombine_Info->combine_para[Next_Num].iEndPosition
						- pCombine_Info->combine_para[Next_Num].iComRegion[0][1] < 400)
                        && (pCombine_Info->combine_para[Next_Num].iEndPosition
                        - pCombine_Info->combine_para[Cur_Num].iEndPosition < 400))
                    {
                        Style = 2;
                        pCombine_Info->combine_para[Cur_Num].iComPosition
							= pCombine_Info->combine_para[Next_Num].iComRegion[0][1];
                        pCombine_Info->combine_pos[Cur_Num].iComPosition
							= pCombine_Info->combine_para[Next_Num].iComRegion[0][1];
                        iComPosition = pCombine_Info->combine_pos[Cur_Num].iComPosition;

						int tmpL = Min(10, iLen - iComPosition + 5);
                        if (iComPosition > 50)
                        {
                            InputMean = GetMeanFloat(pInputSignal + iComPosition - 5, tmpL);
                            CurMean = GetMeanFloat(pCurSignal + iComPosition - 5, tmpL);
                            Gain = InputMean - CurMean;

                            if (fabsf(InputMean - pInputSignal[iComPosition]) > 0.2f)
                            {
								tmpL = Min(20, iLen - iComPosition + 10);
                                Gain = GetMeanFloat(pInputSignal + iComPosition - 10, tmpL)
                                    - GetMeanFloat(pCurSignal + iComPosition - 10, tmpL);
                            }
                        }
                        else
                        {
                            Gain = GetMeanFloat(pInputSignal + Max(iComPosition - 5, 0), tmpL)
								- GetMeanFloat(pCurSignal + Max(iComPosition - 5, 0), tmpL);
                        }
                        pCombine_Info->combine_para[Next_Num].fComGain
							= pCombine_Info->combine_para[Cur_Num].fComGain - Gain;// 计算组合信号的增益值;
                    }
                    else
                    {
                        Style = 3;
                        pCombine_Info->combine_para[Next_Num].fComGain
							= pCombine_Info->combine_para[Cur_Num].fComGain - Gain;// 计算组合信号的增益值;
                    }

                    APP_LOG("----10-----");
                     
                }
                else//正常的保留区间;
                {
                    Style = 4;
					int tmpL = Min(10, iLen - iComPosition + 5);
                    if (iComPosition > 50)
                    {
                        if (pCombine_Info->fSampleRating >= 32.0f)
                        {
                            InputMean = GetMeanFloat(pInputSignal + iComPosition - 5, tmpL);
                            CurMean = GetMeanFloat(pCurSignal + iComPosition - 5, tmpL);
                            Gain = InputMean - CurMean;
                        }
                        else
                        {
							tmpL = Min(20, iLen - iComPosition + 10);
                            InputMean = GetMeanFloat(pInputSignal + iComPosition - 10, tmpL);
                            CurMean = GetMeanFloat(pCurSignal + iComPosition - 10, tmpL);
                            Gain = InputMean - CurMean;
                        }
                        if (fabsf(InputMean - pInputSignal[iComPosition]) > 0.2f)
                        {
							tmpL = Min(30, iLen - iComPosition + 15);
                            Gain = GetMeanFloat(pInputSignal + iComPosition - 15, tmpL)
                                - GetMeanFloat(pCurSignal + iComPosition - 15, tmpL);
                        }
                    }
                    else
                    {
                        Gain = GetMeanFloat(pInputSignal + Max(iComPosition - 5, 0), tmpL)
                            - GetMeanFloat(pCurSignal + Max(iComPosition - 5, 0), tmpL);
                    }
                    pCombine_Info->combine_para[Next_Num].fComGain
						= pCombine_Info->combine_para[Cur_Num].fComGain - Gain;// 计算组合信号的增益值;

                    APP_LOG("----11-----");
                    
                }
            }
            else if (pCombine_Info->combine_pos[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_END)// 保留区间位于末端;
            {
                Style = 5;
                pCombine_Info->combine_para[Next_Num].fComGain
					= pCombine_Info->combine_para[Cur_Num].fComGain - Gain;// 计算组合信号的增益值;
            }
            else if (pCombine_Info->combine_pos[Cur_Num].iComStyle == ENUM_COMBINE_STYLE_NEEDJUDGE)// 保留区间需要再判断;
            {
                // 需要判断组合点位置是否在后续信号的饱和区;
                Style = 6;
                if (pCombine_Info->combine_pos[Cur_Num].iComPosition
					< pCombine_Info->combine_para[Next_Num].iComRegion[0][1] + 100)
                {
                    Style = 7;
                    pCombine_Info->combine_para[Cur_Num].iComPosition
						= Max(iComPosition, pCombine_Info->combine_para[Next_Num].iComRegion[0][1] + 10);
                    pCombine_Info->combine_pos[Cur_Num].iComPosition
						= Max(iComPosition, pCombine_Info->combine_para[Next_Num].iComRegion[0][1] + 10);
                    iComPosition = pCombine_Info->combine_pos[Cur_Num].iComPosition;
                    pCurSignal = pCombine_Info->combine_para[Cur_Num].pData;
                    pInputSignal = pCombine_Info->combine_para[Next_Num].pData;

					int tmpL = Min(10, iLen - iComPosition + 5);
                    if (iComPosition > 50)
                    {
                        InputMean = GetMeanFloat(pInputSignal + iComPosition - 5, tmpL);
                        CurMean = GetMeanFloat(pCurSignal + iComPosition - 5, tmpL);
                        Gain = InputMean - CurMean;
                        if (fabsf(InputMean - pInputSignal[iComPosition]) > 0.3f)
                        {
							tmpL = Min(20, iLen - iComPosition + 10);
                            Gain = GetMeanFloat(pInputSignal + iComPosition - 10, tmpL)
								- GetMeanFloat(pCurSignal + iComPosition - 10, tmpL);
                        }
                    }
                    else
                    {
                        Gain = GetMeanFloat(pInputSignal + Max(iComPosition - 5, 0), tmpL)
							- GetMeanFloat(pCurSignal + Max(iComPosition - 5, 0), tmpL);
                    }
                }
                
                APP_LOG("----12-----");
                
                pCombine_Info->combine_para[Next_Num].fComGain
					= pCombine_Info->combine_para[Cur_Num].fComGain - Gain;
            }

			// 噪声判断，若最后一组信号噪声比前一组大过多，且两组信号末端接近，则不使用该组信号，或者重新计算增益值;
            if (Next_Num == Num - 1)
            {
                // 去掉噪声判断
                if (pCombine_Info->combine_pos[Next_Num].iComPosition
					- pCombine_Info->combine_pos[Cur_Num].iComPosition < 400)
                {
                    if ((Style != 4) && (Style != 6))
                    {
                        pCombine_Info->combine_para[Next_Num].fComGain
							= pCombine_Info->combine_para[Cur_Num].fComGain - Gain;
                    }
                }
            }

            APP_LOG("----13-----");
        }
        else
        {
			// 若信号后面不存在正常信号，则直接退出循环;
            break;
        }
    }

    APP_LOG("----14-----");
    
    // 将所有通道的增益减掉最小增益值，归一化增益;
    for (i = Num; i > 0; --i)
    {
        if (pCombine_Info->combine_para[i - 1].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
        {
            // 去掉噪声的判断，直接归一化增益值 
            for (j = 0; j < i; ++j)
            {
                pCombine_Info->combine_para[j].fComGain
					= pCombine_Info->combine_para[j].fComGain
					- pCombine_Info->combine_para[i - 1].fComGain;
            }
            break;
        }
    }

    APP_LOG("----15-----");
    
    /***********************信号组合************************/

    // 首先将所有的通道数据复制到中间变量上，以避免改变通道原始数据;
    for (i = 0; i < Num; ++i)
    {
        pMidSignal[i] = (float *)calloc(iLen, sizeof(float));
        if (pCombine_Info->combine_pos[i].iErrorFlag > ENUM_COMBINE_ERRORFLAG_NORMAL)
        {
            memcpy(pMidSignal[i], pCombine_Info->combine_para[i].pData, iLen * sizeof(float));
        }
        else
        {
			// 对中间变量赋值，并加上增益值，以方便后续计算;
            for (k = 0; k < iLen; ++k)
            {
                pMidSignal[i][k] = pCombine_Info->combine_para[i].pData[k]
					+ pCombine_Info->combine_para[i].fComGain;
            }
        }
    }

    APP_LOG("----16-----");
    
    for (i = 0; i < Num - 1; ++i)
    {
        Cur_Num = i;
        Next_Num = 0;
		// 若当前信号错误，无法用于判断后续信号;
        if (pCombine_Info->combine_pos[Cur_Num].iErrorFlag > ENUM_COMBINE_ERRORFLAG_NORMAL)
        {
            continue;
        }
        for (j = i + 1; j < Num; ++j)
        {
            if (pCombine_Info->combine_pos[j].iErrorFlag == ENUM_COMBINE_ERRORFLAG_NORMAL)
            {
                Next_Num = j;
                break;
            }
        }
        if (Next_Num > Cur_Num)
        {
            if (pCombine_Info->combine_pos[Cur_Num].iComStyle > ENUM_COMBINE_STYLE_EVENT)
            {
                // 将前面保留区间信号复制给后面信号;
                memcpy(pMidSignal[Next_Num], pMidSignal[Cur_Num],
					pCombine_Info->combine_pos[Cur_Num].iComPosition * sizeof(float));
                // 为了避免出现信号组合的断层，保留位置往后需判断是否满足组合条件
                if ((iBlind / pCombine_Info->fSampleRating <= 5.0f)
					&& (pCombine_Info->iSigLen / pCombine_Info->fSampleRating < 1300.0f))
                {
                    for (j = pCombine_Info->combine_pos[Cur_Num].iComPosition; j < iLen; ++j)
                    {
                        if (pMidSignal[Next_Num][j] < pMidSignal[Cur_Num][j])
                        {
                            pMidSignal[Next_Num][j] = pMidSignal[Cur_Num][j];
                        }
                        else
                            break;
                    }
                }
                Last_Num = Next_Num;
                // 对饱和的事件进行赋值;
                for (j = 1; j < pCombine_Info->combine_para[Next_Num].iCombineNum; ++j)
                {
                    start = pCombine_Info->combine_para[Next_Num].iComRegion[j][0];
                    len = start + pCombine_Info->combine_para[Next_Num].iComRegion[j][1];
                    for (k = start; k < len; ++k)
                    {
                        if (pMidSignal[Cur_Num][k] > pMidSignal[Next_Num][k])
                        {
                            pMidSignal[Next_Num][k] = pMidSignal[Cur_Num][k];
                        }
                    }
                }
				// 如果饱和区大于保留区，则需要判断两者中间是否存在事件饱和;
                if (pCombine_Info->combine_pos[Cur_Num].iComPosition
					< pCombine_Info->combine_para[Next_Num].iComRegion[0][1])
                {
                    for (j = 1; j < pCombine_Info->combine_para[Cur_Num].iCombineNum; ++j)
                    {
                        start = pCombine_Info->combine_para[Cur_Num].iComRegion[j][0];
                        len = start + pCombine_Info->combine_para[Cur_Num].iComRegion[j][1];
                        if ((start > pCombine_Info->combine_pos[Cur_Num].iComPosition)
							&& (start <= pCombine_Info->combine_para[Next_Num].iComRegion[0][1]))
                        {
                            for (k = start; k < len; ++k)
                            {
                                if (pMidSignal[Cur_Num][k] >= pMidSignal[Next_Num][k])
                                {
                                    pMidSignal[Next_Num][k] = pMidSignal[Cur_Num][k];
                                }
                            }
                        }
                    }
                }
            }
            else
            {
				// 若不存在保留区，则只需要对饱和事件进行赋值;
                Last_Num = Next_Num;
                for (j = 0; j < pCombine_Info->combine_para[Next_Num].iCombineNum; ++j)
                {
                    start = pCombine_Info->combine_para[Next_Num].iComRegion[j][0];
                    len = start + pCombine_Info->combine_para[Next_Num].iComRegion[j][1];
                    for (k = start; k < len; ++k)
                    {
                        if (pMidSignal[Cur_Num][k] >= pMidSignal[Next_Num][k])
                        {
                            pMidSignal[Next_Num][k] = pMidSignal[Cur_Num][k];
                        }
                    }
                }
            }
        }
        else
        {
            break;
        }
    }

    APP_LOG("----17-----");
    
	// 累加采集无需进行滤波,由于滤波耗时较长，暂时不进行滤波;
    if ((pCombine_Info->iRealTimeFlag < 2) && (Num - 1 == Last_Num))
    {
        for (i = 0; i < iLen; ++i)
        {
            pCombineData[i] = pMidSignal[Last_Num][i];
        }
    }

    APP_LOG("----18-----");
    
    for (i = 0; i < Num; ++i)
    {
        free(pMidSignal[i]);
    }
}

#if 1

// 5NS进行组合
void SigCombine2(COMBINE_INFO *pCombine_Info)
{	
	int state = 0, istart = 0, iend  = 0, max_index = 500, flag = 0, count = 0;
	int j = 0, i = 0;
    float MaxVar =0.0f, gain = 0.0f;
	float* Signal1 = pCombine_Info->combine_para[pCombine_Info->iTotalNum - 2].pData;
	float* Signal2 = pCombine_Info->combine_para[pCombine_Info->iTotalNum - 1].pData;

	MaxVar = GetMaxFloat(pCombine_Info->combine_para[pCombine_Info->iTotalNum-1].pData 
		+ pCombine_Info->iSigLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 5.0f;
	gain = GetMeanFloat(Signal2 + Min(490, pCombine_Info->iSigLen - 20), 20) 
		 - GetMeanFloat(Signal1 + Min(490, pCombine_Info->iSigLen - 20), 20);

	for ( j = 0; j < 500; ++j)
	{
		Signal2[j] = Signal1[j] + gain;
	}
	
	// 排除第一个反射峰
	for(i = 500; i < pCombine_Info->iSigLen; ++i)
	{
		// 事件区间查找
	    switch(state)
		{
	        case 0:
	            if (Signal2[i] > MaxVar)
	            {
	                istart = i;
	                state = 1;
	            }
				break;
	        case 1:
	            if (Signal2[i] < MaxVar)
	            {	            	
	                iend = i;
					count++;
					if(count >= 5)
					{
						iend = iend -count + 1;
						flag = 1;
						state = 0;
					}
					else
					{
						state = 1;
					}
	            }
				else
					count = 0;
				break;
			default:
	              break;
	    }
		if((flag == 1) && (count >= 5))
		{
			
			max_index = GetMaxIndex(Signal2 + istart,iend - istart + 1) + istart;
	
			gain = GetMeanFloat(Signal2 + Max(max_index - 180, 0), 40)
				- GetMeanFloat(Signal1 + Max(max_index - 180, 0), 40);

			int tmpS = Max(max_index - 200, 0);
			int tmpE = Min(max_index + 2000, pCombine_Info->iSigLen);
			for(j = tmpS; j < tmpE; ++j)
			{
				Signal2[j] = Signal1[j] + gain;
			}
			count = 0;
			flag = 0;
			i += 2000;
		}
	}
	memcpy(pCombine_Info->pCombineData, Signal2, pCombine_Info->iSigLen * sizeof(float));
}

// 局部滑动平均
void  Overlapping_averages(float* Signal, int Len, int Mark, int AvgNum)
{
	int i,j;
	float sum;
	for(i = Mark - 200; i < Mark + 200; ++i)
	{	
		sum  = 0.0f;
		for(j = - AvgNum / 2; j <= AvgNum/ 2; j++)
		{
			sum += Signal[Min(Max(i + j, 0), Len)];
		}
		Signal[i] = sum / AvgNum;	
	}
}

// 曲线平滑(5点法)
void CurveSmooth(float* Signal, int Len, int Mark)
{
	int iStart = Max(Mark - 200, 2);
	int iEnd = Min(Mark + 200, Len - 2);
	int i = 0;
	for(i = iStart; i < iEnd; ++i)
		Signal[i] = (Signal[i - 2] * 5+ Signal[i - 1] * 4
			+ Signal[i] * 2 + Signal[i + 1] * 4 + Signal[i + 2] * 5) / 20;
}

#if 1
void changeEShot(COMBINE_INFO *pCombine_Info)
{
	unsigned short *vCombDataReturn = pCombine_Info->pOriginalData;
	int Len = pCombine_Info->iSigLen;
	int ShootCount = 0;
	int Jumpflag = 0;
	//vector<int>ShootPoint;
	int ShootPoint[100] = { 0 };
	int Blind = pCombine_Info->iBlind;
	if ((Len <= BLACK_LEVEL_LEN) || (Len <= 2 * Blind))
	{
		printf("length too short !!!\n");
		return;
	}
	int wave = pCombine_Info->iWave;

#define SHOT_THR                               6000 
#define DATA_STD_TIMES                          1     
#define ABS_DATA_ERROR_MULT                     1
	//查找可能的过冲点
	int i = 0;
	int j = 0;
	unsigned short maxData1 = 0;
	unsigned short maxData2 = 0;
	int maxDataIndex1 = 0;
	int maxDataIndex2 = 0;
	int higherCount = 0;
	unsigned short minNoise = GetMinUint16(vCombDataReturn + Len - 250, 250);
	for (i = 2 * Blind; i < Len - BLACK_LEVEL_LEN; i++)
	{
		if (Jumpflag > 0)
		{
			Jumpflag--;
			continue;
		}
		if (ShootCount >= 100)
		{
			break;
		}

		if (vCombDataReturn[i] < SHOT_THR)
		{
			/*
			if (vCombDataReturn[i] - vCombDataReturn[i - 1] <= 0)
			{
				int Jumpcount = 0;
				int q = i;
				for (q = i; q < Len; q++)
				{
					if (vCombDataReturn[q] < SHOT_THR / 40 * 39)
					{
						Jumpcount++;
					}
					else
					{
						break;
					}
				}
				Jumpflag = Jumpcount;
				continue;
			}
			*/
			ShootCount++;
			if (i < 30)
			{
				maxData1 = GetMaxUint16(vCombDataReturn, i);
				maxDataIndex1 = GetMaxUint16Index(vCombDataReturn, i);
			}
			else
			{
				maxData1 = GetMaxUint16(vCombDataReturn + i - 30, 30);
				maxDataIndex1 = GetMaxUint16Index(vCombDataReturn + i - 30, 30) + i - 30;
			}

			if (vCombDataReturn[maxDataIndex1] != maxData1)
			{
				printf("index wrong !!!\n");
			}
			maxData2 = GetMaxUint16(vCombDataReturn + maxDataIndex1, i);
			maxDataIndex2 = GetMaxUint16Index(vCombDataReturn + maxDataIndex1, i) + maxDataIndex1;

			if (maxDataIndex2 - maxDataIndex1 > 2 && maxData2 - maxData1 < 5)
			{
				ShootPoint[ShootCount - 1] = maxDataIndex2;
			}
			else
			{
				ShootPoint[ShootCount - 1] = maxDataIndex1;
			}
			higherCount = 0;
			int tmpE = Min(Len - BLACK_LEVEL_LEN, i + 16 * Blind);
			for (j = i; j < tmpE; j++)
			{
				if (vCombDataReturn[j] < SHOT_THR / 40 * 39)//24bit
				{
					higherCount++;
				}
				else
				{
					break;
				}
			}
			Jumpflag = higherCount <= 5 ? higherCount * 2 : higherCount + 10;
		}
	}
	if (ShootCount > 0)
	{
		//在检测到可能的过冲点后，初始化并设置一些参数
		int extend[] = { 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 };//最大10个波长
		float stdIvector[10] = { 0.0f };
		for (i = 1; i < 11; i++)
		{
			//需考虑数据位数变化
			stdIvector[i - 1] = GetStdUint16(vCombDataReturn + Len - (i + 1) * 10, 10) / DATA_STD_TIMES;
		}
		float stdI = GetMaxFloat(stdIvector, 10);
		float maxDelta = (float)(GetMaxUint16(vCombDataReturn + Len - 250, 250) - minNoise);
		unsigned short *Output = (unsigned short *)calloc(Len, sizeof(unsigned short));
		memcpy(Output, vCombDataReturn, Len * sizeof(unsigned short));

		float varThr = stdI;
		float stableThr = stdI / 3.0f;
		int StartPoint[100] = { 0 };
		int overindex[100] = { 0 };
		float meanValue = 0.0f;
		float meanValue2 = 0.0f;
		float stdMean = 0.0f;
		int ss = 0;
		int ExceptionFlag = 0;
		int z = 0;
		for (z = 0; z < ShootCount; z++)
		{
			//printf("ShootPoint[%d] = %d\n", z, ShootPoint[z]);

			if (ShootPoint[z] <= 10 || ShootPoint[z] > Len - 256)
			{
				continue;
			}
			if (z > 0 && ShootPoint[z] == ShootPoint[z - 1])
			{
				continue;
			}
			int v = ShootPoint[z];
			int varLen = Min(500, v - 1);
			int iStep = 10;
			int VarCount = (int)floor(varLen / iStep);
			float varValue[100][50] = { { 0.0f } };
			int multi = ShootCount - z + 1;
			int similarCount = 0;

			if (stdI < 5.0f)
			{
				if (ShootCount > 3)
				{
					varThr = 4 * stdI;
				}
				else
				{
					varThr = 1.5f * stdI;
				}
			}
			else if (stdI < 25.0f)
			{
				varThr = Min(multi * stdI, 20.0f);
			}
			else
			{
				varThr = Min(multi * stdI, 30.0f);
			}

			//往前找平坦区域
			for (i = 0; i < VarCount; i++)
			{
				//需考虑数据位数变化
				varValue[z][i] = GetStdUint16(vCombDataReturn + v - (i + 1) * iStep + 1, iStep) / DATA_STD_TIMES;
				if (i > 0 && (varValue[z][i - 1] < varThr) && (varValue[z][i] < varThr)
					&& (fabsf(varValue[z][i] - varValue[z][i - 1]) < multi * 0.75f * stableThr))
				{
					similarCount++;
				}
				else
				{
					similarCount = 0;
				}
				if (similarCount >= 2)
				{
					StartPoint[z] = Max(v - (i - 2) * iStep - 1, 0);
					break;
				}
			}

			//如果没有找到，可能是受高频（160MHz）噪声的影响，使用周期的整数倍的数据进行计算
			if ((i == VarCount) && (similarCount < 2))
			{
				iStep = 16;
				VarCount = (int)floor(varLen / iStep);
				varThr = 5.0f * stdI;
				for (i = 0; i < VarCount; i++)
				{
					//需考虑数据位数变化
					varValue[z][i] = GetStdUint16(vCombDataReturn + v - (i + 1) * iStep + 1, iStep) / DATA_STD_TIMES;
					if (i > 0 && (varValue[z][i - 1] < varThr) && (varValue[z][i] < varThr)
						&& (fabsf(varValue[z][i] - varValue[z][i - 1]) < multi * 0.75f * stableThr))
					{
						similarCount++;
					}
					else
					{
						similarCount = 0;
					}
					if (similarCount >= 2)
					{
						StartPoint[z] = Max(v - (i - 2) * iStep - 1, 0);
						break;
					}
				}
			}
			//如果还是没有找到，增大阈值
			if ((i == VarCount) && (similarCount < 2))
			{
				iStep = 10;
				VarCount = (int)floor(varLen / iStep);
				varThr = Max(5.0f * stdI, 4.0f * stdI + 1.0f);
				stableThr *= 1.5f;
				for (i = 0; i < VarCount; i++)
				{
					//需考虑数据位数变化
					varValue[z][i] = GetStdUint16(vCombDataReturn + v - (i + 1) * iStep + 1, iStep) / DATA_STD_TIMES;
					if (i > 0 && (varValue[z][i - 1] < varThr) && (varValue[z][i] < varThr)
						&& (fabsf(varValue[z][i] - varValue[z][i - 1]) < multi * 0.75f * stableThr))
					{
						similarCount++;
					}
					else
					{
						similarCount = 0;
					}
					if (similarCount >= 2)
					{
						StartPoint[z] = Max(v - (i - 2) * iStep - 1, 0);
						break;
					}
				}
			}
			
			overindex[z] = i;

			//特殊情况处理
			if (StartPoint[z] == 0 || ShootPoint[z] - StartPoint[z] < 9)
			{
				if (ShootPoint[z] == StartPoint[z] && ShootPoint[z] >= 2
					&& ((vCombDataReturn[ShootPoint[z]] - vCombDataReturn[ShootPoint[z] - 2] > 20)
						|| (vCombDataReturn[ShootPoint[z]] - vCombDataReturn[ShootPoint[z] - 1] > 20)))
				{
					Output[ShootPoint[z]] = vCombDataReturn[ShootPoint[z] - 2] - 2;
				}
				else if ((pCombine_Info->iPulseWidth == ENUM_PULSE_5NS) && (ShootPoint[z] > 50)
					&& (i == VarCount) && (StartPoint[z] == 0) && (ShootPoint[z] < 100))
				{
					int sP = ShootPoint[z] - 2 * Blind;
					int eP = ShootPoint[z] + 2 * Blind;
					int fm = sP;
					int sPos = sP;
					int ePos = eP;
					int tmpC = 0;
					for (fm = ShootPoint[z]; fm > sP; --fm)
					{
						if (vCombDataReturn[fm] < minNoise - 5)
						{
							tmpC++;
							if (tmpC >= 5)
							{
								sPos = fm + 4;
								break;
							}
						}
						else
						{
							tmpC = 0;
						}
					}
					tmpC = 0;
					for (fm = ShootPoint[z]; fm < eP; ++fm)
					{
						if (vCombDataReturn[fm] < minNoise - 5)
						{
							tmpC++;
							if (tmpC >= 5)
							{
								ePos = fm - 4;
								break;
							}
						}
						else
						{
							tmpC = 0;
						}
						
					}
					if (sPos == sP || ePos == eP)
					{
						continue;
					}
					int tmpD = vCombDataReturn[ePos] - vCombDataReturn[sPos];
					tmpC = 1;
					int tmplen = ePos - sPos;
					for (fm = sPos + 1; fm < ePos; ++fm)
					{
						Output[fm] = vCombDataReturn[sPos] + tmpC * tmpD / tmplen;
						tmpC++;
					}
					continue;
				}
				continue;
			}

			if (z > 0 && ShootPoint[z] - ShootPoint[z - 1] < 70)
			{
				if (ShootPoint[z] - ShootPoint[z - 1] < 5)
				{
					meanValue = GetMeanUint16(Output + ShootPoint[z - 1], ShootPoint[z] - ShootPoint[z - 1] + 1);
				}
				else
				{
					int itmpS = Max(ShootPoint[z - 1] - 5, 0);
					meanValue = GetMeanUint16(Output + itmpS, ShootPoint[z - 1] + 1 - itmpS);
				}
				if (vCombDataReturn[ShootPoint[z]] - meanValue > maxDelta / 2)
				{
					int ss1 = ShootPoint[z];
					int ss2 = ShootPoint[z];
					int k = 1;
					for (k = 1; k < iStep; k++)
					{
						if (vCombDataReturn[ShootPoint[z] + k] - meanValue > maxDelta / 2 + 5)
						{
							ss1 = ShootPoint[z] + k;
						}
						else
						{
							if (meanValue - vCombDataReturn[ShootPoint[z] + k] > maxDelta / 2 + 5)
							{
								break;
							}
						}
					}
					
					for (k = 1; k < iStep; k++)
					{
						if (ShootPoint[z] - k < 0)
						{
							printf("index wrong < iStep !!!\n");
							break;
						}
						if (vCombDataReturn[ShootPoint[z] - k] - meanValue > maxDelta / 2 + 5)
						{
							ss2 = ShootPoint[z] - k;
						}
						else
						{
							if (meanValue - vCombDataReturn[ShootPoint[z] - k] > maxDelta / 2 + 5)
							{
								break;
							}
						}
					}
					ShootPoint[z] = ss1;
					StartPoint[z] = ss2;
					if (StartPoint[z] == ShootPoint[z])
					{
						Output[ShootPoint[z]] = vCombDataReturn[Max(ShootPoint[z] - 2, 0)] - 2;
					}
					else
					{
						int L = Max(ShootPoint[z] - StartPoint[z] + 1, 0);
						if (overindex[z - 1] < 2)
						{
							stdMean = stdI;
						}
						else
						{
							int iS = Max(overindex[z - 1] - 2, 0);
							stdMean = GetMeanFloat(varValue[z - 1] + iS, overindex[z - 1] + 1 - iS);
						}
						unsigned short *tmp = (unsigned short *)calloc(L, sizeof(unsigned short));
						Gaussian_O10(tmp, L, meanValue, stdMean, maxDelta / 2);
						memcpy(Output + StartPoint[z], tmp, L * sizeof(unsigned short));
						free(tmp);
						tmp = NULL;
					}
				}
				continue;
			}

			//精确定位过冲区域
			/*
			meanValue = Math_Base<vector<unsigned int>::iterator >::calSigMean
				(vCombDataReturn.begin() + max(StartPoint[z] - 29, 0), vCombDataReturn.begin() + StartPoint[z] + 1);
			stdMean = Math_Base<vector<float>::const_iterator >::calSigMean
				(varValue[z].begin() + max(i - 2, 0), varValue[z].begin() + i + 1);
				*/
			int tS = Max(StartPoint[z] - 29, 0);
			meanValue = GetMeanUint16(vCombDataReturn + tS, StartPoint[z] - tS + 1);
			stdMean = GetMeanFloat(varValue[z] + Max(i - 2, 0), i - Max(i - 2, 0) + 1);
			stdMean = Min(stdMean, 1.3f * stdI);

			if (stdI > 10 && stdI < 15 && wave != 1)
			{
				int smoothcount = 0;
				int firstpick[6] = { 0 };
				int q = 1;
				for (q = 1; q < Min(6, VarCount); q++)
				{
					if (varValue[z][q] < stdI)
					{
						smoothcount++;
						firstpick[q] = q;
					}
					else
					{
						if (varValue[z][q] == 0)
						{
							break;
						}
					}
				}
				if (smoothcount >= 3)
				{
					ExceptionFlag = 1;
					int indextmp = firstpick[0];
					meanValue2 = GetMeanUint16(vCombDataReturn + ShootPoint[z] - 10 * (indextmp + 1), 10);
				}
			}

			int CapThr = 25 * ABS_DATA_ERROR_MULT;
			if (stdI < 5)
			{
				CapThr = (int)(maxDelta / 2 + 3);
			}
			int minPosition1 = 0;
			unsigned short vallyIter1 = 0;
			vallyIter1 = GetMaxUint16(vCombDataReturn + StartPoint[z], ShootPoint[z] + 1 - StartPoint[z]);
			minPosition1 = StartPoint[z] + GetMaxUint16Index(vCombDataReturn + StartPoint[z], ShootPoint[z] + 1 - StartPoint[z]);
			if (fabsf(vallyIter1 - meanValue) < CapThr)
			{
				continue;
			}
			else
			{
				int sp = minPosition1;
				if (minPosition1 < ShootPoint[z])
				{
					int minPosition2 = 0;
					unsigned short vallyIter2 = 0;
					vallyIter2 = GetMaxUint16(vCombDataReturn + minPosition1 + 1, ShootPoint[z] - minPosition1);
					minPosition2 = minPosition1 + 1 + 
						GetMaxUint16Index(vCombDataReturn + minPosition1 + 1, ShootPoint[z] - minPosition1);
					if (vallyIter2 - meanValue > CapThr || minPosition2 - minPosition1 > 64)
					{
						sp = minPosition2;
					}
				}
				ShootPoint[z] = sp;
			}
			ss = ShootPoint[z];

			int k = 1;
			for (k = 1; k < extend[wave]; k++)
			{
				if (vCombDataReturn[ShootPoint[z] + k] - meanValue > maxDelta / 2)
				{
					ss = ShootPoint[z] + k;
				}
				else
				{
					if (meanValue - vCombDataReturn[ShootPoint[z] + k] > 60)
					{
						break;
					}
				}
			}
			ShootPoint[z] = ss;
			if (ShootPoint[z] > extend[wave] + 1)
			{
				if ((ShootPoint[z] - StartPoint[z] < extend[wave] + 1) && wave != 0)
				{
					int k = ShootPoint[z] - extend[wave];
					for (k = ShootPoint[z] - extend[wave]; k < StartPoint[z]; k++)
					{
						if (vCombDataReturn[k] - meanValue > maxDelta / 2)
						{
							stdMean = Min(stdMean, 1.3f * stdI);
							StartPoint[z] = k;
							break;
						}
					}
				}
			}

			int reflectcount = 0;
			int peakend = 0;
			int countT = 2;

			for (j = ShootPoint[z]; j >= StartPoint[z]; j--)
			{
				if (meanValue - vCombDataReturn[j] > Max(60.0f * ABS_DATA_ERROR_MULT, maxDelta / 2.0f))
				{
					reflectcount++;
				}
				else
				{
					reflectcount = 0;
				}
				if (reflectcount >= countT)
				{
					peakend = j + countT - 1;
					int m = 1;
					for (m = 1; m < 5; m++)
					{
						if (meanValue > vCombDataReturn[peakend + m] + 20 * ABS_DATA_ERROR_MULT)
						{
							peakend++;
						}
						else
						{
							break;
						}
					}
					break;
				}
			}
			peakend = Min(peakend, ShootPoint[z]);
			reflectcount = 0;
			if (peakend)
			{
				int peak = GetMinUint16(vCombDataReturn + StartPoint[z],  peakend + 1 - StartPoint[z]);
				if (peak < (int)(meanValue - Max(150.0f * ABS_DATA_ERROR_MULT, maxDelta)))
				{
					StartPoint[z] = peakend;
				}
			}
			else
			{
				if (ShootPoint[z] - peakend < 5)
				{
					for (j = StartPoint[z]; j <= ShootPoint[z]; j++)
					{
						if (meanValue - vCombDataReturn[j] > Max(60.0f * ABS_DATA_ERROR_MULT, maxDelta / 2.0f))
						{
							reflectcount++;
						}
						else
						{
							reflectcount = 0;
						}
						if (reflectcount >= countT)
						{
							ShootPoint[z] = Max(j - (countT + 1), 0);
							break;
						}
					}
				}
			}

			//修正过冲信号
			int L = ShootPoint[z] - StartPoint[z] + 1;
			if (L == 1)
			{
				Output[ShootPoint[z]] = vCombDataReturn[ShootPoint[z] - 1] - 8;
			}
			else
			{
				if (L == 2 && ExceptionFlag == 0 && ShootPoint[z] >= 2)
				{
					Output[ShootPoint[z] - 1] = vCombDataReturn[ShootPoint[z] - 2] + 5;
					Output[ShootPoint[z]] = vCombDataReturn[ShootPoint[z] - 2];
				}
				else
				{
					if (ExceptionFlag == 1 && L == 2)
					{
						meanValue = meanValue2;
					}
					unsigned short *tmp = (unsigned short *)calloc(L, sizeof(unsigned short));
					Gaussian_O10(tmp, L, meanValue, stdMean, maxDelta / 2);
					memcpy(Output + StartPoint[z], tmp, L * sizeof(unsigned short));
					free(tmp);
					tmp = NULL;
				}
			}
		}
		memcpy(vCombDataReturn, Output, Len * sizeof(unsigned short));
		free(Output);
		Output = NULL;
	}
}

void Gaussian_O10(unsigned short *GaussianSignal, int L, float u, float v, float delta)
{
	int j;
	double A, B, C;
	double uni[3] = { 0 };
	unsigned int R = 0;
	//srand((unsigned)time(NULL));
	//int x = 0;
	for (j = 0; j < L; j++)
	{
		uni[2] = (double)(UniformForGaussian(&uni[0]));
		A = (double)(sqrtf((-2.0f) * (float)log(uni[0])));
		B = 2 * PI * uni[1];
		C = A * cos(B);
		R = (unsigned int)round(u + C * v);
		if (fabsf(R - u) > delta)
		{
			GaussianSignal[j] = (unsigned int)(u - delta);
		}
		else
		{
			GaussianSignal[j] = R;
		}
	}
}
//随机生成下一个实数
int UniformForGaussian(double *p)
{
	int i, a;
	double f;
	int x = (int)(p[2]);
	for (i = 0; i < 2; i++, x = x + 689)
	{
		a = rand() + x;
		a = a % 1000;
		f = (double)(a);
		f = f / 1000.0;
		*p = f;
		p++;
	}
	return x;
}
#endif
#endif
