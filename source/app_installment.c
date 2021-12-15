/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_installment.c  
* 摘    要：  定义应用程序中的公共变量、公共操作函数
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "app_installment.h"

#include "app_global.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#define INSTALLMENT_DEBUG 0

const int SerialNumLen = 12;    //序列号总共12位
const int LicenseGrpNum = 8;    //许可证分8组
const int LicenseGrpLen = 3;    //许可证每组数字3位
const int DecryptNum = 31;      //取这个来做彻底解密的掩码
const int PeriodMin = 1;        //期数最小值
const int PeriodMax = 12;       //期数最大值

//最大支持31个密码，第32个就开始和0重复了
static  unsigned char c_code_0[32] =
{
    0x12, 0x34, 0x56, 0x33, 0x4e, 0xf3, 0x21, 0xac,
    0x45, 0xdc, 0x1a, 0x32, 0x36, 0x0f, 0x12, 0x42,
    0x54, 0xd3, 0xaf, 0x23, 0x69, 0xdf, 0xbc, 0x31,
    0x86, 0xcd, 0xb4, 0xbe, 0x8c, 0x94, 0x73, 0x29
};


/***
  * 功能：
        设置OTDR主机序列号
  * 参数：
        1、PINSTALLMENT installment:	 inst
        2、char *serialNum：OTDR主机序列号
  * 返回：
  * 备注：
***/
void InstSetSerialNum(PINSTALLMENT inst, const char *serialNum)
{
	memcpy(inst->cSerialNum, serialNum, SerialNumLen);
    inst->cSerialNum[SerialNumLen] = '\0';
}

/***
  * 功能：
        设置某期的使用月数
  * 参数：
        1、PINSTALLMENT installment:	 inst
        2、int period：期数 范围1~12
        3、int month：月数 范围1~12
  * 返回：
       0:成功，非0：期数或月 参数不符合要求
  * 备注：
***/
int InstSetPeriodMonth(PINSTALLMENT inst, int period, int month)
{
    //判断，正好月数也是最小值为1，最大值位12
    if(period < PeriodMin || period > PeriodMax
       || month < PeriodMin || month > PeriodMax)
    {
        return -1;
    }
    inst->usPeriodMonth[period] = month;
    return 0;
}


/***
  * 功能：
        获取加密标记
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
  * 返回：
       Encrypted：加密状态， NoEncryption：非加密状态
  * 备注：
***/
ENCRYPTION_FLAG InstEncryptedFlag(const PINSTALLMENT inst)
{
    return inst->ucEncryptedFlag;
}

/***
  * 功能：
        设置加密标记
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
        2、ENCRYPTION_FLAG flag：加密标志
  * 返回：
  * 备注：
***/
void InstSetEncryptedFlag(PINSTALLMENT inst, ENCRYPTION_FLAG flag)
{
    inst->ucEncryptedFlag = flag;
}


/***
  * 功能：
        返回分期付款当前是第几期
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
  * 返回：
       当前第几期数
  * 备注：
***/
int InstCurrentPeriod(const PINSTALLMENT installment)
{
    return installment->ucCurrentPeriod;
}

/***
  * 功能：
        设置当前是第几期
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
        2、int period: 期数 范围：1~12， 0是无效的
  * 返回：
       0: 成功， 非0：失败
  * 备注：
***/
int InstSetCurrentPeriod(PINSTALLMENT inst, int period)
{
    if(period < PeriodMin-1 || period > PeriodMax)
    {
        return -1;
    }
    inst->ucCurrentPeriod = period;
    return 0;
}


/***
  * 功能：
        返回分期付款的期数
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
  * 返回：
       期数
  * 备注：
***/
int InstTotalPeriods(const PINSTALLMENT installment)
{
    return installment->usTotalPeriod;
}

/***
  * 功能：
        返回分期付款的期数
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
        2、short total：设置总的期数 范围：1~12
  * 返回：
        0: 成功，非0：失败
  * 备注：
***/
int InstSetTotalPeriod(PINSTALLMENT inst, int total)
{
    if(total < PeriodMin+1 || total > PeriodMax)
    {
        return -1;
    }
    inst->usTotalPeriod = total;
    return 0;
}

