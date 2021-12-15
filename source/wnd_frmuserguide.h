/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmuserguide.h
* 摘    要：  声明主窗体FrmUserGuide的窗体处理操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2021/6/23 
*
*******************************************************************************/

#ifndef _WND_FRM_USER_GUIDE_H_
#define _WND_FRM_USER_GUIDE_H_

/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
        窗体frmuserguide的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/
int FrmUserGuideInit(void *pWndObj);

/***
  * 功能：
        窗体frmuserguide的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/
int FrmUserGuideExit(void *pWndObj);

/***
  * 功能：
        窗体frmuserguide的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuidePaint(void *pWndObj);

/***
  * 功能：
        窗体frmuserguide的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuideLoop(void *pWndObj);

/***
  * 功能：
        窗体frmuserguide的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuidePause(void *pWndObj);

/***
  * 功能：
        窗体frmuserguide的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int FrmUserGuideResume(void *pWndObj);

#endif