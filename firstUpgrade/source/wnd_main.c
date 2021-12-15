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

#include "wnd_global.h"
#include "wnd_frmupdate.h"
#include "wnd_signal.h"


int main(int argc, char *argv[])
{
    GUITHREAD thdFb = -1;
    GUITHREAD thdKeypad = -1;
    GUITHREAD thdTouch = -1;
    GUITHREAD thdSignal = -1; //信号线程

    GUIWINDOW *pWnd = NULL;
    int iExit = 0;

    //初始化信号，用于信号处理线程
    InitSignal();
    InitializeGUI();

    ThreadCreate(&thdFb, DefaultFbThread, GetCurrFbmap());
    ThreadCreate(&thdKeypad, DefaultKeypadThread, GetCurrKeypad());
    //ThreadCreate(&thdTouch, DefaultTouchThread, GetCurrTouch());
    //运行信号处理线程
    ThreadCreate(&thdSignal, DefaultSignalThread, NULL);

    pWnd = CreateWindow(0, 0, 640, 480, 
                        FrmUpdateInit, FrmUpdateExit, FrmUpdatePaint,
                        FrmUpdateLoop, NULL);      //default window handle thread
    SendMsg_CallWindow(pWnd);

    while (1)
    {
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        MsecSleep(100);
    }

    ThreadJoin(thdTouch, NULL);
    ThreadJoin(thdKeypad, NULL);
    ThreadJoin(thdFb, NULL);
	
    ReleaseGUI();

    return 0;
}

