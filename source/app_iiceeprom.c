#include "app_iiceeprom.h"
#include "app_global.h"

#include <stdlib.h>
#include <string.h>

#define I2C_EEPROM_ADDR     0x50
#define I2C_EEPROM_SLAVE    0x0703
#define I2C_EEPROM_RDWR     0x0707
#define I2C_EEPROM_WP       0x0709

#ifdef _WIN32
struct i2c_msg
{
    int             len;
    int             addr;
    int             flags;
    unsigned char*  buf;
};

typedef struct i2c_ioctl
{
    int nmsgs;
    struct i2c_msg* msgs;
} IicIoData;
#define open(_t1, _t2)          1
#define close(_t1)
#define ioctl(_t1, _t2, t3)     0
#define I2C_M_RD                1
#else
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>  
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

typedef struct i2c_rdwr_ioctl_data IicIoData;

#endif

#define IIC_EEPROM_PAGE_SIZE   128

typedef struct _iic_eeprom
{
    int fd;
    char devPath[256];
} IicEEprom;

static int IicEEprom_open(EEprom* ctx)
{
    IicEEprom* iicEEprom = (IicEEprom*)(ctx->privateData);
    if (iicEEprom->fd == -1)
    {
        iicEEprom->fd = open(iicEEprom->devPath, O_RDWR);
        if (iicEEprom->fd > 0)
        {
            return 0;
        }
        else
        {
            iicEEprom->fd = -1;
            return -1;
        }
    }
    return 0;
}

static int IicEEprom_close(EEprom* ctx)
{
    IicEEprom* iicEEprom = (IicEEprom*)(ctx->privateData);
    if (iicEEprom->fd != -1)
    {
        close(iicEEprom->fd);
        iicEEprom->fd = -1;
    }
    return 0;
}

static int IicEEprom_validAddr(EEprom* ctx, int pageIndex, unsigned int pageCount)
{
    return ((pageIndex + pageCount) < ctx->pageCount);
}

static int IicEEprom_polling(EEprom* ctx)
{
    IicEEprom* iicEEprom = (IicEEprom*)(ctx->privateData);
    if (iicEEprom->fd == -1)
    {
        LOG(LOG_ERROR, "Error: EEprom(%s) is not open\n", ctx->name);
        return -1;
    }

    unsigned char tmp = 0;

    struct i2c_msg msg;
    msg.len = 1;
    msg.addr = I2C_EEPROM_ADDR;
    msg.flags = I2C_M_RD;
    msg.buf = &tmp;

    IicIoData ioData;
    ioData.nmsgs = 1;
    ioData.msgs = &msg;

    int i, ret = 0;
    for (i = 0; i < 50; ++i)
    {
        ioctl(iicEEprom->fd, I2C_EEPROM_SLAVE, I2C_EEPROM_ADDR);
        ret = ioctl(iicEEprom->fd, I2C_EEPROM_RDWR, (unsigned long)&ioData);
        if (ret < 0)
            continue;
        else 
            break;
    }

    return ret < 0 ? -1 : 0;
}

static int IicEEprom_readPage(EEprom* ctx, void* buffer, unsigned int pageIndex)
{
    if (-1 == IicEEprom_polling(ctx))
        return -1;

    unsigned char wBuffer[2];
    wBuffer[0] = (unsigned char)(pageIndex >> 1);
    wBuffer[1] = (unsigned char)(pageIndex << 7);

    struct i2c_msg msgs[2];
    msgs[0].len = 2;
    msgs[0].addr = I2C_EEPROM_ADDR;
    msgs[0].flags = 0;
    msgs[0].buf = wBuffer;
    msgs[1].len = ctx->pageSize;
    msgs[1].addr = I2C_EEPROM_ADDR;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = (unsigned char*)buffer;

    IicIoData ioData;
    ioData.nmsgs = 2;
    ioData.msgs = &msgs[0];

    IicEEprom* iicEEprom = (IicEEprom*)(ctx->privateData);
    ioctl(iicEEprom->fd, I2C_EEPROM_RDWR, (unsigned long)&ioData);

    return 0;
}

