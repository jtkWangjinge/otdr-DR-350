/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guievent.c
* ժ    Ҫ��  GUIEVENT����ϵͳ����ģ�飬guievent.cʵ�����¼����͵���ز�������
*             ��ʵ���¼��Ķ���ͷ�װ��
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  yexin.zhu
* ������ڣ�  2012-8-9
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guievent.h"


/*************************************
* Ϊʵ��GUIEVENT����Ҫ���õ�����ͷ�ļ�
*************************************/
#include "guischeduler.h"
#include "guimessage.h"
#include "guiwindow.h"


/********************
* GUI�еĵ�ǰ�¼�����
********************/
static GUIEVENT *pCurrEvent = NULL;


/***
  * ���ܣ�
        ����ָ������Ϣ�����¼�����
  * ������
        1.int iQueueLimit:              ��Ҫ�����¼����е���󳤶�
        2.THREADFUNC fnEventThread:     �ص����������ڴ���֡������߳�
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIEVENT* CreateEvent(int iQueueLimit, THREADFUNC fnEventThread)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIEVENT *pEventObj = NULL;

    if (iErr == 0)
    {
        //�ж�fnEventThread�Ƿ�Ϊ��Чָ��
        if (NULL == fnEventThread)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pEventObj = (GUIEVENT *) malloc(sizeof(GUIEVENT));
        if (NULL == pEventObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //�����¼�����
        pEventObj->pEventQueue = CreateList(iQueueLimit);
        if (NULL == pEventObj->pEventQueue)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //�����½��¼������fnEventThread�������Գ�ʼ��Mutex
        pEventObj->fnEventThread = fnEventThread;
        if (InitMutex(&(pEventObj->Mutex)))
        {
            iErr = -4;
        }
    }

    //������
    switch (iErr)
    {
    case -4:
        DestroyList(pEventObj->pEventQueue);
    case -3:
        free(pEventObj);
    case -2:
    case -1:
        pEventObj = NULL;
    default:
        break;
    }

    return pEventObj;
}


/***
  * ���ܣ�
        ɾ���¼�����
  * ������
        1.GUIEVENT **ppEventObj:    ָ���ָ�룬ָ����Ҫ���ٵ��¼�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyEvent(GUIEVENT **ppEventObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppEventObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppEventObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppEventObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppEventObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //�����¼����У��ͷ���Դ
        if (DestroyList((*ppEventObj)->pEventQueue))
        {
            iErr = -3;
        }
        //�����¼����󣬲���ָ���ÿ�
        free(*ppEventObj);
        *ppEventObj = NULL;
    }

    return iErr;
}


/***
  * ���ܣ�
        ��õ�ǰ�¼�����
  * ������
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIEVENT* GetCurrEvent(void)
{
    return pCurrEvent;
}


/***
  * ���ܣ�
        ����Ϊ��ǰ�¼�����
  * ������
        1.GUIEVENT *pEventObj:  ������Ϊ��ǰ�¼�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetCurrEvent(GUIEVENT *pEventObj)
{
    //�ж�pEventObj�Ƿ���Ч
    if (NULL == pEventObj)
    {
        return -1;
    }

    pCurrEvent = pEventObj;

    return 0;
}


/***
  * ���ܣ�
        ����ָ���¼������е��¼���
  * ������
        1.GUIEVENT *pEventObj:      ��Ҫ�����¼�����
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
        �����ڳ��������¼������еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
GUIEVENT_ITEM* ReadEventQueue(GUIEVENT *pEventObj)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIEVENT_ITEM *pReturn = NULL;
    //��ʱ��������
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //�ж�pEventObj�Ƿ�Ϊ��Чָ��
        if (NULL == pEventObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж��¼������pEventQueue�Ƿ�Ϊ��Чָ����Ƿ�Ϊ��
        if ((NULL == pEventObj->pEventQueue) || 
            ListEmpty(pEventObj->pEventQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //ͨ��pEevent��GUILIST_ITEM��Ա���pPkt�ĵ�ַ
        pPkt = container_of(pEventObj->pEventQueue->pHead, GUIOBJ, List);
        pReturn = (GUIEVENT_ITEM *) (pPkt->pObj);

        //���¼������¼�������ɾ���¼���
        if (ListDel(pEventObj->pEventQueue->pHead, pEventObj->pEventQueue))
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //�ͷ���Դ
        free(pPkt);
    }

    //������
    switch (iErr)
    {
    case -3:
        pReturn = NULL;
    case -2:
    case -1:
    default:
        break;
    }

    return pReturn;
}


/***
  * ���ܣ�
        д���¼��ָ�����¼�����
  * ������
        1.GUIEVENT_ITEM *pEventItem:    ��Ҫд���¼�������¼���
        2.GUIEVENT *pEventObj:          ������Ϊ��ǰ�¼�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����ڳ��������¼������еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
int WriteEventQueue(GUIEVENT_ITEM *pEventItem, GUIEVENT *pEventObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //�ж�pEventObj, pEventItem�Ƿ�Ϊ��Чָ��
        if (NULL == pEventObj || NULL == pEventItem)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж��¼������pEventQueue�Ƿ�����Чָ����Ƿ�Ϊ��
        if ((NULL == pEventObj->pEventQueue) || 
            ListFull(pEventObj->pEventQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //ΪpPkt������Դ
        pPkt = (GUIOBJ *) malloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //���¼�����뵽�¼�������¼�������
        pPkt->pObj = pEventItem;
        if (ListAdd(&(pPkt->List), pEventObj->pEventQueue))
        {
            iErr = -4;
        }
    }

    //������
    switch (iErr)
    {
    case -4:
        free(pPkt);
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}


/***
  * ���ܣ�
        ����¼�������¼�����
  * ������
        1.GUIEVENT *pEventObj:      ��Ҫ������¼����е��¼�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����ڳ��������¼������еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
int ClearEventQueue(GUIEVENT *pEventObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIOBJ *pPkt = NULL;
    GUIEVENT_ITEM *pItem = NULL;

    if (iErr == 0)
    {
        //�ж�pEventObj�Ƿ�Ϊ��Чָ��
        if (NULL == pEventObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж��¼�����pEventQueue�Ƿ�Ϊ��Чָ��
        if (NULL == pEventObj->pEventQueue)
        {
            iErr = -2;
        }
    }
    
    if (iErr == 0)
    {
        //����ɾ���¼�������
        while (pEventObj->pEventQueue->iCount)
        {
            pPkt= container_of(pEventObj->pEventQueue->pHead, GUIOBJ, List);
            pItem = (GUIEVENT_ITEM *) (pPkt->pObj);

            if (ListDel(pEventObj->pEventQueue->pHead, pEventObj->pEventQueue))
            {
                iErr = -3;
                break;
            }

            //�ͷ���Դ
            free(pItem);
            free(pPkt);
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        �¼���װ�߳���ں���
  * ������
        1.void * pThreadArg:    �߳���ں�������������Ϊ(GUIEVENT *)
  * ���أ�
        �����߳̽���״̬
  * ��ע��
***/
void* EventPacketThread(void *pThreadArg)
{
    //�����־������ֵ����
    static int iReturn = 0;
    //��ʱ��������
    int iExit = 0;
    GUIEVENT *pEventObj = pThreadArg;

    //�ȴ�100ms���Ա�GUI������׼������Դ
    MsecSleep(100);

    //������ѭ��
    while (1)
    {
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        //�����¼��ķ�װ�������¼���Ϣ��
        EventPacketFunc(pEventObj);
        //˯��50����
        MsecSleep(50);
    }

    //�˳��߳�
    ThreadExit(&iReturn);
    return &iReturn;    //�����ֻ��Ϊ������������������ʵ���ϲ�������
}


