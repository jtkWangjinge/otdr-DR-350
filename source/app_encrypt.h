/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_encrypt.h
* 摘    要：  提供加解密算法相关的函数
*             
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _APP_ENCRYPT_H_
#define _APP_ENCRYPT_H_

#include "app_middle.h"

#define ENCRYPTSDPATH	MntSDcardDataDirectory"/encrypt.lic"
#define DECODESDPATH	MntSDcardDataDirectory"/decode.lic"
#define ENCRYPTUSBPATH	MntUsbDirectory"/encrypt.lic"
#define DECODEUSBPATH	MntUsbDirectory"/decode.lic"

/* 定义F210中各个应用的编号 */
#define FIBERNUM	0
#define IOLMNUM		1


/* 加密文件使用固定的MASK，而解密文件使用根据序列号生成的MASK */
#define ENCRYPTMASK		0x12345678

#define  MASK_BIT0   0x01
#define  MASK_BIT1   0x02
#define  MASK_BIT2   0x04
#define  MASK_BIT3   0x08
#define  MASK_BIT4   0x10
#define  MASK_BIT5   0x20
#define  MASK_BIT6   0x40
#define  MASK_BIT7   0x80  

/***
  * 功能：
        从64位的序列号中抽取32位组成加解密的钥匙。
  * 参数：
        key_id：64位的序列号
  * 返回：
       32位的钥匙。		
  * 备注：
***/
unsigned int  get_key_mask(unsigned char *key_id);

/***
  * 功能：
        加解密数据。
  * 参数：
        mask：跟加密有关的掩码，取4个字节。
		dir：存放加密后的数据。
		src：待加密数据。
        len：数据长度。
  * 返回：
       无
  * 备注：
***/
void  encrypt_decrypt_data(unsigned int  mask, unsigned char *dir, unsigned char *src, unsigned char len);

/***
  * 功能：
        将BCD码数值转换为int数据类型
  * 参数：
		unsigned int lBcd:	BCD码
  * 返回：
       返回int类型的数值
  * 备注：
***/
int BcdToInt(unsigned int lBcd);

/***
  * 功能：
        将int数据类型转换为BCD码数值
  * 参数：
		int iInt:	int型数值
  * 返回：
       返回BCD码数值
  * 备注：
***/
unsigned int IntToBcd(int iInt);

/***
  * 功能：
        计算经过的天数
  * 参数：
		1、unsigned int Lastdate:	上次的年月日
		2、unsigned int curdate:	当前的年月日
  * 返回：
       返回BCD码数值
  * 备注：
***/
unsigned int CalDate(unsigned int Lastdate, unsigned int curdate);

/***
  * 功能：
        更新试用时间
  * 参数：
		1、unsigned int Lastdate:	上次的年月日
		2、unsigned int curdate:	当前的年月日
  * 返回：
       成功返回0，失败返回非零值
  * 备注：
***/
int CaleTrialTime(void);

/***
  * 功能：
        计算两次时间之间经过的秒数
  * 参数：
		1、RTCVAL *tmDes:	第一个时间
		2、RTCVAL *tmSrc:	第二个时间
  * 返回：
       返回两次时间之间经过的秒数
  * 备注：
***/
unsigned long long CalcTimeSlot(RTCVAL *tmDes, RTCVAL *tmSrc);

#endif
