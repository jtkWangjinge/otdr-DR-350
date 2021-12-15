/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_main.c
* 摘    要：  完成GUI进程的启动
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*
*******************************************************************************/

#include "wnd_frmlogo.h"

/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"

/*********************************
* 为建立首个窗体而需要引用的头文件
*********************************/
#include "wnd_global.h"
#include "app_global.h"
#include "wnd_signal.h"
#include "app_frmfilebrowse.h"
#include "wnd_frmlogo.h"
#include "input_china_pinyin.h"
#include <sys/wait.h>
#include <sys/types.h>


//与MCU异步通信，当实行硬关机时，驱动中会发送信号给应用层
int iPowerDownFlag = 0;		//0表示开机状态，1表示关机状态
GUITHREAD thdSignal = -1;	//信号线程
static MULTISELECT *pEnvQueue = NULL;
CURR_WINDOW_TYPE enCurWindow = ENUM_NULL_WIN;

int main(int argc, char *argv[])
{
    GUITHREAD thdFb = -1;
    GUITHREAD thdKeypad = -1;
#ifdef TOUCH_SCREEN
	GUITHREAD thdTouch = -1;
#endif

    GUIWINDOW *pWnd = NULL;
    int iExit = 0;
	pid_t pid = -1;
	int iStatus;
	#ifdef DEBUG_MEMORY_DETECTER
    memory_detecter_init();
    #endif
reboot:
	if((pid=fork()) < 0)
	{
		return -1;
	}
	else if(pid == 0)
	{	
#ifdef CORE_DUMP //检测是否出现段错误，切换core文件生成路径到usb中
		//解除core文件大小限制
		mysystem("ulimit -c unlimite");
		chdir("/mnt/usb");
#endif
		//初始化信号，用于信号处理线程
		InitSignal();
		//初始化设备驱动
		InitializeDevice();
		//打开多国语言数据库文件
		OpenLanguageDb(LANGEUAGE_DB_FILE);
		//装载输入法数据库
		OpenInputDb(INPUTCNPATH);	
		//初始化GUI环境，运行调度线程及事件封装线程
		InitializeGUI();
		//初始化消息队列
		InitMsg();
		pEnvQueue = CreateQueue(MULTISELECT_LIMIT);
		SetCurrQueue(pEnvQueue);	
		//运行设备抽象层的线程
		ThreadCreate(&thdFb, NULL, 
					 DefaultFbThread, GetCurrFbmap());
		ThreadCreate(&thdKeypad, NULL, 
					 DefaultKeypadThread, GetCurrKeypad());
#ifdef TOUCH_SCREEN
		ThreadCreate(&thdTouch, NULL, 
					 DefaultTouchThread, GetCurrTouch());
#endif
		//运行信号处理线程
		ThreadCreate(&thdSignal, NULL, 
					 DefaultSignalThread, NULL);
		/* 初始化提示信息框 */
		InitInfoDialog();
	    //第一个窗体线程运行
	    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
	                        FrmLogoInit, FrmLogoExit, 
	                        FrmLogoPaint, FrmLogoLoop, 
	                        FrmLogoPause, FrmLogoResume,
	                        NULL);   //pWnd由调度线程释放
	    SendSysMsg_ThreadCreate(pWnd);
	    //等待结束标志设立
	    while (!iExit)
	    {
	        iExit = GetExitFlag();
	        if (iExit == 1)
	        {
#ifdef TOUCH_SCREEN
				ThreadCancel(thdTouch);     //取消可能阻塞的触摸屏线程
#endif
	            ThreadCancel(thdKeypad);    //取消可能阻塞的键盘线程
	            ThreadCancel(thdFb);        //取消可能阻塞的帧缓冲线程
	            ThreadCancel(thdSignal);    //取消可能阻塞的信号线程
	            break;
	        }

	        MsecSleep(100);
	    }
	    
        MsecSleep(5000);                //等待所有数据保存完毕。

	    //回收设备抽象层的线程
#ifdef TOUCH_SCREEN
		ThreadJoin(thdTouch, NULL);
#endif
	    ThreadJoin(thdKeypad, NULL);
	    ThreadJoin(thdFb, NULL);
	    ThreadJoin(thdSignal, NULL);
	    //释放GUI环境，回收事件封装线程及调度线程
	    ReleaseGUI();
	}
	else
	{
		wait(&iStatus);
		goto reboot;
	}
	
    return 0;
}
