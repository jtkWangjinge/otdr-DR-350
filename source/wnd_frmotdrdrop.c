/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  wnd_frmotdrdrop.c
* 摘    要：  otdr界面伸展模式下的参数设置控件
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  2015-5-25
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "wnd_frmotdrdrop.h"
#include "wnd_global.h"
#include "wnd_droppicker.h"
#include "app_frmotdr.h"
#include "app_curve.h"
#include "guiphoto.h"
#include "app_unitconverter.h"
/*******************************************************************************
***                 定义wnd_frmotdrdrop.h引用到系统结构体                    ***
*******************************************************************************/
extern POTDR_TOP_SETTINGS pOtdrTopSettings;

/*******************************************************************************
***                  定义wnd_frmotdrdrop.h静态全局变量                      ***
*******************************************************************************/
//每个下拉列表创建时传的数组
static int iDistEnable[] = {1,1,1,1,1,1,1,1,1,1,1};
static int iPulseEnable[] = {1,1,1,1,1,1,1,1,1,1,1,1};
static int iTimeEnable[] = {1,1,1,1,1,1,1,1,1};
//本地副本控件
static OTDRDROP * LocalOtdrDrop = NULL;

static char *StrWaveLenght[] = {
"1310 nm",
"1550 nm",
};
static char *StrDistKm[] = {
"AUTO",
"1.3 km",
"2.5 km",
"5 km",
"10 km",
"20 km",
"40 km",
"80 km",
"120 km",
"160 km",
"260 km",
};

static char *StrDistMi[] = {
"AUTO",
"0.81 mi",
"1.55 mi",
"3.11 mi",
"6.22 mi",
"12.4 mi",
"24.8 mi",
"49.7 mi",
"74.6 mi",
"99.4 mi",
"161.6 mi",
};

static char *StrDistKft[] = {
"AUTO",
"4.26 kft",
"8.2  kft",
"16.4 kft",
"32.8 kft",
"65.6 kft",
"131.2 kft",
"262.4 kft",
"393.7 kft",
"524.9 kft",
"852.8 kft",
};

static char *StrPulse[] = {
"AUTO",
"5 ns",
"10 ns",
"20 ns",
#ifdef MINI2
"30 ns",
#endif
"50 ns",
"100 ns",
"200 ns",
"500 ns",
"1 us",
"2 us",
"10 us",
"20 us",
};

static char *StrTime[] = {
"AUTO",
"5 s",
"15 s",
"30 s",
"60 s",
"90 s",
"120 s",
"180 s",
"RT",
};

/*******************************************************************************
***                    定义wnd_frmotdrdrop.h内部函数                         ***
*******************************************************************************/
static int OtdrDropWaveSelect_Up(void *pInArg, int iInLen,
									void *pOutArg, int iOutLen);
									
static int OtdrDropWave_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int OtdrDropWave_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int OtdrDropDist_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int OtdrDropPulse_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

static int OtdrDropTime_Up(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);
static int OtdrDropNull_Down(void *pInArg, int iInLen, 
                     		void *pOutArg, int iOutLen);

//获得系统参数到当前控件
static int GetParaFromSystem(OTDRDROP *pOtdrDrop);
//保存当前参数到系统参数
static int SaveParaToSystem(OTDRDROP *pOtdrDrop);
//设置当前的曲线到系统
static int SetSystemCurWave(OTDRDROP *pOtdrDrop);
//刷新更换距离，脉宽和时间
static int SetAllDrop(OTDRDROP *pOtdrDrop);
//刷新左边控件的波长区域
static int SetLeftWave(OTDRDROP *pOtdrDrop);
//距离对脉宽的约束条件
static int Drop1Drop2(OTDRDROP *pOtdrDrop);
//设置三个drop的显示选项
static int SetDropFocus(OTDRDROP *pOtdrDrop,int Drop1,int Drop2,int Drop3);


