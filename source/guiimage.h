/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiimage.h
* 摘    要：  定义GUI图像类型及声明相关操作，并声明GUI基本的图像绘制操作函数，为
*             GUI图形控件的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_IMAGE_H
#define _GUI_IMAGE_H


/*************************************
* 为定义GUIIMAGE而需要引用的其他头文件
*************************************/
#include "guibase.h"
#include "guifbmap.h"
#include "guipalette.h"


/******************
* 定义GUI中图像结构
******************/
typedef struct _image
{
    unsigned int uiImgWidth;    //图像水平宽度，单位为像素
    unsigned int uiImgHeight;   //图像垂直高度，单位为像素
    unsigned int uiImgDepth;    //图像颜色深度，1位、8位、24位
    unsigned char *pImgData;    //图像数据

    unsigned int uiDispWidth;   //可显示的宽度，须符合帧缓冲映射的限制
    unsigned int uiDispHeight;  //可显示的高度，须符合帧缓冲映射的限制
    unsigned int uiPlaceX;      //图像水平放置位置，以左上角为基点，相对坐标
    unsigned int uiPlaceY;      //图像垂直放置位置，以左上角为基点，相对坐标
    unsigned int uiRealX;       //以左上角为基点，加水平偏移后的绝对坐标
    unsigned int uiRealY;       //以左上角为基点，加垂直偏移后的绝对坐标

    unsigned int uiFgColor;     //图像的前景颜色，uiImgDepth为1时使用
    unsigned int uiBgColor;     //图像的背景颜色，uiImgDepth为1时使用
    GUIPALETTE *pRefPal;        //图像引用的调色板，当uiImgDepth为8时使用
} GUIIMAGE;


/**********************************
* 定义GUI中与图像类型相关的操作函数
**********************************/
//根据指定的信息直接建立图像对象
GUIIMAGE* CreateImage(unsigned int uiImgWidth, 
                      unsigned int uiImgHeight, 
                      unsigned int uiImgDepth);
//删除图像对象
int DestroyImage(GUIIMAGE **ppImgObj);

//直接输出指定的图像
int DisplayImage(GUIIMAGE *pImgObj);

//设置图像的放置位置
int SetImagePlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                  GUIIMAGE *pImgObj);
//设置图像的前背景颜色
int SetImageColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                  GUIIMAGE *pImgObj);
//设置图像引用的调色板
int SetImagePalette(GUIPALETTE *pRefPalette, GUIIMAGE *pImgObj);


/****************************
* 定义GUI中基本的图像绘制函数
****************************/
//绘制1位色图像，应用于8位色帧缓冲
void _Image1Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制1位色图像，应用于16位色帧缓冲
void _Image1Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制1位色图像，应用于24位色帧缓冲
void _Image1Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制1位色图像，应用于32位色帧缓冲
void _Image1Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap);

//绘制8位色图像，应用于8位色帧缓冲
void _Image8Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制8位色图像，应用于16位色帧缓冲
void _Image8Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制8位色图像，应用于24位色帧缓冲
void _Image8Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制8位色图像，应用于32位色帧缓冲
void _Image8Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap);

//绘制24位色图像，应用于8位色帧缓冲
void _Image24Fb8(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制24位色图像，应用于16位色帧缓冲
void _Image24Fb16(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制24位色图像，应用于24位色帧缓冲
void _Image24Fb24(GUIIMAGE *pimg, GUIFBMAP *pfbmap);
//绘制24位色图像，应用于32位色帧缓冲
void _Image24Fb32(GUIIMAGE *pimg, GUIFBMAP *pfbmap);


#endif  //_GUI_IMAGE_H

