/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_pdfreportsola.c
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
#include <string.h>
#include <setjmp.h>
#include "guiconfig.h"
#include "app_pdf.h"
#include "app_pdfreportsola.h"
#include "wnd_frmsola.h"
#include "wnd_frmmark.h"
#include "app_getsetparameter.h"
#include "app_screenshots.h"
#include "app_frmupdate.h"
#include "app_unitconverter.h"
#include "app_sola.h"

extern PSYSTEMSET 		    pCurSystemSet;
extern PSerialNum           pCurSN;
extern PFactoryConfig       pFactoryConf;
extern POTDR_TOP_SETTINGS   pOtdrTopSettings;	//当前OTDR的工作设置
extern SOLA_MESSAGER*       pSolaMessager;

static char *font;
static int iUnitConverterFlag = 0;              //单位转换的标志位
static SOLA_DATA_LIST* s_solaDataList = NULL;

//定义文本
static char *pPdfRptFileName = NULL;
static char *pPdfRptTestDate = NULL;
static char *pPdfRptCustomer = NULL;
static char *pPdfRptTestTime = NULL;
static char *pPdfRptCompany = NULL;
static char *pPdfRptJobID = NULL;
static char *pPdfRptComments = NULL;
static char *pPdfRptLocationA = NULL;
static char *pPdfRptLocationB = NULL;
static char *pPdfRptDeraction = NULL;
static char *pPdfRptOperator = NULL;
static char *pPdfRptModel = NULL;
static char *pPdfRptSerialNumber = NULL;
static char *pPdfRptCalibrationData = NULL;
static char *pPdfRptVersion = NULL;
static char *pPdfRptCableID = NULL;
static char *pPdfRptFiberID = NULL;
static char *pPdfRptLinkLength = NULL;
static char *pPdfRptWavelength = NULL;
static char *pPdfRptLinkLoss = NULL;
static char *pPdfRptLinkORL = NULL;
static char *pPdfRptSplitter1_2 = NULL;
static char *pPdfRptSplitter1_4 = NULL;
static char *pPdfRptSplitter1_8 = NULL;
static char *pPdfRptSplitter1_16 = NULL;
static char *pPdfRptSplitter1_32 = NULL;
static char *pPdfRptSplitter1_64 = NULL;
static char *pPdfRptSplitter1_128 = NULL;
static char *pPdfRptSplice = NULL;
static char *pPdfRptConnector = NULL;
static char *pPdfRptSection = NULL;
static char *pPdfRptMacrobend = NULL;
static char *pPdfRptContinuousFiber = NULL;
static char *pPdfRptType = NULL;
static char *pPdfRptNO = NULL;
static char *pPdfRptPosition = NULL;
static char *pPdfRptPosition_km = NULL;
static char *pPdfRptPosition_m = NULL;
static char *pPdfRptPosition_mile = NULL;
static char *pPdfRptPosition_kft = NULL;
static char *pPdfRptPosition_ft = NULL;
static char *pPdfRptLoss = NULL;
static char *pPdfRptReflect = NULL;
static char *pPdfRptWave = NULL;
static char *pPdfRptMaxORL = NULL;
static char *pPdfRptLength = NULL;
static char *pPdfRptLinkLength_km = NULL;
static char *pPdfRptLinkLength_m = NULL;
static char *pPdfRptLinkLength_mile = NULL;
static char *pPdfRptLinkLength_kft = NULL;
static char *pPdfRptLinkLength_ft = NULL;
static char *pPdfRptMin = NULL;
static char *pPdfRptMax = NULL;
static char *pPdfRptUserDefinePassFailThresholds = NULL;
static char *pPdfRptThreshold = NULL;
static char *pPdfRptMaxSpliceLoss = NULL;
static char *pPdfRptMaxConnectorLoss = NULL;
static char *pPdfRptMaxConnectorReflectance = NULL;
static char *pPdfRptMaxSplitterLoss1_2 = NULL;
static char *pPdfRptMaxSplitterLoss1_4 = NULL;
static char *pPdfRptMaxSplitterLoss1_8 = NULL;
static char *pPdfRptMaxSplitterLoss1_16 = NULL;
static char *pPdfRptMaxSplitterLoss1_32 = NULL;
static char *pPdfRptMaxSplitterLoss1_64 = NULL;
static char *pPdfRptMaxSplitterLoss1_128 = NULL;
static char *pPdfRptMaxSplitterReflectance = NULL;
static char *pPdfRptIOR = NULL;
static char *pPdfRptBackScatter = NULL;
static char *pPdfRptLaunchFiber = NULL;
static char *pPdfRptReceiveFiber = NULL;
static char *pPdfRptSOLAReport = NULL;
static char *pPdfRptPass = NULL;
static char *pPdfRptFail = NULL;
static char *pPdfRptSignature = NULL;
static char *pPdfRptDate = NULL;
static char *pPdfRptPage = NULL;
static char *pPdfRptGeneralInformation = NULL;
static char *pPdfRptLocations = NULL;
static char *pPdfRptIdentifiers = NULL;
static char *pPdfRptSOLAResults = NULL;
static char *pPdfRptLinkView = NULL;
static char *pPdfRptElementTable = NULL;
static char *pPdfRptSOLATestPassFailThresholds = NULL;
static char *pPdfRptUserDefineElementPassFailThresholds = NULL;
static char *pPdfRptSOLAParametersAndSettings = NULL;
static char *pPdfRptOTDRCurve = NULL;
//初始化report文本
static void InitPdfReportText(unsigned int language)
{
    pPdfRptFileName = GetCurrLanguageUtf8Text(1, language);
    pPdfRptTestDate = GetCurrLanguageUtf8Text(2, language);
    pPdfRptCustomer = GetCurrLanguageUtf8Text(3, language);
    pPdfRptTestTime = GetCurrLanguageUtf8Text(4, language);
    pPdfRptCompany = GetCurrLanguageUtf8Text(5, language);
    pPdfRptJobID = GetCurrLanguageUtf8Text(6, language);
    pPdfRptComments = GetCurrLanguageUtf8Text(7, language);
    pPdfRptLocationA = GetCurrLanguageUtf8Text(8, language);
    pPdfRptLocationB = GetCurrLanguageUtf8Text(9, language);
    pPdfRptDeraction = GetCurrLanguageUtf8Text(1, language);
    pPdfRptOperator = GetCurrLanguageUtf8Text(2, language);
    pPdfRptModel = GetCurrLanguageUtf8Text(3, language);
    pPdfRptSerialNumber = GetCurrLanguageUtf8Text(4, language);
    pPdfRptCalibrationData = GetCurrLanguageUtf8Text(5, language);
    pPdfRptVersion = GetCurrLanguageUtf8Text(6, language);
    pPdfRptCableID = GetCurrLanguageUtf8Text(7, language);
    pPdfRptFiberID = GetCurrLanguageUtf8Text(8, language);
    pPdfRptLinkLength = GetCurrLanguageUtf8Text(9, language);
    pPdfRptWavelength = GetCurrLanguageUtf8Text(10, language);
    pPdfRptLinkLoss = GetCurrLanguageUtf8Text(1, language);
    pPdfRptLinkORL = GetCurrLanguageUtf8Text(2, language);
    pPdfRptSplitter1_2 = GetCurrLanguageUtf8Text(3, language);
    pPdfRptSplitter1_4 = GetCurrLanguageUtf8Text(4, language);
    pPdfRptSplitter1_8 = GetCurrLanguageUtf8Text(5, language);
    pPdfRptSplitter1_16 = GetCurrLanguageUtf8Text(6, language);
    pPdfRptSplitter1_32 = GetCurrLanguageUtf8Text(7, language);
    pPdfRptSplitter1_64 = GetCurrLanguageUtf8Text(8, language);
    pPdfRptSplitter1_128 = GetCurrLanguageUtf8Text(9, language);
    pPdfRptSplice = GetCurrLanguageUtf8Text(1, language);
    pPdfRptConnector = GetCurrLanguageUtf8Text(2, language);
    pPdfRptSection = GetCurrLanguageUtf8Text(3, language);
    pPdfRptMacrobend = GetCurrLanguageUtf8Text(4, language);
    pPdfRptContinuousFiber = GetCurrLanguageUtf8Text(5, language);
    pPdfRptType = GetCurrLanguageUtf8Text(6, language);
    pPdfRptNO = GetCurrLanguageUtf8Text(7, language);
    pPdfRptPosition = GetCurrLanguageUtf8Text(8, language);
    pPdfRptPosition_km = GetCurrLanguageUtf8Text(9, language);
    pPdfRptPosition_m = GetCurrLanguageUtf8Text(1, language);
    pPdfRptPosition_mile = GetCurrLanguageUtf8Text(2, language);
    pPdfRptPosition_kft = GetCurrLanguageUtf8Text(3, language);
    pPdfRptPosition_ft = GetCurrLanguageUtf8Text(4, language);
    pPdfRptLoss = GetCurrLanguageUtf8Text(5, language);
    pPdfRptReflect = GetCurrLanguageUtf8Text(6, language);
    pPdfRptWave = GetCurrLanguageUtf8Text(7, language);
    pPdfRptMaxORL = GetCurrLanguageUtf8Text(8, language);
    pPdfRptLength = GetCurrLanguageUtf8Text(9, language);
    pPdfRptLinkLength_km= GetCurrLanguageUtf8Text(1, language);
    pPdfRptLinkLength_m= GetCurrLanguageUtf8Text(2, language);
    pPdfRptLinkLength_mile= GetCurrLanguageUtf8Text(3, language);
    pPdfRptLinkLength_kft = GetCurrLanguageUtf8Text(4, language);
    pPdfRptLinkLength_ft = GetCurrLanguageUtf8Text(5, language);
    pPdfRptMin = GetCurrLanguageUtf8Text(6, language);
    pPdfRptMax = GetCurrLanguageUtf8Text(7, language);
    pPdfRptUserDefinePassFailThresholds = GetCurrLanguageUtf8Text(8, language);
    pPdfRptThreshold = GetCurrLanguageUtf8Text(9, language);
    pPdfRptMaxSpliceLoss = GetCurrLanguageUtf8Text(1, language);
    pPdfRptMaxConnectorLoss = GetCurrLanguageUtf8Text(2, language);
    pPdfRptMaxConnectorReflectance = GetCurrLanguageUtf8Text(3, language);
    pPdfRptMaxSplitterLoss1_2 = GetCurrLanguageUtf8Text(4, language);
    pPdfRptMaxSplitterLoss1_4 = GetCurrLanguageUtf8Text(5, language);
    pPdfRptMaxSplitterLoss1_8 = GetCurrLanguageUtf8Text(6, language);
    pPdfRptMaxSplitterLoss1_16 = GetCurrLanguageUtf8Text(7, language);
    pPdfRptMaxSplitterLoss1_32 = GetCurrLanguageUtf8Text(8, language);
    pPdfRptMaxSplitterLoss1_64 = GetCurrLanguageUtf8Text(9, language);
    pPdfRptMaxSplitterLoss1_128 = GetCurrLanguageUtf8Text(1, language);
    pPdfRptMaxSplitterReflectance = GetCurrLanguageUtf8Text(2, language);
    pPdfRptIOR = GetCurrLanguageUtf8Text(3, language);
    pPdfRptBackScatter = GetCurrLanguageUtf8Text(4, language);
    pPdfRptLaunchFiber = GetCurrLanguageUtf8Text(5, language);
    pPdfRptReceiveFiber = GetCurrLanguageUtf8Text(6, language);
    pPdfRptSOLAReport = GetCurrLanguageUtf8Text(7, language);
    pPdfRptPass = GetCurrLanguageUtf8Text(8, language);
    pPdfRptFail = GetCurrLanguageUtf8Text(9, language);
    pPdfRptSignature = GetCurrLanguageUtf8Text(1, language);
    pPdfRptDate = GetCurrLanguageUtf8Text(2, language);
    pPdfRptPage = GetCurrLanguageUtf8Text(3, language);
    pPdfRptGeneralInformation = GetCurrLanguageUtf8Text(4, language);
    pPdfRptLocations = GetCurrLanguageUtf8Text(5, language);
    pPdfRptIdentifiers = GetCurrLanguageUtf8Text(6, language);
    pPdfRptSOLAResults = GetCurrLanguageUtf8Text(7, language);
    pPdfRptLinkView = GetCurrLanguageUtf8Text(8, language);
    pPdfRptElementTable = GetCurrLanguageUtf8Text(9, language);
    pPdfRptSOLATestPassFailThresholds = GetCurrLanguageUtf8Text(1, language);
    pPdfRptUserDefineElementPassFailThresholds = GetCurrLanguageUtf8Text(2, language);
    pPdfRptSOLAParametersAndSettings = GetCurrLanguageUtf8Text(3, language);
    pPdfRptOTDRCurve = GetCurrLanguageUtf8Text(4, language);
}

