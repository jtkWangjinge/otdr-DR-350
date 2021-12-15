/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guimessage.c
* 摘    要：  GUIMESSAGE属于系统调用模块，guimessage.c实现了GUI的消息类型的相关
*             操作，用于实现消息的调度模型。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#include "guimessage.h"
//#include "app_sigcombine.h"
#include "wnd_signal.h"
#include "wnd_frmmenubak.h"

/***************************************
* 为实现GUIMESSAGE而需要引用的其他头文件
***************************************/
//#include ""


/********************
* GUI中的当前消息对象
********************/
static GUIMESSAGE *pCurrMessage = NULL;


/***
  * 功能：
        根据指定的信息直接创建消息对象
  * 参数：
        1.int iQueueLimit:      需要创建的消息对象的消息队列的最大长度
        2.int iRegLimit:        需要创建的消息对象的消息注册队列的最大长度
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIMESSAGE* CreateMessage(int iQueueLimit, int iRegLimit)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIMESSAGE *pMsgObj = NULL;

    if (!iErr)
    {
        //尝试分配内存
        pMsgObj = (GUIMESSAGE *) malloc(sizeof(GUIMESSAGE));
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //创建消息对象的消息队列
        pMsgObj->pMsgQueue = CreateList(iQueueLimit);
        if (NULL == pMsgObj->pMsgQueue)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //创建消息对象的消息注册队列
        pMsgObj->pMsgReg = CreateList(iRegLimit);
        if (NULL == pMsgObj->pMsgReg)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //初始化互斥锁
        if (InitMutex(&(pMsgObj->Mutex), NULL))
        {
            iErr = -4;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -4:
        DestroyList(pMsgObj->pMsgReg);
        //no break
    case -3:
        DestroyList(pMsgObj->pMsgQueue);
        //no break
    case -2:
        free(pMsgObj);
        //no break
    case -1:
        pMsgObj = NULL;
        //no break
    default:
        break;
    }

    return pMsgObj;
}


/***
  * 功能：
        删除消息对象
  * 参数：
        1.GUIMESSAGE **ppMsgObj:    指针的指针，指向需要销毁的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyMessage(GUIMESSAGE **ppMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppMsgObj是否为有效指针
        if (NULL == ppMsgObj)
        {
            iErr = -1; 
        }
    }

    if (!iErr)
    {
        //判断ppMsgObj所指向的是否为有效指针
        if (NULL == *ppMsgObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //销毁消息对象的消息队列
        if (DestroyList((*ppMsgObj)->pMsgQueue))
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //销毁消息对象的消息注册队列
        if (DestroyList((*ppMsgObj)->pMsgReg))
        {
            iErr = -4;
        }
    }

    if (!iErr)
    {
        //销毁消息对象，并将指针置空
        free(*ppMsgObj);
        *ppMsgObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        得到当前消息对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIMESSAGE* GetCurrMessage(void)
{
    return pCurrMessage;
}


/***
  * 功能：
        设置为当前消息对象
  * 参数：
        1.GUIMESSAGE *pEventObj:    被设置为当前消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrMessage(GUIMESSAGE *pMsgObj)
{
    //判断pEventObj是否有效
    if (NULL == pMsgObj)
    {
        return -1;
    }

    pCurrMessage = pMsgObj;

    return 0;
}


/***
  * 功能：
        从指定消息对象中接收一个特定类型的消息项
  * 参数：
        1.int iMsgType:         需要接收的消息项类型
        2.GUIMESSAGE *pMsgObj:  指定的消息对象
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
GUIMESSAGE_ITEM* RecvSpecialMessage(int iMsgType, GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIMESSAGE_ITEM *pReturn = NULL;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (!iErr)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断消息队列是否有效指针和消息队列是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历消息队列找到类型匹配的消息项
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pReturn = (GUIMESSAGE_ITEM *) (pPkt->pObj);

            if (pReturn->iMsgType == iMsgType)
            {
                //找到类型匹配的消息项后从消息队列中删除
                if (ListDel(pItem, pMsgObj->pMsgQueue))
                {
                    iErr = -3;
                }
                break;
            }
        }

        //如果没有找到类型匹配的消息项
        if (-1 == iPos)
        {
            iErr = -4;
        }
    }

    if (!iErr)
    {
        //释放资源
        free(pPkt);    
    }

    //错误处理
    switch (iErr)
    {
    case -4:
    case -3:
        pReturn = NULL;
        //no break
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return pReturn;
}


/***
  * 功能：
        从指定消息对象中清除所有特定类型的消息项
  * 参数：
        1.int iMsgType:         需要清除的消息项类型
        2.GUIMESSAGE *pMsgObj:  指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int ClearSpecialMessage(int iMsgType, GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_ITEM *pMsg = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));
    if (!iErr)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断消息队列是否有效指针和消息队列是否为空
        if ((NULL == pMsgObj->pMsgQueue) ||
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }
    
    if (!iErr)
    {
        //遍历消息队列找到类型匹配的消息项
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pMsg = (GUIMESSAGE_ITEM *) (pPkt->pObj);
            if (pMsg->iMsgType == iMsgType)
            {
                //删除操作将修改列表项结构，因此需先移动pItem
                pItem = pItem->pPrev;
                //找到类型匹配的消息项后从消息队列中删除
                if (ListDel(pItem->pNext, pMsgObj->pMsgQueue))
                {
                    iErr = -3;
                    break;
                }

                //释放资源
                free(pMsg);
                free(pPkt);

                //判断是否已遍历完毕
                if (-1 == iPos)
                {
                    break;
                }
            }
        }
    }
    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return iErr;
}


/***
  * 功能：
        读出指定消息对象中队列头部的消息项
  * 参数：
        1.GUIMESSAGE *pMsgObj:  拥有需读事件队列的事件对象
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
GUIMESSAGE_ITEM* ReadMessageQueue(GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIMESSAGE_ITEM *pReturn = NULL;
    //临时变量定义
    GUIOBJ *pPkt = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));
	
    if (!iErr)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断消息队列是否为有效指针和消息队列是否为空
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //通过pMsgObj的GUILIST_ITEM成员获得pPkt的地址
        pPkt = container_of(pMsgObj->pMsgQueue->pHead, GUIOBJ, List);
        pReturn = (GUIMESSAGE_ITEM *) (pPkt->pObj);
        //从消息队列中删除头列表项
        if (ListDel(pMsgObj->pMsgQueue->pHead, pMsgObj->pMsgQueue))
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //释放资源
        free(pPkt);
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        pReturn = NULL;
        //no break
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return pReturn;
}


/***
  * 功能：
        写入消息项到指定消息对象的队列尾部
  * 参数：
        1.GUIMESSAGE_ITEM *pMsgItem:    需要被写入消息队列中的消息项
        2.GUIMESSAGE *pMsgObj:          指定的消息对象
        3.position:						插入的位置 0:尾部 1:头部
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int WriteMessageQueue(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj, int position)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));
    if (!iErr)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pMsgItem || NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断pMsgQueue是否为有效指针和消息队列是否已满
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListFull(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //申请内存
        pPkt = (GUIOBJ *) malloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //添加消息项到消息队列
        pPkt->pObj = pMsgItem;
        if(position == 1)
        {
	        if (ListAddToHead(&(pPkt->List), pMsgObj->pMsgQueue))
	        {
	            iErr = -4;
	        }
        }
        else if(position == 0)
        {
			if (ListAdd(&(pPkt->List), pMsgObj->pMsgQueue))
	        {
	            iErr = -4;
	        }
        }
    }

    //错误处理
    switch (iErr)
    {
    case -4:
        free(pPkt);
        //no break
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return iErr;
}


/***
  * 功能：
        清空消息队列
  * 参数：
        1.GUIMESSAGE *pMsgObj:      需要被清空消息队列的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int ClearMessageQueue(GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (!iErr)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断事件队列pMsgQueue是否为有效指针
        if (NULL == pMsgObj->pMsgQueue)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历删除消息队列项
        while (pMsgObj->pMsgQueue->iCount)
        {
            pPkt= container_of(pMsgObj->pMsgQueue->pHead, GUIOBJ, List);
            pItem = (GUIMESSAGE_ITEM *) (pPkt->pObj);

            if (ListDel(pMsgObj->pMsgQueue->pHead, pMsgObj->pMsgQueue))
            {
                iErr = -3;
                break;
            }

            //释放资源
            free(pItem);
            free(pPkt);
        }
    }
    
    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return iErr;
}


/***
  * 功能：
        注册消息注册项到指定消息对象
  * 参数：
        1.int iMsgCode:         消息注册项的消息编码
        2.void *pRecvObj:       消息注册项的消息接收对象
        3.MSGFUNC fnMsgFunc:    消息注册项的处理消息的函数
        4.void *pOutArg:        消息注册项的消息处理函数的传出参数
        5.int iOutLength:       消息注册项的传出参数的长度
        6.GUIMESSAGE *pMsgObj:  拥有消息注册队列的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        1.函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
        2.在注册消息注册项时，以消息编码和消息接收对象区分各消息注册项。因此在
        注册时，会首先对消息注册项进行遍历，若发现已有相同的消息注册项（消息编
        码及消息接收对象的值相同），该函数将报错
***/
int LoginMessageReg(int iMsgCode, void *pRecvObj, 
                    MSGFUNC fnMsgFunc, void *pOutArg, int iOutLength, 
                    GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_REG *pReg = NULL;
    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (!iErr)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断pMsgReg是否为有效指针和消息队列是否已满
        if ((NULL == pMsgObj->pMsgReg) || 
            ListFull(pMsgObj->pMsgReg))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历列表
        list_next_each(pItem, pMsgObj->pMsgReg, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pReg = (GUIMESSAGE_REG *) (pPkt->pObj);

            //判断是否已有相同的消息注册项
            if ((pReg->iMsgCode == iMsgCode) &&
                (pReg->pRecvObj == pRecvObj))
            {
                iErr = -3;
                break;
            }
        }
    }

    if (!iErr)
    {
        //为pReg申请内存
        pReg = (GUIMESSAGE_REG *) malloc(sizeof(GUIMESSAGE_REG));
        if (NULL == pReg)
        {
            iErr = -4;
        }
    }

    if (!iErr)
    {
        //为pMsg申请内存
        pPkt = (GUIOBJ *) malloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -5;
        }
    }

    if (!iErr)
    {
        //设置消息注册项的成员属性
        pReg->iMsgCode = iMsgCode;
        pReg->pRecvObj = pRecvObj;
        pReg->fnMsgFunc = fnMsgFunc;
        pReg->pOutArg = pOutArg;
        pReg->iOutLength = iOutLength;
        //添加消息注册项到消息注册队列
        pPkt->pObj = pReg;
        if (ListAdd(&(pPkt->List), pMsgObj->pMsgReg))
        {
            iErr = -6;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -6:
        free(pPkt);
        //no break
    case -5:
        free(pReg);
        //no break
    case -4:
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return iErr;
}


/***
  * 功能：
        从指定消息对象注销消息注册项
  * 参数：
        1.int iMsgCode:         需要注销的消息注册项的消息编码
        2.void *pRecvObj:       需要注销的消息注册项的消息接收对象
        3.GUIMESSAGE *pMsgObj:  拥有需注销消息注册队列的事件对象
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int LogoutMessageReg(int iMsgCode, void *pRecvObj, GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_REG *pReg = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (!iErr)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断消息注册队列是否有有效指针和消息注册队列是否为空
        if ((NULL == pMsgObj->pMsgReg) || 
            ListEmpty(pMsgObj->pMsgReg))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历列表
        list_next_each(pItem, pMsgObj->pMsgReg, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pReg = (GUIMESSAGE_REG *) (pPkt->pObj);

            //从事件队列中删除消息注册项
            if ((pReg->iMsgCode == iMsgCode) && 
                (pReg->pRecvObj == pRecvObj))
            {
                if (ListDel(pItem, pMsgObj->pMsgReg))
                {
                    iErr = -3;
                }
                break;
            }
        }

        //没有找到匹配的消息注册项
        if (-1 == iPos)
        {
            iErr = -4;
        }
    }

    //释放资源
    if (!iErr)
    {
        free(pReg);
        free(pPkt);
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return iErr;
}


/***
  * 功能：
        清空消息注册信息
  * 参数：
        1.GUIMESSAGE *pMsgObj:      需要被清空消息注册队列的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int ClearMessageReg(GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_REG *pItem = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (!iErr)
    {
        //判断pMsgObj是否为有效指针
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断消息队列pMsgReg是否为有效指针
        if (NULL == pMsgObj->pMsgReg)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历删除消息注册队列项
        while (pMsgObj->pMsgReg->iCount)
        {
            pPkt= container_of(pMsgObj->pMsgReg->pHead, GUIOBJ, List);
            pItem = (GUIMESSAGE_REG *) (pPkt->pObj);

            if (ListDel(pMsgObj->pMsgReg->pHead, pMsgObj->pMsgReg))
            {
                iErr = -3;
                break;
            }

            //释放资源
            free(pItem);
            free(pPkt);
        }
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return iErr;
}


/***
  * 功能：
       在指定消息对象中匹配与消息相对应的消息注册项
  * 参数：
        1.GUIMESSAGE_ITEM *pMsgItem:    需匹配的消息队列项
        2.GUIMESSAGE *pMsgObj:          被匹配的消息对象
  * 返回：
        成功返回有效指针，失败返回空指针
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
GUIMESSAGE_REG* MatchMessageReg(GUIMESSAGE_ITEM *pMsgItem, 
                                GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIMESSAGE_REG *pReturn = NULL;
    //临时变量定义
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;

    //加锁
    MutexLock(&(pMsgObj->Mutex));

    if (!iErr)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pMsgItem || NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断pMsgReg是否为有效指针或是否为空
        if ((NULL == pMsgObj->pMsgReg) || 
            ListEmpty(pMsgObj->pMsgReg))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //遍历消息注册队列
        list_next_each(pItem, pMsgObj->pMsgReg, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pReturn = (GUIMESSAGE_REG *) (pPkt->pObj);

            if ((pReturn->iMsgCode == pMsgItem->iMsgCode) &&
                (pReturn->pRecvObj == pMsgItem->pRecvObj))
            {
                break;
            }
        }

        //没有找到匹配的消息注册项
        if (-1 == iPos)
        {
            iErr = -3;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        pReturn = NULL;
        //no break
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    MutexUnlock(&(pMsgObj->Mutex));

    return pReturn;
}


/***
  * 功能：
       在指定消息对象中匹配消息并执行对应的消息处理函数
  * 参数：
        1.GUIMESSAGE_ITEM *pMsgItem:    用来匹配消息注册项的消息项
        2.GUIMESSAGE *pMsgObj:          指定的消息对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        不能在持有锁（消息对象中的互斥锁）的前提下调用，否则会导致死锁
***/
int ExecMessageProc(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIMESSAGE_REG *pReg = NULL;

    if (!iErr)
    {
        //判断pMsgItem和pMsgObj是否为有效指针
        if (NULL == pMsgItem || NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //依据消息项来匹配消息注册项
        pReg = MatchMessageReg(pMsgItem, pMsgObj);
        if (NULL == pReg)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //执行匹配到的消息注册项中的处理函数
        if (pReg->fnMsgFunc((void *)(pMsgItem->uiMsgValue), 
                            pMsgItem->iValueLength, 
                            pReg->pOutArg, 
                            pReg->iOutLength))
        {
            iErr = -3;
        }
    }
	
	if (!iErr)
    {
		if (pMsgItem->iMsgCode == GUIMESSAGE_TCH_DOWN)
		{
			WNDMENU1 *pMenu = NULL;
			pMenu = getCurMenu();

			if(NULL != pMenu)
			{
				CODER_LOG(CoderGu, "----CurItem %d\n", getCurItemPush());
				if(getCurItemPush() == -1)
				{
					ThreadKill(thdSignal, _SIGSPEAKER); 
				}
				else if (getCurItemPush() < pMenu->iItemCnt)
				{
					if (pMenu->CallBack)
					{
						ThreadKill(thdSignal, _SIGSPEAKER); 	
					}
				}
				else if( ((BACK_DOWN == getCurItemPush()) && ((pMenu->iBackHome & 0xff00) >> 8)) || 
						 ((HOME_DOWN == getCurItemPush()) && (pMenu->iBackHome & 0x00ff)) )
				{
					ThreadKill(thdSignal, _SIGSPEAKER); 	
				}
			}
			else
			{
				ThreadKill(thdSignal, _SIGSPEAKER);
			}
		}
		else if(pMsgItem->iMsgCode == GUIMESSAGE_KEY_DOWN)
		{
			ThreadKill(thdSignal, _SIGSPEAKER);
		}
	}

    return iErr;
}

