/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmsystemsetcontrol.c
* 摘    要：  实现主窗体FrmSystemSetControl的窗体处理操作函数声明
*          控制系统设置的各个选项
* 当前版本：  v1.0.0
* 作    者：wjg
* 完成日期：2020/8/31 
*
*******************************************************************************/

#include "wnd_frmsystemsetcontrol.h"

#include "app_frmbrightset.h"
#include "app_frmtimeset.h"
#include "app_frmbatteryset.h"
#include "app_speaker.h"

#include "guimessage.h"

#include "wnd_global.h"
#include "wnd_frminputpassward.h"
#include "wnd_frmlanguageconfig.h"

//声明结构体变量
static SYSTEM_SET_CONTROL* sSystemSetControl = NULL;
//点击亮度光标响应处理
// static int WndSystemSetCursorBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
// static int WndSystemSetCursorBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//亮度按钮响应处理
// static int WndSystemSetBrightnessBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
// static int WndSystemSetBrightnessBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//修改系统设置数据的内容
static void SetLabelContent(SYSTEM_SET_CONTROL* pSystemSetControl);

//按钮响应处理
static int WndSystemSetBtnIcon_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndSystemSetBtnIcon_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//按钮响应处理
static int WndSystemSetBtnSelected_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndSystemSetBtnSelected_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//按键响应处理
static int WndSystemSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int WndSystemSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen);

//刷新选中当前页面的选项
static void FlushCurrentWindow(SYSTEM_SET_CONTROL *pSystemSetControl, int iSelected);
//获取当前窗体焦点位置（物理按键作用下）
static void GetCurWindowFocus(SYSTEM_SET_CONTROL *pSystemSetControl, int option);

//图标背景资源
static char *pBtnIconBmp[SYSTEM_INFO_LIST_NUM] = {
	BmpFileDirectory "bg_sys_language_unpress.bmp",
	BmpFileDirectory "bg_sys_autoOff_unpress.bmp",
	BmpFileDirectory "bg_sys_standby_unpress.bmp",
	BmpFileDirectory "bg_sys_keySound_unpress.bmp",
	BmpFileDirectory "bg_sys_reset_unpress.bmp",
	BmpFileDirectory "bg_sys_upgrade_unpress.bmp",
	BmpFileDirectory "bg_sys_systemInfo_unpress.bmp",
	BmpFileDirectory "bg_sys_userGuide_unpress.bmp",
};

static char *pBtnIconBmpf[SYSTEM_INFO_LIST_NUM] = {
	BmpFileDirectory "bg_sys_language_press.bmp",
	BmpFileDirectory "bg_sys_autoOff_press.bmp",
	BmpFileDirectory "bg_sys_standby_press.bmp",
	BmpFileDirectory "bg_sys_keySound_press.bmp",
	BmpFileDirectory "bg_sys_reset_press.bmp",
	BmpFileDirectory "bg_sys_upgrade_press.bmp",
	BmpFileDirectory "bg_sys_systemInfo_press.bmp",
	BmpFileDirectory "bg_sys_userGuide_press.bmp",
};

//开关状态图标
static char *pSwitchStatusBmp[2] = {
	BmpFileDirectory "btn_off_unpress.bmp",
	BmpFileDirectory "btn_on_unpress.bmp"
};

static char *pSwitchStatusBmpf[2] = {
	BmpFileDirectory "btn_off_select.bmp",
	BmpFileDirectory "btn_on_select.bmp"
};

//状态标志位
static int iSwitchStatus[SYSTEM_INFO_LIST_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0 };

