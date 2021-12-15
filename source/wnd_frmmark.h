/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_FrmMark.h
* 摘    要：  声明主窗体FrmMark 的窗体处理线程及相关
				操作函数
*
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/9/8
*
*******************************************************************************/

#ifndef _WND_FrmMark_H
#define _WND_FrmMark_H


/****************************
* 为使用GUI而需要引用的头文件
****************************/
#include "guiglobal.h"


typedef struct  _markparam
{
	//光纤
    char FiberMark[64];
    char CableMark[64];
    char Location_A[64];
    char Location_B[64];
    int Deraction; // 0表示a->b,1表示b->a
	//测量
	char Test_date[22];
	char Test_time[22];
	char Module[30];
	char Serial_num[22];
	//任务
	char TaskID[22];            //不在使用 2018/7/13 因为需要输入中文和韩语
	char Operator_A[64];
	char Operator_B[64];
	char Company[64];
	char Customer[64];
	char Note[320];
	char TaskID_Unicode[64];
}MarkParam;
/*********************
* 声明窗体处理相关函数
*********************/


//窗体FrmMark的初始化函数，建立窗体控件、注册
//消息处理
int FrmMarkInit(void *pWndObj);
//窗体FrmMark的退出函数，释放所有资源
int FrmMarkExit(void *pWndObj);
//窗体FrmMark的绘制函数，绘制整个窗体
int FrmMarkPaint(void *pWndObj);
//窗体FrmMark的循环函数，进行窗体循环
int FrmMarkLoop(void *pWndObj);
//窗体FrmMark的挂起函数，进行窗体挂起前预处理
int FrmMarkPause(void *pWndObj);
//窗体FrmMark的恢复函数，进行窗体恢复前预处理
int FrmMarkResume(void *pWndObj);

#endif  //_WND_FrmMark_H
