/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_pdfreportotdr.c  
* 摘    要：  实现otdr生成PDF报告
*
* 当前版本：  v1.0.0  
* 作    者：  
* 完成日期：  
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "app_pdf.h"
#include "app_pdfreportotdr.h"
#include "app_getsetparameter.h"
#include "app_frminit.h"
#include "app_screenshots.h"
#include "app_frmfilebrowse.h"
#include "app_frmupdate.h"
#include "app_unitconverter.h"

#include "wnd_frmmark.h"
#include "wnd_frmotdr.h"


//波长
static char *WaveLen[] = {
"1310 nm",
"1550 nm",
"1625 nm",
};
//测量时间
static char *AvgTime[] = {
"AUTO",
"15 sec",
"30 sec",
"60 sec",
"90 sec",
"120 sec",
"180 sec",
"RT",
};

extern MarkParam*           pMarkParam;

extern PSerialNum           pCurSN;
extern PFactoryConfig       pFactoryConf;
extern POTDR_TOP_SETTINGS   pOtdrTopSettings;
extern PSYSTEMSET 		    pCurSystemSet;
jmp_buf env;

static char *font;

//定义文本
static char *pPdfRptResultPass = NULL;
static char *pPdfRptResultFail = NULL;
static char *pPdfRptOtdrReport = NULL;
static char *pPdfRptSummary = NULL;
static char *pPdfRptFileName = NULL;
static char *pPdfRptCompany = NULL;
static char *pPdfRptTestDate = NULL;
static char *pPdfRptCustomer = NULL;
static char *pPdfRptTestTime = NULL;
static char *pPdfRptTask = NULL;
static char *pPdfRptCableMark = NULL;
static char *pPdfRptFiberMark = NULL;
static char *pPdfRptComments = NULL;
static char *pPdfRptLocation = NULL;
static char *pPdfRptOperatorA = NULL;
static char *pPdfRptOperatorB = NULL;
static char *pPdfRptTestSetModekNumber = NULL;
static char *pPdfRptTestSetSerialNumber = NULL;
static char *pPdfRptCalibrationDate = NULL;
static char *pPdfRptVersion = NULL;
static char *pPdfRptOTDRWave = NULL;
static char *pPdfRptMarkLineInformation = NULL;
static char *pPdfRptMarkera = NULL;
static char *pPdfRptMarkerA = NULL;
static char *pPdfRptMarkerb = NULL;
static char *pPdfRptMarkerB = NULL;
static char *pPdfRptDistanceFromAtoB = NULL;
static char *pPdfRptORLFromAtoB = NULL;
static char *pPdfRptDifferenceFromAtoB = NULL;
static char *pPdfRptSpanORL = NULL;
static char *pPdfRptMark2PointEventLoss = NULL;
static char *pPdfRptMark4PointEventLoss = NULL;
static char *pPdfRptEventTable = NULL;
static char *pPdfRptType = NULL;
static char *pPdfRptNO = NULL;
static char *pPdfRptDistanceM = NULL;
static char *pPdfRptDistanceKM = NULL;
static char *pPdfRptDistanceYd = NULL;
static char *pPdfRptDistanceMi = NULL;
static char *pPdfRptDistanceFt = NULL;
static char *pPdfRptDistanceKft = NULL;
static char *pPdfRptLoss = NULL;
static char *pPdfRptReflectivity = NULL;
static char *pPdfRptAttenuationM = NULL;
static char *pPdfRptAttenuationMi = NULL;
static char *pPdfRptAttenuationFt = NULL;
static char *pPdfRptSum = NULL;
static char *pPdfRptEventTypeSpanStart = NULL;
static char *pPdfRptEventTypeSpanEnd = NULL;
static char *pPdfRptEventTypeReflective = NULL;
static char *pPdfRptEventTypeNonReflective = NULL;
static char *pPdfRptEventTypePosition = NULL;
static char *pPdfRptEventTypeContinous = NULL;
static char *pPdfRptEventTypeEndOfAnalysis = NULL;
static char *pPdfRptEventTypeEcho = NULL;
static char *pPdfRptEventTypeSection = NULL;
static char *pPdfRptTraceParThreAndSet = NULL;
static char *pPdfRptTraceParameters = NULL;
static char *pPdfRptTracePluseWidth = NULL;
static char *pPdfRptTraceRange = NULL;
static char *pPdfRptTraceAveragingTime = NULL;
static char *pPdfRptTraceResultThresholds = NULL;
static char *pPdfRptTraceSpliceLoss = NULL;
static char *pPdfRptTraceConnectorLoss = NULL;
static char *pPdfRptTraceReflectance = NULL;
static char *pPdfRptTraceAttenuation = NULL;
static char *pPdfRptTraceSpanLoss = NULL;
static char *pPdfRptTraceSpanLength = NULL;
static char *pPdfRptTraceSpanORL = NULL;
static char *pPdfRptTraceSettings = NULL;
static char *pPdfRptTraceIOR = NULL;
static char *pPdfRptTraceSpliceLossDete = NULL;
static char *pPdfRptTraceBackscatter = NULL;
static char *pPdfRptTraceReflectanceDete = NULL;
static char *pPdfRptTraceHelixFactor = NULL;
static char *pPdfRptTraceEndofFiberDete = NULL;
static char *pPdfRptTraceSamplingRate = NULL;
static char *pPdfRptTraceNormal = NULL;
static char *pPdfRptTraceHigh = NULL;
static char *pPdfRptTraceSummaryTestResults = NULL;
static char *pPdfRptTraceWavelength = NULL;
static char *pPdfRptTraceAverageSpliceLoss = NULL;
static char *pPdfRptTraceMaxSpliceLoss = NULL;
static char *pPdfRptTraceAverageAttenuation = NULL;
static char *pPdfRptCalibration = NULL;
static char *pPdfRptLastCablibrationDate = NULL;
static char *pPdfRptNextCablibrationDate = NULL;
static char *pPdfRptNote = NULL;
static char *pPdfRptName = NULL;
static char *pPdfRptDate = NULL;
static char *pPdfRptLocationA = NULL;
static char *pPdfRptLocationB = NULL;
static char *pPdfRptLossMethod = NULL;
static char *pPdfRptAttenMethod = NULL;
static char *pPdfRptMark4PointAttenua = NULL;
static char *pPdfRptMark2PointAttenua = NULL;
static char *pPdfRptReflectance = NULL;
//初始化report文本
static void InitPdfReportText(unsigned int language)
{
    pPdfRptResultPass = GetCurrLanguageUtf8Text(OTDR_LBL_PASS, language);
    pPdfRptResultFail = GetCurrLanguageUtf8Text(OTDR_LBL_FAIL, language);
    pPdfRptOtdrReport = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_TITLE, language);
    pPdfRptSummary = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_SUMMARY, language);
    pPdfRptFileName = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_FILE_NAME, language);
    pPdfRptCompany = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_COMPANY_NAME, language);
    pPdfRptTestDate = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_TEST_DATE, language);
    pPdfRptCustomer = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_CUSTOMER, language);
    pPdfRptTestTime = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_TEST_TIME, language);
    pPdfRptTask = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_TASK_ID, language);
    pPdfRptCableMark = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_CABLE_NUMBER, language);
    pPdfRptFiberMark = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_FIBER_NUMBER, language);
    pPdfRptComments = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_NOTE, language);
    pPdfRptLocation = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_DIRECTION, language);
    pPdfRptOperatorA = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_LOACATION_A, language);
    pPdfRptOperatorB = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_LOACATION_B, language);
    pPdfRptTestSetModekNumber = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_MEASURING_MODEL, language);
    pPdfRptTestSetSerialNumber = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_SERIAL_NUMBER, language);
    pPdfRptCalibrationDate = GetCurrLanguageUtf8Text(1, language);
    pPdfRptVersion = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_VERSION, language);
    pPdfRptOTDRWave = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_TRACE, language);
    pPdfRptMarkLineInformation = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_MARKER_INFO, language);
    pPdfRptMarkera = GetCurrLanguageUtf8Text(1, language);
    pPdfRptMarkerA = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_MARKER_A, language);
    pPdfRptMarkerb = GetCurrLanguageUtf8Text(2, language);
    pPdfRptMarkerB = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_MARKER_b, language);
    pPdfRptDistanceFromAtoB = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_DISTANCE, language);
    pPdfRptORLFromAtoB = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_MARKER_LOSS, language);
    pPdfRptDifferenceFromAtoB = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_MARKER_ATTN, language);
    pPdfRptSpanORL = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_ORL, language);
    pPdfRptMark2PointEventLoss = GetCurrLanguageUtf8Text(1, language);
    pPdfRptMark4PointEventLoss = GetCurrLanguageUtf8Text(2, language);
	pPdfRptMark2PointAttenua = GetCurrLanguageUtf8Text(3, language);
    pPdfRptEventTable = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_EVENTS, language);
    pPdfRptType = GetCurrLanguageUtf8Text(OTDR_LBL_STYLE, language);
    pPdfRptNO = GetCurrLanguageUtf8Text(OTDR_LBL_NUMBER, language);
    pPdfRptDistanceM = GetCurrLanguageUtf8Text(OTDR_LBL_DISTANCE_M, language);
    pPdfRptDistanceKM = GetCurrLanguageUtf8Text(OTDR_LBL_DISTANCE_KM, language);
    pPdfRptDistanceYd = GetCurrLanguageUtf8Text(OTDR_LBL_DISTANCE_YD, language);
    pPdfRptDistanceMi = GetCurrLanguageUtf8Text(OTDR_LBL_DISTANCE_MILE, language);
    pPdfRptDistanceFt = GetCurrLanguageUtf8Text(OTDR_LBL_DISTANCE_FT, language);
    pPdfRptDistanceKft = GetCurrLanguageUtf8Text(OTDR_LBL_DISTANCE_KFT, language);
    pPdfRptLoss = GetCurrLanguageUtf8Text(OTDR_LBL_LOSS_DB, language);
    pPdfRptReflectivity = GetCurrLanguageUtf8Text(OTDR_LBL_REF, language);
    pPdfRptAttenuationM= GetCurrLanguageUtf8Text(OTDR_LBL_ATT, language);
    pPdfRptAttenuationMi= GetCurrLanguageUtf8Text(OTDR_LBL_ATT, language);
    pPdfRptAttenuationFt = GetCurrLanguageUtf8Text(OTDR_LBL_ATT, language);
    pPdfRptSum = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_CUMULATIVE_LOSS, language);
    pPdfRptEventTypeSpanStart = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_ORIGINALEVENT, language);
    pPdfRptEventTypeSpanEnd = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_END_EVENT, language);
    pPdfRptEventTypeReflective = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_REFLECTION_EVENT, language);
    pPdfRptEventTypeNonReflective = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_SPLICE_EVENT, language);
    pPdfRptEventTypePosition = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_GAIN_EVENT, language);
    pPdfRptEventTypeContinous = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_UNDERPOWER_EVENT, language);
    pPdfRptEventTypeEndOfAnalysis = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_ANALYSIS_END_EVENT, language);
    pPdfRptEventTypeEcho = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_RL_EVENT, language);
    pPdfRptEventTypeSection = GetCurrLanguageUtf8Text(1, language);
    pPdfRptTraceParThreAndSet = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_PARAMETER_SETTING, language);
    pPdfRptTraceParameters = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_PARAMETER, language);
    pPdfRptTracePluseWidth = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_PULSE, language);
    pPdfRptTraceRange = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_RANGE, language);
    pPdfRptTraceAveragingTime = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_AVG_TIME, language);
    pPdfRptTraceResultThresholds = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_THRESHOLD, language);
    pPdfRptTraceSpliceLoss = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_CONNECTION_LOSS, language);
    pPdfRptTraceConnectorLoss = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_CONNECTOR_LOSS, language);
    pPdfRptTraceReflectance = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_REF, language);
    pPdfRptTraceAttenuation = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_ATT, language);
    pPdfRptTraceSpanLoss = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_LOSS, language);
    pPdfRptTraceSpanLength = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_LENGTH, language);
    pPdfRptTraceSpanORL = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_SPAN_ORL, language);
    pPdfRptTraceSettings = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_SETUP, language);
    pPdfRptTraceIOR = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_REFRACTIVE_INDEX, language);
    pPdfRptTraceSpliceLossDete = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_SPLICING_THRE, language);
    pPdfRptTraceBackscatter = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_BACK_SCATTERING, language);
    pPdfRptTraceReflectanceDete = GetCurrLanguageUtf8Text(1, language);
    pPdfRptTraceHelixFactor = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_HELIX_FACTOR, language);
    pPdfRptTraceEndofFiberDete = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_FIBER_THRE, language);
    pPdfRptTraceSamplingRate = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_TEST_LOCATION, language);
    pPdfRptTraceNormal = GetCurrLanguageUtf8Text(0, language);
    pPdfRptTraceHigh = GetCurrLanguageUtf8Text(1, language);
    pPdfRptTraceSummaryTestResults = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_TEST_RESULT, language);
    pPdfRptTraceWavelength = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_WAVELENGTH, language);
    pPdfRptTraceAverageSpliceLoss = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_AVG_SPLICE_LOSS, language);
    pPdfRptTraceMaxSpliceLoss = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_MAX_SPLICE_LOSS, language);
    pPdfRptTraceAverageAttenuation = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_AVG_ATT, language);
    pPdfRptCalibration = GetCurrLanguageUtf8Text(1, language);
    pPdfRptLastCablibrationDate = GetCurrLanguageUtf8Text(2, language);
    pPdfRptNextCablibrationDate = GetCurrLanguageUtf8Text(3, language);
    pPdfRptNote = GetCurrLanguageUtf8Text(OTDR_SET_LBL_NOTE, language);
    pPdfRptName = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_NAME, language);
    pPdfRptDate = GetCurrLanguageUtf8Text(OTDR_REPORT_LBL_DATE, language);
    pPdfRptLocationA = GetCurrLanguageUtf8Text(1, language);
    pPdfRptLocationB = GetCurrLanguageUtf8Text(2, language);
    pPdfRptLossMethod = GetCurrLanguageUtf8Text(3, language);
    pPdfRptAttenMethod = GetCurrLanguageUtf8Text(4, language);
    pPdfRptMark4PointAttenua = GetCurrLanguageUtf8Text(5, language);
    pPdfRptReflectance = GetCurrLanguageUtf8Text(6, language);
}

