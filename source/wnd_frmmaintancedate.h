/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmmaintancedate.h
* 摘    要：  实现主窗体frmmaintancedate的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*******************************************************************************/
#ifndef _WND_FRMMAINTANCE_H
#define _WND_FRMMAINTANCE_H

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmmaintancedate的初始化函数，建立窗体控件、注册消息处理
int FrmMaintanceDateInit(void *pWndObj);
//窗体frmmaintancedate的退出函数，释放所有资源
int FrmMaintanceDatePaint(void *pWndObj);
//窗体frmmaintancedate的绘制函数，绘制整个窗体
int FrmMaintanceDateLoop(void *pWndObj);
//窗体frmmaintancedate的循环函数，进行窗体循环
int FrmMaintanceDatePause(void *pWndObj);
//窗体frmmaintancedate的挂起函数，进行窗体挂起前预处理
int FrmMaintanceDateResume(void *pWndObj);
//窗体frmmaintancedate的恢复函数，进行窗体恢复前预处理
int FrmMaintanceDateExit(void *pWndObj);

#endif	