/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  parameter.h
* 摘    要：  定义参数保存相关的总体的结构体
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
#ifndef _APP_PARAMETER_H_
#define _APP_PARAMETER_H_

/* 包含其他文件中要保存的结构体 */
#include "app_curve.h"
#include "wnd_global.h"
#include "app_middle.h"
#include "app_frmotdr.h"


#define APPNUM			8				// 系统中需要加密的应用数
#define PRODUCTLOCK 	0xa5a5a5a5		// 产品加密状态

#define DEFAULTSYSTEMPARAMETER	"/etc/DefaultSystemPara.bin"		//默认系统参数文件名称
#define MODULEBACKPATH			"/etc/module.bin"					//模块信息备份文件


#define BUFFERVALID				0xA5A5		// 缓冲区有效标志	

#define PAGECOUNT				65			// 当前系统占用Eeprom总的页数
#define PAGESIZE 				128			// Eeprom中每页的字节数

// 各参数区在Eeprom中的页地址
#define SYSTEMPARPAGEADDR       0			// 系统参数区页基地址
#define BUFFPAGEADDR			29			// 缓冲区页基地址
#define MODULEINFOPAGEADDR      61			// 模块区页基地址

//系统参数区中各参数的页地址
#define PARAMETERPAR0_ADR		0			// 参数表区0在eeprom中的页偏移
#define PARAMETERPAR1_ADR		16			// 参数表区1在eeprom中的页偏移

#define FRONTSETTING_ADR		0
#define COMMONSETTING_ADR		3
#define SAMPLESETTING_ADR		4
#define ANALYSISSETTING_ADR		5
#define OTHERSETTING_ADR		6
#define FILENAMESETTING_ADR		8
#define ENCRYPT_ADR				9			// 系统加密区的页偏移
#define FILENAMETYPE_ADR		10			// 文件名类型参数在参数表中的页偏移
#define SYSTEMSET_ADR			11			// 系统设置参数在参数表中的页偏移
#define WIFISET_ADR				12			// WiFi设置参数在参数表中的页偏移
#define SERIALNUM_ADR			13			// WiFi设置参数在参数表中的页偏移
#define FACTORYCONFIG_ADR		14			// WiFi设置参数在参数表中的页偏移
#define SYSTEMVERSION_ADR		15			// 系统版本参数在参数表中的页偏移

#define MACHINETYPE_ADR		    28         	//定义机器类型保存页偏移，该页码只读

//校准数据的宏
#define CALIBRATION_CHANNEL_NUM	8
#define CALIBRATION_WAVE_NUM	7

/* 校准数据 */
typedef struct _channelPara
{
	float				k[CALIBRATION_CHANNEL_NUM];
	float				b[CALIBRATION_CHANNEL_NUM];
	unsigned char		ucReserved[60];	//保留区，用于扩展和页对齐
	unsigned int 		ulCrc32;		//以上数据的CRC32校验码
}CHANNELPARA, *PCHANNELPARA;
#define CHNNELPARAVALID 56    			//校准数据有效字节数

typedef struct _calibrationPara
{
	CHANNELPARA channelPara[CALIBRATION_WAVE_NUM];
}CALIBRATIONPARA, *PCALIBRATIONPARA;

/* 机器型号结构体 */
typedef struct _machineType
{
	unsigned long		ulHeadPad;		 	//机器型号字节头部填充码，有效值为0xA5A5A5A5
	unsigned char       ucType[8];      	//机器型号字符串，最后一字节为0
	unsigned long		ulTailPad;			//机器型号字节尾部填充码，有效值为0x5A5A5A5A
	unsigned char		ucReserved[108];	//保留区，用于扩展和页对齐
	unsigned int 		ulCrc32;
}__attribute__((packed))MACHINETYPE,*PMACHINETYPE;//128byte, 1page
#define MACHINETYPEVALID 124    			//机器型号区有效字节数

