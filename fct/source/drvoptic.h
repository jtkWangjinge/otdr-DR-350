#ifndef _DRV_OPTIC_H
#define _DRV_OPTIC_H

#include <linux/ioctl.h>

#define FPGA_BASE_ADDR              (0x30000000)
#define FPGA_ADDR_SIZE              (70 << 1)

#define   FPGA_CTRLOUT_REG        (FPGA_BASE_ADDR+(0 <<1))
#define   FPGA_SPEAK_TIME         (FPGA_BASE_ADDR+(1 <<1))
#define   FPGA_TRIG_CTRL          (FPGA_BASE_ADDR+(2 <<1))
#define   FPGA_VERSION_ADDR       (FPGA_BASE_ADDR+(3 <<1))
#define   FPGA_OPM_APD_VOLTAGE    (FPGA_BASE_ADDR+(4 <<1))
#define   FPGA_VDD_VOLTAGE        (FPGA_BASE_ADDR+(5 <<1))
#define   FPGA_TUNE_ATT           (FPGA_BASE_ADDR+(6 <<1))
#define   FPGA_LD_PULSE_WIDTH     (FPGA_BASE_ADDR+(7 <<1))
#define   FPGA_ADC_CNT_L          (FPGA_BASE_ADDR+(8 <<1))
#define   FPGA_ADC_CNT_H          (FPGA_BASE_ADDR+(9 <<1))
#define   FPGA_DAQ_GAP            (FPGA_BASE_ADDR+(10<<1))
#define   FPGA_CPU_RD_DDR2_ADDRL  (FPGA_BASE_ADDR+(11<<1))
#define   FPGA_CPU_RD_DDR2_ADDRH  (FPGA_BASE_ADDR+(12<<1))
#define   FPGA_CPU_RD_DDR2_DATA   (FPGA_BASE_ADDR+(13<<1))
#define   FPGA_CPU_WR_DDR2_ADDRL  (FPGA_BASE_ADDR+(14<<1))
#define   FPGA_CPU_WR_DDR2_ADDRH  (FPGA_BASE_ADDR+(15<<1))
#define   FPGA_CPU_WR_DDR2_DATA   (FPGA_BASE_ADDR+(16<<1))
#define   FPGA_DAQ_CTRL           (FPGA_BASE_ADDR+(17<<1))
#define   FPGA_FILTER_SEL         (FPGA_BASE_ADDR+(18<<1))
#define   FPGA_DAQMODE_SET        (FPGA_BASE_ADDR+(19<<1))
#define   FPGA_DELAY_SET_P10      (FPGA_BASE_ADDR+(20<<1))
#define   FPGA_DELAY_SET_P32      (FPGA_BASE_ADDR+(21<<1))
#define   FPGA_DELAY_SET_P54      (FPGA_BASE_ADDR+(22<<1))
#define   FPGA_DELAY_SET_P76      (FPGA_BASE_ADDR+(23<<1))
#define   FPGA_DELAY_SET_P98      (FPGA_BASE_ADDR+(24<<1))
#define   FPGA_DELAY_SET_P11_10   (FPGA_BASE_ADDR+(25<<1))
#define   FPGA_DELAY_SET_P13_12   (FPGA_BASE_ADDR+(26<<1))
#define   FPGA_DELAY_SET_P15_14   (FPGA_BASE_ADDR+(27<<1))
#define   FPGA_DELAY_SET_P17_16   (FPGA_BASE_ADDR+(28<<1))
#define   FPGA_DELAY_SET_P19_18   (FPGA_BASE_ADDR+(29<<1))
#define   FPGA_DELAY_SET_P21_20   (FPGA_BASE_ADDR+(30<<1))
#define   FPGA_DELAY_SET_P23_22   (FPGA_BASE_ADDR+(31<<1))
#define   FPGA_DELAY_SET_P25_24   (FPGA_BASE_ADDR+(32<<1))
#define   FPGA_DELAY_SET_P27_26   (FPGA_BASE_ADDR+(33<<1))
#define   FPGA_DELAY_SET_P29_28   (FPGA_BASE_ADDR+(34<<1))
#define   FPGA_DELAY_SET_P31_30   (FPGA_BASE_ADDR+(35<<1))
#define   FPGA_DELAY_SET_P33_32   (FPGA_BASE_ADDR+(36<<1))
#define   FPGA_DELAY_SET_P35_34   (FPGA_BASE_ADDR+(37<<1))
#define   FPGA_DELAY_SET_P37_36   (FPGA_BASE_ADDR+(38<<1))
#define   FPGA_DELAY_SET_P39_38   (FPGA_BASE_ADDR+(39<<1))
#define   FPGA_DELAY_SET_P41_40   (FPGA_BASE_ADDR+(40<<1))
#define   FPGA_DELAY_SET_P43_42   (FPGA_BASE_ADDR+(41<<1))
#define   FPGA_DELAY_SET_P45_44   (FPGA_BASE_ADDR+(42<<1))
#define   FPGA_DELAY_SET_P47_46   (FPGA_BASE_ADDR+(43<<1))
#define   FPGA_DELAY_SET_P49_48   (FPGA_BASE_ADDR+(44<<1))
#define   FPGA_DELAY_SET_P51_50   (FPGA_BASE_ADDR+(45<<1))
#define   FPGA_DELAY_SET_P53_52   (FPGA_BASE_ADDR+(46<<1))
#define   FPGA_DELAY_SET_P55_54   (FPGA_BASE_ADDR+(47<<1))
#define   FPGA_DELAY_SET_P57_56   (FPGA_BASE_ADDR+(48<<1))
#define   FPGA_DELAY_SET_P59_58   (FPGA_BASE_ADDR+(49<<1))
#define   FPGA_DELAY_SET_P61_60   (FPGA_BASE_ADDR+(50<<1))
#define   FPGA_DELAY_SET_P63_62   (FPGA_BASE_ADDR+(51<<1))
#define   FPGA_CLOCK_CTRL         (FPGA_BASE_ADDR+(52<<1))
#define   FPGA_DDR_RESET          (FPGA_BASE_ADDR+(53<<1))
#define   FPGA_DNA_ENABLE         (FPGA_BASE_ADDR+(54<<1))
#define   FPGA_DNA_DATA1          (FPGA_BASE_ADDR+(55<<1))
#define   FPGA_DNA_DATA2          (FPGA_BASE_ADDR+(56<<1))
#define   FPGA_DNA_DATA3          (FPGA_BASE_ADDR+(57<<1))
#define   FPGA_DNA_DATA4          (FPGA_BASE_ADDR+(58<<1))
#define   OPM_CTRLOUT_REG         (FPGA_BASE_ADDR+(59<<1))
#define   DAC122S_DATA            (FPGA_BASE_ADDR+(60<<1))
#define   FPGA_LM74_READ          (FPGA_BASE_ADDR+(61<<1))
#define   FPGA_LM74_CTRL_DATA     (FPGA_BASE_ADDR+(62<<1))
#define   FPGA_LM74_CTRL          (FPGA_BASE_ADDR+(63<<1))
#define   FPGA_AD9609_INSTR       (FPGA_BASE_ADDR+(64<<1))
#define   FPGA_AD9609_DATA        (FPGA_BASE_ADDR+(65<<1))
#define   FPGA_SPEAK_FREQ_SET     (FPGA_BASE_ADDR+(66<<1))
#define   FPGA_SPEAK_PWM_SET      (FPGA_BASE_ADDR+(67<<1))
#define   FPGA_OPTICAL_PWM_SET    (FPGA_BASE_ADDR+(68<<1))    //?????
#define   FPGA_OPTICAL_POWER      (FPGA_BASE_ADDR + (69 << 1))    //测试otdr电源





