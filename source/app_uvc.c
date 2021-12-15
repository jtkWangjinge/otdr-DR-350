/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_uvc.c  
* 摘    要：  u封装uvc驱动的V4L2接口，并提供API给应用
*
* 当前版本：  v1.1.0
* 作    者：  wujungaung
* 完成日期：  
*
*******************************************************************************/
#include "app_uvc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

/****************************
* 为实现APP_UVC内部静态变量定义
****************************/
struct buffer
{
	void *start;
	unsigned int length;
	int isVaild; //非0 默认为无效
};
static struct buffer *m_buffers = NULL; //用于应用层记录将四个已申请到的缓冲帧映射

//带有默认参数的静态变量
static unsigned int m_pixSize[PIXELSIZENUM][2] = {
	{1280, 960}, {1280, 720}, {640, 480}, {320, 240}, {800, 600}, //MJPEG所支持的像素尺寸
	{1280, 960}, {1280, 720}, {640, 480}, {320, 240}, {800, 600} //YUYV所支持的像素尺寸
};
static unsigned int m_pixWidth = 640;  //分辨率的默认宽度
static unsigned int m_PixHeight = 480; //分辨率的默认高度

static unsigned int m_pixelFormat = V4L2_PIX_FMT_YUYV; //默认设置获取的到的原始数据格式为YUYV

/*******************************
* 为实现APP_UVC内部静态函数声明
********************************/

//设置uvc视频格式，当修改分辨率或格式时都需要重新设置
static int SetUvcFormat(int fd);

//建立内存映射
static int SetUvcMemMap(int fd);

//根据格式转换原始数据
static int YUYV2GRAY(unsigned char *des, unsigned int desSize, unsigned char *src, unsigned int srcSize);

/*****************************
* 为实现APP_UVC内部静态函数实现
******************************/
//设置uvc视频格式，当修改分辨率或格式时都需要重新设置
static int SetUvcFormat(int fd)
{
	int ret = 0;
	if (fd < 0)
	{
		printf("%s:set uvc format false\n", __func__);
		ret = -1;
	}
	if (ret == 0)
	{
		//v4l2_format 结构体用来设置摄像头的视频制式
		struct v4l2_format fmt;
		memset(&fmt, 0, sizeof(fmt));
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = m_pixWidth;
		fmt.fmt.pix.height = m_PixHeight;
		fmt.fmt.pix.pixelformat = m_pixelFormat;
		fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
		if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)) //使设置生效
		{
			printf("set uvc format false\n");
			ret = -2;
		}
	}

	return ret;
}

//建立内存映射
static int SetUvcMemMap(int fd)
{
	int ret = 0;
	if (fd < 0)
	{
		printf("%s:set uvc format false\n", __func__);
		ret = -1;
	}
	if (!ret)
	{
		struct v4l2_requestbuffers reqbuf; //请求分配内存并建立映射
		//请求分配内存
		reqbuf.count = UVCBUFNUM;					  //请求分配内存的个数，真实的申请的获得内存并不一定是4个
		reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	  //类型为捕获 模式
		reqbuf.memory = V4L2_MEMORY_MMAP;			  //设置为memory map模式
		if (-1 == ioctl(fd, VIDIOC_REQBUFS, &reqbuf)) //申请缓冲
		{
			printf("v4l2_requestbuffers false\n");
			ret = -2;
		}
		//映设分配上来的内存
		if (!ret)
		{
			//用buffers 指针记录已申请到的缓冲帧映射
			m_buffers = (struct buffer *)calloc(reqbuf.count, sizeof(struct buffer));
			if (!m_buffers)
			{
				printf("calloc struct buffer false/n");
				ret = -3;
			}
			if (!ret)
			{
				unsigned int n_buffers = 0;
				for (n_buffers = 0; n_buffers < reqbuf.count; ++n_buffers)
				{
					struct v4l2_buffer buf;
					memset(&buf, 0, sizeof(buf));
					buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					buf.memory = V4L2_MEMORY_MMAP;
					buf.index = n_buffers;
					// 查询序号为n_buffers 的缓冲区，得到其起始物理地址和大小
					//获取内存空间
					if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf))
					{
						printf("get mem space false\n");
						m_buffers[n_buffers].isVaild = -1;
						continue;
					}
					//获取内存的大小为buf.lengt
					m_buffers[n_buffers].length = buf.length;
					// 映射内存
					//获取内存的类型为V4L2_MEMORY_MMAP分配的是实实在在的物理内存.获取后需要进行map
					m_buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
					if (MAP_FAILED == m_buffers[n_buffers].start)
					{
						printf("get mem space false\n");
						m_buffers[n_buffers].isVaild = -1;
						continue;
					}
					m_buffers[n_buffers].isVaild = 0;
					//将获取的内存放到内存队列，以便获取图像数据
					if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) //VIDIOC_QBUF// 把帧放入队列 // 将缓冲帧放入队列
						printf("%s:insert farme into queue false\n", __func__);
				}
			}
		}
	}
	return ret;
}

