/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_rj45.c
* 摘    要：  实现rj45公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*******************************************************************************/

#include <stdlib.h> //system()
#include <stdio.h>
#include <string.h> //memset()

#include "guibase.h" //MsecSleep

#include "app_global.h"
#include "app_rj45.h"
#include "app_gpio.h"

//GPIO操作
// #define GPIO_OPTION
#ifdef GPIO_OPTION
//各个网口对应的bit位
#define bit0 (0x1 << 0)
#define bit1 (0x1 << 1)
#define bit2 (0x1 << 2)
//gpio口对应的宏
#define GPIO_ETH_PA9    41  //AT91_PIN_PA9
#define GPIO_ETH1_A     48  //AT91_PIN_PA16
#define GPIO_ETH1_B     49  //AT91_PIN_PA17
#define GPIO_ETH1_C     50  //AT91_PIN_PA18
#define GPIO_ETH_EN     51  //AT91_PIN_PA19
#define GPIO_ETH_CLK2   157 //AT91_PIN_PD29;
#define GPIO_LEVEL_EN   158 //AT91_PIN_PD30;
#define GPIO_ETH1_OUT   159 //AT91_PIN_PD31;

#define GPIO_ETH2_IN    152 //AT91_PIN_PD24;
#define GPIO_ETH2_A     38  //AT91_PIN_PA6
#define GPIO_ETH2_B     39  //AT91_PIN_PA7
#define GPIO_ETH2_C     40  //AT91_PIN_PA8
#endif
//测试网线个数
#define RJ45_NUM        8

//rj45相关操作的结构体声明
struct netwire_option
{
  int length;
  int timer_value;
  unsigned char wire;
};

//ioctl操作
#define DEV_IOCTL_NETWIRE_MAGIC 242
#define NET_IOCTL_CHECK_NETWIRE _IOWR(DEV_IOCTL_NETWIRE_MAGIC, 0, struct netwire_option *)          //check_netware start
#define NET_IOCTL_START_LOOK_FOR_NETWIRE _IOWR(DEV_IOCTL_NETWIRE_MAGIC, 1, struct netwire_option *) //start look for netwire
#define NET_IOCTL_STOP_LOOK_FOR_NETWIRE _IOWR(DEV_IOCTL_NETWIRE_MAGIC, 2, struct netwire_option *)  //stop look for netwire
#define NET_IOCTL_LENGTH_NETWIRE _IOWR(DEV_IOCTL_NETWIRE_MAGIC, 3, struct netwire_option *)         //length of netwire

#ifdef GPIO_OPTION


/***
  * 功能：
        设置gpio数据
  * 参数：
    1、int gpio：       gpio编号
    2、int direction：  gpio方向（0：in，1：out）
    3、int value：      gpio编号
  * 返回：无
  * 备注：
***/
static void SetGpioValue(int gpio, int direction, int value)
{
    gpio_direction(gpio, direction);
    gpio_write(gpio, value);
}
#endif

/***
  * 功能：
        设置占空比
  * 参数：
    1、unsigned int value：      占空比数值(1~100)
  * 返回：成功返回0，失败返回-1
  * 备注：
***/
static int SetPwmDutyRatio(unsigned int value)
{
    //检查参数
    if (value > 100)
    {
        printf("value %d over range(1~100)!\n", value);
        return -1;
    }

    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/backlight/atmel-pwm-bl/brightness");
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        printf("Failed to open brightness value %d for writing!\n", value);
        return -1;
    }

    char buf[4];
    snprintf(buf, sizeof(buf), "%d", value);

    if (write(fd, buf, sizeof(buf)) < 0)
    {
        printf("Failed to write value %d!\n", value);
        return -1;
    }

    close(fd);
    return 0;
}

/***
 * 寻线功能如下
 ***/
/***
  * 功能：
        初始化寻线参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 1、申请gpio；
  * 2、初始化gpio数据
***/
void InitialCableSearchParameter(void)
{
#ifdef GPIO_OPTION
    //申请gpio
    gpio_export(GPIO_ETH1_A);
    gpio_export(GPIO_ETH1_B);
    gpio_export(GPIO_ETH1_C);
    gpio_export(GPIO_ETH_EN);
    gpio_export(GPIO_LEVEL_EN);
    //初始化eth_en
    SetGpioValue(GPIO_ETH_EN, 1, 1);
    //设置寻线模式
    SetGpioValue(GPIO_LEVEL_EN, 1, 0);
#endif
    //设置pwm的占空比
    SetPwmDutyRatio(50);
}

/***
  * 功能：
        销毁寻线参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 去除申请gpio
***/
void ClearCableSearchParameter(void)
{
#ifdef GPIO_OPTION
    gpio_unexport(GPIO_ETH1_A);
    gpio_unexport(GPIO_ETH1_B);
    gpio_unexport(GPIO_ETH1_C);
    gpio_unexport(GPIO_ETH_EN);
    gpio_unexport(GPIO_LEVEL_EN);
#else
    ioctl(GetGlb_DevFd()->iNetWireDev, NET_IOCTL_STOP_LOOK_FOR_NETWIRE, NULL);
#endif
}

