/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_sola.c
* 摘    要：  光眼功能
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "app_sola.h"
#include "FList.h"
#include "app_qep.h"
#include "app_log.h"
#include "app_frmsolamessager.h"
#include "wnd_frmsolaconfig.h"
#include "app_algorithm_support.h"
#include "app_otdr_algorithm_parameters.h"
#include "app_sola_algorithm_parameters.h"
#include "app_matfile.h"

extern SOLA_MESSAGER* pSolaMessager ;
static SOLA_DATA* solaCurveData = NULL;
static int s_waveNum = 0;
extern PUSER_SETTINGS pUser_Settings;
extern POTDR_TOP_SETTINGS pOtdrTopSettings;

//保存sola波形数据
//#define SAVE_CURVE_DATA           
/**
 * 状态机信号以及事件定义
 */
enum _sola_sigs
{
    START_SIG = Q_USER_SIG,     //启动测量信号
    STOP_SIG,                   //停止测量信号
    WAVE_SIG,                   //波长变化信号
    PULSE_SIG,					//脉宽变化信号
    NEW_TASK_SIG,               //新任务信号
    ANALYZE_SIG,                //分析事件数据信号
    EXIT_SIG,                   //退出信号
    FIRST_EVENT_SIG				//起始事件信号
};

/**
 * sola事件的优先级定义，一般来说控制事件的优先级比较高
 */
enum _sloa_event_priority
{
    NORMAL_PRIORITY = 0,        //普通优先级
    HIGH_PRIORITY,              //高优先级
};

/**
 * Sola的任务
 */
typedef struct _sola_task
{
    int iWave;           //波长
    int iPulse;          //脉宽
    int iRange;          //量程

    int iDaqNum;         //采集任务个数
  	int iBlind;          //盲区长度，单位:点数
	int	iDataCount;	     //有效数据点数
	int iDisplayCount;   //数据显示点数 = iDataCount
	int	iOffset;         //无效数据偏移点数
	int	iSmpReg;         //采样寄存器设置
	float autoRange;	 //自动量程
	float fSmpRatio;     //采样比率
	float fSmpInterval;  //采样间隔
} SolaTask;

void SolaTask_Print(SolaTask* me, FILE* out)
{
    static const char* wave[] = {"1310nm", "1550nm", "1625nm"};
    #ifdef MINI2
    static const char* pulse[] = {"auto", "5ns", "10ns", "20ns", "30ns", "50ns", "100ns", "200ns", "500ns", "1us", "2us", "10us", "20us"};
    #else
    static const char* pulse[] = {"auto", "5ns", "10ns", "20ns", "50ns", "100ns", "200ns", "500ns", "1us", "2us", "10us", "20us"};
    #endif
    static const char* range[] = {"auto", "1300m", "2.5km", "5km", "10km", "20km", "40km", "80km", "120km", "160km", "360km"};

    fprintf(out, "SolaTask@[%p]:\n(\n", me);
    fprintf(out, "  Wave = %s\n", wave[me->iWave]);
    fprintf(out, "  Pulse = %s\n", pulse[me->iPulse]);
    fprintf(out, "  Range = %s\n", range[me->iRange]);
    fprintf(out, "  DaqNum = %d\n", me->iDaqNum);
    fprintf(out, "  Blind = %d\n", me->iBlind);
    fprintf(out, "  DataCount = %d\n", me->iDataCount);
    fprintf(out, "  DisplayCount = %d\n", me->iDisplayCount);
    fprintf(out, "  Offset = %d\n", me->iOffset);
    fprintf(out, "  SmpReg = %x\n", me->iSmpReg);
    fprintf(out, "  SmpRatio = %f\n", me->fSmpRatio);
    fprintf(out, "  SmpInterval = %fm\n", me->fSmpInterval);
    fprintf(out, ")\n");
}


/**
 * 任务事件，内部包含了每个任务不同的参数
 * 用于控制整个sola的采集过程
 */
typedef struct _task_event
{
    QEvent super;
    int iWave;          //波长
    int iPulse;         //脉宽
    int iRange;         //量程
} TaskEvent;


/**
 * 定义用于SOLA采集上下文结构
 * 用于对SOLA整个采集过程参数的配置以及结果数据的存放
 */
typedef struct _sola_context
{
    unsigned int iCurrWave;     //当前所用的波长

    COMBINE_INFO Combine;       //当前的组合对象内部有动态分配的组合缓冲区
    SOLA_INFO SolaAlgorithm;    //算法对象
    SolaTask Task;              //当前任务的设置
    SolaEvents Events;          //事件表

    int iOpmFd;                 //光模块设备描述符
    POPMSET pOpmSet;            //光模块句柄

    UINT16 pOrigData[MAX_DATA]; //采集缓冲区
} SolaContext;

/**
 * SOLA上下文的静态函数声明
 */
static int SolaContext_AllocBuff(SolaContext* me);
static void SolaContext_FreeBuff(SolaContext* me);
static int SolaContext_Init(SolaContext* me);
static void SolaData_Init();

static void OpenMatFile(OPM_WAVE wave)
{
	PMATFILE mat = GetMatFile();
    mat->setTestType(mat, MAT_STRUCT_SOLA);
	mat->addMatFile(mat, EnumToWave(wave));
}
static void CloseMatFile()
{
	PMATFILE mat = GetMatFile();
	mat->closeMatFile();
}

static void SaveMatData(SolaTask *pTskSet, DAQ_SETTING *pDaqSet)
{
	PMATFILE mat = GetMatFile();
	float fRefractiveIndex = pSolaMessager->lineDefine.groupIndex1550;
	float fBackScattering = pSolaMessager->lineDefine.backScatter1550;

	if (mat->iIsSave)
	{
		
		mat->setWave(mat, EnumToWave(pTskSet->iWave));
		mat->setPulse(mat, EnumToPulse(pTskSet->iPulse));
		mat->setRange(mat, (int)Range2Dist(pTskSet->iRange));
		mat->setBlind(mat, pTskSet->iBlind);
		mat->setSmpRation(mat, pTskSet->fSmpRatio);
		mat->setPoint2Distance(mat, pTskSet->fSmpInterval);
		mat->setDisplayLen(mat, pDaqSet->iDaqCount+pDaqSet->iNoiseCnt);
  
		if (0 == pTskSet->iWave)
		{
			fRefractiveIndex = 1.4677f * fRefractiveIndex;
			fRefractiveIndex = fRefractiveIndex / 1.468325f;
			fBackScattering += -79.44f;
			fBackScattering -= -81.87f;
		}
		else if(2 == pTskSet->iWave)
		{
			fRefractiveIndex = 1.468734f * fRefractiveIndex;
			fRefractiveIndex = fRefractiveIndex / 1.468325f;
			fBackScattering += -82.58f;
			fBackScattering -= -81.87f;
		}
		else
		{}
	
		mat->setRefractiveIndex(mat, fRefractiveIndex);
		mat->setBackScatter(mat, fBackScattering);
		mat->setSpliceLossThr(mat, pOtdrTopSettings->pUser_Setting->sAnalysisSetting.fSpliceLossThr);
		mat->setReturnLossThr(mat, pOtdrTopSettings->pUser_Setting->sAnalysisSetting.fReturnLossThr);
		mat->setEndLossThr(mat, pOtdrTopSettings->pUser_Setting->sAnalysisSetting.fEndLossThr);

		mat->setApd(mat, pOtdrTopSettings->pOpmSet->RegIndex[pDaqSet->iPulseKind]->pApdIndex[pDaqSet->iAtteDbClass]);
		mat->setVdd(mat, pOtdrTopSettings->pOpmSet->RegIndex[pDaqSet->iPulseKind]->pVddIndex[pDaqSet->iAtteDbClass]);
		mat->setChannel(mat, pOtdrTopSettings->pOpmSet->RegIndex[pDaqSet->iPulseKind]->pAmpIndex[pDaqSet->iAtteDbClass]);
		
		mat->setHighResolution(mat, 1);
		mat->setData(mat, pDaqSet->pOrigData, pDaqSet->iDaqCount+pDaqSet->iNoiseCnt);

		mat->setOffset(mat, pDaqSet->iOffset);
		
		mat->setAtteDbClass(mat, pDaqSet->iAtteDbClass);
		mat->setTestType(mat, MAT_STRUCT_SOLA);

		mat->addMatStruct(mat);
        LOG(LOG_INFO, "-----save sola matlab file---\n");
    }
}


//事件缓冲最大事件个数
#define EVENT_BUFF_MAX 200

/**
 * 定义用于SOLA采集数据的结构
 * 用于对SOLA整个采集过程的控制以及数据的处理
 */
typedef struct _sola
{    
    QFsm super;                     //状态机
    GUITHREAD Thread;               //执行线程
    GUIMUTEX Mutex;                 //互斥量,用于内部共享资源的保护

    int iExit;                      //线程退出标志位
    int iRunning;                   //执行标志位
    int iError;                     //错误码
    
    int iCheckLight;                //是否执行有光检查
    int iCheckEndFace;              //是否执行末端检查
    
    float fProgress;                //任务当前的进度
    float fProgressStep;            //每执行一个任务，进度步进值
   
    FList Events;                   //任务事件队列
    FList Messages;                 //对外的消息队列

    SolaContext Context;            //上下文
    SolaSettings Settings;          //配置
    TaskEvent TaskEventBuffer[EVENT_BUFF_MAX];     //任务事件缓冲，免去动态生成事件
} Sola;

/**
 * SOLA的静态函数声明
 */
static int Sola_Init(Sola* me);
static int Sola_Finish(Sola* me);
static int Sola_Start(Sola* me);
static int Sola_Stop(Sola* me);
static int Sola_GetSolaEvents(Sola* me, SolaEvents* dest);
static void Sola_SetRunning(Sola* me, int iRunning);
static int Sola_IsRunning(Sola* me);
static int Sola_PostEvent(Sola* me, const QEvent* event, int iPriority);
static QEvent* Sola_GetEvent(Sola* me);
static void Sola_ClearEvents(Sola* me);
static int Sola_PostMessage(Sola* me, unsigned short type, unsigned short value);
static int Sola_GetMessage(Sola* me, SolaMessage* message);
static void Sola_ClearMessages(Sola* me);
static void Sola_ProgressChanaged(Sola* me);
static void* Sola_Thread(void* arg);

//SOLA执行过程的所有状态声明
static QState Sola_IdleState(Sola* me, QEvent* e);
static QState Sola_AquireState(Sola* me, QEvent* e);

static void PromptLaserIsOn(int isStart);



/**
 * 分配SOLA上下文(分析过程中所用)的缓冲区
 * 该内存为常驻内存，目前的大小约为: 5.2MB
 */
static int SolaContext_AllocBuff(SolaContext* me)
{
    int i = 0;

    me->Combine.pCombineData = (float*)calloc(MAX_DATA, sizeof(float));
    me->Combine.pOriginalData = (UINT16*)calloc(MAX_DATA, sizeof(UINT16));
    
    for (i = 0; i < MAX_DAQ_NUM; ++i)
    {
        me->Combine.combine_para[i].pData = (float *)calloc(MAX_DATA, sizeof(float));
        me->Combine.combine_para[i].pOriData = (UINT32 *)calloc(MAX_DATA, sizeof(UINT32));
        me->Combine.combine_para[i].pLogData = (float *)calloc(MAX_DATA, sizeof(float));
    }

    for (i = 0; i < SOLA_WAVE_NUM; ++i)
    {
        me->SolaAlgorithm.pAlgorithmInfo[i].pSignalLog = (float*)calloc(MAX_DATA, sizeof(float));
    }
    
    return 0;
}

