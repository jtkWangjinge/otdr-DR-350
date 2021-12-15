/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmfip.h  
* 摘    要：  定义用于完成端面检查仪模块功能的应用程序接口
*
* 当前版本：  v1.1.0
* 作    者：
* 完成日期：  
*
*******************************************************************************/

#ifndef _APP_FRMFIP_H
#define _APP_FRMFIP_H

/*******************************************************************************
*** 					为使用GUI而需要引用的头文件			  			     ***
*******************************************************************************/
#include "guiimage.h"
#include "wnd_global.h"
#include "app_algorithm_analysis.h"

/*******************************************************************************
***							图片显示大小位置坐标宏定义			  			 ***
*******************************************************************************/
#define BIG_IMG_XPOSITION		(10)
#define BIG_IMG_YPOSITION		(50)
#define BIG_IMG_WIDTH			(500)//(517)
#define BIG_IMG_HEIGTH			(420)//(421)

/*******************************************************************************
***							保存图片名字前后缀宏定义			  			 ***
*******************************************************************************/
#define FilePrefix		    "save"
#define fileSufix	        ".gdm"
#define filefiberpdf	    ".pdf"
#define filefibergdmpath	MntDataDirectory"/Fiber_Microscope/"
#define filefiberpdfpath	MntDataDirectory"/Fiber_Microscope/report/"
#define MAX_FILE_NO			1000
#define IMAGE_WIDTH			480
#define IMAGE_HEIGHT		640
#define IMAGE_LEN 			(IMAGE_WIDTH * IMAGE_HEIGHT)
//#define SAVE_IMAGE_DATA		//保存数据的宏定义，应用于保存数据给到算法分析
#ifdef SAVE_IMAGE_DATA
#define IMAGE_DATA_FILE_PATH MntUsbDirectory"/fip_data.txt"
#endif
/*******************************************************************************
***     							类型定义			        			 ***
*******************************************************************************/
typedef void (*CALLBACKSHOWSHARPNESS)(int);

//光纤端面检查设备模块
typedef struct _OpticalFiberEndSurDetector
{		
	int iDevDetector;
	unsigned char pImgBuffer[IMAGE_LEN];
	unsigned long ulImgSize;

	int isData;
	/* mutual exclusion lock */
	GUIMUTEX detectorLock;	
} OpticalFiberEndSurDetector;

typedef struct _OpticalFiberEndSurResult
{
	int FiberCoreScracth; 					//A区划痕
	int FiberCladdingScracthSmall; 			//B区小划痕
	int FiberCladdingScracthLarge; 			//B区大划痕
	int FiberJacketScracth; 				//D区划痕

	int FiberCoreDefect; 					//A区污点
	int FiberCladdingDefectSmall; 			//B区小污点
	int FiberCladdingDefectMedium; 			//B区中污点
	int FiberCladdingDefectLarge; 			//B区大污点
	int FiberJacketDefectSmall; 			//D区小污点
	int FiberJacketDefectLarge;				//D区大污点

	int Isqualified; 						//是否合格
	int CirX; 								//圆心坐标X
	int CirY; 								//圆心坐标Y
	float magnification; 					//放大率

	unsigned char OriginalImage[IMAGE_LEN]; //原始图像
	unsigned char ProcessedImage[IMAGE_LEN]; //处理后的图像
}OpticalFiberEndSurResult;

typedef struct _OpticalFiberEndSur
{
	OpticalFiberEndSurResult ResultInfo;
	char isImage;							//是否有图像
	char isScreen;						    //是否截图
	char isConnectDevice;				    //是否连接设备
	char isAlignCenter;						//是否居中
	char isZoom;							//是否放大
	int Brightness;							//亮度值
	int Sharpness;							//清晰度
	char cName[64];							//文件名
	
	CALLBACKSHOWSHARPNESS ShowSharpness;	//显示清晰度函数
	CALLBACKSHOWSHARPNESS ShowResult;		//显示结果
		
	GUIMUTEX mutexLock;
}OpticalFiberEndSur;

typedef struct _FipIdentify
{
	char taskIdentify[32];
	char client[32];
	char company[32];
	char note[32];
	char positionA[32];
	char positionB[32];
	char cableID[32];
	char fiberID[32];
	char connectorID[32];

	int taskIdentifyIsSelect;
	int clientIsSelect;
	int companyIsSelect;
	int noteIsSelect;
	int positionAIsSelect;
	int positionBIsSelect;
	int cableIDIsSelect;
	int fiberIDIsSelect;
	int connectorIDIsSelect; 
}FipIdentify;

/*******************************************************************************
***     					对外定义接口函数			        			 ***
*******************************************************************************/

//初始化光纤端面检查设备
int InitializeDetector(void);
int openDetector(void);
//
int ReleaseFiberDevice(void);

//开始光纤端面检查设备
int StartDetector(void);

//停止光纤端面检查设备
int StopDetector(void);

//设置亮度
int ChangeBirghtness(int brightness);

//在窗口上显示光纤端面图像
int ShowPicture(int x, int y, int width,int heigth);

//检查光纤端面是否合格
int CheckPicture(int x, int y, int width, int heigth);

//获得文件名字和文件ID
int GetSaveFileName(char *file_path);

//保存gdm文件
int SaveGdmFile(int arg, void *pArg);

#ifdef SAVE_IMAGE_DATA
//保存图像原始数据
void SaveImageData(void);
#endif

//读取数据
int ReadGdmFile(const char *name, OpticalFiberEndSurResult *CheckResult);

//从252读取数据
int ReadPicture();

//显示图像在屏幕上
void DisplayOptFiberEndSur(int x, int y, int width,int height,IMG pImage);

//生成报告
int creatFipReport(char *fipReportName);
//保存报告
int SaveReport(int arg, void *pArg);

//画圆
void DrawAreaCircle(OpticalFiberEndSurResult result);

//判断是否通过
int AnalyzeResult(OpticalFiberEndSurResult result);

void showBlackImage(int x, int y, int width,int height);

//生成原始图片
int creatFipOriginalPicture(char *absolutePath, int width, int height);
//生成处理后的图片
int creatFipProcessedPicture(char *absolutePath, int width, int height);

//??LOG???SDCARD
void FIP_LOG(char *log);
void FIP_LOG1(char *file, const char *func, int line);
#endif  //_APP_FRMFIP_H
