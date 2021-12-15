/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_otdr_algorithm_parameters.c
* 摘    要：  otdr模式下常用的参数设置及函数实现
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-24
*
*******************************************************************************/


#include "app_otdr_algorithm_parameters.h"
// #include "app_algorithm_support.h"
#include "app_math.h"


// 偏移量经验值
const unsigned int offsetExp[OPM_DISTRANGE_KIND][2]={
    { 320, 1230 },	 // auto 
    { 293, 1170 },	 // 0.5km 
	{ 293, 1170 },	 // 1km 
    { 34, 550 },	 // 2km
    { 34, 280 },	 // 5km 
    { 40, 160 },	 // 10km 
    { 21, 85 }, 	 // 20km
    { 10, 40 }, 	 // 50km 
    { 5, 21 }, 		 // 100km 
    { 5, 21 }, 		 // 200km 
};

// 不同脉宽的任务数目
// const  int DaqNum[OPM_PULSE_KIND] = { 1, 1, 2, 2, 2, 3, 3, 5, 5, 5, 5, 5 };
// const  int DaqNum[OPM_PULSE_KIND] = { 1, 1, 2, 2, 2, 3, 3, 5, 5, 5, 6, 6 };
//                                   auto 5 10 20 50 100 200 500 1 2 10 20

const int DaqNum[OPM_PULSE_KIND] = { 1, 1, 2, 2, 2, 2, 3, 5, 5, 5, 5, 5 };

// 通道参数
AMPCOM AmpCom5ns[] = {
    { 1, 0, 0 },   //0db
    { 0, 0, 0 },   //2.5db
    { 1, 0, 0 },   //5db
    { 0, 2, 0 },   //7.5db
    { 0, 3, 0 },   //10db
};

AMPCOM AmpCom10ns[] = {
	{ 3, 0, 0 },   //0db
	{ 1, 0, 0 },   //2.5db
	{ 1, 1, 0 },   //5db
	{ 1, 0, 0 },   //7.5db 应用setattenuation+2
	{ 1, 1, 0 },   //10db  应用setattenuation+2
	{ 2, 0, 0 },   //12.5db
	{ 2, 1, 0 },   //15db
};

AMPCOM AmpCom20ns[] = {
	{ 3, 0, 0 },   //0db
	{ 1, 0, 0 },   //2.5db
	{ 1, 1, 0 },   //5db
	{ 1, 0, 0 },   //7.5db 应用setattenuation+2
	{ 1, 1, 0 },   //10db  应用setattenuation+2
	{ 4, 0, 0 },   //12.5db
	{ 4, 1, 0 },   //15db
};

AMPCOM AmpCom50ns[] = {
    { 4, 1, 3 },   //0db
    { 3, 0, 3 },   //2.5db
    { 3, 1, 3 },   //5db
    { 3, 2, 3 },   //7.5db
    { 3, 3, 3 },   //10db
    { 2, 0, 3 },   //12.5db
    { 1, 0, 3 },   //15db
    { 1, 1, 3 },   //17.5db
    { 1, 2, 3 },   //20db
    { 1, 3, 3 },   //22.5db
};

AMPCOM AmpCom100ns[] = {
    { 4, 1, 3 },   //0db
    { 3, 0, 3 },   //2.5db
    { 3, 1, 3 },   //5db
    { 3, 2, 3 },   //7.5db
    { 3, 3, 3 },   //10db
    { 4, 3, 3 },   //12.5db
    { 1, 0, 3 },   //15.0db
    { 1, 0, 3 },   //17.5db
    { 1, 0, 3 },   //20db
    { 1, 0, 3 },   //22.5db
};

