/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmotdr_test.c
* 摘    要：  实现用于完成OTDR模块功能的应用程序接口
*
* 当前版本：  v1.0.1
* 作    者：  
* 完成日期：  
*******************************************************************************/
#include "app_frmotdr.h"

#include "app_frmotdrmessage.h"
#include "wnd_frmstandbyset.h"
#include "wnd_frmotdr.h"
#include "app_filesor.h"
#include "app_parameter.h"
#include "app_otdr_algorithm_parameters.h"
#include "app_eventsearch.h"
#include "app_inno.h"
#include "app_frmotdrtime.h"
#include "app_matfile.h"
#include "app_unitconverter.h"

extern POTDR_TOP_SETTINGS pOtdrTopSettings;	//当前OTDR的工作设置
extern volatile int iStartSpeaker;//开始测量蜂鸣器设置
extern char *cPulse[12];
extern PFactoryConfig pFactoryConf;
PCURVE_PARA pCurvePara = NULL;      //添加一个结构体，为了兼容F210的算法 added by  2015.8.10
static int iIgnoreExCheck = 0;		//1代表:忽略光纤有光检查，使测量继续
static int iIgnoreConCheck = 0;		//1代表:忽略接头检查，使测量继续
static int iRestart = 0;			//重新开始标志为:主要为了不需要发送stop命令
static FILE *fid = 0;
//保护采集队列
static GUIMUTEX mDaqMutex = PTHREAD_MUTEX_INITIALIZER;

/******************************************************************************/
//实际平均之间转化为枚举类型
static AVERAGE_TIME AvgTimeToEnum(int iAverageTime);
int EnumToWave(OPM_WAVE wave)
{
	int ret = 0;
	if (wave < ENUM_WAVE_1310NM || wave > ENUM_WAVE_1625NM)
	{
		ret = -1;
	}
	if (!ret)
	{
		if (wave == ENUM_WAVE_1310NM)
		{
			ret = 1310;
		}
		else if(wave == ENUM_WAVE_1550NM)
		{
			ret = 1550;
		}
		else
		{
			ret = 1625;
		}
	}
	return ret;
}

int EnumToPulse(OPM_PULSE pulse)
{
	int ret = 0;

	switch(pulse)
	{
	case ENUM_PULSE_5NS:
		ret = 5;
		break;
	case ENUM_PULSE_10NS:
		ret = 10;
		break;
	case ENUM_PULSE_20NS:
		ret = 20;
		break;
    #ifdef MINI2
	case ENUM_PULSE_30NS:
		ret = 30;
		break;
    #endif
	case ENUM_PULSE_50NS:
		ret = 50;
		break;
	case ENUM_PULSE_100NS:
		ret = 100;
		break;
	case ENUM_PULSE_200NS:
		ret = 200;
		break;
	case ENUM_PULSE_500NS:
		ret = 500;
		break;
	case ENUM_PULSE_1US:
		ret = 1000;
		break;
	case ENUM_PULSE_2US:
		ret = 2000;
		break;
	case ENUM_PULSE_10US:
		ret = 10000;
		break;
	case ENUM_PULSE_20US:
		ret = 20000;
		break;
	default:
		break;
	}

	return ret;
}

static void SetMatFilePulseandRange(OPM_PULSE pulse, FIBER_RANGE range);
static void OpenMatFile(OPM_WAVE wave);
static void CloseMatFile();
static void SaveMatData(TASK_SETTINGS *pTskSet, DAQ_SETTING *pDaqSet);

static void SetMatFilePulseandRange(OPM_PULSE pulse, FIBER_RANGE range)
{
	PMATFILE mat = GetMatFile();
	if (mat->iIsSave)
	{
		mat->setPulse(mat, EnumToPulse(pulse));
		mat->setRange(mat, (int)Range2Dist(range));
	}
}

static void OpenMatFile(OPM_WAVE wave)
{
	PMATFILE mat = GetMatFile();
	mat->setTestType(mat, MAT_STRUCT_OTDR);
	mat->addMatFile(mat, EnumToWave(wave));
}
static void CloseMatFile()
{
	PMATFILE mat = GetMatFile();
	mat->closeMatFile();
}

static void SaveMatData(TASK_SETTINGS *pTskSet, DAQ_SETTING *pDaqSet)
{
	PMATFILE mat = GetMatFile();
	if (mat->iIsSave)
	{
		mat->setWave(mat, EnumToWave(pTskSet->enWave));
		mat->setPulse(mat, EnumToPulse(pTskSet->enPulseTime));
		mat->setRange(mat, (int)Range2Dist(pTskSet->enFiberRange));
		mat->setBlind(mat, pTskSet->iBlindLength);
		mat->setSmpRation(mat, pTskSet->fSmpRatio);
		mat->setPoint2Distance(mat, pTskSet->fSmpIntval);
		mat->setDisplayLen(mat, pDaqSet->iDaqCount+pDaqSet->iNoiseCnt);

		mat->setRefractiveIndex(mat, pTskSet->pAlgorithm_info->SetParameters.fRefractiveIndex);
		mat->setBackScatter(mat, pTskSet->pAlgorithm_info->SetParameters.fBackScattering);
		mat->setSpliceLossThr(mat, pTskSet->pAlgorithm_info->SetParameters.fLossThreshold);
		mat->setReturnLossThr(mat, pTskSet->pAlgorithm_info->SetParameters.fReflectThreshold);
		mat->setEndLossThr(mat, pTskSet->pAlgorithm_info->SetParameters.fFiberEndThreshold);

		mat->setApd(mat, pOtdrTopSettings->pOpmSet->RegIndex[pDaqSet->iPulseKind]->pApdIndex[pDaqSet->iAtteDbClass]);
		mat->setVdd(mat, pOtdrTopSettings->pOpmSet->RegIndex[pDaqSet->iPulseKind]->pVddIndex[pDaqSet->iAtteDbClass]);
		mat->setChannel(mat, pOtdrTopSettings->pOpmSet->RegIndex[pDaqSet->iPulseKind]->pAmpIndex[pDaqSet->iAtteDbClass]);

		mat->setHighResolution(mat, (int)pOtdrTopSettings->pUser_Setting->sSampleSetting.enSmpResolution);
		mat->setData(mat, pDaqSet->pOrigData, pDaqSet->iDaqCount+pDaqSet->iNoiseCnt);

		mat->setOffset(mat, pDaqSet->iOffset);
		mat->setAtteDbClass(mat, pDaqSet->iAtteDbClass);
		mat->setTestType(mat, MAT_STRUCT_OTDR);

		mat->addMatStruct(mat);
		LOG(LOG_INFO, "-----save matlab file---\n");
	}
}

//OTDR关机释放资源
int OtdrPlatformRelease(void)
{
    int iRet = 0;
	//销毁线程

	//关闭光模块

	//释放数据空间

	return iRet;
}


int GetTaskRunSwitch(void)
{
	int iReturn;

	PTASK_CONTEXTS pTskContext = pOtdrTopSettings->pTask_Context;

	MutexLock(&pTskContext->mMutex);
	iReturn = pTskContext->iTaskRunSwitch;
	MutexUnlock(&pTskContext->mMutex);

	return iReturn;

}

void SetTaskRunSwitch(int flag)
{
	PTASK_CONTEXTS pTask_Context = pOtdrTopSettings->pTask_Context;

	MutexLock(&pTask_Context->mMutex);
	pTask_Context->iTaskRunSwitch = flag;
	MutexUnlock(&pTask_Context->mMutex);

}

int GetWorkStatus(void)
{
	int iRet;
  	PTASK_CONTEXTS pTask_Context =  pOtdrTopSettings->pTask_Context;

	MutexLock(&pTask_Context->mMutex);
	iRet = pTask_Context->enWorkStatus;
	MutexUnlock(&pTask_Context->mMutex);

	return iRet;
}

void SetWorkStatus(int iStatus)
{
  	PTASK_CONTEXTS pTask_Context =  pOtdrTopSettings->pTask_Context;

	MutexLock(&pTask_Context->mMutex);
	pTask_Context->enWorkStatus = iStatus;
	MutexUnlock(&pTask_Context->mMutex);
}

//重新设置
void ResetOtdrTime()
{
    PTASK_CONTEXTS pTask_Context =  pOtdrTopSettings->pTask_Context;
    PDISPLAY_INFO pDisplay_Info =  pOtdrTopSettings->pDisplayInfo;
    AVERAGE_TIME enAverageTime =  pOtdrTopSettings->pUser_Setting->sFrontSetting[0].enAverageTime;

    if(GetWorkStatus() == ENUM_OTDR_STOPPED)
    {
        return;
    }

    int i = 0;
    int averageTime = GetAverageTime(enAverageTime);

    if(OtdrTimeGetElapsedTime() > averageTime)
    {
        averageTime = OtdrTimeGetElapsedTime();
        enAverageTime = AvgTimeToEnum(averageTime);
    }

    for(i = pTask_Context->iCurTaskPos; i < pTask_Context->iTaskNum; i++)
    {
        if(i == pTask_Context->iCurTaskPos)
        {
            if(!OtdrTimeTimeOut())
            {
                pTask_Context->Task_SetQueue[i].enAverageTime = enAverageTime;
                pTask_Context->Task_SetQueue[i].iAverageTime = averageTime;
                pDisplay_Info->pCurve[i]->pParam.enTime = enAverageTime;
                pDisplay_Info->pCurve[i]->pParam.uiAvgTime = averageTime;

                OtdrTimeSet(averageTime);
            }
        }
        else
        {
            pTask_Context->Task_SetQueue[i].enAverageTime = enAverageTime;
            pTask_Context->Task_SetQueue[i].iAverageTime = averageTime;
            pDisplay_Info->pCurve[i]->pParam.enTime = enAverageTime;
            pDisplay_Info->pCurve[i]->pParam.uiAvgTime = averageTime;
        }
    }
}
//清空组合参数
int ClearCombinePara(COMBINE_PARA* pCombinePara)
{
    //pCombinePara->pData;
    //pCombinePara->pOriData;
    //上述两个缓冲区是预先分配好的，在此处不进行操作

    //此处对结构体内地三个元素以后的部分进行清空
    memset(
        &pCombinePara->iSumNum,
        0,
        sizeof(COMBINE_PARA) - sizeof(pCombinePara->pData) - sizeof(pCombinePara->pOriData) - sizeof(pCombinePara->pLogData)
        );

    return 0;
}


//每次开始任务之前对combineinfo中的必要数据进行恢复初始值
//注意:combineinfo中包含部分预先分配好的数据缓冲区，在此并不会清空
int ClearCombineInfo(COMBINE_INFO *pCombineInfo)
{
    //pCombineInfo->pCombineData;
    //pCombineInfo->pOriginalData;
    //上述两个缓冲区是预先分配好的，在此处不进行操作

    int i;
    //初始化所有的组合参数
    for(i = 0; i < MAX_DAQ_NUM; ++i) {
        ClearCombinePara(&pCombineInfo->combine_para[i]);
    }

    //清空组合位置信息
    memset(pCombineInfo->combine_pos, 0, sizeof(COMBINE_POS));
	pCombineInfo->iApdBreakFlag = 0;
	pCombineInfo->iApdAdjustFlag = 0;
	pCombineInfo->iPulseWidth = 0;
	pCombineInfo->iSigLen = 0;
	pCombineInfo->iBlind = 0;
	pCombineInfo->iDataNum = 0;
	pCombineInfo->iTotalNum = 0;
	pCombineInfo->iTaskCycle = 0;
	pCombineInfo->iCombineFlag = 0;
	pCombineInfo->iRealTimeFlag = 0;
	pCombineInfo->fSampleRating = 0.0f;

    return 0;
}

//OTDR主控制线程
void *OtdrCtrlThread(void *pThreadArg)
{
	//返回值
	static int iReturn = 0;
	OTDR_COMMAND Command;
	POTDR_TOP_SETTINGS pOtdrTopSettings = (POTDR_TOP_SETTINGS)pThreadArg;
	PTASK_CONTEXTS pTask_Context = NULL;

    DBG_ENTER();

	pTask_Context = pOtdrTopSettings->pTask_Context;
	InitCommandQueue(pOtdrTopSettings->pComQueue);
    pOtdrTopSettings->iOtdrExitFlag = 0;
    SetWorkStatus(ENUM_OTDR_STOPPED);

	while (pOtdrTopSettings->iOtdrExitFlag == 0)
	{
	    Command = (OTDR_COMMAND)ReadCommand(pOtdrTopSettings->pComQueue);
	    //判断OTDR是否接收到命令
		if (Command != ENUM_NO_COMMAND)
	    {
			iRestart = 0;
	        //对接收到的命令进行操作
	        switch (Command)
	        {
	            case ENUM_REALT_KEY_UP:
					iIgnoreExCheck = 0;
					iIgnoreConCheck = 0;
	                if (GetWorkStatus() == ENUM_OTDR_STOPPED)
	                {
						SetTaskRunSwitch(OTDR_TASK_START);
						WriteOTDRMsgQueue(ENUM_OTDRMSG_START);
	                }
	                else
	                {
	                    SetTaskRunSwitch(OTDR_TASK_STOP);
	                }
	                break;
	            case ENUM_AVG_KEY_UP:
					iIgnoreExCheck = 0;
					iIgnoreConCheck = 0;
	                if (GetWorkStatus() == ENUM_OTDR_STOPPED)
	                {
	                    SetTaskRunSwitch(OTDR_TASK_START);
	                }
	                else
	                {
	                    SetTaskRunSwitch(OTDR_TASK_STOP);
	                }
	                break;
				case ENUM_IGNORE_EX_CHECK:
					{
						iIgnoreExCheck = 1;
						LOG(LOG_INFO,"iIgnoreExCheck--------\n");
						SetTaskRunSwitch(OTDR_TASK_START);
					}
					break;
				case ENUM_IGNORE_CON_CHECK:
					{
						iIgnoreConCheck = 1;
						LOG(LOG_INFO, "iIgnoreConCheck--------\n");
						SetTaskRunSwitch(OTDR_TASK_START);
					}
					break;
				case ENUM_RESTART:
					iRestart = 1;
					SetTaskRunSwitch(OTDR_TASK_STOP);
					break;
				case ENUM_STOP_BUTTON_UP:
					SetTaskRunSwitch(OTDR_TASK_STOP);
					break;
				case ENUM_RESET_TIME:
				    ResetOtdrTime();
				break;
	            default :
	                break;
	        }
	    }
		MsecSleep(100);
	}

	ThreadExit(&iReturn);

	return &iReturn;
}


//启动计时
void StartTimeCnt(int iStartTime, int iIsRT)
{
	OtdrTimeInit();

	if (iIsRT)
	{
		SendCurveAdditionCmd(EN_ADD_MEATIME, 1);
		OtdrTimeSet(0);
	}
	else
	{
        OtdrTimeSet(iStartTime);
	}
    OtdrTimeStart();
}


//停止计时
void StopTimeCnt(int iIsRT)
{
	if (iIsRT)
	{
		SendCurveAdditionCmd(EN_ADD_MEATIME, -1);
	}

    OtdrTimeStop();
}


//报告otdr测量进度
int ReportOtdrProgress(GUICHAR *pStrProgress)
{
	int iErr = 0;
	GUILABEL *pLbl;
	GUIPICTURE *pBg;
	CURVE_ADD *pAdd;

	if (NULL != pStrProgress)
	{
		pBg = CreatePicture(220, 87, 200, 30, BmpFileDirectory "otdr_addition.bmp");
		pLbl = CreateLabel(220, 94, 200, 16, pStrProgress);
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLbl);
		SetLabelAlign(GUILABEL_ALIGN_CENTER, pLbl);
		pAdd = (CURVE_ADD *)malloc(sizeof(CURVE_ADD));
		if (NULL == pAdd)
		{
			free(pStrProgress);
			return -1;
		}
		pAdd->pLblAddtion = (GUIVISIBLE *)pLbl;
		pAdd->pBgAddtion = (GUIVISIBLE *)pBg;
		free(pStrProgress);
	}
	else
	{
		pAdd = NULL;
	}

	//发送附加消息
	iErr = SendCurveAdditionCmd(EN_ADD_ADDTION, (INT32)pAdd);
	MsecSleep(50);

	return iErr;
}

void setDpMode(PTASK_CONTEXTS pTskContext, int mode)
{
    MutexLock(&(pTskContext->mMutex));
    pTskContext->enDpMode = mode;
    MutexUnlock(&(pTskContext->mMutex));
}

int getDpMode(PTASK_CONTEXTS pTskContext)
{
    int mode;
    MutexLock(&(pTskContext->mMutex));
    mode = pTskContext->enDpMode;
    MutexUnlock(&(pTskContext->mMutex));
    return mode;
}

void setDpState(PTASK_CONTEXTS pTskContext, int state)
{
    MutexLock(&(pTskContext->mMutex));
    pTskContext->enDpState= state;
    MutexUnlock(&(pTskContext->mMutex));
}

int getDpState(PTASK_CONTEXTS pTskContext)
{
    int state;
    MutexLock(&(pTskContext->mMutex));
    state = pTskContext->enDpState;
    MutexUnlock(&(pTskContext->mMutex));
    return state;
}

//数据采集线程
void *DataAcquisitionThread(void *pThreadArg)
{
	//返回值
	static int iReturn = 0;

	//初始化局部变量
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;
	PTASK_CONTEXTS pTaskContext = pOtdrTopSet->pTask_Context;
    PUSER_SETTINGS pUserSet = pOtdrTopSet->pUser_Setting;

	//光模块信息
	POPMSET pOpmSet = pOtdrTopSet->pOpmSet;

	AllocTaskContext(pTaskContext);
	while (!pOtdrTopSet->iOtdrExitFlag)
	{
		int iErr = 0;
		PTASK_SETTINGS pTaskSet = NULL;
    	PDAQ_SETTING pDaqSet = NULL;

		//检查otdr采集模块启动命令
	    if (!GetTaskRunSwitch())
	    {
	    	MsecSleep(20);
			continue;
	    }

		//清除之前所有的曲线
		ClearAllCurve();
        // //清空事件列表
        // WriteOTDRMsgQueue(ENUM_OTDRMSG_CLEAREVENT);
        
		ReportOtdrProgress(NULL);

		//需要判断是否强制停止
        if (!GetTaskRunSwitch())
        {
			goto OtdrFailed;
		}
#ifdef OPTIC_DEVICE
		//板子FPGA初始化
		LOG(LOG_INFO, "start BoardWorkInit\n");
		iErr = BoardWorkInit(pOtdrTopSet->pOpmSet, 1);
		if (iErr)
		{
            ReportOtdrProgress(TransString("Board Initialize Failed!"));
            goto OtdrFailed;
		}
#endif
#ifndef TUNE_ATT
		//自整定
		if(pOpmSet->pOpmTuning->arrAttValue[0] == 1 && pOpmSet->pOpmTuning->arrAttValue[1] == 1)
		{
			ReportOtdrProgress(TransString("Auto-tuning…"));
			if(TuneAtt() != 0)
				goto OtdrStop;
		}
#else
        TuneAtt();
        goto OtdrStop;
#endif
		ReportOtdrProgress(NULL);
		//需要判断是否强制停止
        if (!GetTaskRunSwitch())
        {
			goto OtdrFailed;
		}
        iStartSpeaker = 1;

		//提示激光器发光中
// 		DrawLaserIsOn(0);
		ReportOtdrProgress(NULL);
		//光纤有光检查
		LOG(LOG_INFO,"start CheckFiberLight\n");
        if((pUserSet->sCommonSetting.iLightCheckFlag)
				&& (GetCurrWindowType() == ENUM_OTDR_WIN))
		{
			ReportOtdrProgress(GetCurrLanguageText(OTDR_LBL_CHECAK_TRANSMISSION));
			if(!iIgnoreExCheck)
			{
#ifdef OPTIC_DEVICE
				iErr = CheckFiberLight(pOtdrTopSet);
				if(iErr)
				{
					LOG(LOG_ERROR, "light iRet = %d\n", iErr);
					WriteOTDRMsgQueue(ENUM_OTDRMSG_LIGHTCHECK);
					goto OtdrStop;//暂时注释，测试使用
				}
#endif
			}
		}

		ReportOtdrProgress(NULL);
#ifdef OPTIC_DEVICE
		//击穿电压检测
		LOG(LOG_INFO,"start CheckBreakVoltage\n");
		iErr = CheckBreakVoltage(pOtdrTopSet);
        if (iErr)
		{
			ReportOtdrProgress(TransString("Data Error!"));
			goto OtdrFailed;
		}
#endif
        /*
		if(!iIgnoreExCheck)//有光检查忽略的话就会跳过击穿电压检测
		{
			LOG(LOG_INFO,"start CheckBreakVoltage\n");
			//printf("--------CheckBreakVoltage 1--------\n");
			//ReportOtdrProgress(GetCurrLanguageText(OTDR_CHECK_BREAK_VOL));
	        iErr = CheckBreakVoltage(pOtdrTopSet);
			//printf("--------CheckBreakVoltage 2--------\n");
			if (iErr)
			{
				ReportOtdrProgress(TransString("ERROR: DATA Error!"));
				goto OtdrFailed;
			}
		}
        */
		ReportOtdrProgress(NULL);
		//需要判断是否强制停止
        if (!GetTaskRunSwitch())
        {
			goto OtdrFailed;
		}
        //光纤端面反射率检测
		LOG(LOG_INFO,"start CheckFiberEndFace\n");
		if((pUserSet->sCommonSetting.iConectCheckFlag)
				&& (GetCurrWindowType() == ENUM_OTDR_WIN))
		{
			ReportOtdrProgress(GetCurrLanguageText(OTDR_LBL_CHECKING_CONNECTOR));

			//sola测试使用
			//CheckEndFaceFlag = CheckFiberEndFace(pOtdrTopSet);
			if(!iIgnoreConCheck)
			{
#ifdef OPTIC_DEVICE
				iErr = CheckFiberEndFace(pOtdrTopSet);
				if(iErr)
				{
					LOG(LOG_ERROR, "fiber face iRet = %d\n", iErr);
					WriteOTDRMsgQueue(ENUM_OTDRMSG_CONNECTCHECK);
					goto OtdrStop;
				}
#endif
			}
		}

		ReportOtdrProgress(NULL);
		//需要判断是否强制停止
        if (!GetTaskRunSwitch())
        {
			goto OtdrFailed;
		}

        //初始化任务上下文
        iErr = InitTaskContext(pOtdrTopSet);
		if (iErr)
		{
			ReportOtdrProgress(NULL /*初始化任务失败*/);
			goto OtdrFailed;
		}

        //需要判断是否强制停止
        if (!GetTaskRunSwitch())
        {
			goto OtdrFailed;
		}
        
        //上述各项准备完成后，开始进行数据采集;
		WriteOTDRMsgQueue(ENUM_OTDRMSG_START);
		SetWorkStatus(ENUM_OTDR_START);

		//APP_LOG("TIME start");
        for(;pTaskContext->iCurTaskPos < pTaskContext->iTaskNum;pTaskContext->iCurTaskPos++)
        {
			//初始化当前曲线的文件名
			WriteOTDRMsgQueue(ENUM_OTDRMSG_FILENAME);
			//清空事件列表（解决多波长测试事件列表信息显示bug）
			WriteOTDRMsgQueue(ENUM_OTDRMSG_CLEAREVENT);
			//分配任务所需缓冲区
            pTaskSet = &pTaskContext->Task_SetQueue[pTaskContext->iCurTaskPos];
      		AllocDataBuffer(pTaskSet);
#ifdef OPTIC_DEVICE
			DEVFD *pDevFd = GetGlb_DevFd();
			int iOpmFd = pDevFd->iOpticDev;
			//设置激光器波长
		    Opm_SetWave(iOpmFd, pOpmSet, pTaskSet->enWave);
#endif
			MsecSleep(150);
			//同步参数
			MutexLock(&pUserSet->mDataMutex);
			pUserSet->enWaveCurPos = pTaskSet->enWave;
			MutexUnlock(&pUserSet->mDataMutex);

			WriteOTDRMsgQueue(ENUM_OTDRMSG_WAVWARGS);
           	//启动数据处理(DP)线程并开始计时
            pTaskContext->enDpMode = ENUM_DP_COMBINE;

			//ReportOtdrProgress(GetCurrLanguageText(OTDR_RUNNING));
			//剩余时间赋值
            StartTimeCnt(pTaskSet->iAverageTime, pTaskSet->iRealTimeFlag);

			//初始化任务指针为0
			pTaskSet->iCurDaqPos = 0;
#ifdef SAVE_DATA
			int icnt = 0;
#endif

#ifdef SAVE_DATA
			char buff[512];
			sprintf(buff,"mkdir 777 "MntUsbDirectory"/O10_waveLen_%d_range_%d_pulse_%d_autoRange_%f",
				    pTaskSet->enWave,pTaskSet->enFiberRange,pTaskSet->enPulseTime,pTaskSet->fAutoRange);
			mysystem(buff);
#endif
			SetMatFilePulseandRange(pTaskSet->enPulseTime, pTaskSet->enFiberRange); // 添加保存数据的脉宽和量程参数
			OpenMatFile(pTaskSet->enWave);
			//对采集队列进行循环采集，直到时间用完

			//int CombineiFlag = 0;
            while ((OTDR_TASK_STOP != GetTaskRunSwitch()) && (!OtdrTimeTimeOut()))
            {
            	/*printf("&&&&&&&pTaskSet->pCombine_Info->iTaskCycle = %d\n", pTaskSet->pCombine_Info->iTaskCycle);
            	if (pTaskSet->pCombine_Info->iTaskCycle > 0 && CombineiFlag == 0 && pTaskSet->pCombine_Info->iTotalNum >= 1)
        		{
        			int i = 0;
					for (i = 0; i < pTaskSet->pCombine_Info->iTotalNum; i++)
					{
						pTaskSet->Daq_SetQueue[i] = pTaskSet->Daq_SetQueue[pTaskSet->pCombine_Info->ReChanIndex[i]];
						pTaskSet->Daq_Queue.pDaq_SetQueue[i] = pTaskSet->Daq_Queue.pDaq_SetQueue[pTaskSet->pCombine_Info->ReChanIndex[i]];
					}
					CombineiFlag = 1;
					pTaskSet->iDaqNum = pTaskSet->pCombine_Info->iTotalNum;
					pTaskSet->iCurDaqPos = 0;
					pDaqSet->iNumInTask = 0;
        		}*/
				//检测到总电源掉电，手动上电
				if (!Opm_OpenPwr(pOpmSet))
				{
					LOG(LOG_ERROR, "OTDR test PWR is down!!!\n");
					iStartSpeaker = 1;
				}
                pDaqSet = &pTaskSet->Daq_SetQueue[pTaskSet->iCurDaqPos];
#ifdef OPTIC_DEVICE
				iErr = DaqStart(pDaqSet);//开始数据采集
#endif


#ifdef SAVE_DATA
				char strPath[512];
				sprintf(strPath, MntUsbDirectory"/O10_waveLen_%d_range_%d_pulse_%d_autoRange_%f/",
					    pTaskSet->enWave,pTaskSet->enFiberRange,pTaskSet->enPulseTime,pTaskSet->fAutoRange);
                SaveAdcData(pTaskSet,pTaskSet->iCurDaqPos,icnt,strPath);
#endif
				SaveMatData(pTaskSet, pDaqSet);
                //将采集到数据发送到数据采集队列中，若队列已满，则休眠等待
                while(GetTaskRunSwitch())
                {
                    if(WriteDaqQueue(&pTaskSet->Daq_Queue, pDaqSet))
                        MsecSleep(10);//等待100ms再重新发送
                    else
                        break;
                }

                //需要判断是否强制停止
                if(!GetTaskRunSwitch())
                    break;
                pTaskSet->iCurDaqPos = (pTaskSet->iCurDaqPos + 1) % pTaskSet->iDaqNum;
#ifdef SAVE_DATA
				if(pTaskSet->iCurDaqPos == 0)
				icnt++;
#endif
            }
			CloseMatFile();
			//停止计时
			StopTimeCnt(pTaskSet->iRealTimeFlag);

			//停止通道组合并等待数据处理完毕
			setDpMode(pTaskContext,ENUM_DP_STOP);
			while (ENUM_DPSTATE_WAITE != getDpState(pTaskContext))
			{
				setDpMode(pTaskContext, ENUM_DP_STOP);
				MsecSleep(10);
			}

            //进行事件分析
            if (!iRestart)
			{
				ReportOtdrProgress(GetCurrLanguageText(OTDR_LBL_EVENAT_ANALYZING));
	            EventAnalyze(pTaskContext->Task_SetQueue, pTaskContext->iCurTaskPos);
				ReportOtdrProgress(NULL);
				WriteOTDRMsgQueue(ENUM_OTDRMSG_EVENTOK);
			}

			//释放采集数据空间
            FreeDataBuffer(pTaskSet);

            //需要判断是否强制停止
            if (!GetTaskRunSwitch())
                break;
        }

        if (!pTaskSet->iRealTimeFlag)
			pTaskContext->iCurTaskPos--;

	OtdrFailed:
		if (!iRestart)
		{
			//曲线铺满屏幕
		    //SetCurvesFullScreen();
			//WriteOTDRMsgQueue(ENUM_OTDRMSG_STOP);
			//SendCurveAdditionCmd(EN_ADD_END, 1);
            //发送采集结束的信号到绘图线程
            SendLineCtlMoveMsg(EN_CTL_ENDOFDAQ, 0, 0);
			//DATA_ACQUI_DEBUG("EN_CTL_ENDOFDAQ Signal emited\n");
		}

	OtdrStop:
        //结束测量任务
        iStartSpeaker = 1;
       	WorkFinish(pOtdrTopSet->pOpmSet);
		SetWorkStatus(ENUM_OTDR_STOPPED);
		SetTaskRunSwitch(OTDR_TASK_STOP);
	}

	ThreadExit(&iReturn);
	return &iReturn;
}

