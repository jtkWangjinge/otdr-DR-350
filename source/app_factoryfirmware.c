/****************************************Copyright (c)****************************************************
**
** INNO Instrument(China) Co.,Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name: app_config.c
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
#include "app_factoryfirmware.h"
#include <stdio.h>
#include <string.h>

/*!
 * 计算文件的校验和，用于载入验证和保存更新
 * @param[in]  file 文件
 * @return     校验和，注意计算得到的校验和不包括checkSum字段本身
 */
static unsigned int ConfigGetChecksum(FACTORY_FIRMWARE *file)
{
    unsigned int oldCheckSum = file->checkSum;
    unsigned int newCheckSum = 0;
    unsigned int i, n;

    file->checkSum = 0;
    for (i = 0, n = file->size / 4; i < n; i++) {
        newCheckSum += ((unsigned int*)file)[i];
    }
    file->checkSum = oldCheckSum;

    return newCheckSum;
}

/**
 * 对配置文件头进行填充
 * @param[inout] file 配置文件
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int CreateOtdrConfig(FACTORY_FIRMWARE *file)
{
    int iErr = OtdrConfigOk;

    if (!file)
    {
        iErr = OtdrConfigInvalidParam;
    }

    if (iErr == OtdrConfigOk)
    {
        memset(file, 0, sizeof(FACTORY_FIRMWARE));
    }

    if (iErr == OtdrConfigOk)
    {
        file->magic = OTDR_CONFIG_MAGIC;
        file->revision = OTDR_CONFIG_REVISION;
        file->size = sizeof(FACTORY_FIRMWARE);
    }

    return iErr;
}

/**
 * 将系统可用的语言设置到配置文件中
 * @param[inout] file 配置文件
 * @param[in] ulAvailableLanguage 系统可用的语言（按位保存）
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int SetLanguage(FACTORY_FIRMWARE *file, unsigned long long int ulAvailableLanguage)
{
    int iErr = OtdrConfigOk;

    if (!file)
    {
        iErr = OtdrConfigInvalidParam;
    }

    if (iErr == OtdrConfigOk)
    {
        file->ulAvailableLanguage = ulAvailableLanguage;
    }

    return iErr;
}

/**
* 将目标机器的序列号设置到配置文件中
* @param[inout] file 配置文件
* @param[in] serialNo 序列号字符串
* @return   成功返回0 失败返回错误码
* @note 有关此函数的特别说明：…
*/
int SetSerialNo(FACTORY_FIRMWARE *file, char *serialNo)
{
    int iErr = OtdrConfigOk;

    if (!file || !serialNo)
    {
        iErr = OtdrConfigInvalidParam;
    }

    int serialNoLength = strlen(serialNo);
    if (iErr == OtdrConfigOk)
    {
        if (serialNoLength > OTDR_CONFIG_SERIALNO_MAX_LENGTH || serialNoLength <= 0)
        {
            iErr = OtdrConfigSerialNoError;
        }
    }

    if (iErr == OtdrConfigOk)
    {
        memset(file->serialNo, 0, OTDR_CONFIG_SERIALNO_MAX_LENGTH);
        memcpy(file->serialNo, serialNo, serialNoLength);
    }

    return iErr;
}

/**
 * 保存配置文件
 * @param[in] file 配置文件
 * @param[in] path 配置文件的保存路径（绝对路径）
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int SaveOtdrConfig(FACTORY_FIRMWARE *file, const char *path)
{
    int iErr = OtdrConfigOk;
    FILE *fp = NULL;

    if (!file || !path)
    {
        iErr = OtdrConfigInvalidParam;
    }

    if (iErr == OtdrConfigOk)
    {
        file->checkSum = 0 - ConfigGetChecksum(file);

        fp = fopen(path, "w+b");
        if (fp == NULL)
        {
            iErr = OtdrConfigAccessFailed;
        }
    }

    if (iErr == OtdrConfigOk)
    {
        if (1 != fwrite(file, sizeof(FACTORY_FIRMWARE), 1, fp))
        {
            iErr = OtdrConfigWriteError;
        }
    }

    switch (iErr)
    {
    case OtdrConfigOk:
    case OtdrConfigWriteError:
        fclose(fp);
    case OtdrConfigAccessFailed:
    case OtdrConfigInvalidParam:
    default:
        break;
    }

    return iErr;
}

/**
 * 加载配置文件
 * @param[out] file 配置文件
 * @param[in] path 配置文件的保存路径（绝对路径）
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int LoadOtdrConfig(FACTORY_FIRMWARE *file, const char *path)
{
    int iErr = OtdrConfigOk;
    FILE *fp = NULL;

    if (!file || !path)
    {
        iErr = OtdrConfigInvalidParam;
    }

    if (iErr == OtdrConfigOk)
    {
        fp = fopen(path, "rb");
        if (fp == NULL)
        {
            iErr = OtdrConfigAccessFailed;
        }
    }

    if (iErr == OtdrConfigOk)
    {
        if (0 == fread(file, 1, sizeof(FACTORY_FIRMWARE), fp))
        {
            iErr = OtdrConfigReadError;
        }
    }

    if (iErr == OtdrConfigOk)
    {
        if (file->magic != OTDR_CONFIG_MAGIC)
        {
            iErr = OtdrConfigBadFormat;
        }
    }

    if (iErr == OtdrConfigOk)
    {
        if (file->revision > OTDR_CONFIG_REVISION)
        {
            iErr = OtdrConfigUnsupportedRevision;
        }
    }

    if (iErr == OtdrConfigOk)
    {
        if (file->checkSum + ConfigGetChecksum(file) != 0)
        {
            iErr = OtdrConfigBadChecksum;
        }
    }

    if (iErr == OtdrConfigOk)
    {
        if (file->revision < OTDR_CONFIG_REVISION)
        {
            memset(file->serialNo, 0, OTDR_CONFIG_SERIALNO_MAX_LENGTH);
        }
    }

    switch (iErr)
    {
    case OtdrConfigOk:
    case OtdrConfigBadChecksum:
    case OtdrConfigUnsupportedRevision:
    case OtdrConfigBadFormat:
    case OtdrConfigReadError:
        fclose(fp);
    case OtdrConfigAccessFailed:
    case OtdrConfigInvalidParam:
    default:
        break;
    }

    return iErr;
}