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
* 当前版本：  v1.0.1 (考虑到跨平台，新增了对于GUI_OS_ENV的预编译检验)
* 作    者：  wjg
* 完成日期：  2012-10-12
*
* 取代版本：  v1.0.0
* 原 作 者：  yexin.zhu
* 完成日期：  2012-8-2
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
#define GUITOUCH_TYP_STD        1   //标准触摸屏类型
#define GUITOUCH_TYP_USER       2   //自定义触摸屏类型


/********************
* 定义GUI中触摸屏结构
********************/
typedef struct _touch
{
    int iDevFd;                 //触摸屏设备文件描述符
    int iDevType;               //触摸屏设备类型，1表示标准、2表示自定义

    THREADFUNC fnTouchThread;   //回调函数，用于创建触摸屏线程
} GUITOUCH;


/************************************
* 定义GUI中与触摸屏类型相关的操作函数
************************************/
//依据指定的信息创建触摸屏对象
GUITOUCH* CreateTouch(char *strDevName, int iDevType, 
                      THREADFUNC fnTouchThread);
//删除触摸屏对象
int DestroyTouch(GUITOUCH **ppTouchObj);

//得到当前的触摸屏对象
GUITOUCH* GetCurrTouch(void);
//设置当前的触摸屏对象
int SetCurrTouch(GUITOUCH *pTouchObj);


/***********************
定义GUI中 默认触摸屏线程
***********************/
//默认触摸屏线程入口函数
void* DefaultTouchThread(void *pThreadArg);

//自定义的触摸屏处理函数，用于完成触摸屏线程的处理
int CustomTouchFunc(GUITOUCH *pTouchObj);


#endif  //_GUI_TOUCH_H

