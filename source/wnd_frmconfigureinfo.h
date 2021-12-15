/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmconfigure.h
* 摘    要：  实现主窗体wnd_frmconfigure的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2020/10/27
*
*******************************************************************************/

#ifndef _WND_FRMCONFIGUREINFO_H
#define _WND_FRMCONFIGUREINFO_H

enum factory_menu
{
	DEBUG_INFO = 0,
	LANGUAGE_SET,
	NON_LINEARITY,
	UNCERTAINTY,
	FACTORY_SET,
	AUTHORIZATION,
	FACTORY_MENU_COUNT
};

int FrmConfigureInfoInit(void *pWndObj);
int FrmConfigureInfoPaint(void *pWndObj);
int FrmConfigureInfoLoop(void *pWndObj);
int FrmConfigureInfoPause(void *pWndObj);
int FrmConfigureInfoResume(void *pWndObj);
int FrmConfigureInfoExit(void *pWndObj);

//配置信息的按键响应
void DebugInfoKeyCallBack(int iSelected);
#endif	//_WND_FRMCONFIGUREINFO_H

