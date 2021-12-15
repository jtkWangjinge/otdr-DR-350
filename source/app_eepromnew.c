/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_eeprom.c
* 摘    要：  实现对EEPROM的分区管理
*
* 当前版本：  v1.0.0  
* 作    者：  
* 完成日期：    
*******************************************************************************/

#include "app_eepromnew.h"

#include <string.h>
#include <stdlib.h>

static unsigned int EepromCrc32(void *data, unsigned int length);

//EEPROM块的头信息
typedef struct _eeprom_block_header
{
    unsigned int crc32;          //数据的crc校验
    char name[64];
} EEpromBlockHeader;

#define BLOCK_HEADER_LEN         (sizeof(EEpromBlockHeader))


static void EEprom_printData(void* data, unsigned int size)
{
    unsigned int i = 0;
    for (i = 0; i < size; ++i)
    {
        printf(" %02x", ((unsigned char*)data)[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}

EEprom* EEprom_partition(EEprom* _this, unsigned int pageCount, const char* name)
{
    if (!_this)
        return NULL;
    return _this->ops->partition(_this,  pageCount, name);
}

int EEprom_open(EEprom* _this)
{
    if (!_this)
        return -1;

    return _this->ops->open(_this);
}

int EEprom_close(EEprom* _this)
{
    if (!_this)
        return -1;

    return _this->ops->close(_this);
}

int EEprom_free(EEprom* _this)
{
    if (!_this)
        return -1;

    return _this->ops->free(_this);
}

unsigned int EEprom_pageSize(EEprom* _this)
{
    if (!_this)
        return 0;
    return _this->pageSize;
}


int EEprom_readAllBlocks(EEprom* _this)
{
    unsigned int i;

    for (i = 0; i < _this->blockCount; ++i)
    {
        EEprom_readBlock(_this, i);
    }

    return 0;
}

int EEprom_syncAllBlocks(EEprom* _this)
{
    unsigned int i;

    for (i = 0; i < _this->blockCount; ++i)
    {
        EEprom_syncBlock(_this, i);
    }

    return 0;
}

int EEprom_readBlock(EEprom* _this, unsigned int index)
{
    if (index >= _this->blockCount)
        return -1;

    EEpromBlock* block = &_this->blocks[index];

    //EEPROM只能按页读写，这里需要将数据扩展为以页为单位, 最前4个字节为CRC校验
    char* buffer = (char*)calloc(1, block->pageCount * _this->pageSize);
    if (NULL != buffer)
    {
        int ret = _this->ops->read(_this, buffer, block->pageIndex, block->pageCount);
        if (0 == ret)
        {
            EEpromBlockHeader header = *((EEpromBlockHeader*)buffer);
            unsigned int crc32 = EepromCrc32(buffer + BLOCK_HEADER_LEN, block->size);
            if (_this->debug)
            {
                printf(
                    "Debug: EEprom_readBlock(_this:%p(%s), blockIndex:%d(name:%s, size:%d, crc:0x%08x))\n", 
                    _this, _this->name, index, block->name, block->size, header.crc32
                    );
                EEprom_printData(buffer + BLOCK_HEADER_LEN, block->size);
            }
            if (crc32 == header.crc32 && !strcmp(header.name, block->name))
            {
                memcpy(block->mapBuffer, buffer + BLOCK_HEADER_LEN, block->size);
                if (block->check && (0 != block->check(block->mapBuffer)))
                {
                    if (block->reset)
                    {
                        block->reset(block->mapBuffer);
                    }
                }
            }
            else
            {
                if (block->reset)
                {
                    block->reset(block->mapBuffer);
                }
                printf(
                    "Error: EEprom_readBlock(_this:%p(%s), headerName:%s, blockIndex:%d(name:%s, size:%d)) CRC ERROR\n", 
                    _this, _this->name, header.name, index, block->name, block->size
                    );
                ret = -1;
            }
        }

        free(buffer);
        return ret;
    }

    return -1;
}

int EEprom_readBlockByName(EEprom* _this, const char* name)
{
    int index = EEprom_indexOfBlock(_this, name);
    if (index != -1)
    {
        return EEprom_readBlock(_this, index);
    }
    return -1;
}

int EEprom_syncBlock(EEprom* _this, unsigned int index)
{
    if (index >= _this->blockCount)
        return -1;

    EEpromBlock* block = &_this->blocks[index];

    //EEPROM只能按页读写，这里需要将数据扩展为以页为单位, 最前4个字节为CRC校验
    char* buffer = (char*)calloc(1, block->pageCount * _this->pageSize);
    if (NULL != buffer)
    {
        EEpromBlockHeader header;
        header.crc32 = EepromCrc32(block->mapBuffer, block->size);
        strcpy(header.name, block->name);
        memcpy(buffer, &header, BLOCK_HEADER_LEN);
        memcpy(buffer + BLOCK_HEADER_LEN, block->mapBuffer, block->size);

        int ret = _this->ops->write(_this, buffer, block->pageIndex, block->pageCount);
        if (ret != -1)
        {
            if (_this->debug)
            {
                printf(
                     "Debug: EEprom_syncBlock(_this:%p(%s), headerName:%s, blockIndex:%d(name:%s, size:%d, crc:0x%08x))\n", 
                    _this, _this->name, header.name, index, block->name, block->size, header.crc32
                    );
                EEprom_printData(block->mapBuffer, block->size);
            }
        }

        free(buffer);
        return ret;
    }

    return -1;
}

int EEprom_syncBlockByName(EEprom* _this, const char* name)
{
    int index = EEprom_indexOfBlock(_this, name);
    if (index != -1)
    {
        return EEprom_syncBlock(_this, index);
    }
    return -1;
}

void* EEprom_mapBlockAtEnd(
    EEprom* _this, const char* name, unsigned int size, 
    void(*reset)(void*), int (*check)(void*)
    )
{
    if (!name)
        return NULL;

    unsigned int blockPageCount = (size + BLOCK_HEADER_LEN + _this->pageSize - 1) / _this->pageSize;
    if (_this->freePageCount < blockPageCount)
    {
        return NULL;
    }

    void* buffer = malloc(size);
    if (!buffer)
        return NULL;

    //根据已有的块来计算当前块的pageIndex
    int pageIndex = 0;
    if (_this->blockCount != 0)
    {
        EEpromBlock* lastUsedBlock = &_this->blocks[_this->blockCount - 1];
        pageIndex = lastUsedBlock->pageIndex + lastUsedBlock->pageCount;
    }

    EEpromBlock* theBlock = &_this->blocks[_this->blockCount];

    strncpy(theBlock->name, name, sizeof(theBlock->name));

    theBlock->mapBuffer = buffer;
    theBlock->size = size;
    theBlock->pageIndex = pageIndex;
    theBlock->pageCount = blockPageCount;
    theBlock->reset = reset;
    theBlock->check = check;

    _this->blockCount++;
    _this->freePageCount -= theBlock->pageCount;

    return buffer;
}

int EEprom_unmapEndBlock(EEprom* _this)
{
    if (_this->blockCount == 0)
        return 0;

    EEpromBlock* lastUsedBlock = &_this->blocks[_this->blockCount - 1];

    free(lastUsedBlock->mapBuffer);
    lastUsedBlock->mapBuffer = NULL;
    lastUsedBlock->check = NULL;
    lastUsedBlock->reset = NULL;
    lastUsedBlock->pageCount = 0;
    lastUsedBlock->pageIndex = 0;
    lastUsedBlock->size = 0;
    memset(&lastUsedBlock->name, 0, sizeof(lastUsedBlock->name));
    
    _this->freePageCount += lastUsedBlock->pageCount;
    _this->blockCount--;

    return 0;
}

int EEprom_indexOfBlock(EEprom* _this, const char* name)
{
    unsigned int i;
    int index = -1;
    
    for (i = 0; i < _this->maxBlockCount; ++i)
    {
        if (strcmp(name, _this->blocks[i].name) == 0)
        {
            index = (int)i;
            break;
        }
    }
    return index;
}

int EEprom_setDebug(EEprom* _this, unsigned int debug)
{
    if (!_this)
        return -1;
    _this->debug = debug;
    return 0;
}

void EEprom_resetAllBlock(EEprom* _this)
{
    unsigned int i;
    for (i = 0; i < _this->blockCount; ++i)
    {
        if (_this->blocks[i].reset)
        {
            _this->blocks[i].reset(_this->blocks[i].mapBuffer);
        }
    }
}

int EEprom_getData(EEprom* _this, void* buff, unsigned int size, unsigned int blockIndex)
{

    if(!_this || !buff)
    {
        return -1;
    }    

    if (blockIndex >= _this->blockCount)
        return -1;
    
    EEpromBlock* block = &_this->blocks[blockIndex];

    if (_this->debug)
    {
        printf(
            "Debug: EEprom_getData(_this:%p(%s), buff:%p, size:%d, blockIndex:%d(%s))\n",
            _this, _this->name, buff, size, blockIndex, block->name
            );
    }        

    if (size > block->size)
    {
        printf("Error: EEprom_getData size out of range@block:%s\n", block->name);
        return -1;
    }
    memcpy(buff, block->mapBuffer, size);
    
    return 0;
}

int EEprom_getDataByName(EEprom* _this, void* buff, unsigned int size, const char* blockName)
{
    if(!_this || !buff || !blockName)
    {
        return -1;
    }

    int index = EEprom_indexOfBlock(_this, blockName);
    if(index != -1)
    {
        return EEprom_getData(_this, buff, size, index);
    }
    return -1;
}

int EEprom_setData(EEprom* _this, void* buff, unsigned int size, unsigned int blockIndex)
{
    if(!_this || !buff)
    {
        return -1;
    }
    
    if (blockIndex >= _this->blockCount)
        return -1;

    EEpromBlock* block = &_this->blocks[blockIndex];

    if (_this->debug)
    {
        printf(
            "Debug: EEprom_setData(_this:%p(%s), buff:%p, size:%d, blockIndex:%d(%s))\n",
            _this, _this->name, buff, size, blockIndex, block->name
            );
    }
    
    if (size > block->size)
    {
        fprintf(stderr, "Error: EEprom_setData size out of range@block:%s\n", block->name);
        return -1;
    }
    memcpy(block->mapBuffer, buff, size);
    
    return 0;
}

int EEprom_setDataByName(EEprom* _this, void* buff, unsigned int size, const char* blockName)
{
    if(!_this || !buff || !blockName)
    {
        return -1;
    }

    int index = EEprom_indexOfBlock(_this, blockName);
    if(index != -1)
    {
       return EEprom_setData(_this, buff, size, index);
    }
    return -1;
}

int EEprom_readAbsPage(EEprom* _this, void* buff, unsigned int absolutePageIndex, unsigned int count)
{
    if(!_this || !buff)
    {
        return -1;
    }

    unsigned int endPageIndex = _this->startPageIndex + _this->pageCount;
    
    if ((_this->startPageIndex <= absolutePageIndex)
        && (absolutePageIndex + count <= endPageIndex))
    {
        if (_this->debug)
        {
            printf(
                "Debug: EEprom_readAbsPage(_this:%p(%s), buff:%p, absolutePageIndex:%d, count:%d)\n",
                _this, _this->name, buff, absolutePageIndex, count
                );
        }
        unsigned int index = absolutePageIndex - _this->startPageIndex;
        return _this->ops->read(_this, buff, index, count);
    }

    return -1;
}

void EEprom_print(EEprom* _this, FILE* fp)
{
    if (!_this)
        return;

    fprintf(fp, "EEprom(%p): %s\n[\n", _this, _this->name);
    fprintf(fp, "  startPageIndex:  %d\n", _this->startPageIndex);
    fprintf(fp, "  pageSize:        %d bytes\n", _this->pageSize);
    fprintf(fp, "  pageCount:       %d\n", _this->pageCount);
    fprintf(fp, "  maxBlockCount:   %d\n", _this->maxBlockCount);
    fprintf(fp, "  blockCount:      %d\n", _this->blockCount);

    fprintf(fp, "  all blocks(%p):\n  (\n", _this->blocks);
    unsigned int i;
    for (i = 0; i < _this->blockCount; ++i)
    {
        fprintf(fp, "    block:     %d\n", i);
        fprintf(fp, "    name:      %s\n", _this->blocks[i].name);
        fprintf(fp, "    size:      %d bytes\n", _this->blocks[i].size);
        fprintf(fp, "    pageIndex: %d\n", _this->blocks[i].pageIndex);
        fprintf(fp, "    pageCount: %d\n", _this->blocks[i].pageCount);
        fprintf(fp, "    mapBuffer: %p\n\n", _this->blocks[i].mapBuffer);
    }
    fprintf(fp, "  )\n");

    _this->ops->print(_this->privateData, fp);

    fprintf(fp, "]\n");
}

/*
 * Steps through buffer one byte at at time, calculates reflected
 * crc using table.
 */
static unsigned int EepromCrc32(void* data, unsigned int length)
{
    static const unsigned int crc32cTable[256] =
    {
        0x00000000L, 0xF26B8303L, 0xE13B70F7L, 0x1350F3F4L,
        0xC79A971FL, 0x35F1141CL, 0x26A1E7E8L, 0xD4CA64EBL,
        0x8AD958CFL, 0x78B2DBCCL, 0x6BE22838L, 0x9989AB3BL,
        0x4D43CFD0L, 0xBF284CD3L, 0xAC78BF27L, 0x5E133C24L,
        0x105EC76FL, 0xE235446CL, 0xF165B798L, 0x030E349BL,
        0xD7C45070L, 0x25AFD373L, 0x36FF2087L, 0xC494A384L,
        0x9A879FA0L, 0x68EC1CA3L, 0x7BBCEF57L, 0x89D76C54L,
        0x5D1D08BFL, 0xAF768BBCL, 0xBC267848L, 0x4E4DFB4BL,
        0x20BD8EDEL, 0xD2D60DDDL, 0xC186FE29L, 0x33ED7D2AL,
        0xE72719C1L, 0x154C9AC2L, 0x061C6936L, 0xF477EA35L,
        0xAA64D611L, 0x580F5512L, 0x4B5FA6E6L, 0xB93425E5L,
        0x6DFE410EL, 0x9F95C20DL, 0x8CC531F9L, 0x7EAEB2FAL,
        0x30E349B1L, 0xC288CAB2L, 0xD1D83946L, 0x23B3BA45L,
        0xF779DEAEL, 0x05125DADL, 0x1642AE59L, 0xE4292D5AL,
        0xBA3A117EL, 0x4851927DL, 0x5B016189L, 0xA96AE28AL,
        0x7DA08661L, 0x8FCB0562L, 0x9C9BF696L, 0x6EF07595L,
        0x417B1DBCL, 0xB3109EBFL, 0xA0406D4BL, 0x522BEE48L,
        0x86E18AA3L, 0x748A09A0L, 0x67DAFA54L, 0x95B17957L,
        0xCBA24573L, 0x39C9C670L, 0x2A993584L, 0xD8F2B687L,
        0x0C38D26CL, 0xFE53516FL, 0xED03A29BL, 0x1F682198L,
        0x5125DAD3L, 0xA34E59D0L, 0xB01EAA24L, 0x42752927L,
        0x96BF4DCCL, 0x64D4CECFL, 0x77843D3BL, 0x85EFBE38L,
        0xDBFC821CL, 0x2997011FL, 0x3AC7F2EBL, 0xC8AC71E8L,
        0x1C661503L, 0xEE0D9600L, 0xFD5D65F4L, 0x0F36E6F7L,
        0x61C69362L, 0x93AD1061L, 0x80FDE395L, 0x72966096L,
        0xA65C047DL, 0x5437877EL, 0x4767748AL, 0xB50CF789L,
        0xEB1FCBADL, 0x197448AEL, 0x0A24BB5AL, 0xF84F3859L,
        0x2C855CB2L, 0xDEEEDFB1L, 0xCDBE2C45L, 0x3FD5AF46L,
        0x7198540DL, 0x83F3D70EL, 0x90A324FAL, 0x62C8A7F9L,
        0xB602C312L, 0x44694011L, 0x5739B3E5L, 0xA55230E6L,
        0xFB410CC2L, 0x092A8FC1L, 0x1A7A7C35L, 0xE811FF36L,
        0x3CDB9BDDL, 0xCEB018DEL, 0xDDE0EB2AL, 0x2F8B6829L,
        0x82F63B78L, 0x709DB87BL, 0x63CD4B8FL, 0x91A6C88CL,
        0x456CAC67L, 0xB7072F64L, 0xA457DC90L, 0x563C5F93L,
        0x082F63B7L, 0xFA44E0B4L, 0xE9141340L, 0x1B7F9043L,
        0xCFB5F4A8L, 0x3DDE77ABL, 0x2E8E845FL, 0xDCE5075CL,
        0x92A8FC17L, 0x60C37F14L, 0x73938CE0L, 0x81F80FE3L,
        0x55326B08L, 0xA759E80BL, 0xB4091BFFL, 0x466298FCL,
        0x1871A4D8L, 0xEA1A27DBL, 0xF94AD42FL, 0x0B21572CL,
        0xDFEB33C7L, 0x2D80B0C4L, 0x3ED04330L, 0xCCBBC033L,
        0xA24BB5A6L, 0x502036A5L, 0x4370C551L, 0xB11B4652L,
        0x65D122B9L, 0x97BAA1BAL, 0x84EA524EL, 0x7681D14DL,
        0x2892ED69L, 0xDAF96E6AL, 0xC9A99D9EL, 0x3BC21E9DL,
        0xEF087A76L, 0x1D63F975L, 0x0E330A81L, 0xFC588982L,
        0xB21572C9L, 0x407EF1CAL, 0x532E023EL, 0xA145813DL,
        0x758FE5D6L, 0x87E466D5L, 0x94B49521L, 0x66DF1622L,
        0x38CC2A06L, 0xCAA7A905L, 0xD9F75AF1L, 0x2B9CD9F2L,
        0xFF56BD19L, 0x0D3D3E1AL, 0x1E6DCDEEL, 0xEC064EEDL,
        0xC38D26C4L, 0x31E6A5C7L, 0x22B65633L, 0xD0DDD530L,
        0x0417B1DBL, 0xF67C32D8L, 0xE52CC12CL, 0x1747422FL,
        0x49547E0BL, 0xBB3FFD08L, 0xA86F0EFCL, 0x5A048DFFL,
        0x8ECEE914L, 0x7CA56A17L, 0x6FF599E3L, 0x9D9E1AE0L,
        0xD3D3E1ABL, 0x21B862A8L, 0x32E8915CL, 0xC083125FL,
        0x144976B4L, 0xE622F5B7L, 0xF5720643L, 0x07198540L,
        0x590AB964L, 0xAB613A67L, 0xB831C993L, 0x4A5A4A90L,
        0x9E902E7BL, 0x6CFBAD78L, 0x7FAB5E8CL, 0x8DC0DD8FL,
        0xE330A81AL, 0x115B2B19L, 0x020BD8EDL, 0xF0605BEEL,
        0x24AA3F05L, 0xD6C1BC06L, 0xC5914FF2L, 0x37FACCF1L,
        0x69E9F0D5L, 0x9B8273D6L, 0x88D28022L, 0x7AB90321L,
        0xAE7367CAL, 0x5C18E4C9L, 0x4F48173DL, 0xBD23943EL,
        0xF36E6F75L, 0x0105EC76L, 0x12551F82L, 0xE03E9C81L,
        0x34F4F86AL, 0xC69F7B69L, 0xD5CF889DL, 0x27A40B9EL,
        0x79B737BAL, 0x8BDCB4B9L, 0x988C474DL, 0x6AE7C44EL,
        0xBE2DA0A5L, 0x4C4623A6L, 0x5F16D052L, 0xAD7D5351L
    };

    unsigned int crc = 0;
    unsigned char* d = (unsigned char*)data;

    while (length--) {
        crc = crc32cTable[(crc ^ *d++) & 0xFFL] ^ (crc >> 8);
    }

    return crc;
}
