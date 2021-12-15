/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_eeprom.h
* 摘    要：  实现对EEPROM的分区管理
*
* 当前版本：  v1.0.0  
* 作    者：
* 完成日期：
*
* 取代版本： 
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _APP_EEPROM_PARTITION_H
#define _APP_EEPROM_PARTITION_H

#include <stdio.h>

typedef struct _eeprom EEprom;
typedef struct _eeprom_block EEpromBlock;

struct _eeprom_oprations
{
    int (*open)(EEprom* ctx);
    int (*close)(EEprom* ctx);
    int (*read)(EEprom* ctx, void* buffer, unsigned int pageIndex, unsigned int pageCount);
    int (*write)(EEprom* ctx, void* buffer, unsigned int pageIndex, unsigned int pageCount);
    int (*free)(EEprom* ctx);
    EEprom* (*partition)(EEprom* ctx, unsigned int pageCount, const char* name);
    void (*print)(void* data, FILE* fp);
};

struct _eeprom_block
{
    char          name[64];             //名称
    unsigned int  pageIndex;            //块位于EEProm中的页地址 从0开始(是相对地址)
    unsigned int  size;                 //数据大小
    unsigned int  pageCount;            //存储大小，实际占用的Eeprom页，包含了块头信息所占的空间
    void*         mapBuffer;            //映射的内存空间
    void (*reset)(void*);               //reset函数
    int (*check)(void*);               //检验数据函数
};

struct _eeprom
{
    char name[64];                      //名称
    unsigned int startPageIndex;        //页起始地址，对于整块EEPROM来说就是0，分区的EEPROM为该分区的起始页
    unsigned int pageSize;              //页大小
    unsigned int pageCount;             //总共的页数量
    unsigned int freePageCount;         //空闲的页数量

    EEpromBlock* blocks;                //EEPROM被分成的所有块
    unsigned int maxBlockCount;         //块的最大个数
    unsigned int blockCount;            //已经使用的block的个数

    void* privateData;                  //对应的操作私有数据
    struct _eeprom_oprations* ops;      //EEPROM的操作集

    unsigned int debug;                 //调试标志
};

/*!
 * @brief       对指定的EEPROM进行分区
 * @param[in]   EEprom * _this     目标EEPROM
 * @param[in]   const char * name  新分区的名字
 * @param[in]   unsigned int pageCount 新分出的区的大小
 * @return      EEprom*            新分区EEPROM
 */
EEprom* EEprom_partition(EEprom* _this, unsigned int pageCount, const char* name);

int EEprom_open(EEprom* _this);
int EEprom_close(EEprom* _this);
int EEprom_free(EEprom* _this);

unsigned int EEprom_pageSize(EEprom* _this);

int EEprom_readAllBlocks(EEprom* _this);
int EEprom_syncAllBlocks(EEprom* _this);
int EEprom_readBlock(EEprom* _this, unsigned int index);
int EEprom_readBlockByName(EEprom* _this, const char* name);
int EEprom_syncBlock(EEprom* _this, unsigned int index);
int EEprom_syncBlockByName(EEprom* _this, const char* name);
void* EEprom_mapBlockAtEnd(
    EEprom* _this, const char* name, unsigned int size, 
    void(*reset)(void*), int (*check)(void*)
    );
int EEprom_unmapEndBlock(EEprom* _this);
int EEprom_indexOfBlock(EEprom* _this, const char* name);
int EEprom_setDebug(EEprom* _this, unsigned int debug);
void EEprom_resetAllBlock(EEprom* _this);

int EEprom_getData(EEprom* _this, void* buff, unsigned int size, unsigned int blockIndex);
int EEprom_getDataByName(EEprom* _this, void* buff, unsigned int size, const char* blockName);
int EEprom_setData(EEprom* _this, void* buff, unsigned int size, unsigned int blockIndex);
int EEprom_setDataByName(EEprom* _this, void* buff, unsigned int size, const char* blockName);

/*!
 * @brief       读取该分区中指定页的数据
 * @param[out]  buff   数据缓冲区
 * @param[in]   absolutePageIndex  绝对的页索引也就是相对于整块EEPROM中指定的页地址，而不是相对于分区
 * @param[in]   count  页的个数
 * @return      成功返回0 失败返回-1 (若指定的absolutePageIndex不在该分区内会失败)
 */
int EEprom_readAbsPage(EEprom* _this, void* buff, unsigned int absolutePageIndex, unsigned int count);

void EEprom_print(EEprom* _this, FILE* fp);

#endif //_APP_EEPROM_PARTITION_H
