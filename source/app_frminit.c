/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frminit.c
* 摘    要：  实现用于完成OTDR模块功能的应用程序接口
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：  
*******************************************************************************/

#include "app_frminit.h"

/****************************************
* 为实现app_frmotdr而需要引用的标准头文件
****************************************/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

/****************************************
* 为实现app_frmotdr而需要引用的其他头文件
****************************************/
#include "app_global.h"
#include "guiglobal.h"
#include "drv_fpga.h"
#include "wnd_global.h"
#include "app_frmotdrmessage.h"
#include "app_getsetparameter.h"
#include "app_frmotdr.h"
#include "app_queue.h"
#include "app_frmsolamessager.h"
#include "app_sola.h"
#include "app_frmsourcelight.h"
#include "app_systemsettings.h"
#include "wnd_frmmark.h"
#include "app_unitconverter.h"

POTDR_TOP_SETTINGS pOtdrTopSettings = NULL;	//当前OTDR的工作设置
PUSER_SETTINGS pUser_Settings = NULL;

SOLA_MESSAGER* pSolaMessager = NULL;
SOURCELIGHTCONFIG* pSourceLightConfig = NULL;
LIGHT_SOURCE_POWER_CONFIG_LIST* pLightSourcePowerConfig = NULL;
MarkParam* pMarkParam = NULL; //标识界面参数
static GUITHREAD thdOtdrCtrl;				//OTDR控制线程ID
static GUITHREAD thdDataAcquisition;		//数据采集线程ID
static GUITHREAD thdDataProessing;			//数据处理线程ID
static GUITHREAD thdCurveDrawing;           //画图线程

static Stack *pWndStackInit = NULL;			//窗口栈
static OTDRMSGQUEUE *pOtdrMsg = NULL;		//appotdr消息队列

int CheckFrontSet_1310(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    return 0;
}

void ResetFrontSet_1310(void* buff)
{
    if(!buff)
    {
        return;
    }

    PFRONT_SETTING pFrontSet_1310 = (PFRONT_SETTING)buff;
    pFrontSet_1310->enTestMode = ENUM_RT_MODE;
    pFrontSet_1310->enFiberRange = ENUM_FIBER_RANGE_AUTO;
    pFrontSet_1310->enPulseTime = ENUM_PULSE_AUTO;
    pFrontSet_1310->enAverageTime = ENUM_AVG_TIME_AUTO;
    SaveSettings(FRONTSET_1310);
    return;
}

int CheckFrontSet_1550(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    return 0;
}

void ResetFrontSet_1550(void* buff)
{
    if(!buff)
    {
        return;
    }

    PFRONT_SETTING pFrontSet_1550 = (PFRONT_SETTING)buff;
    pFrontSet_1550->enTestMode = ENUM_RT_MODE;
    pFrontSet_1550->enFiberRange = ENUM_FIBER_RANGE_AUTO;
    pFrontSet_1550->enPulseTime = ENUM_PULSE_AUTO;
    pFrontSet_1550->enAverageTime = ENUM_AVG_TIME_AUTO;
    SaveSettings(FRONTSET_1550);
    return;
}

int CheckFrontSet_1625(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    return 0;
}

void ResetFrontSet_1625(void* buff)
{
    if(!buff)
    {
        return;
    }

    PFRONT_SETTING pFrontSet_1625 = (PFRONT_SETTING)buff;
    pFrontSet_1625->enTestMode = ENUM_RT_MODE;
    pFrontSet_1625->enFiberRange = ENUM_FIBER_RANGE_AUTO;
    pFrontSet_1625->enPulseTime = ENUM_PULSE_AUTO;
    pFrontSet_1625->enAverageTime = ENUM_AVG_TIME_AUTO;
    SaveSettings(FRONTSET_1625);
    return;
}

int CheckCommonSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    PCOMMON_SETTING pCommonSet = (PCOMMON_SETTING)buff;

    if(pCommonSet->iLightCheckFlag > 1
        || pCommonSet->iConectCheckFlag > 1
        || pCommonSet->iAutoSaveFlag > 1
        || pCommonSet->iMacroBendingFlag > 1
        || pCommonSet->iAutoSpanFlag > 1
        || pCommonSet->iPromptSaveFlag > 1
        || pCommonSet->iDisplaySectionEvent > 1
        || pCommonSet->iOTDRFileNameFlag > 1
        || pCommonSet->iPreviewAreaFlag > 1
        || pCommonSet->iScreenShotFlag > 1
        || pCommonSet->iCusorLblFlag > 1
        || pCommonSet->iLossMethodMark > 1
        || pCommonSet->iAtenuMethodMark > 1
        || pCommonSet->iHotkeyKey1 > 4
        || pCommonSet->iHotkeyKey2 > 4
        || pCommonSet->iAutoJumpWindow > 1
        || pCommonSet->iOnlyShowCurrCurve > 1
        || pCommonSet->iUnitConverterFlag > 4)//m/km,ft/kft,mile
     {
            return -1;
     }
     
    return 0;
}

void ResetCommonSet(void* buff)
{
    if(!buff)
    {
        return;
    }

    PCOMMON_SETTING pCommonSet = (PCOMMON_SETTING)buff;
    
    pCommonSet->iAutoSaveFlag = 1; 	                    //自动保存 0
	pCommonSet->iAutoSpanFlag = 1;	                    //自动放大 0
	pCommonSet->iPromptSaveFlag = 1;	                //提示保存 0
	pCommonSet->iDisplaySectionEvent = 1;	            //显示区段事件 0
	pCommonSet->iConectCheckFlag = 0;	                //链接检查 0
	pCommonSet->iLightCheckFlag = 0;	                //有光检查 0
	pCommonSet->iMacroBendingFlag = 0;                  //宏弯曲 0
	pCommonSet->iLossMethodMark = 0;	                //损耗近似方法 LSA
	pCommonSet->iAtenuMethodMark = 0;	                //衰减近似方法 LSA
	pCommonSet->iOTDRFileNameFlag = 1;
	pCommonSet->iCusorLblFlag = 1;
	pCommonSet->iPreviewAreaFlag = 1;
	pCommonSet->iScreenShotFlag = 0;                    //添加全局截图功能后不在显示截图按钮
    pCommonSet->iAutoJumpWindow = 1;
    pCommonSet->iOnlyShowCurrCurve = 0;
    pCommonSet->iUnitConverterFlag = 0;                 //默认单位是m/km
    SaveSettings(COMMON_SET);

    return;
}

int CheckSampleSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    PSAMPLE_SETTING pCommonSet = (PSAMPLE_SETTING)buff;
    
    if(pCommonSet->enAutoAvrTime == ENUM_AVG_TIME_AUTO)
    {
        return -1;
    }
	
    return 0;
}

