/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frminit.h
* 摘    要：  定义用于完成OTDR模块功能的应用程序接口
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _APP_FRMINIT_H
#define _APP_FRMINIT_H


/*************************************
* 为定义APP_OTDR而需要引用的其他头文件
*************************************/
#include "app_middle.h"
#include "guibase.h"
#include "app_frmotdr.h"

//申明变量
extern POTDR_TOP_SETTINGS pOtdrTopSettings;	//当前OTDR的工作设置
extern PUSER_SETTINGS pUser_Settings;

int OtdrPlatformInit(void);
void PrintUserSetting(PUSER_SETTINGS pUserSettings);
int SetDefaultUserSetting(PUSER_SETTINGS pUserSettings);
//sola

int CheckFrontSet_1310(void* buff);
void ResetFrontSet_1310(void* buff);
int CheckFrontSet_1550(void* buff);
void ResetFrontSet_1550(void* buff);
int CheckFrontSet_1625(void* buff);
void ResetFrontSet_1625(void* buff);
int CheckCommonSet(void* buff);
void ResetCommonSet(void* buff);
int CheckSampleSet(void* buff);
void ResetSampleSet(void* buff);
int CheckAnalysisSet(void* buff);
void ResetAnalysisSet(void* buff);
int CheckResultSet(void* buff);
void ResetResultSet(void* buff);
int CheckFileNameSet(void* buff);
void ResetFileNameSet(void* buff);
int CheckSolaIdentifySet(void* buff);
void ResetSolaIdentifySet(void* buff);
int CheckSolaLineDefineSet(void* buff);
void ResetSolaLineDefineSet(void* buff);
int CheckSolaLinePassSet(void* buff);
void ResetSolaLinePassSet(void* buff);
int CheckSolaItemPassSet(void* buff);
void ResetSolaItemPassSet(void* buff);
int CheckLightSourceSet(void* buff);
void ResetLightSourceSet(void* buff);
int CheckLightSourcePowerSet(void* buff);
void ResetLightSourcePowerSet(void* buff);
int CheckDefSavePathSet(void* buff);
void ResetDefSavePathSet(void* buff);
int CheckOtdrMarkSet(void* buff);
void ResetOtdrMarkSet(void* buff);
int CheckSolaSetting(void* buff);
void ResetSolaSetting(void* buff);
int CheckSolaFileNameSet(void* buff);
void ResetSolaFileNameSet(void* buff);
int CheckSolaIdentSet(void* buff);
void ResetSolaIdentSet(void* buff);

#endif  //_APP_FRMOTDR_H