/* 加密信息数据结构 */
typedef struct _encrypt
{
	unsigned int	uiEncryptFlg[APPNUM];	// 应用是否处于加密状态，为PRODUCTLOCK表示处于加密状态
	unsigned int	uiNCount[APPNUM];		// 分期付款期数
	unsigned int	uiDays[APPNUM];			// 分期付款每期天数
	unsigned long long 	DNA;				// F210序列号，即FPGA序列号
	unsigned int	uiLastPowerOFFTime;		// 上次关机时间
	unsigned char	ucReserved[16];			// 保留区，用于扩展和页对齐
	unsigned int	uiCrc32;				// 以上数据的CRC32校验码
}__attribute__((packed))APPENCRYPT, *PAPPENCRYPT;// 128byte, 1page
#define ENCRYPTVALIDBYTE 124    			// 加密信息区有效字节数

/* 文件命名规则结构体存储形式 */
typedef struct _FileNameType
{
    unsigned short 	fileType;    		//文件类型
    unsigned short 	nameType;    		//命名类型
    unsigned int 	idNum;         		//Id No.
    unsigned short 	tapeType;    		//Tape 类型
    unsigned short 	tapeNum;     		//Tape No.
    unsigned int 	waveLength;    		//波长
    char 			comment[20];        //Comment
	unsigned char	ucReserved[88];		//保留区，用于扩展和页对齐
	unsigned int	ulCrc32;
}__attribute__((packed))FILENAMETYPE, *PFILENAMETYPE;//128byte, 1page
#define FILENAMETYPEVALIDBYTE 124    	//文件名设置区有效字节数

/* 系统设置结构体定义 */
typedef struct __systemset
{
	unsigned long 	lCurrentLanguage;   // 系统的语言
	int 			iLanguageSelect;    // 系统设置中语言的选择项
    
	unsigned int 	uiWarning;          // 系统设置中警告的选择
    
	unsigned int 	uiACPowerOffValue;  // 插入电源适配器后自动关机的时间
	unsigned int 	uiACScreenOffValue; // 插入电源适配器后屏幕进入省电模式的时间
    
	unsigned int 	uiACLCDBright;      // 插入电源适配器后屏幕的亮度值
    
	unsigned int 	uiDCPowerOffValue;  // 仅有电池的情况时自动关机的时间
	unsigned int 	uiDCScreenOffValue; // 仅有电池的情况时屏幕进入省电模式的时间
    
	unsigned int 	uiDCLCDBright;      // 仅有电池的情况时屏幕的亮度值
    
	int 			iACPowerSaveSelect; // 
	int 			iACLCDBrightSelect; //
	int 			iDCPowerSaveSelect; //
	int 			iDCLCDBrightSelect; //

	char			cSerialNumber[20];	// 序列号
	char			cWaveLength1[10];	// 波长一
	char			cWaveLength2[10];	// 波长二
	char			cDate[26];			// 日期

	unsigned char 	ucKeyWarning;		//按键警告提示 Added by  2014.07.30
	unsigned char 	ucSkinStyle;		//皮肤样式
	char 	uiCountryStyle;		        //国家(中国和韩国)
    char	uiTimeShowFlag;
    
   	unsigned char	ucReserved[2];
	unsigned int	ulCrc32;
}__attribute__((packed))SYSTEMSET,*PSYSTEMSET;//128byte, 1page
#define SYSTEMSETVALIDBYTE 124    		//系统设置区有效字节数

/* wifi设置结构体定义 */
typedef struct __wifiset
{
	int				iWiFiOpenSta;		// WiFi打开关闭的状态，0、WiFi关闭；1、WiFI打开
	int				iWiFiConnectSta;	// WiFi连接的状态，1、WiFi处于连接状态；0、WiFi处于断开状态
	char			cSSID[40];			// 存放输入的SSID
	char			cPassWord[66];		// 存放输入的WiFi密码
	char			cFlags;			//存放输入的wifi加密类型
    
	unsigned char	ucReserved[9];		//保留区，用于扩展和页对齐
	unsigned int	ulCrc32;
}__attribute__((packed))WIFISET,*PWIFISET;//128byte, 1page
#define WIFISETVALIDBYTE 124    		//系统设置区有效字节数

