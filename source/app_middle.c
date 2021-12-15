/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_middle.c  
* 摘    要：  实现应用与设备驱动之间的中间层接口 
*
* 当前版本：  v1.0.0 
* 作    者：  
* 完成日期：  
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/

#include "app_middle.h"
#include "app_global.h"
#include "wnd_global.h"
#include "app_math.h"
#include <sys/time.h>
#include "app_otdr_algorithm_parameters.h"
#include "app_parameter.h"

//#define OPT_REG_DEBUG

#define READ_OPT_REG	0
#define WRITE_OPT_REG	1
#define READ_BACK_REG	2
#define TEST			1
extern PFactoryConfig pFactoryConf;
#ifdef OPT_REG_DEBUG
//added by zyx for test
void dbg_pri_reg(int iType, int iRegId, int iRegValue)
{
	char strRegName[][25] = {
	"FPGA_INTP_REG         ",
	"FPGA_CTRLOUT_REG      ",
	"FPGA_KEY_CODE_LOW     ",
	"FPGA_KEY_CODE_HIGH    ",
	"FPGA_VERSION_ADDR     ",
	"FPGA_OPM_CTRL0        ",
    "FPGA_OPM_CTRL1        ",
    "FPGA_OPM_ATT0         ",
    "FPGA_OPM_ATT1         ",
    "FPGA_OPM_VDD_VOLTAGE  ",
    "FPGA_OPM_APD_VOLTAGE  ",
    "FPGA_OPM_APD_BIAS     ",
    "FPGA_LD_PULSE_WIDTH   ",
    "FPGA_ADC_CNT_L        ",
    "FPGA_ADC_CNT_H        ",
    "FPGA_ADC_DELAY_CNT    ",
    "FPGA_DAQ_GAP          ",
    "FPGA_CPU_RD_DDR2_ADDRL",
    "FPGA_CPU_RD_DDR2_ADDRH",
    "FPGA_CPU_RD_DDR2_DATA ",
    "FPGA_CPU_WR_DDR2_ADDRL",
    "FPGA_CPU_WR_DDR2_ADDRH",
    "FPGA_CPU_WR_DDR2_DATA ",
    "FPGA_DAQ_CTRL         ",
    "FPGA_FILTER_SEL       ",
    "FPGA_DAQMODE_SET      ",
    "FPGA_DELAY_SET_P10    ",
    "FPGA_DELAY_SET_P32    ",
    "FPGA_DELAY_SET_P54    ",
    "FPGA_DELAY_SET_P76    ",
    "FPGA_CLOCK_CTRL       ",
    "FPGA_DDR_RESET        "
	};
	char IoType[3][10] = {"ReadReg ", "WriteReg", "ReadBack"};

	if(iType == READ_OPT_REG)
	{
        printf("%s: id=%s, val=0x%04x\n", IoType[iType], strRegName[iRegId], iRegValue);
	}
	else
	if(iType == WRITE_OPT_REG)
	{
        printf("%s: id=%s, val=0x%04x;", IoType[iType], strRegName[iRegId], iRegValue);
	}
	else
	{
        printf("%s: val=0x%04x\n", IoType[iType], iRegValue);
	}

	if(iType != WRITE_OPT_REG)
	{
		//putchar('\n');
	}
}
#else
void dbg_pri_reg(int iType, int iRegId, int iRegValue) {}
#endif

//end

/***
  * 功能：
        FPGA程序初始化
  * 参数：
        1.int iFpgaFd:          FPGA设备文件名
        2.char *strProgFile:    FPGA程序文件名
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Fpga_ProgInit(int iFpgaFd, char *strProgFile)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    struct stat Status;
    int iFd;

    if (iErr == 0)
    {
        //参数检查
        if ((iFpgaFd < 0) || (NULL == strProgFile))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //尝试获取FPGA程序文件大小
        iErr = stat(strProgFile, &Status);
        if (iErr == -1)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //通过ioctl设置要下载的程序文件大小
        iErr = ioctl(iFpgaFd, PUT_FPGA_DATA_SIZE, &Status.st_size);
        if (iErr == -1)
        {
            iErr = -3;
        }
    }

    if (iErr == 0)
    {
        //尝试打开FPGA程序文件
        iFd = open(strProgFile, O_RDONLY);
        if (iFd == -1)
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //下载程序文件
        while (1)
        {
			unsigned char Buff[1024];
			int iLength;
            iLength = read(iFd, Buff, sizeof(Buff));    //读取一段程序
            if (iLength == 0)                           //所有程序都已读完
            {
                close(iFd);                             //关闭程序文件
                break;                                  //结束下载
            }
            iLength = write(iFpgaFd, Buff, iLength);    //写入一段程序
            if (iLength < 0)
            {
                iErr = -5;
            }
        }
    }

    if (iErr == 0)
    {
        //启动FPGA程序
        iErr = ioctl(iFpgaFd, LOAD_FPGA_DATA_START);
        //iErr = ioctl(iFpgaFd, INIT_FPGA_EBI_CS2);
        if (iErr == -1)
        {
            iErr = -6;
        }
    }

    return iErr;
}

/*
说明:
	保存数据到OPM板和主板EEPROM上指定的偏移
参数:
	1.iEepromFd	:	指定的EEPROM的设备描述符，区分是OPM板还是主板上的EEPROM
	2.pData		:	指向需要保存的数据的指针
	3.iOff		:	指定的EEPROM上保存数据的偏移
	4.iSize		:	指定需要保存的数据的大小
返回:
	0=成功，其他值失败
*/
int SaveDataToEeprom(int iEepromFd, void * pData, unsigned int iOff, unsigned int iSize)
{
	//返回值
	int iRet = 0;

	//参数检查
	if ((iEepromFd < 0) || (pData == NULL))
	{
		return -1;
	}
	
	//定位到指定参数的偏移
	lseek(iEepromFd, iOff, SEEK_SET);

	//写入参数, 正确返回写入的数据的大小，错误返回负数
	iRet = write(iEepromFd, pData, iSize);

	if (iRet < 0)
	{
        LOG(LOG_ERROR, " write  iRet = %d\n", iRet);
        iRet = -1;
	}
	else
	{
		iRet = 0;
	}

	return iRet;
}

/*
说明:
	读取OPM板和主板EEPROM上指定的偏移的数据
参数:
	1.iEepromFd	:	指定的EEPROM的设备描述符，区分是OPM板还是主板上的EEPROM
	2.pData		:	指向需要读取的数据的指针
	3.iOff		:	指定的EEPROM上读取数据的偏移
	4.iSize		:	指定需要读取的数据的大小
返回:
	0=成功，其他值失败
*/
int ReadDataFromEeprom(int iEepromFd, void * pData, unsigned int iOff, unsigned int iSize)
{
	//返回值
	int iRet = 0;

	//参数检查
	if ((iEepromFd < 0) || (pData == NULL))
	{
		return -1;
	}
	
	//定位到指定参数的偏移
	lseek(iEepromFd, iOff, SEEK_SET);

	//读取参数, 正确返回读取的数据的大小，错误返回负数
	iRet = read(iEepromFd, (UINT8 *)pData, iSize);

	if (iRet < 0)
	{
        LOG(LOG_ERROR, " read  iRet = %d\n", iRet);
        iRet = -1;
	}
	else
	{
		iRet = 0;
	}

	return iRet;
}

/*
说明:
	得到光模块上的整定数据
返回:
	0=成功，其他值失败
*/
int Opm_GetTuning(
POPM_TUNING  pOpmTunning  //光模块整定数据的指针
)
{
	//向EEPROM读取OPM数据,然后再返回
	int iRet = 0;
	
	//获取光模块设备描述符
	DEVFD *pDevFd = NULL;
	int iOpmFd;
	
	if (NULL == pOpmTunning)
	{
        DBG_EXIT(-1);
		return -1;
	}
	
	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;
	
	iRet = ReadDataFromEeprom(iOpmFd, pOpmTunning, OPM_EEPROM_OFF_TUNNING, sizeof(* pOpmTunning));

	if (!iRet)
	{
		int i = 0;
        LOG(LOG_INFO, "************OpmTunning************\n");

        for(i=0;i<OPM_APD_KIND;i++)
		{
            LOG(LOG_INFO, "pOpmTunning->iApdVoltage[%d] = 0x%04x\n", i, (UINT16)pOpmTunning->iApdVoltageSet[i]);
        }

		for(i=0;i<OPM_VDD_KIND;i++)
		{
            LOG(LOG_INFO, "pOpmTunning->iVddValue[%d] = 0x%04x\n", i, (UINT16)pOpmTunning->iVddValue[i]);
        }

		for(i=0;i<OPM_AMP_KIND;i++)
		{
            LOG(LOG_INFO, "att[%d] = 0x%04x  \n", i, pOpmTunning->arrAttValue[i]);
        }

		for(i=0;i<OPM_APD_KIND;i++)
		{
            LOG(LOG_INFO, "pOpmTunning->fTempCoefficient[%d] = %f\n", i, pOpmTunning->fTempCoefficient[i]);
        }

        LOG(LOG_INFO, "pOpmTunning->fTemperatureSet = %f\n", pOpmTunning->fTemperatureSet);
        LOG(LOG_INFO, "pOpmTunning->OffsetPoint = %u\n", pOpmTunning->OffsetPoint);
        LOG(LOG_INFO, "**********************************\n");

        if (pOpmTunning->OffsetPoint < 900 || pOpmTunning->OffsetPoint > 1500)
		{
			pOpmTunning->OffsetPoint = 0;
		}
	}
    else
        LOG(LOG_ERROR, " read OpmTunning iRet = %d\n", iRet);

    iRet = 0;		//如果没接光模块，确保程序仍可运行
	DBG_EXIT(iRet);
	return iRet;	//0=成功，1=失败
}