/***
  * 功能：
        创建一个系统信息控件
  * 参数：
        1.iNum            :转折点（弹出提示框）
        2.int iFocus      :菜单条目焦点 <= 0表示所有项都无焦点
        3.SECBACKFUNC     :回调函数
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 

SYSTEM_SET_CONTROL* CreateSystemSetControlResource(int iNum, int iFocus, SECBACKFUNC CallBack)
{
    //错误标志定义
	int iErr = 0;
    int i= 0;
    SYSTEM_SET_CONTROL* pSystemSetControl = NULL;
    
    if (!iErr)
	{
		//分配资源
		pSystemSetControl = (SYSTEM_SET_CONTROL *)calloc(1, sizeof(SYSTEM_SET_CONTROL));
		if (NULL == pSystemSetControl)
		{
			LOG(LOG_ERROR, "pSystemInfoControl Malloc ERR\n");
			iErr = -2;
		}
	}

    // if(!iErr)
    // {
    //     //分配资源
	// 	pSystemSetControl->pFrmBrightness = (BRIGHTNESS_CONTROL *)calloc(1, sizeof(BRIGHTNESS_CONTROL));
	// 	if (NULL == pSystemSetControl->pFrmBrightness)
	// 	{
	// 		LOG(LOG_ERROR, "pSystemInfoControl->pFrmBrightness Malloc ERR\n");
	// 		iErr = -3;
	// 	}
    // }
    
    // if(!iErr)
    // {
    //     pSystemSetControl->pFrmBrightness->pFrmStrTitle = GetCurrLanguageText(SYSTEM_LBL_BRIGHTNESS);
    //     pSystemSetControl->pFrmBrightness->pFrmLblTitle = CreateLabel(11, 63, 100, 16, pSystemSetControl->pFrmBrightness->pFrmStrTitle);
    //     SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSystemSetControl->pFrmBrightness->pFrmLblTitle);
    //     pSystemSetControl->pFrmBrightness->pFrmBgMask = CreatePicture(136, 63, 180, 6, BmpFileDirectory"cursor_mask_standby_press.bmp");
	// 	pSystemSetControl->pFrmBrightness->pFrmBtnSelected = CreatePicture(7, 63, 309, 16, BmpFileDirectory"bg_sys_select.bmp");
	// 	pSystemSetControl->pFrmBrightness->pFrmBgProgressBar = CreatePicture(136, 69, 180, 8, BmpFileDirectory"bg_sys_processbar.bmp");

	// 	for (i = 0; i < BRIGHTNESS_LEVEL; ++i)
    //     {
    //         pSystemSetControl->pFrmBrightness->pFrmBtnCursor[i] = CreatePicture(141+i*19, 63, 10, 6, BmpFileDirectory"cursor_standby_press.bmp");
    //         pSystemSetControl->pFrmBrightness->pFrmBtnCursorBar[i] = CreatePicture(136+i*10, 63, 19, 6, NULL);
    //     }
    // }
    
    if(!iErr)
    {
		unsigned int strSystemInfoTitle[SYSTEM_INFO_LIST_NUM] =
        {
            SYSTEM_LBL_LANGUGAGE, SYSTEM_LBL_AUTO_OFF, SYSTEM_LBL_STAND_BY_TIME,
            SYSTEM_LBL_KEY_WARNING, SYSTEM_LBL_USER_GUIDE, SYSTEM_LBL_RESTORE_FACTORY_DEFAULT,
            SYSTEM_LBL_UPDATE, SYSTEM_LBL_SYSTEM_INFO 
        };

		//设置标签信息
		SetLabelContent(pSystemSetControl);

		pSystemSetControl->pFrmStatusIcon[0] = CreatePicture(490, 89 + 0 * 45, 32, 16, NULL);

		for(i = 0; i < SYSTEM_INFO_LIST_NUM; ++i)
        {
			pSystemSetControl->pFrmBtnIcon[i] = CreatePicture(45, 87 + i * 45, 20, 20, pBtnIconBmp[i]);
			pSystemSetControl->pFrmStrTitle[i] = GetCurrLanguageText(strSystemInfoTitle[i]);
            pSystemSetControl->pFrmLblTitle[i] = CreateLabel(75, 89+i*45, 150, 16, pSystemSetControl->pFrmStrTitle[i]);
            pSystemSetControl->pFrmLblData[i] = CreateLabel(490, 89+i*45, 100, 16, pSystemSetControl->pFrmStrData[i]);
            if(i <= iNum)
			{
				//默认都是关闭状态
				if ( i != 0)
					pSystemSetControl->pFrmStatusIcon[i] = CreatePicture(490, 134 + (i-1) * 45, 32, 16, pSwitchStatusBmp[iSwitchStatus[i]]);
				if (i < iNum)
					pSystemSetControl->pFrmEnterIcon[i] = CreatePicture(580, 89 + i * 45, 16, 16, BmpFileDirectory "bg_sys_arrow.bmp");
			}
			else
			{
				pSystemSetControl->pFrmStatusIcon[i] = CreatePicture(490, 89 + i * 45, 32, 16, NULL);
				pSystemSetControl->pFrmEnterIcon[i] = CreatePicture(580, 89 + i * 45, 16, 16, NULL);
			}

			SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSystemSetControl->pFrmLblTitle[i]);
            SetLabelFont(getGlobalFnt(EN_FONT_BLACK), pSystemSetControl->pFrmLblData[i]);
            pSystemSetControl->pFrmBgSelected[i] = CreatePicture(1, 75+i*45, 637, 44, BmpFileDirectory"bg_sys_unselect.bmp");
        }

        pSystemSetControl->iFocus = iFocus;
        pSystemSetControl->CallBack = CallBack;
        pSystemSetControl->iBreakNum = iNum;
    }

	sSystemSetControl = pSystemSetControl;
    
    return pSystemSetControl;
}

/***
  * 功能：
     	销毁系统信息控件
  * 参数：
  		1.SYSTEM_SET_CONTROL** pSystemSetControl : 指向系统信息控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/
int DestroySystemSetControlResource(SYSTEM_SET_CONTROL** pSystemSetControl)
{
    //错误标志定义
	int iErr = 0;
    int i= 0;

    if(*pSystemSetControl == NULL)
    {
        LOG(LOG_ERROR, "pSystemInfoControl is NULL\n");
		iErr = -2;
    }

    // if(!iErr)
    // {
    //     if((*pSystemSetControl)->pFrmBrightness == NULL)
    //     {
    //         LOG(LOG_ERROR, "pSystemInfoControl->pFrmBrightness is NULL\n");
	// 	    iErr = -3;
    //     }
    // }

    // if(!iErr)
    // {
    //     GuiMemFree((*pSystemSetControl)->pFrmBrightness->pFrmStrTitle);
    //     DestroyLabel(&((*pSystemSetControl)->pFrmBrightness->pFrmLblTitle));
    //     DestroyPicture(&((*pSystemSetControl)->pFrmBrightness->pFrmBgMask));
	// 	DestroyPicture(&((*pSystemSetControl)->pFrmBrightness->pFrmBtnSelected));
	// 	DestroyPicture(&((*pSystemSetControl)->pFrmBrightness->pFrmBgProgressBar));

	// 	for (i = 0; i < BRIGHTNESS_LEVEL; ++i)
	// 	{
	// 		DestroyPicture(&((*pSystemSetControl)->pFrmBrightness->pFrmBtnCursor[i]));
	// 		DestroyPicture(&((*pSystemSetControl)->pFrmBrightness->pFrmBtnCursorBar[i]));
	// 	}
    // }

    if(!iErr)
    {
        
        for(i = 0; i < SYSTEM_INFO_LIST_NUM; ++i)
        {
			GuiMemFree((*pSystemSetControl)->pFrmStrData[i]);
            GuiMemFree((*pSystemSetControl)->pFrmStrTitle[i]);
            DestroyLabel(&((*pSystemSetControl)->pFrmLblTitle[i]));
            DestroyLabel(&((*pSystemSetControl)->pFrmLblData[i]));
			DestroyPicture(&((*pSystemSetControl)->pFrmBtnIcon[i]));
			DestroyPicture(&((*pSystemSetControl)->pFrmStatusIcon[i]));
			DestroyPicture(&((*pSystemSetControl)->pFrmEnterIcon[i]));
            DestroyPicture(&((*pSystemSetControl)->pFrmBgSelected[i]));
        }
    }

    GuiMemFree(*pSystemSetControl);

    return iErr;
}

/***
  * 功能：
     	显示系统信息控件
  * 参数：
  		1.SYSTEM_SET_CONTROL* pSystemSetControl : 指向系统信息控件
  * 返回：
        成功返回0，失败非0
  * 备注：
***/ 
int DisplaySystemSetControlResource(SYSTEM_SET_CONTROL* pSystemSetControl)
{
    //错误标志定义
	int iErr = 0;
    int i = 0;

    if(pSystemSetControl == NULL)
    {
        LOG(LOG_ERROR, "pSystemInfoControl is NULL\n");
		iErr = -2;
    }

    // if(!iErr)
    // {
    //     if(pSystemSetControl->pFrmBrightness == NULL)
    //     {
    //         LOG(LOG_ERROR, "pSystemInfoControl->pFrmBrightness is NULL\n");
	// 	    iErr = -3;
    //     }
    // }

    // if(!iErr)
    // {
	// 	DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBtnSelected);
    //     DisplayLabel(pSystemSetControl->pFrmBrightness->pFrmLblTitle);
    //     DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBgMask);
    //     DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBtnCursor[GetScreenBrightness()]);
	// 	DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBgProgressBar);
	// 	pSystemSetControl->pFrmBrightness->iFocus = GetScreenBrightness();
    // }

    if(!iErr)
	{
		//设置标签信息
		SetLabelContent(pSystemSetControl);

		for(i = 0; i < SYSTEM_INFO_LIST_NUM; ++i)
        {
            if(pSystemSetControl->iFocus == i)
            {
                SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pSystemSetControl->pFrmBgSelected[i]);
				SetPictureBitmap(BmpFileDirectory "bg_sys_arrow_press.bmp", pSystemSetControl->pFrmEnterIcon[i]);
				SetPictureBitmap(pBtnIconBmpf[i], pSystemSetControl->pFrmBtnIcon[i]);
				if (i != 0)
					SetPictureBitmap(pSwitchStatusBmpf[iSwitchStatus[i]], pSystemSetControl->pFrmStatusIcon[i]);
			}
			
            DisplayPicture(pSystemSetControl->pFrmBgSelected[i]);
            DisplayLabel(pSystemSetControl->pFrmLblTitle[i]);
			//只显示语言选项
			if (i == LANGUAGE)
            	DisplayLabel(pSystemSetControl->pFrmLblData[i]);
            DisplayPicture(pSystemSetControl->pFrmEnterIcon[i]);
			DisplayPicture(pSystemSetControl->pFrmBtnIcon[i]);
			DisplayPicture(pSystemSetControl->pFrmStatusIcon[i]);
		}
    }

    return iErr;
}

