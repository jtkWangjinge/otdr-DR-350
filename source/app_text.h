/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_text.h
* 摘    要：  实现多国语言头文件
*
* 当前版本：  v1.0.0  
* 作    者：  
* 完成日期：  
*
* 取代版本： 
* 作    者：  
* 完成日期：  
*******************************************************************************/
#ifndef _APP_TEXT_H
#define _APP_TEXT_H

/*****************************
* 包含app_text.c所需要的头文件
*****************************/
#include <stdio.h>
#include "guibase.h"
#include "sqlite3.h"

/*****************************
* 定义多国语言支持所需的宏定义
*****************************/
#define 	LANG_TYPE			0x22 	//支持的语言总数

//各国语言编码

#define 	LANG_CHINASIM       0x00    //汉语简体
#define 	LANG_ENGLISH        0x01    //英语
#define 	LANG_KOREAN         0x02    //韩语
#define 	LANG_JAPANESE       0x03    //日语
#define 	LANG_RUSSIAN        0x04    //俄语
#define 	LANG_GERMAN         0x05    //德语
#define 	LANG_FRANCE         0x06    //法语
#define 	LANG_VIETNAM        0x07    //越南语
#define 	LANG_SPANISH        0x08    //西班牙语
#define 	LANG_TURKISH        0x09    //土耳其语
#define 	LANG_ITALY          0x0A    //意大利语
#define 	LANG_PORTUGAL       0x0B    //葡萄牙语
#define 	LANG_HINDI          0x0C    //印地语
#define 	LANG_SWIDISH        0x0D    //瑞典语
#define 	LANG_POLISH         0x0E    //波兰语
#define 	LANG_UYGHUR         0x0F    //维吾尔语
#define 	LANG_BENGALI        0x10    //孟加拉语
#define 	LANG_DANISH         0x11    //丹麦语
#define 	LANG_CZECH          0x12    //捷克语
#define 	LANG_UKRAINIAN      0x13    //乌克兰语
#define 	LANG_GREEK          0x14    //希腊语
#define 	LANG_TAGALOG        0x15    //菲律宾语
#define 	LANG_LATIN          0x16    //拉丁语
#define 	LANG_IRISH          0x17    //爱尔兰语
#define 	LANG_HUNGARIAN      0x18    //匈牙利语
#define 	LANG_DUTCH          0x19    //荷兰语
#define 	LANG_ROMANA         0x1A	//罗马语
#define 	LANG_SUOMI		    0x1B	//芬兰语
#define 	LANG_SERBIAN	    0x1C    //塞尔维亚语
#define 	LANG_CROATIA	    0X1D	//克罗地亚语
#define 	LANG_THAI           0x1E    //泰语
#define 	LANG_ARABIA         0x2F    //阿拉伯语
#define 	LANG_PERSIAN        0x20    //波斯语

//语言类型名
extern char *strLangType[];

//获取GUI当前的语言环境
int GetCurrLanguage(void);

//设置当前的语言环境
int SetCurrLanguage(unsigned int uiLangIndex);

//获得GUI当前的语言环境
int GetCurrLanguageSet(void);

//获得GUI当前的语言文本
char* GetLanguageStrType(void);

//打开语言数据库文件
int OpenLanguageDb(char *pDbName);

//获取当前语言环境的文本字符串
GUICHAR *GetCurrLanguageText(int iIndx);

//获取当前语言环境下文本的utf8编码
char *GetCurrLanguageUtf8Text(int iIndx, unsigned int language);

//转换utf8字符串到unicode
GUICHAR *GetUcs2FromUtf8(char *pUtf8Code);

//获取多国语所对应的字符串的unicode编码
GUICHAR *GetTextUnicode(int iIndx, char *strLanguage);
	
#endif //_APP_TEXT_H
