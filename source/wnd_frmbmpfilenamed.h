/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmbmpfilenamed.h
* 摘    要：  声明主窗体frmbmpfilenamed的窗体处理线程及相
				关操作函数
*
* 当前版本：  v1.0.0
* 作    者： 
* 完成日期：  
*
* 取代版本：	
* 原 作 者：	
* 完成日期：  
**************************************************************/

#ifndef _WND_FRMBMPFILENAMED_H
#define _WND_FRMBMPFILENAMED_H


/**************************************************************
* 			为使用GUI而需要引用的头文件
**************************************************************/
#include "guiglobal.h"

/*
typedef struct  _bmpfilenamedparam
{
	//启用自动命名标志位
	int iAutoNamedEnable;
	//光纤
	char FiberMark_File[20];
	char WaveLen_File[20];
	char UserDefined_File[50];
	char Pulse_File[20];
}bmpfilenamedParam;
*/

/**************************************************************
* 				声明窗体处理相关函数
**************************************************************/
//窗体frmbmpfilenamed的初始化函数，建立窗体控件、注册消
//息处理
int FrmBmpFileNamedInit(void *pWndObj);
//窗体frmbmpfilenamed的退出函数，释放所有资源
int FrmBmpFileNamedExit(void *pWndObj);
//窗体frmbmpfilenamed的绘制函数，绘制整个窗体
int FrmBmpFileNamedPaint(void *pWndObj);
//窗体frmbmpfilenamed的循环函数，进行窗体循环
int FrmBmpFileNamedLoop(void *pWndObj);
//窗体frmbmpfilenamed的挂起函数，进行窗体挂起前预处理
int FrmBmpFileNamedPause(void *pWndObj);
//窗体frmbmpfilenamed的恢复函数，进行窗体恢复前预处理
int FrmBmpFileNamedResume(void *pWndObj);


#endif  //_WND_FRMBMPFILENAMED_H

