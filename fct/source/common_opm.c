#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "common_opm.h"
#include "drvoptic.h"
#include "opm.h"
#include "guiglobal.h"
#include "wnd_frmmain.h"
#include "wnd_frmapd.h"
#include "wnd_frmtemp.h"
#include "wnd_global.h"

//device file path
#define DEV_TTY_NAME      "/dev/ttyS0"
#define DEV_OPTIC_NAME    "/dev/optic0"

//mask
#define MASK_APD_LOW        (0xffffU)    //16 bits DA
#define MASK_VDD            (0xffffU)    //16 bits DA
#define MASK_ATT           (0xffffU)    //16 bits DA
#define DELAY       (10000U)
#define SERIAL_TIME (5)                 //serial read time out (Second)

extern APD_BREAK_PARA *pApd_Break_Para;

OPM_TUNING	cOpmTunning = {};
POPM_TUNING  pOpmTunning = &cOpmTunning;

/* device file discriptor */
S32 fpgaFD = -1;
S32 serialFD = -1;
S32 opticFD = -1;

static U08 SendBuffer[BUFFER_SIZE];
static U08 RecvBuffer[BUFFER_SIZE];
static U32 signalMean = 0;
static F32 signalStd = 0.0f;
/* Serial port related */
static struct termios oldtio, newtio;
static struct timeval tv;
static fd_set rfds;


static F32 Abs(F32 fin);
static void SetTermios( struct termios *pNewtio, unsigned short uBaudRate);
static void Polyfit(float * pk, float * pb, APD_BREAK_PARA * pApd_Break_Para);


static F32 Abs(F32 fin)
{
    return (fin>0)?(fin):(-fin);
}


/* set the serial port */
static void SetTermios( struct termios *pNewtio, unsigned short uBaudRate )
{
    memset( pNewtio, 0, sizeof(struct termios) );

    /* Set baudrate 8 data bits, no parity, 1 stop */
    pNewtio->c_cflag = uBaudRate | CS8 | CREAD | CLOCAL;
    pNewtio->c_iflag = IGNPAR | IGNBRK;
    pNewtio->c_oflag = 0;
    pNewtio->c_lflag = 0; //no ICANON

    /* initialize all control characters, which are not needed now */
    pNewtio->c_cc[VINTR] = 0;       // Ctrl-c
    pNewtio->c_cc[VQUIT] = 0;       // Ctrl-'\'
    pNewtio->c_cc[VERASE] = 0;      // del
    pNewtio->c_cc[VKILL] = 0;       // @
    pNewtio->c_cc[VEOF] = 4;        // Ctrl-d
    pNewtio->c_cc[VTIME] = 10;       // inter-charcter time, timeout VTIME*0.1
    pNewtio->c_cc[VMIN] = COMMAND_LEN+1;       // blocking read until VMIN character 
                                    // arrives
    pNewtio->c_cc[VSWTC] = 0;       // '\0'
    pNewtio->c_cc[VSTART] = 0;      // Ctrl-q
    pNewtio->c_cc[VSTOP] = 0;       // Ctrl-s
    pNewtio->c_cc[VSUSP] = 0;       // Ctrl-z
    pNewtio->c_cc[VEOL] = 0;        // '\0'
    pNewtio->c_cc[VREPRINT] = 0;    // Ctrl-r
    pNewtio->c_cc[VDISCARD] = 0;    // Ctrl-u
    pNewtio->c_cc[VWERASE] = 0;     // Ctrl-w
    pNewtio->c_cc[VLNEXT] = 0;      // Ctrl-v
    pNewtio->c_cc[VEOL2] = 0;       // '\0'

    return;
}
/*
说明；拟合计算APD的温度系数等值
作者；林彦国
日期；2014.02.28
*/
void Polyfit(float *pk, float *pb, APD_BREAK_PARA *pApd_Break_Para)
{
	U16 i, num;
	float fSumx = 0.0f;
	float fSumy = 0.0f;

	float fAveX = 0.0f;
	float fAveY = 0.0f;

	num = pApd_Break_Para->iNum;
	if (num < 1)
	{
		printf("ERROR! Element is too little! \r\n ");
		return;
	}

	for (i = 0; i < num; ++i) //计算平均值
	{
		fAveX += pApd_Break_Para->fTemp[i];
		fAveY += (float)pApd_Break_Para->iVolt[i]; 
		printf("i = %d, fTemp = %5.2f, iVolt = %d\r\n", i, pApd_Break_Para->fTemp[i], pApd_Break_Para->iVolt[i]);
	}
	fAveX /= num;
	fAveY /= num;

	for (i = 0; i < num; ++i)
	{
		fSumy += (pApd_Break_Para->fTemp[i] - fAveX) * (pApd_Break_Para->iVolt[i] - fAveY);
		fSumx += (pApd_Break_Para->fTemp[i] - fAveX) * (pApd_Break_Para->fTemp[i] - fAveX);
	}

	*pk = fSumy / fSumx;
	*pb = fAveY - *pk * fAveX;
	printf("fSumx = %5.2f, fSumy = %5.2f\r\n", fSumx, fSumy);
	printf("k = %5.3f, b = %5.1f ,fAveX = %5.2f, fAveY = %5.2f\r\n ", *pk, *pb, fAveX, fAveY);
}

/**
 * if |fin1-fin2|<delta return 0
 * if fin1-fin2>delta return 1
 * if fin2-fin1>delta return -1
**/
S32 IsEqual( F32 fin1, F32 fin2, F32 delta )
{
    S32 ret = 0;
    F32 tmp = 0;
    
    tmp = Abs(fin1 - fin2);
    if (tmp <= delta)
    {
        ret = 0;
    }
    else if (fin1 > fin2)
    {
        ret = 1;
    }
    else
    {
        ret = -1;
    }

    return ret;
}


S32 SerialClose( void )
{
	if(serialFD != -1)
	{
		tcsetattr(serialFD, TCSANOW, &oldtio);
    	close(serialFD);
    	serialFD = -1;
	}
	return 0;
}

S32 SerialInit( void )
{
    S32 tryTimes = 0;
    S32 ret = 0;
    
    if (serialFD != -1) //have been opened
    {
        ret = REOPENSERIAL;
        
        return ret;
    }
    /* 1. open the serial port */
    tryTimes = 1;
    while (tryTimes--)
    {
        serialFD = open(DEV_TTY_NAME, O_RDWR);
        if( serialFD < 0 ) {
//            MsecSleep(1000);
            continue;
        }
        else
        {
            break;
        }
    }
    if (tryTimes < 0)
    {
        ret = OPENSERIALFAIL;
    }
    
    if (!ret)
    {
    /* 2. set serial port */
        tcgetattr(serialFD, &oldtio);     //save current serial port settings
        SetTermios(&newtio, B115200);     //baudrate 115200
        tcflush(serialFD, TCIFLUSH);
        tcsetattr(serialFD, TCSANOW, &newtio);
    /* 3. set receive data time-out */
        tv.tv_sec = SERIAL_TIME;
        tv.tv_usec = 0;
    /* 4. block */
        fcntl(serialFD, F_SETFL,0);
    }

    return ret;
}



