/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmbrightset.c  
* 摘    要：  实现用于完成亮度设置模块功能的应用程序接口
*
* 当前版本：  v1.0.0 
* 作    者：wjg
* 完成日期：2020/8/31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "app_frmbrightset.h"

#include "app_frmbatteryset.h"
#include "app_parameter.h"

#include "guimyglobal.h"

//屏幕亮度
#define BRIGHTNESSPATH "/sys/devices/platform/atmel-pwm-bl/backlight/atmel-pwm-bl/brightness"

//系统参数结构体
extern PSYSTEMSET pCurSystemSet;                  

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

int BrightnessSet(unsigned int uiValue)
{
	int iFd;
	int iSize;
    //检查设备描述符
    iFd = GetGlb_DevFd()->iLightDev;
    if (iFd < 0)
    {
        perror("error iFd...");
        return -1;
    }
    
    //判断写0还是写1？
    int iValue = (uiValue > 0) ? 1 : 0;
    iSize = write(iFd, &iValue, sizeof(iValue));
    //检查写入字节数
    if (iSize != sizeof(iValue))
    {
        perror("write fd size error...");
        return -2;
    }

	return 0;
}

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
int SetBrightness(unsigned int uiDCValue, unsigned int uiACValue)
{
	if( (uiDCValue>90) || (uiACValue>90) )
	{
		return -1;
	}
#ifdef POWER_DEVICE
    if (CheckPowerOn(ADAPTER))	//有适配器插入
	{
		BrightnessSet(uiACValue);
	}
	else							//只有电池插入
	{
		BrightnessSet(uiDCValue);
	}
#endif
	return 0;
}

/***
  * 功能：
        获取液晶屏的亮度等级
  * 参数：无
  * 返回：0~9 等级
  * 备注：
***/
int GetScreenBrightness(void)
{
    int brightnessLevel = 0;
#ifdef POWER_DEVICE
    //检查适配器是否插入
    if(CheckPowerOn(ADAPTER))
    {   
        brightnessLevel= pCurSystemSet->uiACLCDBright / 10 - 1;
    }
    else
    {
        brightnessLevel= pCurSystemSet->uiDCLCDBright / 10 - 1;
    }
#endif
    return brightnessLevel;
}

/***
  * 功能：
        保存液晶屏的亮度
  * 参数：
        1.iSelected：亮度等级
  * 返回：无
  * 备注：
***/
void SaveScreenBrightness(int iSelected)
{
#ifdef POWER_DEVICE
    if(CheckPowerOn(ADAPTER))
    {
        pCurSystemSet->uiACLCDBright = (iSelected+1)*10;
    }
	else
    {
        pCurSystemSet->uiDCLCDBright = (iSelected+1)*10;
    }
#endif
}

/***
  * 功能：
        设置液晶屏变暗(用于截屏)
  * 参数：无
  * 返回：无
  * 备注：
***/

void SetBrightnessGray(void)
{
    char buf[128] = {0};
    memset(buf, 0, 128);
    
    // 设置暗
    snprintf(buf, 128, "echo 0 > %s", BRIGHTNESSPATH);
    mysystem(buf);
}

/***
  * 功能：
        恢复液晶屏亮度
  * 参数：
        1.brightness:原始液晶屏的亮度
  * 返回：无
  * 备注：
***/

void RecoveryBrightness(int brightness)
{
    char buf[128] = {0};
    memset(buf, 0, 128);
    
    // 设置暗
    snprintf(buf, 128, "echo %d > %s", brightness, BRIGHTNESSPATH);
    mysystem(buf);
}

/***
  * 功能：
        获取液晶屏待机时间
  * 参数：
        无
  * 返回：无
  * 备注：
***/

int GetScreenOffTime(void)
{
    int screeOffTime = 0;
#ifdef POWER_DEVICE
    screeOffTime = (CheckPowerOn(ADAPTER)) ? pCurSystemSet->uiACScreenOffValue
                                           : pCurSystemSet->uiDCScreenOffValue;
#endif
    return screeOffTime;
}

/***
  * 功能：
        设置液晶屏待机时间
  * 参数：
        1.screeOffTime:待机时间
  * 返回：无
  * 备注：
***/

void SetScreenOffTime(int screeOffTime)
{
#ifdef POWER_DEVICE
    if(CheckPowerOn(ADAPTER))
    {
        pCurSystemSet->uiACScreenOffValue = screeOffTime;
    }
    else
    {
        pCurSystemSet->uiDCScreenOffValue = screeOffTime;
    }
#endif   
}

/***
  * 功能：
        获取系统存储液晶屏待机时间索引
  * 参数：
  * 返回：正确索引值（0~3），错误返回-1
  * 备注：
***/

int GetScreenOffTimeIndex(int screeOffTime)
{
    int index[4] = {30, 60, 300, 0};
    int i;
    for(i = 0; i < 4; ++i)
    {
        if(screeOffTime == index[i])
        {
            return i;
        }
    }
    return -1;
}
