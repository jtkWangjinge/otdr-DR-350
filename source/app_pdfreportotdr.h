/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_pdfreportotdr.h  
* 摘    要：  实现otdr生成PDF报告
*
* 当前版本：  v1.0.0  
* 作    者：  
* 完成日期：
*
* 取代版本： 
* 作    者：  
* 完成日期：  
*******************************************************************************/

//得到报告绝对路径，包含文件名 传入0返回 绝对路径  传入1 返回文件名
char* otdrPdfReportGetPathOrName(int pathOrName);

//生成报告
int CreateOtdrReport(char* absolutePath);