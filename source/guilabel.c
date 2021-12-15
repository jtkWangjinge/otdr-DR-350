/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guilabel.c
* 摘    要：  实现GUI的标签类型及操作，为GUI标签控件提供定义与封装。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：
*******************************************************************************/

#include "guilabel.h"
#include "wnd_global.h"
/*************************************
* 为实现GUILABEL而需要引用的其他头文件
*************************************/
//#include ""


/***
  * 功能：
        根据指定的信息直接建立标签对象
  * 参数：
        1.unsigned int uiPlaceX:        标签水平放置位置，以左上角为基点
        2.unsigned int uiPlaceY:        标签垂直放置位置，以左上角为基点
        3.unsigned int uiLblWidth:      标签水平宽度
        4.unsigned int uiLblHeight:     标签垂直高度
        5.unsigned short *pTextData:    用于建立文本资源的文本内容，可为空
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUILABEL* CreateLabel(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                      unsigned int uiLblWidth, unsigned int uiLblHeight, 
                      unsigned short *pTextData)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUILABEL *pLblObj = NULL;

    if (!iErr)
    {
        //判断uiLblWidth和uiLblHeight是否有效
        if (uiLblWidth < 1 || uiLblHeight < 1)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //申请内存资源
        pLblObj = (GUILABEL *) malloc(sizeof(GUILABEL));
        if (NULL == pLblObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //如果指定了文本内容，尝试建立标签中的文本资源
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

    if (!iErr)
    {
        //标签对象可视信息赋值
        pLblObj->Visible.Area.Start.x = uiPlaceX;
		//修改，根据是否是24点阵字体，进行上移4个像素
        pLblObj->Visible.Area.Start.y = uiPlaceY;//isFont24() ? uiPlaceY-4 : uiPlaceY;
        pLblObj->Visible.Area.End.x = uiPlaceX + uiLblWidth - 1;
        pLblObj->Visible.Area.End.y = uiPlaceY + uiLblHeight - 1;
        pLblObj->Visible.iEnable = 1;   //默认可见
        pLblObj->Visible.iLayer = 0;    //默认图层为0(最底层)
        pLblObj->Visible.iFocus = 0;    //默认无焦点
        pLblObj->Visible.iCursor = 0;   //默认无光标
        memset(&(pLblObj->Visible.Hide), 0, sizeof(GUIRECTANGLE));
        pLblObj->Visible.pResume = NULL;
        pLblObj->Visible.fnDestroy = (DESTFUNC) DestroyLabel;
        pLblObj->Visible.fnDisplay = (DISPFUNC) DisplayLabel;
        //设置标签文本的对齐方式
        pLblObj->iLblAlign = GUILABEL_ALIGN_LEFT;   //默认居左
        //如果文本资源已建立，设置文本的显示大小
        if (NULL != pLblObj->pLblText)
        {
            pLblObj->pLblText->uiViewWidth = uiLblWidth;
            pLblObj->pLblText->uiViewHeight = uiLblHeight;
        }
        //标签对象的画刷、画笔及字体设置
        pLblObj->pLblPen = NULL;        //默认使用当前画笔
        pLblObj->pLblBrush = NULL;      //默认使用当前画刷
        pLblObj->pLblFont = getGlobalFnt(EN_FONT_WHITE);       //默认使用当前字体
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pLblObj);
        //no break
    case -2:
    case -1:
        pLblObj = NULL;
        //no break
    default:
        break;
    }

    return pLblObj;
}


/***
  * 功能：
        删除标签对象
  * 参数：
        1.GUILABEL **ppLblObj:  指针的指针，指向需要销毁的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyLabel(GUILABEL **ppLblObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppLblObj是否为有效指针
        if (NULL == ppLblObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppLblObj所指向的是否为有效指针
        if (NULL == *ppLblObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //如果文本资源已建立，尝试销毁文本资源
        if (NULL != (*ppLblObj)->pLblText)
        {
            if (DestroyText(&((*ppLblObj)->pLblText)))
            {
                iErr = -3;
            }
        }
    }

    if (!iErr)
    {
        //销毁标签对象，并将指针置空
        free(*ppLblObj);
        *ppLblObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        直接输出指定的标签
  * 参数：
        1.GUILABEL *pLblObj:    标签指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int DisplayLabel(GUILABEL *pLblObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiX;
    GUIBRUSH *pBrush = NULL;
    GUIPEN *pPen = NULL;
    GUIFONT *pFont = NULL;

    if (!iErr)
    {
        //判断pLblObj是否为有效指针
        if (NULL == pLblObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断pLblObj是否可见和文本资源是否已建立
        if (!pLblObj->Visible.iEnable || (NULL == pLblObj->pLblText))
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //若文本点阵资源未装载，尝试装载文本点阵资源
        if (NULL == pLblObj->pLblText->ppTextMatrix)
        {
            if (LoadTextMatrix(pLblObj->pLblText))
            {
                iErr = -3;
            }
        }
    }
    
#if 0
    /*****框出文本显示区域*****/
    pPen = GetCurrPen();
	//设置画笔颜色
    unsigned int uiColor = pPen->uiPenColor;
    pPen->uiPenColor = 0xff0000;            //红色
    DrawLine(pLblObj->Visible.Area.Start.x, pLblObj->Visible.Area.Start.y, 
        pLblObj->Visible.Area.End.x, pLblObj->Visible.Area.Start.y);
    DrawLine(pLblObj->Visible.Area.Start.x, pLblObj->Visible.Area.Start.y, 
        pLblObj->Visible.Area.Start.x, pLblObj->Visible.Area.End.y);
    DrawLine(pLblObj->Visible.Area.End.x, pLblObj->Visible.Area.Start.y, 
        pLblObj->Visible.Area.End.x, pLblObj->Visible.Area.End.y);
    DrawLine(pLblObj->Visible.Area.Start.x, pLblObj->Visible.Area.End.y, 
        pLblObj->Visible.Area.End.x, pLblObj->Visible.Area.End.y);
    pPen->uiPenColor = uiColor;
