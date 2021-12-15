/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_eventssearch.h
* 摘    要：  事件查找算法函数定义
*
* 当前版本：  v1.0.0 
* 作    者：  sjt
* 完成日期：  2020-8-21
* 
*******************************************************************************/

#ifndef _APP_EVENTSEARCH_H
#define _APP_EVENTSEARCH_H

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

#define MAX_EVT_NUM              500							// 界面上显示的最大事件数量		
#define SPLITER_NUM              3                          // 分路器最大个数
#define SNR_KEY_POINT_NUM        3                          // 分段点最大个数
#define NORMAL_ATTENUATION_DOWN  0.00   					// 正常衰减率(下限)
#define NORMAL_ATTENUATION_UP    0.70   					// 正常衰减率(上限)
#define UNNORMAL_ATTENUATION     3      					// 非正常衰减
#define MAX_ATTENUATION_DISPLAY  1.5    					// 最大显示衰减率
#define END_MIN_FLOSS_THRESH     1.5    					// 末端最小损耗阈值
#define END_REFLECT_THRESH       -40    					// 末端反射率阈值
#define MIN_REFELCT_EVENT_THRESH -72						//最小反射事件阈值
#define MIN_MEASURE_DISTANCE     3.0f   					// 最小精度
#define OTDR_MIN_FLOSS_THRESH    0.01   					// 最小损耗阈值
#define END_MIN_POWER_DB         2.5
#define END_MAX_POWER_DB         10
#define END_MEAN_POWER_DB        3.0
#define SOLA_EFFECTIVE_RANGE_SNR 0.03   					// 有效区域的SNR
#define SOLA_END_MEAN_POWER_DB	 6.5
#define SOLA_END_MIN_POWER_DB	 4.5
#define MEASURE_LIMITATION_1310_20US  78000					// 1310nm的波长测试最大距离
#define MEASURE_LIMITATION_1550_20US  125000    			// 1550nm的波长测试最大距离
#define MEASURE_LIMITATION_1550_2US   105000    			// 1550nm的波长2us测试最大距离
#define SOLA_DEFAULT_END_FLOSSTHRESH 5.0f					// sola末端默认损耗阈值
#define SOLA_MAX_END_FLOSSTHRESH     25.0f					// sola末端损耗最大阈值
#define FLOSS_ERROR_MAX     1000.0f							// 损耗异常最大值
#define FLOSS_ERROR_MIN     -1000.0f						// 损耗异常最小值


// 分段点的数量，位置以及SNR
typedef struct _snr_key_point
{
	int keyPointNum;
	int keyPoint[SNR_KEY_POINT_NUM];
	float keyPointDistance[SNR_KEY_POINT_NUM];
	float referValue[SNR_KEY_POINT_NUM];
}SNR_KEY_POINT;

// 事件(距离、类型、起始、终点、窗口、最大值点、最小值点)
typedef struct _events_region_info
{
    int iBegin;												// 事件起始;
    int iEnd;                   							// 事件末端;
    int iMaxIndex;              							// 最大值点;
    int iMinIndex;              							// 最小值点;
    int iStyle;                 							// 事件类型;
    int iWindow;                							// 窗口大小;
    // float floss;              							// 事件损耗;
}EVENTS_REGION_INFO;

// 事件列表
typedef struct _events_region_table//前置事件表类型;
{
    EVENTS_REGION_INFO EventsRegionInfo[MAX_EVENTS_NUMBER]; // 事件信息;
    int iEventsNumber;                                      // 事件数量;
    int iBlind;                                             // 盲区宽度
}EVENTS_REGION_TABLE;

// 外部参数
typedef struct _extern_Interface
{
    // 测试参数
    int wave;                                               // 波长
    int range;                                              // 量程
    int pulseWidth;                                         // 脉宽

    // 硬件参数
    int iSampleRating;                                      // 移相次数

    // 事件分析参数
	float fLightSpeed;                                      // 光速
    float fRelay;                                           // 瑞利散射强度
    float fLossThreshold;                                   // 损耗阈值
    float fBackScattering;                                  // 背向散射系数
    float fRefractiveIndex;                                 // 折射率
    float fReflectThreshold;                                // 反射率阈值
    float fFiberEndThreshold;                               // 光纤末端阈值
    float fExcessLength;                                    // 余常系数
}EXTERN_INTERFACE;

