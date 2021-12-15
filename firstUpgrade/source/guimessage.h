/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guimessage.h
* ժ    Ҫ��  GUIMESSAGE����ϵͳ����ģ�飬�˲��ֶ���GUI����Ϣ���ͼ�������ز�
*             ��������ʵ����Ϣ�ĵ���ģ�͡�
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  yexin.zhu
* ������ڣ�  2012-8-21
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUI_MESSAGE_H
#define _GUI_MESSAGE_H


/***************************************
* Ϊ����GUIMESSAGE����Ҫ���õ�����ͷ�ļ�
***************************************/
#include "guibase.h"


/**************************************************
* ����GUI����Ϣ�������ܰ�����Ϣ�ע������������
**************************************************/
#define GUIMESSAGE_ITEM_LIMIT       1000    //������Ϣ�������ɰ�������Ϣ��
#define GUIMESSAGE_REG_LIMIT        1000    //������Ϣ�������ɰ�������Ϣע����


/******************
* ����GUI����Ϣ����
******************/
#define GUIMESSAGE_TYP_SYS          1   //ϵͳ��Ϣ��ϵͳ��Ϣֻ���ɵ����̴߳���
#define GUIMESSAGE_TYP_WND          2   //������Ϣ��������Ϣ���ɸ������̴߳���


/******************
* ����GUI����Ϣ����
******************/
#define GUIMESSAGE_SYS_EXIT         1   //ϵͳ��Ϣ��ϵͳ�˳�
#define GUIMESSAGE_THD_CREAT        2   //ϵͳ��Ϣ�����������߳�
#define GUIMESSAGE_THD_DEST         3   //ϵͳ��Ϣ�����ٴ����߳�
#define GUIMESSAGE_WND_INIT         4   //������Ϣ�������ʼ��
#define GUIMESSAGE_WND_EXIT         5   //������Ϣ�������˳�
#define GUIMESSAGE_WND_PAINT        6   //������Ϣ���������
#define GUIMESSAGE_WND_LOOP         7   //������Ϣ������ѭ��
#define GUIMESSAGE_WND_PAUSE        8   //������Ϣ���������
#define GUIMESSAGE_WND_RESUME       9   //������Ϣ������ָ�
#define GUIMESSAGE_LOP_ENABLE       10  //������Ϣ��������������ѭ��
#define GUIMESSAGE_LOP_DISABLE      11  //������Ϣ����ֹ����ѭ��
#define GUIMESSAGE_ERR_PROC         12  //������Ϣ��ִ�д�����
#define GUIMESSAGE_KEY_DOWN         13  //������Ϣ����������
#define GUIMESSAGE_KEY_UP           14  //������Ϣ�������ɿ�
#define GUIMESSAGE_TCH_DOWN         15  //������Ϣ�����������
#define GUIMESSAGE_TCH_UP           16  //������Ϣ���뿪������
#define GUIMESSAGE_KNB_CLOCK        17  //������Ϣ����ť˳ʱ����ת
#define GUIMESSAGE_KNB_ANTI         18  //������Ϣ����ť��ʱ����ת



/********************************
* ����GUI����Ϣ�������ĵ��ýӿ�
********************************/
typedef int (*MSGFUNC)(void *pInArg, int iInLen, void *pOutArg, int iOutLen);


/****************
* ����GUI����Ϣ��
****************/
typedef struct _message_item
{
    int iMsgType;               //��Ϣ���ͣ�1ϵͳ��Ϣ��2������Ϣ��<0�Զ�������
    int iMsgCode;               //��Ϣ���룬WND_EXIT��KEY_UP����<0�Զ������

    unsigned int uiMsgValue;    //��Ϣ��ֵ����ֵ������Ϊ��Ϣ�������Ĳ���
    int iValueLength;           //��Ϣ��ֵ�ĳ��ȣ�����Ϣ��ֵΪĳ��ָ��ʱ��Ч

    void *pRecvObj;             //��Ϣ���ն�������ƥ����Ϣ������
} GUIMESSAGE_ITEM;


/********************
* ����GUI����Ϣע����
********************/
typedef struct _message_reg
{
    int iMsgCode;           //��Ϣ���룬����ƥ����Ϣ������
    void *pRecvObj;         //��Ϣ���ն�������ƥ����Ϣ������

    MSGFUNC fnMsgFunc;      //���ڴ�����Ϣ�ĺ���
    void *pOutArg;          //��Ϣ�������Ĵ�������
    int iOutLength;         //���������ĳ���
} GUIMESSAGE_REG;


/******************
* ����GUI����Ϣ�ṹ
******************/
typedef struct _message
{
    GUILIST *pMsgQueue;     //��Ϣ���У����ڱ���GUI��Ϣ
    GUILIST *pMsgReg;       //ע����Ϣ�����ڱ�����Ϣע����

    GUIMUTEX Mutex;         //������������ͬ����־����Ϣ���м�ע����Ϣ�ķ���
} GUIMESSAGE;


/**********************************
* ����GUI������Ϣ������صĲ�������
**********************************/
//����ָ������Ϣֱ�Ӵ�����Ϣ����
GUIMESSAGE* CreateMessage(int iQueueLimit, int iRegLimit);
//ɾ����Ϣ����
int DestroyMessage(GUIMESSAGE **ppMsgObj);

//�õ���ǰ��Ϣ����
GUIMESSAGE* GetCurrMessage(void);
//����Ϊ��ǰ��Ϣ����
int SetCurrMessage(GUIMESSAGE *pMsgObj);

//��ָ����Ϣ�����н����ض����͵���Ϣ��
GUIMESSAGE_ITEM* RecvSpecialMessage(int iMsgType, GUIMESSAGE *pMsgObj);
//����ָ����Ϣ�����ж���ͷ������Ϣ��
GUIMESSAGE_ITEM* ReadMessageQueue(GUIMESSAGE *pMsgObj);
//д����Ϣ�ָ����Ϣ����Ķ���β��
int WriteMessageQueue(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj);
//�����Ϣ����
int ClearMessageQueue(GUIMESSAGE *pMsgObj);

//ע����Ϣע���ָ����Ϣ����
int LoginMessageReg(int iMsgCode, void *pRecvObj, 
                    MSGFUNC fnMsgFunc, void *pOutArg, int iOutLength, 
                    GUIMESSAGE *pMsgObj);
//��ָ����Ϣ����ע����Ϣע����
int LogoutMessageReg(int iMsgCode, void *pRecvObj, GUIMESSAGE *pMsgObj);
//�����Ϣע����Ϣ
int ClearMessageReg(GUIMESSAGE *pMsgObj);

//��ָ����Ϣ������ƥ������Ϣ���Ӧ����Ϣע����
GUIMESSAGE_REG* MatchMessageReg(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj);
//��ָ����Ϣ������ƥ����Ϣ��ִ�ж�Ӧ����Ϣ������
int ExecMessageProc(GUIMESSAGE_ITEM *pMsgItem, GUIMESSAGE *pMsgObj);


#endif  //_GUI_MESSAGE_H

