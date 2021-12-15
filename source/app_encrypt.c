/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_encrypt.c
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
#include "app_encrypt.h"
#include "app_middle.h"
#include "app_global.h"
#include "app_parameter.h"
#include "app_getsetparameter.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//第0元素表示哪个字节（取值范围0--7），第1元素表示取哪位
static  unsigned char   c_key_mask[32][2] = 
{
    {0, MASK_BIT1},     //31
    {0, MASK_BIT7},     //30
    {0, MASK_BIT2},     //29
    {0, MASK_BIT4},     //28
    {1, MASK_BIT3},     //27
    {1, MASK_BIT6},     //26
    {1, MASK_BIT3},     //25
    {1, MASK_BIT0},     //24
    {2, MASK_BIT3},     //23
    {2, MASK_BIT2},     //22
    {2, MASK_BIT5},     //21
    {2, MASK_BIT7},     //20
    {3, MASK_BIT0},     //19
    {3, MASK_BIT4},     //18
    {3, MASK_BIT3},     //17
    {3, MASK_BIT7},     //16
    {4, MASK_BIT2},     //15
    {4, MASK_BIT1},     //14
    {4, MASK_BIT5},     //13
    {4, MASK_BIT3},     //12
    {5, MASK_BIT2},     //11
    {5, MASK_BIT6},     //10
    {5, MASK_BIT0},     //9
    {5, MASK_BIT1},     //8
    {6, MASK_BIT7},     //7
    {6, MASK_BIT2},     //6
    {6, MASK_BIT1},     //5
    {6, MASK_BIT4},     //4
    {7, MASK_BIT3},     //3
    {7, MASK_BIT2},     //2
    {7, MASK_BIT6},     //1
    {7, MASK_BIT5}      //0
};


/***
  * 功能：
        从64位的序列号中抽取32位组成加解密的钥匙。
  * 参数：
        key_id：64位的序列号
  * 返回：
       32位的钥匙。		
  * 备注：
***/
unsigned int  get_key_mask(unsigned char *key_id)
{
    unsigned char   i;
    unsigned int  ret = 0;

    for (i=0;i<32;i++)
    {
        ret <<= 1;
        if (key_id[c_key_mask[i][0]] & c_key_mask[i][1])
            ret |= 0x01;   
    } 

    return  ret;
}

static  unsigned char  c_code_0[32] = 
{
    0x12, 0x34, 0x56, 0x33, 0x4e, 0xf3, 0x21, 0xac,
    0x45, 0xdc, 0x1a, 0x32, 0x36, 0x0f, 0x12, 0x42,
    0x54, 0xd3, 0xaf, 0x23, 0x69, 0xdf, 0xbc, 0x31,
    0x86, 0xcd, 0xb4, 0xbe, 0x8c, 0x94, 0x73, 0x29    
};
static   unsigned char  c_code_1[32] = 
{
    0x21, 0xfd, 0x36, 0x11, 0xdc, 0xbb, 0x37, 0xbf,
    0xaa, 0x56, 0x29, 0x70, 0x66, 0x2c, 0x78, 0x69,
    0xda, 0xc2, 0xc7, 0xb8, 0x6d, 0x4a, 0x3b, 0xc6,
    0xca, 0xad, 0xb9, 0xa0, 0xc5, 0x2d, 0x8b, 0xf2  
};   


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
void  encrypt_decrypt_data(unsigned int  mask, unsigned char *dir, unsigned char *src, unsigned char len)
{
    unsigned char   i, j;
    unsigned int   tmp;  

    i = 0;
    j = 0;
    tmp = mask;
    while(j < len)
    {
        if (tmp & 0x80000000)
            *dir = *src ^ c_code_1[i];
        else
            *dir = *src ^ c_code_0[i];

        src++;
        dir++;
        tmp <<= 1;  
        i++;
        if ((i % 32) == 0)      //若数据长度超过32个字节
        {
            i = 0;
            tmp = mask;
        }
        j++;
    } 
}

/***
  * 功能：
        将BCD码数值转换为int数据类型
  * 参数：
		unsigned int lBcd:	BCD码
  * 返回：
       返回int类型的数值
  * 备注：
***/
int BcdToInt(unsigned int lBcd)
{
    int iResult = 0;

    if (lBcd > 0 && lBcd <= 0x99999999)
    {
        if (lBcd >= 0x10000000 && lBcd <= 0x99999999)
        {
            iResult += (lBcd >> 28) * 10000000;
            lBcd %= 0x10000000;
        }
        if (lBcd >= 0x1000000 && lBcd <= 0x9999999)
        {
            iResult += (lBcd >> 24) * 1000000;
            lBcd %= 0x1000000;
        }
        if (lBcd >= 0x100000 && lBcd <= 0x999999)
        {
            iResult += (lBcd >> 20) * 100000;
            lBcd %= 0x100000;
        }
        if (lBcd >= 0x10000 && lBcd <= 0x99999)
        {
            iResult += (lBcd >> 16) * 10000;
            lBcd %= 0x10000;
        }
        if (lBcd >= 0x1000 && lBcd <= 0x9999)
        {
            iResult += (lBcd >> 12) * 1000;
            lBcd %= 0x1000;
        }
        if (lBcd >= 0x100 && lBcd <= 0x999)
        {
            iResult += (lBcd >> 8) * 100;
            lBcd %= 0x100;
        }
        if (lBcd >= 0x10 && lBcd <= 0x99)
        {
            iResult += (lBcd >> 4) * 10;
            lBcd %= 0x10;
        }
        if (lBcd >= 0x1 && lBcd <= 0x9)
        {
            iResult += lBcd;
        }
    }

    return iResult;
}

