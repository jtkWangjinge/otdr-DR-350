/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsystemsetcontrol.h
* 摘    要：  实现主窗体FrmSystemSetControl的窗体处理操作函数声明
*          控制系统设置的各个选项
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/31 
*
*******************************************************************************/

#ifndef _WND_FRM_SYSTEM_SET_CONTROL_H_
#define _WND_FRM_SYSTEM_SET_CONTROL_H_

#include "guipicture.h"
#include "guiwindow.h"
#include "guilabel.h"
#include "guibase.h"

#include "wnd_frmselector.h"

/*******************************************************************************
**							窗体FrmSystemSetting 功能结构定义				      **
*******************************************************************************/

#define SYSTEM_INFO_LIST_NUM                8
#define BRIGHTNESS_LEVEL                    9

typedef enum System_Set_Index
{
    LANGUAGE=0,
    AUTO_OFF,
    STANDBY_TIME,
    KEY_WARNING,
    USER_GUIDE,
    RESET,
    UPGRADE,
    VERSION_INFO
}SYSTEM_SET_INDEX;

typedef struct Brightness_Control
{
    GUICHAR* pFrmStrTitle;
    GUILABEL* pFrmLblTitle;
    GUIPICTURE* pFrmBtnCursor[BRIGHTNESS_LEVEL];
    GUIPICTURE* pFrmBtnCursorBar[BRIGHTNESS_LEVEL];
    GUIPICTURE* pFrmBgMask;
    GUIPICTURE* pFrmBtnSelected;
    GUIPICTURE* pFrmBgProgressBar;
    int iFocus;			//当前选中的亮度index
}BRIGHTNESS_CONTROL;

typedef struct System_Set_Control
{
    GUICHAR* pFrmStrTitle[SYSTEM_INFO_LIST_NUM];
    GUILABEL* pFrmLblTitle[SYSTEM_INFO_LIST_NUM];
    GUICHAR* pFrmStrData[SYSTEM_INFO_LIST_NUM];
    GUILABEL* pFrmLblData[SYSTEM_INFO_LIST_NUM];
    GUIPICTURE* pFrmEnterIcon[SYSTEM_INFO_LIST_NUM];
    GUIPICTURE *pFrmBtnIcon[SYSTEM_INFO_LIST_NUM];
    GUIPICTURE *pFrmStatusIcon[SYSTEM_INFO_LIST_NUM]; //针对于显示自动关机、待机时间以及按键音量
    GUIPICTURE* pFrmBgSelected[SYSTEM_INFO_LIST_NUM];
    SECBACKFUNC CallBack;
    BRIGHTNESS_CONTROL* pFrmBrightness;
    int iFocus;
    int iBreakNum;                       //转折点，之后除标题外没有文本
}SYSTEM_SET_CONTROL;

/*******************************************************************************
**							功能控件相关函数							  **
*******************************************************************************/
/***
  * 功能：
        创建一个系统信息控件
  * 参数：
        1.iNum            :转折点（弹出提示框）
        2.int iFocus      :菜单条目焦点 <= 0表示所有项都无焦点
        3.SECBACKFUNC     :回调函数
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
SYSTEM_SET_CONTROL* CreateSystemSetControlResource(int iNum, int iFocus, SECBACKFUNC CallBack);
/***
  * 功能：
     	销毁系统信息控件
  * 参数：
  		1.SYSTEM_SET_CONTROL** pSystemSetControl : 指向系统信息控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/
int DestroySystemSetControlResource(SYSTEM_SET_CONTROL** pSystemSetControl);
/***
  * 功能：
     	显示系统信息控件
  * 参数：
  		1.SYSTEM_SET_CONTROL* pSystemSetControl: 指向系统信息控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/
int DisplaySystemSetControlResource(SYSTEM_SET_CONTROL* pSystemSetControl);
/***
  * 功能：
     	添加控件到接受消息的控件队列
  * 参数：
  		1.SYSTEM_SET_CONTROL* pSystemSetControl: 指向系统信息控件
  		2.GUIWINDOW *pWnd   : 队列的所属窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有互斥锁的情况下调用
***/
int AddSystemSetControlToWindow(SYSTEM_SET_CONTROL* pSystemSetControl, GUIWINDOW* pWnd);
/***
  * 功能：
     	注册消息处理函数
  * 参数：
  		1.SYSTEM_SET_CONTROL* pSystemSetControl: 指向系统信息控件
  		2.GUIMESSAGE *pMsg  : 当前的消息队列
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有消息注册队列互斥锁的情况下调用
***/ 
int LoginSystemSetControlToMsg(SYSTEM_SET_CONTROL* pSystemSetControl, GUIWINDOW* pWnd);

#endif
