/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guimyglobal.h
* 摘    要：  声明一些公共函数
*             
*
* 当前版本：  v1.0.0
* 作    者：
* 完成日期：
*
* 取代版本：
* 原 作 者：
* 完成日期：
*******************************************************************************/


/*******************************************
* 为使用文件操作函数而需要引用的其他头文件
*******************************************/
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "wnd_global.h"
#include "guimyglobal.h"
#include "app_global.h"

unsigned char GucRegularString[] = {'\\', '/', ':', '*', '?', '<', '>', 
									'|', ';', '&', '`', '\'', '(', ')', '\"'};

/*********************************************************************************************************
** 函数名  :         RegularSeach
** 功  能  :         搜索指定的字符串是否含有指定的字符
** 输入参数:         pString : 待搜索的字符串
** 输出参数:         无
** 返回值  :         不包含指定的字符返回0，含有指定的字符返回-1
** 注      :         无
*********************************************************************************************************/
int RegularSeach(char *pString)
{
	int i = 0;

	/*if( (index(pString, ' ')==pString) ||
		(index(pString, '.')==pString) )
	{
		return -1;
	}*/

    if((NULL == pString)||(0 == strlen(pString)))
    {
        return -1;
    }
	
	for(i=0; i < sizeof(GucRegularString); i++)
	{
		char *pChar = NULL;
		pChar = index((const char *)pString, GucRegularString[i]);
		if( pChar !=NULL )
		{
			return -1;
		}
	}

	return 0;
}


