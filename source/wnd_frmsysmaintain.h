/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsysmaintain.h
* 摘    要：  主窗体FrmSysMaintain的窗体处理操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：

*******************************************************************************/

#ifndef _WND_FRMSYSMAINTAIN_H
#define _WND_FRMSYSMAINTAIN_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmupdate的初始化函数，建立窗体控件、注册消息处理
int FrmSysMaintainInit(void *pWndObj);
//窗体frmupdate的退出函数，释放所有资源
int FrmSysMaintainExit(void *pWndObj);
//窗体frmupdate的绘制函数，绘制整个窗体
int FrmSysMaintainPaint(void *pWndObj);
//窗体frmupdate的循环函数，进行窗体循环
int FrmSysMaintainLoop(void *pWndObj);
//窗体frmupdate的挂起函数，进行窗体挂起前预处理
int FrmSysMaintainPause(void *pWndObj);
//窗体frmupdate的恢复函数，进行窗体恢复前预处理
int FrmSysMaintainResume(void *pWndObj);

/***
  * 功能：
        获取是否进行了恢复出厂设置
  * 参数：
		无
  * 返回：
        1、进行了恢复出厂设置
        0、没有进行恢复出厂设置
  * 备注：
***/
unsigned char GetFactoryFlg(void);

#endif
