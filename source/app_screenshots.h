/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_screenshots.h  
* 摘    要：  实现截图功能函数声明以及相关数据结构
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


#ifndef _APP_SCREENSHOTS_H
#define	_APP_SCREENSHOTS_H
#include "guiglobal.h"

//截图函数
int ScreenShots(UINT16 StartX, UINT16 EndX, UINT16 StartY, UINT16 EndY, 
				char *strBmpFile);
int OtdrScreenShot(GUIWINDOW *pWnd);

int ScreenShotsJpg(UINT16 StartX, UINT16 EndX, UINT16 StartY, UINT16 EndY, 
				char *strBmpFile);
//jpeg图片压缩生成
int Jpeg_compress(unsigned char *rgb, int width, int height, const char *fileName);
int DelPdfJpg();

#endif
