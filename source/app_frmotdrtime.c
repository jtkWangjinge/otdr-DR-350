/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmotdrtime.h  
* 摘    要：  定义用于完成OTDR测量计时功能的应用程序实现
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*******************************************************************************/
#include "app_frmotdrtime.h"
#include "guibase.h"
#include "app_curve.h"
#include "app_frmotdr.h"
#include "app_frmotdrmessage.h"

static struct otdr_time
{
    int totalTime;                      //总的时间，单位(s)
    int elapsedTime;                    //已消耗的时间，单位(s)
    int remainingTime;                  //剩余时间， 单位(s) =总时间-已消耗的时间
    int isTimeOut;                      //计时是否超时，超时说明计时已结束，超时=1
    int isRT;                           //是否实时模式
    
    GUIMUTEX mMutex;	                //互斥锁
} otdrTime;

//初始化
void OtdrTimeInit(void)
{
    static int isFirst = 0;

    if(!isFirst)
    {
        isFirst = 1;
        InitMutex(&otdrTime.mMutex, NULL);
    }

    MutexLock(&otdrTime.mMutex);
    otdrTime.totalTime = 0;
    otdrTime.elapsedTime = 0;
    otdrTime.remainingTime = 0;
    otdrTime.isTimeOut = 1;             //默认超时状态
    otdrTime.isRT = 0;
	MutexUnlock(&otdrTime.mMutex);
}

static void sendRemainingTime(int remainingTime)
{
    //显示测量时间
	SendCurveAdditionCmd(EN_ADD_MEATIME, remainingTime);
    //此处利用otdr消息队列将测量剩余发送给窗体线程，目前只有sola窗体处理此消息
    WriteOTDRMsgQueue(ENCODE_OTDRMSG(remainingTime, ENUM_OTDRMSG_MEASTIME));
}

//OTDR测量时间线程
static void *OtdrTimeCnt(void *pThreadArg)
{
	static int iReturn = 0;
    unsigned int iTimeTick = 0;

    MutexLock(&otdrTime.mMutex);
    if(!otdrTime.isRT)
    {
        sendRemainingTime(otdrTime.totalTime);
    }
    MutexUnlock(&otdrTime.mMutex);

	//循环计时(粒度100ms)
	while (0 == OtdrTimeTimeOut())
	{
		MsecSleep(100);
		++iTimeTick;
		if (iTimeTick % 10 == 0)
		{
            MutexLock(&otdrTime.mMutex);
			++otdrTime.elapsedTime;
            if(!otdrTime.isRT)
            {
			    otdrTime.remainingTime = otdrTime.totalTime - otdrTime.elapsedTime;
			
    			if (otdrTime.remainingTime <= 0)
    			{
    				otdrTime.isTimeOut = 1;
    			}
    			else 
    			{
                    sendRemainingTime(otdrTime.remainingTime);
    			}
			}
            MutexUnlock(&otdrTime.mMutex);
		}
	}

    MutexLock(&otdrTime.mMutex);
    if(!otdrTime.isRT)
    {
    	//关闭测量时间的显示
    	SendCurveAdditionCmd(EN_ADD_MEATIME, -1);
        //此处利用otdr消息队列将测量进度发送给窗体线程
        WriteOTDRMsgQueue(ENCODE_OTDRMSG(0, ENUM_OTDRMSG_MEASTIME));
    }
    otdrTime.totalTime = 0;
	otdrTime.elapsedTime = 0;
	otdrTime.remainingTime = 0;
	otdrTime.isRT = 0;
    MutexUnlock(&otdrTime.mMutex);

	ThreadExit(&iReturn);
	return &iReturn;
}


//设置时间
//传入大于0的值为实际采样时间 传入0为实时模式
//成功返回0 失败返回-1
int OtdrTimeSet(int newTime)
{
    int iReturn = 0;
    
    MutexLock(&otdrTime.mMutex);
    if(newTime == 0)
    {
        if(otdrTime.isTimeOut)
        {
            otdrTime.isRT = 1;
            otdrTime.totalTime = 0;
        }
        else
        {
            iReturn = -1;
        }
    }
    else if(newTime > otdrTime.elapsedTime)
    {
        otdrTime.isRT = 0;
        otdrTime.totalTime = newTime;
    }
    else if(otdrTime.isTimeOut == 0)
    {
        otdrTime.isTimeOut = 1;
    }
    MutexUnlock(&otdrTime.mMutex);
    
    return iReturn;
}

//开始计时 成功返回0 失败返回-1
int OtdrTimeStart(void)
{
    int iReturn = 0;
    GUITHREAD th;
    
    MutexLock(&otdrTime.mMutex);
    if(otdrTime.isTimeOut)
    {
        otdrTime.isTimeOut = 0;
        //创建timetick线程，并设置为分离线程(不用父线程回收子线程资源)
		ThreadCreate(&th, NULL, OtdrTimeCnt, NULL);
		pthread_detach(th);
    }
    else
    {
        iReturn = -1;
    }
    MutexUnlock(&otdrTime.mMutex);
    return iReturn;
}

//开始计时 成功返回0 失败返回-1
int OtdrTimeStop(void)
{
    int iReturn = 0;

    MutexLock(&otdrTime.mMutex);
    if(!otdrTime.isTimeOut)
    {
        otdrTime.isTimeOut = 1;
    }
    MutexUnlock(&otdrTime.mMutex);
    
    return iReturn;
}

//超时标志 返回1 超时
int OtdrTimeTimeOut(void)
{
	int iReturn = 0;

	MutexLock(&otdrTime.mMutex);
	iReturn = otdrTime.isTimeOut;
	MutexUnlock(&otdrTime.mMutex);

	return iReturn;
}

//获得已消耗的时间
int OtdrTimeGetElapsedTime()
{
    int iReturn = 0;
    
	MutexLock(&otdrTime.mMutex);
	if(otdrTime.isTimeOut)
	{
	    iReturn = -1;
	}
	else
	{
        iReturn = otdrTime.elapsedTime;
	}
	MutexUnlock(&otdrTime.mMutex);

	return iReturn;
}

//是否实时模式
int OtdrTimeIsRT(void)
{
    return otdrTime.isRT;
}