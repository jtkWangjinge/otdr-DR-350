/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmversioninfo.h
* 摘    要：  主窗体FrmVersionInfo的窗体处理操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/2 
*
*******************************************************************************/

#ifndef _WND_FRM_VERSIONINFO_H_
#define _WND_FRM_VERSIONINFO_H_

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
        窗体frmversioninfo的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmVersionInfoInit(void *pWndObj);

/***
  * 功能：
        窗体frmversioninfo的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmVersionInfoExit(void *pWndObj);

/***
  * 功能：
        窗体frmversioninfo的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoPaint(void *pWndObj);

/***
  * 功能：
        窗体frmversioninfo的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoLoop(void *pWndObj);

/***
  * 功能：
        窗体frmversioninfo的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoPause(void *pWndObj);

/***
  * 功能：
        窗体frmversioninfo的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmVersionInfoResume(void *pWndObj);


#endif


