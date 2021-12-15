#include "apptext.h"
#include "guibase.h"
#include "guimyglobal.h"
#include "wnd_global.h"
#include "app_global.h"

/*******************************************
* 为使用文件操作函数而需要引用的其他头文件
*******************************************/
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

static unsigned int GuiStringLength = 0;

char cLanguageType[34] =  {TXT_LANG_ENGLISH, TXT_LANG_CHINASIM, TXT_LANG_CHINATRA, TXT_LANG_KOREAN,
	                       TXT_LANG_JAPANESE, TXT_LANG_RUSSIAN, TXT_LANG_GERMAN, TXT_LANG_FRANCE,
	                       TXT_LANG_VIETNAM, TXT_LANG_SPANISH, TXT_LANG_TURKISH, TXT_LANG_ITALY,
	                       TXT_LANG_PORTUGAL, TXT_LANG_HINDI, TXT_LANG_SWIDISH, TXT_LANG_POLISH,
	                       TXT_LANG_UYGHUR, TXT_LANG_BENGALI, TXT_LANG_DANISH, TXT_LANG_CZECH,
	                       TXT_LANG_UKRAINIAN, TXT_LANG_GREEK, TXT_LANG_TAGALOG, TXT_LANG_LATIN,
	                       TXT_LANG_IRISH, TXT_LANG_HUNGARIAN, TXT_LANG_DUTCH, TXT_LANG_ROMANA,
	                       TXT_LANG_SUOMI, TXT_LANG_SERBIAN, TXT_LANG_CROATIA, TXT_LANG_THAI,
	                       TXT_LANG_ARABIA, TXT_LANG_PERSIAN};

char * cLanguagePath[34] = {EXTXT_FILE_ENGLISH, EXTXT_FILE_CHINASIM, EXTXT_FILE_CHINATRA, EXTXT_FILE_KOREAN,
                            EXTXT_FILE_JAPANESE, EXTXT_FILE_SPANISH, EXTXT_FILE_RUSSIAN, EXTXT_FILE_THAI,
                            EXTXT_FILE_GERMAN, EXTXT_FILE_FRANCE, EXTXT_FILE_ITALY, EXTXT_FILE_ARABIA,
                            EXTXT_FILE_PORTUGAL, EXTXT_FILE_HINDI, EXTXT_FILE_TURKISH, EXTXT_FILE_VIETNAM,
                            EXTXT_FILE_SWIDISH, EXTXT_FILE_POLISH, EXTXT_FILE_UYGHUR, EXTXT_FILE_BENGALI,
                            EXTXT_FILE_DANISH, EXTXT_FILE_CZECH, EXTXT_FILE_UKRAINIAN, EXTXT_FILE_GREEK, 
                            EXTXT_FILE_TAGALOG, EXTXT_FILE_LATIN, EXTXT_FILE_IRISH, EXTXT_FILE_PERSIAN,
                            EXTXT_FILE_HUNGARIAN, EXTXT_FILE_DUTCH, EXTXT_FILE_ROMANA, EXTXT_FILE_SUOMI,
                            EXTXT_FILE_SERBIAN, EXTXT_FILE_CROATIA};


