/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  getsetparameter.h
* 摘    要：  实现应用层中对eeprom存贮的参数的获取和保存
*             
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "app_getsetparameter.h"
#include "app_eeprom.h"

#include "app_global.h"
#include "app_parameter.h"
#include "app_frmotdr.h"
#include "wnd_frmskinset.h"
#include "wnd_frmfilebrowse.h"
#include "app_systemsettings.h"
#include "app_installment.h"
#include "wnd_frmabout.h"

#include <string.h>
#include <stdlib.h>

/*********************************************************************************************************
* 变量定义
*********************************************************************************************************/
PSYSTEMSET 		pCurSystemSet = NULL;
PWIFISET   		pCurWiFiSet  = NULL;
PSerialNum      pCurSN       = NULL;
PFactoryConfig  pFactoryConf = NULL;


SOLA_FILE_NAME_SETTING *pBmpFileNamedSetting = NULL;
//校准参数
PCALIBRATIONPARA pCalibrationPara = NULL;
/*********************************************************************************************************
* 函数声明
*********************************************************************************************************/


/*
**函数说明:
	初始化系统参数
**参数说明:
	无
**返回值:
	0		初始化成功
	-1		初始化失败
*/
int SystemSetInit(void)
{
	int iReturn = 0;
#ifdef OPTIC_DEVICE
    unsigned long long Tmp;
    GetFPGASerialNum(&Tmp);
#endif

	if( !(pCurSystemSet = (PSYSTEMSET)GuiMemAlloc(sizeof(SYSTEMSET))) )
	{
		return -1;
	}

	if( !(pCurWiFiSet = (WIFISET *)GuiMemAlloc(sizeof(WIFISET))) )
	{
		return -1;
	}
	if( !(pCurSN = (SerialNum *)GuiMemAlloc(sizeof(SerialNum))) )
	{
		return -1;
	}
    
	if( !(pFactoryConf = (FactoryConfig *)GuiMemAlloc(sizeof(FactoryConfig))) )
	{
		return -1;
	}
	if( !(pBmpFileNamedSetting = (SOLA_FILE_NAME_SETTING*)GuiMemAlloc(sizeof(SOLA_FILE_NAME_SETTING))) )
	{
		return -1;
	}
	if (!(pCalibrationPara = (CALIBRATIONPARA*)GuiMemAlloc(sizeof(CALIBRATIONPARA))))
	{
		return -1;
	}
    //清空数据结构体
    memset(pCurSystemSet, 0, sizeof(SYSTEMSET));
    memset(pCurWiFiSet, 0, sizeof(WIFISET));
    memset(pCurSN, 0, sizeof(SerialNum));
    memset(pFactoryConf, 1, sizeof(FactoryConfig));
    memset(pBmpFileNamedSetting, 0, sizeof(SOLA_FILE_NAME_SETTING));
    memset(pCalibrationPara, 0, sizeof(CALIBRATIONPARA));
#ifdef EEPROM_DATA
    GetSettingsData((void*)pCurSystemSet, sizeof(SYSTEMSET), SYSTEM_SET);
    GetSettingsData((void*)pCurWiFiSet, sizeof(WIFISET), WIFI_SET);
    GetSettingsData((void*)pCurSN, sizeof(SerialNum), SERIAL_NUM);
    GetSettingsData((void*)pFactoryConf, sizeof(FactoryConfig), FACTORY_CONFIG);     
    GetSettingsData((void*)pBmpFileNamedSetting, sizeof(SOLA_FILE_NAME_SETTING), BMP_FILENAME_SET);
	GetSettingsData((void*)pCalibrationPara, sizeof(CALIBRATIONPARA), CALIBRATION_PARA);//获取校准参数
#endif
#if 0
    FILE *fd = fopen(MntUsbDirectory"/system_parameter.txt","a+");
    printSerialNumber(pCurSN, fd);
    printSystemSet(pCurSystemSet, fd);
    printWifiSet(pCurWiFiSet, fd);
    printFactoryConfig(pFactoryConf, fd);
    printCalibrationPara(pCalibrationPara, fd);
    fclose(fd);
#endif
	/* 防止某些意外情况下eeprom中语言的索引超出合法的范围 */
	#ifdef VersionB
	{
		pCurSystemSet->iLanguageSelect = LANG_ENGLISH;
		SetCurrLanguage(pCurSystemSet->lCurrentLanguage);
	}	
	#else
    {
        if(pCurSystemSet->lCurrentLanguage<LANG_TYPE && pCurSystemSet->lCurrentLanguage>=0)
        {
            SetCurrLanguage(pCurSystemSet->lCurrentLanguage);
        }
        else
		{
            pCurSystemSet->iLanguageSelect = LANG_CHINASIM;
			SetCurrLanguage(pCurSystemSet->lCurrentLanguage);
        }
    }
	#endif
	
    /* 防止某些意外情况下eeprom中亮度值设为0 */
	if(pCurSystemSet->uiACLCDBright==0)
	{
		pCurSystemSet->iACLCDBrightSelect = 2;
		pCurSystemSet->uiACLCDBright      = 45;
	}
    /* 防止某些意外情况下eeprom中亮度值设为0 */
	if(pCurSystemSet->uiDCLCDBright==0)
	{
		pCurSystemSet->iDCLCDBrightSelect = 2;
		pCurSystemSet->uiDCLCDBright      = 45;
	}

    /* 防止某些意外情况下eeprom中皮肤索引不合法 */
	if(pCurSystemSet->ucSkinStyle >=SKIN_NUM || pCurSystemSet->ucSkinStyle < 0)
	{
		pCurSystemSet->ucSkinStyle = SKIN_A;
	}
    SetSkin(pCurSystemSet->ucSkinStyle);

    /* 防止某些意外情况下eeprom中国家style索引不合法 */
	if(pCurSystemSet->uiCountryStyle >= LANG_TYPE || pCurSystemSet->uiCountryStyle < 0)
	{
		pCurSystemSet->uiCountryStyle = LANG_CHINASIM;
	}
    setCountryStyle(pCurSystemSet->uiCountryStyle);

    if(pCurSystemSet->ucKeyWarning > 1 || pCurSystemSet->ucKeyWarning < 0)
	{
		pCurSystemSet->ucKeyWarning = 0;
	}

    if(pCurSystemSet->uiWarning > 1 || pCurSystemSet->uiWarning < 0)
	{
		pCurSystemSet->uiWarning = 1;
	}

    if(pCurSystemSet->uiACPowerOffValue != 0 && pCurSystemSet->uiACPowerOffValue != 60
		&& pCurSystemSet->uiACPowerOffValue != 300 && pCurSystemSet->uiACPowerOffValue != 600)
	{
		pCurSystemSet->uiACPowerOffValue = 0;
	}

    if(pCurSystemSet->uiDCPowerOffValue != 0 && pCurSystemSet->uiDCPowerOffValue != 60
		&& pCurSystemSet->uiDCPowerOffValue != 300 && pCurSystemSet->uiDCPowerOffValue != 600)
	{
		pCurSystemSet->uiDCPowerOffValue = 0;
	}

    if(pCurSystemSet->uiACScreenOffValue != 0 && pCurSystemSet->uiACScreenOffValue != 10
		&& pCurSystemSet->uiACScreenOffValue != 30 && pCurSystemSet->uiACScreenOffValue != 60
		&& pCurSystemSet->uiACScreenOffValue != 300)
	{
		pCurSystemSet->uiACScreenOffValue = 300;
	}

    if(pCurSystemSet->uiDCScreenOffValue != 0 && pCurSystemSet->uiDCScreenOffValue != 10
		&& pCurSystemSet->uiDCScreenOffValue != 30 && pCurSystemSet->uiDCScreenOffValue != 60
		&& pCurSystemSet->uiDCScreenOffValue != 300)
	{
		pCurSystemSet->uiDCScreenOffValue = 300;
	}
		
	return iReturn;
}


int CheckSystemSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    PSYSTEMSET pSystemSet = (PSYSTEMSET)buff;
    if(pSystemSet->uiACLCDBright == 0 ||
       pSystemSet->uiDCLCDBright == 0 )
    {
        return -1;
    }
    
    return 0;
}

void ResetSystemSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    PSYSTEMSET pSystemSet = (PSYSTEMSET)buff;
    
	pSystemSet->iACLCDBrightSelect = 2;
	pSystemSet->iACPowerSaveSelect = 0;
	pSystemSet->iDCLCDBrightSelect = 2;
	pSystemSet->iDCPowerSaveSelect = 0;
	pSystemSet->iLanguageSelect	   = 1;
	pSystemSet->lCurrentLanguage   = LANG_ENGLISH;
	pSystemSet->uiWarning          = 1;
	pSystemSet->ucKeyWarning       = 1;
	pSystemSet->uiACLCDBright      = 45;
	pSystemSet->uiACPowerOffValue  = 0;
	pSystemSet->uiACScreenOffValue = 300;
	pSystemSet->uiDCLCDBright      = 45;
	pSystemSet->uiDCPowerOffValue  = 0;
	pSystemSet->uiDCScreenOffValue = 300;
    pSystemSet->uiTimeShowFlag     = 0;
	pSystemSet->ucSkinStyle = SKIN_A;

    SaveSettings(SYSTEM_SET);
    return;
}

int CheckWifiSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }

    return 0;
}

void ResetWifiSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    PWIFISET pWifiSet = (PWIFISET)buff;
    
	pWifiSet->iWiFiConnectSta    = 0;
	pWifiSet->iWiFiOpenSta       = 0;
	strcpy(pWifiSet->cPassWord, "");
	strcpy(pWifiSet->cSSID, "");

    SaveSettings(WIFI_SET);
    return;
}

int CheckSerialNumSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    PSerialNum pSerialNumSet = (PSerialNum)buff;
    char SN[20] = {0};
    
    GenerateSerialNumber(SN, pSerialNumSet);

    if(strlen(SN) != 12)
    {
        return -1;
    }
    
    return 0;
}

void ResetSerialNumSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    //这是一个补救，eeprom重构前该项目该结构体机器类型的数组大小为5，但从215合并过来的时候改成4了，所以从eeprom重构前
    //版本升级上来，会出现序列号错误的BUG。
    struct serial_number
    {
        char MachineName[5];
        char MachineType[5];
        char Month[3];
        char Year[3];
        char Date[3];
        char Number[4];
        char Address[2];	
        unsigned char	ucReserved[99];
        unsigned int	ulCrc32;
    }oldSerialNum;
    
    PSerialNum pSerialNumSet = (PSerialNum)buff;
    
    GetEEpromData((void*)&oldSerialNum, sizeof(SerialNum), SERIALNUM_ADR);
    unsigned int crc = crc32c((unsigned char *)&oldSerialNum,SERIALNUMVALIDBYTE);
    
    if(crc == oldSerialNum.ulCrc32 && crc)
    {
        strcpy(pSerialNumSet->MachineName, oldSerialNum.MachineName);
        strcpy(pSerialNumSet->MachineType, oldSerialNum.MachineType);
        strcpy(pSerialNumSet->Month, oldSerialNum.Month);
        strcpy(pSerialNumSet->Year, oldSerialNum.Year);
        strcpy(pSerialNumSet->Date, oldSerialNum.Date);
        strcpy(pSerialNumSet->Number, oldSerialNum.Number);
        strcpy(pSerialNumSet->Address, oldSerialNum.Address);
        printSerialNumber(pSerialNumSet, stdout);
        SerialNumTransform(pSerialNumSet);
        SaveSettings(SERIAL_NUM);
    }
    else
    {
        GetEEpromData((void*)&oldSerialNum, sizeof(SerialNum), SERIALNUM_ADR+PARAMETERPAR1_ADR);
        crc = crc32c((unsigned char *)&oldSerialNum,SERIALNUMVALIDBYTE);
        if(crc == oldSerialNum.ulCrc32 && crc)
        {
            strcpy(pSerialNumSet->MachineName, oldSerialNum.MachineName);
            strcpy(pSerialNumSet->MachineType, oldSerialNum.MachineType);
            strcpy(pSerialNumSet->Month, oldSerialNum.Month);
            strcpy(pSerialNumSet->Year, oldSerialNum.Year);
            strcpy(pSerialNumSet->Date, oldSerialNum.Date);
            strcpy(pSerialNumSet->Number, oldSerialNum.Number);
            strcpy(pSerialNumSet->Address, oldSerialNum.Address);
            printSerialNumber(pSerialNumSet, NULL);
            printSerialNumber(pSerialNumSet, stdout);
            SerialNumTransform(pSerialNumSet);
            SaveSettings(SERIAL_NUM);
        }
        else
        {
            sprintf(pSerialNumSet->MachineName, "I");
            sprintf(pSerialNumSet->MachineType, "01");
            sprintf(pSerialNumSet->Month, "A");
            sprintf(pSerialNumSet->Year, "17");
            sprintf(pSerialNumSet->Date, "01");
            sprintf(pSerialNumSet->Number, "000");
            sprintf(pSerialNumSet->Address, "1");
        }
    }
    
    printSerialNumber(pSerialNumSet, stdout);
    return;
}

int CheckFactoryConfigSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }    
    //该区域的数据很重要，只做检查调整，不reset。

    PFactoryConfig pFactoryConfigSet = (PFactoryConfig)buff;
    unsigned long long temp = 0x3ffffffffLL;

    if((pFactoryConfigSet->ulAvailableLanguage & temp) == 0)
    {
        pFactoryConfigSet->ulAvailableLanguage = 0b10;
    }

    if(pFactoryConfigSet->lastDate.year >= 1000)
    {
        pFactoryConfigSet->lastDate.year = 0;
    }
    if(pFactoryConfigSet->lastDate.month >= 12)
    {
        pFactoryConfigSet->lastDate.month = 0;
    }
    if(pFactoryConfigSet->lastDate.day >= 31)
    {
        pFactoryConfigSet->lastDate.day = 0;
    }

    if(pFactoryConfigSet->currentDate.year >= 1000)
    {
        pFactoryConfigSet->currentDate.year = 0;
    }
    if(pFactoryConfigSet->currentDate.month >= 12)
    {
        pFactoryConfigSet->currentDate.month = 0;
    }
    if(pFactoryConfigSet->currentDate.day >= 31)
    {
        pFactoryConfigSet->currentDate.day = 0;
    }
    
    if(pFactoryConfigSet->iDynamicRange > 50)
    {
        pFactoryConfigSet->iDynamicRange = 30;
    }
    
    if(pFactoryConfigSet->speakerFreq < 1000
        || pFactoryConfigSet->speakerFreq > 10000)
    {
        pFactoryConfigSet->speakerFreq = 2600;
    }
    
    return 0;
}

void ResetFactoryConfigSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    
    //reset只在eeprom数据为空时调用
    
    PFactoryConfig pFactoryConfigSet = (PFactoryConfig)buff;
    FactoryConfig FactoryConfigSet;

    //从eeprom重构前的位置读数据
    GetEEpromData((void*)&FactoryConfigSet, sizeof(FactoryConfig), FACTORYCONFIG_ADR);
    unsigned int crc = crc32c((unsigned char *)&FactoryConfigSet,FACTORYCONFIGVALIDBYTE);
    if((crc == FactoryConfigSet.ulCrc32) && crc)
    {
        FactoryConfigSet.enabled = 0xff;
        if(FactoryConfigSet.ulAvailableLanguage == 0)
        {
            FactoryConfigSet.ulAvailableLanguage = 0b10;
        }
        FactoryConfigSet.speakerFreq = 2600;
        memcpy(pFactoryConfigSet, &FactoryConfigSet, sizeof(FactoryConfig));
        SaveSettings(FACTORY_CONFIG);
    }
    else
    {
        GetEEpromData((void*)&FactoryConfigSet, sizeof(FactoryConfig), FACTORYCONFIG_ADR+PARAMETERPAR1_ADR);
        unsigned int crc = crc32c((unsigned char *)&FactoryConfigSet,FACTORYCONFIGVALIDBYTE);
        if((crc == FactoryConfigSet.ulCrc32) && crc)
        {
            FactoryConfigSet.enabled = 0xff;
            if(FactoryConfigSet.ulAvailableLanguage == 0)
            {
                FactoryConfigSet.ulAvailableLanguage = 0b10;
            }
            FactoryConfigSet.speakerFreq = 2600;
            memcpy(pFactoryConfigSet, &FactoryConfigSet, sizeof(FactoryConfig));
            SaveSettings(FACTORY_CONFIG);
        }
        else
        {
            pFactoryConfigSet->iDynamicRange = 30;
            pFactoryConfigSet->ulAvailableLanguage = 0b10;
            pFactoryConfigSet->lastDate.year = 0;
            pFactoryConfigSet->lastDate.month = 0;
            pFactoryConfigSet->lastDate.day = 0;
            pFactoryConfigSet->currentDate.year = 0;
            pFactoryConfigSet->currentDate.month = 0;
            pFactoryConfigSet->currentDate.day = 0;
            pFactoryConfigSet->enabled = 0xff;
            pFactoryConfigSet->speakerFreq = 2600;
        }
    }

    return;
}

int CheckInstallmentSet(void* buff)
{
	if(!buff)
    {
        return -1;
    }

	PINSTALLMENT pInst = (PINSTALLMENT)buff;
    
    return InstIsValid(pInst);
}

void ResetInstallmentSet(void* buff)
{
	if(!buff)
    {
        return;
    }
	PINSTALLMENT pInst = (PINSTALLMENT)buff;
	InstMemset(pInst);
	SaveSettings(INSTALLMENT_SET);
}

int CheckBmpFileNameSet(void* buff)
{
    if(!buff)
    {
        return -1;
    }
    
    SOLA_FILE_NAME_SETTING* pBmpFileNameSet = (SOLA_FILE_NAME_SETTING*)buff;
    
    if(pBmpFileNameSet->prefix[0] == 0
        || pBmpFileNameSet->suffixWidth > 5 
        || pBmpFileNameSet->suffixWidth < 3
        || pBmpFileNameSet->suffixRule > 1
        || pBmpFileNameSet->fileFormat != 1
        )
    {
        return -1;
    }
    return 0;
}

void ResetBmpFileNameSet(void* buff)
{
    if(!buff)
    {
        return;
    }
    
    SOLA_FILE_NAME_SETTING* pBmpFileNameSet = (SOLA_FILE_NAME_SETTING*)buff;
    	
	strcpy(pBmpFileNameSet->prefix, "BMP");
	pBmpFileNameSet->suffix = 1;
	pBmpFileNameSet->suffixWidth = 3;
	pBmpFileNameSet->suffixRule = 1;
	pBmpFileNameSet->fileFormat = 1;   //BMP
	
    SaveSettings(BMP_FILENAME_SET);
    
    return;
}

