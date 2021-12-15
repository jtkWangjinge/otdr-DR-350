/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guiwindow.h
* ժ    Ҫ��  ����GUI�Ĵ������ͼ�������Ϊʵ�ִ���ؼ��ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUI_WINDOW_H
#define _GUI_WINDOW_H


/**************************************
* Ϊ����GUIWINDOW����Ҫ���õ�����ͷ�ļ�
**************************************/
#include "guibase.h"
#include "guibrush.h"
#include "guipen.h"
#include "guifont.h"


/********************************************
* ����GUI�д���������ܰ����ؼ�������������
********************************************/
#define GUIWINDOW_COMP_LIMIT    1000    //�����������ɰ����Ŀؼ�����


/********************************
* ����GUI�д��崦�����ĵ��ýӿ�
********************************/
typedef int (*WNDFUNC)(void *pWndObj);


/******************
* ����GUI�д���ṹ
******************/
typedef struct _window
{
    GUIVISIBLE Visible;     //����Ŀ�����Ϣ

    WNDFUNC fnWndInit;      //ָ�����ʼ�������������Դ���估��Ϣע���
    WNDFUNC fnWndExit;      //ָ�����˳������������Ϣע������Դ����
    WNDFUNC fnWndPaint;     //ָ������ƺ�������ɴ���ؼ��Ļ���
    WNDFUNC fnWndLoop;      //ָ����ѭ����������ɴ���ѭ������Ϊ�������
    THREADFUNC fnWndThread; //���ڴ������崦���̣߳�Ϊ��ʹ��ϵͳ�Ĵ��崦���߳�
    GUITHREAD thdWndTid;    //�����߳�ID�����崦���̵߳��̱߳�ʶ��
    GUILIST *pWndComps;     //�ؼ����У�������������GUI����ؼ�����
    GUIMUTEX Mutex;         //�����������ڿ��ƶԿؼ����еķ���

    GUIBRUSH *pWndBrush;    //������ʹ�õĻ�ˢ��ΪNULL��ʹ��GUI�еĵ�ǰ��ˢ
    GUIPEN *pWndPen;        //������ʹ�õĻ��ʣ�ΪNULL��ʹ��GUI�еĵ�ǰ����
    GUIFONT *pWndFont;      //������ʹ�õ����壬ΪNULL��ʹ��GUI�еĵ�ǰ����
} GUIWINDOW;


/**********************************
* ����GUI���봰��������صĲ�������
**********************************/
//����ָ������Ϣֱ�ӽ����������
GUIWINDOW* CreateWindow(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                        unsigned int uiWndWidth, unsigned int uiWndHeight, 
                        WNDFUNC fnWndInit, WNDFUNC fnWndExit, 
                        WNDFUNC fnWndPaint, WNDFUNC fnWndLoop, 
                        THREADFUNC fnWndThread);
//ɾ���������
int DestroyWindow(GUIWINDOW **ppWndObj);

//��ʼ����ǰ����
int InitCurrWindow(void);
//��ס��ǰ�������
int LockCurrWindow(void);
//������ǰ�������
int UnlockCurrWindow(void);
//���ֵ�ǰ�Ĵ�����󲻱䣬��Ҫ��UnlockCurrWindow()�ɶ�ʹ��
GUIWINDOW* HoldCurrWindow(void);
//�õ���ǰ�Ĵ������
GUIWINDOW* GetCurrWindow(void);
//���õ�ǰ�Ĵ������
int SetCurrWindow(GUIWINDOW *pWndObj);

//���ô������Ч��������
int SetWindowArea(unsigned int uiStartX, unsigned int uiStartY, 
                  unsigned int uiEndX, unsigned int uiEndY,
                  GUIWINDOW *pWndObj);
//���ô�����ʹ�õĻ�ˢ
int SetWindowBrush(GUIBRUSH *pWndBrush, GUIWINDOW *pWndObj);
//���ô�����ʹ�õĻ���
int SetWindowPen(GUIPEN *pWndPen, GUIWINDOW *pWndObj);
//���ô�������Ӧ������
int SetWindowFont(GUIFONT *pWndFont, GUIWINDOW *pWndObj);

//���GUI����ؼ���ָ������
int AddWindowComp(int iCompTyp, int iCompLen, void *pWndComp, 
                       GUIWINDOW *pWndObj);
//��ָ������ɾ��GUI����ؼ�
int DelWindowComp(void *pWndComp, GUIWINDOW *pWndObj);
//���ָ������Ĵ���ؼ�����
int ClearWindowComp(GUIWINDOW * pWndObj);


#endif  //_GUI_WINDOW_H

