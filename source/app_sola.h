/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_sola.h
* 摘    要：  光眼功能
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef _APP_SOLA_H_
#define _APP_SOLA_H_

#include "app_frmotdr.h"
#include "app_middle.h"

typedef enum _sola_event_type
{
    Ordinary_eventType = 0, //普通事件分析事件
    Splitter_eventType,     //分路器事件
}SOLATYPE;

typedef enum _sola_splitter_kinds
{
    SPLITTER_None = 0,
    SPLITTER1_2 = 1,
    SPLITTER1_4,
    SPLITTER1_8,
    SPLITTER1_16,
    SPLITTER1_32,
    SPLITTER1_64,
    SPLITTER1_128,
    SPLITTER_COUNT,
    
}SOLASPLITTERKINDS;

/**
 * SOLA事件表类型声明
 */
typedef struct _sola_event
{
    int WaveIsSetted[WAVE_NUM];                 //波长是否已经设置
    EVENTS_TABLE WaveEventTable[WAVE_NUM];      //不同波长的事件表
    EVENTS_TABLE EventTable;                    //最终的事件表
    float TotalReturnLoss[WAVE_NUM];            //
    char *absolutePath;                         //文件路径(包含文件名)
    time_t ulDTS;                               //测量事件
} SolaEvents;

typedef struct sola_file_name_setting
{
	//int iAutoNamedEnable;							//是否启用自动命名的标志位
	char prefix[50];
    unsigned int suffix;
    int suffixWidth;                                //文件名后缀的位数(3,4,5)
	unsigned int suffixRule;		                //后缀变化规则，1递增，0递减	
	unsigned int fileFormat;		                //默认的文件类型 1.PDF 2.SOR						
	
	//unsigned int 	enFileOperation;	            //主界面文件管理文件操作类型
	//FILETYPE 		enFilter;			            //主界面文件管理文件过滤类型	

}__attribute__((packed))SOLA_FILE_NAME_SETTING;

//数据信息
typedef struct _sola_data
{
    int  wave;                                      //波长
    short* data;                                    //数据
    int count;                                      //数据个数
    float onePoint;                                 //点与点间距
}SOLA_DATA;
typedef struct _sola_data_list
{
    int waveNum;                                    //波长个数
    SOLA_DATA* solaData;                            //sola数据
}SOLA_DATA_LIST;

//静态初始化事件列表的宏定义
#define EVENTS_TABLE_INITIALIZER {.iEventsNumber = 0, .iEventsStart = 0}
#define SOLA_EVENTS_INITIALIZER {\
        .WaveEventTable = {EVENTS_TABLE_INITIALIZER, EVENTS_TABLE_INITIALIZER, EVENTS_TABLE_INITIALIZER},\
        .EventTable = EVENTS_TABLE_INITIALIZER, \
        .WaveIsSetted = {0, 0, 0},\
    }

//获取事件个数
int SolaEvents_Count(SolaEvents* me);

//判断事件是否有效
int SolaEvents_IsVaild(SolaEvents* me);

//清除事件
void SolaEvents_Clear(SolaEvents* me);

//获取事件总距离
float SolaEvents_GetTotalDistance(SolaEvents* me);

//获取指定位置的事件
const EVENTS_INFO* SolaEvents_GetEvent(SolaEvents* me, int index);

//获取指定波长指定位置的事件
EVENTS_INFO* SolaEvents_GetWaveEvent(SolaEvents* me, int index, int wave);

//获得指定波长的累计损耗
float SolaEvents_GetTotalLoss(const SolaEvents* me, int wave);

//获得sola事件类型
//分otdr事件类型，sola的分路器事件类型
int SolaEvents_GetEventType(SolaEvents* me, int index, SOLATYPE type);
//设置sola事件类型
//分otdr事件类型，sola的分路器事件类型
void SolaEvents_SetEventType(SolaEvents* me, int index, SOLATYPE type, int style);