//根据格式转换原始数据
static int YUYV2GRAY(unsigned char *des, unsigned int desSize, unsigned char *src, unsigned int srcSize)
{
	if (des == NULL || desSize < 0 || src == NULL || srcSize < 0)
	{
		return -1;
	}
	int i = 0;
	for (i = 0; i < desSize; ++i)
	{
		if (i * 2 < srcSize)
		{
			*(des + i) = *(src + (i * 2));
		}
	}
	return 0;
}
/*****************************
* 为实现APP_UVC外部函数实现
******************************/
/*
**函数说明:
	开始uvc视频流传输
**参数说明:
	int fd				：uvc设备文件描述符
**返回值:
	其它:		失败
	0:			成功
*/
int ucv_start(int fd)
{
	int ret = 0;
	if (fd < 0)
	{
		printf("%s:please open uvc device\n", __func__);
		ret = -1;
	}
	if (!ret)
	{
		//建立内存映射
		ret = SetUvcMemMap(fd);
		if (!ret)
		{
			enum v4l2_buf_type type;
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == ioctl(fd, VIDIOC_STREAMON, &type)) //启动VIDIOC_STREAMON
			{
				printf("%s:start video false\n", __func__);
				ret = -2;
			}
		}
	}
	return ret;
}
/*
**函数说明:
	读取uvc设备一帧的数据长度，用于应用申请内存
**参数说明:
	无
**返回值:
	-1:		无效的数据长度
	其他:	有效的数据长度
*/
int uvc_getFrameDataLength(void)
{
	int len = -1;
	//根据分辨率和原始视频数据的格式确定给定的数据长度
	len = m_pixWidth * m_PixHeight;
	return len;
}
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
int uvc_readFrame(int fd, unsigned char *pfarmebuff, unsigned int size)
{
	int ret = 0;
	if (fd < 0 || pfarmebuff == NULL)
	{
		printf("%s:please open uvc device\n", __func__);
		ret = -1;
	}
	if (!ret)
	{
		//首先要队列中的内存取出来一个
		struct v4l2_buffer buftemp;
		memset(&buftemp, 0, sizeof(buftemp));
		buftemp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buftemp.memory = V4L2_MEMORY_MMAP;
		if (-1 == ioctl(fd, VIDIOC_DQBUF, &buftemp)) // 从缓冲区取出一个缓冲帧----内存空间出队列
		{
			printf("%s:get one frame false\n", __func__);
			ret = -2;
		}
		if (!ret)
		{
			if (!m_buffers[buftemp.index].isVaild)
			{
				unsigned int farmeBuffTempSize = m_buffers[buftemp.index].length;
				unsigned char *farmebufftemp = malloc(farmeBuffTempSize);
				if (farmebufftemp != NULL)
				{
					memcpy(farmebufftemp, m_buffers[buftemp.index].start, m_buffers[buftemp.index].length);
					switch (m_pixelFormat)
					{
					case V4L2_PIX_FMT_YUYV:
						//YUYV 转 灰度
						if (size * 2 == farmeBuffTempSize)
						{
							if (YUYV2GRAY(pfarmebuff, size, farmebufftemp, farmeBuffTempSize) == 0)
							{
								//读取成功
								ret = size;
							}
						}
						else
						{
							printf("%s:size is:%d farmeBuffTempSize:%d\n", __func__, size * 2, farmeBuffTempSize);
							ret = -4;
						}
						break;
					default:
						ret = -5;
						break;
					}
					free(farmebufftemp);
				}
				if (-1 == ioctl(fd, VIDIOC_QBUF, &buftemp)) // 内存重新入队列
				{
					printf("%s:insert one frame false\n", __func__);
					ret = -2;
				}
			}
			else
			{
				printf("%s:get one frame data memcpy false\n", __func__);
				ret = -6;
			}
		}
	}
	return ret;
}
/*
**函数说明:
	停止uvc视频流传输
**参数说明:
	int fd				：uvc设备文件描述符
**返回值:
	其它:		失败
	0:			成功
*/
int ucv_stop(int fd)
{
	int ret = 0;
	if (fd < 0)
	{
		printf("%s:please open uvc device\n", __func__);
		ret = -1;
	}
	if (!ret)
	{
		enum v4l2_buf_type type;
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
		{
			printf("%s:VIDIOC_STREAMOFF false\n", __func__);
			ret = -2;
		}
	}
	return ret;
}

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
int uvc_setPixelFormat(int fd, PIXELFORMAT format)
{
	int ret = 0;
	if (fd < 0)
	{
		ret = -1;
	}
	if (!ret)
	{
		struct v4l2_fmtdesc fmtdesc;
		fmtdesc.index = 0;
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0)
		{
			if (fmtdesc.pixelformat == format)
			{
				ret = 0;
				break;
			}
			ret = -2;
			fmtdesc.index++;
		}
		if (!ret)
		{
			m_pixelFormat = format;
			ret = SetUvcFormat(fd);
		}
	}
	return ret;
}