static int IicEEprom_writePage(EEprom* ctx, void* buffer, unsigned int pageIndex)
{
    if (-1 == IicEEprom_polling(ctx))
        return -1;

    int wBufSize = ctx->pageSize + 2;
    unsigned char* wBuffer = (unsigned char*)malloc(wBufSize);
    if (!wBuffer)
        return -1;
    wBuffer[0] = (unsigned char)(pageIndex >> 1);
    wBuffer[1] = (unsigned char)(pageIndex << 7);
    memcpy(wBuffer + 2, buffer, ctx->pageSize);

    struct i2c_msg msg;
    msg.len = wBufSize;
    msg.addr = I2C_EEPROM_ADDR;
    msg.flags = 0;
    msg.buf = wBuffer;

    IicIoData ioData;
    ioData.nmsgs = 1;
    ioData.msgs = &msg;

    IicEEprom* iicEEprom = (IicEEprom*)(ctx->privateData);
    ioctl(iicEEprom->fd, I2C_EEPROM_SLAVE, I2C_EEPROM_ADDR);
    ioctl(iicEEprom->fd, I2C_EEPROM_WP, 0);
    ioctl(iicEEprom->fd, I2C_EEPROM_RDWR, (unsigned long)&ioData);
    ioctl(iicEEprom->fd, I2C_EEPROM_WP, 1);

    free(wBuffer);

    return 0;
}

static int IicEEprom_read(EEprom* ctx, void* buffer, unsigned int pageIndex, unsigned int pageCount)
{
    if (!IicEEprom_validAddr(ctx, pageIndex, pageCount))
        return -1;

    unsigned int startPageIndex = ctx->startPageIndex + pageIndex;

    unsigned int i;
    for (i = 0; i < pageCount; ++i)
    {
        if (-1 == IicEEprom_readPage(
            ctx, 
            (char*)buffer + ctx->pageSize * i, 
            startPageIndex + i)
            )
        {
            return -1;
        }
    }

    return 0;
}

static int IicEEprom_write(EEprom* ctx, void* buffer, unsigned int pageIndex, unsigned int pageCount)
{
    if (!IicEEprom_validAddr(ctx, pageIndex, pageCount))
        return -1;

    unsigned int startPageIndex = ctx->startPageIndex + pageIndex;

    unsigned int i;
    for (i = 0; i < pageCount; ++i)
    {
        if (-1 == IicEEprom_writePage(
            ctx, 
            (char*)buffer + ctx->pageSize * i, 
            startPageIndex + i)
            )
        {
            return -1;
        }
    }

    return 0;
}

static int IicEEprom_free(EEprom* ctx)
{
    IicEEprom_close(ctx);
    free(ctx);
    return 0;
}

static EEprom* IicEEprom_partition(EEprom* ctx, unsigned int pageCount, const char* name)
{
    //无法对已经分块的EEPROM进行分区
    if (ctx->blockCount != 0)
        return NULL;

    if (pageCount >= ctx->pageCount)
        return NULL;

    IicEEprom* iicEEprom = (IicEEprom*)(ctx->privateData);
    EEprom* eeprom = EEprom_newIicEEprom(iicEEprom->devPath, ctx->pageSize, pageCount, name);
    if (eeprom)
    {
        ctx->pageCount -= pageCount;
        ctx->freePageCount -= ctx->pageCount;
        ctx->maxBlockCount = ctx->pageCount;

        eeprom->startPageIndex = ctx->startPageIndex + ctx->pageCount;
    }

    return eeprom;
}

static void IicEEprom_print(void* data, FILE* fp)
{
    IicEEprom* eeprom = (IicEEprom*)(data);
    fprintf(fp, "  IicEEprom: %p\n", eeprom);
    fprintf(fp, "  fd:        %d\n", eeprom->fd);
    fprintf(fp, "  dev:       %s\n", eeprom->devPath);
}

static struct _eeprom_oprations iicEEpromOprations =
{
    .open = IicEEprom_open,
    .close = IicEEprom_close,
    .read = IicEEprom_read,
    .write = IicEEprom_write,
    .free = IicEEprom_free,
    .partition = IicEEprom_partition,
    .print = IicEEprom_print,
};

EEprom* EEprom_newIicEEprom(const char* devPath, unsigned int pageSize, unsigned int pageCount, const char* name)
{
    int memSize = sizeof(EEprom) + sizeof(IicEEprom) + pageCount * sizeof(EEpromBlock);
    EEprom* eeprom = (EEprom*)malloc(memSize);
    if (eeprom)
    {
        memset(eeprom, 0, memSize);
        strncpy(eeprom->name, name, sizeof(eeprom->name));
        eeprom->startPageIndex = 0;
        eeprom->pageSize = pageSize;
        eeprom->pageCount = pageCount;
        eeprom->freePageCount = pageCount;

        IicEEprom* iicEEprom = (IicEEprom*)(&eeprom[1]);
        eeprom->privateData = iicEEprom;
        iicEEprom->fd = -1;
        strncpy(iicEEprom->devPath, devPath, sizeof(iicEEprom->devPath));

        eeprom->blocks = (EEpromBlock*)(&iicEEprom[1]);
        eeprom->maxBlockCount = pageCount;
        eeprom->blockCount = 0;

        eeprom->ops = &iicEEpromOprations;
        eeprom->debug = 0;
    }
    return eeprom;
}
