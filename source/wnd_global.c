/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_global.c
* 摘    要：  实现各窗体的公共数据、公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者： 
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_global.h"

/***************************************
* 为实现wnd_global而需要引用的其他头文件
***************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/msg.h> 

#include "app_global.h"
#include "app_parameter.h"
#include "app_screenshots.h"
#include "app_installment.h"
#include "app_systemsettings.h"
#include "appwifi.h"
#include "app_queue.h"
#include "app_frmsendfile.h"
#include "app_middle.h"
#include "app_frmbatteryset.h"
#include "app_frmtimeset.h"
#include "app_frmbrightset.h"
#include "app_getsetparameter.h"
#include "app_frmotdrmessage.h"		//测试长按移动标记线

#include "guiphoto.h"

#include "wnd_frmstandbyset.h"
#include "wnd_frmquicksave.h"
#include "wnd_frmwifi.h"
#include "wnd_frmmain.h"
#include "wnd_frmlogo.h"
#include "wnd_messagebox.h"
#include "wnd_frmdialog.h"
#include "wnd_frmvfl.h"


/***************************************
* 全局变量
***************************************/
extern int iPowerDownFlag;                          //机器是否关机的标志位
extern unsigned char GucScreenOffFlg;               //屏幕关闭的标志位
extern unsigned long g_Status;                      //机器状态标志位
extern PSYSTEMSET pCurSystemSet;                    //系统参数结构体
extern CURR_WINDOW_TYPE enCurWindow;                //当前界面的窗体
extern POTDR_TOP_SETTINGS pOtdrTopSettings;         //顶层结构体
extern int WifiSwitch;                              //wifi切换的标志位
extern PFactoryConfig pFactoryConf;					//引用保存到eeprom的变量


unsigned char GucClearFlg = 0;                  //清除的标志位
volatile int iSpeakerOnFlag = 0;				//蜂鸣器打开标志
volatile int iStartSpeaker = 0;					//开始测量蜂鸣器设置

static GUITHREAD thdPublicService;		//公共服务线程ID

GUIFONT *pGlobalFntBlack 	= NULL;  	//黑色字体
GUIFONT *pGlobalFntYellow 	= NULL;  	//黑色字体
GUIFONT *pGlobalFntGray  	= NULL;		//灰色字体
GUIFONT *pGlobalFntGray1  	= NULL;		//灰色字体
GUIFONT *pGlobalFntWhite 	= NULL;		//白色字体
GUIFONT *pGlobalFntRed 		= NULL;		//红色字体
GUIFONT *pGlobalFntGreen 	= NULL;		//绿色字体
GUIFONT *pGlobalFntBlue     = NULL;     //蓝色字体
//大号字体 24*24
GUIFONT *pGlobalBigFntBlack 	= NULL;  	//黑色字体
GUIFONT *pGlobalBigFntYellow 	= NULL;  	//黑色字体
GUIFONT *pGlobalBigFntGray  	= NULL;		//灰色字体
GUIFONT *pGlobalBigFntGray1  	= NULL;		//灰色字体
GUIFONT *pGlobalBigFntWhite 	= NULL;		//白色字体
GUIFONT *pGlobalBigFntRed 		= NULL;		//红色字体
GUIFONT *pGlobalBigFntGreen 	= NULL;		//绿色字体
GUIFONT *pGlobalBigFntBlue      = NULL;     //蓝色字体

unsigned char *pFrameBufferBak = NULL;		//帧缓冲备份区域
static int __g_iCurWin = 0;
static int g_MsgId = 0;
/*******************************************************************************
**						     提示信息框控件定义部分					          **
*******************************************************************************/
static GUIPICTURE *__g_pInfoBtnDisable = NULL;
static GUICHAR *__g_pInfoStrInfo = NULL; 
static GUILABEL *__g_pInfoLblInfo = NULL;
static GUICHAR *pGlobalStrDate = NULL;                  //日期信息
static GUILABEL *pGlobalLblDate = NULL;

static GUIPICTURE *pGlobalBgState = NULL;	            //刷新背景
static GUIPICTURE *pGlobalBgPower = NULL;	            //电池
static GUIPICTURE *pGlobalBgWiFi = NULL;                //wifi
static GUIPICTURE *pGlobalBgUDisk = NULL;               //U盘
static GUIPICTURE *pGlobalBgFiberMicroscope = NULL;  //fip

static GUIMUTEX GStatusMutex;
static int iPowerEnable = 1;
static int iLowPower = 0;
//分期付款计数，12小时判断一次
static int iInstallmentCount = 0;

//显示低电量信息
static void ShowLowBatteryValue(int Wnd, int iPowerlevel);

//光标闪烁记数
static int iTimeCount = 0;	

struct msg_st  
{  
    long int msg_type;  
    char text[1024];  
};

void SpeakerAlarm(void);

//对应 外包提供的excel表规则
static unsigned short CombineCode[8][11] = {
	{0x0E31, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0000, 0x0E5C, 0x0E5D, 0x0E5E, 0x0E5F, 0x0000},
	{0x0E34, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E60, 0x0E61, 0x0E62, 0x0E63, 0x0E64},
	{0x0E35, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0000, 0x0E65, 0x0E66, 0x0E67, 0x0E68, 0x0000},
	{0x0E36, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0000, 0x0E69, 0x0E6A, 0x0E6B, 0x0E6C, 0x0000},
	{0x0E37, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0000, 0x0E6D, 0x0E6E, 0x0E6F, 0x0E70, 0x0000},
    {0x0E4D, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0000, 0x0E71, 0x0E72, 0x0E73, 0x0E74, 0x0000},
    {0x0E10, 0x0E38, 0x0E39, 0x0E3A, 0x0000, 0x0000, 0x0E75, 0x0E76, 0x0E77, 0x0000, 0x0000},
    {0x0E0D, 0x0E38, 0x0E39, 0x0E3A, 0x0000, 0x0000, 0x0E78, 0x0E78, 0x0E7A, 0x0000, 0x0000}};