static void FreePdfReportText(void)
{
    free(pPdfRptResultPass);
    free(pPdfRptResultFail);
    free(pPdfRptOtdrReport);
    free(pPdfRptSummary);
    free(pPdfRptFileName);
    free(pPdfRptCompany);
    free(pPdfRptTestDate);
    free(pPdfRptCustomer);
    free(pPdfRptTestTime);
    free(pPdfRptTask);
    free(pPdfRptCableMark);
    free(pPdfRptFiberMark);
    free(pPdfRptComments);
    free(pPdfRptLocation);
    free(pPdfRptOperatorA);
    free(pPdfRptOperatorB);
    free(pPdfRptTestSetModekNumber);
    free(pPdfRptTestSetSerialNumber);
    free(pPdfRptCalibrationDate);
    free(pPdfRptVersion);
    free(pPdfRptOTDRWave);
    free(pPdfRptMarkLineInformation);
    free(pPdfRptMarkera);
    free(pPdfRptMarkerA);
    free(pPdfRptMarkerb);
    free(pPdfRptMarkerB);
    free(pPdfRptDistanceFromAtoB);
    free(pPdfRptORLFromAtoB);
    free(pPdfRptDifferenceFromAtoB);
    free(pPdfRptSpanORL);
    free(pPdfRptMark2PointEventLoss);
    free(pPdfRptMark4PointEventLoss);
    free(pPdfRptEventTable);
    free(pPdfRptType);
    free(pPdfRptNO);
    free(pPdfRptDistanceM);
    free(pPdfRptDistanceKM);
    free(pPdfRptDistanceYd);
    free(pPdfRptDistanceMi);
    free(pPdfRptDistanceFt);
    free(pPdfRptDistanceKft);
    free(pPdfRptLoss);
    free(pPdfRptReflectivity);
    free(pPdfRptAttenuationM);
    free(pPdfRptAttenuationMi);
    free(pPdfRptAttenuationFt);
    free(pPdfRptSum);
    free(pPdfRptEventTypeSpanStart);
    free(pPdfRptEventTypeSpanEnd);
    free(pPdfRptEventTypeReflective);
    free(pPdfRptEventTypeNonReflective);
    free(pPdfRptEventTypePosition);
    free(pPdfRptEventTypeContinous);
    free(pPdfRptEventTypeEndOfAnalysis);
    free(pPdfRptEventTypeEcho);
    free(pPdfRptEventTypeSection);
    free(pPdfRptTraceParThreAndSet);
    free(pPdfRptTraceParameters);
    free(pPdfRptTraceWavelength);
    free(pPdfRptTracePluseWidth);
    free(pPdfRptTraceRange);
    free(pPdfRptTraceAveragingTime);
    free(pPdfRptTraceResultThresholds);
    free(pPdfRptTraceSpliceLoss);
    free(pPdfRptTraceConnectorLoss);
    free(pPdfRptTraceReflectance);
    free(pPdfRptTraceAttenuation);
    free(pPdfRptTraceSpanLoss);
    free(pPdfRptTraceSpanORL);
    free(pPdfRptTraceSettings);
    free(pPdfRptTraceIOR);
    free(pPdfRptTraceSpliceLossDete);
    free(pPdfRptTraceBackscatter);
    free(pPdfRptTraceReflectanceDete);
    free(pPdfRptTraceHelixFactor);
    free(pPdfRptTraceEndofFiberDete);
    free(pPdfRptTraceSamplingRate);
    free(pPdfRptTraceNormal);
    free(pPdfRptTraceHigh);
    free(pPdfRptTraceSummaryTestResults);
    free(pPdfRptTraceAverageSpliceLoss);
    free(pPdfRptTraceMaxSpliceLoss);
    free(pPdfRptTraceAverageAttenuation);
    free(pPdfRptCalibration);
    free(pPdfRptLastCablibrationDate);
    free(pPdfRptNextCablibrationDate);
    free(pPdfRptNote);
    free(pPdfRptName);
    free(pPdfRptDate);
    free(pPdfRptLocationA);
    free(pPdfRptLocationB);
    free(pPdfRptLossMethod);
    free(pPdfRptAttenMethod);
    free(pPdfRptMark4PointAttenua);
    free(pPdfRptMark2PointAttenua);
    free(pPdfRptReflectance);
}

