/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guifont.h
* ժ    Ҫ��  ����GUI���������ͼ�������ΪGUI��ͼģ���ʵ���ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUI_FONT_H
#define _GUI_FONT_H


/************************************
* Ϊ����GUIFONT����Ҫ���õ�����ͷ�ļ�
************************************/
#include "guibase.h"


/*********************************
* ����GUI���������ļ���ʽ��ص�����
*********************************/
//GUI�������ļ�����
#define GUIFONT_FILE_STD    1   //��׼�����ļ���ֻ���������ݣ�������ֵ˳������
#define GUIFONT_FILE_MIX    2   //��������ļ������ǵ������ݣ�������ֵ˳������

//��Ӧ.fnt���ļ�ͷ�ṹ
typedef struct _font_fileheader
{
    UINT32 uiFileType;      //�ļ����ͣ�0x2E464E54('.FNT')Ϊ�ļ��д洢���У�С��
    UINT32 uiFileSize;      //�ļ���С���ļ�ռ�õ��ֽڴ�С
    UINT32 uiFileFormat;    //�ļ���ʽ��1��׼��2��ϣ�Ŀǰֻ֧�ֱ�׼��ʽ
    UINT16 usSegmentFlag;   //�ֶα�ǣ������Ƿ�ֶ�(0���ֶ�)��Ŀǰֻ֧�ֲ��ֶ�
    UINT16 usSegmentCount;  //�ֶμ�������������ε��������ֶα��Ϊ1ʱ��Ч
    UINT32 uiMatrixWidth;   //�����ȣ���������Ŀ��
    UINT32 uiMatrixHeight;  //����߶ȣ���������ĸ߶�
    UINT32 uiMatrixCount;   //���������ʵ�ʰ���������������
    UINT32 uiDataOffset;    //ƫ�������ļ�ͷ���������ݵ�ƫ���������ֽ�Ϊ��λ
} GUIFONT_FILEHEADER;

//��Ӧ.fnt�ķֶ�ӳ��ṹ
typedef struct _font_segmentmap
{
    UINT32 uiStartCode;     //����ζ�Ӧ����ʼ����
    UINT32 uiCodeCount;     //����ΰ������������
    UINT32 uiSegmentOffset; //�ļ�ͷ���������ݶε�ƫ�ƣ����ֽ�Ϊ��λ
    UINT32 uiReserved;      //����ֵ
} GUIFONT_SEGMENTMAP;


/******************
* ����GUI���������
******************/
typedef struct _font_matrix
{
    unsigned int uiRealWidth;   //�������ݿ�������ʾ��ʵ�ʿ��
    unsigned int uiRealHeight;  //�������ݿ�������ʾ��ʵ�ʸ߶�

    unsigned char *pMatrixData; //�������ݣ���ÿ�����ݴ洢ʱ����8λ���벢����
} GUIFONT_MATRIX;


/******************
* ����GUI������ṹ
******************/
typedef struct _font
{
    char *strFontFile;          //�����ļ�����
    int iFontFd;                //�����ļ�������

    unsigned int uiFontWidth;   //�����ȣ�����.fnt��һ�£�Ŀǰ��֧������
    unsigned int uiFontHeight;  //����߶ȣ�����.fnt��һ�£�Ŀǰ��֧������

    unsigned int uiLeftGap;     //�������࣬��ֵ<=������/2
    unsigned int uiRightGap;    //�����Ҽ�࣬��ֵ<=������/2
    unsigned int uiTopGap;      //�����ϼ�࣬��ֵ<=����߶�/2
    unsigned int uiBottomGap;   //�����¼�࣬��ֵ<=����߶�/2

    unsigned int uiFgColor;     //����ǰ��ɫ����������ɫ
    unsigned int uiBgColor;     //���屳��ɫ
} GUIFONT;


/**********************************
* ����GUI��������������صĲ�������
**********************************/
//����ָ������Ϣֱ�ӽ����������
GUIFONT* CreateFont(char *strFontFile, 
                    unsigned int uiFontWidth, unsigned int uiFontHeight, 
                    unsigned int uiFgColor, unsigned int uiBgColor);
//ɾ���������
int DestroyFont(GUIFONT **ppFontObj);

//�õ���ǰ���������
GUIFONT* GetCurrFont(void);
//���õ�ǰ���������
int SetCurrFont(GUIFONT *pFontObj);

//��������ļ��
int SetFontGap(unsigned int uiLeftGap, unsigned int uiRightGap, 
               unsigned int uiTopGap, unsigned int uiBottomGap, 
               GUIFONT *pFontObj);
//���������ǰ������ɫ
int SetFontColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                 GUIFONT *pFontObj);

//��ָ�������������ȡ�ض�������������
GUIFONT_MATRIX* ExtractFontMatrix(unsigned short usMatrixCode, 
                                  GUIFONT *pFontObj);


#endif  //_GUI_FONT_H