/**
 * 释放SOLA上下文(采集过程中所用)的缓冲区
 * 该内存为常驻内存，目前的大小约为: 5.2MB
 */
static void SolaContext_FreeBuff(SolaContext* me)
{
    int i = 0;

    free(me->Combine.pCombineData);
    me->Combine.pCombineData = NULL;
    free(me->Combine.pOriginalData);
    me->Combine.pOriginalData = NULL;
    
    for (i = 0; i < MAX_DAQ_NUM; ++i)
    {
        free(me->Combine.combine_para[i].pData);
        me->Combine.combine_para[i].pData = NULL;
        free(me->Combine.combine_para[i].pOriData);
        me->Combine.combine_para[i].pOriData = NULL;
        free(me->Combine.combine_para[i].pLogData);
        me->Combine.combine_para[i].pLogData = NULL;
    }

    for (i = 0; i < SOLA_WAVE_NUM; ++i)
    {
        free(me->SolaAlgorithm.pAlgorithmInfo[i].pSignalLog);
        me->SolaAlgorithm.pAlgorithmInfo[i].pSignalLog = NULL;
    }
}

static void SolaData_Init()
{
    if(solaCurveData == NULL)
    {
        solaCurveData = (SOLA_DATA*)malloc(SOLA_WAVE_NUM * sizeof(SOLA_DATA));
    }

    int i,j;
    for (i = 0; i < SOLA_WAVE_NUM; i++)
    {
        SOLA_DATA* solaData = solaCurveData + i;
        solaData->count = MAX_DATA;
        solaData->data = (short*)malloc(solaData->count * sizeof(short));
        solaData->onePoint = 0.0f;
        solaData->wave = WAVELEN_1310;
        for (j = 0; j < solaData->count; j++)
        {
            *(solaData->data + j) = 0;
        }
    }
}


/**
 * 初始化采集上下文
 */
static int SolaContext_Init(SolaContext* me)
{
	me->pOpmSet = pOtdrTopSettings->pOpmSet;
    me->iOpmFd = GetGlb_DevFd()->iOpticDev;
    me->iCurrWave = -1;

    //清除上一次的事件列表
    memset(&me->Events, 0, sizeof(SolaEvents));

	int i;
	float* dynamicMemory[SOLA_WAVE_NUM];
	for (i = 0; i < SOLA_WAVE_NUM; ++i)
	{
		dynamicMemory[i] = me->SolaAlgorithm.pAlgorithmInfo[i].pSignalLog;
	}
	memset(&me->SolaAlgorithm, 0, sizeof(SOLA_INFO));
	for (i = 0; i < SOLA_WAVE_NUM; ++i)
	{
		me->SolaAlgorithm.pAlgorithmInfo[i].pSignalLog = dynamicMemory[i];
		me->SolaAlgorithm.pAlgorithmInfo[i].spliterRatioGrade1 = pSolaMessager->lineDefine.splittingRatioLevel1;
		me->SolaAlgorithm.pAlgorithmInfo[i].spliterRatioGrade2 = pSolaMessager->lineDefine.splittingRatioLevel2;
		me->SolaAlgorithm.pAlgorithmInfo[i].spliterRatioGrade3 = pSolaMessager->lineDefine.splittingRatioLevel3;
	}
    SolaData_Destroy();
    //初始化sola数据
    SolaData_Init();
    //初始化硬件
    return BoardWorkInit(me->pOpmSet, 0);    
}

/**
 * 设置波长
 */
static void SolaContext_SetWave(SolaContext* me, int iWave)
{
    if (iWave != me->iCurrWave)
    {
        me->iCurrWave = iWave;
        Opm_SetWave(me->iOpmFd, me->pOpmSet, iWave);
        //Opm_EnableRelay(me->iOpmFd, me->pOpmSet);
	    MsecSleep(300);
        //Opm_DisableRelay(me->iOpmFd, me->pOpmSet);
    }
}



/**
 * 初始化SOLA对象，构造所需的内部对象并且创建sola的执行线程
 */
static int Sola_Init(Sola* me)
{
    //初始化状态为空闲状态
    QFsm_ctor((QFsm*)me, (QStateHandler)Sola_IdleState);

    //初始化成员变量
    me->iExit = 0;
    me->fProgress = 0.0;
    me->iRunning = 0;
    me->iError = SOLA_NO_ERR;
    
    me->iCheckEndFace = 1;
    me->iCheckLight = 1;

    //初始化内部对象
    SolaContext_AllocBuff(&me->Context);
    FList_ctor(&me->Events);
    FList_ctor(&me->Messages);
    InitMutex(&me->Mutex, NULL);

    //创建执行线程
    return ThreadCreate(&me->Thread, NULL, Sola_Thread, me);
}

/**
 * 结束SOLA对象的使用，释放内部已构造的对象，并且销毁sola的执行线程进行资源回收
 */
static int Sola_Finish(Sola* me)
{
    //退出线程事件
    static const QEvent event = {.sig = EXIT_SIG, .dynamic = 0};
    
    //此处使用高优先级post事件
    Sola_PostEvent(me, &event, HIGH_PRIORITY);

    //回收执行线程
    ThreadJoin(me->Thread, NULL);

    //释放内部对象
    FList_dtor(&me->Events);
    FList_dtor(&me->Messages);
    SolaContext_FreeBuff(&me->Context);
    
    return 0;
}

/**
 * 启动Sola进行采集
 */
static int Sola_Start(Sola* me)
{
    //启动事件
    static const QEvent event = {.sig = START_SIG, .dynamic = 0};

    //此处使用高优先级post事件
    return Sola_PostEvent(me, &event, HIGH_PRIORITY);
}

/**
 * 停止Sola的采集过程
 */
static int Sola_Stop(Sola* me)
{
    //停止事件
    static const QEvent event = {.sig = STOP_SIG, .dynamic = 0};;
    
    //此处使用高优先级post事件
    return Sola_PostEvent(me, &event, HIGH_PRIORITY);
}

/**
 * post事件，最后一个参数指定该事件的优先级;
 * 目前只支持两种优先级: NORMAL_PRIORITY | HIGH_PRIORITY
 * SOLA的执行线程总是从队列的头部读取事件，基于此种方式，实现两种优先级，高优先级的事件总会从头部插入
 * BUG: 同是未被处理的高优先级事件，后来的会被先处理，但在F214中这种策略是完全足够的，不可能
 * 同时出现两个高优先级事件未被处理 
 */
static int Sola_PostEvent(Sola* me, const QEvent* event, int iPriority)
{
    int ret = 0;
    MutexLock(&me->Mutex);
    if (iPriority == HIGH_PRIORITY)
    {
        ret = FList_insertAtIndex(&me->Events, 0, event);
    }
    else
    {
        ret = FList_pushBack(&me->Events, event);
    }
    MutexUnlock(&me->Mutex);
    return ret;
}

/**
 * 获取事件队列中的事件，无事件则返回值为空
 */
static QEvent* Sola_GetEvent(Sola* me)
{
    QEvent* event = NULL;
    MutexLock(&me->Mutex);
    event = (QEvent*)FList_takeAtIndex(&me->Events, 0);
    MutexUnlock(&me->Mutex);
    return event;
}

/**
 * 清除事件队列中的所有事件
 * 调用时机: 当正在执行任务的过程中被强制停止，会清空所有未被处理的任务事件，并切换到空闲状态
 */
static void Sola_ClearEvents(Sola* me)
{
    MutexLock(&me->Mutex);
    while (!FList_isEmpty(&me->Events))
    {
        QEvent* e = (QEvent*)FList_takeAtIndex(&me->Events, 0);
        if (e && e->dynamic)
        {
            free(e);
        }
    }
    MutexUnlock(&me->Mutex);
}

/**
 * 向外post事件
 */
typedef union message_helper
{
    SolaMessage Message;
    int Space32Bit;
} MessageHelper;

static int Sola_PostMessage(Sola* me, unsigned short type, unsigned short value)
{
    MessageHelper helper;
    
    helper.Message.Type = type;
    helper.Message.Value = value;
    
    MutexLock(&me->Mutex);
    int ret = FList_pushBack(&me->Messages, (void*)(helper.Space32Bit));
    MutexUnlock(&me->Mutex);

    return ret;
}

/**
 * 读取Sola内部产生的消息，用于标识当前sola的状态进度等
 */
static int Sola_GetMessage(Sola* me, SolaMessage* message)
{
    int ret = -1;
    
    if (!message)
    {
        return ret;
    }

    MessageHelper helper;
    
    MutexLock(&me->Mutex);
    if (!FList_isEmpty(&me->Messages))
    {
        helper.Space32Bit = (int)FList_takeAtIndex(&me->Messages, 0);
        ret = 0;
    }
    MutexUnlock(&me->Mutex);

    *message = helper.Message;

    return ret;
}

/**
 * 处理进度
 */
static void Sola_ProgressChanaged(Sola* me)
{
    Sola_PostMessage(me, SOLA_PROGRESS_CHANGED, (unsigned short)(me->fProgress + 0.5f));
}

/**
 * 清空内部消息
 */
static void Sola_ClearMessages(Sola* me)
{
    MutexLock(&me->Mutex);
    FList_clear(&me->Messages);
    MutexUnlock(&me->Mutex);
}

/**
 * 得到sola最终分析得到的事件列表
 */
static int Sola_GetSolaEvents(Sola* me, SolaEvents* dest)
{
    MutexLock(&me->Mutex);
    memcpy(dest, &me->Context.Events, sizeof(SolaEvents));
    MutexUnlock(&me->Mutex);
    return 0;
}

static void Sola_SetRunning(Sola* me, int iRunning)
{
    MutexLock(&me->Mutex);
    me->iRunning = iRunning;
    MutexUnlock(&me->Mutex);
}

static int Sola_IsRunning(Sola* me)
{
    int ret = 0;
    MutexLock(&me->Mutex);
    ret = me->iRunning;
    MutexUnlock(&me->Mutex);
    return ret;
}

static const char* Sola_GetErrorString(int code)
{
    switch (code)
    {
    case SOLA_NO_ERR:
        return "No error";
    case SOLA_BOARD_ERR:
        return "Board initialization error";
    case SOLA_CHECK_LIGHT_ERR:
        return "Check light error";
    case SOLA_CHECK_ENDFACE_ERR:
        return "Check end face error";
    default:
        break;
    }
    return "Undefined error";
}