/***
  * 功能：
     	添加控件到接受消息的控件队列
  * 参数：
  		1.SYSTEM_SET_CONTROL* pSystemSetControl : 指向系统信息控件
  		2.GUIWINDOW *pWnd   : 队列的所属窗体
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有互斥锁的情况下调用
***/ 
int AddSystemSetControlToWindow(SYSTEM_SET_CONTROL* pSystemSetControl, GUIWINDOW* pWnd)
{
    //错误标志定义
    int iErr = 0;
	int i = 0;
    
	if ((NULL == pSystemSetControl) || (NULL == pWnd))
	{
		LOG(LOG_ERROR, "pSystemSetControl is NULL\n");
		iErr = -2;
	}

    if(!iErr)
    {
        // if(pSystemSetControl->pFrmBrightness == NULL)
        // {
        //     LOG(LOG_ERROR, "pSystemSetControl->pFrmBrightness is NULL\n");
		//     iErr = -3;
        // }
    }

    if (!iErr)
    {
        // for(i = 0; i < BRIGHTNESS_LEVEL; ++i)
        // {
        //     AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
        //         pSystemSetControl->pFrmBrightness->pFrmBtnCursorBar[i], pWnd);
        // }

		//AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
		//	pSystemSetControl->pFrmBrightness->pFrmBtnSelected, pWnd);
    }

    if(!iErr)
    {
        AddWindowComp(OBJTYP_GUIWINDOW, sizeof(GUIWINDOW), pWnd, 
                  pWnd);
        
        for (i = 0; i < SYSTEM_INFO_LIST_NUM; ++i)
        {
			//AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
			//	pSystemSetControl->pFrmBgSelected[i], pWnd);
            
            if(i < pSystemSetControl->iBreakNum)
            {                
                AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
                            pSystemSetControl->pFrmEnterIcon[i], pWnd);
            }
			else
			{
				AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE),
					pSystemSetControl->pFrmBgSelected[i], pWnd);
			}
        }
    }
	
	return iErr;
}

