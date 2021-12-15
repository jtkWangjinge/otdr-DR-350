/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部 
*
* All rights reserved
*
* 文件名称：  guidevice.h
* 摘    要：  声明GUI基本输入/输出设备的用户自定义操作函数，为GUI所使用的这些设
*             备提供设备抽象层。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/
    
#ifndef _GUI_DEVICE_H
#define _GUI_DEVICE_H

/**************************************
* 为定义GUIDEVICE而需要引用的其他头文件
**************************************/
#include "guibase.h"
#include "guifbmap.h"
#include "guikeypad.h"
#include "guitouch.h"


/********************************
* 定义GUI中使用的输入输出设备文件
* 注：以下内容需根据实际情况修改
********************************/
#define DEVFILE_FB      "/dev/fb0"  
#define DEVFILE_KEYPAD  "/dev/event0"   //"/dev/keyb0"
#define DEVFILE_TOUCH   "/dev/event1"
/***********************************************
* 定义GUI中基本输入/输出设备的用户自定义操作函数
* 注：以下函数中的内容需根据实际情况编写
***********************************************/
//用户自定义的帧缓冲处理函数，用于完成自定义类型帧缓冲的处理
int CustomFbmapFunc(GUIFBMAP *pFbmapObj);
//用户自定义的键盘处理函数，用于完成自定义类型键盘的处理
int CustomKeypadFunc(GUIKEYPAD *pKeypadObj);
//用户自定义的触摸屏处理函数，用于完成自定义类型触摸屏的处理
int CustomTouchFunc(GUITOUCH *pTouchObj);
//刷新屏幕处理
void RefreshScreen(char *file, const char *func, int line);
/***
  * 功能：
        获取设备节点的名称字符串
  * 参数：
    1、char* eventName：事件名称，作为传出参数，内存在外部
    2、char *devName：设备名称字符串：如：key、touch，此处
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
void GetDeviceEventName(char* eventName, char *devName);
//获得MT的中心位置
UINT32 GetMtCenterPosition();

#define SyncCurrFbmap() RefreshScreen(__FILE__, __func__, __LINE__)

#endif  //_GUI_DEVICE_H