/*
 * @brief	 CalThreshOffset								计算小波系数偏移量
 * @para[in] blind                      					事件列表
 * @para[in] level                      					小波分析层数
 * @return                              					
*/
void CalThreshOffset(int blind, int level);

/*
 * @brief	 CalTotalFloss									计算累计损耗
 * @para[in] eventTable										事件列表
 * @return
*/
void CalTotalFloss(EVENTS_TABLE* eventTable);

/*
 * @brief	 CheckEcho										回波事件判别
 * @para[in] pAlgorithmInfo									算法结构体
 * @return
*/
void CheckEcho(ALGORITHM_INFO *pAlgorithmInfo);

/*
 * @brief	 SearchEdge										查找末端
 * @para[in] pInputSignal               					小波信号
 * @para[in] fThr                       					检测阈值
 * @para[in] iWindow                    					小波窗口大小
 * @para[in] pEventsRegionTable         					事件列表
 * @para[in] level                      					小波层级
 * @para[in] pAlgorithmInfo             					算法结构体
 * @return
*/
void SearchEdge(
    float *pInputSignal,     								// 归一化后的小波信号
    const float fThr,        								// 检测阈值
    const int iWindow,       								// 窗口大小
    EVENTS_REGION_TABLE *pEventsRegionTable,				// 事件数组
    int level,				 								// 小波层级
    ALGORITHM_INFO *pAlgorithmInfo);						// 算法结构体

/*
 * @brief	 GetShortFiberWaveCoef							获取短距离小波阈值(特殊处理
 * @para[in] WaveLength                                     波长
 * @para[in] level                                          小波层级
 * @para[in] Thresh                                         检测阈值
 * @para[in] fSampleRating                                  分辨率
 * @para[in] Distance                                       距离
 * @para[in] iWindow                                        窗口大小
 * @para[in] iSignalLength                                  小波系数的长度
 * @para[in] fsignal                                        小波系数
 * @return
*/
float GetShortFiberWaveCoef(int WaveLength,
    int level,
    int Thresh,
    float fSampleRating,
    float Distance,
    float iWindow,
    int iSignalLength,
    float *fsignal);

/*
 * @brief	 GetNormalSnrWaveCoef							获取信噪比一般的小波系数的阈值
 * @para[in] level                                          小波层级
 * @para[in] Thresh                                         检测阈值
 * @para[in] Distance                                       距离
 * @para[in] fMaxNoiseValue                                 噪声的最大值
 * @return                                                  当前的阈值
*/
float GetNormalSnrWaveCoef(int level,
	float Thresh,
	float Distance,
	float fMaxNoiseValue);

/*
 * @brief	 GetNormalSnrWaveConfidenceCoef					获取信噪比一般的小波系数的阈值
 * @para[in] signalLength                                   信号长度
 * @para[in] distancePerPoint                               每点代表的物理长度（m）
 * @para[in] coef											阈值
 * @para[in] level											层级
 * @return                                                  当前的阈值
*/
void GetNormalSnrWaveConfidenceCoef(int signalLength,
	float distancePerPoint,
	float coef,
	int level);

/*
 * @brief	 CalfTmpThrEachSection							自适应计算分段阈值，非固定的阈值
 * @para[in] startIndex										起始点
 * @para[in] SectionTotalLen                                选取信号的长度
 * @para[in] SectionNum										选取的段数
 * @para[in] level											层级
 * @para[in] coef											阈值
 * @return                                                  当前的阈值
*/
float CalfTmpThrEachSection(int startIndex,
	int SectionTotalLen,
	int SectionNum,
	int level,
	float coef);

/*
 * @brief	 GetGoodSnrWaveCoef					            获取信噪比较好情况下的小波阈值
 * @para[in] level											层级
 * @para[in] Thresh											阈值
 * @para[in] Distance                                       距离
 * @return                                                  当前的阈值
*/
float GetGoodSnrWaveCoef(int level,
	float Thresh,
	float Distance);

/*
* @brief	 GetEventsSearchRange							获取事件查找范围
* @param[in] pAlgorithmInfo									算法总结构体
* @return  
*/
void GetEventsSearchRange(ALGORITHM_INFO *pAlgorithmInfo);