#define bit0					(0x1<<0)
#define bit1					(0x1<<1)
#define bit2					(0x1<<2)
#define bit3					(0x1<<3)
#define bit4					(0x1<<4)
#define bit5					(0x1<<5)
#define bit6					(0x1<<6)
#define bit7					(0x1<<7)
#define bit8					(0x1<<8)
#define bit9					(0x1<<9)
#define bit10					(0x1<<10)
#define bit11					(0x1<<11)
#define bit12					(0x1<<12)
#define bit13					(0x1<<13)
#define bit14					(0x1<<14)
#define bit15					(0x1<<15)
#define bit16					(0x1<<16)
#define bit17					(0x1<<17)
#define bit18					(0x1<<18)
#define bit19					(0x1<<19)
#define bit20					(0x1<<20)
#define bit21					(0x1<<21)
#define bit22					(0x1<<22)
#define bit23					(0x1<<23)  
#define bit24					(0x1<<24)
#define bit25					(0x1<<25)
#define bit26					(0x1<<26)
#define bit27					(0x1<<27)
#define bit28					(0x1<<28)
#define bit29					(0x1<<29)
#define bit30					(0x1<<30)
#define bit31					(0x1<<31)

// FPGA_DDR_RESET里的位定义说明
#define RESET_DDR2				bit0
//FPGA_CLOCK_CTRL里的位定义说明
#define OMCLK_CTRL				bit0
//FPGA_CTRLOUT_REG里的位寄存器
#define VFL_MODE                	bit0
#define VFL_CW                      0x01
#define VFL_FLASH                   0x02