/***
  * 功能：
        读取光模块寄存器
  * 参数：
        1.int iOpmFd:               光模块设备文件描述符
        2.unsigned int uiRegAddr:   要读取的光模块寄存器地址
        3.unsigned int *pReadVal:   传出参数，保存读取的寄存器值
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_ReadReg(int iOpmFd, unsigned int uiRegAddr, unsigned int *pReadVal)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    OPTARG_REG Arg;

    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断寄存器地址是否合法
        if ((uiRegAddr < FPGA_BASE_ADDR) || 
            (uiRegAddr > (FPGA_BASE_ADDR + FPGA_ADDR_SIZE)))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //调用ioctl()读取寄存器
        Arg.usRegIndex = (unsigned short) ((uiRegAddr - FPGA_BASE_ADDR) >> 1);
        Arg.usRegValue = (unsigned short) ((*pReadVal) & 0xFFFF);
		MsecSleep(1);
        iErr = ioctl(iOpmFd, OPT_IOCTL_RDREG, &Arg);
        if (iErr < 0)
        {
            iErr = -3;
        }
        //dbg_pri_reg(READ_OPT_REG, Arg.usRegIndex,Arg.usRegValue); //
    }

    if (iErr == 0)
    {
        //设置传出参数
        *pReadVal = Arg.usRegValue;   //将读取的寄存器值传出
    }

    return iErr;
}


/***
  * 功能：
        写入光模块寄存器
  * 参数：
        1.int iOpmFd:               光模块设备文件描述符
        2.unsigned int uiRegAddr:   要写入的光模块寄存器地址
        3.unsigned int *pWriteVal:  传入、传出参数，传入时为要写入的值，传出时为
                                    写入前的旧寄存器值
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_WriteReg(int iOpmFd, unsigned int uiRegAddr, unsigned int *pWriteVal)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    OPTARG_REG Arg;

    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断寄存器地址是否合法
        if ((uiRegAddr < FPGA_BASE_ADDR) || 
            (uiRegAddr > (FPGA_BASE_ADDR + FPGA_ADDR_SIZE)))
        {
            iErr = -2;
        }
    }
    if (iErr == 0)
    {
        //调用ioctl()写入寄存器
        Arg.usRegIndex = (unsigned short) ((uiRegAddr - FPGA_BASE_ADDR) >> 1);
        Arg.usRegValue = (unsigned short) ((*pWriteVal) & 0xFFFF);
        //dbg_pri_reg(WRITE_OPT_REG, Arg.usRegIndex, Arg.usRegValue); //
        MsecSleep(1);
        iErr = ioctl(iOpmFd, OPT_IOCTL_WRREG, &Arg);
        if (iErr < 0)
        {
            iErr = -3;
        }

		//read back
        OPTARG_REG ArgTmp;
        ArgTmp.usRegIndex = (unsigned short) ((uiRegAddr - FPGA_BASE_ADDR) >> 1);
		iErr = ioctl(iOpmFd, OPT_IOCTL_RDREG, &ArgTmp);
        //dbg_pri_reg(READ_BACK_REG, ArgTmp.usRegIndex, ArgTmp.usRegValue); //
		if (iErr < 0)
		{
			iErr = -3;
		}
		//end

    }

    if (iErr == 0)
    {
        //设置传出参数
        *pWriteVal = Arg.usRegValue;   //将旧的寄存器值传出
    }

    return iErr;
}
/***
  * 功能：
        得到光模块参数
  * 参数：
        1: 无
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
  		测试通过
***/
OPMSET* Opm_InitSet(void)
{
    //错误标志、返回值定义;
    int iErr = 0;
    OPMSET *pOpm = NULL;
    POPM_TUNING  pOpmTuning = NULL;  
    //临时变量定义;
    OPM_REG_INDEX *pIndex = NULL;
    unsigned char *pCurr = NULL;

	//读取光模块eeprom得到整定的OPM数据
    if (iErr == 0)
    {
        //尝试分配内存
        pOpm = (OPMSET *)GuiMemAlloc(sizeof(OPMSET));
        if (NULL == pOpm)
        {
            iErr = -2;
        }
        else
        {
            if(pOpmTuning == NULL)//为pOpmTuning申请空间，并得到整定值
            {
            	
    	        pOpmTuning = (POPM_TUNING)GuiMemAlloc(sizeof(OPM_TUNING));
                memset(pOpmTuning, 0, sizeof(OPM_TUNING));
                //读取光模块整定值
    	        //该处暂时填充仿真数据
#ifdef EEPROM_DATA
                Opm_GetTuning(pOpmTuning);
#endif
				//设置时候就把下面的代码去掉，并且加上击穿电压检测!!!
				//#define OTDR
				#ifdef OTDR
    	        {
					pOpmTuning->iVddValue[0] = 0x06c0;//2.804
					pOpmTuning->iVddValue[1] = 0x06c0;//2.629
					pOpmTuning->iVddValue[2] = 0x06c0;//2.55
					pOpmTuning->iVddValue[3] = 0x06c0;//2.295

                    int i = 0;
		    		for(i = 0; i < OPM_AMP_KIND; i++)
		    		{
					   pOpmTuning->arrAttValue[i] = 0x0db0;
		    		}

					/*
					pOpmTuning->iApdVoltageSet[0] = 0x9a0;
					pOpmTuning->iApdVoltageSet[1] = 0x970;
					pOpmTuning->iApdVoltageSet[2] = 0x8e0;
					pOpmTuning->iApdVoltageSet[3] = 0x700;
					*/
					pOpmTuning->iApdVoltageSet[0] = 0x8d8;
					pOpmTuning->iApdVoltageSet[1] = 0x888;
					pOpmTuning->iApdVoltageSet[2] = 0x7b0;
					pOpmTuning->iApdVoltageSet[3] = 0x4b0;

					// 温度整定结果2296,0x8F8
					pOpmTuning->fTempCoefficient[0] = 4.188f;
                    pOpmTuning->fTempCoefficient[1] = 4.188f;
                    pOpmTuning->fTempCoefficient[2] = 4.188f;
                    pOpmTuning->fTempCoefficient[3] = 4.188f;
                    pOpmTuning->fTemperatureSet = 0.0f;
					
    	        }
				#endif

				#ifdef OTDR
				{
					for(i = 0; i < OPM_APD_KIND; i++)
                        LOG(LOG_INFO, "pOpmTuning->iApdVoltageSet[%d]=0x%x\n", i, pOpmTuning->iApdVoltageSet[i]);
                    for(i = 0; i < OPM_VDD_KIND; i++)
                        LOG(LOG_INFO, "pOpmTuning->iApdVoltageSet[%d]=0x%x\n", i, pOpmTuning->iVddValue[i]);
                    for(i = 0; i < OPM_AMP_KIND; i++)
                        LOG(LOG_INFO, "pOpmTuning->arrAttValue[%d]=0x%x\n", i, pOpmTuning->arrAttValue[i]);
                }
				#endif

				pOpmTuning->OffsetPoint = 1007;
				
				pOpm->pOpmTuning = pOpmTuning;
				
				InitMutex(&pOpm->mOPMSetMutex, NULL);
    	    }
        }
    }
    
    //保存各脉宽放大档数
    PAMPCOM AmpComTotal[OPM_PULSE_KIND];
    PAMPCOM pAmpComTmp;

	//放大档位
	int Amp[10]={	ENUM_AMP0,				//0db      //1.5k & 2.0 RF4
				    ENUM_AMP1,				//2.5db    //1.5k & 5.7 RF4
				    ENUM_AMP2,				//3.5db    //15k  & 2.0 RF2
					ENUM_AMP3,				//6db	   //15k  & 5.7 RF2
				    ENUM_AMP4,				//6.7db    //100k & 2.0 RF3
				    ENUM_AMP5,				//8.4db    //100k & 5.7 RF3
				    ENUM_AMP6,				//9.2db    //300k & 2.0 RF1
				    ENUM_AMP7,				//10.9db   //300k & 5.7 RF1
				    ENUM_AMP8,				//14.1db   //1.8M & 2.0
				    ENUM_AMP9};				//16.6db   //1.8M & 5.7

    //与脉宽对应的放大档数组数
    int Atten[OPM_PULSE_KIND];

    AmpComTotal[0] = AmpCom10ns;
    AmpComTotal[1] = AmpCom5ns;
    AmpComTotal[2] = AmpCom10ns;
    AmpComTotal[3] = AmpCom20ns;
    #ifdef MINI2
	AmpComTotal[4] = AmpCom30ns;
    AmpComTotal[5] = AmpCom50ns;
    AmpComTotal[6] = AmpCom100ns;
    AmpComTotal[7] = AmpCom200ns;
    AmpComTotal[8] = AmpCom500ns;
    AmpComTotal[9] = AmpCom1us;
    AmpComTotal[10] = AmpCom2us;
    AmpComTotal[11] = AmpCom10us;
    AmpComTotal[12] = AmpCom20us;
    #else
    AmpComTotal[4] = AmpCom50ns;
    AmpComTotal[5] = AmpCom100ns;
    AmpComTotal[6] = AmpCom200ns;
    AmpComTotal[7] = AmpCom500ns;
    AmpComTotal[8] = AmpCom1us;
    AmpComTotal[9] = AmpCom2us;
    AmpComTotal[10] = AmpCom10us;
    AmpComTotal[11] = AmpCom20us;
    #endif

    Atten[0] = sizeof(AmpCom10ns) / sizeof(AMPCOM);
    Atten[1] = sizeof(AmpCom5ns) / sizeof(AMPCOM);
    Atten[2] = sizeof(AmpCom10ns) / sizeof(AMPCOM);
    Atten[3] = sizeof(AmpCom20ns) / sizeof(AMPCOM);
    #ifdef MINI2
	Atten[4] = sizeof(AmpCom30ns) / sizeof(AMPCOM);
    Atten[5] = sizeof(AmpCom50ns) / sizeof(AMPCOM);
    Atten[6] = sizeof(AmpCom100ns) / sizeof(AMPCOM);
    Atten[7] = sizeof(AmpCom200ns) / sizeof(AMPCOM);
    Atten[8] = sizeof(AmpCom500ns) / sizeof(AMPCOM);
    Atten[9] = sizeof(AmpCom1us) / sizeof(AMPCOM);
    Atten[10] = sizeof(AmpCom2us) / sizeof(AMPCOM);
    Atten[11] = sizeof(AmpCom10us) / sizeof(AMPCOM);
    Atten[12] = sizeof(AmpCom20us) / sizeof(AMPCOM);
    #else
    Atten[4] = sizeof(AmpCom50ns) / sizeof(AMPCOM);
    Atten[5] = sizeof(AmpCom100ns) / sizeof(AMPCOM);
    Atten[6] = sizeof(AmpCom200ns) / sizeof(AMPCOM);
    Atten[7] = sizeof(AmpCom500ns) / sizeof(AMPCOM);
    Atten[8] = sizeof(AmpCom1us) / sizeof(AMPCOM);
    Atten[9] = sizeof(AmpCom2us) / sizeof(AMPCOM);
    Atten[10] = sizeof(AmpCom10us) / sizeof(AMPCOM);
    Atten[11] = sizeof(AmpCom20us) / sizeof(AMPCOM);
    #endif

    if (iErr == 0)
    {
		int iLoop, iSize;
        pCurr = (unsigned char *) pOpm->DataBuff;

        //初始化所有的OPM_REG_INDEX
        iSize = sizeof(OPM_REG_INDEX);
        for (iLoop = 0; iLoop < OPM_PULSE_KIND; iLoop++)
        {
        	int iTemp;
            //为OPM_REG_INDEX结构指定存储空间
            pIndex = (OPM_REG_INDEX *) pCurr;
            pOpm->RegIndex[iLoop] = pIndex;
            pCurr += iSize;         //预留OPM_REG_INDEX结构的存储空间
            //设置OPM_REG_INDEX中的放大档数
            pIndex->uiAttenuateNum = Atten[iLoop];
            //为当前脉宽下的各放大档位对应的通道设置索引指定存储空间
            pIndex->pAmpIndex = (unsigned int *) pCurr;
            pCurr += Atten[iLoop] * sizeof(int);  //预留前置放大索引的存储空间
 
            pIndex->pApdIndex = (unsigned int *) pCurr;
            pCurr += Atten[iLoop] * sizeof(int);  //预留Apd索引的存储空间
            pIndex->pVddIndex = (unsigned int *) pCurr;
            pCurr += Atten[iLoop] * sizeof(int);  //预留Vdd索引的存储空间
            //逐个指定各放大档位的通道设置索引
            pAmpComTmp = AmpComTotal[iLoop]; 
            for (iTemp = 0; iTemp < Atten[iLoop]; iTemp++)
            {
                pIndex->pAmpIndex[iTemp] = pAmpComTmp[iTemp].iAmp;
                pIndex->pApdIndex[iTemp] = pAmpComTmp[iTemp].iApd;
                pIndex->pVddIndex[iTemp] = pAmpComTmp[iTemp].iVdd;
            }
        }

      
        //初始化所有的AMPCode
        for (iLoop = 0; iLoop < OPM_AMP_KIND; iLoop++)
        {
            pOpm->AmpCode[iLoop] = Amp[iLoop];
        }
        //初始化当前的OpmRegCtrl值
        pOpm->pCurrReg = (OPM_REG_CTRL *) pCurr;
		memset(pOpm->pCurrReg, 0 , sizeof(OPM_REG_CTRL));
#ifdef OPTIC_DEVICE
        //对pOpm->pCurrReg里面的寄存器初始化，读取FPGA的值
        int iRet;
        iRet = GetFpgaRegVal(pOpm);
		if(iRet)
		{
			LOG(LOG_ERROR,"-- Opm_init reg error !!! iRet = %d --\n",iRet);
		}

	    //设置蜂鸣器的时间
		Opm_SetSpeakerTime(GetGlb_DevFd()->iOpticDev, pOpm, 64);
		//设置蜂鸣器频率
		Opm_SetSpeakerFreq(GetGlb_DevFd()->iOpticDev, pOpm, pFactoryConf->speakerFreq);
		//设置蜂鸣器占空比（若打开，蜂鸣器使能失效，暂时取消等待fpga完成）
		//Opm_SetSpeakerPWM(GetGlb_DevFd()->iOpticDev, pOpm, 50);
#endif

    }
    //错误处理
    switch (iErr)
    {
    case -3:
        GuiMemFree(pOpm); 
    case -2:
    case -1:
        pOpm = NULL;
    default:
        break;
    }
    return pOpm;
}
//最小脉宽和最大量程对应表 change by zh 2016.1.8
static const float pulse_acquistion[OPM_PULSE_KIND-1]=
{
	1.3f,					//1300m光纤的长度 5ns
	15.0f,					//2500m光纤的长度 10ns
	15.0f,					//5km光纤的长度   20ns
	#ifdef MINI2
	15.0f,					//5km光纤的长度   30ns
    #endif
	25.0f,					//10km光纤的长度  50ns
	30.0f,					//20km光纤的长度  100ns
	45.0f,					//40km光纤的长度  200ns
	50.0f,					//60km光纤的长度  500ns
	60.0f,					//80km光纤的长度  1us
	80.0f,					//100km光纤的长度  2us
	110.0f,					//110km光纤的长度  10us
	120.0f,					//100km光纤的长度  20us
};
/***
  * 功能：
        获得FPGA版本号
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
  * 返回：
        成功返回正数版本号，失败返回负值
  * 备注：
  		测试通过
***/
int Opm_GetFwVer(int iOpmFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int iFwVersion = 0;
    
    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
    	//读取版本寄存器
    	iErr = Opm_ReadReg(iOpmFd, FPGA_VERSION_ADDR, &iFwVersion);
    	if (iErr == 0)
    	{
    		iErr = (int)iFwVersion;
            LOG(LOG_INFO, "fpga version is %d\n", iErr);
        }
    	else
    	{
            LOG(LOG_ERROR, "read fpga version error:%d\n", iErr);
        }
    }
    
    return iErr;
}


