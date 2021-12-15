/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_text.c  
* 摘    要：  实现多国语言
*
* 当前版本：  v1.0.0  
* 作    者：  
* 完成日期：
*
  
*******************************************************************************/

/*****************************
* 包含app_text.c所需要的头文件
*****************************/
#include "app_text.h"
#include "app_parameter.h"
#include "app_persianReshape.h"

/*******************************
* 定义app_text.c所使用的公有变量
*******************************/
//各国语言的名称，在数据库为列标识
extern PSYSTEMSET pCurSystemSet;
char *strLangType[] = 
{
	"China",		//汉语简体
	"English",		//英语
	"Spanish",		//西班牙语
	"France",		//法语
	"Portugal",		//葡萄牙语
	"Russian",		//俄语		
	"Italy",		//意大利语
	"Thai",			//泰语
// 	"China_Sim",    //汉语简体
// 	"English",    	//英语
// 	"Korea",   		//韩语
// 	"Japanese",    	//日语
// 	"Russian",    	//俄语
// 	"German",    	//德语
// 	"France",    	//法语
// 	"Vietnam",    	//越南语
// 	"Spanish",    	//西班牙语
// 	"Turkish",    	//土耳其语
// 	"Italy",    	//意大利语
// 	"Portugal",    	//葡萄牙语
// 	"Hindi",    	//印地语
// 	"Swedish",    	//瑞典语
// 	"Polish",    	//波兰语
// 	"Uyghur",    	//维吾尔语
// 	"Bengali",    	//孟加拉语
// 	"Danish",    	//丹麦语
// 	"Czech",    	//捷克语
// 	"Ukrainian",    //乌克兰语
// 	"Greek",    	//希腊语
// 	"Tagalog",    	//菲律宾语
// 	"Latin",    	//拉丁语
// 	"Irish",    	//爱尔兰语
// 	"Hungarian",    //匈牙利语
// 	"Dutch",    	//荷兰语
// 	"Romana",		//罗马语
// 	"Suomi",		//芬兰语
// 	"Serbian",     	//塞尔维亚语
// 	"Croatia",		//克罗地亚语
// 	"Thai",    		//泰语
// 	"Arabia",    	//阿拉伯语
// 	"Persian",    	//波斯语
};


/*******************************
* 定义app_text.c所使用的内部变量
*******************************/
//默认语言为中文简体
static unsigned int uiCurrLanguage = LANG_CHINASIM;

//语言数据库结构指针，为open数据库文件的返回值
static sqlite3 * pLangDb = NULL;



/***
  * 功能：
        获取GUI当前的语言环境
  * 参数：
		无
  * 返回：
        成功返回 0
        失败返回 非0
  * 备注：
***/
int GetCurrLanguage(void)
{
	return pCurSystemSet->lCurrentLanguage;
}


/***
  * 功能：
        设置GUI当前的语言环境
  * 参数：
		unsigned int uiLangIndex :各国语言编码值
  * 返回：
        成功返回 0
        失败返回 非0
  * 备注：
***/
int SetCurrLanguage(unsigned int uiLangIndex)
{
	if (uiLangIndex >= LANG_TYPE)
	{
		return -1;
	}
	uiCurrLanguage = uiLangIndex;
	pCurSystemSet->lCurrentLanguage = uiLangIndex;

	SetCurrFont(getGlobalFnt(EN_FONT_WHITE));
	return 0;
}

/***
  * 功能：
        获得GUI当前的语言环境
  * 参数：
		无
  * 返回：
        总是成功返回 当前语言环境标志
  * 备注：
***/
int GetCurrLanguageSet(void)
{
	return uiCurrLanguage;
}

/***
  * 功能：
        获得GUI当前的语言文本
  * 参数：
		无
  * 返回：
        总是成功返回 当前语言环境文本
  * 备注：
***/

char* GetLanguageStrType(void)
{
    return strLangType[uiCurrLanguage];
}

