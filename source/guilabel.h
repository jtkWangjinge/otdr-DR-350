/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guilabel.h
* 摘    要：  定义GUI的标签类型及操作，为实现标签控件提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _GUI_LABEL_H
#define _GUI_LABEL_H


/*************************************
* 为定义GUILABEL而需要引用的其他头文件
*************************************/
#include "guibase.h"
#include "guibrush.h"
#include "guipen.h"
#include "guifont.h"
#include "guitext.h"


/****************************************
* 定义GUI中标签对象所能支持的文本对齐方式
****************************************/
#define GUILABEL_ALIGN_LEFT         0   //标签文本靠左对齐
#define GUILABEL_ALIGN_RIGHT        1   //标签文本靠右对齐
#define GUILABEL_ALIGN_CENTER       2   //标签文本居中对齐


/******************
* 定义GUI中标签结构
******************/
typedef struct _label
{
    GUIVISIBLE Visible;         //可视信息，任何窗体控件都必须将其作为第一个成员

    GUIPEN *pLblPen;            //标签使用的画笔，为NULL则使用GUI中的当前画笔
    GUIBRUSH *pLblBrush;        //标签使用的画刷，为NULL则使用GUI中的当前画刷
    GUIFONT *pLblFont;          //标签使用的字体，为NULL则使用GUI中的当前字体

    int iLblAlign;              //标签文本对齐方式，0靠左，1靠右，2居中，默认0
    GUITEXT *pLblText;          //标签所对应的文本资源
} GUILABEL;


/**********************************
* 定义GUI中与标签类型相关的操作函数
**********************************/
//根据指定的信息直接建立标签对象
GUILABEL* CreateLabel(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                      unsigned int uiLblWidth, unsigned int uiLblHeight, 
                      unsigned short *pTextData);
//删除标签对象
int DestroyLabel(GUILABEL **ppLblObj);

//直接输出指定的标签
int DisplayLabel(GUILABEL *pLblObj);

//设置标签的有效区域
int SetLabelArea(unsigned int uiStartX, unsigned int uiStartY, 
                 unsigned int uiEndX, unsigned int uiEndY, 
                 GUILABEL *pLblObj);
//设置标签的可见性
int SetLabelEnable(int iEnable, GUILABEL *pLblObj);
//设置标签所使用的画刷
int SetLabelBrush(GUIBRUSH *pLblBrush, GUILABEL *pLblObj);
//设置标签所使用的画笔
int SetLabelPen(GUIPEN *pLblPen, GUILABEL *pLblObj);
//设置标签所对应的字体
int SetLabelFont(GUIFONT *pLblFont, GUILABEL *pLblObj);
//设置标签文本的对齐方式
int SetLabelAlign(int iLblAlign, GUILABEL *pLblObj);
//设置标签所对应的文本资源
int SetLabelText(unsigned short *pTextData, GUILABEL *pLblObj);


#endif  //_GUI_LABEL_H

