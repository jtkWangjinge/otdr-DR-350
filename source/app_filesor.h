/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_filesor.h
* 摘    要：  定义与SOR文件相关的数据结构，声明相应的操作函数
*
* 当前版本：v1.1.0 
* 作    者：
* 完成日期：

* 取代版本： v1.0.0 
* 原 作 者： 
* 完成日期： 
*******************************************************************************/

#ifndef _APP_FILESOR_H
#define _APP_FILESOR_H

#include "app_middle.h"

/*******************************************************************************
***                     为实现对sor文件的读写引用的标准头文件                ***
*******************************************************************************/


/*******************************************************************************
***                     为实现对sor文件的读写引用的系统头文件                ***
*******************************************************************************/

#define CABLEID "CableID"
#define FIBERID "FiberID"
#define FIBERTYPE 652
#define STARTPOINT "StartPoint"
#define ENDPOINT "EndPoint"
#define CABLECODE "CabelCode"
#define OPERATOR "SHI"
#define	COMMENT "Comment"

#ifdef FORC
#define SUPPLIER_NAME	"FORC"			        //OTDR供应商
#define MAINFRAME_NAME	"FORC_MINI2"	        //OTDR主机名
#define OPTICAL_NAME	"MINI2_OPT"	            //OTDR光模块名
#else
#define SUPPLIER_NAME	"INNO"			        //OTDR供应商
#define MAINFRAME_NAME	"INNO_"PROJECT_NAME	    //OTDR主机名
#define OPTICAL_NAME	PROJECT_NAME"_OPT"	    //OTDR光模块名
#endif

#define MAINFRAME_SN	"464DG34634SDGVSD346"	//OTDR主机序列号
#define OPTICAL_SN		"643DSVGDS43643GD464"	//ODTR光模块序列号
#define FIRMWARE_VER	"1.0.0"					//软件版本号


#define FILE_NAME_MAX 512

/*******************************************************************************
***                       定义与MAP块相关的数据结构                          ***
*******************************************************************************/
//定义MAP中的块信息结构
typedef struct _map_blockinfo
{
    char *strBlockId;               //标识
    unsigned short usRevisionNum;   //版本号 
    long lBlockSize;                //块大小
} SORMAP_BLOCKINFO;


//定义MAP块结构
typedef struct _map
{
    char *strMapId;                 //映射块标识

    unsigned short usMapRevisionNu; //MAP块版本号
    long lMapSize;                  //映射块的大小
    short sNuBlock;                 //文件中块的总数，包括映射块

    SORMAP_BLOCKINFO pBlockInfo[8]; //块信息
} SORMAP;


/*******************************************************************************
***               定义与General Parameters块相关的数据结构                   ***
*******************************************************************************/
typedef struct _general 
{
    char *strBlockId;           //块标识

    char pLanguageCode[2];      //语言码 CH=chinese ...
    char *strCableId;           //电缆标识
    char *strFiberId;           //光纤标识

    short sFiberType;           //光纤类型
    short sWaveLength;          //OTDR波长

    char *strOriginatingLoca;   //测量的起始位置
    char *strTerminatingLoca;   //测量的结束位置
    char *strCableCode;         //描述电缆和光纤的信息

    char pDataFlag[2];          //光纤的测量状态 NC= new condition ...

    long lUserOffset;           //测量物理距离，用单程总时间为结果，单位为100ps
    long lUserDist;             //测量物理距离，单位依据Fixed块中pDistUints[2]成员

    char *strOperator;          //描述OTDR的操作的名字或标识
    char *strComment;           //描述电缆或光纤设计或链路问题
} SORGEN;


/*******************************************************************************
***                 定义与Supplier Parameters块相关的数据结构                ***
*******************************************************************************/
typedef struct _supplier
{
    char *strBlockId;           //块标识
    char *strSupplierName;      //OTDR供应商
    char *strMainframeId;       //OTDR主机名
    char *strMainframeSN;       //OTDR主机序列号
    char *strOpticalId;         //OTDR光模块名
    char *strOpticalSN;         //ODTR光模块序列号
    char *strSoftRevision;      //软件版本号
    char *strOther;             //其他添加的信息
} SORSUP;


