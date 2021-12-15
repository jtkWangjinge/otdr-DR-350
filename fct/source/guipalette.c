/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guipalette.c
* ժ    Ҫ��  ����GUI�ĵ�ɫ�����ͼ�������ΪGUI��ͼģ���ʵ���ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-10
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guipalette.h"


/***************************************
* Ϊʵ��GUIPALETTE����Ҫ���õ�����ͷ�ļ�
***************************************/
//#include ""


/**********************
* GUI�еĵ�ǰ��ɫ�����
**********************/
static GUIPALETTE *pCurrPalette = NULL;


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӽ�����ɫ�����
  * ������
        1.int iPalLength:   ָ��Ҫ�����ĵ�ɫ�峤��
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIPALETTE* CreatePalette(int iPalLength)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIPALETTE *pPalObj = NULL;

    if (iErr == 0)
    {
        //�ж�uiPalLength�Ƿ񳬳�����
        if (iPalLength < 1 || iPalLength > 256)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //����Ϊ��ɫ���������ڴ�
        pPalObj = (GUIPALETTE *) malloc(sizeof(GUIPALETTE));
        if (NULL == pPalObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //Ϊ��ɫ������ڴ���Դ
        pPalObj->pTabEntry = (unsigned char *) malloc(3 * iPalLength);
        if (NULL == pPalObj->pTabEntry)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //Ϊ��ɫ���и���ɫ��RGB����ָ������ָ��
        pPalObj->pRedValue = pPalObj->pTabEntry + iPalLength * 0;
        pPalObj->pGreenValue = pPalObj->pTabEntry + iPalLength * 1;
        pPalObj->pBlueValue = pPalObj->pTabEntry + iPalLength * 2;
        //�����½���ɫ�����ĵ�ɫ�峤��
        pPalObj->iPalLength = iPalLength;
    }

    //������
    switch (iErr)
    {
    case -3:
        free(pPalObj);
    case -2:
    case -1:
        pPalObj = NULL;
    default:
        break;
    }

    return pPalObj;
}


/***
  * ���ܣ�
        ɾ����ɫ�����
  * ������
        1.GUIPALETTE *pPalObj:  ָ���ָ�룬ָ����Ҫ���ٵĵ�ɫ�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyPalette(GUIPALETTE **ppPalObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppPalObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppPalObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppPalObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppPalObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //�ͷ���ɫ��
        free((*ppPalObj)->pTabEntry);
        //���ٵ�ɫ����󣬲���ָ���ÿ�
        free(*ppPalObj);
        *ppPalObj = NULL;
    }

    return iErr;
}


/***
  * ���ܣ�
        ��õ�ǰ��ɫ�����
  * ������
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIPALETTE* GetCurrPalette(void)
{
    return pCurrPalette;
}


/***
  * ���ܣ�
        ����Ϊ��ǰ��ɫ�����
  * ������
        1.GUIPALETTE *pPalObj:  ������Ϊ��ǰ��ɫ�����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetCurrPalette(GUIPALETTE *pPalObj)
{
    //�ж�pPalObj�Ƿ���Ч
    if (NULL == pPalObj)
    {
        return -1;
    }

    pCurrPalette = pPalObj;

    return 0;
}


/***
  * ���ܣ�
       ��ָ����ɫ�������ƥ�����ض���ɫ��ӽ��ĵ�ɫ����ɫ
  * ������
        1.unsigned int uiRgbColor:  ��ƥ����ض���ɫ��RGB888
        2.GUIPALETTE *pPalObj:      ��ƥ��ĵ�ɫ�����
  * ���أ�
        �ɹ�������ɫ����(0~255)��ʧ�ܷ��ظ�ֵ
  * ��ע��
***/
int MatchPaletteColor(unsigned int uiRgbColor, GUIPALETTE *pPalObj)
{
    //�����־������ֵ����
    int iErr = 0;
    int iReturn = -1;
    //��ʱ��������
    unsigned char *pRed, *pGreen, *pBlue;
    unsigned char ucRed, ucGreen, ucBlue;
    int iMin, iLength, iCurr, iDiff;

    if (iErr == 0)
    {
        //�ж�pPalObj�Ƿ�Ϊ��Чָ��
        if (NULL == pPalObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�õ���ɫ���и���ɫ���RGB����ָ��
        pRed = pPalObj->pRedValue;
        pGreen = pPalObj->pGreenValue;
        pBlue = pPalObj->pBlueValue;
        //��ȡ�ض���ɫ��RGB������
        ucRed = (uiRgbColor & 0xFF0000) >> 16;
        ucGreen = (uiRgbColor & 0x00FF00) >> 8;
        ucBlue = (uiRgbColor & 0x0000FF) >> 0;
        //������ɫ����������ɫ��Ѱ�����ض���ɫRGB����������֮����С�ĵ�ɫ����
        iMin = 255 * 255 * 3;       //RGB����������֮�����ֵΪ255*255*3
        iLength = pPalObj->iPalLength;
        while (iLength--)
        {
            iCurr = 0;
            iDiff = pRed[iLength] - ucRed;
            iCurr += iDiff * iDiff;
            iDiff = pGreen[iLength] - ucGreen;
            iCurr += iDiff * iDiff;
            iDiff = pBlue[iLength] - ucBlue;
            iCurr += iDiff * iDiff;
            if (iCurr < iMin)
            {
                iMin = iCurr;
                iReturn = iLength;  //��¼��ɫ����ı��
            }
        }
    }

    return iReturn;
}

