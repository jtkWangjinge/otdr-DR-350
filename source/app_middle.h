/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_middle.h  ;
* 摘    要：  定义应用与设备驱动之间的中间层接口 ;
*
* 当前版本：  v1.0.0 ;
* 作    者：  
* 完成日期：
*
* 取代版本：;
* 原 作 者：;
* 完成日期：;
*******************************************************************************/

#ifndef _APP_MIDDLE_H
#define _APP_MIDDLE_H


/***********************************
* 为实现中间层而需要引用的标准头文件;
***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/***********************************
* 为实现中间层而需要引用的系统头文件;
***********************************/
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>


/***********************************
* 为实现中间层而需要引用的其他头文件;
***********************************/
#include "guibase.h"
#include "drv_fpga.h"
#include "drv_optic.h"

#include "app_otdr_algorithm_parameters.h"


/***************************
* 声明与FPGA相关的中间层函数;
***************************/
//FPGA程序初始化;
int Fpga_ProgInit(int iFpgaFd, char *strProgFile);

/***********************************
* 声明在光模块上EEPROM中的参数的偏移;
***********************************/
#define OPM_EEPROM_OFF_TUNNING		0		//整定参数在EEPROM中的偏移;
#define OPM_EEPROM_OFF_WAVECONFIG	0x80	//关于支持的工作波长的配置;

/***********************************
* 声明与光模块相关的结构及中间层函数;
***********************************/
#define OPM_WAVE_KIND       2   //波长个数;

#define OPM_MAX_AMP_KIND    13  //最大放大个数;

#define OPM_APD_KIND        4   //OPM APD 电压设置的种数;
#define OPM_VDD_KIND        4   //OPM VDD 电压设置的种数;
#define OPM_AMP_KIND		10	//OPM 放大


#define DAQ_BEGIN_OFFSET    10   //采样上来的跳空数据;

//added by  2016.7.14 光在真空中的速度
#define LIGHTSPEED_M_S		299792458.0f
#define LIGHTSPEED_1000KM_S	299.792458f


#define OPM_TUNING_PATH  MntSDcardDirectory"data/OpmTuning.txt"


//通道
// typedef enum _opm_amp 
// { 
	// ENUM_AMP0 = 0x11, //1.5k & 2.0 RF4
	// ENUM_AMP1 = 0x21, //1.5k & 5.7 RF4
	// ENUM_AMP2 = 0x15, //15k & 2.0  RF2
	// ENUM_AMP3 = 0x25, //15k & 5.7  RF2
	// ENUM_AMP4 = 0x13, //100k & 2.0 RF3
	// ENUM_AMP5 = 0x23, //100k & 5.7 RF3
	// ENUM_AMP6 = 0x17, //300k & 2.0 RF1
	// ENUM_AMP7 = 0x27, //300k & 5.7 RF1
	// ENUM_AMP8 = 0x10, //1.8M & 2.0
	// ENUM_AMP9 = 0x20  //1.8M & 5.7
// }OPM_AMP;

//通道（反向的后级放大顺序）
typedef enum _opm_amp 
{ 
  ENUM_AMP0 = 0x21, //1.5k & 2.0 RF4
  ENUM_AMP1 = 0x11, //1.5k & 5.7 RF4
  ENUM_AMP2 = 0x25, //15k & 2.0  RF2
  ENUM_AMP3 = 0x15, //15k & 5.7  RF2
  ENUM_AMP4 = 0x23, //100k & 2.0 RF3
  ENUM_AMP5 = 0x13, //100k & 5.7 RF3
  ENUM_AMP6 = 0x27, //300k & 2.0 RF1
  ENUM_AMP7 = 0x17, //300k & 5.7 RF1
  ENUM_AMP8 = 0x20, //1.8M & 2.0
  ENUM_AMP9 = 0x10  //1.8M & 5.7
}OPM_AMP;


//定义光模块中所支持的数据采集模式;
typedef enum _opm_mode
{
    ENUM_DAQ_MODE_IIR 		= DAQMODE_IIR,
    ENUM_DAQ_MODE_SINGLE	= DAQMODE_SINGLE,
    ENUM_DAQ_MODE_ACC		= DAQMODE_ACC,
    ENUM_DAQ_MODE_TEST		= DAQMODE_TEST
} OPM_DAQ_MODE;

