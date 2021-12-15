/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmotdr_test.h
* 摘    要：  定义用于完成OTDR模块功能的应用程序接口
*
* 当前版本：  
* 作    者：  
* 完成日期：
*******************************************************************************/

#ifndef _APP_FRMOTDR_TEST_H
#define _APP_FRMOTDR_TEST_H

/****************************************
* 为实现app_frmotdr而需要引用的标准头文件
****************************************/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

/*************************************
* 为定义APP_OTDR而需要引用的其他头文件
*************************************/
#include "guibase.h"
#include "guiglobal.h"
#include "drv_fpga.h"
#include "wnd_global.h"
#include "app_middle.h"
#include "app_eventsearch.h"
#include "app_sigcombine.h"
#include "app_curve.h"
#include <stdlib.h>
#include "app_frmfilebrowse.h"
#include "app_math.h"

#include "app_otdr_algorithm_parameters.h"



//预定义是否保存数据，是否画出原始DAQ数据
//#define SAVE_DATA
//#define DRAW_DAQ_CURVE
#define APD_VOLTAGE

#define ATTO_CONFIG_PATH	MntDataDirectory"/data/att0.txt"
#define PROFILE_PATH		MntUpDirectory"/data/profile.par"

#define OTDR_TEST_JOB_KIND	5						//OTDR测试任务最大数(考虑到测量程序中可以最多执行5个任务)
#define OPM_AVGTIME_KIND	9						//一共有9中平均时间供选择

#define OPM_SMP_KIND		8						//采样分别率种类

//定义波长种类;
typedef enum _wave_len
{
    WAVELEN_1310    = 0,
    WAVELEN_1550    = 1,
    WAVELEN_1625    = 2,

    WAVE_NUM,               //波长总个数
}WAVELEN;

#define MAX_DATA	100000				            //采集数据的最大长度
#define COMSIZE     10                              //命令队列缓冲区大小


#define OTDR_TASK_STOP		0
#define OTDR_TASK_START		1
#define BLACK_LEVEL_OFFSET  10                      //噪声偏置量 added by  2015.8.7

#ifndef MINI2
#define SCALE_FACTOR 1000
#endif


//OTDR采集线程外发命令类型
typedef enum _otdr_msg_type
{
    ENUM_OTDRMSG_NONE           = 0,
	ENUM_OTDRMSG_NODATA	 		= 1,
	ENUM_OTDRMSG_START			= 2,
	ENUM_OTDRMSG_SHOWING		= 3,
	ENUM_OTDRMSG_STOP			= 4,	
	ENUM_OTDRMSG_WAVWARGS		= 5,
	ENUM_OTDRMSG_EVENTOK		= 6,
	ENUM_OTDRMSG_CONNECTCHECK   = 7,
	ENUM_OTDRMSG_LIGHTCHECK		= 8,
	ENUM_OTDRMSG_MEASARGS		= 9,
	ENUM_OTDRMSG_FILENAME		= 10,
	ENUM_OTDRMSG_READSOR		= 11,
	ENUM_OTDRMSG_MEASTIME       = 12,
	ENUM_OTDRMSG_CLEAREVENT     = 13,
	ENUM_OTDRMSG_LEFT_MOVE_MARK = 14,
	ENUM_OTDRMSG_RIGHT_MOVE_MARK= 15,
    ENUM_OTDRMSG_TYPES,         //最后一条作为消息类型的边界
}OTDRMSGTYPE;


//此处对OTDR消息进行分段，高16位为自定义数据，低16位为消息类型
//以满足消息可携带小量数据的需求(比如SOLA中的测量时间)
//由于原有的消息并不携带任何数据，所以原有处理消息的代码可不必改动
#define OTDRMSG_TYPE_MASK           0x0000ffff
#define OTDRMSG_VALUE_MASK          0xffff0000
#define OTDRMSG_TYPE(__msg)         (__msg & OTDRMSG_TYPE_MASK)
#define OTDRMSG_VALUE(__msg)        ((__msg & OTDRMSG_VALUE_MASK) >> 16)
#define ENCODE_OTDRMSG(__value, __type) ((((__value & 0xffff) << 16) & 0xffff0000) | ((__type & 0xffff)))

#define MILE_TO_M(_value)       ((_value)*1609.35f)       
#define FT_TO_M(_value)         ((_value)/3.28f)  
#define M_TO_MILE(_value)       ((_value)/1609.35f)
#define M_TO_FT(_value)         ((_value)*3.28f)


