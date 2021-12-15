/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmchecklight.h
* 摘    要：  实现右侧菜单栏控件
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMCHECKLIGHT_H
#define _WND_FRMCHECKLIGHT_H


/*******************************************************************************
***                引用wnd_frmchecklight.h所需头文件                         ***
*******************************************************************************/
#include "wnd_global.h"
#include "app_global.h"


//定义回调函数类型
typedef void (*CHECKCALLFUNC)(int select);

/*******************************************************************************
***                定义wnd_frmchecklight.h公有数据类型                       ***
*******************************************************************************/
typedef struct _wnd_check_light {
	GUIPICTURE *pCheckLightBg;	//背景
	
	GUILABEL   *pCheckLblTitle;
	GUICHAR    *pCheckStrTitle;
	
	GUILABEL   *pCheckLblInfo;	
	GUICHAR    *pCheckStrInfo;
	
	GUIPICTURE *pCheckOk;			//Ok按键
	GUIPICTURE *pCheckCancel;			//Cancel按钮
	GUILABEL   *pCheckLblOk;	
	GUICHAR    *pCheckStrOk;
	GUILABEL   *pCheckLblCancel;	
	GUICHAR    *pCheckStrCancel;

	GUILABEL   *pCheckLblLayer;

	GUIWINDOW  *pWnd;
	
	CHECKCALLFUNC CallBack;			//点击按键的回调
	int buttonType;					//按钮类型(0:只显示取消按钮  1:只显示确认按钮  2:同时显示确认和取消)
}WNDCHECKLIGHT;



/*******************************************************************************
***                声明wnd_frmchecklight.h公有函数                           ***
*******************************************************************************/

/***
  * 功能：
     	创建一个提示框
  * 参数：
  		1.TextIndex		              :菜单项的数量
  		2.int button                  :提示框按钮，0:只显示取消按钮  1:只显示确认按钮  2:同时显示确认和取消
  		2.GUIWINDOW *pWnd             :当前窗体
  		3.CHECKCALLFUNC CallBack	  :回调函数
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
WNDCHECKLIGHT *CreateDialog(int TitleIndex, int TextIndex, int button, GUIWINDOW *pWnd,CHECKCALLFUNC CallBack);

#endif //_WND_FRMCHECKLIGHT_H