//Summary
static int otdrPdfReportSummary(PdfControl *pdfControl, char* filename)
{
    if (!pdfControl)
    {
        return -1;
    }
    char temp[128] = {0};

    PdfTable *pBasicInfoTable = NULL;
    PdfTable *pTaskInfoTable = NULL;
    PdfTable *pModelInfoTable = NULL;

    pBasicInfoTable = pdfTableCreate(3, 4, 350);
    pdfTableSetItemFontAndSize(pBasicInfoTable, 0, 0, 3, 4, font, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pBasicInfoTable, 0, 1, 0, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pBasicInfoTable, 0, 3, 0, 3, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pBasicInfoTable, 1, 1, 1, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pBasicInfoTable, 1, 3, 1, 3, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pBasicInfoTable, 2, 1, 2, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pBasicInfoTable, 0);
    pdfTableSetRowHeight(pBasicInfoTable, 0, 3, 10);
    pdfTableSetRowHeightAdaptive(pBasicInfoTable, 0, 1, 1);
    pdfTableSetItemAlignment(pBasicInfoTable, 0, 0, 3, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pBasicInfoTable, 0, 1, 0, 1, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_TOP);
    pdfTableSetColumnWidth(pBasicInfoTable, 0, 0.2);
    pdfTableSetColumnWidth(pBasicInfoTable, 1, 0.3);
    pdfTableSetColumnWidth(pBasicInfoTable, 2, 0.2);
    pdfTableSetColumnWidth(pBasicInfoTable, 3, 0.3);    

    pTaskInfoTable = pdfTableCreate(6, 4, 350);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 0, 0, 5, 3, font, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 0, 1, 0, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 1, 1, 1, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 2, 1, 2, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 2, 3, 2, 3, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 3, 1, 3, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 3, 3, 3, 3, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 4, 1, 4, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 4, 3, 4, 3, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pTaskInfoTable, 5, 1, 5, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pTaskInfoTable, 0);
    pdfTableSetRowHeight(pTaskInfoTable, 0, 5, 10);
    pdfTableSetRowHeightAdaptive(pTaskInfoTable, 0, 5, 1);
    pdfTableSetItemAlignment(pTaskInfoTable, 0, 0, 5, 3, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pTaskInfoTable, 5, 1, 5, 1, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_TOP);
    pdfTableSetColumnWidth(pTaskInfoTable, 0, 0.2);
    pdfTableSetColumnWidth(pTaskInfoTable, 1, 0.3);
    pdfTableSetColumnWidth(pTaskInfoTable, 2, 0.2);
    pdfTableSetColumnWidth(pTaskInfoTable, 3, 0.3);
    pdfTableMergeCells(pTaskInfoTable, 0, 1, 0, 3);
    pdfTableMergeCells(pTaskInfoTable, 1, 1, 1, 3);
    pdfTableMergeCells(pTaskInfoTable, 5, 1, 5, 3);

    pModelInfoTable = pdfTableCreate(3, 2, 350);
    pdfTableSetItemFontAndSize(pModelInfoTable, 0, 0, 3, 2, font, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pModelInfoTable, 0, 1, 0, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pModelInfoTable, 1, 1, 1, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pModelInfoTable, 2, 1, 2, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pModelInfoTable, 3, 1, 3, 1, EPDF_UN, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pModelInfoTable, 0);
    pdfTableSetRowHeight(pModelInfoTable, 0, 4, 10);
    pdfTableSetItemAlignment(pModelInfoTable, 0, 0, 4, 2, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetColumnWidth(pModelInfoTable, 0, 0.35);
    pdfTableSetColumnWidth(pModelInfoTable, 1, 0.65);

    pdfTableSetText(pBasicInfoTable, 0, 0, pPdfRptFileName);
    pdfTableSetText(pBasicInfoTable, 0, 1, filename);
    pdfTableSetText(pBasicInfoTable, 0, 2, pPdfRptCompany);
    pdfTableSetText(pBasicInfoTable, 0, 3, pMarkParam->Company);
    pdfTableSetText(pBasicInfoTable, 1, 0, pPdfRptTestDate);
    getCurveMeasureDate(temp, NULL, pCurSystemSet->uiTimeShowFlag);
    pdfTableSetText(pBasicInfoTable, 1, 1, temp);
    pdfTableSetText(pBasicInfoTable, 1, 2, pPdfRptCustomer);
    pdfTableSetText(pBasicInfoTable, 1, 3, pMarkParam->Customer);
    pdfTableSetText(pBasicInfoTable, 2, 0, pPdfRptTestTime);
    getCurveMeasureDate(NULL, temp, pCurSystemSet->uiTimeShowFlag);
    pdfTableSetText(pBasicInfoTable, 2, 1, temp);

    pdfTableSetText(pTaskInfoTable, 0, 0, pPdfRptTask);
    pdfTableSetText(pTaskInfoTable, 0, 1, pMarkParam->TaskID_Unicode);
    pdfTableSetText(pTaskInfoTable, 1, 0, pPdfRptCableMark);
    pdfTableSetText(pTaskInfoTable, 1, 1, pMarkParam->CableMark);
    pdfTableSetText(pTaskInfoTable, 2, 0, pPdfRptFiberMark);
    pdfTableSetText(pTaskInfoTable, 2, 1, pMarkParam->FiberMark);
    pdfTableSetText(pTaskInfoTable, 2, 2, pPdfRptLocation);
    pMarkParam->Deraction ? pdfTableSetText(pTaskInfoTable, 2, 3, "B->A")
                          : pdfTableSetText(pTaskInfoTable, 2, 3, "A->B");
    pdfTableSetText(pTaskInfoTable, 3, 0, pPdfRptOperatorA);
    pdfTableSetText(pTaskInfoTable, 3, 1, pMarkParam->Operator_A);
    pdfTableSetText(pTaskInfoTable, 3, 2, pPdfRptOperatorB);
    pdfTableSetText(pTaskInfoTable, 3, 3, pMarkParam->Operator_B);
    pdfTableSetText(pTaskInfoTable, 4, 0, pPdfRptLocationA);
    pdfTableSetText(pTaskInfoTable, 4, 1, pMarkParam->Location_A);
    pdfTableSetText(pTaskInfoTable, 4, 2, pPdfRptLocationB);
    pdfTableSetText(pTaskInfoTable, 4, 3, pMarkParam->Location_B);
    pdfTableSetText(pTaskInfoTable, 5, 0, pPdfRptComments);
    pdfTableSetText(pTaskInfoTable, 5, 1, pMarkParam->Note);

    pdfTableSetText(pModelInfoTable, 0, 0, pPdfRptTestSetModekNumber);
    pdfTableSetText(pModelInfoTable, 0, 1, MODEL_NUMBER);
    pdfTableSetText(pModelInfoTable, 1, 0, pPdfRptTestSetSerialNumber);
    sprintf(temp,"%s%s%s%s%s%s%s",
                pCurSN->MachineName,
                pCurSN->Month,
                pCurSN->Year,
                pCurSN->Date,
                pCurSN->Number,
                pCurSN->MachineType,
                pCurSN->Address);
    pdfTableSetText(pModelInfoTable, 1, 1, temp);

    pdfTableSetText(pModelInfoTable, 2, 0, pPdfRptVersion);
    VERSION pPar;
    memset(&pPar, 0, sizeof(VERSION));
    ReadUpdateVersion(VERSION_PATH, &pPar);
    char buff[128] = {0};
    sprintf(buff, "V%s", pPar.Userfs);
    pdfTableSetText(pModelInfoTable, 2, 1, buff);

    pdfTableDraw(pdfControl, pBasicInfoTable);
    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);    
    pdfTableDraw(pdfControl, pTaskInfoTable);
    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);
    pdfTableDraw(pdfControl, pModelInfoTable);
    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);

    pdfDrawParagraphSpace(pdfControl);

    pdfTableFree(pBasicInfoTable);
    pdfTableFree(pTaskInfoTable);
    pdfTableFree(pModelInfoTable);

    return 0;
}

//绘制标记线信息表格
static int otdrPdfReportMarkLineInfo(PdfControl *pdfControl, MARKER markerInfo, int spanORLResult)
{
    if (!pdfControl)
    {
        return -1;
    }
    char *temp = NULL;
    PdfTable *pMarkLineInfoTable = NULL;

    pMarkLineInfoTable = pdfTableCreate(4, 4, 380);
    pdfTableSetItemFontAndSize(pMarkLineInfoTable, 0, 0, 7, 3, font, 7, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pMarkLineInfoTable, 0, 1, 7, 1, font, 7, TEXT_NORMAL);
    pdfTableSetItemFontAndSize(pMarkLineInfoTable, 0, 3, 7, 3, font, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pMarkLineInfoTable, 0);
    pdfTableSetRowHeight(pMarkLineInfoTable, 0, 7, 10);
    pdfTableSetItemAlignment(pMarkLineInfoTable, 0, 0, 7, 0, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetItemAlignment(pMarkLineInfoTable, 0, 2, 7, 2, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetColumnWidth(pMarkLineInfoTable, 0, 0.3);
    pdfTableSetColumnWidth(pMarkLineInfoTable, 1, 0.2);
    pdfTableSetColumnWidth(pMarkLineInfoTable, 2, 0.3);
    pdfTableSetColumnWidth(pMarkLineInfoTable, 3, 0.2);
    
    pdfTableSetText(pMarkLineInfoTable, 0, 0, pPdfRptMarkerA);
    temp = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, 
        markerInfo.dDot[1].fDist, 1);
    pdfTableSetText(pMarkLineInfoTable, 0, 1, temp);
    free(temp);
        
    pdfTableSetText(pMarkLineInfoTable, 0, 2, pPdfRptMarkerB);
    temp = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, 
        markerInfo.dDot[2].fDist, 1);
    pdfTableSetText(pMarkLineInfoTable, 0, 3, temp);
    free(temp);
    
    pdfTableSetText(pMarkLineInfoTable, 1, 0, pPdfRptDistanceFromAtoB);
    temp = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, 
        markerInfo.dDot[2].fDist - markerInfo.dDot[1].fDist, 1);
    pdfTableSetText(pMarkLineInfoTable, 1, 1, temp);
    free(temp);
    
    pdfTableSetText(pMarkLineInfoTable, 1, 2, pPdfRptORLFromAtoB);
    temp = Float2StringUnit(ORL_PRECISION, markerInfo.fReturnLoss, " dB");
    pdfTableSetText(pMarkLineInfoTable, 1, 3, temp);
    free(temp);
    
    pdfTableSetText(pMarkLineInfoTable, 2, 0, pPdfRptDifferenceFromAtoB);
    temp = Float2StringUnit(LOSS_PRECISION, 
        (float)(markerInfo.dDot[2].iValue - markerInfo.dDot[1].iValue)/SCALE_FACTOR, " dB");
    pdfTableSetText(pMarkLineInfoTable, 2, 1, temp);
    free(temp);
    
    pdfTableSetText(pMarkLineInfoTable, 2, 2, pPdfRptSpanORL);
    if(!spanORLResult)
    {
        pdfTableSetItemBackgroundColor(pMarkLineInfoTable, 3, 3, 3, 3, 255, 0, 0);
    }
    if(markerInfo.fTotalReutrnLoss < RETURNLOSS_MAXVALUE)
    {
        temp = Float2StringUnit(ORL_PRECISION, markerInfo.fTotalReutrnLoss, " dB");
        pdfTableSetText(pMarkLineInfoTable, 2, 3, temp);
        free(temp);
    }
    else
    {
        pdfTableSetText(pMarkLineInfoTable, 2, 3, "----");
    }
                                       
    pdfTableSetText(pMarkLineInfoTable, 3, 0, pPdfRptMark2PointAttenua);
    temp = Float2StringUnit(ATTENUATION_PRECISION, markerInfo.fAttenAB, " dB/km");
    pdfTableSetText(pMarkLineInfoTable, 3, 1, temp);
    free(temp);    

    pdfTableDraw(pdfControl, pMarkLineInfoTable);

    pdfDrawParagraphSpace(pdfControl);
    pdfTableFree(pMarkLineInfoTable);

    return 0;
}