/***
  * 功能：
        控制蜂鸣器
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
		2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
		3.int iEnable:		使能蜂鸣器
  * 返回：
        成功返回正数版本号，失败返回负值
  * 备注：
  		测试通过
***/
int Opm_SetSpeaker(int iOpmFd, OPMSET *pCurrOpm, int iEnable)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int iSpeaker = 0;
    
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
	}

    if (iErr == 0)
    {	
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &iSpeaker);

        if (iEnable)
            iSpeaker |= SPEAK_CTRL;
        else
            iSpeaker &= ~SPEAK_CTRL;
        MutexLock(&(pCurrOpm->mOPMSetMutex));     
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &iSpeaker);
	    MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

	return iErr;
}

/***
  * 功能：
        设置蜂鸣器的时间
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
		2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
		3.int iValue:		蜂鸣器的时间值
  * 返回：
        成功返回正数版本号，失败返回负值
  * 备注：
  		测试通过 寄存器设置单位是1ms
***/
int Opm_SetSpeakerTime(int iOpmFd, OPMSET *pCurrOpm, int iValue)
{
    //错误标志、返回值定义
    int iErr = 0;
     
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
	}

    if (iErr == 0)
    { 		
	    MutexLock(&(pCurrOpm->mOPMSetMutex));
	    
        iErr = Opm_WriteReg(iOpmFd, FPGA_SPEAK_TIME, (unsigned int *)&iValue);
        
	    MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        设置蜂鸣器的频率
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
		2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
		3.unsigned int fFreq:		蜂鸣器的频率(Hz)
  * 返回：
        成功返回正数版本号，失败返回负值
  * 备注：
  		测试通过
***/
int Opm_SetSpeakerFreq(int iOpmFd, OPMSET *pCurrOpm, unsigned int freq)
{
    //错误标志、返回值定义
    int iErr = 0;
     
    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
	}
    unsigned int iValue = 0;
    if (iErr == 0)
    { 	
	    MutexLock(&(pCurrOpm->mOPMSetMutex));
	    
    	iValue = 368;//1000000000/25/freq;该数值是硬件测试的结果，可达到2.7khz
        iErr = Opm_WriteReg(iOpmFd, FPGA_SPEAK_FREQ_SET, (unsigned int *)&iValue);
        
	    MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }    
    
    return iErr;
}