/* 
 *重置任务参数
*/
static void AdjustMeasureParament(TaskEvent* task, float autoFiberLen)
{
	// 无分路器情况使用3个脉宽进行测试
	int spliters = pSolaMessager->lineDefine.splittingRatioLevel1
				 + pSolaMessager->lineDefine.splittingRatioLevel2
				 + pSolaMessager->lineDefine.splittingRatioLevel3;
    LOG(LOG_INFO, "autoFiberLen = %f, spliters = %d, iPulse = %d, iRange = %d\n",
        autoFiberLen, spliters, task->iPulse, task->iRange);
    if (!spliters)
	{
		if (task->iPulse == ENUM_PULSE_10NS)
		{
			//task->iRange = ENUM_FIBER_RANGE_1300M;
			task->iRange = ENUM_FIBER_RANGE_1KM;
		}
		
		if ((task->iPulse == ENUM_PULSE_100NS) || (task->iPulse == ENUM_PULSE_200NS))
		{
			task->iRange = GetFiberRange(autoFiberLen / 1000.0f);
			task->iPulse = GetPulseTime(task->iWave, 0, task->iRange, autoFiberLen / 1000.0f);
			/*if(task->iRange >= ENUM_FIBER_RANGE_80KM)*/
			if (task->iRange >= ENUM_FIBER_RANGE_100KM)
			{
// 				task->iRange = Min(task->iRange + 1, ENUM_FIBER_RANGE_160KM);
				task->iRange = Min(task->iRange + 1, ENUM_FIBER_RANGE_200KM);
				task->iPulse = Min(GetPulseTime(task->iWave, 0,
					task->iRange, autoFiberLen / 1000.0f) + 1, ENUM_PULSE_20US);
			}
		}
        LOG(LOG_INFO, "No spliters,iRange = %d, iPulse = %d\n", task->iRange, task->iPulse);
    }
	// 有分路器的情况
	else
	{
		//10NS脉宽
		if (task->iPulse == ENUM_PULSE_10NS)
		{
			/*task->iRange = ENUM_FIBER_RANGE_2500M;*/
			task->iRange = ENUM_FIBER_RANGE_2KM;
		}
		//20NS脉宽
		if (task->iPulse == ENUM_PULSE_20NS)
		{
			task->iRange = ENUM_FIBER_RANGE_5KM;
		}

		//200NS的进行特殊处理
	    if (task->iPulse == ENUM_PULSE_200NS) 
	    {
	    	task->iRange = GetFiberRange(autoFiberLen / 1000.0f);
	    	if (task->iRange < ENUM_FIBER_RANGE_10KM)
	        task->iRange = ENUM_FIBER_RANGE_10KM;
	    }

		//1US的进行特殊处理
	    /*if ((task->iRange < ENUM_FIBER_RANGE_40KM)*/ 
		if ((task->iRange < ENUM_FIBER_RANGE_50KM)
	        &&(task->iPulse >= ENUM_PULSE_1US))
	    {
	        /*task->iRange = ENUM_FIBER_RANGE_40KM;*/
			task->iRange = ENUM_FIBER_RANGE_50KM;
	    }
		
		//10US的进行特殊处理
	    /*if ((task->iRange < ENUM_FIBER_RANGE_160KM)*/ 
		if ((task->iRange < ENUM_FIBER_RANGE_200KM)
	        &&(task->iPulse >= ENUM_PULSE_10US))
	    {
	        /*task->iRange = ENUM_FIBER_RANGE_160KM;*/
			task->iRange = ENUM_FIBER_RANGE_200KM;
	    }

		// 短距离参数调整
		if (autoFiberLen < 8000.0f)
		{
			if ((task->iPulse == ENUM_PULSE_10US) || (task->iPulse == ENUM_PULSE_20US))
			{
				/*task->iRange = ENUM_FIBER_RANGE_40KM;*/
				task->iRange = ENUM_FIBER_RANGE_50KM;
			}
			if (spliters <= 6)
			{
				if (task->iPulse == ENUM_PULSE_2US)
				{
					task->iPulse = ENUM_PULSE_1US;
					/*task->iRange = ENUM_FIBER_RANGE_40KM;*/
					task->iRange = ENUM_FIBER_RANGE_50KM;
				}

				if (task->iPulse == ENUM_PULSE_10US)
				{
					task->iPulse = ENUM_PULSE_2US;
					/*task->iRange = ENUM_FIBER_RANGE_40KM;*/
					task->iRange = ENUM_FIBER_RANGE_50KM;
				}
			}
		}

		// 中距离参数调整
		else if (autoFiberLen < 15000.0f)
		{
			if ((task->iPulse == ENUM_PULSE_10US) || (task->iPulse == ENUM_PULSE_20US))
			{
				/*task->iRange = ENUM_FIBER_RANGE_40KM;*/
				task->iRange = ENUM_FIBER_RANGE_50KM;
			}
		}

        LOG(LOG_INFO, "Yes spliters,iRange = %d, iPulse = %d\n", task->iRange, task->iPulse);
    }
}

/*
 *设置不同量程的脉宽个数
*/
int SetPulseNumdifferentRange(int rangeIndex, float autoRange, int wave)
{
	int flag = 0;
	// 无分路器情况使用3个脉宽进行测试
	int spliters = pSolaMessager->lineDefine.splittingRatioLevel1
				 + pSolaMessager->lineDefine.splittingRatioLevel2
				 + pSolaMessager->lineDefine.splittingRatioLevel3;
	if(autoRange<8000.0f)
	{
		flag = 1;
	}
	else if(autoRange < 15000.0f)
	{
		flag = 2;
	}
		

	if(!spliters)
	{
		return 3;
	}
	else
	{
		if(flag == 1)
		{	
			if(spliters<=2)
			{
				return 3;
			}
			else if((spliters<=5))//&&(wave==0))
			{
				return 4;
			}
		}
		else if(flag == 2)
		{
			if(spliters<=5)
			{
				return 4;
			}
		}
		return SOLAPulseNum[rangeIndex];	
	}
}
/**
 * 生成所有采集任务事件并post生成的所有事件到Sola的事件队列中
 * 执行成功返回0
 *
 * 每次测量有n(用户设置)个波长，分为n个大任务
 * 每个大任务有k个采集脉宽，分为k个小任务
 * 每个小任务循环执行m次 (每个大任务产生的事件个数 = 1+k*(m+1))
 * 
 * 每个大任务生成的任务事件队列如下: 
 * +--------+---------------------------+---------------------------+-------
 * |波长事件|       任务事件(脉宽1)     |       任务事件(脉宽2)     |总共k个
 * +--------+---------------+-----------+---------------+-----------+-------
 * |WAVE_SIG|NEW_TASK_SIG(m)|ANALYZE_SIG|NEW_TASK_SIG(m)|ANALYZE_SIG|....
 * +--------+---------------+-----------+---------------+-----------+-------
 * |设置波长| 采集数据(m次) | 分析数据  | 采集数据(m次) | 分析数据  |....
 * +--------+---------------+-----------+---------------+-----------+-------
 */
static int Sola_GrenrateAllTasks(Sola* me)
{
    //用于统计当前一共产生了多少个有效的任务事件(包含分析事件)
    int TaskEventCnt = 0;
    //用于从内部的缓冲区中分配TaskEvent
    int EventBufferIndex = 0;

    //检查光纤的实际长度，确定量程
    float fiberLen = CheckFiberLen(pOtdrTopSettings,
    				 pSolaMessager->lineDefine.splittingRatioLevel1
    			   + pSolaMessager->lineDefine.splittingRatioLevel2
    			   + pSolaMessager->lineDefine.splittingRatioLevel3);
	me->Context.Task.autoRange = fiberLen;
	int iRange = GetFiberRange(fiberLen / 1000.0f);
	
	// 用于算法分析
	SetFiberLen(fiberLen);

    //根据用户选择的波长生成任务事件
    me->Settings = pUser_Settings->sSolaSetting;
    
    int iWave;
    for (iWave = WAVELEN_1310; iWave < WAVE_NUM; ++iWave)
    {
        //如果用户选择了当前波长 
        if (me->Settings.Wave[iWave])
        {
            //波长事件, 在该事件处理中设置当前任务的波长
            TaskEvent* WaveEvent = &me->TaskEventBuffer[EventBufferIndex++];
            WaveEvent->super.sig = WAVE_SIG;
            WaveEvent->super.dynamic = 0;
            WaveEvent->iRange = iRange;
            WaveEvent->iWave = iWave;
            Sola_PostEvent(me, (QEvent*)WaveEvent, NORMAL_PRIORITY);

			//第一个事件分析事件，执行初步的分析任务
			static const QEvent firstEvent = {.sig = FIRST_EVENT_SIG, .dynamic = 0};
			Sola_PostEvent(me, &firstEvent, NORMAL_PRIORITY);

            //所有的采集任务，每一个任务对应一个脉冲宽度
            int i;
			int solaPulseNumTemp = SetPulseNumdifferentRange(iRange,fiberLen,iWave);
            for (i = 0; i < solaPulseNumTemp; ++i)
            {
                //从事件缓冲区分配一个事件,事件缓冲区自增
                TaskEvent* e = &me->TaskEventBuffer[EventBufferIndex++];

                //任务事件信息赋值
                e->super.sig = NEW_TASK_SIG;
                e->super.dynamic = 0;
                e->iRange = iRange;
                e->iWave = iWave;
                e->iPulse = SOLAChosedPulses[iRange][i];

				// 重置测量参数 主要针对5ns 1us 10us, 20us
				AdjustMeasureParament(e,fiberLen);

				static const QEvent PulseEvent = {.sig = PULSE_SIG, .dynamic = 0};
                Sola_PostEvent(me, &PulseEvent, NORMAL_PRIORITY);
				
                //重复执行该任务多次，以便更好的分析数据
                int j;
                for (j = 0; j < perPulseLoopNum[e->iPulse]; ++j)
                {
                    Sola_PostEvent(me, (QEvent*)e, NORMAL_PRIORITY);
                    TaskEventCnt++;
                }                

                //定义静态只读的分析事件,所有的分析事件都一样，使用同一个事件即可
                //写入分析任务事件，执行多次采集任务之后，该任务进行一次数据分析
                static const QEvent AnalyzeEvent = {.sig = ANALYZE_SIG, .dynamic = 0};
                Sola_PostEvent(me, &AnalyzeEvent, NORMAL_PRIORITY);
                TaskEventCnt++;
            }
        }
    }

    //根据产生的事件总数量计算进度步进值并初始化进度值为0
    me->fProgressStep = 100.0f / (float)TaskEventCnt;
    me->fProgress = 0.0f;

    //停止事件作为收尾信号
    static const QEvent StopEvent = {.sig = STOP_SIG, .dynamic = 0};
    Sola_PostEvent(me, &StopEvent, NORMAL_PRIORITY);
    
    return 0;
}

/**
 * 初始化采集过程，包括打开模拟板电源，执行硬件初始化等动作
 * 并进行可能的自整定过程以及检查光路等
 * 执行成功返回0
 */
static int Sola_DaqInit(Sola* me)
{
    int iErr = 0;

    if (!iErr)
    {   
        //初始化上下文(初始化硬件)
        if (SolaContext_Init(&me->Context))
        {
            iErr = -1;
            me->iError = SOLA_BOARD_ERR;
        }
        SOLA_DEBUG("SolaContext_Init\n");
    }
  
    if (!iErr && me->iCheckLight)  
    {      
        //有光检查
	    if (CheckFiberLight(pOtdrTopSettings))
        {
            //iErr = -2;
            me->iError = SOLA_CHECK_LIGHT_ERR;
        }
        SOLA_DEBUG("CheckBreakVoltage\n");
    }
	
	CheckBreakVoltage(pOtdrTopSettings);
	
    if (!iErr && me->iCheckEndFace)
    {
        //断面检查
        if (CheckFiberEndFace(pOtdrTopSettings))
        {
            //iErr = -3;
            me->iError = SOLA_CHECK_ENDFACE_ERR;
        } 
        SOLA_DEBUG("CheckFiberEndFace\n");
    }

    if (!iErr)
    {
        //生成任务队列
        Sola_GrenrateAllTasks(me);
    }
    
    return iErr;
}

