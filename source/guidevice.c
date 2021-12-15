/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部 
*
* All rights reserved
*
* 文件名称：  guidevice.c
* 摘    要：  实现GUI基本输入/输出设备的用户自定义操作函数，为GUI所使用的这些设
*             备提供设备抽象层。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：
* 完成日期：  
*******************************************************************************/
 
#include "guidevice.h"
    
#define TOUCH_MOVE_INTERVAL 	150000 //150ms
#define INTERVAL	60	
#define x 0
#define y 1
#define MT_MAX		5	//最多支持5点触控
#define EVENT_NUM   2   //事件个数
/**************************************
* 为实现GUIDEVICE而需要引用的其他头文件
**************************************/
#include "guievent.h"

//检测待机和自动关机的标志位
#include "wnd_frmstandbyset.h"

#include "app_getsetparameter.h"
#include "app_frmbrightset.h"

#include "wnd_global.h"
#include "wnd_frmotdr.h"

extern PSYSTEMSET pCurSystemSet;
extern unsigned char GucKeyDownFlg;
extern unsigned char GucScreenOffFlg;
extern unsigned char GucPowerOffFlg;
extern unsigned char GucExecFlg;
//add by SHI-2015-1-22

static UINT32 uiMTCenter;


/***
  * 功能：
        用户自定义的帧缓冲处理函数，用于完成自定义类型帧缓冲的处理
  * 参数：
        1.GUIFBMAP *pFbmapObj:  处理函数的帧缓冲对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        1.必须返回-1表示无帧缓冲数据可刷新                    
        2.请根据实际情况修改代码 
***/
int CustomFbmapFunc(GUIFBMAP *pFbmapObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    //加锁
    //请勿随意修改本段代码
    MutexLock(&(pFbmapObj->Mutex));

    if (pFbmapObj->iSyncFlag)
    {
        //此段根据实际情况完成相应代码
        //...
        pFbmapObj->iSyncFlag = 0;
    }
    else
    {
        iErr = -1;
    }

    //解锁
    //请勿随意修改本段代码
    MutexUnlock(&(pFbmapObj->Mutex));

    return iErr;
}

/***
  * 功能：
        检查按键是否为支持的键值
  * 参数：
        1.unsigned int uiKeyValue:  按键的键值
  * 返回：
        按键键值符合要求返回零，否则返回负值
  * 备注：
***/
static int CheckKeyValue(unsigned int uiKeyValue)
{
	unsigned int keyValueArray[MAX_KEY_COUNT] = {
		KEYCODE_SHIFT, KEYCODE_OTDR_LEFT, KEYCODE_OTDR_RIGHT, KEYCODE_START,
		KEYCODE_UP, KEYCODE_DOWN, KEYCODE_LEFT, KEYCODE_RIGHT, KEYCODE_ENTER,
		KEYCODE_FILE, KEYCODE_VFL, KEYCODE_ESC};

	int i = 0;
	for (; i < MAX_KEY_COUNT; ++i)
	{
		if (uiKeyValue == keyValueArray[i])
		{
			return 0;
		}
	}

	return -1;
}

/***
  * 功能：
        用户自定义的键盘处理函数，用于完成自定义类型键盘的处理
  * 参数：
        1.GUIKEYPAD *pKeypadObj:    处理函数的键盘对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        1.必须返回-1表示无键盘数据可读
        2.请根据实际情况修改代码 
***/
int CustomKeypadFunc(GUIKEYPAD *pKeypadObj)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iTmp;
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:input_event
	  struct input_event event;
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif	  //GUI_OS_ENV == LINUX_OS
    GUIEVENT_ITEM *pEventItem = NULL;
    GUIEVENT *pEventObj = NULL;
	
    //加锁
    //请勿随意修改本段代码
    MutexLock(&(pKeypadObj->Mutex));
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:read()
	  if (!iErr)
	  {
		  //读取触摸屏返回的数据
		  ThreadTestcancel();
		  iTmp = read(pKeypadObj->iDevFd, &event, sizeof(event));
		  ThreadTestcancel();
		  if (iTmp != sizeof(event))
		  {
			  iErr = -1;
		  }
	  }
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif	  //GUI_OS_ENV == LINUX_OS

	if (!iErr)
    {
		if (EV_KEY == event.type)
		{
            if (KEYCODE_ESC == event.code)
            {
                //长按或长按弹起消息都不能下发
                if (event.value >= 2)
                {
                    iErr = -2; //不发送消息处理
                    if (event.value == 2) //长按esc键截屏
                        KeyDownScreenShot();
                }
			}
		}
    }
	 
    if (!iErr)
    {
		//检测按键是否支持？
		if(CheckKeyValue(event.code))
		{
			iErr = -4;
		}
        //尝试为按键事件项分配内存
        //请勿随意修改本段代码
        pEventItem = (GUIEVENT_ITEM *) malloc(sizeof(GUIEVENT_ITEM));
        if (NULL == pEventItem)
        {
            iErr = -2;
        }
    }