/***
  * 功能：
        设置蜂鸣器的占空比
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
		2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
		3.int iPWM:		蜂鸣器的占空比(%)
  * 返回：
        成功返回正数版本号，失败返回负值
  * 备注：
  		占空比的值为0~100的正整数
***/
int Opm_SetSpeakerPWM(int iOpmFd, OPMSET *pCurrOpm, int iPWM)
{
    //错误标志、返回值定义
    int iErr = 0;
    unsigned int iFreqValue = 0;
    unsigned int iValue = 0;
    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
	}

	MutexLock(&(pCurrOpm->mOPMSetMutex));
	
    if (iErr == 0)
    { 	
    	iErr = Opm_ReadReg(iOpmFd, FPGA_SPEAK_FREQ_SET, &iFreqValue);
    }

    if (iErr == 0)
    {
        iPWM = iPWM > 100 ? 100 : iPWM;
        iValue = iFreqValue*iPWM/100;
        iErr = Opm_WriteReg(iOpmFd, FPGA_SPEAK_PWM_SET, (unsigned int *)&iValue);
    }

	MutexUnlock(&(pCurrOpm->mOPMSetMutex));
	
    return iErr;
}


/***
  * 功能：
        使能光模块电源
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnablePwr(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }
	
    if (iErr == 0)
    {
        UINT32 regctlout;
        Opm_ReadReg(iOpmFd, FPGA_OPTICAL_POWER, &regctlout);
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        //使能光模块电源
        regctlout |= OPM_PWR_EN;
        iErr = Opm_WriteReg(iOpmFd, FPGA_OPTICAL_POWER,
                            &(regctlout));
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }
		
    return iErr;
}


/***
  * 功能：
        关闭光模块电源
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisablePwr(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }
    if (iErr == 0)
    {
        //关闭光模块电源
        UINT32 regctlout;
        Opm_ReadReg(iOpmFd, FPGA_OPTICAL_POWER, &regctlout);
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        regctlout &= ~OPM_PWR_EN;
        iErr = Opm_WriteReg(iOpmFd, FPGA_OPTICAL_POWER,
                            &(regctlout));
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}

/***
  * 功能：
        检测光模块电源是否打开
  * 参数：
        1.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_OpenPwr(OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    int iRet = 0;
    
    if (iErr == 0)
    {
        //参数检查
        if (NULL == pCurrOpm)
        {
            iErr = -1;
        }
    }
    
    if (iErr == 0)
    {
        UINT32 regctlout;
        Opm_ReadReg(GetGlb_DevFd()->iOpticDev, FPGA_OPTICAL_POWER, &regctlout);
        iRet = regctlout & OPM_PWR_EN;
    }

    return iRet;
}

/***
  * 功能：
        使能正负5v电源控制
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
        3.unsigned int value:正5v或负5v的bit位
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnablePwrCtl(int iOpmFd, OPMSET *pCurrOpm, unsigned int value)
{
    //错误标志、返回值定义
    int iErr = 0;

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块电源
        unsigned int uiOpmCtrlOut = 0;
        Opm_ReadReg(iOpmFd, OPM_CTRLOUT_REG, &(uiOpmCtrlOut));

        MutexLock(&(pCurrOpm->mOPMSetMutex));
        uiOpmCtrlOut |= value;
        iErr = Opm_WriteReg(iOpmFd, OPM_CTRLOUT_REG, &(uiOpmCtrlOut));
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}

/***
  * 功能：
        关闭正负5v电源控制
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
        3.unsigned int value:正5v或负5v的bit位
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisablePwrCtl(int iOpmFd, OPMSET *pCurrOpm, unsigned int value)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        unsigned int uiOpmCtlout = 0;
        Opm_ReadReg(iOpmFd, OPM_CTRLOUT_REG, &uiOpmCtlout);
        //关闭光模块激光器电压设置
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        uiOpmCtlout &= ~value;
        iErr = Opm_WriteReg(iOpmFd, OPM_CTRLOUT_REG, &uiOpmCtlout);
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        使能光模块ADC
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnableAdc(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块ADC
        UINT32 regctlout;
        Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &regctlout);
		MutexLock(&(pCurrOpm->mOPMSetMutex));
        regctlout |= AD_CLK_EN;
        iErr = Opm_WriteReg(iOpmFd, FPGA_CTRLOUT_REG,
                            &(regctlout));
		MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        关闭光模块ADC
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisableAdc(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //关闭光模块ADC
        UINT32 regctlout;
        Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &regctlout);
		MutexLock(&(pCurrOpm->mOPMSetMutex));
        regctlout &= ~AD_CLK_EN;
        iErr = Opm_WriteReg(iOpmFd, FPGA_CTRLOUT_REG, 
                            &(regctlout));
		MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}

/***
  * 功能：
        使能光模块APD电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnableApdPower(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块APD电压设置
        unsigned int uiFpgaTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        uiFpgaTrigCtrl |= APD_POWER_START;
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
		MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}

/***
  * 功能：
        关闭光模块APD电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisableApdPower(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块APD电压设置
        unsigned int uiFpgaTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        uiFpgaTrigCtrl &= ~APD_POWER_START;
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}

/***
  * 功能：
        检测光模块APD电压设置是否打开
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        打开返回1，关闭返回0，失败返回-1
  * 备注：
  		测试通过
***/
int Opm_openApdPower(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块APD电压设置
        unsigned int uiFpgaTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        iErr = uiFpgaTrigCtrl & APD_POWER_START;
    }

    return iErr;
}

/***
  * 功能：
        使能ADC 1.8v电源控制
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnableAdcVoltage(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块激光器电压设置
        unsigned int uiFpgaTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        MutexLock(&(pCurrOpm->mOPMSetMutex));

        uiFpgaTrigCtrl |= ADC_CONFIG_START;
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));

        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        关闭adc 1.8V电源控制
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisableAdcVoltage(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        unsigned int uiTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &uiTrigCtrl);
        //关闭光模块激光器电压设置
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        uiTrigCtrl &= ~ADC_CONFIG_START;
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &uiTrigCtrl);
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        使能光模块激光器VTT电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_EnableLaserVoltage(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块激光器电压设置
        unsigned int uiFpgaTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        MutexLock(&(pCurrOpm->mOPMSetMutex));
		
        uiFpgaTrigCtrl |= DAC122S_START;
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
		
		MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}
/***
  * 功能：
        关闭光模块激光器电压设置
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_DisableLaserVoltage(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块激光器电压设置
        unsigned int uiFpgaTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        MutexLock(&(pCurrOpm->mOPMSetMutex));

        uiFpgaTrigCtrl &= ~DAC122S_START;
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));

        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}

/***
  * 功能：
        检测光模块激光器电压设置是否打开
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        打开返回1，关闭返回0，失败返回-1
  * 备注：
  		测试通过
***/
int Opm_openLaserVoltage(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块激光器电压设置
        unsigned int uiFpgaTrigCtrl = 0;
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &(uiFpgaTrigCtrl));
        iErr = uiFpgaTrigCtrl & DAC122S_START;
    }

    return iErr;
}
/***
  * 功能：
        使能光模块激光器
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_LdPulseTurnOn(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //使能光模块激光器
        UINT32 regctlout;
        Opm_ReadReg(iOpmFd, FPGA_OPTICAL_POWER, &regctlout);
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        regctlout |= OPM_LD_EN;
        iErr = Opm_WriteReg(iOpmFd, FPGA_OPTICAL_POWER,
                            &(regctlout));
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        关闭光模块激光器
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_LdPulseTurnOff(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //关闭光模块激光器
        UINT32 regctlout;
        Opm_ReadReg(iOpmFd, FPGA_OPTICAL_POWER, &regctlout);
        MutexLock(&(pCurrOpm->mOPMSetMutex));
        regctlout &= ~OPM_LD_EN;
        iErr = Opm_WriteReg(iOpmFd, FPGA_OPTICAL_POWER,
                            &(regctlout));
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        检测光模块激光器是否打开
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        打开返回1，关闭返回0，失败返回-1
  * 备注：
  		测试通过
***/
int Opm_OpenLdPulse(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //关闭光模块激光器
        UINT32 regctlout;
        Opm_ReadReg(iOpmFd, FPGA_OPTICAL_POWER, &regctlout);
        iErr = regctlout & OPM_LD_EN;
    }

    return iErr;
}


/***
  * 功能：
        得到当前光模块的波长
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        枚举类型，当前光模块的波长
  * 备注：
  		测试通过
***/
OPM_WAVE Opm_GetWave(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    OPM_WAVE enuWave = ENUM_WAVE_1310NM;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
		unsigned int uiWave;
        //得到当前光模块的波长
        iErr = Opm_WriteReg(iOpmFd, OPM_CTRLOUT_REG, &uiWave);

        uiWave = (uiWave & WAVE_CTRL_MASK);
		
        if (uiWave == WAVE_1310)
        {
            enuWave = ENUM_WAVE_1310NM;
        }
		else if (uiWave == WAVE_1550)
        {
            enuWave = ENUM_WAVE_1550NM;
        }
    }

    return enuWave;
}


/***
  * 功能：
        设置当前光模块的波长
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
        3.OPM_WAVE enuOpmWave:  枚举类型，要设置的波长
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
		测试通过
***/
int Opm_SetWave(int iOpmFd, OPMSET *pCurrOpm, OPM_WAVE enuOpmWave)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiWave;

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {

		iErr = Opm_ReadReg(iOpmFd, OPM_CTRLOUT_REG, &uiWave);

		if(enuOpmWave == ENUM_WAVE_1310NM)
		{
			uiWave = (uiWave & ~WAVE_CTRL_MASK) | (WAVE_1310);
		}
		else if(enuOpmWave == ENUM_WAVE_1550NM)
		{
			uiWave = (uiWave & ~WAVE_CTRL_MASK) | (WAVE_1550);
		}
		else if(enuOpmWave == ENUM_WAVE_1625NM)
		{
			//待拓展1625波长
		}

        //设置当前光模块的波长
        iErr = Opm_WriteReg(iOpmFd, OPM_CTRLOUT_REG, &uiWave);
    }
    
    return iErr;
}


