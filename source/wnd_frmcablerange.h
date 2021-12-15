/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcablerange.h
* 摘    要：  声明主窗体wnd_frmcablerange的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRM_CABLE_RANGE_H_
#define _WND_FRM_CABLE_RANGE_H_

extern char* pStrMenu_unpress[2];
extern char* pStrMenu_press[2];

#define CABLE_NUMBER				8

extern char* pBmpCableTypeA[CABLE_NUMBER];
extern char* pBmpCableTypeB[CABLE_NUMBER];

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmCableRange的初始化函数，建立窗体控件、注册消息处理
int FrmCableRangeInit(void *pWndObj);
//窗体frmCableRange的退出函数，释放所有资源
int FrmCableRangeExit(void *pWndObj);
//窗体frmCableRange的绘制函数，绘制整个窗体
int FrmCableRangePaint(void *pWndObj);
//窗体frmCableRange的循环函数，进行窗体循环
int FrmCableRangeLoop(void *pWndObj);
//窗体frmCableRange的挂起函数，进行窗体挂起前预处理
int FrmCableRangePause(void *pWndObj);
//窗体frmCableRange的恢复函数，进行窗体恢复前预处理
int FrmCableRangeResume(void *pWndObj);

//创建网线测距界面
void CreateCableRangeWindow();

#endif