/*
AMPCOM AmpCom200ns[] = {
    { 6, 1, 3 },   //0db
    { 4, 1, 3 },   //2.5db
    { 4, 1, 3 },   //5db
    { 4, 2, 3 },   //7.5db
    { 4, 3, 3 },   //10db
    { 4, 3, 3 },   //12.5db
    { 3, 0, 3 },   //15.0db
    { 3, 1, 3 },   //17.5db
    { 3, 2, 3 },   //20db
    { 3, 3, 3 },   //22.5db
};

AMPCOM AmpCom500ns[] = {
	{ 7, 1, 3 },   //0db
	{ 6, 1, 3 },   //2.5db
	{ 5, 1, 3 },   //5db
	{ 4, 1, 3 },   //7.5db
	{ 4, 1, 3 },   //10db
	{ 4, 2, 3 },   //12.5db
	{ 4, 3, 3 },   //15db
	{ 4, 3, 3 },   //17.5db
	{ 3, 3, 3 },   //20db
	{ 2, 3, 3 },   //22.5db
	{ 1, 3, 3 },   //25db
};

AMPCOM AmpCom1us[] = {
	{ 8, 1, 3 },   //0db
	{ 8, 1, 3 },   //2.5db
	{ 7, 1, 3 },   //5db
	{ 6, 1, 3 },   //7.5db
	{ 6, 1, 3 },   //10db
	{ 6, 2, 3 },   //12.5db
	{ 6, 3, 3 },   //15db
	{ 5, 3, 3 },   //17.5db
	{ 4, 3, 3 },   //20db
	{ 3, 3, 3 },   //22.5db
	{ 2, 3, 3 },   //25db
};

AMPCOM AmpCom2us[] = {
	{ 9, 1, 3 },   //0db
	{ 9, 1, 3 },   //2.5db
	{ 7, 0, 3 },   //5db
	{ 7, 1, 3 },   //7.5db
	{ 6, 1, 3 },   //10db
	{ 6, 1, 3 },   //12.5db
	{ 6, 2, 3 },   //15db
	{ 6, 3, 3 },   //17.5db
	{ 6, 3, 3 },   //20db
	{ 5, 3, 3 },   //22.5db
	{ 4, 3, 3 },   //25db
};

AMPCOM AmpCom10us[] = {
	{ 9, 0, 3 },   //0db
	{ 9, 1, 3 },   //2.5db
	{ 8, 1, 3 },   //5db
	{ 7, 0, 3 },   //7.5db
	{ 6, 0, 3 },   //10db
	{ 6, 1, 3 },   //12.5db
	{ 6, 2, 3 },   //15db
	{ 5, 3, 3 },   //17.5db
	{ 4, 3, 3 },   //20db
	{ 3, 3, 3 },   //22.5db
	{ 2, 3, 3 },   //25db
};

AMPCOM AmpCom20us[] = {
	{ 9, 0, 1 },   //0db
	{ 9, 1, 1 },   //2.5db
	{ 8, 1, 1 },   //5db
	{ 7, 1, 1 },   //7.5db
	{ 6, 0, 1 },   //10db
	{ 6, 1, 1 },   //12.5db
	{ 6, 2, 1 },   //15db
	{ 5, 3, 1 },   //17.5db
	{ 3, 3, 1 },   //20db
	{ 2, 3, 1 },   //22.5
	{ 1, 3, 1 },   //25db
};
*/
AMPCOM AmpCom200ns[] = {
	{ 7, 1, 3 },   //0db
	{ 5, 0, 3 },   //2.5db
	{ 5, 1, 3 },   //5db
	{ 5, 2, 3 },   //7.5db
	{ 5, 3, 3 },   //10db
	{ 5, 2, 3 },   //12.5db
	{ 3, 0, 3 },   //15.0db
	{ 3, 1, 3 },   //17.5db
	{ 3, 2, 3 },   //20db
	{ 3, 3, 3 },   //22.5db
};