/***
  * 功能：
        打开多国语数据库文件
  * 参数：
		char *pDbName :数据库文件名
  * 返回：
        成功返回 0
        失败返回 非0
  * 备注：
***/
int OpenLanguageDb(char *pDbName)
{
    int iErr = 0;

    iErr = sqlite3_open(pDbName, &pLangDb);
    if (SQLITE_OK != iErr)
    {
		LOG(LOG_ERROR, "*** SQLite3 open ERR%d\n", iErr);
	}

    return iErr;
}


/***
  * 功能：
        关闭多国语数据库文件
  * 参数：
		无
  * 返回：
        成功返回 0
        失败返回 非0
  * 备注：
***/
int CloseLanguageDb(void)
{
	int iErr;
	
	iErr = sqlite3_close(pLangDb);
	if (SQLITE_OK != iErr)
	{
		LOG(LOG_ERROR, "*** SQLite3 Close ERR%d\n", iErr);
		return iErr;
	}
	
	pLangDb = NULL;

	return 0;
}

/***
  * 功能：
        查找数据库的回调函数
  * 参数：
		//...
  * 返回：
        成功返回 0
        失败返回 非0
  * 备注：
***/
static int GetUtf8Word(void *pInpPara, int iFieldNumber, 
					   char **ppFieldValue, char **ppFieldName)
{
	int iLen;
	char *pBuffer;

	if (0 == iFieldNumber)
	{
		return -1;
	}

    if (NULL == ppFieldValue[0])
    {
        return -2;
    }
    
	iLen = strlen(ppFieldValue[0]);

	pBuffer = (char *)malloc(iLen + 1);
	if (NULL == pBuffer)
	{
		return -3;
	}
	
    strcpy(pBuffer, ppFieldValue[0]);

	*((int *)pInpPara) = (int)pBuffer;
   
    return 0;
}


/***
  * 功能：
        根据索引和语言种类查找数据库相应的文本字符串
  * 参数：
        1. int iIndx		:要转换的字符串在数据库中的索引
        2. char* strLanguage:要查找的语言种类
  * 返回：
        成功返回有效的utf8编码字符串指针
        失败返回NULL
  * 备注：
***/
static char* GetLanguageText(int iIndx, char *strLanguage)
{

	char SqlCmd[128] = {0};
    char *pErrMsg = NULL;
	int iErr;
	int data = 0;
	
	if (NULL == pLangDb)
	{
		return NULL;
	}

    sprintf(SqlCmd, "select %s from language where idx = '%d'", 
			strLanguage, iIndx);
    iErr = sqlite3_exec(pLangDb, SqlCmd, GetUtf8Word, (void *)&data, &pErrMsg);
    if (SQLITE_OK != iErr)
    {
		LOG(LOG_ERROR, "*** SQLite3 ERR%d: [%s]\n", iErr, pErrMsg);
	}
 
    return (char *)data;
}


/***
  * 功能：
        获取当前语言环境下文本字符串的usc2编码
  * 参数：
        1. int iIndx		:要转换的字符串在数据库中的索引
  * 返回：
        成功返回有效的ucs2编码字符串指针
        失败返回NULL
  * 备注：
***/
GUICHAR *GetCurrLanguageText(int iIndx)
{
	return GetTextUnicode(iIndx, strLangType[uiCurrLanguage]);
}


/***
  * 功能：
        获取当前语言环境下文本字符串的utf8编码
  * 参数：
        1. int iIndx		:要转换的字符串在数据库中的索引
  * 返回：
        成功返回有效的utf8编码字符串指针
        失败返回NULL
  * 备注：
***/
char *GetCurrLanguageUtf8Text(int iIndx, unsigned int language)
{
    char *utf8Text = GetLanguageText(iIndx, strLangType[language]);
    
    if(language == LANG_ARABIA || language == LANG_PERSIAN)
    {
        GUICHAR *usc2Text = GetUtf8ToUcs2(utf8Text);
        GUICHAR *usc2Text_Reshape = persianReshape(usc2Text, 1, 1);
        free(utf8Text);
        free(usc2Text);
        utf8Text = (char *)malloc(256);
        
        GetUcs2ToUtf8(usc2Text_Reshape, utf8Text);
        free(usc2Text_Reshape);
    }
    
	return utf8Text;
}

