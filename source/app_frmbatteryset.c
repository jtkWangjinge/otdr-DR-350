/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmbatteryset.c  
* 摘    要：  实现用于完成电池设置模块功能的应用程序接口
*
* 当前版本：  v1.0.0 
* 作    者：wjg
* 完成日期：2020/8/31
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "app_frmbatteryset.h"

#include "app_global.h"
#include "app_getsetparameter.h"
#include "app_parameter.h"
#include "app_frmbrightset.h"


extern PSYSTEMSET 		pCurSystemSet;
unsigned char GucKeyDownFlg;
unsigned char GucScreenOffFlg;
unsigned char GucPowerOffFlg;
unsigned char GucExecFlg;

static unsigned int GuiScreenTimeCnt = 0; //屏幕自动关闭计时
static unsigned int GuiPowerTimeCnt = 0;  //自动关机计时

static PWRINFO gPowInfo = {0};
static int iLowPowerFlag = 0;           //低电量标志位

struct bat_adc { 
	unsigned char adc_value; 
	unsigned char bat_pecnt; 
}; 

//电池电量
#define DISCHARGE_LOW 0 	//低温禁止充电临界值
#define DISCHARGE_HIGH 55	//高温禁止充电临界值
#define ALARM_LOW -10		//低温报警临界值
#define ALARM_HIGH 70		//高温报警临界值
#define SHUTDOWN_LOW -20	//低温关机临界值
#define SHUTDOWN_HIGH 80	//高温关机临界值
#define TEMPERATURE_ERROR   -99.0f

/***
  * 功能：
     	获得电量的百分比
  * 参数：
  		1.pPwrInfo:从MCU读取的电池信息
  * 返回：
        成功返回电量的百分比，失败返回-1
  * 备注：
***/

unsigned char get_pwr_percnt(PWRINFO *pPwrInfo) 
{ 
#define PWR_ADC_THR 26 

	unsigned char pwr_value = 0; 
	unsigned char pwr_type = pPwrInfo->ucStateType; 
	unsigned char pwr_lvl = 0; 
	const struct bat_adc *pwr = NULL; 

	//建立adc值对应电量百分比数据表(多次测量的经验值) 
	static const struct bat_adc pwr_percnt[] = { 
		{30, 100}, {37,  95}, {42,  90}, {49,  85}, {54, 80}, 
		{59,  75}, {63,  70}, {68,  65}, {72,  60}, {76, 55}, 
		{79,  50}, {82,  45}, {84,  40}, {86,  35}, {90, 30}, 
		{95,  25}, {100, 20}, {105, 15}, {120, 10}, {150, 5}, 
		{255,  0} 
	}; 
	static const struct bat_adc pwr_percnt_old[] = { 
		{3,   3}, {5,     5}, {10, 10}, {20, 20}, {30, 30},
		{40, 40}, {50,   50}, {60, 60}, {70, 70}, {80, 80},
		{90, 90}, {100, 100}, {0,   0} 		 
	}; 
	
	static unsigned char last_pwr_value = 0xff; 

    if(pPwrInfo->ucStateBat == 0)
    {
		last_pwr_value = 0xff;
        return 0;
    }

	if (pwr_type == 0x00)	//933给9g45传%值
	{
		pwr = pwr_percnt_old; 
		pwr_lvl = pPwrInfo->ucPowerLevel; 
	}
	else if (pwr_type == 0x01)	//933给9g45传ad值
	{
		pwr = pwr_percnt; 
		//如果适配器插入，对adc值进行校正，校正阈值为经验值 
		pwr_lvl = pPwrInfo->ucPowerLevel; 
		pwr_lvl += (unsigned char)pPwrInfo->ucStateDc * PWR_ADC_THR; 
	}
	else
	{
		pwr = pwr_percnt; 
	}
	
	//查表寻找相应的电量百分比 
	while (pwr->bat_pecnt != 0) 
	{ 
		if (pwr->adc_value >= pwr_lvl) 
		{ 
			break; 
		}
		++pwr; 
	}

	//适配器已插入
	if ((pPwrInfo->ucStateDc == 1) &&
		(pPwrInfo->ucStateFull == 1))
	{
		last_pwr_value = 90;
	}

	//防止临界状态电量波动 
	if (0xff == last_pwr_value) 
	{ 
		pwr_value = pwr->bat_pecnt; 
	} 
	else if (pPwrInfo->ucStateDc && pPwrInfo->ucStateBat) 
	{ 
		pwr_value = last_pwr_value > pwr->bat_pecnt ? 
		last_pwr_value : pwr->bat_pecnt; 
	} 
	else 
	{ 
		pwr_value = last_pwr_value > pwr->bat_pecnt ? 
		pwr->bat_pecnt : last_pwr_value; 
	} 
	last_pwr_value = pwr_value; 

	return pwr_value; 

#undef PWR_ADC_THR 
}


