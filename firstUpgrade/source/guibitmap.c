/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibitmap.c
* 摘    要：  实现GUI位图类型及相关操作函数，实现BMP图片的绘制。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2012-10-12
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "guibitmap.h"


/**************************************
* 为实现GUIBITMAP而需要引用的其他头文件
**************************************/
#include "guiimage.h"


/***
  * 功能：
        根据指定的信息直接建立位图对象
  * 参数：
        1.char *strBitmapFile:      用于建立位图对象的位图文件名称
        2.unsigned int uiPlaceX:    位图水平放置位置，以左上角为基点
        3.unsigned int uiPlaceY:    位图垂直放置位置，以左上角为基点
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIBITMAP* CreateBitmap(char *strBitmapFile, 
                        unsigned int uiPlaceX, unsigned int uiPlaceY)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIBITMAP *pBitmapObj = NULL;
    //临时变量定义
    unsigned int uiSize;

    if (iErr == 0)
    {
        //判断strBitmapName是否为有效指针
        if (NULL == strBitmapFile)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //申请内存资源
        pBitmapObj = (GUIBITMAP *) malloc(sizeof(GUIBITMAP));
        if (NULL == pBitmapObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //为保存位图文件名称分配内存资源
        uiSize = strlen(strBitmapFile) + 1;
        pBitmapObj->strBitmapFile = (char *) malloc(uiSize);
        if (NULL == pBitmapObj->strBitmapFile)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //位图对象成员赋值
        strcpy(pBitmapObj->strBitmapFile, strBitmapFile);
        pBitmapObj->iBitmapFd = -1;
        pBitmapObj->uiPlaceX = uiPlaceX;
        pBitmapObj->uiPlaceY = uiPlaceY;
        pBitmapObj->uiViewWidth = 0;    //默认按BMP中的实际宽度显示
        pBitmapObj->uiViewHeight = 0;   //默认按BMP中的实际高度显示
        pBitmapObj->pBitmapImg = NULL;
    }

    //错误处理
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
  * 功能：
        删除位图对象
  * 参数：
        1.GUIBITMAP **ppBitmapObj:  指针的指针，指向需要销毁的位图对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyBitmap(GUIBITMAP **ppBitmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断ppBitmapObj是否为有效指针
        if (NULL == ppBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断ppBitmapObj所指向的是否为有效指针
        if (NULL == *ppBitmapObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //如果位图图像资源已装载，尝试卸载位图图像资源
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
        //释放资源
        free((*ppBitmapObj)->strBitmapFile);
        //销毁位图对象，并将指针置空
        free(*ppBitmapObj);
        *ppBitmapObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        直接输出指定的位图
  * 参数：
        1.GUIBITMAP *pBitmapObj:    位图指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DisplayBitmap(GUIBITMAP *pBitmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断pBitmapObj是否为有效指针
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //若图像资源未装载，尝试装载位图图像资源
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
        //显示位图图像
        if (DisplayImage(pBitmapObj->pBitmapImg))
        {
            iErr = -3;
        }
    }

    return iErr;
}


/***
  * 功能：
        设置位图的显示大小
  * 参数：
        1.unsigned int uiViewWidth:     显示宽度，>=0，初始化为0则按BMP实际宽度
        2.unsigned int uiViewHeight:    显示高度，>=0，初始化为0则按BMP实际高度
        3.GUIBITMAP *pBitmapObj:        需要设置显示大小的位图对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetBitmapView(unsigned int uiViewWidth, unsigned int uiViewHeight, 
                  GUIBITMAP *pBitmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断pBitmapObj是否有效
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //如果位图图像资源已装载，尝试卸载位图图像资源
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
        //重设位图的显示大小
        pBitmapObj->uiViewWidth = uiViewWidth;
        pBitmapObj->uiViewHeight = uiViewHeight;
    }

    return iErr;
}


/***
  * 功能：
        设置位图的放置位置
  * 参数：
        1.unsigned int uiPlaceX:    位图水平放置位置，以左上角为基点，绝对坐标
        2.unsigned int uiPlaceY:    位图垂直放置位置，以左上角为基点，绝对坐标
        3.GUIBITMAP *pBitmapObj:    需要设置放置位置的位图对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetBitmapPlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                   GUIBITMAP *pBitmapObj)
{
    //判断pBitmapObj是否有效
    if (NULL == pBitmapObj)
    {
        return -1;
    }

    //如果位图图像资源已装载，重设图像的放置位置
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
  * 功能：
        装载位图图像资源
  * 参数：
        1.GUIBITMAP *pBitmapObj:    位图指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int LoadBitmapImage(GUIBITMAP *pBitmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIBITMAP_FILEHEADER fh;
    GUIBITMAP_INFOHEADER ih;
    GUIPALETTE *pPal = NULL;
    unsigned char *pBuf = NULL, *pRes = NULL;
    unsigned int uiBmp, uiImg, uiHeight;

    if (iErr == 0)
    {
        //判断pBitmapObj是否为有效指针
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断sBitmapName是否为有效指针和位图资源是否已加载
        if ((NULL == pBitmapObj->strBitmapFile) || 
            (NULL != pBitmapObj->pBitmapImg))
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:open(),lseek(),read()
    if (iErr == 0)
    {
        //尝试打开位图文件
        pBitmapObj->iBitmapFd = open(pBitmapObj->strBitmapFile, O_RDONLY);
        if (pBitmapObj->iBitmapFd == -1)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //尝试读取位图文件头和信息头
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
        //检查位图文件格式
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
        //确定位图要显示的宽度与高度
        if ((pBitmapObj->uiViewWidth == 0) ||               //为0按实际宽度显示
            (pBitmapObj->uiViewWidth > ih.uiImgWidth))      //超出按实际宽度显示
        {
            pBitmapObj->uiViewWidth = ih.uiImgWidth;
        }
        if ((pBitmapObj->uiViewHeight == 0) ||              //为0按实际高度显示
            (pBitmapObj->uiViewHeight > ih.uiImgHeight))    //超出按实际高度显示
        {
            pBitmapObj->uiViewHeight = ih.uiImgHeight;
        }
        //尝试为位图文件建立图像资源
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
        //当位图文件的位深度<=8时，尝试读取位图文件的调色板
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
        //分别计算BMP和IMG的每行图像数据长度，注意BMP按32位对齐而IMG按8位对齐
        uiBmp = round_up(ih.uiImgWidth * ih.usBitCount, 32) / 8;
        uiImg = round_up(pBitmapObj->uiViewWidth * ih.usBitCount, 8) / 8;
        //为逐行读取图像数据尝试开辟行缓冲
        pBuf = (unsigned char *) malloc(uiBmp);
        if (NULL == pBuf)
        {
            iErr = -8;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:lseek(),read(),close()
    if (iErr == 0)
    {
        //设置位图所定义图像在帧缓冲上的放置位置
        pBitmapObj->pBitmapImg->uiPlaceX = pBitmapObj->uiPlaceX;
        pBitmapObj->pBitmapImg->uiPlaceY = pBitmapObj->uiPlaceY;
        //当位图文件的位深度<=8时，尝试设置对应图像资源的前背景颜色和调色板
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
        //逐行读取BMP图像数据，注意目前只支持倒向位图，即BMP数据在垂直方向翻转
        lseek(pBitmapObj->iBitmapFd, fh.uiDataOffset, SEEK_SET);
        pRes = pBitmapObj->pBitmapImg->pImgData;
        pRes += uiImg * (pBitmapObj->uiViewHeight - 1);
        for (uiHeight = 0; uiHeight < pBitmapObj->uiViewHeight; uiHeight++)
        {
            read(pBitmapObj->iBitmapFd, pBuf, uiBmp);   //从BMP读入一行
            memcpy(pRes, pBuf, uiImg);                  //写入该行图像数据
            pRes -= uiImg;
        }
        //释放行缓冲
        free(pBuf);
        //关闭位图文件并将文件描述符置-1
        close(pBitmapObj->iBitmapFd);
        pBitmapObj->iBitmapFd = -1;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:close()
    //错误处理
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
  * 功能：
        卸载位图图像资源
  * 参数：
        1.GUIBITMAP *pBitmapObj:    位图指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int UnloadBitmapImage(GUIBITMAP *pBitmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断pBitmapObj是否为有效指针
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pBitmapObj的图像资源是否已加载
        if (NULL == pBitmapObj->pBitmapImg)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //如果图像有引用调色板，销毁图像资源所引用的调色板
        if (NULL != pBitmapObj->pBitmapImg->pRefPal)
        {
            DestroyPalette(&(pBitmapObj->pBitmapImg->pRefPal));
        }
        //销毁图像资源，卸载位图图像资源
        DestroyImage(&(pBitmapObj->pBitmapImg));
    }

    return iErr;
}


/***
  * 功能：
        读取位图中的调色板
  * 参数：
        1.GUIBITMAP *pBitmapObj:    位图指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
GUIPALETTE* ReadBitmapPalette(GUIBITMAP *pBitmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIPALETTE *pReturn = NULL;
    //临时变量定义
    int iFd = -1;
    GUIBITMAP_FILEHEADER fh;
    GUIBITMAP_INFOHEADER ih;
    GUIBITMAP_RGBQUAD *pQuad = NULL;
    unsigned int uiSize, uiIndex;

    if (iErr == 0)
    {
        //判断pBitmapObj是否为有效指针
        if (NULL == pBitmapObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断strBitmapName是否为有效指针
        if (NULL == pBitmapObj->strBitmapFile)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:open(),lseek(),read()
    if (iErr == 0)
    {
        //保存iBitmapFd到iFd
        iFd = pBitmapObj->iBitmapFd;
        //若iBitmapFd不为有效文件描述符，尝试打开位图文件
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
        //尝试读取位图文件头和信息头
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
        //检查位图文件格式
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
        //尝试建立调色板对象
        pReturn = CreatePalette(1 << ih.usBitCount);
        if (NULL == pReturn)
        {
            iErr = -6;
        }
    }

    if (iErr == 0)
    {
        //尝试为位图中的调色板结构分配内存
        uiSize = (1 << ih.usBitCount) * sizeof(GUIBITMAP_RGBQUAD);
        pQuad = (GUIBITMAP_RGBQUAD *) malloc(uiSize);
        if (NULL == pQuad)
        {
            iErr = -7;
        }
    }

    if (iErr == 0)
    {
        //尝试读取位图中的调色板
        lseek(pBitmapObj->iBitmapFd, 54, SEEK_SET);
        if (uiSize != read(pBitmapObj->iBitmapFd, pQuad, uiSize))
        {
            iErr = -8;
        }
    }

    if (iErr == 0)
    {
        //将位图中的调色板写入到调色板对象
        for (uiIndex = 0; uiIndex < (1 << ih.usBitCount); uiIndex++)
        {
            pReturn->pRedValue[uiIndex] = pQuad[uiIndex].ucRed;
            pReturn->pGreenValue[uiIndex] = pQuad[uiIndex].ucGreen;
            pReturn->pBlueValue[uiIndex] = pQuad[uiIndex].ucBlue;
        }
        //释放资源
        free(pQuad);
        //若iBitmapFd与iFd不符，则文件被打开过，关闭文件并将文件描述符置-1
        if (pBitmapObj->iBitmapFd != iFd)
        {
            close(pBitmapObj->iBitmapFd);
            pBitmapObj->iBitmapFd = -1;
        }
    }

    //错误处理
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

