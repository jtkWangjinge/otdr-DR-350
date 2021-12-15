/*******************************************************************************
* Copyright(c)2012，大豪信息技术(威海)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmskin.h
* 摘    要：  提供皮肤设置功能
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014-12-12
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _WND_FRMSKINSET_H
#define _WND_FRMSKINSET_H


/*******************************************************************************
**									宏定义 				 					  **
*******************************************************************************/	
#define SKIN_NUM	2
#define SKIN_A	0
#define SKIN_B 	1

/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"

/*********************
* 声明窗体处理相关函数
*********************/
//窗体frmskinset的初始化函数，建立窗体控件、注册消息处理
int FrmSkinSetInit(void *pWndObj);
//窗体frmskinset的退出函数，释放所有资源
int FrmSkinSetExit(void *pWndObj);
//窗体frmskinset的绘制函数，绘制整个窗体
int FrmSkinSetPaint(void *pWndObj);
//窗体frmskinset的循环函数，进行窗体循环
int FrmSkinSetLoop(void *pWndObj);
//窗体frmskinset的挂起函数，进行窗体挂起前预处理
int FrmSkinSetPause(void *pWndObj);
//窗体frmskinset的恢复函数，进行窗体恢复前预处理
int FrmSkinSetResume(void *pWndObj);

/***
  * 功能：
        设置皮肤
  * 参数：
        1、unsigned char ucSkin:	皮肤索引
  * 返回：
        无
  * 备注：
***/
void SetSkin(unsigned char ucSkin);


/***
  * 功能：
        获取当前设置的皮肤
  * 参数：
        无
  * 返回：
        获取当前设置的皮肤
  * 备注：
***/
unsigned char GetSkin(void);


#endif 
