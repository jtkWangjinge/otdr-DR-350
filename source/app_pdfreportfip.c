/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_pdfreportfip.c
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
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "app_pdf.h"
#include "app_global.h"
#include "app_pdfreportfip.h"
#include "guiconfig.h"
#include "wnd_global.h"
#include "app_frmfip.h"
#include "app_parameter.h"
#include "app_frmupdate.h"
#include <string.h>

extern OpticalFiberEndSur   OptFiberEndSurCheck;
extern FipIdentify          fipIdentify;
extern PSYSTEMSET 		    pCurSystemSet;

jmp_buf env;

static char *font;

//定义文本
static char *pFipRptTitle = NULL;
static char *pFipRptInfo = NULL;
static char *pFipRptFilename = NULL;
static char *pFipRptCheckDate = NULL;
static char *pFipRptVersion = NULL;
static char *pFipRptAnalyzeDate = NULL;
static char *pFipRptTaskIdentify = NULL;
static char *pFipRptClient = NULL;
static char *pFipRptCompany = NULL;
static char *pFipRptNote = NULL;
static char *pFipRptPosition = NULL;
static char *pFipRptOperator = NULL;
static char *pFipRptPositionA = NULL;
static char *pFipRptPositionB = NULL;
static char *pFipRptCableID = NULL;
static char *pFipRptImage = NULL;
static char *pFipRptFiberID = NULL;
static char *pFipRptLayering = NULL;
static char *pFipRptLayeringDiameter = NULL;
static char *pFipRptScracth = NULL;
static char *pFipRptDefect = NULL;
static char *pFipRptConnectorID = NULL;
static char *pFipRptGoal = NULL;
static char *pFipRptTotal = NULL;
static char *pFipRptCore = NULL;
static char *pFipRptCladding = NULL;
static char *pFipRptBuffer = NULL;
static char *pFipRptJacket = NULL;
static char *pFipRptIdentification = NULL;
static char *pFipRptResult = NULL;
static char *pFipRptSize = NULL;
static char *pFipRptLe = NULL;
static char *pFipRptInf = NULL;
static char *pFipRptPage = NULL;
static char *pFipRptAny = NULL;
static char *pFipRptSignature = NULL;
static char *pFipRptDate = NULL;

//初始化report文本
static void InitFipReportText(unsigned int currLanguage)
{
    unsigned int language = currLanguage;//GetCurrLanguageSet();
	pFipRptTitle = GetCurrLanguageUtf8Text(1, language);
	pFipRptInfo = GetCurrLanguageUtf8Text(2, language);
	pFipRptFilename = GetCurrLanguageUtf8Text(3, language);
	pFipRptCheckDate = GetCurrLanguageUtf8Text(4, language);
	pFipRptVersion = GetCurrLanguageUtf8Text(5, language);
	pFipRptAnalyzeDate = GetCurrLanguageUtf8Text(6, language);
	pFipRptTaskIdentify = GetCurrLanguageUtf8Text(7, language);
	pFipRptClient = GetCurrLanguageUtf8Text(8, language);
	pFipRptCompany = GetCurrLanguageUtf8Text(9, language);
	pFipRptNote = GetCurrLanguageUtf8Text(1, language);
	pFipRptPosition = GetCurrLanguageUtf8Text(2, language);
	pFipRptOperator = GetCurrLanguageUtf8Text(3, language);
	pFipRptPositionA = GetCurrLanguageUtf8Text(4, language);
	pFipRptPositionB = GetCurrLanguageUtf8Text(5, language);
	pFipRptCableID = GetCurrLanguageUtf8Text(6, language);
	pFipRptImage = GetCurrLanguageUtf8Text(7, language);
	pFipRptFiberID = GetCurrLanguageUtf8Text(8, language);
	pFipRptLayering = GetCurrLanguageUtf8Text(9, language);
	pFipRptLayeringDiameter = GetCurrLanguageUtf8Text(1, language);
	pFipRptScracth = GetCurrLanguageUtf8Text(2, language);
	pFipRptDefect = GetCurrLanguageUtf8Text(3, language);
	pFipRptConnectorID = GetCurrLanguageUtf8Text(4, language);
	pFipRptGoal = GetCurrLanguageUtf8Text(5, language);
	pFipRptTotal = GetCurrLanguageUtf8Text(6, language);
	pFipRptCore = GetCurrLanguageUtf8Text(7, language);
	pFipRptCladding = GetCurrLanguageUtf8Text(8, language);
	pFipRptBuffer = GetCurrLanguageUtf8Text(9, language);
	pFipRptJacket = GetCurrLanguageUtf8Text(1, language);
	pFipRptIdentification = GetCurrLanguageUtf8Text(2, language);
	pFipRptResult = GetCurrLanguageUtf8Text(3, language);
	pFipRptSize = GetCurrLanguageUtf8Text(4, language);
	pFipRptLe = GetCurrLanguageUtf8Text(5, LANG_CHINASIM);
	pFipRptInf = GetCurrLanguageUtf8Text(6, LANG_CHINASIM);
	pFipRptPage = GetCurrLanguageUtf8Text(7, language);
	pFipRptAny = GetCurrLanguageUtf8Text(8, language);
	pFipRptSignature = GetCurrLanguageUtf8Text(9, language);
	pFipRptDate = GetCurrLanguageUtf8Text(1, language);
}

