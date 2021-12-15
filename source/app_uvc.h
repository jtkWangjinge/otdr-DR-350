/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_uvc.h  
* 摘    要：  封装uvc驱动的V4L2接口，并提供API给应用
*
* 当前版本：  v1.1.0
* 作    者：  wujunguang
* 完成日期：  
*
*******************************************************************************/

#ifndef _APP_UVC_H_
#define _APP_UVC_H_

#include <linux/videodev2.h>

#define UVCFILE	        "/dev/video0"
#define UVCBUFNUM       4 

//视频原始数据格式
typedef enum pixel_Format
{
    FORMAT_MJPEG        = V4L2_PIX_FMT_MJPEG,		//MJPEG
    FORMAT_YUYV	        = V4L2_PIX_FMT_YUYV			//YUV 4:2:2 (YUYV)
} PIXELFORMAT;

//不同格式所支持的像素尺寸
typedef enum pixel_Size
{
	MJPEG_1280X960 	= 0,
	MJPEG_1280X720 	= 1,
	MJPEG_640X480	= 2,
	MJPEG_320X240	= 3,
	MJPEG_800X600	= 4,
	YUYV_1280X960 	= 5,
	YUYV_1280X720 	= 6,
	YUYV_640X480	= 7,
	YUYV_320X240	= 8,
	YUYV_800X600	= 9,
	PIXELSIZENUM
}PIXELSIZE;
/*********************************************************************************************************
* 函数声明
*********************************************************************************************************/

/*
**函数说明:
	打开uvc设备
**参数说明:
	const char *pathname 	：设备节点路径
	int flags				:读写权限
**返回值:
	成功--返回uvc设备的文件描述符
	失败--返回负数
*/
int uvc_open(const char *pathname, int flags);
/*
**函数说明:
	int fd	：需要关闭的文件描述符
**参数说明:
	无
**返回值:
	0	--成功
	-1	--失败
*/
int uvc_close(int fd);
/*
**函数说明:
	查看uvc设备信息
**参数说明:
	int fd	：uvc设备文件描述符
**返回值:
	其它:		无
	0:			无
*/
void uvc_checkInfo(int fd);
/*
**函数说明:
	设置uvc设备采集数据的分辨率
**参数说明:
	int fd				：uvc设备文件描述符
	PIXELSIZE pixSize	：像素尺寸
**返回值:
	其它:		失败
	0:			成功
*/
int uvc_setResolution(int fd, PIXELSIZE pixSize);
/*
**函数说明:
	设置uvc设备采集数据的原始格式
**参数说明:
	int fd				：uvc设备文件描述符
	PIXELFORMAT format  : 设置的格式
**返回值:
	其它:		失败
	0:			成功
*/
int uvc_setPixelFormat(int fd, PIXELFORMAT format);
/*
**函数说明:
	开始uvc视频流传输
**参数说明:
	int fd				：uvc设备文件描述符
**返回值:
	其它:		失败
	0:			成功
*/
int ucv_start(int fd);
/*
**函数说明:
	读取uvc设备一帧的数据长度，用于应用申请内存
**参数说明:
	无
**返回值:
	-1:		无效的数据长度
	其他:	有效的数据长度
*/
int uvc_getFrameDataLength(void);
/*
**函数说明:
	读取uvc设备一帧的数据
**参数说明:
	int fd						：uvc设备文件描述符
	unsigned char *pfarmebuff	: 读取一帧数据的首地址
	unsigned int size			: buf的大小
**返回值:
	其它:		失败
	size:		读取的字节数成功
*/
int uvc_readFrame(int fd, unsigned char *pfarmebuff, unsigned int size);
/*
**函数说明:
	停止uvc视频流传输
**参数说明:
	int fd				：uvc设备文件描述符
**返回值:
	其它:		失败
	0:			成功
*/
int ucv_stop(int fd);


#endif