/***
  * 功能：
        显示功能按钮按下
  * 参数：
        1.GUIPICTURE *pBtnFx:   需要显示的功能按钮图形框
        2.GUILABEL *pLblFx:     需要显示的功能按钮标签
        3.GUILABEL *pLblFx1:    需要显示的功能按钮标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowBtn_FuncDownExtra(GUIPICTURE *pBtnFx, GUILABEL *pLblFx, GUILABEL *pLblFx1)
{
    //错误标志、返回值定义
    int iReturn = 0;

	SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pLblFx);
	SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pLblFx1);
    DisplayLabel(pLblFx);
	DisplayLabel(pLblFx1);	

    return iReturn;
}


/***
  * 功能：
        显示功能按钮弹起
  * 参数：
        1.GUIPICTURE *pBtnFx:   需要显示的功能按钮图形框
        2.GUILABEL *pLblFx:     需要显示的功能按钮标签
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowBtn_FuncUpExtra(GUIPICTURE *pBtnFx, GUILABEL *pLblFx, GUILABEL *pLblFx1)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义

	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pLblFx);
	SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pLblFx1);
    DisplayLabel(pLblFx);
	DisplayLabel(pLblFx1);

    return iReturn;
}

/***
  * 功能：
        实现按钮、Label等控件状态的改变
  * 参数：
        1.char *strBitmapFile:   	需要显示的图片控件路径，可以为NULL
        2.GUIPICTURE *pPicObj:     	需要显示的功能按钮标签，可以为NULL
        3.GUICHAR *pStr:       		需要显示的文字信息，可以为NULL
        3.GUILABEL *pLblObj:       	需要显示的功能按钮标签，可以为NULL
        4.int color:     		   	字体颜色选择1是黑色，2是黄色, 3是灰色，0是系统默认颜色
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int TouchChange(char *strBitmapFile, GUIPICTURE *pPicObj, 
				GUICHAR *pStr, GUILABEL *pLblObj, int color)
{
	int iReturn = 0;
	
	GUIFONT *pFont;	
	
	pFont = GetCurrFont();

	if(NULL == pPicObj)
	{
		CODER_LOG(CoderGu, "pPicObj is NULL\n");
	}
	else
	{
		char cBmpFilePath[256];
		//判断是否带路径
		if (strstr(strBitmapFile, BmpFileDirectory))
			snprintf(cBmpFilePath, sizeof(cBmpFilePath), "%s", strBitmapFile);
		else
			snprintf(cBmpFilePath, sizeof(cBmpFilePath), "%s%s", BmpFileDirectory, strBitmapFile);
	    SetPictureBitmap(cBmpFilePath, pPicObj);
	    DisplayPicture(pPicObj);
	}

	if(NULL != pLblObj)
	{
	    unsigned int uiColor = pFont->uiFgColor;
		SetLabelText(pStr, pLblObj);
		switch(color)
		{
			case 1:
				SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLblObj);
				break;
			case 2:
				SetLabelFont(getGlobalFnt(EN_FONT_YELLOW), pLblObj);
				break;
			case 3:
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY), pLblObj);
				break;
			case 4:
				SetLabelFont(getGlobalFnt(EN_FONT_GRAY1), pLblObj);
				break;
			case 0:
				SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pLblObj);
			default:
				break;
		}
		
	    DisplayLabel(pLblObj);
	    SetFontColor(uiColor, 0xFFFFFFFF, pFont);
	}
	
	return iReturn;
}


/***
  * 功能：
        更新LABEL的文本资源
  * 参数：
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int UpdateLabelRes(GUICHAR *pTargetStr, GUILABEL *pTargetLbl, GUIPICTURE *pBackGround)
{
    //错误标志、返回值定义
	int iReturn = 0;
	GUIPEN *pPen = NULL;
	
	if (iReturn == 0)
	{
		//参数检查
		if ((NULL == pTargetLbl) || (NULL == pTargetStr))
		{
			iReturn = -1;
		}
	}
	
	if (iReturn == 0)
	{
		//背景图片不为NULL，则需要重绘背景
		if (pBackGround != NULL)
		{
			DisplayPicture(pBackGround);
		}
		//背景为NULL，则重绘白色矩形为背景
		else
		{
			GUIRECTANGLE *pRectangle;		
			unsigned int uiColorBack;
			
			pPen = GetCurrPen();
			uiColorBack = pPen->uiPenColor;
            pPen->uiPenColor = 0xFFFFFF;
			pRectangle = &(pTargetLbl->Visible.Area);
			DrawBlock(pRectangle->Start.x, pRectangle->Start.y, pRectangle->End.x, pRectangle->End.y);
			pPen->uiPenColor = uiColorBack;
		}
		
		//检查标签是否可见
		if (pTargetLbl->Visible.iEnable == 0)
		{
			iReturn = -2;
		}
	}
	
	if (iReturn == 0)
	{
		SetLabelText(pTargetStr, pTargetLbl);
		DisplayLabel(pTargetLbl);
	}
	
	return iReturn;
}



//显示低电量信息
static void ShowLowBatteryValue(int Wnd, int iPowerlevel)
{ 
#if 0
    if(Wnd == 1)
    {
       if(iPowerlevel == 1)
       {
           SetPictureBitmap(BmpFileDirectory"bg_global_state1.bmp", pGlobalBgState); 
       }
       else
       {
          SetPictureBitmap(BmpFileDirectory"bg_global_state.bmp", pGlobalBgState);
       }
    }
    else
    {
        if(iPowerlevel == 1)
       {
           SetPictureBitmap(BmpFileDirectory"bg_global_state_main1.bmp", pGlobalBgState); 
       }
       else
       {
          SetPictureBitmap(BmpFileDirectory"bg_global_state_main.bmp", pGlobalBgState);
       }
    }  
#endif
    if(iPowerlevel == 1)
   {
       SetPictureBitmap(BmpFileDirectory"bg_global_state_main1.bmp", pGlobalBgState); 
   }
   else
   {
      SetPictureBitmap(BmpFileDirectory"bg_global_state_main.bmp", pGlobalBgState);
   }
}
/***
  * 功能：
        显示电源信息
  * 参数：
        1.GUIPICTURE *pIcoPower:    需要显示的电源状态图形框
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowSta_PowerInfo(GUIPICTURE *pIcoPower)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //临时变量定义
	int iHaveAdaptor = 1;

	char *pPathPowerLevel[6][3] = {
		//没有电池
		{BmpFileDirectory"ico_bat_disconn.bmp", BmpFileDirectory"ico_bat_disconn.bmp", BmpFileDirectory"ico_bat_disconn.bmp"},
		//电量小于等于5%
		{BmpFileDirectory"ico_power_empty.bmp", BmpFileDirectory"ico_power_dc_empty.bmp", BmpFileDirectory"ico_power_forbid_empty.bmp"}, 
		//电量大于5%
		{BmpFileDirectory"ico_power_quar.bmp", BmpFileDirectory"ico_power_dc_quar.bmp", BmpFileDirectory"ico_power_forbid_quar.bmp"}, 
		//电量大于25%
		{BmpFileDirectory"ico_power_half.bmp", BmpFileDirectory"ico_power_dc_half.bmp", BmpFileDirectory"ico_power_forbid_half.bmp"}, 
		//电量大于50%
		{BmpFileDirectory"ico_power_three_quars.bmp", BmpFileDirectory"ico_power_dc_three_quars.bmp", BmpFileDirectory"ico_power_forbid_three_quars.bmp"}, 
		//电量大于75%
		{BmpFileDirectory"ico_power_full.bmp", BmpFileDirectory"ico_power_dc_full.bmp", BmpFileDirectory"ico_power_forbid_full.bmp"}
		};
	
	char *pPathTarget = NULL; 
    int iPowerLevel = -1;				//表示电池电量分为4个级别，1-4
                                        //0表示没有电池
	MutexLock(&GStatusMutex);
	//参数检查
	if (iReturn == 0)
	{
		if (NULL == pIcoPower)
		{
			iReturn = -1;
		}
	}
		
	if (iReturn == 0)
	{											//0表示没有电池
		iPowerLevel = CalcPowerLevel();  
        
		//检查参数
		if (iPowerLevel < 0)
		{
			LOG(LOG_ERROR, "iPowerLevel < 0\n");
			iReturn = -2;
		}
		else//数值为0~5才为有效值
		{
			//电池信息检测正常才允许如下操作
			iHaveAdaptor = CheckPowerOn(ADAPTER); //是否插入适配器，1表示适配器插入，0表示未插入，2表示不能充电
			HIGH_TEMPERATURE_STATE highTemperatureState = CheckTemperatureOverCharge(&iHaveAdaptor);

			if (highTemperatureState == SPEAKER_ALARM)
			{
				SpeakerAlarm();
			}
			else if (highTemperatureState == TURN_OFF_DEV)
			{
				TurnOffDEV();
			}
			//设置电池图标
			pPathTarget = pPathPowerLevel[iPowerLevel][iHaveAdaptor];
		}
	}

	if (iReturn == 0)
	{
		/* 与上一次状态不同才进行更新 */
		if(strcmp(pPathTarget + 25, pIcoPower->pPicBitmap->strBitmapFile + 25))	//add by  2014.10.15
		{
			SetPictureBitmap(pPathTarget, pIcoPower);
            //处理低电量时修改背景图片
            ShowLowBatteryValue(__g_iCurWin,iPowerLevel);    	    
            if(iPowerEnable < 2)        //全屏界面不刷状态栏
	        {
    			DisplayPicture(pGlobalBgState);
    			DisplayPicture(pGlobalBgPower);
				DisplayLabel(pGlobalLblDate);
    			//DisplayPicture(pGlobalBgWiFi);
    			if(iPowerEnable == 0)
    			{
					DispTransparent(80, 0x0, 480, 0, 160, 40);
				}
    			RefreshScreen(__FILE__, __func__, __LINE__);
            }
		}           
	}

	MutexUnlock(&GStatusMutex);
	
    return iReturn;
}

/***
  * 功能：
        显示WIFI信息(状态栏图标)
  * 参数：
        1.GUIPICTURE *pIcoWifi:    需要显示的WIFI状态图形框
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowSta_WifiInfo(GUIPICTURE *pIcoWifi, int Wnd)
{
    //错误标志、返回值定义
    int iReturn = 0;
	int iConnected = 0;
	char *pPathTarget = NULL;
	static int iCurWnd = 0;				//表示当前所在显示窗口
	static int iWIfiLevel = 0; 			//表示WIFI信号强度分为6个级别: 关闭/0-4
	//只显示当前连接的wifi的信号强度情况
	char *pPathWifiLevel[6][2] = {
							//wifi信号强度水平为0
							{ BmpFileDirectory"ico_wifi_empty_main.bmp", BmpFileDirectory"ico_wifi_empty_other.bmp"}, 
							//wifi信号强度水平为1
							{BmpFileDirectory"ico_wifi_quar_main.bmp", BmpFileDirectory"ico_wifi_quar_other.bmp"}, 
							//wifi信号强度水平为2
							{BmpFileDirectory"ico_wifi_half_main.bmp", BmpFileDirectory"ico_wifi_half_other.bmp"},
							//wifi信号强度水平为3
							{BmpFileDirectory"ico_wifi_three_quars_main.bmp", BmpFileDirectory"ico_wifi_three_quars_other.bmp"},
							//wifi信号强度水平为4
							{BmpFileDirectory"ico_wifi_full_main.bmp", BmpFileDirectory"ico_wifi_full_other.bmp"},
							//wifi关闭
							{BmpFileDirectory"ico_wifi_null_main.bmp", BmpFileDirectory"ico_wifi_null_other.bmp"}
							};

	iCurWnd = Wnd;

	MutexLock(&GStatusMutex);
	//参数检查
	if (iReturn == 0)
	{
		if (NULL == pIcoWifi)
		{
			iReturn = -1;
		}
	}
		
	if (iReturn == 0)
	{
		//检测wifi开关是否开启
		if( 1 == WifiSwitch)
		{
			//从扫描生成的状态文件中获取当前的wifi连接情况
			NETWORK_INFO_SINGLE ConnectNet = getConnectNetInfo();
			if(1 == getDhcpready())
			{
				iConnected = 1;
				iWIfiLevel = calcsignallevel(ConnectNet.signallevel, 5);
			}
			else
			{
				iConnected = 0;
				iWIfiLevel = 5;
			}
		     
		}
		else
		{
			iWIfiLevel = 5;
		}

		pPathTarget = pPathWifiLevel[iWIfiLevel][iCurWnd];
	}

	if (iReturn == 0)
	{
		/* 与上一次状态不同才进行更新 */
		if( strcmp(pPathTarget, pIcoWifi->pPicBitmap->strBitmapFile) )	
		{
    	    SetPictureBitmap(pPathTarget, pIcoWifi);
    	    if(iPowerEnable < 2)        //全屏界面不刷状态栏
    	    {
        	    DisplayPicture(pGlobalBgState);
        	    DisplayPicture(pGlobalBgPower);
				DisplayLabel(pGlobalLblDate);
        	    //DisplayPicture(pGlobalBgWiFi);
        	    if(iPowerEnable == 0)
        	    {
					DispTransparent(80, 0x0, 480, 0, 160, 40);
				}
        	    RefreshScreen(__FILE__, __func__, __LINE__);
            }
		}		
	}
	MutexUnlock(&GStatusMutex);
    return iReturn;
}

//显示u盘状态
int ShowSta_UDiskInfo(int iWinType)
{
    //错误标志、返回值定义
    int iReturn = 0;
    if(iWinType == 1)
    {
        /* 挂载U盘失败 */
        if ( access(MntUsbDirectory, F_OK) == -1 )				
        {
            SetPictureBitmap(BmpFileDirectory"bg_u_disk_other1.bmp", pGlobalBgUDisk);
        }
        /* 挂载U盘成功 */
        else		
        {
            SetPictureBitmap(BmpFileDirectory"bg_u_disk_other.bmp", pGlobalBgUDisk);
        }
    }
    else
    {
        /* 挂载U盘失败 */
        if ( access(MntUsbDirectory, F_OK) == -1 )				
        {
            SetPictureBitmap(BmpFileDirectory"bg_u_disk_main1.bmp", pGlobalBgUDisk);
        }
        /* 挂载U盘成功 */
        else		
        {
            SetPictureBitmap(BmpFileDirectory"bg_u_disk_main.bmp", pGlobalBgUDisk);
        }
    }
    DisplayPicture(pGlobalBgState);
    DisplayPicture(pGlobalBgPower);
	DisplayLabel(pGlobalLblDate);
    //DisplayPicture(pGlobalBgWiFi);
    //DisplayPicture(pGlobalBgUDisk);
    //DisplayPicture(pGlobalBgFiberMicroscope);
    return iReturn;
}

