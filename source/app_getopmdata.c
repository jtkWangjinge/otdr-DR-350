/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_getopmdata.c  
* 摘    要：  获取光功率计AD数据 
*
* 当前版本：  v1.0.0 
* 作    者：  wjg
* 完成日期：  
*
*******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "app_getopmdata.h"

//设备结点
#define OPM_DEV "/dev/powermeter_dev" ///< OPM设备
/***
 * BIT     7    6 5 4   3   2   1    0
   NAME ST/DRDY 0 0 SC DR1 DR0 PGA1 PGA0
   DEFAULT 1    0 0 0   1   1   0    0
 * */
/***
 * DR1 DR0 DATA RATE | NUMBER OF BITS | PGA1    PGA0   GAIN
    0   0   240SPS   |       12       |   0       0       1
    0   1   60SPS    |       14       |   0       1       2
    1   0   30SPS    |       15       |   1       0       4
    1   1   15SPS    |       16       |   1       1       8
 * */
#define ADS1110 0x86    //DR1DR0:01,PGA1PGA0:10
//设备描述符
static int iOpmfd = -1;
//ioctl 控制输出
#define DEV_IOCTL_POWER_MAGIC 241
#define POWER_IOCTL_CHANNEL_SET(a) _IOWR(DEV_IOCTL_POWER_MAGIC, a, int *)

/***
 * 功能：
    获取设备描述符
* 参数:
* 返回：
    成功返回0，失败返回负值
* 备注：
    测试通过
***/
int OpenOpmDevice(void)
{
    //获得光功率计设备文件描述符
    iOpmfd = open(OPM_DEV, O_RDWR);

    if (iOpmfd == -1)
    {
        perror(OPM_DEV);
        return -1;
    }

    return 0;
}

/***
 * 功能：
    关闭设备描述符
* 参数:
* 返回：
* 备注：
    测试通过
***/
void CloseOpmDevice(void)
{
    if (iOpmfd != -1)
    {
        close(iOpmfd);
    } 
}

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
int SetPowerSwitch(unsigned int isOpen)
{
    //检查参数
    if (iOpmfd < 0)
    {
        return -1;
    }

    char cmd[2] = {0};
    //设置通道
    cmd[0] = SET_OPM;
    cmd[1] = isOpen; //是否打开电源

    if (write(iOpmfd, cmd, 2) < 0)
    {
        perror("write power command failed!");
        return -2;
    }

    return 0;
}

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
int SetChannelData(unsigned int uiIndex)
{
    //检查参数
    if (iOpmfd < 0 || (uiIndex < CHANNEL_0 || uiIndex > CHANNEL_7))
    {
        return -1;
    }

    char cmd[2] = {0};
    //设置通道
    cmd[0] = SET_CHNNL;
    cmd[1] = uiIndex; //选择那一路通道(0~7)

    if (write(iOpmfd, cmd, 2) < 0)
    {
        perror("write channel command failed!");
        return -2;
    }

    return 0;
}

/***
 * 功能：
    设置ADC命令
* 参数：无
* 返回：
    成功返回0，失败返回负值
* 备注：
    测试通过
***/
int SetAdcCommand(void)
{
    //检查参数
    if (iOpmfd < 0)
    {
        return -1;
    }

    char cmd[2] = {0};
    //设置ADC:
    cmd[0] = SET_ADS1110;
    cmd[1] = ADS1110;

    if (write(iOpmfd, cmd, 2) < 0)
    {
        perror("write ADC command failed!");
        return -2;
    }

    return 0;
}

/***
 * 功能：
    读取ADC数据
* 参数:无
* 返回：
    成功返回0，失败返回负值
* 备注：
    测试通过
***/
short ReadAdcData(void)
{
    //检查参数
    if (iOpmfd < 0)
    {
        return -1;
    }

    int data = 0;

    if (read(iOpmfd, &data, sizeof(data)) <= 0)
    {
        perror("read data failed!");
        return -2;
    }

    short temp = data & 0xffff;
    short value = ((temp & 0xff00) >> 8) + ((temp & 0xff) << 8);

    return value;
}

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
short GetOpmData(unsigned int uiChannelIndex)
{
    //获得光功率计设备文件描述符
    if (iOpmfd == -1)
    {
        perror(OPM_DEV);
        return -1;
    }
#ifndef OPM_IOCTL
    //设置通道值
    if (SetChannelData(uiChannelIndex))
    {
        return -2;
    }
    //设置ADC:
    if (SetAdcCommand())
    {
        return -3;
    }
    //读取ADC数据
    short value = ReadAdcData();
#else
    short value = 0;
    ioctl(iOpmfd, POWER_IOCTL_CHANNEL_SET(uiChannelIndex), &value);
#endif
    return value;
}

//获取浮点数数字的个数
int GetFloatNumbers(double data, int bitNumber)
{
    int value = abs((int)(data * pow(10, bitNumber)));
    int count = 0;

    while (value)
    {
        value /= 10;
        count++;
    }

    //若数值小于1，总数加1
    if (fabs(data) < 1)
    {
        count++;
    }

    return count;
}

//获取浮点数中各个位上的数字
void GetNumbers(double data, int *number, int bitNumber)
{
    int value = abs((int)(data * pow(10, bitNumber)));
    int i = 0;

    while (value)
    {
        number[i] = value % 10;
        value /= 10;
        i++;
    }

    //若数值小于1，则补0
    if (fabs(data) < 1)
    {
        number[i] = 0;
    }
}

//通过字符串获取每个位上的数值
int *GetFloatNumbersByCharacters(float data, int *count)
{
    //转换成字符串
    char buff[10] = {0};
    sprintf(buff, "%.2f", data);
    //存储数值的个数
    int length = strlen(buff);
    int *number = NULL;
    number = (int *)malloc(length * sizeof(int));
    int i = 0;

    for (i = 0; i < length; ++i)
    {
        if (buff[i] != '-' && buff[i] != '.')
        {
            number[(*count)++] = buff[i] - '0';
        }
    }

    return number;
}