AMPCOM AmpCom500ns[] = {
    { 7, 0, 3 },   //0db
    { 7, 1, 3 },   //2.5db
    { 5, 0, 3 },   //5db
    { 5, 0, 3 },   //7.5db
    { 5, 1, 3 },   //10db
    { 5, 2, 3 },   //12.5db
    { 5, 3, 3 },   //15db
    { 5, 3, 3 },   //17.5db
    { 3, 2, 3 },   //20db
    { 1, 2, 3 },   //22.5db
    { 1, 3, 3 },   //25db
};

AMPCOM AmpCom1us[] = {
    { 9, 1, 3 },   //0db
    { 9, 1, 3 },   //2.5db
    { 7, 0, 3 },   //5db
    { 7, 1, 3 },   //7.5db
    { 5, 0, 3 },   //10db
    { 5, 1, 3 },   //12.5db
    { 5, 2, 3 },   //15db
    { 5, 3, 3 },   //17.5db
    { 3, 3, 3 },   //20db
    { 3, 3, 3 },   //22.5db
    { 1, 3, 3 },   //25db
};

AMPCOM AmpCom2us[] = {
    { 9, 0, 3 },   //0db
    { 9, 1, 3 },   //2.5db
    { 7, 0, 3 },   //5db
    { 7, 1, 3 },   //7.5db
    { 5, 0, 3 },   //10db
    { 5, 1, 3 },   //12.5db
    { 5, 2, 3 },   //15db
    { 5, 3, 3 },   //17.5db
    { 5, 3, 3 },   //20db
    { 5, 3, 3 },   //22.5db
    { 3, 3, 3 },   //25db
};
#if 0
AMPCOM AmpCom10us[] = {
	{ 9, 0, 3 },   //0db
	{ 9, 1, 3 },   //2.5db
	{ 8, 1, 3 },   //5db
	{ 7, 0, 3 },   //7.5db
	{ 6, 0, 3 },   //10db
	{ 6, 1, 3 },   //12.5db
	{ 6, 2, 3 },   //15db
	{ 5, 3, 3 },   //17.5db
	{ 4, 3, 3 },   //20db
	{ 3, 0, 3 },   //22.5db
	{ 2, 0, 3 },   //25db
};

AMPCOM AmpCom20us[] = {
	{ 9, 0, 1 },   //0db
	{ 9, 1, 1 },   //2.5db
	{ 7, 0, 1 },   //5db
	{ 7, 1, 1 },   //7.5db
	{ 6, 0, 1 },   //10db
	{ 6, 1, 1 },   //12.5db
	{ 6, 2, 1 },   //15db
	{ 5, 3, 1 },   //17.5db
	{ 3, 0, 1 },   //20db
	{ 2, 0, 1 },   //22.5
	{ 1, 3, 1 },   //25db
};
#endif

AMPCOM AmpCom10us[] = {
    { 9, 0, 3 },   //0db
    { 9, 1, 3 },   //2.5db
    { 7, 0, 3 },   //5db
    { 7, 1, 3 },   //7.5db
    { 5, 0, 3 },   //10db
    { 5, 1, 3 },   //12.5db
    { 5, 2, 3 },   //15db
    { 5, 3, 3 },   //17.5db
    { 3, 2, 3 },   //20db
    { 3, 3, 3 },   //22.5db
    { 1, 3, 3 },   //25db
};

AMPCOM AmpCom20us[] = {
    { 9, 0, 1 },   //0db
    { 9, 1, 1 },   //2.5db
    { 7, 0, 1 },   //5db
    { 7, 1, 1 },   //7.5db
    { 5, 0, 1 },   //10db
    { 5, 1, 1 },   //12.5db
    { 5, 2, 1 },   //15db
    { 5, 3, 1 },   //17.5db
    { 3, 3, 1 },   //20db
    { 1, 2, 1 },   //22.5
    { 1, 3, 1 },   //25db
};

