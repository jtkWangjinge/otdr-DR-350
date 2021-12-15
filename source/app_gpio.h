
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

//函数声明
int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_direction(int pin, int dir);
int gpio_write(int pin, int value);
int gpio_read(int pin);
int gpio_edge(int pin, int edge);