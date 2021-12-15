/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guiwindow.h
* 摘    要：  定义GUI的窗体类型及操作，为实现窗体控件提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：
*******************************************************************************/

#ifndef _GUI_WINDOW_H
#define _GUI_WINDOW_H


/**************************************
* 为定义GUIWINDOW而需要引用的其他头文件
**************************************/
#include "guibase.h"
#include "guibrush.h"
#include "guipen.h"
#include "guifont.h"
#include "guimessage.h"


/********************************************
* 定义GUI中窗体对象所能包含控件对象的最大数量
********************************************/
#define GUIWINDOW_COMP_LIMIT    1000    //单个窗体最多可包含的控件对象


/************************************
* 定义GUI中窗体处理函数的调用接口类型
************************************/
typedef int (*WNDFUNC)(void *pWndObj);


/******************
* 定义GUI中窗体结构
******************/
typedef struct _window
{
    GUIVISIBLE Visible;     //窗体的可视信息

    GUIPEN *pWndPen;        //窗体所使用的画笔，为NULL则使用GUI中的当前画笔
    GUIBRUSH *pWndBrush;    //窗体所使用的画刷，为NULL则使用GUI中的当前画刷
    GUIFONT *pWndFont;      //窗体所使用的字体，为NULL则使用GUI中的当前字体

    WNDFUNC fnWndInit;      //指向窗体初始化函数，完成资源分配及消息注册等
    WNDFUNC fnWndExit;      //指向窗体退出函数，完成消息注销及资源回收
    WNDFUNC fnWndPaint;     //指向窗体绘制函数，完成窗体控件的绘制
    WNDFUNC fnWndLoop;      //指向窗体循环函数，完成窗体循环处理，为空则忽略
    WNDFUNC fnWndPause;     //指向窗体挂起函数，完成挂起前预处理，为空则忽略
    WNDFUNC fnWndResume;    //指向窗体恢复函数，完成恢复前预处理，为空则忽略

    THREADFUNC fnWndThread; //用于创建窗体处理线程，为空使用系统的窗体处理线程
    GUITHREAD thdWndTid;    //窗体线程ID，窗体处理线程的线程标识符

    GUILIST *pWndComps;     //控件队列，窗体所包含的GUI窗体控件对象
    GUIMUTEX Mutex;         //互斥锁，用于控制对控件队列的访问
} GUIWINDOW;


/**********************************
* 定义GUI中与窗体类型相关的操作函数
**********************************/
//根据指定的信息直接建立窗体对象
GUIWINDOW* CreateWindow(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                        unsigned int uiWndWidth, unsigned int uiWndHeight, 
                        WNDFUNC fnWndInit, WNDFUNC fnWndExit, 
                        WNDFUNC fnWndPaint, WNDFUNC fnWndLoop, 
                        WNDFUNC fnWndPause, WNDFUNC fnWndResume,
                        THREADFUNC fnWndThread);
//删除窗体对象
int DestroyWindow(GUIWINDOW **ppWndObj);

//初始化当前窗体
int InitCurrWindow(void);
//锁住当前窗体对象
int LockCurrWindow(void);
//解锁当前窗体对象
int UnlockCurrWindow(void);
//保持当前的窗体对象不变，需要与UnlockCurrWindow()成对使用
GUIWINDOW* HoldCurrWindow(void);
//得到当前的窗体对象
GUIWINDOW* GetCurrWindow(void);
//设置当前的窗体对象
int SetCurrWindow(GUIWINDOW *pWndObj);

//设置窗体的有效作用区域
int SetWindowArea(unsigned int uiStartX, unsigned int uiStartY, 
                  unsigned int uiEndX, unsigned int uiEndY,
                  GUIWINDOW *pWndObj);
//设置窗体所使用的画刷
int SetWindowBrush(GUIBRUSH *pWndBrush, GUIWINDOW *pWndObj);
//设置窗体所使用的画笔
int SetWindowPen(GUIPEN *pWndPen, GUIWINDOW *pWndObj);
//设置窗体所对应的字体
int SetWindowFont(GUIFONT *pWndFont, GUIWINDOW *pWndObj);

//添加GUI窗体控件到指定窗体
int AddWindowComp(int iCompTyp, int iCompLen, void *pWndComp, 
                  GUIWINDOW *pWndObj);
//从指定窗体删除GUI窗体控件
int DelWindowComp(void *pWndComp, GUIWINDOW *pWndObj);
//清空指定窗体的窗体控件队列
int ClearWindowComp(GUIWINDOW * pWndObj);


#endif  //_GUI_WINDOW_H

