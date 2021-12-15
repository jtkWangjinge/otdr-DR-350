/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guievent.c
* 摘    要：  GUIEVENT属于系统调用模块，guievent.c实现了事件类型的相关操作，用
*             于实现事件的定义和封装。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：
*******************************************************************************/

#include "guievent.h"


/*************************************
* 为实现GUIEVENT而需要引用的其他头文件
*************************************/
#include "guischeduler.h"
#include "guimessage.h"
#include "guiwindow.h"

#include "app_global.h"


/********************
* GUI中的当前事件对象
********************/
static GUIEVENT *pCurrEvent = NULL;


/***
  * 功能：
        依据指定的信息创建事件对象
  * 参数：
        1.int iQueueLimit:              需要创建事件队列的最大长度
        2.THREADFUNC fnEventThread:     回调函数，用于创建帧缓冲的线程
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIEVENT* CreateEvent(int iQueueLimit, THREADFUNC fnEventThread)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIEVENT *pEventObj = NULL;

    if (!iErr)
    {
        //判断fnEventThread是否为有效指针
        if (NULL == fnEventThread)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //申请内存资源
        pEventObj = (GUIEVENT *) malloc(sizeof(GUIEVENT));
        if (NULL == pEventObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //创建事件队列
        pEventObj->pEventQueue = CreateList(iQueueLimit);
        if (NULL == pEventObj->pEventQueue)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //设置新建事件对象的fnEventThread，并尝试初始化Mutex
        pEventObj->fnEventThread = fnEventThread;
        if (InitMutex(&(pEventObj->Mutex), NULL))
        {
            iErr = -4;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -4:
        DestroyList(pEventObj->pEventQueue);
        //no break
    case -3:
        free(pEventObj);
        //no break
    case -2:
    case -1:
        pEventObj = NULL;
        //no break
    default:
        break;
    }

    return pEventObj;
}


/***
  * 功能：
        删除事件对象
  * 参数：
        1.GUIEVENT **ppEventObj:    指针的指针，指向需要销毁的事件对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyEvent(GUIEVENT **ppEventObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppEventObj是否为有效指针
        if (NULL == ppEventObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppEventObj所指向的是否为有效指针
        if (NULL == *ppEventObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //销毁事件队列，释放资源
        if (DestroyList((*ppEventObj)->pEventQueue))
        {
            iErr = -3;
        }
        //销毁事件对象，并将指针置空
        free(*ppEventObj);
        *ppEventObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        获得当前事件对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIEVENT* GetCurrEvent(void)
{
    return pCurrEvent;
}


/***
  * 功能：
        设置为当前事件对象
  * 参数：
        1.GUIEVENT *pEventObj:  被设置为当前事件对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrEvent(GUIEVENT *pEventObj)
{
    //判断pEventObj是否有效
    if (NULL == pEventObj)
    {
        return -1;
    }

    pCurrEvent = pEventObj;

    return 0;
}


/***
  * 功能：
        读出指定事件对象中的事件项
  * 参数：
        1.GUIEVENT *pEventObj:      需要读的事件对象
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
GUIEVENT_ITEM* ReadEventQueue(GUIEVENT *pEventObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIEVENT_ITEM *pReturn = NULL;
    //临时变量定义
    GUIOBJ *pPkt = NULL;

    //加锁
    MutexLock(&(pEventObj->Mutex));

    if (!iErr)
    {
        //判断pEventObj是否为有效指针
        if (NULL == pEventObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断事件对象的pEventQueue是否为有效指针和是否为空
        if ((NULL == pEventObj->pEventQueue) || 
            ListEmpty(pEventObj->pEventQueue))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //通过pEeventObj的GUILIST_ITEM成员获得pPkt的地址
        pPkt = container_of(pEventObj->pEventQueue->pHead, GUIOBJ, List);
        pReturn = (GUIEVENT_ITEM *) (pPkt->pObj);
        //从事件队列中删除头列表项
        if (ListDel(pEventObj->pEventQueue->pHead, pEventObj->pEventQueue))
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
    MutexUnlock(&(pEventObj->Mutex));

    return pReturn;
}


/***
  * 功能：
        写入事件项到指定的事件对象
  * 参数：
        1.GUIEVENT_ITEM *pEventItem:    需要写入事件对象的事件项
        2.GUIEVENT *pEventObj:          被设置为当前事件对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int WriteEventQueue(GUIEVENT_ITEM *pEventItem, GUIEVENT *pEventObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;

    //加锁
    MutexLock(&(pEventObj->Mutex));

    if (!iErr)
    {
        //判断pEventObj, pEventItem是否为有效指针
        if (NULL == pEventObj || NULL == pEventItem)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断事件对象的pEventQueue是否有有效指针和是否为满
        if ((NULL == pEventObj->pEventQueue) || 
            ListFull(pEventObj->pEventQueue))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //为pPkt申请资源
        pPkt = (GUIOBJ *) malloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //把事件项插入到事件对象的事件队列中
        pPkt->pObj = pEventItem;
        if (ListAdd(&(pPkt->List), pEventObj->pEventQueue))
        {
            iErr = -4;
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
    MutexUnlock(&(pEventObj->Mutex));

    return iErr;
}


/***
  * 功能：
        清空事件对象的事件队列
  * 参数：
        1.GUIEVENT *pEventObj:      需要被清空事件队列的事件对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（消息对象中的互斥锁），不能在持有锁的前提下调用
***/
int ClearEventQueue(GUIEVENT *pEventObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIOBJ *pPkt = NULL;
    GUIEVENT_ITEM *pItem = NULL;

    //加锁
    MutexLock(&(pEventObj->Mutex));

    if (!iErr)
    {
        //判断pEventObj是否为有效指针
        if (NULL == pEventObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断事件队列pEventQueue是否为有效指针
        if (NULL == pEventObj->pEventQueue)
        {
            iErr = -2;
        }
    }
    
    if (!iErr)
    {
        //遍历删除事件队列项
        while (pEventObj->pEventQueue->iCount)
        {
            pPkt= container_of(pEventObj->pEventQueue->pHead, GUIOBJ, List);
            pItem = (GUIEVENT_ITEM *) (pPkt->pObj);

            if (ListDel(pEventObj->pEventQueue->pHead, pEventObj->pEventQueue))
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
    MutexUnlock(&(pEventObj->Mutex));

    return iErr;
}


/***
  * 功能：
        事件封装线程入口函数
  * 参数：
        1.void * pThreadArg:    线程入口函数参数，类型为(GUIEVENT *)
  * 返回：
        返回线程结束状态
  * 备注：
***/
void* EventPacketThread(void *pThreadArg)
{
    //错误标志、返回值定义
    static int iReturn = 0;
    //临时变量定义
    int iExit = 0;
    GUIEVENT *pEventObj = pThreadArg;

    //等待100ms，以便GUI主进程准备好资源
    MsecSleep(100);

    //主工作循环
    while (1)
    {
        iExit = GetExitFlag();
        if (iExit)
        {
            break;
        }

        //输入事件的封装，生成事件消息项
        EventPacketFunc(pEventObj);
        //睡眠50毫秒
        MsecSleep(50);
    }

    //退出线程
    ThreadExit(&iReturn);
    return &iReturn;    //该语句只是为了消除编译器报警，实际上不起作用
}


/***
  * 功能：
        默认的事件封装函数，用于完成事件封装线程的处理，生成消息项
  * 参数：
        1.GUIEVENT *pEventObj:    处理函数的输入事件对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int EventPacketFunc(GUIEVENT *pEventObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIEVENT_ITEM *pEvent = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;
    GUIMESSAGE *pMsgObj = NULL;

    while (1)
    {
        if (!iErr)
        {
            //读取事件队列中事件项
            pEvent = ReadEventQueue(pEventObj);
            if (NULL == pEvent)
            {
                iErr = -1;
                break;
            }
        }

        if (!iErr)
        {
            //为消息项申请资源
            pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
            if (NULL == pItem)
            {
                iErr = -2;
                break;
            }
        }

        if (!iErr)
        {
            //为消息项各成员赋值
            pItem->iMsgType = GUIMESSAGE_TYP_WND;
            switch (pEvent->uiEventCode)
            {
            case GUIEVENT_KNB_CLOCK:
                pItem->iMsgCode = GUIMESSAGE_KNB_CLOCK;
                break;
            case GUIEVENT_KNB_ANTI:
                pItem->iMsgCode = GUIMESSAGE_KNB_ANTI;
                break;
            case GUIEVENT_KEY_DOWN:
                pItem->iMsgCode = GUIMESSAGE_KEY_DOWN;
                break;
            case GUIEVENT_KEY_UP:
                pItem->iMsgCode = GUIMESSAGE_KEY_UP;
                break;
            case GUIEVENT_KEY_PRESS:
                pItem->iMsgCode = GUIMESSAGE_KEY_PRESS;
                break;
            case GUIEVENT_KEY_CANCEL_PRESS:
                pItem->iMsgCode = GUIMESSAGE_KEY_CANCEL_PRESS;
                break;
            case GUIEVENT_ABS_DOWN:
                pItem->iMsgCode = GUIMESSAGE_TCH_DOWN;
                break;
            case GUIEVENT_ABS_UP:
                pItem->iMsgCode = GUIMESSAGE_TCH_UP;
                break;
            case GUIEVENT_MOV_CURSOR:
                pItem->iMsgCode = GUIMESSAGE_MOV_CURSOR;
                break;
			case GUIEVENT_MT:
				pItem->iMsgCode = GUIMESSAGE_MT;
				break;
            default:
                break;
            }
            pItem->uiMsgValue = pEvent->uiEventValue;
            pItem->iValueLength = 0;
            pItem->pRecvObj = MatchEventObject(pEvent);
            if (NULL == pItem->pRecvObj)
            {
                LOG(LOG_ERROR, "MATCH EVENT FAILED!!!\n");
                iErr = -3;
                break;
            }
        }

        if (!iErr)
        {
            //得到当前消息对象
            pMsgObj = GetCurrMessage();
            if (NULL == pMsgObj)
            {
                iErr = -4;
                break;
            }
        }

        if (!iErr)
        {
            //消息项写入消息队列
            if (WriteMessageQueue(pItem, pMsgObj,0))
            {
                iErr = -5;
                LOG(LOG_ERROR, "write EVENT message FAILED!!!\n");
                break;
            }
        }

        if (!iErr)
        {
            //释放读取出来的事件项
            free(pEvent);
        }
    }
	
    //错误处理
    switch (iErr)
    {
    case -5:
        MutexUnlock(&(pMsgObj->Mutex));
        //no break
    case -4:
    case -3:
        free(pItem);
        //no break
    case -2:
        free(pEvent);
        //no break
    case -1:
    default:
        break;
    }

    return iErr;
}


/***
  * 功能：
        从GUI窗体控件队列中匹配事件对象，在事件封装中用于确定消息接收对象
  * 参数：
        1.GUIEVENT_ITEM *pEventItem:    用来匹配GUI对象的事件项
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
void* MatchEventObject(GUIEVENT_ITEM *pEventItem)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUICOMPONENT *pReturn = NULL;
    //临时变量定义
    unsigned int uiAbsX, uiAbsY;
    int iPos;
    GUIWINDOW *pWnd = NULL;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
    GUICOMPONENT *pComp = NULL, *pMatch = NULL;

    //保持当前窗体对象
    pWnd = HoldCurrWindow();

    if (!iErr)
    {
        //判断pEventItem是否为有效指针
        if (NULL == pEventItem)
        {
            iErr = -1;
        }
        //判断pWnd是否为有效指针
        if (NULL == pWnd)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断控件队列pWndComps是否为有效指针
        if (NULL == pWnd->pWndComps)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //锁住控件队列，匹配时禁止添加或删除窗体控件
        MutexLock(&(pWnd->Mutex));
        //根据不同事件编码，对窗体控件进行事件匹配
        switch (pEventItem->uiEventCode)
        {
        case GUIEVENT_KNB_CLOCK:
        case GUIEVENT_KNB_ANTI:
        case GUIEVENT_KEY_DOWN:
        case GUIEVENT_KEY_UP:
        case GUIEVENT_KEY_PRESS:
        case GUIEVENT_KEY_CANCEL_PRESS:
            //遍历控件队列找到匹配的GUI窗体控件
            list_next_each(pItem, pWnd->pWndComps, iPos)
            {
                pPkt = container_of(pItem, GUIOBJ, List);
                pComp = (GUICOMPONENT *) (pPkt->pObj);
                if ((pComp->Visible.iEnable) && //控件必须可见
                    (pComp->Visible.iFocus))    //必须有焦点才可接收按键
                {
                    pMatch = pComp;
                    if (NULL == pReturn)        //匹配到第一个GUI窗体控件
                    {
                        pReturn = pMatch;
                    }
                    else
                    {
                        //先比较iLayer，图层值更大的更匹配
                        if (pMatch->Visible.iLayer > pReturn->Visible.iLayer)
                        {
                            pReturn = pMatch;
                        }
                        //再比较有效区域，有效区域被包含的更匹配
                        if (RectanglePortion(&(pMatch->Visible.Area),
                                             &(pReturn->Visible.Area)))
                        {
                            pReturn = pMatch;
                        }
                    }
                }
            }
            break;
        case GUIEVENT_ABS_DOWN:
        case GUIEVENT_ABS_UP:
            //得到坐标值(X和Y的值不超过65535)
            uiAbsX = (pEventItem->uiEventValue >> 16) & 0x0000FFFF;
            uiAbsY = pEventItem->uiEventValue & 0x0000FFFF;
            //遍历控件队列找到匹配的GUI窗体控件
            list_next_each(pItem, pWnd->pWndComps, iPos)
            {
                pPkt = container_of(pItem, GUIOBJ, List);
                pComp = (GUICOMPONENT *) (pPkt->pObj);

                if ((pComp->Visible.iEnable) &&     //控件必须可见
                    (uiAbsX >= pComp->Visible.Area.Start.x) && 
                    (uiAbsX <= pComp->Visible.Area.End.x) &&
                    (uiAbsY >= pComp->Visible.Area.Start.y) &&
                    (uiAbsY <= pComp->Visible.Area.End.y))
                {
                    pMatch = pComp;
                    if (NULL == pReturn)            //匹配到第一个GUI窗体控件
                    {
                        pReturn = pMatch;
                    }
                    else
                    {
                        //先比较iLayer，图层值更大的更匹配
                        if (pMatch->Visible.iLayer > pReturn->Visible.iLayer)
                        {
                            pReturn = pMatch;
                        }
                        //再比较有效区域，有效区域更小的更匹配
                        if (RectanglePortion(&(pMatch->Visible.Area),
                                             &(pReturn->Visible.Area)))
                        {
                            pReturn = pMatch;
                        }
                    }
                }
            }
            break;
        case GUIEVENT_MOV_CURSOR:
            //得到坐标值(X和Y的值不超过65535)
            uiAbsX = (pEventItem->uiEventValue >> 16) & 0x0000FFFF;
            uiAbsY = pEventItem->uiEventValue & 0x0000FFFF;
            //遍历控件队列找到匹配的GUI窗体控件
            list_next_each(pItem, pWnd->pWndComps, iPos)
            {
                pPkt = container_of(pItem, GUIOBJ, List);
                pComp = (GUICOMPONENT *) (pPkt->pObj);
                if ((pComp->Visible.iEnable) && //控件必须可见
                    (pComp->Visible.iCursor) && //必须有光标才可接收
                    (uiAbsX >= pComp->Visible.Area.Start.x) && 
                    (uiAbsX <= pComp->Visible.Area.End.x) &&
                    (uiAbsY >= pComp->Visible.Area.Start.y) &&
                    (uiAbsY <= pComp->Visible.Area.End.y))
                {
                    pMatch = pComp;
                    if (NULL == pReturn)        //匹配到第一个GUI窗体控件
                    {
                        pReturn = pMatch;
                    }
                    else
                    {
                        //先比较iLayer，图层值更大的更匹配
                        if (pMatch->Visible.iLayer > pReturn->Visible.iLayer)
                        {
                            pReturn = pMatch;
                        }
                        //再比较有效区域，有效区域被包含的更匹配
                        if (RectanglePortion(&(pMatch->Visible.Area),
                                             &(pReturn->Visible.Area)))
                        {
                            pReturn = pMatch;
                        }
                    }
                }
            }
            break;	
		case GUIEVENT_MT:
			//得到坐标值(X和Y的值不超过65535)
			//uiAbsX = (pEventItem->uiEventValue >> 16) & 0x0000FFFF;
			//uiAbsY = pEventItem->uiEventValue & 0x0000FFFF;
			//遍历控件队列找到匹配的GUI窗体控件
			list_next_each(pItem, pWnd->pWndComps, iPos)
			{
				pPkt = container_of(pItem, GUIOBJ, List);
				pComp = (GUICOMPONENT *) (pPkt->pObj);
				if ((pComp->Visible.iEnable) && //控件必须可见
					(pComp->Visible.iCursor)) //必须有光标才可接收
				{
					pMatch = pComp;
					if (NULL == pReturn)		//匹配到第一个GUI窗体控件
					{
						pReturn = pMatch;
					}
					else
					{
						//先比较iLayer，图层值更大的更匹配
						if (pMatch->Visible.iLayer > pReturn->Visible.iLayer)
						{
							pReturn = pMatch;
						}
					}
				}
			}
			break;
        default:
            break;
        }
        //解锁控件队列，匹配完允许添加或删除窗体控件
        MutexUnlock(&(pWnd->Mutex));
    }

    //解锁当前窗体对象
    UnlockCurrWindow();
	

    return pReturn;
}

