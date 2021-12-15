/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmreportset.h
* 摘    要：  实现主窗体FrmReportSet的窗体处理操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/7
*
*******************************************************************************/

#ifndef _WND_FRM_REPORTSET_H_
#define _WND_FRM_REPORTSET_H_

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
		窗体frmreportset的初始化函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		建立窗体控件、注册消息处理
***/
int FrmReportSetInit(void *pWndObj);

/***
  * 功能：
		窗体frmreportset的退出函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		释放所有资源
***/
int FrmReportSetExit(void *pWndObj);

/***
  * 功能：
		窗体frmreportset的绘制函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetPaint(void *pWndObj);

/***
  * 功能：
		窗体frmreportset的循环函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetLoop(void *pWndObj);

/***
  * 功能：
		窗体frmreportset的挂起函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetPause(void *pWndObj);

/***
  * 功能：
		窗体frmreportset的恢复函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmReportSetResume(void *pWndObj);


#endif
