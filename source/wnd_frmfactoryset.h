/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfactoryset.h
* 摘    要：  实现主窗体frmfac的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020/20/28
*  
*******************************************************************************/

#ifndef _WND_FRMFACTORSET_H
#define _WND_FRMFACTORSET_H

// #include "guiglobal.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmvls的初始化函数，建立窗体控件、注册消息处理
int FrmFactorySetInit(void *pWndObj);
//窗体frmvls的退出函数，释放所有资源
int FrmFactorySetPaint(void *pWndObj);
//窗体frmvls的绘制函数，绘制整个窗体
int FrmFactorySetLoop(void *pWndObj);
//窗体frmvls的循环函数，进行窗体循环
int FrmFactorySetPause(void *pWndObj);
//窗体frmvls的挂起函数，进行窗体挂起前预处理
int FrmFactorySetResume(void *pWndObj);
//窗体frmvls的恢复函数，进行窗体恢复前预处理
int FrmFactorySetExit(void *pWndObj);

//按键响应处理
void FactorySetKeyBackCall(int iOption);

#endif	

