/****************************************Copyright (c)****************************************************
**
**                            捷太科仪器(苏州)有限公司上海研发部
**
**--------------File Info---------------------------------------------------------------------------------
** File name: app_unitConverter.c
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
#include "app_unitconverter.h"
#include "app_frminit.h"

#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//单位转换的参数 target = data * ConverterPara[original][target];
double ConverterPara[UNIT_COUNT][UNIT_COUNT] = { { 1,       0.001,      -0.3048,     -304.8,  -1609.344, -0.9144 },
                                                 { 1000,    1,          -0.0003048,  -0.3048, -1.609344, -0.0009144 },
                                                 { 0.3048,  0.0003048,  1,           0.001,   -5280,     -3 },
                                                 { 304.8,   0.3048,     1000,        1,       -5.280, -0.003 },
                                                 { 1609.344,1.609344,   5280,        5.280,   1,         1760 },
                                                 { 0.9144,  0.0009144,  3,           0.003,   -1760,     1 } };

//整数转换到字符串(返回指针，注意释放内存)
char* Int2String(int data)
{
    char *ret = NULL;
    char buff[32] = {0};
    memset(buff, 0, sizeof(buff));

    sprintf(buff, "%d", data);

    int len = strlen(buff);
    ret = calloc(1, len+1);
    memset(ret, 0, len+1);
    strcpy(ret, buff);

    return ret;
}

//整数转换到宽字符串(返回指针，注意释放内存)
GUICHAR* Int2GuiString(int data)
{
    GUICHAR *ret = NULL;
    
    char *temp = Int2String(data);
    
    ret = TransString(temp);

    free(temp);
    temp = NULL;

    return ret;
}

//浮点数转换到字符串(返回指针，注意释放内存)
char* Float2String(PRECISION precision, double data)
{
    char *ret = NULL;
    char buff1[32] = {0};
    char buff2[32] = {0};
    memset(buff1, 0, sizeof(buff1));
    memset(buff2, 0, sizeof(buff2));

    sprintf(buff1, "%%.%df", precision);
    sprintf(buff2, buff1, data);

    int len = strlen(buff2);
    ret = calloc(1, len+1);
    memset(ret, 0, len+1);
    strcpy(ret, buff2);

    return ret;
}

//浮点数转换到宽字符串(返回指针，注意释放内存)
GUICHAR* Float2GuiString(PRECISION precision, double data)
{
    GUICHAR *ret = NULL;
    
    char *temp = Float2String(precision, data);
    
    ret = TransString(temp);

    free(temp);
    temp = NULL;

    return ret;
}

//浮点数转换到字符串(可带单位)(返回指针，注意释放内存)
char* Float2StringUnit(PRECISION precision, double data, char *unit)
{
    char *ret = NULL;
    
    char *temp = Float2String(precision, data);
    
    if(unit == NULL)
    {
        ret = temp;
    }
    else
    {
        char buff[128] = {0};
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%s%s", temp, unit);

        int len = strlen(buff);

        ret = calloc(1, len + 1);
        strcpy(ret, buff);
    }

    free(temp);
    temp = NULL;

    return ret;
}

//浮点数转换到宽字符串(可带单位)(返回指针，注意释放内存)
GUICHAR* Float2GuiStringUnit(PRECISION precision, double data, char *unit)
{
    GUICHAR *ret = NULL;
    
    char *temp = Float2String(precision, data);
    
    if(unit == NULL)
    {
        ret = TransString(temp);
    }
    else
    {
        char buff[128] = {0};
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%s%s", temp, unit);
        ret = TransString(buff);
    }

    free(temp);
    temp = NULL;

    return ret;
}

//字符串转换到浮点数
double String2Float(char* data)
{
    double ret = 0.0f;
    if (data)
    {
        ret = atof(data);
    }

    return ret;
}

//字符串转换到整数
int String2Int(char* data)
{
    int ret = 0.0f;
    if (data)
    {
        ret = atoi(data);
    }

    return ret;
}

//根据精度四舍五入转换浮点数
double Float2Float(PRECISION precision, double data)
{
    double offset = 5.0f/pow(10, precision+1);
    double ret = 0.0f;
    
    if(data < 0)
    {
        ret = data - offset;
    }
    else
    {
        ret = data + offset;
    }

    int temp = ret * pow(10, precision);
    ret = (double)temp / pow(10, precision);
    
    return ret;
}

//获得系统当前单位
UNIT GetCurrSystemUnit(MODULE_UNIT module)
{
    UNIT unit = UNIT_M;
    switch(module)
    {
        case MODULE_UNIT_OTDR:
            unit = pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag;
        break;
        case MODULE_UNIT_SOLA:
            unit = pOtdrTopSettings->pUser_Setting->sSolaSetting.iUnitSelected;
        break;
        default:
        break;
    }
    return unit;
}

//获得当前系统设置单位的字符串(返回字符串指针，注意内存释放)
char* GetCurrSystemUnitString(MODULE_UNIT module)
{
    char *ret = NULL;
    char buff[8] = {0};
    memset(buff, 0, sizeof(buff));

    switch(GetCurrSystemUnit(module))
    {
        case UNIT_M:
        strcpy(buff, "m");
        break;
        case UNIT_KM:
        strcpy(buff, "km");
        break;
        case UNIT_FT:
        strcpy(buff, "ft");
        break;
        case UNIT_KFT:
        strcpy(buff, "kft");
        break;
        case UNIT_YD:
        strcpy(buff, "yd");
        break;
        case UNIT_MI:
        strcpy(buff, "mi");
        break;
        default:
        break;
    }

    int strLen = strlen(buff);
    ret = calloc(1, strLen + 1);
    strcpy(ret, buff);
    
    return ret;
}


//获得当前系统设置单位的字符串(返回宽字符串指针，注意内存释放)
GUICHAR* GetCurrSystemUnitGuiString(MODULE_UNIT module)
{
    GUICHAR *ret = NULL;
    char *temp = GetCurrSystemUnitString(module);

    ret = TransString(temp);

    free(temp);
    temp = NULL;

    return ret;
}


//设置系统单位
void SetCurrSystemUnit(MODULE_UNIT module, UNIT unit)
{
    switch(module)
    {
        case MODULE_UNIT_OTDR:
            pOtdrTopSettings->pUser_Setting->sCommonSetting.iUnitConverterFlag = unit;
        break;
        case MODULE_UNIT_SOLA:
            pOtdrTopSettings->pUser_Setting->sSolaSetting.iUnitSelected = unit;
        break;
        default:
        break;
    }
}


//获得距离精度
PRECISION GetDistancePrecision(MODULE_UNIT module)
{
    //系统单位是米，保留整数，其他情况，保留一位有效小数
    // if (GetCurrSystemUnit(module) == UNIT_M)
    // {
    //     return PRECISION_0;
    // }
    // else
    // {
    //     return PRECISION_1;
    // }
    return PRECISION_2;
}

/**************************距离的单位转换*****************************/

