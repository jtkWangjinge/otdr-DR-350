/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部 
*
* All rights reserved
*
* 文件名称：  guiconfig.h
* 摘    要：  GUI运行环境配置文件，配置系统环境、硬件环境以及资源环境
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_CONFIG_H
#define _GUI_CONFIG_H

    
/*******************************
* 定义GUI所依赖的操作系统环境
* 注：以下内容需根据实际情况修改
*******************************/
#define NONE_OS             0           //无操作系统
#define LINUX_OS            1           //Linux操作系统
#define GUI_OS_ENV          LINUX_OS    //GUI当前所处的操作系统环境

/*******************************
* 定义GUI中的支持的像素格式
* 注：以下内容需根据实际情况修改
*******************************/
#define PIXEL_8BITS     0
#define PIXEL_16BITS    1
#define PIXEL_24BITS    0
#define PIXEL_32BITS    0

#define MODEL_NUMBER    "O10"//PROJECT_NAME
/*******************************
* 定义GUI中的资源环境配置
* 注：以下内容需根据实际情况修改
*******************************/
#ifdef WNDPROC_SDCARD
#define MntSDcardDirectory "/mnt/sdcard/"
//版本路径
#define VERSION_PATH "/mnt/sdcard/version.bin"
#else
#define MntSDcardDirectory "/app/"
//版本路径
#define VERSION_PATH "/app/version.bin"
#endif
#ifdef VIEW100
#define MntUsbDirectory "/mnt/sdcard/mnt/usb"
#define MntSDcardDataDirectory "/mnt/sdcard/mnt/sdcard"
#define MntUpDirectory "/mnt/sdcard/mnt"
#else
#define MntUsbDirectory "/mnt/usb"
#define MntSDcardDataDirectory "/mnt/sdcard"
#define MntDataDirectory "/mnt/dataDisk"
#define MntUpDirectory "/mnt"
#endif

#ifdef VB
#define VersionB 
#else
#define VersionV 
#endif
//不同分辨率的屏幕加载不同的图片库
#ifdef SKIN_320_240
#define BmpFileDirectory MntSDcardDirectory"bitmap/skin0/"
#elif SKIN_640_480
#define BmpFileDirectory MntSDcardDirectory"bitmap/skin1/"
#endif
#define FontFileDirectory MntSDcardDirectory"font/"
#define LANGEUAGE_DB_FILE FontFileDirectory"language.db"
#define INPUTCNPATH		 MntSDcardDirectory"font/chineseinput/china.db"

#define RUNPATH_ROOT    "/home/ubuntu/workspace/guilib"
#define RESFILE_PATH    RUNPATH_ROOT"/run"
#define FNTFILE_STD     FontFileDirectory"unicode16.fnt"
#define FNTFILE_BIG     FontFileDirectory"unicode24.fnt"

//界面宽高
#ifdef SKIN_320_240
#define WINDOW_WIDTH    320
#define WINDOW_HEIGHT   240
#elif SKIN_640_480
#define WINDOW_WIDTH    640
#define WINDOW_HEIGHT   480
#endif


#endif  //_GUI_CONFIG_H

