/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_screenshots.c  
* 摘    要：  实现截图功能 目前只支持16位帧缓冲的数据保存
*
* 当前版本：  v1.0.1  对帧缓冲区的读取加锁保护 去除不必要的宏定义
* 作    者：
* 完成日期：  
*******************************************************************************/


#include "app_screenshots.h"
#include "app_frmfilebrowse.h"
#include "app_sola.h"

/******************************************
* 应用app_screenshots.c中所需要的其他头文件
******************************************/
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include "global_index.h"
#include "wnd_global.h"
#include "jpeglib.h"

/************************************
* 定义app_screenshots.c中内部使用的宏
************************************/
#define SCREENSHOTDIR 	MntDataDirectory"/screenshots/"
extern SOLA_FILE_NAME_SETTING *pBmpFileNamedSetting;

//交换两个数,此函数必须在两个数不相等的情况下使用
#define ScreenShotSwap(x, y) (((x)=(x)^(y)),((y)=(x)^(y)),((x)=(x)^(y)))
static char strCurrScreenShotFileName[PATH_MAX];

/*****************************************
*定义app_screenshots.c中内部使用的数据结构
*****************************************/
typedef struct _screenshot
{
	UINT16 uiSX;			//截图起始x坐标
	UINT16 uiSY;			//截图结束x坐标
	
	UINT16 uiImgH;			//截图的高度(单位:像素)
	UINT16 uiImgW;			//截图的宽度(单位:像素)
	
	UINT32 uiBmpW;		    //图片保存时真实的行数据宽度(单位:字节)
	
	char BmpFile[PATH_MAX];		//文件名
}SCREENSHOT;


/*************************************
*声明app_screenshots.c中内部使用的函数
*************************************/
//保存24位截图
static int SaveScrn24Bmp(SCREENSHOT *pScreenShots, GUIFBMAP *pfb);
//检查保存路径
static int CheckScreenShotsDir(void);
//计算默认截图文件名
//static int GetDefaultFileName(char *FileName);
//设置24位bmp文件头信息
static int Set24BmpFileHeader(GUIBITMAP_FILEHEADER *pFH, SCREENSHOT *pScreenShots);
//设置24位bmp图片头信息
static int  Set24BmpInfoHeader(GUIBITMAP_INFOHEADER *pIH, SCREENSHOT *pScreenShots);
//保存24的jpg图片
static int SaveScrn24Jpg(SCREENSHOT *pScreenShots, GUIFBMAP *pfb);
//rgb565 to rgb888
static int RGB565_to_RGB24(unsigned char *rgb565, unsigned char *rgb24, 
					int org_width, int org_height, int x, int y,
					int width, int height);
int GetBmpFileName(char *filename);

/*
说明:
	截图函数，提供给外部调用截图功能的函数
参数:
	StartX		:要截取图片的横坐标起始点
	EndX		:要截取图片的横坐标结束点
	StartY		:要截取图片的纵坐标起始点
	EndY		:要截取图片的纵坐标结束点
	strBmpFile	:要保存图片的文件名，如果为空则以时间计算
				 并且存储在SD卡的screenshots/下
返回:
	成功返回 0，失败返回非 0 值
*/
int ScreenShots(UINT16 StartX, UINT16 EndX, UINT16 StartY, UINT16 EndY, 
				char *strBmpFile)
{
	//临时变量定义
	SCREENSHOT ScreenShot;
	GUIFBMAP *pFb = GetCurrFbmap();

	//参数检查
	if ((StartX >= (pFb->uiHorDisp + pFb->uiHorOff)) || (StartX < pFb->uiHorOff) ||
		(EndX >= (pFb->uiHorDisp + pFb->uiHorOff)) ||(EndX < pFb->uiHorOff) ||
		(StartY >= (pFb->uiVerDisp + pFb->uiVerOff)) || (StartY < pFb->uiVerOff) || 
		(EndY >= (pFb->uiVerDisp + pFb->uiVerOff)) || (EndY < pFb->uiVerOff))
	{
		return -1;
	}
	
	if (StartX > EndX)
	{
		ScreenShotSwap(StartX ,EndX);
	}
	if (StartY > EndY)
	{
		ScreenShotSwap(StartY ,EndY);
	}
	
	//获取截图参数
	memset(&ScreenShot, 0, sizeof(SCREENSHOT));
	ScreenShot.uiSX = StartX;
	ScreenShot.uiSY = StartY;
	ScreenShot.uiImgH = EndY - StartY + 1;		//height
	ScreenShot.uiImgW = EndX - StartX + 1;		//width
	ScreenShot.uiBmpW = round_up((ScreenShot.uiImgW) * 24, 32) >> 3;

	//判断文件名是否为空
	if (NULL == strBmpFile)
	{
		//检查SD卡是否挂载
		if (CheckScreenShotsDir())
		{
			return -2;				//No Sdcard or mkdir failed
		}
		
		if (GetBmpFileName(ScreenShot.BmpFile))
		{
			return -3;
		}
	}
	//判断文件名中是否带有存储路径，
	//如果带有路径会保存在该路径下(不会检查路径是否有效)
	else if (NULL == strchr(strBmpFile, '/'))
	{
		//检查SD卡是否挂载
		if (CheckScreenShotsDir())
		{
			return -2;				//No Sdcard or mkdir failed
		}
		
		sprintf(ScreenShot.BmpFile, SCREENSHOTDIR"%s", strBmpFile);
	}
	else
	{
		strcpy(ScreenShot.BmpFile, strBmpFile);
	}

	//保存截图
	if (SaveScrn24Bmp(&ScreenShot, pFb))
	{
		return -4;
	}

	return 0;
}