//得到事件类型 返回0 熔接事件 返回1 连接器事件
static int otdrPdfReportGetEventType(int type)
{
    int iRet = 0;
    
    switch(type)
    {
        case 0:
        case 1:
        case 5:
        case 10:
        case 11:
        case 15:
            break;
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
        case 14:
            iRet = 1;
            break; 
        default:
        break;
     }
     
     return iRet;
}

//获得事件类型
static int otdrPdfReportGetEventTypeName(EVENTS_TABLE Events, int index, char *typeName)
{
    int displaySectionEvent = pOtdrTopSettings->pUser_Setting->sCommonSetting.iDisplaySectionEvent ? 1 : 0;

    if(typeName && index < (Events.iEventsNumber * (1 + displaySectionEvent) - displaySectionEvent))
    {
        if(displaySectionEvent)
        {
            if(index % 2)
            {
                strcpy(typeName, pPdfRptEventTypeSection);
                return 0;
            }
            index /= 2;
        }

        if((Events.iEventsStart != 0 && Events.iEventsStart == index) || 
           (Events.iLaunchFiberIndex != 0 && Events.iLaunchFiberIndex == index) ||
           (Events.iLaunchFiberIndex == 0 && Events.iEventsStart == 0 && index == 0))
        {
            strcpy(typeName, pPdfRptEventTypeSpanStart);
            return 0;
        }

        if((Events.iRecvFiberIndex != 0 && Events.iRecvFiberIndex == index) ||
            (Events.iRecvFiberIndex == 0 && (Events.iEventsNumber - 1) == index))
        {
            strcpy(typeName, pPdfRptEventTypeSpanEnd);
            return 0;
        }

        switch(Events.EventsInfo[index].iStyle)
        {
            case 0:
            case 5:
            strcpy(typeName, pPdfRptEventTypeNonReflective);
            break;
            case 1:
            strcpy(typeName, pPdfRptEventTypePosition);
            break;
            case 3:
            strcpy(typeName, pPdfRptEventTypeEcho);
            break;
            case 2:
            case 4:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            strcpy(typeName, pPdfRptEventTypeReflective);
            break;
            case 20:
            strcpy(typeName, pPdfRptEventTypeContinous);
            break;
            case 21:
            strcpy(typeName, pPdfRptEventTypeEndOfAnalysis);
            break;
            default:
            return -1;
        }
        return 0;
    }
    return -1;
}
//绘制事件列表
static int otdrPdfReportEventTable(PdfControl *pdfControl, EVENTS_TABLE Events)
{
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pEventTable = NULL;
    int i;
    int displaySectionEvent = pOtdrTopSettings->pUser_Setting->sCommonSetting.iDisplaySectionEvent ? 1 : 0;
    int eventCount = (Events.iEventsNumber > 1 && displaySectionEvent) ? Events.iEventsNumber * 2 - 1 : Events.iEventsNumber;
    
    int iStartIndex = (Events.iLaunchFiberIndex > 0) ? Events.iLaunchFiberIndex : Events.iEventsStart;
    int iEndIndex = (Events.iRecvFiberIndex > 0) ? Events.iRecvFiberIndex : Events.iEventsNumber - 1;
	float fStartDistance = Events.EventsInfo[iStartIndex].fEventsPosition;
    float fStartTotalLoss = Events.EventsInfo[iStartIndex].fTotalLoss;
    if(displaySectionEvent)
    {
        iStartIndex *= 2;
        iEndIndex *= 2;
    }

    pEventTable = pdfTableCreate(eventCount+1, 7, pdfControl->pageEditWidth);
    pdfTableSetItemFontAndSize(pEventTable, 0, 0, 0, 7, font, 7, TEXT_BOLD);
    pdfTableSetItemFontAndSize(pEventTable, 1, 0, eventCount, 7, font, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pEventTable, 1);
    pdfTableSetRowHeight(pEventTable, 0, eventCount, 10);
    pdfTableSetItemAlignment(pEventTable, 0, 0, eventCount, 7, PDF_TEXT_ALIGN_H_CENTER, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetColumnWidth(pEventTable, 0, 0.18);
    pdfTableSetColumnWidth(pEventTable, 1, 0.08);
    pdfTableSetColumnWidth(pEventTable, 2, 0.12);
    pdfTableSetColumnWidth(pEventTable, 3, 0.1);
    pdfTableSetColumnWidth(pEventTable, 4, 0.18);
    pdfTableSetColumnWidth(pEventTable, 5, 0.2);
    pdfTableSetColumnWidth(pEventTable, 6, 0.14);

    pdfTableSetText(pEventTable, 0, 0, pPdfRptType);
    pdfTableSetText(pEventTable, 0, 1, pPdfRptNO);
    pdfTableSetText(pEventTable, 0, 3, pPdfRptLoss);
    pdfTableSetText(pEventTable, 0, 4, pPdfRptReflectivity);
    pdfTableSetText(pEventTable, 0, 5, pPdfRptAttenuationM);
    pdfTableSetText(pEventTable, 0, 6, pPdfRptSum);

    if (displaySectionEvent)
    {
        for(i = 1; i < eventCount + 1; i++)
        {
            i % 2 == 1 ? pdfTableSetItemBackgroundColor(pEventTable, i, 0, i, 7, 230, 241, 255)
            : pdfTableSetItemBackgroundColor(pEventTable, i, 0, i, 7, 247, 247, 222);
        }
    }
    else
    {
        pdfTableSetItemBackgroundColor(pEventTable, 1, 0, eventCount, 7, 230, 241, 255);
    }
    
    switch(GetCurrSystemUnit(MODULE_UNIT_OTDR))
    {
        case UNIT_M:
        pdfTableSetText(pEventTable, 0, 2, pPdfRptDistanceM);
        break;
        case UNIT_KM:
        pdfTableSetText(pEventTable, 0, 2, pPdfRptDistanceKM);
        break;
        case UNIT_FT:
        pdfTableSetText(pEventTable, 0, 2, pPdfRptDistanceFt);
        break;
        case UNIT_KFT:
        pdfTableSetText(pEventTable, 0, 2, pPdfRptDistanceKft);
        break;
        case UNIT_YD:
        pdfTableSetText(pEventTable, 0, 2, pPdfRptDistanceYd);
        break;
        case UNIT_MI:
        pdfTableSetText(pEventTable, 0, 2, pPdfRptDistanceMi);
        break;
        default:
        break;
    }

    for (i = 0; i < eventCount; i++)
    {
        char temp[128] = {0};
        char *buff = NULL;
        EVENTS_INFO event = Events.EventsInfo[i/(1+displaySectionEvent)];
        EVENTS_ISPASS result = Events.EventsIsPass[i/(1+displaySectionEvent)];
        //事件的距离和累积损耗以起始点为准(如果存在起始点)
		float fDistanceOrLength = event.fEventsPosition;
		float fTLoss = event.fTotalLoss;
		double fSectionEvtLoss = 0.0f;

        if(displaySectionEvent)
        {
            double fAttenuation = Float2Float(PRECISION_3, event.fAttenuation);
            double SectionLen = Events.EventsInfo[i/2+1].fEventsPosition
                 - event.fEventsPosition;
            fSectionEvtLoss = fAttenuation * (SectionLen / 1000);
        }
		
		if (iStartIndex != 0)
		{
			fDistanceOrLength -= fStartDistance;
			fTLoss = event.fTotalLoss - fStartTotalLoss;
		}	
		
		if(i > 0 && i < eventCount - 1)
		{
            fTLoss += Float2Float(PRECISION_3, event.fLoss);
		}
		
		if((i % 2) && displaySectionEvent)
		{
            fDistanceOrLength = Events.EventsInfo[(i/(1+displaySectionEvent)) + 1].fEventsPosition - event.fEventsPosition;
            fTLoss += fSectionEvtLoss;
		}
		
		//event type
        otdrPdfReportGetEventTypeName(Events, i, temp);
        pdfTableSetText(pEventTable, i + 1, 0, temp);

        //number
        if((i % 2) && displaySectionEvent)
        {
            strcpy(temp, " ");
        }
        else if (i > iStartIndex && i < iEndIndex)
        {
		    sprintf(temp, "%d", (i - iStartIndex) / (displaySectionEvent + 1));
		}
		else if(i == iStartIndex)
		{
            if (Events.iEventsStart > 0) 
            {
                sprintf(temp, "R");
            }
            else
            {
                if(iStartIndex == iEndIndex)
                {
                    sprintf(temp, "S/E");
                }
                else
                {
                    sprintf(temp, "S");
                }
            }
		}
		else if(i == iEndIndex)
		{
            sprintf(temp, "E");
		}
        else
        {
            strcpy(temp, "*");
        }
        pdfTableSetText(pEventTable, i + 1, 1, temp);
        //position
        buff = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, 
            fDistanceOrLength, 0);
        pdfTableSetText(pEventTable, i + 1, 2, buff);
        free(buff);
        
        //loss
        if((i % 2) && displaySectionEvent)
        {
            buff = Float2String(LOSS_PRECISION, fSectionEvtLoss);
            pdfTableSetText(pEventTable, i + 1, 3, buff);
            free(buff);
        }
        else
        {
            if(!result.iIsEventLossPass)
            {
                pdfTableSetItemBackgroundColor(pEventTable, i + 1, 3, i + 1, 3, 255, 0, 0);
            }
            if (i != 0 && i != eventCount - 1)
            {
                buff = Float2String(LOSS_PRECISION, event.fLoss);
                pdfTableSetText(pEventTable, i + 1, 3, buff);
                free(buff);
            }  
            else
            {
                pdfTableSetText(pEventTable, i + 1, 3, "  ");
            }
        }
        
        //reflectivity
        if((i % 2) && displaySectionEvent)
        {
            pdfTableSetText(pEventTable, i + 1, 4, " ");
        }
        else
        {
            if(!result.iIsReflectPass)
            {
                pdfTableSetItemBackgroundColor(pEventTable, i + 1, 4, i + 1, 4, 255, 0, 0);
            }
            if (otdrPdfReportGetEventType(event.iStyle))
            {
                buff = Float2String(REFLECTANCE_PRECISION, event.fReflect);
                pdfTableSetText(pEventTable, i + 1, 4, buff);
                free(buff);
            }
            else
            {
                pdfTableSetText(pEventTable, i + 1, 4, "  ");
            }
        }

        //attenuation
        if((((i % 2) == 0) && displaySectionEvent) || (displaySectionEvent == 0))
        {
            pdfTableSetText(pEventTable, i + 1, 5, " ");
        }
        else
        {
            if(!result.iIsAttenuationPass)
            {
                pdfTableSetItemBackgroundColor(pEventTable, i + 1, 5, i + 1, 5, 255, 0, 0);
            }

            if(i == eventCount - 1)
            {
                pdfTableSetText(pEventTable, i + 1, 5, " ");
            }
            else
            {
                buff = Float2String(ATTENUATION_PRECISION, event.fAttenuation);
                pdfTableSetText(pEventTable, i + 1, 5, buff);
                free(buff);
            }
        }
        //accumulation
        if(i < iStartIndex || (i > iEndIndex && iEndIndex > 0)
            || (Events.iIsThereASpan == 0 && Events.iLaunchFiberIndex == 0 
                && Events.iRecvFiberIndex == 0 && i != 0))
        {
            pdfTableSetText(pEventTable, i + 1,6, "  ");
        }
        else if(i == 0)
        {
            pdfTableSetText(pEventTable, i + 1,6, "0.000");
        }
        else
        {
            buff = Float2String(LOSS_PRECISION, fTLoss);
            pdfTableSetText(pEventTable, i + 1,6, buff);
            free(buff);
        }
    }

    pdfTableDraw(pdfControl, pEventTable);

    pdfDrawParagraphSpace(pdfControl);
    pdfTableFree(pEventTable);

    return 0;
}