/***
  * 功能：
        测试寻线功能
  * 参数：无
  * 返回：无
  * 备注：
***/
void TestCableSearch(void)
{
#ifdef GPIO_OPTION
    int i = 0;
    for (i = 0; i < RJ45_NUM; ++i)
    {
        SetGpioValue(GPIO_ETH1_A, 1, (i & 0x1));
        SetGpioValue(GPIO_ETH1_B, 1, (i & 0x02) >> 1);
        SetGpioValue(GPIO_ETH1_C, 1, (i & 0x04) >> 2);
        MsecSleep(20);//可修改，声音停顿延长/缩短，可能影响界面刷新
    }
#else
    ioctl(GetGlb_DevFd()->iNetWireDev, NET_IOCTL_START_LOOK_FOR_NETWIRE, NULL);
#endif
}

/***
 * 对线功能如下
 * 
 ***/
/***
  * 功能：
        初始化参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 1、申请gpio；
  * 2、初始化gpio数据
***/
void InitialCableSequenceParameter(void)
{
#ifdef GPIO_OPTION
    //申请gpio
    gpio_export(GPIO_ETH_EN);
    gpio_export(GPIO_ETH_CLK2);
    gpio_export(GPIO_LEVEL_EN);
    //设置对线模式
    SetGpioValue(GPIO_LEVEL_EN, 1, 1);
    gpio_export(GPIO_ETH_PA9);

    gpio_export(GPIO_ETH1_A);
    gpio_export(GPIO_ETH1_B);
    gpio_export(GPIO_ETH1_C);
    gpio_export(GPIO_ETH1_OUT);

    gpio_export(GPIO_ETH2_IN);
    gpio_direction(GPIO_ETH2_IN, 0);

    gpio_export(GPIO_ETH2_A);
    gpio_export(GPIO_ETH2_B);
    gpio_export(GPIO_ETH2_C);
#endif
}

/***
  * 功能：
        销毁参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 去除申请gpio
***/
void ClearCableSequenceParameter(void)
{
#ifdef GPIO_OPTION
    gpio_unexport(GPIO_ETH_EN);
    gpio_unexport(GPIO_ETH_CLK2);
    gpio_unexport(GPIO_LEVEL_EN);
    gpio_unexport(GPIO_ETH_PA9);

    gpio_unexport(GPIO_ETH1_A);
    gpio_unexport(GPIO_ETH1_B);
    gpio_unexport(GPIO_ETH1_C);
    gpio_unexport(GPIO_ETH1_OUT);

    gpio_unexport(GPIO_ETH2_IN);
    gpio_unexport(GPIO_ETH2_A);
    gpio_unexport(GPIO_ETH2_B);
    gpio_unexport(GPIO_ETH2_C);
#endif
}

/***
  * 功能：
        测试对线功能
  * 参数：无
  * 返回：无
  * 备注：
***/
int TestCableSequence(void)
{
#ifdef GPIO_OPTION
    int gpio_level = 0;
    int connectWire = 0;
    SetGpioValue(GPIO_ETH_EN, 1, 1);    //SET_ETH_EN(on);
    // SetGpioValue(GPIO_ETH2_IN, 0, 0);   //SET_ETH_IN(eth2_in);
    SetGpioValue(GPIO_ETH_CLK2, 1, 0);  //SET_ETH_OUT(off);
    SetGpioValue(GPIO_ETH_CLK2, 1, 1);  //SET_ETH_OUT(on);
    SetGpioValue(GPIO_ETH_PA9, 1, 0);   //gpio_direction_output(AT91_PIN_PA9, 0);

    //循环检测
    int i = 0;
    int j = 0;

    for (i = 0; i < RJ45_NUM; i++)
    {
        SetGpioValue(GPIO_ETH1_A, 1, (i & 0x1));
        SetGpioValue(GPIO_ETH1_B, 1, (i & 0x02) >> 1);
        SetGpioValue(GPIO_ETH1_C, 1, (i & 0x04) >> 2);
        MsecSleep(10);
        for ( j = 0; j < RJ45_NUM; j++)
        {
            if (j == i)
                continue;
            SetGpioValue(GPIO_ETH2_A, 1, (j & 0x1));
            SetGpioValue(GPIO_ETH2_B, 1, (j & 0x02) >> 1);
            SetGpioValue(GPIO_ETH2_C, 1, (j & 0x04) >> 2);
            MsecSleep(10);
            //获取gpio数据
            gpio_level = gpio_read(GPIO_ETH2_IN);

            if (gpio_level == 1)
            {
                connectWire |= (0x1 << i);
                break;
            }
        }
    }

    SetGpioValue(GPIO_ETH_EN, 1, 0); //SET_ETH_EN(0);
    SetGpioValue(GPIO_ETH_CLK2, 1, 0); //SET_ETH_OUT(off);
    //SET_ETH1(0, 0, 0);
    SetGpioValue(GPIO_ETH1_A, 1, 0);
    SetGpioValue(GPIO_ETH1_B, 1, 0);
    SetGpioValue(GPIO_ETH1_C, 1, 0);
    //SET_ETH2(0, 0, 0);
    SetGpioValue(GPIO_ETH2_A, 1, 0);
    SetGpioValue(GPIO_ETH2_B, 1, 0);
    SetGpioValue(GPIO_ETH2_C, 1, 0);

    return connectWire;
#else
    struct netwire_option data;
    ioctl(GetGlb_DevFd()->iNetWireDev, NET_IOCTL_CHECK_NETWIRE, &data);
    return data.wire;
#endif
}