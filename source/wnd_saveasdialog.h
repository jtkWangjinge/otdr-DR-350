/*******************************************************************************
* Copyright(c)2015，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_saveasdialog.h
* 摘    要：  SaveAsDialog实现，实现一个存储窗口
*             
*
* 当前版本：  v1.0.0(初始版本)
* 作    者：  
* 完成日期：  2017.11.24
*
* 取代版本： 
* 原 作 者：  
* 完成日期：
*******************************************************************************/

#ifndef _WND_SAVEASDIALOG_H_
#define _WND_SAVEASDIALOG_H_


//返回值按键码定义
#define SAVE_AS_DIALOG_KEY_NONE         0        //无按键
#define SAVE_AS_DIALOG_KEY_ENTER        1        //enter 按键
#define SAVE_AS_DIALOG_KEY_ESC          2        //escape 按键


//普通按钮的背景图片
#define NORMAL_KEY_BMP          BmpFileDirectory"key_dialog_unpress.bmp"
#define NORMAL_KEY_PRESSED_BMP  BmpFileDirectory"key_dialog_press.bmp"
//大小写按钮的背景图片
#define SHIFT_KEY_BMP			BmpFileDirectory"shift_key_unpress.bmp"
#define SHIFT_KEY_PRESSED_BMP	BmpFileDirectory"shift_key_press.bmp"
#define SHIFTBIG_KEY__BMP		BmpFileDirectory"shift_key_Big_unpress.bmp"
#define SHIFTBIG_PRESSED_BMP	BmpFileDirectory"shift_key_Big_press.bmp"
//特殊按钮(Shift Esc)的背景图片
#define SPECIAL_KEY_BMP         BmpFileDirectory"special_key_unpress.bmp"
#define SPECIAL_KEY_PRESSED_BMP BmpFileDirectory"special_key_press.bmp"
//特殊按钮(Enter)的背景图片
#define ENTER_KEY_BMP           BmpFileDirectory"enter_key_unpress.bmp"
#define ENTER_KEY_PRESSED_BMP   BmpFileDirectory"enter_key_press.bmp"
//空格按钮的背景图片
#define SPACE_KEY_BMP           BmpFileDirectory"key_space_unpress.bmp"
#define SPACE_KEY_PRESSED_BMP   BmpFileDirectory"key_space_press.bmp"
//退格按钮的背景图片
#define BACK_KEY_BMP            BmpFileDirectory"back_unpress.bmp"
#define BACK_KEY_PRESSED_BMP    BmpFileDirectory"back_press.bmp"
//上 按钮的背景图片
#define UP_KEY_BMP              BmpFileDirectory"key_up_unpress.bmp"
#define UP_KEY_PRESSED_BMP      BmpFileDirectory"key_up_press.bmp"
//下 按钮的背景图片
#define DOWN_KEY_BMP            BmpFileDirectory"key_down_unpress.bmp"
#define DOWN_KEY_PRESSED_BMP    BmpFileDirectory"key_down_press.bmp"
//左 按钮的背景图片
#define LEFT_KEY_BMP            BmpFileDirectory"key_left_unpress.bmp"
#define LEFT_KEY_PRESSED_BMP    BmpFileDirectory"key_left_press.bmp"
//右 按钮的背景图片
#define RIGHT_KEY_BMP           BmpFileDirectory"key_right_unpress.bmp"
#define RIGHT_KEY_PRESSED_BMP   BmpFileDirectory"key_right_press.bmp"

//中英文切换英语图片
#define EN_KEY_BMP              BmpFileDirectory"key_en_unpress.bmp"
#define EN_KEY_PRESS_BMP        BmpFileDirectory"key_en_press.bmp"

//中英文切换中文图片
#define CN_KEY_BMP              BmpFileDirectory"key_cn_unpress.bmp"
#define CN_KEY_PRESS_BMP        BmpFileDirectory"key_cn_press.bmp"



/***
  * 功能：
        弹出带有键盘的保存文件的dialog
  * 参数：
        1.originalName  : 原始文件名(全路径包含后缀)(传入参数)
        2.newName       : 新的文件名(全路径包含后缀)(传出参数)
        3.fileType[]    : 文件类型列表(传入参数)
        4.num           : 文件类型列表个数(传入参数)
        5.defaultOption : 默认文件类型(传入参数)
        6.selectUsb     : 是否保存到usb(传出参数)
  * 返回：
        执行完成返回用户点击的按钮,输出最新的文件名
  * 备注：
***/
int ShowSaveAsDialog(char* originalName, char* newName, char* fileType[], int num, unsigned int defaultOption, int* selectUsb);


#endif
