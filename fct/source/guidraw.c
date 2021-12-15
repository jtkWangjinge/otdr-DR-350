/*******************************************************************************
* Copyright (c) 2012 ,һŵ����(����)���޹�˾�Ϻ��з���
*
* All right reserved
*
* �ļ����ƣ�  guidraw.c
* ժ    Ҫ��  ����GUI������ͼ����������ΪGUIͼ�οؼ���ʵ���ṩ����.
*
* ��ǰ�汾��  v1.0.1 (���ӹ���24λɫ֡����Ļ������ƺ�������)
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-13
*
* ȡ���汾��  v1.0.0
* ԭ �� �ߣ�  chunqiang.qin
* ������ڣ�  2012-09-04
*******************************************************************************/

#include "guidraw.h"


/************************************
* Ϊʵ��GUIDRAW����Ҫ���õ�����ͷ�ļ�
************************************/
#include "guipalette.h"
#include "guipen.h"


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӻ�������
  * ������
        1.unsigned int uiPixelX:    ���ص�ĺ�����
        2.unsigned int uiPixelY:    ���ص��������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DrawPixel(unsigned int uiPixelX, unsigned int uiPixelY)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Pixel;
    unsigned int uiValue;

    if (iErr == 0)
    {
        //�õ���ǰ��ɫ������ж��Ƿ�Ϊ��Чָ��
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�õ���ǰ���ʶ����ж��Ƿ�Ϊ��Чָ��
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {    
        //�õ���ǰ֡����ӳ������ж��Ƿ�Ϊ��Чָ��
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {    
        //ȷ�����ص�
        Pixel.x = uiPixelX;
        Pixel.y = uiPixelY;
        //�ж����ص�λ���Ƿ�Խ��
        if ((Pixel.x >= pFbmap->uiHorRes) || 
            (Pixel.y >= pFbmap->uiVerRes))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //����֡����������ȣ��õ�������ʾ����ɫֵ��������Ӧ�������л���
        switch (pFbmap->uiPixelBits)
        {
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8λɫ
            _DrawPixel8(uiValue, uiPixelX, uiPixelY, pFbmap);
            break;
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            _DrawPixel16(uiValue, uiPixelX, uiPixelY, pFbmap);
            break;
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            _DrawPixel24(uiValue, uiPixelX, uiPixelY, pFbmap);
            break;
        case 32:
            uiValue = pPen->uiPenColor;
            _DrawPixel32(uiValue, uiPixelX, uiPixelY, pFbmap);
            break;
        default:
            iErr = -5;
            break;
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӻ���ֱ��
  * ������
        1.unsigned int uiStartX:    ֱ����ʼ��ĺ�����
        2.unsigned int uiStartY:    ֱ����ʼ���������
        3.unsigned int uiEndX:      ֱ���յ�ĺ�����
        4.unsigned int uiEndY:      ֱ���յ��������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DrawLine(unsigned int uiStartX, unsigned int uiStartY, 
             unsigned int uiEndX, unsigned int uiEndY)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Start, End;
    unsigned int uiValue;

    if (iErr == 0)
    {
        //�õ���ǰ��ɫ������ж��Ƿ�Ϊ��Чָ��
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�õ���ǰ���ʶ����ж��Ƿ�Ϊ��Чָ��
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {    
        //�õ���ǰ֡����ӳ������ж��Ƿ�Ϊ��Чָ��
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {    
        //ȷ����ʼ�����յ�
        Start.x = uiStartX;
        Start.y = uiStartY;
        End.x = uiEndX;
        End.y = uiEndY;
        if (uiStartY == uiEndY) //���������Ϊˮƽ�ߣ�Ҫ��End.x>Start.x
        {
            Start.x = (uiStartX < uiEndX) ? uiStartX : uiEndX;
            End.x = (uiEndX > uiStartX) ? uiEndX : uiStartX;
        }
        if (uiStartX == uiEndX) //���������Ϊ��ֱ�ߣ�Ҫ��End.y>Start.y
        {
            Start.y = (uiStartY < uiEndY) ? uiStartY : uiEndY;
            End.y = (uiEndY > uiStartY) ? uiEndY : uiStartY;
        }
        //�ж���ʼ��λ�����յ�λ���Ƿ�Խ��
        if ((Start.x >= pFbmap->uiHorRes) || 
            (Start.y >= pFbmap->uiVerRes) || 
            (End.x >= pFbmap->uiHorRes) || 
            (End.y >= pFbmap->uiVerRes))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //����֡����������ȣ��õ�������ʾ����ɫֵ��������Ӧ�������л���
        switch (pFbmap->uiPixelBits)
        {
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8λɫ
            if (uiStartY == uiEndY)         //��������ȣ���ˮƽ��
            {
                _DrawHor8(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //��������ȣ�����ֱ��
            {
                _DrawVer8(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //�ᡢ�����궼����ȣ���б��
                (uiStartY != uiEndY))
            {
                _DrawBias8(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            break;
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            if (uiStartY == uiEndY)         //��������ȣ���ˮƽ��
            {
                _DrawHor16(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //��������ȣ�����ֱ��
            {
                _DrawVer16(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //�ᡢ�����궼����ȣ���б��
                (uiStartY != uiEndY))
            {
                _DrawBias16(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            break;
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            if (uiStartY == uiEndY)         //��������ȣ���ˮƽ��
            {
                _DrawHor24(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //��������ȣ�����ֱ��
            {
                _DrawVer24(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //�ᡢ�����궼����ȣ���б��
                (uiStartY != uiEndY))
            {
                _DrawBias24(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            break;
        case 32:
            uiValue = pPen->uiPenColor;
            if (uiStartY == uiEndY)         //��������ȣ���ˮƽ��
            {
                _DrawHor32(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //��������ȣ�����ֱ��
            {
                _DrawVer32(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //�ᡢ�����궼����ȣ���б��
                (uiStartY != uiEndY))
            {
                _DrawBias32(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            break;
        default:
            iErr = -5;
            break;
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӻ��ƾ���
  * ������
        1.unsigned int uiStartX:    ������ʼ��ĺ�����
        2.unsigned int uiStartY:    ������ʼ���������
        3.unsigned int uiEndX:      �����յ�ĺ�����
        4.unsigned int uiEndY:      �����յ��������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DrawRectangle(unsigned int uiStartX, unsigned int uiStartY, 
                  unsigned int uiEndX, unsigned int uiEndY)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Start, End;
    unsigned int uiValue;

    if (iErr == 0)
    {
        //�õ���ǰ��ɫ������ж��Ƿ�Ϊ��Чָ��
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�õ���ǰ���ʶ����ж��Ƿ�Ϊ��Чָ��
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {    
        //�õ���ǰ֡����ӳ������ж��Ƿ�Ϊ��Чָ��
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {    
        //ȷ����ʼ�����յ�
        Start.x = (uiStartX < uiEndX) ? uiStartX : uiEndX;
        Start.y = (uiStartY < uiEndY) ? uiStartY : uiEndY;
        End.x = (uiEndX > uiStartX) ? uiEndX : uiStartX;
        End.y = (uiEndY > uiStartY) ? uiEndY : uiStartY;
        //�ж���ʼ��λ�����յ�λ���Ƿ�Խ��
        if ((Start.x >= pFbmap->uiHorRes) || 
            (Start.y >= pFbmap->uiVerRes) || 
            (End.x >= pFbmap->uiHorRes) || 
            (End.y >= pFbmap->uiVerRes))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //����֡����������ȣ��õ�������ʾ����ɫֵ��������Ӧ�������л���
        switch (pFbmap->uiPixelBits)
        {
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8λɫ
            _DrawHor8(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor8(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer8(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer8(uiValue, End.x, Start.y, End.y, pFbmap);
            break;
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            _DrawHor16(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor16(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer16(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer16(uiValue, End.x, Start.y, End.y, pFbmap);
            break;
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            _DrawHor24(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor24(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer24(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer24(uiValue, End.x, Start.y, End.y, pFbmap);
            break;
        case 32:
            uiValue = pPen->uiPenColor;
            _DrawHor32(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor32(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer32(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer32(uiValue, End.x, Start.y, End.y, pFbmap);
            break;
        default:
            iErr = -5;
            break;
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӻ���Բ��
  * ������
        1.unsigned int uiPointX:    ԭ��ĺ�����
        2.unsigned int uiPointY:    ԭ���������
        3.unsigned int uiCircleR:   Բ�İ뾶
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DrawCircle(unsigned int uiPointX, unsigned int uiPointY, 
               unsigned int uiCircleR)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    unsigned int uiValue;

    if (iErr == 0)
    {
        //�õ���ǰ��ɫ������ж��Ƿ�Ϊ��Чָ��
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�õ���ǰ���ʶ����ж��Ƿ�Ϊ��Чָ��
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {    
        //�õ���ǰ֡����ӳ������ж��Ƿ�Ϊ��Чָ��
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {    
        //�ж�ԭ��λ����뾶�Ƿ�Խ��
        if ((uiPointX < uiCircleR) || 
            (uiPointY < uiCircleR) || 
            (uiPointX + uiCircleR >= pFbmap->uiHorRes) || 
            (uiPointY + uiCircleR >= pFbmap->uiVerRes))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //����֡����������ȣ��õ�������ʾ����ɫֵ��������Ӧ�������л���
        switch (pFbmap->uiPixelBits)
        {
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8λɫ
            _DrawCircle8(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            break;
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            _DrawCircle16(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            break;
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            _DrawCircle24(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            break;
        case 32:
            uiValue = pPen->uiPenColor;
            _DrawCircle32(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            break;
        default:
            iErr = -5;
            break;
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӻ��ƿ�
  * ������
        1.unsigned int uiStartX:    ����ʼ��ĺ�����
        2.unsigned int uiStartY:    ����ʼ���������
        3.unsigned int uiEndX:      ���յ�ĺ�����
        4.unsigned int uiEndY:      ���յ��������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DrawBlock(unsigned int uiStartX, unsigned int uiStartY, 
              unsigned int uiEndX, unsigned int uiEndY)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Start, End;
    unsigned int uiValue;

    if (iErr == 0)
    {
        //�õ���ǰ��ɫ������ж��Ƿ�Ϊ��Чָ��
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�õ���ǰ���ʶ����ж��Ƿ�Ϊ��Чָ��
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {    
        //�õ���ǰ֡����ӳ������ж��Ƿ�Ϊ��Чָ��
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {    
        //ȷ����ʼ�����յ�
        Start.x = (uiStartX < uiEndX) ? uiStartX : uiEndX;
        Start.y = (uiStartY < uiEndY) ? uiStartY : uiEndY;
        End.x = (uiEndX > uiStartX) ? uiEndX : uiStartX;
        End.y = (uiEndY > uiStartY) ? uiEndY : uiStartY;
        //�ж���ʼ��λ�����յ�λ���Ƿ�Խ��
        if ((Start.x >= pFbmap->uiHorRes) || 
            (Start.y >= pFbmap->uiVerRes) || 
            (End.x >= pFbmap->uiHorRes) || 
            (End.y >= pFbmap->uiVerRes))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //����֡����������ȣ��õ�������ʾ����ɫֵ��������Ӧ�������л���
        switch (pFbmap->uiPixelBits)
        {
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8λɫ
            _DrawBlock8(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            break;
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            _DrawBlock16(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            break;
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            _DrawBlock24(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            break;
        case 32:
            uiValue = pPen->uiPenColor;
            _DrawBlock32(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            break;
        default:
            iErr = -5;
            break;
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ���Ƶ������أ�Ӧ����8λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ   
        2.UINT32 pixel_x:   ��ĺ�����
        3.UINT32 pixel_y:   ���������
        4.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawPixel8(COLOR value,
                 UINT32 pixel_x, UINT32 pixel_y,
                 GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;

    p += pixel_x + pixel_y * pfbmap->uiHorRes;
    *p = d;

    return;
}


/***
  * ���ܣ�
       ���Ƶ������أ�Ӧ����16λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ   
        2.UINT32 pixel_x:   ��ĺ�����
        3.UINT32 pixel_y:   ���������
        4.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawPixel16(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;

    p += pixel_x + pixel_y * pfbmap->uiHorRes;
    *p = d;

    return;
}


/***
  * ���ܣ�
       ���Ƶ������أ�Ӧ����24λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ   
        2.UINT32 pixel_x:   ��ĺ�����
        3.UINT32 pixel_y:   ���������
        4.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawPixel24(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (pixel_x + pixel_y * pfbmap->uiHorRes) * 3;
    *p++ = b;   //С�˴洢��B����:byte0
    *p++ = g;   //С�˴洢��G����:byte1
    *p++ = r;   //С�˴洢��R����:byte2

    return ;
}


/***
  * ���ܣ�
       ���Ƶ������أ�Ӧ����32λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ   
        2.UINT32 pixel_x:   ��ĺ�����
        3.UINT32 pixel_y:   ���������
        4.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawPixel32(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;

    p += pixel_x + pixel_y * pfbmap->uiHorRes;
    *p = d;

    return ;
}


/***
  * ���ܣ�
       ����ˮƽ�ߣ�Ӧ����8λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawHor8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_x, 
               GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x  + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p++ = d;
    }

    return;
}


/***
  * ���ܣ�
       ����ˮƽ�ߣ�Ӧ����16λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawHor16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x  + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p++ = d;
    }
 
    return;
}


/***
  * ���ܣ�
       ����ˮƽ�ߣ�Ӧ����24λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawHor24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x + 1;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    while (loop--)
    {
        *p++ = b;   //С�˴洢��B����:byte0
        *p++ = g;   //С�˴洢��G����:byte1
        *p++ = r;   //С�˴洢��R����:byte2
    }

    return;
}


/***
  * ���ܣ�
       ����ˮƽ�ߣ�Ӧ����32λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawHor32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p++ = d;
    }

    return;
}


/***
  * ���ܣ�
        ���ƴ�ֱ�ߣ�Ӧ����8λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_y:     �������������
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
 * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawVer8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_y, 
               GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    { 
        *p = d;
        p += pfbmap->uiHorRes;
    }

    return;
}


/***
  * ���ܣ�
        ���ƴ�ֱ�ߣ�Ӧ����16λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_y:     �������������
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawVer16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap)
{
    UINT16 d =(UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;
    
    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p = d;
        p += pfbmap->uiHorRes;
    }

    return;
}


/***
  * ���ܣ�
        ���ƴ�ֱ�ߣ�Ӧ����24λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_y:     �������������
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawVer24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap)
{
    UINT32 d =(UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    while (loop--)
    {
        *(p + 0) = b;   //С�˴洢��B����:byte0
        *(p + 1) = g;   //С�˴洢��G����:byte1
        *(p + 2) = r;   //С�˴洢��R����:byte2
        p += pfbmap->uiHorRes * 3;
    }

    return;
}


/***
  * ���ܣ�
        ���ƴ�ֱ�ߣ�Ӧ����32λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_y:     �������������
        5.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawVer32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap)
 {
    UINT32 d =(UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p = d;
        p += pfbmap->uiHorRes;
    }

    return;
}


/***
  * ���ܣ�
        ����б�ߣ�Ӧ����8λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.UINT32 end_y:     �������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBias8(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, UINT32 end_y, 
                GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;

    if ((dy = end_y - start_y) > 0)
    {
        t = 1;
        p += start_x + start_y * pfbmap->uiHorRes;
    }
    else
    {   
        dy = -dy;
        t = -1;
        p += end_x + end_y * pfbmap->uiHorRes;
    }
    if ((dx = end_x - start_x) < 0)
    {
        dx = -dx ;
        t = -t;
    }
    if (dx >dy)
    {
        sx = t;
        sy = pfbmap->uiHorRes;
    }
    else
    {
        sx = pfbmap->uiHorRes;
        sy = t;
        t = dx;
        dx = dy;
        dy = t;
    }
    for (t=0, e= -dx; t<= dx; t++)
    {
        *p = d;
        p += sx;

        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}


/***
  * ���ܣ�
        ����б�ߣ�Ӧ����16λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.UINT32 end_y:     �������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBias16(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;

    if ((dy = end_y - start_y) >0)
    {
        t = 1;
        p += start_x + start_y * pfbmap->uiHorRes;
    }
    else
    {
        dy = -dy;
        t = -1;
        p += end_x + end_y * pfbmap->uiHorRes;
    }
    if ((dx = end_x - start_x) < 0 )
    {
        dx = -dx ;
        t = -t;
    }
    if (dx > dy)
    {
        sx = t;
        sy = pfbmap->uiHorRes;
    }
    else
    {
        sx = pfbmap->uiHorRes;
        sy = t;
        t = dx;
        dx = dy;
        dy = t;
    }
    for (t=0, e= -dx; t <= dx; t++)
    {
        *p = d;
        p += sx;

        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}


/***
  * ���ܣ�
        ����б�ߣ�Ӧ����24λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.UINT32 end_y:     �������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBias24(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    if ((dy = end_y - start_y) > 0)
    {
        t = 1;
        p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    }
    else
    {
        dy = -dy;
        t = -1;
        p += (end_x + end_y * pfbmap->uiHorRes) * 3;
    }
    if ((dx = end_x - start_x) < 0)
    {
        dx = -dx;
        t = -t;
    }
    if (dx > dy)
    {
        sx = t * 3;
        sy = pfbmap->uiHorRes * 3;
    }
    else
    {
        sx = pfbmap->uiHorRes * 3;
        sy = t * 3;
        t = dx;
        dx = dy;
        dy = t;
    }
    for (t = 0, e = -dx; t <= dx; t++)
    {
        *(p + 0) = b;   //С�˴洢��B����:byte0
        *(p + 1) = g;   //С�˴洢��G����:byte1
        *(p + 2) = r;   //С�˴洢��R����:byte2
        p += sx;
        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}


/***
  * ���ܣ�
        ����б�ߣ�Ӧ����32λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ��ʼ��ĺ�����
        3.UINT32 start_y:   ��ʼ���������
        4.UINT32 end_x:     ������ĺ�����
        5.UINT32 end_y:     �������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBias32(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;

    if ((dy = end_y - start_y) >0)
    {
        t = 1;
        p += start_x + start_y * pfbmap->uiHorRes;
    }
    else
    {
        dy = -dy;
        t = -1;
        p += end_x + end_y * pfbmap->uiHorRes;
    }
    if ((dx = end_x - start_x) < 0 )
    {
        dx = -dx ;
        t = -t;
    }
    if (dx > dy)
    {
        sx = t;
        sy = pfbmap->uiHorRes;
    }
    else
    {
        sx = pfbmap->uiHorRes;
        sy = t;
        t = dx;
        dx = dy;
        dy =t;
    }
    for (t=0, e= -dx; t <= dx; t++)
    {
        *p = d;
        p += sx;

        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}


/***
  * ���ܣ�
       ���ƿ���Բ��Ӧ����8λɫ֡����
  * ������
       1.COLOR value:       ��ɫֵ
       2.UINT32 point_x:    Բ�ĺ�����
       3.UINT32 point_y:    Բ��������
       4.UINT32 radius:     Բ�뾶
       5.GUIFBMAP *pfbmap:  ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawCircle8(COLOR value,
                  UINT32 point_x, UINT32 point_y, 
                  UINT32 radius,
                  GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;

    p += point_x + point_y * pfbmap->uiHorRes;
    xp = 0;
    yp = radius * 256 ;
    dy = 320 - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = y * pfbmap->uiHorRes + x;
        sym2 = sym1 - 2 * x;
        sym3 = x * pfbmap->uiHorRes + y;
        sym4 = sym3 - 2 * y;

        *(p + sym1) = d;
        *(p + sym2) = d;
        *(p - sym1) = d;
        *(p - sym2) = d;
        *(p + sym3) = d;
        *(p + sym4) = d;
        *(p - sym3) = d;
        *(p - sym4) = d;

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}


/***
  * ���ܣ�
       ���ƿ���Բ��Ӧ����16λɫ֡����
  * ������
       1.COLOR value:       ��ɫֵ
       2.UINT32 point_x:    Բ�ĺ�����
       3.UINT32 point_y:    Բ��������
       4.UINT32 radius:     Բ�뾶
       5.GUIFBMAP *pfbmap:  ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawCircle16(COLOR value,
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius,
                   GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;

    p += point_x + point_y * pfbmap->uiHorRes;
    xp = 0;
    yp = radius * 256 ;
    dy = 320 - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = y * pfbmap->uiHorRes + x;
        sym2 = sym1 - 2 * x;
        sym3 = x * pfbmap->uiHorRes + y;
        sym4 = sym3 - 2 * y;

        *(p + sym1) = d;
        *(p + sym2) = d;
        *(p - sym1) = d;
        *(p - sym2) = d;
        *(p + sym3) = d;
        *(p + sym4) = d;
        *(p - sym3) = d;
        *(p - sym4) = d;

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}


/***
  * ���ܣ�
       ���ƿ���Բ��Ӧ����24λɫ֡����
  * ������
       1.COLOR value:       ��ɫֵ
       2.UINT32 point_x:    Բ�ĺ�����
       3.UINT32 point_y:    Բ��������
       4.UINT32 radius:     Բ�뾶
       5.GUIFBMAP *pfbmap:  ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawCircle24(COLOR value,
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius,
                   GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (point_x + point_y * pfbmap->uiHorRes) * 3;
    xp = 0;
    yp = radius * 256 ;
    dy = 320 - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = (y * pfbmap->uiHorRes + x) * 3;
        sym2 = sym1 - 2 * x * 3;
        sym3 = (x * pfbmap->uiHorRes + y) * 3;
        sym4 = sym3 - 2 * y * 3;

        *(p + sym1 + 0) = b;   //С�˴洢��B����:byte0
        *(p + sym1 + 1) = g;   //С�˴洢��G����:byte1
        *(p + sym1 + 2) = r;   //С�˴洢��R����:byte2
        *(p + sym2 + 0) = b;   //С�˴洢��B����:byte0
        *(p + sym2 + 1) = g;   //С�˴洢��G����:byte1
        *(p + sym2 + 2) = r;   //С�˴洢��R����:byte2
        *(p - sym1 + 0) = b;   //С�˴洢��B����:byte0
        *(p - sym1 + 1) = g;   //С�˴洢��G����:byte1
        *(p - sym1 + 2) = r;   //С�˴洢��R����:byte2
        *(p - sym2 + 0) = b;   //С�˴洢��B����:byte0
        *(p - sym2 + 1) = g;   //С�˴洢��G����:byte1
        *(p - sym2 + 2) = r;   //С�˴洢��R����:byte2
        *(p + sym3 + 0) = b;   //С�˴洢��B����:byte0
        *(p + sym3 + 1) = g;   //С�˴洢��G����:byte1
        *(p + sym3 + 2) = r;   //С�˴洢��R����:byte2
        *(p + sym4 + 0) = b;   //С�˴洢��B����:byte0
        *(p + sym4 + 1) = g;   //С�˴洢��G����:byte1
        *(p + sym4 + 2) = r;   //С�˴洢��R����:byte2
        *(p - sym3 + 0) = b;   //С�˴洢��B����:byte0
        *(p - sym3 + 1) = g;   //С�˴洢��G����:byte1
        *(p - sym3 + 2) = r;   //С�˴洢��R����:byte2
        *(p - sym4 + 0) = b;   //С�˴洢��B����:byte0
        *(p - sym4 + 1) = g;   //С�˴洢��G����:byte1
        *(p - sym4 + 2) = r;   //С�˴洢��R����:byte2

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}


/***
  * ���ܣ�
       ���ƿ���Բ��Ӧ����32λɫ֡����
  * ������
       1.COLOR value:       ��ɫֵ
       2.UINT32 point_x:    Բ�ĺ�����
       3.UINT32 point_y:    Բ��������
       4.UINT32 radius:     Բ�뾶
       5.GUIFBMAP *pfbmap:  ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawCircle32(COLOR value,
                   UINT32 point_x, UINT32 point_y,
                   UINT32 radius,
                   GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;

    p += point_x + point_y * pfbmap->uiHorRes;
    xp = 0;
    yp = radius * 256 ;
    dy = 320 - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = y * pfbmap->uiHorRes + x;
        sym2 = sym1 - 2 * x;
        sym3 = x * pfbmap->uiHorRes + y;
        sym4 = sym3 - 2 * y;

        *(p + sym1) = d;
        *(p + sym2) = d;
        *(p - sym1) = d;
        *(p - sym2) = d;
        *(p + sym3) = d;
        *(p + sym4) = d;
        *(p - sym3) = d;
        *(p - sym4) = d;

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}


/***
  * ���ܣ�
        �������飬Ӧ����8λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ����ʼ��ĺ�����
        3.UINT32 start_y:   ����ʼ���������
        4.UINT32 end_x:     �������ĺ�����
        5.UINT32 end_y:     ��������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBlock8(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;

    p += start_x + start_y * pfbmap->uiHorRes;
    offset = pfbmap->uiHorRes - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = d;
        }
        p += offset;
        w= t;
    }

    return;
}


/***
  * ���ܣ�
        �������飬Ӧ����16λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ����ʼ��ĺ�����
        3.UINT32 start_y:   ����ʼ���������
        4.UINT32 end_x:     �������ĺ�����
        5.UINT32 end_y:     ��������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBlock16(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;

    p += start_x + start_y * pfbmap->uiHorRes;
    offset = pfbmap->uiHorRes - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = d;
        }
        p += offset;
        w= t;
    }

    return;
}


/***
  * ���ܣ�
        �������飬Ӧ����24λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ����ʼ��ĺ�����
        3.UINT32 start_y:   ����ʼ���������
        4.UINT32 end_x:     �������ĺ�����
        5.UINT32 end_y:     ��������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
       ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBlock24(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    offset = (pfbmap->uiHorRes - w) * 3;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = b;   //С�˴洢��B����:byte0
            *p++ = g;   //С�˴洢��G����:byte1
            *p++ = r;   //С�˴洢��R����:byte2
        }
        p += offset;
        w = t;
    }

    return;
}


/***
  * ���ܣ�
        �������飬Ӧ����32λɫ֡����
  * ������
        1.COLOR value:      ��ɫֵ
        2.UINT32 start_x:   ����ʼ��ĺ�����
        3.UINT32 start_y:   ����ʼ���������
        4.UINT32 end_x:     �������ĺ�����
        5.UINT32 end_y:     ��������������
        6.GUIFBMAP *pfbmap: ���Ʋ�������Ե�֡����ӳ�����
  * ���أ�
        ��
  * ��ע��
        ����ʹ��ʵ������ϵ�����߿�Ϊ1
***/
void _DrawBlock32(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;

    p += start_x + start_y * pfbmap->uiHorRes;
    offset = pfbmap->uiHorRes - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = d;
        }
        p += offset;
        w = t;
    }

    return;
}

