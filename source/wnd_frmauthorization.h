/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmauthorization.h
* 摘    要：  实现主窗体wnd_frmauthorization的窗体处理线程及相关操作函数实现
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*******************************************************************************/
#ifndef _WND_FRMAUTHORIZATION_H
#define _WND_FRMAUTHORIZATION_H


/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmauthorization的初始化函数，建立窗体控件、注册消息处理
int FrmAuthorizationInit(void *pWndObj);
//窗体frmauthorization的退出函数，释放所有资源
int FrmAuthorizationExit(void *pWndObj);
//窗体frmauthorization的绘制函数，绘制整个窗体
int FrmAuthorizationPaint(void *pWndObj);
//窗体frmauthorization的循环函数，进行窗体循环
int FrmAuthorizationLoop(void *pWndObj);
//窗体frmauthorization的挂起函数，进行窗体挂起前预处理
int FrmAuthorizationPause(void *pWndObj);
//窗体frmauthorization的恢复函数，进行窗体恢复前预处理
int FrmAuthorizationResume(void *pWndObj);

//按键响应处理
void AuthorizationKeyBackCall(int iOption);
//创建授权管理界面
void CreateAuthorizationWindow();

#endif	