/*
说明:
	检查截图存放目录以及Sdcard是否挂载
参数:
	无
返回:
	成功返回 0，失败返回非 0 值
*/
static int CheckScreenShotsDir(void)
{
	int iRet = 0;

	if ( access(MntDataDirectory, F_OK) )
	{
		iRet = -1;			//No sdcard
	}
	
	if (!iRet)
	{	
		if (access(SCREENSHOTDIR, F_OK | W_OK))
		{
			if (mkdir(SCREENSHOTDIR, 0666))
				iRet = -2;	//Make directory failed
		}
	}
	
	return iRet;
}

/*
说明:
	获取保存屏幕截图bmp文件名(以时间来计算)
参数:
	FileName	:bmp文件名指针
返回:
	成功返回 0，失败返回非 0 值

static int GetDefaultFileName(char *FileName)
{
	time_t t = 0;
	struct tm *p = NULL;

	if(NULL == FileName)
	{
		return -1;
	}
	
	if (0 > (t = time(NULL)))
	{
		return -2;
	}
	
	if (NULL == (p = gmtime(&t))) 
	{
		return -3;
	}

	sprintf(FileName, SCREENSHOTDIR"BMP%.4d-%.2d%.2d-%.2d%.2d%.2d.bmp", 
			p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, 
			p->tm_hour, p->tm_min, p->tm_sec);

	return 0;
}
*/

/*
说明:
	设置bmp文件头信息
参数:
	pFH	:bmp文件头信息结构指针
返回:
	成功返回 0，失败返回非 0 值
*/
static int Set24BmpFileHeader(GUIBITMAP_FILEHEADER *pFH, SCREENSHOT *pScreenShots)
{
	if ((pFH == NULL) ||
		(pScreenShots == NULL))
	{
		return -1;
	}
	
	pFH->usFileType = 0x4d42;
	pFH->uiFileSize = (pScreenShots->uiBmpW) * (pScreenShots->uiImgH) + 54;
	pFH->usReserved1 = 0;
	pFH->usReserved2 = 0;
	pFH->uiDataOffset= 54;

	return 0;
}

/*
说明:
	设置bmp图片头信息
参数:
	pIH	:bmp图片信息结构指针
返回:
	成功返回 0，失败返回非 0 值
*/
static int Set24BmpInfoHeader(GUIBITMAP_INFOHEADER *pIH, SCREENSHOT *pScreenShots)
{
	if ((pIH == NULL) ||
		(pScreenShots == NULL))
	{
		return -1;
	}
	
	pIH->uiInfoSize = 40;
	pIH->uiImgWidth = pScreenShots->uiImgW;
	pIH->uiImgHeight = pScreenShots->uiImgH;
	pIH->usDevPlanes = 1;
  	pIH->usBitCount = 24;
  	pIH->uiCompression = 0;
  	pIH->uiImgSize = 0;
	pIH->uiXRes = 0;
    pIH->uiYRes = 0;
    pIH->uiClrUsed = 0;
    pIH->uiClrImportant = 0;

	return 0;
}

