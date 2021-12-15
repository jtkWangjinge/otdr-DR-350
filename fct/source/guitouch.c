/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guitouch.c
* ժ    Ҫ��  GUIFBMAPģ����Ҫ����豸����㶨�弰ʵ�֣��Ա�GUI�ܹ������ڲ�ͬӲ
*             ��ƽ̨��Ӧ�ò����Ӳ�����졣guitouch.cʵ����guitouch.h�������Ĵ�
*             �����豸����ز���������ʵ��GUI�Ĵ����������豸����
*
* ��ǰ�汾��  v1.0.1 (���ǵ���ƽ̨�������˶���GUI_OS_ENV��Ԥ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-12
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  yexin.zhu
* ������ڣ�  2012-8-2
*******************************************************************************/

#include "guitouch.h"


/*************************************
* Ϊʵ��GUITOUCH����Ҫ���õ�����ͷ�ļ�
*************************************/
#include "guischeduler.h"
#include "guievent.h"


/**************************
* ����GUI�еĵ�ǰ����������
**************************/
static GUITOUCH *pCurrTouch = NULL;


/***
  * ���ܣ�
        ����ָ������Ϣ��������������
  * ������
        1.char *strDevName:         ��Ҫ����������������豸������
        2.int iFbType:              �����������豸���ͣ�1��׼��2�Զ���
        3.THREADFUNC fnTouchThread: �ص����������ڴ���֡������߳�
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUITOUCH* CreateTouch(char *strDevName, int iDevType, 
                      THREADFUNC fnTouchThread)
{
    //�����־������ֵ����
    int iErr = 0;
    GUITOUCH *pTouchObj = NULL;

    if (iErr == 0)
    {
        //�ж�strDevName, fnTouchThread�Ƿ�Ϊ��Чָ��
        if (NULL == strDevName || NULL == fnTouchThread)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pTouchObj = (GUITOUCH *) malloc(sizeof(GUITOUCH));
        if (NULL == pTouchObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:open()
    if (iErr == 0)
    {
        //���Դ��豸�ļ�
        pTouchObj->iDevFd = open(strDevName, O_RDWR);
        if (pTouchObj->iDevFd == -1)
        {
            iErr = -3;
        }
        //�����½������������iDevType
        pTouchObj->iDevType = iDevType;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //�����½������������fnTouchThread
        pTouchObj->fnTouchThread = fnTouchThread;
    }

    //������
    switch (iErr)
    {
    case -3:
        free(pTouchObj);
    case -2:
    case -1:
        pTouchObj = NULL;
    default:
        break;
    }

    return pTouchObj;
}


/***
  * ���ܣ�
        ɾ������������
  * ������
        1.GUITOUCH **ppTouchObj:    ָ���ָ�룬ָ����Ҫ���ٵĴ���������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyTouch(GUITOUCH **ppTouchObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppTouchObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppTouchObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppTouchObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppTouchObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:close()
    if (iErr == 0)
    {
        //�ͷ���Դ
        if (close((*ppTouchObj)->iDevFd))   //�豸�ļ�Ϊ������Դ������֤����ֵ
        {
            iErr = -3;
        }
        //���ٴ��������󣬲���ָ���ÿ�
        free(*ppTouchObj);
        *ppTouchObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iErr;
}


/***
  * ���ܣ�
        ��ȡ��ǰ����������
  * ������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
GUITOUCH* GetCurrTouch(void)
{
    return pCurrTouch;
}


/***
  * ���ܣ�
        ���õ�ǰ����������
  * ������
        1.GUITOUCH *pTouchObj:  ������Ϊ��ǰ�������豸����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetCurrTouch(GUITOUCH *pTouchObj)
{
    //�ж�pTouchObj�Ƿ���Ч
    if (NULL == pTouchObj)
    {
        return -1;
    }

    pCurrTouch = pTouchObj;

    return 0;
}


/***
  * ���ܣ�
        Ĭ�ϴ������߳���ں���
  * ������
        1.void *pThreadArg:     �߳���ں�������������Ϊ(GUITOUCH *)
  * ���أ�
        �����߳̽���״̬
  * ��ע��
***/
void* DefaultTouchThread(void *pThreadArg)
{
    //�����־������ֵ����
    static int iReturn = 0;
    //��ʱ��������
    int iExit = 0;
    GUITOUCH *pTouchObj = pThreadArg;

    while (1)
    {
        //���ݽ�����־�ж��Ƿ���Ҫ�����������߳�
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        //�������¼��ķ�װ
        CustomTouchFunc(pTouchObj);
        //˯��50����
        MsecSleep(50);
    }

    //�˳��߳�
    ThreadExit(&iReturn);
    return &iReturn;    //�����ֻ��Ϊ������������������ʵ���ϲ�������
}

#if 0
/***
  * ���ܣ�
        �Զ���Ĵ�������������������ɴ������̵߳Ĵ���
  * ������
        1.GUIKEYPAD *pTouchObj:    �������ļ��̶���
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int CustomTouchFunc(GUITOUCH *pTouchObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    static int iPrevX = -1, iPrevY = -1; 
    struct input_event Event[5];
    int iLen, iTouchAct, iAbsX, iAbsY, iTmp;
    GUIEVENT_ITEM *pEventItem = NULL;
    GUIEVENT *pEventObj = NULL;

    if (iErr == 0)
    {
        //��ȡ���������ص�����
        iLen = read(pTouchObj->iDevFd, Event, sizeof(Event));
        if (iLen < sizeof(struct input_event))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //��ʼ������������־������ֵ��־
        iTouchAct = -1;
        iAbsX = -1;
        iAbsY = -1;
        //�Ӷ�ȡ��������Ѱ�Ҵ�������������ֵ
        iLen /= sizeof(struct input_event);
        for (iTmp = 0; iTmp < iLen; iTmp++)
        {
            //�ҵ������������޸Ĵ���������־
            if (Event[iTmp].type == EV_KEY)         //����Ϊ����
            {
                if (Event[iTmp].code == BTN_TOUCH)  //����Ϊ��������
                {
                    iTouchAct = Event[iTmp].value;  //�õ���������
                }
            }
            //�ҵ�����ֵ���޸�����ֵ��־
            if (Event[iTmp].type == EV_REL)         //����Ϊ����ֵ
            {
                if (Event[iTmp].code == REL_X)      //����Ϊ������
                {
                    iAbsX = Event[iTmp].value;      //�õ�������ֵ
                }
                if (Event[iTmp].code == REL_Y)      //����Ϊ������
                {
                    iAbsY = Event[iTmp].value;      //�õ�������ֵ
                }
            }
            //�õ������������ֱ�Ӻ���(������������ɺ�������ֵ������)
            if (iTouchAct == 1)
            {
                iTouchAct = -1;                     //���ô���������־
            }
            //�õ��뿪���������������ֵ�������¼���
            if ((iTouchAct == 0) || 
                (iAbsX >= 0 && iAbsY >= 0))
            {
                //����Ϊ�������¼�������ڴ�
                pEventItem = (GUIEVENT_ITEM *) malloc(sizeof(GUIEVENT_ITEM));
                if (NULL == pEventItem)
                {
                    iErr = -2;
                    break;
                }
                //����GUIEVENT_ITEM
                pEventItem->uiEventType = GUIEVENT_TYP_IN;
                if (iTouchAct == 0)                 //�뿪������
                {
                    pEventItem->uiEventCode = GUIEVENT_ABS_UP;
                    pEventItem->uiEventValue = (iPrevX << 16) | iPrevY;
                    iTouchAct = -1;                 //���ô���������־
                }
                if (iAbsX >= 0 && iAbsY >= 0)       //��������ֵ
                {
                    pEventItem->uiEventCode = GUIEVENT_ABS_DOWN;
                    pEventItem->uiEventValue = (iAbsX << 16) | iAbsY;
                    iPrevX = iAbsX;                 //���������
                    iPrevY = iAbsY;                 //����������
                    iAbsX = -1;                     //���ú������־
                    iAbsY = -1;                     //�����������־
                }
                //�¼���д���¼�����
                pEventObj = GetCurrEvent();
                MutexLock(&(pEventObj->Mutex));
                if (WriteEventQueue(pEventItem, pEventObj))
                {
                    iErr = -3;
                    break;
                }
                MutexUnlock(&(pEventObj->Mutex));
            }
        }
    }

    //������
    switch (iErr)
    {
    case -3:
        MutexUnlock(&(pEventObj->Mutex));
        free(pEventItem);
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}
#endif


/***
  * ���ܣ�
        �û��Զ���Ĵ���������������������Զ������ʹ������Ĵ���
  * ������
        1.GUIKEYPAD *pTouchObj:    �������ļ��̶���
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        1.���뷵��-1��ʾ�޴��������ݿɶ�
        2.�����ʵ������޸Ĵ��� 
***/
int CustomTouchFunc(GUITOUCH *pTouchObj)
{
	//�����־������ֵ����
	int iErr = 0;
	//��ʱ��������
	int iTmp;
	GUIEVENT_ITEM *pEventItem = NULL;
	GUIEVENT *pEventObj = NULL;
	
	unsigned iSync = 0;
	unsigned uiEventCode = 0;
	static unsigned uiEventValue = 0;
	long lCurtime = 0;
	static long lLasttime = 0;					
	UINT32 uiCursX = 0;
	UINT32 uiCursY = 0;

	
			
#if (GUI_OS_ENV == LINUX_OS)  //OS���:input_event
	  struct input_event event;
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif    //GUI_OS_ENV == LINUX_OS
	
	if (!iErr)
	{
		//����Ϊ�������¼�������ڴ�
		//���������޸ı��δ���
		pEventItem = (GUIEVENT_ITEM *) malloc(sizeof(GUIEVENT_ITEM));
		if (NULL == pEventItem)
		{
			iErr = -2;
		}
	}

#if (GUI_OS_ENV == LINUX_OS)  //OS���:event , read
	if (!iErr)
	{
		//����GUIEVENT_ITEM
		//�˶θ���ʵ����������Ӧ����
		while ((!iSync) && (!iErr))
		{
			pthread_testcancel();
			iTmp = read(pTouchObj->iDevFd, &event, sizeof(event));
			pthread_testcancel();
			if (iTmp != sizeof(event))
			{
				MsecSleep(50);
				continue;
			}

			switch (event.type)
			{
			case EV_KEY:
				if (BTN_TOUCH == event.code)
				{
					uiEventCode = event.value ? 
						GUIEVENT_ABS_DOWN : GUIEVENT_ABS_UP;
				}
				else
				{
					iErr = -3;
				}
				break;
			case EV_REL:
				switch (event.code)
				{
				case REL_X: //ʵ�ʰ�ABS_X����
					uiCursX = (unsigned int)event.value;
					break;
				case REL_Y: //ʵ�ʰ�ABS_Y����
					uiCursY = (unsigned int)event.value;
					break;
				default:
					iErr = -3;
					break;
				}
				break;
			case EV_ABS:
				switch (event.code)
				{
				case ABS_MT_POSITION_X:
					uiCursX = (unsigned int)event.value;
					break;
				case ABS_MT_POSITION_Y:
					uiCursY = (unsigned int)event.value;
					break;
				}
				break;
			case EV_SYN:
				//��ȡͬ��ʱ��
				switch (uiEventCode)
				{
				case GUIEVENT_ABS_DOWN:
					//���水��ʱ�̵�����ֵ
					uiEventValue = (uiCursX << 16) |
								   (uiCursY);
					iSync = 1;
					break;
				case GUIEVENT_ABS_UP:
					//�ָ�����ʱ�̵�����ֵ
					uiCursX = (uiEventValue >> 16) & 0x0000FFFF;
					uiCursY = uiEventValue & 0x0000FFFF;
					iSync = 1;
					break;
				default:
					iErr = -3;
					break;
				}
				break;
			default:
				iErr = -3;
				break;
			}
		}
	}
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif    //GUI_OS_ENV == LINUX_OS

	if (!iErr)
	{
		//�����¼�����汾���¼���ʱ���
		lLasttime = lCurtime;
		pEventItem->uiEventType = GUIEVENT_TYP_IN;
		pEventItem->uiEventCode = uiEventCode;
		pEventItem->uiEventValue = (uiCursX << 16) |
								   (uiCursY);

	}

	if (!iErr)
	{
		//�¼���д���¼�����
		//���������޸ı��δ���
		pEventObj = GetCurrEvent();
		if (WriteEventQueue(pEventItem, pEventObj))
		{
			iErr = -4;
		}
	}

	//������
	//�˶θ���ʵ����������Ӧ����
	switch (iErr)
	{
	case -4:
	case -3:
		free(pEventItem);
		//no break
	case -2:
	case -1:
	default:
		break;
	}

	return iErr;
}