/***
  * 功能：
        Utf8到usc2的转换编码
  * 参数：
        1.char *pUtf8Code :utf-8编码的字符串
  * 返回：
        成功返回有效的ucs2编码字符串指针
        失败返回NULL
  * 备注：
		U-00000000 - U-0000007F: 0xxxxxxx 
		U-00000080 - U-000007FF: 110xxxxx 10xxxxxx
		U-00000800 - U-0000FFFF: 1110xxxx 110xxxxxx 10xxxxxx
***/
GUICHAR *GetUcs2FromUtf8(char *pUtf8Code)
{

	//临时变量定义
	int ilen = 0;
	GUICHAR *pUcs2 = NULL;
	GUICHAR *pRet = NULL;
 	char *pTmp = NULL;
	int index;

	//参数检测
	if (NULL == pUtf8Code)
	{
		return NULL;
	}
	
	//计算所包含的字符的长度
	for (pTmp = pUtf8Code, ilen = 0; *pTmp != 0; ++pTmp)
	{
		if ((0x00 == (*pTmp & 0x80)) ||
			(0xC0 == (*pTmp & 0xE0)) ||
			(0xE0 == (*pTmp & 0xF0)))
		{
			ilen ++;
		}
	}

	//为unicode分配空间
	pUcs2 = (GUICHAR*) calloc(sizeof(GUICHAR), ilen + 1);
	if (NULL == pUcs2)
	{
		return NULL;
	}

	//逐个转换utf8到usc2
	for(index = 0; index < ilen; ++index)
	{
		//1110xxxx 10xxxxxx 10xxxxxx
		if (0xE0 == (pUtf8Code[0] & 0xF0) &&
            0x80 == (pUtf8Code[1] & 0xC0) &&
            0x80 == (pUtf8Code[2] & 0xC0))
        {
            pUcs2[index] = (GUICHAR) (((pUtf8Code[0] & 0x0F) << 12) |
                             	      ((pUtf8Code[1] & 0x3F) << 6) |
                              	       (pUtf8Code[2] & 0x3F));
			pUtf8Code += 3;
        }
		//110xxxxx 10xxxxxx
		else if(0xC0 == (pUtf8Code[0] & 0xE0) &&
            	0x80 == (pUtf8Code[1] & 0xC0))
        {
            pUcs2[index] = (GUICHAR) (((pUtf8Code[0] & 0x1F) << 6) |
                              	       (pUtf8Code[1] & 0x3F));
			pUtf8Code += 2;
		}
		//0xxxxxxx
		else if(0x00 == (pUtf8Code[0] & 0x80))
		{
			pUcs2[index] = (GUICHAR) (pUtf8Code[0] & 0x7F);
			pUtf8Code += 1;
		}
		else
		{
			free(pUcs2);
			return NULL;
		}
	}
	pUcs2[index] = '\0';
    
	if (isThai())
    {
    	pRet = thaiLangProcess(pUcs2);
		free(pUcs2);
		return pRet;
    }
	
	//波斯语转换
	if (isArabic())
	{
		pRet = persianReshape(pUcs2, 1, 1);
		free(pUcs2);
		return pRet;
	}

	return pUcs2;
}


/***
  * 功能：
        获取多国语言文本字符串的usc2编码
  * 参数：
        1. int iIndx		:要转换的字符串在数据库中的索引
        2. char* strLanguage:要查找的语言种类
  * 返回：
        成功返回有效的ucs2编码字符串指针
        失败返回NULL
  * 备注：
***/
GUICHAR *GetTextUnicode(int iIndx, char *strLanguage)
{
	//临时变量定义
	char *pUtf8Code = NULL;
	GUICHAR *pUsc2 = NULL;

	//查找数据库中所对应的文本字符
	pUtf8Code = GetLanguageText(iIndx, strLanguage);
	if (NULL == pUtf8Code)
	{
		return NULL;
	}

	//返回utf8-usc2的转换结果
	pUsc2 = GetUcs2FromUtf8(pUtf8Code);

	
	free(pUtf8Code);

	return pUsc2;
}

