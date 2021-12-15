/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guilabel.c
* ժ    Ҫ��  ʵ��GUI�ı�ǩ���ͼ�������ΪGUI��ǩ�ؼ��ṩ�������װ��
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guilabel.h"


/*************************************
* Ϊʵ��GUILABEL����Ҫ���õ�����ͷ�ļ�
*************************************/
//#include ""


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӽ�����ǩ����
  * ������
        1.unsigned int uiPlaceX:        ��ǩˮƽ����λ�ã������Ͻ�Ϊ����
        2.unsigned int uiPlaceY:        ��ǩ��ֱ����λ�ã������Ͻ�Ϊ����
        3.unsigned int uiLblWidth:      ��ǩˮƽ���
        4.unsigned int uiLblHeight:     ��ǩ��ֱ�߶�
        5.unsigned short *pTextData:    ���ڽ����ı���Դ���ı����ݣ���Ϊ��
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUILABEL* CreateLabel(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                      unsigned int uiLblWidth, unsigned int uiLblHeight, 
                      unsigned short *pTextData)
{
    //�����־������ֵ����
    int iErr = 0;
    GUILABEL *pLblObj = NULL;

    if (iErr == 0)
    {
        //�ж�uiLblWidth��uiLblHeight�Ƿ���Ч
        if (uiLblWidth < 1 || uiLblHeight < 1)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pLblObj = (GUILABEL *) malloc(sizeof(GUILABEL));
        if (NULL == pLblObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //���ָ�����ı����ݣ����Խ�����ǩ�е��ı���Դ
        if (NULL == pTextData)
        {
            pLblObj->pLblText = NULL;
        }
        else
        {
            pLblObj->pLblText = CreateText(pTextData, uiPlaceX, uiPlaceY);
            if (NULL == pLblObj->pLblText)
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //��ǩ���������Ϣ��ֵ
        pLblObj->Visible.Area.Start.x = uiPlaceX;
        pLblObj->Visible.Area.Start.y = uiPlaceY;
        pLblObj->Visible.Area.End.x = uiPlaceX + uiLblWidth - 1;
        pLblObj->Visible.Area.End.y = uiPlaceY + uiLblHeight - 1;
        pLblObj->Visible.iEnable = 1;   //Ĭ�Ͽɼ�
        pLblObj->Visible.iFocus = 0;    //Ĭ���޽���
        pLblObj->Visible.iLayer = 0;    //Ĭ��ͼ��Ϊ0(��ײ�)
        //���ñ�ǩ�ı��Ķ��뷽ʽ
        pLblObj->iLblAlign = GUILABEL_ALIGN_LEFT;   //Ĭ�Ͽ������
        //����ı���Դ�ѽ����������ı�����ʾ��С
        if (NULL != pLblObj->pLblText)
        {
            pLblObj->pLblText->uiViewWidth = uiLblWidth;
            pLblObj->pLblText->uiViewHeight = uiLblHeight;
        }
        //��ǩ����Ļ�ˢ�����ʼ���������
        pLblObj->pLblBrush = NULL;      //Ĭ��ʹ�õ�ǰ��ˢ
        pLblObj->pLblPen = NULL;        //Ĭ��ʹ�õ�ǰ����
        pLblObj->pLblFont = NULL;       //Ĭ��ʹ�õ�ǰ����
    }

    //������
    switch (iErr)
    {
    case -3:
        free(pLblObj);
    case -2:
    case -1:
        pLblObj = NULL;
    default:
        break;
    }

    return pLblObj;
}


/***
  * ���ܣ�
        ɾ����ǩ����
  * ������
        1.GUILABEL **ppLblObj:  ָ���ָ�룬ָ����Ҫ���ٵı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyLabel(GUILABEL **ppLblObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppLblObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppLblObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppLblObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppLblObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //����ı���Դ�ѽ��������������ı���Դ
        if (NULL != (*ppLblObj)->pLblText)
        {
            if (DestroyText(&((*ppLblObj)->pLblText)))
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //���ٱ�ǩ���󣬲���ָ���ÿ�
        free(*ppLblObj);
        *ppLblObj = NULL;
    }

    return iErr;
}


/***
  * ���ܣ�
        ֱ�����ָ���ı�ǩ
  * ������
        1.GUILABEL *pLblObj:    ��ǩָ�룬����Ϊ��
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DisplayLabel(GUILABEL *pLblObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    unsigned int uiX;
    GUIBRUSH *pBrush = NULL;
    GUIPEN *pPen = NULL;
    GUIFONT *pFont = NULL;

    if (iErr == 0)
    {
        //�ж�pLblObj�Ƿ�Ϊ��Чָ��
        if (NULL == pLblObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�pLblObj�Ƿ�ɼ����ı���Դ�Ƿ��ѽ���
        if (!pLblObj->Visible.iEnable || (NULL == pLblObj->pLblText))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //���ı�������Դδװ�أ�����װ���ı�������Դ
        if (NULL == pLblObj->pLblText->ppTextMatrix)
        {
            if (LoadTextMatrix(pLblObj->pLblText))
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //���ݱ�ǩ�ı��Ķ��뷽ʽ�������ı��ķ���λ��
        switch (pLblObj->iLblAlign)
        {
        case GUILABEL_ALIGN_LEFT:
            //����룬�ı���ˮƽ����λ��Ϊ��ʼ�������
            if (SetTextPlace(pLblObj->Visible.Area.Start.x, 
                             pLblObj->Visible.Area.Start.y, 
                             pLblObj->pLblText))
            {
                iErr = -4;
            }
            break;
        case GUILABEL_ALIGN_RIGHT:
            //��ñ�ǩ����
            uiX = pLblObj->Visible.Area.End.x
                - pLblObj->Visible.Area.Start.x
                + 1;
            //����ǩ���ȳ����ı���ʾ��ȣ�����ƫ�ƣ�����ƫ��Ϊ0
            uiX = (uiX > pLblObj->pLblText->uiViewWidth) ? 
                  (uiX - pLblObj->pLblText->uiViewWidth) : 0;
            //�Ҷ��룬�ı���ˮƽ����λ��Ϊ��ʼ��������ƫ��
            if (SetTextPlace(pLblObj->Visible.Area.Start.x + uiX, 
                             pLblObj->Visible.Area.Start.y, 
                             pLblObj->pLblText))
            {
                iErr = -4;
            }
            break;
        case GUILABEL_ALIGN_CENTER:
            //��ñ�ǩ����
            uiX = pLblObj->Visible.Area.End.x
                - pLblObj->Visible.Area.Start.x
                + 1;
            //����ǩ���ȳ����ı���ʾ��ȣ�����ƫ�ƣ�����ƫ��Ϊ0
            uiX = (uiX > pLblObj->pLblText->uiViewWidth) ? 
                  (uiX - pLblObj->pLblText->uiViewWidth) : 0;
            //�Ҷ��룬�ı���ˮƽ����λ��Ϊ��ʼ��������ƫ�Ƶ�һ��
            if (SetTextPlace(pLblObj->Visible.Area.Start.x + (uiX / 2), 
                             pLblObj->Visible.Area.Start.y, 
                             pLblObj->pLblText))
            {
                iErr = -4;
            }
            break;
        default:
            iErr = -4;
            break;
        }
    }

    if (iErr == 0)
    {
        //�������ñ�ǩʹ�õĻ�ˢ�����赱ǰ��ˢ
        if (NULL != pLblObj->pLblBrush)
        {
            pBrush = GetCurrBrush();
            SetCurrBrush(pLblObj->pLblBrush);
        }
        //�������ñ�ǩʹ�õĻ��ʣ����赱ǰ����
        if (NULL != pLblObj->pLblPen)
        {
            pPen = GetCurrPen();
            SetCurrPen(pLblObj->pLblPen);
        }
        //�������ñ�ǩʹ�õ����壬���赱ǰ����
        if (NULL != pLblObj->pLblFont)
        {
            pFont = GetCurrFont();
            SetCurrFont(pLblObj->pLblFont);
        }
        //������ʾ�ı���Դ
        if (DisplayText(pLblObj->pLblText))
        {
            iErr = -5;
        }
        //�������ñ�ǩʹ�õĻ�ˢ���ָ���ǰ��ˢ
        if (NULL != pLblObj->pLblBrush)
        {
            SetCurrBrush(pBrush);
        }
        //�������ñ�ǩʹ�õĻ��ʣ��ָ���ǰ����
        if (NULL != pLblObj->pLblPen)
        {
            SetCurrPen(pPen);
        }
        //�������ñ�ǩʹ�õ����壬�ָ���ǰ����
        if (NULL != pLblObj->pLblFont)
        {
            SetCurrFont(pFont);
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ���ñ�ǩ����Ч����
  * ������
        1.unsigned int uiStartX:    ��ǩ��Ч�������ϽǺ����꣬�����Ͻ�Ϊ����
        2.unsigned int uiStartY:    ��ǩ��Ч�������Ͻ������꣬�����Ͻ�Ϊ����
        3.unsigned int uiEndX:      ��ǩ��Ч�������½Ǻ����꣬�����Ͻ�Ϊ����
        4.unsigned int uiEndY:      ��ǩ��Ч�������½������꣬�����Ͻ�Ϊ����
        5.GUILABEL *pLblObj:        ��Ҫ������Ч����ı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetLabelArea(unsigned int uiStartX, unsigned int uiStartY, 
                 unsigned int uiEndX, unsigned int uiEndY,
                 GUILABEL *pLblObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�uiEndX��uiEndY��pLblObj�Ƿ���Ч
        if (uiEndX < uiStartX || uiEndY < uiStartY || NULL == pLblObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //��ǩ������Ч����ֵ
        pLblObj->Visible.Area.Start.x = uiStartX;
        pLblObj->Visible.Area.Start.y = uiStartY;
        pLblObj->Visible.Area.End.x = uiEndX;
        pLblObj->Visible.Area.End.y = uiEndY;
        //����ı���Դ�ѽ����������ı�����ʾ��С������λ��
        if (NULL != pLblObj->pLblText)
        {
            SetTextView(uiEndX - uiStartX + 1, uiEndY - uiStartY + 1, 
                        pLblObj->pLblText);
            SetTextPlace(uiStartX, uiStartY, pLblObj->pLblText);
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ���ñ�ǩ�Ŀɼ���
  * ������
        1.int iEnable:          ��ǩ�Ƿ�ɼ���0���ɼ���1�ɼ�
        5.GUILABEL *pLblObj:    ��Ҫ���ÿɼ��Եı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetLabelEnable(int iEnable, GUILABEL *pLblObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�pLblObj�Ƿ���Ч
        if (NULL == pLblObj)
        {
            iErr = -1;
        }
    }

    pLblObj->Visible.iEnable = iEnable;

    return 0;
}


/***
  * ���ܣ�
        ���ñ�ǩ�ı��Ķ��뷽ʽ
  * ������
        1.int iLblAlign:        ��ǩ�ı����뷽ʽ��0��1�ң�2���У�Ĭ��0
        2.GUILABEL *pLblObj:    ��Ҫ���ö��뷽ʽ�ı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
//���ñ�ǩ�ı��Ķ��뷽ʽ
int SetLabelAlign(int iLblAlign, GUILABEL *pLblObj)
{
    //�ж�uiLblAlign��pLblObj�Ƿ���Ч
    if (iLblAlign > 2 || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->iLblAlign = iLblAlign;

    return 0;
}


/***
  * ���ܣ�
        ���ñ�ǩ����Ӧ���ı���Դ
  * ������
        1.unsigned short *pTextData:    ���ڽ����ı���Դ���ı�����
        2.GUILABEL *pLblObj:            ��Ҫ�����ı���Դ�ı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetLabelText(unsigned short *pTextData, GUILABEL *pLblObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������

    if (iErr == 0)
    {
        //�ж�pTextData��pLblObj�Ƿ���Ч
        if (NULL == pTextData || NULL == pLblObj)
        {
            return -1;
        }
    }

    if (iErr == 0)
    {
        //���ñ�ǩ����Ӧ���ı���Դ
        if (NULL == pLblObj->pLblText)  //�ı���Դδ���������Խ����ı���Դ
        {
            pLblObj->pLblText = CreateText(pTextData, 
                                           pLblObj->Visible.Area.Start.x, 
                                           pLblObj->Visible.Area.Start.y);
            if (NULL == pLblObj->pLblText)
            {
                iErr = -2;
            }
        }
        else                            //�ı���Դ�ѽ��������������ı���Դ
        {
            if (SetTextData(pTextData, pLblObj->pLblText))
            {
                iErr = -2;
            }
        }
    }

    if (iErr == 0)
    {
        //�����ı�����ʾ��С
        pLblObj->pLblText->uiViewWidth = pLblObj->Visible.Area.End.x
                                       - pLblObj->Visible.Area.Start.x + 1;
        pLblObj->pLblText->uiViewHeight = pLblObj->Visible.Area.End.y 
                                        - pLblObj->Visible.Area.Start.y + 1;
    }

    return iErr;
}


/***
  * ���ܣ�
        ���ñ�ǩ��ʹ�õĻ�ˢ
  * ������
        1.GUIBRUSH *pLblBrush:  ���ڱ�ǩ����Ļ�ˢ
        2.GUILABEL *pLblObj:    ��Ҫ������ʹ�û�ˢ�ı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetLabelBrush(GUIBRUSH *pLblBrush, GUILABEL *pLblObj)
{
    //�ж�pLblBrush��pLblObj�Ƿ���Ч
    if (NULL == pLblBrush || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->pLblBrush = pLblBrush;

    return 0;
}


/***
  * ���ܣ�
        ���ñ�ǩ��ʹ�õĻ���
  * ������
        1.GUIPEN *pLblPen:      ���ڱ�ǩ����Ļ���
        2.GUILABEL *pLblObj:    ��Ҫ������ʹ�û��ʵı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetLabelPen(GUIPEN *pLblPen, GUILABEL *pLblObj)
{
    //�ж�pLblPen��pLblObj�Ƿ���Ч
    if (NULL == pLblPen || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->pLblPen = pLblPen;

    return 0;
}


/***
  * ���ܣ�
        ���ñ�ǩ����Ӧ������
  * ������
        1.GUIFONT *pLblFont:    ���ڱ�ǩ���������
        2.GUILABEL *pLblObj:    ��Ҫ������ʹ������ı�ǩ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetLabelFont(GUIFONT *pLblFont, GUILABEL *pLblObj)
{
    //�ж�pLblFont��pLblObj�Ƿ���Ч
    if (NULL == pLblFont || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->pLblFont = pLblFont;

    return 0;
}

