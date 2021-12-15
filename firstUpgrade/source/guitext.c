/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guitext.c
* 摘    要：  实现GUI的文本类型及操作，及基本的文本输出操作函数，为GUI图形控件的
*             实现提供基础
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "guitext.h"


/************************************
* 为实现GUITEXT而需要引用的其他头文件
************************************/
#include "guiimage.h"


/***
  * 功能：
        根据指定的信息直接建立文本对象
  * 参数：
        1.unsigned short *pTextData:    用于建立文本对象的文本内容(宽字符)
        2.unsigned int uiPlaceX:        文本水平放置位置，以左上角为基点
        3.unsigned int uiPlaceY:        文本垂直放置位置，以左上角为基点
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUITEXT* CreateText(unsigned short *pTextData, 
                    unsigned int uiPlaceX, unsigned int uiPlaceY)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUITEXT *pTextObj = NULL;
    //临时变量定义
    unsigned int uiSize;

    if (iErr == 0)
    {
        //判断pTextData是否为有效指针
        if (NULL == pTextData)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //申请内存资源
        pTextObj = (GUITEXT *) malloc(sizeof(GUITEXT));
        if (NULL == pTextObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //得到文本长度
        pTextObj->uiTextLength = 0;
        pTextObj->pTextData = pTextData;
        while (*(pTextObj->pTextData))              //文本内容，以'\0'结尾
        {
            pTextObj->uiTextLength++;
            pTextObj->pTextData++;
        }
        //为保存文本内容分配内存资源
        uiSize = 2 * (pTextObj->uiTextLength + 1);  //文本长度不含'\0'
        pTextObj->pTextData = (unsigned short *) malloc(uiSize);
        if (NULL == pTextObj->pTextData)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //文本对象成员赋值
        memcpy(pTextObj->pTextData, pTextData, uiSize);
        pTextObj->uiPlaceX = uiPlaceX;
        pTextObj->uiPlaceY = uiPlaceY;
        pTextObj->uiViewWidth = 0;      //默认按文本实际宽度显示
        pTextObj->uiViewHeight = 0;     //默认按文本实际高度显示
        pTextObj->ppTextMatrix = NULL;
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pTextObj);
    case -2:
    case -1:
        pTextObj = NULL;
    default:
        break;
    }

    return pTextObj;
}


/***
  * 功能：
        删除文本对象
  * 参数：
        1.GUITEXT **ppTextObj:  指针的指针，指向需要销毁的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyText(GUITEXT **ppTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断ppTextObj是否为有效指针
        if (NULL == ppTextObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断ppTextObj所指向的是否为有效指针
        if (NULL == *ppTextObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //如果文本点阵资源已装载，尝试卸载文本点阵资源
        if (NULL != (*ppTextObj)->ppTextMatrix)
        {
            if (UnloadTextMatrix(*ppTextObj))
            {
                iErr = -3;
            }
        }
    }

    if (iErr == 0)
    {
        //释放资源
        free((*ppTextObj)->pTextData);
        //销毁文本对象，并将指针置空
        free(*ppTextObj);
        *ppTextObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        直接输出指定的文本
  * 参数：
        1.GUITEXT *pTextObj:    文本指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DisplayText(GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIFONT *pFont = NULL;
    GUIIMAGE *pImg = NULL;
    GUIFONT_MATRIX **ppMatrix = NULL;
    unsigned int uiX, uiY, uiTmp;

    if (iErr == 0)
    {
        //判断pTextObj是否为有效指针
        if (NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //若文本点阵资源未装载，尝试装载文本点阵资源
        if (NULL == pTextObj->ppTextMatrix)
        {
            if (LoadTextMatrix(pTextObj))
            {
                iErr = -2;
            }
        }
    }

    if (iErr == 0)
    {
        //得到当前字体对象并判断是否为有效指针
        pFont = GetCurrFont();
        if (NULL == pFont)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //输出文本中各字符所对应的点阵
        ppMatrix = pTextObj->ppTextMatrix;
        uiX = pTextObj->uiPlaceX;
        uiY = pTextObj->uiPlaceY;
        for (uiTmp = 0; uiTmp < pTextObj->uiTextLength; uiTmp++)
        {
            //判断是否有足够空间显示文本中的当前字符
            if ((uiX + ppMatrix[uiTmp]->uiRealWidth) > 
                (pTextObj->uiPlaceX + pTextObj->uiViewWidth))
            {
                uiX = pTextObj->uiPlaceX;
                uiY += ppMatrix[uiTmp]->uiRealHeight
                     + pFont->uiTopGap + pFont->uiBottomGap;
            }
            if ((uiY + ppMatrix[uiTmp]->uiRealHeight) > 
                (pTextObj->uiPlaceY + pTextObj->uiViewHeight))
            {
                iErr = -4;
                break;
            }
            //尝试为单个字体点阵建立1位色图像对象
            pImg = CreateImage(pFont->uiFontWidth, pFont->uiFontHeight, 1);
            if (NULL == pImg)
            {
                iErr = -5;
                break;
            }
            //设置图像资源、显示大小、位置与颜色，并显示图像(即字体点阵)
            memcpy(pImg->pImgData, 
                   ppMatrix[uiTmp]->pMatrixData, 
                   round_up(pFont->uiFontWidth, 8) / 8 * pFont->uiFontHeight);
            pImg->uiDispWidth = ppMatrix[uiTmp]->uiRealWidth;
            pImg->uiDispHeight = ppMatrix[uiTmp]->uiRealHeight;
            pImg->uiPlaceX = uiX;
            pImg->uiPlaceY = uiY;
            pImg->uiFgColor = pFont->uiFgColor;
            pImg->uiBgColor = pFont->uiBgColor;
            if (DisplayImage(pImg))
            {
                iErr = -6;
                break;
            }
            //销毁图像资源
            DestroyImage(&pImg);
            //移动到下一个显示位置
            uiX += ppMatrix[uiTmp]->uiRealWidth 
                 + pFont->uiLeftGap + pFont->uiRightGap;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -6:
        DestroyImage(&pImg);
    case -5:
    case -4:
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}


/***
  * 功能：
        设置文本的内容
  * 参数：
        1.unsigned short *pTextData:    文本内容(宽字符)，以'\0'结尾
        2.GUITEXT *pTextObj:            需要设置文本内容的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetTextData(unsigned short *pTextData, GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned short *pData;
    unsigned int uiLen, uiSize;

    if (iErr == 0)
    {
        //判断pTextData和pTextObj是否有效
        if (NULL == pTextData || NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //如果文本点阵资源已装载，尝试卸载文本点阵资源
        if (NULL != pTextObj->ppTextMatrix)
        {
            if (UnloadTextMatrix(pTextObj))
            {
                iErr = -2;
            }
        }
    }

    if (iErr == 0)
    {
        //得到文本长度
        pData = pTextData;
        uiLen = 0;
        while (*pData++)                //文本内容，以'\0'结尾
        {
            uiLen++;
        }
        //为保存文本内容分配内存资源
        uiSize = 2 * (uiLen + 1);       //文本长度不含'\0'
        pData = (unsigned short *) malloc(uiSize);
        if (NULL == pData)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //释放旧的文本内容
        free(pTextObj->pTextData);
        //重设文本长度和文本内容
        pTextObj->uiTextLength = uiLen;
        pTextObj->pTextData = pData;
        memcpy(pTextObj->pTextData, pTextData, uiSize);
        //重设文本的显示大小
        pTextObj->uiViewWidth = 0;
        pTextObj->uiViewHeight = 0;
    }

    return iErr;
}


/***
  * 功能：
        设置文本的显示大小
  * 参数：
        1.unsigned int uiViewWidth:     显示宽度，>=0，初始化为0则按文本实际宽度
        2.unsigned int uiViewHeight:    显示高度，>=0，初始化为0则按文本实际高度
        3.GUITEXT *pTextObj:            需要设置显示大小的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetTextView(unsigned int uiViewWidth, unsigned int uiViewHeight, 
                GUITEXT *pTextObj)
{
    //判断pTextObj是否有效
    if (NULL == pTextObj)
    {
        return -1;
    }

    pTextObj->uiViewWidth = uiViewWidth;
    pTextObj->uiViewHeight = uiViewHeight;

    return 0;
}


/***
  * 功能：
        设置文本的放置位置
  * 参数：
        1.unsigned int uiPlaceX:    文本水平放置位置，以左上角为基点，绝对坐标
        2.unsigned int uiPlaceY:    文本垂直放置位置，以左上角为基点，绝对坐标
        3.GUITEXT *pTextObj:        需要设置放置位置的文本对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetTextPlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                  GUITEXT *pTextObj)
{
    //判断pTextObj是否有效
    if (NULL == pTextObj)
    {
        return -1;
    }

    pTextObj->uiPlaceX = uiPlaceX;
    pTextObj->uiPlaceY = uiPlaceY;

    return 0;
}


/***
  * 功能：
        装载文本点阵资源
  * 参数：
        1.GUITEXT *pTextObj:    文本指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int LoadTextMatrix(GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiSize, uiWidth;
    GUIFONT *pFont = NULL;
    GUIFONT_MATRIX *pMatrix = NULL;

    if (iErr == 0)
    {
        //判断pTextObj是否为有效指针
        if (NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断点阵资源是否已加载
        if (NULL != pTextObj->ppTextMatrix)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //尝试为文本所对应的点阵资源分配内存，注意内存必须清0
        uiSize = pTextObj->uiTextLength * sizeof(GUIFONT_MATRIX **);
        pTextObj->ppTextMatrix = (GUIFONT_MATRIX **) calloc(uiSize, 1);
        if (NULL == pTextObj->ppTextMatrix)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //得到当前字体对象并判断是否为有效指针
        pFont = GetCurrFont();
        if (NULL == pFont)
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //逐个提取点阵资源，并计算文本实际宽度
        uiWidth = 0;
        for (uiSize = 0; uiSize < pTextObj->uiTextLength; uiSize++)
        {
            pMatrix = ExtractFontMatrix(pTextObj->pTextData[uiSize], pFont);
            if (NULL == pMatrix)
            {
                iErr = -5;
                break;
            }
            pTextObj->ppTextMatrix[uiSize] = pMatrix;
            uiWidth += pMatrix->uiRealWidth
                     + pFont->uiLeftGap + pFont->uiRightGap;;
        }
    }

    if (iErr == 0)
    {
        //确定文本要显示的宽度与高度
        if ((pTextObj->uiViewWidth == 0) ||     //为0按实际宽度显示
            (pTextObj->uiViewWidth > uiWidth))  //超出按实际宽度显示
        {
            pTextObj->uiViewWidth = uiWidth;
        }
        if (pTextObj->uiViewHeight == 0)        //为0按实际高度显示
        {
            pTextObj->uiViewHeight = pFont->uiFontHeight;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -5:
        for (uiSize = 0; uiSize < pTextObj->uiTextLength; uiSize++)
        {
            if (NULL != pTextObj->ppTextMatrix[uiSize])
            {
                free(pTextObj->ppTextMatrix[uiSize]->pMatrixData);
                free(pTextObj->ppTextMatrix[uiSize]);
                pTextObj->ppTextMatrix[uiSize] = NULL;
            }
        }
    case -4:
    case -3:
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}


/***
  * 功能：
        卸载文本点阵资源
  * 参数：
        1.GUITEXT *pTextObj:    文本指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int UnloadTextMatrix(GUITEXT *pTextObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiLen;

    if (iErr == 0)
    {
        //判断pTextObj是否为有效指针
        if (NULL == pTextObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pTextObj的点阵资源是否已加载
        if (NULL == pTextObj->ppTextMatrix)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //逐个释放字体点阵
        for (uiLen = 0; uiLen < pTextObj->uiTextLength; uiLen++)
        {
            if (NULL != pTextObj->ppTextMatrix[uiLen])
            {
                free(pTextObj->ppTextMatrix[uiLen]->pMatrixData);
                free(pTextObj->ppTextMatrix[uiLen]);
            }
        }
        //卸载文本点阵资源，并将指针置空
        free(pTextObj->ppTextMatrix);
        pTextObj->ppTextMatrix = NULL;
    }

    return iErr;
}