/*******************************************************************************
***                定义与Fixed Parameters块相关的数据结构                    ***
*******************************************************************************/

//定义Key Events中的窗体坐标结构
typedef struct _fixed_window
{
    long lTopLeftX;       //ODTR显示窗口左上角横坐标
    long lTopLeftY;       //ODTR显示窗口左上角纵坐标
    long lBotRightX;      //ODTR显示窗口右下角横坐标
    long lBotRightY;      //ODTR显示窗口右下角纵坐标
} SOR_FIX_WIN;

//定义Fixed Parameters块结构
typedef struct _fixed
{
    char *strBlockId;               //块标识

    unsigned long ulDTS;            //光纤测量的时间戳，以1970-1-1零点整为起始
    char pDistUints[2];             //距离单位，Km，mt，ft，kf，mi，默认值为km
    short sWaveLength;              //实际波长(以纳米为单位)
    long lAcquisitionOffset;        //激光器到第一个数据的时间(以100皮秒为单位)
    long lOffsetDist;               //激光器到第一个数据的距离

    short sPulesTotal;              //所使用的脉宽总数
    short *pPulseWidth;             //各脉宽值(以纳秒为单位)
    long *pDataSpacing;             //数据间距，1万个数据耗费的时间(以100皮秒为单位)
    long *pPointsNumber;            //每个脉宽所产生的数据量

    long lGroupIndex;               //群折射律，146800代表1.46800,为实际值100000倍
    short sBackscatterCode;         //反向散射系数,为实际值10倍

    long lAveragesNumber;           //平均次数
    unsigned short usAverageTime;   //平均时间，以秒为单位
    
    long lAcquisitionRange;         //OTDR测量的单程总长度，(以100皮秒为单位)
    long lAcquisitionDist;          //OTDR测量的单程总长度，以pDistUints[2]为单位
    long lFrontoffset;              //前面板偏移，(以100皮秒为单位)

    unsigned short usNoiseFloor;    //噪声基值，代表负值，55000代表-55.000dB，实际值乘以-1000
    short sNoiseScale;              //噪声基值比例因子，1000代表1.000，

    unsigned short usPowerOffset;   //衰减，实际值乘以1000，
    unsigned short usLossThreshold; //损耗阈值，实际值乘以1000，
    unsigned short usRefThreshold;  //反射阈值，负值，实际值乘以-1000，
    unsigned short usEndThreshold;  //末端阈值

	char strTraceType[2];			//波形的类型
    SOR_FIX_WIN WndCoordinate;    //窗口坐标
} SORFXD;


/*******************************************************************************
***                  定义与Key Events块相关的数据结构                        ***
*******************************************************************************/
//定义Key Events中的事件信息结构
typedef struct _event_event
{
    short sEventNum;            //事件编号
    long lPropagationTime;      //从链路起始点至事件点的时间(以100皮秒为单位)

    short sAttenuationCoef;     //衰减系数，实际值乘以1000
    short sEventLoss;           //事件损耗，实际值乘以1000
    long lEventReflectance;     //事件反射，实际值乘以1000

    char pEventCode[6];         //事件描述
    char pLossMeasurement[2];   //损耗测量方法

    long lMarkLoca1;            //靠近OTDR端的事件标记 (以100皮秒为单位)
    long lMarkLoca2;            //靠近OTDR端的事件标记
    long lMarkLoca3;            //靠近光纤末端的事件标记 
    long lMarkLoca4;            //靠近光纤末端的事件标记 
    long lMarkLoca5;            //标记计算反射律事件的位置 

    char *strComment;           //用户定义的事件信息
} SOREVT_EVENT;

//定义Key Events中的位置结构
typedef struct _event_position
{
    long lStartPosition;    //起始标记点位置(以100皮秒为单位)
    long lFinishPosition;   //结束标记点位置(以100皮秒为单位)
} SOREVT_POSITION;

