/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmotdrmessage.h
* 摘    要：  GUIMESSAGE属于系统调用模块，此部分定义GUI的消息类型及声明相关操
*             作，用于实现消息的调度模型。
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _APP_FRMOTDRMESSAGE_H
#define _APP_FRMOTDRMESSAGE_H

#include "guiglobal.h"

/**************************************************
* 定义appotdr中消息对象所能包含消息项最大数量
**************************************************/
#define OTDRMESSAGE_ITEM_LIMIT       100			//单个消息对象最多可包含的消息项

typedef struct _otdr_msg_queue
{
	int queuesize;
	int head, tail;
	int *q;
	
    GUIMUTEX Mutex;         //互斥锁，控制同步标志、消息队列及注册信息的访问
}OTDRMSGQUEUE, *POTDRMSGQUEUE;

OTDRMSGQUEUE *CreateOTDRMsgQueue(int iQueueLimit);

//返回0为非空，返回非0为空
int IsEmptyOTDRMsgQueue(void);

//返回非0为满
int IsFULLOTDRMsgQueue(void);

int ClearOTDRMsgQueue(void);

void WriteOTDRMsgQueue(int key);

void ReadOTDRMsgQueue(int *key);

void SetCurrOTDRMsgQueue(OTDRMSGQUEUE *q);

OTDRMSGQUEUE * GetCurrOTDRMsgQueue(void);


#endif