//定义光模块中所支持的数据操作类型;
typedef enum _opm_operation
{
   ENUM_DAQ_CMD_STOP		= DAQ_CMD_STOP,
   ENUM_DAQ_CMD_START		= DAQ_CMD_START,
   ENUM_DAQ_CMD_READ		= DAQ_CMD_READ,
   ENUM_DAQ_CMD_WRITE		= DAQ_CMD_WRITE,
   ENUM_DAQ_CMD_PAUSE		= DAQ_CMD_PAUSE,
   ENUM_DAQ_CMD_RESUME		= DAQ_CMD_RESUME
} OPM_DAQ_CMD;

//定义用于索引光模块寄存器值的结构体;
typedef struct _opm_reg_index
{
    unsigned int uiAttenuateNum;    //该脉冲下，最大的放大档数;
    unsigned int *pAmpIndex;
    unsigned int *pApdIndex;        //用来存放APD的索引;
    unsigned int *pVddIndex;        //用来存放VDD的索引;
} OPM_REG_INDEX;

//定义用于设置光模块寄存器值的结构体;
typedef struct _opm_reg_ctrl
{
    unsigned int uiFpgaCtrlOut;     //FPGA_CTRLOUT_REG
    unsigned int uiAdcCount;        //FPGA_ADC_CNT_H，FPGA_ADC_CNT_L

} OPM_REG_CTRL;

typedef struct _opm_tuning  //光模块整定数据，保存在EEPROM中;在OTDR开机时读取;
{      
	unsigned int iVddValue[OPM_VDD_KIND];        //Vdd 的档位个数，从eeprom里
    unsigned int iApdVoltageSet[OPM_APD_KIND];   //0-3,共4个级别，每个级别相差2.5db
    unsigned int arrAttValue[OPM_AMP_KIND];
   
    float        fTemperatureSet;
    float        fTempCoefficient[OPM_APD_KIND];        //对应不同增益下的，温度曲线
    unsigned int OffsetPoint;        					//偏移点
} OPM_TUNING, *POPM_TUNING;

//////////////////////////////////////////以下是工厂配置参数表////////////////////////////////;
typedef struct _wave_config							//本结构，是工厂出厂时，通过PC机发命令，来设置OTDR;
{													//到底有几个工作波长;
   unsigned int iWaveActive[OPM_WAVE_KIND];			//0=本机器没有此波长，1=本机器有此波长的激光器;
} WAVE_CONFIG, *PWAVE_CONFIG;

//定义用于保存所有光模块参数的结构体;
typedef struct _opm_setting
{
    POPM_TUNING  pOpmTuning;                        //光模块整定值;

    OPM_REG_CTRL *pCurrReg;                         //当前的OpmRegCtrl值;
    OPM_REG_INDEX* RegIndex[OPM_PULSE_KIND];        //各个脉宽下的放大通道设置;
	unsigned int AmpCode[OPM_AMP_KIND];

    unsigned int DataBuff[1024];                    //用于保存实际数据;
	GUIMUTEX	mOPMSetMutex;					//用于互斥访问曲线显示参数
} OPMSET, *POPMSET;

/******************************************
* 声明功能板上EEPROM中保存的各参数的读写接口;
******************************************/
//保存数据到OPM板和主板EEPROM上指定的偏移;
int SaveDataToEeprom(int iEepromFd, void * pData, unsigned int iOff, unsigned int iSize);

//读取OPM板和主板EEPROM上指定的偏移的数据;
int ReadDataFromEeprom(int iEepromFd, void * pData, unsigned int iOff, unsigned int iSize);

/*
说明:;
	得到光模块上的整定数据;
返回:;
	0=成功，1=失败;
*/
int Opm_GetTuning(
POPM_TUNING  pOpmTunning  //光模块整定数据的指针;
);


//读取光模块寄存器;
int Opm_ReadReg(int iOpmFd, unsigned int uiRegAddr, unsigned int *pReadVal);
//写入光模块寄存器;
int Opm_WriteReg(int iOpmFd, unsigned int uiRegAddr, unsigned int *pWriteVal);

//得到光模块参数;
OPMSET* Opm_InitSet(void);

