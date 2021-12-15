/*******************************************************************************
* Copyright(c)2020����̫������(����)���޹�˾�Ϻ��з���
*
* All rights reserved
*
* �ļ����ƣ�  guibitmap.c
* ժ    Ҫ��  ʵ��GUIλͼ���ͼ���ز���������ʵ��BMPͼƬ�Ļ��ơ�
*
* ��ǰ�汾��  v1.0.0
* ��    �ߣ�  wjg
* ������ڣ�  2012-10-12
*
* ȡ���汾��
* ԭ �� �ߣ�
* ������ڣ�
*******************************************************************************/

#include "guibitmap.h"


/**************************************
* Ϊʵ��GUIBITMAP����Ҫ���õ�����ͷ�ļ�
**************************************/
#include "guiimage.h"


/***
  * ���ܣ�
        ����ָ������Ϣֱ�ӽ���λͼ����
  * ������
        1.char *strBitmapFile:      ���ڽ���λͼ�����λͼ�ļ�����
        2.unsigned int uiPlaceX:    λͼˮƽ����λ�ã������Ͻ�Ϊ����
        3.unsigned int uiPlaceY:    λͼ��ֱ����λ�ã������Ͻ�Ϊ����
  * ���أ�
        �ɹ�������Чָ�룬ʧ�ܷ���NULL
  * ��ע��
***/
GUIBITMAP* CreateBitmap(char *strBitmapFile, 
                        unsigned int uiPlaceX, unsigned int uiPlaceY)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIBITMAP *pBitmapObj = NULL;
    //��ʱ��������
    unsigned int uiSize;

    if (iErr == 0)
    {
        //�ж�strBitmapName�Ƿ�Ϊ��Чָ��
        if (NULL == strBitmapFile)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�����ڴ���Դ
        pBitmapObj = (GUIBITMAP *) malloc(sizeof(GUIBITMAP));
        if (NULL == pBitmapObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //Ϊ����λͼ�ļ����Ʒ����ڴ���Դ
        uiSize = strlen(strBitmapFile) + 1;
        pBitmapObj->strBitmapFile = (char *) malloc(uiSize);
        if (NULL == pBitmapObj->strBitmapFile)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //λͼ�����Ա��ֵ
        strcpy(pBitmapObj->strBitmapFile, strBitmapFile);
        pBitmapObj->iBitmapFd = -1;
        pBitmapObj->uiPlaceX = uiPlaceX;
        pBitmapObj->uiPlaceY = uiPlaceY;
        pBitmapObj->uiViewWidth = 0;    //Ĭ�ϰ�BMP�е�ʵ�ʿ����ʾ
        pBitmapObj->uiViewHeight = 0;   //Ĭ�ϰ�BMP�е�ʵ�ʸ߶���ʾ
        pBitmapObj->pBitmapImg = NULL;
    }

    //������
    switch (iErr)
    {
    case -3:
        free(pBitmapObj);
    case -2:
    case -1:
        pBitmapObj = NULL;
    default:
        break;
    }

    return pBitmapObj;
}