static void FreePdfReportText(void)
{
    free(pPdfRptFileName);
    free(pPdfRptTestDate);
    free(pPdfRptCustomer);
    free(pPdfRptTestTime);
    free(pPdfRptCompany);
    free(pPdfRptJobID);
    free(pPdfRptComments);
    free(pPdfRptLocationA);
    free(pPdfRptLocationB);
    free(pPdfRptDeraction);
    free(pPdfRptOperator);
    free(pPdfRptModel);
    free(pPdfRptSerialNumber);
    free(pPdfRptCalibrationData);
    free(pPdfRptVersion);
    free(pPdfRptCableID);
    free(pPdfRptFiberID);
    free(pPdfRptLinkLength);
    free(pPdfRptWavelength);
    free(pPdfRptLinkLoss);
    free(pPdfRptLinkORL);
    free(pPdfRptSplitter1_2);
    free(pPdfRptSplitter1_4);
    free(pPdfRptSplitter1_8);
    free(pPdfRptSplitter1_16);
    free(pPdfRptSplitter1_32);
    free(pPdfRptSplitter1_64);
    free(pPdfRptSplitter1_128);
    free(pPdfRptSplice);
    free(pPdfRptConnector);
    free(pPdfRptSection);
    free(pPdfRptMacrobend);
    free(pPdfRptContinuousFiber);
    free(pPdfRptType);
    free(pPdfRptNO);
    free(pPdfRptPosition_km);
    free(pPdfRptPosition_m);
    free(pPdfRptPosition_mile);
    free(pPdfRptPosition_kft);
    free(pPdfRptPosition_ft);
    free(pPdfRptLoss);
    free(pPdfRptReflect);
    free(pPdfRptWave);
    free(pPdfRptMaxORL);
    free(pPdfRptLinkLength_km);
    free(pPdfRptLinkLength_m);
    free(pPdfRptLinkLength_mile);
    free(pPdfRptLinkLength_kft);
    free(pPdfRptLinkLength_ft);
    free(pPdfRptMin);
    free(pPdfRptMax);
    free(pPdfRptUserDefinePassFailThresholds);
    free(pPdfRptThreshold);
    free(pPdfRptMaxSpliceLoss);
    free(pPdfRptMaxConnectorLoss);
    free(pPdfRptMaxConnectorReflectance);
    free(pPdfRptMaxSplitterLoss1_2);
    free(pPdfRptMaxSplitterLoss1_4);
    free(pPdfRptMaxSplitterLoss1_8);
    free(pPdfRptMaxSplitterLoss1_16);
    free(pPdfRptMaxSplitterLoss1_32);
    free(pPdfRptMaxSplitterLoss1_64);
    free(pPdfRptMaxSplitterLoss1_128);
    free(pPdfRptMaxSplitterReflectance);
    free(pPdfRptIOR);
    free(pPdfRptBackScatter);
    free(pPdfRptLaunchFiber);
    free(pPdfRptReceiveFiber);
    free(pPdfRptSOLAReport);
    free(pPdfRptPass);
    free(pPdfRptSignature);
    free(pPdfRptDate);
    free(pPdfRptPage);
    free(pPdfRptGeneralInformation);
    free(pPdfRptLocations);
    free(pPdfRptIdentifiers);
    free(pPdfRptSOLAResults);
    free(pPdfRptLinkView);
    free(pPdfRptElementTable);
    free(pPdfRptSOLATestPassFailThresholds);
    free(pPdfRptUserDefineElementPassFailThresholds);
    free(pPdfRptSOLAParametersAndSettings);
    free(pPdfRptOTDRCurve);
}

