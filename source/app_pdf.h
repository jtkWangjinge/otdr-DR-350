/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_pdf.h
* 摘    要：  实现生成PDF报告的底层函数封装
*
* 当前版本：  v1.0.0  
* 作    者：  
* 完成日期：
*
* 取代版本： 
* 作    者：  
* 完成日期：  
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "epdf.h"

#define COLOR_RGB(color) color/255.0f

typedef enum __Pdf_Text_Horizontal_Alignment
{
    PDF_TEXT_ALIGN_LEFT = 0x00,
    PDF_TEXT_ALIGN_RIGHT,
    PDF_TEXT_ALIGN_H_CENTER,
    PDF_TEXT_ALIGN_JUSTIFY
}PDF_TEXT_HORIZONTAL_ALIGNMENT;

typedef enum __Pdf_Text_Vertical_Alignment
{
    PDF_TEXT_ALIGN_TOP = 0x10,
    PDF_TEXT_ALIGN_BOTTOM = 0x20,
    PDF_TEXT_ALIGN_V_CENTER = 0x30,
}PDF_TEXT_VERTICAL_ALIGNMENT;

typedef enum __PdfTableAlignment
{
    PDF_TABLE_ALIGN_LEFT = 0,
    PDF_TABLE_ALIGN_RIGHT,
    PDF_TABLE_ALIGN_CENTER,
}PDFTABLEALIGNMENT;

//pdf绘制控制类
typedef struct __pdfcontrol
{
    float pageWidth;                //页宽
    float pageHeight;               //页高
    float currPosition;             //当前位置
    float lineSpace;                //行间距
    float paragraphSpace;           //段间距
    float pageMarginsWidth;         //左右页边距
    float pageMarginsHeight;        //上下页边距
    float pageEditWidth;            //编辑区的宽度
    char *font;
    char fileName[512];
    unsigned int pdfDoc;
    unsigned int page;
}PdfControl;

//pdf绘制对象
typedef struct __pdfPainter
{
    float position;
    unsigned int page;
}PdfPainter;

//表格的单元格
typedef struct __pdftableItem
{
    unsigned int rowNum;
    unsigned int columnNum;
    unsigned int rowCount;                       //单元格所占行数，默认为1
    unsigned int columnCount;                    //单元格所占列数，默认为1
    float cellSize;
    unsigned int backgroundColor;                //单元格背景色，低3字节有效，分别为RGB
    unsigned int textColor;                      //文本颜色，低3字节有效，分别为RGB
    unsigned int isValid;
    char *cellFont;
    char *text;
    float cellWidth;
    PDF_TEXT_HORIZONTAL_ALIGNMENT cellHorizontalAlignment;
    PDF_TEXT_VERTICAL_ALIGNMENT cellVerticalAlignment;
    EPDF_TextStyle cellStyle;
}PdfTableItem;

//表格的一行
typedef struct __pdftablerow
{
    float height;
    int adaptiveHeight;
    PdfTableItem *items;
}PdfTableRow;

//表格
typedef struct __pdftable
{
    unsigned int rowCount;
    unsigned int columnCount;
    unsigned int isShowLine;
    float tableWidth;
    unsigned int lineColor;                //表格线颜色，低3字节有效，分别为RGB
    PDFTABLEALIGNMENT tableAlignment;
    PdfTableRow *rows;
}PdfTable;

//显示文本
void pdfDrawText(unsigned int page, float x, float y, float width, float height, float fontSize, EPDF_TextStyle style,
    PDF_TEXT_HORIZONTAL_ALIGNMENT alignment, char *language, char *text);

//画线
void pdfDrawLine(unsigned int page, float startX, float startY, float endX, float endY);

//画图片
void pdfDrawImage(unsigned int page, char *filepath, float x, float y, float width, float height);

//创建一个pdf绘制控制对象
PdfControl *createPdfControl(char *pdfName);

//申请一片绘制区域
PdfPainter pdfGetPainter(PdfControl *pdfControl, float height);

//设置段间距
void pdfSetParagraphSpace(PdfControl *pdfControl, float paragraphSpace);

//设置字体
void pdfSetFont(PdfControl *pdfControl, char *font);

//设置左右页边距
void pdfSetPageMarginsWidth(PdfControl *pdfControl, float pageMargins);

//设置段间距
void pdfSetPageMarginsHeight(PdfControl *pdfControl, float pageMargins);

//绘制结束，保存文件，回收内存
void pdfDrawTheEnd(PdfControl *pdfControl);

