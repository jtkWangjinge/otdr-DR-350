/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_pdf.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "app_pdf.h"
#include "app_text.h"
#include "app_global.h"

float g_pageMarginsWidth = 90.14;
float g_pageMarginsHeight = 50;//72

#define PAGEWIDTH_A4    595.276
#define PAGEHEIGHT_A4   841.89

static int pdfCreatePage(PdfControl *pdfControl);
/**********************************
*             坐标转换             *
备注：pdf文档中坐标起点在左下角，将起点转换
到左上角便于理解;输入为以左上为起点且除去
页边距宽度的坐标
**********************************/
//X方向 
float coordTransformX(float coordX)
{
    return (coordX + g_pageMarginsWidth);
}
//Y方向 
float coordTransformY(float coordY)
{
    return (PAGEHEIGHT_A4 - g_pageMarginsHeight - coordY);
}

/**********************************
*             显示文本            *
**********************************/
void pdfDrawText(unsigned int page, float x, float y, float width, float height, float fontSize, EPDF_TextStyle style,
    PDF_TEXT_HORIZONTAL_ALIGNMENT alignment, char *language, char *text)
{
    if (text && language)
    {
        epdf_textstyle(page, style);
        epdf_textrect(page, coordTransformX(x), coordTransformY(y), coordTransformX(x + width), coordTransformY(y + height),
                        fontSize, language, text, (EPDF_TextAlignment)alignment);
    }
}

/**********************************
*               画线              *
**********************************/
void pdfDrawLine(unsigned int page, float startX, float startY, float endX, float endY)
{
    epdf_moveto(page, coordTransformX(startX), coordTransformY(startY));
    epdf_lineto(page, coordTransformX(endX), coordTransformY(endY));
    epdf_stroke(page);
}

/**********************************
*              画矩形             *
**********************************/
void pdfDrawRect(unsigned int page, float startX, float startY, float width, float height, int isFill)
{
}

/**********************************
*              画图像             *
**********************************/
void pdfDrawImage(unsigned int page, char *filepath, float x, float y, float width, float height)
{
    if (filepath)
    {
        epdf_imageshow(page, coordTransformX(x), coordTransformY(y + height), width, height, filepath);
    }
}

/**********************************
*        pdf绘制过程控制对象      *
**********************************/
PdfControl *createPdfControl(char *pdfName)
{
    PdfControl *newPdfControl = NULL;
    int iErr = 0;
    
    if (!pdfName)
    {
        LOG(LOG_ERROR, "Please provide the filename. ");
        return NULL;
    }

    newPdfControl = (PdfControl *)malloc(sizeof(PdfControl));

    newPdfControl->pageHeight = PAGEHEIGHT_A4;
    newPdfControl->pageWidth = PAGEWIDTH_A4;
    newPdfControl->currPosition = 0;
    newPdfControl->lineSpace = 0;
    newPdfControl->font = NULL;
    newPdfControl->paragraphSpace = 10;
    newPdfControl->pageMarginsWidth = g_pageMarginsWidth;
    newPdfControl->pageMarginsHeight = g_pageMarginsHeight;
    newPdfControl->pageEditWidth = PAGEWIDTH_A4 - 2 * newPdfControl->pageMarginsWidth;
    strcpy(newPdfControl->fileName, pdfName);
    
    epdf_textpath(FontFileDirectory"ttf/");

    newPdfControl->pdfDoc = epdf_new(pdfName, EPDF_ENABLE_READ);
    if (newPdfControl->pdfDoc < 0) {
        LOG(LOG_ERROR, "error: cannot create PdfDoc object\n");
        iErr = -1;
    }

    if (!iErr)
    {
        pdfCreatePage(newPdfControl);
    }

    if (iErr)
    {
        free(newPdfControl);
        return NULL;
    }

    return newPdfControl;
}

//设置段间距
void pdfSetParagraphSpace(PdfControl *pdfControl, float paragraphSpace)
{
    if (pdfControl)
    {
        pdfControl->paragraphSpace = paragraphSpace;
    }
}

//设置字体
void pdfSetFont(PdfControl *pdfControl, char *font)
{
    if (pdfControl)
    {
        pdfControl->font = font;
    }
}

//设置左右页边距
void pdfSetPageMarginsWidth(PdfControl *pdfControl, float pageMargins)
{
    if (pdfControl)
    {
        g_pageMarginsWidth = pageMargins;
        pdfControl->pageMarginsWidth = pageMargins;
        pdfControl->pageEditWidth = PAGEWIDTH_A4 - 2 * pdfControl->pageMarginsWidth;
    }
}

