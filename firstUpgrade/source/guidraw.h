/****************************************************
* Copyright(c) 2012 ,һŵ����(����)���޹�˾�Ϻ��з��� 
*
* All rights reserved
*
* �ļ����ƣ�  guidraw.h
* ժ    Ҫ��  ����GUI������ͼ��������,ΪGUIͼ�οؼ���ʵ���ṩ����
*
* ��ǰ�汾��  v1.0.1 (���ӹ���24λɫ֡����Ļ������ƺ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-13
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  chunqiang.qin
* ������ڣ�  2012-09-04
****************************************************/

#ifndef _GUIDRAW_H
#define _GUIDRAW_H


/************************************
* Ϊ����GUIDRAW����Ҫ���õ�����ͷ�ļ�
************************************/
#include "guibase.h"
#include "guifbmap.h"


/**********************************
* ����GUI����ͼ��������صĲ�������
**********************************/
//����ָ������Ϣֱ�ӻ�������
int DrawPixel(unsigned int uiPixelX, unsigned int uiPixelY);
//����ָ������Ϣֱ�ӻ���ֱ��
int DrawLine(unsigned int uiStartX, unsigned int uiStartY, 
             unsigned int uiEndX, unsigned int uiEndY);
//����ָ������Ϣֱ�ӻ��ƾ���
int DrawRectangle(unsigned int uiStartX, unsigned int uiStartY, 
                  unsigned int uiEndX, unsigned int uiEndY);
//����ָ������Ϣֱ�ӻ���Բ��
int DrawCircle(unsigned int uiPointX, unsigned int uiPointY, 
               unsigned int uiCircleR);
//����ָ������Ϣֱ�ӻ��ƿ�
int DrawBlock(unsigned int uiStartX, unsigned int uiStartY, 
              unsigned int uiEndX, unsigned int uiEndY);


/****************************
* ����GUI�л�����ͼ�λ��ƺ���
****************************/
//���Ƶ������أ�Ӧ����8λɫ֡����
void _DrawPixel8(COLOR value, 
                 UINT32 pixel_x, UINT32 pixel_y, 
                 GUIFBMAP *pfbmap);
//���Ƶ������أ�Ӧ����16λɫ֡����
void _DrawPixel16(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap);
//���Ƶ������أ�Ӧ����24λɫ֡����
void _DrawPixel24(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap);
//���Ƶ������أ�Ӧ����32λɫ֡����
void _DrawPixel32(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap);

//����ˮƽ�ߣ�Ӧ����8λɫ֡����
void _DrawHor8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_x, 
               GUIFBMAP *pfbmap);
//����ˮƽ�ߣ�Ӧ����16λɫ֡����
void _DrawHor16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap);
//����ˮƽ�ߣ�Ӧ����24λɫ֡����
void _DrawHor24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap);
//����ˮƽ�ߣ�Ӧ����32λɫ֡����
void _DrawHor32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap);

//���ƴ�ֱ�ߣ�Ӧ����8λɫ֡����
void _DrawVer8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_y, 
               GUIFBMAP *pfbmap);
//���ƴ�ֱ�ߣ�Ӧ����16λɫ֡����
void _DrawVer16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap);
//���ƴ�ֱ�ߣ�Ӧ����24λɫ֡����
void _DrawVer24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap);
//���ƴ�ֱ�ߣ�Ӧ����32λɫ֡����
void _DrawVer32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap);

//����б�ߣ�Ӧ����8λɫ֡����
void _DrawBias8(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, UINT32 end_y, 
                GUIFBMAP *pfbmap);
//����б�ߣ�Ӧ����16λɫ֡����
void _DrawBias16(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);
//����б�ߣ�Ӧ����24λɫ֡����
void _DrawBias24(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);
//����б�ߣ�Ӧ����32λɫ֡����
void _DrawBias32(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);

//���ƿ���Բ��Ӧ����8λɫ֡����
void _DrawCircle8(COLOR value,
                  UINT32 point_x, UINT32 point_y, 
                  UINT32 radius, 
                  GUIFBMAP *pfbmap);
//���ƿ���Բ��Ӧ����16λɫ֡����
void _DrawCircle16(COLOR value, 
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius, 
                   GUIFBMAP *pfbmap);
//���ƿ���Բ��Ӧ����24λɫ֡����
void _DrawCircle24(COLOR value, 
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius, 
                   GUIFBMAP *pfbmap);
//���ƿ���Բ��Ӧ����32λɫ֡����
void _DrawCircle32(COLOR value, 
                   UINT32 point_x, UINT32 point_y,
                   UINT32 radius, 
                   GUIFBMAP *pfbmap);

//�������飬Ӧ����8λɫ֡����
void _DrawBlock8(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);
//�������飬Ӧ����16λɫ֡����
void _DrawBlock16(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap);
//�������飬Ӧ����24λɫ֡����
void _DrawBlock24(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap);
//�������飬Ӧ����32λɫ֡����
void _DrawBlock32(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap);


#endif   //_GUIDRAW_H