#endif

    if (!iErr)
    {
        //根据标签文本的对齐方式，重设文本的放置位置
        switch (pLblObj->iLblAlign)
        {
        case GUILABEL_ALIGN_LEFT:
            //左对齐，文本的水平放置位置为起始点横坐标
            if (SetTextPlace(pLblObj->Visible.Area.Start.x, 
                             pLblObj->Visible.Area.Start.y, 
                             pLblObj->pLblText))
            {
                iErr = -4;
            }
            break;
        case GUILABEL_ALIGN_RIGHT:
            //求得标签长度
            uiX = pLblObj->Visible.Area.End.x
                - pLblObj->Visible.Area.Start.x
                + 1;
            //若标签长度超出文本显示宽度，计算偏移，否则偏移为0
            uiX = (uiX > pLblObj->pLblText->uiViewWidth) ? 
                  (uiX - pLblObj->pLblText->uiViewWidth) : 0;
            //右对齐，文本的水平放置位置为起始点横坐标加偏移
            if (SetTextPlace(pLblObj->Visible.Area.Start.x + uiX, 
                             pLblObj->Visible.Area.Start.y, 
                             pLblObj->pLblText))
            {
                iErr = -4;
            }
            break;
        case GUILABEL_ALIGN_CENTER:
            //求得标签长度
            uiX = pLblObj->Visible.Area.End.x
                - pLblObj->Visible.Area.Start.x
                + 1;
            //若标签长度超出文本显示宽度，计算偏移，否则偏移为0
            uiX = (uiX > pLblObj->pLblText->uiViewWidth) ? 
                  (uiX - pLblObj->pLblText->uiViewWidth) : 0;
            //居中对齐，文本的水平放置位置为起始点横坐标加偏移的一半
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

    if (!iErr)
    {
        //若已设置标签使用的画刷，重设当前画刷
        if (NULL != pLblObj->pLblBrush)
        {
            pBrush = GetCurrBrush();
            SetCurrBrush(pLblObj->pLblBrush);
        }
        //若已设置标签使用的画笔，重设当前画笔
        if (NULL != pLblObj->pLblPen)
        {
            pPen = GetCurrPen();
            SetCurrPen(pLblObj->pLblPen);
        }
        //若已设置标签使用的字体，重设当前字体
        if (NULL != pLblObj->pLblFont)
        {
            pFont = GetCurrFont();
            SetCurrFont(pLblObj->pLblFont);
        }
        //尝试显示文本资源
        if (DisplayText(pLblObj->pLblText))
        {
            iErr = -5;
        }
        //若已设置标签使用的画刷，恢复当前画刷
        if (NULL != pLblObj->pLblBrush)
        {
            SetCurrBrush(pBrush);
        }
        //若已设置标签使用的画笔，恢复当前画笔
        if (NULL != pLblObj->pLblPen)
        {
            SetCurrPen(pPen);
        }
        //若已设置标签使用的字体，恢复当前字体
        if (NULL != pLblObj->pLblFont)
        {
            SetCurrFont(pFont);
        }
    }

    return iErr;
}


/***
  * 功能：
        设置标签的有效区域
  * 参数：
        1.unsigned int uiStartX:    标签有效区域左上角横坐标，以左上角为基点
        2.unsigned int uiStartY:    标签有效区域左上角纵坐标，以左上角为基点
        3.unsigned int uiEndX:      标签有效区域右下角横坐标，以左上角为基点
        4.unsigned int uiEndY:      标签有效区域右下角纵坐标，以左上角为基点
        5.GUILABEL *pLblObj:        需要设置有效区域的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetLabelArea(unsigned int uiStartX, unsigned int uiStartY, 
                 unsigned int uiEndX, unsigned int uiEndY,
                 GUILABEL *pLblObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断uiEndX、uiEndY和pLblObj是否有效
        if (uiEndX < uiStartX || uiEndY < uiStartY || NULL == pLblObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //标签对象有效区域赋值
        pLblObj->Visible.Area.Start.x = uiStartX;
        pLblObj->Visible.Area.Start.y = uiStartY;
        pLblObj->Visible.Area.End.x = uiEndX;
        pLblObj->Visible.Area.End.y = uiEndY;
        //如果文本资源已建立，重设文本的显示大小及放置位置
        if (NULL != pLblObj->pLblText)
        {
            //uiViewWidth和uiViewHeight在DisplayLabel中加载点阵字库时修改，用于居中/居左/居右设置；
            //如过已经显示字符串后再进行lable位置变更，就不能再调用SetTextView，否则会导致偏移失效。
            /*SetTextView(uiEndX - uiStartX + 1, uiEndY - uiStartY + 1, 
                        pLblObj->pLblText);*/
            SetTextPlace(uiStartX, uiStartY, pLblObj->pLblText);
        }
    }

    return iErr;
}


