/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guipen.c
* 摘    要：  实现GUI的画笔类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guipen.h"


/***********************************
* 为实现GUIPEN而需要引用的其他头文件
***********************************/
//#include ""


/********************
* GUI中的当前画笔对象
********************/
static GUIPEN *pCurrPen = NULL;


/***
  * 功能：
        根据指定的信息直接建立画笔对象
  * 参数：
        1.int iPenType:             指定要创建的画笔类型
        2.int iPenWidth:            指定要创建的画笔宽度
        3.unsigned int uiPenColor:  指定要创建的画笔颜色
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIPEN* CreatePen(int iPenType, int iPenWidth, unsigned int uiPenColor)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIPEN *pPenObj = NULL;

    if (!iErr)
    {
        //尝试为画笔对象分配内存
        pPenObj = (GUIPEN *) malloc(sizeof(GUIPEN));
        if (NULL == pPenObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //画笔对象成员赋值
        pPenObj->iPenType = iPenType;
        pPenObj->iPenWidth = iPenWidth;
        pPenObj->uiPenColor = uiPenColor;
    }

    return pPenObj;
}


/***
  * 功能：
        删除画笔对象
  * 参数：
        1.GUIPEN **ppPenObj:    指针的指针，指向需要销毁的画笔对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DestroyPen(GUIPEN **ppPenObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppPenObj是否为有效指针
        if (NULL == ppPenObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppPenObj所指向的是否为有效指针
        if (NULL == *ppPenObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //销毁画笔对象，并将指针置空
        free(*ppPenObj);
        *ppPenObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        得到当前的画笔对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIPEN* GetCurrPen(void)
{
    return pCurrPen;
}


/***
  * 功能：
        设置当前的画笔对象
  * 参数：
        1.GUIPEN *pPenObj:  被设置为当前画笔对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrPen(GUIPEN *pPenObj)
{
    //判断pPenObj是否有效
    if (NULL == pPenObj)
    {
        return -1;
    }

    pCurrPen = pPenObj;

    return 0;
}


/***
  * 功能：
        设置画笔的颜色
  * 参数：
        1.GUIPEN *pPenObj:  被设置为当前画笔对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPenColor(unsigned int uiPenColor, GUIPEN *pPenObj)
{
    //判断pPenObj是否有效
    if (NULL == pPenObj)
    {
        return -1;
    }

    //设置颜色
    pPenObj->uiPenColor = uiPenColor;

    return 0;
}