S32 OPMSetApd(U32 value)
{
    S32 ret = 0;
	UINT32 uiApd = 0;
	UINT32 uiFpgaTrigCtrl = 0;
	uiApd = value & 0xFFF;
	
	ret = write_opt_reg(opticFD, FPGA_OPM_APD_VOLTAGE, &uiApd);
	MsecSleep(100);
	
	//设置完成后，进行使能
	read_opt_reg(opticFD, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
    uiFpgaTrigCtrl |= APD_POWER_START;
    ret = write_opt_reg(opticFD, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
	MsecSleep(100);
	
    return ret;
}

S32 OPMSetVdd(U32 value)
{
    S32 ret = 0;
    UINT32 uiVdd = 0;
	UINT32 uiFpgaTrigCtrl = 0;
	uiVdd = value & 0xFFF;
	
	ret = write_opt_reg(opticFD, FPGA_VDD_VOLTAGE, &uiVdd);
	MsecSleep(100);

	//设置完成后，进行使能
	read_opt_reg(opticFD, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
	uiFpgaTrigCtrl |= DAC122S_START;
    ret = write_opt_reg(opticFD, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
	MsecSleep(100);
	
    return ret;
}

S32 OPMSetAtt(U32 value)
{
    S32 ret = 0;
    UINT32 uiAttVolt = 0;
	UINT32 uiFpgaTrigCtrl = 0;
	uiAttVolt = value & 0xFFF;

	ret = write_opt_reg(opticFD, FPGA_TUNE_ATT, &uiAttVolt);
    MsecSleep(100);

	read_opt_reg(opticFD, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
	uiFpgaTrigCtrl |= DAC122S_START;
    ret = write_opt_reg(opticFD, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));	

    return ret;
}

S32 OPMTestInit( void )
{
    S32 ret = 0;

	float fTempValue;
	//ret = system("insmod /lib/modules/2.6.30/drv_fpga.ko");
	system("insmod /lib/modules/2.6.30/drv_fpga.ko");
    if (ret)
    {
        printf("insmod drv_fpga.ko failed\n");
    }
	
    ret = system("insmod /lib/modules/2.6.30/drv_optic.ko");
    if (ret)
    {
        printf("insmod drv_optic.ko failed\n");
    }

    opticFD = open(DEV_OPTIC_NAME, O_RDWR);
    if(opticFD < 0)
    {
        printf("open optic failed\n");
        ret = OPENOPTICFAIL;
    }
	else
	{
		GetOpmTunning();
		
		GetOpmTemperature(&fTempValue);

		ret = 0;
		printf("Temperature = %f\n", fTempValue);
	}

	InitFpgaRegVal();
	
    return ret;
}

S32 OPMSetDacPulse(CommandList chanel , AdcRange range)
{
	S32 ret = 0;
    S32 tryTime = 1;

	memset(SendBuffer, 0 , sizeof(SendBuffer));
	SendBuffer[0] = 3;
    SendBuffer[1] = chanel;
    SendBuffer[2] = range;
    SendBuffer[3] = '#';

	while (tryTime--)
	{
		/* send command to aduc848 */
        ret = SerialSendData(SendBuffer, SendBuffer[0]+1);
		if (ret<0)
        {
            ret = WRITESERIALERR;
            break;
        }
	}
	return ret;
}

S32 OPMGetAdcValue( AdcChanel chanel , AdcRange range, F32 *value )
{
    S32 ret = 0;
    S32 tryTime = 1;

    memset(SendBuffer, 0 , sizeof(SendBuffer));
    memset(RecvBuffer, 0 , sizeof(RecvBuffer));
    SendBuffer[0] = 4;
    SendBuffer[1] = GET_ADC_VALUE;
    SendBuffer[2] = chanel;
    SendBuffer[3] = range;
    SendBuffer[4] = '#';
    
    while (tryTime--)
    {
        /* send command to aduc848 */
        ret = SerialSendData(SendBuffer, SendBuffer[0]+1);
        if (ret<0)
        {
            ret = WRITESERIALERR;
            break;
        }
        ret = SerialRecvData(RecvBuffer, sizeof(RecvBuffer), COMMAND_LEN);
        if(ret)
        {
            break;
        }
        if (RecvBuffer[1] != chanel)
        {
            ret = WRONGADCCHANEL;
            break;
        }
        ((U08 *)value)[0] = RecvBuffer[5];
        ((U08 *)value)[1] = RecvBuffer[4];
        ((U08 *)value)[2] = RecvBuffer[3];
        ((U08 *)value)[3] = RecvBuffer[2];
    }
    
    return ret;
}

S32 OPMGetZhijuAdcValue( AdcChanel chanel , AdcRange range, F32 *value, U32 *value_len, U32 command, U32 command_len)
{
    S32 ret = 0;
    S32 tryTime = 1;

    memset(SendBuffer, 0 , sizeof(SendBuffer));
    memset(RecvBuffer, 0 , sizeof(RecvBuffer));
    SendBuffer[0] = 4;
    SendBuffer[1] = command;
    SendBuffer[2] = chanel;
    SendBuffer[3] = range;
    SendBuffer[4] = '#';
    
    while (tryTime--)
    {
        /* send command to aduc848 */
        ret = SerialSendData(SendBuffer, SendBuffer[0]+1);
        if (ret<0)
        {
            ret = WRITESERIALERR;
            break;
        }

        ret = SerialRecvData(RecvBuffer, sizeof(RecvBuffer), command_len);
        if (ret)
        {
            break;
        }
		
        if (RecvBuffer[1] != chanel)
        {
            ret = WRONGADCCHANEL;
            break;
        }
	
        ((U08 *)value)[0] = RecvBuffer[5];
        ((U08 *)value)[1] = RecvBuffer[4];
        ((U08 *)value)[2] = RecvBuffer[3];
        ((U08 *)value)[3] = RecvBuffer[2];

		if(RecvBuffer[0] == 10)
		{
			((U08 *)value_len)[0] = RecvBuffer[9];
			((U08 *)value_len)[1] = RecvBuffer[8];
			((U08 *)value_len)[2] = RecvBuffer[7];
			((U08 *)value_len)[3] = RecvBuffer[6];
		}
		
    }
    
    return ret;
}


void DrawCurve(void)
{
	GUIPEN *pPen = NULL;
	int lCoordinate1 = 42;	//width = 495,height = 225
	int lCoordinate2 = 56;
	int lCoordinate3 = 537;
	int lCoordinate4 = 281;
	int delta_x = 0;
	int delta_y = 0;
	int i = 0;
	int iMax = 80;
	int iDrawX[12] = {0};
	int iDrawY[12] = {0};

	int fd = 0;
	char buf[128] = {0};
	static int count = 0;
	fd = open("/mnt/sdcard/fct_log.txt",O_CREAT | O_APPEND | O_RDWR,0666);
	
	delta_x = lCoordinate3 - lCoordinate1;
	delta_y = lCoordinate4 - lCoordinate2;
	
	pPen = GetCurrPen();
	pPen->uiPenColor = 0xff0000;

			
	sprintf(buf,"--------%d---------\n\n\n\n",count);
	write(fd,buf,strlen(buf));
	
	for(i = 0; i < pApd_Break_Para->iNum; i++)
	{
		iDrawX[i] = (lCoordinate1 + 1) + (pApd_Break_Para->fTemp[i] / (float)iMax) * delta_x;
		iDrawY[i] = lCoordinate4 - (int)((((float)pApd_Break_Para->iVolt[i] - 2000.0f) / (float)1000.0f) * delta_y);
		DrawBlock((iDrawX[i] - 1), (iDrawY[i] - 1), (iDrawX[i] + 1), (iDrawY[i] + 1));
		//printf("--------------iDrawX[%d] = %d----iDrawY[%d] = %d----pApd_Break_Para->iVolt[i] = %d----\n", i,iDrawX[i],i,iDrawY[i],pApd_Break_Para->iVolt[i]);

		bzero(buf,sizeof(buf));
		sprintf(buf,"%d : %f %d, X:%d Y:%d\n\n",i,pApd_Break_Para->fTemp[i],pApd_Break_Para->iVolt[i],iDrawX[i],iDrawY[i]);
		write(fd,buf,strlen(buf));
		
	}
	for(i = 0;i < pApd_Break_Para->iNum - 1; i++)
	{
		//printf("--------------iDrawX[%d] = %d----iDrawY[%d] = %d----pApd_Break_Para->iVolt[i] = %d---\n", i,iDrawX[i],i,iDrawY[i],pApd_Break_Para->iVolt[i]);
		DrawLine(iDrawX[i], iDrawY[i], iDrawX[i + 1], iDrawY[i + 1]);
	}

	bzero(buf,sizeof(buf));
	sprintf(buf,"--------%d---------\n\n\n\n",count);
	write(fd,buf,strlen(buf));
	count++;

	close(fd);
}



S32 SerialSendData( U08 *data, U16 len )
{
    S32 ret = 0;

    ret = write(serialFD, data, len);
    tcdrain(serialFD);
    
    return ret;
}

S32 SerialRecvData(U08 *data ,U16 len, U32 command_len)
{
    S32 ret = 0;
    S32 nread = 0;
    
    FD_ZERO(&rfds);
    FD_SET(serialFD, &rfds);

    while (1)
    {		
        tv.tv_sec = SERIAL_TIME;
        if (select(1+serialFD, &rfds, NULL, NULL, &tv) > 0)
        {
            if (FD_ISSET(serialFD, &rfds))
            {
                memset(data, 0, len);
				MsecSleep(25);
                nread = read(serialFD, data, BUFFER_SIZE);
				printf("read len = %d\n", nread);
                if (nread != (command_len + 1))
                {
                    ret = WRONGCMDLENG;
                    break;
                    
                }
                else
                {
                    break;
                }
            }
        }
        else
        {        	
            ret = READSERIALTIMEOUT;
            break;
        }
    }
    return ret;
}

//
void GetSignalParameter(U32 iATT)
{
	U32 AmpValue[5] = {0x21, 0x25, 0x23, 0x27, 0x20};

	U32 tmp = 0;
	U32 tmpstd = 0;
	U32 i;
	U32 reg = 0;
	UINT16 *pDataBuf = NULL;

	OPM_FILTER filter = _IIR_256;
	OPM_DAQ_CMD iDaqCmd = ENUM_DAQ_CMD_STOP;
	int iLen = 1536; //!< 1024 + 512

	if(pDataBuf != NULL)
        free(pDataBuf);
	pDataBuf = (UINT16 *)calloc(iLen, sizeof(UINT16));
	
	reg = iATT;
	if((reg < 0x0900) || (reg > 0x0e00))
	{
		reg = 0x0a80;
	}
	OPMSetAtt(iATT);
	
	Opm_SetAmp(AmpValue[1]);
	//设置移相位1:1
	Opm_SetPhase();
	//设置DAQMOD_SET为1:1采集
	Opm_SetIntp(DAQ_INTP01_01);

	//设置数据采集模式	累加模式
    Opm_SetMode(DAQMODE_ACC);

	//设置滤波
    Opm_SetFilter(filter);

	//停止数据采集，设置DAQ_CTRL
	Opm_SetDaqCmd(iDaqCmd);


	//设置脉宽，现在是采集黑电平，由于激光器脉冲关了，所以不起作用
	//Opm_SetPulse(iOpmFd, pOpmSet, ENUM_PULSE_10NS);
	//设置波长，现在是采集黑电平，由于激光器脉冲关了，所以不起作用
	//Opm_SetWave(iOpmFd, pOpmSet, ENUM_WAVE_1550NM);

	
	//设置数据采集的ADC基本参数 分别是 采集个数和采集间隔
    SedAdcParameter(iLen);
	
	//得到采集的数据
    GetAdc_Data(pDataBuf, iLen);
	
	pDataBuf[0] = pDataBuf[1];
	
	for(i = 0; i < 512; i++)
	{
		tmp += pDataBuf[i];		
	}
	
	signalMean = (tmp / 512);
	printf("signalMean %d\n", signalMean);

	for(i = 0; i < 512; i++)
	{
		tmpstd += (pDataBuf[i] - signalMean) * (pDataBuf[i] - signalMean);
	}
	
	float fTemp = 0.0f;
	fTemp = (float)tmpstd / 512.0f;
	signalStd = sqrtf(fTemp);

	if(pDataBuf != NULL)
    {
        free(pDataBuf);
        pDataBuf = NULL;
    }
	
	return;
}

#if 1
int GetBreakVoltage(APD_BREAK_PARA *pApd_Break_Para)
{
    S32 iRet = 0,i = 0, j = 0;
	U16 iTemp = 0;
	float fTemp = 0.0f;
    F32 std1;
	S32 mean1;
	F32 std2 = 0.0;
	S32 mean2;
    U16 step1 = 0x05;
	U16 step2 = 0x03;
    U32 apd = 0x0800;
	U32 apd1 = 0x0800;
	U32 tryTime;
	float fTempValue = 0.0f;
	char buf[50];	
	int ret = 1;
	U32 iATT = 0x0a80;

	//读取当前温度
	GetOpmTemperature(&fTempValue);

	if(GetFlag() == 0)
		return 0;
	
    OPMSetApd(apd1);
	OPMSetVdd(0);
	//激光器脉冲关闭
	Opm_LdPulse(0);
	MsecSleep(500);
	
	if(GetFlag() == 0)
		return 0;
	
    GetSignalParameter(iATT);	
	
	if(GetFlag() == 0)
		return 0;
	
	printf("temperature = %f, mean = %d , std = %5.2f \n", fTempValue, signalMean, signalStd);

	if(GetFlag() == 0)
		return 0;
	
	do
	{
		if(GetFlag() == 0)
			return 0;
		
		GetSignalParameter(iATT);
		
		std1 = signalStd;
		std2 = std1;
		
		if(std2 < 1.0f)
		{
			std2 = 1.0f;
		}
		mean1 = signalMean;
	    mean2 = mean1;
		
		if(GetFlag() == 0)
			return 0;
		
		printf( "iATT = 0x%x, std1: %5.2f std2: %5.2f mean1: %d mean2: %d\n", iATT, std1, std2, mean1, mean2);
		
		MsecSleep(500);
		
		if(mean1 > 63000)
		{
			iATT -= 0x10;
		}
		else if(mean1 < 60000)
		{
			iATT += 0x10;
		}
		
		if((iATT > 0x0e00) || (iATT < 0x0900))
		{
			printf("GetSignalParameter ERROR! ATT = 0x%x, blacklevel = %d\n",iATT,mean1);
			ret = -1;
			return ret;
		}
	}while((mean1 > 63000) || (mean1 < 60000));
	
	apd = 0x0800 + (U32)(max(5 * fTempValue, 0));
	tryTime = 200;
	int upFlag = 0;
	int downFlag = 0;
    while(((std2 <= 5 * std1) || (std2 >= 10 * std1)) && (tryTime--))
	{
		if(GetFlag() == 0)
			return 0;
		
		GetSignalParameter(iATT);
		if(GetFlag() == 0)
			return 0;
		
		std2 = signalStd;
		mean2 = signalMean;
		printf("Apd: 0x%x , mean: %d , std: %5.2f , fTempValue: %f \n", apd, mean2, std2, fTempValue);
		sprintf((char *)buf, "Apd:0x%x,mean:%d,std:%5.2f", apd, mean2, std2);
		FrmTempSetInfo1((char *)buf);
		if(GetFlag() == 0)
			return 0;
		if(std2 <= 5 * std1)
		{
			upFlag = 1;
			if(downFlag)
			{
				apd += step1 / 2;
				break;
			}
			
	        apd += step1;
			
			if(apd > 0x0f00)
			{
				apd = 0x0800;
	   		 	iRet = OPMSetApd(apd);
				ret = -1;
				return ret;	
			}
	        OPMSetApd(apd);
	       
		}
		else if((std2 >= 10 * std1) || ((mean1 - mean2) > 15000))
		{
			downFlag = 1;
			if(upFlag)
			{
				apd -= step2 / 2;
				break;
			}
			
			apd -= step2;
	        OPMSetApd(apd);
		}
		else
		{
			break;
		}
		if(GetFlag() == 0)
			return 0;
		MsecSleep(200);
    }
	//读取当前温度
	GetOpmTemperature(&fTempValue);
	//对ApdPara数组进行赋值	
	pApd_Break_Para->iVolt[pApd_Break_Para->iNum] = apd; 
	pApd_Break_Para->fTemp[pApd_Break_Para->iNum] = fTempValue;
	
	sprintf((char *)buf, "%02d: ApdBreakVoltage = 0x%x ,fTempValue = %f", pApd_Break_Para->iNum + 1,
		pApd_Break_Para->iVolt[pApd_Break_Para->iNum], pApd_Break_Para->fTemp[pApd_Break_Para->iNum]);	
	//FrmTempSetInfo2((char *)buf);
	pApd_Break_Para->iNum += 1;
	
	if(GetFlag() == 0)
		return 0;
	
	//对结构体内的数值从低到高进行排序
	for(i = 0; i < pApd_Break_Para->iNum - 1; ++i)
	{
		for(j = i + 1; j < pApd_Break_Para->iNum; ++j)
		{
			if(GetFlag() == 0)
				return 0;
			
			if(pApd_Break_Para->fTemp[j] < pApd_Break_Para->fTemp[i])
			{
				fTemp = pApd_Break_Para->fTemp[i];
				iTemp = pApd_Break_Para->iVolt[i];
				pApd_Break_Para->fTemp[i] = pApd_Break_Para->fTemp[j];
				pApd_Break_Para->iVolt[i] = pApd_Break_Para->iVolt[j];
				pApd_Break_Para->fTemp[j] = fTemp;
				pApd_Break_Para->iVolt[j] = iTemp;
			}
		}
	}

	apd = 0x0800;
    iRet = OPMSetApd(apd);	
	
	if(GetFlag() == 0)
		return 0;
	
	MsecSleep(100);
	printf("GetBreakVoltage Success!\n");
	ret = 0;
	
	return ret;	
}
#endif
/*
说明；更改opmtunning整定值
作者；林彦国
日期；2014.01.02
*/
void ChangeOpmTunning(UINT16 ibreakvoltage, float ftemp, float k)
{
	int i = 0;
	UINT32 changvalue[4] = { 0, 40, 200, 670 }; 
	if (ibreakvoltage < 2000)
	{
		printf("BreakVoltage is too small，Set Apd Failed! \r\n ");
		return;
	}
		
	pOpmTunning->fTemperature = ftemp;
	for (i = 0; i < 4; ++i)
	{
		pOpmTunning->iApd[i] = ibreakvoltage - changvalue[i];
		pOpmTunning->fTempCoefficient[i] = k;
	} 		
}

/*
说明；计算温度系数
作者；林彦国
日期；2014.03.04
*/
void GetApdCoefficient(float * pk, float * pb, APD_BREAK_PARA * pApd_Break_Para)
{
	U16 i,j,index = 0;
	float delta = 0.0f, max_delta = 0.0f;
	APD_BREAK_PARA *pApd_Para = NULL;
	char buf[128];

	if (pApd_Break_Para->iNum < 3) //判断输入数据量是否正确
	{
		printf("Num = %d, GetApdCoefficient Failed!\r\n", pApd_Break_Para->iNum);
		sprintf(buf, "Num = %d, GetApdCoefficient Failed!", pApd_Break_Para->iNum);
		//FrmTempSetInfo2(buf);
		return;
	}
	
	if(GetFlag() == 0)
		return ;
	
	pApd_Para = (APD_BREAK_PARA *)calloc(1, sizeof(APD_BREAK_PARA));
	
	memcpy(pApd_Para, pApd_Break_Para, sizeof(APD_BREAK_PARA));
	
	if(GetFlag() == 0)
	{
		free(pApd_Para);
		pApd_Para = NULL;
		return;
	}

	Polyfit(pk, pb, pApd_Para);

	if(GetFlag() == 0)
	{
		free(pApd_Para);
		pApd_Para = NULL;
		return;
	}
	
	for (j = 0; j < 4; j++)
	{	
		if(GetFlag() == 0)
		{
			free(pApd_Para);
			pApd_Para = NULL;
			return ;
		}
		
		max_delta = 0.0000001f;		
		for (i = 0; i < pApd_Para->iNum; ++i)//打印拟合值与测量值之间的误差
		{
			if(GetFlag() == 0)
			{
				free(pApd_Para);
				pApd_Para = NULL;
				return ;
			}
			
			delta = (*pk) * pApd_Para->fTemp[i] + (*pb) - pApd_Para->iVolt[i]; 					
			printf("delta = %5.2f, ApdBreakVoltage = 0x%x, %d, fTempValue = %f\r\n",
				delta,pApd_Para->iVolt[i],pApd_Para->iVolt[i],pApd_Para->fTemp[i]);
			if ((delta / max_delta > 1.0f) || (delta / max_delta < -1.0f))
			{  				
				max_delta = delta;//寻找最大误差值及索引
				index = i;  				 
			}			
		}
		
		if(GetFlag() == 0)
		{
			free(pApd_Para);
			pApd_Para = NULL;
			return ;
		}
		
		printf("Num = %d, max_delta = %f \r\n",index,max_delta);
		if ((max_delta >= 50.0) || (max_delta <= -50.0))//删除误差最大点
		{
			for (i = index; i<pApd_Para->iNum; ++i)
			{
			 	pApd_Para->iVolt[i] = pApd_Para->iVolt[i + 1];
				pApd_Para->fTemp[i] = pApd_Para->fTemp[i + 1];
			}			
			pApd_Para->iNum -= 1;
			Polyfit(pk, pb, pApd_Para);//重新计算拟合值
		}
		else
		{				
			break;
		}
	}
	
	if(GetFlag() == 0)
	{
		free(pApd_Para);
		pApd_Para = NULL;
		return ;
	}
	if((j > 3) || (pApd_Break_Para->iNum < 7)) //判断APD整定值是否可靠，若误差过大点超过3个，认为整定不可靠
	{
		
		printf("TempNum = %d, DelNum = %d, GetApdCoefficient Failed!\r\n", pApd_Break_Para->iNum, j);
		if(GetFlag() == 0)
		{
			free(pApd_Para);
			pApd_Para = NULL;
			return ;
		}
		
		ChangeOpmTunning(*pb, 0.0f, *pk);  //输入分别为击穿电压、当前温度、温度系数
		SaveOpmTunning();			 //保存整定值
		if(GetFlag() == 0)
		{
			free(pApd_Para);
			pApd_Para = NULL;
			return ;
		}
		
        printf("SaveOpmTunning Over!\r\n");
		sprintf(buf, "Tunning Failed!");
		FrmTempSetInfo2(buf);
	}
	else
	{
		printf("TempNum = %d, DelNum = %d ,GetApdCoefficient Success！\r\n",pApd_Break_Para->iNum,j);
		
		if(GetFlag() == 0)
		{
			free(pApd_Para);
			pApd_Para = NULL;
			return ;
		}
		
		ChangeOpmTunning(*pb, 0.0f, *pk);  //输入分别为击穿电压、当前温度、温度系数
		
		SaveOpmTunning();			 //保存整定值
		
		if(GetFlag() == 0)
		{
			free(pApd_Para);
			pApd_Para = NULL;
			return ;
		}
		
        printf("SaveOpmTunning Over!\r\n");
		sprintf(buf, "k = %f, b = %f, Tunning Success!", *pk, *pb);
		FrmTempSetInfo2(buf);
	}
	
	printf("k = %f, b = %f\n", *pk, *pb);
	free(pApd_Para);
	pApd_Para = NULL;
	
	return;
}

#if 1
 /*
说明；高低温箱整定APD温度系数
作者；林彦国
日期；2014.04.29
*/
void TempTuning(APD_BREAK_PARA * pApd_Break_Para, U32 DelayTime)
{
	U32 i,j,DelayNum = 0,flag = 0;
	U32 TunNum = 0;
	float fCurTemp = 0.0f;
	float k = 0.0f, b = 0.0f;
	char buf[128];

	if(GetFlag() == 0)
		return ;

	{
		sprintf(buf, "Tunning Start! ");
		FrmTempSetInfo1(buf);
	}
	
	if(GetFlag() == 0)
		return ;
	
	printf("DelayTime = %d\n", DelayTime);
	//DelayTime = 0;//调试
	
	for(i = 0; i < DelayTime; ++i)//延时，以保证高低温箱温度稳定后测量
	{	
		if(GetFlag() == 0)
			return ;
		
		sprintf(buf, "DelayTime = %d min, CurTime = %d min", DelayTime, i);
// 		FrmTempSetInfo2(buf);
		
		for(j = 0; j < 60; ++j)//1分钟延时
		{
			if(GetFlag() == 0)
				return ;
			
			MsecSleep(1000); 
		}
	}
	
	do
	{
		if(GetFlag() == 0)
			return ;
		
		//读取当前温度
		GetOpmTemperature(&fCurTemp);
		
		if(GetFlag() == 0)
			return ;
		
		//判断该温度是否需要整定
		flag = 1;		
		for(i = 0; i < pApd_Break_Para->iNum; ++i)
		{
			if(GetFlag() == 0)
				return ;
			
			if(fabs(pApd_Break_Para->fTemp[i] - fCurTemp) < 2.0f/*4.0f*/)
			{
				flag = 0;
			}
		}
		
		if (flag == 1)//若温度变化超过一定值，开始测量击穿电压
		{
			if(GetFlag() == 0)
				return ;
			
			GetBreakVoltage(pApd_Break_Para);
			
			if(GetFlag() == 0)
				return ;
			
			initTraceArea();
			
			if(GetFlag() == 0)
				return ;
			
			DrawCurve();
			
			if(GetFlag() == 0)
				return ;
			
			TunNum += 1;	
			
			if(TunNum > 3)
			{
				GetApdCoefficient(&k, &b, pApd_Break_Para);
				
				if(GetFlag() == 0)
					return ;
				
    			OPMSetApd(0x0800);
			}
			
			if(GetFlag() == 0)
				return ;
			
			DelayNum = 0;
			
			flag = 0;
			
			sprintf(buf, "TunNum = %d", TunNum);
// 			FrmTempSetInfo1(buf);
		}
		else
		{	
			for (i = 0; i < 60; ++i)
			{
				MsecSleep(1000); //约为1m延时
			}
			
			if(GetFlag() == 0)
				return ;
			
			DelayNum += 1;
			
			sprintf(buf, "TunNum = %d, DelayNum = %d", TunNum, DelayNum);
// 			FrmTempSetInfo1(buf);
		}	
		
	}while(TunNum < 20/*10*/);
	
	sprintf(buf, "Tunning over!");
	FrmTempSetInfo1(buf);
}
#endif 

#if 0
 /*
说明；高低温箱整定APD温度系数
作者；林彦国
日期；2014.04.29
*/
void TempTuning(APD_BREAK_PARA * pApd_Break_Para, U32 DelayTime)
{
	U32 i,j,DelayNum = 0,flag = 0;
	U32 TunNum = 0;
	float fCurTemp = 0.0f;
	//float k = 0.0f, b = 0.0f;
	char buf[128];

	{
		sprintf(buf, "Temperature Tuning Start! ");
		FrmTempSetInfo1(buf);
	}
	
	for(i = 0; i < DelayTime; i++)//延时，以保证高低温箱温度稳定后测量
	{		
		for(j=0;j<60;j++)//1分钟延时
		{
			MsecSleep(1000); 
		}
	}
	
	do
	{	
		//读取当前温度
		GetOpmTemperature(&fCurTemp);
		
		//判断该温度是否需要整定
		flag = 1;		
		for(i = 0;i < pApd_Break_Para->iNum; i++)
		{
			if(fabs(pApd_Break_Para->fTemp[i] - fCurTemp) < 1.0f)
			{
				flag = 0;
			}
		}
		
		if (flag == 1)//若温度变化超过一定值，开始测量击穿电压
		{
			GetBreakVoltage(pApd_Break_Para);
			
			flag = 0;

			TunNum += 1;
			
			sprintf(buf, "/mnt/sdcard/%d.txt", TunNum);
			int fd = open(buf,O_CREAT | O_RDWR,666);
			int i = 0;
			for(i = 0; i < pApd_Break_Para->iNum; i++)
			{
				sprintf(buf, "%d %f %d 0x%X\n", i,pApd_Break_Para->fTemp[i],pApd_Break_Para->iVolt[i],pApd_Break_Para->iVolt[i]);
				write(fd,buf,strlen(buf));
			}
			close(fd);
			printf("-2---%d---\n",TunNum);
		}
		else
		{	
			for (i = 0;i < 30;i++)//30s延时
			{
				MsecSleep(1000); //约为1s延时
			}	
			DelayNum += 1;
		}	

		GetOpmTemperature(&fCurTemp);	
	}while(fCurTemp < 50.0);

	sprintf(buf, "Temperature Tuning over!");
	FrmTempSetInfo1(buf);
}


int GetBreakVoltage(APD_BREAK_PARA *pApd_Break_Para)
{
    S32 iRet = 0,i = 0, j = 0;
	U16 iTemp = 0;
	float fTemp = 0.0f;
    F32 std1;
	S32 mean1;
	F32 std2 = 0.0;
	S32 mean2;
    U16 step1 = 0x05;
	U16 step2 = 0x05;
    U32 apd = 0x0c90;
	U32 apd1 = 0x0b00;
	U32 tryTime;
	float fTempValue = 0.0f;
	//char buf[50];	
	int ret = 1;
	U32 iATT = 0x0730;

	//读取当前温度
	GetOpmTemperature(&fTempValue);

	
    OPMSetApd(apd1);
	OPMSetVdd(0);
	//激光器脉冲关闭
	Opm_LdPulse(0);
	MsecSleep(500);
	
    GetSignalParameter(iATT);	
	
	printf("temp = %f, mean = %d , std = %5.2f \n",fTempValue,signalMean,signalStd);


	do
	{
		GetSignalParameter(iATT);
		
		std1 = signalStd;
		std2 = std1;
		
		if(std2 < 1.0f)
		{
			std2 = 1.0f;
		}
		mean1 = signalMean;
	    mean2 = mean1;
		
		
		printf( "iATT = 0x%x, std1: %5.2f std2: %5.2f mean1: %d mean2: %d\n",iATT, std1, std2, mean1, mean2);
		
		MsecSleep(100);
		
		if(mean1 > 63000)
		{
			iATT -= 0x30;
		}
		else if(mean1 < 58000)
		{
			iATT += 0x30;
		}
		
		if((iATT > 0x0900) || (iATT < 0x0600))
		{
			printf("GetSignalParameter ERROR! ATT = 0x%x, blacklevel = %d\n",iATT,mean1);
			ret = -1;
			return ret;
		}
	}while((mean1 > 63000) || (mean1 < 58000));
	
	
	tryTime = 200;
    while(((std2 <= 20 * std1) || (std2 >= 80 * std1)) && (tryTime--))
	{
		GetSignalParameter(iATT);

		std2 = signalStd;
		mean2 = signalMean;

		printf( "std1: %5.2f std2: %5.2f mean1: %d mean2: %d\n",std1, std2, mean1, mean2);
		
		if(std2 <= 20 * std1)
		{
	        apd += step1;
			if(apd > 0x0dff)
			{
				apd = 0x0da0;
	   		 	iRet = OPMSetApd(apd);
				ret = -1;
				return ret;	
			}
	        OPMSetApd(apd);
	       
		}
		else if((std2 >= 80 * std1) || ((mean1 - mean2) > 15000))
		{
			apd -= step2;
	        OPMSetApd(apd);
		}

		MsecSleep(200);
    }
	//读取当前温度
	GetOpmTemperature(&fTempValue);
	//对ApdPara数组进行赋值	
	pApd_Break_Para->iVolt[pApd_Break_Para->iNum] = apd; 
	pApd_Break_Para->fTemp[pApd_Break_Para->iNum] = fTempValue;
	
	pApd_Break_Para->iNum += 1;
	
	
	//对结构体内的数值从低到高进行排序
	for(i=0;i<pApd_Break_Para->iNum-1;i++)
	{
		for(j=i+1;j<pApd_Break_Para->iNum;j++)
		{
			
			if(pApd_Break_Para->fTemp[j] < pApd_Break_Para->fTemp[i])
			{
				fTemp = pApd_Break_Para->fTemp[i];
				iTemp = pApd_Break_Para->iVolt[i];
				pApd_Break_Para->fTemp[i] = pApd_Break_Para->fTemp[j];
				pApd_Break_Para->iVolt[i] = pApd_Break_Para->iVolt[j];
				pApd_Break_Para->fTemp[j] = fTemp;
				pApd_Break_Para->iVolt[j] = iTemp;
			}
		}
	}

	apd = 0x0d00;
    iRet = OPMSetApd(apd);	
	
	MsecSleep(100);
	printf("GetBreakVoltage Success!\n");
	ret = 0;
	
	return ret;	
}

#endif
#if 0
float GetAttData(U32 AmpValue)
{
	UINT16 *pDataBuf = NULL;
	U32 tmp = 0;
	
	OPM_FILTER filter = _IIR_256;
	OPM_DAQ_CMD iDaqCmd = ENUM_DAQ_CMD_STOP;

	if(pDataBuf != NULL)
        free(pDataBuf);
	pDataBuf = (UINT16 *)calloc(1024, sizeof(UINT16));
	
	Opm_SetAmp(AmpValue);
	//设置移相位1:1
	Opm_SetPhase();
	//设置DAQMOD_SET为1:1采集
	Opm_SetIntp(DAQ_INTP01_01);

	//设置数据采集模式	累加模式
    Opm_SetMode(DAQMODE_ACC);

	//设置滤波
    Opm_SetFilter(filter);

	//停止数据采集，设置DAQ_CTRL
	Opm_SetDaqCmd(iDaqCmd);


	//设置脉宽，现在是采集黑电平，由于激光器脉冲关了，所以不起作用
	//Opm_SetPulse(iOpmFd, pOpmSet, ENUM_PULSE_10NS);
	//设置波长，现在是采集黑电平，由于激光器脉冲关了，所以不起作用
	//Opm_SetWave(iOpmFd, pOpmSet, ENUM_WAVE_1550NM);

	
	//设置数据采集的ADC基本参数 分别是 采集个数和采集间隔
    SedAdcParameter(1024);

	//得到采集的数据
    GetAdc_Data(pDataBuf,1024);


	pDataBuf[0] = pDataBuf[1];

	int i = 0;
	for(i = 0; i < 512; i++)
	{
		tmp += pDataBuf[i];		
	}
	
	F32 meandata = (tmp / 512.0f);
	
	printf("meandata %f\n",meandata);
	
	if(pDataBuf != NULL)
    {
        free(pDataBuf);
        pDataBuf = NULL;
    }
	
	return meandata;
}
#endif

void SignalChainSet(U32 AmpValue)
{

	
	OPM_FILTER filter = _IIR_256;
	OPM_DAQ_CMD iDaqCmd = ENUM_DAQ_CMD_STOP;
	
	Opm_SetAmp(AmpValue);
	//设置移相位1:1
	Opm_SetPhase();
	//设置DAQMOD_SET为1:1采集
	Opm_SetIntp(DAQ_INTP01_01);

	//设置数据采集模式	累加模式
    Opm_SetMode(DAQMODE_ACC);

	//设置滤波
    Opm_SetFilter(filter);

	//停止数据采集，设置DAQ_CTRL
	Opm_SetDaqCmd(iDaqCmd);


	//设置脉宽，现在是采集黑电平，由于激光器脉冲关了，所以不起作用
	//Opm_SetPulse(iOpmFd, pOpmSet, ENUM_PULSE_10NS);
	//设置波长，现在是采集黑电平，由于激光器脉冲关了，所以不起作用
	//Opm_SetWave(iOpmFd, pOpmSet, ENUM_WAVE_1550NM);
}


void GetCheckFiberLen(U16* Data, int len)
{
	U32 AmpValue[5] = {0x21, 0x25, 0x23, 0x27, 0x20};
	//fpga_init(FPGA_DATA_NAME);
	//MsecSleep(500);
	InitFpgaRegVal();
	BoardWorkInit();
	MsecSleep(200);

	Opm_LdPulse(1);

	//设置脉宽，
	Opm_SetPulse(0);
	//设置波长1310
	Opm_SetWave(0);//0
	Opm_SetAmp(AmpValue[0]);//鏈€灏忛€氶亾

	OPMSetAtt(0x0a80);
    OPMSetApd(0x0800);
	MsecSleep(20);
	OPMSetVdd(0x0680);
	MsecSleep(20);
	

	//设置滤波
    Opm_SetFilter(_IIR_256);
	
	//累加模式
	Opm_SetMode(DAQMODE_ACC);

	//设置DAQMOD_SET为1:1采集
	//Opm_SetPhase();
	Opm_SetIntp(DAQ_INTP01_64);


	Opm_SetDaqCmd(ENUM_DAQ_CMD_STOP);
	
	//设置数据采集的ADC基本参数 分别是 采集个数和采集间隔
    SedAdcParameter(len);           

	
	//得到采集的数据
    GetAdc_Data(Data, len); 
	printf("GetCheckFiberLen: len = %d\n", len);
	
	Opm_LdPulse(0);
}


float GetData()
{
	UINT16 *pDataBuf = NULL;
	U32 tmp = 0;
	
	if(pDataBuf != NULL)
        free(pDataBuf);
	pDataBuf = (UINT16 *)calloc(24, sizeof(UINT16));
	
	//设置数据采集的ADC基本参数 分别是 采集个数和采集间隔
    SedAdcParameter(1024);

	//得到采集的数据
    GetAdc_Data(pDataBuf,24);

	pDataBuf[0] = pDataBuf[1];

	int i = 0;
	for(i = 0; i < 20; i++)
	{
		tmp += pDataBuf[i];		
	}
	
	F32 meandata = (tmp / 20.0f);
	
	printf("meandata %f\n",meandata);
	
	if(pDataBuf != NULL)
    {
        free(pDataBuf);
        pDataBuf = NULL;
    }
	
	return meandata;
}

UINT16 GetMeanUint16(UINT16 *pSignal,int iSignalLength)
{
	int i=0;
	int iSumNumber = 0;

	for (i=0; i<iSignalLength; i++)
	{
			iSumNumber = iSumNumber + pSignal[i];
	}

	return iSumNumber / iSignalLength;
}


/***********************************************
*返回float型数组的最大值
*作者:Lin Peihua
*时间:2013-12-12
***********************************************/
float GetMaxFloat(                   //返回数据最大值
		float *pSignal,      //进的数据的数组指针
		int iSignalLength) //输入输出的数据长度
{
	float fMaxNumber = -1000.0f;
	int i = 0;

	for (i=0; i<iSignalLength; i++)
	{
		if (pSignal[i]>fMaxNumber)
		{
			fMaxNumber = pSignal[i];
		}
	}

	return fMaxNumber;
}

/***********************************************
*返回float型数组的平均值
*作者:Lin Peihua
*时间:2013-12-12
***********************************************/
float GetMeanFloat(                   //返回数据平均值
		float *pSignal,      //进的数据的数组指针
		int iSignalLength) //输入输出的数据长度   
{
	int i=0;
	float fSumNumber = 0;

	for (i=0; i<iSignalLength; i++)
		fSumNumber = fSumNumber + pSignal[i];

	return fSumNumber / (float)iSignalLength;
}


//供光纤链路检查提供的函数，检测击穿电压
int GetFiberpath(int wave)
{
	U32 AmpValue[5] = {0x21, 0x25, 0x23, 0x27, 0x20};
	
	S32 i = 0,j = 0;
    F32 std1;
	S32 mean1;
	F32 std2 = 0.0;
	S32 mean2;
    U16 step1 = 0x0c;
	U16 step2 = 0x06;
	U32 apd = 0x0800;
	U32 tryTime;	
	U32 iATT = 0x0a80;
	int iFlag = 0;

	int temp = 0;
	
	int Len = 8704;					//!< 8192+512
	float fMean = 0.0f;
	float fMax = 0.0f;
	//float fDelta = 0.0f;
	int iBlackLevel = 0;

	const float DYNARANGE_1550 = 9.7f;
	const float DYNARANGE_1310 = 12.4f;
	const float DYNARANGE_DELTA = 0.0f;
	
	U16* data = (U16*)malloc(sizeof(U16) * Len);
	F32* LogSigTable = malloc(sizeof(F32) * 65536);
	F32* log_data = (F32*)malloc(sizeof(F32) * Len);
	//float log_data[8192] = {0.0f};
	/*!
	 *!对数表
	*/
	LogSigTable[0] = 0.0f;
	for (i = 1; i < 65536; ++i)
	{
		LogSigTable[i] = 5.0f * log10f((float)i);
	}
	

	InitFpgaRegVal();
	BoardWorkInit();
	MsecSleep(200);

	//------------击穿电压检测----------
    OPMSetApd(apd);
	OPMSetVdd(0);
	//激光器脉冲关闭
	Opm_LdPulse(0);
	MsecSleep(500);

    GetSignalParameter(iATT);	
	
	do
	{
		GetSignalParameter(iATT);
		
		std1 = signalStd;
		std2 = std1;
		
		if(std2 < 1.0f)
		{
			std2 = 1.0f;
		}
		mean1 = signalMean;
	    mean2 = mean1;
		
		MsecSleep(500);
		
		if(mean1 > 63000)
		{
			iATT -= 0x10;
		}
		else if(mean1 < 60000)
		{
			iATT += 0x10;
		}
		
		if((iATT > 0x0e00) || (iATT < 0x0900))
		{
			printf("GetSignalParameter ERROR! ATT = 0x%x, blacklevel = %d\n",iATT,mean1);
			//free(data);
			//free(LogSigTable);
			break;
			//return -1;
		}
	}while((mean1 > 63000) || (mean1 < 60000));
	
	apd = 0x800;
	OPMSetApd(apd);
	MsecSleep(200);
	iFlag = 0;
	
	tryTime = 200;
    while(((std2 <= 5 * std1) || (std2 >= 10 * std1)) && (tryTime--))
	{
		GetSignalParameter(iATT);
		
		std2 = signalStd;
		mean2 = signalMean;
		printf("Apd: 0x%x , mean: %d , std: %5.2f \n",apd,mean2,std2);

		if(std2 <= 5 * std1)
		{
			if (iFlag)
			{
				printf("----break\n");
				break;
			}
				
	        apd += step1;
	        OPMSetApd(apd);
	       
		}
		else if((std2 >= 10 * std1) || ((mean1 - mean2) > 15000))
		{
			apd -= step2;
	        OPMSetApd(apd);
			iFlag = 1;
		}
		else
		{
			break;
		}
		MsecSleep(200);
    }
	//------------击穿电压检测----------

	//------------采集数据----------

	printf("apd = %d,iFlag = %d\n", apd, iFlag);
	apd -= 0x38;
	OPMSetApd(apd);
	MsecSleep(200);
	//激光器脉冲打开
	Opm_LdPulse(1);
	
	//FILE *fp = NULL;
	
	fMean = 0.0f;
	for(j = 0; j < 10; ++j)
	{
		//printf("write file---%d\n",j);
		OPMSetAtt(iATT);
		OPMSetVdd(0x0680);
		//设置脉宽，
		Opm_SetPulse(100);
		//设置波长
		Opm_SetWave(wave);//0
		Opm_SetAmp(AmpValue[1]); //15k閫氶亾
		//设置移相位1:1
		Opm_SetPhase();
		//设置DAQMOD_SET为1:1采集
		Opm_SetIntp(DAQ_INTP01_01);

		//设置数据采集模式	累加模式
	    Opm_SetMode(DAQMODE_ACC);

		//设置滤波
	    Opm_SetFilter(_IIR_256);

		//停止数据采集，设置DAQ_CTRL
		Opm_SetDaqCmd(ENUM_DAQ_CMD_STOP);

		//设置数据采集的ADC基本参数 分别是 采集个数和采集间隔
    	SedAdcParameter(Len);

		//得到采集的数据
	    GetAdc_Data(data,Len);

		//save data
		#if 0
		char dataname[512] = {0};
		sprintf(dataname,"/mnt/usb/data[%d].txt",j);
		fp = fopen(dataname,"w+");
		if (fp != NULL)
		{
			for(i = 0; i < Len; i++)
			{
				fprintf(fp,"%d\n",data[i]);
			}
			fclose(fp);
		}
		#endif
		
		iBlackLevel = (int)floor(GetMeanUint16(data + Len - 512, 512) + 0.5f);
		printf("iBlackLevel = %d\n", iBlackLevel);

		for(i = 0; i < Len; i++)
		{
			temp = iBlackLevel - (int)data[i];

			//printf("data[0] = %d ; 1+temp = %d  -\n",data[i], 1+temp);
			if ((temp > 0) && (temp < 65535))
			{
				log_data[i] = LogSigTable[1+temp];
			}
			else if((temp < 0) && (temp > -65535))
			{
				log_data[i] = LogSigTable[1-temp];
			}
			else
			{
				log_data[i] = 0.0f;
			}
		}
		
		//计算噪声最大值
		fMax = GetMaxFloat(log_data + Len - 512, 512);
		
		//计算信号的最大值
		fMean += GetMeanFloat(log_data + 300, 40);

		printf("---fMean = %f --- fMax = %f--------\n",fMean,fMax);
		//fDelta += (fMean - fMax);

	}
	fMean = fMean / 10;

	printf("------fMean = %f--------\n",fMean);
	//------------采集数据----------
	
	if ((wave == 0)&&(fMean - DYNARANGE_1310 >= DYNARANGE_DELTA))
	{
		free(data);
		free(LogSigTable);
		return 1;
	}
	if ((wave == 1)&&(fMean - DYNARANGE_1550 >= DYNARANGE_DELTA))
	{
		free(data);
		free(LogSigTable);
		return 1;
	}
	
	Opm_LdPulse(0);
	
	free(data);
	free(LogSigTable);
	free(log_data);
	log_data = NULL;
	return 0;
}



/***
  * 功能：
        打开或者关闭光模块激光器
  * 参数：
        1.int OnOff:   1:打开 0:关闭
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_LdPulse(int OnOff)
{
    //错误标志、返回值定义
    int iErr = 0;
	U32 uiFpgaCtrlOut = 0;

    iErr = read_opt_reg(opticFD, FPGA_OPTICAL_POWER, &(uiFpgaCtrlOut));
    if (OnOff)
    	uiFpgaCtrlOut |= OPM_LD_EN;
	else
		uiFpgaCtrlOut &= ~OPM_LD_EN;
    iErr = write_opt_reg(opticFD, FPGA_OPTICAL_POWER, &(uiFpgaCtrlOut));

    return iErr;
}



/* 
用来计算移相周期的 
*/ 
void CalShiftPhase4_90( 
unsigned char iCycle, //移相90度，需要的数 
unsigned char *buf //移相计算的缓冲区,64个字节 
) 
{ 
	int i,iTemp,iCnt; 
	char *bufTemp; 
	float fTemp,fTemp2; 
	fTemp=25000.0/(iCycle*3); 
	fTemp2=25000.0/4; 
	fTemp=fTemp2/fTemp;//90度需要的步数 
	fTemp=fTemp/16; //90度，分成16相，每相需要的步数 
	bufTemp=(char *)buf; 
	for(i=0;i<64;i++) 
	{ 
		iCnt=i%16; 
		iCnt=iCnt-8; 
		fTemp2=fTemp*iCnt; 
		iTemp=fTemp2; 
		bufTemp[63-i]=(char)iTemp; 
	} 
}

/*
说明:
	用来设置采用移相插补，或者正常采集
*/

int Opm_SetIntp(int iIntpReg)	
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiMode = 0;

    if (iErr == 0)
    {
		iErr = read_opt_reg(opticFD, FPGA_DAQMODE_SET, &uiMode);

    	uiMode = uiMode & (~DAQ_INTP_MASK);//清除原来的设置
    	iIntpReg = iIntpReg & DAQ_INTP_MASK;//清除不必要的位
        uiMode = uiMode | iIntpReg;          //changed lichuyuan 2015.08.19
		
        //设置光模块当前的数据采集模式
        iErr = write_opt_reg(opticFD, FPGA_DAQMODE_SET, &uiMode);

    }
	
    return iErr;
}


/*
说明:
	用来设置移相插补的相位移动值
*/
int Opm_SetPhase() 
{
    //错误标志、返回值定义
    int iErr = 0, i;
    unsigned int iPhase[32];//
    unsigned char buf[64];

	CalShiftPhase4_90(0xd2, buf);//0xd2是经验值测试出来的
	
    for(i = 0; i < 32; ++i)
    {
        iPhase[i] = (buf[i*2+1]<<8) + buf[i*2];
    }

    //设置移相插补相位
    iErr = 0;    
    for(i = 1; i < 32; ++i)
    {
        iErr += write_opt_reg(opticFD, FPGA_DELAY_SET_P10 + (i<<1), &iPhase[i]);
    }
    //FPGA_DELAY_SET_P10寄存器最后设置，产生有效信号
    iErr += write_opt_reg(opticFD, FPGA_DELAY_SET_P10, &iPhase[0]);    

    return iErr;

}



/***
  	功能：设置光模块当前的数据采集模式
***/
int Opm_SetMode(OPM_DAQ_MODE enuDaqMode)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiMode;

    if (iErr == 0)
    {
        //更新光模块寄存器FPGA_DAQMODE_SET值
        iErr = read_opt_reg(opticFD, FPGA_DAQMODE_SET, &uiMode);
		
		uiMode = (uiMode & ~(DAQ_MODE_MASK << DAQ_MODE_SHIFT)) | (enuDaqMode);
        //设置光模块当前的数据采集模式
        iErr = write_opt_reg(opticFD, FPGA_DAQMODE_SET, &uiMode);
    }

    return iErr;
}



/***
  * 功能：设置光模块当前的滤波参数
***/
int Opm_SetFilter(OPM_FILTER enuFilterArg)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //更新光模块寄存器FPGA_FILTER_SEL值
        UINT32 uiFilter = 0;
        uiFilter = 0x1F & enuFilterArg;
        //设置光模块当前的滤波参数
        iErr = write_opt_reg(opticFD, FPGA_FILTER_SEL, &uiFilter);
    }

    return iErr;
}