//显示光纤端面插入状态
int ShowSta_FiberMicroscopeInfo(int iWinType)
{
    //错误标志、返回值定义
    int iReturn = 0;
    if(iWinType == 1)
    {
    }
    else
    {
        /* 挂载U盘失败 */
        if ( access("/dev/inno_f252", F_OK) == -1 )				
        {
            SetPictureBitmap(BmpFileDirectory"ico_main_nofibermicroscope.bmp", pGlobalBgFiberMicroscope);
        }
        /* 挂载U盘成功 */
        else		
        {
            SetPictureBitmap(BmpFileDirectory"ico_main_fibermicroscope.bmp", pGlobalBgFiberMicroscope);
        }
    }
    DisplayPicture(pGlobalBgState);
    DisplayPicture(pGlobalBgPower);
	DisplayLabel(pGlobalLblDate);
    //DisplayPicture(pGlobalBgWiFi);
    //DisplayPicture(pGlobalBgUDisk);
    //DisplayPicture(pGlobalBgFiberMicroscope);
    return iReturn;
}

//刷新日期(iLoop：0：不循环，1：循环但只在日期变化的时候刷新)
int ShowSta_DateInfo(int iLoop)
{
    //错误标志、返回值定义
    int iReturn = 0;
    //记录上次年月日
	static int lastYear = 0;
    static int lastMonth = 0;
    static int lastDay = 0;
    int currentYear = 0;
    int currentMonth = 0;
    int currentDay = 0;

	MutexLock(&GStatusMutex);
    RTCVAL *pNewRtcTime = NULL;
	pNewRtcTime = GetCurTime();
	char buff[TEMP_ARRAY_SIZE] = {0};
    memset(buff, 0, TEMP_ARRAY_SIZE);
	//检查参数
	if (!pNewRtcTime)
	{
		LOG(LOG_ERROR, "---pNewRtcTime == NULL-----\n");
		MutexUnlock(&GStatusMutex);
		return -1;
	}
	
    currentYear = pNewRtcTime->tm_year;
    currentMonth = pNewRtcTime->tm_mon;
    currentDay = pNewRtcTime->tm_mday;
    sprintf(buff, "%04d-%02d-%02d", pNewRtcTime->tm_year, pNewRtcTime->tm_mon, pNewRtcTime->tm_mday);
	//高清屏该位置足够显示日期信息，不会与进度条、文件名冲突
	if (iPowerEnable < 2) //OTDR界面
	{
        if (iLoop)  //其他界面
        {
            //只有年月日中任意参数不同就刷新显示
            if ((currentYear != lastYear) || (currentMonth != lastMonth) || (currentDay != lastDay))
            {
                pGlobalStrDate = TransString(buff);
                SetLabelText(pGlobalStrDate, pGlobalLblDate);
                DisplayPicture(pGlobalBgState);
                DisplayPicture(pGlobalBgPower);
                DisplayLabel(pGlobalLblDate);
                lastYear = currentYear;
                lastMonth = currentMonth;
                lastDay = currentDay;
                //半显示时添加透明度显示
                if (iPowerEnable == 0)
                {
                    DispTransparent(80, 0x0, 480, 0, 160, 40);
                }

                RefreshScreen(__FILE__, __func__, __LINE__);
            }
        }
        else //主界面只调用一次
        {
            pGlobalStrDate = TransString(buff);
            SetLabelText(pGlobalStrDate, pGlobalLblDate);
            DisplayPicture(pGlobalBgState);
            DisplayPicture(pGlobalBgPower);
            DisplayLabel(pGlobalLblDate);
        }
    }
	
    MutexUnlock(&GStatusMutex);
    
    return iReturn;
}

/***
  * 功能：
        获取FPGA的序列号
  * 参数：
		unsigned long long *pDNA: 指向存贮序列号的指针
  * 返回：
        0			成功
        -1			失败
  * 备注：add by  2014.7.9
***/
int GetFPGASerialNum(unsigned long long *pDNA)
{
	int iRet = 0;
	DEVFD *pDevFd = NULL;
	int iOpmFd = -1;
	UINT32 reg;
	int i = 100;
	unsigned int uiData;
	unsigned short usData;

	if(NULL == pDNA)
	{
		return -1;
	}

	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;
	*pDNA = 0;

	reg = 0x0001;
	Opm_WriteReg(iOpmFd,FPGA_DNA_ENABLE,&reg);
	MsecSleep(5);
	while(i--)
	{
		Opm_ReadReg(iOpmFd,FPGA_DNA_ENABLE,&reg);
		if(reg == 0)
		{
			break;
		}
	}
	Opm_ReadReg(iOpmFd,FPGA_DNA_DATA1,&uiData);
	usData = (unsigned short)uiData;
	*pDNA |= (unsigned long long)usData<<0;
	MsecSleep(5);
	Opm_ReadReg(iOpmFd,FPGA_DNA_DATA2,&uiData);
	usData = (unsigned short)uiData;
	*pDNA |= (unsigned long long)usData<<16;
	MsecSleep(5);
	Opm_ReadReg(iOpmFd,FPGA_DNA_DATA3,&uiData);
	usData = (unsigned short)uiData;
	*pDNA |= (unsigned long long)usData<<32;
	MsecSleep(5);
	Opm_ReadReg(iOpmFd,FPGA_DNA_DATA4,&uiData);
	usData = (unsigned short)uiData;
	*pDNA |= (unsigned long long)usData<<48;
	MsecSleep(5);


	return iRet;
}

/***
  * 功能：
        实现当前窗体中从(x1, y1)到(x2, y2)区域内的控件的是否可用属性的改变
  * 参数：
        1.int iEnableFlag:    控件是否可用
        2.int iTypeOfGUI:	  要改边的GUI的类型
        3.int x1:			  起始位置的x坐标
        4.int y1:			  起始位置的y坐标
        5.int x2:			  终止位置的x坐标
        6.int y2:			  终止位置的y坐标
  * 返回：
        空
  * 作者:  140903
  * 备注：必须调用两次，释放pSave
***/
void ChangeAbleInArea(int iEnableFlag, int iTypeOfGUI, int x1, int y1, int x2, int y2)
{
	GUIWINDOW *pFrmMain = NULL;
	GUILIST *pWndComps = NULL;

    GUILIST_ITEM *pItem = NULL;
	GUIPICTURE *pBtn = NULL;
    int iPos;
	int iSav = 0;

	static int *pSave = NULL;
	
	pFrmMain = GetCurrWindow();

	pWndComps = pFrmMain->pWndComps;

	if(pSave == NULL)
	{
		pSave = (int *)malloc(pWndComps->iCount * sizeof(int));
	}
	
	list_next_each(pItem, pWndComps, iPos)
	{

		GUIOBJ *pPkt = container_of(pItem, GUIOBJ, List);
		if(pPkt->iType == iTypeOfGUI)
		{
			pBtn = (GUIPICTURE *)pPkt->pObj;
			if((pBtn->Visible.Area.Start.x >= x1) && 
				(pBtn->Visible.Area.Start.y >= y1) &&
				(pBtn->Visible.Area.End.x <= x2) &&
				(pBtn->Visible.Area.End.y <= y2))
			{
				if(iEnableFlag == 0)
				{
					*(pSave + iSav) = pBtn->Visible.iEnable;
					iSav++;
					SetPictureEnable(iEnableFlag, pBtn);
				}
				else
				{
					SetPictureEnable(*(pSave + iSav), pBtn);
					iSav++;
				}
			}
		}
	}
	if((iEnableFlag == 1) && (pSave != NULL))
	{
		GuiMemFree(pSave);
		pSave = NULL;
	}
}

/***
  * 功能：
        设置指定区域内控件与设定的颜色进行透明显示
  * 参数：
        1.unsigned char ucAlpha:	Alpha值越大，原背景色越亮
        2.unsigned long ulColour:	24位颜色代码
        3.unsigned int uiPlaceX:    指定区域的X坐标
        4.unsigned int uiPlaceY:    指定区域的Y坐标
        5.unsigned int uiWidth:  	指定区域的宽度
        6.unsigned int uiHeight: 	指定区域的高度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by  2014.09.01
  		此函数只是将指定区域进行透明色处理，如果要将区域内的控件置为无效，
  		请调用ChangeAbleInArea函数
***/
int DispTransparent(unsigned char ucAlpha, unsigned long ulColour,
					 unsigned int uiPlaceX, unsigned int uiPlaceY, 
                     unsigned int uiWidth, unsigned int uiHeight)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GUIFBMAP *pFbmap = NULL;	//帧缓冲对象
	UINT16 *pFrmBuf = NULL;   	//指向帧缓存入口的指针
	UINT16 *pTmpBuf = NULL;		//临时帧缓冲指针
    
	UINT8 r;
	UINT8 g;
	UINT8 b;
	if(iReturn == 0)
    {    
		if( (uiPlaceX+uiWidth)>WINDOW_WIDTH || (uiPlaceY+uiHeight)>WINDOW_HEIGHT )
		{
			iReturn = -1;
		}
    }


	if(iReturn == 0)
    {    
        /* 得到当前帧缓冲映射对象并判断是否为有效指针 */
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iReturn = -2;
        }
    }

	if(iReturn == 0)
    {   
		int iNum = WINDOW_HEIGHT*WINDOW_WIDTH*2;
		UINT16 *pTmp = NULL;   		//中间指针变量

		/* 申请临时帧缓存，避免刷新时产生闪烁 */
        pTmpBuf = (UINT16 *)GuiMemAlloc(iNum);
	
    	/* 当前帧缓冲入口地址 */
    	pFrmBuf = pFbmap->pMapBuff;
		
        /* 复制出当前帧缓冲中的内容 */
        if (NULL != pTmpBuf)
        {
            memcpy(pTmpBuf, pFrmBuf, iNum);
        }

		//临时变量定义
		int i;
		
		for(i=0; i<uiHeight; i++)
		{	
			int j;
			/* 
			 * 申请临时帧缓冲成功，把计算后的像素值保存在临时帧缓冲中
			 * 全部计算完成后，再进行刷新，可避免闪烁
			 */
			if (NULL != pTmpBuf)
        	{
        		pTmp = pTmpBuf + uiPlaceX + (uiPlaceY+i)*WINDOW_WIDTH;
        	}
			/* 
			 * 申请临时帧缓冲失败，边计算便刷新，虽然闪烁，但保证能正常显示图像
			 */
			else
			{
				pTmp = pFrmBuf + uiPlaceX + (uiPlaceY+i)*WINDOW_WIDTH;
			}
			
			for(j=0; j<uiWidth; j++)
			{
				b = ((*pTmp)&0xF800)>>8;
	     		g = ((*pTmp)&0x7e0)>>3;
	     		r = ((*pTmp)&0x1f)<<3;

				r = (unsigned char)(ulColour>>16)*(255-ucAlpha)/255 + r*ucAlpha/255;
				g = (unsigned char)(ulColour>>8)*(255-ucAlpha)/255 + g*ucAlpha/255;
				b = (unsigned char)(ulColour)*(255-ucAlpha)/255 + b*ucAlpha/255;
				
	            *pTmp++ = ((r & 0xF8) >> 3) |   //RGB565，B分量:低5位
	                   	  ((g & 0xFC) << 3) |   //RGB565，G分量:中间6位
	                      ((b & 0xF8) << 8);    //RGB565，B分量:高5位

				//MakeAlpha(p, &colour, 20);
				//*p++ = colour;
			}
		}

		/* 计算完毕，刷新帧缓冲内容 */
		if (NULL != pTmpBuf)
    	{
    		memcpy(pFrmBuf, pTmpBuf, iNum);
			GuiMemFree(pTmpBuf);
    	}
	}

	return iReturn;
}

