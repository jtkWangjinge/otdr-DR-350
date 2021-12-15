/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_OPMCalibration.c
* 摘    要：  生成 校准数据的函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#include "app_opmcalibration.h"
#include "app_global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//AD数据拟合曲线的范围值
#define MIN_AD_VALUE		2000
#define MAX_AD_VALUE		5000
#define MID_AD_VALUE		((MIN_AD_VALUE+MAX_AD_VALUE)/2)
//通道个数
#define CHANNEL_NUM			7

/*!
 * 获取单片机采集的数据
 * @param[in]		size	采集ad数组的长度
 * @return					成功返回采集ad数组(注意释放内存)，失败返回NULL
 */
unsigned int* GetSampleADdata(int size)
{
	//检测参数
	if (size <= 0)
	{
		LOG(LOG_ERROR, "---size less than 0------\n");
		return NULL;
	}

	unsigned int* buff = NULL;
	buff = (unsigned int*)malloc(size*sizeof(unsigned int));
	if (!buff)
	{
		LOG(LOG_ERROR, "----malloc memory failed------\n");
		return NULL;
	}

	memset(buff, 0, size * sizeof(unsigned int));
	//TODO

	return buff;
}

/*!
 * 获取采集一组数据的平均值
 * @param[in]		data	采集ad数组
 * @param[in]		size	采集ad数组的长度
 * @return					成功返回平均值，失败返回-1
 */
float GetADmeanValue(unsigned int* data, int size)
{
	//检测参数
	if (!data || size <= 0)
	{
		return -1;
	}

	int i = 0;
	int sum = 0;
	for (; i < size; ++i)
	{
		sum += data[i];
	}

	return (sum / size);
}

/*!
 * 获取最佳通道index
 * @param[in]		data	各个通道平均值
 * @param[in]		size	数组的长度
 * @return					成功返回最佳通道index，失败返回-1
 */
int GetValiableChannelIndex(float* data, int size)
{
	//检测参数
	if (!data || size <= 0)
	{
		return -1;
	}
	//存储在范围内的通道值
	unsigned int iIndexArray[CHANNEL_NUM] = {0};
	memset(iIndexArray, 0, CHANNEL_NUM*sizeof(unsigned int));
	unsigned int iIndex = 0;//最终返回的下标值
	int i = 0;		//记录各个通道平均值数组的下标
	int j = 0;		//记录范围内通道值的下标
	int count = 0;	//记录范围内通道个数
	for (; i < size; ++i)
	{
		if (data[i] >= MIN_AD_VALUE && data[i] <= MAX_AD_VALUE)
		{
			iIndexArray[j] = i;
			j++;
			count++;
		}
	}
	//范围内通道
	if (count)
	{
		float minValue = fabsf(data[iIndex] - MID_AD_VALUE);//最接近范围中间值的数据
		for (j = 0; j < count; ++j)
		{
			iIndex = iIndexArray[j];						//获取范围内下标值
			float temp = fabsf(data[iIndex] - MID_AD_VALUE);
			//获取最小值即最接近中间值
			if ( temp < minValue)
			{
				minValue = temp;
			}
		}

		return iIndex;
	}
	else
	{
		return -2;//所有通道皆不符合范围
	}
}