/*
说明:
	用来检查ADC是否正常工作?;
作者:;
	;
日期:;
	2013.10.19
返回:;
	0=非常，1=正常工作;
*/
int Opm_GetAdcStatue
(
int iOpmFd   // 光模块设备文件描述符;
);

/*
说明:;
	该函数只读取ADC的采样数据，不进行状态查询的,;
	数据长度在OPMSET的中指定，如果是移相插补，则需要8倍的长度;
*/
int Opm_ReadAdcData(
int	iOpmFd,				//FPGA的设备句柄;
OPMSET *pCurrOpm,		//OPM设置的数据结构;
UINT16 *pDataBuf		//采样;
);



/*
说明:;
	用来设置APD电压的;
*/
void Opm_SetApdVoltage
(
int iOpmFd,			//FPGA的设备句柄;
OPMSET *pCurrOpm,  	//OPM设置的数据结构;
UINT32 uiApdVoltage	//
);

/*
说明:;
	用来设置VDD电压的;
*/
void Opm_SetVddVoltage
(
int iOpmFd,			//FPGA的设备句柄;
OPMSET *pCurrOpm,	//OPM设置的数据结构;
UINT32 uiVddVoltage	//
);

/*
说明:;
	用来设置FrontAmp放大通道的;
*/
void Opm_SetAmp
(
int iOpmFd,					//FPGA的设备句柄;
OPMSET *pCurrOpm,         	//OPM设置的数据结构;
OPM_AMP EnumAmp	//前端放大;
);
/*
说明:
	用来得到后级放大档位的
备注:
	测试通过
*/
OPM_AMP Opm_GetAmp
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm				//OPM设置的数据结构
);

/*
DDR2的时钟打开后，必须先对它进行复位,只要写1就够了;
*/
void Opm_ResetFpgaDdr2(

int	iOpmFd	//FPGA的设备句柄;
);

/*
设置FPGA的CLOCK;
*/
void Opm_ClkTurnOn
(
int	iOpmFd,			//FPGA的设备句柄;
OPMSET *pCurrOpm	//OPM设置的数据结构;
);

/*
关闭FPGA的CLOCK;
*/
void Opm_ClkTurnOff
(
int iOpmFd,			//FPGA的设备句柄;
OPMSET *pCurrOpm	//OPM设置的数据结构;
);

