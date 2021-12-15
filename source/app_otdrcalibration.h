/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_OtdrCalibration.h
* 摘    要：  生成 校准数据的函数定义
*
* 当前版本：  v1.0.0 
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/
#include "app_frmotdr.h"

#define UNCERT_DATA_LEN     			  10
#define LINEARITY_MAX_VALUE               20
/********************************************************************************************************
* 函数描述                    计算直线的线性度
* @ param[in] LogSignal     输入数据
* @ return                            线性度
* @ note      name 描述
*********************************************************************************************************/
float CalcLinearity(POTDR_TOP_SETTINGS pOtdrTopSettings);

/********************************************************************************************************
* 函数描述                                  计算数据的不确定度
* @ param[in] fData                           输入测试数据
* @ param[in] fFiberRealLen             输入光纤校准的长度
* @ param[in] fDeviation                    输出偏差
* @ param[in] fFiberTestLen              输出光纤测试长度的均值
* @ param[in] Uncertainty                  不确定度
* @ param[in] fConformance             输出符合度的极限值
* @ param[in] fSpecification               输入规范值
* @ return                                           不确定度
* @ note      name 描述
*********************************************************************************************************/
float CalcUncertainty(float fData[UNCERT_DATA_LEN], float fFiberRealLen, float *fDeviation,
					float *fFiberTestLen, float *Uncertainty, float *fConformance, float fSpecification);



