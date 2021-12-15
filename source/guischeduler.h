/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guischeduler.h
* 摘    要：  定义GUI的窗体调度机制及相关操作，用于实现多窗体、多线程运行。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_SCHEDULER_H
#define _GUI_SCHEDULER_H


/*****************************************
* 为定义GUISCHEDULER而需要引用的其他头文件
*****************************************/
//#include ""


/**********************************
* 定义GUI中与调度机制相关的操作函数
**********************************/
//初始化GUI进程退出标志
int InitExitFlag(void);
//得到GUI进程退出标志
int GetExitFlag(void);
//设置GUI进程退出标志
void SetExitFlag(int iFlag);

//窗体调度线程入口函数
void* WindowSchedulerThread(void *pThreadArg);
//默认窗体处理线程入口函数
void* DefaultWindowThread(void *pThreadArg);

//初始化窗体线程互斥锁
int InitThreadLock(void);
//锁住当前窗体线程
int LockWindowThread(void);
//解锁当前窗体线程
int UnlockWindowThread(void);


#endif  //_GUI_SCHEDULER_H