/***
  * 功能：
        设置指定区域内控件与设定的颜色进行透明显示
  * 参数：
        1.unsigned char ucAlpha:	Alpha值越大，原背景色越亮
        2.unsigned long ulColour:	24位颜色代码
        3.unsigned int uiPlaceX:    指定区域的X坐标
        4.unsigned int uiPlaceY:    指定区域的Y坐标
        5.unsigned int uiWidth:  	指定区域的宽度
        6.unsigned int uiHeight: 	指定区域的高度
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by  2018.06.27
  		此函数只是将指定区域进行透明色处理，如果要将区域内的控件置为无效，
  		请调用ChangeAbleInArea函数
  		区别于老的函数，该函数直接修改帧缓冲的数据，有可能出现闪烁现象
***/
int DispTransparentNew(unsigned char ucAlpha, unsigned long ulColour,
					 unsigned int uiPlaceX, unsigned int uiPlaceY, 
                     unsigned int uiWidth, unsigned int uiHeight)
{
	//错误标志、返回值定义
	int iReturn = 0;

	GUIFBMAP *pFbmap = NULL;	//帧缓冲对象
	UINT16 *pFrmBuf = NULL;   	//指向帧缓存入口的指针
    
	UINT8 r;
	UINT8 g;
	UINT8 b;
	if(iReturn == 0)
    {    
		if( (uiPlaceX+uiWidth)>800 || (uiPlaceY+uiHeight)>480 )
		{
			iReturn = -1;
		}
    }


	if(iReturn == 0)
    {    
        /* 得到当前帧缓冲映射对象并判断是否为有效指针 */
        pFbmap = GetCurrFbmap();
        if (NULL == pFbmap)
        {
            iReturn = -2;
        }
    }

	if(iReturn == 0)
    {   
		UINT16 *pTmp = NULL;   		//中间指针变量
	
    	/* 当前帧缓冲入口地址 */
    	pFrmBuf = pFbmap->pMapBuff;
		
		//临时变量定义
		int i;
		
		for(i=0; i<uiHeight; i++)
		{	
			int j;
			/* 
			 * 申请临时帧缓冲成功，把计算后的像素值保存在临时帧缓冲中
			 * 全部计算完成后，再进行刷新，可避免闪烁
			 */
			if (NULL != pFrmBuf)
        	{
        		pTmp = pFrmBuf + uiPlaceX + (uiPlaceY+i)*800;
        	}
			/* 
			 * 申请临时帧缓冲失败，边计算便刷新，虽然闪烁，但保证能正常显示图像
			 */
			else
			{
				pTmp = pFrmBuf + uiPlaceX + (uiPlaceY+i)*800;
			}
			
			for(j=0; j<uiWidth; j++)
			{
				b = ((*pTmp)&0xF800)>>8;
	     		g = ((*pTmp)&0x7e0)>>3;
	     		r = ((*pTmp)&0x1f)<<3;

				r = (unsigned char)(ulColour>>16)*(255-ucAlpha)/255 + r*ucAlpha/255;
				g = (unsigned char)(ulColour>>8)*(255-ucAlpha)/255 + g*ucAlpha/255;
				b = (unsigned char)(ulColour)*(255-ucAlpha)/255 + b*ucAlpha/255;
				
	            *pTmp++ = ((r & 0xF8) >> 3) |   //RGB565，B分量:低5位
	                   	  ((g & 0xFC) << 3) |   //RGB565，G分量:中间6位
	                      ((b & 0xF8) << 8);    //RGB565，B分量:高5位

			}
		}
	}

	return iReturn;
}

/***
  * 功能：
        公共服务线程
  * 参数：
        1.void *pThreadArg:   创建线程时传入的参数
  * 返回：
        无
  * 备注：
  		该线程主要用来处理电量图标显示、WIFI图标显示、LCD亮度管理等
***/
void *PublicService(void *pThreadArg)
{
	//错误标志、返回值定义
	static int iReturn = 0;
	struct timeval lastTime;
	struct timeval currTime;

    pGlobalBgState= CreatePicture(480, 0, 160, 40, BmpFileDirectory"bg_global_state_main.bmp");
    pGlobalBgWiFi = CreatePhoto("ico_wifi_null_main");
	pGlobalBgPower = CreatePicture(593, 11, 33, 18, BmpFileDirectory"ico_power_empty.bmp");
    pGlobalBgUDisk = CreatePhoto("ico_u_disk");
    pGlobalBgFiberMicroscope = CreatePhoto("ico_main_fibermicroscope");    
    pGlobalLblDate= CreateLabel(501, 12, 90, 16, pGlobalStrDate);

    InitMutex(&GStatusMutex, NULL);
#ifdef POWER_DEVICE
	PowerOnChargeStatusHandle();
#endif
	//设置光模块
	SetOpmSet(pOtdrTopSettings->pOpmSet);

	gettimeofday(&lastTime,NULL);
	while(1)
	{
		if(iSpeakerOnFlag == 1)
		{
#ifdef OPTIC_DEVICE
			SpeakerTick();
#endif
			iSpeakerOnFlag = 0;
		}
        
		if(iStartSpeaker == 1)
		{
#ifdef OPTIC_DEVICE
			SpeakerStart();
#endif
			iStartSpeaker = 0;
		}

		if(iPowerDownFlag)
		{
			break;
		}
		if( (enCurWindow == ENUM_LOGO_WIN) || (enCurWindow == ENUM_NULL_WIN))
		{
			MsecSleep(5);
			continue;
		}

		MsecSleep(50);

        gettimeofday(&currTime,NULL);
		if((currTime.tv_sec - lastTime.tv_sec) >= 1)
		{
		    lastTime = currTime;
			iTimeCount++;

			//1s递增1
			++iInstallmentCount;

            //12小时判断一次是否本期已经过期
            if (iInstallmentCount >= 43200)//12*60*60
            {
                iInstallmentCount = 0;
#ifdef EEPROM_DATA
				InstOutOfDate(1);//分期付款暂未使用
				//判断租赁是否到期
				if (IsLeaseExpiration())
				{
					mysystem("poweroff");
				}
#endif
            }
#ifdef POWER_DEVICE
            //只有电池读取成功后才可操作如下功能
            if (!ShowSta_PowerInfo(pGlobalBgPower))
            {
                //设置日期文本(电池信息i2c通信影响到rtc通信)
                ShowSta_DateInfo(1);
            }
            // ShowSta_WifiInfo(pGlobalBgWiFi, __g_iCurWin);
#endif
            //检测是否关屏或自动关机
            PowerSave();
        }
#ifdef POWER_DEVICE
        static int oldLedState = 0;
        int ledState = GetLedState();
        if (ledState != oldLedState) //只有状态位改变才设置led灯，减少寄存器的操作
        {
            //获取led灯的状态，即用户是否短按电源键，若短按打开/关闭led灯，
            Opm_SetLedEnable(pOtdrTopSettings->pOpmSet, ledState);
            oldLedState = ledState;
        }
#endif
    }
    //退出线程
    ThreadExit(&iReturn);
    
    return &iReturn;	
}


/***
  * 功能：
        创建公共服务线程
  * 参数：
       无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by  2014.10.15
  		该函数可以放其他创建线程，如wifi监听、传输文件线程的创建等
***/
int CreateThread(void)
{
	//错误标志、返回值定义
	static int iReturn = 0;	

	if(iReturn == 0)
	{
		if( ThreadCreate(&thdPublicService, NULL, PublicService, NULL) )
		{
			iReturn = -1;
		}
	}

	return iReturn;
}


/*** 
 * 功能： 
 	从帧缓冲区的拷贝中恢复一条横线到帧缓冲区
 * 参数： 
 	1.GUIFBMAP *pFb	:目标帧缓冲
 	2.UINT16 *pBackup:备份的帧缓冲入口地址
 	3.UINT32 sx	 	:起始x坐标
 	4.UINT32 sy   	:起始y坐标
 	5.UINT32 ex 	:结束x坐标
 * 返回： 
 	成功 0，失败负值
 * 备注
 	使用之前需要先备份帧缓冲的数据
***/ 
int RecoverFbLineHor16(GUIFBMAP *pFb,unsigned char *pBackup, 
 					   UINT32 sx, UINT32 sy, UINT32 ex)
{
    char *p = pFb->pMapBuff;
	UINT32 pixelbytes = pFb->uiPixelBits >> 3;
    UINT32 len = (ex - sx  + 1) * pixelbytes;
	UINT32 offset = (sx + sy * pFb->uiHorRes) * pixelbytes;

    p += offset;
	pBackup += offset;

	memcpy(p, pBackup, len);
 
    return 0;
}
/*** 
 * 功能： 
 	从帧缓冲区的拷贝中恢复一条竖线到帧缓冲区
 * 参数： 
 	1.GUIFBMAP *pFb	:目标帧缓冲
 	2.UINT16 *pBackup:备份的帧缓冲入口地址
 	3.UINT32 sx	 	:起始x坐标
 	4.UINT32 sy   	:起始y坐标
 	6.UINT32 ey	 	:结束y坐标
 * 返回： 
 	成功 0，失败负值
 * 备注
 	使用之前需要先备份帧缓冲的数据
***/ 
int RecoverFbLineVer16(GUIFBMAP *pFb, unsigned char *pBackup, 
					   UINT32 sx, UINT32 sy, UINT32 ey)
{
    char *p = pFb->pMapBuff;
	UINT32 pixelbytes = pFb->uiPixelBits >> 3;
    UINT32 loop = ey - sy  + 1;
	UINT32 offset1 = (sx + sy * pFb->uiHorRes) * pixelbytes;
	UINT32 offset2 = pFb->uiHorRes * pixelbytes;
	
    p += offset1;
	pBackup += offset1;
	
	while (loop--)

	{
		memcpy(p, pBackup, pixelbytes);
		p += offset2;
		pBackup += offset2;
	}
 
    return 0;
}


