/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guipen.h
* 摘    要：  定义GUI的画笔类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020-10-29
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _GUI_PEN_H
#define _GUI_PEN_H


/***********************************
* 为定义GUIPEN而需要引用的其他头文件
***********************************/
#include "guibase.h"


/******************
* 定义GUI中画笔类型
******************/
#define GUIPEN_TYP_SOLID        1   //实线画笔
#define GUIPEN_TYP_DASHED       2   //虚线画笔


/******************
* 定义GUI中画笔结构
******************/
typedef struct _pen
{
    int iPenType;               //画笔类型，目前只支持实线画笔
    int iPenWidth;              //画笔宽度，目前只支持宽度为1

    unsigned int uiPenColor;    //画笔颜色，RGB888格式
} GUIPEN;


/**********************************
* 定义GUI中与画笔类型相关的操作函数
**********************************/
//根据指定的信息直接建立画笔对象
GUIPEN* CreatePen(int iPenType, int iPenWidth, unsigned int uiPenColor);
//删除画笔对象
int DestroyPen(GUIPEN **ppPenObj);

//得到当前的画笔对象
GUIPEN* GetCurrPen(void);
//设置当前的画笔对象
int SetCurrPen(GUIPEN *pPenObj);


#endif  //_GUI_PEN_H

