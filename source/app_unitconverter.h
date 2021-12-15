/****************************************Copyright (c)****************************************************
**
**                          捷太科仪器(苏州)有限公司上海研发部
**
**--------------File Info---------------------------------------------------------------------------------
** File name: app_unitConverter.h
** Latest Version: V1.0.0
** Latest modified Date: 
** Modified by:
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by: 
** Created date: 
** Descriptions:
**
*********************************************************************************************************/
#ifndef _APP_UNIT_CONVERTER_H
#define _APP_UNIT_CONVERTER_H

#include "guibase.h"

typedef enum ModuleUnit
{
    MODULE_UNIT_OTDR = 0,
    MODULE_UNIT_SOLA
}MODULE_UNIT;

//距离单位
typedef enum Unit
{
    UNIT_M = 0,
    UNIT_KM,
    UNIT_FT,
    UNIT_KFT,
    UNIT_MI,
    UNIT_YD,
    UNIT_COUNT
}UNIT;

//功率单位(w)
typedef enum Power_Unit
{
    UNIT_NW = 0,
    UNIT_UW,
    UNIT_MW,
    UNIT_W,
    POWER_UNIT_COUNT
}POWER_UNIT;

//功率单位(db)
typedef enum Db_Unit
{
    UNIT_DBM = 0,
    UNIT_DB
}DB_UNIT;

//数据转换精度
typedef enum Precision
{
    PRECISION_0 = 0,
    PRECISION_1,
    PRECISION_2,
    PRECISION_3,
    PRECISION_4,
    PRECISION_5,
    PRECISION_6,
    PRECISION_COUNT
} PRECISION;

#define DISTANCE_LITTLE_UNIT_PRECISION      PRECISION_1         //距离小单位精度（m、ft、yd）
#define DISTANCE_BIG_UNIT_PRECISION         PRECISION_2         //距离大单位精度（km、kft、mi）

#define IOR_PRECISION                       PRECISION_6         //折射率精度
#define BACKSCATTER_COEFFICIENT_PRECISION   PRECISION_2         //背向散射精度
#define HELIX_FACTOR_PRECISION              PRECISION_3         //余长系数精度

#define LOSS_PRECISION                      PRECISION_3         //损耗精度（包括：损耗、累计损耗、损耗阈值、末端阈值等）
#define REFLECTANCE_PRECISION               PRECISION_3         //反射率精度
#define ATTENUATION_PRECISION               PRECISION_3         //衰减精度
#define ORL_PRECISION                       PRECISION_3         //光回损精度


//整数转换到字符串(返回指针，注意释放内存)
char* Int2String(int data);
//整数转换到宽字符串(返回指针，注意释放内存)
GUICHAR* Int2GuiString(int data);

//浮点数转换到字符串(返回指针，注意释放内存)
char* Float2String(PRECISION precision, double data);
//浮点数转换到宽字符串(返回指针，注意释放内存)
GUICHAR* Float2GuiString(PRECISION precision, double data);

//浮点数转换到字符串(带单位)(返回指针，注意释放内存)
char* Float2StringUnit(PRECISION precision, double data, char *unit);
//浮点数转换到宽字符串(带单位)(返回指针，注意释放内存)
GUICHAR* Float2GuiStringUnit(PRECISION precision, double data, char *unit);

//字符串转换到浮点数
double String2Float(char* data);

//字符串转换到整数
int String2Int(char* data);

//根据精度四舍五入转换浮点数
double Float2Float(PRECISION precision, double data);

//获得系统当前单位
UNIT GetCurrSystemUnit(MODULE_UNIT module);

//获得当前系统设置单位的字符串(返回字符串指针，注意内存释放)
char* GetCurrSystemUnitString(MODULE_UNIT module);

//获得当前系统设置单位的字符串(返回宽字符串指针，注意内存释放)
GUICHAR* GetCurrSystemUnitGuiString(MODULE_UNIT module);

//设置系统单位
void SetCurrSystemUnit(MODULE_UNIT module, UNIT unit);