#if 0
// 多任务采集中衰减量的设置
//const OPM_ATTE enAttenu_5ns[MAX_DAQ_NUM]
//= { ENUM_2_5DB, ENUM_0_0DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_5ns[MAX_DAQ_NUM]
	= { ENUM_0_0DB, ENUM_0_0DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_10ns[MAX_DAQ_NUM]
	= { ENUM_5_0DB, ENUM_2_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_15_0DB, ENUM_12_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_20ns[MAX_DAQ_NUM]
	= { ENUM_5_0DB, ENUM_2_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_15_0DB, ENUM_12_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_50ns[MAX_DAQ_NUM]
	= { ENUM_10_0DB, ENUM_5_0DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_22_5DB, ENUM_20_0DB, ENUM_17_5DB, ENUM_15_0DB };
const OPM_ATTE enAttenu_100ns[MAX_DAQ_NUM]
	= { ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_2_5DB, ENUM_22_5DB, ENUM_20_0DB, ENUM_17_5DB, ENUM_15_0DB };
const OPM_ATTE enAttenu_200ns[MAX_DAQ_NUM]
	= { ENUM_10_0DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_22_5DB, ENUM_20_0DB, ENUM_17_5DB, ENUM_15_0DB };
const OPM_ATTE enAttenu_500ns[MAX_DAQ_NUM]
	= { ENUM_20_0DB, ENUM_15_0DB, ENUM_10_0DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB };
//const OPM_ATTE enAttenu_1us[MAX_DAQ_NUM]
//	= { ENUM_15_0DB, ENUM_12_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };
//const OPM_ATTE enAttenu_2us[MAX_DAQ_NUM]
//	= { ENUM_17_5DB, ENUM_12_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_1us[MAX_DAQ_NUM]
	= { ENUM_20_0DB, ENUM_15_0DB, ENUM_12_5DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB};
const OPM_ATTE enAttenu_2us[MAX_DAQ_NUM]
	= { ENUM_22_5DB, ENUM_17_5DB, ENUM_12_5DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_10us[MAX_DAQ_NUM]
	= { ENUM_22_5DB, ENUM_20_0DB, ENUM_15_0DB, ENUM_12_5DB, ENUM_10_0DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_20us[MAX_DAQ_NUM]
	= { ENUM_20_0DB, ENUM_17_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };
//const OPM_ATTE enAttenu_10us[MAX_DAQ_NUM]
	//= { ENUM_22_5DB, ENUM_20_0DB, ENUM_15_0DB, ENUM_12_5DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB };
//const OPM_ATTE enAttenu_20us[MAX_DAQ_NUM]
	//= { ENUM_20_0DB, ENUM_17_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };
#endif

const OPM_ATTE enAttenu_5ns[MAX_DAQ_NUM]
	= { ENUM_0_0DB, ENUM_0_0DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_10ns[MAX_DAQ_NUM]
	= { ENUM_5_0DB, ENUM_2_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_15_0DB, ENUM_12_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_20ns[MAX_DAQ_NUM]
	= { ENUM_5_0DB, ENUM_2_5DB, ENUM_10_0DB, ENUM_7_5DB, ENUM_15_0DB, ENUM_12_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_50ns[MAX_DAQ_NUM]
	= { ENUM_7_5DB, ENUM_2_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_22_5DB, ENUM_20_0DB, ENUM_17_5DB, ENUM_15_0DB };
const OPM_ATTE enAttenu_100ns[MAX_DAQ_NUM]
	= { ENUM_7_5DB, ENUM_2_5DB, ENUM_2_5DB, ENUM_2_5DB, ENUM_22_5DB, ENUM_20_0DB, ENUM_17_5DB, ENUM_15_0DB };
const OPM_ATTE enAttenu_200ns[MAX_DAQ_NUM]
	= { ENUM_10_0DB, ENUM_7_5DB, ENUM_2_5DB, ENUM_2_5DB, ENUM_22_5DB, ENUM_20_0DB, ENUM_17_5DB, ENUM_15_0DB };
const OPM_ATTE enAttenu_500ns[MAX_DAQ_NUM]
	= { ENUM_20_0DB, ENUM_15_0DB, ENUM_10_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_1us[MAX_DAQ_NUM]
	= { ENUM_20_0DB, ENUM_17_5DB, ENUM_12_5DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_2us[MAX_DAQ_NUM]
	= { ENUM_25_0DB, ENUM_20_0DB, ENUM_12_5DB, ENUM_7_5DB, ENUM_5_0DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_10us[MAX_DAQ_NUM]
	= { ENUM_22_5DB, ENUM_17_5DB, ENUM_12_5DB, ENUM_7_5DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB, ENUM_0_0DB };
const OPM_ATTE enAttenu_20us[MAX_DAQ_NUM]
	= { ENUM_20_0DB, ENUM_17_5DB, ENUM_12_5DB, ENUM_7_5DB, ENUM_2_5DB, ENUM_2_5DB, ENUM_0_0DB, ENUM_0_0DB };

//不同脉宽的衰减量
const OPM_ATTE *attr[OPM_PULSE_KIND] =
{
	enAttenu_100ns, 		// auto
    enAttenu_5ns, 			// 5ns
    enAttenu_10ns,			// 10ns
    enAttenu_20ns,			// 20ns
    enAttenu_50ns,			// 50ns
    enAttenu_100ns,			// 100ns
    enAttenu_200ns,			// 200ns
    enAttenu_500ns,			// 500ns
    enAttenu_1us,			// 1us
    enAttenu_2us,			// 2us
    enAttenu_10us,			// 10us
    enAttenu_20us,			// 20us
};

// 存储平均模式每个量程中的每个任务的滤波系数 (行数:量程个数，列数:每个量程的任务个数)
const OPM_FILTER avg_filter[OPM_DISTRANGE_KIND][MAX_DAQ_NUM] =
{
	/*
    { _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128 }, //!< auto
    { _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256 }, //!< 0.5km
	{ _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256 }, //!< 1km
    { _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256 }, //!< 2km
    { _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256, _IIR_256 }, //!< 5km
    { _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128 }, //!< 10km
    { _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128 }, //!< 20km
    { _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128 }, //!< 50km
    { _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128 }, //!< 100km
    { _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128 }, //!< 200km
	*/

	{ _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128, _IIR_128 }, //!< auto
	{ _IIR_2K, _IIR_4K, _IIR_4K, _IIR_4K, _IIR_4K, _IIR_4K, _IIR_4K, _IIR_4K }, //!< 0.5km
	{ _IIR_512, _IIR_2K, _IIR_2K, _IIR_2K, _IIR_2K, _IIR_2K, _IIR_2K, _IIR_2K }, //!< 1km
	{ _IIR_512, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K }, //!< 2km
	{ _IIR_256, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K }, //!< 5km
	{ _IIR_256, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K }, //!< 10km
	{ _IIR_256, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K }, //!< 20km
	{ _IIR_256, _IIR_512, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K }, //!< 50km
	{ _IIR_256, _IIR_256, _IIR_256, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K }, //!< 100km
	{ _IIR_256, _IIR_256, _IIR_256, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K, _IIR_1K }, //!< 200km
};

// 存储实时模式下每个量程的滤波系数(量程下的所有小任务使用同一系数)
const OPM_FILTER rt_filter[OPM_DISTRANGE_KIND] =
{
    _IIR_512, 	//!< auto
    _IIR_512, 	//!< 0.5km
    _IIR_512, 	//!< 1km
    _IIR_512, 	//!< 2km
	_IIR_512, 	//!< 5km
    _IIR_512, 	//!< 10km
    _IIR_512, 	//!< 20km
    _IIR_512, 	//!< 50km
    _IIR_512, 	//!< 100km
    _IIR_512, 	//!< 200km
};

// 移相插补表
const unsigned int iPhaseShiftReg[OPM_DISTRANGE_KIND][2] =
{
		{INTP01_01, INTP01_01}, // auto
		{INTP01_64, INTP01_64}, // 0.5km
		{INTP01_64, INTP01_64}, // 1km
		{INTP01_32, INTP01_32},	// 2km
		{INTP01_16, INTP01_16}, // 5km
		{INTP01_08, INTP01_08}, // 10km
		{INTP01_04, INTP01_04}, // 20km
		{INTP01_02, INTP01_02}, // 50km
		{INTP01_01, INTP01_01}, // 100km
		{INTP01_01, INTP01_01}, // 200km
};

// 采样比率表
const float fSampleRatio[OPM_DISTRANGE_KIND][2] =
{
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // auto
	{ SMP_COUNT01_64, SMP_COUNT01_64 }, // 0.5km
	{ SMP_COUNT01_64, SMP_COUNT01_64 }, // 1km
	{ SMP_COUNT01_32, SMP_COUNT01_32 }, // 2km
	{ SMP_COUNT01_16, SMP_COUNT01_16 }, // 5km
	{ SMP_COUNT01_08, SMP_COUNT01_08 }, // 10km
	{ SMP_COUNT01_04, SMP_COUNT01_04 }, // 20km
	{ SMP_COUNT01_02, SMP_COUNT01_02 }, // 50km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 100km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 200km
};

/*
// 移相插补表
const unsigned int iPhaseShiftReg[OPM_DISTRANGE_KIND][2] =
{
		{INTP01_01, INTP01_01}, // auto
		{INTP01_01, INTP01_01}, // 0.5km
		{INTP01_01, INTP01_01}, // 1km
		{INTP01_01, INTP01_01},	// 2km
		{INTP01_01, INTP01_01}, // 5km
		{INTP01_01, INTP01_01}, // 10km
		{INTP01_01, INTP01_01}, // 20km
		{INTP01_01, INTP01_01}, // 50km
		{INTP01_01, INTP01_01}, // 100km
		{INTP01_01, INTP01_01}, // 200km
};

// 采样比率表
const float fSampleRatio[OPM_DISTRANGE_KIND][2] =
{
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // auto
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 0.5km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 1km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 2km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 5km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 10km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 20km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 50km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 100km
	{ SMP_COUNT01_01, SMP_COUNT01_01 }, // 200km
};
*/
#if 0

// 移相插补表
const unsigned int iPhaseShiftReg[OPM_DISTRANGE_KIND][2] =
{
		{INTP01_01, INTP01_01}, // auto
		{INTP01_16, INTP01_64}, // 0.5km
		{INTP01_16, INTP01_64}, // 1km
		{INTP01_08, INTP01_32},	// 2km
		{INTP01_04, INTP01_16}, // 5km
		{INTP01_02, INTP01_08}, // 10km
		{INTP01_01, INTP01_04}, // 20km
		{INTP01_01, INTP01_02}, // 50km
		{INTP02_01, INTP01_01}, // 100km
		{INTP02_01, INTP01_01}, // 200km
};

// 采样比率表
const float fSampleRatio[OPM_DISTRANGE_KIND][2] =
{
    { SMP_COUNT01_01, SMP_COUNT01_01 }, // auto
    { SMP_COUNT01_16, SMP_COUNT01_64 }, // 0.5km
    { SMP_COUNT01_16, SMP_COUNT01_64 }, // 1km
    { SMP_COUNT01_08, SMP_COUNT01_32 }, // 2km
    { SMP_COUNT01_04, SMP_COUNT01_16 }, // 5km
    { SMP_COUNT01_02, SMP_COUNT01_08 }, // 10km
    { SMP_COUNT01_01, SMP_COUNT01_04 }, // 20km
    { SMP_COUNT01_01, SMP_COUNT01_02 }, // 50km
    { SMP_COUNT02_01, SMP_COUNT01_01 }, // 100km
    { SMP_COUNT02_01, SMP_COUNT01_01 }, // 200km
};


// 根据不同的脉宽和量程设置修改通道参数
unsigned int SetAttenuation(int pulseWidth, 
	const OPM_ATTE *attr[OPM_PULSE_KIND], 
	int taskNum,
	float autoFiberLen,
	int mode)          // 0:平均模式 1:实时模式
{
	// 衰减量
	float rangeDial[] = {0, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000};
	unsigned int attrnuation = attr[pulseWidth][taskNum];	
	if (!mode)
    {
		if ((pulseWidth == ENUM_PULSE_200NS)
			&& (rangeDial[ENUM_FIBER_RANGE_2KM] >= autoFiberLen))
		{
			attrnuation = attr[pulseWidth][taskNum + 4];
		}
    }
    if (mode)
    {
    	if ((pulseWidth < ENUM_PULSE_500NS)
			&& (pulseWidth >= ENUM_PULSE_50NS))
    	{
    		attrnuation = attr[pulseWidth][3];
    	}
		else
		{
			if (pulseWidth >= ENUM_PULSE_2US)
				attrnuation = attr[pulseWidth][0];
			else
				attrnuation = attr[pulseWidth][1];
		}		
    }
	//printf("-----------attrnuation=%u---------\n",attrnuation);
	return attrnuation;
}

// 修改累加次数
unsigned int SetFilters(int range,
	int pulseWidth, 
	const OPM_ATTE *attr[OPM_PULSE_KIND], 
	int taskNum,
	int mode)          // 0:平均模式 1:实时模式
{
	// 衰减量
	int filters = avg_filter[range][taskNum];
	if (!mode)
	{
		if(pulseWidth == ENUM_PULSE_5NS)
			filters = avg_filter[range][taskNum] + 1;
	}
	else
	{
		filters = rt_filter[range];
	}
	return filters;
}

#endif

// 获取偏移量列表
int CalFiberOffset(unsigned int RefValue,
	float smpRatio,
	int range,
	int resolution)
{
	if (RefValue)
	{
		// 整定的机器使用整定值
		return (int)(RefValue / (ADC_MAX_PHASE / smpRatio));
	}
	else
	{
		// 未整定的使用经验值
		//return(offsetExp[range][resolution]);
	}
	return 1;
}

// 计算采样间隔时间
unsigned int CalAdcGapValue(unsigned int dataLen,
	float smpRatio, 
	float autoFiberLen,
	int pulse) 
{
	unsigned int uiAdcGap = 0x300;	
	float measureRange = 2.56f * dataLen / smpRatio;

	//printf("measureRange = %f, dataLen = %d, smpRatio = %f, autoFiberLen = %f", measureRange, dataLen, smpRatio, autoFiberLen);

	/*
	if(measureRange / 5.00f < autoFiberLen)
	{
		float flyPeriod = (autoFiberLen - measureRange / 5.0f ) * 10.0f;
		uiAdcGap = (unsigned int)(flyPeriod / 25) + 0xa00;
	}
	else
		uiAdcGap = 0x300;
	*/

	float times = 1.8f;
	if (measureRange < 90000.0f)
	{
		times = 2.2f;
	}

	if(measureRange < autoFiberLen * times)
	{
		float flyPeriod = (autoFiberLen * times - measureRange) * 10.0f;
		uiAdcGap = (unsigned int)(flyPeriod / 25) + (unsigned int)(autoFiberLen * 0.3f / 2.5f);
	}
	else
		uiAdcGap = 0x300;
	
	uiAdcGap = Max(uiAdcGap, 0x300);
	uiAdcGap = Min(uiAdcGap, 0x3000);
	
	if(pulse == 1)
	{
		uiAdcGap = Min(0x1000, uiAdcGap);
	}
	
	return uiAdcGap;
}





