/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guibitmap.h
* ժ    Ҫ��  ����GUIλͼ���ͼ�������ز�����ʵ��BMPͼƬ�Ļ��ơ�
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-12
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUIBITMAP_H
#define _GUIBITMAP_H


/**************************************
* Ϊ����GUIBITMAP����Ҫ���õ�����ͷ�ļ�
**************************************/
#include "guibase.h"
#include "guiimage.h"


/*********************************
* ����GUI����BMP�ļ���ʽ��ص�����
*********************************/
//��Ӧ.bmp���ļ�ͷ�ṹ
typedef struct _bitmap_fileheader
{
    UINT16 usFillChar;      //����ַ������ڶ����ֱ߽�
    UINT16 usFileType;      //�ļ����ͣ�0x424D('BM')Ϊ�ļ��еĴ洢���У�С�˴洢
    UINT32 uiFileSize;      //�ļ���С���ļ�ռ�õ��ֽڴ�С
    UINT16 usReserved1;     //������1��ֵ����Ϊ0
    UINT16 usReserved2;     //������2��ֵ����Ϊ0
    UINT32 uiDataOffset;    //ƫ�������ļ�ͷ��ʵ��λͼ���ݵ�ƫ���������ֽ�Ϊ��λ
} GUIBITMAP_FILEHEADER;

//��Ӧ.bmp����Ϣͷ�ṹ
typedef struct _bitmap_infoheader
{
    UINT32 uiInfoSize;      //��Ϣͷ��С������Ϣͷռ�õ��ֽڴ�С��ֵΪ40
    UINT32 uiImgWidth;      //ͼ����
    UINT32 uiImgHeight;     //ͼ��߶ȣ�Ŀǰֻ֧�ֵ���λͼ(���½�Ϊԭ��)
    UINT16 usDevPlanes;     //Ŀ���豸λ������ֵ����Ϊ1
    UINT16 usBitCount;      //ÿ������ռ��������Ŀǰֻ֧��1λɫ��8λɫ��24λɫ
    UINT32 uiCompression;   //ѹ���㷨��Ŀǰֻ֧�ֲ�ѹ��ͼ��(BI_RGB��BI_RGB==0)
    UINT32 uiImgSize;       //ͼ���С��ͼ��ռ�õ��ֽڴ�С����ֵ����Ϊ4�ı���
    UINT32 uiXRes;          //ˮƽ�ֱ��ʣ���λΪ����/��
    UINT32 uiYRes;          //��ֱ�ֱ��ʣ���λΪ����/��
    UINT32 uiClrUsed;       //ʵ��ʹ�õ���ɫ��
    UINT32 uiClrImportant;  //��Ҫ����ɫ��
} GUIBITMAP_INFOHEADER;

//��Ӧ.bmp�ĵ�ɫ��ṹ
typedef struct _bitmap_rgbquad
{
    UINT8 ucBlue;       //����ɫ����ɫ����
    UINT8 ucGreen;      //����ɫ����ɫ����
    UINT8 ucRed;        //����ɫ�ĺ�ɫ����
    UINT8 ucReserved;   //����ֵ
} GUIBITMAP_RGBQUAD;


/******************
* ����GUI��λͼ�ṹ
******************/
typedef struct _bitmap
{
    char *strBitmapFile;        //λͼ�ļ�����
    int iBitmapFd;              //λͼ�ļ�������

    unsigned int uiViewWidth;   //��ʾ��ȣ�>=0����ʼ��Ϊ0��BMPʵ�ʿ��
    unsigned int uiViewHeight;  //��ʾ�߶ȣ�>=0����ʼ��Ϊ0��BMPʵ�ʸ߶�

    unsigned int uiPlaceX;      //λͼˮƽ����λ�ã������Ͻ�Ϊ����
    unsigned int uiPlaceY;      //λͼ��ֱ����λ�ã������Ͻ�Ϊ����

    GUIIMAGE *pBitmapImg;       //λͼ����Ӧ��ͼ����Դ
} GUIBITMAP;


/**********************************
* ����GUI����λͼ������صĲ�������
**********************************/
//����ָ������Ϣֱ�ӽ���λͼ����
GUIBITMAP* CreateBitmap(char *strBitmapFile, 
                        unsigned int uiPlaceX, unsigned int uiPlaceY);
//ɾ��λͼ����
int DestroyBitmap(GUIBITMAP **ppBitmapObj);

//ֱ�����ָ����λͼ
int DisplayBitmap(GUIBITMAP *pBitmapObj);

//����λͼ����ʾ��С
int SetBitmapView(unsigned int uiViewWidth, unsigned int uiViewHeight, 
                  GUIBITMAP *pBitmapObj);
//����λͼ�ķ���λ��
int SetBitmapPlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                   GUIBITMAP *pBitmapObj);

//װ��λͼͼ����Դ
int LoadBitmapImage(GUIBITMAP *pBitmapObj);
//ж��λͼͼ����Դ
int UnloadBitmapImage(GUIBITMAP *pBitmapObj);
//��ȡλͼ�еĵ�ɫ��
GUIPALETTE* ReadBitmapPalette(GUIBITMAP *pBitmapObj);


#endif  //_GUIBITMAP_H

