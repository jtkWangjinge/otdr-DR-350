/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guifont.c
* ժ    Ҫ��  ʵ��GUI���������ͼ�������ΪGUI��ͼģ���ʵ���ṩ������
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2020-10-29
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guifont.h"


/************************************
* Ϊʵ��GUIFONT����Ҫ���õ�����ͷ�ļ�
************************************/
//#include ""


/********************
* GUI�еĵ�ǰ�������
********************/
static GUIFONT *pCurrFont = NULL;


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӽ����������
  * ������
        1.char *strFontFile:            ���ڽ����������������ļ�����
        2.unsigned int uiFontWidth:     ����Ŀ�ȣ���λΪ����
        3.unsigned int uiFontHeight:    ����ĸ߶ȣ���λΪ����
        4.unsigned int uiFgColor:       ����ǰ��ɫ
        5.unsigned int uiBgColor:       ���屳��ɫ
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
//����ָ������Ϣֱ�ӽ����������
GUIFONT* CreateFont(char *strFontFile, 
                    unsigned int uiFontWidth, unsigned int uiFontHeight, 
                    unsigned int uiFgColor, unsigned int uiBgColor)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIFONT *pFontObj = NULL;
    //��ʱ��������
    unsigned int uiSize;
    GUIFONT_FILEHEADER fh;

    if (iErr == 0)
    {
        //�ж�strFontFile�Ƿ�Ϊ��Чָ��
        if (NULL == strFontFile)
        {
            iErr = -1;
        }
        //�ж�����Ŀ�Ⱥ͸߶��Ƿ���Ч
        if (uiFontWidth == 0 || uiFontHeight == 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pFontObj = (GUIFONT *) malloc(sizeof(GUIFONT));
        if (NULL == pFontObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //Ϊ���������ļ����Ʒ����ڴ���Դ
        uiSize = strlen(strFontFile) + 1;
        pFontObj->strFontFile = (char *) malloc(uiSize);
        if (NULL == pFontObj->strFontFile)
        {
            iErr = -3;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:open(),lseek(),read()
    if (iErr == 0)
    {
        //���Դ������ļ�
        pFontObj->iFontFd = open(strFontFile, O_RDONLY);
        if (pFontObj->iFontFd == -1)
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //���Զ�ȡ�����ļ�ͷ
        lseek(pFontObj->iFontFd, 0, SEEK_SET);
        if (32 != read(pFontObj->iFontFd, &fh, 32))
        {
            iErr = -5;
        }
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //��������ļ���ʽ
        if ((fh.uiFileType != 0x544E462E) || 
            (fh.uiFileFormat != GUIFONT_FILE_STD) || 
            fh.usSegmentFlag)
        {
            iErr = -6;
        }
    }

    if (iErr == 0)
    {
        //��������Ա��ֵ
        strcpy(pFontObj->strFontFile, strFontFile);
        pFontObj->uiFontWidth = uiFontWidth;
        pFontObj->uiFontHeight = uiFontHeight;
        pFontObj->uiLeftGap = 1;
        pFontObj->uiRightGap = 1;
        pFontObj->uiTopGap = 1;
        pFontObj->uiBottomGap = 1;
        pFontObj->uiFgColor = uiFgColor;
        pFontObj->uiBgColor = uiBgColor;
    }

    //������
    switch (iErr)
    {
    case -6:
    case -5:
        close(pFontObj->iFontFd);
    case -4:
    case -3:
        free(pFontObj);
    case -2:
    case -1:
        pFontObj = NULL;
    default:
        break;
    }

    return pFontObj;
}


/***
  * ���ܣ�
        ɾ���������
  * ������
        1.GUIFONT **ppFontObj:  ָ���ָ�룬ָ����Ҫ���ٵ��������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DestroyFont(GUIFONT **ppFontObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppFontObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppFontObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppFontObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppFontObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:close()
    if (iErr == 0)
    {
        //�ͷ���Դ
        close((*ppFontObj)->iFontFd);
        free((*ppFontObj)->strFontFile);
        //����������󣬲���ָ���ÿ�
        free(*ppFontObj);
        *ppFontObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iErr;
}


/***
  * ���ܣ�
        �õ���ǰ���������
  * ������
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIFONT* GetCurrFont(void)
{
    return pCurrFont;
}


/***
  * ���ܣ�
        ���õ�ǰ���������
  * ������
        1.GUIFONT *pFontObj:    ������Ϊ��ǰ�������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetCurrFont(GUIFONT *pFontObj)
{
    //�ж�pFontObj�Ƿ���Ч
    if (NULL == pFontObj)
    {
        return -1;
    }

    pCurrFont = pFontObj;

    return 0;
}


/***
  * ���ܣ�
        ��������ļ��
  * ������
        1.unsigned int uiLeftGap:   �������࣬��ֵ<=������/2
        2.unsigned int uiRightGap:  �����Ҽ�࣬��ֵ<=������/2
        3.unsigned int uiTopGap:    �����ϼ�࣬��ֵ<=����߶�/2
        4.unsigned int uiBottomGap: �����¼�࣬��ֵ<=����߶�/2
        5.GUIFONT *pFntObj:         ��Ҫ������������������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetFontGap(unsigned int uiLeftGap, unsigned int uiRightGap, 
               unsigned int uiTopGap, unsigned int uiBottomGap, 
               GUIFONT *pFontObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�pFontObj�Ƿ���Ч
        if (NULL == pFontObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�Ҫ���õ��������Ƿ����Ҫ��
        if ((uiLeftGap > pFontObj->uiFontWidth / 2) ||
            (uiRightGap > pFontObj->uiFontWidth / 2) ||
            (uiTopGap > pFontObj->uiFontHeight / 2) ||
            (uiBottomGap > pFontObj->uiFontHeight / 2))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //����������
        pFontObj->uiLeftGap = uiLeftGap;
        pFontObj->uiRightGap = uiRightGap;
        pFontObj->uiTopGap = uiTopGap;
        pFontObj->uiBottomGap = uiBottomGap;
    }

    return iErr;
}


/***
  * ���ܣ�
        ���������ǰ������ɫ
  * ������
        1.unsigned int uiFgColor:   ����ǰ��ɫ����������ɫ
        2.unsigned int uiBgColor:   ���屳��ɫ
        3.GUIFONT *pFntObj:         ��Ҫ����ǰ������ɫ���������
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetFontColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                 GUIFONT *pFontObj)
{
    //�ж�pFontObj�Ƿ���Ч
    if (NULL == pFontObj)
    {
        return -1;
    }

    pFontObj->uiFgColor = uiFgColor;
    pFontObj->uiBgColor = uiBgColor;

    return 0;
}


/***
  * ���ܣ�
        ��ָ�������������ȡ�ض�������������
  * ������
        1.unsigned short usMatrixCode:  ���������룬���ַ�����
        2.GUIFONT *pFntObj:             ��Ҫ������ȡ���������������
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIFONT_MATRIX* ExtractFontMatrix(unsigned short usMatrixCode, 
                                  GUIFONT *pFontObj)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIFONT_MATRIX *pReturn = NULL;
    //��ʱ��������
    GUIFONT_FILEHEADER fh;
    unsigned char *pBuf = NULL, *pMatrix = NULL;
    unsigned int uiFont, uiSize, uiTmp, uiLeft, uiRight, uiBit, uiGap;

    if (iErr == 0)
    {
        //�ж�pFontObj�Ƿ�Ϊ��Чָ��
        if (NULL == pFontObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pReturn = (GUIFONT_MATRIX *) malloc(sizeof(GUIFONT_MATRIX));
        if (NULL == pReturn)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:lseek(),read()
    if (iErr == 0)
    {
        //���Զ�ȡ�����ļ�ͷ
        lseek(pFontObj->iFontFd, 0, SEEK_SET);
        if (32 != read(pFontObj->iFontFd, &fh, 32))
        {
            iErr = -3;
        }
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //�ж�usMatrixCode�Ƿ��ܹ�����ȡ
        if (usMatrixCode >= fh.uiMatrixCount)   //���ֶε�����£������0��ʼ
        {
            iErr = -4;
        }
        //��������Ƿ�������Сһ��(Ŀǰ��δ֧������)
        if ((pFontObj->uiFontWidth != fh.uiMatrixWidth) || 
            (pFontObj->uiFontHeight != fh.uiMatrixHeight))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //����������ݵ�ÿ�����ݳ��ȣ�ע���������ÿ�а�8λ����
        uiFont = round_up(fh.uiMatrixWidth, 8) / 8;
        //����Ϊ��������еĵ������ݷ����ڴ�
        uiSize = uiFont * fh.uiMatrixHeight;
        pReturn->pMatrixData = (unsigned char *) malloc(uiSize);
        if (NULL == pReturn->pMatrixData)
        {
            iErr = -5;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:lseek(),read()
    if (iErr == 0)
    {
        //��ȡ�ļ��еĵ����������������
        lseek(pFontObj->iFontFd, 
              fh.uiDataOffset + usMatrixCode * (uiFont * fh.uiMatrixHeight), 
              SEEK_SET);
        read(pFontObj->iFontFd, 
             pReturn->pMatrixData, 
             uiFont * fh.uiMatrixHeight);
        //Ϊ�����������ʾ��ʵ�ʿ�ȳ��Կ����л��壬ע�⻺�������0
        pBuf = (unsigned char *) calloc(uiFont, 1);
        if (NULL == pBuf)
        {
            iErr = -6;
        }        
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //�Ե������ݰ��н���λ��������Ա�ͳ�����������ʾ��ʵ�ʿ��
        for (uiSize = 0; uiSize < uiFont; uiSize++)
        {
            pMatrix = pReturn->pMatrixData;
            for (uiTmp = 0; uiTmp < fh.uiMatrixHeight; uiTmp++)
            {
                pBuf[uiSize] |= pMatrix[uiSize];
                pMatrix += uiFont;
            }
        }
        //�õ������������հ׵Ĵ�С
        uiLeft = 0;
        for (uiSize = 0; uiSize < uiFont; uiSize++)  //��������
        {
            if (pBuf[uiSize])
            {
                uiTmp = pBuf[uiSize];
                for (uiBit = 0; uiBit < 8; uiBit++)
                {
                    if (uiTmp & 0x80)                   //bit7λ�������
                    {
                        break;
                    }
                    else
                    {
                        uiTmp <<= 1;
                        uiLeft++;
                    }
                }
                break;      //���հ��ѵõ�������ѭ��
            }
            uiLeft += 8;    //ÿ����һ�ֽ�����8��ѭ��������ֵ���ܳ���������
        }
        //�õ����������Ҳ�հ׵Ĵ�С
        uiRight = 0;
        for (uiSize = uiFont; uiSize > 0; uiSize--)  //��������
        {
            if (pBuf[uiSize - 1])
            {
                uiTmp = pBuf[uiSize - 1];
                for (uiBit = 0; uiBit < 8; uiBit++)
                {
                    if (uiTmp & 0x01)                   //bit0λ�����Ҳ�
                    {
                        break;
                    }
                    else
                    {
                        uiTmp >>= 1;
                        uiRight++;
                    }
                }
                break;      //�Ҳ�հ��ѵõ�������ѭ��
            }
            uiRight += 8;   //ÿ����һ�ֽ�����8��ѭ��������ֵ���ܳ���������
        }
        //����������հ��ҵ���Ϊ�ո񣬽��������ݿ����������հ�
        if ((uiLeft > 0) && 
            (uiLeft < fh.uiMatrixWidth))
        {
            uiBit = uiLeft & bitmask_low(3, 8); //����ʱ��Ҫ���Ƶ�λ��
            uiGap = round_down(uiLeft, 8) / 8;  //����ʱ��Ҫ�������ֽ���
            for (uiTmp = 0; uiTmp < fh.uiMatrixHeight; uiTmp++)
            {
                pMatrix = pReturn->pMatrixData + uiFont * uiTmp;
                for (uiSize = uiGap; uiSize < uiFont; uiSize++)
                {
                    pBuf[0] = pMatrix[uiSize] << uiBit;
                    pBuf[0] |= pMatrix[uiSize + 1] >> (8 - uiBit);
                    pMatrix[uiSize - uiGap] = pBuf[0];
                }
            }
        }
        //�ж��Ƿ�Ϊ�ո�֮��ĵ���(������ʾ�κε�)���������������ĳ�Ա����
        if ((uiLeft >= fh.uiMatrixWidth) &&     //�жϵ�������Ϊ>=uiMatrixWidth
            (uiRight >= fh.uiMatrixWidth))
        {
            pReturn->uiRealWidth = fh.uiMatrixWidth * 3 / 8;    //�˷�֮�����
            pReturn->uiRealHeight = fh.uiMatrixHeight;
        }
        else
        {
            pReturn->uiRealWidth = fh.uiMatrixWidth - uiLeft - uiRight;
            pReturn->uiRealHeight = fh.uiMatrixHeight;
        }
        //�ͷ��л���
        free(pBuf);
    }

    //������
    switch (iErr)
    {
    case -6:
        free(pReturn->pMatrixData);
    case -5:
    case -4:
    case -3:
        free(pReturn);
    case -2:
    case -1:
        pReturn = NULL;
    default:
        break;
    }

    return pReturn;
}