/*** 
 * 功能： 
 	在屏幕上画一个选择框
 * 参数： 
 	1.char *pfbsave :帧缓冲区的备份缓存起始地址
	2.SELECTAREA *pSelctArea:选择区域数据结构指针
 * 返回： 
 	成功 0，失败负值
 * 备注
 	使用之前需要先备份帧缓冲的数据
	默认会把选择框画到当前的帧缓冲里
***/ 
int DrawSeclectBox(unsigned char *pBackup, SELECTAREA *pSelctArea)
{
	static UINT32 sx = 0, ex = 0, sy = 0, ey = 0;

	GUIPEN *pPen = GetCurrPen();
	unsigned int uiColor = 0;
	
	//获取当前的帧缓冲区
	GUIFBMAP *pFb = GetCurrFbmap();

	//恢复之前选择框所占用的屏幕内容
	MutexLock(&(pFb->Mutex));
	RecoverFbLineHor16(pFb, pBackup, sx, sy, ex);
	RecoverFbLineVer16(pFb, pBackup, sx, sy, ey);
	RecoverFbLineHor16(pFb, pBackup, sx, ey, ex);
	RecoverFbLineVer16(pFb, pBackup, ex, sy, ey);
	MutexUnlock(&(pFb->Mutex));
	
	//获取当前选中的坐标并处理
	sx = pSelctArea->iSx; 
	ex = pSelctArea->iEx; 
	sy = pSelctArea->iSy; 
	ey = pSelctArea->iEy;

	if (sx > ex)
		GuiSwap(sx, ex);
	if (sy > ey)	
		GuiSwap(sy, ey);

	//绘制当前选择框
	uiColor = pPen->uiPenColor;
	pPen->uiPenColor = 0x0000ff;
	DrawRectangle(sx, sy, ex, ey);
	pPen->uiPenColor = uiColor;

	return 0;
}

/***
  * 功能：
		用于创建各窗体标题LABEL的底色
  * 参数：
		1.GUILABEL *lbl:    指向标签的指针
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
void CreateTitle(GUILABEL *lbl)
{
	//临时变量定义
	GUIPEN *pPen;
	UINT32 iPenColorBack;
	int iTmp, len = 0;

	//获得画笔，得到当前颜色，以便画完曲线后设回原来颜色
	DisplayLabel(lbl);
	for (iTmp = 0; iTmp < lbl->pLblText->uiTextLength; ++iTmp)
	{
		len += lbl->pLblText->ppTextMatrix[iTmp]->uiRealWidth + 2;
	}
	pPen = GetCurrPen();
	iPenColorBack = pPen->uiPenColor;
	pPen->uiPenColor = 40 << 16 | 57 << 8 | 79;//设置画笔颜色
	DrawBlock(2,2,500, 30);
	pPen->uiPenColor = 0x20ABDD;//设置画笔颜色
	DrawBlock(2, 2, len + 2 * (((GUIPOINT *)lbl)->x - 2), 
			  lbl->pLblText->uiViewHeight+7);
	pPen->uiPenColor = iPenColorBack;
}


/***
  * 功能：
        创建全局字体颜色对象
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by  2015.01.20
***/
static int CreateGlobalFont(void)
{
	int iReturn = 0;

	/* 黑色字体 */
    pGlobalFntBlack = CreateFont(FNTFILE_STD, 16, 16, 0x00000000, 0xFFFFFFFF);
                              		
	/* 黄色字体 */
	pGlobalFntYellow = CreateFont(FNTFILE_STD, 16, 16, 0x00F8A800, 0xFFFFFFFF);		

	/* 灰色字体 */
	pGlobalFntGray = CreateFont(FNTFILE_STD, 16, 16, 0x005F5F5F, 0xFFFFFFFF);	

	/* 灰色字体 */
	pGlobalFntGray1 = CreateFont(FNTFILE_STD, 16, 16, 0x00aeacac, 0xFFFFFFFF);		
                              		
	/* 白色字体 */
	pGlobalFntWhite = CreateFont(FNTFILE_STD, 16, 16, 0x00ffffff, 0xFFFFFFFF);		

	/* 红色字体 */
	pGlobalFntRed = CreateFont(FNTFILE_STD, 16, 16, 0x00E81853, 0xFFFFFFFF);			

	/* 绿色字体 */
	pGlobalFntGreen = CreateFont(FNTFILE_STD, 16, 16, 0x001C216, 0xFFFFFFFF);	

	/* 蓝色字体 */
	pGlobalFntBlue = CreateFont(FNTFILE_STD, 16, 16, 0x003F94FF, 0xFFFFFFFF);

	// 大字体 24*24
    /* 黑色字体 */
    pGlobalBigFntBlack = CreateFont(FNTFILE_BIG, 24, 24, 0x00000000, 0xFFFFFFFF);
                              		
	/* 黄色字体 */
	pGlobalBigFntYellow = CreateFont(FNTFILE_BIG, 24, 24, 0x00F8A800, 0xFFFFFFFF);		

	/* 灰色字体 */
	pGlobalBigFntGray = CreateFont(FNTFILE_BIG, 24, 24, 0x005F5F5F, 0xFFFFFFFF);	

	/* 灰色字体 */
	pGlobalBigFntGray1 = CreateFont(FNTFILE_BIG, 24, 24, 0x00aeacac, 0xFFFFFFFF);		
                              		
	/* 白色字体 */
	pGlobalBigFntWhite = CreateFont(FNTFILE_BIG, 24, 24, 0x00ffffff, 0xFFFFFFFF);		

	/* 红色字体 */
	pGlobalBigFntRed = CreateFont(FNTFILE_BIG, 24, 24, 0x00E81853, 0xFFFFFFFF);			

	/* 绿色字体 */
	pGlobalBigFntGreen = CreateFont(FNTFILE_BIG, 24, 24, 0x0000FF00, 0xFFFFFFFF);	

	/* 蓝色字体 */
	pGlobalBigFntBlue = CreateFont(FNTFILE_BIG, 24, 24, 0x003F94FF, 0xFFFFFFFF);
	return iReturn;
}


/***
  * 功能：
        销毁全局字体颜色对象
  * 参数：
		无
  * 返回：
        成功返回零，失败返回非零值
  * 备注：add by  2015.01.20
***/
static int DestoryGlobalFont(void)
{
	int iReturn = 0;

	/* 黑色字体 */
	DestroyFont(&pGlobalFntBlack);
                              		
	/* 黄色字体 */
	DestroyFont(&pGlobalFntYellow);		

	/* 灰色字体 */
	DestroyFont(&pGlobalFntGray);	
                              		
	/* 白色字体 */
	DestroyFont(&pGlobalFntGray1);	

	/* 绿色字体 */
	DestroyFont(&pGlobalFntWhite);

	/* 白色字体 */
	DestroyFont(&pGlobalFntRed);	

	/* 绿色字体 */
	DestroyFont(&pGlobalFntGreen);

	/* 白色字体 */
	DestroyFont(&pGlobalFntBlue);	

	// 大字体
	/* 黑色字体 */
	DestroyFont(&pGlobalBigFntBlack);
                              		
	/* 黄色字体 */
	DestroyFont(&pGlobalBigFntYellow);		

	/* 灰色字体 */
	DestroyFont(&pGlobalBigFntGray);	
                              		
	/* 白色字体 */
	DestroyFont(&pGlobalBigFntGray1);	

	/* 绿色字体 */
	DestroyFont(&pGlobalBigFntWhite);

	/* 白色字体 */
	DestroyFont(&pGlobalBigFntRed);	

	/* 绿色字体 */
	DestroyFont(&pGlobalBigFntGreen);

	/* 白色字体 */
	DestroyFont(&pGlobalBigFntBlue);	

	return iReturn;
}


//获取帧缓冲的备份区域
static int GetFrameBufferBak(void)
{
	GUIFBMAP *pFb = GetCurrFbmap();
	if (NULL == pFb)
	{
		return -1;
	}

	pFrameBufferBak = (unsigned char *)calloc(1, pFb->uiBufSize);
	if (NULL == pFrameBufferBak)
	{
		return -2;
	}

	return 0;
}

//释放帧缓冲备份区域
static void freeFrameBufBak(void)
{
	free(pFrameBufferBak);
	pFrameBufferBak = NULL;
}


//获取公共资源 包括字体等
int GetGlobalResource(void)
{
	//获取全局字体
	CreateGlobalFont();

	//获取全局帧缓冲备份区域
	GetFrameBufferBak();

	return 0;
}

//释放全局资源区域
int ReleasGlobalResource(void)
{
	//销毁字体
	DestoryGlobalFont();

	//释放帧缓冲
	freeFrameBufBak();
	
	return 0;
}

void TurnOffDEV(void)
{
    GUIWINDOW *pWnd = NULL;

	//断开wifi连接，结束wifi扫描
	// wifi_Quit();

	iPowerDownFlag = 1;
	

	pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
						FrmLogoInit, FrmLogoExit, 
						FrmLogoPaint, FrmLogoLoop, 
						FrmLogoPause, FrmLogoResume,
						NULL);			//pWnd由调度线程释放
	//退出当前窗体
	SendWndMsg_WindowExit(GetCurrWindow());//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);			//发送消息以便调用新的窗体
}

/***
  * 功能：
		初始化提示信息框
  * 参数：
		无
  * 返回：
		无
  * 备注：
  		类似于安卓的小的提示信息框
***/
void InitInfoDialog(void)
{
	__g_pInfoBtnDisable = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
	__g_pInfoBtnDisable->Visible.iLayer = 10;
	__g_pInfoBtnDisable->Visible.iCursor = 1;
	__g_pInfoBtnDisable->Visible.iFocus = 1;

	__g_pInfoStrInfo = TransString(""); 
	__g_pInfoLblInfo = CreateLabel(50, 200, 200, 30,
									      __g_pInfoStrInfo);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, __g_pInfoLblInfo);  	
}