void ResetSampleSet(void* buff)
{
    if(!buff)
    {
        return;
    }

    PSAMPLE_SETTING pCommonSet = (PSAMPLE_SETTING)buff;
    
    pCommonSet->enAutoAvrTime = ENUM_AVG_TIME_15S;
	pCommonSet->enSmpResolution = ENUM_SMP_HIGH_RES;
	pCommonSet->sWaveLenArgs[0].fBackScattering = -79.44f;
	pCommonSet->sWaveLenArgs[0].fExcessLength = 0.0f;
	pCommonSet->sWaveLenArgs[0].fRefractiveIndex = 1.467700f;
	pCommonSet->sWaveLenArgs[0].fMacroDaltaThr = 0.5f;
	
	pCommonSet->sWaveLenArgs[1].fBackScattering = -81.87f;
	pCommonSet->sWaveLenArgs[1].fExcessLength = 0.0f;
	pCommonSet->sWaveLenArgs[1].fRefractiveIndex = 1.468325f;
	pCommonSet->sWaveLenArgs[1].fMacroDaltaThr = 0.5f;
	
	pCommonSet->sWaveLenArgs[2].fBackScattering = -82.58f;
	pCommonSet->sWaveLenArgs[2].fExcessLength = 0.0f;
	pCommonSet->sWaveLenArgs[2].fRefractiveIndex = 1.460000f;
	pCommonSet->sWaveLenArgs[2].fMacroDaltaThr = 0.5f;

    SaveSettings(SAMPLE_SET);
    return;
}

int CheckAnalysisSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    PANALYSIS_SETTING pAnalysisSet = (PANALYSIS_SETTING)buff;

    if( pAnalysisSet->fSpliceLossThr < 0.01f 
        || pAnalysisSet->fSpliceLossThr > 5.0f 
        || pAnalysisSet->fReturnLossThr < -78.0f 
        || pAnalysisSet->fReturnLossThr > -14.0f 
        || pAnalysisSet->fEndLossThr < 1.0f 
        || pAnalysisSet->fEndLossThr > 25.0f 
        || pAnalysisSet->iEnableLaunchFiberEvent > 1
        || pAnalysisSet->iEnableLaunchFiberEvent < 0
        || pAnalysisSet->iEnableRecvFiberEvent > 1
        || pAnalysisSet->iEnableRecvFiberEvent < 0
        || pAnalysisSet->iLaunchFiberEvent > 10
        || pAnalysisSet->iLaunchFiberEvent < 1
        || pAnalysisSet->iRecvFiberEvent > 10
        || pAnalysisSet->iRecvFiberEvent < 1
        || pAnalysisSet->iEventOrLength > 1
        || pAnalysisSet->iEventOrLength < 0
        || pAnalysisSet->iEnableAnalysisThreshold > 1
        || pAnalysisSet->iEnableAnalysisThreshold < 0
        || pAnalysisSet->iEnableLaunchAndReceiveFiber > 1
        || pAnalysisSet->iEnableLaunchAndReceiveFiber < 0)
    {
        return -1;
    }

    if(!UnitConverter_Dist_isEqual(pAnalysisSet->fLaunchFiberLen, 
        pAnalysisSet->fLaunchFiberLen_ft, pAnalysisSet->fLaunchFiberLen_mi))
    {
        return -1;
    }
    
    if(!UnitConverter_Dist_isEqual(pAnalysisSet->fRecvFiberLen, 
        pAnalysisSet->fRecvFiberLen_ft, pAnalysisSet->fRecvFiberLen_mi))
    {
        return -1;
    }
    
    return 0;
}

void ResetAnalysisSet(void* buff)
{
    if(!buff)
    {
        return;
    }

    PANALYSIS_SETTING pAnalysisSet = (PANALYSIS_SETTING)buff;
    
	pAnalysisSet->fRecvFiberLen = 5.0f;
	pAnalysisSet->fRecvFiberLen_ft = UnitConverter_Dist_Float2Float(UNIT_M, 
	    UNIT_FT, pAnalysisSet->fRecvFiberLen);
	pAnalysisSet->fRecvFiberLen_mi = UnitConverter_Dist_Float2Float(UNIT_M, 
	    UNIT_MI, pAnalysisSet->fRecvFiberLen);
	pAnalysisSet->fReturnLossThr = -72.0f;
	pAnalysisSet->fSpliceLossThr = 0.020f;
	pAnalysisSet->fLaunchFiberLen = 0.00f;
	pAnalysisSet->fLaunchFiberLen_ft = UnitConverter_Dist_Float2Float(UNIT_M, 
	    UNIT_FT, pAnalysisSet->fLaunchFiberLen);
	pAnalysisSet->fLaunchFiberLen_mi = UnitConverter_Dist_Float2Float(UNIT_M, 
	    UNIT_MI, pAnalysisSet->fLaunchFiberLen);
	pAnalysisSet->iAutoAnalysisFlag = 1;
	pAnalysisSet->iEndEventNum = 0;
	pAnalysisSet->fEndLossThr = 5.0f;
	pAnalysisSet->iEnableRecvFiber = 0;
	pAnalysisSet->iEndPositionFlag = 0;
	pAnalysisSet->iStartEventNum = 0;
	pAnalysisSet->iEnableLaunchFiber = 0;
    
    pAnalysisSet->iEnableLaunchFiberEvent = 1;
    pAnalysisSet->iEnableRecvFiberEvent = 1;
    pAnalysisSet->iLaunchFiberEvent = 1;
    pAnalysisSet->iRecvFiberEvent = 1;
    pAnalysisSet->iEventOrLength = 0;
    pAnalysisSet->iEnableAnalysisThreshold = 1;     //默认使能
    pAnalysisSet->iEnableLaunchAndReceiveFiber = 0; //默认不使能

    SaveSettings(ANALYSIS_SET);
    return;
}

int CheckResultSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    POTHER_SETTING pResultSet = (POTHER_SETTING)buff;
    
    if(pResultSet->PassThr.SpliceLossThr[WAVELEN_1310].fThrValue == 0.0f 
        || pResultSet->PassThr.ConnectLossThr[WAVELEN_1310].fThrValue == 0.0f
        || pResultSet->PassThr.RegionReturnLossThr[WAVELEN_1310].fThrValue == 0.0f
        || pResultSet->PassThr.SpliceLossThr[WAVELEN_1550].fThrValue == 0.0f 
        || pResultSet->PassThr.ConnectLossThr[WAVELEN_1550].fThrValue == 0.0f
        || pResultSet->PassThr.RegionReturnLossThr[WAVELEN_1550].fThrValue == 0.0f
        || pResultSet->PassThr.SpliceLossThr[WAVELEN_1625].fThrValue == 0.0f 
        || pResultSet->PassThr.ConnectLossThr[WAVELEN_1625].fThrValue == 0.0f
        || pResultSet->PassThr.RegionReturnLossThr[WAVELEN_1625].fThrValue == 0.0f
        || (pResultSet->PassThr.iThrEnable[WAVELEN_1310] < 0)//新增参数需要检测
        || (pResultSet->PassThr.iThrEnable[WAVELEN_1310] > 1)
        || (pResultSet->PassThr.iThrEnable[WAVELEN_1550] < 0)
        || (pResultSet->PassThr.iThrEnable[WAVELEN_1550] > 1)
        || (pResultSet->PassThr.iThrEnable[WAVELEN_1625] < 0)
        || (pResultSet->PassThr.iThrEnable[WAVELEN_1625] > 1)
      )
     {
        return -1;
     }
    
    if(!UnitConverter_Dist_isEqual(pResultSet->PassThr.fSpanLength_m[WAVELEN_1310], 
      pResultSet->PassThr.fSpanLength_ft[WAVELEN_1310], pResultSet->PassThr.fSpanLength_mi[WAVELEN_1310]))
    {
      return -1;
    }
    
    if(!UnitConverter_Dist_isEqual(pResultSet->PassThr.fSpanLength_m[WAVELEN_1550], 
    pResultSet->PassThr.fSpanLength_ft[WAVELEN_1550], pResultSet->PassThr.fSpanLength_mi[WAVELEN_1550]))
    {
    return -1;
    }
      
    if(!UnitConverter_Dist_isEqual(pResultSet->PassThr.fSpanLength_m[WAVELEN_1625], 
      pResultSet->PassThr.fSpanLength_ft[WAVELEN_1625], pResultSet->PassThr.fSpanLength_mi[WAVELEN_1625]))
    {
      return -1;
    }
    
    return 0;
}

void ResetResultSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    int i=0;
    POTHER_SETTING pResultSet = (POTHER_SETTING)buff;
    
	pResultSet->iDisplayPassInfoFlag = 0;
	pResultSet->iWaveLength = 0;
	for (i = 0; i < WAVE_NUM; ++i) 					//通过/未通过阈值;
	{												
		pResultSet->PassThr.SpliceLossThr[i].iSlectFlag = 1;//熔接损耗
		pResultSet->PassThr.SpliceLossThr[i].fThrValue = 0.3f;

		pResultSet->PassThr.ConnectLossThr[i].iSlectFlag = 1;//接头损耗
		pResultSet->PassThr.ConnectLossThr[i].fThrValue = 0.75f;

		pResultSet->PassThr.ReturnLossThr[i].iSlectFlag = 1;//反射率损耗
		pResultSet->PassThr.ReturnLossThr[i].fThrValue = -40.0f;

		pResultSet->PassThr.AttenuationThr[i].iSlectFlag = 1;//衰减损耗
		pResultSet->PassThr.AttenuationThr[i].fThrValue = 0.4f;

		pResultSet->PassThr.RegionLossThr[i].iSlectFlag = 1;//跨段损耗
		pResultSet->PassThr.RegionLossThr[i].fThrValue = 20.0f;

		pResultSet->PassThr.RegionLengthThr[i].iSlectFlag = 1;//跨段长度
		pResultSet->PassThr.RegionLengthThr[i].fThrValue = 0.0f;

		pResultSet->PassThr.RegionReturnLossThr[i].iSlectFlag = 1;//跨段光回损
		pResultSet->PassThr.RegionReturnLossThr[i].fThrValue = 15.0f;

		pResultSet->PassThr.fSpanLength_m[i] = 0.0f;
		pResultSet->PassThr.fSpanLength_ft[i] = 0.0f;
		pResultSet->PassThr.fSpanLength_mi[i] = 0.0f;

		pResultSet->PassThr.iThrEnable[i] = 1;      //使能阈值标志位
	}

    SaveSettings(RESULT_SET);
    return;
}

int CheckFileNameSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    FILE_NAME_SETTING* pFileNameSet = (FILE_NAME_SETTING*)buff;
    
    if(pFileNameSet->iAutoNamedEnable > 1
        || pFileNameSet->prefix[0] == 0
        || pFileNameSet->suffixWidth > 5 
        || pFileNameSet->suffixWidth < 3
        || pFileNameSet->suffixRule > 1
        || pFileNameSet->fileFormat < 1
        || pFileNameSet->fileFormat > 3
        || pFileNameSet->enFileOperation > FILE_SAVE_MODE
        || pFileNameSet->enFilter > NONE
        )
     {
        return -1;
     }
    return 0;
}

void ResetFileNameSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    FILE_NAME_SETTING* pFileNameSet = (FILE_NAME_SETTING*)buff;
    
	pFileNameSet->iAutoNamedEnable = 1;
	
	strcpy(pFileNameSet->prefix, "Fiber");
	pFileNameSet->suffix = 1;
	pFileNameSet->suffixWidth = 3;
	
	pFileNameSet->suffixRule = 1;
	pFileNameSet->fileFormat = 2;   //默认SOR文件
	
	pFileNameSet->enFileOperation = FILE_NORMAL_MODE;
	pFileNameSet->enFilter = ANY;

    SaveSettings(FILENAME_SET);
    return;
}

int CheckSolaIdentifySet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    SOLA_IDENTIFY* pSolaIdentifySet = (SOLA_IDENTIFY*)buff;
    
    if(pSolaIdentifySet->cableID.start > pSolaIdentifySet->cableID.stop
        || pSolaIdentifySet->fiberID.start > pSolaIdentifySet->fiberID.stop
        || pSolaIdentifySet->locationA.start > pSolaIdentifySet->locationA.stop
        || pSolaIdentifySet->locationB.start > pSolaIdentifySet->locationB.stop
        || !pSolaIdentifySet->cableID.setp
        || !pSolaIdentifySet->fiberID.setp
        || !pSolaIdentifySet->locationA.setp
        || !pSolaIdentifySet->locationB.setp
      )
     {
        return -1;
     }
    
    return 0;
}

void ResetSolaIdentifySet(void* buff)
{
    if(!buff)
    {
        return;
    }
    SOLA_IDENTIFY* pSolaIdentifySet = (SOLA_IDENTIFY*)buff;
    
    pSolaIdentifySet->cableID.auto_increment=0;
    pSolaIdentifySet->cableID.start=1;
    pSolaIdentifySet->cableID.stop=64;
    pSolaIdentifySet->cableID.setp=1;
    
    pSolaIdentifySet->fiberID.auto_increment=1;
    pSolaIdentifySet->fiberID.start=1;
    pSolaIdentifySet->fiberID.stop=999;
    pSolaIdentifySet->fiberID.setp=1;

    pSolaIdentifySet->locationA.auto_increment=0;
    pSolaIdentifySet->locationA.start=1;
    pSolaIdentifySet->locationA.stop=64;
    pSolaIdentifySet->locationA.setp=1;

    pSolaIdentifySet->locationB.auto_increment=0;
    pSolaIdentifySet->locationB.start=1;
    pSolaIdentifySet->locationB.stop=64;
    pSolaIdentifySet->locationB.setp=1;

    strcpy(pSolaIdentifySet->solaIdentify.cableID, "");
    strcpy(pSolaIdentifySet->solaIdentify.fiberID, "");
    strcpy(pSolaIdentifySet->solaIdentify.LocationA, "");
    strcpy(pSolaIdentifySet->solaIdentify.LocationB, "");
    SaveSettings(SOLA_IDENTIFY_SET);
    return;
}

int CheckSolaLineDefineSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    SOLA_LINEDEFINE* pSolaLineDefineSet = (SOLA_LINEDEFINE*)buff;
    
    if( pSolaLineDefineSet->groupIndex1550 == 0.0f ||
        pSolaLineDefineSet->backScatter1550 == 0.0f
      )
     {
        return -1;
     }
    
    return 0;
}

void ResetSolaLineDefineSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    SOLA_LINEDEFINE* pSolaLineDefineSet = (SOLA_LINEDEFINE*)buff;
    
    pSolaLineDefineSet->splittingRatioLevel1 = 0;
    pSolaLineDefineSet->splittingRatioLevel2 = 0;
    pSolaLineDefineSet->splittingRatioLevel3 = 0;
    pSolaLineDefineSet->groupIndex1550 = 1.468325;
    pSolaLineDefineSet->backScatter1550 = -81.87;
    pSolaLineDefineSet->isReminderDialog = 0;

    SaveSettings(SOLA_LINEDEFINE_SET);
    return;
}

int CheckSolaLinePassSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    SOLA_LINE_PASSTHRESHOLD* pSolaLinePassSet = (SOLA_LINE_PASSTHRESHOLD*)buff;

    if(!UnitConverter_Dist_isEqual(1000*pSolaLinePassSet->lineLength.min,
        pSolaLinePassSet->lineLength_ft.min, pSolaLinePassSet->lineLength_mile.min))
    {
        return -1;
    }

    if(!UnitConverter_Dist_isEqual(1000*pSolaLinePassSet->lineLength.max,
        pSolaLinePassSet->lineLength_ft.max, pSolaLinePassSet->lineLength_mile.max))
    {
        return -1;
    }
    
    return 0;
}

void ResetSolaLinePassSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    SOLA_LINE_PASSTHRESHOLD* pSolaLinePassSet = (SOLA_LINE_PASSTHRESHOLD*)buff;
    
    pSolaLinePassSet->lineLength.type = MINANDMAX;
    pSolaLinePassSet->lineLength.min = 0.0;
    pSolaLinePassSet->lineLength.max = 80.0;
    pSolaLinePassSet->lineLength_ft.min = UnitConverter_Dist_Float2Float(UNIT_KM, UNIT_FT, 0.0f);
    pSolaLinePassSet->lineLength_ft.max = UnitConverter_Dist_Float2Float(UNIT_KM, UNIT_FT, 80.0f);
    pSolaLinePassSet->lineLength_mile.min = UnitConverter_Dist_Float2Float(UNIT_KM, UNIT_MI, 0.0f);
    pSolaLinePassSet->lineLength_mile.max = UnitConverter_Dist_Float2Float(UNIT_KM, UNIT_MI, 80.0f);
    pSolaLinePassSet->currentWave = 0;
    int i= 0;
    for(i =0;i<4;i++)
    {
        pSolaLinePassSet->lineWave[i].lineLoss.type = MINANDMAX;
        pSolaLinePassSet->lineWave[i].lineLoss.min = 0.0;
        pSolaLinePassSet->lineWave[i].lineLoss.max = 20.0;
        pSolaLinePassSet->lineWave[i].lineReturnLoss.type = NOLYMAX;
        pSolaLinePassSet->lineWave[i].lineReturnLoss.max = 15.0;
    }

    SaveSettings(SOLA_LINEPASS_SET);
    return;
}

int CheckSolaItemPassSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    SOLA_ITEM_PASSTHRESHOLD* pSolaLineItemPassSet = (SOLA_ITEM_PASSTHRESHOLD*)buff;
    
    if( pSolaLineItemPassSet->maxSplitter2Loss.fValue[WAVELEN_1310] == 0.0f ||
        pSolaLineItemPassSet->maxSplitter4Loss.fValue[WAVELEN_1310] == 0.0f ||
        pSolaLineItemPassSet->maxSplitter8Loss.fValue[WAVELEN_1310] == 0.0f ||
        pSolaLineItemPassSet->maxSplitter16Loss.fValue[WAVELEN_1310] == 0.0f ||
        pSolaLineItemPassSet->maxSplitter32Loss.fValue[WAVELEN_1310] == 0.0f ||
        pSolaLineItemPassSet->maxSplitter64Loss.fValue[WAVELEN_1310] == 0.0f ||
        pSolaLineItemPassSet->maxSplitter128Loss.fValue[WAVELEN_1310] == 0.0f
      )
    {
        return -1;
    }

    if( pSolaLineItemPassSet->maxSplitter2Loss.iSelectedFlag[WAVELEN_1310] < 0 ||
        pSolaLineItemPassSet->maxSplitter4Loss.iSelectedFlag[WAVELEN_1310] < 0 ||
        pSolaLineItemPassSet->maxSplitter8Loss.iSelectedFlag[WAVELEN_1310] < 0 ||
        pSolaLineItemPassSet->maxSplitter16Loss.iSelectedFlag[WAVELEN_1310] < 0 ||
        pSolaLineItemPassSet->maxSplitter32Loss.iSelectedFlag[WAVELEN_1310] < 0 ||
        pSolaLineItemPassSet->maxSplitter64Loss.iSelectedFlag[WAVELEN_1310] < 0 ||
        pSolaLineItemPassSet->maxSplitter128Loss.iSelectedFlag[WAVELEN_1310] < 0
      )
    {
        return -1;
    }
    return 0;
}

void ResetSolaItemPassSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    SOLA_ITEM_PASSTHRESHOLD* pSolaLineItemPassSet = (SOLA_ITEM_PASSTHRESHOLD*)buff;
    pSolaLineItemPassSet->iCurrentWave = WAVELEN_1310;
    
    int i;
    for(i = 0; i < WAVE_NUM+1; ++i)
    {
        pSolaLineItemPassSet->maxSpliceLoss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSpliceLoss.fValue[i] = 1.000;
        pSolaLineItemPassSet->maxLinkerLoss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxLinkerLoss.fValue[i] = 1.000;
        pSolaLineItemPassSet->maxLinkerReflectance.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxLinkerReflectance.fValue[i]= -40.0;
        pSolaLineItemPassSet->maxSplitter2Loss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitter2Loss.fValue[i] = 4.500;
        pSolaLineItemPassSet->maxSplitter4Loss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitter4Loss.fValue[i] = 8.500;
        pSolaLineItemPassSet->maxSplitter8Loss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitter8Loss.fValue[i] = 12.000;
        pSolaLineItemPassSet->maxSplitter16Loss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitter16Loss.fValue[i] = 15.000;
        pSolaLineItemPassSet->maxSplitter32Loss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitter32Loss.fValue[i] = 18.500;
        pSolaLineItemPassSet->maxSplitter64Loss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitter64Loss.fValue[i] = 22.000;
        pSolaLineItemPassSet->maxSplitter128Loss.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitter128Loss.fValue[i] = 25.500;
        pSolaLineItemPassSet->maxSplitterReflectance.iSelectedFlag[i] = 1;
        pSolaLineItemPassSet->maxSplitterReflectance.fValue[i] = -40.0;
    }

    LOG(LOG_INFO, "----ResetSolaItemPassSet----\n");
    SaveSettings(SOLA_ITEMPASS_SET);
    return;
}

int CheckLightSourceSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    // SOURCELIGHTCONFIG* pLightSourceSet = (SOURCELIGHTCONFIG*)buff;
    // if(pLightSourceSet->frequence == 0)
    // {
    //     return -1;
    // }
    
    return 0;
}

void ResetLightSourceSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    SOURCELIGHTCONFIG* pLightSourceSet = (SOURCELIGHTCONFIG*)buff;
    
    pLightSourceSet->wavelength = SOURCELIGHTWAVELENTH1310;
    pLightSourceSet->frequence = SOURCELIGHTMODECW;

    SaveSettings(LIGHTSOURCE);
    return;
}

int CheckLightSourcePowerSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    LIGHT_SOURCE_POWER_CONFIG_LIST* pLightSourcePowerSet = (LIGHT_SOURCE_POWER_CONFIG_LIST*)buff;
    if( pLightSourcePowerSet->powerConfig[0].pulse < 1 || pLightSourcePowerSet->powerConfig[1].pulse > 499 )
    {
        return -1;
    }
    
    return 0;
}

void ResetLightSourcePowerSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    LIGHT_SOURCE_POWER_CONFIG_LIST* pLightSourcePowerSet = (LIGHT_SOURCE_POWER_CONFIG_LIST*)buff;

    int i;
    for (i=0;i<sizeof(pLightSourcePowerSet->powerConfig)/sizeof(LIGHT_SOURCE_POWER_CONFIG);i++)
    {
        if (i == 0)
        {
            pLightSourcePowerSet->powerConfig[i].wavelength = SOURCELIGHTWAVELENTH1310;
            pLightSourcePowerSet->powerConfig[i].pulse = 400;
        }
        else if (i == 1)
        {
            pLightSourcePowerSet->powerConfig[i].wavelength = SOURCELIGHTWAVELENTH1550;
            pLightSourcePowerSet->powerConfig[i].pulse = 400;
        }
        else
        {
            // do nothing
        }
    }
    
    SaveSettings(LIGHTSOURCE_POWER_CONFIG_LIST);
    return;
}

int CheckDefSavePathSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    PDEFAULT_SAVEPATH pDefSavePathSet = (PDEFAULT_SAVEPATH)buff;
    
    if(!strstr(pDefSavePathSet->cOtdrSorSavePath, MntDataDirectory) 
        || !strstr(pDefSavePathSet->cOtdrReportSavePath, MntDataDirectory)
        || !strstr(pDefSavePathSet->cSolaSavePath, MntDataDirectory)
        )
    {
        return -1;
    }

    char temp[128] = {0};
    sprintf(temp, "mkdir -p %s", pDefSavePathSet->cOtdrSorSavePath);
    access(pDefSavePathSet->cOtdrSorSavePath,F_OK) ? mysystem(temp) : 0;
    memset(temp, 0, 128);
    sprintf(temp, "mkdir -p %s", pDefSavePathSet->cOtdrReportSavePath);
    access(pDefSavePathSet->cOtdrReportSavePath,F_OK) ? mysystem(temp) : 0;
    #ifdef MINI2
    memset(temp, 0, 128);
    sprintf(temp, "mkdir -p %s", pDefSavePathSet->cSolaSavePath);
    access(pDefSavePathSet->cSolaSavePath,F_OK) ? mysystem(temp) : 0;
    #endif
    
    return 0;
}

void ResetDefSavePathSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    PDEFAULT_SAVEPATH pDefSavePathSet = (PDEFAULT_SAVEPATH)buff;
    
    sprintf(pDefSavePathSet->cOtdrSorSavePath, MntDataDirectory"/data");
    sprintf(pDefSavePathSet->cOtdrReportSavePath, MntDataDirectory"/data");
    sprintf(pDefSavePathSet->cSolaSavePath, MntDataDirectory"/soladata");
    
    char temp[128] = {0};
    sprintf(temp, "mkdir -p %s", pDefSavePathSet->cOtdrSorSavePath);
    access(pDefSavePathSet->cOtdrSorSavePath,F_OK) ? mysystem(temp) : 0;
    memset(temp, 0, 128);
    sprintf(temp, "mkdir -p %s", pDefSavePathSet->cOtdrReportSavePath);
    access(pDefSavePathSet->cOtdrReportSavePath,F_OK) ? mysystem(temp) : 0;
    #ifdef MINI2
    memset(temp, 0, 128);
    sprintf(temp, "mkdir -p %s", pDefSavePathSet->cSolaSavePath);
    access(pDefSavePathSet->cSolaSavePath,F_OK) ? mysystem(temp) : 0;
    #endif
    SaveSettings(OTDR_SOLA_DEFSAVEPATH);
    return;
}
int CheckOtdrMarkSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    MarkParam* markParam = (MarkParam*)buff;
    if(markParam->Deraction != 0 && markParam->Deraction != 1)
    {
        return -1;
    }
    return 0;

}
void ResetOtdrMarkSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    MarkParam* markParam = (MarkParam*)buff;
    memset(markParam, 0, sizeof(MarkParam));
    LOG(LOG_INFO, "ResetOtdrMarkSet!!\n\n\n");
}

int CheckSolaSetting(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    SolaSettings* solaSetting = (SolaSettings*)buff;
    
    if((solaSetting->Wave[WAVELEN_1310] == 0 
        && solaSetting->Wave[WAVELEN_1550] == 0 
        && solaSetting->Wave[WAVELEN_1625] == 0)
        || (solaSetting->Wave[WAVELEN_1310] > 1) 
        || (solaSetting->Wave[WAVELEN_1550] > 1)  
        || (solaSetting->Wave[WAVELEN_1625] > 1)  
        || (solaSetting->iEnableLaunchFiber > 1)  
        || (solaSetting->iEnableRecvFiber > 1)
        || (solaSetting->iUnitSelected > 4))
    {
        return -1;
    }

    if(!UnitConverter_Dist_isEqual(solaSetting->fLaunchFiberLen,
        solaSetting->fLaunchFiberLen_ft, solaSetting->fLaunchFiberLen_mile))
    {
        return -1;
    }

    if(!UnitConverter_Dist_isEqual(solaSetting->fRecvFiberLen,
        solaSetting->fRecvFiberLen_ft, solaSetting->fRecvFiberLen_mile))
    {
        return -1;
    }
    
    return 0;

}