//创建距离上拉列表时的回调函数
static void OtdrDropDist(int iSelected)
{
	int iDiff = 0;
	int Pusle_Focus = LocalOtdrDrop->iPulse;
    
	if(LocalOtdrDrop->iDist != iSelected)
	{
		iDiff = 1;
	}
    
	LocalOtdrDrop->iDist = iSelected;
    
	if(iSelected == 0)
	{
		SetDropFocus(LocalOtdrDrop, 0, 0, 0);
	}
	else
	{
		if(LocalOtdrDrop->pOtdrDropStrDistVal != NULL)
		{
			free(LocalOtdrDrop->pOtdrDropStrDistVal);
			if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_MI || 
			    pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_YD)
			{
			    LocalOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistMi[LocalOtdrDrop->iDist]);
			}
			else if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_M || 
			    pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_KM)
			{
                LocalOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistKm[LocalOtdrDrop->iDist]);
			}
            else
            {
                LocalOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistKft[LocalOtdrDrop->iDist]);
            }
		}
		SetLabelText(LocalOtdrDrop->pOtdrDropStrDistVal, LocalOtdrDrop->pOtdrDropLblDistVal);		
	}
	Drop1Drop2(LocalOtdrDrop);
	SaveParaToSystem(LocalOtdrDrop);
	SetAllDrop(LocalOtdrDrop);

	DisplayAllDrop(LocalOtdrDrop);

	if(iDiff == 0)
	{
		if(Pusle_Focus != LocalOtdrDrop->iPulse)
			iDiff = 1;
		else
			iDiff = 0;
	}

	if (LocalOtdrDrop->BackCall != NULL)
		LocalOtdrDrop->BackCall(iDiff);

	SetCurveThreadMode(EN_NORMAL_MODE);
	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
}
//创建脉宽上拉列表时的回调函数
static void OtdrDropPulse(int iSelected)
{
	int iDiff = 0;
	if(LocalOtdrDrop->iPulse != iSelected)
	{
		iDiff = 1;
	}
	LocalOtdrDrop->iPulse = iSelected;
	if(iSelected == 0)
	{
		SetDropFocus(LocalOtdrDrop, 0, 0, 0);
	}
	else
	{
		if(LocalOtdrDrop->pOtdrDropStrPulseVal != NULL)
		{
			free(LocalOtdrDrop->pOtdrDropStrPulseVal);
			LocalOtdrDrop->pOtdrDropStrPulseVal = TransString(StrPulse[LocalOtdrDrop->iPulse]);
		}
		SetLabelText(LocalOtdrDrop->pOtdrDropStrPulseVal, LocalOtdrDrop->pOtdrDropLblPulseVal);		
	}
	SaveParaToSystem(LocalOtdrDrop);
	SetAllDrop(LocalOtdrDrop);
	DisplayAllDrop(LocalOtdrDrop);

	if(LocalOtdrDrop->BackCall != NULL)
		LocalOtdrDrop->BackCall(iDiff);

	SetCurveThreadMode(EN_NORMAL_MODE);
	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
}
//创建平均时间上拉列表时的回调函数
static void OtdrDropTime(int iSelected)
{
	int iDiff = 0;
	if(LocalOtdrDrop->iTime != iSelected)
	{
	    if(iSelected == 0 || iSelected == 8 
	        || LocalOtdrDrop->iTime == 0 || LocalOtdrDrop->iTime == 8)
	    {
            iDiff = 1;
	    }
	    else
	    {
            iDiff = 2;
	    }
	}
	LocalOtdrDrop->iTime = iSelected;
	if(iSelected == 0)
	{
		SetDropFocus(LocalOtdrDrop, 0, 0, 0);
	}
	else
	{
		if(LocalOtdrDrop->pOtdrDropStrTimeVal != NULL)
		{
			free(LocalOtdrDrop->pOtdrDropStrTimeVal);
			LocalOtdrDrop->pOtdrDropStrTimeVal = TransString(StrTime[LocalOtdrDrop->iTime]);
		}
		SetLabelText(LocalOtdrDrop->pOtdrDropStrTimeVal, LocalOtdrDrop->pOtdrDropLblTimeVal);
	}
	SaveParaToSystem(LocalOtdrDrop);
	SetAllDrop(LocalOtdrDrop);
	DisplayAllDrop(LocalOtdrDrop);

	if(LocalOtdrDrop->BackCall != NULL)
		LocalOtdrDrop->BackCall(iDiff);

	SetCurveThreadMode(EN_NORMAL_MODE);
	SendCurveAdditionCmd(EN_ADD_REFSH, 0);
}