//struct _curve_para pCurve_Para;                 //添加结构体声明
typedef enum _data_status
{
	ENUM_DATA_NO_NEW 		= 0,					//为EACH_TASK_BUF结构中iNewDataFlag数据成员做定义的
	ENUM_DATA_HAVE_NEW		= 1						//所有数据已经处理并组合完
} DATA_STATUS;


typedef enum _dp_state
{
	ENUM_DPSTATE_WAITE		= 0,						//信号处理线程等待
	ENUM_DPSTATE_COMBINE	= 1,						//信号处理组合
}DPSTATE;

typedef enum _dp_mode
{
    ENUM_DP_STOP        	= 0,     					//数据处理线程停止工作
    ENUM_DP_COMBINE     	= 1,     					//数据处理线程通道组合
}DP_MODE;


typedef enum _average_time
{
   ENUM_AVG_TIME_AUTO		= 0,   	
   ENUM_AVG_TIME_15S		= 1,
   ENUM_AVG_TIME_30S		= 2,
   ENUM_AVG_TIME_60S		= 3,   
   ENUM_AVG_TIME_90S		= 4,
   ENUM_AVG_TIME_120S		= 5,
   ENUM_AVG_TIME_180S		= 6,
   ENUM_AVG_TIME_REALTIME 	= 7,   	
} AVERAGE_TIME;

typedef enum _otdr_test_mode
{
    ENUM_RT_MODE = 0,		//实时模式
    ENUM_AVG_MODE			//平均模式
}OTDR_TEST_MODE;

typedef enum _sample_interval
{
	ENUM_SMP_NORMAL			= 0,
	ENUM_SMP_HIGH_RES		= 1,
	ENUM_SMP_20CM			= 2,
	ENUM_SMP_50CM			= 3,
	ENUM_SMP_1M				= 4,
	ENUM_SMP_2M				= 5,
	ENUM_SMP_4M				= 6,
	ENUM_SMP_8M				= 7,
} SAMPLE_INTERVAL;


typedef enum _work_mode                  			//工作模式
{
	ENUM_MODE_NORMAL		= 0,					//普通模式
	ENUM_MODE_ONEBUTTON		= 1,					//单键测量模式
	ENUM_MODE_iOLM          = 2,                    //iOLM模式
} WORK_MODE;

//用于标记OTDR工作状态
typedef enum _work_status
{
	ENUM_OTDR_STOPPED	= 0,			//表示当前OTDR是开始测量的,测量程序中使用
	ENUM_OTDR_START		= 1,			//表示当前OTDR是停止测量的,测量程序、普通模式中使用
} WORK_STATUS;

typedef enum _otdr_command      //OTDR主控线程接收到的命令
{
    ENUM_NO_COMMAND        = -1,
    ENUM_REALT_KEY_UP      = 0,
    ENUM_AVG_KEY_UP        = 1,
    ENUM_START_BUTTON_UP   = 2,
    ENUM_STOP_BUTTON_UP    = 3,
    ENUM_IGNORE_EX_CHECK   = 4,		//忽略光纤有光
    ENUM_IGNORE_CON_CHECK  = 5,		//忽略接头检查
	ENUM_RESTART		   = 6,		//重新开始测量
	ENUM_RESET_TIME		   = 7,		//重新设置测量时间
}OTDR_COMMAND;
typedef enum _daq_command       //数据采集线程接收到的命令
{
    ENUM_DAQ_START      = 0,
    ENUM_DAQ_STOP       = 1,
}DAQ_COMMAND;

typedef struct _command_queue                       //接收命令队列定义
{
	int command[COMSIZE];
	int front;
	int rear;

	GUIMUTEX		mMutex;	
}COMMAND_QUEUE,*PCOMMAND_QUEUE;

typedef struct _default_save_path	//默认保存路径
{
	char cOtdrSorSavePath[128];
	char cOtdrReportSavePath[128];
	char cSolaSavePath[128];
}DEFAULT_SAVEPATH, *PDEFAULT_SAVEPATH;

