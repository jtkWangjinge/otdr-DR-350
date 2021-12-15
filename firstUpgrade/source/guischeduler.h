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

#ifndef _GUI_SCHEDULER_H
#define _GUI_SCHEDULER_H


/*****************************************
* Ϊ����GUISCHEDULER����Ҫ���õ�����ͷ�ļ�
*****************************************/
//#include ""


/**********************************
* ����GUI������Ȼ�����صĲ�������
**********************************/
//��ʼ��GUI�����˳���־
int InitExitFlag(void);
//�õ�GUI�����˳���־
int GetExitFlag(void);
//����GUI�����˳���־
void SetExitFlag(int iFlag);

//��������߳���ں���
void* WindowSchedulerThread(void *pThreadArg);
//Ĭ�ϴ��崦���߳���ں���
void* DefaultWindowThread(void *pThreadArg);

//��ʼ�������̻߳�����
int InitThreadLock(void);
//��ס��ǰ�����߳�
int LockWindowThread(void);
//������ǰ�����߳�
int UnlockWindowThread(void);


#endif  //_GUI_SCHEDULER_H