/***
  * 功能：
     	注册消息处理函数
  * 参数：
  		1.SYSTEM_SET_CONTROL* pSystemSetControl : 指向系统信息控件
  		2.GUIMESSAGE *pMsg  : 当前的消息队列
  * 返回：
        成功返回0，失败非0
  * 备注：
  		需要在持有消息注册队列互斥锁的情况下调用
***/ 
int LoginSystemSetControlToMsg(SYSTEM_SET_CONTROL* pSystemSetControl, GUIWINDOW* pWnd)
{
    //错误标志定义
    int iErr = 0;
    int i = 0;
	GUIMESSAGE *pMsg = GetCurrMessage();;
	
	if ((NULL == pSystemSetControl) || (NULL == pMsg))
	{
		LOG(LOG_ERROR, "pSystemInfoControl is NULL\n");
		iErr = -2;
	}

    // if(!iErr)
    // {
    //     if(pSystemSetControl->pFrmBrightness == NULL)
    //     {
    //         LOG(LOG_ERROR, "pSystemInfoControl->pFrmBrightness is NULL\n");
	// 	    iErr = -3;
    //     }
    // }

    // if(!iErr)
    // {
    //     for(i = 0; i < BRIGHTNESS_LEVEL; ++i)
    //     {
    //         LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSystemSetControl->pFrmBrightness->pFrmBtnCursorBar[i], 
    //                 WndSystemSetCursorBtn_Down, pSystemSetControl, i, pMsg);
	// 	    LoginMessageReg(GUIMESSAGE_TCH_UP, pSystemSetControl->pFrmBrightness->pFrmBtnCursorBar[i], 
    //                 WndSystemSetCursorBtn_Up, pSystemSetControl, i, pMsg);
    //     }

	// 	//LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSystemSetControl->pFrmBrightness->pFrmBtnSelected,
	// 	//	WndSystemSetBrightnessBtn_Down, pSystemSetControl, 0, pMsg);
	// 	//LoginMessageReg(GUIMESSAGE_TCH_UP, pSystemSetControl->pFrmBrightness->pFrmBtnSelected,
	// 	//	WndSystemSetBrightnessBtn_Up, pSystemSetControl, 0, pMsg);
    // }
    
    if(!iErr)
    {
        //注册按钮消息处理
    	for (i = 0; i < SYSTEM_INFO_LIST_NUM; ++i)
    	{
            if(i < pSystemSetControl->iBreakNum)
            {                
                LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSystemSetControl->pFrmEnterIcon[i],
                                WndSystemSetBtnIcon_Down, pSystemSetControl, i, pMsg);
                LoginMessageReg(GUIMESSAGE_TCH_UP, pSystemSetControl->pFrmEnterIcon[i],
                                WndSystemSetBtnIcon_Up, pSystemSetControl, i, pMsg);
            }
			else
			{
	     		LoginMessageReg(GUIMESSAGE_TCH_DOWN, pSystemSetControl->pFrmBgSelected[i],
	                     		WndSystemSetBtnSelected_Down, pSystemSetControl, i, pMsg);
	     		LoginMessageReg(GUIMESSAGE_TCH_UP, pSystemSetControl->pFrmBgSelected[i],
	                     		WndSystemSetBtnSelected_Up, pSystemSetControl, i, pMsg);
			}
    	}
        
        LoginMessageReg(GUIMESSAGE_KEY_DOWN, pWnd, 
	                        WndSystemSetKey_Down, pSystemSetControl, 0, GetCurrMessage());
	    LoginMessageReg(GUIMESSAGE_KEY_UP, pWnd, 
	                        WndSystemSetKey_Up, pSystemSetControl, 0, GetCurrMessage());
    }

	return iErr;
}