//校准数据
int CheckCalibrationPara(void* buff)
{
	if (!buff)
	{
		return -1;
	}
/*
	PCALIBRATIONPARA pCalibrationPara = (PCALIBRATIONPARA)buff;
	int i = 0;
	int j = 0;

	for (; i < CALIBRATION_WAVE_NUM; ++i)
	{
		for (j = 0; j < CALIBRATION_CHANNEL_NUM; ++j)
		{
			if (pCalibrationPara->channelPara[i].k[j] < 0 
				|| pCalibrationPara->channelPara[i].b[j] < 0)
			{
				return -2;
			}
		}
	}
*/
	return 0;
}

void ResetCalibrationPara(void* buff)
{
	if (!buff)
	{
		return;
	}

	PCALIBRATIONPARA pCalibrationPara = (PCALIBRATIONPARA)buff;
	int i = 0;
	int j = 0;

	for (; i < CALIBRATION_WAVE_NUM; ++i)
	{
		for (j = 0; j < CALIBRATION_CHANNEL_NUM; ++j)
		{
			pCalibrationPara->channelPara[i].k[j] = 0.0f;
			pCalibrationPara->channelPara[i].b[j] = 0.0f;
		}
	}

	SaveSettings(CALIBRATION_PARA);
}

/*生成char*格式的序列号*/
int GenerateSerialNumber(char* pDest, const SerialNum* pSerialNum)
{
    if (pDest && pSerialNum)
    {
        sprintf(pDest, "%s%s%s%s%s%s%s",
                pSerialNum->MachineName,
                pSerialNum->Month,
                pSerialNum->Year,
                pSerialNum->Date,
                pSerialNum->Number,
                pSerialNum->MachineType,
                pSerialNum->Address);
        return 0;
    }

    return -1;
}

void SerialNumTransform(PSerialNum pSerialNumSet)
{
    if(!pSerialNumSet)
    {
        return;
    }
    if(pSerialNumSet->Month[0] < 'A' 
       || pSerialNumSet->Month[0] > 'L' 
       || pSerialNumSet->MachineName[0] != 'I')
    {
        char *type[] = {"216", "215", "218", "216P", "213", "225", "210"};
        int i;
        int m = atoi(pSerialNumSet->Month);
        pSerialNumSet->Month[0] = 'A'+m-1;
        pSerialNumSet->Month[1] = 0;

        pSerialNumSet->MachineName[0] = 'I';
        pSerialNumSet->MachineName[1] = 0;
        for(i = 0; i < 7; i++)
        {
            if(!strcmp(type[i],pSerialNumSet->MachineType) || (type[i][3] != 0))
            {
                sprintf(pSerialNumSet->MachineType,"%02d",i+1);
                break;
            }
        }
        sprintf(pSerialNumSet->Address,"%1d",(pSerialNumSet->Address[0] - 'A' + 1));
    }
  
    return;
}

//以下测试打印eeprom写入的数据
//测试校准数据
void printCalibrationPara(const PCALIBRATIONPARA pCalibrationPara, FILE *target)
{
    if (target && pCalibrationPara)
    {
        fprintf(target, "***************************Calibration Para****************************\n");
        int i;
        int j;
        for (i = 0; i < CALIBRATION_WAVE_NUM; i++)
        {
            for (j = 0; j < CALIBRATION_CHANNEL_NUM; j++)
            {
                fprintf(target, "k:%f\nb:%f\n", pCalibrationPara->channelPara[i].k[j],
                        pCalibrationPara->channelPara[i].b[j]);
                printf("wave:%d;channel:%d;k:%f;b:%f\n",i,j, pCalibrationPara->channelPara[i].k[j],
                        pCalibrationPara->channelPara[i].b[j]);
            }
        }
    }
}

//测试Frontset
void printFrontSet(const PFRONT_SETTING pFrontSet, FILE *target)
{
    if (target && pFrontSet)
    {
        fprintf(target, "***************************Front set****************************\n");
        fprintf(target, "enTestMode:%d\nenFiberRange:%d\nenPulseTime:%d\nenAverageTime:%d\n",
                pFrontSet->enTestMode, pFrontSet->enFiberRange, pFrontSet->enPulseTime, pFrontSet->enAverageTime);
    }
}

