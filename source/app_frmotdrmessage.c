/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmotdrmessage.c
* 摘    要：  GUIMESSAGE属于系统调用模块，guimessage.c实现了GUI的消息类型的相关
*             操作，用于实现消息的调度模型。
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*******************************************************************************/

#include "app_frmotdrmessage.h"

#include "app_global.h"
#include "wnd_global.h"

/************************
* appotdr中的当前事件对象
*************************/
static OTDRMSGQUEUE *pCurrOTDRMessage = NULL;

/***
  * 功能：
        根据指定的信息直接创建消息对象
  * 参数：
        1.int iQueueLimit:      需要创建的消息对象的消息队列的最大长度
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
OTDRMSGQUEUE *CreateOTDRMsgQueue(int iQueueLimit)
{
	int iErr = 0;
	OTDRMSGQUEUE *pOtdrQueue = NULL;

	if(iErr == 0)
	{
		pOtdrQueue = (OTDRMSGQUEUE *)malloc(sizeof(OTDRMSGQUEUE));
		if(NULL == pOtdrQueue)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		pOtdrQueue->queuesize = iQueueLimit;
		pOtdrQueue->head = 0;
		pOtdrQueue->tail = 0;
		pOtdrQueue->q = (int *)malloc(sizeof(int) * pOtdrQueue->queuesize);
		if(NULL == pOtdrQueue->q)
		{
			iErr = -2;
		}
	}
	
    if (iErr == 0)
    {
        //初始化互斥锁
        if (InitMutex(&(pOtdrQueue->Mutex), NULL))
        {
            iErr = -3;
        }
    }
	
    //错误处理
    switch (iErr)
    {
    case -3:
		GuiMemFree(pOtdrQueue->q);
    case -2:
        GuiMemFree(pOtdrQueue);
    case -1:
    default:
        break;
    }

	return pOtdrQueue;
}

//返回0为非空，返回非0为空
int IsEmptyOTDRMsgQueue(void)
{
	return ( pCurrOTDRMessage->tail == pCurrOTDRMessage->head );
}

//返回非0为满
int IsFULLOTDRMsgQueue(void)
{
	return ( ((pCurrOTDRMessage->tail+1) % pCurrOTDRMessage->queuesize) == pCurrOTDRMessage->head );
}

//清空队列
int ClearOTDRMsgQueue(void)
{
	pCurrOTDRMessage->head = pCurrOTDRMessage->tail = 0;
	return 0;
}

//内部加锁  2015.11.03
void WriteOTDRMsgQueue(int key)
{
	MutexLock(&(pCurrOTDRMessage->Mutex));
	if (!IsFULLOTDRMsgQueue())
	{	
		pCurrOTDRMessage->q[pCurrOTDRMessage->tail] = key;
		pCurrOTDRMessage->tail = (pCurrOTDRMessage->tail+1) % pCurrOTDRMessage->queuesize;;
	}
	MutexUnlock(&(pCurrOTDRMessage->Mutex));
}

void ReadOTDRMsgQueue(int *key)
{
	MutexLock(&(pCurrOTDRMessage->Mutex));
	if(!IsEmptyOTDRMsgQueue())
	{
		*key = pCurrOTDRMessage->q[pCurrOTDRMessage->head];
		pCurrOTDRMessage->head = (pCurrOTDRMessage->head + 1) % pCurrOTDRMessage->queuesize;
	}
	else
	{
		*key = 0;
	}
	MutexUnlock(&(pCurrOTDRMessage->Mutex));
}
//end

void SetCurrOTDRMsgQueue(OTDRMSGQUEUE *q)
{
	pCurrOTDRMessage = q;
}

OTDRMSGQUEUE * GetCurrOTDRMsgQueue(void)
{
	return pCurrOTDRMessage;
}