/*
 * @brief	 GetFinalRange						            获取事件查找的最终范围
 * @para[in] pAlgorithmInfo									算法总结构体
 * @para[in] EndDist										计算的末端位置
 * @para[in] fEndNoise                                      噪声幅值
 * @para[in] Max_Wavelet_position                           小波系数最大值处
 * @return                                                  
*/
void GetFinalRange(ALGORITHM_INFO *pAlgorithmInfo,
	float EndDist, 
	float fEndNoise,
	int Max_Wavelet_position);

/*
* @brief	 GetFiberEnd									查找光纤末端
* @param[in] pAlgorithmInfo									算法总结构体
* @return
*/
void GetFiberEnd(ALGORITHM_INFO *pAlgorithmInfo);

/*
* @brief	 ParaAnlysis									参数分析
* @param[in] pEventsTable									事件列表
* @return
*/
void ParaAnlysis(EVENTS_TABLE *pEventsTable);

/*
* @brief	 EventsTableSort2								事件表排序（pEventsTable）
* @param[in] pEventsTable									事件列表
* @return
*/
void EventsTableSort2(EVENTS_TABLE *pEventsTable);

/*
 * @brief	 GetRealBegin									优化事件点的位置
 * @para[in] pulseWidth               						脉宽
 * @para[in] EventsRegionInfo         						事件区间信息
 * @para[in] fSampleRating             						采样比率
 * @return
*/
int GetRealBegin(int pulseWidth,
	EVENTS_REGION_INFO EventsRegionInfo,
	float fSampleRating);

// 获取小波信号
void GetWaveletCoef(ALGORITHM_INFO *pAlgorithmInfo);

// 获取事件特诊信息
void GetEventsTable(ALGORITHM_INFO *pAlgorithmInfo);

// 计算链路总损耗
void GetTotalReturnLoss(ALGORITHM_INFO *pAlgorithmInfo);

// 获取初始事件位置信息
void GetFrontEventsTable(ALGORITHM_INFO *pAlgorithmInfo);

// 根据事件起始位置将事件表排序序(EVENTS_REGION_TABLE)
void EventsTableSort(EVENTS_REGION_TABLE *pEventsRegionTable);

/*
 * @brief	 SearchFlatRegion						        寻找光纤平坦区域
 * @para[in] sig											小波信号
 * @para[in] len											信号长度
 * @para[in] smpInterval                                    采样间隔
 * @para[in] smpInterval                                    采样间隔
 * @para[in] threshEnd			                            末端阈值
 * @return
*/
int SearchFlatRegion(float *sig,
	int len,
	int smpInterval,
	int threshEnd);

/*
 * @brief	 LineFit								        一次线性拟合函数（最小二乘法）
 * @para[in] pSignal										输入信号
 * @para[in] iSignalLength									拟合长度
 * @para[in] iBegin		                                    拟合起始点
 * @para[in] pFitCoef	                                    拟合结果
 * @return
*/
void LineFit(float *pSignal,
	const int iSignalLength,
	const int iBegin,
	float *pFitCoef);									
								 						
/*
 * @brief	 LineFitTwoPoint						        两点法拟合直线
 * @para[in] fPointA										A点纵坐标
 * @para[in] iPointA										A点横坐标
 * @para[in] fPointB										B点纵坐标
 * @para[in] iPointB		                                B点横坐标
 * @para[in] pFitCoef	                                    拟合结果
 * @return
*/
void LineFitTwoPoint(float fPointA,
	int iPointA,
	float fPointB, 
	int iPointB,
	float *pFitCoef);	

/*
 * @brief	 RegionIntegration								事件合并
 * @para[in] pAnsEventsTable               					已有结果
 * @para[in] pInputEventsTable         						新输入的事件区间信息
 * @return
*/
void RegionIntegration(EVENTS_REGION_TABLE *pAnsEventsTable,
	EVENTS_REGION_TABLE *pInputEventsTable);

/*
 * @brief	 GetGoodSnrWaveCoef2							获取小波系数
 * @para[in] level                                          小波层级
 * @para[in] Thresh                                         检测阈值
 * @para[in] Distance                                       距离
 * @para[in] referValue                                     小波系数偏移量
 * @para[in] maxThresh										最大阈值
 * @para[in] snrKeyPoint                                    分段点
 * @return
*/
float GetGoodSnrWaveCoef2(int level,
	float Thresh,
	float Distance,
	const float referValue, 
	float maxThresh,
	SNR_KEY_POINT* snrKeyPoint);	
	