typedef struct _pass_set_para	//通过/未通过检测中参数的结构，O10中使用;
{
	int		iSlectFlag;		//选择标识位;
	float	fThrValue;		//阈值;
}PASS_SET_PARA;
typedef struct _pass_thr		//通过/未通过检测中的阈值设置，O10中使用;
{
	PASS_SET_PARA			SpliceLossThr[WAVE_NUM];				//熔接损耗;
	PASS_SET_PARA			ConnectLossThr[WAVE_NUM];				//接头损耗;
	PASS_SET_PARA			ReturnLossThr[WAVE_NUM];				//反射率;
	PASS_SET_PARA			AttenuationThr[WAVE_NUM];				//衰减率dB/km;
	PASS_SET_PARA			RegionLossThr[WAVE_NUM];				//跨段损耗;
	PASS_SET_PARA			RegionLengthThr[WAVE_NUM];				//跨段长度(只有使能有效，阈值弃用);
	PASS_SET_PARA			RegionReturnLossThr[WAVE_NUM];			//跨段光回损;
    double                  fSpanLength_m[WAVE_NUM];                          //跨段长度（单位为m）
    double                  fSpanLength_ft[WAVE_NUM];                          //跨段长度（单位为ft）
    double                  fSpanLength_mi[WAVE_NUM];                          //跨段长度（单位为mi）
    int                     iThrEnable[WAVE_NUM];                   //使能开关(0:disable,1:enable)
}PASS_THR;


typedef struct _front_setting
{
	OTDR_TEST_MODE  enTestMode;                     //测试模式，0：实时模式，1：平均模式                 
	FIBER_RANGE		enFiberRange;					//光纤长度范围;
	OPM_PULSE		enPulseTime;					//脉冲时间;
	AVERAGE_TIME	enAverageTime;					//平均时间;
	
	GUIMUTEX		mDataMutex;						//互斥锁
	unsigned char		ucReserved[84];				//保留区，用于扩展和页对齐
	unsigned int		ulCrc32;
}FRONT_SETTING, *PFRONT_SETTING;					// 1 page,128 bit
#define FRONTSETVALIDBYTE 124    					//前面板上设置区的有效字节数


typedef struct _wavelength_args
{	
	float			fRefractiveIndex;				//不同波长的折射率;
	float			fBackScattering;				//不同波长的背向散射系数;
	float			fExcessLength;					//不同波长的余长系数;

	float			fMacroDaltaThr;					//宏弯曲损耗阈值;

}WAVELEN_ARGS, *PWAVELEN_ARGS;

enum _otdr_unit
{
    OTDR_UNIT_M_KM      = 0,
    OTDR_UNIT_YD_MI     = 1,
    OTDR_UNIT_FT_KFT    = 2,
};

typedef struct _common_setting
{
	unsigned int			iLightCheckFlag;				//有光检测标识位;
	unsigned int			iConectCheckFlag;				//首端连接器检测标识位;
	unsigned int            iAutoSaveFlag;                  //自动保存标识位;
	unsigned int			iMacroBendingFlag;				//显示宏弯曲标识位;
	unsigned int 			iAutoSpanFlag;					//自动放大所定义的光纤径距;
	unsigned int 			iPromptSaveFlag;			    //提示保存曲线;
	unsigned int 			iOTDRFileNameFlag;				//是否显示文件名 1 显示 0 不显示
	unsigned int			iPreviewAreaFlag;				//预览区是否显示
	unsigned int			iScreenShotFlag;				//是否显示截图按钮（添加全局截图功能后不在显示截图按钮）
	unsigned int			iCusorLblFlag;					//是否显示光标信息
	unsigned int            iLossMethodMark;				//损耗标记模块中近似方法，0代表LSA，1代表TPA
	unsigned int            iAtenuMethodMark;               //衰减标记模块中近似方法，0代表LSA，1代表TPA
	unsigned int			iHotkeyKey1;					//key1按键指向
	unsigned int			iHotkeyKey2;					//key2按键指向
	unsigned int            iAutoJumpWindow;                //测量完成后自动跳转到事件界面
	unsigned int            iOnlyShowCurrCurve;             //只显示当前曲线
	GUIMUTEX		        mDataMutex;						//互斥锁

    unsigned int            iUnitConverterFlag;             //单位换算标志位,0代表m/km, 1代表Miles, 2代表ft/kft
	unsigned int            iDisplaySectionEvent;           //显示区段事件
    
	unsigned int		    ulCrc32;
}COMMON_SETTING, *PCOMMON_SETTING; // 1 page,128 bit
#define COMMONSETVALIDBYTE 124    					//前面板上设置区的有效字节数


