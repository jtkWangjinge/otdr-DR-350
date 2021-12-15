/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frminputpassward.h
* 摘    要：  声明主窗体FrmInputPassward的窗体处理操作函数
*
* 当前版本：  v1.0
* 作    者：  wjg
* 完成日期：  21020/10/26
*
*******************************************************************************/

#ifndef _WND_FRMINPUTPASSWARD_H
#define _WND_FRMINPUTPASSWARD_H

/**********************************************************************************
**				    	       为使用GUI而需要引用的头文件	            		 **
***********************************************************************************/
#include "guiglobal.h"

typedef enum __FactoryRank
{    
    SALE = 2,
    FACTORY = 6
}FactoryRank;

extern FactoryRank factoryRank;
/**********************************************************************************
**				    	       声明窗体处理相关函数			            		 **
***********************************************************************************/
//窗体InputPassward的初始化函数，建立窗体控件、注册消息处理
int InputPasswardInit(void *pWndObj);

//窗体InputPassward的退出函数，释放所有资源
int InputPasswardExit(void * pWndObj);

//窗体InputPassward的绘制函数，绘制整个窗体
int InputPasswardPaint(void *pWndObj);

//窗体InputPassward的循环函数，进行窗体循环
int InputPaawardLoop(void *pWndObj);

//窗体InputPassward的挂起函数，进行窗体挂起前预处理
int InputPasswardPause(void *pWndObj);

//窗体InputPassward的恢复函数，进行窗体恢复前预处理
int InputPasswardResume(void *pWndObj);

//获得当前的窗体类型
int GetCurrWindowType(void);

#endif  //_WND_FrmMain_H






















































