/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_algorithm_support.h
* 摘    要：  算法支持的函数及结构体定义
*
* 当前版本：  v1.0.0
* 作    者：  sjt
* 完成日期：  2020-8-21
*
*******************************************************************************/

#ifndef _APP_ALGORITHM_SUPPORT_H
#define _APP_ALGORITHM_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DAQ_NUM         8							// 采集队列中，数据采集最大个数
#define OPM_DISTRANGE_KIND  10							// 总共设置了10种光纤长度
#define OPM_PULSE_KIND      12							// 脉宽种数;
#define BLACK_LEVEL_LEN		512  						// 噪声数据长度;               
#define ADC_CLOCK_FREQ  	40 							// 采样时钟（MHz）;
#define FPGA_CLOCK_FREQ_1NS 1000						// 采样时钟与脉宽的转化参数（1ns）
#define ADC_MAX_PHASE   	64							// 最大移相插补次数
#define MAX_DATA_VALUE  	65536						// 最大纵坐标强度
#define SCALE_FACTOR        1000						// 实数转化为整数的比例因子
#define SOLA_WAVE_NUM     	3							// 波长个数
#define MAX_EVENTS_NUMBER 	500							// 事件表最大事件数定义;
#define SHROTFIBER			8000.0f                     // 定义的短光纤的长度
#define MIDDLEFIBER			15000.0f                    // 定义的中短光纤的长度

	extern float fLogSig[MAX_DATA_VALUE];               // 对数表
	extern float fPowerSig[MAX_DATA_VALUE];             // 指数表
	
	// 生成指数表
    void PowerTableCreat(void);

    // 生成log对数表1-65535
    void LogTableCreat2(void);

	// 事件通过判断参数
	typedef struct _events_ispass
	{
		int iIsEventLossPass;           				// 事件损耗是否通过 1 通过 0未通过
		int iIsReflectPass;             				// 反射率是否通过 1 通过 0未通过
		int iIsAttenuationPass;        				    // 衰减是否通过 1 通过 0未通过
	}EVENTS_ISPASS;

	//  链路通过判断参数
	typedef struct _link_ispass
	{
		int iIsRegionLossPass;          			    // 链路损耗是否通过 1 通过 0未通过
		int iIsRegionLengthPass;        			    // 链路长度是否通过 1 通过 0未通过
		int iIsRegionORLPass;           			    // 链路光回损是否通过 1 通过 0未通过
	}LINK_ISPASS;

    //事件(详细信息)
    typedef struct _events_info
    {
        int iBegin;										// 事件起点;
        int iEnd;										// 事件终点;
        int iStyle;										// 事件类型
        int iPulse;										// 该事件是哪个脉宽测得的
		int iContinueLength;							// 后续连续光纤长度;
        float fLoss;									// 事件损耗;
        float fDelta;									// 最大最小值衰减;
        float fReflect; 								// 反射率;
        float fAttenuation;								// 衰减率;
        float fContinueAttenuation; 					// 后续光纤损耗;
        float fTotalLoss;								// 累计损耗;
        float fEventsPosition;							// 事件点实际位置(单位为M)
        float fEventsEndPosition;						// 事件的结束位置(单位为M)    
        float fEventReturnLoss;							// 事件回波损耗

        int iWaveSymbol;								// 波长
        int iSmallLossFlag;								// 小损耗事件标志
        int iSmallReflectFlag;							// 小反射率事件标志
    }EVENTS_INFO;

    // 事件列表(详细信息)
    typedef struct _events_table
    {
        EVENTS_INFO EventsInfo[MAX_EVENTS_NUMBER];	    // 事件信息;
        EVENTS_ISPASS EventsIsPass[MAX_EVENTS_NUMBER];  // 事件是否通过;
        LINK_ISPASS LinkIsPass;                         // 链路是否通过
        int iIsPass;                                    // 当前曲线是否通过
        int iEventsStart;							    // 参考起始点
        int iIsNewEvent; 								// 参考起始点是否是新添加的事件
        int iEventsNumber;							    // 事件数量;
        int iLaunchFiberIndex;                          // 注入光纤事件索引 <= 0 表明没有注入光纤
        int iRecvFiberIndex;                            // 接收光纤事件索引 <= 0 表明没有接收光纤
        float fLaunchFiberPosition;                     // 注入光纤实际位置
        float fRecvFiberPosition;                       // 接收光纤实际位置
        int iIsThereASpan;                              // 是否存在跨段（0 不存在 非0 存在）
    }EVENTS_TABLE;

    // 外部输入的设置参数
    typedef struct _set_parameters
    {
        float fLossThreshold;		                    // 定义事件损耗识别阈值;
        float fReflectThreshold;	                    // 反射率检测阈值;
        float fFiberEndThreshold;	                    // 光纤末端判断阈值;
		float fLightSpeed;			                    // 真空中的光速;
        float fRefractiveIndex;		                    // 标准单模光纤折射率;
        float fBackScattering;	                        // 背向散射系数;
        float fSampleRating; 		                    // 采样速率系数;
        float fExcessLength;		                    // 余长系数;
        int iwavelength;			                    // 发射波长;
    }SET_PARAMETERS;

    // 算法外部接口参数
    typedef struct _algorithm_info
    {
		int iOffsetFiberLen;                            // 机器内部光纤长度（点数）
        
        float *pSignalLog;   				            // 存储对数信号首地址;

        int iFiberRange;	 				            // 量程
        int iSignalLength;   				            // 存储输入的数据长度;
        int iDisplayLenth;   				            // 显示长度;
        int iBlind;          				            // 盲区宽度;
		int PulseWidth;						            // 脉冲宽度;
        float fInputNoise;   				            // 末端噪声
        float fMaxInputNoise;   			            // 末端噪声最大值
		
		// 参数信息与事件列表
        SET_PARAMETERS SetParameters;		            // 外部输入设置参数;
        EVENTS_TABLE EventsTable;   		            // 总事件表;
		
		// 组合点信息
        unsigned int pCombineInfo[2][20];               // 输出组合信息;
       
		// 智能链路分析的设置参数
        float solaEffectiveRange;			            // 光纤有效距离
        int spliterRatioGrade1;				            // 分路器1级
        int spliterRatioGrade2;				            // 分路器2级
        int spliterRatioGrade3;				            // 分路器3级
		int iSOLATest;	 					            // 测量方式为SOLA

		float fTotalReturnLoss;				            // 回波损耗
		float fPointLength;         		            // 每个点代表距离
    }ALGORITHM_INFO;

    // sola算法(多波长)
    typedef struct _sola_info
    {
        ALGORITHM_INFO pAlgorithmInfo[SOLA_WAVE_NUM];	// 事件信息;
        EVENTS_TABLE EventsTable;						// 最终的事件列表
    }SOLA_INFO;

    // 笛卡尔坐标系直线参数 y = kx + b
    typedef struct _straight_line
    {
        float   k;										// 斜率
        float   b;										// 截距
    } SLINE;

	/**
	 * @brief	GetAPDStep								获得APD步长
	 * @param 	temperature								当前APD温度
	 * @return											APD步长
	 **/
    int GetAPDStep(float temperature);

	/**
	 * @brief	CalAdcGapValue							计算采样间隔时间
	 * @param 	dataLen									采集数据的长度
	 * @param 	smpRatio								采样分辨率
	 * @param 	autoFiberLen							自动出来的光线长度（m）
	 * @param 	pulse									脉宽
	 * @return											采样间隔
	 **/
    unsigned int CalAdcGapValue(
		unsigned int dataLen,
		float smpRatio,
		float autoFiberLen,
		int pulse);

	 /**
	 * @brief	CalFiberOffset							获取偏移量列表
	 * @param 	RefValue								64移相的情况下需偏移的个数
	 * @param 	smpRatio								采样分辨率
	 * @param 	range									量程
	 * @param 	resolution								移相次数
	 * @return											偏移量
	 **/
    int CalFiberOffset(
		unsigned int RefValue,
		float smpRatio,
		int range,
		int resolution);

	 /**
	 * @brief	CalSamplePointNum						计算采样点长度
	 * @param 	range									量程
	 * @param 	refractiveIndex							折射率
	 * @param 	smpRatio								采样分辨率
	 * @return											采样点长度
	 **/
    int CalSamplePointNum(
		float range,
		float refractiveIndex,
		float smpRatio);

	/**
	 * @brief	CalBlindWidth							计算盲区宽度
	 * @param 	pulseWidth								脉宽
	 * @param 	ratio									采样分辨率
	 * @return											盲区宽度
	 **/
    int CalBlindWidth(
		int pulseWidth,
		float ratio);

	/**
	 * @brief	GetFiberRealLength						计算光纤实际长度,单位为m
	 * @param 	iSampleLength							采样长度
	 * @param 	fRefractiveIndex						折射率
	 * @param 	ratio									采样分辨率
	 * @return											光线实际长度
	 **/
    float GetFiberRealLength(
		int iSampleLength,
		float fRefractiveIndex,
		float iRatio);

    /*
     * @brief	 ReturnLoss								计算回波损耗值
     * @para[in] pData  								原始数据
     * @para[in] m 	  									起始点
     * @para[in] n 	  									终点
     * @para[in] fRLCoef 								背向散射系数
	 * @para[in] fSamRatio 								采样分辨率
     * @return											回波损耗
     */
    float ReturnLoss(
		unsigned short *pData,
		int m,
		int n,
		float fRLCoef,
		float fSamRatio);

	 /*
	 * @brief	 CalcRayleigh							计算某个脉宽的Reayleigh散射强度
	 * @para[in] enPulse  								脉宽
	 * @para[in] fRatio 	  							采样分辨率
	 * @para[in] fRLCoef 								背向散射系数
	 * @return											散射强度
	 */
    float CalcRayleigh(
		int enPulse,
		float fRatio,
		float fRLCoef);

	/*
	 * @brief	 FittingLine							拟合m 到 n直线，得到斜率和截距
	 * @para[in] pData  								原始数据
	 * @para[in] m 	  									起始点
	 * @para[in] n 										终点
	 * @return											拟合曲线
	 */
    SLINE FittingLine(
		unsigned short *pData,
		int m,
		int n);

	/*
	 * @brief	 LossLSA								使用LSA计算熔接损耗
	 * @para[in] iIndexA  								损耗计算的位置
	 * @para[in] iFactor 	  							比例因子（m,km,分辨率共同决定）
	 * @para[in] pLineA									损耗点前的直线
	 * @para[in] pLineB									损耗点后的直线
	 * @return											损耗
	 */
    float LossLSA(
		int iIndexA,
		int iFactor,
		SLINE *pLineA,
		SLINE *pLineB);

	 /*
	 * @brief	 CalcReflect							计算反射率
	 * @para[in] pData  								原始数据
	 * @para[in] m 	  									起始点
	 * @para[in] n 	  									终点
	 * @para[in] fRayleigh 								背向散射强度
	 * @para[in] iFactor 								比例因子
	 * @para[in] pLineA 								反射峰前拟合的直线
	 * @return											反射率
	 */
    float CalcReflect(
		unsigned short *pData,
		int m,
		int n,
		float fRayleigh,
		int iFactor,
		SLINE *pLineA);

    /*
     * @brief	 AlgorithmMain							OTDR算法接口函数
     * @para[in] pAlgorithmInfo							OTDR算法结构体
     * @return   
     */
    void AlgorithmMain(ALGORITHM_INFO *pAlgorithmInfo);

    /*
     * @brief	 SOLAMergeEventsTable					SOLA事件合并算法总函数
     * @para[in] pNewAlgInfo							当前事件分析信息
     * @para[in] pSolaAlgInfo							总的SOLA事件信息
     * @return  
     */
    void SOLAMergeEventsTable(
		ALGORITHM_INFO *pNewAlgInfo,
		SOLA_INFO *pSolaAlgInfo);

    /*
     * @brief    SpliterRationAutoJudge					分光比自动判断
     * @para[in] SolaAlgorithm							SOLA算法结构体
     * @para[in] waveLen 								当前波长
     * @return  
     */
    void SpliterRationAutoJudge(
		SOLA_INFO *SolaAlgorithm,
		int waveLen);


	 /*
	 * @brief    SpliterRationAutoJudge					设置宏弯曲事件
	 * @para[in] pAlgorithm_info1						波长1测试结果
	 * @para[in] pAlgorithm_info2 						波长2测试结果
	 * @return
	 */
    void OtdrAdapterMacrobending(
		ALGORITHM_INFO  *pAlgorithm_info1,
		ALGORITHM_INFO  *pAlgorithm_info2);

    // 设置光纤真实量程
    void SetFiberLen(const float fiberLen);

	// 补偿sola链路损耗
	void SetSolaEventLossOffset(float *floss, int wave);

	// sola损耗补偿清0
	void ClearSolaEventLossOffset();

	 /*
	 * @brief	 AddEventsTable							添加事件
	 * @para[in] pInputSignal  							输入的对数信号
	 * @para[in] iSignalLength 	  						信号长度
	 * @para[in] pEventsTable 	  						事件列表
	 * @para[in] iBlind 								盲区
	 * @para[in] iNewLocation 							添加位置
	 * @para[in] fSampleRating 							采样比率
	 * @return											添加的事件索引
	 */
    int AddEventsTable(
        float *pInputSignal,  	 	
        int iSignalLength,    		 
        EVENTS_TABLE *pEventsTable,  
        int iBlind, 				
        int iNewLocation, 			
        float fSampleRating);
	        
	/**
	 * @brief	 CalcAddEventsLoss						计算所增加事件的参数
	 * @para[in] pInputSignal  							输入的对数信号
	 * @para[in] iSignalLength 	  						信号长度
	 * @para[in] pEventsTable 	  						事件列表
	 * @para[in] iBlind 								盲区
	 * @para[in] iNewIndex 								添加的事件索引
	 * @para[in] fSampleRating 							采样比率
	 * @return											
	 */
	void CalcAddEventsLoss(
		float *pInputSignal,
		int iSignalLength, 
		EVENTS_TABLE *pEventsTable,
		int iBlind, 
		int iNewIndex,
		float fSampleRating);

	/**
	 * @brief	 DelEventsTable							删除事件
	 * @para[in] pInputSignal  							输入的对数信号
	 * @para[in] iSignalLength 	  						信号长度
	 * @para[in] pEventsTable 	  						事件列表
	 * @para[in] iDelIndex 								待删除的事件索引
	 * @para[in] fSampleRating 							采样比率
	 * @return
	 */
    int DelEventsTable(
        float *pInputSignal,
        int iSignalLength,
        EVENTS_TABLE *pEventsTable,
        int iDelIndex, 
        float fSampleRating);

#ifdef __cplusplus
}
#endif

#endif