//点击亮度光标响应处理
// static int WndSystemSetCursorBtn_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
// {
// #ifdef EEPROM_DATA
//     int iSelected = iOutLen;

//     SetBrightness((iSelected+1)*10,(iSelected+1)*10);
//     SaveScreenBrightness(iSelected);
// 	// 保存系统设置参数
//     SaveSystemSet(0, 0);
// #endif
//     return 0;
// }

// static int WndSystemSetCursorBtn_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
// {
//     // int iSelected = iOutLen;
//     SYSTEM_SET_CONTROL *pSystemSetControl = (SYSTEM_SET_CONTROL *)pOutArg;
// 	// pSystemSetControl->pFrmBrightness->iFocus = iSelected;
// 	FlushCurrentWindow(pSystemSetControl, -1);
    
//     RefreshScreen(__FILE__, __func__, __LINE__);
    
//     return 0;
// }

////亮度按钮按下响应处理
//int WndSystemSetBrightnessBtn_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
//{
//	return 0;
//}
//
//int WndSystemSetBrightnessBtn_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
//{
//	SYSTEM_SET_CONTROL *pSystemSetControl = (SYSTEM_SET_CONTROL *)pOutArg;
//	FlushCurrentWindow(pSystemSetControl, -1);
//	RefreshScreen(__FILE__, __func__, __LINE__);
//
//	return 0;
//}

