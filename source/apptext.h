#ifndef _APP_TEXT_H_
#define _APP_TEXT_H_
#include "guibase.h"

#define CHARLEN 	72		//每个字符的点阵所占的字节数
#define FONTWIDTH 	24		//字体的高度
#define FONTHEIGHT 	24		//字体的高度

#define FONE_TYPE_SIZE    	6		//支持的字体的种类的数目

//定义文本字号
#define FONT_SIZE_SMALL    9       // 15号
#define FONT_SIZE_MIDDLE   10       // 15号
#define FONT_SIZE_BIG      11       // 19号
//定义文本字形
#define FONT_STYLE_NORMAL   1       //常规字形
#define FONT_STYLE_BOLD     2       //粗体字形


/*
  定义GUI所使用的字体设置
*/
#define FONT_MAX_WIDTH      32
#define FONT_MAX_HEIGHT     32


//对应.fnt的文件头结构
typedef struct _fnt_fileheader
{
    unsigned long lFileType;         //文件类型，值0x544e462e,即".FNT"
    unsigned long lVersion;          //文件版本，值0x30303156,即"V100"
    unsigned long lFileSize;         //文件大小，整个字库文件的字节数
    unsigned long lCodeOffset;       //偏移量，文件头道字段编码区的字节数
    unsigned long lDataOffset;       //偏移量，文件头到点阵数据区的字节数
}__attribute__((packed))APPFNT_FILEHEADER;

//对应.fnt的字段映射区
typedef struct _fnt_mapping
{
    unsigned long lCodeBytes;    //当前字段编码所占的总字节数
    unsigned long lCodeOffset;   //偏移量，从编码区到当前字段编码区的字符数
}__attribute__((packed))APPFNT_MAPPING;

//对应.fnt的点阵数据区
typedef struct _fnt_datamap
{
    unsigned char bBlockBoxX;			//点阵有效宽度
    unsigned char bBlockBoxY;			//点阵有效高度
    unsigned char bCellIncX;			//点阵显示宽度
    unsigned char bCellIncY;			//点阵显示高度
    unsigned char pFontData[CHARLEN];	//点阵数据
}__attribute__((packed))APPFNT_DATAMAP;

//定义文本索引
//#include "Global_Index.h"
//定义文本语言类型
#define TXT_LANG_ENGLISH        0x00    //英语
#define TXT_LANG_CHINASIM       0x01    //汉语简体
#define TXT_LANG_CHINATRA       0x02    //汉语繁体
#define TXT_LANG_KOREAN         0x03    //韩语
#define TXT_LANG_JAPANESE       0x04    //日语
#define TXT_LANG_RUSSIAN        0x05    //俄语
#define TXT_LANG_GERMAN         0x06    //德语
#define TXT_LANG_FRANCE         0x07    //法语
#define TXT_LANG_VIETNAM        0x08    //越南语
#define TXT_LANG_SPANISH        0x09    //西班牙语
#define TXT_LANG_TURKISH        0x0A    //土耳其语
#define TXT_LANG_ITALY          0x0B    //意大利语
#define TXT_LANG_PORTUGAL       0x0C    //葡萄牙语
#define TXT_LANG_HINDI          0x0D    //印地语
#define TXT_LANG_SWIDISH        0x0E    //瑞典语
#define TXT_LANG_POLISH         0x0F    //波兰语
#define TXT_LANG_UYGHUR         0x10    //维吾尔语
#define TXT_LANG_BENGALI        0x11    //孟加拉语
#define TXT_LANG_DANISH         0x12    //丹麦语
#define TXT_LANG_CZECH          0x13    //捷克语
#define TXT_LANG_UKRAINIAN      0x14    //乌克兰语
#define TXT_LANG_GREEK          0x15    //希腊语
#define TXT_LANG_TAGALOG        0x16    //菲律宾语
#define TXT_LANG_LATIN          0x17    //拉丁语
#define TXT_LANG_IRISH          0x18    //爱尔兰语
#define TXT_LANG_HUNGARIAN      0x19    //匈牙利语
#define TXT_LANG_DUTCH          0x1A    //荷兰语
#define TXT_LANG_ROMANA         0x1B	//罗马语
#define TXT_LANG_SUOMI		    0x1C	//芬兰语
#define TXT_LANG_SERBIAN	    0x1D     //塞尔维亚语
#define TXT_LANG_CROATIA	    0X1E	//克罗地亚语
#define TXT_LANG_THAI           0x1F    //泰语
#define TXT_LANG_ARABIA         0x20    //阿拉伯语
#define TXT_LANG_PERSIAN        0x21    //波斯语
#define TXT_LANG_GLOBAL         0xFF    //全球版

//定义文本配置文件路径
#define EXTXT_FILE_ENGLISH        "English.fnt"
#define EXTXT_FILE_CHINASIM       "ChinaSim.fnt"
#define EXTXT_FILE_CHINATRA       "ChinaTra.fnt"
#define EXTXT_FILE_KOREAN         "Korea.fnt"
#define EXTXT_FILE_JAPANESE       "Japanese.fnt"
#define EXTXT_FILE_SPANISH        "Spanish.fnt"
#define EXTXT_FILE_RUSSIAN        "Russian.fnt"
#define EXTXT_FILE_THAI           "Thai.fnt"
#define EXTXT_FILE_GERMAN         "German.fnt"
#define EXTXT_FILE_FRANCE         "France.fnt"
#define EXTXT_FILE_ITALY          "Italy.fnt"
#define EXTXT_FILE_ARABIA         "Arabia.fnt"
#define EXTXT_FILE_PORTUGAL       "Portugal.fnt"
#define EXTXT_FILE_HINDI          "Hindi.fnt"
#define EXTXT_FILE_TURKISH        "Turkish.fnt"
#define EXTXT_FILE_VIETNAM        "Vietnam.fnt"
#define EXTXT_FILE_SWIDISH        "Swedish.fnt"
#define EXTXT_FILE_POLISH         "Polish.fnt"
#define EXTXT_FILE_UYGHUR         "Uyghur.fnt"
#define EXTXT_FILE_BENGALI        "Bengali.fnt"
#define EXTXT_FILE_DANISH         "Danish.fnt"
#define EXTXT_FILE_CZECH          "Czech.fnt"
#define EXTXT_FILE_UKRAINIAN      "Ukrainian.fnt"
#define EXTXT_FILE_GREEK          "Greek.fnt"
#define EXTXT_FILE_TAGALOG        "Tagalog.fnt"
#define EXTXT_FILE_LATIN          "Latin.fnt"
#define EXTXT_FILE_IRISH          "Irish.fnt"
#define EXTXT_FILE_PERSIAN        "Persian.fnt"
#define EXTXT_FILE_HUNGARIAN      "Hungarian.fnt"
#define EXTXT_FILE_DUTCH          "Dutch.fnt"
#define EXTXT_FILE_ROMANA		  "Romana.fnt"
#define EXTXT_FILE_SUOMI		  "Suomi.fnt"
#define EXTXT_FILE_SERBIAN        "Serbian.fnt"
#define EXTXT_FILE_CROATIA		  "Croatia.fnt"

//根据文本语言切换，同步处理字体及点阵
int SyncLanguageChange(unsigned long lLanguage);

//根据文本索引及文本语言，读取相应的文本配置文件，以得到该文本，成功返回有效指针
GUICHAR *GetTextResource(unsigned long lIndex, unsigned long lLanguage);


unsigned int GetStringLength(void);

#endif  //apptext.h

