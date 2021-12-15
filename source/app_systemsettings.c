/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_systemsettings.c
* 摘    要：  实现EEPROM对设置的存储
*
* 当前版本：  v1.0.0  
* 作    者：  
* 完成日期：  
*
* 取代版本： 
* 作    者：  
* 完成日期：  
*******************************************************************************/
#include "app_systemsettings.h"
#include "app_iiceeprom.h"
#include "app_frminit.h"
#include "app_getsetparameter.h"

#define EEPROM_DEBUG        0

typedef struct _eeprom_block_info
{
    char *name;
    int size;
    int (*check)(void*);
    void (*result)(void*);
}EEPROM_BLOCK_INFO;

//分区表不要随意更改。
EEPROM_BLOCK_INFO part1BlockInfo[] = { {CALIBRATION_PARA,896, CheckCalibrationPara, ResetCalibrationPara} };
                                     
EEPROM_BLOCK_INFO part2BlockInfo[] = {
                                        {FRONTSET_1310, 256, CheckFrontSet_1310, ResetFrontSet_1310},
                                        {FRONTSET_1550, 256, CheckFrontSet_1550, ResetFrontSet_1550},
                                        {FRONTSET_1625, 256, CheckFrontSet_1625, ResetFrontSet_1625},
                                        {COMMON_SET, 256, CheckCommonSet, ResetCommonSet},
                                        {SAMPLE_SET, 256, CheckSampleSet, ResetSampleSet},
                                        {ANALYSIS_SET, 256, CheckAnalysisSet, ResetAnalysisSet},
                                        {RESULT_SET, 384, CheckResultSet, ResetResultSet},
                                        {FILENAME_SET, 256, CheckFileNameSet, ResetFileNameSet},
                                        {SYSTEM_SET, 256, CheckSystemSet, ResetSystemSet},
                                        {WIFI_SET,256,CheckWifiSet,ResetWifiSet},
                                        {SOLA_IDENTIFY_SET,128,CheckSolaIdentifySet,ResetSolaIdentifySet},
                                        {SOLA_LINEDEFINE_SET,256,CheckSolaLineDefineSet,ResetSolaLineDefineSet},
                                        {SOLA_LINEPASS_SET,256,CheckSolaLinePassSet,ResetSolaLinePassSet},
                                        {SOLA_ITEMPASS_SET,256,CheckSolaItemPassSet,ResetSolaItemPassSet},
                                        {LIGHTSOURCE, 128, CheckLightSourceSet, ResetLightSourceSet},
                                        {OTDR_SOLA_DEFSAVEPATH,512,CheckDefSavePathSet,ResetDefSavePathSet},
                                        {OTDR_MARK_SET, 1024, CheckOtdrMarkSet, ResetOtdrMarkSet},
                                        {SOLA_SETTING_PARA,128, CheckSolaSetting, ResetSolaSetting},
                                        {SOLA_FILENAME_SET,128, CheckSolaFileNameSet, ResetSolaFileNameSet},
                                        {SOLA_IDENT,1024, CheckSolaIdentSet, ResetSolaIdentSet},
                                        {BMP_FILENAME_SET,128, CheckBmpFileNameSet, ResetBmpFileNameSet}
                                     };
                                     
EEPROM_BLOCK_INFO part3BlockInfo[] = {
                                        {SERIAL_NUM,256,CheckSerialNumSet,ResetSerialNumSet},
                                        {FACTORY_CONFIG,256,CheckFactoryConfigSet,ResetFactoryConfigSet},
										{INSTALLMENT_SET,256,CheckInstallmentSet,ResetInstallmentSet},
                                        {LIGHTSOURCE_POWER_CONFIG_LIST,128,CheckLightSourcePowerSet,ResetLightSourcePowerSet}
                                     };

//对EEPROM进行分区(2000页 256KB), 总共分为3个区
static EEprom* part1 = NULL; //第一个区保留 总计 500页 64KB
static EEprom* part2 = NULL; //第二个区作为系统设置参数区域 总计 1000页 128KB
static EEprom* part3 = NULL; //第三个区作为只读数据区域 总计500页 64KB


static void PartitionEEprom()
{
    //EEPROM一共512页 64KB
    part1 = EEprom_newIicEEprom("/dev/i2c-1", 128, 512, "Part1:OpmCalibrationData");

    //从整个Part1中分出412页 43KB
    part2 = EEprom_partition(part1, 412, "Part2:SystemSettings");

    //再从part2中分出112页 14KB
    part3 = EEprom_partition(part2, 112, "Part3:ReadOnlyData");

    EEprom_open(part1);
    EEprom_open(part2);
    EEprom_open(part3);

#if (EEPROM_DEBUG == 1)
    EEprom_setDebug(part1, 1);
    EEprom_setDebug(part2, 1);
    EEprom_setDebug(part3, 1);
#endif
}


