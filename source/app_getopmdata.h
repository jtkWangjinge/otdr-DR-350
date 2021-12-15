/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_getopmdata.h  
* 摘    要：  获取光功率计AD数据 
*
* 当前版本：  v1.0.0 
* 作    者：  wjg
* 完成日期：  
*
*******************************************************************************/

#ifndef _APP_GET_OPM_DATA_
#define _APP_GET_OPM_DATA_

//命令
enum POWERMETER_CMD
{
    SET_ADS1110 = 0x10, //设置ADC
    SET_CHNNL = 0x20,   //设置通道
    SET_OPM = 0x40,     //开关5V OPM电源
};

//通道值
enum CHANNEL_DATA
{
    CHANNEL_0 = 0,      //通道0
    CHANNEL_1 = 1,      //通道1
    CHANNEL_2 = 2,      //通道2
    CHANNEL_3 = 3,      //通道3
    CHANNEL_4 = 4,      //通道4
    CHANNEL_5 = 5,      //通道5
    CHANNEL_6 = 6,      //通道6
    CHANNEL_7 = 7,      //通道7
    CHANNEL_COUNT = 8   //通道总数
};

//电源开关
enum POWER_SWITCH
{
    OPM_CLOSE = 0, //关闭电源
    OPM_OPEN = 1   //打开电源
};

//通道的边界值
#define MIN_AD_VALUE        600
#define MAX_AD_VALUE        7500
#define OPM_IOCTL           //从ioctl获取AD数值

/***
 * 功能：
    获取设备描述符
* 参数:
* 返回：
    成功返回0，失败返回负值
* 备注：
    测试通过
***/
int OpenOpmDevice(void);

/***
 * 功能：
    关闭设备描述符
* 参数:
* 返回：
* 备注：
    测试通过
***/
void CloseOpmDevice(void);

/***
 * 功能：
    设置opm电源开关命令
* 参数：
    1.unsigned int isOpen:      是否打开电源开关
* 返回：
    成功返回0，失败返回负值
* 备注：
    测试通过
***/
int SetPowerSwitch(unsigned int isOpen);

/***
 * 功能：
    设置ADC命令
* 参数:
    1.unsigned int uiIndex:     通道值（0~7）
* 返回：
    成功返回0，失败返回负值
* 备注：
    测试通过
***/
int SetChannelData(unsigned int uiIndex);

/***
 * 功能：
    设置ADC命令
* 参数：无
* 返回：
    成功返回0，失败返回负值
* 备注：
    测试通过
***/
int SetAdcCommand(void);

/***
 * 功能：
    读取ADC数据
* 参数:无
* 返回：
    成功返回ad值，失败返回负值
* 备注：
    测试通过
***/
short ReadAdcData(void);

/***
 * 功能：
    获取OPM测试数据
* 参数：
    1.unsigned int uiChannelIndex:               通道值
* 返回：
    成功返回ad值，失败返回负值
* 备注：
    测试通过
***/
short GetOpmData(unsigned int uiChannelIndex);

//获取浮点数数字的个数
int GetFloatNumbers(double data, int bitNumber);

//获取浮点数中各个位上的数字
void GetNumbers(double data, int *number, int bitNumber);

//通过字符串获取每个位上的数值
int *GetFloatNumbersByCharacters(float data, int *count);

#endif