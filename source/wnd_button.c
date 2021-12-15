/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_button.c
* 摘    要：  Button实现
*             
*
* 当前版本：  v1.0.0(初始版本)
* 作    者：  
* 完成日期：  
*
* 取代版本： 
* 原 作 者：  
* 完成日期：
*******************************************************************************/

#include "wnd_button.h"


Button* CreateButton(int x, int y, int w, int h)
{
    Button* button = (Button*)malloc(sizeof(Button));
    Button_Construct(button, x, y, w, h);
    return button;
}

void DestroyButton(Button* button)
{
    Button_Destruct(button);
    free(button);
}

Button* CreateCheckBox(int x, int y, int w, int h, int padding)
{
    Button* button = (Button*)malloc(sizeof(Button));
    Button_Construct(button, x, y, w, h);
    Button_SetLeftPadding(button, padding);
    return button;
}

void Button_Construct(Button* _this, int x, int y, int w, int h)
{
    _this->Visible.Area.Start.x = x;
    _this->Visible.Area.Start.y = y;
    _this->Visible.Area.End.x = x + w - 1;
    _this->Visible.Area.End.y = y + h - 1;
    _this->Visible.iFocus = 0;
    _this->Visible.iEnable = 1;
    _this->Visible.iCursor = 1;
    _this->Visible.iLayer = 0;

    //文本在按钮中上下居中，此处计算文本的起始y坐标
    int ty = y + (h - 16) / 2;    
    _this->Background1 = CreatePicture(x, y, w, h, NULL);
    _this->Background2 = CreatePicture(x, y, w, h, NULL);
    _this->Text = CreateLabel(x + 2, ty, w, 24, NULL);
    _this->Checkable = 0;

    _this->Checked = 0;
    _this->LeftPadding = 0;
}

void Button_Destruct(Button* _this)
{
    DestroyPicture(&_this->Background1);
    DestroyPicture(&_this->Background2);
    DestroyLabel(&_this->Text);
}

void Button_Show(Button* _this)
{
    if (_this->Checkable && _this->Checked)
        DisplayPicture(_this->Background2);
    else
        DisplayPicture(_this->Background1);
    DisplayLabel(_this->Text);
}

void Button_SetBackground(Button* _this, const char* bmpFile)
{
    SetPictureBitmap((char*)bmpFile, _this->Background1);
}

void Button_SetPressedBackground(Button* _this, const char* bmpFile)
{
    SetPictureBitmap((char*)bmpFile, _this->Background2);
}

void Button_SetFont(Button* _this, GUIFONT* font)
{
    SetLabelFont(font, _this->Text);
}

void Button_SetAlign(Button* _this, int align)
{
    SetLabelAlign(align, _this->Text);
}

void Button_SetText(Button* _this, GUICHAR* text)
{
    SetLabelText(text, _this->Text);
    free(text);
}

void Button_SetCheckable(Button* _this, int checkable)
{
    _this->Checkable = checkable;
}

void Button_SetChecked(Button* _this, int checked)
{
    if (!_this->Checkable)
        return;
    _this->Checked = checked;
}

void Button_SetEnabled(Button* _this, int enable)
{
    _this->Visible.iEnable = enable;
}

void Button_SetLayer(Button* _this, int layer)
{
    _this->Visible.iLayer = layer;
}

void Button_SetCode(Button* _this, int code)
{
    _this->Code = code;
}

void Button_SetLeftPadding(Button* _this, int padding)
{
    int w = _this->Text->Visible.Area.End.x - _this->Text->Visible.Area.Start.x + 1;
    if (padding >= w)
        return;
    _this->LeftPadding = padding;
    
    int lx =  _this->Visible.Area.Start.x + padding;
    SetLabelArea(
        lx, _this->Text->Visible.Area.Start.y, 
        _this->Text->Visible.Area.End.x, _this->Text->Visible.Area.End.y, _this->Text
        );
}

void Button_Down(Button* _this)
{
    DisplayPicture(_this->Background2);
    DisplayLabel(_this->Text);
    if (_this->Checkable)
    {
        _this->Checked = _this->Checked ? 0 : 1;
    }
}

void Button_Up(Button* _this)
{
    if (_this->Checkable && _this->Checked)
        return;
    DisplayPicture(_this->Background1);
    DisplayLabel(_this->Text);
}
