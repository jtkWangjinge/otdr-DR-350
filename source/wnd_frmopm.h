/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmopm.h
* 摘    要：  实现主窗体FrmOPM的窗体处理操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/28 
*
*******************************************************************************/
#ifndef _WND_FRM_OPM_H_
#define _WND_FRM_OPM_H_

/*******************************************************************************
**							结构体声明							  **
*******************************************************************************/

//数字图片资源
typedef struct _OPMNUMBERBMP
{
    char* pNullNumber;                       //图片的空资源
    char* pNegativeNumber;                   //图片的负号资源
    char* pPointNumber;                      //图片的小数点资源
    char** pNumber;                          //图片的数字资源
}OPMNUMBERBMP;

//定义得到OPM的power的值的情况
typedef enum _opm_power_state
{
      TOOHIGHT    = 0,
      NORMAL      = 1,
      TOOLOW      = 2
}OPMPOWERSTATE;
/*******************************************************************************
**							声明窗体处理相关函数							  **
*******************************************************************************/

/***
  * 功能：
        窗体frmopm的初始化函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		建立窗体控件、注册消息处理
***/ 
int FrmOpmInit(void *pWndObj);

/***
  * 功能：
        窗体frmopm的退出函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		释放所有资源
***/ 
int FrmOpmExit(void *pWndObj);

/***
  * 功能：
        窗体frmopm的绘制函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmPaint(void *pWndObj);

/***
  * 功能：
        窗体frmopm的循环函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmLoop(void *pWndObj);

/***
  * 功能：
        窗体frmopm的挂起函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmPause(void *pWndObj);

/***
  * 功能：
        窗体frmopm的恢复函数
  * 参数：
  		void *pWndObj:	指向当前窗体对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/ 
int FrmOpmResume(void *pWndObj);


#endif

