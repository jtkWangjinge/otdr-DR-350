/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_rj45.h
* 摘    要：  声明rj45公共操作函数。
*
* 当前版本：  v1.0.0
* 作    者：  wjg
* 完成日期：  
*******************************************************************************/

/***
 * 寻线功能如下
 * 
 ***/
/***
  * 功能：
        初始化参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 1、申请gpio；
  * 2、初始化gpio数据
***/
void InitialCableSearchParameter(void);

/***
  * 功能：
        销毁参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 去除申请gpio
***/
void ClearCableSearchParameter(void);

/***
  * 功能：
        测试寻线功能
  * 参数：无
  * 返回：无
  * 备注：
***/
void TestCableSearch(void);

/***
 * 对线功能如下
 * 
 ***/
/***
  * 功能：
        初始化参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 1、申请gpio；
  * 2、初始化gpio数据
***/
void InitialCableSequenceParameter(void);

/***
  * 功能：
        销毁参数数据
  * 参数：无
  * 返回：无
  * 备注：
  * 去除申请gpio
***/
void ClearCableSequenceParameter(void);

/***
  * 功能：
        测试对线功能
  * 参数：无
  * 返回：无
  * 备注：
***/
int TestCableSequence(void);