/***
  * 功能：
     	从MCU中获取电量,保存到全局变量gPowInfo
  * 参数：
  		无
  * 返回：
        成功返0，失败返回-1
  * 备注：
***/ 

int UpdatePwrInfoFromMcu(void)
{
    DEVFD *glbDevFd = NULL;
    int iPowerFd = -1;
    PWRINFO *pPwrInfo = NULL;

    glbDevFd = GetGlb_DevFd();
    iPowerFd = glbDevFd->iPowerDev;
    pPwrInfo = Pwr_GetInfo(iPowerFd);
    
    if (NULL == pPwrInfo)
    {
        LOG(LOG_ERROR, "NULL = pPwrInfo\n");
        return -1;
    }
    else
    {
        memcpy(&gPowInfo, pPwrInfo, sizeof(PWRINFO));
    }
    
    free(pPwrInfo);
    pPwrInfo = NULL;
    
    return 0;
}

/***
  * 功能：
     	获取电量等级
  * 参数：
  		无
  * 返回：
        成功返电量等级（1~5），失败返回-1
  * 备注：
***/ 
int CalcPowerLevel(void)
{
    //错误标志、返回值定义
    int iReturn = 0;
    int iPowerLevel = -1;               //表示电池电量分为4个级别，1-4
                                        //0表示没有电池
    PWRINFO *pPwrInfo = NULL;

    //得到电池信息
    if (!iReturn)
    {
        iReturn = UpdatePwrInfoFromMcu();
        if(0 != iReturn)
        {
            return -1;
        }
        else
        {
            pPwrInfo = &gPowInfo;
        }
    }

	if (!iReturn)
	{	
		if (pPwrInfo->ucStateBat == 0)	//未连接电池
		{
			iPowerLevel = 0;
		}
		else
		{
			unsigned int iPowerValue;

			iPowerValue =  pPwrInfo->ucPowerLevel;//get_pwr_percnt(pPwrInfo);

			if (iPowerValue <= 5)
			{
				iPowerLevel = 1;
                iLowPowerFlag= 1;
                
			}
			else if (iPowerValue < 20)
			{
				iPowerLevel = 2;
                iLowPowerFlag= 0;
			}
			else if (iPowerValue <= 50)
			{
				iPowerLevel = 3;
                iLowPowerFlag= 0;
			}	
			else if (iPowerValue < 90)
			{
				iPowerLevel = 4;
                iLowPowerFlag= 0;
			}		
			else if (iPowerValue <= 100)
			{	
				iPowerLevel = 5;
                iLowPowerFlag= 0;
			}	
		}
	}

	return iPowerLevel;
}

/***
  * 功能：
     	检测机器是否安装电池或适配器
  * 参数：
  		1.power_type：枚举类型，电池或适配器
  * 返回：
        成功返回电池或适配器插入状态，失败返回-1
  * 备注：
***/ 

int CheckPowerOn(POWER_TYPE powerType)
{
    if(powerType >= POWER_COUNT || powerType < 0)
    {
        return -1;
    }
    
    //检查参数
    PWRINFO* pPwrInfo = &gPowInfo;
    if (!pPwrInfo)
    {
        LOG(LOG_ERROR, "gPowInfo IS NULL!\n");
        return -1;
    }

    int ucState[POWER_COUNT] = {pPwrInfo->ucStateBat, pPwrInfo->ucStateDc};
    return ucState[powerType];
}

/***
  * 功能：
     	判断电池电量是否超过50%,1:超过50%, 0:小于50%
  * 参数：
  		1.power_type：枚举类型，电池或适配器
  * 返回：
        成功返回电池或适配器插入状态，失败返回-1
  * 备注：
***/ 

int CheckBatteryValueOverHalf(void)
{
    int iFlag = 0;
    //设备描述符
    int iPowerValue = 0;
#ifdef POWER_DEVICE
    //检查参数
    PWRINFO* pPwrInfo = &gPowInfo;
    iPowerValue = pPwrInfo->ucPowerLevel;//get_pwr_percnt(&gPowInfo);
#endif    
    if(iPowerValue > 50)
    {
        iFlag = 1;
    }
    
    return iFlag;
}

/***
  * 功能：
     	获取低电量标志位
  * 参数：
  		无
  * 返回：
        低电量的标志位
  * 备注：
***/

int GetLowPowerFlag(void)
{
    return iLowPowerFlag;
}

/***
  * 功能：
     	检测高温状态下电池禁止充电的状态
  * 参数：
  		iHaveAdaptor:返回适配器禁止充电的标志
  * 返回：
        返回高温下电池禁止充电的警告标志（鸣笛或关机）,异常情况返回-1
  * 备注：
***/

