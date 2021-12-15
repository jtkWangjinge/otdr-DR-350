/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmMain.h
* 摘    要：  声明主窗体FrmMain的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2014-8-18
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FrmMain_H
#define _WND_FrmMain_H

/**********************************************************************************
**				    	       为使用GUI而需要引用的头文件	            		 **
***********************************************************************************/
#include "guiglobal.h"

#define MENUFILE	1
#define MENUSYSSET	2
#define MENUSYSINFO	3
#define MENUWIFI	4
#define MENUHELP	5

typedef enum _cur_window_type
{
	ENUM_NULL_WIN		= 1,	//NULL初始化
	ENUM_LOGO_WIN		= 2,	//LOGO界面
	ENUM_MAIN_WIN		= 3,	//主界面
	ENUM_OTDR_WIN		= 4,	//OTDR界面
	ENUM_FAULT_WIN		= 5,	//故障检测界面
	ENUM_PORT_WIN		= 6,	//光纤端面界面
	ENUM_LIGHT_WIN		= 7,	//可视光源界面
	ENUM_FILE_WIN		= 8,	//文件管理界面
	ENUM_SETTING_WIN	= 9,	//设置界面
	ENUM_ABOUT_WIN		= 10,	//设置界面
	ENUM_OTHER_WIN		= 11,	//其他界面
	ENUM_SCREENSHOT_WIN = 12,   //需要有截图标签的界面
	ENUM_SAVE_OTDR_WIN	= 13,	//保存otdr界面
	ENUM_OPEN_OTDR_WIN	= 14,   //打开otdr界面
	ENUM_DEBUG_INFO_WIN = 15,		//调试信息界面
	ENUM_LANGUAGE_CONFIG_WIN = 16,	//语言配置界面
	ENUM_NON_LINEARITY_WIN = 17,	//非线性度界面
	ENUM_UNCERTAINTY_WIN = 18,		//不确定性界面
	ENUM_FACTORY_SET_WIN = 19,		//工厂设置界面
	ENUM_AUTHORIZATION_WIN = 20,	//授权管理界面
	ENUM_SERIALNUM_WIN = 21,		//序列号管理界面
	ENUM_CABLE_RANGE_WIN = 22,		//网线测距界面
	ENUM_CABLE_SEQUENCE_WIN = 23,	//网线测序界面
	ENUM_CABLE_SEARCH_WIN = 24,		//网线寻线界面
}CURR_WINDOW_TYPE;

/**********************************************************************************
**				    	       声明窗体处理相关函数			            		 **
***********************************************************************************/
//窗体FrmMain的初始化函数，建立窗体控件、注册消息处理
int FrmMainInit(void *pWndObj);

//窗体FrmMain的退出函数，释放所有资源
int FrmMainExit(void *pWndObj);

//窗体FrmMain的绘制函数，绘制整个窗体
int FrmMainPaint(void *pWndObj);

//窗体FrmMain的循环函数，进行窗体循环
int FrmMainLoop(void *pWndObj);

//窗体FrmMain的挂起函数，进行窗体挂起前预处理
int FrmMainPause(void *pWndObj);

//窗体FrmMain的恢复函数，进行窗体恢复前预处理
int FrmMainResume(void *pWndObj);

//获得当前的窗体类型
int GetCurrWindowType(void);

void FrmMainReCreateWindow(GUIWINDOW **pWnd);

#endif  //_WND_FrmMain_H