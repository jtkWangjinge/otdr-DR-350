/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmfactoryfirmware.h
* 摘    要：  声明烧录工厂配置文件功能相关函数
*            
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2018-01-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMFACTORYFIRMWARE_H
#define _WND_FRMFACTORYFIRMWARE_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "app_factoryfirmware.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmfactoryfirmware的初始化函数，建立窗体控件、注册消息处理
int FrmFactoryFirmwareInit(void *pWndObj);
//窗体frmfactoryfirmware的退出函数，释放所有资源
int FrmFactoryFirmwareExit(void *pWndObj);
//窗体frmfactoryfirmware的绘制函数，绘制整个窗体
int FrmFactoryFirmwarePaint(void *pWndObj);
//窗体frmfactoryfirmware的循环函数，进行窗体循环
int FrmFactoryFirmwareLoop(void *pWndObj);
//窗体frmfactoryfirmware的挂起函数，进行窗体挂起前预处理
int FrmFactoryFirmwarePause(void *pWndObj);
//窗体frmfactoryfirmware的恢复函数，进行窗体恢复前预处理
int FrmFactoryFirmwareResume(void *pWndObj);


int FactoryFirmwareViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where);

#endif

