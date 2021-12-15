/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_speaker.h
* 摘    要：  声明蜂鸣器公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者： 
* 完成日期：  
*******************************************************************************/

#ifndef _APP_SPEAKER_H_
#define _APP_SPEAKER_H_

#include "app_middle.h"

/***
  * 功能：
        使能按键蜂鸣器
  * 参数：
        unsigned int iEnable: 0:不使能，1:使能
  * 返回：
  * 备注：
  		触摸屏和按键响
***/
void SetWarningSpeakerEnable(unsigned int iEnable);

/***
  * 功能：
        使能按键蜂鸣器
  * 参数：
        unsigned int iEnable: 0:不使能，1:使能
  * 返回：
  * 备注：
  		用于OTDR开始测试时,响一下
***/
void SetKeyWarningSpeakerEnable(unsigned int iEnable);

/***
  * 功能：
        使能按键蜂鸣器
  * 参数：
        POPMSET pOpmSet: 光模块结构体指针
  * 返回：
  * 备注：
  		用于OTDR开始测试时,响一下
***/
void SetOpmSet(POPMSET pOpmSet);

/***
  * 功能：
        使蜂鸣器滴答响
  * 参数：
  * 返回：
  * 备注：
  		用于OTDR开始测试时,响一下
***/
void SpeakerStart(void);

/***
  * 功能：
        使蜂鸣器滴答响
  * 参数：
  * 返回：
  * 备注：
  		触摸屏和按键响
***/
void SpeakerTick(void);

/***
  * 功能：
 		使蜂鸣器滴答响
  * 参数：
  * 返回：
  * 备注：
        温度异常警报
***/
void SpeakerAlarm(void);

/***
  * 功能：
        获取按键蜂鸣器的使能标志位
  * 参数：
  * 返回：
  * 备注：
          触摸屏和按键响
***/
unsigned int GetWarningSpeakerEnable(void);

#endif