/*
**函数说明:
	设置uvc设备采集数据的分辨率
**参数说明:
	int fd					：uvc设备文件描述符
	PIXELSIZE pixSize 		：分辨率尺寸
**返回值:
	其它:		失败
	0:			成功
*/
int uvc_setResolution(int fd, PIXELSIZE pixSize)
{
	int ret = 0;
	if (pixSize < 0 || pixSize >= PIXELSIZENUM || fd < 0)
	{
		ret = -1;
	}
	if (!ret)
	{
		m_pixWidth = m_pixSize[pixSize][0];
		m_PixHeight = m_pixSize[pixSize][1];
		ret = SetUvcFormat(fd);
	}
	return ret;
}

/*
**函数说明:
	查看uvc设备信息
**参数说明:
	int fd	：uvc设备文件描述符
**返回值:
	其它:		无
	0:			无
*/
void uvc_checkInfo(int fd)
{
	int ret = 0;
	if (fd < 0)
	{
		printf("%s:please open uvc device\n", __func__);
		return;
	}
	//打印显示设备信息
	struct v4l2_capability cap;
	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0)
	{
		printf("check uvc info ioctl false\n");
		return;
	}
	printf("Driver Name:%s\nCard Name:%s\nBus info:%s\nDriver Version:%u.%u.%u\n",
		   cap.driver, cap.card, cap.bus_info, (cap.version >> 16) & 0XFF, (cap.version >> 8) & 0XFF, cap.version & 0XFF);

	//显示当前帧的相关信息
	//v4l2_format 结构体用来设置摄像头的视频制式、帧格式等
	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = m_pixWidth;
	fmt.fmt.pix.height = m_PixHeight;
	fmt.fmt.pix.pixelformat = m_pixelFormat;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(fd, VIDIOC_G_FMT, &fmt);
	printf("Current data format information:\n\twidth:%d\n\theight:%d\n",
		   fmt.fmt.pix.width, fmt.fmt.pix.height);
	//查询并显示所有支持的格式：VIDIOC_ENUM_FMT
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("Support format:\n");
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0)
	{
		printf("\t%d.%s\n", fmtdesc.index, fmtdesc.description);
		if (fmtdesc.pixelformat == fmt.fmt.pix.pixelformat)
		{
			printf("\tcurrent support data format is:%s\n", fmtdesc.description);
		}
		fmtdesc.index++;
	}
	return;
}

/*
**函数说明:
	关闭uvc设备，并释放由open_uvc函数分配的内存空间
**参数说明:
	int fd	：需要关闭的文件描述符
**返回值:
	0	--成功
	-1	--失败
*/
int uvc_close(int fd)
{
	int ret = 0;
	int i = 0;
	if (fd < 0)
	{
		ret = -1;
	}
	if (!ret)
	{
		//本地内存释放
		if (m_buffers != NULL)
		{
			for (i = 0; i < UVCBUFNUM; i++)
			{
				if (!m_buffers[i].isVaild)
				{
					munmap(m_buffers[i].start, m_buffers[i].length);
					m_buffers[i].isVaild = -1;
				}
			}
			free(m_buffers);
			m_buffers = NULL;
		}
		ret = close(fd);
	}
	return ret;
}

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
int uvc_open(const char *pathname, int flags)
{
	int fd = -1;

	fd = open(pathname, flags);
	if (fd < 0)
	{
		perror("open /dev/video0 false\n");
	}
	else
	{
		//设置默认的uvc设置
		SetUvcFormat(fd);
	}
	return fd;
}
