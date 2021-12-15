#ifndef __OPM_H__
#define __OPM_H__

#include "guiglobal.h"
#include "drvoptic.h"
#include <math.h>
#define FPGA_DATA_NAME	  SDCardDirectory"f216fpga_XIN_LX16.bin"

typedef unsigned char		U08;
typedef unsigned short 		U16;
typedef unsigned int	 	U32;

typedef signed char			S08;
typedef signed short		S16;
typedef signed int			S32;

typedef float               F32;

//定义光模块中所支持的数据采集模式;
typedef enum _opm_mode
{
    ENUM_DAQ_MODE_IIR 		= DAQMODE_IIR,
    ENUM_DAQ_MODE_SINGLE	= DAQMODE_SINGLE,
    ENUM_DAQ_MODE_ACC		= DAQMODE_ACC,
    ENUM_DAQ_MODE_TEST		= DAQMODE_TEST
} OPM_DAQ_MODE;

//定义光模块中所支持滤波参数;
typedef enum _opm_filter
{
	_IIR_2		= 0,
	_IIR_4		= 1,
	_IIR_8		= 2,
	_IIR_16		= 3,
	_IIR_32		= 4,
	_IIR_64		= 5,
	_IIR_128	= 6,
	_IIR_256	= 7,
    _IIR_512	= 8,
    _IIR_1K		= 9,
    _IIR_2K		= 10,
    _IIR_4K		= 11,
    _IIR_8K		= 12,
    _IIR_16K	= 13,
    _IIR_32K	= 14,
    _IIR_64K	= 15,
    _IIR_128K	= 16,
	_IIR_256K	= 17,
	_IIR_512K	= 18
} OPM_FILTER;


//定义光模块中所支持的数据操作类型;
typedef enum _opm_operation
{
   ENUM_DAQ_CMD_STOP		= DAQ_CMD_STOP,
   ENUM_DAQ_CMD_START		= DAQ_CMD_START,
   ENUM_DAQ_CMD_READ		= DAQ_CMD_READ,
   ENUM_DAQ_CMD_WRITE		= DAQ_CMD_WRITE,
   ENUM_DAQ_CMD_PAUSE		= DAQ_CMD_PAUSE,
   ENUM_DAQ_CMD_RESUME		= DAQ_CMD_RESUME
} OPM_DAQ_CMD;

/* Command list */
typedef enum _CommandList
{
    START_ADC = 0,
    STOP_ADC = 1,
    GET_ADC_VALUE = 2,
    WRITE_PGA112 = 3,
    READ_PGA112 = 4,
    TP70_VREF = 5,
    GET_ALL_ADC_VALUE = 6,
    CONNECT = 7,    
	GET_ZHIJU_ADC_VALUE = 8, 
	GET_Pulse_Width =9,	
    GET_ADC_VALUE_FCT = 10,
    GET_Pulse_Width_TP5 =11,
    ADC_CHECK = 12,
    VOLTAGE_1 = 13,
    VOLTAGE_0 = 14,
    CHANNEL_1 = 15,
	CHANNEL_2 = 16,
	CHANNEL_3 = 17,
	CHANNEL_4 = 18,
	CHANNEL_5 = 19,
	
} CommandList;

typedef enum _AdcChanel{
	ADC_CHANEL_TP5 = 0,    
	ADC_CHANEL_TP6 = 1,    
	ADC_CHANEL_TP15 = 2,    
	ADC_CHANEL_T70 = 3,    
	ADC_CHANEL_VREF = 4,    
	ADC_CHANEL_GND1 = 5,    
	ADC_CHANEL_TP1 = 6,    
	ADC_CHANEL_TP2 = 7,    
	ADC_CHANEL_TP3 = 8,    
	ADC_CHANEL_TP4 = 9,    
	ADC_CHANEL_GND2 = 10,    
	ADC_CHANEL_TP70_VREF = 11,	
	ADC_VSYS_TP97 = 12,	
	ADC_VSYS_1V8_TP103 = 13,	
	ADC_VSYSIO_3V3_TP109 = 14,	
	ADC_ARM_1V0_TP105 = 15,	
	ADC_VSYS_5V_TP107 = 16,	
	ADC_OPM_PWR_5V_TP113 = 17,	
	ADC_FPGA_CORE_1V2_TP100 = 18,	
	ADC_ADC_3V_TP99 = 19,	
	ADC_MCU_3V3_TP127 = 20,	
	ADC_OPM_7V_TP111 = 21,	
	ADC_VSYSIO_SD_3V3_TP24 = 22,	
	ADC_5V_A0 = 23,	
	ADC_5V_A1 = 24,	
	ADC_5V_A2 = 25,	
	ADC_3_3V_A_TP14 = 26,	
	ADC_7V_A_TP12 = 27,	
	ADC_OPM_7V_TP114 = 28,	
	ADC_F5V_A0 = 29,	
	ADC_F5V_A1 = 30,	
	ADC_F5V_A2 = 31,	
	ADC_F5V_A3 = 32,	
	ADC_FGND = 33,	
	ADC_F5V_APD = 34,	
	ADC_F6_5V_A_TP13 = 35,	
	ADC_TP1_0 = 36,	
	ADC_TP2 = 37,	
	ADC_TP3 = 38,	
	ADC_TP4 = 39,	
	ADC_TP1_7 = 40,	
	ADC_TP1_3 = 41,	
	ADC_TP1_1 = 42,	
} AdcChanel;

