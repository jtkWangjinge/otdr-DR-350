/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmfilebrowse.h
* 摘    要：  声明窗体wnd_frmfilebrowse(文件浏览)的窗体处理线程及相关操作函数。该窗体
*             提供了文件浏览功能。
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：  2020-9-3

*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMFILEBROWSE_H
#define _WND_FRMFILEBROWSE_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "app_frmfilebrowse.h"

/*******************************************************************************
**									宏定义 				 					  **
*******************************************************************************/
#define OPERATIONTYPENUM   		4					//文件操作类型的个数
#define FILETYPENUM    			3					//文件类型的个数
#define FILEBROWSE_LEFTBG_X 	0					//左边背景X坐标
#define FILEBROWSE_LEFTBG_Y 	0 					//左边背景Y坐标
#define FILEBROWSE_RIGHTBG_X 	681					//右边背景X坐标
#define FILEBROWSE_RIGHTBG_Y 	-11 					//右边背景Y坐标
#define FILEBROWSE_RIGHTLBL_Y 	-9 					//右边背景Y坐标


/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmfilebrowse的初始化函数，建立窗体控件、注册消息处理
int FrmFileBrowseInit(void *pWndObj);
//窗体frmfilebrowse的退出函数，释放所有资源
int FrmFileBrowseExit(void *pWndObj);
//窗体frmfilebrowse的绘制函数，绘制整个窗体
int FrmFileBrowsePaint(void *pWndObj);
//窗体frmfilebrowse的循环函数，进行窗体循环
int FrmFileBrowseLoop(void *pWndObj);
//窗体frmfilebrowse的挂起函数，进行窗体挂起前预处理
int FrmFileBrowsePause(void *pWndObj);
//窗体frmfilebrowse的恢复函数，进行窗体恢复前预处理
int FrmFileBrowseResume(void *pWndObj);

//刷新菜单栏
void PaintMenu(int mode);

/***
  * 功能：
        设置当前文件浏览器的操作类型
  * 参数：
        1、FILEOPERATION enFileOperation:	将要设置的文件操作类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：	
***/
int __SetFileOperationType(unsigned int enFileOperation);


/***
  * 功能：
        设置当前的文件过滤类型
  * 参数：
        1.FILETYPE enFilter:	将要设置的文件过滤类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int __SetCurFileType(FILETYPE enFilter);


/***
  * 功能：
		设置文件浏览器打开的目录
  * 参数：
		1.const char *pPathFiles:	将要打开的目录
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int __SetOpenFolder(const char *pPathFiles);


int FileManager(GUIWINDOW* mainWindow);

#endif 
