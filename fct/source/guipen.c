/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guipen.c
* ժ    Ҫ��  ʵ��GUI�Ļ������ͼ�������ΪGUI��ͼģ���ʵ���ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guipen.h"


/***********************************
* Ϊʵ��GUIPEN����Ҫ���õ�����ͷ�ļ�
***********************************/
//#include ""


/********************
* GUI�еĵ�ǰ���ʶ���
********************/
static GUIPEN *pCurrPen = NULL;


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӽ������ʶ���
  * ������
        1.int iPenType:             ָ��Ҫ�����Ļ�������
        2.int iPenWidth:            ָ��Ҫ�����Ļ��ʿ��
        3.unsigned int uiPenColor:  ָ��Ҫ�����Ļ�����ɫ
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIPEN* CreatePen(int iPenType, int iPenWidth, unsigned int uiPenColor)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIPEN *pPenObj = NULL;

    if (iErr == 0)
    {
        //����Ϊ���ʶ�������ڴ�
        pPenObj = (GUIPEN *) malloc(sizeof(GUIPEN));
        if (NULL == pPenObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //���ʶ����Ա��ֵ
        pPenObj->iPenType = iPenType;
        pPenObj->iPenWidth = iPenWidth;
        pPenObj->uiPenColor = uiPenColor;
    }

    return pPenObj;
}


/***
  * ���ܣ�
        ɾ�����ʶ���
  * ������
        1.GUIPEN **ppPenObj:    ָ���ָ�룬ָ����Ҫ���ٵĻ��ʶ���
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DestroyPen(GUIPEN **ppPenObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppPenObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppPenObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppPenObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppPenObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //���ٻ��ʶ��󣬲���ָ���ÿ�
        free(*ppPenObj);
        *ppPenObj = NULL;
    }

    return iErr;
}


/***
  * ���ܣ�
        �õ���ǰ�Ļ��ʶ���
  * ������
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIPEN* GetCurrPen(void)
{
    return pCurrPen;
}


/***
  * ���ܣ�
        ���õ�ǰ�Ļ��ʶ���
  * ������
        1.GUIPEN *pPenObj:  ������Ϊ��ǰ���ʶ���
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetCurrPen(GUIPEN *pPenObj)
{
    //�ж�pPenObj�Ƿ���Ч
    if (NULL == pPenObj)
    {
        return -1;
    }

    pCurrPen = pPenObj;

    return 0;
}