//数据处理线程
void *DataProcessingThread(void *pThreadArg)
{
	//返回值
	static int iReturn = 0;

	PTASK_CONTEXTS pTaskContext = pOtdrTopSettings->pTask_Context;
	PTASK_SETTINGS pTaskSet = NULL;
	COMBINE_INFO *pCombInfo = NULL;
    PDAQ_SETTING pDaqSet = NULL;

    //为了兼容F210的算法，添加该代码added by  2015.8.10
	if (NULL == pCurvePara)
	{
		pCurvePara = (PCURVE_PARA)calloc(1, sizeof(CURVE_PARA));
		pCurvePara->iSOLAMode = 0;
	}

	//生成对数数据表格，以查表法进行log计算
	LogTableCreat2();

	DBG_ENTER();
	while (pOtdrTopSettings->iOtdrExitFlag == 0)
	{
	    if (GetWorkStatus()!= ENUM_OTDR_START)
	    {
	        MsecSleep(50);
	        continue;
	    }

	    switch (getDpState(pTaskContext))//通过状态机判断信号如何处理
	    {
        case ENUM_DPSTATE_WAITE:
            if(getDpMode(pTaskContext) == ENUM_DP_COMBINE)
            {
                setDpState(pTaskContext, ENUM_DPSTATE_COMBINE);
                pTaskSet = &pTaskContext->Task_SetQueue[pTaskContext->iCurTaskPos];
                pCombInfo = pTaskSet->pCombine_Info;
            }
            else
            {
                setDpState(pTaskContext, ENUM_DPSTATE_WAITE);
                MsecSleep(50);
            }
            break;
        case ENUM_DPSTATE_COMBINE:
            //对数据采集队列进行查询，读取数据进行处理
            pDaqSet = ReadDaqQueue(&pTaskSet->Daq_Queue);

			//若采集队列为空，则需要等待
            if (pDaqSet == NULL)
            {
				MsecSleep(20);
            }
			else
			{
		        //进行通道组合
		        pCombInfo->iDataNum = pDaqSet->iNumInTask + 1;

				//added by  2017.6.9,得到背向散射曲线的起始值
				if (pTaskSet->fSmpRatio >= 1.0f)
				{
					GetBackScatterInitialValue(pTaskSet, pDaqSet);
				}

				//拷贝采集数据有效部分到组合缓冲区
				memcpy(pCombInfo->pOriginalData,
					   pDaqSet->pOrigData + pDaqSet->iOffset,
					   pCombInfo->iSigLen * sizeof(UINT16));

				if (pCombInfo->iDataNum % pTaskSet->iDaqNum == 0)
				{
					pCombInfo->iCombineFlag = 1;
				}
				else
				{
					pCombInfo->iCombineFlag = 0;
				}

				//开始组合数据
				SignalCombine(pCombInfo);

		        //数据处理完成后，需判断是否需要画图
		        //if (1 == pCombInfo->iCombineFlag)
		    	{
					CombData2DispInfo(pTaskContext, pCombInfo);

		        }
			}

			//检查是否要停止组合
            if (getDpMode(pTaskContext)  == ENUM_DP_STOP)
            {
                setDpState(pTaskContext, ENUM_DPSTATE_WAITE);
            }
            break;

        default:
            setDpMode(pTaskContext, ENUM_DP_STOP);
            setDpState(pTaskContext, ENUM_DPSTATE_WAITE);
            break;
	    }
	}

	ThreadExit(&iReturn);

	return &iReturn;
}

void GetBackScatterInitialValue(PTASK_SETTINGS pTskSet, PDAQ_SETTING pDaqSet)
{
	int iTemp_Value = 0;
	UINT16 uTemp_Value = pDaqSet->pOrigData[pDaqSet->iOffset-1];

	iTemp_Value = (int)(1.5 * pTskSet->fSmpRatio);

	if (pTskSet->fSmpRatio >= 6.0f)
	{
		uTemp_Value = GetMeanUint16(pDaqSet->pOrigData + pDaqSet->iOffset - iTemp_Value - 4, 8);
	}
	else
	{
		uTemp_Value = pDaqSet->pOrigData[pDaqSet->iOffset - iTemp_Value];
	}
	pDaqSet->pOrigData[pDaqSet->iOffset] = uTemp_Value;

	UINT16 iTmp = GetMaxUint16(pDaqSet->pOrigData + pDaqSet->iOffset, 3);
	pDaqSet->pOrigData[pDaqSet->iOffset] = iTmp;
}


//硬件工作初始化;

int BoardWorkInit(POPMSET pOpmSet, int iDisplayTip)
{
    DEVFD *pDevFd = NULL;
	int iOpmFd;
	int iRet;
	// UINT32  uiTemp;
	// int i;

	DBG_ENTER();

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;
	Opm_EnablePwr(iOpmFd,pOpmSet);		//打开光模块电源
	//使能光模块电源后需等待稳定
	MsecSleep(200);

	//打开正负5v控制电源
	Opm_EnablePwrCtl(iOpmFd, pOpmSet, C_5V_OPM_CTL);
	Opm_EnablePwrCtl(iOpmFd, pOpmSet, _C_5V_OPM_CTL);

	Opm_ClkTurnOn(iOpmFd,pOpmSet);		//打开FPGA的时钟
	MsecSleep(10);

	Opm_ResetFpgaDdr2(iOpmFd);			//对FPGA接的DDR2进行复位
	MsecSleep(10);

    //打开ADC 1.8v控制电源
    Opm_EnableAdcVoltage(iOpmFd, pOpmSet);
    iRet=Opm_EnableAdc(iOpmFd,pOpmSet);	//打开ADC时钟，以及数据输出等,adc上共三个量

    if(!iRet)
    {
        iRet = Opm_GetAdcStatue(iOpmFd);							//得到ADC的状态,返回0为异常
        iRet=!iRet;													//因为0=是不正常工作，所以取反
        if(iRet)
            LOG(LOG_ERROR, "ADC don't work\r\n");
    }

	if(!iRet)
	{
		//设置FpgaCtrlOut寄存器
		Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &(pOpmSet->pCurrReg->uiFpgaCtrlOut));
	    MutexLock(&(pOpmSet->mOPMSetMutex));
		pOpmSet->pCurrReg->uiFpgaCtrlOut |= (OPM_24LC_SDA|OPM_24LC_CLK|OPM_24LC_WP | OPM_ADC3V_POWCON | OPM_ADC_PWR_EN);
		//设置温度
		pOpmSet->pCurrReg->uiFpgaCtrlOut |= (OPM_LM75_OS | OPM_LM75_SCL | OPM_LM75_SDA);
		Opm_SetFpgaCtrlOut(iOpmFd,pOpmSet);
		MutexUnlock(&(pOpmSet->mOPMSetMutex));
	}

	iRet = Opm_SetIntp(iOpmFd, pOpmSet, INTP01_01);
	Opm_SetPhase(iOpmFd);


	DBG_EXIT(iRet);
	return iRet; //0=正常工作
}

//光纤有光检查
int CheckFiberLight(POTDR_TOP_SETTINGS pOtdrTopSettings)
{
    DEVFD *pDevFd = NULL;
    int iOpmFd;
    int iRet = 0;
	int iApdIndex,iVddIndex,iAmpIndex;
    int iApdVolt,iBlackLevel;
    float fStd;
	POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	POPM_TUNING pOpmTunning = pOpmSet->pOpmTuning;

	DBG_ENTER();

    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

	//有算法组来决定修改下面三个值
	iApdIndex = 3;
	iVddIndex = 0;
	iAmpIndex = 5;

	//采集数据必须，设置APD  VDD  ATT  AMP
    iApdVolt = GetApd_SetValue(pOpmTunning, iApdIndex);
    Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);			//设置APD 的反偏电压;
	Opm_SetVddVoltage(iOpmFd, pOpmSet, pOpmTunning->iVddValue[iVddIndex]);
	Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
	Opm_SetAmp(iOpmFd, pOpmSet, pOpmSet->AmpCode[iAmpIndex]);

/*
	int i = 0;
	unsigned int att0 = pOpmTunning->arrAttValue[iAmpIndex];
	for (; i < 100; ++i) 
	{
		iBlackLevel = GetBlack_Level(pOpmSet, &fStd);

		//printf("---%s---iBlackLevel = %d, att0 = %d\n", __func__, iBlackLevel, att0);
		if (iBlackLevel > 60000 && iBlackLevel < 64000)
		{
			break;
		}
		else
		{
			if (iBlackLevel < 60000) 
			{
				att0 += 10;
			}
			else
			{
				att0 -= 6;
			}
		}
		if (att0 < 100 || att0  > 0xff0)
		{
			Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
			break;
		}

		//Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
		Opm_SetAttValue(iOpmFd, pOpmSet, att0);
	}
*/

    //通过黑电平判断光纤内是否有光存在;
	iBlackLevel = GetBlack_Level(pOpmSet, &fStd);

	if (iBlackLevel < 45000)
	{
		iRet = 1;
		LOG(LOG_ERROR,"iBlackLevel = %d\n",iBlackLevel);
	}
	LOG(LOG_INFO,"BlackLevel = %d\n",iBlackLevel);
    DBG_EXIT(iRet);
    return iRet;	//0=没有光，1=有光
}

//击穿电压检测
int CheckBreakVoltage(POTDR_TOP_SETTINGS pOtdrTopSettings)
{
    DEVFD *pDevFd = NULL;
    int iOpmFd;
    int iDelta;
    int i = 0, iRet = 0,downflag = 0,upflag = 0;
    int iApdVolt,iBasicVol;
    float fStd,fBasicStd;
	int iBlackLevel;
	int iApdIndex,iVddIndex,iAmpIndex;
	float fTemperature = 0.0f;

	POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	POPM_TUNING pOpmTunning = pOpmSet->pOpmTuning;


	DBG_ENTER();
    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

	iAmpIndex = 3;//205 3
	iApdIndex = 3;
	iVddIndex = 0;

	iApdVolt = GetApd_SetValue(pOpmTunning, iApdIndex);
    Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);
	Opm_SetVddVoltage(iOpmFd, pOpmSet, pOpmTunning->iVddValue[iVddIndex]);
	Opm_SetAmp(iOpmFd, pOpmSet, pOpmSet->AmpCode[iAmpIndex]);
	Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
	MsecSleep(200);

	// 温度大于60摄氏度，步长变大
	GetOpmTemperature(pDevFd->iOpticDev, &fTemperature);       //!< 得到温度

    // 高温情况下K系数(一次线性拟合不准确)故步长较大
	//int TemperatureOffset = GetAPDStep(fTemperature);

	//采集黑电平
	iBlackLevel = GetBlack_Level(pOpmSet, &fBasicStd);

	unsigned int att0 = pOpmTunning->arrAttValue[iAmpIndex];
	for (i = 0; i < 100; ++i) 
	{
		iBlackLevel = GetBlack_Level(pOpmSet, &fStd);

		if (iBlackLevel > 60000 && iBlackLevel < 63000)
		{
			fBasicStd = fStd;
			pOpmTunning->arrAttValue[iAmpIndex] = att0;
			break;
		}
		else
		{
			if (iBlackLevel < 60000) 
			{
				att0 += 10;
			}
			else
			{
				att0 -= 6;
			}
		}
		if (att0 < 100 || att0  > 0xff0)
		{
			Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
			break;
		}

		Opm_SetAttValue(iOpmFd, pOpmSet, att0);
	}

	if (iBlackLevel < 50000)
	{
		iRet = 1;
		LOG(LOG_INFO,"iBlackLevel = %d\n", iBlackLevel);
		LOG(LOG_INFO, "CheckBreakVoltage Failed, please check ATT0!!!\n");
		return iRet;
	}
	LOG(LOG_INFO,"BlackLevel = %d\n", iBlackLevel);

    //击穿电压调整
    iApdVolt = GetApd_SetValue(pOpmTunning, 0);
    iBasicVol = iApdVolt;

    int step_add = 10;
    int step_dec = 6;
	// static int tmpCount = 0;
	i = 0;
    do//对APD击穿电压在+/-400范围内进行微调
    {
        Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);//第一次设置击穿电压时，由于压差较大，延时时间应增大
    	MsecSleep(200);
        iBlackLevel = GetBlack_Level(pOpmSet, &fStd);
		/*
		//因为采集数据错误，多加一些判断，防止击穿电压判断错误
		if ((fStd > 20 * fBasicStd) && (fStd < 25 * fBasicStd))
		{
			tmpCount++;
			if (tmpCount < 5)
			{
				continue;
			}
			else
			{
				tmpCount = 0;
            	break;
			}
		}
		else
		{
			tmpCount = 0;
		}

		if (((fStd > 1000) && (fStd < 4100)) || ((fStd > 10000) && (iBlackLevel < 50000)))
		{
			fStd = 0.0f;
			continue;
		}

		if (fStd > 1000.0f && iApdVolt < 0x980 && iBlackLevel > 58000)
		{
			fStd = 0.0f;
			continue;
		}
			
		*/
        //LOG(LOG_INFO,"iApdVolt = 0x%x, fBasicStd = %f, fStd = %f,iBlackLevel = %d\n",iApdVolt,fBasicStd,fStd,iBlackLevel);
        if((fStd > 10 * fBasicStd) || (iBlackLevel < 45000))
        {
            upflag = 1;
            if(downflag == 0)
            {
                //iApdVolt = iApdVolt - 25 - TemperatureOffset;
                iApdVolt = iApdVolt - step_dec;
            }
            else
            {
                iApdVolt = iApdVolt - step_add;
                //iApdVolt = iApdVolt - 4 - TemperatureOffset;
                break;
            }
        }
        else if (fStd < 3 * fBasicStd)
        {
            downflag = 1;
            if(upflag == 0)
            {
                //iApdVolt = iApdVolt + 25 + TemperatureOffset;
                iApdVolt = iApdVolt + step_add;
            }
            else
            {
                //iApdVolt = iApdVolt + 4 + TemperatureOffset;
                break;
            }
        }
        else
        {
            break;
        }
        i++;
    }while(i < 50);
	//设置的第一档为击穿电压，所以要减去摸个经验值
	#ifdef MINI2
    iDelta = iApdVolt - iBasicVol - 0x30;
    #else
    //iDelta = iApdVolt - iBasicVol - 0x20;
    iDelta = iApdVolt - iBasicVol - 0x10;
    #endif

	int temp;
    LOG(LOG_INFO, "iDelta = %d, iApdVolt = 0x%x\n", iDelta, iApdVolt);
	LOG(LOG_INFO, "Congratulations to you on your successful CheckBreakVoltage!!!\n");

	for(i=0; i<OPM_APD_KIND; i++)
    {
    	temp = (int)pOpmTunning->iApdVoltageSet[i];
        if(temp + iDelta > 0)//避免出现小于0的情况;
        {
            pOpmTunning->iApdVoltageSet[i] = pOpmTunning->iApdVoltageSet[i] + iDelta;
            LOG(LOG_INFO, "APD[%d] = 0x%x \n", i, pOpmTunning->iApdVoltageSet[i]);
        }
		else
		{
			pOpmTunning->iApdVoltageSet[i] = pOpmTunning->iApdVoltageSet[i] + iDelta;
		}
		LOG(LOG_INFO, "ApdVoltage[%d] = %d\n", i, pOpmTunning->iApdVoltageSet[i]);
	}
    DBG_EXIT(iRet);
    return iRet;
}
//检查光纤端面反射率
int CheckFiberEndFace(POTDR_TOP_SETTINGS pOtdrTopSettings)
{
	int iRet=0;
    PDAQ_SETTING pDaq_Setting = NULL;
	DBG_ENTER();
	if(pOtdrTopSettings == NULL)    return -1;
	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;
	POTDR_TOP_SETTINGS pOtdrTopSet = pOtdrTopSettings;
	POPMSET pOpmSet = pOtdrTopSet->pOpmSet;
    pDaq_Setting = (PDAQ_SETTING)calloc(1,sizeof(DAQ_SETTING));//为采集设置申请空间
    pDaq_Setting->pOrigData = (UINT16 *)calloc(1, MAX_DATA * sizeof(UINT16));
   	pDaq_Setting->iPulseKind = (UINT32)ENUM_PULSE_100NS; //ENUM_PULSE_1US->ENUM_PULSE_100NS 2019.3.13
   	pDaq_Setting->iAtteDbClass = (UINT32)ENUM_5_0DB;//该处衰减量较大，只需探测较大的端面反射    盲区过大，调整为小脉宽
    pDaq_Setting->iFilter = (UINT32)_IIR_256;
    pDaq_Setting->iDaqCount = MAX_DATA - BLACK_LEVEL_LEN;
	pDaq_Setting->iOffset = 15;
	pDaq_Setting->iSmpReg = INTP01_01;
	pDaq_Setting->fSmpRatio = 1.0f;
	pDaq_Setting->iNoiseCnt = BLACK_LEVEL_LEN;
	Opm_SetWave(iOpmFd, pOpmSet, pOtdrTopSet->pUser_Setting->enWaveCurPos);
    iRet = DaqStart(pDaq_Setting);

	//#define CHECKENDFACE
	#ifdef CHECKENDFACE
	int i = 0;
	FILE *fp;
	static int checkendfaceNum = 0;

	char strFile[512];
	sprintf(strFile, MntUsbDirectory"/data/checkendface_%d.txt", checkendfaceNum);
	fp= fopen(strFile, "w");

	if (fp != NULL)
	{
		for(i = 0; i < pDaq_Setting->iDaqCount; i++)
			fprintf(fp,"%d\r\n", pDaq_Setting->pOrigData[i]);
		fclose(fp);
	}

	checkendfaceNum++;
	#endif

	if(!iRet)
	{
		iRet = GetReflectCoff(pDaq_Setting->pOrigData + pDaq_Setting->iOffset, pDaq_Setting->iDaqCount);
	}
    //释放申请的空间
    if(pDaq_Setting != NULL)
    {
        if(pDaq_Setting->pOrigData != NULL)
        {
            free(pDaq_Setting->pOrigData);
            pDaq_Setting->pOrigData = NULL;
        }
        free(pDaq_Setting);
        pDaq_Setting = NULL;
    }

    return iRet;

}

