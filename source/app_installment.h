/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_installment.h  
* 摘    要：  分期付款模块的数据结构和操作函数
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _APP_INSTALLMENT_H_
#define _APP_INSTALLMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

//加密标志
typedef enum _encryption_flag
{
    NoEncryption = 0,
    Encrypted = 1
}ENCRYPTION_FLAG;

//分期付款结构体
typedef struct _installment_
{
    char  		   cSerialNum[16];     	//序列号，固定是12位,详见序列号说明文档
    unsigned short usPeriodMonth[24];   //每期月数 1~12月,bMonth[0]:第一期月数...
    unsigned short usTotalPeriod;       //总共多少期
    unsigned char  ucCurrentPeriod;     //当前第几期
    unsigned char  ucEncryptedFlag;     //加密与否的标记
    unsigned long  ulUtcTime;           //当前期开始的秒数，秒为单位
    unsigned int   uiIsValid;			//标记是否有效
    unsigned char  ucReserved[32];		//保留区，用于扩展和页对齐
}INSTALLMENT, *PINSTALLMENT;

//设置OTDR主机序列号
void InstSetSerialNum(PINSTALLMENT inst, const char *serialNum);

//设置某期的使用月数
int InstSetPeriodMonth(PINSTALLMENT inst, int period, int month);

//获得加密标记
ENCRYPTION_FLAG InstEncryptedFlag(const PINSTALLMENT inst);

//设置加密标志
void InstSetEncryptedFlag(PINSTALLMENT inst, ENCRYPTION_FLAG flag);

//获得当前期数
int InstCurrentPeriod(const PINSTALLMENT inst);

//设置当前期数
int InstSetCurrentPeriod(PINSTALLMENT inst, int period);

//获得总期数
int InstTotalPeriods(const PINSTALLMENT inst);

//设置总期数
int InstSetTotalPeriod(PINSTALLMENT inst, int total);

//获得当前期的开始时间（s）
long InstUtcTime(const PINSTALLMENT inst);

//设置当前期的开始时间（s）
void InstSetUtcTime(PINSTALLMENT inst, int now, unsigned long utcTime);

//检测是否有效
int InstIsValid(PINSTALLMENT inst);

//memset
void InstMemset(PINSTALLMENT inst);

//加载到分期结构体对象
int InstLoad(PINSTALLMENT desInst, void *srcInst);

//分期结构提对象的size
int InstSize(const PINSTALLMENT inst);

//获得某期的授权许可证。
int InstGetLicense(const PINSTALLMENT inst, int period, char *license);

//校验许可证
int InstVerifyLicense(PINSTALLMENT inst, const char *license);

//判断当前时刻是否过期
int InstIsOutOfDate(const PINSTALLMENT inst, int now, unsigned long utcTime);

//当前期已过去的天数
int InstPassByDays(const PINSTALLMENT inst, int now, unsigned long utcTime);

//当前期还剩余的天数
int InstRemainDays(const PINSTALLMENT inst, int now, unsigned long utcTime);

//打印分期付款结构体
void InstPrint(const PINSTALLMENT inst);

#ifdef __cplusplus
}
#endif

#endif  //_APP_INSTALLMENT_H_
