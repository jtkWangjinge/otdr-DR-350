/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guikeypad.h
* 摘    要：  GUIKEYPAD模块主要完成设备抽象层定义及实现，以便GUI能够适用于不同
*             硬件平台对应用层隔离硬件差异。guikeypad.h定义了GUI的键盘映射类型
*             及声明相关操作，用于实现GUI的图形输出设备抽象。
*
* 当前版本：  v1.0.1 (考虑到跨平台，新增了对于GUI_OS_ENV的预编译检验)
* 作    者：  wjg
* 完成日期：  2012-10-12
*
* 取代版本：  v1.0.0
* 原 作 者：  yexin.zhu
* 完成日期：  2012-8-1
*******************************************************************************/

#ifndef _GUI_KEYPAD_H
#define _GUI_KEYPAD_H


/**************************************
* 为定义GUIKEYPAD而需要引用的其他头文件
**************************************/
#include "guibase.h"


/*************
* 定义键盘类型
*************/
#define GUIKEYPAD_TYP_STD       1   //标准键盘类型
#define GUIKEYPAD_TYP_USER      2   //自定义键盘类型


/******************
* 定义GUI中键盘结构
******************/
typedef struct _keypad
{
    int iDevFd;                 //键盘设备文件描述符
    int iDevType;               //键盘设备类型，1表示标准、2表示自定义

    THREADFUNC fnKeypadThread;  //回调函数，用于创建键盘线程
} GUIKEYPAD;


/**********************************
* 定义GUI中与键盘类型相关的操作函数
**********************************/
//依据指定的信息创建键盘对象
GUIKEYPAD* CreateKeypad(char *strDevName, int iDevType, 
                        THREADFUNC fnKeypadThread);
//删除键盘对象
int DestroyKeypad(GUIKEYPAD **ppKeypadObj);

//得到当前的键盘对象
GUIKEYPAD* GetCurrKeypad(void);
//设置当前的键盘对象
int SetCurrKeypad(GUIKEYPAD *pKeypadObj);


/***********************
* 定义GUI中 默认键盘线程
***********************/
//默认键盘线程入口函数
void* DefaultKeypadThread(void *pThreadArg);

//自定义的键盘处理函数，用于完成键盘线程的处理
int CustomKeypadFunc(GUIKEYPAD *pKeypadObj);


#endif  //_GUI_KEYPAD_H

