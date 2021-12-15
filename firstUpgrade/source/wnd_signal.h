/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_signal.h
* 摘    要：  分配栈空间，存储上一次销毁的窗口
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015-1-15
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_SIGNAL_H
#define _WND_SIGNAL_H

#include "guiglobal.h"

#define _SIGPOWEROFF	34
#define _SIGWIFI		35
#define _SIGNODATA		36
#define _SIGSTART		37
#define _SIGSHOWING		38
#define _SIGSTOP		39
#define _SIGSPEAKER		10

void* DefaultSignalThread(void *pThreadArg);

int InitSignal(void);
int ThreadKill(GUITHREAD pThread, int sig);

#endif

