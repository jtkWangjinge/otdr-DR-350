/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guibase.h
* ժ    Ҫ��  ����GUI��ʹ�õ�ϵͳͷ�ļ�������GUI�Ļ������ݽṹ��������Ӧ�Ļ���
*             ��������������GUI����ʹ�õĻص���������Ϊ����ģ���ʵ���ṩ������
*
* ��ǰ�汾��  v1.0.1 (���ǵ���ƽ̨�������˶���GUI_OS_ENV��Ԥ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-11
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  yexin.zhu
* ������ڣ�  2012-7-24
*******************************************************************************/

#ifndef _GUI_BASE_H
#define _GUI_BASE_H


/********************************
* Ϊʵ��GUI����Ҫ���õı�׼ͷ�ļ�
********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/********************************
* Ϊʵ��GUI����Ҫ���õ�ϵͳͷ�ļ�
********************************/
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>


/****************************
* ����GUI��ʹ�õĻ�����������
****************************/
typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned int COLOR;         //�ɸߵ���ΪARGB��ʽ��Ŀǰ�в�֧��Alpha����
typedef unsigned short GUICHAR;     //Ϊ֧�ֶ�����ԣ�GUI��ʹ�ÿ��ַ�

//ת��C�ַ���ΪGUI�ַ���
GUICHAR* TransString(char *pString);
//�ַ���ճ�������������ڱ�׼C���strcat()
GUICHAR* StringCat(GUICHAR **ppDest, GUICHAR *pSrc);
//�ַ������ƣ����������ڱ�׼C���strcpy()
GUICHAR* StringCpy(GUICHAR **ppDest, GUICHAR *pSrc);
//�ַ����Ƚϣ����������ڱ�׼C���strcmp()
int StringCmp(GUICHAR *pS1, GUICHAR *pS2);


/******************************
* ����GUI��ʹ�õ���λ����꺯��
******************************/
#define bitmask_high(n, bits)   (((1 << (n)) - 1) << ((bits) - (n)))
#define bitmask_low(n, bits)    ((1 << (n)) - 1)


/******************************
* ����GUI��ʹ�õ�Բ������꺯��
******************************/
#define round_up(x, align)      (((x) + (align) - 1) & ~((align) - 1))
#define round_down(x, align)    ((x) & ~((align) - 1))


/****************************
* ����GUI�лص������ĵ��ýӿ�
****************************/
typedef struct _hookpar
{
    int ret;
    void *inpar;
    int inlen;
    void *outpar;
    int outlen;
} HOOKPAR;

typedef void* (*HOOKFUNC)(HOOKPAR *);


/************************
* ����GUI��������ĵ�ṹ
************************/
typedef struct _point
{
    unsigned int x;     //������
    unsigned int y;     //������
} GUIPOINT;


/**************************
* ����GUI��������ľ��νṹ
**************************/
typedef struct _rectangle
{
    GUIPOINT Start;     //������㣬�����ϽǺ���������С�ĵ�
    GUIPOINT End;       //�����յ㣬�����½Ǻ����������ĵ�
} GUIRECTANGLE;


/******************************
* ����GUI��������Ŀ��ӿؼ��ṹ
******************************/
typedef struct _visible
{
    GUIRECTANGLE Area;  //���ӿؼ�����Ч����
    int iEnable;        //���ӿؼ��Ƿ�ɼ���0���ɼ���1�ɼ�
    int iFocus;         //���ӿؼ��Ƿ���н��㣬0�޽��㣬1�н���
    int iLayer;         //���ӿؼ�������ͼ�㣬��ֵ����>=0
} GUIVISIBLE;


/****************************
* ����GUI�������Ĳ���ϵͳ����
****************************/
#define NONE_OS             0           //�޲���ϵͳ
#define LINUX_OS            1           //Linux����ϵͳ
#define GUI_OS_ENV          LINUX_OS    //GUI��ǰ�����Ĳ���ϵͳ����


/**********************************
* ����GUI��ʹ�õ��̼߳���ز�������
**********************************/
#if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_t,THREADFUNC
typedef pthread_t GUITHREAD;
typedef void* (*THREADFUNC)(void *arg);
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif

//�����µ��߳�
int ThreadCreate(GUITHREAD *pThread, THREADFUNC fnEntry, void *pArg);
//�˳���ǰ�߳�
int ThreadExit(void *pReturn);
//�ȴ��߳̽���
int ThreadJoin(GUITHREAD Thread, void **ppReturn);


/************************************
* ����GUI��ʹ�õĻ���������ز�������
************************************/
#if (GUI_OS_ENV == LINUX_OS)  //OS���:pthread_mutex_t
typedef pthread_mutex_t GUIMUTEX;
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif

//��ʼ����
int InitMutex(GUIMUTEX *pMutex);
//��������
int MutexLock(GUIMUTEX *pMutex);
//���Լ���
int MutexTrylock(GUIMUTEX *pMutex);
//��������
int MutexUnlock(GUIMUTEX *pMutex);


/************************
* ����GUI��ʹ�õ����ߺ���
************************/
//�������ɺ���
int MsecSleep(unsigned long ulMsec);


/**************************************
* ����GUI��ʹ�õ��б�ṹ����ز�������
**************************************/
typedef struct _list_item
{
    struct _list *pOwner;           //ָ���б�ͷ
    struct _list_item *pNext;       //��һ���б���
    struct _list_item *pPrev;       //ǰһ���б���
} GUILIST_ITEM;

typedef struct _list
{
    int iCount;                     //�б��������0 <= iCount <= iLimit
    int iLimit;                     //�б������ƣ�iLimit >= iCount >= 0
    struct _list_item *pHead;       //ָ���б�ͷ
    struct _list_item *pTail;       //ָ���б�β
} GUILIST;

//�����б�
GUILIST* CreateList(int iLimit);
//ɾ���б�
int DestroyList(GUILIST *pList);

//��ʼ���б�
inline int InitList(GUILIST *pList);

//���һ�б���б�β��
int ListAdd(GUILIST_ITEM *pItem, GUILIST *pList);
//���б���ɾ��һ�б���
int ListDel(GUILIST_ITEM *pItem, GUILIST *pList);
//��һ���б����Ƴ�һ�б����һ�б�β��
int ListMove(GUILIST_ITEM *pItem, GUILIST *pList);
//�ϲ��б���һ�б�β��
int ListSplice(GUILIST *pAttach, GUILIST *pList);

//�ж��б��Ƿ�Ϊ��
inline int ListEmpty(GUILIST *pList);
//�ж��б��Ƿ�Ϊ��
inline int ListFull(GUILIST *pList);


//��������б�ĺ꺯��
#define list_next_each(item, list, pos)                                     \
    for (item = (list)->pHead, pos = (list)->iCount; pos--; item = item->pNext)
//ǰ������б�ĺ꺯��
#define list_prev_each(item, list, pos)                                     \
    for (item = (list)->pTail, pos = (list)->iCount; pos--; item = item->pPrev)


/**********************************
* ����GUI��������İ���������GUIOBJ
**********************************/
typedef struct _object
{
    GUILIST_ITEM List;  //���ڶ���ָ��ͬһ���������ڵ���һ��GUIOBJ
    int iType;          //�������ͣ�GUIOBJ�����������������(��Ҫ���ڴ���ؼ�)
    int iLength;        //���󳤶ȣ�GUIOBJ������������ĳ���
    void *pObj;         //�����������������ض�GUI����
} GUIOBJ;

//������GUIOBJ��ʹ�õĶ�������(��ҪΪ����ؼ�)
#define OBJTYP_GUIBUTTON        1
#define OBJTYP_GUICAPTION       2
#define OBJTYP_GUILABEL         3
#define OBJTYP_GUIMENU          4
#define OBJTYP_GUIPICTURE       5
#define OBJTYP_GUISCROLL        6
#define OBJTYP_GUISTATUS        7
#define OBJTYP_GUIWINDOW        8

//���ڻ�ȡ����������ĺ꺯��
#define container_of(ptr, type, member)                                     \
({                                                                          \
    const typeof(((type *)0)->member) *__mptr = (ptr);                      \
    (type *)((char *)__mptr - ((unsigned int)&((type *)0)->member));        \
})


/************************************
* ����GUI�����ڱ����ؼ����еĿؼ�����
************************************/
typedef struct _component
{
    GUIVISIBLE Visible;
} GUICOMPONENT;


#endif  //_GUI_BASE_H