HIGH_TEMPERATURE_STATE CheckTemperatureOverCharge(int* iHaveAdaptor)
{
    DEVFD *pDevFd = NULL;
    int iOpmFd = -1;
    static int Charge_enable = 1;                   //记录当前是否可以充电 
	float fTemperature = TEMPERATURE_ERROR;			//机器温度
	
	static struct timeval tv_last;
	struct timeval tv_now;
	
    pDevFd = GetGlb_DevFd();
    iOpmFd = pDevFd->iOpticDev;

    GetOpmTemperature(iOpmFd, &fTemperature);//得到温度
    
    if(TEMPERATURE_ERROR == fTemperature)
    {
        LOG(LOG_ERROR, "Read Temperature Error!\n");
        return -1;
    }
    else
    {
        if(fTemperature >= DISCHARGE_HIGH || fTemperature <= DISCHARGE_LOW)//温度超正常工作范围0~49，禁止充电和显示禁止充电标志
        {           
            *iHaveAdaptor = 2;
            if(Charge_enable)
            {
                Charge_enable = 0;
                Pwr_Charge(Charge_enable);
            }
        }
        else if((fTemperature <= (DISCHARGE_HIGH-2)) && (fTemperature >= (DISCHARGE_LOW+2)))//温度防抖
        {
            if(!Charge_enable)
            {
                Charge_enable = 1;
                Pwr_Charge(Charge_enable);
            }
        }

        gettimeofday(&tv_now, NULL);
        if(tv_now.tv_sec - tv_last.tv_sec >= 1)
        {
            tv_last = tv_now;
            if(fTemperature>=ALARM_HIGH || fTemperature<=ALARM_LOW)//温度超正常工作范围-10~60，发出报警声
            {
                return SPEAKER_ALARM;
            }
        }

        if(fTemperature>=SHUTDOWN_HIGH || fTemperature<=SHUTDOWN_LOW)//温度超正常工作范围-20~70，关机
        {
            return TURN_OFF_DEV;
        }
    }

    return NORMAL;
}

/***
  * 功能：
        解决高温关机恢复至室温后检测不到适配器问题，
        设备上电后根据实际温度来禁止/使能适配器
  * 参数：无
  * 返回：无
  * 备注：
***/
void PowerOnChargeStatusHandle(void)
{
    DEVFD *pDevFd = NULL;
	int iOpmFd = -1;
    float fTemperature = 0;
    int iChgEnable = 1;

    pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;
	GetOpmTemperature(iOpmFd, &fTemperature);
    
    if((fTemperature >= DISCHARGE_HIGH) || (fTemperature <= DISCHARGE_LOW))
        
    {
        iChgEnable = 0;
    }
    
    Pwr_Charge(iChgEnable);
    
    return;    
}

/***
  * 功能：
        获取系统存储自动关机的时间（单位是s）
  * 参数：
  * 返回：无
  * 备注：
***/
int GetPowerOffTime(void)
{
    int time = 0;
    #ifdef POWER_DEVICE
    time = CheckPowerOn(ADAPTER) ? pCurSystemSet->uiACPowerOffValue
                                  : pCurSystemSet->uiDCPowerOffValue;
    #endif
    return time;
}

/***
  * 功能：
        设置自动关机的时间
  * 参数：
        int index：单位是分钟，存储的单位是秒，如果数值为0，则一直开着，不关机
  * 返回：无
  * 备注：
***/
void SetPowerOffTime(int index)
{
    if(CheckPowerOn(ADAPTER))
    {
        pCurSystemSet->uiACPowerOffValue = 60 * index;
        
    }
    else
    {
        pCurSystemSet->uiDCPowerOffValue = 60 * index;
    }
}

/***
  * 功能：
        获取系统存储自动关机的时间索引
  * 参数：
  * 返回：正确索引值（0~3），错误返回-1
  * 备注：
***/
int GetPowerOffTimeIndex(int poweroffTime)
{
    int index[4] = {60, 300, 600, 0};
    int i;
    for(i = 0; i < 4; ++i)
    {
        if(poweroffTime == index[i])
        {
            return i;
        }
    }

    return -1;
}

/******************************************************************************
** 函数名  :         PowerSave
** 功  能  :         根据当前使用电源的类型判断是否需要自动关机和自动关闭屏幕
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*******************************************************************************/
int PowerSave(void)
{
    //有检测是否适配器插入
    if (CheckPowerOn(ADAPTER) == 1)
    {
        ACPowerSave();
        if (GucPowerOffFlg)
        {
            TurnOffDEV();
        }
        /* 自动关闭屏幕只执行一次 */
        if (GucScreenOffFlg && !GucExecFlg)
        {
            GucExecFlg = 1;
            BrightnessSet(0); // 自动关闭屏幕
        }
    }
    else //只有电池插入
    {
        DCPowerSave();
        if (GucPowerOffFlg)
        {
            TurnOffDEV();
        }
        /* 自动关闭屏幕只执行一次 */
        if (GucScreenOffFlg && !GucExecFlg)
        {
            GucExecFlg = 1;
            BrightnessSet(0); // 自动关闭屏幕
        }
    }

    return 0;
}