/*
说明:
	24位图片截图功能具体实现
参数:
	无
返回:
	成功返回 0，失败返回非 0 值
*/
static int SaveScrn24Bmp(SCREENSHOT *pScreenShots, GUIFBMAP *pfb)
{
	//返回值
	int iRet = 0;
	//临时变量
	int offset1, offset2, w, h, t, PixelBytes, iImgSize;
	
	FILE* pBmpFile = NULL;
	
	char *pMapBuffer = NULL;
	char *pBmp24buf = NULL;	
	char *pTmp = NULL;
	
	GUIBITMAP_FILEHEADER fh;		//Bmp FileHeader
	GUIBITMAP_INFOHEADER ih;		//Bmp InfoHeader
		
	//创建BMP图片文件
	if (NULL == (pBmpFile = fopen(pScreenShots->BmpFile, "w+")))
	{
		iRet = -1;
		goto Err;
	}
	//写入BMP文件头
	Set24BmpFileHeader(&fh, pScreenShots);
	if (14 != fwrite(&(fh.usFileType), 1, 14, pBmpFile))
	{
		iRet = -2;
		goto Err;
	}
	//写入BMP信息头
	Set24BmpInfoHeader(&ih, pScreenShots);
	if (40 != fwrite(&ih, 1, 40, pBmpFile))
	{
		iRet = -2;
		goto Err;
	}
	//计算文件中图像有效数据的大小并分配缓冲空间
	iImgSize = pScreenShots->uiBmpW * pScreenShots->uiImgH;
	if (NULL == (pBmp24buf = (char *)malloc(iImgSize)))
	{
		iRet = -3;
		goto Err;
	}

	//计算相关其他参数
	pMapBuffer = (char *)pfb->pMapBuff;
	//计算帧缓冲数据的偏移量
	PixelBytes = pfb->uiPixelBits >> 3;
	pMapBuffer += (pScreenShots->uiSX + (pfb->uiHorRes + pfb->uiHorOff * 2) * \
			(pScreenShots->uiSY + pScreenShots->uiImgH - 1)) * PixelBytes;
	//获取暂存空间的指针
	pTmp = pBmp24buf;
	
	h = pScreenShots->uiImgH;
	w = pScreenShots->uiImgW;
	//保存图像
	switch (pfb->uiPixelBits)
	{
	case 16://RGB565 帧缓冲为16位格式
		offset1 = (w + pfb->uiHorRes + pfb->uiHorOff * 2) * PixelBytes;
		offset2 = pScreenShots->uiBmpW - w * 3;
		MutexLock(&(pfb->Mutex)); //加锁
		while (h--)
		{	
			t = w;
			while (t--)
			{	
				*pTmp++ = ((*((UINT16*)pMapBuffer)) & 0x001F) << 3;
				*pTmp++ = ((*((UINT16*)pMapBuffer)) & 0x07E0) >> 3;
				*pTmp++ = ((*((UINT16*)pMapBuffer)) & 0xF800) >> 8;
				pMapBuffer += PixelBytes;
			}
			pMapBuffer -= offset1;
			pTmp += offset2;
		}
		MutexUnlock(&(pfb->Mutex));//解锁
		break;
	case 24://RGB888 帧缓冲为24位格式
		offset1 = (pfb->uiHorRes + pfb->uiHorOff * 2) * PixelBytes;
		offset2 = pScreenShots->uiBmpW;
		MutexLock(&(pfb->Mutex)); //加锁
		while (h--)
		{	
			memcpy(pTmp, pMapBuffer, w * PixelBytes);
			pMapBuffer -= offset1;
			pTmp += offset2;
		}
		MutexUnlock(&(pfb->Mutex));//解锁
		break;
	default:
		iRet = -4;
		goto Err;
		break;
	}
	//将图像数据写入BMP文件
	if (iImgSize != fwrite(pBmp24buf, 1, iImgSize, pBmpFile))
	{
		iRet = -5;
		goto Err;
	}

Err:
	//错误处理
	switch (iRet) 
	{
	case 0:
        strcpy(strCurrScreenShotFileName, pScreenShots->BmpFile);
		LOG(LOG_INFO, "\nScreenShots Success:%s\n", strCurrScreenShotFileName);
	case -5:
	case -4:
		free(pBmp24buf);
	case -3:
	case -2:
		fclose(pBmpFile);
	case -1:
		break;
	}

	return iRet;
}


