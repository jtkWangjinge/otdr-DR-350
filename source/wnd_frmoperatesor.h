/*******************************************************************************
* Copyright(c)2015，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmsavedefdir.h
* 摘    要：  声明窗体wnd_frmsavedefdir的窗体处理线程及相关操作函数
*			  该窗体提供了选择默认存储路径的功能。
*             
*
* 当前版本：  v1.0.1
* 作    者：  
* 完成日期：  2015.05.27
*
* 取代版本：  v1.0.0
* 原 作 者：  
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMSAVEDEFDIR_H
#define _WND_FRMSAVEDEFDIR_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"
#include "app_frmfilebrowse.h"


//创建sola文件操作dialog
//int CreateOperateSorWin(unsigned int enFileOperation, const char *pTopPath, CALLLBACKWINDOW func);


/***
  * 功能：
		创建用户自定义路径窗体
  * 参数：
  		1、FILEOPERATION enFileOperation:	文件操作类型
  		2、const char *pTopPath			:	允许用户返回的顶层目录
  		3、const char *pCurrPath		:	打开的第一个目录
  		4、CALLLBACKWINDOW func		    :	
  		5、FILETYPE fileType		    :	显示的文件类型
  * 返回：
		无
  * 备注：
***/
int FileDialog(unsigned int enFileOperation, const char *pTopPath, char *pCurrPath, CALLLBACKWINDOW func, FILETYPE fileType);

//加载语言配置文件时，是否需要检测序列号
void setLanguageConfigSerialNoFlag(int flag);
//设置初始操作路径
int setOperateFirstPath(const char *pPath);
#endif  