//设置段间距
void pdfSetPageMarginsHeight(PdfControl *pdfControl, float pageMargins)
{
    if (pdfControl)
    {
        g_pageMarginsHeight = pageMargins;
        pdfControl->pageMarginsHeight = pageMargins;
    }
}
//创建新页
static int pdfCreatePage(PdfControl *pdfControl)
{
    if (!pdfControl)
    {
        return -1;
    }

    pdfControl->page = epdf_page2(pdfControl->pdfDoc, PAGE_SIZE_A4, PAGE_PORTRAIT);
    //epdf_pageset(pdfControl->page, PAGE_SIZE_A4, PAGE_PORTRAIT);
    pdfControl->currPosition = 0;
    return 0;
}

//申请一片绘制区域
PdfPainter pdfGetPainter(PdfControl *pdfControl, float height)
{
    PdfPainter painter;
    painter.position = 0;
    
    if (pdfControl->currPosition + 2*pdfControl->pageMarginsHeight + height > pdfControl->pageHeight)
    {
        pdfCreatePage(pdfControl);
    }

    painter.page = pdfControl->page;
    painter.position = pdfControl->currPosition;
    pdfControl->currPosition += height;
    return painter;
}

//绘制结束，保存文件，回收内存
void pdfDrawTheEnd(PdfControl *pdfControl)
{
    if (pdfControl)
    {
        epdf_close(pdfControl->pdfDoc);
        free(pdfControl);
        pdfControl = NULL;
    }
}

//绘制段间距
void pdfDrawParagraphSpace(PdfControl *pdfControl)
{
    if (pdfControl)
    {
        pdfControl->currPosition += pdfControl->paragraphSpace;
    }
}

//绘制指定高度的空白间隔
void pdfDrawSpace(PdfControl *pdfControl, float height)
{
    if (pdfControl)
    {
        pdfControl->currPosition += height;
    }
}

/**********************************
*             显示标题             *
**********************************/
void pdfTitleDraw(PdfControl *pdfControl, float offset, char *text, float lineWidth,
    char *font, float size, EPDF_TextStyle style, float colorR, float colorG, float colorB)
{
    if (!pdfControl)
    {
        return;
    }

    float textWidth = pdfControl->pageWidth - (coordTransformX(offset) * 2);
    PdfPainter painter = pdfGetPainter(pdfControl, size + lineWidth);
	float desent = epdf_textdesent(painter.page, font, size);
    
    epdf_setrgbstroke(painter.page, COLOR_RGB(colorR), COLOR_RGB(colorG), COLOR_RGB(colorB));
    epdf_setlinewidth(painter.page, lineWidth);
    pdfDrawText(painter.page, offset, painter.position, textWidth, size, size, style, PDF_TEXT_ALIGN_LEFT, font, text);
    pdfDrawLine(painter.page, offset, painter.position + size + desent,
        offset + textWidth, painter.position + size + desent);

    pdfDrawParagraphSpace(pdfControl);
}

//绘制带下划线的文本 下划线宽度为0时不画下划线
void pdfTextWithUnderlineDraw(PdfControl *pdfControl, float offset, char *text, float lineWidth,
    char *font, float size, EPDF_TextStyle style)
{
    if (!pdfControl)
    {
        return;
    }
    
    PdfPainter painter = pdfGetPainter(pdfControl, size + lineWidth);
	float desent = epdf_textdesent(painter.page, font, size);
	
    epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
    epdf_setlinewidth(painter.page, lineWidth);
    epdf_textstyle(painter.page, style);
    float textWidth = epdf_textwidth(painter.page, size, font, text);
    pdfDrawText(painter.page, offset, painter.position, textWidth + 5, size, size, style, PDF_TEXT_ALIGN_LEFT, font, text);
    if (lineWidth)
    {
        pdfDrawLine(painter.page, offset, painter.position + size + desent,
            offset + textWidth, painter.position + size + desent);
    }

    pdfDrawSpace(pdfControl, 2);
}

