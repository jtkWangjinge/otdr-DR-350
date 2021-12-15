/*******************************************************************************
* Copyright(c)2014，大豪信息技术(威海)有限公司
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

#include "app_eeprom.h"

#include "app_global.h"
#include "app_parameter.h"

#include <sys/time.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*********************************************************************************************************
* 函数声明
*********************************************************************************************************/


#define I2C_SLAVE 		0x0703
#define I2C_RDWR 		0x0707
#define I2C_EEPROM_WP 	0x0709


static int 				iE2prom = -1;			//定义静态全局变量，保存EEPROM设备文件的设备描述符
static struct i2c_rdwr_ioctl_data EepromData;	//定义静态全局变量，保存IIC通讯单元

/*
**函数说明:
	EEPROM忙查询,
	EEPROM数据写入操作后，后续所有操作前都要调用该函数进行查询
	直到设备就绪或时间溢出
**参数说明:
	无
**返回值:
	ret < 0 ----设备忙
	其它----就绪
*/
int E2prom_polling(void)
{
	int ret;
	unsigned char Buff;
	
	//参数检查
	if(iE2prom == -1)
	{
		return -1;
	}

	EepromData.nmsgs = 1;
	EepromData.msgs[0].len = 1;
	EepromData.msgs[0].addr = EEPROMADDR;
	EepromData.msgs[0].flags = I2C_M_RD;				//read
	EepromData.msgs[0].buf = &Buff;

	//写保护
	ioctl(iE2prom, I2C_SLAVE, EEPROMADDR);
	
	//数据传输
	ret = ioctl(iE2prom, I2C_RDWR, (unsigned long)&EepromData);	
	
	return ret;
}


/*
**函数说明:
	EEPROM忙查询
**参数说明:
	无
**返回值:
	成功返回0
	失败返回-1
*/
int Poll_E2prom(void)
{
	int ret, retries=20;

	do
	{
		ret = E2prom_polling();
	}while((ret<0) && (retries-->0));
	
	if((ret<0) && (retries<=0))
	{
		return -1;
	}

	return 0;
}

/*
**函数说明:
	EEPROM数据页写函数
**参数说明:
	unsigned int pagenum: 待写入页地址号
	void *buff: 数据缓冲区，指向128byte的缓冲区
**返回值:
	<0:		失败
	其它:	成功
*/
int E2prom_write_page(unsigned short pagenum, unsigned char  *buff)
{
	int ret;
	unsigned char *pBuff;
	
	//参数检查
	if((pagenum >= PAGECOUNT) || (!buff))
	{
		CODER_LOG(CoderGu, "E2prom_write_page check\n");
		return -1;
	}
	if(iE2prom == -1)
	{
		CODER_LOG(CoderGu, "E2prom_write_page check\n");
		return -1;
	}

              
	//分配缓冲区
	pBuff = (unsigned char *)GuiMemAlloc(PAGESIZE + 2);
	if(!pBuff)
	{
		CODER_LOG(CoderGu, "E2prom_write_page check\n");
		return -1;
	}

	//拷贝数据，设置地址
	pBuff[0] = (unsigned char)(pagenum>>1);	//high address
	pBuff[1] = (unsigned char)(pagenum<<7);	//low address
	memcpy(&pBuff[2], buff, PAGESIZE);

	EepromData.nmsgs = 1;
	EepromData.msgs[0].len = PAGESIZE + 2;
	EepromData.msgs[0].addr = EEPROMADDR;
	EepromData.msgs[0].flags = 0;				//write
	EepromData.msgs[0].buf = pBuff;

	//写保护
	ioctl(iE2prom, I2C_SLAVE, EEPROMADDR);
	ret = ioctl(iE2prom, I2C_EEPROM_WP, 0);
	
	//数据传输
	ret = ioctl(iE2prom, I2C_RDWR, (unsigned long)&EepromData);
	if(ret < 0)
	{
	}
	
	//写保护
	ioctl(iE2prom, I2C_EEPROM_WP, 1);
	
	//释放内存
	GuiMemFree(pBuff);
	return ret;
}