/**
* 绘制一般信息
* @param[in] pdfControl
* @param[in] name
* @return
* @note 有关此函数的特别说明：…
*/
static int solaPdfReportGeneralInfo(PdfControl *pdfControl, char *name)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pGeneralInfoTable = NULL;
    char buff[F_NAME_MAX] = {0};
    struct tm *nowTime;
    time_t now;
    now = time(NULL);
	nowTime = localtime(&now);
    MarkParam *identifySetting = &pSolaMessager->newIdentify;
    
    pGeneralInfoTable = pdfTableCreate(5, 4, pdfControl->pageEditWidth);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 0, 0, 5, 4, font, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 0, 1, 0, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 1, 1, 1, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 1, 3, 1, 3, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 2, 1, 2, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 2, 3, 2, 3, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 3, 1, 3, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pGeneralInfoTable, 4, 1, 4, 1, EPDF_UN, 9, TEXT_NORMAL);

    pdfTableSetLineEnable(pGeneralInfoTable, 0);
    pdfTableSetRowHeight(pGeneralInfoTable, 0, 5, 15);
    pdfTableSetRowHeightAdaptive(pGeneralInfoTable, 4, 4, 1);
    pdfTableSetItemAlignment(pGeneralInfoTable, 0, 0, 5, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pGeneralInfoTable, 0, 1, 0, 1, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_TOP);
    pdfTableSetItemAlignment(pGeneralInfoTable, 4, 1, 4, 1, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_TOP);
    pdfTableSetColumnWidth(pGeneralInfoTable, 0, 0.15);
    pdfTableSetColumnWidth(pGeneralInfoTable, 1, 0.35);
    pdfTableSetColumnWidth(pGeneralInfoTable, 2, 0.15);
    pdfTableSetColumnWidth(pGeneralInfoTable, 3, 0.35);
    pdfTableMergeCells(pGeneralInfoTable, 4, 1, 4, 3);

    pdfTableSetText(pGeneralInfoTable, 0, 0, pPdfRptFileName);
    pdfTableSetText(pGeneralInfoTable, 0, 1, name);
    pdfTableSetText(pGeneralInfoTable, 1, 0, pPdfRptTestDate);
    getSolaMeasureDate(buff, NULL, pCurSystemSet->uiTimeShowFlag);
    pdfTableSetText(pGeneralInfoTable, 1, 1, buff);
    pdfTableSetText(pGeneralInfoTable, 1, 2, pPdfRptCustomer);
    pdfTableSetText(pGeneralInfoTable, 1, 3, identifySetting->Customer);
    pdfTableSetText(pGeneralInfoTable, 2, 0, pPdfRptTestTime);
    getSolaMeasureDate(NULL, buff, pCurSystemSet->uiTimeShowFlag);
    pdfTableSetText(pGeneralInfoTable, 2, 1, buff);
    pdfTableSetText(pGeneralInfoTable, 2, 2, pPdfRptCompany);
    pdfTableSetText(pGeneralInfoTable, 2, 3, identifySetting->Company);
    pdfTableSetText(pGeneralInfoTable, 3, 0, pPdfRptJobID);
    pdfTableSetText(pGeneralInfoTable, 3, 1, identifySetting->TaskID_Unicode);
    pdfTableSetText(pGeneralInfoTable, 4, 0, pPdfRptComments);
    pdfTableSetText(pGeneralInfoTable, 4, 1, identifySetting->Note);

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
static int solaPdfReportLocations(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }
    char buff[128] = {0};
    PdfTable *pLocationsTable = NULL;
    MarkParam *identifySetting = &pSolaMessager->newIdentify;
    
    pLocationsTable = pdfTableCreate(6, 3, pdfControl->pageEditWidth*3/5);
    pdfTableSetAlignment(pLocationsTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pLocationsTable, 0, 0, 0, 3, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pLocationsTable, 1, 0, 5, 3, font, 10, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 1, 1, 1, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 1, 2, 1, 2, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 2, 1, 2, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 3, 1, 3, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 4, 1, 4, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pLocationsTable, 5, 1, 5, 1, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetLineEnable(pLocationsTable, 1);
    pdfTableSetRowHeight(pLocationsTable, 0, 6, 15);
    pdfTableSetItemAlignment(pLocationsTable, 0, 0, 0, 4, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pLocationsTable, 1, 0, 6, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetRowHeightAdaptive(pLocationsTable, 1, 1, 1);

    pdfTableSetItemBackgroundColor(pLocationsTable, 0, 0, 0, 3, 238, 238, 238);
    pdfTableSetLineColor(pLocationsTable, 173, 216, 230);

    pdfTableSetText(pLocationsTable, 0, 1, pPdfRptLocationA);
    pdfTableSetText(pLocationsTable, 0, 2, pPdfRptLocationB);
    pdfTableSetText(pLocationsTable, 1, 0, pPdfRptOperator);
    pdfTableSetText(pLocationsTable, 1, 1, identifySetting->Operator_A);
    pdfTableSetText(pLocationsTable, 1, 2, identifySetting->Operator_B);
    pdfTableSetText(pLocationsTable, 2, 0, pPdfRptModel);
    pdfTableSetText(pLocationsTable, 3, 0, pPdfRptSerialNumber);
    pdfTableSetText(pLocationsTable, 4, 0, pPdfRptCalibrationData);
    
    if(pCurSystemSet->uiTimeShowFlag == 0)
    {
        sprintf(buff,"%d/%d/%d",pFactoryConf->lastDate.day+1,
        pFactoryConf->lastDate.month+1,pFactoryConf->lastDate.year+1970);
    }
    else if(pCurSystemSet->uiTimeShowFlag == 1)
    {
        sprintf(buff,"%d/%d/%d",pFactoryConf->lastDate.month+1,
        pFactoryConf->lastDate.day+1,pFactoryConf->lastDate.year+1970);
    }
    else
    {
        sprintf(buff,"%d/%d/%d",pFactoryConf->lastDate.year+1970,
        pFactoryConf->lastDate.month+1,pFactoryConf->lastDate.day+1);
    }

    VERSION pPar;
    memset(&pPar, 0, sizeof(VERSION));
    ReadUpdateVersion("/app/version.bin", &pPar);

    if(identifySetting->Deraction == 0)
    {
        pdfTableSetText(pLocationsTable, 2, 1, MODEL_NUMBER);
        pdfTableSetText(pLocationsTable, 2, 2, " ");
        
        pdfTableSetText(pLocationsTable, 4, 1, buff);
        pdfTableSetText(pLocationsTable, 4, 2, " ");
        
        GenerateSerialNumber(buff, pCurSN);
        pdfTableSetText(pLocationsTable, 3, 1, buff);
        pdfTableSetText(pLocationsTable, 3, 2, " ");
        
        sprintf(buff, "V%s", pPar.Userfs);
        pdfTableSetText(pLocationsTable, 5, 1, buff);
        pdfTableSetText(pLocationsTable, 5, 2, " ");
    }
    else
    {
        pdfTableSetText(pLocationsTable, 2, 1, " ");
        pdfTableSetText(pLocationsTable, 2, 2, MODEL_NUMBER);
        
        pdfTableSetText(pLocationsTable, 4, 1, " ");
        pdfTableSetText(pLocationsTable, 4, 2, buff);
        
        GenerateSerialNumber(buff, pCurSN);
        pdfTableSetText(pLocationsTable, 3, 1, " ");
        pdfTableSetText(pLocationsTable, 3, 2, buff);
        
        sprintf(buff, "V%s", pPar.Userfs);
        pdfTableSetText(pLocationsTable, 5, 1, " ");
        pdfTableSetText(pLocationsTable, 5, 2, buff);
    }

    pdfTableSetText(pLocationsTable, 5, 0, pPdfRptVersion);

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
static int solaPdfReportIdentifiers(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pIdentifiersTable = NULL;
    MarkParam *identifySetting = &pSolaMessager->newIdentify;

    pIdentifiersTable = pdfTableCreate(2, 5, pdfControl->pageEditWidth);
    pdfTableSetAlignment(pIdentifiersTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 0, 0, 0, 5, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 0, 1, 5, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 1, 1, 5, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 2, 1, 5, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pIdentifiersTable, 1, 3, 1, 5, EPDF_UN, 9, TEXT_NORMAL);
    pdfTableSetLineEnable(pIdentifiersTable, 1);
    pdfTableSetRowHeight(pIdentifiersTable, 0, 2, 15);
    pdfTableSetItemAlignment(pIdentifiersTable, 0, 0, 2, 5, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetItemBackgroundColor(pIdentifiersTable, 0, 0, 0, 5, 238, 238, 238);
    pdfTableSetLineColor(pIdentifiersTable, 173, 216, 230);
    pdfTableSetRowHeightAdaptive(pIdentifiersTable, 1, 1, 1);

    pdfTableSetText(pIdentifiersTable, 0, 0, pPdfRptCableID);
    pdfTableSetText(pIdentifiersTable, 0, 1, pPdfRptFiberID);
    pdfTableSetText(pIdentifiersTable, 0, 2, pPdfRptLocationA);
    pdfTableSetText(pIdentifiersTable, 0, 3, pPdfRptLocationB);
    pdfTableSetText(pIdentifiersTable, 0, 4, pPdfRptDeraction);
    pdfTableSetText(pIdentifiersTable, 1, 0, identifySetting->CableMark);
    pdfTableSetText(pIdentifiersTable, 1, 1, identifySetting->FiberMark);
    pdfTableSetText(pIdentifiersTable, 1, 2, identifySetting->Location_A);
    pdfTableSetText(pIdentifiersTable, 1, 3, identifySetting->Location_B);
    identifySetting->Deraction > 0 ? 
    pdfTableSetText(pIdentifiersTable, 1, 4, "B->A") :
    pdfTableSetText(pIdentifiersTable, 1, 4, "A->B");

    pdfTableDraw(pdfControl, pIdentifiersTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pIdentifiersTable);

    return iErr;
}

/**
* 绘制结果
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int solaPdfReportResult(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }
    char buff[128] = {0};
    PdfTable *pLinkLengthTable = NULL;
    PdfTable *pResultTable = NULL;
    SOLA_WIDGET* solaWidget = getSolaWidget();
    int linkLengthIsPass = solaWidget->solaEvents->EventTable.LinkIsPass.iIsRegionLengthPass;
    
    //链路长度信息
    pLinkLengthTable = pdfTableCreate(1, 2, pdfControl->pageEditWidth * 2 / 5);
    pdfTableSetAlignment(pLinkLengthTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pLinkLengthTable, 0, 0, 0, 2, font, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pLinkLengthTable, 0);
    pdfTableSetRowHeight(pLinkLengthTable, 0, 0, 15);
    pdfTableSetItemAlignment(pLinkLengthTable, 0, 0, 0, 2, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);

    if(linkLengthIsPass == 0)
    {
        pdfTableSetTextColor(pLinkLengthTable, 0, 1, 0, 1, 255, 0, 0);
    }
    else if(linkLengthIsPass == 1)
    {
        pdfTableSetTextColor(pLinkLengthTable, 0, 1, 0, 1, 0, 0, 255);
    }
    else
    {
        pdfTableSetTextColor(pLinkLengthTable, 0, 1, 0, 1, 0, 0, 0);
    }
    
    pdfTableSetText(pLinkLengthTable, 0, 0, pPdfRptLinkLength);
    GetUcs2ToUtf8(solaWidget->distanceText, buff);
    pdfTableSetText(pLinkLengthTable, 0, 1, buff);

    //链路链路损耗和链路光回损
    int waveCount = 0;
    int i = 0;
    int waveLength = WAVELEN_1310;

    for (i = 0; i < WAVE_NUM; i++)
    {
        if(solaWidget->solaEvents->WaveIsSetted[i] == 1)
        {
            waveCount++;
        }
    }
    pResultTable = pdfTableCreate(waveCount+1, 3, pdfControl->pageEditWidth * 3 / 5);
    pdfTableSetAlignment(pResultTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pResultTable, 0, 0, 0, 3, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pResultTable, 1, 0, waveCount+1, 3, font, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pResultTable, 1);
    pdfTableSetRowHeight(pResultTable, 0, waveCount+1, 15);
    pdfTableSetItemAlignment(pResultTable, 0, 0, 0, 3, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pResultTable, 1, 0, waveCount+1, 3, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemBackgroundColor(pResultTable, 0, 0, 0, 3, 238, 238, 238);
    pdfTableSetLineColor(pResultTable, 173, 216, 230);
    
    pdfTableSetText(pResultTable, 0, 0, pPdfRptWavelength);
    pdfTableSetText(pResultTable, 0, 1, pPdfRptLinkLoss);
    pdfTableSetText(pResultTable, 0, 2, pPdfRptLinkORL);
    for (i = 0; i < waveCount; i++)
    {
        while(!solaWidget->solaEvents->WaveIsSetted[waveLength])
        {
            waveLength++;
        }
        LINK_ISPASS *linkIsPass = &solaWidget->solaEvents->WaveEventTable[waveLength].LinkIsPass;
        switch(linkIsPass->iIsRegionLossPass)
        {
        case 0:
            pdfTableSetTextColor(pResultTable, i+1, 1, i+1, 1, 255, 0, 0);
            break;
        case 1:
            pdfTableSetTextColor(pResultTable, i+1, 1, i+1, 1, 0, 255, 0);
            break;
        case 2:
            pdfTableSetTextColor(pResultTable, i+1, 1, i+1, 1, 0, 0, 0);
            break;
        }

        switch(linkIsPass->iIsRegionORLPass)
        {
        case 0:
            pdfTableSetTextColor(pResultTable, i+1, 2, i+1, 2, 255, 0, 0);
            break;
        case 1:
            pdfTableSetTextColor(pResultTable, i+1, 2, i+1, 2, 0, 255, 0);
            break;
        case 2:
            pdfTableSetTextColor(pResultTable, i+1, 2, i+1, 2, 0, 0, 0);
            break;
        }

        float totalLoss = SolaEvents_GetTotalLoss(solaWidget->solaEvents, waveLength);
        float totalReturnLoss = solaWidget->solaEvents->TotalReturnLoss[waveLength]>0.0f
                                ?solaWidget->solaEvents->TotalReturnLoss[waveLength]
                                :solaWidget->solaEvents->TotalReturnLoss[waveLength]*(-1.0f);
        switch(waveLength)
        {
        case WAVELEN_1310:
            pdfTableSetText(pResultTable, i+1, 0, "1310");
            break;
        case WAVELEN_1550:
            pdfTableSetText(pResultTable, i+1, 0, "1550");
            break;
        case WAVELEN_1625:
            pdfTableSetText(pResultTable, i+1, 0, "1625");
            break;
        default:
            break;
        }

        char* temp = Float2StringUnit(LOSS_PRECISION, totalLoss, " dB");
        sprintf(buff, "%s", temp);
        GuiMemFree(temp);
        pdfTableSetText(pResultTable, i+1, 1, buff);
        char* temp1 = Float2StringUnit(ORL_PRECISION, totalReturnLoss, " dB");
        sprintf(buff, "%s", temp1);
        GuiMemFree(temp1);
        pdfTableSetText(pResultTable, i+1, 2, buff);
        waveLength++;
    }

    pdfTableDraw(pdfControl, pLinkLengthTable);
    pdfTableDraw(pdfControl, pResultTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pLinkLengthTable);
    pdfTableFree(pResultTable);

    return iErr;
}

//截取链路信息
static void solaPdfReportScreenShot()
{
    char cPicturePath[128] = {0};
    strcpy(cPicturePath, "/app/pdf_sola_link.jpg");
	ScreenShotsJpg(30, 30+624, 65, 65+26, cPicturePath);
}

//获得指定事件的图片和位置
static int solaPdfReportGetEventPicture(SOLA_WIDGET* solaWidget, int eventNumber, char *picturePath)
{
    if(!solaWidget || !picturePath)
    {
        return -1;
    }

    if(eventNumber >= solaWidget->itemCount)
    {
        return -2;
    }
    
    SOLA_WIDGET_ITEM *item = solaWidget->items[eventNumber];
    char bitmapFile[256] = {0};
    strcpy(bitmapFile, item->icon->pPicBitmap->strBitmapFile);
    
    char *temp = strstr(bitmapFile, "unchecked");
    if(temp)
    {
        strcpy(picturePath, bitmapFile);
    }
    else
    {
        temp = strstr(bitmapFile, "checked");
        if(temp)
        {
            *(temp-1) = 0;
            sprintf(picturePath, "%s_un%s", bitmapFile, temp);
        }
        else
        {
            return -3;
        }
    }
    
    temp = strstr(picturePath, ".bmp");
    strcpy(temp, ".jpg");

    return 0;
}

static int GetEventPoisitionInfo(SOLA_WIDGET* solaWidget, int index, char *position, int iSectionFlag)
{
    if(!solaWidget || !position)
    {
        return -1;
    }

    if(index >= solaWidget->itemCount)
    {
        return -2;
    }

    float distance = iSectionFlag ? (solaWidget->items[index+1]->value - solaWidget->items[index]->value)
                                    : solaWidget->items[index]->value;
    char marker[5] = {0};
    iSectionFlag ? memset(marker, 0, 5)
                 : strcpy(marker, solaWidget->items[index]->marker);

    char* buff = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_SOLA, distance, 0);
    sprintf(position, "%s%s", buff, marker);
    GuiMemFree(buff);
    
    return 0;
}

/**
* 绘制sola链路视图
* @param[in] pdfControl 
* @param[in] 
* @param[in] 
* @return 
* @note 有关此函数的特别说明：…
*/
static int solaPdfReportLinkView(PdfControl *pdfControl)
{
    int iErr = 0;

    if (!pdfControl)
    {
        return -1;
    }
    SOLA_WIDGET* solaWidget = getSolaWidget();
    const float viewHeight = 90;                                                    //<!链路视图高度
    float viewWidth = pdfControl->pageEditWidth;                                    //<!链路视图宽度
    float pictureWidth = 39;                                                        //<!事件图片宽度
    float pictureHeight = 54;                                                       //<!事件图片高度
    float lineLength = 10;                                                          //<!事件与事件之间光纤链路长度
    float lineWidth = 2;                                                            //<!事件与事件之间光纤链路宽度
    float fontSize = 7;                                                             //<!字体大小
    float offset = 10;                                                              //<!第一个事件图标与链路视图
    float viewPosition = 35;
    int eventCount = solaWidget->itemCount;
    int num = eventCount + 2;
    float scalingRatio = viewWidth / (offset * 2 + num * (pictureWidth + lineLength) - lineLength);
    int i = 0;
    PdfPainter painter = pdfGetPainter(pdfControl, viewHeight + fontSize);

    if (scalingRatio >= 1.0)
    {
        scalingRatio = 1.0;
        lineLength = (viewWidth - offset * 2 - num * pictureWidth) / (num - 1);
    }

    offset *= scalingRatio;
    pictureWidth *= scalingRatio;
    pictureHeight *= scalingRatio;
    lineLength *= scalingRatio;
    lineWidth *= scalingRatio;
    fontSize *= scalingRatio;
    epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
    epdf_setlinewidth(painter.page, lineWidth);

    //绘制链路缩略视图
    solaPdfReportScreenShot();
    pdfDrawImage(painter.page, "/app/pdf_sola_link.jpg", 0, painter.position, 
        pdfControl->pageEditWidth, 20);
    DelPdfJpg();
    //char* pUnit[] = {"km", "kft", "mile"};
    char* pUnit = GetCurrSystemUnitString(MODULE_UNIT_SOLA);
    pdfDrawText(painter.page, offset - 5 * scalingRatio, painter.position + viewPosition - 10 * scalingRatio,
        pictureWidth + 10 * scalingRatio, fontSize, fontSize, TEXT_BOLD, PDF_TEXT_ALIGN_H_CENTER, font, pPdfRptPosition);
    pdfDrawText(painter.page, offset+eventCount*(pictureWidth+lineLength)+pictureWidth, 
        painter.position + viewPosition - 10 * scalingRatio, pictureWidth + 10 * scalingRatio,
        fontSize, fontSize, TEXT_BOLD, PDF_TEXT_ALIGN_H_CENTER, font, pUnit);
    pdfDrawText(painter.page, offset - 5 * scalingRatio, painter.position + viewPosition + pictureHeight,
        pictureWidth + 10 * scalingRatio, fontSize, fontSize, TEXT_BOLD, PDF_TEXT_ALIGN_H_CENTER, font, pPdfRptLength);
    pdfDrawText(painter.page, offset + eventCount*(pictureWidth + lineLength)+pictureWidth,
        painter.position + viewPosition + pictureHeight, pictureWidth + 10 * scalingRatio, fontSize,
        fontSize, TEXT_BOLD, PDF_TEXT_ALIGN_H_CENTER, font, pUnit);
    GuiMemFree(pUnit);
    offset += pictureWidth;
    //链路图
    for (i = 0; i < eventCount; i++)
    {
        char picturePath[256] = {0};
        char position[128] = {0};
        char sectionPosition[128] = {0};
        solaPdfReportGetEventPicture(solaWidget, i, picturePath);
        GetEventPoisitionInfo(solaWidget, i, position, 0);
        if (i != eventCount - 1)
        {
            GetEventPoisitionInfo(solaWidget, i, sectionPosition, 1);
        }
        pdfDrawImage(painter.page, picturePath, offset, painter.position + viewPosition, 
            pictureWidth, pictureHeight);
        pdfDrawText(painter.page, offset - 5 * scalingRatio, painter.position + viewPosition - 10 * scalingRatio,
            pictureWidth + 10 * scalingRatio, fontSize, fontSize, TEXT_BOLD,PDF_TEXT_ALIGN_H_CENTER, font, position);
        offset += pictureWidth;
        if (i != eventCount - 1)
        {
            float y = painter.position + viewPosition + pictureHeight / 2 - scalingRatio;
            pdfDrawLine(painter.page, offset, y, offset + lineLength, y);
            offset += lineLength;
        }
        pdfDrawText(painter.page, offset - lineLength / 2 - 5 * scalingRatio - pictureWidth/2, painter.position + viewPosition + pictureHeight,
            pictureWidth + 10 * scalingRatio, fontSize, fontSize, TEXT_BOLD, PDF_TEXT_ALIGN_H_CENTER, font, sectionPosition);
    }

    return iErr;
}

//画曲线直线
static void curveDrawLine(PdfPainter painter, float xStart, int yStart, int width, int height,
    float lineWidth, float colorR, float colorG, float colorB)
{
    epdf_setrgbstroke(painter.page, COLOR_RGB(colorR), COLOR_RGB(colorG), COLOR_RGB(colorB));
    epdf_setlinewidth(painter.page, lineWidth);
    pdfDrawLine(painter.page, xStart, painter.position + yStart,
        xStart + width, painter.position + yStart + height);
}

//写曲线文字
static void curveDrawText(PdfPainter painter, char* text, int fontSize,
    int xOffset, int yOffset, int width, int height, PDF_TEXT_HORIZONTAL_ALIGNMENT alignment)
{
    pdfDrawText(painter.page, xOffset, painter.position + yOffset, width, height,
        fontSize, TEXT_NORMAL, alignment, font, text);
}

/**
* SOLA 画OTDR曲线坐标轴
* @param[in] painter
* @param[in] 边距
* @param[in] 宽度
* @param[in] 高度
* @param[in] x轴范围
* @param[in] y轴范围
* @param[in] x轴距离单位
* @return
* @note 有关此函数的特别说明：…
*/
static void drawCurveAxisScale(PdfPainter painter, int offset, int viewWidth, int height
                                , float xRange, float yRange, char* xUnit)
{                                  //<!宽度
    epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
    epdf_setlinewidth(painter.page, 1);

    //画矩形
    pdfDrawLine(painter.page, 0, painter.position + offset, viewWidth, painter.position + offset);
    pdfDrawLine(painter.page, 0, painter.position + offset, 0, painter.position + height - offset);
    pdfDrawLine(painter.page, viewWidth, painter.position + offset, viewWidth, painter.position + height - offset);
    pdfDrawLine(painter.page, 0, painter.position + height - offset, viewWidth, painter.position + height - offset);

    //画y轴坐标轴
    curveDrawLine(painter, 55, offset + 5, 0, 160, 0.2, 0, 0, 0);
    curveDrawText(painter, "dB", 8, 7, 77, 10, 20, PDF_TEXT_ALIGN_LEFT);
    int i;
    for (i = 0; i < 41; i++)
    {
        if (i % 5 == 0)
            curveDrawLine(painter, 50, offset + 5 + i * 4, 5, 0, 0.2, 0, 0, 0);
        else
            curveDrawLine(painter, 52, offset + 5 + i * 4, 3, 0, 0.2, 0, 0, 0);

        if (i % 5 == 0 && i != 40)
        {
            curveDrawLine(painter, 55, offset + 5 + i * 4, 500, 0, 1, 215, 235, 255);
        }
    }
    float yPer = yRange / 8;
    for (i = 0; i < 9; i++)
    {
        float yVal = yPer * i;
        char yAxis[10] = { 0 };
        sprintf(yAxis, "%.2f", yVal);
        curveDrawText(painter, yAxis, 8, 20, 165 - i * 20, 30, 10, PDF_TEXT_ALIGN_RIGHT);
    }

    //画x轴坐标
    curveDrawLine(painter, 55, offset + 165, 500, 0, 0.2, 0, 0, 0);
    for (i = 1; i < 51; i++)
    {
        if (i % 5 == 1)
            curveDrawLine(painter, 55 + i * 10, offset + 165, 0, 5, 0.2, 0, 0, 0);
        else
            curveDrawLine(painter, 55 + i * 10, offset + 165, 0, 3, 0.2, 0, 0, 0);

        if (i % 5 == 1 || i == 50)
        {
            curveDrawLine(painter, 55 + i * 10, offset + 5, 0, 160, 1, 215, 235, 255);
        }
    }
    float xPer = xRange / 10;
    for (i = 0; i < 10; i++)
    {
        float xVal = xPer * i;
        char* xAxis = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_SOLA, xVal, 0);
        curveDrawText(painter, xAxis, 8, 49 + i * 50, offset + 170, 50, 10, PDF_TEXT_ALIGN_H_CENTER);
        GuiMemFree(xAxis);
    }
    curveDrawText(painter, xUnit, 8, 40 + 10 * 50, offset + 170, 30, 10, PDF_TEXT_ALIGN_H_CENTER);
    if (s_solaDataList->waveNum > 1)
    {
        //曲线数量为两条
        epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
        epdf_setlinewidth(painter.page, 0.5);
        pdfDrawLine(painter.page, 495, painter.position + offset + 5, 555, painter.position + offset + 5);
        pdfDrawLine(painter.page, 495, painter.position + offset + 5, 495, painter.position + offset + 25);
        pdfDrawLine(painter.page, 555, painter.position + offset + 5, 555, painter.position + offset + 25);
        pdfDrawLine(painter.page, 495, painter.position + offset + 25, 555, painter.position + offset + 25);
        curveDrawLine(painter, 535, offset + 10, 10, 0, 0.2, 0, 0, 0);
        curveDrawText(painter, "1310 nm", 8, 505, offset + 5, 30, 10, PDF_TEXT_ALIGN_H_CENTER);
        curveDrawLine(painter, 535, offset + 20, 10, 0, 0.2, 255, 0, 0);
        curveDrawText(painter, "1550 nm", 8, 505, offset + 15, 30, 10, PDF_TEXT_ALIGN_H_CENTER);
    }
}

/**
* SOLA 画OTDR曲线曲线
* @param[in] painter
* @param[in] 边距
* @param[in] 数据
* @param[in] 个数
* @param[in] 点与点之间的间距
* @param[in] x轴范围
* @param[in] y轴范围
* @param[in] 事件列表
* @return
* @note 有关此函数的特别说明：…
*/
static void drawCurve(PdfPainter painter, int offset, short *data, int count, float onePoint
    , float maxX, float maxY, int index)
{
    //x轴最大490个像素 每个像素代表的距离
    float onePixLen = maxX / 490;
    //y轴最大160个像素 每个像素代表的dB
    float onePixDB = maxY / 160 * 500;
    //画曲线
    int i;
    //int x = 1;
    //int beginYPos;
    for (i = 1; i < 490; i++)
    {
        int prevOffsetIndex = round(onePixLen * (i - 1) / onePoint);
        int offsetIndex = round(onePixLen * i / onePoint);
        if (offsetIndex < count)
        {
            int beginYPos = round(*(data + prevOffsetIndex) / onePixDB);
            int endYPos = round(*(data + offsetIndex) / onePixDB);
            if (beginYPos < 0)
                beginYPos = 0;
            if (endYPos < 0)
                endYPos = 0;
            if (index == 0)
                curveDrawLine(painter, 63 + i, offset + 165 - beginYPos, 1, beginYPos - endYPos, 0.1, 0, 0, 0);
            else
                curveDrawLine(painter, 63 + i, offset + 165 - beginYPos, 1, beginYPos - endYPos, 0.1, 255, 0, 0);
        }
    }
}

static void generateSolaCurveData()
{
    if (s_solaDataList == NULL)
    {
        int waveNum = GetSolaCurveWaveNum();
        s_solaDataList = (SOLA_DATA_LIST*)malloc(sizeof(SOLA_DATA_LIST));
        s_solaDataList->solaData = (SOLA_DATA*)malloc(waveNum * sizeof(SOLA_DATA));
        s_solaDataList->waveNum = waveNum;
        SOLA_DATA* solaCurveData = GetSolaCurveData();
        int i,j;
        for (i = 0; i < waveNum; i++)
        {
            SOLA_DATA* solaData = solaCurveData + i;
            SOLA_DATA* newSolaData = s_solaDataList->solaData + i;
            newSolaData->onePoint = solaData->onePoint;
            newSolaData->wave = solaData->wave;
            newSolaData->count = solaData->count;
            newSolaData->data = (short*)malloc(solaData->count * sizeof(short));
            for (j = 0; j < newSolaData->count; j++)
            {
                *(newSolaData->data + j) = *(solaData->data + j);
            }
        }

    }
}
/**
* SOLA 画OTDR曲线
* @param[in] pdfControl
* @param[in] SOLA采集数据
* @param[in] SOLA数据个数
* @param[in] 点与点之间的距离
* @param[in] 距离单位
* @param[in] 事件列表
* @param[in] SOLA数据个数
* @return
* @note 有关此函数的特别说明：…
*/
static int pdfDrawCurve(PdfControl *pdfControl)
{
    int iErr = 0;

    if (!pdfControl)
    {
        return -1;
    }
    int viewWidth = pdfControl->pageEditWidth;
    PdfPainter painter = pdfGetPainter(pdfControl, 190);
    generateSolaCurveData();
    //求x轴最大范围 能被10整除 单位m
    SOLA_DATA* solaData = s_solaDataList->solaData;
    float len = solaData->count * solaData->onePoint;
    //求y轴最大范围 能被8整除 单位db
    float maxY = 0.0f;
    int i,j;
    for (i = 0; i < s_solaDataList->waveNum; i++)
    {
        solaData = s_solaDataList->solaData + i;
        for (j = 0; j < solaData->count; j++)
        {
            if (maxY < *(solaData->data + j))
                maxY = *(solaData->data + j);
        }
    }
    float yAxis = (maxY / 500.0) + 8;//1px代表的db
    drawCurveAxisScale(painter, 5, viewWidth, 190, len, yAxis, GetCurrSystemUnitString(MODULE_UNIT_SOLA));
    for (i = 0; i < s_solaDataList->waveNum;i++)
    {
        solaData = s_solaDataList->solaData + i;
        drawCurve(painter, 5, solaData->data, solaData->count, solaData->onePoint
            , len, yAxis, i);//m
    }
    SOLA_WIDGET* solaWidget = getSolaWidget();
    SolaEvents* solaEvents = solaWidget->solaEvents;
    int itemCount = solaWidget->itemCount;
    //画事件
    float onePixDB = yAxis / 160 * 500;
    int count = 0;
    for (i = 0; i < itemCount; i++)
    {
        const EVENTS_INFO *eventInfo = SolaEvents_GetEvent(solaEvents, i);
        float pos = eventInfo->fEventsPosition;
        if(pos > len)//事件点位置超出量程，不显示事件点
            continue;
        char num[10] = { 0 };
        double lenPix = pos / (len) * 490;
        int dbPix = 0.0f;
        int differDbPix = 0.0f;
        for (j = 0; j < s_solaDataList->waveNum; j++)
        {
            solaData = s_solaDataList->solaData + j;
            int db = round(*(solaData->data + (int)(pos / solaData->onePoint)) / onePixDB);
            if (dbPix >= 0.1)
                differDbPix = fabsf(dbPix - db);
            if (db > dbPix)
                dbPix = db;
        }
        //计算事件点的标记
        sprintf(num, "%d", count + 1);
        count++;
        curveDrawLine(painter, 64 + lenPix, 165 - dbPix, 0, 10 + differDbPix, 0.4, 255, 0, 0);
        curveDrawText(painter, num, 8, 59 + lenPix, 175 - dbPix + differDbPix, 10, 10, PDF_TEXT_ALIGN_H_CENTER);
    }

    SolaData_Destroy();
    
    return iErr;
}
/**
* 得到事件类型
* @param[in] iEventStyle 输入的整形事件类型
* @param[out] cEventStyle 输出类型名
* @return
* @note 有关此函数的特别说明:
*/
static int solaPdfReportGetType(int iEventStyle, char *cEventStyle)
{
    if(!cEventStyle)
    {
        return -1;
    }
    int normalEvent = iEventStyle & 0x0000ffff;
    int splitterEvent = (iEventStyle & 0xffff0000) >> 16;
    switch(splitterEvent)
    {
    case SPLITTER1_2:
    strcpy(cEventStyle, pPdfRptSplitter1_2);
    return 0;
    case SPLITTER1_4:
    strcpy(cEventStyle, pPdfRptSplitter1_4);
    return 0;
    case SPLITTER1_8:
    strcpy(cEventStyle, pPdfRptSplitter1_8);
    return 0;
    case SPLITTER1_16:
    strcpy(cEventStyle, pPdfRptSplitter1_16);
    return 0;
    case SPLITTER1_32:
    strcpy(cEventStyle, pPdfRptSplitter1_32);
    return 0;
    case SPLITTER1_64:
    strcpy(cEventStyle, pPdfRptSplitter1_64);
    return 0;
    case SPLITTER1_128:
    strcpy(cEventStyle, pPdfRptSplitter1_128);
    return 0;
    default:
    break;
    }
    
    switch(normalEvent)
    {
    case 0:
    case 1:
    case 10:
    case 11:
    strcpy(cEventStyle, pPdfRptSplice);
    return 0;
    case 2:
    case 3:
    case 4:
    case 12:
    case 13:
    case 14:
    strcpy(cEventStyle, pPdfRptConnector);
    return 0;
    case 5:
    case 15:
    strcpy(cEventStyle, pPdfRptMacrobend);
    return 0;
    case 21:
    strcpy(cEventStyle, pPdfRptContinuousFiber);   //光功率不足
    return 0;
    default:
    break;
    }
    
    return -2;
}

/**
* 将结果转换为颜色
* @param[in] isPass 事件信息
* @param[out] Color 损耗文本的颜色0-7位 B， 8-15位 G， 16-23位 R
* @return
* @note 有关此函数的特别说明:
*/
static void solaPdfReportResultColor(int isPass, unsigned int *color)
{
    if(!color)
    {
        return;
    }
    unsigned int temp = 0xff;

    if(isPass == 0)
    {
        *color = temp << 16;
    }
    else if(isPass == 1)
    {
        *color = temp << 8;
    }
    else
    {
        *color = 0;
    }

    return;
}

/**
* 得到损耗和反射率的值和颜色
* @param[in] eventInfo 事件信息
* @param[in] index 事件信息
* @param[in] wave 波长
* @param[out] loss
* @param[out] reflect 
* @param[out] lossColor                 //损耗文本的颜色0-7位 B， 8-15位 G， 16-23位 R
* @param[out] reflectColor 
* @return
* @note 有关此函数的特别说明:
*/
static void solaPdfReportGetLossAndReflect(SolaEvents *solaEvents, int index, int wave, char *loss, char *reflect, 
    unsigned int *lossColor, unsigned int *reflectColor)
{
    if(!solaEvents)
    {
        return;
    }
    EVENTS_INFO *eventInfo = SolaEvents_GetWaveEvent(solaEvents, index, wave);
    if(eventInfo == NULL)
    {
        goto __none;
    }

    EVENTS_ISPASS *eventIsPass = &solaEvents->WaveEventTable[wave].EventsIsPass[index];
    int normalEvent = eventInfo->iStyle & 0x0000ffff;
    int splitterEvent = (eventInfo->iStyle & 0xffff0000) >> 16; 
    
    if((normalEvent < 10) && (solaEvents->EventTable.iEventsNumber == index + 1 || index == 0))
    {
        normalEvent += 10;   
    }

    switch(splitterEvent)
    {
    case SPLITTER1_2:
    case SPLITTER1_4:
    case SPLITTER1_8:
    case SPLITTER1_16:
    case SPLITTER1_32:
    case SPLITTER1_64:
    case SPLITTER1_128:
        if(normalEvent >= 10 && normalEvent < 20)
        {
            goto __reflectOnly;
        }
        else
        {
            goto __lossAndReflect;
        }
    default:
        break;
    }
        
    switch(normalEvent)
    {
    case 0:
    case 1:
        goto __lossOnly;
    case 2:
    case 3:
    case 4:
        goto __lossAndReflect;
    case 5:
        goto __lossOnly;
    case 10:
    case 11:
    case 15:
        goto __none;
    case 12:
    case 13:
    case 14:
        goto __reflectOnly;
    case 21:
        goto __none;
    default:
        return;
    }

__lossAndReflect:
    if(loss)
    {
        //sprintf(loss, "%.3f", eventInfo->fLoss);
        char* temp = Float2String(LOSS_PRECISION, eventInfo->fLoss);
        sprintf(loss, "%s", temp);
        GuiMemFree(temp);
        solaPdfReportResultColor(eventIsPass->iIsEventLossPass, lossColor);
    }
    if(reflect)
    {
        //sprintf(reflect, "%.3f", eventInfo->fReflect);
        char* temp = Float2String(REFLECTANCE_PRECISION, eventInfo->fReflect);
        sprintf(reflect, "%s", temp);
        GuiMemFree(temp);
        solaPdfReportResultColor(eventIsPass->iIsReflectPass, reflectColor);
    }
    return;

__lossOnly:
    if(loss)
    {
        //sprintf(loss, "%.3f", eventInfo->fLoss);
        char* temp = Float2String(LOSS_PRECISION, eventInfo->fLoss);
        sprintf(loss, "%s", temp);
        GuiMemFree(temp);
        solaPdfReportResultColor(eventIsPass->iIsEventLossPass, lossColor);
    }
    if(reflect)
    {
        strcpy(reflect, "--");
        *reflectColor = 0;
    }
    return;

__reflectOnly:
    if(loss)
    {
        strcpy(loss, "--");
        *lossColor = 0;
    }
    if(reflect)
    {
        //sprintf(reflect, "%.3f", eventInfo->fReflect);
        char* temp = Float2String(REFLECTANCE_PRECISION, eventInfo->fReflect);
        sprintf(reflect, "%s", temp);
        GuiMemFree(temp);
        solaPdfReportResultColor(eventIsPass->iIsReflectPass, reflectColor);
    }
    return;

__none:
    if(loss)
    {
        strcpy(loss, "--");
        *lossColor = 0;
    }
    if(reflect)
    {
        strcpy(reflect, "--");
        *reflectColor = 0;
    }
    return;
}

static void SwitchEventPosition(float fDistance, char* position)
{
    if (!position)
        return;
    char* buff = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_SOLA, fDistance, 0);
    sprintf(position, "%s", buff);
    GuiMemFree(buff);
}

/**
* 绘制元素表
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int solaPdfReportElement(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    SOLA_WIDGET* solaWidget = getSolaWidget();
    SolaEvents* solaEvents = solaWidget->solaEvents;
    int launchFiberIndex = solaEvents->EventTable.iLaunchFiberIndex;
    int recvFiberIndex = solaEvents->EventTable.iRecvFiberIndex;
    float launchFiberPosition = solaEvents->EventTable.EventsInfo[launchFiberIndex].fEventsPosition;
    PdfTable *pElementTable = NULL;
    int itemCount = solaWidget->itemCount;
    int tableRow = 2 + 2 * itemCount - 1;
    int tableColumn = 3;
    int waveCount = 0;
    char *waveName[3] = {"1310 nm", "1550 nm", "1625 nm"};
    int waveList[3] = {0, 1, 2};
    
    int i=0;

    if(solaEvents->WaveIsSetted[0])
    {
        waveList[waveCount++] = 0;
    }
    
    if(solaEvents->WaveIsSetted[1])
    {
        waveList[waveCount++] = 1;
    }
    
    if(solaEvents->WaveIsSetted[2])
    {
        waveList[waveCount++] = 2;
    }

    tableColumn += waveCount * 2;

    //链路链路损耗和链路光回损
    pElementTable = pdfTableCreate(tableRow, tableColumn, pdfControl->pageEditWidth);
    pdfTableSetAlignment(pElementTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pElementTable, 0, 0, 1, tableColumn, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pElementTable, 2, 0, tableRow, tableColumn, font, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pElementTable, 1);
    pdfTableSetRowHeight(pElementTable, 0, tableRow, 15);
    pdfTableSetItemAlignment(pElementTable, 0, 0, tableRow, tableColumn, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetItemBackgroundColor(pElementTable, 0, 0, 1, tableColumn, 238, 238, 238);
    pdfTableSetLineColor(pElementTable, 173, 216, 230);

    pdfTableMergeCells(pElementTable, 0, 0, 1, 0);
    pdfTableMergeCells(pElementTable, 0, 1, 1, 1);
    pdfTableMergeCells(pElementTable, 0, 2, 1, 2);
    if(waveCount > 1)
    {
        pdfTableMergeCells(pElementTable, 0, 3, 0, 2+waveCount);
        pdfTableMergeCells(pElementTable, 0, 3+waveCount, 0, 2 + waveCount*2);
    }

    for(i = 0; i < tableColumn; i++)
    {
        pdfTableSetColumnWidth(pElementTable, i, i == 1 ? 0.06 : (0.94 / (tableColumn - 1)));
    }

    for(i = 2; i < tableRow; i++)
    {
        i % 2 == 0 ? pdfTableSetItemBackgroundColor(pElementTable, i, 0, i, 7, 230, 241, 255)
        : pdfTableSetItemBackgroundColor(pElementTable, i, 0, i, 7, 247, 247, 222);
    }

    pdfTableSetText(pElementTable, 0, 0, pPdfRptType);
    pdfTableSetText(pElementTable, 0, 1, pPdfRptNO);

    switch(iUnitConverterFlag)
    {
        case UNIT_M:
            pdfTableSetText(pElementTable, 0, 2, pPdfRptPosition_m);
            break;
        case UNIT_KM:
            pdfTableSetText(pElementTable, 0, 2, pPdfRptPosition_km);
            break;
        case UNIT_FT:
            pdfTableSetText(pElementTable, 0, 2, pPdfRptPosition_ft);
            break;
        case UNIT_KFT:
            pdfTableSetText(pElementTable, 0, 2, pPdfRptPosition_kft);
            break;
        case UNIT_MI:
            pdfTableSetText(pElementTable, 0, 2, pPdfRptPosition_mile);
            break;
        default:
            break;
    }
    pdfTableSetText(pElementTable, 0, 3, pPdfRptLoss);
    pdfTableSetText(pElementTable, 0, 3+waveCount, pPdfRptReflect);

    for(i = 0; i < waveCount; i++)
    {
        pdfTableSetText(pElementTable, 1, 3 + i, waveName[waveList[i]]);
        pdfTableSetText(pElementTable, 1, 3 + waveCount + i, waveName[waveList[i]]);
    }

    for(i = 0; i < itemCount; i++)
    {
        int j=0;
        char buff[512] = {0};
        float fTemp;
        const EVENTS_INFO *eventInfo = SolaEvents_GetEvent(solaEvents, i);
        if(eventInfo)
        {
            solaPdfReportGetType(eventInfo->iStyle, buff);
            pdfTableSetText(pElementTable, 2*i+2, 0, buff);
            if (i != itemCount - 1)
            {
                pdfTableSetText(pElementTable, 2*i+1+2, 0, pPdfRptSection);
            }
            
            //编号
            memset(buff, 0, sizeof(buff));
            sprintf(buff, "%d", i+1);
            pdfTableSetText(pElementTable, 2*i+2, 1, buff);
            
            
            if(launchFiberIndex > 0)
            {
                fTemp = eventInfo->fEventsPosition - launchFiberPosition;
            }
            else
            {
                fTemp = eventInfo->fEventsPosition;
            }
            //转换事件位置信息
            SwitchEventPosition(fTemp, buff);
            
            //起点
            if((launchFiberIndex > 0 && launchFiberIndex == i)
                || (launchFiberIndex <= 0 && i == 0))   
            {
                strcat(buff, "(A)");
            }
            //终点
            else if((recvFiberIndex > 0 && recvFiberIndex == i)
                || (recvFiberIndex <= 0 
                && itemCount > 1 
                && i == (itemCount - 1)))
            {
                strcat(buff, "(B)");
            }

            pdfTableSetText(pElementTable, 2*i+2, 2, buff);
            if (i != itemCount - 1)
            {
                const EVENTS_INFO *eventInfo1 = SolaEvents_GetEvent(solaEvents, i+1);
                float fDist = eventInfo1->fEventsPosition - eventInfo->fEventsPosition;
                SwitchEventPosition(fDist, buff);
                pdfTableSetText(pElementTable, 2*i+1+2, 2, buff);
            }
        }
        else
        {
            pdfTableSetText(pElementTable, i+2, 0, "--");
            pdfTableSetText(pElementTable, i+2, 2, "--");
        }

        for(j = 0; j < waveCount; j++)
        {
            int row = 2*i + 2;
            int column = j + 3;
            char loss[32] = {0};
            char reflect[32] = {0};
            unsigned int lossColor = 0;
            unsigned int reflectColor = 0;
            unsigned int R, G, B;
            int wave = 0;
            wave = waveList[j];
            solaPdfReportGetLossAndReflect(solaEvents, i, wave, loss, reflect, &lossColor, &reflectColor);
            R = (lossColor & 0x00ff0000) >> 16;
            G = (lossColor & 0x0000ff00) >> 8;
            B = (lossColor & 0x000000ff);
            pdfTableSetTextColor(pElementTable, row, column, row, column, R, G, B);
            R = (reflectColor & 0x00ff0000) >> 16;
            G = (reflectColor & 0x0000ff00) >> 8;
            B = (reflectColor & 0x000000ff);
            pdfTableSetTextColor(pElementTable, row, column+waveCount, row, column+waveCount, R, G, B);

            pdfTableSetText(pElementTable, row, column, loss);
            pdfTableSetText(pElementTable, row, column+waveCount, reflect);
            if (i != itemCount - 1)
            {
                //sprintf(loss, "%.3f", eventInfo->fContinueAttenuation);
                //sprintf(loss, "%.3f", solaEvents->WaveEventTable[wave].EventsInfo[i].fContinueAttenuation);
                char* temp = Float2String(LOSS_PRECISION, solaEvents->WaveEventTable[wave].EventsInfo[i].fContinueAttenuation);
                pdfTableSetText(pElementTable, row+1, column, temp);
                GuiMemFree(temp);
                pdfTableSetText(pElementTable, row+1, column+waveCount, "--");
            }
            
        }
    }
    
    pdfTableDraw(pdfControl, pElementTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pElementTable);

    return iErr;
}

/**
* 绘制链路通过未通过阈值
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int solaPdfReportLinkThresholds(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pThresholdsTable = NULL;
    SOLA_WIDGET* solaWidget = getSolaWidget();
    int rowCount = 2;
    int columnCount = 7;
    int waveCount = 0;
    char *waveName[WAVE_NUM] = {"1310", "1550", "1625"};
    int waveList[WAVE_NUM] = {0};
    int i=0;
    
    while(i < WAVE_NUM)
    {
        if(solaWidget->solaEvents->WaveIsSetted[i])
        {
            waveList[waveCount++] = i;
        }
        i++;
    }
    rowCount += waveCount;
    //链路链路损耗和链路光回损
    pThresholdsTable = pdfTableCreate(rowCount, columnCount, pdfControl->pageEditWidth);
    pdfTableSetAlignment(pThresholdsTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pThresholdsTable, 0, 0, 1, columnCount, font, 10, TEXT_BOLD);
    if(0 == strcmp(font, EPDF_PE))
    {
        pdfTableSetItemFontAndSize(pThresholdsTable, 0, 4, 1, 4, EPDF_EN, 10, TEXT_BOLD);
        pdfTableSetItemFontAndSize(pThresholdsTable, 0, 1, 1, 1, EPDF_UN, 10, TEXT_BOLD);
    }
    pdfTableSetItemFontAndSize(pThresholdsTable, 2, 0, rowCount, columnCount, font, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pThresholdsTable, 1);
    pdfTableSetRowHeight(pThresholdsTable, 0, rowCount, 15);
    pdfTableSetItemAlignment(pThresholdsTable, 0, 0, rowCount, columnCount, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetItemBackgroundColor(pThresholdsTable, 0, 0, 1, columnCount, 238, 238, 238);
    pdfTableSetRowHeightAdaptive(pThresholdsTable, 0, rowCount - 1, 1);
    pdfTableSetLineColor(pThresholdsTable, 173, 216, 230);

    pdfTableSetColumnWidth(pThresholdsTable, 0, 0.38);
    pdfTableSetColumnWidth(pThresholdsTable, 1, 0.12);
    pdfTableSetColumnWidth(pThresholdsTable, 2, 0.1);
    pdfTableSetColumnWidth(pThresholdsTable, 3, 0.1);
    pdfTableSetColumnWidth(pThresholdsTable, 4, 0.1);
    pdfTableSetColumnWidth(pThresholdsTable, 5, 0.1);
    pdfTableSetColumnWidth(pThresholdsTable, 6, 0.1);

    pdfTableMergeCells(pThresholdsTable, 0, 0, 1, 0);
    pdfTableMergeCells(pThresholdsTable, 0, 1, 1, 1);
    pdfTableMergeCells(pThresholdsTable, 0, 2, 0, 3);
    pdfTableMergeCells(pThresholdsTable, 0, 4, 1, 4);
    pdfTableMergeCells(pThresholdsTable, 0, 5, 0, 6);

    pdfTableSetRowHeightAdaptive(pThresholdsTable, 0, 1, 1);
    pdfTableSetText(pThresholdsTable, 0, 1, pPdfRptWave);
    pdfTableSetText(pThresholdsTable, 0, 2, pPdfRptLinkLoss);
    pdfTableSetText(pThresholdsTable, 0, 4, pPdfRptMaxORL);

    switch(iUnitConverterFlag)
    {
        case UNIT_M:
            pdfTableSetText(pThresholdsTable, 0, 5, pPdfRptLinkLength_m);
            break;
        case UNIT_KM:
            pdfTableSetText(pThresholdsTable, 0, 5, pPdfRptLinkLength_km);
            break;
        case UNIT_FT:
            pdfTableSetText(pThresholdsTable, 0, 5, pPdfRptLinkLength_ft);
            break;
        case UNIT_KFT:
            pdfTableSetText(pThresholdsTable, 0, 5, pPdfRptLinkLength_kft);
            break;
        case UNIT_MI:
            pdfTableSetText(pThresholdsTable, 0, 5, pPdfRptLinkLength_mile);
            break;
        default:
            break;
    }
    pdfTableSetText(pThresholdsTable, 1, 2, pPdfRptMin);
    pdfTableSetText(pThresholdsTable, 1, 3, pPdfRptMax);
    pdfTableSetText(pThresholdsTable, 1, 5, pPdfRptMin);
    pdfTableSetText(pThresholdsTable, 1, 6, pPdfRptMax);

    char buff[32] = {0};
    for(i = 0; i < waveCount; i++)
    {
        SOLA_MAX_MIN_SET *linkLength = &pSolaMessager->linePassThreshold.lineLength;
        SOLA_MAX_MIN_SET *lineLoss = &pSolaMessager->linePassThreshold.lineWave[waveList[i]].lineLoss;
        SOLA_MAX_MIN_SET *lineReturnLoss = &pSolaMessager->linePassThreshold.lineWave[waveList[i]].lineReturnLoss;

        pdfTableSetText(pThresholdsTable, 2+i, 0, pPdfRptUserDefinePassFailThresholds);
        pdfTableSetText(pThresholdsTable, 2+i, 1, waveName[waveList[i]]);
        if(lineLoss->type == NOLYMIN || lineLoss->type == MINANDMAX)
        {
            char* temp = Float2String(LOSS_PRECISION, lineLoss->min);
            sprintf(buff, "%s", temp);
            GuiMemFree(temp);
            //sprintf(buff, "%.2f", lineLoss->min);
        }
        else
        {
            sprintf(buff, "--");
        }
        pdfTableSetText(pThresholdsTable, 2+i, 2, buff);
        if(lineLoss->type == NOLYMAX || lineLoss->type == MINANDMAX)
        {
            char* temp = Float2String(LOSS_PRECISION, lineLoss->max);
            sprintf(buff, "%s", temp);
            GuiMemFree(temp);
            //sprintf(buff, "%.2f", lineLoss->max);
        }
        else
        {
            sprintf(buff, "--");
        }
        pdfTableSetText(pThresholdsTable, 2+i, 3, buff);
        if(lineReturnLoss->type == NOLYMAX)
        {
            char* temp = Float2String(ORL_PRECISION, lineReturnLoss->max);
            sprintf(buff, "%s", temp);
            GuiMemFree(temp);
            //sprintf(buff, "%.2f", lineReturnLoss->max);
        }
        else
        {
            sprintf(buff, "--");
        }
        pdfTableSetText(pThresholdsTable, 2+i, 4, buff);


        if(linkLength->type == NOLYMIN || linkLength->type == MINANDMAX)
        {
            char* temp = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, 1000* pSolaMessager->linePassThreshold.lineLength.min,
                                                                    pSolaMessager->linePassThreshold.lineLength_ft.min,
                                                                    pSolaMessager->linePassThreshold.lineLength_mile.min, 0);
            sprintf(buff, "%s", temp);
            GuiMemFree(temp);
            //sprintf(buff, "%.2f", linkLength->min);
        }
        else
        {
            sprintf(buff, "--");
        }
        pdfTableSetText(pThresholdsTable, 2+i, 5, buff);
        if(linkLength->type == NOLYMAX || linkLength->type == MINANDMAX)
        {
            char* temp = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, 1000* pSolaMessager->linePassThreshold.lineLength.max,
                                                                    pSolaMessager->linePassThreshold.lineLength_ft.max,
                                                                    pSolaMessager->linePassThreshold.lineLength_mile.max, 0);
            sprintf(buff, "%s", temp);
            GuiMemFree(temp);
            //sprintf(buff, "%.2f", linkLength->max);
        }
        else
        {
            sprintf(buff, "--");
        }
        pdfTableSetText(pThresholdsTable, 2+i, 6, buff);
        /*
        if(iUnitConverterFlag == 0)
        {
            if(linkLength->type == NOLYMIN || linkLength->type == MINANDMAX)
            {
                sprintf(buff, "%.2f", linkLength->min);
            }
            else
            {
                sprintf(buff, "--");
            }
            pdfTableSetText(pThresholdsTable, 2+i, 5, buff);
            if(linkLength->type == NOLYMAX || linkLength->type == MINANDMAX)
            {
                sprintf(buff, "%.2f", linkLength->max);
            }
            else
            {
                sprintf(buff, "--");
            }
            pdfTableSetText(pThresholdsTable, 2+i, 6, buff);
        }
        else if(iUnitConverterFlag == 1)
        {
            if(linkLength->type == NOLYMIN || linkLength->type == MINANDMAX)
            {
                sprintf(buff, "%.2f", linkLength->min/1.609);
            }
            else
            {
                sprintf(buff, "--");
            }
            pdfTableSetText(pThresholdsTable, 2+i, 5, buff);
            if(linkLength->type == NOLYMAX || linkLength->type == MINANDMAX)
            {
                sprintf(buff, "%.2f", linkLength->max/1.609);
            }
            else
            {
                sprintf(buff, "--");
            }
            pdfTableSetText(pThresholdsTable, 2+i, 6, buff);
        }
        else
        {
            if(linkLength->type == NOLYMIN || linkLength->type == MINANDMAX)
            {
                sprintf(buff, "%.2f", linkLength->min*3.28);
            }
            else
            {
                sprintf(buff, "--");
            }
            pdfTableSetText(pThresholdsTable, 2+i, 5, buff);
            if(linkLength->type == NOLYMAX || linkLength->type == MINANDMAX)
            {
                sprintf(buff, "%.2f", linkLength->max*3.28);
            }
            else
            {
                sprintf(buff, "--");
            }
            pdfTableSetText(pThresholdsTable, 2+i, 6, buff);
        }*/
    }

    pdfTableDraw(pdfControl, pThresholdsTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pThresholdsTable);

    return iErr;
}

