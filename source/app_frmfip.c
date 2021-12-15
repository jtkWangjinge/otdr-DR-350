/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmfip.c  
* 摘    要：  实现用于完成端面检查仪模块功能的应用程序接口
*
* 当前版本：  v1.1.0
* 作    者：  
* 完成日期：  
*
*******************************************************************************/

#include "app_frmfip.h"
#include <limits.h>

/*******************************************************************************
***                为实现app_frmfip而需要引用的系统头文件                  ***
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

/*******************************************************************************
***                为实现app_frmfip而需要引用的其他头文件                  ***
*******************************************************************************/
#include "app_middle.h"
#include "guibitmap.h"
#include "app_global.h"
#include "app_screenshots.h"
#include "app_frmfilebrowse.h"
#include "app_parameter.h"
#include "app_pdfreportfip.h"
#include "app_screenshots.h"
#include "app_uvc.h"

/*******************************************************************************
***                   为实现app_frmfip而定义的宏定义                       ***
*******************************************************************************/
#define b16toRGB(data) ((((data)&0xf8)>>3) | (((data)&0xfc)<<3) |(((data)&0xf8)<<8))
#define F252_START          _IO('p', 0x01)
#define F252_STOP           _IO('p', 0x02)
#define F252_SYNC_NOTIFY    _IO('p', 0x03)
#define F252_READ_IMG_INFO  _IOR('p',0x04, struct img_info) /* Read image info*/
#define F252_SET_EXPOSURE 	_IOW('p', 0x04, int) 
#define DEV_INNO_INSPECT    "/dev/inno_f252"
#define LINE_LEN 			(IMAGE_HEIGHT)

//#define FIPLOG
//#define FIPLOG1
/*******************************************************************************
***                            自定义的本地全局变量                          ***
*******************************************************************************/
static OpticalFiberEndSurDetector detector;

OpticalFiberEndSur OptFiberEndSurCheck;
extern FipIdentify fipIdentify;
extern PSYSTEMSET 		pCurSystemSet;
/*******************************************************************************
***                                内部调用函数                              ***
*******************************************************************************/
//同步端面检查设备
// static int SyncDetector(void);


/***
  * 功能：
		同步端面检查设备
  * 参数：
		无
  * 返回：
		0
  * 备注：	
***/
// static int SyncDetector(void)
// {
// 	int iRet = 0;

// 	iRet = ioctl(detector.iDevDetector, F252_SYNC_NOTIFY, 0);

// 	return iRet;
// }

//打印LOG信息到SDCARD
#ifdef FIPLOG
void FIP_LOG(char *log)
{
	const char *wifi_log = MntDataDirectory"/fip_log.txt";
	FILE *stream;

	if(!log)
		return;

	stream = fopen(wifi_log,"a+");

	if(stream == NULL )
		return;
	
	fprintf(stream,"%s\n",log);
	fclose(stream);
}
#endif


#ifdef FIPLOG1
void FIP_LOG(char *file, const char *func, int line)
{
	const char *wifi_log = MntDataDirectory"/fip_log.txt";
	FILE *stream;

	stream = fopen(wifi_log,"a+");

	if(stream == NULL )
		return;
	
	fprintf(stream,"%s----%s----%d\n",file,func,line);
	fclose(stream);
}
#endif
/*******************************************************************************
***                            对外接口函数定义                              ***
*******************************************************************************/

//显示图像在屏幕上
void DisplayOptFiberEndSur(int x, int y, int width,int height,IMG pImage)
{
	GUIFBMAP *pFb = NULL;
	unsigned short *p = NULL;
	unsigned short *tmp = NULL;
	unsigned short *tmpcpy = NULL;
	int w = 0;
	int h = 0;
	int i = 0;
	unsigned char *d;
	
	if(pImage == NULL)
	{
		return;
	}
	d = pImage;
	
	tmp = (unsigned short *)GuiMemAlloc(sizeof(unsigned short) * (width * height));
	tmpcpy = tmp;
	
	pFb = GetCurrFbmap();
	p = pFb->pMapBuff;
	p += x + y*pFb->uiHorRes;
	//d += ((LINE_LEN - height) / 2) * LINE_LEN;
	for (h = height; h != 0; --h)
	{
		i = (LINE_LEN - width)/2;
		for (w = width; w != 0; --w)
		{
			*tmp++ = b16toRGB(d[i]);
			i++;
		}
		d += LINE_LEN;
	}
	tmp = tmpcpy;
	for (h = height; h != 0; --h)
	{
		memcpy(p, tmp, width * sizeof(unsigned short));
		p += WINDOW_WIDTH;
		tmp += width;
	}
	//RefreshScreen(__FILE__, __func__, __LINE__);
	
	if(tmpcpy != NULL)
	{
		GuiMemFree(tmpcpy);
		tmpcpy = NULL;
	}
}

