/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guipalette.c
* 摘    要：  定义GUI的调色板类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#include "guipalette.h"


/***************************************
* 为实现GUIPALETTE而需要引用的其他头文件
***************************************/
//#include ""


/**********************
* GUI中的当前调色板对象
**********************/
static GUIPALETTE *pCurrPalette = NULL;


/***
  * 功能：
        根据指定的信息直接建立调色板对象
  * 参数：
        1.int iPalLength:   指定要创建的调色板长度
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIPALETTE* CreatePalette(int iPalLength)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIPALETTE *pPalObj = NULL;

    if (!iErr)
    {
        //判断uiPalLength是否超出限制
        if (iPalLength < 1 || iPalLength > 256)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //尝试为调色板对象分配内存
        pPalObj = (GUIPALETTE *) malloc(sizeof(GUIPALETTE));
        if (NULL == pPalObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //为颜色表分配内存资源
        pPalObj->pTabEntry = (unsigned char *) malloc(3 * iPalLength);
        if (NULL == pPalObj->pTabEntry)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //为颜色表中各颜色的RGB分量指定访问指针
        pPalObj->pRedValue = pPalObj->pTabEntry + iPalLength * 0;
        pPalObj->pGreenValue = pPalObj->pTabEntry + iPalLength * 1;
        pPalObj->pBlueValue = pPalObj->pTabEntry + iPalLength * 2;
        //设置新建调色板对象的调色板长度
        pPalObj->iPalLength = iPalLength;
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pPalObj);
        //no break
    case -2:
    case -1:
        pPalObj = NULL;
        //no break
    default:
        break;
    }

    return pPalObj;
}


/***
  * 功能：
        删除调色板对象
  * 参数：
        1.GUIPALETTE *pPalObj:  指针的指针，指向需要销毁的调色板对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyPalette(GUIPALETTE **ppPalObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppPalObj是否为有效指针
        if (NULL == ppPalObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppPalObj所指向的是否为有效指针
        if (NULL == *ppPalObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //释放颜色表
        free((*ppPalObj)->pTabEntry);
        //销毁调色板对象，并将指针置空
        free(*ppPalObj);
        *ppPalObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        获得当前调色板对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIPALETTE* GetCurrPalette(void)
{
    return pCurrPalette;
}


/***
  * 功能：
        设置为当前调色板对象
  * 参数：
        1.GUIPALETTE *pPalObj:  被设置为当前调色板对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrPalette(GUIPALETTE *pPalObj)
{
    //判断pPalObj是否有效
    if (NULL == pPalObj)
    {
        return -1;
    }

    pCurrPalette = pPalObj;

    return 0;
}


/***
  * 功能：
       在指定调色板对象中匹配与特定颜色最接近的调色板颜色
  * 参数：
        1.unsigned int uiRgbColor:  需匹配的特定颜色，RGB888
        2.GUIPALETTE *pPalObj:      被匹配的调色板对象
  * 返回：
        成功返回颜色索引(0~255)，失败返回负值
  * 备注：
***/
int MatchPaletteColor(unsigned int uiRgbColor, GUIPALETTE *pPalObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    int iReturn = -1;
    //临时变量定义
    unsigned char *pRed, *pGreen, *pBlue;
    unsigned char ucRed, ucGreen, ucBlue;
    int iMin, iLength, iCurr, iDiff;

    if (!iErr)
    {
        //判断pPalObj是否为有效指针
        if (NULL == pPalObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //得到调色板中各颜色项的RGB分量指针
        pRed = pPalObj->pRedValue;
        pGreen = pPalObj->pGreenValue;
        pBlue = pPalObj->pBlueValue;
        //提取特定颜色的RGB三分量
        ucRed = (uiRgbColor & 0xFF0000) >> 16;
        ucGreen = (uiRgbColor & 0x00FF00) >> 8;
        ucBlue = (uiRgbColor & 0x0000FF) >> 0;
        //遍历调色板中所有颜色，寻找与特定颜色RGB三分量方差之和最小的调色板项
        iMin = 255 * 255 * 3;       //RGB三分量方差之和最大值为255*255*3
        iLength = pPalObj->iPalLength;
        while (iLength--)
        {
            iCurr = 0;
            iDiff = pRed[iLength] - ucRed;
            iCurr += iDiff * iDiff;
            iDiff = pGreen[iLength] - ucGreen;
            iCurr += iDiff * iDiff;
            iDiff = pBlue[iLength] - ucBlue;
            iCurr += iDiff * iDiff;
            if (iCurr < iMin)
            {
                iMin = iCurr;
                iReturn = iLength;  //记录调色板项的编号
            }
        }
    }

    return iReturn;
}