//检测光纤长度 返回光纤总长度，单位:米
float CheckFiberLen(POTDR_TOP_SETTINGS pOtdrTopSet, int solaSpliterNum)
{
	LOG(LOG_INFO, "solaSpliterNum = %d\n", solaSpliterNum);
	int iErr = 0;
   	UINT32 iLenCnt = 0;
	int iReCalFlag = 0;
	float fFiberDist = 0.0f;
	float fEndThr;
	UINT16 *pOrigData;
   	PDAQ_SETTING pDaqSet;
	POPMSET pOpmSet = pOtdrTopSet->pOpmSet;
	DEVFD *pDevFd = GetGlb_DevFd();
	int iOpmFd = pDevFd->iOpticDev;
    DBG_ENTER();

	//计算末端检测阈值
	fEndThr = pOtdrTopSet->pUser_Setting->sAnalysisSetting.fEndLossThr;
	(solaSpliterNum == 0) ? (fEndThr = Max(fEndThr, 5)) : (fEndThr = Max(fEndThr, 8));
    pDaqSet = (PDAQ_SETTING)calloc(1, sizeof(DAQ_SETTING));
    pDaqSet->pOrigData = (UINT16 *)calloc(1, MAX_DATA * sizeof(UINT16));
	pDaqSet->iPulseKind = ENUM_PULSE_1US;
	// sola模式下且存在分路器的情况
	if(solaSpliterNum != 0)
	{
   		pDaqSet->iPulseKind = ENUM_PULSE_10US;
		LOG(LOG_INFO, "-----------pDaqSet->iPulseKind = %d\n", pDaqSet->iPulseKind);
	}
   	pDaqSet->iAtteDbClass = ENUM_2_5DB;
    pDaqSet->iFilter = _IIR_256;
    pDaqSet->iDaqCount = MAX_DATA - BLACK_LEVEL_LEN;
	pDaqSet->iSmpReg = INTP01_01;
	pDaqSet->iNoiseCnt = BLACK_LEVEL_LEN;
	pDaqSet->fSmpRatio = 1.0f;
	pDaqSet->iOffset = 15;
	pDaqSet->iAdcGap = 0x300;


 	//检测光纤长度时采用1550nm波长的光，可以测试的距离更长,量程更准 added by  2015.9.21
   	Opm_SetWave(iOpmFd, pOpmSet, ENUM_WAVE_1550NM);
	int iflag = 0;

    iErr = DaqStart(pDaqSet);
	if (!iErr)
	{
		pOrigData = pDaqSet->pOrigData + pDaqSet->iOffset;
		//pOrigData = pDaqSet->pOrigData;

		//#define CHECKFIBERLEN
		#ifdef CHECKFIBERLEN
		int i = 0;
		FILE *fp;
		fp = fopen(MntUsbDirectory"/CheckFiberLen_1.txt",  "w+");
		if (fp != NULL)
		{
			for(i = 0; i < MAX_DATA; i++)
				//fprintf(fp,"%d\r\n", pOrigData[i]);
				fprintf(fp,"%d\r\n", pDaqSet->pOrigData[i]);
			fclose(fp);
		}
		#endif

		iReCalFlag = GetFiberLength(pOrigData, MAX_DATA - pDaqSet->iOffset, &iLenCnt, fEndThr, iflag);
		// sola模式下且存在分路器的情况
		if(solaSpliterNum!=0)
		{
			fFiberDist = GetFiberRealLength(iLenCnt, 1.46f, pDaqSet->fSmpRatio);
			//释放申请的空间
		    free(pDaqSet->pOrigData);
		    pDaqSet->pOrigData = NULL;
		    free(pDaqSet);
		    pDaqSet = NULL;
			return fFiberDist;
		}
		int iTmpDataLen = MAX_DATA;
		LOG(LOG_INFO, "---1---iReCalFlag = %d, iLenCnt = %d, fEndThr = %f, iOffset = %d\n", iReCalFlag, iLenCnt, fEndThr, pDaqSet->iOffset);
		if (iReCalFlag != 0)//iReCalFlag表示是否进行更精确的查找;
	    {
	    	//int iTmpDataLen = MAX_DATA;
	        if (iReCalFlag == 1)//使用大脉冲查找;
	        {
        		pDaqSet->iPulseKind = ENUM_PULSE_20US;
				iflag = 3;
	        }
	        else if (iReCalFlag == -1)
	        {
				if (iLenCnt < 1200)
				{
					pDaqSet->iFilter = _IIR_2K;
	        		pDaqSet->iPulseKind = ENUM_PULSE_50NS;
					pDaqSet->iAtteDbClass = ENUM_15_0DB;
					Opm_SetWave(iOpmFd, pOpmSet, ENUM_WAVE_1310NM);
					MsecSleep(100);
					iflag = 2;
					pDaqSet->iDaqCount = 3000;
					iTmpDataLen = pDaqSet->iDaqCount + pDaqSet->iNoiseCnt;
					//pDaqSet->iNoiseCnt = BLACK_LEVEL_LEN;
	        	}
	        	else if (iLenCnt < 2500)
	        	{
	        		pDaqSet->iPulseKind = ENUM_PULSE_50NS;
					iflag = 2;
					pDaqSet->iDaqCount = 4000;
					iTmpDataLen = pDaqSet->iDaqCount + pDaqSet->iNoiseCnt;
					//pDaqSet->iNoiseCnt = BLACK_LEVEL_LEN;
	        	}
				else
				{
					pDaqSet->iPulseKind = ENUM_PULSE_100NS;
					pDaqSet->iDaqCount = 4500;
					iTmpDataLen = pDaqSet->iDaqCount + pDaqSet->iNoiseCnt;
					//pDaqSet->iNoiseCnt = BLACK_LEVEL_LEN;
				}
	        }
			else if (iReCalFlag == 2)
			{
				if (iLenCnt < 5000)
	        	{
	        		pDaqSet->iPulseKind = ENUM_PULSE_200NS;
					pDaqSet->iDaqCount = 6500;
					iTmpDataLen = pDaqSet->iDaqCount + pDaqSet->iNoiseCnt;
					//pDaqSet->iNoiseCnt = BLACK_LEVEL_LEN;
	        	}
				else
				{
					pDaqSet->iPulseKind = ENUM_PULSE_500NS;
					pDaqSet->iDaqCount = 17000;
					iTmpDataLen = pDaqSet->iDaqCount + pDaqSet->iNoiseCnt;
					//pDaqSet->iNoiseCnt = BLACK_LEVEL_LEN;
				}
			}

	        iErr = DaqStart(pDaqSet);

	        if (!iErr)
	        {
				pOrigData = pDaqSet->pOrigData + pDaqSet->iOffset;
				//pOrigData = pDaqSet->pOrigData;
				
				iTmpDataLen = pDaqSet->iDaqCount;
				
	        	iReCalFlag = GetFiberLength(pOrigData, iTmpDataLen - pDaqSet->iOffset, &iLenCnt, fEndThr, iflag);

				#ifdef CHECKFIBERLEN
				fp = fopen(MntUsbDirectory"/CheckFiberLen_2.txt",  "w+");
				if (fp != NULL)
				{
					for(i = 0; i < iTmpDataLen; i++)
						//fprintf(fp,"%d\r\n", pOrigData[i]);
						fprintf(fp,"%d\r\n", pDaqSet->pOrigData[i]);
					fclose(fp);
				}
				#endif
				
		    }
			LOG(LOG_INFO, "---2---iReCalFlag = %d, iLenCnt = %d, fEndThr = %f\n", iReCalFlag, iLenCnt, fEndThr);

	    }
		else if (iLenCnt < 380)
		{
			pDaqSet->iPulseKind = ENUM_PULSE_20NS;
			pDaqSet->iDaqCount = 1000;
			iTmpDataLen = pDaqSet->iDaqCount + pDaqSet->iNoiseCnt;
					
			iErr = DaqStart(pDaqSet);
			if (!iErr)
			{
				pOrigData = pDaqSet->pOrigData + pDaqSet->iOffset;
				//pOrigData = pDaqSet->pOrigData;
				
				iTmpDataLen = pDaqSet->iDaqCount;
				
				iReCalFlag = GetFiberLength(pOrigData, iTmpDataLen - pDaqSet->iOffset, &iLenCnt, fEndThr, iflag);
			
				#ifdef CHECKFIBERLEN
				fp = fopen(MntUsbDirectory"/CheckFiberLen_2.txt",  "w+");
				if (fp != NULL)
				{
					for(i = 0; i < iTmpDataLen; i++)
						//fprintf(fp,"%d\r\n", pOrigData[i]);
						fprintf(fp,"%d\r\n", pDaqSet->pOrigData[i]);
					fclose(fp);
				}
				#endif
				
			}
			LOG(LOG_INFO, "---3---iReCalFlag = %d, iLenCnt = %d, fEndThr = %f\n", iReCalFlag, iLenCnt, fEndThr);
		}

		//根据点数计算光纤实际长度
		fFiberDist = GetFiberRealLength(iLenCnt, 1.46f, pDaqSet->fSmpRatio);
	}

	LOG(LOG_INFO, "-------------CheckFiberLen----------fFiberDist = %f\n", fFiberDist);
	//释放申请的空间
    free(pDaqSet->pOrigData);
    pDaqSet->pOrigData = NULL;
    free(pDaqSet);
    pDaqSet = NULL;

	DBG_EXIT(iErr);
   	return fFiberDist;
}

//测试通道
void test_channel(POTDR_TOP_SETTINGS pOtdrTopSettings)
{
    DEVFD *pDevFd = NULL;
    int iOpmFd;
    int iRet = 0;
	int iApdIndex, iVddIndex, iAmpIndex;
    int iApdVolt, iBlackLevel;
    float fStd = 0.0f;
	POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	POPM_TUNING pOpmTunning = pOpmSet->pOpmTuning;

	DBG_ENTER();

    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

	//又算法组来决定修改下面三个值
	iApdIndex = 3;
	iVddIndex = 0;
	iAmpIndex = 6;//205通道

/*
	//采集数据必须，设置APD  VDD  ATT  AMP
    iApdVolt = GetApd_SetValue(pOpmTunning,iApdIndex);
    Opm_SetApdVoltage(iOpmFd,pOpmSet,iApdVolt);			//设置APD 的反偏电压;
	Opm_SetVddVoltage(iOpmFd, pOpmSet, pOpmTunning->iVddValue[iVddIndex]);
	Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
	Opm_SetAmp(iOpmFd, pOpmSet, pOpmSet->AmpCode[iAmpIndex]);
*/
	
	
	unsigned int uiCtrl = 0xff;

	uiCtrl = 0x007c;
	Opm_WriteReg(iOpmFd, FPGA_CTRLOUT_REG, &uiCtrl);
	MsecSleep(10);
	uiCtrl = 0x0e;
	Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &uiCtrl);
	MsecSleep(10);
	uiCtrl = 0x3000;
	Opm_WriteReg(iOpmFd, OPM_CTRLOUT_REG, &uiCtrl);
	MsecSleep(10);


	//采集数据必须，设置APD  ，VDD  ，ATT  ，AMP
    iApdVolt = 0x800;
    Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);			//设置APD 的反偏电压;
	MsecSleep(10);
	
	Opm_SetVddVoltage(iOpmFd, pOpmSet, 0x6c0);
	MsecSleep(10);
	
	Opm_SetAttValue(iOpmFd, pOpmSet, 0xd70);
	MsecSleep(10);
	
	Opm_SetAmp(iOpmFd, pOpmSet, ENUM_AMP5);
	MsecSleep(10);
	

	Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &uiCtrl);
	LOG(LOG_INFO, "--2--FPGA_CTRLOUT_REG---uiCtrl = 0x%x\n", uiCtrl);
	Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &uiCtrl);
	LOG(LOG_INFO, "--2--FPGA_TRIG_CTRL---uiCtrl = 0x%x\n", uiCtrl);
	Opm_ReadReg(iOpmFd, OPM_CTRLOUT_REG, &uiCtrl);
	LOG(LOG_INFO, "--2--OPM_CTRLOUT_REG---uiCtrl = 0x%x\n", uiCtrl);

	int i = 0;
	//unsigned int att0 = pOpmTunning->arrAttValue[iAmpIndex];
	unsigned int att0 = 0xd70;
	for (; i < 100; ++i) 
	{
		iBlackLevel = GetBlack_Level(pOpmSet, &fStd);
		LOG(LOG_INFO, "---%s---iBlackLevel = %d, fStd = %f, att0 = 0x%x\n", __func__, iBlackLevel, fStd, att0);

		if (iBlackLevel > 60000 && iBlackLevel < 63000)
		{
			break;
		}
		else
		{
			if (iBlackLevel < 60000) 
			{
				att0 += 10;
			}
			else
			{
				att0 -= 6;
			}
		}
		if (att0 < 100 || att0  > 0xff0)
		{
			Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
			break;
		}

		Opm_SetAttValue(iOpmFd, pOpmSet, att0);
	}


    //通过黑电平判断光纤内是否有光存在;
	iBlackLevel = GetBlack_Level(pOpmSet, &fStd);
	LOG(LOG_INFO, "---last---iBlackLevel = %d, fStd = %f, att0 = %d\n", iBlackLevel, fStd, att0);

	if (iBlackLevel < 45000)
	{
		iRet = 1;
		LOG(LOG_ERROR,"iBlackLevel = %d\n", iBlackLevel);
	}
	LOG(LOG_INFO,"BlackLevel = %d\n", iBlackLevel);
    DBG_EXIT(iRet);

}

// 整定温度系数
void test_APDBreakVol(POTDR_TOP_SETTINGS pOtdrTopSettings)
{
    DEVFD *pDevFd = NULL;
    int iOpmFd;
	int iApdIndex, iVddIndex, iAmpIndex;
    int iApdVolt, iBlackLevel;
    float fStd = 0.0f;
	POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	POPM_TUNING pOpmTunning = pOpmSet->pOpmTuning;

	DBG_ENTER();

    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

	//有算法组来决定修改下面三个值
	iApdIndex = 3;
	iVddIndex = 0;
	iAmpIndex = 6;//205通道

	//采集数据必须，设置APD  VDD  ATT  AMP
    iApdVolt = 0x700;
    Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);			//设置APD 的反偏电压;
	Opm_SetVddVoltage(iOpmFd, pOpmSet, 0x680);
	Opm_SetAttValue(iOpmFd, pOpmSet, 0xa80);
	Opm_SetAmp(iOpmFd, pOpmSet, ENUM_AMP5);

	LOG(LOG_INFO,"\n\n\n test_APDBreakVol start !!!!\n\n\n");
	int i = 0;
	unsigned int att0 = 0xaf0;
	for (; i < 100; ++i) 
	{
		iBlackLevel = GetBlack_Level(pOpmSet, &fStd);
		LOG(LOG_INFO, "---iBlackLevel = %d, fStd = %f, att0 = %d\n", iBlackLevel, fStd, att0);

		if (iBlackLevel > 60000 && iBlackLevel < 64000)
		{
			if (fStd > 50.0f)
			{
				continue;
			}
			break;
		}
		else
		{
			if (iBlackLevel < 60000) 
			{
				att0 += 10;
			}
			else
			{
				att0 -= 6;
			}
		}
		if (att0 < 100 || att0  > 0xff0)
		{
			Opm_SetAttValue(iOpmFd, pOpmSet, pOpmTunning->arrAttValue[iAmpIndex]);
			break;
		}

		Opm_SetAttValue(iOpmFd, pOpmSet, att0);
	}

	iApdVolt = 0x700;
	int step_add = 4;
    int step_dec = 2;
	float fBasicStd = fStd;
	int downflag = 0;
	int upflag = 0;
	float fTemperature = 0.0f;
	float fTempCoefficient = 0.0f;
	unsigned int iApdVoltageSet = 0;
	
	GetOpmTemperature(iOpmFd, &fTemperature);//得到温度

	int len = 0;
	float temp[100] = { 0.0f };
	int apdValue[100] = { 0 };
	float lastTemp = fTemperature;
	float oriTemp = fTemperature;

	LOG(LOG_INFO, "\n\n\n fTemperature = %f, lastTemp = %f, oriTemp = %f\n\n\n", fTemperature, lastTemp, oriTemp);
	LOG(LOG_INFO, "fBasicStd = %f, fStd = %f\n", fBasicStd, fStd);

	int lastApdValue = 0x900;
	
	while ((fTemperature - oriTemp < 20.0f) && (len < 80))
	//while (len < 100)
	{
		//温度上升0.2°以上再次检测击穿电压
		if (fTemperature - lastTemp < 0.2f)
		{
			GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
			if (fTemperature > 80 || fTemperature < 0)
			{
				fTemperature = lastTemp + 0.3f;
				if (oriTemp > 80 || oriTemp < 0)
				{
					oriTemp = lastTemp + 0.3f;
				}
				MsecSleep(1000);
				LOG(LOG_INFO, "fTemperature = %f\n", fTemperature);
			}
			MsecSleep(200);
			continue;
		}
	
		iApdVolt = lastApdValue - 29;
		
		i = 0;
		downflag = 0;
		upflag = 0;
	    while(i < 100)//对APD击穿电压在+/-400范围内进行微调
	    {
	        Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);//第一次设置击穿电压时，由于压差较大，延时时间应增大
	    	MsecSleep(200);
	        iBlackLevel = GetBlack_Level(pOpmSet, &fStd);

			LOG(LOG_INFO, "-----iBlackLevel = %d, fStd = %f, len = %d\n", iBlackLevel, fStd, len);
			LOG(LOG_INFO,"-----iApdVolt = %x, fTemperature = %f\n", iApdVolt, fTemperature);

	        if ((fStd > 10 * fBasicStd) || (iBlackLevel < 50000))
	        {
	            upflag = 1;
	            if (downflag == 0)
	            {
	                iApdVolt = iApdVolt - step_dec;
	            }
	            else if (iApdVolt >= lastApdValue - 10)
	            {
	                iApdVolt = iApdVolt - step_add / 2;
					//获得击穿电压与温度
					LOG(LOG_INFO, "---2---iApdVolt = %x\n", iApdVolt);
					GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
					Opm_SetApdVoltage(iOpmFd, pOpmSet, 0x800);
					LOG(LOG_INFO, "fTemperature = %f\n", fTemperature);
					MsecSleep(500);
					temp[len] = fTemperature;
					apdValue[len] = iApdVolt;
					len++;

					lastTemp = fTemperature;
					lastApdValue = iApdVolt;
	                break;
	            }
				else
				{
					upflag = 0;
					iApdVolt = iApdVolt + step_add;
				}
	        }
	        else if (fStd < 5 * fBasicStd)
	        {
	            downflag = 1;
	            if (upflag == 0)
	            {
	                iApdVolt = iApdVolt + step_add;
	            }
	            else if (iApdVolt >= lastApdValue - 10)
	            {
	            	//获得击穿电压与温度
					LOG(LOG_INFO, "---3---iApdVolt = %x\n", iApdVolt);
					GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
					Opm_SetApdVoltage(iOpmFd, pOpmSet, 0x800);
					LOG(LOG_INFO, "fTemperature = %f\n", fTemperature);
					MsecSleep(500);
					temp[len] = fTemperature;
					apdValue[len] = iApdVolt;
					len++;

					lastTemp = fTemperature;
					lastApdValue = iApdVolt;
	                break;
	            }
				else
				{
					upflag = 0;
					iApdVolt = iApdVolt + step_add;
				}
	        }
	        else if (iApdVolt >= lastApdValue - 10)
	        {
	        	//获得击穿电压与温度
				LOG(LOG_INFO, "---4---iApdVolt = %x\n", iApdVolt);
				GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
				Opm_SetApdVoltage(iOpmFd, pOpmSet, 0x800);
				LOG(LOG_INFO, "fTemperature = %f\n", fTemperature);
				MsecSleep(500);
				temp[len] = fTemperature;
				apdValue[len] = iApdVolt - 1;
				len++;

				lastTemp = fTemperature;
				lastApdValue = iApdVolt;
	            break;
	        }
	        i++;
	    }

		if (len % 5 == 0)
		{
			float k = 0.0f;
			float b = 0.0f;
			CalLeastSquare_float(temp, apdValue, len, &k, &b);
			fTempCoefficient = k - 0.2f;
			iApdVoltageSet = (unsigned int)b;
			LOG(LOG_INFO, "\n\n  拟合参数：k = %f, b = %f\n\n", k, b);
		}

	}

	SaveApdTemp(temp, apdValue, len);

	iApdVolt = 0x700;
	Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);

	// 设置各档的APD电压
	unsigned int delta[OPM_APD_KIND] = { 0x20, 0x70, 0x148, 0x448};   //APD电压各档位偏移量
	for (i = 0; i < OPM_APD_KIND; ++i)
	{
		if (iApdVolt > delta[i])
		{
			pOpmTunning->iApdVoltageSet[i] = iApdVoltageSet - delta[i];
			pOpmTunning->fTempCoefficient[i] = fTempCoefficient;
		}
		else
		{
			pOpmTunning->iApdVoltageSet[i] = 0;
		}
	}
	pOpmTunning->fTemperatureSet = 0.0f;
	
	int iErr = SaveDataToEeprom(iOpmFd, pOpmTunning, OPM_EEPROM_OFF_TUNNING, sizeof(* pOpmTunning));
	LOG(LOG_INFO, "SaveDataToEeprom iErr = %d\n", iErr);

}

int SaveApdTemp(float *pTemp, int *pApdValue, int len)
{
	int i = 0;
	char strFile1[512];
	char strFile2[512];
	FILE *pFile1 = NULL;
	FILE *pFile2 = NULL;

	sprintf(strFile1, "/mnt/usb/data/tempertrue.txt");
	pFile1 = fopen(strFile1, "w");
	sprintf(strFile2, "/mnt/usb/data/apdValue.txt");
	pFile2 = fopen(strFile2, "w");

	if (pFile1 && pFile2)
	{
		for (i = 0; i < len; ++i)
		{
			fprintf(pFile1, "%f\n\r", pTemp[i]);
			fprintf(pFile2, "%d\n\r", pApdValue[i]);
		}

		fclose(pFile1);
		fclose(pFile2);
		return 0;
	}

	return -1;
}


// 最小二乘法计算斜率与截距
void CalLeastSquare_float(
	float * pXBuf,        	//输入的x变量;
	int * pYbuf,       		//输入的Y变量;
	int count,          	//数据的长度;
	float *k, 				//k,b=用来返回直线方程;
	float *b
	)                             
{
	float fSumX, fSumY, fSumMultXY, fSumMultXX;
	int i;
	float fRealCount;
	//防止数据太短，出现无穷大
	if (count < 2)
	{
		*k = 0.0f;
		*b = pYbuf[0];
		return;
	}
	fSumX = 0.0f;
	fSumY = 0.0f;
	fSumMultXY = 0.0f;
	fSumMultXX = 0.0f;
	fRealCount = 0.0f;
	for(i = 0; i < count; i++)
	{
		fSumX += pXBuf[i];
		fSumY += pYbuf[i];
		fSumMultXY += pXBuf[i] * pYbuf[i];
		fSumMultXX += pXBuf[i] * pXBuf[i];
	}
	fRealCount = count;
	*k = (fRealCount * fSumMultXY - fSumX * fSumY) / (fRealCount * fSumMultXX - fSumX * fSumX);
	*b = fSumY / fRealCount - fSumX / fRealCount * (*k);	
}


	// 测试APD的增益等级，每级差2.5dB
void test_APDGrade(POTDR_TOP_SETTINGS pOtdrTopSettings)
{
	
	DEVFD *pDevFd = NULL;
    int iOpmFd;
    int iApdVolt, iBlackLevel;
    float fStd = 0.0f;
    POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
    DBG_ENTER();

    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

	LOG(LOG_INFO, "\n\n\n test_APDGrade start !!!!\n\n\n");
	float fTemperature = 0.0f;
	GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
	LOG(LOG_INFO, "---1----fTemperature = %f\n", fTemperature);

	PDAQ_SETTING pDaq_Setting = NULL;
    pDaq_Setting = (PDAQ_SETTING)calloc(1, sizeof(DAQ_SETTING));//为采集设置申请空间
    //pDaq_Setting->pOrigData = (UINT16 *)calloc(1, MAX_DATA * sizeof(UINT16));
   	pDaq_Setting->iPulseKind = (UINT32)ENUM_PULSE_1US;
   	pDaq_Setting->iAtteDbClass = (UINT32)ENUM_5_0DB;//该处衰减量较大，只需探测较大的端面反射    盲区过大，调整为小脉宽
    pDaq_Setting->iFilter = (UINT32)_IIR_1K;
    pDaq_Setting->iDaqCount = MAX_DATA - BLACK_LEVEL_LEN;
	pDaq_Setting->iOffset = 15;
	pDaq_Setting->iSmpReg = INTP01_01;
	pDaq_Setting->fSmpRatio = 1.0f;
	pDaq_Setting->iNoiseCnt = BLACK_LEVEL_LEN;
	pDaq_Setting->iAdcGap = 0x100;

	//设置脉宽
	Opm_SetWave(iOpmFd, pOpmSet, ENUM_WAVE_1310NM);
	//设置VDD
	Opm_SetVddVoltage(iOpmFd, pOpmSet, 0x680);
	//设置att
    Opm_SetAttValue(iOpmFd, pOpmSet, 0xdf0);
	//设置累加次数
	Opm_SetFilter(iOpmFd,pOpmSet, pDaq_Setting->iFilter);
	//设置脉冲宽度
    Opm_SetPulse(iOpmFd, pOpmSet, pDaq_Setting->iPulseKind);
	//设置累加模式
	Opm_SetMode(iOpmFd,pOpmSet, ENUM_DAQ_MODE_ACC);
	//设置移相插补
	Opm_SetIntp(iOpmFd, pOpmSet, pDaq_Setting->iSmpReg);
	//设置通道
	Opm_SetAmp(iOpmFd, pOpmSet, ENUM_AMP7);
    //设置采样长度，注意，设置采集的长度与读取数据的长度不一定相等，因为可能存在插补
	SedAdcParameter(iOpmFd, pOpmSet, pDaq_Setting->iDaqCount + pDaq_Setting->iNoiseCnt, pDaq_Setting->iAdcGap);
	//打开光源
    Opm_LdPulseTurnOn(iOpmFd,pOpmSet);

	iApdVolt = 0x400;
    Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);		   //设置APD 的反偏电压;
    
	//开始数据采集
	//memset(pDaq_Setting->pOrigData, 0, (pDaq_Setting->iDaqCount + pDaq_Setting->iNoiseCnt) * sizeof(UINT16));
    static UINT16 pDataTemp1[MAX_DATA];
    memset(pDataTemp1, 0, MAX_DATA * sizeof(UINT16));
	GetAdc_Data(iOpmFd, pOpmSet, pDataTemp1);
	pDataTemp1[0] = pDataTemp1[1];

	// 修改ADC采集的错误数据,FPGA修改后，去掉该函数
	changeErrorData(pDataTemp1, pDaq_Setting->iDaqCount + pDaq_Setting->iNoiseCnt);
	//memcpy(pDaq_Setting->pOrigData, pDataTemp1, (pDaq_Setting->iDaqCount + pDaq_Setting->iNoiseCnt) * sizeof(UINT16));
	fStd = GetStdUint16(pDataTemp1 + MAX_DATA - 10000, 5000);

	LOG(LOG_INFO, "---1----fStd = %f\n", fStd);

	while (fStd > 50.0f)
	{
		memset(pDataTemp1, 0, MAX_DATA * sizeof(UINT16));
		GetAdc_Data(iOpmFd, pOpmSet, pDataTemp1);
		pDataTemp1[0] = pDataTemp1[1];

		// 修改ADC采集的错误数据,FPGA修改后，去掉该函数
		changeErrorData(pDataTemp1, pDaq_Setting->iDaqCount + pDaq_Setting->iNoiseCnt);
		fStd = GetStdUint16(pDataTemp1 + MAX_DATA - 10000, 5000);
		LOG(LOG_INFO, "---2----fStd = %f\n", fStd);
	}
	
    //保存数据
    static int saveDataNum = 0;
    SaveOtdrData(0, saveDataNum, pDaq_Setting->iDaqCount, pDataTemp1);
	saveDataNum++;

	GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
	LOG(LOG_INFO, "---2----fTemperature = %f\n", fTemperature);

	float basicStd = fStd;
	unsigned int step = 0x40;
    while(fStd < 20.0f * basicStd)//对APD击穿电压在+/-400范围内进行微调
    {
    	if (iApdVolt < 0x800)
		{
			step = 0x40;
		}
		else if (iApdVolt < 0x900)
		{
			step = 0x20;
		}
		else if (iApdVolt < 0x980)
		{
			step = 0x08;
		}
		else
		{
			step = 0x04;
		}
		
		iApdVolt += step;
        Opm_SetApdVoltage(iOpmFd, pOpmSet, iApdVolt);//第一次设置击穿电压时，由于压差较大，延时时间应增大
    	MsecSleep(500);

		//采集数据
        memset(pDataTemp1, 0, MAX_DATA * sizeof(UINT16));
		GetAdc_Data(iOpmFd, pOpmSet, pDataTemp1);
		pDataTemp1[0] = pDataTemp1[1];

		// 修改ADC采集的错误数据,FPGA修改后，去掉该函数
		changeErrorData(pDataTemp1, pDaq_Setting->iDaqCount + pDaq_Setting->iNoiseCnt);
		fStd = GetStdUint16(pDataTemp1 + MAX_DATA - 10000, 5000);
		iBlackLevel = GetMeanUint16(pDataTemp1 + MAX_DATA - 10000, 5000);
		LOG(LOG_INFO, "---2----fStd = %f\n", fStd);

		if (((fStd > 1000) && (fStd < 4100)) || ((fStd > 10000) && (iBlackLevel < 50000)))
		{
			fStd = 0.0f;
			continue;
		}

		if (fStd > 1000.0f && iApdVolt < 0x980 && iBlackLevel > 58000)
		{
			LOG(LOG_INFO, "---continue--iBlackLevel = %d, fStd = %f\n", iBlackLevel, fStd);
			fStd = 0.0f;
			continue;
		}

		SaveOtdrData(0, saveDataNum, pDaq_Setting->iDaqCount, pDataTemp1);
		saveDataNum++;
       
        if (fStd > 5 * basicStd)
        {
            break;
        }
    }

	GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
	LOG(LOG_INFO, "---3----fTemperature = %f\n", fTemperature);

	Opm_LdPulseTurnOff(iOpmFd, pOpmSet); 	//关闭光源

	if(pDaq_Setting->pOrigData != NULL)
    {
        free(pDaq_Setting->pOrigData);
        pDaq_Setting->pOrigData = NULL;
    }
    free(pDaq_Setting);
    pDaq_Setting = NULL;

}


