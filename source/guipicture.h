/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guipicture.h
* 摘    要：  定义GUI的图形框类型及操作，为实现图形框控件提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：
*******************************************************************************/

#ifndef _GUI_PICTURE_H
#define _GUI_PICTURE_H


/***************************************
* 为定义GUIPICTURE而需要引用的其他头文件
***************************************/
#include "guibase.h"
#include "guibrush.h"
#include "guipen.h"
#include "guifont.h"
#include "guibitmap.h"


/********************
* 定义GUI中图形框结构
********************/
typedef struct _picture
{
    GUIVISIBLE Visible;         //可视信息，任何窗体控件都必须将其作为第一个成员

    GUIPEN *pPicPen;            //图形框使用的画笔，为NULL则使用GUI中的当前画笔
    GUIBRUSH *pPicBrush;        //图形框使用的画刷，为NULL则使用GUI中的当前画刷
    GUIFONT *pPicFont;          //图形框使用的字体，为NULL则使用GUI中的当前字体

    GUIBITMAP *pPicBitmap;      //图形框所对应的位图资源
} GUIPICTURE;


/************************************
* 定义GUI中与图形框类型相关的操作函数
************************************/
//根据指定的信息直接建立图形框对象
GUIPICTURE* CreatePicture(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                          unsigned int uiPicWidth, unsigned int uiPicHeight, 
                          char *strBitmapFile);
//删除图形框对象
int DestroyPicture(GUIPICTURE **ppPicObj);

//直接输出指定的图形框
int DisplayPicture(GUIPICTURE *pPicObj);

//设置图形框的有效作用区域
int SetPictureArea(unsigned int uiStartX, unsigned int uiStartY, 
                   unsigned int uiEndX, unsigned int uiEndY,
                   GUIPICTURE *pPicObj);
//设置图形框的可见性
int SetPictureEnable(int iEnable, GUIPICTURE *pPicObj);
//获取图形框的可见性
int GetPictureEnable(GUIPICTURE *pPicObj);
//设置图形框所使用的画刷
int SetPictureBrush(GUIBRUSH *pPicBrush, GUIPICTURE *pPicObj);
//设置图形框所使用的画笔
int SetPicturePen(GUIPEN *pPicPen, GUIPICTURE *pPicObj);
//设置图形框所对应的字体
int SetPictureFont(GUIFONT *pPicFont, GUIPICTURE *pPicObj);
//设置图形框所对应的位图资源
int SetPictureBitmap(char *strBitmapFile, GUIPICTURE *pPicObj);


#endif  //_GUI_PICTURE_H

