/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_inno.c  
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
#include "app_inno.h"
#include "app_global.h"

#include <sys/stat.h>  
#include <time.h>

/*!
 * 获得指定路径文件大小
 * @param[in]  path 文件路径
 * @return     文件大小（字节）
 */
static unsigned long getFileSize(const char *path)
{
    unsigned long fileSize = -1;
    struct stat statBuff;

    if (stat(path, &statBuff) < 0)
    {
        return 0;
    }
    else
    {
        fileSize = statBuff.st_size;
    }

    return fileSize;
}

/*!
 * 计算文件的校验和，用于载入验证和保存更新
 * @param[in]  file 文件
 * @return     校验和，注意计算得到的校验和不包括checkSum字段本身
 */
static unsigned int InnoGetChecksum(INNO_HEAHER *file)
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

/*!
 * 填充每个sor文件数据块信息
 * @param[in]  innoItemIndex 存储数据块信息的地址
 * @param[in]  count 文件数量
 * @param[in]  sor 文件列表
 * @return     成功返回0
 */
static int createItemIndex(INNO_ITEM_INDEX * innoItemIndex, unsigned int count, char ** sor)
{
    unsigned int loop;
    int iErr = InnoOk;
    unsigned int size = 0;

    size = sizeof(INNO_HEAHER) + sizeof(INNO_ITEM_INDEX)*count;

    for (loop = 0; loop < count; loop++)
    {
        char *name = strrchr(sor[loop], '/');
        if (name == NULL)
        {
            name = sor[loop];
        }
        else
        {
            name++;
        }
        innoItemIndex[loop].itemMagic = 0;
        innoItemIndex[loop].itemSize = getFileSize(sor[loop]);
        innoItemIndex[loop].dataOffset = size;
        memcpy(innoItemIndex[loop].itemName, name, strlen(name) + 1);

        LOG(LOG_INFO, "name:%s   size:%d\n", innoItemIndex[loop].itemName, innoItemIndex[loop].itemSize);
        size += innoItemIndex[loop].itemSize;
    }
    return iErr;
}

/*!
 * 生成inno格式文件
 * @param[in]  absolutePath      生成文件的存储路径(包含文件名)
 * @param[in]  sor               sor文件路径(带文件名的绝对路径)
 * @param[in]  count             sor文件数量
 * @return                       操作成功返回0 
 */
int InnoCreate(char *absolutePath, char **sor, unsigned int count)
{
    int iErr = InnoOk;
    INNO_HEAHER *innoHeader = NULL;
    INNO_ITEM_INDEX *innoItemIndex = NULL;
    unsigned int size = 0;
    unsigned int loop;
    FILE *innoFp = NULL;
    FILE *sorFp = NULL;
    
    if (!absolutePath || !sor || !count)
    {
        iErr = InnoInvalidParam;
    }

    if (iErr == InnoOk)
    {
        size = sizeof(INNO_HEAHER) + sizeof(INNO_ITEM_INDEX)*count;
        for (loop = 0; loop < count; loop++)
        {
            if (!sor[loop])
            {
                iErr = InnoInvalidParam;
                break;
            }
            else
            {
                size += getFileSize(sor[loop]);
            }
        }
        size += 4 - size % 4;   //校验和时4字节对齐。
    }

    if (iErr == InnoOk)
    {
        innoHeader = (INNO_HEAHER *)calloc(size, 1);
        innoItemIndex = (INNO_ITEM_INDEX *)((char *)innoHeader + sizeof(INNO_HEAHER));
        iErr = createItemIndex(innoItemIndex, count, sor);
    }

    if (iErr == InnoOk)
    {
        innoFp = fopen(absolutePath, "w+b");
        if (!innoFp)
        {
            iErr = InnoAccessFailed;
        }
    }

    if (iErr == InnoOk)
    {
        for (loop = 0; loop < count; loop++)
        {
            sorFp = fopen(sor[loop], "rb");
            if (sorFp == NULL)
            {
                iErr = InnoAccessFailed;
                break;
            }

            if (1 != fread((char *)innoHeader + innoItemIndex[loop].dataOffset, innoItemIndex[loop].itemSize, 1, sorFp))
            {
                iErr = InnoReadError;
            }

            fclose(sorFp);
            if (iErr != InnoOk)
                break;
        }
    }

    if (iErr == InnoOk)
    {
        innoHeader->magic = INNO_MAGIC;
        innoHeader->revision = INNO_REVISION;
        innoHeader->size = size;
        innoHeader->timestamp = (unsigned int)time(NULL); // 仅在创建和保存时记录时间
        innoHeader->reserved = 0;
        innoHeader->indexCount = count;
        innoHeader->indexOffset = sizeof(INNO_HEAHER);
        innoHeader->checkSum = 0 - InnoGetChecksum(innoHeader);
    }

    if (iErr == InnoOk)
    {
        if (fwrite(innoHeader, size, 1, innoFp) != 1)
        {
            iErr = InnoWriteError;
        }
    }

    switch (iErr)
    {
    case InnoOk:
    case InnoWriteError:
    case InnoAccessFailed:
        if (innoFp)
            fclose(innoFp);
        free(innoHeader);
    case InnoInvalidParam:
    default:
        break;
    }

    return iErr;
}

