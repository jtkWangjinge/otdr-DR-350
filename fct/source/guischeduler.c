/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guischeduler.h
* ժ    Ҫ��  ����GUI�Ĵ�����Ȼ��Ƽ���ز���������ʵ�ֶര�塢���߳����С�
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  yexin.zhu
* ������ڣ�  2012-8-31
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guischeduler.h"


/*****************************************
* Ϊʵ��GUISCHEDULER����Ҫ���õ�����ͷ�ļ�
*****************************************/
#include "guibase.h"
#include "guimessage.h"
#include "guiwindow.h"


/**********************************
* ����GUI�еĽ����˳���־���以����
**********************************/
//GUI�еĽ����˳���־
static int iExitFlag = 0;
//���ڿ��Ʒ��ʽ����˳���־�Ļ�����
static GUIMUTEX mutexExitFlag;


/**************************
* ����GUI�еĴ����̻߳�����
**************************/
//���ڿ��ƴ����߳������Ļ�����(GUIĿǰֻ֧��һ�������߳�)
static GUIMUTEX mutexThreadLock;


/***
  * ���ܣ�
        ��ʼ��GUI�����˳���־
  * ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        Ĭ�Ͻ������˳���־��ʼ��Ϊ0
***/
int InitExitFlag(void)
{
    //�����־������ֵ����
    int iReturn = 0;

    if (InitMutex(&mutexExitFlag))
    {
        iReturn = -1;
    }
    else 
    {
        MutexLock(&mutexExitFlag);   
        iExitFlag = 0;
        MutexUnlock(&mutexExitFlag);
    }

    return iReturn;
}


/***
  * ���ܣ�
        �õ�GUI�����˳���־
  * ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int GetExitFlag(void)
{
    //�����־������ֵ����
    int iReturn = 0;

    MutexLock(&mutexExitFlag);   
    iReturn = iExitFlag;
    MutexUnlock(&mutexExitFlag);

    return iReturn;
}


/***
  * ���ܣ�
        ����GUI�����˳���־
  * ������
        1.int iFlag:    ��Ҫ��Ϊ��GUI�����˳���־
  * ���أ�
        ��
  * ��ע��
***/
void SetExitFlag(int iFlag)
{
    MutexLock(&mutexExitFlag);   
    iExitFlag = iFlag;
    MutexUnlock(&mutexExitFlag);
}