//初始化任务上下文
int InitTaskContext(POTDR_TOP_SETTINGS pOtdrTopSet)
{
	int iErr = 0, i, j;

	PTASK_SETTINGS pTaskSet = NULL;

    DBG_ENTER();

	if (NULL == pOtdrTopSet)
	{
		DBG_EXIT(-1);
		return -1;
    }

	PTASK_CONTEXTS pTaskContext = pOtdrTopSet->pTask_Context;
    PUSER_SETTINGS pUserSet = pOtdrTopSet->pUser_Setting;
	if ((NULL == pTaskContext) || (NULL == pUserSet))
	{
		DBG_EXIT(-2);
		return -2;
    }

	if (ENUM_MODE_NORMAL != pUserSet->enWorkMode)
	{
		DBG_EXIT(-3);
		return -3;
	}

    FRONT_SETTING sFrntSet[WAVE_NUM];
	memcpy(&sFrntSet, &pUserSet->sFrontSetting, sizeof(FRONT_SETTING) * WAVE_NUM);

	COMMON_SETTING sComSet = pUserSet->sCommonSetting;
	SAMPLE_SETTING sSmpSet = pUserSet->sSampleSetting;
	ANALYSIS_SETTING sAlysSet = pUserSet->sAnalysisSetting;

	//pTask_Context赋值
    MutexLock(&pTaskContext->mMutex);
	pTaskContext->iTaskNum = 0;
    pTaskContext->enWorkMode = pUserSet->enWorkMode;
	pTaskContext->iCurTaskPos = 0;
    pTaskContext->enWorkStatus = ENUM_OTDR_STOPPED;
	pTaskContext->enDpState = ENUM_DPSTATE_WAITE;
    pTaskContext->iEventAnalysFlag = sAlysSet.iAutoAnalysisFlag;
    pTaskContext->iWriteSorFlag = sComSet.iAutoSaveFlag;
	pTaskContext->iDrawCurveFlag = 1;
	//pTaskContext->pLogSig = ?
    MutexUnlock(&pTaskContext->mMutex);

	//各个任务项的设置 task_settings
    for (i = 0; i < WAVE_NUM; i++)
    {
    	//判断当前任务是否设置
    	if (0 == pUserSet->iWave[i])
    	{
			continue;
    	}

		pTaskSet = &pTaskContext->Task_SetQueue[pTaskContext->iTaskNum];
		pTaskSet->enWave = (OPM_WAVE)i;

		// 自动量程
		float fFiberDist = CheckFiberLen(pOtdrTopSet, 0);

		if (ENUM_FIBER_RANGE_AUTO == sFrntSet[i].enFiberRange)
		{
			//自动量程下检查光纤长度
			pTaskSet->enFiberRange = GetFiberRange(fFiberDist / 1000.0f);
		}
		else
		{
    		pTaskSet->enFiberRange = sFrntSet[i].enFiberRange;
		}

		// 设置算法全局变量
		SetFiberLen(fFiberDist);

		//根据采样分辨率设置查表获取采样比率, 移相寄存器，skip offset的值
		int iSmpIndex = (int)sSmpSet.enSmpResolution;
		pTaskSet->fSmpRatio = fSampleRatio[pTaskSet->enFiberRange][iSmpIndex];
		pTaskSet->iSmpReg = iPhaseShiftReg[pTaskSet->enFiberRange][iSmpIndex];
		pTaskSet->fAutoRange = fFiberDist;

		pTaskSet->iOffset = CalFiberOffset(pOtdrTopSettings->pOpmSet->pOpmTuning->OffsetPoint,
		 				    pTaskSet->fSmpRatio,
		 				    pTaskSet->enFiberRange,
		 				    iSmpIndex);

		//pTaskSet->iOffset = 1;//使用注释内容会导致算法死机，暂时使用1，若fpga修改完成，代码应使用上述注释的部分
		LOG(LOG_INFO, "OTDR--pTaskSet->iOffset == %d\n", pTaskSet->iOffset);

		//得到脉冲时间
		pTaskSet->enPulseTime = GetPulseTime(pTaskSet->enWave, sFrntSet[i].enPulseTime,
											pTaskSet->enFiberRange, fFiberDist / 1000.0f);

		//FPGA延时的长度不同，导致在脉宽大于等于20ns时，offset发生变化
		if (pTaskSet->enPulseTime >= ENUM_PULSE_20NS)
		{
			pTaskSet->iOffset = pTaskSet->iOffset - (int)(pTaskSet->fSmpRatio);
			pTaskSet->iOffset = Max(pTaskSet->iOffset, 0);
		}

		//得到平均时间
		if (ENUM_AVG_TIME_REALTIME == sFrntSet[i].enAverageTime)
		{
		    pTaskSet->enAverageTime = sFrntSet[i].enAverageTime;
			pTaskSet->iAverageTime = 1;
			pTaskSet->iRealTimeFlag = 1;
		}
		else
		{
			if (ENUM_AVG_TIME_AUTO == sFrntSet[i].enAverageTime)
			{
				sFrntSet[i].enAverageTime = pUserSet->sSampleSetting.enAutoAvrTime;
			}

			pTaskSet->enAverageTime = sFrntSet[i].enAverageTime;
			pTaskSet->iAverageTime = GetAverageTime(pTaskSet->enAverageTime);
			pTaskSet->iRealTimeFlag = 0;
		}

		//得到显示长度,转换成需要显示的数据点数
		pTaskSet->iDataCount = GetSmpCount(pTaskSet->enFiberRange,
										  sSmpSet.sWaveLenArgs[i].fRefractiveIndex,
										  pTaskSet->fSmpRatio) + pTaskSet->iOffset;
		pTaskSet->iDisplayCount = pTaskSet->iDataCount - pTaskSet->iOffset;

		//得到量程实际距离
        float exl = 1.0f - sSmpSet.sWaveLenArgs[i].fExcessLength / 100.0f;
		LOG(LOG_INFO, "exl = %f\n", exl);
		pTaskSet->fRangeDist = Range2Dist(pTaskSet->enFiberRange) * exl;

		//计算采样间隔
// 		pTaskSet->fSmpIntval = pTaskSet->fRangeDist / (float)pTaskSet->iDataCount;

		pTaskSet->fSmpIntval = exl * LIGHTSPEED_1000KM_S / (2 * ADC_CLOCK_FREQ
        						* pTaskSet->fSmpRatio * sSmpSet.sWaveLenArgs[i].fRefractiveIndex);
		//得到盲区长度
		pTaskSet->iBlindLength = GetBlindLength(pTaskSet->enPulseTime, pTaskSet->fSmpRatio);

		//赋值到用户设置结构体
		MutexLock(&pUserSet->mDataMutex);
		if (ENUM_AVG_TIME_REALTIME != sFrntSet[i].enAverageTime)
		{
			pUserSet->sFrontSetting[i].enAverageTime = pTaskSet->enAverageTime;
		}
		//自动量程或自动脉宽下不能赋值
		if ((ENUM_FIBER_RANGE_AUTO != sFrntSet[i].enFiberRange)
			&& (ENUM_PULSE_AUTO != sFrntSet[i].enPulseTime))
		{
			pUserSet->sFrontSetting[i].enPulseTime = pTaskSet->enPulseTime;
			pUserSet->sFrontSetting[i].enFiberRange = pTaskSet->enFiberRange;
		}
		
		MutexUnlock(&pUserSet->mDataMutex);

        pTaskContext->iTaskNum++;
    }

    //最后为各个任务填充参数
    for (i = 0; i < pTaskContext->iTaskNum; i++)
    {
		int iDataCount, iDispCount;
        pTaskSet = &pTaskContext->Task_SetQueue[i];

		iDataCount = pTaskSet->iDataCount;
        iDispCount = pTaskSet->iDisplayCount;

       	if (iDataCount > MAX_DATA)
        {
            iErr = -3;
			LOG(LOG_ERROR, "(iDataCount > MAX_DATA)\n");
            break;
        }

		//得到Daq采集队列参数
		GetDaqQueue(pTaskSet);

        //对通道组合结构赋初值
        memset(pTaskSet->pCombine_Info, 0, sizeof(COMBINE_INFO));
    	pTaskSet->pCombine_Info->iSigLen = iDispCount + BLACK_LEVEL_LEN ;
    	pTaskSet->pCombine_Info->iDataNum = 0;
    	pTaskSet->pCombine_Info->iBlind = pTaskSet->iBlindLength;
    	pTaskSet->pCombine_Info->iCombineFlag = 0;
    	pTaskSet->pCombine_Info->iRealTimeFlag = pTaskSet->iRealTimeFlag;
    	pTaskSet->pCombine_Info->iTotalNum = pTaskSet->iDaqNum;
    	pTaskSet->pCombine_Info->iPulseWidth = pTaskSet->enPulseTime;
		pTaskSet->pCombine_Info->iWave = pTaskSet->enWave;
    	pTaskSet->pCombine_Info->iTaskCycle = 0;
		pTaskSet->pCombine_Info->fSampleRating=pTaskSet->fSmpRatio;
    	for (j = 0; j < pTaskSet->iDaqNum; j++)
    	{
    	    pTaskSet->pCombine_Info->combine_para[j].iAttenuation =
							pTaskSet->Daq_SetQueue[j].iAtteDbClass;
    	}

		//对事件识别结构赋初值
        pTaskSet->pAlgorithm_info->iSignalLength = iDispCount + BLACK_LEVEL_LEN;	//信号长度;
    	pTaskSet->pAlgorithm_info->iDisplayLenth = iDispCount;        			//显示长度;
    	pTaskSet->pAlgorithm_info->PulseWidth = pTaskSet->enPulseTime;			//脉冲宽度
    	pTaskSet->pAlgorithm_info->iBlind = pTaskSet->iBlindLength;				//盲区长度
    	pTaskSet->pAlgorithm_info->SetParameters.fSampleRating = pTaskSet->fSmpRatio;
    	pTaskSet->pAlgorithm_info->SetParameters.fRefractiveIndex = pUserSet->sSampleSetting.sWaveLenArgs[pTaskSet->enWave].fRefractiveIndex;//折射率
		pTaskSet->pAlgorithm_info->SetParameters.fLossThreshold = pUserSet->sAnalysisSetting.fSpliceLossThr;//损耗阈值
    	pTaskSet->pAlgorithm_info->SetParameters.fReflectThreshold = pUserSet->sAnalysisSetting.fReturnLossThr;//反射率阈值
    	pTaskSet->pAlgorithm_info->SetParameters.fFiberEndThreshold = pUserSet->sAnalysisSetting.fEndLossThr;//末端检测阈值
    	pTaskSet->pAlgorithm_info->SetParameters.fBackScattering = pUserSet->sSampleSetting.sWaveLenArgs[pTaskSet->enWave].fBackScattering;	//背向散射强度值
		pTaskSet->pAlgorithm_info->iFiberRange = pTaskSet->enFiberRange;
    	pTaskSet->pAlgorithm_info->SetParameters.fExcessLength = pUserSet->sSampleSetting.sWaveLenArgs[pTaskSet->enWave].fExcessLength / 100.0f;
		pTaskSet->pAlgorithm_info->iOffsetFiberLen = pTaskSet->iOffset;
		InitDaqQueue(&pTaskSet->Daq_Queue, pTaskSet->iDaqNum);
    }
	//画DAQ曲线
    SetTaskCurvePara(pTaskContext);

	WriteOTDRMsgQueue(ENUM_OTDRMSG_NODATA);

    DBG_EXIT(iErr);
    return iErr;
}


//分配通道组合 算法等数据结构内存
int AllocTaskContext(PTASK_CONTEXTS pTaskContext)
{
    int i;

    DBG_ENTER();

	for (i = 0; i < MAX_TASK_NUM; i++)
    {
        PTASK_SETTINGS pTaskSet = &pTaskContext->Task_SetQueue[i];

 		//为算法中通道组合结构申请空间并赋值
        pTaskSet->pCombine_Info = (COMBINE_INFO *)calloc(1, sizeof(COMBINE_INFO));

        //为算法中事件分析结构申请空间并赋值
        pTaskSet->pAlgorithm_info = (ALGORITHM_INFO *)calloc(1, sizeof(ALGORITHM_INFO));
        pTaskSet->pAlgorithm_info->pSignalLog = (float *)calloc(MAX_DATA, sizeof(float));
    }

    DBG_EXIT(0);
    return 0;
}


/*
	返回值 0--成功 -1--整定超时失败 -2--EEPROM保存整定数据失败
*/
int TuneAtt()
{
	int iRet = 0;
	int iErr = 0;
	int i = 0;

	DEVFD *pDevFd = NULL;
    int iOpmFd;
	int tryTimes = 0;
    int iBlackLevel;
    float fStd;
	unsigned int regValueTmp = 0;
	POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	POPM_TUNING pOpmTunning = pOpmSet->pOpmTuning;

	DBG_ENTER();
	LOG(LOG_INFO, "TuneAtt start");
#ifndef TUNE_ATT
    //治具会给ATT全写1，如果为1就进行整定
	if(pOpmSet->pOpmTuning->arrAttValue[0] != 1)
	{
		return 0;
	}
#endif
    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

	//采集数据必须，设置APD  VDD  ATT  AMP
    Opm_SetApdVoltage(iOpmFd, pOpmSet, 1);			//设置APD 的反偏电压;
	Opm_SetVddVoltage(iOpmFd, pOpmSet, 0);

	for (i = 1; i < OPM_AMP_KIND; i += 2)
	{
		Opm_SetAmp(iOpmFd, pOpmSet, pOpmSet->AmpCode[i]);
		tryTimes = 200;
		regValueTmp = 0x0a80;
		Opm_SetAttValue(iOpmFd, pOpmSet, regValueTmp);
		MsecSleep(100);

		do
	    {
			MsecSleep(100);
			iBlackLevel = GetBlack_Level(pOpmSet,&fStd);

			LOG(LOG_INFO, "-TuneAtt--iBlackLevel = %d---\n", iBlackLevel);

			if(iBlackLevel < 61000)
	        {
	            regValueTmp += 0x10;
	            if ((regValueTmp > (0x0100)) && (regValueTmp < 0x0fff))
	            {
	                Opm_SetAttValue(iOpmFd, pOpmSet, regValueTmp);
	            }
				else
				{
					regValueTmp = 0x0a80;
					Opm_SetAttValue(iOpmFd, pOpmSet, regValueTmp);
					break;
				}

	        }
	        else if (iBlackLevel > 63000)
	        {
	        	regValueTmp -= 0x0a;
	            if ((regValueTmp > (0x0100)) && (regValueTmp < 0x0fff))
	            {
	                Opm_SetAttValue(iOpmFd, pOpmSet, regValueTmp);
	            }
				else
				{
					regValueTmp = 0x0a80;
					Opm_SetAttValue(iOpmFd, pOpmSet, regValueTmp);
					break;
				}
	        }
	        else
	        {
	        	LOG(LOG_INFO, "-TuneAtt--i = %d, att0 = %d, iBlackLevel = %d---\n", i, regValueTmp, iBlackLevel);
	            break;
	        }
	    } while(tryTimes--);

	    if(tryTimes > 0)
	    {
	    	//1.5倍放大通道不使用，ATT设置和4.3倍相同
			pOpmTunning->arrAttValue[i] = regValueTmp;
			pOpmTunning->arrAttValue[i - 1] = regValueTmp;
	    }
	    else
	    {
			LOG(LOG_ERROR, "---time over---\n");
			iRet = -1;
	    }
	}

	LOG(LOG_INFO, "TuneAtt over");
	iErr = SaveDataToEeprom(iOpmFd, pOpmTunning, OPM_EEPROM_OFF_TUNNING, sizeof(* pOpmTunning));
	if(iErr != 0)
		iRet = -2;

	return iRet;
}

int MatchOtdrEvent(EVENTS_TABLE* events, float len, int iBlind, int location)
{
    int i = 0;
    for (i = 0; i < events->iEventsNumber; ++i)
    {
		if ((events->EventsInfo[i].iBegin - iBlind <= location)
		    && (events->EventsInfo[i].iEnd >= location))
		    return i;
    }

    return -1;
}

float ThresholdOfMatch(float len, float smpInterval)
{
    //阈值公式
    return (1.0 + 0.00001 * len + smpInterval);
}

