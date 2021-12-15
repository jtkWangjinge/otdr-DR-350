/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmtimeset.c  
* 摘    要：  实现用于完成时间设置模块功能的应用程序接口
*
* 当前版本：  v1.0.0 
* 作    者：wjg
* 完成日期：2020/8/31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "app_frmtimeset.h"

#include "app_global.h"

#include <stdio.h>

/**********************************************************************************
**								内部函数声明				 						 **
***********************************************************************************/

/***
  * 功能：
        日期增加处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static void SetAddDate(RTCVAL* pRtc, int iSelected);

/***
  * 功能：
        日期减少处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        无
  * 备注：
***/ 
static void SetReduceDate(RTCVAL* pRtc, int iSelected);

/***
  * 功能：
     	年份增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddYear(RTCVAL* pRtc);
/***
  * 功能：
     	年份减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceYear(RTCVAL* pRtc);

/***
  * 功能：
     	月份增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddMonth(RTCVAL* pRtc);

/***
  * 功能：
     	月份减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceMonth(RTCVAL* pRtc);

/***
  * 功能：
     	天数增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddDay(RTCVAL* pRtc);

/***
  * 功能：
     	天数减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceDay(RTCVAL* pRtc);

/***
  * 功能：
        时间增加处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static void SetAddTime(RTCVAL* pRtc, int iSelected);

/***
  * 功能：
        时间减少处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        无
  * 备注：
***/ 
static void SetReduceTime(RTCVAL* pRtc, int iSelected);

/***
  * 功能：
     	小时增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddHour(RTCVAL* pRtc);

/***
  * 功能：
     	小时减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceHour(RTCVAL* pRtc);

/***
  * 功能：
     	分钟增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddMinute(RTCVAL* pRtc);

/***
  * 功能：
     	分钟减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceMinute(RTCVAL* pRtc);

/***
  * 功能：
     	秒数增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddSecond(RTCVAL* pRtc);

/***
  * 功能：
     	秒数减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceSecond(RTCVAL* pRtc);

/**********************************************************************************
**								外部函数实现				 						 **
***********************************************************************************/

/***
  * 功能：
     	获取当前Rtc时间
  * 参数：
  		无
  * 返回：
        成功返rtc_time结构指针，失败返回NULL
  * 备注：
***/ 
RTCVAL* GetCurTime(void)
{
	DEVFD *pDevFd = NULL;
	RTCVAL *pRtc = NULL;
	
	pDevFd = GetGlb_DevFd();
	pRtc = Rtc_GetTime(pDevFd->iRtcDev);
	if (NULL == pRtc)
	{
		LOG(LOG_ERROR, "Get Time Failed\n");
		return pRtc;
	}
	pRtc->tm_year += 1900;
	pRtc->tm_mon++;

	return pRtc;
}

/***
  * 功能：
     	设置当前Rtc时间
  * 参数：
  		无
  * 返回：
        成功返 0，失败返回非 0
  * 备注：
***/ 
int SetCurTime(RTCVAL* pRtc)
{
	int iRet = 0;
	DEVFD *pDevFd = NULL;

	pDevFd = GetGlb_DevFd();

	//月份需要-1(0~11) 
	pRtc->tm_mon--;
	iRet = Rtc_SetTime(pDevFd->iRtcDev, pRtc);
	CODER_LOG(CoderYun, "Set RtcTime Returned %d\n", iRet);
	//设置完成月份恢复 
	pRtc->tm_mon++;

	return iRet;
}

/***
  * 功能：
     	显示当前的日期
  * 参数：
  		无
  * 返回：
        成功返回日期的字符串，失败返回NULL
  * 备注：函数调用后需要释放内存
***/ 
GUICHAR* DisplayCurDate(void)
{
    RTCVAL *pRtc = GetCurTime();
    char tempDate[20] = {0};
    GUICHAR* date = NULL;
    sprintf(tempDate, "%04d-%02d-%02d", pRtc->tm_year, pRtc->tm_mon,pRtc->tm_mday);
    date = TransString(tempDate);
    free(pRtc);
    pRtc = NULL;
    
    return date;
}

/***
  * 功能：
     	显示当前的时间
  * 参数：
  		无
  * 返回：
        成功返回日期的字符串，失败返回NULL
  * 备注：函数调用后需要释放内存
***/ 
GUICHAR* DisplayCurTime(void)
{
    RTCVAL *pRtc = GetCurTime();
    char tempDate[20] = {0};
    GUICHAR* time = NULL;
    sprintf(tempDate, "%02d:%02d:%02d", pRtc->tm_hour, pRtc->tm_min,pRtc->tm_sec);
    time = TransString(tempDate);
    free(pRtc);
    pRtc = NULL;
    
    return time;
}