//获得平均熔接损耗、最大熔接损耗和平均损耗
static int otdrPdfReportGetAvgAndMaxLoss(EVENTS_TABLE events, float *avgSpliceLoss, float *maxSpliceLoss, float *avgAttenuation)
{
    if(!avgSpliceLoss || !maxSpliceLoss || !avgAttenuation)
    {
        return -1;
    }
    int i, spliceLossCount = 0;
    
    *avgSpliceLoss = 0;
    *maxSpliceLoss = 0;
    *avgAttenuation = 0;

    for(i=0; i<events.iEventsNumber; i++)
    {
        *avgAttenuation += events.EventsInfo[i].fAttenuation;
        if(!otdrPdfReportGetEventType(events.EventsInfo[i].iStyle))
        {
            *avgSpliceLoss += events.EventsInfo[i].fLoss;
            if(*maxSpliceLoss < events.EventsInfo[i].fLoss)
            {
                *maxSpliceLoss = events.EventsInfo[i].fLoss;
            }
            spliceLossCount++;
        }
    }
    if(*avgAttenuation)
    {
        *avgAttenuation /= events.iEventsNumber;
    }

    if(*avgSpliceLoss)
    {
        *avgSpliceLoss /= spliceLossCount;
    }

    return 0;
}

//绘制曲线参数阈值和设置信息表格
static int otdrPdfReportTraceParaThreAndSetTable(PdfControl *pdfControl, PCURVE_INFO curveInfo, LINK_ISPASS linkResult)
{
    if (!pdfControl)
    {
        return -1;
    }

    PdfTable *pParametersTable = NULL;
    PdfTable *pThresholdsTable = NULL;
    PdfTable *pSettingsTable = NULL;
    PdfTable *pTestResultsTable = NULL;
    
    CURVE_PRARM curvePara = curveInfo->pParam;//曲线参数信息
    WAVELEN wave = (WAVELEN)curvePara.enWave;//波长
    PASS_THR threshold = pOtdrTopSettings->pUser_Setting->sOtherSetting.PassThr;//通过/不通过阈值
//     SAMPLE_INTERVAL smpResolution = pOtdrTopSettings->pUser_Setting->sSampleSetting.enSmpResolution;//采样分辨率
    WAVELEN_ARGS wavelengthArgs = pOtdrTopSettings->pUser_Setting->sSampleSetting.sWaveLenArgs[wave];//波长参数
    ANALYSIS_SETTING analysisSet = pOtdrTopSettings->pUser_Setting->sAnalysisSetting;//分析设置参数
    float spanLength = GetSpanLength(&curveInfo->Events);//跨段长度
    float spanLoss = GetSpanLoss(&curveInfo->Events);//跨段损耗
    float spanORL = curveInfo->mMarker.fTotalReutrnLoss;//跨段回波损耗
    float avgSpliceLoss = 0.0f;
    float maxSpliceLoss = 0.0f;
    float avgAttenuation = 0.0f;
    char temp[TEMP_ARRAY_SIZE] = {0};
    char *buff = NULL;
    
    otdrPdfReportGetAvgAndMaxLoss(curveInfo->Events, &avgSpliceLoss, &maxSpliceLoss, &avgAttenuation);

    pParametersTable = pdfTableCreate(2, 4, 350);
    pdfTableSetItemFontAndSize(pParametersTable, 0, 0, 2, 4, font, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pParametersTable, 0);
    pdfTableSetRowHeight(pParametersTable, 0, 2, 10);
    pdfTableSetItemAlignment(pParametersTable, 0, 0, 3, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetColumnWidth(pParametersTable, 0, 0.37);
    pdfTableSetColumnWidth(pParametersTable, 1, 0.13);
    pdfTableSetColumnWidth(pParametersTable, 2, 0.37);
    pdfTableSetColumnWidth(pParametersTable, 3, 0.13);

    pThresholdsTable = pdfTableCreate(4, 4, 350);
    pdfTableSetItemFontAndSize(pThresholdsTable, 0, 0, 4, 4, font, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pThresholdsTable, 0);
    pdfTableSetRowHeight(pThresholdsTable, 0, 4, 10);
    pdfTableSetItemAlignment(pThresholdsTable, 0, 0, 4, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetColumnWidth(pThresholdsTable, 0, 0.37);
    pdfTableSetColumnWidth(pThresholdsTable, 1, 0.13);
    pdfTableSetColumnWidth(pThresholdsTable, 2, 0.37);
    pdfTableSetColumnWidth(pThresholdsTable, 3, 0.13);

    pSettingsTable = pdfTableCreate(3, 4, 350);
    pdfTableSetItemFontAndSize(pSettingsTable, 0, 0, 5, 4, font, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pSettingsTable, 0);
    pdfTableSetRowHeight(pSettingsTable, 0, 5, 10);
    pdfTableSetItemAlignment(pSettingsTable, 0, 0, 5, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);
    pdfTableSetColumnWidth(pSettingsTable, 0, 0.37);
    pdfTableSetColumnWidth(pSettingsTable, 1, 0.13);
    pdfTableSetColumnWidth(pSettingsTable, 2, 0.37);
    pdfTableSetColumnWidth(pSettingsTable, 3, 0.13);

    pTestResultsTable = pdfTableCreate(5, 4, 350);
    pdfTableSetItemFontAndSize(pTestResultsTable, 0, 0, 5, 4, font, 7, TEXT_NORMAL);
    pdfTableSetLineEnable(pTestResultsTable, 0);
    pdfTableSetRowHeight(pTestResultsTable, 0, 5, 10);
    pdfTableSetItemAlignment(pTestResultsTable, 0, 0, 5, 4, PDF_TEXT_ALIGN_LEFT, PDF_TEXT_ALIGN_V_CENTER);

    pdfTableSetText(pParametersTable, 0, 0, pPdfRptTraceWavelength);
    pdfTableSetText(pParametersTable, 0, 1, WaveLen[wave]);
    pdfTableSetText(pParametersTable, 0, 2, pPdfRptTracePluseWidth);
	//设置脉宽信息
	int iRangePulse[10][9] =
	{
		{0, 5, 10, 20, 50, 100, 200, 500, 1000},//auto
		{0, 5, 10, 20, 50, 100, -1, -1, -1},
		{0, 5, 10, 20, 50, 100, 200, -1, -1},
		{0, 5, 10, 20, 50, 100, 200, -1, -1},
		{0, 5, 10, 20, 50, 100, 200, 500, -1},
		{0, 5, 10, 20, 50, 100, 200, 500, 1000},
		{0, 5, 10, 20, 50, 100, 200, 500, 1000},
		{0, 20, 50, 100, 200, 500, 1000, 2000, 10000},
		{0, 50, 100, 200, 500, 1000, 2000, 10000, 20000},
		{0, 50, 100, 200, 500, 1000, 2000, 10000, 20000}
	};
	memset(temp, 0, TEMP_ARRAY_SIZE);
	if (iRangePulse[curvePara.enRange][curvePara.enPluse] == 0)
	{
		strcpy(temp, "Auto");
	}
	else
	{
		sprintf(temp, "%dns", iRangePulse[curvePara.enRange][curvePara.enPluse]);
	}

    pdfTableSetText(pParametersTable, 0, 3, temp);
    pdfTableSetText(pParametersTable, 1, 0, pPdfRptTraceRange);
	//设置量程信息
	float fRange[10] = { 0, 0.5, 1, 2, 5, 10, 20, 50, 100, 200 };
	buff = UnitConverter_Dist_Km2System_Float2String(MODULE_UNIT_OTDR, fRange[curvePara.enRange], 1);
	pdfTableSetText(pParametersTable, 1, 1, buff);
	GuiMemFree(buff);
    pdfTableSetText(pParametersTable, 1, 2, pPdfRptTraceAveragingTime);
	//设置平均时间信息
	memset(temp, 0, TEMP_ARRAY_SIZE);
    if(curvePara.enTime == ENUM_AVG_TIME_AUTO || curvePara.enTime == ENUM_AVG_TIME_REALTIME)
    {
        sprintf(temp, "%s", AvgTime[curvePara.enTime]);
    }
    else
    {
        sprintf(temp, "%d sec", curvePara.uiAvgTime);
    }
    pdfTableSetText(pParametersTable, 1, 3, temp);

    pdfTableSetText(pThresholdsTable, 0, 0, pPdfRptTraceSpliceLoss);
    if(threshold.SpliceLossThr[wave].iSlectFlag)
    {
        buff = Float2StringUnit(LOSS_PRECISION, threshold.SpliceLossThr[wave].fThrValue, " dB");
        pdfTableSetText(pThresholdsTable, 0, 1, buff);
        free(buff);
    }
    pdfTableSetText(pThresholdsTable, 0, 2, pPdfRptTraceConnectorLoss);
    if(threshold.ConnectLossThr[wave].iSlectFlag)
    {
        buff = Float2StringUnit(LOSS_PRECISION, threshold.ConnectLossThr[wave].fThrValue, " dB");
        pdfTableSetText(pThresholdsTable, 0, 3, buff);
        free(buff);
    }
    pdfTableSetText(pThresholdsTable, 1, 0, pPdfRptTraceReflectance);
    if(threshold.ReturnLossThr[wave].iSlectFlag)
    {
        buff = Float2StringUnit(REFLECTANCE_PRECISION, threshold.ReturnLossThr[wave].fThrValue, " dB");
        pdfTableSetText(pThresholdsTable, 1, 1, buff);
        free(buff);
    }
    pdfTableSetText(pThresholdsTable, 1, 2, pPdfRptTraceAttenuation);
    if(threshold.AttenuationThr[wave].iSlectFlag)
    {
        buff = Float2StringUnit(ATTENUATION_PRECISION, threshold.AttenuationThr[wave].fThrValue, " dB/km");
        pdfTableSetText(pThresholdsTable, 1, 3, buff);
        free(buff);
    }
    pdfTableSetText(pThresholdsTable, 2, 0, pPdfRptTraceSpanLoss);
    if(threshold.RegionLossThr[wave].iSlectFlag)
    {
        buff = Float2StringUnit(LOSS_PRECISION, threshold.RegionLossThr[wave].fThrValue, " dB");
        pdfTableSetText(pThresholdsTable, 2, 1, buff);
        free(buff);
    }
    pdfTableSetText(pThresholdsTable, 2, 2, pPdfRptTraceSpanLength);
    if(threshold.RegionLengthThr[wave].iSlectFlag)
    {    
        char *buff = UnitConverter_Dist_Eeprom2System_Float2String(MODULE_UNIT_OTDR, 
            threshold.fSpanLength_m[wave], threshold.fSpanLength_ft[wave], threshold.fSpanLength_mi[wave], 1);
        pdfTableSetText(pThresholdsTable, 2, 3, buff);
        free(buff);
        buff = NULL;
    }
    pdfTableSetText(pThresholdsTable, 3, 0, pPdfRptTraceSpanORL);
    if(threshold.RegionReturnLossThr[wave].iSlectFlag)
    {
        buff = Float2StringUnit(ORL_PRECISION, threshold.RegionReturnLossThr[wave].fThrValue, " dB");
        pdfTableSetText(pThresholdsTable, 3, 1, buff);
        free(buff);
    }

    pdfTableSetText(pSettingsTable, 0, 0, pPdfRptTraceIOR);
    buff = Float2String(IOR_PRECISION, wavelengthArgs.fRefractiveIndex);
    pdfTableSetText(pSettingsTable, 0, 1, buff);
    free(buff);
    
    pdfTableSetText(pSettingsTable, 0, 2, pPdfRptTraceSpliceLossDete);
    buff = Float2StringUnit(LOSS_PRECISION, analysisSet.fSpliceLossThr, " dB");
    pdfTableSetText(pSettingsTable, 0, 3, buff);
    free(buff);
    
    pdfTableSetText(pSettingsTable, 1, 0, pPdfRptTraceBackscatter);
    buff = Float2StringUnit(BACKSCATTER_COEFFICIENT_PRECISION, wavelengthArgs.fBackScattering, " dB");
    pdfTableSetText(pSettingsTable, 1, 1, buff);
    free(buff);
        
    pdfTableSetText(pSettingsTable, 1, 2, pPdfRptTraceHelixFactor);
    buff = Float2StringUnit(HELIX_FACTOR_PRECISION, wavelengthArgs.fExcessLength, "\%");
    pdfTableSetText(pSettingsTable, 1, 3, buff);
    free(buff);
    
    pdfTableSetText(pSettingsTable, 2, 0, pPdfRptTraceEndofFiberDete);
    buff = Float2StringUnit(LOSS_PRECISION, analysisSet.fEndLossThr, " dB");
    pdfTableSetText(pSettingsTable, 2, 1, buff);
    free(buff);
    
	pdfTableSetText(pSettingsTable, 2, 2, pPdfRptTraceSamplingRate);
	//采样点
	PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
	PCURVE_INFO pCurve = pDisplay->pCurve[wave];
	memset(temp, 0, TEMP_ARRAY_SIZE);
	sprintf(temp, "%d", pCurve->dIn.uiCnt);
	pdfTableSetText(pSettingsTable, 2, 3, temp);

    pdfTableSetText(pTestResultsTable, 0, 0, pPdfRptTraceWavelength);
    pdfTableSetText(pTestResultsTable, 0, 1, WaveLen[wave]);
    pdfTableSetText(pTestResultsTable, 1, 0, pPdfRptTraceSpanLength);
    if(!linkResult.iIsRegionLengthPass)
    {
        pdfTableSetItemBackgroundColor(pTestResultsTable, 1, 1, 1, 1, 255, 0, 0);
    }
    
    buff = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, spanLength, 1);
    pdfTableSetText(pTestResultsTable, 1, 1, buff);
    free(buff);
    
    pdfTableSetText(pTestResultsTable, 1, 2, pPdfRptTraceSpanLoss);
    if(!linkResult.iIsRegionLossPass)
    {
        pdfTableSetItemBackgroundColor(pTestResultsTable, 1, 3, 1, 3, 255, 0, 0);
    }
    buff = Float2StringUnit(LOSS_PRECISION, spanLoss, " dB");
    pdfTableSetText(pTestResultsTable, 1, 3, buff);
    free(buff);
    
    pdfTableSetText(pTestResultsTable, 2, 0, pPdfRptTraceSpanORL);
    if(!linkResult.iIsRegionORLPass)
    {
        pdfTableSetItemBackgroundColor(pTestResultsTable, 2, 1, 2, 1, 255, 0, 0);
    }
    
    if(spanORL < RETURNLOSS_MAXVALUE)
    {
        buff = Float2StringUnit(ORL_PRECISION, spanORL, " dB");
        pdfTableSetText(pTestResultsTable, 2, 1, buff);
        free(buff);
    }
    else
    {
		memset(temp, 0, TEMP_ARRAY_SIZE);
        sprintf(temp, "----");
        pdfTableSetText(pTestResultsTable, 2, 1, temp);
    }
    pdfTableSetText(pTestResultsTable, 2, 2, pPdfRptTraceAverageSpliceLoss);
    buff = Float2StringUnit(LOSS_PRECISION, avgSpliceLoss, " dB");
    pdfTableSetText(pTestResultsTable, 2, 3, buff);
    free(buff);
    
    pdfTableSetText(pTestResultsTable, 3, 0, pPdfRptTraceMaxSpliceLoss);
    buff = Float2StringUnit(LOSS_PRECISION, maxSpliceLoss, " dB");
    pdfTableSetText(pTestResultsTable, 3, 1, buff);
    free(buff);
    
    pdfTableSetText(pTestResultsTable, 3, 2, pPdfRptTraceAverageAttenuation);
    buff = Float2StringUnit(ATTENUATION_PRECISION, avgAttenuation, " dB/km");
    pdfTableSetText(pTestResultsTable, 3, 3, buff);
    free(buff);

    float titleOffset = (pdfControl->pageEditWidth - 350) / 2;
    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);
    pdfTextWithUnderlineDraw(pdfControl, titleOffset, pPdfRptTraceParameters, 0.5, font, 7, TEXT_BOLD);
    pdfTableDraw(pdfControl, pParametersTable);

    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);
    pdfTextWithUnderlineDraw(pdfControl, titleOffset, pPdfRptTraceResultThresholds, 0.5, font, 7, TEXT_BOLD);
    pdfTableDraw(pdfControl, pThresholdsTable);

    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);
    pdfTextWithUnderlineDraw(pdfControl, titleOffset, pPdfRptTraceSettings, 0.5, font, 7, TEXT_BOLD);
    pdfTableDraw(pdfControl, pSettingsTable);

    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);
    pdfTextWithUnderlineDraw(pdfControl, titleOffset, pPdfRptTraceSummaryTestResults, 0.5, font, 7, TEXT_BOLD);
    pdfTableDraw(pdfControl, pTestResultsTable);
    pdfSeparatorDraw(pdfControl, 10, 350, 217, 217, 217);

    pdfDrawParagraphSpace(pdfControl);

    pdfTableFree(pParametersTable);
    pdfTableFree(pThresholdsTable);
    pdfTableFree(pSettingsTable);
    pdfTableFree(pTestResultsTable);
    return 0;
}

