/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_sorviewer.h
* 摘    要：  实现sorviewer的相关函数声明
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef __APP_SORVIEWER_H
#define __APP_SORVIEWER_H

#include "wnd_global.h"

//定义SorViewer接口
typedef int (*SorViewerHandler)(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where);

//设置当前读取sor的handler
void SetCurrSorViewer(SorViewerHandler handler);
SorViewerHandler GetCurrSorViewer();

//读取SOR
int SorViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where);

#endif