typedef struct serial_number
{
    char MachineName[5];
    char MachineType[4];
    char Month[3];
    char Year[3];
    char Date[3];
    char Number[4];
    char Address[2];
	unsigned char	ucReserved[100];		//保留区，用于扩展和页对齐
	unsigned int	ulCrc32;
}__attribute__((packed))SerialNum, *PSerialNum;
#define SERIALNUMVALIDBYTE 124    		//系统设置区有效字节数

typedef struct _maintance_date
{
	unsigned int year;                               //年
    unsigned int month;                              //月
    unsigned int day;                                //日
}MAINTANCE_DATE, *PMAINTANCE_DATE;
typedef struct _factor_conifg
{
    int iDynamicRange;
    unsigned long long ulAvailableLanguage; //可用的语言
    MAINTANCE_DATE  lastDate;               //上次维护日期
    MAINTANCE_DATE  currentDate;            //当前维护日期
    unsigned char   enabled;                //授权使能(第一位:OPM 第二位:Lightsource 第三位:VFL 第四位:FiberMicroscope 高四位:SOAL)
    unsigned int    speakerFreq;            //蜂鸣器频率(Hz) 默认2600Hz
	unsigned char	ucReserved[83];		    //保留区，用于扩展和页对齐(112-24)
	unsigned int	ulCrc32;
}__attribute__((packed))FactoryConfig, *PFactoryConfig;
#define FACTORYCONFIGVALIDBYTE 124    		//系统设置区有效字节数


/* 文件版本控制结构体定义 */
typedef struct _version
{
	char 			BootStrap[8];		// at自带的bootstrap的版本号
	char 			Logo[8];			// Logo的版本号
	char 			Recovery[8];		// Recovery的版本号
	char 			Fpga[8];			// Fpga的版本号
    char 			Kernel[8];			// kernel的版本号
    char 			Rootfs[8];			// rootfs的版本号
    char 			Userfs[8];			// userfs的版本号
    char 			Reserved1[8];		// 预留1的版本号
    char 			Reserved2[8];		// 预留2的版本号
    char 			MachineType[8];		// 机器型号
	unsigned char	ucReserved[44];		// 保留区，用于扩展和页对齐
	unsigned int	ulCrc32;			// 校验码
}__attribute__((packed))VERSION, *PVERSION;
#define VERSIONVALIDBYTE 124    		//系统版本区有效字节数


/*********************************************************************************************************
* eeprom中相关结构体的定义
*********************************************************************************************************/

/* EEPROM页结构 */
typedef struct _page	
{
	unsigned char 	ucCount[128];
}__attribute__((packed))PAGE;				/* 128Byte */

//系统参数数据结构
typedef struct _parameter_module	
{
	FRONT_SETTING		sFrontSetting[WAVE_NUM];	//3page
	COMMON_SETTING		sCommonSetting;				//1page
	SAMPLE_SETTING		sSampleSetting;				//1page
	ANALYSIS_SETTING	sAnalysisSetting;			//1page
	OTHER_SETTING		sOtherSetting;				//2page
	FILE_NAME_SETTING 	sFileNameSetting;			//1page
	//USER_SETTINGS	UserSettingsParameter;	//9 pages
	APPENCRYPT		UserEncryptParameter;	// 系统加密区；1page
	FILENAMETYPE   	FileNameTypeParameter;	// 文件名设置，1page
	SYSTEMSET		SystemSetParameter;		// 系统设置参数，1page
	WIFISET			WiFiSetParameter;		// WiFi设置参数，1page
	SerialNum       SerialNum;
    FactoryConfig   FactoryConfg;
	VERSION			SystemVersion;			// 当前系统中的版本号，1page
}__attribute__((packed))ParameterModule;	// 14 pages

/* 缓冲数据结构 */
/*
 *	缓冲区的作用，防止掉电等意外情况发生时中断了往eeprom中的写操作，从而造成数据错误。
 *	当往eeprom中写数据时，先写到有效的缓冲区，再写到真正的保存区域；如果在写缓冲区的过程中
 *  掉电，则不影响真正的数据，如果在写到真正的保存区域时掉电，则在读这个区域的数据时，发现crc
 *	校验错误，会从缓冲区恢复正确的数据
 */
