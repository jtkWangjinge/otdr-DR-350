/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guifbmap.h
* 摘    要：  GUIFBMAP模块主要完成设备抽象层定义及实现，以便GUI能够适用于不同硬
*             件平台对应用层隔离硬件差异。guifbmap.h定义了GUI的帧缓冲映射类型及
*             声明相关操作，用于实现GUI的图形输出设备抽象。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：
*******************************************************************************/

#ifndef _GUI_FBMAP_H
#define _GUI_FBMAP_H


/********************v****************
* 为定义GUIFBMAP而需要引用的其他头文件
*************************************/
#include "guibase.h"


/************************
* 定义GUI中帧缓冲设备类型
************************/
#define GUI_FBTYP_STD           1   //标准类型帧缓冲
#define GUI_FBTYP_DUAL          2   //双缓冲类型帧缓冲
#define GUI_FBTYP_CUST          3   //自定义类型帧缓冲


/************************
* 定义GUI中帧缓冲映射结构
************************/
typedef struct _fbmap
{
    int iDevFd;                 //帧缓冲设备文件描述符
    int iDevType;               //帧缓冲设备类型，1标准、2双缓冲

    unsigned int uiMapOffset;   //mmap()映射时的页偏移，munmap()时使用该值
    unsigned int uiMapLength;   //mmap()映射时的长度，munmap()时使用该值
    void *pFbEntry;             //经mmap()映射后得到的帧缓冲入口

    unsigned int uiBufSize;     //帧缓冲大小，字节单位
    unsigned int uiPixelBits;   //像素深度，8、16(RGB565)、24、32(即非压缩24位)
    unsigned int uiRedBits;     //每像素中红色分量所占位数
    unsigned int uiRedShift;    //每像素中红色分量起始位置
    unsigned int uiGreenBits;   //每像素中绿色分量所占位数
    unsigned int uiGreenShift;  //每像素中绿色分量起始位置
    unsigned int uiBlueBits;    //每像素中蓝色分量所占位数
    unsigned int uiBlueShift;   //每像素中蓝色分量起始位置
    unsigned int uiHorRes;      //帧缓冲水平分辨率
    unsigned int uiVerRes;      //帧缓冲垂直分辨率

    unsigned int uiHorDisp;     //映射后用于显示的水平分辨率，必须<=uiHorRes
    unsigned int uiVerDisp;     //映射后用于显示的垂直分辨率，必须<=uiVerRes
    unsigned int uiHorOff;      //水平偏移，uiHorOff=(uiHorRes-uiHorDisp)/2
    unsigned int uiVerOff;      //垂直偏移，uiVerOff=(uiVerRes-uiVerDisp)/2

    int iSyncFlag;              //同步标志，当输入缓冲需要同步刷新时置1
    void *pMapBuff;             //输入缓冲，GUI对象总是将其自身绘制到该缓冲

    THREADFUNC fnFbThread;      //回调函数，用于创建帧缓冲线程
    GUIMUTEX Mutex;             //互斥锁，用于控制帧缓冲线程对输入缓冲的访问
} GUIFBMAP;


/****************************************
* 定义GUI中与帧缓冲映射类型相关的操作函数
****************************************/
//根据指定的信息直接创建帧缓冲映射对象
GUIFBMAP* CreateFbmap(char *strDevName, int iDevType, 
                      unsigned int uiHorDisp, unsigned int uiVerDisp, 
                      THREADFUNC fnFbThread);
//删除帧缓冲映射对象
int DestroyFbmap(GUIFBMAP **ppFbmapObj);

//得到当前帧缓冲映射对象
GUIFBMAP* GetCurrFbmap(void);
//设置为当前帧缓冲映射对象
int SetCurrFbmap(GUIFBMAP *pFbmapObj);

//设置帧缓冲映射的同步标志
int SetFbmapSync(int iSyncFlag, GUIFBMAP *pFbmapObj);


/**************************
* 定义GUI中的默认帧缓冲线程
**************************/
//默认帧缓冲线程入口函数
void* DefaultFbThread(void *pThreadArg);

//标准类型帧缓冲的处理函数，用于完成帧缓冲线程的处理
int StandardFbmapProcess(GUIFBMAP *pFbmapObj);
//双缓冲类型帧缓冲的处理函数，用于完成帧缓冲线程的处理
int DualFbmapProcess(GUIFBMAP *pFbmapObj);
//自定义类型帧缓冲的处理函数，用于完成帧缓冲线程的处理
int CustomFbmapProcess(GUIFBMAP *pFbmapObj);


#endif  //_GUI_FBMAP_H

