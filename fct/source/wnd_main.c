/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmain.c
* 摘    要：  实现主窗体frmmain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
*******************************************************************************/

#include "wnd_frmmain.h"
#include "common_opm.h"
#include "wnd_global.h"

int iPowerFd = -1;
void poweroff(int arg)
{
	int iErr = 0;
	iErr = ioctl(iPowerFd, 1);
	if (iErr < 0)
	{
		iErr = -2;
		perror("ioctl() p89lpc933_0");
	}
	SetExitFlag(1);
}


int main(int argc, char *argv[])
{
    GUITHREAD thdFb = -1;
    GUITHREAD thdKeypad = -1;
    // GUITHREAD thdTouch = -1;

    GUIWINDOW *pWnd = NULL;
	int f_flag;

	
    int iExit = 0;
    int iRet = 0;
    
    //Initialize OPM and serial
    iRet = OPMTestInit();
    if (iRet)
    {
        printf("OPMTestInit failed\n");
    }
    else
    {
        printf("OPMTestInit ok\n");
    }
    InitializeGUI();

    ThreadCreate(&thdFb, DefaultFbThread, GetCurrFbmap());
    ThreadCreate(&thdKeypad, DefaultKeypadThread, GetCurrKeypad());
    // ThreadCreate(&thdTouch, DefaultTouchThread, GetCurrTouch());

    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                        FrmMainInit, FrmMainExit, FrmMainPaint,
                        FrmMainLoop, NULL); //default window handle thread
    SendMsg_CallWindow(pWnd);		
    
	iPowerFd = open("/dev/p89lpc933_0", O_RDWR);
    if(iPowerFd < 0)
    {
        printf("open p89lpc933_0 failed\n");
    }
	
	signal(SIGIO, poweroff);
	fcntl(iPowerFd, F_SETOWN, getpid());
	f_flag = fcntl(iPowerFd, F_GETFL);
	fcntl(iPowerFd, F_SETFL, f_flag | FASYNC);
	
    while (1)
    {
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        MsecSleep(100);
    }

    // ThreadJoin(thdTouch, NULL);
    ThreadJoin(thdKeypad, NULL);
    ThreadJoin(thdFb, NULL);
	
    ReleaseGUI();

    return 0;
}

