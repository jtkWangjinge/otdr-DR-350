/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_sola_algorithm_parameters.h
* 摘    要：  sola算法参数设置
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-21
*
*******************************************************************************/


#ifndef _APP_SOLA_ALGORITHM_H
#define _APP_SOLA_ALGORITHM_H

#ifdef __cplusplus 
extern "C" {
#endif

#include "app_otdr_algorithm_parameters.h"

// sola模式测试时，每个脉宽的每个任务的滤波系数 (列数:脉宽个数，行数:每个脉宽的任务个数)
extern const  OPM_FILTER solaFilters[OPM_PULSE_KIND][MAX_DAQ_NUM];

// 各个量程下使用的脉冲宽度
extern const OPM_PULSE SOLAChosedPulses[OPM_DISTRANGE_KIND][5];

// 各个量程下使用的移相次数
extern const unsigned int iSOLAPhaseShiftReg[OPM_DISTRANGE_KIND];

// 各个量程下使用的采样比率
extern const float fSOLASampleRatio[OPM_DISTRANGE_KIND];

//不同量程下使用的脉宽的任务数目
extern const int SOLAPulseNum[OPM_DISTRANGE_KIND];

// 每个脉宽测试时设置的采样间隔
extern const unsigned int perPulseAdcGap[OPM_PULSE_KIND];

// 每个脉宽循环次数
extern const int perPulseLoopNum[OPM_PULSE_KIND];

#ifdef __cplusplus
}
#endif

#endif