#if (GUI_OS_ENV == LINUX_OS)  //OS相关:event
	if (!iErr)
	{
		//生成GUIEVENT_ITEM
		switch (event.type)
		{
        case EV_KEY:
        {
            int eventCodeArray[4] = {GUIEVENT_KEY_UP, GUIEVENT_KEY_DOWN, GUIEVENT_KEY_PRESS, GUIEVENT_KEY_CANCEL_PRESS};
            pEventItem->uiEventCode = eventCodeArray[event.value];
            pEventItem->uiEventValue = event.code;
            //事件项写入事件队列
            //请勿随意修改本段代码
            pEventObj = GetCurrEvent();
            if (WriteEventQueue(pEventItem, pEventObj))
            {
                iErr = -3;
            }
        }
			break;
		case EV_REL:			  
			break;
		default:
			iErr = -3;
			break;
		}
	}
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif	  //GUI_OS_ENV == LINUX_OS


	if (!iErr)
    {
		//待机和自动关机需要
		int i = 0;
		GucKeyDownFlg = 1;			//有按键动作
		if(GucScreenOffFlg)			//屏幕已自动关闭			
		{
			GucExecFlg = 0;
			GucScreenOffFlg = 0;
			GucPowerOffFlg = 0;
			
			for(i=0; i<10; i++)
			{
				#ifdef POWER_DEVICE
				BrightnessSet(i);
				#endif
			}
#ifdef POWER_DEVICE
			/* 屏幕亮度恢复 */
	        SetBrightness(pCurSystemSet->uiDCLCDBright, pCurSystemSet->uiACLCDBright);   
#endif
		}
	}

    //错误处理
    //此段根据实际情况完成相应代码
    switch (iErr)
    {
    case -3:
        free(pEventItem);
        //no break
    case -2:
    case -1:
    default:
        break;
    }

    //解锁
    //请勿随意修改本段代码
    MutexUnlock(&(pKeypadObj->Mutex));

    return iErr;
}

