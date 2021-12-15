/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmtimeset.h  
* 摘    要：  用于完成时间设置模块功能的应用程序接口声明
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#ifndef _APP_FRM_TIMESET_H_
#define _APP_FRM_TIMESET_H_

#include "app_middle.h"
#include "guibase.h"


//年月日数据索引
#define T_YEAR		(0)
#define T_MONTH		(1)
#define T_DAY		(2)

//时分秒数据索引
#define T_HOURS		(0)
#define T_MINUTE	(1)
#define T_SECOND	(2)


/***
  * 功能：
     	获取当前Rtc时间
  * 参数：
  		无
  * 返回：
        成功返rtc_time结构指针，失败返回NULL
  * 备注：
***/ 

RTCVAL* GetCurTime(void);

/***
  * 功能：
     	设置当前Rtc时间
  * 参数：
  		无
  * 返回：
        成功返 0，失败返回非 0
  * 备注：
***/ 
int SetCurTime(RTCVAL* pRtc);


/***
  * 功能：
     	显示当前的日期
  * 参数：
  		无
  * 返回：
        成功返回日期的字符串，失败返回NULL
  * 备注：函数调用后需要释放内存
***/ 
GUICHAR*  DisplayCurDate(void);

/***
  * 功能：
     	显示当前的时间
  * 参数：
  		无
  * 返回：
        成功返回日期的字符串，失败返回NULL
  * 备注：函数调用后需要释放内存
***/ 
GUICHAR* DisplayCurTime(void);

/***
  * 功能：
        修改日期处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int addOrReduce：增加或减少日期
        3.int iSelected：选择年月日
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
void ModifyDate(RTCVAL* pRtc, int addOrReduce, int iSelected);

/***
  * 功能：
        修改时间处理函数
  * 参数：
        1.RTCVAL* pRtc:时钟参数
        2.int addOrReduce：增加或减少时间
        3.int iSelected：选择时分秒
  * 返回：
        成功返0，失败非0
  * 备注：
***/ 
void ModifyTime(RTCVAL* pRtc, int addOrReduce, int iSelected);

#endif