/*
 * @brief	 SetSpliterStyle								设置分路器
 * @para[in] pEventsTable                                   事件列表
 * @para[in] index1                                         遍历事件列表的起始索引
 * @para[in] index2											遍历事件列表的终止索引
 * @para[in] spliterNum                                     分路器个数
 * @para[in] goalSpliterRatio								目标分路器比率
 * @para[in] grade		                                    分路器的级别
 * @return
*/
void SetSpliterStyle(EVENTS_TABLE *pEventsTable, 
	int index1, 
	int index2, 
	int spliterNum, 
	int *goalSpliterRatio, 
	int grade);

/*
 * @brief	 NormWaveletCoef								归一化各级小波信号
 * @para[in] pNormSignal                                    待归一化的小波信号
 * @para[in] iSignalLength                                  小波信号长度
 * @para[in] iEffective_Len									信号有效长度
 * @para[in] iNormWindow                                    归一化窗口宽度
 * @para[in] iWaveletWindow  								小波窗口宽度
 * @para[in] iMaxEventsNumber		                        最大事件数量查找阈值
 * @para[in] iIterationNumber		                        迭代次数
 * @return
*/
void NormWaveletCoef(float *pNormSignal,
	const int iSignalLength,
	const int iEffective_Len,
	const int iNormWindow,
	const int iWaveletWindow,
	const int iMaxEventsNumber,
	const int iIterationNumber);
   
/*
 * @brief	 NoiseEstimata									估计非平稳噪声阈值
 * @para[in] pInputSignal                                   待估计的小波信号
 * @para[in] iSignalLength                                  小波信号长度
 * @para[in] iEffective_Len									信号有效长度
 * @para[in] iNormWindow                                    估计窗口宽度
 * @para[in] iStep  										噪声估计步长
 * @para[in] fSigma											噪声估计系数
 * @para[in] pNoiseThr										噪声估测阈值
 * @para[in] pNoiseMean										噪声估计均值
 * @return
*/
void NoiseEstimata(float *pInputSignal, 
	const int iSignalLength,
	const int iEffective_Len,
	const int iNormWindow,
	const int iStep,
	const float fSigma,
	float *pNoiseThr, 
	float *pNoiseMean);
    
/*
 * @brief	 SearchRegion								    初步查找各级小波的事件范围
 * @para[in] level											小波层级
 * @para[in] fThr											检测阈值
 * @para[in] pEventsRegionTable		                        事件区间信息
 * @para[in] pAlgorithmInfo	                                算法总结构体
 * @return
*/
void SearchRegion(int level,
	const float fThr,
	EVENTS_REGION_TABLE *pEventsRegionTable,
	ALGORITHM_INFO *pAlgorithmInfo);
		
/*
 * @brief	 SearchEdge2								    查找小波域的上升沿和下降沿2
 * @para[in] pInputSignal									输入的小波信号	
 * @para[in] iWindow										窗口
 * @para[in] pEventsRegionTable		                        事件区间信息
 * @para[in] level					                        小波层级
 * @para[in] pAlgorithmInfo	                                算法总结构体
 * @return
*/
void SearchEdge2(float *pInputSignal,
	const int iWindow, 
	EVENTS_REGION_TABLE *pEventsRegionTable,
	int level,
	ALGORITHM_INFO *pAlgorithmInfo);

/*
 * @brief	 PeakRegion									    找出上升区间和下降区间
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] iSignalLength									信号长度
 * @para[in] fLowThr										低阈值
 * @para[in] fHighThr					                    高阈值
 * @para[in] pRiseRegion					                上升区间
 * @para[in] pFallRegion	                                下降区间
 * @return
*/
void PeakRegion(float *pInputSignal,
	const int iSignalLength,
	const float fLowThr, 
	const float fHighThr,
	int pRiseRegion[],
	int pFallRegion[]);

/*
 * @brief	 CalEventFloss									计算事件损耗
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] index											事件索引
 * @para[in] pEventsRegionTable								事件区间信息
 * @para[in] pCoef											拟合系数
 * @para[in] pLoss											事件损耗
 * @para[in] stdNoise										噪声方差
 * @para[in] PrelsaFlag										事件点前端使用最小二乘法的标志
 * @para[in] BacklsaFlag	                                事件点后端使用最小二乘法的标志
 * @return
*/
void CalEventFloss(float *pInputSignal,
	int index,
	EVENTS_REGION_TABLE *pEventsRegionTable,
	float pCoef[][2],
	float pLoss[][2],
	float stdNoise,
	int PrelsaFlag,
	int BacklsaFlag);