/***
  * 功能：
     	创建一个otdrdrop控件
  * 参数：
  		无
  * 返回：
        成功返回有效指针，失败NULL
  * 备注：
***/ 
OTDRDROP * CreateOtdrDrop(OTDRDROPBACKFUNC BackCall)
{
	int iErr = 0;
	OTDRDROP *pOtdrDrop = NULL;

	if (!iErr)
	{	//分配资源
		pOtdrDrop = (OTDRDROP *)calloc(1, sizeof(OTDRDROP));
		if (NULL == pOtdrDrop)
		{
			iErr = -1;
		}
	}

	if(!iErr)
	{
		GetParaFromSystem(pOtdrDrop);
        pOtdrDrop->pOtdrDropBg = CreatePhoto("otdr_drop_bg");
        pOtdrDrop->pOtdrDropWave = CreatePhoto("otdr_drop_wave_unpress");
		
		if(pOtdrDrop->iCurWave == 1)
		{
			if(pOtdrDrop->iWave[1] == 1)
			{
                pOtdrDrop->pOtdrDropWavePoint = CreatePhoto("otdr_drop_point_press");
			}
			else
			{
                pOtdrDrop->pOtdrDropWavePoint = CreatePhoto("otdr_drop_point_unpress");
			}
			pOtdrDrop->pOtdrDropStrWaveVal = TransString(StrWaveLenght[1]);
		}
		else
		{
			if(pOtdrDrop->iWave[0] == 1)
			{
                pOtdrDrop->pOtdrDropWavePoint = CreatePhoto("otdr_drop_point_press");
			}
			else
			{
                pOtdrDrop->pOtdrDropWavePoint = CreatePhoto("otdr_drop_point_unpress");
			}
			pOtdrDrop->pOtdrDropStrWaveVal = TransString(StrWaveLenght[0]);

		}
        
        pOtdrDrop->pOtdrDropDist = CreatePhoto("otdr_drop_list");
        pOtdrDrop->pOtdrDropPulse = CreatePhoto("otdr_drop_list1");
        pOtdrDrop->pOtdrDropTime = CreatePhoto("otdr_drop_list2");
        pOtdrDrop->pOtdrDropAllDrop = CreatePhoto("otdr_all_drop");

        pOtdrDrop->pOtdrDropWavePointTouch = CreatePicture(0, 420, 42, 60, NULL);
        pOtdrDrop->pOtdrDropWaveTouch = CreatePicture(42, 420, 124, 60, NULL);
        pOtdrDrop->pOtdrDropDistTouch = CreatePicture(168, 420, 170, 60, NULL);
        pOtdrDrop->pOtdrDropPulseTouch = CreatePicture(339, 420, 170, 60, NULL);
        pOtdrDrop->pOtdrDropTimeTouch = CreatePicture(509, 420, 170, 60, NULL);
		
		pOtdrDrop->pOtdrDropStrWave = TransString("OTDR_LBL_WAVELENGTH");
		pOtdrDrop->pOtdrDropStrPulse = TransString("OTDR_LBL_PULSE");
		pOtdrDrop->pOtdrDropStrTime = TransString("OTDR_LBL_TIME");

		if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_MI || 
			    pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_YD)
		{
		    pOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistMi[pOtdrDrop->iDist]);
		    pOtdrDrop->pOtdrDropStrDist = TransString("OTDR_FULL_LBL_DISTANCE");
		}
		else if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_M || 
			    pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_KM)
		{
		    pOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistKm[pOtdrDrop->iDist]);
		    pOtdrDrop->pOtdrDropStrDist = GetCurrLanguageText(OTDR_LBL_DISTANCE_M);
		}
		else
        {
            pOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistKft[pOtdrDrop->iDist]);
		    pOtdrDrop->pOtdrDropStrDist = TransString("Distance(kft)");
        }
        
		pOtdrDrop->pOtdrDropStrPulseVal = TransString(StrPulse[pOtdrDrop->iPulse]);
		pOtdrDrop->pOtdrDropStrTimeVal = TransString(StrTime[pOtdrDrop->iTime]);
		
		pOtdrDrop->pOtdrDropLblWave = CreateLabel(0, 407, 167, 24, pOtdrDrop->pOtdrDropStrWave);
		pOtdrDrop->pOtdrDropLblDist = CreateLabel(167, 407, 171, 24, pOtdrDrop->pOtdrDropStrDist);
		pOtdrDrop->pOtdrDropLblPulse = CreateLabel(338, 407, 169, 24, pOtdrDrop->pOtdrDropStrPulse);
		pOtdrDrop->pOtdrDropLblTime = CreateLabel(507, 407, 174, 24, pOtdrDrop->pOtdrDropStrTime);
		pOtdrDrop->pOtdrDropLblWaveVal = CreateLabel(52, 447, 82, 24, pOtdrDrop->pOtdrDropStrWaveVal);
		pOtdrDrop->pOtdrDropLblDistVal = CreateLabel(190, 447, 109, 24, pOtdrDrop->pOtdrDropStrDistVal);
		pOtdrDrop->pOtdrDropLblPulseVal = CreateLabel(362, 447, 109, 24, pOtdrDrop->pOtdrDropStrPulseVal);
		pOtdrDrop->pOtdrDropLblTimeVal = CreateLabel(533, 447, 109, 24, pOtdrDrop->pOtdrDropStrTimeVal);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblWave);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblDist);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblPulse);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblTime);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblWaveVal);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblDistVal);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblPulseVal);
		SetLabelAlign(2, pOtdrDrop->pOtdrDropLblTimeVal);
        SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblWave);
        SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblDist);
        SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblPulse);
        SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblTime);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblWaveVal);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblDistVal);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblPulseVal);
		SetLabelFont(getGlobalFnt(EN_FONT_WHITE), pOtdrDrop->pOtdrDropLblTimeVal);
	}
	pOtdrDrop->iWaveEnable = 1;
	pOtdrDrop->iDropEnable = 1;
	pOtdrDrop->iEnable = 0;

	if(BackCall != NULL)
		pOtdrDrop->BackCall = BackCall;
	else
		pOtdrDrop->BackCall = NULL;
	
	LocalOtdrDrop = pOtdrDrop;
	
	return pOtdrDrop;	
}		