//定义Key Events块结构
typedef struct _sor_event
{
    char *strBlockId;               //块标识

    short sEventTotal;              //关键事件的数量

    SOREVT_EVENT *pEventInfo;       //事件信息

    long lEndtoendLoss;             //链路端到端损耗，实际值乘以1000
    SOREVT_POSITION EndtoendMarker; //端到端损耗的标记位置(以100皮秒为单位)
    unsigned short usReturnLoss;    //回波损耗，实际值乘以1000
    SOREVT_POSITION ReturnMarker;   //回波损耗的标记位置(以100皮秒为单位)
} SOREVT;



/*******************************************************************************
***                  定义与Link Parameters块相关的数据结构                   ***
*******************************************************************************/
//定义Link Parameters中的地标结构
typedef struct _link_landmark
{
    short sLandmarkNum;     //地标编号
    char pLandmarkCode[2];  //地标类型
    long lLandmarkLoca;     //链路开始到该地标的光单程距离(以100皮秒为单位)
    short sEventNum;        //与该地标相关的事件编号

    long lGpsLongitude;     //GPS经度信息
    long lGpsLatitude;      //GPS纬度信息

    short sFCI;             //电缆光路长度与电缆包层长度百分比
    long lSMI;              //地标设定前的电缆包层的编号
    long lSML;              //地标设定后的电缆包层编号
    char pDistUints[2];     //电缆包层标记的距离单位
    short sMFD;             //光纤的模场直径 

    char *strComment;       //地标的注释
} SORLNK_LANDMARK;


//定义Link Parameters块结构
typedef struct _link
{
    char *strBlockId;               //块标识
    short sLandmarksTotal;          //地标总数量
    SORLNK_LANDMARK *pLandmarkInfo; //地标信息  
} SORLNK;


/*******************************************************************************
***                        定义与Data Points块相关的数据结构                 ***
*******************************************************************************/
//定义Data Points中的比例因子结构
typedef struct _data_scalefactor
{
    long lPointsTotal;              //使用该比列因子的数据点的总数
    short sScaleFactor;             //比列因子，实际值乘以1000
    unsigned short *pDataPoints;    //使用比列因子n的每个数据点的相对功率水平，实际值乘以sScaleFactor，且65536-翻转
} SORDAT_SCALEFACTOR;


//定义Data Points块结构
typedef struct _data
{
    char *strBlockId;                   //块标识
    long lPointsTotal;                  //数据点的总数
    short sFactorsTotal;                //比列因子的总数
    SORDAT_SCALEFACTOR *pFactorInfo;    //比例因子信息
} SORDAT;


/*******************************************************************************
***               定义与Special Proprietary块相关的数据结构                  ***
*******************************************************************************/
typedef struct _special
{
    char *strBlockId;   //块标识
    char *strInfo;		//自定义信息
} SORSPC;


/*******************************************************************************
***                      定义与Check Sum块相关的数据结构                     ***
*******************************************************************************/
typedef struct _check
{
    char *strBlockId;           //块标识
    unsigned short usCheckSum;  //校验和
} SORCHK;


/*******************************************************************************
***                      定义与pSor块相关的数据结构                     ***
*******************************************************************************/
//事件信息 为了保持与otdr事件结构体一致
typedef struct _sor_events_info//最终处理结果事件信息;
{
	int iBegin;//事件起点;
	int iEnd;//事件终点;
	int iStyle;//事件类型 事件类型定义：0：损耗事件；1：增益事件；2：反射事件；3：回波事件；4：复合事件；10+x：末端事件;
	int iPulse;//该事件是在哪个脉宽下测得的
	float fLoss;//事件损耗;
	float fDelta;//最大最小值衰减;
	float fReflect; //反射率;
	int iContinueLength;//后续连续光纤长度;
	float fAttenuation;//衰减率;
	float fContinueAttenuation;//后续光纤损耗;
	float fTotalLoss;//累加损耗;
	float fEventsPosition;//事件点实际位置 = iBegin * g_fLengthPerPoint
	float fEventsEndPosition;//事件点实际位置 = iEnd * g_fLengthPerPoint
	float fEventReturnLoss;//事件回波损耗
	int iWaveSymbol;//该事件是有哪些波长测得的(主要用于SOLA多波长测试)
}SOR_EVENTS_INFO;