/*
说明:
	设置LED的使能
备注:

*/
int Opm_SetLedEnable(OPMSET *pCurrOpm, int iEnable);
//获得固件版本号;
int Opm_GetFwVer(int iOpmFd);
//控制蜂鸣器;
int Opm_SetSpeaker(int iOpmFd, OPMSET *pCurrOpm, int iEnable);
//设置蜂鸣器时间;
int Opm_SetSpeakerTime(int iOpmFd, OPMSET *pCurrOpm, int iValue);
//设置蜂鸣器频率;
int Opm_SetSpeakerFreq(int iOpmFd, OPMSET *pCurrOpm, unsigned int freq);
//设置蜂鸣器占空比;
int Opm_SetSpeakerPWM(int iOpmFd, OPMSET *pCurrOpm, int iPWM);
//使能光模块电源;
int Opm_EnablePwr(int iOpmFd, OPMSET *pCurrOpm);
//关闭光模块电源;
int Opm_DisablePwr(int iOpmFd, OPMSET *pCurrOpm);
//检测光模块电源是否打开
int Opm_OpenPwr(OPMSET *pCurrOpm);
//使能正负5v电源控制
int Opm_EnablePwrCtl(int iOpmFd, OPMSET *pCurrOpm, unsigned int value);
//关闭正负5v电源控制
int Opm_DisablePwrCtl(int iOpmFd, OPMSET *pCurrOpm, unsigned int value);
//使能光模块ADC;
int Opm_EnableAdc(int iOpmFd, OPMSET *pCurrOpm);
//关闭光模块ADC;
int Opm_DisableAdc(int iOpmFd, OPMSET *pCurrOpm);
/***
  * 功能：
        使能光模块APD电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnableApdPower(int iOpmFd, OPMSET *pCurrOpm);
/***
  * 功能：
        关闭光模块APD电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisableApdPower(int iOpmFd, OPMSET *pCurrOpm);
/***
  * 功能：
        检测光模块APD电压设置是否打开
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        打开返回1，关闭返回0，失败返回-1
  * 备注：
  		测试通过
***/
int Opm_openApdPower(int iOpmFd, OPMSET *pCurrOpm);
/***
  * 功能：
        使能ADC 1.8v电源控制
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnableAdcVoltage(int iOpmFd, OPMSET *pCurrOpm);
/***
  * 功能：
        关闭adc 1.8V电源控制
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisableAdcVoltage(int iOpmFd, OPMSET *pCurrOpm);
    /***
  * 功能：
        使能光模块激光器电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnableLaserVoltage(int iOpmFd, OPMSET *pCurrOpm);
/***
  * 功能：
        关闭光模块激光器电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisableLaserVoltage(int iOpmFd, OPMSET *pCurrOpm);
/***
  * 功能：
        检测光模块激光器电压设置是否打开
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_openLaserVoltage(int iOpmFd, OPMSET *pCurrOpm);
//使能光模块激光器;
int Opm_LdPulseTurnOn(int iOpmFd, OPMSET *pCurrOpm);
//关闭光模块激光器;
int Opm_LdPulseTurnOff(int iOpmFd, OPMSET *pCurrOpm);
//检测光模块激光器是否打开
int Opm_OpenLdPulse(int iOpmFd, OPMSET *pCurrOpm);
// 1020
//该函数，用来关闭后级放大的信号
void Opm_TurnOffBackEnd(int	iOpmFd, OPMSET *pCurrOpm);
//该函数，用来打开后级放大的信号
void Opm_TurnOnBackEnd(int iOpmFd, OPMSET *pCurrOpm);
//end 

//得到当前光模块的波长;
OPM_WAVE Opm_GetWave(int iOpmFd, OPMSET *pCurrOpm);
//设置当前光模块的波长;
int Opm_SetWave(int iOpmFd, OPMSET *pCurrOpm, OPM_WAVE enuOpmWave);
//得到当前光模块的脉宽;
OPM_PULSE Opm_GetPulse(int iOpmFd, OPMSET *pCurrOpm);
//设置当前光模块的脉宽;
int Opm_SetPulse(int iOpmFd, OPMSET *pCurrOpm, OPM_PULSE enuOpmPulse);

//得到采样计数;
unsigned int Opm_GetCount(int iOpmFd, OPMSET *pCurrOpm);
//设置采样计数;
int Opm_SetCount(int iOpmFd, OPMSET *pCurrOpm, unsigned int uiSampleCount);
//得到采样延时;
unsigned int Opm_GetDelay(int iOpmFd, OPMSET *pCurrOpm);
//设置采样延时;
int Opm_SetDelay(int iOpmFd, OPMSET *pCurrOpm, unsigned int uiSampleDelay);
//得到采样间隔;
unsigned int Opm_GetGap(int iOpmFd, OPMSET *pCurrOpm);
//设置采样间隔;
int Opm_SetGap(int iOpmFd, OPMSET *pCurrOpm, unsigned int uiSampleGap);

//得到光模块当前的滤波参数;
OPM_FILTER Opm_GetFilter(int iOpmFd, OPMSET *pCurrOpm);
//设置光模块当前的滤波参数;
int Opm_SetFilter(int iOpmFd, OPMSET *pCurrOpm, OPM_FILTER enuFilterArg);
//得到采样模式
UINT32 Opm_GetDaqMode(
int iOpmFd, 
OPMSET *pCurrOpm
);

/*
说明:;
	用来设置FPGA_OPM_ATT0,FPGA_OPM_ATT1这两个寄存器的;
作者:;
*/
void Opm_SetAttValue
(
int	iOpmFd,			//FPGA的设备句柄;
OPMSET	*pCurrOpm,	//OPM设置的数据结构;
unsigned int uiAttValue          //设置值
);

//设置光模块当前的数据采集模式;
int Opm_SetMode(int iOpmFd, OPMSET *pCurrOpm, OPM_DAQ_MODE enuDaqMode);
/*
说明:
	用来设置采用移相插补，或者正常采集
*/
int Opm_SetIntp
(
int           iOpmFd,    //OPM的设备句柄
OPMSET *pCurrOpm, //OPM的设置指针
int          iIntpFlag    //1=用移相插补运算，0=不用移相插补
);
/*
说明:
	用来设置移相插补的相位移动值
*/
int Opm_SetPhase(int iOpmFd);
/*
说明:
	用来读取移相插补的相位移动值
*/
int Opm_readPhase(int iOpmFd);