/***
  * 功能：
        分配一块lSize字节的内存，成功返回指向内存的有效指针，否则返回NULL
  * 参数：
        1.long lSize:   要分配内存的大小，lSize不能小于1
  * 返回：
        成功返回指向内存的有效指针，否则返回NULL。
***/
/*
void * GuiMemAlloc1(long lSize)
{
    void * pMem;

    //参数检查
    if (lSize < 1)
    {
        return NULL;    //lSize不符合要求
    }

    //申请内存
    if (!(pMem = GuiMemAlloc(lSize)))
    {
        return NULL;    //动态分配失败
    }
    return pMem;
}

*/
/***
  * 功能：
        释放先前由GuiMemAlloc分配地内存，成功返回零
  * 参数：
        1.void * pMem:  指针，指向由GuiMemAlloc分配地内存
  * 返回：
        成功返回零，否则返回错误代码
  * 备注：
        函数GuiMemFree允许传入参数pMem为空指针，此时函数不做任何动作，直接返回零。
***/
/*
long GuiMemFree1(void * pMem)
{
    if (pMem)
    {
        GuiMemFree(pMem);                 //释放动态内存
    }

    return 0;
}
*/
/***
  * 功能：
        函数将src复制到des并返回des，如果des为NULL，函数会自动建立一个des
  * 参数：
        1.STRING des:目标字符串
        2.const STRING src:源字符串
  * 返回：
        成功返回目标字符串
  * 备注：

***/
/*
char *_usrstrcpy(char *des, const char *src)
{
    if (!src)
    {
        return des = NULL;
    }
    if (!des)
    {
        if (!(des = GuiMemAlloc(strlen(src) + 1)))//strlen得出的字符数不包括\0，所以要加1
        {
            return des;
        }
    }

    return strcpy(des, src);
}
*/
/*********************************************************************************************************
** 函数名  :         mySprintf1
** 功  能  :         混合英文和中文字符串为宽字符串
** 输入参数:         String1 :	英文字符串
**					 uiText  :	中英文字符数组
**					 uiIndex :  数组中的第几个元素
**					 pString :  中英文字符数组所指向的英文字符串数组
** 输出参数:         无
** 返回值  :         成功返回混合后的内存首地址
** 注      :         无
*********************************************************************************************************/
/*
GUICHAR *mySprintf1(char *String1, unsigned int uiText[][2], unsigned int uiIndex, char **pString)
{
	GUICHAR *pTmpBuf = NULL;
	if(String1 != NULL)
	{
		unsigned int uiLength = 0, uiSize1 = 0, uiSize2 = 0;
		GUICHAR *pBuf = NULL;
		GUICHAR *pBuf1 = NULL;
		char * pBuf2 = NULL;
		uiSize1 = strlen(String1);
		if(uiText[uiIndex][0] == 0)			//外部字库索引
		{
			unsigned int uiTmp = 0, i = 0;
			pTmpBuf = GetTextResource(uiText[uiIndex][1], lCurrentLanguage);
			uiSize2 = GetStringLength();
			uiTmp = uiSize2/2;
			uiLength = uiSize1 + 1 + uiTmp;
			if(!(pBuf = (GUICHAR *) GuiMemAlloc(uiLength * sizeof(GUICHAR))))
			{
				return NULL;
			}

			for(i=0; i<uiSize1; i++)
			{
				pBuf[i] = String1[i];
			}
			for(i=uiSize1; i<(uiTmp+uiSize1); i++)
			{
				pBuf[i] = pTmpBuf[i-uiSize1];
			}
			pBuf[i] = 0;
			GuiMemFree(pTmpBuf);
			return pBuf;
		}
		else
		{
			uiSize2 = strlen(pString[ uiText[uiIndex][1] ]);
			uiLength = uiSize1 + 1 + uiSize2;
			if(!(pBuf2 = (char *) GuiMemAlloc(uiLength * sizeof(char))))
			{
				return NULL;
			}

			strcpy(pBuf2, String1);
			strcat(pBuf2, pString[ uiText[uiIndex][1] ]);
			pBuf2[uiLength] = 0;
			pBuf1 = TransString(pBuf2);
			GuiMemFree(pBuf2);
			return pBuf1;
		}
	}
	else
	{
		return NULL;
	}
	
	return NULL;
}
*/
/*********************************************************************************************************
** 函数名  :         GetTotalLength
** 功  能  :         返回混合后总的长度，以字节为单位
** 输入参数:         无
** 输出参数:         无
** 返回值  :         返回混合后总的长度，以字节为单位
** 注      :         无
*********************************************************************************************************/
/*
unsigned int GetTotalLength(void)
{
	int i = 0;
	i++;
	return GuiStringLength;
}
*/
/*********************************************************************************************************
** 函数名  :         GetDropListTextResource
** 功  能  :         得到下拉列表菜单项的资源文本
** 输入参数:         ucIndex[][2]	:	文本资源数组
**					 pString		:	文本资源数组所指向的英文字符串数组
** 输出参数:         无
** 返回值  :         返回资源文本的地址
** 注      :         无
*********************************************************************************************************/
/*
GUICHAR *GetDropListTextResource(unsigned int uiText[][2], unsigned int uiIndex, char **pString)
{
	if(uiText[uiIndex][0] == 0)		//外部字库
	{
		return GetTextResource(uiText[uiIndex][1], lCurrentLanguage);
	}
	else							//内部英文字符串数组
	{
		if(NULL != pString)
		{
			return TransString(pString[ uiText[uiIndex][1] ]);
		}
		else
		{
			return NULL;
		}	
	}
	
	return NULL;
}
*/
/***
  * 功能：
        实现对popen函数的二次封装
  * 参数：
        1.char *pCmd:	需要执行的命令行
  * 返回：
        成功返回零，失败返回非零值
  * 备注：
***/
int mysystem(char *pCmd)
{
	int iReturn = 0;
#if 0
	SendMsg(pCmd);
	iReturn = RecMsg();
	return iReturn;
#else

	int iStatus = 0;
	FILE *Fp = NULL;
	char cResultBuff[1024];

	if(NULL == pCmd)
	{
		LOG(LOG_ERROR, "pCmd is NULL\n");
		return -1;
	}

	/* 执行命令 */
	Fp = popen(pCmd, "r");
	/* 命令执行失败 */
	if(NULL == Fp)
	{
		LOG(LOG_ERROR, "popen error: %s\n", strerror(errno));
		return -1;
	}

	LOG(LOG_INFO, "%s\n", pCmd);

	/* 打印命令执行结果 */
    while(fgets(cResultBuff, sizeof(cResultBuff), Fp) != NULL)
    {
		LOG(LOG_INFO, "%s", cResultBuff);
	}	

    /*等待命令执行完毕并关闭管道及文件指针*/
    iStatus = pclose(Fp);
    if(-1 == iStatus)
    {
		LOG(LOG_ERROR, "pclose fail!\n");
		return -2;
    }
    else
    {
		LOG(LOG_INFO, "WEXITSTATUS(iStatus) = %d\n", WEXITSTATUS(iStatus));
		iReturn = WEXITSTATUS(iStatus);
    }	

	return iReturn;

#endif
}