/***
  * 功能：
        用户自定义的触摸屏处理函数，用于完成自定义类型触摸屏的处理
  * 参数：
        1.GUIKEYPAD *pTouchObj:    处理函数的键盘对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        1.必须返回-1表示无触摸屏数据可读
        2.请根据实际情况修改代码 
***/
int CustomTouchFunc(GUITOUCH *pTouchObj)
{
	//错误标志、返回值定义
	int iErr = 0;
	//临时变量定义
	int iTmp;
	GUIEVENT_ITEM *pEventItem = NULL;
	GUIEVENT *pEventObj = NULL;
	
	unsigned iSync = 0;
	unsigned uiEventCode = GUIEVENT_MOV_CURSOR;
	static unsigned uiEventValue = 0;
	long long lCurtime = 0;
	static long long lLasttime = 0;
	
	static unsigned int uiCurPosition[2][MT_MAX];	//记录按下时x,y坐标
	static unsigned int uiCurId = 0;
			
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:input_event
	  struct input_event event;
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif    //GUI_OS_ENV == LINUX_OS
	
	//加锁
	//请勿随意修改本段代码
	MutexLock(&(pTouchObj->Mutex));

	if (!iErr)
	{
		//尝试为触摸屏事件项分配内存
		//请勿随意修改本段代码
		pEventItem = (GUIEVENT_ITEM *) malloc(sizeof(GUIEVENT_ITEM));
		if (NULL == pEventItem)
		{
			iErr = -2;
		}
	}

#if (GUI_OS_ENV == LINUX_OS)  //OS相关:event , read
	if (!iErr)
	{
		//生成GUIEVENT_ITEM
		//此段根据实际情况完成相应代码
		while ((!iSync) && (!iErr))
		{
			ThreadTestcancel();
			iTmp = read(pTouchObj->iDevFd, &event, sizeof(event));
			ThreadTestcancel();
			if (iTmp != sizeof(event))
			{
				MsecSleep(50);
				continue;
			}

			switch (event.type)
			{
			case EV_KEY:
				if (BTN_TOUCH == event.code)
				{
					uiEventCode = event.value ? 
						GUIEVENT_ABS_DOWN : GUIEVENT_ABS_UP;
				}
				else
				{
					iErr = -3;
				}
				break;
			case EV_REL:
				switch (event.code)
				{
				case REL_X: //实际按ABS_X处理
					pTouchObj->uiCursX = (unsigned int)event.value;
					break;
				case REL_Y: //实际按ABS_Y处理
					pTouchObj->uiCursY = (unsigned int)event.value;
					break;
				default:
					iErr = -3;
					break;
				}
				break;
			case EV_ABS:
				switch (event.code)
				{
				case ABS_MT_POSITION_X:
					pTouchObj->uiCursX = (unsigned int)event.value;
					uiCurPosition[x][uiCurId] = (unsigned int)event.value;
					break;
				case ABS_MT_POSITION_Y:
					pTouchObj->uiCursY = (unsigned int)event.value;
					uiCurPosition[y][uiCurId] = (unsigned int)event.value;
					break;
				}
				break;
			case EV_SYN:
				switch (event.code)
				{
					case SYN_MT_REPORT:
						if(uiCurId > 0)
						{
							uiEventCode = GUIEVENT_MT;
						}
						uiCurId++;
						break;
					case SYN_REPORT:
						{
							uiCurId = 0;
							//获取同步时间
							lCurtime = event.time.tv_sec * 1000000 +  event.time.tv_usec;
							switch (uiEventCode)
							{
							case GUIEVENT_MOV_CURSOR:
								//判断与上次事件的时间差 单位为us
								if ((lCurtime - lLasttime) > TOUCH_MOVE_INTERVAL)
								{
								    OtdrScathCoorAdjust(&pTouchObj->uiCursX, &pTouchObj->uiCursY);
									iSync = 1;
								}
								break;
							case GUIEVENT_ABS_DOWN:
								//保存按下时刻的坐标值
								uiEventValue = (pTouchObj->uiCursX << 16) |
											   (pTouchObj->uiCursY);
								iSync = 1;
								break;
							case GUIEVENT_ABS_UP:
								//恢复按下时刻的坐标值
								pTouchObj->uiCursX = (uiEventValue >> 16) & 0x0000FFFF;
								pTouchObj->uiCursY = uiEventValue & 0x0000FFFF;
								uiEventValue = -1;
								iSync = 1;
								break;
							case GUIEVENT_MT:
								if ((lCurtime - lLasttime) > TOUCH_MOVE_INTERVAL)
								{
									int iCurXLen = uiCurPosition[x][0] - uiCurPosition[x][1];
									int iCurYLen = uiCurPosition[y][0] - uiCurPosition[y][1];
									UINT32 uiTmpCenX = uiCurPosition[x][1] + (iCurXLen/2);
									UINT32 uiTmpCenY = uiCurPosition[y][1] + (iCurYLen/2);
									uiMTCenter = (uiTmpCenX << 16) | (uiTmpCenY);
									
									UINT32 iChgX = abs(iCurXLen);
									UINT32 iChgY = abs(iCurYLen);
									pTouchObj->uiCursX = iChgX;
									pTouchObj->uiCursY = iChgY;
									iSync = 1;
								}
								break;
							default:
								iErr = -3;
								break;
							}
							break;
						}
						break;
					default:
						break;
				}
				default:
					break;
			}
		}
	}
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif    //GUI_OS_ENV == LINUX_OS

	if (!iErr)
	{
		//生成事件项并保存本次事件的时间戳
		lLasttime = lCurtime;
		pEventItem->uiEventType = GUIEVENT_TYP_IN;
		pEventItem->uiEventCode = uiEventCode;
		pEventItem->uiEventValue = (pTouchObj->uiCursX << 16) |
								   (pTouchObj->uiCursY);

		GucKeyDownFlg = 1;
		if(GucScreenOffFlg)
		{
			GucExecFlg =0;
			GucScreenOffFlg = 0;	
			GucPowerOffFlg = 0;
			for(iTmp = 0; iTmp < 10; iTmp++)
			{
				BrightnessSet(iTmp);
			}
			SetBrightness(pCurSystemSet->uiDCLCDBright, 
						  pCurSystemSet->uiACLCDBright);   
		}	
		/*if (!MatchEventObject(pEventItem))
		{
			iErr = -4;
		}*/
	}

	if (!iErr)
	{
		//事件项写入事件队列
		//请勿随意修改本段代码
		pEventObj = GetCurrEvent();
		if (WriteEventQueue(pEventItem, pEventObj))
		{
			iErr = -4;
		}
	}

	//错误处理
	//此段根据实际情况完成相应代码
	switch (iErr)
	{
	case -4:
	case -3:
		free(pEventItem);
		//no break
	case -2:
	case -1:
	default:
		break;
	}

	//解锁
	//请勿随意修改本段代码
	MutexUnlock(&(pTouchObj->Mutex));

	return iErr;
}

