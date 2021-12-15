/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_speaker.c
* 摘    要：  实现蜂鸣器公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者： 
* 完成日期：  
*******************************************************************************/

#include "app_speaker.h"

#include "app_global.h"
#include "app_parameter.h"

//光模块结构体
static POPMSET s_pOpmSet;

extern PSYSTEMSET pCurSystemSet;

/***
  * 功能：
        使能按键蜂鸣器
  * 参数：
        unsigned int iEnable: 0:不使能，1:使能
  * 返回：
  * 备注：
          触摸屏和按键响
***/
void SetWarningSpeakerEnable(unsigned int iEnable)
{
    pCurSystemSet->uiWarning = iEnable;
}

/***
  * 功能：
        使能按键蜂鸣器
  * 参数：
        unsigned int iEnable: 0:不使能，1:使能
  * 返回：
  * 备注：
          用于OTDR开始测试时,响一下
***/
void SetKeyWarningSpeakerEnable(unsigned int iEnable)
{
    pCurSystemSet->ucKeyWarning = iEnable;
}

/***
  * 功能：
        使能按键蜂鸣器
  * 参数：
        unsigned int iEnable: 0:不使能，1:使能
  * 返回：
  * 备注：
          用于OTDR开始测试时,响一下
***/
void SetOpmSet(POPMSET pOpmSet)
{
    s_pOpmSet = pOpmSet;
}

/***
  * 功能：
        使蜂鸣器滴答响
  * 参数：
  * 返回：
  * 备注：
          用于OTDR开始测试时,响一下
***/
void SpeakerStart(void)
{
    //获取光模块设备描述符
    DEVFD *pDevFd = NULL;

    if (pCurSystemSet->ucKeyWarning)
    {
        int iOpmFd;
        pDevFd = GetGlb_DevFd();
        iOpmFd = pDevFd->iOpticDev;

        if (Opm_SetSpeaker(iOpmFd, s_pOpmSet, 1) < 0)
            LOG(LOG_ERROR,"---set speaker 0 failed");
    }
}

/***
  * 功能：
        使蜂鸣器滴答响
  * 参数：
  * 返回：
  * 备注：
          触摸屏和按键响
***/
void SpeakerTick(void)
{
    //获取光模块设备描述符
    DEVFD *pDevFd = NULL;

    if (pCurSystemSet->uiWarning)
    {
        int iOpmFd;
        pDevFd = GetGlb_DevFd();
        iOpmFd = pDevFd->iOpticDev;

        if (Opm_SetSpeaker(iOpmFd, s_pOpmSet, 1) < 0)
            LOG(LOG_ERROR,"---set speaker 0 failed");
    }
}

/***
  * 功能：
         使蜂鸣器滴答响
  * 参数：
  * 返回：
  * 备注：
        温度异常警报
***/
void SpeakerAlarm(void)
{
    //获取光模块设备描述符
    DEVFD *pDevFd = NULL;
    int iOpmFd;

    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

    if (Opm_SetSpeaker(iOpmFd, s_pOpmSet, 1) < 0)
        LOG(LOG_ERROR,"---set speaker 0 failed");
}

/***
  * 功能：
        获取按键蜂鸣器的使能标志位
  * 参数：
  * 返回：
  * 备注：
          触摸屏和按键响
***/
unsigned int GetWarningSpeakerEnable(void)
{
      return pCurSystemSet->uiWarning;
}
