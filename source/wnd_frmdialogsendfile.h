/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmdialog.h
* 摘    要：  声明窗体wnd_frmdialog(对话框)的窗体处理线程及相关操作函数。该窗体
*             提供了对话框功能。
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMDIALOGSENDFILE_H
#define _WND_FRMDIALOGSENDFILE_H


/*******************************************************************************
**							为使用GUI而需要引用的头文件						  **
*******************************************************************************/
#include "guiglobal.h"
#include "wnd_global.h"


/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/
//窗体frmdialog的初始化函数，建立窗体控件、注册消息处理
int FrmDialogSendfileInit(void *pWndObj);
//窗体frmdialog的退出函数，释放所有资源
int FrmDialogSendfileExit(void *pWndObj);
//窗体frmdialog的绘制函数，绘制整个窗体
int FrmDialogSendfilePaint(void *pWndObj);
//窗体frmdialog的循环函数，进行窗体循环
int FrmDialogSendfileLoop(void *pWndObj);
//窗体frmdialog的挂起函数，进行窗体挂起前预处理
int FrmDialogSendfilePause(void *pWndObj);
//窗体frmdialog的恢复函数，进行窗体恢复前预处理
int FrmDialogSendfileResume(void *pWndObj);


//定义文件传输数据结构
typedef struct _file_sendtopc
{
	char 		PC_Name[256];  			//PC端名字
	char 		PC_Address[128];		//PC端IP地址
	int			iSendSuccess;			//成功传送文件个数
	int			iSendFail;				//传送失败文件个数
	int			iCount;					//总的文件传输个数
	int 		iStart;					//开始传送文件开关，1为开始传送
	int			iTimeout;				//超时标志
}FILESENDTOPC;

GUIMUTEX		mSendfile;				//互斥锁
/***
  * 功能：
		按下OK按钮后需要显示的信息
  * 参数：
		1.info : 	需要显示的信息
  * 返回：
		无
  * 备注：
***/
void ShowDialogSendfileExtraInfo(GUICHAR *info);


/***
  * 功能：
		建立对话框
  * 参数：
		1.x         : 	对话框左上角的横坐标
		2.y         : 	对话框左上角的纵坐标
		3.func	   : 	还原窗体的回调函数
		4.cancelfunc: 	按下cancel按钮后执行的回调函数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
  		如果没有需要在按下cancel按钮后需要处理的回调函数，可输入NULL
***/
void DialogSendfileInit(int x, int y, CALLLBACKWINDOW func, CALLLBACKHANDLE cancelfunc);

#endif