//绘制文本后带线 xxxx:____________________
void pdfTextWithWriteDraw(PdfControl *pdfControl, float offset, char *text, 
    float lineWidth, float length, char *font, float size, EPDF_TextStyle style)
{
    if (!pdfControl)
    {
        return;
    }
    PdfPainter painter = pdfGetPainter(pdfControl, size + lineWidth);
    epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
    epdf_setlinewidth(painter.page, lineWidth);
    epdf_textstyle(painter.page, style);
    float textWidth = epdf_textwidth(painter.page,  size, font, text);
    pdfDrawText(painter.page, offset, painter.position, textWidth + 20, size, size, style, PDF_TEXT_ALIGN_LEFT, font, text);
    pdfDrawLine(painter.page, offset + textWidth, painter.position + size,
        length, painter.position + size);
}
/**********************************
*              画表格             *
**********************************/
//创建一个表格
PdfTable * pdfTableCreate(unsigned int rowCount, unsigned int columnCount, float width)
{
    PdfTable *newTable;
    unsigned int i = 0, j = 0;
    newTable = (PdfTable*)malloc(sizeof(PdfTable));

    newTable->rowCount = rowCount;
    newTable->columnCount = columnCount;
    newTable->isShowLine = 1;
    newTable->tableWidth = width;
    newTable->tableAlignment = PDF_TABLE_ALIGN_CENTER;
    newTable->lineColor = 0x00000000;

    newTable->rows = (PdfTableRow*)malloc(rowCount*sizeof(PdfTableRow));
    for (i = 0; i < rowCount; i++)
    {
        newTable->rows[i].height = 20;
        newTable->rows[i].adaptiveHeight = 0;
        newTable->rows[i].items = (PdfTableItem*)malloc(columnCount*sizeof(PdfTableItem));
        for (j = 0; j < columnCount; j++)
        {
            newTable->rows[i].items[j].rowNum = i;
            newTable->rows[i].items[j].columnNum = j;
            newTable->rows[i].items[j].cellWidth = width / columnCount;
            newTable->rows[i].items[j].rowCount = 1;
            newTable->rows[i].items[j].columnCount = 1;
            newTable->rows[i].items[j].cellHorizontalAlignment = PDF_TEXT_ALIGN_H_CENTER;
            newTable->rows[i].items[j].cellVerticalAlignment = PDF_TEXT_ALIGN_V_CENTER;
            newTable->rows[i].items[j].cellStyle = TEXT_NORMAL;
            newTable->rows[i].items[j].backgroundColor = 0;
            newTable->rows[i].items[j].textColor = 0;
            newTable->rows[i].items[j].cellSize = 16;
            newTable->rows[i].items[j].text = NULL;
            newTable->rows[i].items[j].isValid = 1;
        }
    }

    return newTable;
}

//处理需要自适应的单元格，当文本太长不能显示时，插入换行符使其换行。
static char* pdfTableHandleText(PdfTableRow *row, unsigned int column, char *text)
{
    if(!row || !text)
    {
        return NULL;
    }
    
    char buff[512];
    char buff1[1024];
    char *ret = NULL;
    PdfTableItem *item = &row->items[column];
    float textWidth = 0;
    float cellWidth = 0;
    int textLength = strlen(text);
    int i = 0, j = 0;
    memset(buff, 0, 512);
    memset(buff1, 0, 1024);
    
    textWidth = epdf_textwidth(0, item->cellSize, item->cellFont, text);
    for (i = 0; i < item->columnCount; i++)
    {
        cellWidth += row->items[column + i].cellWidth;
    }
    cellWidth -= 1;
    
    if(row->adaptiveHeight && (textWidth > cellWidth))
    {
        for(i = 0; i < textLength; i++)
        {
            if(text[i] == '\t' || text[i] == '\n' || text[i] == '\r' || text[i] == ' ' || i == textLength - 1)
            {
                buff[j] = text[i];
                strcat(buff1, buff);
                memset(buff, 0, 512);
                j = 0;
            }
            else if((text[i] & 0x80) && (!(text[i] & 0x40)))
            {
                buff[j] = text[i];
                j++;
            }
            else
            {
                int buffLength = epdf_textwidth(0, item->cellSize, item->cellFont, buff);
                if(buffLength > cellWidth - item->cellSize * 2)
                {
                    buff[j] = ' ';
                    strcat(buff1, buff);
                    memset(buff, 0, 512);
                    j = 0;
                }
                buff[j] = text[i];
                j++;
            }
        }
    }
    else
    {
        strcpy(buff1, text);
    }
    
    textLength = strlen(buff1);
    ret = (char *)calloc(textLength+1, sizeof(char));
    memcpy(ret ,buff1, textLength);

    return ret;
}