/*
 * @brief	 EventsAnalysis									事件分析函数，寻找事件的详细区间并定位，给出事件类型
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] iSignalLength									信号长度
 * @para[in] iBlind											盲区宽度
 * @para[in] pEventsRegionTable					            事件区间信息
 * @para[in] fSampleRating					                采样比率
 * @return
*/
void EventsAnalysis(float *pInputSignal,
	const int iSignalLength,
    const int iBlind,
    EVENTS_REGION_TABLE *pEventsRegionTable,
   	float fSampleRating);

/*
 * @brief	 GetEventsParameter								计算事件特征参数
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] iSignalLength									信号长度
 * @para[in] pEventsRegionTable					            事件区间信息
 * @para[in] pEventsTable						            最终事件表信息
 * @para[in] fSampleRating					                采样比率
 * @return
*/
void GetEventsParameter(float *pInputSignal,
	const int iSignalLength,
	EVENTS_REGION_TABLE *pEventsRegionTable,
	EVENTS_TABLE *pEventsTable,
	float fSampleRating);
   	 			
/*
 * @brief	 GetEventsParameter								事件损耗计算函数
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] iSignalLength									信号长度
 * @para[in] pEventsRegionTable					            事件区间信息
 * @para[in] pCoef											拟合参数
 * @para[in] pLoss								            事件损耗pLoss[][0] 前损耗 pLoss[][1]区间损耗
 * @para[in] fSampleRating					                采样比率
 * @return
*/
void GetEventsTableLoss(float *pInputSignal,
	const int iSignalLogLength,
	EVENTS_REGION_TABLE *pEventsRegionTable,
	float pCoef[][2],
	float pLoss[][2],
	float fSampleRating);			

/*
 * @brief	 GetReflectAndLength							求反射率和事件区间长度函数
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] iSignalLength									信号长度
 * @para[in] fRelay											瑞利散射强度
 * @para[in] pEventsRegionTable								事件区间信息
 * @para[in] pCoef											拟合系数
 * @para[in] pReflect										反射率参数
 * @para[in] pDelta										    事件区域最大最小值差距
 * @para[in] pEventsFiberLength								事件长度
 * @para[in] fSampleRating	                                采样比率
 * @return
*/
void GetReflectAndLength(float *pInputSignal,
	const int iSignalLength,
	const float fRelay,
	EVENTS_REGION_TABLE *pEventsRegionTable,
	float pCoef[][2],
	float *pReflect,
	float *pDelta,
	int *pEventsFiberLength,
	float fSampleRating);

/*
 * @brief	 GetReflectCoff									光纤端面检查
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] iLen											信号长度
 * @return
*/
unsigned int GetReflectCoff(unsigned short *pSignal, int iLen);
	
/*
 * @brief	 SearchEnd										查找光纤末端
 * @para[in] pInputSignal									输入的对数信号
 * @para[in] iSignalLength									信号长度
 * @para[in] fEndThr										末端阈值
 * @return
*/
int SearchEnd(float *pInputSignal,
	int iSignalLength,
	float fEndThr);	
	
/*
 * @brief	 EndAnalysis									光纤末端分析：损耗阈值>末端后面反射事件>末端前面事件
 * @para[in] pAlgorithmInfo									输入的对数信号
 * @para[in] fEndThr										末端阈值
 * @para[in] iTail											末端位置
 * @return
*/
void EndAnalysis(ALGORITHM_INFO *pAlgorithmInfo,
	const float fEndThr,
	int iTail);

/*
 * @brief	 GetEventsMaxMinIndex							判断pEventsRegionTable事件最大最小值索引位置
 * @para[in] pInputSignal									输入的小波系数
 * @para[in] pEventsRegionTable								事件区间信息
 * @return
*/
void GetEventsMaxMinIndex(float *pInputSignal,
	EVENTS_REGION_TABLE *pEventsRegionTable);

