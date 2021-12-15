#ifndef _OPM_H
#define _OPM_H
#include "guiglobal.h"

#define OPM_VDD_KIND                  4   //OPM VDD 电压设置的种数
#define OPM_APD_KIND                  4   //OPM APD 电压设置的种数 
#define OPM_AMP_KIND         	  	  10   //OPM 电阻放大设置的种数 

typedef struct _opm_tuning  //光模块整定数据，保存在EEPROM中;在OTDR开机时读取;
{      
	unsigned int iVdd[OPM_VDD_KIND];        //Vdd 的档位个数，从eeprom里
    unsigned int iApd[OPM_APD_KIND];   //0-3,共4个级别，每个级别相差2.5db
    unsigned int iAtt[OPM_AMP_KIND];
   
    float        fTemperature;
    float        fTempCoefficient[OPM_APD_KIND];        //对应不同增益下的，温度曲线

	unsigned int OffsetPoint;        		//偏移点
} OPM_TUNING, *POPM_TUNING;
/*
说明；从OPM里，把整定参数读取上来
作者；李楚元
日期；2013.11.12
*/
void GetOpmTunning(void);
/*
说明；把整定参数，设置到光模块去
作者；李楚元
日期；2013.11.12
*/
void SaveOpmTunning(void);


int GetOpmTemperature (float *pTempValue);

int fpga_init(char *file);
int read_opt_reg(int fd, unsigned int addr, unsigned int *rd_val);
int write_opt_reg(int fd, unsigned int addr, unsigned int *wr_val);
int opt_read_data(int fd, unsigned short *data, unsigned short cnt);

int InitFpgaRegVal();
#endif 