//得到光模块当前的数据操作类型;
OPM_DAQ_CMD Opm_GetStatus(int iOpmFd, OPMSET *pCurrOpm);
//设置光模块当前的数据操作类型;
int Opm_SetDaqCmd(int iOpmFd, OPMSET *pCurrOpm, OPM_DAQ_CMD enuOperType);

//判断光模块的ADC是否已准备好;
int Opm_AdcReady(int iOpmFd);
//判断光模块的数据是否已准备好;
int Opm_DataReady(int iOpmFd);
//判断光模块的DAQ是否正忙;
int Opm_GetDaqStatus(int iOpmFd);

/*
说明:;
	用来设置FPGA_CTRLOUT_REG整个寄存器的;
	该寄存器，因为分散，所以干脆直接设置;
作者:;
	;
日期:;
	2013.10.19;
*/
void Opm_SetFpgaCtrlOut
(
int iOpmFd,			//FPGA的设备句柄;
OPMSET *pCurrOpm	//OPM设置的数据结构;
);
//得到FPGA_CTRLOUT_REG
void Opm_GetFpgaCtrlOut
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm				//OPM设置的数据结构
);
/*
功能说明：;
  该函数主要用来设置ADC采样参数的 ,会设置以下参数;
  1. ADC采样的个数                FPGA_ADC_CNT;
  2. ADC采样的Gap                 FPGA_DAQ_GAP;
  3. ADC采样延迟的个数            FPGA_ADC_DELAY_CNT;
  返回参数;;
  0=正常，1=失败;
*/
int SedAdcParameter(
int iOpmFd,		  			//FPGA的设备句柄;
OPMSET *pCurrOpm,			//OPM设置的数据结构;
UINT32 uiFiberLen,			//光纤的长度;
UINT32 gap
);

/*************************************
* 声明与电源管理相关的结构及中间层函数;
*************************************/
typedef struct _power
{
    unsigned char ucStateDc:1;	//占1个byte，电源适配器插入状态，1表示适配器插入，0表示未插入。（只读）;
    unsigned char ucStateBat:1;	//占1个byte，电池连接状态，1表示已连接，0表示未连接。只有当适配插入时才有效。（只读）;
    unsigned char ucStateFull:1;//占1个byte，充电状态，1表示充满电，0表示正在充电。只有当适配插入时才有效。（只读）;
    unsigned char ucStateType:2;//占2个byte，01代表933传ad值给9g45, 00代表933传%给9g45
    unsigned char ucStateLed:1; //led状态位
    unsigned char : 1;           //保留了1个byte;
    unsigned char ucStatePd : 1; //占1个byte，关机，1关闭主板电源，0无动作。（可写）;

    unsigned char ucPowerLevel; //电源电量;
} PWRINFO;

//得到电源信息;
PWRINFO* Pwr_GetInfo(int PwrFd);
//关闭电源;
int Pwr_ShutDown(void);
int Pwr_Charge(int enable);

/************************************
* 声明与RTC处理相关的结构及中间层函数;
************************************/
typedef struct rtc_time RTCVAL;     //rtc_time的定义在<linux/rtc.h>中;

//得到RTC时间;
RTCVAL* Rtc_GetTime(int iRtcFd);
//设置RTC时间;
int Rtc_SetTime(int iRtcFd, RTCVAL *pRtcVal);

/*
说明:;
	该函数根据枚举类型OPM_PULSE，来计算脉宽的clk的;
作者:;
	;
日期:;
	2013.10.19
*/
int CalPulseTime
(
OPM_PULSE enPulseTimeLen   //输入的枚举类型的脉冲长度;
);

/*
说明:;
	因为APD 击穿电压随温度改变而改变的原因，因此必须实时检测OPM 的温度;
返回:;
	温度，浮点数;
*/
int GetOpmTemperature
(
int iOpmFd,					//光模块设备的设备描述符;
float *pTempValue			//指向温度的指针变量;
);

//初始化Opm_init的时候对这些寄存器值初始化
int GetFpgaRegVal(OPMSET *pOpm);


//void CreatOffsetAdjust(UINT32 RefValue);

#endif  //_APP_MIDDLE_H