/***
  * 功能：
        得到当前光模块的脉宽
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        枚举类型，当前光模块的脉宽
  * 备注：
  		测试通过
***/
OPM_PULSE Opm_GetPulse(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    OPM_PULSE enuPulse = ENUM_PULSE_5NS;
    //临时变量定义
    //unsigned int uiPulse;

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //得到当前光模块的脉宽
        //待定
    }

    return enuPulse;
}


/***
  * 功能：
        设置当前光模块的脉宽
  * 参数：
        1.int iOpmFd:               光模块设备文件描述符
        2.OPMSET *pCurrOpm:         指针，指向当前光模块的参数
        3.OPM_PULSE enuOpmPulse:    枚举类型，要设置的脉宽
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_SetPulse(int iOpmFd, OPMSET *pCurrOpm, OPM_PULSE enuOpmPulse)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiPulse;
	unsigned int uiCtrl;
	
    DBG_ENTER();
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        uiPulse = CalPulseTime(enuOpmPulse);
		Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &uiCtrl);

        if (ENUM_PULSE_5NS == enuOpmPulse)
        {
            //5ns脉冲如何设置
            uiCtrl |= SHORT_PULSE;
            //设置该寄存器，调整5ns的实际脉宽，（0~0x20）
            unsigned int temp = 0x10;
            Opm_WriteReg(iOpmFd, FPGA_OPTICAL_PWM_SET, &temp);
        }
		else
		{
			uiCtrl &= ~SHORT_PULSE;
		}
        #ifdef MINI2
        //30ns需要特殊设置
        if (ENUM_PULSE_30NS == enuOpmPulse)
        {
            
            uiCtrl |= PULSE_30NS_EN;	
        }
		else
		{
			uiCtrl &= ~PULSE_30NS_EN;
		}
        #else
        //mini1未添加30ns，但FPGA寄存器有30ns的使能位，所以需要置为0
        uiCtrl &= ~PULSE_30NS_EN;
        #endif
        //设置当前光模块的脉宽
        pCurrOpm->pCurrReg->uiFpgaCtrlOut = uiCtrl;
        iErr = Opm_WriteReg(iOpmFd, FPGA_LD_PULSE_WIDTH, &uiPulse);
		iErr = Opm_WriteReg(iOpmFd, FPGA_CTRLOUT_REG, &uiCtrl);
    }
    DBG_EXIT(iErr);
    return iErr;
}

/***
  * 功能：
        得到采样计数
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        当前的采样计数
  * 备注：
  		测试通过
***/
unsigned int Opm_GetCount(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    unsigned int uiCount = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //得到采样计数
        uiCount = pCurrOpm->pCurrReg->uiAdcCount;
    }

    return uiCount;
}


/***
  * 功能：
        设置采样计数
  * 参数：
        1.int iOpmFd:                   光模块设备文件描述符
        2.OPMSET *pCurrOpm:             指针，指向当前光模块的参数
        3.unsigned int uiSampleCount:   要设置的采样计数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_SetCount(int iOpmFd, OPMSET *pCurrOpm, unsigned int uiSampleCount)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
    	pCurrOpm->pCurrReg->uiAdcCount= uiSampleCount;
		
        //设置采样计数
        uiSampleCount = uiSampleCount & 0xFFFF;
        iErr = Opm_WriteReg(iOpmFd, FPGA_ADC_CNT_L, &uiSampleCount);
		
        uiSampleCount = (uiSampleCount & 0xFFFF0000) >> 16;
        iErr = Opm_WriteReg(iOpmFd, FPGA_ADC_CNT_H, &uiSampleCount);
    }

    return iErr;
}




/***
  * 功能：
        得到采样间隔
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        当前的采样间隔
  * 备注：
  		测试通过
***/
unsigned int Opm_GetGap(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    unsigned int uiGap = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    return uiGap;
}


/***
  * 功能：
        设置采样间隔
  * 参数：
        1.int iOpmFd:               光模块设备文件描述符
        2.OPMSET *pCurrOpm:         指针，指向当前光模块的参数
        3.unsigned int uiSampleGap: 要设置的采样间隔
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_SetGap(int iOpmFd, OPMSET *pCurrOpm, unsigned int uiSampleGap)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm) || (uiSampleGap > 0xFFFF))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //设置采样间隔
        iErr = Opm_WriteReg(iOpmFd, FPGA_DAQ_GAP, &uiSampleGap);
    }

    return iErr;
}


/***
  * 功能：
        得到光模块当前的滤波参数
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        枚举类型，当前的滤波参数
  * 备注：
  		测试通过
***/
OPM_FILTER Opm_GetFilter(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    OPM_FILTER enuFilter = _IIR_512;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
		unsigned int uiFilter;
        //尝试读取FPGA_FILTER_SEL
        iErr = Opm_ReadReg(iOpmFd, FPGA_FILTER_SEL, &uiFilter);
        if (!iErr)
		{
			//更新光模块寄存器FPGA_FILTER_SEL值
			uiFilter = uiFilter & 0xF;
			//得到光模块当前的滤波参数
			if (uiFilter <= _IIR_128K)
			{
				enuFilter = (OPM_FILTER) uiFilter;
			}
		}
    }

    return enuFilter;
}


/***
  * 功能：
        设置光模块当前的滤波参数
  * 参数：
        1.int iOpmFd:               光模块设备文件描述符
        2.OPMSET *pCurrOpm:         指针，指向当前光模块的参数
        3.OPM_FILTER enuFilterArg:  枚举类型，要设置的滤波参数
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_SetFilter(int iOpmFd, OPMSET *pCurrOpm, OPM_FILTER enuFilterArg)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //更新光模块寄存器FPGA_FILTER_SEL值
        UINT32 uiFilter = 0;
        uiFilter = 0x1F & enuFilterArg;
        //设置光模块当前的滤波参数
        iErr = Opm_WriteReg(iOpmFd, FPGA_FILTER_SEL, &uiFilter);
    }

    return iErr;
}

/***
  * 功能：
        读取FPGA_DAQMODE_SET寄存器里的值，用来获取状态
  * 参数：
        1.int iOpmFd:           光模块设备文件描述符
        2.OPMSET *pCurrOpm:     指针，指向当前光模块的参数
  * 返回：
		FPGA_DAQMODE_SET寄存器的值
***/
UINT32 Opm_GetDaqMode(
int iOpmFd, 
OPMSET *pCurrOpm
)
{
    UINT32 uiMode;
    Opm_ReadReg(iOpmFd, FPGA_DAQMODE_SET, &uiMode);
    return uiMode;
}


/***
  * 功能：
        设置光模块当前的数据采集模式
  * 参数：
        1.int iOpmFd:           光模块设备文件描述符
        2.OPMSET *pCurrOpm:     指针，指向当前光模块的参数
        3.OPM_MODE enuDaqMode:  枚举类型，要设置的数据采集模式
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_SetMode(int iOpmFd, OPMSET *pCurrOpm, OPM_DAQ_MODE enuDaqMode)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiMode;
	//DBG_ENTER();
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //更新光模块寄存器FPGA_DAQMODE_SET值
        iErr = Opm_ReadReg(iOpmFd, FPGA_DAQMODE_SET, &uiMode);
		
		uiMode = (uiMode & ~(DAQ_MODE_MASK << DAQ_MODE_SHIFT)) | (enuDaqMode);
        //设置光模块当前的数据采集模式
        iErr = Opm_WriteReg(iOpmFd, FPGA_DAQMODE_SET, &uiMode);
    }
	//DBG_EXIT(iErr);
    return iErr;
}

/*
说明:
	用来设置采用移相插补，或者正常采集
*/

int Opm_SetIntp
(
int           iOpmFd,    	//OPM的设备句柄
OPMSET *pCurrOpm, 			//OPM的设置指针
int          iIntpReg    	//移相插补次数寄存器设置
)	
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiMode = 0;
    
    DBG_ENTER();
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
		iErr = Opm_ReadReg(iOpmFd, FPGA_DAQMODE_SET, &uiMode);

    	uiMode=uiMode&(~DAQ_INTP_MASK);//清除原来的设置
    	iIntpReg=iIntpReg&DAQ_INTP_MASK;//清除不必要的位
        uiMode=uiMode|iIntpReg;          //changed  2015.08.19
		
        //设置光模块当前的数据采集模式
        iErr = Opm_WriteReg(iOpmFd, FPGA_DAQMODE_SET, &uiMode);

    }
    DBG_EXIT(iErr);
    return iErr;
}

