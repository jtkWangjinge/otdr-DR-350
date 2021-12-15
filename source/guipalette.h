/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guipalette.h
* 摘    要：  定义GUI的调色板类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：
*******************************************************************************/

#ifndef _GUI_PALETTE_H
#define _GUI_PALETTE_H


/***************************************
* 为定义GUIPALETTE而需要引用的其他头文件
***************************************/
#include "guibase.h"


/********************
* 定义GUI中调色板结构
********************/
typedef struct _palette
{
    int iPalLength;             //调色板长度，即有多少个颜色，取值范围为1~256
    unsigned char *pTabEntry;   //调色板颜色表的入口地址

    unsigned char *pRedValue;   //指针，用于访问颜色表中各颜色的红色分量
    unsigned char *pGreenValue; //指针，用于访问颜色表中各颜色的绿色分量
    unsigned char *pBlueValue;  //指针，用于访问颜色表中各颜色的蓝色分量
} GUIPALETTE;


/************************************
* 定义GUI中与调色板类型相关的操作函数
************************************/
//根据指定的信息直接建立调色板对象
GUIPALETTE* CreatePalette(int iPalLength);
//删除调色板对象
int DestroyPalette(GUIPALETTE **ppPalObj);

//得到当前的调色板对象
GUIPALETTE* GetCurrPalette(void);
//设置当前的调色板对象
int SetCurrPalette(GUIPALETTE *pPalObj);

//在指定调色板对象中匹配与特定颜色最接近的调色板颜色
int MatchPaletteColor(unsigned int uiRgbColor, GUIPALETTE *pPalObj);


#endif  //_GUI_PALETTE_H