/***
  * ���ܣ�
        ��������߳���ں���
  * ������
        1.void *pThreadArg:     �߳���ں�������������Ϊ(GUIMESSAGE *)
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
void* WindowSchedulerThread(void *pThreadArg)
{
    //�����־������ֵ����
    static int iReturn = 0;
    //��ʱ��������
    int iExit = 0;
    GUIMESSAGE *pMsg = (GUIMESSAGE *) pThreadArg;
    GUIMESSAGE_ITEM *pRecv = NULL;
    GUIWINDOW *pWnd = NULL;
    GUITHREAD thdWnd = -1;
    THREADFUNC fnThread = NULL;

    //�ȴ�100ms���Ա�GUI������׼������Դ
    MsecSleep(100);

    //������ѭ��
    while (1)
    {
        //���ݽ�����־�ж��Ƿ���Ҫ���������߳�
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        //����Ϣ���ж��ض�������Ϣ��(ϵͳ��Ϣ)
        MutexLock(&(pMsg->Mutex));
        pRecv = RecvSpecialMessage(GUIMESSAGE_TYP_SYS, pMsg);
        MutexUnlock(&(pMsg->Mutex));
        if (NULL == pRecv)
        {
            MsecSleep(50);          //��û��ϵͳ��Ϣ��˯��50����
            continue;
        }

        //���ݲ�ͬϵͳ��Ϣ���������Ӧ����
        switch (pRecv->iMsgCode)
        {
        case GUIMESSAGE_SYS_EXIT:
            //ϵͳ�˳�����GUI�����˳���־
            SetExitFlag(1);
            break;
        case GUIMESSAGE_THD_CREAT:
            //���������̣߳���ThreadCreate()
            pWnd = (GUIWINDOW *) (pRecv->uiMsgValue);
            if (NULL != pWnd)                           //����Ϣֵ��Ч
            {                                           //���õ�Ҫ�����Ĵ���ṹ
                if (NULL != pWnd->fnWndThread)          //�ж��Ƿ�ָ���̺߳���
                {                                       //����ָ�������̺߳���
                    fnThread = pWnd->fnWndThread;       //ʹ��ָ�����̺߳�����
                }                                       //���������߳�
                else                                    //����
                {                                       //��δָ�������̺߳���
                    fnThread = DefaultWindowThread;     //ʹ��Ĭ�ϵ��̺߳�����
                }                                       //���������߳�
                ThreadCreate(&thdWnd, fnThread, pWnd);  //��ɴ����̵߳Ĵ���
                pWnd->thdWndTid = thdWnd;               //�����߳�ID
            }
            break;
        case GUIMESSAGE_THD_DEST:
            //���ٴ����̣߳���ThreadJoin()
            pWnd = (GUIWINDOW *) (pRecv->uiMsgValue);
            if (NULL != pWnd)                           //����Ϣֵ��Ч
            {                                           //���õ�Ҫ�����Ĵ���ṹ
                thdWnd = pWnd->thdWndTid;               //��ȡ�����Ӧ���߳�ID
                ThreadJoin(thdWnd, NULL);               //�ȴ��߳̽�����������Դ
                if (pWnd != GetCurrWindow())            //����ǰ��������������
                {                                       //���µĴ�������ɻ���
                    DestroyWindow(&pWnd);               //���پɵĴ���
                }
            }
            break;
        default:
            break;
        }

        //�ͷŶ�������Ϣ��
        free(pRecv);
    }

    //�˳��߳�
    ThreadExit(&iReturn);
    return &iReturn;    //�����ֻ��Ϊ������������������ʵ���ϲ�������
}


/***
  * ���ܣ�
        Ĭ�ϴ��崦���߳���ں���
  * ������
        1.void *pThreadArg:     �߳���ں�������������Ϊ(GUIWINDOW *)
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
        Ĭ�ϵĴ��崦���̺߳��������ڴ�������ʱ����δָ��fnWndThread����ʹ�øú�
        ���������崦���̡߳������Ҫʹ���Զ���ĺ�������Ϊ���崦���̣߳���ο�
        �������Ŀ�ܽṹ��
***/
void* DefaultWindowThread(void *pThreadArg)
{
    //�����־������ֵ����
    static int iReturn = 0;
    //��ʱ��������
    int iRun = 1;
    int iLoop = 1;
    GUIWINDOW *pWnd = (GUIWINDOW *) pThreadArg;
    GUIMESSAGE *pMsg = NULL;
    GUIMESSAGE_ITEM *pItem = NULL;
    GUIMESSAGE_ITEM *pRecv = NULL;

    //��ס��ǰ�����߳�
    LockWindowThread();

    //�õ���ǰ��Ϣ����
    pMsg = GetCurrMessage();

    //��������ϢGUIMESSAGE_WND_INIT������
    pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
    pItem->iMsgType = GUIMESSAGE_TYP_WND;
    pItem->iMsgCode = GUIMESSAGE_WND_INIT;
    MutexLock(&(pMsg->Mutex));
    WriteMessageQueue(pItem, pMsg); //pItem�ɱ��������Ϣ�����ͷ�
    MutexUnlock(&(pMsg->Mutex));

    //���崦��
    while (iRun)
    {
        //����Ϣ���ж�������Ϣ��
        MutexLock(&(pMsg->Mutex));
        pRecv = RecvSpecialMessage(GUIMESSAGE_TYP_WND, pMsg);
        MutexUnlock(&(pMsg->Mutex));
        if (NULL == pRecv)
        {
            MsecSleep(20);          //��û�д�����Ϣ��˯��20����
            continue;
        }

        //���ݲ�ͬ������Ϣ���������Ӧ����
        switch (pRecv->iMsgCode)
        {
        case GUIMESSAGE_WND_INIT:
            if (NULL != pWnd->fnWndInit)
            {
                //�����ʼ������fnWndInit()
                pWnd->fnWndInit(pWnd);
                //��ʼ����ɺ󣬷���GUIMESSAGE_WND_PAINT�����д������
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_WND;
                pItem->iMsgCode = GUIMESSAGE_WND_PAINT;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem�ɱ��������Ϣ�����ͷ�
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_WND_EXIT:
            if (NULL != pWnd->fnWndExit)
            {
                //�����˳�����fnWndExit()
                pWnd->fnWndExit(pWnd);
                iRun = 0;                       //���б�־��0�����崦�����
                //�˳���ɺ󣬷���GUIMESSAGE_THRD_DEST�����ٴ����߳�
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_SYS;
                pItem->iMsgCode = GUIMESSAGE_THD_DEST;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem�ɴ�������߳��ͷ�
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_WND_PAINT:
            if (NULL != pWnd->fnWndPaint)
            {
                //������ƣ���fnWndPaint()
                pWnd->fnWndPaint(pWnd);
                //�����ѻ��ƣ����ý��㼰��ǰ����
                pWnd->Visible.iFocus = 1;       //��ϵ���԰����¼��ķ�װ
                SetCurrWindow(pWnd);            //��ϵ���¼���װ�߳��ܷ���
                //������ɺ󣬷���GUIMESSAGE_WND_LOOP�����д���ѭ��
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_WND;
                pItem->iMsgCode = GUIMESSAGE_WND_LOOP;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem�ɴ�������߳��ͷ�
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_WND_LOOP:
            if (NULL != pWnd->fnWndLoop)
            {
                //����ѭ����־�ж��Ƿ�������ѭ��
                if (iLoop != 1)
                {
                    break;                      //�����������Դ���ѭ��
                }
                //����ѭ������fnWndLoop()
                pWnd->fnWndLoop(pWnd);
                //���δ�����ɺ󣬷���GUIMESSAGE_WND_LOOP��������һ�δ���ѭ��
                pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
                pItem->iMsgType = GUIMESSAGE_TYP_WND;
                pItem->iMsgCode = GUIMESSAGE_WND_LOOP;
                pItem->uiMsgValue = (unsigned int) pWnd;
                MutexLock(&(pMsg->Mutex));
                WriteMessageQueue(pItem, pMsg); //pItem�ɴ�������߳��ͷ�
                MutexUnlock(&(pMsg->Mutex));
            }
            break;
        case GUIMESSAGE_LOP_ENABLE:
            //ѭ����־��1��������ѭ��
            iLoop = 1;
            //����GUIMESSAGE_WND_LOOP��������������ѭ��
            pItem = (GUIMESSAGE_ITEM *) malloc(sizeof(GUIMESSAGE_ITEM));
            pItem->iMsgType = GUIMESSAGE_TYP_WND;
            pItem->iMsgCode = GUIMESSAGE_WND_LOOP;
            pItem->uiMsgValue = (unsigned int) pWnd;
            MutexLock(&(pMsg->Mutex));
            WriteMessageQueue(pItem, pMsg);     //pItem�ɴ�������߳��ͷ�
            MutexUnlock(&(pMsg->Mutex));
            break;
        case GUIMESSAGE_LOP_DISABLE:
            //ѭ����־��0����ֹ����ѭ��
            iLoop = 0;
            break;
        default:
            //��������������Ϣ����
            ExecMessageProc(pRecv, pMsg);
            break;
        }

        //�ͷŶ�������Ϣ��
        free(pRecv);
    }

    //������ǰ�����߳�
    UnlockWindowThread();

    //�˳��߳�
    ThreadExit(&iReturn);
    return &iReturn;    //�����ֻ��Ϊ������������������ʵ���ϲ�������
}


/***
  * ���ܣ�
        ��ʼ�������̻߳�����
  * ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int InitThreadLock(void)
{
    //�����־������ֵ����
    int iReturn = 0;

    if (InitMutex(&mutexThreadLock))
    {
        iReturn = -1;
    }

    return iReturn;
}


/***
  * ���ܣ�
        ��ס��ǰ�����߳�
  * ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����߳����к�Ӧ����һʱ����øú������Ա�֤GUI��ʼ��ֻ��һ�������߳�
***/
int LockWindowThread(void)
{
    //�����־������ֵ����
    int iReturn = 0;

    iReturn = MutexLock(&mutexThreadLock);

    return iReturn;
}


/***
  * ���ܣ�
        ������ǰ�����߳�
  * ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        �����߳��˳�ǰ��������øú�������ʹ���������̵߳�������
***/
int UnlockWindowThread(void)
{
    //�����־������ֵ����
    int iReturn = 0;

    iReturn = MutexUnlock(&mutexThreadLock);

    return iReturn;
}

