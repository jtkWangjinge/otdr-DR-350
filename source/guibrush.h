/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibrush.h
* 摘    要：  定义GUI的画刷类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_BRUSH_H
#define _GUI_BRUSH_H


/*************************************
* 为定义GUIBRUSH而需要引用的其他头文件
*************************************/
#include "guibase.h"


/******************
* 定义GUI中画刷类型
******************/
#define GUIBRUSH_TYP_SOLID      1   //纯色画刷
#define GUIBRUSH_TYP_IMAGE      2   //图像画刷


/******************
* 定义GUI中画刷结构
******************/
typedef struct _brush
{
    int iBrushType;         //画刷类型，目前只支持纯色画刷

    unsigned int uiFgColor; //画刷前景颜色，RGB888格式
    unsigned int uiBgColor; //画刷背景颜色，RGB888格式
} GUIBRUSH;


/**********************************
* 定义GUI中与画刷类型相关的操作函数
**********************************/
//根据指定的信息直接建立画刷对象
GUIBRUSH* CreateBrush(int iBrushType);
//删除画刷对象
int DestroyBrush(GUIBRUSH **ppBrushObj);

//得到当前的画刷对象
GUIBRUSH* GetCurrBrush(void);
//设置当前的画刷对象
int SetCurrBrush(GUIBRUSH *pBrushObj);

//设置画刷的前背景颜色
int SetBrushColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                  GUIBRUSH *pBrushObj);


#endif  //_GUI_BRUSH_H