/***
  * ���ܣ�
        Ĭ�ϵ��¼���װ��������������¼���װ�̵߳Ĵ���������Ϣ��
  * ������
        1.GUIEVENT *pEventObj:    �������������¼�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int EventPacketFunc(GUIEVENT *pEventObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIEVENT_ITEM *pEvent = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;
    GUIMESSAGE *pMsgObj = NULL;

    while (1)
    {
        if (iErr == 0)
        {
            //��ȡ�¼��������¼���
            MutexLock(&(pEventObj->Mutex));
            pEvent = ReadEventQueue(pEventObj);
            MutexUnlock(&(pEventObj->Mutex));
            if (NULL == pEvent)
            {
                iErr = -1;
                break;
            }
        }

        if (iErr == 0)
        {
            //Ϊ��Ϣ��������Դ
            pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
            if (NULL == pItem)
            {
                iErr = -2;
                break;
            }
        }

        if (iErr == 0)
        {
            //Ϊ��Ϣ�����Ա��ֵ
            pItem->iMsgType = GUIMESSAGE_TYP_WND;
            switch (pEvent->uiEventCode)
            {
            case GUIEVENT_KEY_DOWN:
                pItem->iMsgCode = GUIMESSAGE_KEY_DOWN;
                break;
            case GUIEVENT_KEY_UP:
                pItem->iMsgCode = GUIMESSAGE_KEY_UP;
                break;
            case GUIEVENT_ABS_DOWN:
                pItem->iMsgCode = GUIMESSAGE_TCH_DOWN;
                break;
            case GUIEVENT_ABS_UP:
                pItem->iMsgCode = GUIMESSAGE_TCH_UP;
		 break;
	     case GUIEVENT_KNB_CLOCK: 
		pItem->iMsgCode = GUIMESSAGE_KNB_CLOCK; 
		break; 
	     case GUIEVENT_KNB_ANTI: 
		pItem->iMsgCode = GUIMESSAGE_KNB_ANTI;
                break;
            default:
                break;
            }
            pItem->uiMsgValue = pEvent->uiEventValue;
            pItem->iValueLength = 0;
            pItem->pRecvObj = MatchEventObject(pEvent);
            if (NULL == pItem->pRecvObj)
            {
                iErr = -3;
                break;
            }
        }

        if (iErr == 0)
        {
            //�õ���ǰ��Ϣ����
            pMsgObj = GetCurrMessage();
            if (NULL == pMsgObj)
            {
                iErr = -4;
                break;
            }
        }

        if (iErr == 0)
        {
            //��Ϣ��д����Ϣ����
            MutexLock(&(pMsgObj->Mutex));
            if (WriteMessageQueue(pItem, pMsgObj))
            {
                iErr = -5;
                break;
            }
            MutexUnlock(&(pMsgObj->Mutex));
        }

        if (iErr == 0)
        {
            //�ͷŶ�ȡ�������¼���
            free(pEvent);
        }
    }

    //������
    switch (iErr)
    {
    case -5:
        MutexUnlock(&(pMsgObj->Mutex));
    case -4:
    case -3:
        free(pItem);
    case -2:
        free(pEvent);
    case -1:
    default:
        break;
    }

    return iErr;
}


/***
  * ���ܣ�
        ��GUI����ؼ�������ƥ���¼��������¼���װ������ȷ����Ϣ���ն���
  * ������
        1.GUIEVENT_ITEM *pEventItem:    ����ƥ��GUI������¼���
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
void* MatchEventObject(GUIEVENT_ITEM *pEventItem)
{
    //�����־������ֵ����
    int iErr = 0;
    GUICOMPONENT *pReturn = NULL;
    //��ʱ��������
    unsigned int uiAbsX, uiAbsY;
    int iPos;
    GUIWINDOW *pWnd = NULL;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
    GUICOMPONENT *pComp = NULL, *pMatch = NULL;

    //���ֵ�ǰ�������
    pWnd = HoldCurrWindow();

    if (iErr == 0)
    {
        //�ж�pEventItem�Ƿ�Ϊ��Чָ��
        if (NULL == pEventItem)
        {
            iErr = -1;
        }
        //�ж�pWnd�Ƿ�Ϊ��Чָ��
        if (NULL == pWnd)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�жϿؼ�����pWndComps�Ƿ�Ϊ��Чָ��
        if (NULL == pWnd->pWndComps)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //��ס�ؼ����У�ƥ��ʱ��ֹ��ӻ�ɾ������ؼ�
        MutexLock(&(pWnd->Mutex));
        //���ݲ�ͬ�¼����룬�Դ���ؼ������¼�ƥ��
        switch (pEventItem->uiEventCode)
        {
        case GUIEVENT_KEY_DOWN:
        case GUIEVENT_KEY_UP:
	 case GUIEVENT_KNB_CLOCK:
        case GUIEVENT_KNB_ANTI:
            //�����ؼ������ҵ�ƥ���GUI����ؼ�
            list_next_each(pItem, pWnd->pWndComps, iPos)
            {
                pPkt = container_of(pItem, GUIOBJ, List);
                pComp = (GUICOMPONENT *) (pPkt->pObj);
                if ((pComp->Visible.iEnable) &&     //�ؼ�����ɼ�
                    (pComp->Visible.iFocus))
                {
                    pMatch = pComp;
                    if (NULL == pReturn)            //ƥ�䵽��һ��GUI����ؼ�
                    {
                        pReturn = pMatch;
                    }
                    else
                    {
                        //�ȱȽ�iLayer��ͼ��ֵ����ĸ�ƥ��
                        if (pMatch->Visible.iLayer > pReturn->Visible.iLayer)
                        {
                            pReturn = pMatch;
                        }
                        //�ٱȽ���Ч������Ч�����С�ĸ�ƥ��
                        if ((pMatch->Visible.Area.Start.x
                            >= pReturn->Visible.Area.Start.x) && 
                            (pMatch->Visible.Area.Start.y
                            >= pReturn->Visible.Area.Start.y) &&
                            (pMatch->Visible.Area.End.x
                            <= pReturn->Visible.Area.End.x) &&
                            (pMatch->Visible.Area.End.y
                            <= pReturn->Visible.Area.End.y))
                        {
                            pReturn = pMatch;
                        }
                    }
                }
            }
            break;
        case GUIEVENT_ABS_DOWN:
        case GUIEVENT_ABS_UP:
            //�õ�����ֵ(X��Y��ֵ������2047)
            uiAbsX = (pEventItem->uiEventValue & 0x0FFF0000) >> 16;
            uiAbsY = pEventItem->uiEventValue & 0x0FFF;
            //�����ؼ������ҵ�ƥ���GUI����ؼ�
            list_next_each(pItem, pWnd->pWndComps, iPos)
            {
                pPkt = container_of(pItem, GUIOBJ, List);
                pComp = (GUICOMPONENT *) (pPkt->pObj);
                if ((pComp->Visible.iEnable) &&     //�ؼ�����ɼ�
                    (uiAbsX >= pComp->Visible.Area.Start.x) && 
                    (uiAbsX <= pComp->Visible.Area.End.x) &&
                    (uiAbsY >= pComp->Visible.Area.Start.y) &&
                    (uiAbsY <= pComp->Visible.Area.End.y))
                {
                    pMatch = pComp;
                    if (NULL == pReturn)            //ƥ�䵽��һ��GUI����ؼ�
                    {
                        pReturn = pMatch;
                    }
                    else
                    {
                        //�ȱȽ�iLayer��ͼ��ֵ����ĸ�ƥ��
                        if (pMatch->Visible.iLayer > pReturn->Visible.iLayer)
                        {
                            pReturn = pMatch;
                        }
                        //�ٱȽ���Ч������Ч�����С�ĸ�ƥ��
                        if ((pMatch->Visible.Area.Start.x
                            >= pReturn->Visible.Area.Start.x) && 
                            (pMatch->Visible.Area.Start.y
                            >= pReturn->Visible.Area.Start.y) &&
                            (pMatch->Visible.Area.End.x
                            <= pReturn->Visible.Area.End.x) &&
                            (pMatch->Visible.Area.End.y
                            <= pReturn->Visible.Area.End.y))
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
        //�����ؼ����У�ƥ����������ӻ�ɾ������ؼ�
        MutexUnlock(&(pWnd->Mutex));
    }

    //������ǰ�������
    UnlockCurrWindow();

    return pReturn;
}

