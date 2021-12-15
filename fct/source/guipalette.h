/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guipalette.h
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

#ifndef _GUI_PALETTE_H
#define _GUI_PALETTE_H


/***************************************
* Ϊ����GUIPALETTE����Ҫ���õ�����ͷ�ļ�
***************************************/
#include "guibase.h"


/********************
* ����GUI�е�ɫ��ṹ
********************/
typedef struct _palette
{
    int iPalLength;             //��ɫ�峤�ȣ����ж��ٸ���ɫ��ȡֵ��ΧΪ1~256
    unsigned char *pTabEntry;   //��ɫ����ɫ�����ڵ�ַ

    unsigned char *pRedValue;   //ָ�룬���ڷ�����ɫ���и���ɫ�ĺ�ɫ����
    unsigned char *pGreenValue; //ָ�룬���ڷ�����ɫ���и���ɫ����ɫ����
    unsigned char *pBlueValue;  //ָ�룬���ڷ�����ɫ���и���ɫ����ɫ����
} GUIPALETTE;


/************************************
* ����GUI�����ɫ��������صĲ�������
************************************/
//����ָ������Ϣֱ�ӽ�����ɫ�����
GUIPALETTE* CreatePalette(int iPalLength);
//ɾ����ɫ�����
int DestroyPalette(GUIPALETTE **ppPalObj);

//�õ���ǰ�ĵ�ɫ�����
GUIPALETTE* GetCurrPalette(void);
//���õ�ǰ�ĵ�ɫ�����
int SetCurrPalette(GUIPALETTE *pPalObj);

//��ָ����ɫ�������ƥ�����ض���ɫ��ӽ��ĵ�ɫ����ɫ
int MatchPaletteColor(unsigned int uiRgbColor, GUIPALETTE *pPalObj);


#endif  //_GUI_PALETTE_H