/***
  * 功能：
        将int数据类型转换为BCD码数值
  * 参数：
		int iInt:	int型数值
  * 返回：
       返回BCD码数值
  * 备注：
***/
unsigned int IntToBcd(int iInt)
{
    unsigned int lResult = 0;

    if (iInt > 0 && iInt <= 99999999)
    {
        if (iInt >= 10000000 && iInt <= 99999999)
        {
            lResult += (iInt / 10000000) << 28;
            iInt %= 10000000;
        }
        if (iInt >= 1000000 && iInt <= 9999999)
        {
            lResult += (iInt / 1000000) << 24;
            iInt %= 1000000;
        }
        if (iInt >= 100000 && iInt <= 999999)
        {
            lResult += (iInt / 100000) << 20;
            iInt %= 100000;
        }
        if (iInt >= 10000 && iInt <= 99999)
        {
            lResult += (iInt / 10000) << 16;
            iInt %= 10000;
        }
        if (iInt >= 1000 && iInt <= 9999)
        {
            lResult += (iInt / 1000) << 12;
            iInt %= 1000;
        }
        if (iInt >= 100 && iInt <= 999)
        {
            lResult += (iInt / 100) << 8;
            iInt %= 100;
        }
        if (iInt >= 10 && iInt <= 99)
        {
            lResult += (iInt / 10) << 4;
            iInt %= 10;
        }
        if (iInt >= 1 && iInt <= 9)
        {
            lResult += iInt;
        }
    }

    return lResult;
}

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
unsigned int CalDate(unsigned int Lastdate, unsigned int curdate)
{
    unsigned int cur, last;

    if((Lastdate & 0x7c0) != (curdate & 0x7c0))					//month & date
    {
        last = (((Lastdate & 0x7c0)>>6) & 0x10) ? 300 : 0;
        last += (((Lastdate & 0x7c0)>>6) & 0x0f) * 30;
        last +=((Lastdate & 0x30)>>4) * 10;
        last +=(Lastdate & 0x0f);

        cur = (((curdate & 0x7c0)>>6) & 0x10) ? 300 : 0;
        cur += (((curdate & 0x7c0)>>6) & 0x0f) * 30;
        cur +=((curdate & 0x30)>>4) * 10;
        cur +=(curdate & 0x0f);
    }
    else
    {
        last =((Lastdate & 0x30)>>4) * 10;
        last +=(Lastdate & 0x0f);

        cur =((curdate & 0x30)>>4) * 10;
        cur +=(curdate & 0x0f);

    }

    if((Lastdate & 0xfffff800) == (curdate &0xfffff800))		//year
    {
        return (cur - last);
    }
    else if(((Lastdate & 0xfffff800) + 0x800) == (curdate &0xfffff800))
    {
        return (365 - last + cur);
    }
    else
        return 0xffffffff;
}

/***
  * 功能：
        计算从1900/01/01 00:00:00到Time所经历的分钟数
  * 参数：
		RTCVAL *Time:	现在的时间
  * 返回：
       返回从1900/01/01 00:00:00到Time所经历的分钟数
  * 备注：
  		
***/
static unsigned long long ConversionSeconds(RTCVAL *Time)
{
	unsigned long long ullSeconds;

	if(Time->tm_year>2038 || Time->tm_year<1900)
	{
		ullSeconds = 0;
	}
	else
	{
		ullSeconds = (Time->tm_year-1900)*365*24*60;
		ullSeconds += Time->tm_mday*24*60;
		ullSeconds += Time->tm_hour*60;
		ullSeconds += Time->tm_min;
		//ullSeconds += Time->tm_sec;
		switch (Time->tm_mon)
		{
			case 1:     
			case 3:     
			case 5:   
			case 7:   
			case 8:
			case 10:
			case 12:
				ullSeconds += Time->tm_mon*31*24*60;
			    break;
			case 4:     
			case 6:     
			case 9:   
			case 11:   
				ullSeconds += Time->tm_mon*30*24*60;
			    break;
			case 2:    
				if ( (Time->tm_year%4 == 0 && Time->tm_year%100 != 0) || 
					 (Time->tm_year%400 == 0) ) 
			    {
					ullSeconds += Time->tm_mon*29*24*60;
			    }
				else
				{
					ullSeconds += Time->tm_mon*28*24*60;			
				}
			    break;
			default:
			    break;
		}
	}

	return ullSeconds;
}

/***
  * 功能：
        计算两次时间之间经过的分钟数
  * 参数：
		1、RTCVAL *tmDes:	第一个时间
		2、RTCVAL *tmSrc:	第二个时间
  * 返回：
       返回两次时间之间经过的分钟数
  * 备注：
***/
unsigned long long CalcTimeSlot(RTCVAL *tmDes, RTCVAL *tmSrc)
{
	unsigned long long ullSeconds;
	unsigned long long ullSeconds1;
	unsigned long long ullSeconds2;

	ullSeconds1 = ConversionSeconds(tmDes);
	ullSeconds2 = ConversionSeconds(tmSrc);

	if(ullSeconds1 >= ullSeconds2)
	{
		ullSeconds = ullSeconds1 - ullSeconds2;
	}
	else
	{
		ullSeconds = ullSeconds2 - ullSeconds1;
	}

	return ullSeconds;
}

