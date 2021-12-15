/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiimage.c
* 摘    要：  实现GUI图像类型的相关操作，及基本的图像绘制操作函数，为GUI图形控件
*             的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#include "guiimage.h"


/*************************************
* 为实现GUIIMAGE而需要引用的其他头文件
*************************************/
//#include ""


/***
  * 功能：
        根据指定的信息直接建立图像对象
  * 参数：
        1.unsigned int uiImgWidth:      图像的水平宽度，单位为像素
        2.unsigned int uiImgHeight:     图像的垂直高度，单位为像素
        3.unsigned int uiImgDepth:      图像中像素的颜色深度
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIIMAGE* CreateImage(unsigned int uiImgWidth, unsigned int uiImgHeight, 
                      unsigned int uiImgDepth)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIIMAGE *pImageObj = NULL;
    //临时变量定义
    unsigned int uiSize;

    if (!iErr)
    {
        //判断图像的宽度和高度是否有效
        if (0 == uiImgWidth || 0 == uiImgHeight)
        {
            iErr = -1;
        }
        //判断uiImgDepth是否为有效颜色深度
        if ((uiImgDepth != 1) && 
            (uiImgDepth != 8) && 
            (uiImgDepth != 24))
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //申请内存资源
        pImageObj = (GUIIMAGE *) malloc(sizeof(GUIIMAGE));
        if (NULL == pImageObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //为图像数据分配内存资源，分配时图像的每行数据都按字节对齐
        uiSize = round_up(uiImgWidth * uiImgDepth, 8) / 8;
        pImageObj->pImgData = (unsigned char *) malloc(uiSize * uiImgHeight);
        if (NULL == pImageObj->pImgData)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {
        //图像对象部分成员赋值
        pImageObj->uiImgWidth = uiImgWidth;
        pImageObj->uiImgHeight = uiImgHeight;
        pImageObj->uiImgDepth = uiImgDepth;
        pImageObj->uiDispWidth = uiImgWidth;    //默认显示全部宽度
        pImageObj->uiDispHeight = uiImgHeight;  //默认显示全部高度
        pImageObj->uiPlaceX = 0;
        pImageObj->uiPlaceY = 0;
        pImageObj->uiRealX = 0;
        pImageObj->uiRealY = 0;
        pImageObj->uiFgColor = COLOR_TRANS;     //全透明，前景无效
        pImageObj->uiBgColor = COLOR_TRANS;     //全透明，背景无效
        pImageObj->pRefPal = NULL;
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pImageObj);
        //no break
    case -2:
    case -1:
        pImageObj = NULL;
        //no break
    default:
        break;
    }

    return pImageObj;
}


/***
  * 功能：
        删除图像对象
  * 参数：
        1.GUIIMAGE **ppImgObj:  指针的指针，指向需要销毁的图像对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyImage(GUIIMAGE **ppImgObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppImgObj是否为有效指针
        if (NULL == ppImgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppImgObj所指向的是否为有效指针
        if (NULL == *ppImgObj)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {
        //释放资源
        free((*ppImgObj)->pImgData);
        //销毁图像对象，并将指针置空
        free(*ppImgObj);
        *ppImgObj = NULL;
    }

    return iErr;
}


/***
  * 功能：
        直接输出指定的图像
  * 参数：
        1.GUIIMAGE *pImgObj:    图像指针，不能为空
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（帧缓冲映射中的互斥锁），不能在持有锁的前提下调用
***/
int DisplayImage(GUIIMAGE *pImgObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIFBMAP *pFbmap = NULL;

    if (!iErr)
    {
        //判断pImgObj是否为有效指针
        if (NULL == pImgObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //得到当前帧缓冲映射对象并判断是否为有效指针
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {    
        //判断图像放置位置是否超出显示范围
        if ((pImgObj->uiPlaceX >= pFbmap->uiHorDisp) || 
            (pImgObj->uiPlaceY >= pFbmap->uiVerDisp))
        {
            iErr = -3;
        }
        //重定位绘制坐标
        pImgObj->uiRealX = pImgObj->uiPlaceX + pFbmap->uiHorOff;
        pImgObj->uiRealY = pImgObj->uiPlaceY + pFbmap->uiVerOff;
    }

    if (!iErr)
    {
        //调整图像可显示的宽度和高度，以符合帧缓冲映射的限制
        if (pImgObj->uiDispWidth + pImgObj->uiPlaceX > pFbmap->uiHorDisp)
        {
            pImgObj->uiDispWidth = pFbmap->uiHorDisp - pImgObj->uiPlaceX;
        }
        if (pImgObj->uiDispHeight + pImgObj->uiPlaceY > pFbmap->uiVerDisp)
        {
            pImgObj->uiDispHeight = pFbmap->uiVerDisp - pImgObj->uiPlaceY;
        }
        //输出图像
        MutexLock(&(pFbmap->Mutex));
        switch (pImgObj->uiImgDepth)
        {
        case 1:     //绘制1位色图像
            switch (pFbmap->uiPixelBits)
            {
          #if (PIXEL_8BITS)
            case 8:
                _Image1Fb8(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_8BITS
          #if (PIXEL_16BITS)
            case 16:
                _Image1Fb16(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_16BITS
          #if (PIXEL_24BITS)
            case 24:
                _Image1Fb24(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_24BITS
          #if (PIXEL_32BITS)
            case 32:
                _Image1Fb32(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_32BITS
            default:
                iErr = -4;
                break;
            }
            break;
        case 8:     //绘制8位色图像
            switch (pFbmap->uiPixelBits)
            {
          #if (PIXEL_8BITS)
            case 8:
                _Image8Fb8(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_8BITS
          #if (PIXEL_16BITS)
            case 16:
                _Image8Fb16(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_16BITS
          #if (PIXEL_24BITS)
            case 24:
                _Image8Fb24(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_24BITS
          #if (PIXEL_32BITS)
            case 32:
                _Image8Fb32(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_32BITS
            default:
                iErr = -4;
                break;
            }
            break;
        case 24:    //绘制24位色图像
            switch (pFbmap->uiPixelBits)
            {
          #if (PIXEL_8BITS)
            case 8:
                _Image24Fb8(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_8BITS
          #if (PIXEL_16BITS)
            case 16:
                _Image24Fb16(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_16BITS
          #if (PIXEL_24BITS)
            case 24:
                _Image24Fb24(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_24BITS
          #if (PIXEL_32BITS)
            case 32:
                _Image24Fb32(pImgObj, pFbmap);
                break;
          #endif    //PIXEL_32BITS
            default:
                iErr = -4;
                break;
            }
            break;
        default:
            iErr = -4;
            break;
        }
        MutexUnlock(&(pFbmap->Mutex));
        //设置同步标志，标记缓冲区需刷新
        SetFbmapSync(1, pFbmap);
    }

    return iErr;
}


/***
  * 功能：
        设置图像的放置位置
  * 参数：
        1.unsigned int uiPlaceX:    图像水平放置位置，以左上角为基点，绝对坐标
        2.unsigned int uiPlaceY:    图像垂直放置位置，以左上角为基点，绝对坐标
        3.GUIIMAGE *pImgObj:        需要设置放置位置的图像对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetImagePlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                  GUIIMAGE *pImgObj)
{
    //判断pImgObj是否有效
    if (NULL == pImgObj)
    {
        return -1;
    }

    pImgObj->uiPlaceX = uiPlaceX;
    pImgObj->uiPlaceY = uiPlaceY;

    return 0;
}


/***
  * 功能：
        设置图像的前背景颜色
  * 参数：
        1.unsigned int uiFgColor:   图像前景颜色值，uiImgDepth为1时使用
        2.unsigned int uiBgColor:   图像背景颜色值，uiImgDepth为1时使用
        3.GUIIMAGE *pImgObj:        需要设置前背景的图像对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetImageColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                  GUIIMAGE *pImgObj)
{
    //判断pImgObj是否有效
    if (NULL == pImgObj)
    {
        return -1;
    }

    pImgObj->uiFgColor = uiFgColor;
    pImgObj->uiBgColor = uiBgColor;

    return 0;
}


/***
  * 功能：
        设置图像引用的调色板
  * 参数：
        1.GUIPALETTE *pRefPal:  图像引用的调色板，当uiImgDepth为8时使用
        2.GUIIMAGE *pImgObj:    需要设置调色板的图像对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetImagePalette(GUIPALETTE *pRefPal, GUIIMAGE *pImgObj)
{
    //判断pRefPal和pImgObj是否为有效指针
    if (NULL == pRefPal || NULL == pImgObj)
    {
        return -1;
    }

    pImgObj->pRefPal = pRefPal;

    return 0;
}


/***
  * 功能：
        绘制1位色图像，应用于8位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_8BITS)
void _Image1Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    //首先将前景色fg与背景色bg转换为系统调色板中的颜色索引
    //转换的方法为:遍历调色板，并求各调色板中各颜色与fg和bg的方差
    //方差最小者的索引即为转换后的索引

    //输出image的过程与其他_Image1FbX()函数类似

    return;
}
#endif    //PIXEL_8BITS


/***
  * 功能：
        绘制1位色图像，应用于16位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_16BITS)
void _Image1Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //1位的图像数据，使用8位的指针进行访问
    UINT16 *p = pfbmap->pMapBuff;   //p指向16位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;
    UINT16 fg = ((pimg->uiFgColor & 0x0000F8) >> 3) |   //RGB565，B分量:低5位
                ((pimg->uiFgColor & 0x00FC00) >> 5) |   //RGB565，G分量:中间6位
                ((pimg->uiFgColor & 0xF80000) >> 8);    //RGB565，R分量:高5位
    UINT16 bg = ((pimg->uiBgColor & 0x0000F8) >> 3) |   //RGB565，B分量:低5位
                ((pimg->uiBgColor & 0x00FC00) >> 5) |   //RGB565，G分量:中间6位
                ((pimg->uiBgColor & 0xF80000) >> 8);    //RGB565，R分量:高5位
    UINT32 byte = round_up(w, 8) / 8;
    UINT32 bit, dot;

    p += pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes;
    off_p = pfbmap->uiHorRes - w;
    off_d = round_up(pimg->uiImgWidth, 8) / 8 - byte;
    t = byte;
    //针对标准1位色图像，前背景色皆有效
    if ((pimg->uiFgColor != COLOR_TRANS) && 
        (pimg->uiBgColor != COLOR_TRANS))
    {
        while (h--)
        {
            while (byte--)
            {
                dot = *d++;
                bit = 8;
                while (bit && w)
                {
                    bit--;
                    w--;
                    if (dot & 0x80)
                    {
                        *p++ = fg;
                    }
                    else
                    {
                        *p++ = bg;
                    }
                    dot <<= 1;
                }
            }
            p += off_p;
            d += off_d;
            w = pimg->uiDispWidth;
            byte = t;
        }
    }
    //针对特殊单色图像(如字模)，只有前景色有效
    if ((pimg->uiFgColor != COLOR_TRANS) && 
        (pimg->uiBgColor == COLOR_TRANS))
    {
        while (h--)
        {
            while (byte--)
            {
                dot = *d++;
                bit = 8;
                while (bit && w)
                {
                    bit--;
                    w--;
                    if (dot & 0x80)
                    {
                        *p++ = fg;
                    }
                    else
                    {
                        p++;
                    }
                    dot <<= 1;
                }
            }
            p += off_p;
            d += off_d;
            w = pimg->uiDispWidth;
            byte = t;
        }
    }

    return;
}
#endif    //PIXEL_16BITS


/***
  * 功能：
        绘制1位色图像，应用于24位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_24BITS)
void _Image1Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //1位的图像数据，使用8位的指针进行访问
    UINT8 *p = pfbmap->pMapBuff;    //p指向24位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;
    UINT8 *fg = (UINT8 *) &(pimg->uiFgColor);
    UINT8 *bg = (UINT8 *) &(pimg->uiBgColor);
    UINT32 byte = round_up(w, 8) / 8;
    UINT32 bit, dot;

    p += (pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes) * 3;
    off_p = (pfbmap->uiHorRes - w) * 3;
    off_d = round_up(pimg->uiImgWidth, 8) / 8 - byte;
    t = byte;
    //针对标准1位色图像，前背景色皆有效
    if ((pimg->uiFgColor != COLOR_TRANS) && 
        (pimg->uiBgColor != COLOR_TRANS))
    {
        while (h--)
        {
            while (byte--)
            {
                dot = *d++;
                bit = 8;
                while (bit && w)
                {
                    bit--;
                    w--;
                    if (dot & 0x80)
                    {
                        *p++ = fg[0];   //小端存储，B分量:byte0
                        *p++ = fg[1];   //小端存储，G分量:byte1
                        *p++ = fg[2];   //小端存储，R分量:byte2
                    }
                    else
                    {
                        *p++ = bg[0];   //小端存储，B分量:byte0
                        *p++ = bg[1];   //小端存储，G分量:byte1
                        *p++ = bg[2];   //小端存储，R分量:byte2
                    }
                    dot <<= 1;
                }
            }
            p += off_p;
            d += off_d;
            w = pimg->uiDispWidth;
            byte = t;
        }
    }
    //针对特殊单色图像(如字模)，只有前景色有效
    if ((pimg->uiFgColor != COLOR_TRANS) && 
        (pimg->uiBgColor == COLOR_TRANS))
    {
        while (h--)
        {
            while (byte--)
            {
                dot = *d++;
                bit = 8;
                while (bit && w)
                {
                    bit--;
                    w--;
                    if (dot & 0x80)
                    {
                        *p++ = fg[0];   //小端存储，B分量:byte0
                        *p++ = fg[1];   //小端存储，G分量:byte1
                        *p++ = fg[2];   //小端存储，R分量:byte2
                    }
                    else
                    {
                        p += 3;
                    }
                    dot <<= 1;
                }
            }
            p += off_p;
            d += off_d;
            w = pimg->uiDispWidth;
            byte = t;
        }
    }

    return;
}
#endif    //PIXEL_24BITS


/***
  * 功能：
        绘制1位色图像，应用于32位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_32BITS)
void _Image1Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //1位的图像数据，使用8位的指针进行访问
    UINT32 *p = pfbmap->pMapBuff;   //p指向32位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;
    UINT32 fg = pimg->uiFgColor;
    UINT32 bg = pimg->uiBgColor;
    UINT32 byte = round_up(w, 8) / 8;
    UINT32 bit, dot;

    p += pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes;
    off_p = pfbmap->uiHorRes - w;
    off_d = round_up(pimg->uiImgWidth, 8) / 8 - byte;
    t = byte;
    //针对标准1位色图像，前背景色皆有效
    if ((pimg->uiFgColor != COLOR_TRANS) && 
        (pimg->uiBgColor != COLOR_TRANS))
    {
        while (h--)
        {
            while (byte--)
            {
                dot = *d++;
                bit = 8;
                while (bit && w)
                {
                    bit--;
                    w--;
                    if (dot & 0x80)
                    {
                        *p++ = fg;
                    }
                    else
                    {
                        *p++ = bg;
                    }
                    dot <<= 1;
                }
            }
            p += off_p;
            d += off_d;
            w = pimg->uiDispWidth;
            byte = t;
        }
    }
    //针对特殊单色图像(如字模)，只有前景色有效
    if ((pimg->uiFgColor != COLOR_TRANS) && 
        (pimg->uiBgColor == COLOR_TRANS))
    {
        while (h--)
        {
            while (byte--)
            {
                dot = *d++;
                bit = 8;
                while (bit && w)
                {
                    bit--;
                    w--;
                    if (dot & 0x80)
                    {
                        *p++ = fg;
                    }
                    else
                    {
                        p++;
                    }
                    dot <<= 1;
                }
            }
            p += off_p;
            d += off_d;
            w = pimg->uiDispWidth;
            byte = t;
        }
    }

    return;
}
#endif    //PIXEL_32BITS


/***
  * 功能：
        绘制8位色图像，应用于8位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_8BITS)
void _Image8Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    //首先将图像调色板与系统调色板建立映射表map[256]
    //map[i]中下标号i表示图像调色板索引，map[i]表示对应的系统调色板索引
    //映射表建立的方法为:遍历系统调色板，并求系统调色板中各颜色
    //与图像调色板中某一颜色的方差，方差最小者即可建立映射关系

    //输出image的过程与其他_Image1FbX()函数类似
    //*p++ = map[*d++]

    return;
}
#endif    //PIXEL_8BITS


/***
  * 功能：
        绘制8位色图像，应用于16位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_16BITS)
void _Image8Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //8位的图像数据，使用8位的指针进行访问
    UINT16 *p = pfbmap->pMapBuff;   //p指向16位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;
    UINT8 *r = pimg->pRefPal->pRedValue;
    UINT8 *g = pimg->pRefPal->pGreenValue;
    UINT8 *b = pimg->pRefPal->pBlueValue;

    p += pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes;
    off_p = pfbmap->uiHorRes - w;
    off_d = pimg->uiImgWidth - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = ((b[*d] & 0xF8) >> 3) |  //RGB565，B分量:低5位
                   ((g[*d] & 0xFC) << 3) |  //RGB565，G分量:中间6位
                   ((r[*d] & 0xF8) << 8);   //RGB565，R分量:高5位
            d++;
        }
        p += off_p;
        d += off_d;
        w = t;
    }

    return;
}
#endif    //PIXEL_16BITS


/***
  * 功能：
        绘制8位色图像，应用于24位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_24BITS)
void _Image8Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //8位的图像数据，使用8位的指针进行访问
    UINT8 *p = pfbmap->pMapBuff;    //p指向24位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;
    UINT8 *r = pimg->pRefPal->pRedValue;
    UINT8 *g = pimg->pRefPal->pGreenValue;
    UINT8 *b = pimg->pRefPal->pBlueValue;

    p += (pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes) * 3;
    off_p = (pfbmap->uiHorRes - w) * 3;
    off_d = pimg->uiImgWidth - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = b[*d];   //小端存储，B分量:byte0
            *p++ = g[*d];   //小端存储，G分量:byte1
            *p++ = r[*d];   //小端存储，R分量:byte2
            d++;
        }
        p += off_p;
        d += off_d;
        w = t;
    }

    return;
}
#endif    //PIXEL_24BITS


/***
  * 功能：
        绘制8位色图像，应用于32位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_32BITS)
void _Image8Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //8位的图像数据，使用8位的指针进行访问
    UINT8 *p = pfbmap->pMapBuff;    //p指向32位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;
    UINT8 *r = pimg->pRefPal->pRedValue;
    UINT8 *g = pimg->pRefPal->pGreenValue;
    UINT8 *b = pimg->pRefPal->pBlueValue;

    p += (pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes) * 4;
    off_p = (pfbmap->uiHorRes - w) * 4;
    off_d = pimg->uiImgWidth - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = b[*d];   //小端存储，B分量:byte0
            *p++ = g[*d];   //小端存储，G分量:byte1
            *p++ = r[*d];   //小端存储，R分量:byte2
            p++;            //小端存储，忽略透明分量
            d++;
        }
        p += off_p;
        d += off_d;
        w = t;
    }

    return;
}
#endif    //PIXEL_32BITS


/***
  * 功能：
        绘制24位色图像，应用于8位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_8BITS)
void _Image24Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    //首先为24位色图像与与系统调色板之间建立映射map[4096]
    //map[i]中下标号i表示图像颜色值(RGB444)，map[i]表示对应的系统调色板索引
    //映射表建立的方法为:遍历系统调色板，并求系统调色板中各颜色
    //与图像颜色值的方差，方差最小者即可在系统调色板与图像颜色值间建立映射关系

    //输出image的过程与其他_Image1FbX()函数类似
    //*p++ = map[((d[2] & 0xF0) << 8) | (d[1] & 0xF0) | ((d[0] & 0xF0) >> 4)]

    return;
}
#endif    //PIXEL_8BITS


/***
  * 功能：
        绘制24位色图像，应用于16位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_16BITS)
void _Image24Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //24位的图像数据，使用8位的指针进行访问
    UINT16 *p = pfbmap->pMapBuff;   //p指向16位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;

    p += pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes;
    off_p = pfbmap->uiHorRes - w;
    off_d = (pimg->uiImgWidth - w) * 3;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = ((d[0] & 0xF8) >> 3) |   //RGB565，B分量:低5位
                   ((d[1] & 0xFC) << 3) |   //RGB565，G分量:中间6位
                   ((d[2] & 0xF8) << 8);    //RGB565，B分量:高5位
            d += 3;
        }
        p += off_p;
        d += off_d;
        w = t;
    }

    return;
}
#endif    //PIXEL_16BITS


/***
  * 功能：
        绘制24位色图像，应用于24位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_24BITS)
void _Image24Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //24位的图像数据，使用8位的指针进行访问
    UINT8 *p = pfbmap->pMapBuff;    //p指向24位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;

    p += (pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes) * 3;
    off_p = (pfbmap->uiHorRes - w) * 3;
    off_d = (pimg->uiImgWidth - w) * 3;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = *d++;    //小端存储，B分量:byte0
            *p++ = *d++;    //小端存储，G分量:byte1
            *p++ = *d++;    //小端存储，R分量:byte2
        }
        p += off_p;
        d += off_d;
        w = t;
    }

    return;
}
#endif    //PIXEL_24BITS


/***
  * 功能：
        绘制24位色图像，应用于32位色帧缓冲
  * 参数：
        1.GUIIMAGE *pimg:   要绘制的图像对象
        2.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
***/
#if (PIXEL_32BITS)
void _Image24Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap)
{
    UINT8 *d = pimg->pImgData;      //24位的图像数据，使用8位的指针进行访问
    UINT8 *p = pfbmap->pMapBuff;    //p指向32位帧缓冲
    UINT32 h = pimg->uiDispHeight;
    UINT32 w = pimg->uiDispWidth;
    UINT32 off_p, off_d, t;

    p += (pimg->uiRealX + pimg->uiRealY * pfbmap->uiHorRes) * 4;
    off_p = (pfbmap->uiHorRes - w) * 4;
    off_d = (pimg->uiImgWidth - w) * 3;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = *d++;    //小端存储，B分量:byte0
            *p++ = *d++;    //小端存储，G分量:byte1
            *p++ = *d++;    //小端存储，R分量:byte2
            p++;            //小端存储，忽略透明分量
        }
        p += off_p;
        d += off_d;
        w = t;
    }

    return;
}
#endif    //PIXEL_32BITS