/*
 * @brief	 TransEventsTable								将事件g_pEventsRegionTable跟着pEventsTable改变
 * @para[in] pEventsTable									输入的小波系数
 * @para[in] pEventsRegionTable								事件区间信息
 * @return
*/
void TransEventsTable(EVENTS_TABLE *pEventsTable,
	EVENTS_REGION_TABLE *pEventsRegionTable);
	
/*
 * @brief	 SearchEnd										光纤长度估算
 * @para[in] pSignal										输入的对数信号
 * @para[in] iLen											信号长度
 * @para[in] iFiberLength									光纤长度
 * @para[in] END											末端阈值
 * @para[in] iflag											末端标志
 * @return
*/
int GetFiberLength(unsigned short *pSignal,
	int iLen,
	unsigned int *iFiberLength,
	float END,
	int iflag);							   						
				        				   								   						      
/*
 * @brief	 GetStartPoint									查找事件点初始位置
 * @para[in] pAlgorithmInfo									输入的小波系数
 * @return
*/
int GetStartPoint(ALGORITHM_INFO *pAlgorithmInfo);

/*
 * @brief	 IsEventPassed									事件通过判断
 * @para[in] event											事件列表
 * @return
*/
int IsEventPassed(const EVENTS_INFO* event);

/*
 * @brief	 GenerateSolaTable								生成总的事件列表
 * @para[in] pSolaAlgInfo									各个波长测得的事件信息
 * @return
*/
void GenerateSolaTable(SOLA_INFO *pSolaAlgInfo);

/*
 * @brief	 SOLAMergeEventsTable							事件合并算法总函数
 * @para[in] pNewAlgInfo									新测得的事件信息
 * @para[in] pSolaAlgInfo									当前的事件信息
 * @return
*/
void SOLAMergeEventsTable(ALGORITHM_INFO *pNewAlgInfo,
	SOLA_INFO *pSolaAlgInfo);

/*
 * @brief	 SOLAMergeEventsTable							不同的脉宽事件进行合并
 * @para[in] CurAlgorithmStruct								当前的事件信息
 * @para[in] RetSola										最终的事件信息
 * @return
*/
void MergeDifferentPulseEventLis(ALGORITHM_INFO *CurAlgorithmStruct,
	ALGORITHM_INFO *RetSola);

/*
 * @brief	 JudgeMacrobendingLossEvent						判断宏弯曲事件 
 * @para[in] pSolaAlgInfo									事件信息
 * @return
*/
void JudgeMacrobendingLossEvent(SOLA_INFO *pSolaAlgInfo);

/*
 * @brief	IsEventExist									判断事件是否存在 
 *@para[in] EventsTable										事件列表
 *@para[in] postion 										事件位置
 *@return													事件索引位置
*/
int IsEventExist(EVENTS_TABLE *EventsTable, float position);

/*
 * @brief	 SpliterJudgeAgain								分路器再次判断
 * @para[in] SolaAlgorithm									事件信息列表
 * @para[in] waveLen                                        测量波长
 * @return
*/
void SpliterJudgeAgain(SOLA_INFO *SolaAlgorithm, int waveLen);

/*
 * @brief	 FindKeyPoint									获取动态阈值分界点
 * @para[in] sig											输入信号
 * @para[in] stdArray										放长
 * @para[in] startPos										起始点
 * @para[in] calTime										计算次数
 * @para[in] pulse											脉宽
 * @para[in] iterval										间隔
 * @para[in] continueNum									持续次数
 * @para[in] stdSmpRatio									采样比率
 * @para[in] snrKeyPoint	                                分界点信息
 * @para[in] mode											模式
 * @return
*/
void FindKeyPoint(float* sig,
	float* stdArray,
	int startPos,
	int calTime,
	int pulse,
	int iterval,
	int continueNum,
	float stdSmpRatio,
	SNR_KEY_POINT *snrKeyPoint,
	int mode);

/*
* @brief    GetEffectiveFiberRange							计算信号信噪比SNR,获取不同脉宽下的有效距离
* @para[in] pNewAlgInfo										算法信息
* @return													有效距离
*/
float GetEffectiveFiberRange(ALGORITHM_INFO *pNewAlgInfo);

/*
* @brief    GetEffectiveFiberRange_mini2					计算信号信噪比SNR,获取不同脉宽下的有效距离
* @para[in] pNewAlgInfo										算法信息
* @return													有效距离信息
*/
SNR_KEY_POINT* GetEffectiveFiberRange_mini2(ALGORITHM_INFO *pNewAlgInfo);

