/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsavefile.h
* 摘    要：  声明主窗体frmsavefile的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/26
*
*******************************************************************************/

#ifndef _WND_FRM_SAVE_FILE_H_
#define _WND_FRM_SAVE_FILE_H_

#include "wnd_global.h"

enum File_Option_Type
{
	OPEN_FILE = 0,
	SAVE_FILE = 1,
	FILE_OPTION_NUM
};

extern char* pStrSaveFileButton_unpress[FILE_OPTION_NUM];
extern char* pStrSaveFileButton_press[FILE_OPTION_NUM];


/***
  * 功能：
		创建外部调用保存文件的窗体
  * 参数：
		CALLLBACKWINDOW func:回调函数
  * 返回：
		无
  * 备注：
***/
void CreateSaveFileDialog(CALLLBACKWINDOW func);

//创建打开文件窗体（应用于菜单栏调用）
void CreateOpenFileDialog(void);
//设置按键焦点位置（option：0，up，1：down）
void SaveFileSetKeyFocusPos(int option);
//设置左侧文件列表背景选中框(iEnable:0,未选中，1，选中)
void SaveFileSetLeftBackground(int iEnable);
//刷新左侧列表选中项
void RefreshSelectedFile(void);
#endif