/**
 * 关闭光模块
 */
static void Sola_DaqFinish(Sola* me)
{
    CloseOpm(me->Context.pOpmSet);
}

/**
 * 初始化当前的任务参数
 */
static void Sola_InitTask(Sola* me, TaskEvent*e)
{
    SolaTask* task = &me->Context.Task;

    task->iWave = e->iWave;
    task->iPulse = e->iPulse;
    task->iRange = e->iRange;

	SAMPLE_SETTING sSmpSet = pOtdrTopSettings->pUser_Setting->sSampleSetting;
    task->fSmpRatio = fSOLASampleRatio[task->iRange];
    task->iSmpReg = iSOLAPhaseShiftReg[task->iRange];

	int offset = CalFiberOffset(pOtdrTopSettings->pOpmSet->pOpmTuning->OffsetPoint, 
								task->fSmpRatio, 
								task->iRange, 
								1);
    task->iOffset = offset;
	
    task->iBlind = GetBlindLength(task->iPulse, task->fSmpRatio);
    task->iDaqNum = DaqNum[task->iPulse];
    float Distance = Range2Dist(task->iRange);
    //采样长度
    task->iDataCount = GetSmpCount(
        task->iRange,
        sSmpSet.sWaveLenArgs[task->iWave].fRefractiveIndex,
        task->fSmpRatio
        );
    task->fSmpInterval = Distance / (float)task->iDataCount;
    task->iDisplayCount = task->iDataCount;
    
#if (ENABLE_SOLA_DEBUG == 1)
    SolaTask_Print(task, stdout);
#endif
}

// 调整通道
unsigned int AdjustChanelParameters(Sola* me,int num)
{
	SolaTask *task = &me->Context.Task;
	int pulse = task->iPulse;
	int spliters[3]={me->Context.SolaAlgorithm.pAlgorithmInfo[task->iWave].spliterRatioGrade1,
					 me->Context.SolaAlgorithm.pAlgorithmInfo[task->iWave].spliterRatioGrade2,
					 me->Context.SolaAlgorithm.pAlgorithmInfo[task->iWave].spliterRatioGrade3};
	if(task->autoRange<=8000.0f)
	{	
		int i = 0, flag = 0;
		for(i = 0; i < 3; i++)
		{
			if((spliters[i]>3)||(spliters[0]+spliters[1]+spliters[2]>=5))
			{
				flag = 1;
				break;
			}
		}
		if((flag!=1)&&(pulse==ENUM_PULSE_200NS))
		{
			return attr[pulse][num+4];
		}
	}

	if(pulse==ENUM_PULSE_10NS)
	{
		return attr[pulse][num+2];
	}
	else
	{
		return attr[pulse][num];
	}
}



/**
 * 处理新的任务事件，
 * 执行一次任务(内部有n个小任务)的采集过程
 */
static void Sola_HandleTaskEvent(Sola* me, TaskEvent* e)
{
	//#define SAVE_ADC_DATA_SOLA
	#ifdef SAVE_ADC_DATA_SOLA
		int iDaqCnt = 0;
    #endif
    //根据任务事件，计算当前任务的相关输入参数
    Sola_InitTask(me, e);
    //获得当前任务的参数并用来初始化采集参数
    DAQ_SETTING DaqSettings;
    SolaTask* Task = &me->Context.Task;
    //注意: 所有的任务使用同一个缓冲区
    DaqSettings.pOrigData = me->Context.pOrigData;
    DaqSettings.iCloseFrontEndFlag = 0;
    DaqSettings.iCloseLdPulseFlag = 0;
    DaqSettings.iOffset = Task->iOffset;

    DaqSettings.iNoiseCnt = BLACK_LEVEL_LEN;
    DaqSettings.iPulseKind = Task->iPulse;
    DaqSettings.iDaqCount = Task->iDataCount + Task->iOffset;
    DaqSettings.iDisplayCount = Task->iDisplayCount;
	DaqSettings.iSmpReg = Task->iSmpReg;
	DaqSettings.fSmpRatio = Task->fSmpRatio;
    DaqSettings.iAdcGap = CalAdcGapValue(DaqSettings.iDaqCount,DaqSettings.fSmpRatio,
										Task->autoRange,DaqSettings.iPulseKind);
	//初始化当前的组合结构数据
    COMBINE_INFO* pCombine = &me->Context.Combine;
	
    pCombine->iSigLen = Task->iDataCount + BLACK_LEVEL_LEN;
    pCombine->iDataNum = 0;
    pCombine->iBlind = Task->iBlind;
    pCombine->iCombineFlag = 0;
    pCombine->iRealTimeFlag = 0;
    pCombine->iPulseWidth = Task->iPulse;
    pCombine->fSampleRating = Task->fSmpRatio;
    pCombine->iTaskCycle = 0;
    pCombine->iTotalNum = Task->iDaqNum;
    //开始执行所有小任务的采集过程，并且组合数据
    SOLA_DEBUG("Task->iDaqNum is %d.\n", Task->iDaqNum);
    int iDaqPos = 0;
    for (iDaqPos = 0; iDaqPos < Task->iDaqNum; ++iDaqPos)
    {
        SOLA_DEBUG("iDaqPos is %d.\n", iDaqPos);
        DaqSettings.iAtteDbClass = AdjustChanelParameters(me,iDaqPos);
        SOLA_DEBUG("iDaqPos is %d, iAtteDbClass is %d.\n", iDaqPos, DaqSettings.iAtteDbClass);
        DaqSettings.iFilter = solaFilters[Task->iPulse][iDaqPos];
        SOLA_DEBUG("iDaqPos is %d, iFilter is %d.\n", iDaqPos, DaqSettings.iFilter);
        //采集任务
        DaqStart(&DaqSettings);
        SOLA_DEBUG("after DaqStart.\n");
        //拷贝数据到组合缓冲区
        memcpy(
            pCombine->pOriginalData, 
			DaqSettings.pOrigData + DaqSettings.iOffset, 
			pCombine->iSigLen * sizeof(UINT16)
			);
        SOLA_DEBUG("after memcpy.\n");
        //组合的不同参数赋值
        pCombine->combine_para[iDaqPos].iAttenuation = DaqSettings.iAtteDbClass;
        pCombine->iDataNum = iDaqPos+1;
        //最后一次设置组合标志位
        if (pCombine->iDataNum == Task->iDaqNum)
            pCombine->iCombineFlag = 1;
        else
            pCombine->iCombineFlag = 0;

        SOLA_DEBUG("before SaveMatData.\n");
        SaveMatData(Task, &DaqSettings);
        SOLA_DEBUG("after SaveMatData.\n");

        SOLA_DEBUG("before SignalCombine.\n");
        //通道组合
        SignalCombine(pCombine);
        SOLA_DEBUG("after SignalCombine.\n");
        
    }
    
    //Post 初始进度变化的消息
    me->fProgress += me->fProgressStep;
    Sola_ProgressChanaged(me);
}

//处理注入光纤和接收光纤
void Sola_HandleLaunchRecvFiber(ALGORITHM_INFO* pAlg)
{
    //未设置注入光纤
    if (!pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableRecvFiber && 
        !pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableLaunchFiber)
        return;

    EVENTS_TABLE* events = &pAlg->EventsTable;
    events->iLaunchFiberIndex = 0;
    events->iRecvFiberIndex = 0;
    events->fLaunchFiberPosition = 0;
    events->fRecvFiberPosition = 0;
    
    //如果事件个数小于2个，无法设置跨段
    if (events->iEventsNumber < 2)
    {
        return;
    }

    //计算跨段长度    
    float fiberLen = CalcRealDist(events->EventsInfo[events->iEventsNumber - 1].iBegin, pAlg->fPointLength);
    float launchFiberLen = pOtdrTopSettings->pUser_Setting->sSolaSetting.fLaunchFiberLen;
    float recvFiberLen = fiberLen - pOtdrTopSettings->pUser_Setting->sSolaSetting.fRecvFiberLen;
    //float totalThr = ThresholdOfMatch(fiberLen, pAlg->fPointLength);

    if (pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableLaunchFiber)
    {
        //注入光纤在有效范围之内
        if (launchFiberLen > 0)
        {
            int begin = (int)(launchFiberLen / pAlg->fPointLength + 0.5f);
            int index = MatchOtdrEvent(events, launchFiberLen, pAlg->iBlind, begin);
            if (index > 0 )
            {
                events->fLaunchFiberPosition = events->EventsInfo[index].fEventsPosition;
                events->iLaunchFiberIndex = index;
            }
            else if((fiberLen - launchFiberLen) >= -1.0f)
            {
                index = AddEventsTable(pAlg->pSignalLog, pAlg->iSignalLength, events, pAlg->iBlind, begin, pAlg->fPointLength);
                if(index >= 0)
                {
                    events->fLaunchFiberPosition = events->EventsInfo[index].fEventsPosition;
                    events->iLaunchFiberIndex = index;
                }
            }        
            else    //注入光纤长度超过光纤长度，将光纤末端设置为注入光纤
            {
                events->fLaunchFiberPosition = events->EventsInfo[events->iEventsNumber - 1].fEventsPosition;
                events->iLaunchFiberIndex = events->iEventsNumber - 1;
            }
        }
    }
    
    //若跨段间距太小，则无法设置跨段
    if (pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableRecvFiber && 
        pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableLaunchFiber)
    {
        if ((recvFiberLen - launchFiberLen) <= 0)
        {
            //无法设置跨段
            LOG(LOG_ERROR, "Can not set launch fiber:distance between launch and recv is too small\n");
            return;
        }
    }
    
    if (pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableRecvFiber)
    {
        //接收光纤在有效范围之内
        if ((fiberLen - recvFiberLen) >= -1.0f
            && recvFiberLen > 0)
        {
            int begin = (int)(recvFiberLen / pAlg->fPointLength + 0.5f);
            int index = MatchOtdrEvent(events, recvFiberLen, pAlg->iBlind, begin);
            if (index > 0 )
            {
                events->fRecvFiberPosition = events->EventsInfo[index].fEventsPosition;
                events->iRecvFiberIndex = index;
            }
            else
            {
                index = AddEventsTable(pAlg->pSignalLog, pAlg->iSignalLength, events, pAlg->iBlind, begin, pAlg->fPointLength);
                if (index >= 0)
                {
                    events->fRecvFiberPosition = events->EventsInfo[index].fEventsPosition;
                    events->iRecvFiberIndex = index;
                }
            }
        }
    }

    if(events->iLaunchFiberIndex > 0 || events->iRecvFiberIndex > 0)
    {
        //重新计算光回损
        unsigned short *pTransferSignal = (unsigned short *)malloc(sizeof(unsigned short) * pAlg->iSignalLength);
    	memset(pTransferSignal, 0, sizeof(unsigned short) * pAlg->iSignalLength);

        float *fSignalLog = pAlg->pSignalLog;
        float fMin = GetMinFloat(fSignalLog + pAlg->iSignalLength - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN); 
        float fTmp = 0.0f; 
        int start = 0, end = 0, i;

        if (fMin > 0.0f) 
        { 
            fMin = 0.0f; 
        } 
        
        for (i=0; i<pAlg->iSignalLength; i++) 
        { 
            fTmp = fSignalLog[i] - fMin; 
            if (fTmp < 0.0f) 
            { 
                pTransferSignal[i] = 0.0f; 
            } 
            else 
            { 
                pTransferSignal[i] = (unsigned short)floor(fTmp * SCALE_FACTOR + 0.5f); 
            } 
        }
        
    	end = Min(events->EventsInfo[events->iEventsNumber-1].iEnd, pAlg->iDisplayLenth);
    	
    	if(events->iLaunchFiberIndex > 0)
    	{
            start = events->EventsInfo[events->iLaunchFiberIndex].iBegin;
    	}
    	
    	if((events->iRecvFiberIndex > 0) && (events->iRecvFiberIndex != events->iEventsNumber-1))
    	{
            end = events->EventsInfo[events->iRecvFiberIndex].iEnd;
    	}

        if ((events->iEventsNumber == 1) || (events->iLaunchFiberIndex == events->iEventsNumber-1)
            || (events->iLaunchFiberIndex == events->iRecvFiberIndex))
    	{
    		pAlg->fTotalReturnLoss = 0;
    	}
    	else
    	{
    	    unsigned short uTmpValue = pTransferSignal[start];
    	    pTransferSignal[start] = pTransferSignal[0];
    	    
    		pAlg->fTotalReturnLoss = ReturnLoss(pTransferSignal, start, end, 
    					pAlg->SetParameters.fBackScattering, pAlg->SetParameters.fSampleRating);
    		pTransferSignal[start] = uTmpValue;
    	}
    	
    	free(pTransferSignal);
    	pTransferSignal = NULL;
   }
}

