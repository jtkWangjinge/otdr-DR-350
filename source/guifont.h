/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  guifont.h
* 摘    要：  定义GUI的字体类型及操作，为GUI绘图模块的实现提供基础。
*
* 当前版本：  v1.1.0 (新增资源环境配置文件及设备抽象层文件)
* 作    者：  
* 完成日期：  
*******************************************************************************/

#ifndef _GUI_FONT_H
#define _GUI_FONT_H


/************************************
* 为定义GUIFONT而需要引用的其他头文件
************************************/
#include "guibase.h"


/*********************************
* 定义GUI中与字体文件格式相关的类型
*********************************/
//GUI中字体文件类型
#define GUIFONT_FILE_STD    1   //标准字体文件，只含点阵数据，点阵按码值顺序排列
#define GUIFONT_FILE_MIX    2   //混合字体文件，含非点阵数据，点阵按码值顺序排列

//对应.fnt的文件头结构
typedef struct _font_fileheader
{
    UINT32 uiFileType;      //文件类型，0x2E464E54('.FNT')为文件中存储序列，小端
    UINT32 uiFileSize;      //文件大小，文件占用的字节大小
    UINT32 uiFileFormat;    //文件格式，1标准、2混合，目前只支持标准格式
    UINT16 usSegmentFlag;   //分段标记，编码是否分段(0不分段)，目前只支持不分段
    UINT16 usSegmentCount;  //分段计数，包含点阵段的数量，分段标记为1时有效
    UINT32 uiMatrixWidth;   //点阵宽度，单个点阵的宽度
    UINT32 uiMatrixHeight;  //点阵高度，单个点阵的高度
    UINT32 uiMatrixCount;   //点阵计数，实际包含字体点阵的数量
    UINT32 uiDataOffset;    //偏移量，文件头到点阵数据的偏移量，以字节为单位
} GUIFONT_FILEHEADER;

//对应.fnt的分段映射结构
typedef struct _font_segmentmap
{
    UINT32 uiStartCode;     //点阵段对应的起始编码
    UINT32 uiCodeCount;     //点阵段包含编码的数量
    UINT32 uiSegmentOffset; //文件头到点阵数据段的偏移，以字节为单位
    UINT32 uiReserved;      //保留值
} GUIFONT_SEGMENTMAP;


/******************
* 定义GUI中字体点阵
******************/
typedef struct _font_matrix
{
    unsigned int uiRealWidth;   //点阵数据可用于显示的实际宽度
    unsigned int uiRealHeight;  //点阵数据可用于显示的实际高度

    unsigned char *pMatrixData; //点阵数据，其每行数据存储时都按8位对齐并靠左
} GUIFONT_MATRIX;


/******************
* 定义GUI中字体结构
******************/
typedef struct _font
{
    char *strFontFile;          //字体文件名称
    int iFontFd;                //字体文件描述符

    unsigned int uiFontWidth;   //字体宽度，须与.fnt中一致，目前不支持缩放
    unsigned int uiFontHeight;  //字体高度，须与.fnt中一致，目前不支持缩放

    unsigned int uiLeftGap;     //字体左间距，其值<=字体宽度/2
    unsigned int uiRightGap;    //字体右间距，其值<=字体宽度/2
    unsigned int uiTopGap;      //字体上间距，其值<=字体高度/2
    unsigned int uiBottomGap;   //字体下间距，其值<=字体高度/2

    unsigned int uiFgColor;     //字体前景色，即字体颜色
    unsigned int uiBgColor;     //字体背景色
} GUIFONT;


/**********************************
* 定义GUI中与字体类型相关的操作函数
**********************************/
//根据指定的信息直接建立字体对象
GUIFONT* CreateFont(char *strFontFile, 
                    unsigned int uiFontWidth, unsigned int uiFontHeight, 
                    unsigned int uiFgColor, unsigned int uiBgColor);
//删除字体对象
int DestroyFont(GUIFONT **ppFontObj);

//得到当前的字体对象
GUIFONT* GetCurrFont(void);
//设置当前的字体对象
int SetCurrFont(GUIFONT *pFontObj);

//设置字体的间距
int SetFontGap(unsigned int uiLeftGap, unsigned int uiRightGap, 
               unsigned int uiTopGap, unsigned int uiBottomGap, 
               GUIFONT *pFontObj);
//设置字体的前背景颜色
int SetFontColor(unsigned int uiFgColor, unsigned int uiBgColor, 
                 GUIFONT *pFontObj);

//从指定字体对象中提取特定编码的字体点阵
GUIFONT_MATRIX* ExtractFontMatrix(unsigned short usMatrixCode, 
                                  GUIFONT *pFontObj);

GUIFONT_MATRIX* ExtractFontMatrixJoin(unsigned short *usMatrixCode, 
                                  GUIFONT *pFontObj, int len, int isRight);


#endif  //_GUI_FONT_H

