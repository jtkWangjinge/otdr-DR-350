/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolafilenamed.h
* 摘    要：  声明主窗体frmsolafilenamed的窗体处理线程及相
				关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  	    wjg
* 完成日期：  2018-3-30
*
* 取代版本：	
* 原 作 者：	
* 完成日期：  
**************************************************************/

#ifndef _WND_FRMSOLAFILENAMED_H
#define _WND_FRMSOLAFILENAMED_H

/**************************************************************
* 				声明窗体处理相关函数
**************************************************************/
int FrmSolaFileNamedInit(void *pWndObj);
//窗体frmfilenamed的退出函数，释放所有资源
int FrmSolaFileNamedExit(void *pWndObj);
//窗体frmfilenamed的绘制函数，绘制整个窗体
int FrmSolaFileNamedPaint(void *pWndObj);
//窗体frmfilenamed的循环函数，进行窗体循环
int FrmSolaFileNamedLoop(void *pWndObj);
//窗体frmfilenamed的挂起函数，进行窗体挂起前预处理
int FrmSolaFileNamedPause(void *pWndObj);
//窗体frmfilenamed的恢复函数，进行窗体恢复前预处理
int FrmSolaFileNamedResume(void *pWndObj);


#endif  //_WND_FRMFILENAMED_H