typedef struct _sample_setting
{
	int				iHighResolutionFlag;			//暂时没有使用到;
	SAMPLE_INTERVAL	enSmpResolution;				//采样分辨率
	AVERAGE_TIME	enAutoAvrTime;					//自动测试时平均时间选择;

	WAVELEN_ARGS	sWaveLenArgs[WAVE_NUM];			//不同波长的参数设置
	
	GUIMUTEX		mDataMutex;						//互斥锁

	unsigned char		ucReserved[40];					//保留区，用于扩展和页对齐
	unsigned int		ulCrc32;
}SAMPLE_SETTING, *PSAMPLE_SETTING;
#define SAMPLESETVALIDBYTE 124    					//前面板上设置区的有效字节数


typedef struct _analysis_setting
{
	int				iAutoAnalysisFlag;				//自动分析标识位;
	float			fSpliceLossThr;					//损耗检测阈值;
	float			fReturnLossThr;					//回波损耗阈值;
	float			fEndLossThr;					//末端检测损耗阈值;
	int				iEnableLaunchFiber;				//注入光纤使能
	int				iStartEventNum;					//径距起点事件编号;
	float			fLaunchFiberLen_old;			//注入光纤长度(弃用)
	int				iEnableRecvFiber;				//接收光纤使能
	int				iEndEventNum;					//径距终点事件编号（从光纤端部）;
	float			fRecvFiberLen_old;				//接收光纤长度(弃用)
	int				iEndPositionFlag;				//径距终点事件位置标识位，置0表示从径距起点开始，置1表示从光纤端部开始;
	GUIMUTEX		mDataMutex;						//互斥锁
	double			fLaunchFiberLen_ft;				//注入光纤长度
	double			fLaunchFiberLen_mi;				//注入光纤长度
	double			fRecvFiberLen_ft;				//接收光纤长度
	double			fRecvFiberLen_mi;				//接收光纤长度
	double			fLaunchFiberLen;			    //注入光纤长度
	double			fRecvFiberLen;				    //接收光纤长度
    int             iEnableLaunchFiberEvent;        //注入光纤事件使能
    int             iEnableRecvFiberEvent;          //接收光纤事件使能
    int             iLaunchFiberEvent;              //注入光纤的事件编号
    int             iRecvFiberEvent;                //接收光纤的事件编号（从末端算起）
    int             iEventOrLength;                 //按事件还是长度选项(0 长度， 非 0 事件)
    int             iEnableAnalysisThreshold;       //使能分析阈值
    int             iEnableLaunchAndReceiveFiber;   //使能注入光纤/接收光纤
}ANALYSIS_SETTING, *PANALYSIS_SETTING;
#define ANASETVALIDBYTE 124    						//分析设置区有效字节数

typedef struct _other_setting
{
	int				iDisplayPassInfoFlag;			//显示通过/未通过信息;
	int				iWaveLength;					//波长;
	PASS_THR		PassThr;						//通过/未通过阈值;
	
	GUIMUTEX		mDataMutex;						//互斥锁40
	
	unsigned char		ucReserved[40];				//保留区，用于扩展和页对齐
	unsigned int		ulCrc32;
}OTHER_SETTING, *POTHER_SETTING;// 2 pages,256 bit
#define OTHERSETVALIDBYTE 252    					//前面板上设置区的有效字节数

typedef struct file_name_setting
{
	int iAutoNamedEnable;							//是否启用自动命名的标志位
	char prefix[50];
    unsigned int suffix;
    int suffixWidth;                                //文件名后缀的位数(3,4,5)
	unsigned int suffixRule;		                //后缀变化规则，1递增，0递减	
	unsigned int fileFormat;		                //默认的文件类型 1.PDF 2.SOR 3.CUR					
	
	unsigned int 	enFileOperation;	            //主界面文件管理文件操作类型
	FILETYPE 		enFilter;			            //主界面文件管理文件过滤类型	

}__attribute__((packed))FILE_NAME_SETTING;

/**
 * SOLA配置结构声明
 */