//通过指定单位的单位转换
double UnitConverter_Dist_Float2Float(UNIT originalUnit, UNIT targetUnit, double data)
{
    if(originalUnit >= UNIT_COUNT || targetUnit >= UNIT_COUNT)
    {
        LOG(LOG_ERROR, "Unit conversion parameter error!!!\n");
        return 0.0f;
    }

    double temp = 0.0f;
    double para = ConverterPara[originalUnit][targetUnit];
    if(para < 0)
    {
        temp = data / (0.0f - para);
    }
    else
    {
        temp = data * para;
    }
    
    return temp;
}

//从系统设置的单位转换到米和千米(返回浮点值)
double UnitConverter_Dist_System2M_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(GetCurrSystemUnit(module), UNIT_M, data);
}

double UnitConverter_Dist_System2Km_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(GetCurrSystemUnit(module), UNIT_KM, data);
}

//从系统设置的单位转换到英尺和千英尺(返回浮点值)
double UnitConverter_Dist_System2FT_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(GetCurrSystemUnit(module), UNIT_FT, data);
}

double UnitConverter_Dist_System2KFT_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(GetCurrSystemUnit(module), UNIT_KFT, data);
}

//从系统设置的单位转换到码和英里(返回浮点值)
double UnitConverter_Dist_System2YD_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(GetCurrSystemUnit(module), UNIT_YD, data);
}

double UnitConverter_Dist_System2MI_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(GetCurrSystemUnit(module), UNIT_MI, data);
}

