#include "math.h"
#include "stdio.h"
#include "opm.h"
#include "string.h"
#include "crc32.h"
#include <sys/types.h>
#include <unistd.h>
#include "common_opm.h"
#include "drvoptic.h"
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#define OPMTUNNING_OFFSET	0		//EEPROM中保存OPMTUNNING参数的偏移

#define FPGA_MAGIC              'f'
#define PUT_FPGA_DATA_SIZE      _IOW(FPGA_MAGIC, 0, int)
#define LOAD_FPGA_DATA_START    _IO(FPGA_MAGIC, 1)

#define BUFF_SIZE				1024
#define FPGA_DEV_NAME			"/dev/fpga0"


extern POPM_TUNING  pOpmTunning;
 
extern S32 opticFD;

/*
说明；把整定参数，设置到光模块去
作者；李楚元
日期；2013.11.12
*/
void SaveOpmTunning()
{
	int len;
	len = sizeof(OPM_TUNING);

 	lseek(opticFD, OPMTUNNING_OFFSET, SEEK_SET);
	write(opticFD, (UINT8 *)pOpmTunning, len);

	return;
}

/*
说明；从OPM里，把整定参数读取上来
作者；李楚元
日期；2013.11.12
*/
void GetOpmTunning()
{
	int i;

	lseek(opticFD, OPMTUNNING_OFFSET, SEEK_SET);
	read(opticFD, (UINT8 *)pOpmTunning, sizeof(OPM_TUNING));
	
	printf("**********Get Opm Tunning************\n");
 	for (i=0; i<OPM_AMP_KIND; i++)
 	{
 		printf("pOpmTunning->iAtt[%d] = 0x%04x\n",i,(UINT16)pOpmTunning->iAtt[i]);
 	}

	for (i=0; i<OPM_AMP_KIND; i++)
 	{
 		pOpmTunning->iAtt[i] = 1;
 	}
	
  	for(i=0;i<OPM_VDD_KIND;i++)
  	{
		printf("pOpmTunning->iVdd[%d] = 0x%04x\n",i,(UINT16)pOpmTunning->iVdd[i]);
   	}
  	for(i=0;i<OPM_APD_KIND;i++)
  	{
		printf("pOpmTunning->iApd[%d] = 0x%04x\n",i,(UINT16)pOpmTunning->iApd[i]);
   	}
  	for(i=0;i<OPM_APD_KIND;i++)
  	{
		printf("pOpmTunning->fTempCoefficient[%d] = %f\n",i,pOpmTunning->fTempCoefficient[i]);
   	}
	printf("pOpmTunning->fTemperature = %f\n",pOpmTunning->fTemperature);

	printf("pOpmTunning->OffsetPoint = %d\n",pOpmTunning->OffsetPoint);
 	
    printf("**********Get Opm Tunning*************\n");
 }

/* 
说明: 
因为APD 击穿电压随温度改变而改变的原因， 
因此必须实时检测OPM 的温度 
备注: 
测试通过 
*/ 
int GetOpmTemperature(float *pTempValue) 
{
	int iErr = 0;

#if 0
	//定义温度临时变量
	UINT32 iReg = 0;
	//错误标志、返回值定义
	

	iReg = LM_START_CONV;
	write_opt_reg(opticFD, FPGA_TRIG_CTRL, &iReg);
	iReg = 0;
	while(1)
    {
        read_opt_reg(opticFD, FPGA_LM74_CTRL, &iReg);
		
        if(!(iReg & LM74_BUSY))
        	break;
		else
			printf("---LM--while 1---\n");
    }
	if(!iErr)
	{
		read_opt_reg(opticFD, FPGA_LM74_READ, &iReg);
		

		if((iReg >> 2) & 0x01)
		{
			//判断温度正负值
			if(!(iReg >> 15))
			{
				iReg = iReg >> 3;
				*pTempValue = iReg * 0.0625f;
			}
			else
			{
				iReg = (((~iReg) & 0x0000ffff) >> 3) + 1;
				*pTempValue = iReg * (-0.0625f);
			}
		}
		
	}
#endif

	 int temp = 0;
    //娓╁害璇诲彇鏂瑰紡淇敼涓篿ic锛岄�氳繃椹卞姩鐩存帴璇诲彇鑺墖娓╁害
    ioctl(opticFD, OPT_IOCTL_GET_TEMP, &temp);
    *pTempValue = (float)(temp / 1000.0f);

	return iErr;
}

