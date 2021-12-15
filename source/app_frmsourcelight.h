/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmsourcelight.h
* 摘    要：  app_frmsourcelight结构体定义
*
* 当前版本：  v1.1.0
* 作    者：  
* 完成日期：
*******************************************************************************/
#ifndef _APP_FRMSOURCELIGHT_H
#define _APP_FRMSOURCELIGHT_H

#define SOURCELIGHTWAVELENTH1310 0
#define SOURCELIGHTWAVELENTH1550 1

#define SOURCELIGHTMODECW       0
#define SOURCELIGHTMODE1KHZ     1
#define SOURCELIGHTMODE2KHZ     2
#define SOURCELIGHTMODE1KHZ_SHINE     5
#define SOURCELIGHTMODE2KHZ_SHINE     6

typedef struct _source_light_config
{
    int wavelength;
    int frequence;
}SOURCELIGHTCONFIG;


/****************定义保存稳定光源调整功率的频率配置******************/
typedef struct _light_source_power_config
{
    int wavelength;                //波长
    int pulse;                     //调整的稳定光源的频率，为了调整激光的功率 
}LIGHT_SOURCE_POWER_CONFIG;

typedef struct _light_source_power_config_list
{
  LIGHT_SOURCE_POWER_CONFIG  powerConfig[2];
}LIGHT_SOURCE_POWER_CONFIG_LIST;

#endif  //_APP_FRMSOURCELIGHT_H