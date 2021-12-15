/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_sigcombine.h
* 摘    要：  otdr通道组合函数定义
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-31
*
*******************************************************************************/

#ifndef APP_SIGCOMBINE_H
#define APP_SIGCOMBINE_H

#ifdef __cplusplus 
extern "C" {
#endif

// #include <math.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
#include "app_math.h"
// #include "app_algorithm_support.h"
#include "app_otdr_algorithm_parameters.h"

#define MAX_TASK_NUM    	3						// OTDR测试任务最大数(1310 1550 1625)
#define MAX_DATA_NUMBER (20)						// 定义最大组合次数;
#define MAX_COMBINE_NUMBER (500)					// 最大组合区间数量;

    typedef struct _combine_pos						// 组合位置结构体,保留最终的组合位置信息;
    {
        int iComPosition;							// 组合位置;
        int iComStyle;								// 区间组合点类型:饱和组合、区间组合、事件组合;
        int iErrorFlag;								// 错误标识位：击穿、黑电平错误等;
    }COMBINE_POS;

    // 组合算法信息结构类型定义;
    typedef struct _combine_para					// 组合参数结构体;
    {
        float *pData;								// 输入原始对数数据;
        unsigned int *pOriData;                     // 输入原始数据;经累加后的原始数据
        float *pLogData;                            // 输入对数数据;经累加后的对数数据
        int iSumNum;								// 累加次数;
        int iComRegion[MAX_COMBINE_NUMBER][2];		// 区间组合点位置;  第一列记录饱和区间的起始点，第二列记录饱和区间的长度  added by lichuyuan 2015.03.18
        int iComStyle;								// 区间组合点类型:饱和组合、区间组合、事件组合;
        int iCombineNum;							// 组合区间数量;
        int iSavePosition;							// 信号保留区间(高信噪比区间);
        int iEndPosition;							// 信号末端位置;
        int iComPosition;							// 组合位置;
        int iInputBlacklevel;						// 输入信号黑电平;
        int iErrorFlag;								// 错误标识位：击穿、黑电平错误等;
        float fComGain;								// 组合增益值;
        int iAttenuation;							// 衰减量;
        float fSatuValue;							// 饱和值;
        float fSatuThr;								// 饱和区间检测阈值;
        float fInputNoise;							// 噪声值;
        float fSaveThr;								// 保留区间检测阈值;
        float fMeanIntensity;						// 信号平均强度;
        float fMaxIntensity;						// 信号最大强度;
    }COMBINE_PARA;

	// 通道组合信息结构体;
    typedef struct _combine_info					
    {
        COMBINE_PARA combine_para[MAX_DAQ_NUM];		// 组合参数;
        COMBINE_POS combine_pos[MAX_DAQ_NUM];		// 组合位置信息;
        float *pCombineData;						// 组合后数据;
        unsigned short *pOriginalData;				// 原始数据暂存指针
        int iApdBreakFlag;							// APD击穿标志位，出现该位代表APD被击穿过;
        int iApdAdjustFlag;                         // APD调整标志位，表明APD上次微调过;
        int iPulseWidth;							// 脉冲档位;
		int iWave;                                  // 波长索引
        int iSigLen;								// 信号长度;
        int iBlind;									// 信号盲区;
        int iDataNum;								// 输入数据个数;当前输入时第几组数据
        int iTotalNum;								// 每次任务队列总的数据个数;
        int iTaskCycle;								// 任务队列循环次数;
        int iCombineFlag;							// 组合标识位，通知函数进行信号组合;
        int iRealTimeFlag;							// 实时测量标志位;
        float fSampleRating;
		int iLogFlg;                                // 取对数标志test
		int ReChanIndex[MAX_DAQ_NUM];               // 重新选择的通道索引号
    }COMBINE_INFO;

	// 组合类型
    typedef enum _combine_style
    {
        ENUM_COMBINE_STYLE_EVENT = 0,				// 事件类型组合
        ENUM_COMBINE_STYLE_NORMAL = 1,              // 正常组合
        ENUM_COMBINE_STYLE_NEEDJUDGE = 2,           // 可能的正常组合
        ENUM_COMBINE_STYLE_END = 3					// 末端组合
    } COMBINE_STYLE;

	// 组合错误类型
    typedef enum _combine_errorflag
    {
        ENUM_COMBINE_ERRORFLAG_NORMAL = 0,			// 正常组合，没有错误
        ENUM_COMBINE_ERRORFLAG_BLACKBREAK = 1,		// 黑电平击穿错误
        ENUM_COMBINE_ERRORFLAG_NOISEBREAK = 2,		// 噪声击穿错误
        ENUM_COMBINE_ERRORFLAG_ATT0ERROR = 3,		// ATT0错误
        ENUM_COMBINE_ERRORFLAG_NOISEESTERROR = 4,	// 噪声估计错误	
        ENUM_COMBINE_ERRORFLAG_SIGNALWEEK = 5,		// 信号过弱错误	
        ENUM_COMBINE_ERRORFLAG_SHORTWAVE = 6,		// 短脉冲错误
        ENUM_COMBINE_ERRORFLAG_ENDERROR = 7			// 末端后面还有组合错误
    } COMBINE_ERRORFLAG;

	// 通道组合主接口
    void SignalCombine(COMBINE_INFO *pCombine_Info);
	// 排除异常点
    void EliminateAbnormalPoints(COMBINE_INFO *pCombine_Info);
	// 时域信号累加及对数转换
    void SigTransform(COMBINE_INFO *pCombine_Info);
	// 对数转换及对数域累加
	void LogSigTransform(COMBINE_INFO *pCombine_Info);
	// 信号特征分析
    void SignatureAnalysis(COMBINE_INFO *pCombine_Info);
	// 查找饱和点
    void FindSatuRegion(COMBINE_INFO *pCombine_Info);
	// 查找保留区间
    void FindSaveRegion(COMBINE_INFO *pCombine_Info);
	// 查找组合点
    void FindCombinePosition(COMBINE_INFO *pCombine_Info);
	// 通道重新选择函数，对组合过程中的通道进行最优选择;
	void ChannelReChoise(COMBINE_INFO *pCombine_Info);
	// 信号组合
    void SigCombine(COMBINE_INFO *pCombine_Info);
	// 小信号进行组合（5ns）
    void SigCombine2(COMBINE_INFO *pCombine_Info);
	// 局部滑动平均
    void Overlapping_averages(float *Signal, int Len, int Mark, int AvgNum);
	// 曲线平滑
    void CurveSmooth(float *Signal, int Len, int Mark);
	//修改反射峰前的过冲点
	void changeEShot(COMBINE_INFO *pCombine_Info);
	//高斯噪声
	void Gaussian_O10(unsigned short *GaussianSignal, int L, float u, float v, float delta);
	//随机生成下一个实数
	int UniformForGaussian(double *p);

#ifdef __cplusplus
}
#endif

#endif