void fipDrawCircle(unsigned int color, int point_x, int point_y, int radius,
		int area_start_x, int area_start_y, int area_end_x, int area_end_y)
{
    UINT16 d = (UINT16)color;
    GUIFBMAP *pFbmap = NULL;
    UINT16 *p = NULL;
	UINT16 *start = NULL;
    int xp, yp, dy, dp, x, y, sym1, sym2, sym3, sym4;

	if(area_start_x > area_end_x || area_start_y > area_end_y)
	{
		return;
	}

	pFbmap = GetCurrFbmap();
	MutexLock(&(pFbmap->Mutex));
	p = pFbmap->pMapBuff;
	start = pFbmap->pMapBuff;
    point_x += pFbmap->uiHorOff;
    point_y += pFbmap->uiVerOff;
    p += point_x + point_y * pFbmap->uiHorRes;
    xp = 0;
    yp = radius * 256 ;
    dy = WINDOW_WIDTH - yp;
    for (dp = xp - yp; dp <= 0; dp = xp - yp)
    {
        x = xp >> 8;
        y = yp >> 8;
        sym1 = y * pFbmap->uiHorRes + x;
        sym2 = sym1 - 2 * x;
        sym3 = x * pFbmap->uiHorRes + y;
        sym4 = sym3 - 2 * y;

		if(((p + sym1 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p + sym1 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p + sym1 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p + sym1 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p + sym1) = d;
		}		
		if(((p + sym2 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p + sym2 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p + sym2 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p + sym2 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p + sym2) = d;
		}		
		if(((p - sym1 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p - sym1 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p - sym1 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p - sym1 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p - sym1) = d;
		}		
		if(((p - sym2 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p - sym2 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p - sym2 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p - sym2 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p - sym2) = d;
		}		
		if(((p + sym3 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p + sym3 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p + sym3 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p + sym3 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p + sym3) = d;
		}		
		if(((p + sym4 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p + sym4 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p + sym4 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p + sym4 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p + sym4) = d;
		}		
		if(((p - sym3 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p - sym3 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p - sym3 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p - sym3 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p - sym3) = d;
		}
		if(((p - sym4 - start)%pFbmap->uiHorRes > area_start_x) 
			&& ((p - sym4 - start)%pFbmap->uiHorRes < area_end_x)
			&& ((p - sym4 - start)/pFbmap->uiHorRes > area_start_y)
			&& ((p - sym4 - start)/pFbmap->uiHorRes < area_end_y))
		{
        	*(p - sym4) = d;
		}		

        if (dy < 0)
        {
            dy += 2 * xp + 768;
        }
        else
        {
            dy += 2 * dp + 1280;
            yp -= 256;
        }
        xp += 256;
    }
	MutexUnlock(&(pFbmap->Mutex));
}

//画圆
void DrawAreaCircle(OpticalFiberEndSurResult result)
{	
	unsigned int uiColorRed = ((0X0000FF & 0x0000F8) >> 3) |		//RGB565
			  				  ((0X0000FF & 0x00FC00) >> 5) |		//RGB565
			  				  ((0X0000FF & 0xF80000) >> 8); 		//RGB565
	unsigned int uiColorBlue = ((0XB55A00 & 0x0000F8) >> 3) |		//RGB565
			  				   ((0XB55A00 & 0x00FC00) >> 5) |		//RGB565
			  				   ((0XB55A00 & 0xF80000) >> 8); 		//RGB565
	unsigned int uiColorGreen = ((0X8EF702 & 0x0000F8) >> 3) |		//RGB565
			  				    ((0X8EF702 & 0x00FC00) >> 5) |		//RGB565
			  				    ((0X8EF702 & 0xF80000) >> 8); 		//RGB565
	unsigned int uiColorYellow = ((0X66FFFF & 0x0000F8) >> 3) |		//RGB565
			  				     ((0X66FFFF & 0x00FC00) >> 5) |		//RGB565
			  				     ((0X66FFFF & 0xF80000) >> 8); 		//RGB565
	unsigned int uiColorSkyblue = ((0XFFFF33 & 0x0000F8) >> 3) |	//RGB565
			  				      ((0XFFFF33 & 0x00FC00) >> 5) |	//RGB565
			  				      ((0XFFFF33 & 0xF80000) >> 8); 	//RGB565

	int office_x = BIG_IMG_XPOSITION - (IMAGE_WIDTH - BIG_IMG_WIDTH) / 2;
	int office_y = BIG_IMG_YPOSITION - (IMAGE_HEIGHT - BIG_IMG_HEIGTH) / 2;

	fipDrawCircle(uiColorRed,result.CirX + office_x, result.CirY + office_y, 5 * GetZoomScale(),
					BIG_IMG_XPOSITION,BIG_IMG_YPOSITION,
					BIG_IMG_XPOSITION+BIG_IMG_WIDTH,
					BIG_IMG_YPOSITION+BIG_IMG_HEIGTH);

	fipDrawCircle(uiColorBlue,result.CirX + office_x, result.CirY + office_y, 18 * GetZoomScale(),
					BIG_IMG_XPOSITION,BIG_IMG_YPOSITION,
					BIG_IMG_XPOSITION+BIG_IMG_WIDTH,
					BIG_IMG_YPOSITION+BIG_IMG_HEIGTH);

	fipDrawCircle(uiColorGreen,result.CirX + office_x, result.CirY + office_y, 78 * GetZoomScale(),
					BIG_IMG_XPOSITION,BIG_IMG_YPOSITION,
					BIG_IMG_XPOSITION+BIG_IMG_WIDTH,
					BIG_IMG_YPOSITION+BIG_IMG_HEIGTH);

	fipDrawCircle(uiColorYellow,result.CirX + office_x, result.CirY + office_y, 93 * GetZoomScale(),
					BIG_IMG_XPOSITION,BIG_IMG_YPOSITION,
					BIG_IMG_XPOSITION+BIG_IMG_WIDTH,
					BIG_IMG_YPOSITION+BIG_IMG_HEIGTH);

	fipDrawCircle(uiColorSkyblue,result.CirX + office_x, result.CirY + office_y, 171 * GetZoomScale(),
					BIG_IMG_XPOSITION,BIG_IMG_YPOSITION,
					BIG_IMG_XPOSITION+BIG_IMG_WIDTH,
					BIG_IMG_YPOSITION+BIG_IMG_HEIGTH);

	//RefreshScreen(__FILE__, __func__, __LINE__);
}

/*
检查是否通过
返回0 未通过
返回1 通过
*/
int AnalyzeResult(OpticalFiberEndSurResult result)
{
	if(result.FiberCoreScracth > 0 
		|| result.FiberCoreDefect > 0
		|| result.FiberCladdingScracthLarge > 0
		|| result.FiberCladdingDefectMedium > 5
		|| result.FiberCladdingDefectLarge > 0
		|| result.FiberJacketDefectLarge > 0)
	{
		return 0;
	}
	LOG(LOG_INFO, "**************pass****************\n");
	return 1;
}

/***
  * 功能：
		检查光纤端面是否合格
  * 参数：
  		int x: 初始横坐标
  		int y: 初始纵坐标
  		int width: 宽度
  		int height: 高度
  * 返回：
		1: 合格
		2: 不合格
		0: 没找到光纤端面
		-1: 无图像
  * 备注：	
***/
int CheckPicture(int x, int y, int width, int height)
{
	int iRet = 0;
	SIZED ImageSize;
	RESULT CheckPicResult;

	memset(&CheckPicResult,0,sizeof(CheckPicResult));
	
	ImageSize.cx = IMAGE_WIDTH;
	ImageSize.cy = IMAGE_HEIGHT;
	if(OptFiberEndSurCheck.isImage)
	{
		MutexLock(&OptFiberEndSurCheck.mutexLock);
		OptFiberEndSurCheck.isScreen = 1;
		iRet = GetCheckPic(OptFiberEndSurCheck.ResultInfo.OriginalImage,
					ImageSize,
					OptFiberEndSurCheck.ResultInfo.ProcessedImage,
					&CheckPicResult);
		MutexUnlock(&OptFiberEndSurCheck.mutexLock);

		OptFiberEndSurCheck.ResultInfo.Isqualified = 2;
	
		OptFiberEndSurCheck.ResultInfo.FiberCoreScracth = CheckPicResult.ARegionScracthNum;
		OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthSmall = CheckPicResult.BRegionSmallScracthNum;
		OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge = CheckPicResult.BRegionLargeScracthNum;
		OptFiberEndSurCheck.ResultInfo.FiberJacketScracth = CheckPicResult.DRegionScracthNum;
		
		OptFiberEndSurCheck.ResultInfo.FiberCoreDefect = CheckPicResult.ARegionDefectNum;
		OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectSmall = CheckPicResult.BRegionSmallDefectNum;
		OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium = CheckPicResult.BRegionMiddleDefectNum;
		OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge = CheckPicResult.BRegionLargeDefectNum;
		OptFiberEndSurCheck.ResultInfo.FiberJacketDefectSmall = CheckPicResult.DRegionSmallDefectNum;
		OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge = CheckPicResult.DRegionLargeDefectNum;
		
		OptFiberEndSurCheck.ResultInfo.magnification = GetZoomScale();

		OptFiberEndSurCheck.ResultInfo.CirX = CheckPicResult.Cir.y;
		OptFiberEndSurCheck.ResultInfo.CirY= CheckPicResult.Cir.x;

		if(iRet == 0)
		{
			if(AnalyzeResult(OptFiberEndSurCheck.ResultInfo))
			{
				OptFiberEndSurCheck.ResultInfo.Isqualified = 1;
			}
		}
		else
		{
			OptFiberEndSurCheck.ResultInfo.CirX = -1;
			OptFiberEndSurCheck.ResultInfo.CirY = -1;
		}
		
		/*printf("OptFiberEndSurCheck.ResultInfo.Isqualified = %d\n",OptFiberEndSurCheck.ResultInfo.Isqualified);

		printf("OptFiberEndSurCheck.ResultInfo.FiberCoreScracth = %d\n",OptFiberEndSurCheck.ResultInfo.FiberCoreScracth);
		printf("OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthSmall = %d\n",OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthSmall);
		printf("OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge = %d\n",OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge);
		printf("OptFiberEndSurCheck.ResultInfo.FiberJacketScracth = %d\n",OptFiberEndSurCheck.ResultInfo.FiberJacketScracth);
		printf("OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectSmall = %d\n",OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectSmall);
		printf("OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium = %d\n",OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium);
		printf("OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge = %d\n",OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge);
		printf("OptFiberEndSurCheck.ResultInfo.FiberJacketDefectSmall = %d\n",OptFiberEndSurCheck.ResultInfo.FiberJacketDefectSmall);
		printf("OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge = %d\n",OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge);
		printf("OptFiberEndSurCheck.ResultInfo.magnification = %f\n",OptFiberEndSurCheck.ResultInfo.magnification);
		printf("OptFiberEndSurCheck.ResultInfo.CirX = %d\n",OptFiberEndSurCheck.ResultInfo.CirX);
		printf("OptFiberEndSurCheck.ResultInfo.CirY = %d\n",OptFiberEndSurCheck.ResultInfo.CirY);
		*/
		DisplayOptFiberEndSur(x, y, width, height, OptFiberEndSurCheck.ResultInfo.ProcessedImage);
		
		if(!iRet)
		{
			DrawAreaCircle(OptFiberEndSurCheck.ResultInfo);
		}
		
		if(OptFiberEndSurCheck.ShowResult)
		{
			(*OptFiberEndSurCheck.ShowResult)(OptFiberEndSurCheck.ResultInfo.Isqualified);
		}
	    RefreshScreen(__FILE__, __func__, __LINE__);

		if(!iRet)
		{
			if(OptFiberEndSurCheck.ResultInfo.Isqualified == 1)
			{
				return 1;
			}
			else
			{
				return 2;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
	
}

/***
  * 功能：
		从252读取数据
  * 参数：
  		int x: 初始横坐标
  		int y: 初始纵坐标
  		int width: 宽度
  		int height: 高度
  * 返回：
		0: 成功
		负数: 失败
  * 备注：	
***/
int ReadPicture()
{			
	int iRet = 0;
#ifdef FIP_DEVICE
	static int count = 0;
	//unsigned char *d_cp = NULL;
	fd_set fds;
	struct timeval tv, tv_start, tv_end;

	FD_ZERO(&fds);
	FD_SET(detector.iDevDetector, &fds);

	count ++;
	if (count == 1)
		gettimeofday(&tv_start, NULL);

	if (count == 100) {
		count = 0;
		gettimeofday(&tv_end, NULL);
		LOG(LOG_INFO, "FPS is %ld\n", 100 / (tv_end.tv_sec - tv_start.tv_sec));
	}
	
	tv.tv_sec = 0;
	tv.tv_usec = (500*1000);

	//检查是否可读，如果成功则读，否则说明通信有误，有可能是usb被拔了，返回-1
	if (select(detector.iDevDetector+1, &fds, NULL, NULL, &tv) >0)
	{
		MutexLock(&detector.detectorLock);
		iRet = uvc_readFrame(detector.iDevDetector, detector.pImgBuffer,
							 detector.ulImgSize);
		MutexUnlock(&detector.detectorLock);
		if (iRet != detector.ulImgSize)
		{
			ReleaseFiberDevice();
			return iRet;
		}
		detector.isData = 1;
	}
	else
	{	
		iRet = open(UVCFILE, O_RDWR);
		if(iRet < 0)
		{
			ReleaseFiberDevice();
			return iRet;
		}
		else
		{
			close(iRet);
		}
	}
#endif
	ThreadTestcancel();
	return iRet;
}

/***
  * 功能：
		在窗口上显示光纤端面图像
  * 参数：
  		int x: 初始横坐标
  		int y: 初始纵坐标
  		int width: 宽度
  		int height: 高度
  * 返回：
		0: 成功
		负数: 失败
  * 备注：	
***/
int ShowPicture(int x, int y, int width,int height)
{			
	if(!OptFiberEndSurCheck.isScreen)
	{
		if(detector.isData)
		{
			detector.isData = 0;
			SIZED ImageSize;
			ImageSize.cx = IMAGE_WIDTH;
			ImageSize.cy = IMAGE_HEIGHT; 	
			int brightness = 0;
			if(!MutexTrylock(&OptFiberEndSurCheck.mutexLock))
			{
				MutexLock(&detector.detectorLock);
				memcpy(OptFiberEndSurCheck.ResultInfo.OriginalImage,detector.pImgBuffer,detector.ulImgSize);
				MutexUnlock(&detector.detectorLock);

                struct timeval tv_start, tv_end;
                gettimeofday(&tv_start, NULL);
                GetResultPic(OptFiberEndSurCheck.ResultInfo.OriginalImage,
								ImageSize,
								OptFiberEndSurCheck.ResultInfo.ProcessedImage,
								&brightness,
								&OptFiberEndSurCheck.Sharpness);
                gettimeofday(&tv_end, NULL);
                printf("GetResultPic------%ld\n", ((tv_end.tv_sec - tv_start.tv_sec) * 1000000 + tv_end.tv_usec - tv_start.tv_usec));

                MutexUnlock(&OptFiberEndSurCheck.mutexLock);	
				OptFiberEndSurCheck.isImage = 1;
				ThreadTestcancel();
				DisplayOptFiberEndSur(x, y, width, height, OptFiberEndSurCheck.ResultInfo.ProcessedImage);
				
				if(OptFiberEndSurCheck.ShowSharpness)
				{
					(*OptFiberEndSurCheck.ShowSharpness)(OptFiberEndSurCheck.Sharpness);
				}
				RefreshScreen(__FILE__, __func__, __LINE__);
			}
		}

		if(!OptFiberEndSurCheck.isConnectDevice)
		{
            showBlackImage(BIG_IMG_XPOSITION,BIG_IMG_YPOSITION,BIG_IMG_WIDTH,BIG_IMG_HEIGTH);
			OptFiberEndSurCheck.isImage = 0;
			RefreshScreen(__FILE__, __func__, __LINE__);
            MsecSleep(200);
		}
		
	}
	else
	{
		MsecSleep(100);
	}
	ThreadTestcancel();
	
	return 0;
}

void showBlackImage(int x, int y, int width,int height)
{
    //临时变量定义
    GUIPEN *pPen;
	unsigned int uiColorBack;
	
    unsigned int StartX = x;
    unsigned int StartY = y;
    unsigned int EndX = x + width - 1;
    unsigned int EndY = y + height - 1;
    
	pPen = GetCurrPen();
	uiColorBack = pPen->uiPenColor;
	pPen->uiPenColor = 0x000000; 
	DrawBlock(StartX, StartY, EndX, EndY);
	pPen->uiPenColor = uiColorBack;	
}

/***
  * 功能：
		开始光纤端面检查设备
  * 参数：
  		空
  * 返回：
		0
  * 备注：	
***/
int StartDetector(void)
{
	int iRet = 0;

	iRet = ucv_start(detector.iDevDetector);
	
	return iRet;
}


/***
  * 功能：
		停止光纤端面检查设备
  * 参数：
  		空
  * 返回：
		0
  * 备注：	
***/
int StopDetector(void)
{
	int iRet = 0;

	iRet = ucv_stop(detector.iDevDetector);
	
	return iRet;
}

/***
  * 功能：
		设置亮度
  * 参数：
  		空
  * 返回：
  		成功返回 0
		失败返回 非0
  * 备注：	
***/
int ChangeBirghtness(int brightness)
{
	int iRet = 0;

	iRet = ioctl(detector.iDevDetector, F252_SET_EXPOSURE, brightness);
	if(iRet != 0)
	{
		LOG(LOG_ERROR, "Change Birghtness error\n");
	}

	return iRet;
}

/***
  * 功能：
		打开光纤端面检查设备
  * 参数：
  		空
  * 返回：
		0: 成功
		-1: 失败
  * 备注：	
***/
int openDetector(void)
{
	static int iRet = 0;
	int fd = -1;
	
	fd = uvc_open(UVCFILE, O_RDWR);
	
	if (fd < 0)
	{
	    if(iRet >= 0)
	    {
			LOG(LOG_ERROR, "open " UVCFILE " failed!\n");
		}    
		iRet = -1;
	}
	else
	{
		LOG(LOG_INFO, "open " UVCFILE " Success!\n");
		detector.iDevDetector = fd;
		detector.isData = 0;
		
		iRet = StartDetector();
		
		// ChangeBirghtness(OptFiberEndSurCheck.Brightness);
		if (!iRet)
		{
			detector.ulImgSize = IMAGE_LEN;
			memset(detector.pImgBuffer,0,detector.ulImgSize);
			OptFiberEndSurCheck.isImage = 1;
		}
	}

	return iRet;
}

/***
  * 功能：
		初始化光纤端面检查设备
  * 参数：
  		空
  * 返回：
		0: 成功
		-1: 失败
  * 备注：	
***/
int InitializeDetector(void)
{
	int iRet = 0;
	static int isFirst = 1;

	if(isFirst)
	{
		isFirst = 0;
		OptFiberEndSurCheck.isAlignCenter = 0;
		OptFiberEndSurCheck.Brightness = 4;
		OptFiberEndSurCheck.isZoom = 0;
        OptFiberEndSurCheck.isScreen = 0;
		
		SetAutoCenter(OptFiberEndSurCheck.isAlignCenter);
		SetZoom(1.0);

		InitMutex(&(detector.detectorLock), NULL);
		InitMutex(&(OptFiberEndSurCheck.mutexLock),NULL);
		
		memset(&fipIdentify,0,sizeof(FipIdentify));
	}
	else
	{
		MutexTrylock(&(detector.detectorLock));	
		MutexUnlock(&(detector.detectorLock));	
	}
#ifdef FIP_DEVICE
    OptFiberEndSurCheck.isConnectDevice = openDetector() ? 0 : 1;
#endif
    //OptFiberEndSurCheck.isScreen = 0;
	return iRet;
}


/***
  * 功能：
		创建gdm文件
  * 参数：
  		name:文件名(绝对路径名)
  		CheckResult:检查结果
  * 返回：
		0: 成功
		非0: 失败
  * 备注：	
***/
int CreatFile(char *name, OpticalFiberEndSurResult CheckResult)
{
	FILE *fp;
	if(!name)
	{
		return -1;
	}

	fp = fopen(name, "w+");
	if(fp == NULL)
	{
		LOG(LOG_ERROR, "fopen %s failed !!!\n", name);
		return -1;
	}

	if(!fwrite(&CheckResult,sizeof(OpticalFiberEndSurResult),1,fp))
	{
		LOG(LOG_ERROR, "fwrite data to gdm file failed!!!\n");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

void printGdfFile(OpticalFiberEndSurResult *result)
{
    if(result)
    {
		LOG(LOG_INFO, "FiberCoreScracth %d\n", result->FiberCoreScracth);
		LOG(LOG_INFO, "FiberCladdingScracthSmall %d\n", result->FiberCladdingScracthSmall);
		LOG(LOG_INFO, "FiberCladdingScracthLarge %d\n", result->FiberCladdingScracthLarge);
		LOG(LOG_INFO, "FiberJacketScracth %d\n", result->FiberJacketScracth);
		LOG(LOG_INFO, "FiberCoreDefect %d\n", result->FiberCoreDefect);
		LOG(LOG_INFO, "FiberCladdingDefectSmall %d\n", result->FiberCladdingDefectSmall);
		LOG(LOG_INFO, "FiberCladdingDefectMedium %d\n", result->FiberCladdingDefectMedium);
		LOG(LOG_INFO, "FiberCladdingDefectLarge %d\n", result->FiberCladdingDefectLarge);
		LOG(LOG_INFO, "FiberJacketDefectSmall %d\n", result->FiberJacketDefectSmall);
		LOG(LOG_INFO, "FiberJacketDefectLarge %d\n", result->FiberJacketDefectLarge);
		LOG(LOG_INFO, "Isqualified %d\n", result->Isqualified);
		LOG(LOG_INFO, "CirX %d\n", result->CirX);
		LOG(LOG_INFO, "CirY %d\n", result->CirY);
		LOG(LOG_INFO, "magnification %f\n", result->magnification);
	}
}

/*!
 * 获得指定路径文件大小
 * @param[in]  path 文件路径
 * @return     文件大小（字节）
 */
static unsigned long getFileSize(const char *path)
{
    unsigned long fileSize = -1;
    struct stat statBuff;

    if (stat(path, &statBuff) < 0)
    {
        return 0;
    }
    else
    {
        fileSize = statBuff.st_size;
    }

    return fileSize;
}

/***
  * 功能：
		读取gdm文件
  * 参数：
  		name:文件名(绝对路径名)
  		CheckResult:检查结果
  * 返回：
		0: 成功
		非0: 失败
  * 备注：	
***/
int ReadGdmFile(const char *name, OpticalFiberEndSurResult *CheckResult)
{
	FILE *fp;
	int readSize = 0;
	unsigned long fileSize = 0;

	OpticalFiberEndSurResult buff;
	if(!name || !CheckResult)
	{
		return -1;
	}
	fileSize = getFileSize(name);

	if(fileSize != sizeof(OpticalFiberEndSurResult))
	{
        return -1;
	}
	
	fp = fopen(name, "r");
	
	if(fp == NULL)
	{
		LOG(LOG_ERROR, "fopen %s failed !!!\n", name);
		return -1;
	}
	
	readSize = fread(&buff, 1, sizeof(OpticalFiberEndSurResult), fp);

	if(readSize != sizeof(OpticalFiberEndSurResult))
	{
		LOG(LOG_ERROR, "fread data from gdm file failed!!!\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);

	memcpy(CheckResult, &buff, sizeof(OpticalFiberEndSurResult));

	printGdfFile(CheckResult);
	return 0;
}


/***
  * 功能：
		释放光纤端面检查仪设备
  * 参数：
  		空
  * 返回：
		0: 成功
		-1: 失败
  * 备注：	
***/
int ReleaseFiberDevice(void)
{
	int iRet = 0;
	iRet = uvc_close(detector.iDevDetector);
	detector.iDevDetector = -1;
	if(iRet)
	{
		iRet = -1;
	}
	return iRet;
}


/***
  * 功能：
		获得文件名字和文件ID
  * 参数：
		char *file_path: 传入的文件路径
  * 返回：
		成功: 可以使用的文件num
		失败: -1 超出最大限制 -2 SD卡不存在 -3 路径创建失败
  * 备注：	
***/
int GetSaveFileName(char *file_path)
{
	int iRet = 0;
	int i;
	char FileGdmName[128] = {0};
	char FileHtmlName[128] = {0};
	if( access(MntDataDirectory, F_OK) == 0)
	{
        isExist(file_path);
        
		for (i = 1; i < MAX_FILE_NO; i++)
		{
			sprintf(FileGdmName, "%s%s%d%s", file_path, FilePrefix, i, fileSufix);
			sprintf(FileHtmlName, "%sreport/%s%d%s", file_path, FilePrefix, i, filefiberpdf);
			if(access(FileGdmName, F_OK) != 0 && access(FileHtmlName, F_OK) != 0) //find a valid file name
			{
				iRet = i;
				break;
			}
		}
		if(i == MAX_FILE_NO)
			iRet = 999;
	}
	else
	{
		iRet = -2;
	}
	return iRet;
}

/***
  * 功能：
		保存gdm文件
  * 参数：
  		
  * 返回：
		0: 成功
		非0: 失败
  * 备注：	
***/
int SaveGdmFile(int arg, void *pArg)
{
	int iRet = 0;
	char FileName[128] = {0};
	if( access(MntDataDirectory, F_OK) == 0)
	{
		if(strlen(OptFiberEndSurCheck.cName))
		{
	        isExist(filefibergdmpath);
			sprintf(FileName, filefibergdmpath"%s", OptFiberEndSurCheck.cName);
		}
		else
		{
			int iRet = 0;
			iRet = GetSaveFileName(filefibergdmpath);
			if(iRet >= 0)
			{
				sprintf(FileName, "%s%d%s", FilePrefix, iRet, fileSufix);
			}
			else
			{
				return iRet;
			}
			memset(OptFiberEndSurCheck.cName,0,sizeof(OptFiberEndSurCheck.cName));
			strcpy(OptFiberEndSurCheck.cName,FileName);
		}

        if(CanWriteFile(FileName))
		{
    		if(0 != (iRet = CreatFile(FileName, OptFiberEndSurCheck.ResultInfo)))
    		{
    			iRet = -4;
    		}
		}
		else
		{
            iRet = -10;
		}
	}
	else
	{
		iRet = -2;//sd卡不存在
	}
	
	return iRet;
}

#ifdef SAVE_IMAGE_DATA
//保存图像原始数据
void SaveImageData(void)
{
	FILE *fp = NULL;

	static int tmpStaticCount = 0;

	char name[512];
	sprintf(name, MntUsbDirectory"/fip_data_%d.txt", tmpStaticCount);
	tmpStaticCount++;
	fp = fopen(name, "w");
	//fp = fopen(IMAGE_DATA_FILE_PATH, "w");
	//检查参数
	if (!fp)
		return;
	int i = 0;
	unsigned char data[IMAGE_LEN] = {0};
	memcpy(data, OptFiberEndSurCheck.ResultInfo.OriginalImage, detector.ulImgSize);

	for ( i = 0; i < IMAGE_LEN; i++)
	{
		fprintf(fp, "%d\n", data[i]);
	}

	fclose(fp);
}
#endif

/***
  * 功能：
		保存报告
  * 参数：
  		
  * 返回：
		0: 成功
		非0: 失败
  * 备注：	
***/
int SaveReport(int arg, void *pArg)
{
	int iRet = 0;
	char FileName[128] = {0};
	char path[1024] = {0};
	if(access(MntDataDirectory, F_OK) == 0)
	{
		if(strlen(OptFiberEndSurCheck.cName))
		{
			strcpy(FileName, OptFiberEndSurCheck.cName);
			isExist(filefiberpdfpath);
		}
		else
		{
			int iRet = 0;
			iRet = GetSaveFileName(filefibergdmpath);
			if(iRet >= 0)
			{
				sprintf(FileName, "%s%d%s", FilePrefix, iRet, fileSufix);
			}
			else
			{
				return iRet;
			}
			memset(OptFiberEndSurCheck.cName,0,sizeof(OptFiberEndSurCheck.cName));
			strcpy(OptFiberEndSurCheck.cName,FileName);
		}

		sprintf(path, filefiberpdfpath"%s", FileName);
		char *suffix = strstr(path, fileSufix);
		if(suffix)
		{
            strcpy(suffix, filefiberpdf);
		}

		if(CanWriteFile(path))
		{
            iRet =CreateFipReport(path);
            if(1 != iRet)		
            {
				LOG(LOG_INFO, "creatFipReport path = %s   iRet = %d\n", path, iRet);
				iRet = -5;
		    }
		}
		else
		{
            iRet = -10;
		}
	}
	else
	{
		iRet = -2;//sd卡不存在
	}
	
	return iRet;
}

/******************************************************************************
*                               生成报告									  *
******************************************************************************/

static unsigned char* rgb8_to_rgb24(unsigned char* data_8, int width, int height)
{
    unsigned char *rgb24 = NULL;
	unsigned char *tmp = NULL;
	unsigned char *d = data_8;
    int imageWidth = width * 3;//round_up(width * 24, 32) >> 3;
	int BmpSize = imageWidth*height;
	int w = 0;
	int h = 0;
	int i = 0;

    if(!data_8)
    {
        return NULL;
    }

    rgb24 = (unsigned char *)GuiMemAlloc(sizeof(unsigned char) * BmpSize);
    
	tmp = rgb24;
	d += LINE_LEN * (LINE_LEN - height) / 2;
	
	for (h = height; h != 0; --h)
	{
		i = (LINE_LEN - width) / 2;
		for (w = width; w != 0; --w)
		{
			*tmp++ = d[i];
			*tmp++ = d[i];
			*tmp++ = d[i];
			i++;
		}
		d += LINE_LEN;
	}

	return rgb24;
}

//画圆
static int DrawImg24Circle(int Cir_X, int Cir_Y, unsigned char *pImag, int width, int height)
{
	unsigned int CirColor[5] = {0X0000FF,0XB55A00,0X8EF702,0X66FFFF,0XFFFF33};
	int CirR[5] = {0};
	CirR[0] = 5 * OptFiberEndSurCheck.ResultInfo.magnification;
	CirR[1] = 18 * OptFiberEndSurCheck.ResultInfo.magnification;
	CirR[2] = 78 * OptFiberEndSurCheck.ResultInfo.magnification;
	CirR[3] = 93 * OptFiberEndSurCheck.ResultInfo.magnification;
	CirR[4] = 171 * OptFiberEndSurCheck.ResultInfo.magnification;
	
	int CirX = Cir_X - (IMAGE_WIDTH - width) / 2;
	int CirY = Cir_Y - (IMAGE_HEIGHT - height) / 2;
	int imageWidth = width * 3;
	int i = 0;
	for(;i<5;i++)
	{
		int r = CirR[i];
		unsigned char colorR = CirColor[i] & 0x0000ff;
		unsigned char colorG = (CirColor[i] & 0x00ff00)>>8;
		unsigned char colorB = (CirColor[i] & 0xff0000)>>16;
		int x = 0,y = 0,d = 0;
		y = r;
		d = 1 - r;

		while(y>=x)
		{
			int x1,y1;
			x1=x;
			y1=y;
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;
			}				
			x1=-x;
			y1=y;		
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;

			}				
			x1=x;
			y1=-y;
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;
			}				
			x1=-x;
			y1=-y;
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;

			}				
			x1=y;
			y1=x;
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;

			}				
			x1=-y;
			y1=x;
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;

			}				
			x1=y;
			y1=-x;
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;

			}				
			x1=-y;
			y1=-x;
			if((x1+CirX >= 0) && (x1+CirX < width)
				&&(y1+CirY >= 0) && (y1+CirY < height))
			{
				int offset = (x1+CirX)*3 + (y1+CirY)*imageWidth;
				*(pImag+offset) = colorR;
				*(pImag+offset+1) = colorG;
				*(pImag+offset+2) = colorB;

			}

			if(d < 0)
			{
				d = d + 2*x +3;
			}
			else
			{
				d = d + 2*(x-y) + 5;
				y--;
			}
			x++;
		}
	}
	
	return 0;
}

//生成原始图片
int creatFipOriginalPicture(char *absolutePath, int width, int height)
{	
	if(absolutePath == NULL)
	{
		return -1;
	}
	
	unsigned char *data_24 = rgb8_to_rgb24(OptFiberEndSurCheck.ResultInfo.OriginalImage, width, height);
	Jpeg_compress(data_24, width, height, absolutePath);
	free(data_24);
	
	return 0;
}

//生成处理后的图片
int creatFipProcessedPicture(char *absolutePath, int width, int height)
{	
	if(absolutePath == NULL)
	{
		return -1;
	}
	
	unsigned char *data_24 = rgb8_to_rgb24(OptFiberEndSurCheck.ResultInfo.ProcessedImage, width, height);
	if(OptFiberEndSurCheck.ResultInfo.CirX >= 0 && OptFiberEndSurCheck.ResultInfo.CirY >= 0)
	{
	    DrawImg24Circle(OptFiberEndSurCheck.ResultInfo.CirX, OptFiberEndSurCheck.ResultInfo.CirY, 
	        data_24, width, height);
	}
	Jpeg_compress(data_24, width, height, absolutePath);
	free(data_24);
	
	return 0;
}


//创建报告 成功返回1
int creatFipReport(char *fipReportName)
{
    #if 0
	FILE *pFipReportFile = NULL;
	time_t currentTime;
	struct tm *pTime = NULL;
	char *fipReportFilePath = "/mnt/sdcard/Fiber_Microscope/report";
	char strPath[PATH_MAX] = {0};
	char imgPath[PATH_MAX] = {0};
	char cTime[128] = {0};
	if (!fipReportName)
	{
		return -1;
	}
	
	//检查存储路径
	if (access(fipReportFilePath, F_OK)) 
	{
		sprintf(strPath, "mkdir -p %s", fipReportFilePath);
		mysystem(strPath);
	}
	//创建html文件	
	char *pTmp = strrchr(fipReportName, '.');
	if(pTmp)
	{
		*pTmp = '\0';
	}
	strcat(fipReportName,".html");
	sprintf(strPath, "%s/%s", fipReportFilePath, fipReportName);
    printf("strPath = %s\n",strPath);
	pFipReportFile = fopen(strPath, "w+");
	if (!pFipReportFile) 
	{
		return -2;
	}

	InitFipReportText();
	
	//html头
	GENERATE_HTML_HEADER_FIBER_TO_HTML(pFipReportFile, "Fiber Microscope");

    //html标题
	GENERATE_TITLE_TO_HTML(pFipReportFile, 1, pFipRptTitle);
	
	/********************************是否通过**********************************/
	GenerateFiberImg(fipReportName,imgPath,0);
	GENERATE_IMG_ALIGN_TO_HTML(pFipReportFile, "right",imgPath);

	/********************************一般信息**********************************/
	//表格标题
	GENERATE_TITLE_TO_HTML(pFipReportFile, 2, pFipRptInfo);

    //表格开始
	GENRATE_NOBORDER_TAB_TO_HTML(pFipReportFile);

	time(&currentTime);
	pTime = gmtime(&currentTime);
    if(pCurSystemSet->uiTimeShowFlag == 0)
    {
	    strftime(cTime, 128, "%d/%m/%Y %X", pTime);
    }
    else if(pCurSystemSet->uiTimeShowFlag == 1)
    {
        strftime(cTime, 128, "%m/%d/%Y %X", pTime);
    }
    else
    {
        strftime(cTime, 128, "%Y/%m/%d %X", pTime);
    }

	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptFilename);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, fipReportName);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptCheckDate);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, cTime);
	GENERATE_TR_END_TO_HTML(pFipReportFile);

	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptVersion);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, "1.0");
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptAnalyzeDate);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, cTime);
	GENERATE_TR_END_TO_HTML(pFipReportFile);

	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptTaskIdentify);
	if(fipIdentify.taskIdentifyIsSelect)
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, fipIdentify.taskIdentify);
	}
	else
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, " ");	
	}
	
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptClient);
	if(fipIdentify.clientIsSelect)
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, fipIdentify.client);
	}
	else
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, " ");
	}
	GENERATE_TR_END_TO_HTML(pFipReportFile);

	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptCompany);	
	if(fipIdentify.companyIsSelect)
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, fipIdentify.company);
	}
	else
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, " ");
	}
	GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRptNote);
	if(fipIdentify.noteIsSelect)
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, fipIdentify.note);
	}
	else
	{
		GENERATE_NOBORDER_TD_TO_HTML(pFipReportFile, "%s", 32, " ");
	}
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	
	//表格结束
	GENRATE_TAB_END_TO_HTML(pFipReportFile);
	/********************************位置**********************************/
	//表格标题
	GENERATE_TITLE_TO_HTML(pFipReportFile, 4, pFipRptPosition);
	
	//表格开始
	GENRATE_BORDER_TAB_TO_HTML(pFipReportFile);
	//第一行
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 33, " ");		
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 33, pFipRptPositionA);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 33, pFipRptPositionB);
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	//第二行
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 33, pFipRptOperator);
	//GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 33, " ");
	//GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 33, " ");
    
    if(fipIdentify.positionAIsSelect)
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 33, fipIdentify.positionA);
	}
	else
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 33, " ");
	}
	if(fipIdentify.positionBIsSelect)
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 33, fipIdentify.positionB);
	}
	else
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 33, " ");
	}
    
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	
	//表格结束
	GENRATE_TAB_END_TO_HTML(pFipReportFile);
	
	/********************************标识**********************************/
	//表格标题
	GENERATE_TITLE_TO_HTML(pFipReportFile, 4, pFipRptIdentify);
	
	//表格开始
	GENRATE_BORDER_TAB_TO_HTML(pFipReportFile);
	//第一行
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 20, "Cable ID");		
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 20, "Fiber ID");
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 20, "Location A");
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 20, "Location B");
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 20, "Connector ID");
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	//第二行
	GENERATE_TR_TO_HTML(pFipReportFile);

	if(fipIdentify.cableIDIsSelect)
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, fipIdentify.cableID);
	}
	else
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, " ");
	}

	if(fipIdentify.fiberIDIsSelect)
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, fipIdentify.fiberID);
	}
	else
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, " ");
	}
	if(fipIdentify.positionAIsSelect)
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, fipIdentify.positionA);
	}
	else
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, " ");
	}
	if(fipIdentify.positionBIsSelect)
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, fipIdentify.positionB);
	}
	else
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, " ");
	}
	if(fipIdentify.connectorIDIsSelect)
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, fipIdentify.connectorID);
	}
	else
	{
		GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 20, " ");
	}
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	
	//表格结束
	GENRATE_TAB_END_TO_HTML(pFipReportFile);
	/********************************图像**********************************/
	//标题
	GENERATE_TITLE_TO_HTML(pFipReportFile, 2, pFipRptImage);
	//表格开始
	GENRATE_NOBORDER_TAB_TO_HTML(pFipReportFile);
	
	GENERATE_TR_TO_HTML(pFipReportFile);
	GenerateFiberImg(fipReportName,imgPath,1);
	GENERATE_NOBORDER_IMAGE_TD_TO_HTML(pFipReportFile, 50, imgPath);
	GenerateFiberImg(fipReportName,imgPath,2);
	GENERATE_NOBORDER_IMAGE_TD_TO_HTML(pFipReportFile, 50, imgPath);
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	//表格结束
	GENRATE_TAB_END_TO_HTML(pFipReportFile);

	/********************************结果**********************************/
	//表格标题
	GENERATE_TITLE_TO_HTML(pFipReportFile, 4, pFipRptResult);
	
	//表格开始
	GENRATE_BORDER_TAB_TO_HTML(pFipReportFile);
	//表头 第一行
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_ROWS_TH_TO_HTML(pFipReportFile, "%s", 2, 16, pFipRptLayering);		
	GENERATE_BORDER_ROWS_TH_TO_HTML(pFipReportFile, "%s", 2, 16, pFipRptLayeringDiameter);
	GENERATE_BORDER_COLS_TH_TO_HTML(pFipReportFile, "%s", 3, 34, pFipRptScracth);
	GENERATE_BORDER_COLS_TH_TO_HTML(pFipReportFile, "%s", 3, 34, pFipRptDefect);
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	//表头 第二行
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 18, pFipRptCriterion);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 8, pFipRptThreshold);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 8, pFipRptAmount);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 18, pFipRptCriterion);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 8, pFipRptThreshold);
	GENERATE_BORDER_TH_TO_HTML(pFipReportFile, "%s", 8, pFipRptAmount);
	GENERATE_TR_END_TO_HTML(pFipReportFile);

	//单元格 A区
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 16, pFipRptCore);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 16, "0 - 25");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt0_INF);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, 0);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberCoreScracth);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt0_INF);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, 0);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberCoreDefect);
	GENERATE_TR_END_TO_HTML(pFipReportFile);

	//单元格 B区
	//小污点
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_ROWS_TD_TO_HTML(pFipReportFile, "%s", 3, 16, pFipRptCladding);
	GENERATE_BORDER_ROWS_TD_TO_HTML(pFipReportFile, "%s", 3, 16, "25 - 120");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt0_3);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, pFipRptAny);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthSmall);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt0_2);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, pFipRptAny);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectSmall);
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	//中污点
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt3_INF);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, 0);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt2_5);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, 5);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium);
	GENERATE_TR_END_TO_HTML(pFipReportFile);	
	//大污点
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, "");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt5_INF);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, 0);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge);
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	
	//单元格 C区
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 16, pFipRptBuffer);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 16, "120 - 130");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, "---");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "---");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "---");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, "---");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "---");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "---");
	GENERATE_TR_END_TO_HTML(pFipReportFile);

	//单元格 D区
	//小污点
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_ROWS_TD_TO_HTML(pFipReportFile, "%s", 2, 16, pFipRptJacket);
	GENERATE_BORDER_ROWS_TD_TO_HTML(pFipReportFile, "%s", 2, 16, "130 - 250");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt0_INF);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, pFipRptAny);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberJacketScracth);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt0_10);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, pFipRptAny);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberJacketDefectSmall);
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	//中污点
	GENERATE_TR_TO_HTML(pFipReportFile);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, "");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 8, "");
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%s", 18, pFipRpt10_INF);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, 0);
	GENERATE_BORDER_TD_TO_HTML(pFipReportFile, "%d", 8, OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge);
	GENERATE_TR_END_TO_HTML(pFipReportFile);
	
	//表格结束
	GENRATE_TAB_END_TO_HTML(pFipReportFile);
	
    //注释
    memset(cTime,0,sizeof(cTime));
    if(pCurSystemSet->uiTimeShowFlag == 0)
    {
	    strftime(cTime, 128, "%d/%m/%Y", pTime);
    }
    else if(pCurSystemSet->uiTimeShowFlag == 1)
    {
        strftime(cTime, 128, "%m/%d/%Y", pTime);
    }
    else
    {
        strftime(cTime, 128, "%Y/%m/%d", pTime);
    }	
    GENERATE_HTML_FOOTER_FIBER_TO_HTML(pFipReportFile, pFipRptSignature, pFipRptDate, cTime);
	fclose(pFipReportFile);
	sync();

	ExitFipReportText();
	
	printf("creat report success\n");
	#endif
	return 1;
}
