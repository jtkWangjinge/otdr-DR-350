/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guievent.h
* 摘    要：  GUIEVENT属于系统调用模块，此部分定义GUI的事件类型及声明相关操作，
*             用于实现事件的定义和封装。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_EVENT_H
#define _GUI_EVENT_H


/*************************************
* 为定义GUIEVENT而需要引用的其他头文件
*************************************/
#include "guibase.h"


/******************************************
* 定义GUI中事件对象所能包含事件项的最大数量
******************************************/
#define GUIEVENT_ITEM_LIMIT     1000    //单个事件对象最多可包含的事件项


/**************
* 定义GUI中事件类型
**************/
#define GUIEVENT_TYP_IN         1   //输入事件


/******************
* 定义GUI中事件代码
******************/
#define GUIEVENT_KNB_CLOCK          1   //事件，旋钮顺时针旋转
#define GUIEVENT_KNB_ANTI           2   //事件，旋钮逆时针旋转
#define GUIEVENT_KEY_DOWN           3   //事件，按键按下
#define GUIEVENT_KEY_UP             4   //事件，按键松开
#define GUIEVENT_ABS_DOWN           5   //事件，点击触摸屏
#define GUIEVENT_ABS_UP             6   //事件，离开触摸屏
#define GUIEVENT_MOV_CURSOR         7   //事件，移动触屏光标
#define GUIEVENT_MT			        8   //事件，多点触摸
#define GUIEVENT_KEY_PRESS          9   //事件，长按按键
#define GUIEVENT_KEY_CANCEL_PRESS   10  //事件，取消长按按键


/****************
* 定义GUI中事件项
****************/
typedef struct _event_item
{
    unsigned int uiEventType;   //事件类型，1输入事件
    unsigned int uiEventCode;   //事件编码，KEY_DOWN，KEY_UP…
    unsigned int uiEventValue;  //事件数值，按键编码或坐标数值(X|Y：X高16位，Y低16位)
} GUIEVENT_ITEM;


/******************
* 定义GUI中事件结构
******************/
typedef struct _event
{
    GUILIST *pEventQueue;       //输入事件队列，输入抽象层利用其保存标准输入事件

    THREADFUNC fnEventThread;   //线程函数，用于创建事件封装线程
    GUIMUTEX Mutex;             //互斥锁，用于控制对输入事件队列的访问
} GUIEVENT;


/**********************************
* 定义GUI中与事件类型相关的操作函数
**********************************/
//依据指定的信息创建事件对象
GUIEVENT* CreateEvent(int iQueueLimit, THREADFUNC fnEventThread);
//删除事件对象
int DestroyEvent(GUIEVENT **ppEventObj);

//得到当前的事件对象
GUIEVENT* GetCurrEvent(void);
//设置当前的事件对象
int SetCurrEvent(GUIEVENT *pEventObj);

//读出指定事件对象中的事件项
GUIEVENT_ITEM* ReadEventQueue(GUIEVENT *pEventObj);
//写入事件项到指定的事件对象
int WriteEventQueue(GUIEVENT_ITEM *pEventItem, GUIEVENT *pEventObj);
//清空事件队列
int ClearEventQueue(GUIEVENT *pEventObj);


/***********************
* 定义GUI中的默认事件封装线程
***********************/
//事件封装线程入口函数
void* EventPacketThread(void *pThreadArg);

//事件封装函数，用于完成事件封装线程的处理
int EventPacketFunc(GUIEVENT *pEventObj);

//从GUI对象队列中匹配事件对象，在事件封装中用于确定消息接收对象
void* MatchEventObject(GUIEVENT_ITEM *pEventItem);


#endif  //_GUI_EVENT_H

