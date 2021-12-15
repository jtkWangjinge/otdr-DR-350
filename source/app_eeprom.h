/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  eeprom.h
* 摘    要：  实现eeprom按页读写的功能
*             
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*******************************************************************************/
#ifndef _APP_EEPROM_H_
#define _APP_EEPROM_H_

#include <stdio.h>  
#include <linux/types.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/ioctl.h>  
#include <errno.h>  
#include <assert.h>  
#include <string.h>  
#include <linux/i2c.h>  
#include <linux/i2c-dev.h> 

#define EEPROMFILE	"/dev/i2c-1"

#define EEPROMADDR	0x50

#define EDEVBUSY 	1		//设备无响应
#define EPAGEFAULT 	2		//页校验错误

#define OPMBOARDEEPROM		0
#define MAINBOARDEEPROM		1

/*********************************************************************************************************
* 函数声明
*********************************************************************************************************/
/*
**函数说明:
	EEPROM数据页写函数
**参数说明:
	unsigned int pagenum: 	待写入页地址号
	pbuff				: 	数据缓冲区，指向128byte的缓冲区
**返回值:
	其它:		失败
	0:			成功
*/
int E2prom_write_page(unsigned short uspagenum, unsigned char *pbuff);

/*
**函数说明:
	EEPROM数据页读函数
**参数说明:
	unsigned int pagenum: 	待读出页地址号
	pbuff				: 	数据缓冲区，指向128byte的缓冲区
**返回值:
	其它:		失败
	0:			成功
*/
int E2prom_read_page(unsigned short uspagenum, unsigned char *pbuff);

/*
**函数说明:
	EEPROM数据页写检查函数
**参数说明:
	unsigned short pagenum: 待写入页地址号
	pbuff				: 	数据缓冲区，指向128byte的缓冲区
**返回值:
	成功	---- 	0
	页错误	----	-EPAGEFAULT
	设备忙	----	-EDEVBUSY
*/
int E2prom_write_check(unsigned short uspagenum, unsigned char *pBuff);

/*
**函数说明:
	EEPROM忙查询
**参数说明:
	无
**返回值:
	成功返回0
	失败返回-1
*/
int Poll_E2prom(void);

/*
**函数说明:
	打开EEPROM设备，并分配IIC通讯单元内存
**参数说明:
	无
**返回值:
	成功--返回EEPROM设备的文件描述符
	失败--返回负数
*/
int E2prom_open(void);

#endif
