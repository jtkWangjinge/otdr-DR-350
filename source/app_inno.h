/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_inno.h  
* 摘    要：  inno格式文件的加载和生成功能
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#ifndef APP_INNO
#define APP_INNO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INNO_MAGIC                          0x4F4E4E49                  //!< INNO文件头标识码
#define SOR_MAGIC                           0x0070614D                  //!< INNO文件头标识码
#define INNO_REVISION                       0x01000001                  //!< 文件格式版本号
#define MAX_NAME_LENGTH                     512                         //!< 最大文件名长度

/**
*  @enum   INNO_ERROR
*  @brief  文件操作错误码
*/
typedef enum file_error_type
{
    InnoOk = 0,                             //!< 操作成功，无错误

    InnoInvalidParam,                       //!< 无效参数，例如空句柄，非预期的负数计数值等
    InnoAccessFailed,                       //!< 文件无法创建/打开
    InnoOutOfMemory,                        //!< 内存不足，导致创建部分对象失败
    InnoBadFormat,                          //!< 格式错误，通常说明Magic错误
    InnoUnsupportedRevision,                //!< 不支持的版本，通常说明版本过高
    InnoBadChecksum,                        //!< 校验和无效
    InnoFileNotExist,                       //!< 文件不存在
    InnoWriteError,                         //!< 文件写错误
    InnoReadError,                          //!< 文件读错误
    InnoSORFormat,                          //!< 文件为sor文件

    InnoUnknownError = -1                   //!< 未知错误
} INNO_ERROR;

/**
 *  @struct file_item_index_type
 *  @brief  数据块索引，即数据包内各个数据项的索引
 */
typedef struct file_item_index_type
{
    unsigned int itemMagic;                          //!< 数据块的Magic，表明数据块的类型
    unsigned int itemSize;                           //!< 数据块的大小
    unsigned int dataOffset;                         //!< 数据块在数据包中的偏移量（包括包头在内的总偏移量）
    char itemName[MAX_NAME_LENGTH];                  //!< 数据块的名字，实际为当前数据块曲线名。
} INNO_ITEM_INDEX;

/**
 *  @struct file_inno_Header
 *  @brief  OTDR参数文件头，文件内容的总索引
 */
typedef struct file_inno_Header
{
    unsigned int magic;                              //!< 文件头部Magic(SOP)，固定值为INNO_MAGIC
    unsigned int revision;                           //!< 文件版本，固定值为INNO_REVISION
    unsigned int size;                               //!< 包含全部包数据在内的完整数据包大小，单位：字节
    unsigned int checkSum;                           //!< 文件头部的校验和，保证整个包头32位对齐求和等于零

    unsigned int timestamp;                          //!< 32-bit时间戳，从1900-1-1开始?
    unsigned int reserved;                           //!< 保留字段
    unsigned int indexCount;                         //!< 数据块索引个数
    unsigned int indexOffset;                        //!< 数据块索引表的起始偏移量，默认为32，即头部以后立即开始
}INNO_HEAHER;

/*!
 * 生成inno格式文件
 * @param[in]  absolutePath      生成文件的存储路径(包含文件名)
 * @param[in]  sor               sor文件路径(带文件名的绝对路径)
 * @param[in]  count             sor文件数量
 * @return                       操作成功返回0
 */
int InnoCreate(char *absolutePath, char **sor, unsigned int count);

/*!
 * 加载inno格式文件
 * @param[in]  absolutePath      生成文件的存储路径(包含文件名)
 * @param[in]  sorPath           sor文件解析出来存放的路径
 * @param[out]  sorList          输出从文件中解析出来的sor文件列表(绝对路径)(注意使用完释放内存)
 * @param[out]  count            sor文件数量
 * @return                       操作成功返回0
 */
int InnoLoad(char *absolutePath, char *sorPath, char ***sorList, unsigned int *count);

#endif //APP_INNO