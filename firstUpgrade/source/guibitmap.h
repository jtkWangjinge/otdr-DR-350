/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guibitmap.h
* 摘    要：  定义GUI位图类型及声明相关操作，实现BMP图片的绘制。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  2012-10-12
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _GUIBITMAP_H
#define _GUIBITMAP_H


/**************************************
* 为定义GUIBITMAP而需要引用的其他头文件
**************************************/
#include "guibase.h"
#include "guiimage.h"


/*********************************
* 定义GUI中与BMP文件格式相关的类型
*********************************/
//对应.bmp的文件头结构
typedef struct _bitmap_fileheader
{
    UINT16 usFillChar;      //填充字符，用于对齐字边界
    UINT16 usFileType;      //文件类型，0x424D('BM')为文件中的存储序列，小端存储
    UINT32 uiFileSize;      //文件大小，文件占用的字节大小
    UINT16 usReserved1;     //保留字1，值必须为0
    UINT16 usReserved2;     //保留字2，值必须为0
    UINT32 uiDataOffset;    //偏移量，文件头到实际位图数据的偏移量，以字节为单位
} GUIBITMAP_FILEHEADER;

//对应.bmp的信息头结构
typedef struct _bitmap_infoheader
{
    UINT32 uiInfoSize;      //信息头大小，该信息头占用的字节大小，值为40
    UINT32 uiImgWidth;      //图像宽度
    UINT32 uiImgHeight;     //图像高度，目前只支持倒向位图(左下角为原点)
    UINT16 usDevPlanes;     //目标设备位面数，值必须为1
    UINT16 usBitCount;      //每像素所占比特数，目前只支持1位色、8位色、24位色
    UINT32 uiCompression;   //压缩算法，目前只支持不压缩图像(BI_RGB，BI_RGB==0)
    UINT32 uiImgSize;       //图像大小，图像占用的字节大小，其值必须为4的倍数
    UINT32 uiXRes;          //水平分辨率，单位为像素/米
    UINT32 uiYRes;          //垂直分辨率，单位为像素/米
    UINT32 uiClrUsed;       //实际使用的颜色数
    UINT32 uiClrImportant;  //重要的颜色数
} GUIBITMAP_INFOHEADER;

//对应.bmp的调色板结构
typedef struct _bitmap_rgbquad
{
    UINT8 ucBlue;       //该颜色的蓝色分量
    UINT8 ucGreen;      //该颜色的绿色分量
    UINT8 ucRed;        //该颜色的红色分量
    UINT8 ucReserved;   //保留值
} GUIBITMAP_RGBQUAD;


/******************
* 定义GUI中位图结构
******************/
typedef struct _bitmap
{
    char *strBitmapFile;        //位图文件名称
    int iBitmapFd;              //位图文件描述符

    unsigned int uiViewWidth;   //显示宽度，>=0，初始化为0则按BMP实际宽度
    unsigned int uiViewHeight;  //显示高度，>=0，初始化为0则按BMP实际高度

    unsigned int uiPlaceX;      //位图水平放置位置，以左上角为基点
    unsigned int uiPlaceY;      //位图垂直放置位置，以左上角为基点

    GUIIMAGE *pBitmapImg;       //位图所对应的图像资源
} GUIBITMAP;


/**********************************
* 定义GUI中与位图类型相关的操作函数
**********************************/
//根据指定的信息直接建立位图对象
GUIBITMAP* CreateBitmap(char *strBitmapFile, 
                        unsigned int uiPlaceX, unsigned int uiPlaceY);
//删除位图对象
int DestroyBitmap(GUIBITMAP **ppBitmapObj);

//直接输出指定的位图
int DisplayBitmap(GUIBITMAP *pBitmapObj);

//设置位图的显示大小
int SetBitmapView(unsigned int uiViewWidth, unsigned int uiViewHeight, 
                  GUIBITMAP *pBitmapObj);
//设置位图的放置位置
int SetBitmapPlace(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                   GUIBITMAP *pBitmapObj);

//装载位图图像资源
int LoadBitmapImage(GUIBITMAP *pBitmapObj);
//卸载位图图像资源
int UnloadBitmapImage(GUIBITMAP *pBitmapObj);
//读取位图中的调色板
GUIPALETTE* ReadBitmapPalette(GUIBITMAP *pBitmapObj);


#endif  //_GUIBITMAP_H