/*!
 * 生成sor格式文件
 * @param[in]  count             文件数量
 * @param[in]  sorPath           sor文件存储路径
 * @param[in]  innoHeader        inno文件
 * @param[out]  sorList          输出生成的文件列表
 * @return                       操作成功返回0
 */
static int sorCreate(unsigned int count, char * sorPath, INNO_HEAHER * innoHeader, char *** sorList)
{
    unsigned int loop;
    int iErr = InnoOk;
    INNO_ITEM_INDEX *innoItemIndex = NULL;
    FILE *sorFp = NULL;

    innoItemIndex = (INNO_ITEM_INDEX *)((char *)innoHeader + sizeof(INNO_HEAHER));

    for (loop = 0; loop < count; loop++)
    {
        char tempPath[512];

        sprintf(tempPath, "%s/%s", sorPath, innoItemIndex[loop].itemName);
        (*sorList)[loop] = (char *)calloc(1, strlen(tempPath) + 2);
        LOG(LOG_INFO, "%s\n", tempPath);
        memcpy((*sorList)[loop], tempPath, strlen(tempPath) + 1);

        sorFp = fopen(tempPath, "w+b");
        if (sorFp == NULL)
        {
            iErr = InnoWriteError;
            break;
        }

        if (iErr == InnoOk)
        {
            if (fwrite((char *)innoHeader + innoItemIndex[loop].dataOffset, innoItemIndex[loop].itemSize, 1, sorFp) != 1)
            {
                iErr = InnoWriteError;
            }
        }

        fclose(sorFp);
        if (iErr != InnoOk)
        {
            break;
        }
    }

    return iErr;
}

/*!
 * 加载inno格式文件
 * @param[in]  absolutePath      生成文件的存储路径(包含文件名)
 * @param[in]  sorPath           sor文件解析出来存放的路径
 * @param[out]  sorList          输出从文件中解析出来的sor文件列表(绝对路径)(注意使用完释放内存)
 * @param[out]  count            sor文件数量
 * @return                       操作成功返回0
 */
int InnoLoad(char *absolutePath, char *sorPath, char ***sorList, unsigned int *count)
{
    int iErr = InnoOk;
    INNO_HEAHER *innoHeader = NULL;
    unsigned int size = 0;
    FILE *innoFp = NULL;

    if (!absolutePath || !sorPath || !sorList || !count)
    {
        iErr = InnoInvalidParam;
    }

    if (iErr == InnoOk)
    {
        size = getFileSize(absolutePath);
        if (size < sizeof(INNO_HEAHER))
        {
            iErr = InnoAccessFailed;
        }
    }

    if (iErr == InnoOk)
    {
        innoFp = fopen(absolutePath, "rb");
        if (innoFp == NULL)
        {
            iErr = InnoAccessFailed;
        }
    }

    if (iErr == InnoOk)
    {
        innoHeader = (INNO_HEAHER *)calloc(size + 4, 1);
        if ((NULL == innoHeader) || (1 != fread(innoHeader, size, 1, innoFp)))
        {
            iErr = InnoReadError;
        }
    }

    if (iErr == InnoOk)
    {
        if (innoHeader->magic == INNO_MAGIC)
        {
            
        }
        else if (innoHeader->magic == SOR_MAGIC)
        {
            iErr = InnoSORFormat;
        }
        else
        {
            iErr = InnoBadFormat;
        }
    }

    if (iErr == InnoOk)
    {
        //检查校验和
        if (InnoGetChecksum(innoHeader) + innoHeader->checkSum != 0)
        {
            iErr = InnoBadChecksum;
        }
    }

    if (iErr == InnoOk)
    {
        *count = innoHeader->indexCount;
        *sorList = (char **)calloc((*count), sizeof(char *));
        iErr = sorCreate((*count), sorPath, innoHeader, sorList);
    }

    switch (iErr)
    {
    case InnoOk:
    case InnoWriteError:
    case InnoReadError:
        if (iErr != InnoOk && (*sorList))
        {
            unsigned int loop;
            for (loop = 0; loop < *count; loop++)
            {
                if ((*sorList)[loop])
                    free((*sorList)[loop]);
                (*sorList)[loop] = NULL;
            }
            free(*sorList);
            *sorList = NULL;
        }
    case InnoBadChecksum:
    case InnoBadFormat:
    case InnoSORFormat:
        if (innoHeader)
            free(innoHeader);
        fclose(innoFp);
    case InnoAccessFailed:
    case InnoFileNotExist:
    case InnoInvalidParam:
    default:
        break;
    }

    return iErr;
}