typedef struct _data_buffer
{
	PAGE		   	Data;					//数据域，待写入页的数据
	unsigned short 	State;					//状态域，缓冲数据状态，0xA5A5 ----Valid, 其它----stopped   
	unsigned short 	PageAddr;				//地址域，待写入页地址
	unsigned long  		Reserved[30];			//未使用
	unsigned int  		ulCrc32;				//缓冲数据CRC32码
}__attribute__((packed))APPBUFFER;			// 256Byte, 2 PAGE
#define DATABUFFVALIDBYTE	252 			//数据缓冲区有效字节数

//模块数据结构
typedef struct _module_info
{
	unsigned short	SysParBaseAddr;			//参数表基地址，参数区0----0x0000, 参数区1----
	unsigned short	ReservedBaseAddr;			//保留区基地址，
	unsigned short	BufferBaseAddr;			//缓冲区基地址，
	unsigned short	ModuleInfoBaseAddr;		//模块信息区基地址，0xFE00，同时用于系统第一次启动时参数格式化用

	//用户信息针对当前参数表，参数表切换时需要进行相应的设置
	unsigned char	UserInfoIndex;			//当前活动用户信息区索引值，0~15
	unsigned short	UserInfoErr;			//当前参数表用户信息区错误域，bit0~15对应分区0~15,  1--损坏	，0--正常							

	unsigned char	SysParErr;				//参数区错误标志域bit0 -- 参数区0，bit1 -- 参数区1， 1--损坏，0--正常
	unsigned short	BufferErr;				//缓冲区错误标志域，bit0~15分别对应缓冲区0~15, 1--损坏，0--正常

	unsigned char	Reserved[110];			//保留区，用于扩展和页对齐
	unsigned int	ulCrc32;				//以上数据的CRC32校验码
}__attribute__((packed))APPMODULEINFO;		//128Byte, 1page
#define MODULEVAILDBYTE	124 				//模块信息区有效字节数


//系统参数区，定义的都是指针
typedef struct _app_parameter
{
	FRONT_SETTING		*sFrontSetting[WAVE_NUM];
	COMMON_SETTING		*sCommonSetting;
	SAMPLE_SETTING		*sSampleSetting;
	ANALYSIS_SETTING	*sAnalysisSetting;
	OTHER_SETTING		*sOtherSetting;
	FILE_NAME_SETTING 	*sFileNameSetting;
	//USER_SETTINGS	*pUserSettingsParameter;
	APPENCRYPT		*pUserEncryptParameter;	//系统加密区
	FILENAMETYPE	*pFileNameTypeParameter;//文件名设置
	SYSTEMSET		*pSystemSetParameter;	//系统设置参数
	WIFISET			*pWiFiSetParameter;		// WiFi设置参数
	SerialNum       *pSerialNum;
    FactoryConfig   *pFactoryConfg;
	VERSION			*pSystemVersion;		//当前系统中的版本号
}__attribute__((packed))APPPARAMETER;


/* EEPROM中实际数据存储分布 */
typedef struct _eeprom_data
{
	/* 系统参数区 */
	ParameterModule		Parameter[2];		/* 14 * 2 = 28page, Parameter[1]是备份区     */
	/* 机器型号区 */
	MACHINETYPE         MachineType;		/* 机器型号保存区，1page                 1  */
	/* 保留区     */
	//PAGE				Reserved[467];		/* 467page                         	  	   */				      
	/* 缓冲区     */
	APPBUFFER			Buffer[16];			/* 缓冲区，每份2page，共32page           32  */
	/* 模块信息区 */
	APPMODULEINFO		ModuleInfo[4];		/* 参数模块信息，同时保存4分，SD备份一份  4 */
}__attribute__((packed))EEPROMDATA;			/* 此款E2prom共512 page, 64KB，目前只用前65page */

#endif
