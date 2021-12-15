/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_signal.c
* 摘    要：  信号接收
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015-1-7
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_signal.h"
#include <signal.h>
#include "wnd_frmlogo.h"
#include "app_frmotdrmessage.h"
#include "app_frmotdr.h"
#include "wnd_global.h"
OTDRSIG stOtdrSig;

extern volatile int iSpeakerOnFlag;

int InitSignal(void)
{
	int iRet = 0;
    sigset_t bset, oset;
	
	sigemptyset(&bset);
	sigaddset(&bset, SIGIO);			//添加关机信号到信号集
	//sigaddset(&bset, _SIGWIFI);			//添加信号到信号集
	sigaddset(&bset, _SIGPOWEROFF);		//添加信号到信号集
	sigaddset(&bset, _SIGSTART);		//添加信号到信号集
	sigaddset(&bset, _SIGSHOWING);
	sigaddset(&bset, _SIGNODATA);
	sigaddset(&bset, _SIGSTOP);
	sigaddset(&bset, _SIGSPEAKER);

	if (pthread_sigmask(SIG_BLOCK, &bset, &oset) != 0)
	{
		iRet = -1;
		LOG(LOG_ERROR, "!! Set pthread mask failed\n");
	}

	return iRet;
}

void InitMCUCommSig(void)
{
	//设备描述符
	DEVFD *glbDevFd = NULL;
	int iPowerFd = -1;
	int f_flag;
	
	glbDevFd = GetGlb_DevFd();
	iPowerFd = glbDevFd->iPowerDev;
	
	//与MCU异步通信
	fcntl(iPowerFd, F_SETOWN, getpid());
	f_flag = fcntl(iPowerFd, F_GETFL);
	fcntl(iPowerFd, F_SETFL, f_flag | FASYNC);
}

/***
  * 功能：
        发送信号
  * 参数：
        1.GUITHREAD pThread:       		  指向创建的新线程的线程ID
	    2.int sig: 	  					  信号
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		 2015-04-22
***/
int ThreadKill(GUITHREAD pThread, int sig)
{
    //错误标志、返回值定义
    int iReturn = 0;
	
#if (GUI_OS_ENV == LINUX_OS)
	  iReturn = pthread_kill(pThread, sig);
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif
	
	  return iReturn;
}


void* DefaultSignalThread(void *pThreadArg)
{
    int iExit = 0;
	int err, signo;
	CODER_LOG(CoderGu, "sigmgr_thread Enter\n");

    sigset_t bset;
	sigemptyset(&bset);
	sigaddset(&bset, SIGIO);
	//sigaddset(&bset, _SIGWIFI);			//添加信号到信号集
	sigaddset(&bset, _SIGPOWEROFF);
	sigaddset(&bset, _SIGSTART);
	sigaddset(&bset, _SIGSHOWING);
	sigaddset(&bset, _SIGNODATA);
	sigaddset(&bset, _SIGSTOP);
	sigaddset(&bset, _SIGSPEAKER);
	
	InitMutex(&(stOtdrSig.mMutex), NULL);
	InitStructOtdrSig();
	
	InitMCUCommSig();

    while (!iExit)
    {		
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        ThreadTestcancel();
		err = sigwait(&bset, &signo);
        ThreadTestcancel();
		
		switch(signo)
		{
			case SIGIO:
				LOG(LOG_ERROR, "sigwait SIGIO\n");
				//TurnOffDEV();
				break;
			case _SIGWIFI:
				LOG(LOG_ERROR, "sigwait _SIGWIFI\n");
				break;
			case _SIGPOWEROFF:
				LOG(LOG_ERROR, "sigwait _SIGPOWEROFF\n");
				TurnOffDEV();
				break;
			case _SIGNODATA://生成数据无效消息
				break;
			case _SIGSTART:
				break;
			case _SIGSHOWING:
				break;
			case _SIGSTOP:
				break;
			case _SIGSPEAKER:
				iSpeakerOnFlag = 1;
				break;
			default:
				break;
		}
		MsecSleep(100);
    }	
	return NULL;
}

void InitStructOtdrSig(void)
{
	MutexLock(&(stOtdrSig.mMutex));
	stOtdrSig.iOtdrSigStart = 0;
	stOtdrSig.iOtdrSigStop = 0;
	MutexUnlock(&(stOtdrSig.mMutex));
}

