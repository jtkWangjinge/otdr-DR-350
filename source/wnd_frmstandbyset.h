/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmstandbyset.h
* 摘    要：  主窗体FrmStandbyset的窗体处理操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


#ifndef _WND_FRMSTANDBYSET_H
#define _WND_FRMSTANDBYSET_H

/*******************************************************************************
***                     为使用GUI而需要引用的头文件                          ***
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"

/*******************************************************************************
***                        声明窗体处理相关函数                              ***
*******************************************************************************/

//窗体FrmStandbyset的初始化函数，建立窗体控件、注册消息处理
int FrmStandbysetInit(void *pWndObj);
//窗体FrmStandbyset的退出函数，释放所有资源
int FrmStandbysetExit(void *pWndObj);
//窗体FrmStandbyset的绘制函数，绘制整个窗体
int FrmStandbysetPaint(void *pWndObj);
//窗体FrmStandbyset的循环函数，进行窗体循环
int FrmStandbysetLoop(void *pWndObj);
//窗体FrmStandbyset的挂起函数，进行窗体挂起前预处理
int FrmStandbysetPause(void *pWndObj);
//窗体FrmStandbyset的恢复函数，进行窗体恢复前预处理
int FrmStandbysetResume(void *pWndObj);

#endif  //_WND_FRMSTANDBYSET_H

