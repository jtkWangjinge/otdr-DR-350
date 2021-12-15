/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部 
*
* All rights reserved
*
* 文件名称：  guidraw.h
* 摘    要：  声明GUI基本绘图操作函数，为GUI图形控件的实现提供基础
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：
*******************************************************************************/

#ifndef _GUI_DRAW_H
#define _GUI_DRAW_H


/************************************
* 为定义GUIDRAW而需要引用的其他头文件
************************************/
#include "guibase.h"
#include "guifbmap.h"


//绘制箭头方向的枚举类型
enum arrow_dirction
{
    ARROW_UP = 0,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
};
/**********************************
* 定义GUI中与图像类型相关的操作函数
**********************************/
//根据指定的信息直接绘制像素
int DrawPixel(unsigned int uiPixelX, unsigned int uiPixelY);
//根据指定的信息直接绘制直线
int DrawLine(unsigned int uiStartX, unsigned int uiStartY, 
             unsigned int uiEndX, unsigned int uiEndY);
//根据指定的信息直接绘制矩形
int DrawRectangle(unsigned int uiStartX, unsigned int uiStartY, 
                  unsigned int uiEndX, unsigned int uiEndY);
//根据指定的信息直接绘制圆形
int DrawCircle(unsigned int uiPointX, unsigned int uiPointY, 
               unsigned int uiCircleR);
//根据指定的信息直接绘制块
int DrawBlock(unsigned int uiStartX, unsigned int uiStartY, 
              unsigned int uiEndX, unsigned int uiEndY);
//根据指定的信息直接绘制箭头
int DrawArrow(unsigned int uiStartX, unsigned int uiStartY,
              unsigned int uiEndX, unsigned int uiEndY,
              unsigned int uiWidth, unsigned int uiheight,
              unsigned int uiDirction);


/****************************
* 定义GUI中基本的图形绘制函数
****************************/
//绘制单个像素，应用于8位色帧缓冲
void _DrawPixel8(COLOR value, 
                 UINT32 pixel_x, UINT32 pixel_y, 
                 GUIFBMAP *pfbmap);
//绘制单个像素，应用于16位色帧缓冲
void _DrawPixel16(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap);
//绘制单个像素，应用于24位色帧缓冲
void _DrawPixel24(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap);
//绘制单个像素，应用于32位色帧缓冲
void _DrawPixel32(COLOR value, 
                  UINT32 pixel_x, UINT32 pixel_y, 
                  GUIFBMAP *pfbmap);

//绘制水平线，应用于8位色帧缓冲
void _DrawHor8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_x, 
               GUIFBMAP *pfbmap);
//绘制水平线，应用于16位色帧缓冲
void _DrawHor16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap);
//绘制水平线，应用于24位色帧缓冲
void _DrawHor24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap);
//绘制水平线，应用于32位色帧缓冲
void _DrawHor32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, 
                GUIFBMAP *pfbmap);

//绘制垂直线，应用于8位色帧缓冲
void _DrawVer8(COLOR value, 
               UINT32 start_x, UINT32 start_y, 
               UINT32 end_y, 
               GUIFBMAP *pfbmap);
//绘制垂直线，应用于16位色帧缓冲
void _DrawVer16(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap);
//绘制垂直线，应用于24位色帧缓冲
void _DrawVer24(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap);
//绘制垂直线，应用于32位色帧缓冲
void _DrawVer32(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_y, 
                GUIFBMAP *pfbmap);

//绘制斜线，应用于8位色帧缓冲
void _DrawBias8(COLOR value, 
                UINT32 start_x, UINT32 start_y, 
                UINT32 end_x, UINT32 end_y, 
                GUIFBMAP *pfbmap);
//绘制斜线，应用于16位色帧缓冲
void _DrawBias16(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);
//绘制斜线，应用于24位色帧缓冲
void _DrawBias24(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);
//绘制斜线，应用于32位色帧缓冲
void _DrawBias32(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);

//绘制空心圆，应用于8位色帧缓冲
void _DrawCircle8(COLOR value,
                  UINT32 point_x, UINT32 point_y, 
                  UINT32 radius, 
                  GUIFBMAP *pfbmap);
//绘制空心圆，应用于16位色帧缓冲
void _DrawCircle16(COLOR value, 
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius, 
                   GUIFBMAP *pfbmap);
//绘制空心圆，应用于24位色帧缓冲
void _DrawCircle24(COLOR value, 
                   UINT32 point_x, UINT32 point_y, 
                   UINT32 radius, 
                   GUIFBMAP *pfbmap);
//绘制空心圆，应用于32位色帧缓冲
void _DrawCircle32(COLOR value, 
                   UINT32 point_x, UINT32 point_y,
                   UINT32 radius, 
                   GUIFBMAP *pfbmap);

//绘制填充块，应用于8位色帧缓冲
void _DrawBlock8(COLOR value, 
                 UINT32 start_x, UINT32 start_y, 
                 UINT32 end_x, UINT32 end_y, 
                 GUIFBMAP *pfbmap);
//绘制填充块，应用于16位色帧缓冲
void _DrawBlock16(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap);
//绘制填充块，应用于24位色帧缓冲
void _DrawBlock24(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap);
//绘制填充块，应用于32位色帧缓冲
void _DrawBlock32(COLOR value, 
                  UINT32 start_x, UINT32 start_y, 
                  UINT32 end_x, UINT32 end_y, 
                  GUIFBMAP *pfbmap);


#endif   //_GUI_DRAW_H