int SaveScreen(int arg, void *pArg)
{
	return ScreenShots(0, WINDOW_WIDTH - 1, 0, WINDOW_HEIGHT-1, NULL);
}

GUICHAR* SetScreenStr(int arg)
{
	GUICHAR* Text = NULL;
    GUICHAR* FileName;
	
	if (0 == arg)
	{
		Text = TransString("The screenshot has been saved under /sdcard/screenshots");
        char *p = strrchr(strCurrScreenShotFileName, '/');
        FileName = TransString(p);
        StringCat(&Text, FileName);
	}
	else
	{
		Text = TransString("Fail to save screenshot. Please check your SD card!");
	}

	return Text;
}


int OtdrScreenShot(GUIWINDOW *pWnd)
{
	return DisplayCue(pWnd, SaveScreen, SetScreenStr);
}

int Jpeg_compress(unsigned char *rgb, int width, int height,
				  const char *fileName)
{
	int iRet = 0;
	FILE * outfile;
	int row_stride;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(fileName, "wb")) == NULL)
	{
		LOG(LOG_ERROR, "can not open %s\n", fileName);
		iRet = -1;
	}
	if (!iRet)
	{
		jpeg_stdio_dest(&cinfo, outfile);

		cinfo.image_width = width;
		cinfo.image_height = height;
		cinfo.input_components = 3;

		cinfo.in_color_space = JCS_RGB;

		jpeg_set_defaults(&cinfo);

		jpeg_set_quality(&cinfo, 90, TRUE );
		jpeg_start_compress(&cinfo, TRUE);
		row_stride = width * 3;

		while (cinfo.next_scanline < cinfo.image_height)
		{
			row_pointer[0] = (JSAMPROW)&rgb[cinfo.next_scanline * row_stride];
			jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}

		jpeg_finish_compress(&cinfo);
		fclose(outfile);

		jpeg_destroy_compress(&cinfo);
	}
	
	return iRet;
}

/**
 * \brief: bad argumets arrangement @_@!!
 */
static int RGB565_to_RGB24(unsigned char *rgb565, unsigned char *rgb24,
                    int org_width, int org_height,
                    int x, int y,
					int width, int height)
{
	int iRet = 0;
	int i = 0, j = 0;
	unsigned char r,g,b;
	unsigned short *pix565 = NULL;
    
	pix565 = (unsigned short*)rgb565;
    pix565 += (org_width*y);

    if ((width + x) > org_width)
    {
        width = org_width - x;
    }
    if ((height + y) > org_height)
    {
        height = org_height - y;
    }

	for(i = 0;i < height; i++)
	{   
	    pix565 += x;
	    for(j = 0; j < width; j++)
        {   
    		r = ((*pix565)>>11)&0x1f;
    		*rgb24 = (r<<3) | (r>>2);
    		rgb24++;
    		g = ((*pix565)>>5)&0x3f;
    		*rgb24 = (g<<2) | (g>>4);
    		rgb24++;
    		b = (*pix565)&0x1f;
    		*rgb24 = (b<<3) | (b>>2);
    		rgb24++;
            pix565++;   
        }
		pix565 += (org_width-width -x);
	}
	
	return iRet;
}

