/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiglobal.h
* 摘    要：  定义GUI的公共数据及声明相关操作，并提供对于GUI环境的初始化设置。
*
* 当前版本：  v1.0.0
* 作    者：  yexin.zhu
* 完成日期：  2012-8-1
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _GUI_GLOBAL_H
#define _GUI_GLOBAL_H


/**************************************
* 为定义GUIGLOBAL而需要引用的其他头文件
**************************************/
#include "guibase.h"
#include "guifbmap.h"
#include "guikeypad.h"
#include "guitouch.h"
#include "guievent.h"
#include "guimessage.h"
#include "guischeduler.h"
#include "guibrush.h"
#include "guifont.h"
#include "guipalette.h"
#include "guipen.h"
#include "guidraw.h"
#include "guiimage.h"
#include "guibitmap.h"
#include "guitext.h"
#include "guilabel.h"
#include "guipicture.h"
#include "guiwindow.h"


/********************************
* 定义GUI中当前使用的按键映射码值
********************************/
#define KEYCODE_File    	144
#define KEYCODE_HOME    	139
#define KEYCODE_Setup   	141
#define KEYCODE_BACK    	1 

#define KEYCODE_up      	103
#define KEYCODE_down    	108
#define KEYCODE_left    	105
#define KEYCODE_right   	106
#define KEYCODE_OTDR   	    28

#define KEYCODE_Start   	315
/**********************************
* 定义与GUI环境初始化相关的操作函数
**********************************/
//初始化GUI环境，分配GUI资源
int InitializeGUI(void);
//释放GUI环境，回收资源
int ReleaseGUI(void);


#endif  //_GUI_GLOBAL_H

