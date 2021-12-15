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
#ifndef __APP_FRMSORVIEWER_H
#define __APP_FRMSORVIEWER_H

#include "app_sorviewer.h"
#include "wnd_frmotdr.h"


//当前的SorViewerHandler 默认为OtdrSorViewer
static SorViewerHandler CurrSorViewerHandler = OtdrSorViewer;


//设置当前读取sor的handler
void SetCurrSorViewer(SorViewerHandler handler)
{
    CurrSorViewerHandler = handler;
}

SorViewerHandler GetCurrSorViewer()
{
    return CurrSorViewerHandler;
}

//读取SOR
int SorViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where)
{
    if (CurrSorViewerHandler)
    {
        return CurrSorViewerHandler(fileName, from, where);
    }
    else 
    {
        return -1;
    }
}

#endif