static char* otdrPdfReportGetCurveTitle(PCURVE_INFO curveInfo)
{
    char *cRet = (char *)malloc(64*sizeof(char));
    switch(curveInfo->pParam.enWave)
    {
        case ENUM_WAVE_1310NM:
            sprintf(cRet, "%s - 1310 nm", pPdfRptOTDRWave);
            break;
        case ENUM_WAVE_1550NM:
            sprintf(cRet, "%s - 1550 nm", pPdfRptOTDRWave);
            break;
        case ENUM_WAVE_1625NM:
            sprintf(cRet, "%s - 1625 nm", pPdfRptOTDRWave);
            break;
    }

    return cRet;
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
static void curveDrawText(PdfPainter painter, char *text, int fontSize,
                          int xOffset, int yOffset, int width, int height, PDF_TEXT_HORIZONTAL_ALIGNMENT alignment)
{
    pdfDrawText(painter.page, xOffset, painter.position + yOffset, width, height,
                fontSize, TEXT_NORMAL, alignment, font, text);
}

/**
* 画OTDR曲线坐标轴
* @param[in] painter
* @param[in] 边距
* @param[in] 可视宽度
* @param[in] 整体高度
* @param[in] x轴范围
* @param[in] y轴范围
* @param[in] 绘制曲线宽度
* @param[in] 绘制曲线宽度
* @param[in] x轴距离单位
* @return
* @note 有关此函数的特别说明：…
*/
static void drawCurveAxisScale(PdfPainter painter, int offset, int viewWidth, int height, float xRange, float yRange,
                               int drawWidth, int drawHeight, char *xUnit)
{
    //<!宽度
    epdf_setrgbstroke(painter.page, COLOR_RGB(0), COLOR_RGB(0), COLOR_RGB(0));
    epdf_setlinewidth(painter.page, 1);

    //画矩形
    pdfDrawLine(painter.page, 0, painter.position + offset, viewWidth, painter.position + offset);
    pdfDrawLine(painter.page, 0, painter.position + offset, 0, painter.position + height - offset);
    pdfDrawLine(painter.page, viewWidth, painter.position + offset, viewWidth, painter.position + height - offset);
    pdfDrawLine(painter.page, 0, painter.position + height - offset, viewWidth, painter.position + height - offset);

    //画y轴坐标轴
    curveDrawLine(painter, 55, offset + 5, 0, drawHeight, 0.2, 0, 0, 0);
    curveDrawText(painter, "dB", 8, 7, 77, 10, 20, PDF_TEXT_ALIGN_LEFT);

    //绘制纵坐标表格线以及刻度值
    int i; 
    //定义行列
    int row = 8;
    int column = 7;

    for (i = 0; i < (5*row+1); i++)
    {
        if (i % 5 == 0)
            curveDrawLine(painter, 50, offset + 5 + i * 4, 5, 0, 0.2, 0, 0, 0);
        else
            curveDrawLine(painter, 52, offset + 5 + i * 4, 3, 0, 0.2, 0, 0, 0);

        if (i % 5 == 0 && i != 5*row)
        {
            curveDrawLine(painter, 55, offset + 5 + i * 4, drawWidth, 0, 1, 215, 235, 255);
        }
    }
    //绘制纵坐标数值
    float yPer = yRange / row;
    for (i = 0; i < (row+1); i++)
    {
        float yVal = yPer * i;
        char yAxis[10] = {0};
        sprintf(yAxis, "%.2f", yVal);
        curveDrawText(painter, yAxis, 8, 20, 165 - i * 20, 30, 10, PDF_TEXT_ALIGN_RIGHT);
    }

    //画x轴坐标
    curveDrawLine(painter, 55, offset + 165, drawWidth, 0, 0.2, 0, 0, 0);
    //绘制表格线以及刻度值
    for (i = 1; i < (5*column+1); i++)
    {
        if (i % 5 == 1)
            curveDrawLine(painter, 55 + i * 10, offset + 165, 0, 5, 0.2, 0, 0, 0);
        else
            curveDrawLine(painter, 55 + i * 10, offset + 165, 0, 3, 0.2, 0, 0, 0);

        if (i % 5 == 1 || i == 5 * column)
        {
            curveDrawLine(painter, 55 + i * 10, offset + 5, 0, drawHeight, 1, 215, 235, 255);
        }
    }
    //绘制横坐标数值
    float xPer = xRange / column;
    for (i = 0; i < column; i++)
    {
        float xVal = xPer * i;
        char *xAxis = UnitConverter_Dist_M2System_Float2String(MODULE_UNIT_OTDR, xVal, 0);
        curveDrawText(painter, xAxis, 8, 49 + i * 50, offset + 170, 50, 10, PDF_TEXT_ALIGN_H_CENTER);
        GuiMemFree(xAxis);
    }
    //绘制单位
    curveDrawText(painter, xUnit, 8, 40 + 7 * 50, offset + 170, 30, 10, PDF_TEXT_ALIGN_H_CENTER);
}

/**
* 画OTDR曲线曲线
* @param[in] painter
* @param[in] 边距
* @param[in] 数据
* @param[in] 个数
* @param[in] 每个db代表的像素值
* @return
* @note 有关此函数的特别说明：…
*/
static void drawCurve(PdfPainter painter, int offset, UINT16 *data, int count, float fValPix)
{
    //画曲线
    int i;
    for (i = 1; i < count; i++)
    {
        int beginYPos = *(data + (i - 1)) * fValPix;
        int endYPos = *(data + i) * fValPix;
        curveDrawLine(painter, 64 + i, (offset + 165 - beginYPos), 1, (beginYPos - endYPos), 0.1, 255, 0, 0);
    }
}

//
//根据显示信息，得到曲线的特征值
static DATA_OUT *GetPixelOutValue(PCURVE_INFO pCurve, int xAxisPix, int yAxisPix, int iDrawHeight)
{
    //初始化结构体
    DATA_OUT* dOut = NULL;
    dOut = (DATA_OUT *)calloc(1, sizeof(DATA_OUT));
    dOut->pValue = (UINT16 *)calloc(MAX_PIXEL, sizeof(UINT16));
    dOut->pIndex = (UINT32 *)calloc(MAX_PIXEL, sizeof(UINT32));
    dOut->uiDrawCnt = (pCurve->pParam.fDistance / 1.0f) / pCurve->pParam.fSmpIntval;
    dOut->iLeftOffset = 0;
    dOut->fDataPix = (float)xAxisPix / (float)dOut->uiDrawCnt;
    dOut->fValPix = (float)yAxisPix / (float)iDrawHeight;
    
    int iDataCnt = pCurve->dIn.uiCnt;
    int iDrawLen = dOut->uiDrawCnt;
    int iEignNum = xAxisPix;
    UINT16 *pPixVal = dOut->pValue;
    UINT32 *pPixIndex = dOut->pIndex;
    UINT16 *pDrawData = pCurve->dIn.pData;

    if (iEignNum >= iDrawLen)
    {
        iEignNum = GetEigenValue(pPixVal, pPixIndex, iEignNum, pDrawData,
                                    iDrawLen, pCurve->dIn.pData + iDataCnt);
    }
    else
    {
        iEignNum = GetEigenValue(pPixVal + 2, pPixIndex + 2, iEignNum, pDrawData,
                                    iDrawLen, pCurve->dIn.pData + iDataCnt);
        pPixVal[0] = pPixVal[2];
        pPixVal[1] = pPixVal[2];
        pPixIndex[0] = pPixIndex[2];
        pPixIndex[1] = pPixIndex[2];
    }

    dOut->iValNum = iEignNum;

    return dOut;
}

/**
* 画OTDR曲线
* @param[in] pdfControl
* @return
* @note 有关此函数的特别说明：…
*/
static int pdfDrawCurve(PdfControl *pdfControl, int curveIndex)
{
    int iErr = 0;

    if (!pdfControl)
    {
        return -1;
    }

    int viewWidth = pdfControl->pageEditWidth;
    PdfPainter painter = pdfGetPainter(pdfControl, 190);
    PDISPLAY_INFO pDisplay = pOtdrTopSettings->pDisplayInfo;
    PCURVE_INFO curveInfo = pDisplay->pCurve[curveIndex];
    float maxX = curveInfo->pParam.fDistance;
    float maxY = (float)MAX_DATA_VALUE / (float)curveInfo->pParam.iFactor;
    int drawWidth = 350;
    int drawHeight = 160;
    //绘制坐标轴（8*7）
    drawCurveAxisScale(painter, 5, viewWidth, 190, maxX, maxY, drawWidth, drawHeight, GetCurrSystemUnitString(MODULE_UNIT_OTDR));
    //重新计算特征值点
    DATA_OUT *dOut = GetPixelOutValue(curveInfo, drawWidth, drawHeight, MAX_DATA_VALUE);
    if (!dOut)
    {
        return -2;
    }
    //绘制曲线
    drawCurve(painter, 5, dOut->pValue, dOut->iValNum, dOut->fValPix); //m
    //绘制事件点
    EVENTS_TABLE Events = curveInfo->Events;
    int eventsNum = Events.iEventsNumber;
    int displaySectionEvent = pOtdrTopSettings->pUser_Setting->sCommonSetting.iDisplaySectionEvent ? 1 : 0;
    int count = 0;
    int i = 0;
    for (i = 0; i < eventsNum; i++)
    {
        EVENTS_INFO eventInfo = Events.EventsInfo[i / (1 + displaySectionEvent)];
        float pos = eventInfo.fEventsPosition;

        if (pos > maxX) //事件点位置超出量程，不显示事件点
            continue;
        char num[10] = {0};
        double lenPix = pos / (maxX)*drawWidth;
        int dbPix = 0.0f;
        int differDbPix = 0.0f;
        int db = round(*(curveInfo->dIn.pData + (int)(pos / curveInfo->pParam.fSmpIntval)) * dOut->fValPix);

        if (dbPix >= 0.1)
            differDbPix = fabsf(dbPix - db);

        if (db > dbPix)
            dbPix = db;

        //计算事件点的标记
        sprintf(num, "%d", count + 1);
        count++;
        (i == 0) ? curveDrawLine(painter, 65 + lenPix, 165 - dbPix, 0, 10 + differDbPix, 0.4, 255, 0, 0)
                 : curveDrawLine(painter, 64 + lenPix, 165 - dbPix, 0, 10 + differDbPix, 0.4, 255, 0, 0);
        curveDrawText(painter, num, 8, 59 + lenPix, 175 - dbPix + differDbPix, 10, 10, PDF_TEXT_ALIGN_H_CENTER);
    }
    //销毁输出信息结构体
    GuiMemFree(dOut);

    return iErr;
}

static int otdrPdfReportCurve(PdfControl *pdfControl, int curveIndex)
{
    if (!pdfControl)
    {
        return -1;
    }
    PCURVE_INFO curveInfo = pOtdrTopSettings->pDisplayInfo->pCurve[curveIndex];
    char *title = otdrPdfReportGetCurveTitle(curveInfo);
    //曲线标题
    pdfSeparatorDraw(pdfControl, 2, pdfControl->pageEditWidth, 13, 93, 192);
    pdfDrawParagraphSpace(pdfControl);
    pdfTitleDraw(pdfControl, 0, title, 2, font, 10, TEXT_BOLD, 13, 93, 192);
    //绘制曲线
    pdfDrawCurve(pdfControl, curveIndex);
    //标记线信息标题
    pdfTitleDraw(pdfControl, 0, pPdfRptMarkLineInformation, 1, font, 10, TEXT_BOLD, 152, 152, 152);
    //标记线信息表格
    otdrPdfReportMarkLineInfo(pdfControl, curveInfo->mMarker, curveInfo->Events.LinkIsPass.iIsRegionORLPass);
    //事件列表标题
    pdfTitleDraw(pdfControl, 0, pPdfRptEventTable, 1, font, 10, TEXT_BOLD, 152, 152, 152);
    //事件列表
    otdrPdfReportEventTable(pdfControl, curveInfo->Events);
    //曲线参数，阈值和设置信息标题
    pdfTitleDraw(pdfControl, 0, pPdfRptTraceParThreAndSet, 1, font, 10, TEXT_BOLD, 152, 152, 152);
    //绘制曲线参数，阈值和设置信息
    otdrPdfReportTraceParaThreAndSetTable(pdfControl, curveInfo, curveInfo->Events.LinkIsPass);

    free(title);
    return 0;
}

//NOTE
static int otdrPdfReportNote(PdfControl *pdfControl)
{
    if (!pdfControl)
    {
        return -1;
    }
    pdfTitleDraw(pdfControl, 0, pPdfRptNote, 1, font, 10, TEXT_BOLD, 152, 152, 152);
    pdfDrawSpace(pdfControl, 2);
    pdfTextWithWriteDraw(pdfControl, 0, pPdfRptName, 0.5, 300, font, 7, TEXT_NORMAL);
    pdfDrawSpace(pdfControl, 7);
    pdfTextWithWriteDraw(pdfControl, 0, pPdfRptDate, 0.5, 300, font, 7, TEXT_NORMAL);
    pdfDrawParagraphSpace(pdfControl);

    return 0;
}

//得到报告绝对路径，包含文件名 传入0返回 绝对路径  传入1 返回文件名
char* otdrPdfReportGetPathOrName(int pathOrName)
{
    char pFileName[F_NAME_MAX] = {0};
    char *pFilePath = pOtdrTopSettings->sDefSavePath.cOtdrReportSavePath;
    char *cRet = NULL;
    
    if(pFilePath[strlen(pFilePath)-1] == '/')
    {
        pFilePath[strlen(pFilePath)-1] = 0; 
    }
    
	//检查存储路径
	isExist(pFilePath);
	
	//创建报告名
	GetCurveFile(0, pOtdrTopSettings->pDisplayInfo, pFileName);
    char *position = strrchr(pFileName, '.');
    
    if(position != NULL)
    {
        if(strstr(pFileName, "_1310") || strstr(pFileName, "_1550") || strstr(pFileName, "_1625"))
        {
            position -= 5;
        }
        
        strcpy(position, ".PDF");
        
        if(!pathOrName)
        {
            cRet = (char *)malloc((strlen(pFilePath)+strlen(pFileName)+5)*sizeof(char));
            sprintf(cRet,"%s/%s",pFilePath,pFileName);
        }
        else
        {
            cRet = (char *)malloc((strlen(pFileName)+5)*sizeof(char));
            sprintf(cRet,"%s",pFileName);
        }
    }

    return cRet;
}

//创建otdr报告
int createOtdrPdfReport(char *pAbsolutePath, unsigned int currLanguage)
{
    int i;
    char temp[F_NAME_MAX] = {0};
    char *pName = strrchr(pAbsolutePath, '/');
    pName++;
    PdfControl *pControl = createPdfControl(pAbsolutePath);

    if(!pControl)
    {
        return -1;
    }
    if (setjmp(env)) 
    {
        epdf_close(pControl->pdfDoc);
        free(pControl);
        return -1;
    }
    font = pdfReportGetLanguage(pControl, currLanguage);//HPDF_GetFont(pControl->pdfDoc, "Helvetica", NULL);
    pdfSetFont(pControl, font);
    LOG(LOG_INFO, "font = %s\n", font);
    InitPdfReportText(currLanguage);

    //设置段间距
    pdfSetParagraphSpace(pControl, 8);

    //设置页边距
    pdfSetPageMarginsWidth(pControl, 90.14);

    //页眉
    getCurveMeasureDate(temp, NULL, pCurSystemSet->uiTimeShowFlag);
    pdfHeaderDraw(0, 80, 30, temp, font, 5.1, TEXT_NORMAL, 0, 0, 0);
    pdfHeaderDraw(0, (pControl->pageWidth / 2 - 12), 30, "OTDR", font, 6, TEXT_NORMAL, 0, 0, 0);
    //otdr report
    pdfTitleDraw(pControl, 0, pPdfRptOtdrReport, 2, font, 12, TEXT_BOLD, 13, 93, 192);
    //summary
    pdfTitleDraw(pControl, 0, pPdfRptSummary, 1, font, 10, TEXT_BOLD, 152, 152, 152);
    //summary table
    otdrPdfReportSummary(pControl, pName);
    //曲线信息
    for(i = 0; i < pOtdrTopSettings->pDisplayInfo->iCurveNum; i++)
    {
        otdrPdfReportCurve(pControl, i);
    }
    //NOTE
    otdrPdfReportNote(pControl);
    //logo
    pdfPictureDraw(pControl, BmpFileDirectory"/pdf_logo.jpg", 291/2, 53/2, 0);
    pdfDrawTheEnd(pControl);
    //删除截取的图片
//     DelPdfJpg();
    FreePdfReportText();

    return 0;
}

//生成报告
int CreateOtdrReport(char* absolutePath)
{
    int iErr = 0;
    if(absolutePath)
    {
        iErr = createOtdrPdfReport(absolutePath, GetCurrLanguageSet());
        if(access(absolutePath, F_OK) != 0)
        {
            iErr = -1;
        }
    }
	return iErr;
}