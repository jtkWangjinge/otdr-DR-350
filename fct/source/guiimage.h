/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guiimage.h
* ժ    Ҫ��  ����GUIͼ�����ͼ�������ز�����������GUI������ͼ����Ʋ���������Ϊ
*             GUIͼ�οؼ���ʵ���ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-11
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#ifndef _GUIIMAGE_H
#define _GUIIMAGE_H


/*************************************
* Ϊ����GUIIMAGE����Ҫ���õ�����ͷ�ļ�
*************************************/
#include "guibase.h"
#include "guifbmap.h"
#include "guipalette.h"


/******************
* ����GUI��ͼ��ṹ
******************/
typedef struct _image
{
    unsigned int uiImgWidth;    //ͼ��ˮƽ��ȣ���λΪ����
    unsigned int uiImgHeight;   //ͼ��ֱ�߶ȣ���λΪ����
    unsigned int uiImgDepth;    //ͼ����ɫ��ȣ�1λ��8λ��24λ
    unsigned char *pImgData;    //ͼ������

    unsigned int uiDispWidth;   //����ʾ�Ŀ�ȣ������֡����ӳ�������
    unsigned int uiDispHeight;  //����ʾ�ĸ߶ȣ������֡����ӳ�������

    unsigned int uiPlaceX;      //ͼ��ˮƽ����λ�ã������Ͻ�Ϊ���㣬��������
    unsigned int uiPlaceY;      //ͼ��ֱ����λ�ã������Ͻ�Ϊ���㣬��������

    unsigned int uiFgColor;     //ͼ���ǰ����ɫ��uiImgDepthΪ1ʱʹ��
    unsigned int uiBgColor;     //ͼ��ı�����ɫ��uiImgDepthΪ1ʱʹ��
    GUIPALETTE *pRefPal;        //ͼ�����õĵ�ɫ�壬��uiImgDepthΪ8ʱʹ��
} GUIIMAGE;


/**********************************
* ����GUI����ͼ��������صĲ�������
**********************************/
//����ָ������Ϣֱ�ӽ���ͼ�����
GUIIMAGE* CreateImage(unsigned int uiImgWidth, 
                      unsigned int uiImgHeight, 
                      unsigned int uiImgDepth);
//ɾ��ͼ�����
int DestroyImage(GUIIMAGE **ppImgObj);

//ֱ�����ָ����ͼ��
int DisplayImage(GUIIMAGE *pImgObj);

//����ͼ��ķ���λ��
int SetImagePlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                  GUIIMAGE *pImgObj);
//����ͼ���ǰ������ɫ
int SetImageColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                  GUIIMAGE *pImgObj);
//����ͼ�����õĵ�ɫ��
int SetImagePalette(GUIPALETTE *pRefPalette, GUIIMAGE *pImgObj);


/****************************
* ����GUI�л�����ͼ����ƺ���
****************************/
//����1λɫͼ��Ӧ����8λɫ֡����
void _Image1Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����1λɫͼ��Ӧ����16λɫ֡����
void _Image1Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����1λɫͼ��Ӧ����24λɫ֡����
void _Image1Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����1λɫͼ��Ӧ����32λɫ֡����
void _Image1Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap);

//����8λɫͼ��Ӧ����8λɫ֡����
void _Image8Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����8λɫͼ��Ӧ����16λɫ֡����
void _Image8Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����8λɫͼ��Ӧ����24λɫ֡����
void _Image8Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����8λɫͼ��Ӧ����32λɫ֡����
void _Image8Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap);

//����24λɫͼ��Ӧ����8λɫ֡����
void _Image24Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����24λɫͼ��Ӧ����16λɫ֡����
void _Image24Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����24λɫͼ��Ӧ����24λɫ֡����
void _Image24Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//����24λɫͼ��Ӧ����32λɫ֡����
void _Image24Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap);


#endif  //_GUIIMAGE_H