//匹配注入光纤和接收光纤
void Sola_MatchLaunchRecvFiber(ALGORITHM_INFO* pAlg, int wave, 
    SOLA_INFO *solaInfo, SolaEvents *solaEvents)
{
    if(!pAlg || !solaInfo || !solaEvents)
        return;

        //未设置注入光纤
    if (!pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableRecvFiber && 
        !pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableLaunchFiber)
        return;
        
    if(pAlg->EventsTable.iEventsNumber < 2)
    {
        return;
    }
        
    int i = 0;
    
    EVENTS_TABLE *eventTableOfWave = &solaInfo->pAlgorithmInfo[wave].EventsTable;
    float fiberLength = eventTableOfWave->EventsInfo[eventTableOfWave->iEventsNumber-1].fEventsPosition;
    float launchFiberLen = pOtdrTopSettings->pUser_Setting->sSolaSetting.fLaunchFiberLen;
    float recvFiberLen = fiberLength - pOtdrTopSettings->pUser_Setting->sSolaSetting.fRecvFiberLen;
    float fLaunchFiberPosition = pAlg->EventsTable.fLaunchFiberPosition;
    float fRecvFiberPosition = pAlg->EventsTable.fRecvFiberPosition;

    //匹配当前波长合并后的事件列表注入光纤和接收光纤位置
    if((fabsf(fLaunchFiberPosition - eventTableOfWave->fLaunchFiberPosition) > 0.001f)
        || (fabsf(fRecvFiberPosition - eventTableOfWave->fRecvFiberPosition) > 0.001f))
    {
        for(i=0; i<eventTableOfWave->iEventsNumber; i++)
        {
            //找到对应的事件
            if(fabsf(eventTableOfWave->EventsInfo[i].fEventsPosition - launchFiberLen) < 1.0f)
            {
                eventTableOfWave->fLaunchFiberPosition = eventTableOfWave->EventsInfo[i].fEventsPosition;
                eventTableOfWave->iLaunchFiberIndex = i;
            }
            else if(fabsf(eventTableOfWave->EventsInfo[i].fEventsPosition - fLaunchFiberPosition) < 1.0f)
            {
                //和当前注入光纤位置比较，取离注入光纤长度近的事件
                if(eventTableOfWave->iLaunchFiberIndex == 0 || eventTableOfWave->iLaunchFiberIndex >= eventTableOfWave->iEventsNumber
                    || (fabsf(eventTableOfWave->EventsInfo[i].fEventsPosition - launchFiberLen) < 
                    fabsf(eventTableOfWave->EventsInfo[eventTableOfWave->iLaunchFiberIndex].fEventsPosition - launchFiberLen)))
                {
                    eventTableOfWave->fLaunchFiberPosition = eventTableOfWave->EventsInfo[i].fEventsPosition;
                    eventTableOfWave->iLaunchFiberIndex = i;
                }
            }
                
            if(fabsf(eventTableOfWave->EventsInfo[i].fEventsPosition - recvFiberLen) < 1.0f)
            {
                eventTableOfWave->fRecvFiberPosition = eventTableOfWave->EventsInfo[i].fEventsPosition;
                eventTableOfWave->iRecvFiberIndex = i;
            }
            else if(fabsf(eventTableOfWave->EventsInfo[i].fEventsPosition - fRecvFiberPosition) < 1.0f)
            {
                //和当前接收光纤位置比较，取离接收光纤长度近的事件
                if(eventTableOfWave->iLaunchFiberIndex == 0 || eventTableOfWave->iLaunchFiberIndex >= eventTableOfWave->iEventsNumber
                    || (fabsf(eventTableOfWave->EventsInfo[i].fEventsPosition - recvFiberLen) < 
                    fabsf(eventTableOfWave->EventsInfo[eventTableOfWave->iLaunchFiberIndex].fEventsPosition - recvFiberLen)))
                {
                    eventTableOfWave->fRecvFiberPosition = eventTableOfWave->EventsInfo[i].fEventsPosition;
                    eventTableOfWave->iRecvFiberIndex = i;
                }
            }
        }
    }

    //匹配多波长合并后的事件列表注入光纤和接收光纤位置
    EVENTS_TABLE *eventTable = &solaInfo->EventsTable;
    fiberLength = eventTable->EventsInfo[eventTable->iEventsNumber-1].fEventsPosition;
    recvFiberLen = fiberLength - pOtdrTopSettings->pUser_Setting->sSolaSetting.fRecvFiberLen;
    for (i=0; i<WAVE_NUM; i++)
	{
		if (solaEvents->WaveIsSetted[i] == 0)
		{
            continue;
		}
		
        eventTableOfWave = &solaInfo->pAlgorithmInfo[i].EventsTable;
        
        if(0 != eventTableOfWave->fLaunchFiberPosition || 0 != eventTableOfWave->fRecvFiberPosition)
        {
            int j = 0;
            for(j=0; j<eventTable->iEventsNumber; j++)
            {
                if(fabsf(eventTable->EventsInfo[j].fEventsPosition - launchFiberLen) < 1.0f)
                {
                    eventTable->fLaunchFiberPosition = eventTable->EventsInfo[j].fEventsPosition;
                    eventTable->iLaunchFiberIndex = j;                
                }
                else if(fabsf(eventTable->EventsInfo[j].fEventsPosition - eventTableOfWave->fLaunchFiberPosition) < 1.0f
                    && eventTableOfWave->fLaunchFiberPosition != 0.0f)
                {
                    eventTable->fLaunchFiberPosition = eventTable->EventsInfo[j].fEventsPosition;
                    eventTable->iLaunchFiberIndex = j;
                }
                
                if(fabsf(eventTable->EventsInfo[j].fEventsPosition - recvFiberLen) < 1.0f)
                {
                    eventTable->fRecvFiberPosition = eventTable->EventsInfo[j].fEventsPosition;
                    eventTable->iRecvFiberIndex = j;               
                }                
                else if(fabsf(eventTable->EventsInfo[j].fEventsPosition - eventTableOfWave->fRecvFiberPosition) < 1.0f
                    && eventTableOfWave->fRecvFiberPosition != 0.0f)
                {
                    eventTable->fRecvFiberPosition = eventTable->EventsInfo[j].fEventsPosition;
                    eventTable->iRecvFiberIndex = j;
                }
            }
        }
	}

	return;
}

#ifdef SAVE_CURVE_DATA
//保存sola波形数据
static void SaveSolaCurveData(ALGORITHM_INFO* Algorithm)
{
    FILE* fp = NULL;
    char strfile[64] = {0};
    sprintf(strfile, "/mnt/usb/wave_%d_data.dat", Algorithm->SetParameters.iwavelength);
    fp = fopen(strfile, "w+");
    if(fp != NULL)
    {
        int i;
        for(i = 0; i < Algorithm->iSignalLength; ++i)
        {
            fprintf(fp, "%f\n", *(Algorithm->pSignalLog + i));
        }
        fclose(fp);
    }
    else
    {
        LOG(LOG_ERROR, "open file failed\n");
    }
}

#endif
//处理sola波形数据
static void HandleSolaCurveData(ALGORITHM_INFO* Algorithm, SOLA_DATA* newSolaData)
{
    float fMin = 0.0f;
    float fTmp = 0.0f;
    int count = Algorithm->iSignalLength;
	if (count <= BLACK_LEVEL_LEN)
	{
		fMin = 0.0f;
	}
	else
	{
		fMin = GetMinFloat(Algorithm->pSignalLog + count - BLACK_LEVEL_LEN, 
					   BLACK_LEVEL_LEN);
		if (fMin > 0.0f)
		{
			fMin = 0.0f;
		}
	}
    int i = 0;
    for(i = 0; i < count; ++i)
    {
        fTmp = *(Algorithm->pSignalLog +i) - fMin;
        if (fTmp >= 0.0f)
		{
			*(newSolaData->data + i) = (UINT16)(SCALE_FACTOR * fTmp);
		}
		else
		{
			*(newSolaData->data + i) = 0;
		}
    }
}

//设置sola波形数据
static void SetSolaData(ALGORITHM_INFO* Algorithm)
{
    int pulseWidth = Algorithm->PulseWidth;
    #ifdef MINI2
    if(pulseWidth > ENUM_PULSE_30NS && pulseWidth < ENUM_PULSE_1US)
    #else
    if (pulseWidth == ENUM_PULSE_100NS || pulseWidth == ENUM_PULSE_200NS)
    #endif
    {
        #ifdef SAVE_CURVE_DATA
        SaveSolaCurveData(Algorithm);
        #endif
        s_waveNum++;
        SOLA_DATA* newSolaData = solaCurveData + s_waveNum - 1;
        if(newSolaData->onePoint == 0.0f)
        {
            newSolaData->onePoint = Algorithm->fPointLength;
            newSolaData->count = Algorithm->iSignalLength;
            //处理sola波形数据
            HandleSolaCurveData(Algorithm, newSolaData);
        }
    }
}

/**
 * 分析新数据
 */