/*
int Opm_SetIntp
(
int           iOpmFd,    	//OPM的设备句柄
OPMSET *pCurrOpm, 			//OPM的设置指针
int          iIntpReg    	//移相插补次数寄存器设置
)	
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiMode = iIntpReg;
    
    DBG_ENTER();
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        iErr = Opm_WriteReg(iOpmFd, FPGA_DELAY_SET_P10, &uiMode);
    }
    DBG_EXIT(iErr);
    return iErr;
}
*/
/* 
用来计算移相周期的 
*/ 
void CalShiftPhase4_90( 
unsigned char iCycle, //移相90度，需要的数 
unsigned char *buf //移相计算的缓冲区,64个字节 
) 
{ 
	//int i,iOffset,iTemp,iCnt; 
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
	用来设置移相插补的相位移动值
*/
int Opm_SetPhase(int iOpmFd) 
{
    //错误标志、返回值定义
    int iErr = 0,i;
    unsigned int iPhase[32];//
    unsigned char buf[64];
    DBG_ENTER();
    //CalShiftPhase(0xd2,buf);//0xd2是经验值测试出来的   2015.09.10
	CalShiftPhase4_90(0xd2, buf);

	
    for(i=0;i<32;i++)
    {
        iPhase[i]=(buf[i*2+1]<<8)+buf[i*2];
    }
    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
    }
    //设置移相插补相位
    iErr=0;    
    for(i=1;i<32;i++)
    {
        iErr += Opm_WriteReg(iOpmFd, FPGA_DELAY_SET_P10+(i<<1), &iPhase[i]);
    }
    //FPGA_DELAY_SET_P10寄存器最后设置，产生有效信号
    iErr += Opm_WriteReg(iOpmFd, FPGA_DELAY_SET_P10, &iPhase[0]);    

    DBG_EXIT(iErr);
    return iErr;

}

/*
说明:
	用来读取移相插补的相位移动值
*/
int Opm_readPhase(int iOpmFd) 
{
	int iErr = 0,i;
    unsigned int iPhase[32];

	for(i = 1; i < 32; i++)
    {
        iErr += Opm_ReadReg(iOpmFd, FPGA_DELAY_SET_P10 + (i<<1), &iPhase[i]);
    }
	iErr += Opm_ReadReg(iOpmFd, FPGA_DELAY_SET_P10, &iPhase[0]);   

	LOG(LOG_INFO, "addr = 0x%x, iPhase[0] = 0x%x\n", FPGA_DELAY_SET_P10, iPhase[0]);
	for (i = 1; i < 32; i++)
	{
		LOG(LOG_INFO, "addr = 0x%x, iPhase[%d] = 0x%x\n", FPGA_DELAY_SET_P10 + (i<<1), i, iPhase[i]);
	}

	unsigned int iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &iReg);
	LOG(LOG_INFO, "\n FPGA_TRIG_CTRL = 0x%x\n\n", iReg);
	iReg = 0;
	Opm_ReadReg(iOpmFd, FPGA_DAQMODE_SET, &iReg);
	LOG(LOG_INFO, "\n FPGA_DAQMODE_SET = 0x%x\n\n", iReg);

	return 0;
}

/***
  * 功能：
        得到光模块当前的数据操作类型
  * 参数：
        1.int iOpmFd:       光模块设备文件描述符
        2.OPMSET *pCurrOpm: 指针，指向当前光模块的参数
  * 返回：
        枚举类型，当前的数据操作类型
  * 备注：
  		测试通过
***/
OPM_DAQ_CMD Opm_GetStatus(int iOpmFd, OPMSET *pCurrOpm)
{
    //错误标志、返回值定义
    int iErr = 0;
    OPM_DAQ_CMD enuOper = ENUM_DAQ_CMD_STOP;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
		unsigned int uiOper;
        //尝试读取FPGA_DAQ_CTRL
        iErr = Opm_ReadReg(iOpmFd, FPGA_DAQ_CTRL, &uiOper);
        if (!iErr)
		{
			//更新光模块寄存器FPGA_DAQ_CTRL值
			uiOper = uiOper & 0xF7;
			//得到光模块当前的数据操作类型
			uiOper = (uiOper & DAQ_CMD_MASK) >> DAQ_CMD_SHIFT;
			if (uiOper <= ENUM_DAQ_CMD_RESUME)
			{
				enuOper = (OPM_DAQ_MODE) uiOper;
			}
		}
    }

    return enuOper;
}


/***
  * 功能：O10
        设置光模块当前的数据操作类型
  * 参数：
        1.int iOpmFd:           光模块设备文件描述符
        2.OPMSET *pCurrOpm:     指针，指向当前光模块的参数
        3.OPM_OPER enuOperType: 枚举类型，要设置的数据操作类型
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
  		测试通过
***/
int Opm_SetDaqCmd(int iOpmFd, OPMSET *pCurrOpm, OPM_DAQ_CMD enuOperType)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int uiOper;
	DBG_ENTER();
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {

		iErr = Opm_ReadReg(iOpmFd, FPGA_DAQ_CTRL, &uiOper);
		
        uiOper = (uiOper & ~DAQ_CMD_MASK) | (enuOperType << DAQ_CMD_SHIFT);

        //设置光模块当前的数据操作类型
        iErr = Opm_WriteReg(iOpmFd, FPGA_DAQ_CTRL, &uiOper);
    }
	DBG_EXIT(iErr);
    return iErr;
}

/***
  * 功能：
        判断光模块的ADC是否已准备好
  * 参数：
        1.int iOpmFd:   光模块设备文件描述符
  * 返回：
        成功返回非零，失败返回零
  * 备注：
  		测试通过
  	
***/
int Opm_AdcReady(int iOpmFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;
	//DBG_ENTER();
    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //尝试读取FPGA_DAQ_CTRL
        iErr = Opm_ReadReg(iOpmFd, FPGA_DAQ_CTRL, &uiDaq);
        if (iErr)
        {
            iErr = -2;
        }
    }


    if (iErr == 0)
    {
        //判断光模块的ADC是否已准备好
        iRet = uiDaq & ADC_DATA_READY;
    }
	//DBG_EXIT(iRet);
    return iRet;
}

int Opm_GetDaqCtrl(int iOpmFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;

    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //尝试读取FPGA_DAQ_CTRL
        iErr = Opm_ReadReg(iOpmFd, FPGA_DAQ_CTRL, &uiDaq);
        if (iErr)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        iRet = uiDaq ;
    }

    return iRet;
}

/***
  * 功能：
        判断光模块的数据是否已准备好
  * 参数：
        1.int iOpmFd:   光模块设备文件描述符
  * 返回：
        成功返回非零，失败返回零
  * 备注：
  		测试通过
***/
int Opm_DataReady(int iOpmFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;

    if (iErr == 0)
    {
        //参数检查
        if (iOpmFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //尝试读取FPGA_DAQ_CTRL
        iErr = Opm_ReadReg(iOpmFd, FPGA_DAQ_CTRL, &uiDaq);
        if (iErr)
        {
            iErr = -2;
        }
    }
    if (iErr == 0)
    {
        //判断光模块的数据是否已准备好
        iRet = uiDaq & CPU_RD_DDR_READY;
    }

    return iRet;
}

/***
  * 功能：
        判断光模块的DAQ是否正忙
  * 参数：
        1.int iOpmFd:   光模块设备文件描述符
  * 返回：
        0=空闲，1=正在忙
  * 备注：
  		测试通过
***/
int Opm_GetDaqStatus(int iOpmFd)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;

    //尝试读取FPGA_DAQ_CTRL
    Opm_ReadReg(iOpmFd, FPGA_DAQ_CTRL, &uiDaq);

    //判断光模块的DAQ是否正忙
    iRet = uiDaq & DAQ_STATUS;
    return iRet;
}

/*
说明:
	用来检查ADC是否正常工作?
作者:
	
日期:
	2013.10.19
返回值:
	0=非常，1=正常工作
*/
int Opm_GetAdcStatue(
int iOpmFd					//光模块设备文件描述符
)
{
    //错误标志、返回值定义
    int iRet = 0;
    //临时变量定义
    unsigned int uiDaq;

    //尝试读取FPGA_DAQ_CTRL
    Opm_ReadReg(iOpmFd, FPGA_DAQMODE_SET, &uiDaq);

    LOG(LOG_INFO, "uiDaq:0x%04x\n", uiDaq);
    //判断光模块的DAQ是否正忙
    iRet = uiDaq & ADC_STATUE;
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
int Opm_ReadAdcData(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm,				//OPM设置的数据结构
UINT16	*pDataBuf				//采样
)
{
	int iRet = 0;
	
	OPTARG_DAT  arg;
	DBG_ENTER();
	arg.iDataCnt = pCurrOpm->pCurrReg->uiAdcCount;

	arg.pDataBuf = pDataBuf;
    
    memset(arg.pDataBuf, 0, sizeof(UINT16) * arg.iDataCnt);

	iRet = ioctl(iOpmFd, OPT_IOCTL_RDDAT, &arg);

	DBG_EXIT(iRet);
	return iRet;
}

/*
说明:
	用来设置APD电压的
备注:
  	测试通过
*/
void Opm_SetApdVoltage
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm,				//OPM设置的数据结构
UINT32	uiApdVoltage			//
)
{
    UINT32 uiApd = 0;
	uiApd = uiApdVoltage & 0xFFF;
	
	Opm_WriteReg(iOpmFd, FPGA_OPM_APD_VOLTAGE, &uiApd);

	//检测电源是否打开，若未打开则打开电源，否则不能重复打开电源
    if (!Opm_openApdPower(iOpmFd, pCurrOpm))
        //设置完成后，进行使能
        Opm_EnableApdPower(iOpmFd, pCurrOpm);

}

/*
说明:
	用来设置VDD电压的
备注:
  	测试通过
*/
void Opm_SetVddVoltage
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm,				//OPM设置的数据结构
UINT32	uiVddVoltage			//
)
{
    UINT32 uiVdd = 0;
	uiVdd = uiVddVoltage&0xFFF;

	Opm_WriteReg(iOpmFd, FPGA_VDD_VOLTAGE, &uiVdd);

	//检测电源是否打开，若未打开则打开电源，否则不能重复打开电源
    if (!Opm_openLaserVoltage(iOpmFd, pCurrOpm))
        //设置完成后，进行使能
        Opm_EnableLaserVoltage(iOpmFd, pCurrOpm);
}