//返回系统是否设置注入光纤或接收光纤 返回0 未设置
int LaunchRecvFiberEnable()
{
    if(pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEventOrLength == 0)
    {
        if (!pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiber &&
            !pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiber)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if (!pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiberEvent &&
            !pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiberEvent)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

//处理注入光纤和接收光纤
void HandleLaunchRecvFiberByLength(PTASK_SETTINGS pTastSet)
{
    ALGORITHM_INFO *pAlg = pTastSet->pAlgorithm_info;
    EVENTS_TABLE* events = &pAlg->EventsTable;
    events->iLaunchFiberIndex = 0;
    events->iRecvFiberIndex = 0;
    events->iIsThereASpan = 1;

    //未设置注入光纤
    if (!pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiber &&
        !pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiber)
        return;

    //计算跨段长度
    float fiberLen = CalcRealDist(events->EventsInfo[events->iEventsNumber - 1].iBegin, pTastSet->fSmpIntval);
    float launchFiberLen = pOtdrTopSettings->pUser_Setting->sAnalysisSetting.fLaunchFiberLen;
    float recvFiberLen = fiberLen - pOtdrTopSettings->pUser_Setting->sAnalysisSetting.fRecvFiberLen;
    //float totalThr = ThresholdOfMatch(fiberLen, Param.fSmpIntval);

    //若跨段间距太小，则无法设置跨段
    if (pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiber &&
        pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiber)
    {
        if ((recvFiberLen - launchFiberLen) < -0.1)
        {
            //无法设置跨段
            events->iIsThereASpan = 0;
        }
    }

    if (pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiber)
    {
        //注入光纤在有效范围之内
        if ((fiberLen - launchFiberLen) > -0.01f
            && launchFiberLen >= 0)
        {
            int begin = (int)(launchFiberLen / pTastSet->fSmpIntval + 0.5f);
            int index = MatchOtdrEvent(events, launchFiberLen, pAlg->iBlind, begin);
            if (index > 0 )
            {
                events->iLaunchFiberIndex = index;
            }
            else
            {
                index = AddEventsTable(pAlg->pSignalLog, pAlg->iSignalLength, events, pAlg->iBlind, begin, pTastSet->fSmpRatio);
                if (index >= 0)
                    events->iLaunchFiberIndex = index;
            }
        }
        else
        {
            //无法设置跨段
            events->iIsThereASpan = 0;
            events->iLaunchFiberIndex = events->iEventsNumber - 1;
            if(pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiber)
            {
                events->iRecvFiberIndex = events->iEventsNumber - 1;
            }
            return;
        }
    }

    if (pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiber)
    {
        if(events->iIsThereASpan == 0)
        {
            events->iRecvFiberIndex = events->iLaunchFiberIndex;
            return;
        }

        //接收光纤在有效范围之内
        if ((fiberLen - recvFiberLen) > -0.01f
            && recvFiberLen >= 0)
        {
            int begin = (int)(recvFiberLen / pTastSet->fSmpIntval + 0.5f);
            int index = MatchOtdrEvent(events, recvFiberLen, pAlg->iBlind, begin);
            if (index > 0 )
            {
                events->iRecvFiberIndex = index;
            }
            else
            {
                index = AddEventsTable(pAlg->pSignalLog, pAlg->iSignalLength, events, pAlg->iBlind, begin, pTastSet->fSmpRatio);
                if (index >= 0)
                    events->iRecvFiberIndex = index;
            }
        }
        else
        {
            //无法设置跨段
            events->iIsThereASpan = 0;
            events->iRecvFiberIndex = 0;
        }
    }
}

//处理注入光纤和接收光纤
void HandleLaunchRecvFiberByEvent(ALGORITHM_INFO* pAlg)
{
    EVENTS_TABLE* events = &pAlg->EventsTable;
    events->iLaunchFiberIndex = 0;
    events->iRecvFiberIndex = 0;
    events->iIsThereASpan = 1;

    //未设置注入光纤
    if (!pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiberEvent &&
        !pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiberEvent)
        return;

    //计算跨段长度
    int eventNum = events->iEventsNumber;
    int launchFiberEvent = pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iLaunchFiberEvent;
    int recvFiberEvent = pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iRecvFiberEvent;

    //若跨段间距太小，则无法设置跨段
    if (pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiberEvent &&
        pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiberEvent)
    {
        if ((launchFiberEvent + recvFiberEvent) > eventNum + 1)
        {
            //无法设置跨段
            events->iIsThereASpan = 0;
        }
    }

    if (pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableLaunchFiberEvent)
    {
        //注入光纤在有效范围之内
        if (launchFiberEvent <= eventNum)
        {
            events->iLaunchFiberIndex = launchFiberEvent - 1;
        }
        else
        {
            //无法设置跨段
            events->iIsThereASpan = 0;
            events->iLaunchFiberIndex = events->iEventsNumber - 1;
            if(pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiberEvent)
            {
                events->iRecvFiberIndex = events->iEventsNumber - 1;
            }
            return;
        }
    }

    if (pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEnableRecvFiberEvent)
    {
        if(events->iIsThereASpan == 0)
        {
            events->iRecvFiberIndex = events->iLaunchFiberIndex;
            return;
        }

        //接收光纤在有效范围之内
        if (recvFiberEvent <= eventNum)
        {
            events->iRecvFiberIndex = eventNum - recvFiberEvent;
        }
        else
        {
            //无法设置跨段
            events->iIsThereASpan = 0;
            events->iRecvFiberIndex = 0;
        }
    }
}


//事件分析
void EventAnalyze(PTASK_SETTINGS pTaskSetQueue, int iCurPos)
{
	DBG_ENTER();

    PTASK_SETTINGS pTaskSet = &pTaskSetQueue[iCurPos];
	pTaskSet->pAlgorithm_info->SetParameters.iwavelength = pTaskSet->enWave;

    ALGORITHM_INFO *pAlgInfo = pTaskSet->pAlgorithm_info;
	COMBINE_INFO *pCombInfo = pTaskSet->pCombine_Info;
	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;

	//pAlgInfo->iOffsetFiberLen = 1;
	//pAlgInfo->iOffsetFiberLen = pTaskSet->iOffset;
	pAlgInfo->iOffsetFiberLen = (int)(1.5f * pAlgInfo->SetParameters.fSampleRating);
	pAlgInfo->iOffsetFiberLen = Max(pAlgInfo->iOffsetFiberLen, 1);

	//将组合完成的最终对数数据复制算法数据缓冲区
	memcpy(pAlgInfo->pSignalLog, pCombInfo->pCombineData, pAlgInfo->iSignalLength * sizeof(float));

	//拷贝组合点位置信息 0行=组合点位置,1行=组合类型, [0][19]=代表组合次数 added by  2015.10.9
	pAlgInfo->pCombineInfo[0][19] = pTaskSet->iDaqNum;//组合次数

	int i;
	for (i = 0; i < pTaskSet->iDaqNum; i++)
	{
		pAlgInfo->pCombineInfo[0][i] = (UINT32)pCombInfo->combine_pos[i].iComPosition;
		pAlgInfo->pCombineInfo[1][i] = (UINT32)pCombInfo->combine_pos[i].iComStyle;
	}

	AlgorithmMain(pAlgInfo);

    //当前的任务指针为1，说明是第二条波形，需要处理宏弯曲事件
    if (iCurPos == 1)
    {
        //处理宏弯曲
        OtdrAdapterMacrobending(
            pTaskSetQueue[iCurPos - 1].pAlgorithm_info,
            pTaskSetQueue[iCurPos].pAlgorithm_info
            );
    }

    //根据设置来设置注入光纤和接收光纤
    if(pOtdrTopSettings->pUser_Setting->sAnalysisSetting.iEventOrLength == 0)
    {
        HandleLaunchRecvFiberByLength(pTaskSet);
    }
    else
    {
        HandleLaunchRecvFiberByEvent(pAlgInfo);
    }

	//事件阈值判断
	IsEventPass(pTaskSet->enWave, &pAlgInfo->EventsTable,
	    GetSpanORL(&pAlgInfo->EventsTable, pDisplay->pCurve[pDisplay->iCurCurve]));
	changeIsPassUI(pAlgInfo->EventsTable.iIsPass, pDisplay->iCurCurve);
	if(pOtdrTopSettings->pTask_Context->iTaskNum == pOtdrTopSettings->pTask_Context->iCurTaskPos + 1)
	{
	    SetDrawIsPass(1);
	}

    //重新计算累计损耗(原因：测试得曲线和从SOR读取的曲线对应的累计损耗有误差，所以重新用截取精度的值计算)
    double fTotalLoss = 0.0f;
    EVENTS_TABLE *eventsTable = &pAlgInfo->EventsTable;
    eventsTable->EventsInfo[0].fTotalLoss = fTotalLoss;
    for(i = 1; i < eventsTable->iEventsNumber; i++)
    {
        double length = eventsTable->EventsInfo[i].fEventsPosition
            - eventsTable->EventsInfo[i - 1].fEventsPosition;
        double loss = Float2Float(PRECISION_3, eventsTable->EventsInfo[i - 1].fAttenuation) * (length / 1000);
        if(i == 1)
        {
            fTotalLoss = Float2Float(PRECISION_3, loss) + fTotalLoss;
        }
        else
        {
            fTotalLoss = Float2Float(PRECISION_3, eventsTable->EventsInfo[i-1].fLoss)
                + Float2Float(PRECISION_3, loss) + fTotalLoss;
        }
        fTotalLoss = Float2Float(PRECISION_3, fTotalLoss);
        eventsTable->EventsInfo[i].fTotalLoss = fTotalLoss;
    }

	//将事件列表复制到画图结构
	SetCurveEvents(CURR_CURVE, pDisplay, &pAlgInfo->EventsTable);
	SendCurveAdditionCmd(EN_ADD_REFSH, 0);

	DBG_EXIT(0);
}

//修改参数后重新事件分析
//该函数仅在熔接损耗阈值、反射率阈值、光纤末端阈值发生变化时调用，因此部分参数不需要重新赋值
void ReeventAnalyze(void)
{
	DBG_ENTER();

	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
	PUSER_SETTINGS pUserSet = pOtdrTopSettings->pUser_Setting;
	int i = 0;

	for(i = 0; i < pDisplay->iCurveNum; i++)
	{
        PTASK_SETTINGS pTskSet = &pOtdrTopSettings->pTask_Context->Task_SetQueue[i];
        ALGORITHM_INFO *pAlgInfo = pTskSet->pAlgorithm_info;
	    PCURVE_INFO pCurve = pDisplay->pCurve[i];

	    pAlgInfo->iSOLATest = 0;
        /*pAlgInfo->iFiberRange = pCurve->pParam.enRange;
        pAlgInfo->iSignalLength = pCurve->dIn.uiCnt;
        pAlgInfo->iDisplayLenth = GetSmpCount(pAlgInfo->iFiberRange,
                                             pAlgInfo->SetParameters.fRefractiveIndex,
                                             pCurve->pParam.fSmpRatio);
        pAlgInfo->PulseWidth = pCurve->pParam.enPluse;*/
        pAlgInfo->SetParameters.fLossThreshold = pUserSet->sAnalysisSetting.fSpliceLossThr;
        pAlgInfo->SetParameters.fReflectThreshold = pUserSet->sAnalysisSetting.fReturnLossThr;
        pAlgInfo->SetParameters.fFiberEndThreshold = pUserSet->sAnalysisSetting.fEndLossThr;
        /*pAlgInfo->SetParameters.fLightSpeed = LIGHTSPEED_M_S;
        pAlgInfo->SetParameters.iwavelength = pCurve->pParam.enWave;
        pAlgInfo->
		fPointLength = pCurve->pParam.fSmpIntval;*/

        int j = 0, k = 0;
        for(j = 0; j < 2; j++)
        {
            for(k = 0; k < 20; k++)
            {
                pAlgInfo->pCombineInfo[j][k] = 0;
            }
        }

    	/*//将组合完成的最终对数数据复制算法数据缓冲区
    	for(j = 0; j < pCurve->dIn.uiCnt; j++)
    	{
                pAlgInfo->pSignalLog[j] = (float)pCurve->dIn.pData[j] / SCALE_FACTOR;
    	}*/

    	memset(&pAlgInfo->EventsTable, 0, sizeof(EVENTS_TABLE));

    	//算法主函数
    	AlgorithmMain(pAlgInfo);

        //当前的任务指针为1，说明是第二条波形，需要处理宏弯曲事件
        if (i == 1)
        {
            //处理宏弯曲
            OtdrAdapterMacrobending(
                pOtdrTopSettings->pTask_Context->Task_SetQueue[0].pAlgorithm_info,
                pOtdrTopSettings->pTask_Context->Task_SetQueue[1].pAlgorithm_info
                );
        }

        pAlgInfo->EventsTable.iIsThereASpan = pCurve->Events.iIsThereASpan;
        if(pCurve->Events.iLaunchFiberIndex > 0 || pCurve->Events.iRecvFiberIndex > 0)
        {
            int iLaunchFiberEnable = pUserSet->sAnalysisSetting.iEnableLaunchFiber;
            int iRecvFiberEnable = pUserSet->sAnalysisSetting.iEnableRecvFiber;
            double fLaunchFiberLength = pUserSet->sAnalysisSetting.fLaunchFiberLen;
            double fRecvFiberLength = pUserSet->sAnalysisSetting.fRecvFiberLen;
            pUserSet->sAnalysisSetting.iEnableLaunchFiber = 0;
            pUserSet->sAnalysisSetting.iEnableRecvFiber = 0;

            if(pCurve->Events.iLaunchFiberIndex > 0)
            {
                pUserSet->sAnalysisSetting.iEnableLaunchFiber = 1;
                pUserSet->sAnalysisSetting.fLaunchFiberLen =
                    pCurve->Events.EventsInfo[pCurve->Events.iLaunchFiberIndex].fEventsPosition;
            }

            if(pCurve->Events.iRecvFiberIndex > 0)
            {
                pUserSet->sAnalysisSetting.iEnableRecvFiber = 1;
                pUserSet->sAnalysisSetting.fRecvFiberLen =
                    pCurve->Events.EventsInfo[pCurve->Events.iEventsNumber - 1].fEventsPosition
                    - pCurve->Events.EventsInfo[pCurve->Events.iRecvFiberIndex].fEventsPosition;
            }

            HandleLaunchRecvFiberByLength(pTskSet);
            pUserSet->sAnalysisSetting.iEnableLaunchFiber = iLaunchFiberEnable;
            pUserSet->sAnalysisSetting.iEnableRecvFiber = iRecvFiberEnable;
            pUserSet->sAnalysisSetting.fLaunchFiberLen = fLaunchFiberLength;
            pUserSet->sAnalysisSetting.fRecvFiberLen = fRecvFiberLength;
        }

    	//事件阈值判断
    	IsEventPass(pTskSet->enWave, &pAlgInfo->EventsTable,
    	    GetSpanORL(&pAlgInfo->EventsTable, pDisplay->pCurve[pDisplay->iCurCurve]));
    	changeIsPassUI(pAlgInfo->EventsTable.iIsPass, pDisplay->iCurCurve);
    	if(pOtdrTopSettings->pTask_Context->iTaskNum == pOtdrTopSettings->pTask_Context->iCurTaskPos + 1)
    	{
    	    SetDrawIsPass(1);
    	}

    	//将事件列表复制到画图结构
    	SetCurveEvents(i, pDisplay, &pAlgInfo->EventsTable);
    	DBG_EXIT(0);
	}
}


//分配采集数据缓冲区，组合数据缓冲区等
int AllocDataBuffer(PTASK_SETTINGS pTaskSet)
{
	int i, iDaqSize, iCombSize;

    DBG_ENTER();
	/*
	 * 数据采集缓冲区:
	 * |<------------------- 有光采集长度 ---------------->|<- 无光采集(512) ->|
	 * |<- offset ->|<--- 根据量程计算出所需数据长度 ----->|
	 * +------------+--------------------------------------+-------------------+
	 * |  无效数据  |               有效数据               |       黑电平      |
	 * +------------+--------------------------------------+-------------------+
	 *
	 * 数据组合缓冲区:
	 *              |<--- 根据量程计算出所需数据长度 ----->|<------ 512 ------>|
	 *              +--------------------------------------+-------------------+
 	 *              |               有效数据               |      黑电平       |
 	 *              +--------------------------------------+-------------------+
	 *
	 * offset 的值跟量程的大小和采样分辨率有关
	 *
	**/

	//组合数据长度 = 有效数据长度 + 尾部黑电平数据长度(512)
	iCombSize = pTaskSet->pCombine_Info->iSigLen;
	pTaskSet->pCombine_Info->pCombineData = (float *)calloc(iCombSize, sizeof(float));
	pTaskSet->pCombine_Info->pOriginalData = (UINT16 *)calloc(iCombSize, sizeof(UINT16));

	for (i = 0; i < pTaskSet->iDaqNum; ++i)
	{
		//实际数据长度 = 采集数据长度 + 尾部噪声长度(512)
		iDaqSize = pTaskSet->Daq_SetQueue[i].iDaqCount + pTaskSet->Daq_SetQueue[i].iNoiseCnt;
		pTaskSet->Daq_SetQueue[i].pOrigData = (UINT16 *)calloc(iDaqSize, sizeof(UINT16));
        pTaskSet->pCombine_Info->combine_para[i].pData = (float *)calloc(iCombSize, sizeof(float));
        pTaskSet->pCombine_Info->combine_para[i].pOriData = (UINT32 *)calloc(iCombSize, sizeof(UINT32));
		pTaskSet->pCombine_Info->combine_para[i].pLogData = (float *)calloc(iCombSize, sizeof(float));
	}

    DBG_EXIT(0);
    return 0;
}


//释放采集任务的临时空间
void FreeDataBuffer(PTASK_SETTINGS pTaskSet)
{
    int i;
    DBG_ENTER();

	//释放组合数据缓冲
	free(pTaskSet->pCombine_Info->pCombineData);
	pTaskSet->pCombine_Info->pCombineData = NULL;
	free(pTaskSet->pCombine_Info->pOriginalData);
	pTaskSet->pCombine_Info->pOriginalData = NULL;

	for (i = 0; i < pTaskSet->iDaqNum; i++)
    {
        free(pTaskSet->Daq_SetQueue[i].pOrigData);
        pTaskSet->Daq_SetQueue[i].pOrigData = NULL;

        free(pTaskSet->pCombine_Info->combine_para[i].pData);
        pTaskSet->pCombine_Info->combine_para[i].pData = NULL;

        free(pTaskSet->pCombine_Info->combine_para[i].pOriData);
        pTaskSet->pCombine_Info->combine_para[i].pOriData = NULL;

		free(pTaskSet->pCombine_Info->combine_para[i].pLogData);
        pTaskSet->pCombine_Info->combine_para[i].pLogData = NULL;
    }

    DBG_EXIT(0);
}

//得到Daq采集队列
int GetDaqQueue(PTASK_SETTINGS pTaskSet)
{
    int iErr = 0;
    int iDaqPos, pulseIndex, rangeIndex;

	DBG_ENTER();

	//该模式下实时采集使用默认衰减值
    pulseIndex = pTaskSet->enPulseTime;
	rangeIndex = pTaskSet->enFiberRange;

	if (!pTaskSet->iRealTimeFlag)
		pTaskSet->iDaqNum = DaqNum[pulseIndex];//平均
	else
		pTaskSet->iDaqNum = 1;//实时

	unsigned int iAdcGap = CalAdcGapValue(pTaskSet->iDataCount + pTaskSet->iOffset,
										  pTaskSet->fSmpRatio, pTaskSet->fAutoRange,
										  pTaskSet->enPulseTime);

	/*if (pTaskSet->enFiberRange == ENUM_FIBER_RANGE_260KM && pTaskSet->enPulseTime == ENUM_PULSE_20US && pTaskSet->enAverageTime == 7)  // added by wyl
	{
		pTaskSet->iDaqNum = 5;
	}
	*/


    for (iDaqPos = 0; iDaqPos < pTaskSet->iDaqNum; ++iDaqPos)
	{
		//初始化任务队列参数
		pTaskSet->Daq_SetQueue[iDaqPos].iCloseLdPulseFlag = 0;
		pTaskSet->Daq_SetQueue[iDaqPos].iOffset = pTaskSet->iOffset;
	  	pTaskSet->Daq_SetQueue[iDaqPos].iNumInTask = iDaqPos;
		pTaskSet->Daq_SetQueue[iDaqPos].fSmpRatio = pTaskSet->fSmpRatio;
		pTaskSet->Daq_SetQueue[iDaqPos].iSmpReg = pTaskSet->iSmpReg;
	    pTaskSet->Daq_SetQueue[iDaqPos].iPulseKind = pTaskSet->enPulseTime;
		pTaskSet->Daq_SetQueue[iDaqPos].iNoiseCnt = BLACK_LEVEL_LEN;
		pTaskSet->Daq_SetQueue[iDaqPos].iAdcGap = iAdcGap;

		//实际采集的数据点数 = 需要的数据点数 + 开始跳空的数据点数
		pTaskSet->Daq_SetQueue[iDaqPos].iDaqCount = pTaskSet->iDataCount;
		pTaskSet->Daq_SetQueue[iDaqPos].iDisplayCount = pTaskSet->iDisplayCount;

        #ifdef MINI2
		// 设置衰减率
		pTaskSet->Daq_SetQueue[iDaqPos].iAtteDbClass = SetAttenuation( pTaskSet->enPulseTime,
																	   attr,iDaqPos,
																	   pTaskSet->fAutoRange,
																	   pTaskSet->iRealTimeFlag);


		// 设置累加次数
		 pTaskSet->Daq_SetQueue[iDaqPos].iFilter = SetFilters( pTaskSet->enFiberRange,
															   pTaskSet->enPulseTime,
															   attr, iDaqPos,
															   pTaskSet->iRealTimeFlag);
        #else
        pTaskSet->Daq_SetQueue[iDaqPos].iAtteDbClass = SetAttenuation(pTaskSet,iDaqPos);
		// 设置累加次数
		pTaskSet->Daq_SetQueue[iDaqPos].iFilter = SetFilters(pTaskSet, iDaqPos);
        #endif
	}

    DBG_EXIT(iErr);
    return iErr;
}
//根据采集队列，设置采集参数，并采集数据
int DaqStart(PDAQ_SETTING pDaq_Settings)
{
    POPMSET pOpmSet = pOtdrTopSettings->pOpmSet;
	POPM_TUNING pOpmTunning = pOpmSet->pOpmTuning;
	//定义的临时变量;
	int iRet = 0;
	UINT32 arrAttValue;
	UINT32 uiPulseKind;
	UINT32 uiAtteDbClass;
	UINT32 uiFilter;
	//放大、APD、VDD档数
	UINT32 uiAmpIndex = 0,uiAmpCode=0;
	UINT32 uiApdIndex = 0;
	UINT32 uiVddIndex = 0;

	//apd电压和vdd电压
	UINT32 uiApdVoltage;
	UINT32 uiVddVoltage;

	//获取光模块设备描述符
	DEVFD *pDevFd = NULL;
	int iOpmFd;

	DBG_ENTER();

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;

	uiPulseKind = pDaq_Settings->iPulseKind;
	uiAtteDbClass = pDaq_Settings->iAtteDbClass;

	//获得对应TASK_SETTINGd的放大、APD、VDD档数、ATT0值
	uiAmpIndex = pOpmSet->RegIndex[uiPulseKind]->pAmpIndex[uiAtteDbClass];
	uiApdIndex = pOpmSet->RegIndex[uiPulseKind]->pApdIndex[uiAtteDbClass];
	uiVddIndex = pOpmSet->RegIndex[uiPulseKind]->pVddIndex[uiAtteDbClass];
	arrAttValue = pOpmTunning->arrAttValue[uiAmpIndex];

    //设置ATT
	Opm_SetAttValue(iOpmFd, pOpmSet, arrAttValue);
	uiApdVoltage = GetApd_SetValue(pOpmTunning,uiApdIndex);
	//设置APD
	#ifdef MINI2
    if ((uiPulseKind > ENUM_PULSE_2US) && (uiApdVoltage > 0x10))
	{
		uiApdVoltage -= 0x10;
	}
    #else
    if (uiPulseKind < ENUM_PULSE_50NS)
	{
		uiApdVoltage += 0x02;
	}
	else if (uiPulseKind > ENUM_PULSE_500NS)
	{
		uiApdVoltage -= 0x38;
	}
    #endif

	//uiApdVoltage = 0x980;

	static unsigned int lastApdVoltage = 0x500;
	int sleepTime = 150;
	if (lastApdVoltage > uiApdVoltage)
	{
		sleepTime = Max(sleepTime, lastApdVoltage - uiApdVoltage);
	}
	else
	{
		sleepTime = Max(sleepTime, uiApdVoltage - lastApdVoltage);
	}
	Opm_SetApdVoltage(iOpmFd, pOpmSet, uiApdVoltage);
    MsecSleep(sleepTime);

	LOG(LOG_INFO, "sleepTime = %d, uiApdVoltage = 0x%x\n", sleepTime, uiApdVoltage);

	lastApdVoltage = uiApdVoltage;

	//设置AMP
	uiAmpCode = pOpmSet->AmpCode[uiAmpIndex];
	Opm_SetAmp(iOpmFd, pOpmSet, uiAmpCode);

	//设置VDD电压
	uiVddVoltage = pOpmTunning->iVddValue[uiVddIndex];
	Opm_SetVddVoltage(iOpmFd, pOpmSet, uiVddVoltage);
	MsecSleep(20);

	//设置累加次数
	uiFilter = pDaq_Settings->iFilter;

    if (!iRet)
	    iRet = Opm_SetFilter(iOpmFd,pOpmSet, uiFilter);
	//设置脉冲宽度
    if (!iRet)
    	iRet = Opm_SetPulse(iOpmFd, pOpmSet, uiPulseKind);

	//RegValueInfo(iOpmFd);

	//设置累加模式
	if (!iRet)
	    iRet = Opm_SetMode(iOpmFd,pOpmSet, ENUM_DAQ_MODE_ACC);
	//设置移相插补
	if (!iRet)
	    iRet = Opm_SetIntp(iOpmFd, pOpmSet, pDaq_Settings->iSmpReg);
    //设置采样长度，注意，设置采集的长度与读取数据的长度不一定相等，因为可能存在插补
	if (!iRet)
	    iRet = SedAdcParameter(iOpmFd, pOpmSet, pDaq_Settings->iDaqCount + pDaq_Settings->iNoiseCnt, pDaq_Settings->iAdcGap);

	//判断打开/关闭激光器
	if(!iRet)
	{
		//检测到激光器电源关闭，此时需打开，若已打开，无需重复打开
		if (!Opm_OpenLdPulse(iOpmFd, pOpmSet))
			Opm_LdPulseTurnOn(iOpmFd, pOpmSet); //打开光源
		//开始数据采集
		memset(pDaq_Settings->pOrigData, 0,
			   (pDaq_Settings->iDaqCount + pDaq_Settings->iNoiseCnt) * sizeof(UINT16));

        static UINT16 pDataTemp[MAX_DATA];
        memset(pDataTemp, 0, MAX_DATA * sizeof(UINT16));
		iRet = GetAdc_Data(iOpmFd, pOpmSet, pDataTemp);
		pDataTemp[0] = pDataTemp[1];

		// 修改ADC采集的错误数据,FPGA修改后，去掉该函数
		changeErrorData(pDataTemp, pDaq_Settings->iDaqCount + pDaq_Settings->iNoiseCnt);

		/********************************************************************************
										数据格式
		---------------------------------------------------------------------------------
		|  512 个噪声  |        offset       |                 正常数据                 |
		---------------------------------------------------------------------------------
		*********************************************************************************/
		memcpy(pDaq_Settings->pOrigData, pDataTemp + pDaq_Settings->iNoiseCnt, pDaq_Settings->iDaqCount * sizeof(UINT16));

		memcpy(pDaq_Settings->pOrigData + pDaq_Settings->iDaqCount, pDataTemp, pDaq_Settings->iNoiseCnt * sizeof(UINT16));
		//memcpy(pDaq_Settings->pOrigData, pDataTemp, (pDaq_Settings->iDaqCount + pDaq_Settings->iNoiseCnt) * sizeof(UINT16));

		/********************************************************************************
										数据格式
		---------------------------------------------------------------------------------
		|        offset       |                 正常数据                 |  512 个噪声  |
		---------------------------------------------------------------------------------
		*********************************************************************************/
	}

	//做后续的黑电平采集(512个)
	/*
	if (!iRet && (pDaq_Settings->iNoiseCnt > 0))
	{
		//设置adc参数
		iRet = SedAdcParameter(iOpmFd, pOpmSet, pDaq_Settings->iNoiseCnt,1.0f,0);

		//关闭激光器
		if (!iRet);
	    	Opm_LdPulseTurnOff(iOpmFd, pOpmSet);

		//开始数据采集
	    UINT16 *blackBuf = pDaq_Settings->pOrigData + pDaq_Settings->iDaqCount;

		iRet = GetAdc_Data(iOpmFd, pOpmSet, blackBuf);

		//去掉第一个无效数据
		blackBuf[0] = blackBuf[1];
	}
	*/

	DBG_EXIT(iRet);
	return iRet;
}

// 修改ADC采集的错误数据,FPGA修改后，去掉该函数
void changeErrorData(UINT16 *pData, int sigLen)
{
	if (sigLen <= 20)
	{
		return;
	}

	int i = 0;
	int tmpLen = Max(10000, sigLen / 3);
	tmpLen = Min(tmpLen, sigLen - 5);
	for (i = 20; i < tmpLen; ++i)
	{
		if ((pData[i] == 0) && (pData[i + 1] == 0)
			&& (pData[i + 2] > 20000)&& (pData[i - 1] > 20000))
		{
			pData[i] = (pData[i - 1] + pData[i - 2]) / 2;
			pData[i + 1] = (pData[i + 2] + pData[i + 3]) / 2;

			LOG(LOG_ERROR, "error data changed !!!\n");
			i = i + 5;
			//break;
		}
	}
}

//判断数据是否累加，如果没有累加重新采集
int judgeData(UINT16 *pData, int sigLen)
{
	if (sigLen < 2000)
	{
		return 0;
	}
	int curTaskPos = pOtdrTopSettings->pTask_Context->iCurTaskPos;
	float stdThr = 3.0f + pOtdrTopSettings->pTask_Context->Task_SetQueue[curTaskPos].enPulseTime / 2.0f;
	float fStd = GetStdUint16(pData + sigLen - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
	if (fStd > 5.0f * stdThr)
	{
		printf("------fStd = %f----------------------\n", fStd);
		printf("------Data not accumulation !!!------\n\n");
		return 1;
	}

	return 0;
}

//打开光模块电源(开机调用)
int OpenOpmPwr(POPMSET pCurrOpm)
{
	int iRet = 0;
	//获取光模块设备描述符
	DEVFD *pDevFd = NULL;
	int iOpmFd = -1;
	DBG_ENTER();
	//检测参数
	if (pCurrOpm == NULL)
	{
		iRet = -1;
	}

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;

	//打开opm电源
	Opm_EnablePwr(iOpmFd, pCurrOpm);
	//使能光模块电源后需等待稳定
	MsecSleep(200);
	DBG_EXIT(iRet);

	return iRet;
}

//关闭光模块电源(关机调用)
int CloseOpmPwr(POPMSET pCurrOpm)
{
	int iRet = 0;
	//获取光模块设备描述符
	DEVFD *pDevFd = NULL;
	int iOpmFd = -1;
	DBG_ENTER();
	//检测参数
	if (pCurrOpm == NULL)
	{
		iRet = -1;
	}

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;

	//关闭opm电源
	Opm_DisablePwr(iOpmFd, pCurrOpm);
	DBG_EXIT(iRet);

	return iRet;
}

//关闭光模块adc等的电源(开机调用)
int CloseOpm(POPMSET pCurrOpm)
{
    int iRet = 0;
	//获取光模块设备描述符
	DEVFD *pDevFd = NULL;
	int iOpmFd = -1;

    DBG_ENTER();
	if(pCurrOpm == NULL)
	{
	    iRet = -1;
	}
	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;

	//关闭ADC
	if (!iRet)
		iRet = Opm_DisableAdc(iOpmFd, pCurrOpm);

	//关闭ADC 1.8v电源控制
	if(!iRet)
	    iRet = Opm_DisableAdcVoltage(iOpmFd, pCurrOpm);

	//关闭APD电压
	if(!iRet)
	    Opm_SetApdVoltage(iOpmFd, pCurrOpm, 0);

	//关闭光模块APD电压设置
	if (!iRet)
		Opm_DisableApdPower(iOpmFd, pCurrOpm);

	//关闭光模块激光器电压设置
	if (!iRet)
		Opm_DisableLaserVoltage(iOpmFd, pCurrOpm);

	//关闭FPGA 时钟
	if(iRet)
	    Opm_ClkTurnOff(iOpmFd, pCurrOpm);

	//关闭负5v电源电源控制
	if (!iRet)
		iRet = Opm_DisablePwrCtl(iOpmFd, pCurrOpm, _C_5V_OPM_CTL);

	//关闭正5v电源电源控制
	if (!iRet)
		iRet = Opm_DisablePwrCtl(iOpmFd, pCurrOpm, C_5V_OPM_CTL);

	//关闭光模块激光器
	if (!iRet)
		iRet = Opm_LdPulseTurnOff(iOpmFd, pCurrOpm);

	//关闭光模块电源
	if(!iRet)
	    iRet = Opm_DisablePwr(iOpmFd, pCurrOpm);
    DBG_EXIT(iRet);
	return iRet;
}

//工作结束后的后续处理
int WorkFinish(POPMSET	pCurrOpm)
{
	return CloseOpm(pCurrOpm);
}


//得到脉冲时间，输入参数为用户设置脉冲与光纤实际长度
OPM_PULSE GetPulseTime(OPM_WAVE iWave, OPM_PULSE enPulseTime, FIBER_RANGE enFiberRange, float fFiberDist)
{
    OPM_PULSE enReturnPulse;
    DBG_ENTER();

    if(enPulseTime == ENUM_PULSE_AUTO)
	{
		switch(enFiberRange)
		{
		case ENUM_FIBER_RANGE_500M:
			enReturnPulse = ENUM_PULSE_20NS;
			if(fFiberDist < 0.15f) 
				enReturnPulse = ENUM_PULSE_5NS;
			break;
		case ENUM_FIBER_RANGE_1KM:
			enReturnPulse = ENUM_PULSE_50NS;
			break;
		case ENUM_FIBER_RANGE_2KM:
			//enReturnPulse = ENUM_PULSE_50NS + iWave;
            enReturnPulse = ENUM_PULSE_100NS;
			break;
		case ENUM_FIBER_RANGE_5KM:
			enReturnPulse = ENUM_PULSE_100NS;
			break;
		case ENUM_FIBER_RANGE_10KM:
			enReturnPulse = ENUM_PULSE_100NS;
			break;
		case ENUM_FIBER_RANGE_20KM:
			if(fFiberDist < 8.0f)
				enReturnPulse = ENUM_PULSE_100NS;
			else
				enReturnPulse = ENUM_PULSE_200NS;

			//enReturnPulse = ENUM_PULSE_200NS;
			break;
		case ENUM_FIBER_RANGE_50KM:
			if(fFiberDist < 8.0f)         enReturnPulse = ENUM_PULSE_100NS;
			else if(fFiberDist < 16.0f)   enReturnPulse = ENUM_PULSE_200NS;
		    else if(fFiberDist < 26.0f)   enReturnPulse = ENUM_PULSE_500NS;
		    else                          enReturnPulse = ENUM_PULSE_1US;

			//enReturnPulse = ENUM_PULSE_1US;
			break;
		case ENUM_FIBER_RANGE_100KM:
			if(fFiberDist < 8.0f)         enReturnPulse = ENUM_PULSE_100NS;
			else if(fFiberDist < 16.0f)   enReturnPulse = ENUM_PULSE_200NS;
		    else if(fFiberDist < 26.0f)   enReturnPulse = ENUM_PULSE_500NS;
		    else if(fFiberDist < 46.0f)   enReturnPulse = ENUM_PULSE_1US;
			else if(fFiberDist < 56.0f)   enReturnPulse = ENUM_PULSE_2US;
			else                          enReturnPulse= ENUM_PULSE_2US + !iWave;

			//enReturnPulse= ENUM_PULSE_2US + !i;
			break;
// 		case ENUM_FIBER_RANGE_120KM:
// 			if(fFiberDist < 8.0f)         enReturnPulse = ENUM_PULSE_100NS;
// 			else if(fFiberDist < 16.0f)   enReturnPulse = ENUM_PULSE_200NS;
// 		    else if(fFiberDist < 26.0f)   enReturnPulse = ENUM_PULSE_500NS;
// 		    else if(fFiberDist < 46.0f)   enReturnPulse = ENUM_PULSE_1US;
// 			else if(fFiberDist < 56.0f)   enReturnPulse = ENUM_PULSE_2US;
// 			else if(fFiberDist < 76.0f)   enReturnPulse = ENUM_PULSE_2US + !iWave;
// 			else                          enReturnPulse = ENUM_PULSE_10US + !iWave;
// 
// 			//enReturnPulse = ENUM_PULSE_10US + !i;
// 			break;
// 		case ENUM_FIBER_RANGE_160KM:
// 			if(fFiberDist < 8.0f)         enReturnPulse = ENUM_PULSE_100NS;
// 			else if(fFiberDist < 16.0f)   enReturnPulse = ENUM_PULSE_200NS;
// 		    else if(fFiberDist < 26.0f)   enReturnPulse = ENUM_PULSE_500NS;
// 		    else if(fFiberDist < 46.0f)   enReturnPulse = ENUM_PULSE_1US;
// 			else if(fFiberDist < 56.0f)   enReturnPulse = ENUM_PULSE_2US;
// 			else if(fFiberDist < 76.0f)   enReturnPulse = ENUM_PULSE_2US + !iWave;
// 			else if(fFiberDist < 108.0f)  enReturnPulse = ENUM_PULSE_10US + !iWave;
// 			else                          enReturnPulse = ENUM_PULSE_20US;

			//enReturnPulse = ENUM_PULSE_10US + !i;
			break;
		case ENUM_FIBER_RANGE_200KM:
			if(fFiberDist < 8.0f)         enReturnPulse = ENUM_PULSE_100NS;
			else if(fFiberDist < 16.0f)   enReturnPulse = ENUM_PULSE_200NS;
		    else if(fFiberDist < 26.0f)   enReturnPulse = ENUM_PULSE_500NS;
		    else if(fFiberDist < 46.0f)   enReturnPulse = ENUM_PULSE_1US;
			else if(fFiberDist < 56.0f)   enReturnPulse = ENUM_PULSE_2US;
			else if(fFiberDist < 76.0f)   enReturnPulse = ENUM_PULSE_2US + !iWave;
			else if(fFiberDist < 108.0f)  enReturnPulse = ENUM_PULSE_10US + !iWave;
			else                          enReturnPulse = ENUM_PULSE_20US;

			//enReturnPulse = ENUM_PULSE_20US;
			break;
		default:
			enReturnPulse = ENUM_PULSE_100NS;
			break;
		}

	}
    else//用户已经设置脉宽的情况下，使用用户设置值
    {
        enReturnPulse = enPulseTime;
    }

    DBG_EXIT(enReturnPulse);
    return enReturnPulse;
}
//根据用户设置，得到平均时间
int GetAverageTime(AVERAGE_TIME enAverageTime)
{
    int iAverageTime;
    DBG_ENTER();
	iAverageTime=0;
	switch(enAverageTime)
	{
	case ENUM_AVG_TIME_AUTO:
		iAverageTime=15;
		break;
	case ENUM_AVG_TIME_REALTIME:
		iAverageTime=1;
		break;
	case ENUM_AVG_TIME_15S:
		iAverageTime=15;
		break;
	case ENUM_AVG_TIME_30S:
		iAverageTime=30;
		break;
	case ENUM_AVG_TIME_60S:
		iAverageTime=60;
		break;
	case ENUM_AVG_TIME_90S:
		iAverageTime=90;
		break;
	case ENUM_AVG_TIME_120S:
		iAverageTime=120;
		break;
	case ENUM_AVG_TIME_180S:
		iAverageTime=180;
		break;
	default:
	    iAverageTime=15;
		break;
	}
	DBG_EXIT(iAverageTime);
	return iAverageTime;
}
//根据光纤实际长度，得到光纤范围
FIBER_RANGE GetFiberRange(float fFiberRealLength)
{
    FIBER_RANGE enFiberRange;
    DBG_ENTER();

    if(fFiberRealLength < 0.3f)         enFiberRange = ENUM_FIBER_RANGE_500M;
	else if(fFiberRealLength < 0.8f)    enFiberRange = ENUM_FIBER_RANGE_1KM;
    else if(fFiberRealLength < 1.6f)    enFiberRange = ENUM_FIBER_RANGE_2KM;
    else if(fFiberRealLength < 4.0f)    enFiberRange = ENUM_FIBER_RANGE_5KM;
    else if(fFiberRealLength < 8.0f)    enFiberRange = ENUM_FIBER_RANGE_10KM;
    else if(fFiberRealLength < 16.0f)   enFiberRange = ENUM_FIBER_RANGE_20KM;
    else if(fFiberRealLength < 40.0f)   enFiberRange = ENUM_FIBER_RANGE_50KM;
    else if(fFiberRealLength < 80.0f)   enFiberRange = ENUM_FIBER_RANGE_100KM;
    else                                enFiberRange = ENUM_FIBER_RANGE_200KM;
    DBG_EXIT(enFiberRange);
    return enFiberRange;
}

//测量量程设置到实际距离的转换 单位为米
float Range2Dist(FIBER_RANGE enRange)
{
	float fRangeSheet[OPM_DISTRANGE_KIND] =
    {
		500.0f,	//auto
    	500.0f,
		1000.0f,
		2000.0f,
		5000.0f,
		10000.0f,
		20000.0f,
		50000.0f,
		100000.0f,
		200000.0f
	};

	return fRangeSheet[enRange];
}

//根据光纤范围，得到显示数据点数
int GetSmpCount(FIBER_RANGE enFiberRange, float fRefractiveIndex, float fRatio)
{
    float fRangeToLength[OPM_DISTRANGE_KIND] =
    {0.5f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f, 20.0f, 50.0f, 100.0f, 200.0f};

	float fRealLength = fRangeToLength[enFiberRange];//得到光纤范围对应的实际长度;
    DBG_ENTER();

	if(enFiberRange == ENUM_FIBER_RANGE_AUTO)
	{
		LOG(LOG_ERROR, "enFiberRange is ENUM_FIBER_RANGE_AUTO\n");
		return -1;
	}

	int smpCnt = CalSamplePointNum(fRealLength, fRefractiveIndex, fRatio);

    return smpCnt;
}


//根据脉宽类型和是否移相，计算盲区数据点数
int GetBlindLength(OPM_PULSE enPulse, float iRatio)
{
	int iBlindLength = 0;
	int iPulseTime = 0;		//单位ns
	#ifdef MINI2
	int arrPulseTime[OPM_PULSE_KIND] = {5000, 5, 10, 20, 30, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
    #else
    int arrPulseTime[OPM_PULSE_KIND] = {5000, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
    #endif

	if (enPulse == ENUM_PULSE_AUTO)
		iPulseTime = arrPulseTime[4];
	else
		iPulseTime = arrPulseTime[enPulse];

    iBlindLength = CalBlindWidth(iPulseTime,iRatio);

	return iBlindLength;
}
//初始化采集队列
int InitDaqQueue(PDAQ_QUEUE pDaq_Queue, int iMaxQueueNum)
{
    int iRet = 0;
	pDaq_Queue->front = 0;
	pDaq_Queue->rear = 0;
	pDaq_Queue->iMaxQueueNum = iMaxQueueNum + 1;
	return iRet;
}
//写入数据采集队列
int WriteDaqQueue(PDAQ_QUEUE pDaq_Queue, PDAQ_SETTING pDaq_Setting)
{
    int iRet = 0;
	DBG_ENTER();

	MutexLock(&mDaqMutex);
	if (((pDaq_Queue->rear + 1) % (pDaq_Queue->iMaxQueueNum)) == pDaq_Queue->front)
	{
		iRet = -1;
	}
	else
	{
		pDaq_Queue->pDaq_SetQueue[pDaq_Queue->rear] = pDaq_Setting;
		pDaq_Queue->rear = (pDaq_Queue->rear + 1) % (pDaq_Queue->iMaxQueueNum);
	}
	MutexUnlock(&mDaqMutex);

	DBG_EXIT(iRet);
	return iRet;
}
//读取数据采集队列
PDAQ_SETTING ReadDaqQueue(PDAQ_QUEUE pDaq_Queue)
{
    PDAQ_SETTING iRet = NULL;

	MutexLock(&mDaqMutex);
	if (pDaq_Queue->rear == pDaq_Queue->front)
	{
		iRet = NULL;
	}
	else
	{
		iRet = pDaq_Queue->pDaq_SetQueue[pDaq_Queue->front];
		pDaq_Queue->front = (pDaq_Queue->front + 1) % (pDaq_Queue->iMaxQueueNum);
	}
	MutexUnlock(&mDaqMutex);

	return iRet;
}
//返回数据采集队列的三种状态:-1:Empty;0:Nomal;1:OverFolw
int CheckDaqQueue(PDAQ_QUEUE pDaq_Queue)
{
    int iRet;

	MutexLock(&mDaqMutex);
    if (pDaq_Queue->rear == pDaq_Queue->front)
        iRet = -1;
    else if (((pDaq_Queue->rear + 1) % (pDaq_Queue->iMaxQueueNum)) == pDaq_Queue->front)
        iRet = 1;
    else
        iRet = 0;
	MutexUnlock(&mDaqMutex);

    return iRet;
}

//初始化命令队列
int InitCommandQueue(COMMAND_QUEUE *pComQueue)
{
	int iRet = 0;
	pComQueue->front = 0;
	pComQueue->rear = 0;
	return iRet;
}
//发送命令
int SendCommand(COMMAND_QUEUE *pComQueue, int Command)
{
	int iRet = 0;
	if (((pComQueue->rear + 1) % COMSIZE) == pComQueue->front)
	{
		LOG(LOG_ERROR, "ComQueue is Overflow!\n");
		iRet = 1;
	}
	else
	{
		pComQueue->rear = (pComQueue->rear + 1) % COMSIZE;
		pComQueue->command[pComQueue->rear] = Command;
		//LOG(LOG_INFO,"rear = %d, front = %d, command = %d\n",pComQueue->rear,pComQueue->front,Command);
	}
	return iRet;
}
//读取命令
int ReadCommand(COMMAND_QUEUE *pComQueue)
{
	int iRet = 0;
	if (pComQueue->rear == pComQueue->front)
	{
		iRet = -1;
		//LOG(LOG_INFO,"No Command, iRet = %d\n",iRet);
	}
	else
	{
		pComQueue->front = (pComQueue->front + 1) % COMSIZE;
		iRet = pComQueue->command[pComQueue->front];
		//LOG(LOG_INFO,"rear = %d, front = %d, command = %d\n",pComQueue->rear,pComQueue->front,iRet);
	}
	return iRet;
}

//得到黑电平,并得到黑电平的方差
/*
*	采集黑电平:即采集噪声，两种办法:	 1-激光器脉冲关闭   2-VDD设置为0
*	1.把激光器脉冲关闭的话，设置的波长、脉宽、VDD都不会起作用，也就是白设置。
*	2.如果激光器不关闭的话，就把VDD设置为0，这样也可以采到黑电平(噪声)。就是说激光器脉冲电源
*	  打开也不起作用,脉宽设置和波长设置也都不起作用。
*/
int GetBlack_Level(POPMSET pOpmSet, float *fStd)
{
	UINT16 *pDataBuf = NULL;
	int iLen;
	DEVFD *pDevFd = NULL;
	int iOpmFd;
	OPM_FILTER filter;
	UINT32 gap = 0x100;
	OPM_DAQ_CMD iDaqCmd;

	//DBG_ENTER();
    //检测输入参数是否正确;
    if((pOpmSet == NULL) || (fStd == NULL))  return -1;

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;

	//采集 1024个数据
	iLen = 1024;

    if(pDataBuf != NULL)
        free(pDataBuf);
	pDataBuf = (UINT16 *)calloc(iLen, sizeof(UINT16));

	//设置脉冲
	Opm_SetPulse(iOpmFd, pOpmSet, ENUM_PULSE_100NS);

	//激光器脉冲关闭
	Opm_LdPulseTurnOff(iOpmFd, pOpmSet);

	//设置移相位1:1
	Opm_SetPhase(iOpmFd);
	//设置DAQMOD_SET为1:1采集
	Opm_SetIntp(iOpmFd, pOpmSet, INTP01_01);

	//设置数据采集模式	累加模式
    Opm_SetMode(iOpmFd, pOpmSet, DAQMODE_ACC);

	//设置滤波
	filter = _IIR_256;
    Opm_SetFilter(iOpmFd, pOpmSet, filter);

	//停止数据采集，设置DAQ_CTRL
	iDaqCmd = ENUM_DAQ_CMD_STOP;
	Opm_SetDaqCmd(iOpmFd, pOpmSet, iDaqCmd);

	//设置数据采集的ADC基本参数 分别是 采集个数和采集间隔
    SedAdcParameter(iOpmFd, pOpmSet, iLen, gap);

	//得到采集的数据
    GetAdc_Data(iOpmFd, pOpmSet, pDataBuf);

	//把激光器脉冲打开
    Opm_LdPulseTurnOn(iOpmFd, pOpmSet);

	pDataBuf[0] = pDataBuf[1];

	changeErrorData(pDataBuf, iLen);
	// 均值
	int mean = GetMeanUint16(pDataBuf, BLACK_LEVEL_LEN);

	// 方差
	*fStd = GetStdUint16(pDataBuf, BLACK_LEVEL_LEN);

    if(pDataBuf != NULL)
    {
        free(pDataBuf);
        pDataBuf = NULL;
    }

	DBG_EXIT(mean);
    return(mean);
}

//得到采样数据
int  GetAdc_Data(
int		iOpmFd,			//指向OPM的设备指针
OPMSET	*pCurrOpm,		//OPM set的指针
UINT16	*pBuf			//用来接收采集数据的缓冲区
)
{
	OPM_DAQ_CMD    iDaqCmd;
	int i,iRet = 0;

	DBG_ENTER();

	//发出DAQ停止的命令
	iDaqCmd=ENUM_DAQ_CMD_STOP;
	Opm_SetDaqCmd(iOpmFd,pCurrOpm,iDaqCmd);

	i=0;
	//读取DAQ的状态，如果正忙，则要先停止数据采集
	while(Opm_GetDaqStatus(iOpmFd))
	{
		i++;
		MsecSleep(20);
		if(i>600)
		{
			//ADC 停止超时
			SendMessageError(1);
			return -1;
		}
	}
	//发出开始采集命令
	iDaqCmd=ENUM_DAQ_CMD_START;
	Opm_SetDaqCmd(iOpmFd,pCurrOpm,iDaqCmd);

	i=0;
	//读取ADC_DATA_READY的状态，如果为0，则要数据采集还未完成
	do
	{
		i++;
		MsecSleep(10);

		//ADC采样超时
		if(i>1000)
		{
			//ADC 停止超时
			SendMessageError(2);
			iRet=1;
			break;
		}
	}while(!Opm_AdcReady(iOpmFd));

	//发出DAQ停止的命令
	iDaqCmd=ENUM_DAQ_CMD_STOP;
	Opm_SetDaqCmd(iOpmFd,pCurrOpm,iDaqCmd);

	//发出读取DDR数据命令
	iDaqCmd=ENUM_DAQ_CMD_READ;
	Opm_SetDaqCmd(iOpmFd,pCurrOpm,iDaqCmd);

	i=0;
	//读取CPU_RD_DDR_READY的状态，如果为0，则要DDR中数据未准备好
	while(!Opm_DataReady(iOpmFd))
	{
		i++;
		MsecSleep(50);

		//DDR Ready超时
		if(i>100)
		{
			//ADC 停止超时
			SendMessageError(3);
			iRet=1;
			break;
		}
	}

	Opm_ReadAdcData(iOpmFd, pCurrOpm, pBuf); // 2013.12.19

	DBG_EXIT(iRet);
	return iRet;
}

//用来显示错误消息的
void SendMessageError(int iMessageID)
{
	//得到当前OTDR窗口
	//GUIWINDOW *pFrmOtdr = GetCurrWindow();
	LOG(LOG_ERROR, "%s: Error = %d\n", __func__, iMessageID);
}

//获得APD设置寄存器值
UINT32 GetApd_SetValue(POPM_TUNING pOpmTunning, int iDbNum)
{
	float fTemperature;
	INT32  uiAd;
	//获取光模块设备描述符
	DEVFD *pDevFd = NULL;
	int iOpmFd;

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;

	GetOpmTemperature(iOpmFd, &fTemperature);//得到温度

	//uiAd = pOpmTunning->iApdVoltageSet[iDbNum];
	uiAd = pOpmTunning->iApdVoltageSet[iDbNum] + (UINT32)((fTemperature - pOpmTunning->fTemperatureSet) * pOpmTunning->fTempCoefficient[iDbNum]);
	
	if(uiAd < 0)
	{
		uiAd = 0;
		LOG(LOG_ERROR,"ApdVoltage < 0\n");
	}
	else if(uiAd > 0x0f00)
	{
		uiAd = 0x0870;
		LOG(LOG_ERROR,"ApdVoltage > 0xf000\n");
	}
	// LOG(LOG_INFO,"iApdSet[%d] = %x, fTemp = %5.2f, fCoeffi = %5.2f, uiAd = %x\n",
    //     iDbNum,pOpmTunning->iApdVoltageSet[iDbNum],fTemperature,pOpmTunning->fTempCoefficient[0],uiAd);

	return (UINT32)uiAd;
}


//设置任务上下文中多个任务部分的曲线参数，显示最终组合数据
void SetTaskCurvePara(PTASK_CONTEXTS pTask_Contexts)
{
    int i;
    PTASK_SETTINGS pTaskSet = NULL;
    CURVE_PRARM Param;
	DATA_IN	dIn;
    DBG_ENTER();

    if(NULL == pTask_Contexts)   return;


	//添加根据任务添加曲线
	for (i = 0; i < pTask_Contexts->iTaskNum; ++i)
	{
		pTaskSet = &pTask_Contexts->Task_SetQueue[i];

		//时间戳
		Param.ulDTS = time(NULL);

		//基本参数
		Param.enTime = (UINT32)pTaskSet->enAverageTime;
		Param.enWave = (UINT32)pTaskSet->enWave;
		Param.enRange = (UINT32)pTaskSet->enFiberRange;
		Param.enPluse = (UINT32)pTaskSet->enPulseTime;

		Param.uiAvgTime = pTaskSet->iAverageTime;
		Param.iFactor = SCALE_FACTOR;
		Param.fDistance = pTaskSet->fRangeDist;
		Param.fSmpIntval = pTaskSet->fSmpIntval;
		Param.fSmpRatio = pTaskSet->fSmpRatio;

		//分析参数
		Param.fRefractive = pTaskSet->pAlgorithm_info->SetParameters.fRefractiveIndex;
		Param.fBackScatter = pTaskSet->pAlgorithm_info->SetParameters.fBackScattering;
		Param.fEndThr = pTaskSet->pAlgorithm_info->SetParameters.fFiberEndThreshold;
		Param.fLossThr = pTaskSet->pAlgorithm_info->SetParameters.fLossThreshold;
		Param.fReturnThr = pTaskSet->pAlgorithm_info->SetParameters.fReflectThreshold;

		//数据输入
		dIn.uiCnt = pTaskSet->iDisplayCount;
		dIn.pData = (UINT16 *)calloc(sizeof(UINT16), pTaskSet->iDisplayCount);
		if (NULL == dIn.pData)
		{
			break;
		}

		//添加曲线
		DisplayAddCurve(pOtdrTopSettings->pDisplayInfo, &Param, &dIn);

		free(dIn.pData);
	}

	//发送初始化命令
	SetCurrCurvePos(0, pOtdrTopSettings->pDisplayInfo);
	SendLineCtlMoveMsg(EN_CTL_INIT, 0, 0);

    DBG_EXIT(0);
}

//启动otdr开始测量
//add by  2015.05.23
int RunOtdrModule(int arg)
{
	//错误标志、返回值定义
	int iErr = 0;
	COMMAND_QUEUE *pComQueue = NULL;

	pComQueue = pOtdrTopSettings->pComQueue;
	do
	{
		iErr = SendCommand(pComQueue, arg);
		MsecSleep(100);
	}while(iErr);	//判断是否发送成功

	return iErr;
}




//通道组合完成后，复制数据到显示缓冲区
void CombData2DispInfo(PTASK_CONTEXTS pTskContext, COMBINE_INFO *pCombInfo)
{
	int i, iCnt;
	float fMin, fTmp;
	//float fNoiseMax, fNoiseStd, fNoiseMean;
	DATA_IN dIn;
	//int NN = 65535;

	PTASK_SETTINGS pTskSet = &pTskContext->Task_SetQueue[pTskContext->iCurTaskPos];
    PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;

	//资源申请
	dIn.uiCnt = pTskSet->iDisplayCount;
	dIn.pData = (UINT16 *)calloc(sizeof(UINT16), dIn.uiCnt);
	if (NULL == dIn.pData)
	{
		return;
	}

	//处理组合完成的数据
	iCnt = pCombInfo->iSigLen;

	if (iCnt <= BLACK_LEVEL_LEN)
	{
		fMin = 0.0f;
	}
	else
	{
		fMin = GetMinFloat(pCombInfo->pCombineData + iCnt - BLACK_LEVEL_LEN,
					   BLACK_LEVEL_LEN);
		/*if (pTskSet->enPulseTime == 11 && pTskSet->enFiberRange == 10 && pTskSet->enAverageTime == 7)
		{
			fNoiseMax = GetMaxFloat(pCombInfo->pCombineData + iCnt - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN) + 0.1;
			fNoiseStd = GetStdFloat(pCombInfo->pCombineData + iCnt - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
			fNoiseMean = GetMeanFloat(pCombInfo->pCombineData + iCnt - BLACK_LEVEL_LEN, BLACK_LEVEL_LEN);
			for (i = 0; i < iCnt; i++)
			{
				if (pCombInfo->pCombineData[i] < fNoiseMax && pCombInfo->pCombineData[i] > fMin)
				{
					pCombInfo->pCombineData[i] = pCombInfo->pCombineData[i] * 0.8;
				}
				else if (pCombInfo->pCombineData[i] <= fMin)
				{
					pCombInfo->pCombineData[i] = ((float)(rand() % NN) / (float)NN - 0.5) * fNoiseStd * 2 + fNoiseMean;
				}
			}
		}
		fMin = GetMinFloat(pCombInfo->pCombineData + iCnt - BLACK_LEVEL_LEN,
					   BLACK_LEVEL_LEN);
					   */
		
		if (fMin > 0.0f)
		{
			fMin = 0.0f;
		}
	}

	for (i = 0; i < dIn.uiCnt;  i++)
	{
		fTmp = pCombInfo->pCombineData[i] - fMin;

		if (fTmp >= 0.0f)
		{
			dIn.pData[i] = (UINT16)(SCALE_FACTOR * fTmp);
		}
		else
		{
			dIn.pData[i] = 0;
		}
	}


	//修改曲线相关信息
	SetCurrCurvePos(pTskContext->iCurTaskPos, pDisplay);
	SetCurveData(CURR_CURVE, pDisplay, &dIn);
	SendLineCtlMoveMsg(EN_CTL_NEWDATA, 0, 0);

	//资源释放
	free(dIn.pData);
}

/*
**函数说明:
	获取sor 文件名
**参数说明:

**返回值:
	无
*/
void GetFileName(char *filename)
{
    FILE_NAME_SETTING fileNameSetting = pOtdrTopSettings->pUser_Setting->sFileNameSetting;
    char name[128];
    unsigned int iSuffix;
    int i, max;
    if(!filename)
    {
        return;
    }
    max = pow(10, fileNameSetting.suffixWidth);
    iSuffix = fileNameSetting.suffix%max ? fileNameSetting.suffix%max : 1;
    for(i=0; i<max; i++)
    {
        char temp[512];
        int isGetName = 1;


        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd_1310%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".SOR");
            sprintf(name, temp, iSuffix);
            isGetName = !isFileExist(name, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
        }

        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd_1550%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".SOR");
            sprintf(name, temp, iSuffix);
            isGetName = !isFileExist(name, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
        }

 
        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".CUR");
            sprintf(name, temp, iSuffix);
            isGetName = !isFileExist(name, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
        }

        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd_1310%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".CUR");
            sprintf(name, temp, iSuffix);
            isGetName = !isFileExist(name, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
        }

        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd_1550%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".CUR");
            sprintf(name, temp, iSuffix);
            isGetName = !isFileExist(name, pOtdrTopSettings->sDefSavePath.cOtdrSorSavePath);
        }
        
        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".PDF");
            sprintf(name, temp, iSuffix);
            isGetName = !isFileExist(name, pOtdrTopSettings->sDefSavePath.cOtdrReportSavePath);
        }

        if(pOtdrTopSettings->pDisplayInfo->pCurve[pOtdrTopSettings->pTask_Context->iCurTaskPos]->pParam.enWave == WAVELEN_1310)
        {
            sprintf(temp, "%s%%0%dd_1310%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".SOR");
            sprintf(filename, temp, iSuffix);
        }
        else
        {
            sprintf(temp, "%s%%0%dd_1550%s",fileNameSetting.prefix, fileNameSetting.suffixWidth, ".SOR");
            sprintf(filename, temp, iSuffix);
        }

        if(!isGetName)
        {
            if(fileNameSetting.suffixRule)
            {
                if(++iSuffix == max)
                {
                    iSuffix = 1;
                }
            }
            else
            {
                if(--iSuffix == 0)
                {
                    iSuffix = max - 1;
                }
            }

            if(iSuffix == fileNameSetting.suffix)
            {
                break;
            }
        }
        else
        {
            pOtdrTopSettings->pUser_Setting->sFileNameSetting.suffix = iSuffix;
			LOG(LOG_INFO, "otdr file name : %s\n", filename);
			break;
        }
    }
}



