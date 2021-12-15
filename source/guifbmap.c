/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guifbmap.h
* 摘    要：  GUIFBMAP模块主要完成设备抽象层定义及实现，以便GUI能够适用于不同硬
*             件平台对应用层隔离硬件差异。guifbmap.c实现了guifbmap.h中声明的帧
*             缓冲设备的相关操作，用于实现GUI的图形输出设备抽象。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#include "guifbmap.h"


/*************************************
* 为实现GUIFBMAP而需要引用的其他头文件
*************************************/
#include "guidevice.h"
#include "guischeduler.h"


/**************************
* 定义GUI中的当前帧缓冲映射
**************************/
static GUIFBMAP *pCurrFbmap = NULL;


/***
  * 功能：
        根据指定的信息直接创建帧缓冲映射对象
  * 参数：
        1.char *strDevName:         需要创建帧缓冲映射对象的设备的名称
        2.int iDevType:             帧缓冲设备类型，0标准、1双缓冲、2页交换
        3.unsigned int uiHorDisp:   映射后用于显示的水平分辨率
        4.unsigned int uiVerDisp:   映射后用于显示的垂直分辨率
        5.THREADFUNC fnFbThread:    回调函数，用于创建帧缓冲的线程
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIFBMAP* CreateFbmap(char *strDevName, int iDevType, 
                      unsigned int uiHorDisp, unsigned int uiVerDisp, 
                      THREADFUNC fnFbThread)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUIFBMAP *pFbmapObj = NULL;
    //临时变量定义
  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:fb_var_screeninfo,fb_fix_screeninfo
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    if (!iErr)
    {
        //判断strDevName和fnFbThread是否为有效指针
        if (NULL == strDevName || NULL == fnFbThread)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //尝试为帧缓冲映射对象分配资源
        pFbmapObj = (GUIFBMAP *) malloc(sizeof(GUIFBMAP));
        if (NULL == pFbmapObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:open(),ioctl(),mmap(),fb_fix,fb_var
    if (!iErr)
    {
        //尝试打开设备文件
        pFbmapObj->iDevFd = open(strDevName, O_RDWR);
        if (-1 == pFbmapObj->iDevFd)
        {
            iErr = -3;
        }
        //设置帧缓冲映射对象的iDevType
        pFbmapObj->iDevType = iDevType;
    }

    if (!iErr)
    {
        //尝试调用ioctl ()，获取fb_fix_screeninfo和fb_var_screeninfo
        if ((-1 == ioctl(pFbmapObj->iDevFd, FBIOGET_FSCREENINFO, &fb_fix)) ||
            (-1 == ioctl(pFbmapObj->iDevFd, FBIOGET_VSCREENINFO, &fb_var)))
        {
            iErr = -4;
        }
    }

    if (!iErr)
    {
        //设置uiMapOffset，uiMapLength，尝试执行mmap()并设置pFbEntry
        pFbmapObj->uiMapOffset = fb_fix.smem_start & (sysconf(_SC_PAGESIZE) - 1);
        pFbmapObj->uiMapLength = fb_fix.smem_len + pFbmapObj->uiMapOffset;
        pFbmapObj->pFbEntry = mmap(NULL, pFbmapObj->uiMapLength, 
                                   PROT_READ|PROT_WRITE, MAP_SHARED, 
                                   pFbmapObj->iDevFd, pFbmapObj->uiMapOffset);
        if (pFbmapObj->pFbEntry == MAP_FAILED)
        {
            iErr = -5;
        }
    }

    if (!iErr)
    {
        //检测帧缓冲映射对象的像素深度是否能够支持
        if ((fb_var.bits_per_pixel != 8) && 
            (fb_var.bits_per_pixel != 16) && 
            (fb_var.bits_per_pixel != 24) && 
            (fb_var.bits_per_pixel != 32))
        {
            iErr = -6;
        }
        //对于像素深度为16位的帧缓冲映射对象，检验其是否为RGB565
        if (16 == fb_var.bits_per_pixel)
        {
            if ((fb_var.red.length != 5) || 
                (fb_var.red.offset != 11) ||
                (fb_var.green.length != 6) || 
                (fb_var.green.offset != 5) ||
                (fb_var.blue.length != 5) ||
                (fb_var.blue.offset != 0))
            {
                iErr = -6;
            }
        }
        //此处注释的原因是驱动映射的帧缓冲分辨率不符合以下条件
        //确保用于显示的分辨率小于帧缓冲分辨率
        // if ((uiHorDisp > fb_var.xres) || 
        //     (uiVerDisp > fb_var.yres))
        // {
        //     iErr = -6;
        // }
    }

    if (!iErr)
    {
        //帧缓冲映射对象中与系统相关的部分成员赋值
        pFbmapObj->uiBufSize = fb_fix.smem_len;
        pFbmapObj->uiPixelBits = fb_var.bits_per_pixel;
        pFbmapObj->uiRedBits = fb_var.red.length;
        pFbmapObj->uiRedShift = fb_var.red.offset;
        pFbmapObj->uiGreenBits = fb_var.green.length;
        pFbmapObj->uiGreenShift = fb_var.green.offset;
        pFbmapObj->uiBlueBits = fb_var.blue.length;
        pFbmapObj->uiBlueShift = fb_var.blue.offset;
        pFbmapObj->uiHorRes = uiHorDisp; //fb_var.xres;
        pFbmapObj->uiVerRes = uiVerDisp; //fb_var.yres;
        //对uiHorDisp和uiVerDisp进行设置，并计算uiHorOff和uiVerOff
        pFbmapObj->uiHorDisp = uiHorDisp;//由于驱动映射的帧缓冲与传入的显示分辨率不符合，此处强制修改
        pFbmapObj->uiVerDisp = uiVerDisp;//
        pFbmapObj->uiHorOff = (pFbmapObj->uiHorRes - uiHorDisp) / 2;
        pFbmapObj->uiVerOff= (pFbmapObj->uiVerRes - uiVerDisp) / 2;
        //iSyncFlag初始化为0，并根据iDevType的值对pMapBuff进行设置
        pFbmapObj->iSyncFlag = 0;
        switch (iDevType)
        {
        case GUI_FBTYP_STD:
            pFbmapObj->pMapBuff = pFbmapObj->pFbEntry;
            break;
        case GUI_FBTYP_DUAL:
        case GUI_FBTYP_CUST:
            pFbmapObj->pMapBuff = malloc(pFbmapObj->uiBufSize);
            if (NULL == pFbmapObj->pMapBuff)
            {
                iErr = -7;
            }
            break;
        default:
            iErr = -7;
            break;
        }
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    if (!iErr)
    {
        //设置新建帧缓冲映射对象的fnFbThread，并尝试初始化Mutex
        pFbmapObj->fnFbThread = fnFbThread;
        if (InitMutex(&(pFbmapObj->Mutex), NULL))
        {
            iErr = -8;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:munmap(),close()
    //错误处理
    switch (iErr)
    {
    case -8:
        if (iDevType == GUI_FBTYP_DUAL)
        {
            free(pFbmapObj->pMapBuff);
        }
        //no break
    case -7:
    case -6:
        munmap(pFbmapObj->pFbEntry, pFbmapObj->uiMapLength);
        //no break
    case -5:
    case -4:
        close(pFbmapObj->iDevFd);
        //no break
    case -3:
        free(pFbmapObj);
        //no break
    case -2:
    case -1:
        pFbmapObj = NULL;
        //no break
    default:
        break;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return pFbmapObj;
}


/***
  * 功能：
        删除帧缓冲映射对象
  * 参数：
        1.GUIFBMAP **ppFbmapObj:    指针的指针，指向需要销毁的帧缓冲映射对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyFbmap(GUIFBMAP **ppFbmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (!iErr)
    {
        //判断ppFbmapObj是否有效
        if (NULL == ppFbmapObj)
        {
            iErr = -1;
        }
    }

    if (!iErr)
    {
        //判断ppFbmapObj所指向的是否为有效指针
        if (NULL == *ppFbmapObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:munmap(),close()
    if (!iErr)
    {
        //释放输入缓冲
        if ((*ppFbmapObj)->iDevType == GUI_FBTYP_DUAL)
        {
            free((*ppFbmapObj)->pMapBuff);
        }
        //撤销内存映射
        if (munmap((*ppFbmapObj)->pFbEntry, (*ppFbmapObj)->uiMapLength))
        {
            iErr = -3;
        }
        //关闭设备
        if (close((*ppFbmapObj)->iDevFd))   //设备文件为共享资源，需验证返回值
        {
            iErr = -4;
        }
        //销毁帧缓冲映射对象，并将指针置空
        free(*ppFbmapObj);
        *ppFbmapObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif    //GUI_OS_ENV == LINUX_OS

    return iErr;
}


/***
  * 功能：
        得到当前帧缓冲映射对象
  * 参数：
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUIFBMAP* GetCurrFbmap(void)
{
    return pCurrFbmap;
}


/***
  * 功能：
        设置当前帧缓冲映射对象
  * 参数：
        1.GUIFBMAP *pFbmapObj:  被设置为当前帧缓冲映射对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrFbmap(GUIFBMAP *pFbmapObj)
{
    //判断pFbmapObj是否有效
    if (NULL == pFbmapObj)
    {
        return -1;
    }

    pCurrFbmap = pFbmapObj;

    return 0;
}


/***
  * 功能：
        设置帧缓冲映射的同步标志
  * 参数：
        1.int iSyncFlag:        输入缓冲需要同步刷新时的同步标志位
        2.GUIFBMAP *pFbmapObj:  需要设置同步标志的帧缓冲映射对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        函数将自动加锁（帧缓冲映射对象中的互斥锁），不能在持有锁的前提下调用
***/
int SetFbmapSync(int iSyncFlag, GUIFBMAP *pFbmapObj)
{
    //加锁
    MutexLock(&(pFbmapObj->Mutex));

    //判断pFbmapObj是否有效
    if (NULL == pFbmapObj)
    {
        return -1;
    }

    //设置同步标志位
    //pFbmapObj->iSyncFlag = iSyncFlag;

    //解锁
    MutexUnlock(&(pFbmapObj->Mutex));

    return 0;
}