/***
  * 功能：
        修改日期处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int addOrReduce：增加或减少日期，0:增加，1：减少
        3.int iSelected：选择年月日
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
void ModifyDate(RTCVAL* pRtc, int addOrReduce, int iSelected)
{
    if(addOrReduce == 0)//add date
    {
        SetAddDate(pRtc, iSelected);
    }
    else//reduce date
    {
        SetReduceDate(pRtc, iSelected);
    }
}

/***
  * 功能：
        修改时间处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int addOrReduce：增加或减少时间,0:增加，1：减少
        3.int iSelected：选择时分秒
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
void ModifyTime(RTCVAL* pRtc, int addOrReduce, int iSelected)
{
    if(addOrReduce == 0)//add date
    {
        SetAddTime(pRtc, iSelected);
    }
    else//reduce date
    {
        SetReduceTime(pRtc, iSelected);
    }
}

/**********************************************************************************
**								内部函数实现				 						 **
***********************************************************************************/
/***
  * 功能：
        日期增加处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static void SetAddDate(RTCVAL* pRtc, int iSelected)
{
    switch(iSelected)
    {
        case T_YEAR:
            AddYear(pRtc);
            break;
        case T_MONTH:
            AddMonth(pRtc);
            break;
        case T_DAY:
            AddDay(pRtc);
            break;
        default:
            break;
    }
}

/***
  * 功能：
        日期减少处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        无
  * 备注：
***/ 
static void SetReduceDate(RTCVAL* pRtc, int iSelected)
{
    switch(iSelected)
    {
        case T_YEAR:
            ReduceYear(pRtc);
            break;
        case T_MONTH:
            ReduceMonth(pRtc);
            break;
        case T_DAY:
            ReduceDay(pRtc);
            break;
        default:
            break;
    }
}