//销毁report文本
static void ExitFipReportText(void)
{	
	free(pFipRptTitle);
	free(pFipRptInfo);
	free(pFipRptFilename);
	free(pFipRptCheckDate);
	free(pFipRptVersion);
	free(pFipRptAnalyzeDate);
	free(pFipRptTaskIdentify);
	free(pFipRptClient);
	free(pFipRptCompany);
	free(pFipRptNote);
	free(pFipRptPosition);
	free(pFipRptOperator);
	free(pFipRptPositionA);
	free(pFipRptPositionB);
	free(pFipRptCableID);
	free(pFipRptImage);
	free(pFipRptFiberID);
	free(pFipRptLayering);
	free(pFipRptLayeringDiameter);
	free(pFipRptScracth);
	free(pFipRptDefect);
	free(pFipRptConnectorID);
	free(pFipRptGoal);
	free(pFipRptTotal);
	free(pFipRptCore);
	free(pFipRptCladding);
	free(pFipRptBuffer);
	free(pFipRptJacket);
	free(pFipRptIdentification);
	free(pFipRptResult);
	free(pFipRptSize);
	free(pFipRptLe);
	free(pFipRptInf);
	free(pFipRptPage);
	free(pFipRptAny);
	free(pFipRptSignature);
	free(pFipRptDate);
}

/**
* 将时间转换为字符串
* @param[in] timeT 指定的时间(为空时 使用当前时间)
* @param[out] cTime
* @return
* @note 有关此函数的特别说明：…
*/
static int fipPdfReportGetTime(time_t *timeT, char *cTime)
{
    if(!cTime)
    {
        return -1;
    }
    
	struct tm *pTime = NULL;
	if(timeT)
	{
	    pTime = gmtime(timeT);
	}
	else
	{
        time_t currentTime;
    	time(&currentTime);
    	pTime = gmtime(&currentTime);
	}

    if(pCurSystemSet->uiTimeShowFlag == 0)
    {
	    strftime(cTime, 128, "%d/%m/%Y %X", pTime);
    }
    else if(pCurSystemSet->uiTimeShowFlag == 1)
    {
        strftime(cTime, 128, "%m/%d/%Y %X", pTime);
    }
    else
    {
        strftime(cTime, 128, "%Y/%m/%d %X", pTime);
    }

    return 0;
}

static int fipPdfReportGetIdentifierInfo(int enable, char *info, char *out)
{
    if(!info || !out)
    {
        return -1;
    }

    if(enable)
    {
        strcpy(out, info);
    }
    else
    {
        strcpy(out, " ");
    }

    return 0;
}

