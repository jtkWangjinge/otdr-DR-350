/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmbrightset.h  
* 摘    要：  用于完成亮度设置模块功能的应用程序接口声明
*
* 当前版本：  v1.0.0 
* 作    者：wjg
* 完成日期：2020/8/31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _APP_FRM_BRIGHTSET_H_
#define _APP_FRM_BRIGHTSET_H_

/***
  * 功能：
        设置液晶屏的亮度
  * 参数：
        1.uiValue:   亮度值，最大90
  * 返回：
        0			成功
        -1			失败
  * 备注：
***/

int BrightnessSet(unsigned int uiValue);

/***
  * 功能：
        设置液晶屏的亮度
  * 参数：
        1.uiDCValue:   亮度值，最大90
        2.uiACValue:   亮度值，最大90
  * 返回：
        0			成功
        -1			失败
  * 备注：
***/
int SetBrightness(unsigned int uiDCValue, unsigned int uiACValue);

/***
  * 功能：
        获取液晶屏的亮度等级
  * 参数：无
  * 返回：0~9 等级
  * 备注：
***/
int GetScreenBrightness(void);
/***
  * 功能：
        保存液晶屏的亮度
  * 参数：
        1.iSelected：亮度等级
  * 返回：无
  * 备注：
***/
void SaveScreenBrightness(int iSelected);

/***
  * 功能：
        设置液晶屏变暗(用于截屏)
  * 参数：无
  * 返回：无
  * 备注：
***/

void SetBrightnessGray(void);

/***
  * 功能：
        恢复液晶屏亮度
  * 参数：
        1.brightness:原始液晶屏的亮度
  * 返回：无
  * 备注：
***/

void RecoveryBrightness(int brightness);

/***
  * 功能：
        获取液晶屏待机时间
  * 参数：
        无
  * 返回：无
  * 备注：
***/

int GetScreenOffTime(void);


/***
  * 功能：
        设置液晶屏待机时间
  * 参数：
        1.screeOffTime:待机时间
  * 返回：无
  * 备注：
***/

void SetScreenOffTime(int screeOffTime);

/***
  * 功能：
        获取系统存储液晶屏待机时间索引
  * 参数：
  * 返回：正确索引值（0~3），错误返回-1
  * 备注：
***/

int GetScreenOffTimeIndex(int screeOffTime);


#endif

