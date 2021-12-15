/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmprogressbar.c
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

#include "wnd_frmprogressbar.h"
#include "wnd_global.h"


PROGRESS_BAR* ProgressBar_Create(UINT32 x, UINT32 y, UINT32 w, UINT32 h)
{
    if (w < 50 || h < 16)
        return NULL;

    //分配内存
    PROGRESS_BAR* me = (PROGRESS_BAR*)malloc(sizeof(PROGRESS_BAR));

    //可视信息初始化
    me->Visible.Area.Start.x = x;
    me->Visible.Area.Start.y = y;
    me->Visible.Area.End.x = x + w - 1;
    me->Visible.Area.End.y = y + h - 1;
    me->Visible.iEnable = 1;
    me->Visible.iFocus = 0;
    me->Visible.iLayer = 0;
    
    me->iBackgroundColor = COLOR_TRANS;
    me->iColor = 0x000000;
    me->iDisplayText = 1;
    
    me->iMinimum = 0;
    me->iMaximum = 100;
    me->iValue = 0;

	if (isFont24())
	{
		me->pFont = CreateFont(FNTFILE_BIG, 24, 24, me->iColor, COLOR_TRANS);
	}
	else
	{
		me->pFont = CreateFont(FNTFILE_STD, 16, 16, me->iColor, COLOR_TRANS);
	}
    me->pText = TransString("0%");
    x = me->Visible.Area.Start.x + w - 30;
    y = me->Visible.Area.Start.y + (h - 16) / 2;
    LOG(LOG_INFO, "x=%d, y=%d\n", x, y);
    me->pTextLabel = CreateLabel(x, y, 30, 24, me->pText);
    SetLabelFont(me->pFont, me->pTextLabel);
    SetLabelAlign(GUILABEL_ALIGN_RIGHT, me->pTextLabel);
    
    return me;
}


int ProgressBar_Destroy(PROGRESS_BAR* me)
{
    if (me)
    {
        DestroyLabel(&me->pTextLabel);
        free(me->pText);
        free(me);
    }
    return 0;
}


int ProgressBar_Display(PROGRESS_BAR* me)
{
    if (me->Visible.iEnable == 0)
        return -1;

    char buffer[12];    
    GUIPEN *pen = GetCurrPen();
    int OldColor = pen->uiPenColor;

    //绘制背景
    if (me->iBackgroundColor != COLOR_TRANS)
    {
        pen->uiPenColor = me->iBackgroundColor;
        DrawBlock(
            me->Visible.Area.Start.x, me->Visible.Area.Start.y, 
            me->Visible.Area.End.x, me->Visible.Area.End.y
            );
    }
    
    //绘制边框
    GUIRECTANGLE rect = me->Visible.Area;
    if (me->iDisplayText)
    {
        rect.End.x -= 30;
    }
    pen->uiPenColor = me->iColor;
    DrawRectangle(rect.Start.x, rect.Start.y, rect.End.x, rect.End.y);

    //绘制进度条
    rect.Start.x += 2;
    rect.Start.y += 2;
    rect.End.x -= 2;
    rect.End.y -= 2;
    int percent = (me->iValue - me->iMinimum) * 100 / (me->iMaximum - me->iMinimum);
    int offset = rect.End.x - rect.Start.x;
    offset = offset - (offset * percent / 100);
    rect.End.x -= offset;
    DrawBlock(rect.Start.x, rect.Start.y, rect.End.x, rect.End.y);

    //显示文字
    if (me->iDisplayText)
    {
        sprintf(buffer, "%d%%", percent);
        free(me->pText);
        me->pText = TransString(buffer);
        SetLabelText(me->pText, me->pTextLabel);
        DisplayLabel(me->pTextLabel);
    }

    pen->uiPenColor = OldColor;
    
    return 0;
}

int ProgressBar_SetRange(PROGRESS_BAR* me, int min, int max)
{
    if (min >= max)
    {
        return -1;
    }

    me->iMinimum = min;
    me->iMaximum = max;
    me->iValue = 0;

    return 0;
}


int ProgressBar_SetValue(PROGRESS_BAR* me, int value)
{
    if ((me->iMinimum > value)
       || (me->iMaximum < value))
       return -1;

    me->iValue = value;
    
    return 0;
}


void ProgressBar_SetColor(PROGRESS_BAR* me, COLOR iColor)
{
    me->iColor = iColor;
    DestroyFont(&me->pFont);
	if (isFont24())
	{
		me->pFont = CreateFont(FNTFILE_BIG, 24, 24, me->iColor, COLOR_TRANS);
	}
	else
	{
		me->pFont = CreateFont(FNTFILE_STD, 16, 16, me->iColor, COLOR_TRANS);
	}
    SetLabelFont(me->pFont, me->pTextLabel);
}


void ProgressBar_SetBackgroundColor(PROGRESS_BAR* me, COLOR iColor)
{
    me->iBackgroundColor = iColor;
}

void ProgressBar_SetEnable(PROGRESS_BAR* me, int iEnable)
{
    me->Visible.iEnable = iEnable;
}

void ProgressBar_SetTextEnable(PROGRESS_BAR* me, int iEnable)
{
    me->iDisplayText = iEnable;
}