/***
  * 功能：设置光模块当前的数据操作类型
***/
int Opm_SetDaqCmd(OPM_DAQ_CMD enuOperType)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiOper;
   
    if (iErr == 0)
    {
		iErr = read_opt_reg(opticFD, FPGA_DAQ_CTRL, &uiOper);
		
        uiOper = (uiOper & ~DAQ_CMD_MASK) | (enuOperType << DAQ_CMD_SHIFT);
        //设置光模块当前的数据操作类型
        iErr = write_opt_reg(opticFD, FPGA_DAQ_CTRL, &uiOper);
    }

    return iErr;
}


/*
说明：
	该函数主要用来设置ADC采样参数的 ,会设置以下参数
返回:
	0=正常，-1=失败
*/
int SedAdcParameter(UINT32 uiFiberLen)
{
	UINT32 uiAdcCount, uiAdcGap;

    //错误标志、返回值定义
	int iErr = 0;
    //临时变量定义
	unsigned int uiSampleCount;

    if (iErr == 0)
    {
		uiAdcCount = uiFiberLen;

        //采样数据长度不能小于256个
 		if(uiAdcCount < 256)
	    	iErr = -1;
		
		uiAdcGap = 0x200;
    }

    if (iErr == 0)
    {
        //设置采样计数
        uiSampleCount = uiAdcCount & 0xFFFF;
        iErr = write_opt_reg(opticFD, FPGA_ADC_CNT_L, &uiSampleCount);
        uiSampleCount = (uiAdcCount & 0xFFFF0000) >> 16;
        iErr = write_opt_reg(opticFD, FPGA_ADC_CNT_H, &uiSampleCount);
	
        //更新光模块寄存器FPGA_DAQ_GAP值 采集间隔
        iErr = write_opt_reg(opticFD, FPGA_DAQ_GAP, &uiAdcGap);
    }

    return iErr;

}