/***
  * ���ܣ�
        ɾ��λͼ����
  * ������
        1.GUIBITMAP **ppBitmapObj:  ָ���ָ�룬ָ����Ҫ���ٵ�λͼ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
        ����ɹ��������ָ�뽫���ÿ�
***/
int DestroyBitmap(GUIBITMAP **ppBitmapObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�ppBitmapObj�Ƿ�Ϊ��Чָ��
        if (NULL == ppBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�ppBitmapObj��ָ����Ƿ�Ϊ��Чָ��
        if (NULL == *ppBitmapObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //���λͼͼ����Դ��װ�أ�����ж��λͼͼ����Դ
        if (NULL != (*ppBitmapObj)->pBitmapImg)
        {
            if (UnloadBitmapImage(*ppBitmapObj))
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //�ͷ���Դ
        free((*ppBitmapObj)->strBitmapFile);
        //����λͼ���󣬲���ָ���ÿ�
        free(*ppBitmapObj);
        *ppBitmapObj = NULL;
    }

    return iErr;
}


/***
  * ���ܣ�
        ֱ�����ָ����λͼ
  * ������
        1.GUIBITMAP *pBitmapObj:    λͼָ�룬����Ϊ��
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int DisplayBitmap(GUIBITMAP *pBitmapObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�pBitmapObj�Ƿ�Ϊ��Чָ��
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //��ͼ����Դδװ�أ�����װ��λͼͼ����Դ
        if (NULL == pBitmapObj->pBitmapImg)
        {
            if (LoadBitmapImage(pBitmapObj))
            {
                iErr = -2;
            }
        }
    }

    if (iErr == 0)
    {
        //��ʾλͼͼ��
        if (DisplayImage(pBitmapObj->pBitmapImg))
        {
            iErr = -3;
        }
    }

    return iErr;
}


/***
  * ���ܣ�
        ����λͼ����ʾ��С
  * ������
        1.unsigned int uiViewWidth:     ��ʾ��ȣ�>=0����ʼ��Ϊ0��BMPʵ�ʿ��
        2.unsigned int uiViewHeight:    ��ʾ�߶ȣ�>=0����ʼ��Ϊ0��BMPʵ�ʸ߶�
        3.GUIBITMAP *pBitmapObj:        ��Ҫ������ʾ��С��λͼ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetBitmapView(unsigned int uiViewWidth, unsigned int uiViewHeight, 
                  GUIBITMAP *pBitmapObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�pBitmapObj�Ƿ���Ч
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //���λͼͼ����Դ��װ�أ�����ж��λͼͼ����Դ
        if (NULL != pBitmapObj->pBitmapImg)
        {
            if (UnloadBitmapImage(pBitmapObj))
            {
                iErr = -2;
            }
        }
    }

    if (iErr == 0)
    {
        //����λͼ����ʾ��С
        pBitmapObj->uiViewWidth = uiViewWidth;
        pBitmapObj->uiViewHeight = uiViewHeight;
    }

    return iErr;
}


/***
  * ���ܣ�
        ����λͼ�ķ���λ��
  * ������
        1.unsigned int uiPlaceX:    λͼˮƽ����λ�ã������Ͻ�Ϊ���㣬��������
        2.unsigned int uiPlaceY:    λͼ��ֱ����λ�ã������Ͻ�Ϊ���㣬��������
        3.GUIBITMAP *pBitmapObj:    ��Ҫ���÷���λ�õ�λͼ����
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int SetBitmapPlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                   GUIBITMAP *pBitmapObj)
{
    //�ж�pBitmapObj�Ƿ���Ч
    if (NULL == pBitmapObj)
    {
        return -1;
    }

    //���λͼͼ����Դ��װ�أ�����ͼ��ķ���λ��
    if (NULL != pBitmapObj->pBitmapImg)
    {
        if (SetImagePlace(uiPlaceX, uiPlaceY, pBitmapObj->pBitmapImg))
        {
            return -2;
        }
    }

    pBitmapObj->uiPlaceX = uiPlaceX;
    pBitmapObj->uiPlaceY = uiPlaceY;

    return 0;
}


/***
  * ���ܣ�
        װ��λͼͼ����Դ
  * ������
        1.GUIBITMAP *pBitmapObj:    λͼָ�룬����Ϊ��
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int LoadBitmapImage(GUIBITMAP *pBitmapObj)
{
    //�����־������ֵ����
    int iErr = 0;
    //��ʱ��������
    GUIBITMAP_FILEHEADER fh;
    GUIBITMAP_INFOHEADER ih;
    GUIPALETTE *pPal = NULL;
    unsigned char *pBuf = NULL, *pRes = NULL;
    unsigned int uiBmp, uiImg, uiHeight;

    if (iErr == 0)
    {
        //�ж�pBitmapObj�Ƿ�Ϊ��Чָ��
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�sBitmapName�Ƿ�Ϊ��Чָ���λͼ��Դ�Ƿ��Ѽ���
        if ((NULL == pBitmapObj->strBitmapFile) || 
            (NULL != pBitmapObj->pBitmapImg))
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:open(),lseek(),read()
    if (iErr == 0)
    {
        //���Դ�λͼ�ļ�
        pBitmapObj->iBitmapFd = open(pBitmapObj->strBitmapFile, O_RDONLY);
        if (pBitmapObj->iBitmapFd == -1)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //���Զ�ȡλͼ�ļ�ͷ����Ϣͷ
        lseek(pBitmapObj->iBitmapFd, 0, SEEK_SET);
        if ((14 != read(pBitmapObj->iBitmapFd, &fh.usFileType, 14)) || 
            (40 != read(pBitmapObj->iBitmapFd, &ih, 40)))
        {
            iErr = -4;
        }
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //���λͼ�ļ���ʽ
        if ((fh.usFileType != 0x4d42) || 
            (ih.uiInfoSize != sizeof(GUIBITMAP_INFOHEADER)) || 
            (ih.usBitCount != 1 && ih.usBitCount != 8 && ih.usBitCount != 24) || 
            ih.uiCompression)
        {
            iErr = -5;
        }
    }

    if (iErr == 0)
    {
        //ȷ��λͼҪ��ʾ�Ŀ����߶�
        if ((pBitmapObj->uiViewWidth == 0) ||               //Ϊ0��ʵ�ʿ����ʾ
            (pBitmapObj->uiViewWidth > ih.uiImgWidth))      //������ʵ�ʿ����ʾ
        {
            pBitmapObj->uiViewWidth = ih.uiImgWidth;
        }
        if ((pBitmapObj->uiViewHeight == 0) ||              //Ϊ0��ʵ�ʸ߶���ʾ
            (pBitmapObj->uiViewHeight > ih.uiImgHeight))    //������ʵ�ʸ߶���ʾ
        {
            pBitmapObj->uiViewHeight = ih.uiImgHeight;
        }
        //����Ϊλͼ�ļ�����ͼ����Դ
        pBitmapObj->pBitmapImg = CreateImage(pBitmapObj->uiViewWidth, 
                                             pBitmapObj->uiViewHeight, 
                                             ih.usBitCount);
        if (NULL == pBitmapObj->pBitmapImg)
        {
            iErr = -6;
        }
    }

    if (iErr == 0)
    {
        //��λͼ�ļ���λ���<=8ʱ�����Զ�ȡλͼ�ļ��ĵ�ɫ��
        if (ih.usBitCount <= 8)
        {
            pPal = ReadBitmapPalette(pBitmapObj);
            if (NULL == pPal)
            {
                iErr = -7;
            }
        }
    }

    if (iErr == 0)
    {
        //�ֱ����BMP��IMG��ÿ��ͼ�����ݳ��ȣ�ע��BMP��32λ�����IMG��8λ����
        uiBmp = round_up(ih.uiImgWidth * ih.usBitCount, 32) / 8;
        uiImg = round_up(pBitmapObj->uiViewWidth * ih.usBitCount, 8) / 8;
        //Ϊ���ж�ȡͼ�����ݳ��Կ����л���
        pBuf = (unsigned char *) malloc(uiBmp);
        if (NULL == pBuf)
        {
            iErr = -8;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:lseek(),read(),close()
    if (iErr == 0)
    {
        //����λͼ������ͼ����֡�����ϵķ���λ��
        pBitmapObj->pBitmapImg->uiPlaceX = pBitmapObj->uiPlaceX;
        pBitmapObj->pBitmapImg->uiPlaceY = pBitmapObj->uiPlaceY;
        //��λͼ�ļ���λ���<=8ʱ���������ö�Ӧͼ����Դ��ǰ������ɫ�͵�ɫ��
        switch (ih.usBitCount)
        {
        case 1:
            pBitmapObj->pBitmapImg->uiFgColor = (pPal->pRedValue[1] << 16) |
                                                (pPal->pGreenValue[1] << 8) |
                                                (pPal->pBlueValue[1]);
            pBitmapObj->pBitmapImg->uiBgColor = (pPal->pRedValue[0] << 16) |
                                                (pPal->pGreenValue[0] << 8) |
                                                (pPal->pBlueValue[0]);
            DestroyPalette(&pPal);
            break;
        case 8:
            pBitmapObj->pBitmapImg->pRefPal = pPal;
            break;
        default:
            break;
        }
        //���ж�ȡBMPͼ�����ݣ�ע��Ŀǰֻ֧�ֵ���λͼ����BMP�����ڴ�ֱ����ת
        lseek(pBitmapObj->iBitmapFd, fh.uiDataOffset, SEEK_SET);
        pRes = pBitmapObj->pBitmapImg->pImgData;
        pRes += uiImg * (pBitmapObj->uiViewHeight - 1);
        for (uiHeight = 0; uiHeight < pBitmapObj->uiViewHeight; uiHeight++)
        {
            read(pBitmapObj->iBitmapFd, pBuf, uiBmp);   //��BMP����һ��
            memcpy(pRes, pBuf, uiImg);                  //д�����ͼ������
            pRes -= uiImg;
        }
        //�ͷ��л���
        free(pBuf);
        //�ر�λͼ�ļ������ļ���������-1
        close(pBitmapObj->iBitmapFd);
        pBitmapObj->iBitmapFd = -1;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:close()
    //������
    switch (iErr)
    {
    case -8:
        if (ih.usBitCount <= 8)
        {
            DestroyPalette(&pPal);
        }
    case -7:
        DestroyImage(&(pBitmapObj->pBitmapImg));
    case -6:
    case -5:
    case -4:
        close(pBitmapObj->iBitmapFd);
        pBitmapObj->iBitmapFd = -1;
    case -3:
    case -2:
    case -1:
    default:
        break;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iErr;
}


/***
  * ���ܣ�
        ж��λͼͼ����Դ
  * ������
        1.GUIBITMAP *pBitmapObj:    λͼָ�룬����Ϊ��
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
int UnloadBitmapImage(GUIBITMAP *pBitmapObj)
{
    //�����־������ֵ����
    int iErr = 0;

    if (iErr == 0)
    {
        //�ж�pBitmapObj�Ƿ�Ϊ��Чָ��
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�pBitmapObj��ͼ����Դ�Ƿ��Ѽ���
        if (NULL == pBitmapObj->pBitmapImg)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //���ͼ�������õ�ɫ�壬����ͼ����Դ�����õĵ�ɫ��
        if (NULL != pBitmapObj->pBitmapImg->pRefPal)
        {
            DestroyPalette(&(pBitmapObj->pBitmapImg->pRefPal));
        }
        //����ͼ����Դ��ж��λͼͼ����Դ
        DestroyImage(&(pBitmapObj->pBitmapImg));
    }

    return iErr;
}


/***
  * ���ܣ�
        ��ȡλͼ�еĵ�ɫ��
  * ������
        1.GUIBITMAP *pBitmapObj:    λͼָ�룬����Ϊ��
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
GUIPALETTE* ReadBitmapPalette(GUIBITMAP *pBitmapObj)
{
    //�����־������ֵ����
    int iErr = 0;
    GUIPALETTE *pReturn = NULL;
    //��ʱ��������
    int iFd = -1;
    GUIBITMAP_FILEHEADER fh;
    GUIBITMAP_INFOHEADER ih;
    GUIBITMAP_RGBQUAD *pQuad = NULL;
    unsigned int uiSize, uiIndex;

    if (iErr == 0)
    {
        //�ж�pBitmapObj�Ƿ�Ϊ��Чָ��
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //�ж�strBitmapName�Ƿ�Ϊ��Чָ��
        if (NULL == pBitmapObj->strBitmapFile)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS���:open(),lseek(),read()
    if (iErr == 0)
    {
        //����iBitmapFd��iFd
        iFd = pBitmapObj->iBitmapFd;
        //��iBitmapFd��Ϊ��Ч�ļ������������Դ�λͼ�ļ�
        if (pBitmapObj->iBitmapFd == -1)
        {
            pBitmapObj->iBitmapFd = open(pBitmapObj->strBitmapFile, O_RDONLY);
            if (pBitmapObj->iBitmapFd == -1)
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //���Զ�ȡλͼ�ļ�ͷ����Ϣͷ
        lseek(pBitmapObj->iBitmapFd, 0, SEEK_SET);
        if ((14 != read(pBitmapObj->iBitmapFd, &fh.usFileType, 14)) || 
            (40 != read(pBitmapObj->iBitmapFd, &ih, 40)))
        {
            iErr = -4;
        }
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //���λͼ�ļ���ʽ
        if ((fh.usFileType != 0x4d42) || 
            (ih.uiInfoSize != sizeof(GUIBITMAP_INFOHEADER)) || 
            (ih.usBitCount > 8) || 
            ih.uiCompression)
        {
            iErr = -5;
        }
    }

    if (iErr == 0)
    {
        //���Խ�����ɫ�����
        pReturn = CreatePalette(1 << ih.usBitCount);
        if (NULL == pReturn)
        {
            iErr = -6;
        }
    }

    if (iErr == 0)
    {
        //����Ϊλͼ�еĵ�ɫ��ṹ�����ڴ�
        uiSize = (1 << ih.usBitCount) * sizeof(GUIBITMAP_RGBQUAD);
        pQuad = (GUIBITMAP_RGBQUAD *) malloc(uiSize);
        if (NULL == pQuad)
        {
            iErr = -7;
        }
    }

    if (iErr == 0)
    {
        //���Զ�ȡλͼ�еĵ�ɫ��
        lseek(pBitmapObj->iBitmapFd, 54, SEEK_SET);
        if (uiSize != read(pBitmapObj->iBitmapFd, pQuad, uiSize))
        {
            iErr = -8;
        }
    }

    if (iErr == 0)
    {
        //��λͼ�еĵ�ɫ��д�뵽��ɫ�����
        for (uiIndex = 0; uiIndex < (1 << ih.usBitCount); uiIndex++)
        {
            pReturn->pRedValue[uiIndex] = pQuad[uiIndex].ucRed;
            pReturn->pGreenValue[uiIndex] = pQuad[uiIndex].ucGreen;
            pReturn->pBlueValue[uiIndex] = pQuad[uiIndex].ucBlue;
        }
        //�ͷ���Դ
        free(pQuad);
        //��iBitmapFd��iFd���������ļ����򿪹����ر��ļ������ļ���������-1
        if (pBitmapObj->iBitmapFd != iFd)
        {
            close(pBitmapObj->iBitmapFd);
            pBitmapObj->iBitmapFd = -1;
        }
    }

    //������
    switch (iErr)
    {
    case -8:
        free(pQuad);
    case -7:
        DestroyPalette(&pReturn);
    case -6:
    case -5:
    case -4:
        if (pBitmapObj->iBitmapFd != iFd)
        {
            close(pBitmapObj->iBitmapFd);
            pBitmapObj->iBitmapFd = -1;
        }
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    return pReturn;
}

