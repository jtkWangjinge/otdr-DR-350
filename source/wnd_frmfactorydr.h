/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactorDR.h
* 摘    要：  
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMFACTOR_DR_H
#define _WND_FRMFACTOR_DR_H

#include "guiglobal.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体FrmFacDRInit的初始化函数，建立窗体控件、注册消息处理
int FrmFacDRInit(void *pWndObj);
//窗体FrmFacDRInit的退出函数，释放所有资源
int FrmFacDRPaint(void *pWndObj);
//窗体FrmFacDRInit的绘制函数，绘制整个窗体
int FrmFacDRLoop(void *pWndObj);
//窗体FrmFacDRInit的循环函数，进行窗体循环
int FrmFacDRPause(void *pWndObj);
//窗体FrmFacDRInit的挂起函数，进行窗体挂起前预处理
int FrmFacDRResume(void *pWndObj);
//窗体frmvls的恢复函数，进行窗体恢复前预处理
int FrmFacDRExit(void *pWndObj);

GUIWINDOW* CreateFactoryDRWindow() ;

#endif	//_WND_FRMFACTOR_DR_H