/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guitouch.c
* 摘    要：  GUIFBMAP模块主要完成设备抽象层定义及实现，以便GUI能够适用于不同硬
*             件平台对应用层隔离硬件差异。guitouch.c实现了guitouch.h中声明的触
*             摸屏设备的相关操作，用于实现GUI的触摸屏输入设备抽象。
*
* 当前版本：  v1.0.1 (考虑到跨平台，新增了对于GUI_OS_ENV的预编译检验)
* 作    者：  wjg
* 完成日期：  2012-10-12
*
* 取代版本：  v1.0.0
* 原 作 者：  yexin.zhu
* 完成日期：  2012-8-2
*******************************************************************************/

#include "guitouch.h"


/*************************************
* 为实现GUITOUCH而需要引用的其他头文件
*************************************/
#include "guischeduler.h"
#include "guievent.h"


/**************************
* 定义GUI中的当前触摸屏对象
**************************/
static GUITOUCH *pCurrTouch = NULL;


/***
  * 功能：
        依据指定的信息创建触摸屏对象
  * 参数：
        1.char *strDevName:         需要创建触摸屏对象的设备的名称
        2.int iFbType:              触摸屏缓冲设备类型，1标准、2自定义
        3.THREADFUNC fnTouchThread: 回调函数，用于创建帧缓冲的线程
  * 返回：
        成功返回有效指针，失败返回NULL
  * 备注：
***/
GUITOUCH* CreateTouch(char *strDevName, int iDevType, 
                      THREADFUNC fnTouchThread)
{
    //错误标志、返回值定义
    int iErr = 0;
    GUITOUCH *pTouchObj = NULL;

    if (iErr == 0)
    {
        //判断strDevName, fnTouchThread是否为有效指针
        if (NULL == strDevName || NULL == fnTouchThread)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //申请内存资源
        pTouchObj = (GUITOUCH *) malloc(sizeof(GUITOUCH));
        if (NULL == pTouchObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:open()
    if (iErr == 0)
    {
        //尝试打开设备文件
        pTouchObj->iDevFd = open(strDevName, O_RDWR);
        if (pTouchObj->iDevFd == -1)
        {
            iErr = -3;
        }
        //设置新建触摸屏对象的iDevType
        pTouchObj->iDevType = iDevType;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    if (iErr == 0)
    {
        //设置新建触摸屏对象的fnTouchThread
        pTouchObj->fnTouchThread = fnTouchThread;
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        free(pTouchObj);
    case -2:
    case -1:
        pTouchObj = NULL;
    default:
        break;
    }

    return pTouchObj;
}


/***
  * 功能：
        删除触摸屏对象
  * 参数：
        1.GUITOUCH **ppTouchObj:    指针的指针，指向需要销毁的触摸屏对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
        如果成功，传入的指针将被置空
***/
int DestroyTouch(GUITOUCH **ppTouchObj)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //判断ppTouchObj是否为有效指针
        if (NULL == ppTouchObj)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断ppTouchObj所指向的是否为有效指针
        if (NULL == *ppTouchObj)
        {
            iErr = -2;
        }
    }

  #if (GUI_OS_ENV == LINUX_OS)  //OS相关:close()
    if (iErr == 0)
    {
        //释放资源
        if (close((*ppTouchObj)->iDevFd))   //设备文件为共享资源，需验证返回值
        {
            iErr = -3;
        }
        //销毁触摸屏对象，并将指针置空
        free(*ppTouchObj);
        *ppTouchObj = NULL;
    }
  #else
  #error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
  #endif

    return iErr;
}


/***
  * 功能：
        获取当前触摸屏对象
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
GUITOUCH* GetCurrTouch(void)
{
    return pCurrTouch;
}


/***
  * 功能：
        设置当前触摸屏对象
  * 参数：
        1.GUITOUCH *pTouchObj:  被设置为当前触摸屏设备对象
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int SetCurrTouch(GUITOUCH *pTouchObj)
{
    //判断pTouchObj是否有效
    if (NULL == pTouchObj)
    {
        return -1;
    }

    pCurrTouch = pTouchObj;

    return 0;
}


/***
  * 功能：
        默认触摸屏线程入口函数
  * 参数：
        1.void *pThreadArg:     线程入口函数参数，类型为(GUITOUCH *)
  * 返回：
        返回线程结束状态
  * 备注：
***/
void* DefaultTouchThread(void *pThreadArg)
{
    //错误标志、返回值定义
    static int iReturn = 0;
    //临时变量定义
    int iExit = 0;
    GUITOUCH *pTouchObj = pThreadArg;

    while (1)
    {
        //根据结束标志判断是否需要结束触摸屏线程
        iExit = GetExitFlag();
        if (iExit == 1)
        {
            break;
        }

        //触摸屏事件的封装
        CustomTouchFunc(pTouchObj);
        //睡眠50毫秒
        MsecSleep(50);
    }

    //退出线程
    ThreadExit(&iReturn);
    return &iReturn;    //该语句只是为了消除编译器报警，实际上不起作用
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
    //临时变量定义
    static int iPrevX = -1, iPrevY = -1; 
    struct input_event Event[5];
    int iLen, iTouchAct, iAbsX, iAbsY, iTmp;
    GUIEVENT_ITEM *pEventItem = NULL;
    GUIEVENT *pEventObj = NULL;

    if (iErr == 0)
    {
        //读取触摸屏返回的数据
        iLen = read(pTouchObj->iDevFd, Event, sizeof(Event));
        if (iLen < sizeof(struct input_event))
        {
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
                //尝试为触摸屏事件项分配内存
                pEventItem = (GUIEVENT_ITEM *) malloc(sizeof(GUIEVENT_ITEM));
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
                MutexLock(&(pEventObj->Mutex));
                if (WriteEventQueue(pEventItem, pEventObj))
                {
                    iErr = -3;
                    break;
                }
                MutexUnlock(&(pEventObj->Mutex));
            }
        }
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        MutexUnlock(&(pEventObj->Mutex));
        free(pEventItem);
    case -2:
    case -1:
    default:
        break;
    }

    return iErr;
}
#endif


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
	unsigned uiEventCode = 0;
	static unsigned uiEventValue = 0;
	long lCurtime = 0;
	static long lLasttime = 0;					
	UINT32 uiCursX = 0;
	UINT32 uiCursY = 0;

	//检查参数
	if (!iErr)
	{
		if (!pTouchObj)
		{
			iErr = -1;
		}
	}
			