//测试commonset
void printCommonSet(const PCOMMON_SETTING pCommonSet, FILE *target)
{
    if (target && pCommonSet)
    {
        fprintf(target, "***************************Common set****************************\n");
        fprintf(target, "iAutoSaveFlag:%d\niAutoSpanFlag:%d\niPromptSaveFlag:%d\niDisplaySectionEvent:%d\n"
                        "iConectCheckFlag:%d\niLightCheckFlag:%d\niMacroBendingFlag:%d\niLossMethodMark:%d\n"
                        "iAtenuMethodMark:%d\niOTDRFileNameFlag:%d\niCusorLblFlag:%d\niPreviewAreaFlag:%d\n"
                        "iScreenShotFlag:%d\niAutoJumpWindow:%d\niOnlyShowCurrCurve:%d\niUnitConverterFlag:%d\n",
                pCommonSet->iAutoSaveFlag,
                pCommonSet->iAutoSpanFlag,
                pCommonSet->iPromptSaveFlag,
                pCommonSet->iDisplaySectionEvent,
                
                pCommonSet->iConectCheckFlag,
                pCommonSet->iLightCheckFlag,
                pCommonSet->iMacroBendingFlag,
                pCommonSet->iLossMethodMark,

                pCommonSet->iAtenuMethodMark,
                pCommonSet->iOTDRFileNameFlag,
                pCommonSet->iCusorLblFlag,
                pCommonSet->iPreviewAreaFlag,

                pCommonSet->iScreenShotFlag,
                pCommonSet->iAutoJumpWindow,
                pCommonSet->iOnlyShowCurrCurve,
                pCommonSet->iUnitConverterFlag
        );
    }
}

//测试sampleset
void printSampleSet(const PSAMPLE_SETTING pSampleSet, FILE *target)
{
    if (target && pSampleSet)
    {
        fprintf(target, "***************************Sample set****************************\n");
        fprintf(target, "enAutoAvrTime:%d\nenSmpResolution:%d\n", pSampleSet->enAutoAvrTime, pSampleSet->enSmpResolution);
        int i;
        for (i = 0; i < 3; i++)
        {
            fprintf(target, "fBackScattering:%f\nfExcessLength:%f\nfRefractiveIndex:%f\nfMacroDaltaThr:%f\n",
                    pSampleSet->sWaveLenArgs[i].fBackScattering, pSampleSet->sWaveLenArgs[i].fExcessLength,
                    pSampleSet->sWaveLenArgs[i].fRefractiveIndex, pSampleSet->sWaveLenArgs[i].fMacroDaltaThr);
        }
    }
}

//测试analysisSet
void printAnalysisSet(const PANALYSIS_SETTING pAnalysisSet, FILE *target)
{
    if (target && pAnalysisSet)
    {
        fprintf(target, "***************************Analysis set****************************\n");
        fprintf(target, "fRecvFiberLen:%f\nfRecvFiberLen_ft:%f\nfRecvFiberLen_mi:%f\nfReturnLossThr:%f\n"
                        "fSpliceLossThr:%f\nfLaunchFiberLen:%f\nfLaunchFiberLen_ft:%f\nfLaunchFiberLen_mi:%f\n"
                        "iAutoAnalysisFlag:%d\niEndEventNum:%d\nfEndLossThr:%f\niEnableRecvFiber:%d\n"
                        "iEndPositionFlag:%d\niStartEventNum:%d\niEnableLaunchFiber:%d\niEnableLaunchFiberEvent:%d\n"
                        "iEnableRecvFiberEvent:%d\niLaunchFiberEvent:%d\niRecvFiberEvent:%d\niEventOrLength:%d\niEnableAnalysisThreshold:%d\n",
                pAnalysisSet->fRecvFiberLen,
                pAnalysisSet->fRecvFiberLen_ft,
                pAnalysisSet->fRecvFiberLen_mi,
                pAnalysisSet->fReturnLossThr,

                pAnalysisSet->fSpliceLossThr,
                pAnalysisSet->fLaunchFiberLen,
                pAnalysisSet->fLaunchFiberLen_ft,
                pAnalysisSet->fLaunchFiberLen_mi,

                pAnalysisSet->iAutoAnalysisFlag,
                pAnalysisSet->iEndEventNum,
                pAnalysisSet->fEndLossThr,
                pAnalysisSet->iEnableRecvFiber,

                pAnalysisSet->iEndPositionFlag,
                pAnalysisSet->iStartEventNum,
                pAnalysisSet->iEnableLaunchFiber,
                pAnalysisSet->iEnableLaunchFiberEvent,

                pAnalysisSet->iEnableRecvFiberEvent,
                pAnalysisSet->iLaunchFiberEvent,
                pAnalysisSet->iRecvFiberEvent,
                pAnalysisSet->iEventOrLength,

                pAnalysisSet->iEnableAnalysisThreshold
        );
    }
}