/***
  * 功能：
		在屏幕上显示提示信息
  * 参数：
		1、GUICHAR *str	:	将要显示的提示信息
		2、GUIWINDOW *pWndObj:   需要添加GUI窗体控件的窗体对象
  * 返回：
		0、成功
		非0、失败
  * 备注：
  		类似于安卓的小的提示信息框，显示后其他控件不使能
***/
int ShowInfoDialog(GUICHAR *str, GUIWINDOW *pWndObj)
{ 
	int iErr = 0;

	if(iErr == 0)
	{
		if( (NULL == str) ||
			(NULL == pWndObj))
		{
			iErr = -1;
		}
	}

	/* 屏蔽控件加入窗体对列 */
	if(iErr == 0)
	{	
		AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), __g_pInfoBtnDisable, pWndObj); 
	}

	if(iErr == 0)
	{
		DispTransparent(20, 0x535353, 20, 200, 500, 30);
		SetLabelArea(170, 207, 370, 223, __g_pInfoLblInfo);
		__g_pInfoStrInfo = str;
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), __g_pInfoLblInfo);
		SetLabelText(__g_pInfoStrInfo, __g_pInfoLblInfo);
		DisplayLabel(__g_pInfoLblInfo);
		
		RefreshScreen(__FILE__, __func__, __LINE__);	
	}

	return iErr;
}

/***
  * 功能：
		不使能提示信息框
  * 参数：
		1、GUIWINDOW *pWndObj:   需要添加GUI窗体控件的窗体对象
  * 返回：
		0、成功
		非0、失败
  * 备注：
  		类似于安卓的小的提示信息框
***/
int DisableInfoDialog(GUIWINDOW *pWndObj)
{
	int iErr = 0;

	if(iErr == 0)
	{
		if(NULL == pWndObj)
		{
			iErr = -1;
		}
	}	

	if(iErr == 0)
	{
		/* 删除屏蔽控件从窗体队列 */
		DelWindowComp(__g_pInfoBtnDisable, pWndObj);
	}

	return iErr;
}

/***
  * 功能：
		弹出信息框
  * 参数：
		1、GUIWINDOW *pWndObj : 需要添加GUI窗体控件的窗体对象
		2、InCueFun fCue	  : 增加在弹出框期间相应的函数 不能为NULL
		3、InCueSetStr fCurStr: 根据参数2返回的值判断弹出框显示的内容 可以为NULL
								若fCurStr == NULL: fCue返回>=0 默认 "成功"
												       返回<0  默认 "失败"
  * 返回：
		0、成功
		非0、失败
  * 备注：
  		by 
  		edit 
***/
int DisplayCue(GUIWINDOW *pWnd, InCueFun fCue, InCueSetStr fCurStr)
{
	int iRet;
	if(NULL == fCue)
	{
		return -1;
	}
	GUIFBMAP *pFb = GetCurrFbmap();
	MutexLock(&(pFb->Mutex));
	memcpy(pFrameBufferBak, pFb->pMapBuff, pFb->uiBufSize);
    MutexUnlock(&(pFb->Mutex));
    
	GUIPEN *pPen = GetCurrPen();
	unsigned int uiPenColorBak;

	GUIPICTURE *pBgObj = CreatePicture(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,NULL);
	pBgObj->Visible.iLayer = 10;
	pBgObj->Visible.iCursor = 1;
	pBgObj->Visible.iFocus = 1;

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pBgObj, pWnd);

	iRet = (*fCue)(0, NULL);
	CODER_LOG(CoderGu, "iRet = %d\n", iRet);


    GUICHAR    *pStrObj = NULL;
	if(NULL == fCurStr)
	{
        pStrObj = (iRet == 0) ? TransString("CUE_SUCCESS") :  TransString("CUE_FAILURE");
	}
	else
	{
		pStrObj = (*fCurStr)(iRet);
	}

    if(pStrObj)
    {
    	GUILABEL   *pLblObj = CreateLabel(55, 235, 630, 24, pStrObj);
    	SetLabelAlign(GUILABEL_ALIGN_CENTER, pLblObj);
    		
    	uiPenColorBak = pPen->uiPenColor;
        if(iRet == -6)
        {
            //删除屏蔽控件从窗体队列 并销毁
        	DelWindowComp(pBgObj, pWnd);
        	DestroyPicture(&pBgObj);
            return 0;
        }
    	pPen->uiPenColor = (iRet < 0) ? 0xff0000 : 0x008fff;
    	DrawBlock(55, 220, 685, 265);
    	pPen->uiPenColor = uiPenColorBak;
    	
    	DisplayLabel(pLblObj);
    	RefreshScreen(__FILE__, __func__, __LINE__);

    	DestroyLabel(&pLblObj);
    	GuiMemFree(pStrObj);
    	
    	MsecSleep(2800);
	}
	
	MutexLock(&(pFb->Mutex));
	memcpy(pFb->pMapBuff, pFrameBufferBak, pFb->uiBufSize);
    MutexUnlock(&(pFb->Mutex));
	RefreshScreen(__FILE__, __func__, __LINE__);

	//删除屏蔽控件从窗体队列 并销毁
	DelWindowComp(pBgObj, pWnd);
	DestroyPicture(&pBgObj);

	return 0;

}

int CheckStorgeDev(int ac, void *gv)
{
	return -1;
}

GUICHAR* GetMsgLabel(int ac)
{
	return TransString("DIALOG_NO_STORAGE");
}

int MsgNoSdcard(GUIWINDOW *pWnd)
{
	return DisplayCue(pWnd, CheckStorgeDev, GetMsgLabel);
}

//设置wifi和电池信息状态栏是否显示和显示的状态
//iEnable:是否显示使能 0:半透明显示 1:全显示 2:不显示
//Wnd:是否显示使能 2:OTDR界面 其他:其他界面
int SetPowerEnable(int iEnable, int Wnd)
{
    char bitmapName[512] = {0};
    char* temp = NULL;//查找wifi图标的路径名
	MutexLock(&GStatusMutex);
	iPowerEnable = iEnable;
	
	if(iEnable == 1)        //
	{
		__g_iCurWin = Wnd;
		strcpy(bitmapName, pGlobalBgWiFi->pPicBitmap->strBitmapFile);
        
		if(Wnd == 1)
		{            
            temp = strstr(bitmapName, "main.bmp");
            if(temp)
            {
                strcpy(temp, "other.bmp");
            }
		}
		else
		{
            temp = strstr(bitmapName, "other.bmp");
            if(temp)
            {
                strcpy(temp, "main.bmp");
            }
		}
        
        SetPictureBitmap(bitmapName, pGlobalBgWiFi);
        
        if(iLowPower == 1)
        {
		    SetPictureBitmap(BmpFileDirectory"bg_global_state_main1.bmp", pGlobalBgState);
        }
        else
        {
            SetPictureBitmap(BmpFileDirectory"bg_global_state_main.bmp", pGlobalBgState);
        }
        
		DisplayPicture(pGlobalBgState);
		DisplayPicture(pGlobalBgPower);
		DisplayLabel(pGlobalLblDate);
	}

	MutexUnlock(&GStatusMutex);
	return 0;
}

/***
  * 功能：
        显示进度条
  * 参数：
        1.unsigned int uiPlaceX	:       进度条水平放置位置，以左上角为基点
        2.unsigned int uiPlaceY	:       进度条垂直放置位置，以左上角为基点
        3.unsigned int uiWidth	:      	进度条水平宽度
        4.unsigned int uiHeight	:     	进度条垂直高度  
        5、int iProgress		:		当前进度，0-100
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int ShowProgressBar(unsigned int uiPlaceX, unsigned int uiPlaceY, 
                    unsigned int uiWidth, unsigned int uiHeight, 
                    int iProgress)
{
	int iErr = 0;

	GUICHAR *pProgressStr   = NULL;
	GUILABEL *pProgressLbl 	= NULL;

    GUIPEN *pPen;
	unsigned int uiColorBack;	
	unsigned int uiCurWidth = 0;
	
	char cTmpBuf[50];


	if (iErr == 0)
	{
		//判断uiWidth和uiHeight是否有效
		if (uiWidth < 1 || uiHeight < 1)
		{
			iErr = -1;
		}
	}

	if(iErr == 0)
	{
		pPen = GetCurrPen();
		uiColorBack = pPen->uiPenColor;

		/* 根据当前的进度，计算绘制的进度条的宽度 */
		uiCurWidth = (iProgress*uiWidth)/100;
		
		/* 画进度条背景 */
		pPen->uiPenColor = 0xF4F4F4;
		DrawBlock(uiPlaceX, uiPlaceY, uiPlaceX + uiWidth, uiPlaceY + uiHeight);

		/* 画当前进度 */
		pPen->uiPenColor = 0xEBD300;
		DrawBlock(uiPlaceX, uiPlaceY, uiPlaceX + uiCurWidth, uiPlaceY + uiHeight);
	}

	if(iErr == 0)
	{
		
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%d%%", iProgress);
		pProgressStr = TransString(cTmpBuf);
		if(uiWidth - uiCurWidth > 32)
		{
		    pProgressLbl = CreateLabel(uiPlaceX+uiCurWidth, uiPlaceY-10, 50, 24, 
								   pProgressStr);
		}
		else
		{
            pProgressLbl = CreateLabel(uiPlaceX+uiWidth-32, uiPlaceY-10, 50, 24, 
								   pProgressStr); 
		}
		
		SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pProgressLbl);

		/* 画背景 */
		pPen->uiPenColor = 0xF4F4F4;
		DrawBlock(uiPlaceX, uiPlaceY-10, uiWidth + uiPlaceX, uiHeight + uiPlaceY-10);
		
		DisplayLabel(pProgressLbl);
		RefreshScreen(__FILE__, __func__, __LINE__);	
		GuiMemFree(pProgressStr);
		DestroyLabel(&pProgressLbl);

		pPen->uiPenColor = uiColorBack;	
	}

	return iErr;
}

/***
  * 功能：
        分期付款功能是否有效
  * 参数：
		无
  * 返回：
        加密为非0 ，不加密为0
  * 备注：
***/
int InstIsEncrypted()
{
	//获得分期结构体，判断是否过期。
	INSTALLMENT inst;
	GetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET);
	return InstEncryptedFlag(&inst);
}

/***
  * 功能：
        分期付款信息是否已经过期
  * 参数：
		1、int shutdown：过期则立即关机的标记
  * 返回：
        过期返回非0，不过期返回0
  * 备注：
***/
int InstOutOfDate(int shutdown)
{
	//获得分期结构体，判断是否过期。
	INSTALLMENT inst;
	GetSettingsData(&inst, InstSize(&inst), INSTALLMENT_SET);
	InstPrint(&inst);
	if (Encrypted != InstEncryptedFlag(&inst))
	{
		return 0;
	}

	int flag = InstIsOutOfDate(&inst, 1, 0);
	if (flag)
	{
		//是否立即关机
		if (shutdown)
		{
			mysystem("poweroff");
		}
	}
	return flag;
}

int getTimeCount()
{
	return iTimeCount;
}

