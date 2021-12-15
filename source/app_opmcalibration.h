/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_OPMCalibration.h
* 摘    要：  生成 校准数据的函数声明
*
* 当前版本：  v1.0.0
* 作    者：  wangjinge
* 完成日期：
*******************************************************************************/

#ifndef _APP_OPM_CALIBRATION_H_
#define _APP_OPM_CALIBRATION_H_


/*!
 * 获取单片机采集的数据
 * @param[in]		size	采集ad数组的长度
 * @return					成功返回采集ad数组(注意释放内存)，失败返回NULL
 */
unsigned int* GetSampleADdata(int size);

/*!
 * 获取采集一组数据的平均值
 * @param[in]		data	采集ad数组
 * @param[in]		size	采集ad数组的长度
 * @return					成功返回平均值，失败返回-1
 */
float GetADmeanValue(unsigned int* data, int size);

/*!
 * 获取最佳通道index
 * @param[in]		data	各个通道平均值
 * @param[in]		size	数组的长度
 * @return					成功返回最佳通道index，失败返回-1
 */
int GetValiableChannelIndex(float* data, int size);

#endif