//修改系统设置数据的内容
static void SetLabelContent(SYSTEM_SET_CONTROL* pSystemSetControl)
{
    //语言
	int count = GetLanguageCount();
	int* pStrLanguage = GetSelectedLanguageList();
	int* pLanguageIndex = GetSelectedLanguageListIndex();

	if (pStrLanguage)
	{
		int i;
		for (i = 0; i < count; ++i)
		{
			if (pLanguageIndex[i] == GetCurrLanguage())
			{
				pSystemSetControl->pFrmStrData[LANGUAGE] = GetCurrLanguageText(pStrLanguage[i]);
				break;
			}
		}

		GuiMemFree(pStrLanguage);
	}
	else
	{
		pSystemSetControl->pFrmStrData[LANGUAGE] = TransString("English");
	}
    //自动关机
    char temp[10] = {0};
    int autoOffTime = GetPowerOffTime();

    if(autoOffTime == 0)
    {
        sprintf(temp, "Off");
		iSwitchStatus[AUTO_OFF] = 0;//设置开关状态
	}
    else
    {
        sprintf(temp, "%d m", autoOffTime/60);
		iSwitchStatus[AUTO_OFF] = 1;
	}

    pSystemSetControl->pFrmStrData[AUTO_OFF] = TransString(temp);
    //待机时间
    memset(temp, 0, 10);
    int screenOffTime = GetScreenOffTime();

    if(screenOffTime == 0)
    {
        sprintf(temp, "Off");
		iSwitchStatus[STANDBY_TIME] = 0;
	}
    else if(screenOffTime < 60)
    {
        sprintf(temp, "%d s", screenOffTime);
		iSwitchStatus[STANDBY_TIME] = 1;
	}
    else
    {
        sprintf(temp, "%d m", screenOffTime/60);
		iSwitchStatus[STANDBY_TIME] = 1;
	}

    pSystemSetControl->pFrmStrData[STANDBY_TIME] = TransString(temp);
	//按键音使能
	memset(temp, 0, 10);
	int iEnable = GetWarningSpeakerEnable();
	iEnable ? sprintf(temp, "On") : sprintf(temp, "Off");
	iSwitchStatus[KEY_WARNING] = iEnable ? 1 : 0;
	pSystemSetControl->pFrmStrData[KEY_WARNING] = TransString(temp);

	SetLabelText(pSystemSetControl->pFrmStrData[LANGUAGE], pSystemSetControl->pFrmLblData[LANGUAGE]);
    SetLabelText(pSystemSetControl->pFrmStrData[AUTO_OFF], pSystemSetControl->pFrmLblData[AUTO_OFF]);
    SetLabelText(pSystemSetControl->pFrmStrData[STANDBY_TIME], pSystemSetControl->pFrmLblData[STANDBY_TIME]);
	SetLabelText(pSystemSetControl->pFrmStrData[KEY_WARNING], pSystemSetControl->pFrmLblData[KEY_WARNING]);
}

//按钮按下处理
static int WndSystemSetBtnIcon_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

//按钮弹起处理
static int WndSystemSetBtnIcon_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iSelected = iOutLen;
	SYSTEM_SET_CONTROL *pSystemSetControl = (SYSTEM_SET_CONTROL *)pOutArg;

	if (iSelected < SYSTEM_INFO_LIST_NUM)
	{
		if (pSystemSetControl->CallBack)
		{
			(*(pSystemSetControl->CallBack))(iSelected);
		}
	}

	return 0;
}

//选择框选中处理
int WndSystemSetBtnSelected_Down(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{

	return 0;
}

int WndSystemSetBtnSelected_Up(void * pInArg, int iInLen, void * pOutArg, int iOutLen)
{
	int iSelected = iOutLen;
	SYSTEM_SET_CONTROL *pSystemSetControl = (SYSTEM_SET_CONTROL *)pOutArg;
	FlushCurrentWindow(pSystemSetControl, iSelected);

	if (iSelected != -1)
	{
		pSystemSetControl->iFocus = iSelected;
	}

	if (iSelected >= pSystemSetControl->iBreakNum)
	{
		WndSystemSetBtnIcon_Up(NULL, 0, pSystemSetControl, iSelected);
	}

	RefreshScreen(__FILE__, __func__, __LINE__);

	return 0;
}

//按键按下处理
static int WndSystemSetKey_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	//LOG(LOG_ERROR, "WndSystemSetKey_Down pInArg = %d\n", (unsigned int)pInArg);
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;

	switch (uiValue)
	{
		case KEYCODE_UP:
			LOG(LOG_INFO, "---up------\n");
			break;
		case KEYCODE_DOWN:
			LOG(LOG_INFO, "---down------\n");
			break;
		case KEYCODE_ENTER:
			LOG(LOG_INFO, "---enter------\n");
			break;
		case KEYCODE_HOME:
			//测试up
			LOG(LOG_INFO, "---up------\n");
			break;
		case KEYCODE_START:
			LOG(LOG_INFO, "---down------\n");
			//测试down
			break;
		case KEYCODE_ESC:
			//测试enter
			break;
		default:
			break;
	}

	return iReturn;
}

