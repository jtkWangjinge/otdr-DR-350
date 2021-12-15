/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guikeypad.h
* ժ    Ҫ��  GUIKEYPADģ����Ҫ����豸����㶨�弰ʵ�֣��Ա�GUI�ܹ������ڲ�ͬ
*             Ӳ��ƽ̨��Ӧ�ò����Ӳ�����졣guikeypad.h������GUI�ļ���ӳ������
*             ��������ز���������ʵ��GUI��ͼ������豸����
*
* ��ǰ�汾��  v1.0.1 (���ǵ���ƽ̨�������˶���GUI_OS_ENV��Ԥ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-12
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  yexin.zhu
* ������ڣ�  2012-8-1
*******************************************************************************/

#ifndef _GUI_KEYPAD_H
#define _GUI_KEYPAD_H


/**************************************
* Ϊ����GUIKEYPAD����Ҫ���õ�����ͷ�ļ�
**************************************/
#include "guibase.h"


/*************
* �����������
*************/
#define GUIKEYPAD_TYP_STD       1   //��׼��������
#define GUIKEYPAD_TYP_USER      2   //�Զ����������


/******************
* ����GUI�м��̽ṹ
******************/
typedef struct _keypad
{
    int iDevFd;                 //�����豸�ļ�������
    int iDevType;               //�����豸���ͣ�1��ʾ��׼��2��ʾ�Զ���

    THREADFUNC fnKeypadThread;  //�ص����������ڴ��������߳�
} GUIKEYPAD;


/**********************************
* ����GUI�������������صĲ�������
**********************************/
//����ָ������Ϣ�������̶���
GUIKEYPAD* CreateKeypad(char *strDevName, int iDevType, 
                        THREADFUNC fnKeypadThread);
//ɾ�����̶���
int DestroyKeypad(GUIKEYPAD **ppKeypadObj);

//�õ���ǰ�ļ��̶���
GUIKEYPAD* GetCurrKeypad(void);
//���õ�ǰ�ļ��̶���
int SetCurrKeypad(GUIKEYPAD *pKeypadObj);


/***********************
* ����GUI�� Ĭ�ϼ����߳�
***********************/
//Ĭ�ϼ����߳���ں���
void* DefaultKeypadThread(void *pThreadArg);

//�Զ���ļ��̴�������������ɼ����̵߳Ĵ���
int CustomKeypadFunc(GUIKEYPAD *pKeypadObj);


#endif  //_GUI_KEYPAD_H

