/*******************************************************************************
* Copyright(c)2018，一诺仪器(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactorylaserset.h
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  v1.0.0
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#ifndef _WND_FRMFACTORLASERSET_H
#define _WND_FRMFACTORLASERSET_H

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmvls的初始化函数，建立窗体控件、注册消息处理
int FrmFactoryLaserSetInit(void *pWndObj);
//窗体frmvls的退出函数，释放所有资源
int FrmFactoryLaserSetPaint(void *pWndObj);
//窗体frmvls的绘制函数，绘制整个窗体
int FrmFactoryLaserSetLoop(void *pWndObj);
//窗体frmvls的循环函数，进行窗体循环
int FrmFactoryLaserSetPause(void *pWndObj);
//窗体frmvls的挂起函数，进行窗体挂起前预处理
int FrmFactoryLaserSetResume(void *pWndObj);
//窗体frmvls的恢复函数，进行窗体恢复前预处理
int FrmFactoryLaserSetExit(void *pWndObj);

#endif	