int Opm_SetPulse(int iPulse)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiPulse = 0;
	unsigned int uiCtrl = 0;


    if (iErr == 0)
    {
		iErr = read_opt_reg(opticFD, FPGA_CTRLOUT_REG, &uiCtrl);
        if (0 == iPulse || 5 == iPulse)
        {
            //5ns脉冲如何设置
            uiCtrl |= SHORT_PULSE;	
			uiPulse = 0;
            
            unsigned int temp = 0x10;
            write_opt_reg(opticFD, FPGA_OPTICAL_PWM_SET, &temp);
        }
		else
		{
			uiCtrl &= ~SHORT_PULSE;
		}

		if(iPulse == 10)
		{
			uiPulse = 0;
		}
		else if(iPulse == 20)
		{
			uiPulse = 1;
		}
		else
		{
			uiPulse = iPulse / 25;
		}
		
        iErr = write_opt_reg(opticFD, FPGA_LD_PULSE_WIDTH, &uiPulse);
		iErr = write_opt_reg(opticFD, FPGA_CTRLOUT_REG, &uiCtrl);
    }

    return iErr;
}


int Opm_SetWave(int iWave)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiWave = 0;

    if (iErr == 0)
    {

		iErr = read_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiWave);

		if(iWave == 0)
		{
			uiWave = (uiWave & ~WAVE_CTRL_MASK) | (WAVE_1310);
		}
		else if(iWave == 1)
		{
			uiWave = (uiWave & ~WAVE_CTRL_MASK) | (WAVE_1550);
		}
		else if(iWave == 2)
		{
			//待拓展1625波长
		}

        //设置当前光模块的波长
        iErr = write_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiWave);
    }
    
    return iErr;
}

