/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_global.h
* 摘    要：  声明各窗体的公共数据、公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2012-12-31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_GLOBAL_H
#define _WND_GLOBAL_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"


#define MntUsbDirectory "/mnt/usb"
#define SDCardDirectory "/mnt/sdcard/"


#define BmpFileDirectory    SDCardDirectory"bitmap/"
#define FNTFILE_STD     SDCardDirectory"font/unicode16.fnt"

#define WINDOW_WIDTH    640
#define WINDOW_HEIGHT   480

/***************************************
* 定义用于发送窗体切换消息的公共操作函数
***************************************/
//发送消息以便退出当前窗体
int SendMsg_ExitWindow(GUIWINDOW *pWndObj);
//发送消息以便调用新的窗体
int SendMsg_CallWindow(GUIWINDOW *pWndObj);

//发送消息以便允许并启动窗体循环
int SendMsg_EnableLoop(GUIWINDOW *pWndObj);
//发送消息以便禁止并停止窗体循环
int SendMsg_DisableLoop(GUIWINDOW *pWndObj);


/***************************************
* 定义用于完成功能按钮显示的公共操作函数
***************************************/
//显示功能按钮按下
int ShowBtn_FuncDown(GUIPICTURE *pBtnFx, GUILABEL *pLblFx);
//显示功能按钮弹起
int ShowBtn_FuncUp(GUIPICTURE *pBtnFx, GUILABEL *pLblFx);


/***********************************
* 定义用于完成状态显示的公共操作函数
***********************************/
//显示当前日期
int ShowSta_CurrDate(GUILABEL *pLblDate);
//显示当前时间
int ShowSta_CurrTime(GUILABEL *pLblTime);
//显示LCD背光亮度
int ShowSta_LcdLight(GUILABEL *pLblLight);
//显示电源信息
int ShowSta_PowerInfo(GUIPICTURE *pIcoPower);

int UpdateLabelRes(GUICHAR *pTargetStr, GUILABEL *pTargetLbl, GUIPICTURE *pBackGround);


/***
  * 功能：
		自己实现system系统调用
  * 参数：
		1.const char * cmdstring:	 需要执行的命令
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int my_system(const char * cmdstring);

/***
  * 功能：
		初始化消息队列
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int InitMsg(void);
/***
  * 功能：
		向消息队列发送消息
  * 参数：
		1、char *cmdBuf		:	发送的数据
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int SendMsg(char *cmdBuf);
/***
  * 功能：
		从消息队列中获取消息
  * 参数：
		1、struct msg *pMsg:	消息缓冲区
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int RecMsg();
#endif  //_WND_GLOBAL_H

