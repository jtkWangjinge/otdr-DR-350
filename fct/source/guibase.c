/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guibase.c
* ժ    Ҫ��  ʵ��GUI�����������͵Ĳ���������Ϊ����ģ���ṩ֧��
*
* ��ǰ�汾��  v1.0.1 (���ǵ���ƽ̨�������˶���GUI_OS_ENV��Ԥ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-11
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  yexin.zhu
* ������ڣ�  2012-7-24
*******************************************************************************/

#include "guibase.h"


/***
  * ���ܣ�
        ת��C�ַ���ΪGUI�ַ���
  * ������
        1.char *pString:    Ҫת����C�ַ���
  * ���أ�
        �ɹ�������Чָ�룬���򷵻�NULL
  * ��ע��
***/
GUICHAR* TransString(char *pString)
{
    //�����־������ֵ����
    GUICHAR *pReturn = NULL;
    //��ʱ��������
    char *pTmp = NULL;
    int iSize;

    //�������
    if (NULL == pString)
    {
        return NULL;
    }

    //ͳ���ַ������ȣ�����'\0'
    for (pTmp = pString, iSize = 1; *pTmp; pTmp++)
    {
        iSize++;
    }

    //����Ϊ�ַ������ݷ����ڴ�
    pReturn = (GUICHAR *) malloc(iSize * sizeof(GUICHAR));
    if (NULL == pReturn)
    {
        return NULL;
    }

    //����ַ�����ת��
    while (--iSize >= 0)
    {
        pReturn[iSize] = pString[iSize];
    }

    return pReturn;
}


/***
  * ���ܣ�
        �ַ���ճ�������������ڱ�׼C���strcat()
  * ������
        1.GUICHAR **ppDest: ָ��Ŀ���ַ���������Ϊ��
        2.GUICHAR *pSrc:    Դ�ַ���������Ϊ��
  * ���أ�
        �ɹ�������Чָ�룬���򷵻�NULL
  * ��ע��
***/
GUICHAR* StringCat(GUICHAR **ppDest, GUICHAR *pSrc)
{
    //��ʱ��������
    GUICHAR *pTmp = NULL;
    int iDest, iSrc;

    //�������
    if (NULL == ppDest || NULL == pSrc)
    {
        return NULL;
    }

    //�ж�ppDest��ָ����Ƿ�Ϊ��Чָ��
    if (NULL == *ppDest)
    {
        return NULL;
    }

    //ͳ��Ŀ���ַ�����Դ�ַ������ȣ�������'\0'
    for (pTmp = *ppDest, iDest = 0; *pTmp; pTmp++)
    {
        iDest++;
    }
    for (pTmp = pSrc, iSrc = 0; *pTmp; pTmp++)
    {
        iSrc++;
    }

    //���ǵ�ԴΪ���ַ��������
    if (iSrc == 0)
    {
        return *ppDest;
    }

    //����Ŀ���ַ����洢�ռ�
    pTmp = (GUICHAR *) malloc((iDest + iSrc + 1) * sizeof(GUICHAR));
    if (NULL == pTmp)
    {
        return NULL;
    }

    //����ַ���ճ��
    if (iDest)
    {
        memcpy(pTmp, *ppDest, iDest * sizeof(GUICHAR));
    }
    memcpy(pTmp + iDest, pSrc, (iSrc + 1) * sizeof(GUICHAR));

    //�ͷžɵ�Ŀ���ַ������ݣ������¶���Ŀ���ַ���
    free(*ppDest);
    *ppDest = pTmp;

    return *ppDest;
}


/***
  * ���ܣ�
        �ַ������ƣ����������ڱ�׼C���strcpy()
  * ������
        1.GUICHAR **ppDest: ָ��Ŀ���ַ���������Ϊ��
        2.GUICHAR *pSrc:    Դ�ַ���������Ϊ��
  * ���أ�
        �ɹ�������Чָ�룬���򷵻�NULL
  * ��ע��
***/
GUICHAR* StringCpy(GUICHAR **ppDest, GUICHAR *pSrc)
{
    //��ʱ��������
    GUICHAR *pTmp = NULL;
    int iSize;

    //�������
    if (NULL == ppDest || NULL == pSrc)
    {
        return NULL;
    }

    //�ж�ppDest��ָ����Ƿ�Ϊ��Чָ��
    if (NULL == *ppDest)
    {
        return NULL;
    }

    //ͳ��Դ�ַ������ȣ�����'\0'
    for (pTmp = pSrc, iSize = 1; *pTmp; pTmp++)
    {
        iSize++;
    }

    //����Ŀ���ַ����洢�ռ�
    pTmp = (GUICHAR *) malloc(iSize * sizeof(GUICHAR));
    if (NULL == pTmp)
    {
        return NULL;
    }

    //����ַ�������
    memcpy(pTmp, pSrc, iSize * sizeof(GUICHAR));

    //�ͷžɵ�Ŀ���ַ������ݣ������¶���Ŀ���ַ���
    free(*ppDest);
    *ppDest = pTmp;

    return *ppDest;
}


/***
  * ���ܣ�
        �ַ����Ƚϣ����������ڱ�׼C���strcmp()
  * ������
        1.GUICHAR *pS1: Ҫ�Ƚϵ��ַ���1������Ϊ��
        2.GUICHAR *pS2: Ҫ�Ƚϵ��ַ���2������Ϊ��
  * ���أ�
        pS1<pS2�����ظ�����pS1>pS2��������������ȣ�������
  * ��ע��
***/
int StringCmp(GUICHAR *pS1, GUICHAR *pS2)
{
    //�����־������ֵ����
    int iCmp = 0;
    //��ʱ��������
    GUICHAR *a = NULL, *b = NULL;

    //�ж�pS1��pS2�Ƿ�Ϊ��Чָ��
    if (NULL == pS1 || NULL == pS2)
    {
        return 0;
    }

    //�Ƚ��ַ���
    for (a = pS1, b = pS2; !(iCmp = *a - *b); a++, b++)
    {
        if (*a == 0 || *b == 0) //�ַ�����'\0'��β
        {
            break;
        }
    }

    return iCmp;
}


/***
  * ���ܣ�
        �����µ��߳�
  * ������
        1.GUITHREAD *pThread:   ָ�򴴽������̵߳��߳�ID
        2.THREADFUNC fnEntry:   �������߳�ʱָ�����߳���ں���
        3.void *pArg:           �������߳�ʱ���ݵĲ���
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ThreadCreate(GUITHREAD *pThread, THREADFUNC fnEntry, void *pArg)
{
    //�����־������ֵ����
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_create()
    iReturn = pthread_create(pThread, NULL, fnEntry, pArg);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iReturn;
}


/***
  * ���ܣ�
        �˳���ǰ�߳�
  * ������
        1.void *pReturn:    �߳̽���״̬
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ThreadExit(void *pReturn)
{
  #if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_exit()
    pthread_exit(pReturn);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return 0;
}


/***
  * ���ܣ�
        �ȴ��߳̽���
  * ������
        1.GUITHREAD Thread:     ָ����Ҫ�ȴ����̵߳��߳�ID
        2.void **ppReturn:      ָ���߳̽���״̬����������pthread_exit()
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int ThreadJoin(GUITHREAD Thread, void **ppReturn)
{
    //�����־������ֵ����
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_join()
    iReturn = pthread_join(Thread, ppReturn);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iReturn;
}


/***
  * ���ܣ�
        ��ʼ����
  * ������
        1.GUIMUTEX *pMutex:   ָ�룬ָ����Ҫ��ʼ���Ļ�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int InitMutex(GUIMUTEX *pMutex)
{
    //�����־������ֵ����
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_mutex_init()
    iReturn = pthread_mutex_init(pMutex, NULL);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iReturn;
}


/***
  * ���ܣ�
        ��������
  * ������
        1.GUIMUTEX *pMutex:   ָ�룬ָ����Ҫ�����ϵĻ�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int MutexLock(GUIMUTEX *pMutex)
{
    //�����־������ֵ����
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_mutex_lock()
    iReturn = pthread_mutex_lock(pMutex);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iReturn;
}


/***
  * ���ܣ�
        ���Լ���
  * ������
        1.GUIMUTEX *pMutex:   ָ�룬ָ����Ҫ���Լ����Ļ�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int MutexTrylock(GUIMUTEX *pMutex)
{
    //�����־������ֵ����
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_mutex_trylock()
    iReturn = pthread_mutex_trylock(pMutex);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iReturn;
}


/***
  * ���ܣ�
        ��������
  * ������
        1.GUIMUTEX *pMutex:   ָ�룬ָ����Ҫ���Խ����Ļ�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int MutexUnlock(GUIMUTEX *pMutex)
{
    //�����־������ֵ����
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_mutex_unlock()
    iReturn = pthread_mutex_unlock(pMutex);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iReturn;
}


/***
  * ���ܣ�
        �������ɺ���
  * ������
        1.unsigned long ulMsec:   ��Ҫ˯�ߵĺ�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int MsecSleep(unsigned long ulMsec)
{
    //�����־������ֵ����
    int iReturn = 0;

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:timeval,select()
    struct timeval tv;

    tv.tv_sec = ulMsec / 1000;
    tv.tv_usec = ulMsec % 1000 * 1000;
    iReturn = select(0, NULL, NULL, NULL, &tv);
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iReturn;
}


/***
  * ���ܣ�
        �½�һ���б����б��������iLimit����Ŀ
  * ������
        1.int iLimit:  Ҫ�������б�����������ɵ���Ŀ����iLimit����С��1
  * ���أ�
        �ɹ�����ָ�����б����Чָ�룬���򷵻�NULL
  * ��ע��
        ʹ��CreateList�����½����б��������Ϊ�ա�
***/
GUILIST* CreateList(int iLimit)
{
    //�����־������ֵ����
    GUILIST *pList = NULL;

    //�������
    if (iLimit < 0)
    {
        return NULL;    
    }

    //������Դ
    pList = (GUILIST *) malloc(sizeof(GUILIST));
    if (NULL == pList)
    {
        return NULL;    
    }

    //��ʼ���б�
    pList->iCount = 0;
    pList->iLimit = iLimit;
    pList->pHead = NULL;
    pList->pTail = NULL;

    return pList;
}


/***
  * ���ܣ�
        ɾ���б�pList�����ͷ�pListռ�õ�������Դ���ɹ�������
  * ������
        1.GUILIST *pList:   ָ�룬ָ����CreateList�������б����
  * ���أ�
        �ɹ������㣬���򷵻ش������
  * ��ע��
        DestroyList��֤�������pListΪ�ǿ�ָ�룬����б�pListΪ�գ�
        DestroyList�᷵�ش������-2��
***/
int DestroyList(GUILIST *pList)
{
    //�ж�pList�Ƿ�Ϊ��Чָ��
    if (NULL == pList)
    {
        return -1;      
    }

    //�ж�pList�Ƿ�Ϊ�գ����ǿ�������б����ɾ���ɳ���Ա�Լ�����
    if (!ListEmpty(pList))
    {
        return -2;
    }

    //�����б����
    free(pList);

    return 0;
}


/***
  * ���ܣ�
        ��ʼ���б�pList�� �ɹ������㣬���򷵻ط���ֵ
  * ������
        1.GUILIST *pList:   ָ�룬ָ����CreateList�������б����
  * ���أ�
        �ɹ������㣬���򷵻ش������
  * ��ע��
        ��ʼ��pList��iCount = 0��pHead = pTail = NULL
***/
inline int InitList(GUILIST *pList)
{
    //�ж�pList�Ƿ�Ϊ��Чָ��
    if (NULL == pList)
    {
        return -1;
    }

    //��ʼ���б�
    pList->iCount = 0;
    pList->pHead = NULL;
    pList->pTail = NULL;

    return 0;
}


/***
  * ���ܣ�
        �����б���pItem���б�pListβ��
  * ������
        1.GUILIST_ITEM *pItem:  ָ�룬ָ����Ҫ������б���
        2.GUILIST *pList:       ָ�룬ָ����CreateList�������б����
  * ���أ�
        �ɹ������㣬���򷵻ش������
  * ��ע��
        ����ListAddҪ��pList��ָ����б�δ���������ǽ�����Ŀ���뵽
        �б�pList��β�ˣ�����б���������������
***/
int ListAdd(GUILIST_ITEM *pItem, GUILIST *pList)
{
    //�ж�pItem��pList�Ƿ�Ϊ��Чָ��
    if (NULL == pList || NULL == pItem)
    {
        return -1;
    }

    // �ж�pList�Ƿ�Ϊ��
    if (ListFull(pList))
    {
        return -2;
    }

    //���pItem��pListβ��
    if (ListEmpty(pList))
    {
        //�б�pListΪ��ʱ
        pItem->pPrev = pItem;
        pItem->pNext = pItem;
        pList->pHead = pItem;
        pList->pTail = pItem;
    }
    else
    {
        //�б�pList��Ϊ��ʱ
        pItem->pPrev = pList->pTail;
        pItem->pNext = pList->pHead;
        pList->pTail->pNext = pItem;
        pList->pHead->pPrev = pItem;
        pList->pTail = pItem;
    }
    pItem->pOwner = pList;
    pList->iCount++;

    return 0;
}


/***
  * ���ܣ�
        ���б�pList��ɾ���б���pItem
  * ������
        1.GUIITEM *pItem:   ָ�룬ָ����Ҫɾ�����б���
        2.GUILIST *pList:   ָ�룬ָ����CreateList�������б����
  * ���أ�
        �ɹ������㣬���򷵻ش������
  * ��ע��
        ����ListAddҪ��pList��ָ����б�δ�գ�����б��ѿգ���������
***/
int ListDel(GUILIST_ITEM *pItem, GUILIST *pList)
{
    //�ж�pItem��pList�Ƿ�Ϊ��Чָ��
    if (NULL == pList || NULL == pItem)
    {
        return -1;
    }

    //�ж�pItem�Ƿ���pList�к�pList�Ƿ�Ϊ��
    if ((pItem->pOwner != pList) || 
        ListEmpty(pList))
    {
        return -2;
    }

    //ɾ��pList��pItem
    if (pList->iCount == 1)
    {
        //�б���ֻ��һ���б���
        pList->pHead = NULL;
        pList->pTail = NULL;
    }
    else
    {
        //�б����ж���б���
        pItem->pNext->pPrev = pItem->pPrev;
        pItem->pPrev->pNext = pItem->pNext;
        if (pItem == pList->pHead)
        {
            pList->pHead = pItem->pNext;
        }
        else if (pItem == pList->pTail)
        {
            pList->pTail = pItem->pPrev;
        }
    }

    //ɾ������pItem����ԱΪNULL
    pItem->pOwner = NULL;
    pItem->pNext = NULL;
    pItem->pPrev = NULL;
    pList->iCount--;

    return 0;
}


/***
  * ���ܣ�
        ��һ���б����б���pItem�Ƶ���һ���б�β��
  * ������
        1.GUIITEM *pItem:   ָ�룬ָ����Ҫ�ƶ����б���
        2.GUILIST *pList:   ָ�룬ָ����CreateList�������б����
  * ���أ�
        �ɹ������㣬���򷵻ش������
  * ��ע��
***/
int ListMove(GUILIST_ITEM *pItem, GUILIST *pList)
{
    //�ж�pItem��pList�Ƿ�Ϊ��Чָ��
    if (NULL == pList || NULL == pItem)
    {
        return -1;
    }

    //�ж�pList�Ƿ�Ϊ��
    if (ListFull(pList))
    {
        return -2;
    }

    //��pItem->pOwnerɾ���б���pItem
    if (ListDel(pItem, pItem->pOwner))
    {
        return -3;
    }

    //���pItem��pListβ��
    ListAdd(pItem, pList);

    return 0;
}


/***
  * ���ܣ�
        �ϲ��б�pAttach����һ�б�pList��β�˲���ʼ��ԭ�б�  
  * ������
        1.GUILIST *pAttach: ָ�룬ָ����Ҫ�ϲ����б����
        2.GUILIST *pList:   ָ�룬ָ����CreateList�������б����
  * ���أ�
        �ѿշ���0��ʧ�ܷ��ط���
  * ��ע��
        �豣֤pAttach��Ϊ�գ���Ϊ���򷵻ش������-2��
        ��pList�ϲ�pAttach����Ҫδ��,��pAttach->iCount����pList->iCountҪ����
        ��pList->iLimit��
***/
int ListSplice(GUILIST *pAttach, GUILIST *pList)
{
    //��ʱ��������
    int iCnt;
    GUILIST_ITEM *pTmp = NULL;

    //�ж�pAttach��pList�Ƿ�Ϊ��Чָ��
    if (NULL == pList || NULL == pAttach)
    {
        return -1;
    }

    //�ж�pAttach�Ƿ�Ϊ�պ�pList�Ƿ�δ��
    if (ListEmpty(pAttach) || 
        ListFull(pList))
    {
        return -2;
    }

    //�жϺϲ���pList���ȴ���iLimit
    if (pList->iCount + pAttach->iCount > pList->iLimit)
    {
        return -3;
    }

    //�ϲ�pAttach��pListβ�˲�����iCount
    pAttach->pHead->pPrev = pList->pTail;
    pList->pTail->pNext = pAttach->pHead;
    pAttach->pTail->pNext = pList->pHead;
    pList->pHead->pPrev = pAttach->pTail;
    pList->pTail = pAttach->pTail;
    pList->iCount +=  pAttach->iCount;

    iCnt = pAttach->iCount;
    pTmp = pAttach->pHead;

    //����pAttach�ı����б����pOwner
    while (iCnt--)
    {
        pTmp->pOwner = pList;
        pTmp = pTmp->pNext;
    }

    //��ʼ��pAttach
    InitList(pAttach);

    return 0;
}


/***
  * ���ܣ�
        ���pList�Ƿ�Ϊ�գ��ѿշ���1���ǿշ���0
  * ������
        1.CGList *pList:    ָ�룬ָ����GreateList�������б����
  * ���أ�
        �ѿշ���1���ǿշ���0
  * ��ע��
***/
inline int ListEmpty(GUILIST *pList)
{
    return !(pList->iCount);
}


/***
  * ���ܣ�
        ���pList�Ƿ�Ϊ������������1��δ������0
  * ������
        1.CGList *pList:    ָ�룬ָ����GreateList�������б����
  * ���أ�
        ��������1��δ������0
  * ��ע��
***/
inline int ListFull(GUILIST *pList)
{
    return pList->iCount >= pList->iLimit;
}