//得到采样数据
int  GetAdc_Data(UINT16	*pBuf,int iCount)
{
	OPM_DAQ_CMD    iDaqCmd;
	int i,iRet = 0;

	UINT16 *pDataTemp = NULL;
	pDataTemp = (UINT16 *)calloc(iCount, sizeof(UINT16)); //!< 4096 + 512
	if(pDataTemp == NULL)
	{
		printf(" calloc fail!!!");
		return -1;
	}
	
	//发出DAQ停止的命令
	iDaqCmd = ENUM_DAQ_CMD_STOP;
	Opm_SetDaqCmd(iDaqCmd);
	
	i=0;
	//读取DAQ的状态，如果正忙，则要先停止数据采集
	while(Opm_GetDaqStatus())
	{
		i++;
		MsecSleep(20);
		if(i>600)
		{
			//ADC 停止超时
			printf("ADC timeout --\n");
			return -1;
		}		  	  
	}
	//发出开始采集命令
	iDaqCmd = ENUM_DAQ_CMD_START;
	Opm_SetDaqCmd(iDaqCmd);
	
	i=0;
	//读取ADC_DATA_READY的状态，如果为0，则要数据采集还未完成
	do
	{
		i++;
		MsecSleep(10);
		
		//ADC采样超时
		if(i>1000)
		{
			//ADC 停止超时
			printf("adc outtime\n");
			iRet=1;
			break;
		}	
	}while(!Opm_AdcReady());


	//发出DAQ停止的命令
	iDaqCmd = ENUM_DAQ_CMD_STOP;
	Opm_SetDaqCmd(iDaqCmd);
	
	//发出读取DDR数据命令
	iDaqCmd = ENUM_DAQ_CMD_READ;
	Opm_SetDaqCmd(iDaqCmd);

	i=0;
	//读取CPU_RD_DDR_READY的状态，如果为0，则要DDR中数据未准备好
	while(!Opm_DataReady())
	{
		i++;
		MsecSleep(50);
		
		//DDR Ready超时
		if(i>100)
		{
			//ADC 停止超时
			iRet=1;
			break;
		}		  	  
	}

	Opm_ReadAdcData(iCount, pDataTemp); //lichuyuan 2013.12.19

	pDataTemp[0] = pDataTemp[1];

	if(iCount >= 512)
	{
	/*****************************************************************
								数据格式(前)
	------------------------------------------------------------------
	| 512噪声 |    offset    |                  data                 |
	------------------------------------------------------------------
	*****************************************************************/

		memcpy(pBuf, pDataTemp + 512, (iCount - 512) * sizeof(U16));
		memcpy(pBuf + (iCount - 512), pDataTemp, 512 * sizeof(U16));
	
	/*****************************************************************
								数据格式(后)
	------------------------------------------------------------------
	|    offset    |                  data                 | 512噪声 |
	------------------------------------------------------------------
	*****************************************************************/
	}
	else
	{
		memcpy(pBuf,pDataTemp,iCount * sizeof(U16));
	}
	
#if 0
	for(i = 0; i < iCount; i++)
	{
		printf("data[%d] = %d\n",i,pBuf[i]);
	}
#endif

	if(pDataTemp != NULL)
	{
		free(pDataTemp);
		pDataTemp = NULL;
	}

	return iRet;
}


