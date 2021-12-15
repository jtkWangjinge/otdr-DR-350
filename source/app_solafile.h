/****************************************Copyright (c)****************************************************
**
**                             捷太科仪器(苏州)有限公司上海研发部
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  SolaFile.h
** Latest Version:             V1.0.0
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               
** Descriptions:               用于将SOLA功能的全局结构体保存到文件或者从文件载入的适配器
** 
*********************************************************************************************************/
#ifndef SOLAFILE_H
#define SOLAFILE_H
/*
 * @brief 如果结构体格式有所调整：
 *        直接将旧版本的各个结构体定义在app_solaevent.h中重命名为_Vn的形式即可，
 *        例如改动两次后，最新版本为WAVE_SOLA_EVENT，旧版本为WAVE_SOLA_EVENT_V1和WAVE_SOLA_EVENT_V2。
 */
#include "app_sola.h"
#include "app_frmsolamessager.h"

#define SOLA_EVENT_VERSION                  1
#define TOTAL_SOLA_EVENT_VERSION            1
#define WAVE_SOLA_EVENT_VERSION             1

#ifdef __cplusplus
extern "C" {
#endif

/**   
 *  @enum   SOLA_FILE_ERROR
 *  @brief  文件操作错误码
 */
typedef enum sola_file_error_type
{
    SolaOk = 0,                             //!< 操作成功，无错误

    SolaInvalidParam,                       //!< 无效参数，例如空句柄，非预期的负数计数值等
    SolaAccessFailed,                       //!< 文件无法创建/打开
    SolaOutOfMemory,                        //!< 内存不足，导致创建部分对象失败
    SolaBadFormat,                          //!< 格式错误，通常说明Magic错误
    SolaUnsupportedRevision,                //!< 不支持的版本，通常说明版本过高
    SolaBadChecksum,                        //!< 校验和无效
    SolaIsDirectory,                        //!< 为目录
    SolaBadVersion = -2,                    //!< 不支持的SOLA文件版本号

    SolaUnknownError = -1                   //!< 未知错误
} SOLA_FILE_ERROR;

/** 
 * 载入一个SOLA文件
 * @param[in]  pszPath      SOLA文件路径
 * @param[out] pSolaEvent   将载入的SOLA文件内容保存到指定的SOLA事件表结构体
 * @return     int          错误码（SOLA_FILE_ERROR）
 */
int SolaLoad(const char* pszPath, SolaEvents* pSolaEvent);

/** 
 * 保存一个SOLA文件
 * @param[in]  pszPath      SOLA文件路径
 * @param[out] pSolaEvent   将指定的SOLA事件表结构体的内容保存到SOLA文件
 * @return     int          错误码（SOLA_FILE_ERROR）
 */
int SolaSave(const SolaEvents* pSolaEvent, char *absolutePath);

/** 
 * 获得SOLA文件文件名
 * @param[in]  char *pDirPath SOLA文件存储目录
               int iNameMaxLength 文件名最大
               SOLA_FILE_NAME_SETTING *pAutoname 自动命名，根据此规则获得文件名
 * @param[out] char *pName 得到的SOLA文件名(绝对路径)
 * @return     int          错误码（0 错误   1 正确）
 */
int SolaNamed(char *pDirPath, char *pName, int iNameMaxLength, SOLA_FILE_NAME_SETTING *pAutoname);

#ifdef __cplusplus
}
#endif

#endif // SOLAFILE_H
/*********************************************************************************************************
** End of file
*********************************************************************************************************/

