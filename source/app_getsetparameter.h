/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  getsetparameter.h
* 摘    要：  实现应用层中对eeprom存贮的参数的获取和保存
*             
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _APP_GETSETPARAMETER_H_
#define _APP_GETSETPARAMETER_H_

#include "app_parameter.h"
#include "app_frmsolamessager.h"
#include "app_frmsourcelight.h"

/*********************************************************************************************************
* 函数声明
*********************************************************************************************************/
int GenerateSerialNumber(char* pDest, const SerialNum* pSerialNum);
/*
**函数说明:
	初始化系统参数
**参数说明:
	无
**返回值:
	0		初始化成功
	-1		初始化失败
*/
int SystemSetInit(void);

int CheckSystemSet(void* buff);
void ResetSystemSet(void* buff);
int CheckWifiSet(void* buff);
void ResetWifiSet(void* buff);
int CheckSerialNumSet(void* buff);
void ResetSerialNumSet(void* buff);
int CheckFactoryConfigSet(void* buff);
void ResetFactoryConfigSet(void* buff);
int CheckOPMCalibrationKSet(void* buff);
void ResetOPMCalibrationKSet(void* buff);
int CheckOPMCalibrationBSet(void* buff);
void ResetOPMCalibrationKRecoverySet(void* buff);
void ResetOPMCalibrationBRecoverySet(void* buff);

//分期付款结构体相关函数
int CheckInstallmentSet(void* buff);
void ResetInstallmentSet(void* buff);

//BMP图片
int CheckBmpFileNameSet(void* buff);
void ResetBmpFileNameSet(void* buff);

//校准数据
int CheckCalibrationPara(void* buff);
void ResetCalibrationPara(void* buff);

void ResetOPMCalibrationBSet(void* buff);
void SerialNumTransform(PSerialNum pSerialNumSet);

//测试校准数据
void printCalibrationPara(const PCALIBRATIONPARA pCalibrationPara, FILE *target);
//测试Frontset
void printFrontSet(const PFRONT_SETTING pFrontSet, FILE *target);
//测试commonset
void printCommonSet(const PCOMMON_SETTING pCommonSet, FILE *target);
//测试sampleset
void printSampleSet(const PSAMPLE_SETTING pSampleSet, FILE *target);
//测试analysisSet
void printAnalysisSet(const PANALYSIS_SETTING pAnalysisSet, FILE *target);
//测试结果设置数据
void printResultSet(const POTHER_SETTING pOtherSet, FILE *target);
//测试系统设置数据
void printSystemSet(const PSYSTEMSET pSystemSet, FILE *target);
//测试wifi设置
void printWifiSet(const PWIFISET pWifiSet, FILE *target);
//测试lightsource
void printLightSource(const SOURCELIGHTCONFIG *pSourceLight, FILE *target);
//测试otdrMarkSet
void printOtdrMarkSet(const MarkParam *pMarkParam, FILE *target);

void printSerialNumber(const SerialNum *pSerialNum, FILE *target);
void printFactoryConfig(const PFactoryConfig pFactoryConfig, FILE *target);

#endif