//获取波长的End事件信息
const EVENTS_INFO* SolaEvents_GetEndEvent(SolaEvents* me, int wave);

int IsSolaEventPassed(SolaEvents* me, int index);
int IsSolaLossPassed(const EVENTS_INFO* info, int wave, int splitter_type, int otdr_type);
int IsSolaIORPassed(const EVENTS_INFO* info, int wave, int splitter_type, int otdr_type);

//打印调试
void SolaEvents_Print(SolaEvents* me, FILE* out);

//获取事件类型的名字
const char* SolaEvents_GetEventTypeName(SolaEvents* me, int index);

/**
 * 错误码
 */
enum _sola_error_code
{
    SOLA_NO_ERR,                    //无错误
    SOLA_BOARD_ERR,                 //硬件初始化错误
    SOLA_CHECK_LIGHT_ERR,           //有光检查错误
    SOLA_CHECK_ENDFACE_ERR,         //断面检查错误
};


/**
 * 枚举SOLA执行过程的状态
 */
enum _sola_status
{
    SOLA_STOPED,                //已停止
    SOLA_STARTED,               //已启动
    SOLA_INITIALIZING_BOARD,    //正在初始化硬件
    SOLA_ACQUIRING_DATA,        //正在采集数据
    SOLA_ANALYZING_DATA,        //正在分析事件
    SOLA_DATA_READY,            //数据已经准备好
    SOLA_PROGRESS_CHANGED,      //进度发生变化
    SOLA_WAVE_CHANGED,          //波长发生变化
    SOLA_ERROR,
};


/**
 * sola 传出消息类型声明
 * 为了消息以值传递(不牵扯动态内存分配)，将消息的总大小设计为4个字节，
 * 在F214中，完全可满足应用需求
 */
typedef struct _sola_message
{
    unsigned short Type;
    unsigned short Value;
} SolaMessage;

/**
 * 初始化当前的SOLA对象，开机调用一次
 */
int CurrSola_Init();

/**
 * 完成SOLA对象的使用，和Sola_Init()配对调用
 * 对于本项目，一般在关机时调用
 */
int CurrSola_Finish();

/**
 * 启动SOLA进行采集
 */
int CurrSola_Start();

/**
 * 停止SOLA的采集过程
 */
int CurrSola_Stop();

/**
 * 判断当前的Sola是否正在运行
 */
int CurrSola_IsRunning();

/**
 * 读取Sola对外产生的消息，成功返回0其他返回非0值
 * SOLA 内部有一个消息队列，会将SOLA测量过程中的各种消息放入队列中，外部需要知道SOLA的当前的状态等
 * 信息需要轮询该消息队列，该队列在每次开始测量时清空
 */
int CurrSola_GetMessage(SolaMessage* message);

/**
 * 获得SOLA分析完成的事件表
 * 每次判断到有新的事件的时候，调用该函数将事件表拷贝到指定的对象中
 */
int CurrSola_GetSolaEvents(SolaEvents* dest);

/**
 * 获得错误信息
 */
const char* CurrSola_GetErrorString(int code);

//保存组合数据
//void savedata_s(SolaTask *Task, COMBINE_INFO *pCombine, int iDaqCnt, int iDaqPos, char *strPath);

/*
 *保存sola采集数据
*/
void SaveAdcSolaData(PDAQ_SETTING DaqSettings, int cyc, int iDaqPos, int wave, int pulse);


/*
 *保存sola采集数据
*/
void SaveSigCombineSolaData(COMBINE_INFO *pCombine, int cyc, int iDaqPos, int wave, int pulse);
//设置文件名
int setSolaName(SolaEvents* _this, const char *absoluteName);

//获取sola波形数据
SOLA_DATA* GetSolaCurveData();
//获取sola波形数据的波长个数
int GetSolaCurveWaveNum();
//销毁sola曲线数据
void SolaData_Destroy();
#endif