/*
说明:	
	用来设置Amp放大通道的
备注:
	测试通过
*/
void Opm_SetAmp
(
int				iOpmFd,			//FPGA的设备句柄
OPMSET			*pCurrOpm,		//OPM设置的数据结构
OPM_AMP	EnumAmp	//前端放大
)
{
   int iErr = 0;
   UINT32 uiOpmCtrlReg;
   iErr = Opm_ReadReg(iOpmFd, OPM_CTRLOUT_REG, &uiOpmCtrlReg);

   uiOpmCtrlReg = uiOpmCtrlReg&(~AMP_MASK);
   uiOpmCtrlReg |= EnumAmp;

   iErr = Opm_WriteReg(iOpmFd, OPM_CTRLOUT_REG, &uiOpmCtrlReg);
 
}

/*
说明:
	用来得到前端放大档位的
备注:
	测试通过
*/
OPM_AMP Opm_GetAmp
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm				//OPM设置的数据结构
)
{
	UINT32 iOpmCtrlReg;
	OPM_AMP EnumReAmp;
	int iErr = 0;
	
   	iErr = Opm_WriteReg(iOpmFd, OPM_CTRLOUT_REG, &iOpmCtrlReg);

	iOpmCtrlReg = iOpmCtrlReg & AMP_MASK;
	EnumReAmp = iOpmCtrlReg;

	return EnumReAmp;
}




/*
说明:
	用来设置FPGA_OPM_ATT0,FPGA_OPM_ATT1这两个寄存器的
	Author Lichuyuan 2013.10.23
备注:
  	测试通过
*/
void Opm_SetAttValue
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm,				//OPM设置的数据结构
unsigned int uiAttValue          //设置值
)
{
    UINT32 uiAttVolt = 0;
	uiAttVolt = uiAttValue&0xFFF;

	Opm_WriteReg(iOpmFd, FPGA_TUNE_ATT, &uiAttVolt);

	//检测电源是否打开，若未打开则打开电源，否则不能重复打开电源
    if (!Opm_openLaserVoltage(iOpmFd, pCurrOpm))
        //设置完成后，进行使能
        Opm_EnableLaserVoltage(iOpmFd, pCurrOpm);
}

/*
说明:
	用来设置FPGA_CTRLOUT_REG整个寄存器的
	该寄存器，因为分散，所以干脆直接设置
作者:
	
日期:
	2013.10.19
备注:
  	测试通过
*/
void Opm_SetFpgaCtrlOut
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm				//OPM设置的数据结构
)
{
   Opm_WriteReg(iOpmFd, FPGA_CTRLOUT_REG, &pCurrOpm->pCurrReg->uiFpgaCtrlOut);
}
/*
说明:
	用来读取FPGA_CTRLOUT_REG整个寄存器的
	该寄存器，因为分散，所以干脆直接设置
作者:
	
日期:
	2013.10.19
备注:
  	测试通过
*/
void Opm_GetFpgaCtrlOut
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm				//OPM设置的数据结构
)
{
   Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &pCurrOpm->pCurrReg->uiFpgaCtrlOut);
}

/*
说明:
	DDR2的时钟打开后，必须先对它进行复位,只要写1就够了
备注:
  	测试通过
*/
void Opm_ResetFpgaDdr2(
int		iOpmFd                  //FPGA的设备句柄
)
{
   UINT32 uiFpgaDdr2Reset;
   uiFpgaDdr2Reset=RESET_DDR2;
   Opm_WriteReg(iOpmFd, FPGA_DDR_RESET, &uiFpgaDdr2Reset);   
}

/*
说明:
	设置FPGA的CLOCK
备注:

*/
void Opm_ClkTurnOn
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm				//OPM设置的数据结构
)
{
	UINT32 uiClockCtrl = 0;
    uiClockCtrl = OMCLK_CTRL;
    Opm_WriteReg(iOpmFd, FPGA_CLOCK_CTRL, &uiClockCtrl); 	
}
/*
说明:
	关闭FPGA的CLOCK
备注:

*/
void Opm_ClkTurnOff
(
int		iOpmFd,					//FPGA的设备句柄
OPMSET	*pCurrOpm				//OPM设置的数据结构
)
{
	UINT32 uiClockCtrl = 0;
    Opm_WriteReg(iOpmFd, FPGA_CLOCK_CTRL, &uiClockCtrl); 	
}

/*
说明:
	设置LED的使能
备注:

*/
int Opm_SetLedEnable(OPMSET *pCurrOpm, int iEnable)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    unsigned int iLed = 0;
    //获取光模块设备描述符
    DEVFD *pDevFd = NULL;
    pDevFd = GetGlb_DevFd();
    int iOpmFd = pDevFd->iOpticDev;

    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        Opm_ReadReg(iOpmFd, FPGA_TRIG_CTRL, &iLed);
        //设置led灯的使能
        if (iEnable == 1)
            iLed |= LED_EN;
        else
            iLed &= ~LED_EN;

        MutexLock(&(pCurrOpm->mOPMSetMutex));
        iErr = Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &iLed);
        MutexUnlock(&(pCurrOpm->mOPMSetMutex));
    }

    return iErr;
}


/***
  * 功能：
        得到电源信息
  * 参数：
        1.int iPwrFd:   电源设备文件描述符
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
***/
PWRINFO* Pwr_GetInfo(int iPwrFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    PWRINFO *pPwrInfo = NULL;
    int i = 0;
    int sum = 0;
    
    //临时变量定义

    //参数检查
	if (iErr == 0)
	{
		if (iPwrFd < 0)
		{
			iErr = -1;
		}
	}
	
	//申请资源
    if (iErr == 0)
    {
		pPwrInfo = (PWRINFO *)GuiMemAlloc(sizeof(PWRINFO));
		if (NULL == pPwrInfo)
		{
			iErr = -2;
		}
    }

	//从MCU读取电源信息
    if (iErr == 0)
    {

        for(i = 0; i<5;i++)
        {
            iErr = read(iPwrFd, pPwrInfo, sizeof(PWRINFO));
            if (iErr == sizeof(PWRINFO))
            {
                sum += pPwrInfo->ucStateDc;
                iErr = 0;
            }
            else
            {
                iErr = -3;
                break;
            }
        }
    }

    if(iErr == 0)
    {
        if(sum>3)
        {
            pPwrInfo->ucStateDc = 1;
        }
        else
        {
            pPwrInfo->ucStateDc = 0;
        }
    }

    switch (iErr)
    {
	case -1:
		break;
	case -2:
		break;
	case -3:
		GuiMemFree(pPwrInfo);
		pPwrInfo = NULL;
		break;
	default:
		break;
    }
    
    return pPwrInfo;
}

/***
  * 功能：
        关闭电源
  * 参数：
        1.int iPwrFd:   电源设备文件描述符
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int Pwr_ShutDown(void)
{
    //错误标志、返回值定义
    int iErr = 0;

#define SOFT_POWERDOWN 1	//关闭电源的IOCTL COMMAND

    //临时变量定义
	DEVFD *glbDevFd = NULL;
	int iPwrFd;
	
	glbDevFd = GetGlb_DevFd();
	iPwrFd = glbDevFd->iPowerDev;

	//参数检查
	if (iErr == 0)
	{
		if (iPwrFd < 0)
		{
			iErr = -1;
		}
	}

	//发送IOCTL COMMAND给MCU
	if (iErr == 0)
	{
		iErr = ioctl(iPwrFd, SOFT_POWERDOWN);
		if (iErr < 0)
		{
			iErr = -2;
			perror("ioctl()");
		}
	}
	
#undef SOFT_POWERDOWN
    return iErr;
}

/***
  * 功能：
        充电控制
  * 参数：
        1.int enable : 充电使能 0:禁止充电  1:允许充电
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int Pwr_Charge(int enable)
{
	#define SOFT_DISCHARGE 2	//禁止充电的IOCTL COMMAND
	#define SOFT_CHARGE 3	//允许充电的IOCTL COMMAND

    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
	DEVFD *glbDevFd = NULL;
	int iPwrFd;
	
	glbDevFd = GetGlb_DevFd();
	iPwrFd = glbDevFd->iPowerDev;

	//参数检查
	if (iErr == 0)
	{
		if (iPwrFd < 0)
		{
			iErr = -1;
		}
	}

	//发送IOCTL COMMAND给MCU
	if (iErr == 0)
	{
		if(enable)
		{
			iErr = ioctl(iPwrFd, SOFT_CHARGE);
		}
		else
		{
			iErr = ioctl(iPwrFd, SOFT_DISCHARGE);
		}
		
		if (iErr < 0)
		{
			iErr = -2;
			perror("ioctl()");
		}
	}
	
	#undef SOFT_DISCHARGE
	#undef SOFT_CHARGE
    return iErr;
}


/***
  * 功能：
        得到RTC时间
  * 参数：
        1.int iRtcFd:   RTC设备文件描述符
  * 返回：
        成功返回有效指针，否则返回NULL
  * 备注：
***/
RTCVAL* Rtc_GetTime(int iRtcFd)
{
    //错误标志、返回值定义
    int iErr = 0;
    RTCVAL *pRtc = NULL;
    //临时变量定义

    if (iErr == 0)
    {
        //参数检查
        if (iRtcFd < 0)
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //尝试分配内存 为了兼容struct tm 结构 加一个long长度 加一个char*长度
        pRtc = (RTCVAL *)GuiMemAlloc(sizeof(RTCVAL) + sizeof(long) + sizeof(char *));
        if (NULL == pRtc)
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //得到RTC时间
        iErr = ioctl(iRtcFd, RTC_RD_TIME, pRtc);
       if (iErr < 0)
        {
            iErr = -3;
        }
    }

    //错误处理
    switch (iErr)
    {
    case -3:
        GuiMemFree(pRtc); 
    case -2:
    case -1:
        pRtc = NULL;
    default:
        break;
    }

    return pRtc;
}

