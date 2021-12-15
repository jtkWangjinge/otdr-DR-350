/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guitouch.h
* ժ    Ҫ��  GUITOUCHģ����Ҫ����豸����㶨�弰ʵ�֣��Ա�GUI�ܹ������ڲ�ͬӲ
*             ��ƽ̨��Ӧ�ò����Ӳ�����졣guitouch.h������GUI�Ĵ�����ӳ�����ͼ�
*             ������ز���������ʵ��GUI�Ĵ����������豸����
*
* ��ǰ�汾��  v1.0.1 (���ǵ���ƽ̨�������˶���GUI_OS_ENV��Ԥ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-12
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  yexin.zhu
* ������ڣ�  2012-8-2
*******************************************************************************/

#ifndef _GUI_TOUCH_H
#define _GUI_TOUCH_H


/*************************************
* Ϊ����GUITOUCH����Ҫ���õ�����ͷ�ļ�
*************************************/
#include "guibase.h"


/********************
* ����GUI�д���������
********************/
#define GUITOUCH_TYP_STD        1   //��׼����������
#define GUITOUCH_TYP_USER       2   //�Զ��崥��������


/********************
* ����GUI�д������ṹ
********************/
typedef struct _touch
{
    int iDevFd;                 //�������豸�ļ�������
    int iDevType;               //�������豸���ͣ�1��ʾ��׼��2��ʾ�Զ���

    THREADFUNC fnTouchThread;   //�ص����������ڴ����������߳�
} GUITOUCH;


/************************************
* ����GUI���봥����������صĲ�������
************************************/
//����ָ������Ϣ��������������
GUITOUCH* CreateTouch(char *strDevName, int iDevType, 
                      THREADFUNC fnTouchThread);
//ɾ������������
int DestroyTouch(GUITOUCH **ppTouchObj);

//�õ���ǰ�Ĵ���������
GUITOUCH* GetCurrTouch(void);
//���õ�ǰ�Ĵ���������
int SetCurrTouch(GUITOUCH *pTouchObj);


/***********************
����GUI�� Ĭ�ϴ������߳�
***********************/
//Ĭ�ϴ������߳���ں���
void* DefaultTouchThread(void *pThreadArg);

//�Զ���Ĵ�������������������ɴ������̵߳Ĵ���
int CustomTouchFunc(GUITOUCH *pTouchObj);


#endif  //_GUI_TOUCH_H