//按键弹起处理
static int WndSystemSetKey_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iReturn = 0;
	//LOG(LOG_ERROR, "WndSystemSetKey_Up pInArg = %d\n", (unsigned int)pInArg);
    unsigned int uiValue;
    uiValue = (unsigned int)pInArg;
	SYSTEM_SET_CONTROL *pSystemSetControl = (SYSTEM_SET_CONTROL *)pOutArg;

	//进入工厂菜单的方式（otdr left 3 times+otdr right 3 times+shift）
	static int iOtdrLeftFlag = 0;
	static int iOtdrRightFlag = 0;

	switch (uiValue)
	{
	case KEYCODE_SHIFT:
		if (iOtdrLeftFlag == 3 && iOtdrRightFlag == 3)
		{
			LOG(LOG_INFO, "-enter factory model-----\n");
			iOtdrLeftFlag = 0;
			iOtdrRightFlag = 0;
			GUIWINDOW *pWnd = CreateWindow(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
										   InputPasswardInit, InputPasswardExit,
										   InputPasswardPaint, InputPaawardLoop,
										   InputPasswardPause, InputPasswardResume,
										   NULL);
			SendWndMsg_WindowExit(GetCurrWindow());
			SendSysMsg_ThreadCreate(pWnd);
		}
		break;
	case KEYCODE_OTDR_LEFT:
		if (iOtdrLeftFlag < 3)
		{
			iOtdrLeftFlag++;
		}
		else
		{
			iOtdrLeftFlag = 0;
		}
		break;
	case KEYCODE_OTDR_RIGHT:
		if (iOtdrLeftFlag == 3)
		{
			if (iOtdrRightFlag < 3)
			{
				iOtdrRightFlag++;
			}
			else
			{
				iOtdrRightFlag = 0;
			}
		}
		break;
	case KEYCODE_UP:
		iOtdrLeftFlag = 0;
		iOtdrRightFlag = 0;
		GetCurWindowFocus(pSystemSetControl, 0);
		FlushCurrentWindow(pSystemSetControl, pSystemSetControl->iFocus);
		break;
	case KEYCODE_DOWN:
		iOtdrLeftFlag = 0;
		iOtdrRightFlag = 0;
		GetCurWindowFocus(pSystemSetControl, 1);
		FlushCurrentWindow(pSystemSetControl, pSystemSetControl->iFocus);
		break;
	case KEYCODE_LEFT:
		iOtdrLeftFlag = 0;
		iOtdrRightFlag = 0;
		// if (pSystemSetControl->iFocus == -1)
		// {
		// 	GetCurWindowFocus(pSystemSetControl, 2);
		// 	FlushCurrentWindow(pSystemSetControl, -1);
		// 	WndSystemSetCursorBtn_Down(NULL, 0, NULL, pSystemSetControl->pFrmBrightness->iFocus);
		// }
		break;
	case KEYCODE_RIGHT:
		iOtdrLeftFlag = 0;
		iOtdrRightFlag = 0;
		// if (pSystemSetControl->iFocus == -1)
		// {
		// 	GetCurWindowFocus(pSystemSetControl, 3);
		// 	FlushCurrentWindow(pSystemSetControl, -1);
		// 	WndSystemSetCursorBtn_Down(NULL, 0, NULL, pSystemSetControl->pFrmBrightness->iFocus);
		// }
		break;
	case KEYCODE_ENTER:
		iOtdrLeftFlag = 0;
		iOtdrRightFlag = 0;
		if (pSystemSetControl->iFocus != -1)
		{
			if (pSystemSetControl->iFocus < pSystemSetControl->iBreakNum)
			{
				WndSystemSetBtnIcon_Up(NULL, 0, pSystemSetControl, pSystemSetControl->iFocus);
			}
			else
			{
				WndSystemSetBtnSelected_Up(NULL, 0, pSystemSetControl, pSystemSetControl->iFocus);
			}
		}
		break;
	case KEYCODE_VFL:
		JumpVflWindow();
		break;
	case KEYCODE_ESC:
		iOtdrLeftFlag = 0;
		iOtdrRightFlag = 0;
		ReturnMenuOK();
		break;
	default:
		break;
	}

	RefreshScreen(__FILE__, __func__, __LINE__);

	return iReturn;
}

