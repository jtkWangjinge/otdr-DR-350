/****************************************Copyright (c)****************************************************
**
** INNO Instrument(China) Co.,Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File name: app_config.h
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
#ifndef APP_CONFIG
#define APP_CONFIG

#ifdef __cplusplus
extern "C" {
#endif

#define OTDR_CONFIG_MAGIC                          0x5244544F                  //!< 配置文件头标识码 OTDR
#define OTDR_CONFIG_REVISION                       0x01000002                  //!< 文件格式版本号
#define OTDR_CONFIG_SERIALNO_MAX_LENGTH            20                          //!< 序列号最大长度

/**
 *  @enum   INNO_ERROR
 *  @brief  文件操作错误码
 */
typedef enum file_error_type
{
    OtdrConfigOk = 0,                             //!< 操作成功，无错误

    OtdrConfigInvalidParam,                       //!< 无效参数，例如空句柄，非预期的负数计数值等
    OtdrConfigAccessFailed,                       //!< 文件无法创建/打开
    OtdrConfigOutOfMemory,                        //!< 内存不足，导致创建部分对象失败
    OtdrConfigBadFormat,                          //!< 格式错误，通常说明Magic错误
    OtdrConfigUnsupportedRevision,                //!< 不支持的版本，通常说明版本过高
    OtdrConfigBadChecksum,                        //!< 校验和无效
    OtdrConfigFileNotExist,                       //!< 文件不存在
    OtdrConfigWriteError,                         //!< 文件写错误
    OtdrConfigReadError,                          //!< 文件读错误
    OtdrConfigSerialNoError,                      //!< 序列号错误

    OtdrConfigUnknownError = -1,                  //!< 未知错误
} OTDR_CONFIG_ERROR;

/**
 *  @struct Otdr_Factory_Firmware
 *  @brief  OTDR配置文件头 结构体总大小为512字节
 */
typedef struct Otdr_Factory_Firmware
{
    unsigned int magic;                              //!< 文件头部Magic(SOP)，固定值为OTDR_CONFIG_MAGIC
    unsigned int revision;                           //!< 文件版本，固定值为OTDR_CONFIG_REVISION
    unsigned int size;                               //!< 包含全部包数据在内的完整数据包大小，单位：字节
    unsigned int checkSum;                           //!< 文件头部的校验和，保证整个包头32位对齐求和等于零

    unsigned long long int ulAvailableLanguage;                     //!< 系统可用的语言
    char serialNo[OTDR_CONFIG_SERIALNO_MAX_LENGTH];                 //!< 序列号
    unsigned char reserved[488 - OTDR_CONFIG_SERIALNO_MAX_LENGTH];  //!< 保留字段
}FACTORY_FIRMWARE;

/**
 * 对配置文件头进行填充
 * @param[inout] file 配置文件
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int CreateOtdrConfig(FACTORY_FIRMWARE *file);

/**
 * 将系统可用的语言设置到配置文件中
 * @param[inout] file 配置文件
 * @param[in] ulAvailableLanguage 系统可用的语言（按位保存）
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int SetLanguage(FACTORY_FIRMWARE *file, unsigned long long int ulAvailableLanguage);

/**
* 将目标机器的序列号设置到配置文件中
* @param[inout] file 配置文件
* @param[in] serialNo 序列号字符串
* @return   成功返回0 失败返回错误码
* @note 有关此函数的特别说明：…
*/
int SetSerialNo(FACTORY_FIRMWARE *file, char *serialNo);

/**
 * 保存配置文件
 * @param[in] file 配置文件
 * @param[in] path 配置文件的保存路径（绝对路径）
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int SaveOtdrConfig(FACTORY_FIRMWARE *file, const char *path);

/**
 * 加载配置文件
 * @param[out] file 配置文件
 * @param[in] path 配置文件的保存路径（绝对路径）
 * @return   成功返回0 失败返回错误码
 * @note 有关此函数的特别说明：…
 */
int LoadOtdrConfig(FACTORY_FIRMWARE *file, const char *path);

#ifdef __cplusplus
}
#endif

#endif