static void Sola_HandleAnalyzeEvent(Sola* me, QEvent* e)
{
    //读取组合参数
    SolaTask* Task = &me->Context.Task;
    
    ALGORITHM_INFO Algorithm;
    COMBINE_INFO* pCombine = &me->Context.Combine;
 
    //复用组合的缓冲区
    Algorithm.pSignalLog = pCombine->pCombineData;

	//拷贝组合点位置信息 0行=组合点位置,1行=组合类型, [0][19]=代表组合次数 added by  2015.10.9
	int i;
	for (i = 0; i < Task->iDaqNum; i++)
	{
		Algorithm.pCombineInfo[0][i] = (UINT32)pCombine->combine_pos[i].iComPosition;
		Algorithm.pCombineInfo[1][i] = (UINT32)pCombine->combine_pos[i].iComStyle;
	}
    Algorithm.pCombineInfo[0][19] = Task->iDaqNum;

    Algorithm.iSOLATest = 1;
    Algorithm.iSignalLength = Task->iDataCount + BLACK_LEVEL_LEN;
    Algorithm.iBlind = Task->iBlind;
    Algorithm.iFiberRange = Task->iRange;
    Algorithm.PulseWidth = Task->iPulse;
    Algorithm.iDisplayLenth = Task->iDisplayCount;
    Algorithm.SetParameters.fSampleRating = Task->fSmpRatio;
	Algorithm.iOffsetFiberLen = Task->iOffset - (int)(8 * Task->fSmpRatio);            // added by wyl 2019/09/17

    PUSER_SETTINGS pUsrSet = pOtdrTopSettings->pUser_Setting;
	Algorithm.SetParameters.fRefractiveIndex = pSolaMessager->lineDefine.groupIndex1550;
	Algorithm.SetParameters.fLossThreshold = pUsrSet->sAnalysisSetting.fSpliceLossThr;			                  //损耗阈值
	Algorithm.SetParameters.fReflectThreshold = pUsrSet->sAnalysisSetting.fReturnLossThr;		                  //反射率阈值
	Algorithm.SetParameters.fFiberEndThreshold = pUsrSet->sAnalysisSetting.fEndLossThr;		                      //末端检测阈值
	Algorithm.SetParameters.fBackScattering = pSolaMessager->lineDefine.backScatter1550;                          //背向散射强度值
    Algorithm.SetParameters.iwavelength = Task->iWave;

	//设置分光比
	Algorithm.spliterRatioGrade1 = me->Context.SolaAlgorithm.pAlgorithmInfo[Task->iWave].spliterRatioGrade1;
	Algorithm.spliterRatioGrade2 = me->Context.SolaAlgorithm.pAlgorithmInfo[Task->iWave].spliterRatioGrade2;
	Algorithm.spliterRatioGrade3 = me->Context.SolaAlgorithm.pAlgorithmInfo[Task->iWave].spliterRatioGrade3;

	//计算折射率和背向散射系数
	switch (Algorithm.SetParameters.iwavelength)
	{
		case 0:
			Algorithm.SetParameters.fRefractiveIndex = 1.4677f * Algorithm.SetParameters.fRefractiveIndex;
			Algorithm.SetParameters.fRefractiveIndex = Algorithm.SetParameters.fRefractiveIndex / 1.468325f;
			Algorithm.SetParameters.fBackScattering += -79.44f;
			Algorithm.SetParameters.fBackScattering -= -81.87f;
			break;
		case 1:
			break;
		case 2:
			Algorithm.SetParameters.fRefractiveIndex = 1.468734f * Algorithm.SetParameters.fRefractiveIndex;
			Algorithm.SetParameters.fRefractiveIndex = Algorithm.SetParameters.fRefractiveIndex / 1.468325f;
			Algorithm.SetParameters.fBackScattering += -82.58f;
			Algorithm.SetParameters.fBackScattering -= -81.87f;
			break;
		default:
			break;
				
	}
    //算法分析
    AlgorithmMain(&Algorithm);
    
    //处理注入光纤和接收光纤
    Sola_HandleLaunchRecvFiber(&Algorithm);
    
    //合并事件列表
    SOLAMergeEventsTable(&Algorithm, &me->Context.SolaAlgorithm);

	//自动判别分光比
    SpliterRationAutoJudge(&me->Context.SolaAlgorithm,Task->iWave);
	
    //得到事件分析数据
    me->Context.Events.WaveIsSetted[Task->iWave] = 1;
    
    	//设置注入光纤和接收光纤
    Sola_MatchLaunchRecvFiber(&Algorithm, Task->iWave, 
        &me->Context.SolaAlgorithm, &me->Context.Events);

    me->Context.Events.EventTable = me->Context.SolaAlgorithm.EventsTable;
	for (i=0; i<WAVE_NUM; i++)
	{
		if (me->Context.Events.WaveIsSetted[i] == 1)
		{
			    me->Context.Events.WaveEventTable[i]
					= me->Context.SolaAlgorithm.pAlgorithmInfo[i].EventsTable;
				me->Context.Events.TotalReturnLoss[i]
					= me->Context.SolaAlgorithm.pAlgorithmInfo[i].fTotalReturnLoss;
		}
	}

#if (ENABLE_SOLA_DEBUG == 1)
    SolaEvents_Print(&me->Context.Events, stdout);
#endif

    //Post 初始进度变化的消息
    me->fProgress += me->fProgressStep;
    Sola_ProgressChanaged(me);

    //时间戳
    me->Context.Events.ulDTS = time(NULL);
    
    //post 新的数据已经准备好
    Sola_PostMessage(me, SOLA_DATA_READY, 0); 
	//保存数据
    SetSolaData(&Algorithm);
}

/*
 *sola第一个事件损耗计算的数据采集
*/
static void Sola_FirstSampleHandle(SOLA_INFO *pSolaAlgInfo, int iWave)
{
	PDAQ_SETTING pDaq_Setting = NULL;
	DBG_ENTER();
	#define SEVENTEENKM  55000
	
	pDaq_Setting = (PDAQ_SETTING)calloc(1, sizeof(DAQ_SETTING));//为采集设置申请空间
    pDaq_Setting->pOrigData = (UINT16 *)calloc(1, MAX_DATA * sizeof(UINT16));
   	pDaq_Setting->iPulseKind = (UINT32)ENUM_PULSE_20NS;
   	pDaq_Setting->iAtteDbClass = (UINT32)ENUM_7_5DB;
    pDaq_Setting->iFilter = (UINT32)_IIR_128;
    pDaq_Setting->iDaqCount = SEVENTEENKM - BLACK_LEVEL_LEN;
    pDaq_Setting->iDisplayCount = SEVENTEENKM;
	pDaq_Setting->iAdcGap = 0x500;
	int offset = CalFiberOffset(pOtdrTopSettings->pOpmSet->pOpmTuning->OffsetPoint, 
								8, 4, 1);
	pDaq_Setting->iOffset = offset;                  //10km高分辨率的偏移量
	pDaq_Setting->iSmpReg = DAQ_INTP01_08;
	pDaq_Setting->fSmpRatio = SMP_COUNT01_08;
	pDaq_Setting->iNoiseCnt = BLACK_LEVEL_LEN;
	
	POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;
	
	Opm_SetWave(iOpmFd, pOpmSet, iWave);
	
	MsecSleep(80);

	// 最多采集两次
	int i = 0, loop = 2;
	EVENTS_INFO event;

	float *logSignal = (float *)calloc(pDaq_Setting->iDisplayCount, sizeof(float));
	for(i = 0; i < loop; i++)
	{
		memset(&event,0,sizeof(EVENTS_INFO));
	
		// 数据采集
		DaqStart(pDaq_Setting);
		
		IntTransferLog(pDaq_Setting->pOrigData,pDaq_Setting->iDaqCount,logSignal);

		SolaGetFirstEventLoss(logSignal,pDaq_Setting->iDaqCount,pDaq_Setting->iOffset,&event);

		if(event.fLoss== -1)
		{
			loop = 2;
		}
		pSolaAlgInfo->pAlgorithmInfo[iWave].EventsTable.EventsInfo[0] = event;
		pSolaAlgInfo->pAlgorithmInfo[iWave].EventsTable.iEventsNumber = 1;
		pSolaAlgInfo->pAlgorithmInfo[iWave].EventsTable.iEventsStart  = 0;
		pSolaAlgInfo->pAlgorithmInfo[iWave].fTotalReturnLoss = -80;
	} 

	if (pSolaAlgInfo->EventsTable.iEventsNumber <= 0)
	{
		pSolaAlgInfo->EventsTable.iEventsNumber = 1;
		pSolaAlgInfo->EventsTable.iEventsStart = 0;
		pSolaAlgInfo->EventsTable.EventsInfo[0] = event;
	}

	free(logSignal);
	logSignal = NULL;

	free(pDaq_Setting->pOrigData);
	pDaq_Setting->pOrigData = NULL;	

	free(pDaq_Setting);
    pDaq_Setting = NULL;
}


static void Sola_FirstEventHandle(Sola* me, QEvent* e)
{
	int iWave = me->Context.iCurrWave;
	Sola_FirstSampleHandle(&me->Context.SolaAlgorithm, iWave);	
	me->Context.Events.WaveIsSetted[iWave] = 1;

	#define test
	#ifdef test
	int k = 0;
	for(k = 0; k < me->Context.SolaAlgorithm.pAlgorithmInfo[iWave].EventsTable.iEventsNumber; k++)
	{
		//printf("fEventsPosition[%d]=%f\n",k,me->Context.SolaAlgorithm.pAlgorithmInfo[iWave].EventsTable.EventsInfo[k].fEventsPosition);
		//printf("fEventsPosition[%d]=%f\n",k,me->Context.SolaAlgorithm.pAlgorithmInfo[iWave].EventsTable.EventsInfo[k].fEventsEndPosition);

	}
	#endif
	

	
	int i;
	for (i=0; i<WAVE_NUM; i++)
	{
		if (me->Context.Events.WaveIsSetted[i] == 1)
		{
			    me->Context.Events.WaveEventTable[i]
					= me->Context.SolaAlgorithm.pAlgorithmInfo[i].EventsTable;
				me->Context.Events.TotalReturnLoss[i]
					= me->Context.SolaAlgorithm.pAlgorithmInfo[i].fTotalReturnLoss;
		}
	}
    me->Context.Events.EventTable = me->Context.SolaAlgorithm.EventsTable;
    Sola_PostMessage(me, SOLA_DATA_READY, 0); 
}


/**
 * 空闲状态，停止测量时进入该状态，也作为初始状态的页状态
 */
static QState Sola_IdleState(Sola* me, QEvent* e)
{
    switch (e->sig)
    {
    //进入动作
    case Q_ENTRY_SIG: {
        SOLA_DEBUG("On Entry\n");
        Sola_SetRunning(me, 0);
        return Q_HANDLED();
    }
    
    //收到启动信号，切换到采集数据的状态
    case START_SIG: {
        SOLA_DEBUG("Start Sola\n");
        return Q_TRAN(&Sola_AquireState);
    }
    
    //收到退出信号，设置退出标志位，退出整个线程
    case EXIT_SIG: {
        me->iExit = 1;
        return Q_HANDLED();
    }
    
    default:
        break;
    }
    
    return Q_IGNORED();
}

/**
 * 数据采集状态，执行采集任务
 */