/***
  * 功能：
        返回分期付款的期数
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
  * 返回：
        当前期的其实时刻的UTC秒数
  * 备注：
***/
long InstUtcTime(const PINSTALLMENT inst)
{
    return inst->ulUtcTime;
}

/***
  * 功能：
        返回分期付款的期数
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
        2、int now：设置当前时间的标记？
        3、unsigned long utcTime：UTC时间，秒数
  * 返回：
  * 备注：
***/
void InstSetUtcTime(PINSTALLMENT inst, int now, unsigned long utcTime)
{
	if (now)
	{
		time_t timep;
    	time(&timep);
		inst->ulUtcTime = (unsigned long)timep;
	}
	else
	{
		inst->ulUtcTime = utcTime;
	}
}

/***
  * 功能：
        检测结构体是否有效
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
  * 返回：
  * 备注：有效返回0， 无效返回-1
***/
int InstIsValid(PINSTALLMENT inst)
{
	if (inst->uiIsValid == 0x12345678)
	{
		return 0;
	}
	return -1;
}


/***
  * 功能：
        加载分期付款结构对象
  * 参数：
        1、PINSTALLMENT desInst:	 分期付款结构体对象指针
        2、void *srcInst:	源内容
  * 返回：
       0：成功， 非0失败
  * 备注：
***/
void InstMemset(PINSTALLMENT inst)
{
	memset(inst, 0, sizeof(INSTALLMENT));
	inst->uiIsValid = 0x12345678;//magic
}


/***
  * 功能：
        加载分期付款结构对象
  * 参数：
        1、PINSTALLMENT desInst:	 分期付款结构体对象指针
        2、void *srcInst:	源内容
  * 返回：
       0：成功， 非0失败
  * 备注：
***/
int InstLoad(PINSTALLMENT desInst, void *srcInst)
{
    memcpy(desInst, srcInst, sizeof(*desInst));
    return 0;
}


/***
  * 功能：
        返回分期付款结构体size
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
  * 返回：
       对象的size
  * 备注：
***/
int InstSize(const PINSTALLMENT installment)
{
    return sizeof(*installment);
}

/***
  * 功能：
        获得加密序列号
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
        2、int period: 掩码，其实就对应的第N期。1~12，对应1~12的秘钥，31是解密的秘钥。
        3、char *license：生成的秘钥。秘钥是24位，3*8，提前申请好大于24字节的内存。
  * 返回：
       0：加密成功， 非0：失败
  * 备注：
***/
int InstGetLicense(const PINSTALLMENT inst, int period, char *license)
{
    if(period < PeriodMin || (period > PeriodMax && period != DecryptNum))
    {
        return -1;
    }

    int i = 0;
    unsigned int sum = 0;
    char buf[8] = {0};
    unsigned char temp = 0;

    for (i = 0; i < SerialNumLen; ++i) //12位机器序列号
    {
        sum += inst->cSerialNum[i];
    }

    for (i = 0; i < LicenseGrpNum; ++i) //8组
    {
        period = (period + i) % 32;
        temp = sum * c_code_0[period] & 0xFF;
#ifdef WIN32
        sprintf_s(buf, 4,"%03d", temp);
#else
        snprintf(buf, 4, "%03d", temp);
#endif
        memcpy(license + LicenseGrpLen*i, buf, LicenseGrpLen);
    }
    return 0;
}

/***
  * 功能：
        授权或者解密,只根据当前期来做授权（特殊情况下是解密）
  * 参数：
        1、PINSTALLMENT installment:	 分期付款结构体对象
        2、char *license: 许可证字符串
  * 返回：
       1：解密成功； 0 : 授权成功；非0：失败
  * 备注：解密成功之后会修改加密标志
***/
int InstVerifyLicense(PINSTALLMENT inst, const char *license)
{
    char buf[28] = {0};

    //判断解密
    InstGetLicense(inst, DecryptNum, buf);
    LOG(LOG_INFO, "-----1---licensee = %s, buf = %s\n", license, buf);
    if(!memcmp(buf, license, LicenseGrpNum * LicenseGrpLen))
    {
        //解密成功，将设置处于非加密状态
        inst->ucEncryptedFlag = NoEncryption;
        inst->ulUtcTime = 0;
        inst->usTotalPeriod = 0;
        return 1;
    }

    //判断授权
    InstGetLicense(inst,inst->ucCurrentPeriod+1, buf);
    LOG(LOG_INFO, "----2---licensee = %s, buf = %s\n", license, buf);
    if(!memcmp(buf, license, LicenseGrpNum * LicenseGrpLen))
    {
        //授权成功
        return 0;
    }
    return -1;
}

