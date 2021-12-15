/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmprogressbar.h 
* 摘    要：  实现进度条
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  2016-12-04
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _WND_FRMPROGRESSBAR_H
#define _WND_FRMPROGRESSBAR_H

#include "guiglobal.h"


typedef struct progress_bar
{
    GUIVISIBLE Visible;         //可视信息，任何窗体控件都必须将其作为第一个成员
    
    GUICHAR* pText;
    GUILABEL* pTextLabel;
    GUIFONT* pFont;
    
    int iMaximum;
    int iMinimum;
    int iValue;
    int iDisplayText;

    COLOR iBackgroundColor;
    COLOR iColor;
} PROGRESS_BAR;


PROGRESS_BAR* ProgressBar_Create(UINT32 x, UINT32 y, UINT32 w, UINT32 h);

int ProgressBar_Destroy(PROGRESS_BAR* me);

int ProgressBar_Display(PROGRESS_BAR* me);

int ProgressBar_SetRange(PROGRESS_BAR* me, int min, int max);

int ProgressBar_SetValue(PROGRESS_BAR* me, int value);

void ProgressBar_SetColor(PROGRESS_BAR* me, COLOR iColor);

void ProgressBar_SetBackgroundColor(PROGRESS_BAR* me, COLOR iColor);

void ProgressBar_SetEnable(PROGRESS_BAR* me, int iEnable);

void ProgressBar_SetTextEnable(PROGRESS_BAR* me, int iEnable);

#endif
