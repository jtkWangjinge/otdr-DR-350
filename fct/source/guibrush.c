/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibrush.c
* 摘    要：  实现GUI的画刷类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "guibrush.h"


/*************************************
* 为实现GUIBRUSH而需要引用的其他头文件
*************************************/
//#include ""


/********************
* GUI中的当前画刷对象
********************/
static GUIBRUSH *pCurrBrush = NULL;


/***
  * 功能：
        根据指定的信息直接建立画刷对象
  * 参数：
        1.int iBrushType:   指定要创建的画刷类型
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIBRUSH* CreateBrush(int iBrushType)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIBRUSH *pBrushObj = NULL;

    if (iErr == 0)
    {
        //尝试为画刷对象分配内存
        pBrushObj = (GUIBRUSH *) malloc(sizeof(GUIBRUSH));
        if (NULL == pBrushObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //画刷对象成员赋值
        pBrushObj->iBrushType = iBrushType;
        pBrushObj->uiFgColor = 0xFFFFFFFF;  //0xFFFFFFFF(全透明)表示前景颜色无效
        pBrushObj->uiBgColor = 0xFFFFFFFF;  //0xFFFFFFFF(全透明)表示背景颜色无效
    }

    return pBrushObj;
}


/***
  * 功能：
        删除画刷对象
  * 参数：
        1.GUIBRUSH **ppBrushObj:    指针的指针，指向需要销毁的画刷对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DestroyBrush(GUIBRUSH **ppBrushObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断ppBrushObj是否为有效指针
        if (NULL == ppBrushObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断ppBrushObj所指向的是否为有效指针
        if (NULL == *ppBrushObj)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //销毁画刷对象，并将指针置空
        free(*ppBrushObj);
        *ppBrushObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        得到当前的画刷对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIBRUSH* GetCurrBrush(void)
{
    return pCurrBrush;
}


/***
  * 功能：
        设置当前的画刷对象
  * 参数：
        1.GUIBRUSH *pBrushObj:  被设置为当前画刷对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrBrush(GUIBRUSH *pBrushObj)
{
    //判断pBrushObj是否有效
    if (NULL == pBrushObj)
    {
        return -1;
    }

    pCurrBrush = pBrushObj;

    return 0;
}


/***
  * 功能：
        设置画刷的前背景颜色
  * 参数：
        1.unsigned int uiFgColor:   画刷前景颜色值，画刷类型为纯色画刷时使用
        2.unsigned int uiBgColor:   画刷背景颜色值，画刷类型为纯色画刷时使用
        3.GUIBRUSH *pBrushObj:      需要设置前背景的画刷对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetBrushColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                  GUIBRUSH *pBrushObj)
{
    //判断pBrushObj是否有效
    if (NULL == pBrushObj)
    {
        return -1;
    }

    pBrushObj->uiFgColor = uiFgColor;
    pBrushObj->uiBgColor = uiBgColor;

    return 0;
}

