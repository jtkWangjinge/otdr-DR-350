/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_messagebox.h
* 摘    要：  提供meessagebox相关的功能
*
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：
*
*******************************************************************************/

#ifndef _WND_MESSAGEBOX_H_
#define _WND_MESSAGEBOX_H_

#include "guiglobal.h"

//标准按钮
typedef enum _message_box_buttons
{
    MessageBox_NoButton     = 0x00000000,
    MessageBox_Yes          = 0x00000001,
    MessageBox_No           = 0x00000002,
} MessageBoxButton;


/***
  * 功能：
        弹出一个标准的MessageBox
  * 参数：
        1.title  :标题
        2.text   :文本
  * 返回：
        执行完成返回用户点击的按钮
  * 备注：
***/
unsigned int ShowStandardMessageBox(GUICHAR* title, GUICHAR* text);


/***
  * 功能：
        弹出一个自动关闭的的MessageBox
  * 参数：
        1.title   : 标题
        2.text    : 文本
        4.holdTime: 保持时间(ms)
  * 返回：
        执行完成返回用户点击的按钮
        自动关闭时不返回任何按钮
  * 备注：
***/
unsigned int ShowAutoCloseMessageBox(GUICHAR* title, GUICHAR* text, int holdTime);

#endif //_WND_MESSAGEBOX_H_