//为表格设置显示文本
int pdfTableSetText(PdfTable *table, unsigned int row, unsigned int column, char *text)
{
    if (!text || !table)
    {
        return -1;
    }

    if (row >= table->rowCount || column >= table->columnCount)
    {
        LOG(LOG_ERROR, "The table does not have %d row and %d column\n", row, column);
        return -2;
    }

    PdfTableItem *item = &table->rows[row].items[column];
    if (item->text)
    {
        free(item->text);
        item->text = NULL;
    }
    
    item->text = pdfTableHandleText(&table->rows[row], column, text);
    return 0;
}

//设置单元格文本颜色
int pdfTableSetTextColor(PdfTable *table, 
    unsigned int startRow, unsigned int startColumn,
    unsigned int endRow, unsigned int endColumn, 
    unsigned int colorR, unsigned int colorG, unsigned int colorB)
{
    if (!table)
    {
        return -1;
    }

    unsigned int i, j;
    for (i = startRow; i <= endRow; i++)
    {
        if (i >= table->rowCount) break;
        for (j = startColumn; j <= endColumn; j++)
        {
            if (j >= table->columnCount) break;

            PdfTableItem *tableItem = &table->rows[i].items[j];

            tableItem->textColor = (colorR << 16) | (colorG << 8) | (colorB);
        }
    }

    return 0;
}

//修改列宽 宽度为占总宽度的百分比
int pdfTableSetColumnWidth(PdfTable *table, unsigned int column, float width)
{
    if (!table || width >= 1)
    {
        return -1;
    }

    if (column >= table->columnCount)
    {
        return -2;
    }
    unsigned int i;
    for (i = 0; i < table->rowCount; i++)
    {
        table->rows[i].items[column].cellWidth = table->tableWidth * width;
    }

    return 0;
}

//修改表格指定行高度
int pdfTableSetRowHeight(PdfTable *table, unsigned int startRow, unsigned int endRow, float height)
{
    if (!table || startRow > endRow)
    {
        return -1;
    }

    unsigned int i;
    for (i = startRow; i <= endRow; i++)
    {
        if (i >= table->rowCount)
        {
            break;
        }
        table->rows[i].height = height;
    }
    return 0;
}

//设置指定行高是否自适应
int pdfTableSetRowHeightAdaptive(PdfTable *table, unsigned int startRow, unsigned int endRow, int isAdaptive)
{
    if (!table || startRow > endRow)
    {
        return -1;
    }

    unsigned int i;
    for (i = startRow; i <= endRow; i++)
    {
        if (i >= table->rowCount)
        {
            break;
        }
        table->rows[i].adaptiveHeight = isAdaptive;
    }
    return 0;
}

//设置是否画表格线
int pdfTableSetLineEnable(PdfTable *table, unsigned int enable)
{
    if (!table)
    {
        return -1;
    }
    table->isShowLine = enable;

    return 0;
}

//设置表格线颜色
int pdfTableSetLineColor(PdfTable *table, unsigned int colorR, unsigned int colorG, unsigned int colorB)
{
    if (!table)
    {
        return -1;
    }

    table->lineColor = (colorR << 16) | (colorG << 8) | (colorB);
    return 0;
}

//设置表格对齐方式
int pdfTableSetAlignment(PdfTable *table, PDFTABLEALIGNMENT alignment)
{
    if (!table)
    {
        return -1;
    }
    table->tableAlignment = alignment;

    return 0;
}

//设置指定单元格的字体,大小和字体样式
int pdfTableSetItemFontAndSize(PdfTable *table, unsigned int startRow, unsigned int startColumn, 
                                                unsigned int endRow, unsigned int endColumn,
                                                char *font, float size, EPDF_TextStyle style)
{
    if (!table)
    {
        return -1;
    }
    unsigned int i, j;
    for (i = startRow; i <= endRow; i++)
    {
        if (i >= table->rowCount) break;
        for (j = startColumn; j <= endColumn; j++)
        {
            if (j >= table->columnCount) break;

            PdfTableItem *tableItem = &table->rows[i].items[j];

            tableItem->cellFont = font;
            tableItem->cellSize = size;
            tableItem->cellStyle = style;
        }
    }
    return 0;
}