//下面是保存数据的代码 added by  2015.9.6
/*
说明；读取U盘里sn.txt的文件序列号，然后把它返回来，如果不存在，则直接创建sn.txt
作者；
日期；2015.03.31
返回值；0=失败，
        正整数=要创建的序列号
*/
int GetSnNum(void)
{
	int sn;
	FILE * fp=0;
	sn=0;
	char cBuff[64];

	fp = fopen(MntUsbDirectory"/sn.txt",  "r+");
	if(!fp)
	{
		fp = fopen(MntUsbDirectory"/sn.txt",  "w+");
	}
	if(fp)
	{
		fseek(fp,0,SEEK_SET);
		fread(cBuff, sizeof(cBuff), 1, fp);
		sn = atoi(cBuff);
		//rtn=fscanf(fp,"%d",&sn);
		DISP_LINE(sn);
		sn++;
		fseek(fp,0,SEEK_SET);
		fprintf(fp,"%d",sn);
		fclose(fp);
	}
	return sn;
}


/*
说明:
	用来做调试用的，在数据采集完毕后，把每个任务缓冲区的数据做完平均后，保存到文件当中
作者:
	
日期:
	2013.11.28
*/
void SaveEachBufdata()
{
	int i,iCount;
	UINT16 *pBuf;
	UINT32 iAdcCnt;
	int uiPulseKind,uiAtteDbClass;
	int uiAmpIndex,uiApdIndex,uiVddIndex,uiAmpCode;

	PTASK_CONTEXTS 			pCurrTaskContext = NULL;
	TASK_SETTINGS   		TaskSetting;
	DAQ_SETTING	   			*pDaqSetting = NULL;

	POPMSET					pOpmSet=NULL;
	UINT32					iCurrentPos = 0;
	UINT32					iTaskNum = 0;
	int iSn;
    UINT16 iFlagbuf[11];
	//获取光模块设备描述符
	DEVFD *pDevFd = NULL;
	int iOpmFd = -1;

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;
	DBG_ENTER();
	pCurrTaskContext=pOtdrTopSettings->pTask_Context;
	iCurrentPos = pCurrTaskContext->iCurTaskPos;
	TaskSetting = pCurrTaskContext->Task_SetQueue[iCurrentPos];
	iTaskNum = TaskSetting.iDaqNum;

	pOpmSet=pOtdrTopSettings->pOpmSet;
	iAdcCnt=Opm_GetCount(iOpmFd,pOpmSet);

	pDaqSetting = TaskSetting.Daq_SetQueue;
	iSn=GetSnNum();
    if(!iSn)//如果文件检查失败，则不保存数据
    {
        DISP_LINE(iSn);
        return;
    }
	for(i=0;i<iTaskNum;i++)
	{
		pBuf=pDaqSetting[i].pOrigData + pDaqSetting[i].iOffset;
		iAdcCnt=pDaqSetting[i].iDaqCount+pDaqSetting[i].iNoiseCnt-pDaqSetting[i].iOffset;
		iCount=pDaqSetting[i].iFilter;
		uiPulseKind = pDaqSetting[i].iPulseKind;
		uiAtteDbClass = pDaqSetting[i].iAtteDbClass;
		uiAmpIndex = pOpmSet->RegIndex[uiPulseKind]->pAmpIndex[uiAtteDbClass];
		uiApdIndex = pOpmSet->RegIndex[uiPulseKind]->pApdIndex[uiAtteDbClass];
		uiVddIndex = pOpmSet->RegIndex[uiPulseKind]->pVddIndex[uiAtteDbClass];
		uiAmpCode = pOpmSet->AmpCode[uiAmpIndex];

		//把通道设置信息添加到数据开头
		iFlagbuf[0]=9;
		iFlagbuf[1]=i;
		iFlagbuf[2]=uiPulseKind;
		iFlagbuf[3]=uiAtteDbClass;
		iFlagbuf[4]=uiAmpIndex;
		iFlagbuf[5]=uiAmpIndex;
		iFlagbuf[6]=uiApdIndex;
		iFlagbuf[7]=uiVddIndex;
		iFlagbuf[8]=uiAmpCode;
		iFlagbuf[9]=uiAmpCode;
		SaveOtdrData(iSn,i,iAdcCnt,pBuf);
	}

    if(mysystem("cp /usr/O10Data*.txt "MntUsbDirectory))
    {
		LOG(LOG_ERROR, "copy to /media faild\n");
	}
    else
    {
        mysystem("rm /usr/O10Data*.txt -rf");
		LOG(LOG_INFO, "copy O10Data*.txt /mnt/usb\n");
	}

	int fd = open("/usr/file_valid", O_CREAT | O_RDWR, 0666);
	close(fd);

	DBG_EXIT(0);
}