/**
* 绘制元素通过/未通过阈值
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int solaPdfReportElementThresholds(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pThresholdsTable = NULL;
    SOLA_WIDGET* solaWidget = getSolaWidget();
    SOLA_ITEM_PASSTHRESHOLD *itemPassThreshold = &pSolaMessager->itemPassThreshold;
    float tableWidth = 0.0f;
    int rowCount = 12;
    int columnCount = 1;
    int waveCount = 0;
    char *waveName[WAVE_NUM] = {"1310 nm", "1550 nm", "1625 nm"};
    int waveList[WAVE_NUM] = {0};
    float threshold[WAVE_NUM][11];
    int selectedFlag[WAVE_NUM][11];
    int i=0;
    
    while(i < WAVE_NUM)
    {
        if(solaWidget->solaEvents->WaveIsSetted[i])
        {
            waveList[waveCount++] = i;
        }
        i++;
    }
    
    threshold[WAVELEN_1310][0] = itemPassThreshold->maxSpliceLoss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][1] = itemPassThreshold->maxLinkerLoss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][2] = itemPassThreshold->maxLinkerReflectance.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][3] = itemPassThreshold->maxSplitter2Loss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][4] = itemPassThreshold->maxSplitter4Loss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][5] = itemPassThreshold->maxSplitter8Loss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][6] = itemPassThreshold->maxSplitter16Loss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][7] = itemPassThreshold->maxSplitter32Loss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][8] = itemPassThreshold->maxSplitter64Loss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][9] = itemPassThreshold->maxSplitter128Loss.fValue[WAVELEN_1310];
    threshold[WAVELEN_1310][10] = itemPassThreshold->maxSplitterReflectance.fValue[WAVELEN_1310];

    selectedFlag[WAVELEN_1310][0] = itemPassThreshold->maxSpliceLoss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][1] = itemPassThreshold->maxLinkerLoss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][2] = itemPassThreshold->maxLinkerReflectance.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][3] = itemPassThreshold->maxSplitter2Loss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][4] = itemPassThreshold->maxSplitter4Loss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][5] = itemPassThreshold->maxSplitter8Loss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][6] = itemPassThreshold->maxSplitter16Loss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][7] = itemPassThreshold->maxSplitter32Loss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][8] = itemPassThreshold->maxSplitter64Loss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][9] = itemPassThreshold->maxSplitter128Loss.iSelectedFlag[WAVELEN_1310];
    selectedFlag[WAVELEN_1310][10] = itemPassThreshold->maxSplitterReflectance.iSelectedFlag[WAVELEN_1310];
    
    threshold[WAVELEN_1550][0] = itemPassThreshold->maxSpliceLoss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][1] = itemPassThreshold->maxLinkerLoss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][2] = itemPassThreshold->maxLinkerReflectance.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][3] = itemPassThreshold->maxSplitter2Loss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][4] = itemPassThreshold->maxSplitter4Loss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][5] = itemPassThreshold->maxSplitter8Loss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][6] = itemPassThreshold->maxSplitter16Loss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][7] = itemPassThreshold->maxSplitter32Loss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][8] = itemPassThreshold->maxSplitter64Loss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][9] = itemPassThreshold->maxSplitter128Loss.fValue[WAVELEN_1550];
    threshold[WAVELEN_1550][10] = itemPassThreshold->maxSplitterReflectance.fValue[WAVELEN_1550];

    selectedFlag[WAVELEN_1550][0] = itemPassThreshold->maxSpliceLoss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][1] = itemPassThreshold->maxLinkerLoss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][2] = itemPassThreshold->maxLinkerReflectance.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][3] = itemPassThreshold->maxSplitter2Loss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][4] = itemPassThreshold->maxSplitter4Loss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][5] = itemPassThreshold->maxSplitter8Loss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][6] = itemPassThreshold->maxSplitter16Loss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][7] = itemPassThreshold->maxSplitter32Loss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][8] = itemPassThreshold->maxSplitter64Loss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][9] = itemPassThreshold->maxSplitter128Loss.iSelectedFlag[WAVELEN_1550];
    selectedFlag[WAVELEN_1550][10] = itemPassThreshold->maxSplitterReflectance.iSelectedFlag[WAVELEN_1550];
    
    threshold[WAVELEN_1625][0] = itemPassThreshold->maxSpliceLoss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][1] = itemPassThreshold->maxLinkerLoss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][2] = itemPassThreshold->maxLinkerReflectance.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][3] = itemPassThreshold->maxSplitter2Loss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][4] = itemPassThreshold->maxSplitter4Loss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][5] = itemPassThreshold->maxSplitter8Loss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][6] = itemPassThreshold->maxSplitter16Loss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][7] = itemPassThreshold->maxSplitter32Loss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][8] = itemPassThreshold->maxSplitter64Loss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][9] = itemPassThreshold->maxSplitter128Loss.fValue[WAVELEN_1625];
    threshold[WAVELEN_1625][10] = itemPassThreshold->maxSplitterReflectance.fValue[WAVELEN_1625];

    selectedFlag[WAVELEN_1625][0] = itemPassThreshold->maxSpliceLoss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][1] = itemPassThreshold->maxLinkerLoss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][2] = itemPassThreshold->maxLinkerReflectance.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][3] = itemPassThreshold->maxSplitter2Loss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][4] = itemPassThreshold->maxSplitter4Loss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][5] = itemPassThreshold->maxSplitter8Loss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][6] = itemPassThreshold->maxSplitter16Loss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][7] = itemPassThreshold->maxSplitter32Loss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][8] = itemPassThreshold->maxSplitter64Loss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][9] = itemPassThreshold->maxSplitter128Loss.iSelectedFlag[WAVELEN_1625];
    selectedFlag[WAVELEN_1625][10] = itemPassThreshold->maxSplitterReflectance.iSelectedFlag[WAVELEN_1625];
    
    columnCount += waveCount;
    tableWidth = pdfControl->pageEditWidth*(0.7+0.3*waveCount)/2;
    //链路链路损耗和链路光回损
    pThresholdsTable = pdfTableCreate(rowCount, columnCount, tableWidth);
    pdfTableSetAlignment(pThresholdsTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pThresholdsTable, 0, 0, 0, columnCount, font, 10, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pThresholdsTable, 1, 0, rowCount, columnCount, font, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pThresholdsTable, 1);
    pdfTableSetRowHeight(pThresholdsTable, 0, rowCount, 15);
    pdfTableSetItemAlignment(pThresholdsTable, 0, 0, rowCount, columnCount, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetItemBackgroundColor(pThresholdsTable, 0, 0, 0, columnCount, 238, 238, 238);
    pdfTableSetLineColor(pThresholdsTable, 173, 216, 230);

    pdfTableSetColumnWidth(pThresholdsTable, 0, 0.7/(0.7+0.3*waveCount));
    pdfTableSetText(pThresholdsTable, 0, 0, pPdfRptThreshold);
    pdfTableSetText(pThresholdsTable, 1, 0, pPdfRptMaxSpliceLoss);
    pdfTableSetText(pThresholdsTable, 2, 0, pPdfRptMaxConnectorLoss);
    pdfTableSetText(pThresholdsTable, 3, 0, pPdfRptMaxConnectorReflectance);
    pdfTableSetText(pThresholdsTable, 4, 0, pPdfRptMaxSplitterLoss1_2);
    pdfTableSetText(pThresholdsTable, 5, 0, pPdfRptMaxSplitterLoss1_4);
    pdfTableSetText(pThresholdsTable, 6, 0, pPdfRptMaxSplitterLoss1_8);
    pdfTableSetText(pThresholdsTable, 7, 0, pPdfRptMaxSplitterLoss1_16);
    pdfTableSetText(pThresholdsTable, 8, 0, pPdfRptMaxSplitterLoss1_32);
    pdfTableSetText(pThresholdsTable, 9, 0, pPdfRptMaxSplitterLoss1_64);
    pdfTableSetText(pThresholdsTable, 10, 0, pPdfRptMaxSplitterLoss1_128);
    pdfTableSetText(pThresholdsTable, 11, 0, pPdfRptMaxSplitterReflectance);
    
    for(i = 0; i < waveCount; i++)
    {
        char buff[32] = {0};
        int j = 0;
        pdfTableSetColumnWidth(pThresholdsTable, 1+i, 0.3/(0.7+0.3*waveCount));

        pdfTableSetText(pThresholdsTable, 0, 1+i, waveName[waveList[i]]);
        for(j = 0; j < 11; j++)
        {
            #if 0
            if(itemPassThreshold->isUsed)
            {
                char* temp = (j == 2 || j == 10) ? Float2String(REFLECTANCE_PRECISION, threshold[waveList[i]][j])
                                                 : Float2String(LOSS_PRECISION, threshold[waveList[i]][j]);
                sprintf(buff, "%s", temp);
                GuiMemFree(temp);
                //sprintf(buff, "%.3f", threshold[waveList[i]][j]);
            }
            else
            {
                sprintf(buff, "--");
            }
            #endif
            if(selectedFlag[i][j])
            {
                char* temp = (j == 2 || j == 10) ? Float2String(REFLECTANCE_PRECISION, threshold[waveList[i]][j])
                                                 : Float2String(LOSS_PRECISION, threshold[waveList[i]][j]);
                sprintf(buff, "%s", temp);
                GuiMemFree(temp);
            }
            else
            {
                sprintf(buff, "--");
            }
            pdfTableSetText(pThresholdsTable, 1+j, 1+i, buff);
        }
    }

    pdfTableDraw(pdfControl, pThresholdsTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pThresholdsTable);

    return iErr;
}

/**
* SOLA 参数和设置
* @param[in] pdfControl
* @param[in]
* @param[in]
* @return
* @note 有关此函数的特别说明：…
*/
static int solaPdfReportParaAndSetting(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pParaAndSettingTable = NULL;
    float IOR = pSolaMessager->lineDefine.groupIndex1550;
    float backScatter = pSolaMessager->lineDefine.backScatter1550;
    int launchFiberEnable = pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableLaunchFiber;
    int recvFiberEnable = pOtdrTopSettings->pUser_Setting->sSolaSetting.iEnableRecvFiber;
    char buff[32] = {0};
    char buff1[32] = {0};

    //链路链路损耗和链路光回损
    pParaAndSettingTable = pdfTableCreate(2, 4, pdfControl->pageEditWidth);
    pdfTableSetAlignment(pParaAndSettingTable, PDF_TABLE_ALIGN_LEFT);
    pdfTableSetItemFontAndSize(pParaAndSettingTable, 0, 0, 2, 4, font, 10, TEXT_NORMAL);
    pdfTableSetLineEnable(pParaAndSettingTable, 0);
    pdfTableSetRowHeight(pParaAndSettingTable, 0, 2, 15);
    pdfTableSetItemAlignment(pParaAndSettingTable, 0, 0, 2, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetColumnWidth(pParaAndSettingTable, 0, 0.28);
    pdfTableSetColumnWidth(pParaAndSettingTable, 1, 0.22);
    pdfTableSetColumnWidth(pParaAndSettingTable, 2, 0.28);
    pdfTableSetColumnWidth(pParaAndSettingTable, 3, 0.22);

    pdfTableSetText(pParaAndSettingTable, 0, 2, pPdfRptIOR);
    //sprintf(buff, "%.6f", IOR);
    char* temp = Float2String(IOR_PRECISION, IOR);
    sprintf(buff, "%s", temp);
    GuiMemFree(temp);
    pdfTableSetText(pParaAndSettingTable, 0, 3, buff);
    pdfTableSetText(pParaAndSettingTable, 1, 2, pPdfRptBackScatter);
    char* temp1 = Float2StringUnit(BACKSCATTER_COEFFICIENT_PRECISION, backScatter, " dB");
    sprintf(buff, "%s", temp1);
    GuiMemFree(temp1);
    //sprintf(buff, "%.2f dB", backScatter);
    pdfTableSetText(pParaAndSettingTable, 1, 3, buff);
    
    pdfTableSetText(pParaAndSettingTable, 0, 0, pPdfRptLaunchFiber);
    pdfTableSetText(pParaAndSettingTable, 1, 0, pPdfRptReceiveFiber);

    char* pLaunchFiberLen = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, pOtdrTopSettings->pUser_Setting->sSolaSetting.fLaunchFiberLen,
                                                                         pOtdrTopSettings->pUser_Setting->sSolaSetting.fLaunchFiberLen_ft,
                                                                         pOtdrTopSettings->pUser_Setting->sSolaSetting.fLaunchFiberLen_mile, 1);
    char* pRecvFiberLen = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_SOLA, pOtdrTopSettings->pUser_Setting->sSolaSetting.fRecvFiberLen,
                                                                         pOtdrTopSettings->pUser_Setting->sSolaSetting.fRecvFiberLen_ft,
                                                                         pOtdrTopSettings->pUser_Setting->sSolaSetting.fRecvFiberLen_mile, 1);
    sprintf(buff, "%s", pLaunchFiberLen);
    sprintf(buff1, "%s", pRecvFiberLen);
    GuiMemFree(pLaunchFiberLen);
    GuiMemFree(pRecvFiberLen);
    
    if(!launchFiberEnable)
    {
        strcpy(buff, " ");
    }
    
    if(!recvFiberEnable)
    {
        strcpy(buff1, " ");
    }
    
    pdfTableSetText(pParaAndSettingTable, 0, 1, buff);
    pdfTableSetText(pParaAndSettingTable, 1, 1, buff1);

    pdfTableDraw(pdfControl, pParaAndSettingTable);
    pdfDrawSpace(pdfControl, 5);
    pdfTableFree(pParaAndSettingTable);

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
static int solaPdfReportHeaderAndFooter(PdfControl *pdfControl)
{
    int iErr = 0;
    if (!pdfControl)
    {
        return -1;
    }
    SOLA_TABLE_PASS* passTable = getSolaTablePass();
    struct tm *nowTime;
    time_t now;
    now = time(NULL);
	nowTime = localtime(&now);
    char buff[128];
    unsigned int i = 0;
    
    for (i = 0; i <= pdfControl->page; i++)
    {
        /**********页眉*********/
        //sola 报告
        pdfHeaderDraw(i, pdfControl->pageMarginsWidth, 15, pPdfRptSOLAReport, font, 18, TEXT_BOLD, 0, 0, 0);

        //通过/不通过
        if(passTable->isPassed)
        {
            pdfDrawImage(i, BmpFileDirectory"sola_pass.jpg", 400, 15 - pdfControl->pageMarginsHeight, 34 / 1.8, 34 / 1.8);
            pdfHeaderDraw(i, 400 + 34 / 1.8 + 20, 13, pPdfRptPass, font, 20, TEXT_BOLD, 0, 255, 0);
        }
        else
        {
            pdfDrawImage(i, BmpFileDirectory"sola_fail.jpg", 400, 15 - pdfControl->pageMarginsHeight, 34 / 1.8, 34 / 1.8);
            pdfHeaderDraw(i, 400 + 34 / 1.8 + 20, 13, pPdfRptFail, font, 20, TEXT_BOLD, 255, 0, 0);
        }

        /**********页脚*********/
        //logo
        #ifdef FORC
        pdfDrawImage(i, BmpFileDirectory"pdf_logo.jpg", 0, pdfControl->pageHeight - pdfControl->pageMarginsHeight - 27,
            102/3, 53/3);
        #else
        pdfDrawImage(i, BmpFileDirectory"pdf_logo.jpg", 0, pdfControl->pageHeight - pdfControl->pageMarginsHeight - 27,
            291/3, 53/3);
        #endif

        //签名
        epdf_setrgbstroke(i, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
        epdf_setlinewidth(i, 1);
        sprintf(buff, pPdfRptSignature);
        float textWidth = epdf_textwidth(i, 10.0, font, buff);
        float signaturePosition = 120;
        pdfDrawText(i, signaturePosition, pdfControl->pageHeight - pdfControl->pageMarginsHeight - 20,
            textWidth + 20, 10.0, 10.0, TEXT_NORMAL, PDF_TEXT_ALIGN_LEFT, font, buff);
        pdfDrawLine(i, signaturePosition + textWidth, pdfControl->pageHeight - pdfControl->pageMarginsHeight - 10,
            signaturePosition + textWidth + 80, pdfControl->pageHeight - pdfControl->pageMarginsHeight - 10);

        //日期
        char time[32] = {0};
        switch(pCurSystemSet->uiTimeShowFlag)
        {
            case 0:
        	    strftime(time, 32, "%d/%m/%Y", nowTime);
            break;
            case 1:
                strftime(time, 32, "%m/%d/%Y", nowTime);
            break;
            default:
                strftime(time, 32, "%Y/%m/%d", nowTime);
            break;
        }
        sprintf(buff, "%s%s", pPdfRptDate, time);
        pdfFooterDraw(i, pdfControl->pageWidth / 2 + 30, 20, buff, font, 10, TEXT_NORMAL, 0, 0, 0);
        //页码
        sprintf(buff, "%s%d/%d", pPdfRptPage, i + 1, pdfControl->page + 1);
        pdfFooterDraw(i, pdfControl->pageWidth - 100, 20, buff, font, 10, TEXT_NORMAL, 0, 0, 0);
    }

    return iErr;
}

static void destroySolaCurveData()
{
    if (s_solaDataList != NULL)
    {
        int i;
        for (i = 0; i < s_solaDataList->waveNum; i++)
        {
            SOLA_DATA *solaData = s_solaDataList->solaData + i;
            GuiMemFree(solaData->data);
        }
        GuiMemFree(s_solaDataList->solaData);
        GuiMemFree(s_solaDataList);
    }
}

int createSolaPdfReport(char *pAbsolutePath, unsigned int currLanguage)
{
    char *pName = strrchr(pAbsolutePath, '/');
    pName++;
    PdfControl *pControl = createPdfControl(pAbsolutePath);

    if (!pControl)
    {
        return -1;
    }
    
    InitPdfReportText(currLanguage);
    iUnitConverterFlag = GetCurrSystemUnit(MODULE_UNIT_SOLA);
    font = pdfReportGetLanguage(pControl, currLanguage);
    pdfSetFont(pControl, font);
    //设置段间距
    pdfSetParagraphSpace(pControl, 5);
    //设置页边距
    pdfSetPageMarginsWidth(pControl, 10);
    //一般信息
    pdfTitleDraw(pControl, 0, pPdfRptGeneralInformation, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportGeneralInfo(pControl ,pName);
    //位置信息
    pdfTitleDraw(pControl, 0, pPdfRptLocations, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportLocations(pControl);
    //标识信息
    pdfTitleDraw(pControl, 0, pPdfRptIdentifiers, 1, font, 14, TEXT_BOLD, 255, 255, 255);
    solaPdfReportIdentifiers(pControl);
    //结果信息
    pdfTitleDraw(pControl, 0, pPdfRptSOLAResults, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportResult(pControl);
    //链路视图
    pdfTitleDraw(pControl, 0, pPdfRptLinkView, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportLinkView(pControl);
    //OTDR曲线
    if (GetSolaCurveWaveNum() != 0)
    {
        pdfTitleDraw(pControl, 0, pPdfRptOTDRCurve, 1, font, 14, TEXT_BOLD, 22, 180, 240);
        pdfDrawCurve(pControl);
    }
    //元素表
    pdfTitleDraw(pControl, 0, pPdfRptElementTable, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportElement(pControl);
    //链路通过/未通过阈值
    pdfTitleDraw(pControl, 0, pPdfRptSOLATestPassFailThresholds, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportLinkThresholds(pControl);
    //元素通过/未通过阈值
    pdfTitleDraw(pControl, 0, pPdfRptUserDefineElementPassFailThresholds, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportElementThresholds(pControl);
    //参数和设置
    pdfTitleDraw(pControl, 0, pPdfRptSOLAParametersAndSettings, 1, font, 14, TEXT_BOLD, 173, 216, 230);
    solaPdfReportParaAndSetting(pControl);
    //页眉页脚
    solaPdfReportHeaderAndFooter(pControl);
    
    pdfDrawTheEnd(pControl);
    FreePdfReportText();
    destroySolaCurveData();
    return 0;
}

//生成报告
int CreateSolaReport(char* absolutePath)
{
    int iErr = 0;
    if (absolutePath)
    {
        if(CanWriteFile(absolutePath))
        {
            iErr = createSolaPdfReport(absolutePath, GetCurrLanguageSet());
            if(access(absolutePath, F_OK) != 0)
            {
                iErr = -1;
            }
        }
        else
        {
            iErr = -1;
        }
    }
    return iErr;
}