//根据文本语言切换，同步处理字体及点阵
int SyncLanguageChange(unsigned long lLanguage)
{
	int iIndex = 0;
    char cFontPath[6][50];
    int fd;
    APPFNT_FILEHEADER fh;
    unsigned long count[FONE_TYPE_SIZE];

    for (iIndex = 0; iIndex < 34; iIndex++)
    {
        if (lLanguage == cLanguageType[iIndex])
        {
            sprintf(cFontPath[0], "%ssmall/normal/%s", FontFileDirectory, cLanguagePath[iIndex]);
            sprintf(cFontPath[1], "%ssmall/bold/%s", FontFileDirectory, cLanguagePath[iIndex]);
            sprintf(cFontPath[2], "%smiddle/normal/%s", FontFileDirectory, cLanguagePath[iIndex]);
            sprintf(cFontPath[3], "%smiddle/bold/%s", FontFileDirectory, cLanguagePath[iIndex]);			
            sprintf(cFontPath[4], "%sbig/normal/%s", FontFileDirectory, cLanguagePath[iIndex]);
            sprintf(cFontPath[5], "%sbig/bold/%s", FontFileDirectory, cLanguagePath[iIndex]);
            break;
        }
    }	

	for (iIndex = 0; iIndex < FONE_TYPE_SIZE; iIndex++)
    {
    	//打开配置文件并检查其格式
	    if ((fd = open(cFontPath[iIndex], O_RDONLY)) == -1)
	    {
	        return 1;
	    }
	    if (read(fd, &fh, 20) != 20)
	    {
	        close(fd);
	        return 1;
	    }
	    if (fh.lFileType != 0x544e462e || fh.lVersion != 0x30303156)
	    {
	        return 1;
	    }
	    close(fd);
	    count[iIndex] = (fh.lFileSize - fh.lDataOffset)/CHARLEN;
	}



    //删除之前语言字库
    #if 0
    if(GetMatrixObject("_FNT_SMALL_NORMAL"))
    {
        DeleteMatrixDirect(GetMatrixObject("_FNT_SAMLL_NORMAL"));
    }
    if(GetMatrixObject("_FNT_SMALL_BOLD"))
    {
        DeleteMatrixDirect(GetMatrixObject("_FNT_SMALL_BOLD"));
    }	
    if(GetMatrixObject("_FNT_MIDDLE_NORMAL"))
    {
        DeleteMatrixDirect(GetMatrixObject("_FNT_MIDDLE_NORMAL"));
    }
    if(GetMatrixObject("_FNT_MIDDLE_BOLD"))
    {
        DeleteMatrixDirect(GetMatrixObject("_FNT_MIDDLE_BOLD"));
    }	
	if(GetMatrixObject("_FNT_BIG_NORMAL"))
    {
        DeleteMatrixDirect(GetMatrixObject("_FNT_BIG_NORMAL"));
    }
	if(GetMatrixObject("_FNT_BIG_BOLD"))
    {
        DeleteMatrixDirect(GetMatrixObject("_FNT_BIG_BOLD"));
    }	
	
    //创建新的语言字库文件
    CreateMatrixDirectEX(FONTWIDTH, FONTHEIGHT, count[0], cFontPath[0], "_FNT_SMALL_NORMAL");	//放入字体链表中
    CreateMatrixDirectEX(FONTWIDTH, FONTHEIGHT, count[1], cFontPath[1], "_FNT_SMALL_BOLD");		//放入字体链表中
    CreateMatrixDirectEX(FONTWIDTH, FONTHEIGHT, count[2], cFontPath[2], "_FNT_MIDDLE_NORMAL");	//放入字体链表中
    CreateMatrixDirectEX(FONTWIDTH, FONTHEIGHT, count[3], cFontPath[3], "_FNT_MIDDLE_BOLD");	//放入字体链表中    
	CreateMatrixDirectEX(FONTWIDTH, FONTHEIGHT, count[4], cFontPath[4], "_FNT_BIG_NORMAL");		//放入字体链表中
	CreateMatrixDirectEX(FONTWIDTH, FONTHEIGHT, count[5], cFontPath[5], "_FNT_BIG_BOLD");		//放入字体链表中
    #endif
	
    return 0;
}

//根据文本索引及文本语言，读取相应的文本配置文件，以得到该文本，成功返回有效指针
GUICHAR *GetTextResource(unsigned long lIndex, unsigned long lLanguage)
{
	int iIndex = 0;
    APPFNT_FILEHEADER fh;
    APPFNT_MAPPING map;
    char cFontPath[1][50];
    unsigned short *pBuf;
    int fd;

    for (iIndex = 0; iIndex < 34; iIndex++)
    {
        if (lLanguage == cLanguageType[iIndex])
        {
            sprintf(cFontPath[0], "%ssmall/normal/%s", FontFileDirectory, cLanguagePath[iIndex]);
            break;
        }
    }	


    //打开配置文件并检查其格式
    if ((fd = open(cFontPath[0], O_RDONLY)) == -1)
    {
        return NULL;
    }
    if (read(fd, &fh, 20) != 20)
    {
        close(fd);
        return NULL;
    }
    if (fh.lFileType != 0x544e462e || fh.lVersion != 0x30303156)
    {
        close(fd);
        return NULL;
    }

    //读取文件映射数据
    lseek(fd, lIndex*8, SEEK_CUR);
    if(read(fd, &map, 8) !=8)	//根据index，读取该字段的字段映射表
    {
        close(fd);
        return NULL;
    }
	
	GuiStringLength = map.lCodeBytes;
		
    //根据找到的文本映射来读取文本内容
    if (!(pBuf = GuiMemAlloc(map.lCodeBytes + 2)))//宏定义的字段不包括\0，所以申请内存时加上\0
    {
        close(fd);
        return NULL;
    }
    lseek(fd, fh.lCodeOffset + map.lCodeOffset*2, SEEK_SET);//移动到编码区开始的位置
    if (read(fd, pBuf, map.lCodeBytes) != map.lCodeBytes)	//根据字段映射表，读取该字段的字段编码表
    {
        GuiMemFree(pBuf);
        close(fd);
        return NULL;
    }
    pBuf[map.lCodeBytes >> 1] = 0;	//由于pBuf是unsigned short型，所以除以2是这个数组的最后一个索引；
    								//字段编码表的结尾加上\0

    //建立字符串对象
    //pTxt = CreateStringDirect(pBuf);//建立GUISTRING的类型
    //GuiMemFree(pBuf);
    close(fd);

    return pBuf;
}

 
unsigned int GetStringLength(void)
{
	return GuiStringLength;
}

