/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_math.h
* 摘    要：  常用的数学函数定义
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-21
*
*******************************************************************************/

#ifndef _APP_MATH_H
#define _APP_MATH_H

#ifdef __cplusplus 
extern "C" {
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//大小比较宏定义;
#define Max(x,y) ((x)>(y)?(x):(y))
#define Min(x,y) ((x)<(y)?(x):(y))

static const float fEps = 0.00000001f;

// unsigned short

/**
 * @brief	GetMaxUint16Index          得到其数据类型最大值的索引值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最小值的索引值
 **/
int GetMaxUint16Index(                 // 返回最大值的索引值;
		unsigned short *pSignal,	   // 输入数据
		int iSignalLength);			   // 输入数据长度;
		
/**
 * @brief	GetMinUint16Index          数组中第一次出现最小值的索引值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最小值的索引值
 **/
int GetMinUint16Index(                 // 返回最小值的索引值;
		unsigned short *pSignal,	   // 输入数据
		int iSignalLength);			   // 输入数据长度;
				
/**
 * @brief	GetMeanUint16              求unsigned short型数据的均值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回unsigned short型数据的均值
 **/
unsigned short GetMeanUint16(          // 返回数组均值
			   unsigned short *pSignal,// 输入的数据
			   int iSignalLength);     // 输入的数据长度
/**
 * @brief	GetMaxUint16               得到 unsigned short型数据的最大值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最大值
 **/
unsigned short GetMaxUint16(           // 返回数组最大值
			  unsigned short *pSignal, // 输入的数据
			  int iSignalLength);      // 输入的数据长度

/**
 * @brief	GetMinUint16               得到 unsigned short型数据的最小值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入输出的数据长度
 * @return	                           返回最小值
 **/
unsigned short GetMinUint16(           // 返回数组最小值
			  unsigned short *pSignal, // 输入的数据
			  int iSignalLength);      // 输入的数据长度

/**
 * @brief	GetStdUint16               求 unsigned short 型数组的标准差
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回 unsigned short 型数组的标准差
 **/
float GetStdUint16(					   // 返回数据标准差
		unsigned short *pSignal,       // 输入数据
		int iSignalLength); 	       // 输入数据长度 

// unsigned int

/**
 * @brief	GetMaxUint32               得到 unsigned int型数据的最大值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最大值
 **/
unsigned int GetMaxUint32(             // 返回数据最大值
		unsigned int *pSignal,         // 输入数据
		int iSignalLength);            // 输入数据长度      

/**
 * @brief	GetMinUint32               得到 unsigned int型数据的最小值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最小值
 **/
unsigned int GetMinUint32(             // 返回数据最小值
		unsigned int *pSignal,         // 输入数据
		int iSignalLength);            // 输入输出的数据长度   

/**
 * @brief	GetMeanUint32              求unsigned int型数组的平均值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回unsigned int型数组的平均值
 **/
float GetMeanUint32(                   // 返回数据平均值
		unsigned int *pSignal,         // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
 * @brief	GetStdUint32               求unsigned int型数组的标准差
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回uuint32型数组的标准差
 **/
float GetStdUint32(                    // 返回数据标准差
		unsigned int *pSignal,         // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
* @brief	FindFirstIndexUint32       得到数组中第一次出现iFindNumber的索引值
* @param 	pSignal                    输入数据
* @param 	iSignalLength              输入数据长度
* @param 	iFindNumber                输入待索引的数据
* @return	                           返回数组中第一次出现iFindNumber的索引值
**/
int FindFirstIndexUint32(              // 返回索引值，返回值返回iSignalLength表示查找失败
		unsigned int *pSignal,		   // 输入数据
		int iSignalLength,			   // 输入数据长度
		unsigned int iFindNumber);     // 输入待索引的数据

// int

/**
 * @brief	GetMaxInt                  得到int型数据的最大值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最大值
 **/
int GetMaxInt(                         // 返回数据最大值
		int *pSignal,                  // 输入数据
		int iSignalLength);            // 输入数据长度      

/**
 * @brief	GetMinInt                  得到 int 型数据的最小值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最小值
 **/
int GetMinInt(                         // 返回数据最小值
		int *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度   

/**
* @brief	GetMaxIntIndex             得到int型数据最大值的索引值
* @param 	pSignal                    输入数据
* @param 	iSignalLength              输入数据长度
* @return	                           返回int型数据最大值的索引值
**/
int GetMaxIntIndex(                    // 返回int型数据最大值的索引值
		int *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
* @brief	GetMinIntIndex             得到int型数据最小值的索引值
* @param 	pSignal                    输入数据
* @param 	iSignalLength              输入数据长度
* @return	                           返回int型数据最小值的索引值
**/
int GetMinIntIndex(                    // 返回int型数据最小值的索引值
		int *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
 * @brief	GetMeanAbsInt              求int型数组的绝对值的平均值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回int型数组的绝对值的平均值
 **/
float GetMeanAbsInt(                   // 返回数据绝对值的平均值
		int *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
 * @brief	GetMeanInt                 求 int 型数组的平均值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回 int 型数组的平均值
 **/
float GetMeanInt(                      // 返回数据平均值
		int *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度
  
/**
 * @brief	GetStdInt                  求 int 型数组的标准差
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回 int 型数组的标准差
 **/
float GetStdInt(                       // 返回数据标准差
		int *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度  

// float

/**
 * @brief	GetMaxFloat                得到float型数据的最大值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回最大值
 **/
float GetMaxFloat(                     // 返回float型数据的最大值
		float *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
 * @brief	GetMinFloat                得到 float 型数据的最小值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回float型数据的最小值
 **/
float GetMinFloat(                     // 返回float型数据的最小值
		float *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
 * @brief	GetMeanAbsFloat            得到 float 型数据绝对值的平均值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回float型绝对值的平均值
 **/
float GetMeanAbsFloat(                 // 返回数据绝对值的平均值
		float *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度   
/**
 * @brief	GetMeanFloat               得到 float 型数据平均值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回 float 型数据平均值
 **/
float GetMeanFloat(                    // 返回数据平均值
		float *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度   

/**
 * @brief	GetStdFloat                求 float 型数组的标准差
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回 float 型数组的标准差
 **/
float GetStdFloat(                     // 返回数据标准差
		float *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度 

/**
 * @brief	GetMaxAbsFloat             得到float型数组的绝对值的最大值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @return	                           返回float型数组的绝对值的最大值
 **/
float GetMaxAbsFloat(                  // 返回数据绝对值最大值
		float *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
 * @brief	FindFirstIndex             得到数组中第一次出现fFindNumber的索引值
 * @param 	pSignal                    输入数据
 * @param 	iSignalLength              输入数据长度
 * @param 	fFindNumber                输入待索引的数据
 * @return	                           返回数组中第一次出现fFindNumber的索引值
 **/
int FindFirstIndex(                    // 返回索引值，返回值返回iSignalLength表示查找失败
		float *pSignal,				   // 输入数据
		int iSignalLength,			   // 输入数据长度
		float fFindNumber);			   // 输入待索引的数据

/**
* @brief	GetMaxIndex                得到float型数据最大值的索引值
* @param 	pSignal                    输入数据
* @param 	iSignalLength              输入数据长度
* @return	                           返回float型数据最大值的索引值
**/
int GetMaxIndex(                       // 返回float型数据最大值的索引值
		float *pSignal,				   // 输入数据
		int iSignalLength);			   // 输入数据长度

/**
* @brief	GetMinIndex                得到float型数据最小值的索引值
* @param 	pSignal                    输入数据
* @param 	iSignalLength              输入数据长度
* @return	                           返回float型数据最小值的索引值
**/
int GetMinIndex(					   // 返回最小值的索引值;
		float *pSignal,				   // 输入数据;
		int iSignalLength);			   // 输入的数据长度;

/**
* @brief	CalLeastSquare             最小二乘法
* @param 	pX						   输入的x变量
* @param 	pY						   输入的y变量
* @param 	count                      数据的长度
* @param 	k						   斜率
* @param 	b						   截距
* @return	                           
**/
void CalLeastSquare(
	int * pX,        				   // 输入的x变量;
	int * pY,       				   // 输入的Y变量;
	int count,          			   // 数据的长度;
	float *k, 						   // 斜率;
	float *b);        				   // 截距;

/**
* @brief	Cwt_Haar                   连续haar小波变换
* @param 	pSignal					   进行小波变换的数据的数组指针
* @param 	pTransSignal			   小波变化后的数据的数组指针
* @param 	iSignalLength              输入数据的长度
* @param 	iScale					   小波变换尺
* @return
**/
void Cwt_Haar(						   // 连续haar小波变换;
	const float *pSignal,			   // 进行小波变换的数据的数组指针;
	float *pTransSignal,			   // 小波变化后的数据的数组指针;
	const int iSignalLength,		   // 输入的数据长度;
	const int iScale);				   // 小波变换尺度;

/**
* @brief	Iir_Filter                 IIR滤波
* @param 	pSignal					   进行滤波的数据的数组指针
* @param 	pFilterSignal			   滤波后的数据的数组指针
* @param 	pCoefB                     iir滤波器系数b
* @param 	pCoefA                     iir滤波器系数a
* @param 	iSignalLength              进行滤波的数据长度
* @param 	iFilterLength              滤波器系数长度
* @return
**/
void Iir_Filter(                       // IIR滤波;
	const float *pSignal,			   // 进行滤波的数据的数组指针;
	float *pFilterSignal,			   // 滤波后的数据的数组指针;
	const float *pCoefB,			   // iir滤波器系数b;
	const float *pCoefA,			   // iir滤波器系数a;
	const int iSignalLength,		   // 进行滤波的数据长度;
	const int iFilterLength);		   // 滤波器系数长度;

/**
* @brief	Filt_Db1                   平稳db1小波专用滤波器
* @note								   滤波系数分别为1/sqrt(2) 1/sqrt(2)和-1/sqrt(2) 1/sqrt(2)
*                                      对信号进行了双边延拓, 输入信号的长度需为2^iScale-1的整数
* @param 	pSignal					   进行小波滤波的数据
* @param 	pLowSignal			       滤波信号低频分量
* @param 	pHighSignal                滤波信号高频分量
* @param 	iSignalLength              输入信号的长度
* @param 	iScale					   表示进行db1小波滤波的尺度
* @return
**/
void Filt_Db1(                     
	const float *pSignal,			   // 进行小波滤波的数据;
	float *pLowSignal,				   // 滤波信号低频分量;
	float *pHighSignal,				   // 滤波信号高频分量;
	const int iSignalLength,		   // 输入信号的数据长度;
	const int iScale);				   // 表示进行db1小波滤波的尺度;

/**
* @brief	Swt_Db1                    平稳db1小波变换
* @param 	pSignal					   进行小波变换的数据的数组指针
* @param 	pTransSignal			   小波变化后的数据的数组指针
* @param 	iSignalLength              输入数据的长度
* @param 	iScale					   小波变换尺
* @return
**/
void Swt_Db1(                          // 平稳db1小波变换;
	const float *pSignal,			   // 进行小波变换的数据的数组指针;
	float *pTransSignal[],			   // 进行平稳小波变换后的低频数据指针;
	const int iSignalLength,		   // 输入小波变换的数据长度;
	const int iScale);				   // 平稳小波变换尺度;

/**
* @brief	ReFilt_Db1                 平稳db1小波专用滤波器
* @note								   滤波系数分别为1/sqrt(2) 1/sqrt(2),swt滤波器采用循环卷积
*                                      对信号进行了双边延拓, 输入信号的长度需为2^iScale-1的整数
* @param 	pLowSignal			       滤波信号低频分量
* @param 	pHighSignal                滤波信号高频分量
* @param 	pTransLowSignal			   进行滤波变换的数据指针
* @param 	iSignalLength              输入信号的长度
* @param 	iScale					   表示进行db1小波滤波的尺度
* @return
**/
void ReFilt_Db1(                     
	const float *pLowSignal,		  // 滤波信号低频分量;
	const float *pHighSignal,		  // 滤波信号高频分量 ;
	float *pTransLowSignal,           // 进行滤波变换的数据指针;
	const int iSignalLength,		  // 输入信号的长度;
	const int iScale);				  // 表示进行db1小波滤波的尺度;

/**
* @brief	iSwt_Db1                   平稳小波db1变换逆变换
* @param 	pTransSignal			   进行平稳小波变换后的低频数据指针
* @param 	pSignal                    进行小波变换的数据的数组指针
* @param 	iSignalLength              输入小波变换的数据长度
* @param 	iScale					   平稳小波变换尺度
* @return
**/
void iSwt_Db1(                         // 平稳小波db1变换逆变换;
	float *pTransSignal[],			   // 进行平稳小波变换后的低频数据指针;
	float *pSignal,					   // 进行小波变换的数据的数组指针;
	const int iSignalLength,		   // 输入小波变换的数据长度;
	const int iScale);				   // 平稳小波变换尺度;

void APP_LOG(char *log);

#ifdef __cplusplus
}
#endif

#endif