/***
  * 功能：
        检查传入时间是否已经过期
  * 参数：
        1、PINSTALLMENT inst:	 分期付款结构体对象
        2、int now：	当前时刻？不是当前时刻则按第三个参数来计算
        3、unsigned long utcTime: UTC时间
  * 返回：
       0：不过期， 非0：过期
  * 备注：
***/
int InstIsOutOfDate(const PINSTALLMENT inst, int now, unsigned long utcTime)
{
	unsigned long utc = 0;
	if (now)
	{
		time_t timep;
    	time(&timep);
		utc = (unsigned long)timep;
	}
	else
	{
		utc = utcTime;
	}

	//求出时间差
    int diffDays = (utc - inst->ulUtcTime) / 24 / 3600;
    if (0 == inst->ucCurrentPeriod || diffDays < 0
		|| diffDays >= inst->usPeriodMonth[inst->ucCurrentPeriod]*30)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


/***
  * 功能：
        返回分期付款当前期已经经历过多少天
  * 参数：
        1、PINSTALLMENT inst:	 分期付款结构体对象
        2、int now：	当前时刻？不是当前时刻则按第三个参数来计算
        3、unsigned long utcTime: UTC时间
  * 返回：
       当前期已经过了多少天
  * 备注：
***/
int InstPassByDays(const PINSTALLMENT inst, int now, unsigned long utcTime)
{
	unsigned long utc = 0;
	if (now)
	{
		time_t timep;
    	time(&timep);
		utc = (unsigned long)timep;
	}
	else
	{
		utc = utcTime;
	}
    //求出时间差
    int diffDays = (utc - inst->ulUtcTime) / 24 / 3600;
	if (diffDays < 0)
		diffDays = 0;
	
    return diffDays;
}

/***
  * 功能：
        返回分期付款当前期剩余多少天
  * 参数：
        1、PINSTALLMENT inst:	 分期付款结构体对象
        2、int now：	当前时刻？不是当前时刻则按第三个参数来计算
        3、unsigned long utcTime: UTC时间
  * 返回：
       当前期剩余天数
  * 备注：
***/
int InstRemainDays(const PINSTALLMENT inst, int now, unsigned long utcTime)
{
	unsigned long utc = 0;
	if (now)
	{
		time_t timep;
    	time(&timep);
		utc = (unsigned long)timep;
	}
	else
	{
		utc = utcTime;
	}
	
    //求出时间差
    int diffDays = (utc - inst->ulUtcTime) / 24 / 3600;
    int currPeriodMonth = inst->usPeriodMonth[inst->ucCurrentPeriod];
	int remainDays = currPeriodMonth*30 - diffDays;
	if (remainDays < 0)
	{
		remainDays = 0;
	}
	return remainDays;
}


/***
  * 功能：
        打印分期付款结构体信息
  * 参数：
        1、PINSTALLMENT inst:	 分期付款结构体对象
  * 返回：
       
  * 备注：
***/
void InstPrint(const PINSTALLMENT inst)
{
	if (inst && INSTALLMENT_DEBUG)
	{
        LOG(LOG_INFO, "installment info ---------start---------\n");
        LOG(LOG_INFO, "serial num: %s\n", inst->cSerialNum);
        LOG(LOG_INFO, "magic num: %x\n", inst->uiIsValid);
        LOG(LOG_INFO, "encrypt flag: %d\n", inst->ucEncryptedFlag);
        LOG(LOG_INFO, "total periods: %d\n", inst->usTotalPeriod);
        LOG(LOG_INFO, "current period: %d\n", inst->ucCurrentPeriod);
        LOG(LOG_INFO, "utc time: %ld\n", inst->ulUtcTime);
        int i = 0;
		for(i = 0; i < PeriodMax; ++i)
		{
            LOG(LOG_INFO, "period: %d , month: %d\n", i + 1, inst->usPeriodMonth[i + 1]);
        }

        LOG(LOG_INFO, "installment info ----------end----------\n");
    }
}

