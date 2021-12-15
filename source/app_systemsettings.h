/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_systemsettings.h
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

#ifndef _APP_SYSTEM_SETTINGS_H
#define _APP_SYSTEM_SETTINGS_H
//part1
#define CALIBRATION_PARA				"CalibrationPara"

//part2
#define FRONTSET_1310                   "FrontSet1310"
#define FRONTSET_1550                   "FrontSet1550"
#define FRONTSET_1625                   "FrontSet1625"
#define COMMON_SET                      "CommonSet"
#define SAMPLE_SET                      "SampleSet"
#define ANALYSIS_SET                    "AnalysisSet"
#define RESULT_SET                      "ResultSet"
#define FILENAME_SET                    "FileNameSet"
#define SYSTEM_SET                      "SystemSet"
#define WIFI_SET                        "WifiSet"
#define SOLA_IDENTIFY_SET               "SolaIdentify"
#define SOLA_LINEDEFINE_SET             "SolaLineDefine"
#define SOLA_LINEPASS_SET               "SolaLinePass"
#define SOLA_ITEMPASS_SET               "SolaItemPass"
#define LIGHTSOURCE                     "LightSource"
#define OTDR_SOLA_DEFSAVEPATH           "DefSavePath"
#define OTDR_MARK_SET                   "OtdrMarkSet"
#define SOLA_SETTING_PARA               "SolaSetting"
#define SOLA_FILENAME_SET               "SolaFilenameSetting"
#define SOLA_IDENT                      "SolaIdent"
#define LIGHTSOURCE_POWER_CONFIG_LIST   "LightSourcePowerConfigList"
#define BMP_FILENAME_SET                "BmpFilenameSetting"
//part3
#define SERIAL_NUM                      "SerialNum"
#define FACTORY_CONFIG                  "FactoryConfig"
#define INSTALLMENT_SET					"InstallmentSet"

void LoadAllSettings();
void ResetAllSettings();
void SaveAllSettings();

void LoadSettings(const char* name);
void SaveSettings(const char* name);

int SetSettingsData(void* buff, int size, const char* name);
int GetSettingsData(void* buff, int size, const char* name);

int GetEEpromData(void* buff, unsigned int size, unsigned int absPageIndex);

#endif