/***
  * 功能：
        设置RTC时间
  * 参数：
        1.int iRtcFd:       RTC设备文件描述符
        2.RTCVAL *pRtcVal:  指针，指向用于设置RTC的时间值
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int Rtc_SetTime(int iRtcFd, RTCVAL *pRtcVal)
{
    //错误标志、返回值定义
    int iErr = 0;
    //临时变量定义
    struct timeval tv;

    if (iErr == 0)
    {
        //参数检查
        if ((iRtcFd < 0) || (NULL == pRtcVal))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
        //判断pRtc中时间值的可用性
        if ((pRtcVal->tm_sec < 0 || pRtcVal->tm_sec > 59) || 
            (pRtcVal->tm_min < 0 || pRtcVal->tm_min > 59) || 
            (pRtcVal->tm_hour < 0 || pRtcVal->tm_hour > 23) || 
            (pRtcVal->tm_mday < 1 || pRtcVal->tm_mday > 31) || 
            (pRtcVal->tm_mon < 0 || pRtcVal->tm_mon > 11) || 
            (pRtcVal->tm_year < 1970 || pRtcVal->tm_year > 2037) || 
            (pRtcVal->tm_wday < 0 || pRtcVal->tm_wday > 6) || 
            (pRtcVal->tm_yday < 0 || pRtcVal->tm_yday > 365))
        {
            iErr = -2;
        }
    }

    if (iErr == 0)
    {
        //检查pRtc中月份与天数的一致性
        switch (pRtcVal->tm_mon)
        {
        case 3:     //mon:4
        case 5:     //mon:6
        case 8:     //mon:9
        case 10:    //mon:11
            if (pRtcVal->tm_mday > 30)
            {
                iErr = -3;
            }
            break;
        case 1:     //mon:2
            if ((pRtcVal->tm_year % 400 != 0) && 
                ((pRtcVal->tm_year % 4 != 0) || (pRtcVal->tm_year % 100 == 0)))
            {
                if (pRtcVal->tm_mday > 28)
                {
                    iErr = -3;
                }
            }
            break;
        default:
            break;
        }
    }

    if (iErr == 0)
    {
		//pRtcVal->tm_year = pRtcVal->tm_year - 1900;
        //设置RTC时间
        iErr = ioctl(iRtcFd, RTC_SET_TIME, pRtcVal);
        if (iErr == -1)
        {
            iErr = -4;
        }
    }

    if (iErr == 0)
    {
        //设置系统时间
        pRtcVal->tm_year -= 1900;
        tv.tv_sec = mktime((struct tm *)pRtcVal);
        tv.tv_usec = 0;
        iErr = settimeofday(&tv, NULL);
        if (iErr == -1)
        {
            iErr = -5;
        }
		pRtcVal->tm_year += 1900;
    }

    return iErr;
}

/*
说明：
	该函数主要用来设置ADC采样参数的 ,会设置以下参数
	1. ADC采样的个数                FPGA_ADC_CNT
	2. ADC采样的Gap                 FPGA_DAQ_GAP
	3. ADC采样延迟的个数            FPGA_ADC_DELAY_CNT
返回:
	0=正常，-1=失败
*/
int SedAdcParameter(
int    iOpmFd,		  		//FPGA的设备句柄
OPMSET *pCurrOpm,			//OPM设置的数据结构
UINT32 uiFiberLen,			//光纤的长度,需要采样的点数
UINT32 gap	              //黑电平采集标志(采集黑电平时GAP为定值)
)
{
    //错误标志、返回值定义
	int iErr = 0;
	
    //临时变量定义
	DBG_ENTER();
	
    if (iErr == 0)
    {
        //参数检查
        if ((iOpmFd < 0) || (NULL == pCurrOpm))
        {
            iErr = -1;
        }
    }

    if (iErr == 0)
    {
    	pCurrOpm->pCurrReg->uiAdcCount = uiFiberLen;

        //设置采样计数
        unsigned int uiSampleCount = uiFiberLen & 0xFFFF;
        iErr = Opm_WriteReg(iOpmFd, FPGA_ADC_CNT_L, &uiSampleCount);
        uiSampleCount = (uiFiberLen & 0xFFFF0000) >> 16;
        iErr = Opm_WriteReg(iOpmFd, FPGA_ADC_CNT_H, &uiSampleCount);
	
        //更新光模块寄存器FPGA_DAQ_GAP值 采集间隔
        iErr = Opm_WriteReg(iOpmFd, FPGA_DAQ_GAP, &gap);
    }
	DBG_EXIT(iErr);

    return iErr;

}
/*
说明:
	该函数根据枚举类型OPM_PULSE，来计算脉宽的clk的
作者:
	
日期:
	2013.10.19
备注:
  	测试通过
*/
int CalPulseTime
(
OPM_PULSE enPulseTimeLen	//输入的枚举类型的脉冲长度
)
{
    #ifdef MINI2
    int iTimeLen[OPM_PULSE_KIND] = {1000, 5, 10, 20, 30, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
    #else
    int iTimeLen[OPM_PULSE_KIND]={1000, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 10000, 20000};
    #endif
    int iPulseTime;
    int iFpgaClockCycle;
    iFpgaClockCycle = 1000 / FPGA_CLOCK_FREQ;
	DBG_ENTER();
    switch(enPulseTimeLen)
    {
	case ENUM_PULSE_AUTO:
		iPulseTime = 0;	
		break;
	case ENUM_PULSE_5NS:
		iPulseTime = 0;  
		break;
	case ENUM_PULSE_10NS:
		iPulseTime = 0;		//10ns需做额外的设置，此处暂时发出12.5ns
		break;
	case ENUM_PULSE_20NS:
		iPulseTime = 1; 	//20ns需做额外的设置，此处暂时发出25ns
		break;
    #ifdef MINI2
	case ENUM_PULSE_30NS:
		iPulseTime = 1; 	//30ns需做额外的设置，
		break;
    #endif
	default:
		iPulseTime = iTimeLen[enPulseTimeLen] / iFpgaClockCycle;
		break;
    }
    CODER_LOG(CoderAlgorithm, "+++++++++++iPulseTime = %d++++++++++++\n",iPulseTime);

	DBG_EXIT(iPulseTime);
    return iPulseTime;
};

/*
说明:
	因为APD 击穿电压随温度改变而改变的原因，
	因此必须实时检测OPM 的温度
备注:
  	测试通过 返回值为0读取成功
  	O10  0918
*/

int GetOpmTemperature
(
int iOpmFd,					//光模块设备的设备描述符
float *pTempValue			//指向温度的指针变量
)
{
	//错误标志、返回值定义
	int iErr = 0;
#if 0
//定义温度临时变量
	UINT32 iReg = 0;
	iReg = 0;
	iReg = LM_START_CONV;
	Opm_WriteReg(iOpmFd, FPGA_TRIG_CTRL, &iReg);
	iReg = 0;
	while(1)
    {
        Opm_ReadReg(iOpmFd, FPGA_LM74_CTRL, &iReg);
		
        if(!(iReg&LM74_BUSY))
        	break;
		else
			LOG(LOG_ERROR,"---LM--while---\n");
    }
	if(!iErr)
	{
		Opm_ReadReg(iOpmFd, FPGA_LM74_READ, &iReg);
		
		CODER_LOG(CoderAlgorithm, "Current temperature Reg is 0x%4x\n", iReg);

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
		
		CODER_LOG(CoderAlgorithm, "Current temperature is %f\n", *pTempValue);
	}
    /*
    static float test = -20.0f;
        test++;
    if(test > 80.0f)
        test = -20.0f;
	*pTempValue = test;
    */
#endif
    int temp = 0;
    //温度读取方式修改为iic，通过驱动直接读取芯片温度
    ioctl(iOpmFd, OPT_IOCTL_GET_TEMP, &temp);
    *pTempValue = (float)(temp / 1000.0f);

    return iErr;
}

//初始化Opm_init的时候对这些寄存器值初始化
int GetFpgaRegVal(OPMSET *pOpm)
{
	int iRet,iErr = 0;
	DEVFD *pDevFd = NULL;
	int iOpmFd;
	UINT32  uiRegVal;
	
	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;

	uiRegVal = 0;
    iRet = Opm_ReadReg(iOpmFd, FPGA_CTRLOUT_REG, &uiRegVal);
	if(iRet)
		iErr = -1;
	uiRegVal &= (~SHORT_PULSE);//关闭短脉宽3ns
	uiRegVal &= (~OPM_PWR_EN);//关闭模拟板电源
	pOpm->pCurrReg->uiFpgaCtrlOut = uiRegVal;

	LOG(LOG_INFO, "----FPGA_CTRLOUT_REG = 0x%x----\n",uiRegVal);
    iRet = Opm_WriteReg(iOpmFd, FPGA_CTRLOUT_REG, &(uiRegVal));
	if(iRet)
		iErr = -2;
	
	pOpm->pCurrReg->uiAdcCount = 0;
				
	return iErr;
}