/***
  * 功能：
     	刷新otdrdrop控件到缓冲区
  * 参数：
  		void *pArg: 控件指针
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int DisplayOtdrDrop(void *pArg)
{
	OTDRDROP *pOtdrDrop = (OTDRDROP *)pArg;
	if(pOtdrDrop == NULL)
		return -1;

	if(0 == pOtdrDrop->iEnable)
		return 0;
	
	DisplayPicture(pOtdrDrop->pOtdrDropBg);
	DisplayPicture(pOtdrDrop->pOtdrDropWavePoint);
	DisplayPicture(pOtdrDrop->pOtdrDropWave);
	DisplayPicture(pOtdrDrop->pOtdrDropDist);
	DisplayPicture(pOtdrDrop->pOtdrDropPulse);
	DisplayPicture(pOtdrDrop->pOtdrDropTime);
	
	DisplayLabel(pOtdrDrop->pOtdrDropLblWave);
	DisplayLabel(pOtdrDrop->pOtdrDropLblDist);
	DisplayLabel(pOtdrDrop->pOtdrDropLblPulse);
	DisplayLabel(pOtdrDrop->pOtdrDropLblTime);

	DisplayLabel(pOtdrDrop->pOtdrDropLblWaveVal);
	DisplayLabel(pOtdrDrop->pOtdrDropLblDistVal);
	DisplayLabel(pOtdrDrop->pOtdrDropLblPulseVal);
	DisplayLabel(pOtdrDrop->pOtdrDropLblTimeVal);

	return 0;
}

/***
  * 功能：
     	注销otdrdrop控件
  * 参数：
  		OTDRDROP **pOtdrDrop: 控件的地址
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int DestroyOtdrDrop(OTDRDROP **pOtdrDrop)
{
	if(pOtdrDrop == NULL)
		return -1;

	GuiMemFree((*pOtdrDrop)->pOtdrDropStrWave);
	GuiMemFree((*pOtdrDrop)->pOtdrDropStrDist);
	GuiMemFree((*pOtdrDrop)->pOtdrDropStrPulse);
	GuiMemFree((*pOtdrDrop)->pOtdrDropStrTime);
	GuiMemFree((*pOtdrDrop)->pOtdrDropStrWaveVal);
	GuiMemFree((*pOtdrDrop)->pOtdrDropStrDistVal);
	GuiMemFree((*pOtdrDrop)->pOtdrDropStrPulseVal);
	GuiMemFree((*pOtdrDrop)->pOtdrDropStrTimeVal);

	DestroyPicture(&((*pOtdrDrop)->pOtdrDropBg));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropWavePoint));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropWave));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropDist));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropPulse));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropTime));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropAllDrop));
	
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropWavePointTouch));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropWaveTouch));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropDistTouch));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropPulseTouch));
	DestroyPicture(&((*pOtdrDrop)->pOtdrDropTimeTouch));
	
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblWave));
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblDist));
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblPulse));
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblTime));
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblWaveVal));
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblDistVal));
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblPulseVal));
	DestroyLabel(&((*pOtdrDrop)->pOtdrDropLblTimeVal));

	free(*pOtdrDrop);
	*pOtdrDrop = NULL;
	
	return 0;
}

//添加控件到事件控件队列中
static int AddOtdrDropToComp(OTDRDROP *pOtdrDrop, GUIWINDOW *pWnd)
{
	GUIMESSAGE *pMsg;

	if ((NULL == pOtdrDrop) || (NULL == pWnd))
	{
		return -1;
	}
	
	//
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrDrop->pOtdrDropWavePointTouch, pWnd);
 	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrDrop->pOtdrDropWaveTouch, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrDrop->pOtdrDropDistTouch, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrDrop->pOtdrDropPulseTouch, pWnd);
	AddWindowComp(OBJTYP_GUIPICTURE, sizeof(GUIPICTURE), 
 				  pOtdrDrop->pOtdrDropTimeTouch, pWnd);


	pMsg = GetCurrMessage();
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropWavePointTouch,
               		OtdrDropNull_Down, pOtdrDrop, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropWaveTouch,
               		OtdrDropWave_Down, pOtdrDrop, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropDistTouch,
               		OtdrDropNull_Down, pWnd, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropPulseTouch,
               		OtdrDropNull_Down, pWnd, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropTimeTouch,
               		OtdrDropNull_Down, pWnd, 4, pMsg);

	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropWavePointTouch,
               		OtdrDropWaveSelect_Up, pOtdrDrop, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropWaveTouch,
               		OtdrDropWave_Up, pOtdrDrop, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropDistTouch,
               		OtdrDropDist_Up, pWnd, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropPulseTouch,
               		OtdrDropPulse_Up, pWnd, 4, pMsg);
	LoginMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropTimeTouch,
               		OtdrDropTime_Up, pWnd, 4, pMsg);

	return 0;
}


//删除控件从接受消息的控件队列中
static int DelOtdrDropFromComp(OTDRDROP *pOtdrDrop, GUIWINDOW *pWnd) 
{
	GUIMESSAGE *pMsg;
	pMsg = GetCurrMessage();
	if ((NULL == pOtdrDrop) || (NULL == pWnd))
	{
		return -1;
	}

	DelWindowComp(pOtdrDrop->pOtdrDropWavePointTouch, pWnd);
	DelWindowComp(pOtdrDrop->pOtdrDropWaveTouch, pWnd);
	DelWindowComp(pOtdrDrop->pOtdrDropDistTouch, pWnd);
	DelWindowComp(pOtdrDrop->pOtdrDropPulseTouch, pWnd);
	DelWindowComp(pOtdrDrop->pOtdrDropTimeTouch, pWnd);

	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropWavePointTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropWaveTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropDistTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropPulseTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_DOWN, pOtdrDrop->pOtdrDropTimeTouch, pMsg);

	LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropWavePointTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropWaveTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropDistTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropPulseTouch, pMsg);
	LogoutMessageReg(GUIMESSAGE_TCH_UP, pOtdrDrop->pOtdrDropTimeTouch, pMsg);

	return 0;
} 


//波长选择弹起函数
static int OtdrDropWaveSelect_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{	
	OTDRDROP *pOtdrDrop = (OTDRDROP *)pOutArg;

	if(pOtdrDrop->iWaveEnable == 1)
	{
		if(pOtdrDrop->iCurWave == 0)
		{
			if(pOtdrDrop->iWave[1] == 1)
			{
				if(pOtdrDrop->iWave[0] == 1)
				{
					pOtdrDrop->iWave[0] = 0;
					SetPictureBitmap(BmpFileDirectory"otdr_drop_point_unpress.bmp", pOtdrDrop->pOtdrDropWavePoint);
				}
				else
				{
					pOtdrDrop->iWave[0] = 1;
					SetPictureBitmap(BmpFileDirectory"otdr_drop_point_press.bmp", pOtdrDrop->pOtdrDropWavePoint);
				}	
			}
		}
		else
		{
			if(pOtdrDrop->iWave[0] == 1)
			{
				if(pOtdrDrop->iWave[1] == 1)
				{
					pOtdrDrop->iWave[1] = 0;
					SetPictureBitmap(BmpFileDirectory"otdr_drop_point_unpress.bmp", pOtdrDrop->pOtdrDropWavePoint);
				}
				else
				{
					pOtdrDrop->iWave[1] = 1;
					SetPictureBitmap(BmpFileDirectory"otdr_drop_point_press.bmp", pOtdrDrop->pOtdrDropWavePoint);
				}	
			}

		}

		SaveParaToSystem(pOtdrDrop);
		DisplayOtdrDrop(pOtdrDrop);

		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}


//脉宽选择弹起函数
static int OtdrDropPulse_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{		
	GUIWINDOW *pWnd = (GUIWINDOW *)pOutArg;

	if(LocalOtdrDrop->iDropEnable == 1)
	{

		SetCurveThreadMode(EN_NOT_DRAW_CURVE_MODE);
		while (EN_STATUS_BUSY == CurrDisplayStatus())
		{
			MsecSleep(5);
		}
		CreateDropPicker(357, 441, 12, StrPulse, iPulseEnable, LocalOtdrDrop->iPulse, OtdrDropPulse, pWnd);
	}
	
	return 0;
}

//时间选择弹起函数
static int OtdrDropTime_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = (GUIWINDOW *)pOutArg;

	if(LocalOtdrDrop->iDropEnable == 1)
	{

		SetCurveThreadMode(EN_NOT_DRAW_CURVE_MODE);

		while (EN_STATUS_BUSY == CurrDisplayStatus())
		{
			MsecSleep(5);
		}
		CreateDropPicker(526, 441, 9, StrTime, iTimeEnable, LocalOtdrDrop->iTime, OtdrDropTime, pWnd);
	}
	return 0;
}


//距离选择弹起函数
static int OtdrDropDist_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	GUIWINDOW *pWnd = (GUIWINDOW *)pOutArg;

	if(LocalOtdrDrop->iDropEnable == 1)
	{

		SetCurveThreadMode(EN_NOT_DRAW_CURVE_MODE);

		while (EN_STATUS_BUSY == CurrDisplayStatus())
		{
			MsecSleep(5);
		}	
		
		if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_MI || 
			    pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_YD)
		{
		    CreateDropPicker(188, 441, 11, StrDistMi, iDistEnable, LocalOtdrDrop->iDist, OtdrDropDist, pWnd);
		}
		else if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_M || 
			    pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_KM)
		{
		    CreateDropPicker(188, 441, 11, StrDistKm, iDistEnable, LocalOtdrDrop->iDist, OtdrDropDist, pWnd);
		}
        else
        {
            CreateDropPicker(188, 441, 11, StrDistKft, iDistEnable, LocalOtdrDrop->iDist, OtdrDropDist, pWnd);
        }

	}
	return 0;
}

//当前波长选择按下函数
static int OtdrDropWave_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	OTDRDROP *pOtdrDrop = (OTDRDROP *)pOutArg;

	if(pOtdrDrop->iWaveEnable == 1)
	{
		TouchChange("otdr_drop_wave_press.bmp", pOtdrDrop->pOtdrDropWave, 
				NULL, pOtdrDrop->pOtdrDropLblWaveVal, 0);
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	return 0;
}
//当前波长选择弹起函数
static int OtdrDropWave_Up(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	OTDRDROP *pOtdrDrop = (OTDRDROP *)pOutArg;

	if(pOtdrDrop->iWaveEnable == 1)
	{
		if(pOtdrDrop->iCurWave == 1)
		{
			pOtdrDrop->iCurWave = 0;
		}
		else
		{
			pOtdrDrop->iCurWave = 1;
		}
		
		SetSystemCurWave(pOtdrDrop);
		GetParaFromSystem(pOtdrDrop);

		SetLeftWave(pOtdrDrop);
		SetAllDrop(pOtdrDrop);
		DisplayOtdrDrop(pOtdrDrop);
		
		RefreshScreen(__FILE__, __func__, __LINE__);
	}
	
	return 0;
}

static int OtdrDropNull_Down(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
	return 0;
}

//设置当前波长到系统参数中
static int SetSystemCurWave(OTDRDROP *pOtdrDrop)
{
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;
	
	if (NULL == pOtdrDrop || pUser_Setting == NULL)
	{
		return -1;
	}
	pUser_Setting->enWaveCurPos = pOtdrDrop->iCurWave;
	return 0;
}

//获得系统参数到控件中
static int GetParaFromSystem(OTDRDROP *pOtdrDrop)
{
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;
	int i = 0;
	if (NULL == pOtdrDrop || pUser_Setting == NULL)
	{
		return -1;
	}

	pOtdrDrop->iCurWave = pUser_Setting->enWaveCurPos;
	pOtdrDrop->iWave[0] = pUser_Setting->iWave[0];
	pOtdrDrop->iWave[1] = pUser_Setting->iWave[1];

	pOtdrDrop->iDist = pUser_Setting->sFrontSetting[pOtdrDrop->iCurWave].enFiberRange;
	pOtdrDrop->iPulse = pUser_Setting->sFrontSetting[pOtdrDrop->iCurWave].enPulseTime;
	pOtdrDrop->iTime = pUser_Setting->sFrontSetting[pOtdrDrop->iCurWave].enAverageTime;
	
    for(i=0;i<3;i++)
    {
        pUser_Setting->sFrontSetting[i].enFiberRange = pOtdrDrop->iDist;
        pUser_Setting->sFrontSetting[i].enPulseTime = pOtdrDrop->iPulse;
        pUser_Setting->sFrontSetting[i].enAverageTime = pOtdrDrop->iTime;
    }

	return 0;
}

//设置控件中参数到系统参数中
static int SaveParaToSystem(OTDRDROP *pOtdrDrop)
{
	PUSER_SETTINGS pUser_Setting = pOtdrTopSettings->pUser_Setting;
	int i = 0;
	if (NULL == pOtdrDrop || pUser_Setting == NULL)
	{
		return -1;
	}
	
	pUser_Setting->iWave[0] = pOtdrDrop->iWave[0];
	pUser_Setting->iWave[1] = pOtdrDrop->iWave[1];
	pUser_Setting->enWaveCurPos = pOtdrDrop->iCurWave;

    for(i=0;i<3;i++){
        pUser_Setting->sFrontSetting[i].enFiberRange = pOtdrDrop->iDist;
        pUser_Setting->sFrontSetting[i].enPulseTime = pOtdrDrop->iPulse;
        pUser_Setting->sFrontSetting[i].enAverageTime = pOtdrDrop->iTime;
    }
    
	return 0;
}

//距离对脉宽的约束条件
static int Drop1Drop2(OTDRDROP *pOtdrDrop)
{
	return 0;
}
//设置距离、脉宽、时间的显示选项
static int SetDropFocus(OTDRDROP *pOtdrDrop,int Drop1,int Drop2,int Drop3)
{
	if(pOtdrDrop == NULL || Drop1 < 0 || Drop1 > 10 || Drop2 < 0 || Drop2 > 11
						 || Drop3 < 0 || Drop3 > 8)
		return 0;

	pOtdrDrop->iDist = Drop1;
	pOtdrDrop->iPulse = Drop2;
	pOtdrDrop->iTime = Drop3;

	return 0;
}

//修改距离、脉宽、时间的显示选项
static int SetAllDrop(OTDRDROP *pOtdrDrop)
{
	
	GuiMemFree(pOtdrDrop->pOtdrDropStrDistVal);
	GuiMemFree(pOtdrDrop->pOtdrDropStrPulseVal);
	GuiMemFree(pOtdrDrop->pOtdrDropStrTimeVal);		
	if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_MI || 
        pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_YD)
	{
	    pOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistMi[pOtdrDrop->iDist]);
	}
	else if(pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_M || 
        pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag == UNIT_KM)
	{
	    pOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistKm[pOtdrDrop->iDist]);
	}
    else
    {
        pOtdrDrop->pOtdrDropStrDistVal = TransString(StrDistKft[pOtdrDrop->iDist]);
    }
	pOtdrDrop->pOtdrDropStrPulseVal = TransString(StrPulse[pOtdrDrop->iPulse]);
	pOtdrDrop->pOtdrDropStrTimeVal = TransString(StrTime[pOtdrDrop->iTime]);
	SetLabelText(pOtdrDrop->pOtdrDropStrDistVal, pOtdrDrop->pOtdrDropLblDistVal);
	SetLabelText(pOtdrDrop->pOtdrDropStrPulseVal, pOtdrDrop->pOtdrDropLblPulseVal);
	SetLabelText(pOtdrDrop->pOtdrDropStrTimeVal, pOtdrDrop->pOtdrDropLblTimeVal);

	return 0;
}

//修改控件左部波长区域的参数
static int SetLeftWave(OTDRDROP *pOtdrDrop)
{	
	
	if(pOtdrDrop->iWave[pOtdrDrop->iCurWave] == 1)
	{
		SetPictureBitmap(BmpFileDirectory"otdr_drop_point_press.bmp", pOtdrDrop->pOtdrDropWavePoint);
	}
	else
	{
		SetPictureBitmap(BmpFileDirectory"otdr_drop_point_unpress.bmp", pOtdrDrop->pOtdrDropWavePoint);
	}
	SetPictureBitmap(BmpFileDirectory"otdr_drop_wave_unpress.bmp", pOtdrDrop->pOtdrDropWave);
	
	GuiMemFree(pOtdrDrop->pOtdrDropStrWaveVal);
	pOtdrDrop->pOtdrDropStrWaveVal = TransString(StrWaveLenght[pOtdrDrop->iCurWave]);
	SetLabelText(pOtdrDrop->pOtdrDropStrWaveVal, pOtdrDrop->pOtdrDropLblWaveVal);
	
	return 0;
}


int DisplayAllDrop(OTDRDROP *pOtdrDrop)
{
	DisplayPicture(pOtdrDrop->pOtdrDropAllDrop);

	DisplayPicture(pOtdrDrop->pOtdrDropDist);
	DisplayPicture(pOtdrDrop->pOtdrDropPulse);
	DisplayPicture(pOtdrDrop->pOtdrDropTime);
	
	DisplayLabel(pOtdrDrop->pOtdrDropLblDist);
	DisplayLabel(pOtdrDrop->pOtdrDropLblPulse);
	DisplayLabel(pOtdrDrop->pOtdrDropLblTime);

	DisplayLabel(pOtdrDrop->pOtdrDropLblDistVal);
	DisplayLabel(pOtdrDrop->pOtdrDropLblPulseVal);
	DisplayLabel(pOtdrDrop->pOtdrDropLblTimeVal);

	return 0;
}

int SetAllDropEnable(OTDRDROP *pOtdrDrop ,int iEnable)
{
	pOtdrDrop->iDropEnable = iEnable;
	return 0;
}
	
/***
  * 功能：
     	使能otdrdrop控件，以便注册和注销事件
  * 参数：
  		1.void *pArg: 控件的地址
  		2.int iEnable: 1--注册，0--注销
  		3.GUIWINDOW *pWnd:当前窗体
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int SetOtdrDropEnable(void *pArg, int iEnable, GUIWINDOW *pWnd)
{
	OTDRDROP *pOtdrDrop = (OTDRDROP *)pArg;
	if ((NULL == pOtdrDrop) || (NULL == pWnd) || (iEnable < 0) || (iEnable > 1))
	{
		return -1;
	}

	if(iEnable == 1)
	{
		AddOtdrDropToComp(pOtdrDrop, pWnd);
	}
	else
	{
		DelOtdrDropFromComp(pOtdrDrop, pWnd);
	}
	
	pOtdrDrop->iEnable = iEnable;
	
	return 0;
}

/***
  * 功能：
     	设置otdrdrop控件中波长的使能
  * 参数：
  		1.OTDRDROP *pOtdrDrop: 控件指针
  		2.int iEnable: 1--使能，0--取消
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int DropSetWaveEnable(OTDRDROP *pOtdrDrop, int iEnable)
{
	pOtdrDrop->iWaveEnable = iEnable;
	
	return 0;
}


/***
  * 功能：
     	设置当前波长的选择
  * 参数：
  		1.OTDRDROP *pOtdrDrop: 控件指针
  		2.int iCurWave: 1--1550nm，0--1310nm
  * 返回：
        成功返回0，失败返回非0
  * 备注：
***/ 
int DropSetWaveSelect(OTDRDROP *pOtdrDrop,int iCurWave)
{
	pOtdrDrop->iCurWave = iCurWave;
	
	GetParaFromSystem(pOtdrDrop);
	
	SetLeftWave(pOtdrDrop);
	SetAllDrop(pOtdrDrop);

	return 0;
}


int RefreshDropTime(int elapsedTime)
{
    int disableItemCount = 0, i = 0;

    if(LocalOtdrDrop->iTime == 0 || LocalOtdrDrop->iTime == 8)
    {
        memset(iTimeEnable, 1, sizeof(iTimeEnable));
        return 0;
    }

    if(elapsedTime >= 180)
    {
        
    }
    else if(elapsedTime >= 120)
    {
        disableItemCount = 6;
    }
    else if(elapsedTime >= 90)
    {
        disableItemCount = 5;
    }
    else if(elapsedTime >= 60)
    {
        disableItemCount = 4;
    }
    else if(elapsedTime >= 30)
    {
        disableItemCount = 3;
    }
    else if(elapsedTime >= 15)
    {
        disableItemCount = 2;
    }
    else if(elapsedTime >= 5)
    {
        disableItemCount = 1;
    }

    for(i = 1; i < sizeof(iTimeEnable)/sizeof(iTimeEnable[0]) - 2; i++)
    {
        if(i <= disableItemCount)
        {
            iTimeEnable[i] = 0;
        }
        else
        {
            iTimeEnable[i] = 1;
        }
    }
    
    return 0;
}