/***
  * 功能：
        默认帧缓冲线程入口函数
  * 参数：
        1.void *pThreadArg:     线程入口函数参数，类型为(GUIFBMAP *)
  * 返回：
        返回线程结束装态
  * 备注：
***/
void* DefaultFbThread(void *pThreadArg)
{
    //错误标志、返回值定义
    int iErr = 0;
    static int iReturn = 0;
    //临时变量定义
    int iExit = 0;
    GUIFBMAP *pFbmapObj = pThreadArg;

    while (!iExit)
    {
        //根据结束标志判断是否需要结束帧缓冲线程
        iExit = GetExitFlag();
        if (iExit)
        {
            break;
        }

        //完成同步刷新输入缓冲
        switch (pFbmapObj->iDevType)
        {
        case GUI_FBTYP_STD:
            iErr = StandardFbmapProcess(pFbmapObj);
            break;
        case GUI_FBTYP_DUAL:
            iErr = DualFbmapProcess(pFbmapObj);
            break;
        case GUI_FBTYP_CUST:
            iErr = CustomFbmapProcess(pFbmapObj);
            break;
        default:
            break;
        }

        //出错为-1，表示无帧缓冲数据可刷新，睡眠50毫秒
        if (-1 == iErr)
        {
            MsecSleep(50);
        }
    }

    //退出线程
    ThreadExit(&iReturn);
    return &iReturn;    //该语句只是为了消除编译器报警，实际上不起作用
}


