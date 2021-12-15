/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_otdr_algorithm_parameters.h
* 摘    要：  otdr模式下常用的参数设置及函数定义
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-24
*
*******************************************************************************/


#ifndef _APP_OTDR_ALGOTITHM_PARAMENT_H
#define _APP_OTDR_ALGOTITHM_PARAMENT_H

#ifdef __cplusplus 
extern "C" {
#endif

#include "app_algorithm_support.h"

#define UNCERT_DATA_LEN     			  10
#define LINEARITY_MAX_VALUE               20

#define INTP01_01 (0<<8)  //采样1个点，输出一个点	
#define INTP01_02 (1<<8)  //2倍移相插补	
#define INTP01_04 (2<<8)  //4倍移相插补	
#define INTP01_08 (3<<8)  //8倍移相插补	
#define INTP01_16 (4<<8)  //16倍移相插补	
#define INTP01_32 (5<<8)  //32倍移相插补	
#define INTP01_64 (6<<8)  //64倍移相插补	
#define INTP16_01 (7<<8)  //采样16个点，输出一个点	
#define INTP08_01 (8<<8)  //采样8个点，输出一个点	
#define INTP04_01 (9<<8)  //采样4个点，输出一个点	
#define INTP02_01 (10<<8) //采样2个点，输出一个点	

//定义放大档数组合
typedef struct _ampcom{
	int iAmp;
	int iApd;
	int iVdd;
} AMPCOM, *PAMPCOM;

//定义光模块中所使用的波长类型;
typedef enum _opm_wave
{
    ENUM_WAVE_1310NM 	= 0,
    ENUM_WAVE_1550NM	= 1,
    ENUM_WAVE_1625NM	= 2
} OPM_WAVE;

//定义光模块中所使用的脉冲宽度;
typedef enum _opm_pulse
{
    ENUM_PULSE_AUTO		= 0,
    ENUM_PULSE_5NS		= 1,
    ENUM_PULSE_10NS		= 2,
    ENUM_PULSE_20NS		= 3,
    ENUM_PULSE_50NS		= 4,
    ENUM_PULSE_100NS	= 5,
    ENUM_PULSE_200NS	= 6,
    ENUM_PULSE_500NS	= 7,
    ENUM_PULSE_1US		= 8,
    ENUM_PULSE_2US		= 9,
    ENUM_PULSE_10US 	= 10,
    ENUM_PULSE_20US		= 11,   
} OPM_PULSE;

typedef enum _fiber_range
{
	ENUM_FIBER_RANGE_AUTO	= 0,					// 距离量程设为自动
	ENUM_FIBER_RANGE_500M	= 1,					// 500m光纤的长度
	ENUM_FIBER_RANGE_1KM	= 2,					// 1km光纤的长度
	ENUM_FIBER_RANGE_2KM	= 3,					// 2km光纤的长度
	ENUM_FIBER_RANGE_5KM	= 4,					// 5km光纤的长度
	ENUM_FIBER_RANGE_10KM	= 5,					// 10km光纤的长度
	ENUM_FIBER_RANGE_20KM	= 6,					// 20km光纤的长度
	ENUM_FIBER_RANGE_50KM	= 7,					// 50km光纤的长度
	ENUM_FIBER_RANGE_100KM	= 8,					// 100km光纤的长度
	ENUM_FIBER_RANGE_200KM	= 9,					// 200km光纤的长度
} FIBER_RANGE;

// 定义光模块中所使用的放大功率;
typedef enum _opm_attenuation
{
    ENUM_0_0DB			= 0,
    ENUM_2_5DB			= 1,
    ENUM_5_0DB			= 2,
    ENUM_7_5DB			= 3,
    ENUM_10_0DB			= 4,
    ENUM_12_5DB			= 5,
    ENUM_15_0DB			= 6,
    ENUM_17_5DB			= 7,
    ENUM_20_0DB			= 8,
    ENUM_22_5DB			= 9,
    ENUM_25_0DB			= 10,
    ENUM_27_5DB			= 11,
    ENUM_30_0DB			= 12
} OPM_ATTE;

// 定义光模块中所支持滤波参数;
typedef enum _opm_filter
{
	_IIR_2		= 0,
	_IIR_4		= 1,
	_IIR_8		= 2,
	_IIR_16		= 3,
	_IIR_32		= 4,
	_IIR_64		= 5,
	_IIR_128	= 6,
	_IIR_256	= 7,
    _IIR_512	= 8,
    _IIR_1K		= 9,
    _IIR_2K		= 10,
    _IIR_4K		= 11,
    _IIR_8K		= 12,
    _IIR_16K	= 13,
    _IIR_32K	= 14,
    _IIR_64K	= 15,
    _IIR_128K	= 16,
	_IIR_256K	= 17,
	_IIR_512K	= 18
} OPM_FILTER;

#define SMP_COUNT016_01	0.0625f						// fpga采集16个点上报给arm1个点，简称为1/16
#define SMP_COUNT08_01	0.125f						// fpga采集8个点上报给arm1个点，简称为1/8
#define SMP_COUNT04_01	0.25f						// fpga采集4个点上报给arm1个点，简称为1/4
#define SMP_COUNT02_01	0.5f						// fpga采集2个点上报给arm1个点，简称为1/2
#define SMP_COUNT01_01	1.0f						// 1次移相
#define SMP_COUNT01_02	2.0f						// 2次移相
#define SMP_COUNT01_04	4.0f						// 4次移相
#define SMP_COUNT01_08	8.0f						// 8次移相
#define SMP_COUNT01_16	16.0f						// 16次移相
#define SMP_COUNT01_32	32.0f						// 32次移相
#define SMP_COUNT01_64	64.0f						// 64次移相

extern const unsigned int offsetExp[OPM_DISTRANGE_KIND][2]; 
extern const int DaqNum[OPM_PULSE_KIND];

// 移相表
extern const unsigned int iPhaseShiftReg[OPM_DISTRANGE_KIND][2];

// 采样比率表
extern const float fSampleRatio[OPM_DISTRANGE_KIND][2];

// 脉宽任务
extern const OPM_ATTE enAttenu_5ns[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_10ns[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_20ns[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_50ns[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_100ns[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_200ns[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_500ns[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_1us[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_2us[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_10us[MAX_DAQ_NUM];
extern const OPM_ATTE enAttenu_20us[MAX_DAQ_NUM];

// 各个脉宽所对应的总放大通道这个关系
extern AMPCOM AmpCom5ns[5];
extern AMPCOM AmpCom10ns[7];
extern AMPCOM AmpCom20ns[7];
extern AMPCOM AmpCom50ns[10];
extern AMPCOM AmpCom100ns[10];
extern AMPCOM AmpCom200ns[10];
extern AMPCOM AmpCom500ns[11];
extern AMPCOM AmpCom1us[11];
extern AMPCOM AmpCom2us[11];
extern AMPCOM AmpCom10us[11];
extern AMPCOM AmpCom20us[11];

//不同脉宽的衰减量
extern const  OPM_ATTE *attr[OPM_PULSE_KIND];

// 存储平均模式每个量程中的每个任务的滤波系数 (行数:量程个数，列数:每个量程的任务个数)
extern const  OPM_FILTER avg_filter[OPM_DISTRANGE_KIND][MAX_DAQ_NUM];

// 存储实时模式下每个量程的滤波系数(量程下的所有小任务使用同一系数)
extern const OPM_FILTER rt_filter[OPM_DISTRANGE_KIND];

// 移相插补表
extern const unsigned int iPhaseShiftReg[OPM_DISTRANGE_KIND][2];

// 采样比率表
extern const float fSampleRatio[OPM_DISTRANGE_KIND][2];

#if 0
/**
* @brief	SetAttenuation					据不同的脉宽和量程设置修改通道参数
* @param 	pulseWidth						脉宽
* @param 	attr							通道参数
* @param 	taskNum							任务数
* @param 	autoFiberLen					auto出来的光纤长度
* @param 	mode							测量模式
* @return									累加次数
**/
unsigned int SetAttenuation(int pulseWidth, 
	const OPM_ATTE *attr[OPM_PULSE_KIND], 
	int taskNum,
	float autoFiberLen,
	int mode);			 // 0:平均模式 1:实时模式    		      

/**
* @brief	SetFilters						计算累加次数
* @param 	range							量程
* @param 	pulseWidth						脉宽
* @param 	attr							通道参数
* @param 	taskNum							任务数
* @param 	mode							测量模式
* @return									累加次数
**/
unsigned int SetFilters(int range,
	int pulseWidth, 
	const OPM_ATTE *attr[OPM_PULSE_KIND], 
	int taskNum,
	int mode);  			// 0:平均模式 1:实时模式
#endif

#ifdef __cplusplus
}
#endif

#endif
