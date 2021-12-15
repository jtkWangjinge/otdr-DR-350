/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guipicture.c
* 摘    要：  实现GUI的图形框类型及操作，为GUI图形框控件提供定义与封装。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guipicture.h"


/***************************************
* 为实现GUIPICTURE而需要引用的其他头文件
***************************************/
//#include ""


/***
  * 功能：
        根据指定的信息直接建立图形框对象
  * 参数：
        1.unsigned int uiPlaceX:    图形框水平放置位置，以左上角为基点
        2.unsigned int uiPlaceY:    图形框垂直放置位置，以左上角为基点
        3.unsigned int uiPicWidth:  图形框水平宽度
        4.unsigned int uiPicHeight: 图形框垂直高度
        5.char *strBitmapFile:      用于建立位图资源的位图文件，可为空
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIPICTURE* CreatePicture(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                          unsigned int uiPicWidth, unsigned int uiPicHeight, 
                          char *strBitmapFile)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIPICTURE *pPicObj = NULL;

    if (!iErr)
    {
        //判断uiPicWidth和uiPicHeight是否有效
        if (uiPicWidth < 1 || uiPicHeight < 1)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //申请内存资源
        pPicObj = (GUIPICTURE *) malloc(sizeof(GUIPICTURE));
        if (NULL == pPicObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //如果指定了位图文件，尝试建立标签中的位图资源
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

    if (!iErr)
    {
        //标签对象可视信息赋值
        pPicObj->Visible.Area.Start.x = uiPlaceX;
        pPicObj->Visible.Area.Start.y = uiPlaceY;
        pPicObj->Visible.Area.End.x = uiPlaceX + uiPicWidth - 1;
        pPicObj->Visible.Area.End.y = uiPlaceY + uiPicHeight - 1;
        pPicObj->Visible.iEnable = 1;   //默认可见
        pPicObj->Visible.iLayer = 0;    //默认图层为0(最底层)
        pPicObj->Visible.iFocus = 0;    //默认无焦点
        pPicObj->Visible.iCursor = 0;   //默认无光标
        memset(&(pPicObj->Visible.Hide), 0, sizeof(GUIRECTANGLE));
        pPicObj->Visible.pResume = NULL;
        pPicObj->Visible.fnDestroy = (DESTFUNC) DestroyPicture;
        pPicObj->Visible.fnDisplay = (DISPFUNC) DisplayPicture;
        //如果位图资源已建立，设置位图的显示大小
        if (NULL != pPicObj->pPicBitmap)
        {
            pPicObj->pPicBitmap->uiViewWidth = uiPicWidth;
            pPicObj->pPicBitmap->uiViewHeight = uiPicHeight;
        }
        //标签对象的画刷、画笔及字体设置
        pPicObj->pPicPen = NULL;        //默认使用当前画笔
        pPicObj->pPicBrush = NULL;      //默认使用当前画刷
        pPicObj->pPicFont = NULL;       //默认使用当前字体
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pPicObj);
        //no break
    case -2:
    case -1:
        pPicObj = NULL;
        //no break
    default:
        break;
    }

    return pPicObj;
}


/***
  * 功能：
        删除图形框对象
  * 参数：
        1.GUIPICTURE **ppPicObj:    指针的指针，指向需要销毁的图形框对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyPicture(GUIPICTURE **ppPicObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppPicObj是否为有效指针
        if (NULL == ppPicObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppPicObj所指向的是否为有效指针
        if (NULL == *ppPicObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //如果位图资源已建立，尝试销毁位图资源
        if (NULL != (*ppPicObj)->pPicBitmap)
        {
            if (DestroyBitmap(&((*ppPicObj)->pPicBitmap)))
            {
                iErr = -3;
            }
        }
    }

    if (!iErr)
    {
        //销毁图形框对象，并将指针置空
        free(*ppPicObj);
        *ppPicObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        直接输出指定的图形框
  * 参数：
        1.GUIPICTURE *pPicObj:  图形框指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DisplayPicture(GUIPICTURE *pPicObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIBRUSH *pBrush = NULL;
    GUIPEN *pPen = NULL;
    GUIFONT *pFont = NULL;

    if (!iErr)
    {
        //判断pPicObj是否为有效指针
        if (NULL == pPicObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断pPicObj是否可见和位图资源是否已建立
        if (!pPicObj->Visible.iEnable || (NULL == pPicObj->pPicBitmap))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //若已设置标签使用的画刷，重设当前画刷
        if (NULL != pPicObj->pPicBrush)
        {
            pBrush = GetCurrBrush();
            SetCurrBrush(pPicObj->pPicBrush);
        }
        //若已设置标签使用的画笔，重设当前画笔
        if (NULL != pPicObj->pPicPen)
        {
            pPen = GetCurrPen();
            SetCurrPen(pPicObj->pPicPen);
        }
        //若已设置标签使用的字体，重设当前字体
        if (NULL != pPicObj->pPicFont)
        {
            pFont = GetCurrFont();
            SetCurrFont(pPicObj->pPicFont);
        }
        //尝试显示位图资源
        if (DisplayBitmap(pPicObj->pPicBitmap))
        {
            iErr = -3;
        }
        //若已设置标签使用的画刷，恢复当前画刷
        if (NULL != pPicObj->pPicBrush)
        {
            SetCurrBrush(pBrush);
        }
        //若已设置标签使用的画笔，恢复当前画笔
        if (NULL != pPicObj->pPicPen)
        {
            SetCurrPen(pPen);
        }
        //若已设置标签使用的字体，恢复当前字体
        if (NULL != pPicObj->pPicFont)
        {
            SetCurrFont(pFont);
        }
    }

    return iErr;
}


/***
  * 功能：
        设置图形框的有效区域
  * 参数：
        1.unsigned int uiStartX:    图形框有效区域左上角横坐标，以左上角为基点
        2.unsigned int uiStartY:    图形框有效区域左上角纵坐标，以左上角为基点
        3.unsigned int uiEndX:      图形框有效区域右下角横坐标，以左上角为基点
        4.unsigned int uiEndY:      图形框有效区域右下角纵坐标，以左上角为基点
        5.GUIPICTURE *pPicObj:      需要设置有效区域的图形框对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPictureArea(unsigned int uiStartX, unsigned int uiStartY, 
                   unsigned int uiEndX, unsigned int uiEndY,
                   GUIPICTURE *pPicObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断uiEndX、uiEndY和pPicObj是否有效
        if (uiEndX < uiStartX || uiEndY < uiStartY || NULL == pPicObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //图形框对象有效区域赋值
        pPicObj->Visible.Area.Start.x = uiStartX;
        pPicObj->Visible.Area.Start.y = uiStartY;
        pPicObj->Visible.Area.End.x = uiEndX;
        pPicObj->Visible.Area.End.y = uiEndY;
        //如果位图资源已建立，重设位图的显示大小及放置位置
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
  * 功能：
        设置图形框的可见性
  * 参数：
        1.int iEnable:          图形框是否可见，0不可见，1可见
        2.GUIPICTURE *pPicObj:  需要设置可见性的图形框对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPictureEnable(int iEnable, GUIPICTURE *pPicObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断pPicObj是否有效
        if (NULL == pPicObj)
        {
            iErr = -1;
        }
    }

    pPicObj->Visible.iEnable = iEnable;

    return 0;
}
/***
  * 功能：
		设置图形框的可见性
  * 参数：
		GUIPICTURE *pPicObj:  需要获取可见性的图形框对象
  * 返回：
		可见返回1，不可见返回0，失败返回负值
  * 备注：
***/
int GetPictureEnable(GUIPICTURE * pPicObj)
{
	//错误标志、返回值定义
	int iErr = 0;

	if (!iErr)
	{
		//判断pPicObj是否有效
		if (NULL == pPicObj)
		{
			iErr = -1;
		}
	}

	return (pPicObj->Visible.iEnable);
}