typedef struct _sola_settings
{
    int     Wave[WAVE_NUM];         //波长
	int     iEnableLaunchFiber;     //注入光纤使能
    float   fLaunchFiberLen_old;    //注入光纤长度(弃用)
    int     iEnableRecvFiber;       //接收光纤使能
    float   fRecvFiberLen_old;      //接收光纤长度(弃用)
	int     iUnitSelected;          //当前选择的单位
	double  fLaunchFiberLen_ft;     //注入光纤长度(ft)
	double  fLaunchFiberLen_mile;   //注入光纤长度(mile)
	double  fRecvFiberLen_ft;       //接收光纤长度(ft)
	double  fRecvFiberLen_mile;     //接收光纤长度(mile)
	double  fLaunchFiberLen;        //注入光纤长度
	double  fRecvFiberLen;          //接收光纤长度
	unsigned int	ulCrc32;

}__attribute__((packed))SolaSettings;


typedef struct _user_settings						//所有在OTDR界面上的设置;
{
	WORK_MODE			enWorkMode;					//工作模式，单模or单模在线;
	int     			iWave[WAVE_NUM];			//波长选择，若选择该波长，则置一;

	OPM_WAVE			enWaveCurPos;				//当前指向的波长，与前面板设置的显示有关
													//当前只能选中一个波长的波形，需与任务同步
	
	FRONT_SETTING		sFrontSetting[WAVE_NUM];

	COMMON_SETTING		sCommonSetting;

	SAMPLE_SETTING		sSampleSetting;

	ANALYSIS_SETTING	sAnalysisSetting;

	OTHER_SETTING		sOtherSetting;

	FILE_NAME_SETTING 	sFileNameSetting;

    SolaSettings        sSolaSetting;
	GUIMUTEX			mDataMutex;					//互斥锁

} USER_SETTINGS,*PUSER_SETTINGS;

//结构定义说明：每次测量工作中，可运行多个任务，如多波长、iOLM等；
//每个任务都有独自的工作任务设置，会根据采集队列进行多次数据采集;
typedef struct _daq_setting
{
    UINT16          *pOrigData;                     //原始数据;
    unsigned int    iCloseFrontEndFlag;             //前端放大打开/关闭标识位;
    unsigned int    iCloseLdPulseFlag;              //光源打开/关闭标识位;
    unsigned int 	iOffset;						//无效数据偏移量
    float			fSmpRatio;						//采样比率
    unsigned int 	iSmpReg;						//采样寄存器设置值
    unsigned int    iNumInTask;                     //记录该组采集在总任务中的位置
    unsigned int	iDaqCount;			            //数据有光采集长度;
    unsigned int 	iDisplayCount;					//显示的长度
    unsigned int	iNoiseCnt;						//无光(黑电平)采集长度
	unsigned int	iPulseKind;						//用OPM_PULSE这个来赋值，表示当前任务要用来个脉宽的任务设置;
	unsigned int	iAtteDbClass;					//用哪个衰减量等级数据;
	unsigned int	iFilter;						//指定使用哪个累加次数;
	unsigned int    iAdcGap;						//ADC采集间隔	
	unsigned int    iPhaseShiftFilter;				//解决移相差补带来的速度慢问题  没用
}DAQ_SETTING,*PDAQ_SETTING;

typedef struct _daq_queue           //数据采集队列
{
    PDAQ_SETTING pDaq_SetQueue[MAX_DAQ_NUM];        //数据存储位置
    int iMaxQueueNum;                               //队列的最大数据数量
    int front;
    int rear;    
}DAQ_QUEUE,*PDAQ_QUEUE;

typedef enum _analys_status
{
	ENUM_NO_ANALYS		= 0,
	ENUM_ANALYS_START	= 1,
	ENUM_ANALYS_OVER	= 2
}ANALYS_STATUS;

typedef struct _task_settings			//单个任务设置;
{
	int				iDaqNum;			//采集任务个数
	int				iCurDaqPos;			//当前采集任务指针

	OPM_WAVE		enWave;				//波长
	OPM_PULSE		enPulseTime;		//脉冲时间
	AVERAGE_TIME	enAverageTime;		//平均时间
	FIBER_RANGE		enFiberRange;		//光纤长度范围
	
	int             iBlindLength;       //盲区长度，单位:点数;
	int				iAverageTime;		//平均时间,单位:秒;
	int				iDataCount;			//数据采集点数;
	int				iDisplayCount;		//数据显示点数 = iDataCount;
	int				iOffset;			//无效数据偏移点数;
	int             iSmpReg;     	 	//采样寄存器设置;
	float           fSmpRatio;     	 	//采样比率;
	float           fRangeDist;     	//量程，单位m;
	float           fSmpIntval;     	//采样间隔，单位m;
	float 			fAutoRange;			//自动量程(每次测试的光纤长度预测)

	int             iRealTimeFlag;     	//实时采集标志;

	COMBINE_INFO    *pCombine_Info;     //组合部分，多通道组合中的计算信
	ALGORITHM_INFO  *pAlgorithm_info;   //算法部分，事件识别信息

	DAQ_SETTING	    Daq_SetQueue[MAX_DAQ_NUM];		//采集参数队列
	DAQ_QUEUE       Daq_Queue;                      //数据采集队列

	GUIMUTEX		mMutex;	
	int				iDynamicRange;		//当前机型的动态范围
}TASK_SETTINGS,*PTASK_SETTINGS;

