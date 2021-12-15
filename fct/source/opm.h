#ifndef _OPM_H
#define _OPM_H
#include "guiglobal.h"

#define OPM_VDD_KIND                  4   //OPM VDD ��ѹ���õ�����
#define OPM_APD_KIND                  4   //OPM APD ��ѹ���õ����� 
#define OPM_AMP_KIND         	  	  10   //OPM ����Ŵ����õ����� 

typedef struct _opm_tuning  //��ģ���������ݣ�������EEPROM��;��OTDR����ʱ��ȡ;
{      
	unsigned int iVdd[OPM_VDD_KIND];        //Vdd �ĵ�λ��������eeprom��
    unsigned int iApd[OPM_APD_KIND];   //0-3,��4������ÿ���������2.5db
    unsigned int iAtt[OPM_AMP_KIND];
   
    float        fTemperature;
    float        fTempCoefficient[OPM_APD_KIND];        //��Ӧ��ͬ�����µģ��¶�����

	unsigned int OffsetPoint;        		//ƫ�Ƶ�
} OPM_TUNING, *POPM_TUNING;
/*
˵������OPM�������������ȡ����
���ߣ����Ԫ
���ڣ�2013.11.12
*/
void GetOpmTunning(void);
/*
˵�������������������õ���ģ��ȥ
���ߣ����Ԫ
���ڣ�2013.11.12
*/
void SaveOpmTunning(void);


int GetOpmTemperature (float *pTempValue);

int fpga_init(char *file);
int read_opt_reg(int fd, unsigned int addr, unsigned int *rd_val);
int write_opt_reg(int fd, unsigned int addr, unsigned int *wr_val);
int opt_read_data(int fd, unsigned short *data, unsigned short cnt);

int InitFpgaRegVal();
#endif 

