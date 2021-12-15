/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmotdrtime.h  
* 摘    要：  定义用于完成OTDR测量计时功能的应用程序接口
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  
* 原 作 者：  
* 完成日期： 
*******************************************************************************/

#ifndef _APP_FRMOTDRTIME_H
#define _APP_FRMOTDRTIME_H

//初始化
void OtdrTimeInit(void);

//设置时间 成功返回0 失败返回-1
int OtdrTimeSet(int newTime);

//开始计时 成功返回0 失败返回-1
int OtdrTimeStart(void);

//开始计时 成功返回0 失败返回-1
int OtdrTimeStop(void);

//超时标志 返回1 超时
int OtdrTimeTimeOut(void);

//获得已消耗的时间
int OtdrTimeGetElapsedTime();

//是否实时模式
int OtdrTimeIsRT(void);

#endif