static QState Sola_AquireState(Sola* me, QEvent* e)
{
    switch (e->sig)
    {
    //进入动作
    case Q_ENTRY_SIG: {
        SOLA_DEBUG("On Entry\n");
        Sola_SetRunning(me, 1);

        //清除错误
        me->iError = SOLA_NO_ERR;
        //清除内部的消息队列
        Sola_ClearMessages(me); 
        //蜂鸣器发声
        SpeakerStart();
        DrawLaserIsOn(0);
        RefreshScreen(__FILE__, __func__, __LINE__);
        //执行采集前的初始化
        if (0 == Sola_DaqInit(me))
        {
            //Post 启动消息
            Sola_PostMessage(me, SOLA_STARTED, 0);
            //Post 初始进度变化的消息
            Sola_ProgressChanaged(me);
            PromptLaserIsOn(1);
        }
        else 
        {
            //抛出错误消息
            Sola_PostMessage(me, SOLA_ERROR, me->iError);
            //初始化执行失败则停止任务
            Sola_Stop(me);
        }
        return Q_HANDLED();
    }

    /*********************************核心步骤**********************************/
    //设置波长
    case WAVE_SIG: {
        TaskEvent* te = ((TaskEvent*)e);
        SOLA_DEBUG("Setting Wave, Wave = %d\n", te->iWave);
        //设置波长
        SolaContext_SetWave(&me->Context, te->iWave);
        //post 波长发生变化的信号
        Sola_PostMessage(me, SOLA_WAVE_CHANGED, te->iWave);
		CloseMatFile();
		OpenMatFile(te->iWave);
        return Q_HANDLED();
    }

	case FIRST_EVENT_SIG: {
        SOLA_DEBUG("FIRST_EVENT_SIG\n");
        Sola_FirstEventHandle(me, e);
		return Q_HANDLED();
	}
	
	case PULSE_SIG: {
        SOLA_DEBUG("PULSE_SIG\n");
        //初始化当前的组合结构数据
	    COMBINE_INFO* pCombine = &me->Context.Combine;
	    //清除上一次组合的参数
	    ClearCombineInfo(pCombine);
		return Q_HANDLED();
	}
    //执行采集任务
    case NEW_TASK_SIG: {
        SOLA_DEBUG("Getting Data, Pulse = %d, Range = %d\n", ((TaskEvent*)e)->iPulse, ((TaskEvent*)e)->iRange);
        //处理新的任务
        Sola_HandleTaskEvent(me, (TaskEvent*)e);
        return Q_HANDLED();
    }
    //分析数据
    case ANALYZE_SIG: {
        SOLA_DEBUG("Analyzing Data\n");
        Sola_HandleAnalyzeEvent(me, e);
        return Q_HANDLED();
    }
    /***************************************************************************/
   
    //执行任务的停止
    case STOP_SIG: {
        SOLA_DEBUG("Stop Sola\n");
        PromptLaserIsOn(0);
		CloseMatFile();
        //清除任务队列里所有的任务事件，切换到空闲状态
        //BUG: 清除了所有事件，在本项目中，此时只有任务事件
        Sola_ClearEvents(me);
        return Q_TRAN(&Sola_IdleState);
    }

    //执行线程停止信号
    case EXIT_SIG: {
        me->iExit = 1;
        return Q_HANDLED();
    }

    //退出动作
    case Q_EXIT_SIG: {
        SOLA_DEBUG("On Exit\n");
        //执行收尾工作并且Post 已停止消息
        Sola_DaqFinish(me);
        //蜂鸣器发声
        SpeakerStart();
        Sola_PostMessage(me, SOLA_STOPED, 0);
        return Q_HANDLED();
    }
    
    default:
        break;
    }

    return Q_IGNORED();
}

/**
 * SOLA内部的执行线程，用于采集数据，分析事件
 */
static void* Sola_Thread(void* arg)
{
    Sola* me = (Sola*)arg;

    //主循环
    while (!me->iExit)
    {
        QEvent* event = Sola_GetEvent(me);
        if (event)
        {
            //有事件则派发事件
            QFsm_dispatch((QFsm*)me, event);
            if (event->dynamic)
            {
                //如果是动态分配的事件，则进行释放
                free(event);
            }
        }
        else
        {
            //空闲则休眠50ms
            MsecSleep(50);
        }
    }

    //退出时清除所有的内部队列
    Sola_ClearEvents(me);
    Sola_ClearMessages(me);

	ThreadExit(NULL);
	return NULL;
}



static int SolaEvents_IsEventEqual(const EVENTS_INFO* left, const EVENTS_INFO* right)
{
    float diff = left->fEventsPosition - right->fEventsPosition;
    return FLOATEQZERO(diff);
}

int SolaEvents_Count(SolaEvents* me)
{
    return me->EventTable.iEventsNumber;
}

//判断事件是否有效
int SolaEvents_IsVaild(SolaEvents* me)
{
    return me->EventTable.iEventsNumber > 0;
}

//清除事件
void SolaEvents_Clear(SolaEvents* me)
{
    if(me->absolutePath)
    {
        free(me->absolutePath);
    }
    memset(me, 0 , sizeof(SolaEvents));
}

const EVENTS_INFO* SolaEvents_GetEvent(SolaEvents* me, int index)
{
    if ( (index >= 0) &&(index < me->EventTable.iEventsNumber) )
    {
        EVENTS_INFO* event = &me->EventTable.EventsInfo[index];
        return (const EVENTS_INFO*)event;
    }

    return NULL;
}

EVENTS_INFO* SolaEvents_GetWaveEvent(SolaEvents* me, int index, int wave)
{
    //获取指定的事件
    const EVENTS_INFO* event = SolaEvents_GetEvent(me, index);

    //判断当前波长和事件是否有效
    if (event && me->WaveIsSetted[wave])
    {
        int i = 0;
        //遍历指定波长的所有事件，指定事件相等的事件并返回
        for (i = 0; i < me->WaveEventTable[wave].iEventsNumber; ++i)
        {
            EVENTS_INFO* WaveEvent = &(me->WaveEventTable[wave].EventsInfo[i]);
            //判断事件是否相等
            if (SolaEvents_IsEventEqual(event, WaveEvent))
            {
                return WaveEvent;
            }
        }
    }

    return NULL;
}

int SolaEvents_GetEventType(SolaEvents* me, int index, SOLATYPE type)
{
    int style = -1;
    const EVENTS_INFO* info = SolaEvents_GetEvent(me, index);
    if(NULL == info)
        return style;
    style = info->iStyle;
    if(type == Ordinary_eventType)
        style &= 0x0000ffff;
    else
    {
        style &= 0xffff0000;
        style = style >> 16;
    }

    return style;
}

void SolaEvents_SetEventType(SolaEvents* me, int index, SOLATYPE type, int style)
{
    int kind = -1;
    int wave = 0;
    for(wave = 0; wave < WAVE_NUM; ++wave)
    {
        EVENTS_INFO* info = SolaEvents_GetWaveEvent(me, index, wave);
        if(NULL == info)
            continue;
        kind = info->iStyle;
        
        if(type == Ordinary_eventType)
            kind = (kind & 0xffff0000) | (style & 0x0000ffff);
        else
            kind = (kind & 0x0000ffff) | ((style & 0x0000ffff)<<16);
        
        info->iStyle = kind;
    }
    if(kind >= 0)
        me->EventTable.EventsInfo[index].iStyle = kind;
}

const EVENTS_INFO* SolaEvents_GetEndEvent(SolaEvents* me, int wave)
{    
    if (me->WaveIsSetted[wave])
    {
        if (me->WaveEventTable[wave].iEventsNumber > 0)
        {
            return &(me->WaveEventTable[wave].EventsInfo[me->WaveEventTable[wave].iEventsNumber - 1]);
        }
    }

    return NULL;
}

float SolaEvents_GetTotalLoss(const SolaEvents* me, int wave)
{    
    if (me->WaveIsSetted[wave])
    {
        if (me->WaveEventTable[wave].iEventsNumber > 0)
        {
            if(me->WaveEventTable[wave].iLaunchFiberIndex <= 0 && me->WaveEventTable[wave].iRecvFiberIndex <= 0)
            {
                return me->WaveEventTable[wave].EventsInfo[me->WaveEventTable[wave].iEventsNumber - 1].fTotalLoss;
            }
            else if(me->WaveEventTable[wave].iRecvFiberIndex > 0)
            {
                float totalLoss = me->WaveEventTable[wave].EventsInfo[me->WaveEventTable[wave].iRecvFiberIndex].fTotalLoss
                    - me->WaveEventTable[wave].EventsInfo[me->WaveEventTable[wave].iLaunchFiberIndex].fTotalLoss;
                return totalLoss;
            }
            else
            {
                float totalLoss = me->WaveEventTable[wave].EventsInfo[me->WaveEventTable[wave].iEventsNumber - 1].fTotalLoss
                    - me->WaveEventTable[wave].EventsInfo[me->WaveEventTable[wave].iLaunchFiberIndex].fTotalLoss;
                return totalLoss;
            }
        }
    }

    return 0.0f;
}

float SolaEvents_GetTotalDistance(SolaEvents* me)
{
    float distance = 0.0f;
    
    int count = SolaEvents_Count(me);
    if (count > 0)
    {
        const EVENTS_INFO* StartEvent = SolaEvents_GetEvent(me, 0);
        const EVENTS_INFO* EndEvent = SolaEvents_GetEvent(me, count - 1);
        distance = EndEvent->fEventsPosition - StartEvent->fEventsPosition;
    }

    return distance;
}

//获取事件类型的名字
const char* SolaEvents_GetEventTypeName(SolaEvents* me, int index)
{
    //不同类型的事件的名字
    static const char* allNames[] = 
    {
        "start",            //0
        "splice",           //1
        "gain",             //2
        "reflect",          //3    
        "end",              //4
        "macrobend",        //5 
        "start_splitter",   //6
        "end_splitter",     //7
        "echo",             //8
        "lowpower",         //9
        "continue",         //10
        "unknow",           //11
    };
    
    //所有的分路器事件类型
    static const char* allSplitterNames[SPLITTER_COUNT] =
    {
        "splitter0",        //占位，未使用
        "splitter2",        //2分路
        "splitter4",        //4分路
        "splitter8",        //8分路
        "splitter16",       //16分路
        "splitter32",       //32分路
        "splitter64",       //64分路
        "splitter128",      //128分路
    };
    
    const char* typeName = NULL;
    
    //如果是分路器事件，则使用分路器类型
    int solaType = SolaEvents_GetEventType(me, index, Splitter_eventType);
    if(solaType > SPLITTER_None && solaType < SPLITTER_COUNT)
    {  
        typeName = allSplitterNames[solaType];  
    }
    //否则使用原有事件类型
    else
    {
        int type = calcEventType(SolaEvents_GetEvent(me, index), index);
        typeName = allNames[type];
    }

    return typeName;
}

void SolaEvents_Print(SolaEvents* me, FILE* out)
{
    fprintf(out, "SolaEvents@[%p]:\n(\n", me);
    fprintf(out, "  WaveIsSetted[1310nm] = %d\n", me->WaveIsSetted[0]);
    fprintf(out, "  WaveIsSetted[1550nm] = %d\n", me->WaveIsSetted[1]);
    fprintf(out, "  WaveIsSetted[1625nm] = %d\n", me->WaveIsSetted[2]);
    fprintf(out, "  1310nm EventNumber = %d\n", me->WaveEventTable[0].iEventsNumber);
    fprintf(out, "  1550nm EventNumber = %d\n", me->WaveEventTable[1].iEventsNumber);
    fprintf(out, "  1625nm EventNumber = %d\n", me->WaveEventTable[2].iEventsNumber);
    fprintf(out, "  Total EventNumber = %d\n", me->EventTable.iEventsNumber);
    fprintf(out, ")\n");
}

static int GetSplitterIOSValue(int splitter_type, WAVELEN wave, float* retIOS)
{
    int iReturn = 0;
    *retIOS = -40.0f;
    SOLA_ITEM_WAVE_ARG* reflectance = &pSolaMessager->itemPassThreshold.maxSplitterReflectance;

    if(reflectance->iSelectedFlag[wave])
    {
        *retIOS = reflectance->fValue[wave];
    }
    else
    {
        iReturn = -1;
    }
    
    return iReturn;
}