//刷新当前窗体
static void FlushCurrentWindow(SYSTEM_SET_CONTROL *pSystemSetControl, int iSelected)
{
	int i;
	//将所有选项置为未选项
	for (i = 0; i < SYSTEM_INFO_LIST_NUM; ++i)
	{
		SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pSystemSetControl->pFrmBgSelected[i]);
		SetPictureBitmap(pBtnIconBmp[i], pSystemSetControl->pFrmBtnIcon[i]);
		if (i <= pSystemSetControl->iBreakNum)
		{
			if (i < pSystemSetControl->iBreakNum)
				SetPictureBitmap(BmpFileDirectory "bg_sys_arrow.bmp", pSystemSetControl->pFrmEnterIcon[i]);
			if (i != 0)
				SetPictureBitmap(pSwitchStatusBmp[iSwitchStatus[i]], pSystemSetControl->pFrmStatusIcon[i]);
		}
	}
	// SetPictureBitmap(BmpFileDirectory"bg_sys_unselect.bmp", pSystemSetControl->pFrmBrightness->pFrmBtnSelected);
	// SetPictureBitmap(BmpFileDirectory"cursor_mask_standby.bmp", pSystemSetControl->pFrmBrightness->pFrmBgMask);
	// SetPictureBitmap(BmpFileDirectory"cursor_standby.bmp", pSystemSetControl->pFrmBrightness->pFrmBtnCursor[pSystemSetControl->pFrmBrightness->iFocus]);
	//设置选中项
	if (iSelected == -1)
	{
		// SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pSystemSetControl->pFrmBrightness->pFrmBtnSelected);
		// SetPictureBitmap(BmpFileDirectory"cursor_mask_standby_press.bmp", pSystemSetControl->pFrmBrightness->pFrmBgMask);
		// SetPictureBitmap(BmpFileDirectory"cursor_standby_press.bmp", pSystemSetControl->pFrmBrightness->pFrmBtnCursor[pSystemSetControl->pFrmBrightness->iFocus]);
	}
	else
	{
		SetPictureBitmap(pBtnIconBmpf[iSelected], pSystemSetControl->pFrmBtnIcon[iSelected]);
		SetPictureBitmap(BmpFileDirectory"bg_sys_select.bmp", pSystemSetControl->pFrmBgSelected[iSelected]);
		if (iSelected <= pSystemSetControl->iBreakNum)
		{
			if (iSelected < pSystemSetControl->iBreakNum)
				SetPictureBitmap(BmpFileDirectory "bg_sys_arrow_press.bmp", pSystemSetControl->pFrmEnterIcon[iSelected]);
			if (iSelected != 0)
				SetPictureBitmap(pSwitchStatusBmpf[iSwitchStatus[iSelected]], pSystemSetControl->pFrmStatusIcon[iSelected]);
		}
	}
	//显示所有选项内容
	// DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBtnSelected);
	// DisplayLabel(pSystemSetControl->pFrmBrightness->pFrmLblTitle);
	// DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBgMask);
	// DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBgProgressBar);
	// DisplayPicture(pSystemSetControl->pFrmBrightness->pFrmBtnCursor[pSystemSetControl->pFrmBrightness->iFocus]);
	for (i = 0; i < SYSTEM_INFO_LIST_NUM; ++i)
	{
		DisplayPicture(pSystemSetControl->pFrmBgSelected[i]);
		DisplayLabel(pSystemSetControl->pFrmLblTitle[i]);
		// //只显示语言选项
		if (i == LANGUAGE)
			DisplayLabel(pSystemSetControl->pFrmLblData[i]);
		DisplayPicture(pSystemSetControl->pFrmEnterIcon[i]);
		DisplayPicture(pSystemSetControl->pFrmBtnIcon[i]);
		DisplayPicture(pSystemSetControl->pFrmStatusIcon[i]);
	}
}

//获取当前窗体焦点位置（物理按键作用下）
static void GetCurWindowFocus(SYSTEM_SET_CONTROL *pSystemSetControl, int option)
{
	switch (option)
	{
	case 0://up
		if (pSystemSetControl->iFocus != 0)
		{
			pSystemSetControl->iFocus -= 1;
		}
		break;
	case 1://down
		if (pSystemSetControl->iFocus != (SYSTEM_INFO_LIST_NUM-1))
		{
			pSystemSetControl->iFocus += 1;
		}
		break;
	case 2://left
		// if ((pSystemSetControl->iFocus == -1) && (pSystemSetControl->pFrmBrightness->iFocus != 0))
		// {
		// 	pSystemSetControl->pFrmBrightness->iFocus -= 1;
		// }
		break;
	case 3://right
		// if ((pSystemSetControl->iFocus == -1) && (pSystemSetControl->pFrmBrightness->iFocus != (BRIGHTNESS_LEVEL-1)))
		// {
		// 	pSystemSetControl->pFrmBrightness->iFocus += 1;
		// }
		break;
	default:
		break;
	}
}