#define AD_CLK_EN         		    bit2

#define OPM_24LC_SDA         		bit3
#define OPM_24LC_CLK          		bit4
#define OPM_24LC_WP          		bit5

#define OPM_PWR_EN          		bit6
#define OPM_LD_EN           		bit7
#define SHORT_PULSE                 bit8

#define OPM_ADC3V_POWCON			bit9
#define OPM_ADC_PWR_EN				bit10

//温度
#define OPM_LM75_OS                 bit12
#define OPM_LM75_SCL                bit13
#define OPM_LM75_SDA                bit14

//寄存器名字 FPGA_TRIG_CTRL
#define SPEAK_CTRL                  bit0
#define APD_POWER_START             bit1
#define DAC122S_START               bit2
#define ADC_CONFIG_START            bit3
#define LM_START_CONV               bit4

//寄存器名字 OPM_CTRLOUT_REG
#define AMP_MASK                   0xff
//#define WAVE_CTRL_MASK             (0x03<<8)
//#define WAVE_1310                  (0x01<<8)
//#define WAVE_1550                  (0x02<<8)
#define WAVE_CTRL_MASK             (0x07<<8)
#define WAVE_1310                  (0x07<<8)
#define WAVE_1550                  (0x04<<8)
#define WAVE_1625                  (0x02<<8)
#define WAVE_OFF      

#define C_AMP_SW1               bit0
#define C_AMP_SW2               bit1
#define C_AMP_SW3               bit2
#define SI_1D5P                 bit3
#define SI_3D3P                 bit4


//正负5v电源控制
#define C_5V_OPM_CTL            bit12
#define _C_5V_OPM_CTL           bit13 

//寄存器名 FPGA_LM74_CTRL
#define CPU_WR_LMEN	               bit0   //配置LM74参数的写使能信号，1=使能，0=不使能
#define LM74_BUSY	               bit1   //LM74温度采集完成 1=正在采集
#define AD_BUSY	                   bit2   //1=AD9609正在配置，0=配置完成


//FPGA_DAQ_CTRL    bit name

#define ADC_DATA_READY	       		bit7
#define CPU_WR_DDR_READY			bit6
#define CPU_RD_DDR_READY			bit5
#define DAQ_STATUS                	bit4

#define DAQ_CMD_STOP           		0
#define DAQ_CMD_START         		1
#define DAQ_CMD_READ           		2
#define DAQ_CMD_WRITE         		3
#define DAQ_CMD_PAUSE           4
#define DAQ_CMD_RESUME          5
#define DAQ_CMD_UNUSE           6

#define DAQ_CMD_SHIFT           0
#define DAQ_CMD_MASK            7

