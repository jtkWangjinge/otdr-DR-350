/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_queue.c  
* 摘    要：  实现循环队列
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


#include "app_queue.h"


//当前画线队列
static CIR_QUEUE *pCurveQueue = NULL;


//初始化循环队列
CIR_QUEUE* InitCirQueue(UINT32 uiSize)
{
	CIR_QUEUE *pQueue = (CIR_QUEUE *)malloc(sizeof(CIR_QUEUE));
	if (NULL != pQueue)
	{
		pQueue->iFront = 0;
		pQueue->iRear = 0;
		pQueue->uiSize = uiSize + 1;
		pQueue->ppMsg = (QUEUE_MSG**)malloc(sizeof(int) * (uiSize + 1));
		if (NULL == pQueue->ppMsg)
		{
			free(pQueue);
			return NULL;
		}
		InitMutex(&(pQueue->mMutex), NULL);
	}

	return pQueue;
}

//循环队列判空
int CirQueueIsEmpty(CIR_QUEUE *pQueue)
{
	return pQueue->iFront == pQueue->iRear;
}

//循环队列判满
int CirQueueIsFull(CIR_QUEUE *pQueue)
{
	return ((pQueue->iRear + 1) % pQueue->uiSize) == pQueue->iFront;
}

//读取循环队列
int ReadCirQueue(CIR_QUEUE *pQueue, QUEUE_MSG *pQueueMsg)
{
	int iErr = 0;
	QUEUE_MSG *pRead;

	MutexLock(&pQueue->mMutex);
	if (CirQueueIsEmpty(pQueue))
	{
		iErr = -2;
	}
	else
	{
		pRead = pQueue->ppMsg[pQueue->iFront];
		pQueue->ppMsg[pQueue->iFront] = NULL;			
		pQueue->iFront = (pQueue->iFront + 1) % pQueue->uiSize;			
		memcpy(pQueueMsg, pRead, sizeof(QUEUE_MSG));	
		free(pRead);			
	}
	MutexUnlock(&pQueue->mMutex);

	return iErr;
}

//写循环队列
int WriteCirQueue(CIR_QUEUE *pQueue, QUEUE_MSG *pQueueMsg)
{
	int iErr = 0;
	
	MutexLock(&pQueue->mMutex);
	if (CirQueueIsFull(pQueue))
	{
		iErr = -1;
	}
	else
	{
		pQueue->ppMsg[pQueue->iRear] = pQueueMsg;	
		pQueue->iRear = (pQueue->iRear + 1) % pQueue->uiSize;
	}
	MutexUnlock(&pQueue->mMutex);

	return iErr;
}

//清空循环队列
int ClearCirQueue(CIR_QUEUE *pQueue)
{
	int iErr = 0;
	
	QUEUE_MSG *pRead;
	
	MutexLock(&pQueue->mMutex);
	while (!CirQueueIsEmpty(pQueue))
	{
		pRead = pQueue->ppMsg[pQueue->iFront];
		pQueue->ppMsg[pQueue->iFront] = NULL;		
		pQueue->iFront = (pQueue->iFront + 1) % pQueue->uiSize;

		free(pRead->pContent);
		free(pRead);
	}
	MutexUnlock(&pQueue->mMutex);

	return iErr;
}



//初始化画线线程的循环队列
int InitCurveQueue(UINT32 uiSize)
{
	int iErr = 0;
	
	pCurveQueue = InitCirQueue(uiSize);
	if (NULL == pCurveQueue)
	{
		iErr = -1;
	}

	return iErr;
}

//读取画线线程循环队列
int ReadCurveQueue(QUEUE_MSG *pDrawMsg)
{
	return ReadCirQueue(pCurveQueue, pDrawMsg);
}

//写画线线程循环队列
int WriteCurveQueue(QUEUE_MSG *pDrawMsg)
{
	return WriteCirQueue(pCurveQueue, pDrawMsg);
}

//清除画线命令队列
int ClearCurveQueue(void)
{
	return ClearCirQueue(pCurveQueue);
}

//写画线线程命令
int WriteCurveCmd(CMD_TYPE enType, void *pContent)
{
	QUEUE_MSG *pDrawMsg;

	if (NULL == pContent)
	{
		return -1;
	}
	
	pDrawMsg = (QUEUE_MSG *)malloc(sizeof(QUEUE_MSG));
	if (NULL == pDrawMsg)
	{
		return -2;
	}
	pDrawMsg->enType = enType;
	pDrawMsg->pContent = pContent;

	if (WriteCurveQueue(pDrawMsg))
	{
		free(pDrawMsg);
		return -3;
	}
	
	return 0;
}

