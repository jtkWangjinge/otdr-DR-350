/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsolaconfig.c
* 摘    要：  实现初始化窗体frmsola的窗体处理线程及相关操作函数
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2016-10-28
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/

#include "wnd_frmsolaconfig.h"

int calcEventType(const EVENTS_INFO* pEvent, int eventIndex)
{
     //计算事件类型
    int type = 0;
    //第0个事件为起始事件
    if (eventIndex == 0) 
    {
        type = 0; //TODO:事件类型还未完成
    }
    else 
    {
        if(pEvent->iStyle >= 21)  //光用率不足
        {
            type = 9;
        } 
        else if(pEvent->iStyle == 20) //连续光纤事件
        {
            type = 10;
        }
        //else if(pEvent->iStyle == 15) //end分路器事件
        //{
        //    type = 7;
        //}
        else if(pEvent->iStyle >= 10)  //末端事件
        {
            type = 4;
        }
        else if(pEvent->iStyle == 5) //宏弯曲事件
        {
            type = 5;
        }
        //else if(pEvent->iStyle == 6) //start分路器事件
        //{
        //    type = 6;
        //}
        else if(pEvent->iStyle == 3) //回波事件
        {
            type = 3;
        }
        else if(pEvent->iStyle == 2) //反射事件
        {
            type = 3;
        }
        else if (pEvent->iStyle == 1) //增益事件
        {
            type = 2;
        }
        else if(pEvent->iStyle == 0)  //衰减事件
        {
            type = 1;
        }
        else                          //unknow事件
        {
            type = 11;
        }            
    }

    return type;
}