//绘制段间距
void pdfDrawParagraphSpace(PdfControl *pdfControl);

//绘制指定高度的空白间隔
void pdfDrawSpace(PdfControl *pdfControl, float height);

//绘制一个标题
void pdfTitleDraw(PdfControl *pdfControl, float offset, char *text, float lineWidth,
    char *font, float size, EPDF_TextStyle style, float colorR, float colorG, float colorB);

//绘制带下划线的文本 下划线宽度为0时不画下划线
void pdfTextWithUnderlineDraw(PdfControl *pdfControl, float offset, char *text, 
    float lineWidth, char *font, float size, EPDF_TextStyle style);

//绘制文本后带线 xxxx:____________________
void pdfTextWithWriteDraw(PdfControl *pdfControl, float offset, char *text, 
    float lineWidth, float length, char *font, float size, EPDF_TextStyle style);

//创建一个表格
PdfTable * pdfTableCreate(unsigned int rowCount, unsigned int columnCount, float width);

//为表格设置显示文本
int pdfTableSetText(PdfTable *table, unsigned int row, unsigned int column, char *text);

//设置单元格文本颜色
int pdfTableSetTextColor(PdfTable *table,
    unsigned int startRow, unsigned int startColumn,
    unsigned int endRow, unsigned int endColumn,
    unsigned int colorR, unsigned int colorG, unsigned int colorB);

//修改列宽 宽度为占总宽度的百分比
int pdfTableSetColumnWidth(PdfTable *table, unsigned int column, float width);

//修改表格指定行高度
int pdfTableSetRowHeight(PdfTable *table, unsigned int startRow, unsigned int endRow, float height);

//设置指定行高是否自适应
int pdfTableSetRowHeightAdaptive(PdfTable *table, unsigned int startRow, unsigned int endRow, int isAdaptive);

//设置是否画表格线
int pdfTableSetLineEnable(PdfTable *table, unsigned int enable);

//设置表格线颜色
int pdfTableSetLineColor(PdfTable *table, unsigned int colorR, unsigned int colorG, unsigned int colorB);

//设置表格对齐方式
int pdfTableSetAlignment(PdfTable *table, PDFTABLEALIGNMENT alignment);

//设置指定单元格的字体和大小
int pdfTableSetItemFontAndSize(PdfTable *table, unsigned int startRow, unsigned int startColumn, 
                                                unsigned int endRow, unsigned int endColumn,
                                                char *font, float size, EPDF_TextStyle style);

//设置指定单元格的对齐方式
int pdfTableSetItemAlignment(PdfTable *table,unsigned int startRow, unsigned int startColumn,
                                unsigned int endRow, unsigned int endColumn,
                                PDF_TEXT_HORIZONTAL_ALIGNMENT Halignment,
                                PDF_TEXT_VERTICAL_ALIGNMENT Valignment);
                                                
//设置指定单元格的背景色
int pdfTableSetItemBackgroundColor(PdfTable *table, unsigned int startRow, unsigned int startColumn,
                                      unsigned int endRow, unsigned int endColumn,unsigned int colorR, 
                                      unsigned int colorG, unsigned int colorB);

//设置合并单元格
int pdfTableMergeCells(PdfTable *table, unsigned int startRow, unsigned int startColumn,
    unsigned int endRow, unsigned int endColumn);

//绘制表格
void pdfTableDraw(PdfControl *pdfControl, PdfTable *table);

//回收表格资源
void pdfTableFree(PdfTable *table);

//绘制一张图片
void pdfPictureDraw(PdfControl *pdfControl, char *filePath, float width, float height, float offset);

//绘制一条分隔线
void pdfSeparatorDraw(PdfControl *pdfControl, float width, float length, 
    unsigned int colorR, unsigned int colorG, unsigned int colorB);

//绘制页眉信息
void pdfHeaderDraw(unsigned int page, float OffsetFromTheLeft, float OffsetFromTheTop,
    char *text, char *font, float size, EPDF_TextStyle style, 
    unsigned int fontColorR, unsigned int fontColorG, unsigned int fontColorB);

//绘制页脚信息
void pdfFooterDraw(unsigned int page, float OffsetFromTheLeft, float OffsetFromTheBottom,
    char *text, char *font, float size, EPDF_TextStyle style,
    unsigned int fontColorR, unsigned int fontColorG, unsigned int fontColorB);

//得到语言
char *pdfReportGetLanguage(PdfControl *pControl, unsigned int currLanguage);

#ifdef __cplusplus
}
#endif