/*
说明:
	24位图片截图功能具体实现
参数:
	无
返回:
	成功返回 0，失败返回非 0 值
*/
static int SaveScrn24Jpg(SCREENSHOT *pScreenShots, GUIFBMAP *pfb)
{
	int iRet = 0;	
	
	UINT16 *pMapBuffer = NULL;	
	unsigned char *pRGB24 = NULL;
	
	do {
		//获取帧缓冲并计算相关其他参数
		pRGB24 = (unsigned char *)malloc(pScreenShots->uiImgW * pScreenShots->uiImgH *3);
		
		MutexLock(&(pfb->Mutex)); //加锁
		pMapBuffer = (UINT16 *)pfb->pMapBuff;
		RGB565_to_RGB24((unsigned char *)pMapBuffer, pRGB24,
						pfb->uiHorRes, pfb->uiVerRes,
						pScreenShots->uiSX, pScreenShots->uiSY,
						pScreenShots->uiImgW, pScreenShots->uiImgH);
		MutexUnlock(&(pfb->Mutex)); //解锁
		
		iRet = Jpeg_compress(pRGB24, pScreenShots->uiImgW, pScreenShots->uiImgH,
							pScreenShots->BmpFile);
		GuiMemFree(pRGB24);
	} while (0);
	
	return iRet;
}

//截图生成pdf.jpg
int ScreenShotsJpg(UINT16 StartX, UINT16 EndX, UINT16 StartY, UINT16 EndY, 
				char *strBmpFile)
{
	//临时变量定义
	SCREENSHOT ScreenShot;
	GUIFBMAP *pFb = GetCurrFbmap();

	//参数检查
	if ((StartX >= (pFb->uiHorDisp + pFb->uiHorOff)) || (StartX < pFb->uiHorOff) ||
		(EndX >= (pFb->uiHorDisp + pFb->uiHorOff)) ||(EndX < pFb->uiHorOff) ||
		(StartY >= (pFb->uiVerDisp + pFb->uiVerOff)) || (StartY < pFb->uiVerOff) || 
		(EndY >= (pFb->uiVerDisp + pFb->uiVerOff)) || (EndY < pFb->uiVerOff))
	{
		return -1;
	}
	
	if (StartX > EndX)
	{
		ScreenShotSwap(StartX ,EndX);
	}
	if (StartY > EndY)
	{
		ScreenShotSwap(StartY ,EndY);
	}
	
	//获取截图参数
	memset(&ScreenShot, 0, sizeof(SCREENSHOT));
	ScreenShot.uiSX = StartX;
	ScreenShot.uiSY = StartY;
	ScreenShot.uiImgH = EndY - StartY + 1;		//height
	ScreenShot.uiImgW = EndX - StartX + 1;		//width
	ScreenShot.uiBmpW = round_up((ScreenShot.uiImgW) * 24, 32) >> 3;


	//检查SD卡是否挂载
	if (CheckScreenShotsDir())
	{
		return -2;				//No Sdcard or mkdir failed
	}

	strcpy(ScreenShot.BmpFile, strBmpFile);

	//保存截图
	if (SaveScrn24Jpg(&ScreenShot, pFb))
	{
		return -3;
	}

	return 0;
}

int DelPdfJpg()
{
	return mysystem("rm /app/pdf*.jpg");
}

/*
**函数说明:
	获取截屏文件名
**参数说明:
	
**返回值:
	无
*/
int GetBmpFileName(char *filename)
{
    char name[128];
    unsigned int iSuffix;
    int i, max;
    if(!filename)
    {
        return -1;
    }
    max = pow(10, pBmpFileNamedSetting->suffixWidth);
    iSuffix = pBmpFileNamedSetting->suffix % max ? pBmpFileNamedSetting->suffix % max : 1;
    for(i=0; i<max; i++)
    {
        char temp[512];
        int isGetName = 1;

        if(isGetName)
        {
            sprintf(temp, "%s%%0%dd%s",pBmpFileNamedSetting->prefix, pBmpFileNamedSetting->suffixWidth, ".bmp");
            sprintf(name, temp, iSuffix);
            isGetName = !isFileExist(name, SCREENSHOTDIR);
        }
        
        if(!isGetName)
        {
            if(pBmpFileNamedSetting->suffixRule)
            {
                if(++iSuffix == max)
                {
                    iSuffix = 1;
                }
            }
            else
            {
                if(--iSuffix == 0)
                {
                    iSuffix = max - 1;
                }
            }

            if(iSuffix == pBmpFileNamedSetting->suffix)
            {
                return -1;
            }
        }
        else
        {
            pBmpFileNamedSetting->suffix = iSuffix;
            sprintf(filename, "%s%s", SCREENSHOTDIR, name);
            return 0;
        }
    }
    
    return -2;
}

