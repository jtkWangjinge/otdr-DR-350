/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guimessage.c
* ժ    Ҫ��  GUIMESSAGE����ϵͳ����ģ�飬guimessage.cʵ����GUI����Ϣ���͵����
*             ����������ʵ����Ϣ�ĵ���ģ�͡�
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  yexin.zhu
* ������ڣ�  2012-8-21
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guimessage.h"


/***************************************
* Ϊʵ��GUIMESSAGE����Ҫ���õ�����ͷ�ļ�
***************************************/
//#include ""


/********************
* GUI�еĵ�ǰ�¼�����
********************/
static GUIMESSAGE *pCurrMessage = NULL;


/***
  * ���ܣ�
        ����ָ������Ϣֱ�Ӵ�����Ϣ����
  * ������
        1.int iQueueLimit:      ��Ҫ��������Ϣ�������Ϣ���е���󳤶�
        2.int iRegLimit:        ��Ҫ��������Ϣ�������Ϣע����е���󳤶�
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIMESSAGE* CreateMessage(int iQueueLimit, int iRegLimit)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIMESSAGE *pMsgObj = NULL;

    if (iErr == 0)
    {
        //���Է����ڴ�
        pMsgObj = (GUIMESSAGE *) malloc(sizeof(GUIMESSAGE));
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //������Ϣ�������Ϣ����
        pMsgObj->pMsgQueue = CreateList(iQueueLimit);
        if (NULL == pMsgObj->pMsgQueue)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //������Ϣ�������Ϣע�����
        pMsgObj->pMsgReg = CreateList(iRegLimit);
        if (NULL == pMsgObj->pMsgReg)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //��ʼ��������
        if (InitMutex(&(pMsgObj->Mutex)))
        {
            iErr = -4;
        }
    }

    //������
    switch (iErr)
    {
    case -4:
        DestroyList(pMsgObj->pMsgReg);
    case -3:
        DestroyList(pMsgObj->pMsgQueue);
    case -2:
        free(pMsgObj);
    case -1:
        pMsgObj = NULL;
    default:
        break;
    }

    return pMsgObj;
}


/***
  * ���ܣ�
        ɾ����Ϣ����
  * ������
        1.GUIMESSAGE **ppMsgObj:    ָ���ָ�룬ָ����Ҫ���ٵ���Ϣ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyMessage(GUIMESSAGE **ppMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppMsgObj)
        {
            iErr = -1; 
        }
    }

    if (iErr == 0)
    {
        //�ж�ppMsgObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppMsgObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //������Ϣ�������Ϣ����
        if (DestroyList((*ppMsgObj)->pMsgQueue))
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //������Ϣ�������Ϣע�����
        if (DestroyList((*ppMsgObj)->pMsgReg))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //������Ϣ���󣬲���ָ���ÿ�
        free(*ppMsgObj);
        *ppMsgObj = NULL;
    }

    return iErr;
}


/***
  * ���ܣ�
        �õ���ǰ��Ϣ����
  * ������
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIMESSAGE* GetCurrMessage(void)
{
    return pCurrMessage;
}


/***
  * ���ܣ�
        ����Ϊ��ǰ��Ϣ����
  * ������
        1.GUIMESSAGE *pEventObj:    ������Ϊ��ǰ��Ϣ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetCurrMessage(GUIMESSAGE *pMsgObj)
{
    //�ж�pEventObj�Ƿ���Ч
    if (NULL == pMsgObj)
    {
        return -1;
    }

    pCurrMessage = pMsgObj;

    return 0;
}


/***
  * ���ܣ�
        ��ָ����Ϣ�����н����ض����͵���Ϣ��
  * ������
        1.int iMsgType:         ��Ҫ����Ϣ�������
        2.GUIMESSAGE *pMsgObj:  ָ������Ϣ����
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
    ���������ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
GUIMESSAGE_ITEM* RecvSpecialMessage(int iMsgType, GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIMESSAGE_ITEM *pReturn = NULL;
    //��ʱ��������
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж���Ϣ�����Ƿ�����Чָ�����Ϣ�����Ƿ�Ϊ��
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //������Ϣ�����ҵ�����ƥ�����Ϣ��
        list_next_each(pItem, pMsgObj->pMsgQueue, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pReturn = (GUIMESSAGE_ITEM *) (pPkt->pObj);
            if (pReturn->iMsgType == iMsgType)
            {
                //�ҵ�����ƥ�����Ϣ������Ϣ������ɾ��
                if (ListDel(pItem, pMsgObj->pMsgQueue))
                {
                    iErr = -3;
                }
                break;
            }
        }

        //���û���ҵ�����ƥ�����Ϣ��
        if (iPos == -1)
        {
            iErr = -4;
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
    case -4:
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
        ����ָ����Ϣ�����ж���ͷ������Ϣ��
  * ������
        1.GUIMESSAGE *pMsgObj:  ӵ������¼����е��¼�����
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
    ���������ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
GUIMESSAGE_ITEM* ReadMessageQueue(GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIMESSAGE_ITEM *pReturn = NULL;
    //��ʱ��������
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж���Ϣ�����Ƿ�Ϊ��Чָ�����Ϣ�����Ƿ�Ϊ��
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListEmpty(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        pPkt = container_of(pMsgObj->pMsgQueue->pHead, GUIOBJ, List);
        pReturn = (GUIMESSAGE_ITEM *) (pPkt->pObj);
        //����Ϣ������ɾ��ͷ�б���
        if (ListDel(pMsgObj->pMsgQueue->pHead, pMsgObj->pMsgQueue))
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
        д����Ϣ�ָ����Ϣ����Ķ���β��
  * ������
        1.GUIMESSAGE_ITEM *pMsgItem:    ��Ҫ��д����Ϣ�����е���Ϣ��
        2.GUIMESSAGE *pMsgObj:          ָ������Ϣ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
    ���������ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
int WriteMessageQueue(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgItem��pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgItem || NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�pMsgQueue�Ƿ�Ϊ��Чָ�����Ϣ�����Ƿ�����
        if ((NULL == pMsgObj->pMsgQueue) || 
            ListFull(pMsgObj->pMsgQueue))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ�
        pPkt = (GUIOBJ *) malloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //�����Ϣ���Ϣ����
        pPkt->pObj = pMsgItem;
        if (ListAdd(&(pPkt->List), pMsgObj->pMsgQueue))
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
        �����Ϣ����
  * ������
        1.GUIMESSAGE *pMsgObj:      ��Ҫ�������Ϣ���е���Ϣ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
int ClearMessageQueue(GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж��¼�����pMsgQueue�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj->pMsgQueue)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //����ɾ����Ϣ������
        while (pMsgObj->pMsgQueue->iCount)
        {
            pPkt= container_of(pMsgObj->pMsgQueue->pHead, GUIOBJ, List);
            pItem = (GUIMESSAGE_ITEM *) (pPkt->pObj);

            if (ListDel(pMsgObj->pMsgQueue->pHead, pMsgObj->pMsgQueue))
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
        ע����Ϣע���ָ����Ϣ����
  * ������
        1.int iMsgCode:         ��Ϣע�������Ϣ����
        2.void *pRecvObj:       ��Ϣע�������Ϣ���ն���
        3.MSGFUNC fnMsgFunc:    ��Ϣע����Ĵ�����Ϣ�ĺ���
        4.void *pOutArg:        ��Ϣע�������Ϣ�������Ĵ�������
        5.int iOutLength:       ��Ϣע����Ĵ��������ĳ���
        6.GUIMESSAGE *pMsgObj:  ӵ����Ϣע����е���Ϣ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
int LoginMessageReg(int iMsgCode, void *pRecvObj, 
                    MSGFUNC fnMsgFunc, void *pOutArg, int iOutLength, 
                    GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIMESSAGE_REG *pReg = NULL;
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�pMsgReg�Ƿ�Ϊ��Чָ�����Ϣ�����Ƿ�����
        if ((NULL == pMsgObj->pMsgReg) || 
            ListFull(pMsgObj->pMsgReg))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //ΪpReg�����ڴ�
        pReg = (GUIMESSAGE_REG *) malloc(sizeof(GUIMESSAGE_REG));
        if (NULL == pReg)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //ΪpMsg�����ڴ�
        pPkt = (GUIOBJ *) malloc(sizeof(GUIOBJ));
        if (NULL == pPkt)
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //������Ϣע����ĳ�Ա����
        pReg->iMsgCode = iMsgCode;
        pReg->pRecvObj = pRecvObj;
        pReg->fnMsgFunc = fnMsgFunc;
        pReg->pOutArg = pOutArg;
        pReg->iOutLength = iOutLength;
        //�����Ϣע�����Ϣע�����
        pPkt->pObj = pReg;
        if (ListAdd(&(pPkt->List), pMsgObj->pMsgReg))
        {
            iErr = -5;
        }
    }

    //������
    switch (iErr)
    {
    case -5:
        free(pPkt);
    case -4:
        free(pReg);
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
        ��ָ����Ϣ����ע����Ϣע����
  * ������
        1.int iMsgCode:         ��Ҫע������Ϣע�������Ϣ����
        2.void *pRecvObj:       ��Ҫע������Ϣע�������Ϣ���ն���
        3.GUIMESSAGE *pMsgObj:  ӵ����ע����Ϣע����е��¼�����
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
        �����ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
int LogoutMessageReg(int iMsgCode, void *pRecvObj, GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_REG *pReg = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж���Ϣע������Ƿ�����Чָ�����Ϣע������Ƿ�Ϊ��
        if ((NULL == pMsgObj->pMsgReg) || 
            ListEmpty(pMsgObj->pMsgReg))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //�����б�
        list_next_each(pItem, pMsgObj->pMsgReg, iPos)
        {
            pPkt = container_of(pItem, GUIOBJ, List);
            pReg = (GUIMESSAGE_REG *) (pPkt->pObj);

            //���¼�������ɾ����Ϣע����
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

        //û���ҵ�ƥ�����Ϣע����
        if (iPos == -1)
        {
            iErr = -4;
        }
    }

    //�ͷ���Դ
    if (iErr == 0)
    {
        free(pReg);
        free(pPkt);
    }

  return iErr;
}


/***
  * ���ܣ�
        �����Ϣע����Ϣ
  * ������
        1.GUIMESSAGE *pMsgObj:      ��Ҫ�������Ϣע����е���Ϣ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
int ClearMessageReg(GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIOBJ *pPkt = NULL;
    GUIMESSAGE_REG *pItem = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж���Ϣ����pMsgReg�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgObj->pMsgReg)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //����ɾ����Ϣע�������
        while (pMsgObj->pMsgReg->iCount)
        {
            pPkt= container_of(pMsgObj->pMsgReg->pHead, GUIOBJ, List);
            pItem = (GUIMESSAGE_REG *) (pPkt->pObj);

            if (ListDel(pMsgObj->pMsgReg->pHead, pMsgObj->pMsgReg))
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
       ��ָ����Ϣ������ƥ������Ϣ���Ӧ����Ϣע����
  * ������
        1.GUIMESSAGE_ITEM *pMsgItem:    ��ƥ�����Ϣ������
        2.GUIMESSAGE *pMsgObj:          ��ƥ�����Ϣ����
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ��ؿ�ָ��
  * ��ע��
        �����ڳ���������Ϣ�����еĻ���������ǰ���²��ܵ��ã�����ᵼ�¾�̬
***/
GUIMESSAGE_REG* MatchMessageReg(GUIMESSAGE_ITEM *pMsgItem, 
                                GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIMESSAGE_REG *pReturn = NULL;
    //��ʱ��������
    int iPos;
    GUILIST_ITEM *pItem = NULL;
    GUIOBJ *pPkt = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgItem��pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgItem || NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�pMsgReg�Ƿ�Ϊ��Чָ����Ƿ�Ϊ��
        if ((NULL == pMsgObj->pMsgReg) || 
            ListEmpty(pMsgObj->pMsgReg))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //������Ϣע�����
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

        //û���ҵ�ƥ�����Ϣע����
        if (iPos == -1)
        {
            iErr = -3;
        }
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
       ��ָ����Ϣ������ƥ����Ϣ��ִ�ж�Ӧ����Ϣ������
  * ������
        1.GUIMESSAGE_ITEM *pMsgItem:    ����ƥ����Ϣע�������Ϣ��
        2.GUIMESSAGE *pMsgObj:          ָ������Ϣ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����ڳ���������Ϣ�����еĻ���������ǰ���µ��ã�����ᵼ������
***/
int ExecMessageProc(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIMESSAGE_REG *pReg = NULL;

    if (iErr == 0)
    {
        //�ж�pMsgItem��pMsgObj�Ƿ�Ϊ��Чָ��
        if (NULL == pMsgItem || NULL == pMsgObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //������Ϣ����ƥ����Ϣע����
        MutexLock(&(pMsgObj->Mutex));
        pReg = MatchMessageReg(pMsgItem, pMsgObj);
        MutexUnlock(&(pMsgObj->Mutex));
        if (NULL == pReg)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //ִ��ƥ�䵽����Ϣע�����еĴ�����
        if (pReg->fnMsgFunc((void *)(pMsgItem->uiMsgValue), 
                            pMsgItem->iValueLength, 
                            pReg->pOutArg, 
                            pReg->iOutLength))
        {
            iErr = -3;
        }
    }

    return iErr;
}

