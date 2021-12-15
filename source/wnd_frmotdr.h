/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotd.h
* 摘    要：  实现主窗体frmotdr的窗体处理线程及相关操作函数声明
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015.05.22
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _WND_FRMOTDR_H
#define _WND_FRMOTDR_H
#include "guibase.h"
#include "guiwindow.h"
#include "wnd_global.h"


//定义选项卡类型宏
#define OTDR_OPC_SETUP 		0
#define OTDR_OPC_EVENT 		1
#define OTDR_OPC_MEASU 		2
#define OTDR_OPC_LINK_MAP	3
#define OTDR_OPC_CINFO 		3
#define OTDR_OPC_USER 		4


//定义OTDR选项卡数据结构类型
typedef int (*OPC_PAINT)(void *);
typedef int (*OPC_SETUP)(void *, int, GUIWINDOW *);

//光标类型
typedef enum _coursor_type
{
    EN_TCURSOR   = 0,			//光标类型
    EN_THAND	 = 1, 			//手型类型
}COURSOR_TYPE;

//放大模式
typedef enum _EZoom_mode
{
    EN_SCATCH	= 0,			//拖拽模式
    EN_NORMAL	= 1,			//正常模式
}EZOOM_MODE;

//窗体相关参数
typedef struct _otdr_drawcurvepart_set
{
	COURSOR_TYPE	enCursorType;	//光标类型
	EZOOM_MODE		enZoomMode;		//放大模式	
}OTDR_WIN_PARA, *POTDR_WIN_PARA;

//定义OTDR选项卡数据结构
typedef struct _otdr_opc
{
	OPC_PAINT pOpcPaint;  			//选项卡的绘制
	OPC_SETUP pOpcSetup;			//选项卡的设置
	void 	  *pOpc;				//选项卡句柄
	int 	  iCurrCard;			//当前的选项卡
	int 	  iCurrWnd;				//当前的窗体类型  COOR_NOMAL  COOR_LARGE
}OTDROPC;

//改变参数后重新开始测量
void ChangeArgsRestart(int arg);
void CheckOtdrStop(int arg);

//函数声明
int FrmOtdrInit(void *pWndObj);
int FrmOtdrPaint(void *pWndObj);
int FrmOtdrLoop(void *pWndObj);
int FrmOtdrPause(void *pWndObj);
int FrmOtdrResume(void *pWndObj);
int FrmOtdrExit(void *pWndObj);

//读取sor到otdr界面

int OtdrSorViewer(const char* fileName, GUIWINDOW* from, CALLLBACKWINDOW where);

//显示下一条曲线
void DisplayNextCur(void);
//获得OTDR当前窗体的模式    普通模式或伸展模式
int GetOtdrCurrWnd(void);
//切换到测量界面 传入1切换到测量界面， 0切换回来
int OtdrSwitchToMeas(int iSwitchDir);
void otdrModifyCoorScale(int Mode);
void otdrStartOrStopMeasure(void);
void OtdrScathCoorAdjust(unsigned int* CoorX, unsigned int* CoorY);

//测试otdr(在otdr设置各个界面可直接测试)
void OtdrTest(int iRefreshPara);

#endif	//_WND_FRMOTDR_H

