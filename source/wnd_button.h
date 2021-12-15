/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_button.h
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

#include "guiglobal.h"

typedef struct _button
{
    GUIVISIBLE  Visible;        //可视信息
    GUIPICTURE* Background1;    //背景1，按钮普通状态的背景
    GUIPICTURE* Background2;    //背景2，按钮按下时候的背景
    GUILABEL*   Text;           //按钮的文本

    int         Checkable;      //按钮是否Checkable的
    int         Checked;        //按钮的Checked状态
    int         Code;
    int         LeftPadding;    //文字边距
} Button;


Button* CreateButton(int x, int y, int w, int h);
void DestroyButton(Button* button);

Button* CreateCheckBox(int x, int y, int w, int h, int padding);

void Button_Construct(Button* _this, int x, int y, int w, int h);
void Button_Destruct(Button* _this);
void Button_Show(Button* _this);
void Button_SetBackground(Button* _this, const char* bmpFile);
void Button_SetPressedBackground(Button* _this, const char* bmpFile);
void Button_SetFont(Button* _this, GUIFONT* font);
void Button_SetAlign(Button* _this, int align);
void Button_SetText(Button* _this, GUICHAR* text);
void Button_SetCheckable(Button* _this, int checkable);
void Button_SetChecked(Button* _this, int checked);
void Button_SetEnabled(Button* _this, int enable);
void Button_SetLayer(Button* _this, int layer);
void Button_SetCode(Button* _this, int code);
void Button_SetLeftPadding(Button* _this, int padding);

void Button_Down(Button* _this);
void Button_Up(Button* _this);