#if (GUI_OS_ENV == LINUX_OS)  //OS相关:input_event
	  struct input_event event;
#else
#error "Unknown GUI_OS_ENV, not supported current GUI_OS_ENV!"
#endif    //GUI_OS_ENV == LINUX_OS
	
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
			pthread_testcancel();
			iTmp = read(pTouchObj->iDevFd, &event, sizeof(event));
			pthread_testcancel();
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
					uiCursX = (unsigned int)event.value;
					break;
				case REL_Y: //实际按ABS_Y处理
					uiCursY = (unsigned int)event.value;
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
					uiCursX = (unsigned int)event.value;
					break;
				case ABS_MT_POSITION_Y:
					uiCursY = (unsigned int)event.value;
					break;
				}
				break;
			case EV_SYN:
				//获取同步时间
				switch (uiEventCode)
				{
				case GUIEVENT_ABS_DOWN:
					//保存按下时刻的坐标值
					uiEventValue = (uiCursX << 16) |
								   (uiCursY);
					iSync = 1;
					break;
				case GUIEVENT_ABS_UP:
					//恢复按下时刻的坐标值
					uiCursX = (uiEventValue >> 16) & 0x0000FFFF;
					uiCursY = uiEventValue & 0x0000FFFF;
					iSync = 1;
					break;
				default:
					iErr = -3;
					break;
				}
				break;
			default:
				iErr = -3;
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
		pEventItem->uiEventValue = (uiCursX << 16) |
								   (uiCursY);

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

	return iErr;
}