//测试结果设置数据
void printResultSet(const POTHER_SETTING pOtherSet, FILE *target)
{
    if (target && pOtherSet)
    {
        fprintf(target, "***************************Other set****************************\n");
        fprintf(target, "iDisplayPassInfoFlag:%d\niWaveLength:%d\n", pOtherSet->iDisplayPassInfoFlag, pOtherSet->iWaveLength);
        int i;
        for ( i = 0; i < WAVE_NUM; i++)
        {
            fprintf(target, "SpliceLossThr:iSlectFlag:%d\nfThrValue:%f\n",
                    pOtherSet->PassThr.SpliceLossThr[i].iSlectFlag,
                    pOtherSet->PassThr.SpliceLossThr[i].fThrValue);

            fprintf(target, "ConnectLossThr:iSlectFlag:%d\nfThrValue:%f\n",
                    pOtherSet->PassThr.ConnectLossThr[i].iSlectFlag,
                    pOtherSet->PassThr.ConnectLossThr[i].fThrValue);

            fprintf(target, "ReturnLossThr:iSlectFlag:%d\nfThrValue:%f\n",
                    pOtherSet->PassThr.ReturnLossThr[i].iSlectFlag,
                    pOtherSet->PassThr.ReturnLossThr[i].fThrValue);

            fprintf(target, "AttenuationThr:iSlectFlag:%d\nfThrValue:%f\n",
                    pOtherSet->PassThr.AttenuationThr[i].iSlectFlag,
                    pOtherSet->PassThr.AttenuationThr[i].fThrValue);

            fprintf(target, "RegionLossThr:iSlectFlag:%d\nfThrValue:%f\n",
                    pOtherSet->PassThr.RegionLossThr[i].iSlectFlag,
                    pOtherSet->PassThr.RegionLossThr[i].fThrValue);

            fprintf(target, "RegionLengthThr:iSlectFlag:%d\nfThrValue:%f\n",
                    pOtherSet->PassThr.RegionLengthThr[i].iSlectFlag,
                    pOtherSet->PassThr.RegionLengthThr[i].fThrValue);

            fprintf(target, "RegionReturnLossThr:iSlectFlag:%d\nfThrValue:%f\n",
                    pOtherSet->PassThr.RegionReturnLossThr[i].iSlectFlag,
                    pOtherSet->PassThr.RegionReturnLossThr[i].fThrValue);

            fprintf(target, "fSpanLength_m:%f\nfSpanLength_ft:%f\nfSpanLength_mi:%fiThrEnable:%d\n",
                    pOtherSet->PassThr.fSpanLength_m[i],
                    pOtherSet->PassThr.fSpanLength_ft[i],
                    pOtherSet->PassThr.fSpanLength_mi[i],
                    pOtherSet->PassThr.iThrEnable[i]);
        }
    }
}

void printSystemSet(const PSYSTEMSET pSystemSet, FILE *target)
{
    if(target && pSystemSet)
    {
        fprintf(target,"***************************System Set****************************\n");
        fprintf(target,"pSystemSet->lCurrentLanguage = %ld\n",pSystemSet->lCurrentLanguage);
        fprintf(target,"pSystemSet->iLanguageSelect = %d\n",pSystemSet->iLanguageSelect);
        fprintf(target,"pSystemSet->uiWarning = %d\n",pSystemSet->uiWarning);
        fprintf(target,"pSystemSet->uiACPowerOffValue = %d\n",pSystemSet->uiACPowerOffValue);
        fprintf(target,"pSystemSet->uiACScreenOffValue = %d\n",pSystemSet->uiACScreenOffValue);
        fprintf(target,"pSystemSet->uiACLCDBright = %d\n",pSystemSet->uiACLCDBright);
        fprintf(target,"pSystemSet->uiDCPowerOffValue = %d\n",pSystemSet->uiDCPowerOffValue);
        fprintf(target,"pSystemSet->uiDCScreenOffValue = %d\n",pSystemSet->uiDCScreenOffValue);
        fprintf(target,"pSystemSet->uiDCLCDBright = %d\n",pSystemSet->uiDCLCDBright);
        fprintf(target,"pSystemSet->iACPowerSaveSelect = %d\n",pSystemSet->iACPowerSaveSelect);
        fprintf(target,"pSystemSet->iACLCDBrightSelect = %d\n",pSystemSet->iACLCDBrightSelect);
        fprintf(target,"pSystemSet->iDCPowerSaveSelect = %d\n",pSystemSet->iDCPowerSaveSelect);
        fprintf(target,"pSystemSet->iDCLCDBrightSelect = %d\n",pSystemSet->iDCLCDBrightSelect);
        fprintf(target,"pSystemSet->cSerialNumber = %s\n",pSystemSet->cSerialNumber);
        fprintf(target,"pSystemSet->cWaveLength1 = %s\n",pSystemSet->cWaveLength1);
        fprintf(target,"pSystemSet->cWaveLength2 = %s\n",pSystemSet->cWaveLength2);
        fprintf(target,"pSystemSet->cDate = %s\n",pSystemSet->cDate);
        fprintf(target,"pSystemSet->ucKeyWarning = %d\n",pSystemSet->ucKeyWarning);
        fprintf(target,"pSystemSet->ucSkinStyle = %d\n",pSystemSet->ucSkinStyle);
        fprintf(target,"pSystemSet->uiTimeShowFlag = %d\n",pSystemSet->uiTimeShowFlag);
    }
}