/*
*返回值:
* 成功----返回对应文件描述符
* 失败----返回0
*/
int OpenFile(
int iSn, //该文件组序列号
int iNum //第几组数据
)
{
	char fpath[1024];
	sprintf(fpath, MntUsbDirectory"/data/O10Data%03d_%d.txt", iSn, iNum);
	//打开文件并返回
	fid = fopen(fpath, "w+");
	if (fid != NULL)
	{
		return (int)fid;
	}
		else
	{
		return 0;
	}
}

/*
 *参数:
 *
 *返回值:
 *		成功---- 返回0
 *		失败---- 返回其他
 */
int CloseFile(void)
{
	if(!fid)
	{
		LOG(LOG_ERROR, "Invalid file\n");
		return -1;
	}

	//关闭文件
	fclose(fid);

	fid=0;
	//sleep(1);

	return 0;
}


int GetFid(void)
{
	return (int)fid;
}


void SaveOtdrData(      //把一定长度的数据保存到文件当中去
int iSn,                //存储一组数据的序列号
int iNum,               //第几组数据
int len,                //保存数据的长度
UINT16 *DataBuf         //数据缓冲区
)
{
	int i;
	FILE * pFid=NULL;
	OpenFile(iSn,iNum);
	pFid=(FILE * )GetFid();

	if(pFid)
	{
		for(i=0; i<len; i++)
			fprintf(pFid,"%d\r\n", DataBuf[i]);

	}
	CloseFile();
}

int SaveAdcData(PTASK_SETTINGS pTaskSet, int iTaskPos, int iCnt, char *strPath)
{
	int i = 0;
	char strFile[512];
	FILE *pFile = NULL;
	PDAQ_SETTING pDaqSet = &pTaskSet->Daq_SetQueue[pTaskSet->iCurDaqPos];

	// 输入前三个变量分别为: 脉宽、盲区、每个点距离
	float iFlagbuf[3]={(float)pDaqSet->iPulseKind,(float)pTaskSet->iBlindLength,pDaqSet->fSmpRatio};

	sprintf(strFile, "%s/O10Data%d_%d.txt", strPath, iCnt, iTaskPos);

	pFile = fopen(strFile, "w");
	if (pFile)
	{
		for (i = 0; i < 3;i++)
		{
			fprintf(pFile, "%f\n\r", iFlagbuf[i]);
		}
		for (i = 0; i < pDaqSet->iDaqCount + pDaqSet->iNoiseCnt; ++i)
		{
			fprintf(pFile, "%u\n\r", pDaqSet->pOrigData[i]);
		}

		fclose(pFile);
		return 0;
	}

	return -1;
}