/***
  * 功能：
     	年份增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddYear(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---AddYear----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_year++;
	if (pRtc->tm_year > 2037)
	{
		pRtc->tm_year = 1970;
	}
	
	return 0;
}

/***
  * 功能：
     	年份减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceYear(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---ReduceYear----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_year--;
    
	if (pRtc->tm_year < 1970)
	{
		pRtc->tm_year = 2037;
	}
	
	return 0;
}

/***
  * 功能：
     	月份增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddMonth(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---AddMonth----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_mon++;
    
	if (pRtc->tm_mon > 12)
	{
		pRtc->tm_mon = 1;
	}
	//判断当前设置的月份天数是否符合要求，比如当前是5.31号，
	//那么月份加为6时day不能为显示31，2月也要特殊处理 
	switch(pRtc->tm_mon)
	{
	case 4:
	case 6:
	case 9:
	case 11:
		if (pRtc->tm_mday > 30)
	    {
	        pRtc->tm_mday = 1;
	    }
	    break;
	case 2:    
		if ( (pRtc->tm_year%4 == 0 && pRtc->tm_year%100 != 0) || 
			 (pRtc->tm_year%400 == 0) ) 
	    {
	        if (pRtc->tm_mday > 29)
	        {
	            pRtc->tm_mday = 1;
	        }
	    }
		else
		{
	        if (pRtc->tm_mday > 28)
	        {
	            pRtc->tm_mday = 1;
	        }			
		}
	    break;
		
	default:
		break;
	}
	
	return 0;
}

/***
  * 功能：
     	月份减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceMonth(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---ReduceMonth----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_mon--;
    
	if (pRtc->tm_mon < 1)
	{
		pRtc->tm_mon = 12;
	}
	//判断当前设置的月份天数是否符合要求，比如当前是5.31号，
	//那么月份加为6时day不能为显示31，2月也要特殊处理 
	switch(pRtc->tm_mon)
	{
	case 4:
	case 6:
	case 9:
	case 11:
		if (pRtc->tm_mday > 30)
	    {
	        pRtc->tm_mday = 1;
	    }
	    break;
	case 2:    
		if ( (pRtc->tm_year%4 == 0 && pRtc->tm_year%100 != 0) || 
			 (pRtc->tm_year%400 == 0) ) 
	    {
	        if (pRtc->tm_mday > 29)
	        {
	            pRtc->tm_mday = 1;
	        }
	    }
		else
		{
	        if (pRtc->tm_mday > 28)
	        {
	            pRtc->tm_mday = 1;
	        }			
		}
	    break;
		
	default:
		break;
	}
		
	return 0;
}

/***
  * 功能：
     	天数增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddDay(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---AddDay----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_mday++;
    
	switch (pRtc->tm_mon)
	{
	case 0://此处是为了修复一个超低概率bug其实是没有零月的
	case 1:     
	case 3:     
	case 5:   
	case 7:   
	case 8:
	case 10:
	case 12:
	    if (pRtc->tm_mday > 31)
	    {
	        pRtc->tm_mday = 1;
	    }
	    break;
	case 4:     
	case 6:     
	case 9:
	case 11:   
	    if (pRtc->tm_mday > 30)
	    {
	        pRtc->tm_mday = 1;
	    }
	    break;
	case 2:    
		if ( (pRtc->tm_year%4 == 0 && pRtc->tm_year%100 != 0) || 
			 (pRtc->tm_year%400 == 0) ) 
	    {
	        if (pRtc->tm_mday > 29)
	        {
	            pRtc->tm_mday = 1;
	        }
	    }
		else
		{
	        if (pRtc->tm_mday > 28)
	        {
	            pRtc->tm_mday = 1;
	        }			
		}
	    break;
	default:
	    break;
	}

	return 0;
}

/***
  * 功能：
     	天数减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceDay(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---ReduceDay----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_mday--;
    
	switch (pRtc->tm_mon)
	{
	case 0://此处是为了修复一个超低概率bug其实是没有零月的
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
	    if (pRtc->tm_mday < 1)
	    {
	        pRtc->tm_mday = 31;
	    }
	    break;
	case 4:
	case 6:
	case 9:
	case 11:   
	    if (pRtc->tm_mday < 1)
	    {
	        pRtc->tm_mday = 30;
	    }
	    break;
	case 2:    
		if ( (pRtc->tm_year%4 == 0 && pRtc->tm_year%100 != 0) || 
			 (pRtc->tm_year%400 == 0) ) 
	    {
	        if (pRtc->tm_mday < 1)
	        {
	            pRtc->tm_mday = 29;
	        }
	    }
		else
		{
	        if (pRtc->tm_mday < 1)
	        {
	            pRtc->tm_mday = 28;
	        }			
		}
	    break;
	default:
	    break;
	}
	
	return 0;
}

/***
  * 功能：
        时间增加处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static void SetAddTime(RTCVAL* pRtc, int iSelected)
{
    switch(iSelected)
    {
        case T_HOURS:
            AddHour(pRtc);
            break;
        case T_MINUTE:
            AddMinute(pRtc);
            break;
        case T_SECOND:
            AddSecond(pRtc);
            break;
        default:
            break;
    }
}

/***
  * 功能：
        时间减少处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int iSelected：选择年月日
  * 返回：
        无
  * 备注：
***/ 
static void SetReduceTime(RTCVAL* pRtc, int iSelected)
{
    switch(iSelected)
    {
        case T_HOURS:
            ReduceHour(pRtc);
            break;
        case T_MINUTE:
            ReduceMinute(pRtc);
            break;
        case T_SECOND:
            ReduceSecond(pRtc);
            break;
        default:
            break;
    }
}


/***
  * 功能：
     	小时增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddHour(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---AddHour----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_hour++;
    
	if (pRtc->tm_hour > 23)
	{
		pRtc->tm_hour = 0;
	}
		
	return 0;
}

/***
  * 功能：
     	小时减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceHour(RTCVAL* pRtc)
{	
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---ReduceHour----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_hour--;
    
	if (pRtc->tm_hour < 0)
	{
		pRtc->tm_hour = 23;
	}
	
	return 0;
}

/***
  * 功能：
     	分钟增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddMinute(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---AddMinute----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_min++;
    
	if (pRtc->tm_min > 59)
	{
		pRtc->tm_min = 0;
	}
			
	return 0;
}

/***
  * 功能：
     	分钟减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceMinute(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---ReduceMinute----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_min--;
    
	if (pRtc->tm_min <0)
	{
		pRtc->tm_min = 59;
	}
		
	return 0;
}

/***
  * 功能：
     	秒数增加处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int AddSecond(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---AddSecond----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_sec++;
    
	if (pRtc->tm_sec > 59)
	{
		pRtc->tm_sec = 0;
	}
		
	return 0;
}

/***
  * 功能：
     	秒数减少处理函数
  * 参数：
  		RTCVAL* pRtc:时钟参数
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
static int ReduceSecond(RTCVAL* pRtc)
{
    if(pRtc == NULL)
    {
        LOG(LOG_ERROR, "---AddSecond----pRtc == NULL\n");
        return -1;
    }
    
	pRtc->tm_sec--;
    
	if (pRtc->tm_sec < 0)
	{
		pRtc->tm_sec = 59;
	}
		
	return 0;
}