//设置指定单元格的对齐方式
int pdfTableSetItemAlignment(PdfTable *table, unsigned int startRow, unsigned int startColumn,
                                              unsigned int endRow, unsigned int endColumn,
                                              PDF_TEXT_HORIZONTAL_ALIGNMENT Halignment,
                                              PDF_TEXT_VERTICAL_ALIGNMENT Valignment)
{
    if (!table)
    {
        return -1;
    }

    unsigned int i, j;
    for (i = startRow; i <= endRow; i++)
    {
        if (i >= table->rowCount) break;
        for (j = startColumn; j <= endColumn; j++)
        {
            if (j >= table->columnCount) break;

            PdfTableItem *tableItem = &table->rows[i].items[j];

            tableItem->cellHorizontalAlignment = Halignment;
            tableItem->cellVerticalAlignment = Valignment;
        }
    }

    return 0;
}

//设置指定单元格的背景色
int pdfTableSetItemBackgroundColor(PdfTable *table, unsigned int startRow, unsigned int startColumn,
                                                unsigned int endRow, unsigned int endColumn,
                                                unsigned int colorR, unsigned int colorG, unsigned int colorB)
{
    if (!table)
    {
        return -1;
    }

    unsigned int i, j;
    for (i = startRow; i <= endRow; i++)
    {
        if (i >= table->rowCount) break;
        for (j = startColumn; j <= endColumn; j++)
        {
            if (j >= table->columnCount) break;

            PdfTableItem *tableItem = &table->rows[i].items[j];

            tableItem->backgroundColor = (colorR<<16)|(colorG<<8)|(colorB);
        }
    }

    return 0;
}

//设置合并单元格
int pdfTableMergeCells(PdfTable *table, unsigned int startRow, unsigned int startColumn,
    unsigned int endRow, unsigned int endColumn)
{
    if (!table)
    {
        return -1;
    }

    if (startRow >= table->rowCount || endRow >= table->rowCount
        || startColumn >= table->columnCount || endColumn >= table->columnCount)
    {
        return -2;
    }

    table->rows[startRow].items[startColumn].rowCount = endRow - startRow + 1;
    table->rows[startRow].items[startColumn].columnCount = endColumn - startColumn + 1;

    unsigned int i, j;
    for (i = startRow; i <= endRow; i++)
    {
        for (j = startColumn; j <= endColumn; j++)
        {
            if (i == startRow && j == startColumn)
                continue;

            PdfTableItem *tableItem = &table->rows[i].items[j];

            tableItem->isValid = 0;
        }
    }

    return 0;
}

//计算表格单元格文本高度
static float pdfTableGetCellTextHeight(PdfControl *pdfControl, float cellWidth, char *font, float fontSize, char *text)
{
    if (pdfControl && text)
    {
        float textWidth = epdf_textwidth(pdfControl->page, fontSize, font, text);

        fontSize += 1;
                
        int LineCount = (int)textWidth / (cellWidth - fontSize) + 1;

        if(LineCount == 2)
        {
            fontSize += 1;
        }
        
        float height = LineCount * fontSize;
                
        return height;
    }

    return 0;
}

//获得单元格需要的高度，根据实际高度和文本长度决定
static float pdfTableGetCellHeight(PdfControl *pdfControl, PdfTable *table, int rowNum, int columnNum)
{
    float cellHeight = 0.0f;
    float cellWidth = 0.0f;
    float textHeight = 0.0f;
    PdfTableRow *row = &table->rows[rowNum];
    PdfTableItem *item = &row->items[columnNum];
    int i = 0;
    if (!item->isValid)
        return cellHeight;
        
    for(i = 0; i < item->rowCount; i++)
    {
        cellHeight += table->rows[rowNum + i].height;
    }
    
    for (i = 0; i < item->columnCount; i++)
    {
        cellWidth += row->items[columnNum + i].cellWidth;
    }

    textHeight = pdfTableGetCellTextHeight(pdfControl, cellWidth, item->cellFont, item->cellSize, item->text);

    if(cellHeight < textHeight)
    {
        cellHeight = textHeight;
        if(row->adaptiveHeight)
        {
            for(i = 0; i < item->rowCount; i++)
            {
                table->rows[rowNum + i].height = cellHeight / item->rowCount;
            }
        }
    }
    
    return cellHeight;
}

