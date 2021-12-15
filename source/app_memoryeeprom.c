#include "app_memoryeeprom.h"

#include <stdlib.h>
#include <string.h>

typedef struct _memory_eeprom
{
    char *buffer;
    unsigned int size;
} MemoryEEprom;

static int MemoryEEprom_open(EEprom* ctx)
{
    (void)ctx;
    return 0;
}

static int MemoryEEprom_close(EEprom* ctx)
{
    (void)ctx;
    return 0;
}

static int MemoryEEprom_validAddr(EEprom*ctx, int pageIndex, unsigned int pageCount)
{
    return ((pageIndex + pageCount) < ctx->pageCount);
}

static int MemoryEEprom_read(EEprom* ctx, void* buffer, unsigned int pageIndex, unsigned int pageCount)
{
    if (!MemoryEEprom_validAddr(ctx, pageIndex, pageCount))
        return -1;

    unsigned int offset = ctx->startPageIndex + pageIndex;
    unsigned int size = pageCount * ctx->pageSize;

    MemoryEEprom* memEEprom = (MemoryEEprom*)(ctx->privateData);
    memcpy(buffer, (char*)memEEprom->buffer + offset, size);

    return 0;
}

static int MemoryEEprom_write(EEprom* ctx, void* buffer, unsigned int pageIndex, unsigned int pageCount)
{
    if (!MemoryEEprom_validAddr(ctx, pageIndex, pageCount))
        return -1;

    unsigned int offset = ctx->startPageIndex + pageIndex;
    unsigned int size = pageCount * ctx->pageSize;

    MemoryEEprom* memEEprom = (MemoryEEprom*)(ctx->privateData);
    memcpy((char*)memEEprom->buffer + offset, buffer, size);
    
    return 0;
}

static int MemoryEEprom_free(EEprom* ctx)
{
    MemoryEEprom* memEEprom = (MemoryEEprom*)(ctx->privateData);
    free(memEEprom->buffer);
    free(ctx);
    return 0;
}

static EEprom* MemoryEEprom_partition(EEprom* ctx, unsigned int pageCount, const char* name)
{
    return EEprom_newMemoryEEprom(ctx->pageSize, pageCount, name);
}

static void MemoryEEprom_print(void* data, FILE* fp)
{
    MemoryEEprom* memEEprom = (MemoryEEprom*)(data);
    fprintf(fp, "  MemoryEEprom: %p\n", memEEprom);
    fprintf(fp, "  size:         %d bytes\n", memEEprom->size);
    fprintf(fp, "  buffer:       %p\n", memEEprom->buffer);
}

static struct _eeprom_oprations memoryEEpromOprations =
{
    .open = MemoryEEprom_open,
    .close = MemoryEEprom_close,
    .read = MemoryEEprom_read,
    .write = MemoryEEprom_write,
    .free = MemoryEEprom_free,
    .partition = MemoryEEprom_partition,
    .print = MemoryEEprom_print,
};

EEprom* EEprom_newMemoryEEprom(unsigned int pageSize, unsigned pageCount, const char* name)
{
    int memSize = sizeof(EEprom) + pageCount * sizeof(EEpromBlock) + sizeof(MemoryEEprom);
    EEprom* eeprom = (EEprom*)malloc(memSize);
    if (eeprom)
    {
        memset(eeprom, 0, memSize);
        strncpy(eeprom->name, name, sizeof(eeprom->name));
        eeprom->startPageIndex = 0;
        eeprom->pageSize = pageSize;
        eeprom->pageCount = pageCount;
        eeprom->freePageCount = pageCount;

        MemoryEEprom* memEEprom = (MemoryEEprom*)(&eeprom[1]);
        eeprom->privateData = memEEprom;
        eeprom->blocks = (EEpromBlock*)(&memEEprom[1]);
        eeprom->maxBlockCount = pageCount;
        eeprom->blockCount = 0;

        eeprom->ops = &memoryEEpromOprations;
        memEEprom->size = pageCount * pageSize;
        memEEprom->buffer = (char*)malloc(memEEprom->size);

        eeprom->debug = 0;
    }
    return eeprom;
}