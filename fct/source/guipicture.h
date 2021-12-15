/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guipicture.h
* ժ    Ҫ��  ����GUI��ͼ�ο����ͼ�������Ϊʵ��ͼ�ο�ؼ��ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUI_PICTURE_H
#define _GUI_PICTURE_H


/***************************************
* Ϊ����GUIPICTURE����Ҫ���õ�����ͷ�ļ�
***************************************/
#include "guibase.h"
#include "guibrush.h"
#include "guipen.h"
#include "guifont.h"
#include "guibitmap.h"


/********************
* ����GUI��ͼ�ο�ṹ
********************/
typedef struct _picture
{
    GUIVISIBLE Visible;         //������Ϣ���κδ���ؼ������뽫����Ϊ��һ����Ա

    GUIBITMAP *pPicBitmap;      //ͼ�ο�����Ӧ��λͼ��Դ

    GUIBRUSH *pPicBrush;        //ͼ�ο�ʹ�õĻ�ˢ��ΪNULL��ʹ��GUI�еĵ�ǰ��ˢ
    GUIPEN *pPicPen;            //ͼ�ο�ʹ�õĻ��ʣ�ΪNULL��ʹ��GUI�еĵ�ǰ����
    GUIFONT *pPicFont;          //ͼ�ο�ʹ�õ����壬ΪNULL��ʹ��GUI�еĵ�ǰ����
} GUIPICTURE;


/************************************
* ����GUI����ͼ�ο�������صĲ�������
************************************/
//����ָ������Ϣֱ�ӽ���ͼ�ο����
GUIPICTURE* CreatePicture(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                          unsigned int uiPicWidth, unsigned int uiPicHeight, 
                          char *strBitmapFile);
//ɾ��ͼ�ο����
int DestroyPicture(GUIPICTURE **ppPicObj);

//ֱ�����ָ����ͼ�ο�
int DisplayPicture(GUIPICTURE *pPicObj);

//����ͼ�ο����Ч��������
int SetPictureArea(unsigned int uiStartX, unsigned int uiStartY, 
                   unsigned int uiEndX, unsigned int uiEndY,
                   GUIPICTURE *pPicObj);
//����ͼ�ο�Ŀɼ���
int SetPictureEnable(int iEnable, GUIPICTURE *pPicObj);
//����ͼ�ο�����Ӧ��λͼ��Դ
int SetPictureBitmap(char *strBitmapFile, GUIPICTURE *pPicObj);
//����ͼ�ο���ʹ�õĻ�ˢ
int SetPictureBrush(GUIBRUSH *pPicBrush, GUIPICTURE *pPicObj);
//����ͼ�ο���ʹ�õĻ���
int SetPicturePen(GUIPEN *pPicPen, GUIPICTURE *pPicObj);
//����ͼ�ο�����Ӧ������
int SetPictureFont(GUIFONT *pPicFont, GUIPICTURE *pPicObj);


#endif  //_GUI_PICTURE_H