typedef struct _task_contexts
{
	unsigned int          	iTaskNum;				//本次任务个数;
	WORK_MODE		        enWorkMode;				//工作模式，单模or单模在线;
	unsigned int          	iCurTaskPos;			//当前任务的位置指针;
	unsigned int			iTaskRunSwitch;			//任务开始标识位,置 1 则启动任务,置0 则结束任务 ;
	WORK_STATUS             enWorkStatus;           //工作状态;
	DPSTATE                 enDpState;              //当前数据处理状态: 等待、组合、分析等;
	DP_MODE                 enDpMode;               //数据处理模式: 停止、组合、事件分析;
	unsigned int			iEventAnalysFlag;		//是否进行事件分析;
	unsigned int			iWriteSorFlag;			//是否写SOR文件;
	int                     iDrawCurveFlag;         //画图标识位;
	int                     *pLogSig;               //将Log数据查找表放置此处 delete
	GUIMUTEX			    mMutex;	

	TASK_SETTINGS		  	Task_SetQueue[MAX_TASK_NUM];	//指向工作任务设置的指针;
} TASK_CONTEXTS,*PTASK_CONTEXTS;
typedef struct _otdr_top_settings
{
	POPMSET				pOpmSet;                    //光模块寄存器设置
	PUSER_SETTINGS		pUser_Setting;              //参数面板中的用户设置
	PTASK_CONTEXTS		pTask_Context;              //任务上下文
	PDISPLAY_INFO       pDisplayInfo;               //显示信息结构体
	PCOMMAND_QUEUE      pComQueue;                  //接收到的命令队列
	int                 iOtdrExitFlag;              //OTDR退出标识位
	DEFAULT_SAVEPATH	sDefSavePath;				//各界面默认保存路径
}OTDR_TOP_SETTINGS,*POTDR_TOP_SETTINGS;


/*为了兼容F210的算法，强制添加一个结构体；后期优化时，可以去掉；added by  2015.8.10*/
typedef struct _curve_para
{
	int iSOLAMode;					//SOLA链路标志位，为了兼容F210算法添加的结构体
}CURVE_PARA,*PCURVE_PARA;

//OTDR关机释放资源
int OtdrPlatformRelease(void);
//系统监控线程
void* SystemMonitor(void *pThreadArg);
//获得任务开始的标志位
int GetTaskRunSwitch(void);
//设置任务开始的标志位
void SetTaskRunSwitch(int flag);
//获取当前的工作状态
int GetWorkStatus(void);
//设置当前的工作状态
void SetWorkStatus(int iStatus);
//OTDR主控线程
void *OtdrCtrlThread(void *pThreadArg);
//数据采集线程
void *DataAcquisitionThread(void *pThreadArg);
//计算第一个点的瑞利散射功率水平
void GetBackScatterInitialValue(PTASK_SETTINGS pTskSet, PDAQ_SETTING pDaqSet);

//数据处理线程
void *DataProcessingThread(void *pThreadArg);
//打开蜂鸣器
//void SpeakerStart(POPMSET pOpmSet, int iSpeakTime);
//工作初始化;
int BoardWorkInit(POPMSET pOpmSet, int iDisplayTip);
//光纤有光检查
int CheckFiberLight(POTDR_TOP_SETTINGS pOtdrTopSettings);
//击穿电压检测
int CheckBreakVoltage(POTDR_TOP_SETTINGS pOtdrTopSettings);
//检查光纤端面反射率
int CheckFiberEndFace(POTDR_TOP_SETTINGS pOtdrTopSettings);
//检测光纤长度
float CheckFiberLen(POTDR_TOP_SETTINGS pOtdrTopSettings, int solaSpliterNum);