//从系统设置的单位转换到米和千米(返回浮点值)
double UnitConverter_Dist_System2M_String2Float(MODULE_UNIT module, char* data)
{
    double temp = String2Float(data);
    return UnitConverter_Dist_System2M_Float2Float(module, temp);
}
double UnitConverter_Dist_System2Km_String2Float(MODULE_UNIT module, char* data)
{
    double temp = String2Float(data);
    return UnitConverter_Dist_System2Km_Float2Float(module, temp);
}

//从系统设置的单位转换到英尺和千英尺(返回浮点值)
double UnitConverter_Dist_System2FT_String2Float(MODULE_UNIT module, char* data)
{
    double temp = String2Float(data);
    return UnitConverter_Dist_System2FT_Float2Float(module, temp);
}
double UnitConverter_Dist_System2KFT_String2Float(MODULE_UNIT module, char* data)
{
    double temp = String2Float(data);
    return UnitConverter_Dist_System2KFT_Float2Float(module, temp);
}

//从系统设置的单位转换到码和英里(返回浮点值)
double UnitConverter_Dist_System2YD_String2Float(MODULE_UNIT module, char* data)
{
    double temp = String2Float(data);
    return UnitConverter_Dist_System2YD_Float2Float(module, temp);
}
double UnitConverter_Dist_System2MI_String2Float(MODULE_UNIT module, char* data)
{
    double temp = String2Float(data);
    return UnitConverter_Dist_System2MI_Float2Float(module, temp);
}

//从米和千米转换到系统设置的单位(返回浮点值)
double UnitConverter_Dist_M2System_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(UNIT_M, GetCurrSystemUnit(module), data);
}
double UnitConverter_Dist_Km2System_Float2Float(MODULE_UNIT module, double data)
{
    return UnitConverter_Dist_Float2Float(UNIT_KM, GetCurrSystemUnit(module), data);
}

//从米和千米转换到系统设置的单位(返回字符串指针，注意内存释放)
char* UnitConverter_Dist_M2System_Float2String(MODULE_UNIT module, double data,
    int isAddUnitString)
{
    char *ret = NULL;
    double temp = UnitConverter_Dist_M2System_Float2Float(module, data);
    char *buff = Float2String(GetDistancePrecision(module), temp);
    if(isAddUnitString)
    {
        char buff1[32] = {0};
        char *buffUnit = GetCurrSystemUnitString(module);
        memset(buff1, 0, sizeof(buff1));
        sprintf(buff1, "%s%s", buff, buffUnit);

        free(buff);
        free(buffUnit);

        int strLen = strlen(buff1);
        ret = calloc(1, strLen + 1);
        strcpy(ret, buff1);
    }
    else
    {
        ret = buff;
    }
    
    return ret;
}

char* UnitConverter_Dist_Km2System_Float2String(MODULE_UNIT module, double data,
    int isAddUnitString)
{
    char *ret = NULL;
    double temp = UnitConverter_Dist_Km2System_Float2Float(module, data);
    char *buff = Float2String(GetDistancePrecision(module), temp);
    if(isAddUnitString)
    {
        char buff1[32] = {0};
        char *buffUnit = GetCurrSystemUnitString(module);
        memset(buff1, 0, sizeof(buff1));
        sprintf(buff1, "%s %s", buff, buffUnit);

        free(buff);
        free(buffUnit);

        int strLen = strlen(buff1);
        ret = calloc(1, strLen + 1);
        strcpy(ret, buff1);
    }
    else
    {
        ret = buff;
    }
    
    return ret;
}

//从米和千米转换到系统设置的单位(返回字符串指针，注意内存释放)
GUICHAR* UnitConverter_Dist_M2System_Float2GuiString(MODULE_UNIT module, double data,
    int isAddUnitString)
{
    char *buff = UnitConverter_Dist_M2System_Float2String(module, data, isAddUnitString);
    GUICHAR *ret = TransString(buff);
    free(buff);
    buff = NULL;
    
    return ret;
}

GUICHAR* UnitConverter_Dist_Km2System_Float2GuiString(MODULE_UNIT module, double data,
    int isAddUnitString)
{
    char *buff = UnitConverter_Dist_Km2System_Float2String(module, data, isAddUnitString);
    GUICHAR *ret = TransString(buff);
    free(buff);
    buff = NULL;
    
    return ret;
}

