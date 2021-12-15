/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmthresholdset.h
* 摘    要：  实现主窗体FrmThresholdSet的窗体处理操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/7 
*
*******************************************************************************/

#ifndef _WND_FRM_THRESHOLDSET_H_
#define _WND_FRM_THRESHOLDSET_H_

//阈值设置的标题
enum otdrset_title
{
	TEST_SET = 0,
	OTDR_ANALYSIS_SET,
	THRESHOLD_SET,
	REPORT_SET
};

#define THRESHOLDSET_TITLE_NUM          4
extern char* pStrOtdrSetButton_unpress[THRESHOLDSET_TITLE_NUM];
extern char* pStrOtdrSetButton_press[THRESHOLDSET_TITLE_NUM];
/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
        窗体frmthresholdset的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmThresholdSetInit(void *pWndObj);

/***
  * 功能：
        窗体frmthresholdset的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmThresholdSetExit(void *pWndObj);

/***
  * 功能：
        窗体frmthresholdset的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetPaint(void *pWndObj);

/***
  * 功能：
        窗体frmthresholdset的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetLoop(void *pWndObj);

/***
  * 功能：
        窗体frmthresholdset的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetPause(void *pWndObj);

/***
  * 功能：
        窗体frmthresholdset的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmThresholdSetResume(void *pWndObj);


#endif


