/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_global.h  
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

#ifndef _APP_GLOBAL_H
#define _APP_GLOBAL_H


/***************************************
* 为定义APP_GLOBAL而需要引用的其他头文件
***************************************/
#include "app_middle.h"
#include "guibase.h"
//#include "app_getsetparameter.h"

#include  "app_memory_detecter.h"

//#define APP_DEBUG			//打开全局的调试开关，如需调试单个文件，可在源文件中定义调试宏
//#define TIME_DEBUG

#ifdef APP_DEBUG
#define DBG_ENTER()		printf("###########Enter : %s###########\n", __func__)
#define DBG_EXIT(rtn)  	printf("###########Exit : %s,code = %d###########\n", __func__, rtn)
#define DBG_LINE()		printf("%s:-----------------Line:%d-----------------\n", __func__, __LINE__);
#define DISP_LINE(n)	printf("%s:Line=%d,Value=%d\n", __func__, __LINE__,n) //added by  2015.9.6

#else
#define DBG_ENTER()
#define DBG_EXIT(rtn)
#define DBG_LINE()
#define DISP_LINE(n)//added by  2015.9.6

#endif

//#define TIME_DEBUG
#ifdef TIME_DEBUG
#define TRANSFER_TIME(stv, etv) do { \
			printf("[CALC_TIME|%s@%s,%d]: used %ld usec\n",\
			__func__, __FILE__, __LINE__, \
			(((etv).tv_sec) - ((stv).tv_sec)) * 1000000 + (((etv).tv_usec) - ((stv).tv_usec))); \
			} while(0)
#else
#define TRANSFER_TIME(stv, etv)
#endif

//LOG级别
#define LOG_VERBO	0
#define LOG_TRACE	1
#define LOG_DEBUG	2
#define LOG_INFO	3
#define LOG_ERROR	4
#define LOG_OPEN	5

//定义打印调试的LOG级别，大于等于此级别的才会输出
//int log_level;

static const char strLog[6][10] = {{"VERBOSE"}, {"TRACE"}, {"DEBUG"}, {"INFOR"}, {"ERROR"}, {"OPEN"}}; 

extern unsigned int uiCoderID;

#define LOG(level, format, ...) \
	do {\
    	if (level >= LOG_LEVEL)\
    	{\
			fprintf(stderr, "[%s|%s,%d] " format, \
				strLog[level], __func__, __LINE__, ##__VA_ARGS__); \
    	}\
    } while (0)



#define CoderRoot 		(0x01u << 0)
#define CoderAlgorithm	(0x01u << 1)
#define CoderWu 		(0x01u << 2)
#define CoderShi		(0x01u << 3)
#define CoderShan		(0x01u << 4)
#define CoderYun		(0x01u << 5)
#define CoderGu			(0x01u << 6)
#define LogOff			(0x00u)

#ifdef CODER_EYES
#define CODER_LOG(level, format, ...) \
			do {\
				if (level & uiCoderID)\
				{\
					fprintf(stderr, "[CODER_EYES 0x%.2X|%s@%s,%d] " format, \
							level, __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
				}\
			} while (0)
#else
#define CODER_LOG(level, format, ...)

#endif

#define FLOATEQZERO(x) ( (x < 0.000001f) && (x > -0.000001f) )

/***********************************************
* 定义关于设备文件描述符集合的数据结构及相关函数
***********************************************/
typedef struct _device_fd
{
    int iFpgaDev;
    int iOpticDev;
    int iPowerDev;
    int iEepromDev;
    int iRtcDev;
	int iLightDev;		//lcd亮度设备描述符
	int iNetWireDev;	//rj45设备
} DEVFD;

//初始化设备驱动，获得设备文件描述符
int InitializeDevice(void);
//释放设备驱动，关闭设备文件描述符
int ReleaseDevice(void);

//得到全局的设备文件描述符集合
DEVFD* GetGlb_DevFd(void);

/*******************************
* 定义关于实时时钟的公共操作函数
*******************************/
//获取宽字符格式的当前日期字符串xxxx/xx/xx
GUICHAR* GetRtc_DateString(void);
//获取宽字符格式的当前时间字符串xx:xx
GUICHAR* GetRtc_TimeString(void);


/*******************************
* 定义CRC32校验码的公共操作函数
*******************************/
unsigned int crc32c(unsigned char *data, unsigned int length);

#endif  //_APP_GLOBAL_H

