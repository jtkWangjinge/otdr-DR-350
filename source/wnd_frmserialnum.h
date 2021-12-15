/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmserialnum.h
* 摘    要：  实现主窗体wnd_frmserialnum的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRM_SERIAL_NUM_H_
#define _WND_FRM_SERIAL_NUM_H_

extern char* pStrButton_unpress[3];
extern char* pStrButton_press[3];

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmSerialnum的初始化函数，建立窗体控件、注册消息处理
int FrmSerialnumInit(void *pWndObj);
//窗体frmSerialnum的退出函数，释放所有资源
int FrmSerialnumExit(void *pWndObj);
//窗体frmSerialnum的绘制函数，绘制整个窗体
int FrmSerialnumPaint(void *pWndObj);
//窗体frmSerialnum的循环函数，进行窗体循环
int FrmSerialnumLoop(void *pWndObj);
//窗体frmSerialnum的挂起函数，进行窗体挂起前预处理
int FrmSerialnumPause(void *pWndObj);
//窗体frmSerialnum的恢复函数，进行窗体恢复前预处理
int FrmSerialnumResume(void *pWndObj);

//创建序列号界面
void CreateSerialNumWindow();

#endif
