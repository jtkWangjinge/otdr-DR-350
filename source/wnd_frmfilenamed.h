/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfilenamed.h
* 摘    要：  声明主窗体frmfilenamed的窗体处理线程及相
				关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  	    
* 完成日期：  2015-1-22
*
* 取代版本：	
* 原 作 者：	
* 完成日期：  
**************************************************************/

#ifndef _WND_FRMFILENAMED_H
#define _WND_FRMFILENAMED_H


/**************************************************************
* 			为使用GUI而需要引用的头文件
**************************************************************/
#include "guiglobal.h"

/*
typedef struct  _filenamedparam
{
	//启用自动命名标志位
	int iAutoNamedEnable;
	//光纤
	char FiberMark_File[20];
	char WaveLen_File[20];
	char UserDefined_File[50];
	char Pulse_File[20];
}FileNamedParam;
*/

/**************************************************************
* 				声明窗体处理相关函数
**************************************************************/
//窗体frmfilenamed的初始化函数，建立窗体控件、注册消
//息处理
int FrmFileNamedInit(void *pWndObj);
//窗体frmfilenamed的退出函数，释放所有资源
int FrmFileNamedExit(void *pWndObj);
//窗体frmfilenamed的绘制函数，绘制整个窗体
int FrmFileNamedPaint(void *pWndObj);
//窗体frmfilenamed的循环函数，进行窗体循环
int FrmFileNamedLoop(void *pWndObj);
//窗体frmfilenamed的挂起函数，进行窗体挂起前预处理
int FrmFileNamedPause(void *pWndObj);
//窗体frmfilenamed的恢复函数，进行窗体恢复前预处理
int FrmFileNamedResume(void *pWndObj);


#endif  //_WND_FRMFILENAMED_H

