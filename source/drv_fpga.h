
#include <linux/ioctl.h>

#ifndef _DRV_FPGA_H
#define _DRV_FPGA_H

#define bit0   (0x1<<0)
#define bit1   (0x1<<1)
#define bit2   (0x1<<2)
#define bit3   (0x1<<3)
#define bit4   (0x1<<4)
#define bit5   (0x1<<5)
#define bit6   (0x1<<6)
#define bit7   (0x1<<7)
#define bit8   (0x1<<8)
#define bit9   (0x1<<9)
#define bit10  (0x1<<10)
#define bit11  (0x1<<11)
#define bit12  (0x1<<12)
#define bit13  (0x1<<13)
#define bit14  (0x1<<14)
#define bit15  (0x1<<15)
#define bit16  (0x1<<16)
#define bit17  (0x1<<17)
#define bit18  (0x1<<18)
#define bit19  (0x1<<19)
#define bit20  (0x1<<20)
#define bit21  (0x1<<21)
#define bit22  (0x1<<22)
#define bit23  (0x1<<23)  
#define bit24  (0x1<<24)
#define bit25  (0x1<<25)
#define bit26  (0x1<<26)
#define bit27  (0x1<<27)
#define bit28  (0x1<<28)
#define bit29  (0x1<<29)
#define bit30  (0x1<<30)
#define bit31  (0x1<<31)

#define FPGA_DONE           (0x1<<16)
#define FPGA_INIT_B         (0x1<<17)
#define FPGA_CSI_B          (0x1<<11)
#define FPGA_CCLK           (0x1<<10)
#define FPGA_D0             (0x1<<18)
#define FPGA_D1             (0x1<<27)
#define FPGA_D2             (0x1<<20)
#define FPGA_D3             (0x1<<21)
#define FPGA_D4             (0x1<<22)
#define FPGA_D5             (0x1<<23)
#define FPGA_D6             (0x1<<24)
#define FPGA_D7             (0x1<<25)
#define FPGA_PROG_B         (0x1<<9)
#define FPGA_RDWR_B         (0x1<<12)

#define AT91C_ID_PIOC       (4)         // Parallel IO Controller C
#define AT91C_BASE_AIC      0xFFFFF000  // (AIC) Base Address
#define AT91C_BASE_PIOC     0xFFFFF600  // (PIOC) Base Address
#define AT91C_BASE_PIOD     0xFFFFF800  // (PIOD) Base Address
#define AT91C_BASE_SMC      0xFFFFE800  // (SMC) Base Address
#define AT91C_BASE_PMC      0xFFFFFC00  // (PMC) Base Address

#define FPGA_ID             31
#define AIC_ICER_OFFSET     0x00000120

#define DATA_BUFF_LEN       0x0F000000  //the size of fpgadata.bin

#define FPGA_MAGIC              'f'
#define PUT_FPGA_DATA_SIZE      _IOW(FPGA_MAGIC, 0, int *)
#define LOAD_FPGA_DATA_START    _IO(FPGA_MAGIC, 1)
#define INIT_FPGA_EBI_CS2       _IO(FPGA_MAGIC, 2)

#endif