static int isSelectedLoss(SOLA_ITEM_WAVE_ARG* loss, WAVELEN wave, float* retLoss)
{
    int iReturn = 0;
    if(loss->iSelectedFlag[wave])
    {
        *retLoss = loss->fValue[wave];
    }
    else
    {
        iReturn = -1;
    }
    return iReturn;
}

static int GetSplitterLossValue(int splitter_type, int wave, float* retLoss)
{
    switch(splitter_type)
    {
        case SPLITTER1_2:
            if(isSelectedLoss(&pSolaMessager->itemPassThreshold.maxSplitter2Loss, wave, retLoss))
            {
                return -1;
            }
            break;
        case SPLITTER1_4:
            if(isSelectedLoss(&pSolaMessager->itemPassThreshold.maxSplitter4Loss, wave, retLoss))
            {
                return -1;
            }
            break;
        case SPLITTER1_8:
            if(isSelectedLoss(&pSolaMessager->itemPassThreshold.maxSplitter8Loss, wave, retLoss))
            {
                return -1;
            }
            break;
        case SPLITTER1_16:
            if(isSelectedLoss(&pSolaMessager->itemPassThreshold.maxSplitter16Loss, wave, retLoss))
            {
                return -1;
            }
            break;
        case SPLITTER1_32:
            if(isSelectedLoss(&pSolaMessager->itemPassThreshold.maxSplitter32Loss, wave, retLoss))
            {
                return -1;
            }
            break;
        case SPLITTER1_64:
            if(isSelectedLoss(&pSolaMessager->itemPassThreshold.maxSplitter64Loss, wave, retLoss))
            {
                return -1;
            }
            break;
        case SPLITTER1_128:
            if(isSelectedLoss(&pSolaMessager->itemPassThreshold.maxSplitter128Loss, wave, retLoss))
            {
                return -1;
            }
            break;
        default:
            break;

    }
    return 0;
}

//判断事件是否通过
static int IsOtdrIORPassed(const EVENTS_INFO* info, int wave)
{
    int pass = 0;
    float reflect = 0.0f;
    int ret_reflectSelected = 0;

    if(wave == WAVELEN_1310)
    {
        ret_reflectSelected = pSolaMessager->itemPassThreshold.maxLinkerReflectance.iSelectedFlag[WAVELEN_1310];
        reflect = pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1310];
    }  
    else if(wave == WAVELEN_1550)
    {
        ret_reflectSelected = pSolaMessager->itemPassThreshold.maxLinkerReflectance.iSelectedFlag[WAVELEN_1550];
        reflect = pSolaMessager->itemPassThreshold.maxLinkerReflectance.fValue[WAVELEN_1550];
    }
        
    if(!ret_reflectSelected)
    {
        pass = 2;
    }
    else
    {
        pass = (info->fReflect > reflect) ? 0 : 1;
    }
    return pass;
}

static int IsOtdrLossPassed(const EVENTS_INFO* info, int wave)
{
    int pass = 1;
    float spliceLoss = 0.0f;
    float linkerLoss = 0.0f;    
    int otdr_type = -1;
    int ret_spliceLossSelected = 0;
    int ret_linkerLossSelected = 0;
    otdr_type = info->iStyle & 0x0000ffff;
    
    if(wave == WAVELEN_1310)
    {
        ret_spliceLossSelected = pSolaMessager->itemPassThreshold.maxSpliceLoss.iSelectedFlag[WAVELEN_1310];
        ret_linkerLossSelected = pSolaMessager->itemPassThreshold.maxLinkerLoss.iSelectedFlag[WAVELEN_1310];
        spliceLoss = pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1310];
        linkerLoss = pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1310];
    }    
    else if(wave == WAVELEN_1550)
    {
        ret_spliceLossSelected = pSolaMessager->itemPassThreshold.maxSpliceLoss.iSelectedFlag[WAVELEN_1550];
        ret_linkerLossSelected = pSolaMessager->itemPassThreshold.maxLinkerLoss.iSelectedFlag[WAVELEN_1550];
        spliceLoss = pSolaMessager->itemPassThreshold.maxSpliceLoss.fValue[WAVELEN_1550];
        linkerLoss = pSolaMessager->itemPassThreshold.maxLinkerLoss.fValue[WAVELEN_1550];
    }

    switch(otdr_type)
    {
    case 0:
    case 5:
        if(!ret_spliceLossSelected)
        {
            pass = 2;
        }
        else 
        {
            pass = (info->fLoss > spliceLoss) ? 0 : 1;
        }
        break;
    case 1:
        //增益事件损耗不做判断,默认通过
        pass = 1;
        break;
    case 10:
    case 11:
        break;
    case 2:
    case 3:
    case 4:
        if(!ret_linkerLossSelected)
        {
            pass = 2;
        }
        else
        {
            pass = (info->fLoss > linkerLoss) ? 0 : 1;
        }
        break;
    case 12:
    case 13:
        break;
    default:
        break;
    }

    return pass;
}
/*
//判断事件是否通过
static int IsOtdrEventPassed(const EVENTS_INFO* event, int wave)
{
    int passed = 0;
    
    printf("event->iStyle = %d\n",
                    event->iStyle);
    passed = IsOtdrLossPassed(event, wave);
    if(!passed)
        passed = IsOtdrIORPassed(event, wave);
    
    printf("######IsOtdrEventPassed  pass = %d\n", passed);

    return passed;
}
*/

int IsSolaLossPassed(const EVENTS_INFO* info, int wave, int splitter_type, int otdr_type)
{
    int pass = 0;
    float spliceLoss = 0.0f;
    //float linkerLoss = 0.0f;
    if(splitter_type > 0)
    {
        if(!GetSplitterLossValue(splitter_type, wave, &spliceLoss))
        {
            pass = (info->fLoss > spliceLoss) ? 0 : 1;
        }
        else
        {
            pass = 2;
        }
    }
    else
    {
        pass = IsOtdrLossPassed(info, wave);
    }
    
    return pass;
}

int IsSolaIORPassed(const EVENTS_INFO* info, int wave, int splitter_type, int otdr_type)
{
    int pass = 0;
    if(splitter_type > 0)
    {
        float reflect = 0.0f;
        if(!GetSplitterIOSValue(splitter_type, wave, &reflect))
        {
            pass = (info->fReflect > reflect) ? 0 : 1;
        }
        else
        {
            pass = 2;
        }
    }
    else
    {
        pass = IsOtdrIORPassed(info, wave);
    }
    
    return pass;

}

//1---pass, 0---fail, 2---未检查通过以及未通过
int IsSolaEventPassed(SolaEvents* me, int index)
{
    int pass = 1;
    int wave =0;
    EVENTS_INFO* info = NULL;
    for(wave = 0; wave < WAVE_NUM; ++wave)
    {
        int splitter_type = 0;
        int otdr_type = -1;
        EVENTS_ISPASS* isPass = NULL;

        info = SolaEvents_GetWaveEvent(me, index, wave);
        if(NULL == info)
            continue;
        isPass = &me->WaveEventTable[wave].EventsIsPass[index];
        splitter_type = SolaEvents_GetEventType(me, index, Splitter_eventType);//= (info->iStyle & 0xffff0000) >> 16;
        otdr_type = SolaEvents_GetEventType(me, index, Ordinary_eventType);//info->iStyle & 0x0000ffff;
        
        isPass->iIsEventLossPass = IsSolaLossPassed(info, wave, splitter_type, otdr_type);
        if(!isPass->iIsEventLossPass)
        {
            pass = 0;
        }
        else if (isPass->iIsEventLossPass == 2)
        {
            pass = 2;
        }
        
        if(otdr_type != 0)
        {
            isPass->iIsReflectPass = IsSolaIORPassed(info, wave, splitter_type, otdr_type);
            if(!isPass->iIsReflectPass)
            {
                pass = 0;
            }
            else if (isPass->iIsReflectPass == 2)
            {
                pass = 2;
            }
        }
    }

    return pass;
}

/**
 * 定义用于当前SOLA采集的静态变量以及相关的互斥锁资源
 * 对于F214来说，全系统中只有一个SOLA对象，需要在开机的时候进行初始化
 * 在整个程序运行的过程中，对SOLA的一切操作，均通过下面提供的几个公有函数进行
 */
static Sola CurrSola;


/**
 * 对外提供的函数接口
 */
int CurrSola_Init()
{
    //构造sola对象
    int ret = Sola_Init(&CurrSola);

    QEvent e;
    e.sig = Q_INIT_SIG;
    e.dynamic = 0;
    
    //执行初始化转换
    QFsm_init((QFsm*)&CurrSola, &e);
    
    return ret;
}

int CurrSola_Finish()
{
    return Sola_Finish(&CurrSola);
}

int CurrSola_Start()
{
    return Sola_Start(&CurrSola);
}

int CurrSola_Stop()
{
    return Sola_Stop(&CurrSola);
}

int CurrSola_IsRunning()
{
    return Sola_IsRunning(&CurrSola);
}

int CurrSola_GetMessage(SolaMessage* status)
{
    return Sola_GetMessage(&CurrSola, status);
}

int CurrSola_GetSolaEvents(SolaEvents* dest)
{
    return Sola_GetSolaEvents(&CurrSola, dest);
}

const char* CurrSola_GetErrorString(int code)
{
    return Sola_GetErrorString(code);
}

//设置文件名
int setSolaName(SolaEvents* _this, const char *absoluteName)
{
    if(!_this || !absoluteName)
    {
        return -1;
    }

    if(_this->absolutePath)
    {
        free(_this->absolutePath);
    }

    _this->absolutePath = (char *)malloc(PATH_MAX);
    strcpy(_this->absolutePath, absoluteName);

    return 0;
}

//获取sola波形数据
SOLA_DATA* GetSolaCurveData()
{
    return solaCurveData;
}

//获取sola波形数据的波长个数
int GetSolaCurveWaveNum()
{
    return s_waveNum;
}

//销毁sola波形数据
void SolaData_Destroy()
{
    if(solaCurveData != NULL)
    {
        int i;
        for(i = 0; i < SOLA_WAVE_NUM; ++i)
        {
            SOLA_DATA* solaData = solaCurveData + i;
            solaData->count = 0;
            solaData->onePoint = 0;
            GuiMemFree(solaData->data);
        }
        GuiMemFree(solaCurveData);
    }
        
    s_waveNum = 0;
}

static void* ShowLaserIsOn(void * arg)
{
    while(1)
    {
        DrawLaserIsOn(1);
        RefreshScreen(__FILE__, __func__, __LINE__);
        MsecSleep(1000);
    }

    return NULL;
}

//开始刷新激光发射中图片传入1开始，传入0停止
static void PromptLaserIsOn(int isStart)
{
    static GUITHREAD m_thread = -1;
    static int m_isStart = 0;
    if(m_isStart == isStart)
    {
        return;
    }
    
    m_isStart = isStart;
    if(m_isStart)
    {
        ThreadCreate(&m_thread, NULL, ShowLaserIsOn, NULL);
    }
    else
    {
        ThreadCancel(m_thread);
        ThreadJoin(m_thread, NULL);
    }
    return;
}