/***
  * 功能：
        标准类型帧缓冲的处理函数，用于完成帧缓冲线程的处理
  * 参数：
        1.GUIFBMAP *pFbmapObj:  处理函数要处理的帧缓冲对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        返回-1表示无帧缓冲数据可刷新                    
***/
int StandardFbmapProcess(GUIFBMAP *pFbmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    //加锁
    MutexLock(&(pFbmapObj->Mutex));

    //标准类型帧缓冲不进行任何处理
    if (pFbmapObj->iSyncFlag)
    {
        pFbmapObj->iSyncFlag = 0;
    }
    else
    {
        iErr = -1;
    }

    //解锁
    MutexUnlock(&(pFbmapObj->Mutex));

    return iErr;
}


/***
  * 功能：
        双缓冲类型帧缓冲的处理函数，用于完成帧缓冲线程的处理
  * 参数：
        1.GUIFBMAP *pFbmapObj:  处理函数要处理的帧缓冲对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        返回-1表示无帧缓冲数据可刷新                    
***/
int DualFbmapProcess(GUIFBMAP *pFbmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    //加锁
    MutexLock(&(pFbmapObj->Mutex));

    //判断是否需要同步刷新输入缓冲
    if (pFbmapObj->iSyncFlag)
    {
        //完成双缓冲类型帧缓冲的同步
        memcpy(pFbmapObj->pFbEntry, pFbmapObj->pMapBuff, pFbmapObj->uiBufSize);
        pFbmapObj->iSyncFlag = 0;
    }
    else
    {
        iErr = -1;
    }

    //解锁
    MutexUnlock(&(pFbmapObj->Mutex));

    return iErr;
}


/***
  * 功能：
        自定义类型帧缓冲的处理函数，用于完成帧缓冲线程的处理
  * 参数：
        1.GUIFBMAP *pFbmapObj:  处理函数要处理的帧缓冲对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        返回-1表示无帧缓冲数据可刷新                    
***/
int CustomFbmapProcess(GUIFBMAP *pFbmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    //调用设备抽象层函数完成自定义类型帧缓冲的处理
    iErr = CustomFbmapFunc(pFbmapObj);

    return iErr;
}