//从eeprom中存储的值中根据系统设置选择一个转换成字符串输出(返回字符串指针，注意内存释放)
char* UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT module,
    double data_m, double data_ft, double data_mi, int isAddUnitString)
{
    char *ret = NULL;
    double temp = 0.0f;

    switch(GetCurrSystemUnit(module))
    {
        case UNIT_M:
        temp = data_m;
        break;
        case UNIT_KM:
        temp = data_m / 1000;
        break;
        case UNIT_FT:
        temp = data_ft;
        break;
        case UNIT_KFT:
        temp = data_ft / 1000;
        break;
        case UNIT_YD:
        break;
        case UNIT_MI:
        temp = data_mi;
        break;
        default:
        break;
    }
    
    char *buff = Float2String(GetDistancePrecision(module), temp);
    if(isAddUnitString)
    {
        char buff1[32] = {0};
        char *buffUnit = GetCurrSystemUnitString(module);
        memset(buff1, 0, sizeof(buff1));
        sprintf(buff1, "%s %s", buff, buffUnit);

        free(buff);
        free(buffUnit);

        int strLen = strlen(buff1);
        ret = calloc(1, strLen + 1);
        strcpy(ret, buff1);
    }
    else
    {
        ret = buff;
    }
    
    return ret;
}

//从eeprom中存储的值中根据系统设置选择一个转换成字符串输出(返回宽字符串指针，注意内存释放)
GUICHAR* UnitConverter_Dist_Eeprom2System_Float2GuiString(MODULE_UNIT module,
    double data_m, double data_ft, double data_mi, int isAddUnitString)
{
    char *buff = UnitConverter_Dist_Eeprom2System_Float2String(module, 
        data_m, data_ft, data_mi, isAddUnitString);
    GUICHAR *ret = TransString(buff);
    free(buff);
    buff = NULL;
    
    return ret;
}

//判断米、英尺和英里三个值转换后是否相等（误差0.1以内认为相等）(返回非0 相等)
int UnitConverter_Dist_isEqual(double data_m, double data_ft, double data_mi)
{
    double temp1 = UnitConverter_Dist_Float2Float(UNIT_FT, UNIT_M, data_ft);
    double temp2 = UnitConverter_Dist_Float2Float(UNIT_MI, UNIT_M, data_mi);

    double temp3 = fabsf(data_m - temp1);
    double temp4 = fabsf(data_m - temp2);
    if(temp3 > 0.1 || temp4 > 0.1)
    {
        return 0;
    }

    return 1;
}

//OPM中功率值转换成dbm
double UnitConverter_Power_P2Dbm(POWER_UNIT powerUnit, double powerValue)
{
    float ratio[POWER_UNIT_COUNT] = {  pow(10,-6), pow(10,-3), 1, pow(10,3) };
    double dBmValue = 10*log10(powerValue * ratio[powerUnit]);
    return dBmValue;
}

//OPM中功率值转换成db
double UnitConverter_Power_P2Db(POWER_UNIT powerUnit, double powerValue, double refValue)
{
    double dBvalue = UnitConverter_Power_P2Dbm(powerUnit, powerValue) - UnitConverter_Power_P2Dbm(powerUnit, refValue);
    return dBvalue;
}

//OPM中功率值转换成db/dBm
double UnitConverter_Power_P2Db_Float2Float(DB_UNIT dbUnit, POWER_UNIT powerUnit, double powerValue, double refValue)
{
    double value = 0.00f;

    if (dbUnit == UNIT_DBM)
    {
        value = UnitConverter_Power_P2Dbm(powerUnit, powerValue);
    }
    else
    {
        value = UnitConverter_Power_P2Db(powerUnit, powerValue, refValue);
    }

    return value;
}

//OPM中功率值转换成db/dbm 的guichar格式
GUICHAR* UnitConver_Power_Float2GuiString(DB_UNIT dbUnit, POWER_UNIT powerUnit, double powerValue, double refValue)
{
    double value = 0.00f;
    char tempValue[10] = {0};
    memset(tempValue, 0, 10);
    
    if(dbUnit == UNIT_DBM)
    {
        value = UnitConverter_Power_P2Dbm(powerUnit, powerValue);
    }
    else
    {
        value = UnitConverter_Power_P2Db(powerUnit, powerValue, refValue);
    }

    sprintf(tempValue, "%.3f", value);
    GUICHAR* ret = TransString(tempValue);
    return ret;
}