//测试通道
void test_channel(POTDR_TOP_SETTINGS pOtdrTopSettings);
//得到APD击穿电压和温度，便于计算整定参数
void test_APDBreakVol(POTDR_TOP_SETTINGS pOtdrTopSettings);
//保存温度和APD电压值，便于计算整定参数
int SaveApdTemp(float *pTemp, int *pApdValue, int len);
// 最小二乘法计算斜率与截距
void CalLeastSquare_float(float *pXBuf, int *pYbuf, int count, float *k, float *b);
// 测试APD的增益等级，每级差2.5dB
void test_APDGrade(POTDR_TOP_SETTINGS pOtdrTopSettings);


int MatchOtdrEvent(EVENTS_TABLE* events, float len, int iBlind, int location);
float ThresholdOfMatch(float len, float smpInterval);
int AllocTaskContext(PTASK_CONTEXTS pTskContext);

//初始化任务上下文
int InitTaskContext(POTDR_TOP_SETTINGS pOtdrTopSettings);
//根据任务上下文，分别对每个任务进行初始化，包括任务空间申请，数据处理空间申请，画图参数初始化等
int AllocDataBuffer(PTASK_SETTINGS);
//任务结束的一些后续处理，主要为释放空间
void FreeDataBuffer(PTASK_SETTINGS pTask_Settings);
//ATT自整定
int TuneAtt();

//事件分析
void EventAnalyze(PTASK_SETTINGS pTskSet, int iCurPos);

//得到Daq采集队列
int GetDaqQueue(PTASK_SETTINGS pTask_Settings);
//根据采集队列，设置采集参数，并采集数据
int DaqStart(PDAQ_SETTING pDaq_Settings);

// 修改ADC采集的错误数据,FPGA修改后，去掉该函数
void changeErrorData(UINT16 *pData, int sigLen);


//打开光模块电源(开机调用)
int OpenOpmPwr(POPMSET pCurrOpm);
//关闭光模块电源(关机调用)
int CloseOpmPwr(POPMSET pCurrOpm);
//关闭opm电源
int CloseOpm(POPMSET pCurrOpm);

//工作结束后的后续处理
int WorkFinish(POPMSET	pCurrOpm);

//得到脉冲时间，输入参数为用户设置脉冲与光纤实际长度
OPM_PULSE GetPulseTime(OPM_WAVE iWave, OPM_PULSE enPulseTime, FIBER_RANGE enFiberRange, float fFiberDist);
//根据用户设置，得到平均时间
int GetAverageTime(AVERAGE_TIME enAverageTime);
//根据光纤实际长度，得到光纤范围
FIBER_RANGE GetFiberRange(float fFiberRealLength);
//根据光纤范围，得到显示数据点数
int GetSmpCount(FIBER_RANGE enFiberRange, float fRefractiveIndex, float fRatio);
//根据光纤设置量程得到实际距离
float Range2Dist(FIBER_RANGE enRange);

//根据脉宽类型和是否移相，计算盲区数据点数
int GetBlindLength(OPM_PULSE enPulse, float fRatio);
//初始化采集队列
int InitDaqQueue(PDAQ_QUEUE pDaq_Queue, int iMaxQueueNum);
//写入数据采集队列
int WriteDaqQueue(PDAQ_QUEUE pDaq_Queue, PDAQ_SETTING pDaq_Setting);
//读取数据采集队列
PDAQ_SETTING ReadDaqQueue(PDAQ_QUEUE pDaq_Queue);
//返回数据采集队列的三种状态:-1:Empty;0:Nomal;1:OverFolw
int CheckDaqQueue(PDAQ_QUEUE pDaq_Queue);
//初始化命令队列
int InitCommandQueue(COMMAND_QUEUE *pComQueue);
//发送命令
int SendCommand(COMMAND_QUEUE *pComQueue, int Command);
//读取命令
int ReadCommand(COMMAND_QUEUE *pComQueue);
//得到黑电平,并得到黑电平的方差
int GetBlack_Level(POPMSET pOpmSet, float *fStd);
//得到采样数据
int  GetAdc_Data(
int		iOpmFd,			//指向OPM的设备指针
OPMSET	*pCurrOpm,		//OPM set的指针
UINT16	*pBuf			//用来接收采集数据的缓冲区
);
//用来显示错误消息的
void SendMessageError(int iMessageID);
//获得APD设置寄存器值
UINT32 GetApd_SetValue(POPM_TUNING pOpmTunning, int iDbNum);
//设置任务上下文中多个任务部分的曲线参数，显示最终组合数据
void SetTaskCurvePara(PTASK_CONTEXTS pTask_Contexts);