int ScreenShot(void)
{
	int iErr = 0;
#ifdef SAVE_SCREEN

	char strFile[512] = {0};

	GUIFBMAP *pFbmap = GetCurrFbmap();	//帧缓冲对象

	time_t t = 0;
	struct tm *p = NULL;
	
	if (0 > (t = time(NULL)))
	{
		return -2;
	}
	
	if (NULL == (p = gmtime(&t))) 
	{
		return -3;
	}
	GUIPICTURE *pBgObj = CreatePicture(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,NULL);
	pBgObj->Visible.iLayer = 10;
	pBgObj->Visible.iCursor = 1;
	pBgObj->Visible.iFocus = 1;

	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pBgObj, GetCurrWindow());

    if (0 == CheckMountUSB())
    {
    	sprintf(strFile, MntUsbDirectory"/BMP%.4d-%.2d%.2d-%.2d%.2d%.2d.bmp", 
    			p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, 
    			p->tm_hour, p->tm_min, p->tm_sec);
    }
    else
    {
    	sprintf(strFile, MntDataDirectory"/BMP%.4d-%.2d%.2d-%.2d%.2d%.2d.bmp", 
    			p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, 
    			p->tm_hour, p->tm_min, p->tm_sec);    
    }
    
	memcpy(pFrameBufferBak, pFbmap->pMapBuff, pFbmap->uiBufSize);

    if(__g_iCurWin== ENUM_PORT_WIN)//fip界面截取端面图像
    {
        iErr = ScreenShots(39, 517, 106, 458, strFile);
    }
    else
    {
        iErr = ScreenShots(0, 799, 0, 479, strFile);
    }
    
	if (!iErr) {
		memset(pFbmap->pMapBuff, 33, pFbmap->uiBufSize);
		RefreshScreen(__FILE__, __func__, __LINE__);
		MsecSleep(100);

	    MutexLock(&(pFbmap->Mutex));
		memcpy(pFbmap->pMapBuff, pFrameBufferBak, pFbmap->uiBufSize);
	    MutexUnlock(&(pFbmap->Mutex));
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	//删除屏蔽控件从窗体队列 并销毁
	DelWindowComp(pBgObj, GetCurrWindow());
	DestroyPicture(&pBgObj);
#endif

	return iErr;
}

static GUIPICTURE *pDisableLayer = NULL;

int DisableAllWidgets(GUIWINDOW *pWnd)
{
	pDisableLayer = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
	pDisableLayer->Visible.iCursor = 1;
	pDisableLayer->Visible.iFocus = 1;
	pDisableLayer->Visible.iLayer = 100;

	if (pWnd != NULL)
	{
	    AddWindowComp(OBJTYP_GUIPICTURE, sizeof(pDisableLayer), pDisableLayer, 
					  pWnd);
	}
	else
	{
		DestroyPicture(&pDisableLayer);
		return -1;
	}

	return 0;
}

int EnableAllWidgets(GUIWINDOW *pWnd)
{
	if (pWnd != NULL)
	{
	    DelWindowComp(pDisableLayer, pWnd);
	}

	DestroyPicture(&pDisableLayer);
	return 0;
}


int GetLabelRealWidth(GUILABEL *pLblObj)
{
	unsigned int uiSize, uiWidth;
	GUIFONT *pFont = GetCurrFont();

	if (NULL == pLblObj->pLblText)
	{
		return -1;
	}
	
    //若文本点阵资源未装载，尝试装载文本点阵资源
    if (NULL == pLblObj->pLblText->ppTextMatrix)
    {
        if (LoadTextMatrix(pLblObj->pLblText))
        {
            return -2;
        }
    }
	
	//计算点阵的实际宽度
	uiWidth = 0;
    for (uiSize = 0; uiSize < pLblObj->pLblText->uiTextLength; uiSize++)
    {
		uiWidth += pLblObj->pLblText->ppTextMatrix[uiSize]->uiRealWidth
		       + pFont->uiLeftGap + pFont->uiRightGap;
    }

	//卸载点阵
	UnloadTextMatrix(pLblObj->pLblText);
	
	return uiWidth;
}

int KnobFrequency(unsigned int uiMsec)
{
	static struct timeval lastTv = {0, 0};
	struct timeval tv, nowTv;
	
	gettimeofday(&nowTv, NULL);
	tv.tv_sec = nowTv.tv_sec - lastTv.tv_sec;
	tv.tv_usec = nowTv.tv_usec - lastTv.tv_usec;
	if ((tv.tv_sec * 1000000 + tv.tv_usec) > uiMsec * 1000)
	{
		lastTv = nowTv;
		return 0;
	}
	return 1;
}

/***
  * 功能：
		初始化消息队列
  * 参数：
		无
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int InitMsg(void)
{
	int iErr = 0;

	//建立消息队列	
	g_MsgId = msgget((key_t)2222, 0666 | IPC_CREAT);	
	if(g_MsgId == -1)  
	{  
		iErr = -1;	
	}		

	return iErr;
}


/***
  * 功能：
		向消息队列发送消息
  * 参数：
		1、char *cmdBuf		:	发送的数据
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int SendMsg(char *cmdBuf)
{
	int iErr = 0;

	struct msg_st Message;
	
	Message.msg_type = 2;
	memcpy(Message.text, cmdBuf, 1024);
	if(msgsnd(g_MsgId, (void*)&Message, 1024, 0) == -1)  
	{  
		iErr = -1; 
	} 

	return iErr;
}

/***
  * 功能：
		从消息队列中获取消息
  * 参数：
		1、struct msg *pMsg:	消息缓冲区	
  * 返回：
		成功返回零，失败返回非零值
  * 备注：
***/
int RecMsg()
{
	int iErr = 0;	
	struct msg_st Message;
	
	Message.msg_type = 0;
	if(msgrcv(g_MsgId, (void*)&Message, 128, 1, 0) == -1)  
	{  
		return iErr;
	}
	else
	{
		if(strncmp(Message.text,"OK",2) == 0)
			return 0;
		else
			return -1;
	}
}

/***
  * 功能：
        判断是否可以写指定文件
        若文件存在，内部弹出对话框让用户选择是否覆盖
  * 参数：
        filePath 指定的文件全路径
  * 返回：
        成功返回1，否则返回0
  * 备注：
        由于涉及到UI的绘制，本函数只能在UI线程里调用
***/
int CanWriteFile(const char* filePath)
{
    int r = 1;
    
    if (0 == access(filePath, F_OK))
    {
        char* p = strrchr(filePath, '/');
        if (!p)
        {
            p = (char*)filePath;
        }
        else 
        {
            p += 1;
        }
        
        unsigned int button = ShowStandardMessageBox(
            GetCurrLanguageText(OTDR_LBL_SAVE_FILE), 
            GetCurrLanguageText(FILE_LBL_FILE_ALREADY_EXIST)
            );
        if (button != MessageBox_Yes)
        {
            r = 0;
        }
    }   

    return r;
}

//保存到U盘(针对sor与sola)
void SaveToUsb(CALLLBACKWINDOW func, char* lastPath, char* fileName, FILETYPE fileType)
{
    if(fileName == NULL)
        return;
    SetQuickSaveName(fileName);
	CreateQuickSaveWin(MntUsbDirectory, lastPath, func,
					   TransString("OTDR_SAVE_TO_USB"), fileType);
}

/***
  * 功能：
    执行一个保存操作，结束后在界面显示操作成功或失败
  * 参数：
        pFbBackup 临时备份屏幕缓冲内容
        pCallerWnd 当前窗体
        operateFunc 执行保存的函数
        absolutePath 保存的绝对路径
  * 返回：
        成功返回1，否则返回0
  * 备注：
        由于涉及到UI的绘制，本函数只能在UI线程里调用
***/
void GuiOperateSave(unsigned char *pFbBackup, GUIWINDOW *pCallerWnd, Operate operateFunc, char *absolutePath)
{
	int iErr = 0;
	GUILABEL *pLbl = NULL;
	GUICHAR *pStr = NULL;
    char *strFileName;
	//创建屏蔽控件加入窗体对列
	GUIPICTURE* pDisable = CreatePicture(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
	pDisable->Visible.iLayer = 10;
	pDisable->Visible.iCursor = 1;
	pDisable->Visible.iFocus = 1;
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), pDisable, pCallerWnd);
	//创建报告
    strFileName = strrchr(absolutePath, '/');
    strFileName++;
	iErr = (*operateFunc)(absolutePath);
	//显示创建完成提示信息
	GUIFBMAP *pFb = GetCurrFbmap();	
	memcpy(pFbBackup, pFb->pMapBuff, pFb->uiBufSize);

	pStr = iErr ? GetCurrLanguageText(OTDR_CURVE_LBL_SAVE_FAILED) :
				  GetCurrLanguageText(OTDR_CURVE_LBL_SAVE_SUCCESS);
    if (!iErr) 
    {
        GUICHAR* pTemp = TransString("  ");
        StringCat(&pStr, pTemp);
        free(pTemp);
        pTemp = TransString(strFileName);
        StringCat(&pStr, pTemp);
        free(pTemp);
    }
	pLbl = CreateLabel(120, 235, 300, 24, pStr);
	SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pLbl);
	SetLabelAlign(GUILABEL_ALIGN_CENTER, pLbl);

	GUIPEN *pPen = GetCurrPen();
	unsigned int uiValue = pPen->uiPenColor;
	pPen->uiPenColor = iErr ? 0xff0000 : 0x008fff;
	DrawBlock(120, 220, 420, 274);
	pPen->uiPenColor = uiValue;

	DisplayLabel(pLbl);
	RefreshScreen(__FILE__, __func__, __LINE__);

	DestroyLabel(&pLbl);
	free(pStr);

	MsecSleep(2800);
	memcpy(pFb->pMapBuff, pFbBackup, pFb->uiBufSize);
	RefreshScreen(__FILE__, __func__, __LINE__);

	//删除屏蔽控件从窗体队列 并销毁
	DelWindowComp(pDisable, pCallerWnd);
	DestroyPicture(&pDisable);
}

int GetFirstPath(char* pCurrPath, char *pFirstPath)
{
    int iErr = 0;
    if(access(pCurrPath, F_OK) != 0)
    {
        if(access(MntSDcardDataDirectory, F_OK) != 0)
        {
            if((iErr = (access(MntUsbDirectory, F_OK) != 0) ? -1 : 1) == 1)
            {
                strcpy(pFirstPath, MntUsbDirectory);
            }
        }
        else
        {
            strcpy(pFirstPath, MntSDcardDataDirectory);
            iErr = 2;
        }
    }
    else
    {
        strcpy(pFirstPath, pCurrPath);
    }

    return iErr;
}