void ResetSolaSetting(void* buff)
{
    if(!buff)
    {
        return;
    }
    SolaSettings* solaSetting = (SolaSettings*)buff;
    memset(solaSetting, 0, sizeof(SolaSettings));
    solaSetting->Wave[WAVELEN_1310] = 1;
    solaSetting->Wave[WAVELEN_1550] = 1;
    solaSetting->Wave[WAVELEN_1625] = 0;
    solaSetting->iUnitSelected = 0;
    float LaunchFiberLength = solaSetting->fLaunchFiberLen;
    float RecvFiberLen = solaSetting->fRecvFiberLen;
    solaSetting->fLaunchFiberLen_ft = UnitConverter_Dist_Float2Float(UNIT_M, UNIT_FT, LaunchFiberLength);
    solaSetting->fLaunchFiberLen_mile = UnitConverter_Dist_Float2Float(UNIT_M, UNIT_MI, LaunchFiberLength);
    solaSetting->fRecvFiberLen_ft = UnitConverter_Dist_Float2Float(UNIT_M, UNIT_FT, RecvFiberLen);
    solaSetting->fRecvFiberLen_mile = UnitConverter_Dist_Float2Float(UNIT_M, UNIT_MI, RecvFiberLen);
    LOG(LOG_INFO, "ResetSolaSettings!!\n\n\n");
}

int CheckSolaFileNameSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    SOLA_FILE_NAME_SETTING* pSolaFileNameSet = (SOLA_FILE_NAME_SETTING*)buff;
    
    if(pSolaFileNameSet->prefix[0] == 0
        || pSolaFileNameSet->suffixWidth > 5 
        || pSolaFileNameSet->suffixWidth < 3
        || pSolaFileNameSet->suffixRule > 1
        || pSolaFileNameSet->fileFormat < 1
        || pSolaFileNameSet->fileFormat > 3
        )
    {
        return -1;
    }
    return 0;
}

void ResetSolaFileNameSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    
    SOLA_FILE_NAME_SETTING* pFileNameSet = (SOLA_FILE_NAME_SETTING*)buff;
    	
	strcpy(pFileNameSet->prefix, "Sola");
	pFileNameSet->suffix = 1;
	pFileNameSet->suffixWidth = 3;
	pFileNameSet->suffixRule = 1;
	pFileNameSet->fileFormat = 2;   //默认SOLA文件
	
    SaveSettings(SOLA_FILENAME_SET);
    
    return;
}

int CheckSolaIdentSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    MarkParam* pSolaIdentSet = (MarkParam*)buff;
    
    if(pSolaIdentSet->Deraction != 0 && pSolaIdentSet->Deraction != 1)
    {
        return -1;
    }
    return 0;
}

void ResetSolaIdentSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    
    MarkParam* pSolaIdentSet = (MarkParam*)buff;
    	
    memset(pSolaIdentSet, 0, sizeof(MarkParam));
	
    SaveSettings(SOLA_FILENAME_SET);
    
    return;
}
//OTDR平台开机初始化
int OtdrPlatformInit(void)
{
    int iErr=0;

	DBG_ENTER();
    pMarkParam = (MarkParam*)malloc(sizeof(MarkParam));
    if(NULL != pMarkParam)
    {
        memset(pMarkParam, 0, sizeof(MarkParam));
#ifdef EEPROM_DATA
        GetSettingsData((void*)pMarkParam, sizeof(MarkParam), OTDR_MARK_SET);
#endif
    }

    pSolaMessager = (SOLA_MESSAGER*)malloc(sizeof(SOLA_MESSAGER));
    if(pSolaMessager != NULL)
    {
        GetSettingsData((void*)&pSolaMessager->identify, sizeof(SOLA_IDENTIFY), SOLA_IDENTIFY_SET);
        GetSettingsData((void*)&pSolaMessager->lineDefine, sizeof(SOLA_LINEDEFINE), SOLA_LINEDEFINE_SET);
        GetSettingsData((void*)&pSolaMessager->linePassThreshold, sizeof(SOLA_LINE_PASSTHRESHOLD), SOLA_LINEPASS_SET);
        GetSettingsData((void*)&pSolaMessager->itemPassThreshold, sizeof(SOLA_ITEM_PASSTHRESHOLD), SOLA_ITEMPASS_SET);
        GetSettingsData((void*)&pSolaMessager->autoFilename, sizeof(SOLA_FILE_NAME_SETTING), SOLA_FILENAME_SET);
        GetSettingsData((void*)&pSolaMessager->newIdentify, sizeof(MarkParam), SOLA_IDENT);
	}

    pSourceLightConfig = (SOURCELIGHTCONFIG*)malloc(sizeof(SOURCELIGHTCONFIG));
    if(NULL != pSourceLightConfig)
    {
        memset(pSourceLightConfig, 0, sizeof(SOURCELIGHTCONFIG));
#ifdef EEPROM_DATA
        GetSettingsData((void*)pSourceLightConfig, sizeof(SOURCELIGHTCONFIG), LIGHTSOURCE);
#endif
	}
    
    pLightSourcePowerConfig = (LIGHT_SOURCE_POWER_CONFIG_LIST*)malloc(sizeof(LIGHT_SOURCE_POWER_CONFIG_LIST));
    if(NULL != pLightSourcePowerConfig)
    {
        memset(pLightSourcePowerConfig, 0, sizeof(LIGHT_SOURCE_POWER_CONFIG_LIST));
#ifdef EEPROM_DATA
        GetSettingsData((void*)pLightSourcePowerConfig, sizeof(LIGHT_SOURCE_POWER_CONFIG_LIST), LIGHTSOURCE_POWER_CONFIG_LIST);
#endif
    }
    
    //OTDR初始化时，申请数据空间
	pOtdrTopSettings = (POTDR_TOP_SETTINGS)calloc(1, sizeof(OTDR_TOP_SETTINGS));
	if (pOtdrTopSettings != NULL)
	{
	    pOtdrTopSettings->iOtdrExitFlag = 0;
        pOtdrTopSettings->pOpmSet = Opm_InitSet();
    	pOtdrTopSettings->pUser_Setting = (PUSER_SETTINGS)calloc(1,sizeof(USER_SETTINGS));
    	pOtdrTopSettings->pTask_Context = (PTASK_CONTEXTS)calloc(1,sizeof(TASK_CONTEXTS));
    	pOtdrTopSettings->pDisplayInfo = (PDISPLAY_INFO)calloc(1,sizeof(DISPLAY_INFO));
    	pOtdrTopSettings->pComQueue = (PCOMMAND_QUEUE)calloc(1,sizeof(COMMAND_QUEUE));
	}
	
    //判断内存申请是否成功
	iErr = !(pOtdrTopSettings &&
		   	 pOtdrTopSettings->pOpmSet && 
		   	 pOtdrTopSettings->pUser_Setting &&
	       	 pOtdrTopSettings->pTask_Context && 
	       	 pOtdrTopSettings->pDisplayInfo &&  
	       	 pOtdrTopSettings->pComQueue);
	if (iErr == 0)
	{
#ifdef EEPROM_DATA
        GetSettingsData((void*)&pOtdrTopSettings->sDefSavePath, sizeof(DEFAULT_SAVEPATH), OTDR_SOLA_DEFSAVEPATH);
#endif	
		pUser_Settings = pOtdrTopSettings->pUser_Setting;
	}

	if (!iErr)
	{
#ifdef OPTIC_DEVICE
        //关闭光模块等电源
		CloseOpm(pOtdrTopSettings->pOpmSet);
#endif
	}

	if (!iErr)
	{
   		iErr = InitDisplayInfo(pOtdrTopSettings->pDisplayInfo);
	}
	
	//设置默认参数
	if (0 == iErr)
	{
		SetDefaultUserSetting(pUser_Settings);	
		DISPLAY_PARA CtlPara;

		CtlPara.enSize = EN_SIZE_NOMAL;
		//if (pUser_Settings->sCommonSetting.iScreenShotFlag)
		//	CtlPara.enShot = EN_SHOT_YES;
		//else
		CtlPara.enShot = EN_SHOT_NO;

		if (pUser_Settings->sCommonSetting.iLossMethodMark)
			CtlPara.enMethd = EN_METHD_TPA;
		else
			CtlPara.enMethd = EN_METHD_LSA;

		if (pUser_Settings->sCommonSetting.iAtenuMethodMark)
    		CtlPara.enMethdAtten = EN_METHD_TPA;
    	else
    		CtlPara.enMethdAtten = EN_METHD_LSA;

		SetDisplayCtlPara(pOtdrTopSettings->pDisplayInfo, &CtlPara);
	}
#if 0//测试eeprom数据
    FILE *fd = fopen(MntUsbDirectory "/system_parameter.txt", "a+");
    printFrontSet((void *)&(pUser_Settings->sFrontSetting[WAVELEN_1310]), fd);
    printFrontSet((void *)&(pUser_Settings->sFrontSetting[WAVELEN_1550]), fd);
    printFrontSet((void *)&(pUser_Settings->sFrontSetting[WAVELEN_1625]), fd);

    printCommonSet((void *)&(pUser_Settings->sCommonSetting), fd);
    printSampleSet((void *)&(pUser_Settings->sSampleSetting), fd);
    printAnalysisSet((void *)&(pUser_Settings->sAnalysisSetting), fd);
    printResultSet((void *)&(pUser_Settings->sOtherSetting), fd);

    printLightSource(pSourceLightConfig, fd);
    printOtdrMarkSet(pMarkParam, fd);

    fclose(fd);
#endif
    //初始化锁
	if(iErr == 0)
	{
        InitMutex(&pUser_Settings->mDataMutex, NULL);
        InitMutex(&pUser_Settings->sAnalysisSetting.mDataMutex, NULL);
        InitMutex(&pUser_Settings->sAnalysisSetting.mDataMutex, NULL);
        InitMutex(&pUser_Settings->sCommonSetting.mDataMutex, NULL);
        InitMutex(&pUser_Settings->sOtherSetting.mDataMutex, NULL);
        InitMutex(&pUser_Settings->sSampleSetting.mDataMutex, NULL);
	}

    //各线程的创建
    if (iErr == 0)
    {
		//创建OTDR控制线程
		if(ThreadCreate(&thdOtdrCtrl, NULL, OtdrCtrlThread, pOtdrTopSettings))
		{
			iErr = -4;
		}
		//创建数据采集(DAQ)线程
		if(ThreadCreate(&thdDataAcquisition, NULL, DataAcquisitionThread, NULL))
		{
			iErr = -5;
		}
		//创建信号处理(DP)线程
		if(ThreadCreate(&thdDataProessing, NULL, DataProcessingThread, pOtdrTopSettings))
		{
			iErr = -6;
		}
		//创建描绘曲线线程
		if(ThreadCreate(&thdCurveDrawing, NULL, DrawCurveThread, pOtdrTopSettings->pDisplayInfo))
		{
			iErr = -7;
		}
    }

	if(!iErr)
	{
		//创建窗口调用栈
		pWndStackInit = InitStack();
		if(pWndStackInit != NULL)
		{
			SetCurrWndStack(pWndStackInit);
		}
		
		//创建otdr消息队列
		pOtdrMsg = CreateOTDRMsgQueue(OTDRMESSAGE_ITEM_LIMIT);
		if(pOtdrMsg != NULL)
		{
			SetCurrOTDRMsgQueue(pOtdrMsg);
		}
	}

    if (!iErr)
    {
        if (CurrSola_Init())
        {
            iErr = -9;
        }
    }
	
	if(iErr)
	{
		LOG(LOG_ERROR, "iErr = %d\n",iErr);
		OtdrPlatformRelease();//若初始化失败，则释放资源;
	}
	DBG_EXIT(iErr);
    return iErr;
}

