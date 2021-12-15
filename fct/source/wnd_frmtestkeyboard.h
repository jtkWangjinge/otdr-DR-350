/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmtestkeyboard.c
* 摘    要：  声明主窗体frmtestkeyboard的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/10/9
*
*******************************************************************************/

#ifndef _WND_FRM_TEST_KEYBOARD_H_
#define _WND_FRM_TEST_KEYBOARD_H_

#include "guipicture.h"
#include "guiwindow.h"

// #include "wnd_frmselector.h"
//定义回调函数类型
typedef void(*SECBACKFUNC)(int selected);

#define MAX_KEY_NUM		12

struct KeyBoard_Control
{
	GUIPICTURE* pBg;
	GUIPICTURE* pBtn[MAX_KEY_NUM];
	int iFocus;
	int iEnabled;
	SECBACKFUNC pCallBack;
};

enum KEYBOARD_CODE
{
	KEYBOARD_SHIFT = 0,
	KEYBOARD_OTDR_LEFT,
	KEYBOARD_OTDR_RIGHT,
	KEYBOARD_OTDR_TEST,
	KEYBOARD_UP,
	KEYBOARD_DOWN,
	KEYBOARD_LEFT,
	KEYBOARD_RIGHT,
	KEYBOARD_ENTER,
	KEYBOARD_FILE,
	KEYBOARD_VFL,
	KEYBOARD_ESC
};

/***
  * 功能：
		创建一个按键控件
  * 参数：
		1.int iStartX:			控件的起始横坐标
		2.int iStartY:			控件的起始纵坐标
  * 返回：
		成功返回有效指针，失败NULL
  * 备注：
***/
struct KeyBoard_Control* CreateKeyBoard(int iStartX, int iStartY, SECBACKFUNC CallBack);
/***
  * 功能：
		销毁一个按键控件
  * 参数：
		struct KeyBoard_Control** pKeyBoardControl：指向按键控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DestroyKeyBoardControl(struct KeyBoard_Control** pKeyBoardControl);
/***
  * 功能：
		显示一个按键控件
  * 参数：
		struct KeyBoard_Control* pKeyBoardControl：指向按键控件的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DisplayKeyBoardControl(struct KeyBoard_Control* pKeyBoardControl);
/***
  * 功能：
		注册一个按键控件
  * 参数：
		1.struct KeyBoard_Control* pKeyBoardControl：指向按键控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int AddKeyBoardControlToWnd(struct KeyBoard_Control* pKeyBoardControl, GUIWINDOW* pWnd);
/***
  * 功能：
		移除一个按键控件
  * 参数：
		1.struct KeyBoard_Control* pKeyBoardControl：指向按键控件的指针
		2.GUIWINDOW* pWnd：指向窗体的指针
  * 返回：
		成功返回0，失败非0
  * 备注：
***/
int DelKeyBoardControlFromWnd(struct KeyBoard_Control* pKeyBoardControl, GUIWINDOW* pWnd);

#endif
