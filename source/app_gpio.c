/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_gpio.c
* 摘    要：  实现gpio公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*******************************************************************************/

#include "app_gpio.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define MSG(args...) printf(args)


int gpio_export(int pin)
{
    char buffer[64];
    int len;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0)
    {
        MSG("Failed to open export for writing!\n");
        return (-1);
    }

    len = snprintf(buffer, sizeof(buffer), "%d", pin);
    // printf("%s,%d,%d\n", buffer, sizeof(buffer), len);
    if (write(fd, buffer, len) < 0)
    {
        MSG("Failed to export gpio!");
        return -1;
    }

    close(fd);
    return 0;
}

int gpio_unexport(int pin)
{
    char buffer[64];
    int len;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0)
    {
        MSG("Failed to open unexport for writing!\n");
        return -1;
    }

    len = snprintf(buffer, sizeof(buffer), "%d", pin);
    if (write(fd, buffer, len) < 0)
    {
        MSG("Failed to unexport gpio!");
        return -1;
    }

    close(fd);
    return 0;
}
//dir: 0-->IN, 1-->OUT
int gpio_direction(int pin, int dir)
{
    static const char dir_str[] = "in\0out";
    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        MSG("Failed to open gpio direction %d for writing!\n", pin);
        return -1;
    }

    if (write(fd, &dir_str[dir == 0 ? 0 : 3], dir == 0 ? 2 : 3) < 0)
    {
        MSG("Failed to set direction!\n");
        return -1;
    }

    close(fd);
    return 0;
}
//value: 0-->LOW, 1-->HIGH
int gpio_write(int pin, int value)
{
    static const char values_str[] = "01";
    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        MSG("Failed to open gpio value %d for writing!\n", pin);
        return -1;
    }

    if (write(fd, &values_str[value == 0 ? 0 : 1], 1) < 0)
    {
        MSG("Failed to write value %d!\n", pin);
        return -1;
    }

    close(fd);
    return 0;
}

int gpio_read(int pin)
{
    char path[64];
    char value_str[3];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        MSG("Failed to open gpio value for reading!\n");
        return -1;
    }

    if (read(fd, value_str, 3) < 0)
    {
        MSG("Failed to read value!\n");
        return -1;
    }

    close(fd);
    return (atoi(value_str));
}
// none表示引脚为输入，不是中断引脚
// rising表示引脚为中断输入，上升沿触发
// falling表示引脚为中断输入，下降沿触发
// both表示引脚为中断输入，边沿触发
// 0-->none, 1-->rising, 2-->falling, 3-->both
int gpio_edge(int pin, int edge)
{
    const char dir_str[] = "none\0rising\0falling\0both";
    char ptr;
    char path[64];
    int fd;
    switch (edge)
    {
    case 0:
        ptr = 0;
        break;
    case 1:
        ptr = 5;
        break;
    case 2:
        ptr = 12;
        break;
    case 3:
        ptr = 20;
        break;
    default:
        ptr = 0;
    }

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        MSG("Failed to open gpio edge for writing!\n");
        return -1;
    }

    if (write(fd, &dir_str[(int)ptr], strlen(&dir_str[(int)ptr])) < 0)
    {
        MSG("Failed to set edge!\n");
        return -1;
    }

    close(fd);
    return 0;
}