/*******************************************************************************
** 函数名  :         DCPowerSave
** 功  能  :         只有电池的情况下判断是否需要自动关机和自动关闭屏幕
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*******************************************************************************/
void DCPowerSave(void)
{
    if (GucKeyDownFlg) //有按键和触摸动作
    {
        GuiScreenTimeCnt = 0; //计时清零
        GuiPowerTimeCnt = 0;  //计时清零
        GucKeyDownFlg = 0;    //触摸和按键标记清零
    }
    else //没有按键和触摸动作
    {
        //大于0说明要屏幕自动关闭
        if (pCurSystemSet->uiDCScreenOffValue > 0)
        {
            //计时还没有达到屏幕关闭时间
            if (!GucScreenOffFlg)
            {
                GuiScreenTimeCnt++; //继续计时
            }
        }

        //大于0说明要自动关机
        if (pCurSystemSet->uiDCPowerOffValue > 0)
        {
            //计时没有达到自动关机
            if (!GucPowerOffFlg)
            {
                GuiPowerTimeCnt++; //继续计时
            }
        }
    }

    //大于0说明要屏幕自动关闭
    if (pCurSystemSet->uiDCScreenOffValue > 0)
    {
        //判断计时是否到屏幕自动关闭时间
        if (GuiScreenTimeCnt >= pCurSystemSet->uiDCScreenOffValue)
        {
            GuiScreenTimeCnt = 0;
            GucScreenOffFlg = 1;
        }
    }

    //大于0说明要自动关机
    if (pCurSystemSet->uiDCPowerOffValue > 0)
    {
        //判断计时是否到自动关机时间
        if (GuiPowerTimeCnt >= pCurSystemSet->uiDCPowerOffValue)
        {
            GuiPowerTimeCnt = 0;
            GucPowerOffFlg = 1;
        }
    }
}

/*********************************************************************************************************
** 函数名  :         ACPowerSave
** 功  能  :         插入适配器后判断是否需要自动关机和自动关闭屏幕
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*********************************************************************************************************/
void ACPowerSave(void)
{
    //有按键和触摸动作
    if (GucKeyDownFlg)
    {
        GuiScreenTimeCnt = 0; //计时清零
        GuiPowerTimeCnt = 0;  //计时清零
        GucKeyDownFlg = 0;    //触摸和按键标记清零
    }
    else //没有按键和触摸动作
    {
        //大于0说明要屏幕自动关闭
        if (pCurSystemSet->uiACScreenOffValue > 0)
        {
            //计时还没有达到屏幕关闭时间
            if (!GucScreenOffFlg)
            {
                GuiScreenTimeCnt++; //继续计时
            }
        }

        //大于0说明要自动关机
        if (pCurSystemSet->uiACPowerOffValue > 0)
        {
            //计时没有达到自动关机
            if (!GucPowerOffFlg)
            {
                GuiPowerTimeCnt++; //继续计时
            }
        }
    }

    //大于0说明要屏幕自动关闭
    if (pCurSystemSet->uiACScreenOffValue > 0)
    {
        //判断计时是否到屏幕自动关闭时间
        if (GuiScreenTimeCnt >= pCurSystemSet->uiACScreenOffValue)
        {
            GuiScreenTimeCnt = 0;
            GucScreenOffFlg = 1;
            LOG(LOG_ERROR, "Screen auto off\n");
        }
    }

    //大于0说明要自动关机
    if (pCurSystemSet->uiACPowerOffValue > 0)
    {
        //判断计时是否到自动关机时间
        if (GuiPowerTimeCnt >= pCurSystemSet->uiACPowerOffValue)
        {
            GuiPowerTimeCnt = 0;
            GucPowerOffFlg = 1;
            LOG(LOG_ERROR, "Power auto off\n");
        }
    }
}

/******************************************************************************
** 函数名  :         GetLedState
** 功  能  :         根据使用者短按开机键，打开或关闭led灯
** 输入参数:         无
** 输出参数:         无
** 返回值  :         无
** 注      :         无
*******************************************************************************/
int GetLedState(void)
{
    //错误标志、返回值定义
    int iReturn = 0;
    PWRINFO* pPwrInfo = NULL; 
    //得到电池信息
    if (!iReturn)
    {
        iReturn = UpdatePwrInfoFromMcu();
        if (0 != iReturn)
        {
            return -1;
        }
        else
        {
            pPwrInfo = &gPowInfo;
        }
    }

    if (!iReturn)
    {
        //获取led状态位，实际为电源是否被短按
        iReturn = pPwrInfo->ucStateLed;
    }

    return iReturn;
}