//是否阿拉伯波斯语
int isArabic()
{
	int font24 = 0;
	int index = GetCurrLanguageSet();
	// 语言阿拉伯语，波斯
	if (index >= LANG_ARABIA && index <= LANG_PERSIAN)
	{
		font24 = 1;
	}
	return font24;
}

//是否泰语
int isThai()
{
	int font24 = 0;
	int index = GetCurrLanguageSet();
	// 泰语，萨瓦迪卡
	if (index == LANG_THAI)
	{
		font24 = 1;
	}
	return font24;
}

int isFont24()
{
	int font24 = 0;
	int index = GetCurrLanguageSet();
	// 语言阿拉伯语，波斯
	if (index >= LANG_THAI && index <= LANG_PERSIAN)
	{
		font24 = 1;
	}
	return font24;
}

//泰语处理算法，不通用，只能结合我们的字体来使用
unsigned short * thaiLangProcess(unsigned short * unicode)
{
	unsigned short *processedUnicode = NULL;
    if(unicode)
    {	
    	static unsigned short buf[2048] = {0};

		int len = 0;
		unsigned short *tmp = unicode;
		while (*(tmp))              //文本内容，以'\0'结尾
        {
            len++;
            tmp++;
        }
		int i = 0;
		int j = 0;
		while(unicode[i] != '\0')
		for (i = 0; i < len; i++)
		{
			int flag = 0;
			int index = 0;
			for (index = 0; index < 8; index++)
			{
				if (unicode[i] == CombineCode[i][0] && (i+1 < len))
				{
					int k = 1;
					for (k = 1; k < 6; k++)
					{
						if (CombineCode[i][k] == unicode[i+1])
						{
							flag = 1;
							buf[j++] = CombineCode[i][k+5];
							i += 1;
							break;
						}
					}
					if (flag)
					{
						break;
					}
				}
			}

			//非组合字符
			if (0 == flag)
			{
				//特殊处理字符0x0E33
				if (0x0E33 == unicode[i])
				{
					buf[j++] = 0x0E4D;
					buf[j++] = 0x0E32;
				}
				else
				{
					buf[j++] = unicode[i];
				}
			}
		}
		buf[j] = '\0';
		
        processedUnicode = (unsigned short *)malloc(sizeof(unsigned short) * (j+1));
		memcpy(processedUnicode, buf, sizeof(unsigned short) * (j+1));
		processedUnicode[j] = '\0';
    }
    return processedUnicode;
}


GUIFONT * getGlobalFnt(FontColor color)
{
	GUIFONT *font = pGlobalFntWhite;
	int font24 = isFont24();
	
	switch (color)
	{
		case EN_FONT_BLACK:
			font = font24 ? pGlobalBigFntBlack : pGlobalFntBlack;
			break;
		case EN_FONT_YELLOW:
			font = font24 ? pGlobalBigFntYellow : pGlobalFntYellow;
			break;
		case EN_FONT_GRAY:
			font = font24 ? pGlobalBigFntGray : pGlobalFntGray;
			break;
		case EN_FONT_GRAY1:
			font = font24 ? pGlobalBigFntGray1 : pGlobalFntGray1;
			break;
		case EN_FONT_WHITE:
			font = font24 ? pGlobalBigFntWhite : pGlobalFntWhite;
			break;
		case EN_FONT_RED:
			font = font24 ? pGlobalBigFntRed: pGlobalFntRed;
			break;
		case EN_FONT_GREEN:
			font = font24 ? pGlobalBigFntGreen : pGlobalFntGreen;
			break;
		case EN_FONT_BLUE:
			font = font24 ? pGlobalBigFntBlue : pGlobalFntBlue;
			break;
		default:
			font = font24 ? pGlobalBigFntWhite : pGlobalFntWhite;
			break;
	}

	return font;
}

//检测字符串是否只包含字母数字和下划线  返回0"name"合格
int isNameStr(char *name)
{
    if(!name || strlen(name) == 0)
    {
        return -1;
    }

    int i;
    for(i=0; i<strlen(name); i++)
    {
	    if(strchr("/\\:*?<>|\"", name[i]))
        {
        	
            return -1;
        }
    }

    return 0;
}

//判断给的文件和文件名是否存在
int isFileExist(const char *name, const char *path)
{
    char temp[512] = {0};
    int pathLen = strlen(path);
    
    if(name && path)
    {
        if(path[pathLen - 1] != '/')
        {
            sprintf(temp, "%s/%s", path, name);
        }
        else
        {
            sprintf(temp, "%s%s", path, name);
        }
        if(!access(temp, F_OK))
        {
            return 1;
        }
    }
    
    return 0;
}

//移动标记线
static void MoveOtdrMarker(int option)
{
	if (__g_iCurWin == 2)//otdr界面有效
	{
		if (option == KEYCODE_LEFT) //left move
			WriteOTDRMsgQueue(ENUM_OTDRMSG_LEFT_MOVE_MARK);
		else if (option == KEYCODE_RIGHT) //right move
			WriteOTDRMsgQueue(ENUM_OTDRMSG_RIGHT_MOVE_MARK);
	}
}

//处理长按按键移动标记线响应
int HandleMoveOtdrMark(int KeyState, int KeyCode)
{
	int ret = 0;

	static int downFlag = 0;
	static struct timeval start, end;
	static int iMoveMarkCount = 0; //标记线移动次数

	if (KeyState) // 按键按下的情况
	{
		if (downFlag) // 已经被标记按下
		{
			gettimeofday(&end, NULL);
			long msec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

			if (msec > 1000) // 按下大于1s 才能移动标记线
			{
				//处理otdr长按左右键移动标记线,暂时间隔200ms发送消息命令控制标记线移动
				if ((msec - 1000) >= 300 * iMoveMarkCount)
				{
					MoveOtdrMarker(KeyCode);
					iMoveMarkCount++;
				}
			}

			ret = -2;
		}
		else //没被标记按下，计时开始
		{
			downFlag = 1;
			iMoveMarkCount = 0;
			gettimeofday(&start, NULL);
		}
	}
	else // 按键弹起的情况
	{
		if (iMoveMarkCount) //取消弹起操作,长按移动标记线的状态下取消界面按键弹起响应
		{
			ret = -2;
		}

		downFlag = 0;
		iMoveMarkCount = 0;
	}

	return ret;
}

// 长按截图功能，注意这段代码，里面的标志位设置以及返回值比较绕，一定要在纸上划一下
void KeyDownScreenShot(void)
{
    unsigned int bright = CheckPowerOn(ADAPTER) ? pCurSystemSet->uiDCLCDBright : pCurSystemSet->uiACLCDBright;
    // 截图成功
    ScreenShots(0, WINDOW_WIDTH - 1, 0, WINDOW_HEIGHT - 1, NULL);
    //设置亮度变暗
    BrightnessSet(0);
    MsecSleep(10);
    // 设置恢复亮度
    BrightnessSet(bright);
}

//输出错误信息到文件中
void PrintErrorInfo(char* fileName, char* log, int iCreateFile)
{
	FILE *stream;

	if(!log)
		return;

    if(access(fileName, F_OK) == -1)
    {
        if(iCreateFile)
        {
            char buf[128] = {0};
            sprintf(buf, "touch %s", fileName);
            mysystem(buf);
        }
        else
        {
            return;
        }
    }
    
	stream = fopen(fileName, "a+");

	if(stream == NULL)
		return;

	fprintf(stream, "%s\n", log);

	fclose(stream);
}

//返回主界面
void ReturnMenuOK(void)
{
	GUIWINDOW *pWnd = NULL;
    pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
						FrmMainInit, FrmMainExit, 
						FrmMainPaint, FrmMainLoop, 
						FrmMainPause, FrmMainResume,
						NULL);			//pWnd由调度线程释放
	SendWndMsg_WindowExit(GetCurrWindow());	//发送消息以便退出当前窗体
	SendSysMsg_ThreadCreate(pWnd);		//发送消息以便调用新的窗体
}

//跳转到vfl界面
void JumpVflWindow(void)
{
	//判断当前界面
	if (__g_iCurWin != 2)//otdr界面有效
	{
		GUIWINDOW *pWnd = NULL;
		pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
							FrmVflInit, FrmVflExit,
							FrmVflPaint, FrmVflLoop,
							FrmVflPause, FrmVflResume,
							NULL);				//pWnd由调度线程释放
		SendWndMsg_WindowExit(GetCurrWindow()); //发送消息以便退出当前窗体
		SendSysMsg_ThreadCreate(pWnd);			//发送消息以便调用新的窗体
	}
}

/***
  * 功能：
        保存系统设置
  * 参数：
		1.int iResetSystemFlag:是否要重置系统参数的标志位，0：不重置，1：重置
		2.unsigned short languageRecover：当iResetSystemFlag=1，该参数会被使用，否则不用（只有重置系统的时候，系统设置的语言不能修改）
        无
  * 备注：
***/

void SaveSystemSet(int iResetSystemFlag, unsigned short languageRecover)
{
    if(iResetSystemFlag)
    {
        ResetSystemSet((void *)pCurSystemSet);
        SetCurrLanguage(languageRecover);
    }
    // 保存系统设置参数
    SetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    SaveSettings(SYSTEM_SET);
}

//租赁是否到期,到期返回0，未到期返回1
int IsLeaseExpiration()
{
	RTCVAL* pCurRtc = NULL;
	pCurRtc = GetCurTime();
	int iReturn = 0;

	//检测参数
	if (!pCurRtc)
	{
		LOG(LOG_ERROR, "---pCurRtc == NULL-----\n");
		return -1;
	}

	if (pCurRtc->tm_year > (pFactoryConf->currentDate.year+1970))
	{
		iReturn = 1;
		LOG(LOG_ERROR, "------The year is over--------\n");
	}
	else if ((pCurRtc->tm_year == (pFactoryConf->currentDate.year + 1970)) 
		&& (pCurRtc->tm_mon > (pFactoryConf->currentDate.month + 1)))
	{
		iReturn = 2;
		LOG(LOG_ERROR, "------The month is over--------\n");
	}
	else if ((pCurRtc->tm_year == (pFactoryConf->currentDate.year + 1970))
		&& (pCurRtc->tm_mon == (pFactoryConf->currentDate.month + 1))
		&&(pCurRtc->tm_mday > (pFactoryConf->currentDate.day + 1)))
	{
		iReturn = 3;
		LOG(LOG_ERROR, "------The day is over--------\n");
	}

	return iReturn;
}