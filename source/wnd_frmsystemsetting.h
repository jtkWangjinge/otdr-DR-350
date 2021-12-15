/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsystemsetting.h
* 摘    要：  声明主窗体FrmSystemSetting的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/31 
*
*******************************************************************************/
#ifndef _WND_FRM_SYSTEM_SETTING_H_
#define _WND_FRM_SYSTEM_SETTING_H_

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
        窗体frmsystemsetting的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmSystemSettingInit(void *pWndObj);

/***
  * 功能：
        窗体frmsystemsetting的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmSystemSettingExit(void *pWndObj);

/***
  * 功能：
        窗体frmsystemsetting的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingPaint(void *pWndObj);

/***
  * 功能：
        窗体frmsystemsetting的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingLoop(void *pWndObj);

/***
  * 功能：
        窗体frmsystemsetting的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingPause(void *pWndObj);

/***
  * 功能：
        窗体frmsystemsetting的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmSystemSettingResume(void *pWndObj);

/***
  * 功能：
        获取恢复出厂设置的标志位
  * 参数：
		无
  * 返回：
        无
  * 备注：
***/
unsigned char GetFactoryFlag(void);

#endif