//FPGA_DAQMODE_SET
#define DAQMODE_IIR         (0x00<<4)
#define DAQMODE_SINGLE   	(0x01<<4)
#define DAQMODE_ACC         (0x02<<4)
#define DAQMODE_TEST       	(0x03<<4)

#define DAQ_MODE_SHIFT           4
#define DAQ_MODE_MASK            7

#define C1_CALIB_DONE       bit0
#define C1_RST0             bit1
#define ADC_STATUE				bit2

#define DAQ_INTP_SHIFT          8
#define DAQ_INTP_MASK           (0xf<<DAQ_INTP_SHIFT)

#define DAQ_INTP01_01 (0<<8)  //采样1个点，输出一个点	
#define DAQ_INTP01_02 (1<<8)  //2倍移相插补	
#define DAQ_INTP01_04 (2<<8)  //4倍移相插补	
#define DAQ_INTP01_08 (3<<8)  //8倍移相插补	
#define DAQ_INTP01_16 (4<<8)  //16倍移相插补	
#define DAQ_INTP01_32 (5<<8)  //32倍移相插补	
#define DAQ_INTP01_64 (6<<8)  //64倍移相插补	
#define DAQ_INTP16_01 (7<<8)  //采样16个点，输出一个点	
#define DAQ_INTP08_01 (8<<8)  //采样8个点，输出一个点	
#define DAQ_INTP04_01 (9<<8)  //采样4个点，输出一个点	
#define DAQ_INTP02_01 (10<<8) //采样2个点，输出一个点


#ifndef OPT_IOCTLS_ARGUMENT
typedef struct _argument
{
    unsigned short usRegIndex;
    unsigned short usRegValue;
} OPTARGUMENT;

typedef struct _argument_dat
{
    int iDataCnt;
    void *pDataBuf;
} OPTARG_DAT;

#define OPT_IOCTLS_ARGUMENT
#endif

#define DEV_IOCTL_MAGIC     240

/*
    OPTARGUMENT arg;
    arg.usRegIndex = 0x1;
    ioctl(fd, OPT_IOCTL_RDREG, &arg);   //读取寄存器                     */
#define OPT_IOCTL_RDREG     _IOWR(DEV_IOCTL_MAGIC, 0, OPTARGUMENT *)    //读取寄存器
/*
    OPTARGUMENT arg;
    arg.usRegIndex = 0x1;
    arg.usRegValue = 0x23;
    ioctl(fd, OPT_IOCTL_WRREG, &arg);   //设置寄存器                     */
#define OPT_IOCTL_WRREG     _IOWR(DEV_IOCTL_MAGIC, 1, OPTARGUMENT *)    //设置寄存器

/*
    OPTARG_DAT arg;
    arg.iDataCnt = 8000;
    arg.pDataBuf = malloc(8000*2);
    ioctl(fd, OPT_IOCTL_RDDAT, &arg);   //读取数据                      */
#define OPT_IOCTL_RDDAT     _IOWR(DEV_IOCTL_MAGIC, 2, OPTARG_DAT *)     //读取数据  

/*
    ioctl(fd, OPT_IOCTL_GET_TEMP, &temp);
*/
#define OPT_IOCTL_GET_TEMP      _IOWR(DEV_IOCTL_MAGIC, 3, float *)	//获取温度

/*
    ioctl(fd, OPT_IOCTL_ERASE_EEPROM);
*/
#define OPT_IOCTL_ERASE_EEPROM	_IO(DEV_IOCTL_MAGIC, 4)				//擦出OPM上EEPROM上的所有数据

#define OPT_DEBUG
#ifdef OPT_DEBUG
#define opt_dbg(fmt, arg...) printk(KERN_DEBUG fmt, ##arg)
#else
#define opt_dbg(fmt, arg...)
#endif


unsigned short RdOptReg(unsigned short usRegIndex);
unsigned short WrOptReg(unsigned short usRegIndex, unsigned short usRegValue);

#endif  //_DRV_OPTIC_H

