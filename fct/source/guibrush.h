/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guibrush.h
* ժ    Ҫ��  ����GUI�Ļ�ˢ���ͼ�������ΪGUI��ͼģ���ʵ���ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUI_BRUSH_H
#define _GUI_BRUSH_H


/*************************************
* Ϊ����GUIBRUSH����Ҫ���õ�����ͷ�ļ�
*************************************/
#include "guibase.h"


/******************
* ����GUI�л�ˢ����
******************/
#define GUIBRUSH_TYP_SOLID      1   //��ɫ��ˢ
#define GUIBRUSH_TYP_IMAGE      2   //ͼ��ˢ


/******************
* ����GUI�л�ˢ�ṹ
******************/
typedef struct _brush
{
    int iBrushType;         //��ˢ���ͣ�Ŀǰֻ֧�ִ�ɫ��ˢ

    unsigned int uiFgColor; //��ˢǰ����ɫ��RGB888��ʽ
    unsigned int uiBgColor; //��ˢ������ɫ��RGB888��ʽ
} GUIBRUSH;


/**********************************
* ����GUI���뻭ˢ������صĲ�������
**********************************/
//����ָ������Ϣֱ�ӽ�����ˢ����
GUIBRUSH* CreateBrush(int iBrushType);
//ɾ����ˢ����
int DestroyBrush(GUIBRUSH **ppBrushObj);

//�õ���ǰ�Ļ�ˢ����
GUIBRUSH* GetCurrBrush(void);
//���õ�ǰ�Ļ�ˢ����
int SetCurrBrush(GUIBRUSH *pBrushObj);

//���û�ˢ��ǰ������ɫ
int SetBrushColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                  GUIBRUSH *pBrushObj);


#endif  //_GUI_BRUSH_H