void RegValueInfo(int iOpmFd)
{
	if(iOpmFd <= 0)
	{
		return;
	}

	UINT32 iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &iReg);
	LOG(LOG_INFO,"FPGA_CTRLOUT_REG %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, OPM_CTRLOUT_REG, &iReg);
	LOG(LOG_INFO, "OPM_CTRLOUT_REG %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_LD_PULSE_WIDTH, &iReg);
	LOG(LOG_INFO, "FPGA_LD_PULSE_WIDTH %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_CPU_RD_DDR2_ADDRL, &iReg);
	LOG(LOG_INFO, "FPGA_CPU_RD_DDR2_ADDRL %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_CPU_RD_DDR2_ADDRH, &iReg);
	LOG(LOG_INFO, "FPGA_CPU_RD_DDR2_ADDRH %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_CPU_WR_DDR2_ADDRH, &iReg);
	LOG(LOG_INFO, "FPGA_CPU_WR_DDR2_ADDRH %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_CPU_WR_DDR2_ADDRL, &iReg);
	LOG(LOG_INFO, "FPGA_CPU_WR_DDR2_ADDRL %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_CPU_WR_DDR2_DATA, &iReg);
	LOG(LOG_INFO, "FPGA_CPU_WR_DDR2_DATA %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_FILTER_SEL, &iReg);
	LOG(LOG_INFO, "FPGA_FILTER_SEL %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_DAQ_CTRL, &iReg);
	LOG(LOG_INFO, "FPAG_DAQ_CTRL %04x\n", iReg);

	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_DAQMODE_SET, &iReg);
	LOG(LOG_INFO, "FPAG_DAQMODE_SET %04x\n", iReg);
}


//调试PSOR_DATA结构里面的数据
void PrintSorData(PSOR_DATA pSor)
{
	LOG(LOG_INFO, "---------------------------------------------------\n");

	LOG(LOG_INFO, "----iWave = %d----\n", pSor->iWave);
	LOG(LOG_INFO, "----iPulse = %d----\n", pSor->iPulse);
	LOG(LOG_INFO, "----iAverageTime = %d----\n", pSor->iAverageTime);
	LOG(LOG_INFO, "----lAcquisitionDist = %ld----\n", pSor->lAcquisitionDist);
	LOG(LOG_INFO, "----cFileName = %s----\n", pSor->cFileName);
	LOG(LOG_INFO, "----iEventAnalysisFlag = %d----\n", pSor->iEventAnalysisFlag);
	LOG(LOG_INFO, "----fSmpRatio = %f----\n", pSor->fSmpRatio);
	LOG(LOG_INFO, "----fRefractive = %f----\n", pSor->fRefractive);
	LOG(LOG_INFO, "----fBackScattering = %f----\n", pSor->fBackScattering);
	LOG(LOG_INFO, "----iLossMethodMark = %i----\n", pSor->iLossMethodMark);
	LOG(LOG_INFO, "----fSpliceLossThr = %f----\n", pSor->fSpliceLossThr);
	LOG(LOG_INFO, "----fReturnLossThr = %f----\n", pSor->fReturnLossThr);
	LOG(LOG_INFO, "----fEndLossThr = %f----\n", pSor->fEndLossThr);
	LOG(LOG_INFO, "----iDataCount = %d----\n", pSor->iDataCount);
	LOG(LOG_INFO, "----iScaleFactor = %d----\n", pSor->iScaleFactor);
	LOG(LOG_INFO, "----iEventsNumber = %d----\n", pSor->eventTable.iEventsNumber);
	LOG(LOG_INFO, "---------------------------------------------------\n");
}


//实际脉宽转化为枚举类型
static OPM_PULSE PulseToEnum(int iPulse)
{
	OPM_PULSE enPulse = ENUM_PULSE_AUTO;

    int count = 0;
    #ifdef MINI2
    const int iPulseArray[12] = {5, 10, 20, 30, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
    const OPM_PULSE enPulseArray[12] = {ENUM_PULSE_5NS, ENUM_PULSE_10NS, ENUM_PULSE_20NS,
                                        ENUM_PULSE_30NS, ENUM_PULSE_50NS, ENUM_PULSE_100NS,
                                        ENUM_PULSE_200NS, ENUM_PULSE_500NS, ENUM_PULSE_1US,
                                        ENUM_PULSE_2US, ENUM_PULSE_10US, ENUM_PULSE_20US};
    count = 12;
    #else
    const int iPulseArray[11] = {5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
    const OPM_PULSE enPulseArray[11] = {ENUM_PULSE_5NS, ENUM_PULSE_10NS, ENUM_PULSE_20NS,
                                        ENUM_PULSE_50NS, ENUM_PULSE_100NS, ENUM_PULSE_200NS,
                                        ENUM_PULSE_500NS, ENUM_PULSE_1US, ENUM_PULSE_2US,
                                        ENUM_PULSE_10US, ENUM_PULSE_20US};
    count = 11;
    #endif
    int i;
    for(i = 0; i < count; ++i)
    {
        if(iPulse <= iPulseArray[i])
        {
            enPulse = enPulseArray[i];
            break;
        }
    }

	return enPulse;
}


//实际平均之间转化为枚举类型
static AVERAGE_TIME AvgTimeToEnum(int iAverageTime)
{
	AVERAGE_TIME enAvgTime = ENUM_AVG_TIME_AUTO;

	if(iAverageTime >= 180)
	{
        enAvgTime = ENUM_AVG_TIME_180S;
	}
	else if(iAverageTime >= 120)
	{
        enAvgTime = ENUM_AVG_TIME_120S;
	}
	else if(iAverageTime >= 90)
	{
        enAvgTime = ENUM_AVG_TIME_90S;
	}
	else if(iAverageTime >= 60)
	{
        enAvgTime = ENUM_AVG_TIME_60S;
	}
	else if(iAverageTime >= 30)
	{
        enAvgTime = ENUM_AVG_TIME_30S;
	}
	else if(iAverageTime >= 15)
	{
        enAvgTime = ENUM_AVG_TIME_15S;
	}
    else if(1 == iAverageTime)
    {
        enAvgTime = ENUM_AVG_TIME_REALTIME;
    }

	return enAvgTime;
}

//实际长度转化为枚举类型(量程的80%)
static FIBER_RANGE LengthToEnum(int iLength)
{
	FIBER_RANGE enDisplayLength = ENUM_FIBER_RANGE_AUTO;

	if(iLength <= 400)
	{
		enDisplayLength = ENUM_FIBER_RANGE_500M;
	}
	else if(iLength <= 800)
	{
		enDisplayLength = ENUM_FIBER_RANGE_1KM;
	}
	else if(iLength <= 1600)
	{
		enDisplayLength = ENUM_FIBER_RANGE_2KM;
	}
	else if(iLength <= 4000)
	{
		enDisplayLength = ENUM_FIBER_RANGE_5KM;
	}
	else if(iLength <= 8000)
	{
		enDisplayLength = ENUM_FIBER_RANGE_10KM;
	}
	else if(iLength <= 16000)
	{
		enDisplayLength = ENUM_FIBER_RANGE_20KM;
	}
	else if(iLength <= 40000)
	{
		enDisplayLength = ENUM_FIBER_RANGE_50KM;
	}
	else if(iLength <= 80000)
	{
		enDisplayLength = ENUM_FIBER_RANGE_100KM;
	}
	else if(iLength <= 160000)
	{
		enDisplayLength = ENUM_FIBER_RANGE_200KM;
	}
	else
		enDisplayLength = ENUM_FIBER_RANGE_200KM;

	return enDisplayLength;
}

static SAMPLE_INTERVAL GetSmpMode(FIBER_RANGE len, float smp)
{
	SAMPLE_INTERVAL iSmp = ENUM_SMP_NORMAL;

	if(len == 11)
		return ENUM_SMP_NORMAL;

	iSmp = fabsf(fSampleRatio[len][0]-smp) < fabsf(fSampleRatio[len][1]-smp) ? ENUM_SMP_NORMAL : ENUM_SMP_HIGH_RES;

	return iSmp;
}

//
static int Sor2Otdr(PSOR_DATA pSor, int clearCurve)
{
	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
	PUSER_SETTINGS pUser_Settings = pOtdrTopSettings->pUser_Setting;
	CURVE_PRARM Param;
	DATA_IN dIn;

	OPM_WAVE enWave = ENUM_WAVE_1310NM;

	if (pSor->iWave == 1310)
	{
		enWave = ENUM_WAVE_1310NM;
		pUser_Settings->iWave[0] = 1;
		if(clearCurve)
		{
    		pUser_Settings->iWave[1] = 0;
    		pUser_Settings->iWave[2] = 0;
    	}
	}
	if (pSor->iWave == 1550)
	{
		enWave = ENUM_WAVE_1550NM;
		pUser_Settings->iWave[1] = 1;
		if(clearCurve)
		{
    		pUser_Settings->iWave[0] = 0;
    		pUser_Settings->iWave[2] = 0;
    	}
	}
	if (pSor->iWave == 1625)
	{
		enWave = ENUM_WAVE_1625NM;
		pUser_Settings->iWave[2] = 1;
		if(clearCurve)
		{
    		pUser_Settings->iWave[0] = 0;
    		pUser_Settings->iWave[1] = 0;
    	}
	}

	pUser_Settings->enWaveCurPos = enWave;
	pUser_Settings->sFrontSetting[enWave].enPulseTime = PulseToEnum(pSor->iPulse);
	pUser_Settings->sFrontSetting[pUser_Settings->enWaveCurPos].enAverageTime = AvgTimeToEnum(pSor->iAverageTime);
	pUser_Settings->sFrontSetting[pUser_Settings->enWaveCurPos].enFiberRange = LengthToEnum(pSor->lAcquisitionDist);
	pUser_Settings->sSampleSetting.enSmpResolution = GetSmpMode(pUser_Settings->sFrontSetting[pUser_Settings->enWaveCurPos].enFiberRange, pSor->fSmpRatio);

	/* 曲线信息赋值 */
	//基本参数

	Param.ulDTS = (time_t)atoi(pSor->generalInfo.pComment);
	Param.enWave = (UINT32)enWave;
	Param.enRange = (UINT32)LengthToEnum(pSor->lAcquisitionDist);
	Param.enPluse = (UINT32)PulseToEnum(pSor->iPulse);
	Param.enTime = (UINT32) AvgTimeToEnum(pSor->iAverageTime);
	Param.uiAvgTime = pSor->iAverageTime;
	Param.iFactor = pSor->iScaleFactor;
	Param.fRefractive = pSor->fRefractive;
	Param.fDistance = pSor->lAcquisitionDist;
	Param.fSmpIntval = pSor->pDataSpacing[0] / 10000.0f * LIGHTSPEED_M_S / pSor->fRefractive / 1e+10;//Param.fDistance / pSor->iDataCount;
	Param.fSmpRatio = pSor->fSmpRatio;

	Param.fBackScatter = pSor->fBackScattering;
	Param.fEndThr = pSor->fEndLossThr;
	Param.fLossThr = pSor->fSpliceLossThr;
	Param.fReturnThr = pSor->fReturnLossThr;

	//设置每个点代表的距离到算法
    SetLengthPerPoint(Param.fSmpIntval);
    #ifndef MINI2
    EXTERN_INTERFACE FrontParament;
    FrontParament.wave = enWave;
    FrontParament.range = LengthToEnum(pSor->lAcquisitionDist);
    FrontParament.pulseWidth = PulseToEnum(pSor->iPulse);
	LOG(LOG_INFO, "pSor->iPulse %d, pulseWidth %d\n", pSor->iPulse, FrontParament.pulseWidth);
	FrontParament.iSampleRating = pSor->fSmpRatio;
    FrontParament.fLightSpeed = LIGHTSPEED_M_S;

    FrontParament.fRelay = pSor->fBackScattering + 10 * log10f(pSor->iPulse);
    FrontParament.fLossThreshold = pSor->fSpliceLossThr;
    FrontParament.fBackScattering = pSor->fBackScattering;
    FrontParament.fRefractiveIndex = pSor->fRefractive;
    FrontParament.fReflectThreshold = pSor->fReturnLossThr;
    FrontParament.fFiberEndThreshold = pSor->fEndLossThr;
    FrontParament.fExcessLength = 0;
    //设置外部参数
    SetfrontParament(&FrontParament);
    #endif
	//数据输入
	dIn.uiCnt = pSor->iDataCount;
	dIn.pData = pSor->pData;

	//清除所有波形
	if(clearCurve)
	{
	    ClearAllCurve();
	}

	//添加曲线
	DisplayAddCurve(pDisplay, &Param, &dIn);

	//设置当前曲线有效
	pDisplay->pCurve[pDisplay->iCurCurve]->uiIsVaild = 1;
	//添加事件
	if (pSor->iEventAnalysisFlag)
	{
        EVENTS_TABLE eventTable;
        memset(&eventTable, 0, sizeof(eventTable));
        SOR_EVENTS_TABLE* sorTable = &pSor->eventTable;
        int i;
        eventTable.iEventsNumber = sorTable->iEventsNumber;
        eventTable.iEventsStart = sorTable->iEventsStart;
        eventTable.iLaunchFiberIndex = sorTable->iLaunchFiberIndex;
        eventTable.iRecvFiberIndex = sorTable->iRecvFiberIndex;
        eventTable.iIsThereASpan = sorTable->iIsThereASpan;
        for (i = 0; i < sorTable->iEventsNumber; ++i)
        {
             eventTable.EventsInfo[i].iBegin = sorTable->EventsInfo[i].iBegin;
             eventTable.EventsInfo[i].iEnd = sorTable->EventsInfo[i].iEnd;
             eventTable.EventsInfo[i].iStyle = sorTable->EventsInfo[i].iStyle;
             eventTable.EventsInfo[i].iPulse = sorTable->EventsInfo[i].iPulse;
             eventTable.EventsInfo[i].fLoss = sorTable->EventsInfo[i].fLoss;
             eventTable.EventsInfo[i].fDelta = sorTable->EventsInfo[i].fDelta;
             eventTable.EventsInfo[i].fReflect = sorTable->EventsInfo[i].fReflect;
             eventTable.EventsInfo[i].iContinueLength = sorTable->EventsInfo[i].iContinueLength;
             eventTable.EventsInfo[i].fAttenuation = sorTable->EventsInfo[i].fAttenuation;
             eventTable.EventsInfo[i].fTotalLoss = sorTable->EventsInfo[i].fTotalLoss;
             eventTable.EventsInfo[i].fEventsPosition = CalcRealDist(sorTable->EventsInfo[i].iBegin,Param.fSmpIntval);//sorTable->EventsInfo[i].fEventsPosition;
             if(i != 0)
             {
                eventTable.EventsInfo[i-1].fContinueAttenuation = sorTable->EventsInfo[i-1].fAttenuation *
                    (eventTable.EventsInfo[i].fEventsPosition - eventTable.EventsInfo[i-1].fEventsPosition);
             }
             eventTable.EventsInfo[i].fEventsEndPosition = sorTable->EventsInfo[i].fEventsEndPosition;
             eventTable.EventsInfo[i].fEventReturnLoss = sorTable->EventsInfo[i].fEventReturnLoss;
             eventTable.EventsInfo[i].iWaveSymbol = sorTable->EventsInfo[i].iWaveSymbol;
             eventTable.EventsInfo[i].iSmallReflectFlag = 0;
             eventTable.EventsInfo[i].iSmallLossFlag = 0;

             //保证打开的sor文件事件列表最后一个事件为末端事件，末端事件后面的事件不显示
             if(eventTable.EventsInfo[i].iStyle >= 10 && eventTable.EventsInfo[i].iStyle <= 19)
             {
                eventTable.iEventsNumber = i+1;
                break;
             }
        }

		SetCurveEvents(CURR_CURVE, pDisplay, &eventTable);
	}

	//设置文件名
	char *strFile = strrchr(pSor->cFileName, '/');
	if (NULL == strFile)
	{
		SetCurveFile(CURR_CURVE, pDisplay, pSor->cFileName);
	}
	else
	{
	    strFile++;
		SetCurveFile(CURR_CURVE, pDisplay, strFile);
	}

	//设置读 sor 标志
	SetDisplaySor(pDisplay, 1);
	//算法结构体
	PTASK_SETTINGS pTskSet = &(pOtdrTopSettings->pTask_Context->Task_SetQueue[pDisplay->iCurCurve]);
	int i;
	pTskSet->pAlgorithm_info->iSignalLength = pSor->iDataCount;

	for (i = 0; i < pTskSet->pAlgorithm_info->iSignalLength; ++i)
	{
		pTskSet->pAlgorithm_info->pSignalLog[i] = (float)pSor->pData[i] / (float)pSor->iScaleFactor;
	}
	pTskSet->pAlgorithm_info->SetParameters.fBackScattering = pSor->fBackScattering;
	pTskSet->pAlgorithm_info->SetParameters.fSampleRating = pSor->fSmpRatio;
	pTskSet->pAlgorithm_info->SetParameters.fRefractiveIndex = pSor->fRefractive;
	pTskSet->pAlgorithm_info->SetParameters.fLossThreshold = pSor->fSpliceLossThr;;			//损耗阈值
	pTskSet->pAlgorithm_info->SetParameters.fReflectThreshold = pSor->fReturnLossThr;		//反射率阈值
	pTskSet->pAlgorithm_info->SetParameters.fFiberEndThreshold = pSor->fEndLossThr;			//末端检测阈值
	pTskSet->pAlgorithm_info->SetParameters.fExcessLength = 0;								//余长系数，默认是0
	pTskSet->pAlgorithm_info->SetParameters.fLightSpeed = LIGHTSPEED_M_S;					//光速
	pTskSet->pAlgorithm_info->SetParameters.iwavelength = enWave;							//波长
	pTskSet->pAlgorithm_info->iBlind = GetBlindLength(Param.enPluse, pSor->fSmpRatio);

    SetSavedFlag(pDisplay, 1);
	return 0;
}


/***
  * 功能：
  		保存sor的接口函数
  * 参数：
  		1、int iCurCurve:	第几条曲线，只有0和1
  		2、char *FileName:	保存的SOR文件名字
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
		返回值错误描述:       		返回值
		传入PSOR_DATA指针为空:      -1
		打开文件失败:         		-2
		InitWriteToSor:         	-3
  		WriteMap:					-4
  		WriteGeneral:				-5
  		WriteSupplier:				-6
  		WriteFixed:					-7
  		WriteKeyevent:				-8
  		WriteDatapoint:				-9
  		WriteChecksum:				-10
  		WriteCrc:					-11,-12,-13,-14,-15,-16
  		申请PSOR_DATA内存失败:		-17
  		申请采集数据点的内存失败:	-18

 ***/
int WriteSor(int iCurCurve, char *FileName)
{
	int iRet = 0;
	PSOR_DATA pSor;

	CURVE_PRARM Param;
	DATA_IN dIn;

	pSor = (PSOR_DATA)calloc(1,sizeof(SOR_DATA));
	if (!pSor)
	{
		LOG(LOG_ERROR,"----calloc memory PSOR failed!!!--\n");
		return -17;
	}

	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
	PUSER_SETTINGS pUser_Settings = pOtdrTopSettings->pUser_Setting;

    int arrPulseTime[OPM_PULSE_KIND] = {100, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
	int arrWaveLen[OPM_WAVE_KIND] = {1310, 1550};

	//文件名
	memset(pSor->cFileName, 0, FILE_NAME_MAX);
	strcpy(pSor->cFileName, FileName);

	pSor->iLossMethodMark = pUser_Settings->sCommonSetting.iLossMethodMark;

	//基本参数
	GetCurvePara(iCurCurve, pDisplay, &Param);
	pSor->iWave = arrWaveLen[Param.enWave];
	pSor->iPulse = arrPulseTime[Param.enPluse];
	pSor->iAverageTime = Param.uiAvgTime;
	pSor->lAcquisitionDist = (long)Param.fDistance;
	pSor->fSmpRatio = Param.fSmpRatio;
	pSor->iScaleFactor = Param.iFactor;
	pSor->fRefractive = Param.fRefractive;
	pSor->fBackScattering = Param.fBackScatter;
	pSor->fSpliceLossThr = Param.fLossThr;
	pSor->fReturnLossThr = Param.fReturnThr;
	pSor->fEndLossThr = Param.fEndThr;

	//曲线原始数据
	dIn.pData = (unsigned short *)calloc(sizeof(unsigned short), MAX_DATA);
	if (NULL == dIn.pData)
	{
		free(pSor);
		return -18;
	}

	GetCurveData(iCurCurve, pDisplay, &dIn);
	pSor->iDataCount = dIn.uiCnt;
	pSor->pData = dIn.pData;

	//曲线事件表
	if (CurveIsAnalysised(iCurCurve, pDisplay))
	{
        EVENTS_TABLE eventTable;
        GetCurveEvents(iCurCurve, pDisplay, &eventTable);

        SOR_EVENTS_TABLE* sorTable = &pSor->eventTable;
        int i;
        sorTable->iEventsNumber = eventTable.iEventsNumber;
        sorTable->iEventsStart = eventTable.iEventsStart;
        sorTable->iLaunchFiberIndex = eventTable.iLaunchFiberIndex;
        sorTable->iRecvFiberIndex = eventTable.iRecvFiberIndex;
        sorTable->iIsThereASpan = eventTable.iIsThereASpan;
        for (i = 0; i < sorTable->iEventsNumber; ++i)
        {
             sorTable->EventsInfo[i].iBegin = eventTable.EventsInfo[i].iBegin;
             sorTable->EventsInfo[i].iEnd = eventTable.EventsInfo[i].iEnd;
             sorTable->EventsInfo[i].iStyle = eventTable.EventsInfo[i].iStyle;
             sorTable->EventsInfo[i].iPulse = eventTable.EventsInfo[i].iPulse;
             sorTable->EventsInfo[i].fLoss = Float2Float(LOSS_PRECISION, eventTable.EventsInfo[i].fLoss);
             sorTable->EventsInfo[i].fDelta = eventTable.EventsInfo[i].fDelta;
             sorTable->EventsInfo[i].fReflect = Float2Float(PRECISION_3, eventTable.EventsInfo[i].fReflect);
             sorTable->EventsInfo[i].iContinueLength = eventTable.EventsInfo[i].iContinueLength;
             sorTable->EventsInfo[i].fAttenuation = Float2Float(ATTENUATION_PRECISION, eventTable.EventsInfo[i].fAttenuation);
             sorTable->EventsInfo[i].fContinueAttenuation = eventTable.EventsInfo[i].fContinueAttenuation;
             sorTable->EventsInfo[i].fTotalLoss = eventTable.EventsInfo[i].fTotalLoss;
             sorTable->EventsInfo[i].fEventsPosition = eventTable.EventsInfo[i].fEventsPosition;
             sorTable->EventsInfo[i].fEventsEndPosition = eventTable.EventsInfo[i].fEventsEndPosition;
             sorTable->EventsInfo[i].fEventReturnLoss = eventTable.EventsInfo[i].fEventReturnLoss;
             sorTable->EventsInfo[i].iWaveSymbol = eventTable.EventsInfo[i].iWaveSymbol;
        }
		pSor->iEventAnalysisFlag = 1;
	}

	char dateBuf[64];
	sprintf(dateBuf,"%d",(int)Param.ulDTS);
	strcpy(pSor->generalInfo.pComment, dateBuf);
	//附加信息
	strcpy(pSor->generalInfo.pCompanyName, SUPPLIER_NAME);
	strcpy(pSor->generalInfo.pOperator, COMMENT);
	strcpy(pSor->generalInfo.pCableID, CABLEID);
	strcpy(pSor->generalInfo.pFiberID, FIBERID);
	strcpy(pSor->generalInfo.pCableCode, CABLECODE);
	strcpy(pSor->generalInfo.pStartPoint, STARTPOINT);
	strcpy(pSor->generalInfo.pEndPoint, ENDPOINT);
	pSor->generalInfo.sFiberType = FIBERTYPE;
	pSor->generalInfo.pDataFlag[0] = 'B';
	pSor->generalInfo.pDataFlag[1] = 'C';

	//写sor
	iRet = WriteToSor(pSor);
	if(!iRet)
	{
        SetSavedFlag(pDisplay, 1);
	}

	//释放资源
	free(dIn.pData);
	free(pSor);

	return iRet;
}

//保存sor数据
int SaveSorFile(int iActiveCurve, char * FilePath)
{
	if (access("/mnt/dataDisk", F_OK) != 0)
	{
		return -2;
	}

	if (access(FilePath, F_OK) != 0)
	{
		if (mkdir(FilePath, 0666))
			return -3;
	}

	char buff[F_NAME_MAX];
	char strFile[F_NAME_MAX];
	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
	GetCurveFile(iActiveCurve, pDisplay, strFile);
	sprintf(buff, "%s/%s", FilePath, strFile);

	int iErr = 0;
	if (iErr == 0)
	{
		if (!CanWriteFile(buff))
		{
			iErr = -1;
		}
	}

	if (iErr == 0)
	{
		iErr = WriteSor(iActiveCurve, buff);
	}

	return iErr;
}

/***
  * 功能：
	    从SOR文件读取数据
  * 参数：
  		1、int iCurCurve:	第几条曲线，只有0和1
  		2、char *FileName:	保存的SOR文件名字
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
  		返回值错误描述:       		返回值
		打开文件失败:         		-1
		获取文件信息失败:         	-2
		文件size太小:               -3
		申请每个块的资源失败:       -4
		ReadMapBlock失败:			-5
		ReadGeneral失败:			-6
		ReadSupplier失败:			-7
		ReadFixed失败:				-8
		Event和Dada块都为空:		-9
		曲线位置参数错误:			-10
		曲线名字貌似错误:			-11
		申请PSOR_DATA结构失败:		-12
		申请采集数据点内存失败:		-13

 ***/
int ReadSor(int iCurCurve, char *FileName, int clearCurve)
{
	int iRet = 0;
	PSOR_DATA pSor;

	if(iCurCurve < 0 || iCurCurve > 1)
	{
		LOG(LOG_ERROR,"----iCurCurve ERROR!!! iCurCurve = %d--\n",iCurCurve);
		return -10;
	}

	if(strlen(FileName) < 6)
	{
		LOG(LOG_ERROR,"----FileName ERROR!!! FileName = %s--\n",FileName);
		return -11;
	}

	pSor = (PSOR_DATA)calloc(1,sizeof(SOR_DATA));
	if(!pSor)
	{
		LOG(LOG_ERROR,"----calloc memory PSOR failed!!!--\n");
		return -12;
	}

	pSor->pData = (unsigned short *)calloc(1, sizeof(unsigned short) * MAX_DATA);
	if(!pSor->pData)
	{
		LOG(LOG_ERROR,"----calloc memory pSor->pData failed!!!--\n");
		if(pSor)
		{
			free(pSor);
			pSor = NULL;
		}
		return -13;
	}

	pSor->pDataSpacing = NULL;

	//读到pSor上
	iRet = ReadFromSor(FileName, pSor);

	//调试
	//PrintSorData(pSor);

	if(iRet == 0)
	{
		Sor2Otdr(pSor, clearCurve);
	}

    if(pSor->pDataSpacing)
    {
        free(pSor->pDataSpacing);
        pSor->pDataSpacing = NULL;
    }

	if(pSor->pData)
	{
		free(pSor->pData);
		pSor->pData = NULL;
	}
	if(pSor)
	{
		free(pSor);
		pSor = NULL;
	}

	return iRet;
}

/***
  * 功能：
	    从INNO文件读取数据
  * 参数：
  		1、char *FileName:	需要打开的INNO文件名字(绝对路径)
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
  		返回值错误描述:       		返回值
		打开文件失败:         		-1
		获取文件信息失败:         	-2
		文件size太小:               -3
		申请每个块的资源失败:       -4
		ReadMapBlock失败:			-5
		ReadGeneral失败:			-6
		ReadSupplier失败:			-7
		ReadFixed失败:				-8
		Event和Dada块都为空:		-9
		曲线位置参数错误:			-10
		曲线名字貌似错误:			-11
		申请PSOR_DATA结构失败:		-12
		申请采集数据点内存失败:		-13
 ***/
int ReadInno(char *FileName)
{
    int iErr = 0;
    unsigned int count;
    char *sorTempPath = "/app/sor";     //存储sor文件的临时路径
    char **sorList = NULL;  //从inno文件中解析出来的sor文件列表

    isExist(sorTempPath);
    iErr = InnoLoad(FileName, sorTempPath, &sorList, &count);
    if(!iErr)
    {
        int i;
        ClearAllCurve();
        pOtdrTopSettings->pUser_Setting->iWave[WAVELEN_1310] = 0;
        pOtdrTopSettings->pUser_Setting->iWave[WAVELEN_1550] = 0;
        pOtdrTopSettings->pUser_Setting->iWave[WAVELEN_1625] = 0;
        for(i=0; i<count; i++)
        {
            iErr = ReadSor(i, sorList[i], 0);
            free(sorList[i]);
        }
        free(sorList);
    }
    else if(iErr == InnoSORFormat)
    {
        iErr = ReadSor(0, FileName, 1);
    }

    mysystem("rm /app/sor -rf");
    return iErr;
}
//
SAMPLE_INTERVAL GetSmpResolution(FIBER_RANGE range, float fRatio)
{
	SAMPLE_INTERVAL res;

	if (fabs(fRatio - fSampleRatio[range][0]) < 0.000001) {
		res = ENUM_SMP_NORMAL;
	}
	else {
		res = ENUM_SMP_HIGH_RES;
	}
	return res;
}
//重新对各曲线事件列表做阈值判断
void ReevaluateEvent(void)
{
    PDISPLAY_INFO Display = pOtdrTopSettings->pDisplayInfo;
    int i;

    for (i = 0 ; i < Display->iCurveNum ; i++)
    {
        EVENTS_TABLE *eventsTable = &Display->pCurve[i]->Events;
        IsEventPass(Display->pCurve[i]->pParam.enWave, &Display->pCurve[i]->Events,
            GetSpanORL(&Display->pCurve[i]->Events, Display->pCurve[i]));
        changeIsPassUI(eventsTable->iIsPass, i);
    }
}

//计算跨段长度
float GetSpanLength(const EVENTS_TABLE *eventsTable)
{
    if(eventsTable)
    {
        float spanLength = eventsTable->EventsInfo[eventsTable->iEventsNumber-1].fEventsPosition;
        if(eventsTable->iRecvFiberIndex > 0)
        {
            spanLength = eventsTable->EventsInfo[eventsTable->iRecvFiberIndex].fEventsPosition;
        }

        if(eventsTable->iLaunchFiberIndex > 0)
        {
            spanLength -= eventsTable->EventsInfo[eventsTable->iLaunchFiberIndex].fEventsPosition;
        }

        if(eventsTable->iEventsStart > 0)
        {
            spanLength -= eventsTable->EventsInfo[eventsTable->iEventsStart].fEventsPosition;
        }
        return spanLength;
    }
    return 0;
}

//计算跨段损耗
float GetSpanLoss(const EVENTS_TABLE *eventsTable)
{
    if(eventsTable)
    {
        float spanLoss = eventsTable->EventsInfo[eventsTable->iEventsNumber-1].fTotalLoss;

        if(eventsTable->iRecvFiberIndex > 0)
        {
            spanLoss = eventsTable->EventsInfo[eventsTable->iRecvFiberIndex].fTotalLoss
                + eventsTable->EventsInfo[eventsTable->iRecvFiberIndex].fLoss;
        }

        if(eventsTable->iLaunchFiberIndex > 0)
        {
            spanLoss -= eventsTable->EventsInfo[eventsTable->iLaunchFiberIndex].fTotalLoss;
        }

        if(eventsTable->iEventsStart > 0)
        {
            spanLoss -= eventsTable->EventsInfo[eventsTable->iEventsStart].fTotalLoss;
        }
        return spanLoss;
    }
    return 0;
}


//计算跨段光回损
float GetSpanORL(EVENTS_TABLE *eventsTable, PCURVE_INFO pCurve)
{
    if(pCurve == NULL || eventsTable == NULL)
    {
        return 0.0f;
    }

    if(eventsTable->iEventsNumber <= 1)
    {
        return RETURNLOSS_MAXVALUE;
    }

    MARKER maker = pCurve->mMarker;

    float spanORL = maker.fTotalReutrnLoss;
    int startIndex = eventsTable->EventsInfo[0].iBegin;
    int endIndex = eventsTable->EventsInfo[eventsTable->iEventsNumber - 1].iBegin;

    if(eventsTable->iRecvFiberIndex > 0)
    {
        startIndex = eventsTable->EventsInfo[eventsTable->iRecvFiberIndex].iBegin;
    }

    if(eventsTable->iLaunchFiberIndex > 0)
    {
        endIndex = eventsTable->EventsInfo[eventsTable->iLaunchFiberIndex].iBegin;
    }

    if(eventsTable->iEventsStart > 0)
    {
        startIndex = eventsTable->EventsInfo[eventsTable->iEventsStart].iBegin;
    }

    spanORL = ReturnLoss(pCurve->dIn.pData, startIndex,
                         endIndex,
                         pCurve->pParam.fBackScatter,
                         pCurve->pParam.fSmpRatio);
    return spanORL;
}


//判断事件是否通过
void IsEventPass(OPM_WAVE wave, EVENTS_TABLE *eventsTable, float fTotalReturnLoss)
{
    if(eventsTable == NULL)
    {
        return;
    }

    int i = 0;
    eventsTable->iIsPass = 1;
    eventsTable->LinkIsPass.iIsRegionLossPass = IsRegionLossPass(wave, GetSpanLoss(eventsTable));
    eventsTable->LinkIsPass.iIsRegionLengthPass = IsRegionLengthPass(wave, GetSpanLength(eventsTable));
    eventsTable->LinkIsPass.iIsRegionORLPass = eventsTable->iEventsNumber > 1 ? IsRegionORLPass(wave, fTotalReturnLoss) : 1;

    if (!eventsTable->LinkIsPass.iIsRegionLossPass ||
        !eventsTable->LinkIsPass.iIsRegionLengthPass||
        !eventsTable->LinkIsPass.iIsRegionORLPass)
    {
        eventsTable->iIsPass = 0;
    }

	for (i = 0; i < eventsTable->iEventsNumber; i++)
	{
        EVENTS_INFO event = eventsTable->EventsInfo[i];
        EVENTS_ISPASS *eventIsPass  = &eventsTable->EventsIsPass[i];

	    if((eventsTable->iLaunchFiberIndex > 0 && i < eventsTable->iLaunchFiberIndex)
	        || (eventsTable->iRecvFiberIndex > 0 && i > eventsTable->iRecvFiberIndex)
	        || (eventsTable->iEventsStart > 0 && i < eventsTable->iEventsStart)
	        || (eventsTable->iIsThereASpan == 0 && eventsTable->iLaunchFiberIndex == 0
	            && eventsTable->iRecvFiberIndex == 0 && i != 0))
	    {
	        eventIsPass->iIsEventLossPass = 1;
	        eventIsPass->iIsReflectPass = 1;
	        eventIsPass->iIsAttenuationPass = 1;
            continue;
	    }

        eventIsPass->iIsEventLossPass = IsEventLossPass(wave, event.fLoss, event.iStyle);
        eventIsPass->iIsReflectPass = IsEventReflectPass(wave, event.fReflect, event.iStyle);
        eventIsPass->iIsAttenuationPass = IsAttenuationPass(wave, event.fAttenuation, event.iStyle);
        //第一个事件和最后一个事件损耗不显示，默认通过
        if(i == 0 || (i == eventsTable->iEventsNumber - 1))
        {
            eventIsPass->iIsEventLossPass = 1;
        }
        //接收光纤位置衰减默认通过
        if((eventsTable->iRecvFiberIndex > 0 && i == eventsTable->iRecvFiberIndex)
            || eventsTable->iIsThereASpan == 0)
        {
            eventIsPass->iIsAttenuationPass = 1;
        }

        if(!eventIsPass->iIsEventLossPass ||
           !eventIsPass->iIsReflectPass ||
           !eventIsPass->iIsAttenuationPass)
        {
            eventsTable->iIsPass = 0;
        }
    }
}

extern PUSER_SETTINGS pUser_Settings;
//判断事件损耗是否通过  返回1 通过  0 不通过
int IsEventLossPass(UINT32 wave, float fLoss, int iStyle)
{
    float fThreshold = 0.0f;
    int iEnable = 0;
    int isPass = 1;
    //增益事件（1）和回波事件（3）不做阈值判断
    switch(iStyle)
    {
        case 0:
        case 5:
            fThreshold = pUser_Settings->sOtherSetting.PassThr.SpliceLossThr[wave].fThrValue;
            iEnable = pUser_Settings->sOtherSetting.PassThr.SpliceLossThr[wave].iSlectFlag;
            break;
        case 2:
        case 4:
            fThreshold = pUser_Settings->sOtherSetting.PassThr.ConnectLossThr[wave].fThrValue;
            iEnable = pUser_Settings->sOtherSetting.PassThr.ConnectLossThr[wave].iSlectFlag;
            break;
        default:
            break;
    }

    if(iEnable && fThreshold < fLoss)
    {
        isPass = 0;
    }
    return isPass;
}

//判断事件反射率是否通过  返回1 通过  0 不通过
int IsEventReflectPass(UINT32 wave, float fReflect, int iStyle)
{
    float fThreshold = 0.0f;
    int iEnable = 0;
    int isPass = 1;
    switch(iStyle)
    {
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
        case 14:
            fThreshold = pUser_Settings->sOtherSetting.PassThr.ReturnLossThr[wave].fThrValue;
            iEnable = pUser_Settings->sOtherSetting.PassThr.ReturnLossThr[wave].iSlectFlag;
            break;
        default:
            break;
    }

    if(iEnable && fThreshold < fReflect)
    {
        isPass = 0;
    }

    return isPass;
}

//判断衰减是否通过  返回1 通过  0 不通过
int IsAttenuationPass(UINT32 wave, float fAttenuation, int iStyle)
{
    float fThreshold = 0.0f;
    int iEnable = 0;
    int isPass = 1;
    switch(iStyle)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            fThreshold = pUser_Settings->sOtherSetting.PassThr.AttenuationThr[wave].fThrValue;
            iEnable = pUser_Settings->sOtherSetting.PassThr.AttenuationThr[wave].iSlectFlag;
            break;
        default:
            break;
    }

    if(iEnable && fThreshold < fAttenuation)
    {
        isPass = 0;
    }
    return isPass;
}

//判断跨段损耗是否通过  返回1 通过  0 不通过
int IsRegionLossPass(UINT32 wave, float fRegionLoss)
{
    float fThreshold = pUser_Settings->sOtherSetting.PassThr.RegionLossThr[wave].fThrValue;
    int iEnable = pUser_Settings->sOtherSetting.PassThr.RegionLossThr[wave].iSlectFlag;
    int isPass = 1;

    if(iEnable && fThreshold < fRegionLoss)
    {
        isPass = 0;
    }
    return isPass;
}

//判断跨段长度是否通过  返回1 通过  0 不通过
int IsRegionLengthPass(UINT32 wave, float fRegionLength)
{
    float fThreshold = pUser_Settings->sOtherSetting.PassThr.fSpanLength_m[wave];
    int iEnable = pUser_Settings->sOtherSetting.PassThr.RegionLengthThr[wave].iSlectFlag;
    int isPass = 1;
    //阈值为0时默认不判断
    if(iEnable && (fThreshold != 0) && (fThreshold < fRegionLength))
    {
        isPass = 0;
    }

    return isPass;
}

//判断跨段光回损是否通过  返回1 通过  0 不通过
int IsRegionORLPass(UINT32 wave, float fRegionORL)
{
    float fThreshold = pUser_Settings->sOtherSetting.PassThr.RegionReturnLossThr[wave].fThrValue;
    int iEnable = pUser_Settings->sOtherSetting.PassThr.RegionReturnLossThr[wave].iSlectFlag;
    int isPass = 1;

    if(iEnable && fThreshold > fRegionORL)
    {
        isPass = 0;
    }
    return isPass;
}

#ifndef MINI2
// 获得衰减量
unsigned int SetAttenuation(PTASK_SETTINGS pTaskSet, int taskNum) // 0:平均模式 1:实时模式
{
    unsigned int attenuation = attr[pTaskSet->enPulseTime][taskNum];

    //平均测量模式下
    if (!pTaskSet->iRealTimeFlag)
    {
        if ((pTaskSet->enPulseTime == ENUM_PULSE_20NS) || (pTaskSet->enPulseTime == ENUM_PULSE_10NS))
        {
        	attenuation = attr[pTaskSet->enPulseTime][taskNum + 2];
        }
        else if ((pTaskSet->enPulseTime == ENUM_PULSE_200NS) && (ENUM_FIBER_RANGE_5KM >= pTaskSet->enFiberRange))
        {
            attenuation = attr[pTaskSet->enPulseTime][taskNum + 4];
        }
/*		else if (pTaskSet->enFiberRange == ENUM_FIBER_RANGE_260KM && pTaskSet->enPulseTime == ENUM_PULSE_20US && pTaskSet->enAverageTime == 7) // added by wyl 08/23
		{
			attenuation = attr[pTaskSet->enPulseTime][taskNum];
		}*/
        else
            attenuation = attr[pTaskSet->enPulseTime][taskNum];
    }
    //实时测量模式下
    else
    {
        if ((pTaskSet->enPulseTime < ENUM_PULSE_500NS) && (pTaskSet->enPulseTime >= ENUM_PULSE_50NS))
        {
            attenuation = attr[pTaskSet->enPulseTime][3];
        }
        else
        {
            if (pTaskSet->enPulseTime >= ENUM_PULSE_2US)  // changed by wyl v1.108
                attenuation = attr[pTaskSet->enPulseTime][0];
            else
                attenuation = attr[pTaskSet->enPulseTime][1];
        }
    }

	return attenuation;
}


// 获得累加次数
unsigned int SetFilters(PTASK_SETTINGS pTaskSet, int taskNum)// 0:平均模式 1:实时模式
{
	unsigned int filters = avg_filter[pTaskSet->enFiberRange][taskNum];

	if (!pTaskSet->iRealTimeFlag)
    {
		if((pTaskSet->enFiberRange <= ENUM_FIBER_RANGE_10KM )&&(pTaskSet->Daq_SetQueue[taskNum].fSmpRatio < 16)
			&&(pTaskSet->enPulseTime <= ENUM_PULSE_50NS))
			filters = avg_filter[pTaskSet->enFiberRange][taskNum] + 1;
		else if((pTaskSet->enFiberRange <= ENUM_FIBER_RANGE_10KM )&&(pTaskSet->Daq_SetQueue[taskNum].fSmpRatio > 30))
			filters = avg_filter[pTaskSet->enFiberRange][taskNum] + 0;
		else
			filters = avg_filter[pTaskSet->enFiberRange][taskNum] ;
    }
	//实时测量模式下
    else
    {
		filters = rt_filter[pTaskSet->enFiberRange];
    }

	return filters;
}
#endif

