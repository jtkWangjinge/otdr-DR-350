/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_frmvls.c  
* 摘    要：  实现用于完成可视光源模块功能的应用程序接口
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：
*******************************************************************************/

#include "app_frmvls.h"

/*******************************************************************************
**				    为实现app_frmvls而需要引用的其他头文件                   **
*******************************************************************************/
#include "app_middle.h"
#include "app_frmotdr.h"

/*******************************************************************************
**				         app_frmvls定义的对外接口函数                         **
*******************************************************************************/
extern POTDR_TOP_SETTINGS pOtdrTopSettings;

/***
  * 功能：
        产生连续光函数
  * 参数：
        int OnOff:  1----打开  0----关闭
  * 返回：
        0
  * 备注：
        无
***/
int CreateCW(int OnOff)
{
	//临时变量定义
	UINT32 regctlout;
	
	DEVFD *pDevFd;
	int iOpmFd;
	//获得设备描述符
	// OPM_REG_CTRL *pOpmCurrReg = pOtdrTopSettings->pOpmSet->pCurrReg;
	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;
	//读寄存器并改写寄存器连续光使能
	Opm_ReadReg(iOpmFd,FPGA_CTRLOUT_REG,&regctlout);
	if(OnOff == 1)
	{
		regctlout |= VFL_CW;
	}
	else
	{
		//关闭连续光
		regctlout &= ~VFL_CW;
	}
	// regctlout = pOpmCurrReg->uiFpgaCtrlOut;
	Opm_WriteReg(iOpmFd,FPGA_CTRLOUT_REG,&regctlout);
		
	return 0;
}


/***
  * 功能：
        产生2Hz光函数
  * 参数：
        int OnOff:  1----打开  0----关闭
  * 返回：
        0
  * 备注：
        无
***/
int Create2Hz(int OnOff)
{
	//错误标志、返回值定义
	int iReturn = 0;
	//临时变量定义
	UINT32 regctlout;
	
	DEVFD *pDevFd;
	int iOpmFd;
	//获得设备描述符
	// OPM_REG_CTRL *pOpmCurrReg = pOtdrTopSettings->pOpmSet->pCurrReg;
	pDevFd = GetGlb_DevFd();
	iOpmFd = pDevFd->iOpticDev;
	//读寄存器并改写寄存器连续光使能
	Opm_ReadReg(iOpmFd,FPGA_CTRLOUT_REG,&regctlout);
	
	if(OnOff == 1)
	{
		regctlout |= VFL_FLASH;
	}
	else
	{
		//关闭闪烁
		regctlout &= ~VFL_FLASH;
	}
	// regctlout = pOpmCurrReg->uiFpgaCtrlOut;
	Opm_WriteReg(iOpmFd, FPGA_CTRLOUT_REG, &regctlout);
	
	return iReturn;	
}

