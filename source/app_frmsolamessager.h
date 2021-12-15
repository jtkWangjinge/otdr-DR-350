/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmsloamessager.h
* 摘    要：  sloamessager结构体定义
*
* 当前版本：  v1.1.0
* 作    者：  
* 完成日期：  
*
*******************************************************************************/
#ifndef _APP_FRMSOLAMESSAGER_H
#define _APP_FRMSOLAMESSAGER_H

#include "app_sola.h"
#include "wnd_frmmark.h"

/*****************标识*********************/
typedef struct _sola_identify_item
{
  int auto_increment;
  int start;
  int stop;
  int setp;
}SOLA_IDENTIFY_ITEM;

/****************链路定义*******************/
typedef struct _identify_ {
	char cableID[16];
    char fiberID[16];
    char LocationA[16];
    char LocationB[16];
}solaIdentifySetting;

typedef struct _sola_identify
{
  SOLA_IDENTIFY_ITEM cableID;
  SOLA_IDENTIFY_ITEM fiberID;
  SOLA_IDENTIFY_ITEM locationA;
  SOLA_IDENTIFY_ITEM locationB; 
  solaIdentifySetting solaIdentify;
}SOLA_IDENTIFY;


typedef struct _sola_linedefine
{
  int splittingRatioLevel1;
  int splittingRatioLevel2;
  int splittingRatioLevel3;
  float groupIndex1550;
  float backScatter1550;
  int isReminderDialog;
}SOLA_LINEDEFINE;

/****************链路是否通过阈值******************/
typedef enum sola_set_type
{
  KNONE = 0,
  NOLYMIN,
  NOLYMAX,
  MINANDMAX,
}SOLA_SET_TYPE;

typedef enum sola_wavelength
{
  W1310NM = 1,
  W1550NM,
  W1625NM,
  ALL,
}SOLA_WAVELENGTH;

typedef struct _sola_max_min_set
{
  SOLA_SET_TYPE type;
  float min;
  float max;
}SOLA_MAX_MIN_SET;

typedef struct _sola_line_wavelength
{
  SOLA_MAX_MIN_SET lineLoss;
  SOLA_MAX_MIN_SET lineReturnLoss;
}SOLA_LINE_WAVELENGTH;

typedef struct _sola_line_passthreshold
{
  SOLA_MAX_MIN_SET      lineLength;
  SOLA_LINE_WAVELENGTH  lineWave[4];
  SOLA_WAVELENGTH       currentWave;
  SOLA_MAX_MIN_SET      lineLength_ft;
  SOLA_MAX_MIN_SET      lineLength_mile;
}SOLA_LINE_PASSTHRESHOLD;

/****************元素是否通过阈值******************/
typedef struct _sola_item_wave_arg
{
    float fValue[WAVE_NUM+1];
    char iSelectedFlag[WAVE_NUM+1];
}SOLA_ITEM_WAVE_ARG;

typedef struct _sola_item_passthreshold
{
    char iCurrentWave;
    SOLA_ITEM_WAVE_ARG maxSpliceLoss;
    SOLA_ITEM_WAVE_ARG maxLinkerLoss;
    SOLA_ITEM_WAVE_ARG maxLinkerReflectance;
    SOLA_ITEM_WAVE_ARG maxSplitter2Loss;
    SOLA_ITEM_WAVE_ARG maxSplitter4Loss;
    SOLA_ITEM_WAVE_ARG maxSplitter8Loss;
    SOLA_ITEM_WAVE_ARG maxSplitter16Loss;
    SOLA_ITEM_WAVE_ARG maxSplitter32Loss;
    SOLA_ITEM_WAVE_ARG maxSplitter64Loss;
    SOLA_ITEM_WAVE_ARG maxSplitter128Loss;
    SOLA_ITEM_WAVE_ARG maxSplitterReflectance;
  
}SOLA_ITEM_PASSTHRESHOLD;

/****************功率是否通过阈值******************/
typedef struct _sola_power_passthreshold
{
  SOLA_MAX_MIN_SET power[4];
}SOLA_POWER_PASSTHRESHOLD;

/****************messager结构体******************/
typedef struct _sola_messager
{
  char name[512];
  SOLA_IDENTIFY identify;
  SOLA_LINEDEFINE lineDefine;                   //该结构体不在使用 2018/05/02
  SOLA_LINE_PASSTHRESHOLD linePassThreshold;
  SOLA_ITEM_PASSTHRESHOLD itemPassThreshold;
  SOLA_POWER_PASSTHRESHOLD powerPassTHreshold;
  SOLA_FILE_NAME_SETTING autoFilename;
  MarkParam newIdentify;
  
  int permission_copy;
  int permission_modify;
  int permission_delete;
}SOLA_MESSAGER;

#endif  //_APP_FRMSOLAMESSAGER_H