int fpga_init(char *file)
{
	struct stat datastat;
	int dev_fd, bin_fd, len;
	unsigned char buff[BUFF_SIZE];

	/*get the fpgadata.bin's status*/
	if (stat(file, &datastat) == -1)
	{
		perror("stat");
		return -1;
	}

	/*open the fpga device*/
	dev_fd = open(FPGA_DEV_NAME, O_WRONLY);
	if (dev_fd < 0)
	{
		perror("fpga open()");
		return -2;
	}

	/*put the size of the file fpgadata.bin to driver*/
	if (ioctl(dev_fd, PUT_FPGA_DATA_SIZE, &datastat.st_size))
	{
		perror("iotcl()");
		return -3;
	}

	/*open the fpgadata.bin*/
	bin_fd = open(file, O_RDONLY);
	if (bin_fd < 0)
	{
		perror("bin open()");
		return -4;
	}

	/*start transfering data to driver*/
	while (1)
	{
		len = read(bin_fd, buff, BUFF_SIZE);
		if (len == 0)
		{
			printf("write done\n");
			close(bin_fd);
			break;
		}

		len = write(dev_fd, buff, len);
		if (len < 0)
		{
			close(bin_fd);
			return -5;
		}
	}

	/*load the fpgadata to fpga device*/
	if (ioctl(dev_fd, LOAD_FPGA_DATA_START))
	{
		perror("ioctl");
		return -6;
	}

	close(dev_fd);
	
	return 0;
}

int read_opt_reg(int fd, unsigned int addr, unsigned int *rd_val)
{
    OPTARGUMENT arg;
    int ret;

    arg.usRegIndex = (unsigned short) ((addr - FPGA_BASE_ADDR) >> 1);
    arg.usRegValue = (unsigned short) ((*rd_val) & 0xFFFF);
    ret = ioctl(fd, OPT_IOCTL_RDREG, &arg);
    *rd_val = arg.usRegValue;   //curr reg val

    return ret;
}

int write_opt_reg(int fd, unsigned int addr, unsigned int *wr_val)
{
    OPTARGUMENT arg;
    int ret;

    arg.usRegIndex = (unsigned short) ((addr - FPGA_BASE_ADDR) >> 1);
    arg.usRegValue = (unsigned short) ((*wr_val) & 0xFFFF);
    ret = ioctl(fd, OPT_IOCTL_WRREG, &arg);
    *wr_val = arg.usRegValue;   //Old reg val

    return ret;
}

int opt_read_data(int fd, unsigned short *data, unsigned short cnt)
{
    OPTARG_DAT arg;
    int ret;

    arg.iDataCnt= cnt;
    arg.pDataBuf = data;
    ret = ioctl(fd, OPT_IOCTL_RDDAT, &arg);
    
    return ret;
}

//初始化Opm_init的时候对这些寄存器值初始化
int InitFpgaRegVal()
{
	int iRet,iErr = 0;

	UINT32  uiRegVal = 0;
	
    iRet = read_opt_reg(opticFD, FPGA_CTRLOUT_REG, &uiRegVal);
	if(iRet)
		iErr = -1;
	printf("--1--FPGA_CTRLOUT_REG = 0x%x----\n",uiRegVal);
	uiRegVal &= (~SHORT_PULSE);//关闭短脉宽3ns
	//uiRegVal &= (~OPM_PWR_EN);//关闭模拟板电源

	printf("--2--FPGA_CTRLOUT_REG = 0x%x----\n",uiRegVal);
    iRet = write_opt_reg(opticFD, FPGA_CTRLOUT_REG, &(uiRegVal));
	if(iRet)
		iErr = -2;
				
	return iErr;
}