/***
  * 功能：
        设置图形框所使用的画刷
  * 参数：
        1.GUIBRUSH *pPicBrush:  用于图形框对象的画刷
        2.GUIPICTURE *pPicObj:  需要设置所使用画刷的图形框对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPictureBrush(GUIBRUSH *pPicBrush, GUIPICTURE *pPicObj)
{
    //判断pPicBrush和pPicObj是否有效
    if (NULL == pPicBrush || NULL == pPicObj)
    {
        return -1;
    }

    pPicObj->pPicBrush = pPicBrush;

    return 0;
}


/***
  * 功能：
        设置图形框所使用的画笔
  * 参数：
        1.GUIPEN *pPicPen:      用于图形框对象的画笔
        2.GUIPICTURE *pPicObj:  需要设置所使用画笔的图形框对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPicturePen(GUIPEN *pPicPen, GUIPICTURE *pPicObj)
{
    //判断pPicPen和pLblObj是否有效
    if (NULL == pPicPen || NULL == pPicObj)
    {
        return -1;
    }

    pPicObj->pPicPen = pPicPen;

    return 0;
}


/***
  * 功能：
        设置图形框所对应的字体
  * 参数：
        1.GUIFONT *pPicFont:    用于图形框对象的字体
        2.GUIPICTURE *pPicObj:  需要设置所使用字体的图形框对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPictureFont(GUIFONT *pPicFont, GUIPICTURE *pPicObj)
{
    //判断pPicFont和pPicObj是否有效
    if (NULL == pPicFont || NULL == pPicObj)
    {
        return -1;
    }

    pPicObj->pPicFont = pPicFont;

    return 0;
}


/***
  * 功能：
        设置图形框所对应的位图资源
  * 参数：
        1.char *strBitmapFile:  用于建立位图资源的位图文件
        2.GUIPICTURE *pPicObj:  需要设置位图资源的图形框对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPictureBitmap(char *strBitmapFile, GUIPICTURE *pPicObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断strBitmapFile和pPicObj是否有效
        if (NULL == strBitmapFile || NULL == pPicObj)
        {
            return -1;
        }
    }

    if (!iErr)
    {
        //如果位图资源已建立，尝试销毁旧的位图资源
        if (NULL != pPicObj->pPicBitmap)
        {
            if (DestroyBitmap(&(pPicObj->pPicBitmap)))
            {
                return -2;
            }
        }
    }

    if (!iErr)
    {
        //设置图形框所对应的位图资源
        pPicObj->pPicBitmap = CreateBitmap(strBitmapFile, 
                                           pPicObj->Visible.Area.Start.x, 
                                           pPicObj->Visible.Area.Start.y);
        if (NULL == pPicObj->pPicBitmap)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //重设位图的显示大小
        pPicObj->pPicBitmap->uiViewWidth = pPicObj->Visible.Area.End.x
                                         - pPicObj->Visible.Area.Start.x + 1;
        pPicObj->pPicBitmap->uiViewHeight = pPicObj->Visible.Area.End.y 
                                          - pPicObj->Visible.Area.Start.y + 1;
    }

    return iErr;
}