/**
* 绘制一般信息
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int fipPdfReportGeneralInfo(PdfControl *pdfControl, char *name)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pGeneralInfoTable = NULL;
    char buff[128] = {0};
    VERSION pPar;

    pGeneralInfoTable = pdfTableCreate(4, 4, pdfControl->pageEditWidth);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 0, 0, 4, 4, font, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 0, 1, 0, 1, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 0, 3, 0, 3, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 1, 1, 1, 1, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 1, 3, 1, 3, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 2, 1, 2, 1, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 2, 3, 2, 3, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 3, 1, 3, 1, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 3, 3, 3, 3, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pGeneralInfoTable, 0);
    pdfTableSetRowHeight(pGeneralInfoTable, 0, 4, 15);
    pdfTableSetItemAlignment(pGeneralInfoTable, 0, 0, 4, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pGeneralInfoTable, 0, 1, 0, 1, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_TOP);
    pdfTableSetItemAlignment(pGeneralInfoTable, 3, 3, 3, 3, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_TOP);
    pdfTableSetColumnWidth(pGeneralInfoTable, 0, 0.15);
    pdfTableSetColumnWidth(pGeneralInfoTable, 1, 0.35);
    pdfTableSetColumnWidth(pGeneralInfoTable, 2, 0.15);
    pdfTableSetColumnWidth(pGeneralInfoTable, 3, 0.35);

    pdfTableSetText(pGeneralInfoTable, 0, 0, pFipRptFilename);
    pdfTableSetText(pGeneralInfoTable, 0, 1, name);
    pdfTableSetText(pGeneralInfoTable, 0, 2, pFipRptCheckDate);
    fipPdfReportGetTime(NULL, buff);
    pdfTableSetText(pGeneralInfoTable, 0, 3, buff);
    pdfTableSetText(pGeneralInfoTable, 1, 0, pFipRptVersion);
    memset(&pPar, 0, sizeof(VERSION));
    ReadUpdateVersion("/app/version.bin", &pPar);
    sprintf(buff, "V%s", pPar.Userfs);
    pdfTableSetText(pGeneralInfoTable, 1, 1, buff);
    pdfTableSetText(pGeneralInfoTable, 1, 2, pFipRptAnalyzeDate);
    fipPdfReportGetTime(NULL, buff);
    pdfTableSetText(pGeneralInfoTable, 1, 3, buff);
    pdfTableSetText(pGeneralInfoTable, 2, 0, pFipRptTaskIdentify);
    fipPdfReportGetIdentifierInfo(fipIdentify.taskIdentifyIsSelect, fipIdentify.taskIdentify, buff);
    pdfTableSetText(pGeneralInfoTable, 2, 1, buff);
    pdfTableSetText(pGeneralInfoTable, 2, 2, pFipRptClient);
    fipPdfReportGetIdentifierInfo(fipIdentify.clientIsSelect, fipIdentify.client, buff);
    pdfTableSetText(pGeneralInfoTable, 2, 3, buff);
    pdfTableSetText(pGeneralInfoTable, 3, 0, pFipRptCompany);
    fipPdfReportGetIdentifierInfo(fipIdentify.companyIsSelect, fipIdentify.company, buff);
    pdfTableSetText(pGeneralInfoTable, 3, 1, buff);
    pdfTableSetText(pGeneralInfoTable, 3, 2, pFipRptNote);
    fipPdfReportGetIdentifierInfo(fipIdentify.noteIsSelect, fipIdentify.note, buff);
    pdfTableSetText(pGeneralInfoTable, 3, 3, buff);

    pdfTableDraw(pdfControl, pGeneralInfoTable);

    pdfTableFree(pGeneralInfoTable);

    return iErr;
}

/**
* 绘制位置信息
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int fipPdfReportLocations(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pLocationsTable = NULL;
    char buff[128] = {0};

    pLocationsTable = pdfTableCreate(2, 3, pdfControl->pageEditWidth);
    pdfTableSetItemFontAndSize(pLocationsTable, 0, 0, 0, 3, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pLocationsTable, 1, 0, 1, 3, font, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 1, 1, 1, 1, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 1, 2, 1, 2, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pLocationsTable, 1);
    pdfTableSetRowHeight(pLocationsTable, 0, 5, 15);
    pdfTableSetItemAlignment(pLocationsTable, 0, 0, 1, 3, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetItemBackgroundColor(pLocationsTable, 0, 0, 0, 3, 192, 192, 192);
    pdfTableSetLineColor(pLocationsTable, 22, 180, 240);

    pdfTableSetText(pLocationsTable, 0, 1, pFipRptPositionA);
    pdfTableSetText(pLocationsTable, 0, 2, pFipRptPositionB);
    pdfTableSetText(pLocationsTable, 1, 0, pFipRptOperator);
    fipPdfReportGetIdentifierInfo(fipIdentify.positionAIsSelect, fipIdentify.positionA, buff);
    pdfTableSetText(pLocationsTable, 1, 1, buff);
    fipPdfReportGetIdentifierInfo(fipIdentify.positionBIsSelect, fipIdentify.positionB, buff);
    pdfTableSetText(pLocationsTable, 1, 2, buff);

    pdfTableDraw(pdfControl, pLocationsTable);
    pdfDrawSpace(pdfControl, 5);

    pdfTableFree(pLocationsTable);

    return iErr;
}

/**
* 绘制标识信息
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int fipPdfReportIdentifiers(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pIdentifiersTable = NULL;
    char buff[128] = {0};

    pIdentifiersTable = pdfTableCreate(2, 5, pdfControl->pageEditWidth);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 0, 0, 0, 4, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 0, 1, 4, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 1, 1, 1, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 2, 1, 2, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 3, 1, 3, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 4, 1, 4, EPDF_UN, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pIdentifiersTable, 1);
    pdfTableSetRowHeight(pIdentifiersTable, 0, 2, 15);
    pdfTableSetItemAlignment(pIdentifiersTable, 0, 0, 2, 4, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetItemBackgroundColor(pIdentifiersTable, 0, 0, 0, 4, 192, 192, 192);
    pdfTableSetLineColor(pIdentifiersTable, 22, 180, 240);

    pdfTableSetText(pIdentifiersTable, 0, 0, pFipRptCableID);
    pdfTableSetText(pIdentifiersTable, 0, 1, pFipRptFiberID);
    pdfTableSetText(pIdentifiersTable, 0, 2, pFipRptPositionA);
    pdfTableSetText(pIdentifiersTable, 0, 3, pFipRptPositionB);
    pdfTableSetText(pIdentifiersTable, 0, 4, pFipRptConnectorID);
    fipPdfReportGetIdentifierInfo(fipIdentify.cableIDIsSelect, fipIdentify.cableID, buff);
    pdfTableSetText(pIdentifiersTable, 1, 0, buff);
    fipPdfReportGetIdentifierInfo(fipIdentify.fiberIDIsSelect, fipIdentify.fiberID, buff);
    pdfTableSetText(pIdentifiersTable, 1, 1, buff);
    fipPdfReportGetIdentifierInfo(fipIdentify.positionAIsSelect, fipIdentify.positionA, buff);
    pdfTableSetText(pIdentifiersTable, 1, 2, buff);
    fipPdfReportGetIdentifierInfo(fipIdentify.positionBIsSelect, fipIdentify.positionB, buff);
    pdfTableSetText(pIdentifiersTable, 1, 3, buff);
    fipPdfReportGetIdentifierInfo(fipIdentify.connectorIDIsSelect, fipIdentify.connectorID, buff);
    pdfTableSetText(pIdentifiersTable, 1, 4, buff);

    pdfTableDraw(pdfControl, pIdentifiersTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pIdentifiersTable);

    return iErr;
}


/**
* 绘制图像
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int fipPdfReportShowImage(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }
    char *originalPicturePath = "/app/fip_original.jpeg";
    char *processedPicturePath = "/app/fip_processed.jpeg";
    int imageWidth = 478;
    int imageHeight = 472;
    int zoomin = 2;
    
    float xPosition = (pdfControl->pageEditWidth / 2 - imageWidth/zoomin) / 2;
    PdfPainter painter = pdfGetPainter(pdfControl, imageHeight/zoomin);
    
    creatFipOriginalPicture(originalPicturePath, imageWidth, imageHeight);
    
    pdfDrawImage(painter.page, originalPicturePath, xPosition, painter.position, 
        imageWidth/zoomin, imageHeight/zoomin);
        
    xPosition = (pdfControl->pageEditWidth / 2 - imageWidth/zoomin) / 2 + pdfControl->pageEditWidth / 2;
    
    creatFipProcessedPicture(processedPicturePath, imageWidth, imageHeight);
    
    pdfDrawImage(painter.page, processedPicturePath, xPosition, painter.position, 
        imageWidth/zoomin, imageHeight/zoomin);

    mysystem("rm /app/fip*.jpeg");

    pdfDrawSpace(pdfControl, 5);
    return iErr;
}

static char* fipPdfReportGetSizeFont(char *currFont)
{
    char *sizeFont = EPDF_CN;

    if(strcmp(currFont, EPDF_VI) == 0)
    {
        sizeFont = EPDF_VI;
    }
    else if(strcmp(currFont, EPDF_SP) == 0)
    {
        sizeFont = EPDF_RU;
    }
    else if(strcmp(currFont, EPDF_GE) == 0)
    {
        sizeFont = EPDF_RU;
    }
    else if(strcmp(currFont, EPDF_KO) == 0)
    {
        sizeFont = EPDF_KO;
    }
    else if(strcmp(currFont, EPDF_TH) == 0)
    {
        sizeFont = EPDF_TH;
    }
    else if(strcmp(currFont, EPDF_PE) == 0)
    {
        sizeFont = EPDF_PE;
    }
    else if(strcmp(currFont, EPDF_AR) == 0)
    {
        sizeFont = EPDF_AR;
    }
    
    return sizeFont;
}

/**
* 绘制结果信息
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int fipPdfReportResult(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pResultTable = NULL;
    int isFindCore = 0;
    char buff[128] = {0};
    char *cellFont = fipPdfReportGetSizeFont(font);
    if(OptFiberEndSurCheck.ResultInfo.CirX >= 0 || OptFiberEndSurCheck.ResultInfo.CirY >= 0)
    {
        isFindCore = 1;
    }

    int isAllRed = 0;
    //若没有找到光纤端面则全显示黑色
	if(AnalyzeResult(OptFiberEndSurCheck.ResultInfo) && OptFiberEndSurCheck.ResultInfo.Isqualified != 1)
	{
        isAllRed = 1;
	}
    
    pResultTable = pdfTableCreate(9, 9, pdfControl->pageEditWidth);
    pdfTableSetItemFontAndSize(pResultTable, 0, 0, 0, 8, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pResultTable, 1, 0, 8, 8, font, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pResultTable, 2, 3, 8, 8, cellFont, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pResultTable, 1);
    pdfTableSetRowHeight(pResultTable, 0, 8, 15);
    pdfTableSetItemAlignment(pResultTable, 0, 0, 8, 8, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pResultTable, 0, 2, 0, 2, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_TOP);

    pdfTableSetItemBackgroundColor(pResultTable, 0, 0, 1, 8, 192, 192, 192);
    pdfTableSetLineColor(pResultTable, 22, 180, 240);

    pdfTableMergeCells(pResultTable, 0, 0, 1, 1);
    pdfTableMergeCells(pResultTable, 0, 2, 1, 2);
    pdfTableMergeCells(pResultTable, 0, 3, 0, 5);
    pdfTableMergeCells(pResultTable, 0, 6, 0, 8);
    pdfTableMergeCells(pResultTable, 3, 0, 5, 0);
    pdfTableMergeCells(pResultTable, 3, 1, 5, 1);
    pdfTableMergeCells(pResultTable, 3, 2, 5, 2);
    pdfTableMergeCells(pResultTable, 7, 0, 8, 0);
    pdfTableMergeCells(pResultTable, 7, 1, 8, 1);
    pdfTableMergeCells(pResultTable, 7, 2, 8, 2);

    pdfTableSetColumnWidth(pResultTable, 0, 0.03);
    pdfTableSetColumnWidth(pResultTable, 1, 0.12);
    pdfTableSetColumnWidth(pResultTable, 2, 0.15);
    pdfTableSetColumnWidth(pResultTable, 3, 0.19);
    pdfTableSetColumnWidth(pResultTable, 4, 0.08);
    pdfTableSetColumnWidth(pResultTable, 5, 0.08);
    pdfTableSetColumnWidth(pResultTable, 6, 0.19);
    pdfTableSetColumnWidth(pResultTable, 7, 0.08);
    pdfTableSetColumnWidth(pResultTable, 8, 0.08);

    pdfTableSetText(pResultTable, 0, 0, pFipRptLayering);
    sprintf(buff, "%s\n(um)", pFipRptLayeringDiameter);
    pdfTableSetText(pResultTable, 0, 2, buff);
    pdfTableSetText(pResultTable, 0, 3, pFipRptScracth);
    pdfTableSetText(pResultTable, 0, 6, pFipRptDefect);
    pdfTableSetText(pResultTable, 1, 3, pFipRptSize);
    pdfTableSetText(pResultTable, 1, 4, pFipRptGoal);
    pdfTableSetText(pResultTable, 1, 5, pFipRptTotal);
    pdfTableSetText(pResultTable, 1, 6, pFipRptSize);
    pdfTableSetText(pResultTable, 1, 7, pFipRptGoal);
    pdfTableSetText(pResultTable, 1, 8, pFipRptTotal);

    pdfTableSetText(pResultTable, 2, 0, "A");
    pdfTableSetText(pResultTable, 2, 1, pFipRptCore);
    pdfTableSetText(pResultTable, 2, 2, "0-25");
    sprintf(buff, "0%s%s<%s", pFipRptLe, pFipRptSize, pFipRptInf);
    pdfTableSetText(pResultTable, 2, 3, buff);
    pdfTableSetText(pResultTable, 2, 4, "0");
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberCoreScracth);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    if(OptFiberEndSurCheck.ResultInfo.FiberCoreScracth == 0 && !isAllRed)
	{
        pdfTableSetTextColor(pResultTable, 2, 5, 2, 5, 0, 255, 0);
	}
    else
    {
        pdfTableSetTextColor(pResultTable, 2, 5, 2, 5, 255, 255, 255);
        pdfTableSetItemBackgroundColor(pResultTable, 2, 5, 2, 5, 255, 0, 0);
    }
    
    pdfTableSetText(pResultTable, 2, 5, buff);
    sprintf(buff, "0%s%s<%s", pFipRptLe, pFipRptSize, pFipRptInf);
    pdfTableSetText(pResultTable, 2, 6, buff);
    pdfTableSetText(pResultTable, 2, 7, "0");
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberCoreDefect);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    if(OptFiberEndSurCheck.ResultInfo.FiberCoreDefect== 0 && !isAllRed)
	{
        pdfTableSetTextColor(pResultTable, 2, 8, 2, 8, 0, 255, 0);
	}
    else
    {
        pdfTableSetTextColor(pResultTable, 2, 8, 2, 8, 255, 255, 255);
        pdfTableSetItemBackgroundColor(pResultTable, 2, 8, 2, 8, 255, 0, 0);
    }
    
    pdfTableSetText(pResultTable, 2, 8, buff);
    pdfTableSetText(pResultTable, 3, 0, "B");
    pdfTableSetText(pResultTable, 3, 1, pFipRptCladding);
    pdfTableSetText(pResultTable, 3, 2, "25-120");
    sprintf(buff, "0%s%s<3", pFipRptLe, pFipRptSize);
    pdfTableSetText(pResultTable, 3, 3, buff);
    pdfTableSetText(pResultTable, 3, 4, pFipRptAny);
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthSmall);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    isAllRed ? pdfTableSetTextColor(pResultTable, 3, 5, 3, 5, 0, 0, 0)
             : pdfTableSetTextColor(pResultTable, 3, 5, 3, 5, 0, 255, 0);
    pdfTableSetText(pResultTable, 3, 5, buff);
    sprintf(buff, "0%s%s<2", pFipRptLe, pFipRptSize);
    pdfTableSetText(pResultTable, 3, 6, buff);
    pdfTableSetText(pResultTable, 3, 7, pFipRptAny);
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectSmall);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    isAllRed ? pdfTableSetTextColor(pResultTable, 3, 8, 3, 8, 0, 0, 0)
             : pdfTableSetTextColor(pResultTable, 3, 8, 3, 8, 0, 255, 0);    
    pdfTableSetText(pResultTable, 3, 8, buff);
    sprintf(buff, "3%s%s<%s", pFipRptLe, pFipRptSize, pFipRptInf);
    pdfTableSetText(pResultTable, 4, 3, buff);
    pdfTableSetText(pResultTable, 4, 4, "0");
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    if(OptFiberEndSurCheck.ResultInfo.FiberCladdingScracthLarge == 0 && !isAllRed)
	{
        pdfTableSetTextColor(pResultTable, 4, 5, 4, 5, 0, 255, 0);
	}
    else
    {
        pdfTableSetTextColor(pResultTable, 4, 5, 4, 5, 255, 255, 255);
        pdfTableSetItemBackgroundColor(pResultTable, 4, 5, 4, 5, 255, 0, 0);
    }
    
    pdfTableSetText(pResultTable, 4, 5, buff);
    sprintf(buff, "2%s%s<5", pFipRptLe, pFipRptSize);
    pdfTableSetText(pResultTable, 4, 6, buff);
    pdfTableSetText(pResultTable, 4, 7, "5");
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    if(OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectMedium <= 5 && !isAllRed)
	{
        pdfTableSetTextColor(pResultTable, 4, 8, 4, 8, 0, 255, 0);
	}
    else
    {
        pdfTableSetTextColor(pResultTable, 4, 8, 4, 8, 255, 255, 255);
        pdfTableSetItemBackgroundColor(pResultTable, 4, 8, 4, 8, 255, 0, 0);
    }
    
    pdfTableSetText(pResultTable, 4, 8, buff);
    sprintf(buff, "5%s%s<%s", pFipRptLe, pFipRptSize, pFipRptInf);
    pdfTableSetText(pResultTable, 5, 6, buff);
    pdfTableSetText(pResultTable, 5, 7, "0");
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    if(OptFiberEndSurCheck.ResultInfo.FiberCladdingDefectLarge == 0 && !isAllRed)
	{
        pdfTableSetTextColor(pResultTable, 5, 8, 5, 8, 0, 255, 0);
	}
    else
    {
        pdfTableSetTextColor(pResultTable, 5, 8, 5, 8, 255, 255, 255);
        pdfTableSetItemBackgroundColor(pResultTable, 5, 8, 5, 8, 255, 0, 0);
    }
    
    pdfTableSetText(pResultTable, 5, 8, buff);
    pdfTableSetText(pResultTable, 6, 0, "C");
    pdfTableSetText(pResultTable, 6, 1, pFipRptBuffer);
    pdfTableSetText(pResultTable, 6, 2, "120-130");
    pdfTableSetText(pResultTable, 6, 3, "---");
    pdfTableSetText(pResultTable, 6, 4, "---");
    pdfTableSetText(pResultTable, 6, 5, "---");
    pdfTableSetText(pResultTable, 6, 6, "---");
    pdfTableSetText(pResultTable, 6, 7, "---");
    pdfTableSetText(pResultTable, 6, 8, "---");

    pdfTableSetText(pResultTable, 7, 0, "D");
    pdfTableSetText(pResultTable, 7, 1, pFipRptJacket);
    pdfTableSetText(pResultTable, 7, 2, "130-250");
    sprintf(buff, "0%s%s<%s", pFipRptLe, pFipRptSize, pFipRptInf);
    pdfTableSetText(pResultTable, 7, 3, buff);
    pdfTableSetText(pResultTable, 7, 4, pFipRptAny);
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberJacketScracth);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    isAllRed ? pdfTableSetTextColor(pResultTable, 7, 5, 7, 5, 0, 0, 0)
             : pdfTableSetTextColor(pResultTable, 7, 5, 7, 5, 0, 255, 0);
    pdfTableSetText(pResultTable, 7, 5, buff);
    sprintf(buff, "0%s%s<10", pFipRptLe, pFipRptSize);
    pdfTableSetText(pResultTable, 7, 6, buff);
    pdfTableSetText(pResultTable, 7, 7, pFipRptAny);
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberJacketDefectSmall);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    isAllRed ? pdfTableSetTextColor(pResultTable, 7, 8, 7, 8, 0, 0, 0)
             : pdfTableSetTextColor(pResultTable, 7, 8, 7, 8, 0, 255, 0);
    
    pdfTableSetText(pResultTable, 7, 8, buff);
    sprintf(buff, "10%s%s<%s", pFipRptLe, pFipRptSize, pFipRptInf);
    pdfTableSetText(pResultTable, 8, 6, buff);
    pdfTableSetText(pResultTable, 8, 7, "0");
    if(isFindCore)
    {
        sprintf(buff, "%d", OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge);
    }
    else
    {
        strcpy(buff, "N/A");
    }

    if(OptFiberEndSurCheck.ResultInfo.FiberJacketDefectLarge == 0 && !isAllRed)
	{
        pdfTableSetTextColor(pResultTable, 8, 8, 8, 8, 0, 255, 0);
	}
    else
    {
        pdfTableSetTextColor(pResultTable, 8, 8, 8, 8, 255, 255, 255);
        pdfTableSetItemBackgroundColor(pResultTable, 8, 8, 8, 8, 255, 0, 0);
    }

    if(isAllRed)
    {
        pdfTableSetTextColor(pResultTable, 2, 5, 2, 5, 0, 0, 0);
        pdfTableSetItemBackgroundColor(pResultTable, 2, 5, 2, 5, 255, 255, 255);
        
        pdfTableSetTextColor(pResultTable, 2, 8, 2, 8, 0, 0, 0);
        pdfTableSetItemBackgroundColor(pResultTable, 2, 8, 2, 8, 255, 255, 255);
        
        pdfTableSetTextColor(pResultTable, 4, 5, 4, 5, 0, 0, 0);
        pdfTableSetItemBackgroundColor(pResultTable, 4, 5, 4, 5, 255, 255, 255);
        
        pdfTableSetTextColor(pResultTable, 4, 8, 4, 8, 0, 0, 0);
        pdfTableSetItemBackgroundColor(pResultTable, 4, 8, 4, 8, 255, 255, 255);
        
        pdfTableSetTextColor(pResultTable, 5, 8, 5, 8, 0, 0, 0);
        pdfTableSetItemBackgroundColor(pResultTable, 5, 8, 5, 8, 255, 255, 255);
        
        pdfTableSetTextColor(pResultTable, 8, 8, 8, 8, 0, 0, 0);
        pdfTableSetItemBackgroundColor(pResultTable, 8, 8, 8, 8, 255, 255, 255);
    }
    
    pdfTableSetText(pResultTable, 8, 8, buff);
    pdfTableDraw(pdfControl, pResultTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pResultTable);


    return iErr;
}


/**
* SOLA 页眉页脚
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int fipPdfReportHeaderAndFooter(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }
    char buff[128];
    unsigned int i = 0;
    for (i = 0; i <= pdfControl->page; i++)
    {
        /**********页眉*********/
        //sola 报告
        pdfHeaderDraw(i, pdfControl->pageMarginsWidth, 15, pFipRptTitle, font, 18, TEXT_BOLD, 0, 0, 0);

        //通过/不通过
        if(OptFiberEndSurCheck.ResultInfo.Isqualified == 1)
        {
            pdfDrawImage(i, BmpFileDirectory"fip_pass.jpg", 400, 15 - pdfControl->pageMarginsHeight, 89 / 1.8, 49 / 1.8);
        }
        else
        {
            pdfDrawImage(i, BmpFileDirectory"fip_fail.jpg", 400, 15 - pdfControl->pageMarginsHeight, 89 / 1.8, 49 / 1.8);
        }
        /**********页脚*********/
        //签名
        float textWidth = epdf_textwidth(i, 10.0, font, pFipRptSignature);
        pdfDrawText(i, 120, pdfControl->pageHeight - pdfControl->pageMarginsHeight - 20,
            textWidth + 20, 10.0, 10.0, TEXT_NORMAL, PDF_TEXT_ALIGN_LEFT, font, pFipRptSignature);

        //日期
        time_t currentTime;
        struct tm *pTime = NULL;
    	time(&currentTime);
    	pTime = gmtime(&currentTime);
        char cTime[128] = {0};
        if(pCurSystemSet->uiTimeShowFlag == 0)
        {
    	    strftime(cTime, 128, "%d/%m/%Y", pTime);
        }
        else if(pCurSystemSet->uiTimeShowFlag == 1)
        {
            strftime(cTime, 128, "%m/%d/%Y", pTime);
        }
        else
        {
            strftime(cTime, 128, "%Y/%m/%d", pTime);
        }

        sprintf(buff, "%s%s", pFipRptDate, cTime);
        pdfFooterDraw(i, pdfControl->pageWidth / 2 + 30, 20, buff, font, 10, TEXT_NORMAL, 0, 0, 0);
        //页码
        sprintf(buff, "%s%d/%d", pFipRptPage, i + 1, pdfControl->page + 1);
        pdfFooterDraw(i, pdfControl->pageWidth - 100, 20, buff, font, 10, TEXT_NORMAL, 0, 0, 0);
    }

    return iErr;
}

