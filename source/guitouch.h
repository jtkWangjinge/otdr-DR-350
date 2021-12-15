/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guitouch.h
* 摘    要：  GUITOUCH模块主要完成设备抽象层定义及实现，以便GUI能够适用于不同硬
*             件平台对应用层隔离硬件差异。guitouch.h定义了GUI的触摸屏映射类型及
*             声明相关操作，用于实现GUI的触摸屏输入设备抽象。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_TOUCH_H
#define _GUI_TOUCH_H


/*************************************
* 为定义GUITOUCH而需要引用的其他头文件
*************************************/
#include "guibase.h"


/********************
* 定义GUI中触摸屏类型
********************/
#define GUITOUCH_TYP_STD        1   //标准类型触摸屏(PC鼠标)
#define GUITOUCH_TYP_CUST       2   //自定义类型触摸屏


/********************
* 定义GUI中触摸屏结构
********************/
typedef struct _touch
{
    int iDevFd;                 //触摸屏设备文件描述符
    int iDevType;               //触摸屏设备类型，1表示标准、2表示自定义

    unsigned int uiHorRes;      //触摸屏水平分辨率，0~0xFFFF
    unsigned int uiVerRes;      //触摸屏垂直分辨率，0~0xFFFF
    unsigned int uiCursX;       //触摸屏光标当前横坐标值，0~0xFFFF
    unsigned int uiCursY;       //触摸屏光标当前纵坐标值，0~0xFFFF

    THREADFUNC fnTouchThread;   //回调函数，用于创建触摸屏线程
    GUIMUTEX Mutex;             //互斥锁，用于控制触摸屏线程对触摸屏设备的访问
} GUITOUCH;


/************************************
* 定义GUI中与触摸屏类型相关的操作函数
************************************/
//依据指定的信息创建触摸屏对象
GUITOUCH* CreateTouch(char *strDevName, int iDevType, 
                      unsigned int uiHorRes, unsigned int uiVerRes,
                      THREADFUNC fnTouchThread);
//删除触摸屏对象
int DestroyTouch(GUITOUCH **ppTouchObj);

//得到当前的触摸屏对象
GUITOUCH* GetCurrTouch(void);
//设置当前的触摸屏对象
int SetCurrTouch(GUITOUCH *pTouchObj);

//设置触摸屏的光标位置
int SetTouchCursor(unsigned int uiCursX, unsigned int uiCursY,
                   GUITOUCH *pTouchObj);


/***********************
定义GUI中默认触摸屏线程
***********************/
//默认触摸屏线程入口函数
void* DefaultTouchThread(void *pThreadArg);

//标准类型触摸屏(PC鼠标)的处理函数，用于完成触摸屏线程的处理
int StandardTouchProcess(GUITOUCH *pTouchObj);
//自定义类型触摸屏的处理函数，用于完成触摸屏线程的处理
int CustomTouchProcess(GUITOUCH *pTouchObj);


#endif  //_GUI_TOUCH_H

