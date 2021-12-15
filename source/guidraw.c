/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All right reserved
*
* 文件名称：  guidraw.c
* 摘    要：  定义GUI基本绘图操作函数，为GUI图形控件的实现提供基础.
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guidraw.h"


/************************************
* 为实现GUIDRAW而需要引用的其他头文件
************************************/
#include "guipalette.h"
#include "guipen.h"


/***
  * 功能：
        根据指定的信息直接绘制像素
  * 参数：
        1.unsigned int uiPixelX:    像素点的横坐标
        2.unsigned int uiPixelY:    像素点的纵坐标
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（帧缓冲映射中的互斥锁），不能在持有锁的前提下调用
***/
int DrawPixel(unsigned int uiPixelX, unsigned int uiPixelY)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Pixel;
    unsigned int uiValue;

    if (!iErr)
    {
        //得到当前调色板对象并判断是否为有效指针
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //得到当前画笔对象并判断是否为有效指针
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {    
        //得到当前帧缓冲映射对象并判断是否为有效指针
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {    
        //确定像素点
        Pixel.x = uiPixelX;
        Pixel.y = uiPixelY;
        //判断像素点位置是否超出显示范围
        if ((Pixel.x >= pFbmap->uiHorDisp) || 
            (Pixel.y >= pFbmap->uiVerDisp))
        {
            iErr = -4;
        }
        //重定位绘制坐标
        Pixel.x += pFbmap->uiHorOff;
        Pixel.y += pFbmap->uiVerOff;
    }

    if (!iErr)
    {
        //根据帧缓冲像素深度，得到用于显示的颜色值，并调对应函数进行绘制
        switch (pFbmap->uiPixelBits)
        {
      #if (PIXEL_8BITS)
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8位色
            MutexLock(&(pFbmap->Mutex));
            _DrawPixel8(uiValue, uiPixelX, uiPixelY, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif    //PIXEL_8BITS
      #if (PIXEL_16BITS)
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            MutexLock(&(pFbmap->Mutex));
            _DrawPixel16(uiValue, uiPixelX, uiPixelY, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif    //PIXEL_16BITS
      #if (PIXEL_24BITS)
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            MutexLock(&(pFbmap->Mutex));
            _DrawPixel24(uiValue, uiPixelX, uiPixelY, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif    //PIXEL_24BITS
      #if (PIXEL_32BITS)
        case 32:
            uiValue = pPen->uiPenColor;
            MutexLock(&(pFbmap->Mutex));
            _DrawPixel32(uiValue, uiPixelX, uiPixelY, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif    //PIXEL_32BITS
        default:
            iErr = -5;
            break;
        }
        //设置同步标志，标记缓冲区需刷新
        SetFbmapSync(1, pFbmap);
    }

    return iErr;
}


/***
  * 功能：
        根据指定的信息直接绘制直线
  * 参数：
        1.unsigned int uiStartX:    直线起始点的横坐标
        2.unsigned int uiStartY:    直线起始点的纵坐标
        3.unsigned int uiEndX:      直线终点的横坐标
        4.unsigned int uiEndY:      直线终点的纵坐标
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（帧缓冲映射中的互斥锁），不能在持有锁的前提下调用
***/
int DrawLine(unsigned int uiStartX, unsigned int uiStartY, 
             unsigned int uiEndX, unsigned int uiEndY)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Start, End;
    unsigned int uiValue;

    if (!iErr)
    {
        //得到当前调色板对象并判断是否为有效指针
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //得到当前画笔对象并判断是否为有效指针
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {    
        //得到当前帧缓冲映射对象并判断是否为有效指针
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {    
        //确定起始点与终点
        Start.x = uiStartX;
        Start.y = uiStartY;
        End.x = uiEndX;
        End.y = uiEndY;
        if (uiStartY == uiEndY) //横坐标相等为水平线，要求End.x>Start.x
        {
            Start.x = (uiStartX < uiEndX) ? uiStartX : uiEndX;
            End.x = (uiEndX > uiStartX) ? uiEndX : uiStartX;
        }
        if (uiStartX == uiEndX) //纵坐标相等为垂直线，要求End.y>Start.y
        {
            Start.y = (uiStartY < uiEndY) ? uiStartY : uiEndY;
            End.y = (uiEndY > uiStartY) ? uiEndY : uiStartY;
        }
        //判断起始点位置与终点位置是否超出显示范围
        if ((Start.x >= pFbmap->uiHorDisp) || 
            (Start.y >= pFbmap->uiVerDisp) || 
            (End.x >= pFbmap->uiHorDisp) || 
            (End.y >= pFbmap->uiVerDisp))
        {
            iErr = -4;
        }
        //重定位绘制坐标
        Start.x += pFbmap->uiHorOff;
        Start.y += pFbmap->uiVerOff;
        End.x += pFbmap->uiHorOff;
        End.y += pFbmap->uiVerOff;
    }

    if (!iErr)
    {
        //根据帧缓冲像素深度，得到用于显示的颜色值，并调对应函数进行绘制
        switch (pFbmap->uiPixelBits)
        {
      #if (PIXEL_8BITS)
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8位色
            MutexLock(&(pFbmap->Mutex));
            if (uiStartY == uiEndY)         //横坐标相等，画水平线
            {
                _DrawHor8(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //纵坐标相等，画垂直线
            {
                _DrawVer8(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //横、纵坐标都不相等，画斜线
                (uiStartY != uiEndY))
            {
                _DrawBias8(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_8BITS
      #if (PIXEL_16BITS)
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            MutexLock(&(pFbmap->Mutex));
            if (uiStartY == uiEndY)         //横坐标相等，画水平线
            {
                _DrawHor16(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //纵坐标相等，画垂直线
            {
                _DrawVer16(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //横、纵坐标都不相等，画斜线
                (uiStartY != uiEndY))
            {
                _DrawBias16(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_16BITS
      #if (PIXEL_24BITS)
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            MutexLock(&(pFbmap->Mutex));
            if (uiStartY == uiEndY)         //横坐标相等，画水平线
            {
                _DrawHor24(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //纵坐标相等，画垂直线
            {
                _DrawVer24(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //横、纵坐标都不相等，画斜线
                (uiStartY != uiEndY))
            {
                _DrawBias24(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_24BITS
      #if (PIXEL_32BITS)
        case 32:
            uiValue = pPen->uiPenColor;
            MutexLock(&(pFbmap->Mutex));
            if (uiStartY == uiEndY)         //横坐标相等，画水平线
            {
                _DrawHor32(uiValue, Start.x, Start.y, End.x, pFbmap);
            }
            if (uiStartX == uiEndX)         //纵坐标相等，画垂直线
            {
                _DrawVer32(uiValue, Start.x, Start.y, End.y, pFbmap);
            }
            if ((uiStartX != uiEndX) &&     //横、纵坐标都不相等，画斜线
                (uiStartY != uiEndY))
            {
                _DrawBias32(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            }
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_32BITS
        default:
            iErr = -5;
            break;
        }
        //设置同步标志，标记缓冲区需刷新
        SetFbmapSync(1, pFbmap);
    }

    return iErr;
}


/***
  * 功能：
        根据指定的信息直接绘制矩形
  * 参数：
        1.unsigned int uiStartX:    矩形起始点的横坐标
        2.unsigned int uiStartY:    矩形起始点的纵坐标
        3.unsigned int uiEndX:      矩形终点的横坐标
        4.unsigned int uiEndY:      矩形终点的纵坐标
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（帧缓冲映射中的互斥锁），不能在持有锁的前提下调用
***/
int DrawRectangle(unsigned int uiStartX, unsigned int uiStartY, 
                  unsigned int uiEndX, unsigned int uiEndY)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Start, End;
    unsigned int uiValue;

    if (!iErr)
    {
        //得到当前调色板对象并判断是否为有效指针
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //得到当前画笔对象并判断是否为有效指针
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {    
        //得到当前帧缓冲映射对象并判断是否为有效指针
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {    
        //确定起始点与终点
        Start.x = (uiStartX < uiEndX) ? uiStartX : uiEndX;
        Start.y = (uiStartY < uiEndY) ? uiStartY : uiEndY;
        End.x = (uiEndX > uiStartX) ? uiEndX : uiStartX;
        End.y = (uiEndY > uiStartY) ? uiEndY : uiStartY;
        //判断起始点位置与终点位置是否超出显示范围
        if ((Start.x >= pFbmap->uiHorDisp) || 
            (Start.y >= pFbmap->uiVerDisp) || 
            (End.x >= pFbmap->uiHorDisp) || 
            (End.y >= pFbmap->uiVerDisp))
        {
            iErr = -4;
        }
        //重定位绘制坐标
        Start.x += pFbmap->uiHorOff;
        Start.y += pFbmap->uiVerOff;
        End.x += pFbmap->uiHorOff;
        End.y += pFbmap->uiVerOff;
    }

    if (!iErr)
    {
        //根据帧缓冲像素深度，得到用于显示的颜色值，并调对应函数进行绘制
        switch (pFbmap->uiPixelBits)
        {
      #if (PIXEL_8BITS)
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8位色
            MutexLock(&(pFbmap->Mutex));
            _DrawHor8(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor8(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer8(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer8(uiValue, End.x, Start.y, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_8BITS
      #if (PIXEL_16BITS)
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            MutexLock(&(pFbmap->Mutex));
            _DrawHor16(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor16(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer16(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer16(uiValue, End.x, Start.y, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_16BITS
      #if (PIXEL_24BITS)
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            MutexLock(&(pFbmap->Mutex));
            _DrawHor24(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor24(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer24(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer24(uiValue, End.x, Start.y, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_24BITS
      #if (PIXEL_32BITS)
        case 32:
            uiValue = pPen->uiPenColor;
            MutexLock(&(pFbmap->Mutex));
            _DrawHor32(uiValue, Start.x, Start.y, End.x, pFbmap);
            _DrawHor32(uiValue, Start.x, End.y, End.x, pFbmap);
            _DrawVer32(uiValue, Start.x, Start.y, End.y, pFbmap);
            _DrawVer32(uiValue, End.x, Start.y, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_32BITS
        default:
            iErr = -5;
            break;
        }
        //设置同步标志，标记缓冲区需刷新
        SetFbmapSync(1, pFbmap);
    }

    return iErr;
}


/***
  * 功能：
        根据指定的信息直接绘制圆形
  * 参数：
        1.unsigned int uiPointX:    原点的横坐标
        2.unsigned int uiPointY:    原点的纵坐标
        3.unsigned int uiCircleR:   圆的半径
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（帧缓冲映射中的互斥锁），不能在持有锁的前提下调用
***/
int DrawCircle(unsigned int uiPointX, unsigned int uiPointY, 
               unsigned int uiCircleR)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    unsigned int uiValue;

    if (!iErr)
    {
        //得到当前调色板对象并判断是否为有效指针
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //得到当前画笔对象并判断是否为有效指针
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {    
        //得到当前帧缓冲映射对象并判断是否为有效指针
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {    
        //判断原点位置与半径是否超出显示范围
        if ((uiPointX < uiCircleR) || 
            (uiPointY < uiCircleR) || 
            (uiPointX + uiCircleR >= pFbmap->uiHorDisp) || 
            (uiPointY + uiCircleR >= pFbmap->uiVerDisp))
        {
            iErr = -4;
        }
        //重定位绘制坐标
        uiPointX += pFbmap->uiHorOff;
        uiPointY += pFbmap->uiVerOff;
    }

    if (!iErr)
    {
        //根据帧缓冲像素深度，得到用于显示的颜色值，并调对应函数进行绘制
        switch (pFbmap->uiPixelBits)
        {
      #if (PIXEL_8BITS)
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8位色
            MutexLock(&(pFbmap->Mutex));
            _DrawCircle8(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_8BITS
      #if (PIXEL_16BITS)
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            MutexLock(&(pFbmap->Mutex));
            _DrawCircle16(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_16BITS
      #if (PIXEL_24BITS)
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            MutexLock(&(pFbmap->Mutex));
            _DrawCircle24(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_24BITS
      #if (PIXEL_32BITS)
        case 32:
            uiValue = pPen->uiPenColor;
            MutexLock(&(pFbmap->Mutex));
            _DrawCircle32(uiValue, uiPointX, uiPointY, uiCircleR, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_32BITS
        default:
            iErr = -5;
            break;
        }
    }

    return iErr;
}


/***
  * 功能：
        根据指定的信息直接绘制块
  * 参数：
        1.unsigned int uiStartX:    块起始点的横坐标
        2.unsigned int uiStartY:    块起始点的纵坐标
        3.unsigned int uiEndX:      块终点的横坐标
        4.unsigned int uiEndY:      块终点的纵坐标
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（帧缓冲映射中的互斥锁），不能在持有锁的前提下调用
***/
int DrawBlock(unsigned int uiStartX, unsigned int uiStartY, 
              unsigned int uiEndX, unsigned int uiEndY)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    GUIPALETTE *pPal = NULL;
    GUIPEN *pPen = NULL;
    GUIFBMAP *pFbmap = NULL;
    GUIPOINT Start, End;
    unsigned int uiValue;

    if (!iErr)
    {
        //得到当前调色板对象并判断是否为有效指针
        pPal = GetCurrPalette();
        if (NULL == pPal)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //得到当前画笔对象并判断是否为有效指针
        pPen = GetCurrPen();
        if (NULL == pPen)
        {
            iErr = -2;
        }
    }

    if (!iErr)
    {    
        //得到当前帧缓冲映射对象并判断是否为有效指针
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iErr = -3;
        }
    }

    if (!iErr)
    {    
        //确定起始点与终点
        Start.x = (uiStartX < uiEndX) ? uiStartX : uiEndX;
        Start.y = (uiStartY < uiEndY) ? uiStartY : uiEndY;
        End.x = (uiEndX > uiStartX) ? uiEndX : uiStartX;
        End.y = (uiEndY > uiStartY) ? uiEndY : uiStartY;
        //判断起始点位置与终点位置是否超出显示范围
        if ((Start.x >= pFbmap->uiHorDisp) || 
            (Start.y >= pFbmap->uiVerDisp) || 
            (End.x >= pFbmap->uiHorDisp) || 
            (End.y >= pFbmap->uiVerDisp))
        {
            iErr = -4;
        }
        //重定位绘制坐标
        Start.x += pFbmap->uiHorOff;
        Start.y += pFbmap->uiVerOff;
        End.x += pFbmap->uiHorOff;
        End.y += pFbmap->uiVerOff;
    }

    if (!iErr)
    {
        //根据帧缓冲像素深度，得到用于显示的颜色值，并调对应函数进行绘制
        switch (pFbmap->uiPixelBits)
        {
      #if (PIXEL_8BITS)
        case 8:
            uiValue = MatchPaletteColor(pPen->uiPenColor, pPal);    //8位色
            MutexLock(&(pFbmap->Mutex));
            _DrawBlock8(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_8BITS
      #if (PIXEL_16BITS)
        case 16:
            uiValue = ((pPen->uiPenColor & 0x0000F8) >> 3) |        //RGB565
                      ((pPen->uiPenColor & 0x00FC00) >> 5) |        //RGB565
                      ((pPen->uiPenColor & 0xF80000) >> 8);         //RGB565
            MutexLock(&(pFbmap->Mutex));
            _DrawBlock16(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_16BITS
      #if (PIXEL_24BITS)
        case 24:
            uiValue = pPen->uiPenColor & 0xFFFFFF;                  //RGB888
            MutexLock(&(pFbmap->Mutex));
            _DrawBlock24(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_24BITS
      #if (PIXEL_32BITS)
        case 32:
            uiValue = pPen->uiPenColor;
            MutexLock(&(pFbmap->Mutex));
            _DrawBlock32(uiValue, Start.x, Start.y, End.x, End.y, pFbmap);
            MutexUnlock(&(pFbmap->Mutex));
            break;
      #endif  //PIXEL_32BITS
        default:
            iErr = -5;
            break;
        }
        //设置同步标志，标记缓冲区需刷新
        SetFbmapSync(1, pFbmap);
    }

    return iErr;
}

//根据指定的信息直接绘制箭头(uiWidth，uiheight:夹角位置的宽度、高度)
int DrawArrow(unsigned int uiStartX, unsigned int uiStartY,
              unsigned int uiEndX, unsigned int uiEndY,
              unsigned int uiWidth, unsigned int uiheight, 
              unsigned int uiDirction)
{
    GUIPEN *pPen;
    UINT32 iColor;

    //获得画笔，设置曲线颜色
    pPen = GetCurrPen();

    //设置画笔颜色
    iColor = pPen->uiPenColor;
    pPen->uiPenColor = 0xff0000;
    //直线段
    DrawLine(uiStartX, uiStartY, uiEndX, uiEndY);
    DrawLine(uiStartX, uiStartY + 1, uiEndX, uiEndY + 1); //加粗显示
    //根据箭头方向绘制线段
    switch (uiDirction)
    {
    case ARROW_UP://暂无
        break;
    case ARROW_DOWN://暂无
        break;
    case ARROW_LEFT:
        DrawLine(uiStartX, uiStartY, uiStartX + uiWidth, uiStartY - uiheight);
        DrawLine(uiStartX, uiStartY, uiStartX + uiWidth, uiStartY + uiheight);
        DrawLine(uiStartX, uiStartY + 1, uiStartX + uiWidth, uiStartY - uiheight + 1); //加粗显示
        DrawLine(uiStartX, uiStartY + 1, uiStartX + uiWidth, uiStartY + uiheight + 1); //加粗显示
        break;
    case ARROW_RIGHT:
        DrawLine(uiStartX + ((uiEndX - uiStartX) - uiWidth), uiStartY - uiheight, uiEndX, uiEndY);
        DrawLine(uiStartX + ((uiEndX - uiStartX) - uiWidth), uiStartY - uiheight + 1, uiEndX, uiEndY + 1); //加粗显示
        DrawLine(uiStartX + ((uiEndX - uiStartX) - uiWidth), uiStartY + uiheight, uiEndX, uiEndY);
        DrawLine(uiStartX + ((uiEndX - uiStartX) - uiWidth), uiStartY + uiheight + 1, uiEndX, uiEndY + 1); //加粗显示
        break;
    default:
        break;
    }
    
    pPen->uiPenColor = iColor;

    return 0;
}

/***
  * 功能：
        绘制单个像素，应用于8位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值   
        2.UINT32 pixel_x:   点的横坐标
        3.UINT32 pixel_y:   点的纵坐标
        4.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_8BITS)
void _DrawPixel8(COLOR value,
                 UINT32 pixel_x, UINT32 pixel_y,
                 GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;

    p += pixel_x + pixel_y * pfbmap->uiHorRes;
    *p = d;

    return;
}
#endif  //PIXEL_8BITS


/***
  * 功能：
       绘制单个像素，应用于16位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值   
        2.UINT32 pixel_x:   点的横坐标
        3.UINT32 pixel_y:   点的纵坐标
        4.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_16BITS)
void _DrawPixel16(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;

    p += pixel_x + pixel_y * pfbmap->uiHorRes;
    *p = d;

    return;
}
#endif  //PIXEL_16BITS


/***
  * 功能：
       绘制单个像素，应用于24位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值   
        2.UINT32 pixel_x:   点的横坐标
        3.UINT32 pixel_y:   点的纵坐标
        4.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_24BITS)
void _DrawPixel24(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (pixel_x + pixel_y * pfbmap->uiHorRes) * 3;
    *p++ = b;   //小端存储，B分量:byte0
    *p++ = g;   //小端存储，G分量:byte1
    *p++ = r;   //小端存储，R分量:byte2

    return ;
}
#endif  //PIXEL_24BITS


/***
  * 功能：
       绘制单个像素，应用于32位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值   
        2.UINT32 pixel_x:   点的横坐标
        3.UINT32 pixel_y:   点的纵坐标
        4.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_32BITS)
void _DrawPixel32(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;

    p += pixel_x + pixel_y * pfbmap->uiHorRes;
    *p = d;

    return ;
}
#endif  //PIXEL_32BITS


/***
  * 功能：
       绘制水平线，应用于8位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_8BITS)
void _DrawHor8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_x, 
               GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p++ = d;
    }

    return;
}
#endif  //PIXEL_8BITS


/***
  * 功能：
       绘制水平线，应用于16位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_16BITS)
void _DrawHor16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p++ = d;
    }
 
    return;
}
#endif  //PIXEL_16BITS


/***
  * 功能：
       绘制水平线，应用于24位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_24BITS)
void _DrawHor24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x + 1;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    while (loop--)
    {
        *p++ = b;   //小端存储，B分量:byte0
        *p++ = g;   //小端存储，G分量:byte1
        *p++ = r;   //小端存储，R分量:byte2
    }

    return;
}
#endif  //PIXEL_24BITS


/***
  * 功能：
       绘制水平线，应用于32位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_32BITS)
void _DrawHor32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    UINT32 loop = end_x - start_x + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p++ = d;
    }

    return;
}
#endif  //PIXEL_32BITS


/***
  * 功能：
        绘制垂直线，应用于8位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_y:     结束点的纵坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
 * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_8BITS)
void _DrawVer8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_y, 
               GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    { 
        *p = d;
        p += pfbmap->uiHorRes;
    }

    return;
}
#endif  //PIXEL_8BITS


/***
  * 功能：
        绘制垂直线，应用于16位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_y:     结束点的纵坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_16BITS)
void _DrawVer16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap)
{
    UINT16 d =(UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;
    
    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p = d;
        p += pfbmap->uiHorRes;
    }

    return;
}
#endif  //PIXEL_16BITS


/***
  * 功能：
        绘制垂直线，应用于24位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_y:     结束点的纵坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_24BITS)
void _DrawVer24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap)
{
    UINT32 d =(UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    while (loop--)
    {
        *(p + 0) = b;   //小端存储，B分量:byte0
        *(p + 1) = g;   //小端存储，G分量:byte1
        *(p + 2) = r;   //小端存储，R分量:byte2
        p += pfbmap->uiHorRes * 3;
    }

    return;
}
#endif  //PIXEL_24BITS


/***
  * 功能：
        绘制垂直线，应用于32位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_y:     结束点的纵坐标
        5.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_32BITS)
void _DrawVer32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap)
 {
    UINT32 d =(UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    UINT32 loop = end_y - start_y + 1;

    p += start_x + start_y * pfbmap->uiHorRes;
    while (loop--)
    {
        *p = d;
        p += pfbmap->uiHorRes;
    }

    return;
}
#endif  //PIXEL_32BITS


/***
  * 功能：
        绘制斜线，应用于8位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.UINT32 end_y:     结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_8BITS)
void _DrawBias8(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, UINT32 end_y, 
                GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;

    if ((dy = end_y - start_y) > 0)
    {
        t = 1;
        p += start_x + start_y * pfbmap->uiHorRes;
    }
    else
    {   
        dy = -dy;
        t = -1;
        p += end_x + end_y * pfbmap->uiHorRes;
    }
    if ((dx = end_x - start_x) < 0)
    {
        dx = -dx ;
        t = -t;
    }
    if (dx >dy)
    {
        sx = t;
        sy = pfbmap->uiHorRes;
    }
    else
    {
        sx = pfbmap->uiHorRes;
        sy = t;
        t = dx;
        dx = dy;
        dy = t;
    }
    for (t=0, e= -dx; t<= dx; t++)
    {
        *p = d;
        p += sx;

        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}
#endif  //PIXEL_8BITS


/***
  * 功能：
        绘制斜线，应用于16位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.UINT32 end_y:     结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_16BITS)
void _DrawBias16(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;

    if ((dy = end_y - start_y) >0)
    {
        t = 1;
        p += start_x + start_y * pfbmap->uiHorRes;
    }
    else
    {
        dy = -dy;
        t = -1;
        p += end_x + end_y * pfbmap->uiHorRes;
    }
    if ((dx = end_x - start_x) < 0 )
    {
        dx = -dx ;
        t = -t;
    }
    if (dx > dy)
    {
        sx = t;
        sy = pfbmap->uiHorRes;
    }
    else
    {
        sx = pfbmap->uiHorRes;
        sy = t;
        t = dx;
        dx = dy;
        dy = t;
    }
    for (t=0, e= -dx; t <= dx; t++)
    {
        *p = d;
        p += sx;

        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}
#endif  //PIXEL_16BITS


/***
  * 功能：
        绘制斜线，应用于24位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.UINT32 end_y:     结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_24BITS)
void _DrawBias24(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    if ((dy = end_y - start_y) > 0)
    {
        t = 1;
        p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    }
    else
    {
        dy = -dy;
        t = -1;
        p += (end_x + end_y * pfbmap->uiHorRes) * 3;
    }
    if ((dx = end_x - start_x) < 0)
    {
        dx = -dx;
        t = -t;
    }
    if (dx > dy)
    {
        sx = t * 3;
        sy = pfbmap->uiHorRes * 3;
    }
    else
    {
        sx = pfbmap->uiHorRes * 3;
        sy = t * 3;
        t = dx;
        dx = dy;
        dy = t;
    }
    for (t = 0, e = -dx; t <= dx; t++)
    {
        *(p + 0) = b;   //小端存储，B分量:byte0
        *(p + 1) = g;   //小端存储，G分量:byte1
        *(p + 2) = r;   //小端存储，R分量:byte2
        p += sx;
        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}
#endif  //PIXEL_24BITS


/***
  * 功能：
        绘制斜线，应用于32位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   起始点的横坐标
        3.UINT32 start_y:   起始点的纵坐标
        4.UINT32 end_x:     结束点的横坐标
        5.UINT32 end_y:     结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_32BITS)
void _DrawBias32(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    INT32 dx, dy, sx, sy, e, t;

    if ((dy = end_y - start_y) >0)
    {
        t = 1;
        p += start_x + start_y * pfbmap->uiHorRes;
    }
    else
    {
        dy = -dy;
        t = -1;
        p += end_x + end_y * pfbmap->uiHorRes;
    }
    if ((dx = end_x - start_x) < 0 )
    {
        dx = -dx ;
        t = -t;
    }
    if (dx > dy)
    {
        sx = t;
        sy = pfbmap->uiHorRes;
    }
    else
    {
        sx = pfbmap->uiHorRes;
        sy = t;
        t = dx;
        dx = dy;
        dy =t;
    }
    for (t=0, e= -dx; t <= dx; t++)
    {
        *p = d;
        p += sx;

        if ((e += 2 * dy) >= 0)
        {
            p += sy;
            e -= 2 * dx;
        }
    }

    return;
}
#endif  //PIXEL_32BITS


/***
  * 功能：
       绘制空心圆，应用于8位色帧缓冲
  * 参数：
       1.COLOR value:       颜色值
       2.UINT32 point_x:    圆心横坐标
       3.UINT32 point_y:    圆心纵坐标
       4.UINT32 radius:     圆半径
       5.GUIFBMAP *pfbmap:  绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_8BITS)
void _DrawCircle8(COLOR value,
                  UINT32 point_x, UINT32 point_y, 
                  UINT32 radius,
                  GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;

    p += point_x + point_y * pfbmap->uiHorRes;
    xp = 0;
    yp = radius * 256 ;
    dy = WINDOW_WIDTH - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = y * pfbmap->uiHorRes + x;
        sym2 = sym1 - 2 * x;
        sym3 = x * pfbmap->uiHorRes + y;
        sym4 = sym3 - 2 * y;

        *(p + sym1) = d;
        *(p + sym2) = d;
        *(p - sym1) = d;
        *(p - sym2) = d;
        *(p + sym3) = d;
        *(p + sym4) = d;
        *(p - sym3) = d;
        *(p - sym4) = d;

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}
#endif  //PIXEL_8BITS


/***
  * 功能：
       绘制空心圆，应用于16位色帧缓冲
  * 参数：
       1.COLOR value:       颜色值
       2.UINT32 point_x:    圆心横坐标
       3.UINT32 point_y:    圆心纵坐标
       4.UINT32 radius:     圆半径
       5.GUIFBMAP *pfbmap:  绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_16BITS)
void _DrawCircle16(COLOR value,
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius,
                   GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;

    p += point_x + point_y * pfbmap->uiHorRes;
    xp = 0;
    yp = radius * 256 ;
    dy = WINDOW_WIDTH - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = y * pfbmap->uiHorRes + x;
        sym2 = sym1 - 2 * x;
        sym3 = x * pfbmap->uiHorRes + y;
        sym4 = sym3 - 2 * y;

        *(p + sym1) = d;
        *(p + sym2) = d;
        *(p - sym1) = d;
        *(p - sym2) = d;
        *(p + sym3) = d;
        *(p + sym4) = d;
        *(p - sym3) = d;
        *(p - sym4) = d;

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}
#endif  //PIXEL_16BITS


/***
  * 功能：
       绘制空心圆，应用于24位色帧缓冲
  * 参数：
       1.COLOR value:       颜色值
       2.UINT32 point_x:    圆心横坐标
       3.UINT32 point_y:    圆心纵坐标
       4.UINT32 radius:     圆半径
       5.GUIFBMAP *pfbmap:  绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_24BITS)
void _DrawCircle24(COLOR value,
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius,
                   GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (point_x + point_y * pfbmap->uiHorRes) * 3;
    xp = 0;
    yp = radius * 256 ;
    dy = WINDOW_WIDTH - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = (y * pfbmap->uiHorRes + x) * 3;
        sym2 = sym1 - 2 * x * 3;
        sym3 = (x * pfbmap->uiHorRes + y) * 3;
        sym4 = sym3 - 2 * y * 3;

        *(p + sym1 + 0) = b;   //小端存储，B分量:byte0
        *(p + sym1 + 1) = g;   //小端存储，G分量:byte1
        *(p + sym1 + 2) = r;   //小端存储，R分量:byte2
        *(p + sym2 + 0) = b;   //小端存储，B分量:byte0
        *(p + sym2 + 1) = g;   //小端存储，G分量:byte1
        *(p + sym2 + 2) = r;   //小端存储，R分量:byte2
        *(p - sym1 + 0) = b;   //小端存储，B分量:byte0
        *(p - sym1 + 1) = g;   //小端存储，G分量:byte1
        *(p - sym1 + 2) = r;   //小端存储，R分量:byte2
        *(p - sym2 + 0) = b;   //小端存储，B分量:byte0
        *(p - sym2 + 1) = g;   //小端存储，G分量:byte1
        *(p - sym2 + 2) = r;   //小端存储，R分量:byte2
        *(p + sym3 + 0) = b;   //小端存储，B分量:byte0
        *(p + sym3 + 1) = g;   //小端存储，G分量:byte1
        *(p + sym3 + 2) = r;   //小端存储，R分量:byte2
        *(p + sym4 + 0) = b;   //小端存储，B分量:byte0
        *(p + sym4 + 1) = g;   //小端存储，G分量:byte1
        *(p + sym4 + 2) = r;   //小端存储，R分量:byte2
        *(p - sym3 + 0) = b;   //小端存储，B分量:byte0
        *(p - sym3 + 1) = g;   //小端存储，G分量:byte1
        *(p - sym3 + 2) = r;   //小端存储，R分量:byte2
        *(p - sym4 + 0) = b;   //小端存储，B分量:byte0
        *(p - sym4 + 1) = g;   //小端存储，G分量:byte1
        *(p - sym4 + 2) = r;   //小端存储，R分量:byte2

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}
#endif  //PIXEL_24BITS


/***
  * 功能：
       绘制空心圆，应用于32位色帧缓冲
  * 参数：
       1.COLOR value:       颜色值
       2.UINT32 point_x:    圆心横坐标
       3.UINT32 point_y:    圆心纵坐标
       4.UINT32 radius:     圆半径
       5.GUIFBMAP *pfbmap:  绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_32BITS)
void _DrawCircle32(COLOR value,
                   UINT32 point_x, UINT32 point_y,
                   UINT32 radius,
                   GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;

    p += point_x + point_y * pfbmap->uiHorRes;
    xp = 0;
    yp = radius * 256 ;
    dy = WINDOW_WIDTH - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = y * pfbmap->uiHorRes + x;
        sym2 = sym1 - 2 * x;
        sym3 = x * pfbmap->uiHorRes + y;
        sym4 = sym3 - 2 * y;

        *(p + sym1) = d;
        *(p + sym2) = d;
        *(p - sym1) = d;
        *(p - sym2) = d;
        *(p + sym3) = d;
        *(p + sym4) = d;
        *(p - sym3) = d;
        *(p - sym4) = d;

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }

    return;
}
#endif  //PIXEL_32BITS


/***
  * 功能：
        绘制填充块，应用于8位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   块起始点的横坐标
        3.UINT32 start_y:   块起始点的纵坐标
        4.UINT32 end_x:     块结束点的横坐标
        5.UINT32 end_y:     块结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_8BITS)
void _DrawBlock8(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap)
{
    UINT8 d = (UINT8)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;

    p += start_x + start_y * pfbmap->uiHorRes;
    offset = pfbmap->uiHorRes - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = d;
        }
        p += offset;
        w= t;
    }

    return;
}
#endif  //PIXEL_8BITS


/***
  * 功能：
        绘制填充块，应用于16位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   块起始点的横坐标
        3.UINT32 start_y:   块起始点的纵坐标
        4.UINT32 end_x:     块结束点的横坐标
        5.UINT32 end_y:     块结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_16BITS)
void _DrawBlock16(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap)
{
    UINT16 d = (UINT16)value;
    UINT16 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;

    p += start_x + start_y * pfbmap->uiHorRes;
    offset = pfbmap->uiHorRes - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = d;
        }
        p += offset;
        w= t;
    }

    return;
}
#endif  //PIXEL_16BITS


/***
  * 功能：
        绘制填充块，应用于24位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   块起始点的横坐标
        3.UINT32 start_y:   块起始点的纵坐标
        4.UINT32 end_x:     块结束点的横坐标
        5.UINT32 end_y:     块结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
       无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_24BITS)
void _DrawBlock24(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT8 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;
    UINT8 r = (d & 0xFF0000) >> 16;
    UINT8 g = (d & 0xFF00) >> 8;
    UINT8 b = d & 0xFF;

    p += (start_x + start_y * pfbmap->uiHorRes) * 3;
    offset = (pfbmap->uiHorRes - w) * 3;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = b;   //小端存储，B分量:byte0
            *p++ = g;   //小端存储，G分量:byte1
            *p++ = r;   //小端存储，R分量:byte2
        }
        p += offset;
        w = t;
    }

    return;
}
#endif  //PIXEL_24BITS


/***
  * 功能：
        绘制填充块，应用于32位色帧缓冲
  * 参数：
        1.COLOR value:      颜色值
        2.UINT32 start_x:   块起始点的横坐标
        3.UINT32 start_y:   块起始点的纵坐标
        4.UINT32 end_x:     块结束点的横坐标
        5.UINT32 end_y:     块结束点的纵坐标
        6.GUIFBMAP *pfbmap: 绘制操作所针对的帧缓冲映射对象
  * 返回：
        无
  * 备注：
        函数使用实际坐标系，且线宽为1
***/
#if (PIXEL_32BITS)
void _DrawBlock32(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap)
{
    UINT32 d = (UINT32)value;
    UINT32 *p = pfbmap->pMapBuff;
    UINT32 h = end_y - start_y + 1;
    UINT32 w = end_x - start_x + 1;
    UINT32 offset, t;

    p += start_x + start_y * pfbmap->uiHorRes;
    offset = pfbmap->uiHorRes - w;
    t = w;
    while (h--)
    {
        while (w--)
        {
            *p++ = d;
        }
        p += offset;
        w = t;
    }

    return;
}
#endif  //PIXEL_32BITS

