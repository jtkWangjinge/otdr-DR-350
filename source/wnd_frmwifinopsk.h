/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmwifinopsk.h
* 摘    要：  显示wifi的连接窗口
*             
*
* 当前版本：  v1.0.0
* 作    者：  xiazhizhu
* 完成日期：  2016-4-7
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMWIFINOPSK_H
#define _WND_FRMWIFINOPSK_H
#include "appwifi.h"

//窗体WIfiNoPsk初始化函数，建立窗体控件、注册消息处理
int WIfiNoPskInit(void *pWndObj);
//窗体WIfiNoPsk退出函数，释放所有资源
int WIfiNoPskExit(void *pWndObj);
//窗体WIfiNoPsk绘制函数，绘制整个窗体
int WIfiNoPskPaint(void *pWndObj);
//窗体WIfiNoPsk循环函数，进行窗体循环
int WIfiNoPskLoop(void *pWndObj);
//窗体WIfiNoPsk的挂起函数，进行窗体挂起前预处理
int WIfiNoPskPause(void *pWndObj);
//窗体WIfiNoPsk的恢复函数，进行窗体恢复前预处理
int WIfiNoPskResume(void *pWndObj);


/*
获取IP后的回调函数

参数: 是否获得IP地址
*/
int WiFiGetIPCallBack_nopsk(char GetIPflag);

/*
连接热点后的回调函数

参数: 是否连接成功(仅连接还未分配IP)
*/
int WiFiConnectCallBack_nopsk(char Connectflag);

int DisplayConnect(void *pWndObj, NETWORK_INFO_SINGLE WifiInfo);


#endif //_WND_FRMWIFINOPSK_H