typedef enum _AdcRange
{
    ADCRANGE_20MV = 0,
    ADCRANGE_40MV = 1,
    ADCRANGE_80MV = 2,
    ADCRANGE_160MV = 3,
    ADCRANGE_320MV = 4,
    ADCRANGE_640MV = 5,
    ADCRANGE_1280MV = 6,
    ADCRANGE_2560MV = 7
} AdcRange;

#define OPM_APD_KIND        4
#define OPM_VDD_KIND        4
#define OPM_AMP_KIND   		10

#define BUFFER_SIZE         (20u)
#define COMMAND_LEN         (6u)

//error number
#define NOERR               (0)
#define INVALID_WAVELEN     (-1)
#define READSERIALTIMEOUT   (-2)
#define OPENSERIALFAIL      (-3)
#define READOPMREGFAIL      (-4)
#define WRITEOPMREGFAIL     (-5)
#define INITFPGAFAIL        (-6)
#define OPENOPTICFAIL       (-7)
#define TRYTIMESOUT         (-8)
#define WRONGCMDLENG        (-9)
#define WRONGADCCHANEL      (-11)
#define WRITESERIALERR      (-12)
#define REOPENSERIAL        (-13)

//error message
#define NOERR_MSG               "OK"
#define INVALID_WAVELEN_MSG     "Invalid Wave Length."
#define READSERIALTIMEOUT_MSG   "Read Serial Time Out."
#define OPENSERIALFAIL_MSG      "Open Serial Failed."
#define READOPMREGFAIL_MSG      "Read OPM Registor Failed."
#define WRITEOPMREGFAIL_MSG     "Write OPM Registor Failed."
#define INITFPGAFAIL_MSG        "Initialize FPGA Failed."
#define OPENOPTICFAIL_MSG       "Open OPM Device Failed."
#define TRYTIMESOUT_MSG         "Ajust try times out."
#define WRONGCMDLENG_MSG        "Receive Data Length Error."
#define WRONGADCCHANEL_MSG      "Receive Adc Chanel Error."
#define WRITESERIALERR_MSG      "Write Serial Failed."
#define REOPENSERIAL_MSG        "Reopen Serial."
#define UNKOWNERROR_MSG         "Unkown Error."

#define POINT_MAX 11

#define max(a,b) ((a) > (b))?(a):(b)

typedef struct _APD_BREAK_PARA
{
	U32 iVolt[100];
	float fTemp[100];
	int iNum;
}APD_BREAK_PARA;

extern S32 serialFD ;
extern S32 opticFD;

S32 OPMTestInit(void);
S32 OPMSetDacPulse(CommandList chanel , AdcRange range);
S32 OPMGetZhijuAdcValue( AdcChanel chanel , AdcRange range, F32 *value, U32 *value_len, U32 command, U32 command_len);
void GetSignalParameter(U32 iATT0);
int GetBreakVoltage(APD_BREAK_PARA *pApd_Break_Para);
void ChangeOpmTunning(U16 ibreakvoltage, float ftemp, float k);								  
void GetApdCoefficient(float * pk, float * pb, APD_BREAK_PARA * pApd_Break_Para);
void TempTuning(APD_BREAK_PARA * pApd_Break_Para, U32 DelayTime); 
void DrawCurve(void);

void SignalChainSet(U32 AmpValue);
void GetCheckFiberLen(U16* Data, int len);
float GetData();
float GetAttData(U32 AmpValue);
S32 IsEqual( F32 fin1, F32 fin2, F32 delta );
S32 OPMGetAdcValue( AdcChanel chanel , AdcRange range, F32 *value );

S32 OPMSetApd(U32 value);
S32 OPMSetVdd(U32 value);
S32 OPMSetAtt(U32 value);

S32 SerialInit( void );
S32 SerialClose( void );
S32 SerialSendData( U08 *data, U16 len );
S32 SerialRecvData(U08 *data ,U16 len, U32 command_len);

int Opm_LdPulse(int OnOff);
int BoardWorkInit();

//获得击穿电压，供光纤链路检查使用
int GetFiberpath(int wave);

int Opm_ReadAdcData(int adcCount, UINT16 *pDataBuf);
void CalShiftPhase4_90(unsigned char iCycle,unsigned char *buf);
int Opm_SetIntp(int iIntpReg);
int Opm_SetPhase();
int Opm_SetMode(OPM_DAQ_MODE enuDaqMode);
int Opm_SetFilter(OPM_FILTER enuFilterArg);
int Opm_SetDaqCmd(OPM_DAQ_CMD enuOperType);
int SedAdcParameter(UINT32 uiFiberLen);
int GetAdc_Data(UINT16	*pBuf,int iCount);
int Opm_GetDaqStatus();
int Opm_AdcReady();
int Opm_DataReady();
int Opm_ReadAdcData(int adcCount, UINT16 *pDataBuf);
void Opm_SetAmp(U32 Amp);
int Opm_EnablePwr(int OnOff);
int Opm_SetWave(int iWave);
int Opm_SetPulse(int iPulse);
UINT16 GetMeanUint16(UINT16 *pSignal,int iSignalLength);	
#endif /* __OPM_H__ */