void GetFileName(char *filename);

//每次开始任务之前对combineinfo中的必要数据进行恢复初始值
//注意:combineinfo中包含部分预先分配好的数据缓冲区，在此并不会清空
int ClearCombineInfo(COMBINE_INFO *pCombineInfo);

//
void CombData2DispInfo(PTASK_CONTEXTS pTskContext, COMBINE_INFO *pCombInfo);

//启动otdr开始测量
int RunOtdrModule(int arg);


//后面的代码是保存数据的函数原型说明 added by  2015.9.6
//得到文件序列号
int GetSnNum(void);
//在数据采集完毕后，把每个任务缓冲区的数据做完平均后，保存到文件当中
void SaveEachBufdata();
//把一定长度的数据保存到文件当中去
void SaveOtdrData(      
	int iSn,                //存储一组数据的序列号
	int iNum,               //第几组数据
	int len,                //保存数据的长度
	UINT16 *DataBuf         //数据缓冲区
);
//得到文件指针
int GetFid(void);
//打开文件
int OpenFile(
	int iSn,        //该文件组序列号
	int iNum        //第几组数据
);
//关闭文件
int CloseFile(void);

//存储adc数据
int SaveAdcData(PTASK_SETTINGS pTaskSet, int iTaskPos, int iCnt, char *strPath);


//启动计时
void StartTimeCnt(int iStartTime, int iIsRT);
//停止计时
void StopTimeCnt(int iIsRT);

//打印寄存器的值，调试用
void RegValueInfo(int iOpmFd);

//读取sor数据
int ReadSor(int iCurCurve, char *FileName, int clearCurve);
//写sor数据
int WriteSor(int iCurCurve, char *FileName);

//保存sor数据
int SaveSorFile(int iActiveCurve, char * FilePath);

int ReadInno(char *FileName);
int ReportOtdrProgress(GUICHAR *pStrProgress);

//
SAMPLE_INTERVAL GetSmpResolution(FIBER_RANGE range, float fRatio);

//每次开始任务之前对combineinfo中的必要数据进行恢复初始值
//注意:combineinfo中包含部分预先分配好的数据缓冲区，在此并不会清空
//设置曲线Y轴放大铺满显示区
//void SetCurvesFullScreen(void);

//重新对各曲线事件列表做阈值判断
void ReevaluateEvent(void);
//计算跨段长度
float GetSpanLength(const EVENTS_TABLE *eventsTable);
//计算跨段损耗
float GetSpanLoss(const EVENTS_TABLE *eventsTable);
//计算跨段光回损
float GetSpanORL(EVENTS_TABLE *eventsTable, PCURVE_INFO pCurve);
//判断事件是否通过
void IsEventPass(OPM_WAVE wave, EVENTS_TABLE *eventsTable, float fTotalReturnLoss);
//判断事件损耗是否通过  返回1 通过  0 不通过
int IsEventLossPass(UINT32 wave, float fLoss, int iStyle);
//判断事件损耗是否通过  返回1 通过  0 不通过
int IsEventReflectPass(UINT32 wave, float fReflect, int iStyle);
//判断衰减是否通过  返回1 通过  0 不通过
int IsAttenuationPass(UINT32 wave, float fAttenuation, int iStyle);
//判断跨段损耗是否通过  返回1 通过  0 不通过
int IsRegionLossPass(UINT32 wave, float fRegionLoss);
//判断跨段长度是否通过  返回1 通过  0 不通过
int IsRegionLengthPass(UINT32 wave, float fRegionLength);
//判断跨段长度是否通过  返回1 通过  0 不通过
int IsRegionORLPass(UINT32 wave, float fRegionORL);
#ifndef MINI2
//获得衰减率
unsigned int SetAttenuation(PTASK_SETTINGS pTaskSet, int taskNum);   		      
//获得累加次数
unsigned int SetFilters(PTASK_SETTINGS pTaskSet, int taskNum);
#endif
int EnumToWave(OPM_WAVE wave);
int EnumToPulse(OPM_PULSE pulse);

//返回系统是否设置注入光纤或接收光纤 返回0 未设置
int LaunchRecvFiberEnable();

//修改参数后重新事件分析
void ReeventAnalyze(void);
#endif 

