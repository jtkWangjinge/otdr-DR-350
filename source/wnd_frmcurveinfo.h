/**************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcurveinfo.h
* 摘    要：  声明主窗体frmcurveinfo的窗体处理线程及相
				关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期： 
**************************************************************/

#ifndef _WND_FRMCURVEINFO_H
#define _WND_FRMCURVEINFO_H


/**************************************************************
* 			为使用GUI而需要引用的头文件
**************************************************************/
#include "guiglobal.h"


//刷新曲线信息
int RefreshCurveInfo(void *arg);
//设置是否显示曲线信息
int SetCurveInfoDisplay(void *arg, int iFlag, GUIWINDOW *pWnd);
//获得是否显示曲线信息
int GetCurveInfoDisplay(void);
//创建曲线信息控件
int CreateCurveInfo(GUIWINDOW *pWnd, int iType);
//销毁
void DestoryCurveInfo();


#endif  //_WND_FRMCURVEINFO_H
