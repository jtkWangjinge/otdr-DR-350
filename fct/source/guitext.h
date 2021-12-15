/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guitext.h
* 摘    要：  定义GUI的文本类型及操作，并声明GUI基本的文本输出操作函数，为GUI图
*             形控件的实现提供基础。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _GUI_TEXT_H
#define _GUI_TEXT_H


/************************************
* 为定义GUITEXT而需要引用的其他头文件
************************************/
#include "guibase.h"
#include "guifont.h"


/******************
* 定义GUI中文本结构
******************/
typedef struct _text
{
    unsigned int uiTextLength;      //文本长度，即文本中字符的个数，不含'\0'
    unsigned short *pTextData;      //文本内容，以'\0'结尾

    unsigned int uiViewWidth;       //显示宽度，>=0，初始化为0则按文本实际宽度
    unsigned int uiViewHeight;      //显示高度，>=0，初始化为0则按文本实际高度

    unsigned int uiPlaceX;          //文本水平放置位置，以左上角为基点
    unsigned int uiPlaceY;          //文本垂直放置位置，以左上角为基点

    GUIFONT_MATRIX **ppTextMatrix;  //文本所对应的点阵资源入口
} GUITEXT;


/**********************************
* 定义GUI中与文本类型相关的操作函数
**********************************/
//根据指定的信息直接建立文本对象
GUITEXT* CreateText(unsigned short *pTextData, 
                    unsigned int uiPlaceX, unsigned int uiPlaceY);
//删除文本对象
int DestroyText(GUITEXT **ppTextObj);

//直接输出指定的文本
int DisplayText(GUITEXT *pTextObj);

//设置文本的内容
int SetTextData(unsigned short *pTextData, GUITEXT *pTextObj);
//设置文本的显示大小
int SetTextView(unsigned int uiViewWidth, unsigned int uiViewHeight, 
                GUITEXT *pTextObj);
//设置文本的放置位置
int SetTextPlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                  GUITEXT *pTextObj);

//装载文本点阵资源
int LoadTextMatrix(GUITEXT *pTextObj);
//卸载文本点阵资源
int UnloadTextMatrix(GUITEXT *pTextObj);


#endif  //_GUI_TEXT_H

