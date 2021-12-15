/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmreportset.h
* 摘    要：  声明主窗体FrmOtdrAnalysisSet的窗体处理操作函数
*
* 当前版本：v1.0.0
* 作    者：wjg
* 完成日期：
*
*******************************************************************************/

#ifndef _WND_FRM_OTDR_ANALYSIS_SET_H_
#define _WND_FRM_OTDR_ANALYSIS_SET_H_

/***
  * 功能：
		窗体frmotdranalysisset的初始化函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		建立窗体控件、注册消息处理
***/
int FrmOtdrAnalysisSetInit(void *pWndObj);

/***
  * 功能：
		窗体frmotdranalysisset的退出函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
		释放所有资源
***/
int FrmOtdrAnalysisSetExit(void *pWndObj);

/***
  * 功能：
		窗体frmotdranalysisset的绘制函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetPaint(void *pWndObj);

/***
  * 功能：
		窗体frmotdranalysisset的循环函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetLoop(void *pWndObj);

/***
  * 功能：
		窗体frmotdranalysisset的挂起函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetPause(void *pWndObj);

/***
  * 功能：
		窗体frmotdranalysisset的恢复函数
  * 参数：
		void *pWndObj:	指向当前窗体对象
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int FrmOtdrAnalysisSetResume(void *pWndObj);

#endif