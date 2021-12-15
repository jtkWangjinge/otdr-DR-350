/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guikeypad.c
* ժ    Ҫ��  GUIKEYPADģ����Ҫ����豸����㶨�弰ʵ�֣��Ա�GUI�ܹ������ڲ�ͬ
*             Ӳ��ƽ̨��Ӧ�ò����Ӳ�����졣guikeypad.cʵ����guikeypad.h������
*             �ļ����豸����ز���������ʵ��GUI��ͼ������豸����
*
* ��ǰ�汾��  v1.0.1 (���ǵ���ƽ̨�������˶���GUI_OS_ENV��Ԥ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-12
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  yexin.zhu
* ������ڣ�  2012-8-1
*******************************************************************************/

#include "guikeypad.h"


/**************************************
* Ϊʵ��GUIKEYPAD����Ҫ���õ�����ͷ�ļ�
**************************************/
#include "guischeduler.h"
#include "guievent.h"
#include "guiglobal.h"


/************************
* ����GUI�еĵ�ǰ���̶���
************************/
static GUIKEYPAD *pCurrKeypad = NULL;


/***
  * ���ܣ�
        ����ָ������Ϣ�������̶���
  * ������
        1.char *strDevName:             ��Ҫ�������̶�����豸������
        2.int iFbType:                  ���̻����豸���ͣ�1��׼��2�Զ���
        3.THREADFUNC fnKeypadThread:    �ص����������ڴ���֡������߳�
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIKEYPAD* CreateKeypad(char *strDevName, int iDevType, 
                        THREADFUNC fnKeypadThread)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIKEYPAD *pKeypadObj = NULL;

    if (iErr == 0)
    {
        //�ж�strDevName, fnKeypadThread�Ƿ�Ϊ��Чָ��
        if (NULL == strDevName || NULL == fnKeypadThread)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pKeypadObj = (GUIKEYPAD *) malloc(sizeof(GUIKEYPAD));
        if (NULL == pKeypadObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:open()
    if (iErr == 0)
    {
        //���Դ��豸�ļ�
        pKeypadObj->iDevFd = open(strDevName, O_RDWR);
        if (pKeypadObj->iDevFd == -1)
        {
            iErr = -3;
        }
        //�����½����̶����iDevType
        pKeypadObj->iDevType = iDevType;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //�����½����̶����fnKeypadThread
        pKeypadObj->fnKeypadThread = fnKeypadThread;
    }

    //������
    switch (iErr)
    {
    case -3:
        free(pKeypadObj);
    case -2:
    case -1:
        pKeypadObj = NULL;
    default:
        break;
    }

    return pKeypadObj;
}


/***
  * ���ܣ�
        ɾ�����̶���
  * ������
        1.GUIKEYPAD **ppKeypadObj:  ָ���ָ�룬ָ����Ҫ���ٵļ��̶���
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyKeypad(GUIKEYPAD **ppKeypadObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppKeypadObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppKeypadObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppKeypadObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppKeypadObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:close()
    if (iErr == 0)
    {
        //�ͷ���Դ
        if (close((*ppKeypadObj)->iDevFd))  //�豸�ļ�Ϊ������Դ������֤����ֵ
        {
            iErr = -3;
        }
        //���ټ��̶��󣬲���ָ���ÿ�
        free(*ppKeypadObj);
        *ppKeypadObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iErr;
}


/***
  * ���ܣ�
        ��ȡ��ǰ���̶���
  * ������
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIKEYPAD* GetCurrKeypad(void)
{
    return pCurrKeypad;
}


/***
  * ���ܣ�
        ���õ�ǰ���̶���
  * ������
        1.GUIKEYPAD *pKeypadObj:    ������Ϊ��ǰ�����豸����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetCurrKeypad(GUIKEYPAD *pKeypadObj)
{
    //�ж�pKeypadObj�Ƿ���Ч
    if (NULL == pKeypadObj)
    {
        return -1;
    }

    pCurrKeypad = pKeypadObj;

    return 0;
}


/***
  * ���ܣ�
        Ĭ�ϼ����߳���ں���
  * ������
        1.void *pThreadArg:     �߳���ں�������������Ϊ(GUIKEYPAD *)
  * ���أ�
        �����߳̽���״̬
  * ��ע��
***/
void* DefaultKeypadThread(void *pThreadArg)
{
    //�����־������ֵ����
    static int iReturn = 0;
    //��ʱ��������
    int iExit = 0;
    GUIKEYPAD *pKeypadObj = pThreadArg;

    while (1)
    {
        //���ݽ�����־�ж��Ƿ���Ҫ���������߳�
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        //�����¼��ķ�װ
        CustomKeypadFunc(pKeypadObj);
        //˯��50����
        MsecSleep(50);
    }

    //�˳��߳�
    ThreadExit(&iReturn);
    return &iReturn;    //�����ֻ��Ϊ������������������ʵ���ϲ�������
}


/***
  * ���ܣ�
        �Զ���ļ��̴�������������ɼ����̵߳Ĵ���
  * ������
        1.GUIKEYPAD *pKeypadObj:    �������ļ��̶���
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int CustomKeypadFunc(GUIKEYPAD *pKeypadObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    int iLen;
    //unsigned int uiKeyValue;
    GUIEVENT_ITEM *pEventItem = NULL;
    GUIEVENT *pEventObj = NULL;
	struct input_event event;
	
    if (iErr == 0)
    {
        //��ȡ���̷��ص�����
        iLen = read(pKeypadObj->iDevFd, &event, sizeof(event));
        if (iLen != sizeof(event))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //����Ϊ�����¼�������ڴ�
        pEventItem = (GUIEVENT_ITEM *) malloc(sizeof(GUIEVENT_ITEM));
        if (NULL == pEventItem)
        {
            iErr = -2;
        }
    }


	 if (!iErr)
	  {
		  //����GUIEVENT_ITEM
		  switch (event.type)
		  {
		  case EV_KEY:
	            pEventItem->uiEventCode = (!event.value) ?
	                                      GUIEVENT_KEY_UP : GUIEVENT_KEY_DOWN;
	            pEventItem->uiEventValue = event.code;
				//�¼���д���¼�����
				//���������޸ı��δ���
				pEventObj = GetCurrEvent();
				MutexLock(&(pEventObj->Mutex));
				if (WriteEventQueue(pEventItem, pEventObj))
				{
					iErr = -3;
				}
				MutexUnlock(&(pEventObj->Mutex));
			  break;
		  case EV_REL:			  
			  break;
		  default:
			  iErr = -3;
			  break;
		  }
	  }

	 #if 0
    if (iErr == 0)
    {
        //����GUIEVENT_ITEM
        pEventItem->uiEventType = GUIEVENT_TYP_IN;
       if (uiKeyValue & 0x000F0000) //bit19~bit16:0=��ť��1=���� 
	{ 
	pEventItem->uiEventCode = (uiKeyValue & 0x00000F00) ? 
								GUIEVENT_KEY_UP : GUIEVENT_KEY_DOWN; 
	pEventItem->uiEventValue = uiKeyValue & 0x000000FF; 
	} 
	else 
	{ 
	pEventItem->uiEventCode = (uiKeyValue & 0x0F000000) ? 
							GUIEVENT_KNB_ANTI : GUIEVENT_KNB_CLOCK; 
	pEventItem->uiEventValue = uiKeyValue & 0x000000FF; 
	}
        //�¼���д���¼�����
        pEventObj = GetCurrEvent();
        MutexLock(&(pEventObj->Mutex));
        if (WriteEventQueue(pEventItem, pEventObj))
        {
            iErr = -3;
        }
        MutexUnlock(&(pEventObj->Mutex));
    }
	#endif

    //������
    switch (iErr)
    {
    case -3:
        free(pEventItem);
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}

