/****************************************Copyright (c)****************************************************
**
**                            捷太科仪器(苏州)有限公司上海研发部
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  app_measuringresult.h
** Latest Version:             V1.0.0
** Latest modified Date:       
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 
** Created date:               
** Descriptions:               保存测量结果信息，给测试和工程使用
** 
*********************************************************************************************************/

#ifndef APP_MEASURING_RESULT_H
#define APP_MEASURING_RESULT_H

/**
* 设置是否保存测量结果，默认是不保存
* @param[in] enable: 0 或者 1
* @return void
*/
void SetMeasuringResultEnable(int enable);


/**
* 保存OTDR测量结果信息
* U盘不存在则不保存；U盘存在则保存到U盘下Result目录下面
* @param 	void 
* @return	void
*/
void SaveOTDRMeasuringResult();

/**
* 保存SOLA测量结果信息
* U盘不存在则不保存；U盘存在则保存到U盘下Result目录下面
* @param 	void 
* @return	void
*/
void SaveSOLAMeasuringResult();

#endif // APP_MEASURING_RESULT_H

/*********************************************************************************************************
** End of file
*********************************************************************************************************/