//获得MT的中心位置
UINT32 GetMtCenterPosition()
{
	return uiMTCenter;
}


#if 0
/***
  * 功能：
        自定义的触摸屏处理函数，用于完成触摸屏线程的处理
  * 参数：
        1.GUIKEYPAD *pTouchObj:    处理函数的键盘对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int CustomTouchFunc(GUITOUCH *pTouchObj)
{
    //错误标志、返回值定义
    int iErr = 0;
	int iReturn = 0;
    //临时变量定义
    static int iPrevX = -1, iPrevY = -1; 
    struct input_event Event[5];
    int iLen, iTouchAct, iAbsX, iAbsY, iTmp;
    GUIEVENT_ITEM *pEventItem = NULL;
    GUIEVENT *pEventObj = NULL;

	/* start add by  2014.10.17 */
	struct timeval NowTime;
	static unsigned int uiLastTime;		//始终代表触摸屏驱动上一次发送数据的时间，单位为ms
	static unsigned int uiLastClock;	//始终代表上一次响应触摸屏down事件时的时间，单位为ms
	unsigned int uiNowTime;				//始终代表本次获得触摸屏数据时的时间，单位为ms
	unsigned int uiInterval;			//始终代表获取触摸屏驱动相邻两次数据的时间间隔，单位为ms
	/* end add by  2014.10.17 */

    //加锁
    //请勿随意修改本段代码
    MutexLock(&(pTouchObj->Mutex));

    if (iErr == 0)
    {
        //读取触摸屏返回的数据
        ThreadTestcancel();
        iLen = read(pTouchObj->iDevFd, Event, sizeof(Event));
		printf("1----------------------------------------iLen = %d\n", iLen);
        ThreadTestcancel();
        //if (iLen < sizeof(struct input_event))
        if (iLen < 0)
        {
        	printf("3----------------------------------------iLen = %d\n", iLen);
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //初始化触摸动作标志及坐标值标志
        iTouchAct = -1;
        iAbsX = -1;
        iAbsY = -1;
        //从读取的数据中寻找触摸动作及坐标值
        iLen /= sizeof(struct input_event);
		printf("2----------------------------------------iLen = %d\n", iLen);
        for (iTmp = 0; iTmp < iLen; iTmp++)
        {
            //找到触摸动作，修改触摸动作标志
            if (Event[iTmp].type == EV_KEY)         //类型为按键
            {
                if (Event[iTmp].code == BTN_TOUCH)  //编码为触摸动作
                {
                    iTouchAct = Event[iTmp].value;  //得到触摸动作
                }
            }		
			
            //找到坐标值，修改坐标值标志
            if (Event[iTmp].type == EV_REL)         //类型为坐标值
            {
                if (Event[iTmp].code == REL_X)      //编码为横坐标
                {
                    iAbsX = Event[iTmp].value;      //得到横坐标值
                }
                if (Event[iTmp].code == REL_Y)      //编码为纵坐标
                {
                    iAbsY = Event[iTmp].value;      //得到纵坐标值
                }
            }
            //得到点击触摸屏，直接忽略(点击触摸屏将由横纵坐标值来生成)
            if (iTouchAct == 1)
            {
                iTouchAct = -1;                     //重置触摸动作标志
            }
            //得到离开触摸屏或横纵坐标值，生成事件项
            if ((iTouchAct == 0) || 
                (iAbsX >= 0 && iAbsY >= 0))
            {

				#if 1
				/* start add by  2014.10.17 */
				gettimeofday(&NowTime, NULL);
				/* 触摸按键按下 */
				if(iAbsX >= 0 && iAbsY >= 0)
				{
					/* 计算本次按下的时间，单位为ms */
					uiNowTime = NowTime.tv_sec*1000 + NowTime.tv_usec/1000;
					/* 计算相邻两次触摸按键按下的时间间隔 */
					uiInterval = uiNowTime-uiLastTime;
					//printf("uiInterval = %d\n", uiInterval);
					/* 两次按键时间间隔很小，可以认为发生长按事件 */
					if(uiInterval < INTERVAL)
					{
						MutexUnlock(&(pTouchObj->Mutex));
						return 0;
					}
					/* 对于某一个按钮区域，发生第一次down事件 */
					else
					{
						uiLastTime = uiNowTime;
						uiLastClock = uiNowTime;
					}
				}
				/* end add by  2014.10.17 */
				#endif

				//尝试为触摸屏事件项分配内存
                pEventItem = (GUIEVENT_ITEM *) GuiMemAlloc(sizeof(GUIEVENT_ITEM));
                if (NULL == pEventItem)
                {
                    iErr = -2;
                    break;
                }			
			
                //生成GUIEVENT_ITEM
                pEventItem->uiEventType = GUIEVENT_TYP_IN;
                if (iTouchAct == 0)                 //离开触摸屏
                {
                    pEventItem->uiEventCode = GUIEVENT_ABS_UP;
                    pEventItem->uiEventValue = (iPrevX << 16) | iPrevY;
                    iTouchAct = -1;                 //重置触摸动作标志
                }
                if (iAbsX >= 0 && iAbsY >= 0)       //横纵坐标值
                {
                    pEventItem->uiEventCode = GUIEVENT_ABS_DOWN;
                    pEventItem->uiEventValue = (iAbsX << 16) | iAbsY;
                    iPrevX = iAbsX;                 //保存横坐标
                    iPrevY = iAbsY;                 //保存纵坐标
                    iAbsX = -1;                     //重置横坐标标志
                    iAbsY = -1;                     //重置纵坐标标志
                }		
				
                //事件项写入事件队列
	           	pEventObj = GetCurrEvent();
  				//MutexLock(&(pEventObj->Mutex));
                if (WriteEventQueue(pEventItem, pEventObj))
                {
                    iErr = -3;
                    break;
                }
                //MutexUnlock(&(pEventObj->Mutex));
            }
        }
    }

//待机和自动关机需要
	int i = 0;
	GucKeyDownFlg = 1;			//有触摸动作
	if(GucScreenOffFlg)			//屏幕已自动关闭	
	{
		GucExecFlg =0;
		GucScreenOffFlg = 0;	
		GucPowerOffFlg = 0;
	
		for(i=0; i<10; i++)
		{
			BrightnessSet(i);
		}
		/* 屏幕亮度恢复 */
        SetBrightness(pCurSystemSet->uiDCLCDBright, pCurSystemSet->uiACLCDBright);   
	}
//
		
    //错误处理
    switch (iErr)
    {
	    case -3:
	        MutexUnlock(&(pEventObj->Mutex));
	        GuiMemFree(pEventItem);
	    case -2:
	    case -1:
	    default:
	        break;
    }

    //解锁
    //请勿随意修改本段代码
    MutexUnlock(&(pTouchObj->Mutex));

    return iErr;
}
#endif

void RefreshScreen(char *file, const char *func, int line)
{
	GUIFBMAP *pFbmapObj;
	
	pFbmapObj = GetCurrFbmap();
	//CODER_LOG(CoderGu, "refresh screen %s|%s|%d\n", file, func, line);

    MutexLock(&(pFbmapObj->Mutex));
	
	memcpy(pFbmapObj->pFbEntry, pFbmapObj->pMapBuff, pFbmapObj->uiBufSize);
	
    MutexUnlock(&(pFbmapObj->Mutex));
}

/***
  * 功能：
        获取设备节点的名称字符串
  * 参数：
    1、char* eventName：事件名称，作为传出参数，内存在外部申请
    2、char *devName：设备名称字符串：如：key、touch，此处暂时使用F216_KEY
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
void GetDeviceEventName(char* eventName, char *devName)
{
    int fd;
    int i = 0;
    char buf[128] = {0};

    for (i = 0; i < EVENT_NUM; i++)
    {
        sprintf(eventName, "/dev/event%d", i);
        if ((fd = open(eventName, O_RDONLY, 0)) >= 0)
        {
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            if (strstr(buf, devName))
            {
                close(fd);
                break;
            }
            close(fd);
        }
    }
}