int createFipPdfReport(char *pAbsolutePath, unsigned int currLanguage)
{
    char *pName = strrchr(pAbsolutePath, '/');
    pName++;
    PdfControl *pControl = createPdfControl(pAbsolutePath);

    InitFipReportText(currLanguage);
    if (!pControl)
    {
        return -1;
    }

    if (setjmp(env))
    {
        epdf_close(pControl->pdfDoc);
        free(pControl);
        return -1;
    }
    font = pdfReportGetLanguage(pControl, currLanguage);
    pdfSetFont(pControl, font);
    //设置段间距
    pdfSetParagraphSpace(pControl, 5);
    //设置页边距
    pdfSetPageMarginsWidth(pControl, 30);
    //一般信息
    pdfTitleDraw(pControl, 0, pFipRptInfo, 1, font, 14, TEXT_BOLD, 22, 180, 240);
    fipPdfReportGeneralInfo(pControl, pName);
    //位置信息
    pdfTitleDraw(pControl, 0, pFipRptPosition, 1, font, 14, TEXT_BOLD, 22, 180, 240);
    fipPdfReportLocations(pControl);
    //标识信息
    pdfTitleDraw(pControl, 0, pFipRptIdentification, 1, font, 14, TEXT_BOLD, 255, 255, 255);
    fipPdfReportIdentifiers(pControl);
    //绘制图片
    pdfTitleDraw(pControl, 0, pFipRptImage, 1, font, 14, TEXT_BOLD, 22, 180, 240);
    fipPdfReportShowImage(pControl);
    //结果信息
    pdfTitleDraw(pControl, 0, pFipRptResult, 1, font, 14, TEXT_BOLD, 255, 255, 255);
    fipPdfReportResult(pControl);
    //页眉页脚
    fipPdfReportHeaderAndFooter(pControl);

    pdfDrawTheEnd(pControl);
    ExitFipReportText();
    
    return 1;
}

//生成报告
int CreateFipReport(char* absolutePath)
{
    int iErr = 0;
    if (absolutePath)
    {
        iErr = createFipPdfReport(absolutePath, GetCurrLanguageSet());
        if(access(absolutePath, F_OK) != 0)
        {
            iErr = -1;
        }
    }
    
    return iErr;
}
