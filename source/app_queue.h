/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_queue.h 
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

#ifndef _APP_QUEUE_H_
#define _APP_QUEUE_H_

#include "guiglobal.h"
#include "guibase.h"

//消息类型
typedef enum _msg_type
{
	EN_CURSOR_CTL 	    = 1,
	EN_LINE_CTL   	    = 2,
	EN_MAKER_CTL	    = 3,
	EN_CURVE_ADD	    = 4,
	EN_EVENT_MARKER_CTL = 5,
} CMD_TYPE;

//消息对象
typedef struct _draw_msg
{
	CMD_TYPE 	enType;
	void*		pContent;
} QUEUE_MSG;

//循环队列实现
typedef struct _cirqueue                 
{
	QUEUE_MSG 	**ppMsg;
	INT32  		iFront;
	INT32 		iRear;
	UINT32 		uiSize;

	GUIMUTEX 	mMutex;	
} CIR_QUEUE;



//初始化循环队列
CIR_QUEUE* InitCirQueue(UINT32 uiSize);

//循环队列判空
int CirQueueIsEmpty(CIR_QUEUE *pQueue);

//循环队列判满
int CirQueueIsFull(CIR_QUEUE *pQueue);

//读取循环队列
int ReadCirQueue(CIR_QUEUE *pQueue, QUEUE_MSG *pDrawMsg);

//写循环队列
int WriteCirQueue(CIR_QUEUE *pQueue, QUEUE_MSG *pDrawMsg);

//清空循环队列
int ClearCirQueue(CIR_QUEUE *pQueue);



//初始化画线线程的循环队列
int InitCurveQueue(UINT32 uiSize);

//读取画线线程循环队列
int ReadCurveQueue(QUEUE_MSG *pDrawMsg);

//写画线线程循环队列
int WriteCurveQueue(QUEUE_MSG *pDrawMsg);

//清除画线命令队列
int ClearCurveQueue(void);

//写画线线程命令
int WriteCurveCmd(CMD_TYPE enType, void *pContent);

#endif //_APP_QUEUE_H_