/***
  * 功能：
        判断光模块的DAQ是否正忙
***/
int Opm_GetDaqStatus()
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;

    //尝试读取FPGA_DAQ_CTRL
    read_opt_reg(opticFD, FPGA_DAQ_CTRL, &uiDaq);

    //判断光模块的DAQ是否正忙
    iRet = uiDaq & DAQ_STATUS;
	
    return iRet;
}



/***
  * 功能：判断光模块的ADC是否已准备好
***/
int Opm_AdcReady()
{
    //错误标志、返回值定义
    int iErr = 0;
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;

  
    //尝试读取FPGA_DAQ_CTRL
    iErr = read_opt_reg(opticFD, FPGA_DAQ_CTRL, &uiDaq);
    if (iErr)
    {
        iErr = -2;
    }
   
    if (iErr == 0)
    {
        //判断光模块的ADC是否已准备好
        iRet = uiDaq & ADC_DATA_READY;
    }
    return iRet;
}


/***
  * 功能：
        判断光模块的数据是否已准备好
***/
int Opm_DataReady()
{
    //错误标志、返回值定义
    int iErr = 0;
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;


    //尝试读取FPGA_DAQ_CTRL
    iErr = read_opt_reg(opticFD, FPGA_DAQ_CTRL, &uiDaq);
    if (iErr)
    {
        iErr = -2;
    }
    if (iErr == 0)
    {
        //判断光模块的数据是否已准备好
        iRet = uiDaq & CPU_RD_DDR_READY;
    }

    return iRet;
}


