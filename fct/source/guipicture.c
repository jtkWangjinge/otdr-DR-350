/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guipicture.c
* ժ    Ҫ��  ʵ��GUI��ͼ�ο����ͼ�������ΪGUIͼ�ο�ؼ��ṩ�������װ��
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guipicture.h"


/***************************************
* Ϊʵ��GUIPICTURE����Ҫ���õ�����ͷ�ļ�
***************************************/
//#include ""


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӽ���ͼ�ο����
  * ������
        1.unsigned int uiPlaceX:    ͼ�ο�ˮƽ����λ�ã������Ͻ�Ϊ����
        2.unsigned int uiPlaceY:    ͼ�ο�ֱ����λ�ã������Ͻ�Ϊ����
        3.unsigned int uiPicWidth:  ͼ�ο�ˮƽ���
        4.unsigned int uiPicHeight: ͼ�ο�ֱ�߶�
        5.char *strBitmapFile:      ���ڽ���λͼ��Դ��λͼ�ļ�����Ϊ��
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIPICTURE* CreatePicture(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                          unsigned int uiPicWidth, unsigned int uiPicHeight, 
                          char *strBitmapFile)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIPICTURE *pPicObj = NULL;

    if (iErr == 0)
    {
        //�ж�uiPicWidth��uiPicHeight�Ƿ���Ч
        if (uiPicWidth < 1 || uiPicHeight < 1)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pPicObj = (GUIPICTURE *) malloc(sizeof(GUIPICTURE));
        if (NULL == pPicObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //���ָ����λͼ�ļ������Խ�����ǩ�е�λͼ��Դ
        if (NULL == strBitmapFile)
        {
            pPicObj->pPicBitmap = NULL;
        }
        else
        {
            pPicObj->pPicBitmap = CreateBitmap(strBitmapFile, uiPlaceX, uiPlaceY);
            if (NULL == pPicObj->pPicBitmap)
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //��ǩ���������Ϣ��ֵ
        pPicObj->Visible.Area.Start.x = uiPlaceX;
        pPicObj->Visible.Area.Start.y = uiPlaceY;
        pPicObj->Visible.Area.End.x = uiPlaceX + uiPicWidth - 1;
        pPicObj->Visible.Area.End.y = uiPlaceY + uiPicHeight - 1;
        pPicObj->Visible.iEnable = 1;   //Ĭ�Ͽɼ�
        pPicObj->Visible.iFocus = 0;    //Ĭ���޽���
        pPicObj->Visible.iLayer = 0;    //Ĭ��ͼ��Ϊ0(��ײ�)
        //���λͼ��Դ�ѽ���������λͼ����ʾ��С
        if (NULL != pPicObj->pPicBitmap)
        {
            pPicObj->pPicBitmap->uiViewWidth = uiPicWidth;
            pPicObj->pPicBitmap->uiViewHeight = uiPicHeight;
        }
        //��ǩ����Ļ�ˢ�����ʼ���������
        pPicObj->pPicBrush = NULL;      //Ĭ��ʹ�õ�ǰ��ˢ
        pPicObj->pPicPen = NULL;        //Ĭ��ʹ�õ�ǰ����
        pPicObj->pPicFont = NULL;       //Ĭ��ʹ�õ�ǰ����
    }

    //������
    switch (iErr)
    {
    case -3:
        free(pPicObj);
    case -2:
    case -1:
        pPicObj = NULL;
    default:
        break;
    }

    return pPicObj;
}


/***
  * ���ܣ�
        ɾ��ͼ�ο����
  * ������
        1.GUIPICTURE **ppPicObj:    ָ���ָ�룬ָ����Ҫ���ٵ�ͼ�ο����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyPicture(GUIPICTURE **ppPicObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppPicObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppPicObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppPicObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppPicObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //���λͼ��Դ�ѽ�������������λͼ��Դ
        if (NULL != (*ppPicObj)->pPicBitmap)
        {
            if (DestroyBitmap(&((*ppPicObj)->pPicBitmap)))
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //����ͼ�ο���󣬲���ָ���ÿ�
        free(*ppPicObj);
        *ppPicObj = NULL;
    }

    return iErr;
}


/***
  * ���ܣ�
        ֱ�����ָ����ͼ�ο�
  * ������
        1.GUIPICTURE *pPicObj:  ͼ�ο�ָ�룬����Ϊ��
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DisplayPicture(GUIPICTURE *pPicObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIBRUSH *pBrush = NULL;
    GUIPEN *pPen = NULL;
    GUIFONT *pFont = NULL;

    if (iErr == 0)
    {
        //�ж�pPicObj�Ƿ�Ϊ��Чָ��
        if (NULL == pPicObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�pPicObj�Ƿ�ɼ���λͼ��Դ�Ƿ��ѽ���
        if (!pPicObj->Visible.iEnable || (NULL == pPicObj->pPicBitmap))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //�������ñ�ǩʹ�õĻ�ˢ�����赱ǰ��ˢ
        if (NULL != pPicObj->pPicBrush)
        {
            pBrush = GetCurrBrush();
            SetCurrBrush(pPicObj->pPicBrush);
        }
        //�������ñ�ǩʹ�õĻ��ʣ����赱ǰ����
        if (NULL != pPicObj->pPicPen)
        {
            pPen = GetCurrPen();
            SetCurrPen(pPicObj->pPicPen);
        }
        //�������ñ�ǩʹ�õ����壬���赱ǰ����
        if (NULL != pPicObj->pPicFont)
        {
            pFont = GetCurrFont();
            SetCurrFont(pPicObj->pPicFont);
        }
        //������ʾλͼ��Դ
        if (DisplayBitmap(pPicObj->pPicBitmap))
        {
            iErr = -3;
        }
        //�������ñ�ǩʹ�õĻ�ˢ���ָ���ǰ��ˢ
        if (NULL != pPicObj->pPicBrush)
        {
            SetCurrBrush(pBrush);
        }
        //�������ñ�ǩʹ�õĻ��ʣ��ָ���ǰ����
        if (NULL != pPicObj->pPicPen)
        {
            SetCurrPen(pPen);
        }
        //�������ñ�ǩʹ�õ����壬�ָ���ǰ����
        if (NULL != pPicObj->pPicFont)
        {
            SetCurrFont(pFont);
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ����ͼ�ο����Ч����
  * ������
        1.unsigned int uiStartX:    ͼ�ο���Ч�������ϽǺ����꣬�����Ͻ�Ϊ����
        2.unsigned int uiStartY:    ͼ�ο���Ч�������Ͻ������꣬�����Ͻ�Ϊ����
        3.unsigned int uiEndX:      ͼ�ο���Ч�������½Ǻ����꣬�����Ͻ�Ϊ����
        4.unsigned int uiEndY:      ͼ�ο���Ч�������½������꣬�����Ͻ�Ϊ����
        5.GUIPICTURE *pPicObj:      ��Ҫ������Ч�����ͼ�ο����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetPictureArea(unsigned int uiStartX, unsigned int uiStartY, 
                   unsigned int uiEndX, unsigned int uiEndY,
                   GUIPICTURE *pPicObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�uiEndX��uiEndY��pPicObj�Ƿ���Ч
        if (uiEndX < uiStartX || uiEndY < uiStartY || NULL == pPicObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //ͼ�ο������Ч����ֵ
        pPicObj->Visible.Area.Start.x = uiStartX;
        pPicObj->Visible.Area.Start.y = uiStartY;
        pPicObj->Visible.Area.End.x = uiEndX;
        pPicObj->Visible.Area.End.y = uiEndY;
        //���λͼ��Դ�ѽ���������λͼ����ʾ��С������λ��
        if (NULL != pPicObj->pPicBitmap)
        {
            SetBitmapView(uiEndX - uiStartX + 1, uiEndY - uiStartY + 1, 
                          pPicObj->pPicBitmap);
            SetBitmapPlace(uiStartX, uiStartY, pPicObj->pPicBitmap);
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ����ͼ�ο�Ŀɼ���
  * ������
        1.int iEnable:          ͼ�ο��Ƿ�ɼ���0���ɼ���1�ɼ�
        5.GUIPICTURE *pPicObj:  ��Ҫ���ÿɼ��Ե�ͼ�ο����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetPictureEnable(int iEnable, GUIPICTURE *pPicObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�pPicObj�Ƿ���Ч
        if (NULL == pPicObj)
        {
            iErr = -1;
        }
    }

    pPicObj->Visible.iEnable = iEnable;

    return 0;
}


/***
  * ���ܣ�
        ����ͼ�ο�����Ӧ��λͼ��Դ
  * ������
        1.char *strBitmapFile:  ���ڽ���λͼ��Դ��λͼ�ļ�
        2.GUIPICTURE *pPicObj:  ��Ҫ����λͼ��Դ��ͼ�ο����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetPictureBitmap(char *strBitmapFile, GUIPICTURE *pPicObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�strBitmapFile��pPicObj�Ƿ���Ч
        if (NULL == strBitmapFile || NULL == pPicObj)
        {
            return -1;
        }
    }

    if (iErr == 0)
    {
        //���λͼ��Դ�ѽ������������پɵ�λͼ��Դ
        if (NULL != pPicObj->pPicBitmap)
        {
            if (DestroyBitmap(&(pPicObj->pPicBitmap)))
            {
                return -2;
            }
        }
    }

    if (iErr == 0)
    {
        //����ͼ�ο�����Ӧ��λͼ��Դ
        pPicObj->pPicBitmap = CreateBitmap(strBitmapFile, 
                                           pPicObj->Visible.Area.Start.x, 
                                           pPicObj->Visible.Area.Start.y);
        if (NULL == pPicObj->pPicBitmap)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //����λͼ����ʾ��С
        pPicObj->pPicBitmap->uiViewWidth = pPicObj->Visible.Area.End.x
                                         - pPicObj->Visible.Area.Start.x + 1;
        pPicObj->pPicBitmap->uiViewHeight = pPicObj->Visible.Area.End.y 
                                          - pPicObj->Visible.Area.Start.y + 1;
    }

    return iErr;
}


/***
  * ���ܣ�
        ����ͼ�ο���ʹ�õĻ�ˢ
  * ������
        1.GUIBRUSH *pPicBrush:  ����ͼ�ο����Ļ�ˢ
        2.GUIPICTURE *pPicObj:  ��Ҫ������ʹ�û�ˢ��ͼ�ο����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetPictureBrush(GUIBRUSH *pPicBrush, GUIPICTURE *pPicObj)
{
    //�ж�pPicBrush��pPicObj�Ƿ���Ч
    if (NULL == pPicBrush || NULL == pPicObj)
    {
        return -1;
    }

    pPicObj->pPicBrush = pPicBrush;

    return 0;
}


/***
  * ���ܣ�
        ����ͼ�ο���ʹ�õĻ���
  * ������
        1.GUIPEN *pPicPen:      ����ͼ�ο����Ļ���
        2.GUIPICTURE *pPicObj:  ��Ҫ������ʹ�û��ʵ�ͼ�ο����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetPicturePen(GUIPEN *pPicPen, GUIPICTURE *pPicObj)
{
    //�ж�pPicPen��pLblObj�Ƿ���Ч
    if (NULL == pPicPen || NULL == pPicObj)
    {
        return -1;
    }

    pPicObj->pPicPen = pPicPen;

    return 0;
}


/***
  * ���ܣ�
        ����ͼ�ο�����Ӧ������
  * ������
        1.GUIFONT *pPicFont:    ����ͼ�ο���������
        2.GUIPICTURE *pPicObj:  ��Ҫ������ʹ�������ͼ�ο����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetPictureFont(GUIFONT *pPicFont, GUIPICTURE *pPicObj)
{
    //�ж�pPicFont��pPicObj�Ƿ���Ч
    if (NULL == pPicFont || NULL == pPicObj)
    {
        return -1;
    }

    pPicObj->pPicFont = pPicFont;

    return 0;
}