/***
  * 功能：
        设置标签的可见性
  * 参数：
        1.int iEnable:          标签是否可见，0不可见，1可见
        5.GUILABEL *pLblObj:    需要设置可见性的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetLabelEnable(int iEnable, GUILABEL *pLblObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断pLblObj是否有效
        if (NULL == pLblObj)
        {
            iErr = -1;
        }
    }

    pLblObj->Visible.iEnable = iEnable;

    return 0;
}


/***
  * 功能：
        设置标签所使用的画刷
  * 参数：
        1.GUIBRUSH *pLblBrush:  用于标签对象的画刷
        2.GUILABEL *pLblObj:    需要设置所使用画刷的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetLabelBrush(GUIBRUSH *pLblBrush, GUILABEL *pLblObj)
{
    //判断pLblBrush和pLblObj是否有效
    if (NULL == pLblBrush || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->pLblBrush = pLblBrush;

    return 0;
}


/***
  * 功能：
        设置标签所使用的画笔
  * 参数：
        1.GUIPEN *pLblPen:      用于标签对象的画笔
        2.GUILABEL *pLblObj:    需要设置所使用画笔的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetLabelPen(GUIPEN *pLblPen, GUILABEL *pLblObj)
{
    //判断pLblPen和pLblObj是否有效
    if (NULL == pLblPen || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->pLblPen = pLblPen;

    return 0;
}


/***
  * 功能：
        设置标签所对应的字体
  * 参数：
        1.GUIFONT *pLblFont:    用于标签对象的字体
        2.GUILABEL *pLblObj:    需要设置所使用字体的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetLabelFont(GUIFONT *pLblFont, GUILABEL *pLblObj)
{
    //判断pLblFont和pLblObj是否有效
    if (NULL == pLblFont || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->pLblFont = pLblFont;

    return 0;
}


/***
  * 功能：
        设置标签文本的对齐方式
  * 参数：
        1.int iLblAlign:        标签文本对齐方式，0左，1右，2居中，默认0
        2.GUILABEL *pLblObj:    需要设置对齐方式的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
//设置标签文本的对齐方式
int SetLabelAlign(int iLblAlign, GUILABEL *pLblObj)
{
    //判断uiLblAlign和pLblObj是否有效
    if (iLblAlign > 2 || NULL == pLblObj)
    {
        return -1;
    }

    pLblObj->iLblAlign = iLblAlign;

    return 0;
}


/***
  * 功能：
        设置标签所对应的文本资源
  * 参数：
        1.unsigned short *pTextData:    用于建立文本资源的文本内容
        2.GUILABEL *pLblObj:            需要设置文本资源的标签对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetLabelText(unsigned short *pTextData, GUILABEL *pLblObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (!iErr)
    {
        //判断pTextData和pLblObj是否有效
        if (NULL == pTextData || NULL == pLblObj)
        {
            return -1;
        }
    }

    if (!iErr)
    {
        //设置标签所对应的文本资源
        if (NULL == pLblObj->pLblText)  //文本资源未建立，尝试建立文本资源
        {
            pLblObj->pLblText = CreateText(pTextData, 
                                           pLblObj->Visible.Area.Start.x, 
                                           pLblObj->Visible.Area.Start.y);
            if (NULL == pLblObj->pLblText)
            {
                iErr = -2;
            }
        }
        else                            //文本资源已建立，尝试重设文本资源
        {
            if (SetTextData(pTextData, pLblObj->pLblText))
            {
                iErr = -2;
            }
        }
    }

    if (!iErr)
    {
        //重设文本的显示大小
        pLblObj->pLblText->uiViewWidth = pLblObj->Visible.Area.End.x
                                       - pLblObj->Visible.Area.Start.x + 1;
        pLblObj->pLblText->uiViewHeight = pLblObj->Visible.Area.End.y 
                                        - pLblObj->Visible.Area.Start.y + 1;
    }

    return iErr;
}