/*
说明:
	该函数只读取ADC的采样数据，不进行状态查询的,
	数据长度在OPMSET的中指定，如果是移相插补，则
	需要8倍的长度
备注:
  	测试通过
*/
int Opm_ReadAdcData(int adcCount, UINT16 *pDataBuf)
{
	int iRet = 0;
	
	OPTARG_DAT  arg;

	arg.iDataCnt = adcCount;

	arg.pDataBuf = pDataBuf;
	
	iRet = ioctl(opticFD, OPT_IOCTL_RDDAT, &arg);

	return iRet;
}




/*
说明:	
	用来设置Amp放大通道的
备注:
	测试通过
*/
void Opm_SetAmp(U32 Amp)
{
   int iErr = 0;
   UINT32 uiOpmCtrlReg;
   iErr = read_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiOpmCtrlReg);

   uiOpmCtrlReg = uiOpmCtrlReg & (~AMP_MASK);
   uiOpmCtrlReg |= Amp;

   iErr = write_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiOpmCtrlReg);
 
}




//硬件工作初始化;
int BoardWorkInit()
{
	int iRet;
	UINT32  uiTemp = 0;
	int i = 0;
		
	Opm_EnablePwr(1);		//打开光模块电源
	//使能光模块电源后需等待稳定
	MsecSleep(200);
	//打开正负5v控制电源
	read_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiTemp);
	uiTemp |= C_5V_OPM_CTL;
	write_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiTemp);
	//打开负5v
	read_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiTemp);
	uiTemp |= _C_5V_OPM_CTL;
	write_opt_reg(opticFD, OPM_CTRLOUT_REG, &uiTemp);
	//打开FPGA的时钟
	uiTemp = OMCLK_CTRL;
    write_opt_reg(opticFD, FPGA_CLOCK_CTRL, &uiTemp); 
	MsecSleep(20);
	//对FPGA接的DDR2进行复位
	uiTemp = RESET_DDR2;
    write_opt_reg(opticFD, FPGA_DDR_RESET, &uiTemp); 			//对FPGA接的DDR2进行复位
	MsecSleep(20);
	
	iRet=1;
	while(i++<100)
	{
		uiTemp = 0;
		read_opt_reg(opticFD, FPGA_DAQMODE_SET, &uiTemp);
		if(!(uiTemp & C1_RST0) && (uiTemp & C1_CALIB_DONE))  //检查DDR2是否校准成功
		{ 
			iRet = 0;
			break;
		}
		MsecSleep(10);
	}

	// if(iRet)
	// {
	// 	printf("--DDR2 init failed--\n");
	// 	return -1;
	// }

	//打开ADC 1.8v控制电源
	read_opt_reg(opticFD, FPGA_TRIG_CTRL, &uiTemp);
	uiTemp |= ADC_CONFIG_START;
	write_opt_reg(opticFD, FPGA_TRIG_CTRL, &uiTemp);
	//打开ADC时钟，以及数据输出等,adc上共三个量
	uiTemp = 0;
	read_opt_reg(opticFD, FPGA_CTRLOUT_REG, &uiTemp);
	uiTemp |= AD_CLK_EN;
    write_opt_reg(opticFD, FPGA_CTRLOUT_REG, &(uiTemp));

	MsecSleep(500);
	uiTemp = 0;
	read_opt_reg(opticFD, FPGA_DAQMODE_SET, &uiTemp);
	if(!(uiTemp & ADC_STATUE))
		printf("--ADC Clock init failed--\n");

	//设置FpgaCtrlOut寄存器
	read_opt_reg(opticFD, FPGA_CTRLOUT_REG, &uiTemp);
	uiTemp |= (OPM_24LC_SDA | OPM_24LC_CLK | OPM_24LC_WP | OPM_LD_EN | OPM_ADC3V_POWCON | OPM_ADC_PWR_EN);
	uiTemp |= (OPM_LM75_OS | OPM_LM75_SCL | OPM_LM75_SDA);
	write_opt_reg(opticFD, FPGA_CTRLOUT_REG, &(uiTemp));

	iRet = Opm_SetIntp(DAQ_INTP01_01);
	Opm_SetPhase();

	return 0; //0=正常工作
}




/***
  * 功能：使能光模块电源
***/
int Opm_EnablePwr(int OnOff)
{
    //错误标志、返回值定义
    int iErr = 0;
	int iRet = 0;
    UINT32  uiRegVal = 0;
	
    if(iErr == 0)
    {
        //使能光模块电源
        iRet = read_opt_reg(opticFD, FPGA_OPTICAL_POWER, &uiRegVal);
        if(iRet)
			iErr = -1;
	
		if(OnOff)
			uiRegVal |= OPM_PWR_EN;//打开模拟板电源
		else
			uiRegVal &= (~OPM_PWR_EN);//关闭模拟板电源

        iRet = write_opt_reg(opticFD, FPGA_OPTICAL_POWER, &(uiRegVal));
    }
		
    return iErr;
}


