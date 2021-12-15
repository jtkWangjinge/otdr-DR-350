/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmcalendar.h
* 摘    要：  声明窗体wnd_frmcalendar(日期或时间)的窗体处理线程及相关操作函数。
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRM_CALENDAR_H_
#define _WND_FRM_CALENDAR_H_

#include "wnd_global.h"

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

typedef enum Calendar_Type
{
    CALEBDAR_DATE = 0,
    CALEBDAR_TIME    
}CALENDAR_TYPE;

/***
  * 功能：
        外部调用函数显示日历（日期/时间）窗体
  * 参数：
		1.int x : 	                  对话框左上角的横坐标
		2.int y : 	                  对话框左上角的纵坐标
  		3.CALENDAR_TYPE calendarType：0：date，1：time
  		4.CALLLBACKWINDOW func：       窗体回调函数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int CalendarInit(int x, int y, CALENDAR_TYPE calendarType, CALLLBACKWINDOW func);

#endif

