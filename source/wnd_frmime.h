/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_frmime.h
* 摘    要：  声明窗体wnd_frmime(输入法)的窗体处理线程及相关操作函数。该窗体
*             提供了输入法功能。
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014-03-21
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMIME_H
#define _WND_FRMIME_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"

/*******************************************************************************
**								变量声明					  				  **
*******************************************************************************/

//定义一个输入法退出时还原窗体的回调函数
typedef void (*IMECALLLBACK)(GUIWINDOW **);

/* 输入法控件的回调函数 */
typedef void (*IMECALLLBACKHANDLE)(void);


/*********************
* 声明窗体处理相关函数
*********************/
//窗体frmime的初始化函数，建立窗体控件、注册消息处理
int FrmIMEInit(void *pWndObj);
//窗体frmime的退出函数，释放所有资源
int FrmIMEExit(void *pWndObj);
//窗体frmime的绘制函数，绘制整个窗体
int FrmIMEPaint(void *pWndObj);
//窗体frmime的循环函数，进行窗体循环
int FrmIMELoop(void *pWndObj);
//窗体frmime的挂起函数，进行窗体挂起前预处理
int FrmIMEPause(void *pWndObj);
//窗体frmime的恢复函数，进行窗体恢复前预处理
int FrmIMEResume(void *pWndObj);

void IMEInit(char *pInitInStr, int maxstr, char keytype, 
			 IMECALLLBACK func, IMECALLLBACKHANDLE okfunc, IMECALLLBACKHANDLE cancelfunc);

/***
  * 功能：
        创建软键盘中的对话框
  * 参数：
        1.GUICHAR *pTextStr:	需要显示的警告信息
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		当软键盘输入了一个不合法的值后，需要弹出警告对话框，
  		此时涉及到了多重窗体，返回时容易出问题，不能使用通用的对话框，
  		为此专门为软键盘建立了一个对话框供调用。
***/
int CreateIMEDialog(GUICHAR *pTextStr);


/***
  * 功能：
		获取软件盘输入
  * 参数：
		无
  * 返回：
		0、成功
	  非0、失败	
  * 备注：
  		自己保证pBuff缓冲区足够大
***/
int GetIMEInputBuff(char *pBuff);


/***
  * 功能：
		获取软件盘原始输入
  * 参数：
		无
  * 返回：
		0、成功
	  非0、失败	
  * 备注：
  		自己保证pBuff缓冲区足够大
***/

int GetIMEInputBuffWithoutSpaceTrim(char *pBuff);

#endif  