//获得距离精度
PRECISION GetDistancePrecision(MODULE_UNIT module);

/**************************距离的单位转换*****************************/

//通过指定单位的单位转换
double UnitConverter_Dist_Float2Float(UNIT originalUnit, UNIT targetUnit, double data);

//从系统设置的单位转换到米和千米(返回浮点值)
double UnitConverter_Dist_System2M_Float2Float(MODULE_UNIT module, double data);
double UnitConverter_Dist_System2Km_Float2Float(MODULE_UNIT module, double data);

//从系统设置的单位转换到英尺和千英尺(返回浮点值)
double UnitConverter_Dist_System2FT_Float2Float(MODULE_UNIT module, double data);
double UnitConverter_Dist_System2KFT_Float2Float(MODULE_UNIT module, double data);

//从系统设置的单位转换到码和英里(返回浮点值)
double UnitConverter_Dist_System2YD_Float2Float(MODULE_UNIT module, double data);
double UnitConverter_Dist_System2MI_Float2Float(MODULE_UNIT module, double data);

//从系统设置的单位转换到米和千米(返回字符串指针，注意内存释放)
double UnitConverter_Dist_System2M_String2Float(MODULE_UNIT module, char* data);
double UnitConverter_Dist_System2Km_String2Float(MODULE_UNIT module, char* data);

//从系统设置的单位转换到英尺和千英尺(返回字符串指针，注意内存释放)
double UnitConverter_Dist_System2FT_String2Float(MODULE_UNIT module, char* data);
double UnitConverter_Dist_System2KFT_String2Float(MODULE_UNIT module, char* data);

//从系统设置的单位转换到码和英里(返回字符串指针，注意内存释放)
double UnitConverter_Dist_System2YD_String2Float(MODULE_UNIT module, char* data);
double UnitConverter_Dist_System2MI_String2Float(MODULE_UNIT module, char* data);

//从米和千米转换到系统设置的单位(返回浮点值)
double UnitConverter_Dist_M2System_Float2Float(MODULE_UNIT module, double data);
double UnitConverter_Dist_Km2System_Float2Float(MODULE_UNIT module, double data);

//从米和千米转换到系统设置的单位(返回字符串指针，注意内存释放)
char* UnitConverter_Dist_M2System_Float2String(MODULE_UNIT module, double data, int isAddUnitString);
char* UnitConverter_Dist_Km2System_Float2String(MODULE_UNIT module, double data, int isAddUnitString);

//从米和千米转换到系统设置的单位(返回字符串指针，注意内存释放)
GUICHAR* UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT module, double data, int isAddUnitString);
GUICHAR* UnitConverter_Dist_Km2System_Float2GuiString(MODULE_UNIT module, double data, int isAddUnitString);

//从eeprom中存储的值中根据系统设置选择一个转换成字符串输出(返回字符串指针，注意内存释放)
char* UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT module,
    double data_m, double data_ft, double data_mi, int isAddUnitString);

//从eeprom中存储的值中根据系统设置选择一个转换成字符串输出(返回宽字符串指针，注意内存释放)
GUICHAR* UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT module,
    double data_m, double data_ft, double data_mi, int isAddUnitString);

//判断米、英尺和英里三个值转换后是否相等（误差0.1以内认为相等）
int UnitConverter_Dist_isEqual(double data_m, double data_ft, double data_mi);

//OPM中功率值转换成dbm
double UnitConverter_Power_P2Dbm(POWER_UNIT powerUnit, double powerValue);

//OPM中功率值转换成db
double UnitConverter_Power_P2Db(POWER_UNIT powerUnit, double powerValue, double refValue);

//OPM中功率值转换成db/dBm
double UnitConverter_Power_P2Db_Float2Float(DB_UNIT dbUnit, POWER_UNIT powerUnit, double powerValue, double refValue);

//OPM中功率值转换成db/dbm 的guichar格式
GUICHAR* UnitConver_Power_Float2GuiString(DB_UNIT dbUnit, POWER_UNIT powerUnit, double powerValue, double refValue);

#endif
