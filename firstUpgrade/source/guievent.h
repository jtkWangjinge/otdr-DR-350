/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guievent.h
* ժ    Ҫ��  GUIEVENT����ϵͳ����ģ�飬�˲��ֶ���GUI���¼����ͼ�������ز�����
*             ����ʵ���¼��Ķ���ͷ�װ��
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  yexin.zhu
* ������ڣ�  2012-8-6
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUI_EVENT_H
#define _GUI_EVENT_H


/*************************************
* Ϊ����GUIEVENT����Ҫ���õ�����ͷ�ļ�
*************************************/
#include "guibase.h"


/******************************************
* ����GUI���¼��������ܰ����¼�����������
******************************************/
#define GUIEVENT_ITEM_LIMIT     1000    //�����¼��������ɰ������¼���


/**************
* ����GUI���¼�����
**************/
#define GUIEVENT_TYP_IN         1   //�����¼�


/******************
* ����GUI���¼�����
******************/
#define GUIEVENT_KEY_DOWN       1   //�¼�����������
#define GUIEVENT_KEY_UP         2   //�¼��������ɿ�
#define GUIEVENT_ABS_DOWN       3   //�¼������������
#define GUIEVENT_ABS_UP         4   //�¼����뿪������
#define GUIEVENT_KNB_CLOCK      5   //�¼�����ť˳ʱ����ת
#define GUIEVENT_KNB_ANTI       6   //�¼�����ť��ʱ����ת


/****************
* ����GUI���¼���
****************/
typedef struct _event_item
{
    unsigned int uiEventType;   //�¼����ͣ�1�����¼�
    unsigned int uiEventCode;   //�¼����룬KEY_DOWN��KEY_UP��
    unsigned int uiEventValue;  //�¼���ֵ�����������������ֵ
} GUIEVENT_ITEM;


/******************
* ����GUI���¼��ṹ
******************/
typedef struct _event
{
    GUILIST *pEventQueue;       //�����¼����У��������������䱣���׼�����¼�

    THREADFUNC fnEventThread;   //�̺߳��������ڴ����¼���װ�߳�
    GUIMUTEX Mutex;             //�����������ڿ��ƶ������¼����еķ���
} GUIEVENT;


/**********************************
* ����GUI�����¼�������صĲ�������
**********************************/
//����ָ������Ϣ�����¼�����
GUIEVENT* CreateEvent(int iQueueLimit, THREADFUNC fnEventThread);
//ɾ���¼�����
int DestroyEvent(GUIEVENT **ppEventObj);

//�õ���ǰ���¼�����
GUIEVENT* GetCurrEvent(void);
//���õ�ǰ���¼�����
int SetCurrEvent(GUIEVENT *pEventObj);

//����ָ���¼������е��¼���
GUIEVENT_ITEM* ReadEventQueue(GUIEVENT *pEventObj);
//д���¼��ָ�����¼�����
int WriteEventQueue(GUIEVENT_ITEM *pEventItem, GUIEVENT *pEventObj);
//����¼�����
int ClearEventQueue(GUIEVENT *pEventObj);


/***********************
* ����GUI�е�Ĭ���¼���װ�߳�
***********************/
//�¼���װ�߳���ں���
void* EventPacketThread(void *pThreadArg);

//�¼���װ��������������¼���װ�̵߳Ĵ���
int EventPacketFunc(GUIEVENT *pEventObj);

//��GUI���������ƥ���¼��������¼���װ������ȷ����Ϣ���ն���
void* MatchEventObject(GUIEVENT_ITEM *pEventItem);


#endif  //_GUI_EVENT_H