void LoadAllSettings()
{
    int i = 0;
    //先对EEPROM进行分区
    PartitionEEprom();

    //对part1 系统设置区映射
    for (i = 0; i < sizeof(part1BlockInfo)/sizeof(EEPROM_BLOCK_INFO); i++)
    {
        EEprom_mapBlockAtEnd(
            part1, part1BlockInfo[i].name, part1BlockInfo[i].size, 
            part1BlockInfo[i].result, part1BlockInfo[i].check
            );
    }
    
    //对part2 系统设置区映射
    for (i = 0; i < sizeof(part2BlockInfo)/sizeof(EEPROM_BLOCK_INFO); i++)
    {
        EEprom_mapBlockAtEnd(
            part2, part2BlockInfo[i].name, part2BlockInfo[i].size, 
            part2BlockInfo[i].result, part2BlockInfo[i].check
            );
    }
    
    //对part3 系统设置区映射
    for (i = 0; i < sizeof(part3BlockInfo)/sizeof(EEPROM_BLOCK_INFO); i++)
    {
        EEprom_mapBlockAtEnd(
            part3, part3BlockInfo[i].name, part3BlockInfo[i].size, 
            part3BlockInfo[i].result, part3BlockInfo[i].check
            );
    }
    
#if (EEPROM_DEBUG == 1)
    EEprom_print(part1, stdout);
    EEprom_print(part2, stdout);
    EEprom_print(part3, stdout);
#endif

    //读取part1 part2 part3的数据到映射的内存块中
    EEprom_readAllBlocks(part1);
    EEprom_readAllBlocks(part2);
    EEprom_readAllBlocks(part3);
}


void ResetAllSettings()
{
    //仅对part2进行reset并同步
    EEprom_resetAllBlock(part2);
    EEprom_syncAllBlocks(part2);
}


void SaveAllSettings()
{
    EEprom_syncAllBlocks(part2);
    EEprom_syncAllBlocks(part3);
}

void LoadSettings(const char* name)
{
    EEprom* parts[] = {part1, part2, part3};

    int i;
    for (i = 0; i < sizeof(parts) / sizeof(EEprom*); ++i)
    {
        int index = EEprom_indexOfBlock(parts[i], name);
        if (-1 != index)
        {
            EEprom_readBlock(parts[i], index);
            break;
        }
    }
}

void SaveSettings(const char* name)
{
    EEprom* parts[] = {part1, part2, part3};

    int i;
    for (i = 0; i < sizeof(parts) / sizeof(EEprom*); ++i)
    {
        int index = EEprom_indexOfBlock(parts[i], name);
        if (-1 != index)
        {
            EEprom_syncBlock(parts[i], index);
            break;
        }
    }
}

int SetSettingsData(void* buff, int size, const char* name)
{
    if(!buff || !size || !name)
    {
        return -1;
    }
    
    EEprom* parts[] = {part1, part2, part3};

    int i;
    for (i = 0; i < sizeof(parts) / sizeof(EEprom*); ++i)
    {
        int index = EEprom_indexOfBlock(parts[i], name);
        if (-1 != index)
        {
            return EEprom_setData(parts[i], buff, size, index); 
        }
    }
    
    return 0;
}

int GetSettingsData(void* buff, int size, const char* name)
{
    if(!buff || !size || !name)
    {
        return -1;
    }
    
    EEprom* parts[] = {part1, part2, part3};

    int i;
    for (i = 0; i < sizeof(parts) / sizeof(EEprom*); ++i)
    {
        int index = EEprom_indexOfBlock(parts[i], name);
        if (-1 != index)
        {
            return EEprom_getData(parts[i], buff, size, index); 
        }
    }
    
    return 0;
}

int GetEEpromData(void* buff, unsigned int size, unsigned int absPageIndex)
{
    if(!buff)
        return -1;

    EEprom* parts[] = {part1, part2, part3};

    unsigned int pageSize = EEprom_pageSize(part1);
    int count = round_up(size, pageSize) / pageSize;

    void* readBuff = malloc(count * pageSize);
    if (readBuff)
    {
        int i;
        for (i = 0; i < sizeof(parts) / sizeof(EEprom*); ++i)
        {
            if (0 == EEprom_readAbsPage(parts[i], readBuff, absPageIndex, count))
            {    
                memcpy(buff, readBuff, size);
                free(readBuff);
                return 0;
            }
        }    
        free(readBuff);
    }
    
    return -1;
}

