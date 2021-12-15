/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmupdate.h
* 摘    要：  声明系统升级相关函数
*            
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：
*******************************************************************************/

#ifndef _APP_FRMUPDATE_H
#define _APP_FRMUPDATE_H

/*******************************************************************************
**					为实现应用层frmupdate而需要引用的其他头文件 			  **
*******************************************************************************/
#include "app_global.h"
#include <sys/msg.h>
#include "app_parameter.h"

/**********************************************************************************
**									宏定义 				 						 **
***********************************************************************************/
//#define	FIRST_WRITE		//进行第一次出场烧录的宏开关，如定义此宏，则为第一次出场烧录	
//#define	TEST_WRITE		//进行烧录测试的宏开关

#ifdef	FIRST_WRITE
#define USBPATH 		MntUsbDirectory"/firmware_O10_first.md5"
#define SDPATH  		MntSDcardDataDirectory"/firmware_O10_first.md5"
#else
#define USBPATH 		MntUsbDirectory"/firmware_O10.md5"				    
#define SDPATH  		MntSDcardDataDirectory"/firmware_O10.md5"			
#endif

#define RECOVERY_USBPATH MntUsbDirectory"/firmware_O10_recovery.md5"				// 包含升级包和相应的md5值文件的修复包的名字
#define RECOVERY_SDPATH  MntSDcardDataDirectory"/firmware_O10_recovery.md5"		// 包含升级包和相应的md5值文件的修复包的名字
#define ROOTFS_RECOVERY_THRESHOLD   5*1024*1024										//roofs分区需要修复的阈值 5M


#define USB				"/tmp/usb"
#define SD				"/tmp/sdcard"	


/**********************************************************************************
**								类型定义 				 						 **
***********************************************************************************/
#define MESSAGE_NUM 10
	
struct msg_st  
{  
	long int msg_type;	
	char message[MESSAGE_NUM];	
}; 

struct msg	
{  
	int iCmdType;			// 命令码  
	int iParameter; 		// 附加参数
}; 
	
	
/**********************************************************************************
**								 错误码、命令码定义 							 **
***********************************************************************************/
#define FROM_SERVER			01
#define TO_SERVER			02
	
#define SUCESS				01
#define FAIL				00
	
/**********************************服务器侧定义************************************/
#define CMD_UPDATE			1		// 开始升级命令
#define CMD_WRUSERFS		2		// 升级Userfs分区命令
#define CMD_SAVEPARAMETER	3		// 保存版本信息
	
/**********************************客户端侧定义************************************/
#define ERR_MKDIR			1		// 创建挂载点目录命令
#define ERR_MOUNT_ROOTFS	2		// 挂载Rootfs分区命令
#define ERR_MOUNT_USERFS	3		// 挂载Userfs分区命令
#define ERR_MOUNT_CACHE		4		// 挂载临时分区命令
#define ERR_EXTRACT			5		// 解压升级包命令
#define ERR_WRBOOTSTRAP		6		// 升级BootStrap
#define ERR_WRLOGO			7		// 升级Logo
#define ERR_WRRECOVERY		8		// 升级Recovery
#define ERR_WRFPGA			9		// 升级Fpga
#define ERR_WRKERNEL		10		// 升级Kernel
#define ERR_WRROOTFS		11		// 升级Rootfs
#define ERR_WRUSERFS		12		// 升级Userfs
#define ERR_SAVEPARAMETER	13		// 保存版本信息
#define ERR_SOFT_NOT_MATCH	14		// 机器型号不匹配
	
#define CMD_SHOWPROGRESS	70		// 显示升级进度命令
#define CMD_LOWFLG			71		// 获取待升级应用程序低于当前版本标记
#define CMD_WRSECTOR		50		// 显示"正在写入扇区..."信息
#define CMD_WRSUCESS		51		// 显示"升级成功..."信息
	
#define	PAR_USB				90
#define	PAR_SD				91

/***
  * 功能：
		获取测试升级文件路径
  * 参数：
		1、char *pUpdatePath:	存贮升级文件路径
  * 返回：
		成功返回零，失败返回非零值
		-2:	未插入SD卡或U盘
		-3:	未放入升级文件
  * 备注：
		优先加载U盘中的应用程序升级文件
***/
int GetTestUpdatePath(char *pUpdatePath);

/***
  * 功能：
        获取升级文件路径
  * 参数：
        1、char *pUpdatePath:	存贮升级文件路径
  * 返回：
        成功返回零，失败返回非零值
        -2:	未插入SD卡或U盘
        -3:	未放入升级文件
  * 备注：
  		优先加载U盘中的应用程序升级文件
***/
int GetUpdatePath(char *pUpdatePath);


/***
  * 功能：
		解压升级包文件，解压出升级应用程序和真正的升级包
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int CreateUpdateFile(const char *cUpdateFileName);


/***
  * 功能：
        设置电源管理的使能性
  * 参数：
        1、unsigned char ucEnable:	1、使能电源管理；0、不使能电源管理
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetPowerManageEnable(unsigned char ucEnable);


/***
  * 功能：
		初始化消息队列
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int InitMsgQueue(void);


/***
  * 功能：
		向消息队列发送消息
  * 参数：
		1、int iCmdType		:	命令码
		2、int iParameter	:	附加参数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int MsgSend(int iCmdType, int iParameter);


/***
  * 功能：
		向自己的消息队列发送消息
  * 参数：
		1、int iCmdType		:	命令码
		2、int iParameter	:	附加参数
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int MsgSendSelf(int iCmdType, int iParameter);


/***
  * 功能：
		从消息队列中获取消息
  * 参数：
		1、struct msg *pMsg:	消息缓冲区	
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int GetMsg(struct msg *pMsg);


/***
  * 功能：
        从指定路径中读取版本信息
  * 参数：
        1、const char *pFilePath	:	存放版本信息的路径
        2、VERSION *pPar			:	存贮版本信息的变量
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ReadUpdateVersion(const char *pFilePath, VERSION *pPar);


/***
  * 功能：
		获得磁盘指定分区剩余空间大小
  * 参数：
		1、const char *path:磁盘路径	
  * 返回：
		返回磁盘剩余空间大小(字节)
  * 备注：
***/
unsigned long long getDiskFreeSize(const char *path);

#endif
