/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guifont.c
* 摘    要：  实现GUI的字体类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "guifont.h"


/************************************
* 为实现GUIFONT而需要引用的其他头文件
************************************/
//#include ""


/********************
* GUI中的当前字体对象
********************/
static GUIFONT *pCurrFont = NULL;


/***
  * 功能：
        根据指定的信息直接建立字体对象
  * 参数：
        1.char *strFontFile:            用于建立字体对象的字体文件名称
        2.unsigned int uiFontWidth:     字体的宽度，单位为像素
        3.unsigned int uiFontHeight:    字体的高度，单位为像素
        4.unsigned int uiFgColor:       字体前景色
        5.unsigned int uiBgColor:       字体背景色
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
//根据指定的信息直接建立字体对象
GUIFONT* CreateFont(char *strFontFile, 
                    unsigned int uiFontWidth, unsigned int uiFontHeight, 
                    unsigned int uiFgColor, unsigned int uiBgColor)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIFONT *pFontObj = NULL;
    //临时变量定义
    unsigned int uiSize;
    GUIFONT_FILEHEADER fh;

    if (iErr == 0)
    {
        //判断strFontFile是否为有效指针
        if (NULL == strFontFile)
        {
            iErr = -1;
        }
        //判断字体的宽度和高度是否有效
        if (uiFontWidth == 0 || uiFontHeight == 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //申请内存资源
        pFontObj = (GUIFONT *) malloc(sizeof(GUIFONT));
        if (NULL == pFontObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //为保存字体文件名称分配内存资源
        uiSize = strlen(strFontFile) + 1;
        pFontObj->strFontFile = (char *) malloc(uiSize);
        if (NULL == pFontObj->strFontFile)
        {
            iErr = -3;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:open(),lseek(),read()
    if (iErr == 0)
    {
        //尝试打开字体文件
        pFontObj->iFontFd = open(strFontFile, O_RDONLY);
        if (pFontObj->iFontFd == -1)
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //尝试读取字体文件头
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
        //检查字体文件格式
        if ((fh.uiFileType != 0x544E462E) || 
            (fh.uiFileFormat != GUIFONT_FILE_STD) || 
            fh.usSegmentFlag)
        {
            iErr = -6;
        }
    }

    if (iErr == 0)
    {
        //字体对象成员赋值
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

    //错误处理
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
  * 功能：
        删除字体对象
  * 参数：
        1.GUIFONT **ppFontObj:  指针的指针，指向需要销毁的字体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DestroyFont(GUIFONT **ppFontObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断ppFontObj是否为有效指针
        if (NULL == ppFontObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断ppFontObj所指向的是否为有效指针
        if (NULL == *ppFontObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:close()
    if (iErr == 0)
    {
        //释放资源
        close((*ppFontObj)->iFontFd);
        free((*ppFontObj)->strFontFile);
        //销毁字体对象，并将指针置空
        free(*ppFontObj);
        *ppFontObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iErr;
}


/***
  * 功能：
        得到当前的字体对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIFONT* GetCurrFont(void)
{
    return pCurrFont;
}


/***
  * 功能：
        设置当前的字体对象
  * 参数：
        1.GUIFONT *pFontObj:    被设置为当前字体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrFont(GUIFONT *pFontObj)
{
    //判断pFontObj是否有效
    if (NULL == pFontObj)
    {
        return -1;
    }

    pCurrFont = pFontObj;

    return 0;
}


/***
  * 功能：
        设置字体的间距
  * 参数：
        1.unsigned int uiLeftGap:   字体左间距，其值<=字体宽度/2
        2.unsigned int uiRightGap:  字体右间距，其值<=字体宽度/2
        3.unsigned int uiTopGap:    字体上间距，其值<=字体高度/2
        4.unsigned int uiBottomGap: 字体下间距，其值<=字体高度/2
        5.GUIFONT *pFntObj:         需要设置字体间距的字体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetFontGap(unsigned int uiLeftGap, unsigned int uiRightGap, 
               unsigned int uiTopGap, unsigned int uiBottomGap, 
               GUIFONT *pFontObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断pFontObj是否有效
        if (NULL == pFontObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断要设置的字体间距是否符合要求
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
        //重设字体间距
        pFontObj->uiLeftGap = uiLeftGap;
        pFontObj->uiRightGap = uiRightGap;
        pFontObj->uiTopGap = uiTopGap;
        pFontObj->uiBottomGap = uiBottomGap;
    }

    return iErr;
}


/***
  * 功能：
        设置字体的前背景颜色
  * 参数：
        1.unsigned int uiFgColor:   字体前景色，即字体颜色
        2.unsigned int uiBgColor:   字体背景色
        3.GUIFONT *pFntObj:         需要设置前背景颜色的字体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetFontColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                 GUIFONT *pFontObj)
{
    //判断pFontObj是否有效
    if (NULL == pFontObj)
    {
        return -1;
    }

    pFontObj->uiFgColor = uiFgColor;
    pFontObj->uiBgColor = uiBgColor;

    return 0;
}


/***
  * 功能：
        从指定字体对象中提取特定编码的字体点阵
  * 参数：
        1.unsigned short usMatrixCode:  字体点阵编码，即字符编码
        2.GUIFONT *pFntObj:             需要从中提取字体点阵的字体对象
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIFONT_MATRIX* ExtractFontMatrix(unsigned short usMatrixCode, 
                                  GUIFONT *pFontObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIFONT_MATRIX *pReturn = NULL;
    //临时变量定义
    GUIFONT_FILEHEADER fh;
    unsigned char *pBuf = NULL, *pMatrix = NULL;
    unsigned int uiFont, uiSize, uiTmp, uiLeft, uiRight, uiBit, uiGap;

    if (iErr == 0)
    {
        //判断pFontObj是否为有效指针
        if (NULL == pFontObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //申请内存资源
        pReturn = (GUIFONT_MATRIX *) malloc(sizeof(GUIFONT_MATRIX));
        if (NULL == pReturn)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:lseek(),read()
    if (iErr == 0)
    {
        //尝试读取字体文件头
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
        //判断usMatrixCode是否能够被提取
        if (usMatrixCode >= fh.uiMatrixCount)   //不分段的情况下，编码从0开始
        {
            iErr = -4;
        }
        //检查字体是否与点阵大小一致(目前尚未支持缩放)
        if ((pFontObj->uiFontWidth != fh.uiMatrixWidth) || 
            (pFontObj->uiFontHeight != fh.uiMatrixHeight))
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //计算点阵数据的每行数据长度，注意点阵数据每行按8位对齐
        uiFont = round_up(fh.uiMatrixWidth, 8) / 8;
        //尝试为字体点阵中的点阵数据分配内存
        uiSize = uiFont * fh.uiMatrixHeight;
        pReturn->pMatrixData = (unsigned char *) malloc(uiSize);
        if (NULL == pReturn->pMatrixData)
        {
            iErr = -5;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:lseek(),read()
    if (iErr == 0)
    {
        //读取文件中的点阵数据至字体点阵
        lseek(pFontObj->iFontFd, 
              fh.uiDataOffset + usMatrixCode * (uiFont * fh.uiMatrixHeight), 
              SEEK_SET);
        read(pFontObj->iFontFd, 
             pReturn->pMatrixData, 
             uiFont * fh.uiMatrixHeight);
        //为计算可用于显示的实际宽度尝试开辟行缓冲，注意缓冲必须清0
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
        //对点阵数据按行进行位或操作，以便统计其可用于显示的实际宽度
        for (uiSize = 0; uiSize < uiFont; uiSize++)
        {
            pMatrix = pReturn->pMatrixData;
            for (uiTmp = 0; uiTmp < fh.uiMatrixHeight; uiTmp++)
            {
                pBuf[uiSize] |= pMatrix[uiSize];
                pMatrix += uiFont;
            }
        }
        //得到点阵数据左侧空白的大小
        uiLeft = 0;
        for (uiSize = 0; uiSize < uiFont; uiSize++)  //从左往右
        {
            if (pBuf[uiSize])
            {
                uiTmp = pBuf[uiSize];
                for (uiBit = 0; uiBit < 8; uiBit++)
                {
                    if (uiTmp & 0x80)                   //bit7位于最左侧
                    {
                        break;
                    }
                    else
                    {
                        uiTmp <<= 1;
                        uiLeft++;
                    }
                }
                break;      //左侧空白已得到，结束循环
            }
            uiLeft += 8;    //每往右一字节增加8，循环结束其值可能超出点阵宽度
        }
        //得到点阵数据右侧空白的大小
        uiRight = 0;
        for (uiSize = uiFont; uiSize > 0; uiSize--)  //从右往左
        {
            if (pBuf[uiSize - 1])
            {
                uiTmp = pBuf[uiSize - 1];
                for (uiBit = 0; uiBit < 8; uiBit++)
                {
                    if (uiTmp & 0x01)                   //bit0位于最右侧
                    {
                        break;
                    }
                    else
                    {
                        uiTmp >>= 1;
                        uiRight++;
                    }
                }
                break;      //右侧空白已得到，结束循环
            }
            uiRight += 8;   //每往左一字节增加8，循环结束其值可能超出点阵宽度
        }
        //如果存在左侧空白且点阵不为空格，将点阵数据靠左以消除空白
        if ((uiLeft > 0) && 
            (uiLeft < fh.uiMatrixWidth))
        {
            uiBit = uiLeft & bitmask_low(3, 8); //靠左时需要左移的位数
            uiGap = round_down(uiLeft, 8) / 8;  //靠左时需要跳开的字节数
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
        //判断是否为空格之类的点阵(即不显示任何点)，并设置字体点阵的成员属性
        if ((uiLeft >= fh.uiMatrixWidth) &&     //判断点阵条件为>=uiMatrixWidth
            (uiRight >= fh.uiMatrixWidth))
        {
            pReturn->uiRealWidth = fh.uiMatrixWidth * 3 / 8;    //八分之三宽度
            pReturn->uiRealHeight = fh.uiMatrixHeight;
        }
        else
        {
            pReturn->uiRealWidth = fh.uiMatrixWidth - uiLeft - uiRight;
            pReturn->uiRealHeight = fh.uiMatrixHeight;
        }
        //释放行缓冲
        free(pBuf);
    }

    //错误处理
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