/*
 * @brief    NormalAttenuationSectionExist					判断后续信号是否有正常区间
 * @para[in] pAlgorithmInfo									算法总结构体 
 * @para[in] index											事件索引
 * @return													是否有正常区间     
*/
int NormalAttenuationSectionExist(ALGORITHM_INFO *pAlgorithmInfo, int index);

/*
 * @brief    CompareFloat									比较函数
 * @para[in] a												a
 * @para[in] b												b
 * @return													a>=b
*/
int CompareFloat(const void *a, const void *b);

/*
 * @brief    AdjustWaveThresh								针对sola测量，使用较小脉宽时，自适应小波阈值
 * @para[in] coef											小波系数
 * @para[in] coefLen										信号长度
 * @return													阈值
*/
float AdjustWaveThresh(float *coef, int coefLen);

/*
 * @brief    FixPositionAccurate							精确定位事件位置
 * @para[in] pAlgorithmInfo									算法总结构体
 * @return
*/
void FixPositionAccurate(ALGORITHM_INFO *pAlgorithmInfo);

/*
 * @brief    IntTransferLog									对数转换
 * @para[in] idata											原始信号
 * @para[in] len											信号长度
 * @para[in] flogdata										对数信号
 * @return
*/
void IntTransferLog(unsigned short *idata, int len, float *flogdata);

/*
 * @brief    MeasureLimitation								针对不同波长、脉宽定义测试极限、弥补有效距离定位的精度不够问题
 * @para[in] distance										距离
 * @para[in] level											层级
 * @para[in] referenceValue									偏移量
 * @para[in] currentThresh									当前阈值
 * @return
*/
float MeasureLimitation(float distance, 
	int level,
	float referenceValue,
	float currentThresh);

/*
 * @brief    SolaGetFirstEventLoss							计算第一个事件损耗
 * @para[in] logSig											对数信号
 * @para[in] sigLen											信号长度
 * @para[in] startOffset									起始偏移量
 * @para[in] event											事件列表
 * @return
*/
void SolaGetFirstEventLoss(float *logSig,
	int sigLen,
	int startOffset,
	EVENTS_INFO* event);

/*
 * @brief    InsertEvent									事件合并添加 
 * @para[in] CurAlgorithmStruct								当前算法结构体
 * @para[in] RetSola										返回的算法结构体
 * @para[in] curIndex										当前索引
 * @para[in] retIndex										返回的索引
 * @para[in] flag											合并的标志
 * @return
*/
void InsertEvent(ALGORITHM_INFO *CurAlgorithmStruct,
	ALGORITHM_INFO *RetSola,
	int curIndex,
	int retIndex,
	int flag);

// 设置每个点代表的距离 
void SetLengthPerPoint(float fLengthPerPoint);

// 设置每个事件的测试波长
void SetEventsWave(ALGORITHM_INFO *pAlgorithmInfo);

// 设置外部参数
void SetfrontParament(EXTERN_INTERFACE *outsideFrontParament);

/***************************************************************
* 以下代码是保存及打印
****************************************************************/
// 打印FrontEventsTable（界面）
void FrontEventsTable_printf(EVENTS_REGION_TABLE *EventsTable);
// 打印EventsTable（界面）
void FiberEndEventsTable(EVENTS_TABLE *EventsTable);
// 保存事件分析的参数（文件中）
void SaveEventAnalyseParam(ALGORITHM_INFO *pAlgorithmInfo);
// 打印小波系数（文件中）
void PrintWaveLetCoef(float *sig, int sigLen, int num, int pulse);
// 打印小波阈值（文件中）
void PrintWaveLetThresh(float *sig, int sigLen, int num, int pulse);
// 打印小波系数的std
void PrintWaveLetStd(float *sig, int sigLen, int pulse);
// 打印波形（文件中）
void PrintAlgorithmInfo(ALGORITHM_INFO *pAlgorithmInfo, int pulse);
// 打印事件信息（文件中）
void PrintEventInfo(FILE* stream, EVENTS_TABLE* events);
// 打印事件区间信息（文件中）
void PrintEventRegionInfo(FILE* stream, EVENTS_REGION_TABLE* eventRegion);

#ifdef __cplusplus
}
#endif

#endif
