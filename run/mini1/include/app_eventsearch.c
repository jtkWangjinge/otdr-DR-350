/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司
*
* All rights reserved
*
* 文件名称：  app_eventssearch.c
* 摘    要：  事件查找算法函数定义
*
* 当前版本：  v1.0.0 
* 作    者：  sjt
* 完成日期：  2020-8-21
*
*******************************************************************************/

#include "app_eventsearch.h"
// #include "app_algorithm_support.h"

// 外部参数赋值
EXTERN_INTERFACE frontParament =  { .wave = 0,
                                    .range = 0,
                                    .pulseWidth = 0,
                                    .iSampleRating = 40000000,
                                    .fLightSpeed = 299792458.0f,
                                    .fRelay = 0,
                                    .fLossThreshold = 0.02f,
                                    .fBackScattering = -80.0f,
                                    .fRefractiveIndex = 1.4600f,
                                    .fReflectThreshold = -70.0f,
                                    .fFiberEndThreshold = 5.0f,
                                    .fExcessLength = 0 };

static float fFiberDist = 0.0f;
static int iFlag = 0;
static float EndThrNoise = 0.0f;
static float ThreshOffset[4] = { 0.0f, 0.0f, 0.0f, 0.0f };   // 阈值偏移
static float *g_pSignalLog = NULL;							 // 存储对数信号;
static float *g_pWaveletCoef[4] = {NULL};					 // 存储小波信号;
static int g_pWaveletWindow[4] = {4,16,64,256};				 // 定义小波窗口大小;
static int g_iExSignalLength = 0;							 // 拓展零后数据长度;
static float g_fLengthPerPoint = 0.0f;						 // 每点数代表的光纤长度;
static EVENTS_REGION_TABLE *g_pEventsRegionTable = NULL;     // 事件区间信息列表

// 各个真实的脉冲宽度(0, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000)
static float fPulseWidth[OPM_PULSE_KIND] = {6.25f, 9.375f, 14.8438f, 24.6094f, 62.1094f,103.125f,
											200.0f, 500.0f, 1000.0f, 2000.0f, 10000.0f, 20000.0f};

static float range = 0.0f;                                   // 测量距离 
static float Max_fAttenuation = 0.5f;						 // 衰减计算不准的时候，最大衰减
static float Normal_fAttenuation = 0.33f;					 // 衰减计算不准的时候，最大衰减
static int iEndpointindex[500][2] = { { 0 } };				 // 记录小波256窗口下各反射事件的始末端位置，用来删除其下降沿衰减事件误报
static float *AdptThreshold[4] = { NULL };					 // 自适应阈值;

FILE *test_file_name = NULL;

// 每个脉宽下对应的盲区大小                                                                      
const static float blindWidth[OPM_PULSE_KIND][3] = {{23.0f, 5.0f, 5.0f},  // auto  
												   {13.0f, 2.0f, 2.0f},	  // 5ns   
												   {13.0f, 4.5f, 4.5f},	  // 10ns  
												   {45.0f, 7.0f, 7.0f},   // 20ns  
												   {45.0f, 8.0f, 8.0f},	  // 50ns	
												   {45.0f, 13.0f, 13.0f}, // 100ns 
												   {65.0f, 22.0f, 22.0f}, // 200ns 
												   {170.0f, 50.0f, 50.0f},// 500ns 
												   {230.0f, 160.0f, 160.0f},	// 1us   
												   {600.0f, 210.0f, 210.0f},	// 2us		
											 	   {1100.0f, 1000.0f, 1000.0f}, // 10us
											       {2100.0f, 2000.0f, 2000.0f}};// 20us

// sola 事件损耗、链路总损耗补偿
static float solaFloss[SOLA_WAVE_NUM][MAX_EVENTS_NUMBER];

//打印信息
#define DEBUGTEST                                   0
#define ENBALE_ALG_DEBUG                            0
#define ISSERIAL                                    0
//#define SAVEDATA                                    0

#if (DEBUGTEST == 1)

//#define DEBUG_EVENTINFO
#define SOLADEBUG
//#define SAVEDATA

#define ALG_DEBUG_COLOR_CARMINE             "\033[35m"      //洋红
#define ALG_DEBUG_COLOR_CLEAR               "\033[0m"       //清除颜色

#define DEBUG_LOG(color, format, ...) \
	do {\
        fprintf(stderr, color "[%s|%s,%d] " format ALG_DEBUG_COLOR_CLEAR, \
				__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#else
#define DEBUG_LOG(color, format, ...)
#endif

#if (ENBALE_ALG_DEBUG == 1)
#define  ALG_DEBUG(format, ...)           DEBUG_LOG(ALG_DEBUG_COLOR_CARMINE, format, ##__VA_ARGS__)
#else
#define  ALG_DEBUG(format, ...)
#endif

// 算法主函数
void AlgorithmMain(ALGORITHM_INFO *pAlgorithmInfo) //算法信息;
{
    //保存参数
#ifdef SAVEDATA
    SaveEventAnalyseParam(pAlgorithmInfo);
#endif

	ALG_DEBUG("--------AlgorithmMain 1------\n");

    //外部参数输入(目前主要用到三个阈值);
    frontParament.range = pAlgorithmInfo->iFiberRange;
    frontParament.pulseWidth = pAlgorithmInfo->PulseWidth;
    frontParament.wave = pAlgorithmInfo->SetParameters.iwavelength;
	frontParament.iSampleRating = ADC_CLOCK_FREQ * 1000000;
    frontParament.fExcessLength = pAlgorithmInfo->SetParameters.fExcessLength;
    frontParament.fLossThreshold = pAlgorithmInfo->SetParameters.fLossThreshold;
    frontParament.fBackScattering = pAlgorithmInfo->SetParameters.fBackScattering;
    frontParament.fRefractiveIndex = pAlgorithmInfo->SetParameters.fRefractiveIndex;
    frontParament.fReflectThreshold = pAlgorithmInfo->SetParameters.fReflectThreshold;
    frontParament.fFiberEndThreshold = pAlgorithmInfo->SetParameters.fFiberEndThreshold;
    frontParament.fRelay = frontParament.fBackScattering
		+ 10.0f * log10f(fPulseWidth[pAlgorithmInfo->PulseWidth]);

    g_fLengthPerPoint = (1 - frontParament.fExcessLength)
		* frontParament.fLightSpeed / (2.0f * frontParament.iSampleRating
        * pAlgorithmInfo->SetParameters.fSampleRating * frontParament.fRefractiveIndex);

	ALG_DEBUG("--------AlgorithmMain 2------\n");
    // 每个点代表的距离
    pAlgorithmInfo->fPointLength = g_fLengthPerPoint;

    switch (frontParament.wave)
	{
		case 0:
			Normal_fAttenuation = 0.329f;
			Max_fAttenuation = 0.7f;
			break;
		case 1:
			Normal_fAttenuation = 0.179f;
			Max_fAttenuation = 0.5f;
			break;
		case 2:
			Normal_fAttenuation = 0.208f;
			Max_fAttenuation = 0.9f;
			break;
		default:
			Normal_fAttenuation = 0.219f;
			Max_fAttenuation = 0.9f;
			break;
	}

	ALG_DEBUG("--------AlgorithmMain 3------\n");
    //添加噪声参数
	int calNoiseLen = BLACK_LEVEL_LEN;
	if (pAlgorithmInfo->iFiberRange > 180000)
	{
		calNoiseLen = BLACK_LEVEL_LEN * 2;
	}
    pAlgorithmInfo->fInputNoise = GetStdFloat(pAlgorithmInfo->pSignalLog
        + pAlgorithmInfo->iSignalLength - calNoiseLen, calNoiseLen);
	pAlgorithmInfo->fMaxInputNoise = GetMaxAbsFloat(pAlgorithmInfo->pSignalLog
        + pAlgorithmInfo->iSignalLength - calNoiseLen, calNoiseLen);
	float fMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
		+ pAlgorithmInfo->iSignalLength - calNoiseLen, calNoiseLen);
	pAlgorithmInfo->fInputNoise = Max(pAlgorithmInfo->fInputNoise, fMean + 1.0f);

	EndThrNoise = Max(pAlgorithmInfo->fMaxInputNoise - 2.0f, pAlgorithmInfo->fInputNoise);

	ALG_DEBUG("--------AlgorithmMain 4------\n");

    //重置事件表;
    memset(&(pAlgorithmInfo->EventsTable), 0, sizeof(EVENTS_TABLE));

    //信号补零;
    if (pAlgorithmInfo->iSignalLength % 256 == 0)
    {
        g_iExSignalLength = pAlgorithmInfo->iSignalLength;
    }
    else
    {
        g_iExSignalLength = 256 * (pAlgorithmInfo->iSignalLength / 256 + 1);
    }

    g_pSignalLog = (float *)calloc(g_iExSignalLength, sizeof(float));
    memcpy(g_pSignalLog, pAlgorithmInfo->pSignalLog, sizeof(float) * pAlgorithmInfo->iSignalLength);

	ALG_DEBUG("--------AlgorithmMain 5------\n");
    // 小波变换
    g_pWaveletCoef[0] = (float *)calloc(g_iExSignalLength, sizeof(float));
    g_pWaveletCoef[1] = (float *)calloc(g_iExSignalLength, sizeof(float));
    g_pWaveletCoef[2] = (float *)calloc(g_iExSignalLength, sizeof(float));
    g_pWaveletCoef[3] = (float *)calloc(g_iExSignalLength, sizeof(float));
    GetWaveletCoef(pAlgorithmInfo);

	ALG_DEBUG("----Pulse = %fns-------\n", fPulseWidth[pAlgorithmInfo->PulseWidth]);
	ALG_DEBUG("----wave = %d-------\n", frontParament.wave);
	ALG_DEBUG("---------iBlind = %d-------------\n", pAlgorithmInfo->iBlind);
	ALG_DEBUG("----------g_fLengthPerPoint = %f---------\n", g_fLengthPerPoint);
	ALG_DEBUG("---------SignalLength = %d-------------\n", pAlgorithmInfo->iSignalLength);
	ALG_DEBUG("---------DisplayLength = %d-------------\n", pAlgorithmInfo->iDisplayLenth);
	ALG_DEBUG("pAlgorithmInfo->fInputNoise = %f\n", pAlgorithmInfo->fInputNoise);
	ALG_DEBUG("pAlgorithmInfo->fMaxInputNoise = %f\n", pAlgorithmInfo->fMaxInputNoise);
	ALG_DEBUG("----fSampleRating = %f\n", pAlgorithmInfo->SetParameters.fSampleRating);
	ALG_DEBUG("----g_fFiberEndThreshold = %f\n", frontParament.fFiberEndThreshold);
	ALG_DEBUG("----------g_fRelay = %f---------\n", frontParament.fRelay);
	ALG_DEBUG("---------g_iExSignalLength = %d-------------\n", g_iExSignalLength);
	ALG_DEBUG("---------fLossThreshold = %f-------------\n\n\n", frontParament.fLossThreshold);

	#if ISSERIAL == 1
		char tmp_file_name[512];
	    sprintf(tmp_file_name, "/mnt/usb/test_log_pulse_%f.txt", fPulseWidth[pAlgorithmInfo->PulseWidth]);
		test_file_name = fopen(tmp_file_name, "a+");
	#endif

#ifdef SAVEDATA
    PrintAlgorithmInfo(pAlgorithmInfo, pAlgorithmInfo->PulseWidth);
    int i = 0;
    for (i = 0; i < 4; ++i)
        PrintWaveLetCoef(g_pWaveletCoef[i], g_iExSignalLength, i, pAlgorithmInfo->PulseWidth);
#endif

	ALG_DEBUG("--------AlgorithmMain 6------\n");


    // 计算sola有效测试距离
    pAlgorithmInfo->solaEffectiveRange = GetEffectiveFiberRange(pAlgorithmInfo);
	ALG_DEBUG("SolaEffectiveRange = %f\n", pAlgorithmInfo->solaEffectiveRange);
	
    // 确定事件查找范围
    GetEventsSearchRange(pAlgorithmInfo);
	ALG_DEBUG("----GetEventsSearchRange over!-----\n");
	
    pAlgorithmInfo->solaEffectiveRange = range;
	ALG_DEBUG("-----range = %f-------\n", range);

	#if ISSERIAL == 1
		fprintf(test_file_name, "--range = %f---\n", range);
	#endif

	AdptThreshold[0] = (float *)calloc(g_iExSignalLength, sizeof(float));
	AdptThreshold[1] = (float *)calloc(g_iExSignalLength, sizeof(float));
	AdptThreshold[2] = (float *)calloc(g_iExSignalLength, sizeof(float));
	AdptThreshold[3] = (float *)calloc(g_iExSignalLength, sizeof(float));
    // 获取事件列表
    g_pEventsRegionTable = (EVENTS_REGION_TABLE *)calloc(1, sizeof(EVENTS_REGION_TABLE));
    GetFrontEventsTable(pAlgorithmInfo);
	ALG_DEBUG("-----GetFrontEventsTable over--------\n");



	free(AdptThreshold[0]);
    AdptThreshold[0] = NULL;
    free(AdptThreshold[1]);
    AdptThreshold[1] = NULL;
    free(AdptThreshold[2]);
    AdptThreshold[2] = NULL;;
    free(AdptThreshold[3]);
    AdptThreshold[3] = NULL;

	
    free(g_pWaveletCoef[0]);
    g_pWaveletCoef[0] = NULL;
    free(g_pWaveletCoef[1]);
    g_pWaveletCoef[1] = NULL;
    free(g_pWaveletCoef[2]);
    g_pWaveletCoef[2] = NULL;;
    free(g_pWaveletCoef[3]);
    g_pWaveletCoef[3] = NULL;

    // 获得详细事件列表、计算事件信息;
    GetEventsTable(pAlgorithmInfo);
	ALG_DEBUG("-----GetEventsTable over--------\n");
	
    // 回波判断;
    CheckEcho(pAlgorithmInfo);
	ALG_DEBUG("-----CheckEcho over--------\n");

    // 末端定位;
    GetFiberEnd(pAlgorithmInfo);
	ALG_DEBUG("-----GetFiberEnd over--------\n");

	// 设置每个事件的测试波长 
	SetEventsWave(pAlgorithmInfo);
	ALG_DEBUG("-----SetEventsWave over--------\n");

    //计算累计损耗
    CalTotalFloss(&pAlgorithmInfo->EventsTable);
	ALG_DEBUG("-----CalTotalFloss over--------\n");

    // 获得链路总的回波损耗 
    GetTotalReturnLoss(pAlgorithmInfo);
	ALG_DEBUG("-----GetTotalReturnLoss over--------\n");

    free(g_pEventsRegionTable);
    g_pEventsRegionTable = NULL;

    free(g_pSignalLog);
    g_pSignalLog = NULL;

	#if ISSERIAL == 1
		fclose(test_file_name);
	#endif
}

// 在事件列表中指定位置点加入一个事件
int AddEventsTable(   		   //返回值判定;
    float *pInputSignal,       //输入的对数信号;
    int iSignalLength,         //输入的信号长度;
    EVENTS_TABLE *pEventsTable,//原数据表;
    int iBlind, 			   //盲区宽度;
    int iNewLocation,		   //新加入事件的位置
    float fSampleRating)
{
    int iErr = 0;		//!< 错误类型判定
    int i = 0, j = 0;
    int iNewIndex = -1;	//!< 新加入事件的索引值,-1表示需要在最后一个事件背后增加新事件
    EVENTS_REGION_TABLE *pEventsRegionTable;
    pEventsRegionTable = (EVENTS_REGION_TABLE *)calloc(1, sizeof(EVENTS_REGION_TABLE));

    if ((iNewLocation < 0) || (iNewLocation >= iSignalLength))
    {
        iErr = -1; //新加入的事件超出制定范围
        free(pEventsRegionTable);
        return iErr;
    }
    if (pEventsTable->iEventsNumber >= MAX_EVENTS_NUMBER)
    {
        iErr = -2;//事件表已满
        free(pEventsRegionTable);
        return iErr;
    }

    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
		if ((pEventsTable->EventsInfo[i].iBegin - iBlind <= iNewLocation)
			&& (pEventsTable->EventsInfo[i].iEnd >= iNewLocation))
		{
			iErr = -3;
			free(pEventsRegionTable);
			pEventsRegionTable = NULL;
			return iErr;//当前区域有事件
		}
	}

    // 将新事件加入事件表中
    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
        if (pEventsTable->EventsInfo[i].iBegin > iNewLocation)
        {
            iNewIndex = i;
            //将新事件后边的事件向后挪
            for (j = pEventsTable->iEventsNumber; j > i; --j)
            {
                pEventsTable->EventsInfo[j] = pEventsTable->EventsInfo[j - 1];
            }
            pEventsTable->EventsInfo[i].iBegin = iNewLocation; 		  //事件位置定为新加入事件的位置
			pEventsTable->EventsInfo[i].iEnd
				= Min(iNewLocation + (int)(1.5 * iBlind),
					pEventsTable->EventsInfo[i+1].iBegin - 1); //事件重点为起点加盲区
            pEventsTable->EventsInfo[i].iStyle = 0; 				  //事件类型初定为损耗事件
            pEventsTable->iEventsNumber++;
            break;
        }
    }

    if (iNewIndex == -1)
    {
        if ((pEventsTable->iEventsNumber > 1)
			&& (pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle >= 10))
        {
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle -= 10;
        }
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iBegin = iNewLocation;        //事件位置定为新加入事件的位置
		pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iEnd
			= Min(iNewLocation + (int)(1.5f * iBlind), iSignalLength - 1); //事件重点为起点加盲区
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iStyle = 0;                   //事件类型初定为损耗事件
        iNewIndex = pEventsTable->iEventsNumber;
        pEventsTable->iEventsNumber++;
    }

	for (i = 0; i < pEventsTable->iEventsNumber - 1; ++i)
	{
		pEventsTable->EventsInfo[i].iEnd
				= Min(pEventsTable->EventsInfo[i].iBegin + (int)(1.5f * iBlind),
					pEventsTable->EventsInfo[i + 1].iBegin - 1); //事件重点为起点加盲区
	}
    TransEventsTable(pEventsTable, pEventsRegionTable);

    //排除由于新事件导入增加的异常点
    for (i = 0; i < pEventsRegionTable->iEventsNumber; ++i)
    {
        if ((i > 0) && (pEventsRegionTable->EventsRegionInfo[i].iBegin
			< pEventsRegionTable->EventsRegionInfo[i - 1].iEnd))
        {
            if (pEventsRegionTable->EventsRegionInfo[i - 1].iBegin
				< pEventsRegionTable->EventsRegionInfo[i].iBegin)
                pEventsRegionTable->EventsRegionInfo[i - 1].iEnd
				= pEventsRegionTable->EventsRegionInfo[i].iBegin - 1;
            else
                pEventsRegionTable->EventsRegionInfo[i - 1].iEnd
				= pEventsRegionTable->EventsRegionInfo[i - 1].iBegin;
        }
    }
    //重新计算事件参数
    pEventsRegionTable->iBlind = iBlind;
    GetEventsParameter(pInputSignal, iSignalLength,
		pEventsRegionTable, pEventsTable, fSampleRating);
    //CalcAddEventsLoss(pInputSignal, iSignalLength, pEventsTable, iBlind, iNewIndex, fSampleRating);

    //新加入事件类型判断
    if (pEventsTable->EventsInfo[iNewIndex].fReflect > frontParament.fReflectThreshold)
    {
        pEventsTable->EventsInfo[iNewIndex].iStyle = 2;//反射事件
    }
    else
    {
		if (pEventsTable->EventsInfo[iNewIndex].fLoss < 0)//增益事件
        {
            pEventsTable->EventsInfo[iNewIndex].iStyle = 1;
        }
        else
            pEventsTable->EventsInfo[iNewIndex].iStyle = 0;
    }

	//重新计算事件参数
	TransEventsTable(pEventsTable, pEventsRegionTable);
    GetEventsParameter(pInputSignal, iSignalLength,
		pEventsRegionTable, pEventsTable, fSampleRating);

    i = 0;
    int iTmp = 0;
    iTmp = Max(iBlind, 5);
    int iBegin = pEventsTable->EventsInfo[0].iEnd;
    int iTmpIndex = GetMinIndex(pInputSignal + iBegin, Min(iTmp, 20));
    iBegin = iBegin + iTmpIndex;
    pEventsTable->EventsInfo[0].fLoss = pInputSignal[0] - pInputSignal[iBegin];
    pEventsTable->EventsInfo[0].fLoss = Max(pEventsTable->EventsInfo[0].fLoss, 0.0f);
	CalTotalFloss(pEventsTable);

    if (iNewIndex == pEventsTable->iEventsNumber - 1)
    {
        pEventsTable->EventsInfo[iNewIndex].iStyle += 10;
    }

    //处理注入光纤和接收光纤
    if (pEventsTable->iLaunchFiberIndex > 0)
    {
        if (iNewIndex <= pEventsTable->iLaunchFiberIndex)
            pEventsTable->iLaunchFiberIndex++;
    }

    if (pEventsTable->iRecvFiberIndex > 0)
    {
        if (iNewIndex <= pEventsTable->iRecvFiberIndex)
            pEventsTable->iRecvFiberIndex++;
    }

    if (pEventsTable->iEventsStart > 0)
    {
        if (iNewIndex <= pEventsTable->iEventsStart)
           pEventsTable->iEventsStart++; 
    } 
	
	free(pEventsRegionTable);
	return iNewIndex;
}

// 计算添加事件的损耗
void CalcAddEventsLoss(   		   //返回值判定;
	float *pInputSignal,       //输入的对数信号;
	int iSignalLength,         //输入的信号长度;
	EVENTS_TABLE *pEventsTable,//原数据表;
	int iBlind, 			   //盲区宽度;
	int iNewIndex,		   //新加入事件的位置
	float fSampleRating)
{
	int i = 0;
	float pFitCoef[2] = {0.0f};
	float fFrontMean = 0.0f;
	float fEndMean = 0.0f;
	float fMaxNoise = GetMaxFloat(
		pInputSignal + iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
	float fPerPointLen = g_fLengthPerPoint;
	float fMaxAttn = Max_fAttenuation;
	
	if (iNewIndex < 0)
	{
		return;
	}
	int iStartEventIndex = 0;
	if (iNewIndex == 0)
	{
		iStartEventIndex = 1;
	}

	pEventsTable->EventsInfo[iNewIndex].fDelta
		= GetMaxFloat(pInputSignal + pEventsTable->EventsInfo[iNewIndex].iBegin, iBlind)
		- pInputSignal[pEventsTable->EventsInfo[iNewIndex].iBegin];
	if (pEventsTable->EventsInfo[iNewIndex].fDelta < 0.01f)
	{
		pEventsTable->EventsInfo[iNewIndex].fReflect = -80.0f;
	}
	else
	{
	
		pEventsTable->EventsInfo[iNewIndex].fReflect = frontParament.fRelay
			+ 10 * log10f(powf(10, pEventsTable->EventsInfo[iNewIndex].fDelta / 5.0f) - 1);	
	}
	
	if (iNewIndex == 0)
	{
		pEventsTable->EventsInfo[iNewIndex].iContinueLength
			= pEventsTable->EventsInfo[iNewIndex + 1].iBegin
			- pEventsTable->EventsInfo[iNewIndex].iBegin;
		pEventsTable->EventsInfo[iNewIndex].fEventsPosition 
			= pEventsTable->EventsInfo[iNewIndex].iBegin * fPerPointLen;
		pEventsTable->EventsInfo[iNewIndex].fEventsEndPosition
			= pEventsTable->EventsInfo[iNewIndex].iEnd * fPerPointLen;
	}
	else
	{
		pEventsTable->EventsInfo[iNewIndex - 1].iContinueLength
			= pEventsTable->EventsInfo[iNewIndex].iBegin
			- pEventsTable->EventsInfo[iNewIndex - 1].iBegin;
		pEventsTable->EventsInfo[iNewIndex].fEventsPosition
			= pEventsTable->EventsInfo[iNewIndex].iBegin * fPerPointLen;
		pEventsTable->EventsInfo[iNewIndex].fEventsEndPosition
			= pEventsTable->EventsInfo[iNewIndex].iEnd * fPerPointLen;
	}
	
	if (iNewIndex == pEventsTable->iEventsNumber - 1)
	{
		if (pEventsTable->EventsInfo[iNewIndex - 1].iBegin <= 10)
		{
			fFrontMean = pInputSignal[pEventsTable->EventsInfo[iNewIndex - 1].iBegin];
		}
		else
		{
			fFrontMean = GetMeanFloat(pInputSignal
				+ pEventsTable->EventsInfo[iNewIndex - 1].iBegin - 10, 10);
		}

		int iTmpLength = pEventsTable->EventsInfo[iNewIndex].iBegin
			- pEventsTable->EventsInfo[iNewIndex - 1].iEnd;
		LineFit(pInputSignal, iTmpLength,
			pEventsTable->EventsInfo[iNewIndex - 1].iEnd, pFitCoef);
		
		float fAttn = - pFitCoef[0] * 1000.0f / fPerPointLen;
		if ((fAttn > fMaxAttn) || (fAttn <= 0.01f))
		{
			fAttn = fMaxAttn;
		}
		pEventsTable->EventsInfo[iNewIndex - 1].fAttenuation = fAttn;
		pEventsTable->EventsInfo[iNewIndex - 1].fContinueAttenuation = fAttn * fPerPointLen
			/ 1000.0f * pEventsTable->EventsInfo[iNewIndex - 1].iContinueLength;

		if (iNewIndex == 1)
		{
			pEventsTable->EventsInfo[iNewIndex - 1].fLoss = 0.0f;
		}
		else
		{
			pEventsTable->EventsInfo[iNewIndex - 1].fLoss
				= fFrontMean - pInputSignal[pEventsTable->EventsInfo[iNewIndex].iBegin]
				- pEventsTable->EventsInfo[iNewIndex - 1].fContinueAttenuation;
			pEventsTable->EventsInfo[iNewIndex - 1].fLoss 
				= Max(0.01f, pEventsTable->EventsInfo[iNewIndex - 1].fLoss);
			pEventsTable->EventsInfo[iNewIndex - 1].fLoss
				= Min(fFrontMean - fMaxNoise, pEventsTable->EventsInfo[iNewIndex - 1].fLoss);
		}
		
		pEventsTable->EventsInfo[iNewIndex].fTotalLoss
			= pEventsTable->EventsInfo[iNewIndex - 1].fTotalLoss
			+ pEventsTable->EventsInfo[iNewIndex - 1].fLoss
			+ pEventsTable->EventsInfo[iNewIndex - 1].fContinueAttenuation;
	}
	else
	{
		pEventsTable->EventsInfo[iNewIndex - 1].fContinueAttenuation
			= pEventsTable->EventsInfo[iNewIndex - 1].fAttenuation
			* fPerPointLen / 1000.0f * pEventsTable->EventsInfo[iNewIndex - 1].iContinueLength;
		
		pEventsTable->EventsInfo[iNewIndex].iContinueLength
			= pEventsTable->EventsInfo[iNewIndex + 1].iBegin
			- pEventsTable->EventsInfo[iNewIndex].iBegin;		
		fFrontMean = GetMeanFloat(pInputSignal
			+ pEventsTable->EventsInfo[iNewIndex].iBegin - 10, 10);
		fEndMean = GetMeanFloat(pInputSignal
			+ Max(pEventsTable->EventsInfo[iNewIndex].iEnd,
				pEventsTable->EventsInfo[iNewIndex + 1].iBegin - 10), 10);
		pEventsTable->EventsInfo[iNewIndex].fLoss = fFrontMean - fEndMean 
			- pEventsTable->EventsInfo[iNewIndex-1].fAttenuation
			* Max(iBlind, pEventsTable->EventsInfo[iNewIndex + 1].iBegin
			- pEventsTable->EventsInfo[iNewIndex].iBegin) * fPerPointLen / 1000.0f;
		
		int iTmpLength = pEventsTable->EventsInfo[iNewIndex + 1].iBegin
			- pEventsTable->EventsInfo[iNewIndex].iEnd;
		LineFit(pInputSignal, iTmpLength,
			pEventsTable->EventsInfo[iNewIndex].iEnd, pFitCoef);	
		float fAttn = - pFitCoef[0] * 1000.0f / fPerPointLen;
		if ((fAttn > fMaxAttn) || (fAttn <= 0.01f))
		{
			fAttn = fMaxAttn;
		}
		pEventsTable->EventsInfo[iNewIndex].fAttenuation = fAttn;
		pEventsTable->EventsInfo[iNewIndex].fContinueAttenuation = fAttn * fPerPointLen
			/ 1000.0f * pEventsTable->EventsInfo[iNewIndex].iContinueLength;
		for (i = iNewIndex; i < pEventsTable->iEventsNumber; ++i)
		{
			pEventsTable->EventsInfo[i].fTotalLoss
				= pEventsTable->EventsInfo[i - 1].fLoss
				+ pEventsTable->EventsInfo[i - 1].fContinueAttenuation
				+ pEventsTable->EventsInfo[i - 1].fTotalLoss;
		}
	}
}

// 在事件列表中删除一个事件
int DelEventsTable(   ///返回值判定
    float *pInputSignal,   //输入的对数信号
    int iSignalLength,     //输入的信号长度 
    EVENTS_TABLE *pEventsTable,//原数据表
    int iDelIndex,//待删除的事件索引
    float fSampleRating)
{
    int iErr = 0;
    int i = 0;
    EVENTS_REGION_TABLE *pEventsRegionTable
		= (EVENTS_REGION_TABLE *)malloc(sizeof(EVENTS_REGION_TABLE));
    memset(pEventsRegionTable, 0, sizeof(EVENTS_REGION_TABLE));
    if ((iDelIndex < 0) || (iDelIndex >= pEventsTable->iEventsNumber))
    {
        iErr = -1;//删除的事件索引超出范围
        free(pEventsRegionTable);
        return iErr;
    }
    if (pEventsTable->iEventsNumber <= 0)
    {
        iErr = -2;//没有足够的事件进行删除
        free(pEventsRegionTable);
        return iErr;
    }

    //第一个事件和最后一个事件不能删除,参考起始点不能删除
    if ((iDelIndex == 0) || (iDelIndex == pEventsTable->iEventsNumber - 1)
		|| (iDelIndex == pEventsTable->iEventsStart))
    {
        iErr = -3;//不能删除起点和终点
        free(pEventsRegionTable);
        return iErr;
    }
	if (iDelIndex >= 1)
	{
		//pEventsTable->EventsInfo[iDelIndex - 1].iContinueLength = pEventsTable->EventsInfo[iDelIndex - 1].iContinueLength + pEventsTable->EventsInfo[iDelIndex].iContinueLength;
		//pEventsTable->EventsInfo[iDelIndex - 1].fTotalLoss = pEventsTable->EventsInfo[iDelIndex - 1].fContinueAttenuation + pEventsTable->EventsInfo[iDelIndex].fContinueAttenuation + pEventsTable->EventsInfo[iDelIndex].fLoss;
		pEventsTable->EventsInfo[iDelIndex - 1].fContinueAttenuation
			= pEventsTable->EventsInfo[iDelIndex - 1].fContinueAttenuation
			+ pEventsTable->EventsInfo[iDelIndex].fContinueAttenuation
			+ pEventsTable->EventsInfo[iDelIndex].fLoss;
	}
    for (i = iDelIndex; i < pEventsTable->iEventsNumber; ++i)
    {
        pEventsTable->EventsInfo[i] = pEventsTable->EventsInfo[i + 1];
    }
    pEventsTable->iEventsNumber--;
    if (pEventsTable->iEventsNumber >= 1)
    {
        TransEventsTable(pEventsTable, pEventsRegionTable);
        //重新计算事件参数
        //GetEventsParameter(pInputSignal, iSignalLength, pEventsRegionTable, pEventsTable, fSampleRating);
    }

    //处理注入光纤和接收光纤
    if (pEventsTable->iLaunchFiberIndex > 0)
    {
        if (iDelIndex < pEventsTable->iLaunchFiberIndex)
            pEventsTable->iLaunchFiberIndex--;
        else if (pEventsTable->iLaunchFiberIndex == iDelIndex)
            pEventsTable->iLaunchFiberIndex = 0;
    }

    if (pEventsTable->iRecvFiberIndex > 0)
    {
        if (iDelIndex < pEventsTable->iRecvFiberIndex)
            pEventsTable->iRecvFiberIndex--;
        else if (pEventsTable->iRecvFiberIndex == iDelIndex)
            pEventsTable->iRecvFiberIndex = 0;
    }

    if (pEventsTable->iEventsStart > 0)
    {
        if (iDelIndex < pEventsTable->iEventsStart)
            pEventsTable->iEventsStart--;
    }

    free(pEventsRegionTable);

    return iErr;
}

// 获取小波信号
void GetWaveletCoef(ALGORITHM_INFO *pAlgorithmInfo)
{
    int i = 0;
    float *pTmpLowSignal = (float *)malloc(sizeof(float) * g_iExSignalLength);
    float *pTmpHighSignal = (float *)malloc(sizeof(float) * g_iExSignalLength);
    float *pTmpSignal = (float *)malloc(sizeof(float) * g_iExSignalLength);
    memcpy(pTmpSignal, g_pSignalLog, sizeof(float) * g_iExSignalLength);
    for (i = 0; i < 4; ++i)
    {
        Filt_Db1(pTmpSignal, pTmpLowSignal, pTmpHighSignal, g_iExSignalLength, 2 * i + 1);
        memcpy(pTmpSignal, pTmpLowSignal, sizeof(float) * g_iExSignalLength);
        Filt_Db1(pTmpSignal, pTmpLowSignal, pTmpHighSignal, g_iExSignalLength, 2 * i + 2);
        memcpy(pTmpSignal, pTmpLowSignal, sizeof(float) * g_iExSignalLength);
        memset(g_pWaveletCoef[i], 0, sizeof(float) * g_iExSignalLength);
        memcpy(g_pWaveletCoef[i], pTmpHighSignal, sizeof(float) * (g_iExSignalLength));
    }
    free(pTmpHighSignal);
    free(pTmpLowSignal);
    free(pTmpSignal);
}

// 计算阈值偏移
void CalThreshOffset(int blind, int level)
{
    int i = 0, j = 0;
    int time = 10;
    int count = 0;
    int startPos = Max(2 * blind, g_pWaveletWindow[level]);
    int breakFlag = 0;
    int smpLen = Min(blind, 50);
    int len = Min(Max(2 * blind, g_pWaveletWindow[level]), g_pWaveletWindow[3] / 2);
    while (!breakFlag)
    {
        startPos = startPos + 2 * j * smpLen;
        float MeanCoef = GetMeanFloat(g_pWaveletCoef[level] + startPos,
            Min(len, Max(g_iExSignalLength - startPos, 1)));
        if (startPos + 2 * j * smpLen > g_iExSignalLength - g_pWaveletWindow[3] - len)
        {
            break;
        }
        for (i = 1; i<time; ++i)
        {
            float temp = GetMeanFloat(g_pWaveletCoef[level] + startPos + i * smpLen,
                Min(len, Max(g_iExSignalLength - startPos - i * smpLen, 1)));
            if (startPos + i * smpLen > g_iExSignalLength - g_pWaveletWindow[3] - len)
            {
                breakFlag = 1;
                break;
            }
            if (fabsf(temp - MeanCoef) < 0.1f)
            {
                count++;
            }
            else
            {
                count = 0;
                j++;
                break;
            }
            if (count == 5)
            {
                ThreshOffset[level] = MeanCoef;
                breakFlag = 1;
                break;
            }
        }
    }
}

// 获取初始事件位置信息
void GetFrontEventsTable(ALGORITHM_INFO *pAlgorithmInfo)
{
    int i = 0, j = 0, k = 0;
    int iSaveIndex = 0;
    float fTempThr[4] = { 0 };
    int iBlind = pAlgorithmInfo->iBlind;
    float fSampleRating = pAlgorithmInfo->SetParameters.fSampleRating;   //!< 采样分辨比率
    EVENTS_REGION_TABLE *pTmpEventsRegionTable
		= (EVENTS_REGION_TABLE *)malloc(sizeof(EVENTS_REGION_TABLE) * 4);
    memset(pTmpEventsRegionTable, 0, sizeof(EVENTS_REGION_TABLE) * 4);

    //设置各级小波初始阈值
    if ((range < 7500.0f))
    {
        fTempThr[0] = 1000.0f;
        fTempThr[1] = 1.5f;
        fTempThr[2] = 1.5f;
        fTempThr[3] = 1.5f;
        iFlag = 1;              //!< 短量程标志
    }
    else
    {
        fTempThr[0] = 1.00f;
        fTempThr[1] = 1.50f;
        fTempThr[2] = 4.00f;
        fTempThr[3] = 6.0f;
        iFlag = 0;
    }

    int level = 0;              //!< 小波分析尺度
    int IntegrationTimes = 3;   //!< 组合次数

    // 对每一层小波寻找事件区间
    for (i = 0; i <= IntegrationTimes; ++i)
    {
		ALG_DEBUG("------------------CalThreshOffset--------i = %d\n\n", i);
        // 计算各尺度小波阈值补偿
        CalThreshOffset(iBlind, i);
		ALG_DEBUG("------------------SearchRegion--------i = %d\n\n", i);
        // 搜索事件区间
        SearchRegion(level, fTempThr[i], &pTmpEventsRegionTable[i], pAlgorithmInfo);
        level++;
    }

    // 事件区间组合
    memcpy(g_pEventsRegionTable, &pTmpEventsRegionTable[0], sizeof(EVENTS_REGION_TABLE));
	g_pEventsRegionTable->iBlind = iBlind;
    for (i = 1; i <= IntegrationTimes; ++i)
    {
		ALG_DEBUG("------------------RegionIntegration--------i = %d\n\n", i);
        RegionIntegration(g_pEventsRegionTable, &pTmpEventsRegionTable[i]);
    }

    g_pEventsRegionTable->EventsRegionInfo[0].iBegin = 0; 		//!< 对起始事件位置重新赋值
    g_pEventsRegionTable->EventsRegionInfo[0].iEnd = iBlind;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetFrontEventsTable1-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetFrontEventsTable1-----------------------\n\n");
		PrintEventRegionInfo(test_file_name, g_pEventsRegionTable);
	#else
		PrintEventRegionInfo(stdout, g_pEventsRegionTable);
	#endif
	//PrintEventRegionInfo(stdout, g_pEventsRegionTable);
	ALG_DEBUG("\n\n\n");
#endif


	//根据组合次数，将各个区间里的同一个事件的起始点和结束点，改为相同值。以小窗口为准，
	//如果小窗口下的事件类型和大窗口的不同，则使用大窗口下的参数changed by sjt 2015.07.19
	i = 0;
	while (i < g_pEventsRegionTable->iEventsNumber - 1)
	{
		iSaveIndex = i;
		int iTemp = Min(i+IntegrationTimes, g_pEventsRegionTable->iEventsNumber - 1);

		for (j = i + 1; j <= iTemp; ++j)
		{
			if ((g_pEventsRegionTable->EventsRegionInfo[j].iBegin 
				<= g_pEventsRegionTable->EventsRegionInfo[0].iBegin)
				&& (g_pEventsRegionTable->EventsRegionInfo[j].iStyle 
				== g_pEventsRegionTable->EventsRegionInfo[0].iStyle))
			{
				g_pEventsRegionTable->EventsRegionInfo[j]
					= g_pEventsRegionTable->EventsRegionInfo[0];
				continue;
			}
			if (g_pEventsRegionTable->EventsRegionInfo[j].iBegin 
				- g_pEventsRegionTable->EventsRegionInfo[i].iBegin < iBlind)
			{
				//反射峰前的下冲导致的误判
				if ((g_pEventsRegionTable->EventsRegionInfo[i].iStyle == 0)
					&& (g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 2))
				{
					g_pEventsRegionTable->EventsRegionInfo[i]
						= g_pEventsRegionTable->EventsRegionInfo[j];
				}
				else
				{
					g_pEventsRegionTable->EventsRegionInfo[j]
						= g_pEventsRegionTable->EventsRegionInfo[i];
				}
				continue;
			}
			if ((g_pEventsRegionTable->EventsRegionInfo[j].iBegin 
				- g_pEventsRegionTable->EventsRegionInfo[i].iBegin) 
				<= Max(iBlind, g_pEventsRegionTable->EventsRegionInfo[i].iWindow))
			{
				if ((g_pEventsRegionTable->EventsRegionInfo[i].iStyle 
					> g_pEventsRegionTable->EventsRegionInfo[j].iStyle)
					&& (g_pEventsRegionTable->EventsRegionInfo[i].iWindow
					> g_pEventsRegionTable->EventsRegionInfo[j].iWindow))
				{
					g_pEventsRegionTable->EventsRegionInfo[j]
						= g_pEventsRegionTable->EventsRegionInfo[i];
				}
				else
				{
					if (g_pEventsRegionTable->EventsRegionInfo[i].iWindow
						> g_pEventsRegionTable->EventsRegionInfo[j].iWindow) 
					{
						g_pEventsRegionTable->EventsRegionInfo[i]
							= g_pEventsRegionTable->EventsRegionInfo[j];
					}
					else
					{
						iSaveIndex = j - 1;
						break;
					}
				}
				iSaveIndex = j;
			}
			else
			{
				break;
			}
		}
		for (k = i; k <= iSaveIndex; ++k)
		{
			g_pEventsRegionTable->EventsRegionInfo[k]
				= g_pEventsRegionTable->EventsRegionInfo[iSaveIndex];
		}
		i = j;
	}

    g_pEventsRegionTable->iBlind = pAlgorithmInfo->iBlind;

	int pSaveIndex[MAX_EVENTS_NUMBER];
	pSaveIndex[0] = 0;
	iSaveIndex = 1;
	for (i = 1; i < g_pEventsRegionTable->iEventsNumber; ++i)
	{
		//将重复事件删除;
		if ((g_pEventsRegionTable->EventsRegionInfo[i].iBegin
			!= g_pEventsRegionTable->EventsRegionInfo[i - 1].iBegin)
			&& (g_pEventsRegionTable->EventsRegionInfo[i].iEnd
			!= g_pEventsRegionTable->EventsRegionInfo[i - 1].iEnd))
		{
			pSaveIndex[iSaveIndex] = i;
			iSaveIndex++;
		}
	}

	for (i = 0; i < iSaveIndex; ++i)
	{
		g_pEventsRegionTable->EventsRegionInfo[i]
			= g_pEventsRegionTable->EventsRegionInfo[pSaveIndex[i]];
	}
	g_pEventsRegionTable->iEventsNumber = iSaveIndex;
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetFrontEventsTable2-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetFrontEventsTable2-----------------------\n\n");
		PrintEventRegionInfo(test_file_name, g_pEventsRegionTable);
	#else
		PrintEventRegionInfo(stdout, g_pEventsRegionTable);
	#endif
	//PrintEventRegionInfo(stdout, g_pEventsRegionTable);
	ALG_DEBUG("\n\n\n");
#endif

    // 事件区间的精确定位及事件类型判断;
    EventsAnalysis(g_pSignalLog, pAlgorithmInfo->iSignalLength,
		iBlind, g_pEventsRegionTable, fSampleRating);
    
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetFrontEventsTable3-----------------------\n\n");
	#if ISSERIAL == 1
        fprintf(test_file_name, "------------------GetFrontEventsTable3-----------------------\n\n");
		PrintEventRegionInfo(test_file_name, g_pEventsRegionTable);
	#else
		PrintEventRegionInfo(stdout, g_pEventsRegionTable);
	#endif
	//PrintEventRegionInfo(stdout, g_pEventsRegionTable);
	ALG_DEBUG("\n\n\n");
#endif

    free(pTmpEventsRegionTable);
    pTmpEventsRegionTable = NULL;
}

// 精确定位事件位置
void FixPositionAccurate(ALGORITHM_INFO *pAlgorithmInfo)
{
    int i = 0, j = 0, k = 0;
    int iBlind = Max(pAlgorithmInfo->iBlind, 1);
    float fSampleRating = pAlgorithmInfo->SetParameters.fSampleRating;

    float standardAttenuation[] = { 0.33f, 0.18f, 0.2f };
    int len = Max((int)(10.0f * fSampleRating / 2.56f), iBlind);
    for (j = 1; j < g_pEventsRegionTable->iEventsNumber; ++j)
    {
        int count = 0, start = 0, end = 0;

        int curMaxPos = 0, searchLen = 0;
        float preMean = 0.0f, bliMean = 0.0f, maxMean = 0.0f, thresh = 0.0f;

        // 最后一个事件
        if ((j == g_pEventsRegionTable->iEventsNumber - 1) 
			&& (g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 2))
        {
            searchLen = Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd
                - g_pEventsRegionTable->EventsRegionInfo[j].iBegin, 0);
            curMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog
                + g_pEventsRegionTable->EventsRegionInfo[j].iBegin, searchLen)
                + g_pEventsRegionTable->EventsRegionInfo[j].iBegin;
            preMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
                + Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin
				- len, 0), Max((int)(len / 2), 1));

            for (i = curMaxPos; i < pAlgorithmInfo->iSignalLength; ++i)
            {
                if (pAlgorithmInfo->pSignalLog[i] < preMean)
                {
                    count++;
                }
                else
                {
                    count = 0;
                }
                if (count >= 3)
                {
                    g_pEventsRegionTable->EventsRegionInfo[j].iEnd
                        = Max(i - count + 1, g_pEventsRegionTable->EventsRegionInfo[j].iEnd);
                    break;
                }
            }

            // 调整起始点
            // 目前只有少数情况下事件点位置有偏差，故制作修补
            if ((curMaxPos > g_pEventsRegionTable->EventsRegionInfo[j].iBegin)
                && (curMaxPos < g_pEventsRegionTable->EventsRegionInfo[j].iEnd)
				&& (frontParament.pulseWidth > ENUM_PULSE_200NS))
            {
                g_pEventsRegionTable->EventsRegionInfo[j].iBegin
                    = GetMinIndex(pAlgorithmInfo->pSignalLog
					+ g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd,
                    Max(curMaxPos - g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd, 0) + 1)
                    + g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd;
            }

        }
		else if((g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 0)
			&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_20US))
        {
			if (pAlgorithmInfo->PulseWidth > ENUM_PULSE_5NS)
            {
                preMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
                    + Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - len, 0),
					Max((int)(len / 2), 1));
                bliMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
                    + Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + 2 * iBlind,
                    g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - len), Max(len, 1));
                // 补偿一个盲区内的区间损耗
                bliMean += iBlind*pAlgorithmInfo->fPointLength / 1000
					* standardAttenuation[frontParament.wave];

                start = Max(Min(g_pEventsRegionTable->EventsRegionInfo[j].iBegin + len,
                    g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - len - 1), len);


                if (j == g_pEventsRegionTable->iEventsNumber - 1)
                {
                    preMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
						+ Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin
						- len, 0), Max((int)(len / 2), 1));

                    bliMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
                        + Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + 2 * iBlind,
                        pAlgorithmInfo->iSignalLength - len), Max(len / 2, 1));
                    start = Min(g_pEventsRegionTable->EventsRegionInfo[j].iBegin 
						+ 2 * len, pAlgorithmInfo->iSignalLength);
                }

                for (k = 0; k < 2; ++k)
                {
                    thresh = 0.0f;
                    if (k == 0)
                    {
                        thresh = 0.98f * (preMean - bliMean) + bliMean;
                    }
                    else
                    {
                        thresh = 0.05f * (preMean - bliMean) + bliMean;
                    }
                    for (i = start; i > start - 2 * len; --i)
                    {
                        if (pAlgorithmInfo->pSignalLog[i] > thresh)
                        {
                            count++;
                        }
                        else
                        {
                            count = 0;
                        }
                        if (count >= 5)
                        {
                            if (k == 0)
                                g_pEventsRegionTable->EventsRegionInfo[j].iBegin = i + count;

                            else
                                g_pEventsRegionTable->EventsRegionInfo[j].iEnd = i + count;

                            break;
                        }
                    }
                }
            }
        }

		// 对上升沿事件不进行末端定位
		else if((pAlgorithmInfo->PulseWidth <= ENUM_PULSE_20US)
			&& (g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 2))
        {


            searchLen = Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd
                - g_pEventsRegionTable->EventsRegionInfo[j].iBegin, 0);
            curMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog
                + g_pEventsRegionTable->EventsRegionInfo[j].iBegin, searchLen)
                + g_pEventsRegionTable->EventsRegionInfo[j].iBegin;

            preMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
                + Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin
				- len, 0), Max((int)(len / 2), 1));

            if (g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin
                - g_pEventsRegionTable->EventsRegionInfo[j].iEnd < 2 * (iBlind + len))
            {
                bliMean = pAlgorithmInfo->pSignalLog[Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd
                    + len, pAlgorithmInfo->iSignalLength - 1)];
            }
            else
            {
                bliMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
                    + Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + 2 * iBlind,
                    g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - len), len);
            }


            maxMean = GetMeanFloat(pAlgorithmInfo->pSignalLog
				+ g_pEventsRegionTable->EventsRegionInfo[j].iBegin,
                Max(curMaxPos - g_pEventsRegionTable->EventsRegionInfo[j].iBegin, 1));

            if (preMean < bliMean)
            {
                thresh = Min(preMean, bliMean);
            }
            else
            {
                thresh = (preMean - bliMean) * 0.1f + bliMean;
            }
            start = curMaxPos;
            end = g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin;
            for (i = start; i < end; i++)
            {
                if (pAlgorithmInfo->pSignalLog[i] < thresh)
                {
                    count++;
                }
                else
                {
                    count = 0;
                }
                if (count >= 3)
                {
                    g_pEventsRegionTable->EventsRegionInfo[j].iEnd = i - count + 1;
                    break;
                }
            }
			searchLen = Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd 
					  - g_pEventsRegionTable->EventsRegionInfo[j].iBegin, 0);
			curMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog 
				      + g_pEventsRegionTable->EventsRegionInfo[j].iBegin, searchLen)
				      + g_pEventsRegionTable->EventsRegionInfo[j].iBegin;
			// 调整起始点
			// 目前只有少数情况下事件点位置有偏差，故制作修补
			if ((curMaxPos > g_pEventsRegionTable->EventsRegionInfo[j].iBegin)
			&& (curMaxPos <= g_pEventsRegionTable->EventsRegionInfo[j].iEnd)
			&& (frontParament.pulseWidth >= ENUM_PULSE_200NS)
			&& (preMean > pAlgorithmInfo->fMaxInputNoise))
			{
				int iTmpLen = Max(g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd,
					g_pEventsRegionTable->EventsRegionInfo[j].iBegin - len);
				int iTmpFlag = 0;
				for (i = curMaxPos; i > iTmpLen; --i)
				{
					if (pAlgorithmInfo->pSignalLog[i-1] - pAlgorithmInfo->pSignalLog[i] > 0.12f
						|| fabs(pAlgorithmInfo->pSignalLog[i] - preMean) < 0.05f)
					{
						g_pEventsRegionTable->EventsRegionInfo[j].iBegin = i;
						iTmpFlag = 1;
						break;
					}
				}
				if (iTmpFlag == 0)
				{
					g_pEventsRegionTable->EventsRegionInfo[j].iBegin
						= GetMinIndex(pAlgorithmInfo->pSignalLog + iTmpLen,
						Max(curMaxPos - iTmpLen, 0) + 1) + iTmpLen;
				}
				
			}
			
        }
        // 对于增益事件位置重新定位
        else if ((g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 1)
			&& (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_2US))
        {
            if ((g_pEventsRegionTable->EventsRegionInfo[j].iBegin
				- g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd < 2 * iBlind)
                && (g_pEventsRegionTable->EventsRegionInfo[j].iBegin
					- g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd > 0))
            {
                curMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog
					+ g_pEventsRegionTable->EventsRegionInfo[j].iBegin,
                    g_pEventsRegionTable->EventsRegionInfo[j].iEnd
					- g_pEventsRegionTable->EventsRegionInfo[j].iBegin)
                    + g_pEventsRegionTable->EventsRegionInfo[j].iBegin;
                searchLen = curMaxPos - g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd;
                start = GetMinIndex(pAlgorithmInfo->pSignalLog
					+ g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd, searchLen)
                    + g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd;
                g_pEventsRegionTable->EventsRegionInfo[j].iBegin
					= Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin, start);
            }
        }
    }
	float fMaxNoise = GetMaxFloat(pAlgorithmInfo->pSignalLog
		+ pAlgorithmInfo->iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
    // 针对衰减事件位置点不准确
    for (i = 1; i < g_pEventsRegionTable->iEventsNumber; ++i)
    {
        if ((g_pEventsRegionTable->EventsRegionInfo[i].iStyle == 0)
			&& (frontParament.pulseWidth >= ENUM_PULSE_1US))
        {
            int begin = g_pEventsRegionTable->EventsRegionInfo[i].iBegin;
            if (begin - g_pEventsRegionTable->EventsRegionInfo[i - 1].iEnd > 10 * Max(iBlind, 10))
            {
                for (j = begin; j > g_pEventsRegionTable->EventsRegionInfo[i - 1].iEnd + iBlind; --j)
                {
                    j = Max(Max(j, 2), iBlind);
                    float delta = pAlgorithmInfo->pSignalLog[j] - pAlgorithmInfo->pSignalLog[j - 2];
					float fMaxBegin = GetMaxFloat(pAlgorithmInfo->pSignalLog + j - iBlind, iBlind);
					float fMinBegin = GetMinFloat(pAlgorithmInfo->pSignalLog + j - iBlind, iBlind);
					float maxDelta = fMaxBegin - fMinBegin - iBlind
						* g_fLengthPerPoint / 1000 * standardAttenuation[frontParament.wave];

                    if ((fabsf(delta) < 0.2f) && (maxDelta < 0.2f))
                    {
						g_pEventsRegionTable->EventsRegionInfo[i].iBegin
							= Max(j, g_pEventsRegionTable->EventsRegionInfo[i].iMaxIndex
							- Max(iBlind, g_pEventsRegionTable->EventsRegionInfo[i].iWindow));
						
						g_pEventsRegionTable->EventsRegionInfo[i].iEnd = j + iBlind;
						#if 0
						g_pEventsRegionTable->EventsRegionInfo[i].iBegin 
						= Min(g_pEventsRegionTable->EventsRegionInfo[i - 1].iEnd-1,
							g_pEventsRegionTable->EventsRegionInfo[i].iBegin);

                        if (i < g_pEventsRegionTable->iEventsNumber - 1)
                        {
                            g_pEventsRegionTable->EventsRegionInfo[i].iEnd
                                = Min(g_pEventsRegionTable->EventsRegionInfo[i + 1].iBegin - 1,
									g_pEventsRegionTable->EventsRegionInfo[i].iEnd);
                        }
                        else
                        {
                            g_pEventsRegionTable->EventsRegionInfo[i].iEnd
                                = Min(pAlgorithmInfo->iDisplayLenth - 1,
									g_pEventsRegionTable->EventsRegionInfo[i].iEnd);
                        }
                        printf("i = %d, begin = %d, end = %d\n", i, j, j + iBlind);
#endif
                        break;
                    }
					else if ((fMaxBegin < fMaxNoise + 4.5f) 
						&& (fMinBegin > fMaxNoise + 0.5f) && (fabsf(delta) < 0.2f) )
					{
						g_pEventsRegionTable->EventsRegionInfo[i].iBegin
							= Max(j, g_pEventsRegionTable->EventsRegionInfo[i].iMaxIndex
							- Max(iBlind, g_pEventsRegionTable->EventsRegionInfo[i].iWindow));
						
						g_pEventsRegionTable->EventsRegionInfo[i].iEnd = j + iBlind;
					}
				}
			}
		}
	}
}

// 获取事件特征信息
void GetEventsTable(ALGORITHM_INFO *pAlgorithmInfo)
{
    EVENTS_TABLE *pEventsTable = &pAlgorithmInfo->EventsTable;
    int i = 0;
    int j = 0;
    int iBlind = pAlgorithmInfo->iBlind;

	float SaveEventLoss[MAX_EVENTS_NUMBER] = { 0 };
    int pSaveIndex[MAX_EVENTS_NUMBER] = { 0 };  // 未删除的事件索引
    int iSaveNumber = 0;					    // 未删除的事件数量
    float fLimitLoss = 0.0f;
    float fSampleRating = pAlgorithmInfo->SetParameters.fSampleRating;
    int iPointNum = (int)(10.0f * fSampleRating);
    float fEndThr =  pAlgorithmInfo->fMaxInputNoise + 0.72f;
    memset(pEventsTable, 0, sizeof(EVENTS_TABLE));
    float fMaxEventValue = 0.0f;
    ALG_DEBUG("------------------GetEventsTable 0-----------------------\n\n");
    // 调整熔接事件位置
    // 10ns由于信噪比差故做特殊处理
    int Len = Max((int)(10.0f * fSampleRating / 2.56f), iBlind);
	Len = Max(Len, 16);
	float normalAttenuation[][2] = {{0.30f, 0.36f}, {0.15f, 0.21f}, {0.17f, 0.23f}};	
	int looptimes = 0;
	for (looptimes = 0; looptimes < 1; ++looptimes)
	{
	    for (j = 1; j < g_pEventsRegionTable->iEventsNumber; ++j)
	    {
	        int Count = 0;
	        int EndFlag = 0;

	        // 最后一个事件
	        if (j == g_pEventsRegionTable->iEventsNumber - 1
				&& g_pEventsRegionTable->EventsRegionInfo[j].iStyle != 2)
	        {
	            int SearchLen = Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd
					- g_pEventsRegionTable->EventsRegionInfo[j].iBegin, 0);
	            int CurMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog 
					+ g_pEventsRegionTable->EventsRegionInfo[j].iBegin,
					Max(Min(SearchLen, pAlgorithmInfo->iSignalLength
						- g_pEventsRegionTable->EventsRegionInfo[j].iBegin + 1), 1))
					+ g_pEventsRegionTable->EventsRegionInfo[j].iBegin;
	            float Premean = GetMeanFloat(pAlgorithmInfo->pSignalLog 
					+ Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin
						- Len, 0), Max((int)(Len / 2), 1));

	            for (i = CurMaxPos; i < pAlgorithmInfo->iSignalLength; ++i)
	            {
	                if (pAlgorithmInfo->pSignalLog[i] < Premean)
	                {
	                    Count++;
	                }
	                else
	                {
	                    Count = 0;
	                }
	                if (Count >= 3)
	                {
	                    g_pEventsRegionTable->EventsRegionInfo[j].iEnd
							= Max(i - Count + 1, g_pEventsRegionTable->EventsRegionInfo[j].iEnd);
	                    break;
	                }
	            }
	            continue;
	        }

	        if ((g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 0)
				&& (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_20US)
				&& (j < g_pEventsRegionTable->iEventsNumber - 1))
	        {
	            if (pAlgorithmInfo->PulseWidth > 2)
	            {
	                int k = 0;
					int preMeanStartIndex = Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - Len, 0);
	                float Premean = GetMeanFloat(pAlgorithmInfo->pSignalLog + preMeanStartIndex, Max((int)(Len / 2), 1));
					
					int bliMeanStartIndex = Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + 2 * iBlind,
												g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - Len);
					float Blimean = GetMeanFloat(pAlgorithmInfo->pSignalLog + bliMeanStartIndex, Max(Len, 1));

					/*
					if ((pAlgorithmInfo->PulseWidth < ENUM_PULSE_1US)
						&& (g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin
						- g_pEventsRegionTable->EventsRegionInfo[j].iBegin
						* pAlgorithmInfo->fPointLength < 200.0f))
					{
						Blimean = GetMeanFloat(pAlgorithmInfo->pSignalLog 
						+ Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + 2 * iBlind,
							g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - Len), Max(Len, 1));
					}
					*/

					float atten = (bliMeanStartIndex - g_pEventsRegionTable->EventsRegionInfo[j].iEnd)
						* pAlgorithmInfo->fPointLength / 1000.0f * Normal_fAttenuation;

	                int start = Max(Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin + Len,
						g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - Len - 1), Len);
	                for (k = 0; k < 2; ++k)
	                {
	                    float Thresh = 0.0f;
						Count = 0;
	                    if (k == 0)
	                    {
	                        Thresh = 0.95f * (Premean - Blimean - atten) + Blimean;
							Thresh = Min(Thresh, Premean - 0.02f);
							//Thresh = 0.95f * (Premean - Blimean) + Blimean;
	                    }
	                    else
	                    {
	                        Thresh = 0.05f * (Premean - Blimean - atten) + Blimean;
							//Thresh = 0.05f * (Premean - Blimean) + Blimean;
	                    }
						int tmpiE = Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - Len, 0);
	                    for (i = start; i > tmpiE; i--)
	                    {
	                        if (pAlgorithmInfo->pSignalLog[i] > Thresh)
	                        {
	                            Count++;
	                        }
	                        else
	                        {
	                            Count = 0;
	                        }
	                        if (Count >= 3)
	                        {
	                            if (k == 0)
	                            {
	                            	if (fSampleRating < 4.0f)
	                            	{
										/*
		                                g_pEventsRegionTable->EventsRegionInfo[j].iBegin
		                                = Min(Max(i + Count - (int)fSampleRating - 5, 0),
											g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - iBlind);
										*/
										g_pEventsRegionTable->EventsRegionInfo[j].iBegin
											= Min(Max(i + Count - Min((int)(fSampleRating / 2), iBlind / 2) - 5, 0),
												g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - iBlind);
	                            	}
									else
									{
										/*
										g_pEventsRegionTable->EventsRegionInfo[j].iBegin
		                                = Min(Max(i + Count - (int)fSampleRating, 0),
											g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - iBlind);
										*/
										g_pEventsRegionTable->EventsRegionInfo[j].iBegin
											= Min(Max(i + Count - Min((int)(fSampleRating / 2), iBlind / 2), 0),
												g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - iBlind);
									}
	                            }
	                            else
	                                g_pEventsRegionTable->EventsRegionInfo[j].iEnd
	                                = Max(i + Count - (int)fSampleRating, 0);
	                            break;
	                        }
	                    }
	                }
					int iMax = g_pEventsRegionTable->EventsRegionInfo[j].iMaxIndex;
					int iWin = g_pEventsRegionTable->EventsRegionInfo[j].iWindow;
					if ((pAlgorithmInfo->PulseWidth < ENUM_PULSE_50NS)
						&& ((g_pEventsRegionTable->EventsRegionInfo[j].iBegin > +iMax + iWin)
							|| (g_pEventsRegionTable->EventsRegionInfo[j].iBegin < iMax - iWin)))
					{
						g_pEventsRegionTable->EventsRegionInfo[j].iBegin = iMax + iWin / 2 - (int)(log(iWin) / log(2));
					}
					g_pEventsRegionTable->EventsRegionInfo[j].iEnd
						= Min(Min(Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd,
							g_pEventsRegionTable->EventsRegionInfo[j].iBegin + iBlind),
							g_pEventsRegionTable->EventsRegionInfo[j].iBegin + 2 * iBlind),
							g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - Len);
	            }
	        }
			else if ((g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 0)
				&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_20US)
				&& (j == g_pEventsRegionTable->iEventsNumber - 1)
				&& (g_pEventsRegionTable->EventsRegionInfo[j].iBegin > 3))
	        {
	            if (pAlgorithmInfo->PulseWidth > 1)
	            {
	                float Premean = GetMeanFloat(pAlgorithmInfo->pSignalLog
						+ Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - Len, 0),
						      Max((int)(Len / 2), 1));
	                float Blimean = GetMeanFloat(pAlgorithmInfo->pSignalLog 
						+ Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + 2 * iBlind,
						       pAlgorithmInfo->iSignalLength - Len), Max(Len, 1)); 
					Blimean = Min(Blimean, GetMeanFloat(pAlgorithmInfo->pSignalLog
						+ pAlgorithmInfo->iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN));
	                int start = Max(Min(g_pEventsRegionTable->EventsRegionInfo[j].iBegin + Len, 
						             pAlgorithmInfo->iSignalLength - Len - 1), Len);
					float Thresh = 0.0f;
					Thresh = 0.05f * (Premean - Blimean) + Blimean;
					Count = 0;
					for (i = start; i < pAlgorithmInfo->iSignalLength - 1; ++i)
	                {
	                    if (pAlgorithmInfo->pSignalLog[i] < Thresh)
	                    {
	                        Count++;
	                    }
	                    else
	                    {
	                        Count = 0;
	                    }
	                    if (Count >= 3)
	                    {
	                        g_pEventsRegionTable->EventsRegionInfo[j].iEnd
	                        = Max(i - Count - (int)fSampleRating, 0);
	                        break;
	                    }
	                }
					start = Max(Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd, 
						             pAlgorithmInfo->iSignalLength - Len - 1), Len);
					Count = 0;
					Thresh = 0.95f * Premean;
					int endIndex = Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - Len, 0);
					for (i = start; i > endIndex; --i)
	                {
	                    if (pAlgorithmInfo->pSignalLog[i] > Thresh)
	                    {
	                        Count++;
	                    }
	                    else
	                    {
	                        Count = 0;
	                    }
	                    if ((Count >= 3)
							&& (pAlgorithmInfo->pSignalLog[i] >= pAlgorithmInfo->pSignalLog[i - 1])
							&& (pAlgorithmInfo->pSignalLog[i] >= pAlgorithmInfo->pSignalLog[i + 1])
							&& (i < pAlgorithmInfo->iSignalLength && i > 0))
	                    {
	                        g_pEventsRegionTable->EventsRegionInfo[j].iBegin = Max(i, 0);
	                        break;
	                    }
	                }
					g_pEventsRegionTable->EventsRegionInfo[j].iEnd
						= Min(Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin + iBlind,
							g_pEventsRegionTable->EventsRegionInfo[j].iEnd),
							pAlgorithmInfo->iSignalLength);
	            }
	        }
	        // 对上升沿事件不进行末端定位
	        else if ((pAlgorithmInfo->PulseWidth <= ENUM_PULSE_20US) && (j > 0)
				&& (g_pEventsRegionTable->EventsRegionInfo[j].iStyle == 2))
	        {
	            int SearchLen = Max(Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd
	                - g_pEventsRegionTable->EventsRegionInfo[j].iBegin, 0), 2 * iBlind);
				if (j < g_pEventsRegionTable->iEventsNumber - 1)
				{
					if (pAlgorithmInfo->PulseWidth > ENUM_PULSE_500NS)
					{
						SearchLen = Min(SearchLen, g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin
			                - g_pEventsRegionTable->EventsRegionInfo[j].iBegin);
					}
					else
					{
						SearchLen = Max(SearchLen, g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin
							- g_pEventsRegionTable->EventsRegionInfo[j].iBegin);
					}
				}
				else
				{
					SearchLen = Min(SearchLen, Min(pAlgorithmInfo->iSignalLength
		                - g_pEventsRegionTable->EventsRegionInfo[j].iEnd, 256));
				}
	            int CurMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog
	                + g_pEventsRegionTable->EventsRegionInfo[j].iBegin, SearchLen)
	                + g_pEventsRegionTable->EventsRegionInfo[j].iBegin;

	            float Premean = GetMeanFloat(pAlgorithmInfo->pSignalLog
	                + Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - Len / 2, 0),
	                		Max((int)(Len / 2), 1));
	            float Blimean = 0.0f;
	            float Thresh = 0.0f;
				if (j < g_pEventsRegionTable->iEventsNumber - 1)
				{
		            if (g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin
						- g_pEventsRegionTable->EventsRegionInfo[j].iEnd < 2 * (iBlind + Len))
		            {
		                Blimean = GetMeanFloat(pAlgorithmInfo->pSignalLog
							+ Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + Len,
							        g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - Len), Len);
		            }
		            else
		            {
		                Blimean = GetMeanFloat(pAlgorithmInfo->pSignalLog
							+ Min(g_pEventsRegionTable->EventsRegionInfo[j].iEnd + 2 * iBlind,
							        g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - Len), Len);
		            }
				}
	            if (Premean < Blimean)
	            {
	                Thresh = Min(Premean, Blimean);
	            }
	            else
	            {
	                Thresh = (Premean - Blimean) * 0.1f + Blimean;
	            }
				Count = 0;
	            for (i = CurMaxPos; i < g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin; ++i)
	            {
	                if (pAlgorithmInfo->pSignalLog[i] < Thresh)
	                {
	                    Count++;
	                }
	                else
	                {
	                    Count = 0;
	                }
	                if (Count >= 3)
	                {
	                    g_pEventsRegionTable->EventsRegionInfo[j].iEnd
							= Max(i - Count + 1, g_pEventsRegionTable->EventsRegionInfo[j].iEnd);
	                    EndFlag = 1;
	                }
	                if (EndFlag == 1)
	                {
	                    break;
	                }
	            }
	            if (EndFlag != 1)
	            {
	                g_pEventsRegionTable->EventsRegionInfo[j].iEnd
						= Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd,
						       g_pEventsRegionTable->EventsRegionInfo[j].iBegin + iBlind);
	            }
				// 增加起始点的再判断
				EndFlag = 0;
				Count = 0;
				int CurMinPos = GetMinIndex(pAlgorithmInfo->pSignalLog
	                + Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin
						- Min(iBlind / 4, (int)(25 / pAlgorithmInfo->fPointLength)), 0),
					Max(CurMaxPos - Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin
						- Min(iBlind / 4, (int)(25 / pAlgorithmInfo->fPointLength)), 0), 1))
	                + Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin
						- Min(iBlind / 4, (int)(25 / pAlgorithmInfo->fPointLength)), 0);
				float TempMaxValue = 0;
				int k = 0;
				int temp = 0;
				// 一阶导数最大值的前端
				if (CurMaxPos < pAlgorithmInfo->iSignalLength - 2)
				{
					int tmpStart = Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - iBlind, 0);
					for (k = tmpStart; k < CurMaxPos; ++k)
					{
						if ((pAlgorithmInfo->pSignalLog[k + 1] - pAlgorithmInfo->pSignalLog[k] > TempMaxValue)
							&& (pAlgorithmInfo->pSignalLog[k + 2] - pAlgorithmInfo->pSignalLog[k] > TempMaxValue))
						{
							TempMaxValue = pAlgorithmInfo->pSignalLog[k + 1] - pAlgorithmInfo->pSignalLog[k];
							temp = k;
						}
					}
				}

				if (((temp <= CurMinPos || temp >= CurMaxPos) && (j >= 1)) || (temp == 0))
				{
					TempMaxValue = 0;
					if (j != 1)
					{
						int calMaxStartIndex = Max(g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd,
							g_pEventsRegionTable->EventsRegionInfo[j].iBegin - 100 * iPointNum);
						calMaxStartIndex = Max(calMaxStartIndex, g_pEventsRegionTable->EventsRegionInfo[j].iMinIndex
							- g_pEventsRegionTable->EventsRegionInfo[j].iWindow / 2);
						CurMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog + calMaxStartIndex,
							Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd - calMaxStartIndex, 0)) + calMaxStartIndex;

						int calMinStartIndex = Max(g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd, CurMaxPos - (int)(1.5f * iBlind));
						CurMinPos = GetMinIndex(pAlgorithmInfo->pSignalLog + calMinStartIndex,
							Max(CurMaxPos - calMinStartIndex, 1)) + calMinStartIndex;
						/*
						CurMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog
		                + g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd,
							Max(g_pEventsRegionTable->EventsRegionInfo[j].iEnd
								- g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd, 0))
		                + g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd;
						CurMinPos = GetMinIndex(pAlgorithmInfo->pSignalLog
		                + Max(g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd, CurMaxPos - iBlind),
							Max(CurMaxPos - Max(g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd,
								CurMaxPos - iBlind), 1))
		                + Max(g_pEventsRegionTable->EventsRegionInfo[j - 1].iEnd, CurMaxPos - iBlind);
						*/
					}
					for (k = CurMinPos; k < CurMaxPos; k++)
					{
						if (pAlgorithmInfo->pSignalLog[k + 1] - pAlgorithmInfo->pSignalLog[k] > TempMaxValue)
						{
							TempMaxValue = pAlgorithmInfo->pSignalLog[k + 1] - pAlgorithmInfo->pSignalLog[k];
							temp = k;
						}
					}
				}
				int tmpEnd = Min(CurMinPos, temp - iBlind);
				float fDelta = Min(0.1f, TempMaxValue / 2.0f);
				for (i = temp; i >= tmpEnd; --i)
	            {
	                if ((i > 0) && (fabs(pAlgorithmInfo->pSignalLog[i]
						- pAlgorithmInfo->pSignalLog[i - 1]) < fDelta))
	                {
	                    Count++;
	                }
	                else
	                {
	                    Count = 0;
	                }
	                if (Count >= 3)
	                {
	                    g_pEventsRegionTable->EventsRegionInfo[j].iBegin = i + Count - 1;
						if ((g_pEventsRegionTable->EventsRegionInfo[j].iBegin < CurMinPos)
							&& (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US))
						{
							g_pEventsRegionTable->EventsRegionInfo[j].iBegin += 1;
						}
						else if ((g_pEventsRegionTable->EventsRegionInfo[j].iBegin > CurMinPos)
							&& (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US))
						{
							g_pEventsRegionTable->EventsRegionInfo[j].iBegin -= 1;
						}
	                    EndFlag = 1;
	                }
	                if (EndFlag == 1)
	                {
	                    break;
	                }
	            }
				int comIndex = Max(DaqNum[pAlgorithmInfo->PulseWidth] - 2, 0);
				int compsition = pAlgorithmInfo->pCombineInfo[0][comIndex];
				if ((pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US)
					&& (pAlgorithmInfo->pSignalLog[CurMaxPos] - pAlgorithmInfo->pSignalLog[CurMinPos] < 1.0f)
					&& (g_pEventsRegionTable->EventsRegionInfo[j].iBegin > compsition)
					&& (fabs(g_pEventsRegionTable->EventsRegionInfo[j].iMinIndex - CurMinPos)
						< g_pEventsRegionTable->EventsRegionInfo[j].iWindow / 2))
				{
					g_pEventsRegionTable->EventsRegionInfo[j].iBegin = CurMinPos;
				}

				int iMin = g_pEventsRegionTable->EventsRegionInfo[j].iMinIndex;
				int iWin = g_pEventsRegionTable->EventsRegionInfo[j].iWindow;
				int position = iMin + iWin / 2 - (int)(log(iWin) / log(2));
				if ((pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US)
					&& (fabs(g_pEventsRegionTable->EventsRegionInfo[j].iBegin - position) >= 10)
					&& (fabs(CurMinPos - position) <= 10) && (fDelta < 0.105f))
				{
					g_pEventsRegionTable->EventsRegionInfo[j].iBegin = CurMinPos;
				}

				if (j < g_pEventsRegionTable->iEventsNumber - 1
					&& g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin
					- g_pEventsRegionTable->EventsRegionInfo[j].iEnd < Len)
				{
					g_pEventsRegionTable->EventsRegionInfo[j].iEnd
						= Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin + 2 * iBlind,
							g_pEventsRegionTable->EventsRegionInfo[j].iEnd);
					g_pEventsRegionTable->EventsRegionInfo[j].iEnd
						= Min(g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - 1,
							g_pEventsRegionTable->EventsRegionInfo[j].iEnd);
				}
				else if (j < g_pEventsRegionTable->iEventsNumber - 1)
				{
					g_pEventsRegionTable->EventsRegionInfo[j].iEnd 
						= Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin + 2 * iBlind,
							g_pEventsRegionTable->EventsRegionInfo[j].iEnd);
					g_pEventsRegionTable->EventsRegionInfo[j].iEnd
						= Min(g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - 1,
							g_pEventsRegionTable->EventsRegionInfo[j].iEnd);
				}
				else
				{
					g_pEventsRegionTable->EventsRegionInfo[j].iEnd
						= Min(Max(g_pEventsRegionTable->EventsRegionInfo[j].iBegin + 2 * iBlind,
							g_pEventsRegionTable->EventsRegionInfo[j].iEnd),
							pAlgorithmInfo->iSignalLength - Len);
				}
	        }
	    }
	}

	//防止两个事件重叠
	for (j = 0; j < g_pEventsRegionTable->iEventsNumber - 1; ++j)
	{
		if (g_pEventsRegionTable->EventsRegionInfo[j].iEnd >= g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin)
		{
			g_pEventsRegionTable->EventsRegionInfo[j].iEnd = g_pEventsRegionTable->EventsRegionInfo[j + 1].iBegin - 1;
		}
	}

    GetEventsParameter(g_pSignalLog, pAlgorithmInfo->iSignalLength,
		g_pEventsRegionTable, pEventsTable, fSampleRating);

	// 重置事件合并导致的类型错误
	if (pAlgorithmInfo->PulseWidth > ENUM_PULSE_2US)
	{
		for (j = 0; j < pEventsTable->iEventsNumber; ++j)
		{
			if (pEventsTable->EventsInfo[j].fReflect > -35.0f)
			{
				pEventsTable->EventsInfo[j].iStyle = 2;
			}
			else
			{
				pEventsTable->EventsInfo[j].iEnd
					= Max(Min(pEventsTable->EventsInfo[j].iBegin + (int)(iBlind * 1.5f),
						pAlgorithmInfo->iSignalLength), pEventsTable->EventsInfo[j].iEnd);
			}
		}
	}

	if ((pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US)
		&& (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_20US))
	{
		for (j = 1; j < pEventsTable->iEventsNumber; ++j)
    	{
        
			if (pEventsTable->EventsInfo[j].fReflect > -65.0f)
			{
				pEventsTable->EventsInfo[j].iStyle = 2;
			}
#if 0
			int Count = 0;
	        int EndFlag = 0;
	        if ((pEventsTable->EventsInfo[j].iBegin > 3 * iBlind)
				&& (pEventsTable->EventsInfo[j].iEnd < pAlgorithmInfo->iSignalLength - 3 * iBlind))
	        {
	            int SearchLen = Min(6 * iBlind, 2 * (pEventsTable->EventsInfo[j].iBegin
					- pEventsTable->EventsInfo[j - 1].iEnd));
				if (j < pEventsTable->iEventsNumber - 1)
				{
					SearchLen = Min(SearchLen, 2 * (pEventsTable->EventsInfo[j + 1].iBegin
						- pEventsTable->EventsInfo[j].iEnd));
				}
	            int CurMaxPos = Max(GetMaxIndex(pAlgorithmInfo->pSignalLog
	                + pEventsTable->EventsInfo[j].iBegin - SearchLen / 2, SearchLen)
					+ pEventsTable->EventsInfo[j].iBegin - SearchLen / 2,
					pEventsTable->EventsInfo[j].iBegin);
				int SearchLen2 = CurMaxPos - (pEventsTable->EventsInfo[j].iBegin - SearchLen / 2);
				/*
				int CurMinPos = GetMinIndex(pAlgorithmInfo->pSignalLog
	                + pEventsTable->EventsInfo[j].iBegin - SearchLen / 2, SearchLen2)
	                + pEventsTable->EventsInfo[j].iBegin - SearchLen / 2;
				float freNoisestd = GetStdFloat(pAlgorithmInfo->pSignalLog + CurMinPos - 10, 10);
				*/
				float CurMaxValue = pAlgorithmInfo->pSignalLog[CurMaxPos];
				float CurMinValue = GetMinFloat(pAlgorithmInfo->pSignalLog
	                + pEventsTable->EventsInfo[j].iBegin - SearchLen / 2, SearchLen2);
				if (CurMaxValue - CurMinValue > 0.3f)
				{
					pEventsTable->EventsInfo[j].iStyle = 2;
				}
				EndFlag = 0;
				Count = 0;
				float TempMaxValue = 0.0f;
				int k = 0;
				int temp = 0;
				// 一阶导数最大值的前端
				int tmpKS = Max(pEventsTable->EventsInfo[j].iBegin - g_pEventsRegionTable->iBlind, 0);
				for (k = tmpKS; k < pEventsTable->EventsInfo[j].iEnd; ++k)
				{
					if (pAlgorithmInfo->pSignalLog[k + 1] - pAlgorithmInfo->pSignalLog[k] > TempMaxValue)
					{
						TempMaxValue = pAlgorithmInfo->pSignalLog[k + 1] - pAlgorithmInfo->pSignalLog[k];
						temp = k;
					}
				}

				if (temp != 0)
				{	
					if (pEventsTable->EventsInfo[j].iBegin - temp + 4 > 5)
					{
						pEventsTable->EventsInfo[j].iBegin = temp - 4;
					}	
				}
				pEventsTable->EventsInfo[j].iEnd 
					= Min(Max(pEventsTable->EventsInfo[j].iBegin + iBlind,
						pEventsTable->EventsInfo[j].iEnd), pAlgorithmInfo->iSignalLength - 1);
	        }
#endif
		}
	}

	// 衰减事件重搜索
	if ((pAlgorithmInfo->PulseWidth >= ENUM_PULSE_20NS)
		&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_500NS))
	{
		for (j = 0; j < pEventsTable->iEventsNumber - 1; j++)
		{
			int Count = 0;
			if ((pEventsTable->EventsInfo[j].fAttenuation > 2)
				&& ((pEventsTable->EventsInfo[j + 1].iBegin - pEventsTable->EventsInfo[j].iBegin)
				* pAlgorithmInfo->fPointLength > 50.0f))
			{
				int iBeginAdd = 0;
				int iEndAdd = 0;
				int iStyleAdd = 0;
				int k = 0;
                float Premean = GetMeanFloat(pAlgorithmInfo->pSignalLog
					+ Max(pEventsTable->EventsInfo[j].iEnd + iBlind, 0), Max((int)(Len / 2), 1));
				if (j == 0)
				{
					Premean = GetMeanFloat(pAlgorithmInfo->pSignalLog
					+ (int)(50.0f / pAlgorithmInfo->fPointLength), Max((int)(Len / 2), 1));
				}
                float Blimean = GetMeanFloat(pAlgorithmInfo->pSignalLog 
					+ Max(pEventsTable->EventsInfo[j].iEnd + 2 * iBlind,
					       pEventsTable->EventsInfo[j + 1].iBegin - Len), Max(Len, 1));
                int start = Max(Max(pEventsTable->EventsInfo[j].iEnd + iBlind, 
					             pEventsTable->EventsInfo[j + 1].iBegin - Len - 1), Len);
                for (k = 0; k < 2; ++k)
                {
                    float Thresh = 0.0f;
					Count = 0;
                    if (k == 0)
                    {
                        Thresh = 0.92f*(Premean - Blimean) + Blimean;

                    }
                    else
                    {
                        Thresh = 0.05f*(Premean - Blimean) + Blimean;
                    }
                    for (i = start; i > Max(pEventsTable->EventsInfo[j].iEnd + iBlind, 0); --i)
                    {
                        if (pAlgorithmInfo->pSignalLog[i] > Thresh)
                        {
                            Count++;
                        }
                        else
                        {
                            Count = 0;
                        }
                        if (Count >= 3)
                        {
                            if (k == 0)
                                iBeginAdd = Max(i + Count - (int)fSampleRating, 0);
                            else
                                iEndAdd = Max(i + Count - (int)fSampleRating, 0);
                            break;
                        }
                    }
                }
				iEndAdd = Min(Max(iEndAdd, iBeginAdd + iBlind), iBeginAdd + 2 * iBlind);

				if ((iBeginAdd > pEventsTable->EventsInfo[j].iEnd)
					&& (iBeginAdd < pEventsTable->EventsInfo[j + 1].iBegin)
					&& (iEndAdd < pEventsTable->EventsInfo[j + 1].iBegin)
					&& (iBeginAdd * pAlgorithmInfo->fPointLength < pAlgorithmInfo->solaEffectiveRange))
				{
					iStyleAdd = 0;
					pEventsTable->iEventsNumber = pEventsTable->iEventsNumber + 1;
					int jj = 0;
					for (jj = pEventsTable->iEventsNumber - 2; jj > j ; --jj)
					{
						pEventsTable->EventsInfo[jj + 1] = pEventsTable->EventsInfo[jj];
					}
					pEventsTable->EventsInfo[j + 1].iBegin = iBeginAdd;
					pEventsTable->EventsInfo[j + 1].iEnd = iEndAdd;
					pEventsTable->EventsInfo[j + 1].iStyle = iStyleAdd;
				}
				
			}
		}
	}
	
	for (j = 0; j < pEventsTable->iEventsNumber - 1; ++j)
	{
		if ((pEventsTable->EventsInfo[j + 1].iBegin < pEventsTable->EventsInfo[j].iEnd)
			&& (pEventsTable->EventsInfo[j + 1].iBegin >= 1))
		{
			pEventsTable->EventsInfo[j].iEnd = pEventsTable->EventsInfo[j + 1].iBegin - 1;
		}
	}

	TransEventsTable(pEventsTable, g_pEventsRegionTable);
	GetEventsParameter(g_pSignalLog, pAlgorithmInfo->iSignalLength,
		g_pEventsRegionTable, pEventsTable, fSampleRating);

	if ((pAlgorithmInfo->PulseWidth >= ENUM_PULSE_2US)
		&& (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_20US))
	{
		for (j = 1; j < pEventsTable->iEventsNumber; ++j)
    	{
			if (pEventsTable->EventsInfo[j].fReflect > -50)
			{
				pEventsTable->EventsInfo[j].iStyle = 2;
			}
		}
	}

	if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_100NS)
	{
		for (j = 1; j < pEventsTable->iEventsNumber; j++)
    	{
        
			if ((pEventsTable->EventsInfo[j].fReflect > -35.0f)
				&& (pEventsTable->EventsInfo[j].iStyle == 1))
			{
				pEventsTable->EventsInfo[j].iStyle = 2;
			}
		}
	}

    int spliter = ((!pAlgorithmInfo->spliterRatioGrade1)
        && (!pAlgorithmInfo->spliterRatioGrade2)
        && (!pAlgorithmInfo->spliterRatioGrade3));

    float distance = 0.0f;
    if (frontParament.pulseWidth == ENUM_PULSE_20NS)
    {
        distance = 300.0f;
    }
    else if (frontParament.pulseWidth == ENUM_PULSE_200NS)
    {
        distance = 500.0f;
    }

	ALG_DEBUG("----distance = %f---spliter = %d---iEventsNumber = %d----\n",
						distance, spliter, pEventsTable->iEventsNumber);

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetEventsTable1-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetEventsTable1-----------------------\n\n");
		PrintEventInfo(test_file_name, pEventsTable);
	#else
		PrintEventInfo(stdout, pEventsTable);
	#endif
	//PrintEventInfo(stdout, pEventsTable);
	ALG_DEBUG("\n\n\n");
#endif

    // 删除分路器造成的误判事件
    if (!spliter)
    {
        for (i = 1; i < pEventsTable->iEventsNumber - 1; ++i)
        {
            if ((pEventsTable->EventsInfo[i].iStyle == 2)
				&& (pEventsTable->EventsInfo[i].fReflect > -45.0f)
                && (pEventsTable->EventsInfo[i].fAttenuation > 1.0f)
                && (frontParament.pulseWidth <= ENUM_PULSE_200NS))
            {
                for (j = i + 1; j < pEventsTable->iEventsNumber; ++j)
                {
                    int delFlag = 0;
                    if ((pEventsTable->EventsInfo[j].iStyle == 0)
						&& ((pEventsTable->EventsInfo[j].fLoss < 0.20f)
                        || (pEventsTable->EventsInfo[j].fAttenuation > 1.3f))
                        && ((pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[i].iBegin
                        < (int)(distance / g_fLengthPerPoint))))
                    {
                        delFlag = 1;
						
						#if ISSERIAL == 1
							fprintf(test_file_name, "--------------i = %d-----------\n", i);
						#else
							ALG_DEBUG("--------------i = %d-----------\n", i);
						#endif
						
                    }
                    if (delFlag == 1)
                    {
                        int k = 0;
                        for (k = j; k < pEventsTable->iEventsNumber; ++k)
                        {
                            pEventsTable->EventsInfo[k] = pEventsTable->EventsInfo[k + 1];
                            pEventsTable->iEventsNumber--;
                        }
                    }
                }

            }
        }
    }
	
	#if ISSERIAL == 1
		fprintf(test_file_name, "----1---iEventsNumber = %d---------\n", pEventsTable->iEventsNumber);
	#else
		ALG_DEBUG("----1---iEventsNumber = %d---------\n", pEventsTable->iEventsNumber);
	#endif
		
    //迭代删除不符合条件的事件
    for (i = 0; i < 5; ++i)
    {
    	#if ISSERIAL == 1
			fprintf(test_file_name, "----1.1---i = %d---------\n", pEventsTable->iEventsNumber);
		#else
			ALG_DEBUG("----1---iEventsNumber = %d---------\n", pEventsTable->iEventsNumber);
		#endif
		
        iSaveNumber = 1;
		//对于非反射事件，使用损耗阈值，且阈值随信噪比的增加而增加；对于反射事件，则使用反射阈值
        for (j = 1; j < pEventsTable->iEventsNumber; ++j)
        {
            // 删除均值小于6db的事件
            float fMean = 0.0f;
            float fMax = 0.0f;
            if (pEventsTable->EventsInfo[j].iEnd <= pEventsTable->EventsInfo[j].iBegin)
            {
                continue;
            }
            else
            {
                fMean = GetMeanFloat(pAlgorithmInfo->pSignalLog + pEventsTable->EventsInfo[j].iBegin,
                    pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin + 1);
                fMax = GetMaxFloat(pAlgorithmInfo->pSignalLog + pEventsTable->EventsInfo[j].iBegin,
                    pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin + 1);
            }

            // 损耗阈值
            fLimitLoss = Min(0.02f, pAlgorithmInfo->SetParameters.fLossThreshold);
            
			float noisestd = 0.0f;
			float noisemean = 0.0f;
			float noisemax = 0.0f;
			float curmean = 0.0f;
			if (pEventsTable->EventsInfo[j].iBegin > 2 * iBlind)
			{
				int calLen = Min(50, iBlind);
				noisestd = GetStdFloat(g_pSignalLog 
					+ pEventsTable->EventsInfo[j].iBegin - iBlind - 10, calLen); // 100+100出现问题
				noisemean = GetMeanFloat(g_pSignalLog
					+ pEventsTable->EventsInfo[j].iBegin - iBlind - 10, calLen);
				noisemax = GetMaxFloat(g_pSignalLog
					+ pEventsTable->EventsInfo[j].iBegin - iBlind - 10, calLen);
				curmean = GetMeanFloat(g_pSignalLog
					+ pEventsTable->EventsInfo[j].iBegin + iBlind, calLen)
					+ normalAttenuation[frontParament.wave][0] * (iBlind + calLen / 2) * g_fLengthPerPoint / 1000;
			}	
			fLimitLoss = Max(fLimitLoss, noisestd);
			fLimitLoss = Min(fLimitLoss, 0.25f);

            if (j < pEventsTable->iEventsNumber - 1)
        	{
        		fMaxEventValue = GetMaxFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin,
					pEventsTable->EventsInfo[j + 1].iBegin - pEventsTable->EventsInfo[j].iBegin);
        	}
			else
			{
				fMaxEventValue = GetMaxFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin,
					Max(pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin,
						Min(pAlgorithmInfo->iSignalLength - pEventsTable->EventsInfo[j].iBegin, 256)));
			}
			float fMaxEventValue2 = GetMaxFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin - iBlind,
				pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin + iBlind + 1);
            float deltaTemp = g_pSignalLog[pEventsTable->EventsInfo[j].iBegin]
				- g_pSignalLog[pEventsTable->EventsInfo[j].iEnd];

			// 删除部分误判 1.反射率小于-50且fDelta < 1的反射事件；
            // 2.由于拖尾的影响，反射事件后2个盲区内的损耗增益事件；
            // 3.反射事件前一个盲区内的小损耗增益事件

			if ((frontParament.pulseWidth == ENUM_PULSE_5NS)
				&& (pEventsTable->EventsInfo[j].iStyle == 0)
				&& (pEventsTable->EventsInfo[j].fLoss < 1.0f))
			{
				printf("fLoss = %f\n", pEventsTable->EventsInfo[j].fLoss);
				#if ISSERIAL == 1
				fprintf(test_file_name, "-------if 00--------j = %d-----------\n", j);
				#else
				ALG_DEBUG("-------if 00-------j = %d-----------\n", j);
				#endif

				continue;
			}

            if ((j < pEventsTable->iEventsNumber - 1)
				&& (pEventsTable->EventsInfo[j].iStyle == 0)
				&& (pEventsTable->EventsInfo[j].fLoss < fLimitLoss))
            {
            	printf("fLoss = %f\n", pEventsTable->EventsInfo[j].fLoss);
            	#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 00--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 00-------j = %d-----------\n", j);
				#endif
				
            	continue;
            }
			
			if ((j > 0) && (j < pEventsTable->iEventsNumber - 1)
				&& (pEventsTable->EventsInfo[j].iStyle == 0)
				&& ((pEventsTable->EventsInfo[j].fLoss >= fLimitLoss)
				&& (pEventsTable->EventsInfo[j].fLoss < 0.10f))) 
            {
            	if ((fLimitLoss <= 0.02f) && ((pEventsTable->EventsInfo[j].fLoss > FLOSS_ERROR_MAX)
				|| (pEventsTable->EventsInfo[j].fLoss < FLOSS_ERROR_MIN)))
            	{
					printf("Good SNR!");
            	}
				else
				{
	            	printf("*****1**********");
	            	//printf("fLoss = %f\n", pEventsTable->EventsInfo[j].fLoss);
	            	#if ISSERIAL == 1
						fprintf(test_file_name, "-------if 0--------j = %d-----------\n", j);
					#else
						ALG_DEBUG("-------if 0-------j = %d-----------\n", j);
					#endif
					
	            	continue;
				}
            }

			if ((j > 0) && (j < pEventsTable->iEventsNumber - 1)
				&& (pEventsTable->EventsInfo[j].iStyle == 1)
				&& ((fabs(pEventsTable->EventsInfo[j].fLoss) >= fLimitLoss)
				&& (pEventsTable->EventsInfo[j].fLoss > -0.1f))) 
            {

            	printf("*****1111**********");
            	//printf("fLoss = %f\n", pEventsTable->EventsInfo[j].fLoss);
            	#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 0--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 0-------j = %d-----------\n", j);
				#endif
				
            	continue;
            }


			if ((pEventsTable->EventsInfo[j].fLoss > FLOSS_ERROR_MAX)
				|| (pEventsTable->EventsInfo[j].fLoss < FLOSS_ERROR_MIN))
			{
				pEventsTable->EventsInfo[j].fLoss = 0.0f;
			}

			if ((j < pEventsTable->iEventsNumber - 1)
				&& (pEventsTable->EventsInfo[j].iStyle == 0)
				&& (pEventsTable->EventsInfo[j].fLoss > fLimitLoss)
				&& (noisemean - curmean < noisestd * 0.8f))
            {
            	if (pEventsTable->EventsInfo[j].fLoss > 0.1f)
        		{
        			int jj = 0;
					if ((pEventsTable->EventsInfo[j].iStyle == 0)
						&& (frontParament.pulseWidth > ENUM_PULSE_1US)
			            && (pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin > 2 * 64))
			        {
			            int tmpBegin = pEventsTable->EventsInfo[j].iBegin;
						int tmpEnd = Max(pEventsTable->EventsInfo[j].iEnd - 64, tmpBegin);
		                for (jj = tmpBegin; jj < tmpEnd; ++jj)
		                {
		                	float standardAttenuation[] = { 0.33f, 0.18f, 0.2f };
		                    jj = Max(Max(jj, 2), iBlind);
		                    float delta = g_pSignalLog[jj + 2] - g_pSignalLog[jj];
							float stdNoise = GetStdFloat(g_pSignalLog + jj + iBlind, iBlind);
		                    float maxDelta = GetMaxFloat(g_pSignalLog + jj + iBlind, iBlind)
		                        - GetMinFloat(g_pSignalLog + jj + iBlind, iBlind)
		                        - iBlind * g_fLengthPerPoint / 1000.0f
								* standardAttenuation[frontParament.wave];
		                    if (((delta > 0.2f) && (delta < -0.2f))
								&& (maxDelta > Max(0.2f, stdNoise)))
		                    {
		                        pEventsTable->EventsInfo[j].iBegin = jj;
								break;
		                    }
		                }
		            }
            
        		}
				else
				{
	            	printf("*****2**********\n");
	            	printf("noisemean = %f, curmean = %f\n", noisemean, curmean);
					#if ISSERIAL == 1
									fprintf(test_file_name, "-------if 0--------j = %d-----------\n", j);
					#else
									ALG_DEBUG("-------if 0-------j = %d-----------\n", j);
					#endif
								
				    continue;
				}
            }

			int k = 0;
			int z = 0;
			int continueflag = 0;
			//删除反射事件下降沿的误报
			
			if ((j > 0) && (pEventsTable->EventsInfo[j].iStyle <= 2)
				&& (((pEventsTable->EventsInfo[j].fReflect < -65.0f)
					&& (frontParament.pulseWidth > ENUM_PULSE_10NS)) 
				|| ((pEventsTable->EventsInfo[j].fReflect < -70.0f)
					&& (frontParament.pulseWidth <= ENUM_PULSE_10NS))))
			{
				for (z = 0; z < 30; z++)
				{
					if ((iEndpointindex[z][0] <= pEventsTable->EventsInfo[j].iBegin) 
						&& (pEventsTable->EventsInfo[j].iBegin <= iEndpointindex[z][1]))
					{
						for (k = j - 1; k >= 0; --k)
						{
							if ((pEventsTable->EventsInfo[k].iStyle == 2)
								&& (pEventsTable->EventsInfo[k].fReflect > -65)
								&& ((pEventsTable->EventsInfo[k].fAttenuation > 12) || (k == 0))
								&& (iEndpointindex[z][0] <= pEventsTable->EventsInfo[k].iBegin))
							{
								if ((pEventsTable->EventsInfo[k].fAttenuation < Max_fAttenuation)
									&& (pEventsTable->EventsInfo[k].fLoss > 0.5f))
								{
									continue;
								}
								else
								{
									continueflag = 1;
									break;
								}
							}
							if (pEventsTable->EventsInfo[k].iBegin < iEndpointindex[z][0])
							{
								break;
							}
						}
					}
					if ((continueflag == 1)
						|| (iEndpointindex[z][0] > pEventsTable->EventsInfo[j].iBegin))
					{
						break;
					}
				}
				if (continueflag)
				{
					continue;
				}
			}
			//删除末段事件后面近距离的误报
			if ((j > 0) && ((pEventsTable->EventsInfo[j].fAttenuation < 0.5f)
				|| (pEventsTable->EventsInfo[j].fAttenuation > 8.0f))
				&& (pEventsTable->EventsInfo[j].iStyle <= 1)
				&& (pEventsTable->EventsInfo[j].fReflect < -60.0f))
			{
				int templength = (int)Max(5.5f * pAlgorithmInfo->iBlind, 265 / g_fLengthPerPoint);
				for (k = j - 1; k > 0; --k)
				{
					if(pEventsTable->EventsInfo[k].fLoss > 2.0f)
					{
						templength = templength * 2;
					}
					else
					{
						templength = (int)Max(5.5f * pAlgorithmInfo->iBlind, 265 / g_fLengthPerPoint);
					}
					if (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[k].iBegin < templength)
					{
						if ((pEventsTable->EventsInfo[k].iStyle == 2)
							&& (pEventsTable->EventsInfo[k].fLoss > 3.0f)
							&& (pEventsTable->EventsInfo[k].fReflect > -31.0f))
						{
							continueflag = 1;
						}
					}
					else
						break;
				}
				if (continueflag)
				{

					#if ISSERIAL == 1
					    fprintf(test_file_name, "-------if 1--------j = %d-----------\n", j);
				    #else
					    ALG_DEBUG("-------if 1-------j = %d-----------\n", j);
				    #endif
					continue;
				}
			}

			// 小反射事件的删除，主要是淹没在噪声区里的反射事件
			if ((j < pEventsTable->iEventsNumber - 1)
				&& (pEventsTable->EventsInfo[j].iStyle == 2)
				&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd > 10 * iBlind)
				&& (fMaxEventValue < noisemean + 2.0f * noisestd)
				&& (fMaxEventValue > noisemean + 1.0f)
				&& (noisestd < 3.0f)
				&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_10US))

			{
				if ((pEventsTable->EventsInfo[j].fLoss > 0.2f)
					&& (pEventsTable->EventsInfo[j].fReflect < -55.0f)
					&& (pAlgorithmInfo->PulseWidth > ENUM_PULSE_10NS))
				{
					pEventsTable->EventsInfo[j].iStyle = 0;
				}
				else
				{
					printf("*****5**********\n");
					#if ISSERIAL == 1
						fprintf(test_file_name, "-------if 12--------j = %d-----------\n", j);
					#else
						ALG_DEBUG("-------if 12-------j = %d-----------\n", j);
					#endif
	            	continue;
				}
			}
			
			if ((j < pEventsTable->iEventsNumber - 1)
				&& (pEventsTable->EventsInfo[j].iStyle == 2)
				&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_10US)
				&& (fMaxEventValue > noisemean + 0.5f))
			{
				int OutMeanLen = 0;
				int tmpkB = pEventsTable->EventsInfo[j].iBegin;
				int tmpkE = Max(pEventsTable->EventsInfo[j].iEnd,
					pEventsTable->EventsInfo[j].iBegin + 2 * iBlind);
				for (k = tmpkB; k < tmpkE; ++k)
				{
					if (g_pSignalLog[k] > noisemean)
					{
						OutMeanLen++;
					}
					else
					{
						OutMeanLen = 0;
					}
					if (OutMeanLen > iBlind / 2)
					{
						break;
					}
				}
				if (OutMeanLen < iBlind / 2)
				{
					if ((pEventsTable->EventsInfo[j].fLoss > 0.2f)
						&& (pEventsTable->EventsInfo[j].fReflect < -55.0f)
						&& (pAlgorithmInfo->PulseWidth > ENUM_PULSE_10NS))
					{
						pEventsTable->EventsInfo[j].iStyle = 0;
					}
					else
					{
						printf("*****6**********\n");
						continue;
					}
					
				}        	
			}

			if ((j < pEventsTable->iEventsNumber - 1)
				&& (((pEventsTable->EventsInfo[j].iStyle == 0)
				&& (pEventsTable->EventsInfo[j].fDelta < 0.8f)
                && (pEventsTable->EventsInfo[j].fLoss < fLimitLoss))
                || (pEventsTable->EventsInfo[j].iStyle == 1))
                && ((pEventsTable->EventsInfo[j + 1].iStyle == 2)
                && (pEventsTable->EventsInfo[j + 1].fReflect
					> frontParament.fReflectThreshold + 20.0f))
                && (pEventsTable->EventsInfo[j + 1].iBegin - pEventsTable->EventsInfo[j].iEnd
					< Max(pAlgorithmInfo->iBlind / 2, 2 * iPointNum)))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 2--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 2-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			
			if ((fMaxEventValue2 < fEndThr)
				&& (pEventsTable->EventsInfo[j].iStyle <= 3))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 3--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 3-------j = %d-----------\n", j);
				#endif
            	continue;
			}
			
			if ((pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd
				<= pAlgorithmInfo->iBlind / 2)
                && (pEventsTable->EventsInfo[j].iStyle == 2)
                && (pEventsTable->EventsInfo[j - 1].iStyle == 2)
                && (pEventsTable->EventsInfo[j].fReflect < -60.0f)
                && (pEventsTable->EventsInfo[j].fDelta < 1.0f)
                && (pEventsTable->EventsInfo[j - 1].fReflect > pEventsTable->EventsInfo[j].fReflect))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 4--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 4-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			if ((j < pEventsTable->iEventsNumber - 1)
				&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_10US)
				&& (3 * noisestd + noisemax > fMaxEventValue)       // added by wyl 2019/07/30
                && (pEventsTable->EventsInfo[j].iStyle == 2)
                && ((pEventsTable->EventsInfo[j - 1].iStyle != 2)
                || ((pEventsTable->EventsInfo[j - 1].iStyle == 2)
                && (pEventsTable->EventsInfo[j].fDelta < 1.0f)
                && (pEventsTable->EventsInfo[j].iBegin 
					- pEventsTable->EventsInfo[j - 1].iEnd > Max(50, 10 * iBlind)))))
			{
				if ((pEventsTable->EventsInfo[j].fLoss > 0.2f)
					&& (pEventsTable->EventsInfo[j].fReflect < -55.0f)
					&& (pAlgorithmInfo->PulseWidth > ENUM_PULSE_10NS))
				{
					pEventsTable->EventsInfo[j].iStyle = 0;
				}
				else
				{
					printf("*****11**********\n");
					#if ISSERIAL == 1
						fprintf(test_file_name, "-------if 6--------j = %d-----------\n", j);
					#else
						ALG_DEBUG("-------if 6-------j = %d-----------\n", j);
					#endif
	            	continue;
				}
			}
				
			if ((fMean < pAlgorithmInfo->fMaxInputNoise + 1.0f)
				&& (fMax < pAlgorithmInfo->fMaxInputNoise + 2.0f)
				&& (pEventsTable->EventsInfo[j].iStyle > 0))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 7--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 7-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			if (!spliter)
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 8--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 8-------j = %d-----------\n", j);
				#endif
            	continue;
			}
			if ((pEventsTable->EventsInfo[j].iStyle == 0)
				&& (frontParament.pulseWidth >= ENUM_PULSE_200NS)
                && (pEventsTable->EventsInfo[j - 1].fAttenuation > 1.0f)
				&& (deltaTemp < 0.1f)
                && (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin 
					< (int)(200.0f / g_fLengthPerPoint)))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 9--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 9-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			if ((j >= 1) && (pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j - 1].fReflect > -45.0f)
                && (pEventsTable->EventsInfo[j - 1].fLoss > 2.0f)
                && (pEventsTable->EventsInfo[j].fReflect < -50.0f)
                && (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin
                 < (int)(200.0f / g_fLengthPerPoint))
                && (pEventsTable->EventsInfo[j].iStyle == 2))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 10--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 10-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			if ((j > 1) && (pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j - 1].fReflect > -45.0f)
                && (pEventsTable->EventsInfo[j - 1].fAttenuation > 1.0f)
                && (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin
                 < (int)(200.0f / g_fLengthPerPoint))
                && (pEventsTable->EventsInfo[j].iStyle == 0)
                && (pEventsTable->EventsInfo[j].fLoss < 0.15f))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 11--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 11-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			if ((j >= 1) && (pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j - 1].fReflect > -20.0f)
                && (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin < 6 * iBlind)
                && (pEventsTable->EventsInfo[j].iStyle <= 2)
                && (pEventsTable->EventsInfo[j - 1].fLoss > 6.0f)
                && (pEventsTable->EventsInfo[j].fReflect < -35.0f || fLimitLoss >= 0.25f)
                && (pAlgorithmInfo->PulseWidth > ENUM_PULSE_2US)
                && (fMaxEventValue < 14.0f))
			{
				printf("*****17**********\n");
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 12--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 12-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			if ((pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (((pEventsTable->EventsInfo[j].fReflect < -60.0f)
                && (pEventsTable->EventsInfo[j].iStyle == 2))
                || ((pEventsTable->EventsInfo[j].fLoss > 1.0f)
                && (pEventsTable->EventsInfo[j].fLoss < 3.0f)
                && (pEventsTable->EventsInfo[j].iStyle == 0)
				&& (noisemean - curmean <= 1.0f)))
                && (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_50NS)
                && (fLimitLoss >= 0.25f)
                && ((pEventsTable->EventsInfo[j - 1].fContinueAttenuation
				> pEventsTable->EventsInfo[j - 1].iContinueLength * 0.4f
					* pAlgorithmInfo->fPointLength / 1000.0f)
                || (pEventsTable->EventsInfo[j - 1].fContinueAttenuation
					< pEventsTable->EventsInfo[j - 1].iContinueLength * 0.1f
					* pAlgorithmInfo->fPointLength / 1000.0f)))
			{
				if (((fMax - g_pSignalLog[pEventsTable->EventsInfo[j].iBegin]) > 2.2f)
					&& (fMax > 4.5f) && (pEventsTable->EventsInfo[j].iStyle == 2)
					&& (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_10NS))
				{
					;
				}
				else
				{
					printf("*****18**********\n");
					#if ISSERIAL == 1
						fprintf(test_file_name, "-------if 13--------j = %d-----------\n", j);
					#else
						ALG_DEBUG("-------if 13-------j = %d-----------\n", j);
					#endif
	            	continue;
				}
			}

			if ((pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j - 1].iEnd - pEventsTable->EventsInfo[j - 1].iBegin
				 > pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd)
				&& (pEventsTable->EventsInfo[j - 1].iEnd - pEventsTable->EventsInfo[j - 1].iBegin > 256)
				&& (pEventsTable->EventsInfo[j].iStyle <= 2)
                && (pEventsTable->EventsInfo[j - 1].fLoss > 1.0f)
                && (pAlgorithmInfo->iFiberRange < 500.0f)
                && (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_100NS))
			{
				if ((fMax - g_pSignalLog[pEventsTable->EventsInfo[j].iBegin] > 3.0f)
					&& (fMax > 8.0f) && (pEventsTable->EventsInfo[j].iStyle == 2))
				{
					;
				}
				else
				{
					printf("*****19**********\n");
					#if ISSERIAL == 1
						fprintf(test_file_name, "-------if 14--------j = %d-----------\n", j);
					#else
						ALG_DEBUG("-------if 14-------j = %d-----------\n", j);
					#endif
	            	continue;
				}
			}

			if ((j == 1) 
				&& (((pAlgorithmInfo->PulseWidth < ENUM_PULSE_10NS)
					&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < 55 * iBlind))
				|| ((pAlgorithmInfo->PulseWidth == ENUM_PULSE_10NS)
					&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < 20 * iBlind)))
				&& (pEventsTable->EventsInfo[j].iStyle == 0)
                && (pEventsTable->EventsInfo[j - 1].fReflect > -45.0f)
                && (pAlgorithmInfo->iFiberRange < 500.0f))
			{
				printf("*****20**********\n");
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 15--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 15-------j = %d-----------\n", j);
				#endif
            	continue;
			}
			// 去除噪声区的小事件
			if ((pAlgorithmInfo->PulseWidth < ENUM_PULSE_200NS) && (j > 0)
				&& (pEventsTable->EventsInfo[j].iStyle < 2)
				&& (g_pSignalLog[pEventsTable->EventsInfo[j].iBegin] < 4.0f)
				&& (GetMinFloat(g_pSignalLog + pEventsTable->EventsInfo[j - 1].iEnd,
					Max(pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd, 0)) < 1.0f)
				&& (pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j - 1].fReflect > -35.0f))
			{
				printf("*****21**********\n");
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 16--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 16-------j = %d-----------\n", j);
				#endif
            	continue;
			}
			// 处理因鬼影导致的range距离不对，从而导致检测错误
			if ((j > 0) && (pAlgorithmInfo->PulseWidth > ENUM_PULSE_10NS)
				&& (pEventsTable->EventsInfo[j].iBegin * pAlgorithmInfo->fPointLength
				> pAlgorithmInfo->solaEffectiveRange / 2) 
				&& (pEventsTable->EventsInfo[j].iStyle == 0) 
				&& (pEventsTable->EventsInfo[j - 1].fAttenuation > 5.0f)
				&& (pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j - 1].fReflect > -40.0f))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 17--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 17-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			// 处理大脉宽下误判的衰减事件
			if ((pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US) 
				&& (pEventsTable->EventsInfo[j].iStyle == 0)
				&& (pEventsTable->EventsInfo[j].fLoss < 0.1f))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 18--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 18-------j = %d-----------\n", j);
				#endif
            	continue;
			}

			if ((j > 0) && (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US)
				&& (pEventsTable->EventsInfo[j].iStyle == 2)
				&& (pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j].fReflect < frontParament.fRelay + 0.5f)
				&& (pEventsTable->EventsInfo[j - 1].fReflect > -30.0f)
				&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < iBlind / 8)
				&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin < 2 * iBlind)
				&& (fabsf(pEventsTable->EventsInfo[j - 1].fAttenuation) > 1.0f))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 18--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 18.5-------j = %d-----------\n", j);
				#endif
				continue;
			}

			if ((j > 0) && (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_10US)
				&& (pEventsTable->EventsInfo[j].iStyle < 2)
				&& (pEventsTable->EventsInfo[j - 1].iStyle == 2)
				&& (pEventsTable->EventsInfo[j].fReflect < frontParament.fRelay)
				&& (pEventsTable->EventsInfo[j - 1].fReflect > -30.0f)
				&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < iBlind / 8)
				&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin < 1.6 * iBlind))
			{
				#if ISSERIAL == 1
					fprintf(test_file_name, "-------if 18--------j = %d-----------\n", j);
				#else
					ALG_DEBUG("-------if 18.6-------j = %d-----------\n", j);
				#endif
				continue;
			}

			// 处理大脉宽下误判的衰减事件
			if ((j > 0) && (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_1US)
				&& (pEventsTable->EventsInfo[j].iStyle == 0)
				&& (pEventsTable->EventsInfo[j].iBegin < pEventsTable->EventsInfo[j - 1].iEnd)
				&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin < 2 * iBlind)
				&& (pEventsTable->EventsInfo[j - 1].iStyle == 2))
			{
				#if ISSERIAL == 1
								fprintf(test_file_name, "-------if 19--------j = %d-----------\n", j);
				#else
								ALG_DEBUG("-------if 19-------j = %d-----------\n", j);
				#endif
				continue;
			}

			SaveEventLoss[iSaveNumber] = fLimitLoss;
			pSaveIndex[iSaveNumber] = j;
            iSaveNumber++;
			
        }

        for (j = 0; j < iSaveNumber; ++j)
        {
            pEventsTable->EventsInfo[j] = pEventsTable->EventsInfo[pSaveIndex[j]];
        }

		if (pEventsTable->iEventsNumber == iSaveNumber)
		{
			break;
		}
        pEventsTable->iEventsNumber = iSaveNumber;

		#if ISSERIAL == 1
			fprintf(test_file_name, "----3---iEventsNumber = %d---------\n", pEventsTable->iEventsNumber);
		#else
			ALG_DEBUG("----3---iEventsNumber = %d---------\n", pEventsTable->iEventsNumber);
		#endif
		
        //重新计算各个参数
        TransEventsTable(pEventsTable, g_pEventsRegionTable);
        GetEventsParameter(g_pSignalLog, pAlgorithmInfo->iSignalLength,
			g_pEventsRegionTable, pEventsTable, fSampleRating);
    }

    //重新计算各个参数
    TransEventsTable(pEventsTable, g_pEventsRegionTable);
    GetEventsParameter(g_pSignalLog, pAlgorithmInfo->iSignalLength,
		g_pEventsRegionTable, pEventsTable, fSampleRating);
		
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetEventsTable2-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetEventsTable2-----------------------\n\n");
		PrintEventInfo(test_file_name, pEventsTable);
	#else
		PrintEventInfo(stdout, pEventsTable);
	#endif
	//PrintEventInfo(stdout, pEventsTable);	ALG_DEBUG("\n\n\n");
#endif

	iSaveNumber = 1;
	pSaveIndex[0] = 0;
	int k = 0;
	int iDelFlag = 0;
	fLimitLoss = Max(pAlgorithmInfo->SetParameters.fLossThreshold + 0.02f, 0.05f);
	for (j = 1; j < pEventsTable->iEventsNumber; ++j)
	{
		iDelFlag = 0;
		for (k = 0; k < 19; ++k)
		{
			if ((int)pAlgorithmInfo->pCombineInfo[0][k] <= Max(iBlind, iPointNum / 2))
			{
				continue;
			}
			//组合点位于事件区间内部；
			if (((int)pAlgorithmInfo->pCombineInfo[0][k]
				>= pEventsTable->EventsInfo[j].iBegin)
				&& ((int)pAlgorithmInfo->pCombineInfo[0][k]
					<= pEventsTable->EventsInfo[j].iEnd + 3 * iBlind + iPointNum))
			{
				//根据删除组合点附近误判的衰减事件
				if ((pEventsTable->EventsInfo[j].iStyle == 0)
					&& (pEventsTable->EventsInfo[j].fLoss < 2 * fLimitLoss))
				{
					iDelFlag = 1;
					break;
				}
				//删除组合点附近误判的衰减事件(主要是大脉宽下的误判事件)
				if ((pEventsTable->EventsInfo[j].iStyle == 0)
					&& (pEventsTable->EventsInfo[j].fLoss < 0.06f)
					&& (pEventsTable->EventsInfo[j].fDelta < 0.15f))
				{
					iDelFlag = 1;
					break;
				}
				//删除组合点附近误判的增益事件
				if ((pEventsTable->EventsInfo[j].iStyle == 1)
					&& (pEventsTable->EventsInfo[j].fLoss > -2.0f * fLimitLoss)
					&& (pEventsTable->EventsInfo[j].fDelta < 0.5f)
					&& (pEventsTable->EventsInfo[j].fReflect < -42.0f)
					&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_10US))
				{
					iDelFlag = 1;
					break;
				}
				//删除组合点附近误判的反射事件
				if ((pEventsTable->EventsInfo[j].iStyle > 1)
					&& (pEventsTable->EventsInfo[j].fReflect < -65.0f)
					&& (pEventsTable->EventsInfo[j].fDelta < 0.5f)
					&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_10US))
				{
					iDelFlag = 1;
					break;
				}
			}
		}

		if (pEventsTable->EventsInfo[j].iStyle == 0 
			&& pEventsTable->EventsInfo[j].fLoss < SaveEventLoss[j])
		{
			iDelFlag = 1;
		}

		if ((j >= 1) && (j < pEventsTable->iEventsNumber - 1)
			&& (pEventsTable->EventsInfo[j].iStyle < 2)
			&& ((pEventsTable->EventsInfo[j - 1].iStyle == 2
				&& pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin < 10 * iBlind)
				|| (pEventsTable->EventsInfo[j - 1].iStyle == 0
					&& pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin < 4 * iBlind))
			&& ((pEventsTable->EventsInfo[j - 1].fContinueAttenuation
					> pEventsTable->EventsInfo[j - 1].iContinueLength * 0.5f
					* pAlgorithmInfo->fPointLength / 1000.0f)
			 || (pEventsTable->EventsInfo[j - 1].fContinueAttenuation
					< pEventsTable->EventsInfo[j - 1].iContinueLength * 0.1f
					* pAlgorithmInfo->fPointLength / 1000.0f)))
		{
			
			if (pEventsTable->EventsInfo[j].iStyle == 0
				&& pEventsTable->EventsInfo[j].iBegin
					- pEventsTable->EventsInfo[j - 1].iBegin > 2 * iBlind 
				&& pAlgorithmInfo->PulseWidth <= ENUM_PULSE_200NS
				&& pAlgorithmInfo->PulseWidth >= ENUM_PULSE_50NS
				&& SaveEventLoss[j] < 0.08f
				&& pEventsTable->EventsInfo[j].fLoss > 0.1f) 
			{
				float fContinueAttenuation2 = (GetMeanFloat(g_pSignalLog
					+ pEventsTable->EventsInfo[j].iBegin - 2 * iBlind, iBlind)
					- GetMeanFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin - iBlind,
						iBlind)) / pAlgorithmInfo->fPointLength * 1000.0f / (float)(iBlind);
				if (fContinueAttenuation2 > -5.5 && fContinueAttenuation2 < 2.5f)
				{
					;
				}
				else
				{
					iDelFlag = 1;
				}
						
			}
			else if (pEventsTable->EventsInfo[j - 1].iStyle == pEventsTable->EventsInfo[j].iStyle
				&& pEventsTable->EventsInfo[j - 1].fLoss < pEventsTable->EventsInfo[j].fLoss
				&& pEventsTable->EventsInfo[j].fLoss > 0.25f
				&& j - 1 == pSaveIndex[iSaveNumber - 1])
			{
				pEventsTable->EventsInfo[j - 1] = pEventsTable->EventsInfo[j];
				iDelFlag = 1;
			}
			else
			{
				iDelFlag = 1;
			}
		}
		if ((j >= 1) && (j < pEventsTable->iEventsNumber - 1) 
			&& (pEventsTable->EventsInfo[j].iStyle == 0)
			&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin > Max(4 * iBlind, 3 * iPointNum))
			&& (pEventsTable->EventsInfo[j - 1].fContinueAttenuation > pAlgorithmInfo->fPointLength / 1000.0f
				* 2 * pEventsTable->EventsInfo[j - 1].iContinueLength * 0.4f)
			&& (pEventsTable->EventsInfo[j].fLoss < 0.15f))
		{
			if (pEventsTable->EventsInfo[j].iStyle == 0
				&& pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin > 2 * iBlind
				&& pAlgorithmInfo->PulseWidth <= ENUM_PULSE_200NS
				&& pAlgorithmInfo->PulseWidth >= ENUM_PULSE_50NS
				&& SaveEventLoss[j] < 0.08f
				&& pEventsTable->EventsInfo[j].fLoss > 0.2f) 
			{
				float fContinueAttenuation2 = (GetMeanFloat(g_pSignalLog
					+ pEventsTable->EventsInfo[j].iBegin - 2 * iBlind, iBlind)
					- GetMeanFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin - iBlind,
						iBlind)) / pAlgorithmInfo->fPointLength * 1000.0f / (float)(iBlind);;
				if (fContinueAttenuation2 > -2.5f && fContinueAttenuation2 < 2.5f)
				{
					;
				}
				else
				{
					iDelFlag = 1;
				}
						
			}
			else
			{
				iDelFlag = 1;
			}
		}
		if ((j >= 1) && (j < pEventsTable->iEventsNumber - 1) 
			&& (pEventsTable->EventsInfo[j].iStyle == 2)
			&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iBegin > 4 * iBlind)
			&& (pEventsTable->EventsInfo[j - 1].fContinueAttenuation
				> 2 * pEventsTable->EventsInfo[j - 1].iContinueLength * 0.4f
				* pAlgorithmInfo->fPointLength / 1000.0f)
			&& (((pEventsTable->EventsInfo[j].fReflect < -58.0f)
				&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_2US)
				&& (pAlgorithmInfo->PulseWidth > ENUM_PULSE_5NS))
				|| ((pEventsTable->EventsInfo[j].fReflect < -70.0f)
					&& (pAlgorithmInfo->PulseWidth == ENUM_PULSE_5NS)))
			&& (SaveEventLoss[j] < 0.25f))
		{
			iDelFlag = 1;
		}
		// 删除E点前过冲导致多报一个衰减事件
		if ((j >= 1) && (j < pEventsTable->iEventsNumber - 1)
			&& (pEventsTable->EventsInfo[j + 1].iStyle == 2)
			&& (pEventsTable->EventsInfo[j].iStyle == 0)
			&& (pEventsTable->EventsInfo[j + 1].iBegin - pEventsTable->EventsInfo[j].iEnd < iBlind)
			&& (pEventsTable->EventsInfo[j + 1].fReflect > -40.0f)
			&& (frontParament.pulseWidth == ENUM_PULSE_50NS
				|| frontParament.pulseWidth == ENUM_PULSE_100NS)
			&& (g_pSignalLog[pEventsTable->EventsInfo[j + 1].iBegin - 10]
				- GetMinFloat(g_pSignalLog + Max(pEventsTable->EventsInfo[j + 1].iBegin - 10, 0), 10) > 0.1f))
		{
			iDelFlag = 1;
		}

		if ((pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[0].iEnd > 2 * iBlind)
			&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[0].iBegin < 20 * iBlind)
			&& (pEventsTable->EventsInfo[j - 1].fContinueAttenuation
			> pEventsTable->EventsInfo[j - 1].iContinueLength * 0.4f
				* pAlgorithmInfo->fPointLength / 1000.0f)
			&& (pEventsTable->EventsInfo[j].fAttenuation > 1.2f)
			&& (pAlgorithmInfo->PulseWidth > ENUM_PULSE_200NS)
			&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_10US))
		{
				iDelFlag = 1;
		}
		if ((pEventsTable->EventsInfo[j].iStyle == 2)
			&& (pEventsTable->EventsInfo[j].fReflect < -65.0f)
			&& (pEventsTable->EventsInfo[j].fLoss < 0.5f)
			&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_10US)
			&& (pAlgorithmInfo->PulseWidth > ENUM_PULSE_5NS))
		{
			iDelFlag = 1;
		}	
		if ((j >= 1) && (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pEventsTable->EventsInfo[j - 1].iStyle == 2) 
			&& (pEventsTable->EventsInfo[j].iBegin 
				- pEventsTable->EventsInfo[j - 1].iEnd < 15 * iBlind)
			&& (SaveEventLoss[j] > 0.1f))
		{
			iDelFlag = 1;
		}

		if ((j >= 1) && (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pEventsTable->EventsInfo[j - 1].iStyle == 2) 
			&& ((pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < 35 * iBlind
				&& pAlgorithmInfo->PulseWidth == ENUM_PULSE_10NS)
			|| (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < 45 * iBlind
				&& pAlgorithmInfo->PulseWidth == ENUM_PULSE_5NS))
			&& (SaveEventLoss[j] > 0.1f))
		{
			iDelFlag = 1;
		}
		if ((j >= 1) && (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pEventsTable->EventsInfo[j - 1].iStyle == 2) 
			&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd > 15 * iBlind)
			&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < 25 * iBlind)
			&& (pEventsTable->EventsInfo[j].fLoss < 0.2f)
			&& (SaveEventLoss[j] > 0.1f))
		{
			iDelFlag = 1;
		}
		if ((j > 0) && (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < 10 * iBlind)
			&& (pEventsTable->EventsInfo[j].fLoss < pEventsTable->EventsInfo[j - 1].fLoss)
			&& (SaveEventLoss[j] >= 0.25f))
		{
			iDelFlag = 1;
		}
		if ((j > 0) && (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pEventsTable->EventsInfo[j].iBegin - pEventsTable->EventsInfo[j - 1].iEnd < 30 * iBlind)
			&& ((SaveEventLoss[j] >= 0.25f)
				|| ((pEventsTable->EventsInfo[j - 1].fContinueAttenuation
					> 50.0f * pEventsTable->EventsInfo[j - 1].iContinueLength * 0.4f
					* pAlgorithmInfo->fPointLength / 1000.0f)
			&& (SaveEventLoss[j] > 0.05f)
					&& (pEventsTable->EventsInfo[j].fLoss > 14.0f * SaveEventLoss[j]))))
		{
			iDelFlag = 1;
		}
		if ((j > 0) && (j < pEventsTable->iEventsNumber - 1)
			&& (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pEventsTable->EventsInfo[j + 1].iBegin - pEventsTable->EventsInfo[j].iEnd < 10 * iBlind)
			&& (SaveEventLoss[j] >= 0.25)
			&& (pAlgorithmInfo->PulseWidth == ENUM_PULSE_50NS)
			&& (pEventsTable->EventsInfo[j + 1].fLoss > 4.0f))
		{
			iDelFlag = 1;
		}

		if ((j > 0) && (j < pEventsTable->iEventsNumber - 1)
			&& (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pAlgorithmInfo->PulseWidth == ENUM_PULSE_2US)
			&& (pAlgorithmInfo->solaEffectiveRange > 60000.0f)
			&& (((pEventsTable->EventsInfo[j].fReflect < -46.5f)
				&& (SaveEventLoss[j] >= 0.04f))
			|| (SaveEventLoss[j] >= 0.25f
				&& pEventsTable->EventsInfo[j].fReflect < -45.0f)
			|| (pEventsTable->EventsInfo[j].fLoss > 1.0f
				&& pEventsTable->EventsInfo[j].fReflect < -50.0f)))
		{
			fMaxEventValue = GetMaxFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin,
				pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin);
			if (fMaxEventValue < pAlgorithmInfo->fMaxInputNoise + 3.0f)
			{
				iDelFlag = 1;
			}		
		}

		if ((j > 0) && (j < pEventsTable->iEventsNumber - 1) 
			&& (pEventsTable->EventsInfo[j].iStyle == 0) 
			&& (pEventsTable->EventsInfo[j + 1].iBegin - pEventsTable->EventsInfo[j].iBegin < 4 * iBlind)
			&& (pEventsTable->EventsInfo[j + 1].iStyle == 2)
			&& pEventsTable->EventsInfo[j].fLoss < 0.2f)
		{
			iDelFlag = 1;
		}
		if (pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin < 1)
		{
			iDelFlag = 1;
		}

		// 信噪比不好的区域误判
		if ((iDelFlag == 0) && (pEventsTable->EventsInfo[j].iStyle == 2)
			&& (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_2US)
			&& (pEventsTable->EventsInfo[j].fReflect < -65.0f) && (j > 0))
		{
			float fMeanBeforeEvent = GetMeanFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin - 40, 20);
			float fMaxEvent = GetMaxFloat(g_pSignalLog + pEventsTable->EventsInfo[j].iBegin,
				pEventsTable->EventsInfo[j].iEnd - pEventsTable->EventsInfo[j].iBegin);
			if ((fMaxEvent - fMeanBeforeEvent < 0.5f) && (fMaxEvent < pAlgorithmInfo->fMaxInputNoise + 3.0f))
			{
				iDelFlag = 1;
			}
		}
			
		if (iDelFlag == 0)
		{
			pSaveIndex[iSaveNumber] = j;
			iSaveNumber++;
		}
	}
	
	for (j = 0; j < iSaveNumber; ++j)
	{
		pEventsTable->EventsInfo[j] = pEventsTable->EventsInfo[pSaveIndex[j]];
	}
	pEventsTable->iEventsNumber = iSaveNumber;
	
	
	if (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_2US
		&& pAlgorithmInfo->PulseWidth <= ENUM_PULSE_20US)
	{
		for (j = 1; j < pEventsTable->iEventsNumber; ++j)
    	{
        
			if (pEventsTable->EventsInfo[j].fReflect > -50.0f)
			{
				pEventsTable->EventsInfo[j].iStyle = 2;
			}
			int Count = 0;
	        int EndFlag = 0;
	        if (pEventsTable->EventsInfo[j].iBegin > iBlind
				&& pEventsTable->EventsInfo[j].iEnd < pAlgorithmInfo->iSignalLength - iBlind)
	        {
	            int SearchLen = iBlind;
				if (j < pEventsTable->iEventsNumber - 1)
				{
					SearchLen = Min(SearchLen, 2 * (pEventsTable->EventsInfo[j + 1].iBegin
						- pEventsTable->EventsInfo[j].iEnd));
				}
	            int CurMaxPos = GetMaxIndex(pAlgorithmInfo->pSignalLog
	                + pEventsTable->EventsInfo[j].iBegin, SearchLen)
					+ pEventsTable->EventsInfo[j].iBegin;
				int SearchLen2 = Max(CurMaxPos - pEventsTable->EventsInfo[j].iBegin, 1);
				int CurMinPos = GetMinIndex(pAlgorithmInfo->pSignalLog
	                + pEventsTable->EventsInfo[j].iBegin, SearchLen2)
	                + pEventsTable->EventsInfo[j].iBegin;
				float freNoisestd = GetStdFloat(pAlgorithmInfo->pSignalLog + CurMinPos - 10, 10);

				float CurMaxValue = pAlgorithmInfo->pSignalLog[CurMaxPos];
				float CurMinValue = GetMinFloat(pAlgorithmInfo->pSignalLog
	                + pEventsTable->EventsInfo[j].iBegin, SearchLen2);
				if (CurMaxValue - CurMinValue > 0.3f)
				{
					pEventsTable->EventsInfo[j].iStyle = 2;
				}
				// 增加大脉宽起始点的再判断
				EndFlag = 0;
				Count = 0;
				if (CurMinPos < CurMaxPos && pEventsTable->EventsInfo[j].iStyle == 2)
				{
					pEventsTable->EventsInfo[j].iBegin = CurMinPos;
					for (i = CurMinPos; i < CurMaxPos; i++)
		            {
		                if (i < pAlgorithmInfo->iSignalLength
							&& pAlgorithmInfo->pSignalLog[i + 1] - pAlgorithmInfo->pSignalLog[i]
								> 1.5f * Max(freNoisestd, 0.05f))
		                {
		                    Count++;
		                }
		                else
		                {
		                    Count = 0;
		                }
		                if (Count >= 3)
		                {
		                    pEventsTable->EventsInfo[j].iBegin = i - Count + 1;
		                    EndFlag = 1;
		                }
		                if (EndFlag == 1)
		                {
		                    break;
		                }
		            }
				}
				CurMinPos = GetMinIndex(pAlgorithmInfo->pSignalLog
					+ pEventsTable->EventsInfo[j].iBegin, SearchLen2)
	                + pEventsTable->EventsInfo[j].iBegin;
				pEventsTable->EventsInfo[j].iBegin 
					= Min(pEventsTable->EventsInfo[j].iBegin, CurMinPos);
	        }
		}
	}
	//重新计算各个参数
    TransEventsTable(pEventsTable, g_pEventsRegionTable);
    GetEventsParameter(g_pSignalLog, pAlgorithmInfo->iSignalLength,
		g_pEventsRegionTable, pEventsTable, fSampleRating);

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetEventsTable3-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetEventsTable3-----------------------\n\n");
		PrintEventInfo(test_file_name, pEventsTable);
	#else
		PrintEventInfo(stdout, pEventsTable);
	#endif
	//PrintEventInfo(stdout, pEventsTable);	ALG_DEBUG("\n\n\n");
#endif

}

// 回波事件判别
void CheckEcho(ALGORITHM_INFO *pAlgorithmInfo)
{
    EVENTS_TABLE *pEventsTable = &(pAlgorithmInfo->EventsTable);////反射事件的数量
    EVENTS_TABLE *pTmpEventsTable = (EVENTS_TABLE *)malloc(sizeof(EVENTS_TABLE));//临时变量

	int iOffsetLen = pAlgorithmInfo->iOffsetFiberLen;
    int i = 0, j = 0;// k = 0;
    //int iRoundTimes = 0; //回波与事假相差的倍数
    int iJudgeLength = 0;//回波与事件相差的长度
    int iJudgeLength1 = 0;//回波与事件相差的长度
    int iJudgeLength2 = 0;//跳线较脏导致的鬼影
    //int iEchoFlag = 0;//事件是回波的标志
    float fMaxTemp1 = 0.0f;
    float fMaxTemp2 = 0.0f;
	float fMinTemp = 0.0f;
    pTmpEventsTable->iEventsNumber = 0;
    int iTail = 0;
    int iTailIndex = 0;
    float fEndThr = 0.0f;
    float fSampleRating = pAlgorithmInfo->SetParameters.fSampleRating;
    int iPointNum = (int)(10.0f * fSampleRating); // 40M采样率下10个点对应的当前采样率下的数据个数
    int iBlind = pAlgorithmInfo->iBlind;

    fEndThr = Max(4.0f * pAlgorithmInfo->fInputNoise, 5.0f) + 6;
    //防止末端阈值过大，添加该判断 
    if (pAlgorithmInfo->iSignalLength < 800 * iPointNum)
        fEndThr = Min(fEndThr, 15.0f);
    else
        fEndThr = Min(fEndThr, 17.0f);
    //计算粗略的末端位置
    for (i = 15; i < pAlgorithmInfo->iSignalLength; i++)
    {
        if (pAlgorithmInfo->pSignalLog[i] < fEndThr)
        {
            iTail = i;
            break;
        }
    }

    //记录反射事件
    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
        if (pEventsTable->EventsInfo[i].iStyle == 2)
        {
            pTmpEventsTable->EventsInfo[pTmpEventsTable->iEventsNumber]
				= pEventsTable->EventsInfo[i];
            pTmpEventsTable->iEventsNumber++;
            if (pEventsTable->EventsInfo[i].iBegin <= iTail)
            {
                iTailIndex = pTmpEventsTable->iEventsNumber - 1;
            }
        }
    }
    if (iTailIndex < 1)
    {
        iTailIndex = 1;
    }

	//计算每个反射峰的最大值
	float maxEventValue[500] = { 0.0f };
	if (pTmpEventsTable->iEventsNumber >= 2)
	{
		int iBegin = 0;
		int iEnd = 0;
		int iLen = 0;
		for (i = pTmpEventsTable->iEventsNumber - 1; i >= 0; --i)
		{
			iBegin = pTmpEventsTable->EventsInfo[i].iBegin;
			iEnd = pTmpEventsTable->EventsInfo[i].iEnd;
			iLen = iEnd - iBegin + 1;
			maxEventValue[i] = GetMaxFloat(pAlgorithmInfo->pSignalLog + iBegin, iLen);
		}
	}

    if (pTmpEventsTable->iEventsNumber > 2)
    {
        for (i = pTmpEventsTable->iEventsNumber - 1; i > 1; i--) // 从最后一个反射事件往前搜索回波,第一个和第二个反射事件不进行判断
        {
			if (pTmpEventsTable->EventsInfo[i].fEventsPosition < 40.0f)
			{
				//continue;
			}
			/*
		    fMaxTemp1 = GetMaxFloat(pAlgorithmInfo->pSignalLog
				+ pTmpEventsTable->EventsInfo[i].iBegin,
                pTmpEventsTable->EventsInfo[i].iEnd 
				- pTmpEventsTable->EventsInfo[i].iBegin + 1);
			*/
			fMaxTemp1 = maxEventValue[i];
			fMinTemp = GetMinFloat(pAlgorithmInfo->pSignalLog
				+ pTmpEventsTable->EventsInfo[i - 1].iEnd,
                pTmpEventsTable->EventsInfo[i].iBegin
				- pTmpEventsTable->EventsInfo[i - 1].iEnd + 1);
            j = i - 1;
            while (j > 0) // 第二个反射事件开始判断
            {
				fMaxTemp2 = GetMaxFloat(pAlgorithmInfo->pSignalLog
					+ pTmpEventsTable->EventsInfo[j].iBegin,
                        Min(pTmpEventsTable->EventsInfo[j + 1].iBegin
							- pTmpEventsTable->EventsInfo[j].iBegin + 1, 2 * iBlind));
				// 回波的判断依据1、事件段应大于50m；2、求余数，余数小于5m；3、反射强度不能变强
				iJudgeLength = Min((pTmpEventsTable->EventsInfo[i].iBegin + iOffsetLen) 
							% (pTmpEventsTable->EventsInfo[j].iBegin + iOffsetLen),
					pTmpEventsTable->EventsInfo[j].iBegin + iOffsetLen
					- (pTmpEventsTable->EventsInfo[i].iBegin + iOffsetLen) 
							% (pTmpEventsTable->EventsInfo[j].iBegin + iOffsetLen));

				// 由于延长光纤的影响会出现余数大于20m的现象，故要排除
				iJudgeLength1 = Min((pTmpEventsTable->EventsInfo[i].iBegin) 
							% (pTmpEventsTable->EventsInfo[j].iBegin),
					(pTmpEventsTable->EventsInfo[j].iBegin)
					- (pTmpEventsTable->EventsInfo[i].iBegin) 
							% (pTmpEventsTable->EventsInfo[j].iBegin));

				iJudgeLength2 = (int)fabs(pTmpEventsTable->EventsInfo[i].iBegin 
					- iOffsetLen - pTmpEventsTable->EventsInfo[j].iBegin);

				if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_5NS)
				{
					if ((pTmpEventsTable->EventsInfo[i].fReflect < -50.0f)
						&& (iJudgeLength < iPointNum / 10 || iJudgeLength1 < iPointNum / 10 || iJudgeLength2 < iPointNum / 10)
						&& (pTmpEventsTable->EventsInfo[j].fReflect > -40.0f)
						&& (pTmpEventsTable->EventsInfo[i].iBegin / pTmpEventsTable->EventsInfo[j].iBegin < 4)
						&& (maxEventValue[j] - maxEventValue[i] > 8.0f)
						&& (pTmpEventsTable->EventsInfo[j].fReflect - pTmpEventsTable->EventsInfo[i].fReflect > 15.0f))
					{
						pTmpEventsTable->EventsInfo[i].iStyle = 3;//说明是回波
						ALG_DEBUG("-----first time1----i = %d, j = %d\n", i, j);
						break;
					}
					else
					{
						j--;
						continue;
					}
				}

				int disThr = iPointNum / 2;
				if (pAlgorithmInfo->SetParameters.iwavelength == 1
					&& range > 10000.0f 
					&& pAlgorithmInfo->PulseWidth == ENUM_PULSE_2US
					&& i == pTmpEventsTable->iEventsNumber - 1)
				{
					disThr = (int)5 * iPointNum / 2;
				}
				if (((iJudgeLength < Min(disThr, Max(iBlind * 2.5f, 8.0f / pAlgorithmInfo->fPointLength)))
					|| ((iJudgeLength < Max(iBlind * 2.5f, 10.0f * pAlgorithmInfo->fPointLength)) 
						&& (pAlgorithmInfo->PulseWidth > ENUM_PULSE_2US))
					|| (iJudgeLength2 < Min(disThr, Max(iBlind * 2.5f, 8.0f / pAlgorithmInfo->fPointLength)))
					|| ((iJudgeLength1 < Max(Min(iPointNum / 2, iBlind * 2), iPointNum / 5))))
				&& ((pTmpEventsTable->EventsInfo[i].fReflect <= pTmpEventsTable->EventsInfo[j].fReflect)
					|| ((i == pTmpEventsTable->iEventsNumber - 1) && (pTmpEventsTable->EventsInfo[j].fReflect > -55.0f))
					|| ((fMinTemp > pAlgorithmInfo->fInputNoise) && (pTmpEventsTable->EventsInfo[j].fReflect > -55.0f)))
				&& ((pTmpEventsTable->EventsInfo[i].fLoss < pTmpEventsTable->EventsInfo[j].fLoss)
					|| ((i == pTmpEventsTable->iEventsNumber - 1) && (pTmpEventsTable->EventsInfo[j].fReflect > -55.0f))
					|| ((fMinTemp < pAlgorithmInfo->fInputNoise) && (pTmpEventsTable->EventsInfo[j].fReflect > -55.0f)))
				&& (pTmpEventsTable->EventsInfo[i].iBegin / pTmpEventsTable->EventsInfo[j].iBegin < 4)
				&& (fMaxTemp2 > fMaxTemp1))
				{
					if ((pTmpEventsTable->EventsInfo[i].fLoss > 2)
						&& (fMinTemp > pAlgorithmInfo->fInputNoise)
						&& ((pTmpEventsTable->EventsInfo[i].fAttenuation > 4)
						|| (pTmpEventsTable->EventsInfo[i].fAttenuation < -6))
						&& (pTmpEventsTable->EventsInfo[i].fReflect > -50.0f))
					{
						j--;
						continue;
					}

					if ((frontParament.pulseWidth <= ENUM_PULSE_100NS)
						&& (fabsf(fMaxTemp2 - fMaxTemp1) < 2.8f 
						|| fabs(pAlgorithmInfo->pSignalLog[pTmpEventsTable->EventsInfo[i].iBegin] 
						- pAlgorithmInfo->pSignalLog[pTmpEventsTable->EventsInfo[j].iBegin]) < 2.0f)
						&& (fMaxTemp2 < 22.0f) && (pTmpEventsTable->EventsInfo[i].fReflect > -50.0f))
					{
						j--;
						continue;
					}
					if ((pTmpEventsTable->EventsInfo[i].fLoss > 2.0f)
						&& (i == pTmpEventsTable->iEventsNumber - 1) 
						&& ((pTmpEventsTable->EventsInfo[i].iBegin 
						- pTmpEventsTable->EventsInfo[i - 1].iBegin > 30.0f / pAlgorithmInfo->fPointLength
						&& frontParament.pulseWidth > ENUM_PULSE_20NS)
						|| (frontParament.pulseWidth <= ENUM_PULSE_20NS))
						&& (pAlgorithmInfo->pSignalLog[pTmpEventsTable->EventsInfo[i].iBegin] > EndThrNoise
						|| pTmpEventsTable->EventsInfo[i].fLoss < -2.0f)
						&& (pTmpEventsTable->EventsInfo[i].fReflect > -50.0f)
						&& (pTmpEventsTable->EventsInfo[j].fReflect - pTmpEventsTable->EventsInfo[i].fReflect < 15.0f))
					{
						j--;
						continue;
					}

					if (((fMinTemp > pAlgorithmInfo->fInputNoise)
						|| (frontParament.pulseWidth < ENUM_PULSE_20NS)
						|| ((pTmpEventsTable->EventsInfo[i].fLoss < -2.0f)
						&& (frontParament.pulseWidth < ENUM_PULSE_100NS)
						&& (pTmpEventsTable->EventsInfo[i].fEventsPosition > 10000.0f)))
						&& (pTmpEventsTable->EventsInfo[i].fReflect > -50.0f))
					{
						if ((pTmpEventsTable->EventsInfo[i].fReflect < -38.0f)
							&& (pTmpEventsTable->EventsInfo[j].fReflect > -30.0f)
							&& (pTmpEventsTable->EventsInfo[j].fReflect
								- pTmpEventsTable->EventsInfo[i].fReflect > 15.0f)
							&& (iJudgeLength < Max(Min(disThr, 2 * iBlind), disThr / 4)))
						{
							pTmpEventsTable->EventsInfo[i].iStyle = 3;//说明是回波
							ALG_DEBUG("-----first time1----i = %d, j = %d\n", i, j);
							break;
						}
						j--;
						continue;
					}
					pTmpEventsTable->EventsInfo[i].iStyle = 3;//说明是回波
					ALG_DEBUG("-----first time2----i = %d, j = %d\n", i, j);
					break;
				}
				j--;
				/*
                ///回波的判断依据1、事件段应大于50m；2、求余数，余数小于5m；3、反射强度不能变强
                if (((pTmpEventsTable->EventsInfo[i].iBegin - pTmpEventsTable->EventsInfo[i - 1].iBegin) > 2 * iPointNum)
                    && ((pTmpEventsTable->EventsInfo[i].iBegin + iPointNum) % pTmpEventsTable->EventsInfo[j].iBegin < 5 * iPointNum / 2)
                    && (pTmpEventsTable->EventsInfo[i].fReflect <= pTmpEventsTable->EventsInfo[j].fReflect)
                    && (pTmpEventsTable->EventsInfo[i].iBegin / pTmpEventsTable->EventsInfo[j].iBegin < 5)
                    && (pTmpEventsTable->EventsInfo[i].fReflect < -50.0f))  
                {
                    pTmpEventsTable->EventsInfo[i].iStyle = 3;//说明是回波
                    //printf("-----------------------echo1-------------------------\n");
                    break;
                }
                j--;
                */
            }
        }
    }

	// 小脉宽UPC接口以及接跳线时，在机器内部光纤长度下会出现跟起始点相关的鬼影事件
	if (pTmpEventsTable->iEventsNumber == 2)
    {
		fMaxTemp1 = maxEventValue[1];
		fMaxTemp2 = maxEventValue[0];
		int iBegin1 = pTmpEventsTable->EventsInfo[1].iBegin;
		int iBegin0 = pTmpEventsTable->EventsInfo[0].iBegin;
		iJudgeLength = Min((iBegin1 + iOffsetLen) % (iBegin0 + iOffsetLen),
			iBegin0 + iOffsetLen - (iBegin1 + iOffsetLen) % (iBegin0 + iOffsetLen));

		int disThr = (int)(2.0f / pAlgorithmInfo->fPointLength);
		if ((iJudgeLength < disThr)
			&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_100NS)
			&& (pTmpEventsTable->EventsInfo[1].fReflect <= pTmpEventsTable->EventsInfo[0].fReflect)
			&& (pTmpEventsTable->EventsInfo[1].fLoss < pTmpEventsTable->EventsInfo[0].fLoss)
			&& ((iBegin1 + iOffsetLen) / (iBegin0 + iOffsetLen) <= 2)
			&& (fMaxTemp2 > fMaxTemp1))
		{
			if (((pTmpEventsTable->EventsInfo[1].fLoss > 2.0f)
				&& (pTmpEventsTable->EventsInfo[1].fAttenuation > 4.0f
					|| pTmpEventsTable->EventsInfo[1].fAttenuation < -6.0f))
				|| (pTmpEventsTable->EventsInfo[1].fLoss > 3.0f
					&& pTmpEventsTable->EventsInfo[0].fContinueAttenuation
					< pTmpEventsTable->EventsInfo[0].iContinueLength * 0.4f
					* pAlgorithmInfo->fPointLength / 1000.0f))
			{
				;
			}
			else
			{
				pTmpEventsTable->EventsInfo[1].iStyle = 3;//说明是回波
				ALG_DEBUG("-----first time--********--i = %d, j = %d\n", i, j);
			}
		}
    }

    /*if (pTmpEventsTable->iEventsNumber >= 2)
    {
        for (i = 2; i < pTmpEventsTable->iEventsNumber; i++)
        {
            iEchoFlag = 0;
            //得到反射事件i的最大值
            fMaxTemp1 = GetMaxFloat(pAlgorithmInfo->pSignalLog + pTmpEventsTable->EventsInfo[i].iBegin,
                pTmpEventsTable->EventsInfo[i].iEnd - pTmpEventsTable->EventsInfo[i].iBegin + 1);
            //再次判断反射事件是否是回波事件
            for (j = i - 1; j >= iTailIndex; j--)
            {
                iJudgeLength = pTmpEventsTable->EventsInfo[i].iBegin - pTmpEventsTable->EventsInfo[j].iBegin + iPointNum;
                for (k = 1; k <= j; k++)
                {
                    //反射事件在末端之后，其减去前面的反射事件，如果长度是末端之前的反射事件的整数倍，
                    //并且反射率小于前面的事件的反射率，则该事件是回波事件
                    iRoundTimes = (int)(1.0*iJudgeLength / pTmpEventsTable->EventsInfo[k].iBegin + 0.5f);
                    //得到反射事件k的最大值
                    fMaxTemp2 = GetMaxFloat(pAlgorithmInfo->pSignalLog + pTmpEventsTable->EventsInfo[k].iBegin,
                        pTmpEventsTable->EventsInfo[k].iEnd - pTmpEventsTable->EventsInfo[k].iBegin + 1);

                    if ((iRoundTimes >= 1) && (iRoundTimes <= 5) && (abs(iRoundTimes*pTmpEventsTable->EventsInfo[k].iBegin - iJudgeLength) < iBlind)  // iPointNum->iBlind  changed by wyl 2019/07/27
                        && (pTmpEventsTable->EventsInfo[k].fReflect >= pTmpEventsTable->EventsInfo[i].fReflect + 0.2f*iRoundTimes)
                        && (fMaxTemp1 + 1.0f < fMaxTemp2) && (pTmpEventsTable->EventsInfo[i].fReflect<-30.0f) && (frontParament.pulseWidth > ENUM_PULSE_5NS))
                    {
						if ((pTmpEventsTable->EventsInfo[i].fLoss > 3 
							&& (pTmpEventsTable->EventsInfo[i].fAttenuation > 4 || pTmpEventsTable->EventsInfo[i].fAttenuation < -6))
							|| (pTmpEventsTable->EventsInfo[i].fLoss > 4) || (pTmpEventsTable->EventsInfo[i].iStyle == 1))
							continue;
                        iEchoFlag = 1;
                        pTmpEventsTable->EventsInfo[i].iStyle = 3;
						printf("-----second time----i = %d, j = %d, iBegin = %d\n", i, j, pTmpEventsTable->EventsInfo[i].iBegin);
                        break;
                    }
                }
                if (iEchoFlag > 0)
                    break;
            }
        }
    }
    */
	int iEchoFlag = 0;
	int k = 0;
	int iRoundTimes = 0;
	for (i = 2; i < pTmpEventsTable->iEventsNumber; i++)
	{
		if (pTmpEventsTable->EventsInfo[i].iStyle == 3)
		{
			continue;
		}
		iEchoFlag = 0;
		//得到反射事件i的最大值
		fMaxTemp1 = maxEventValue[i];
		//再次判断反射事件是否是回波事件
		for (j = i - 1; j >= iTailIndex; j--)
		{
			iJudgeLength = pTmpEventsTable->EventsInfo[i].iBegin - pTmpEventsTable->EventsInfo[j].iBegin;
			for (k = 1; k <= j; k++)
			{
				//反射事件在末端之后，其减去前面的反射事件，如果长度是末端之前的反射事件的整数倍，
				//并且反射率小于前面的事件的反射率，则该事件是回波事件
				iRoundTimes = (int)(1.0 * iJudgeLength / pTmpEventsTable->EventsInfo[k].iBegin + 0.5f);
				//得到反射事件k的最大值
				fMaxTemp2 = maxEventValue[k];

				if ((iRoundTimes >= 1) && (iRoundTimes <= 5) 
					&& (abs(iRoundTimes * pTmpEventsTable->EventsInfo[k].iBegin - iJudgeLength) < Min(iBlind, iOffsetLen))
					&& (pTmpEventsTable->EventsInfo[k].fReflect > pTmpEventsTable->EventsInfo[i].fReflect + 0.2f * iRoundTimes)
					&& (fMaxTemp1 + 1.0f < fMaxTemp2) && (pTmpEventsTable->EventsInfo[i].fReflect < -40.0f)
					&& (pTmpEventsTable->EventsInfo[k].fReflect > -40.0f))
				{
					if ((pTmpEventsTable->EventsInfo[i].fLoss > 3.0f
						&& (pTmpEventsTable->EventsInfo[i].fAttenuation > 4.0f || pTmpEventsTable->EventsInfo[i].fAttenuation < -6.0f))
						|| (pTmpEventsTable->EventsInfo[i].fLoss > 4.0f) || (pTmpEventsTable->EventsInfo[i].iStyle == 1))
						continue;
					iEchoFlag = 1;
					pTmpEventsTable->EventsInfo[i].iStyle = 3;
					ALG_DEBUG("-----second time----i = %d, j = %d, iBegin = %d\n", i, j, pTmpEventsTable->EventsInfo[i].iBegin);
					break;
				}
			}
			if (iEchoFlag > 0)
				break;

			if ((pTmpEventsTable->EventsInfo[j].fReflect > -30.0f)
				&& (pTmpEventsTable->EventsInfo[j].fReflect - pTmpEventsTable->EventsInfo[i].fReflect > 20.0f))
			{
				for (k = 1; k < j; k++)
				{
					if (pTmpEventsTable->EventsInfo[k].fReflect < -30.0f)
					{
						continue;
					}
					fMaxTemp2 = maxEventValue[k];
					iJudgeLength2 = pTmpEventsTable->EventsInfo[j].iBegin - pTmpEventsTable->EventsInfo[k].iBegin;
					if (abs(iJudgeLength2 - iJudgeLength) < Min(iOffsetLen, iBlind * 3 / 2)
						&& (pTmpEventsTable->EventsInfo[i].fReflect < -40.0f) && (fMaxTemp1 + 1.0f < fMaxTemp2))
					{
						pTmpEventsTable->EventsInfo[i].iStyle = 3;
						ALG_DEBUG("-----second time----i = %d, j = %d, iBegin = %d\n", i, j, pTmpEventsTable->EventsInfo[i].iBegin);
						iEchoFlag = 1;
						break;
					}
				}
				if (iEchoFlag > 0)
					break;
			}
		}
	}

    //将EventsTable内容随之改变
    j = 0;
    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
        if (pEventsTable->EventsInfo[i].iBegin == pTmpEventsTable->EventsInfo[j].iBegin)
        {
            pEventsTable->EventsInfo[i].iStyle = pTmpEventsTable->EventsInfo[j].iStyle;
            j++;
        }
        if (j >= pTmpEventsTable->iEventsNumber)
            break;
    }
    //添加代码，回波事件的二次判断
  /*  for (i = 2; i < pEventsTable->iEventsNumber; i++)
    {
        if (pEventsTable->EventsInfo[i].iStyle == 2)
        {
            for (j = 0; j<i; j++)//查找前方是否存在2倍距离的事件
            {
                if ((abs(pEventsTable->EventsInfo[i].iBegin - (2 * pEventsTable->EventsInfo[j].iBegin)) < 3 * iPointNum / 2)
                    && (fabsf(pEventsTable->EventsInfo[j].fLoss) > fabsf(pEventsTable->EventsInfo[i].fLoss))
                    && (pEventsTable->EventsInfo[j].iStyle == 2)&& (pEventsTable->EventsInfo[j].fReflect > pEventsTable->EventsInfo[i].fReflect)
                    && (pTmpEventsTable->EventsInfo[i].fReflect < -30.0f))
                {
                    pEventsTable->EventsInfo[i].iStyle = 3;
					//printf("-----third time----i = %d, j = %d\n", i, j);
                }
            }
        }
    }
    */
	//添加代码，回波事件的二次判断
	for (i = 2; i < pEventsTable->iEventsNumber; i++)
	{
		if (pEventsTable->EventsInfo[i].iStyle == 2)
		{
			for (j = 0; j < i; j++)//查找前方是否存在2倍距离的事件
			{
				if (pEventsTable->EventsInfo[i].iStyle != 2)
				{
					continue;
				}
				if ((abs(pEventsTable->EventsInfo[i].iBegin - (2 * pEventsTable->EventsInfo[j].iBegin)) < Min(iBlind, iOffsetLen))
					&& (fabsf(pEventsTable->EventsInfo[j].fLoss) > fabsf(pEventsTable->EventsInfo[i].fLoss))
					&& (pEventsTable->EventsInfo[j].iStyle == 2)
					&& (pEventsTable->EventsInfo[j].fReflect > pEventsTable->EventsInfo[i].fReflect)
					&& (pEventsTable->EventsInfo[i].fReflect < -40.0f)
					&& (pEventsTable->EventsInfo[j].fReflect - pTmpEventsTable->EventsInfo[i].fReflect > 10.0f)
					&& (pEventsTable->EventsInfo[i].fEventsPosition > 40.0f)
					&& (pEventsTable->EventsInfo[i].fEventsPosition - pEventsTable->EventsInfo[j].fEventsPosition < 10000.0f))
				{
					pEventsTable->EventsInfo[i].iStyle = 3;
					//printf("-----third time----i = %d, j = %d\n", i, j);
					break;
				}
			}
		}
	}
    free(pTmpEventsTable);
}

// 查找光纤末端;
void GetFiberEnd(ALGORITHM_INFO *pAlgorithmInfo)
{	
    EVENTS_TABLE *pEventsTable = &(pAlgorithmInfo->EventsTable);
    // 计算实际距离(以米为单位)
    int i = 0;
    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
        pEventsTable->EventsInfo[i].fEventsPosition
			= pEventsTable->EventsInfo[i].iBegin * g_fLengthPerPoint;
        pEventsTable->EventsInfo[i].fEventsEndPosition
			= pEventsTable->EventsInfo[i].iEnd * g_fLengthPerPoint;
    }

    // sola不进行末端分析
	float fEndThr = Max(pAlgorithmInfo->fMaxInputNoise + 1.0f, EndThrNoise);
    int iTail = SearchEnd(g_pSignalLog, pAlgorithmInfo->iDisplayLenth, fEndThr);
	for (i = 0; i < pEventsTable->iEventsNumber; ++i)
	{
		if ((pEventsTable->EventsInfo[i].iStyle == 2)
			&& (pEventsTable->EventsInfo[i].fReflect > -50.0f)
			&& (pEventsTable->EventsInfo[i].fLoss > 2.0f)
			&& (pEventsTable->EventsInfo[i].iEnd > iTail))
		{
			iTail = pEventsTable->EventsInfo[i].iEnd;
		}
	}

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetFiberEnd 0-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetFiberEnd 0-----------------------\n\n");
		PrintEventInfo(test_file_name, pEventsTable);
	#else
		PrintEventInfo(stdout, pEventsTable);
	#endif
	//PrintEventInfo(stdout, pEventsTable);
	ALG_DEBUG("\n\n\n");
#endif

    // 末端分析
    EndAnalysis(pAlgorithmInfo, frontParament.fFiberEndThreshold, iTail);
	//防止末端定位到鬼影上
	if (pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle == 13)
    {
		pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle = 3;
		for (i = pEventsTable->iEventsNumber - 2; i > 0; --i)
		{
			if (pEventsTable->EventsInfo[i].iStyle <= 2)
			{
				pEventsTable->iEventsNumber = i + 1;
				break;
			}
		}
		EndAnalysis(pAlgorithmInfo, frontParament.fFiberEndThreshold, iTail);
    }
    TransEventsTable(pEventsTable, g_pEventsRegionTable);

    if (pEventsTable->iEventsNumber > 50)
    {
        memcpy(&pEventsTable->EventsInfo[49],
            &pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1], sizeof(EVENTS_INFO));
        pEventsTable->iEventsNumber = 50;
    }
    pEventsTable->EventsInfo[0].fLoss = 0.0f;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetFiberEnd 1-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetFiberEnd 1-----------------------\n\n");
		PrintEventInfo(test_file_name, pEventsTable);
	#else
		PrintEventInfo(stdout, pEventsTable);
	#endif
	//PrintEventInfo(stdout, pEventsTable);
	ALG_DEBUG("\n\n\n");
#endif

    // 删除数值不正常的事件
    int pSaveIndex[MAX_EVENTS_NUMBER] = { 0 };//未删除的事件索引
    int iSaveNumber = 0;					//未删除的事件数量
    
	ALG_DEBUG("----1---iEventsNumber = %d----------\n", pEventsTable->iEventsNumber);
	
    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
        int iIsNan = 0;
        int iIsInf = 0;
        iIsNan = isnan(pEventsTable->EventsInfo[i].fAttenuation);
        iIsInf = isinf(pEventsTable->EventsInfo[i].fAttenuation);
        if ((iIsNan) || (iIsInf) || (fabsf(pEventsTable->EventsInfo[i].fAttenuation) > 1.5f))
            pEventsTable->EventsInfo[i].fAttenuation = 1.5f;

        if ((i == 0) || (i == pEventsTable->iEventsNumber - 1))
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
            continue;
        }
        if ((pEventsTable->EventsInfo[i].iStyle != 1)
			&& (pEventsTable->EventsInfo[i].fLoss < 0.0f))
        {
            pEventsTable->EventsInfo[i].fLoss = -pEventsTable->EventsInfo[i].fLoss;
        }
		/*if ((pEventsTable->EventsInfo[i].iStyle == 2)
			&& (pEventsTable->EventsInfo[i].fReflect < frontParament.fReflectThreshold)
			&& (i > 0) && (i < pEventsTable->iEventsNumber - 1)
			&& (pEventsTable->EventsInfo[i].fLoss < frontParament.fLossThreshold))
		{
			continue;
		}
		*/

        if (((pEventsTable->EventsInfo[i].iStyle < 1)
			&& ((pEventsTable->EventsInfo[i].fLoss < frontParament.fLossThreshold)
            || (isnan(pEventsTable->EventsInfo[i].fLoss))
            || (isinf(pEventsTable->EventsInfo[i].fLoss))))
            || ((pEventsTable->EventsInfo[i].iStyle == 1)
            && ((fabsf(pEventsTable->EventsInfo[i].fLoss) > -frontParament.fLossThreshold)
            || (isnan(pEventsTable->EventsInfo[i].fLoss))
            || (isinf(pEventsTable->EventsInfo[i].fLoss)))))
        {
            ;
        }
        else
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
        }
    }
    for (i = 0; i < iSaveNumber; ++i)
    {
        pEventsTable->EventsInfo[i] = pEventsTable->EventsInfo[pSaveIndex[i]];
    }
    pEventsTable->iEventsNumber = iSaveNumber;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetFiberEnd 2-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetFiberEnd 2-----------------------\n\n");
		PrintEventInfo(test_file_name, pEventsTable);
	#else
		PrintEventInfo(stdout, pEventsTable);
	#endif
	//PrintEventInfo(stdout, pEventsTable);
	ALG_DEBUG("\n\n\n");
#endif

	ALG_DEBUG("----2---iEventsNumber = %d----------\n", pEventsTable->iEventsNumber);

	/*
    // 调整起始点位置
    if ((pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle == 10) && (pEventsTable->iEventsNumber >= 2))
    {
        int pos = Max(pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin
            - (int)(1.1 * pAlgorithmInfo->iBlind), pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 2].iBegin);
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin
            = GetMinIndex(pAlgorithmInfo->pSignalLog + pos, (int)(1.1 * pAlgorithmInfo->iBlind)) + pos;
    }
*/
    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
        pEventsTable->EventsInfo[i].iPulse = pAlgorithmInfo->PulseWidth;
        if (pEventsTable->EventsInfo[i].fAttenuation < 0.0f)
        {
            pEventsTable->EventsInfo[i].fAttenuation
				= fabsf(pEventsTable->EventsInfo[i].fAttenuation);
        }
        if (pEventsTable->EventsInfo[0].fAttenuation > 0.5f)
        {
            if (pAlgorithmInfo->SetParameters.iwavelength == 0)
            {
                pEventsTable->EventsInfo[0].fAttenuation = 0.33f;
            }
            else
            {
                pEventsTable->EventsInfo[0].fAttenuation = 0.18f;
            }
        }

        if ((frontParament.pulseWidth == 1)
			&& (pEventsTable->EventsInfo[i].fLoss < 0.0f)
			&& (i > 0) && (i < pEventsTable->iEventsNumber - 1)
            && ((pEventsTable->EventsInfo[i].iStyle == 0)
            || (pEventsTable->EventsInfo[i].iStyle == 2)))
        {
            pEventsTable->EventsInfo[i].fLoss = 0.02f;
        }
    }

    // 调整事件的起始位置
    int time = 10;
    for (i = 1; i < pEventsTable->iEventsNumber; ++i)
    {
        int j = 0;
        int index = pEventsTable->EventsInfo[i].iBegin;
        for (j = 0; j < time; j++)
        {
            if ((fabsf(pAlgorithmInfo->pSignalLog[index]
				- pAlgorithmInfo->pSignalLog[index + 1]) < 0.05f)
                && (index < Min(pEventsTable->EventsInfo[i].iEnd - 1,
					pAlgorithmInfo->iSignalLength)))
            {
                index++;
            }
            else
            {
	        	if (pEventsTable->EventsInfo[i].iStyle == 0
					&& pAlgorithmInfo->PulseWidth > ENUM_PULSE_500NS)
        		{
					pEventsTable->EventsInfo[i].iBegin 
						= pEventsTable->EventsInfo[i].iBegin;
					break;
        		}
				else
				{
					pEventsTable->EventsInfo[i].iBegin = index;
            		break;
				}
            }
        }
    }
    pEventsTable->EventsInfo[0].iBegin = 0;
	// 事件end点的修改
	for (i = 0; i < pEventsTable->iEventsNumber - 1; ++i)
    {
        pEventsTable->EventsInfo[i].iEnd
			= Min(pEventsTable->EventsInfo[i].iBegin 
				+ (int)(1.5f * pAlgorithmInfo->iBlind),
				pEventsTable->EventsInfo[i + 1].iBegin - 1);
    }
    TransEventsTable(pEventsTable, g_pEventsRegionTable);
	GetEventsParameter(g_pSignalLog, pAlgorithmInfo->iSignalLength,
		g_pEventsRegionTable, pEventsTable, pAlgorithmInfo->SetParameters.fSampleRating);
	ParaAnlysis(pEventsTable);
	TransEventsTable(pEventsTable, g_pEventsRegionTable);
	GetEventsParameter(g_pSignalLog, pAlgorithmInfo->iSignalLength,
		g_pEventsRegionTable, pEventsTable, pAlgorithmInfo->SetParameters.fSampleRating);
	for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
        pEventsTable->EventsInfo[i].fEventsPosition
			= pEventsTable->EventsInfo[i].iBegin * g_fLengthPerPoint;
        pEventsTable->EventsInfo[i].fEventsEndPosition
			= pEventsTable->EventsInfo[i].iEnd * g_fLengthPerPoint;
    }
	
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------GetFiberEnd 3-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------GetFiberEnd 3-----------------------\n\n");
		PrintEventInfo(test_file_name, pEventsTable);
	#else
		PrintEventInfo(stdout, pEventsTable);
	#endif
	//PrintEventInfo(stdout, pEventsTable);
	ALG_DEBUG("\n\n\n");
#endif

}

// 参数分析
void ParaAnlysis(EVENTS_TABLE *pEventsTable)
{
    int pSaveIndex[MAX_EVENTS_NUMBER] = { 0 };
    int iSaveNumber = 0;	
	int i = 0;
    for (i = 0; i < pEventsTable->iEventsNumber; ++i)
    {
		if ((i == 0) || (i == pEventsTable->iEventsNumber - 1))
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
            continue;
        }
		if (((pEventsTable->EventsInfo[i].iStyle == 2)
			|| (pEventsTable->EventsInfo[i].iStyle == 3))
			&& (pEventsTable->EventsInfo[i].fReflect < frontParament.fReflectThreshold))
		{
			if (pEventsTable->EventsInfo[i].fLoss < -frontParament.fLossThreshold)
			{
				pEventsTable->EventsInfo[i].iStyle = 1;
			}
			else if (pEventsTable->EventsInfo[i].fLoss > frontParament.fLossThreshold)
			{
				pEventsTable->EventsInfo[i].iStyle = 0;
			}
			else
			{
				continue;
			}
		}
		if ((pEventsTable->EventsInfo[i].iStyle == 1)
			&& (pEventsTable->EventsInfo[i].fLoss > -frontParament.fLossThreshold))
		{
			continue;
		}
		if ((pEventsTable->EventsInfo[i].iStyle == 0)
			&& (pEventsTable->EventsInfo[i].fLoss < frontParament.fLossThreshold))
		{
			continue;
		}
        pSaveIndex[iSaveNumber] = i;
        iSaveNumber++;
    }
    for (i = 0; i < iSaveNumber; ++i)
    {
        pEventsTable->EventsInfo[i] = pEventsTable->EventsInfo[pSaveIndex[i]];
    }
    pEventsTable->iEventsNumber = iSaveNumber;
}

// 归一化各级小波信号
void NormWaveletCoef(
	float *pNormSignal, //待归一化的小波信号
	const int iSignalLength, //小波信号长度
	const int iEffective_Len,//信号有效长度
	const int iNormWindow,        //归一化窗口宽度
	const int iWaveletWindow, //小波窗口宽度
	const int iMaxEventsNumber, //最大事件数量查找阈值
	const int iIterationNumber) //迭代次数) 
{
	int i = 0;
	int j = 0;
	int iNumber = 0;
	int pTable[MAX_EVENTS_NUMBER][2] = {{0}};
	int iTableNumber = 0; //事件数量
	int flag = 0;//是否有奇异点标志
	int iStep = 0;////噪声估计步长
	float fSigma = 0;///噪声估计系数
	int iIterationTimes = 0;//迭代次数
	float *pNoiseThr = NULL;//噪声阈值
	float *pNoiseMean = NULL;//噪声均值
	int *pIndex = NULL; //小波奇异点索引值
	int iIndexNumber = 0; //小波奇异点数量
	int iIndexFlag = 0; //奇异点数量标志位 大于制定阈值则为1，否则为0

	int iEventsFlag = 0; //事件个数标志位 大于制定阈值为1，否则为0

	float fTmpThr = 0.0f;
	float fTmpMean = 0.0f;
	//光纤较短时，噪声估计系数小一点 changed by sjt 2015.07.14
	if (iWaveletWindow < 64)
	{
		if (iEffective_Len < 8000)
		{
			fSigma = 2.0f;
		}
		else
		{
			fSigma = 3.5f;
		}
		iStep = 64;
	}
	else
	{	
		if (iEffective_Len < 8000)
		{
			fSigma = 2.5f;
		}
		else
		{
			fSigma = 4.0f;
		}
		iStep = iWaveletWindow;
	}

	///计算链路噪声阈值和均值
	pNoiseThr = (float *)calloc(iSignalLength, sizeof(float));
	pNoiseMean= (float *)calloc(iSignalLength, sizeof(float));
	pIndex = (int *)calloc(iSignalLength, sizeof(int));

	NoiseEstimata(pNormSignal, iSignalLength, iEffective_Len,
		iNormWindow, iStep, fSigma,  pNoiseThr, pNoiseMean);

	//开始迭代，查找小波系数中的奇异点，并将其剔除运算区域，避免参数计算错误
	while ((iIterationTimes < iIterationNumber) || (flag == 1))
	{
		if(iSignalLength > 50000)//若输入信号超过120km，则不进入后续运算
		{
			break;
		}
		iIterationTimes++;
		flag = 0;
		iIndexNumber = 0;//奇异点个数初始化
		memset(pIndex, 0, sizeof(int) * iSignalLength);
		iIndexFlag = 0;//初始化奇异点标志
		for (i = 0; i < iSignalLength; ++i)
		{
			if ((pNormSignal[i] > (pNoiseThr[i] + pNoiseMean[i]))
				|| (pNormSignal[i] < (pNoiseMean[i] - pNoiseThr[i])))
			{
				pIndex[iIndexNumber] = i;
				iIndexNumber++;
			}
		}
		//对找到的离散点进行归类，分成不同的区间段

		iTableNumber = 0;//事件数量初始化为0
		iEventsFlag = 0;//初始化事件标志位
		if (!iIndexFlag)
		{
			for (i = 0; i < iIndexNumber; ++i)
			{
				//将index内的点分成事件区间，判断依据为点之间的距离index(i)-table(n,1)>len && index(i)-index(i-1)>2*width
				if (i == 0)
					pTable[i][0] = pIndex[i];
				else
				{
					if ((pIndex[i] - pIndex[i - 1]) > (iWaveletWindow + 50))
					{
						pTable[iTableNumber][1] = pIndex[i - 1];
						iTableNumber++;
						pTable[iTableNumber][0] = pIndex[i];
					}
				}
				if (i == iIndexNumber - 1)
				{
					pTable[iTableNumber][1] = pIndex[i];
					iTableNumber++;
				}
				if (iTableNumber > iMaxEventsNumber)
				{
					iEventsFlag = 1;
					break;
				}
			}
		}

		//将末端也定义为事件，方便后续处理
		if ((iSignalLength - pTable[iTableNumber - 1][1]) > (4 * iWaveletWindow + 50))
		{
			pTable[iTableNumber][0] = iSignalLength - 1;
			pTable[iTableNumber][1] = iSignalLength - 1;
			iTableNumber++;
		}

		//如果查找结果与上次相同则跳出循环
		if (iNumber == iIndexNumber)
			break;
		iNumber = iIndexNumber;

		//以事件作为分界点，重新计算各个位置的噪声阈值
		//计算原则：若两个区间距离足够近，则阈值保持原状。若区间足够长，则计算并使用两事件区中间的噪声阈值。每个事件区只能使用其后方的计算参数，而非前方。
		if (iTableNumber == 1)
			break;
		for (i = 0; i < iTableNumber - 1; ++i)
		{
			if ((pTable[i][1] + 2 * iStep - 1) <= (pTable[i + 1][0]))
			{
				fTmpThr = GetMeanFloat(pNoiseThr + pTable[i][1] + iStep, iStep);
				fTmpMean = GetMeanFloat(pNoiseMean + pTable[i][1] + iStep, iStep);
			}
			else
			{
				fTmpThr = GetMeanFloat(pNoiseThr + pTable[i][1],
					pTable[i + 1][0] - pTable[i][1] + 1);
				fTmpMean = GetMeanFloat(pNoiseMean + pTable[i][1],
					pTable[i + 1][0] - pTable[i][1] + 1);
			}

			for (j = pTable[i][0] + iStep - 1; j <= pTable[i][1] + iStep - 1; ++j)
			{
				if (j < iSignalLength)
				{
					pNoiseThr[j] = fTmpThr;
					pNoiseMean[j] = fTmpMean;
				}
			}
		}
	}
	for (i = 0; i < iSignalLength; ++i)
	{
		pNormSignal[i] -= pNoiseMean[i];
		//pNormSignal[i] /= pNoiseThr[i]; //只减去均值，不除以方差
	}
	free(pNoiseThr);
	pNoiseThr = NULL;
	free(pNoiseMean);
	pNoiseMean = NULL;
	free(pIndex);
	pIndex = NULL;

}

// 估计非平稳噪声阈值，输出动态阈值以及平均值
void NoiseEstimata(
	float *pInputSignal,   //待噪声检测的信号
	const int iSignalLength, //小波信号长度
	const int iEffective_Len,//信号有效长度
	const int iNormWindow,        //归一化窗口宽度
	const int iStep,         //噪声估计步长
	const float fSigma,       //噪声估计系数
	float *pNoiseThr,           ///噪声估测阈值
	float *pNoiseMean)  //噪声估计平均值
{
	int i = 0, j = 0; //计数变量
	float *pTmp = NULL; // 存储临时指针
	int iStepNumber = iSignalLength / iStep; //步长数量
	int iWindowDivStep = iNormWindow / iStep; //每个窗口的步长数量
	float *pMean = NULL;//窗口信号均值
	float *pStd = NULL; //窗口信号标准差
	int iTmp = 0; //临时整数变量

	float fMeanNumber = 0.0f;//存储均值
	float fStdNumber = 0.0f; //存储信号标准差
	float fEndNoiseStd = 0.0f;//存储有效信号后方噪声的标准差
	float fEndNoise = 0.0f;

	int iMinIndex = 0;
	int iTrueMinIndex = 0;
	//计算末端噪声方差，对有效信号后方数据进行噪声估计
	//最后一个步长内有一个跳变，导致方差计算不正确吗，应去掉；changed by sjt 2015.07.14
	fEndNoiseStd = GetStdFloat(pInputSignal + iEffective_Len,
		iSignalLength - iEffective_Len - iStep);
    if(iSignalLength - iEffective_Len > 800)
    {
        fEndNoise = GetStdFloat(pInputSignal + iSignalLength - 801, 800 - 2 * iStep);
    }
	
    fEndNoiseStd = Max(fEndNoiseStd, fEndNoise);
	fEndNoiseStd *= fSigma;
	fEndNoiseStd = Max(fEndNoiseStd, 2.0f);
	//防止噪声阈值计算错误，设置一个上限值changed by sjt 2015.07.14
	if (iEffective_Len < 8000)
	{
		fEndNoiseStd = Min(fEndNoiseStd, 12.0f);
	}
	else
	{
		fEndNoiseStd = Min(fEndNoiseStd, 14.0f);
	}

	if (iSignalLength % iStep != 0) iStepNumber++;//如果不能整除则加上1
	if (iNormWindow % iStep != 0) iWindowDivStep++;//如果不能整除则加上1

	////按步长和窗口长度计算信号均值和标准差
	pMean = (float *)calloc(iStepNumber, sizeof(float));
	pStd = (float *)calloc(iStepNumber, sizeof(float));

	pTmp = pInputSignal;
	iTmp  = iSignalLength - iNormWindow; //长度减窗口宽度
	j = 0;
	for (i = 0; i < iTmp; i = i + iStep)
	{
		if (i <= iEffective_Len)
		{
			fMeanNumber = GetMeanFloat(pTmp, iNormWindow);///窗口平均值
			fStdNumber = fSigma * GetStdFloat(pTmp, iNormWindow);///窗口标准差
			fStdNumber = Max(fStdNumber, 0.01f);/////防止标准差太小，设置标准差最小值					
		}
		else
		{
			fMeanNumber = 0.0f;
			fStdNumber = fEndNoiseStd;//有效数据之后，使用末端噪声					
		}
		pStd[j] = fStdNumber;
		pMean[j] = fMeanNumber;
		j = j + 1;
		pTmp = pTmp + iStep;
	}
	iTmp = Max(0,iTmp);
	fMeanNumber = GetMeanFloat(pInputSignal + iTmp, iSignalLength - iTmp);
	fStdNumber = fSigma * GetStdFloat(pInputSignal + iTmp, iSignalLength - iTmp);
	fStdNumber = Max(fStdNumber, 0.01f);
	for (i = j; i < iStepNumber; ++i)
	{
		pStd[i] = fStdNumber;
		pMean[i] = fMeanNumber;
	}
	///为避免第一段噪声估计值偏大，进行重新赋值
	if ((iSignalLength > 2 * iNormWindow) && (pStd[0] > 2 * pStd[1]))
	{
		pStd[0] = 2 * pStd[1];
		pMean[0] = 2 * pMean[1];
	}
	////为每个区间段选择合适的噪声阈值
	for (i = 0; i < iStepNumber; ++i)
	{
		if (i < iWindowDivStep)
		{
			iMinIndex = GetMinIndex(pStd, i + 1);
			iTmp = i * iStep;
			for (j = 0; j < iStep; ++j)
			{
				if (iTmp + j < iSignalLength)
				{
					pNoiseThr[iTmp + j] = pStd[iMinIndex];
					pNoiseMean[iTmp + j] = pMean[iMinIndex];
				}
			}

		}
		else
		{
			iMinIndex = GetMinIndex(pStd + i - iWindowDivStep, iWindowDivStep + 1);
			iTmp = i * iStep;
			iTrueMinIndex = iMinIndex + i - iWindowDivStep;
			for (j = 0; j < iStep; ++j)
			{
				if (iTmp + j < iSignalLength)
				{
					pNoiseThr[iTmp + j] = pStd[iTrueMinIndex];
					pNoiseMean[iTmp + j] = pMean[iTrueMinIndex];
				}
			}
		}
	}
	free(pStd);
	pStd = NULL;
	free(pMean);
	pMean = NULL;

}

// 初步查找各级小波的事件范围
void SearchRegion(
    int level,                              // 小波层级
    const float fThr,                       // 检测阈值
    EVENTS_REGION_TABLE *pEventsRegionTable,// 事件数组
    ALGORITHM_INFO *pAlgorithmInfo          // 算法结构体
    )
{
    // 初始化事件信息结构体
    EVENTS_REGION_TABLE *pTmpEventsRegionTable
		= (EVENTS_REGION_TABLE *)malloc(sizeof(EVENTS_REGION_TABLE));///临时数组变量
    EVENTS_REGION_INFO *pTmpEventsRegionInfo
		= pTmpEventsRegionTable->EventsRegionInfo;//临时事件数组
    memset(pTmpEventsRegionTable, 0, sizeof(EVENTS_REGION_TABLE));

    //查找输入信号的上升沿与下降沿区间
    int window = g_pWaveletWindow[level];
	int iBlind = pAlgorithmInfo->iBlind;
    float *pInputSignal = g_pWaveletCoef[level];

	ALG_DEBUG("------------------SearchEdge--------level = %d\n\n", level);
    SearchEdge(pInputSignal, fThr, window, pTmpEventsRegionTable, level, pAlgorithmInfo);
	ALG_DEBUG("------------------SearchEdge over-------level = %d\n\n", level);
    // 对上升沿事件和下降沿事件进行合并
    int *pEventsNumber = &pEventsRegionTable->iEventsNumber;
    EVENTS_REGION_INFO *pEventsRegionInfo = pEventsRegionTable->EventsRegionInfo;      //事件数组
    *pEventsNumber = 0;
    int *pTmpEventsNumber = &pTmpEventsRegionTable->iEventsNumber;

    int iState = 0, i = 0;
    int blind = pAlgorithmInfo->iBlind;
    int iPointNum = (int)(10.0f * pAlgorithmInfo->SetParameters.fSampleRating);
	int iPointNum3 = Max((int)(2.0f / pAlgorithmInfo->fPointLength), 1);

    for (i = 0; i < *pTmpEventsNumber; ++i)
    {
        switch (iState)
        {
        case 0:
        {
            (*pEventsNumber)++;
            pEventsRegionInfo[*pEventsNumber - 1] = pTmpEventsRegionInfo[i];
            if (pTmpEventsRegionInfo[i].iStyle == 0)
                iState = 1;//下降沿初始化为损耗事件，进入state1 
            else
                iState = 2;//上升沿初始化为增益事件，进入state2
            break;
        }
        case 1:
        {
            //分段依据:位置为上升沿，后区盲区内无事件或者存在非下降沿事件
            if ((pTmpEventsRegionInfo[i].iStyle == 0)
                && (pTmpEventsRegionInfo[i].iBegin - pTmpEventsRegionInfo[i - 1].iBegin > blind / 2))
            {
                (*pEventsNumber)++;
                pEventsRegionInfo[*pEventsNumber - 1] = pTmpEventsRegionInfo[i];
            }
            else
            {
                if (pTmpEventsRegionInfo[i].iStyle == 1)
                {
                    (*pEventsNumber)++;
                    pEventsRegionInfo[*pEventsNumber - 1] = pTmpEventsRegionInfo[i];
                    iState = 2;
                }
                else
                {
                    pEventsRegionInfo[*pEventsNumber - 1].iEnd = pTmpEventsRegionInfo[i].iEnd;
                }
            }

            break;
        }
        case 2:
        {
            if (pTmpEventsRegionInfo[i].iStyle == 0)
            {
                if ((pTmpEventsRegionInfo[i].iBegin - pTmpEventsRegionInfo[i - 1].iBegin
						> Min(2 * blind + iPointNum, 20 * iBlind))
                    && (pTmpEventsRegionInfo[i].iBegin - pTmpEventsRegionInfo[i - 1].iEnd
						> Min(2 * window + iPointNum, 20 * iBlind)))
                {
                    pEventsRegionInfo[*pEventsNumber - 1].iEnd
						= pTmpEventsRegionInfo[i - 1].iEnd;
                    (*pEventsNumber)++;
                    pEventsRegionInfo[*pEventsNumber - 1]
						= pTmpEventsRegionInfo[i];
                }
                else
                {
                    pEventsRegionInfo[*pEventsNumber - 1].iEnd
						= pTmpEventsRegionInfo[i].iEnd;
                    pEventsRegionInfo[*pEventsNumber - 1].iStyle = 2;//该类事件为反射事件
                }

                iState = 1;
            }
            else
            {
                if (pTmpEventsRegionInfo[i].iBegin - pTmpEventsRegionInfo[i - 1].iBegin > blind / 2)
                {
                    (*pEventsNumber)++;
                    pEventsRegionInfo[*pEventsNumber - 1]
						= pTmpEventsRegionInfo[i];
                }
                else
                {
                    pEventsRegionInfo[*pEventsNumber - 1].iEnd
						= pTmpEventsRegionInfo[i].iEnd;
                }
            }
            break;
        }
        default:
        {
            iState = 0;
            break;
        }
        }
    }

	if (level == 3)
	{
		int k = 0;
		if (pEventsRegionInfo[0].iBegin == 0)
		{
			iEndpointindex[k][1] = Min(pEventsRegionInfo[0].iEnd,
				(int)Max(20 * pAlgorithmInfo->SetParameters.fSampleRating, 3 * iBlind));
			iEndpointindex[k][1] = Min(iEndpointindex[k][1],
				(int)(1200 * pAlgorithmInfo->SetParameters.fSampleRating));
			i = 1;
			k++;
		} 
		else
		{
			i = 0;
			if (pEventsRegionInfo[0].iBegin > Min(iBlind, 128));
			{
				iEndpointindex[k][1] = Min(pEventsRegionInfo[0].iEnd,
					(int)Max(20 * pAlgorithmInfo->SetParameters.fSampleRating, 3 * iBlind));
				iEndpointindex[k][1] = Min(iEndpointindex[k][1],
					(int)(1200 * pAlgorithmInfo->SetParameters.fSampleRating));
				i = 1;
				k++;
			}
		}
		for (; i < *pEventsNumber; ++i)
		{
			if (pEventsRegionInfo[i].iStyle == 2)
			{
				iEndpointindex[k][0] = pEventsRegionInfo[i].iBegin;
				iEndpointindex[k][1] = pEventsRegionInfo[i].iEnd;
				k++;
			}
			if (k >= 49)
			{
				break;
			}
		}
	}
    // 合并起始端2个window前的事件
    int istartRegion = 0;
    for (i = 0; i < *pEventsNumber; ++i)
    {
		if (((pAlgorithmInfo->PulseWidth <= 3)
			&& (pEventsRegionInfo[i].iBegin <= Min(iPointNum3, 2 * window)))
            || ((pEventsRegionInfo[i].iStyle == 0)
            && (pEventsRegionInfo[i].iBegin < Max(iPointNum3, window))))
		{
			istartRegion = i;
            pEventsRegionInfo[i].iBegin = 0;
            pEventsRegionInfo[i].iStyle = 2;
		}
        else if (((pAlgorithmInfo->PulseWidth > 3)
			&& (pEventsRegionInfo[i].iBegin <= Max(Min(blind + iPointNum, 2 * window), blind)))
            || ((pEventsRegionInfo[i].iStyle == 0)
            && (pEventsRegionInfo[i].iBegin < Max(iPointNum, window))))
        {
            istartRegion = i;
            pEventsRegionInfo[i].iBegin = 0;
            pEventsRegionInfo[i].iStyle = 2;
        }
        else
        {
            break;
        }
    }
    if (istartRegion != 0)
    {
        *pEventsNumber = *pEventsNumber - istartRegion;
        for (i = 0; i<*pEventsNumber; i++)
            pEventsRegionInfo[i] = pEventsRegionInfo[i + istartRegion];
    }

    // 添加第一个事件，S点
    pEventsRegionTable->iEventsNumber += 1;
    for (i = pEventsRegionTable->iEventsNumber; i > 0; --i)
    {
        pEventsRegionInfo[i] = pEventsRegionInfo[i - 1];
    }
    pEventsRegionInfo[0].iBegin = 0;
    pEventsRegionInfo[0].iEnd = blind;
    pEventsRegionInfo[0].iStyle = 2;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("---0--iSaveNumber = %d------\n", pEventsRegionTable->iEventsNumber);
	#if ISSERIAL == 1
		fprintf(test_file_name, "---0--iSaveNumber = %d------\n", pEventsRegionTable->iEventsNumber);
	#endif
#endif

    //判断末端事件是否是由小波变换导致的误报
    int pSaveIndex[MAX_EVENTS_NUMBER] = { 0 };
    int iSaveNumber = 0;

    for (i = 0; i < pEventsRegionTable->iEventsNumber; ++i)
    {
        if ((pEventsRegionInfo[i].iEnd < pAlgorithmInfo->iDisplayLenth - (int)(window / 2))
            && (pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i].iBegin
            >= Max(Min(window / 8, iBlind / 4), 1)))
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
        }
    }
    for (i = 0; i < iSaveNumber; ++i)
    {
        pEventsRegionTable->EventsRegionInfo[i]
			= pEventsRegionTable->EventsRegionInfo[pSaveIndex[i]];
    }
    pEventsRegionTable->iEventsNumber = iSaveNumber;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("---1--iSaveNumber = %d------\n", iSaveNumber);
	#if ISSERIAL == 1
		fprintf(test_file_name, "---1--iSaveNumber = %d------\n", iSaveNumber);
	#endif
#endif

    ///将事件窗口赋值
    for (i = 0; i < *pEventsNumber; ++i)
    {
        pEventsRegionInfo[i].iWindow = window;
    }

    GetEventsMaxMinIndex(pInputSignal, pEventsRegionTable);

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------SearchRegion_2--------1---------------\n\n");
	ALG_DEBUG("-----iWindow = %d------\n", pEventsRegionTable->EventsRegionInfo[0].iWindow);
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------SearchRegion_2--------1---------------\n\n");
		fprintf(test_file_name, "--------iWindow = %d-----------\n", pEventsRegionTable->EventsRegionInfo[0].iWindow);
		PrintEventRegionInfo(test_file_name, pEventsRegionTable);
	#else
		PrintEventRegionInfo(stdout, pEventsRegionTable);
	#endif
	//PrintEventRegionInfo(stdout, pEventsRegionTable);
	ALG_DEBUG("\n\n\n");
#endif

    // 删除分辨率不高的事件
    pSaveIndex[0] = 0;
    iSaveNumber = 1;
    for (i = 1; i<pEventsRegionTable->iEventsNumber; ++i)
    {
        if (i == pEventsRegionTable->iEventsNumber - 1)
        {
            if (pEventsRegionInfo[i].iMaxIndex 
				- pEventsRegionInfo[i - 1].iMaxIndex > window / 2)
            {
                pSaveIndex[iSaveNumber] = i;
                iSaveNumber++;
            }
            continue;
        }
        if ((pEventsRegionInfo[i + 1].iMaxIndex 
			- pEventsRegionInfo[i].iMaxIndex > window / 2)
            && (pEventsRegionInfo[i].iMaxIndex
				- pEventsRegionInfo[i - 1].iMaxIndex > window / 2))
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
			continue;
        }
		if ((pEventsRegionInfo[i].iMaxIndex - pEventsRegionInfo[i - 1].iMaxIndex <= window / 2)
			&& (level == 3) && (pEventsRegionInfo[i].iStyle == pEventsRegionInfo[i - 1].iStyle)
			&& (pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i - 1].iBegin <= window)
			&& (pSaveIndex[iSaveNumber - 1] != i - 1))
		{
			pSaveIndex[iSaveNumber] = i;
			iSaveNumber++;
			if (pInputSignal[pEventsRegionInfo[i].iMaxIndex]  < pInputSignal[pEventsRegionInfo[i - 1].iMaxIndex])
			{
				pEventsRegionInfo[i].iMaxIndex = pEventsRegionInfo[i - 1].iMaxIndex;
			}
			if (pInputSignal[pEventsRegionInfo[i].iMinIndex] > pInputSignal[pEventsRegionInfo[i - 1].iMinIndex])
			{
				pEventsRegionInfo[i].iMinIndex = pEventsRegionInfo[i - 1].iMinIndex;
			}
			pEventsRegionInfo[i].iBegin = pEventsRegionInfo[i - 1].iBegin;
			continue;
		}
    }
    for (i = 0; i < iSaveNumber; ++i)
    {
        pEventsRegionTable->EventsRegionInfo[i]
			= pEventsRegionTable->EventsRegionInfo[pSaveIndex[i]];
    }
    pEventsRegionTable->iEventsNumber = iSaveNumber;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("---2--iSaveNumber = %d------\n", iSaveNumber);
	#if ISSERIAL == 1
		fprintf(test_file_name, "---2--iSaveNumber = %d------\n", iSaveNumber);
	#endif
#endif
	
	//若事件区间范围过小，则认为其为毛刺噪声，不可靠事件，将其删除
    pSaveIndex[0] = 0;
    iSaveNumber = 1;
    for (i = 1; i<pEventsRegionTable->iEventsNumber; ++i)
    {
        if (((pEventsRegionInfo[i].iStyle == 2)
			&& (pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i].iBegin >= Min(blind, window / 4)))
            || ((pEventsRegionInfo[i].iStyle < 2)
            && (pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i].iBegin
			>= Min(Max(blind / 2, window / 8), (int)(1.6f * iPointNum)))))
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
        }
    }
    for (i = 0; i < iSaveNumber; ++i)
    {
        pEventsRegionTable->EventsRegionInfo[i]
			= pEventsRegionTable->EventsRegionInfo[pSaveIndex[i]];
    }
    pEventsRegionTable->iEventsNumber = iSaveNumber;
    
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("---3--iSaveNumber = %d------\n", iSaveNumber);
	#if ISSERIAL == 1
		fprintf(test_file_name, "---3--iSaveNumber = %d------\n", iSaveNumber);
	#endif
#endif

	// 若获得该事件的小波窗口大于盲区的4倍（blind * 4）,则删除该事件;
    // 若获得该事件的小波窗口小于盲区的1/4（blind/4）,则删除该事件
    pSaveIndex[0] = 0;
    iSaveNumber = 1;
    for (i = 1; i<pEventsRegionTable->iEventsNumber; ++i)
    {
        if ((pEventsRegionInfo[i].iStyle == 2) 
			|| ((pEventsRegionInfo[i].iStyle < 2) && ((window > blind / 4)
				|| (window >= 64 && pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i].iBegin >= window / 2)))
            || (pInputSignal[pEventsRegionInfo[i].iMaxIndex] > 2)
            || (pInputSignal[pEventsRegionInfo[i].iMinIndex] < -2))
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
        }
    }
    for (i = 0; i < iSaveNumber; ++i)
    {
        pEventsRegionTable->EventsRegionInfo[i]
			= pEventsRegionTable->EventsRegionInfo[pSaveIndex[i]];
    }
    pEventsRegionTable->iEventsNumber = iSaveNumber;

    //若该事件长度大于3*window,删除该事件
    pSaveIndex[0] = 0;
    iSaveNumber = 1;
    for (i = 1; i < pEventsRegionTable->iEventsNumber; ++i)
    {
        if ((pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i].iBegin
				> Max((int)(3.5f * window), 3 * blind))
            && (pEventsRegionInfo[i].iStyle == 0)
			&& (i < pEventsRegionTable->iEventsNumber - 1))
        {
        }
        else
        {
            pSaveIndex[iSaveNumber] = i;
            iSaveNumber++;
        }
    }
    for (i = 0; i < iSaveNumber; ++i)
    {
        pEventsRegionTable->EventsRegionInfo[i]
			= pEventsRegionTable->EventsRegionInfo[pSaveIndex[i]];
    }
    pEventsRegionTable->iEventsNumber = iSaveNumber;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------SearchRegion_2--------------2---------\n\n");
	ALG_DEBUG("-----iWindow = %d------\n", pEventsRegionTable->EventsRegionInfo[0].iWindow);
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------SearchRegion_2--------2---------------\n\n");
		fprintf(test_file_name, "--------iWindow = %d-----------\n", pEventsRegionTable->EventsRegionInfo[0].iWindow);
		PrintEventRegionInfo(test_file_name, pEventsRegionTable);
	#else
		PrintEventRegionInfo(stdout, pEventsRegionTable);
	#endif
	//PrintEventRegionInfo(stdout, pEventsRegionTable);
	ALG_DEBUG("\n\n\n");
#endif

    free(pTmpEventsRegionTable);
}

void SearchEdge2(
	float *pInputSignal,     				   // 归一化后的小波信号
	const int iWindow,       				   // 窗口大小
	EVENTS_REGION_TABLE *pEventsRegionTable,   // 事件数组
	int level,				 				   // 小波层级
	ALGORITHM_INFO *pAlgorithmInfo)
{
	int i = 0;
	int iState = 0; //状态机状态变量
	int iUpCount = 0;//上升沿计数
	int iDownCount = 0;//下降沿计数

	float fTmpLength = 0.0f;

	pEventsRegionTable->iEventsNumber = 0;

	float distancePerPoint = pAlgorithmInfo->fPointLength;
	int signalLength = pAlgorithmInfo->iSignalLength;

	int iTmpTail = SearchEnd(pAlgorithmInfo->pSignalLog,
		signalLength, pAlgorithmInfo->fMaxInputNoise + 1.0f);

#ifdef CHECKSNR
	float* waveCoef = (float*)malloc(sizeof(float) * signalLength);
#endif

	// 防止在同一个事件中阈值发生跳变
	int keepFlag = 0;
	float keepThresh = 0.0f;
	float referValue = 0.0f;

	SNR_KEY_POINT* snrKeyPoint = GetEffectiveFiberRange_mini2(pAlgorithmInfo);
	pAlgorithmInfo->solaEffectiveRange
		= snrKeyPoint->keyPointDistance[snrKeyPoint->keyPointNum - 1];
	float tempDistance = snrKeyPoint->keyPointDistance[0];

	int tmpLen = 0;
	if ((pAlgorithmInfo->iSOLATest == 1)
		&& (fFiberDist < MIDDLEFIBER)
		&& (frontParament.pulseWidth >= ENUM_PULSE_200NS))
	{
		tmpLen = Max(2000, g_iExSignalLength - signalLength);
	}
	else
	{
		tmpLen = BLACK_LEVEL_LEN;
	}
	float maxThresh = GetMaxAbsFloat(g_pWaveletCoef[level]
		+ Max(g_iExSignalLength - tmpLen - 256, 0), tmpLen) + 2;

	float waveletThresh[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (snrKeyPoint->referValue[0] < 0.03f)
	{
		waveletThresh[0] = 0.20f;
		waveletThresh[1] = 0.15f;
		waveletThresh[2] = 0.20f;
		waveletThresh[3] = 0.20f;
	}
	else if (snrKeyPoint->referValue[0] < 0.04f)
	{
		waveletThresh[0] = 0.20f;
		waveletThresh[1] = 0.15f;
		waveletThresh[2] = 0.20f;
		waveletThresh[3] = 0.70f;
	}
	else if (snrKeyPoint->referValue[0] < 0.08f)
	{
		waveletThresh[0] = 0.35f;
		waveletThresh[1] = 0.35f;
		waveletThresh[2] = 0.85f;
		waveletThresh[3] = 1.20f;
	}
	else if (snrKeyPoint->referValue[0] < 0.20f)
	{
		waveletThresh[0] = 0.5f;
		waveletThresh[1] = 0.6f;
		waveletThresh[2] = 0.7f;
		waveletThresh[3] = 0.95f;
	}
	else
	{
		waveletThresh[0] = maxThresh;
		waveletThresh[1] = maxThresh;
		waveletThresh[2] = maxThresh;
		waveletThresh[3] = maxThresh;
	}
	// 连接分路器时短距离情况下，分路器之乘积小于1:64，目测2us的有效测试距离大于5km
	// 弥补有效距离不准确的情况
	int spliter = pAlgorithmInfo->spliterRatioGrade1
		+ pAlgorithmInfo->spliterRatioGrade2
		+ pAlgorithmInfo->spliterRatioGrade3;

	if ((pAlgorithmInfo->iSOLATest == 1)
		&& (fFiberDist < MIDDLEFIBER) && (spliter <= 6))
	{
		if (spliter < 6)
		{
			if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_1US)
				pAlgorithmInfo->solaEffectiveRange
				= Max(pAlgorithmInfo->solaEffectiveRange, 3000.0f);
			if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_2US)
				pAlgorithmInfo->solaEffectiveRange
				= Max(pAlgorithmInfo->solaEffectiveRange, 5000.0f);
		}
		else
		{
			if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_1US)
				pAlgorithmInfo->solaEffectiveRange
				= Max(pAlgorithmInfo->solaEffectiveRange, 2000.0f);
			if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_2US)
				pAlgorithmInfo->solaEffectiveRange
				= Max(pAlgorithmInfo->solaEffectiveRange, 2500.0f);
		}
		snrKeyPoint->keyPointDistance[snrKeyPoint->keyPointNum - 1]
			= pAlgorithmInfo->solaEffectiveRange;
	}

	pEventsRegionTable->iEventsNumber = 0;
	referValue = waveletThresh[level];

	int searchLen = signalLength - BLACK_LEVEL_LEN;
	for (i = 0; i < searchLen; ++i)
	{
		fTmpLength = distancePerPoint * i;

		float fTmpThr = GetGoodSnrWaveCoef2(level, waveletThresh[level],
			fTmpLength, referValue, maxThresh, snrKeyPoint);
		fTmpThr = MeasureLimitation(fTmpLength, level, maxThresh, fTmpThr);

		referValue = fTmpThr;

		if ((pAlgorithmInfo->iSOLATest == 1) && (fFiberDist < SHROTFIBER)
			&& (spliter >= 6) && (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_200NS))
		{
			if (level == 1)
			{
				if (fTmpLength > tempDistance)
					fTmpThr = Max(fTmpThr, maxThresh);
			}
		}

		// 5ns特殊处理
		if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_5NS)
		{
			if (level == 3)
				fTmpThr = 10.0f;
			else
				fTmpThr = Max(maxThresh, 5.0f);
		}

		if ((pAlgorithmInfo->iSOLATest == 0)
			&& (pAlgorithmInfo->PulseWidth == ENUM_PULSE_10NS))
		{
			if (level <= 2)
				fTmpThr += 0.4f;
		}
		//2us的误报，特殊处理
		if ((pAlgorithmInfo->iSOLATest == 1)
			&& (pAlgorithmInfo->PulseWidth == ENUM_PULSE_2US)
			&& (fFiberDist < 1.5f * MIDDLEFIBER))
		{
			if (level >= 2)
				fTmpThr += 0.4f;  // 0.5->0.4
		}

		// 10ns特殊处理
		if ((pAlgorithmInfo->iSOLATest == 1)
			&& (pAlgorithmInfo->PulseWidth == ENUM_PULSE_10NS)
			&& (spliter == 0))
		{
			if (level >= 2)
				fTmpThr += 0.4f;
		}

		// 10ns特殊处理
		if ((pAlgorithmInfo->iSOLATest == 1)
			&& (pAlgorithmInfo->PulseWidth == ENUM_PULSE_20NS)
			&& (spliter != 0))
		{
			if (level >= 2)
				fTmpThr += 0.1f;
		}

		if (i > iTmpTail)
		{
			fTmpThr = Max(fTmpThr, maxThresh);
		}
		if ((pAlgorithmInfo->PulseWidth > ENUM_PULSE_200NS) && (level < 2))
		{
			fTmpThr = fTmpThr + 1.0f + level;
		}
		else if (pAlgorithmInfo->iSOLATest == 1)
		{
			fTmpThr = fTmpThr + 0.2f + (1.5f - level / 2.0f);
		}

		if (keepFlag == 1)
		{
			fTmpThr = keepThresh;
		}
		//referValue = fTmpThr;

#ifdef CHECKSNR
		waveCoef[i] = fTmpThr;
#endif
		switch (iState)
		{
		case 0:
		{
			iUpCount = 0;
			iDownCount = 0;
			if (pInputSignal[i] > fTmpThr)
			{
				pEventsRegionTable->iEventsNumber++;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 0;
				keepFlag = 1;
				keepThresh = fTmpThr;
				iState = 1;
			}
			if (pInputSignal[i] < -fTmpThr)
			{
				pEventsRegionTable->iEventsNumber++;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 1;
				keepFlag = 1;
				keepThresh = fTmpThr;
				iState = 2;
			}
			break;
		}
		case 1:
		{
			if (pInputSignal[i] > fTmpThr)
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
			else
			{
				iState = 0;
				keepFlag = 0;
			}

			if (pInputSignal[i] < -fTmpThr)
			{
				pEventsRegionTable->iEventsNumber++;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 1;
				keepFlag = 1;
				keepThresh = fTmpThr;
				iState = 2;
			}
			break;
		}
		case 2:
		{
			if (pInputSignal[i] < -fTmpThr)
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
			else
			{
				keepFlag = 0;
				iState = 0;
			}
			if (pInputSignal[i] > fTmpThr)
			{
				pEventsRegionTable->iEventsNumber++;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
				pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 0;
				keepFlag = 1;
				keepThresh = fTmpThr;
				iState = 1;
			}
			break;
		}
		default:
			iState = 0;
		}
		if (pEventsRegionTable->iEventsNumber > 400)
		{
			break;
		}
	}

#ifdef CHECKSNR 
	PrintWaveLetThresh(waveCoef, signalLength, level, pAlgorithmInfo->PulseWidth);
	free(waveCoef);
#endif

	if ((pEventsRegionTable->iEventsNumber > 1)
		&& ((signalLength - pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin) < iWindow))
	{
		pEventsRegionTable->iEventsNumber--;//将小波变换导致的数据末端误判删除
	}

	free(snrKeyPoint);
}

// 获取短距离小波阈值(特殊处理)
float GetShortFiberWaveCoef(
    int WaveLength,
    int level,
    int Thresh,
    float fSampleRating,
    float Distance,
    float iWindow,
    int iSignalLength,
    float *fsignal)
{
    float WaveletThroffset = 0.0f;
    float WaveletThroffsetcoef = 0.0f;
    if (WaveLength == 0)
    {
        WaveletThroffset = 0.0f;
        WaveletThroffsetcoef = 0.5f;
    }
    else
    {
        WaveletThroffset = 1.2f;
        WaveletThroffsetcoef = 0.3f;
    }

    float fTmpThr = 0.0f;

    switch (level)
    {
    case 0:
    {
        fTmpThr = 1000.0f;
        break;
    }
    case 1:
    {
        if (fSampleRating > 15.0f)
        {
            fTmpThr = Thresh + 1000.0f;
        }
        else
        {
            if (Distance < range)
            {
                if (frontParament.pulseWidth < 3)
                {
                    if (frontParament.pulseWidth == 1)
                        fTmpThr = 4.5f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1); 
                    else
                    {
                        if (Distance < range * 0.75f)
                        {
                            fTmpThr = 0.35f;
                        }
                        else if (Distance < range * 0.95f) 
                        {
                            fTmpThr = 0.55f;
                        }
                        else
                            fTmpThr = 5.0f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1);
                    }
                }
                else if (frontParament.pulseWidth == 3)
                {
                    int i = 0;
                    float fMean = 0.0f;
                    int Len = (int)Min(Max(iWindow, 8), 64);


                    if (i < iSignalLength - 257 - Len);
                    {
                        fMean = GetMeanFloat(fsignal + i, Len);
                    }
                    if (Distance < range * 0.7f)
                    {
                        if (fMean < 6.5f)
                        {
                            fTmpThr = 0.5f;
                        }
                        else
                        {
                            fTmpThr = 0.25f;
                        }
                    }
                    else if (Distance < range * 0.95f)
                    {
                        fTmpThr = 0.55f;
                    }
                    else
                    {
                        fTmpThr = 4.0f;
                    }
                }
                else
                {
                    if (Distance < range * 0.4f)
                        fTmpThr = 0.45f;
                    else if (Distance < range * 0.8f)
                    {
                        if (range < 3000.0f)
                        {
                            fTmpThr = Max(1.6f - (frontParament.pulseWidth - 3) * WaveletThroffsetcoef, 1.2f);
                        }
                        else
                        {
                            fTmpThr = Max(2.8f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 2.3f);
                        }
                    }
                    else if (Distance < range)
                        fTmpThr = Max(6.8f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 6.3f) + 0.3f;

                }
            }
            else
            {
                fTmpThr = Thresh + 1000.0f;
            }
        }
        break;
    }
    case 2:
    {
        if (fSampleRating > 5.0f)
        {
            if (Distance < range)
            {
                if (frontParament.pulseWidth < 3)
                {
                    if (frontParament.pulseWidth == 1)
                        fTmpThr = 10.0f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1);
                    else
                    {
                        if (Distance < range * 0.75f)
                        {
                            fTmpThr = 0.65f;      // 100+100m误报
							if (range < 210.0f)
							{
								fTmpThr = 1.20f; // 改通道以后出现震荡
							}
                        }
                        else if (Distance < range * 0.95f)
                        {
                            fTmpThr = 0.75f;  
							if (range < 210.0f)
							{
								fTmpThr = 1.00f; //改通道以后出现震荡
							}
                        }
                        else
                            fTmpThr = 10.0f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1);
                    }
                }
                else if (frontParament.pulseWidth == 3)
                {
                    if (Distance < range * 0.7f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.35f;
						else
						{
							fTmpThr = 0.45f;
							if (range < 210.0f)
							{
								fTmpThr = 0.65f;
								if (range < 150.0f)
								{
									fTmpThr = 1.0f;
								}
							}
						}
						    
                    }
                    else if (Distance < range * 0.95f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.45f;
						else
						{
							fTmpThr = 0.45f;
							if (range < 210.0f)
							{
								fTmpThr = 0.65f;
							}
						}
                    }
                    else
                    {
                        fTmpThr = 4.0f;
                    }
                }

				else if ((frontParament.pulseWidth == 4)
					&& (frontParament.wave == 1)
					&& (range > 1000.0f) && (range < 3000.0f)
					&& (fSampleRating < 20.0f) && (fSampleRating > 10.0f))
                {
                    if (Distance < range * 0.7f)
                    {
						fTmpThr = 0.42f;						    
                    }
                    else if (Distance < range * 0.95f)
                    {
						fTmpThr = 0.52f;
                    }
                    else
                    {
                        fTmpThr = 3.8f;
                    }
                }

				
                else
                {
                    if (Distance < range * 0.8f)
                        fTmpThr = Max(0.8f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 0.3f);
                    else
                        fTmpThr = Max(0.8f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 0.5f) + 0.6f; // 末端后误报过近，导致末端损耗太小，影响末端定位
                }
            }
            else
            {
                fTmpThr = Thresh + 1000.0f;
            }
        }
        else
        {
            if (Distance < range)
            {
                if (frontParament.pulseWidth < 3)
                {
                    if (frontParament.pulseWidth == 1)
                        fTmpThr = 10.0f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1);
                    else
                    {
                        if (Distance < range * 0.5f)
                        {
                            fTmpThr = 0.35f;
                        }
                        else if (Distance < range * 0.75f)
                        {
                            fTmpThr = 0.45f;
                        }
                        else if (Distance < range * 0.95f)
                        {
                            fTmpThr = 0.55f;
                        }
                        else
                            fTmpThr = 10.0f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1);
                    }
                }
              // 1500波长光较弱，故信噪比差，小波阈值要提高
                else if (frontParament.pulseWidth == 3)
                {

                    if (Distance < range * 0.7f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.35f;
                        else
                            fTmpThr = 0.4f;
                    }
                    else if (Distance < range * 0.95f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.4f;
                        else
                            fTmpThr = 0.55f;
                    }
                    else
                    {
                        fTmpThr = 4.0f;
                    }
                }
                else
                {
                    if (Distance < range * 0.9f)
                        fTmpThr = Max(0.35f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 0.3f);
                    else
                        fTmpThr = Max(0.6f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 0.3f);
                }
            }
            else
                fTmpThr = Thresh + 1000.0f;

        }
        break;
    }
    case 3:
    {
        if (fSampleRating > 5)
        {
            if (Distance < range)
            {
                if (frontParament.pulseWidth < 3)
                {
                    if (frontParament.pulseWidth == 1)
                        fTmpThr = 10.0f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1.5f); 
                    else
                    {
                        if (Distance < range * 0.85f)
                        {
                            fTmpThr = 0.65f; 
                        }
						else
						{
							if (Distance < range * 0.95f)
							{
								if (WaveLength == 0)
								{
									fTmpThr = 0.65f;
									if (range < 210.0f)
									{
										fTmpThr = 0.9f;
									}
								}
								else
								{
									fTmpThr = 0.65f;
								}
							}
							else
								fTmpThr = 10.0f + (4 - frontParament.pulseWidth) * (WaveletThroffsetcoef + 1.25f);
						}
                        
                    }
                }
				else if (frontParament.pulseWidth == 3 && fSampleRating < 20.0f)
                {
                    if (Distance < range * 0.7f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.33f;     // 0.4
						else
						{
							fTmpThr = 0.32f;       // 0.35
							if (range < 210.0f)
							{
								fTmpThr = 1.0f;
								if (Distance < range * 0.35f)
									fTmpThr = 0.45f;               
							}
						}
                    }
					else if (Distance < range * 0.9f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.35f;     // 0.4
						else
						{
							fTmpThr = 0.23f;       // 0.35
							if (range < 210.0f)
							{
								fTmpThr = 1.0f;
								if (Distance < range * 0.35f)
									fTmpThr = 0.45f;               
							}
						}
                    }
                    else
                    {
						if (WaveLength == 0)
						{
							fTmpThr = 0.50f; 
							if (range < 210.0f)
							{
								fTmpThr = 0.55f;       // 0.55
							}
						}
						else
						{
							fTmpThr = 0.50f;                 // 0.55
							if (range < 210.0f)
							{
								fTmpThr = 1.0f;
							}
						}
                    }
                }
				else if (frontParament.pulseWidth == 3 && fSampleRating >= 20.0f)
                {
                    if (Distance < range * 0.7f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.33f;     // 0.4
						else
						{
							fTmpThr = 0.35f;       // 0.35

							if (range < 210.0f)
							{
								fTmpThr = 1.0f;
								if (Distance < range * 0.35f)
									fTmpThr = 0.45f;               
							}
						}
                    }
					else if (Distance < range * 0.9f)
                    {
                        if (WaveLength == 0)
                            fTmpThr = 0.35f;     // 0.4
						else
						{
							fTmpThr = 0.3f;       // 0.35
							if (range < 210.0f)
							{
								fTmpThr = 1.0f;
								if (Distance < range * 0.35f)
									fTmpThr = 0.45f;               
							}
						}
                    }
                    else
                    {
						if (WaveLength == 0)
						{
							fTmpThr = 0.50f; 
							if (range < 210.0f)
							{
								fTmpThr = 0.55f;       // 0.55
							}
						}
						else
						{
							fTmpThr = 0.50f;                 // 0.55
							if (range < 210.0f)
							{
								fTmpThr = 1.0f;
							}
						}
                    }
                }
				else if ((frontParament.pulseWidth == 4)
				&& (frontParament.wave == 1)
				&& (range > 1000.0f) && (range < 3000.0f)
				&& (fSampleRating < 20.0f) && (fSampleRating > 10.0f))
                {
                    if (Distance < range * 0.7f)
                    {
						fTmpThr = 0.22f;       // 0.35
                    }
                    else
                    {
						fTmpThr = 0.40f;                 // 0.55
                    }
                }
                else
                {
                    if (Distance < 0.5f * range)
                        fTmpThr = Max(0.42f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 0.4f);
                    else if (Distance < 0.95f * range)
                        fTmpThr = Max(0.5f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 0.4f);
                    else
                        fTmpThr = Max(4.5f - (frontParament.pulseWidth - 4) * WaveletThroffsetcoef, 0.4f) + 0.15f;
                }
            }
            else
            {
            	if ((frontParament.pulseWidth == 4)
					&& (frontParament.wave == 1) && (range > 1000.0f)
					&& (range < 3000.0f) && (fSampleRating < 20.0f)
					&& (fSampleRating > 10.0f) && (Distance < 1.3f * range))
                {
                	fTmpThr = 0.42f;
        		}
				else
				{
					fTmpThr = Thresh + 1000.0f;
				}
        	}
        }
        else
        {
        	 if ((frontParament.pulseWidth == 4)
				 && (frontParament.wave == 1) 
				 && (range > 1000.0f) && (range < 3000.0f))
             {
                    if (Distance < range * 0.7f)
                    {
						fTmpThr = 0.22f;       // 0.35
                    }
                    else
                    {
						fTmpThr = 0.42f;                 // 0.5
                    }
             }
            else if (Distance < range)
                fTmpThr = 5.5f;
            else
                fTmpThr = Thresh + 1000.0f;
        }
        break;
    }
    }

    if ((frontParament.pulseWidth == 1) && (Distance > range))
    {
        fTmpThr = 1000.0f;
    }

    return fTmpThr;
}

// 获取信噪比较好情况下的小波阈值
float GetGoodSnrWaveCoef(int level, 
	float Thresh,
	float Distance)
{
    float fTmpThr = 0.0f;
    switch (level)
    {
    case 0:
    {
		// 大脉宽大窗口找前面短距离事件严重不准确，故不适用该层级小波阈查找事件
        if (frontParament.pulseWidth > ENUM_PULSE_2US)
            fTmpThr = 10000.0f;
        else
        {
            if (Distance < range * 0.3f)
                fTmpThr = 0.30f;
            else if (Distance < range * 0.5f)
                fTmpThr = 0.75f;
            else if (Distance < range * 0.6f)
                fTmpThr = 1.80f;
            else if (Distance < range * 0.8f)
                fTmpThr = 2.5f;
            else if (Distance < range)
                fTmpThr = Thresh + 20.0f;
            else
                fTmpThr = Thresh + 50.0f;
        }
        break;
    }
    case 1:
    {
        if (Distance < range * 0.3f)
            fTmpThr = 0.3f;
        else if (Distance < range * 0.4f)
            fTmpThr = 0.45f;
        else if (Distance < range * 0.5f)  
            fTmpThr = 0.55f;
        else if (Distance < range * 0.75f)
            fTmpThr = 0.65f;
        else if (Distance < range * 0.98f)
            fTmpThr = 0.80f;
        else if (Distance < range)
            fTmpThr = Thresh + 20.0f;
        else
            fTmpThr = Thresh + 50.0f;
        break;
    }
    case 2:
    {
        if (Distance < range * 0.4f)
            fTmpThr = ((0.20f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.365f) : (0.2f)); 
        else if (Distance < range * 0.61f)
            fTmpThr = ((0.21f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.4f) : (0.21f));  
        else if (Distance < range * 0.7f)
            fTmpThr = ((0.35f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.5f) : (0.35f));
        else if (Distance < range * 0.98f)
            fTmpThr = ((0.60f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.6f) : (0.60f));  
        else if (Distance < range)
            fTmpThr = Thresh + 10.0f;
        else
            fTmpThr = Thresh + 70.0f;
        break;
    }
    case 3:
    {
		// 大脉宽大窗口找前面短距离事件严重不准确，故不适用该层级小波阈查找事件 0.4->0.75
        if ((frontParament.pulseWidth > ENUM_PULSE_2US)
			&& (Distance < 0.4f * range) && (range < 30000.0f))
            fTmpThr = 10000.0f;
        else if (Distance < range * 0.4f)
            fTmpThr = ((0.45f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.3f) : (0.45f)); 
        else if (Distance < range * 0.5f)
            fTmpThr = ((0.50f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.3f) : (0.50f));	
        else if (Distance < range * 0.75f)
            fTmpThr = ((0.65f < ThreshOffset[3] + 0.45f) ? (ThreshOffset[3] + 0.45f) : (0.65f));		 
        else if (Distance < range * 0.95f)
            fTmpThr = ((1.20f < ThreshOffset[3] + 0.55f) ? (ThreshOffset[3] + 0.55f) : (1.20f));
        else if (Distance < range)
            fTmpThr = Thresh + 20.0f;
        else if (Distance < 1.1f * range)
            fTmpThr = Thresh + 100.0f;
        else
            fTmpThr = Thresh + 200.0f;
        break;
    }
    default:
    {
       level = 3;
    }
    }

    // 
    if ((range < 18000.0f)
		&& (frontParament.pulseWidth >= ENUM_PULSE_10US))
    {
        if (level == 0)
        {
            fTmpThr = Max(fTmpThr, 1.8f);
        }
        else if (level == 1)
        {
            fTmpThr = Max(fTmpThr, 1.8f);
        }
        if (level == 2)
        {
            fTmpThr = Max(fTmpThr, 2.0f);
        }
        if (level == 3)
        {
            fTmpThr = Max(fTmpThr, 3.5f);
        }
    }

    return fTmpThr;
}

// 获取信噪比一般的小波系数
float GetNormalSnrWaveCoef(int level,
	float Thresh,
	float Distance,
	float fMaxNoiseValue)
{

    float fTmpThr = 0.0f;
    switch (level)
    {
        case 0:
        {
              if (Distance < range * 0.1f)
                  fTmpThr = 0.25f;
              else if (Distance < range * 0.4f)
                  fTmpThr = 0.40f; 
              else if (Distance < range * 0.5f)
                  fTmpThr = 2.30f;
              else if (Distance < range * 0.7f)
                  fTmpThr = 3.0f;
              else if (Distance < range)
                  fTmpThr = Thresh + 20.0f;
              else
                  fTmpThr = Thresh + 50.0f;
              break;
        }
        case 1:
        {
              if (Distance < range * 0.3f)
                  fTmpThr = 0.20f;
              else if (Distance < range* 0.5f)
                  fTmpThr = 0.25f;
              else if (Distance < range * 0.7f)
                  fTmpThr = 0.70f;
              else if (Distance < range * 0.8f)
                  fTmpThr = 1.30f;
              else if (Distance < range)
                  fTmpThr = Thresh + 20.0f;
              else
                  fTmpThr = Thresh + 50.0f;
			  
			  fTmpThr = Min(fMaxNoiseValue + 4.0f, fTmpThr);
              break;
        }
        case 2:
        {
              if (Distance < range * 0.1f)
              {
                  if (frontParament.pulseWidth == ENUM_PULSE_20US)
                      fTmpThr = 0.20f;
                  else
                      fTmpThr = 0.48f;
              }
              else if (Distance < range * 0.5f)
                  fTmpThr = ((0.40f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.3f) : (0.40f));
              else if (Distance < range * 0.7f)
                  fTmpThr = ((0.60f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.4f) : (0.60f));
              else if (Distance < range * 0.8f)
                  fTmpThr = ((0.80f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.5f) : (0.80f));
              else if (Distance < range * 0.9f)
                  fTmpThr = ((2.30f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.6f) : (2.30f));        
              else if (Distance < range * 0.95f)
                  fTmpThr = Thresh;
              else if (Distance < range)
                  fTmpThr = Thresh + 5.0f;
              else
              {
                  //fTmpThr = Thresh + 70;
                  fTmpThr = Max(Thresh + 10.0f, 3.0f * Thresh);
              }
			  fTmpThr = Min(fMaxNoiseValue + 4.5f, fTmpThr);
              break;
        }
        case 3:
        {
			  // 大脉宽大窗口找前面短距离事件严重不准确，故不适用该层级小波阈查找事件
              if ((frontParament.pulseWidth > ENUM_PULSE_2US) 
				  && (Distance < range * 0.4f) && (range < 30000.0f))
                  fTmpThr = 10000.0f;
              else if (Distance < range * 0.3f)
                  fTmpThr = ((0.80f < ThreshOffset[3] + 0.30f) ? (ThreshOffset[3] + 0.3f) : (0.80f));
              else if (Distance < range * 0.5f) 			  
                  fTmpThr = ((1.00f < ThreshOffset[3] + 0.40f) ? (ThreshOffset[3] + 0.4f) : (1.00f));
			  else if (Distance < range * 0.75f)
                  fTmpThr = ((1.10f < ThreshOffset[3] + 0.50f) ? (ThreshOffset[3] + 0.5f) : (1.10f));
              else if (Distance < range * 0.85f)
                  fTmpThr = ((1.45f < ThreshOffset[3] + 0.60f) ? (ThreshOffset[3] + 0.6f) : (1.45f));
              else if (Distance<range * 0.95f)
                  fTmpThr = ((1.60f < ThreshOffset[3] + 0.80f) ? (ThreshOffset[3] + 0.8f) : (1.60f));
              else if (Distance < range)
                  fTmpThr = 5.0f;
              else if (Distance < 1.2f * range)
                  fTmpThr = Thresh + 100.0f;
              else
                  fTmpThr = 200.0f;

			  fTmpThr = Min(fMaxNoiseValue + 7.0f, fTmpThr);
              break;
        }
        default:
            level = 3;
    }

    return fTmpThr;
}

// 获取分段阈值，自适应计算，非固定阈值
float CalfTmpThrEachSection(int startIndex,
	int SectionTotalLen,
	int SectionNum,
	int level,
	float coef)
{
	float fTmpThr = 0.0f;
	float MaxValue1 = 0.0f;
	float MaxValue2 = 0.0f;
	int i = 0;
	float* MeanMat = (float *)malloc(sizeof(float) * SectionNum);
	float* StdMat = (float *)malloc(sizeof(float) * SectionNum);
	MeanMat[0] = GetMeanFloat(g_pWaveletCoef[level] + startIndex, SectionTotalLen);
	StdMat[0] = GetStdFloat(g_pWaveletCoef[level] + startIndex, SectionTotalLen);
	fTmpThr = MeanMat[0] + coef * StdMat[0];
	MaxValue1 = GetMaxFloat(g_pWaveletCoef[level] + startIndex, SectionTotalLen * SectionNum);
	MaxValue2 = GetMinFloat(g_pWaveletCoef[level] + startIndex, SectionTotalLen * SectionNum);
	MaxValue2 = ((MaxValue2 < 0) ? -MaxValue2: MaxValue1);
	MaxValue1 = Min(MaxValue1, MaxValue2);
	for (i = 1; i < SectionNum; ++i)
	{
		MeanMat[i] = GetMeanFloat(g_pWaveletCoef[level]
			+ startIndex + i * SectionTotalLen, SectionTotalLen);
		StdMat[i] = GetStdFloat(g_pWaveletCoef[level]
			+ startIndex + i * SectionTotalLen, SectionTotalLen);		
	}
	float MeanValue = 0.0f;
	MeanValue = GetMinFloat(MeanMat, SectionNum);
	MeanValue = Max(0.0f, MeanValue);
	float StdValue = 0.0f;
	StdValue = GetMinFloat(StdMat, SectionNum);
	StdValue = Max(0.0f, StdValue);
	fTmpThr = MeanValue + coef * StdValue;
	fTmpThr = Max(fTmpThr, 0.2f);
	fTmpThr = Min(fTmpThr, 20.0f);
	free(MeanMat);
	free(StdMat);
	return fTmpThr;
}

// 获取信噪比一般的小波系数 
void GetNormalSnrWaveConfidenceCoef(int signalLength,
	float distancePerPoint,
	float coef, 
	int level)
{
	int SectionNum = 6;
	int startIndex = 0;
	float Distance = 0.0f;
	int SectionTotalLen = 0;
	int j = 0;
	int TotalSectionNum = 6;
	if (signalLength > 12000)
	{
		TotalSectionNum = 6;
		SectionNum = 6;
	}
	if (frontParament.pulseWidth == ENUM_PULSE_100NS
		&& level >= 2 && signalLength < 10000)
	{
		SectionNum = 12;
	}
	int k = 0;
	float* fTmpThr = (float *)malloc(sizeof(float) * TotalSectionNum);
	float lengthNode[6] = { 0.0f };
	startIndex = 0;
	SectionTotalLen = (int)(range * 1.0f / (float)TotalSectionNum
		/ distancePerPoint / (float)SectionNum);

	for (k = 0; k < TotalSectionNum; ++k)
	{
  		fTmpThr[k] = CalfTmpThrEachSection(startIndex, SectionTotalLen,
			SectionNum, level, coef);
		startIndex = startIndex + (int)(Min((float)signalLength,
			(range / distancePerPoint)) / (float)TotalSectionNum);
		lengthNode[k] = (float)k * range / TotalSectionNum;
		/*
		printf("startIndex[%d] = %f, fTmpThr[%d] = %f, lengthNode[%d] = %f\n",
			k, startIndex * distancePerPoint, k, fTmpThr[k], k, lengthNode[k]);
			*/
	}

	for (j = 0; j < signalLength; ++j)
	{
		Distance = j * distancePerPoint;
		if (Distance <= range && Distance >= lengthNode[TotalSectionNum - 1])
		{
			AdptThreshold[level][j] = fTmpThr[TotalSectionNum - 1];
			continue;
		}
		else if (Distance > range)
		{
			AdptThreshold[level][j] = 1000.0f;
			continue;
		}
		for (k = 1; k < TotalSectionNum; ++k)
		{
			if (Distance < lengthNode[k] && Distance >= lengthNode[k - 1])
			{
				AdptThreshold[level][j] = fTmpThr[k - 1];
				break;
			}
			/*
			if (Distance < range * (float)k / (float)TotalSectionNum 
				&& Distance >= range * (float)(k - 1) / (float)TotalSectionNum)
          	{
				AdptThreshold[level][j] = fTmpThr[k - 1];
				break;
			} 
	        */
		}
	}
	free(fTmpThr);
}

// 末端查找
void SearchEdge(
    float *pInputSignal,     				//归一化后的小波信号
    const float fThr,        				//检测阈值
    const int iWindow,       				//窗口大小
    EVENTS_REGION_TABLE *pEventsRegionTable,//事件数组
    int level,				 				//小波层级
    ALGORITHM_INFO *pAlgorithmInfo)
{
    int i = 0;
    int iState = 0; //状态机状态变量
    int iUpCount = 0;//上升沿计数
    int iDownCount = 0;//下降沿计数
    float fTmpThr = fThr;
    float fTmpLength = 0.0f;
    pEventsRegionTable->iEventsNumber = 0;
    float distancePerPoint = pAlgorithmInfo->fPointLength;
    int signalLength = pAlgorithmInfo->iSignalLength;
    int waveLength = pAlgorithmInfo->SetParameters.iwavelength;
    int pulseWidth = pAlgorithmInfo->PulseWidth;
	float coef = 8.0f;

#ifdef SAVEDATA
    float* waveCoef = (float *)malloc(sizeof(float) * signalLength);
#endif

    // 防止在同一个事件中阈值发生跳变
    int keepFlag = 0;
    float keepThresh = 0.0f;
	
	float fMaxNoiseValue = GetMaxAbsFloat(pInputSignal + signalLength
		- BLACK_LEVEL_LEN - g_pWaveletWindow[level], BLACK_LEVEL_LEN);
	float fMaxNoiseValue2 = GetMaxAbsFloat(pInputSignal + signalLength
		- 2 * BLACK_LEVEL_LEN - g_pWaveletWindow[level], 2 * BLACK_LEVEL_LEN);

	if (level <= 2)
	{
		coef = 20.0f;
	}
	
	if (iFlag == 1 && range / pAlgorithmInfo->fPointLength > 100.0f)
	{
		GetNormalSnrWaveConfidenceCoef(signalLength, distancePerPoint, coef, level);
		if (level == 3 && pAlgorithmInfo->PulseWidth == 2)
		{
			for (i = 0; i < signalLength; ++i)
			{
				AdptThreshold[level][i] += 0.2f;
			}
		}
	}
	
	int seachLen = signalLength - BLACK_LEVEL_LEN;
    for (i = 0; i < seachLen; ++i)
    {
        fTmpThr = 0.0f;
        fTmpLength = distancePerPoint * i;
        if (iFlag == 1 && range / pAlgorithmInfo->fPointLength > 100.0f)
        {
           // fTmpThr = GetShortFiberWaveCoef(waveLength, level, fThr, sampleRating,
            //    fTmpLength, iWindow, signalLength, pAlgorithmInfo->pSignalLog);
             fTmpThr = AdptThreshold[level][i];
        }
        else if ((iFlag == 0) && (((range < 88000.0f)
			&& (pulseWidth > ENUM_PULSE_10US) && (waveLength == 1))
            || ((range < 70000.0f) && (waveLength == 0) && (pulseWidth == ENUM_PULSE_20US))
            || ((range < 60000.0f) && (pulseWidth > ENUM_PULSE_2US))
			|| ((range < 18000.0f) && (pulseWidth > ENUM_PULSE_200NS))
            || ((range < 10000.0f) && (pulseWidth >= ENUM_PULSE_200NS))
			|| ((range < 40000.0f) && (pulseWidth > ENUM_PULSE_500NS))))
        {
            fTmpThr = GetGoodSnrWaveCoef(level, fThr, fTmpLength);
			if (range > 10000.0f && range < 15000.0f && pulseWidth == ENUM_PULSE_2US)
			{
				fTmpThr += 0.25f;
			}
        }
        else if ((iFlag == 0) && (range > 70000.0f) && (pulseWidth < ENUM_PULSE_10US))
        {
            switch (level)
            {
                case 0:
                {
                      if (fTmpLength < range * 0.1f)
                          fTmpThr = 0.20f;
                      else if (fTmpLength < range * 0.4f)
                          fTmpThr = 0.30f;
                      else if (fTmpLength < range * 0.5f)
                          fTmpThr = 2.30f;
                      else if (fTmpLength < range)
                          fTmpThr = 15.0f;
                      else
                          fTmpThr = fThr + 50.0f;
					  fTmpThr = Min(fMaxNoiseValue + 6.0f, fTmpThr);
                      break;
                }
                case 1:
                {
                      if (fTmpLength < range * 0.3f)
                          fTmpThr = 0.30f;
                      else if (fTmpLength < range * 0.5f)
                          fTmpThr = 0.40f;
                      else if (fTmpLength < range * 0.6f)
                          fTmpThr = 2.50f;
                      else if (fTmpLength < range)
                          fTmpThr = 15.0f;
                      else
                          fTmpThr = fThr + 50.0f;
					  fTmpThr = Min(fMaxNoiseValue + 7.0f, fTmpThr);
                      break;
                }
                case 2:
                {
                      if (fTmpLength < range * 0.1f)
                          fTmpThr = ((0.48f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.3f) : (0.48f));
                      else if (fTmpLength < range * 0.4f)
                          fTmpThr = ((0.60f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.4f) : (0.60f));
                      else if (fTmpLength < range * 0.5f)
                          fTmpThr = ((0.70f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.5f) : (0.70f));
                      else if (fTmpLength < range * 0.6f)
                          fTmpThr = ((2.50f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.6f) : (2.50f));
                      else if (fTmpLength < range * 0.9f)
                          fTmpThr = ((3.50f < ThreshOffset[2] + 0.15f) ? (ThreshOffset[2] + 0.6f) : (3.50f));
                      else if (fTmpLength < range)
                          fTmpThr = 20.0f;
                      else
                          fTmpThr = fThr + 70.0f;
					  //fTmpThr = Min(fMaxNoiseValue + 8.0f, fTmpThr);
					  fTmpThr = Min(fMaxNoiseValue + 10.0f, fTmpThr);
                      break;
                }
                case 3:
                {
                      if (fTmpLength < range * 0.3f)
                          fTmpThr = ((1.00f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.3f) : (1.00f));
                      else if (fTmpLength < range * 0.4f)
                          fTmpThr = ((1.80f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.4f) : (1.80f)); //2.30f
                      else if (fTmpLength < range * 0.60f)
                          fTmpThr = ((2.50f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.5f) : (2.50f)); //3.0f
                      else if (fTmpLength < range * 0.95f)
                          fTmpThr = ((4.20f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.6f) : (4.20f)); //5.50f
                      else if (fTmpLength < range)
                          fTmpThr = fThr + 15.0f;
                      else if (fTmpLength < 1.2f * range)
                          fTmpThr = fThr + 100.0f;
                      else
                          fTmpThr = 200.0f;
					  //fTmpThr = Min(fMaxNoiseValue + 10.0f, fTmpThr);
					  fTmpThr = Min(fMaxNoiseValue + 15.0f, fTmpThr);
                      break;
                }
                default:
                    level = 3;
            }

        }
        else if (iFlag == 0)
        {
            fTmpThr = GetNormalSnrWaveCoef(level, fThr, fTmpLength, fMaxNoiseValue);

            // 120km极限距离(1550ns)
            if ((fTmpLength > 110000.0f) && (fTmpLength < range)
				&& (pulseWidth == ENUM_PULSE_20US))
            {
                fTmpThr = Max(5.0f, fTmpThr);
            }

            if ((fTmpLength > 105000.0f) && (fTmpLength < range) 
				&& (pulseWidth == ENUM_PULSE_10US))
            {
                fTmpThr = Max(5.0f, fTmpThr);
            }

            // 70km极限距离(1310ns)
            if ((fTmpLength > 63000.0f) && (fTmpLength < range)
				&& ((pulseWidth == ENUM_PULSE_10US)
                || (pulseWidth == ENUM_PULSE_2US)) && (waveLength == 0))
            {
                fTmpThr = Max(4.5f, fTmpThr);
            }

			if ((fTmpLength >= 0.5f * range) && (pulseWidth >= ENUM_PULSE_50NS)
				&& (i < signalLength - 10) && (i > 10) && (fTmpLength <= range))
			{
				if (g_pSignalLog[i] < pAlgorithmInfo->fMaxInputNoise + 5.5f)
				{
					float fTmpMean = GetMeanFloat(g_pSignalLog + i - 10, 20);
					fTmpThr = Max(fTmpThr, fTmpThr + 1.2f * (pAlgorithmInfo->fMaxInputNoise + 5.5f - fTmpMean));
				}
			}
        }

        // sola和otdr共同使用有效距离
        if (((pAlgorithmInfo->solaEffectiveRange < range) && (fTmpLength < range)
            && ((fTmpLength > pAlgorithmInfo->solaEffectiveRange) || ((range > 50000.0f)
            && (fTmpLength > pAlgorithmInfo->solaEffectiveRange - 2000.0f))))
            && ((pAlgorithmInfo->iSOLATest == 0) && (range > 2000.0f)
				&& (frontParament.pulseWidth >= ENUM_PULSE_20NS)))
        {
            float distance = range - pAlgorithmInfo->solaEffectiveRange;
            if (frontParament.pulseWidth == ENUM_PULSE_2US)
            {
                if (fTmpLength < pAlgorithmInfo->solaEffectiveRange + distance / 3.0f)
                {
                    fTmpThr += 3.0f;
                }
                else if (fTmpLength < pAlgorithmInfo->solaEffectiveRange + 2.0f * distance / 3.0f)
                {
                    fTmpThr += 7.0f;
                }
                else
                {
                    fTmpThr += 15.0f;
                }
            }
            else if (frontParament.pulseWidth >= ENUM_PULSE_10US)
            {
                if (fTmpLength < pAlgorithmInfo->solaEffectiveRange + distance / 3.0f)
                {
                    fTmpThr += 4.0f;
                }
                else if (fTmpLength < pAlgorithmInfo->solaEffectiveRange + 2.0f * distance / 3.0f)
                {
                    fTmpThr += 8.0f;
                }
                else
                {
                    fTmpThr += 15.0f;
                }
            }
            else
            {
                if (level == 0)
                    fTmpThr = Max(1.5f, fTmpThr);
                else if (level == 1)
                    fTmpThr = Max(2.5f, fTmpThr);
                else if (level == 2)
                    fTmpThr = Max(8.0f, fTmpThr);
                else
                    fTmpThr = Max(15.0f, fTmpThr);
            }
        }

        if (frontParament.pulseWidth == ENUM_PULSE_5NS)
        {
        	if (fTmpLength < Min(25.0f, 0.1f * range))
        	{
				if (level == 3)
					fTmpThr = fMaxNoiseValue2 + 1.5f;
                else
                    fTmpThr = 4.0f;
        	}
            else if (fTmpLength < 0.5f * range)
            {
				if (level == 3)
				{
					fTmpThr = fMaxNoiseValue2 + 1.5f;
					if (fTmpLength > 0.35f * range)
						fTmpThr = fMaxNoiseValue2 + 6.5f;
				}
                else
                    fTmpThr = 4.5f;  // 5.5->4.5
            }
            else if (fTmpLength < range)
            {
                if (level == 3)
                    fTmpThr = 30.0f;
                else
                    fTmpThr = 7.0f;  // 8->7
            }
            else
            {
                fTmpThr = 1000.0f;
            }
        }
		
		fTmpThr = Min(fMaxNoiseValue2 + 15.0f, fTmpThr);
		fTmpThr = Min(fTmpThr, fMaxNoiseValue2 * 2.0f);
		if (((frontParament.pulseWidth == ENUM_PULSE_100NS)
			|| (frontParament.pulseWidth == ENUM_PULSE_50NS)) && range < 200.0f)
		{
			fTmpThr = Min(fMaxNoiseValue2 + 7.5f, fTmpThr);
		}
		if ((frontParament.pulseWidth == ENUM_PULSE_5NS)
			&& (range < 50.0f) && (waveLength == ENUM_WAVE_1550NM))
		{
			fTmpThr = Min(fMaxNoiseValue2 + 8.0f, fTmpThr);
		}
		
		fTmpThr = Min(fTmpThr, 1000.0f);

        if (keepFlag == 1)
        {
            fTmpThr = keepThresh;
        }
		
#ifdef SAVEDATA
        waveCoef[i] = fTmpThr;
#endif
        switch (iState)
        {
            case 0:
            {
                  iUpCount = 0;
                  iDownCount = 0;
                  if (pInputSignal[i] > fTmpThr)
                  {
                      pEventsRegionTable->iEventsNumber++;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 0;
                      keepFlag = 1;
                      keepThresh = fTmpThr;
                      iState = 1;
                  }
                  if (pInputSignal[i] < -fTmpThr + ThreshOffset[level] / 2.0f)
                  {
                      pEventsRegionTable->iEventsNumber++;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 1;
                      keepFlag = 1;
                      keepThresh = fTmpThr;
                      iState = 2;
                  }
                  break;
            }
            case 1:
            {

                  if (pInputSignal[i] > fTmpThr)
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
                  else
                  {
                      iState = 0;
                      keepFlag = 0;
                  }

                  if (pInputSignal[i] < -fTmpThr + ThreshOffset[level] / 2.0f)
                  {
                      pEventsRegionTable->iEventsNumber++;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 1;
                      keepFlag = 1;
                      keepThresh = fTmpThr;
                      iState = 2;
                  }
                  break;
            }
            case 2:
            {
                  if (pInputSignal[i] < -fTmpThr + ThreshOffset[level] / 2.0f)
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
                  else
                  {
                      keepFlag = 0;
                      iState = 0;
                  }
                  if (pInputSignal[i] > fTmpThr)
                  {
                      pEventsRegionTable->iEventsNumber++;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iEnd = i;
                      pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iStyle = 0;
                      keepFlag = 1;
                      keepThresh = fTmpThr;
                      iState = 1;
                  }
                  break;
            }
            default:
                iState = 0;
        }
        if (pEventsRegionTable->iEventsNumber > 400)
        {
            break;
        }
    }
	if ((pEventsRegionTable->iEventsNumber > 80 && level < 3)
		|| (pEventsRegionTable->iEventsNumber > 20 && level == 0))
	{
		pEventsRegionTable->iEventsNumber = 0;
	}

    if ((pEventsRegionTable->iEventsNumber > 1) &&
        ((signalLength - pEventsRegionTable->EventsRegionInfo[pEventsRegionTable->iEventsNumber - 1].iBegin) < iWindow))
    {
        pEventsRegionTable->iEventsNumber--;//将小波变换导致的数据末端误判删除
    }

#ifdef SAVEDATA
	for (i = seachLen; i < signalLength; ++i)
	{
		waveCoef[i] = waveCoef[i - 1];
	}
#endif
	
	
#ifdef DEBUG_EVENTINFO	
	#if ISSERIAL == 1
		fprintf(test_file_name, "----SearchEdge---iWindow = %d--\n\n", iWindow);
		PrintEventRegionInfo(test_file_name, pEventsRegionTable);
	#else
		PrintEventRegionInfo(stdout, pEventsRegionTable);
	#endif
#endif

#ifdef SAVEDATA
	PrintWaveLetThresh(waveCoef, signalLength, level, pulseWidth);
    free(waveCoef);
	waveCoef = NULL;
#endif
}

// 将不同小波域上的事件合并起来
void RegionIntegration(
    EVENTS_REGION_TABLE *pAnsEventsTable, 
    EVENTS_REGION_TABLE *pInputEventsTable)//事件数组;
{
    int i = 0;
    int j = 0;
    int flag = 0;
	
	int iBlind = pAnsEventsTable->iBlind;

    //防止事件数量过多，影响后面事件的添加
    if (pAnsEventsTable->iEventsNumber > 480)
    {
        pAnsEventsTable->iEventsNumber = 480;
    }
    //先删除ansEventsTable中不准确的数据;
    for (i = 0; i < pAnsEventsTable->iEventsNumber; ++i)
    {
        for (j = 0; j < pInputEventsTable->iEventsNumber; ++j)
        {
            if ((pAnsEventsTable->EventsRegionInfo[i].iBegin
				< pInputEventsTable->EventsRegionInfo[j].iEnd)
                && (pAnsEventsTable->EventsRegionInfo[i].iEnd
					>= pInputEventsTable->EventsRegionInfo[j].iBegin))
            {
				if ((pAnsEventsTable->EventsRegionInfo[i].iStyle < 2)
					&& (pAnsEventsTable->EventsRegionInfo[i].iStyle
						== pInputEventsTable->EventsRegionInfo[j].iStyle)
					&& (pAnsEventsTable->EventsRegionInfo[i].iWindow <= (int)(2.0f * iBlind))
					&& (pAnsEventsTable->EventsRegionInfo[i].iWindow 
						<= pInputEventsTable->EventsRegionInfo[j].iWindow)
					&& (pAnsEventsTable->EventsRegionInfo[i].iBegin
						!= pInputEventsTable->EventsRegionInfo[j].iBegin))
            	{
            		pAnsEventsTable->EventsRegionInfo[i]
						= pInputEventsTable->EventsRegionInfo[j];
                    break;

            	}
                else if ((pAnsEventsTable->EventsRegionInfo[i].iBegin
					< pInputEventsTable->EventsRegionInfo[j].iBegin)
                    && (pAnsEventsTable->EventsRegionInfo[i].iStyle
						== pInputEventsTable->EventsRegionInfo[j].iStyle)
                    && (pInputEventsTable->EventsRegionInfo[j].iWindow
						<= pAnsEventsTable->EventsRegionInfo[i].iWindow)
                    && (pAnsEventsTable->EventsRegionInfo[i].iEnd
						>= pInputEventsTable->EventsRegionInfo[j].iEnd))
                {
                    pAnsEventsTable->EventsRegionInfo[i]
						= pInputEventsTable->EventsRegionInfo[j];
                    break;
                }
                else if ((pAnsEventsTable->EventsRegionInfo[i].iBegin
					> pInputEventsTable->EventsRegionInfo[j].iBegin)
                    && (pAnsEventsTable->EventsRegionInfo[i].iStyle 
						== pInputEventsTable->EventsRegionInfo[j].iStyle)
                    && (pAnsEventsTable->EventsRegionInfo[i].iWindow 
						<= pInputEventsTable->EventsRegionInfo[j].iWindow)
                    && (pAnsEventsTable->EventsRegionInfo[i].iEnd 
						<= pInputEventsTable->EventsRegionInfo[j].iEnd)
                    && (pAnsEventsTable->EventsRegionInfo[i].iEnd 
						- pAnsEventsTable->EventsRegionInfo[i].iBegin 
						>= pInputEventsTable->EventsRegionInfo[j].iWindow)) 
                {
					pInputEventsTable->EventsRegionInfo[j]
						= pAnsEventsTable->EventsRegionInfo[i];
					break;
                }
				else if ((pAnsEventsTable->EventsRegionInfo[i].iBegin
				> pInputEventsTable->EventsRegionInfo[j].iBegin)
                    && ((pAnsEventsTable->EventsRegionInfo[i].iStyle < 2) 
                    || (pInputEventsTable->EventsRegionInfo[j].iStyle < 2) 
                    || (pAnsEventsTable->EventsRegionInfo[i].iStyle
						== pInputEventsTable->EventsRegionInfo[j].iStyle))
                    && (pAnsEventsTable->EventsRegionInfo[i].iEnd
						<= pInputEventsTable->EventsRegionInfo[j].iEnd)
                    && (pInputEventsTable->EventsRegionInfo[j].iBegin == 0))
				{
					pInputEventsTable->EventsRegionInfo[j].iBegin
						= pAnsEventsTable->EventsRegionInfo[i].iBegin;
					break;
				}
				else if ((pAnsEventsTable->EventsRegionInfo[i].iBegin
					< pInputEventsTable->EventsRegionInfo[j].iBegin)
                    && ((pAnsEventsTable->EventsRegionInfo[i].iStyle < 2) 
                    || (pInputEventsTable->EventsRegionInfo[j].iStyle < 2)
                    || (pAnsEventsTable->EventsRegionInfo[i].iStyle
						== pInputEventsTable->EventsRegionInfo[j].iStyle))
                    && (pAnsEventsTable->EventsRegionInfo[i].iEnd
						>= pInputEventsTable->EventsRegionInfo[j].iEnd)
                    && (pAnsEventsTable->EventsRegionInfo[i].iBegin == 0))
				{
					pAnsEventsTable->EventsRegionInfo[i].iBegin
						= pInputEventsTable->EventsRegionInfo[j].iBegin;
					pInputEventsTable->EventsRegionInfo[j] 
						= pAnsEventsTable->EventsRegionInfo[i];
					break;
                }
				else if ((pAnsEventsTable->EventsRegionInfo[i].iBegin
					> pInputEventsTable->EventsRegionInfo[j].iBegin)
                    && (pAnsEventsTable->EventsRegionInfo[i].iStyle
						== pInputEventsTable->EventsRegionInfo[j].iStyle)
                    && (pInputEventsTable->EventsRegionInfo[j].iBegin == 0))
				{
					pInputEventsTable->EventsRegionInfo[j].iBegin
						= pAnsEventsTable->EventsRegionInfo[i].iBegin;
					break;
				}
				else if ((pAnsEventsTable->EventsRegionInfo[i].iBegin
					< pInputEventsTable->EventsRegionInfo[j].iBegin)
                    && (pAnsEventsTable->EventsRegionInfo[i].iStyle
						== pInputEventsTable->EventsRegionInfo[j].iStyle)
                    && (pAnsEventsTable->EventsRegionInfo[i].iBegin == 0))
				{
					pAnsEventsTable->EventsRegionInfo[i].iBegin 
						= pInputEventsTable->EventsRegionInfo[j].iBegin;
					pInputEventsTable->EventsRegionInfo[j]
						= pAnsEventsTable->EventsRegionInfo[i];
					break;
                }
				else if ((pAnsEventsTable->EventsRegionInfo[i].iStyle < 2)
					&& (pInputEventsTable->EventsRegionInfo[j].iStyle == 2)
					&& (pInputEventsTable->EventsRegionInfo[j].iWindow <= 64)
					&& (pAnsEventsTable->EventsRegionInfo[i].iWindow <= pAnsEventsTable->iBlind))
				{
					pAnsEventsTable->EventsRegionInfo[i]
						= pInputEventsTable->EventsRegionInfo[j];
					break;
				}
            }
        }
    }
    EventsTableSort(pAnsEventsTable);

    // 提取pInputEventsTable数据合并;
    for (i = 0; i < pInputEventsTable->iEventsNumber; ++i)
    {
        if (pAnsEventsTable->iEventsNumber > 495)
        {
            break;
        }
		flag = 0;
        for (j = 0; j < pAnsEventsTable->iEventsNumber; ++j)
        {
            //events其中一个事件的初始端小于events_region第i个事件的末端，且末端大于初始端;
            if ((pAnsEventsTable->EventsRegionInfo[j].iBegin
				< pInputEventsTable->EventsRegionInfo[i].iEnd)
                && (pAnsEventsTable->EventsRegionInfo[j].iEnd
					>= pInputEventsTable->EventsRegionInfo[i].iBegin))
            {
                if (((pInputEventsTable->EventsRegionInfo[i].iStyle != 2))
                    || (pAnsEventsTable->EventsRegionInfo[j].iBegin
						- pInputEventsTable->EventsRegionInfo[i].iBegin
                    < pInputEventsTable->EventsRegionInfo[j].iWindow)
                    || ((pInputEventsTable->EventsRegionInfo[i].iWindow = 256)
						&& (j != pAnsEventsTable->iEventsNumber - 1)
                    && (pAnsEventsTable->EventsRegionInfo[i + 1].iBegin
						< pInputEventsTable->EventsRegionInfo[i].iEnd)))
                {
					flag++;
                    break;
                }
                else
                {
                    break;
                }
            }
            else if (pAnsEventsTable->EventsRegionInfo[j].iBegin
						> pInputEventsTable->EventsRegionInfo[i].iEnd)
            {
                break;
            }
        }
        if (!flag)
        {
            pAnsEventsTable->EventsRegionInfo[pAnsEventsTable->iEventsNumber]
				= pInputEventsTable->EventsRegionInfo[i];
            pAnsEventsTable->iEventsNumber++;
        }
    }

    EventsTableSort(pAnsEventsTable);
}

// 根据事件起始位置将事件表排序序(EVENTS_REGION_TABLE)
void EventsTableSort(EVENTS_REGION_TABLE *pEventsRegionTable)//事件表信息
{
    int i = 0, j = 0;
    EVENTS_REGION_INFO TmpEventsInfo;//存储临时事件信息
    int iTmpMin = 0;//存储位置最小值
    int iTmpIndex = 0; //存储最小值索引

    //用选择排序法对事件位置进行排序
    for (i = 0; i < pEventsRegionTable->iEventsNumber - 1; ++i)
    {
        iTmpMin = pEventsRegionTable->EventsRegionInfo[i].iBegin;
        iTmpIndex = i;
        for (j = i + 1; j < pEventsRegionTable->iEventsNumber; ++j)
        {
            if (pEventsRegionTable->EventsRegionInfo[j].iBegin < iTmpMin)
            {
                iTmpIndex = j;
                iTmpMin = pEventsRegionTable->EventsRegionInfo[j].iBegin;
            }
        }
        TmpEventsInfo = pEventsRegionTable->EventsRegionInfo[i];
        pEventsRegionTable->EventsRegionInfo[i]
			= pEventsRegionTable->EventsRegionInfo[iTmpIndex];
        pEventsRegionTable->EventsRegionInfo[iTmpIndex] = TmpEventsInfo;
    }
}

// 根据事件起始位置将事件表排序(EVENTS_TABLE)
void EventsTableSort2(EVENTS_TABLE *pEventsTable)//事件表信息
{
	int i = 0, j = 0;
	EVENTS_INFO TmpEventsInfo;//存储临时事件信息
	float fTmpMin = 0;//存储位置最小值
	int iTmpIndex = 0; //存储最小值索引

	//用选择排序法对事件位置进行排序
	for (i = 0; i < pEventsTable->iEventsNumber - 1; ++i)
	{
		fTmpMin = pEventsTable->EventsInfo[i].fEventsPosition;
		iTmpIndex = i;
		for (j = i + 1; j<pEventsTable->iEventsNumber; ++j)
		{
			if (pEventsTable->EventsInfo[j].fEventsPosition < fTmpMin)
			{
				iTmpIndex = j;
				fTmpMin = pEventsTable->EventsInfo[j].fEventsPosition;
			}
		}
		TmpEventsInfo = pEventsTable->EventsInfo[i];
		pEventsTable->EventsInfo[i] = pEventsTable->EventsInfo[iTmpIndex];
		pEventsTable->EventsInfo[iTmpIndex] = TmpEventsInfo;
	}
}

// 优化事件点的位置
int GetRealBegin(int pulseWidth,
	EVENTS_REGION_INFO EventsRegionInfo,
	float fSampleRating)
{
	int iWindow = EventsRegionInfo.iWindow;
	int iBegin = EventsRegionInfo.iBegin;
	int iMaxIndex = EventsRegionInfo.iMaxIndex;

	int iWindowIndex = 0;
    switch (iWindow)
    {
    case 4:
        iWindowIndex = 0;
        break;
    case 16:
        iWindowIndex = 1;
        break;
    case 64:
        iWindowIndex = 2;
        break;
    case 256:
        iWindowIndex = 3;
        break;
    default:
        iWindowIndex = 0;
        break;
    }

    int powerIndex = (int)(log(fSampleRating) / log(2));
	int tmpWaveError = 45 * frontParament.wave;
	
#define PHASEINDEXNUM 9
#define  WAVELET_DECOMPOSITION_LEVEL 4

	int experienceValue[PHASEINDEXNUM][WAVELET_DECOMPOSITION_LEVEL] = {
				{ iBegin - 2, iBegin - 5, iMaxIndex + 16, iMaxIndex + 104 },
                { iBegin - 2, iBegin - 5, iMaxIndex,      iMaxIndex + 100 },
                { iBegin - 2, iBegin + 7, iMaxIndex - 12, iMaxIndex + 83 },
                { iBegin - 2, iBegin,     iBegin + 27,    iMaxIndex + 76 - tmpWaveError },
                { iBegin - 2, iBegin - 3, iBegin + 34,    iBegin + 122 },
                { iBegin - 2, iBegin - 5, iBegin - 38,    iBegin + 198 },
                { iBegin - 2, iBegin - 5, iBegin - 228,   iBegin + 164 },
                { iBegin - 2, iBegin - 5, iBegin - 230,   iBegin + 103 },
                { iBegin - 2, iBegin - 5, iBegin - 230,   iBegin + 61 } };
	
	return experienceValue[powerIndex + 2][iWindowIndex];
}

// 事件分析函数，寻找事件的详细区间并定位，给出事件类型;
void EventsAnalysis(
    float *pInputSignal, //输入的对数信号;
    const int iSignalLength, //信号长度;
    const int iBlind, //盲区宽度;
    EVENTS_REGION_TABLE *pEventsRegionTable,//事件表信息;
    float fSampleRating)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int iPointShift = 0;//点位移;
    float fMean = 0.0f;//均值;
    float fMax = 0.0f;//最大值;
    int iMaxIndex = 0;//最大值的索引
    float fMin = 0.0f;//防止把小反射事件判断为增益事件,而导致漏判
    int iStart = 0;
    float fDelta = 0.0f; //计算发射率;
    int iTail = 0;//事件末端标志位
    const float fDeltaThr = 0.2f; //反射率检测阈值;
    int pFallRegion[2] = { 0 };
    int pRiseRegion[2] = { 0 };
    int iLen = 0;
    int index_num = 0;
    int iPointNum = (int)(10.0f * fSampleRating);//40M采样率下10个点对应的当前采样率下的数据个数

    int pSaveIndex[MAX_EVENTS_NUMBER] = { 0 };
    int iSaveNumber = 0;
    EVENTS_REGION_TABLE *pTmpEventTable = (EVENTS_REGION_TABLE *)malloc(sizeof(EVENTS_REGION_TABLE));

    // 事件位置计算
    memset(pTmpEventTable, 0, sizeof(EVENTS_REGION_TABLE));
	
    ALG_DEBUG("iEventsNumber = %d, iSignalLength = %d\n", pEventsRegionTable->iEventsNumber, iSignalLength);

	j = 0;
    for (i = 0; i < pEventsRegionTable->iEventsNumber; ++i)
    {
        pTmpEventTable->EventsRegionInfo[j].iStyle
			= pEventsRegionTable->EventsRegionInfo[i].iStyle;

        if (i == 0)
        {
            pTmpEventTable->EventsRegionInfo[0]
				= pEventsRegionTable->EventsRegionInfo[0];
			j = 1;
            pTmpEventTable->iEventsNumber = j;
			
        }
        else
        {			
            if (pEventsRegionTable->EventsRegionInfo[i].iBegin
				== pEventsRegionTable->EventsRegionInfo[0].iBegin)
                continue;
			
            index_num = (int)(log(pEventsRegionTable->EventsRegionInfo[i].iWindow) / log(2));
            iPointShift = index_num - pEventsRegionTable->EventsRegionInfo[i].iWindow / 2;
			
            if ((pEventsRegionTable->EventsRegionInfo[i].iStyle == 0))
            {
            	if (i == pEventsRegionTable->iEventsNumber - 1)
            	{
            		if (pEventsRegionTable->EventsRegionInfo[i].iWindow < 64)
            		{
            			iPointShift += 4;
            		}
					
            		if (frontParament.pulseWidth == ENUM_PULSE_500NS)
            		{
            			pTmpEventTable->EventsRegionInfo[j].iBegin = GetRealBegin(
							frontParament.pulseWidth, pEventsRegionTable->EventsRegionInfo[i], fSampleRating);
            		}
					else
					{
						float tmpCoef = 1.21f;
						if (range < 300)
						{
							switch (frontParament.pulseWidth)
							{
							case 2:
								if (pEventsRegionTable->EventsRegionInfo[i].iWindow == 64)
								{
									tmpCoef = 1.40f;
								} 
								else if (pEventsRegionTable->EventsRegionInfo[i].iWindow == 256)
								{
									tmpCoef = 1.36f;
								}
								break;
							case 3:
								tmpCoef = 1.40f;
								break;
							case 4:
								tmpCoef = 1.57f;
								break;
							case 5:
								tmpCoef = 1.22f;
								break;
							default:
								break;
							}
						}
						else
						{
							if (frontParament.pulseWidth == ENUM_PULSE_1US)
							{
								tmpCoef = 1.71f;
							}
							if (frontParament.pulseWidth == ENUM_PULSE_20US)
							{
								tmpCoef = 1.10f;
							}
						}
						
						pTmpEventTable->EventsRegionInfo[j].iBegin
							= pEventsRegionTable->EventsRegionInfo[i].iMaxIndex
							- iPointShift - (int)round(tmpCoef * iBlind + 0.2f);
					}
					
					pTmpEventTable->EventsRegionInfo[j].iBegin
						= Max(pTmpEventTable->EventsRegionInfo[j].iBegin, 
						pTmpEventTable->EventsRegionInfo[j - 1].iEnd + 1);
					
					pTmpEventTable->EventsRegionInfo[j].iEnd
						= Max(pEventsRegionTable->EventsRegionInfo[i].iEnd,
						pEventsRegionTable->EventsRegionInfo[i].iMaxIndex
						+ pEventsRegionTable->EventsRegionInfo[i].iWindow / 2);
					
					pTmpEventTable->EventsRegionInfo[j].iEnd
						= Min(pTmpEventTable->EventsRegionInfo[j].iEnd, iSignalLength);
            		pTmpEventTable->EventsRegionInfo[j].iMaxIndex
						= Min(pEventsRegionTable->EventsRegionInfo[i].iMaxIndex, iSignalLength);
	                pTmpEventTable->EventsRegionInfo[j].iMinIndex
						= Min(pEventsRegionTable->EventsRegionInfo[i].iMinIndex, iSignalLength);
	                pTmpEventTable->EventsRegionInfo[j].iStyle
						= pEventsRegionTable->EventsRegionInfo[i].iStyle;
	                pTmpEventTable->EventsRegionInfo[j].iWindow
						= pEventsRegionTable->EventsRegionInfo[i].iWindow;

					j++;
	            	pTmpEventTable->iEventsNumber = j;
            		continue;
            	}
                if (pEventsRegionTable->EventsRegionInfo[i].iWindow == 64)
                {
                	if (frontParament.pulseWidth == ENUM_PULSE_5NS)
                	{
                		pTmpEventTable->EventsRegionInfo[j].iBegin
							= Min(pEventsRegionTable->EventsRegionInfo[i].iMaxIndex + iPointShift / 2,
									pEventsRegionTable->EventsRegionInfo[i].iBegin);
                	}
					else
					{
						if (iBlind > 2 * pEventsRegionTable->EventsRegionInfo[i].iWindow)
						{
							float tmpCoef = 1.05f;
							pTmpEventTable->EventsRegionInfo[j].iBegin
								= pEventsRegionTable->EventsRegionInfo[i].iMaxIndex
								- iPointShift - (int)round(tmpCoef * iBlind + 0.2f);
						}
						else
						{
							pTmpEventTable->EventsRegionInfo[j].iBegin
								= pEventsRegionTable->EventsRegionInfo[i].iMaxIndex + 25;
							pTmpEventTable->EventsRegionInfo[j].iBegin
								= Min(pTmpEventTable->EventsRegionInfo[j].iBegin, iSignalLength);
						}
					}
               }
                else if (pEventsRegionTable->EventsRegionInfo[i].iWindow == 256)
                {
					if (iBlind > 1.5 * pEventsRegionTable->EventsRegionInfo[i].iWindow)
					{
						float tmpCoef = 1.10f;
						pTmpEventTable->EventsRegionInfo[j].iBegin
							= pEventsRegionTable->EventsRegionInfo[i].iMaxIndex
							- iPointShift - (int)round(tmpCoef * iBlind + 0.2f);
					} 
					else
					{
						pTmpEventTable->EventsRegionInfo[j].iBegin
							= pEventsRegionTable->EventsRegionInfo[i].iMaxIndex + 120;
					}
                    pTmpEventTable->EventsRegionInfo[j].iBegin
						= Min(pTmpEventTable->EventsRegionInfo[j].iBegin, iSignalLength);
                }
                else
                {
                    pTmpEventTable->EventsRegionInfo[j].iBegin
						= Min(pEventsRegionTable->EventsRegionInfo[i].iBegin
						+ pEventsRegionTable->EventsRegionInfo[i].iWindow / 2,
                        pEventsRegionTable->EventsRegionInfo[i].iMaxIndex - iPointShift / 2);
                    pTmpEventTable->EventsRegionInfo[j].iBegin
						= Min(pTmpEventTable->EventsRegionInfo[j].iBegin, iSignalLength);
                }

                if ((frontParament.pulseWidth == ENUM_PULSE_20US)
					&& (pEventsRegionTable->EventsRegionInfo[i].iWindow == 16))
                {
                    ;
                }
                else
                {
                    pTmpEventTable->EventsRegionInfo[j].iEnd
						= Max(pTmpEventTable->EventsRegionInfo[j].iBegin
						+ iBlind, pEventsRegionTable->EventsRegionInfo[i].iEnd - iPointShift);
                    pTmpEventTable->EventsRegionInfo[j].iEnd
						= Min(pTmpEventTable->EventsRegionInfo[j].iEnd, iSignalLength);
                }
                pTmpEventTable->EventsRegionInfo[j].iMaxIndex
					= Min(pEventsRegionTable->EventsRegionInfo[i].iMaxIndex, iSignalLength);
                pTmpEventTable->EventsRegionInfo[j].iMinIndex
					= Min(pEventsRegionTable->EventsRegionInfo[i].iMinIndex, iSignalLength);
                pTmpEventTable->EventsRegionInfo[j].iStyle
					= pEventsRegionTable->EventsRegionInfo[i].iStyle;
                pTmpEventTable->EventsRegionInfo[j].iWindow
					= pEventsRegionTable->EventsRegionInfo[i].iWindow;

				j++;
            	pTmpEventTable->iEventsNumber = j;		
            }
            else
            {
                if (pEventsRegionTable->EventsRegionInfo[i].iWindow == 16)
                {
                	
                    pTmpEventTable->EventsRegionInfo[j].iBegin
						= Min(pEventsRegionTable->EventsRegionInfo[i].iMinIndex - iPointShift,
                        pEventsRegionTable->EventsRegionInfo[i].iBegin
                        + pEventsRegionTable->EventsRegionInfo[i].iWindow) + 1;
                    pTmpEventTable->EventsRegionInfo[j].iBegin
						= Min(pTmpEventTable->EventsRegionInfo[j].iBegin, iSignalLength);

                    pTmpEventTable->EventsRegionInfo[j].iEnd
						= Max(pTmpEventTable->EventsRegionInfo[j].iBegin + iBlind,
						pEventsRegionTable->EventsRegionInfo[i].iEnd - iPointShift);
                    pTmpEventTable->EventsRegionInfo[j].iEnd
						= Min(pTmpEventTable->EventsRegionInfo[j].iEnd, iSignalLength);

                    pTmpEventTable->EventsRegionInfo[j].iMaxIndex
						= pEventsRegionTable->EventsRegionInfo[i].iMaxIndex;
                    pTmpEventTable->EventsRegionInfo[j].iMinIndex
						= pEventsRegionTable->EventsRegionInfo[i].iMinIndex;
                    pTmpEventTable->EventsRegionInfo[j].iStyle
						= pEventsRegionTable->EventsRegionInfo[i].iStyle;
                    pTmpEventTable->EventsRegionInfo[j].iWindow
						= pEventsRegionTable->EventsRegionInfo[i].iWindow;
					
					j++;
            		pTmpEventTable->iEventsNumber = j;
                }
                else
                {
                    if (pEventsRegionTable->EventsRegionInfo[i].iWindow == 64)
                    {
                        if (fSampleRating > 30)
                        {
                            pTmpEventTable->EventsRegionInfo[j].iBegin
								= Min(pEventsRegionTable->EventsRegionInfo[i].iMinIndex - iPointShift,
                                pEventsRegionTable->EventsRegionInfo[i].iBegin
                                + pEventsRegionTable->EventsRegionInfo[i].iWindow) - 2;
                            pTmpEventTable->EventsRegionInfo[j].iBegin
								= Max(pTmpEventTable->EventsRegionInfo[j].iBegin, 0);
                            pTmpEventTable->EventsRegionInfo[j].iBegin
								= Min(pTmpEventTable->EventsRegionInfo[j].iBegin, iSignalLength);
                        }
                        else
                        {
                            pTmpEventTable->EventsRegionInfo[j].iBegin
								= Min(pEventsRegionTable->EventsRegionInfo[i].iMinIndex - iPointShift,
                                pEventsRegionTable->EventsRegionInfo[i].iBegin
                                + pEventsRegionTable->EventsRegionInfo[i].iWindow) + 2;
                            pTmpEventTable->EventsRegionInfo[j].iBegin
								= Min(pTmpEventTable->EventsRegionInfo[j].iBegin, iSignalLength);

                        }
                    }
                    else
                    {
                        pTmpEventTable->EventsRegionInfo[j].iBegin
                        = Min(pEventsRegionTable->EventsRegionInfo[i].iMinIndex,
                            pEventsRegionTable->EventsRegionInfo[i].iBegin
                            + pEventsRegionTable->EventsRegionInfo[i].iWindow);
                        pTmpEventTable->EventsRegionInfo[j].iBegin
							= Min(pTmpEventTable->EventsRegionInfo[j].iBegin, iSignalLength);
                    }
                    pTmpEventTable->EventsRegionInfo[j].iEnd
						= Max(pTmpEventTable->EventsRegionInfo[j].iBegin + iBlind,
								pEventsRegionTable->EventsRegionInfo[i].iEnd - iPointShift);
                    pTmpEventTable->EventsRegionInfo[j].iEnd
						= Min(pTmpEventTable->EventsRegionInfo[j].iEnd, iSignalLength);
                    pTmpEventTable->EventsRegionInfo[j].iMaxIndex
						= pEventsRegionTable->EventsRegionInfo[i].iMaxIndex;
                    pTmpEventTable->EventsRegionInfo[j].iMinIndex
						= pEventsRegionTable->EventsRegionInfo[i].iMinIndex;
                    pTmpEventTable->EventsRegionInfo[j].iStyle
						= pEventsRegionTable->EventsRegionInfo[i].iStyle;
                    pTmpEventTable->EventsRegionInfo[j].iWindow
						= pEventsRegionTable->EventsRegionInfo[i].iWindow;

					j++;
            		pTmpEventTable->iEventsNumber = j;
                }
            }
        }
    }
    //排序;
    EventsTableSort(pTmpEventTable);
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------EventsAnalysis 1-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------EventsAnalysis 1-----------------------\n\n");
		PrintEventRegionInfo(test_file_name, pTmpEventTable);
	#else
		PrintEventRegionInfo(stdout, pTmpEventTable);
	#endif
	//PrintEventRegionInfo(stdout, pTmpEventTable);
	ALG_DEBUG("\n\n\n");
#endif
    for (k = 0; k < 50; ++k)
    {
        for (i = 0; i<pTmpEventTable->iEventsNumber; ++i)
        {
            if ((i > 0) && (pTmpEventTable->EventsRegionInfo[i].iBegin
				< pTmpEventTable->EventsRegionInfo[i - 1].iEnd))
            {
                if (pTmpEventTable->EventsRegionInfo[i - 1].iBegin
					< pTmpEventTable->EventsRegionInfo[i].iBegin)
                {
                    if ((pTmpEventTable->EventsRegionInfo[i - 1].iStyle
							== pTmpEventTable->EventsRegionInfo[i].iStyle)
                        && (pTmpEventTable->EventsRegionInfo[i - 1].iWindow 
							> pTmpEventTable->EventsRegionInfo[i].iWindow)
                        && (pTmpEventTable->EventsRegionInfo[i].iBegin
							- pTmpEventTable->EventsRegionInfo[i - 1].iBegin < iBlind))
                    {
                        if (pTmpEventTable->EventsRegionInfo[i].iEnd
							> pTmpEventTable->EventsRegionInfo[i - 1].iEnd)
                        {
                            pTmpEventTable->EventsRegionInfo[i].iBegin
								= pTmpEventTable->EventsRegionInfo[i - 1].iBegin;
                            pTmpEventTable->EventsRegionInfo[i - 1]
								= pTmpEventTable->EventsRegionInfo[i];
                        }
                        else
                        {
                        	pTmpEventTable->EventsRegionInfo[i].iBegin
								= pTmpEventTable->EventsRegionInfo[i - 1].iBegin;
                            pTmpEventTable->EventsRegionInfo[i - 1]
								= pTmpEventTable->EventsRegionInfo[i];
                        }
                    }
                    else
                    {

						if (Max(pTmpEventTable->EventsRegionInfo[i].iEnd,
							pTmpEventTable->EventsRegionInfo[i - 1].iEnd)
							- Min(pTmpEventTable->EventsRegionInfo[i].iBegin,
								pTmpEventTable->EventsRegionInfo[i - 1].iBegin) < Min(256 * 4, iBlind * 20))
			        	{
			        		if (pTmpEventTable->EventsRegionInfo[i].iWindow
								> pTmpEventTable->EventsRegionInfo[i - 1].iWindow)
		        			{
								pTmpEventTable->EventsRegionInfo[i]
									= pTmpEventTable->EventsRegionInfo[i - 1];
								pTmpEventTable->EventsRegionInfo[i].iStyle
									= Max(pTmpEventTable->EventsRegionInfo[i - 1].iStyle,
										pTmpEventTable->EventsRegionInfo[i].iStyle);
		        			}
							else if (pTmpEventTable->EventsRegionInfo[i].iWindow
								< pTmpEventTable->EventsRegionInfo[i - 1].iWindow)
							{
								;
							}
							else
							{
					            pTmpEventTable->EventsRegionInfo[i].iBegin
									= Min(pTmpEventTable->EventsRegionInfo[i].iBegin,
									pTmpEventTable->EventsRegionInfo[i - 1].iBegin);
					            pTmpEventTable->EventsRegionInfo[i].iEnd
									= Max(pTmpEventTable->EventsRegionInfo[i].iEnd,
									pTmpEventTable->EventsRegionInfo[i - 1].iEnd);
								pTmpEventTable->EventsRegionInfo[i].iStyle 
									= Max(pTmpEventTable->EventsRegionInfo[i - 1].iStyle,
										pTmpEventTable->EventsRegionInfo[i].iStyle);
							}
			        	}
						else
						{

							if (pTmpEventTable->EventsRegionInfo[i].iWindow
								> pTmpEventTable->EventsRegionInfo[i - 1].iWindow)
		        			{
								pTmpEventTable->EventsRegionInfo[i]
									= pTmpEventTable->EventsRegionInfo[i - 1];
								pTmpEventTable->EventsRegionInfo[i].iStyle
									= Max(pTmpEventTable->EventsRegionInfo[i - 1].iStyle,
										pTmpEventTable->EventsRegionInfo[i].iStyle);
		        			}
							else if (pTmpEventTable->EventsRegionInfo[i].iWindow
								< pTmpEventTable->EventsRegionInfo[i - 1].iWindow)
							{
								;
							}
							else
							{
					            pTmpEventTable->EventsRegionInfo[i].iBegin
									= Min(pTmpEventTable->EventsRegionInfo[i].iBegin,
									pTmpEventTable->EventsRegionInfo[i - 1].iBegin);
								pTmpEventTable->EventsRegionInfo[i].iEnd
									= Max(pTmpEventTable->EventsRegionInfo[i].iEnd,
									pTmpEventTable->EventsRegionInfo[i - 1].iEnd);
								pTmpEventTable->EventsRegionInfo[i].iStyle
									= Max(pTmpEventTable->EventsRegionInfo[i - 1].iStyle,
										pTmpEventTable->EventsRegionInfo[i].iStyle);
							}	
						}
                        pTmpEventTable->EventsRegionInfo[i - 1]
							= pTmpEventTable->EventsRegionInfo[i];
                    }
                }
                else
                    pTmpEventTable->EventsRegionInfo[i - 1].iEnd
					= pTmpEventTable->EventsRegionInfo[i - 1].iBegin;
            }
        }
        pSaveIndex[0] = 0;
        iSaveNumber = 0;
        for (i = 0; i < pTmpEventTable->iEventsNumber; ++i)
        {
            //将小于0的点和重合的点删除;
            if ((pTmpEventTable->EventsRegionInfo[i].iBegin >= 0)
				&& (pTmpEventTable->EventsRegionInfo[i].iEnd >= 0)
                && ((i == pTmpEventTable->iEventsNumber - 1)
					|| (pTmpEventTable->EventsRegionInfo[i].iBegin
						< pTmpEventTable->EventsRegionInfo[i + 1].iBegin)))
            {
                pSaveIndex[iSaveNumber] = i;
                iSaveNumber++;
            }
        }

        if (iSaveNumber == pTmpEventTable->iEventsNumber)
            break;

        for (i = 0; i < iSaveNumber; ++i)
        {
            pTmpEventTable->EventsRegionInfo[i]
				= pTmpEventTable->EventsRegionInfo[pSaveIndex[i]];
        }
        pTmpEventTable->iEventsNumber = iSaveNumber;
        EventsTableSort(pTmpEventTable);
    }

		
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------EventsAnalysis 2-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------EventsAnalysis 2-----------------------\n\n");
		PrintEventRegionInfo(test_file_name, pTmpEventTable);
	#else
		PrintEventRegionInfo(stdout, pTmpEventTable);
	#endif
	//PrintEventRegionInfo(stdout, pTmpEventTable);
	ALG_DEBUG("\n\n\n");
#endif

    // 合并小于紧挨的事件
    iSaveNumber = 1;
    pSaveIndex[0] = 0;
    for (j = 1; j < pTmpEventTable->iEventsNumber; j++)
    {
        if (pTmpEventTable->EventsRegionInfo[j].iBegin
			<= pTmpEventTable->EventsRegionInfo[j - 1].iEnd)

        {
        	if (pTmpEventTable->EventsRegionInfo[j - 1].iStyle
				!= pTmpEventTable->EventsRegionInfo[j].iStyle)
    		{
				pTmpEventTable->EventsRegionInfo[j].iBegin
					= Min(pTmpEventTable->EventsRegionInfo[j].iBegin,
					pTmpEventTable->EventsRegionInfo[j - 1].iBegin);
	            pTmpEventTable->EventsRegionInfo[j].iEnd
					= Max(pTmpEventTable->EventsRegionInfo[j].iEnd,
					pTmpEventTable->EventsRegionInfo[j - 1].iEnd);
	            pTmpEventTable->EventsRegionInfo[j - 1].iEnd
					= pTmpEventTable->EventsRegionInfo[j].iEnd;
	            pTmpEventTable->EventsRegionInfo[j - 1].iBegin
					= pTmpEventTable->EventsRegionInfo[j].iBegin;
				pTmpEventTable->EventsRegionInfo[j - 1].iStyle
					= Max(pTmpEventTable->EventsRegionInfo[j].iStyle,
						pTmpEventTable->EventsRegionInfo[j - 1].iStyle);
    		}
        	else if (Max(pTmpEventTable->EventsRegionInfo[j].iEnd,
				pTmpEventTable->EventsRegionInfo[j - 1].iEnd)
				- Min(pTmpEventTable->EventsRegionInfo[j].iBegin,
					pTmpEventTable->EventsRegionInfo[j - 1].iBegin) < Max(256, iBlind * 5))
        	{
	            pTmpEventTable->EventsRegionInfo[j].iBegin
					= Min(pTmpEventTable->EventsRegionInfo[j].iBegin,
					pTmpEventTable->EventsRegionInfo[j - 1].iBegin);
	            pTmpEventTable->EventsRegionInfo[j].iEnd
					= Max(pTmpEventTable->EventsRegionInfo[j].iEnd,
					pTmpEventTable->EventsRegionInfo[j - 1].iEnd);
	            pTmpEventTable->EventsRegionInfo[j - 1].iEnd
					= pTmpEventTable->EventsRegionInfo[j].iEnd;
	            pTmpEventTable->EventsRegionInfo[j - 1].iBegin
					= pTmpEventTable->EventsRegionInfo[j].iBegin;
				pTmpEventTable->EventsRegionInfo[j - 1].iStyle
					= Max(pTmpEventTable->EventsRegionInfo[j].iStyle,
						pTmpEventTable->EventsRegionInfo[j - 1].iStyle);
        	}
			else
			{
				 if (pTmpEventTable->EventsRegionInfo[j].iWindow
					> pTmpEventTable->EventsRegionInfo[j - 1].iWindow)
			 	{
					pTmpEventTable->EventsRegionInfo[j - 1].iEnd 
						= Max(pTmpEventTable->EventsRegionInfo[j].iBegin - iBlind,
							pTmpEventTable->EventsRegionInfo[j - 1].iBegin + iBlind);
			 	}
				 else if (pTmpEventTable->EventsRegionInfo[j].iWindow
					 < pTmpEventTable->EventsRegionInfo[j - 1].iWindow)
			 	{
					pTmpEventTable->EventsRegionInfo[j - 1].iEnd
						= Max(pTmpEventTable->EventsRegionInfo[j].iBegin - iBlind,
							pTmpEventTable->EventsRegionInfo[j - 1].iBegin + iBlind);
					pTmpEventTable->EventsRegionInfo[j - 1].iStyle
						= Max(pTmpEventTable->EventsRegionInfo[j].iStyle,
							pTmpEventTable->EventsRegionInfo[j - 1].iStyle);
			 	}
				 else
			 	{
					pTmpEventTable->EventsRegionInfo[j].iBegin
						= Min(pTmpEventTable->EventsRegionInfo[j].iBegin,
						pTmpEventTable->EventsRegionInfo[j - 1].iBegin);
					pTmpEventTable->EventsRegionInfo[j].iEnd
						= Max(pTmpEventTable->EventsRegionInfo[j].iEnd,
						pTmpEventTable->EventsRegionInfo[j - 1].iEnd);
					pTmpEventTable->EventsRegionInfo[j - 1].iEnd
						= pTmpEventTable->EventsRegionInfo[j].iEnd;
					pTmpEventTable->EventsRegionInfo[j - 1].iBegin
						= pTmpEventTable->EventsRegionInfo[j].iBegin;
					pTmpEventTable->EventsRegionInfo[j - 1].iStyle 
						= Max(pTmpEventTable->EventsRegionInfo[j].iStyle,
							pTmpEventTable->EventsRegionInfo[j - 1].iStyle);
			 	}
			}
        }
		/*else if ((pTmpEventTable->EventsRegionInfo[j].iStyle == pTmpEventTable->EventsRegionInfo[j - 1].iStyle) 
			&& (pTmpEventTable->EventsRegionInfo[j].iStyle < 2)
			&& (pTmpEventTable->EventsRegionInfo[j].iBegin - pTmpEventTable->EventsRegionInfo[j - 1].iEnd < Min(pTmpEventTable->EventsRegionInfo[j].iWindow, iBlind)))            // added by wyl 2019/07/30
		{
			pTmpEventTable->EventsRegionInfo[j].iBegin
				= Min(pTmpEventTable->EventsRegionInfo[j].iBegin,
				pTmpEventTable->EventsRegionInfo[j - 1].iBegin);
            pTmpEventTable->EventsRegionInfo[j].iEnd
				= Max(pTmpEventTable->EventsRegionInfo[j].iEnd,
				pTmpEventTable->EventsRegionInfo[j - 1].iEnd);
			

	
            pTmpEventTable->EventsRegionInfo[j - 1].iEnd
				= pTmpEventTable->EventsRegionInfo[j].iEnd;
            pTmpEventTable->EventsRegionInfo[j - 1].iBegin
				= pTmpEventTable->EventsRegionInfo[j].iBegin;
		}*/
        else
        {
            pSaveIndex[iSaveNumber] = j;
            iSaveNumber++;
        }
    }

    for (j = 0; j < iSaveNumber; ++j)
    {
        pTmpEventTable->EventsRegionInfo[j] 
			= pTmpEventTable->EventsRegionInfo[pSaveIndex[j]];
    }
    pTmpEventTable->iEventsNumber = iSaveNumber;
    
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------EventsAnalysis 3-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------EventsAnalysis 3-----------------------\n\n");
		PrintEventRegionInfo(test_file_name, pTmpEventTable);
	#else
		PrintEventRegionInfo(stdout, pTmpEventTable);
	#endif
	//PrintEventRegionInfo(stdout, pTmpEventTable);
	ALG_DEBUG("\n\n\n");
#endif

    //对两类事件进行重新定位和类型判断;
    //1、小窗口下的上升沿事件；2、大窗口下的反射事件;
    for (i = 1; i < pTmpEventTable->iEventsNumber; ++i)//计算每个区间事件的上升和下降高度，以判断是否为反射信号 ;
    {
        //小窗口下的非反射事件;
        if (((pTmpEventTable->EventsRegionInfo[i].iStyle <= 1)
			&& (pTmpEventTable->EventsRegionInfo[i].iWindow <= 64))
            || ((pTmpEventTable->EventsRegionInfo[i].iStyle == 2)
            && (pTmpEventTable->EventsRegionInfo[i].iWindow >= 64))
            || (((pTmpEventTable->EventsRegionInfo[i].iStyle == 2)
            && (pTmpEventTable->EventsRegionInfo[i].iWindow == 16))
            && (pTmpEventTable->EventsRegionInfo[i].iBegin
             - pTmpEventTable->EventsRegionInfo[i - 1].iEnd >= Max(200, 5 * iBlind))))
        {
            fMean = GetMaxFloat(pInputSignal + pTmpEventTable->EventsRegionInfo[i].iBegin - 1,
				pTmpEventTable->EventsRegionInfo[i].iEnd - pTmpEventTable->EventsRegionInfo[i].iBegin + 1);

			int itmpLen = Max(5 * iPointNum, iBlind);
            if ((pTmpEventTable->EventsRegionInfo[i].iBegin - itmpLen
					> pTmpEventTable->EventsRegionInfo[i - 1].iEnd)
                || (pTmpEventTable->EventsRegionInfo[i - 1].iStyle == 0))
            {

                iStart = Max(pTmpEventTable->EventsRegionInfo[i - 1].iEnd,
					pTmpEventTable->EventsRegionInfo[i].iBegin - itmpLen);
                iLen = Max(1, pTmpEventTable->EventsRegionInfo[i].iBegin - itmpLen / 4 - iStart);
                fMax = GetMaxFloat(pInputSignal + iStart, iLen);//计算增益上升沿后方盲区内最大值;
            }
            else
            {
                iStart = Max(pTmpEventTable->EventsRegionInfo[i].iBegin - Max(3 * iBlind + 1, 10),
								pTmpEventTable->EventsRegionInfo[i - 1].iEnd);
                iLen = Max(pTmpEventTable->EventsRegionInfo[i].iBegin - iBlind - iStart, 1);
                fMax = GetMaxFloat(pInputSignal + iStart, iLen);
            }
            fMax = Max(fMax, 3);

            fDelta = fMean - fMax;
            if (i == pTmpEventTable->iEventsNumber - 1)
				iTail = Min(pTmpEventTable->EventsRegionInfo[i].iEnd + 5 * iPointNum, iSignalLength - 2);
            else
            {
                iTail = Min(pTmpEventTable->EventsRegionInfo[i].iEnd + 5 * iPointNum,
								pTmpEventTable->EventsRegionInfo[i + 1].iBegin - 3);
                iTail = Min(iTail, iSignalLength - 2);
            }
			
			ALG_DEBUG("iStart = %d, iTail = %d, fDelta = %f, fMax = %f\n", iStart, iTail, fDelta, fMax);
			if (fDelta < -3.0f * fDeltaThr)
			{
				//反射事件判断为衰减事件，且起始位置找在反射峰的后端
        		iStart = Max(iStart - iBlind, pTmpEventTable->EventsRegionInfo[i - 1].iEnd);
				fMean = Max(fMean, pInputSignal[iStart]);
				fDelta = fMax - fMean;
			}		
			ALG_DEBUG("i = %d, iStart = %d, iTail = %d, fDelta = %f, fMax = %f\n", i, iStart, iTail, fDelta, fMax);
			
            if (fDelta >= 2.5f * fDeltaThr)//该种情况下，增益事件很可能为反射事件，需要详细计算 1->2.5
            {	
                PeakRegion(pInputSignal + iStart, iTail - iStart + 1,
					fMax, fMax + fDelta / 2, pRiseRegion, pFallRegion);

				ALG_DEBUG("pRiseRegion[0] = %d, pRiseRegion[1] = %d, pFallRegion[0] = %d, pFallRegion[1] = %d\n",
					pRiseRegion[0], pRiseRegion[1], pFallRegion[0], pFallRegion[1]);
				
                pTmpEventTable->EventsRegionInfo[i].iBegin = iStart + pRiseRegion[0] - 1;
                if ((iTail - pTmpEventTable->EventsRegionInfo[i].iBegin < iBlind)
					&& (pFallRegion[1] - pRiseRegion[1] >= Min(5 * iPointNum, iBlind / 4)))
                {
                    pTmpEventTable->EventsRegionInfo[i].iEnd = iTail;
                    pTmpEventTable->EventsRegionInfo[i].iStyle = 2;
                }
                else
                {
                    //若上升区间小于1/2盲区，则认为该事件为反射事件，并重新定位					
                    if ((pFallRegion[1] > 0) && (pFallRegion[1] - pRiseRegion[1]
						>= Min(iPointNum, iBlind / 4)))
                    {
                        pTmpEventTable->EventsRegionInfo[i].iEnd = iStart + pFallRegion[1];
                        pTmpEventTable->EventsRegionInfo[i].iStyle = 2;
                    }
                    else
                    {
                        //添加判断，防止因为拖尾严重，没有找到下降区间而导致的判断错误
                        if (fDelta > 1.5f)
                        {
                            pTmpEventTable->EventsRegionInfo[i].iStyle = 2;
                            pTmpEventTable->EventsRegionInfo[i].iEnd = iTail;
                        }
                    }
                }
            }
            else
            {
                if (pTmpEventTable->EventsRegionInfo[i].iStyle == 1)
                {
                    fMin = GetMinFloat(pInputSignal
						+ pTmpEventTable->EventsRegionInfo[i].iBegin,
						pTmpEventTable->EventsRegionInfo[i].iEnd
                        - pTmpEventTable->EventsRegionInfo[i].iBegin + 1);
                    if (fMean - fMin > 0.5f)
                    {
                        pTmpEventTable->EventsRegionInfo[i].iStyle = 2;
                        iMaxIndex = GetMaxIndex(pInputSignal
							+ pTmpEventTable->EventsRegionInfo[i].iBegin,
							pTmpEventTable->EventsRegionInfo[i].iEnd
                            - pTmpEventTable->EventsRegionInfo[i].iBegin + 1);
                        pTmpEventTable->EventsRegionInfo[i].iBegin
							= pTmpEventTable->EventsRegionInfo[i].iBegin + iMaxIndex - iBlind / 2;
                    }
                }
            }
        }
		else if (((pTmpEventTable->EventsRegionInfo[i].iEnd
				- pTmpEventTable->EventsRegionInfo[i].iBegin > 1000
				&& frontParament.iSampleRating > 16)
			|| (pTmpEventTable->EventsRegionInfo[i].iEnd 
				- pTmpEventTable->EventsRegionInfo[i].iBegin > 256
				&& frontParament.iSampleRating <= 16))
			&& (pTmpEventTable->EventsRegionInfo[i].iStyle == 2) 
			&& (frontParament.pulseWidth >= ENUM_PULSE_20NS)
			&& (frontParament.pulseWidth <= ENUM_PULSE_200NS))
		{
			float MaxValue = GetMaxFloat(pInputSignal
				+ pTmpEventTable->EventsRegionInfo[i].iBegin,
				pTmpEventTable->EventsRegionInfo[i].iEnd
				- pTmpEventTable->EventsRegionInfo[i].iBegin);
			float MinValue = GetMinFloat(pInputSignal
				+ pTmpEventTable->EventsRegionInfo[i].iBegin,
				pTmpEventTable->EventsRegionInfo[i].iEnd 
				- pTmpEventTable->EventsRegionInfo[i].iBegin);
			if (MaxValue - MinValue < 2.0f)
			{
				pTmpEventTable->EventsRegionInfo[i].iStyle = 0;
			}
		}
    }
    for (i = 0; i < pTmpEventTable->iEventsNumber - 1; ++i)
    {
        pTmpEventTable->EventsRegionInfo[i].iEnd
			= Min(pTmpEventTable->EventsRegionInfo[i].iEnd,
				pTmpEventTable->EventsRegionInfo[i + 1].iBegin - 1);
    }
    
#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------EventsAnalysis 4-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------EventsAnalysis 4-----------------------\n\n");
		PrintEventRegionInfo(test_file_name, pTmpEventTable);
	#else
		PrintEventRegionInfo(stdout, pTmpEventTable);
	#endif
	//PrintEventRegionInfo(stdout, pTmpEventTable);
	ALG_DEBUG("\n\n\n");
#endif
	
    // 将紧靠在一起的损耗增益事件合并为一个事件，其很可能是下冲毛刺导致。
	// 删除反射事件后方紧靠的非反射事件
    for (j = 0; j < 10; ++j)
    {
        pSaveIndex[0] = 0;
        iSaveNumber = 1;
        for (i = 1; i < pTmpEventTable->iEventsNumber; ++i)
        {
            //add by sjt 2015.07.01 防止一些事件被删除和删除一些误报
            if (((pTmpEventTable->EventsRegionInfo[i - 1].iStyle == 0)
				&& (pTmpEventTable->EventsRegionInfo[i].iStyle == 1)
                && (((pTmpEventTable->EventsRegionInfo[i].iBegin
                 - pTmpEventTable->EventsRegionInfo[i - 1].iEnd < iBlind)
                && (frontParament.pulseWidth < ENUM_PULSE_20US))
                || ((pTmpEventTable->EventsRegionInfo[i].iBegin
                 - pTmpEventTable->EventsRegionInfo[i - 1].iBegin < iBlind)
                && (frontParament.pulseWidth == ENUM_PULSE_20US))))
                || (pTmpEventTable->EventsRegionInfo[i].iEnd
                 - pTmpEventTable->EventsRegionInfo[i].iBegin <= 0)
                || ((pTmpEventTable->EventsRegionInfo[i - 1].iStyle == 2)
                && (pTmpEventTable->EventsRegionInfo[i].iStyle < 2)
                && (pTmpEventTable->EventsRegionInfo[i].iBegin
                 - pTmpEventTable->EventsRegionInfo[i - 1].iEnd < (int)(iBlind / 3))))
            {
            	#ifdef DEBUG_EVENTINFO
					#if ISSERIAL == 1
						fprintf(test_file_name, "---i = %d--j = %d---\n", i, j);
						PrintEventRegionInfo(test_file_name, pTmpEventTable);
					#endif
				#endif
            }
            else
            {
                pSaveIndex[iSaveNumber] = i;
                iSaveNumber++;
            }
        }
		
        if (iSaveNumber == pTmpEventTable->iEventsNumber)
        {
            break;
        }
        else
        {
        	for (i = 0; i < iSaveNumber; i++)
	        {
	            pTmpEventTable->EventsRegionInfo[i]
					= pTmpEventTable->EventsRegionInfo[pSaveIndex[i]];
	        }
            pTmpEventTable->iEventsNumber = iSaveNumber;
        }
    }
	
	for (i = 0; i < pTmpEventTable->iEventsNumber; ++i)
    {
        pEventsRegionTable->EventsRegionInfo[i]
			= pTmpEventTable->EventsRegionInfo[i];
    }
	pEventsRegionTable->iEventsNumber = pTmpEventTable->iEventsNumber;
	
	
    // 防止事件末端过长而导致的衰减率计算不准确，故添加该循环
    for (i = 0; i < pEventsRegionTable->iEventsNumber; ++i)
    {
       if (frontParament.pulseWidth < ENUM_PULSE_50NS
		   || frontParament.pulseWidth > ENUM_PULSE_100NS)
       	{
	       pEventsRegionTable->EventsRegionInfo[i].iEnd
				= Min(pEventsRegionTable->EventsRegionInfo[i].iEnd,
				pEventsRegionTable->EventsRegionInfo[i].iBegin + 6 * iBlind); 
       	}	
    }

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------ EventsAnalysis 5-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------EventsAnalysis 5-----------------------\n\n");
		fprintf(test_file_name, "----iEventsNumber = %d----\n\n", pTmpEventTable->iEventsNumber);
		PrintEventRegionInfo(test_file_name, pEventsRegionTable);
	#else
		PrintEventRegionInfo(stdout, pEventsRegionTable);
	#endif
	//PrintEventRegionInfo(stdout, pEventsRegionTable);
	ALG_DEBUG("\n\n\n");
    
#endif

    free(pTmpEventTable);
}

// 找出上升区间和下降区间
void PeakRegion(
    float *pInputSignal, //输入的对数信号
    const int iSignalLength, //信号长度
    const float fLowThr,
    const float fHighThr,
    int pRiseRegion[],
    int pFallRegion[])
{
    int i = 0;
    int iState = 0;
    int iBreakFlag = 0; //1跳出for循环
    //初始化上升与下降区间
    pRiseRegion[0] = -1;
    pRiseRegion[1] = -1;
    pFallRegion[0] = -1;//rise_region内两个元素分别为：上升区超过thr_low索引和上升区超过thr_high索引，默认为-1
    pFallRegion[1] = -1;
    for (i = 0; i < iSignalLength; ++i)//该状态机用于查找波峰的上升沿区间和下降沿区间
    {
        switch (iState)
        {
        case 0:
        {
                  if (pInputSignal[i] > fHighThr)
                  {
                      pRiseRegion[0] = i;
                      pRiseRegion[1] = i;
                      iState = 2;
                  }
                  else
                  {
                      if (pInputSignal[i] > fLowThr)
                      {
                          pRiseRegion[0] = i;
                          iState = 1;
                      }
                  }
                  break;
        }
        case 1:
        {
                  if (pInputSignal[i] > fHighThr)
                  {
                      pRiseRegion[1] = i;
                      iState = 2;
                  }
                  else
                  {
                      if (pInputSignal[i] < fLowThr)
                          iState = 0;
                  }
                  break;
        }
        case 2:
        {
                  if (pInputSignal[i] < fLowThr)
                  {
                      pFallRegion[0] = i;
                      pFallRegion[1] = i;
                      iBreakFlag = 1;
                  }
                  else
                  {
                      if (pInputSignal[i] < fHighThr)
                      {
                          pFallRegion[0] = i;
                          iState = 3;
                      }
                  }
                  break;
        }
        case 3:
        {
                  if (pInputSignal[i] >= fHighThr)
                  {
                      pFallRegion[0] = i;
                      iState = 2;
                  }
                  else
                  {
                      if (pInputSignal[i] < fLowThr)
                      {
                          pFallRegion[1] = i;
                          iBreakFlag = 1;
                      }
                  }
                  break;
        }
        default: iState = 0;
        }
        if (iBreakFlag) break;
    }
}

// 获取事件主要特征参数
void GetEventsParameter(
	float *pInputSignal,                    //输入的对数信号
	const int iSignalLength,                //信号长度
	EVENTS_REGION_TABLE *pEventsRegionTable,//事件信息
	EVENTS_TABLE *pEventsTable,             //最终事件表信息
	float fSampleRating)
{
    int i = 0;
    float pCoef[MAX_EVENTS_NUMBER][2] = { { 0.0f } };//数据拟合值
    float pLoss[MAX_EVENTS_NUMBER][2] = { { 0.0f } };//事件损耗信息
    float pReflect[MAX_EVENTS_NUMBER] = { 0.0f }; //反射率
    float pDelta[MAX_EVENTS_NUMBER] = { 0.0f };//最大最小值差
    int pEventsFiberLength[MAX_EVENTS_NUMBER] = { 0 }; //后续光纤损耗长度
    float fTotalLoss = 0.0f;
	float standardAttenuation[3] = {0.33f, 0.18f, 0.20f};				// 不同波长标准衰减
    GetEventsTableLoss(pInputSignal, iSignalLength, pEventsRegionTable, pCoef, pLoss, fSampleRating);
    GetReflectAndLength(pInputSignal, iSignalLength, frontParament.fRelay,
        pEventsRegionTable, pCoef, pReflect, pDelta, pEventsFiberLength, fSampleRating);   
 
    pLoss[0][0] = 0.0f;
    for (i = 0; i < pEventsRegionTable->iEventsNumber; ++i)
    {
        if (i == 0)
        {
            pEventsTable->EventsInfo[i].iBegin = 0;
            pEventsTable->EventsInfo[i].iEnd = pEventsRegionTable->iBlind;
        }
        else
        {
            pEventsTable->EventsInfo[i].iBegin
				= pEventsRegionTable->EventsRegionInfo[i].iBegin;
            pEventsTable->EventsInfo[i].iEnd
				= pEventsRegionTable->EventsRegionInfo[i].iEnd;
        }
        pEventsTable->EventsInfo[i].iStyle
			= pEventsRegionTable->EventsRegionInfo[i].iStyle;
        pEventsTable->EventsInfo[i].fLoss = pLoss[i][0];

		int iStart1 = Max(pEventsRegionTable->EventsRegionInfo[i].iBegin - pEventsRegionTable->iBlind, 0);
		int iStart2 = Max(pEventsRegionTable->EventsRegionInfo[i + 1].iBegin - pEventsRegionTable->iBlind, 0);
		float fLen = (iStart2 - iStart1) * g_fLengthPerPoint;

		if (i < pEventsRegionTable->iEventsNumber - 1
			&& pEventsTable->EventsInfo[i].iStyle == 0
			&& frontParament.pulseWidth == 2)
		{
			float fMean1 = GetMeanFloat(pInputSignal + iStart1, pEventsRegionTable->iBlind);
			float fMean2 = GetMeanFloat(pInputSignal + iStart2, pEventsRegionTable->iBlind);
			float fMean = fMean1 - fMean2;

			if ((fLen < 600.0f) && (fMean < pLoss[i][0] + 1.0f)
				&& (fMean > pLoss[i][0] - 1.0f))
			{
				pEventsTable->EventsInfo[i].fLoss
					= fMean - standardAttenuation[frontParament.wave] * fLen / 1000.0f;
			}
		}
		/*
		if (i < pEventsRegionTable->iEventsNumber - 1
			&& pEventsTable->EventsInfo[i].iStyle == 0 
			&& GetMeanFloat(pInputSignal + Max(pEventsRegionTable->EventsRegionInfo[i].iBegin
				- pEventsRegionTable->iBlind, 0), pEventsRegionTable->iBlind)
				- GetMeanFloat(pInputSignal + Max(pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
					- pEventsRegionTable->iBlind, 0), pEventsRegionTable->iBlind) > 0)
		{
			if ((pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
				- pEventsRegionTable->EventsRegionInfo[i].iBegin) * g_fLengthPerPoint
				< 600.0f && frontParament.pulseWidth == 2)
			{
				pEventsTable->EventsInfo[i].fLoss = GetMeanFloat(pInputSignal
					+ Max(pEventsRegionTable->EventsRegionInfo[i].iBegin 
						- pEventsRegionTable->iBlind, 0), pEventsRegionTable->iBlind)
					- GetMeanFloat(pInputSignal + Max(pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
						- pEventsRegionTable->iBlind, 0), pEventsRegionTable->iBlind)
					- standardAttenuation[frontParament.wave] * (pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
						- pEventsRegionTable->EventsRegionInfo[i].iBegin) * g_fLengthPerPoint / 1000.0f;
			}
		}
		*/
		if ((i < pEventsRegionTable->iEventsNumber - 1) 
			&& (pEventsTable->EventsInfo[i].iStyle == 0) 
			&& (pEventsTable->EventsInfo[i + 1].iStyle == 0) 
			&& (frontParament.pulseWidth != ENUM_PULSE_10NS)
			&& (fLen < 600.0f)
			&& (pInputSignal[iStart1] - pInputSignal[iStart2] < 0.2f)) 
		{
			pEventsTable->EventsInfo[i].fLoss
				= Min(pLoss[i][0], pInputSignal[iStart1] - pInputSignal[iStart2]);
		}

        pEventsTable->EventsInfo[i].fDelta = pDelta[i];
        pEventsTable->EventsInfo[i].fReflect = pReflect[i];
        pEventsTable->EventsInfo[i].iContinueLength = pEventsFiberLength[i];
        pEventsTable->EventsInfo[i].fAttenuation
			= -pCoef[i][0] * 1000.0f / g_fLengthPerPoint;
        pEventsTable->EventsInfo[i].fEventsPosition
			= pEventsTable->EventsInfo[i].iBegin * g_fLengthPerPoint;
        pEventsTable->EventsInfo[i].fEventsEndPosition
			= pEventsTable->EventsInfo[i].iEnd * g_fLengthPerPoint;
        if (fabsf(pEventsTable->EventsInfo[0].fAttenuation) > 0.5f)
        {
            pEventsTable->EventsInfo[0].fAttenuation = 0.5f;
            pLoss[0][1] = pEventsTable->EventsInfo[0].fAttenuation
				* (pEventsTable->EventsInfo[1].iBegin -
                pEventsTable->EventsInfo[0].iBegin) / 1000.0f * g_fLengthPerPoint;
        }
        if (i != pEventsRegionTable->iEventsNumber - 1)
            pEventsTable->EventsInfo[i].fContinueAttenuation
                = pCoef[i][0] * (pEventsTable->EventsInfo[i].iBegin
					- pEventsTable->EventsInfo[i + 1].iBegin);

        if (i == 0)
        {
            fTotalLoss = 0.0f;
        }
        else
        {
            fTotalLoss += pLoss[i - 1][0] + pLoss[i - 1][1];
        }
        pEventsTable->EventsInfo[i].fTotalLoss = fTotalLoss;//累加损耗
        pEventsTable->iEventsNumber = pEventsRegionTable->iEventsNumber;
    }
}

// 一次线性拟合函数,纵坐标为整数（最小二乘法）
void LineFit(   ////成功返回零，失败返回非零值;
	float *pSignal,///待拟合信号;
	const int iSignalLength,//待拟合信号长度;
	const int iBegin,//横坐标起始点;
	float *pFitCoef)//返回拟合系数 pFitCoef[0] = a, pFitCoef[1] = b, y=a*x+b;
{
	float sx = 0, sy = 0, sxy = 0, sxx = 0;
	int i = 0;
	float iX = 0;
	const float fDelta = 10000.0f;//缩小量，防止溢出;

    if (iSignalLength <= 1)
    {
        pFitCoef[0] = 0;
        pFitCoef[1] = pSignal[0];
        return;
    }
	for (i = 0; i < iSignalLength; ++i)
	{
		iX = (iBegin + i) / fDelta;;
		sx = sx + iX;
		sy = sy + pSignal[i];
		sxy = sxy + iX * pSignal[i];
		sxx = sxx + iX * iX;
	}
	pFitCoef[0] = (iSignalLength * sxy - sx * sy) / (iSignalLength * sxx - sx * sx) / fDelta;
	pFitCoef[1] = (sxx * sy - sx * sxy) / (iSignalLength * sxx - sx * sx);
}

// 一次两点式拟合直线,纵坐标为整数;
void LineFitTwoPoint(
	float fPointA,//A点纵坐标;
	int iPointA,//A点横坐标;
	float fPointB,//B点纵坐标;
	int iPointB,//B点横坐标;
	float *pFitCoef)//返回拟合系数 pFitCoef[0] = a, pFitCoef[1] = b, y=a*x+b;
{
    if (abs(iPointA - iPointB) <= 0)
    {
        pFitCoef[0] = 0;
        pFitCoef[1] = fPointA;
        return;
    }
	pFitCoef[0] = (fPointA - fPointB) / (iPointA - iPointB + 1);
	pFitCoef[1] = fPointA - pFitCoef[0] * iPointA;
}

#if 0
// 获得事件列表的损耗
void GetEventsTableLoss(
	float *pInputSignal,
	const int iSignalLogLength,
	EVENTS_REGION_TABLE *pEventsRegionTable,
	float pCoef[][2],
	float pLoss[][2],
    float fSampleRating)
{
    int iEventsNumber = pEventsRegionTable->iEventsNumber;//事件数量;
    EVENTS_REGION_INFO *EventsTable = pEventsRegionTable->EventsRegionInfo;//事件表信息;

    int i = 0;
    float *pTmpFloat = NULL;
    int iFitNumber = 0;//拟合数据量;
    int iFitBegin = 0;//拟合横坐标开始点;
    float fMean = 0;//存储临时变量;
    float fMean1 = 0;
    float fMean2 = 0;
    float fTmp = 0;//临时变量;
    int iTmp = 0;
    int iPointNum = 20;
    int iIsNan = 0;
	int iIsInf = 0;
    float fAttenuation = 0.33f;
    
	float fStdNoise = GetStdFloat(pInputSignal + iSignalLogLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
	
    if (iEventsNumber == 1)//如果事件只有一个;
    {
        pTmpFloat = pInputSignal + EventsTable[0].iEnd;
        iFitNumber = iSignalLogLength - EventsTable[0].iEnd;
        iFitBegin = EventsTable[0].iEnd;
        LineFit(pTmpFloat, iFitNumber, iFitBegin, pCoef[0]);
        iIsNan = isnan(pCoef[0][0]);// added by sjt 2015.10.28
        iIsInf = isinf(pCoef[0][0]);// added by sjt 2015.10.28
        if ((iIsNan) || (iIsInf))
        {
            pCoef[0][0] = 0;
            pCoef[0][1] = pInputSignal[EventsTable[0].iBegin];
        }
        
        pLoss[0][0] = pCoef[0][0] + pCoef[0][1];//前损耗;
        pLoss[0][1] = pCoef[0][0] * EventsTable[0].iEnd - pCoef[0][0] * iSignalLogLength;
    }
    else
    {
        int iTempLength = 0;
        for (i = 0; i < iEventsNumber - 1; i++)
        {
            int EventInterval = (int)((EventsTable[i + 1].iBegin - EventsTable[i].iEnd) / 4);
            iTempLength = Max(floor(EventInterval / 2), 1);
            // 60个点以上拟合较为准确
            if (EventInterval > 60)
            {
                pTmpFloat = pInputSignal + EventsTable[i].iEnd + 2 * iTempLength;
				iFitNumber = 5 * iTempLength - 1;
                iFitNumber = Min(iFitNumber, (int)(5000.0f / g_fLengthPerPoint));
                /*iFitNumber = 3 * EventInterval - 1;*/
				iFitBegin = EventsTable[i].iEnd + 2 * iTempLength;
				LineFit(pTmpFloat, iFitNumber, iFitBegin, pCoef[i]);

                iIsNan = isnan(pCoef[i][0]);// added by sjt 2015.10.28
    		    iIsInf = isinf(pCoef[i][0]);// added by sjt 2015.10.28
    		    if ((iIsNan) || (iIsInf))
                {
                    pCoef[i][0] = 0;
                    pCoef[i][1] = pInputSignal[EventsTable[i].iBegin];
                }
                fAttenuation = -pCoef[i - 1][0] * 1000.0f / g_fLengthPerPoint;//2019.05.10
                /*
                float coef[2] = { 0 };
                if (EventInterval < 250)
                {
                    fMean1 = GetMeanFloat(pInputSignal + EventsTable[i].iEnd + 2 * iTempLength, iTempLength);
                    fMean2 = GetMeanFloat(pInputSignal + EventsTable[i + 1].iBegin - iTempLength, iTempLength);
                    LineFitTwoPoint(fMean2, EventsTable[i + 1].iBegin - iTempLength,
						fMean1, EventsTable[i].iEnd + 2 * iTempLength, coef);

					float StandardAttenuation = 0.0f;
                    if (frontParament.wave == 0)
                    {
                        StandardAttenuation = 0.38f;
                    }
                    else
                    {
                        StandardAttenuation = 0.18f;
                    }

                    if (fabsf(fabsf(coef[0] * 1000 / g_fLengthPerPoint) - StandardAttenuation)
                        < fabsf(fabsf(pCoef[i][0] * 1000 / g_fLengthPerPoint) - StandardAttenuation))
                    {
                        pCoef[i][0] = coef[0];
                        pCoef[i][1] = coef[1];
                    }
                }
                */

                // S点
                if (i == 0)
                {
                /*
                    pTmpFloat = pInputSignal + EventsTable[i].iBegin + EventInterval;
                    iFitNumber = Max(2 * EventInterval - 1, 1);
                    iFitBegin = EventsTable[i].iBegin + EventInterval;
                    LineFit(pTmpFloat, iFitNumber, iFitBegin, pCoef[i]);

                    iIsNan = isnan(pCoef[i][0]);// added by sjt 2015.10.28
        		    iIsInf = isinf(pCoef[i][0]);// added by sjt 2015.10.28
        		    if ((iIsNan) || (iIsInf))
                    {
                        pCoef[i][0] = 0;
                        pCoef[i][1] = pInputSignal[EventsTable[i].iBegin];
                    }

                    if (pCoef[i][0] > 0)
                    {
                        if (frontParament.wave == 0)
                        {
                            pLoss[i][1] = -0.38 * g_fLengthPerPoint / 1000 * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);
                        }
                        else
                        {
                            pLoss[i][1] = -0.18 * g_fLengthPerPoint / 1000 * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);
                        }

                    }
                    else
                    {
                        pLoss[i][1] = pCoef[i][0] * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);

                    }
                    */
                    pLoss[i][1] = pCoef[i][0] * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);
                    pLoss[i][0] = 0.0f;

                    /*
                    if ((fAttenuation >= Normal_fAttenuation_range[0])
                        && (fAttenuation <= Normal_fAttenuation_range[1]))
                    {
                        pLoss[i][1] = pCoef[i][0] * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);
                    }
                    else
                    {
                        
                    }
                    */   
                }
                else
                {
                    if ((i == 1) && (fabs(pCoef[i - 1][0] * 1000.0f / g_fLengthPerPoint) > 0.5f))
                    {
                        if (fabs(pCoef[i][0] * 1000.0f / g_fLengthPerPoint) > 0.5f)
                        {
                            pLoss[i][0] = pInputSignal[EventsTable[i].iBegin] - pInputSignal[EventsTable[i].iEnd];
                        }
                        else
                        {
                            if (EventsTable[i].iBegin*g_fLengthPerPoint > 20.0f)
                            {
                                pTmpFloat = pInputSignal + Max(EventsTable[i].iBegin - (int)(10.0f / g_fLengthPerPoint), 1);
                                fMean1 = GetMeanFloat(pTmpFloat, Max((int)(10.0f / g_fLengthPerPoint), 1));
                                pLoss[i][0] = fMean1 - (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);
                            }
                            else
                            {
                                pLoss[i][0] = pInputSignal[EventsTable[i].iBegin] - pInputSignal[EventsTable[i].iEnd];
                            }
                        }

                    }
                    else
                    {
                        pLoss[i][0] = (pCoef[i - 1][0] * EventsTable[i].iBegin + pCoef[i - 1][1]) - (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);//计算前端事件损耗;
                        if (fabs(pCoef[i - 1][0] * 1000.0f / g_fLengthPerPoint) > 3.0f)
                        {
                            pLoss[i][0] = Min(pInputSignal[EventsTable[i].iBegin] - pInputSignal[EventsTable[i].iEnd], pLoss[i][0]);
                        }
                    }
                    pLoss[i][1] = pCoef[i][0] * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);

                }
            }
            else
            {
                // 计算后一个事件前段均值
                pTmpFloat = pInputSignal + EventsTable[i + 1].iBegin - iTempLength;
                float BliMean = GetMeanFloat(pTmpFloat, iTempLength);
                // 计算前一个事件后段均值
                pTmpFloat = pInputSignal + EventsTable[i].iEnd + iTempLength;
                float PreMean = GetMeanFloat(pTmpFloat, iTempLength);
                LineFitTwoPoint(BliMean, EventsTable[i + 1].iBegin - iTempLength, PreMean,
					EventsTable[i].iEnd + iTempLength, pCoef[i]);

                if (i == 0)
                {
                    if (pCoef[i][0] > 0)
                    {
                        if (frontParament.wave == 0)
                        {
                            pLoss[i][1] = -0.38 * g_fLengthPerPoint / 1000 
								* (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);
                        }
                        else
                        {
                            pLoss[i][1] = -0.18 * g_fLengthPerPoint / 1000
								* (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);
                        }

                    }
                    else
                    {
                        pLoss[i][1] = pCoef[i][0] * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);

                    }
                    pLoss[i][0] = 0;
                }
                else
                {
                	pTmpFloat = pInputSignal + Max(EventsTable[i].iBegin
							- (int)(10.0f / g_fLengthPerPoint), 1);
                    fMean1 = GetMeanFloat(pTmpFloat, Max((int)(10.0f / g_fLengthPerPoint), 1));
						
                    if ((i == 1) && (fabs(pCoef[i - 1][0] * 1000.0f / g_fLengthPerPoint) > 0.5f))
                    {
                        pLoss[i][0] = fMean1 - (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);
                    }
                    else
                    {
                    /*
                        pLoss[i][0] = (pCoef[i - 1][0] * EventsTable[i].iBegin + pCoef[i - 1][1])
							- (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);
							*/
						pLoss[i][0] = (pCoef[i - 1][0] * EventsTable[i].iBegin + pCoef[i - 1][1])
									- Max(fStdNoise, BliMean);
						pLoss[i][0] = Min(fMean1 - Max(fStdNoise, BliMean), pLoss[i][0]);
                    }
                    pLoss[i][1] = pCoef[i][0] * (EventsTable[i].iBegin - EventsTable[i + 1].iBegin);
                    pLoss[i][0] = Min(pLoss[i][0], pInputSignal[Max(EventsTable[i].iBegin - 1, 0)]
						- pInputSignal[Max(EventsTable[i + 1].iBegin - 1, 0)]);

                    printf("TP-1:i = %d,pLoss[%d][0] = %f\n", i, i, pLoss[i][0]);

                    if ((i > 1) && (fabs(pCoef[i - 1][0] * 1000.0f / g_fLengthPerPoint) > 3.0f)
                        && (fabs(pCoef[i][0] * 1000.0f / g_fLengthPerPoint) > 1.0f))
                    {
                        pLoss[i][0] = Min(pLoss[i][0], pInputSignal[Max(EventsTable[i].iBegin - 1, 0)]
							- pInputSignal[Max(EventsTable[i].iEnd, 0)]);
                        printf("TP-2:i = %d,pLoss[%d][0] = %f\n", i, i, pLoss[i][0]);
                    }
                }
            }
        }

        int j = 0;
        if (iSignalLogLength - EventsTable[i].iEnd > 6 * iPointNum)
        {
            iTempLength = Min((EventsTable[i].iEnd - EventsTable[i].iBegin), 6 * iPointNum);
            iFitNumber = Min(400, 3 * iTempLength);
            iFitNumber = Min(iFitNumber, iSignalLogLength - EventsTable[i].iEnd - iTempLength);
            iFitNumber = Max(iFitNumber, 0);
            iFitBegin = Min(EventsTable[i].iEnd + iTempLength, iSignalLogLength - 1);
            pTmpFloat = pInputSignal + iFitBegin;
            LineFit(pTmpFloat, iFitNumber, iFitBegin, pCoef[i]);
            iIsNan = isnan(pCoef[i][0]);// added by sjt 2015.10.28
		    iIsInf = isinf(pCoef[i][0]);// added by sjt 2015.10.28
		    if ((iIsNan) || (iIsInf))
            {
                pCoef[i][0] = 0;
                pCoef[i][1] = pInputSignal[EventsTable[i].iBegin];
            }      
		    
            pLoss[i][0] = (pCoef[i - 1][0] * EventsTable[i].iBegin
                + pCoef[i - 1][1]) - (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);
            
            for (j = 0; j < 5; j++)
            {
                if (pLoss[i][0] < 0.0f)
                {
                    iTempLength = Min(iFitNumber + iTempLength, iSignalLogLength - iFitNumber - 1);
                    iFitBegin = iTempLength;
                    LineFit(pTmpFloat + iTempLength, iFitNumber, iFitBegin, pCoef[i]);

                    iIsNan = isnan(pCoef[i][0]);// added by sjt 2015.10.28
        		    iIsInf = isinf(pCoef[i][0]);// added by sjt 2015.10.28
        		    if ((iIsNan) || (iIsInf))
                    {
                        pCoef[i][0] = 0;
                        pCoef[i][1] = pInputSignal[EventsTable[i].iBegin];
                    } 
                    pLoss[i][0] = (pCoef[i - 1][0] * EventsTable[i].iBegin
                        + pCoef[i - 1][1]) - (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);

                }
                else
                    break;
            }
            pLoss[i][1] = (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1])
                - (pCoef[i][0] * iSignalLogLength + pCoef[i][1]);
        }
        else
        {
            pTmpFloat = pInputSignal + EventsTable[i].iEnd;
            iTempLength = EventsTable[i].iEnd - EventsTable[i].iBegin;
            iFitNumber = Min(400, 3 * iTempLength);
            iFitNumber = Min(iFitNumber, iSignalLogLength - EventsTable[i].iEnd);
            fMean = GetMeanFloat(pTmpFloat, iFitNumber);//计算平均值;
            fTmp = pCoef[i - 1][0] * EventsTable[i].iBegin + pCoef[i - 1][1];
            LineFitTwoPoint(fMean, iSignalLogLength, fTmp, EventsTable[i].iBegin, pCoef[i]);
            pLoss[i][0] = pCoef[i][0] * EventsTable[i].iBegin - pCoef[i][0] * iSignalLogLength;
            pLoss[i][1] = 0;
        }
    }

    // 针对衰减率非常大的事件重新计算事件损耗
	float standardAttenuation[3] = { 0.33, 0.18, 0.20 };
	float normalAttenuation[][2] = { { 0.246f, 0.40f }, { 0.15f, 0.21f }, { 0.17f, 0.23f } };

    for (i = 1; i<iEventsNumber; i++)
    {
        if (i == iEventsNumber - 1)
        {
            continue;
        }
		else if ((fabsf(pCoef[i][0] * 1000 / g_fLengthPerPoint) < normalAttenuation[frontParament.wave][0]) || 
			fabsf(pCoef[i][0] * 1000 / g_fLengthPerPoint) > normalAttenuation[frontParament.wave][1])
        {
            float flossOffset = (EventsTable[i + 1].iBegin - EventsTable[i].iBegin)
                * g_fLengthPerPoint / 1000 * standardAttenuation[frontParament.wave];
			float fTmp1 = 0;
			float fTmp2 = 0;
			float fTmp3 = 0; 
			float fTmp4 = 0; 
			float fTmp5 = 0;
			int meanpoint = (int)(21 / frontParament.pulseWidth);
            
            if ((EventsTable[i].iBegin >= 3 * pEventsRegionTable->iBlind) 
                && (EventsTable[i].iBegin >= meanpoint * pEventsRegionTable->iBlind)
                && (EventsTable[i+1].iBegin >= meanpoint * pEventsRegionTable->iBlind))
            {
                fTmp1 = GetMinFloat(pInputSignal + EventsTable[i].iBegin
                    - 3 * pEventsRegionTable->iBlind, 3 * pEventsRegionTable->iBlind);
                fTmp2 = GetMinFloat(pInputSignal + EventsTable[i + 1].iBegin
                    - 3 * pEventsRegionTable->iBlind, 3 * pEventsRegionTable->iBlind);
				fTmp3 = GetMeanFloat(pInputSignal + EventsTable[i+1].iBegin
					- meanpoint * pEventsRegionTable->iBlind, meanpoint * pEventsRegionTable->iBlind);
				fTmp4 = GetMeanFloat(pInputSignal + EventsTable[i].iBegin
					- meanpoint * pEventsRegionTable->iBlind, meanpoint * pEventsRegionTable->iBlind);
				if (fTmp4-fTmp1>0.05)
				{
					fTmp4 = GetMeanFloat(pInputSignal + EventsTable[i].iBegin - 50, 50);
				}
				

				if (fTmp2 < fStdNoise)
				{
					fTmp2 = fStdNoise;
				}
				if (fTmp3 < fStdNoise)
				{
					fTmp3 = fStdNoise;
				}
				if (((i + 1) == iEventsNumber - 1) 
					&& ((EventsTable[i + 1].iStyle == 0) || (EventsTable[i + 1].iStyle == 10))
					&& (fabsf(pCoef[i][0] * 1000 / g_fLengthPerPoint) < 3))
				{
					int FakeEnd = Max(Min(EventsTable[i].iEnd + 5 * pEventsRegionTable->iBlind,
						EventsTable[i + 1].iBegin - 10 * fSampleRating - 20), 
						EventsTable[i].iEnd + (EventsTable[i + 1].iBegin - EventsTable[i].iEnd) / 2);
					fTmp5 = GetMeanFloat(pInputSignal + FakeEnd - Min(20, (EventsTable[i + 1].iBegin - EventsTable[i].iEnd) / 2), 
						2 * Min(20, (EventsTable[i + 1].iBegin - EventsTable[i].iEnd) / 2));
					flossOffset = (FakeEnd - EventsTable[i].iBegin)
						* g_fLengthPerPoint / 1000 * standardAttenuation[frontParament.wave];
					pLoss[i][0] = fTmp4 - fTmp5 - flossOffset;
				}
				else
				{
					if (EventsTable[i + 1].iBegin - EventsTable[i].iBegin > Max(480, 80 * fSampleRating)
						&& (fabsf(pCoef[i][0] * 1000 / g_fLengthPerPoint) < 3))
					{
						pLoss[i][0] = fTmp1 - fTmp3 - flossOffset;
					}
					else
					{
						pLoss[i][0] = fTmp1 - fTmp2 - flossOffset;
					}
				}
            }
            else
            {
            	if (pInputSignal[EventsTable[i + 1].iBegin] < fStdNoise)
            	{
            		pLoss[i][0] = pInputSignal[EventsTable[i].iBegin]
                   		- fStdNoise - flossOffset;
            	}
				else
				{
					pLoss[i][0] = pInputSignal[EventsTable[i].iBegin]
                   		- pInputSignal[EventsTable[i + 1].iBegin] - flossOffset;
				}

			}
		}
		
		if ((EventsTable[i + 1].iBegin - EventsTable[i].iEnd
			< Max(pEventsRegionTable->iBlind, (int)(10.0f / g_fLengthPerPoint)))
			&& ((pLoss[i][0] < 0.0f) || (pLoss[i][0] > frontParament.fFiberEndThreshold)))
		{
			fMean1 = GetMeanFloat(pInputSignal + Max(EventsTable[i].iBegin - 1, 0), 3);
            fMean2 = GetMeanFloat(pInputSignal + Max(EventsTable[i + 1].iBegin - 1, 0), 3);

			if (fMean2 < fStdNoise)
			{
				fMean2 = fStdNoise;
			}
			pLoss[i][0] = fMean1 - fMean2;
		}
	}
	// 针对损耗小于0.1db的反射和损耗事件进行重新计算
	for (i = 1; i < iEventsNumber; i++)
	{
		if (i == iEventsNumber - 1)
		{
			continue;
		}
		else if ((pLoss[i][0] < 0.1) && ((EventsTable[i].iStyle == 0) || (EventsTable[i].iStyle == 2)))
		{
			fMean1 = GetMeanFloat(pInputSignal + Max(EventsTable[i].iBegin - 10, 0), 10);
			fMean2 = GetMeanFloat(pInputSignal + Max(EventsTable[i + 1].iBegin - 10, 0), 10);
			int Len = Max(EventsTable[i + 1].iBegin - EventsTable[i].iBegin, 1);
			pLoss[i][0] = Max((fMean1 - fMean2) / Len * (EventsTable[i].iEnd - EventsTable[i].iBegin + 1), pLoss[i][0]);
		}
    }

    if (pEventsRegionTable->iBlind / fSampleRating > 300)
    {
        for (i = 1; i < iEventsNumber; i++)
        {
            if ((pLoss[i][0] < 0) && (EventsTable[i].iStyle == 0))
            {
                fMean1 = GetMeanFloat(pInputSignal + Max(EventsTable[i].iBegin - 10, 0), 10);
                iTmp = Min(EventsTable[i].iEnd + 10, iSignalLogLength);
                fMean2 = GetMeanFloat(pInputSignal + iTmp, iTmp - EventsTable[i].iEnd + 1);
                pLoss[i][0] = fMean1 - fMean2 - 0.2f * 2.5667f 
					/ fSampleRating*(EventsTable[i].iEnd - EventsTable[i].iBegin + 10) / 1000;
            }
        }
    }

    for (i = 1; i < iEventsNumber; i++)
    {
        if (fabsf(pLoss[i][0]) > 100.0f)
        {
            pLoss[i][0] = 100.0f;
        }
    }
}
#endif

// 获得事件列表的损耗
void GetEventsTableLoss(
	float *pInputSignal, 					 //对数值原始信号;
	const int iSignalLogLength,				 //对数值原始信号长度;
	EVENTS_REGION_TABLE *pEventsRegionTable, //前置事件表信息;
	float pCoef[][2],						 //返回拟合参数;
	float pLoss[][2],						 //返回事件损耗.pLoss[][0] 前损耗 pLoss[][1]区间损耗;
	float fSampleRating)					 //采样比率
{
	int iEventsNumber = pEventsRegionTable->iEventsNumber;					//事件数量;
	EVENTS_REGION_INFO *EventsTable = pEventsRegionTable->EventsRegionInfo; //事件表信息;
	float tmpCoef[MAX_EVENTS_NUMBER][2] = { {0.0f, 0.0f} };
	float tmpLoss[MAX_EVENTS_NUMBER][2] = { {0.0f, 0.0f} };
	//float stdNoise = GetStdFloat(pInputSignal
	//	+ Max(iSignalLogLength - BLACK_LEVEL_LEN, 0), BLACK_LEVEL_LEN);
	float stdNoise = GetMaxFloat(pInputSignal
			+ Max(iSignalLogLength - BLACK_LEVEL_LEN, 0), BLACK_LEVEL_LEN);
	int maxFitLength = (int)(6000 * 2.56f * fSampleRating); // 最多拟合15km
	int i = 0;
	float *pTmpFloat = NULL;
	int iFitNumber = 0, iFitBegin = 0;
	float normalAttenuation[][2] = {{0.30f, 0.36f}, {0.15f, 0.21f}, {0.17f, 0.23f}};// 不同波长衰减上下限
	int PrelsaFlag = 0;
	int BacklsaFlag = 0;
	if (iEventsNumber == 1)//如果事件只有一个;
	{
		pTmpFloat = pInputSignal + EventsTable[0].iEnd;
		iFitNumber = iSignalLogLength - EventsTable[0].iEnd;
		iFitBegin = EventsTable[0].iEnd;
		LineFit(pTmpFloat,iFitNumber,iFitBegin,pCoef[0]);
		pLoss[0][0] = pCoef[0][0] + pCoef[0][1];
		pLoss[0][1] = pCoef[0][0] * EventsTable[0].iEnd - pCoef[0][0] * iSignalLogLength;
	}
	else
	{
		int iTempLength = 0;
		float sigTemp1 = 0.0f, sigTemp = 0.0f;
		float standardAttenuation[3] = {0.33f, 0.18f, 0.20f};
		for (i = 0; i < iEventsNumber - 1; ++i)
		{
			PrelsaFlag = BacklsaFlag;
			BacklsaFlag = 0;
			int EventInterval = (int)((EventsTable[i + 1].iBegin - EventsTable[i].iEnd) / 4);
			iTempLength = (int)floor(EventInterval / 2);
			iTempLength = Min(iTempLength, maxFitLength / 3);
			
			// 60个点以上拟合较为准确
			if (EventInterval > Max(100, (int)(10.0f * fSampleRating))
				&& EventInterval > pEventsRegionTable->iBlind)
			{	
				pTmpFloat = pInputSignal + EventsTable[i].iEnd + 2 * iTempLength;
				iFitNumber = Max(3 * EventInterval - 1 - pEventsRegionTable->iBlind, 1) ;
				iFitBegin = EventsTable[i].iEnd + 2 * iTempLength;

				if (iFitNumber > maxFitLength + pEventsRegionTable->iBlind)
				{
					iFitNumber = maxFitLength;
				}
				LineFit(pTmpFloat, iFitNumber, iFitBegin, pCoef[i]);
				BacklsaFlag = 1;
				CalEventFloss(pInputSignal, i, pEventsRegionTable,
					pCoef, pLoss, stdNoise, PrelsaFlag, BacklsaFlag);

				float fTmpAttn = -pCoef[i][0] * 1000.0f / g_fLengthPerPoint;
				if ((iFitNumber > 2000) && (EventInterval > pEventsRegionTable->iBlind)
					&& ((fTmpAttn > normalAttenuation[frontParament.wave][1])
					|| (fTmpAttn < normalAttenuation[frontParament.wave][0])))
				{
					pTmpFloat = pInputSignal + EventsTable[i].iEnd + 4 * iTempLength;
					iFitNumber = 2 * EventInterval - pEventsRegionTable->iBlind;
					iFitBegin = EventsTable[i].iEnd + 4 * iTempLength;

					if (iFitNumber > maxFitLength + pEventsRegionTable->iBlind)
					{
						iFitNumber = maxFitLength;
					}
					
					LineFit(pTmpFloat, iFitNumber, iFitBegin, tmpCoef[0]);
					CalEventFloss(pInputSignal, i, pEventsRegionTable,
						tmpCoef, tmpLoss, stdNoise, PrelsaFlag, BacklsaFlag);
					float fTmpAttn2 = -tmpCoef[i][0] * 1000.0f / g_fLengthPerPoint;

					if (fabsf(fTmpAttn - standardAttenuation[frontParament.wave])
						 > fabsf(fTmpAttn2 - standardAttenuation[frontParament.wave]))
					{
						pCoef[i][0] = tmpCoef[i][0];
						pCoef[i][1] = tmpCoef[i][1];
						pLoss[i][0] = tmpLoss[i][0];
						pLoss[i][1] = tmpLoss[i][1];
					}
				}	
			}
			else
			{
				// 计算后一个事件前段均值
				pTmpFloat = pInputSignal + EventsTable[i + 1].iBegin - iTempLength;
				sigTemp = GetMeanFloat(pTmpFloat, iTempLength);
				
				// 计算前一个事件后段均值
				pTmpFloat = pInputSignal + EventsTable[i].iEnd + iTempLength;
				sigTemp1 = GetMeanFloat(pTmpFloat, iTempLength);					
				
				LineFitTwoPoint(sigTemp, EventsTable[i + 1].iBegin - iTempLength,
					sigTemp1, EventsTable[i].iEnd + iTempLength, pCoef[i]);

				CalEventFloss(pInputSignal, i, pEventsRegionTable, pCoef,
					pLoss, stdNoise, PrelsaFlag, BacklsaFlag);	
			}

			tmpCoef[i][0] = pCoef[i][0];
			tmpCoef[i][1] = pCoef[i][1];
			tmpLoss[i][0] = pLoss[i][0];
			tmpLoss[i][1] = pLoss[i][1];

		}

		// 最后一个事件                  
		int iblind = (int)(blindWidth[frontParament.pulseWidth][0] * 5 / g_fLengthPerPoint);
		iblind = Min(iblind, iSignalLogLength - BLACK_LEVEL_LEN
			- (int)(iblind / 5) - 1 - EventsTable[i].iEnd);
		
		iFitBegin = EventsTable[i].iEnd + iblind;
		pTmpFloat = pInputSignal + iFitBegin;
		iFitNumber = iSignalLogLength - BLACK_LEVEL_LEN - iFitBegin;
		LineFit(pTmpFloat, iFitNumber, iFitBegin, pCoef[i]);

		float fPreAtten = -pCoef[i - 1][0] * 1000.0f / g_fLengthPerPoint;
		float fBackAtten = -pCoef[i][0] * 1000.0f / g_fLengthPerPoint;
		PrelsaFlag = BacklsaFlag;
		if ((PrelsaFlag) && (fPreAtten < normalAttenuation[frontParament.wave][1])
		&& (fPreAtten > normalAttenuation[frontParament.wave][0]))
		{
			if((fBackAtten < normalAttenuation[frontParament.wave][1])
			&& (fBackAtten > normalAttenuation[frontParament.wave][0]))
			{
				pLoss[i][0] = (pCoef[i - 1][0] * EventsTable[i].iBegin + pCoef[i - 1][1]) 
							- (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);
		
			}
			else
			{
				int tmpLen = Min(iFitNumber, 20);
				sigTemp = GetMeanFloat(pTmpFloat, tmpLen);
				int tmpOffset = Max((int)(5.12f / g_fLengthPerPoint), pEventsRegionTable->iBlind / 2);
				tmpOffset = Min(tmpOffset, 3 * pEventsRegionTable->iBlind / 2);

				if ((sigTemp < stdNoise + 2.5f) && ((EventsTable[i].iStyle == 0)
					|| (iFitBegin - EventsTable[i].iEnd >= (int)(8000.0f / g_fLengthPerPoint))))
				{
					sigTemp = GetMeanFloat(pInputSignal
						+ EventsTable[i].iEnd + tmpOffset - tmpLen / 2, tmpLen);
					iFitBegin = EventsTable[i].iEnd;
				}
				
				pLoss[i][0] = (pCoef[i - 1][0] * EventsTable[i].iBegin + pCoef[i - 1][1]) 
							- sigTemp + standardAttenuation[frontParament.wave] * g_fLengthPerPoint
							/ 1000.0f * (EventsTable[i].iBegin - iFitBegin);
			}
		}
		else
		{
			if ((fBackAtten < normalAttenuation[frontParament.wave][1])
			&& (fBackAtten > normalAttenuation[frontParament.wave][0]))
			{
				sigTemp = GetMinFloat(pInputSignal + EventsTable[i - 1].iEnd, 
						  EventsTable[i].iBegin - EventsTable[i - 1].iEnd + 1);
				pLoss[i][0] = sigTemp - (pCoef[i][0] * EventsTable[i].iBegin + pCoef[i][1]);

			}
			else
			{
				sigTemp = GetMinFloat(pInputSignal + EventsTable[i - 1].iEnd, 
						  EventsTable[i].iBegin - EventsTable[i - 1].iEnd + 1);
				sigTemp1= GetMeanFloat(pInputSignal + iFitBegin,
						  Min((int)(iblind / 5), iSignalLogLength - iFitBegin));
				sigTemp1 = Max(sigTemp1, stdNoise);
				
				pLoss[i][0] = sigTemp - sigTemp1;
			}
		}
		pLoss[i][1] = 0;
	}
}

// 计算事件损耗
void CalEventFloss(float *pInputSignal,
	int index,
	EVENTS_REGION_TABLE *pEventsRegionTable,
	float pCoef[][2],
	float pLoss[][2],
	float stdNoise,
	int PrelsaFlag,
	int BacklsaFlag)
{
	EVENTS_REGION_INFO *EventsTable = pEventsRegionTable->EventsRegionInfo;//事件表信息;
	// 当前事件衰减正常

	float sigTemp = 0.0f, sigTemp1 = 0.0f;
	float standardAttenuation[3] = {0.33f, 0.18f, 0.20f};				// 不同波长标准衰减
	float normalAttenuation[][2] = {{0.28f, 0.37f}, {0.15f, 0.21f}, {0.17f, 0.23f}};// 不同波长衰减上下限 1310修改已适应2+2km损耗一致性问题
	float fBackAtten = -pCoef[index][0] * 1000.0f / g_fLengthPerPoint;
	stdNoise = Max(stdNoise, 0.0f);
	//stdNoise = Min(stdNoise, 2.6f);
	int iNeedReCalFalg = 0;
	if (index == 0)
	{
		pLoss[index][1] = fBackAtten * (EventsTable[index].iBegin
			- EventsTable[index+1].iBegin) * g_fLengthPerPoint / 1000.0f;	
		pLoss[index][0] = 0;
	}
	else
	{
		int tmpOffset = Max((int)(5.12f / g_fLengthPerPoint), pEventsRegionTable->iBlind / 2);
		tmpOffset = Min(tmpOffset, 3 * pEventsRegionTable->iBlind / 2);
		int iRedOffsetFlag = 0;
		
		float fPreAtten = -pCoef[index - 1][0] * 1000.0f / g_fLengthPerPoint;

		if ((BacklsaFlag) && (fBackAtten < normalAttenuation[frontParament.wave][1])
		&& (fBackAtten > normalAttenuation[frontParament.wave][0]))
		{
			// 上一个事件衰减正常
			if ((PrelsaFlag) && (fPreAtten < normalAttenuation[frontParament.wave][1])
			&& (fPreAtten > normalAttenuation[frontParament.wave][0]))
			{
				pLoss[index][0] = (pCoef[index - 1][0] * EventsTable[index].iBegin + pCoef[index - 1][1]) 
					        - (pCoef[index][0] * EventsTable[index].iBegin + pCoef[index][1]);
				iNeedReCalFalg = 1;
			}
			else
			{
				if (PrelsaFlag)
				{
					int tmpLen = Min(20, Max(EventsTable[index].iBegin - EventsTable[index - 1].iEnd, 1));

					if (EventsTable[index].iBegin - EventsTable[index - 1].iEnd >= tmpLen + tmpOffset)
					{
						sigTemp = GetMeanFloat(pInputSignal 
							+ EventsTable[index].iBegin - tmpLen - tmpOffset, tmpLen);
						iRedOffsetFlag = 1; 
					}
					else
					{
						sigTemp = GetMeanFloat(pInputSignal
							+ EventsTable[index].iBegin - tmpLen, tmpLen);
					}
				}
				else
				{
					if (EventsTable[index - 1].iStyle == 0
						|| EventsTable[index].iBegin - EventsTable[index - 1].iEnd > 3 * pEventsRegionTable->iBlind)
					{
						int tmpLen = Min(20, Max(EventsTable[index].iBegin - EventsTable[index - 1].iEnd, 1));
						sigTemp = GetMeanFloat(pInputSignal
							+ EventsTable[index].iBegin - tmpLen, tmpLen);
					}
					else
					{
						sigTemp = GetMinFloat(pInputSignal + EventsTable[index - 1].iEnd,
							Max(EventsTable[index].iBegin - EventsTable[index - 1].iEnd + 1, 1));
					}
					
				}
				
				sigTemp = Max(stdNoise, sigTemp);
				pLoss[index][0] = sigTemp - (pCoef[index][0]
					* (EventsTable[index].iBegin - iRedOffsetFlag * tmpOffset) + pCoef[index][1]); 
			}
			pLoss[index][1] = pCoef[index][0] * (EventsTable[index].iBegin - EventsTable[index + 1].iBegin);
		}
		else
		{
			if ((PrelsaFlag) && (fPreAtten < normalAttenuation[frontParament.wave][1])
			&& (fPreAtten > normalAttenuation[frontParament.wave][0]))
			{
				int calLossEndPos = EventsTable[index + 1].iBegin;
				if (BacklsaFlag)
				{
					int tmpLen = Min(20, Max(calLossEndPos - EventsTable[index].iEnd, 1));
					if (calLossEndPos - EventsTable[index].iEnd >= tmpLen + tmpOffset)
					{
						sigTemp = GetMeanFloat(pInputSignal + calLossEndPos - tmpLen - tmpOffset, tmpLen);
						if ((sigTemp < stdNoise + 0.5f) && ((EventsTable[index].iStyle == 0)
							|| (calLossEndPos - EventsTable[index].iEnd >= (int)(8000.0f / g_fLengthPerPoint))))
						{
							sigTemp = GetMeanFloat(pInputSignal
								+ EventsTable[index].iEnd + tmpOffset - tmpLen / 2, tmpLen);
							calLossEndPos = EventsTable[index].iEnd;
						}
						iRedOffsetFlag = 1;
					}
					else
					{
						sigTemp = GetMeanFloat(pInputSignal + calLossEndPos - tmpLen / 2, tmpLen);
					}
				}
				else
				{
					sigTemp = GetMinFloat(pInputSignal + EventsTable[index].iEnd, 
						  Max(calLossEndPos - EventsTable[index].iEnd + 1, 1));
				}
				sigTemp = Max(stdNoise, sigTemp);
				float sigTemp1 = (pCoef[index - 1][0] * EventsTable[index].iBegin + pCoef[index - 1][1]);
				pLoss[index][0] = sigTemp1 - sigTemp + standardAttenuation[frontParament.wave]
					* g_fLengthPerPoint / 1000.0f * (EventsTable[index].iBegin
					- calLossEndPos + iRedOffsetFlag * tmpOffset);
			}
			else
			{
				int calLossPrePos = EventsTable[index].iBegin;
				int calLossEndPos = EventsTable[index + 1].iBegin;
				if (PrelsaFlag)
				{
					int tmpLen = Min(20, Max(calLossPrePos - EventsTable[index - 1].iEnd, 1));

					if (calLossPrePos - EventsTable[index - 1].iEnd >= tmpLen + tmpOffset)
					{
						sigTemp = GetMeanFloat(pInputSignal + calLossPrePos - tmpLen - tmpOffset, tmpLen);
						iRedOffsetFlag += 1; 
					}
					else
					{
						sigTemp = GetMeanFloat(pInputSignal + calLossPrePos - tmpLen, tmpLen);
					}
				}
				else
				{
					int iSt = Max(EventsTable[index - 1].iEnd, calLossPrePos - 2 * pEventsRegionTable->iBlind);
					sigTemp = GetMinFloat(pInputSignal + iSt, calLossPrePos - iSt + 1);
				}
				sigTemp = Max(stdNoise, sigTemp);
				
				int tmpL = Min(Max(3 * pEventsRegionTable->iBlind, 60), calLossEndPos - EventsTable[index].iEnd);
				tmpL = Min(tmpL, (int)(2500.0f / g_fLengthPerPoint));

				if (BacklsaFlag)
				{
					int tmpLen = Min(20, Max(calLossEndPos - EventsTable[index].iEnd, 1));
					if (calLossEndPos - EventsTable[index].iEnd >= tmpLen + tmpOffset)
					{
						if (EventsTable[index].iStyle == 0)
						{
							sigTemp1 = GetMeanFloat(pInputSignal + EventsTable[index].iEnd
								+ 2 * pEventsRegionTable->iBlind, tmpL);
							sigTemp = GetMeanFloat(pInputSignal + EventsTable[index].iBegin - tmpL, tmpL);
							calLossPrePos = EventsTable[index].iBegin - tmpL / 2;
							calLossEndPos = EventsTable[index].iEnd + 2 * pEventsRegionTable->iBlind;
						}
						else
						{
							sigTemp1 = GetMeanFloat(pInputSignal + calLossEndPos - tmpLen - tmpOffset, tmpLen);

							if ((sigTemp1 < stdNoise + 0.5f) && ((EventsTable[index].iStyle == 0)
								|| (calLossEndPos - EventsTable[index].iEnd >= (int)(8000.0f / g_fLengthPerPoint))))
							{
								sigTemp1 = GetMeanFloat(pInputSignal
									+ EventsTable[index].iEnd + tmpOffset - tmpLen / 2, tmpLen);
								calLossEndPos = EventsTable[index].iEnd;
							}
						}
						iRedOffsetFlag -= 1; 
					}
					else
					{
						if (EventsTable[index].iStyle == 0)
						{
							sigTemp1 = GetMeanFloat(pInputSignal + EventsTable[index].iEnd 
								+ 2 * pEventsRegionTable->iBlind, tmpL);
							sigTemp = GetMeanFloat(pInputSignal + EventsTable[index].iBegin - tmpL, tmpL);
							calLossEndPos = EventsTable[index].iEnd + 2 * pEventsRegionTable->iBlind;
							calLossPrePos = EventsTable[index].iBegin;
						}
						else
						{
							sigTemp1 = GetMeanFloat(pInputSignal + calLossEndPos - tmpLen, tmpLen);
						}
					}
				}
				else
				{
					if (EventsTable[index].iStyle == 0)
					{
						sigTemp1 = GetMinFloat(pInputSignal + Min(EventsTable[index].iEnd
							+ 3 * pEventsRegionTable->iBlind, EventsTable[index + 1].iBegin
							- pEventsRegionTable->iBlind), tmpL);
						sigTemp = GetMinFloat(pInputSignal + EventsTable[index].iBegin - tmpL, tmpL);
					}
					else
					{
						sigTemp1 = GetMinFloat(pInputSignal + EventsTable[index].iEnd, 
						  Max(EventsTable[index + 1].iBegin - EventsTable[index].iEnd + 1, 1));
					}
				}
				sigTemp1 = Max(stdNoise, sigTemp1);
				pLoss[index][0] = sigTemp - sigTemp1 + standardAttenuation[frontParament.wave]
					* g_fLengthPerPoint * (-tmpL - iRedOffsetFlag * tmpOffset) / 1000.0f;
			}
			pLoss[index][1] = - standardAttenuation[frontParament.wave] * g_fLengthPerPoint
				/ 1000.0f * (EventsTable[index].iBegin - EventsTable[index + 1].iBegin);
				      			  
		}
		if ((pLoss[index][0] < 0) && (EventsTable[index].iStyle != 1)
			&& (!iNeedReCalFalg))
		{	
			sigTemp  = GetMinFloat(pInputSignal + EventsTable[index - 1].iEnd, 
				EventsTable[index].iBegin - EventsTable[index - 1].iEnd + 1);
			sigTemp1 = GetMinFloat(pInputSignal + EventsTable[index].iEnd, 
				EventsTable[index + 1].iBegin - EventsTable[index].iEnd + 1);
			pLoss[index][0] = (sigTemp - sigTemp1) + standardAttenuation[frontParament.wave]
				* g_fLengthPerPoint / 1000.0f * (EventsTable[index].iBegin - EventsTable[index + 1].iBegin);
		}
	}	
}

// 求反射率和事件区间长度函数;
void GetReflectAndLength(
	float *pInputSignal, ///对数信号;
	const int iSignalLength, //信号长度;
	const float fRelay, //瑞利散射强度;
	EVENTS_REGION_TABLE *pEventsRegionTable, //事件表信息;
	float pCoef[][2], //拟合参数;
	float *pReflect, //反射率参数;
	float *pDelta, //事件区域最大最小值差距;
	int *pEventsFiberLength,//事件长度;
	float fSampleRating)
{
    int i = 0;
    float fMax = 0.0f;
    int iTmp = 0;
    int middle_point;
	//float fStdNoise = GetStdFloat(pInputSignal + iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
	float fMaxNoise = GetMaxFloat(pInputSignal + iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
	int fMaxIndex = 0;
	//int k;

    for (i = 0; i < pEventsRegionTable->iEventsNumber; ++i)
    {
        if (i == 0)
        {
        	if (i < pEventsRegionTable->iEventsNumber - 1)
    		{
				fMax = GetMaxFloat(pInputSignal + pEventsRegionTable->EventsRegionInfo[i].iBegin,
					Min(pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
						- pEventsRegionTable->EventsRegionInfo[i].iBegin + 1,
						Max(256, (int)(1.2f * pEventsRegionTable->iBlind))));
				fMaxIndex = GetMaxIndex(pInputSignal + pEventsRegionTable->EventsRegionInfo[i].iBegin,
					Min(pEventsRegionTable->EventsRegionInfo[i + 1].iBegin 
						- pEventsRegionTable->EventsRegionInfo[i].iBegin + 1,
						Max(256, (int)(1.2f * pEventsRegionTable->iBlind))))
					+ pEventsRegionTable->EventsRegionInfo[i].iBegin;
    		}
			else
			{
	            fMax = GetMaxFloat(pInputSignal + pEventsRegionTable->EventsRegionInfo[i].iBegin,
					Max(pEventsRegionTable->EventsRegionInfo[i].iEnd
						- pEventsRegionTable->EventsRegionInfo[i].iBegin + 1,
						Max(256, (int)(1.2f * pEventsRegionTable->iBlind))));
				fMaxIndex = GetMaxIndex(pInputSignal + pEventsRegionTable->EventsRegionInfo[i].iBegin,
					Max(pEventsRegionTable->EventsRegionInfo[i].iEnd 
						- pEventsRegionTable->EventsRegionInfo[i].iBegin + 1,
						Max(256, (int)(1.2f * pEventsRegionTable->iBlind))))
					+ pEventsRegionTable->EventsRegionInfo[i].iBegin;
			}
            pDelta[i] = fMax - Min((pCoef[i][0]
				* (Max(pEventsRegionTable->EventsRegionInfo[i].iBegin - 10, 0)) + pCoef[i][1]), 
				(pCoef[i][0] * pEventsRegionTable->EventsRegionInfo[i].iBegin + pCoef[i][1]));

            if (frontParament.pulseWidth >= ENUM_PULSE_10US && pDelta[i] < 0.1f)
        	{
				pDelta[i] = fMax - (pCoef[i][0] * Max(fMaxIndex, pEventsRegionTable->iBlind) + pCoef[i][1]);
        	}

            if (pDelta[i] <= 0.01f)
            {
                pReflect[i] = frontParament.fBackScattering;
            }
            else
            {

                pReflect[i] = fRelay + 10 * log10f(powf(10, pDelta[i] / 5.0f) - 1);

                if (pReflect[i] < frontParament.fBackScattering)
                {
                    pReflect[i] = frontParament.fBackScattering;
                }
            }
        }
        else
        {
            iTmp = Max(pEventsRegionTable->EventsRegionInfo[i - 1].iEnd,
                pEventsRegionTable->EventsRegionInfo[i].iBegin 
				- Min(pEventsRegionTable->iBlind, 256));
			/*
			if (frontParament.pulseWidth >= ENUM_PULSE_10US)
			{
				iTmp = Max(pEventsRegionTable->EventsRegionInfo[i - 1].iEnd,
					pEventsRegionTable->EventsRegionInfo[i].iBegin);
				float TempMaxValue = 0;
				// 一阶导数最大值的前端
				int tmpKS = Max(pEventsRegionTable->EventsRegionInfo[i].iBegin
					- pEventsRegionTable->iBlind, 0);
				int tmpKE = Max(pEventsRegionTable->EventsRegionInfo[i].iEnd - 1,
					pEventsRegionTable->EventsRegionInfo[i].iBegin);
				for (k = tmpKS; k < tmpKE; ++k)
				{
					if (pInputSignal[k + 1] - pInputSignal[k] > TempMaxValue)
					{
						TempMaxValue = pInputSignal[k + 1] - pInputSignal[k];
						iTmp = k - 5;
					}
				}
			}
			*/
			if (i < pEventsRegionTable->iEventsNumber - 1)
    		{
				fMax = GetMaxFloat(pInputSignal + iTmp,
				Min(pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
					- pEventsRegionTable->EventsRegionInfo[i].iBegin
					+ Min(pEventsRegionTable->iBlind, 256), 512));
				fMaxIndex = GetMaxIndex(pInputSignal + iTmp,
					Min(pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
						- pEventsRegionTable->EventsRegionInfo[i].iBegin
						+ Min(pEventsRegionTable->iBlind, 256), 512)) + iTmp;
    		}
			else
			{
				int calMaxLen = Max(pEventsRegionTable->EventsRegionInfo[i].iEnd
					- pEventsRegionTable->EventsRegionInfo[i].iBegin
					+ Min(pEventsRegionTable->iBlind, 256), 512);
				calMaxLen = Max(Min(calMaxLen, iSignalLength - iTmp - 1), 0);

				fMax = GetMaxFloat(pInputSignal + iTmp, calMaxLen);		
				fMaxIndex = GetMaxIndex(pInputSignal + iTmp, calMaxLen) + iTmp;
			}

			float fMin = GetMinFloat(pInputSignal + iTmp, Max(fMaxIndex - iTmp + 1, 1)); 
			fMin = Max(fMin, fMaxNoise);

			float fMean = GetMeanFloat(pInputSignal
				+ pEventsRegionTable->EventsRegionInfo[i].iBegin - 40, 20);
			if (fMean - fMin < 2.0f)
			{
				fMin = Max(fMin, fMean);
			}
            pDelta[i] = fMax - fMin;
			
            if (pDelta[i] <= 0.1f)
            {
                pReflect[i] = frontParament.fBackScattering;
            }
            else
            {
                pReflect[i] = fRelay + 10.0f * log10f(powf(10, pDelta[i] / 5.0f) - 1);
                middle_point = (int)floor((pEventsRegionTable->EventsRegionInfo[i].iBegin
                    + pEventsRegionTable->EventsRegionInfo[i].iEnd) / 2);
                if ((pReflect[i] > -40.0f) && (pInputSignal[middle_point]
					< pInputSignal[pEventsRegionTable->EventsRegionInfo[i].iBegin])
                    && (pInputSignal[middle_point] < pInputSignal[pEventsRegionTable->EventsRegionInfo[i].iEnd])
                    && (pEventsRegionTable->EventsRegionInfo[i].iStyle == 2))
                {
                    pReflect[i] = frontParament.fBackScattering;
                }
                if (pReflect[i] < frontParament.fBackScattering)
                {
                    pReflect[i] = frontParament.fBackScattering;
                }
            }
        }
        if (i == pEventsRegionTable->iEventsNumber - 1)
        {
            pEventsFiberLength[i] = iSignalLength
				- pEventsRegionTable->EventsRegionInfo[i].iBegin - 1;
        }
        else
        {
            pEventsFiberLength[i] = pEventsRegionTable->EventsRegionInfo[i + 1].iBegin
                - pEventsRegionTable->EventsRegionInfo[i].iBegin;
        }
    }
}

// 查找光纤末端，连续十个点小于末端检测阈值;
int SearchEnd( //返回末端点;
	float *pInputSignal, //对数信号;
	int iSignalLength, //光纤长度;
	float fEndThr)//末端检测阈值;
{
	int i = 0;
	int iCount = 0;//记录小于阈值数量;
	int iTail = -1;//记录光纤末端位置;
	int iState = 0;//状态机变量;

	//状态机，查找连续5个点小于末端阈值;
	for (i = 1; i < iSignalLength; ++i)
	{
		switch(iState)
		{
		case 0:
			{                     
                if (pInputSignal[i] < fEndThr)
				{
					iCount = 1;
					iState = 1;
				}
				else
				{
					iCount = 0;
					iState = 0;
				}
				break;
			}
		case 1:
			{
				if (pInputSignal[i] < fEndThr)
				{
					iCount++;
					if ((iCount >= 10) && (i > 40))// 防止过充被判断为光纤末端;
					{
						iTail = i - iCount;
						//寻找该区间的最大值索引
						if(iTail > 15)
						    iTail -= (15 - GetMaxIndex(pInputSignal + iTail - 15, 15) + 1);
					}
					iState = 1;
				}
				else
				{
					iCount = 0;
					iState = 0;
				}
				break;
			}
		default:
			{
				iCount = 0;
				iState = 0;
			}
		}
		if (iTail != -1) break;//iTail！=-1表示已经查找到光纤末端，跳出循环;
	}

	if(iTail == -1)
	{
		iTail = Max(iSignalLength - 1, 0);
	}
	return iTail;
}

// 末端分析中，查找大于损耗阈值的事件作为末端。优先级：损耗阈值>末端后面反射事件>末端前面事件;
void EndAnalysis(ALGORITHM_INFO *pAlgorithmInfo,
	const float fEndThr,
	int iTail)
{
    float *pInputSignal = pAlgorithmInfo->pSignalLog;
    EVENTS_TABLE *pEventsTable = &pAlgorithmInfo->EventsTable;
    int iSignalLength = pAlgorithmInfo->iSignalLength;
    int iDisplayLenth = pAlgorithmInfo->iDisplayLenth;
    int iBlind = pAlgorithmInfo->iBlind;
    float fSampleRating = pAlgorithmInfo->SetParameters.fSampleRating;
    int iTailFlag = 0;//%定义末端查找标志位。-1代表未找到末端，0代表末端不在事件上，1代表末端为中间某个事件;
    int iTrueTail = 0;
    int i = 0;
    int j = 0;
    int iTailIndex = 0;//末端索引;
    int iStart = 0;
    float pCoef[2] = { 0.0f };
    float fDelta = 0.0f;
    float fTemp = 0.0f;
    float fTemp1 = 0.0f;
    float fTemp2 = 0.0f;
    float fMean = 0.0f;
	int fEndTail = iTail;
    //float fTempDelta = 0.0f;
    int iPointNum = (int)(10.0f * fSampleRating);//40M采样率下10个点对应的当前采样率下的数据个数
    int iTailPrecision = 0;
    //pEventsTable->EventsInfo[0].fAttenuation = 1.5f;

#ifdef DEBUG_EVENTINFO
	ALG_DEBUG("------------------EndAnalysis-----------------------\n\n");
	#if ISSERIAL == 1
		fprintf(test_file_name, "------------------EndAnalysis-----------------------\n\n");
		fprintf(test_file_name, "fEndThr = %f,iTail = %d,iDisplayLenth = %d,iSignalLength = %d\n",
			fEndThr, iTail, iDisplayLenth, iSignalLength);
		fprintf(test_file_name, "---iPointNum = %d---iBlind = %d---fSampleRating = %f----\n",
			iPointNum, iBlind, fSampleRating);
		fprintf(test_file_name, "range = %f, solaEffectiveRange = %f, EndThrNoise = %f\n",
			range, pAlgorithmInfo->solaEffectiveRange, EndThrNoise);
	#endif
	ALG_DEBUG("\n\n\n");
#endif

    // 5ns将末端定位在最后一个不是鬼影的事件上
    if ((pAlgorithmInfo->PulseWidth == ENUM_PULSE_5NS)
		&& (pEventsTable->iEventsNumber > 1))
    {
    	for (i = pEventsTable->iEventsNumber - 1; i > 0; --i)
		{
			if (pEventsTable->EventsInfo[i].iStyle <= 2)
			{
				pEventsTable->EventsInfo[i].iStyle += 10;
				pEventsTable->iEventsNumber = i + 1;
				break;
			}
		}
        return;
    }

    // 未找到末端点，认为连续光纤事件
    int continueFlag = 0;
    if (iTail == iDisplayLenth - 1)
    {
        continueFlag = 1;
        if (pEventsTable->iEventsNumber == 1)
        {
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iBegin = iDisplayLenth - 2;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iEnd = iDisplayLenth - 1;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iStyle = 20;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fLoss = 0.0f;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fDelta = 0.0f;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fReflect = 0.0f;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iContinueLength = 0;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fAttenuation = 0;
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fContinueAttenuation = 0;
            pEventsTable->iEventsNumber++;
            return;
        }
    }

    if ((iTail >= 0) && (pEventsTable->iEventsNumber > 1))
    {
        // 首先回溯判断末端是否在拖尾上，若在拖尾上，则找到拖尾前端作为末端值
        // 找到末端前方的事件，并计算两者的距离
        if (iDisplayLenth * g_fLengthPerPoint <= 3000.0f)
        {
        	//短距离拖尾严重 F216拖尾不同于F214 F210 且信噪比很差，且FPGA纹波导致衰减
        	//、损耗计算偏差比较大，配合前面的小波阈值很大，故可以这样使用
            iTail = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin;
            if (iDisplayLenth * g_fLengthPerPoint <= 1500.0f)
            {
                iTailIndex = pEventsTable->iEventsNumber - 1;
            }
        }
        else
        {
            iTailPrecision = (int)(Min(iTail / 4, 10000.0f / g_fLengthPerPoint));
            iStart = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin;//以该点作为起始值，查找末端
            if (fabs(iTail - iStart) < iTailPrecision)
            {
                iTail = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin;
            }
        }
    }

    if (iTailFlag == 0)
    {
        if (pEventsTable->iEventsNumber > 1)
        {
            for (i = 1; i < pEventsTable->iEventsNumber; i++)//查找最大损耗点,判断是否存在阈值末端;
            {
				if ((pEventsTable->EventsInfo[i].fLoss > fEndThr)
					&& (pEventsTable->EventsInfo[i].iBegin < iDisplayLenth))
                {
                    if ((pEventsTable->EventsInfo[i].iBegin
						- pEventsTable->EventsInfo[i - 1].iEnd < Max(iBlind, iPointNum))
                        && (pEventsTable->EventsInfo[i - 1].iStyle == 2)
                        && (fabsf(pEventsTable->EventsInfo[i - 1].fAttenuation) > 1.0f)
                        && ((pEventsTable->EventsInfo[i].fAttenuation < 0.5f)
                        && (pEventsTable->EventsInfo[i].fAttenuation > 0.08f)))
                    {
                        continue;
                    }
                    else
                    {
                        fTemp = GetMaxFloat(pInputSignal + pEventsTable->EventsInfo[i].iBegin,
                            pEventsTable->EventsInfo[i].iEnd - pEventsTable->EventsInfo[i].iBegin);
                        if (pEventsTable->EventsInfo[i].iStyle == 0)
                        {
                            fTemp1 = GetMeanFloat(pInputSignal + pEventsTable->EventsInfo[i].iBegin - 10, 10);
                            if (pEventsTable->EventsInfo[i].iEnd + 10 < iDisplayLenth)
                            {
                                fTemp2 = GetMeanFloat(pInputSignal + pEventsTable->EventsInfo[i].iEnd, 10);
                            }
                            else
                            {
                                fTemp2 = pInputSignal[pEventsTable->EventsInfo[i].iEnd];
                            }
                            fDelta = fTemp1 - fTemp2;
                            if (fDelta > 4.0f)
                            {
                                iTailFlag = 1;
                                iTailIndex = i;
                                iTail = pEventsTable->EventsInfo[i].iBegin;
                                iTrueTail = pEventsTable->EventsInfo[i].iBegin;
								
								#ifdef DEBUG_EVENTINFO
									ALG_DEBUG("fTemp = %f,fTemp1 = %f,fTemp2 = %f\n", fTemp, fTemp1, fTemp2);
									#if ISSERIAL == 1
										fprintf(test_file_name, "fTemp = %f,fTemp1 = %f,fTemp2 = %f\n", fTemp, fTemp1, fTemp2);
									#endif
									ALG_DEBUG("\n\n\n");
								#endif
								
                                break;
                            }
                            else
                            {
                                pEventsTable->EventsInfo[i].fLoss = fDelta;
                            }
                        }
                        else
                        {
                            if ((((pEventsTable->EventsInfo[i - 1].fAttenuation < 0.5f)
								&& (i > 1) && (pEventsTable->EventsInfo[i - 1].fAttenuation > -0.5f))
								|| (i == 1)) && (fTemp > pInputSignal[iTail]))
                            {
                                iTailFlag = 1;
                                iTailIndex = i;
                                iTail = pEventsTable->EventsInfo[i].iBegin;
                                iTrueTail = pEventsTable->EventsInfo[i].iBegin;

								#ifdef DEBUG_EVENTINFO
									ALG_DEBUG("i = %d,fTemp = %f\n", i, fTemp);
									#if ISSERIAL == 1
										fprintf(test_file_name, "i = %d,fTemp = %f\n", i, fTemp);
									#endif
									ALG_DEBUG("\n\n\n");
								#endif
								
                                break;
                            }

							if ((pEventsTable->EventsInfo[i].iBegin
								- pEventsTable->EventsInfo[i - 1].iBegin < Max(iBlind, 3 * iPointNum)
								&& (pEventsTable->EventsInfo[i].fAttenuation > 10.0f)))
							{
								iTailFlag = 1;
								iTailIndex = i;
								iTail = pEventsTable->EventsInfo[i].iBegin;
								iTrueTail = pEventsTable->EventsInfo[i].iBegin;

								#ifdef DEBUG_EVENTINFO
									ALG_DEBUG("i = %d,fTemp = %f\n", i, fTemp);
								#if ISSERIAL == 1
									fprintf(test_file_name, "i = %d,fTemp = %f\n", i, fTemp);
								#endif
									ALG_DEBUG("\n\n\n");
								#endif

								break;
							}
                        }
                    }
                }
            }
        }
        //未找到阈值末端，使用后续判断
        if (iTailFlag == 0)// %tail等于0代表采样数据不够（连续光纤）;
        {
        	//若末端靠近第一个事件，认为未连接光纤
            iTailIndex = 0;
            if ((iTail >= 0) && (iTail < pEventsTable->EventsInfo[0].iBegin + iBlind + 2 * iPointNum))
            {
                iTailFlag = 1;
                iTail = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin;
                iTrueTail = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle
					= pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle + 10;//定义末端类型 ;
            }
			
            if (iTailFlag == 0)
            {
                int LongTail = 0;
                if (fSampleRating == 64)
                    LongTail = 800;
                else if (fSampleRating == 32)
                    LongTail = 400;
                else
                    LongTail = 280;
                for (i = 1; i <= pEventsTable->iEventsNumber - 1; ++i)
                {
                    if (pEventsTable->EventsInfo[i].iEnd - pEventsTable->EventsInfo[i].iBegin
						> Max(15 * iBlind, LongTail))
                    {
                        float coef[2] = { 0.0f };
                        if (i != pEventsTable->iEventsNumber - 1)
                        {
                            int start = pEventsTable->EventsInfo[i].iEnd;
                            int len = Min(5, pEventsTable->EventsInfo[i + 1].iBegin
								- pEventsTable->EventsInfo[i].iEnd);
                            float fmean1 = GetMeanFloat(pInputSignal
								+ pEventsTable->EventsInfo[i].iEnd, len);
                            LineFitTwoPoint(pInputSignal[pEventsTable->EventsInfo[i + 1].iBegin],
								pEventsTable->EventsInfo[i + 1].iBegin, fmean1, start, coef);
							
                            if (fabsf(coef[0] * 1000.0f / g_fLengthPerPoint) > 2.0f)
                            {
                                iTailFlag = 1;
                                iTailIndex = i;
                                iTail = pEventsTable->EventsInfo[i].iBegin;
                                iTrueTail = pEventsTable->EventsInfo[i].iBegin;

								#ifdef DEBUG_EVENTINFO
									ALG_DEBUG("start = %d,len = %d, fmean1 = %f, coef[0] = %f\n",
										start, len, fmean1, coef[0]);
									#if ISSERIAL == 1
										fprintf(test_file_name, "start = %d,len = %d, fmean1 = %f, coef[0] = %f\n",
											start, len, fmean1, coef[0]);
									#endif
									ALG_DEBUG("\n\n\n");
								#endif
								
                                break;
                            }
                        }
                        else
                        {
                            LineFitTwoPoint(pInputSignal[Min(pEventsTable->EventsInfo[i].iEnd
								+ 2 * iBlind, iSignalLength - 1)], Min(pEventsTable->EventsInfo[i].iEnd
								+ 2 * iBlind, iSignalLength - 1), pInputSignal[pEventsTable->EventsInfo[i].iEnd],
								pEventsTable->EventsInfo[i].iEnd, coef);
							
                            if (fabsf(coef[0] * 1000.0f / g_fLengthPerPoint) > 2.0f)
                            {
                                iTailFlag = 1;
                                iTailIndex = i;
                                iTail = pEventsTable->EventsInfo[i].iBegin;
                                iTrueTail = pEventsTable->EventsInfo[i].iBegin;

								#ifdef DEBUG_EVENTINFO
									ALG_DEBUG("coef[0] = %f\n", coef[0]);
									#if ISSERIAL == 1
										fprintf(test_file_name, "coef[0] = %f\n", coef[0]);
									#endif
									ALG_DEBUG("\n\n\n");
								#endif
								
                                break;
                            }
                        }
                    }
					/*
                    // 查找末端前面一段距离内是否有反射事件，有的话，将其定义为末端
                    int rightFlag = 0;
                    rightFlag = NormalAttenuationSectionExist(pAlgorithmInfo, i);
                    if ((iTail >= pEventsTable->EventsInfo[i].iBegin)
						&& (fabsf(pEventsTable->EventsInfo[i].fAttenuation) > 1.3f) && (rightFlag)
                        && ((pEventsTable->EventsInfo[i].iStyle == 2)
                        || (pEventsTable->EventsInfo[i].fLoss > 1.5f)))
                    {
                        // 防止由于两个距离比较近的反射事件而导致的末端错误
                        if (i > 0)
                        {
                            fTempDelta = 0;
                            if ((pEventsTable->EventsInfo[i].iBegin - pEventsTable->EventsInfo[i - 1].iEnd > 120)
                                && (fabsf(pEventsTable->EventsInfo[i - 1].fAttenuation) < 0.8f)
                                && (pEventsTable->EventsInfo[i].fLoss > 1.5f))
                            {
                                iTailFlag = 1;
                                iTailIndex = i;
                                iTail = pEventsTable->EventsInfo[i].iBegin;
                                iTrueTail = pEventsTable->EventsInfo[i].iBegin;
								
								#ifdef DEBUG_EVENTINFO
									ALG_DEBUG("----1----i = %d, rightFlag = %d\n", i, rightFlag);
									#if ISSERIAL == 1
										fprintf(test_file_name, "----1----i = %d, rightFlag = %d\n", i, rightFlag);
									#endif
									ALG_DEBUG("\n\n\n");
								#endif
								
                                break;
                            }

                            if (((i != 1) && (pEventsTable->EventsInfo[i - 1].iStyle < 3))
								|| ((i == 1) && (pEventsTable->EventsInfo[i - 1].iStyle = 2)))
                            {
                                if (((i != 1) && (pEventsTable->EventsInfo[i - 1].fAttenuation < 0.8f)
									&& (pEventsTable->EventsInfo[i - 1].fAttenuation > -0.05f)
                                    && ((i != pEventsTable->iEventsNumber - 1)
                                    && (pEventsTable->EventsInfo[i+1].iBegin - pEventsTable->EventsInfo[i].iEnd
                                    > Max(240, iBlind * 2))))
                                    // 拖尾较长特征   
                                    || ((i == 1) && (fabsf(pEventsTable->EventsInfo[i].fAttenuation) > 5.0f))
                                    || ((pEventsTable->EventsInfo[i - 1].fAttenuation < 0.8f)
                                    && (pEventsTable->EventsInfo[i].fAttenuation < 2)
                                    && (pEventsTable->EventsInfo[i].fLoss > 1.5f)))
                                {
                                    iTailFlag = 1;
                                    iTailIndex = i;
                                    iTail = pEventsTable->EventsInfo[i].iBegin;
                                    iTrueTail = pEventsTable->EventsInfo[i].iBegin;
										
									#ifdef DEBUG_EVENTINFO
										ALG_DEBUG("----2----i = %d, rightFlag = %d\n", i, rightFlag);
										#if ISSERIAL == 1
											fprintf(test_file_name, "----2----i = %d, rightFlag = %d\n", i, rightFlag);
										#endif
										ALG_DEBUG("\n\n\n");
									#endif
								
                                    break;
                                }
                            }
                            else
                            {
                                fTempDelta = pInputSignal[Max(pEventsTable->EventsInfo[i - 1].iBegin - 1, 0)]
									- pInputSignal[pEventsTable->EventsInfo[i].iBegin - 1];
                                fTempDelta = Min(fTempDelta, pInputSignal[Max(pEventsTable->EventsInfo[i - 1].iEnd - 1, 0)]
									- pInputSignal[pEventsTable->EventsInfo[i].iBegin - 1] + 0.3);

                            }
                            if ((fTempDelta < 0.5f) && (fTempDelta > 0.0f)
								&& (pEventsTable->EventsInfo[i - 1].iStyle == 3)
                                && (pEventsTable->EventsInfo[i].iStyle == 2)
                                && (pEventsTable->EventsInfo[i].fReflect > -50.0f))
                            {
                                iTailFlag = 1;
                                iTailIndex = i;
                                iTail = pEventsTable->EventsInfo[i].iBegin;
                                iTrueTail = pEventsTable->EventsInfo[i].iBegin;
									
								#ifdef DEBUG_EVENTINFO
									ALG_DEBUG("----3----i = %d, rightFlag = %d, fTempDelta = %f\n", i, rightFlag, fTempDelta);
									#if ISSERIAL == 1
										fprintf(test_file_name, "----3----i = %d, rightFlag = %d, fTempDelta = %f\n",
										i, rightFlag, fTempDelta);
									#endif
									ALG_DEBUG("\n\n\n");
								#endif
								
                                break;
                            }
                            else
                                continue;
                        }
                    }
					*/
                }
            }

            if (iTailFlag == 0)
            {
                for (i = 1; i < pEventsTable->iEventsNumber; ++i)
                {
                    // 查找末端后面一段距离内是否有反射事件，有的话，将其定义为末端
                    if ((iTail < pEventsTable->EventsInfo[i].iBegin)
						&& (pEventsTable->EventsInfo[i].iStyle == 2))
                    {
                        fTemp = GetMaxFloat(pInputSignal + pEventsTable->EventsInfo[i].iBegin
							, pEventsTable->EventsInfo[i].iEnd - pEventsTable->EventsInfo[i].iBegin + 1);
                        if (fTemp > pInputSignal[iTail] - 1.0f)
                        {
                            LineFit(pInputSignal + Min(iTail + 2 * iBlind,
								pEventsTable->EventsInfo[i].iBegin), pEventsTable->EventsInfo[i].iBegin
								- iTail - 2 * iBlind + 1, iTail, pCoef);
                            iStart = Max(pEventsTable->EventsInfo[i].iBegin - iPointNum, 0);
                            fMean = GetMeanFloat(pInputSignal + iStart, iPointNum);
							
                            if ((pInputSignal[iTail] - fMean > 4.0f) 
								|| (fabsf(pCoef[0]) > 0.002f / fSampleRating))
                            {
                                if ((pEventsTable->EventsInfo[i].iBegin - iTail > 400 * iPointNum)
									&& ((pCoef[0] > -0.001f / fSampleRating)
										&& (pCoef[0] < -0.0001f / fSampleRating)))
                                {
                                	#ifdef DEBUG_EVENTINFO
										ALG_DEBUG("pInputSignal[iTail] = %f, fMean = %f, pCoef[0] = %f\n",
											pInputSignal[iTail], fMean, pCoef[0]);
										#if ISSERIAL == 1
											fprintf(test_file_name, "pInputSignal[iTail] = %f, fMean = %f,  pCoef[0] = %f\n",
												pInputSignal[iTail], fMean,  pCoef[0]);
										#endif
										ALG_DEBUG("\n\n\n");
									#endif
									
                                    iTailFlag = 1;
                                    iTailIndex = i;
                                    iTail = pEventsTable->EventsInfo[i].iBegin;
                                    iTrueTail = pEventsTable->EventsInfo[i].iBegin;
                                    break;
                                }
                                else
                                {
                                    if ((pEventsTable->EventsInfo[i].iBegin - iTail <= 400 * iPointNum)
										&& (fabsf(pCoef[0]) < 0.002f / fSampleRating)
                                        && (pEventsTable->EventsInfo[i - 1].fAttenuation < 0.45f)
                                        && (pEventsTable->EventsInfo[i - 1].fAttenuation > 0.08f)
                                        && (pEventsTable->EventsInfo[i].fReflect > -40.0f))
                                    {
                                    	#ifdef DEBUG_EVENTINFO
										ALG_DEBUG("i = %d, pCoef[0] = %f\n", i, pCoef[0]);
										#if ISSERIAL == 1
											fprintf(test_file_name, "i = %d, pCoef[0] = %f\n", i, pCoef[0]);
										#endif
										ALG_DEBUG("\n\n\n");
										#endif
									
                                        iTailFlag = 1;
                                        iTailIndex = i;
                                        iTail = pEventsTable->EventsInfo[i].iBegin;
                                        iTrueTail = pEventsTable->EventsInfo[i].iBegin;
                                        break;
                                    }
                                    else
                                    {
                                        if ((pEventsTable->EventsInfo[i].iBegin - iTail > 400 * iPointNum)
                                            || (((pEventsTable->EventsInfo[i - 1].fAttenuation > 0.5f)
                                            || (pEventsTable->EventsInfo[i - 1].fAttenuation < 0.05f))
                                            && (pEventsTable->EventsInfo[i - 1].iStyle >= 2))
                                            || (fabsf(pCoef[0]) > 0.002f / fSampleRating))
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            continue;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (((pEventsTable->EventsInfo[i - 1].fAttenuation < 0.5f)
									&& (pEventsTable->EventsInfo[i - 1].fAttenuation > 0.08f))
                                    && ((pEventsTable->EventsInfo[i].fAttenuation > 0.8f)
                                    || (pEventsTable->EventsInfo[i].fAttenuation < 0.05f)))
                                {
                                	#ifdef DEBUG_EVENTINFO
										ALG_DEBUG("i = %d\n", i);
										#if ISSERIAL == 1
											fprintf(test_file_name, "i = %d\n", i);
										#endif
										ALG_DEBUG("\n\n\n");
									#endif
										
                                    iTailFlag = 1;
                                    iTailIndex = i;
                                    iTail = pEventsTable->EventsInfo[i].iBegin;
                                    iTrueTail = pEventsTable->EventsInfo[i].iBegin;
                                    break;
                                }
                                else
                                {
                                    continue;
                                }
                            }
                        }
                        else
                            continue;
                    }
                }
            }
			printf("****4**********iTail = %d\n", iTail);
			
            //防止之前已经把末端定位在某个事件前端，但是iTailFlag没有置1，
            //从而出现末端点与某个事件重合
            if (iTailFlag == 0)
            {
                for (i = pEventsTable->iEventsNumber - 1; i > 0; --i)
                {
                    if (pEventsTable->EventsInfo[i].iBegin == iTail)
                    {
						if (pEventsTable->EventsInfo[i].iStyle == 3)
						{
							iTail = pEventsTable->EventsInfo[i - 1].iBegin;
							continue;
						}
                    	#ifdef DEBUG_EVENTINFO
							ALG_DEBUG("---here---i = %d-\n", i);
							#if ISSERIAL == 1
								fprintf(test_file_name, "---here---i = %d-\n", i);
							#endif
							ALG_DEBUG("\n\n\n");
						#endif
						
                        iTailFlag = 1;
                        iTailIndex = i;
                        iTrueTail = pEventsTable->EventsInfo[i].iBegin;
                        break;
                    }
                }
            }
        }

		if ((pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].fReflect > -25.0f
			|| pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].fLoss > Max(6.0f, frontParament.fFiberEndThreshold))
			&& pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle == 2
			&& pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].fDelta > 3.0f)
		{
			if ((iTailFlag == 0) || (pEventsTable->EventsInfo[iTailIndex].fLoss < frontParament.fFiberEndThreshold))
			{
				iTrueTail = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin;
				iTailIndex = pEventsTable->iEventsNumber - 1;
				iTailFlag = 1;
				for (i = 0; i < pEventsTable->iEventsNumber - 1; i++)
				{
					if (pEventsTable->EventsInfo[i].iStyle >= 10)
					{
						pEventsTable->EventsInfo[i].iStyle -= 10;
					}
				}
			}
		}

		// 删除E点判断在拖尾上的衰减事件
		if ((iTailFlag == 1) && (pEventsTable->EventsInfo[iTailIndex].iStyle == 0)
			&& (pEventsTable->EventsInfo[iTailIndex].fLoss < 1.0f) && (iTailIndex > 1))
		{
			iTailIndex = iTailIndex - 1;
			iTrueTail = pEventsTable->EventsInfo[iTailIndex].iBegin;
		}

        if (iTailFlag == 1)
            pEventsTable->EventsInfo[iTailIndex].iStyle
            	= pEventsTable->EventsInfo[iTailIndex].iStyle + 10; //!< 定义末端类型;
        else
        {
            // 记录在iTail之前的最后一个事件
            for (i = 0; i < pEventsTable->iEventsNumber; i++)
            {
                if ((pEventsTable->EventsInfo[i].iBegin >= iTail) && (i > 0))
                {
                    pEventsTable->iEventsNumber = i + 1;
                }
            }

            if (((iTail - pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin)
				* g_fLengthPerPoint > 5000.0f) && (continueFlag == 0))
            {
                iTrueTail = iTail;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iBegin = iTail;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fEventsPosition = iTail * g_fLengthPerPoint;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iEnd = Min(iTail + iBlind, iDisplayLenth);
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iStyle = 21;          //!< 光功率不足;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fLoss = 0.0f;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fDelta = 0.0f;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fReflect = 0.0f;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iContinueLength = 0;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fAttenuation = 0.0f;
                pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fContinueAttenuation = 0.0f;
                pEventsTable->iEventsNumber++;
            }
            else if (continueFlag == 0)
            {
            	if (pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle == 3)
            	{
            		for (i = pEventsTable->iEventsNumber - 2; i > 0; i--)
		            {
		                if (pEventsTable->EventsInfo[i].iStyle == 2)
		                {
		                    pEventsTable->EventsInfo[i].iStyle += 10;//pEventsTable->iEventsNumber - 1->i
			                iTrueTail = pEventsTable->EventsInfo[i].iBegin;
			                iTailFlag = 1;
							break;
		                }
		            }
            	}
				else
				{
					pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle += 10;
	                iTrueTail = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iBegin;
	                iTailFlag = 1;
				}
            }
        }
    }
    EventsTableSort2(pEventsTable);

    if ((continueFlag == 1) && (iTailFlag == 0))
    {
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iBegin = iDisplayLenth - 2;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iEnd = iDisplayLenth - 1;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iStyle = 20;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fLoss = 0.0f;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fDelta = 0.0f;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fReflect = 0.0f;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].iContinueLength = 0;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fAttenuation = 0.0f;
        pEventsTable->EventsInfo[pEventsTable->iEventsNumber].fContinueAttenuation = 0.0f;
        pEventsTable->iEventsNumber++;
        iTrueTail = pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iEnd;
    }

	//  对于脉宽大于10ns的情况充分利用末端阈值
	if (pAlgorithmInfo->PulseWidth > ENUM_PULSE_10NS && iTailFlag != 1)
	{
		//printf("iTrueTail = %d, fEndTail = %d\n", iTrueTail, fEndTail);
		iTrueTail = Min(iTrueTail, fEndTail);
		for (i = pEventsTable->iEventsNumber - 1; i > 0; i--)
	    {
	    	//printf("pEventsTable->EventsInfo[%d].iStyle = %d\n", i, pEventsTable->EventsInfo[i].iStyle);
	        if ((pEventsTable->EventsInfo[i].iStyle >= 10)
				&& (pEventsTable->EventsInfo[i].iBegin > iTrueTail))
	        {
	            pEventsTable->EventsInfo[i].iStyle -= 10;
	        }
			else if (pEventsTable->EventsInfo[i].iBegin <= iTrueTail)
			{
				if (pEventsTable->EventsInfo[i].iStyle < 10)
				{
					pEventsTable->EventsInfo[i].iStyle += 10;
					break;
				}
				else
				{
					break;
				}
			}
	    }
	}

    //删除末端后面的事件;
    j = 0;
    for (i = 0; i < pEventsTable->iEventsNumber; i++)
    {
        if ((i == 0) || (pEventsTable->EventsInfo[i].iBegin <= iTrueTail))
        {
            pEventsTable->EventsInfo[j] = pEventsTable->EventsInfo[i];
            j++;
        }
    }
    pEventsTable->iEventsNumber = j;

	if ((pAlgorithmInfo->PulseWidth <= 2)
		&& ((pEventsTable->EventsInfo[pEventsTable->iEventsNumber-1].iStyle < 10)
		|| (pEventsTable->EventsInfo[pEventsTable->iEventsNumber-1].iStyle == 13)))
	{
		//防止找不到末端或者找到鬼影上
		for (i = pEventsTable->iEventsNumber-1; i > 0; i--)
		{
			if (pEventsTable->EventsInfo[i].iStyle <= 2)
			{
				pEventsTable->EventsInfo[i].iStyle += 10;
				pEventsTable->iEventsNumber = i + 1;
				break;
			}
		}
	}
	ALG_DEBUG("iEventsNumber = %d, PulseWidth = %d\n", 
		pEventsTable->iEventsNumber, pAlgorithmInfo->PulseWidth);
}

// 将事件g_pEventsRegionTable跟着pEventsTable改变;
void TransEventsTable(
	EVENTS_TABLE *pEventsTable,
	EVENTS_REGION_TABLE *pEventsRegionTable)
{
	int i = 0;

	for (i = 0; i < pEventsTable->iEventsNumber; ++i)
	{
		pEventsRegionTable->EventsRegionInfo[i].iBegin = pEventsTable->EventsInfo[i].iBegin;
		pEventsRegionTable->EventsRegionInfo[i].iEnd = pEventsTable->EventsInfo[i].iEnd;
		pEventsRegionTable->EventsRegionInfo[i].iStyle = pEventsTable->EventsInfo[i].iStyle;
	}
	pEventsRegionTable->iEventsNumber = pEventsTable->iEventsNumber;
}

// 判断pEventsRegionTable事件最大最小值索引位置
void GetEventsMaxMinIndex(
	float *pInputSignal,//输入的信号;
	EVENTS_REGION_TABLE *pEventsRegionTable)
{
	EVENTS_REGION_INFO *pEventsRegionInfo = pEventsRegionTable->EventsRegionInfo;
	int i = 0;
	for (; i < pEventsRegionTable->iEventsNumber; ++i)
	{
		pEventsRegionInfo[i].iMaxIndex = pEventsRegionInfo[i].iBegin
			+ GetMaxIndex(pInputSignal + pEventsRegionInfo[i].iBegin,
				pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i].iBegin + 1);
		pEventsRegionInfo[i].iMinIndex = pEventsRegionInfo[i].iBegin 
			+ GetMinIndex(pInputSignal+pEventsRegionInfo[i].iBegin,
				pEventsRegionInfo[i].iEnd - pEventsRegionInfo[i].iBegin + 1);
	}
}

// 光纤长度估算
int GetFiberLength(unsigned short *pSignal,
	int iLen,
	unsigned int *iFiberLength,
	float END,
	int iflag)
{
    int i = 0, j = 0, iRet = 1, iState = 0, iCount = 0, iTail = -1;
    unsigned int iBlackLevel = 0, iMeanLen = 0;
    float *fSignal = NULL;
    float fInputNoise = 0.0f;
    float fEndThr = 1.5f;
    int iTemp = 0;
    iMeanLen = (int)floor(iLen / 100);
    fSignal = (float *)calloc(iLen, sizeof(float));
    float fMeanSignal1 = 0.0f;
    float fMeanSignal2 = 0.0f;
    iBlackLevel = (unsigned int)floor(GetMeanUint16(pSignal + iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 0.5f);
    //将输入原始信号转换为对数信号;
    for (i = 0; i < iLen; ++i)
    {
        iTemp = iBlackLevel - pSignal[i];
        if ((iTemp > 0) && (iTemp < 65536))
        {
            fSignal[i] = fLogSig[iTemp];
        }
        else if ((iTemp > -65536) && (iTemp < 0))
        {
            fSignal[i] = -fLogSig[-iTemp];
        }
        else
        {
            fSignal[i] = fEps;
        }
    }

 	fInputNoise = GetMaxAbsFloat(fSignal + iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
	fEndThr = fInputNoise + 2.0f;
	fEndThr = Min(fEndThr, 1.5f * fInputNoise);
	
    printf("------------fEndThr = %f, fInputNoise = %f, iBlackLevel = %d------------\n", fEndThr, fInputNoise, iBlackLevel);

    for (i = 20; i < iLen; i++)
    {
        switch (iState)
        {
        case 0:
        {
                  if (fSignal[i] < fEndThr)
                  {
                      iCount = 1;
                      iState = 1;
                  }
                  else
                  {
                      iCount = 0;
                      iState = 0;
                  }
                  break;
        }
        case 1:
        {
                  if (fSignal[i] < fEndThr)
                  {
                      iCount++;
                      //if ((iCount >= 6) && (i > 50))//连续5个点小于EndThr视为光纤末端;
					  if (iCount >= 6)//连续5个点小于EndThr视为光纤末端;
                      {
                          iTail = i - iCount + 1;
                      }
                      iState = 1;
                  }
                  else
                  {
                      iCount = 0;
                      iState = 0;
                  }
                  break;
        }
        default:
        {
                   iCount = 0;
                   iState = 0;
        }
        }
        if (iTail != -1)
        {
            *iFiberLength = iTail;
            break;//iTail！=-1表示已经查找到光纤末端，跳出循环;
        }
    }
    if (iTail == -1)
    {
        *iFiberLength = iLen;
    }
    //int iFlagExit = 0;
    int ii = 0;
    if (*iFiberLength > 2000)
    {
		int tmpS = (int)(*iFiberLength / 100);
		int tmpE = (int)(*iFiberLength / 100 / 4 * 3);
        for (ii = tmpS; ii > tmpE; --ii)//从100点后开始检测
        {
            fMeanSignal1 = GetMeanFloat(fSignal + 100 * ii, 100);
            for (j = 0; j < ii; ++j)
            {
                fMeanSignal2 = GetMeanFloat(fSignal + 100 * (ii - j), 100);
                if (fMeanSignal1 - fMeanSignal2 > END)
                {
                    *iFiberLength = (unsigned int)100 * (ii - j + 1);
                    break;
                }
            }
        }
    }
    else//短光纤判断是否存在回波
    {
		int tmpS = (int)(*iFiberLength / 10);
		int tmpE = (int)(*iFiberLength / 10 / 2);
        for (ii = tmpS; ii > tmpE; --ii)//从100点后开始检测
        {
            fMeanSignal1 = GetMeanFloat(fSignal + 10 * ii, 10);
            for (j = 0; j < ii; ++j)
            {
                fMeanSignal2 = GetMeanFloat(fSignal + 10 * (ii - j), 10);
                if (fMeanSignal1 - fMeanSignal2 > END)
                {
                    *iFiberLength = (unsigned int)10 * (ii - j + 1);
                    break;
                }
            }
        }
    }
    printf("-------iFiberLength_END = %d---------------\n", *iFiberLength);
    if (*iFiberLength < 380)
        iRet = 0;
    else if (*iFiberLength < 3500)
        iRet = -1;
    else if ((*iFiberLength >= 3500) && (*iFiberLength <= 15000))
        iRet = 2;
    else if ((*iFiberLength > 15000) && (*iFiberLength <= 25000))
        iRet = 0;
    else
        iRet = 1;
    free(fSignal);
	fSignal = NULL;
    return iRet;

}

// 寻找光纤平坦区域
int SearchFlatRegion(float *sig,
	int len,
	int smpInterval,
	int threshEnd)
{
	int i = 0, j = 0;
	int time=10;
	int count=0;
	int startPos = 0;
	int breakFlag = 0;
	while(!breakFlag)
	{
		startPos = startPos + j * smpInterval; 
		float MeanCoef = 0.0f;
		if(startPos + j * smpInterval < len - smpInterval)
		{	
			MeanCoef = GetMeanFloat(sig + startPos,
				Min(smpInterval, Max(len - startPos - smpInterval - 1, 1))); 
		}           
		if(startPos > len - smpInterval)
		{
			return 99999;
			break;
		}
		for(i = 1 ; i < time; ++i)
		{
			float temp = GetMeanFloat(sig + startPos + i * smpInterval, 
				         Min(smpInterval, Max(len - startPos - i * smpInterval, 1)));
			if(startPos + i * smpInterval > len - smpInterval)
			{
				return 99999;
			}
			if(fabsf(temp - MeanCoef) < 0.1f)
			{
				count++;
			}
			else
			{
				count = 0;
				j++;
				break;
			}
			if(count == 5)
			{
				if(MeanCoef > threshEnd)
				{
					breakFlag = 1;
					break;
				}
				else
				{
					return 99999;
				}
			}
		}
	}

	return (j * smpInterval);
}

// 光纤端面检查;
unsigned int GetReflectCoff(unsigned short *pSignal,
	int iLen)
{
    int i = 0;
    unsigned int iBlackLevel = 0, iRet = 0;
    float *fSignal = NULL;
    float fReflectivity = 0.0f, pDelta = 0.0f;
    float fMean1 = 0.0f, fMean2 = 0.0f, fInputNoise = 0.0f;
    int iTemp = 0;
    float fMean = 0.0f, fSignalMean = 0.0f;
    #define MULTIPLE    1.8f

    fSignal = (float *)calloc(iLen, sizeof(float));
    //修改噪声计算
    iBlackLevel = (unsigned int)floor(GetMeanUint16(pSignal 
		+ iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 0.5f);
    //将输入原始信号转换为对数信号;
    for (i = 0; i < iLen; ++i)
    {
        iTemp = iBlackLevel - pSignal[i];
        if ((iTemp > 1) && (iTemp < 65536))
        {
            fSignal[i] = fLogSig[iTemp];
        }
        else if ((iTemp > -65536) && (iTemp < -1))
        {
            fSignal[i] = -fLogSig[-iTemp];
        }
        else
        {
            fSignal[i] = fEps;
        }
    }
    int iFiberLen = 0;
    int count = 0;
    
    fInputNoise = Max(MULTIPLE * GetStdFloat(fSignal 
		+ iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN), 2.5f);
	float fMaxNoise = GetMaxFloat(fSignal + iLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);

	fInputNoise = Max(fInputNoise, fMaxNoise + 3.0f);
    fMean = GetMeanFloat(fSignal + 6, 3);
	fMean1 = GetMeanFloat(fSignal + 33 , 5);
	fMean2 = GetMeanFloat(fSignal + 45 , 5);

	printf("fMaxNoise = %f, fInputNoise = %f, fMean = %f, fMean1 = %f, fMean2 = %f\n\n", 
		fMaxNoise, fInputNoise, fMean, fMean1, fMean2);
	
	if ((fMean1 < fInputNoise) && (fMean2 < fInputNoise))
	{
		free(fSignal);
		fSignal = NULL;
		iRet = 1;
		
		return iRet;
	}
    
    for (i = 30; i < iLen; ++i)
    {
        if (fSignal[i] < fInputNoise)
        {
            count++;
        }
        else
        {
            count = 0;
        }
        if (count >= 5)
        {
            iFiberLen = i - count;
            break;
        }
    }
	
    if (iFiberLen > 100)
    {
        fSignalMean = fInputNoise / MULTIPLE;
		pDelta = fMean - fSignalMean;
		printf("\n\n---------step 2.0-----------\n");
    }   
    else if (fMean1 - fMean2 > 1.0f)
    {
    	float fMax = 0.0f;
		fMax = GetMaxFloat(fSignal + 36, Min(10000, iLen - 36));
		
		if (fMax > fMean1 + 1.0f)
		{
			pDelta = fMean - fInputNoise / MULTIPLE;
			printf("\n\n---------step 2.1-----------\n");
		}
		else
		{
		    fMean2 = GetMeanFloat(fSignal + 42 , 3);
            if (fMean1 - fMean2 > 0.6f)
            {
                pDelta = 0.1f;
			    printf("\n\n---------step 2.2-----------\n");
            }
            else
            {
                pDelta = fMean2 - fInputNoise / MULTIPLE;
                printf("\n\n---------step 2.3-----------\n");
            }
		}
    }
	else
	{
		pDelta = fMean1 - fInputNoise / MULTIPLE;
		printf("\n\n---------step 3-----------\n");
	}
    pDelta = Max(pDelta, 1.0f);
    fReflectivity = -50.0f + 10.0f * log10f(powf(10, pDelta / 5.0f) - 1.0f);
	
    if(fReflectivity < -50.0f && fMean2 < fInputNoise + 4.5f)
        iRet = 1;
    /**/
    printf("---------------------checkEndface---------------\n");
    printf("fMean1 = %f\n",fMean1);
    printf("fMean2 = %f\n",fMean2);
	printf("pDelta = %f\n", pDelta);
    printf("iFiberLen = %d\n", iFiberLen);
    printf("fReflectivity = %f\n", fReflectivity);
    
    free(fSignal);

    return iRet;
}

// 获取信噪比较好的小波系数
float GetGoodSnrWaveCoef2(
    int level,
    float Thresh,
    float Distance,
    const float referValue,
    float maxThresh,
    SNR_KEY_POINT* snrKeyPoint)
{
    float fTmpThr = 0.0f;
    switch (level)
    {
        case 0:
        {
            if (Distance < snrKeyPoint->keyPointDistance[0] * 0.3f)
                fTmpThr = Thresh;
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.5f)
                fTmpThr = 0.55f;
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.6f)
                fTmpThr = 1.30f;
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.8f)
                fTmpThr = 2.0f;
            else if (Distance < snrKeyPoint->keyPointDistance[0])
                fTmpThr = Thresh + 5.0f;
            else if (Distance < snrKeyPoint->keyPointDistance[1])
                fTmpThr = Thresh + 7.0f;
            else if (Distance < snrKeyPoint->keyPointDistance[2])
                fTmpThr = Thresh + 9.0f;
            else
                fTmpThr = Thresh + 50.0f;
            break;
        }
        case 1:
        {
            if (Distance < snrKeyPoint->keyPointDistance[0] * 0.3f)
                fTmpThr = ((Thresh < ThreshOffset[1] + 0.15f) ? (ThreshOffset[1] + 0.15f) : (Thresh));
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.4f)
                fTmpThr = ((0.20f < ThreshOffset[1] + 0.1f) ? (ThreshOffset[1] + 0.1f) : (0.15f));
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.5f)
                fTmpThr = ((0.25f < ThreshOffset[1] + 0.1f) ? (ThreshOffset[1] + 0.1f) : (0.25f));
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.75f)
                fTmpThr = ((0.50f < ThreshOffset[1] + 0.2f) ? (ThreshOffset[1] + 0.2f) : (0.50f));
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.98f)
                fTmpThr = ((0.50f < ThreshOffset[1] + 0.3f) ? (ThreshOffset[1] + 0.3f) : (0.50f));
            else if (Distance < snrKeyPoint->keyPointDistance[0])
                fTmpThr = 1.20f;
            else if (Distance < snrKeyPoint->keyPointDistance[1])
                fTmpThr = 2.00f;

            else if (Distance < snrKeyPoint->keyPointDistance[2])
                fTmpThr = Thresh + 10.0f;
            else
                fTmpThr = Thresh + 50.0f;
            if (frontParament.wave == 0)
            {
                if (Distance > 78000.0f)
                    fTmpThr = Max(fTmpThr, maxThresh);
            }
            break;
        }
        case 2:
        {
            if (Distance < snrKeyPoint->keyPointDistance[0] * 0.4f)
                fTmpThr = ((Thresh < ThreshOffset[2] + 0.2f) ? (ThreshOffset[2] + 0.2f) : (Thresh)); 
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.6f)
                fTmpThr = ((0.25f < ThreshOffset[2] + 0.25f) ? (ThreshOffset[2] + 0.4f) : (0.21f)); 
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.7f)
                fTmpThr = ((0.35f < ThreshOffset[2] + 0.25f) ? (ThreshOffset[2] + 0.5f) : (0.35f));	
            else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.98f)
                fTmpThr = ((0.45f < ThreshOffset[2] + 0.25f) ? (ThreshOffset[2] + 0.6f) : (0.45f)); 
            else if (Distance < snrKeyPoint->keyPointDistance[0])
                fTmpThr = ((0.9f < ThreshOffset[2] + 0.25f) ? (ThreshOffset[2] + 0.6f) : (0.9f));  
            else if (Distance < snrKeyPoint->keyPointDistance[1])
                fTmpThr = ((1.3f < ThreshOffset[2] + 0.25f) ? (ThreshOffset[2] + 0.6f) : (1.3f)); 
            else if (Distance < 0.98f * snrKeyPoint->keyPointDistance[2])
                fTmpThr = ((1.9f < ThreshOffset[2] + 0.25f) ? (ThreshOffset[2] + 0.6f) : (1.9f));
            else if (Distance < snrKeyPoint->keyPointDistance[2])
                fTmpThr = Thresh + 10.0f;
            else
                fTmpThr = Thresh + 70.0f;
            if (frontParament.wave == 0)
            {
                if (Distance > 78000.0f)
                    fTmpThr = Max(fTmpThr, maxThresh);
            }
            break;
        }
        case 3:
        {
              if (Distance < snrKeyPoint->keyPointDistance[0] * 0.4f)
                  fTmpThr = ((Thresh < ThreshOffset[3] + 0.25f) ? (ThreshOffset[3] + 0.25f) : (Thresh)); 
              else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.5f)
                  fTmpThr = ((0.45f < ThreshOffset[3] + 0.4f) ? (ThreshOffset[3] + 0.4f) : (0.45f));	
              else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.75f)
                  fTmpThr = ((0.50f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.5f) : (0.50f));	
              else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.85f)
                  fTmpThr = ((0.55f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.5f) : (0.55f));		 
              else if (Distance < snrKeyPoint->keyPointDistance[0] * 0.98f)
                  fTmpThr = ((0.65f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.6f) : (0.65f));
              else if (Distance < snrKeyPoint->keyPointDistance[0])
              {
                  if (frontParament.pulseWidth <= 5)
                      fTmpThr = ((0.80f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.6f) : (0.80f));
                  else
                      fTmpThr = ((1.50f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.6f) : (1.50f));
              }
              else if (Distance < snrKeyPoint->keyPointDistance[1])
              {
                  if (frontParament.pulseWidth <= 5)
                      fTmpThr = ((1.00f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.6f) : (1.00f));
                  else
                      fTmpThr = ((2.00f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.6f) : (2.00f));
              }
              else if (Distance < 0.98f * snrKeyPoint->keyPointDistance[2])
                  fTmpThr = ((2.30f < ThreshOffset[3] + 0.3f) ? (ThreshOffset[3] + 0.6f) : (2.30f));
              else if (Distance < snrKeyPoint->keyPointDistance[2])
              {
                  fTmpThr = Thresh + 20.0f;
              }
              else
                  fTmpThr = Thresh + 200.0f;

              break;
        }
        default:
        {
               level = 3;
        }
    }

    return Min(Max(fTmpThr, referValue), maxThresh);
}

//找到初始点位置
int GetStartPoint(ALGORITHM_INFO *pAlgorithmInfo)
{
    int iReturn = 0;
    float *fData = NULL;
    int   iLen = 0;
    int   iBlind = 1;
    EVENTS_TABLE *pEvents_tabal = NULL;

    int i = 0, flag = 0;
    int iMaxIndex = 0;
    float fMaxValue = 0.0f;
    int iState = 0;
    int iFallPos = 0;
    int iCnt = 0;


    fData = pAlgorithmInfo->pSignalLog;
    iLen = pAlgorithmInfo->iDisplayLenth;
    iBlind = pAlgorithmInfo->iBlind;
    pEvents_tabal = &pAlgorithmInfo->EventsTable;

    //首先查找突变点，找到突变点后再查找突变结束点，将突变结束位置作为初始点
    for(i = iBlind; i < 2 * iBlind + 50; ++i)
    {
        switch(iState)
        {
            case 0:
            {
                if(fData[i - 1] - fData[i] > 0.5f)
                {
                    iFallPos = i + 1;
                    iState = 1;
                }
                break;
            }
            case 1:
            {
                if(fData[i - 1] - fData[i] > 0.5f)
                {
                    iFallPos = i + 1;
                    iCnt = 0;
                }
                if((fData[i - 1] - fData[i] < 0.5f) || (fData[i] < 5.0f))
                {
                    iCnt++;
                    if(iCnt >= 5)
                    {
                        iReturn = i - 4;
						flag = 1;
                    }
                }
                break;
            }
            default:
                break;
        }
        if(flag)
            break;
        
    }
    //若不存在突变点，则查找盲区后方变化较大点
    if(!flag)
    {
        fMaxValue = GetMaxFloat(fData, iBlind + 4);
        iMaxIndex = GetMaxIndex(fData, iBlind + 4);
        for(i = iBlind; i < 2 * iBlind + 4; ++i)
        {
            if(fMaxValue - fData[i] > 1.0)
            {
				flag = 1;
                iReturn = i;
                break;
            }
        }
        if(!flag)
        {
            iReturn = iBlind;
        }
    }
    //最后需要保证该点不大于第二个事件点
    iReturn = Max(iFallPos,iReturn);

    if(pEvents_tabal->iEventsNumber > 1)
    {	
    	//由于光线短，且脉宽大时，光纤前端出现的波形乱七八糟，所以查找的S点位置就以第一个点为准
        if(pEvents_tabal->EventsInfo[1].iBegin - iReturn <= iBlind)
        {
            //iReturn = Min(iBlind,pEvents_tabal->EventsInfo[1].iBegin-1);//防止s点位置超过第二个事件点的起始点
            iReturn = 0;
        }
    }
    return Max(0,iReturn);
}

// 事件通过判断（损耗与反射率）
int IsEventPassed(const EVENTS_INFO* event)
{
    int passed = 0;

    switch (event->iStyle)
    {
    case 0:
    case 10:
        if (event->fLoss > 0.1f)
        {
            passed = 1;
        }
        break;
    case 1:
    case 11:
        if (fabs(event->fLoss) > 0.1f)
        {
            passed = 1;
        }
        break;
    case 2:
    case 12:
        if (event->fLoss > 0.1f || event->fReflect > -40.0f)
        {
            passed = 1;
        }
        break;
    case 20:
    case 21:
    case 3:
        passed = 1;
        break;
    default:
        break;
    }

    return passed;
}

// 不同的脉宽事件进行合并
void MergeDifferentPulseEventLis(ALGORITHM_INFO *CurAlgorithmStruct,
	ALGORITHM_INFO *RetSola)
{
    // 当前波长SOLA列表为空，直接拷贝
    if (RetSola->EventsTable.iEventsNumber == 0)
    {
        memcpy(RetSola, CurAlgorithmStruct, sizeof(ALGORITHM_INFO));
        return;
    }
    else
    {
        //光回损赋值
        float curWaveReturnLoss = CurAlgorithmStruct->fTotalReturnLoss;
        float solaWaveReturnLoss = RetSola->fTotalReturnLoss;
        RetSola->fTotalReturnLoss = (curWaveReturnLoss > solaWaveReturnLoss) ? curWaveReturnLoss : solaWaveReturnLoss;

        //重置回波损耗、有效距离
        RetSola->solaEffectiveRange = Max(CurAlgorithmStruct->solaEffectiveRange, RetSola->solaEffectiveRange);

        int i = 0, j = 0;
        if (CurAlgorithmStruct->EventsTable.iEventsNumber == 1)
        {
            return;
        }

        EVENTS_TABLE *RetEvent = &RetSola->EventsTable;
        EVENTS_TABLE *CurEvent = &CurAlgorithmStruct->EventsTable;

        // sola没有连续光纤事件
        if (RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle == 20)
        {
            RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle = 21;
        }

        // sola没有连续光纤事件
        if (CurEvent->EventsInfo[CurEvent->iEventsNumber - 1].iStyle == 20)
        {
            CurEvent->iEventsNumber--;
        }
        else
        {
            if (CurEvent->EventsInfo[CurEvent->iEventsNumber - 1].iStyle != 21)
            {
                CurEvent->EventsInfo[CurEvent->iEventsNumber - 1].iStyle -= 10;
            }
        }

        // 修改末端事件类型 
        if (RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle != 21)
        {
            if (RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle < 10)
            {
                ;
            }
            else
            {
                RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle -= 10;
            }
        }

        // 删除SOLA事件返回列表中功率不足事件和连续光纤事件
        if ((RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle == 21)
            && (CurEvent->EventsInfo[CurEvent->iEventsNumber - 1].fEventsPosition
            > RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].fEventsPosition))
        {
            RetEvent->iEventsNumber--;
        }

        // 删除SOLA事件返回列表中功率不足事件和连续光纤事件
        if ((CurEvent->EventsInfo[CurEvent->iEventsNumber - 1].iStyle == 21)
            && (CurEvent->EventsInfo[CurEvent->iEventsNumber - 1].fEventsPosition
            < RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].fEventsPosition))
        {
            CurEvent->iEventsNumber--;
        }

        // 只保留一个起始事件
        for (i = 1; i < CurEvent->iEventsNumber; ++i)
        {
            int mergeFlag = 0;    //!< 0:添加 1: 合并 2:不做操作
            if (CurEvent->EventsInfo[i].iStyle == 21)
            {
                mergeFlag = 0;
            }
            else
            {
                float CurEventBegin_M = CurEvent->EventsInfo[i].fEventsPosition;
                float CurEventEnd_M = CurEvent->EventsInfo[i].fEventsEndPosition;

                for (j = 1; j < RetEvent->iEventsNumber; ++j)
                {
                    float RetEventBegin_M = RetEvent->EventsInfo[j].fEventsPosition;
                    float RetEventEnd_M = RetEvent->EventsInfo[j].fEventsEndPosition;

                    // 合并条件
                    if ((fabsf(Max(CurEventEnd_M, RetEventEnd_M) - Min(CurEventBegin_M, RetEventBegin_M))
                        < CurEventEnd_M - CurEventBegin_M + RetEventEnd_M - RetEventBegin_M)
                        || (fabsf(CurEventBegin_M - RetEventBegin_M)
                        < blindWidth[RetEvent->EventsInfo[j].iPulse][RetEvent->EventsInfo[j].iStyle]))

                    {
                        mergeFlag = 1;
                        if (CurEvent->EventsInfo[i].iStyle < RetEvent->EventsInfo[j].iStyle)
                        {
                            mergeFlag = 2;
                        }

                        // 多事件交叉 2对1
                        if (i < CurEvent->iEventsNumber - 1)
                        {
                            float nextCurEventBegin = CurEvent->EventsInfo[i + 1].fEventsPosition;
                            float nextCurEventEnd = CurEvent->EventsInfo[i + 1].fEventsEndPosition;
                            if (fabsf(Max(RetEventEnd_M, nextCurEventEnd) - Min(RetEventBegin_M, nextCurEventBegin))
                                < RetEventEnd_M - RetEventBegin_M + nextCurEventEnd - nextCurEventBegin)
                            {
                                mergeFlag = 2;
                                printf("mergeFlag0:i = %d,j = %d\n", i, j);
                            }

                        }
                        printf("mergeFlag1:i = %d,j = %d\n", i, j);

                        // 多事件交叉 1对2
                        if (j < RetEvent->iEventsNumber - 1)
                        {
                            float nextRetEventBegin = RetEvent->EventsInfo[j + 1].fEventsPosition;
                            float nextRetEventEnd = RetEvent->EventsInfo[j + 1].fEventsEndPosition;
                            if (fabsf(Max(CurEventEnd_M, nextRetEventEnd) - Min(CurEventBegin_M, nextRetEventBegin))
                                < CurEventEnd_M - CurEventBegin_M + nextRetEventEnd - nextRetEventBegin)
                            {
                                mergeFlag = 4;
                                printf("mergeFlag2:i = %d,j = %d\n", i, j);
                                if ((RetEventBegin_M > RetSola->solaEffectiveRange)
                                    && (CurEventBegin_M < CurAlgorithmStruct->solaEffectiveRange))
                                {
                                    mergeFlag = 3;
                                    printf("mergeFlag3:i = %d,j = %d\n", i, j);
                                }

                            }
                        }

                        if (CurEventBegin_M > CurAlgorithmStruct->solaEffectiveRange)
                        {
                            printf("mergeFlag22:i = %d,j = %d\n", i, j);
                            mergeFlag = 2;
                        }
                        break;
                    }
                }
            }
            // 事件合并添加
            InsertEvent(CurAlgorithmStruct, RetSola, i, j, mergeFlag);
        }

        // 修改返回事件列表的末端事件类型
        if (RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle != 21)
        {
            if (RetEvent->iEventsNumber > 1)
            {
                RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].iStyle += 10;
            }
        }

        //计算链路总损耗
        CalTotalFloss(&RetSola->EventsTable);
    }
}

// 判断事件是否存在 
int IsEventExist(EVENTS_TABLE *EventsTable, float position)
{
	int i = 0, index = -1;
	if (EventsTable->iEventsNumber <= 0)
	{
		return 1000;
	}

	for (i = 0; i < EventsTable->iEventsNumber; ++i)
	{
		if (fabsf(EventsTable->EventsInfo[i].fEventsPosition - position) < 0.01f)
		{
			index = i;
			break;
		}
	}
	return index;
}

// 适配OTDR宏弯曲事件
void OtdrAdapterMacrobending(ALGORITHM_INFO  *pAlgorithm_info1,
	ALGORITHM_INFO  *pAlgorithm_info2)
{
	SOLA_INFO *pSolaAlgInfo = (SOLA_INFO*)malloc(sizeof(SOLA_INFO));
	memset(pSolaAlgInfo, 0, sizeof(SOLA_INFO));
	
	int waveLen1 = pAlgorithm_info1->SetParameters.iwavelength;
	memcpy(&pSolaAlgInfo->pAlgorithmInfo[waveLen1], pAlgorithm_info1, sizeof(ALGORITHM_INFO));

	int waveLen2 = pAlgorithm_info2->SetParameters.iwavelength;
	memcpy(&pSolaAlgInfo->pAlgorithmInfo[waveLen2], pAlgorithm_info2, sizeof(ALGORITHM_INFO));

	// 生成总的事件列表(多波长事件合并)
	GenerateSolaTable(pSolaAlgInfo);

	// 判断是否是宏弯曲事件
	JudgeMacrobendingLossEvent(pSolaAlgInfo);

	// 事件总的结构体 
	EVENTS_TABLE *EventsTable = &pSolaAlgInfo->EventsTable;

	int i = 0, j = 0;
	for(i = 0; i < pSolaAlgInfo->EventsTable.iEventsNumber; ++i)
	{
		for(j = 0; j < SOLA_WAVE_NUM; ++j)
		{	
			if((EventsTable->EventsInfo[i].iStyle == 5)
				|| (EventsTable->EventsInfo[i].iStyle == 15))
			{
				int index = IsEventExist(&pSolaAlgInfo->pAlgorithmInfo[j].EventsTable,
					EventsTable->EventsInfo[i].fEventsPosition);
				if((index > 0) && (index != 1000))   //  不包含起始事件
				{
					if(j == waveLen1)
					{
						if(pAlgorithm_info1->EventsTable.EventsInfo[index].iStyle >= 10)
							pAlgorithm_info1->EventsTable.EventsInfo[index].iStyle += 5;
						else
							pAlgorithm_info1->EventsTable.EventsInfo[index].iStyle = 5;
	
					}
					else
					{
						if(pAlgorithm_info2->EventsTable.EventsInfo[index].iStyle >= 10)
							pAlgorithm_info2->EventsTable.EventsInfo[index].iStyle += 5;
						else
							pAlgorithm_info2->EventsTable.EventsInfo[index].iStyle = 5;
	
					}
				}
			}
		}
	}

	free(pSolaAlgInfo);
}

/* 
* 判断宏弯曲事件 
* 宏弯曲判断依据 1)多波长测试，其中只有一个波长测到该事件，且该事件为增益事件或者衰减事件
 						   2)多波长测试，两个波长测试出的事件类型相同，但损耗相差2db以上
 						   3)多波长测试，两个波长测试出的事件类型不同
*/
void JudgeMacrobendingLossEvent(SOLA_INFO *pSolaAlgInfo)
{
	int i = 0, j = 0, k = 0;

	printf("JudgeMacrobendingLossEvent start !!!\n");

	EVENTS_TABLE *EventsTable = &pSolaAlgInfo->EventsTable;
	for(i = 1; i < EventsTable->iEventsNumber; ++i)
	{
		if((EventsTable->EventsInfo[i].iStyle != 2)
		&& (EventsTable->EventsInfo[i].iStyle < 10))
		{
			printf("JudgeMacrobendingLossEvent 1111-----i = %d!!!\n", i);

			float position = EventsTable->EventsInfo[i].fEventsPosition;
			int index[SOLA_WAVE_NUM] = { 0, 0, 0 };
			float minEnd = 500000.0f;
			for(j = 0; j < SOLA_WAVE_NUM; ++j)
			{
				int eventNum = pSolaAlgInfo->pAlgorithmInfo[j].EventsTable.iEventsNumber;
				index[j] = IsEventExist(&pSolaAlgInfo->pAlgorithmInfo[j].EventsTable, position);
				if(eventNum != 0)
				{
					minEnd = Min(minEnd, pSolaAlgInfo->pAlgorithmInfo[j].EventsTable.EventsInfo[eventNum - 1].fEventsPosition);
				}
			}

			printf("JudgeMacrobendingLossEvent 2222-----i = %d!!!\n", i);

			int isMacrobending = 0, cnt = 0;
			for(j = 0; j < SOLA_WAVE_NUM; ++j)
			{
				// 判断依据 1) 1550nm测试出衰减事件
				//          2) 1310nm未能测试出事件
				//			3) 事件损耗是正值
				if((index[j] < 0) && (EventsTable->EventsInfo[i].fLoss > 0.0f)
				&&(j == 0) && (EventsTable->EventsInfo[i].iStyle == 0))
				{
					isMacrobending = 1;
					break;
				}

				// 判断依据 1) 1310nm,1550nm测试出事件,且都不是反射事件
				// 			2) 该事件不是1310nm以及1550nm的末端事件
				//			3) 该事件小于最小末端
				else if((index[j] != 1000) && (index[j] != -1)
					&& (EventsTable->EventsInfo[i].fEventsPosition < minEnd)
					&& ((pSolaAlgInfo->pAlgorithmInfo[j].EventsTable.EventsInfo[index[j]].iStyle != 2)
					&& (pSolaAlgInfo->pAlgorithmInfo[j].EventsTable.EventsInfo[index[j]].iStyle != 12)))
				{
					cnt++;
				}
			}

			printf("JudgeMacrobendingLossEvent 3333-----i = %d!!!\n", i);

			if (isMacrobending)
			{
				printf("JudgeMacrobendingLossEvent 4444-----i = %d!!!\n", i);
				pSolaAlgInfo->EventsTable.EventsInfo[i].iStyle = 5;
				for (k = 0; k < SOLA_WAVE_NUM; ++k)
				{
					if ((k != j) && (index[k] != 1000))
					{
						pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].iStyle = 5;
					}
				}
			}
			else
			{
				printf("JudgeMacrobendingLossEvent 5555-----i = %d!!!\n", i);
				float times = 1.0f;
				int deltaEventStyle = 0;
				float deltaEventLoss = 0.0f;
				if(cnt > 1)
				{
					printf("JudgeMacrobendingLossEvent 6666-----i = %d!!!\n", i);
					cnt = 0;
					for(k = 0; k < SOLA_WAVE_NUM; ++k)
					{
						if((index[k] != 1000)
						&& (pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].iStyle <= 11))
						{
							deltaEventLoss = pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].fLoss - deltaEventLoss;
							times = pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].fLoss / times;
							int styleTemp = pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].iStyle;
							if(pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].iStyle >= 10)
							{
								styleTemp -= 10;
							}
							deltaEventStyle = styleTemp - deltaEventStyle;
							cnt++;
						}
					}
					printf("JudgeMacrobendingLossEvent 7777-----i = %d!!!\n", i);

					if((cnt > 1) && ((fabsf(deltaEventLoss) > 0.2f)
						|| (times >= 5) || (deltaEventStyle != 0)))
					{
						if(pSolaAlgInfo->EventsTable.EventsInfo[i].iStyle >= 10)
						{
							pSolaAlgInfo->EventsTable.EventsInfo[i].iStyle = 15;
						}
						else
						{
							pSolaAlgInfo->EventsTable.EventsInfo[i].iStyle = 5;
						}
						printf("JudgeMacrobendingLossEvent 8888-----i = %d!!!\n", i);
						for(k = 0; k < SOLA_WAVE_NUM; ++k)
						{
							if((index[k] != 1000) && (index[k] >= 0))
							{
								if(pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].iStyle >= 10)
								{
									pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].iStyle = 15;
								}
								else
								{
									pSolaAlgInfo->pAlgorithmInfo[k].EventsTable.EventsInfo[index[k]].iStyle = 5;
								}
							}
						}

						printf("JudgeMacrobendingLossEvent 9999-----i = %d!!!\n", i);
					}
				}
			}
		}
	}

	printf("JudgeMacrobendingLossEvent over !!!\n");
}

// 生成总的事件列表
void GenerateSolaTable(SOLA_INFO *pSolaAlgInfo)
{
    int i = 0, j = 0, k = 0;
    EVENTS_TABLE *SolaFinal = &pSolaAlgInfo->EventsTable;
	float blind1 = 0.0f;
	float blind2 = 0.0f;
    //衰减率
    int index = 0;
    for (index = 0; index < SOLA_WAVE_NUM; ++index)
    {
        if (pSolaAlgInfo->pAlgorithmInfo[index].EventsTable.iEventsNumber != 0)
        {
            memcpy(SolaFinal, &pSolaAlgInfo->pAlgorithmInfo[index].EventsTable, sizeof(EVENTS_TABLE));
			blind1 = pSolaAlgInfo->pAlgorithmInfo[index].iBlind * pSolaAlgInfo->pAlgorithmInfo[index].fPointLength;
            break;
        }
    }
    float  attenuationRatio[3] = { 0.33f, 0.18f, 0.20f };
    SolaFinal->EventsInfo[0].fAttenuation = attenuationRatio[j];

    for (j = 0; j < SOLA_WAVE_NUM; ++j)
    {
        if (j != index)
        {
            EVENTS_TABLE *SolaSingleWave = &pSolaAlgInfo->pAlgorithmInfo[j].EventsTable;

            if (SolaSingleWave->iEventsNumber == 0)
            {
                continue;
            }
			blind2 = pSolaAlgInfo->pAlgorithmInfo[j].iBlind * pSolaAlgInfo->pAlgorithmInfo[j].fPointLength;
            // 临时存储，用于重置被修改的算法结构体pAlgorithmInfo
            int solaSingleWaveEventNum = SolaSingleWave->iEventsNumber, istyleReturn = 0;

            printf("solaSingleWaveEventNum1 = %d\n", solaSingleWaveEventNum);
            // 正常结束事件
            if ((SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle != 21)
                && (SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle != 21))
            {
                if (SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].fEventsPosition
					> SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].fEventsPosition)
                {
                    if (SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle >= 10)
                    {
                        printf("-----0------\n");
                        SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle -= 10;
                        istyleReturn = 2;
                    }
                }
                else
                {
					if (SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle >= 10)
					{
						SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle -= 10;
					}
                    printf("-----1------\n");
                    //SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle -= 10;
                    istyleReturn = 1;
                }
            }
            else if (SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle == 21)
            {
                if (SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].fEventsPosition
                    < SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].fEventsPosition)
                {
                    printf("-----2------\n");
                    solaSingleWaveEventNum = SolaSingleWave->iEventsNumber - 1;
                }
                else
                {
                    if (SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle == 21)
                    {
                        printf("-----3------\n");
                        SolaFinal->iEventsNumber--;
                    }
                    else
                    {
						if (SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle >= 10)
						{
							SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle -= 10;
						}
                        //SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle -= 10;
                    }
                }
            }
            else
            {
                if (SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].fEventsPosition
                    > SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].fEventsPosition)
                {
                    printf("-----4------\n");
                    SolaFinal->iEventsNumber--;
                }
                else
                {
                    if (SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle == 21)
                    {
                        printf("-----5------\n");
                        solaSingleWaveEventNum = SolaSingleWave->iEventsNumber - 1;
                    }
                    else
                    {
						if (SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle >= 10)
						{
							SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle -= 10;
						}
                        //SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle -= 10;
                        istyleReturn = 1;
                    }
                }
            }

#ifdef SOLADEBUG
            printf("-----------GenerateSolaTable start2:SolaFinal------------------\n");
            FiberEndEventsTable(SolaFinal);
            printf("-----------GenerateSolaTable start2:SolaSingleWave------------------\n");
            FiberEndEventsTable(SolaSingleWave);
#endif
            printf("solaSingleWaveEventNum2 = %d\n", solaSingleWaveEventNum);
            for (i = 1; i < solaSingleWaveEventNum; ++i)
            {
                int SameFlag = 0;

                float posBegin1 = SolaSingleWave->EventsInfo[i].fEventsPosition;
                float posEnd1 = SolaSingleWave->EventsInfo[i].fEventsEndPosition;

                for (k = 1; k < SolaFinal->iEventsNumber; ++k)
                {
                    float posBegin2 = SolaFinal->EventsInfo[k].fEventsPosition;
                    float posEnd2 = SolaFinal->EventsInfo[k].fEventsEndPosition;
					float precision = Min(blind1, blind2);
					if (SolaFinal->EventsInfo[k].iStyle == 0)
					{
						precision = Max(precision, 10.0f);
						precision = Min(precision, 100.0f);
					}
                    // 相同事件					
                    if ((fabsf(Max(posEnd1, posEnd2) - Min(posBegin1, posBegin2))
                        < posEnd1 - posBegin1 + posEnd2 - posBegin2)
						|| (fabsf(posBegin1 - posBegin2) < precision))
                    {
                        SameFlag = 1;
                        // 对相同事件距离进行统一
                        SolaSingleWave->EventsInfo[i].fEventsPosition
                            = SolaFinal->EventsInfo[k].fEventsPosition;
                        SolaSingleWave->EventsInfo[i].fEventsEndPosition
                            = SolaFinal->EventsInfo[k].fEventsEndPosition;
                        if ((i == SolaSingleWave->iEventsNumber - 1)
							&& (k == SolaFinal->iEventsNumber - 1)
                            && (istyleReturn == 2))
                        {
                            SolaFinal->EventsInfo[SolaFinal->iEventsNumber - 1].iStyle += 10;
                        }
                        break;
                    }
                }
                if (!SameFlag)
                {
                    printf("--------------hello--------------\n");
                    SolaFinal->EventsInfo[SolaFinal->iEventsNumber]
						= SolaSingleWave->EventsInfo[i];
                    SolaFinal->iEventsNumber++;
                }
                EventsTableSort2(SolaFinal);
            }
            // 重置事件类型
            if (istyleReturn == 1)
            {
                SolaSingleWave->EventsInfo[SolaSingleWave->iEventsNumber - 1].iStyle += 10;
            }
        }
    }

    CalTotalFloss(SolaFinal);

#ifdef SOLADEBUG
    for (i = 0; i < SOLA_WAVE_NUM; i++)
    {
        printf("-----------GenerateSolaTable over:pAlgorithmInfo[%d].EventsTable------------------\n", i);
        FiberEndEventsTable(&pSolaAlgInfo->pAlgorithmInfo[i].EventsTable);
        printf("\n");
    }

    printf("-----------GenerateSolaTable over------------------\n");
    FiberEndEventsTable(&pSolaAlgInfo->EventsTable);

#endif

}

// SOLA事件合并算法总函数
void SOLAMergeEventsTable(ALGORITHM_INFO *pNewAlgInfo,
	SOLA_INFO *pSolaAlgInfo)
{
    // 每次进行事件合并前对SOLA返回的事件列表清零
    memset(&pSolaAlgInfo->EventsTable, 0, sizeof(pSolaAlgInfo->EventsTable));

    // 取消补偿
    int i = 0, j = 0;
    for (i = 0; i < SOLA_WAVE_NUM; ++i)
    {
        int eventNum = pSolaAlgInfo->pAlgorithmInfo[i].EventsTable.iEventsNumber;
        for (j = 0; j < eventNum; ++j)
        {
            pSolaAlgInfo->pAlgorithmInfo[i].EventsTable.EventsInfo[j].fLoss -= solaFloss[i][j];
        }
        if (eventNum > 0)
            pSolaAlgInfo->pAlgorithmInfo[i].EventsTable.EventsInfo[eventNum - 1].fTotalLoss
			-= solaFloss[i][MAX_EVENTS_NUMBER - 1];
    }

    int iNewWave = pNewAlgInfo->SetParameters.iwavelength;
    // 不同脉宽的事件合并
    MergeDifferentPulseEventLis(pNewAlgInfo, &pSolaAlgInfo->pAlgorithmInfo[iNewWave]);

    // 生成总的事件列表
    GenerateSolaTable(pSolaAlgInfo);

    // 判断是否是宏弯曲事件
    JudgeMacrobendingLossEvent(pSolaAlgInfo);

	// 清除sola损耗补偿
	ClearSolaEventLossOffset();
}

// sola插入合并事件
void InsertEvent(ALGORITHM_INFO *CurAlgorithmStruct,
	ALGORITHM_INFO *RetSola,
	int curIndex,
	int retIndex,
	int flag)
{
    EVENTS_TABLE *RetEvent = &RetSola->EventsTable;
    EVENTS_TABLE *CurEvent = &CurAlgorithmStruct->EventsTable;

    float standardAttenuation[] = { 0.33f, 0.18f, 0.2f };

    if ((curIndex == 0) || (flag == 2))
    {
        return;
    }

    // 事件合并
    if (flag == 1)
    {
        if (RetEvent->EventsInfo[retIndex].fEventsPosition
			< RetSola->solaEffectiveRange)
        {
            if (retIndex != RetEvent->iEventsNumber - 1)
            {
                printf("-0-----curIndex=%d,retIndex=%d--\n", curIndex, retIndex);
            }
            else
            {
                RetEvent->EventsInfo[retIndex].fLoss
					= CurEvent->EventsInfo[curIndex].fLoss;
#if 0
                float flossTemp = Max(RetEvent->EventsInfo[retIndex].fLoss,
                    CurEvent->EventsInfo[curIndex].fLoss);
                RetEvent->EventsInfo[retIndex] = CurEvent->EventsInfo[curIndex];
                if ((CurAlgorithmStruct->spliterRatioGrade1 >= 4)
                    || (CurAlgorithmStruct->spliterRatioGrade2 >= 4)
                    || (CurAlgorithmStruct->spliterRatioGrade3 >= 4))
                {
                    if ((RetSola->PulseWidth < 6) && (curIndex != CurEvent->iEventsNumber - 1))
                    {

                        RetEvent->EventsInfo[retIndex].fLoss = flossTemp;
                    }
                }
#endif
                printf("-00-----curIndex=%d,retIndex=%d--\n", curIndex, retIndex);
            }
        }
        else
        {
            if (CurEvent->EventsInfo[curIndex].fEventsPosition
				< CurAlgorithmStruct->solaEffectiveRange)
            {
                if ((curIndex == CurEvent->iEventsNumber - 1)
					&& (retIndex != RetEvent->iEventsNumber - 1))
                {
                    printf("-1-----curIndex=%d,retIndex=%d--\n", curIndex, retIndex);
                }
                else
                {
                    float twoEventDistance = RetEvent->EventsInfo[retIndex].fEventsPosition
                        - RetEvent->EventsInfo[retIndex - 1].fEventsPosition;
                    if (CurAlgorithmStruct->iBlind * CurAlgorithmStruct->fPointLength
						> twoEventDistance / 3.0f)
                    {
                        printf("-2-----curIndex=%d,retIndex=%d--\n", curIndex, retIndex);
                    }
                    else
                    {
                        RetEvent->EventsInfo[retIndex] = CurEvent->EventsInfo[curIndex];
                        printf("-3-----curIndex=%d,retIndex=%d--\n", curIndex, retIndex);
                    }
                }
            }

        }
    }
    // 事件添加
    else if (flag == 0)
    {

        // 末尾添加事件
        if (CurEvent->EventsInfo[curIndex].fEventsPosition
					> RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].fEventsPosition)
        {
            RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].fAttenuation
                = standardAttenuation[CurAlgorithmStruct->SetParameters.iwavelength];

            if (curIndex - 1 != 0)
            {
                float beforeCurEventBegin = CurEvent->EventsInfo[curIndex - 1].fEventsPosition;
                float beforeCurEventEnd = CurEvent->EventsInfo[curIndex - 1].fEventsEndPosition;

                float retEventBegin = RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].fEventsPosition;
                float retEventEnd = RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].fEventsEndPosition;

                if (fabsf(Max(retEventEnd, beforeCurEventEnd)
					- Min(retEventBegin, beforeCurEventBegin))
                    < retEventEnd - retEventBegin + beforeCurEventEnd - beforeCurEventBegin)
                {
                    if (RetEvent->iEventsNumber - 2 >= 0)
                    {
                        float beforeRetEventBegin = RetEvent->EventsInfo[RetEvent->iEventsNumber - 2].fEventsPosition;
                        float beforeRetEventEnd = RetEvent->EventsInfo[RetEvent->iEventsNumber - 2].fEventsEndPosition;
                        if (fabsf(Max(beforeRetEventEnd, beforeCurEventEnd)
							- Min(beforeRetEventBegin, beforeCurEventBegin))
                            < beforeRetEventEnd - beforeRetEventBegin + beforeCurEventEnd - beforeCurEventBegin)
                        {
                            ;
                        }

                    }
                    else
                    {
                        RetEvent->EventsInfo[RetEvent->iEventsNumber - 1].fLoss
							= CurEvent->EventsInfo[curIndex - 1].fLoss;
                    }
                }
            }

        }
        // 中间添加事件
        else
        {
            if (curIndex == CurEvent->iEventsNumber - 1)
            {
                CurEvent->EventsInfo[curIndex].fAttenuation
                    = standardAttenuation[CurAlgorithmStruct->SetParameters.iwavelength];
            }
        }
        RetEvent->EventsInfo[RetEvent->iEventsNumber] = CurEvent->EventsInfo[curIndex];
        RetEvent->iEventsNumber++;
        EventsTableSort2(RetEvent);
    }
    else if (flag == 4)
    {
        if ((((RetEvent->EventsInfo[retIndex].iPulse == 2)
			&& (RetEvent->EventsInfo[retIndex + 1].iPulse == 2))
            || ((RetEvent->EventsInfo[retIndex].iPulse == 3)
				&& (RetEvent->EventsInfo[retIndex + 1].iPulse == 2)))
            && (RetEvent->EventsInfo[retIndex + 1].fEventsPosition
				- RetEvent->EventsInfo[retIndex].fEventsPosition < MIN_MEASURE_DISTANCE))
        {
            RetEvent->EventsInfo[retIndex].fLoss
				= CurEvent->EventsInfo[curIndex].fLoss;

        }

        if ((retIndex + 1 == RetEvent->iEventsNumber - 1)
			&& (curIndex != CurEvent->iEventsNumber - 1))
        {
            if (RetEvent->EventsInfo[retIndex + 1].fLoss
                + RetEvent->EventsInfo[retIndex].fLoss
				> CurEvent->EventsInfo[curIndex].fLoss)
            {
                printf("----5-------\n");
            }
            else
            {
                RetEvent->EventsInfo[retIndex + 1].fLoss
					= CurEvent->EventsInfo[curIndex].fLoss
                    - RetEvent->EventsInfo[retIndex].fLoss;
                printf("----6-------\n");
            }
        }
    }

    int i = 0;
    for (i = 0; i < RetEvent->iEventsNumber - 1; ++i)
    {
        if ((RetEvent->EventsInfo[i].fAttenuation > 1.0f)
            || (isnan(RetEvent->EventsInfo[i].fAttenuation))
            || (isinf(RetEvent->EventsInfo[i].fAttenuation)))
        {
            RetEvent->EventsInfo[i].fAttenuation
                = standardAttenuation[CurAlgorithmStruct->SetParameters.iwavelength];
        }
    }
}

// 计算累计损耗
void CalTotalFloss(EVENTS_TABLE* eventTable)
{
    //计算链路总损耗
    int i = 0;
    float totalLoss = 0.0f;
    float standardAttenuation[3] = { 0.33f, 0.18f, 0.21f };				// 不同波长标准衰减
    float normalAttenuation[][2] = { { 0.2f, 0.55f }, { 0.05f, 0.35f }, { 0.05f, 0.35f } };	// 不同波长衰减上下限
	eventTable->EventsInfo[0].fLoss = 0.0f;
    for (i = 1; i < eventTable->iEventsNumber; ++i)
    {
        if ((eventTable->EventsInfo[i - 1].fAttenuation 
				> normalAttenuation[eventTable->EventsInfo[i].iWaveSymbol][0])
            && (eventTable->EventsInfo[i - 1].fAttenuation
				< normalAttenuation[eventTable->EventsInfo[i].iWaveSymbol][1]))
            totalLoss += (eventTable->EventsInfo[i - 1].fLoss
				+ eventTable->EventsInfo[i - 1].fAttenuation
				* (eventTable->EventsInfo[i].fEventsPosition
				- eventTable->EventsInfo[i - 1].fEventsPosition) / 1000.0f);
		else
		{
			eventTable->EventsInfo[i - 1].fAttenuation = standardAttenuation[eventTable->EventsInfo[i].iWaveSymbol];
			eventTable->EventsInfo[i - 1].fContinueAttenuation = eventTable->EventsInfo[i - 1].fAttenuation
				* (eventTable->EventsInfo[i].fEventsPosition - eventTable->EventsInfo[i - 1].fEventsPosition) / 1000.0f;
			totalLoss += eventTable->EventsInfo[i - 1].fLoss;
			totalLoss += eventTable->EventsInfo[i - 1].fContinueAttenuation;
		}
            
        eventTable->EventsInfo[i].fTotalLoss = totalLoss;
    }
}

// 获取动态阈值分界点
void FindKeyPoint(float* sig,
	float* stdArray,
	int startPos,
	int calTime,
	int pulse,
	int iterval,
	int continueNum,
	float stdSmpRatio,
	SNR_KEY_POINT *snrKeyPoint,
	int mode)
{
	float sola_snr = 0.03f;
	float max_sola_snr = 0.2f;
	if (mode)
	{
		max_sola_snr = 0.1f;
		sola_snr = 0.02f;
	}

	if (snrKeyPoint->keyPointNum > 2)
	{
		return;
	}

	// 获得SNR参考值
	int i = 0, cnt = 0;
	float minVar = 0.0f, maxVar = 0.0f;
	float stdRefference = 0.0f;

	if (snrKeyPoint->keyPointNum == 0)
	{
		startPos = (int)ceil(blindWidth[pulse][0] / g_fLengthPerPoint / iterval);
	}

	for (i = startPos; i < calTime - 10; ++i)
	{
		minVar = GetMinFloat(stdArray + i, (int)Max(3, stdSmpRatio));
		maxVar = GetMaxFloat(stdArray + i, (int)Max(3, stdSmpRatio));

		if (maxVar - minVar < sola_snr)
		{
			stdRefference = GetMeanFloat(stdArray + i, (int)Max(3, stdSmpRatio));
			startPos = i;

			int kk = 0;
			minVar = 100, maxVar = 0;
			for (kk = i + 3; kk < Min(calTime - 10, i + 3 + 10); ++kk)
			{
				if (stdArray[kk] < stdRefference)
				{
					minVar = Min(minVar, stdArray[kk]);
					maxVar = Max(maxVar, stdArray[kk]);
					cnt++;
				}
				if ((cnt >= 5) && (maxVar - minVar < sola_snr))
				{
					stdRefference = (maxVar + minVar) / 2;
					startPos = kk;
					break;
				}
			}

			break;
		}
	}

	//printf("stdRefference = %f\n", stdRefference);

	// 信噪比太差
	if (stdRefference == 0.0f)
	{
		if (snrKeyPoint->keyPointNum == 0)
		{
			startPos = 0;
		}
		snrKeyPoint->keyPoint[snrKeyPoint->keyPointNum] = startPos;
		snrKeyPoint->referValue[snrKeyPoint->keyPointNum] = stdRefference;
		snrKeyPoint->keyPointNum++;
		return;
	}

	if (stdRefference > max_sola_snr)
	{
		return;
	}

	// 获得有效光纤距离
	cnt = 0;
	int endPos = 0;

	//向前搜索
	for (i = startPos; i < calTime; ++i)
	{
		float temp = stdArray[i];
		if (temp > stdRefference + sola_snr)
		{
			cnt++;
		}
		else
		{
			cnt = 0;
		}

		if ((i == calTime - 1) && (stdRefference < 1.0f)
			&& (cnt < continueNum * stdSmpRatio))
		{
			endPos = calTime - 1;
			break;
		}
		int error = 0;
		if (cnt >= continueNum * stdSmpRatio)
		{
			// 防止由于分路器或者通道选择过大导致有效距离判断错误
			cnt = 0;
			int jj = 0;
			for (jj = i; jj < calTime; ++jj)
			{
				if (calTime - jj < 10)
				{
					temp = 1000;
				}
				else
				{
					temp = stdArray[jj];
				}
				if (temp <= stdRefference + sola_snr)
				{
					cnt++;
				}
				else
				{
					cnt = 0;
				}
				if (cnt > continueNum*stdSmpRatio)
				{
					error = 1;
					cnt = 0;
					break;
				}
			}

			if (error == 0)
			{
				endPos = i - (int)(continueNum * stdSmpRatio) + 1;
				//判断前面10个点是否存在反射峰
				float delta = GetMaxFloat(sig + (int)((i - continueNum * stdSmpRatio) * iterval),
					(int)(continueNum * stdSmpRatio) * iterval)
					- sig[(i - (int)(continueNum * stdSmpRatio)) * iterval];

				if (delta > 2.0f)
				{
					endPos = i;
				}

				if (endPos < 10)
				{

					break;
				}
				else
				{
					int k = 0, success = 0, successFlag = 0;

					// 反向搜索
					for (k = endPos; k > startPos; --k)
					{
						if (k < 9)
						{
							endPos = k;
							successFlag = 1;
							break;
						}
						maxVar = GetMeanFloat(stdArray + k - 9, 10);

						if (maxVar < 0.5f)
						{
							if (maxVar - stdRefference < sola_snr)
							{
								success++;
								if (success >= 3)
								{
									endPos = k + 3;
									successFlag = 1;
									break;
								}
							}
							else
							{
								success = 0;
							}
						}
						else
						{
							if (stdArray[k] > stdRefference + 0.2f)
							{
								endPos = k + 1;
								successFlag = 1;
								break;
							}
						}
					}
					if (successFlag == 1)
					{
						break;
					}
					break;
				}
			}

		}
	}

	snrKeyPoint->keyPoint[snrKeyPoint->keyPointNum] = endPos;
	snrKeyPoint->referValue[snrKeyPoint->keyPointNum] = stdRefference;
	snrKeyPoint->keyPointNum++;

	FindKeyPoint(sig, stdArray, endPos, calTime, 
		pulse, iterval, continueNum, stdSmpRatio, snrKeyPoint, mode);
}

// 计算信号信噪比SNR,获取不同脉宽下的有效距离
float GetEffectiveFiberRange(ALGORITHM_INFO *pNewAlgInfo)
{
    int iterval = Max(pNewAlgInfo->iBlind, 5);

    int continueNum = 0;
    int blind = pNewAlgInfo->iBlind;
    int pulse = pNewAlgInfo->PulseWidth;

    // 正常事件区间不会超过10倍盲区(一般情况)，10us，20us脉宽的事件区间不会超过3倍盲区
    if (pulse >= ENUM_PULSE_10US)
    {
        continueNum = 5;
    }
    else
    {
        continueNum = 10;
    }

    // 防止采样间隔过大，导致标准差分辨率不够
    int stdSmpRatio = 1;
    if (iterval >= 50)
    {
        iterval = 50;
        stdSmpRatio = blind / iterval;
    }

    int len = pNewAlgInfo->iSignalLength;
    float *sig = pNewAlgInfo->pSignalLog;
    int calTime = (int)(len / iterval);

    // 计算信噪比(用标准差衡量信噪比)
    int i = 0;
    float *stdArray = (float*)malloc(sizeof(float) * calTime);
    for (i = 0; i < calTime; i++)
    {
        stdArray[i] = GetStdFloat(sig + i * iterval, iterval);
    }

    // 获得SNR参考值
    int cnt = 0;
    int startPos = 0;
    float minVar = 0.0f, maxVar = 0.0f;
    float stdRefference = 0.0f;
    for (i = 0; i < calTime - 10; ++i)
    {
        cnt = 0;
        minVar = GetMinFloat(stdArray + i, 5);
        maxVar = GetMaxFloat(stdArray + i, 5);

        if (maxVar - minVar < SOLA_EFFECTIVE_RANGE_SNR)
        {
            stdRefference = GetMeanFloat(stdArray + i, 5);
            startPos = i;
            break;
        }
    }

    // 信噪比太差
    if (stdRefference == 0.0f)
    {
        if (frontParament.pulseWidth >= ENUM_PULSE_2US)
        {
            free(stdArray);
            return range;
        }
        else
        {
            free(stdArray);
            return 200.0f;
        }
    }
    printf("stdRefference=%f\n", stdRefference);


    // 获得有效光纤距离
    cnt = 0;
    int endPos = 0;
    float effectiveRange = 0.0f;

    //向前搜索
    for (i = startPos; i < calTime; ++i)
    {
        if (stdArray[i] > stdRefference + SOLA_EFFECTIVE_RANGE_SNR)
        {
            cnt++;
        }
        else
        {
            cnt = 0;
        }
        int error = 0;
        if (cnt >= continueNum * stdSmpRatio)
        {
            // 防止由于分路器或者通道选择过大导致有效距离判断错误
            minVar = GetMinFloat(stdArray + i, Min(30, calTime - 1 - i));
            if (minVar < stdRefference + SOLA_EFFECTIVE_RANGE_SNR)
            {
                error = 1;
                cnt = 0;
            }
            if (error == 0)
            {
                endPos = i - (int)(continueNum * stdSmpRatio) + 1;
                if (endPos < 10)
                {
                    break;
                }
                else
                {
                    int k = 0, success = 0, successFlag = 0;

                    // 反向搜索
                    for (k = endPos; k > startPos; --k)
                    {
                        if (k < 9)
                        {
                            endPos = k;
                            successFlag = 1;
                            break;
                        }
                        minVar = GetMinFloat(stdArray + k - 9, 10);
                        maxVar = GetMaxFloat(stdArray + k - 9, 10);

                        if (maxVar < 0.5f)
                        {
                            if (maxVar - minVar < SOLA_EFFECTIVE_RANGE_SNR)
                            {
                                success++;
                                if (success >= 3)
                                {
                                    endPos = k + 3;
                                    successFlag = 1;
                                    break;
                                }
                            }
                            else
                            {
                                success = 0;
                            }
                        }
                        else
                        {
                            if (stdArray[k] > stdRefference + 0.2f)
                            {
                                endPos = k + 1;
                                successFlag = 1;
                                break;
                            }
                        }
                    }
                    if (successFlag == 1)
                    {
                        break;
                    }
                    break;
                }
            }
        }
    }

    int snrRangeOffset = (int)(blind * continueNum / 2);
    if (pulse >= ENUM_PULSE_10US)
    {
        snrRangeOffset = 2 * blind;
    }
    minVar = GetMinFloat(sig + endPos * iterval,
		Min(snrRangeOffset, len - endPos * iterval));
    maxVar = GetMaxFloat(sig + endPos * iterval,
		Min(snrRangeOffset, len - endPos * iterval));

    if (maxVar - minVar > 2.0f)
    {
        effectiveRange = (endPos * iterval + snrRangeOffset) * pNewAlgInfo->fPointLength;
    }
    else
    {
        effectiveRange = endPos * iterval * pNewAlgInfo->fPointLength;
    }

    free(stdArray);

    return effectiveRange;
}

// 计算信号信噪比SNR,获取不同脉宽下的有效距离
SNR_KEY_POINT* GetEffectiveFiberRange_mini2(ALGORITHM_INFO *pNewAlgInfo)
{
	int iterval = Max(pNewAlgInfo->iBlind, 5);

	int continueNum = 0;
	int blind = pNewAlgInfo->iBlind;
	int pulse = pNewAlgInfo->PulseWidth;

	// 10ns,20ns使用的通道较大
	// 正常事件区间不会超过10倍盲区(一般情况)，10us，20us脉宽的事件区间不会超过3倍盲区
	if ((pulse == ENUM_PULSE_10NS) || (pulse == ENUM_PULSE_20NS))
	{
		continueNum = 30;
	}
	else if (pulse >= ENUM_PULSE_500NS)
	{
		continueNum = 2;
	}
	else
	{
		continueNum = 10;
	}

	// 防止采样间隔过大，导致标准差分辨率不够
	float stdSmpRatio = 1.0f;
	if (iterval >= 50)
	{
		iterval = 50;
		stdSmpRatio = (float)ceil((float)blind / iterval);
	}

	int len = pNewAlgInfo->iSignalLength;
	float *sig = pNewAlgInfo->pSignalLog;
	int calTime = (int)(len / iterval);

	// 计算信噪比(用标准差衡量信噪比)
	int i = 0;
	float *stdArray = (float*)malloc(sizeof(float) * calTime);
	for (i = 0; i < calTime; ++i)
	{
		stdArray[i] = GetStdFloat(sig + i * iterval, iterval);
	}

#ifdef CHECKSNR 
	PrintWaveLetStd(stdArray, calTime, pulse);
#endif

	SNR_KEY_POINT *snrKeyPoint = (SNR_KEY_POINT *)malloc(sizeof(SNR_KEY_POINT));
	memset(snrKeyPoint, 0, sizeof(SNR_KEY_POINT));

	// sola测试力求测试准确，且认为事件判别在信噪比好的情况下更可信，且不需要担心漏判(多脉宽)；
	// otdr测试对于信噪比的要求不需要那么苛刻，防止漏判
	int spliterFlag = ((pNewAlgInfo->spliterRatioGrade1)
		|| (pNewAlgInfo->spliterRatioGrade2)
		|| (pNewAlgInfo->spliterRatioGrade3));
	int mode = 0;
	if ((spliterFlag) && (pNewAlgInfo->iSOLATest == 1))
	{
		mode = 1;
	}

	FindKeyPoint(sig, stdArray, 0, calTime, pulse,
		iterval, continueNum, stdSmpRatio, snrKeyPoint, mode);

	// 将std小于0.03归为一类
	int j = 0;
	for (i = 1; i < snrKeyPoint->keyPointNum - 1;)
	{
		if (snrKeyPoint->referValue[i] < 0.03f)
		{
			for (j = i; j < snrKeyPoint->keyPointNum - 1; ++j)
			{
				snrKeyPoint->keyPoint[j - 1] = snrKeyPoint->keyPoint[j];
			}
			snrKeyPoint->keyPointNum--;
		}
		else
		{
			i++;
		}
	}

	float maxThreshold = 7.0f;
	float minThreshold = 1.0f;
	if (pulse <= ENUM_PULSE_500NS)
	{
		maxThreshold = 6.0f;
	}

	if (pulse <= ENUM_PULSE_200NS)
	{
		maxThreshold = 5.5f;
	}

	if (pulse == ENUM_PULSE_1US)
	{
		maxThreshold = 5.0f;
	}
	if (pNewAlgInfo->iSOLATest == 1)
	{
		maxThreshold += 1.0f;
		minThreshold += 1.0f;
	}

	int minPosition = blind + SearchEnd(sig + blind,
		len, pNewAlgInfo->fMaxInputNoise + maxThreshold);
	int maxPosition = blind + SearchEnd(sig + blind,
		len, pNewAlgInfo->fMaxInputNoise + minThreshold);

	if (snrKeyPoint->keyPointNum > 0)
	{
		if (snrKeyPoint->keyPoint[snrKeyPoint->keyPointNum - 1] * iterval < minPosition)
		{
			snrKeyPoint->keyPoint[snrKeyPoint->keyPointNum - 1] = minPosition / iterval;
		}
		else if (snrKeyPoint->keyPoint[snrKeyPoint->keyPointNum - 1] * iterval > maxPosition)
		{
			snrKeyPoint->keyPoint[snrKeyPoint->keyPointNum - 1] = maxPosition / iterval;

			for (i = 0; i < snrKeyPoint->keyPointNum - 1; ++i)
			{
				snrKeyPoint->keyPoint[i]
					= Min(maxPosition / iterval - 1, snrKeyPoint->keyPoint[i]);
			}
		}
	}

	for (i = 0; i < snrKeyPoint->keyPointNum; ++i)
	{
		snrKeyPoint->keyPointDistance[i]
			= snrKeyPoint->keyPoint[i] * iterval * pNewAlgInfo->fPointLength;
	}

	int endPos = snrKeyPoint->keyPoint[snrKeyPoint->keyPointNum - 1];

	if (endPos == 0)
	{
		pNewAlgInfo->solaEffectiveRange = 0;
		free(stdArray);
		return snrKeyPoint;
	}

	int snrRangeOffset = 0;
	if (pulse >= ENUM_PULSE_10US)
	{
		snrRangeOffset = 2 * blind;
	}
	float minVar = GetMeanFloat(sig + endPos * iterval,
		Min(snrRangeOffset, len - endPos * iterval));
	float maxVar = GetMaxFloat(sig + endPos * iterval,
		Min(snrRangeOffset, len - endPos * iterval));

	float effectiveRange = 0.0f;

	if (maxVar - minVar > 3.0f)
	{
		effectiveRange = (endPos * iterval + snrRangeOffset) * pNewAlgInfo->fPointLength;
	}
	else
	{
		effectiveRange = endPos * iterval * pNewAlgInfo->fPointLength;
	}

	snrKeyPoint->keyPointDistance[snrKeyPoint->keyPointNum - 1] = effectiveRange;

	free(stdArray);
	pNewAlgInfo->solaEffectiveRange = effectiveRange;
	return snrKeyPoint;
}

// 针对sola测量，使用较小脉宽时，自适应小波阈值
float AdjustWaveThresh(float *coef, int coefLen)
{
    if (coefLen == 0)
    {
        return 0;
    }
    float *coefTemp = (float*)malloc(coefLen * sizeof(float));
    memcpy(coefTemp, coef, coefLen * sizeof(float));

    //vector<float>Coef(vWavecoef);
    int  k = 0, i = 0;

    for (i = 0; i < coefLen; ++i)
    {
        coefTemp[i] = coef[i] * coef[i];
    }

    qsort(coefTemp, coefLen, sizeof(float), CompareFloat);

    float *risk = (float*)malloc(coefLen * sizeof(float));


    float sum = 0.0f;
    for (k = 0; k < coefLen; ++k)
    {
        sum += coefTemp[k];
        risk[k] = (sum + coefLen - 2 * (k + 1)
			+ (coefLen - 1 - k) * coefTemp[k]) / coefLen;
    }
    int MinIndex = GetMinIndex(risk, coefLen);

    free(coefTemp);
    free(risk);
    return sqrtf(coefTemp[MinIndex]);
}

// 比较函数
int CompareFloat(const void *a, const void *b)
{
	return (*((float*)a) >  *((float*)b));
}

// 判断后续信号是否有正常区间 
int NormalAttenuationSectionExist(ALGORITHM_INFO *pAlgorithmInfo, int index)
{
    int i = 0;
    EVENTS_TABLE* e = &pAlgorithmInfo->EventsTable;
    int normFlag = 1;

    float normalAttenuation[][2] = { { 0.1f, 0.7f }, { 0.05f, 0.4f } };

    if (fabsf(e->EventsInfo[index].fAttenuation) > NORMAL_ATTENUATION_UP)
    {
        for (i = index + 1; i < pAlgorithmInfo->EventsTable.iEventsNumber; ++i)
        {
            //存在一个正常区间衰减则退出 
            if ((e->EventsInfo[i].fAttenuation > normalAttenuation[frontParament.wave][0])
                && (e->EventsInfo[i].fAttenuation < normalAttenuation[frontParament.wave][1])
                && (e->EventsInfo[i].fEventsPosition < pAlgorithmInfo->solaEffectiveRange)
                && (e->EventsInfo[i].fEventsPosition - e->EventsInfo[index].fEventsEndPosition
					> 2.5f * blindWidth[frontParament.pulseWidth][0])
                && ((i != pAlgorithmInfo->EventsTable.iEventsNumber - 1)))
            {
                normFlag = 0;
                break;
            }
            else
            {
                // 重新计算衰减
                int len = (int)((e->EventsInfo[i].iBegin - e->EventsInfo[i - 1].iEnd) / 2);

                // 事件距离较短，不在进行拟合，直接下一个
                if (len < 120)
                {
                    continue;
                }
                else
                {
                    int startPos = e->EventsInfo[i].iBegin - len;
                    float coef[2] = { 0.0f, 0.0f };
                    LineFit(pAlgorithmInfo->pSignalLog + startPos, len, startPos, coef);
                    if ((coef[0] * 1000.0f / pAlgorithmInfo->fPointLength
							> normalAttenuation[frontParament.wave][0])
                        && (coef[0] * 1000.0f / pAlgorithmInfo->fPointLength
							< normalAttenuation[frontParament.wave][1])
                        && (e->EventsInfo[i].fEventsPosition 
							< pAlgorithmInfo->solaEffectiveRange))
                    {
                        normFlag = 0;
                    }
                }
            }
        }
    }
    return normFlag;
}

// 获得链路总的回波损耗
void GetTotalReturnLoss(ALGORITHM_INFO *pAlgorithmInfo)
{
    unsigned short *pTransferSignal = (unsigned short *)malloc(sizeof(unsigned short) * pAlgorithmInfo->iSignalLength);
    memset(pTransferSignal, 0, sizeof(unsigned short) * pAlgorithmInfo->iSignalLength);
    float *fSignalLog = pAlgorithmInfo->pSignalLog;
    int i = 0;
    int iTmp = 0;
    iTmp = Max(pAlgorithmInfo->iBlind, 5);
    int iBegin = pAlgorithmInfo->EventsTable.EventsInfo[0].iEnd;
    int iTmpIndex = GetMinIndex(fSignalLog + iBegin, Min(iTmp, 20));
    iBegin = iBegin + iTmpIndex;
    pAlgorithmInfo->EventsTable.EventsInfo[0].fLoss 
		= fSignalLog[0] - fSignalLog[iBegin];
    pAlgorithmInfo->EventsTable.EventsInfo[0].fLoss
		= Max(pAlgorithmInfo->EventsTable.EventsInfo[0].fLoss, 0.0f);
    int iEventsNumber = pAlgorithmInfo->EventsTable.iEventsNumber;
    int iEnd = 0;
    iEnd = Min(pAlgorithmInfo->EventsTable.EventsInfo[iEventsNumber - 1].iEnd + 2 * iTmp,
        pAlgorithmInfo->iDisplayLenth);
    float fMin = GetMinFloat(fSignalLog 
		+ pAlgorithmInfo->iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
    float fTmp = 0.0f;
    for (i = 0; i < pAlgorithmInfo->iSignalLength; ++i)
    {
        fTmp = fSignalLog[i] - fMin;
        if (fTmp < 0.0f)
        {
            pTransferSignal[i] = 0;
        }
        else
        {
            pTransferSignal[i] = (unsigned short)floor(fTmp * SCALE_FACTOR + 0.5f);
        }
    }

	 iEnd = Min(pAlgorithmInfo->iDisplayLenth,
	 pAlgorithmInfo->EventsTable.EventsInfo[iEventsNumber - 1].iEnd);
    if (iEventsNumber == 1)
    {
        pAlgorithmInfo->fTotalReturnLoss = 0.0f;
    }
    else
    {
        pAlgorithmInfo->fTotalReturnLoss
			= ReturnLoss(pTransferSignal, 0, iEnd,
				pAlgorithmInfo->SetParameters.fBackScattering,
				pAlgorithmInfo->SetParameters.fSampleRating);
    }

    free(pTransferSignal);
}

// sola损耗补偿清0
void ClearSolaEventLossOffset()
{
    memset(solaFloss[0], 0, MAX_EVENTS_NUMBER * sizeof(float));
    memset(solaFloss[1], 0, MAX_EVENTS_NUMBER * sizeof(float));
    memset(solaFloss[2], 0, MAX_EVENTS_NUMBER * sizeof(float));
}

// 设置分路器
void SetSpliterStyle(EVENTS_TABLE *pEventsTable,
	int index1, 
	int index2, 
	int spliterNum,
	int *goalSpliterRatio,
	int grade)
{
    int scale = 1 << 16;
    if ((grade > spliterNum)
		|| (index2 >= pEventsTable->iEventsNumber - 1))
    {
        return;
    }
    // 损耗刻度
    float dial[9] = { 0.0f, 2.8f, 5.5f, 8.5f, 11.5f, 14.5f, 17.5f, 20.5f, 30.0f };
    // 误差容许范围
    float errorFloss[9] = { 0.0f, 0.5f, 1.5f, 2.0f, 2.5f, 3.5f, 4.0f, 4.5f, 5.0f };

    int i = 0, successFlag = 0, startPos = 0;
    for (i = index1; i < index2; ++i)
    {
        if ((pEventsTable->EventsInfo[i].fLoss 
				>= dial[goalSpliterRatio[grade]]
				- errorFloss[goalSpliterRatio[grade]])
            && (pEventsTable->EventsInfo[i].fLoss
				< dial[goalSpliterRatio[grade] + 1]))
        {
            successFlag = 1;
            break;
        }
    }
    if (successFlag)
    {
        pEventsTable->EventsInfo[i].iStyle 
			+= scale * goalSpliterRatio[grade];
        startPos = i + 1;
    }
    else
    {
        int k = 0, minIndex = 0;
        float delta1 = 0.0f, delta2 = 0.0f, deltaFlossTemp = 1000.0f;

        // 寻找最接近该分路器的事件
        successFlag = 0;
        for (k = index1; k < index2; ++k)
        {
            if (k == index2 - 2)
            {
                if (successFlag == 1)
                {
                    break;
                }
            }
            delta1 = fabsf(pEventsTable->EventsInfo[k].fLoss
				- dial[goalSpliterRatio[grade]]);
            delta2 = fabsf(pEventsTable->EventsInfo[k].fLoss
				- dial[goalSpliterRatio[grade] + 1]);

            //小于该分路器下线2db，则认为不存在该分路器
            if ((Min(delta1, delta2) < deltaFlossTemp)
                && (pEventsTable->EventsInfo[k].fLoss
					> dial[goalSpliterRatio[grade]]
					- errorFloss[goalSpliterRatio[grade]]))
            {
                successFlag = 1;
                // 对于某个事件损耗都大于各个分路器损耗
                deltaFlossTemp = Min(delta1, delta2);
                minIndex = k;
            }
        }

        if (successFlag == 0)
        {
            ;
        }
        else
        {
            // 该事件损耗是否大于后续的分路器损耗
            int gradeIndex = grade;
            deltaFlossTemp = 1000.0f;
            for (k = grade; k < SPLITER_NUM; ++k)
            {
                if ((pEventsTable->EventsInfo[minIndex].fLoss
						> dial[goalSpliterRatio[k]]
						- errorFloss[goalSpliterRatio[k]])
                    && (dial[goalSpliterRatio[k]]
						>= dial[goalSpliterRatio[grade]]))
                {
                    delta1 = fabsf(pEventsTable->EventsInfo[minIndex].fLoss
						- dial[goalSpliterRatio[k]]);
                    delta2 = fabsf(pEventsTable->EventsInfo[minIndex].fLoss
						- dial[goalSpliterRatio[k] + 1]);
                    if (Min(delta1, delta2) < deltaFlossTemp)
                    {
                        successFlag = 2;
                        gradeIndex = k;
                        deltaFlossTemp = Min(delta1, delta2);
                    }
                }
            }
            if (successFlag == 2)
            {
                grade = gradeIndex;
            }
            pEventsTable->EventsInfo[minIndex].iStyle
				+= scale * goalSpliterRatio[grade];
            startPos = minIndex + 1;
        }
    }

    grade++;
    SetSpliterStyle(pEventsTable, startPos,
		index2 + 1, spliterNum, goalSpliterRatio, grade);
}

// 分光比自动判断
void SpliterRationAutoJudge(SOLA_INFO *SolaAlgorithm, int waveLen)
{
    EVENTS_TABLE *pEventsTable = &SolaAlgorithm->EventsTable;

    int iSpliterSet[] = { SolaAlgorithm->pAlgorithmInfo[waveLen].spliterRatioGrade1,
        SolaAlgorithm->pAlgorithmInfo[waveLen].spliterRatioGrade2,
        SolaAlgorithm->pAlgorithmInfo[waveLen].spliterRatioGrade3 };

	printf("iSpliterSet[0] = %d, iSpliterSet[1] = %d, iSpliterSet[2] = %d\n",
		iSpliterSet[0], iSpliterSet[1], iSpliterSet[2]);

    //用户不设置分光比，不做任何处理
    if (iSpliterSet[0] <= 0)
    {
        return;
    }

    //用户设置了几级的分光比
    int i = 0, iSetCount = 0;
    for (i = 0; i < 3; ++i)
    {
        printf("---iSpliterSet[%d]=%d---\n", i, iSpliterSet[i]);
        if (iSpliterSet[i] > 0)
        {
            iSetCount++;
        }
    }
    int iScale = 1 << 16;

    // 如果分路器事件数量大于测试的事件数量	
    if (pEventsTable->iEventsNumber <= iSetCount)
    {
        int spliterNum = Min(iSetCount, pEventsTable->iEventsNumber);
        for (i = 0; i < spliterNum; ++i)
        {
            pEventsTable->EventsInfo[i].iStyle += iScale * iSpliterSet[i];
        }
    }
    else
    {
        // 末端不显示分路器事件
        SetSpliterStyle(pEventsTable, 0, 
			pEventsTable->iEventsNumber - iSetCount, 2, iSpliterSet, 0);
    }
    // 分路器在判断
    SpliterJudgeAgain(SolaAlgorithm, waveLen);
}

// 对于寻找分路器不成功的情况,会再次进行判断，力求找打正确的分路器
void SpliterJudgeAgain(SOLA_INFO *SolaAlgorithm, int waveLen)
{
    int iScale = 1 << 16;
    EVENTS_TABLE *pEventsTable = &SolaAlgorithm->EventsTable;
    int iSpliterSet[] = { SolaAlgorithm->pAlgorithmInfo[waveLen].spliterRatioGrade1,
        SolaAlgorithm->pAlgorithmInfo[waveLen].spliterRatioGrade2,
        SolaAlgorithm->pAlgorithmInfo[waveLen].spliterRatioGrade3 };

    int i = 0, spliterNum = 0, largeFlossNum = 0;
    int spliterIndex[SPLITER_NUM] = { 0, 0, 0 };
    for (i = 0; i < SPLITER_NUM; ++i)
    {
        if (iSpliterSet[i] > 0)
        {
            spliterIndex[spliterNum] = i;
            spliterNum++;
        }
    }

#ifdef DEBUG_EVENTINFO
    for (i = 0; i < spliterNum; i++)
    {
        printf("spliterIndex[%d]=%d", i, spliterIndex[i]);
    }
#endif

    int minSpliterIndex = 100;
    for (i = 0; i < SPLITER_NUM; ++i)
    {
        if ((iSpliterSet[i] < minSpliterIndex) && (iSpliterSet[i] > 0))
        {
            minSpliterIndex = iSpliterSet[i];
        }
    }
    // 损耗刻度
    float dial[9] = { 0.0f, 2.8f, 5.5f, 8.5f, 11.5f, 14.5f, 17.5f, 20.5f, 30.0f };
    // 误差容许范围
    float errorFloss[9] = { 0.0f, 0.5f, 1.5f, 2.0f, 2.5f, 3.5f, 4.0f, 4.5f, 5.0f };

    int successFlag = 0;
    int spliterEventNum = 0;
    int pointIndex[SPLITER_NUM] = { 0, 0, 0 };

    float step = 0.1f, spliterMinfloss = 2.0f;
    float spliterFlossThresh = dial[minSpliterIndex] - errorFloss[minSpliterIndex];
    while (spliterFlossThresh > spliterMinfloss)
    {
        spliterFlossThresh = Min(SOLA_DEFAULT_END_FLOSSTHRESH, spliterFlossThresh);

        // 记录大损耗值点
        largeFlossNum = 0;
        spliterEventNum = 0;
        for (i = 0; i < SPLITER_NUM; ++i)
        {
            pointIndex[i] = 0;
        }
        for (i = 0; i < pEventsTable->iEventsNumber - 1; ++i)
        {
            if (pEventsTable->EventsInfo[i].iStyle > 22)
            {
                spliterEventNum++;
            }
            if (pEventsTable->EventsInfo[i].fLoss > spliterFlossThresh)
            {
                pointIndex[largeFlossNum] = i;
                largeFlossNum++;
            }
        }
        if (spliterEventNum == spliterNum)
        {
            successFlag = 1;
            break;
        }
        else
        {
            // 优先选择末端之前的事件
            if (largeFlossNum == spliterNum)
            {
                successFlag = 1;
                break;
            }
        }
        spliterFlossThresh -= step;

        // 分路器事件最小损耗值
        if (spliterFlossThresh < spliterMinfloss)
        {
            return;
        }
    }

    if (successFlag)
    {
        for (i = 0; i < largeFlossNum; ++i)
        {
            pEventsTable->EventsInfo[pointIndex[i]].iStyle &= 0x0000ffff;
            pEventsTable->EventsInfo[pointIndex[i]].iStyle += iScale * iSpliterSet[spliterIndex[i]];

            if ((pEventsTable->EventsInfo[pointIndex[i]].fLoss < 3.0f * iSpliterSet[spliterIndex[i]])
                && (iSpliterSet[spliterIndex[i]] != 0))
            {
                int k = 0;
                for (k = 0; k < SOLA_WAVE_NUM; k++)
                {
                    int existFlag = IsEventExist(&SolaAlgorithm->pAlgorithmInfo[k].EventsTable,
						pEventsTable->EventsInfo[pointIndex[i]].fEventsPosition);
                    if ((existFlag >= 0) && (existFlag != 1000))
                    {
                        int eventNum = SolaAlgorithm->pAlgorithmInfo[k].EventsTable.iEventsNumber;
                        SolaAlgorithm->pAlgorithmInfo[k].EventsTable.EventsInfo[eventNum - 1].fTotalLoss
							+= 3.0f * iSpliterSet[spliterIndex[i]] - pEventsTable->EventsInfo[pointIndex[i]].fLoss;
                        solaFloss[k][MAX_EVT_NUM - 1] = 3.0f * iSpliterSet[spliterIndex[i]]
							- pEventsTable->EventsInfo[pointIndex[i]].fLoss;
                        if (SolaAlgorithm->pAlgorithmInfo[k].EventsTable.EventsInfo[existFlag].fLoss
							< 3.0f * iSpliterSet[spliterIndex[i]])
                        {
                            solaFloss[k][existFlag] = 3.0f * iSpliterSet[spliterIndex[i]]
                                - SolaAlgorithm->pAlgorithmInfo[k].EventsTable.EventsInfo[existFlag].fLoss;
                            SolaAlgorithm->pAlgorithmInfo[k].EventsTable.EventsInfo[existFlag].fLoss
								= 3.0f * iSpliterSet[spliterIndex[i]];
                        }
                    }
                }
            }
        }

        if (pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle > 22)
        {
            pEventsTable->EventsInfo[pEventsTable->iEventsNumber - 1].iStyle &= 0x0000ffff;
        }
    }
}

// 针对不同波长、脉宽定义测试极限、弥补有效距离定位的精度不够问题
float MeasureLimitation(float distance,
	int level, 
	float referenceValue,
	float currentThresh)
{
    float thresh = currentThresh;

    // 1310极限测试距离78km
    // 1550极限测试距离120km
    if (((frontParament.wave == 0) && (distance > MEASURE_LIMITATION_1310_20US))
        || ((frontParament.wave == 1) && (distance > MEASURE_LIMITATION_1550_20US)))
    {
        thresh = Max(referenceValue, currentThresh);
    }

    if ((frontParament.wave == 1) && (distance > MEASURE_LIMITATION_1550_2US)
        && (frontParament.pulseWidth == ENUM_PULSE_2US))
    {
        if (level == 3)
        {
            thresh += 0.5f;
        }
    }
    return thresh;
}

// 对数转换
void IntTransferLog(unsigned short *idata, 
	int len, 
	float *flogdata)
{
    int  i = 0;

    //对数基准值
    int referValue = GetMeanUint16(idata + len, BLACK_LEVEL_LEN);
    for (i = 0; i < len; ++i)
    {
        int Temp = referValue - idata[i];

        if ((Temp > 0) && (Temp < 65536))
        {
            flogdata[i] = fLogSig[Temp];
        }
        else if ((Temp > -65536) && (Temp < 0))
        {
            flogdata[i] = -fLogSig[-Temp];
        }
        else
        {
            flogdata[i] = 0;
        }
    }
}

// 计算第一个事件损耗
void SolaGetFirstEventLoss(float *logSig,
	int sigLen, 
	int startOffset,
	EVENTS_INFO* event)
{
    float eventFrontValue = 0;
    float eventBackValue = GetMinFloat(logSig + startOffset, 25);
    float maxNoiseValue = GetMaxFloat(logSig + sigLen, BLACK_LEVEL_LEN);

    if (startOffset < 15)
    {
        memset(event, 0, sizeof(EVENTS_INFO));
        return;
    }

    if (eventBackValue < maxNoiseValue + 1)
    {
       // event->fLoss = -1;
    }
    else
    {
        eventFrontValue = GetMeanFloat(logSig + startOffset - 15, 10);

        if (eventBackValue > eventFrontValue)
        {
            event->fLoss = 0.0f;
        }
        else
        {
            event->fLoss = eventFrontValue - eventBackValue;
        }
    }

    float peakValue = GetMaxFloat(logSig + startOffset, 50);
    float fDelta = peakValue - eventFrontValue;
    frontParament.fRelay = frontParament.fBackScattering + 10.0f * log10f(48.0f);        //瑞利散射强度 20ns,8移相

    event->fReflect = frontParament.fRelay + 10 * log10f(powf(10, fDelta / 5.0f) - 1.0f);
    event->fEventsPosition = 0.0f;
    event->fContinueAttenuation = 0.0f;
    event->fDelta = fDelta;
    event->fEventReturnLoss = 0.0f;
    event->fEventsEndPosition = 3.2f;
    event->fTotalLoss = 0.0f;
    event->fAttenuation = 0.0f;
    event->iBegin = 0;
    event->iEnd = 10;
    event->iPulse = 3;
    event->iWaveSymbol = 0;
    event->iStyle = 2;
}

//计算回波损耗值 
float ReturnLoss(unsigned short *pData,
	int m,
	int n,
	float fRLCoef,
	float fSamRatio)
{
    //返回值
    float fReturnLoss;

    //临时变量
    float fEcho;
    int i, iTmp;
    float fSum = 0.0f;

    //计算回波损耗是利用原始数据进行计算的
    iTmp = abs(n - m);

    if (iTmp == 0)
    {
        return 0;
    }
    //防止出现m>n的BUG
    if (m > n)
    {
        for (i = 1; i <= iTmp; ++i)
        {
            fSum += fPowerSig[pData[n + i]] / fPowerSig[pData[n]];
        }
    }
    else
    {
        for (i = 1; i <= iTmp; ++i)
        {
            fSum += fPowerSig[pData[m + i]] / fPowerSig[pData[m]];
        }
    }

	//printf("----ReturnLoss--- fRLCoef %f, fSamRatio = %f, fSum = %f\n", fRLCoef, fSamRatio, fSum);

    fEcho = fRLCoef + 10.0f * log10f((float)(FPGA_CLOCK_FREQ_1NS / ADC_CLOCK_FREQ / fSamRatio));

    fReturnLoss = fEcho + 10.0f * log10f(fSum);
    if (fReturnLoss < fRLCoef)
    {
        fReturnLoss = fRLCoef;
    }

    fReturnLoss = fabsf(fReturnLoss);

    return fReturnLoss;
}

// 获取事件查找范围
void GetEventsSearchRange(ALGORITHM_INFO *pAlgorithmInfo)
{
    int i = 0;
    int End_position = 0;
    float MeanVarTemp[3] = { 0.0f, 0.0f, 0.0f };
    float End_len = 0.0f;//末端长度

#if 1
    // 预估最大光纤长度
	float endThr = Max(5.0f, pAlgorithmInfo->fMaxInputNoise - 1.0f);
    int MaxFiberLen = SearchEnd(pAlgorithmInfo->pSignalLog,
		pAlgorithmInfo->iSignalLength, endThr);
    printf("MaxLen=%f\n", MaxFiberLen * g_fLengthPerPoint);

    // 在保证信噪比可以的情况下，允许的衰减余量
    // 依赖于自动量程选择
    float PowerLossOffset = 0.0f;
    if (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_100NS)
    {
        PowerLossOffset = 2.0f;
    }
    else if (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_500NS)
    {
        PowerLossOffset = 3.0f;
    }
    else if (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_2US)
    {
        PowerLossOffset = 4.0f;
    }
    else
    {
        PowerLossOffset = 5.0f;
    }

    // 计算初始的功率值
    int iStart = Max((int)(30.0f / g_fLengthPerPoint),
		pAlgorithmInfo->iBlind + (int)(10.0f / g_fLengthPerPoint));
    for (i = 0; i < 3; ++i)
    {
        MeanVarTemp[i] = GetMeanFloat(pAlgorithmInfo->pSignalLog + iStart * (i + 1), 5);
    }
    float MeanVarStart = GetMinFloat(MeanVarTemp, 3);

    // 计算光纤末端值(预估)
    float MeanVarEnd = 0.0f;
    int Max_Wavelet_position = GetMaxIndex(g_pWaveletCoef[3] + 300,
		g_iExSignalLength - 1200) + 300;

    if (Max_Wavelet_position >= 2 * MaxFiberLen / 3)
    {
        MeanVarEnd = MeanVarStart - Max_Wavelet_position * g_fLengthPerPoint / 1000.0f
            * Normal_fAttenuation - PowerLossOffset;
    }
    else
    {
        MeanVarEnd = MeanVarStart - MaxFiberLen * g_fLengthPerPoint / 1000.0f
            * Normal_fAttenuation - PowerLossOffset + 2.0f;
    }
    MeanVarEnd = Max(2.0f, MeanVarEnd);
    printf("MeanVarEnd = %f, Max_Wavelet_position = %d\n", MeanVarEnd, Max_Wavelet_position);

    // 计算末端噪声值
    float fEndNoise = GetStdFloat(pAlgorithmInfo->pSignalLog
		+ pAlgorithmInfo->iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN - 100);
    fEndNoise = Max((MeanVarEnd + fEndNoise) / 3.0f * 2.0f, fEndNoise);
    if (pAlgorithmInfo->SetParameters.iwavelength == 1)
    {
        fEndNoise = Max(fEndNoise - 2.5f, 2.0f);
    }

    // 脉宽为5ns有效距离为500m
    if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_5NS)
    {
        //range = 600.0f;
        //return;
    }

    if (MaxFiberLen * g_fLengthPerPoint < 10000.0f)
    {
        if (MaxFiberLen * g_fLengthPerPoint < 1500.0f)//1.3km量程使用的是3k通道
        {
            fEndNoise = 3.5f + (pAlgorithmInfo->PulseWidth - 2) * 1.5f;
            if (pAlgorithmInfo->SetParameters.iwavelength == 1)
                fEndNoise = fEndNoise - 1.3f;
        }
        else if ((pAlgorithmInfo->PulseWidth >= ENUM_PULSE_20NS)
			&& (pAlgorithmInfo->PulseWidth <= ENUM_PULSE_200NS))
        {
            fEndNoise = 5.0f + (pAlgorithmInfo->PulseWidth - 3) * 1.5f;
            if (pAlgorithmInfo->SetParameters.iwavelength == 1)
                fEndNoise = Max(fEndNoise - 2.0f, 3.5f);
            fEndNoise = Min(fEndNoise, 9.0f);
            printf("--hello-----fEndNoise=%f\n", fEndNoise);
        }
    }
    else if (pAlgorithmInfo->PulseWidth >= ENUM_PULSE_2US)
        fEndNoise = Max(4.0f, Min(fEndNoise, 6.0f));
    else
        fEndNoise = Min(fEndNoise, 15.0f);

    if ((pAlgorithmInfo->PulseWidth > ENUM_PULSE_5NS)
		&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_2US)
		&& (MaxFiberLen * g_fLengthPerPoint > 3000.0f))
    {
        if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_10NS)
        {
            fEndNoise = 3.0f;
        }
        else
        {
            fEndNoise = 4.0f + Max(pAlgorithmInfo->PulseWidth - 3, 0) * 0.5f;
            fEndNoise = Max(4.0f, fEndNoise);
        }
    }

    if ((pAlgorithmInfo->PulseWidth > ENUM_PULSE_5NS)
		&& (pAlgorithmInfo->PulseWidth < ENUM_PULSE_2US)
		&& (MaxFiberLen * g_fLengthPerPoint < 3000.0f))
    {
        fEndNoise = Max(6.0f, fEndNoise);
    }
    if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_2US)
    {
        fEndNoise = Min(fEndNoise, 10.0f);
    }
	EndThrNoise = Max(pAlgorithmInfo->fMaxInputNoise - 2.0f, pAlgorithmInfo->fInputNoise);
	fEndNoise = Max(fEndNoise, EndThrNoise);
    EndThrNoise = fEndNoise;

    End_position = SearchEnd(pAlgorithmInfo->pSignalLog + 20,
		pAlgorithmInfo->iSignalLength - BLACK_LEVEL_LEN, fEndNoise);
    if (End_position == -1)
    {
        End_position = pAlgorithmInfo->iSignalLength - BLACK_LEVEL_LEN;
    }
    End_len = End_position * g_fLengthPerPoint;
    range = Max_Wavelet_position * g_fLengthPerPoint;

#endif
    GetFinalRange(pAlgorithmInfo, End_len, fEndNoise, Max_Wavelet_position);

	if (pAlgorithmInfo->PulseWidth == ENUM_PULSE_5NS)
	{
		range = Min(600.0f, range);
	}

}

// 获取最终的事件查找范围
void GetFinalRange(ALGORITHM_INFO *pAlgorithmInfo,
	float EndDist,
	float fEndNoise, 
	int Max_Wavelet_position)
{
    int i = 0;
    int time = 0;   //!< 查找次数
    int MaxPos[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int search_width = 0;
    int center = 0;
    int iBlind = pAlgorithmInfo->iBlind;
    float mean1 = 0.0f;
    float mean2 = 0.0f;
    int pos_temp1 = 0, pos_temp2 = 0;
    int End_position = (int)(EndDist / g_fLengthPerPoint);
	float firstrange = 0.0f;

	printf("End_position = %d\n", End_position);
    // 由于F216短距离拖尾严重,误差在50m范围内也认为是接近末端
    if ((Max_Wavelet_position * g_fLengthPerPoint > EndDist - 100.0f)
        && (pAlgorithmInfo->iSignalLength * g_fLengthPerPoint < 2000.0f)
        && (pAlgorithmInfo->PulseWidth > ENUM_PULSE_10NS))
    {
        int pointoffset = (int)(100.0f / g_fLengthPerPoint);
        Max_Wavelet_position = GetMaxIndex(g_pWaveletCoef[3]
			+ Min((int)(50.0f / g_fLengthPerPoint), End_position),
            Max(End_position - pointoffset - 300, 1))
            + Min((int)(50.0f / g_fLengthPerPoint), End_position);
    }
    else if (End_position <= Max_Wavelet_position)
    {
        Max_Wavelet_position = GetMaxIndex(g_pWaveletCoef[3]
			+ Min((int)(50.0f / g_fLengthPerPoint), End_position),
            Max(End_position - 300, 1)) + Min((int)(50.0f / g_fLengthPerPoint), End_position);
    }

	firstrange = range;
    range = Max_Wavelet_position * g_fLengthPerPoint;
    MaxPos[time++] = Max_Wavelet_position;
    int Flag = 0;                                                           // 短距离标志
    int step = 256;                                                         // 设置初始查找步长
    float DistPrecision = 3.0f * pAlgorithmInfo->iBlind * g_fLengthPerPoint;   // 查找精度

    // 由于短距离拖尾严重，故步长和搜寻精度要做不同程度的调整
    if (EndDist < 3000.0f)
    {
        Flag = 1;
        if (EndDist > 1200.0f)
        {
            DistPrecision = 200.0f;
            step = 256;
        }
        else if (EndDist >= 600.0f)
        {
            DistPrecision = 100.0f;
            step = (int)(50.0f / g_fLengthPerPoint);
        }
    }

    // 极短距离做特殊处理
    if (EndDist < 300.0f)
    {
        if ((Max_Wavelet_position > End_position)
			|| (EndDist - range > 100.0f))
        {
            Max_Wavelet_position
				= GetMaxIndex(g_pWaveletCoef[3] + 500, End_position - 500) + 500;
        }
        range = (Max_Wavelet_position + 64) * g_fLengthPerPoint;
    }
    else
    {
        int abortflag = 0;
        int LengthExWavelet = End_position;
        while (time < 10)
        {
            if (((EndDist - range) <= DistPrecision) && (EndDist > range))
            {
                range = Min((Max_Wavelet_position + 32) * g_fLengthPerPoint, EndDist);
                break;
            }
            else
            {
                if (range < EndDist)
                {
                    
                    int position0 = Min(Max_Wavelet_position + step, End_position);
                    int LengthWavelet = Max(End_position
						- Min(Max_Wavelet_position + step, End_position), 1);
                    
                    Max_Wavelet_position = GetMaxIndex(
						g_pWaveletCoef[3] + position0, LengthWavelet) + position0;
                    range = (Max_Wavelet_position + 64) * g_fLengthPerPoint;

                    // 对应原始信号的中心最大值
                    if (pAlgorithmInfo->iSignalLength * g_fLengthPerPoint < 1800.0f)
                    {
                        search_width = Max(2 * iBlind, 20);
                        center = Max(Max_Wavelet_position - search_width, 0) +
                            GetMaxIndex(pAlgorithmInfo->pSignalLog
								+ Max(Max_Wavelet_position - search_width, 0),
								Min(2 * search_width, pAlgorithmInfo->iSignalLength 
								- Max_Wavelet_position + search_width));
                        pos_temp1 = Max(center - 2 * search_width, 1);
                        mean1 = GetMeanFloat(pAlgorithmInfo->pSignalLog + pos_temp1, search_width / 2);
                        
                        pos_temp1 = Min(center + 2 * search_width, pAlgorithmInfo->iSignalLength);
                        pos_temp2 = Min(search_width, LengthExWavelet - Min(center
							+ 2 * search_width, pAlgorithmInfo->iSignalLength) - search_width);
                        mean2 = GetMeanFloat(pAlgorithmInfo->pSignalLog + pos_temp1, pos_temp2);
                       
                        if (((fabsf(pAlgorithmInfo->pSignalLog[center] - mean1) < 1.0f)
                            && (fabsf(pAlgorithmInfo->pSignalLog[center] - mean2) < 1.0f))
                            || (pAlgorithmInfo->pSignalLog[center] < mean1)
                            || (pAlgorithmInfo->pSignalLog[center] < mean2)
                            || (Max_Wavelet_position * g_fLengthPerPoint > EndDist))
                        {
                            break;
                        }
                        else
                        {
                            MaxPos[time++] = Max_Wavelet_position;
                            //time++;
                            if (range >= EndDist)
                            {
                                range = Min((Max_Wavelet_position + 32) * g_fLengthPerPoint, EndDist);
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (abs(Max_Wavelet_position - End_position)
							* g_fLengthPerPoint < iBlind * g_fLengthPerPoint)
                        {
                            break;
                        }

                        for (i = 0; i< 10; ++i)
                        {
                            if (abs(Max_Wavelet_position - MaxPos[i])
								* g_fLengthPerPoint < iBlind * g_fLengthPerPoint)
                            {
                                abortflag = 1;
                                break;
                            }
                        }
                        if (abortflag == 1)
                        {
                            break;
                        }

                        if (range >= EndDist)
                        {
                            range = Min((Max_Wavelet_position + 32) * g_fLengthPerPoint, EndDist);
                            break;
                        }

                        MaxPos[time++] = Max_Wavelet_position;
                    }
                }
                else
                    time++;
            }
        }
        int j;
        if (Flag == 1)
        {
            float MaxPosMean = 0.0f;
            for (i = 0; i < 10; ++i)
            {
                MaxPosMean = GetMeanFloat(
					pAlgorithmInfo->pSignalLog + Max(MaxPos[i] - 300, 1), 10);
                if ((MaxPosMean < fEndNoise) || (MaxPos[i] > End_position))
                {
                    MaxPos[i] = 0;
                }
            }
            if (frontParament.pulseWidth > 2)
            {
                for (i = 0; i < 10; ++i)
                {
                    for (j = 0; j < 10; ++j)
                    {
						if ((fabs(MaxPos[j] - MaxPos[i])
							< (int)(30.0f / g_fLengthPerPoint)) && (j != i))
						{
							MaxPos[j] = 0;
						}
                    }
                }
            }

            int PosTemp[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            for (i = 0; i < 10; ++i)
            {
                PosTemp[i] = abs(MaxPos[i] - End_position);
            }
            int PosIndex = GetMinIntIndex(PosTemp, 10);
            range = (MaxPos[PosIndex] + 128) * g_fLengthPerPoint;
        }
    }
	if ((firstrange / range > 2) && (range != 0) && (firstrange - range < 30000.0f))
	{
		range = Max(firstrange, Max_Wavelet_position * g_fLengthPerPoint);
	}
	printf("*******1*****range = %f, Max_Wavelet_position = %d\n", range, Max_Wavelet_position);
}

// 设置每个点代表的距离 
void SetLengthPerPoint(float fLengthPerPoint)
{
    if(fLengthPerPoint > 0.0f)
    {
        g_fLengthPerPoint = fLengthPerPoint;
    }
}

// 设置每个事件的测试波长
void SetEventsWave(ALGORITHM_INFO *pAlgorithmInfo) 
{
	int i = 0;
	int iWave = pAlgorithmInfo->SetParameters.iwavelength;
	EVENTS_TABLE *EventsTable = &(pAlgorithmInfo->EventsTable);
	for (i = 0; i < EventsTable->iEventsNumber; ++i)
	{
		EventsTable->EventsInfo[i].iWaveSymbol = iWave;
	}
}

// 设置外部参数
void SetfrontParament(EXTERN_INTERFACE *outsideFrontParament)
{
    frontParament.wave = outsideFrontParament->wave;
    frontParament.range = outsideFrontParament->range;
    frontParament.pulseWidth = outsideFrontParament->pulseWidth;
    frontParament.iSampleRating = outsideFrontParament->iSampleRating;
    frontParament.fLightSpeed = outsideFrontParament->fLightSpeed;
    frontParament.fRelay = outsideFrontParament->fRelay;
    frontParament.fLossThreshold = outsideFrontParament->fLossThreshold;
    frontParament.fBackScattering = outsideFrontParament->fBackScattering;
    frontParament.fRefractiveIndex = outsideFrontParament->fRefractiveIndex;
    frontParament.fReflectThreshold = outsideFrontParament->fReflectThreshold;
    frontParament.fFiberEndThreshold = outsideFrontParament->fFiberEndThreshold;
    frontParament.fExcessLength = outsideFrontParament->fExcessLength;
}

// 设置自动量程
void SetFiberLen(const float fiberLen)
{
    fFiberDist = fiberLen;
}

// 保存事件分析的参数（文件中）
void SaveEventAnalyseParam(ALGORITHM_INFO *pAlgorithmInfo)
{
    char str[512];
#ifdef _WIN32
    sprintf(str,"F:\\project\\data\\test\\parament_(wave=%d,pulse=%d).txt",pAlgorithmInfo->SetParameters.iwavelength,pAlgorithmInfo->PulseWidth);
#else
    sprintf(str, "/mnt/usb/parament_(wave=%d,pulse=%d).txt", pAlgorithmInfo->SetParameters.iwavelength, pAlgorithmInfo->PulseWidth);
#endif
    FILE *stream = fopen(str, "w+");
    if (stream)
    {
        fprintf(stream, "%d\r\n", pAlgorithmInfo->iFiberRange);
        fprintf(stream, "%d\r\n", pAlgorithmInfo->PulseWidth);
        fprintf(stream, "%d\r\n", pAlgorithmInfo->iSOLATest);
        fprintf(stream, "%d\r\n", pAlgorithmInfo->SetParameters.iwavelength);
        fprintf(stream, "%f\r\n", pAlgorithmInfo->SetParameters.fExcessLength);
        fprintf(stream, "%f\r\n", pAlgorithmInfo->SetParameters.fSampleRating);
        fprintf(stream, "%f\r\n", pAlgorithmInfo->SetParameters.fLossThreshold);
        fprintf(stream, "%f\r\n", pAlgorithmInfo->SetParameters.fBackScattering);
        fprintf(stream, "%f\r\n", pAlgorithmInfo->SetParameters.fRefractiveIndex);
        fprintf(stream, "%f\r\n", pAlgorithmInfo->SetParameters.fReflectThreshold);
        fprintf(stream, "%f\r\n", pAlgorithmInfo->SetParameters.fFiberEndThreshold);
        fprintf(stream, "%d\r\n", pAlgorithmInfo->iBlind);
        fprintf(stream, "%f\r\n", fFiberDist);
        fprintf(stream, "%d\r\n", pAlgorithmInfo->spliterRatioGrade1);
        fprintf(stream, "%d\r\n", pAlgorithmInfo->spliterRatioGrade2);
        fprintf(stream, "%d\r\n", pAlgorithmInfo->spliterRatioGrade3);

        fclose(stream);
    }
}

// 打印小波系数（文件中）
void PrintWaveLetCoef(float *sig,
	int sigLen,
	int num, 
	int pulse)
{
    int i;
    char str[512];
#ifdef _WIN32
    sprintf(str, "F:\\project\\data\\test\\coef[%d](pulse=%d).txt", num, pulse);
#else
    sprintf(str, "/mnt/usb/coef[%d](pulse=%d).txt", num, pulse);
#endif

    FILE *stream = fopen(str, "w+");
    if (stream)
    {
        for (i = 0; i < sigLen; ++i)
        {
            fprintf(stream, "%f\r\n", sig[i]);
        }
        fclose(stream);
    }
}

// 打印小波阈值（文件中）
void PrintWaveLetThresh(float *sig,
	int sigLen,
	int num, 
	int pulse)
{
    int i;
    char str[512];
#ifdef _WIN32
    sprintf(str, "F:\\project\\data\\test\\waveThresh[%d](pulse=%d).txt", num, pulse);
#else
    sprintf(str, "/mnt/usb/waveThresh[%d](pulse=%d).txt", num, pulse);
#endif

    FILE *stream = fopen(str, "w+");
    if (stream)
    {
        for (i = 0; i < sigLen; ++i)
        {
            fprintf(stream, "%f\r\n", sig[i]);
        }
        fclose(stream);
    }
}

// 打印小波系数的std
void PrintWaveLetStd(float *sig, int sigLen, int pulse)
{
    int i;
    char str[512];
    // sprintf(str, "/mnt/usb/std_pulse_%d.txt", pulse);
#ifdef _WIN32
	sprintf(str, "F:\\project\\data\\test\\data_%d.txt", pulse);
#else
	sprintf(str, "/mnt/usb/data_%d.txt", pulse);
#endif

    FILE *stream = fopen(str, "w+");
    if (stream)
    {
        for (i = 0; i < sigLen; ++i)
        {
            fprintf(stream, "%f\r\n", sig[i]);
        }
        fclose(stream);
    }
}

// 打印波形（文件中）
void PrintAlgorithmInfo(ALGORITHM_INFO *pAlgorithmInfo, int pulse)
{
    int i;
    char str[512];
#ifdef _WIN32
	sprintf(str, "F:\\project\\data\\test\\sigcombineData(wave=%d,pulse=%d).txt", pAlgorithmInfo->SetParameters.iwavelength, pulse);
#else
	sprintf(str, "/mnt/usb/sigcombineData(wave=%d,pulse=%d).txt", pAlgorithmInfo->SetParameters.iwavelength, pulse);
#endif

    FILE *stream = fopen(str, "w+");

    if (stream)
    {
        for (i = 0; i < pAlgorithmInfo->iSignalLength; ++i)
        {
            fprintf(stream, "%f\r\n", pAlgorithmInfo->pSignalLog[i]);
        }
        fclose(stream);
    }
}

// 打印事件信息（文件中）
void PrintEventInfo(FILE* stream, EVENTS_TABLE* events)
{
    fprintf(stream, "events[%p]:\n(\n", events);
    fprintf(stream, "  events->iEventsNumber = %d\n", events->iEventsNumber);
    int i;
    for (i = 0; i < events->iEventsNumber; ++i)
    {
        EVENTS_INFO* event = &events->EventsInfo[i];
        fprintf(stream, "  event[%d]:\n", i);
        fprintf(stream, "    event.iBegin = %d\n", event->iBegin);
        fprintf(stream, "    event.iEnd = %d\n", event->iEnd);
        fprintf(stream, "    event.iStyle = %d\n", event->iStyle);
        fprintf(stream, "    event.iPulse = %d\n", event->iPulse);
        fprintf(stream, "    event.fLoss = %f\n", event->fLoss);
        fprintf(stream, "    event.fDelta = %f\n", event->fDelta);
        fprintf(stream, "    event.fReflect = %f\n", event->fReflect);
        fprintf(stream, "    event.iContinueLength = %d\n", event->iContinueLength);
        fprintf(stream, "    event.fAttenuation = %f\n", event->fAttenuation);
        fprintf(stream, "    event.fContinueAttenuation = %f\n", event->fContinueAttenuation);
        fprintf(stream, "    event.fTotalLoss = %f\n", event->fTotalLoss);
        fprintf(stream, "    event.fEventsPosition = %f\n", event->fEventsPosition);
        fprintf(stream, "    event.fEventsEndPosition = %f\n", event->fEventsEndPosition);
        fprintf(stream, "    event.fEventReturnLoss = %f\n", event->fEventReturnLoss);
        fprintf(stream, "    event.iWaveSymbol = %d\n\n", event->iWaveSymbol);
    }
    fprintf(stream, ")\n");
}

// 打印事件区间信息（文件中）
void PrintEventRegionInfo(FILE* stream, EVENTS_REGION_TABLE* eventRegion)
{
    fprintf(stream, "eventRegion[%p]:\n(\n", eventRegion);
    fprintf(stream, "  events->iEventsNumber = %d\n", eventRegion->iEventsNumber);
    int i;
    for (i = 0; i < eventRegion->iEventsNumber; ++i)
    {
        EVENTS_REGION_INFO* event = &eventRegion->EventsRegionInfo[i];
        fprintf(stream, "  event[%d]:\n", i);
        fprintf(stream, "    event.iBegin = %d\n", event->iBegin);
        fprintf(stream, "    event.iEnd = %d\n", event->iEnd);
        fprintf(stream, "    event.iStyle = %d\n", event->iStyle);
        fprintf(stream, "    event.iWindow = %d\n", event->iWindow);
        fprintf(stream, "    event.iMaxIndex= %d\n", event->iMaxIndex);
        fprintf(stream, "    event.iMinIndex = %d\n", event->iMinIndex);
    }
    fprintf(stream, ")\n");
}

// 打印FrontEventsTable（界面）
void FrontEventsTable_printf(EVENTS_REGION_TABLE *EventsTable)
{
	int i = 0;
	for (; i < EventsTable->iEventsNumber; ++i)
	{
		printf("g_pEventsRegionTable[%d].iBegin=%d\n", i, EventsTable->EventsRegionInfo[i].iBegin);
		printf("g_pEventsRegionTable[%d].iEnd=%d\n", i, EventsTable->EventsRegionInfo[i].iEnd);
		printf("g_pEventsRegionTable[%d].iStyle=%d\n", i, EventsTable->EventsRegionInfo[i].iStyle);
		printf("g_pEventsRegionTable[%d].iWindow=%d\n", i, EventsTable->EventsRegionInfo[i].iWindow);
		printf("\n");
	}
}

// 打印EventsTable（界面）
void FiberEndEventsTable(EVENTS_TABLE *EventsTable)
{
	int i = 0;
	for (; i < EventsTable->iEventsNumber; ++i)
	{
		printf("pEventsTable->EventsInfo[%d].iStyle=%d\n", i, EventsTable->EventsInfo[i].iStyle);
		printf("pEventsTable->EventsInfo[%d].iBegin=%d\n", i, EventsTable->EventsInfo[i].iBegin);
		printf("pEventsTable->EventsInfo[%d].iEnd=%d\n", i, EventsTable->EventsInfo[i].iEnd);
		printf("pEventsTable->EventsInfo[%d].iPulse=%d\n", i, EventsTable->EventsInfo[i].iPulse);
		printf("pEventsTable->EventsInfo[%d].fLoss=%f\n", i, EventsTable->EventsInfo[i].fLoss);
		printf("pEventsTable->EventsInfo[%d].fReflect=%f\n", i, EventsTable->EventsInfo[i].fReflect);
		printf("pEventsTable->EventsInfo[%d].fAttenuation=%f\n", i, EventsTable->EventsInfo[i].fAttenuation);
		printf("pEventsTable->EventsInfo[%d].fDelta=%f\n", i, EventsTable->EventsInfo[i].fDelta);
		printf("pEventsTable->EventsInfo[%d].fEventsPosition=%f\n", i, EventsTable->EventsInfo[i].fEventsPosition);
		printf("pEventsTable->EventsInfo[%d].fEventsPositionEnd=%f\n", i, EventsTable->EventsInfo[i].fEventsEndPosition);
		printf("\n");
	}
}