//计算表格指定行所需要占用的高度，若当前页剩余空间不够，则移植下一页显示,返回绘制对象
static PdfPainter pdfTableGetPainter(PdfControl *pdfControl, PdfTable *table, unsigned int rowNum)
{
    PdfPainter painter = {0.0f, 0};
    if (table && pdfControl && rowNum < table->rowCount)
    {
        int i = 0;
        PdfTableRow *tableRow = &table->rows[rowNum];
        float maxHeight = tableRow->height;
        for (i = 0; i < table->columnCount; i++)
        {
            float cellHeight = pdfTableGetCellHeight(pdfControl, table, rowNum, i);
            if(cellHeight == 0.0f)
                continue;
            
            if (maxHeight < cellHeight)
            {
                maxHeight = cellHeight;
            }
        }

        //判断当前页是否够用
        if (pdfControl->currPosition + 2 * pdfControl->pageMarginsHeight + maxHeight > pdfControl->pageHeight)
        {
            pdfCreatePage(pdfControl);
        }

        painter = pdfGetPainter(pdfControl, tableRow->height);
    }

    return painter;
}

//绘制表格的一行
void pdfTableDrawRow(PdfControl *pdfControl, PdfTable *table, unsigned int rowNum)
{
    if (table && pdfControl && rowNum < table->rowCount)
    {
        PdfTableRow *tableRow = &table->rows[rowNum];
        PdfPainter painter = pdfTableGetPainter(pdfControl, table, rowNum);
        float drawPosition = 0;
        unsigned int i;

        switch (table->tableAlignment)
        {
        case PDF_TABLE_ALIGN_LEFT:
            break;
        case PDF_TABLE_ALIGN_RIGHT:
            drawPosition = pdfControl->pageEditWidth - table->tableWidth;
            break;
        case PDF_TABLE_ALIGN_CENTER:
            drawPosition = (pdfControl->pageEditWidth - table->tableWidth) / 2;
            break;
        default:
            break;
        }

        //绘制背景和文本
        for (i = 0; i < table->columnCount; i++)
        {
            unsigned int R = 0;
            unsigned int G = 0;
            unsigned int B = 0;
            PdfTableItem *item = &tableRow->items[i];
            float cellWidth = 0.0;
            float cellheight = 0.0;

            //计算单元格宽度
            unsigned int j = 0;
            for (j = 0; j < item->columnCount; j++)
            {
                cellWidth += tableRow->items[j + i].cellWidth;
            }

            //计算单元格高度
            for (j = 0; j < item->rowCount; j++)
            {
                cellheight += table->rows[rowNum + j].height;
            }

            //绘制表格线
            if (table->isShowLine)
            {
                R = (table->lineColor & 0x00ff0000) >> 16;
                G = (table->lineColor & 0x0000ff00) >> 8;
                B = (table->lineColor & 0x000000ff);

                //设置字体，颜色
                epdf_setrgbstroke(painter.page, COLOR_RGB(R), COLOR_RGB(G), COLOR_RGB(B));
                epdf_setlinewidth(painter.page, 0.5);

                //表格第一行或分页第一行 画上边线
                if (painter.position == 0 || rowNum == 0)
                {
                    pdfDrawLine(painter.page, drawPosition, painter.position - 0.25, 
                        drawPosition + cellWidth, painter.position - 0.25);
                }

                //第一列 画左边线
                if (i == 0)
                {
                    pdfDrawLine(painter.page,
                        drawPosition, painter.position - 0.5,
                        drawPosition, painter.position + tableRow->height);
                }

                //最后一行 或 下一行有效 或 当前单元格有效且只占一行但下一行无效 画下边线
                if (rowNum == table->rowCount - 1 || table->rows[rowNum+1].items[i].isValid
                    || (!table->rows[rowNum + 1].items[i].isValid && item->isValid && item->rowCount == 1))
                {
                    pdfDrawLine(painter.page, drawPosition, painter.position + tableRow->height - 0.25,
                        drawPosition + cellWidth, painter.position + tableRow->height - 0.25);
                }

                //当前单元格有效 画右边线
                if (item->isValid)
                {
                    pdfDrawLine(painter.page,
                        drawPosition + cellWidth, painter.position - 0.5,
                        drawPosition + cellWidth, painter.position + cellheight);
                }
                epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
            }

            //单元格有效 画单元格背景色
            if (item->isValid && item->backgroundColor)
            {
                R = (item->backgroundColor & 0x00ff0000)>>16;
                G = (item->backgroundColor & 0x0000ff00)>>8;
                B = (item->backgroundColor & 0x000000ff);
                epdf_setrgbstroke(painter.page, COLOR_RGB(R), COLOR_RGB(G), COLOR_RGB(B));
                epdf_setlinewidth(painter.page, cellheight - 1);

                pdfDrawLine(painter.page, drawPosition + 0.5, painter.position + (cellheight - 0.5) / 2,
                    drawPosition + cellWidth - 0.5, painter.position + (cellheight - 0.5) / 2);
                epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
            }
            
            //单元格有效 显示文本
            if (item->isValid && item->text)
            {
                float Xposition = drawPosition + 1;
                float Yposition = 0.0f;
                float height = item->cellSize;
                if (item->cellVerticalAlignment == PDF_TEXT_ALIGN_TOP)
                {
                    Yposition = painter.position + 1;
                    height = cellheight;
                }
                else if (item->cellVerticalAlignment == PDF_TEXT_ALIGN_V_CENTER)
                {
                    height = pdfTableGetCellTextHeight(pdfControl, cellWidth, item->cellFont, item->cellSize, item->text);
                    height = height > cellheight ? cellheight : height;
                    Yposition = painter.position + (cellheight - height) / 2;
                }
                else
                {
                    height = pdfTableGetCellTextHeight(pdfControl, cellWidth, item->cellFont, item->cellSize, item->text);
                    height = height > cellheight ? cellheight : height;
                    Yposition = painter.position + cellheight - height;
                }
                
                R = (item->textColor & 0x00ff0000) >> 16;
                G = (item->textColor & 0x0000ff00) >> 8;
                B = (item->textColor & 0x000000ff);
                epdf_setrgbfill(painter.page, COLOR_RGB(R), COLOR_RGB(G), COLOR_RGB(B));
                epdf_settextleading(painter.page, item->cellSize);
                pdfDrawText(painter.page, Xposition, Yposition, cellWidth - 1,
                    height, item->cellSize, item->cellStyle, item->cellHorizontalAlignment,
                    item->cellFont, item->text);
                epdf_setrgbfill(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
            }
            drawPosition += item->cellWidth;
        }
    }
}

//绘制表格
void pdfTableDraw(PdfControl *pdfControl, PdfTable *table)
{
    if (pdfControl && table)
    {
        unsigned int i;
        for (i = 0; i < table->rowCount; i++)
        {
            pdfTableDrawRow(pdfControl, table,i);
        }
    }
}

//回收表格资源
void pdfTableFree(PdfTable *table)
{   
    if (table)
    {
        unsigned int i = 0, j = 0;

        for (i = 0; i < table->rowCount; i++)
        {
            for (j = 0; j < table->columnCount; j++)
            {
                if (table->rows[i].items[j].text)
                {
                    free(table->rows[i].items[j].text);
                }
            }
            free(table->rows[i].items);
        }
        free(table->rows);

        free(table);
        table = NULL;
    }
}

/**********************************
*              画图片             *
**********************************/
void pdfPictureDraw(PdfControl *pdfControl, char *filePath, float width, float height, float offset)
{
    if (pdfControl && filePath)
    {
        PdfPainter painter = pdfGetPainter(pdfControl, height);
        pdfDrawImage(painter.page, filePath, offset, painter.position, width, height);

        pdfDrawParagraphSpace(pdfControl);
    }
}

/**********************************
*              绘制分隔线           *
**********************************/
void pdfSeparatorDraw(PdfControl *pdfControl, float width, float length,
    unsigned int colorR, unsigned int colorG, unsigned int colorB)
{
    if (pdfControl)
    {
        PdfPainter painter = pdfGetPainter(pdfControl, width);
        float offset = (pdfControl->pageEditWidth - length) / 2;
        //设置字体，颜色
        epdf_setrgbstroke(painter.page, COLOR_RGB(colorR), COLOR_RGB(colorG), COLOR_RGB(colorB));
        epdf_setlinewidth(painter.page, width);

        pdfDrawLine(painter.page, offset, painter.position + width/2, offset + length, painter.position + width/2);
    }
}

/**********************************
*              绘制页眉信息       *
**********************************/
void pdfHeaderDraw(unsigned int page, float OffsetFromTheLeft, float OffsetFromTheTop,
    char *text, char *font, float size, EPDF_TextStyle style,
    unsigned int fontColorR, unsigned int fontColorG, unsigned int fontColorB)
{
    epdf_setrgbfill(page, COLOR_RGB(fontColorR), COLOR_RGB(fontColorG), COLOR_RGB(fontColorB));
    float textWidth = epdf_textwidth(page, size, font, text);
    textWidth += 5;
    pdfDrawText(page, OffsetFromTheLeft - g_pageMarginsWidth, OffsetFromTheTop - g_pageMarginsHeight, textWidth, size, size, style, PDF_TEXT_ALIGN_LEFT, font, text);
    epdf_setrgbfill(page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
}

/**********************************
*              绘制页脚信息       *
**********************************/
void pdfFooterDraw(unsigned int page, float OffsetFromTheLeft, float OffsetFromTheBottom,
    char *text, char *font, float  size, EPDF_TextStyle style,
    unsigned int fontColorR, unsigned int fontColorG, unsigned int fontColorB)
{
    epdf_setrgbfill(page, COLOR_RGB(fontColorR), COLOR_RGB(fontColorG), COLOR_RGB(fontColorB));
    float textWidth = epdf_textwidth(page, size, font, text);
    textWidth += 5;
    pdfDrawText(page, OffsetFromTheLeft - g_pageMarginsWidth, PAGEHEIGHT_A4 - g_pageMarginsHeight - OffsetFromTheBottom, textWidth, size, size, style, PDF_TEXT_ALIGN_LEFT, font, text);
    epdf_setrgbfill(page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
}

/**********************************
*              得到当前语言       *
**********************************/
char *pdfReportGetLanguage(PdfControl *pControl, unsigned int currLanguage)
{
    char *pRet = EPDF_EN;
    int pdf = pControl->pdfDoc;
    //中文 韩文 日文 阿拉伯 越南 泰文 波斯 只能用标准字体，不能重新设置字体
    switch (currLanguage)
    {
    case LANG_CHINASIM:
        pRet = EPDF_CN;
        break;
    case LANG_ENGLISH:
        epdf_textfont(pdf, EPDF_EN, FONT_HELVETICA);
        pRet = EPDF_EN;
        break;
    case LANG_KOREAN:
        pRet = EPDF_KO;
        break;
    case LANG_JAPANESE:
        pRet = EPDF_JP;
        break;
    case LANG_RUSSIAN:
        pRet = EPDF_RU;
        break;
    case LANG_GERMAN:
        epdf_textfont(pdf, EPDF_GE, FONT_HELVETICA);
        pRet = EPDF_GE;
        break;
    case LANG_FRANCE:
        epdf_textfont(pdf, EPDF_FR, FONT_HELVETICA);
        pRet = EPDF_FR;
        break;
    case LANG_VIETNAM:
        pRet = EPDF_VI;
        break;
    case LANG_SPANISH:
        epdf_textfont(pdf, EPDF_SP, FONT_HELVETICA);
        pRet = EPDF_SP;
        break;
    case LANG_TURKISH:
        pRet = EPDF_TU;
        break;
    case LANG_ITALY:
        pRet = EPDF_IT;
        break;
    case LANG_PORTUGAL:
        pRet = EPDF_PO;
        break;
    case LANG_HINDI:
        pRet = EPDF_HI;
        break;
    case LANG_SWIDISH:
        pRet = EPDF_SW;
        break;
    case LANG_POLISH:
        pRet = EPDF_PL;
        break;
    case LANG_UYGHUR:
        break;
    case LANG_BENGALI:
        pRet = EPDF_BE;
        break;
    case LANG_DANISH:
        pRet = EPDF_DA;
        break;
    case LANG_CZECH:
        pRet = EPDF_CZ;
        break;
    case LANG_UKRAINIAN:
        pRet = EPDF_UK;
        break;
    case LANG_GREEK:
        pRet = EPDF_GR;
        break;
    case LANG_TAGALOG:
        break;
    case LANG_LATIN:
        pRet = EPDF_LA;
        break;
    case LANG_IRISH:
        pRet = EPDF_IR;
        break;
    case LANG_HUNGARIAN:
        pRet = EPDF_HU;
        break;
    case LANG_DUTCH:
        pRet = EPDF_DU;
        break;
    case LANG_ROMANA:
        pRet = EPDF_RO;
        break;
    case LANG_SUOMI:
        pRet = EPDF_SU;
        break;
    case LANG_SERBIAN:
        pRet = EPDF_SE;
        break;
    case LANG_CROATIA:
        pRet = EPDF_CR;
        break;
    case LANG_THAI:
        pRet = EPDF_TH;
        break;
    case LANG_ARABIA:
        pRet = EPDF_AR;
        break;
    case LANG_PERSIAN:
        pRet = EPDF_PE;
        break;
    default:
        break;

    }
    return pRet;
}