void printWifiSet(const PWIFISET pWifiSet, FILE *target)
{
    if(target && pWifiSet)
    {
        fprintf(target,"***************************Wifi Set****************************\n");
        fprintf(target,"pWifiSet->iWiFiOpenSta = %d\n",pWifiSet->iWiFiOpenSta);
        fprintf(target,"pWifiSet->iWiFiConnectSta = %d\n",pWifiSet->iWiFiConnectSta);
        fprintf(target,"pWifiSet->cSSID = %s\n",pWifiSet->cSSID);
        fprintf(target,"pWifiSet->cPassWord = %s\n",pWifiSet->cPassWord);
        fprintf(target,"pWifiSet->cFlags = %d\n",pWifiSet->cFlags);
    }
}

//测试lightsource
void printLightSource(const SOURCELIGHTCONFIG *pSourceLight, FILE *target)
{
    if (target && pSourceLight)
    {
        fprintf(target, "***************************lightSource Set****************************\n");
        fprintf(target, "pSourceLight->frequence = %d\n", pSourceLight->frequence);
        fprintf(target, "pSourceLight->wavelength = %d\n", pSourceLight->wavelength);
    }
}

//测试otdrMarkSet
void printOtdrMarkSet(const MarkParam *pMarkParam, FILE *target)
{
    if (target && pMarkParam)
    {
        fprintf(target, "***************************mark Set****************************\n");
        fprintf(target, "FiberMark:%s\nCableMark:%s\nLocation_A:%s\nLocation_B:%s\nDeraction:%d\n"
                        "Test_date:%s\nTest_time:%s\nModule:%s\nSerial_num:%s\n"
                        "TaskID:%s\nOperator_A:%s\nOperator_B:%s\nCompany:%s\nCustomer:%s\nNote:%s\nTaskID_Unicode:%s\n",
                pMarkParam->FiberMark,
                pMarkParam->CableMark,
                pMarkParam->Location_A,
                pMarkParam->Location_B,
                pMarkParam->Deraction,

                pMarkParam->Test_date,
                pMarkParam->Test_time,
                pMarkParam->Module,
                pMarkParam->Serial_num,

                pMarkParam->TaskID,
                pMarkParam->Operator_A,
                pMarkParam->Operator_B,
                pMarkParam->Company,
                pMarkParam->Customer,
                pMarkParam->Note,
                pMarkParam->TaskID_Unicode);
    }
}

void printSerialNumber(const SerialNum *pSerialNum, FILE *target)
{
    if (target && pSerialNum)
    {
        fprintf(target, "***************************Serial Number****************************\n");
        fprintf(target, "MachineName:%s\nMonth:%s\nYear:%s\nDate:%s\nNumber:%s\nMachineType:%s\nAddress:%s\n",
                pSerialNum->MachineName,
                pSerialNum->Month,
                pSerialNum->Year,
                pSerialNum->Date,
                pSerialNum->Number,
                pSerialNum->MachineType,
                pSerialNum->Address);
    }
}

void printFactoryConfig(const PFactoryConfig pFactoryConfig, FILE *target)
{
    if(target && pFactoryConfig)
    {
        fprintf(target,"***************************Factory Config****************************\n");
        fprintf(target,"pFactoryConfig->iDynamicRange = %d\n",pFactoryConfig->iDynamicRange);
        fprintf(target,"pFactoryConfig->ulAvailableLanguage = %lld\n",pFactoryConfig->ulAvailableLanguage);
        fprintf(target,"pFactoryConfig->lastDate.year = %d\n",pFactoryConfig->lastDate.year);
        fprintf(target,"pFactoryConfig->lastDate.month = %d\n",pFactoryConfig->lastDate.month);
        fprintf(target,"pFactoryConfig->lastDate.day = %d\n",pFactoryConfig->lastDate.day);
        fprintf(target,"pFactoryConfig->currentDate.year = %d\n",pFactoryConfig->currentDate.year);
        fprintf(target,"pFactoryConfig->currentDate.month = %d\n",pFactoryConfig->currentDate.month);
        fprintf(target,"pFactoryConfig->currentDate.day = %d\n",pFactoryConfig->currentDate.day);
        fprintf(target,"pFactoryConfig->enabled = 0x%x\n",pFactoryConfig->enabled);
    }
}

/*********************************************************************************************************
* End File
*********************************************************************************************************/
