/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmlightsource.h
* 摘    要：  声明初始化窗体wnd_frmlightsource的窗体处理线程及相关操作函数
*
* 当前版本： 
* 作    者：  wjg
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMLIGHTSOURCE_H
#define _WND_FRMLIGHTSOURCE_H

//窗体FrmLightSource的初始化函数，建立窗体控件、注册消息处理
int FrmLightSourceInit(void *pWndObj);
//窗体FrmLightSource的退出函数，释放所有资源
int FrmLightSourceExit(void *pWndObj);
//窗体FrmLightSource的绘制函数，绘制整个窗体
int FrmLightSourcePaint(void *pWndObj);
//窗体FrmLightSource的循环函数，进行窗体循环
int FrmLightSourceLoop(void *pWndObj);
//窗体FrmLightSource的挂起函数，进行窗体挂起前预处理
int FrmLightSourcePause(void *pWndObj);
//窗体FrmLightSource的恢复函数，进行窗体恢复前预处理
int FrmLightSourceResume(void *pWndObj);


#endif  //_WND_FRMLIGHTSOURCE_H