/*
**函数说明:
	EEPROM数据页读函数
**参数说明:
	unsigned int pagenum: 待写入页号
	void * buff: 待写入数据缓冲区，指向128byte的缓冲
**返回值:
	ret < 0: 失败
	其它:成功
*/
int E2prom_read_page(unsigned short pagenum, unsigned char *buff)
{
	int ret;
	unsigned char *pBuff;
	
	//参数检查
	if((pagenum >= PAGECOUNT) || (!buff))
	{
		CODER_LOG(CoderGu, "iRet\n");
		return -1;
	}
	if(iE2prom == -1)
	{
		CODER_LOG(CoderGu, "iRet\n");
		return -1;
	}


	//分配内存，设置参数
	pBuff=(unsigned char*)GuiMemAlloc(2);
	pBuff[0] = (unsigned char)(pagenum>>1);	//high address
	pBuff[1] = (unsigned char)(pagenum<<7);	//low address
	
	EepromData.nmsgs = 2;
	EepromData.msgs[0].len = 2;
	EepromData.msgs[0].addr = EEPROMADDR;
	EepromData.msgs[0].flags = 0;				//write
	EepromData.msgs[0].buf = pBuff;		
	EepromData.msgs[1].len = PAGESIZE;
	EepromData.msgs[1].addr = EEPROMADDR;
	EepromData.msgs[1].flags = I2C_M_RD;		//read
	EepromData.msgs[1].buf = buff;		

	//数据传输
	ret = ioctl(iE2prom, I2C_RDWR, (unsigned long)&EepromData);
	if(ret<0)
	{
	}

	//释放内存
	GuiMemFree(pBuff);
	return ret;
}

/*
**函数说明:
	EEPROM数据页写检查函数
**参数说明:
	unsigned int pagenum: 待写入页地址号
	void *buff: 数据缓冲区，指向128byte的缓冲区
**返回值:
	成功		---- 	0
	页错误	----	-EPAGEFAULT
	设备忙	----	-EDEVBUSY
*/
int E2prom_write_check(unsigned short pagenum, unsigned char  *pBuff)
{
	int ret, retries;
	PAGE temp;

	//polling
	if(Poll_E2prom())
	{
		return -EDEVBUSY;
	}
	
	//page wirte
	retries = 10;
	do{
		ret = E2prom_write_page(pagenum, pBuff);
		retries--;
	}while((ret<0) && (retries>0));
	if((ret<0) && (retries<=0)){
		return -EDEVBUSY;
	}	

	//polling
	if(Poll_E2prom())
	{
		return -EDEVBUSY;
	}

	//read back
	retries = 10;
	do{
		ret = E2prom_read_page(pagenum, (unsigned char*)&temp);
		retries--;
	}while((ret<0) &&(retries>0));
	if((ret<0) && (retries <= 0)){
		return -EDEVBUSY;
	}

	//data check
	ret = memcmp(pBuff, &temp, PAGESIZE);

	if(!ret){
		return 0;
	}else{
		return -EPAGEFAULT;
	}
}

/*
**函数说明:
	关闭EEPROM设备，并释放由E2prom_open函数分配的内存空间
**参数说明:
	无
**返回值:
	0	--成功
	-1	--失败
*/
int E2prom_close(void)
{
	int ret;

	ret = close(iE2prom);
	iE2prom = -1;
	GuiMemFree(EepromData.msgs);
	
	return ret;
}

/*
**函数说明:
	打开EEPROM设备，并分配IIC通讯单元内存
**参数说明:
	无
**返回值:
	成功--返回EEPROM设备的文件描述符
	失败--返回负数
*/
int E2prom_open(void)
{
	if(iE2prom == -1)
	{
		iE2prom=open(EEPROMFILE, O_RDWR);

		if(iE2prom<0)
		{
        	perror("open /dev/eeprom:");
		}
		else
		{
			EepromData.nmsgs = 2;
			EepromData.msgs = (struct i2c_msg*)GuiMemAlloc(EepromData.nmsgs * sizeof(struct i2c_msg));
			if(!EepromData.msgs)
			{
				E2prom_close();
			}
		}
		return iE2prom;
	}
	else
	{
		LOG(LOG_ERROR, "Eeprom busy..\n");
		return -1;
	}
}

/*********************************************************************************************************
* End File
*********************************************************************************************************/