#if 0
/*
说明:
	打印用户设置参数，用于调试
返回:
	void
作者:
	
日期:
	2014.10.21
修改: 
*/
void PrintUserSetting(PUSER_SETTINGS pUserSettings)
{
	if (pUserSettings == NULL)
	{
		return;
	}
	
	LOG(LOG_ERROR, "###############################################################################################\n");
	LOG(LOG_ERROR, "enWorkMode = %d\n", pUserSettings->enWorkMode);
	LOG(LOG_ERROR, "iWave[0] = %d\n, iWave[1] = %d\n,iWave[2] = %d\n\n", pUserSettings->iWave[0], pUserSettings->iWave[1], pUserSettings->iWave[2]);
	LOG(LOG_ERROR, "enFiberRange = %d\n", pUserSettings->enFiberRange);
	LOG(LOG_ERROR, "enPulseTime = %d\n", pUserSettings->enPulseTime);
	LOG(LOG_ERROR, "enAverageTime = %d\n\n", pUserSettings->enAverageTime);
	LOG(LOG_ERROR, "iLightCheckFlag = %d\n", pUserSettings->iLightCheckFlag);
	LOG(LOG_ERROR, "iConectCheckFlag = %d\n", pUserSettings->iConectCheckFlag);
	LOG(LOG_ERROR, "iAutoSaveFlag = %d\n", pUserSettings->iAutoSaveFlag);
	LOG(LOG_ERROR, "iMacroBendingFlag = %d\n", pUserSettings->iMacroBendingFlag);
	LOG(LOG_ERROR, "iAutoSpanFlag = %d\n\n", pUserSettings->iAutoSpanFlag);
	LOG(LOG_ERROR, "fMacroDaltaThr[0] = %f, fMacroDaltaThr[1] = %f, fMacroDaltaThr[2] = %f\n", pUserSettings->fMacroDaltaThr[0], pUserSettings->fMacroDaltaThr[1], pUserSettings->fMacroDaltaThr[2]);
	LOG(LOG_ERROR, "iLossMethodMark = %d\n", pUserSettings->iLossMethodMark);
	LOG(LOG_ERROR, "iAtenuMethodMark = %d\n", pUserSettings->iAtenuMethodMark);
	LOG(LOG_ERROR, "iHighResolutionFlag = %d\n", pUserSettings->iHighResolutionFlag);
	LOG(LOG_ERROR, "enSamleInterval = %d\n", pUserSettings->enSamleInterval);
	LOG(LOG_ERROR, "enAutoAvrTime = %d\n", pUserSettings->enAutoAvrTime);
	LOG(LOG_ERROR, "fRefractiveIndex[0] = %f, fRefractiveIndex[1] = %f, fRefractiveIndex[2] = %f\n", pUserSettings->fRefractiveIndex[0], pUserSettings->fRefractiveIndex[1], pUserSettings->fRefractiveIndex[2]);
	LOG(LOG_ERROR, "fBackScattering[0] = %f, fBackScattering[1] = %f, fBackScattering[2] = %f\n", pUserSettings->fBackScattering[0], pUserSettings->fBackScattering[1], pUserSettings->fBackScattering[2]);
	LOG(LOG_ERROR, "fExcessLength[0] = %f, fExcessLength[1] = %f, fExcessLength[2] = %f\n", pUserSettings->fExcessLength[0], pUserSettings->fExcessLength[1], pUserSettings->fExcessLength[2]);
	LOG(LOG_ERROR, "iAutoAnalysisFlag = %d\n",pUserSettings->iAutoAnalysisFlag); 
	LOG(LOG_ERROR, "fSpliceLossThr = %f\n", pUserSettings->fSpliceLossThr);
	LOG(LOG_ERROR, "fReturnLossThr = %f\n", pUserSettings->fReturnLossThr);
	LOG(LOG_ERROR, "iEndLossThr = %f\n", pUserSettings->iEndLossThr);
	LOG(LOG_ERROR, "iStartPointSetFlag = %d\n", pUserSettings->iStartPointSetFlag);
	LOG(LOG_ERROR, "iStartEventNum = %d\n", pUserSettings->iStartEventNum);
	LOG(LOG_ERROR, "fStartPosition = %f\n", pUserSettings->fStartPosition);
	LOG(LOG_ERROR, "iEndPointSetFlag = %d\n", pUserSettings->iEndPointSetFlag);
	LOG(LOG_ERROR, "iEndEventNum = %d\n", pUserSettings->iEndEventNum);
	LOG(LOG_ERROR, "fEndPosition = %f\n", pUserSettings->fEndPosition);
	LOG(LOG_ERROR, "iEndPositionFlag = %d\n", pUserSettings->iEndPositionFlag);
	LOG(LOG_ERROR, "iDisplayPassInfoFlag = %d\n", pUserSettings->iDisplayPassInfoFlag);
	LOG(LOG_ERROR, "iWaveLength = %d\n\n", pUserSettings->iWaveLength);
	//LOG(LOG_ERROR, "PassThr = %d\n", pUserSettings->PassThr);

}
#endif

/*
说明:
	设置默认的用户设置
返回:
	成功 0，失败其他
作者:
	
日期:
	2014.10.22
修改: 
*/
int SetDefaultUserSetting(PUSER_SETTINGS pUserSettings)
{
	int iRet = 0;

	if (NULL == pUserSettings)
	{	
		return -1;
	}

	memset(pUserSettings, 0, sizeof(PUSER_SETTINGS));
	
	pUserSettings->enWorkMode = ENUM_MODE_NORMAL;	//工作模式默认NORMAL
	pUserSettings->iWave[WAVELEN_1310] = 1;
	pUserSettings->iWave[WAVELEN_1550] = 0;
	pUserSettings->iWave[WAVELEN_1625] = 0;
	pUserSettings->enWaveCurPos = ENUM_WAVE_1310NM;
#ifdef EEPROM_DATA
	GetSettingsData((void*)&(pUserSettings->sFrontSetting[WAVELEN_1310]), sizeof(FRONT_SETTING), FRONTSET_1310);
	GetSettingsData((void*)&(pUserSettings->sFrontSetting[WAVELEN_1550]), sizeof(FRONT_SETTING), FRONTSET_1550);
	GetSettingsData((void*)&(pUserSettings->sFrontSetting[WAVELEN_1625]), sizeof(FRONT_SETTING), FRONTSET_1625);

	GetSettingsData((void*)&(pUserSettings->sCommonSetting), sizeof(COMMON_SETTING), COMMON_SET);
	
	GetSettingsData((void*)&(pUserSettings->sSampleSetting), sizeof(SAMPLE_SETTING), SAMPLE_SET);
	
	GetSettingsData((void*)&(pUserSettings->sAnalysisSetting), sizeof(ANALYSIS_SETTING), ANALYSIS_SET);
	
	GetSettingsData((void*)&(pUserSettings->sOtherSetting), sizeof(OTHER_SETTING), RESULT_SET);
	
	GetSettingsData((void*)&(pUserSettings->sFileNameSetting), sizeof(FILE_NAME_SETTING), FILENAME_SET);
	
	GetSettingsData((void*)&(pUserSettings->sSolaSetting), sizeof(SolaSettings), SOLA_SETTING_PARA);
#endif
    pUserSettings->sSolaSetting.Wave[0] = 1;
    pUserSettings->sSolaSetting.Wave[1] = 1;
    pUserSettings->sSolaSetting.Wave[2] = 0;

	CODER_LOG(CoderGu, "enAverageTime = %d\n", pUserSettings->sFrontSetting[0].enAverageTime);
	CODER_LOG(CoderGu, "enFiberRange = %d\n", pUserSettings->sFrontSetting[0].enFiberRange);
	CODER_LOG(CoderGu, "enPulseTime = %d\n", pUserSettings->sFrontSetting[0].enPulseTime);

	pUserSettings->sCommonSetting.iConectCheckFlag = 1;	                //链接检查 1
	pUserSettings->sCommonSetting.iLightCheckFlag = 1;	                //有光检查 1
	pUserSettings->sCommonSetting.iDisplaySectionEvent = 0;				//默认不显示区段事件
	pUser_Settings->sCommonSetting.iAutoSaveFlag = 0;					//默认不自动保存
	pUser_Settings->sCommonSetting.iPromptSaveFlag = 0;					//默认不提示保存文件
	pUser_Settings->sCommonSetting.iAutoSpanFlag = 0;					//默认不自动放大
	pUser_Settings->sCommonSetting.iAutoJumpWindow = 1;					//默认自动跳转到事件列表
    pUserSettings->sSampleSetting.enSmpResolution = ENUM_SMP_HIGH_RES;  //默认高分辨率
	return iRet;
}


