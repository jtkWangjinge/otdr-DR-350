/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guimessage.h
* 摘    要：  GUIMESSAGE属于系统调用模块，此部分定义GUI的消息类型及声明相关操
*             作，用于实现消息的调度模型。
*
* 当前版本：  v1.0.0
* 作    者：  yexin.zhu
* 完成日期：  2012-8-21
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _GUI_MESSAGE_H
#define _GUI_MESSAGE_H


/***************************************
* 为定义GUIMESSAGE而需要引用的其他头文件
***************************************/
#include "guibase.h"


/**************************************************
* 定义GUI中消息对象所能包含消息项、注册项的最大数量
**************************************************/
#define GUIMESSAGE_ITEM_LIMIT       1000    //单个消息对象最多可包含的消息项
#define GUIMESSAGE_REG_LIMIT        1000    //单个消息对象最多可包含的消息注册项


/******************
* 定义GUI中消息类型
******************/
#define GUIMESSAGE_TYP_SYS          1   //系统消息，系统消息只能由调度线程处理
#define GUIMESSAGE_TYP_WND          2   //窗体消息，窗体消息可由各窗体线程处理


/******************
* 定义GUI中消息编码
******************/
#define GUIMESSAGE_SYS_EXIT         1   //系统消息，系统退出
#define GUIMESSAGE_THD_CREAT        2   //系统消息，建立窗体线程
#define GUIMESSAGE_THD_DEST         3   //系统消息，销毁窗体线程
#define GUIMESSAGE_WND_INIT         4   //窗体消息，窗体初始化
#define GUIMESSAGE_WND_EXIT         5   //窗体消息，窗体退出
#define GUIMESSAGE_WND_PAINT        6   //窗体消息，窗体绘制
#define GUIMESSAGE_WND_LOOP         7   //窗体消息，窗体循环
#define GUIMESSAGE_WND_PAUSE        8   //窗体消息，窗体挂起
#define GUIMESSAGE_WND_RESUME       9   //窗体消息，窗体恢复
#define GUIMESSAGE_LOP_ENABLE       10  //窗体消息，允许并启动窗体循环
#define GUIMESSAGE_LOP_DISABLE      11  //窗体消息，禁止窗体循环
#define GUIMESSAGE_ERR_PROC         12  //窗体消息，执行错误处理
#define GUIMESSAGE_KEY_DOWN         13  //窗体消息，按键按下
#define GUIMESSAGE_KEY_UP           14  //窗体消息，按键松开
#define GUIMESSAGE_TCH_DOWN         15  //窗体消息，点击触摸屏
#define GUIMESSAGE_TCH_UP           16  //窗体消息，离开触摸屏
#define GUIMESSAGE_KNB_CLOCK        17  //窗体消息，旋钮顺时针旋转
#define GUIMESSAGE_KNB_ANTI         18  //窗体消息，旋钮逆时针旋转



/********************************
* 定义GUI中消息处理函数的调用接口
********************************/
typedef int (*MSGFUNC)(void *pInArg, int iInLen, void *pOutArg, int iOutLen);


/****************
* 定义GUI中消息项
****************/
typedef struct _message_item
{
    int iMsgType;               //消息类型，1系统消息，2窗体消息，<0自定义类型
    int iMsgCode;               //消息编码，WND_EXIT，KEY_UP…，<0自定义编码

    unsigned int uiMsgValue;    //消息数值，键值…，作为消息处理函数的参数
    int iValueLength;           //消息数值的长度，当消息数值为某个指针时有效

    void *pRecvObj;             //消息接收对象，用于匹配消息处理函数
} GUIMESSAGE_ITEM;


/********************
* 定义GUI中消息注册项
********************/
typedef struct _message_reg
{
    int iMsgCode;           //消息编码，用于匹配消息处理函数
    void *pRecvObj;         //消息接收对象，用于匹配消息处理函数

    MSGFUNC fnMsgFunc;      //用于处理消息的函数
    void *pOutArg;          //消息处理函数的传出参数
    int iOutLength;         //传出参数的长度
} GUIMESSAGE_REG;


/******************
* 定义GUI中消息结构
******************/
typedef struct _message
{
    GUILIST *pMsgQueue;     //消息队列，用于保存GUI消息
    GUILIST *pMsgReg;       //注册信息，用于保存消息注册项

    GUIMUTEX Mutex;         //互斥锁，控制同步标志、消息队列及注册信息的访问
} GUIMESSAGE;


/**********************************
* 定义GUI中与消息类型相关的操作函数
**********************************/
//根据指定的信息直接创建消息对象
GUIMESSAGE* CreateMessage(int iQueueLimit, int iRegLimit);
//删除消息对象
int DestroyMessage(GUIMESSAGE **ppMsgObj);

//得到当前消息对象
GUIMESSAGE* GetCurrMessage(void);
//设置为当前消息对象
int SetCurrMessage(GUIMESSAGE *pMsgObj);

//从指定消息对象中接收特定类型的消息项
GUIMESSAGE_ITEM* RecvSpecialMessage(int iMsgType, GUIMESSAGE *pMsgObj);
//读出指定消息对象中队列头部的消息项
GUIMESSAGE_ITEM* ReadMessageQueue(GUIMESSAGE *pMsgObj);
//写入消息项到指定消息对象的队列尾部
int WriteMessageQueue(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj);
//清空消息队列
int ClearMessageQueue(GUIMESSAGE *pMsgObj);

//注册消息注册项到指定消息对象
int LoginMessageReg(int iMsgCode, void *pRecvObj, 
                    MSGFUNC fnMsgFunc, void *pOutArg, int iOutLength, 
                    GUIMESSAGE *pMsgObj);
//从指定消息对象注销消息注册项
int LogoutMessageReg(int iMsgCode, void *pRecvObj, GUIMESSAGE *pMsgObj);
//清空消息注册信息
int ClearMessageReg(GUIMESSAGE *pMsgObj);

//在指定消息对象中匹配与消息相对应的消息注册项
GUIMESSAGE_REG* MatchMessageReg(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj);
//在指定消息对象中匹配消息并执行对应的消息处理函数
int ExecMessageProc(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj);


#endif  //_GUI_MESSAGE_H