typedef struct _sor_events_table//最终处理事件表信息;
{
	SOR_EVENTS_INFO EventsInfo[500];                //事件信息;
	int iEventsStart;	                            //参考起始点
	int iEventsNumber;	                            //事件数量;
	int iLaunchFiberIndex;                          //注入光纤事件索引 <= 0 表明没有注入光纤
    int iRecvFiberIndex;                            //接收光纤事件索引 <= 0 表明没有接收光纤
    int iIsThereASpan;                              //是否存在跨段（0 不存在 非0 存在）
}SOR_EVENTS_TABLE;

//ger和sup块一些信息
typedef struct _general_info
{
	char pComment[128];
	char pCompanyName[128];
	char pOperator[128];
	char pCableID[128];
	char pFiberID[128];
	short sFiberType;
	char pCableCode[128];
	char pStartPoint[128];
	char pEndPoint[128];
	char pDataFlag[2];
} GENERAL_INFO;

//对外接口使用的数据结构
typedef struct _sor
{
	int 	iWave;						//波长，1310 或 1550
	int     iPulse;						//脉宽，以ns为单位，例如5ns，10ns，100ns......
	int 	iAverageTime;				//测量的平均时间
	long  	lAcquisitionDist;			//测量时候所设置的距离量程，单位m(米)

	char 	cFileName[FILE_NAME_MAX];	//文件名
		
    int 	iEventAnalysisFlag;			//时间分析标志位，1已经分析过，0没有

	float   fSmpRatio;					//采样率
    long    *pDataSpacing;              //数据间距，1万个数据耗费的时间(以100皮秒为单位)
	
	float   fRefractive;				//折射率 标准是1.46000
	float 	fBackScattering;			//背向(反向)散射系数

	int 	iLossMethodMark;			//损耗标记模块中近似方法，0代表LSA，1代表TPA
	float 	fSpliceLossThr;				//损耗检测阈值
	float	fReturnLossThr;				//回波损耗阈值
	float	fEndLossThr;				//末端损耗阈值

	int 	iDataCount;					//采集的数据点个数
	int 	iScaleFactor;				//比例因子
	unsigned short *pData;				//数据

	SOR_EVENTS_TABLE eventTable;		//事件列表，尽量在成员结构上和OTDR那边保持一致

	GENERAL_INFO generalInfo;			//标签信息
} SOR_DATA, *PSOR_DATA;


/***
  * 功能：
  		保存测试数据到SOR文件中
  * 参数：
  		1、char *pSorDirPath:	即将保存的完整路径名
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
		返回值错误描述:       	返回值
		传入指针为空:         	-1
		打开文件失败:         	-2
		InitWriteToSor:         -3 
  		WriteMap:				-4
  		WriteGeneral:			-5
  		WriteSupplier:			-6
  		WriteFixed:				-7
  		WriteKeyevent:			-8
  		WriteDatapoint:			-9
  		WriteSpecial:			-10
  		WriteChecksum:			-11
  		WriteCrc:				-12,-13,-14,-15,-16,-17
  		
 ***/
int WriteToSor(PSOR_DATA pSor);


/***
  * 功能：
	  从SOR文件读取数据
  * 参数：
  * 返回：
        成功返回零，失败返回非零.
  * 备注：
  		返回值错误描述:       		返回值
		打开文件失败:         		-1
		获取文件信息失败:         	-2
		文件size太小:               -3
		申请每个块的资源失败:       -4
		ReadMapBlock失败:			-5
		ReadGeneral失败:			-6
		ReadSupplier失败:			-7
		ReadFixed失败:				-8
		Event和Dada块都为空:		-9

 ***/
int ReadFromSor(char *FileName, PSOR_DATA pSor);


#endif  //_APP_FILESOR_H

